
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <signal.h>
#include <getopt.h>
#include <string.h>

#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

#include "mod_daq.h"
#include "mod_prt.h"

OPC_UA::TProt *OPC_UA::modPrt;

using namespace OPC_UA;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name )
{
    mId		= PRT_ID;
    mType	= PRT_TYPE;
    mName	= PRT_NAME;
    mVers	= PRT_MVER;
    mAutor	= PRT_AUTOR;
    mDescr	= PRT_DESCR;
    mLicense	= PRT_LICENSE;
    mSource	= name;

    modPrt	= this;
}

TProt::~TProt()
{
    nodeDelAll();
}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	PRT_TYPE,PRT_ID,nodePath().c_str());

    return buf;
}

void TProt::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL, 0 }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TProt::save_( )
{

}

void TProt::modStart( )
{

}

void TProt::modStop( )
{

}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string mbap, err, rez;
    char buf[1000];

    ResAlloc resN( tro.nodeRes(), true );

    //????
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else TProtocol::cntrCmdProc(opt);
}


//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name), mNotFull(false)
{

}

TProtIn::~TProtIn()
{

}

TProt &TProtIn::owner( )	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &reqst, string &answer, const string &sender )
{
    uint32_t mSz;
    bool KeepAlive = false;

    //> Continue for full request
    if( mNotFull )	{ mBuf = mBuf+reqst; mNotFull = false; }
    else mBuf = reqst;  //Save request to bufer

    string &request = mBuf;

    answer = "";
    if( request.size() <= 0 ) return mNotFull;

#if OSC_DEBUG >= 3
    mess_debug(nodePath().c_str(),_("Content:\n%s"),request.c_str());
#endif

    //> Check for hello message type
    if( request.size() > 8 && request.compare(0,4,"HELF") == 0 )
    {
	mSz = *(uint32_t*)(request.data()+4);		//Message size
	if( request.size() < mSz ) return (mNotFull=true);
	//>> Check for minimum request size
	//????
	uint32_t prtVer = *(uint32_t*)(request.data()+8);	//Protocol version
	uint32_t rBufSz = *(uint32_t*)(request.data()+12);	//Recive buffer size
	uint32_t wBufSz = *(uint32_t*)(request.data()+16);	//Send buffer size
	uint32_t mMsgSz = *(uint32_t*)(request.data()+20);	//Max message size
	uint32_t mChnk  = *(uint32_t*)(request.data()+24);	//Max chunk count
	string EndpntURL( (char*)(request.data()+32), *(uint32_t*)(request.data()+28) );	//EndpointURL

	printf( "TEST 00: Hello request: prtVer = %d, rBufSz = %d, wBufSz = %d, mMsgSz = %d, mChnk = %d, EndpntURL = '%s'\n", 
	    prtVer, rBufSz, wBufSz, mMsgSz, mChnk, EndpntURL.c_str() );

	//> Prepare acknowledge message
	mSz = 28;
	answer.reserve(mSz);
	answer.append("ACKF");				//Acknowledge message type
	answer.append((char*)&mSz,sizeof(mSz));		//Message size
	answer.append((char*)&prtVer,sizeof(prtVer));	//Protocol version
	answer.append((char*)&rBufSz,sizeof(rBufSz));	//Recive buffer size
	answer.append((char*)&wBufSz,sizeof(wBufSz));	//Send buffer size
	answer.append((char*)&mMsgSz,sizeof(mMsgSz));	//Max message size
	answer.append((char*)&mChnk,sizeof(mChnk));	//Max chunk count
	return mNotFull;
    }
    /*if( request.size() > 8 && request.compare(0,4,"OPNF") == 0 )
    {
	mSz = *(uint32_t*)(request.data()+4);		//Message size
	if( request.size() < mSz ) return (mNotFull=true);
	uint32_t secChnId = *(uint32_t*)(request.data()+8);	//Secure channel identifier
	string secPlcURI( (char*)(request.data()+16), *(uint32_t*)(request.data()+12) );	//Security policy URI

    }*/

    //> Post error for unrecognized request
    printf("TEST 01: Unsupported request:\n%s\n",TSYS::strDecode(request,TSYS::Bin).c_str());

    uint32_t errCode = 1;
    string errMess = _("Request message isn't recognize.");
    mSz = 16*errMess.size();
    answer.reserve(mSz);
    answer.append("ERRF");				//Error message type
    answer.append((char*)&mSz,sizeof(mSz));		//Message size
    answer.append((char*)&errCode,sizeof(errCode));	//Error code
    mSz = errMess.size();
    answer.append((char*)&mSz,sizeof(mSz));		//Error message
    answer.append(errMess);

    return false;
}


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
    const char *rd = reqst.data();
    const char *re = rd+reqst.size();

    //> Continue for full request
    if( mNotFull )	{ mBuf = mBuf+reqst; mNotFull = false; }
    else mBuf = reqst;  //Save request to bufer

    string &request = mBuf;

    answer = "";
    if( request.size() <= 0 ) return mNotFull;

#if OSC_DEBUG >= 3
    mess_debug(nodePath().c_str(),_("Content:\n%s"),request.c_str());
#endif

    //printf("TEST 00: Request:\n%s\n",TSYS::strDecode(request,TSYS::Bin).c_str());

    //> Check for hello message type
    if( (re-rd) > 8 && strncmp(rd,"HELF",4) == 0 )
    {
	rd += 4;
	mSz = *(uint32_t*)rd; rd += 4;		//Message size
	if( (re-rd+8) < mSz ) return (mNotFull=true);
	uint32_t prtVer = *(uint32_t*)rd; rd += 4;	//Protocol version
	uint32_t rBufSz = *(uint32_t*)rd; rd += 4;	//Recive buffer size
	uint32_t wBufSz = *(uint32_t*)rd; rd += 4;	//Send buffer size
	uint32_t mMsgSz = *(uint32_t*)rd; rd += 4;	//Max message size
	uint32_t mChnk  = *(uint32_t*)rd; rd += 4;	//Max chunk count
	mSz = vmax(0,*(int32_t*)rd); rd += 4;
	string EndpntURL( rd, mSz ); rd += mSz;		//EndpointURL

	printf( "TEST 00: Hello request: prtVer = %d, rBufSz = %d, wBufSz = %d, mMsgSz = %d, mChnk = %d, EndpntURL = '%s'\n", 
	    prtVer, rBufSz, wBufSz, mMsgSz, mChnk, EndpntURL.c_str() );

	//> Prepare acknowledge message
	mSz = 28;
	answer.reserve( mSz );
	answer.append( "ACKF" );			//Acknowledge message type
	answer.append( (char*)&mSz, sizeof(mSz) );	//Message size
	answer.append( (char*)&prtVer, sizeof(prtVer) );//Protocol version
	answer.append( (char*)&rBufSz, sizeof(rBufSz) );//Recive buffer size
	answer.append( (char*)&wBufSz, sizeof(wBufSz) );//Send buffer size
	answer.append( (char*)&mMsgSz, sizeof(mMsgSz) );//Max message size
	answer.append( (char*)&mChnk, sizeof(mChnk) );	//Max chunk count

	return mNotFull;
    }
    //> Check for Open SecureChannel message type
    if( (re-rd) > 8 && strncmp(rd,"OPNF",4) == 0 )
    {
	rd += 4;
	mSz = *(uint32_t*)rd; rd += 4;				//Message size
	if( (re-rd+8) < mSz ) return (mNotFull=true);
	uint32_t secChnId = *(uint32_t*)rd; rd += 4;		//Secure channel identifier

								//> Security Header
	mSz = vmax(0,*(int32_t*)rd); rd += 4;
	string secPlcURI( rd, mSz ); rd += mSz;			//Security policy URI
	if( secPlcURI == "http://opcfoundation.org/UA/SecurityPolicy#None" )
	{
	    int32_t senderCertLength = *(int32_t*)rd; rd += 4;	//SenderCertificateLength
	    int32_t recCertThbLength = *(int32_t*)rd; rd += 4;	//ReceiverCertificateThumbprintLength
								//> Sequence header
	    uint32_t secNumb = *(uint32_t*)rd; rd += 4;		//Sequence number
	    uint32_t reqId = *(uint32_t*)rd; rd += 4;		//RequestId
								//> Extension body object
	    uint16_t eoTpId = *(uint16_t*)(rd+2); rd += 4;	//TypeId
								//>> Request Header
	    rd += 2;						//Session AuthenticationToken
	    int64_t tmStamp = *(int64_t*)rd; rd += 8;		//timestamp
	    int32_t rqHndl = *(int32_t*)rd; rd += 4;		//requestHandle
	    uint32_t retDgn = *(uint32_t*)rd; rd += 4;		//returnDiagnostics
	    mSz = vmax(0,*(int32_t*)rd); rd += 4;
	    string secPlcURI( rd, mSz ); rd += mSz;		//auditEntryId
	    uint32_t tmHnt = *(uint32_t*)rd; rd += 4;		//timeoutHint
	    rd += 3;						//Extensible parameter.NodeId (0)

	    uint32_t clntPrtVer = *(uint32_t*)rd; rd += 4;	//ClientProtocolVersion
	    uint32_t reqTp = *(uint32_t*)rd; rd += 4;		//RequestType
	    uint32_t secMode = *(uint32_t*)rd; rd += 4;		//SecurityMode
	    mSz = vmax(0,*(int32_t*)rd); rd += 4;
	    string clntNonce( rd, mSz ); rd += mSz;		//ClientNonce
	    int32_t reqLifeTm = *(int32_t*)rd; rd += 4;		//RequestedLifetime

	    //> Prepare respond message
	    
	}
    }

    //> Post error for unrecognized request
    printf("TEST 01: Unsupported request:\n%s\n",TSYS::strDecode(request,TSYS::Bin).c_str());

    answer = mkError( 1, _("Request message isn't recognize.") );

    return false;
}

string TProtIn::mkError( uint32_t errId, const string &err )
{
    string rez;

    uint32_t mSz = 16*err.size();
    rez.reserve(mSz);
    rez.append("ERRF");				//Error message type
    rez.append((char*)&mSz,sizeof(mSz));	//Message size
    rez.append((char*)&errId,sizeof(errId));	//Error code
    mSz = err.size();
    rez.append((char*)&mSz,sizeof(mSz));	//Error message
    rez.append(err);
}

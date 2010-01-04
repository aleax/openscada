
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

int32_t TProt::iN( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    switch( vSz )
    {
	case 1:	return *(int8_t*)(rb.data()+off-vSz);
	case 2:	return *(int16_t*)(rb.data()+off-vSz);
	case 4:	return *(int32_t*)(rb.data()+off-vSz);
    }
    throw TError(modPrt->nodePath().c_str(),_("Number size '%d' is error."),vSz);
}

uint32_t TProt::iNu( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    switch( vSz )
    {
	case 1:	return *(uint8_t*)(rb.data()+off-vSz);
	case 2:	return *(uint16_t*)(rb.data()+off-vSz);
	case 4:	return *(uint32_t*)(rb.data()+off-vSz);
    }
    throw TError(modPrt->nodePath().c_str(),_("Number size '%d' is error."),vSz);
}

const char *TProt::iVal( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    return rb.data()+off-vSz;
}

string TProt::iS( const string &rb, int &off )
{
    int sSz = iN(rb,off,4); sSz = vmax(0,sSz);
    off += sSz;
    if( off > rb.size() ) throw TError(modPrt->nodePath().c_str(),_("Buffer size is less for requested string."));
    return rb.substr(off-sSz,sSz);
}

long long TProt::iTm( const string &rb, int &off )
{
    int64_t tmStamp = *(int64_t*)TProt::iVal(rb,off,8);
    return (tmStamp/10ll)-11644473600000000ll;
}

int TProt::iNodeId( const string &rb, int &off, int *ns )
{
    off += 1;
    if( off > rb.size() ) throw TError(modPrt->nodePath().c_str(),_("Buffer size is less for requested NodeId."));
    char enc = rb[off-1];
    switch( enc )
    {
	case 0x00:
	    if( ns ) *ns = 0;
	    return iNu(rb,off,1);
	case 0x01:
	    off += 1;
	    if( off > rb.size() ) throw TError(modPrt->nodePath().c_str(),_("Buffer size is less for requested NodeId."));
	    if( ns ) *ns = (uint8_t)rb[off-1];
	    return iNu(rb,off,2);
    }
    throw TError(modPrt->nodePath().c_str(),_("NodeId type %d is error or not support."),enc);
}

void TProt::oN( string &buf, int32_t val, char sz, int off )
{
    if( off < 0 || (off+sz) > buf.size() ) buf.append( (char*)&val, sz );
    else buf.replace( off, sz, (char*)&val, sz );
}

void TProt::oNu( string &buf, uint32_t val, char sz, int off )
{
    if( off < 0 || (off+sz) > buf.size() ) buf.append( (char*)&val, sz );
    else buf.replace( off, sz, (char*)&val, sz );
}

void TProt::oS( string &buf, const string &val )
{
    oN(buf,(val.size()?val.size():-1),4);
    buf.append(val);
}

void TProt::oNodeId( string &buf, int val, int ns )
{
    if( !ns && val <= 255 )
    {
	buf += (char)0x00;
	buf += (char)val;
    }
    else
    {
	buf += (char)0x01;
	buf += (char)ns;
	buf.append( (char*)&val, 2 );
    }
}

void TProt::oTm( string &buf, long long val )
{
    int64_t tmStamp = 10ll*(val+11644473600000000ll);
    buf.append( (char*)&tmStamp, sizeof(tmStamp) );
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
    int off = 0;

    //> Continue for full request
    if( mNotFull )	{ mBuf = mBuf+reqst; mNotFull = false; }
    else mBuf = reqst;  //Save request to bufer

    string &rb = mBuf;

    answer = "";
    if( rb.size() <= 0 ) return mNotFull;

#if OSC_DEBUG >= 3
    mess_debug(nodePath().c_str(),_("Content:\n%s"),rb.c_str());
#endif

    //printf("TEST 00: Request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

    //> Check for hello message type
    if( rb.size() > 8 && rb.compare(0,4,"HELF") == 0 )
    {
	off += 4;
	mSz = TProt::iNu(rb,off,4);	//Message size
	if( rb.size() < mSz ) return (mNotFull=true);
	uint32_t prtVer = TProt::iNu(rb,off,4);	//Protocol version
	uint32_t rBufSz = TProt::iNu(rb,off,4);	//Recive buffer size
	uint32_t wBufSz = TProt::iNu(rb,off,4);	//Send buffer size
	uint32_t mMsgSz = TProt::iNu(rb,off,4);	//Max message size
	uint32_t mChnk  = TProt::iNu(rb,off,4);	//Max chunk count
	string EndpntURL = TProt::iS(rb,off);	//EndpointURL

	printf( "TEST 00: Hello request: prtVer = %d, rBufSz = %d, wBufSz = %d, mMsgSz = %d, mChnk = %d, EndpntURL = '%s'\n", 
	    prtVer, rBufSz, wBufSz, mMsgSz, mChnk, EndpntURL.c_str() );

	//> Prepare acknowledge message
	mSz = 28;
	answer.reserve( mSz );
	answer.append( "ACKF" );	//Acknowledge message type
	TProt::oNu(answer,mSz,4);	//Message size
	TProt::oNu(answer,prtVer,4);	//Protocol version
	TProt::oNu(answer,rBufSz,4);	//Recive buffer size
	TProt::oNu(answer,wBufSz,4);	//Send buffer size
	TProt::oNu(answer,mMsgSz,4);	//Max message size
	TProt::oNu(answer,mChnk,4);	//Max chunk count

	return mNotFull;
    }
    //> Check for Open SecureChannel message type
    if( rb.size() > 8 && rb.compare(0,4,"OPNF") == 0 )
    {
	off += 4;
	mSz = TProt::iNu(rb,off,4);			//Message size
	if( rb.size() < mSz ) return (mNotFull=true);
	uint32_t secChnId = TProt::iNu(rb,off,4);		//Secure channel identifier

								//> Security Header
	string secPlcURI = TProt::iS(rb,off);			//Security policy URI
	if( secPlcURI == "http://opcfoundation.org/UA/SecurityPolicy#None" )
	{
	    int32_t senderCertLength = TProt::iN(rb,off,4);	//SenderCertificateLength
	    int32_t recCertThbLength = TProt::iN(rb,off,4);	//ReceiverCertificateThumbprintLength
								//> Sequence header
	    uint32_t secNumb = TProt::iNu(rb,off,4);		//Sequence number
	    uint32_t reqId = TProt::iNu(rb,off,4);		//RequestId
								//> Extension body object
	    uint16_t eoTpId = TProt::iNodeId(rb,off);		//TypeId
								//>> Request Header
	    TProt::iVal(rb,off,2);				//Session AuthenticationToken
	    long long tmStamp = TProt::iTm(rb,off);		//timestamp
	    int32_t rqHndl = TProt::iN(rb,off,4);		//requestHandle
	    uint32_t retDgn = TProt::iNu(rb,off,4);		//returnDiagnostics
	    string AuditEntrId = TProt::iS(rb,off);		//auditEntryId
	    uint32_t tmHnt = TProt::iNu(rb,off,4);		//timeoutHint
								//>>> Extensible parameter
	    TProt::iNodeId(rb,off);				//TypeId (0)
	    TProt::iNu(rb,off,1);				//Encoding
								//>>>> Standard request
	    uint32_t clntPrtVer = TProt::iNu(rb,off,4);		//ClientProtocolVersion
	    uint32_t reqTp = TProt::iNu(rb,off,4);		//RequestType
	    uint32_t secMode = TProt::iNu(rb,off,4);		//SecurityMode
	    string clntNonce = TProt::iS(rb,off);		//ClientNonce
	    int32_t reqLifeTm = TProt::iN(rb,off,4);		//RequestedLifetime

	    printf( "TEST 00: Open SecureChannel request: prtVer = '%s'\n", secPlcURI.c_str() );

	    //> Prepare respond message
	    answer.reserve( 200 );
	    answer.append( "OPNF" );				//OpenSecureChannel message type
	    TProt::oNu(answer,0,4);				//Message size
	    TProt::oNu(answer,4,4);				//Secure channel identifier
	    TProt::oS(answer,secPlcURI);			//Security policy URI
	    TProt::oN(answer,senderCertLength,4);		//SenderCertificateLength
	    TProt::oN(answer,recCertThbLength,4);		//ReceiverCertificateThumbprintLength
	    TProt::oNu(answer,secNumb,4);			//Sequence number
	    TProt::oNu(answer,reqId,4);				//RequestId
								//> Extension Object
	    TProt::oNodeId(answer,449);				//TypeId (449 - NodeId)
								//>> Body
								//>>> RespondHeader
	    TProt::oTm(answer,TSYS::curTime());			//timestamp
	    TProt::oN(answer,rqHndl,4);				//requestHandle
	    TProt::oN(answer,0,4);				//StatusCode
	    TProt::oN(answer,0,1);				//serviceDiagnostics
	    TProt::oS(answer,"");				//stringTable
								//>>> Extensible parameter
	    TProt::oNodeId(answer,0);				//TypeId (0)
	    TProt::oNu(answer,0,1);				//Encoding
								//>>>> Standard respond
	    TProt::oNu(answer,0,4);				//ServerProtocolVersion
	    TProt::oNu(answer,4,4);				//Secure channel identifier
	    TProt::oNu(answer,1,4);				//TokenId
	    TProt::oTm(answer,TSYS::curTime());			//CreatedAt
	    TProt::oN(answer,600000,4);				//RevisedLifeTime (600000)
	    TProt::oS(answer,"\001");				//nonce
	    TProt::oNu(answer,answer.size(),4,4);		//Real message size

	    return mNotFull;
	}
    }

    //> Post error for unrecognized request
    printf("TEST 01: Unsupported request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

    answer = mkError( 1, _("Request message isn't recognize.") );

    return false;
}

string TProtIn::mkError( uint32_t errId, const string &err )
{
    string rez;
    rez.reserve(100);
    rez.append("ERRF");				//Error message type
    TProt::oNu(rez,16+err.size(),4);		//Message size
    TProt::oNu(rez,errId,4);			//Error code
    TProt::oS(rez,err); 			//Error message

    return rez;
}

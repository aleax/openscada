
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.cpp
/***************************************************************************
 *   Copyright (C) 2009-2010 by Roman Savochenko                           *
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
#include <sys/stat.h>
#include <fcntl.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/aes.h"
#include "openssl/err.h"
#include <openssl/rand.h>
#include "openssl/hmac.h"
#include "openssl/evp.h"

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
TProt::TProt( string name ) : TProtocol(PRT_ID), mSecCnlIdLast(1)
{
    modPrt	= this;

    mType	= PRT_TYPE;
    mName	= PRT_NAME;
    mVers	= PRT_MVER;
    mAutor	= PRT_AUTOR;
    mDescr	= PRT_DESCR;
    mLicense	= PRT_LICENSE;
    mSource	= name;

    mEndPnt = grpAdd("ep_");

    //> Node DB structure
    mEndPntEl.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"20") );
    mEndPntEl.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    mEndPntEl.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300") );
    mEndPntEl.fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0") );
    mEndPntEl.fldAdd( new TFld("SerialzType",_("Serializer type"),TFld::Integer,TFld::Selected,"1","0","0",_("Binary")) );
    mEndPntEl.fldAdd( new TFld("URL",_("URL"),TFld::String,0,"50","opc.tcp://localhost:4841") );
    mEndPntEl.fldAdd( new TFld("SecPolicies",_("Security policies"),TFld::String,TFld::FullText,"100","None:0\nBasic128Rsa15:1") );
    mEndPntEl.fldAdd( new TFld("ServCert",_("Server certificate (PEM)"),TFld::String,TFld::FullText,"10000") );
    mEndPntEl.fldAdd( new TFld("ServPvKey",_("Server private key (PEM)"),TFld::String,TFld::FullText,"10000") );
}

TProt::~TProt()
{
    nodeDelAll();
}

void TProt::epAdd( const string &iid, const string &db )
{
    if( chldPresent(mEndPnt,iid) ) return;
    chldAdd( mEndPnt, new OPCEndPoint(iid,db,&endPntEl()) );
}

void TProt::discoveryUrls( vector<string> &ls )
{
    ls.clear();
    //>> Get allowed enpoints list
    vector<string> epLs;
    epList(epLs);
    for( int i_ep = 0; i_ep < epLs.size(); i_ep++ )
    {
	AutoHD<OPCEndPoint> ep = epAt(epLs[i_ep]);
	if( !ep.at().enableStat() ) continue;
	ls.push_back(ep.at().url());
	break;
    }
}

void TProt::load_( )
{
    //> Load DB
    try
    {
	TConfig g_cfg(&endPntEl());
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;

	//>> Search into DB
	SYS->db().at().dbList(db_ls,true);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    for( int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+modId()+"_ep","",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if( !epPresent(id) )	epAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }

	//>> Search into config file
	if( SYS->chkSelDB("<cfg>") )
	    for( int fld_cnt=0; SYS->db().at().dataSeek("",nodePath()+modId()+"_ep",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if( !epPresent(id) )	epAdd(id,"*.*");
	    }
    }catch(TError err)
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new server error."));
    }
}

void TProt::save_( )
{

}

void TProt::modStart( )
{
    vector<string> ls;
    epList(ls);
    for( int i_n = 0; i_n < ls.size(); i_n++ )
	if( epAt(ls[i_n]).at().toEnable( ) )
	    epAt(ls[i_n]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    epList(ls);
    for( int i_n = 0; i_n < ls.size(); i_n++ )
	epAt(ls[i_n]).at().setEnable(false);
}

int TProt::chnlOpen( const string &iEp, int32_t lifeTm, const string& iClCert, const string &iSecPolicy, char iSecMessMode )
{
    ResAlloc res( nodeRes(), true );
    do
    {
	if( !(++mSecCnlIdLast) ) mSecCnlIdLast = 2;
    } while( mSecCnl.find(mSecCnlIdLast) != mSecCnl.end() );

    mSecCnl[mSecCnlIdLast] = SecCnl(iEp,1,lifeTm,iClCert,iSecPolicy,iSecMessMode);
    return mSecCnlIdLast;
}

void TProt::chnlClose( int cid )
{
    ResAlloc res( nodeRes(), true );
    mSecCnl.erase( cid );
}

SecCnl TProt::chnlGet( int cid )
{
    ResAlloc res( nodeRes(), false );
    if( mSecCnl.find(cid) == mSecCnl.end() ) return SecCnl();
    return mSecCnl[cid];
}

void TProt::chnlSecSet( int cid, const string &iServKey, const string &iClKey )
{
    ResAlloc res( nodeRes(), true );
    if( mSecCnl.find(cid) == mSecCnl.end() ) return;
    mSecCnl[cid].servKey = iServKey;
    mSecCnl[cid].clKey = iClKey;
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string rez, err;
    char buf[1000];

    ResAlloc resN( tro.nodeRes(), true );

    try
    {
	if( io.name() == "opc.tcp" )
	{
	    if( io.attr("id") == "HEL" )
	    {
		rez.reserve(50);
		rez.append("HELF");			//> HELLO message type
		oNu(rez,0,4);				//Message size
		oNu(rez,OpcUa_ProtocolVersion,4);	//Protocol version
		oNu(rez,OpcUa_ReciveBufferSize,4);	//Recive buffer size
		oNu(rez,OpcUa_SendBufferSize,4);	//Send buffer size
		oNu(rez,OpcUa_MaxMessageSize,4);	//Max message size
		oNu(rez,OpcUa_MaxChunkCount,4);		//Max chunk count
		oS(rez,io.attr("EndPoint"));		//EndpointURL
		oNu(rez,rez.size(),4,4);		//Real message size

#if OSC_DEBUG >= 5
		printf("TEST 10: HELLO request:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif

		//> Send request
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );

#if OSC_DEBUG >= 5
		printf("TEST 10a: HELLO response:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif

		int off = 4;
		if( rez.size() < 8 || rez.size() > 4096 || iNu(rez,off,4) != rez.size() )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTooLarge,_("Respond size is not coincedence."));
		else if( rez.compare(0,4,"ERRF") == 0 ) err = iErr(rez,off);
		else if( rez.compare(0,4,"ACKF") != 0 )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTypeInvalid,_("Respond don't acknowledge."));
		else
		{
		    iNu(rez,off,4);			//Protocol version
		    iNu(rez,off,4);			//Recive buffer size
		    iNu(rez,off,4);			//Send buffer size
		    iNu(rez,off,4);			//Max message size
		    iNu(rez,off,4);			//Max chunk count
		}
	    }
	    else if( io.attr("id") == "OPN" )
	    {
		bool isSecNone = false;
		int symKeySz = 0, asymKeyPad = 0;
		string secPlc = io.attr("SecPolicy");
		if( secPlc == "None" )			isSecNone = true;
		else if( secPlc == "Basic128Rsa15" )	{ symKeySz = 16; asymKeyPad = 11; }
		else if( secPlc == "Basic256" )		{ symKeySz = 32; asymKeyPad = 42; }
		else throw TError(OpcUa_BadSecurityPolicyRejected,"","");

		rez.reserve( 200 );
		rez.append( "OPNF" );			//OpenSecureChannel message type
		oNu(rez,0,4);				//Message size
		oNu(rez,strtoul(io.attr("SecChnId").c_str(),NULL,10),4);	//Secure channel identifier
							//> Security Header
		oS(rez,"http://opcfoundation.org/UA/SecurityPolicy#"+secPlc);	//Security policy URI
		oS(rez,certPEM2DER(io.attr("ClntCert")));	//ClientCertificate
		oS(rez,certThumbprint(io.attr("ServCert")));	//ServerCertificateThumbprint
		uint32_t begEncBlck = rez.size();
							//> Sequence header
		oNu(rez,strtoul(io.attr("SeqNumber").c_str(),NULL,10),4);	//Sequence number
		oNu(rez,strtoul(io.attr("SeqReqId").c_str(),NULL,10),4);	//RequestId
							//> Extension body object
		oNodeId(rez,NodeId(OpcUa_OpenSecureChannelRequest));	//TypeId
							//>> Request Header
		oNodeId(rez,0);				//Session AuthenticationToken
		oTm(rez,TSYS::curTime());		//timestamp
		oN(rez,atoi(io.attr("ReqHandle").c_str()),4);	//requestHandle
		oNu(rez,0,4);				//returnDiagnostics
		oS(rez,"");				//auditEntryId
		oNu(rez,0,4);				//timeoutHint
							//>>> Extensible parameter
		oNodeId(rez,0);				//TypeId (0)
		oNu(rez,0,1);				//Encoding
							//>>>> Standard request
		oNu(rez,OpcUa_ProtocolVersion,4);	//ClientProtocolVersion
		oNu(rez,0,4);				//RequestType
		oN(rez,atoi(io.attr("SecurityMode").c_str()),4);	//SecurityMode
		string clNonce = isSecNone?string("\000"):randBytes(symKeySz);
		oS(rez,clNonce);			//ClientNonce
		oN(rez,atoi(io.attr("SecLifeTm").c_str()),4);	//RequestedLifetime
		oNu(rez,rez.size(),4,4);		//> Real message size

		if( !isSecNone )
		{
		    //> Padding place
		    int kSz = asymmetricKeyLength(io.attr("ClntCert"));
		    int paddingSize = ((rez.size()-begEncBlck+1+kSz+(kSz-asymKeyPad)-1)/(kSz-asymKeyPad))*(kSz-asymKeyPad)-(rez.size()+kSz-begEncBlck);
		    rez += string(paddingSize,(char)(paddingSize-1));
		    //> Real message size calc and place
		    oNu( rez, begEncBlck + kSz*((rez.size()-begEncBlck+kSz)/(kSz-asymKeyPad)), 4, 4 );
		    //> Signature
		    rez += asymmetricSign( rez, io.attr("PvKey") );
		    //> Encoding
#if OSC_DEBUG >= 5
		    printf("TEST 11a: OPN request, decoded:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif
		    rez.replace(begEncBlck,rez.size()-begEncBlck,asymmetricEncrypt(rez.substr(begEncBlck),io.attr("ServCert"),secPlc));
		}
#if OSC_DEBUG >= 5
		printf("TEST 11: OPN request:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif
		//> Send request and wait respond
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );
		int off = 4;
		for( ; rez.size() < 8 || rez.size() < iNu(rez,off,4); off = 4 )
		{
		    resp_len = tro.messIO( NULL, 0, buf, sizeof(buf), 0, true );
		    rez.append( buf, resp_len );
		}

		off = 4;
		if( rez.size() < 8 || iNu(rez,off,4) != rez.size() )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTooLarge,_("Respond size is not coincedence."));
		else if( rez.compare(0,4,"ERRF") == 0 ) err = iErr(rez,off);
		else if( rez.compare(0,4,"OPNF") != 0 )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTypeInvalid,_("Respond don't acknowledge."));
		else
		{
#if OSC_DEBUG >= 5
		    printf("TEST 11a: OPN response:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif
		    iNu(rez,off,4);					//Secure channel identifier
		    iS(rez,off);					//Security policy URI
		    string servCert = iS(rez,off);			//ServerCertificate
		    string clntCertThmb = iS(rez,off);			//ClientCertificateThumbprint

		    if( !isSecNone )
		    {
			if( clntCertThmb != TProt::certThumbprint(io.attr("ClntCert")) )
			    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Client certificate thumbprint error.") );
			//> Decoding
			rez.replace(off,rez.size()-off,asymmetricDecrypt(rez.substr(off),io.attr("PvKey"),secPlc));
		    }

		    iNu(rez,off,4);					//Sequence number
		    iNu(rez,off,4);					//RequestId
									//> Extension Object
		    if( iNodeId(rez,off).numbVal() != OpcUa_OpenSecureChannelResponse )	//TypeId
			throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC_UA Bin", _("Respond's NodeId don't acknowledge") );
									//>> Body
									//>>> RespondHeader
		    iTm(rez,off);					//timestamp
		    iN(rez,off,4);					//requestHandle
		    iN(rez,off,4);					//StatusCode
		    iN(rez,off,1);					//serviceDiagnostics
		    iS(rez,off);					//stringTable
									//>>> Extensible parameter
		    iNodeId(rez,off);					//TypeId (0)
		    iNu(rez,off,1);					//Encoding
									//>>>> Standard respond
		    iNu(rez,off,4);					//ServerProtocolVersion
		    io.setAttr("SecChnId",TSYS::uint2str(iNu(rez,off,4)));	//Secure channel identifier
		    io.setAttr("SecTokenId",TSYS::uint2str(iNu(rez,off,4)));	//TokenId
		    iTm(rez,off);					//CreatedAt
		    io.setAttr("SecLifeTm",TSYS::int2str(iN(rez,off,4)));	//RevisedLifeTime
		    string servNonce = iS(rez,off);			//nonce
		    //> Signature
		    if( !isSecNone )
		    {
			io.setAttr("clKey",deriveKey(clNonce,servNonce,symKeySz*3));
			io.setAttr("servKey",deriveKey(servNonce,clNonce,symKeySz*3));
			off += iNu(rez,off,1);				//Pass padding
			if( !asymmetricVerify(rez.substr(0,off),rez.substr(off),io.attr("ServCert")) )	//Check Signature
			    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Signature error") );
		    }
		}
	    }
	    else if( io.attr("id") == "CLO" )
	    {
		rez.reserve( 200 );
		rez.append( "CLOF" );					//OpenSecureChannel close
		oNu(rez,0,4);						//Message size
		oNu(rez,strtoul(io.attr("SecChnId").c_str(),NULL,10),4);	//Secure channel identifier
		oNu(rez,strtoul(io.attr("SecTokenId").c_str(),NULL,10),4);	//TokenId
		int begEncBlck = rez.size();
									//> Sequence header
		uint32_t SeqNumber = strtoul(io.attr("SeqNumber").c_str(),NULL,10)+1;
		uint32_t SeqReqId = strtoul(io.attr("SeqReqId").c_str(),NULL,10)+1;
		oNu(rez,SeqNumber,4);					//Sequence number
		oNu(rez,SeqReqId,4);					//RequestId
									//> Extension body object
		oNodeId(rez,OpcUa_CloseSecureChannelRequest);		//TypeId request: CloseSecureChannel
									//>> Request Header
		oNodeId(rez,0);						//Session AuthenticationToken
		oTm(rez,TSYS::curTime());				//timestamp
		oN(rez,0,4);						//requestHandle
		oNu(rez,0,4);						//returnDiagnostics
		oS(rez,"");						//auditEntryId
		oNu(rez,0,4);						//timeoutHint
									//>>> Extensible parameter
		oNodeId(rez,0);						//TypeId (0)
		oNu(rez,0,1);						//Encoding
		oNu(rez,rez.size(),4,4);				//> Real message size
		string secPolicy = io.attr("SecPolicy");
		char secMessMode = atoi(io.attr("SecurityMode").c_str());
		if( secMessMode == OPCEndPoint::Sign || secMessMode == OPCEndPoint::SignAndEncrypt )
		{
		    string servKey = io.attr("servKey");
		    //> Padding place
		    if( secMessMode == OPCEndPoint::SignAndEncrypt )
		    {
			int kSz = servKey.size()/3;
			int paddingSize = ((rez.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(rez.size()-begEncBlck+20);
			rez += string(paddingSize,(char)(paddingSize-1));
		    }
		    //Real message size calc and place
		    oNu( rez, rez.size()+20, 4, 4 );
		    //> Signature
		    rez += symmetricSign(rez,servKey,secPolicy);
		    //> Encoding
		    if( secMessMode == OPCEndPoint::SignAndEncrypt )
			rez.replace(begEncBlck,rez.size()-begEncBlck,symmetricEncrypt(rez.substr(begEncBlck),servKey,secPolicy));
		}

		//> Parameters clear
		io.setAttr("SecChnId",""); io.setAttr("SecTokenId","");
		io.setAttr("SeqNumber",""); io.setAttr("SeqReqId","");

#if OSC_DEBUG >= 5
		printf("TEST 12: CLO request:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif
		//> Send request and don't wait response
		tro.messIO( rez.data(), rez.size(), NULL, 0, 0, true );
	    }
	    else
	    {
		int iTpId = 0;
		string mReq;
		if( io.attr("id") == "FindServers" )
		{
		    iTpId = OpcUa_FindServersRequest;
		    oS(mReq,io.attr("EndPoint"));			//endpointUrl
		    oS(mReq,"");					//localeIds []
		    oS(mReq,"");					//serverUris []
		}
		else if( io.attr("id") == "GetEndpoints" )
		{
		    iTpId = OpcUa_GetEndpointsRequest;
		    oS(mReq,io.attr("EndPoint"));			//endpointUrl
		    oS(mReq,"");					//localeIds []
		    oS(mReq,"");					//profileUris []
		}
		else if( io.attr("id") == "CreateSession" )
		{
		    iTpId = OpcUa_CreateSessionRequest;
									//> clientDescription (Application Description)
		    oS(mReq,"");					//applicationUri
		    oS(mReq,"");					//productUri
		    oSl(mReq,"");					//applicationName
		    oNu(mReq,1,4);					//applicationType (CLIENT_1)
		    oS(mReq,"");					//gatewayServerUri
		    oS(mReq,"");					//discoveryProfileUri
		    oS(mReq,"");					//discoveryUrls

		    oS(mReq,"");					//serverUri
		    oS(mReq,io.attr("EndPoint"));			//endpointUrl
		    oS(mReq,"OpenSCADA station "+SYS->id());		//sessionName
		    io.setAttr("Nonce",randBytes(32));			//???? check for policy
		    oS(mReq,io.attr("Nonce"));				//clientNonce
		    oS(mReq,certPEM2DER(io.childGet("ClientCert")->text()));	//clientCertificate
		    oR(mReq,atof(io.attr("sesTm").c_str()),8);		//Requested SessionTimeout, ms
		    oNu(mReq,0x1000000,4);				//maxResponse MessageSize
		}
		else if( io.attr("id") == "ActivateSession" )
		{
		    iTpId = OpcUa_ActivateSessionRequest;
									//> clientSignature
		    if( io.attr("SecPolicy") == "None" )
		    {
			oS(mReq,"");					//algorithm
			oS(mReq,"");					//signature
		    }
		    else
		    {
			oS(mReq,"http://www.w3.org/2000/09/xmldsig#rsa-sha1");	//algorithm
			oS(mReq,asymmetricSign(certPEM2DER(io.attr("ServCert"))+io.attr("servNonce"),io.attr("PvKey")));	//signature
		    }

		    oNu(mReq,0,4);					//clientSoftwareCertificates []
									//> localeIds []
		    oNu(mReq,1,4);					//List number 1
		    oS(mReq,"en");					//localeId
									//> userIdentityToken
		    oNodeId(mReq,321);					//TypeId
		    oNu(mReq,1,1);					//Encode
		    oNu(mReq,4,4);					//Length
		    oS(mReq,"");					//policyId
									//> userTokenSignature
		    oS(mReq,"");					//signature
		    oS(mReq,"");					//algorithm
		}
		else if( io.attr("id") == "CloseSession" )
		{
		    iTpId = OpcUa_CloseSessionRequest;
		    oN(mReq,1,1);					//deleteSubscriptions
		}
		else if( io.attr("id") == "Read" )
		{
		    iTpId = OpcUa_ReadRequest;
		    oR(mReq,atof(io.attr("maxAge").c_str()),8);		//maxAge 0 ms
		    oN(mReq,atoi(io.attr("timestampsToReturn").c_str()),4);//timestampsTo Return (SERVER_1)
									//> nodesToRead []
		    oNu(mReq,io.childSize(),4);				//nodes
		    for( int i_n = 0; i_n < io.childSize(); i_n++ )
		    {
			oNodeId(mReq,NodeId::fromAddr(io.childGet(i_n)->attr("nodeId")));	//nodeId
			oNu(mReq,strtoul(io.childGet(i_n)->attr("attributeId").c_str(),NULL,0),4);	//attributeId (Value)
			oS(mReq,"");					//indexRange
			oSqlf(mReq,"");					//dataEncoding
		    }
		}
		else if( io.attr("id") == "Write" )
		{
		    iTpId = OpcUa_WriteRequest;
									//> nodesToWrite []
		    oNu(mReq,io.childSize(),4);				//nodes
		    for( int i_n = 0; i_n < io.childSize(); i_n++ )
		    {
			XMLNode *nd = io.childGet(i_n);
			oNodeId(mReq,NodeId::fromAddr(nd->attr("nodeId")));			//nodeId
			oNu(mReq,strtoul(nd->attr("attributeId").c_str(),NULL,0),4);		//attributeId (Value)
			oS(mReq,"");								//indexRange
			oDataValue(mReq,0x0D,nd->text(),atoi(nd->attr("EncodingMask").c_str()));//value
		    }
		}
		else if( io.attr("id") == "Browse" )
		{
		    iTpId = OpcUa_BrowseRequest;
									//> view
		    oNodeId(mReq,0);					//viewId (0)
		    oTm(mReq,0);					//timestamp
		    oNu(mReq,0,4);					//viewVersion

		    oNu(mReq,100,4);					//requestedMax ReferencesPerNode
									//> nodesToBrowse
		    oNu(mReq,io.childSize(),4);				//Nodes 1
		    for( int i_n = 0; i_n < io.childSize(); i_n++ )
		    {
			oNodeId(mReq,NodeId::fromAddr(io.childGet(i_n)->attr("nodeId")));	//nodeId (RootFolder)
			oNu(mReq,strtoul(io.childGet(i_n)->attr("browseDirection").c_str(),NULL,0),4);	//browseDirection (FORWARD_0)
			oNodeId(mReq,OpcUa_HierarchicalReferences);	//referenceTypeId (HierarchicalReferences)
			oNu(mReq,1,1);					//includeSubtypes (true)
			oNu(mReq,strtoul(io.childGet(i_n)->attr("nodeClassMask").c_str(),NULL,0),4);	//nodeClassMask
			oNu(mReq,strtoul(io.childGet(i_n)->attr("resultMask").c_str(),NULL,0),4);	//resultMask
		    }
		}
		else throw TError(OpcUa_BadNotSupported,"OPC UA Bin",_("Request '%s' isn't supported."),io.attr("id").c_str());

		rez.reserve( 200 );
		rez.append( "MSGF" );					//SecureChannel message
		oNu(rez,0,4);						//Message size
		oNu(rez,strtoul(io.attr("SecChnId").c_str(),NULL,10),4);	//Secure channel identifier
		oNu(rez,strtoul(io.attr("SecTokenId").c_str(),NULL,10),4);	//TokenId
		int begEncBlck = rez.size();
									//> Sequence header
		uint32_t SeqNumber = strtoul(io.attr("SeqNumber").c_str(),NULL,10)+1;
		io.setAttr("SeqNumber",TSYS::uint2str(SeqNumber));
		uint32_t SeqReqId = strtoul(io.attr("SeqReqId").c_str(),NULL,10)+1;
		io.setAttr("SeqReqId",TSYS::uint2str(SeqReqId));
		oNu(rez,SeqNumber,4);					//Sequence number
		oNu(rez,SeqReqId,4);					//RequestId
									//> Extension body object
		oNodeId(rez,iTpId);					//TypeId request
									//>> Request Header
		oNodeId(rez,strtoul(io.attr("authTokenId").c_str(),NULL,10));	//Session AuthenticationToken
		oTm(rez,TSYS::curTime());				//timestamp
		uint32_t ReqHandle = strtoul(io.attr("ReqHandle").c_str(),NULL,10)+1;
		io.setAttr("ReqHandle",TSYS::uint2str(ReqHandle));
		oN(rez,ReqHandle,4);					//requestHandle
		oNu(rez,0,4);						//returnDiagnostics
		oS(rez,"");						//auditEntryId
		oNu(rez,3000,4);					//timeoutHint
									//>>> Extensible parameter
		oNodeId(rez,0);						//TypeId (0)
		oNu(rez,0,1);						//Encoding
		rez.append(mReq);					//Same request
		oNu(rez,rez.size(),4,4);				//> Real message size
		string secPolicy = io.attr("SecPolicy");
		char secMessMode = atoi(io.attr("SecurityMode").c_str());
		if( secMessMode == OPCEndPoint::Sign || secMessMode == OPCEndPoint::SignAndEncrypt )
		{
		    string servKey = io.attr("servKey");
		    //> Padding place
		    if( secMessMode == OPCEndPoint::SignAndEncrypt )
		    {
			int kSz = servKey.size()/3;
			int paddingSize = ((rez.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(rez.size()-begEncBlck+20);
			rez += string(paddingSize,(char)(paddingSize-1));
		    }
		    //Real message size calc and place
		    oNu( rez, rez.size()+20, 4, 4 );
		    //> Signature
		    rez += symmetricSign(rez,servKey,secPolicy);
		    //> Encoding
		    if( secMessMode == OPCEndPoint::SignAndEncrypt )
			rez.replace(begEncBlck,rez.size()-begEncBlck,symmetricEncrypt(rez.substr(begEncBlck),servKey,secPolicy));
		}

#if OSC_DEBUG >= 5
		printf("TEST 13: Request '%s':\n%s\n",io.attr("id").c_str(),TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif
		//> Send request and wait respond
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );
		int off = 4;
		for( ; rez.size() < 8 || rez.size() < iNu(rez,off,4); off = 4 )
		{
		    resp_len = tro.messIO( NULL, 0, buf, sizeof(buf), 0, true );
		    rez.append( buf, resp_len );
		}

#if OSC_DEBUG >= 5
		printf("TEST 13a: Response '%s':\n%s\n",io.attr("id").c_str(),TSYS::strDecode(rez,TSYS::Bin).c_str());
#endif

		off = 4;
		if( rez.size() < 8 || iNu(rez,off,4) != rez.size() )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTooLarge,_("Respond size is not coincedence."));
		else if( rez.compare(0,4,"ERRF") == 0 ) err = iErr(rez,off);
		else if( rez.compare(0,4,"MSGF") != 0 )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTypeInvalid,_("Respond don't acknowledge."));
		else
		{
		    iNu(rez,off,4);					//Secure channel identifier
		    iNu(rez,off,4);					//Symmetric Algorithm Security Header : TokenId
		    //>> Decrypt message block and signature check
		    if( secMessMode == OPCEndPoint::Sign || secMessMode == OPCEndPoint::SignAndEncrypt )
		    {
			string clKey = io.attr("clKey");
			if( secMessMode == OPCEndPoint::SignAndEncrypt )
			    rez.replace(off,rez.size()-off,symmetricDecrypt(rez.substr(off),clKey,secPolicy));
			if( rez.substr(rez.size()-20) != symmetricSign(rez.substr(0,rez.size()-20),clKey,secPolicy) )	//Check Signature
			    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Signature error") );
		    }
									//> Sequence header
		    iNu(rez,off,4);					//Sequence number
		    iNu(rez,off,4);					//RequestId
									//> Extension Object
		    uint32_t oTpId = iNodeId(rez,off).numbVal();	//TypeId
									//>> Body
									//>>> RespondHeader
		    iTm(rez,off);					//timestamp
		    iN(rez,off,4);					//requestHandle
		    uint32_t stCode = iNu(rez,off,4);			//StatusCode
		    iN(rez,off,1);					//serviceDiagnostics
		    iS(rez,off);					//stringTable
									//>>> Extensible parameter
		    iNodeId(rez,off);					//TypeId (0)
		    iNu(rez,off,1);					//Encoding

		    switch( oTpId )
		    {
			case OpcUa_FindServersResponse:
			{
			    if( iTpId != OpcUa_FindServersRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    int AppDescrNub = iNu(rez,off,4);				//List items
			    for( int i_l = 0; i_l < AppDescrNub; i_l++ )
			    {
				XMLNode *ad = io.childAdd("ApplicationDescription");
				ad->setAttr("applicationUri",iS(rez,off));		//applicationUri
				ad->setAttr("productUri",iS(rez,off));			//productUri
				ad->setAttr("applicationName",iSl(rez,off));		//applicationName
				ad->setAttr("applicationType",TSYS::uint2str(iNu(rez,off,4)));	//applicationType
				ad->setAttr("gatewayServerUri",iS(rez,off));		//gatewayServerUri
				ad->setAttr("discoveryProfileUri",iS(rez,off));		//discoveryProfileUri
											//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);			//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    ad->childAdd("discoveryUrl")->setText(iS(rez,off));	//discoveryUrl
			    }
			    break;
			}
			case OpcUa_GetEndpointsResponse:
			{
			    if( iTpId != OpcUa_GetEndpointsRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    int EndpointDescrNub = iNu(rez,off,4);		//List items
			    for( int i_l = 0; i_l < EndpointDescrNub; i_l++ )
			    {
				XMLNode *xep = io.childAdd("EndpointDescription");
										//>>> EndpointDescription
				xep->setAttr("endpointUrl",iS(rez,off));	//endpointUrl
										//>>>> server (ApplicationDescription)
				xep->setAttr("applicationUri",iS(rez,off));	//applicationUri
				xep->setAttr("productUri",iS(rez,off));		//productUri
				xep->setAttr("applicationName",iSl(rez,off));	//applicationName
				xep->setAttr("applicationType",TSYS::uint2str(iNu(rez,off,4)));	//applicationType
				xep->setAttr("gatewayServerUri",iS(rez,off));	//gatewayServerUri
				xep->setAttr("discoveryProfileUri",iS(rez,off));//discoveryProfileUri
										//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);		//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    xep->childAdd("discoveryUrl")->setText(iS(rez,off));	//discoveryUrl
				xep->childAdd("serverCertificate")->setText(certDER2PEM(iS(rez,off)));	//>>> serverCertificate
				xep->setAttr("securityMode",TSYS::uint2str(iNu(rez,off,4)));	//securityMode
				xep->setAttr("securityPolicyUri",iS(rez,off));	//securityPolicyUri
										//>>>> userIdentityTokens
				int userIdentityTokensN = iNu(rez,off,4);	//List items
				for( int i_l2 = 0; i_l2 < userIdentityTokensN; i_l2++ )
				{
				    XMLNode *xit = xep->childAdd("userIdentityToken");
				    xit->setAttr("policyId",iS(rez,off));	//policyId
				    xit->setAttr("tokenType",TSYS::uint2str(iNu(rez,off,4)));	//tokenType
				    xit->setAttr("issuedTokenType",iS(rez,off));		//issuedTokenType
				    xit->setAttr("issuerEndpointUrl",iS(rez,off));		//issuerEndpointUrl
				    xit->setAttr("securityPolicyUri",iS(rez,off));		//securityPolicyUri
				}
				xep->setAttr("transportProfileUri",iS(rez,off));		//transportProfileUri
				xep->setAttr("securityLevel",TSYS::uint2str(iNu(rez,off,1)));	//securityLevel
			    }
			    break;
			}
			case OpcUa_CreateSessionResponse:
			{
			    if( iTpId != OpcUa_CreateSessionRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    io.setAttr("sesId",TSYS::uint2str(iNodeId(rez,off).numbVal()));	//sessionId
			    io.setAttr("authTokenId",TSYS::uint2str(iNodeId(rez,off).numbVal()));//authentication Token
			    io.setAttr("sesTm",TSYS::real2str(iR(rez,off,8)));			//revisedSession Timeout, ms
			    io.setAttr("servNonce",iS(rez,off));				//serverNonce
			    string servCert = certDER2PEM(iS(rez,off));	//serverCertificate
			    int EndpointDescrNub = iNu(rez,off,4);	//List items
			    for( int i_l = 0; i_l < EndpointDescrNub; i_l++ )
			    {
									//> EndpointDescription
				iS(rez,off);				//endpointUrl
									//>> server (ApplicationDescription)
				iS(rez,off);				//applicationUri
				iS(rez,off);				//productUri
				iSl(rez,off);				//applicationName
				iNu(rez,off,4);				//applicationType
				iS(rez,off);				//gatewayServerUri
				iS(rez,off);				//discoveryProfileUri
									//>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);	//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    iS(rez,off);			//discoveryUrl
				iS(rez,off);				//> serverCertificate
				iNu(rez,off,4);				//securityMode
				iS(rez,off);				//securityPolicyUri
									//>> userIdentityTokens
				int userIdentityTokensN = iNu(rez,off,4);//List items
				for( int i_l2 = 0; i_l2 < userIdentityTokensN; i_l2++ )
				{
				    iS(rez,off);			//policyId
				    iNu(rez,off,4);			//tokenType
				    iS(rez,off);			//issuedTokenType
				    iS(rez,off);			//issuerEndpointUrl
				    iS(rez,off);			//securityPolicyUri
				}
				iS(rez,off);				//transportProfileUri
				iNu(rez,off,1);				//securityLevel
			    }
			    iS(rez,off);				//serverSoftware Certificates []
									//> serverSignature
			    string alg = iS(rez,off);			//algorithm
			    string sign = iS(rez,off);			//signature
			    if( io.attr("SecPolicy") != "None" )
			    {
				if( !asymmetricVerify( certPEM2DER(io.childGet("ClientCert")->text())+io.attr("Nonce"), sign, servCert ) )
				    throw TError( OpcUa_BadApplicationSignatureInvalid, "OPC UA Bin", _("Application signature error") );
			    }

			    iNu(rez,off,4);				//maxRequest MessageSize
			    break;
			}
			case OpcUa_ActivateSessionResponse:
			{
			    if( iTpId != OpcUa_ActivateSessionRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    iS(rez,off);				//serverNonce
			    iS(rez,off);				//results []
			    iS(rez,off);				//diagnosticInfos []
			    break;
			}
			case OpcUa_CloseSessionResponse:
			    if( iTpId != OpcUa_CloseSessionRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    io.setAttr("sesId","");			//sessionId
			    io.setAttr("authTokenId","");		//authentication Token
			    break;
			case OpcUa_ReadResponse:
			{
			    if( iTpId != OpcUa_ReadRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
									//> results []
			    int resN = iNu(rez,off,4);			//Nodes number
			    for( int i_r = 0; i_r < resN && i_r < io.childSize(); i_r++ )
				iDataValue(rez,off,*io.childGet(i_r));
									//>> diagnosticInfos []
			    iNu(rez,off,4);				//Items number
			    break;
			}
			case OpcUa_WriteResponse:
			{
			    if( iTpId != OpcUa_WriteRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
									//> results []
			    int resN = iNu(rez,off,4);			//Number
			    for( int i_r = 0; i_r < resN && i_r < io.childSize(); i_r++ )
				io.childGet(i_r)->setAttr("Status",TSYS::strMess("0x%x",iNu(rez,off,4)));
									//>> diagnosticInfos []
			    iNu(rez,off,4);				//Items number
			    break;
			}
			case OpcUa_BrowseResponse:
			{
			    if( iTpId != OpcUa_BrowseRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
									//> results []
			    int resN = iNu(rez,off,4);			//Numbers
			    for( int i_r = 0; i_r < resN && i_r < io.childSize(); i_r++ )
			    {
				XMLNode *rno = io.childGet(i_r);
				uint32_t resMask = strtoul(rno->attr("resultMask").c_str(),NULL,0);	//resultMask
				rno->setAttr("statusCode",TSYS::uint2str(iNu(rez,off,4)));		//statusCode
				iS(rez,off);				//continuationPoint
									//>> References []
				int refN = iNu(rez,off,4);		//Numbers
				for( int i_rf = 0; i_rf < refN; i_rf++ )
				{
				    XMLNode *bno = rno->childAdd("bNode");
				    bno->setAttr("referenceTypeId",TSYS::uint2str(iNodeId(rez,off).numbVal()));
				    bno->setAttr("isForward",iNu(rez,off,1)?"1":"0");
				    bno->setAttr("nodeId",iNodeId(rez,off).toAddr());
				    bno->setAttr("browseName",iSqlf(rez,off));
				    bno->setAttr("displayName",iSl(rez,off));
				    bno->setAttr("nodeClass",TSYS::uint2str(iNu(rez,off,4)));
				    bno->setAttr("typeDefinition",TSYS::uint2str(iNodeId(rez,off).numbVal()));
				}
			    }
			    iS(rez,off);				//diagnosticInfos []
			    break;
			}
			case OpcUa_ServiceFault:
			    err = TSYS::strMess("0x%x:%s",stCode,_("Service fault"));
			    break;
		    }
                }
	    }
	}
	else err = TSYS::strMess("0x%x:%s",OpcUa_BadServiceUnsupported,TSYS::strMess(_("OPC_UA protocol '%s' isn't supported."),io.name().c_str()).c_str());
    }
    catch(TError er)
    {
	if( er.cod < 0 || er.cod > 100 ) err = TSYS::strMess("0x%x:%s",er.cod,er.mess.c_str());
	else err = TSYS::strMess("0x%x:%s",OpcUa_BadInvalidArgument,_("Remote host error"),er.mess.c_str());
    }

    io.setAttr("err",err);
}

string TProt::iErr( const string &buf, int &off )
{
    uint32_t ecod = iNu(buf,off,4);
    string   estr = iS(buf,off);
    return TSYS::strMess("0x%x:%s",ecod,estr.c_str());
}

const char *TProt::iVal( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    return rb.data()+off-vSz;
}

int32_t TProt::iN( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    switch( vSz )
    {
	case 1:	return *(int8_t*)(rb.data()+off-vSz);
	case 2:	return *(int16_t*)(rb.data()+off-vSz);
	case 4:	return *(int32_t*)(rb.data()+off-vSz);
    }
    throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Number size '%d' error."),vSz);
}

uint32_t TProt::iNu( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    switch( vSz )
    {
	case 1:	return *(uint8_t*)(rb.data()+off-vSz);
	case 2:	return *(uint16_t*)(rb.data()+off-vSz);
	case 4:	return *(uint32_t*)(rb.data()+off-vSz);
    }
    throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Number size '%d' error."),vSz);
}

double TProt::iR( const string &rb, int &off, char vSz )
{
    if( vSz == 4 ) return *(float *)iVal(rb,off,vSz);
    else if( vSz == 8 ) return *(double *)iVal(rb,off,vSz);
    throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Real number size '%d' error."),vSz);
}

string TProt::iS( const string &rb, int &off )
{
    int sSz = iN(rb,off,4); sSz = vmax(0,sSz);
    off += sSz;
    if( off > rb.size() ) throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Buffer size is less for requested string."));
    return rb.substr(off-sSz,sSz);
}

string TProt::iSl( const string &rb, int &off, string *locale )
{
    char encMsk = iN(rb,off,1);
    string sloc;
    if( encMsk & 0x01 )
    {
	sloc = iS(rb,off);
	if( locale ) *locale = sloc;
    }
    if( encMsk & 0x02 ) return iS(rb,off);
    return sloc;
}

string TProt::iSqlf( const string &rb, int &off, uint16_t *nsIdx )
{
    uint16_t tNsIdx = iNu(rb,off,2);
    if( nsIdx ) *nsIdx = tNsIdx;
    return iS(rb,off);
}

long long TProt::iTm( const string &rb, int &off )
{
    int64_t tmStamp = *(int64_t*)TProt::iVal(rb,off,8);
    return (tmStamp/10ll)-11644473600000000ll;
}

NodeId TProt::iNodeId( const string &rb, int &off )
{
    off += 1;
    if( off > rb.size() ) throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("Buffer size is less for requested NodeId."));
    char enc = rb[off-1];
    switch( enc )
    {
	case 0x00:	//Two Byte
	    return NodeId(iNu(rb,off,1));
	case 0x01:	//Four Byte
	{
	    uint8_t ns = iNu(rb,off,1);
	    return NodeId(iNu(rb,off,2),ns);
	}
	case 0x02:	//Numeric
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iNu(rb,off,4),ns);
	}
	case 0x03:	//String
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns);
	}
	case 0x04:	//Guid
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(string(iVal(rb,off,16),16),ns,NodeId::Guid);
	}
	case 0x05:	//Opaque
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns,NodeId::Opaque);
	}
    }
    throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("NodeId type %d error or don't support."),enc);
}

void TProt::iDataValue( const string &buf, int &off, XMLNode &nd )
{
    nd.setAttr("Status","");
    //>> Data Value
    uint8_t em = iNu(buf,off,1);	//Encoding Mask
    if( em&0x01 )			//Value
    {
	//>>> Variant
	uint8_t emv = iNu(buf,off,1);	//Encoding Mask
	nd.setAttr("EncodingMask",TSYS::uint2str(emv));
	int32_t arrL = 1;
	if( emv&0x80 )	arrL = iNu(buf,off,4);	//ArrayLength
	string rezVl;
	for( int i_v = 0; i_v < arrL; i_v++ )
	{
	    if( arrL > 1 && i_v ) rezVl += "\n";
	    switch( emv&0x3F )
	    {
		case OpcUa_Boolean:
		case OpcUa_SByte:	rezVl += TSYS::int2str(iN(buf,off,1));	break;
		case OpcUa_Byte:	rezVl += TSYS::int2str(iNu(buf,off,1));	break;
		case OpcUa_Int16:	rezVl += TSYS::int2str(iN(buf,off,2));	break;
		case OpcUa_UInt16:	rezVl += TSYS::uint2str(iNu(buf,off,2));break;
		case OpcUa_Int32:	rezVl += TSYS::int2str(iN(buf,off,4));	break;
		case OpcUa_UInt32:	rezVl += TSYS::uint2str(iNu(buf,off,4));break;
		case OpcUa_Int64:	rezVl += TSYS::strMess("%lld",*(int64_t*)iVal(buf,off,8));	break;
		case OpcUa_UInt64:	rezVl += TSYS::strMess("%llu",*(uint64_t*)iVal(buf,off,8));	break;
		case OpcUa_Float:	rezVl += TSYS::real2str(iR(buf,off,4));	break;
		case OpcUa_Double:	rezVl += TSYS::real2str(iR(buf,off,8));	break;
		case OpcUa_String:
		case OpcUa_ByteString:	rezVl += iS(buf,off);	break;
		case OpcUa_NodeId:	rezVl += iNodeId(buf,off).toAddr();	break;
		case OpcUa_StatusCode:	rezVl += TSYS::strMess("0x%x",iNu(buf,off,4));	break;
		case OpcUa_QualifiedName:
		{
		    uint16_t ns;
		    string vl = iSqlf(buf,off,&ns);
		    rezVl += TSYS::uint2str(ns)+":"+vl;
		    break;
		}
		case OpcUa_LocalizedText:
		{
		    string loc, vl;
		    vl = iSl(buf,off,&loc);
		    rezVl += loc+":"+vl;
		    break;
		}
		default: throw TError(OpcUa_BadDecodingError,"OPC_UA Bin",_("Data type '%d' isn't supported."),emv&0x3F);
	    }
	}
	nd.setText(rezVl);
	//> ArrayDimension
	if( emv&0x40 ) throw TError(OpcUa_BadDecodingError,"OPC_UA Bin",_("ArrayDimensions field don't supported now."));
	//????
    }
    if( em&0x02 )	//Status
	nd.setAttr("Status",TSYS::strMess("0x%x",iNu(buf,off,4)));
    if( em&0x04 )	//SourceTimestamp
	nd.setAttr("SourceTimestamp",TSYS::ll2str(iTm(buf,off)));
    if( em&0x10 )	//SourcePicoseconds
	nd.setAttr("SourcePicoseconds",TSYS::uint2str(iNu(buf,off,2)));
    if( em&0x08 )	//ServerTimestamp
	nd.setAttr("ServerTimestamp",TSYS::ll2str(iTm(buf,off)));
    if( em&0x20 )	//ServerPicoseconds
	nd.setAttr("ServerPicoseconds",TSYS::uint2str(iNu(buf,off,2)));
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

void TProt::oR( string &buf, double val, char sz )
{
    if( sz == 4 ) { float vl = val; buf.append( (char*)&vl, sz ); }
    else if( sz == 8 ) buf.append( (char*)&val, sz );
    else throw TError(OpcUa_BadEncodingError,modPrt->nodePath().c_str(),_("Real number size '%d' error."),sz);
}

void TProt::oS( string &buf, const string &val )
{
    oN(buf,(val.size()?val.size():-1),4);
    buf.append(val);
}

void TProt::oSl( string &buf, const string &val, const string &locale )
{
    oN(buf,(locale.empty()?0:0x01)|(val.empty()?0:0x02),1);
    if( !locale.empty() ) oS(buf,locale);
    if( !val.empty() ) oS(buf,val);
}

void TProt::oSqlf( string &buf, const string &val, uint16_t nsIdx )
{
    oN(buf,nsIdx,2);
    oS(buf,val);
}

void TProt::oNodeId( string &buf, const NodeId &val )
{
    switch( val.type() )
    {
	case NodeId::Numeric:
	    if( val.ns() == 0 && val.numbVal() <= 255 )
	    {
		buf += (char)0x00;
		buf += (char)val.numbVal();
	    }
	    else if( val.ns() <= 255 && val.numbVal() <= 65535 )
	    {
		buf += (char)0x01;
		buf += (char)val.ns();
		oNu(buf,val.numbVal(),2);
	    }
	    else
	    {
		buf += (char)0x02;
		oNu(buf,val.ns(),2);
		oNu(buf,val.numbVal(),4);
	    }
	    break;
	case NodeId::String:
	    buf += (char)0x03;
	    oNu(buf,val.ns(),2);
	    oS(buf,val.strVal());
	    break;
	case NodeId::Guid:
	    buf += (char)0x04;
	    oNu(buf,val.ns(),2);
	    buf += val.strVal().substr(0,16);
	    break;
	case NodeId::Opaque:
	    buf += (char)0x05;
	    oNu(buf,val.ns(),2);
	    oS(buf,val.strVal());
	    break;
    }
}

void TProt::oTm( string &buf, long long val )
{
    int64_t tmStamp = 10ll*(val+11644473600000000ll);
    buf.append( (char*)&tmStamp, sizeof(tmStamp) );
}

void TProt::oRef( string &buf, uint32_t resMask, const NodeId &nodeId, const NodeId &refTypeId,
	bool isForward, const string &name, uint32_t nodeClass, const NodeId &typeDef )
{
    if( resMask&RdRm_RefType ) oNodeId(buf,refTypeId);		else oNodeId(buf,0);
    if( resMask&RdRm_IsForward ) oNu(buf,isForward,1);		else oNu(buf,0,1);
    oNodeId(buf,nodeId);
    if( resMask&RdRm_BrowseName ) oSqlf(buf,name,nodeId.ns());	else oSqlf(buf,"");
    if( resMask&RdRm_DisplayName ) oSl(buf,name,"en");		else oSl(buf,"");
    if( resMask&RdRm_NodeClass ) oNu(buf,nodeClass,4);		else oNu(buf,0,4);
    if( resMask&RdRm_TypeDef ) oNodeId(buf,typeDef);		else oNodeId(buf,0);
}

void TProt::oDataValue( string &buf, uint8_t eMsk, const TVariant &vl, uint8_t vEMsk, long long srcTmStmp )
{
    eMsk = eMsk & (~0x30);	//Exclude picoseconds parts
    if( eMsk & 0x02 )	eMsk = eMsk & (~0x01);

    oNu(buf,eMsk,1);		//Encoding Mask
    if( eMsk & 0x01 )		//> Variant
    {
	oNu(buf,vEMsk,1);	//Encoding Mask
	int32_t arrL = 1;
	if( vEMsk&0x80 )	//Array process
	{
	    arrL = 0;
	    for( int off = 0; TSYS::strParse(vl.getS(),0,"\n",&off).size(); ) arrL++;
	    oNu(buf,arrL,4);	//ArrayLength
	}
	for( int i_v = 0, off = 0; i_v < arrL; i_v++ )
	{
	    TVariant setVl = (arrL==1) ? vl : TSYS::strParse(vl.getS(),0,"\n",&off);
	    switch( vEMsk&0x3F )
	    {
		case OpcUa_Boolean:
		case OpcUa_SByte:	oN(buf,setVl.getI(),1);		break;
		case OpcUa_Byte:	oNu(buf,setVl.getI(),1);	break;
		case OpcUa_Int16:	oN(buf,setVl.getI(),2);		break;
		case OpcUa_UInt16:	oNu(buf,setVl.getI(),2);	break;
		case OpcUa_Int32:	oN(buf,setVl.getI(),4);		break;
		case OpcUa_UInt32:	oNu(buf,strtoul(setVl.getS().c_str(),NULL,10),4);	break;
		case OpcUa_Int64:	{ int64_t vl = strtoll(setVl.getS().c_str(),NULL,10); buf.append((char*)&vl,8); break; }
		case OpcUa_UInt64:	{ uint64_t vl = strtoull(setVl.getS().c_str(),NULL,10); buf.append((char*)&vl,8); break; }
		case OpcUa_Float:	oR(buf,setVl.getR(),4);		break;
		case OpcUa_Double:	oR(buf,setVl.getR(),8);		break;
		case OpcUa_String:
		case OpcUa_ByteString:	oS(buf,setVl.getS());		break;
		case OpcUa_NodeId:	oNodeId(buf,NodeId::fromAddr(setVl.getS()));	break;
		case OpcUa_StatusCode:	oNu(buf,strtoll(setVl.getS().c_str(),NULL,0),4);	break;
		case OpcUa_QualifiedName:	oSqlf(buf,setVl.getS());	break;
		case OpcUa_LocalizedText:	oSl(buf,setVl.getS(),Mess->lang2Code());	break;
		default: throw TError(OpcUa_BadDecodingError,"OPC_UA Bin",_("Data type '%d' isn't supported."),vEMsk&0x3F);
	    }
	}
	//> ArrayDimension
	if( vEMsk & 0x40 ) throw TError(OpcUa_BadDecodingError,"OPC_UA Bin",_("ArrayDimensions field don't supported now."));
	//????
    }
    if( eMsk & 0x02 )	oN(buf,vl.getI(),4);		//Status
    if( eMsk & 0x04 )	oTm(buf,srcTmStmp?srcTmStmp:TSYS::curTime());	//SourceTimestamp
    if( eMsk & 0x08 )	oTm(buf,TSYS::curTime());	//ServerTimestamp
}

string TProt::applicationUri( )		{ return SYS->host()+"/OpenSCADA/DAQ.OPC_UA"; }

string TProt::productUri( )		{ return "OpenSCADA/DAQ.OPC_UA"; }

string TProt::applicationName( )	{ return "OpenScadaOpcServer@"+SYS->host(); }

string TProt::randBytes( int num )
{
    unsigned char buf[num];
    if( RAND_bytes(buf,num) != 1 )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("randBytes error: %s"),err);
    }
    return string((char*)buf,num);
}

string TProt::certPEM2DER( const string &spem )
{
    int len = 0;
    BIO *bm = NULL;
    X509 *x = NULL;
    string rez = "";

    if( spem.empty() )	return rez;
    bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,spem.data(),spem.size()) == spem.size() )
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if( x ) len = i2d_X509(x,NULL);
    if( len > 0 )
    {
	char *buf = (char*)malloc(len);
	unsigned char *p = (unsigned char *)buf;
	if( buf )
	{
	    i2d_X509(x,&p);
	    rez.assign(buf,len);
	    free(buf);
	}
    }
    //> Free temporary data
    if( bm ) BIO_free(bm);
    if( x ) X509_free(x);

    if( len <= 0 )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("certPEM2DER error: %s"),err);
    }
    return rez;
}

string TProt::certDER2PEM( const string &certDer )
{
    BIO *bm = NULL;
    X509 *x = NULL;
    string rez = "";

    if( certDer.empty() ) return rez;
    const unsigned char *ind = (const unsigned char *)certDer.data();
    x = d2i_X509(NULL,&ind,certDer.size());
    if( x ) bm = BIO_new(BIO_s_mem());
    if( bm && PEM_write_bio_X509(bm,x) > 0 )
    {
	char buf[STR_BUF_LEN];
	for( int i_r = 0; (i_r=BIO_read(bm,buf,sizeof(buf))) > 0; ) rez.append(buf,i_r);
    }
    //> Free temporary data
    if( bm ) BIO_free(bm);
    if( x ) X509_free(x);

    if( rez.empty() )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("certDER2PEM error: %s"),err);
    }
    return rez;
}

string TProt::certThumbprint( const string &spem )
{
    BIO *bm = NULL;
    X509 *x = NULL;
    unsigned int n = 0;
    unsigned char md[EVP_MAX_MD_SIZE];

    if( spem.empty() ) return "";
    bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,spem.data(),spem.size()) == spem.size() )
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if( x ) X509_digest(x,EVP_sha1(),md,&n);

    //> Free temporary data
    if( bm ) BIO_free(bm);
    if( x ) X509_free(x);

    if( n <= 0 )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("certThumbprint: %s"),err);
    }

    return string((char*)md,n);
}

string TProt::asymmetricEncrypt( const string &mess, const string &certPem, const string &secPolicy )
{
    int keysize = 0;
    string rez;
    X509 *x = NULL;
    BIO *bm = NULL;
    EVP_PKEY *pkey = NULL;
    RSA *rsa = NULL;
    EVP_MD_CTX *ctx;

    int paddSize = 11, padd = RSA_PKCS1_PADDING;
    if( secPolicy.find("Rsa15") == string::npos ) { paddSize = 42; padd = RSA_PKCS1_OAEP_PADDING; }

    if( !certPem.empty() && !mess.empty() ) bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,certPem.data(),certPem.size()) == certPem.size() );
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if( x ) pkey = X509_get_pubkey(x);
    if( pkey ) rsa = EVP_PKEY_get1_RSA(pkey);
    if( rsa ) keysize = RSA_size(rsa);
    if( keysize && !(mess.size()%(keysize-paddSize)) )
    {
	unsigned char rsaOut[keysize];
	for( int i_b = 0; i_b < mess.size()/(keysize-paddSize); i_b++ )
	{
	    int blen = RSA_public_encrypt( (keysize-paddSize), (const unsigned char *)(mess.data()+i_b*(keysize-paddSize)), rsaOut, rsa, padd );
	    if( blen <= 0 ) break;
	    rez.append((char*)rsaOut,blen);
	}
    }
    //> Free temporary data
    if( pkey ) EVP_PKEY_free(pkey);
    if( bm ) BIO_free(bm);
    if( rsa ) RSA_free(rsa);
    if( x ) X509_free(x);

    if( rez.empty() )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("asymmetricEncrypt: %s"),err);
    }

    return rez;
}

string TProt::asymmetricDecrypt( const string &mess, const string &keyPem, const string &secPolicy )
{
    int keysize = 0;
    BIO *bm = NULL;
    RSA *rsa = NULL;
    EVP_PKEY *pkey = NULL;
    string rez = "";

    if( !keyPem.empty() && !mess.empty() ) bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,keyPem.data(),keyPem.size()) == keyPem.size() );
	pkey = PEM_read_bio_PrivateKey(bm,NULL,0,(char*)"keypass");
    if( pkey ) rsa = EVP_PKEY_get1_RSA(pkey);
    if( rsa ) keysize = RSA_size(rsa);
    if( keysize && !(mess.size()%keysize) )
    {
	unsigned char rsaOut[keysize];
	for( int i_b = 0; i_b < mess.size()/keysize; i_b++ )
	{
	    int blen = RSA_private_decrypt( keysize, (const unsigned char *)(mess.data()+i_b*keysize), rsaOut, rsa,
		((secPolicy.find("Rsa15") == string::npos) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING) );
	    if( blen <= 0 ) break;
	    rez.append((char*)rsaOut,blen);
	}
    }

    //> Free temporary data
    if( pkey ) EVP_PKEY_free(pkey);
    if( bm ) BIO_free(bm);
    if( rsa ) RSA_free(rsa);

    if( rez.empty() )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("asymmetricDecrypt: %s"),err);
    }

    return rez;
}

bool TProt::asymmetricVerify( const string &mess, const string &sign, const string &certPem )
{
    int rez = -1;
    X509 *x = NULL;
    BIO *bm = NULL, *mdtmp = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX *ctx;

    if( !certPem.empty() && !sign.empty() && !mess.empty() ) bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,certPem.data(),certPem.size()) == certPem.size() );
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if( x ) pkey = X509_get_pubkey(x);
    if( pkey && sign.size() == EVP_PKEY_size(pkey) ) mdtmp = BIO_new(BIO_f_md());
    if( mdtmp )
    {
	BIO_set_md(mdtmp, EVP_sha1());
	mdtmp = BIO_push(mdtmp,bm);
	if( BIO_write(mdtmp,mess.data(),mess.size()) == mess.size() )
	{
	    BIO_get_md_ctx(mdtmp, &ctx);
	    if( ctx ) rez = EVP_VerifyFinal( ctx, (const unsigned char*)sign.data(), sign.size(), pkey );
	}
    }
    //> Free temporary data
    if( x ) X509_free(x);
    if( mdtmp ) BIO_free(mdtmp);
    if( bm ) BIO_free(bm);
    if( pkey ) EVP_PKEY_free(pkey);

    if( rez == -1 )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("asymmetricVerify: %s"),err);
    }
    return (rez == 1);
}

string TProt::asymmetricSign( const string &mess, const string &pvPem )
{
    BIO *bm = NULL, *mdtmp = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX *ctx;
    unsigned char buf[STR_BUF_LEN];
    unsigned int blen = 0;

    if( !pvPem.empty() && !mess.empty() ) bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,pvPem.data(),pvPem.size()) == pvPem.size() );
	pkey = PEM_read_bio_PrivateKey(bm,NULL,0,(char*)"keypass");
    if( pkey ) mdtmp = BIO_new(BIO_f_md());
    if( mdtmp )
    {
	BIO_set_md(mdtmp, EVP_sha1());
	mdtmp = BIO_push(mdtmp,bm);
	if( BIO_write(mdtmp,mess.data(),mess.size()) == mess.size() )
	{
	    BIO_get_md_ctx(mdtmp, &ctx);
	    if( ctx ) EVP_SignFinal(ctx, buf, &blen, pkey);
	}
    }
    //> Free temporary data
    if( mdtmp ) BIO_free(mdtmp);
    if( bm ) BIO_free(bm);
    if( pkey ) EVP_PKEY_free(pkey);

    if( !blen )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("asymmetricSign: %s"),err);
    }

    return string((char*)buf,blen);
}

int TProt::asymmetricKeyLength( const string &keyCertPem )
{
    int rez = -1;
    X509 *x = NULL;
    BIO *bm = NULL, *mdtmp = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX *ctx;

    if( !keyCertPem.empty() ) bm = BIO_new(BIO_s_mem());
    if( bm && BIO_write(bm,keyCertPem.data(),keyCertPem.size()) == keyCertPem.size() );
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if( x ) pkey = X509_get_pubkey(x);
    if( pkey ) rez = EVP_PKEY_size(pkey);

    //> Free temporary data
    if( x ) X509_free(x);
    if( bm ) BIO_free(bm);
    if( pkey ) EVP_PKEY_free(pkey);

    if( rez == -1 )
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("keyCertPem: %s"),err);
    }

    return rez;
}

string TProt::deriveKey( const string &secret, const string &seed, int keyLen )
{
    int hashCnt = (keyLen+19)/20;
    unsigned char hashRez[20*hashCnt], hashTmp[20+seed.size()];

    memcpy(hashTmp+20,seed.data(),seed.size());
    HMAC( EVP_sha1(), secret.data(), secret.size(), (const unsigned char*)seed.data(), seed.size(), hashTmp, 0 );
    for( int i_c = 0; i_c < hashCnt; i_c++ )
    {
	HMAC(EVP_sha1(), secret.data(), secret.size(), hashTmp, 20+seed.size(), hashRez+20*i_c, 0);
	HMAC(EVP_sha1(), secret.data(), secret.size(), hashTmp, 20, hashTmp, 0);
    }
    return string((const char*)hashRez,keyLen);
}

string TProt::symmetricEncrypt( const string &mess, const string &keySet, const string &secPolicy )
{
    if( mess.empty() || keySet.size() < (3*16) ) return "";
    int keySize = keySet.size()/3;
    int signSize = keySize;
    if( secPolicy == "Basic256" ) signSize = 24;

    unsigned char obuf[mess.size()], ivecb[keySize];

    AES_KEY key;
    AES_set_encrypt_key((const unsigned char*)(keySet.data()+signSize),keySize*8,&key);
    memcpy(ivecb,keySet.data()+signSize+keySize,keySize);
    AES_cbc_encrypt((const unsigned char*)mess.data(),obuf,mess.size(),&key,ivecb,1);

    return string((char*)obuf,mess.size());
}

string TProt::symmetricDecrypt( const string &mess, const string &keySet, const string &secPolicy )
{
    if( mess.empty() || keySet.size() < (3*16) ) return "";
    int keySize = keySet.size()/3;
    int signSize = keySize;
    if( secPolicy == "Basic256" ) signSize = 24;    

    unsigned char obuf[mess.size()], ivecb[keySize];

    AES_KEY key;
    AES_set_decrypt_key((const unsigned char*)(keySet.data()+signSize),keySize*8,&key);
    memcpy(ivecb,keySet.data()+signSize+keySize,keySize);
    AES_cbc_encrypt((const unsigned char*)mess.data(),obuf,mess.size(),&key,ivecb,0);

    return string((char*)obuf,mess.size());
}

string TProt::symmetricSign( const string &mess, const string &keySet, const string &secPolicy )
{
    unsigned char hashRez[20];
    int keySize = keySet.size()/3;
    int signSize = keySize;
    if( secPolicy == "Basic256" ) signSize = 24;    

    HMAC( EVP_sha1(), keySet.data(), signSize, (const unsigned char*)mess.data(), mess.size(), hashRez, 0 );
    return string((const char*)hashRez,20);
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/ep_",_("End point"),RWRWR_,"root",SPRT_ID,2,"idm","1","idSz","20");
	if(ctrMkNode("area",opt,0,"/ep",_("End points")))
	    ctrMkNode("list",opt,-1,"/ep/ep",_("End points"),RWRWR_,"root",SPRT_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","ep_","idSz","20");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/ep_" || a_path == "/ep/ep")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    vector<string> lst;
	    epList(lst);
	    for(unsigned i_f=0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(epAt(lst[i_f]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    epAdd(vid); epAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))	chldDel(mEndPnt,opt->attr("id"),-1,1);
    }
    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* NodeId object                                 *
//*************************************************
NodeId::NodeId( uint32_t in, uint16_t ins ) : mTp(NodeId::Numeric), mNs(ins)
{
    setNumbVal(in);
}

NodeId::NodeId( const string &istr, uint16_t ins, NodeId::Type tp ) : mTp(NodeId::Numeric), mNs(ins)
{
    setStrVal(istr,tp);
}

NodeId::~NodeId( )
{
    if( type() != NodeId::Numeric )
    {
	delete str;
	mTp = NodeId::Numeric;
    }
}

NodeId &NodeId::operator=( NodeId &node )
{
    setNs( node.ns() );
    switch( node.type() )
    {
	case NodeId::Numeric:	setNumbVal(node.numbVal());	break;
	case NodeId::String: case NodeId::Guid: case NodeId::Opaque:
	    setStrVal(node.strVal(),node.type());	break;
    }
    return *this;
}

uint32_t NodeId::numbVal( ) const
{
    if( type() == NodeId::Numeric )	return numb;
    return strtoul(str->c_str(),NULL,0);
}

string NodeId::strVal( ) const
{
    if( type() == NodeId::Numeric )	return TSYS::uint2str(numb);
    return *str;
}

void NodeId::setNumbVal( uint32_t in )
{
    if( type() == NodeId::String ) delete str;
    mTp = NodeId::Numeric;
    numb = in;
}

void NodeId::setStrVal( const string &istr, NodeId::Type tp )
{
    if( tp == NodeId::Numeric ) return;
    if( type() == NodeId::Numeric ) str = new string(istr);
    mTp = tp;
    *str = istr;
}

NodeId NodeId::fromAddr( const string &strAddr )
{
    int off = 0;
    string vl, dt, rez;
    char bf[3];
    uint16_t ns = strtoul(TSYS::strParse(strAddr,0,":",&off).c_str(),NULL,0);
    if( off < strAddr.size() ) vl = strAddr.substr(off);
    else { vl = strAddr; ns = 0; }

    //> Check for Guid
    if( vl.size() == 38 && vl[0] == '{' && vl[vl.size()-1] == '}' && 
	vl[9] == '-' && vl[14] == '-' && vl[19] == '-' && vl[24] == '-' )
    {
	bf[2] = 0;
	//>> Get Data1
	dt = vl.substr(1,8);
	for( int i_s = (dt.size()-2); i_s >= 0; i_s-=2 )
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	//>> Get Data2
	dt = vl.substr(10,4);
	for( int i_s = (dt.size()-2); i_s >= 0; i_s-=2 )
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	//>> Get Data3
	dt = vl.substr(15,4);
	for( int i_s = (dt.size()-2); i_s >= 0; i_s-=2 )
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	//>> Get Data4a
	dt = vl.substr(20,4);
	for( int i_s = 0; i_s < dt.size(); i_s+=2 )
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	//>> Get Data4b
	dt = vl.substr(25,12);
	for( int i_s = 0; i_s < dt.size(); i_s+=2 )
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	return NodeId(rez,ns,NodeId::Guid);
    }

    //> Check for string or opaque
    if( vl.size() >= 2 && vl[0] == '\"' && vl[vl.size()-1] == '\"' )
    {
	bf[2] = 0;
	char *endptr = 0;
	rez = "";
	for( int i_s = 1; !(vl.size()%2) && (!endptr || *endptr == 0) && i_s < (vl.size()-1); i_s+=2 )
	{ bf[0] = vl[i_s]; bf[1] = vl[i_s+1]; rez += (char)strtol(bf,&endptr,16); }
	if( rez.size() == (vl.size()-2)/2 )	return NodeId(rez,ns,NodeId::Opaque);
	return NodeId(vl.substr(1,vl.size()-2),ns);
    }

    //> Check for number
    bool isStr = false;
    for( int i_s = 0; i_s < vl.size() && !isStr; i_s++ )
	if( !isdigit(vl[i_s]) ) isStr = true;
    if( isStr ) return NodeId(vl,ns);
    return NodeId((uint32_t)strtoul(vl.c_str(),NULL,0),ns);
}

string NodeId::toAddr( ) const
{
    string vl;
    if( ns() ) vl = TSYS::uint2str(ns())+":";
    switch( type() )
    {
	case NodeId::Numeric:	vl += TSYS::uint2str(numbVal());	break;
	case NodeId::String:	vl += "\""+strVal()+"\"";		break;
	case NodeId::Guid:
	{
	    vl += "{";
	    //>> Get Data1
	    string svl = strVal().substr(0,4);
	    for( int i_sz = (svl.size()-1); i_sz >= 0; i_sz-- )
		vl += TSYS::strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //>> Get Data2
	    vl += "-";
	    svl = strVal().substr(4,2);
	    for( int i_sz = (svl.size()-1); i_sz >= 0; i_sz-- )
		vl += TSYS::strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //>> Get Data3
	    vl += "-";
	    svl = strVal().substr(6,2);
	    for( int i_sz = (svl.size()-1); i_sz >= 0; i_sz-- )
		vl += TSYS::strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //>> Get Data4a
	    vl += "-";
	    svl = strVal().substr(8,2);
	    for( int i_sz = 0; i_sz < svl.size(); i_sz++ )
		vl += TSYS::strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //>> Get Data4b
	    vl += "-";
	    svl = strVal().substr(10,6);
	    for( int i_sz = 0; i_sz < svl.size(); i_sz++ )
		vl += TSYS::strMess("%0.2x",(unsigned char)svl[i_sz]);
	    vl += "}";
	    break;
	}
	case NodeId::Opaque:
	{
	    vl += "\"";
	    string svl = strVal();
	    for( int i_sz = 0; i_sz < svl.size(); i_sz++ )
		vl += TSYS::strMess("%0.2x",(unsigned char)svl[i_sz]);
	    vl += "\"";
	    break;
	}
    }
    return vl;
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

bool TProtIn::mess( const string &reqst, string &answ, const string &sender )
{
    uint32_t mSz;
    bool KeepAlive = false;
    int off = 0;

    //> Continue for full request
    if( mNotFull )	{ mBuf = mBuf+reqst; mNotFull = false; }
    else mBuf = reqst;  //Save request to bufer

    string &rba = mBuf;

    answ = "";
    if( rba.size() <= 0 ) return mNotFull;

nextReq:
    string rb,out;
    off = 4;
    if( rba.size() < 8 || rba.size() < (mSz=TProt::iNu(rba,off,4)) ) return (mNotFull=true);
    rb = rba.substr(0,mSz);

    try
    {
	//> Check for hello message type
	if( rb.compare(0,4,"HELF") == 0 )
	{
	    if( rb.size() > 4096 )	throw TError(OpcUa_BadTcpMessageTooLarge,"","");

#if OSC_DEBUG >= 5
	    printf( "TEST 00: Hello request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());
#endif

	    off = 8;
	    TProt::iNu(rb,off,4);					//Protocol version
	    TProt::iNu(rb,off,4);					//Recive buffer size
	    TProt::iNu(rb,off,4);					//Send buffer size
	    TProt::iNu(rb,off,4);					//Max message size
	    TProt::iNu(rb,off,4);					//Max chunk count
	    string EndpntURL = TProt::iS(rb,off);			//EndpointURL

	    //>> Find accessable endpoint
	    vector<string> epLs;
	    owner().epList(epLs);
	    int i_ep;
	    for( i_ep = 0; i_ep < epLs.size(); i_ep++ )
		if( owner().epAt(epLs[i_ep]).at().enableStat() )
		    break;
	    if( i_ep >= epLs.size() )	throw TError(OpcUa_BadTcpEndpointUrlInvalid,"","");

	    //> Prepare acknowledge message
	    out.reserve( 28 );
	    out.append( "ACKF" );					//Acknowledge message type
	    TProt::oNu(out,28,4);					//Message size
	    TProt::oNu(out,OpcUa_ProtocolVersion,4);			//Protocol version
	    TProt::oNu(out,OpcUa_ReciveBufferSize,4);			//Recive buffer size
	    TProt::oNu(out,OpcUa_SendBufferSize,4);			//Send buffer size
	    TProt::oNu(out,OpcUa_MaxMessageSize,4);			//Max message size
	    TProt::oNu(out,OpcUa_MaxChunkCount,4);			//Max chunk count

#if OSC_DEBUG >= 5
	    printf( "TEST 00a: Hello response:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());
#endif
	}
	//> Check for Open SecureChannel message type
	else if( rb.compare(0,4,"OPNF") == 0 )
	{
#if OSC_DEBUG >= 5
	    printf( "TEST 01: Open SecureChannel request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());
#endif

	    off = 8;
	    uint32_t secChnId = TProt::iNu(rb,off,4);			//Secure channel identifier
									//> Security Header
	    string secPlcURI = TProt::iS(rb,off);			//Security policy URI
	    string secPlc = TSYS::strParse(secPlcURI,1,"#");
	    bool isSecNone = false;
	    int symKeySz = 0, asymKeyPad = 0;
	    if( secPlc == "None" )		isSecNone = true;
	    else if( secPlc == "Basic128Rsa15" ){ symKeySz = 16; asymKeyPad = 11; }
	    else if( secPlc == "Basic256" )	{ symKeySz = 32; asymKeyPad = 42; }
	    else throw TError(OpcUa_BadSecurityPolicyRejected,"","");

	    //>> Find server with that policy
	    vector<string> epLs;
	    owner().epList(epLs);
	    int i_epOk = -1;
	    AutoHD<OPCEndPoint> wep;
	    for( int i_ep = 0; i_epOk < 0 && i_ep < epLs.size(); i_ep++ )
	    {
		wep = owner().epAt(epLs[i_ep]);
		if( !wep.at().enableStat() ) continue;
		for( int i_s = 0; i_epOk < 0 && i_s < wep.at().secSize(); i_s++ )
		    if( wep.at().secPolicy(i_s) == secPlc )
			i_epOk = i_ep;
	    }
	    if( i_epOk < 0 ) throw TError(OpcUa_BadSecurityPolicyRejected,"","");

	    string clntCert = TProt::certDER2PEM(TProt::iS(rb,off));	//ClientCertificate
	    string serverCertThmbp = TProt::iS(rb,off);
	    if( !isSecNone )
	    {
		if( serverCertThmbp != TProt::certThumbprint(wep.at().cert()) )	//ServerCertificateThumbprint
		    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Server certificate thumbprint error.") );
		//>> Decode message block
		rb.replace(off,rb.size()-off,TProt::asymmetricDecrypt(rb.substr(off),wep.at().pvKey(),secPlc));
#if OSC_DEBUG >= 5
		printf( "TEST 01a: Open SecureChannel decrypted request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());
#endif
	    }
									//> Sequence header
	    uint32_t secNumb = TProt::iNu(rb,off,4);			//Sequence number
	    uint32_t reqId = TProt::iNu(rb,off,4);			//RequestId
									//> Extension body object
	    if( TProt::iNodeId(rb,off).numbVal() != OpcUa_OpenSecureChannelRequest )	//TypeId
		throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Requested OpenSecureChannel NodeId don't acknowledge") );
									//>> Request Header
	    TProt::iVal(rb,off,2);					//Session AuthenticationToken
	    TProt::iTm(rb,off);						//timestamp
	    int32_t rqHndl = TProt::iN(rb,off,4);			//requestHandle
	    TProt::iNu(rb,off,4);					//returnDiagnostics
	    TProt::iS(rb,off);						//auditEntryId
	    TProt::iNu(rb,off,4);					//timeoutHint
									//>>> Extensible parameter
	    TProt::iNodeId(rb,off);					//TypeId (0)
	    TProt::iNu(rb,off,1);					//Encoding
									//>>>> Standard request
	    TProt::iNu(rb,off,4);					//ClientProtocolVersion
	    TProt::iNu(rb,off,4);					//RequestType
	    char secMode = TProt::iNu(rb,off,4);				//SecurityMode
	    string clNonce = TProt::iS(rb,off);				//ClientNonce
	    int32_t reqLifeTm = TProt::iN(rb,off,4);			//RequestedLifetime

	    if( !isSecNone )
	    {
		off += TProt::iNu(rb,off,1);				//Pass padding
		if( !TProt::asymmetricVerify(rb.substr(0,off),rb.substr(off),clntCert) )	//Check Signature
		    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Signature error") );
	    }

	    //>> Find message secure mode
	    bool secModOK = false;
	    for( int i_s = 0; !secModOK && i_s < wep.at().secSize(); i_s++ )
		if( wep.at().secPolicy(i_s) == secPlc && wep.at().secMessageMode(i_s) == secMode )
		    secModOK = true;
	    if( !secModOK ) throw TError(OpcUa_BadSecurityModeRejected,"","");

	    uint32_t chnlId = owner().chnlOpen(wep.at().id(),reqLifeTm,clntCert,secPlc,secMode);

	    //> Prepare respond message
	    out.reserve( 200 );
	    out.append( "OPNF" );					//OpenSecureChannel message type
	    TProt::oNu(out,0,4);					//Message size
	    TProt::oNu(out,chnlId,4);					//Secure channel identifier
	    TProt::oS(out,secPlcURI);					//Security policy URI
	    TProt::oS(out,isSecNone?string(""):TProt::certPEM2DER(wep.at().cert()));	//ServerCertificate
	    TProt::oS(out,isSecNone?string(""):TProt::certThumbprint(clntCert));	//ClientCertificateThumbprint
	    int begEncBlck = out.size();
	    TProt::oNu(out,secNumb,4);					//Sequence number
	    TProt::oNu(out,reqId,4);					//RequestId
									//> Extension Object
	    TProt::oNodeId(out,NodeId(OpcUa_OpenSecureChannelResponse));	//TypeId
									//>> Body
									//>>> RespondHeader
	    TProt::oTm(out,TSYS::curTime());				//timestamp
	    TProt::oN(out,rqHndl,4);					//requestHandle
	    TProt::oN(out,0,4);						//StatusCode
	    TProt::oN(out,0,1);						//serviceDiagnostics
	    TProt::oS(out,"");						//stringTable
									//>>> Extensible parameter
	    TProt::oNodeId(out,0u);					//TypeId (0)
	    TProt::oNu(out,0,1);					//Encoding
									//>>>> Standard respond
	    TProt::oNu(out,OpcUa_ProtocolVersion,4);			//ServerProtocolVersion
	    TProt::oNu(out,chnlId,4);					//Secure channel identifier
	    TProt::oNu(out,owner().chnlGet(chnlId).TokenId,4);		//TokenId
	    TProt::oTm(out,owner().chnlGet(chnlId).tCreate);		//CreatedAt
	    TProt::oN(out,owner().chnlGet(chnlId).tLife,4);		//RevisedLifeTime (600000, minimum)

	    if( !isSecNone )
	    {
		//> Generate nonce
		string servNonce = TProt::randBytes(symKeySz);
		TProt::oS(out,servNonce);				//nonce
		//> Padding place
		int kSz = TProt::asymmetricKeyLength(wep.at().cert());
		int paddingSize = ((out.size()-begEncBlck+1+kSz+(kSz-asymKeyPad)-1)/(kSz-asymKeyPad))*(kSz-asymKeyPad)-(out.size()+kSz-begEncBlck);
		out += string(paddingSize,(char)(paddingSize-1));
		//Real message size calc and place
		TProt::oNu( out, begEncBlck + kSz*((out.size()-begEncBlck+kSz)/(kSz-asymKeyPad)), 4, 4 );
		//> Signature
		out += TProt::asymmetricSign( out, wep.at().pvKey() );
		//> Encoding
		out.replace(begEncBlck,out.size()-begEncBlck,TProt::asymmetricEncrypt(out.substr(begEncBlck),clntCert,secPlc));
		//> Set channel secure properties
		owner().chnlSecSet(chnlId,TProt::deriveKey(servNonce,clNonce,symKeySz*3),TProt::deriveKey(clNonce,servNonce,symKeySz*3));
	    }
	    else
	    {
		TProt::oS(out,"\001");					//nonce
		TProt::oNu(out,out.size(),4,4);				//Real message size
	    }

#if OSC_DEBUG >= 5
	    printf("TEST 01a: Open sec respond:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());
#endif
	}
	//> Check for Close SecureChannel message type
	else if( rb.compare(0,4,"CLOF") == 0 )
	{
#if OSC_DEBUG >= 5
	    printf( "TEST 01: Close SecureChannel request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());
#endif

	    off = 8;
	    uint32_t secId = TProt::iNu(rb,off,4);			//Secure channel identifier
	    uint32_t tokId = TProt::iNu(rb,off,4);			//TokenId
	    SecCnl scHd = owner().chnlGet(secId);
	    //>> Secure channel and token check
	    if( !scHd.TokenId )	return false;
	    if( scHd.TokenId != tokId )	throw TError( OpcUa_BadSecureChannelTokenUnknown, "OPC UA Bin", _("Secure channel unknown") );
	    //>> Decrypt message block
	    if( scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
		rb.replace(off,rb.size()-off,TProt::symmetricDecrypt(rb.substr(off),scHd.servKey,scHd.secPolicy));
									//> Sequence header
	    TProt::iNu(rb,off,4);					//Sequence number
	    TProt::iNu(rb,off,4);					//RequestId
									//> Extension body object
	    if( TProt::iNodeId(rb,off).numbVal() != OpcUa_CloseSecureChannelRequest )	//TypeId
		throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Requested OpenSecureChannel NodeId don't acknowledge") );
									//>> Request Header
	    TProt::iNodeId(rb,off);					//Session AuthenticationToken
	    TProt::iTm(rb,off);						//timestamp
	    TProt::iN(rb,off,4);					//requestHandle
	    TProt::iNu(rb,off,4);					//returnDiagnostics
	    TProt::iS(rb,off);						//auditEntryId
	    TProt::iNu(rb,off,4);					//timeoutHint
									//>>> Extensible parameter
	    TProt::iNodeId(rb,off);					//TypeId (0)
	    TProt::iNu(rb,off,1);					//Encoding
	    if( scHd.secMessMode == OPCEndPoint::Sign || scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
	    {
		if( scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
		    off += TProt::iNu(rb,off,1);			//Pass padding
		if( rb.substr(off) != TProt::symmetricSign(rb.substr(0,off),scHd.servKey,scHd.secPolicy) )	//Check Signature
		    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Signature error") );
	    }

	    owner().chnlClose( secId );

	    //> No respond. Close socket
	    return false;
	}
	//> Check for SecureChannel message type
	else if( rb.compare(0,4,"MSGF") == 0 )
	{
#if OSC_DEBUG >= 5
	    printf( "TEST 02: SecureChannel message:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());
#endif

	    off = 8;
	    uint32_t stCode = 0;
	    uint32_t secId = TProt::iNu(rb,off,4);			//Secure channel identifier
	    uint32_t tokId = TProt::iNu(rb,off,4);			//TokenId
	    SecCnl scHd = owner().chnlGet(secId);
	    //>> Secure channel and token check
	    if( !scHd.TokenId )	throw TError( OpcUa_BadSecureChannelClosed, "OPC UA Bin", _("Secure channel closed") );
	    if( scHd.TokenId != tokId )	throw TError( OpcUa_BadSecureChannelTokenUnknown, "OPC UA Bin", _("Secure channel unknown") );
	    AutoHD<OPCEndPoint> wep = owner().epAt(scHd.endPoint);
	    //>> Decrypt message block and signature check
	    if( scHd.secMessMode == OPCEndPoint::Sign || scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
	    {
		if( scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
		    rb.replace(off,rb.size()-off,TProt::symmetricDecrypt(rb.substr(off),scHd.servKey,scHd.secPolicy));
		if( rb.substr(rb.size()-20) != TProt::symmetricSign(rb.substr(0,rb.size()-20),scHd.servKey,scHd.secPolicy) )	//Check Signature
		    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Signature error") );
	    }
									//> Sequence header
	    uint32_t seqN = TProt::iNu(rb,off,4);			//Sequence number
	    uint32_t reqId = TProt::iNu(rb,off,4);			//RequestId
									//> Extension body object
	    int reqTp = TProt::iNodeId(rb,off).numbVal();		//TypeId request
									//>> Request Header
	    uint32_t sesTokId = TProt::iNodeId(rb,off).numbVal();	//Session AuthenticationToken
	    //>> Session check
	    if( sesTokId && reqTp!=OpcUa_CreateSessionRequest && !wep.at().sessActivate(sesTokId,secId,reqTp!=OpcUa_ActivateSessionRequest) )
	    { stCode = OpcUa_BadSessionIdInvalid; reqTp = OpcUa_ServiceFault; }
	    TProt::iTm(rb,off);						//timestamp
	    int32_t reqHndl = TProt::iN(rb,off,4);			//requestHandle
	    TProt::iNu(rb,off,4);					//returnDiagnostics
	    TProt::iS(rb,off);						//auditEntryId
	    TProt::iNu(rb,off,4);					//timeoutHint
									//>>> Extensible parameter
	    TProt::iNodeId(rb,off);					//TypeId (0)
	    TProt::iNu(rb,off,1);					//Encoding

	    //> Prepare respond message
	    string respEp;
	    switch( reqTp )
	    {
		case OpcUa_FindServersRequest:
		{
		    //>> Request
		    TProt::iS(rb,off);					//endpointUrl
		    TProt::iS(rb,off);					//localeIds []
		    TProt::iS(rb,off);					//serverUris []

		    //>> Respond
		    reqTp = OpcUa_FindServersResponse;
		    TProt::oNu(respEp,1,4);				//ApplicationDescription list items
									//>>>> ApplicationDescription 1
		    TProt::oS(respEp,TProt::applicationUri());		//applicationUri
		    TProt::oS(respEp,TProt::productUri());		//productUri
		    TProt::oSl(respEp,TProt::applicationName(),"en");	//applicationName
		    TProt::oNu(respEp,0,4);				//applicationType (SERVER)
		    TProt::oS(respEp,"");				//gatewayServerUri
		    TProt::oS(respEp,"");				//discoveryProfileUri
									//>>>> discoveryUrls
		    vector<string> duLs;
		    owner().discoveryUrls(duLs);
		    TProt::oNu(respEp,duLs.size(),4);			//List items
		    for( int i_du = 0; i_du < duLs.size(); i_du++ )
			TProt::oS(respEp,duLs[i_du]);			//discoveryUrl
		    break;
		}
		case OpcUa_GetEndpointsRequest:
		{
		    //>> Request
		    TProt::iS(rb,off);					//endpointUrl
		    TProt::iS(rb,off);					//localeIds []
		    TProt::iS(rb,off);					//profileUris []

		    //>> Respond
		    reqTp = OpcUa_GetEndpointsResponse;

		    respEp.reserve(2000);
		    TProt::oNu(respEp,0,4);				//EndpointDescrNubers list items
		    //>> Get enpoints policies list
		    vector<string> epLs;
		    owner().epList(epLs);
		    int epCnt = 0;
		    for( int i_ep = 0; i_ep < epLs.size(); i_ep++ )
		    {
			AutoHD<OPCEndPoint> ep = owner().epAt(epLs[i_ep]);
			if( !ep.at().enableStat() ) continue;
									//>>> EndpointDescription
			for( int i_sec = 0; i_sec < ep.at().secSize( ); i_sec++, epCnt++ )
			{
			    TProt::oS(respEp,ep.at().url());		//endpointUrl
									//>>>> server (ApplicationDescription)
			    TProt::oS(respEp,TProt::applicationUri());		//applicationUri
			    TProt::oS(respEp,TProt::productUri());		//productUri
			    TProt::oSl(respEp,TProt::applicationName(),"en");	//applicationName
			    TProt::oNu(respEp,0,4);			//applicationType (SERVER)
			    TProt::oS(respEp,"");			//gatewayServerUri
			    TProt::oS(respEp,"");			//discoveryProfileUri

									//>>>> discoveryUrls
			    vector<string> duLs;
			    owner().discoveryUrls(duLs);
			    TProt::oNu(respEp,duLs.size(),4);		//List items
			    for( int i_du = 0; i_du < duLs.size(); i_du++ )
				TProt::oS(respEp,duLs[i_du]);		//discoveryUrl

			    TProt::oS(respEp,TProt::certPEM2DER(ep.at().cert()));	//>>> serverCertificate
			    TProt::oNu(respEp,ep.at().secMessageMode(i_sec),4);	//securityMode:MessageSecurityMode
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#"+ep.at().secPolicy(i_sec));	//securityPolicyUri

									//>>>> userIdentityTokens
			    TProt::oNu(respEp,2,4);			//List items
									//>>>> userIdentityToken 1
			    TProt::oS(respEp,"Anonymous");		//policyId
			    TProt::oNu(respEp,0,4);			//tokenType
			    TProt::oS(respEp,"");			//issuedTokenType
			    TProt::oS(respEp,"");			//issuerEndpointUrl
			    TProt::oS(respEp,"");			//securityPolicyUri
									//>>>> userIdentityToken 2
			    TProt::oS(respEp,"UserName");		//policyId
			    TProt::oNu(respEp,1,4);			//tokenType
			    TProt::oS(respEp,"");			//issuedTokenType
			    TProt::oS(respEp,"");			//issuerEndpointUrl
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

			    TProt::oS(respEp,"");			//transportProfileUri
			    TProt::oNu(respEp,0,1);			//securityLevel
			}
		    }
		    TProt::oNu(respEp,epCnt,4,0);			//EndpointDescrNubers list items

		    break;
		}
		case OpcUa_CreateSessionRequest:
		{
		    //>> Request
									//> clientDescription (Application Description)
		    TProt::iS(rb,off);					//applicationUri
		    TProt::iS(rb,off);					//productUri
		    TProt::iSl(rb,off);					//applicationName
		    TProt::iNu(rb,off,4);				//applicationType (CLIENT_1)
		    TProt::iS(rb,off);					//gatewayServerUri
		    TProt::iS(rb,off);					//discoveryProfileUri
		    TProt::iS(rb,off);					//discoveryUrls

		    TProt::iS(rb,off);					//serverUri
		    TProt::iS(rb,off);					//endpointUrl
		    string sessNm = TProt::iS(rb,off);			//sessionName
		    string clNonce = TProt::iS(rb,off);			//clientNonce
		    string clCert = TProt::iS(rb,off);			//clientCertificate
		    double rStm = TProt::iR(rb,off,8);			//Requested SessionTimeout, ms
		    TProt::iNu(rb,off,4);				//maxResponse MessageSize

		    //>> Try for session reusing
		    int sessId = 0;
		    if( !sesTokId && wep.at().sessActivate(sesTokId,secId,true) ) sessId = sesTokId;
		    //>> Create new session
		    if( !sessId ) sessId = wep.at().sessCreate( sessNm, rStm );
		    string servNonce = TProt::randBytes(32);
		    wep.at().sessServNonceSet(sessId,servNonce);

		    //>> Respond
		    reqTp = OpcUa_CreateSessionResponse;

		    respEp.reserve(2000);
		    TProt::oNodeId(respEp,sessId);			//sessionId
		    TProt::oNodeId(respEp,sessId);			//authentication Token
		    TProt::oR(respEp,wep.at().sessGet(sessId).tInact,8);	//revisedSession Timeout, ms
		    TProt::oS(respEp,servNonce);				//serverNonce
		    TProt::oS(respEp,TProt::certPEM2DER(wep.at().cert()));	//serverCertificate
									//> EndpointDescr []
		    int enpNumperPos = respEp.size();
		    TProt::oNu(respEp,0,4);				//EndpointDescrNubers list items
		    //>> Get enpoints policies list
		    vector<string> epLs;
		    owner().epList(epLs);
		    int epCnt = 0;
		    for( int i_ep = 0; i_ep < epLs.size(); i_ep++ )
		    {
			AutoHD<OPCEndPoint> ep = owner().epAt(epLs[i_ep]);
			if( !ep.at().enableStat() ) continue;
									//>>> EndpointDescription
			for( int i_sec = 0; i_sec < ep.at().secSize( ); i_sec++, epCnt++ )
			{
			    TProt::oS(respEp,ep.at().url());		//endpointUrl
									//>>>> server (ApplicationDescription)
			    TProt::oS(respEp,TProt::applicationUri());	//applicationUri
			    TProt::oS(respEp,TProt::productUri());	//productUri
			    TProt::oSl(respEp,TProt::applicationName(),"en");	//applicationName
			    TProt::oNu(respEp,0,4);			//applicationType (SERVER)
			    TProt::oS(respEp,"");			//gatewayServerUri
			    TProt::oS(respEp,"");			//discoveryProfileUri

									//>>>> discoveryUrls
			    vector<string> duLs;
			    owner().discoveryUrls(duLs);
			    TProt::oNu(respEp,duLs.size(),4);		//List items
			    for( int i_du = 0; i_du < duLs.size(); i_du++ )
				TProt::oS(respEp,duLs[i_du]);		//discoveryUrl

			    TProt::oS(respEp,TProt::certPEM2DER(ep.at().cert()));	//>>> serverCertificate
			    TProt::oNu(respEp,ep.at().secMessageMode(i_sec),4);	//securityMode:MessageSecurityMode
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#"+ep.at().secPolicy(i_sec));	//securityPolicyUri

									//>>>> userIdentityTokens
			    TProt::oNu(respEp,2,4);			//List items
									//>>>> userIdentityToken 1
			    TProt::oS(respEp,"Anonymous");		//policyId
			    TProt::oNu(respEp,0,4);			//tokenType
			    TProt::oS(respEp,"");			//issuedTokenType
			    TProt::oS(respEp,"");			//issuerEndpointUrl
			    TProt::oS(respEp,"");			//securityPolicyUri
									//>>>> userIdentityToken 2
			    TProt::oS(respEp,"UserName");		//policyId
			    TProt::oNu(respEp,1,4);			//tokenType
			    TProt::oS(respEp,"");			//issuedTokenType
			    TProt::oS(respEp,"");			//issuerEndpointUrl
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

			    TProt::oS(respEp,"");			//transportProfileUri
			    TProt::oNu(respEp,0,1);			//securityLevel
			}
		    }
		    TProt::oNu(respEp,epCnt,4,enpNumperPos);		//EndpointDescrNubers real list items

		    TProt::oS(respEp,"");				//serverSoftware Certificates []
									//> serverSignature
		    if( scHd.secPolicy == "None" )
		    {
			TProt::oS(respEp,"");				//algorithm
			TProt::oS(respEp,"");				//signature
		    }
		    else
		    {
			TProt::oS(respEp,"http://www.w3.org/2000/09/xmldsig#rsa-sha1");			//algorithm
			TProt::oS(respEp,TProt::asymmetricSign(clCert+clNonce,wep.at().pvKey()));	//signature
		    }

		    TProt::oNu(respEp,0,4);				//maxRequest MessageSize
		    break;
		}
		case OpcUa_ActivateSessionRequest:
		{
		    //>> Request
									//> clientSignature
		    string alg = TProt::iS(rb,off);			//algorithm
		    string sign = TProt::iS(rb,off);			//signature
		    if( scHd.secPolicy != "None" )
		    {
			if( !TProt::asymmetricVerify( TProt::certPEM2DER(wep.at().cert())+wep.at().sessGet(sesTokId).servNonce, sign, scHd.clCert ) )
			    throw TError( OpcUa_BadApplicationSignatureInvalid, "OPC UA Bin", _("Application signature error") );
		    }

		    TProt::iNu(rb,off,4);				//clientSoftwareCertificates []
									//> localeIds []
		    uint32_t ln = TProt::iNu(rb,off,4);			//List number
		    for( int i_l = 0; i_l < ln; i_l++ )
			TProt::iS(rb,off);				//localeId
									//> userIdentityToken
		    uint32_t userIdTk = TProt::iNodeId(rb,off).numbVal();	//TypeId
		    TProt::iNu(rb,off,1);				//Encode
		    TProt::iNu(rb,off,4);				//Length
		    switch( userIdTk )
		    {
			case 321:					//AnonymousIdentityToken
			    TProt::iS(rb,off);				//policyId
			    break;
			default:
			    throw TError(OpcUa_BadUnexpectedError,modPrt->nodePath().c_str(),_("No supported userIdentityToken '%d'."),userIdTk);
		    }
									//> userTokenSignature
		    TProt::iS(rb,off);					//signature
		    TProt::iS(rb,off);					//algorithm

		    //>> Respond
		    reqTp = OpcUa_ActivateSessionResponse;
		    respEp.reserve(100);
		    string servNonce = TProt::randBytes(32);
		    wep.at().sessServNonceSet(sesTokId,servNonce);
		    TProt::oS(respEp,servNonce);			//serverNonce
		    TProt::oS(respEp,"");				//results []
		    TProt::oS(respEp,"");				//diagnosticInfos []
		    break;
		}
		case OpcUa_CloseSessionRequest:
		    //>> Request
		    TProt::iNu(rb,off,1);				//deleteSubscriptions
		    wep.at().sessClose( sesTokId );

		    //>> Respond
		    reqTp = OpcUa_CloseSessionResponse;
		    break;
		case OpcUa_ReadRequest:
		    respEp = wep.at().tcpReq( reqTp, rb.substr(off) );
		    reqTp = OpcUa_ReadResponse;
		    break;
		case OpcUa_BrowseRequest:
		    respEp = wep.at().tcpReq( reqTp, rb.substr(off) );
		    reqTp = OpcUa_BrowseResponse;
		    break;
		case OpcUa_WriteRequest:
		    respEp = wep.at().tcpReq( reqTp, rb.substr(off) );
		    reqTp = OpcUa_WriteResponse;
		    break;
		case OpcUa_ServiceFault:	break;
		default:
		    throw TError(OpcUa_BadNotSupported,modPrt->nodePath().c_str(),_("No supported request id '%d'."),reqTp);
	    }

	    out.reserve( 200 );
	    out.append( "MSGF" );					//OpenSecureChannel message type
	    TProt::oNu(out,0,4);					//Message size
	    TProt::oNu(out,secId,4);					//Secure channel identifier
	    TProt::oNu(out,tokId,4);					//Symmetric Algorithm Security Header : TokenId
	    int begEncBlck = out.size();
									//> Sequence header
	    TProt::oNu(out,seqN,4);					//Sequence number
	    TProt::oNu(out,reqId,4);					//RequestId
									//> Extension Object
	    TProt::oNodeId(out,reqTp);					//TypeId
									//>> Body
									//>>> RespondHeader
	    TProt::oTm(out,TSYS::curTime());				//timestamp
	    TProt::oN(out,reqHndl,4);					//requestHandle
	    TProt::oN(out,stCode,4);					//StatusCode
	    TProt::oN(out,0,1);						//serviceDiagnostics
	    TProt::oS(out,"");						//stringTable
									//>>> Extensible parameter
	    TProt::oNodeId(out,0u);					//TypeId (0)
	    TProt::oNu(out,0,1);					//Encoding
	    out.append(respEp);
	    TProt::oNu(out,out.size(),4,4);				//Real message size

	    if( scHd.secMessMode == OPCEndPoint::Sign || scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
	    {
		//> Padding place
		if( scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
		{
		    int kSz = scHd.clKey.size()/3;
		    int paddingSize = ((out.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(out.size()-begEncBlck+20);
		    out += string(paddingSize,(char)(paddingSize-1));
		}
		//Real message size calc and place
		TProt::oNu( out, out.size()+20, 4, 4 );
		//> Signature
		out += TProt::symmetricSign(out,scHd.clKey,scHd.secPolicy);
		//> Encoding
		if( scHd.secMessMode == OPCEndPoint::SignAndEncrypt )
		    out.replace(begEncBlck,out.size()-begEncBlck,TProt::symmetricEncrypt(out.substr(begEncBlck),scHd.clKey,scHd.secPolicy));
	    }

#if OSC_DEBUG >= 5
	    printf("TEST 02a: SecureChannel message respond:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());
#endif
	}
	else throw TError(OpcUa_BadNotSupported,"","");
    }
    catch(TError er)	{ answ.append(mkError(er.cod,er.mess)); return false; }

    answ.append(out);

    //> Check for multiply requests into the message
    if( rba.size() > mSz ) { rba.erase(0,mSz); goto nextReq; }

    return true;
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

//*************************************************
//* OPCEndPoint                                   *
//*************************************************
OPCEndPoint::OPCEndPoint( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), mDB(idb), mEn(false), cntReq(0),
    mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDscr(cfg("DESCR").getSd()), mAEn(cfg("EN").getBd()),
    mSerType(cfg("SerialzType").getId()), mURL(cfg("URL").getSd()), objTree("root")
{
    mId = iid;
    mURL = "opc.tcp://"+SYS->host()+":4841";
}

OPCEndPoint::~OPCEndPoint( )
{
    try{ setEnable(false); } catch(...) { }
}

TCntrNode &OPCEndPoint::operator=( TCntrNode &node )
{
    OPCEndPoint *src_n = dynamic_cast<OPCEndPoint*>(&node);
    if( !src_n ) return *this;

    if( enableStat( ) )	setEnable(false);

    //> Copy parameters
    string prevId = mId;
    *(TConfig*)this = *(TConfig*)src_n;
    mId = prevId;
    setDB(src_n->DB());

    return *this;
}

void OPCEndPoint::postDisable( int flag )
{
    try
    {
	if( flag ) SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TProt &OPCEndPoint::owner( )	{ return *(TProt*)nodePrev(); }

string OPCEndPoint::name( )	{ return mName.size() ? mName : id(); }

string OPCEndPoint::tbl( )	{ return owner().modId()+"_ep"; }

string OPCEndPoint::cert( )	{ return cfg("ServCert").getS(); }

string OPCEndPoint::pvKey( )	{ return cfg("ServPvKey").getS(); }

string OPCEndPoint::secPolicy( int isec )
{
    ResAlloc res( nodeRes(), false );
    if( isec < 0 || isec >= mSec.size() ) throw TError(nodePath().c_str(),_("Security setting %d error."));
    return mSec[isec].policy;
}

XMLNode *OPCEndPoint::nodeReg( const NodeId &parent, const NodeId &ndId, const string &name,
	int ndClass, const NodeId &refTypeId, const NodeId &typeDef )
{
    XMLNode *cNx = NULL;
    map<string,XMLNode*>::iterator rN, cN;
    //> Find already presented node
    cN = ndMap.find(ndId.toAddr());
    if( cN != ndMap.end() ) cNx = cN->second;
    else
    {
	if( parent.isNull() ) { cNx = &objTree; cNx->clear(); }
	else
	{
	    rN = ndMap.find(parent.toAddr());
	    if( rN == ndMap.end() )
		throw TError(nodePath().c_str(),_("Parent node '%s' no present for node '%s'."),parent.toAddr().c_str(),ndId.toAddr().c_str());
	    cNx = rN->second->childAdd("nd");
	}
    }
    cNx->setAttr("NodeId",ndId.toAddr())->
	setAttr("name",name)->
	setAttr("NodeClass",TSYS::int2str(ndClass))->
	setAttr("referenceTypeId",refTypeId.toAddr())->
	setAttr("typeDefinition",typeDef.toAddr());

    switch(ndClass)
    {
	case TProt::NC_Object:
	    cNx->setAttr("EventNotifier","0");
	    break;
	case TProt::NC_ObjectType: case TProt::NC_DataType:
	    cNx->setAttr("IsAbstract","0");
	    break;
	case TProt::NC_ReferenceType:
	    cNx->setAttr("IsAbstract","0")->setAttr("Symmetric","0");
	    break;
	case TProt::NC_VariableType:
	    cNx->setAttr("IsAbstract","0")->setAttr("DataType","0:0")->setAttr("ValueRank","-2");
	    break;
    }

    ndMap[ndId.toAddr()] = cNx;

    return cNx;
}

int OPCEndPoint::sessCreate( const string &iName, double iTInact )
{
    ResAlloc res( nodeRes(), true );
    int i_s = 0;
    for( ; i_s < mSess.size(); i_s++ )
	if( !mSess[i_s].tAccess ) break;
    if( i_s < mSess.size() ) mSess[i_s] = OPCSess(iName,iTInact);
    else mSess.push_back(OPCSess(iName,iTInact));

    return i_s+1;
}

void OPCEndPoint::sessServNonceSet( int sid, const string &servNonce )
{
    ResAlloc res( nodeRes(), false );
    if( sid <= 0 || sid > mSess.size() ) return;
    mSess[sid-1].servNonce = servNonce;
}

bool OPCEndPoint::sessActivate( int sid, uint32_t secCnl, bool check )
{
    ResAlloc res( nodeRes(), true );
    if( sid <= 0 || sid > mSess.size() || !mSess[sid-1].tAccess ) return false;
    mSess[sid-1].tAccess = TSYS::curTime();
    int i_s = 0;
    for( ; i_s < mSess[sid-1].secCnls.size(); i_s++ )
	if( mSess[sid-1].secCnls[i_s] == secCnl )
	    break;
    if( check && i_s >= mSess[sid-1].secCnls.size() ) return false;
    if( i_s >= mSess[sid-1].secCnls.size() ) mSess[sid-1].secCnls.push_back(secCnl);
    return true;
}

void OPCEndPoint::sessClose( int sid )
{
    ResAlloc res( nodeRes(), true );
    if( sid <= 0 || sid > mSess.size() || !mSess[sid-1].tAccess ) throw TError(nodePath().c_str(),_("No session %d present."),sid-1);
    mSess[sid-1] = OPCSess();
}

OPCSess OPCEndPoint::sessGet( int sid )
{
    ResAlloc res( nodeRes(), false );
    if( sid <= 0 || sid > mSess.size() ) return OPCSess();
    return mSess[sid-1];
}

OPCEndPoint::MessageSecurityMode OPCEndPoint::secMessageMode( int isec )
{
    ResAlloc res( nodeRes(), false );
    if( isec < 0 || isec >= mSec.size() ) throw TError(nodePath().c_str(),_("Security setting %d error."));
    return mSec[isec].messageMode;
}

bool OPCEndPoint::cfgChange( TCfg &ce )
{
    modif();
    return true;
}

void OPCEndPoint::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;
    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);

    //Security policies parse
    string sp = cfg("SecPolicies").getS();
    string spi;
    ResAlloc res( nodeRes(), true );
    mSec.clear();
    for( int off = 0; (spi=TSYS::strParse(sp,0,"\n",&off)).size(); )
	mSec.push_back( SecuritySetting(TSYS::strParse(spi,0,":"),atoi(TSYS::strParse(spi,1,":").c_str())) );
}

void OPCEndPoint::save_( )
{
    //Security policies store
    string sp;
    ResAlloc res( nodeRes(), false );
    for( int i_p = 0; i_p < mSec.size(); i_p++ )
	sp += mSec[i_p].policy + ":" + TSYS::int2str(mSec[i_p].messageMode)+"\n";
    cfg("SecPolicies").setS(sp);

    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void OPCEndPoint::setEnable( bool vl )
{
    if( mEn == vl ) return;

    cntReq = 0;

    if( vl )
    {
     //> Objects tree and nodes map init
     nodeReg(0u,OpcUa_RootFolder,"Root",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
      nodeReg(OpcUa_RootFolder,OpcUa_ViewsFolder,"Views",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
      nodeReg(OpcUa_RootFolder,OpcUa_ObjectsFolder,"Objects",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
       nodeReg(OpcUa_ObjectsFolder,NodeId(SYS->daq().at().subId(),1),SYS->daq().at().subId(),TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType)->setAttr("DisplayName",SYS->daq().at().subName());
      nodeReg(OpcUa_RootFolder,OpcUa_TypesFolder,"Types",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
       nodeReg(OpcUa_TypesFolder,OpcUa_ObjectTypesFolder,"ObjectTypes",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_ObjectTypesFolder,OpcUa_BaseObjectType,"BaseObjectType",TProt::NC_ObjectType,OpcUa_Organizes);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_BaseEventType,"BaseEventType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerCapabilitiesType,"ServerCapabilitiesType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerDiagnosticsType,"ServerDiagnosticsType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_SessionsDiagnosticsSummaryType,"SessionsDiagnosticsSummaryType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_SessionDiagnosticsObjectType,"SessionDiagnosticsObjectType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_VendorServerInfoType,"VendorServerInfoType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerRedundancyType,"ServerRedundancyType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ModellingRuleType,"ModellingRuleType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_FolderType,"FolderType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_DataTypeEncodingType,"DataTypeEncodingType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_DataTypeSystemType,"DataTypeSystemType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_StateType,"StateType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_TransitionType,"TransitionType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerType,"ServerType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,NodeId("DAQModuleObjectType",1),"DAQModuleObjectType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,NodeId("DAQControllerObjectType",1),"DAQControllerObjectType",TProt::NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,NodeId("DAQParameterObjectType",1),"DAQParameterObjectType",TProt::NC_ObjectType,OpcUa_HasSubtype);
       nodeReg(OpcUa_TypesFolder,OpcUa_VariableTypesFolder,"VariableTypes",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_VariableTypesFolder,OpcUa_BaseVariableType,"BaseVariableType",TProt::NC_VariableType,OpcUa_Organizes);
	 nodeReg(OpcUa_BaseVariableType,OpcUa_BaseDataVariableType,"BaseDataVariableType",TProt::NC_VariableType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseVariableType,OpcUa_PropertyType,"PropertyType",TProt::NC_VariableType,OpcUa_HasSubtype);
       nodeReg(OpcUa_TypesFolder,OpcUa_ReferenceTypesFolder,"ReferenceTypes",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_ReferenceTypesFolder,OpcUa_References,"References",TProt::NC_ReferenceType,OpcUa_Organizes,OpcUa_FolderType)->setAttr("IsAbstract","1")->setAttr("Symmetric","1");
	 nodeReg(OpcUa_References,OpcUa_HierarchicalReferences,"HierarchicalReferences",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	  nodeReg(OpcUa_HierarchicalReferences,OpcUa_HasChild,"HasChild",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_HasChild,OpcUa_Aggregates,"Aggregates",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	    nodeReg(OpcUa_Aggregates,OpcUa_HasComponent,"HasComponent",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","ComponentOf");
	    nodeReg(OpcUa_Aggregates,OpcUa_HasProperty,"HasProperty",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","PropertyOf");
	   nodeReg(OpcUa_HasChild,OpcUa_HasSubtype,"HasSubtype",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","SubtypeOf");
	  nodeReg(OpcUa_HierarchicalReferences,OpcUa_HasEventSource,"HasEventSource",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","EventSourceOf");
	  nodeReg(OpcUa_HierarchicalReferences,OpcUa_Organizes,"Organizes",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","OrganizedBy");
	 nodeReg(OpcUa_References,OpcUa_NonHierarchicalReferences,"NonHierarchicalReferences",TProt::NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
       nodeReg(OpcUa_TypesFolder,OpcUa_DataTypesFolder,"DataTypes",TProt::NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_DataTypesFolder,OpcUa_BaseDataType,"BaseDataType",TProt::NC_DataType,OpcUa_Organizes)->setAttr("IsAbstract","1");
	 nodeReg(OpcUa_BaseDataType,OpcUa_Boolean,"Boolean",TProt::NC_DataType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseDataType,OpcUa_Number,"Number",TProt::NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	  nodeReg(OpcUa_Number,OpcUa_Integer,"Integer",TProt::NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_Integer,OpcUa_SByte,"SByte",TProt::NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_Integer,OpcUa_Int16,"Int16",TProt::NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_Integer,OpcUa_Int32,"Int32",TProt::NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_Integer,OpcUa_Int64,"Int64",TProt::NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_UInteger,"UInteger",TProt::NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_UInteger,OpcUa_Byte,"Byte",TProt::NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt16,"UInt16",TProt::NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt32,"UInt32",TProt::NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt64,"UInt64",TProt::NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_Double,"Double",TProt::NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_Float,"Float",TProt::NC_DataType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseDataType,OpcUa_String,"String",TProt::NC_DataType,OpcUa_HasSubtype);
    }
    else
    {
	ndMap.clear();
	objTree.clear();
    }

    mEn = vl;
}

string OPCEndPoint::getStatus( )
{
    string rez = _("Disabled. ");
    if( enableStat( ) )
    {
	rez = _("Enabled. ");
	rez += TSYS::strMess( _("Requests %.4g."), cntReq );
    }

    return rez;
}

string OPCEndPoint::tcpReq( int reqTp, const string &rb )
{
    string respEp;
    int off = 0;
    switch( reqTp )
    {
	case OpcUa_BrowseRequest:
	{
	    //>> Request
						//> view
	    TProt::iNodeId(rb,off);		//viewId
	    TProt::iTm(rb,off);			//timestamp
	    TProt::iNu(rb,off,4);		//viewVersion

	    uint32_t nMaxRef = TProt::iNu(rb,off,4);	//requestedMax ReferencesPerNode
						//> nodesToBrowse
	    uint32_t nc = TProt::iNu(rb,off,4);	//Nodes

	    //>> Respond
						//> results []
	    TProt::oNu(respEp,nc,4);		//Nodes

	    //>>> Nodes list processing
	    for( uint32_t i_c = 0; i_c < nc; i_c++ )
	    {
		NodeId nid = TProt::iNodeId(rb,off);	//nodeId
		uint32_t bd = TProt::iNu(rb,off,4);	//browseDirection
		NodeId rtId = TProt::iNodeId(rb,off);	//referenceTypeId
		bool incSubTp = TProt::iNu(rb,off,1);	//includeSubtypes
		uint32_t nClass = TProt::iNu(rb,off,4);	//nodeClassMask
		uint32_t resMask = TProt::iNu(rb,off,4);//resultMask

		uint32_t stCode = 0, refNumb = 0;
		int stCodeOff = respEp.size(); TProt::oNu(respEp,stCode,4);	//statusCode
		TProt::oS(respEp,"");						//continuationPoint
		int refNumbOff = respEp.size(); TProt::oNu(respEp,refNumb,4);	//References [] = 0

		if( rtId.numbVal() != OpcUa_HierarchicalReferences && rtId.numbVal() != OpcUa_References ) continue;

		map<string,XMLNode*>::iterator ndX = ndMap.find(nid.toAddr());
		if( ndX == ndMap.end() )
		{
		    stCode = OpcUa_BadBrowseNameInvalid;
#if OSC_DEBUG >= 5
		    printf("TEST 101: Browse request to unknown node: %s\n",nid.toAddr().c_str());
#endif
		}
		else
		{
		    //> typeDefinition reference process
		    if( rtId.numbVal() == OpcUa_References && (bd == TProt::BD_FORWARD || bd == TProt::BD_BOTH) )
		    {
			map<string,XMLNode*>::iterator ndTpDef = ndMap.find(ndX->second->attr("typeDefinition"));
			if( ndTpDef != ndMap.end() )
			{
			    int cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
			    if( !nClass || nClass == cnClass )
			    {
				TProt::oRef(respEp,resMask,NodeId::fromAddr(ndTpDef->second->attr("NodeId")),
				    NodeId::fromAddr(ndTpDef->second->attr("referenceTypeId")),1,
				    ndTpDef->second->attr("name"),cnClass,
				    NodeId::fromAddr(ndTpDef->second->attr("typeDefinition")));
				refNumb++;
			    }
			}
		    }
		    //> Forward hierarchical references process
		    for( int i_ch = 0; (bd == TProt::BD_FORWARD || bd == TProt::BD_BOTH) && i_ch < ndX->second->childSize(); i_ch++ )
		    {
			XMLNode *chNd = ndX->second->childGet(i_ch);
			int cnClass = atoi(chNd->attr("NodeClass").c_str());
			if( nClass && nClass != cnClass ) continue;
			TProt::oRef(respEp,resMask,NodeId::fromAddr(chNd->attr("NodeId")),
			    NodeId::fromAddr(chNd->attr("referenceTypeId")),1,chNd->attr("name"),cnClass,
			    NodeId::fromAddr(chNd->attr("typeDefinition")));
			refNumb++;
		    }
		    //> Inverse hierarchical references process
		    if( (bd == TProt::BD_INVERSE || bd == TProt::BD_BOTH) && ndX->second->parent() )
		    {
			XMLNode *chNd = ndX->second->parent();
			int cnClass = atoi(chNd->attr("NodeClass").c_str());
			if( !nClass || nClass == cnClass )
			{
			    TProt::oRef(respEp,resMask,NodeId::fromAddr(chNd->attr("NodeId")),
				NodeId::fromAddr(chNd->attr("referenceTypeId")),0,chNd->attr("name"),cnClass,
				NodeId::fromAddr(chNd->attr("typeDefinition")));
			    refNumb++;
			}
		    }

		}
		//> Check for DAQ subsystem data
		if( nid.ns() == 1 && TSYS::strParse(nid.strVal(),0,".") == SYS->daq().at().subId() )
		{
		    vector<string> chLs;
		    stCode = 0;
		    //>> Connect to DAQ node
		    AutoHD<TCntrNode> cNd = SYS->daq();
		    string sel;
		    int nLev = 0;
		    for( int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++ )
			try { cNd = cNd.at().nodeAt(sel); }
			catch( TError err ) { stCode = OpcUa_BadBrowseNameInvalid; break; }
		    if( !stCode )
		    {
			//> typeDefinition reference browse
			if( nLev && rtId.numbVal() == OpcUa_References && (bd == TProt::BD_FORWARD || bd == TProt::BD_BOTH) )
			{
			    map<string,XMLNode*>::iterator ndTpDef;
			    switch( nLev )
			    {
				case 1:	ndTpDef = ndMap.find(NodeId("DAQModuleObjectType",1).toAddr());	break;
				case 2:	ndTpDef = ndMap.find(NodeId("DAQControllerObjectType",1).toAddr());	break;
				case 3:	ndTpDef = ndMap.find(NodeId("DAQParameterObjectType",1).toAddr());	break;
				case 4:	ndTpDef = ndMap.find(NodeId(OpcUa_BaseDataVariableType).toAddr());	break;
			    }
			    if( ndTpDef != ndMap.end() )
			    {
				int cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
				if( !nClass || nClass == cnClass )
				{
				    TProt::oRef(respEp,resMask,NodeId::fromAddr(ndTpDef->second->attr("NodeId")),
					NodeId::fromAddr(ndTpDef->second->attr("referenceTypeId")),1,
					ndTpDef->second->attr("name"),cnClass,
					NodeId::fromAddr(ndTpDef->second->attr("typeDefinition")));
				    refNumb++;
				}
			    }
			}
			//>> Forward browse
			if( (!nClass || nClass == TProt::NC_Object) && (bd == TProt::BD_FORWARD || bd == TProt::BD_BOTH) )
			{
			    switch( nLev )
			    {
				case 0:		//>>> Subsystem
				    ((AutoHD<TDAQS>)cNd).at().modList( chLs );
				    for( int i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++ )
					TProt::oRef(respEp,resMask,NodeId(nid.strVal()+"."+chLs[i_ch],1),OpcUa_Organizes,
					    true,chLs[i_ch],TProt::NC_Object,NodeId("DAQModuleObjectType",1));
				    break;
				case 1:		//>>> Module
				    ((AutoHD<TTipDAQ>)cNd).at().list( chLs );
				    for( int i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++ )
					TProt::oRef(respEp,resMask,NodeId(nid.strVal()+"."+chLs[i_ch],1),OpcUa_Organizes,
					    true,chLs[i_ch],TProt::NC_Object,NodeId("DAQControllerObjectType",1));
				    break;
				case 2:		//>>> Controller
				    ((AutoHD<TController>)cNd).at().list( chLs );
				    for( int i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++ )
					TProt::oRef(respEp,resMask,NodeId(nid.strVal()+"."+chLs[i_ch],1),OpcUa_Organizes,
					    true,chLs[i_ch],TProt::NC_Object,NodeId("DAQParameterObjectType",1));
				    break;
				case 3:		//>>> Parameter
				    ((AutoHD<TParamContr>)cNd).at().vlList( chLs );
				    for( int i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++ )
					TProt::oRef(respEp,resMask,NodeId(nid.strVal()+"."+chLs[i_ch],1),OpcUa_HasComponent,
					    true,chLs[i_ch],TProt::NC_Variable,OpcUa_BaseDataVariableType);
				    break;
			    }
			}
			//>> Inverse browse
			if( (!nClass || nClass == TProt::NC_Object) && (bd == TProt::BD_INVERSE || bd == TProt::BD_BOTH) && nid.strVal() != "DAQ" )
			{
			    TProt::oRef(respEp,resMask,NodeId(nid.strVal().substr(0,nid.strVal().rfind(".")),1),OpcUa_Organizes,
				false,TSYS::strParse(nid.strVal(),nLev,"."),TProt::NC_Object,OpcUa_FolderType);
			    refNumb++;
			}
		    }
		}
		if( stCode )	TProt::oNu(respEp,stCode,4,stCodeOff);
		if( refNumb )	TProt::oNu(respEp,refNumb,4,refNumbOff);
	    }
	    TProt::oS(respEp,"");		//diagnosticInfos []
	    break;
	}
	case OpcUa_ReadRequest:
	{
	    //>> Request
	    TProt::iR(rb,off,8);			//maxAge
	    uint32_t tmStRet = TProt::iNu(rb,off,4);	//timestampsTo Return
							//> nodesToRead []
	    uint32_t nc = TProt::iNu(rb,off,4);		//nodes
	    uint8_t eMsk = 0x01;
	    switch( tmStRet )
	    {
		case TProt::TS_SOURCE:	eMsk |= 0x04;	break;
		case TProt::TS_SERVER:	eMsk |= 0x08;	break;
		case TProt::TS_BOTH:	eMsk |= 0x0C;	break;
	    }

	    //>> Respond
	    TProt::oNu(respEp,nc,4);			//Numbers

	    //>>> Nodes list processing
	    for( uint32_t i_c = 0; i_c < nc; i_c++ )
	    {
		NodeId nid = TProt::iNodeId(rb,off);	//nodeId
		uint32_t aid = TProt::iNu(rb,off,4);	//attributeId
		TProt::iS(rb,off);			//indexRange
		TProt::iSqlf(rb,off);			//dataEncoding

		//> For temporary ????
		if( nid.ns() == 0 && nid.numbVal() == OpcUa_Server_ServerStatus_State )
		{
		    switch( aid )
		    {
			case TProt::AId_NodeId:	TProt::oDataValue(respEp,eMsk,nid.toAddr(),OpcUa_NodeId);	break;
			case TProt::AId_Value:	TProt::oDataValue(respEp,eMsk,0,OpcUa_Int32);		break;
			default: TProt::oDataValue(respEp,0x02,(int)OpcUa_BadAttributeIdInvalid);	break;
		    }
		    continue;
		}

		//> Get node from objects tree
		map<string,XMLNode*>::iterator ndX = ndMap.find(nid.toAddr());
		if( ndX != ndMap.end() )
		{
		    if( aid == TProt::AId_NodeId )	TProt::oDataValue(respEp,eMsk,nid.toAddr(),OpcUa_NodeId);
		    else if( aid == TProt::AId_NodeClass )
			TProt::oDataValue(respEp,eMsk,ndX->second->attr("NodeClass"),OpcUa_Int32);
		    else if( aid == TProt::AId_BrowseName )
			TProt::oDataValue(respEp,eMsk,ndX->second->attr("name"),OpcUa_QualifiedName);
		    else if( aid == TProt::AId_DisplayName )
			TProt::oDataValue(respEp,eMsk,ndX->second->attr(ndX->second->attr("DisplayName").empty()?"name":"DisplayName"),OpcUa_LocalizedText);
		    else if( aid == TProt::AId_WriteMask || aid == TProt::AId_UserWriteMask )
			TProt::oDataValue(respEp,eMsk,0,OpcUa_UInt32);
		    else if( aid == TProt::AId_IsAbstract && !ndX->second->attr("IsAbstract").empty() )
			TProt::oDataValue(respEp,eMsk,atoi(ndX->second->attr("IsAbstract").c_str()),OpcUa_Boolean);
		    else if( aid == TProt::AId_Symmetric && !ndX->second->attr("Symmetric").empty() )
			TProt::oDataValue(respEp,eMsk,atoi(ndX->second->attr("Symmetric").c_str()),OpcUa_Boolean);
		    else if( aid == TProt::AId_InverseName && !ndX->second->attr("InverseName").empty() )
			TProt::oDataValue(respEp,eMsk,ndX->second->attr("InverseName"),OpcUa_LocalizedText);
		    else if( aid == TProt::AId_EventNotifier && !ndX->second->attr("EventNotifier").empty() )
			TProt::oDataValue(respEp,eMsk,atoi(ndX->second->attr("EventNotifier").c_str()),OpcUa_Byte);
		    else if( aid == TProt::AId_DataType && !ndX->second->attr("DataType").empty() )
			TProt::oDataValue(respEp,eMsk,ndX->second->attr("DataType"),OpcUa_NodeId);
		    else if( aid == TProt::AId_ValueRank && !ndX->second->attr("ValueRank").empty() )
			TProt::oDataValue(respEp,eMsk,atoi(ndX->second->attr("ValueRank").c_str()),OpcUa_Int32);
		    else TProt::oDataValue(respEp,0x02,(int)OpcUa_BadAttributeIdInvalid);
		    continue;
		}

		//OpenSCADA DAQ parameter's attribute
		if( nid.ns() == 1 )
		{
		    //>> Connect to DAQ node
		    AutoHD<TCntrNode> cNd = SYS->daq();
		    string sel;
		    int nLev = 0;
		    for( int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++ )
			try { cNd = cNd.at().nodeAt(sel); }
			catch( TError err ) { break; }

		    if( sel.empty() )
		    {
			switch( aid )
			{
			    case TProt::AId_NodeId:
				TProt::oDataValue(respEp,eMsk,nid.toAddr(),OpcUa_NodeId);
				break;
			    case TProt::AId_BrowseName:
				TProt::oDataValue(respEp,eMsk,cNd.at().nodeName(),OpcUa_QualifiedName);
				break;
			    case TProt::AId_WriteMask: case TProt::AId_UserWriteMask:
				TProt::oDataValue(respEp,eMsk,0,OpcUa_UInt32);
				break;
			    default:
				//>>> Variable
				if( dynamic_cast<TVal*>(&cNd.at()) )
				    switch( aid )
				    {
					case TProt::AId_NodeClass:
					    TProt::oDataValue(respEp,eMsk,TProt::NC_Variable,OpcUa_Int32);
					    break;
					case TProt::AId_DisplayName:
					    TProt::oDataValue(respEp,eMsk,cNd.at().nodeName(),OpcUa_LocalizedText);
					    break;
					case TProt::AId_Descr:
					    TProt::oDataValue(respEp,eMsk,((AutoHD<TVal>)cNd).at().fld().descr(),OpcUa_String);
					    break;
					case TProt::AId_Value:
					    switch( ((AutoHD<TVal>)cNd).at().fld().type() )
					    {
						case TFld::Boolean:
						    TProt::oDataValue(respEp,eMsk,((AutoHD<TVal>)cNd).at().getB(),OpcUa_Boolean);
						    break;
						case TFld::Integer:
						    TProt::oDataValue(respEp,eMsk,((AutoHD<TVal>)cNd).at().getI(),OpcUa_Int32);
						    break;
						case TFld::Real:
						    TProt::oDataValue(respEp,eMsk,((AutoHD<TVal>)cNd).at().getR(),OpcUa_Double);
						    break;
						case TFld::String:
						    TProt::oDataValue(respEp,eMsk,((AutoHD<TVal>)cNd).at().getS(),OpcUa_String);
						    break;
					    }
					    break;
					case TProt::AId_DataType:
					{
					    NodeId dt;
					    switch(((AutoHD<TVal>)cNd).at().fld().type())
					    {
						case TFld::Boolean:	dt.setNumbVal(OpcUa_Boolean);	break;
						case TFld::Integer:	dt.setNumbVal(OpcUa_Int32);	break;
						case TFld::Real:	dt.setNumbVal(OpcUa_Double);	break;
						case TFld::String:	dt.setNumbVal(OpcUa_String);	break;
					    }
					    TProt::oDataValue(respEp,eMsk,dt.toAddr(),OpcUa_NodeId);
					    break;
					}
					case TProt::AId_ValueRank:
					    TProt::oDataValue(respEp,eMsk,-1,OpcUa_Int32);
					    break;
					case TProt::AId_ArrayDimensions:
					    TProt::oDataValue(respEp,eMsk,"",0x80|OpcUa_Int32);
					    break;
					case TProt::AId_AccessLevel: case TProt::AId_UserAccessLevel:
					    TProt::oDataValue( respEp,eMsk, TProt::ACS_Read | ( ((AutoHD<TVal>)cNd).at().fld().flg()&TFld::NoWrite ? 0 : TProt::ACS_Write ), OpcUa_Byte );
					    break;
					case TProt::AId_MinimumSamplingInterval:
					    TProt::oDataValue(respEp,eMsk,0,OpcUa_Double);
					    break;
					case TProt::AId_Historizing:
					    TProt::oDataValue(respEp,eMsk,false,OpcUa_Boolean);
					    break;
					default: TProt::oDataValue(respEp,0x02,(int)OpcUa_BadAttributeIdInvalid);
				    }
				//>>> Objects
				else
				    switch( aid )
				    {
					case TProt::AId_NodeClass:
					    TProt::oDataValue(respEp,eMsk,TProt::NC_Object,OpcUa_Int32);
					    break;
					case TProt::AId_DisplayName:
					    if( dynamic_cast<TModule*>(&cNd.at()) )
						TProt::oDataValue(respEp,eMsk,((AutoHD<TModule>)cNd).at().modName(),OpcUa_LocalizedText);
					    else if( dynamic_cast<TController*>(&cNd.at()) )
						TProt::oDataValue(respEp,eMsk,((AutoHD<TController>)cNd).at().name(),OpcUa_LocalizedText);
					    else if( dynamic_cast<TParamContr*>(&cNd.at()) )
						TProt::oDataValue(respEp,eMsk,((AutoHD<TParamContr>)cNd).at().name(),OpcUa_LocalizedText);
					    else TProt::oDataValue(respEp,eMsk,cNd.at().nodeName(),OpcUa_LocalizedText);
					    break;
					case TProt::AId_Descr:
					    if( dynamic_cast<TModule*>(&cNd.at()) )
						TProt::oDataValue(respEp,eMsk,((AutoHD<TModule>)cNd).at().modInfo("Description"),OpcUa_LocalizedText);
					    else if( dynamic_cast<TController*>(&cNd.at()) )
						TProt::oDataValue(respEp,eMsk,((AutoHD<TController>)cNd).at().descr(),OpcUa_LocalizedText);
					    else if( dynamic_cast<TParamContr*>(&cNd.at()) )
						TProt::oDataValue(respEp,eMsk,((AutoHD<TParamContr>)cNd).at().descr(),OpcUa_LocalizedText);
					    else TProt::oDataValue(respEp,0x02,(int)OpcUa_BadAttributeIdInvalid);
					    break;
					case TProt::AId_EventNotifier:
					    TProt::oDataValue(respEp,eMsk,0,OpcUa_Byte);	break;
					default: TProt::oDataValue(respEp,0x02,(int)OpcUa_BadAttributeIdInvalid);
				    }
			}
			continue;
		    }
		}

		TProt::oDataValue(respEp,0x02,(int)OpcUa_BadNodeIdUnknown);
	    }
	    TProt::oS(respEp,"");		//diagnosticInfos []
	    break;
	}
	case OpcUa_WriteRequest:
	{
	    //>> Request
							//> nodesToWrite []
	    uint32_t nc = TProt::iNu(rb,off,4);		//nodes

	    //>> Respond
	    TProt::oNu(respEp,nc,4);			//Numbers
	    for( int i_n = 0; i_n < nc; i_n++ )
	    {
		uint32_t rezSt = 0;
		NodeId nid = TProt::iNodeId(rb,off);	//nodeId
		uint32_t aid = TProt::iNu(rb,off,4);	//attributeId (Value)
		TProt::iS(rb,off);			//indexRange
		XMLNode nVal;
		TProt::iDataValue(rb,off,nVal);		//value

		//> Get node from objects tree
		map<string,XMLNode*>::iterator ndX = ndMap.find(nid.toAddr());
		if( ndX != ndMap.end() ) rezSt = OpcUa_BadNothingToDo;
		else if( nid.ns() == 1 )
		{
		    //>> Connect to DAQ node
		    AutoHD<TCntrNode> cNd = SYS->daq();
		    string sel;
		    int nLev = 0;
		    for( int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++ )
			try { cNd = cNd.at().nodeAt(sel); }
			catch( TError err ) { break; }

		    if( !sel.empty() )	rezSt = OpcUa_BadNodeIdUnknown;
		    else if( aid != TProt::AId_Value || !dynamic_cast<TVal*>(&cNd.at()) )
			rezSt = OpcUa_BadNothingToDo;
		    else ((AutoHD<TVal>)cNd).at().setS(nVal.text());
		}

		//>>> Write result status code
		TProt::oNu(respEp,rezSt,4);		//StatusCode
	    }
	    TProt::oS(respEp,"");		//diagnosticInfos []
	    break;
	}
    }
    return respEp;
}

void OPCEndPoint::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("End point: ")+name(),RWRWR_,"root",SPRT_ID);
	if(ctrMkNode("area",opt,-1,"/ep",_("End point")))
	{
	    if(ctrMkNode("area",opt,-1,"/ep/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/ep/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/ep/st/en_st",_("Enable"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/ep/st/db",_("DB"),RWRWR_,"root",SDB_ID,4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/ep/cfg",_("Config")))
	    {
		TConfig::cntrCmdMake(opt,"/ep/cfg",0,"root",SPRT_ID,RWRWR_);
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServCert",cfg("ServCert").fld().descr(),RWRW__,"root",SPRT_ID,3,"tp","str","cols","90","rows","7");
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServPvKey",cfg("ServPvKey").fld().descr(),RWRW__,"root",SPRT_ID,3,"tp","str","cols","90","rows","7");
		ctrRemoveNode(opt,"/ep/cfg/SecPolicies");
		if(ctrMkNode("table",opt,-1,"/ep/cfg/secPlc",cfg("SecPolicies").fld().descr(),RWRWR_,"root",SPRT_ID,1,"s_com","add,del"))
		{
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0",_("Policy"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","select","sel_list","None;Basic128Rsa15;Basic256");
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1",_("Message mode"),RWRWR_,"root",SPRT_ID,4,"tp","dec","dest","select","sel_id","1;2;3","sel_list",_("None;Sign;Sign&Encrypt"));
		}
	    }
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ep/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/ep/st/en_st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(atoi(opt->text().c_str()));
    }
    else if(a_path == "/ep/st/db")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/ep/cfg/ls_itr" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for(int i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path == "/ep/cfg/secPlc")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    XMLNode *n_pol	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0","",RWRWR_);
	    XMLNode *n_mm	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1","",RWRWR_);
	    ResAlloc res(nodeRes(), false);
	    for(int i_p = 0; i_p < mSec.size(); i_p++)
	    {
		if(n_pol)	n_pol->childAdd("el")->setText(mSec[i_p].policy);
		if(n_mm)	n_mm->childAdd("el")->setText(TSYS::int2str(mSec[i_p].messageMode));
	    }
	    return;
	}
	ResAlloc res(nodeRes(), true);
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{ mSec.push_back(SecuritySetting("None",OPCEndPoint::None)); modif(); return; }
	int row = atoi(opt->attr("row").c_str());
	if(row < 0 || row >= mSec.size())
	    throw TError(nodePath().c_str(),_("No present seleted row."));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))
	{ mSec.erase(mSec.begin()+row); modif(); return; }
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    int col = atoi(opt->attr("col").c_str());
	    if(col == 0)	mSec[row].policy = opt->text();
	    else if(col == 1)	mSec[row].messageMode = (OPCEndPoint::MessageSecurityMode)atoi(opt->text().c_str());
	    modif();
	}
    }
    else if(a_path.compare(0,7,"/ep/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SPRT_ID,RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}


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
#include <sys/stat.h>
#include <fcntl.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <openssl/rand.h>

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
TProt::TProt( string name ) : mSecCnlIdLast(1)
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

int TProt::chnlOpen( const string &iEp, int32_t lifeTm )
{
    ResAlloc res( nodeRes(), true );
    do
    {
	if( !(++mSecCnlIdLast) ) mSecCnlIdLast = 2;
    } while( mSecCnl.find(mSecCnlIdLast) != mSecCnl.end() );

    mSecCnl[mSecCnlIdLast] = SecCnl(iEp,1,lifeTm);
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

		printf("TEST 10: HELLO request:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());

		//> Send request
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );

		int off = 4;
		if( rez.size() < 8 || rez.size() > 4096 || iNu(rez,off,4) != rez.size() )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTooLarge,_("Respond size is not coincedence."));
		else if( rez.compare(0,4,"ERRF") == 0 )
		    err = TSYS::strMess("0x%x:%s",iNu(rez,off,4),iS(rez,off).c_str());
		else if( rez.compare(0,4,"ACKF") != 0 )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTypeInvalid,_("Respond don't acknowledge."));
		else
		{
		    printf("TEST 10a: HELLO response:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
		    iNu(rez,off,4);			//Protocol version
		    iNu(rez,off,4);			//Recive buffer size
		    iNu(rez,off,4);			//Send buffer size
		    iNu(rez,off,4);			//Max message size
		    iNu(rez,off,4);			//Max chunk count
		}
	    }
	    else if( io.attr("id") == "OPN" )
	    {
		rez.reserve( 200 );
		rez.append( "OPNF" );			//OpenSecureChannel message type
		oNu(rez,0,4);				//Message size
		oNu(rez,0,4);				//Secure channel identifier
		if( io.attr("SecPolicy") == "None" )
		{
							//> Security Header
		    oS(rez,"http://opcfoundation.org/UA/SecurityPolicy#"+io.attr("SecPolicy"));	//Security policy URI
		    oS(rez,"");				//SenderCertificate
		    oS(rez,"");				//ReceiverCertificateThumbprint
							//> Sequence header
		    oNu(rez,atoi(io.attr("SeqNumber").c_str()),4);	//Sequence number
		    oNu(rez,atoi(io.attr("SeqReqId").c_str()),4);	//RequestId
							//> Extension body object
		    oNodeId(rez,NodeId(OpcUa_OpenSecureChannelRequest));	//TypeId
							//>> Request Header
		    oNodeId(rez,0);			//Session AuthenticationToken
		    oTm(rez,TSYS::curTime());		//timestamp
		    oN(rez,atoi(io.attr("ReqHandle").c_str()),4);	//requestHandle
		    oNu(rez,0,4);			//returnDiagnostics
		    oS(rez,"");				//auditEntryId
		    oNu(rez,0,4);			//timeoutHint
							//>>> Extensible parameter
		    oNodeId(rez,0);			//TypeId (0)
		    oNu(rez,0,1);			//Encoding
							//>>>> Standard request
		    oNu(rez,OpcUa_ProtocolVersion,4);	//ClientProtocolVersion
		    oNu(rez,0,4);			//RequestType
		    oNu(rez,1,4);			//SecurityMode
		    oS(rez,"\000");			//ClientNonce
		    oN(rez,atoi(io.attr("SecLifeTm").c_str()),4);	//RequestedLifetime
		    oNu(rez,rez.size(),4,4);		//> Real message size

		    printf("TEST 11: OPN request:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());

		    //> Send request
		    int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		    rez.assign( buf, resp_len );

		    int off = 4;
		    if( rez.size() < 8 || iNu(rez,off,4) != rez.size() )
			err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTooLarge,_("Respond size is not coincedence."));
		    else if( rez.compare(0,4,"ERRF") == 0 )
			err = TSYS::strMess("0x%x:%s",iNu(rez,off,4),iS(rez,off).c_str());
		    else if( rez.compare(0,4,"OPNF") != 0 )
			err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTypeInvalid,_("Respond don't acknowledge."));
		    else
		    {
			iNu(rez,off,4);			//Secure channel identifier
			iS(rez,off);			//Security policy URI
			iN(rez,off,4);			//SenderCertificateLength
			iN(rez,off,4);			//ReceiverCertificateThumbprintLength
			iNu(rez,off,4);			//Sequence number
			iNu(rez,off,4);			//RequestId
							//> Extension Object
			if( iNodeId(rez,off).numbVal() != OpcUa_OpenSecureChannelResponse )	//TypeId
			    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC_UA Bin", _("Respond's NodeId don't acknowledge") );
							//>> Body
							//>>> RespondHeader
			iTm(rez,off);			//timestamp
			iN(rez,off,4);			//requestHandle
			iN(rez,off,4);			//StatusCode
			iN(rez,off,1);			//serviceDiagnostics
			iS(rez,off);			//stringTable
							//>>> Extensible parameter
			iNodeId(rez,off);		//TypeId (0)
			iNu(rez,off,1);			//Encoding
							//>>>> Standard respond
			iNu(rez,off,4);			//ServerProtocolVersion
			io.setAttr("SecChnId",TSYS::uint2str(iNu(rez,off,4)));		//Secure channel identifier
			io.setAttr("SecTokenId",TSYS::uint2str(iNu(rez,off,4)));	//TokenId
			iTm(rez,off);			//CreatedAt
			io.setAttr("SecLifeTm",TSYS::int2str(iN(rez,off,4)));		//RevisedLifeTime
			iS(rez,off);			//nonce
		    }
		    printf("TEST 11a: OPN response:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
		}
		else err = TSYS::strMess("0x%x:%s",OpcUa_BadSecurityPolicyRejected,_("Security policy isn't supported."));
	    }
	    else if( io.attr("id") == "CLO" )
	    {
		rez.reserve( 200 );
		rez.append( "CLOF" );			//OpenSecureChannel close
		oNu(rez,0,4);				//Message size
		oNu(rez,atoi(io.attr("SecChnId").c_str()),4);	//Secure channel identifier
		oNu(rez,atoi(io.attr("SecTokenId").c_str()),4);	//TokenId
							//> Sequence header
		uint32_t SeqNumber = atoi(io.attr("SeqNumber").c_str())+1;
		uint32_t SeqReqId = atoi(io.attr("SeqReqId").c_str())+1;
		oNu(rez,SeqNumber,4);			//Sequence number
		oNu(rez,SeqReqId,4);			//RequestId
							//> Extension body object
		oNodeId(rez,OpcUa_CloseSecureChannelRequest);	//TypeId request: CloseSecureChannel
							//>> Request Header
		oNodeId(rez,0);				//Session AuthenticationToken
		oTm(rez,TSYS::curTime());		//timestamp
		oN(rez,0,4);				//requestHandle
		oNu(rez,0,4);				//returnDiagnostics
		oS(rez,"");				//auditEntryId
		oNu(rez,0,4);				//timeoutHint
							//>>> Extensible parameter
		oNodeId(rez,0);				//TypeId (0)
		oNu(rez,0,1);				//Encoding
		oNu(rez,rez.size(),4,4);		//> Real message size

		//> Parameters clear
		io.setAttr("SecChnId",""); io.setAttr("SecTokenId","");
		io.setAttr("SeqNumber",""); io.setAttr("SeqReqId","");

		printf("TEST 12: CLO request:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());

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
		    oS(mReq,io.attr("EndPoint"));	//endpointUrl
		    oS(mReq,"");			//localeIds []
		    oS(mReq,"");			//serverUris []
		}
		else if( io.attr("id") == "GetEndpoints" )
		{
		    iTpId = OpcUa_GetEndpointsRequest;
		    oS(mReq,io.attr("EndPoint"));	//endpointUrl
		    oS(mReq,"");			//localeIds []
		    oS(mReq,"");			//profileUris []
		}
		else if( io.attr("id") == "CreateSession" )
		{
		    iTpId = OpcUa_CreateSessionRequest;
							//> clientDescription (Application Description)
		    oS(mReq,"");			//applicationUri
		    oS(mReq,"");			//productUri
		    oSl(mReq,"");			//applicationName
		    oNu(mReq,1,4);			//applicationType (CLIENT_1)
		    oS(mReq,"");			//gatewayServerUri
		    oS(mReq,"");			//discoveryProfileUri
		    oS(mReq,"");			//discoveryUrls

		    oS(mReq,"");			//serverUri
		    oS(mReq,io.attr("EndPoint"));	//endpointUrl
		    oS(mReq,"OpenSCADA station "+SYS->id());	//sessionName
		    oS(mReq,string(16,0)+string(4,0xFF)+string(12,0));	//clientNonce
		    oS(mReq,certPEM2DER(io.childGet("ClientCert")->text()));	//clientCertificate
		    oR(mReq,atof(io.attr("sesTm").c_str()),8);	//Requested SessionTimeout, ms
		    oNu(mReq,0x1000000,4);		//maxResponse MessageSize
		    io.childClear();
		}
		else if( io.attr("id") == "ActivateSession" )
		{
		    iTpId = OpcUa_ActivateSessionRequest;
							//> clientSignature
		    oS(mReq,"");			//signature
		    oS(mReq,"");			//algorithm

		    oNu(mReq,0,4);			//clientSoftwareCertificates []
							//> localeIds []
		    oNu(mReq,1,4);			//List number 1
		    oS(mReq,"en");			//localeId
							//> userIdentityToken
		    oNodeId(mReq,321);			//TypeId
		    oNu(mReq,1,1);			//Encode
		    oNu(mReq,4,4);			//Length
		    oS(mReq,"");			//policyId
							//> userTokenSignature
		    oS(mReq,"");			//signature
		    oS(mReq,"");			//algorithm
		}
		else if( io.attr("id") == "CloseSession" )
		{
		    iTpId = OpcUa_CloseSessionRequest;
		    oNu(mReq,1,1);			//deleteSubscriptions
		}
		else if( io.attr("id") == "Read" )
		{
		    iTpId = OpcUa_ReadRequest;
		    oR(mReq,atof(io.attr("maxAge").c_str()),8);	//maxAge 0 ms
		    oNu(mReq,atoi(io.attr("timestampsToReturn").c_str()),4);//timestampsTo Return (SERVER_1)
							//> nodesToRead []
		    oNu(mReq,io.childSize(),4);		//nodes
		    for( int i_n = 0; i_n < io.childSize(); i_n++ )
		    {
			oNodeId(mReq,NodeId::fromAddr(io.childGet(i_n)->attr("nodeId")));	//nodeId
			oNu(mReq,strtoul(io.childGet(i_n)->attr("attributeId").c_str(),NULL,0),4);	//attributeId (Value)
			oS(mReq,"");			//indexRange
			oSqlf(mReq,"");			//dataEncoding
		    }
		}
		else if( io.attr("id") == "Browse" )
		{
		    iTpId = OpcUa_BrowseRequest;
							//> view
		    oNodeId(mReq,0);			//viewId (0)
		    oTm(mReq,0);			//timestamp
		    oNu(mReq,0,4);			//viewVersion

		    oNu(mReq,100,4);			//requestedMax ReferencesPerNode
							//> nodesToBrowse
		    oNu(mReq,io.childSize(),4);			//Nodes 1
		    for( int i_n = 0; i_n < io.childSize(); i_n++ )
		    {
			oNodeId(mReq,NodeId::fromAddr(io.childGet(i_n)->attr("nodeId")));	//nodeId (RootFolder)
			oNu(mReq,strtoul(io.childGet(i_n)->attr("browseDirection").c_str(),NULL,0),4);	//browseDirection (FORWARD_0)
			oNodeId(mReq,33);		//referenceTypeId (HierarchicalReferences)
			oNu(mReq,1,1);			//includeSubtypes (true)
			oNu(mReq,0,4);			//nodeClassMask ( all NodeClasses )
			oNu(mReq,0x3f,4);		//resultMask ( all )
		    }
		}
		else throw TError(OpcUa_BadNotSupported,"OPC UA Bin",_("Request '%s' isn't supported."),io.attr("id").c_str());

		rez.reserve( 200 );
		rez.append( "MSGF" );			//SecureChannel message
		oNu(rez,0,4);				//Message size
		oNu(rez,atoi(io.attr("SecChnId").c_str()),4);		//Secure channel identifier
		oNu(rez,atoi(io.attr("SecTokenId").c_str()),4);		//TokenId
							//> Sequence header
		uint32_t SeqNumber = atoi(io.attr("SeqNumber").c_str())+1;
		io.setAttr("SeqNumber",TSYS::int2str(SeqNumber));
		uint32_t SeqReqId = atoi(io.attr("SeqReqId").c_str())+1;
		io.setAttr("SeqReqId",TSYS::int2str(SeqReqId));
		oNu(rez,SeqNumber,4);			//Sequence number
		oNu(rez,SeqReqId,4);			//RequestId
							//> Extension body object
		oNodeId(rez,iTpId);			//TypeId request
							//>> Request Header
		oNodeId(rez,atoi(io.attr("authTokenId").c_str()));	//Session AuthenticationToken
		oTm(rez,TSYS::curTime());		//timestamp
		uint32_t ReqHandle = atoi(io.attr("ReqHandle").c_str())+1;
		io.setAttr("ReqHandle",TSYS::int2str(ReqHandle));
		oN(rez,ReqHandle,4);			//requestHandle
		oNu(rez,0,4);				//returnDiagnostics
		oS(rez,"");				//auditEntryId
		oNu(rez,3000,4);			//timeoutHint
							//>>> Extensible parameter
		oNodeId(rez,0);				//TypeId (0)
		oNu(rez,0,1);				//Encoding
		rez.append(mReq);			//Same request
		oNu(rez,rez.size(),4,4);		//> Real message size

		printf("TEST 13: Request '%s':\n%s\n",io.attr("id").c_str(),TSYS::strDecode(rez,TSYS::Bin).c_str());

		//> Send request and wait respond
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );
		int off = 4;
		for( ; rez.size() < 8 || rez.size() < iNu(rez,off,4); off = 4 )
		{
		    resp_len = tro.messIO( NULL, 0, buf, sizeof(buf), 0, true );
		    rez.append( buf, resp_len );
		}

		printf("TEST 13a: Response '%s':\n%s\n",io.attr("id").c_str(),TSYS::strDecode(rez,TSYS::Bin).c_str());

		off = 4;
		if( rez.size() < 8 || iNu(rez,off,4) != rez.size() )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTooLarge,_("Respond size is not coincedence."));
		else if( rez.compare(0,4,"ERRF") == 0 )
		    err = TSYS::strMess("0x%x:%s",iNu(rez,off,4),iS(rez,off).c_str());
		else if( rez.compare(0,4,"MSGF") != 0 )
		    err = TSYS::strMess("0x%x:%s",OpcUa_BadTcpMessageTypeInvalid,_("Respond don't acknowledge."));
		else
		{
		    iNu(rez,off,4);				//Secure channel identifier
		    iNu(rez,off,4);				//Symmetric Algorithm Security Header : TokenId
								//> Sequence header
		    iNu(rez,off,4);				//Sequence number
		    iNu(rez,off,4);				//RequestId
								//> Extension Object
		    int oTpId = iNodeId(rez,off).numbVal();		//TypeId
								//>> Body
								//>>> RespondHeader
		    iTm(rez,off);				//timestamp
		    iN(rez,off,4);				//requestHandle
		    uint32_t stCode = iN(rez,off,4);		//StatusCode
		    iN(rez,off,1);				//serviceDiagnostics
		    iS(rez,off);				//stringTable
								//>>> Extensible parameter
		    iNodeId(rez,off);				//TypeId (0)
		    iNu(rez,off,1);				//Encoding

		    switch( oTpId )
		    {
			case OpcUa_FindServersResponse:
			{
			    if( iTpId != OpcUa_FindServersRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    int AppDescrNub = iNu(rez,off,4);		//List items
			    for( int i_l = 0; i_l < AppDescrNub; i_l++ )
			    {
				XMLNode *ad = io.childAdd("ApplicationDescription");
				ad->setAttr("applicationUri",iS(rez,off));	//applicationUri
				ad->setAttr("productUri",iS(rez,off));		//productUri
				ad->setAttr("applicationName",iSl(rez,off));	//applicationName
				ad->setAttr("applicationType",TSYS::uint2str(iNu(rez,off,4)));	//applicationType
				ad->setAttr("gatewayServerUri",iS(rez,off));	//gatewayServerUri
				ad->setAttr("discoveryProfileUri",iS(rez,off));	//discoveryProfileUri
									//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);	//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    ad->childAdd("discoveryUrl")->setText(iS(rez,off));	//discoveryUrl
			    }
			    break;
			}
			case OpcUa_GetEndpointsResponse:
			{
			    if( iTpId != OpcUa_GetEndpointsRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    int EndpointDescrNub = iNu(rez,off,4);	//List items
			    for( int i_l = 0; i_l < EndpointDescrNub; i_l++ )
			    {
				XMLNode *xep = io.childAdd("EndpointDescription");
									//>>> EndpointDescription
				xep->setAttr("endpointUrl",iS(rez,off));	//endpointUrl
									//>>>> server (ApplicationDescription)
				xep->setAttr("applicationUri",iS(rez,off));	//applicationUri
				xep->setAttr("productUri",iS(rez,off));	//productUri
				xep->setAttr("applicationName",iSl(rez,off));	//applicationName
				xep->setAttr("applicationType",TSYS::uint2str(iNu(rez,off,4)));	//applicationType
				xep->setAttr("gatewayServerUri",iS(rez,off));	//gatewayServerUri
				xep->setAttr("discoveryProfileUri",iS(rez,off));//discoveryProfileUri
									//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);	//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    xep->childAdd("discoveryUrl")->setText(iS(rez,off));//discoveryUrl
				xep->childAdd("serverCertificate")->setText(iS(rez,off));	//>>> serverCertificate
				xep->setAttr("securityMode",TSYS::uint2str(iNu(rez,off,4)));	//securityMode
				xep->setAttr("securityPolicyUri",iS(rez,off));	//securityPolicyUri
									//>>>> userIdentityTokens
				int userIdentityTokensN = iNu(rez,off,4);//List items
				for( int i_l2 = 0; i_l2 < userIdentityTokensN; i_l2++ )
				{
				    XMLNode *xit = xep->childAdd("userIdentityToken");
				    xit->setAttr("policyId",iS(rez,off));	//policyId
				    xit->setAttr("tokenType",TSYS::uint2str(iNu(rez,off,4)));	//tokenType
				    xit->setAttr("issuedTokenType",iS(rez,off));	//issuedTokenType
				    xit->setAttr("issuerEndpointUrl",iS(rez,off));	//issuerEndpointUrl
				    xit->setAttr("securityPolicyUri",iS(rez,off));	//securityPolicyUri
				}
				xep->setAttr("transportProfileUri",iS(rez,off));	//transportProfileUri
				xep->setAttr("securityLevel",TSYS::uint2str(iNu(rez,off,1)));	//securityLevel
			    }
			    break;
			}
			case OpcUa_CreateSessionResponse:
			{
			    if( iTpId != OpcUa_CreateSessionRequest )
				throw TError(OpcUa_BadTcpMessageTypeInvalid,"OPC_UA Bin",_("Respond's NodeId don't acknowledge"));
			    io.setAttr("sesId",TSYS::int2str(iNodeId(rez,off).numbVal()));	//sessionId
			    io.setAttr("authTokenId",TSYS::int2str(iNodeId(rez,off).numbVal()));//authentication Token
			    io.setAttr("sesTm",TSYS::real2str(iR(rez,off,8)));		//revisedSession Timeout, ms
			    iS(rez,off);				//serverNonce
			    iS(rez,off);				//serverCertificate
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
			    iS(rez,off);				//signature
			    iS(rez,off);				//algorithm

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
			    int resN = iNu(rez,off,4);			//Numbers
			    for( int i_r = 0; i_r < resN; i_r++ )
			    {
									//>> value
				iNu(rez,off,1);				//Encoding Mask
									//>>> Variant
				iNu(rez,off,1);				//Encoding Mask
				iN(rez,off,4);				//Value (int32)
				iTm(rez,off);				//sourceTimestamp
				iTm(rez,off);				//serverTimestamp
				iS(rez,off);				//diagnosticInfos []
			    }
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
				rno->setAttr("statusCode",TSYS::uint2str(iNu(rez,off,4)));	//statusCode
				iS(rez,off);				//continuationPoint
									//>> References []
				int refN = iNu(rez,off,4);		//Numbers
				for( int i_rf = 0; i_rf < refN; i_rf++ )
				{
				    XMLNode *bno = rno->childAdd("bNode");
				    bno->setAttr("referenceTypeId",TSYS::uint2str(iNodeId(rez,off).numbVal()));	//referenceTypeId
				    bno->setAttr("isForward",iNu(rez,off,1)?"1":"0");				//isForward
				    bno->setAttr("nodeId",iNodeId(rez,off).toAddr());				//nodeId
				    bno->setAttr("browseName",iSqlf(rez,off));					//browseName
				    bno->setAttr("displayName",iSl(rez,off));					//displayName
				    bno->setAttr("nodeClass",TSYS::uint2str(iNu(rez,off,4)));			//nodeClass
				    bno->setAttr("typeDefinition",TSYS::uint2str(iNodeId(rez,off).numbVal()));	//typeDefinition
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
	case 0x00:	return NodeId(iNu(rb,off,1));
	case 0x01:
	{
	    uint8_t ns = iNu(rb,off,1);
	    return NodeId(iNu(rb,off,2),ns);
	}
	case 0x02:
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iNu(rb,off,4),ns);
	}
	case 0x03:
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns);
	}
    }
    throw TError(OpcUa_BadDecodingError,modPrt->nodePath().c_str(),_("NodeId type %d error or don't support."),enc);
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
    if( val.type() == NodeId::Numeric )
    {
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
	    buf += (char)0x01;
	    oNu(buf,val.ns(),2);
	    oNu(buf,val.numbVal(),4);
	}
    }
    else if( val.type() == NodeId::String )
    {
	buf += (char)0x03;
	oNu(buf,val.ns(),2);
	oS(buf,val.strVal());
    }
}

void TProt::oTm( string &buf, long long val )
{
    int64_t tmStamp = 10ll*(val+11644473600000000ll);
    buf.append( (char*)&tmStamp, sizeof(tmStamp) );
}

string TProt::applicationUri( )		{ return SYS->host()+"/OpenSCADA/DAQ.OPC_UA"; }

string TProt::productUri( )		{ return "OpenSCADA/DAQ.OPC_UA"; }

string TProt::applicationName( )	{ return "OpenScadaOpcServer@"+SYS->host(); }

string TProt::certPEM2DER( const string &spem )
{
    string rez = "";
    char err[255];

    if( spem.empty() ) return rez;

    BIO *bm = BIO_new(BIO_s_mem());
    if( !bm )
    {
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("BIO_new error: %s"),err);
    }
    if( BIO_write(bm,spem.data(),spem.size()) != spem.size() )
    {
	BIO_free_all(bm);
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("BIO_write error: %s"),err);
    }
    X509 *x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if( !x )
    {
	BIO_free_all(bm);
	ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	throw TError(mod->nodePath().c_str(),_("PEM_read_bio_X509_AUX error: %s"),err);
    }

    int len = i2d_X509(x,NULL);
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

    BIO_free_all(bm);
    X509_free(x);

    return rez;
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/ep_",_("End point"),0664,"root","Protocol",2,"idm","1","idSz","20");
	if( ctrMkNode("area",opt,0,"/ep",_("End points")) )
	    ctrMkNode("list",opt,-1,"/ep/ep",_("End points"),0664,"root","Protocol",5,"tp","br","idm","1","s_com","add,del","br_pref","ep_","idSz","20");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/br/ep_" || a_path == "/ep/ep" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Protocol",SEQ_RD) )
	{
	    vector<string> lst;
	    epList(lst);
	    for( unsigned i_f=0; i_f < lst.size(); i_f++ )
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(epAt(lst[i_f]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","Protocol",SEQ_WR) )
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    epAdd(vid); epAt(vid).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","Protocol",SEQ_WR) )	chldDel(mEndPnt,opt->attr("id"),-1,1);
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

NodeId::NodeId( const string istr, uint16_t ins ) : mTp(NodeId::Numeric), mNs(ins)
{
    setStrVal(istr);
}

NodeId::~NodeId( )
{
    if( type() == NodeId::String ) delete str;
}

uint32_t NodeId::numbVal( ) const
{
    if( type() != NodeId::Numeric ) return 0;
    return numb;
}

string NodeId::strVal( ) const
{
    if( type() != NodeId::String ) return "";
    return *str;
}

void NodeId::setNumbVal( uint32_t in )
{
    if( type() == NodeId::String ) delete str;
    mTp = NodeId::Numeric;
    numb = in;
}

void NodeId::setStrVal( const string istr )
{
    if( type() != NodeId::String ) str = new string(istr);
    mTp = NodeId::String;
    *str = istr;
}

NodeId NodeId::fromAddr( const string &strAddr )
{
    uint16_t ns = strtoul(TSYS::strParse(strAddr,0,":").c_str(),NULL,0);
    string vl = TSYS::strParse(strAddr,1,":");
    bool isStr = false;
    for( int i_s = 0; i_s < vl.size() && !isStr; i_s++ )
	if( !isdigit(vl[i_s]) ) isStr = true;
    if( isStr ) return NodeId(vl,ns);
    return NodeId(strtoul(vl.c_str(),NULL,0),ns);
}

string NodeId::toAddr( )
{
    if( type() == NodeId::Numeric )	return TSYS::uint2str(ns())+":"+TSYS::uint2str(numbVal());
    return TSYS::uint2str(ns())+":"+strVal();
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

bool TProtIn::mess( const string &reqst, string &out, const string &sender )
{
    uint32_t mSz;
    bool KeepAlive = false;
    int off = 0;

    //> Continue for full request
    if( mNotFull )	{ mBuf = mBuf+reqst; mNotFull = false; }
    else mBuf = reqst;  //Save request to bufer

    string &rb = mBuf;

    out = "";
    if( rb.size() <= 0 ) return mNotFull;

#if OSC_DEBUG >= 3
    mess_debug(nodePath().c_str(),_("Content:\n%s"),rb.c_str());
#endif

    //printf("TEST 00: Request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

    off = 4;
    if( rb.size() < 8 || rb.size() < TProt::iNu(rb,off,4) ) return (mNotFull=true);

    try
    {
	//> Check for hello message type
	if( rb.compare(0,4,"HELF") == 0 )
	{
	    if( rb.size() > 4096 )	{ out = mkError(OpcUa_BadTcpMessageTooLarge); return false; }

	    printf( "TEST 00: Hello request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    off = 8;
	    TProt::iNu(rb,off,4);			//Protocol version
	    TProt::iNu(rb,off,4);			//Recive buffer size
	    TProt::iNu(rb,off,4);			//Send buffer size
	    TProt::iNu(rb,off,4);			//Max message size
	    TProt::iNu(rb,off,4);			//Max chunk count
	    string EndpntURL = TProt::iS(rb,off);	//EndpointURL

	    //>> Find accessable endpoint
	    vector<string> epLs;
	    owner().epList(epLs);
	    int i_ep;
	    for( i_ep = 0; i_ep < epLs.size(); i_ep++ )
		if( owner().epAt(epLs[i_ep]).at().enableStat() )
		    break;
	    if( i_ep >= epLs.size() )	{ out = mkError(OpcUa_BadTcpEndpointUrlInvalid); return false; }

	    //> Prepare acknowledge message
	    out.reserve( 28 );
	    out.append( "ACKF" );			//Acknowledge message type
	    TProt::oNu(out,28,4);			//Message size
	    TProt::oNu(out,OpcUa_ProtocolVersion,4);	//Protocol version
	    TProt::oNu(out,OpcUa_ReciveBufferSize,4);	//Recive buffer size
	    TProt::oNu(out,OpcUa_SendBufferSize,4);	//Send buffer size
	    TProt::oNu(out,OpcUa_MaxMessageSize,4);	//Max message size
	    TProt::oNu(out,OpcUa_MaxChunkCount,4);	//Max chunk count

	    printf( "TEST 00a: Hello response:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());

	    return true;
	}
	//> Check for Open SecureChannel message type
	if( rb.compare(0,4,"OPNF") == 0 )
	{
	    printf( "TEST 01: Open SecureChannel request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    off = 8;
	    uint32_t secChnId = TProt::iNu(rb,off,4);		//Secure channel identifier
								//> Security Header
	    string secPlcURI = TProt::iS(rb,off);		//Security policy URI
	    //>> Find server with that policy
	    string secPlc = TSYS::strParse(secPlcURI,1,"#");
	    vector<string> epLs;
	    owner().epList(epLs);
	    int i_epOk = -1;
	    for( int i_ep = 0; i_epOk < 0 && i_ep < epLs.size(); i_ep++ )
	    {
		AutoHD<OPCEndPoint> ep = owner().epAt(epLs[i_ep]);
		if( !ep.at().enableStat() ) continue;
		for( int i_s = 0; i_epOk < 0 && i_s < ep.at().secSize(); i_s++ )
		    if( ep.at().secPolicy(i_s) == secPlc )
			i_epOk = i_ep;
	    }
	    if( i_epOk < 0 ) { out = mkError(OpcUa_BadSecurityPolicyRejected); return false; }

	    if( secPlc == "None" )
	    {
		TProt::iN(rb,off,4);				//SenderCertificateLength
		TProt::iN(rb,off,4);				//ReceiverCertificateThumbprintLength
								//> Sequence header
		uint32_t secNumb = TProt::iNu(rb,off,4);	//Sequence number
		uint32_t reqId = TProt::iNu(rb,off,4);		//RequestId
								//> Extension body object
		if( TProt::iNodeId(rb,off).numbVal() != OpcUa_OpenSecureChannelRequest )	//TypeId
		    throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Requested OpenSecureChannel NodeId don't acknowledge") );
								//>> Request Header
		TProt::iVal(rb,off,2);				//Session AuthenticationToken
		TProt::iTm(rb,off);				//timestamp
		int32_t rqHndl = TProt::iN(rb,off,4);		//requestHandle
		TProt::iNu(rb,off,4);				//returnDiagnostics
		TProt::iS(rb,off);				//auditEntryId
		TProt::iNu(rb,off,4);				//timeoutHint
								//>>> Extensible parameter
		TProt::iNodeId(rb,off);				//TypeId (0)
		TProt::iNu(rb,off,1);				//Encoding
								//>>>> Standard request
		TProt::iNu(rb,off,4);				//ClientProtocolVersion
		TProt::iNu(rb,off,4);				//RequestType
		TProt::iNu(rb,off,4);				//SecurityMode
		TProt::iS(rb,off);				//ClientNonce
		int32_t reqLifeTm = TProt::iN(rb,off,4);	//RequestedLifetime

		uint32_t chnlId = owner().chnlOpen(epLs[i_epOk],reqLifeTm);

		//> Prepare respond message
		out.reserve( 200 );
		out.append( "OPNF" );				//OpenSecureChannel message type
		TProt::oNu(out,0,4);				//Message size
		TProt::oNu(out,chnlId,4);			//Secure channel identifier
		TProt::oS(out,secPlcURI);			//Security policy URI
		TProt::oN(out,-1,4);				//SenderCertificateLength
		TProt::oN(out,-1,4);				//ReceiverCertificateThumbprintLength
		TProt::oNu(out,secNumb,4);			//Sequence number
		TProt::oNu(out,reqId,4);			//RequestId
								//> Extension Object
		TProt::oNodeId(out,NodeId(OpcUa_OpenSecureChannelResponse));	//TypeId
								//>> Body
								//>>> RespondHeader
		TProt::oTm(out,TSYS::curTime());		//timestamp
		TProt::oN(out,rqHndl,4);			//requestHandle
		TProt::oN(out,0,4);				//StatusCode
		TProt::oN(out,0,1);				//serviceDiagnostics
		TProt::oS(out,"");				//stringTable
								//>>> Extensible parameter
		TProt::oNodeId(out,0);				//TypeId (0)
		TProt::oNu(out,0,1);				//Encoding
								//>>>> Standard respond
		TProt::oNu(out,OpcUa_ProtocolVersion,4);	//ServerProtocolVersion
		TProt::oNu(out,chnlId,4);			//Secure channel identifier
		TProt::oNu(out,owner().chnlGet(chnlId).TokenId,4);	//TokenId
		TProt::oTm(out,owner().chnlGet(chnlId).tCreate);	//CreatedAt
		TProt::oN(out,owner().chnlGet(chnlId).tLife,4);	//RevisedLifeTime (600000, minimum)
		TProt::oS(out,"\001");				//nonce
		TProt::oNu(out,out.size(),4,4);			//Real message size

		printf("TEST 01a: Open sec respond:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());

		return true;
	    }
	}
	//> Check for Close SecureChannel message type
	if( rb.compare(0,4,"CLOF") == 0 )
	{
	    printf( "TEST 01: Close SecureChannel request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    off = 8;
	    uint32_t secId = TProt::iNu(rb,off,4);	//Secure channel identifier
	    uint32_t tokId = TProt::iNu(rb,off,4);	//TokenId
							//> Sequence header
	    TProt::iNu(rb,off,4);			//Sequence number
	    TProt::iNu(rb,off,4);			//RequestId
							//> Extension body object
	    if( TProt::iNodeId(rb,off).numbVal() != OpcUa_CloseSecureChannelRequest )	//TypeId
		throw TError( OpcUa_BadTcpMessageTypeInvalid, "OPC UA Bin", _("Requested OpenSecureChannel NodeId don't acknowledge") );
							//>> Request Header
	    TProt::iNodeId(rb,off);			//Session AuthenticationToken
	    TProt::iTm(rb,off);				//timestamp
	    TProt::iN(rb,off,4);			//requestHandle
	    TProt::iNu(rb,off,4);			//returnDiagnostics
	    TProt::iS(rb,off);				//auditEntryId
	    TProt::iNu(rb,off,4);			//timeoutHint
							//>>> Extensible parameter
	    TProt::iNodeId(rb,off);			//TypeId (0)
	    TProt::iNu(rb,off,1);			//Encoding

	    owner().chnlClose( secId );

	    //> No respond. Close socket
	    return false;
	}
	//> Check for SecureChannel message type
	if( rb.compare(0,4,"MSGF") == 0 )
	{
	    printf( "TEST 02: SecureChannel message:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    off = 8;
	    uint32_t stCode = 0;
	    uint32_t secId = TProt::iNu(rb,off,4);	//Secure channel identifier
	    uint32_t tokId = TProt::iNu(rb,off,4);	//TokenId
	    SecCnl scHd = owner().chnlGet(secId);
	    //>> Secure channel and token check
	    if( !scHd.TokenId )	throw TError( OpcUa_BadSecureChannelClosed, "OPC UA Bin", _("Secure channel closed") );
	    if( scHd.TokenId != tokId )	throw TError( OpcUa_BadSecureChannelTokenUnknown, "OPC UA Bin", _("Secure channel unknown") );
	    AutoHD<OPCEndPoint> wep = owner().epAt(scHd.endPoint);
							//> Sequence header
	    uint32_t seqN = TProt::iNu(rb,off,4);	//Sequence number
	    uint32_t reqId = TProt::iNu(rb,off,4);	//RequestId
							//> Extension body object
	    int reqTp = TProt::iNodeId(rb,off).numbVal();	//TypeId request
							//>> Request Header
	    uint16_t sesTokId = TProt::iNodeId(rb,off).numbVal();	//Session AuthenticationToken
	    //>> Session check
	    if( sesTokId && !wep.at().sessActivate(sesTokId,secId,reqTp!=OpcUa_ActivateSessionRequest) )
	    { stCode = OpcUa_BadSessionIdInvalid; reqTp = OpcUa_ServiceFault; }
	    TProt::iTm(rb,off);				//timestamp
	    int32_t reqHndl = TProt::iN(rb,off,4);	//requestHandle
	    TProt::iNu(rb,off,4);			//returnDiagnostics
	    TProt::iS(rb,off);				//auditEntryId
	    TProt::iNu(rb,off,4);			//timeoutHint
							//>>> Extensible parameter
	    TProt::iNodeId(rb,off);			//TypeId (0)
	    TProt::iNu(rb,off,1);			//Encoding

	    //> Prepare respond message
	    string respEp;
	    switch( reqTp )
	    {
		case OpcUa_FindServersRequest:
		{
		    //>> Request
		    TProt::iS(rb,off);			//endpointUrl
		    TProt::iS(rb,off);			//localeIds []
		    TProt::iS(rb,off);			//serverUris []

		    printf( "TEST 02a\n" );

		    //>> Respond
		    reqTp = OpcUa_FindServersResponse;
		    TProt::oNu(respEp,1,4);		//ApplicationDescription list items
							//>>>> ApplicationDescription 1
		    TProt::oS(respEp,TProt::applicationUri());		//applicationUri
		    TProt::oS(respEp,TProt::productUri());		//productUri
		    TProt::oSl(respEp,TProt::applicationName(),"en");	//applicationName
		    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
		    TProt::oS(respEp,"");		//gatewayServerUri
		    TProt::oS(respEp,"");		//discoveryProfileUri
							//>>>> discoveryUrls
		    vector<string> duLs;
		    owner().discoveryUrls(duLs);
		    TProt::oNu(respEp,duLs.size(),4);	//List items
		    for( int i_du = 0; i_du < duLs.size(); i_du++ )
			TProt::oS(respEp,duLs[i_du]);	//discoveryUrl
		    break;
		}
		case OpcUa_GetEndpointsRequest:
		{
		    //>> Request
		    TProt::iS(rb,off);			//endpointUrl
		    TProt::iS(rb,off);			//localeIds []
		    TProt::iS(rb,off);			//profileUris []

		    //>> Respond
		    reqTp = OpcUa_GetEndpointsResponse;

		    respEp.reserve(2000);
		    TProt::oNu(respEp,0,4);			//EndpointDescrNubers list items
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
			    TProt::oS(respEp,ep.at().url());	//endpointUrl
								//>>>> server (ApplicationDescription)
			    TProt::oS(respEp,TProt::applicationUri());		//applicationUri
			    TProt::oS(respEp,TProt::productUri());		//productUri
			    TProt::oSl(respEp,TProt::applicationName(),"en");	//applicationName
			    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
			    TProt::oS(respEp,"");		//gatewayServerUri
			    TProt::oS(respEp,"");		//discoveryProfileUri

								//>>>> discoveryUrls
			    vector<string> duLs;
			    owner().discoveryUrls(duLs);
			    TProt::oNu(respEp,duLs.size(),4);	//List items
			    for( int i_du = 0; i_du < duLs.size(); i_du++ )
				TProt::oS(respEp,duLs[i_du]);	//discoveryUrl

			    TProt::oS(respEp,TProt::certPEM2DER(ep.at().servCert()));	//>>> serverCertificate
			    TProt::oNu(respEp,ep.at().secMessageMode(i_sec),4);	//securityMode:MessageSecurityMode
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#"+ep.at().secPolicy(i_sec));	//securityPolicyUri

								//>>>> userIdentityTokens
			    TProt::oNu(respEp,2,4);		//List items
								//>>>> userIdentityToken 1
			    TProt::oS(respEp,"Anonymous");	//policyId
			    TProt::oNu(respEp,0,4);		//tokenType
			    TProt::oS(respEp,"");		//issuedTokenType
			    TProt::oS(respEp,"");		//issuerEndpointUrl
			    TProt::oS(respEp,"");		//securityPolicyUri
								//>>>> userIdentityToken 2
			    TProt::oS(respEp,"UserName");	//policyId
			    TProt::oNu(respEp,1,4);		//tokenType
			    TProt::oS(respEp,"");		//issuedTokenType
			    TProt::oS(respEp,"");		//issuerEndpointUrl
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

			    TProt::oS(respEp,"");		//transportProfileUri
			    TProt::oNu(respEp,0,1);		//securityLevel
			}
		    }
		    TProt::oNu(respEp,epCnt,4,0);			//EndpointDescrNubers list items

		    break;
		}
		case OpcUa_CreateSessionRequest:
		{
		    //>> Request
							//> clientDescription (Application Description)
		    TProt::iS(rb,off);			//applicationUri
		    TProt::iS(rb,off);			//productUri
		    TProt::iSl(rb,off);			//applicationName
		    TProt::iNu(rb,off,4);		//applicationType (CLIENT_1)
		    TProt::iS(rb,off);			//gatewayServerUri
		    TProt::iS(rb,off);			//discoveryProfileUri
		    TProt::iS(rb,off);			//discoveryUrls

		    TProt::iS(rb,off);			//serverUri
		    TProt::iS(rb,off);			//endpointUrl
		    string sessNm = TProt::iS(rb,off);	//sessionName
		    TProt::iS(rb,off);			//clientNonce
		    TProt::iS(rb,off);			//clientCertificate
		    double rStm = TProt::iR(rb,off,8);	//Requested SessionTimeout, ms
		    TProt::iNu(rb,off,4);		//maxResponse MessageSize

		    int sessId = wep.at().sessCreate( sessNm, rStm );

		    //>> Respond
		    reqTp = OpcUa_CreateSessionResponse;

		    respEp.reserve(2000);
		    TProt::oNodeId(respEp,sessId);	//sessionId
		    TProt::oNodeId(respEp,sessId);	//authentication Token
		    TProt::oR(respEp,wep.at().sessGet(sessId).tInact,8);	//revisedSession Timeout, ms
		    TProt::oS(respEp,"");		//serverNonce
		    TProt::oS(respEp,TProt::certPEM2DER(wep.at().servCert()));	//serverCertificate
							//> EndpointDescr []
		    int enpNumperPos = respEp.size();
		    TProt::oNu(respEp,0,4);			//EndpointDescrNubers list items
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
			    TProt::oS(respEp,ep.at().url());	//endpointUrl
								//>>>> server (ApplicationDescription)
			    TProt::oS(respEp,TProt::applicationUri());		//applicationUri
			    TProt::oS(respEp,TProt::productUri());		//productUri
			    TProt::oSl(respEp,TProt::applicationName(),"en");	//applicationName
			    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
			    TProt::oS(respEp,"");		//gatewayServerUri
			    TProt::oS(respEp,"");		//discoveryProfileUri

								//>>>> discoveryUrls
			    vector<string> duLs;
			    owner().discoveryUrls(duLs);
			    TProt::oNu(respEp,duLs.size(),4);	//List items
			    for( int i_du = 0; i_du < duLs.size(); i_du++ )
				TProt::oS(respEp,duLs[i_du]);	//discoveryUrl

			    TProt::oS(respEp,TProt::certPEM2DER(ep.at().servCert()));	//>>> serverCertificate
			    TProt::oNu(respEp,ep.at().secMessageMode(i_sec),4);	//securityMode:MessageSecurityMode
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#"+ep.at().secPolicy(i_sec));	//securityPolicyUri

								//>>>> userIdentityTokens
			    TProt::oNu(respEp,2,4);		//List items
								//>>>> userIdentityToken 1
			    TProt::oS(respEp,"Anonymous");	//policyId
			    TProt::oNu(respEp,0,4);		//tokenType
			    TProt::oS(respEp,"");		//issuedTokenType
			    TProt::oS(respEp,"");		//issuerEndpointUrl
			    TProt::oS(respEp,"");		//securityPolicyUri
								//>>>> userIdentityToken 2
			    TProt::oS(respEp,"UserName");	//policyId
			    TProt::oNu(respEp,1,4);		//tokenType
			    TProt::oS(respEp,"");		//issuedTokenType
			    TProt::oS(respEp,"");		//issuerEndpointUrl
			    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

			    TProt::oS(respEp,"");		//transportProfileUri
			    TProt::oNu(respEp,0,1);		//securityLevel
			}
		    }
		    TProt::oNu(respEp,epCnt,4,enpNumperPos);	//EndpointDescrNubers real list items

		    TProt::oS(respEp,"");			//serverSoftware Certificates []
								//> serverSignature
		    TProt::oS(respEp,"");			//signature
		    TProt::oS(respEp,"");			//algorithm

		    TProt::oNu(respEp,0,4);			//maxRequest MessageSize
		    break;
		}
		case OpcUa_ActivateSessionRequest:
		{
		    //>> Request
							//> clientSignature
		    TProt::iS(rb,off);			//signature
		    TProt::iS(rb,off);			//algorithm

		    TProt::iNu(rb,off,4);		//clientSoftwareCertificates []
							//> localeIds []
		    uint32_t ln = TProt::iNu(rb,off,4);	//List number
		    for( int i_l; i_l < ln; i_l++ )
			TProt::iS(rb,off);		//localeId
							//> userIdentityToken
		    int userIdTk = TProt::iNodeId(rb,off).numbVal();//TypeId
		    TProt::iNu(rb,off,1);		//Encode
		    TProt::iNu(rb,off,4);		//Length
		    switch( userIdTk )
		    {
			case 321:	//AnonymousIdentityToken
			    TProt::iS(rb,off);		//policyId
			    break;
			default:
			    throw TError(OpcUa_BadUnexpectedError,modPrt->nodePath().c_str(),_("No supported userIdentityToken '%d'."),userIdTk);
		    }
							//> userTokenSignature
		    TProt::iS(rb,off);			//signature
		    TProt::iS(rb,off);			//algorithm

		    //>> Respond
		    reqTp = OpcUa_ActivateSessionResponse;
		    respEp.reserve(100);
		    TProt::oS(respEp,"");		//serverNonce
		    TProt::oS(respEp,"");		//results []
		    TProt::oS(respEp,"");		//diagnosticInfos []
		    break;
		}
		case OpcUa_CloseSessionRequest:
		{
		    //>> Request
		    TProt::iNu(rb,off,1);		//deleteSubscriptions

		    //>> Respond
		    reqTp = OpcUa_CloseSessionResponse;
		    break;
		}
		case OpcUa_ReadRequest:
		{
		    //>> Request
		    TProt::iR(rb,off,8);		//maxAge
		    TProt::iNu(rb,off,4);		//timestampsTo Return
							//> nodesToRead []
		    TProt::iNu(rb,off,4);		//nodes
		    TProt::iNodeId(rb,off);		//nodeId
		    TProt::iNu(rb,off,4);		//attributeId
		    TProt::iS(rb,off);			//indexRange
							//> dataEncoding
		    TProt::iNu(rb,off,2);		//namespaceIndex
		    TProt::iS(rb,off);			//name

		    //>> Respond
		    reqTp = OpcUa_ReadResponse;
		    TProt::oNu(respEp,1,4);		//Numbers
							//>> value 1
		    TProt::oNu(respEp,13,1);		//Encoding Mask
							//>>> Variant
		    TProt::oNu(respEp,6,1);		//Encoding Mask
		    TProt::oN(respEp,0,4);		//Value (int32)
		    TProt::oTm(respEp,TSYS::curTime());	//sourceTimestamp
		    TProt::oTm(respEp,TSYS::curTime());	//serverTimestamp
		    TProt::oS(respEp,"");		//diagnosticInfos []
		    break;
		}
		case OpcUa_BrowseRequest:
		{
		    //>> Request
							//> view
		    TProt::iNodeId(rb,off);		//viewId
		    TProt::iTm(rb,off);			//timestamp
		    TProt::iNu(rb,off,4);		//viewVersion

		    TProt::iNu(rb,off,4);		//requestedMax ReferencesPerNode
							//> nodesToBrowse
		    uint32_t nc = TProt::iNu(rb,off,4);	//Nodes
		    for( uint32_t i_c = 0; i_c < nc; i_c++ )
		    {
			TProt::iNodeId(rb,off);		//nodeId
			TProt::iNu(rb,off,4);		//browseDirection
			TProt::iNodeId(rb,off);		//referenceTypeId
			TProt::iNu(rb,off,1);		//includeSubtypes
			TProt::iNu(rb,off,4);		//nodeClassMask
			TProt::iNu(rb,off,4);		//resultMask
		    }
		    //>> Respond
		    reqTp = OpcUa_BrowseResponse;
							//> results []
		    TProt::oNu(respEp,1,4);		//Numbers 1
							//Number 1
		    TProt::oNu(respEp,0,4);		//statusCode
		    TProt::oS(respEp,"");		//continuationPoint
							//>> References []
		    TProt::oNu(respEp,3,4);		//Numbers 3
							//References 1
		    TProt::oNodeId(respEp,35);		//referenceTypeId (Organizes)
		    TProt::oNu(respEp,1,1);		//isForward
		    TProt::oNodeId(respEp,87);		//nodeId (ViewsFolder)
		    TProt::oSqlf(respEp,"Views");	//browseName
		    TProt::oSl(respEp,"Views","en");	//displayName
		    TProt::oNu(respEp,1,4);		//nodeClass (OBJECT_1)
		    TProt::oNodeId(respEp,61);		//typeDefinition (FolderType)
							//References 2
		    TProt::oNodeId(respEp,35);		//referenceTypeId (Organizes)
		    TProt::oNu(respEp,1,1);		//isForward
		    TProt::oNodeId(respEp,85);		//nodeId (ObjectsFolder)
		    TProt::oSqlf(respEp,"Objects");	//browseName
		    TProt::oSl(respEp,"Objects","en");	//displayName
		    TProt::oNu(respEp,1,4);		//nodeClass (OBJECT_1)
		    TProt::oNodeId(respEp,61);		//typeDefinition (FolderType)
							//References 3
		    TProt::oNodeId(respEp,35);		//referenceTypeId (Organizes)
		    TProt::oNu(respEp,1,1);		//isForward
		    TProt::oNodeId(respEp,86);		//nodeId (TypesFolder)
		    TProt::oSqlf(respEp,"Types");	//browseName
		    TProt::oSl(respEp,"Types","en");	//displayName
		    TProt::oNu(respEp,1,4);		//nodeClass (OBJECT_1)
		    TProt::oNodeId(respEp,61);		//typeDefinition (FolderType)

		    TProt::oS(respEp,"");		//diagnosticInfos []
		    break;
		}
		case OpcUa_ServiceFault:	break;
		default:
		    throw TError(OpcUa_BadNotSupported,modPrt->nodePath().c_str(),_("No supported request id '%d'."),reqTp);
	    }

	    out.reserve( 200 );
	    out.append( "MSGF" );			//OpenSecureChannel message type
	    TProt::oNu(out,0,4);			//Message size
	    TProt::oNu(out,secId,4);			//Secure channel identifier
	    TProt::oNu(out,tokId,4);			//Symmetric Algorithm Security Header : TokenId
							//> Sequence header
	    TProt::oNu(out,seqN,4);			//Sequence number
	    TProt::oNu(out,reqId,4);			//RequestId
							//> Extension Object
	    TProt::oNodeId(out,reqTp);			//TypeId
							//>> Body
							//>>> RespondHeader
	    TProt::oTm(out,TSYS::curTime());		//timestamp
	    TProt::oN(out,reqHndl,4);			//requestHandle
	    TProt::oN(out,stCode,4);			//StatusCode
	    TProt::oN(out,0,1);				//serviceDiagnostics
	    TProt::oS(out,"");				//stringTable
							//>>> Extensible parameter
	    TProt::oNodeId(out,0);			//TypeId (0)
	    TProt::oNu(out,0,1);			//Encoding
	    out.append(respEp);
	    TProt::oNu(out,out.size(),4,4);		//Real message size

	    printf("TEST 02a: SecureChannel message respond:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());

	    return true;
	}
    }
    catch(TError er)	{ out = mkError(er.cod,er.mess); return false; }

    //> Post error for unrecognized request
    printf("TEST 05: Unsupported request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

    out = mkError( 1, _("Request message isn't recognize.") );

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

//*************************************************
//* OPCEndPoint                                   *
//*************************************************
OPCEndPoint::OPCEndPoint( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), mDB(idb), mEn(false), cntReq(0),
    mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDscr(cfg("DESCR").getSd()), mAEn(cfg("EN").getBd()),
    mSerType(cfg("SerialzType").getId()), mURL(cfg("URL").getSd())
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

string OPCEndPoint::servCert( )	{ return cfg("ServCert").getS(); }

string OPCEndPoint::servPvKey( ){ return cfg("ServPvKey").getS(); }

string OPCEndPoint::secPolicy( int isec )
{
    ResAlloc res( nodeRes(), false );
    if( isec < 0 || isec >= mSec.size() ) throw TError(nodePath().c_str(),_("Security setting %d error."));
    return mSec[isec].policy;
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

void OPCEndPoint::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("End point: ")+name(),0664,"root","root");
	if( ctrMkNode("area",opt,-1,"/ep",_("End point")) )
	{
	    if( ctrMkNode("area",opt,-1,"/ep/st",_("State")) )
	    {
		ctrMkNode("fld",opt,-1,"/ep/st/status",_("Status"),R_R_R_,"root","Protocol",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/ep/st/en_st",_("Enable"),RWRWR_,"root","Protocol",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/ep/st/db",_("DB"),RWRWR_,"root","Protocol",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if( ctrMkNode("area",opt,-1,"/ep/cfg",_("Config")) )
	    {
		TConfig::cntrCmdMake(opt,"/ep/cfg",0,"root","Protocol",RWRWR_);
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServCert",cfg("ServCert").fld().descr(),0660,"root","Protocol",3,"tp","str","cols","90","rows","7");
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServPvKey",cfg("ServPvKey").fld().descr(),0660,"root","Protocol",3,"tp","str","cols","90","rows","7");
		ctrRemoveNode(opt,"/ep/cfg/SecPolicies");
		if( ctrMkNode("table",opt,-1,"/ep/cfg/secPlc",cfg("SecPolicies").fld().descr(),0664,"root","Protocol",1,"s_com","add,del") )
		{
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0",_("Policy"),0664,"root","Protocol",3,"tp","str","dest","select","sel_list","None;Basic128;Basic128Rsa15;Basic256");
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1",_("Message mode"),0664,"root","Protocol",4,"tp","dec","dest","select","sel_id","0;1;2","sel_list",_("None;Sign;Sign&Encrypt"));
		}
	    }
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/ep/st/status" && ctrChkNode(opt) )	opt->setText(getStatus());
    else if( a_path == "/ep/st/en_st" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","Protocol",SEQ_RD) )	opt->setText(enableStat()?"1":"0");
	if( ctrChkNode(opt,"set",RWRWR_,"root","Protocol",SEQ_WR) )	setEnable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/ep/st/db" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","Protocol",SEQ_RD) )	opt->setText(DB());
	if( ctrChkNode(opt,"set",RWRWR_,"root","Protocol",SEQ_WR) )	setDB(opt->text());
    }
    else if( a_path == "/ep/cfg/ls_itr" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if( a_path == "/ep/cfg/secPlc" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Protocol",SEQ_RD) )
	{
	    XMLNode *n_pol	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0","",0664);
	    XMLNode *n_mm	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1","",0664);
	    ResAlloc res( nodeRes(), false );
	    for( int i_p = 0; i_p < mSec.size(); i_p++ )
	    {
		if( n_pol )	n_pol->childAdd("el")->setText(mSec[i_p].policy);
		if( n_mm )	n_mm->childAdd("el")->setText(TSYS::int2str(mSec[i_p].messageMode));
	    }
	    return;
	}
	ResAlloc res( nodeRes(), true );
	if( ctrChkNode(opt,"add",0664,"root","Protocol",SEQ_WR) )
	{ mSec.push_back( SecuritySetting("None",OPCEndPoint::None) ); modif(); return; }
	int row = atoi(opt->attr("row").c_str());
	if( row < 0 || row >= mSec.size() )
	    throw TError(nodePath().c_str(),_("No present seleted row."));
	if( ctrChkNode(opt,"del",0664,"root","Protocol",SEQ_WR) )
	{ mSec.erase(mSec.begin()+row); modif(); return; }
	if( ctrChkNode(opt,"set",0664,"root","Protocol",SEQ_WR) )
	{
	    int col = atoi(opt->attr("col").c_str());
	    if( col == 0 )	mSec[row].policy = opt->text();
	    else if( col == 1 )	mSec[row].messageMode = (OPCEndPoint::MessageSecurityMode)atoi(opt->text().c_str());
	    modif();
	}
    }
    else if( a_path.compare(0,7,"/ep/cfg") == 0 ) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root","root",RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}

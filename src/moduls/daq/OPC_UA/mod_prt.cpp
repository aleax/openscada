
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

    mServer = grpAdd("s_");

    //> Node DB structure
    mServerEl.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"20") );
    mServerEl.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    mServerEl.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300") );
    mServerEl.fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0") );
    mServerEl.fldAdd( new TFld("EndPoint",_("End point"),TFld::String,0,"50","opc.tcp://localhost:4841") );
    mServerEl.fldAdd( new TFld("SecPolicy",_("Security policy"),TFld::String,TFld::Selected,"20","None","None;Basic128;Basic128Rsa15;Basic256",_("None;Basic128;Basic128Rsa15;Basic256")) );
    mServerEl.fldAdd( new TFld("InTR",_("Input transport"),TFld::String,0,"20","*") );
}

TProt::~TProt()
{
    nodeDelAll();
}

void TProt::sAdd( const string &iid, const string &db )
{
    if( chldPresent(mServer,iid) ) return;
    chldAdd( mServer, new OPCServer(iid,db,&serverEl()) );
}

void TProt::load_( )
{
    //> Load DB
    try
    {
	TConfig g_cfg(&serverEl());
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;

	//>> Search into DB
	SYS->db().at().dbList(db_ls,true);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    for( int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+modId()+"_serv","",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if( !sPresent(id) )	sAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }

	//>> Search into config file
	if( SYS->chkSelDB("<cfg>") )
	    for( int fld_cnt=0; SYS->db().at().dataSeek("",nodePath()+modId()+"_serv",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if( !sPresent(id) )	sAdd(id,"*.*");
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
    sList(ls);
    for( int i_n = 0; i_n < ls.size(); i_n++ )
	if( sAt(ls[i_n]).at().toEnable( ) )
	    sAt(ls[i_n]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    sList(ls);
    for( int i_n = 0; i_n < ls.size(); i_n++ )
	sAt(ls[i_n]).at().setEnable(false);
}

int TProt::chnlOpen( )
{
    ResAlloc res( nodeRes(), true );
    if( !(++mSecCnlIdLast) ) mSecCnlIdLast = 0;
    mSecCnl[mSecCnlIdLast] = SecCnl();
    return mSecCnlIdLast;
}

void TProt::chnlClose( int cid )
{
    ResAlloc res( nodeRes(), true );
    mSecCnl.erase( cid );
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

		//> Send request
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );

		int off = 4;
		if( rez.size() < 8 || rez.size() > 4096 || iNu(rez,off,4) != rez.size() )
		    err = _("13:Error respond: Respond size is not coincedence.");
		else if( rez.compare(0,4,"ERRF") == 0 )
		{
		    int merr = iNu(rez,off,4);		//Error
		    string reas = iS(rez,off);		//Reason
		    err = TSYS::uint2str(merr)+":"+reas;
		}
		else if( rez.compare(0,4,"ACKF") != 0 )
		    err = _("13:Error respond: Too short or not acknowledge.");
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
		    io.setAttr("SeqNumber","33");
		    oNu(rez,atoi(io.attr("SeqNumber").c_str()),4);	//Sequence number
		    io.setAttr("SeqReqId","1");
		    oNu(rez,atoi(io.attr("SeqReqId").c_str()),4);	//RequestId
							//> Extension body object
		    oNodeId(rez,446);			//TypeId
							//>> Request Header
		    oNodeId(rez,0);			//Session AuthenticationToken
		    oTm(rez,TSYS::curTime());		//timestamp
		    io.setAttr("ReqHandle","0");
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
		    oN(rez,300000,4);			//RequestedLifetime
		    oNu(rez,rez.size(),4,4);		//> Real message size

		    //> Send request
		    int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		    rez.assign( buf, resp_len );

		    int off = 4;
		    if( rez.size() < 8 || rez.compare(0,4,"OPNF") != 0 )	err = _("13:Error respond: Too short or not acknowledge.");
		    else if( iNu(rez,off,4) != rez.size() )	err = _("13:Error respond: Respond size is not coincedence.");
		    {
			iNu(rez,off,4);			//Secure channel identifier
			iS(rez,off);			//Security policy URI
			iN(rez,off,4);			//SenderCertificateLength
			iN(rez,off,4);			//ReceiverCertificateThumbprintLength
			iNu(rez,off,4);			//Sequence number
			iNu(rez,off,4);			//RequestId
							//> Extension Object
			if( iNodeId(rez,off) != 449 )	//TypeId (449 - NodeId)
			    throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
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
			io.setAttr("secChnId",TSYS::int2str(iNu(rez,off,4)));		//Secure channel identifier
			io.setAttr("secTokenId",TSYS::int2str(iNu(rez,off,4)));		//TokenId
			iTm(rez,off);			//CreatedAt
			iN(rez,off,4);			//RevisedLifeTime (600000)
			iS(rez,off);			//nonce
		    }
		    printf("TEST 10: OPN respond:\n%s\n",TSYS::strDecode(rez,TSYS::Bin).c_str());
		}
		else err = TSYS::strMess(_("12:OPC UA '%s': security policy '%s' is not supported."),io.name().c_str(),io.attr("SecPolicy").c_str());
	    }
	    else if( io.attr("id") == "CLO" )
	    {
		rez.reserve( 200 );
		rez.append( "CLOF" );			//OpenSecureChannel close
		oNu(rez,0,4);				//Message size
		oNu(rez,atoi(io.attr("secChnId").c_str()),4);	//Secure channel identifier
		oNu(rez,atoi(io.attr("secTokenId").c_str()),4);	//TokenId
							//> Sequence header
		uint32_t SeqNumber = atoi(io.attr("SeqNumber").c_str())+1;
		io.setAttr("SeqNumber",TSYS::int2str(SeqNumber));
		uint32_t SeqReqId = atoi(io.attr("SeqReqId").c_str())+1;
		io.setAttr("SeqReqId",TSYS::int2str(SeqReqId));
		oNu(rez,SeqNumber,4);			//Sequence number
		oNu(rez,SeqReqId,4);			//RequestId
							//> Extension body object
		oNodeId(rez,452);			//TypeId request: CloseSecureChannel
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

		//> Send request and don't wait response
		tro.messIO( rez.data(), rez.size(), NULL, 0, 0, true );
	    }
	    else
	    {
		int iTpId = 0;
		string mReq;
		if( io.attr("id") == "FindServers" )
		{
		    iTpId = 422;
		    oS(mReq,"opc.tcp://roman.home:4841");	//endpointUrl
		    oS(mReq,"");				//localeIds []
		    oS(mReq,"");				//serverUris []
		}
		else if( io.attr("id") == "GetEndpoints" )
		{
		    iTpId = 428;
		    oS(mReq,"opc.tcp://roman.home:4841");	//endpointUrl
		    oS(mReq,"");				//localeIds []
		    oS(mReq,"");				//profileUris []
		}
		else if( io.attr("id") == "CreateSession" )
		{
		    iTpId =  461;
							//> clientDescription (Application Description)
		    oS(mReq,"");			//applicationUri
		    oS(mReq,"");			//productUri
		    oSl(mReq,"");			//applicationName
		    oNu(mReq,1,4);			//applicationType (CLIENT_1)
		    oS(mReq,"");			//gatewayServerUri
		    oS(mReq,"");			//discoveryProfileUri
		    oS(mReq,"");			//discoveryUrls

		    oS(mReq,"");			//serverUri
		    oS(mReq,"opc.tcp://roman.home:4841");	//endpointUrl
		    oS(mReq,"OpenSCADA client");	//sessionName
		    oS(mReq,string(16,0)+string(4,0xFF)+string(12,0));	//clientNonce

		    //>>> Certificate reading
		    int hd = ::open("uaexpert.der",O_RDONLY);
		    if( hd < 0 ) throw TError(100,nodePath().c_str(),_("Certificate uaservercpp.der open error."));
		    int cf_sz = lseek(hd,0,SEEK_END);
		    lseek(hd,0,SEEK_SET);
		    char *buf = (char *)malloc(cf_sz);
		    read(hd,buf,cf_sz);
		    ::close(hd);
		    string cert(buf,cf_sz);
		    free(buf);

		    oS(mReq,cert);			//clientCertificate
		    oR(mReq,1.2e6,8);			//Requested SessionTimeout, ms
		    oNu(mReq,0x1000000,4);		//maxResponse MessageSize
		}
		else if( io.attr("id") == "ActivateSession" )
		{
		    iTpId = 467;
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
		    iTpId = 473;
		    oNu(mReq,1,1);			//deleteSubscriptions
		}
		else if( io.attr("id") == "Read" )
		{
		    iTpId = 631;
		    oR(mReq,0,8);			//maxAge 0 ms
		    oNu(mReq,1,4);			//timestampsTo Return (SERVER_1)
							//> nodesToRead []
		    oNu(mReq,1,4);			//nodes
		    oNodeId(mReq,2259);			//nodeId (ServerStatus_State)
		    oNu(mReq,13,4);			//attributeId (Value)
		    oS(mReq,"");			//indexRange
							//> dataEncoding
		    oNu(mReq,0,2);			//namespaceIndex
		    oS(mReq,"");			//name
		}
		else if( io.attr("id") == "Browse" )
		{
		    iTpId = 527;
							//> view
		    oNodeId(mReq,0);			//viewId (0)
		    oTm(mReq,0);			//timestamp
		    oNu(mReq,0,4);			//viewVersion

		    oNu(mReq,100,4);			//requestedMax ReferencesPerNode
							//> nodesToBrowse
		    oNu(mReq,1,4);			//Nodes 1
							//>> Node 1
		    oNodeId(mReq,84);			//nodeId (RootFolder)
		    oNu(mReq,0,4);			//browseDirection (FORWARD_0)
		    oNodeId(mReq,33);			//referenceTypeId (HierarchicalReferences)
		    oNu(mReq,1,1);			//includeSubtypes (true)
		    oNu(mReq,0,4);			//nodeClassMask ( all NodeClasses )
		    oNu(mReq,0x3f,4);			//resultMask ( all )
		}
		else throw TError( 100, "OPC UA Bin", _("11:OPC UA '%s': request '%s' is not supported."),io.name().c_str(),io.attr("id").c_str());

		rez.reserve( 200 );
		rez.append( "MSGF" );			//SecureChannel message
		oNu(rez,0,4);				//Message size
		oNu(rez,atoi(io.attr("secChnId").c_str()),4);		//Secure channel identifier
		oNu(rez,atoi(io.attr("secTokenId").c_str()),4);		//TokenId
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

		printf("TEST 11a: Request '%s':\n%s\n",io.attr("id").c_str(),TSYS::strDecode(rez,TSYS::Bin).c_str());

		//> Send request and wait respond
		int resp_len = tro.messIO( rez.data(), rez.size(), buf, sizeof(buf), 0, true );
		rez.assign( buf, resp_len );
		int off = 4;
		for( ; rez.size() < 8 || rez.size() < iNu(rez,off,4); off = 4 )
		{
		    resp_len = tro.messIO( NULL, 0, buf, sizeof(buf), 0, true );
		    rez.append( buf, resp_len );
		}

		printf("TEST 11b: Respond '%s':\n%s\n",io.attr("id").c_str(),TSYS::strDecode(rez,TSYS::Bin).c_str());

		off = 4;
		if( rez.size() < 8 || rez.compare(0,4,"MSGF") != 0 )	err = _("13:Error respond: Too short or not acknowledge.");
		else if( iNu(rez,off,4) != rez.size() )	err = _("13:Error respond: Respond size is not coincedence.");
		{
		    iNu(rez,off,4);				//Secure channel identifier
		    iNu(rez,off,4);				//Symmetric Algorithm Security Header : TokenId
								//> Sequence header
		    iNu(rez,off,4);				//Sequence number
		    iNu(rez,off,4);				//RequestId
								//> Extension Object
		    int oTpId = iNodeId(rez,off);		//TypeId
								//>> Body
								//>>> RespondHeader
		    iTm(rez,off);				//timestamp
		    iN(rez,off,4);				//requestHandle
		    iN(rez,off,4);				//StatusCode
		    iN(rez,off,1);				//serviceDiagnostics
		    iS(rez,off);				//stringTable
								//>>> Extensible parameter
		    iNodeId(rez,off);				//TypeId (0)
		    iNu(rez,off,1);				//Encoding

		    switch( iTpId )
		    {
			case 422:	//FindServers
			{
			    if( oTpId != 425 )	throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
			    int AppDescrNub = iNu(rez,off,4);		//List items
			    for( int i_l = 0; i_l < AppDescrNub; i_l++ )
			    {
				iS(rez,off);				//applicationUri
				iS(rez,off);				//productUri
				iSl(rez,off);				//applicationName
				iNu(rez,off,4);				//applicationType
				iS(rez,off);				//gatewayServerUri
				iS(rez,off);				//discoveryProfileUri
									//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);	//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    iS(rez,off);			//discoveryUrl
			    }
			    break;
			}
			case 428:	//GetEndpoints
			{
			    if( oTpId != 431 )	throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
			    int EndpointDescrNub = iNu(rez,off,4);	//List items
			    for( int i_l = 0; i_l < EndpointDescrNub; i_l++ )
			    {
									//>>> EndpointDescription
				iS(rez,off);				//endpointUrl
									//>>>> server (ApplicationDescription)
				iS(rez,off);				//applicationUri
				iS(rez,off);				//productUri
				iSl(rez,off);				//applicationName
				iNu(rez,off,4);				//applicationType
				iS(rez,off);				//gatewayServerUri
				iS(rez,off);				//discoveryProfileUri
									//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez,off,4);	//List items
				for( int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++ )
				    iS(rez,off);			//discoveryUrl
				iS(rez,off);				//>>> serverCertificate
				iNu(rez,off,4);				//securityMode
				iS(rez,off);				//securityPolicyUri
									//>>>> userIdentityTokens
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
			    break;
			}
			case 461:	//CreateSession
			{
			    if( oTpId != 464 )	throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
			    io.setAttr("sesId",TSYS::int2str(iNodeId(rez,off)));	//sessionId
			    io.setAttr("authTokenId",TSYS::int2str(iNodeId(rez,off)));	//authentication Token
			    iR(rez,off,8);				//revisedSession Timeout, ms
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
			case 467:	//ActivateSession
			{
			    if( oTpId != 470 )	throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
			    iS(rez,off);				//serverNonce
			    iS(rez,off);				//results []
			    iS(rez,off);				//diagnosticInfos []
			    break;
			}
			case 473:	//CloseSession
			    break;
			case 631:	//Read
			{
			    if( oTpId != 634 )	throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
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
			case 527:	//Browse
			{
			    if( oTpId != 530 )	throw TError( 100, "OPC UA Bin", _("15:Respond's NodeId don't acknowledge") );
									//> results []
			    int resN = iNu(rez,off,4);			//Numbers
			    for( int i_r = 0; i_r < resN; i_r++ )
			    {
				iNu(rez,off,4);				//statusCode
				iS(rez,off);				//continuationPoint
									//>> References []
				int refN = iNu(rez,off,4);		//Numbers
				for( int i_rf = 0; i_rf < refN; i_rf++ )
				{
				    iNodeId(rez,off);			//referenceTypeId
				    iNu(rez,off,1);			//isForward
				    iNodeId(rez,off);			//nodeId
				    iSqlf(rez,off);			//browseName
				    iSl(rez,off);			//displayName
				    iNu(rez,off,4);			//nodeClass
				    iNodeId(rez,off);			//typeDefinition
				}
			    }
			    iS(rez,off);				//diagnosticInfos []
			    break;
			}
		    }
                }
	    }
	}
	else err = TSYS::strMess(_("10:OPC UA protocol '%s' is not supported."),io.name().c_str());
    }
    catch(TError er) { err = (er.cod==100) ? er.mess : TSYS::strMess(_("14:Remote host error: %s"),er.mess.c_str()); }

    io.setAttr("err",err);
}

const char *TProt::iVal( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(100,modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    return rb.data()+off-vSz;
}

int32_t TProt::iN( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(100,modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    switch( vSz )
    {
	case 1:	return *(int8_t*)(rb.data()+off-vSz);
	case 2:	return *(int16_t*)(rb.data()+off-vSz);
	case 4:	return *(int32_t*)(rb.data()+off-vSz);
    }
    throw TError(100,modPrt->nodePath().c_str(),_("Number size '%d' error."),vSz);
}

uint32_t TProt::iNu( const string &rb, int &off, char vSz )
{
    off += vSz;
    if( off > rb.size() ) throw TError(100,modPrt->nodePath().c_str(),_("Buffer size is less for requested value."));
    switch( vSz )
    {
	case 1:	return *(uint8_t*)(rb.data()+off-vSz);
	case 2:	return *(uint16_t*)(rb.data()+off-vSz);
	case 4:	return *(uint32_t*)(rb.data()+off-vSz);
    }
    throw TError(100,modPrt->nodePath().c_str(),_("Number size '%d' error."),vSz);
}

double TProt::iR( const string &rb, int &off, char vSz )
{
    if( vSz == 4 ) return *(float *)iVal(rb,off,vSz);
    else if( vSz == 8 ) return *(double *)iVal(rb,off,vSz);
    throw TError(100,modPrt->nodePath().c_str(),_("Real number size '%d' error."),vSz);
}

string TProt::iS( const string &rb, int &off )
{
    int sSz = iN(rb,off,4); sSz = vmax(0,sSz);
    off += sSz;
    if( off > rb.size() ) throw TError(100,modPrt->nodePath().c_str(),_("Buffer size is less for requested string."));
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
    if( !nsIdx ) *nsIdx = tNsIdx;
    return iS(rb,off);
}

long long TProt::iTm( const string &rb, int &off )
{
    int64_t tmStamp = *(int64_t*)TProt::iVal(rb,off,8);
    return (tmStamp/10ll)-11644473600000000ll;
}

int TProt::iNodeId( const string &rb, int &off, int *ns )
{
    off += 1;
    if( off > rb.size() ) throw TError(100,modPrt->nodePath().c_str(),_("Buffer size is less for requested NodeId."));
    char enc = rb[off-1];
    switch( enc )
    {
	case 0x00:
	    if( ns ) *ns = 0;
	    return iNu(rb,off,1);
	case 0x01:
	    off += 1;
	    if( off > rb.size() ) throw TError(100,modPrt->nodePath().c_str(),_("Buffer size is less for requested NodeId."));
	    if( ns ) *ns = (uint8_t)rb[off-1];
	    return iNu(rb,off,2);
    }
    throw TError(100,modPrt->nodePath().c_str(),_("NodeId type %d error or don't support."),enc);
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
    else throw TError(100,modPrt->nodePath().c_str(),_("Real number size '%d' error."),sz);
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

void TProt::oNodeId( string &buf, int val, int ns )
{
    if( ns == 0 && val <= 255 )
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

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/s_",_("Server"),0664,"root","Protocol",2,"idm","1","idSz","20");
	if( ctrMkNode("area",opt,0,"/serv",_("Servers")) )
	    ctrMkNode("list",opt,-1,"/serv/serv",_("Servers"),0664,"root","Protocol",5,"tp","br","idm","1","s_com","add,del","br_pref","s_","idSz","20");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/br/s_" || a_path == "/serv/serv" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Protocol",SEQ_RD) )
	{
	    vector<string> lst;
	    sList(lst);
	    for( unsigned i_f=0; i_f < lst.size(); i_f++ )
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(sAt(lst[i_f]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","Protocol",SEQ_WR) )
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    sAdd(vid); sAt(vid).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","Protocol",SEQ_WR) )	chldDel(mServer,opt->attr("id"),-1,1);
    }
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
	    if( rb.size() > 4096 )	{ out = mkError(OpcUa_BadTcpEndpointUrlInvalid); return false; }
	    off = 8;
	    uint32_t prtVer = TProt::iNu(rb,off,4);	//Protocol version
	    uint32_t rBufSz = TProt::iNu(rb,off,4);	//Recive buffer size
	    uint32_t wBufSz = TProt::iNu(rb,off,4);	//Send buffer size
	    uint32_t mMsgSz = TProt::iNu(rb,off,4);	//Max message size
	    uint32_t mChnk  = TProt::iNu(rb,off,4);	//Max chunk count
	    string EndpntURL = TProt::iS(rb,off);	//EndpointURL

	    printf( "TEST 00: Hello request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    //>> Find endpoint into allowed servers
	    vector<string> sLs;
	    owner().sList(sLs);
	    AutoHD<OPCServer> tos;
	    int i_os;
	    for( i_os = 0; i_os < sLs.size(); i_os++ )
	    {
		tos = owner().sAt(sLs[i_os]);
		if( tos.at().enableStat() && tos.at().endPoint() == EndpntURL &&
			(tos.at().inTransport() == "*" || tos.at().inTransport() == srcTr()) )
		    break;
	    }
	    if( i_os >= sLs.size() )	{ out = mkError(OpcUa_BadTcpEndpointUrlInvalid); return false; }

	    //> Prepare acknowledge message
	    out.reserve( 28 );
	    out.append( "ACKF" );			//Acknowledge message type
	    TProt::oNu(out,28,4);			//Message size
	    TProt::oNu(out,OpcUa_ProtocolVersion,4);	//Protocol version
	    TProt::oNu(out,OpcUa_ReciveBufferSize,4);	//Recive buffer size
	    TProt::oNu(out,OpcUa_SendBufferSize,4);	//Send buffer size
	    TProt::oNu(out,OpcUa_MaxMessageSize,4);	//Max message size
	    TProt::oNu(out,OpcUa_MaxChunkCount,4);	//Max chunk count

	    printf( "TEST 01: Hello response:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());

	    return true;
	}
	//> Check for Open SecureChannel message type
	if( rb.size() > 8 && rb.compare(0,4,"OPNF") == 0 )
	{
	    off = 8;
	    uint32_t secChnId = TProt::iNu(rb,off,4);		//Secure channel identifier

								//> Security Header
	    string secPlcURI = TProt::iS(rb,off);			//Security policy URI
	    if( secPlcURI == "http://opcfoundation.org/UA/SecurityPolicy#None" )
	    {
		int32_t senderCertLength = TProt::iN(rb,off,4);	//SenderCertificateLength
		int32_t recCertThbLength = TProt::iN(rb,off,4);	//ReceiverCertificateThumbprintLength
								//> Sequence header
		uint32_t secNumb = TProt::iNu(rb,off,4);	//Sequence number
		uint32_t reqId = TProt::iNu(rb,off,4);		//RequestId
								//> Extension body object
		uint16_t eoTpId = TProt::iNodeId(rb,off);	//TypeId
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
		uint32_t clntPrtVer = TProt::iNu(rb,off,4);	//ClientProtocolVersion
		uint32_t reqTp = TProt::iNu(rb,off,4);		//RequestType
		uint32_t secMode = TProt::iNu(rb,off,4);	//SecurityMode
		string clntNonce = TProt::iS(rb,off);		//ClientNonce
		int32_t reqLifeTm = TProt::iN(rb,off,4);	//RequestedLifetime

		printf( "TEST 01: Open SecureChannel request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

		uint32_t chnlId = owner().chnlOpen();

		//> Prepare respond message
		out.reserve( 200 );
		out.append( "OPNF" );				//OpenSecureChannel message type
		TProt::oNu(out,0,4);				//Message size
		TProt::oNu(out,chnlId,4);			//Secure channel identifier
		TProt::oS(out,secPlcURI);			//Security policy URI
		TProt::oN(out,senderCertLength,4);		//SenderCertificateLength
		TProt::oN(out,recCertThbLength,4);		//ReceiverCertificateThumbprintLength
		TProt::oNu(out,secNumb,4);			//Sequence number
		TProt::oNu(out,reqId,4);			//RequestId
								//> Extension Object
		TProt::oNodeId(out,449,0);			//TypeId (449 - NodeId)
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
		TProt::oNu(out,1,4);				//TokenId
		TProt::oTm(out,TSYS::curTime());		//CreatedAt
		TProt::oN(out,vmax(600000,reqLifeTm),4);	//RevisedLifeTime (600000, minimum)
		TProt::oS(out,"\001");				//nonce
		TProt::oNu(out,out.size(),4,4);			//Real message size

		printf("TEST 01a: Open sec respond:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());

		return true;
	    }
	}
	//> Check for Close SecureChannel message type
	if( rb.size() > 8 && rb.compare(0,4,"CLOF") == 0 )
	{
	    off = 8;
	    uint32_t secId = TProt::iNu(rb,off,4);	//Secure channel identifier
	    uint32_t tokId = TProt::iNu(rb,off,4);	//TokenId
							//> Sequence header
	    uint32_t seqN = TProt::iNu(rb,off,4);	//Sequence number
	    uint32_t reqId = TProt::iNu(rb,off,4);	//RequestId
							//> Extension body object
	    int reqTp = TProt::iNodeId(rb,off);		//TypeId request
							//>> Request Header
	    TProt::iNodeId(rb,off);			//Session AuthenticationToken
	    TProt::iTm(rb,off);				//timestamp
	    int32_t reqHndl = TProt::iN(rb,off,4);	//requestHandle
	    TProt::iNu(rb,off,4);			//returnDiagnostics
	    TProt::iS(rb,off);				//auditEntryId
	    TProt::iNu(rb,off,4);			//timeoutHint
							//>>> Extensible parameter
	    TProt::iNodeId(rb,off);			//TypeId (0)
	    TProt::iNu(rb,off,1);			//Encoding

	    printf( "TEST 01: Close SecureChannel request:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    owner().chnlClose( secId );

	    //> No respond. Close socket
	    return false;
	}
	//> Check for SecureChannel message type
	if( rb.compare(0,4,"MSGF") == 0 )
	{
	    printf( "TEST 02: SecureChannel message:\n%s\n",TSYS::strDecode(rb,TSYS::Bin).c_str());

	    off = 8;
	    uint32_t secId = TProt::iNu(rb,off,4);	//Secure channel identifier
	    uint32_t tokId = TProt::iNu(rb,off,4);	//TokenId
							//> Sequence header
	    uint32_t seqN = TProt::iNu(rb,off,4);	//Sequence number
	    uint32_t reqId = TProt::iNu(rb,off,4);	//RequestId
							//> Extension body object
	    int reqTp = TProt::iNodeId(rb,off);		//TypeId request
							//>> Request Header
	    TProt::iNodeId(rb,off);			//Session AuthenticationToken
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
		case 422:	//FindServers
		    //>> Request
		    TProt::iS(rb,off);			//endpointUrl
		    TProt::iS(rb,off);			//localeIds []
		    TProt::iS(rb,off);			//serverUris []

		    printf( "TEST 02a\n" );

		    //>> Respond
		    reqTp = 425;
		    TProt::oNu(respEp,1,4);		//ApplicationDescription list items
							//>>>> ApplicationDescription 1
		    TProt::oS(respEp,"roman.home/Vendor/UaDemoserver");//applicationUri
		    TProt::oS(respEp,"Vendor/UaDemoserver");	//productUri
		    TProt::oSl(respEp,"OpcDemoServer@roman.home","en");//applicationName
		    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
		    TProt::oS(respEp,"");		//gatewayServerUri
		    TProt::oS(respEp,"");		//discoveryProfileUri
							//>>>> discoveryUrls
		    TProt::oNu(respEp,1,4);		//List items
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//discoveryUrl
		    break;
		case 428:	//GetEndpoints
		{
		    //>> Request
		    TProt::iS(rb,off);			//endpointUrl
		    TProt::iS(rb,off);			//localeIds []
		    TProt::iS(rb,off);			//profileUris []

		    //>> Respond
		    reqTp = 431;
		    //>>> Certificate reading
		    int hd = open("uaservercpp.der",O_RDONLY);
		    if( hd < 0 ) throw TError(nodePath().c_str(),_("Certificate uaservercpp.der open error."));
		    int cf_sz = lseek(hd,0,SEEK_END);
		    lseek(hd,0,SEEK_SET);
		    char *buf = (char *)malloc(cf_sz);
		    read(hd,buf,cf_sz);
		    close(hd);
		    string cert(buf,cf_sz);
		    free(buf);

		    respEp.reserve(2000);
		    TProt::oNu(respEp,2,4);		//EndpointDescrNubers list items

							//>>> EndpointDescription 1
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//endpointUrl
							//>>>> server (ApplicationDescription)
		    TProt::oS(respEp,"roman.home/Vendor/UaDemoserver");//applicationUri
		    TProt::oS(respEp,"");		//productUri
		    TProt::oSl(respEp,"OpcDemoServer@roman.home","en");//applicationName
		    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
		    TProt::oS(respEp,"");		//gatewayServerUri
		    TProt::oS(respEp,"");		//discoveryProfileUri
							//>>>> discoveryUrls
		    TProt::oNu(respEp,1,4);		//List items
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//discoveryUrl

		    TProt::oS(respEp,cert);		//>>> serverCertificate
		    TProt::oNu(respEp,1,4);		//securityMode:MessageSecurityMode (NONE)
		    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#None");	//securityPolicyUri

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

							//>>> EndpointDescription 2
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//endpointUrl
							//>>>> server (ApplicationDescription)
		    TProt::oS(respEp,"roman.home/Vendor/UaDemoserver");//applicationUri
		    TProt::oS(respEp,"");		//productUri
		    TProt::oSl(respEp,"OpcDemoServer@roman.home","en");//applicationName
		    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
		    TProt::oS(respEp,"");		//gatewayServerUri
		    TProt::oS(respEp,"");		//discoveryProfileUri
							//>>>> discoveryUrls
		    TProt::oNu(respEp,1,4);		//List items
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//discoveryUrl

		    TProt::oS(respEp,cert);		//>>> serverCertificate
		    TProt::oNu(respEp,3,4);		//securityMode:MessageSecurityMode (SIGNANDENCRYPT)
		    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

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
		    break;
		}
		case 461:	//CreateSessionRequest
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
		    TProt::iS(rb,off);			//sessionName
		    TProt::iS(rb,off);			//clientNonce
		    TProt::iS(rb,off);			//clientCertificate
		    double rStm = TProt::iR(rb,off,8);	//Requested SessionTimeout, ms
		    TProt::iNu(rb,off,4);		//maxResponse MessageSize

		    //>> Respond
		    reqTp = 464;
		    //>>> Certificate reading
		    int hd = open("uaservercpp.der",O_RDONLY);
		    if( hd < 0 ) throw TError(nodePath().c_str(),_("Certificate uaservercpp.der open error."));
		    int cf_sz = lseek(hd,0,SEEK_END);
		    lseek(hd,0,SEEK_SET);
		    char *buf = (char *)malloc(cf_sz);
		    read(hd,buf,cf_sz);
		    close(hd);
		    string cert(buf,cf_sz);
		    free(buf);

		    respEp.reserve(2000);
		    TProt::oNodeId(respEp,1);		//sessionId
		    TProt::oNodeId(respEp,1);		//authentication Token
		    TProt::oR(respEp,1.2e6,8);		//revisedSession Timeout, ms
		    TProt::oS(respEp,"");		//serverNonce
		    TProt::oS(respEp,cert);		//serverCertificate
							//> EndpointDescr []
		    TProt::oNu(respEp,2,4);		//EndpointDescrNubers list items
							//>>> EndpointDescription 1
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//endpointUrl
							//>>>> server (ApplicationDescription)
		    TProt::oS(respEp,"roman.home/Vendor/UaDemoserver");//applicationUri
		    TProt::oS(respEp,"");		//productUri
		    TProt::oSl(respEp,"OpcDemoServer@roman.home","en");//applicationName
		    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
		    TProt::oS(respEp,"");		//gatewayServerUri
		    TProt::oS(respEp,"");		//discoveryProfileUri
							//>>>> discoveryUrls
		    TProt::oNu(respEp,1,4);		//List items
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//discoveryUrl

		    TProt::oS(respEp,cert);		//>>> serverCertificate
		    TProt::oNu(respEp,1,4);		//securityMode:MessageSecurityMode (NONE)
		    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#None");	//securityPolicyUri

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

							//>>> EndpointDescription 2
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//endpointUrl
							//>>>> server (ApplicationDescription)
		    TProt::oS(respEp,"roman.home/Vendor/UaDemoserver");//applicationUri
		    TProt::oS(respEp,"");		//productUri
		    TProt::oSl(respEp,"OpcDemoServer@roman.home","en");//applicationName
		    TProt::oNu(respEp,0,4);		//applicationType (SERVER)
		    TProt::oS(respEp,"");		//gatewayServerUri
		    TProt::oS(respEp,"");		//discoveryProfileUri
							//>>>> discoveryUrls
		    TProt::oNu(respEp,1,4);		//List items
		    TProt::oS(respEp,"opc.tcp://roman.home:4841");	//discoveryUrl

		    TProt::oS(respEp,cert);		//>>> serverCertificate
		    TProt::oNu(respEp,3,4);		//securityMode:MessageSecurityMode (SIGNANDENCRYPT)
		    TProt::oS(respEp,"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

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

		    TProt::oS(respEp,"");		//serverSoftware Certificates []
							//> serverSignature
		    TProt::oS(respEp,"");		//signature
		    TProt::oS(respEp,"");		//algorithm

		    TProt::oNu(respEp,0,4);		//maxRequest MessageSize
		    break;
		}
		case 467:	//ActivateSessionRequest
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
		    int userIdTk = TProt::iNodeId(rb,off);//TypeId
		    TProt::iNu(rb,off,1);		//Encode
		    TProt::iNu(rb,off,4);		//Length
		    switch( userIdTk )
		    {
			case 321:	//AnonymousIdentityToken
			    TProt::iS(rb,off);		//policyId
			    break;
			default:
			    throw TError(modPrt->nodePath().c_str(),_("No supported userIdentityToken '%d'."),userIdTk);
		    }
							//> userTokenSignature
		    TProt::iS(rb,off);			//signature
		    TProt::iS(rb,off);			//algorithm

		    //>> Respond
		    reqTp = 470;
		    respEp.reserve(100);
		    TProt::oS(respEp,"");		//serverNonce
		    TProt::oS(respEp,"");		//results []
		    TProt::oS(respEp,"");		//diagnosticInfos []
		    break;
		}
		case 473:	//CloseSessionRequest
		{
		    //>> Request
		    TProt::iNu(rb,off,1);		//deleteSubscriptions

		    //>> Respond
		    reqTp = 476;
		    break;
		}
		case 631:	//ReadRequest
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
		    reqTp = 634;
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
		case 527:	//BrowseRequest
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
		    reqTp = 530;
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
		default:
		    throw TError(modPrt->nodePath().c_str(),_("No supported request id '%d'."),reqTp);
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
	    TProt::oN(out,0,4);				//StatusCode
	    TProt::oN(out,0,1);				//serviceDiagnostics
	    TProt::oS(out,"");				//stringTable
							//>>> Extensible parameter
	    TProt::oNodeId(out,0);			//TypeId (0)
	    TProt::oNu(out,0,1);			//Encoding
	    out.append(respEp);
	    TProt::oNu(out,out.size(),4,4);		//Real message size

	    printf("TEST 01b: SecureChannel message respond:\n%s\n",TSYS::strDecode(out,TSYS::Bin).c_str());

	    return true;
	}
    }
    catch(TError er)	{ out = mkError(2,er.mess); return false; }

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
//* OPCServer                                     *
//*************************************************
OPCServer::OPCServer( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), mDB(idb), mEn(false), cntReq(0),
    mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDscr(cfg("DESCR").getSd()), mAEn(cfg("EN").getBd())
{
    mId = iid;
}

OPCServer::~OPCServer( )
{
    try{ setEnable(false); } catch(...) { }
}

TCntrNode &OPCServer::operator=( TCntrNode &node )
{
    OPCServer *src_n = dynamic_cast<OPCServer*>(&node);
    if( !src_n ) return *this;

    if( enableStat( ) )	setEnable(false);

    //> Copy parameters
    string prevId = mId;
    *(TConfig*)this = *(TConfig*)src_n;
    mId = prevId;
    setDB(src_n->DB());

    return *this;
}

void OPCServer::postDisable( int flag )
{
    try
    {
	if( flag ) SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TProt &OPCServer::owner( )	{ return *(TProt*)nodePrev(); }

string OPCServer::name( )	{ return mName.size() ? mName : id(); }

string OPCServer::tbl( )	{ return owner().modId()+"_serv"; }

string OPCServer::endPoint( )	{ return cfg("EndPoint").getS(); }

string OPCServer::inTransport( ){ return cfg("InTR").getS(); }

bool OPCServer::cfgChange( TCfg &ce )
{
    modif();
    return true;
}

void OPCServer::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;
    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
}

void OPCServer::save_( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void OPCServer::setEnable( bool vl )
{
    if( mEn == vl ) return;

    cntReq = 0;

    mEn = vl;
}

string OPCServer::getStatus( )
{
    string rez = _("Disabled. ");
    if( enableStat( ) )
    {
	rez = _("Enabled. ");
	rez += TSYS::strMess( _("Requests %.4g."), cntReq );
    }

    return rez;
}

void OPCServer::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Server: ")+name(),0664,"root","root");
	if( ctrMkNode("area",opt,-1,"/serv",_("Server")) )
	{
	    if( ctrMkNode("area",opt,-1,"/serv/st",_("State")) )
	    {
		ctrMkNode("fld",opt,-1,"/serv/st/status",_("Status"),R_R_R_,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/serv/st/en_st",_("Enable"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/serv/st/db",_("DB"),RWRWR_,"root","root",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if( ctrMkNode("area",opt,-1,"/serv/cfg",_("Config")) )
	    {
		TConfig::cntrCmdMake(opt,"/serv/cfg",0,"root","root",RWRWR_);
		ctrMkNode("fld",opt,-1,"/serv/cfg/InTR",cfg("InTR").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/serv/cfg/ls_itr");
	    }
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/serv/st/status" && ctrChkNode(opt) )	opt->setText(getStatus());
    else if( a_path == "/serv/st/en_st" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->setText(enableStat()?"1":"0");
	if( ctrChkNode(opt,"set",RWRWR_,"root","root",SEQ_WR) )	setEnable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/serv/st/db" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->setText(DB());
	if( ctrChkNode(opt,"set",RWRWR_,"root","root",SEQ_WR) )	setDB(opt->text());
    }
    else if( a_path == "/serv/cfg/ls_itr" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if( a_path.substr(0,9) == "/serv/cfg" ) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root","root",RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}

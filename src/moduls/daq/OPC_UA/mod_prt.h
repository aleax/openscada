
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.h
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

#ifndef MOD_PRT_H
#define MOD_PRT_H

#include <stdint.h>

#include <string>
#include <map>

#include <tprotocols.h>

using std::string;
using std::map;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"OPC_UA"
#define PRT_NAME	"OPC UA"
#define PRT_TYPE	"Protocol"
#define PRT_SUBVER	VER_PROT
#define PRT_MVER	"0.5.0"
#define PRT_AUTOR	"Roman Savochenko"
#define PRT_DESCR	"Allow realisation of OPC UA protocol."
#define PRT_LICENSE	"GPL"
//*************************************************

namespace OPC_UA
{

//> Constants
#define OpcUa_ProtocolVersion		0
#define OpcUa_ReciveBufferSize		0x10000
#define OpcUa_SendBufferSize		0x10000
#define OpcUa_MaxMessageSize		0x1000000
#define OpcUa_MaxChunkCount		5000

//> Built-in Data Types
#define OpcUa_Boolean 1
#define OpcUa_SByte 2
#define OpcUa_Byte 3
#define OpcUa_Int16 4
#define OpcUa_UInt16 5
#define OpcUa_Int32 6
#define OpcUa_UInt32 7
#define OpcUa_Int64 8
#define OpcUa_UInt64 9
#define OpcUa_Float 10
#define OpcUa_Double 11
#define OpcUa_String 12
#define OpcUa_DateTime 13
#define OpcUa_Guid 14
#define OpcUa_ByteString 15
#define OpcUa_XmlElement 16
#define OpcUa_NodeId 17
#define OpcUa_ExpandedNodeId 18
#define OpcUa_StatusCode 19
#define OpcUa_QualifiedName 20
#define OpcUa_LocalizedText 21
#define OpcUa_Structure 22
#define OpcUa_DataValue 23
#define OpcUa_DiagnosticInfo 25

//> Status codes
#define OpcUa_BadUnexpectedError	0x80010000
#define OpcUa_BadCommunicationError	0x80050000
#define OpcUa_BadEncodingError		0x80060000
#define OpcUa_BadDecodingError		0x80070000
#define OpcUa_BadEncodingLimitsExceeded	0x80080000
#define OpcUa_BadTimeout		0x800A0000
#define OpcUa_BadServiceUnsupported	0x800B0000
#define OpcUa_BadNothingToDo		0x800F0000
#define OpcUa_BadSecureChannelIdInvalid	0x80220000
#define OpcUa_BadSessionIdInvalid	0x80250000
#define OpcUa_BadNodeIdInvalid		0x80330000
#define OpcUa_BadNodeIdUnknown		0x80340000
#define OpcUa_BadAttributeIdInvalid	0x80350000
#define OpcUa_BadNotSupported		0x803D0000
#define OpcUa_BadSecurityPolicyRejected	0x80550000
#define OpcUa_BadBrowseNameInvalid	0x80600000
#define OpcUa_BadTcpMessageTypeInvalid	0x807E0000
#define OpcUa_BadTcpMessageTooLarge	0x80800000
#define OpcUa_BadTcpEndpointUrlInvalid	0x80830000
#define OpcUa_BadSecureChannelClosed	0x80860000
#define OpcUa_BadSecureChannelTokenUnknown	0x80870000
#define OpcUa_BadInvalidArgument	0x80AB0000
#define OpcUa_BadRequestTooLarge	0x80B80000
#define OpcUa_BadResponseTooLarge	0x80B90000
#define OpcUa_BadProtocolVersionUnsupported	0x80BE0000

//> Requests types
#define OpcUa_ServiceFault		397
#define OpcUa_FindServersRequest	422
#define OpcUa_FindServersResponse	425
#define OpcUa_GetEndpointsRequest	428
#define OpcUa_GetEndpointsResponse	431
#define OpcUa_OpenSecureChannelRequest	446
#define OpcUa_OpenSecureChannelResponse	449
#define OpcUa_CloseSecureChannelRequest	452
#define OpcUa_CreateSessionRequest	461
#define OpcUa_CreateSessionResponse	464
#define OpcUa_ActivateSessionRequest	467
#define OpcUa_ActivateSessionResponse	470
#define OpcUa_CloseSessionRequest	473
#define OpcUa_CloseSessionResponse	476
#define OpcUa_BrowseRequest		527
#define OpcUa_BrowseResponse		530
#define OpcUa_ReadRequest		631
#define OpcUa_ReadResponse		634
#define OpcUa_WriteRequest		673
#define OpcUa_WriteResponse		676
#define OpcUa_PublishRequest		826
#define OpcUa_PublishResponse		829

//> Object Identifiers
#define OpcUa_RootFolder		84
#define OpcUa_ObjectsFolder		85
#define OpcUa_TypesFolder		86
#define OpcUa_ViewsFolder		87
#define OpcUa_ObjectTypesFolder		88
#define OpcUa_VariableTypesFolder	89
#define OpcUa_DataTypesFolder		90
#define OpcUa_ReferenceTypesFolder	91

//> ObjectType Identifiers
#define OpcUa_BaseObjectType		58
#define OpcUa_FolderType		61
#define OpcUa_DataTypeSystemType	75
#define OpcUa_DataTypeEncodingType	76
#define OpcUa_ModellingRuleType		77
#define OpcUa_ServerType		2004
#define OpcUa_ServerCapabilitiesType	2013
#define OpcUa_ServerDiagnosticsType	2020
#define OpcUa_SessionsDiagnosticsSummaryType	2026
#define OpcUa_SessionDiagnosticsObjectType	2029
#define OpcUa_VendorServerInfoType	2033
#define OpcUa_ServerRedundancyType	2034
#define OpcUa_TransparentRedundancyType	2036
#define OpcUa_NonTransparentRedundancyType	2039
#define OpcUa_BaseEventType		2041
#define OpcUa_StateType			2307
#define OpcUa_TransitionType		2310

//> Variable Identifiers
#define OpcUa_BaseVariableType		62
#define OpcUa_BaseDataVariableType	63
#define OpcUa_PropertyType		68
#define OpcUa_Server_ServerStatus_State	2259

//> ReferenceType Identifiers
#define OpcUa_References		31
#define OpcUa_NonHierarchicalReferences	32
#define OpcUa_HierarchicalReferences	33
#define OpcUa_HasChild			34
#define OpcUa_Organizes			35
#define OpcUa_HasEventSource		36
#define OpcUa_HasModellingRule		37
#define OpcUa_HasEncoding		38
#define OpcUa_HasDescription		39
#define OpcUa_HasTypeDefinition		40
#define OpcUa_GeneratesEvent		41
#define OpcUa_Aggregates		44
#define OpcUa_HasSubtype		45
#define OpcUa_HasProperty		46
#define OpcUa_HasComponent		47
#define OpcUa_HasModelParent		50

//> DataType Identifiers
#define OpcUa_BaseDataType		24
#define OpcUa_Number			26
#define OpcUa_Integer			27
#define OpcUa_UInteger			28
#define OpcUa_Enumeration		29
#define OpcUa_Boolean			1
#define OpcUa_SByte			2
#define OpcUa_Byte			3
#define OpcUa_Int16			4
#define OpcUa_UInt16			5
#define OpcUa_Int32			6
#define OpcUa_UInt32			7
#define OpcUa_Int64			8
#define OpcUa_UInt64			9
#define OpcUa_Float			10
#define OpcUa_Double			11
#define OpcUa_String			12

//*************************************************
//* NodeId object                                 *
//*************************************************
class NodeId
{
    public:
	//Data
	enum Type	{ Numeric, String, Guid, Opaque };

	//Methods
	NodeId( ) : mNs(0), mTp(Numeric), numb(0)	{ }
	NodeId( uint32_t in, uint16_t ins = 0 );
	NodeId( const string &istr, uint16_t ins = 0, Type tp = String );
	~NodeId( );

	NodeId &operator=( NodeId &node );

	Type type( ) const	{ return mTp; }
	bool isNull( ) const	{ return (mTp == Numeric && numb == 0); }

	uint16_t ns( ) const	{ return mNs; }
	uint32_t numbVal( ) const;
	string   strVal( ) const;

	void setNs( uint16_t ins )	{ mNs = ins; }
	void setNumbVal( uint32_t in );
	void setStrVal( const string &istr, Type tp = String );

	//> Static
	static NodeId fromAddr( const string &strAddr );
	string toAddr( ) const;

    private:
	//Attributes
	uint16_t mNs;
	Type	mTp;
	union
	{
	    uint32_t	numb;
	    string	*str;
	};
};

//*************************************************
//* TProtIn                                       *
//*************************************************
class TProt;

class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer, const string &sender );

	TProt &owner( );

	//> Special
	string mkError( uint32_t errId, const string &err = "" );

    public:
	//Attributes
	bool mNotFull;
	string mBuf;
};

//*************************************************
//* OPCSess                                       *
//*************************************************
class OPCSess
{
    public:
	//Methods
	OPCSess( const string &iName, double iTInact ) :
	    name(iName), tInact(vmax(iTInact,1)), tAccess(SYS->curTime())	{ }
	OPCSess( ) : tInact(0), tAccess(0)	{ }

	//Attributes
	string		name;
	vector<uint32_t> secCnls;
	double		tInact;
	long long	tAccess;
};

//*************************************************
//* OPCEndPoint                                   *
//*************************************************
class OPCEndPoint : public TCntrNode, public TConfig
{
    public:
	//Data
	enum SerializerType	{ Binary };
	enum MessageSecurityMode{ None = 1, Sign, SignAndEncrypt };

	class SecuritySetting
	{
	    public:
		SecuritySetting( const string &iplc, char imMode ) :
		    policy(iplc), messageMode((MessageSecurityMode)imMode) { }

	    string		policy;
	    MessageSecurityMode	messageMode;
	};

	//Methods
	OPCEndPoint( const string &iid, const string &db, TElem *el );
	~OPCEndPoint( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string name( );
	string descr( )		{ return mDscr; }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( )	{ return mEn; }
	SerializerType serType( )	{ return (SerializerType)mSerType; }
	string url( )		{ return mURL; }
	string cert( );
	string pvKey( );

	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; modif(); }
	void setDescr( const string &idsc )	{ mDscr = idsc; modif(); }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	//> Security policies
	int secSize( )				{ return mSec.size(); }
	string secPolicy( int isec );
	MessageSecurityMode secMessageMode( int isec );

	//> Sessions
	int sessCreate( const string &iName, double iTInact );
	bool sessActivate( int sid, uint32_t secCnl, bool check = false );
	void sessClose( int sid );
	OPCSess sessGet( int sid );

	string tcpReq( int reqTp, const string &rb );

	TProt &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	string nodeName( )	{ return mId; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg );

	XMLNode *nodeReg( const NodeId &parent, const NodeId &ndId, const string &name,
	    int ndClass, const NodeId &refTypeId, const NodeId &typeDef = 0 );

	//Attributes
	string	&mId, &mName, &mDscr, &mURL;
	int	&mSerType;
	bool	&mAEn, mEn;
	string	mDB;

	float	cntReq;
	vector<SecuritySetting>	mSec;
	vector<OPCSess>		mSess;

	XMLNode	objTree;
	map<string,XMLNode*>	ndMap;
};

//*************************************************
//* SecCnl                                        *
//*************************************************
class SecCnl
{
    public:
	//Methods
	SecCnl( const string &iEp, uint32_t iTokenId, int32_t iLifeTm ) :
	    TokenId(iTokenId), tCreate(TSYS::curTime()), tLife(vmax(600000,iLifeTm)), endPoint(iEp)	{ }
	SecCnl( ) : TokenId(0), tCreate(TSYS::curTime()), tLife(600000)	{ }

	//Attributes
	string		endPoint;
	long long	tCreate;
	int32_t		tLife;
	uint32_t	TokenId;
	string		clCert;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Data
	enum NodeClasses	{ NC_Object=1, NC_Variable=2, NC_Method=4, NC_ObjectType=8, NC_VariableType=16, NC_ReferenceType=32, NC_DataType=64, NC_View=128 };
	enum browseDirection	{ BD_FORWARD, BD_INVERSE, BD_BOTH };
	enum timestampsToReturn	{ TS_SOURCE, TS_SERVER, TS_BOTH, TS_NEITHER };
	enum Access		{ ACS_Read=0x01, ACS_Write=0x02, ACS_HistRead=0x04, ACS_HistWrite=0x08, ACS_SemChange=0x10 };
	enum RefDscrResMask	{ RdRm_RefType=0x01, RdRm_IsForward=0x02, RdRm_NodeClass=0x04, RdRm_BrowseName=0x08,
				    RdRm_DisplayName=0x10, RdRm_TypeDef=0x20 };
	enum AttrIds		{ AId_NodeId=1, AId_NodeClass, AId_BrowseName, AId_DisplayName, AId_Descr, AId_WriteMask, AId_UserWriteMask,
				    AId_IsAbstract, AId_Symmetric, AId_InverseName, AId_ContainsNoLoops, AId_EventNotifier, AId_Value,
				    AId_DataType, AId_ValueRank, AId_ArrayDimensions, AId_AccessLevel, AId_UserAccessLevel,
				    AId_MinimumSamplingInterval, AId_Historizing, AId_Executable, AId_UserExecutable };

	//Methods
	TProt( string name );
	~TProt( );

	void modStart( );
	void modStop( );

	//> Server's functions
	void epList( vector<string> &ls )	{ chldList(mEndPnt,ls); }
	bool epPresent( const string &id )	{ return chldPresent(mEndPnt,id); }
	void epAdd( const string &id, const string &db = "*.*" );
	void epDel( const string &id )		{ chldDel(mEndPnt,id); }
	AutoHD<OPCEndPoint> epAt( const string &id )	{ return chldAt(mEndPnt,id); }

	void discoveryUrls( vector<string> &ls );

	//> Channel manipulation functions
	int chnlOpen( const string &iEp, int32_t lifeTm = 0 );
	void chnlClose( int cid );
	SecCnl chnlGet( int cid );

	TElem &endPntEl( )			{ return mEndPntEl; }

	void outMess( XMLNode &io, TTransportOut &tro );

	//> Generic variables
	static string applicationUri( );
	static string productUri( );
	static string applicationName( );

	//> Protocol's data process
	static const char *iVal( const string &buf, int &off, char vSz );
	static int32_t iN( const string &rb, int &off, char vSz );
	static uint32_t iNu( const string &rb, int &off, char vSz );
	static double iR( const string &rb, int &off, char vSz = 4 );
	static string iS( const string &buf, int &off );
	static string iSl( const string &buf, int &off, string *locale = NULL );
	static string iSqlf( const string &buf, int &off, uint16_t *nsIdx = NULL );
	static long long iTm( const string &buf, int &off );
	static NodeId iNodeId( const string &buf, int &off );
	static void iDataValue( const string &buf, int &off, XMLNode &nVal );

	static void oN( string &buf, int32_t val, char sz, int off = -1 );
	static void oNu( string &buf, uint32_t val, char sz, int off = -1 );
	static void oR( string &buf, double val, char sz = 4 );
	static void oS( string &buf, const string &val );
	static void oSl( string &buf, const string &val, const string &locale = "" );
	static void oSqlf( string &buf, const string &val, uint16_t nsIdx = 0 );
	static void oTm( string &buf, long long val );
	static void oNodeId( string &buf, const NodeId &val );
	static void oRef( string &buf, uint32_t resMask, const NodeId &nodeId, const NodeId &refTypeId,
	    bool isForward, const string &name, uint32_t nodeClass, const NodeId &typeDef );
	static void oDataValue( string &buf, uint8_t eMsk, const TVariant &vl, uint8_t vEMsk = 0, long long srcTmStmp = 0 );

	static string certPEM2DER( const string &certPem );
	static string certThumbprint( const string &certPem );
	static string messDecPKey( const string &mess, const string &keyPem );

	Res &nodeRes( )		{ return nRes; }

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	int	mEndPnt;

	TElem	mEndPntEl;

	map<uint32_t,SecCnl>	mSecCnl;
	uint32_t		mSecCnlIdLast;

	Res	nRes;
};

extern TProt *modPrt;

} //End namespace OPC_UA

#endif //MOD_PRT_H

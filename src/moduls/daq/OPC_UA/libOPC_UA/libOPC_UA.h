
//OpenSCADA OPC_UA implementation library file: libOPC_UA.h
/******************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>	      *
 *									      *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as	      *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *									      *
 *   This library is distributed in the hope that it will be useful,	      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of	      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	      *
 *   GNU Lesser General Public License for more details.		      *
 *									      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the			      *
 *   Free Software Foundation, Inc.,					      *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.		      *
 ******************************************************************************/

#ifndef LIBOPC_UA_H
#define LIBOPC_UA_H

//Used for some specific problems reprodiction and debug
//#define DEBUG_SPEC

#include <stdint.h>

#include <string>
#include <deque>
#include <vector>
#include <map>
#include <algorithm>

using std::string;
using std::deque;
using std::vector;
using std::pair;
using std::map;

namespace OPC
{

//Constants
#define OpcUa_ProtocolVersion	0
#define OpcUa_ReciveBufferSize	0x10000
#define OpcUa_SendBufferSize	0x10000
#define OpcUa_MaxMessageSize	0x1000000
#define OpcUa_MaxChunkCount	5000

#define OpcUa_NPosID		0xFFFFFFFF

#define NS_OPC_UA		0
#define NS_SERVER		1

//Built-in Data Types
#define OpcUa_Boolean		1
#define OpcUa_SByte		2
#define OpcUa_Byte		3
#define OpcUa_Int16		4
#define OpcUa_UInt16		5
#define OpcUa_Int32		6
#define OpcUa_UInt32		7
#define OpcUa_Int64		8
#define OpcUa_UInt64		9
#define OpcUa_Float		10
#define OpcUa_Double		11
#define OpcUa_String		12
#define OpcUa_DateTime		13
#define OpcUa_Guid		14
#define OpcUa_ByteString	15
#define OpcUa_XmlElement	16
#define OpcUa_NodeId		17
#define OpcUa_ExpandedNodeId	18
#define OpcUa_StatusCode	19
#define OpcUa_QualifiedName	20
#define OpcUa_LocalizedText	21
#define OpcUa_Structure		22
#define OpcUa_DataValue		23
#define OpcUa_DiagnosticInfo	25

//Status codes
#define OpcUa_BadUnexpectedError	0x80010000
#define OpcUa_BadCommunicationError	0x80050000
#define OpcUa_BadEncodingError		0x80060000
#define OpcUa_BadDecodingError		0x80070000
#define OpcUa_BadEncodingLimitsExceeded	0x80080000
#define OpcUa_BadTimeout		0x800A0000
#define OpcUa_BadServiceUnsupported	0x800B0000
#define OpcUa_BadNothingToDo		0x800F0000
#define OpcUa_BadTooManyOperations	0x80100000
#define OpcUa_BadUserAccessDenied	0x801F0000
#define OpcUa_BadSecureChannelIdInvalid	0x80220000
#define OpcUa_BadSessionIdInvalid	0x80250000
#define OpcUa_BadSubscriptionIdInvalid	0x80280000
#define OpcUa_BadNodeIdInvalid		0x80330000
#define OpcUa_BadNodeIdUnknown		0x80340000
#define OpcUa_BadAttributeIdInvalid	0x80350000
#define OpcUa_BadNotSupported		0x803D0000
#define OpcUa_BadMonitoredItemIdInvalid	0x80420000
#define OpcUa_BadFilterNotAllowed	0x80450000
#define OpcUa_BadContinuationPointInvalid	0x804A0000
#define OpcUa_BadNoContinuationPoints	0x804B0000
#define OpcUa_BadSecurityModeRejected	0x80540000
#define OpcUa_BadSecurityPolicyRejected	0x80550000
#define OpcUa_BadApplicationSignatureInvalid	0x80580000
#define OpcUa_BadBrowseNameInvalid	0x80600000
#define OpcUa_BadNoMatch		0x806F0000
#define OpcUa_BadWriteNotSupported	0x80730000
#define OpcUa_BadTooManySubscriptions	0x80770000
#define OpcUa_BadSequenceNumberUnknown	0x807A0000
#define OpcUa_BadTcpMessageTypeInvalid	0x807E0000
#define OpcUa_BadTcpMessageTooLarge	0x80800000
#define OpcUa_BadTcpEndpointUrlInvalid	0x80830000
#define OpcUa_BadSecureChannelClosed	0x80860000
#define OpcUa_BadSecureChannelTokenUnknown	0x80870000
#define OpcUa_BadInvalidArgument	0x80AB0000
#define OpcUa_BadRequestTooLarge	0x80B80000
#define OpcUa_BadResponseTooLarge	0x80B90000
#define OpcUa_BadProtocolVersionUnsupported	0x80BE0000

//Requests types
#define OpcUa_ServiceFault			397
#define OpcUa_FindServersRequest		422
#define OpcUa_FindServersResponse		425
#define OpcUa_GetEndpointsRequest		428
#define OpcUa_GetEndpointsResponse		431
#define OpcUa_OpenSecureChannelRequest		446
#define OpcUa_OpenSecureChannelResponse		449
#define OpcUa_CloseSecureChannelRequest		452
#define OpcUa_CreateSessionRequest		461
#define OpcUa_CreateSessionResponse		464
#define OpcUa_ActivateSessionRequest		467
#define OpcUa_ActivateSessionResponse		470
#define OpcUa_CloseSessionRequest		473
#define OpcUa_CloseSessionResponse		476
#define OpcUa_BrowseRequest			527
#define OpcUa_BrowseResponse			530
#define OpcUa_BrowseNextRequest			533
#define OpcUa_BrowseNextResponse		536
#define OpcUa_TranslateBrowsePathsToNodeIdsRequest	554
#define OpcUa_TranslateBrowsePathsToNodeIdsResponse	557
#define OpcUa_ReadRequest			631
#define OpcUa_ReadResponse			634
#define OpcUa_WriteRequest			673
#define OpcUa_WriteResponse			676
#define OpcUa_CreateMonitoredItemsRequest	751
#define OpcUa_CreateMonitoredItemsResponse	754
#define OpcUa_ModifyMonitoredItemsRequest	763
#define OpcUa_ModifyMonitoredItemsResponse	766
#define OpcUa_SetMonitoringModeRequest		769
#define OpcUa_SetMonitoringModeResponse		772
#define OpcUa_DeleteMonitoredItemsRequest	781
#define OpcUa_DeleteMonitoredItemsResponse	784
#define OpcUa_CreateSubscriptionRequest		787
#define OpcUa_CreateSubscriptionResponse	790
#define OpcUa_ModifySubscriptionRequest		793
#define OpcUa_ModifySubscriptionResponse	796
#define OpcUa_SetPublishingModeRequest		799
#define OpcUa_SetPublishingModeResponse		802
#define OpcUa_PublishRequest			826
#define OpcUa_PublishResponse			829
#define OpcUa_RepublishRequest			832
#define OpcUa_RepublishResponse			835
#define OpcUa_TransferSubscriptionsRequest	841
#define OpcUa_TransferSubscriptionsResponse	844
#define OpcUa_DeleteSubscriptionsRequest	847
#define OpcUa_DeleteSubscriptionsResponse	850

//Object Identifiers
#define OpcUa_RootFolder		84
#define OpcUa_ObjectsFolder		85
#define OpcUa_TypesFolder		86
#define OpcUa_ViewsFolder		87
#define OpcUa_ObjectTypesFolder		88
#define OpcUa_VariableTypesFolder	89
#define OpcUa_DataTypesFolder		90
#define OpcUa_ReferenceTypesFolder	91
#define OpcUa_AnonymousIdentityToken	321
#define OpcUa_UserNameIdentityToken	324
#define OpcUa_X509IdentityToken		327
#define OpcUa_LiteralOperand		597
#define OpcUa_EventFilter		727
#define OpcUa_DataChangeNotification	811
#define OpcUa_Server			2253

//ObjectType Identifiers
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
#define OpcUa_GeneralModelChangeEventType 2133
#define OpcUa_StateType			2307
#define OpcUa_TransitionType		2310

//Variable Identifiers
#define OpcUa_BaseVariableType		62
#define OpcUa_BaseDataVariableType	63
#define OpcUa_PropertyType		68
#define OpcUa_Server_ServerArray	2254
#define OpcUa_Server_NamespaceArray	2255
#define OpcUa_Server_ServerStatus	2256
#define OpcUa_Server_ServerStatus_State	2259
#define OpcUa_Server_Auditing		2994

//ReferenceType Identifiers
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

//DataType Identifiers
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
#define OpcUa_ServerStatusDataType	862

//VariableType Identifiers
#define OpcUa_ServerStatusType		2138

enum SerializerType	{ ST_Binary };
enum SC_ReqTP		{ SC_ISSUE = 0, SC_RENEW = 1 };
enum MessageSecurityMode{ MS_None = 1, MS_Sign, MS_SignAndEncrypt };
enum NodeClasses	{ NC_Object = 1, NC_Variable = 2, NC_Method = 4, NC_ObjectType = 8, NC_VariableType = 16,
			  NC_ReferenceType = 32, NC_DataType = 64, NC_View = 128 };
enum BrowseDirection	{ BD_FORWARD = 0, BD_INVERSE, BD_BOTH };
enum TimestampsToReturn	{ TS_SOURCE = 0, TS_SERVER, TS_BOTH, TS_NEITHER };
enum Access		{ ACS_Read = 0x01, ACS_Write = 0x02, ACS_HistRead = 0x04, ACS_HistWrite = 0x08, ACS_SemChange = 0x10 };
enum RefDscrResMask	{ RdRm_RefType = 0x01, RdRm_IsForward = 0x02, RdRm_NodeClass = 0x04, RdRm_BrowseName = 0x08,
			  RdRm_DisplayName = 0x10, RdRm_TypeDef = 0x20 };
enum AttrIds		{ Aid_Error = 0, AId_NodeId, AId_NodeClass, AId_BrowseName, AId_DisplayName, AId_Descr, AId_WriteMask,
			  AId_UserWriteMask, AId_IsAbstract, AId_Symmetric, AId_InverseName, AId_ContainsNoLoops, AId_EventNotifier,
			  AId_Value, AId_DataType, AId_ValueRank, AId_ArrayDimensions, AId_AccessLevel, AId_UserAccessLevel,
			  AId_MinimumSamplingInterval, AId_Historizing, AId_Executable, AId_UserExecutable };
enum SubScrSt		{ SS_CUR = 0, SS_CLOSED, SS_CREATING, SS_NORMAL, SS_LATE, SS_KEEPALIVE };
enum MonitoringMode 	{ MM_CUR = -1, MM_DISABLED = 0, MM_SAMPLING, MM_REPORTING };

//* External functions                        */
extern int64_t curTime( );
extern string int2str( int val );
extern string uint2str( unsigned val );
extern string ll2str( int64_t val );
extern string real2str( double val, int prec = 15, char tp = 'g' );
extern string strParse( const string &path, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
static string strLine( const string &str, int level, int *off = NULL );
extern string strMess( const char *fmt, ... );

//*************************************************
//* OPCError					  *
//*************************************************
class OPCError
{
    public:
	//Methods
	OPCError( const char *fmt, ... );
	OPCError( int cod, const char *fmt, ... );

	//Attributes
	int	cod;
	string	mess;
};

//*************************************************
//* XML_N					  *
//*************************************************
class XML_N
{
    public:
	//Methods
	XML_N( const string &name = "" ) : mName(name), mText(""), mParent(NULL)	{  }
	XML_N( const XML_N &nd );
	~XML_N( )				{ clear(); }

	XML_N	&operator=( const XML_N &prm );

	string	name( ) const			{ return mName; }
	XML_N*	setName( const string &s )	{ mName = s; return this; }

	string	text( bool childs = false, bool recursive = false ) const;
	XML_N*	setText( const string &s, bool childs = false );

	void	attrList( vector<string> &list ) const;
	XML_N*	attrDel( const string &name );
	void	attrClear( );
	string	attr( const string &name, bool caseSens = true ) const;
	XML_N*	setAttr( const string &name, const string &val );

	XML_N*	clear( );

	bool	childEmpty( ) const		{ return mChildren.empty(); }
	unsigned childSize( ) const		{ return mChildren.size(); }
	void	childAdd( XML_N *nd );
	XML_N*	childAdd( const string &name = "" );
	int	childIns( unsigned id, XML_N *nd );
	XML_N*	childIns( unsigned id, const string &name = "" );
	void	childDel( const unsigned id );
	void	childDel( XML_N *nd );
	void	childClear( const string &name = "" );
	XML_N*	childGet( const int, bool noex = false ) const;
	XML_N*	childGet( const string &name, const int numb = 0, bool noex = false ) const;
	XML_N*	childGet( const string &attr, const string &name, bool noex = false ) const;
	XML_N*	getElementBy( const string &attr, const string &val );

	XML_N*	parent( )			{ return mParent; }
	XML_N*	root( );

    private:
	//Attributes
	string	mName;
	string	mText;
	vector<XML_N*>			mChildren;
	vector<pair<string,string> >	mAttr;
	XML_N	*mParent;
};

//*************************************************
//* NodeId object				  *
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

	NodeId &operator=( const NodeId &node );

	Type	type( ) const	{ return mTp; }
	bool	isNull( ) const	{ return (mTp == Numeric && numb == 0); }

	uint16_t ns( ) const	{ return mNs; }
	uint32_t numbVal( ) const;
	string	strVal( ) const;

	void setNs( uint16_t ins )	{ mNs = ins; }
	void setNumbVal( uint32_t in );
	void setStrVal( const string &istr, Type tp = String );

	// Static
	static NodeId fromAddr( const string &strAddr );
	string toAddr( ) const;

    private:
	//Attributes
	uint16_t mNs;
	Type	mTp;
#ifdef DEBUG_SPEC
	union
	{
	    uint32_t	numb;
	    string	*str;
	};
#else
	uint32_t numb;
	string   str;
#endif
};

//*************************************************
//* Protocol OPC UA				  *
//*************************************************
class UA
{
    public:
	//Data
	class SecuritySetting
	{
	    public:
		SecuritySetting( const string &iplc, char imMode ) : policy(iplc), messageMode((MessageSecurityMode)imMode) { }
		SecuritySetting( ) : policy("None"), messageMode(MS_None)	{ }

	    string		policy;
	    MessageSecurityMode	messageMode;
	};

	//Methods
	UA( );
	~UA( );

	virtual string lang2CodeSYS( )	{ return "en"; }
	virtual void debugMess( const string &mess ) { }

	// Protocol's data processing
	//----------------------------------------------------
	static string iErr( const string &buf, int &off );
	static const char *iVal( const string &buf, int &off, char vSz );
	static int32_t iN( const string &rb, int &off, char vSz );
	static uint32_t iNu( const string &rb, int &off, char vSz );
	static double iR( const string &rb, int &off, char vSz = 4 );
	static string iS( const string &buf, int &off );
	static string iSl( const string &buf, int &off, string *locale = NULL );
	static string iSqlf( const string &buf, int &off, uint16_t *nsIdx = NULL );
	static int64_t iTm( const string &buf, int &off );
	static NodeId iNodeId( const string &buf, int &off );
	static void iDataValue( const string &buf, int &off, XML_N &nVal );

	static void oN( string &buf, int32_t val, char sz, int off = -1 );
	static void oNu( string &buf, uint32_t val, char sz, int off = -1 );
	static void oR( string &buf, double val, char sz = 4 );
	static void oS( string &buf, const string &val, int off = -1 );
	static void oSl( string &buf, const string &val, const string &locale = "" );
	static void oSqlf( string &buf, const string &val, uint16_t nsIdx = 0 );
	static void oTm( string &buf, int64_t val );
	static void oNodeId( string &buf, const NodeId &val );
	static void oRef( string &buf, uint32_t resMask, const NodeId &nodeId, const NodeId &refTypeId,
			bool isForward, const string &name, uint32_t nodeClass, const NodeId &typeDef );
	void oDataValue( string &buf, uint8_t eMsk, const string &vl, uint8_t vEMsk = 0, int64_t srcTmStmp = 0 );

	static string randBytes( int num );
	static string certPEM2DER( const string &certPem );
	static string certDER2PEM( const string &certDer );
	static string certThumbprint( const string &certPem );
	static string asymmetricEncrypt( const string &mess, const string &certPem, const string &secPolicy );
	static string asymmetricDecrypt( const string &mess, const string &pvKeyPem, const string &secPolicy );
	static bool asymmetricVerify( const string &mess, const string &sign, const string &certPem );
	static string asymmetricSign( const string &mess, const string &pvPem );
	static int asymmetricKeyLength( const string &keyCertPem );
	static string deriveKey( const string &secret, const string &seed, int keyLen );
	static string symmetricEncrypt( const string &mess, const string &keySet, const string &secPolicy );
	static string symmetricDecrypt( const string &mess, const string &keySet, const string &secPolicy );
	static string symmetricSign( const string &mess, const string &keySet, const string &secPolicy );
};

//*************************************************
//* Protocol OPC UA client part			  *
//*************************************************
class Client: public UA
{
    public:
	//Data
	class SClntSess
	{
	    public:
		//Methods
		SClntSess( )		{ clearFull( ); }
		void clearSess( )	{ sesId = authTkId = ""; sesLifeTime = 1.2e6; }
		void clearFull( )
		{
		    endPoint = servCert = clKey = servKey = "";
		    secPolicy = "None"; secMessMode = 1;
		    secChnl = secToken = reqHndl = 0;
		    sqNumb = 33;
		    sqReqId = 1;
		    secLifeTime = 0;
		    sessOpen = 0;
		    clearSess( );
		}

		string		endPoint;
		uint32_t	secChnl;
		uint32_t	secToken;
		uint32_t	sqNumb;
		uint32_t	sqReqId;
		uint32_t	reqHndl;
		int		secLifeTime;
		string		sesId;
		string		authTkId;
		int64_t		sessOpen;
		double		sesLifeTime;
		string		servCert;
		string		secPolicy;
		char		secMessMode;
		string		clKey, servKey;
	};

	//Methods
	Client( );
	~Client( );

	// Main variables
	virtual string	sessionName( ) = 0;
	virtual string	endPoint( ) = 0;
	virtual string	secPolicy( ) = 0;
	virtual int	secMessMode( ) = 0;
	virtual string	cert( ) = 0;
	virtual string	pvKey( ) = 0;
	virtual string	authData( ) = 0;	//Empty			- anonymous
						//{User}\n{Password}	- by user and password

	// External imlementations
	virtual int	messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0 ) = 0;

	// Main call methods
	virtual void	protIO( XML_N &io );
	virtual void	reqService( XML_N &io );

    protected:
	//Attributes
	SClntSess	sess;
};

//*************************************************
//* Protocol OPC UA server part			  *
//*************************************************
class Server: public UA
{
    public:
	//Data
	//* Security Channel
	class SecCnl
	{
	    public:
	    //Methods
	    SecCnl( const string &iEp, uint32_t iTokenId, int32_t iLifeTm, const string &iClCert, const string &iSecPolicy, char iSecMessMode );
	    SecCnl( );

	    //Attributes
	    string	endPoint;
	    string	secPolicy;
	    char	secMessMode;
	    int64_t	tCreate;
	    int32_t	tLife;
	    uint32_t	TokenId, TokenIdPrev;
	    string	clCert;
	    string	servKey, clKey;
	};
	//* Session
	class Sess
	{
	    public:
	    //Data
	    //* Connection point for Browse and BrowseNext services
	    class ContPoint
	    {
		public:
		//Methods
		ContPoint( ) : brDir(0), refPerN(100), nClassMask(0), resMask(0) { }
		ContPoint( const string &i_brNode, const string &i_lstNode,
			uint32_t i_brDir, uint32_t i_refPerN, const string &i_refTypeId, uint32_t i_nClassMask, uint32_t i_resMask ) :
		    brDir(i_brDir), refPerN(i_refPerN), nClassMask(i_nClassMask), resMask(i_resMask), brNode(i_brNode), lstNode(i_lstNode),
		    refTypeId(i_refTypeId) { }

		bool empty( ) const	{ return brNode.empty(); }

		//Attributes
		uint32_t brDir,		//Browse direction
			refPerN,	//References per node
			nClassMask,	//Node class mask
			resMask;	//Result mask
		string	brNode, lstNode,
			refTypeId;	//Reference type id
	    };

	    //Methods
	    Sess( const string &iName, double iTInact );
	    Sess( );

	    //Attributes
	    string	name, inPrtId,
			idPolicyId, user;
	    vector<uint32_t> secCnls;
	    double	tInact;
	    int64_t	tAccess;
	    string	servNonce;

	    map<string, ContPoint> cntPnts;	//>> Continuation points

	    // Subscription
	    deque<string>	publishReqs;	//Publish requests queue
	};

	//* Subscription object by monitoreditems set
	class Subscr
	{
	    public:
		//Data
		//* Monitored item
		class MonitItem
		{
		    public:
			class Val
			{
			    public:
				Val( ) { }
				Val( const string &ivl, int64_t itm ) : vl(ivl), tm(itm) { }

				string	vl;
				int64_t	tm;
			};

			//Methods
			MonitItem( ) : md(MM_DISABLED), aid(0), tmToRet(TS_SOURCE), smplItv(1000), qSz(OpcUa_NPosID),
			    dO(false), cH(0), vTp(0), dtTm(0)	{ }

			//Attributes
			MonitoringMode	md;		//Monitoring mode
			NodeId		nd;		//Target node
			uint32_t	aid;		//The node's attribute ID
			TimestampsToReturn tmToRet;	//Timestamps to return
			double		smplItv;	//Sample interval
			uint32_t	qSz;		//Queue size
			bool		dO;		//Discard oldest
			uint32_t	cH;		//Client handle

			int		vTp;		//Values type
			int64_t		dtTm;		//Last value time
			deque<Val>	vQueue;		//Values queue
		};

		//Methods
		Subscr( ) : st(SS_CLOSED), sess(-1), en(false), publInterv(100), seqN(1),
		    cntrLifeTime(12000), wLT(0), cntrKeepAlive(50), wKA(0), maxNotPerPubl(0), pr(0) 	{ }

		Subscr copy( bool noWorkData = true );
		SubScrSt setState( SubScrSt st = SS_CUR );

		//Attributes
		SubScrSt st;			//Subscription status
		int	sess;			//Session assign
		bool	en;			//Enable state
		double	publInterv;		//Publish interval (ms)
		uint32_t seqN,			//Sequence number for responds, rolls over 1, no increment for KeepAlive messages
			 cntrLifeTime, wLT,	//Counter after that miss notifications from client remove the object and
			 cntrKeepAlive, wKA,	//Counter after that neet send empty publish respond
						//send StatusChangeNotification with Bad_Timeout
			 maxNotPerPubl;		//Maximum notifications per single Publish response
		uint8_t	pr;			//Priority
		vector<MonitItem> mItems;
		deque<string> retrQueue;	//Retransmission queue; used by Republish request;
						//cleared to deep by KeepAlive! or by field Acknowledgements sets
	};

	//* End Point
	class EP
	{
	    friend class Server;
	    public:
		//Methods
		EP( Server *serv );
		~EP( );

		virtual string id( ) = 0;
		virtual string url( ) = 0;
		virtual string cert( ) = 0;
		virtual string pvKey( ) = 0;
		virtual double subscrProcPer( ) = 0;	//Generic minimum cycle period of publishes and the data processing

		// Limits
		virtual uint32_t limSubScr( )	{ return 10; }
		virtual uint32_t limMonitItms( ){ return 1000; }

		bool enableStat( )	{ return mEn; }

		virtual void setEnable( bool vl );
		void subScrCycle( unsigned cntr );	//Subscriptions processing cycle

		// Security policies
		int secSize( )		{ return mSec.size(); }
		string secPolicy( int isec );
		MessageSecurityMode secMessageMode( int isec );

		// Sessions
		int sessCreate( const string &iName, double iTInact );
		void sessServNonceSet( int sid, const string &servNonce );
		virtual uint32_t sessActivate( int sid, uint32_t secCnl, bool check = false,
		    const string &inPrtId = "", const XML_N &identTkn = XML_N() );
		void sessClose( int sid );
		Sess sessGet( int sid );
		//  Continuation points by Browse and BrowseNext
		Sess::ContPoint sessCpGet( int sid, const string &cpId );
		void sessCpSet( int sid, const string &cpId, const Sess::ContPoint &cp = Sess::ContPoint() );	//Empty "cp" remove "cpId"
		//  Subsciption
		uint32_t subscrSet( uint32_t ssId, SubScrSt st, bool en = false, int sess = -1,	// "sId" = 0 for new create
		    double publInterv = 0, uint32_t cntrLifeTime = 0, uint32_t cntrKeepAlive = 0,
		    uint32_t maxNotePerPubl = OpcUa_NPosID, int pr = -1 );
		Subscr subscrGet( uint32_t ssId, bool noWorkData = true );
		//  Monitored items
		uint32_t mItSet( uint32_t ssId, uint32_t mItId, MonitoringMode md = MM_CUR,	// "mItId" = 0 for new create
		    const NodeId &nd = NodeId(), uint32_t aid = OpcUa_NPosID, TimestampsToReturn tmToRet = TimestampsToReturn(-1),
		    double smplItv = -2, uint32_t qSz = OpcUa_NPosID, int8_t dO = -1, uint32_t cH = OpcUa_NPosID );
		Subscr::MonitItem mItGet( uint32_t ssId, uint32_t mItId );

		virtual uint32_t reqData( int reqTp, XML_N &req );

	    protected:
		//Methods
		XML_N *nodeReg( const NodeId &parent, const NodeId &ndId, const string &name,
		    int ndClass, const NodeId &refTypeId, const NodeId &typeDef = 0 );
		Sess *sessGet_( int sid );	//Unresourced

		//Attributes
		char			mEn;

		uint64_t		cntReq;

		vector<SecuritySetting>	mSec;
		vector<Sess>		mSess;
		vector<Subscr>		mSubScr;	//Subscriptions list

		XML_N			objTree;
		map<string, XML_N*>	ndMap;
		pthread_mutex_t		mtxData;

		Server			*serv;
	};

	//Methods
	Server( );
	~Server( );

	// Generic variables
	virtual string applicationUri( ) = 0;
	virtual string productUri( ) = 0;
	virtual string applicationName( ) = 0;
	virtual bool debug( )	{ return false; }

	virtual void discoveryUrls( vector<string> &ls ) = 0;
	virtual bool inReq( string &request, const string &inPrtId, string *answ = NULL );
	virtual int writeToClient( const string &threadId, const string &data ) = 0;

	// Channel manipulation functions
	int chnlSet( int cid, const string &iEp, int32_t lifeTm = 0,
	    const string& iClCert = "", const string &iSecPolicy = "None", char iSecMessMode = 1 );
	void chnlClose( int cid );
	SecCnl chnlGet( int cid );
	void chnlSecSet( int cid, const string &servKey, const string &clKey );

	static string mkError( uint32_t errId, const string &err = "" );

    protected:
	//Methods
	// Request to EndPoints
	virtual void epEnList( vector<string> &ls ) = 0;
	virtual EP *epEnAt( const string &ep ) = 0;

    private:
	//Attributes
	map<uint32_t, SecCnl>	mSecCnl;
	uint32_t		mSecCnlIdLast;
};

} //End namespace OPC_UA

#endif //LIBOPC_UA

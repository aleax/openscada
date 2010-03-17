
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
#define PRT_MVER	"0.0.1"
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

//> Status codes
#define OpcUa_BadUnexpectedError	0x80010000
#define OpcUa_BadEncodingError		0x80060000
#define OpcUa_BadDecodingError		0x80070000
#define OpcUa_BadTimeout		0x800A0000
#define OpcUa_BadServiceUnsupported	0x800B0000
#define OpcUa_BadSecureChannelIdInvalid	0x80220000
#define OpcUa_BadSessionIdInvalid	0x80250000
#define OpcUa_BadNotSupported		0x803D0000
#define OpcUa_BadTcpMessageTypeInvalid	0x807E0000
#define OpcUa_BadTcpMessageTooLarge	0x80800000
#define OpcUa_BadTcpEndpointUrlInvalid	0x80830000
#define OpcUa_BadSecureChannelClosed	0x80860000
#define OpcUa_BadSecureChannelTokenUnknown	0x80870000
#define OpcUa_BadRequestTooLarge	0x80B80000
#define OpcUa_BadResponseTooLarge	0x80B90000
#define OpcUa_BadProtocolVersionUnsupported	0x80BE0000

//> Requests types
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
//* OPCEndPoint                                   *
//*************************************************
class OPCEndPoint : public TCntrNode, public TConfig
{
    public:
	//Methods
	OPCEndPoint( const string &iid, const string &db, TElem *el );
	~OPCEndPoint( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string name( );
	string descr( )		{ return mDscr; }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( )	{ return mEn; }
	string endPoint( );
	string inTransport( );
	string secPolicy( );
	string cert( );

	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; modif(); }
	void setDescr( const string &idsc )	{ mDscr = idsc; modif(); }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	TProt &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Data

	//Methods
	string nodeName( )	{ return mId; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg );

	//Attributes
	string	&mId, &mName, &mDscr;
	bool	&mAEn, mEn;
	string	mDB;

	float	cntReq;
};

//*************************************************
//* SecCnl                                        *
//*************************************************
class SecCnl
{
    public:
	//Methods
	SecCnl( uint32_t iTokenId, int32_t iLifeTm ) :
	    TokenId(iTokenId), tCreate(TSYS::curTime()), tLife(vmax(600000,iLifeTm))	{ }
	SecCnl( ) : TokenId(0), tCreate(TSYS::curTime()), tLife(600000)	{ }

	//Attributes
	long long	tCreate;
	int32_t		tLife;
	uint32_t	TokenId;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	TProt( string name );
	~TProt( );

	void modStart( );
	void modStop( );

	//> Server's functions
	void epList( vector<string> &ls )	{ chldList(mEndPnt,ls); }
	bool sPresent( const string &id )	{ return chldPresent(mEndPnt,id); }
	void sAdd( const string &id, const string &db = "*.*" );
	void sDel( const string &id )		{ chldDel(mEndPnt,id); }
	AutoHD<OPCEndPoint> sAt( const string &id )	{ return chldAt(mEndPnt,id); }

	//> Channel manipulation functions
	int chnlOpen( int32_t lifeTm = 0 );
	void chnlClose( int cid );
	SecCnl chnlGet( int cid );

	TElem &endPntEl( )			{ return mEndPntEl; }

	void outMess( XMLNode &io, TTransportOut &tro );

	//> Protocol's data process
	static const char *iVal( const string &buf, int &off, char vSz );
	static int32_t iN( const string &rb, int &off, char vSz );
	static uint32_t iNu( const string &rb, int &off, char vSz );
	static double iR( const string &rb, int &off, char vSz = 4 );
	static string iS( const string &buf, int &off );
	static string iSl( const string &buf, int &off, string *locale = NULL );
	static string iSqlf( const string &buf, int &off, uint16_t *nsIdx = NULL );
	static long long iTm( const string &buf, int &off );
	static int iNodeId( const string &buf, int &off, int *ns = NULL );

	static void oN( string &buf, int32_t val, char sz, int off = -1 );
	static void oNu( string &buf, uint32_t val, char sz, int off = -1 );
	static void oR( string &buf, double val, char sz = 4 );
	static void oS( string &buf, const string &val );
	static void oSl( string &buf, const string &val, const string &locale = "" );
	static void oSqlf( string &buf, const string &val, uint16_t nsIdx = 0 );
	static void oNodeId( string &buf, int val, int ns = 0 );
	static void oTm( string &buf, long long val );

	static string certPEM2DER( const string &spem );

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

	map<int,SecCnl>	mSecCnl;
	uint32_t mSecCnlIdLast;

	Res	nRes;
};

extern TProt *modPrt;

} //End namespace OPC_UA

#endif //MOD_PRT_H

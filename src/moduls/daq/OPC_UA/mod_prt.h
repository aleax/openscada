
//OpenSCADA module DAQ.OPC_UA file: mod_prt.h
/***************************************************************************
 *   Copyright (C) 2009-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include "libOPC_UA/libOPC_UA.h"

#undef _
#define _(mess) modPrt->I18N(mess).c_str()
#undef trS
#define trS(mess) modPrt->I18N(mess,mess_PreSave)

using std::string;
using std::map;
using namespace OSCADA;
using namespace OPC;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"OPC_UA"
#define PRT_NAME	trS("Server OPC-UA")
#define PRT_TYPE	SPRT_ID
#define PRT_SUBVER	SPRT_VER
#define PRT_MVER	"2.2.7"
#define PRT_AUTOR	trS("Roman Savochenko")
#define PRT_DESCR	trS("Provides OPC-UA server service implementation.")
#define PRT_LICENSE	"GPL2"
//*************************************************

#define NS_OpenSCADA_DAQ 4

namespace OPC_UA
{

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

	unsigned waitReqTm( )	{ return mPollTm; }

	bool mess( const string &request, string &answer );

	TProt &owner( ) const;

	//Attributes
	bool	mSubscrIn;
	unsigned mPollTm;
	int64_t	mPrevTm;
	string	mBuf, mEp;
	uint32_t mRcvBufSz, mSndBufSz, mMsgMaxSz, mChunkMaxCnt;
};

//*************************************************
//* OPCEndPoint                                   *
//*************************************************
class OPCEndPoint: public TCntrNode, public TConfig, public Server::EP
{
    public:
	//Methods
	OPCEndPoint( const string &iid, const string &db, TElem *el );
	~OPCEndPoint( );

	TCntrNode &operator=( const TCntrNode &node );

	string id( )		{ return mId; }
	string name( );
	string descr( )		{ return mDescr; }
	bool toEnable( )	{ return mAEn; }
	SerializerType serType( )	{ return (SerializerType)mSerType; }
	string url( )		{ return mURL; }
	string cert( );
	string pvKey( );
	double subscrProcPer( )	{ return 100; }

	string getStatus( );

	string DB( bool qTop = false ) const	{ return storage(mDB, qTop); }
	string tbl( ) const;
	string fullDB( bool qTop = false ) const{ return DB(qTop)+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; }
	void setDescr( const string &idsc )	{ mDescr = idsc; }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );
	void setPublish( const string &inPrtId );

	void setDB( const string &vl, bool qTop = false )	{ setStorage(mDB, vl, qTop); if(!qTop) modifG(); }

	uint32_t reqData( int reqTp, XML_N &req );

	// Limits
	uint32_t limSubScr( )			{ return mLimSubScr; }
	uint32_t limMonitItms( )		{ return mLimMonitItms; }
	uint32_t limRetrQueueTm( )		{ return mLimRetrQueueTm; }

	void setLimSubScr( uint32_t vl )	{ mLimSubScr = vmax(1,vmin(vl,1000)); modif(); }
	void setLimMonitItms( uint32_t vl )	{ mLimMonitItms = vmax(10,vmin(1000000,vl)); modif(); }
	void setLimRetrQueueTm( uint32_t vl )	{ mLimRetrQueueTm = vmin(3600, vl); modif(); }

	TProt &owner( ) const;

    protected:
	//Methods
	void load_( TConfig *cfg );
	void save_( );

    private:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &co, const TVariant &pc );

	static void *Task( void *ep );

	//Attributes
	TCfg	&mId,
		&mName,
		&mDescr,
		&mURL;
	int64_t	&mSerType;
	char	&mAEn;
	string	mDB;

	uint32_t mLimSubScr, mLimMonitItms, mLimRetrQueueTm;

	ResMtx	secRes;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol, public Server
{
    friend class OPCEndPoint;
    public:
	//Methods
	TProt( string name );
	~TProt( );

	// Generic variables
	string lang2CodeSYS( )		{ return Mess->lang2Code(); }
	string applicationUri( );
	string productUri( );
	string applicationName( );

	uint32_t clientRcvBufSz( const string &inPrtId );
	uint32_t clientSndBufSz( const string &inPrtId );
	uint32_t clientMsgMaxSz( const string &inPrtId );
	uint32_t clientChunkMaxCnt( const string &inPrtId );

	void clientRcvBufSzSet( const string &inPrtId, uint32_t vl );
	void clientSndBufSzSet( const string &inPrtId, uint32_t vl );
	void clientMsgMaxSzSet( const string &inPrtId, uint32_t vl );
	void clientChunkMaxCntSet( const string &inPrtId, uint32_t vl );

	void modStart( );
	void modStop( );

	// Server's functions
	void epList( vector<string> &ls ) const			{ chldList(mEndPnt,ls); }
	bool epPresent( const string &id ) const		{ return chldPresent(mEndPnt,id); }
	string epAdd( const string &id, const string &db = "*.*" );
	void epDel( const string &id )				{ chldDel(mEndPnt, id); }
	AutoHD<OPCEndPoint> epAt( const string &id ) const	{ return chldAt(mEndPnt, id); }

	void discoveryUrls( vector<string> &ls );
	bool inReq( string &request, const string &inPrtId, string *answ = NULL );
	int writeToClient( const string &inPrtId, const string &data );
	string clientAddr( const string &inPrtId );

	TElem &endPntEl( )			{ return mEndPntEl; }

	//void outMess( XMLNode &io, TTransportOut &tro );	//!!!! Need for translate from XMLNode to XML_N

	AutoHD<TProtIn> at( const string &id )	{ return TProtocol::at(id); }

	ResRW &nodeRes( )	{ return nRes; }

    protected:
	//Methods
	void epEn( const string &id, bool val );

	void load_( );
	void save_( );

	bool debug( );
	void debugMess( const string &mess );
	void epEnList( vector<string> &ls );
	EP *epEnAt( const string &ep );

    private:
	//Methods
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	int	mEndPnt;
	TElem	mEndPntEl;
	vector< AutoHD<OPCEndPoint> > ep_hd;

	ResRW	nRes,
		enRes;				//Resource for enable endpoints
};

extern TProt *modPrt;

} //End namespace OPC_UA

#endif //MOD_PRT_H

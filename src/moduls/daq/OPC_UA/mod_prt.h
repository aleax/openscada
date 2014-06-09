
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.h
/***************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define _(mess) modPrt->I18N(mess)

using std::string;
using std::map;
using namespace OSCADA;
using namespace OPC;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"OPC_UA"
#define PRT_NAME	_("OPC UA")
#define PRT_TYPE	SPRT_ID
#define PRT_SUBVER	SPRT_VER
#define PRT_MVER	"0.6.2"
#define PRT_AUTOR	_("Roman Savochenko")
#define PRT_DESCR	_("OPC UA protocol implementation.")
#define PRT_LICENSE	"GPL2"
//*************************************************

#define NS_OpenSCADA_DAQ 2

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

	bool mess( const string &request, string &answer );

	TProt &owner( );

    public:
	//Attributes
	string mBuf;
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

	TCntrNode &operator=( TCntrNode &node );

	string id( )		{ return mId; }
	string name( );
	string descr( )		{ return mDescr; }
	bool toEnable( )	{ return mAEn; }
	SerializerType serType( )	{ return (SerializerType)mSerType; }
	string url( )		{ return mURL; }
	string cert( );
	string pvKey( );
	double subscrProcPer( ) { return 100; }

	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; }
	void setDescr( const string &idsc )	{ mDescr = idsc; }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	uint32_t reqData( int reqTp, XML_N &req );

	TProt &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	const char *nodeName( )	{ return mId.getSd(); }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg );

	static void *Task( void *ep );

	//Attributes
	TCfg	&mId,
		&mName,
		&mDescr,
		&mURL;
	int64_t	&mSerType;
	char	&mAEn;
	string	mDB;
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

	void modStart( );
	void modStop( );

	// Server's functions
	void epList( vector<string> &ls )	{ chldList(mEndPnt,ls); }
	bool epPresent( const string &id )	{ return chldPresent(mEndPnt,id); }
	void epAdd( const string &id, const string &db = "*.*" );
	void epDel( const string &id )		{ chldDel(mEndPnt,id); }
	AutoHD<OPCEndPoint> epAt( const string &id )	{ return chldAt(mEndPnt,id); }

	void discoveryUrls( vector<string> &ls );
	bool inReq( string &request, const string &inPrtId, string *answ = NULL );
	int writeToClient( const string &inPrtId, const string &data );

	TElem &endPntEl( )			{ return mEndPntEl; }

	//void outMess( XMLNode &io, TTransportOut &tro );	//!!!! Need for translate from XMLNode to XML_N

	Res &nodeRes( )		{ return nRes; }

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

	Res	nRes;
	Res	enRes;				//Resource for enable endpoints
};

extern TProt *modPrt;

} //End namespace OPC_UA

#endif //MOD_PRT_H

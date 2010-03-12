
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
#define OpcUa_BadTcpEndpointUrlInvalid	0x80830000

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
//* OPCServer                                     *
//*************************************************
class OPCServer : public TCntrNode, public TConfig
{
    public:
	//Methods
	OPCServer( const string &iid, const string &db, TElem *el );
	~OPCServer( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string name( );
	string descr( )		{ return mDscr; }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( )	{ return mEn; }
	string endPoint( );
	string inTransport( );

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
	SecCnl( ) : TokenId(1)	{ }

	//Attributes
	time_t		tCreate;
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
	void sList( vector<string> &ls )	{ chldList(mServer,ls); }
	bool sPresent( const string &id )	{ return chldPresent(mServer,id); }
	void sAdd( const string &id, const string &db = "*.*" );
	void sDel( const string &id )		{ chldDel(mServer,id); }
	AutoHD<OPCServer> sAt( const string &id )	{ return chldAt(mServer,id); }

	//> Channel manipulation functions
	int chnlOpen( );
	void chnlClose( int cid );

	TElem &serverEl( )			{ return mServerEl; }

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
	int	mServer;

	TElem	mServerEl;

	map<int,SecCnl>	mSecCnl;
	uint32_t mSecCnlIdLast;

	Res	nRes;
};

extern TProt *modPrt;

} //End namespace OPC_UA

#endif //MOD_PRT_H

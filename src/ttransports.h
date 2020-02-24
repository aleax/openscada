
//OpenSCADA file: ttransports.h
/***************************************************************************
 *   Copyright (C) 2003-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef TTRANSPORTS_H
#define TTRANSPORTS_H

#define STR_VER		22		//TransportS type modules version
#define STR_ID		"Transport"

#include <string>

#include "tbds.h"

using std::string;

namespace OSCADA
{

//************************************************
//* TTransportIn				 *
//************************************************
class TTypeTransport;
class TTransportOut;

class TTransportIn : public TCntrNode, public TConfig
{
    public:
	//Methods
	TTransportIn( const string &id, const string &db, TElem *el );
	virtual ~TTransportIn( );

	TCntrNode &operator=( const TCntrNode &node );

	string	id( )		{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )		{ return cfg("DESCRIPT").getS(); }
	string	addr( ) const	{ return cfg("ADDR").getS(); }
	string	protocols( )	{ return cfg("PROT").getS(); }
	virtual unsigned keepAliveReqs( )	{ return 0; }
	virtual unsigned keepAliveTm( )		{ return 0; }
	virtual	string getStatus( );

	bool toStart( )		{ return mStart; }
	bool startStat( ) const	{ return runSt; }

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ cfg("NAME").setS(inm); }
	void setDscr( const string &idscr )	{ cfg("DESCRIPT").setS(idscr); }
	void setAddr( const string &addr )	{ cfg("ADDR").setS(addr); }
	void setProtocols( const string &prt )	{ cfg("PROT").setS(prt); }
	void setToStart( bool val )		{ mStart = val; modif(); }

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	virtual void start( )	{ }
	virtual void stop( );
	virtual int writeTo( const string &sender, const string &data )	{ return 0; }

	vector<AutoHD<TTransportOut> > assTrs( bool checkForCleanDisabled = false );	//Assigned transports

	// IO log
	int logLen( )	{ return mLogLen; }
	void setLogLen( int vl );
	void pushLogMess( const string &vl );

	TTypeTransport &owner( ) const;

    protected:
	//Methods
	string assTrO( const string &addr );	//Assign new output transport

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( TConfig *cfg );
	void load_( )			{ }
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	//Attributes
	bool	runSt;

    private:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	//Attributes
	TCfg	&mId;
	char	&mStart;
	string	mDB;

	ResMtx	assTrRes, mLogRes;
	vector<AutoHD<TTransportOut> >	mAssTrO;

	// IO log
	int		mLogLen;
	deque<string>	mLog;
};

//************************************************
//* TTransportOut				 *
//************************************************
class TTransportOut : public TCntrNode, public TConfig
{
    public:
	//Methods
	TTransportOut( const string &id, const string &db, TElem *el );
	virtual ~TTransportOut( );

	TCntrNode &operator=( const TCntrNode &node );

	string	id( )			{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )			{ return cfg("DESCRIPT").getS(); }
	string	addr( ) const		{ return cfg("ADDR").getS(); }
	virtual	string timings( )	{ return ""; }
	virtual	unsigned short attempts( ) { return 2; }
	int64_t	prm1( )			{ return mPrm1; }
	int64_t	prm2( )			{ return mPrm2; }
	bool	toStart( )		{ return mStart; }
	bool	startStat( ) const	{ return runSt; }
	time_t	startTm( )		{ return mStartTm; }
	int64_t	lstReqTm( )		{ return mLstReqTm; }
	virtual	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )		{ cfg("NAME").setS(inm); }
	void setDscr( const string &idscr )		{ cfg("DESCRIPT").setS(idscr); }
	void setAddr( const string &addr )		{ cfg("ADDR").setS(addr); }
	virtual void setTimings( const string &vl )	{ }
	virtual void setAttempts( unsigned short vl )	{ }
	void setPrm1( int64_t vl )			{ mPrm1 = vl; }
	void setPrm2( int64_t vl )			{ mPrm2 = vl; }
	void setToStart( bool vl )			{ mStart = vl; modif(); }

	void setDB( const string &vl )			{ mDB = vl; modifG(); }

	virtual void start( int time = 0 );
	virtual void stop( )		{ };

	virtual int messIO( const char *oBuf, int oLen, char *iBuf = NULL, int iLen = 0, int time = 0 )
	{ return 0; }

	void messProtIO( XMLNode &io, const string &prot );

	// IO log
	int logLen( )	{ return mLogLen; }
	void setLogLen( int vl );
	void pushLogMess( const string &vl );

	TTypeTransport &owner( ) const;

	ResMtx &reqRes( )		{ return mReqRes; }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &co, const TVariant &pc );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	void load_( TConfig *cfg );
	void load_( )			{ }
	void save_( );

	//Attributes
	bool	runSt;

	int64_t	mLstReqTm;

    private:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	//Attributes
	TCfg	&mId;
	char	&mStart;
	string	mDB;

	// Reserve parameters
	time_t	mStartTm;
	int64_t	mPrm1, mPrm2;
	ResMtx	mReqRes, mLogRes;

	// IO log
	int		mLogLen;
	deque<string>	mLog;
};

//************************************************
//* TTypeTransport				 *
//************************************************
class TTransportS;

class TTypeTransport: public TModule
{
    public:
	//Methods
	TTypeTransport( const string &id );
	virtual ~TTypeTransport( );

	// Input transports
	void inList( vector<string> &list ) const		{ chldList(mIn, list); }
	bool inPresent( const string &id ) const		{ return chldPresent(mIn, id); }
	string inAdd( const string &id, const string &db = "*.*" );
	void inDel( const string &id, bool complete = false )	{ chldDel(mIn, id, -1, complete); }
	AutoHD<TTransportIn> inAt( const string &id ) const	{ return chldAt(mIn, id); }

	// Output transports
	void outList( vector<string> &list ) const		{ chldList(mOut, list); }
	bool outPresent( const string &id ) const		{ return chldPresent(mOut, id); }
	string outAdd( const string &id, const string &idb = "*.*" );
	void outDel( const string &id, bool complete = false )	{ chldDel(mOut, id, -1, complete); }
	AutoHD<TTransportOut> outAt( const string &id ) const	{ return chldAt(mOut, id); }
	virtual	string outAddrHelp( )				{ return ""; }

	int outLifeTime( )		{ return mOutLifeTime; }
	void setOutLifeTime( int vl )	{ mOutLifeTime = vmax(0, vl); modif(); }

	void perSYSCall( unsigned int cnt );

	TTransportS &owner( ) const;

    protected:
	//Methods
	string optDescr( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void load_( );
	void save_( );

	virtual TTransportIn  *In( const string &name, const string &db )
	{ throw TError(nodePath().c_str(),_("Input transport is not supported!")); }
	virtual TTransportOut *Out( const string &name, const string &db )
	{ throw TError(nodePath().c_str(),_("Output transport is not supported!")); }

    private:
	//Attributes
	int	mIn, mOut,
		mOutLifeTime;	//Output transports lifetime
};

//************************************************
//* TTransportS					 *
//************************************************
class TTransportS : public TSubSYS
{
    public:
	//Data
	class ExtHost
	{
	    public:
		//Data
		enum Mode { User = 0, System, UserSystem };
		//Methods
		ExtHost( const string &iUserOpen, const string &iid, const string &iname = "", const string &itransp = "",
			 const string &iaddr = "", const string &iuser = "", const string &ipass = "", uint8_t iUpRiseLev = 0 ) :
		    userOpen(iUserOpen), id(iid), name(iname), transp(itransp), addr(iaddr),
		    user(iuser), pass(ipass), upRiseLev(iUpRiseLev), mode(-1), mdf(time(NULL)) { }

		//Attributes
		string	userOpen,	//User which open remote host
			id,		//External host id
			name,		//Name
			transp,		//Connect transport
			addr,		//External host address
			user,		//External host user
			pass;		//External host password
		uint8_t	upRiseLev;	//Underneath hosts uprising level in the cascading access, zero for disable

		int8_t	mode;		//Mode; Only dynamic set
		time_t	mdf;		//Modify time, for detect the reconnection need
	};

	//Methods
	TTransportS( );
	~TTransportS( );

	int subVer( )			{ return STR_VER; }
	void inTrList( vector<string> &ls );
	void outTrList( vector<string> &ls );

	// External hosts
	string extHostsDB( );
	void extHostList( const string &user, vector<ExtHost> &list, bool andSYS = false, int upRiseLev = -1 );
	ExtHost extHostGet( const string &user, const string &id, bool andSYS = false );
	AutoHD<TTransportOut> extHost( TTransportS::ExtHost host, const string &pref = "" );
	void extHostSet( const ExtHost &host, bool andSYS = false, bool load = false );
	void extHostDel( const string &user, const string &id, bool andSYS = false );

	// Request to remote or local OpenSCADA control interface
	int cntrIfCmd( XMLNode &node, const string &senderPref, const string &user = "" );

	void unload( );

	void subStart( );
	void subStop( );

	TElem &inEl( )			{ return elIn; }
	TElem &outEl( ) 		{ return elOut; }

	AutoHD<TTypeTransport> at( const string &iid ) const	{ return modAt(iid); }

    protected:
	void load_( );
	void save_( );

    private:
	//Methods
	string optDescr( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	TElem	elIn, elOut, elExt;

	ResRW	extHostRes;		//External hosts resource
	vector<ExtHost> extHostLs;	//External hosts list
	time_t	extHostLoad;		//Last hosts loading
};

}

#endif // TTRANSPORTS_H

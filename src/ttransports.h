
//OpenSCADA system file: ttransports.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#define STR_VER		6		//TransportS type modules version
#define STR_ID		"Transport"

#include <string>

#include "tbds.h"

using std::string;

namespace OSCADA
{

//************************************************
//* TTransportIn				 *
//************************************************
class TTipTransport;
class TTransportOut;

class TTransportIn : public TCntrNode, public TConfig
{
    public:
	//Methods
	TTransportIn( const string &id, const string &db, TElem *el );
	virtual ~TTransportIn( );

	TCntrNode &operator=( TCntrNode &node );

	string	id( )		{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )		{ return cfg("DESCRIPT").getS(); }
	string	addr( )		{ return cfg("ADDR").getS(); }
	string	protocolFull( )	{ return cfg("PROT").getS(); }
	string	protocol( );
	virtual string getStatus( );

	bool toStart( )		{ return mStart; }
	bool startStat( )	{ return run_st; }

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )		{ cfg("NAME").setS(inm); }
	void setDscr( const string &idscr )		{ cfg("DESCRIPT").setS(idscr); }
	virtual void setAddr( const string &addr )	{ cfg("ADDR").setS(addr); }
	void setProtocolFull( const string &prt )	{ cfg("PROT").setS(prt); }
	void setToStart( bool val )			{ mStart = val; modif(); }

	void setDB( const string &vl )			{ mDB = vl; modifG(); }

	virtual void start( )	{ }
	virtual void stop( );
	virtual int writeTo( const string &sender, const string &data )	{ return 0; }

	vector<AutoHD<TTransportOut> > assTrs( );	//Assigned transports

	TTipTransport &owner( );

	Res &nodeRes( )			{ return nRes; }

    protected:
	//Methods
	string assTrO( const string &addr );	//Assign new output transport

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg )     { modif(); return true; }

	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	//Attributes
	bool	run_st;

    private:
	//Methods
	const char *nodeName( )	{ return mId.getSd(); }

	//Attributes
	TCfg	&mId;
	char	&mStart;
	string	mDB;
	Res	nRes;
	vector<AutoHD<TTransportOut> >	mAssTrO;
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

	TCntrNode &operator=( TCntrNode &node );

	string	id( )		{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )		{ return cfg("DESCRIPT").getS(); }
	string	addr( )		{ return cfg("ADDR").getS(); }
	virtual string timings( ) { return ""; }
	int64_t	prm1( )		{ return mPrm1; }
	int64_t	prm2( )		{ return mPrm2; }
	bool	toStart( )	{ return mStart; }
	bool	startStat( )	{ return run_st; }
	virtual	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )		{ cfg("NAME").setS(inm); }
	void setDscr( const string &idscr )		{ cfg("DESCRIPT").setS(idscr); }
	virtual void setAddr( const string &addr )	{ cfg("ADDR").setS(addr); }
	virtual void setTimings( const string &vl )	{ }
	void setPrm1( int64_t vl )			{ mPrm1 = vl; }
	void setPrm2( int64_t vl )			{ mPrm2 = vl; }
	void setToStart( bool vl )			{ mStart = vl; modif(); }

	void setDB( const string &vl )			{ mDB = vl; modifG(); }

	virtual void start( int time = 0 )	{ };
	virtual void stop( )			{ };

	virtual int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0, bool noRes = false )
	{ return 0; }

	void messProtIO( XMLNode &io, const string &prot );

	TTipTransport &owner( );

	Res &nodeRes( )			{ return nRes; }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	void load_( );
	void save_( );

	//Attributes
	bool	run_st;

    private:
	//Methods
	const char *nodeName( )	{ return mId.getSd(); }

	//Attributes
	TCfg	&mId;
	char	&mStart;
	string	mDB;

	//> Reserve parameters
	int64_t	mPrm1, mPrm2;
	Res	nRes;
};

//************************************************
//* TTipTransport				 *
//************************************************
class TTransportS;

class TTipTransport: public TModule
{
    public:
	//Methods
	TTipTransport( const string &id );
	virtual ~TTipTransport( );

	//> Input transports
	void inList( vector<string> &list )			{ chldList(mIn,list); }
	bool inPresent( const string &name )			{ return chldPresent(mIn,name); }
	void inAdd( const string &name, const string &db = "*.*" );
	void inDel( const string &name, bool complete = false )	{ chldDel(mIn,name,-1,complete); }
	AutoHD<TTransportIn> inAt( const string &name )		{ return chldAt(mIn,name); }

	//> Output transports
	void outList( vector<string> &list )			{ chldList(mOut,list); }
	bool outPresent( const string &name )			{ return chldPresent(mOut,name); }
	void outAdd( const string &name, const string &idb = "*.*" );
	void outDel( const string &name, bool complete = false ){ chldDel(mOut,name,-1,complete); }
	AutoHD<TTransportOut> outAt( const string &name )	{ return chldAt(mOut,name); }

	TTransportS &owner( );

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	virtual TTransportIn  *In( const string &name, const string &db )
	{ throw TError(nodePath().c_str(),_("Input transport no support!")); }
	virtual TTransportOut *Out( const string &name, const string &db )
	{ throw TError(nodePath().c_str(),_("Output transport no support!")); }

    private:
	//Attributes
	int	mIn, mOut;
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
		ExtHost( const string &iuser_open, const string &iid, const string &iname = "",
			const string &itransp = "", const string &iaddr = "", const string &iuser = "", const string &ipass = "" ) :
		    user_open(iuser_open), id(iid), name(iname), transp(itransp), addr(iaddr),
		    user(iuser), pass(ipass), link_ok(false), mode(-1) { }

		//Attributes
		string	user_open;	//User which open remote host
		string	id;		//External host id
		string	name;		//Name
		string	transp;		//Connect transport
		string	addr;		//External host address
		string	user;		//External host user
		string	pass;		//External host password
		bool	link_ok;	//Link OK

		int8_t	mode;		//Mode; Only dynamic set
	};

	//Methods
	TTransportS( );
	~TTransportS( );

	int subVer( )			{ return STR_VER; }
	void inTrList( vector<string> &ls );
	void outTrList( vector<string> &ls );

	//> External hosts
	string extHostsDB( );
	void extHostList( const string &user, vector<string> &list, bool andSYS = false );
	bool extHostPresent( const string &user, const string &id );
	AutoHD<TTransportOut> extHost( TTransportS::ExtHost host, const string &pref = "" );
	ExtHost extHostGet( const string &user, const string &id, bool andSYS = false );
	void extHostSet( const ExtHost &host, bool andSYS = false );
	void extHostDel( const string &user, const string &id, bool andSYS = false );

	//> Request to remote or local OpenSCADA control interface
	int cntrIfCmd( XMLNode &node, const string &senderPref, const string &user = "" );

	void subStart( );
	void subStop( );

	TElem &inEl( )			{ return el_in; }
	TElem &outEl( ) 		{ return el_out; }

	AutoHD<TTipTransport> at( const string &iid )	{ return modAt(iid); }

    protected:
	void load_( );
	void save_( );

    private:
	//Methods
	string optDescr( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//Attributes
	TElem	el_in, el_out, el_ext;

	Res	extHostRes;             //External hosts resource
	vector<ExtHost> extHostLs;      //External hosts list
};

}

#endif // TTRANSPORTS_H

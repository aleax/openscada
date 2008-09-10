
//OpenSCADA system file: ttransports.h
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#define  VER_TR 1    //TransportS type modules version

#include <string>

#include "tbds.h"

using std::string;

//************************************************
//* TTransportIn				 *
//************************************************
class TTipTransport;

class TTransportIn : public TCntrNode, public TConfig
{
    public:
	//Methods
	TTransportIn( const string &id, const string &db, TElem *el );
	virtual ~TTransportIn( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return m_id; }
	string name( );
	string dscr( )		{ return m_dscr; }
	string addr( )		{ return m_addr; }
	string protocol( )	{ return m_prot; }

	bool toStart( ) 	{ return m_start; }
	bool startStat( )	{ return run_st; }

	string DB( )		{ return m_db; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ m_name = inm; modif(); }
	void setDscr( const string &idscr )	{ m_dscr = idscr; modif(); }
	void setAddr( const string &addr )	{ m_addr = addr; modif(); }
	void setProtocol( const string &prt )	{ m_prot = prt; modif(); }
	void setToStart( bool val )             { m_start = val; modif(); }

	void setDB( const string &vl )          { m_db = vl; modifG(); }

	virtual void start( )	{ };
	virtual void stop( )	{ };

	TTipTransport &owner( )	{ return *(TTipTransport*)nodePrev(); }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1

	void load_( );
	void save_( );

	//Attributes
	bool	run_st;

    private:
	//Methods
	string nodeName( )	{ return m_id; }

	//Attributes
	string	&m_id;
	string	&m_name;
	string	&m_dscr;
	string	&m_addr;
	string	&m_prot;
	bool	&m_start;
	string	m_db;
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

	const string &id( )	{ return m_id; }
	string	name( );
	string	dscr( )		{ return m_dscr; }
	string	addr( )		{ return m_addr; }
	int	prm1( )		{ return m_prm1; }
	int	prm2( )		{ return m_prm2; }
	bool	toStart()	{ return m_start; }
	bool	startStat()	{ return run_st; }

	string DB( )		{ return m_db; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ m_name = inm; modif(); }
	void setDscr( const string &idscr )	{ m_dscr = idscr; modif(); }
	void setAddr( const string &addr )	{ m_addr = addr; modif(); }
	void setPrm1( int vl )			{ m_prm1 = vl; }
	void setPrm2( int vl )			{ m_prm2 = vl; }
	void setToStart( bool val )		{ m_start = val; modif(); }

	void setDB( const string &vl )		{ m_db = vl; modifG(); }

	virtual void start( )			{ };
	virtual void stop( )			{ };

	virtual int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return 0; }

	string messProtIO( const string &in, const string &prot );

	TTipTransport &owner( )	{ return *(TTipTransport*)nodePrev(); }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1

	void load_( );
	void save_( );

	//Attributes
	bool    run_st;

    private:
	//Methods
	string nodeName( )	{ return m_id; }

	//Attributes
	string	&m_id;
	string	&m_name;
	string	&m_dscr;
	string	&m_addr;
	bool	&m_start;
	string	m_db;

	//- Reserve parameters -
	int	m_prm1, m_prm2;
};

//************************************************
//* TTipTransport				 *
//************************************************
class TTransportS;

class TTipTransport: public TModule
{
    public:
	//Methods
	TTipTransport( );
	virtual ~TTipTransport( );

	//- Input transports -
	void inList( vector<string> &list )			{ chldList(m_in,list); }
	bool inPresent( const string &name )			{ return chldPresent(m_in,name); }
	void inAdd( const string &name, const string &db = "*.*" );
	void inDel( const string &name, bool complete = false )	{ chldDel(m_in,name,-1,complete); }
	AutoHD<TTransportIn> inAt( const string &name )		{ return chldAt(m_in,name); }

	//- Output transports -
	void outList( vector<string> &list )			{ chldList(m_out,list); }
	bool outPresent( const string &name )			{ return chldPresent(m_out,name); }
	void outAdd( const string &name, const string &idb = "*.*" );
	void outDel( const string &name, bool complete = false ){ chldDel(m_out,name,-1,complete); }
	AutoHD<TTransportOut> outAt( const string &name )	{ return chldAt(m_out,name); }

	TTransportS &owner( )					{ return (TTransportS&)TModule::owner(); }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	virtual TTransportIn  *In( const string &name, const string &db )
	{ throw TError(nodePath().c_str(),_("Input transport no support!")); }
	virtual TTransportOut *Out( const string &name, const string &db )
	{ throw TError(nodePath().c_str(),_("Output transport no support!")); }

    private:
	//Attributes
	int	m_in, m_out;
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
		//Methods
		ExtHost( const string &iuser_open, const string &iid, const string &iname,
			    const string &itransp, const string &iaddr, const string &iuser, const string &ipass ) :
		    user_open(iuser_open), id(iid), name(iname), transp(itransp), addr(iaddr),
		    user(iuser), pass(ipass), link_ok(false) { }

		//Attributes
		string	user_open;	//User which open remote host
		string	id;		//External host id
		string	name;		//Name
		string	transp;		//Connect transport
		string	addr;		//External host address
		string	user;		//External host user
		string	pass;		//External host password
		bool	link_ok;	//Link OK
	};

	//Methods
	TTransportS( );
	~TTransportS( );

	int subVer( )			{ return VER_TR; }

	//- External hosts -
	bool sysHost( )			{ return sys_host; }
	void setSysHost( bool vl )	{ sys_host = vl; }
	string extHostsDB( );
	void extHostList( const string &user, vector<string> &list );
	bool extHostPresent( const string &user, const string &id );
	AutoHD<TTransportOut> extHost( TTransportS::ExtHost host, const string &pref = "" );
	ExtHost extHostGet( const string &user, const string &id );
	void extHostSet( const ExtHost &host );
	void extHostDel( const string &user, const string &id );

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
	bool	sys_host;
	TElem	el_in, el_out, el_ext;

	Res	extHostRes;             //External hosts resource
	vector<ExtHost> extHostLs;      //External hosts list
};

#endif // TTRANSPORTS_H

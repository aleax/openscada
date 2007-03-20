
//OpenSCADA system file: ttransports.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
//================================================================
//=========== TTransportIn =======================================
//================================================================
class TTipTransport;

class TTransportIn : public TCntrNode, public TConfig
{
    public:
	TTransportIn( const string &id, const string &idb, TElem *el );
	virtual ~TTransportIn();

	const string &id()	{ return m_id; }
	string name();
        string dscr()		{ return m_dscr; }
	string addr()		{ return m_addr; }
	string protocol()	{ return m_prot; }
	
	bool toStart() 	{ return m_start; }
	bool startStat(){ return run_st; }
	
	string DB( )            { return m_db; }
        string tbl( );
        string fullDB( )        { return DB()+'.'+tbl(); }
	
	void name( const string &inm )  { m_name = inm; }
        void dscr( const string &idscr ){ m_dscr = idscr; }
        void addr( const string &addr ) { m_addr = addr; }			
	
	virtual void start()	{ };
	virtual void stop()	{ };
	
	void load( );
	void save( );
	
	TTipTransport &owner()	{ return *(TTipTransport*)nodePrev(); }
	
    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	void preEnable(int flag);
	void postDisable(int flag);     //Delete all DB if flag 1
	
	//Attributes
	bool    run_st;
	
    private:
	//Methods
        string nodeName()       { return m_id; }
	
	//Attributes
	string  &m_id;
	string  &m_name;
	string  &m_dscr;
	string  &m_addr;
	string  &m_prot;
	bool    &m_start;
	string	m_db;
};

//================================================================
//=========== TTransportOut ======================================
//================================================================
class TTransportOut : public TCntrNode, public TConfig
{
    public:
	TTransportOut( const string &id, const string &idb, TElem *el );
	virtual ~TTransportOut();	
	
	const string &id()      { return m_id; }
	string name();
	string dscr()		{ return m_dscr; }
	string addr() 		{ return m_addr; }
	
        string DB( )            { return m_db; }
        string tbl( );
        string fullDB( )        { return DB()+'.'+tbl(); }				
	
	bool toStart()  { return m_start; }
        bool startStat(){ return run_st; }
	
	void setName( const string &inm )	{ m_name = inm; }
	void setDscr( const string &idscr )	{ m_dscr = idscr; }
	void setAddr( const string &addr )	{ m_addr = addr; }
	
	void toStart( bool val )        { m_start = val; }
	
	virtual void start( )		{ };
	virtual void stop( )		{ };	
	void load( );
	void save( );
	
	virtual int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return 0; }
	
	TTipTransport &owner() 	{ return *(TTipTransport*)nodePrev(); }
	
    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void preEnable(int flag);	
	void postDisable(int flag);     //Delete all DB if flag 1

	//Attributes
	bool    run_st;

    private:
	//Methods
        string nodeName()       { return m_id; }
	
	//Attributes	
	string  &m_id;
	string  &m_name;
	string  &m_dscr;
	string  &m_addr;
	bool    &m_start;
	string	m_db;
};

//================================================================
//=========== TTipTransport ======================================
//================================================================
class TTransportS;

class TTipTransport: public TModule
{
    public:
    	TTipTransport( );
	virtual ~TTipTransport();

	//- Input transports -
	void inList( vector<string> &list )	{ chldList(m_in,list); }
        bool inPresent( const string &name )	{ return chldPresent(m_in,name); }
	void inAdd( const string &name, const string &idb = "*.*" );
	void inDel( const string &name, bool complete = false )	{ chldDel(m_in,name,-1,complete); }
	AutoHD<TTransportIn> inAt( const string &name )	
	{ return chldAt(m_in,name); }

	//- Output transports -
	void outList( vector<string> &list ) 	{ chldList(m_out,list); }
        bool outPresent( const string &name )	{ return chldPresent(m_out,name); }
	void outAdd( const string &name, const string &idb = "*.*" );
	void outDel( const string &name, bool complete = false ){ chldDel(m_out,name,-1,complete); }
	AutoHD<TTransportOut> outAt( const string &name )
	{ return chldAt(m_out,name); }
	
	TTransportS &owner()	{ return (TTransportS&)TModule::owner(); }
	
    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	virtual TTransportIn  *In( const string &name, const string &idb )
	{ throw TError(nodePath().c_str(),_("Input transport no support!")); }
	virtual TTransportOut *Out( const string &name, const string &idb )
	{ throw TError(nodePath().c_str(),_("Output transport no support!")); }
	
    private:	
	//Attributes
	int	m_in, m_out;
};

//================================================================
//=========== TTransportS ========================================
//================================================================

class TTransportS : public TSubSYS
{
    public:
     	TTransportS( );
	~TTransportS( );

	int subVer( ) 	{ return(VER_TR); }
	
	void subLoad( );
	void subSave( );
	void subStart( );
	void subStop( );
	
	TElem &inEl()	{ return(el_in); }
	TElem &outEl() 	{ return(el_out); }
	
        AutoHD<TTipTransport> at( const string &iid )	{ return modAt(iid); }
	
	string optDescr( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes	
	TElem  		el_in, el_out;
};

#endif // TTRANSPORTS_H

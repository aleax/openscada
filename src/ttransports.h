/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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
	TTransportIn( const string &name, TTipTransport *owner );
	virtual ~TTransportIn();

	string &name() { return(m_name); }
        string &lName()  { return(m_lname); }			
	
	bool toStart() { return(m_start); }
	bool startStat(){ return(run_st); }
	
	virtual void start(){};
	virtual void stop(){};	
	
	void load( );
	void save( );
	
	TTipTransport &owner() { return(*m_owner); }
	
    protected:
	string nodeName(){ return m_name; }
	void preEnable();
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
	void postDisable(int flag);     //Delete all DB if flag 1
	
    protected:
	string  &m_name;
	string  &m_lname;
	string  &m_addr;
	string  &m_prot;
	bool    &m_start;

	bool    run_st;
	
    private:
	TTipTransport *m_owner;
	
	static const char *o_name;
};

//================================================================
//=========== TTransportOut ======================================
//================================================================
class TTransportOut : public TCntrNode, public TConfig
{
    public:
	TTransportOut( const string &name, TTipTransport *owner );
	virtual ~TTransportOut();	
	
	string &name() { return(m_name); }
	string &lName() { return(m_lname); }
	string &address() { return(m_addr); }
	
	void address( const string &addr ) { m_addr = addr; }
	
	bool toStart() { return(m_start); }
	bool startStat(){ return(run_st); }
	
	virtual void start(){};
	virtual void stop(){};
	
	void load( );
	void save( );
	
	virtual int messIO(char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return(0); }
	
	TTipTransport &owner() { return(*m_owner); }
	
    protected:
	string nodeName(){ return m_name; }
	void preEnable();
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
	void postDisable(int flag);     //Delete all DB if flag 1
	
    protected:
	string  &m_name;
	string  &m_lname;
	string  &m_addr;
	bool    &m_start;
	
	bool    run_st;
	
    private:
	TTipTransport *m_owner;
	
	static const char *o_name;
};

//================================================================
//=========== TTipTransport ======================================
//================================================================
class TTipTransport: public TModule
{
/** Public methods: */
    public:
    	TTipTransport( );
	virtual ~TTipTransport();

	// Input transports
	void inList( vector<string> &list )	{ chldList(m_in,list); }
        bool inAvoid( const string &name )	{ return chldAvoid(m_in,name); }
	void inAdd( const string &name );
	void inDel( const string &name )	{ chldDel(m_in,name); }
	AutoHD<TTransportIn> inAt( const string &name )	
	{ return chldAt(m_in,name); }

	// Output transports
	void outList( vector<string> &list ) 	{ chldList(m_out,list); }
        bool outAvoid( const string &name )	{ return chldAvoid(m_out,name); }
	void outAdd( const string &name );
	void outDel( const string &name )	{ chldDel(m_out,name); }
	AutoHD<TTransportOut> outAt( const string &name )
	{ return chldAt(m_out,name); }
	
    protected:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
/** Public atributes:: */
    private:
	virtual TTransportIn  *In( const string &name )
	{ throw TError("(%s) Input transport no support!",o_name); }
	virtual TTransportOut *Out( const string &name )
	{ throw TError("(%s) Output transport no support!",o_name); }
	
/** Private atributes:: */
    private:
	int	m_in, m_out;
	
	static const char *o_name;
};

//================================================================
//=========== TTransportS ========================================
//================================================================

class TTransportS : public TGRPModule
{
    /** Public methods: */
    public:
     	TTransportS( TKernel *app );
	~TTransportS();

	int gmdVer( ) { return(VER_TR); }
	// Init All transport's modules
	void gmdLoad( );
	// Start all transports
	void gmdStart( );
	// Stop all transports
	void gmdStop( );
	
	// Load/Reload all BD and update internal controllers structure!
	void loadBD( );
	// Update all BD from current to external BD.
	void saveBD( );
	
	TBDS::SName inBD();
	TBDS::SName outBD();
	
	TElem &inEl(){ return(el_in); }
	TElem &outEl() { return(el_out); }
	
	string optDescr( );

    /** Private methods: */
    private:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
    /** Private atributes: */
    private:
	TBDS::SName	m_bd_in, m_bd_out;
	TElem  		el_in, el_out;
    
	static const char *o_name;
};

#endif // TTRANSPORTS_H

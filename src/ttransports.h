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
#include "tconfig.h"
#include "tmodule.h"
#include "tgrpmodule.h"

using std::string;
//================================================================
//=========== TTransportIn =======================================
//================================================================
class TTipTransport;

class TTransportIn : public TContr, public TConfig
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
	
	void load();
	void save();
	
	TTipTransport &owner() { return(*m_owner); }
	
    protected:
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	
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
class TTransportOut : public TContr, public TConfig
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
	
	void load();
	void save();
	
	virtual int messIO(char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return(0); }
	
	TTipTransport &owner() { return(*m_owner); }
	
    protected:
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	
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

	// Avoid input transports list
	void inList( vector<string> &list )
	{ m_hd_in.objList( list ); }
	// Avoid stat
        bool inAvoid( const string &name )
	{ return m_hd_in.objAvoid(name); }
    	// Add input transport
	void inAdd( const string &name );
	// Del input transport
	void inDel( const string &name )
	{ delete (TTransportIn *)m_hd_in.objDel( name ); }
	// Input transport
	AutoHD<TTransportIn> inAt( const string &name )
	{ AutoHD<TTransportIn> obj( name, m_hd_in ); return obj; }

	// Avoid output transports list
	void outList( vector<string> &list )
	{ m_hd_out.objList( list ); }
	// Avoid stat
        bool outAvoid( const string &name )
	{ return m_hd_out.objAvoid(name); }
    	// Add output transport
	void outAdd( const string &name );
	// Del output transport
	void outDel( const string &name )
	{ delete (TTransportOut *)m_hd_out.objDel( name ); }
	// Output transport
	AutoHD<TTransportOut> outAt( const string &name )
	{ AutoHD<TTransportOut> obj( name, m_hd_out ); return obj; }
	
    protected:
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TContr> ctrAt1( const string &br );
	
/** Public atributes:: */
    private:
	virtual TTransportIn  *In( const string &name )
	{ throw TError("(%s) Input transport no support!",o_name); }
	virtual TTransportOut *Out( const string &name )
	{ throw TError("(%s) Output transport no support!",o_name); }
	
/** Private atributes:: */
    private:
	THD    m_hd_in;
	THD    m_hd_out;
	
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
	void gmdInit( );
	// Start all transports
	void gmdStart( );
	// Stop all transports
	void gmdStop( );
	
	// Load/Reload all BD and update internal controllers structure!
	void loadBD( );
	// Update all BD from current to external BD.
	void saveBD( );
	
	void gmdCheckCommandLine( );
	void gmdUpdateOpt();

	TBDS::SName &inBD() { return(m_bd_in); }
	TBDS::SName &outBD(){ return(m_bd_out); }
	
	TElem &inEl(){ return(el_in); }
	TElem &outEl() { return(el_out); }
	
	string optDescr( );

    /** Private methods: */
    private:
	void gmdDel( const string &name );
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	
    /** Private atributes: */
    private:
	TBDS::SName	m_bd_in, m_bd_out;
	TElem  		el_in, el_out;
    
	static const char *o_name;
};

#endif // TTRANSPORTS_H

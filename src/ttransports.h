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

	virtual void start(){};
	virtual void stop(){};

	string &Name() { return(m_name); }
	
	TTipTransport &Owner() { return(*m_owner); }
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
    protected:
	string  &m_name;
	string  &m_lname;
	string  &m_addr;
	string  &m_prot;

	bool    run_st;
    private:
	TTipTransport *m_owner;
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TTransportOut ======================================
//================================================================
class TTransportOut : public TContr, public TConfig
{
    public:
	TTransportOut( const string &name, TTipTransport *owner );
	virtual ~TTransportOut();
	
	virtual void start(){};
	virtual void stop(){};
	
	virtual int IOMess(char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return(0); }
	
	string &Name() { return(m_name); }
	string &lName() { return(m_lname); }
	string &addres() { return(m_addr); }
	
	TTipTransport &Owner() { return(*m_owner); }
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
    protected:
	string  &m_name;
	string  &m_lname;
	string  &m_addr;
	
	bool    run_st;
    private:
	TTipTransport *m_owner;
	
	static const char *o_name;
	static const char *i_cntr;
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
	void in_list( vector<string> &list )
	{ m_hd_in.obj_list( list ); }
    	// Add input transport
	void in_add( const string &name );
	// Del input transport
	void in_del( const string &name )
	{ delete (TTransportIn *)m_hd_in.obj_del( name ); }
	// Input transport
	AutoHD<TTransportIn> in_at( const string &name )
	{ AutoHD<TTransportIn> obj( name, m_hd_in ); return obj; }

	// Avoid output transports list
	void out_list( vector<string> &list )
	{ m_hd_out.obj_list( list ); }
    	// Add output transport
	void out_add( const string &name );
	// Del output transport
	void out_del( const string &name )
	{ delete (TTransportOut *)m_hd_out.obj_del( name ); }
	// Output transport
	AutoHD<TTransportOut> out_at( const string &name )
	{ AutoHD<TTransportOut> obj( name, m_hd_out ); return obj; }
/** Public atributes:: */
    public:
	
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	AutoHD<TContr> ctr_at1( const string &br );
/** Public atributes:: */
    private:
	virtual TTransportIn  *In( const string &name )
	{ throw TError("(%s) Input transport no support!",o_name); }
	virtual TTransportOut *Out( const string &name )
	{ throw TError("(%s) Output transport no support!",o_name); }
/** Private atributes:: */
    private:
        //unsigned hd_res;
	
	THD    m_hd_in;
	THD    m_hd_out;
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TTransportS ========================================
//================================================================

class TTransportS : public TGRPModule, public TElem
{
    /** Public methods: */
    public:
     	TTransportS( TKernel *app );
	~TTransportS();

	int gmd_Ver( ) { return(VER_TR); }
	// Init All transport's modules
	void gmd_Init( );
	// Start all transports
	void gmd_Start( );
	// Stop all transports
	void gmd_Stop( );
	
	// Load/Reload all BD and update internal controllers structure!
	void LoadBD( );
	// Update all BD from current to external BD.
	void UpdateBD( );
	
	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    /** Private methods: */
    private:
    	string opt_descr( );
	
	void gmd_del( const string &name );
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:
	static SFld        gen_elem[]; //Generic BD elements

	SBDS   m_bd;
    
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TTRANSPORTS_H

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

#ifndef TARHIVES_H
#define TARHIVES_H

#define  VER_ARH 1    //ArhiveS type modules version

#include <string>
#include <vector>

#include "terror.h"
#include "thd.h"
#include "tbds.h"
#include "tmodule.h"
#include "tconfig.h"
#include "tmessage.h"
#include "tgrpmodule.h"

using std::string;
using std::vector;

//================================================================
//=========== TArhiveMess ========================================
//================================================================
class TTipArhive;

class TArhiveMess : public TContr, public TConfig
{
    public:
	TArhiveMess(string name, TTipArhive *owner );
	virtual ~TArhiveMess();
	

	virtual void put( vector<SBufRec> &mess ){ };
        virtual void get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, string category = "", char level = 0 ) { };
        virtual void start(){ };
	virtual void stop(){ };
	
	string &Name()   { return(m_name); }
	string &Descr()  { return(m_lname); }
	bool   toStart() { return(m_start); }
	string &Addr()   { return(m_addr); }
	int    &Level()  { return(m_level); }
	void Categ( vector<string> &list );
	
	//Load and save parameter
	void Load( );
	void Save( );
	
	TTipArhive &Owner() { return(*m_owner); }
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    protected:
	string         &m_name;
	string         &m_lname;
	bool           &m_start;
	string         &m_addr;
	string         &m_cat_o;
	int            &m_level;

	bool           run_st;
    private:
	TTipArhive     *m_owner;

	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TArhiveVal =========================================
//================================================================
class TArhiveVal : public TContr, public TConfig
{
    public:
	TArhiveVal(string name, TTipArhive *owner );
	virtual ~TArhiveVal();

	string &Name() { return(m_name); }
	
	TTipArhive &Owner() { return(*m_owner); }
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
    protected:
	string  &m_name;
	string  &m_bd;
    private:
	TTipArhive *m_owner;
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TTipArhive =========================================
//================================================================

class TTipArhive: public TModule
{
    /** Public methods: */
    public:
    	TTipArhive( );
	virtual ~TTipArhive();
		
	// Avoid message list
	void mess_list( vector<string> &list ) 
	{ m_hd_mess.obj_list( list ); }
	// Add message arhive
	void mess_add( string name );
	// Del message arhive
	void mess_del( string name ) 
	{ delete (TArhiveMess *)m_hd_mess.obj_del( name ); }
	// Attach to message arhive
	unsigned mess_att( string name )
	{ return( m_hd_mess.hd_att( name ) ); }	
	// Detach from message arhive
	void mess_det( unsigned hd )
	{ m_hd_mess.hd_det( hd ); }
	// Get attached object
        TArhiveMess &mess_at( unsigned hd )
	{ return( *(TArhiveMess *)m_hd_mess.hd_at( hd ) ); }
	
	// Avoid message list
	void val_list( vector<string> &list )
	{ m_hd_val.obj_list( list ); }
	// Add message arhive
	void val_add( string name );
	// Del message arhive
        void val_del( string name )
	{ delete (TArhiveVal *)m_hd_val.obj_del( name ); }
	// Attach to message arhive
        unsigned val_att( string name )
	{ return( m_hd_val.hd_att( name ) ); }
	// Detach from message arhive/
        void val_det( unsigned hd )
	{ m_hd_val.hd_det( hd ); }
	// Get attached object
        TArhiveVal &val_at( unsigned hd )
	{ return( *(TArhiveVal *)m_hd_val.hd_at( hd ) ); }

    /** Public atributes:: */
    public:

    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	unsigned ctr_att( string br );
	void     ctr_det( string br, unsigned hd );
	TContr  &ctr_at( string br, unsigned hd );
    /** Private atributes:: */
    private:
	virtual TArhiveMess *AMess(string name )
	{ throw TError("(%s) Message arhiv no support!",o_name); }
	virtual TArhiveVal  *AVal(string name )
	{ throw TError("(%s) Value arhiv no support!",o_name); }
	
    /** Private atributes:: */
    private:
        THD    m_hd_mess; 
        THD    m_hd_val; 
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//================ TArhiveS ======================================
//================================================================

class SArhS
{
    public:
	SArhS( string m_tp, string m_obj ) : tp(m_tp), obj(m_obj) { }
    	string tp;
	string obj;
};

struct SHDArh
{
    unsigned h_tp;
    unsigned h_obj;
};

class TArhiveS : public TGRPModule
{
    /** Public methods: */
    public:
	TArhiveS( TKernel *app );

	~TArhiveS(  );

	int gmd_Ver( ) { return(VER_ARH); }
    	// Init All transport's modules
	void gmd_Init( );
	// Load/Reload all BD and update internal controllers structure!
	void LoadBD( );
	// Update all BD from current to external BD.
	void UpdateBD( );
	
    	TTipArhive &gmd_at( unsigned hd )     { return( (TTipArhive &)TGRPModule::gmd_at(hd) ); }
	TTipArhive &operator[]( unsigned hd ) { return( gmd_at(hd) ); }

	// Avoid message list
	void mess_list( vector<SArhS> &list );
	// Add message arhive
	void mess_add( SArhS arh );
	// Del message arhive
	void mess_del( SArhS arh );
	/*
	 * Attach to message arhive
	 * Return arhive header
	 */
	SHDArh mess_att( SArhS arh );
	// Detach from message arhive
	void mess_det( SHDArh &hd );
	// Get attached object
        TArhiveMess &mess_at( SHDArh &hd ) { return( gmd_at( hd.h_tp ).mess_at( hd.h_obj ) ); }
	
	// Avoid message list
	void val_list( vector<SArhS> &list );
	// Add message arhive
	void val_add( SArhS arh );
	// Del message arhive
	void val_del( SArhS arh );
	/*
	 * Attach to message arhive
	 * Return arhive header
	 */
	SHDArh val_att( SArhS arh );
	// Detach from message arhive
	void val_det( SHDArh &hd );
	// Get attached object
        TArhiveVal &val_at( SHDArh &hd ) { return( gmd_at( hd.h_tp ).val_at( hd.h_obj ) ); }

	void gmd_Start( );
	void gmd_Stop( );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();

	SBDS &messB() { return(m_bd_mess); }
	SBDS &valB()  { return(m_bd_val); }
	
	TElem &messE(){ return(el_mess); }
	TElem &valE() { return(el_val); }
    /** Privates: */
    private:
	string opt_descr(  );

	static void *MessArhTask(void *param);
	
	void gmd_del( string name );    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:	
	SBDS   m_bd_mess;
	SBDS   m_bd_val;
	
	TElem  el_mess;
	TElem  el_val;

	int       m_mess_per;       //Mmessage arhiving period
	pthread_t m_mess_pthr;
	bool      m_mess_r_stat;
	bool      m_mess_r_endrun;
	
	
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TARHIVES_H

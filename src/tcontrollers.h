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

#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#define  VER_CNTR 1    //ControllerS type modules version

#include <string>

#include "tbds.h"
#include "tconfig.h"
#include "tgrpmodule.h"
#include "ttipcontroller.h"

using std::string;

class SCntrS
{
    public:
	SCntrS( string m_tp, string m_obj ) : tp(m_tp), obj(m_obj) { }
	string tp;
	string obj;
};

struct SHDCntr
{
    unsigned h_tp;
    unsigned h_obj;
};

class TController;
class TTipController;

class TControllerS : public TGRPModule, public TElem
{
    /** Public methods: */
    public:
	TControllerS( TKernel *app );
	~TControllerS(  );

	int gmd_Ver( ) { return(VER_CNTR); }
	// Init All controller's modules    
	void gmd_Init( );
	// Init and start all configured controllers. 
	virtual void gmd_Start(  );                                         
	// Stop and deinit all configured controllers. 
	virtual void gmd_Stop(  );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    
	TTipController &gmd_at( unsigned hd ){ return( (TTipController &)TGRPModule::gmd_at(hd) ); }
	TTipController &operator[]( unsigned hd ){ return( gmd_at(hd) ); }

	// Avoid controllers list
	void list( vector<SCntrS> &list );
	// Add controller
	void add( SCntrS cntr, SBDS bd );
	// Del controller
	void del( SCntrS cntr );
	/*
	* Attach to controller
	* Return controller header
	*/
	SHDCntr att( SCntrS cntr, string how = "th_contrs" );
	// Detach from controller
	void det( SHDCntr &hd );
	// Get attached controller
	TController &at( SHDCntr &hd ) { return( gmd_at( hd.h_tp ).at( hd.h_obj ) ); }
	
	// Load/Reload all BD and update internal controllers structure!
	void LoadBD( );
	// Update all BD from current to external BD.
	void UpdateBD( );    
    
	SBDS &BD() { return(m_bd); }
    /** Public atributes: */
    public:
	
    /** Private methods: */
    private:
	string opt_descr( );

	void gmd_del( string name );
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:
	SBDS     m_bd;
    
	static SFld       gen_elem[];  //Generic BD elements
    
	static const char *o_name;
	static const char *i_cntr;
};

#endif // TCONTROLLERS_H

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

#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
#include <vector>

#include "thd.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tconfig.h"
#include "tbds.h"
#include "tcontroller.h"

using std::string;
using std::vector;

class TTipParam;

class TTipController : public TModule, public TElem
{
    /** Public methods: */
    public:
	TTipController( );
	virtual ~TTipController();
    
	// Avoid controllers list
	void list( vector<string> &list )
	{ m_hd_cntr.obj_list( list ); }
	// Add controller
	void add( string name, SBDS bd );
	// Del controller
	void del( string name )
	{ delete (TController *)m_hd_cntr.obj_del( name ); }
	/*
	 * Attach to controller
	 * Return controller header
	 */
	unsigned att( string name, string how = "" )
	{ return( m_hd_cntr.hd_att( name, how ) ); }
	// Detach from controller
	void det( unsigned hd )
	{ m_hd_cntr.hd_det( hd ); }
	// Get attached controller
	TController &at( unsigned hd )
	{ return( *(TController *)m_hd_cntr.hd_at( hd ) ); }
	TController &operator[]( unsigned hd ){ return( at(hd) ); }	
	
	void LoadCfg( SFld *elements, int numb );
	
	unsigned NameTpPrmToId(string name_t);
	unsigned SizeTpPrm( ) { return( paramt.size()); }
	TTipParam &at_TpPrm( unsigned id )
	{ if(id >= paramt.size()) throw TError("%s: id of param type error!",o_name); return( *paramt[id]); }
	int AddTpParm(string name_t, string n_fld_bd, string descr);
	void LoadTpParmCfg(unsigned id, SFld *elements, int numb );

	void ListTpVal( vector<string> & List );
	void AddTpVal(string name, SFld *vl_el, int number);
	TElem &at_TpVal( string name);
    /** Public atributes: */
    public:
    /** Protected methods: */
    protected: 
	virtual TController *ContrAttach(string name, SBDS bd)
	{ throw TError("%s: Error controller %s attach!",o_name,name.c_str()); }
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	unsigned ctr_att( string br );
	void     ctr_det( string br, unsigned hd );
	TContr  &ctr_at( string br, unsigned hd );
    /** Private methods: */
    private:
    
    /** Private atributes: */
    private:    
	vector<TTipParam *>   paramt;  // List type parameter and Structure configs of parameter.
	vector<TElem *>       val_el;  // Value types for value of parameter            
	THD m_hd_cntr;  // List controller       

	static const char *o_name;
	static const char *i_cntr;
};

#endif // TTIPCONTROLLER_H


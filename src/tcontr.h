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
 
#ifndef TCNTR_H
#define TCNTR_H

#include <string>
#include <vector>

#include "terror.h"
#include "tconfig.h"
#include "tvalue.h"

using std::string;
using std::vector;

class XMLNode;

class TContr
{
    public:
	TContr(  );
	~TContr();

	// Get control info
        void ctr_info( XMLNode &node );
	
	// Change of control dinamics from <opt>
	void ctr_din_set( string area_path, XMLNode *opt );
	void ctr_din_get( string area_path, XMLNode *opt );
	// Path parse
	string ctr_path_l(string path, int level);
	//========== Options manipulation ===================================
	XMLNode *ctr_opt( XMLNode *inf, unsigned numb );  //scan options node
	XMLNode *ctr_id( XMLNode *inf, string name_id );  //get node for he individual number
	// Check fld valid
	string chk_opt_val( XMLNode *fld, bool fix = false );
	// Get option's values
	string ctr_opt_getS( XMLNode *fld );      //string
	int    ctr_opt_getI( XMLNode *fld );      //integer
	double ctr_opt_getR( XMLNode *fld );      //real
	bool   ctr_opt_getB( XMLNode *fld );      //boolean
	// Set option's values	
	void ctr_opt_setS( XMLNode *fld, string val, int id=0 );    //string
	void ctr_opt_setI( XMLNode *fld, int val, int id=0 );       //integer
	void ctr_opt_setR( XMLNode *fld, double val, int id=0 );    //real
	void ctr_opt_setB( XMLNode *fld, bool val, int id=0 );      //boolean
	
	//========== Commands manipulation ===================================	
	void ctr_cmd_go( string area_path, XMLNode *fld, XMLNode *rez );        // Command go 
	
	//========== Branchs manipulation ===================================
	//---------- att mode ------------------
	virtual unsigned ctr_att( string br )
	{ throw TError("(%s) Function <ctr_att> no support!",o_name); }
	virtual void ctr_det( string br, unsigned hd )
	{ throw TError("(%s) Function <ctr_det> no support!",o_name); }
	virtual TContr &ctr_at( string br, unsigned hd )
	{ throw TError("(%s) Function <ctr_at> no support!",o_name); }
	//---------- at mode ------------------
	virtual TContr &ctr_at( string br )	
	{ throw TError("(%s) Function <ctr_at> no support!",o_name); }
    protected:
	virtual void ctr_fill_info( XMLNode *inf ){ };
	virtual void ctr_din_set_( string area_path, XMLNode *opt ){ };
	virtual void ctr_din_get_( string area_path, XMLNode *opt ){ };
	virtual void ctr_cmd_go_( string area_path, XMLNode *fld, XMLNode *rez ){ };        // Command go 
	//TConfig functions
	void ctr_cfg_parse( string p_elem, XMLNode *fld, TConfig *cfg, int id_cf = 0 );
	void ctr_cfg_set( string elem, XMLNode *fld, TConfig *cfg, int id_cf = 0 );
	void ctr_cfg_get( string elem, XMLNode *fld, TConfig *cfg, int id_cf = 0 );
	//TValue functions
	void ctr_val_parse( string p_elem, XMLNode *fld, TValue *val );
	void ctr_val_set( string elem, XMLNode *fld, TValue *val );
	void ctr_val_get( string elem, XMLNode *fld, TValue *val );
    private:
	void ctr_fld_parse( string p_elem, TFld &fld, XMLNode *w_fld );
	
    private:
        static const char *o_name;	
};

#endif //TCNTR_H


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
#include "thd.h"
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

        void ctrStat( XMLNode &node );				// Get control static code	
	void ctrDinGet( const string &path, XMLNode *opt );	//Get field value
	void ctrDinSet( const string &path, XMLNode *opt );	//Set field value
	
	XMLNode *ctrId( XMLNode *inf, const string &n_id );	//get node for he individual number
        //========== Branchs manipulation ===================================
	//---------- at mode ------------------
	virtual TContr &ctrAt( const string &br )
	{ throw TError("(%s) Function <ctrAt> no support!",o_name); }
	//---------- Auto at mode ------------------
	virtual AutoHD<TContr> ctrAt1( const string &br )
	{ throw TError("(%s) Function <ctrAt1> no support!",o_name); }
	
	string ctrChk( XMLNode *fld, bool fix = false );	// Check fld valid
	
	// Get option's values
	string ctrGetS( XMLNode *fld );	//string
	int    ctrGetI( XMLNode *fld );	//integer
	double ctrGetR( XMLNode *fld );	//real
	bool   ctrGetB( XMLNode *fld );	//boolean
	
	// Set option's values	
	void ctrSetS( XMLNode *fld, const string &val, int id=0 );	//string
	void ctrSetI( XMLNode *fld, int val, int id=0 );       		//integer
	void ctrSetR( XMLNode *fld, double val, int id=0 );		//real
	void ctrSetB( XMLNode *fld, bool val, int id=0 );		//boolean

	// Path parse
        string pathLev( const string &path, int level, bool encode = true );
        string pathCode( const string &in, bool el );
        string pathEncode( const string &in, bool el );				
	
    protected:
	virtual void ctrStat_( XMLNode *inf ){ };
	virtual void ctrDinSet_( const string &area_path, XMLNode *opt ){ };
	virtual void ctrDinGet_( const string &area_path, XMLNode *opt ){ };
	
	//TConfig functions
	void ctr_cfg_parse( const string &p_elem, XMLNode *fld, int pos, TConfig *cfg );
	void ctr_cfg_set( const string &elem, XMLNode *fld, TConfig *cfg );
	void ctr_cfg_get( const string &elem, XMLNode *fld, TConfig *cfg );
	
	//TValue functions
	void ctr_val_parse( const string &p_elem, XMLNode *fld, int pos, TValue *val );
	void ctr_val_set( const string &elem, XMLNode *fld, TValue *val );
	void ctr_val_get( const string &elem, XMLNode *fld, TValue *val );
	
    private:
	void ctr_fld_parse( const string &p_elem, TFld &fld, XMLNode *w_fld, int pos );
	
    private:
        static const char *o_name;	
};

#endif //TCNTR_H


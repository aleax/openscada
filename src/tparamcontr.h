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

#ifndef TPARAMCONTR_H
#define TPARAMCONTR_H

#include <time.h>
#include <string>
using std::string;

#include "tcontr.h"
#include "tconfig.h"
#include "tvalue.h"

//==============================================================================================
//Use for register into TControllers and make direct access to controller function for parameter
//and direct acces to parameter from controller
//==============================================================================================
class TController;
class TTipParam;

class TParamContr : public TContr, public TConfig, public TValue
{
    /** Public methods: */
    public:
	TParamContr( const string &name, TTipParam *tpprm, TController *contr); 
	virtual ~TParamContr();
	
    	string &Name()       { return(m_name); }
    	string &LName()      { return(m_lname); }
	
    	bool   &auto_export(){ return(m_aexport); }
    	bool   st_export()   { return(m_export); }
	
	void Load( );
	void Save( );
	
    	void Export( ); 
	void UnExport( );
	
	// Compare object
    	bool operator==( TParamContr & PrmCntr )
	{ if( Name() == PrmCntr.Name() ) return(true); return(false); };
	// Equaly config 
    	TParamContr & operator=( TParamContr & PrmCntr );

	//TValue &val();
	// Check for new value type
    	//void UpdateVAL();            //????
	// Enable parameter and open access to value
    	virtual void Enable();
	// Disable parameter and close access to value
    	virtual void Disable();
	// Type of parameter
    	TTipParam   &Type() { return(*tipparm); }
	TController &Owner() { return(*owner); }
    /**Attributes: */
    public:

    protected:    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
	
    private:
	//virtual TConfig *vl_GetCfg( ) { return(this); }
    /**Attributes: */
    private:
        string &m_name;
        string &m_lname;
	bool   &m_aexport;
	bool   m_export;
	
	short       own;   // id from password
	short       grp;   // id from group
	TController *owner;
	TTipParam   *tipparm;
    
	static const char *o_name;
        static const char *i_cntr;
};

#endif // TPARAMCONTR_H

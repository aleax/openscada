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

#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <time.h>
#include <string>

#include "tconfig.h"
#include "tcontr.h"
#include "tbds.h"

using std::string;

class TTipController;
class TParamContr;

class TController : public TContr, public TConfig
{
    /** Public methods: */
    public:
     	TController( const string &name_c, const SBDS &bd, TTipController *tcntr, TElem *cfgelem );
	virtual ~TController(  );
	
	string &name()       { return(m_name); }
	string &lName()      { return(m_lname); }
	
    	bool &toEnable()	{ return(m_aen); }
    	bool &toStart() 	{ return(m_astart); }
	bool enableStat()	{ return(en_st); }
	bool startStat()      	{ return(run_st); }

	void load( bool self = false );
	void save( bool self = false );
	void start(  );
	void stop(  );
	void enable(  );
	void disable(  );

	// Avoid parameters list
	void list( vector<string> &list )
	{ m_hd.obj_list( list ); }
	// Add parameter
	void add( const string &name, unsigned type, int pos = -1);
	// Del parameter
	void del( const string &name );
	// Rotate parameters (positions)
	void rotate( const string &name1, const string &name2)
	{ m_hd.obj_rotate( name1, name2 ); }
        // Parameter
        AutoHD<TParamContr> at( const string &name, const string &how = "th_contr" )
	{ AutoHD<TParamContr> obj( name, m_hd, how ); return obj; }
	
	SBDS &BD()         { return(m_bd); }
		
	TTipController &Owner() { return( *owner ); }
	
    protected:    
    	string  &m_name;
    	string  &m_lname;
    	bool    &m_aen;
    	bool    &m_astart;
	int     m_add_type;    //Add parameter type
	
	bool    en_st;    
	bool    run_st;
    protected:    
	virtual void load_(  ){ }
	virtual void save_(  ){ }
	virtual void start_(  ){ }
	virtual void stop_(  ){ }
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
	AutoHD<TContr> ctr_at1( const string &br );
    /** Private methods: */
    private:
	void LoadParmCfg(  );
	void SaveParmCfg(  );
	void FreeParmCfg(  );
	
	virtual TParamContr *ParamAttach( const string &name, int type );
	
    /**Attributes: */
    private:    
	SBDS    m_bd;

	THD     m_hd;     //hd 
    
	TTipController *owner;    

	static const char *o_name;
};


#endif // TCONTROLLER_H

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
	
	string &Name()       { return(m_name); }
	string &LName()      { return(m_lname); }

	SBDS &BD()         { return(m_bd); }
	
    	bool &auto_enable(){ return(m_aen); }
    	bool &auto_start() { return(m_astart); }
	bool st_enable()   { return(en_st); }
	bool st_run()      { return(run_st); }

	void Load( bool self = false );
	void Save( bool self = false );
	void Start(  );
	void Stop(  );
	void Enable(  );
	void Disable(  );

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
		
	/*
	 * Attach to parameter
         * Return parameter header
	 */
	unsigned att( const string &name, const string &how = "th_contr" )
	{ return( m_hd.hd_att( name, how ) ); }
	// Detach from parameter
	void det( unsigned hd )
    	{ m_hd.hd_det( hd ); }
    	// Get attached parameter
	TParamContr &at( unsigned hd );
    
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
	virtual void Load_(  ){ }
	virtual void Save_(  ){ }
	virtual void Start_(  ){ }
	virtual void Stop_(  ){ }
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
        static const char *i_cntr;
};


#endif // TCONTROLLER_H

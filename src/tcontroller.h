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
     	TController( const string &name_c, const TBDS::SName &bd, TTipController *tcntr, TElem *cfgelem );
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
	{ m_hd.objList( list ); }
	// Avoid stat
        bool avoid( const string &name )
        { return m_hd.objAvoid(name); }	
	// Add parameter
	void add( const string &name, unsigned type, int pos = -1);
	// Del parameter
	void del( const string &name );
        // Parameter
        AutoHD<TParamContr> at( const string &name, const string &how = "th_contr" )
	{ AutoHD<TParamContr> obj( name, m_hd, how ); return obj; }
	
	TBDS::SName &BD()         { return(m_bd); }
		
	TTipController &owner() { return( *m_owner ); }
	
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
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TContr> ctrAt1( const string &br );
	
    /** Private methods: */
    private:
	void LoadParmCfg(  );
	void SaveParmCfg(  );
	void FreeParmCfg(  );
	
	virtual TParamContr *ParamAttach( const string &name, int type );
	
    /**Attributes: */
    private:    
	TBDS::SName	m_bd;

	THD     m_hd;     //hd 
    
	TTipController *m_owner;    

	static const char *o_name;
};


#endif // TCONTROLLER_H

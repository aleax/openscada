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

#include "tbds.h"
#include "tparamcontr.h"

using std::string;

class TTipController;

class TController : public TCntrNode, public TConfig
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

	void load( );
	void save( );
	void start(  );
	void stop(  );
	void enable(  );
	void disable(  );

	// Parameters
	void list( vector<string> &list )	{ chldList(m_prm,list); }
        bool avoid( const string &name )	{ return chldAvoid(m_prm,name); }
	void add( const string &name, unsigned type, int pos = -1);
	void del( const string &name )		{ chldDel(m_prm,name); }
        AutoHD<TParamContr> at( const string &name, const string &how = "th_contr" )
	{ return chldAt(m_prm,name); }
	
	TBDS::SName BD();
		
	TTipController &owner() { return *(TTipController *)nodePrev(); }
	
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
	virtual void enable_(  ){ }
        virtual void disable_(  ){ }
			
	string nodeName(){ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
	void preDisable(int flag);	//Disable if delete
	void postDisable(int flag);     //Delete all DB if flag 1
	
    /** Private methods: */
    private:
	void LoadParmCfg(  );
	void SaveParmCfg(  );
	void FreeParmCfg(  );
	
	virtual TParamContr *ParamAttach( const string &name, int type );
	
    /**Attributes: */
    private:    
	TBDS::SName	m_bd;
	int	m_prm;
};


#endif // TCONTROLLER_H

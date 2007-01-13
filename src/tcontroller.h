
//OpenSCADA system file: tcontroller.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

class TTipDAQ;

class TController : public TCntrNode, public TConfig
{
    public:
	//Methods
     	TController( const string &name_c, const string &daq_db, TElem *cfgelem );
	virtual ~TController(  );
	
	const string &id() 	{ return m_id; }
	string name();
	string descr()		{ return m_descr; }
	
        string DB( )            { return m_db; }
        string tbl( );
        string fullDB( )        { return DB()+'.'+tbl(); }
	
	void name( const string &nm ) 	{ m_name = nm; }
	void descr( const string &dscr ){ m_descr = dscr; }	
	
    	bool toEnable()		{ return m_aen; }
    	bool toStart() 		{ return m_astart; }
	bool enableStat()	{ return en_st; }
	bool startStat()      	{ return run_st; }

	virtual void load( );
	virtual void save( );
	virtual void start( );
	virtual void stop( );
	void enable( );
	void disable( );

	//- Parameters -
	void list( vector<string> &list )	{ chldList(m_prm,list); }
        bool present( const string &name )	{ return chldPresent(m_prm,name); }
	void add( const string &name, unsigned type );
	void del( const string &name, bool full = false )	{ chldDel(m_prm,name,-1,full); }
        AutoHD<TParamContr> at( const string &name, const string &who = "th_contr" )
	{ return chldAt(m_prm,name); }
	
	TTipDAQ &owner()	{ return *(TTipDAQ *)nodePrev(); }
	
    protected:    
	//Attributes
	bool    en_st;    
	bool    run_st;
	
	//Methods
	virtual void enable_(  ) { }
        virtual void disable_(  ){ }
    
	virtual TParamContr *ParamAttach( const string &name, int type );    
    
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	void preDisable(int flag);	//Disable if delete
	void postDisable(int flag);     //Delete all DB if flag 1
	
    private:
	//Methods
	string nodeName()       { return m_id; }
    
	void LoadParmCfg(  );
	void SaveParmCfg(  );
	void FreeParmCfg(  );
	
	//Attributes
	string  &m_id;
        string  &m_name;
        string  &m_descr;
        bool    &m_aen;
        bool    &m_astart;
	
	string	m_db;
	int	m_prm;
};


#endif // TCONTROLLER_H

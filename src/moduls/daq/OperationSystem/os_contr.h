
//OpenSCADA system module DAQ.OperationSystem file: os_contr.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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
 
#ifndef SYS_H
#define SYS_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#include "da.h"

using std::string;
using std::vector;

namespace SystemCntr
{

//======================================================================
//==== TMdPrm 
//======================================================================
class TMdPrm : public TParamContr
{
    public:
    	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );	
	
	void enable();
	void disable();
	void load( );
	void save( );
	
	void autoC( bool val )	{ m_auto = val; }
	bool autoC( )	{ return m_auto; }

	//set perameter type
	void setType( const string &da_id );
	//get new value
	void getVal();
	
    protected:
        bool cfgChange( TCfg &cfg );	//config change
		       
	void vlGet( TVal &val );

	void postEnable();
	//void preDisable(int flag);
	
    private:
	bool	m_auto;	//Autocreated
	DA	*m_da;
};

//======================================================================
//==== TMdContr 
//======================================================================
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
    	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr();   

	AutoHD<TMdPrm> at( const string &nm )
        { return TController::at(nm); }

	void enable_( );
	void load( );
	void save( );
	void start( );
	void stop( );    
	
    protected:
	void prmEn( const string &id, bool val );
    	
    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	
	//Attributes
	int	en_res;         //Resource for enable params
	int	&m_per,     	// ms
		&m_prior;	// Process task priority
		
	bool    prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;    

	pthread_t procPthr;     // Process task thread
};

//======================================================================
//==== TTpContr 
//======================================================================
class TTpContr: public TTipDAQ
{
    public:
    	TTpContr( string name );
	~TTpContr();
	
	void postEnable();
	void modLoad( );

	void daList( vector<string> &da );
	void daReg( DA *da );
	DA  *daGet( const string &da );	
    
    private:
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );
	vector<DA *> m_da;
};

extern TTpContr *mod;

} //End namespace 

#endif //SYS_H


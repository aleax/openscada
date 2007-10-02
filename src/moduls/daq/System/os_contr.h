
//OpenSCADA system module DAQ.System file: os_contr.h
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
 
#ifndef OS_CONTR_H
#define OS_CONTR_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#include "da.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace SystemCntr
{

//======================================================================
//==== TMdPrm 
//======================================================================
class TMdContr;

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
	
	TMdContr &owner()       { return (TMdContr&)TParamContr::owner(); }
	
    protected:
        bool cfgChange( TCfg &cfg );	//config change
		       
	void vlGet( TVal &val );
	void vlArchMake( TVal &val );

	void postEnable( int flag );
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

	int period( )	{ return m_per; }
	int prior( )	{ return m_prior; }

	AutoHD<TMdPrm> at( const string &nm )
        { return TController::at(nm); }

	void load( );
	void save( );
	void enable_( );
	void start_( );
	void stop_( );    
	
    protected:
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	void prmEn( const string &id, bool val );
    	
    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	
	//Attributes
	Res	en_res;         //Resource for enable params
	int	&m_per,     	// ms
		&m_prior;	// Process task priority
		
	bool    prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;    

	double  tm_calc;	// Scheme's calc time
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
	
	void postEnable( int flag );
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

#endif //OS_CONTR_H


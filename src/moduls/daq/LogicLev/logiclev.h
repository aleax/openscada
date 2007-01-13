
//OpenSCADA system module DAQ.LogicLev file: logiclev.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko                                *
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
 
#ifndef LOGICLEV_H
#define LOGICLEV_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace LogicLev
{

//======================================================================
//==== TMdPrm 
//======================================================================
class TMdContr;
    
class TMdPrm : public TParamContr
{
    public:
	enum Mode { Free, DirRefl, Template };
	
    	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );	
	
	Mode mode( )	{ return m_wmode; }
        void mode( Mode md, const string &prm = "" );		
	
	void enable();
	void disable();
	void load( );
	void save( );
	
	void calc( bool first, bool last );	//Calc template's algoritmes
	
	TMdContr &owner()	{ return (TMdContr&)TParamContr::owner(); }
	
    private:
	//Data
	class SLnk
	{
	    public:
	        SLnk(int iid, const string &iprm_attr = "") : io_id(iid), prm_attr(iprm_attr) { }
	        int 	io_id;
		string  prm_attr;
	        AutoHD<TVal> aprm;
	};
	
	//Methods
        void postEnable( );
        void postDisable(int flag);
	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process			
						
        void vlGet( TVal &val );
        void vlSet( TVal &val );
	
	//- Template link operations -
        int lnkSize();
        int lnkId( int id );
        int lnkId( const string &id );
        SLnk &lnk( int num );
			
        void loadIO();
        void saveIO();
        void initTmplLnks();
							
        //Attributes
        string  &m_prm, m_wprm;
        int     &m_mode;        //Config parameter mode
        Mode    m_wmode;        //Work parameter mode

        TElem   p_el;           //Work atribute elements
	
	bool	chk_lnk_need;	//Check lnk need flag
	int 	moderes;	//Resource
	int	id_freq, id_start, id_stop;	//Fixed system attributes identifiers
        
	//Data
        struct STmpl
        {
            TValFunc     	val;
    	    vector<SLnk> 	lnk;
        };
	
	union
        {
            AutoHD<TValue> *prm_refl;   //Direct reflection
            STmpl *tmpl;                //Template
        };
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

	int period()	{ return m_per; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void load( );
	void save( );
	void start( );
	void stop( );    
	
    protected:
	void prmEn( const string &id, bool val );
	void postDisable(int flag);     	//Delete all DB if flag 1
    	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
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
	
	double 	tm_calc;	// Template functions calc time
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

	TElem   &prmIOE()	{ return el_prm_io; }
	
    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );
    
	//Attributes
	TElem   el_prm_io;	
};

extern TTpContr *mod;

} //End namespace 

#endif //LOGICLEV_H



//OpenSCADA system module DAQ.JavaLikeCalc file: virtual.h
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

#ifndef FREEFUNCLIBS_H
#define FREEFUNCLIBS_H

#include <string>
#include <vector>

#include <tfunction.h>
#include <tdaqs.h>

#include "freelib.h"
#include "freefunc.h"

using std::string;
using std::vector;

namespace JavaLikeCalc
{

//===================================================================
//================ Constant structure ===============================
//===================================================================
class NConst
{
    public:
        NConst( TFld::Type itp, const string &inm, const string &ival ) : 
	    tp(itp), name(inm), val(ival) { }
	
	TFld::Type tp;
        string name;
        string val;
};

//===================================================================
//================ Buildin functions structure ======================
//===================================================================
class BFunc
{
    public:
	BFunc( const char *inm, Reg::Code icd, int iprm ) : 
	    name(inm), code(icd), prm(iprm) { }	
    
	string 	name;
	Reg::Code code;
	int prm;
};

//===================================================================
//================ Parameter object =================================
//===================================================================
class Contr;

class Prm : public TParamContr
{
    public:
        Prm( string name, TTipParam *tp_prm );
	~Prm( );

	void enable();
	void disable();
	
	Contr &owner()	{ return (Contr&)TParamContr::owner(); }
			    
    private:
	void postEnable();
	//void postDisable(int flag);
		
        void vlSet( TVal &val );
        void vlGet( TVal &val );
	
    private:
	TElem  v_el;   //Values elem
};						

//===================================================================
//================ Controller object ================================
//===================================================================
class Contr: public TController, public TValFunc
{
    public:
        Contr( string name_c, const string &daq_db, ::TElem *cfgelem );
        ~Contr();
			
        int period()  { return m_per; }
        int iterate() { return m_iter; }
	
    private:
	//Methods
	bool cfgChange( TCfg &cfg );
	void preDisable(int flag);
	void postDisable(int flag);
	
	void load( );
        void save( );
        void start( );
        void stop( );
        void enable_( );
        void disable_( );
	
	TParamContr *ParamAttach( const string &name, int type );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	static void *Task( void *icntr );
	static void TaskDBSync(union sigval obj);
	
	//Attributes
        bool    prc_st,		// Process task active
		endrun_req,     // Request to stop of the Process task
		sync_st;        // Sync DB status
        int     &m_per,		// Calc period (ms)
		&m_prior,	// Calc task priority
		&m_dbper,	// DB sync period (s)
    		&m_iter;	// iteration number
	string	&m_fnc;		// Work function
	
	pthread_t procPthr;     // Process task thread
	timer_t sncDBTm;	// Sync DB timer
};

//===================================================================
//======================= Type DAQ object ===========================
//===================================================================
class TipContr : public TTipDAQ
{
    public:
	TipContr( string src );
	~TipContr();

	void modLoad( );
	void modSave( );
	void modStart( );
	void modStop( );

	string libTable()	{ return "UserFuncLibs"; }
	
	TElem &elVal()  { return val_el; }
	TElem &elLib()	{ return lb_el; }
	TElem &elFnc()	{ return fnc_el; }
	TElem &elFncIO(){ return fncio_el; }
	
	void lbList( vector<string> &ls ) 	{ chldList(m_lib,ls); }
	bool lbPresent( const string &id )	{ return chldPresent(m_lib,id); }
	void lbReg( Lib *lib )       		{ chldAdd(m_lib,lib); }
	void lbUnreg( const string &id, int flg = 0 )	{ chldDel(m_lib,id,-1,flg); }
	AutoHD<Lib> lbAt( const string &id )	{ return chldAt(m_lib,id); }
	
	int &parseRes( ){ return parse_res; }
	
        //Named constant
        NConst *constGet( const char *nm );
	
	//BuildIn functions
	BFunc *bFuncGet( const char *nm );	

    protected:
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	void postEnable( );
	//void preDisable(int flag);
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );

    private:
	int		m_lib;	//Function libraries
	TElem   	val_el, lb_el, fnc_el, fncio_el;
	
	//General parse data
	int     	parse_res;	//Syntax analisator
	vector<NConst>  m_const;        //Name constant table
	vector<BFunc>	m_bfunc;	//Buildin functions
};

extern TipContr *mod;

} //End namespace StatFunc

#endif //FREEFUNCLIBS_H


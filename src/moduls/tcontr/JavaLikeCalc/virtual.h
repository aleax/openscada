/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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
#include <tcontrollers.h>

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
        NConst( const char *inm, double ival ) : name(inm), val(ival) { }
	    
        string name;
        double val;
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
class Prm : public TParamContr
{
    public:
        Prm( string name, TTipParam *tp_prm );

	void enable();
	void disable();
			    
    private:
	void preDisable(int flag);
		
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
        Contr( string name_c, const TBDS::SName &bd, ::TElem *cfgelem );
        ~Contr();
			
        int period()  { return m_per; }
        int iterate() { return m_iter; }
	
    private:
	void postDisable(int flag);
	
	void load( );
        void save( );
        void start( );
        void stop( );
        void enable_( );
        void disable_( );
	
	TParamContr *ParamAttach( const string &name, int type );
        //================== Controll functions ========================
        void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	static void *Task(void *);
	
    private:
        bool    endrun;         // Command for stop task
        int     &m_per;         // calc period ms
        int     &m_iter;	// iteration number
	string	&m_fnc;		// Work function
        pthread_t pthr_tsk;	// task pthread header
};

//===================================================================
//================ Type controller object ===========================
//===================================================================
class TipContr : public TTipController
{
    public:
	TipContr( string src );
	~TipContr();

	void modLoad( );
	void modSave( );
	void modStart( );
	void modStop( );

	TBDS::SName 	BD();
	
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
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );

    private:
	void postEnable( );
	//void preDisable(int flag);
	TController *ContrAttach( const string &name, const TBDS::SName &bd);

    private:
	int		m_lib;	//Function libraries
	TElem   	val_el, lb_el, fnc_el, fncio_el;
	TBDS::SName     m_bd;
	
	//General parse data
	int     	parse_res;	//Syntax analisator
	vector<NConst>  m_const;        //Name constant table
	vector<BFunc>	m_bfunc;	//Buildin functions
};

extern TipContr *mod;

} //End namespace StatFunc

#endif //FREEFUNCLIBS_H


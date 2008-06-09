
//OpenSCADA system module DAQ.JavaLikeCalc file: virtual.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace JavaLikeCalc
{

//*************************************************
//* NConst: Constant structure                    *
//*************************************************
class NConst
{
    public:
	//Methods
	NConst( TFld::Type itp, const string &inm, const string &ival ) :
	    tp(itp), name(inm), val(ival) { }

	//Attributes
	TFld::Type	tp;
	string		name;
	string		val;
};

//*************************************************
//* BFunc: Buildin functions structure            *
//*************************************************
class BFunc
{
    public:
	//Methods
	BFunc( const char *inm, Reg::Code icd, int iprm ) :
	    name(inm), code(icd), prm(iprm) { }

	//Attributes
	string		name;
	Reg::Code	code;
	int		prm;
};

//*************************************************
//* Contr: Parameter object                       *
//*************************************************
class Contr;

class Prm : public TParamContr
{
    public:
	//Methods
	Prm( string name, TTipParam *tp_prm );
	~Prm( );

	void enable( );
	void disable( );

	Contr &owner( )	{ return (Contr&)TParamContr::owner(); }

    private:
	//Methods
	void postEnable( int flag );

	void vlSet( TVal &val );
	void vlGet( TVal &val );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	v_el;	//Values elem
};

//*************************************************
//* Contr: Controller object                      *
//*************************************************
class Contr: public TController, public TValFunc
{
    public:
	//Methods
	Contr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~Contr( );

	int period( )	{ return vmax(m_per,1); }
	int iterate( )	{ return m_iter; }

    protected:
	//Methods
	void postDisable( int flag );

	void load_( );
	void save_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );
	void loadFunc( );

	TParamContr *ParamAttach( const string &name, int type );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	static void *Task( void *icntr );

	//Attributes
	bool	prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task
	int	&m_per,		// Calc period (ms)
		&m_prior,	// Calc task priority
		&m_iter;	// iteration number
	string	&m_fnc;		// Work function

	pthread_t procPthr;	// Process task thread
};

//*************************************************
//* Type DAQ object                               *
//*************************************************
class TipContr : public TTipDAQ
{
    public:
	//Methods
	TipContr( string src );
	~TipContr( );

	void modStart( );
	void modStop( );

	string libTable( )	{ return "UserFuncLibs"; }

	TElem &elVal( )		{ return val_el; }
	TElem &elLib( )		{ return lb_el; }
	TElem &elFnc( )		{ return fnc_el; }
	TElem &elFncIO( )	{ return fncio_el; }

	void lbList( vector<string> &ls ) 	{ chldList(m_lib,ls); }
	bool lbPresent( const string &id )	{ return chldPresent(m_lib,id); }
	void lbReg( Lib *lib )       		{ chldAdd(m_lib,lib); }
	void lbUnreg( const string &id, int flg = 0 )	{ chldDel(m_lib,id,-1,flg); }
	AutoHD<Lib> lbAt( const string &id )	{ return chldAt(m_lib,id); }

	Res &parseRes( )			{ return parse_res; }

	//- Named constant -
	NConst *constGet( const char *nm );

	//- BuildIn functions -
	BFunc *bFuncGet( const char *nm );

	void compileFuncLangs( vector<string> &ls );
	string compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text );

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );

    private:
	//Attributes
	int		m_lib;		//Function libraries
	TElem		val_el, lb_el, fnc_el, fncio_el;

	//- General parse data -
	Res		parse_res;	//Syntax analisator
	vector<NConst>	m_const;	//Name constant table
	vector<BFunc>	m_bfunc;	//Buildin functions
};

extern TipContr *mod;

} //End namespace StatFunc

#endif //FREEFUNCLIBS_H


//OpenSCADA module DAQ.JavaLikeCalc file: virtual.h
/***************************************************************************
 *   Copyright (C) 2005-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
using namespace OSCADA;

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
	Prm( string name, TTypeParam *tp_prm );
	~Prm( );

	void enable( );
	void disable( );

	Contr &owner( ) const;

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Methods
	void postEnable( int flag );

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	v_el;	//Values elem
};

//*************************************************
//* Contr: Controller object                      *
//*************************************************
class Contr: public TController, public TPrmTempl::Impl
{
    public:
	//Methods
	Contr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~Contr( );

	TCntrNode &operator=( const TCntrNode &node );

	string getStatus( );

	int64_t period( )	{ return mPer; }
	string cron( )		{ return cfg("SCHEDULE").getS(); }
	string fnc( )		{ return cfg("FUNC").getS(); }
	int iterate( )		{ return mIter; }

	void redntDataUpdate( );

	//Attributes
	bool	isDAQTmpl;	// Using the DAQ-template

    protected:
	//Methods
	void postDisable( int flag );

	void load_( );
	void save_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );
	void loadFunc( bool onlyVl = false );

	void postIOCfgChange( );

	TParamContr *ParamAttach( const string &name, int type );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	static void *Task( void *icntr );

	//Attributes
	bool	prcSt,		// Process task active
		callSt,		// Calc now stat
		endrunReq,	// Request to stop of the Process task
		chkLnkNeed;	// Check lnks need flag
	int64_t	&mPrior,	// Calc task priority
		&mIter;		// iteration number

	int	idFreq, idStart, idStop;

	int64_t	mPer;
};

//*************************************************
//* Type DAQ object                               *
//*************************************************
class TpContr : public TTypeDAQ
{
    public:
	//Methods
	TpContr( string src );
	~TpContr( );

	void modInfo( vector<string> &list );
	string modInfo( const string &name );
	void modStart( );
	void modStop( );

	int safeTm( )		{ return mSafeTm; }
	void setSafeTm( int vl ){ mSafeTm = vl; modif(); }

	string libTable( )	{ return "UserFuncLibs"; }

	TElem &elVal( )		{ return val_el; }
	TElem &elLib( )		{ return lb_el; }
	TElem &elFnc( )		{ return fnc_el; }
	TElem &elFncIO( )	{ return fncio_el; }

	void lbList( vector<string> &ls ) const		{ chldList(mLib, ls); }
	bool lbPresent( const string &id ) const	{ return chldPresent(mLib, id); }
	void lbReg( Lib *lib )				{ chldAdd(mLib, lib); }
	void lbUnreg( const string &id, int flg = 0 )	{ chldDel(mLib, id, -1, flg); }
	AutoHD<Lib> lbAt( const string &id ) const	{ return chldAt(mLib, id); }

	ResRW &parseRes( )			{ return mParseRes; }

	// Named constant
	NConst *constGet( const char *nm );

	// BuildIn functions
	BFunc *bFuncGet( const char *nm );

	bool compileFuncLangs( vector<string> *ls = NULL );
	void compileFuncSynthHighl( const string &lang, XMLNode &shgl );
	string compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text, const string &usings = "", int maxCalcTm = 0 );

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );
	void save_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	bool redntAllow( )	{ return true; }

	TController *ContrAttach( const string &name, const string &daq_db );

    private:
	//Attributes
	int		mLib,		//Function libraries
			mSafeTm;	//Safe timeout
	TElem		val_el, lb_el, fnc_el, fncio_el;

	// General parse data
	ResRW		mParseRes;	//Syntax analisator
	vector<NConst>	mConst;		//Name constant table
	vector<BFunc>	mBFunc;		//Buildin functions
};

extern TpContr *mod;

} //End namespace StatFunc

#endif //FREEFUNCLIBS_H


//OpenSCADA system module DAQ.JavaLikeCalc file: freefunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2009 by Roman Savochenko                           *
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

#include <limits.h>
#include <math.h>

#include <tsys.h>
#include <tmess.h>
#include "virtual.h"
#include "freelib.h"
#include "freefunc.h"

using namespace JavaLikeCalc;

Func *JavaLikeCalc::p_fnc;

//*************************************************
//* Func: Function                                *
//*************************************************
Func::Func( const char *iid, const char *name ) :
    TConfig(&mod->elFnc()), TFunction(iid), parse_res(mod->parseRes( )),
    mName(cfg("NAME").getSd()), mDescr(cfg("DESCR").getSd()),
    max_calc_tm(cfg("MAXCALCTM").getId()),prg_src(cfg("FORMULA").getSd())
{
    cfg("ID").setS(id());
    mName = name;
    if( !mName.size() ) mName = id();
}

Func::~Func( )
{

}

void Func::postEnable( int flag )
{
    if( owner().DB().empty() )	modifClr();
}

void Func::preDisable( int flag )
{
    if( mTVal ) { delete mTVal; mTVal = NULL; }
}

void Func::postDisable( int flag )
{
    setStart(false);
    if( flag && !owner().DB().empty() )
    {
	try{ del( ); }
	catch(TError err)
	{ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

Lib &Func::owner( )
{
    return *((Lib*)nodePrev());
}

string Func::name( )
{
    return mName.size()?mName:id();
}

TCntrNode &Func::operator=( TCntrNode &node )
{
    Func *src_n = dynamic_cast<Func*>(&node);
    if( !src_n ) return *this;

    *(TConfig *)this = *(TConfig*)src_n;
    *(TFunction *)this = *(TFunction*)src_n;

    //> Set to DB
    cfg("ID").setS(mId);

    if( src_n->startStat( ) && !startStat( ) )	setStart( true );
}

Func &Func::operator=(Func &func)
{
    *(TConfig *)this = (TConfig&)func;
    *(TFunction *)this = (TFunction&)func;

    //> Set to DB
    cfg("ID").setS(mId);
}

void Func::setName( const string &nm )
{
    mName = nm;
    if(!owner().DB().empty()) modif();
}

void Func::setDescr( const string &dscr )
{
    mDescr = dscr;
    if(!owner().DB().empty()) modif();
}

void Func::setMaxCalcTm( int vl )
{
    max_calc_tm = vl;
    if(!owner().DB().empty()) modif();
}

void Func::setProg( const string &prg )
{
    prg_src = prg;
    if(!owner().DB().empty()) modif();
}

void Func::load_( )
{
    if( owner().DB().empty() || (!SYS->chkSelDB(owner().DB())) )	return;

    SYS->db().at().dataGet(owner().fullDB(),mod->nodePath()+owner().tbl(),*this);

    loadIO( );
}

void Func::loadIO( )
{
    TConfig cfg(&mod->elFncIO());

    vector<string> u_pos;
    cfg.cfg("F_ID").setS(id(),true);
    for( int fld_cnt=0; SYS->db().at().dataSeek(owner().fullDB()+"_io",mod->nodePath()+owner().tbl()+"_io",fld_cnt,cfg); fld_cnt++ )
    {
	string sid = cfg.cfg("ID").getS();

	//> Position storing
	int pos = cfg.cfg("POS").getI();

	while( pos >= u_pos.size() )    u_pos.push_back("");
	u_pos[pos] = sid;

	if( ioId(sid) < 0 )
	    ioIns( new IO(sid.c_str(),"",IO::Real,IO::Default), pos );

	//> Set values
	int id = ioId(sid);
	io(id)->setName(cfg.cfg("NAME").getS());
	io(id)->setType((IO::Type)cfg.cfg("TYPE").getI());
	io(id)->setFlg(cfg.cfg("MODE").getI());
	io(id)->setDef(cfg.cfg("DEF").getS());
	io(id)->setHide(cfg.cfg("HIDE").getB());
    }
    //> Remove holes
    for( int i_p = 0; i_p < u_pos.size(); i_p++ )
	if( u_pos[i_p].empty() ) { u_pos.erase(u_pos.begin()+i_p); i_p--; }
    //> Position fixing
    for( int i_p = 0; i_p < u_pos.size(); i_p++ )
    {
	int iid = ioId(u_pos[i_p]);
	if( iid != i_p ) try{ ioMove(iid,i_p); } catch(...){ }
    }
}

void Func::save_( )
{
    if( owner().DB().empty() )  return;

    SYS->db().at().dataSet(owner().fullDB(),mod->nodePath()+owner().tbl(),*this);

    //> Save io config
    saveIO( );
}

void Func::saveIO( )
{
    TConfig cfg(&mod->elFncIO());

    string io_bd = owner().fullDB()+"_io";
    string io_cfgpath = mod->nodePath()+owner().tbl()+"_io/";

    //> Save allow IO
    cfg.cfg("F_ID").setS(id(),true);
    for( int i_io = 0; i_io < ioSize(); i_io++ )
    {
	if( io(i_io)->flg()&Func::SysAttr ) continue;
	cfg.cfg("ID").setS(io(i_io)->id());
	cfg.cfg("NAME").setS(io(i_io)->name());
	cfg.cfg("TYPE").setI(io(i_io)->type());
	cfg.cfg("MODE").setI(io(i_io)->flg());
	cfg.cfg("DEF").setNoTransl(io(i_io)->type()!=IO::String);
	cfg.cfg("DEF").setS(io(i_io)->def());
	cfg.cfg("HIDE").setB(io(i_io)->hide());
	cfg.cfg("POS").setI(i_io);
	SYS->db().at().dataSet(io_bd,io_cfgpath,cfg);
    }

    //> Clear IO
    cfg.cfgViewAll(false);
    for( int fld_cnt=0; SYS->db().at().dataSeek(io_bd,io_cfgpath,fld_cnt++,cfg ); )
	if( ioId(cfg.cfg("ID").getS()) < 0 )
	{
	    SYS->db().at().dataDel(io_bd,io_cfgpath,cfg,true);
	    fld_cnt--;
	}
}

void Func::del( )
{
    if( !owner().DB().size() )  return;

    SYS->db().at().dataDel(owner().fullDB(),mod->nodePath()+owner().tbl(),*this,true);

    //> Delete io from DB
    delIO( );
}

void Func::delIO( )
{
    TConfig cfg(&mod->elFncIO());
    cfg.cfg("F_ID").setS(id(),true);
    SYS->db().at().dataDel(owner().fullDB()+"_io",mod->nodePath()+owner().tbl()+"_io",cfg);
}

void Func::preIOCfgChange()
{
    be_start = startStat();
    if( be_start )
    {
	setStart(false);
	if( mTVal ) { delete mTVal; mTVal = NULL; }
    }
    TFunction::preIOCfgChange();
}

void Func::postIOCfgChange()
{
    if( be_start ) setStart(true);
    TFunction::postIOCfgChange();
}

void Func::setStart( bool val )
{
    //> Start calc
    if( val )
    {
	progCompile( );
	run_st = true;
    }
    //> Stop calc
    else
    {
	ResAlloc res(calc_res,true);
	prg = "";
	regClear();
	regTmpClean( );
	funcClear();
	run_st = false;
    }
}

void Func::ioAdd( IO *io )
{
    TFunction::ioAdd(io);
    if(!owner().DB().empty()) modif();
}

void Func::ioIns( IO *io, int pos )
{
    TFunction::ioIns(io,pos);
    if(!owner().DB().empty()) modif();
}

void Func::ioDel( int pos )
{
    TFunction::ioDel(pos);
    if(!owner().DB().empty()) modif();
}

void Func::ioMove( int pos, int to )
{
    TFunction::ioMove(pos,to);
    if(!owner().DB().empty()) modif();
}

void Func::progCompile( )
{
    ResAlloc res(parse_res,true);
    ResAlloc res1(calc_res,true);

    p_fnc  = this;	//Parse func
    p_err  = "";	//Clear error messages
    la_pos = 0;		//LA position
    prg    = "";	//Clear programm
    regClear();		//Clear registers list
    regTmpClean( );	//Clear temporary registers list
    funcClear();	//Clear functions list

    if( yyparse( ) )
    {
	prg = "";
	regClear();
	regTmpClean( );
	funcClear();
	run_st = false;
	throw TError(nodePath().c_str(),"%s",p_err.c_str());
    }
    regTmpClean( );
}

int Func::funcGet( const string &path )
{
    string ns, f_path;

    //> Check to correct function's path
    try
    {
	if( dynamic_cast<TFunction*>(&SYS->nodeAt(path,0,'.').at()) )
	    f_path = SYS->nodeAt(path,0,'.').at().nodePath();
    }catch(...){ }

    if( f_path.empty() )
    {
	for( int off = 0; !(ns=TSYS::strSepParse(mUsings,0,';',&off)).empty(); )
	    try{ if( dynamic_cast<TFunction*>(&SYS->nodeAt(ns+"."+path,0,'.').at()) ) break; }
	    catch(...){ continue; }
	if( ns.empty() ) return -1;
	f_path = SYS->nodeAt(ns+"."+path,0,'.').at().nodePath();
    }

    //> Search for already registered function
    for( int i_fnc = 0; i_fnc < mFncs.size(); i_fnc++ )
	if( f_path == mFncs[i_fnc]->func().at().nodePath() )
	    return i_fnc;
    mFncs.push_back(new UFunc(ns.empty()?path:ns+"."+path));
    return mFncs.size()-1;
}

void Func::funcClear( )
{
    for( int i_fnc = 0; i_fnc < mFncs.size(); i_fnc++ )
	delete mFncs[i_fnc];
    mFncs.clear();
}

int Func::regNew( bool var )
{
    //> Get new register
    int i_rg = mRegs.size();
    if( !var )
	for( i_rg = 0; i_rg < mRegs.size(); i_rg++ )
	    if( !mRegs[i_rg]->lock() && mRegs[i_rg]->type() == Reg::Free )
		break;
    if( i_rg >= mRegs.size() ) mRegs.push_back(new Reg(i_rg));

    return i_rg;
}

int Func::regGet( const string &nm )
{
    //> Check allow registers
    for( int i_rg = 0; i_rg < mRegs.size(); i_rg++ )
	if( mRegs[i_rg]->name() == nm )
	    return i_rg;
    return -1;
}

int Func::ioGet( const string &nm )
{
    int rez = -1;

    if( nm == "SYS" )
    {
	rez = regNew();
	Reg *rg = regAt(rez);
	rg->setType(Reg::Obj);

	//> Make code
	uint16_t addr;
	prg += (uint8_t)Reg::MviSysObject;
	addr = rg->pos(); prg.append((char*)&addr,sizeof(uint16_t));
	prg += (char)0;
    }
    //> Check IO
    else
	for( int i_io = 0; i_io < ioSize(); i_io++ )
	    if( io(i_io)->id() == nm )
	    {
		rez = regNew(true);
		Reg *rg = regAt(rez);
		rg->setName(nm);
		rg->setVar(i_io);
		rg->setLock(true);
		break;
	    }
    return rez;
}

void Func::regClear( )
{
    for( int i_rg = 0; i_rg < mRegs.size(); i_rg++ )
        delete mRegs[i_rg];
    mRegs.clear();
}

Reg *Func::regTmpNew( )
{
    int i_rg;
    for( i_rg = 0; i_rg < mTmpRegs.size(); i_rg++ )
	if( mTmpRegs[i_rg]->type() == Reg::Free )
	    break;
    if( i_rg >= mTmpRegs.size() ) mTmpRegs.push_back(new Reg());
    return mTmpRegs[i_rg];
}

void Func::regTmpClean( )
{
    for( int i_rg = 0; i_rg < mTmpRegs.size(); i_rg++ )
	delete mTmpRegs[i_rg];
    mTmpRegs.clear();
}

Reg *Func::cdMvi( Reg *op, bool no_code )
{
    if( op->pos() >= 0 ) return op;	//Already load
    int r_id = p_fnc->regNew( );
    Reg *rez = regAt(r_id);
    *rez = *op;
    op->free();
    if( no_code ) return rez;
    uint16_t addr = rez->pos();

    switch(rez->type())
    {
	case Reg::Free: case Reg::Dynamic:
	    throw TError(nodePath().c_str(),_("Variable <%s> is used but undefined"),rez->name().c_str());
	case Reg::Bool:
	    prg+=(uint8_t)Reg::MviB;
	    prg.append((char *)&addr,sizeof(uint16_t));
	    prg+=(uint8_t)rez->val().b_el;
	    break;
	case Reg::Int:
	    prg+=(uint8_t)Reg::MviI;
	    prg.append((char *)&addr,sizeof(uint16_t));
	    prg.append((char *)&rez->val().i_el,sizeof(int));
	    break;
	case Reg::Real:
	    prg+=(uint8_t)Reg::MviR;
	    prg.append((char *)&addr,sizeof(uint16_t));
	    prg.append((char *)&rez->val().r_el,sizeof(double));
	    break;
	case Reg::String:
	    if( rez->val().s_el->size() > 255 )
		throw TError(nodePath().c_str(),_("String constant size is more 255 symbols."));
	    prg+=(uint8_t)Reg::MviS;
	    prg.append((char *)&addr,sizeof(uint16_t));
	    prg+=(uint8_t)rez->val().s_el->size();
	    prg+= *rez->val().s_el;
	    break;
    }
    return rez;
}

Reg *Func::cdMviObject( )
{
    //> Make result
    Reg *rez = regAt(regNew());
    rez->setType(Reg::Obj);

    //> Make code
    uint16_t addr;
    prg += (uint8_t)Reg::MviObject;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));

    return rez;
}

Reg *Func::cdMviArray( int p_cnt )
{
    deque<int> p_pos;

    //> Mvi all parameters
    for( int i_prm = 0; i_prm < p_cnt; i_prm++ )
	f_prmst[i_prm] = cdMvi( f_prmst[i_prm] );
    //> Get parameters.
    for( int i_prm = 0; i_prm < p_cnt; i_prm++ )
    {
	p_pos.push_front( f_prmst.front()->pos() );
	f_prmst.front()->free();
	f_prmst.pop_front();
    }
    //> Make result
    Reg *rez = regAt(regNew());
    rez->setType(Reg::Obj);

    //> Make code
    uint16_t addr;
    prg += (uint8_t)Reg::MviArray;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    prg += (uint8_t)p_cnt;
    for( int i_prm = 0; i_prm < p_pos.size(); i_prm++ )
    { addr = p_pos[i_prm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdTypeConv( Reg *op, Reg::Type tp, bool no_code )
{
    Reg *rez = op;
    if( rez->pos() < 0 )
    {
	if( tp != rez->vType(this) )
	    switch(tp)
	    {
		case Reg::Bool:
		    switch(rez->vType(this))
		    {
		        case Reg::Int:
			    *rez = (bool)rez->val().i_el;
			    break;
			case Reg::Real:
			    *rez = (bool)rez->val().r_el;
			    break;
			case Reg::String:
			    *rez = (bool)atoi(rez->val().s_el->c_str());
			    break;
		    }
		    break;
		case Reg::Int:
		    switch(rez->vType(this))
		    {
			case Reg::Bool:
			    *rez = (int)rez->val().b_el;
			    break;
			case Reg::Real:
			//    *rez = (int)rez->val().r_el;
			    break;
			case Reg::String:
			    *rez = atoi(rez->val().s_el->c_str());
			    break;
		    }
		    break;
		case Reg::Real:
		    switch(rez->vType(this))
		    {
			case Reg::Bool:
			    *rez = (double)rez->val().b_el;
			    break;
			case Reg::Int:
			    *rez = (double)rez->val().i_el;
			    break;
			case Reg::String:
			    *rez = atof(rez->val().s_el->c_str());
			    break;
		    }
		    break;
		case Reg::String:
		    switch(rez->vType(this))
		    {
			case Reg::Bool:
			    *rez = TSYS::int2str(rez->val().b_el);
			    break;
			case Reg::Int:
			    *rez = TSYS::int2str(rez->val().i_el);
			    break;
			case Reg::Real:
			    *rez = TSYS::real2str(rez->val().r_el);
			    break;
		    }
		    break;
	    }
	if(!no_code) rez = cdMvi(rez);
    }

    return rez;
}

void Func::cdAssign( Reg *rez, Reg *op )
{
    uint16_t addr;
    if( op->pos() < 0 ) op = cdMvi( op );
    prg += (uint8_t)Reg::Ass;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    addr = op->pos();  prg.append((char*)&addr,sizeof(uint16_t));
    op->free();		//> Free temp operands
}

Reg *Func::cdMove( Reg *rez, Reg *op, bool force )
{
    if( !force && !op->lock() ) return op;

    uint16_t addr;
    Reg *rez_n=rez;
    op = cdMvi( op );
    if( rez_n == NULL ) rez_n = regAt(regNew());
    rez_n = cdMvi( rez_n, true );
    rez_n->setType(op->vType(this));
    prg += (uint8_t)Reg::Mov;
    addr = rez_n->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    addr = op->pos();    prg.append((char*)&addr,sizeof(uint16_t));

    op->free();	//> Free temp operands

    return rez_n;
}

Reg *Func::cdBinaryOp( Reg::Code cod, Reg *op1, Reg *op2 )
{
    //> Check allowing type operations
    if( !op1->objEl() )
	switch( op1->vType(this) )
	{
	    case Reg::Bool:
		switch(cod)
		{
		    case Reg::BitShLeft: case Reg::BitShRight:
			throw TError(nodePath().c_str(),_("Operation %d don't support bool type"),cod);
		}
		break;
	    case Reg::String:
		switch(cod)
		{
		    case Reg::Sub: case Reg::Mul: case Reg::Div: case Reg::RstI: case Reg::BitOr: case Reg::BitAnd:
		    case Reg::BitXor: case Reg::BitShLeft: case Reg::BitShRight: case Reg::LOr: case Reg::LAnd:
		    case Reg::LT: case Reg::GT: case Reg::LEQ: case Reg::GEQ:
			throw TError(nodePath().c_str(),_("Operation %d don't support string type"),cod);
		}
		break;
	    case Reg::Obj:
		throw TError(nodePath().c_str(),_("Operation %d don't support for object type"),cod);
	}

    //> Check allow the buildin calc and calc
    if( op1->pos() < 0 && op2->pos() < 0 )
    {
	switch( cod )
	{
	    case Reg::Add: case Reg::Sub: case Reg::Mul:
	    case Reg::Div: case Reg::LT: case Reg::GT:
	    case Reg::LEQ: case Reg::GEQ: case Reg::EQU: case Reg::NEQ:
		if( op1->vType(this) != Reg::String )
		    op1 = cdTypeConv( op1, Reg::Real, true );
		break;
	}

	op2 = cdTypeConv( op2, op1->vType(this), true);
	switch( op1->vType(this) )
	{
	    case Reg::Int:
		switch(cod)
		{
		    case Reg::RstI:	*op1 = op1->val().i_el % op2->val().i_el;	break;
		    case Reg::BitOr:	*op1 = op1->val().i_el | op2->val().i_el;	break;
		    case Reg::BitAnd:	*op1 = op1->val().i_el & op2->val().i_el;	break;
		    case Reg::BitXor:	*op1 = op1->val().i_el ^ op2->val().i_el;	break;
		    case Reg::BitShLeft: *op1 = op1->val().i_el << op2->val().i_el;	break;
		    case Reg::BitShRight: *op1 = op1->val().i_el >> op2->val().i_el;	break;
		    case Reg::LOr:	*op1 = op1->val().i_el || op2->val().i_el;	break;
		    case Reg::LAnd:	*op1 = op1->val().i_el && op2->val().i_el;	break;
		}
		break;
	    case Reg::Real:
		switch(cod)
		{
		    case Reg::Add:	*op1 = op1->val().r_el + op2->val().r_el;	break;
		    case Reg::Sub:	*op1 = op1->val().r_el - op2->val().r_el;	break;
		    case Reg::Mul:	*op1 = op1->val().r_el * op2->val().r_el;	break;
		    case Reg::Div:	*op1 = op1->val().r_el / op2->val().r_el;	break;
		    case Reg::BitOr:	*op1 = (int)op1->val().r_el | (int)op2->val().r_el;	break;
		    case Reg::BitAnd:	*op1 = (int)op1->val().r_el & (int)op2->val().r_el;	break;
		    case Reg::BitXor:	*op1 = (int)op1->val().i_el ^ (int)op2->val().i_el;	break;
		    case Reg::BitShLeft: *op1 = (int)op1->val().i_el << (int)op2->val().i_el;	break;
		    case Reg::BitShRight: *op1 = (int)op1->val().i_el >> (int)op2->val().i_el;	break;
		    case Reg::LOr:	*op1 = op1->val().r_el || op2->val().r_el;	break;
		    case Reg::LAnd:	*op1 = op1->val().r_el && op2->val().r_el;	break;
		    case Reg::LT:	*op1 = op1->val().r_el < op2->val().r_el;	break;
		    case Reg::GT:	*op1 = op1->val().r_el > op2->val().r_el;	break;
		    case Reg::LEQ:	*op1 = op1->val().r_el <= op2->val().r_el;	break;
		    case Reg::GEQ:	*op1 = op1->val().r_el >= op2->val().r_el;	break;
		    case Reg::EQU:	*op1 = op1->val().r_el == op2->val().r_el;	break;
		    case Reg::NEQ:	*op1 = op1->val().r_el != op2->val().r_el;	break;
		}
		break;
	    case Reg::Bool:
		switch(cod)
		{
		    case Reg::RstI:	*op1 = op1->val().b_el % op2->val().b_el;	break;
		    case Reg::BitOr:	*op1 = op1->val().b_el | op2->val().b_el;	break;
		    case Reg::BitAnd:	*op1 = op1->val().b_el & op2->val().b_el;	break;
		    case Reg::BitXor:	*op1 = op1->val().b_el ^ op2->val().b_el;	break;
		    case Reg::LOr:	*op1 = op1->val().b_el || op2->val().b_el;	break;
		    case Reg::LAnd:	*op1 = op1->val().b_el && op2->val().b_el;	break;
		}	
	    case Reg::String:
		switch(cod)
		{
		    case Reg::Add:	*op1->val().s_el += *op2->val().s_el;		break;
		    case Reg::EQU:	*op1 = (char)(*op1->val().s_el == *op2->val().s_el);	break;
		    case Reg::NEQ:	*op1 = (char)(*op1->val().s_el != *op2->val().s_el);	break;
		}
		break;
	}

	return op1;
    }

    //> Make operation cod
    //>> Prepare operands
    op1 = cdMvi( op1 );
    Reg::Type op1_tp = op1->vType(this);
    int op1_pos = op1->pos();
    if( op1_tp != Reg::Dynamic ) op2 = cdTypeConv(op2,op1_tp);
    else if( op2->pos() < 0 ) op2 = cdMvi( op2 );
    int op2_pos = op2->pos();
    op1->free();
    op2->free();
    //>> Prepare rezult
    Reg *rez = regAt(regNew());
    rez->setType(op1_tp);
    //>> Add code
    switch(cod)
    {
	case Reg::Add:		prg += (uint8_t)Reg::Add;	break;
	case Reg::Sub:		prg += (uint8_t)Reg::Sub;	break;
	case Reg::Mul:		prg += (uint8_t)Reg::Mul;	break;
	case Reg::Div:		prg += (uint8_t)Reg::Div;	break;
	case Reg::RstI:		prg += (uint8_t)Reg::RstI;	break;
	case Reg::BitOr:	prg += (uint8_t)Reg::BitOr;	break;
	case Reg::BitAnd:	prg += (uint8_t)Reg::BitAnd;	break;
	case Reg::BitXor:	prg += (uint8_t)Reg::BitXor;	break;
	case Reg::BitShLeft:	prg += (uint8_t)Reg::BitShLeft;	rez->setType(Reg::Int);	break;
	case Reg::BitShRight:	prg += (uint8_t)Reg::BitShRight;	rez->setType(Reg::Int);	break;
	case Reg::LOr:		prg += (uint8_t)Reg::LOr;	rez->setType(Reg::Bool);	break;
	case Reg::LAnd:		prg += (uint8_t)Reg::LAnd;	rez->setType(Reg::Bool);	break;
	case Reg::LT:		prg += (uint8_t)Reg::LT;	rez->setType(Reg::Bool);	break;
	case Reg::GT:		prg += (uint8_t)Reg::GT;	rez->setType(Reg::Bool);	break;
	case Reg::LEQ:		prg += (uint8_t)Reg::LEQ;	rez->setType(Reg::Bool);	break;
	case Reg::GEQ:		prg += (uint8_t)Reg::GEQ;	rez->setType(Reg::Bool);	break;
	case Reg::EQU:		prg += (uint8_t)Reg::EQU;	rez->setType(Reg::Bool);	break;
	case Reg::NEQ:		prg += (uint8_t)Reg::NEQ;	rez->setType(Reg::Bool);	break;
	default: throw TError(nodePath().c_str(),_("Don't support operation code %d."),cod);
    }

    uint16_t addr;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    addr = op1_pos;    prg.append((char*)&addr,sizeof(uint16_t));
    addr = op2_pos;    prg.append((char*)&addr,sizeof(uint16_t));

    return rez;
}

Reg *Func::cdUnaryOp( Reg::Code cod, Reg *op )
{
    //> Check allowing type operations
    if( !op->objEl() )
	switch( op->type() )
	{
	    case Reg::String:
		switch(cod)
		{
		    case Reg::Not: case Reg::BitNot: case Reg::Neg:
			throw TError(nodePath().c_str(),_("Operation %d don't support string type"),cod);
		}
		break;
	    case Reg::Obj:
		throw TError(nodePath().c_str(),_("Operation %d don't support for object type"),cod);
	}

    //> Check allow the buildin calc and calc
    if( op->pos() < 0 )
    {
	switch(op->vType(this))
	{
	    case Reg::Int:
		switch(cod)
		{
		    case Reg::Not:	*op = !op->val().i_el;	break;
		    case Reg::BitNot:	*op = ~op->val().i_el;	break;
		    case Reg::Neg:	*op = -op->val().i_el;	break;
		}
		break;
	    case Reg::Obj:
	    case Reg::Real:
		switch(cod)
		{
		    case Reg::Not:	*op = !op->val().r_el;	break;
		    case Reg::BitNot:	*op = ~(int)op->val().r_el;	break;
		    case Reg::Neg:	*op = -op->val().r_el;	break;
		}
		break;
	    case Reg::Bool:
		switch(cod)
		{
		    case Reg::Not:	*op = !op->val().b_el;	break;
		    case Reg::BitNot:	*op = ~op->val().b_el;	break;
		    case Reg::Neg:	*op = -op->val().b_el;	break;
		}
	}
	return op;
    }

    //> Make operation cod
    //>> Prepare operand
    op = cdMvi( op );
    Reg::Type op_tp = op->vType(this);
    int op_pos = op->pos();
    op->free();
    //>> Prepare rezult
    Reg *rez = regAt(regNew());
    rez->setType(op->vType(this));
    //>> Add code
    switch(cod)
    {
	case Reg::Not:		prg += (uint8_t)Reg::Not;	break;
	case Reg::BitNot:	prg += (uint8_t)Reg::BitNot;	break;
	case Reg::Neg:		prg += (uint8_t)Reg::Neg;	break;
	default: throw TError(nodePath().c_str(),_("Don't support operation code %d."),cod);
    }
    uint16_t addr;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    addr = op_pos;     prg.append((char*)&addr,sizeof(uint16_t));

    return rez;
}

Reg *Func::cdCond( Reg *cond, int p_cmd, int p_else, int p_end, Reg *thn, Reg *els )
{
    Reg *rez = NULL;
    int a_sz = sizeof(uint16_t);
    string cd_tmp;

    //> Mvi cond register (insert to programm)
    cd_tmp = prg.substr(p_cmd);
    prg.erase(p_cmd);
    cond = cdMvi(cond);
    p_else += prg.size()-p_cmd;
    p_end += prg.size()-p_cmd;
    p_cmd = prg.size();
    prg += cd_tmp;
    uint16_t p_cond = cond->pos(); cond->free();

    if( thn != NULL && els != NULL )
    {
	//> Add Move command to "then" end (insert to programm)
	cd_tmp = prg.substr(p_else-1);	//-1 pass end command
	prg.erase(p_else-1);
	thn = cdMvi( thn );
	rez = cdMove(NULL,thn);
	p_end += prg.size()-p_else+1;
	p_else = prg.size()+1;
	prg+=cd_tmp;
	//> Add Move command to "else" end (insert to programm)
	cd_tmp = prg.substr(p_end-1);   //-1 pass end command
	prg.erase(p_end-1);
	els = cdMvi( els );
	cdMove(rez,els);
	p_end = prg.size()+1;
	prg += cd_tmp;
    }

    //> Make apropos adress
    p_else -= p_cmd;
    p_end  -= p_cmd;

    //> [CRR00nn]
    prg.replace(p_cmd+1,sizeof(uint16_t),(char*)&p_cond,sizeof(uint16_t));
    prg.replace(p_cmd+3,a_sz,((char *)&p_else),a_sz);
    prg.replace(p_cmd+3+a_sz,a_sz,((char *)&p_end),a_sz);

    return rez;
}

void Func::cdCycle( int p_cmd, Reg *cond, int p_solve, int p_end, int p_postiter )
{
    int a_sz = sizeof(uint16_t);
    string cd_tmp;
    int p_body = (p_postiter?p_postiter:p_solve)-1;	//Include Reg::End command

    //> Mvi cond register (insert to programm)
    cd_tmp = prg.substr(p_body);
    prg.erase(p_body);
    cond = cdMvi(cond);
    p_solve+=prg.size()-p_body;
    p_end+=prg.size()-p_body;
    if( p_postiter ) p_postiter+=prg.size()-p_body;
    prg+=cd_tmp;
    uint16_t p_cond = cond->pos();
    cond->free();

    //> Make apropos adress
    p_solve -= p_cmd;
    p_end   -= p_cmd;
    if( p_postiter ) p_postiter -= p_cmd;

    //> [CRRbbaann]
    prg.replace(p_cmd+1,sizeof(uint16_t),(char*)&p_cond,sizeof(uint16_t));
    prg.replace(p_cmd+3,a_sz,((char *)&p_solve),a_sz);
    prg.replace(p_cmd+3+a_sz,a_sz,((char *)&p_postiter),a_sz);
    prg.replace(p_cmd+3+2*a_sz,a_sz,((char *)&p_end),a_sz);
}

Reg *Func::cdBldFnc( int f_cod, Reg *prm1, Reg *prm2 )
{
    Reg *rez;
    uint16_t addr;
    int p1_pos = -1, p2_pos = -1;

    if( (prm1 && !prm1->objEl() && prm1->vType(this) == Reg::String) ||
	(prm2 && !prm2->objEl() && prm2->vType(this) == Reg::String) )
	throw TError(nodePath().c_str(),_("Buildin functions don't support string type"));
    //> Free parameter's registers
    if( prm1 )	{ prm1 = cdMvi( prm1 ); p1_pos = prm1->pos(); }
    if( prm2 )	{ prm2 = cdMvi( prm2 ); p2_pos = prm2->pos(); }
    if( prm1 )	prm1->free();
    if( prm2 )	prm2->free();
    //> Get rezult register
    rez = regAt(regNew());
    rez->setType(Reg::Real);
    //> Make code
    prg += (uint8_t)f_cod;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    if( p1_pos >= 0 )
    { addr = p1_pos; prg.append((char*)&addr,sizeof(uint16_t)); }
    if( p2_pos >= 0 )
    { addr = p2_pos; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdExtFnc( int f_id, int p_cnt, bool proc )
{
    int r_pos;	//Return function's position
    Reg *rez = NULL;
    deque<int> p_pos;

    //> Check return IO position
    bool ret_ok = false;
    for( r_pos = 0; r_pos < funcAt(f_id)->func().at().ioSize(); r_pos++ )
	if( funcAt(f_id)->func().at().io(r_pos)->flg()&IO::Return )
	{ ret_ok=true; break; }
    //> Check IO and parameters count
    if( p_cnt > funcAt(f_id)->func().at().ioSize()-ret_ok )
        throw TError(nodePath().c_str(),_("More than %d(%d) parameters are specified for function <%s>"),
	    (funcAt(f_id)->func().at().ioSize()-ret_ok),p_cnt,funcAt(f_id)->func().at().id().c_str());
    //> Check the present return for fuction
    if( !proc && !ret_ok )
	throw TError(nodePath().c_str(),_("Function is requested <%s>, but it doesn't have return of IO"),funcAt(f_id)->func().at().id().c_str());
    //> Mvi all parameters
    for( int i_prm = 0; i_prm < p_cnt; i_prm++ )
	f_prmst[i_prm] = cdMvi( f_prmst[i_prm] );
    //> Get parameters. Add check parameters type !!!!
    for( int i_prm = 0; i_prm < p_cnt; i_prm++ )
    {
	p_pos.push_front( f_prmst.front()->pos() );
	f_prmst.front()->free();
	f_prmst.pop_front();
    }
    //> Make result
    if( !proc )
    {
	rez = regAt(regNew());
	switch(funcAt(f_id)->func().at().io(r_pos)->type())
	{
	    case IO::String:	rez->setType(Reg::String);	break;
	    case IO::Integer:	rez->setType(Reg::Int);		break;
	    case IO::Real:	rez->setType(Reg::Real);	break;
	    case IO::Boolean:	rez->setType(Reg::Bool);	break;
	    case IO::Object:	rez->setType(Reg::Obj);		break;
	}
    }

    //> Make code
    uint16_t addr;
    prg += proc ? (uint8_t)Reg::CProc : (uint8_t)Reg::CFunc;
    prg += (uint8_t)f_id;
    prg += (uint8_t)p_cnt;
    addr = proc ? 0 : rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    for( int i_prm = 0; i_prm < p_pos.size(); i_prm++ )
    { addr = p_pos[i_prm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdObjFnc( Reg *obj, int p_cnt )
{
    if( !obj->objEl( ) )
	throw TError(nodePath().c_str(),_("No object variable for function"));

    Reg *rez = NULL;
    deque<int> p_pos;

    //> Mvi all parameters
    for( int i_prm = 0; i_prm < p_cnt; i_prm++ )
	f_prmst[i_prm] = cdMvi( f_prmst[i_prm] );
    //> Get parameters
    for( int i_prm = 0; i_prm < p_cnt; i_prm++ )
    {
	p_pos.push_front( f_prmst.front()->pos() );
	f_prmst.front()->free();
	f_prmst.pop_front();
    }
    obj->free();
    rez = regAt(regNew());
    rez->setType(Reg::Dynamic);

    //> Make code
    uint16_t addr;
    prg += (uint8_t)Reg::CFuncObj;
    addr = obj->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    prg += (uint8_t)p_cnt;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));

    for( int i_prm = 0; i_prm < p_pos.size(); i_prm++ )
    { addr = p_pos[i_prm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdProp( Reg *obj, const string &sprp, Reg *dprp )
{
    uint16_t addr;
    Reg *ro = obj;
    if( !ro->objEl() ) { ro = cdMove( NULL, cdMvi(ro), false ); ro->setObjEl(); }

    if( !dprp )
    {
	prg += (uint8_t)Reg::OPrpSt;
	addr = ro->pos(); prg.append((char*)&addr,sizeof(uint16_t));
	prg += (uint8_t)sprp.size();
	prg += sprp;
    }
    else
    {
	dprp = cdMvi( dprp );

	prg += (uint8_t)Reg::OPrpDin;
	addr = ro->pos(); prg.append((char*)&addr,sizeof(uint16_t));
	addr = dprp->pos(); prg.append((char*)&addr,sizeof(uint16_t));

	dprp->free();
    }
    return ro;
}

TVariant Func::oPropGet( TVariant vl, const string &prop )
{
    switch( vl.type() )
    {
	case TVariant::Object:	return vl.getO()->propGet(prop);
	case TVariant::Boolean:	throw TError(nodePath().c_str(),_("Boolean type have not one properties."));
	case TVariant::Integer:
	    if( prop == "MAX_VALUE" )	return INT_MAX;
	    if( prop == "MIN_VALUE" )	return INT_MIN;
	    if( prop == "NaN" )		return EVAL_INT;
	    throw TError(nodePath().c_str(),_("Integer type have not properties '%s'."),prop.c_str());
	case TVariant::Real:
	    if( prop == "MAX_VALUE" )	return 3.4e300;
	    if( prop == "MIN_VALUE" )	return -3.4e300;
	    if( prop == "NaN" )		return EVAL_REAL;
	    throw TError(nodePath().c_str(),_("Real type have not properties '%s'."),prop.c_str());
	case TVariant::String:
	    if( prop == "length" )	return (int)vl.getS().size();
	    return vl.getS().substr(vmax(0,vmin(vl.getS().size()-1,atoi(prop.c_str()))),1);
    }
    throw TError(nodePath().c_str(),_("Get properties '%s' from value type '%d' error."),prop.c_str(),vl.type());
}

TVariant Func::oFuncCall( TVariant vl, const string &prop, vector<TVariant> &prms )
{
    switch( vl.type() )
    {
	case TVariant::Object:	return vl.getO()->funcCall( prop, prms );
	case TVariant::Boolean:
	    if( prop == "toString" )	return string(vl.getB() ? "true" : "false");
	    throw TError(nodePath().c_str(),_("Boolean type have not function '%s' or not enough parameters for it."),prop.c_str());
	case TVariant::Integer:
	case TVariant::Real:
	    if( prop == "toExponential" )
	    {
		int n = prms.size() ? vmax(0,vmin(20,prms[0].getI())) : -1;
		if( n < 0 )	return TSYS::strMess("%e",vl.getR());
		return TSYS::strMess("%.*e",n,vl.getR());
	    }
	    if( prop == "toFixed" )
	    {
		int n = prms.size() ? vmax(0,vmin(20,prms[0].getI())) : 0;
		return TSYS::strMess("%.*f",n,vl.getR());
	    }
	    if( prop == "toPrecision" )
	    {
		int n = prms.size() ? vmax(1,vmin(21,prms[0].getI())) : -1;
		if( n < 0 )	return TSYS::strMess("%g",vl.getR());
		return TSYS::strMess("%.*g",n,vl.getR());
	    }
	    if( prop == "toString" )
	    {
		int n = 10;
		if( prms.size() ) n = prms[0].getI();
		return TSYS::strMess( (n==16)?"%x":((n==8)?"%o":"%d"),vl.getI() );
	    }
	    throw TError(nodePath().c_str(),_("Integer type have not function '%s' or not enough parameters for it."),prop.c_str());
	case TVariant::String:
	    if( prop == "charAt" && prms.size() )
	    {
		int n = prms[0].getI();
		if( n < 0 || n >= vl.getS().size() )	return string("");
		return vl.getS().substr(n,1);
	    }
	    if( prop == "charCodeAt" && prms.size() )
	    {
		int n = prms[0].getI();
		if( n < 0 || n >= vl.getS().size() )	return EVAL_INT;
		return (int)vl.getS()[n];
	    }
	    if( prop == "concat" && prms.size() )
	    {
		string rez = vl.getS();
		for( int i_p = 0; i_p < prms.size(); i_p++ )
		    rez += prms[i_p].getS();
		return rez;
	    }
	    if( prop == "indexOf" && prms.size() )
	    {
		int sp = 0;
		if( prms.size() > 1 ) sp = vmax(0,vmin(vl.getS().size()-1,prms[1].getI()));
		sp = vl.getS().find(prms[0].getS(),sp);
		return (sp==string::npos)?-1:sp;
	    }
	    if( prop == "lastIndexOf" && prms.size() )
	    {
		int sp = vl.getS().size()-1;
		if( prms.size() > 1 ) sp = vmax(0,vmin(vl.getS().size()-1,prms[1].getI()));
		sp = vl.getS().rfind(prms[0].getS(),sp);
		return (sp==string::npos)?-1:sp;
	    }
	    if( (prop == "slice" || prop == "substring") && prms.size() )
	    {
		int beg = prms[0].getI();
		if( beg < 0 ) beg = vl.getS().size()+beg;
		int end = vl.getS().size();
		if( prms.size()>=2 ) end = prms[1].getI();
		if( end < 0 ) end = vl.getS().size()+end;
		end = vmin(end,vl.getS().size());
		if( beg >= end ) return string("");
		return vl.getS().substr(beg,end-beg);
	    }
	    if( prop == "split" && prms.size() )
	    {
		TArrayObj *rez = new TArrayObj();
		for( int posB = 0, i_p = 0; true; i_p++ )
		{
		    if( prms.size() > 1 && rez->size() >= prms[1].getI() ) break;
		    int posC = vl.getS().find(prms[0].getS(),posB);
		    if( posC != posB )
			rez->propSet( TSYS::int2str(i_p), vl.getS().substr(posB,posC-posB) );
		    if( posC == string::npos ) break;
		    posB = posC+prms[0].getS().size();
		}
		return rez;
	    }
	    if( prop == "insert" && prms.size() >= 2 )
		return vl.getS().insert( vmax(0,vmin(vl.getS().size(),prms[0].getI())), prms[1].getS() );
	    if( prop == "replace" && prms.size() >= 3 )
	    {
		int pos = prms[0].getI();
		if( pos < 0 || pos >= vl.getS().size() ) return vl;
		int n = prms[1].getI();
		if( n < 0 ) n = vl.getS().size();
		n = vmin(vl.getS().size()-pos,n);
		return vl.getS().replace( pos, n, prms[2].getS() );
	    }
	    if( prop == "toReal" ) return atof(vl.getS().c_str());
	    if( prop == "toInt" ) return (int)strtol(vl.getS().c_str(),NULL,0);
	    if( prop == "parse" && prms.size() )
	    {
		int off = (prms.size() >= 3) ? prms[2].getI() : 0;
		string rez = TSYS::strSepParse( vl.getS(), prms[0].getI(),
		    (prms.size() >= 2 && prms[1].getS().size()) ? prms[1].getS()[0] : '.', &off );
		if( prms.size() >= 3 ) { prms[2].setI(off); prms[2].setModify(); }
		return rez;
	    }
	    if( prop == "parsePath" && prms.size() )
	    {
		int off = (prms.size() >= 2) ? prms[1].getI() : 0;
		string rez = TSYS::pathLev( vl.getS(), prms[0].getI(), &off );
		if( prms.size() >= 2 ) { prms[1].setI(off); prms[1].setModify(); }
		return rez;
	    }
	    if( prop == "path2sep" )
		return TSYS::path2sepstr( vl.getS(), (prms.size() && prms[0].getS().size()) ? prms[0].getS()[0] : '.' );

	    throw TError(nodePath().c_str(),_("Integer type have not properties '%s' or not enough parameters for it."),prop.c_str());
    }
    return TVariant();
}

TVariant Func::getVal( TValFunc *io, RegW &rg, bool fObj )
{
    TVariant vl(string(EVAL_STR));

    //> Get base value
    switch( rg.type() )
    {
	case Reg::Bool:		vl = rg.val().b_el;	break;
	case Reg::Int:		vl = rg.val().i_el;	break;
	case Reg::Real:		vl = rg.val().r_el;	break;
	case Reg::String:	vl = *rg.val().s_el;	break;
	case Reg::Var:
	    switch(io->ioType(rg.val().io))
	    {
		case IO::Boolean:	vl = io->getB(rg.val().io);	break;
		case IO::Integer:	vl = io->getI(rg.val().io);	break;
		case IO::Real:		vl = io->getR(rg.val().io);	break;
		case IO::String:	vl = io->getS(rg.val().io);	break;
		case IO::Object:	vl = io->getO(rg.val().io);	break;
	    }
	    break;
	case Reg::PrmAttr:
	    switch(rg.val().p_attr->at().fld().type())
	    {
		case TFld::Boolean:	vl = rg.val().p_attr->at().getB();	break;
		case TFld::Integer:	vl = rg.val().p_attr->at().getI();	break;
		case TFld::Real:	vl = rg.val().p_attr->at().getR();	break;
		case TFld::String:	vl = rg.val().p_attr->at().getS();	break;
	    }
	    break;
	case Reg::Obj:	vl = rg.val().o_el;	break;
    }

    for( int i_p = 0; i_p < rg.propSize( ); i_p++ )
    {
	if( fObj && i_p == (rg.propSize( )-1) ) break;
	vl = oPropGet(vl,rg.propGet(i_p));
    }

    return vl;
}

string Func::getValS( TValFunc *io, RegW &rg )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Bool:	return (rg.val().b_el!=EVAL_BOOL) ? TSYS::int2str((bool)rg.val().b_el) : EVAL_STR;
	    case Reg::Int:	return (rg.val().i_el!=EVAL_INT) ? TSYS::int2str(rg.val().i_el) : EVAL_STR;
	    case Reg::Real:	return (rg.val().r_el!=EVAL_REAL) ? TSYS::real2str(rg.val().r_el) : EVAL_STR;
	    case Reg::String:	return *rg.val().s_el;
	    case Reg::Var:	return io->getS(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().p_attr->at().getS();
//	    case Reg::Obj:	return vl.getO();
	}
    else return getVal(io,rg).getS();

    return EVAL_STR;
}

int Func::getValI( TValFunc *io, RegW &rg )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Bool:	return (rg.val().b_el!=EVAL_BOOL) ? (bool)rg.val().b_el : EVAL_INT;
	    case Reg::Int:	return rg.val().i_el;
	    case Reg::Real:	return (rg.val().r_el!=EVAL_REAL) ? (int)rg.val().r_el : EVAL_INT;
	    case Reg::String:	return ((*rg.val().s_el)!=EVAL_STR) ? atoi(rg.val().s_el->c_str()) : EVAL_INT;
	    case Reg::Var:	return io->getI(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().p_attr->at().getI();
//	    case Reg::Obj:	return vl.getO();
	}
    else return getVal(io,rg).getI();

    return EVAL_INT;
}

double Func::getValR( TValFunc *io, RegW &rg )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Bool:	return (rg.val().b_el!=EVAL_BOOL) ? (bool)rg.val().b_el : EVAL_REAL;
	    case Reg::Int:	return (rg.val().i_el!=EVAL_INT) ? rg.val().i_el : EVAL_REAL;
	    case Reg::Real:	return rg.val().r_el;
	    case Reg::String:	return ((*rg.val().s_el)!=EVAL_STR) ? atof(rg.val().s_el->c_str()) : EVAL_REAL;
	    case Reg::Var:	return io->getR(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().p_attr->at().getR();
//	    case Reg::Obj:	return vl.getO();
	}
    else return getVal(io,rg).getR();

    return EVAL_REAL;
}

char Func::getValB( TValFunc *io, RegW &rg )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Bool:	return rg.val().b_el;
	    case Reg::Int:	return (rg.val().i_el!=EVAL_INT) ? (bool)rg.val().i_el : EVAL_BOOL;
	    case Reg::Real:	return (rg.val().r_el!=EVAL_REAL) ? (bool)rg.val().r_el : EVAL_BOOL;
	    case Reg::String:	return ((*rg.val().s_el)!=EVAL_STR) ? (bool)atoi(rg.val().s_el->c_str()) : EVAL_BOOL;
	    case Reg::Var:	return io->getB(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().p_attr->at().getB();
//	    case Reg::Obj:	return vl.getO();
	}
    else return getVal(io,rg).getB();

    return EVAL_BOOL;
}

TVarObj *Func::getValO( TValFunc *io, RegW &rg )
{
    if( rg.propEmpty( ) )
    {
	switch( rg.type() )
	{
	    case Reg::Obj:	return rg.val().o_el;
	    case Reg::Var:
		if( io->ioType(rg.val().io) == IO::Object ) return io->getO(rg.val().io);
	}
	throw TError(nodePath().c_str(),_("Get object from no object's register"));
    }
    else return getVal(io,rg).getO();
}

void Func::setVal( TValFunc *io, RegW &rg, const TVariant &val )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Var:
		switch( val.type() )
		{
		    case TVariant::Boolean:	io->setB(rg.val().io,val.getB());	break;
		    case TVariant::Integer:	io->setI(rg.val().io,val.getI());	break;
		    case TVariant::Real:	io->setR(rg.val().io,val.getR());	break;
		    case TVariant::String:	io->setS(rg.val().io,val.getS());	break;
		    case TVariant::Object:	io->setO(rg.val().io,val.getO());	break;
		}
		break;
	    case Reg::PrmAttr:
		switch( val.type() )
		{
		    case TVariant::Boolean:	rg.val().p_attr->at().setB(val.getB());	break;
		    case TVariant::Integer:	rg.val().p_attr->at().setI(val.getI());	break;
		    case TVariant::Real:	rg.val().p_attr->at().setR(val.getR());	break;
		    case TVariant::String:	rg.val().p_attr->at().setS(val.getS());	break;
		}
		break;
	    default:
		switch( val.type() )
		{
		    case TVariant::Boolean:	rg = val.getB();	break;
		    case TVariant::Integer:	rg = val.getI();	break;
		    case TVariant::Real:	rg = val.getR();	break;
		    case TVariant::String:	rg = val.getS();	break;
		    case TVariant::Object:	rg = val.getO();	break;
		}
		break;
	}
    else if( rg.type() == Reg::Obj )
    {
	TVariant vl(rg.val().o_el);
	for( int i_p = 0; i_p < rg.propSize( ); i_p++ )
	    if( i_p < (rg.propSize( )-1) ) vl = vl.getO()->propGet(rg.propGet(i_p));
	    else vl.getO()->propSet(rg.propGet(i_p),val);    
    }
}

void Func::setValS( TValFunc *io, RegW &rg, const string &val )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Var:	io->setS(rg.val().io,val);		break;
	    case Reg::PrmAttr:	rg.val().p_attr->at().setS(val);	break;
	    default: rg = val; break;
	}
    else setVal(io,rg,val);
}

void Func::setValI( TValFunc *io, RegW &rg, int val )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Var:	io->setI(rg.val().io,val);		break;
	    case Reg::PrmAttr:	rg.val().p_attr->at().setI(val);	break;
	    default: rg = val; break;
	}
    else setVal(io,rg,val);
}

void Func::setValR( TValFunc *io, RegW &rg, double val )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Var:	io->setR(rg.val().io,val);		break;
	    case Reg::PrmAttr:	rg.val().p_attr->at().setR(val);	break;
	    default: rg = val; break;
	}
    else setVal(io,rg,val);
}

void Func::setValB( TValFunc *io, RegW &rg, char val )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Var:	io->setB(rg.val().io,val);		break;
	    case Reg::PrmAttr:	rg.val().p_attr->at().setB(val);	break;
	    default: rg = val; break;
	}
    else setVal(io,rg,val);
}

void Func::setValO( TValFunc *io, RegW &rg, TVarObj *val )
{
    if( rg.propEmpty( ) )
	switch( rg.type() )
	{
	    case Reg::Var:	io->setO(rg.val().io,val);	break;
	    case Reg::PrmAttr:	break;
	    default: rg = val; break;
	}
    else setVal(io,rg,val);
}

void Func::calc( TValFunc *val )
{
    ResAlloc res(calc_res,false);
    if( !startStat( ) )	return;

    //> Init list of registers
    RegW reg[mRegs.size()];
    for( int i_rg = 0; i_rg < mRegs.size(); i_rg++ )
    {
	reg[i_rg].setType(mRegs[i_rg]->type());
	if(reg[i_rg].type() == Reg::Var)
	    reg[i_rg].val().io = mRegs[i_rg]->val().io;
	else if(reg[i_rg].type() == Reg::PrmAttr)
	    *reg[i_rg].val().p_attr = *mRegs[i_rg]->val().p_attr;
    }

    //> Exec calc
    ExecData dt = { 1, 0, 0 };
    try{ exec(val,reg,(const uint8_t*)prg.c_str(),dt); }
    catch(TError err){ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    res.release();
    if( dt.flg&0x07 == 0x01 )	setStart(false);
}

void Func::exec( TValFunc *val, RegW *reg, const uint8_t *cprg, ExecData &dt )
{
    while( !(dt.flg&0x01) )
    {
	//> Calc time control mechanism
	if( !((dt.com_cnt++)%1000) )
	{
	    if( !dt.start_tm )	dt.start_tm = time(NULL);
	    else if( time(NULL) > dt.start_tm+max_calc_tm )
	    {
		mess_err(nodePath().c_str(),_("Timeouted function calc."));
		dt.flg|=0x01;
		return;
	    }
	}
	//> Calc operation
	switch(*cprg)
	{
	    case Reg::EndFull: dt.flg |= 0x01;
	    case Reg::End: return;
	    //>> MVI codes
	    case Reg::MviB:
#if OSC_DEBUG >= 5
		printf("CODE: Load bool %d to reg %d.\n",*(uint8_t*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		reg[*(uint16_t*)(cprg+1)] = *(char*)(cprg+3);
		cprg += 4; break;
	    case Reg::MviI:
#if OSC_DEBUG >= 5
		printf("CODE: Load integer %d to reg %d.\n",*(int*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		reg[*(uint16_t*)(cprg+1)] = *(int*)(cprg+3);
		cprg += 3+sizeof(int); break;
	    case Reg::MviR:
#if OSC_DEBUG >= 5
		printf("CODE: Load real %f to reg %d.\n",*(double *)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		reg[*(uint16_t*)(cprg+1)] = *(double *)(cprg+3);
		cprg += 3+sizeof(double); break;
	    case Reg::MviS:
#if OSC_DEBUG >= 5
		printf("CODE: Load string %s(%d) to reg %d.\n",string((char*)cprg+4,*(uint8_t*)(cprg+3)).c_str(),*(uint8_t*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		reg[*(uint16_t*)(cprg+1)] = string((char*)cprg+4,*(uint8_t*)(cprg+3));
		cprg += 4+ *(uint8_t*)(cprg+3); break;
	    case Reg::MviObject:
#if OSC_DEBUG >= 5
		printf("CODE: Load object to reg %d.\n",*(uint16_t*)(cprg+1));
#endif
		reg[*(uint16_t*)(cprg+1)] = new TVarObj();
		cprg += 3; break;
	    case Reg::MviArray:
	    {
#if OSC_DEBUG >= 5
		printf("CODE: Load array elements %d to reg %d.\n",*(uint8_t*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		TArrayObj *ar = new TArrayObj();
		int pcnt = *(uint8_t*)(cprg+3);
		//>>> Fill array by empty elements number
		if( pcnt == 1 )
		    for( int i_p = 0; i_p < getValI(val,reg[*(uint16_t*)(cprg+4)]); i_p++ )
			ar->propSet(TSYS::int2str(i_p),EVAL_REAL);
		//>>> Fill array by parameters
		else
		    for( int i_p = 0; i_p < *(uint8_t*)(cprg+3); i_p++ )
			switch( reg[*(uint16_t*)(cprg+4+i_p*sizeof(uint16_t))].vType(this) )
			{
			    case Reg::Bool: ar->propSet(TSYS::int2str(i_p),getValB(val,reg[*(uint16_t*)(cprg+4+i_p*sizeof(uint16_t))]));	break;
			    case Reg::Int: ar->propSet(TSYS::int2str(i_p),getValI(val,reg[*(uint16_t*)(cprg+4+i_p*sizeof(uint16_t))]));		break;
			    case Reg::Real: ar->propSet(TSYS::int2str(i_p),getValR(val,reg[*(uint16_t*)(cprg+4+i_p*sizeof(uint16_t))]));	break;
			    case Reg::String: ar->propSet(TSYS::int2str(i_p),getValS(val,reg[*(uint16_t*)(cprg+4+i_p*sizeof(uint16_t))]));	break;
			    case Reg::Obj: ar->propSet( TSYS::int2str(i_p), reg[*(uint16_t*)(cprg+4+i_p*sizeof(uint16_t))].val().o_el );	break;
			}
		reg[*(uint16_t*)(cprg+1)] = ar;
		cprg += 4 + *(uint8_t*)(cprg+3)*sizeof(uint16_t); break;
	    }
	    case Reg::MviSysObject:
	    {
#if OSC_DEBUG >= 5
		printf("CODE: Load system object %s(%d) to reg %d.\n",string((char*)cprg+4,*(uint8_t*)(cprg+3)).c_str(),*(uint8_t*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		reg[*(uint16_t*)(cprg+1)] = new TCntrNodeObj(SYS->nodeAt(string((char*)cprg+4,*(uint8_t*)(cprg+3)),0,'.'),val->user());
		cprg += 4+ *(uint8_t*)(cprg+3); break;
	    }
	    //>> Assign codes
	    case Reg::Ass:
	    {
#if OSC_DEBUG >= 5
		printf("CODE: Assign from %d to %d.\n",*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		if( !reg[*(uint16_t*)(cprg+1)].propSize( ) )
		    switch( reg[*(uint16_t*)(cprg+3)].vType(this) )
		    {
			case Reg::Bool:		setValB(val,reg[*(uint16_t*)(cprg+1)],getValB(val,reg[*(uint16_t*)(cprg+3)]));	break;
			case Reg::Int:		setValI(val,reg[*(uint16_t*)(cprg+1)],getValI(val,reg[*(uint16_t*)(cprg+3)]));	break;
			case Reg::Real:		setValR(val,reg[*(uint16_t*)(cprg+1)],getValR(val,reg[*(uint16_t*)(cprg+3)]));	break;
			case Reg::String:	setValS(val,reg[*(uint16_t*)(cprg+1)],getValS(val,reg[*(uint16_t*)(cprg+3)]));	break;
			case Reg::Obj:		setVal(val,reg[*(uint16_t*)(cprg+1)],getVal(val,reg[*(uint16_t*)(cprg+3)]));	break;
		    }
		else setVal(val,reg[*(uint16_t*)(cprg+1)],getVal(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    }
	    //>> Mov codes
	    case Reg::Mov:
#if OSC_DEBUG >= 5
		printf("CODE: Move from %d to %d.\n",*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+1));
#endif
		switch( reg[*(uint16_t*)(cprg+3)].vType(this) )
		{
		    case Reg::Bool:	reg[*(uint16_t*)(cprg+1)] = getValB(val,reg[*(uint16_t*)(cprg+3)]);	break;
		    case Reg::Int:	reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]);	break;
		    case Reg::Real:	reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]);	break;
		    case Reg::String:	reg[*(uint16_t*)(cprg+1)] = getValS(val,reg[*(uint16_t*)(cprg+3)]);	break;
		    case Reg::Obj:	reg[*(uint16_t*)(cprg+1)] = getValO(val,reg[*(uint16_t*)(cprg+3)]);	break;
		}
		cprg+=5; break;
	    //>> Load properties for object
	    case Reg::OPrpSt:
#if OSC_DEBUG >= 5
		printf("CODE: Set object's %d properties to string len %d(%s)\n",*(uint16_t*)(cprg+1),*(uint8_t*)(cprg+3),string((char*)cprg+4,*(uint8_t*)(cprg+3)).c_str());
#endif
		reg[*(uint16_t*)(cprg+1)].propAdd(string((char*)cprg+4,*(uint8_t*)(cprg+3)));
		cprg += 4 + *(uint8_t*)(cprg+3); break;
	    case Reg::OPrpDin:
#if OSC_DEBUG >= 5
		printf("CODE: Set object's %d properties to register's %d value\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)].propAdd(getValS(val,reg[*(uint16_t*)(cprg+3)]));
		cprg += 5;  break;
	    //>> Binary operations
	    case Reg::Add:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d + %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		if( !reg[*(uint16_t*)(cprg+3)].propSize( ) )
		    switch( reg[*(uint16_t*)(cprg+3)].vType(this) )
		    {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) + getValR(val,reg[*(uint16_t*)(cprg+5)]);
			    break;
			case Reg::String:
			    reg[*(uint16_t*)(cprg+1)] = getValS(val,reg[*(uint16_t*)(cprg+3)]) + getValS(val,reg[*(uint16_t*)(cprg+5)]);
			    break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'\n"));
		    }
		else
		{
		    TVariant op1 = getVal(val,reg[*(uint16_t*)(cprg+3)]);
		    switch( op1.type() )
		    {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[*(uint16_t*)(cprg+1)] = op1.getR() + getValR(val,reg[*(uint16_t*)(cprg+5)]); break;
			case TVariant::String:
			    reg[*(uint16_t*)(cprg+1)] = op1.getS() + getValS(val,reg[*(uint16_t*)(cprg+5)]); break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'\n"));
		    }
		}
		cprg+=7; break;
	    case Reg::Sub:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d - %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) - getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::Mul:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d * %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) * getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::Div:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d / %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) / getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::RstI:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d % %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]) % getValI(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::BitOr:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d | %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]) | getValI(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::BitAnd:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d & %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]) & getValI(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::BitXor:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d ^ %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]) ^ getValI(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::BitShLeft:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d << %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]) << getValI(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::BitShRight:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d >> %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValI(val,reg[*(uint16_t*)(cprg+3)]) >> getValI(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::LOr:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d || %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = (getValB(val,reg[*(uint16_t*)(cprg+3)])==1) || (getValB(val,reg[*(uint16_t*)(cprg+5)])==1);
		cprg+=7; break;
	    case Reg::LAnd:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d && %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = (getValB(val,reg[*(uint16_t*)(cprg+3)])==1) && (getValB(val,reg[*(uint16_t*)(cprg+5)])==1);
		cprg+=7; break;
	    case Reg::LT:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d < %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) < getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::GT:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d > %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) > getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::LEQ:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d <= %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) <= getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::GEQ:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d >= %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) >= getValR(val,reg[*(uint16_t*)(cprg+5)]);
		cprg+=7; break;
	    case Reg::EQU:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d == %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		if( !reg[*(uint16_t*)(cprg+3)].propSize( ) )
		    switch( reg[*(uint16_t*)(cprg+3)].vType(this) )
		    {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) == getValR(val,reg[*(uint16_t*)(cprg+5)]);
			    break;
			case Reg::String:
			    reg[*(uint16_t*)(cprg+1)] = getValS(val,reg[*(uint16_t*)(cprg+3)]) == getValS(val,reg[*(uint16_t*)(cprg+5)]);
			    break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'EQU'\n"));
		    }
		else
		{
		    TVariant op1 = getVal(val,reg[*(uint16_t*)(cprg+3)]);
		    switch( op1.type() )
		    {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[*(uint16_t*)(cprg+1)] = op1.getR() == getValR(val,reg[*(uint16_t*)(cprg+5)]); break;
			case TVariant::String:
			    reg[*(uint16_t*)(cprg+1)] = op1.getS() == getValS(val,reg[*(uint16_t*)(cprg+5)]); break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'EQU'\n"));
		    }
		}
		cprg+=7; break;
	    case Reg::NEQ:
#if OSC_DEBUG >= 5
		printf("CODE: %d = %d != %d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		if( !reg[*(uint16_t*)(cprg+3)].propSize( ) )
		    switch( reg[*(uint16_t*)(cprg+3)].vType(this) )
		    {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)]) != getValR(val,reg[*(uint16_t*)(cprg+5)]);
			    break;
			case Reg::String:
			    reg[*(uint16_t*)(cprg+1)] = getValS(val,reg[*(uint16_t*)(cprg+3)]) != getValS(val,reg[*(uint16_t*)(cprg+5)]);
			    break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'\n"));
		    }
		else
		{
		    TVariant op1 = getVal(val,reg[*(uint16_t*)(cprg+3)]);
		    switch( op1.type() )
		    {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[*(uint16_t*)(cprg+1)] = op1.getR() != getValR(val,reg[*(uint16_t*)(cprg+5)]); break;
			case TVariant::String:
			    reg[*(uint16_t*)(cprg+1)] = op1.getS() != getValS(val,reg[*(uint16_t*)(cprg+5)]); break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'\n"));
		    }
		}
		cprg+=7; break;
	    //>> Unary operations
	    case Reg::Not:
#if OSC_DEBUG >= 5
		printf("CODE: %d = !%d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = !getValB(val,reg[*(uint16_t*)(cprg+3)]);
		cprg+=5; break;
	    case Reg::BitNot:
#if OSC_DEBUG >= 5
		printf("CODE: %d = ~%d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = ~getValI(val,reg[*(uint16_t*)(cprg+3)]);
		cprg+=5; break;
	    case Reg::Neg:
#if OSC_DEBUG >= 5
		printf("CODE: %d = -%d.\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = -getValR(val,reg[*(uint16_t*)(cprg+3)]);
		cprg+=5; break;
	    //>> Condition
	    case Reg::If:
#if OSC_DEBUG >= 5
		printf("CODE: Condition %d: %d|%d|%d.\n",*(uint16_t*)(cprg+1),7,*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		if( getValB(val,reg[*(uint16_t*)(cprg+1)]) )	exec(val,reg,cprg+7,dt);
		else if( *(uint16_t*)(cprg+3) != *(uint16_t*)(cprg+5) )	exec(val,reg,cprg + *(uint16_t*)(cprg+3),dt);
		cprg = cprg + *(uint16_t*)(cprg+5);
		continue;
	    case Reg::Cycle:
#if OSC_DEBUG >= 5
		printf("CODE: Cycle %d: %d|%d|%d|%d.\n",*(uint16_t*)(cprg+1),9,*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5),*(uint16_t*)(cprg+7));
#endif
		while( !(dt.flg&0x01) )
		{
		    exec(val,reg,cprg+9,dt);
		    if( !getValB(val,reg[*(uint16_t*)(cprg+1)]) )	break;
		    dt.flg &= ~0x06;
		    exec( val, reg, cprg + *(uint16_t*)(cprg+3), dt );
		    //Check break and continue operators
		    if( dt.flg&0x02 )	{ dt.flg=0; break; }
		    else if( dt.flg&0x04 ) dt.flg=0;

		    if( *(uint16_t*)(cprg+5) ) exec( val, reg, cprg + *(uint16_t*)(cprg+5),dt);
		}
		cprg = cprg + *(uint16_t*)(cprg+7);
		continue;
	    case Reg::Break:	dt.flg|=0x03;	break;
	    case Reg::Continue:	dt.flg|=0x05;	break;
	    //>> Buildin functions
	    case Reg::FSin:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=sin(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = sin(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FCos:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=cos(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = cos(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FTan:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=tan(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = tan(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FSinh:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=sinh(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = sinh(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FCosh:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=cosh(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = cosh(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FTanh:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=tanh(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = tanh(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FAsin:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=asin(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = asin(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FAcos:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=acos(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = acos(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FAtan:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=atan(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = atan(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FRand:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=rand(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = getValR(val,reg[*(uint16_t*)(cprg+3)])*(double)rand()/(double)RAND_MAX;
		cprg+=5; break;
	    case Reg::FLg:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=lg(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = log10(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FLn:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=ln(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = log(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FExp:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=exp(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = exp(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FPow:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=pow(%d,%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = pow(getValR(val,reg[*(uint16_t*)(cprg+3)]),getValR(val,reg[*(uint16_t*)(cprg+5)]));
		cprg+=7; break;
	    case Reg::FMin:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=min(%d,%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = vmin(getValR(val,reg[*(uint16_t*)(cprg+3)]),getValR(val,reg[*(uint16_t*)(cprg+5)]));
		cprg+=7; break;
	    case Reg::FMax:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=max(%d,%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3),*(uint16_t*)(cprg+5));
#endif
		reg[*(uint16_t*)(cprg+1)] = vmax(getValR(val,reg[*(uint16_t*)(cprg+3)]),getValR(val,reg[*(uint16_t*)(cprg+5)]));
		cprg+=7; break;
	    case Reg::FSqrt:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=sqrt(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = sqrt(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FAbs:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=abs(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = fabs(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FSign:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=sign(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = (getValR(val,reg[*(uint16_t*)(cprg+3)])>=0)?1:-1;
		cprg+=5; break;
	    case Reg::FCeil:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=ceil(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = ceil(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::FFloor:
#if OSC_DEBUG >= 5
		printf("CODE: Function %d=floor(%d).\n",*(uint16_t*)(cprg+1),*(uint16_t*)(cprg+3));
#endif
		reg[*(uint16_t*)(cprg+1)] = floor(getValR(val,reg[*(uint16_t*)(cprg+3)]));
		cprg+=5; break;
	    case Reg::CProc:
	    case Reg::CFunc:
	    {
		TValFunc vfnc("JavaLikeFuncCalc",&funcAt(*(uint8_t*)(cprg+1))->func().at());
#if OSC_DEBUG >= 5
		printf("CODE: Call function/procedure %d = %s(%d).\n",*(uint16_t*)(cprg+3),vfnc.func()->id().c_str(),*(uint8_t*)(cprg+2));
#endif
		//>>> Get return position
		int r_pos, i_p, p_p;
		for( r_pos = 0; r_pos < vfnc.func()->ioSize(); r_pos++ )
		    if( vfnc.ioFlg(r_pos)&IO::Return ) break;
		//>>> Process parameters
		for( i_p = 0; i_p < *(uint8_t*)(cprg+2); i_p++ )
		{
		    p_p = (i_p>=r_pos)?i_p+1:i_p;
		    switch(vfnc.ioType(p_p))
		    {
			case IO::String:	vfnc.setS(p_p,getValS(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))])); break;
			case IO::Integer:	vfnc.setI(p_p,getValI(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))])); break;
			case IO::Real:		vfnc.setR(p_p,getValR(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))])); break;
			case IO::Boolean:	vfnc.setB(p_p,getValB(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))])); break;
			case IO::Object:	vfnc.setO(p_p,getValO(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))])); break;
		    }
		}
		//>>> Make calc
		vfnc.calc(vfnc.user());
		//>>> Process outputs
		for( i_p = 0; i_p < *(uint8_t*)(cprg+2); i_p++ )
		{
		    p_p = (i_p>=r_pos)?i_p+1:i_p;
		    if( vfnc.ioFlg(p_p)&IO::Output )
			switch(vfnc.ioType(p_p))
			{
			    case IO::String:	setValS(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))],vfnc.getS(p_p)); break;
			    case IO::Integer:	setValI(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))],vfnc.getI(p_p)); break;
			    case IO::Real:	setValR(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))],vfnc.getR(p_p)); break;
			    case IO::Boolean:	setValB(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))],vfnc.getB(p_p)); break;
			    case IO::Object:	setValO(val,reg[*(uint16_t*)(cprg+5+i_p*sizeof(uint16_t))],vfnc.getO(p_p)); break;
			}
		}
		//>>> Set return
		if( *cprg == Reg::CFunc )
		    switch(vfnc.ioType(r_pos))
		    {
			case IO::String:	reg[*(uint16_t*)(cprg+3)] = vfnc.getS(r_pos); break;
			case IO::Integer:	reg[*(uint16_t*)(cprg+3)] = vfnc.getI(r_pos); break;
			case IO::Real:		reg[*(uint16_t*)(cprg+3)] = vfnc.getR(r_pos); break;
			case IO::Boolean:	reg[*(uint16_t*)(cprg+3)] = vfnc.getB(r_pos); break;
			case IO::Object:	reg[*(uint16_t*)(cprg+3)] = vfnc.getO(r_pos); break;
		    }

		cprg += 5 + (*(uint8_t*)(cprg+2))*sizeof(uint16_t); break;
	    }
	    case Reg::CFuncObj:
	    {
#if OSC_DEBUG >= 5
		printf("CODE: Call object's function %d = %d(%d).\n",*(uint16_t*)(cprg+4),*(uint16_t*)(cprg+1),*(uint8_t*)(cprg+3));
#endif

		if( reg[*(uint16_t*)(cprg+1)].propEmpty() )
		    throw TError(nodePath().c_str(),_("Call object's function for no object or function name is empty."));

		TVariant obj = getVal(val,reg[*(uint16_t*)(cprg+1)],true);

		//> Prepare inputs
		vector<TVariant> prms;
		for( int i_p = 0; i_p < *(uint8_t*)(cprg+3); i_p++ )
		    prms.push_back(getVal(val,reg[*(uint16_t*)(cprg+6+i_p*sizeof(uint16_t))]));

		//> Call
		TVariant rez = oFuncCall( obj, reg[*(uint16_t*)(cprg+1)].propGet(reg[*(uint16_t*)(cprg+1)].propSize()-1), prms );
		//> Process outputs
		for( int i_p = 0; i_p < prms.size(); i_p++ )
		    if( prms[i_p].isModify() )
			switch( prms[i_p].type() )
			{
			    case TVariant::Boolean:	setValB(val,reg[*(uint16_t*)(cprg+6+i_p*sizeof(uint16_t))],prms[i_p].getB());	break;
			    case TVariant::Integer:	setValI(val,reg[*(uint16_t*)(cprg+6+i_p*sizeof(uint16_t))],prms[i_p].getI());	break;
			    case TVariant::Real:	setValR(val,reg[*(uint16_t*)(cprg+6+i_p*sizeof(uint16_t))],prms[i_p].getR());	break;
			    case TVariant::String:	setValS(val,reg[*(uint16_t*)(cprg+6+i_p*sizeof(uint16_t))],prms[i_p].getS());	break;
			    case TVariant::Object:	setValO(val,reg[*(uint16_t*)(cprg+6+i_p*sizeof(uint16_t))],prms[i_p].getO());	break;
			}
		//> Process return
		switch( rez.type() )
		{
		    case TVariant::Boolean:	reg[*(uint16_t*)(cprg+4)] = rez.getB();	break;
		    case TVariant::Integer:	reg[*(uint16_t*)(cprg+4)] = rez.getI();	break;
		    case TVariant::Real:	reg[*(uint16_t*)(cprg+4)] = rez.getR();	break;
		    case TVariant::String:	reg[*(uint16_t*)(cprg+4)] = rez.getS();	break;
		    case TVariant::Object:	reg[*(uint16_t*)(cprg+4)] = rez.getO();	break;
		}

		cprg += 6 + (*(uint8_t*)(cprg+3))*sizeof(uint16_t); break;
	    }
	    default:
		setStart(false);
		throw TError(nodePath().c_str(),_("Operation %c(%xh) error. Function <%s> is stoped."),*cprg,*cprg,id().c_str());
	}
    }
}

void Func::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TFunction::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function: ")+name(),owner().DB().empty()?0444:0664,"root","root");
	ctrMkNode("fld",opt,-1,"/func/cfg/name",_("Name"),owner().DB().empty()?0444:0664,"root","root",2,"tp","str","len","50");
	ctrMkNode("fld",opt,-1,"/func/cfg/descr",_("Description"),owner().DB().empty()?0444:0664,"root","root",3,"tp","str","cols","100","rows","5");
	ctrMkNode("fld",opt,-1,"/func/cfg/m_calc_tm",_("Maximum calc time (sec)"),0664,"root","root",1,"tp","dec");
	if(ctrMkNode("area",opt,-1,"/io",_("Programm")))
	{
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),0664,"root","root",1,"s_com","add,del,ins,move"))
	    {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Id"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),0664,"root","root",5,"tp","dec","idm","1","dest","select",
		    "sel_id",(TSYS::int2str(IO::Real)+";"+TSYS::int2str(IO::Integer)+";"+TSYS::int2str(IO::Boolean)+";"+TSYS::int2str(IO::String)).c_str(),
		    "sel_list",_("Real;Integer;Boolean;String"));
		ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),0664,"root","root",5,"tp","dec","idm","1","dest","select",
		    "sel_id",(TSYS::int2str(IO::Default)+";"+TSYS::int2str(IO::Output)+";"+TSYS::int2str(IO::Return)).c_str(),
		    "sel_list",_("Input;Output;Return"));
		ctrMkNode("list",opt,-1,"/io/io/4",_("Hide"),0664,"root","root",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/io/io/5",_("Default"),0664,"root","root",1,"tp","str");
	    }
	    ctrMkNode("fld",opt,-1,"/io/prog",_("Programm"),0664,"root","root",2,"tp","str","rows","10");
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/func/cfg/name" && ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )		setName( opt->text() );
    else if( a_path == "/func/cfg/descr" && ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDescr( opt->text() );
    else if( a_path == "/func/cfg/m_calc_tm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(maxCalcTm()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setMaxCalcTm( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/io/io" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/io/io/0","",0664);
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/io/io/1","",0664);
	    XMLNode *n_type	= ctrMkNode("list",opt,-1,"/io/io/2","",0664);
	    XMLNode *n_mode	= ctrMkNode("list",opt,-1,"/io/io/3","",0664);
	    XMLNode *n_hide	= ctrMkNode("list",opt,-1,"/io/io/4","",0664);
	    XMLNode *n_def	= ctrMkNode("list",opt,-1,"/io/io/5","",0664);
	    for( int id = 0; id < ioSize(); id++ )
	    {
		if(n_id)	n_id->childAdd("el")->setText(io(id)->id());
		if(n_nm)	n_nm->childAdd("el")->setText(io(id)->name());
		if(n_type)	n_type->childAdd("el")->setText(TSYS::int2str(io(id)->type()));
		if(n_mode)	n_mode->childAdd("el")->setText(TSYS::int2str(io(id)->flg()&(IO::Output|IO::Return)));
		if(n_hide)	n_hide->childAdd("el")->setText(io(id)->hide()?"1":"0");
		if(n_def)	n_def->childAdd("el")->setText(io(id)->def());
	    }
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	ioAdd( new IO("new",_("New IO"),IO::Real,IO::Default) );
	if( ctrChkNode(opt,"ins",0664,"root","root",SEQ_WR) )	ioIns( new IO("new",_("New IO"),IO::Real,IO::Default), atoi(opt->attr("row").c_str()) );
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	ioDel( atoi(opt->attr("row").c_str()) );
	if( ctrChkNode(opt,"move",0664,"root","root",SEQ_WR) )	ioMove( atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    int row = atoi(opt->attr("row").c_str());
	    int col = atoi(opt->attr("col").c_str());
	    if( (col == 0 || col == 1) && !opt->text().size() )
	        throw TError(nodePath().c_str(),_("Empty value is not valid."));
	    if( col == 0 )	io(row)->setId(opt->text());
	    else if( col == 1 )	io(row)->setName(opt->text());
	    else if( col == 2 )	io(row)->setType((IO::Type)atoi(opt->text().c_str()));
	    else if( col == 3 )	io(row)->setFlg(io(row)->flg()^((io(row)->flg()^atoi(opt->text().c_str()))&(IO::Output|IO::Return)));
	    else if( col == 4 )	io(row)->setHide(atoi(opt->text().c_str()));
	    else if( col == 5 )	io(row)->setDef(opt->text());
	    if(!owner().DB().empty()) modif();
	}
    }
    else if( a_path == "/io/prog" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( prog() );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	{ setProg( opt->text() ); progCompile(); }
    }
    else TFunction::cntrCmdProc(opt);
}

//*************************************************
//* Reg                                           *
//*************************************************
Reg::~Reg( )
{
    setType(Free);
}

Reg &Reg::operator=( Reg &irg )
{
    setType(irg.type());
    switch( type() )
    {
	case Bool:	el.b_el = irg.el.b_el;	 break;
	case Int:	el.i_el = irg.el.i_el;	break;
	case Real:	el.r_el = irg.el.r_el;	break;
	case String:	*el.s_el = *irg.el.s_el;break;
	case Obj:
	    el.o_el = irg.el.o_el;
	    if( el.o_el ) el.o_el->connect();
	    break;
	case Var:	el.io = irg.el.io;	break;
	case PrmAttr:	*el.p_attr = *irg.el.p_attr;	break;
    }
    setName(irg.name());	//name
    mLock = irg.mLock;		//locked
    return *this;
}

void Reg::operator=( TVarObj *ivar )
{
    setType(Obj);
    el.o_el = ivar;
    el.o_el->connect();
}

void Reg::setType( Type tp )
{
    if( mTp == tp && mTp != Reg::Obj )	return;
    //Free old type
    switch( mTp )
    {
	case Reg::String:	delete el.s_el;		break;
	case Reg::Obj:
	    if( el.o_el && !el.o_el->disconnect() ) delete el.o_el;
	    break;
	case Reg::PrmAttr:	delete el.p_attr;	break;
    }
    //Set new type
    switch( tp )
    {
	case Reg::String:	el.s_el = new string;	break;
	case Reg::Obj:		el.o_el = NULL;	break;
	case Reg::PrmAttr:	el.p_attr = new AutoHD<TVal>;	break;
    }
    mTp = tp;
}

Reg::Type Reg::vType( Func *fnc )
{
    switch(type())
    {
	case Free: return Int;
	case Var:
	    switch(fnc->io(val().io)->type())
	    {
		case IO::String:	return String;
		case IO::Boolean:	return Bool;
		case IO::Integer:	return Int;
		case IO::Real:		return Real;
		case IO::Object:	return Obj;
	    }
	case PrmAttr:
	    switch(val().p_attr->at().fld().type())
	    {
		case TFld::Boolean:	return Bool;
		case TFld::Integer:	return Int;
		case TFld::Real:	return Real;
		case TFld::String:	return String;
	    }
    }
    return type();
}

void Reg::free()
{
    if( lock() ) return;

    setType(Free);
    mObjEl = mLock = false;
}

//*************************************************
//* RegW : Work register                          *
//*************************************************
void RegW::operator=( TVarObj *ivar )
{
    setType(Reg::Obj);
    el.o_el = ivar;
    el.o_el->connect();
}

void RegW::setType( Reg::Type tp )
{
    mPrps.clear();
    if( mTp == tp && mTp != Reg::Obj )    return;
    //Free old type
    switch(mTp)
    {
	case Reg::String:	delete el.s_el;		break;
	case Reg::Obj:
	    if( el.o_el && !el.o_el->disconnect() ) delete el.o_el;
	    break;
	case Reg::PrmAttr:	delete el.p_attr;	break;
    }
    //Set new type
    switch(tp)
    {
	case Reg::String:	el.s_el = new string;	break;
	case Reg::Obj:		el.o_el = NULL;	break;
	case Reg::PrmAttr:	el.p_attr = new AutoHD<TVal>;	break;
    }
    mTp = tp;
}

Reg::Type RegW::vType( Func *fnc )
{
    switch( type() )
    {
	case Reg::Free: return Reg::Int;
	case Reg::Var:
	    switch(fnc->io(val().io)->type())
	    {
		case IO::String:	return Reg::String;
		case IO::Boolean:	return Reg::Bool;
		case IO::Integer:	return Reg::Int;
		case IO::Real:		return Reg::Real;
		case IO::Object:	return Reg::Obj;
	    }
	    break;
	case Reg::PrmAttr:
	    switch(val().p_attr->at().fld().type())
	    {
		case TFld::Boolean:	return Reg::Bool;
		case TFld::Integer:	return Reg::Int;
		case TFld::Real:	return Reg::Real;
		case TFld::String:	return Reg::String;
	    }
	    break;
    }
    return type();
}

string RegW::propGet( int id )
{
    if( id < 0 || id >= mPrps.size() ) return "";
    return mPrps[id];
}

void RegW::propAdd( const string &vl )
{
    mPrps.push_back(vl);
}

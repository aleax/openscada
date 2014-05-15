
//OpenSCADA system module DAQ.JavaLikeCalc file: freefunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko                           *
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

#include <string.h>
#include <limits.h>
#include <stdint.h>
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
Func::Func( const string &iid, const string &name ) :
    TConfig(&mod->elFnc()), TFunction(iid,SDAQ_ID),
    mMaxCalcTm(cfg("MAXCALCTM").getId()), mTimeStamp(cfg("TIMESTAMP").getId()), parse_res(mod->parseRes())
{
    cfg("ID").setS(id());
    cfg("NAME").setS(name.empty() ? id() : name);
    mMaxCalcTm = mod->safeTm();
}

Func::~Func( )
{

}

void Func::postEnable( int flag )
{
    if(owner().DB().empty()) modifClr();
}

void Func::postDisable( int flag )
{
    setStart(false);
    if(flag && !owner().DB().empty())
    {
	try{ del(); }
	catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

Lib &Func::owner( )	{ return *((Lib*)nodePrev()); }

string Func::name( )
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : id();
}

TCntrNode &Func::operator=( TCntrNode &node )
{
    Func *src_n = dynamic_cast<Func*>(&node);
    if(!src_n) return *this;

    *(TConfig *)this = *(TConfig*)src_n;
    *(TFunction *)this = *(TFunction*)src_n;

    //Set to DB
    cfg("ID").setS(mId);

    if(src_n->startStat() && !startStat()) setStart(true);

    return *this;
}

Func &Func::operator=(Func &func)
{
    *(TConfig *)this = (TConfig&)func;
    *(TFunction *)this = (TFunction&)func;

    //Set to DB
    cfg("ID").setS(mId);

    return *this;
}

void Func::setName( const string &nm )
{
    cfg("NAME").setS(nm);
    if(owner().DB().empty()) modifClr();
}

void Func::setDescr( const string &dscr )
{
    cfg("DESCR").setS(dscr);
    if(owner().DB().empty()) modifClr();
}

void Func::setMaxCalcTm( int vl )
{
    mMaxCalcTm = vl;
    if(!owner().DB().empty()) modif();
}

void Func::setProg( const string &prg )
{
    cfg("FORMULA").setS(prg);
    if(owner().DB().empty()) modifClr();
}

void Func::load_( )
{
    if(owner().DB().empty() || (!SYS->chkSelDB(owner().DB())))	return;

    cfg("FORMULA").setNoTransl(!owner().progTr());
    SYS->db().at().dataGet(owner().fullDB(), mod->nodePath()+owner().tbl(), *this);

    loadIO( );
}

void Func::loadIO( )
{
    TConfig cfg(&mod->elFncIO());

    vector<string> u_pos;
    cfg.cfg("F_ID").setS(id(), true);
    for(int fld_cnt=0; SYS->db().at().dataSeek(owner().fullDB()+"_io",mod->nodePath()+owner().tbl()+"_io",fld_cnt,cfg); fld_cnt++)
    {
	string sid = cfg.cfg("ID").getS();

	//Position storing
	int pos = cfg.cfg("POS").getI();

	while(pos >= (int)u_pos.size())	u_pos.push_back("");
	u_pos[pos] = sid;

	if(ioId(sid) < 0) ioIns(new IO(sid.c_str(),"",IO::Real,IO::Default), pos);

	//Set values
	int id = ioId(sid);
	io(id)->setName(cfg.cfg("NAME").getS());
	io(id)->setType((IO::Type)cfg.cfg("TYPE").getI());
	io(id)->setFlg(cfg.cfg("MODE").getI());
	io(id)->setDef(cfg.cfg("DEF").getS());
	io(id)->setHide(cfg.cfg("HIDE").getB());
    }
    //Remove holes
    for(unsigned i_p = 0; i_p < u_pos.size(); )
    {
	if(u_pos[i_p].empty()) { u_pos.erase(u_pos.begin()+i_p); continue; }
	i_p++;
    }
    //Position fixing
    for(int i_p = 0; i_p < (int)u_pos.size(); i_p++)
    {
	int iid = ioId(u_pos[i_p]);
	if(iid != i_p) try{ ioMove(iid,i_p); } catch(...){ }
    }
}

void Func::save_( )
{
    if(owner().DB().empty()) return;

    cfg("FORMULA").setNoTransl(!owner().progTr());
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(owner().fullDB(), mod->nodePath()+owner().tbl(), *this);

    //Save io config
    saveIO();
}

void Func::saveIO( )
{
    TConfig cfg(&mod->elFncIO());

    string io_bd = owner().fullDB()+"_io";
    string io_cfgpath = mod->nodePath()+owner().tbl()+"_io/";

    //Save allow IO
    cfg.cfg("F_ID").setS(id(), true);
    for(int i_io = 0; i_io < ioSize(); i_io++)
    {
	if(io(i_io)->flg()&Func::SysAttr) continue;
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

    //Clear IO
    cfg.cfgViewAll(false);
    for(int fld_cnt = 0; SYS->db().at().dataSeek(io_bd,io_cfgpath,fld_cnt++,cfg); )
	if(ioId(cfg.cfg("ID").getS()) < 0)
	{
	    SYS->db().at().dataDel(io_bd,io_cfgpath,cfg,true);
	    fld_cnt--;
	}
}

void Func::del( )
{
    if(!owner().DB().size()) return;

    SYS->db().at().dataDel(owner().fullDB(), mod->nodePath()+owner().tbl(), *this, true);

    //Delete io from DB
    delIO();
}

void Func::delIO( )
{
    TConfig cfg(&mod->elFncIO());
    cfg.cfg("F_ID").setS(id(), true);
    SYS->db().at().dataDel(owner().fullDB()+"_io", mod->nodePath()+owner().tbl()+"_io", cfg);
}

void Func::valAtt( TValFunc *vfnc )
{
    TFunction::valAtt(vfnc);
    workRegControl(vfnc);
}

void Func::valDet( TValFunc *vfnc )
{
    TFunction::valDet(vfnc);
    workRegControl(vfnc, true);
}

void Func::workRegControl( TValFunc *vfnc, bool toFree )
{
    if(vfnc->exCtx)
    {
	delete [] (RegW*)vfnc->exCtx;
	vfnc->exCtx = NULL;
    }
    if(!toFree && mRegs.size())
    {
	vfnc->exCtx = new RegW[mRegs.size()];
	RegW *reg = (RegW*)vfnc->exCtx;
	//Init list of registers
	for(unsigned i_rg = 0; i_rg < mRegs.size(); i_rg++)
	{
	    Reg *tR = mRegs[i_rg];
	    switch(tR->type())
	    {
		// Saved constants check
		case Reg::Bool:	  if(tR->lock() && tR->name().empty()) { reg[i_rg] = tR->val().b; reg[i_rg].setVConst(); }	break;
		case Reg::Int:	  if(tR->lock() && tR->name().empty()) { reg[i_rg] = tR->val().i; reg[i_rg].setVConst(); }	break;
		case Reg::Real:	  if(tR->lock() && tR->name().empty()) { reg[i_rg] = tR->val().r; reg[i_rg].setVConst(); }	break;
		case Reg::String: if(tR->lock() && tR->name().empty()) { reg[i_rg] = *tR->val().s;reg[i_rg].setVConst(); }	break;

		case Reg::Var:	  reg[i_rg].setType(Reg::Var); reg[i_rg].val().io = tR->val().io;	break;
		case Reg::PrmAttr:reg[i_rg].setType(Reg::PrmAttr); *reg[i_rg].val().pA = *tR->val().pA;	break;
		default: break;
	    }
	}
    }
}

void Func::setStart( bool val )
{
    if(val == run_st) return;
    //Start calc
    if(val)
    {
	progCompile( );
	run_st = true;
    }
    //Stop calc
    else
    {
	ResAlloc res(fRes(), true);
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
    ResAlloc res(parse_res, true);
    ResAlloc res1(fRes(), true);

    //Context clear for usings
    for(unsigned i = 0; i < used.size(); i++) used[i]->ctxClear();

    p_fnc  = this;	//Parse func
    p_err  = "";	//Clear error messages
    la_pos = 0;		//LA position
    sprg   = cfg("FORMULA").getS();
    prg.clear();	//Clear program
    regClear();		//Clear registers list
    regTmpClean( );	//Clear temporary registers list
    funcClear();	//Clear functions list

    if(yyparse())
    {
	prg.clear();
	sprg.clear();
	regClear();
	regTmpClean();
	funcClear();
	run_st = false;
	throw TError(nodePath().c_str(), "%s", p_err.c_str());
    }
    sprg.clear();
    regTmpClean();

    //Work registers update for calc contexts
    for(unsigned i = 0; i < used.size(); i++) workRegControl(used[i]);
}

int Func::funcGet( const string &path )
{
    string ns, f_path;

    //Check to correct function's path
    try
    {
	if(dynamic_cast<TFunction*>(&SYS->nodeAt(path,0,'.').at()))
	    f_path = SYS->nodeAt(path,0,'.').at().nodePath();
    }catch(...){ }

    if(f_path.empty())
    {
	for(int off = 0; !(ns=TSYS::strSepParse(mUsings,0,';',&off)).empty(); )
	    try{ if(dynamic_cast<TFunction*>(&SYS->nodeAt(ns+"."+path,0,'.').at())) break; }
	    catch(...){ continue; }
	if(ns.empty()) return -1;
	f_path = SYS->nodeAt(ns+"."+path,0,'.').at().nodePath();
    }

    //Search for already registered function
    for(int i_fnc = 0; i_fnc < (int)mFncs.size(); i_fnc++)
	if(f_path == mFncs[i_fnc]->func().at().nodePath())
	    return i_fnc;
    mFncs.push_back(new UFunc(ns.empty()?path:ns+"."+path));

    return mFncs.size()-1;
}

void Func::funcClear( )
{
    for(unsigned i_fnc = 0; i_fnc < mFncs.size(); i_fnc++)
	delete mFncs[i_fnc];
    mFncs.clear();
}

int Func::regNew( bool sep )
{
    //Get new register
    unsigned i_rg = mRegs.size();
    if(!sep)
	for(i_rg = 0; i_rg < mRegs.size(); i_rg++)
	    if(!mRegs[i_rg]->lock() && mRegs[i_rg]->type() == Reg::Free)
		break;
    if(i_rg >= mRegs.size()) mRegs.push_back(new Reg(i_rg));

    return i_rg;
}

int Func::regGet( const string &nm )
{
    //Check allow registers
    for(int i_rg = 0; i_rg < (int)mRegs.size(); i_rg++)
	if(mRegs[i_rg]->name() == nm)
	    return i_rg;
    return -1;
}

int Func::ioGet( const string &nm )
{
    int rez = -1;

    for(int i_io = 0; i_io < ioSize(); i_io++)
	if(io(i_io)->id() == nm)
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
    for(unsigned i_rg = 0; i_rg < mRegs.size(); i_rg++)
	delete mRegs[i_rg];
    mRegs.clear();
}

Reg *Func::regTmpNew( )
{
    unsigned i_rg;
    for(i_rg = 0; i_rg < mTmpRegs.size(); i_rg++)
	if(mTmpRegs[i_rg]->type() == Reg::Free)
	    break;
    if(i_rg >= mTmpRegs.size()) mTmpRegs.push_back(new Reg());
    return mTmpRegs[i_rg];
}

void Func::regTmpClean( )
{
    for(unsigned i_rg = 0; i_rg < mTmpRegs.size(); i_rg++)
	delete mTmpRegs[i_rg];
    mTmpRegs.clear();
}

Reg *Func::cdMvi( Reg *op, bool no_code )
{
    Reg *rez = NULL, *tR = NULL;
    if(op->pos() >= 0) return op;	//Already load

    //Check for place use and set standalone
    int i_rg = -1;
    switch(op->type())
    {
	case Reg::Bool:
	    for(i_rg = 0; i_rg < (int)mRegs.size(); i_rg++)
		if((tR=mRegs[i_rg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().b == op->val().b)
		    break;
	    break;
	case Reg::Int:
	    for(i_rg = 0; i_rg < (int)mRegs.size(); i_rg++)
		if((tR=mRegs[i_rg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().i == op->val().i)
		    break;
	    break;
	case Reg::Real:
	    for(i_rg = 0; i_rg < (int)mRegs.size(); i_rg++)
		if((tR=mRegs[i_rg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().r == op->val().r)
		    break;
	    break;
	case Reg::String:
	    for(i_rg = 0; i_rg < (int)mRegs.size(); i_rg++)
		if((tR=mRegs[i_rg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().s == op->val().s)
		    break;
	    break;
	default: break;
    }
    if(i_rg >= 0)
    {
	if(i_rg < (int)mRegs.size())
	{
	    rez = mRegs[i_rg];
#ifdef OSC_DEBUG
	    SYS->cntrIter("ReusedConstants",1);
#endif
	}
	else if(mRegs.size() < cnstStatLim())
	{
	    *(rez=regAt(p_fnc->regNew(true))) = *op;
	    rez->setLock(true);
	    op->free();
#ifdef OSC_DEBUG
	    SYS->cntrIter("FixConstants",1);
#endif
	}
	if(rez) return rez;
    }

    //Same mvi
    int r_id = p_fnc->regNew();
    *(rez=regAt(r_id)) = *op;
    op->free();
    if(no_code) return rez;
    uint16_t addr = rez->pos();

    switch(rez->type())
    {
	case Reg::Free: case Reg::Dynamic:
	    throw TError(nodePath().c_str(),_("Variable '%s' is used but undefined"),rez->name().c_str());
	case Reg::Bool:
	    prg += (uint8_t)Reg::MviB;
	    prg.append((char *)&addr, sizeof(uint16_t));
	    prg += (uint8_t)rez->val().b;
	    break;
	case Reg::Int:
	    prg += (uint8_t)Reg::MviI;
	    prg.append((char *)&addr,sizeof(uint16_t));
	    prg.append((char *)&rez->val().i, sizeof(int64_t));
	    break;
	case Reg::Real:
	    prg += (uint8_t)Reg::MviR;
	    prg.append((char *)&addr,sizeof(uint16_t));
	    prg.append((char *)&rez->val().r, sizeof(double));
	    break;
	case Reg::String:
	{
	    string sval = *rez->val().s;
	    //if(sval.size() > 255) throw TError(nodePath().c_str(),_("String constant size is more 255 symbols."));
	    prg += (uint8_t)Reg::MviS;
	    prg.append((char*)&addr, sizeof(uint16_t));
	    prg += (uint8_t)vmin(255, sval.size());
	    prg += sval.substr(0, vmin(255,sval.size()));
	    //> Load and append next parts for big string (>255)
	    for(unsigned i_chunk = 1; i_chunk < (sval.size()/255+((sval.size()%255)?1:0)); i_chunk++)
	    {
		Reg *treg = regTmpNew();
		*treg = sval.substr(i_chunk*255, vmin(255,(sval.size()-i_chunk*255)));
		rez = cdBinaryOp(Reg::Add, rez, treg);
	    }
	    break;
	}
	case Reg::Obj:
	    if(rez->name() == "SYS")
	    {
		prg += (uint8_t)Reg::MviSysObject;
		prg.append((char*)&addr, sizeof(uint16_t));
		prg += (char)0;
	    }
	    else if(rez->name() == "arguments")
	    {
		prg += (uint8_t)Reg::MviFuncArg;
		prg.append((char*)&addr, sizeof(uint16_t));
	    }
	    break;
	default: break;
    }
    return rez;
}

Reg *Func::cdMviObject( )
{
    //Make result
    Reg *rez = regAt(regNew());
    rez->setType(Reg::Obj);

    //Make code
    uint16_t addr;
    prg += (uint8_t)Reg::MviObject;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));

    return rez;
}

Reg *Func::cdMviArray( int p_cnt )
{
    if(p_cnt > 255) throw TError(nodePath().c_str(),_("Array have more 255 items."));
    deque<int> p_pos;

    //Mvi all parameters
    for(int i_prm = 0; i_prm < p_cnt; i_prm++)
	f_prmst[i_prm] = cdMvi(f_prmst[i_prm]);
    //Get parameters.
    for(int i_prm = 0; i_prm < p_cnt; i_prm++)
    {
	p_pos.push_front(f_prmst.front()->pos());
	f_prmst.front()->free();
	f_prmst.pop_front();
    }
    //Make result
    Reg *rez = regAt(regNew());
    rez->setType(Reg::Obj);

    //Make code
    uint16_t addr;
    prg += (uint8_t)Reg::MviArray;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    prg += (uint8_t)p_cnt;
    for(unsigned i_prm = 0; i_prm < p_pos.size(); i_prm++)
    { addr = p_pos[i_prm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdMviRegExp( int p_cnt )
{
    if(p_cnt < 1 || p_cnt > 2) throw TError(nodePath().c_str(),_("RegExp require one or two parameters."));

    Reg *rg_expr = NULL;
    Reg *rg_arg  = NULL;

    if(p_cnt == 2)
    {
	rg_arg = cdMvi(f_prmst.front());	f_prmst.pop_front();
	rg_expr = cdMvi(f_prmst.front());	f_prmst.pop_front();
    }
    else
    {
	rg_expr = cdMvi(f_prmst.front());	f_prmst.pop_front();
	rg_arg = regTmpNew();
	rg_arg->setType(Reg::String);
        rg_arg = cdMvi(rg_arg);
    }
    int p_expr = rg_expr->pos();
    int p_arg  = rg_arg->pos();
    rg_expr->free();
    rg_arg->free();

    //Make result
    Reg *rez = regAt(regNew());
    rez->setType(Reg::Obj);

    //Make code
    uint16_t addr;
    prg += (uint8_t)Reg::MviRegExp;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    addr = p_expr; prg.append((char*)&addr, sizeof(uint16_t));
    addr = p_arg; prg.append((char*)&addr, sizeof(uint16_t));

    return rez;
}

Reg *Func::cdTypeConv( Reg *op, Reg::Type tp, bool no_code )
{
    Reg *rez = op;
    if(rez->pos() < 0)
    {
	if(tp != rez->vType(this))
	    switch(tp)
	    {
		case Reg::Bool:
		    switch(rez->vType(this))
		    {
		        case Reg::Int: *rez = (char)((rez->val().i!=EVAL_INT) ? (bool)rez->val().i : (char)EVAL_BOOL);	break;
			case Reg::Real:*rez = (char)((rez->val().r!=EVAL_REAL) ? (bool)rez->val().r : (char)EVAL_BOOL);	break;
			case Reg::String: *rez = (char)((*rez->val().s!=EVAL_STR) ? (bool)atoi(rez->val().s->c_str()) : (char)EVAL_BOOL); break;
			default: break;
		    }
		    break;
		case Reg::Int:
		    switch(rez->vType(this))
		    {
			case Reg::Bool: *rez = (rez->val().b != EVAL_BOOL) ? (int64_t)rez->val().b : (int64_t)EVAL_INT;	break;
			case Reg::Real:	break;
			case Reg::String:*rez = (*rez->val().s != EVAL_STR) ? (int64_t)atoll(rez->val().s->c_str()) : (int64_t)EVAL_INT; break;
			default: break;
		    }
		    break;
		case Reg::Real:
		    switch(rez->vType(this))
		    {
			case Reg::Bool:	  *rez = (rez->val().b != EVAL_BOOL) ? (double)rez->val().b : EVAL_REAL;	break;
			case Reg::Int:    *rez = (rez->val().i != EVAL_INT) ? (double)rez->val().i : EVAL_REAL;		break;
			case Reg::String: *rez = (*rez->val().s != EVAL_STR) ? atof(rez->val().s->c_str()) : EVAL_REAL;	break;
			default: break;
		    }
		    break;
		case Reg::String:
		    switch(rez->vType(this))
		    {
			case Reg::Bool: *rez = (rez->val().b != EVAL_BOOL) ? i2s(rez->val().b) : EVAL_STR;	break;
			case Reg::Int:  *rez = (rez->val().i != EVAL_INT) ? ll2s(rez->val().i) : EVAL_STR;	break;
			case Reg::Real: *rez = (rez->val().r != EVAL_REAL) ? r2s(rez->val().r) : EVAL_STR;	break;
			default: break;
		    }
		    break;
		default: break;
	    }
	if(!no_code) rez = cdMvi(rez);
    }

    return rez;
}

void Func::cdAssign( Reg *rez, Reg *op )
{
    uint16_t addr;
    if(op->pos() < 0) op = cdMvi(op);
    prg += (uint8_t)Reg::Ass;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    addr = op->pos();  prg.append((char*)&addr, sizeof(uint16_t));

    op->free();		//Free temp operands
}

Reg *Func::cdMove( Reg *rez, Reg *op, bool force )
{
    if(!force && !op->lock()) return op;

    uint16_t addr;
    Reg *rez_n = rez;
    op = cdMvi(op);
    if(rez_n == NULL) rez_n = regAt(regNew());
    rez_n = cdMvi(rez_n, true);
    rez_n->setType(op->vType(this));
    prg += (uint8_t)Reg::Mov;
    addr = rez_n->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    addr = op->pos();    prg.append((char*)&addr, sizeof(uint16_t));

    op->free();		//Free temp operands

    return rez_n;
}

Reg *Func::cdBinaryOp( Reg::Code cod, Reg *op1, Reg *op2 )
{
    //Check allow the buildin calc and calc
    if(op1->pos() < 0 && op2->pos() < 0)
    {
	switch(cod)
	{
	    case Reg::Add: case Reg::Sub: case Reg::Mul:
	    case Reg::Div: case Reg::LT: case Reg::GT:
	    case Reg::LEQ: case Reg::GEQ: case Reg::EQU: case Reg::NEQ:
		if(op1->vType(this) != Reg::String)
		    op1 = cdTypeConv(op1, Reg::Real, true);
		break;
	    default: break;
	}

	op2 = cdTypeConv( op2, op1->vType(this), true);
	switch(op1->vType(this))
	{
	    case Reg::Int:
		switch(cod)
		{
		    case Reg::RstI:	*op1 = op1->val().i % op2->val().i;	break;
		    case Reg::BitOr:	*op1 = op1->val().i | op2->val().i;	break;
		    case Reg::BitAnd:	*op1 = op1->val().i & op2->val().i;	break;
		    case Reg::BitXor:	*op1 = op1->val().i ^ op2->val().i;	break;
		    case Reg::BitShLeft:*op1 = op1->val().i << op2->val().i;	break;
		    case Reg::BitShRight:*op1 = op1->val().i >> op2->val().i;	break;
		    case Reg::LOr:	*op1 = op1->val().i || op2->val().i;	break;
		    case Reg::LAnd:	*op1 = op1->val().i && op2->val().i;	break;
		    default: break;
		}
		break;
	    case Reg::Real:
		switch(cod)
		{
		    case Reg::Add:	*op1 = op1->val().r + op2->val().r;	break;
		    case Reg::Sub:	*op1 = op1->val().r - op2->val().r;	break;
		    case Reg::Mul:	*op1 = op1->val().r * op2->val().r;	break;
		    case Reg::Div:	*op1 = op1->val().r / op2->val().r;	break;
		    case Reg::BitOr:	*op1 = (int64_t)op1->val().r | (int64_t)op2->val().r;	break;
		    case Reg::BitAnd:	*op1 = (int64_t)op1->val().r & (int64_t)op2->val().r;	break;
		    case Reg::BitXor:	*op1 = (int64_t)op1->val().i ^ (int64_t)op2->val().i;	break;
		    case Reg::BitShLeft:*op1 = (int64_t)op1->val().i << (int64_t)op2->val().i;	break;
		    case Reg::BitShRight:*op1 = (int64_t)op1->val().i >> (int64_t)op2->val().i;	break;
		    case Reg::LOr:	*op1 = op1->val().r || op2->val().r;	break;
		    case Reg::LAnd:	*op1 = op1->val().r && op2->val().r;	break;
		    case Reg::LT:	*op1 = op1->val().r < op2->val().r;	break;
		    case Reg::GT:	*op1 = op1->val().r > op2->val().r;	break;
		    case Reg::LEQ:	*op1 = op1->val().r <= op2->val().r;	break;
		    case Reg::GEQ:	*op1 = op1->val().r >= op2->val().r;	break;
		    case Reg::EQU:	*op1 = op1->val().r == op2->val().r;	break;
		    case Reg::NEQ:	*op1 = op1->val().r != op2->val().r;	break;
		    default: break;
		}
		break;
	    case Reg::Bool:
		switch(cod)
		{
		    case Reg::RstI:	*op1 = op1->val().b % op2->val().b;	break;
		    case Reg::BitOr:	*op1 = op1->val().b | op2->val().b;	break;
		    case Reg::BitAnd:	*op1 = op1->val().b & op2->val().b;	break;
		    case Reg::BitXor:	*op1 = op1->val().b ^ op2->val().b;	break;
		    case Reg::LOr:	*op1 = op1->val().b || op2->val().b;	break;
		    case Reg::LAnd:	*op1 = op1->val().b && op2->val().b;	break;
		    default: break;
		}
	    case Reg::String:
		switch(cod)
		{
		    case Reg::Add:	*op1->val().s += *op2->val().s;		break;
		    case Reg::EQU:	*op1 = (char)(*op1->val().s == *op2->val().s);	break;
		    case Reg::NEQ:	*op1 = (char)(*op1->val().s != *op2->val().s);	break;
		    default: break;
		}
		break;
	    default: break;
	}

	return op1;
    }

    //Make operation cod
    // Prepare operands
    op1 = cdMvi(op1);
    Reg::Type op1_tp = op1->vType(this);
    Reg::Type rez_tp = op1->objEl() ? Reg::Dynamic : op1_tp;
    int op1_pos = op1->pos();
    if(op1_tp != Reg::Dynamic) op2 = cdTypeConv(op2, op1_tp);
    else if(op2->pos() < 0) op2 = cdMvi(op2);
    int op2_pos = op2->pos();
    // Prepare rezult
    Reg *rez = regAt(regNew());
    rez->setType(rez_tp);
    //!!!! Free operands after alloc rezult for prevent operations from self by some problems with object
    op1->free();
    op2->free();
    // Add code
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
	case Reg::BitShRight:	prg += (uint8_t)Reg::BitShRight;rez->setType(Reg::Int);	break;
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
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    addr = op1_pos;    prg.append((char*)&addr, sizeof(uint16_t));
    addr = op2_pos;    prg.append((char*)&addr, sizeof(uint16_t));

    return rez;
}

Reg *Func::cdCondBinaryOp( int p_cmd, Reg *op1, Reg *op2, int p_end )
{
    //Mvi cond-op1 register (insert to program)
    string cd_tmp = prg.substr(p_cmd);
    prg.erase(p_cmd);
    op1 = cdMvi(op1);
    p_end += prg.size()-p_cmd;
    p_cmd = prg.size();
    prg += cd_tmp;
    //Mvi op2 register (insert to program)
    cd_tmp = prg.substr(p_end-1);   //-1 pass end command
    prg.erase(p_end-1);
    op2 = cdMvi(op2);
    p_end = prg.size()+1;
    prg += cd_tmp;

    //Make operation code
    p_end -= p_cmd;
    Reg::Type op1_tp = op1->vType(this);
    Reg::Type rez_tp = op1->objEl() ? Reg::Dynamic : op1_tp;
    int op1_pos = op1->pos();
    int op2_pos = op2->pos();
    op1->free();
    op2->free();

    // Prepare rezult
    Reg *rez = regAt(regNew());
    rez->setType(rez_tp);

    //[CRRrrRRnn]
    int a_sz = sizeof(uint16_t);
    uint16_t addr = rez->pos();	prg.replace(p_cmd+1, sizeof(uint16_t), (char*)&addr, sizeof(uint16_t));
    prg.replace(p_cmd+3, sizeof(uint16_t), (char*)&op1_pos, sizeof(uint16_t));
    prg.replace(p_cmd+5, sizeof(uint16_t), (char*)&op2_pos, sizeof(uint16_t));
    prg.replace(p_cmd+5+a_sz, a_sz, ((char *)&p_end), a_sz);

    return rez;
}

Reg *Func::cdUnaryOp( Reg::Code cod, Reg *op )
{
    //Check allow the buildin calc and calc
    if(op->pos() < 0)
    {
	switch(op->vType(this))
	{
	    case Reg::Int:
		switch(cod)
		{
		    case Reg::Not:	*op = !op->val().i;	break;
		    case Reg::BitNot:	*op = ~op->val().i;	break;
		    case Reg::Neg:	*op = -op->val().i;	break;
		    default: break;
		}
		break;
	    case Reg::Obj:
	    case Reg::Real:
		switch(cod)
		{
		    case Reg::Not:	*op = !op->val().r;	break;
		    case Reg::BitNot:	*op = ~(int)op->val().r;break;
		    case Reg::Neg:	*op = -op->val().r;	break;
		    default: break;
		}
		break;
	    case Reg::Bool:
		switch(cod)
		{
		    case Reg::Not:	*op = !op->val().b;	break;
		    case Reg::BitNot:	*op = ~op->val().b;	break;
		    case Reg::Neg:	*op = -op->val().b;	break;
		    default: break;
		}
	    default: break;
	}
	return op;
    }

    //Make operation cod
    // Prepare operand
    op = cdMvi(op);
    Reg::Type op_tp = op->vType(this);
    int op_pos = op->pos();
    op->free();
    // Prepare rezult
    Reg *rez = regAt(regNew());
    rez->setType(op_tp);
    // Add code
    switch(cod)
    {
	case Reg::Not:		prg += (uint8_t)Reg::Not;	break;
	case Reg::BitNot:	prg += (uint8_t)Reg::BitNot;	break;
	case Reg::Neg:		prg += (uint8_t)Reg::Neg;	break;
	default: throw TError(nodePath().c_str(),_("Don't support operation code %d."),cod);
    }
    uint16_t addr;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    addr = op_pos;     prg.append((char*)&addr, sizeof(uint16_t));

    return rez;
}

Reg *Func::cdCond( Reg *cond, int p_cmd, int p_else, int p_end, Reg *thn, Reg *els )
{
    Reg *rez = NULL;
    int a_sz = sizeof(uint16_t);
    string cd_tmp;

    //Mvi cond register (insert to program)
    cd_tmp = prg.substr(p_cmd);
    prg.erase(p_cmd);
    cond = cdMvi(cond);
    p_else += prg.size()-p_cmd;
    p_end += prg.size()-p_cmd;
    p_cmd = prg.size();
    prg += cd_tmp;
    uint16_t p_cond = cond->pos(); cond->free();

    if(thn != NULL && els != NULL)
    {
	//Add Move command to "then" end (insert to program)
	cd_tmp = prg.substr(p_else-1);	//-1 pass end command
	prg.erase(p_else-1);
	thn = cdMvi(thn);
	rez = cdMove(NULL, thn);
	p_end += prg.size()-p_else+1;
	p_else = prg.size()+1;
	prg += cd_tmp;
	//Add Move command to "else" end (insert to program)
	cd_tmp = prg.substr(p_end-1);   //-1 pass end command
	prg.erase(p_end-1);
	els = cdMvi(els);
	cdMove(rez, els);
	p_end = prg.size()+1;
	prg += cd_tmp;
    }

    //Make apropos adress
    p_else -= p_cmd;
    p_end  -= p_cmd;

    //[CRR00nn]
    prg.replace(p_cmd+1, sizeof(uint16_t), (char*)&p_cond, sizeof(uint16_t));
    prg.replace(p_cmd+3, a_sz, ((char *)&p_else), a_sz);
    prg.replace(p_cmd+3+a_sz, a_sz, ((char *)&p_end), a_sz);

    return rez;
}

void Func::cdCycle( int p_cmd, Reg *cond, int p_solve, int p_end, int p_postiter )
{
    int a_sz = sizeof(uint16_t);
    string cd_tmp;
    int p_body = (p_postiter?p_postiter:p_solve)-1;	//Include Reg::End command

    //> Mvi cond register (insert to program)
    cd_tmp = prg.substr(p_body);
    prg.erase(p_body);
    cond = cdMvi(cond);
    p_solve += prg.size()-p_body;
    p_end += prg.size()-p_body;
    if(p_postiter) p_postiter += prg.size()-p_body;
    prg += cd_tmp;
    uint16_t p_cond = cond->pos();
    cond->free();

    //> Make apropos adress
    p_solve -= p_cmd;
    p_end   -= p_cmd;
    if(p_postiter) p_postiter -= p_cmd;

    //> [CRRbbaann]
    prg.replace(p_cmd+1, sizeof(uint16_t), (char*)&p_cond, sizeof(uint16_t));
    prg.replace(p_cmd+3, a_sz, ((char*)&p_solve), a_sz);
    prg.replace(p_cmd+3+a_sz, a_sz, ((char*)&p_postiter), a_sz);
    prg.replace(p_cmd+3+2*a_sz, a_sz, ((char*)&p_end), a_sz);
}

void Func::cdCycleObj( int p_cmd, Reg *cond, int p_solve, int p_end, Reg *var )
{
    int p_body = p_solve-1;	//Include Reg::End command

    var = cdMvi(var);
    uint16_t p_var = var->pos();
    var->free();

    //> Mvi cond register (insert to program)
    string cd_tmp = prg.substr(p_body);
    prg.erase(p_body);
    cond = cdMvi(cond);
    p_solve += prg.size()-p_body;
    p_end += prg.size()-p_body;
    prg += cd_tmp;
    uint16_t p_cond = cond->pos();
    cond->free();

    //> Make apropos adress
    p_solve -= p_cmd;
    p_end   -= p_cmd;

    //> [COObbRRnn]
    prg[p_cmd] = (uint8_t)Reg::CycleObj;
    prg.replace(p_cmd+1, sizeof(uint16_t), (char*)&p_cond, sizeof(uint16_t));
    prg.replace(p_cmd+3, sizeof(uint16_t), ((char*)&p_solve), sizeof(uint16_t));
    prg.replace(p_cmd+3+sizeof(uint16_t), sizeof(uint16_t), (char*)&p_var, sizeof(uint16_t));
    prg.replace(p_cmd+3+2*sizeof(uint16_t), sizeof(uint16_t), ((char*)&p_end), sizeof(uint16_t));
}

Reg *Func::cdBldFnc( int f_cod, Reg *prm1, Reg *prm2 )
{
    Reg *rez;
    uint16_t addr;
    int p1_pos = -1, p2_pos = -1;

    //if( (prm1 && !prm1->objEl() && prm1->vType(this) == Reg::String) ||
    //	(prm2 && !prm2->objEl() && prm2->vType(this) == Reg::String) )
    //	throw TError(nodePath().c_str(),_("Buildin functions don't support string type"));
    //> Free parameter's registers
    if(prm1)	{ prm1 = cdMvi(prm1); p1_pos = prm1->pos(); }
    if(prm2)	{ prm2 = cdMvi(prm2); p2_pos = prm2->pos(); }
    if(prm1)	prm1->free();
    if(prm2)	prm2->free();
    //> Get rezult register
    rez = regAt(regNew());
    rez->setType(Reg::Dynamic/*Reg::Real*/);
    //> Make code
    prg += (uint8_t)f_cod;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    if(p1_pos >= 0) { addr = p1_pos; prg.append((char*)&addr, sizeof(uint16_t)); }
    if(p2_pos >= 0) { addr = p2_pos; prg.append((char*)&addr, sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdExtFnc( int f_id, int p_cnt, bool proc )
{
    int r_pos;	//Return function's position
    Reg *rez = NULL;
    deque<int> p_pos;

    //Check return IO position
    bool ret_ok = false;
    for(r_pos = 0; r_pos < funcAt(f_id)->func().at().ioSize(); r_pos++)
	if((ret_ok=(bool)(funcAt(f_id)->func().at().io(r_pos)->flg()&IO::Return)))
	    break;

    //Check IO and parameters count
    if(p_cnt > funcAt(f_id)->func().at().ioSize()-ret_ok)
	throw TError(nodePath().c_str(), _("More than %d(%d) parameters are specified for function '%s'"),
	    (funcAt(f_id)->func().at().ioSize()-ret_ok), p_cnt, funcAt(f_id)->func().at().id().c_str());

    //Check the present return for fuction
    if(!proc && !ret_ok)
	throw TError(nodePath().c_str(), _("Function is requested '%s', but it doesn't have return of IO"), funcAt(f_id)->func().at().id().c_str());

    //Mvi all parameters
    for(int i_prm = 0; i_prm < p_cnt; i_prm++) f_prmst[i_prm] = cdMvi(f_prmst[i_prm]);

    //Get parameters. Add check parameters type !!!!
    for(int i_prm = 0; i_prm < p_cnt; i_prm++)
    {
	p_pos.push_front(f_prmst.front()->pos());
	f_prmst.front()->free();
	f_prmst.pop_front();
    }

    //Make result
    if(!proc)
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

    //Make code
    uint16_t addr;
    prg += proc ? (uint8_t)Reg::CProc : (uint8_t)Reg::CFunc;
    prg += (uint8_t)f_id;
    prg += (uint8_t)p_cnt;
    addr = proc ? 0 : rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    for(unsigned i_prm = 0; i_prm < p_pos.size(); i_prm++)
    { addr = p_pos[i_prm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdObjFnc( Reg *obj, int p_cnt )
{
    if(!obj->objEl())	throw TError(nodePath().c_str(), _("No object variable for function"));
    if(p_cnt > 255)	throw TError(nodePath().c_str(), _("Object's function have more 255 parameters."));

    Reg *rez = NULL;
    deque<int> p_pos;

    //> Mvi all parameters
    for(int i_prm = 0; i_prm < p_cnt; i_prm++) f_prmst[i_prm] = cdMvi(f_prmst[i_prm]);
    //> Get parameters
    for(int i_prm = 0; i_prm < p_cnt; i_prm++)
    {
	p_pos.push_front(f_prmst.front()->pos());
	f_prmst.front()->free();
	f_prmst.pop_front();
    }
    obj->free();
    rez = regAt(regNew());
    rez->setType(Reg::Dynamic);

    //> Make code
    uint16_t addr;
    prg += (uint8_t)Reg::CFuncObj;
    addr = obj->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    prg += (uint8_t)p_cnt;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));

    for(unsigned i_prm = 0; i_prm < p_pos.size(); i_prm++)
    { addr = p_pos[i_prm]; prg.append((char*)&addr, sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdProp( Reg *obj, const string &sprp, Reg *dprp )
{
    uint16_t addr;
    Reg *ro = obj;
    if(!ro->objEl()) { ro = cdMove(NULL, cdMvi(ro), false); ro->setObjEl(); }

    if(!dprp)
    {
	prg += (uint8_t)Reg::OPrpSt;
	addr = ro->pos(); prg.append((char*)&addr, sizeof(uint16_t));
	prg += (uint8_t)sprp.size();
	prg += sprp;
    }
    else
    {
	dprp = cdMvi(dprp);

	prg += (uint8_t)Reg::OPrpDin;
	addr = ro->pos(); prg.append((char*)&addr, sizeof(uint16_t));
	addr = dprp->pos(); prg.append((char*)&addr, sizeof(uint16_t));

	dprp->free();
    }
    return ro;
}

TVariant Func::oPropGet( TVariant vl, const string &prop )
{
    switch(vl.type())
    {
	case TVariant::Object:		return vl.getO().at().propGet(prop);
	case TVariant::Boolean:		return TVariant();
	case TVariant::Integer:
	    if(prop == "MAX_VALUE")	return (int64_t) 9223372036854775807LL;
	    if(prop == "MIN_VALUE")	return (int64_t)-9223372036854775807LL;
	    if(prop == "NaN")		return (int64_t)EVAL_INT;
	    return TVariant();
	case TVariant::Real:
	    if(prop == "MAX_VALUE")	return  3.4e300;
	    if(prop == "MIN_VALUE")	return -3.4e300;
	    if(prop == "NaN")		return EVAL_REAL;
	    return TVariant();
	case TVariant::String:
	{
	    if(prop == "length")	return (int64_t)vl.getS().size();
	    int sid = atoi(prop.c_str());
	    if(sid < 0 || sid >= (int)vl.getS().size()) return "";
	    return vl.getS().substr(sid,1);
	}
	default: return TVariant();
    }
    return TVariant();
}

TVariant Func::oFuncCall( TVariant &vl, const string &prop, vector<TVariant> &prms )
{
    try
    {
	switch(vl.type())
	{
	    case TVariant::Null:
		if(prop == "isEVal")	return true;
		return false;
	    case TVariant::Object:	return vl.getO().at().funcCall(prop, prms);
	    case TVariant::Boolean:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal")	return (vl.getB() == EVAL_BOOL);
		// string toString( ) - performs the value as the string “true” or “false”
		if(prop == "toString")	return string(vl.getB() ? "true" : "false");
		return false;
		//throw TError(nodePath().c_str(),_("Boolean type have not function '%s' or not enough parameters for it."),prop.c_str());
	    case TVariant::Integer:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal")	return (vl.getI() == EVAL_INT);
	    case TVariant::Real:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal")	return (vl.getR() == EVAL_REAL);
		// string toExponential(int numbs = -1) - return the string of the number, formatted in exponential notation, 
		//      and with the number of significant digits <numbs>
		//  numbs - number of significant digits, if is missing the number of digits will have as much as needed
		if(prop == "toExponential")
		{
		    int n = prms.size() ? vmax(0,vmin(20,prms[0].getI())) : -1;
		    if(n < 0) return TSYS::strMess("%e", vl.getR());
		    return TSYS::strMess("%.*e", n, vl.getR());
		}
		// string toFixed(int numbs = 0, int len = 0, bool sign = false) - return the string of the number, formatted in the notation of fixed-point,
		//      and with the number of significant digits after the decimal point <numbs>
		//  numbs - the number of significant digits after the decimal point, if <numbs> is missing the number
		//          of digits after the decimal point is equal to zero
		//  len - minimum string number length (fill by 0);
		//  sign - strong sign present flag.
		if(prop == "toFixed")
		{
		    double val = vl.getR();
		    bool sign = (val < 0 || (prms.size() >= 3 && prms[2].getB()));
		    int n = prms.size() ? vmax(0,vmin(20,prms[0].getI())) : 0;
		    int w = (prms.size() >= 2) ? vmin(100,prms[1].getI()) : 0;
		    return TSYS::strMess((sign?"%+0*.*f":"%0*.*f"),w,n,val);
		}
		// string toPrecision(int prec = -1) - return the string of the formatted number with the number of significant digits <prec>
		//  prec - number of significant digits
		if(prop == "toPrecision")
		{
		    int n = prms.size() ? vmax(1,vmin(21,prms[0].getI())) : -1;
		    if(n < 0)	return TSYS::strMess("%g",vl.getR());
		    return TSYS::strMess("%.*g",n,vl.getR());
		}
		// string toString(int base = 10, int len = -1, bool sign = false) - return the string of the formatted number of integer type for
		//	minimum length <len> and strong sign present <sign>.
		//  base - representation base (2-36);
		//  len - minimum string number length (fill by 0);
		//  sign - strong sign present flag.
		if(prop == "toString")
		{
		    const char *dsymb = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		    int n = 10, w = -1, val = vl.getI();
		    bool sign = (val < 0 || (prms.size() >= 3 && prms[2].getB()));
		    if(prms.size()) n = vmax(2,vmin(36,prms[0].getI()));
		    if(prms.size() >= 2) w = vmin(100,prms[1].getI()) - (int)sign;
		    string rez;
		    for(unsigned c_vl = abs(val); c_vl || rez.empty() || (w > 0 && (int)rez.size() < w); c_vl = c_vl/n)
			rez += dsymb[c_vl%n];
		    if(val < 0 || sign) rez += (val >= 0) ? "+" : "-";
		    return TSYS::strEncode(rez,TSYS::Reverse);
		}
		return false;
		//throw TError(nodePath().c_str(),_("Integer or real type have not function '%s' or not enough parameters for it."),prop.c_str());
	    case TVariant::String:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal")	return (vl.getS() == EVAL_STR);
		// string charAt(int symb) - extracts from the string the symbol <symb>
		//  symb - symbol position
		if(prop == "charAt" && prms.size())
		{
		    int n = prms[0].getI();
		    if(n < 0 || n >= (int)vl.getS().size()) return string("");
		    return vl.getS().substr(n,1);
		}
		// int charCodeAt(int symb) - extracts from the string the symbol code <symb>
		//  symb - symbol position
		if(prop == "charCodeAt" && prms.size())
		{
		    int n = prms[0].getI();
		    if(n < 0 || n >= (int)vl.getS().size())	return (int64_t)EVAL_INT;
		    return (int64_t)(unsigned char)vl.getS()[n];
		}
		// string concat(string val1, string val2, ...) - returns a new string formed by joining the values <val1> etc
		//  val1, val2 - appended values
		if(prop == "concat" && prms.size())
		{
		    string rez = vl.getS();
		    for(unsigned i_p = 0; i_p < prms.size(); i_p++)
			rez += prms[i_p].getS();
		    return rez;
		}
		// int indexOf(string substr, int start) - returns the position of the required string <substr> in the original
		//       row from the position <start>
		//  substr - requested substring value
		//  start - start position for search
		if(prop == "indexOf" && prms.size())
		{
		    size_t sp = 0;
		    if(prms.size() > 1) sp = vmax(0,vmin(vl.getS().size()-1,(unsigned)prms[1].getI()));
		    sp = vl.getS().find(prms[0].getS(),sp);
		    return (sp==string::npos) ? -1 : (int)sp;
		}
		// int lastIndexOf(string substr, int start) - returns the position of the search string <substr> in the original
		//       one beginning from the position of <start> when searching from the end
		//  substr - requested substring value
		//  start - start position for search from end
		if(prop == "lastIndexOf" && prms.size())
		{
		    size_t sp = string::npos;
		    if(prms.size() > 1) sp = vmax(0,vmin(vl.getS().size()-1,(unsigned)prms[1].getI()));
		    sp = vl.getS().rfind(prms[0].getS(),sp);
		    return (sp==string::npos) ? -1 : (int)sp;
		}
		// int search( string pat, string flg = "" ); - search into the string by pattern 'pat' and flags 'flg'.
		//       Return found substring position or -1 for else.
		// int search( RegExp pat ); - search into the string by RegExp 'pat'.
		//       Return found substring position or -1 for else.
		//  pat - regular expression pattern;
		//  flg - regular expression flags.
		if(prop == "search" && prms.size())
		{
		    AutoHD<TRegExp> re;
		    if(prms[0].type() == TVariant::String) re = new TRegExp(prms[0].getS(),(prms.size()>=2)?prms[1].getS():string());
		    else if(prms[0].type() != TVariant::Object || (re=prms[0].getO()).freeStat()) return -1;
		    return re.at().search(vl.getS());
		}
		// Array match( string pat, string flg = "" ); - call match for the string by pattern 'pat' and flags 'flg'.
		//       Return matched substring (0) and subexpressions (>0) array.
		//       Set 'index' attribute of the array to substring position. Set 'input' attribute to source string.
		// Array match( TRegExp pat ); - call match for the string and RegExp pattern 'pat'.
		//       Return matched substring (0) and subexpressions (>0) array.
		//       Set 'index' attribute of the array to substring position. Set 'input' attribute to source string.
		//  pat - regular expression pattern;
		//  flg - regular expression flags.
		if(prop == "match" && prms.size())
		{
		    AutoHD<TRegExp> re;
		    if(prms[0].type() == TVariant::String) re = new TRegExp(prms[0].getS(),(prms.size()>=2)?prms[1].getS():string());
		    else if(prms[0].type() != TVariant::Object || (re=prms[0].getO()).freeStat()) return -1;
		    return re.at().match(vl.getS(),true);
		}
		// string slice(int beg, int end) - return the string extracted from the original one starting from the <beg> position
		//       and ending be the <end>
		//  beg - begin position
		//  end - end position
		if((prop == "slice" || prop == "substring") && prms.size())
		{
		    int beg = prms[0].getI();
		    if(beg < 0) beg = vl.getS().size()+beg;
		    beg = vmax(beg,0);
		    int end = vl.getS().size();
		    if(prms.size() >= 2) end = prms[1].getI();
		    if(end < 0) end = vl.getS().size()+end;
		    end = vmin(end,(int)vl.getS().size());
		    if(beg >= end) return string("");
		    return vl.getS().substr(beg,end-beg);
		}
		// Array split(string sep, int limit) - return the array of strings separated by <sep> with the limit of the number of elements <limit>.
		//  sep - items separator
		//  limit - items limit
		// Array split(RegExp pat, int limit) - return the array of strings separated by RegExp pattern <pat> with the limit of the number of elements <limit>.
		//  pat - regular expression pattern.
		if(prop == "split" && prms.size())
		{
		    //> Use RegExp for split
		    if(prms[0].type() == TVariant::Object && !AutoHD<TRegExp>(prms[0].getO()).freeStat())
			return AutoHD<TRegExp>(prms[0].getO()).at().split(vl.getS(),(prms.size()>=2)?prms[1].getI():0);

		    //> Use simple string separator
		    TArrayObj *rez = new TArrayObj();
		    for(size_t posB = 0, posC, i_p = 0; true; i_p++)
		    {
			if(prms.size() > 1 && rez->size() >= prms[1].getI()) break;
			posC = vl.getS().find(prms[0].getS(),posB);
			if(posC != posB)
			    rez->arSet(i_p, vl.getS().substr(posB,posC-posB));
			if(posC == string::npos) break;
			posB = posC + prms[0].getS().size();
		    }
		    return rez;
		}
		// string insert(int pos, string substr) - insert substring <substr> into this string's position <pos>
		//  pos - position for insert
		//  substr - substring for insert
		if(prop == "insert" && prms.size() >= 2)
		    return vl.getS().insert(vmax(0,vmin(vl.getS().size(),(unsigned)prms[0].getI())), prms[1].getS() );
		// string replace(int pos, int n, string str) - replace substring into position <pos> and length <n> to string <str>.
		//  pos - position for start replace
		//  n - number symbols for replace
		//  str - string for replace
		// string replace(string substr, string str) - replace all substrings <substr> to string <str>.
		//  substr - substring into all string
		// string replace(RegExp pat, string str) - replace substrings by pattern <pat> to string <str>.
		//  pat - regular expression pattern
		if(prop == "replace" && prms.size() >= 2)
		{
		    string cstr = vl.getS();
		    //> Replace simple substrings by other string
		    if(prms[0].type() == TVariant::String && prms[1].type() == TVariant::String)
			for(size_t cpos = 0; (cpos=cstr.find(prms[0].getS(),cpos)) != string::npos; cpos += prms[1].getS().size())
			    cstr.replace(cpos,prms[0].getS().size(),prms[1].getS());
		    //> Replace substrings by RegExp patern by other string
		    else if(prms[0].type() == TVariant::Object && !AutoHD<TRegExp>(prms[0].getO()).freeStat())
			cstr = AutoHD<TRegExp>(prms[0].getO()).at().replace(cstr,prms[1].getS());
		    //> Simple - direct replace
		    if(prms.size() >= 3 && prms[2].type() == TVariant::String)
		    {
			int pos = prms[0].getI();
			if(pos < 0 || pos >= (int)cstr.size()) return vl;
			int n = prms[1].getI();
			if(n < 0) n = cstr.size();
			n = vmin((int)cstr.size()-pos,n);
			cstr.replace(pos, n, prms[2].getS());
		    }
		    //vl.setS(cstr);
		    //vl.setModify();
		    return cstr;
		}
		// real toReal() - convert this string to real number
		if(prop == "toReal") return atof(vl.getS().c_str());
		// int toInt(int base = 0) - convert this string to integer number
		//  base - radix of subject sequence
		if(prop == "toInt") return (int)strtol(vl.getS().c_str(),NULL,(prms.size()>=1?prms[0].getI():0));
		// string parse(int pos, string sep = ".", int off = 0) - get token with numbet <pos> from the string when separated by <sep>
		//       and from offset <off>
		//  pos - item position
		//  sep - items separator
		//  off - start position
		if(prop == "parse" && prms.size())
		{
		    int off = (prms.size() >= 3) ? prms[2].getI() : 0;
		    string rez = TSYS::strParse( vl.getS(), prms[0].getI(),
			(prms.size()>=2) ? prms[1].getS() : ".", &off, (prms.size()>=4) ? prms[3].getB() : false );
		    if( prms.size() >= 3 ) { prms[2].setI(off); prms[2].setModify(); }
		    return rez;
		}
		// string parsePath(int pos, int off = 0) - get path token with numbet <pos> from the string and from offset <off>
		//  pos - item position
		//  off - start position
		if(prop == "parsePath" && prms.size())
		{
		    int off = (prms.size() >= 2) ? prms[1].getI() : 0;
		    string rez = TSYS::pathLev(vl.getS(), prms[0].getI(), true, &off);
		    if(prms.size() >= 2) { prms[1].setI(off); prms[1].setModify(); }
		    return rez;
		}
		// string path2sep(string sep = ".") - convert path into this string to separated by <sep> string.
		//  sep - item separator
		if(prop == "path2sep")
		    return TSYS::path2sepstr(vl.getS(), (prms.size() && prms[0].getS().size()) ? prms[0].getS()[0] : '.');

		return false;
		//throw TError(nodePath().c_str(),_("String type have not properties '%s' or not enough parameters for it."),prop.c_str());
	    default:	break;
	}
	return false;
	//throw TError(nodePath().c_str(),_("Unknown type '%d' for property '%s'."),vl.type(),prop.c_str());
    }
    catch(TError err)
    {
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%s", err.mess.c_str());
    }
    return false;
}

TVariant Func::getVal( TValFunc *io, RegW &rg, bool fObj )
{
    TVariant vl(EVAL_REAL);

    //> Get base value
    switch(rg.type())
    {
	case Reg::Bool:	  vl = rg.val().b;	break;
	case Reg::Int:	  vl = rg.val().i;	break;
	case Reg::Real:	  vl = rg.val().r;	break;
	case Reg::String: vl = *rg.val().s;	break;
	case Reg::Var:
	    switch(io->ioType(rg.val().io))
	    {
		case IO::Boolean:	vl = io->getB(rg.val().io);	break;
		case IO::Integer:	vl = io->getI(rg.val().io);	break;
		case IO::Real:		vl = io->getR(rg.val().io);	break;
		case IO::String:	vl = io->getS(rg.val().io);	break;
		case IO::Object:	vl = io->getO(rg.val().io);	break;
		default: vl = EVAL_BOOL;
	    }
	    break;
	case Reg::PrmAttr:
	    switch(rg.val().pA->at().fld().type())
	    {
		case TFld::Boolean:	vl = rg.val().pA->at().getB();	break;
		case TFld::Integer:	vl = rg.val().pA->at().getI();	break;
		case TFld::Real:	vl = rg.val().pA->at().getR();	break;
		case TFld::String:	vl = rg.val().pA->at().getS();	break;
		case TFld::Object:	vl = rg.val().pA->at().getO();	break;
		default: vl = EVAL_BOOL;
	    }
	    break;
	case Reg::Obj:	vl = *rg.val().o;	break;
	default: break;
    }

    for(unsigned i_p = 0; i_p < rg.props().size(); i_p++)
    {
	if(fObj && i_p == (rg.props().size()-1)) break;
	if(vl.isNull())	return TVariant();	//Null //return false;
	vl = oPropGet(vl, rg.props()[i_p]);
    }

    return vl;
}

string Func::getValS( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Bool:	return (rg.val().b != EVAL_BOOL) ? i2s((bool)rg.val().b) : EVAL_STR;
	    case Reg::Int:	return (rg.val().i != EVAL_INT) ? ll2s(rg.val().i) : EVAL_STR;
	    case Reg::Real:	return (rg.val().r != EVAL_REAL) ? r2s(rg.val().r) : EVAL_STR;
	    case Reg::String:	return *rg.val().s;
	    case Reg::Var:	return io->getS(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getS();
	    case Reg::Obj:	return rg.val().o->at().getStrXML();
	    default:	break;
	}
    else return getVal(io,rg).getS();

    return EVAL_STR;
}

int64_t Func::getValI( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Bool:	return (rg.val().b != EVAL_BOOL) ? (bool)rg.val().b : EVAL_INT;
	    case Reg::Int:	return rg.val().i;
	    case Reg::Real:	return (rg.val().r != EVAL_REAL) ? (int64_t)rg.val().r : EVAL_INT;
	    case Reg::String:	return ((*rg.val().s) != EVAL_STR) ? atoll(rg.val().s->c_str()) : EVAL_INT;
	    case Reg::Var:	return io->getI(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getI();
	    case Reg::Obj:	return 1;
	    default:	break;
	}
    else return getVal(io,rg).getI();

    return EVAL_INT;
}

double Func::getValR( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Bool:	return (rg.val().b != EVAL_BOOL) ? (bool)rg.val().b : EVAL_REAL;
	    case Reg::Int:	return (rg.val().i != EVAL_INT) ? rg.val().i : EVAL_REAL;
	    case Reg::Real:	return rg.val().r;
	    case Reg::String:	return ((*rg.val().s) != EVAL_STR) ? atof(rg.val().s->c_str()) : EVAL_REAL;
	    case Reg::Var:	return io->getR(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getR();
	    case Reg::Obj:	return 1;
	    default:	break;
	}
    else return getVal(io,rg).getR();

    return EVAL_REAL;
}

char Func::getValB( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Bool:	return rg.val().b;
	    case Reg::Int:	return (rg.val().i != EVAL_INT) ? (bool)rg.val().i : EVAL_BOOL;
	    case Reg::Real:	return (rg.val().r != EVAL_REAL) ? (bool)rg.val().r : EVAL_BOOL;
	    case Reg::String:	return ((*rg.val().s) != EVAL_STR) ? (bool)atoi(rg.val().s->c_str()) : EVAL_BOOL;
	    case Reg::Var:	return io->getB(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getB();
	    case Reg::Obj:	return true;
	    default:	break;
	}
    else return getVal(io,rg).getB();

    return EVAL_BOOL;
}

AutoHD<TVarObj> Func::getValO( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
    {
	switch(rg.type())
	{
	    case Reg::Obj:	return *rg.val().o;
	    case Reg::Var:
		if(io->ioType(rg.val().io) == IO::Object) return io->getO(rg.val().io);
	    default:	break;
	}
	throw TError(nodePath().c_str(),_("Get object from no object's register"));
    }
    else return getVal(io,rg).getO();
}

void Func::setVal( TValFunc *io, RegW &rg, const TVariant &val )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Var:
		switch(val.type())
		{
		    case TVariant::Boolean:	io->setB(rg.val().io,val.getB());	break;
		    case TVariant::Integer:	io->setI(rg.val().io,val.getI());	break;
		    case TVariant::Real:	io->setR(rg.val().io,val.getR());	break;
		    case TVariant::String:	io->setS(rg.val().io,val.getS());	break;
		    case TVariant::Object:	io->setO(rg.val().io,val.getO());	break;
		    default:			io->setB(rg.val().io,EVAL_BOOL);
		}
		break;
	    case Reg::PrmAttr:
		switch(val.type())
		{
		    case TVariant::Boolean:	rg.val().pA->at().setB(val.getB());	break;
		    case TVariant::Integer:	rg.val().pA->at().setI(val.getI());	break;
		    case TVariant::Real:	rg.val().pA->at().setR(val.getR());	break;
		    case TVariant::String:	rg.val().pA->at().setS(val.getS());	break;
		    case TVariant::Object:	rg.val().pA->at().setO(val.getO());	break;
		    default:			rg.val().pA->at().setB(EVAL_BOOL);
		}
		break;
	    default:
		if(rg.vConst())	break;
		switch(val.type())
		{
		    case TVariant::Boolean:	rg = val.getB();	break;
		    case TVariant::Integer:	rg = val.getI();	break;
		    case TVariant::Real:	rg = val.getR();	break;
		    case TVariant::String:	rg = val.getS();	break;
		    case TVariant::Object:	rg = val.getO();	break;
		    default:			rg = (char)EVAL_BOOL;	break;
		}
		break;
	}
    else if(rg.type() == Reg::Obj)
    {
	TVariant vl(*rg.val().o);
	for(unsigned i_p = 0; i_p < rg.props().size(); i_p++)
	    if(i_p < (rg.props().size()-1)) vl = vl.getO().at().propGet(rg.props()[i_p]);
	    else vl.getO().at().propSet(rg.props()[i_p], val);
    }
}

void Func::setValS( TValFunc *io, RegW &rg, const string &val )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Var:	io->setS(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setS(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValI( TValFunc *io, RegW &rg, int64_t val )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Var:	io->setI(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setI(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValR( TValFunc *io, RegW &rg, double val )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Var:	io->setR(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setR(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValB( TValFunc *io, RegW &rg, char val )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Var:	io->setB(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setB(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValO( TValFunc *io, RegW &rg, AutoHD<TVarObj> val )
{
    if(rg.props().empty())
	switch(rg.type())
	{
	    case Reg::Var:	io->setO(rg.val().io,val);	break;
	    case Reg::PrmAttr:	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io,rg,val);
}

void Func::calc( TValFunc *val )
{
    ResAlloc res(fRes(), false);
    if(!startStat()) return;

    //> Exec calc
    ExecData dt = { SYS->sysTm(), 0 };
    exec(val, (const uint8_t*)prg.c_str(), dt);
    if(dt.flg&0x08) throw TError(nodePath().c_str(),_("Function execution terminated by error"));
}

void Func::exec( TValFunc *val, const uint8_t *cprg, ExecData &dt )
{
    RegW *reg = (RegW*)val->exCtx;

    while(!(dt.flg&0x01))
    {
	//> Calc time control mechanism
	if(SYS->sysTm() > (dt.start_tm+mMaxCalcTm))
	{
	    mess_err(nodePath().c_str(), _("Timeouted function calculation %d > %d+%d"), SYS->sysTm(), dt.start_tm, mMaxCalcTm);
	    dt.flg |= 0x09;
	    return;
	}
	//> Calc operation
	switch(*cprg)
	{
	    case Reg::EndFull: dt.flg |= 0x01;
	    case Reg::End: return;
	    //>> MVI codes
	    case Reg::MviB:
	    {
		struct SCode { uint8_t cod; uint16_t reg; char val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load bool %d to reg %d.", ptr->val, ptr->reg);
#endif
		reg[ptr->reg] = ptr->val;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviI:
	    {
		struct SCode { uint8_t cod; uint16_t reg; int64_t val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load integer %d to reg %d.", ptr->val, ptr->reg);
#endif
		reg[ptr->reg] = ptr->val;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviR:
	    {
		struct SCode { uint8_t cod; uint16_t reg; double val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load real %f to reg %d.", ptr->val, ptr->reg);
#endif
		reg[ptr->reg] = ptr->val;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviS:
            {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t len; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load string %s(%d) to reg %d.", string((const char*)(cprg+sizeof(SCode)),ptr->len).c_str(), ptr->len, ptr->reg);
#endif
		reg[ptr->reg] = string((const char*)(cprg+sizeof(SCode)),ptr->len);
		cprg += sizeof(SCode)+ptr->len; continue;
	    }
	    case Reg::MviObject:
	    {
		struct SCode { uint8_t cod; uint16_t reg; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load object to reg %d.", ptr->reg);
#endif
		reg[ptr->reg] = AutoHD<TVarObj>(new TVarObj());
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviArray:
	    {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t numb; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load array elements %d to reg %d.", ptr->numb, ptr->reg);
#endif
		TArrayObj *ar = new TArrayObj();
		//>>> Fill array by empty elements number
		if(ptr->numb == 1)
		    for(int i_p = 0; i_p < getValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode))]); i_p++)
			ar->arSet(i_p, EVAL_REAL);
		//>>> Fill array by parameters
		else
		    for(int i_p = 0; i_p < ptr->numb; i_p++)
			switch(reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))].vType(this) )
			{
			    case Reg::Bool:	ar->arSet(i_p, getValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))]));	break;
			    case Reg::Int:	ar->arSet(i_p, getValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))]));	break;
			    case Reg::Real:	ar->arSet(i_p, getValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))]));	break;
			    case Reg::String:	ar->arSet(i_p, getValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))]));	break;
			    case Reg::Obj:	ar->arSet(i_p, getVal(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))]));	break;
			    default:	break;
			}
		reg[ptr->reg] = AutoHD<TVarObj>(ar);
		cprg += sizeof(SCode) + ptr->numb*sizeof(uint16_t); continue;
	    }
	    case Reg::MviRegExp:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t expr; uint16_t arg; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load RegExpr to reg %d = (%d,%d).", ptr->rez, ptr->expr, ptr->arg);
#endif
		reg[ptr->rez] = AutoHD<TVarObj>(new TRegExp(getValS(val,reg[ptr->expr]), getValS(val,reg[ptr->arg])));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviSysObject:
	    {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t len; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load system object %s(%d) to reg %d.", string((const char*)(cprg+sizeof(SCode)),ptr->len).c_str(), ptr->len, ptr->reg);
#endif
		reg[ptr->reg] = AutoHD<TVarObj>(new TCntrNodeObj(SYS->nodeAt(string((const char*)(cprg+sizeof(SCode)),ptr->len),0,'.'),val->user()));
		cprg += sizeof(SCode)+ptr->len; continue;
	    }
	    case Reg::MviFuncArg:
	    {
		struct SCode { uint8_t cod; uint16_t reg; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Load the function arguments object to reg %d.", ptr->reg);
#endif
		reg[ptr->reg] = AutoHD<TVarObj>(new TFuncArgsObj(*val));
		cprg += sizeof(SCode); continue;
	    }
	    //>> Assign codes
	    case Reg::Ass:
	    {
		struct SCode { uint8_t cod; uint16_t toR; uint16_t fromR; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Assign from %d to %d.", ptr->fromR, ptr->toR);
#endif
		if(!reg[ptr->toR].props().size())
		    switch(reg[ptr->fromR].vType(this))
		    {
			case Reg::Bool:	  setValB(val, reg[ptr->toR], getValB(val,reg[ptr->fromR]));	break;
			case Reg::Int:	  setValI(val, reg[ptr->toR], getValI(val,reg[ptr->fromR]));	break;
			case Reg::Real:	  setValR(val, reg[ptr->toR], getValR(val,reg[ptr->fromR]));	break;
			case Reg::String: setValS(val, reg[ptr->toR], getValS(val,reg[ptr->fromR]));	break;
			default:	  setVal(val, reg[ptr->toR], getVal(val,reg[ptr->fromR]));		break;
		    }
		else setVal(val,reg[ptr->toR], getVal(val,reg[ptr->fromR]));
		cprg += sizeof(SCode); continue;
	    }
	    //>> Mov codes
	    case Reg::Mov:
	    {
		struct SCode { uint8_t cod; uint16_t toR; uint16_t fromR; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Move from %d to %d.", ptr->fromR, ptr->toR);
#endif
		switch(reg[ptr->fromR].vType(this))
		{
		    case Reg::Bool:	reg[ptr->toR] = getValB(val,reg[ptr->fromR]);	break;
		    case Reg::Int:	reg[ptr->toR] = getValI(val,reg[ptr->fromR]);	break;
		    case Reg::Real:	reg[ptr->toR] = getValR(val,reg[ptr->fromR]);	break;
		    case Reg::String:	reg[ptr->toR] = getValS(val,reg[ptr->fromR]);	break;
		    case Reg::Obj:	reg[ptr->toR] = getVal(val,reg[ptr->fromR]);	break;
		    default:	break;
		}
		cprg += sizeof(SCode); continue;
	    }
	    //>> Load properties for object
	    case Reg::OPrpSt:
	    {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t len; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Set object's %d properties to string len %d(%s).", ptr->reg, ptr->len, string((const char*)(cprg+sizeof(SCode)),ptr->len).c_str());
#endif
		reg[ptr->reg].props().push_back(string((const char*)(cprg+sizeof(SCode)),ptr->len));
		cprg += sizeof(SCode) + ptr->len; continue;
	    }
	    case Reg::OPrpDin:
	    {
		struct SCode { uint8_t cod; uint16_t reg; uint16_t val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Set object's %d properties to register's %d value.", ptr->reg, ptr->val);
#endif
		reg[ptr->reg].props().push_back(getValS(val,reg[ptr->val]));
		cprg += sizeof(SCode);  continue;
	    }
	    //>> Binary operations
	    case Reg::Add:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d + %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this))
		    {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[ptr->rez] = getValR(val,reg[ptr->a1]) + getValR(val,reg[ptr->a2]);
			    break;
			case Reg::String:
			    reg[ptr->rez] = getValS(val,reg[ptr->a1]) + getValS(val,reg[ptr->a2]);
			    break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'."));
		    }
		else
		{
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type())
		    {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[ptr->rez] = op1.getR() + getValR(val,reg[ptr->a2]); break;
			case TVariant::String:
			    reg[ptr->rez] = op1.getS() + getValS(val,reg[ptr->a2]); break;
			default:
			    throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'."));
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Sub:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d - %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) - getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Mul:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d * %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) * getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Div:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d / %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) / getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::RstI:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d %% %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) % getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitOr:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d | %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) | getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitAnd:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d & %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) & getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitXor:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d ^ %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) ^ getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitShLeft:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d << %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) << getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitShRight:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d >> %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) >> getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LOr:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d || %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = (getValB(val,reg[ptr->a1])==1) || (getValB(val,reg[ptr->a2])==1);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LCOr:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d c|| %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		if(getValB(val,reg[ptr->a1]) != true)
		{
		    exec(val, cprg+sizeof(SCode), dt);
		    reg[ptr->rez] = (getValB(val,reg[ptr->a2])==true);
		}
		else reg[ptr->rez] = true;
		cprg += ptr->end; continue;
	    }
	    case Reg::LAnd:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d && %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = (getValB(val,reg[ptr->a1])==1) && (getValB(val,reg[ptr->a2])==1);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LCAnd:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d c&& %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		if(getValB(val,reg[ptr->a1]) == true)
		{
		    exec(val, cprg+sizeof(SCode), dt);
		    reg[ptr->rez] = (getValB(val,reg[ptr->a2])==true);
		}
		else reg[ptr->rez] = false;
		cprg += ptr->end; continue;
	    }
	    case Reg::LT:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d < %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) < getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::GT:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d > %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) > getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LEQ:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d <= %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) <= getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::GEQ:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d >= %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) >= getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::EQU:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d == %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this))
		    {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[ptr->rez] = getValR(val,reg[ptr->a1]) == getValR(val,reg[ptr->a2]);
			    break;
			case Reg::String:
			    reg[ptr->rez] = getValS(val,reg[ptr->a1]) == getValS(val,reg[ptr->a2]);
			    break;
			default:
			    reg[ptr->rez] = false;
			    break;
			    //throw TError(nodePath().c_str(),_("Not supported type for operation 'EQU'."));
		    }
		else
		{
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type())
		    {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[ptr->rez] = op1.getR() == getValR(val,reg[ptr->a2]);
			    break;
			case TVariant::String:
			    reg[ptr->rez] = op1.getS() == getValS(val,reg[ptr->a2]);
			    break;
			default:
			    reg[ptr->rez] = false;
			    break;
			    //throw TError(nodePath().c_str(),_("Not supported type for operation 'EQU'."));
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::NEQ:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = %d != %d.", ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this))
		    {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[ptr->rez] = getValR(val,reg[ptr->a1]) != getValR(val,reg[ptr->a2]);
			    break;
			case Reg::String:
			    reg[ptr->rez] = getValS(val,reg[ptr->a1]) != getValS(val,reg[ptr->a2]);
			    break;
			default:
			    reg[ptr->rez] = false;
			    break;
			    //throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'."));
		    }
		else
		{
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type())
		    {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[ptr->rez] = op1.getR() != getValR(val,reg[ptr->a2]); break;
			case TVariant::String:
			    reg[ptr->rez] = op1.getS() != getValS(val,reg[ptr->a2]); break;
			default:
			    reg[ptr->rez] = false;
			    break;
			    //throw TError(nodePath().c_str(),_("Not supported type for operation 'Add'."));
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    //>> Unary operations
	    case Reg::Not:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = !%d.", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = !getValB(val,reg[ptr->a]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitNot:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = ~%d.", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = ~getValI(val,reg[ptr->a]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Neg:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: %d = -%d.", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = -getValR(val,reg[ptr->a]);
		cprg += sizeof(SCode); continue;
	    }
	    //>> Condition
	    case Reg::If:
	    {
		struct SCode { uint8_t cod; uint16_t cond; uint16_t toFalse; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "CODE: Condition %d: %d|%d|%d.", ptr->cond, sizeof(SCode), ptr->toFalse, ptr->end);
#endif
		if(getValB(val,reg[ptr->cond]))	exec(val, cprg+sizeof(SCode), dt);
		else if(ptr->toFalse != ptr->end) exec(val, cprg+ptr->toFalse, dt);
		cprg += ptr->end;
		continue;
	    }
	    case Reg::Cycle:
	    {
		struct SCode { uint8_t cod; uint16_t cond; uint16_t body; uint16_t after; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "CODE: Cycle %d: %d|%d|%d|%d.", ptr->cond, sizeof(SCode), ptr->body, ptr->after, ptr->end);
#endif
		while(!(dt.flg&0x01))
		{
		    exec(val, cprg+sizeof(SCode), dt);
		    if(!getValB(val,reg[ptr->cond])) break;
		    dt.flg &= ~0x06;
		    exec(val, cprg+ptr->body, dt);
		    //Check break and continue operators
		    if(dt.flg&0x02)	{ dt.flg=0; break; }
		    else if(dt.flg&0x04)dt.flg=0;

		    if(ptr->after) exec(val, cprg+ptr->after, dt);
		}
		cprg += ptr->end;
		continue;
	    }
	    case Reg::CycleObj:
	    {
		struct SCode { uint8_t cod; uint16_t obj; uint16_t body; uint16_t val; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "CODE: CycleObj %d: %d|%d|%d.", ptr->obj, ptr->body, ptr->val, ptr->end);
#endif
		TVariant obj = getVal(val,reg[ptr->obj]);
		if(obj.type() == TVariant::Object)
		{
		    vector<string> pLs;
		    obj.getO().at().propList(pLs);
		    for(unsigned i_l = 0; i_l < pLs.size() && !(dt.flg&0x01); i_l++)
		    {
			setValS(val,reg[ptr->val],pLs[i_l]);
			dt.flg &= ~0x06;
			exec(val, cprg + ptr->body, dt);
			//Check break and continue operators
			if(dt.flg&0x02)		{ dt.flg=0; break; }
			else if(dt.flg&0x04)	dt.flg=0;
		    }
		}
		cprg += ptr->end;
		continue;
	    }
	    case Reg::Break:	dt.flg|=0x03;	continue;
	    case Reg::Continue:	dt.flg|=0x05;	continue;
	    //>> Buildin functions
	    case Reg::FSin:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=sin(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = sin(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FCos:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=cos(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = cos(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTan:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=tan(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = tan(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FSinh:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=sinh(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = sinh(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FCosh:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=cosh(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = cosh(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTanh:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=tanh(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = tanh(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAsin:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=asin(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = asin(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAcos:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=acos(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = acos(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAtan:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=atan(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = atan(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FRand:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=rand(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a])*(double)rand()/(double)RAND_MAX;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FLg:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=lg(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = log10(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FLn:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=ln(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = log(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FExp:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=exp(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = exp(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FPow:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=pow(%d,%d).", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = pow(getValR(val,reg[ptr->a1]),getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FMin:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=min(%d,%d).", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = vmin(getValR(val,reg[ptr->a1]),getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FMax:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=max(%d,%d).", ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = vmax(getValR(val,reg[ptr->a1]),getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FSqrt:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=sqrt(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = sqrt(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAbs:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=abs(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = fabs(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FSign:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=sign(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = (getValR(val,reg[ptr->a])>=0)?1:-1;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FCeil:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=ceil(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = ceil(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FFloor:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=floor(%d).", ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = floor(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTypeOf:
	    {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "CODE: Function %d=typeof(%d).", ptr->rez, ptr->a);
#endif
		string rez = "undefined";
		TVariant vl = getVal(val, reg[ptr->a]);
		switch(vl.type())
		{
		    case TVariant::Null:	rez = "null";	break;
		    case TVariant::Boolean:	rez = "boolean";break;
		    case TVariant::Integer:	rez = "int";    break;
		    case TVariant::Real:	rez = "real";   break;
		    case TVariant::String:	rez = "string"; break;
		    case TVariant::Object:	rez = vl.getO().at().objName(); break;
		}
		reg[ptr->rez] = rez;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::CProc:
	    case Reg::CFunc:
	    {
		struct SCode { uint8_t cod; uint8_t f; uint8_t n; uint16_t rez; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;

		TValFunc *vfnc = val->ctxGet(ptr->f);
		if(!vfnc)
		{
		    vfnc = new TValFunc("JavaLikeFuncCalc",&funcAt(ptr->f)->func().at());
		    val->ctxSet(ptr->f,vfnc);
		}
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "CODE: Call function/procedure %d = %s(%d).", ptr->rez, vfnc->func()->id().c_str(), ptr->n);
#endif
		//  Get return position
		int r_pos, i_p, p_p;
		for(r_pos = 0; r_pos < vfnc->func()->ioSize(); r_pos++)
		    if(vfnc->ioFlg(r_pos)&IO::Return) break;
		//  Process parameters
		for(i_p = p_p = 0; true; i_p++)
		{
		    p_p = (i_p>=r_pos)?i_p+1:i_p;
		    if(p_p >= vfnc->func()->ioSize()) break;
		    //   Set default value
		    if(i_p >= ptr->n)	{ vfnc->setS(p_p,vfnc->func()->io(p_p)->def()); continue; }
		    switch(vfnc->ioType(p_p))
		    {
			case IO::String:	vfnc->setS(p_p,getValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))])); break;
			case IO::Integer:	vfnc->setI(p_p,getValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))])); break;
			case IO::Real:		vfnc->setR(p_p,getValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))])); break;
			case IO::Boolean:	vfnc->setB(p_p,getValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))])); break;
			case IO::Object:	vfnc->setO(p_p,getValO(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))])); break;
		    }
		}
		//  Make calc
		vfnc->calc(vfnc->user());
		//  Process outputs
		for(i_p = 0; i_p < ptr->n; i_p++)
		{
		    p_p = (i_p>=r_pos)?i_p+1:i_p;
		    if(p_p >= vfnc->func()->ioSize()) break;
		    if(vfnc->ioFlg(p_p)&IO::Output)
			switch(vfnc->ioType(p_p))
			{
			    case IO::String:	setValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],vfnc->getS(p_p)); break;
			    case IO::Integer:	setValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],vfnc->getI(p_p)); break;
			    case IO::Real:	setValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],vfnc->getR(p_p)); break;
			    case IO::Boolean:	setValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],vfnc->getB(p_p)); break;
			    case IO::Object:	setValO(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],vfnc->getO(p_p)); break;
			}
		}
		//  Set return
		if(ptr->cod == Reg::CFunc)
		    switch(vfnc->ioType(r_pos))
		    {
			case IO::String:	reg[ptr->rez] = vfnc->getS(r_pos); break;
			case IO::Integer:	reg[ptr->rez] = vfnc->getI(r_pos); break;
			case IO::Real:		reg[ptr->rez] = vfnc->getR(r_pos); break;
			case IO::Boolean:	reg[ptr->rez] = vfnc->getB(r_pos); break;
			case IO::Object:	reg[ptr->rez] = vfnc->getO(r_pos); break;
		    }

		cprg += sizeof(SCode) + ptr->n*sizeof(uint16_t); continue;
	    }
	    case Reg::CFuncObj:
	    {
		struct SCode { uint8_t cod; uint16_t obj; uint8_t n; uint16_t rez; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "CODE: Call object's function %d = %d(%d).", ptr->rez, ptr->obj, ptr->n);
#endif
		if(reg[ptr->obj].props().empty())
		    throw TError(nodePath().c_str(),_("Call object's function for no object or function name is empty."));

		TVariant obj = getVal(val,reg[ptr->obj],true);

		//> Prepare inputs
		vector<TVariant> prms;
		for(int i_p = 0; i_p < ptr->n; i_p++)
		    prms.push_back(getVal(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))]));

		//> Call
		TVariant rez = oFuncCall(obj, reg[ptr->obj].props().back(), prms);
		//if(obj.isModify()) setVal(val,reg[ptr->obj],obj,true);
		//> Process outputs
		for(unsigned i_p = 0; i_p < prms.size(); i_p++)
		    if(prms[i_p].isModify())
			switch(prms[i_p].type())
			{
			    case TVariant::Boolean:	setValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],prms[i_p].getB());	break;
			    case TVariant::Integer:	setValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],prms[i_p].getI());	break;
			    case TVariant::Real:	setValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],prms[i_p].getR());	break;
			    case TVariant::String:	setValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],prms[i_p].getS());	break;
			    case TVariant::Object:	setValO(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+i_p*sizeof(uint16_t))],prms[i_p].getO());	break;
			    default:	break;
			}
		//> Process return
		switch(rez.type())
		{
		    case TVariant::Boolean:	reg[ptr->rez] = rez.getB();	break;
		    case TVariant::Integer:	reg[ptr->rez] = rez.getI();	break;
		    case TVariant::Real:	reg[ptr->rez] = rez.getR();	break;
		    case TVariant::String:	reg[ptr->rez] = rez.getS();	break;
		    case TVariant::Object:	reg[ptr->rez] = rez.getO();	break;
		    default:	break;
		}

		cprg += sizeof(SCode) + ptr->n*sizeof(uint16_t); continue;
	    }
	    default:
		setStart(false);
		throw TError(nodePath().c_str(),_("Operation %c(%xh) error. Function '%s' is stopped."),*cprg,*cprg,id().c_str());
	}
    }
}

void Func::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TFunction::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function: ")+name(),/*owner().DB().empty()?R_R_R_:*/RWRWR_,"root",SDAQ_ID);
	if(owner().DB().size())
	    ctrMkNode("fld",opt,-1,"/func/st/timestamp",_("Date of modification"),R_R_R_,"root",SDAQ_ID,1,"tp","time");
	ctrMkNode("fld",opt,-1,"/func/cfg/name",_("Name"),owner().DB().empty()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"tp","str","len",OBJ_NM_SZ);
	ctrMkNode("fld",opt,-1,"/func/cfg/descr",_("Description"),owner().DB().empty()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "tp","str","cols","100","rows","5");
	ctrMkNode("fld",opt,-1,"/func/cfg/m_calc_tm",_("Maximum calculate time (sec)"),RWRWR_,"root",SDAQ_ID,3,
	    "tp","dec","min","0","max","3600");
	if(ctrMkNode("area",opt,-1,"/io",_("Program")))
	{
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),RWRWR_,"root",SDAQ_ID,1,"s_com","add,del,ins,move"))
	    {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Id"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),RWRWR_,"root",SDAQ_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d;%d;%d;%d",IO::Real,IO::Integer,IO::Boolean,IO::String,IO::String|(IO::FullText<<8),IO::Object).c_str(),
		    "sel_list",_("Real;Integer;Boolean;String;Text;Object"));
		ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),RWRWR_,"root",SDAQ_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d",IO::Default,IO::Output,IO::Return).c_str(),
		    "sel_list",_("Input;Output;Return"));
		ctrMkNode("list",opt,-1,"/io/io/4",_("Hide"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/io/io/5",_("Default"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	    ctrMkNode("fld",opt,-1,"/io/prog",_("Program"),RWRW__,"root",SDAQ_ID,3,"tp","str","rows","10","SnthHgl","1");
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/func/st/timestamp" && ctrChkNode(opt)) opt->setText(i2s(timeStamp()));
    else if(a_path == "/func/cfg/name" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setName(opt->text());
    else if(a_path == "/func/cfg/descr" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setDescr(opt->text());
    else if(a_path == "/func/cfg/m_calc_tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(maxCalcTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setMaxCalcTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/io/io")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    XMLNode *nId	= ctrMkNode("list",opt,-1,"/io/io/0","",RWRWR_);
	    XMLNode *nNm	= ctrMkNode("list",opt,-1,"/io/io/1","",RWRWR_);
	    XMLNode *nType	= ctrMkNode("list",opt,-1,"/io/io/2","",RWRWR_);
	    XMLNode *nMode	= ctrMkNode("list",opt,-1,"/io/io/3","",RWRWR_);
	    XMLNode *nHide	= ctrMkNode("list",opt,-1,"/io/io/4","",RWRWR_);
	    XMLNode *nDef	= ctrMkNode("list",opt,-1,"/io/io/5","",RWRWR_);
	    for(int id = 0; id < ioSize(); id++)
	    {
		if(nId)   nId->childAdd("el")->setText(io(id)->id());
		if(nNm)   nNm->childAdd("el")->setText(io(id)->name());
		if(nType) nType->childAdd("el")->setText(i2s(io(id)->type()|((io(id)->flg()&IO::FullText)<<8)));
		if(nMode) nMode->childAdd("el")->setText(i2s(io(id)->flg()&(IO::Output|IO::Return)));
		if(nHide) nHide->childAdd("el")->setText(io(id)->hide()?"1":"0");
		if(nDef)  nDef->childAdd("el")->setText(io(id)->def());
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))	ioAdd(new IO("new",_("New IO"),IO::Real,IO::Default));
	if(ctrChkNode(opt,"ins",RWRWR_,"root",SDAQ_ID,SEC_WR))	ioIns(new IO("new",_("New IO"),IO::Real,IO::Default), atoi(opt->attr("row").c_str()));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	ioDel(atoi(opt->attr("row").c_str()));
	if(ctrChkNode(opt,"move",RWRWR_,"root",SDAQ_ID,SEC_WR))	ioMove(atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    int row = atoi(opt->attr("row").c_str());
	    int col = atoi(opt->attr("col").c_str());
	    if((col == 0 || col == 1) && !opt->text().size())
	        throw TError(nodePath().c_str(),_("Empty value is not valid."));
	    switch(col)
	    {
		case 0:	io(row)->setId(opt->text());	break;
		case 1:	io(row)->setName(opt->text());	break;
		case 2:
		    io(row)->setType((IO::Type)(atoi(opt->text().c_str())&0xFF));
                    io(row)->setFlg(io(row)->flg()^((io(row)->flg()^(atoi(opt->text().c_str())>>8))&IO::FullText));
		    break;
		case 3:	io(row)->setFlg(io(row)->flg()^((io(row)->flg()^atoi(opt->text().c_str()))&(IO::Output|IO::Return)));	break;
		case 4:	io(row)->setHide(atoi(opt->text().c_str()));	break;
		case 5:	io(row)->setDef(opt->text());	break;
	    }
	    if(!owner().DB().empty()) modif();
	}
    }
    else if(a_path == "/io/prog")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(prog());
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))	{ setProg(opt->text()); progCompile(); }
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SDAQ_ID,SEC_RD))	mod->compileFuncSynthHighl("JavaScript",*opt);
    }
    else TFunction::cntrCmdProc(opt);
}

//*************************************************
//* Reg                                           *
//*************************************************
Reg::~Reg( )	{ setType(Free); }

Reg &Reg::operator=( Reg &irg )
{
    setType(irg.type());
    switch(type())
    {
	case Bool:	el.b = irg.el.b;	break;
	case Int:	el.i = irg.el.i;	break;
	case Real:	el.r = irg.el.r;	break;
	case String:	*el.s = *irg.el.s;	break;
	case Obj:	*el.o = *irg.el.o;	break;
	case Var:	el.io = irg.el.io;	break;
	case PrmAttr:	*el.pA = *irg.el.pA;	break;
	default:	break;
    }
    setName(irg.name());	//name
    mLock = irg.mLock;		//locked
    return *this;
}

void Reg::setType( Type tp )
{
    if(mTp == tp && mTp != Reg::Obj)	return;
    //Free old type
    switch(mTp)
    {
	case Reg::String:	delete el.s;	break;
	case Reg::Obj:		delete el.o;	break;
	case Reg::PrmAttr:	delete el.pA;	break;
	default:	break;
    }
    //Set new type
    switch(tp)
    {
	case Reg::String:	el.s = new string;		break;
	case Reg::Obj:		el.o = new AutoHD<TVarObj>;	break;
	case Reg::PrmAttr:	el.pA = new AutoHD<TVal>;	break;
	default:	break;
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
	    switch(val().pA->at().fld().type())
	    {
		case TFld::Boolean:	return Bool;
		case TFld::Integer:	return Int;
		case TFld::Real:	return Real;
		case TFld::String:	return String;
		default: break;
	    }
	default: break;
    }
    return type();
}

void Reg::free()
{
    if(lock()) return;

    setType(Free);
    mNm.clear();
    mObjEl = mLock = false;
}

//*************************************************
//* RegW : Work register                          *
//*************************************************
void RegW::operator=( const TVariant &ivar )
{
    switch(ivar.type())
    {
	case TVariant::Null:	*this = EVAL_REAL;	break;
	case TVariant::Boolean:	*this = ivar.getB();	break;
	case TVariant::Integer:	*this = ivar.getI();	break;
	case TVariant::Real:	*this = ivar.getR();	break;
	case TVariant::String:	*this = ivar.getS();	break;
	case TVariant::Object:	*this = ivar.getO();	break;
    }
}

void RegW::setType( Reg::Type tp )
{
    mPrps.clear();
    if(mTp == tp) return;
    //Free old type
    switch(mTp)
    {
	case Reg::String:	delete el.s;	break;
	case Reg::Obj:		delete el.o;	break;
	case Reg::PrmAttr:	delete el.pA;	break;
	default:	break;
    }
    //Set new type
    switch(tp)
    {
	case Reg::String:	el.s = new string;		break;
	case Reg::Obj:		el.o = new AutoHD<TVarObj>(new TVarObj()); break;
	case Reg::PrmAttr:	el.pA = new AutoHD<TVal>;	break;
	default:	break;
    }
    mTp = tp;
}

Reg::Type RegW::vType( Func *fnc )
{
    switch(type())
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
	    switch(val().pA->at().fld().type())
	    {
		case TFld::Boolean:	return Reg::Bool;
		case TFld::Integer:	return Reg::Int;
		case TFld::Real:	return Reg::Real;
		case TFld::String:	return Reg::String;
		default: break;
	    }
	    break;
	default: break;
    }
    return type();
}

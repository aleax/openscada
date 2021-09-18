
//OpenSCADA module DAQ.JavaLikeCalc file: freefunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2021 by Roman Savochenko, <roman@oscada.org>       *
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

Func *JavaLikeCalc::pF;

//*************************************************
//* Func: Function                                *
//*************************************************
Func::Func( const string &iid, const string &name ) : TConfig(&mod->elFnc()), TFunction(iid, SDAQ_ID),
    mMaxCalcTm(cfg("MAXCALCTM").getId()), mTimeStamp(cfg("TIMESTAMP").getId()), parseRes(mod->parseRes()), cntrInF(0), cntrCnst(0), cntrInFRegs(0)
{
    cfg("ID").setS(id());
    cfg("NAME").setS(name.empty() ? id() : name);
    cfg("FORMULA").setExtVal(true);
    mMaxCalcTm = mod->safeTm();
}

Func::Func( const Func &ifunc ) : TConfig(&mod->elFnc()), TFunction(ifunc.id(), SDAQ_ID),
    mMaxCalcTm(ifunc.mMaxCalcTm), mTimeStamp(ifunc.mTimeStamp), parseRes(mod->parseRes())
{
    operator=(ifunc);
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
    if(flag&NodeRemove && !owner().DB().empty())
	try{ del(); }
	catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

bool Func::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PR_TR") cfg("FORMULA").setNoTransl(!progTr());
    else if(co.name() == "FORMULA" && startStat() && co.getS() != pc.getS())
	setStart(false);
    modif();
    return true;
}

Lib &Func::owner( ) const	{ return *((Lib*)nodePrev()); }

string Func::name( )
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : id();
}

string Func::stor( ) const	{ return TFunction::stor().size() ? TFunction::stor() : owner().DB(); }

TCntrNode &Func::operator=( const TCntrNode &node )
{
    const Func *src_n = dynamic_cast<const Func*>(&node);
    if(!src_n) return *this;

    *(TConfig *)this = *(TConfig*)src_n;
    *(TFunction *)this = *(TFunction*)src_n;

    //Set to DB
    cfg("ID").setS(mId);

    if(src_n->startStat() && !startStat()) setStart(true);

    return *this;
}

Func &Func::operator=( const Func &func )
{
    *(TConfig*)this = (TConfig&)func;
    *(TFunction*)this = (TFunction&)func;

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

void Func::setProg( const string &iprg )
{
    cfg("FORMULA").setS(iprg);

    if(owner().DB().empty()) modifClr();
}

void Func::load_( TConfig *icfg )
{
    if(owner().DB().empty() || (!SYS->chkSelDB(owner().DB())))	throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(owner().fullDB(), mod->nodePath()+owner().tbl(), *this);

    loadIO();
}

void Func::loadIO( )
{
    if(startStat()) return;
    TConfig cfg(&mod->elFncIO());

    vector<string> u_pos;
    cfg.cfg("F_ID").setS(id(), TCfg::ForceUse);
    cfg.cfg("DEF").setExtVal(true);
    for(int fldCnt = 0; TBDS::dataSeek(owner().fullDB()+"_io",mod->nodePath()+owner().tbl()+"_io",fldCnt,cfg,TBDS::UseCache); fldCnt++) {
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
	if(io(id)->type() != IO::String || !(io(id)->flg()&IO::TransltText))
	    io(id)->setDef(cfg.cfg("DEF").getS(TCfg::ExtValOne));	//Force to no translation
	else io(id)->setDef(cfg.cfg("DEF").getS());
	io(id)->setHide(cfg.cfg("HIDE").getB());
    }
    //Remove holes
    for(unsigned iP = 0; iP < u_pos.size(); ) {
	if(u_pos[iP].empty()) { u_pos.erase(u_pos.begin()+iP); continue; }
	iP++;
    }
    //Position fixing
    for(int iP = 0; iP < (int)u_pos.size(); iP++) {
	int iid = ioId(u_pos[iP]);
	if(iid != iP) try{ ioMove(iid,iP); } catch(...) { }
    }
}

void Func::save_( )
{
    if(owner().DB().empty()) return;

    mTimeStamp = SYS->sysTm();
    TBDS::dataSet(owner().fullDB(), mod->nodePath()+owner().tbl(), *this);

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
    for(int i_io = 0; i_io < ioSize(); i_io++) {
	if(io(i_io)->flg()&Func::SysAttr) continue;
	cfg.cfg("ID").setS(io(i_io)->id());
	cfg.cfg("NAME").setS(io(i_io)->name());
	cfg.cfg("TYPE").setI(io(i_io)->type());
	cfg.cfg("MODE").setI(io(i_io)->flg());
	cfg.cfg("DEF").setNoTransl(!(io(i_io)->type()==IO::String && io(i_io)->flg()&IO::TransltText));
	cfg.cfg("DEF").setS(io(i_io)->def());
	cfg.cfg("HIDE").setB(io(i_io)->hide());
	cfg.cfg("POS").setI(i_io);
	TBDS::dataSet(io_bd, io_cfgpath, cfg);
    }

    //Clear IO
    cfg.cfgViewAll(false);
    for(int fldCnt = 0; TBDS::dataSeek(io_bd,io_cfgpath,fldCnt++,cfg); )
	if(ioId(cfg.cfg("ID").getS()) < 0) {
	    if(!TBDS::dataDel(io_bd,io_cfgpath,cfg,TBDS::UseAllKeys|TBDS::NoException)) break;
	    fldCnt--;
	}
}

void Func::del( )
{
    if(!owner().DB().size()) return;

    TBDS::dataDel(owner().fullDB(), mod->nodePath()+owner().tbl(), *this, TBDS::UseAllKeys);

    //Delete io from DB
    delIO();
}

void Func::delIO( )
{
    TConfig cfg(&mod->elFncIO());
    cfg.cfg("F_ID").setS(id(), true);
    TBDS::dataDel(owner().fullDB()+"_io", mod->nodePath()+owner().tbl()+"_io", cfg);
}

void Func::valAtt( TValFunc *vfnc )
{
    TFunction::valAtt(vfnc);
    ResAlloc res1(fRes(), false);
    workRegControl(vfnc);
}

void Func::valDet( TValFunc *vfnc )
{
    TFunction::valDet(vfnc);
    ResAlloc res1(fRes(), false);
    workRegControl(vfnc, true);
}

void Func::workRegControl( TValFunc *vfnc, bool toFree )
{
    if(vfnc->exCtx) {
	delete [] (RegW*)vfnc->exCtx;
	vfnc->exCtx = NULL;
    }
    if(!toFree && mRegs.size()) {
	vfnc->exCtx = new RegW[mRegs.size()];
	RegW *reg = (RegW*)vfnc->exCtx;
	//Init list of the registers
	for(unsigned iRg = 0; iRg < mRegs.size(); iRg++) {
	    Reg *tR = mRegs[iRg];
	    switch(tR->type()) {
		// Saved constants check
		case Reg::Bool:	if(tR->lock() && tR->name().empty()) { reg[iRg] = tR->val().b; reg[iRg].setVConst(); }	break;
		case Reg::Int:	if(tR->lock() && tR->name().empty()) { reg[iRg] = tR->val().i; reg[iRg].setVConst(); }	break;
		case Reg::Real:	if(tR->lock() && tR->name().empty()) { reg[iRg] = tR->val().r; reg[iRg].setVConst(); }	break;
		case Reg::String: if(tR->lock() && tR->name().empty()) { reg[iRg] = *tR->val().s;reg[iRg].setVConst(); }	break;

		case Reg::Var:	reg[iRg].setType(Reg::Var); reg[iRg].val().io = tR->val().io;			break;
		case Reg::PrmAttr: reg[iRg].setType(Reg::PrmAttr); *reg[iRg].val().pA = *tR->val().pA;		break;
		case Reg::Function: reg[iRg].setType(Reg::Function);						break;
		default: break;
	    }
	}
    }
}

void Func::setStart( bool val )
{
    if(val == runSt) return;
    //Start calcing
    if(val) {
	progCompile();

	TFunction::setStart(val);

	//Mark the users for the program changing
	ResAlloc res1(fRes(), false);
	for(unsigned i = 0; i < used.size(); i++) used[i]->progChange();
    }
    //Stop calcing
    else {
	ResAlloc res(fRes(), true);
	workClear();

	TFunction::setStart(val);
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
    ResAlloc res(parseRes, true);
    ResAlloc res1(fRes(), true);

    //Context clear for usings
    for(unsigned i = 0; i < used.size(); i++) used[i]->ctxClear();

    buildClear();
    workClear();

    pF   = this;	//Parse func
    sprg = cfg("FORMULA").getS();
    cntrCnst = cntrInFRegs = 0;

    if(yyparse()) {
	string tErr = pErr;
	buildClear();
	workClear();
	runSt = false;
	throw TError(nodePath().c_str(), "%s", tErr.c_str());
    }
    cntrInF = mInFncs.size();

    //Prepairing the shared registers list of the internal functions context
#ifdef IN_F_SHARE_REGS
    for(unsigned iRg = 0; iRg < mRegs.size(); iRg++) {
	Reg *tR = mRegs[iRg];
	if(!tR->inFnc()) continue;
	switch(tR->type()) {
	    case Reg::Bool: case Reg::Int: case Reg::Real: case Reg::String:
		cntrCnst++;
		if(tR->lock() && tR->name().empty() && tR->inFnc())	tR->setInFnc(false);
		break;
	    default: break;
	}
	if(!tR->inFnc()) continue;
	mShareRegs.push_back(iRg);
	tR->setInFnc("");	//To clean up
	cntrInFRegs++;
    }
#else
    for(unsigned iRg = 0; iRg < mRegs.size(); iRg++) {
	Reg *tR = mRegs[iRg];
	if(!tR->inFnc().size()) continue;
	switch(tR->type()) {
	    case Reg::Bool: case Reg::Int: case Reg::Real: case Reg::String:
		cntrCnst++;
		if(tR->lock() && tR->name().empty() && tR->inFnc().size())	tR->setInFnc("");
		break;
	    default: break;
	}
	if(!tR->inFnc().size()) continue;
	mInFncRegs[mInFncs[tR->inFnc()]].push_back(iRg);
	tR->setInFnc("");	//To clean up
	cntrInFRegs++;
    }
#endif

    buildClear();

    //Work registers update for the calculating contexts
    for(unsigned i = 0; i < used.size(); i++) workRegControl(used[i]);
}

int Func::funcGet( const string &path )
{
    string ns, fPath;

    //Check to correct function's path
    AutoHD<TCntrNode> nd = SYS->nodeAt(path, 0, '.', 0, true);
    for(int off = 0; (nd.freeStat() || !dynamic_cast<TFunction*>(&nd.at())) && (ns=TSYS::strParse(mUsings,0,";",&off)).size(); )
	nd = SYS->nodeAt(ns+"."+path, 0, '.', 0, true);
    if(nd.freeStat() || !dynamic_cast<TFunction*>(&nd.at())) return -1;
    fPath = nd.at().nodePath();

    //Search for already registered function
    for(int iFnc = 0; iFnc < (int)mFncs.size(); iFnc++)
	if(fPath == mFncs[iFnc].at().nodePath())
	    return iFnc;
    mFncs.push_back(nd);

    return mFncs.size()-1;
}

int Func::inFuncGet( const string &nm )
{
    map<string, int>::iterator iF = mInFncs.find(nm);
    return (iF == mInFncs.end()) ? -1 : iF->second;
}

void Func::inFuncDef( const string &nm, int pos )
{
    if(pos >= 0) { mInFncs[nm] = pos; mInFnc = nm; }
    else mInFnc = "";
}

int Func::regNew( bool sep, int recom, bool inFncNS )
{
    //Getting new register
    unsigned iRg = mRegs.size();
#ifdef IN_F_SHARE_REGS
    if(!sep) {
	if(recom >= 0 && recom < (int)mRegs.size() && !mRegs[recom]->lock() && mRegs[recom]->type() == Reg::Free)
	    iRg = recom;
	else for(iRg = 0; iRg < mRegs.size(); iRg++)
	    if(!mRegs[iRg]->lock() && mRegs[iRg]->type() == Reg::Free)	break;
    }
    if(iRg >= mRegs.size()) mRegs.push_back(new Reg(iRg));
    if(inFncNS && mInFnc.size() && !mRegs[iRg]->inFnc()) mRegs[iRg]->setInFnc(true);
#else
    if(!sep) {
	if(recom >= 0 && recom < (int)mRegs.size() && !mRegs[recom]->lock() &&
		mRegs[recom]->type() == Reg::Free && (!inFncNS || mRegs[recom]->inFnc() == mInFnc))
	    iRg = recom;
	else for(iRg = 0; iRg < mRegs.size(); iRg++)
	    if(!mRegs[iRg]->lock() && mRegs[iRg]->type() == Reg::Free && (!inFncNS || mRegs[iRg]->inFnc() == mInFnc))
		break;
    }
    if(iRg >= mRegs.size()) mRegs.push_back(new Reg(iRg));
    if(inFncNS) mRegs[iRg]->setInFnc(mInFnc);
#endif

    return iRg;
}

int Func::regGet( const string &inm, bool inFncNS )
{
    string nm = inm;
#ifndef IN_F_SHARE_REGS
    if(inFncNS && mInFnc.size()) nm = mInFnc+":"+nm;
#endif
    //Checking for allowed registers
    for(int iRg = 0; iRg < (int)mRegs.size(); iRg++)
	if(mRegs[iRg]->name() == nm)
	    return iRg;
    return -1;
}

int Func::ioGet( const string &nm )
{
    int rez = -1;

    for(int i_io = 0; i_io < ioSize(); i_io++)
	if(io(i_io)->id() == nm) {
	    rez = regNew(true);
	    Reg *rg = regAt(rez);
	    rg->setName(nm);
	    rg->setVar(i_io);
	    rg->setLock(true);
#ifdef IN_F_SHARE_REGS
	    rg->setInFnc(false);
#else
	    rg->setInFnc("");
#endif
	    break;
	}
    return rez;
}

void Func::buildClear( )
{
    laPos  = 0;		//LA position
    pErr   = "";
    sprg.clear();
    regTmpClear();	//Clear temporary registers list
    mInFnc = ""; mInFncs.clear();
    fPrmst.clear();
}

void Func::workClear( )
{
    prg.clear();	//Clear program
    mFncs.clear();	//Clear static linked external functions list
    regClear();		//Clear registers list
}

void Func::regClear( )
{
#ifdef IN_F_SHARE_REGS
    mShareRegs.clear();
#else
    mInFncRegs.clear();
#endif
    for(unsigned iRg = 0; iRg < mRegs.size(); iRg++)
	delete mRegs[iRg];
    mRegs.clear();
}

Reg *Func::regTmpNew( )
{
    unsigned iRg;
    for(iRg = 0; iRg < mTmpRegs.size(); iRg++)
	if(mTmpRegs[iRg]->type() == Reg::Free)
	    break;
    if(iRg >= mTmpRegs.size()) mTmpRegs.push_back(new Reg());
    return mTmpRegs[iRg];
}

void Func::regTmpClear( )
{
    for(unsigned iRg = 0; iRg < mTmpRegs.size(); iRg++)
	delete mTmpRegs[iRg];
    mTmpRegs.clear();
}

Reg *Func::cdMvi( Reg *op, bool no_code )
{
    Reg *rez = NULL, *tR = NULL;
    if(op->pos() >= 0) return op;	//Already load

    //Check for place used and set standalone
    int iRg = -1;
    switch(op->type()) {
	case Reg::Bool:
	    for(iRg = 0; iRg < (int)mRegs.size(); iRg++)
		if((tR=mRegs[iRg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().b == op->val().b)
		    break;
	    break;
	case Reg::Int:
	    for(iRg = 0; iRg < (int)mRegs.size(); iRg++)
		if((tR=mRegs[iRg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().i == op->val().i)
		    break;
	    break;
	case Reg::Real:
	    for(iRg = 0; iRg < (int)mRegs.size(); iRg++)
		if((tR=mRegs[iRg])->name().empty() && tR->type() == op->type() && tR->lock() && tR->val().r == op->val().r)
		    break;
	    break;
	case Reg::String:
	    for(iRg = 0; iRg < (int)mRegs.size(); iRg++)
		if((tR=mRegs[iRg])->name().empty() && tR->type() == op->type() && tR->lock() && *tR->val().s == *op->val().s)
		    break;
	    break;
	default: break;
    }
    if(iRg >= 0) {
	if(iRg < (int)mRegs.size()) {
	    rez = mRegs[iRg];
#ifdef OSC_DEBUG
	    SYS->cntrIter("ReusedConstants",1);
#endif
	}
	else if(mRegs.size() < cnstStatLim()) {
	    *(rez=regAt(pF->regNew(true))) = *op;
	    rez->setLock(true);
	    op->free();
#ifdef OSC_DEBUG
	    SYS->cntrIter("FixConstants",1);
#endif
	}
	if(rez) return rez;
    }

    //Same mvi
    int r_id = pF->regNew();
    *(rez=regAt(r_id)) = *op;
    op->free();
    if(no_code) return rez;
    uint16_t addr = rez->pos();

    switch(rez->type()) {
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
	case Reg::String: {
	    string sval = *rez->val().s;
	    //if(sval.size() > 255) throw TError(nodePath().c_str(),_("String-constant size is more for 255 symbols."));
	    prg += (uint8_t)Reg::MviS;
	    prg.append((char*)&addr, sizeof(uint16_t));
	    prg += (uint8_t)vmin(255, sval.size());
	    prg += sval.substr(0, vmin(255,sval.size()));
	    //Load and append next parts for big string (>255)
	    for(unsigned i_chunk = 1; i_chunk < (sval.size()/255+((sval.size()%255)?1:0)); i_chunk++) {
		Reg *treg = regTmpNew();
		*treg = sval.substr(i_chunk*255, vmin(255,(sval.size()-i_chunk*255)));
		rez = cdBinaryOp(Reg::Add, rez, treg);
	    }
	    break;
	}
	case Reg::Obj:
	    if(rez->name() == "SYS") {
		prg += (uint8_t)Reg::MviSysObject;
		prg.append((char*)&addr, sizeof(uint16_t));
		prg += (char)0;
	    }
	    else if(rez->name() == "arguments") {
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
    if(p_cnt > 255) throw TError(nodePath().c_str(),_("Array has more than 255 elements."));
    deque<int> p_pos;

    //Mvi all parameters
    for(int iPrm = 0; iPrm < p_cnt; iPrm++)
	fPrmst[iPrm] = cdMvi(fPrmst[iPrm]);
    //Get parameters.
    for(int iPrm = 0; iPrm < p_cnt; iPrm++) {
	p_pos.push_front(fPrmst.front()->pos());
	fPrmst.front()->free();
	fPrmst.pop_front();
    }
    //Make result
    Reg *rez = regAt(regNew());
    rez->setType(Reg::Obj);

    //Make code
    uint16_t addr;
    prg += (uint8_t)Reg::MviArray;
    addr = rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    prg += (uint8_t)p_cnt;
    for(unsigned iPrm = 0; iPrm < p_pos.size(); iPrm++)
    { addr = p_pos[iPrm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdMviRegExp( int p_cnt )
{
    if(p_cnt < 1 || p_cnt > 2) throw TError(nodePath().c_str(),_("RegExp requires one or two parameters."));

    Reg *rg_expr = NULL;
    Reg *rg_arg  = NULL;

    if(p_cnt == 2) {
	rg_arg = cdMvi(fPrmst.front());	fPrmst.pop_front();
	rg_expr = cdMvi(fPrmst.front());fPrmst.pop_front();
    }
    else {
	rg_expr = cdMvi(fPrmst.front());	fPrmst.pop_front();
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
    if(rez->pos() < 0) {
	if(tp != rez->vType(this))
	    switch(tp) {
		case Reg::Bool:
		    switch(rez->vType(this)) {
			case Reg::Int:    *rez = (char)((rez->val().i!=EVAL_INT) ? (bool)rez->val().i : (char)EVAL_BOOL);	break;
			case Reg::Real:   *rez = (char)((rez->val().r!=EVAL_REAL) ? (bool)rez->val().r : (char)EVAL_BOOL);	break;
			case Reg::String: *rez = (char)((*rez->val().s!=EVAL_STR) ? (bool)s2i(*rez->val().s) : (char)EVAL_BOOL); break;
			default: break;
		    }
		    break;
		case Reg::Int:
		    switch(rez->vType(this)) {
			case Reg::Bool:   *rez = (rez->val().b != EVAL_BOOL) ? (int64_t)rez->val().b : (int64_t)EVAL_INT;	break;
			case Reg::Real:	break;
			case Reg::String: *rez = (*rez->val().s != EVAL_STR) ? (int64_t)atoll(rez->val().s->c_str()) : (int64_t)EVAL_INT; break;
			default: break;
		    }
		    break;
		case Reg::Real:
		    switch(rez->vType(this)) {
			case Reg::Bool:	  *rez = (rez->val().b != EVAL_BOOL) ? (double)rez->val().b : EVAL_REAL;	break;
			case Reg::Int:    *rez = (rez->val().i != EVAL_INT) ? (double)rez->val().i : EVAL_REAL;		break;
			case Reg::String: *rez = (*rez->val().s != EVAL_STR) ? s2r(*rez->val().s) : EVAL_REAL;		break;
			default: break;
		    }
		    break;
		case Reg::String:
		    switch(rez->vType(this)) {
			case Reg::Bool:   *rez = (rez->val().b != EVAL_BOOL) ? i2s(rez->val().b) : EVAL_STR;	break;
			case Reg::Int:    *rez = (rez->val().i != EVAL_INT) ? ll2s(rez->val().i) : EVAL_STR;	break;
			case Reg::Real:   *rez = (rez->val().r != EVAL_REAL) ? r2s(rez->val().r) : EVAL_STR;	break;
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

Reg *Func::cdBinaryOp( Reg::Code cod, Reg *op1, Reg *op2, Reg *rez )
{
    //Check to allow the buildin calc and calc
    if(op1->pos() < 0 && op2->pos() < 0) {
	switch(cod) {
	    case Reg::Add: case Reg::AddAss: case Reg::Sub: case Reg::SubAss:
	    case Reg::Mul: case Reg::MulAss: case Reg::Div: case Reg::DivAss:
	    case Reg::LT: case Reg::GT:
	    case Reg::LEQ: case Reg::GEQ: case Reg::EQU: case Reg::NEQ:
		if(op1->vType(this) != Reg::String) op1 = cdTypeConv(op1, Reg::Real, true);
		break;
	    default: break;
	}

	op2 = cdTypeConv(op2, op1->vType(this), true);
	switch(op1->vType(this)) {
	    case Reg::Int:
		switch(cod) {
		    case Reg::RstI:	*op1 = op2->val().i ? (op1->val().i % op2->val().i) : 0;	break;
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
		switch(cod) {
		    case Reg::Add: case Reg::AddAss:	*op1 = op1->val().r + op2->val().r;	break;
		    case Reg::Sub: case Reg::SubAss:	*op1 = op1->val().r - op2->val().r;	break;
		    case Reg::Mul: case Reg::MulAss:	*op1 = op1->val().r * op2->val().r;	break;
		    case Reg::Div: case Reg::DivAss:	*op1 = op1->val().r / op2->val().r;	break;
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
		switch(cod) {
		    case Reg::RstI:	*op1 = op1->val().b % op2->val().b;	break;
		    case Reg::BitOr:	*op1 = op1->val().b | op2->val().b;	break;
		    case Reg::BitAnd:	*op1 = op1->val().b & op2->val().b;	break;
		    case Reg::BitXor:	*op1 = op1->val().b ^ op2->val().b;	break;
		    case Reg::LOr:	*op1 = op1->val().b || op2->val().b;	break;
		    case Reg::LAnd:	*op1 = op1->val().b && op2->val().b;	break;
		    default: break;
		}
	    case Reg::String:
		switch(cod) {
		    case Reg::Add: case Reg::AddAss:	*op1->val().s += *op2->val().s;	break;
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
    //if(op1_tp != Reg::Dynamic) op2 = cdTypeConv(op2, op1_tp);
    //else 
    if(op2->pos() < 0) op2 = cdMvi(op2);
    int op2_pos = op2->pos();

    if(rez != op1) op1->free();
    if(rez != op2) op2->free();
    // Prepare rezult
    if(!rez) {
	rez = regAt(regNew(false,op1_pos));	//Set recomend to op1_pos for appending fast, mostly for strings.
	rez->setType(rez_tp);
    }
    //!!!! Free operands after alloc rezult for prevent operations from self by some problems with object
    //if(rez != op1) op1->free();
    //if(rez != op2) op2->free();
    // Add code
    switch(cod) {
	case Reg::Add: case Reg::AddAss:
	case Reg::Sub: case Reg::SubAss:
	case Reg::Mul: case Reg::MulAss:
	case Reg::Div: case Reg::DivAss:
	case Reg::RstI: case Reg::BitOr: case Reg::BitAnd: case Reg::BitXor:
	    prg += (uint8_t)cod;	break;
	case Reg::BitShLeft: case Reg::BitShRight:
	    prg += (uint8_t)cod; rez->setType(Reg::Int);	break;
	case Reg::LOr: case Reg::LAnd:
	case Reg::LT: case Reg::GT:
	case Reg::LEQ: case Reg::GEQ: case Reg::EQU: case Reg::NEQ:
	    prg += (uint8_t)cod; rez->setType(Reg::Bool);	break;
	default: throw TError(nodePath().c_str(),_("Operation code %d is not supported."),cod);
    }

    uint16_t addr;
    addr = rez->pos();	prg.append((char*)&addr, sizeof(uint16_t));
    addr = op1_pos;	prg.append((char*)&addr, sizeof(uint16_t));
    addr = op2_pos;	prg.append((char*)&addr, sizeof(uint16_t));

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
    if(op->pos() < 0) {
	switch(op->vType(this)) {
	    case Reg::Int:
		switch(cod) {
		    case Reg::Not:	*op = !op->val().i;	break;
		    case Reg::BitNot:	*op = ~op->val().i;	break;
		    case Reg::Neg:	*op = -op->val().i;	break;
		    default: break;
		}
		break;
	    case Reg::Obj:
	    case Reg::Real:
		switch(cod) {
		    case Reg::Not:	*op = !op->val().r;	break;
		    case Reg::BitNot:	*op = ~(int)op->val().r;break;
		    case Reg::Neg:	*op = -op->val().r;	break;
		    default: break;
		}
		break;
	    case Reg::Bool:
		switch(cod) {
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
    switch(cod) {
	case Reg::Not:		prg += (uint8_t)Reg::Not;	break;
	case Reg::BitNot:	prg += (uint8_t)Reg::BitNot;	break;
	case Reg::Neg:		prg += (uint8_t)Reg::Neg;	break;
	default: throw TError(nodePath().c_str(),_("Operation code %d is not supported."),cod);
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

    if(thn != NULL && els != NULL) {
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

    //Mvi cond register (insert to program)
    cd_tmp = prg.substr(p_body);
    prg.erase(p_body);
    cond = cdMvi(cond);
    p_solve += prg.size()-p_body;
    p_end += prg.size()-p_body;
    if(p_postiter) p_postiter += prg.size()-p_body;
    prg += cd_tmp;
    uint16_t p_cond = cond->pos();
    cond->free();

    //Make apropos adress
    p_solve -= p_cmd;
    p_end   -= p_cmd;
    if(p_postiter) p_postiter -= p_cmd;

    //[CRRbbaann]
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

    //Mvi cond register (insert to program)
    string cd_tmp = prg.substr(p_body);
    prg.erase(p_body);
    cond = cdMvi(cond);
    p_solve += prg.size()-p_body;
    p_end += prg.size()-p_body;
    prg += cd_tmp;
    uint16_t p_cond = cond->pos();
    cond->free();

    //Make apropos adress
    p_solve -= p_cmd;
    p_end   -= p_cmd;

    //[COObbRRnn]
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
    //	throw TError(nodePath().c_str(),_("Built-in functions don't support the string type"));

    //Free parameter's registers
    if(prm1)	{ prm1 = cdMvi(prm1); p1_pos = prm1->pos(); }
    if(prm2)	{ prm2 = cdMvi(prm2); p2_pos = prm2->pos(); }
    if(prm1)	prm1->free();
    if(prm2)	prm2->free();

    //Get rezult register
    rez = regAt(regNew());
    rez->setType(Reg::Dynamic/*Reg::Real*/);

    //Make code
    prg += (uint8_t)f_cod;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    if(p1_pos >= 0) { addr = p1_pos; prg.append((char*)&addr, sizeof(uint16_t)); }
    if(p2_pos >= 0) { addr = p2_pos; prg.append((char*)&addr, sizeof(uint16_t)); }

    //Register constant's translations for FTr at compile stage
    if(f_cod == Reg::FTr && prm1 && prm1->type() == Reg::String) Mess->translGet(*prm1->val().s, "", "uapi:"+stor());

    return rez;
}

Reg *Func::cdExtFnc( int f_id, int p_cnt, bool proc, Reg *f_r )
{
    int r_pos;	//Return function's position
    Reg *rez = NULL;
    deque<int> p_pos;

    if(!f_r) {
	if(f_id >= EXT_F_LIM)
	    throw TError(nodePath().c_str(), _("Number of external functions reached the limit of %d on the function '%s'"), f_id, mFncs[f_id].at().id().c_str());

	//Check for return IO position
	bool ret_ok = false;
	for(r_pos = 0; r_pos < mFncs[f_id].at().ioSize(); r_pos++)
	    if((ret_ok=(bool)(mFncs[f_id].at().io(r_pos)->flg()&IO::Return)))
		break;

	//Check IO and parameters count
	if(p_cnt > mFncs[f_id].at().ioSize()-ret_ok)
	    throw TError(nodePath().c_str(), _("More than %d(%d) parameters are specified for the function '%s'"),
		(mFncs[f_id].at().ioSize()-ret_ok), p_cnt, mFncs[f_id].at().id().c_str());

	//Check the present return for fuction
	if(!proc && !ret_ok)
	    throw TError(nodePath().c_str(), _("Function '%s' is called, but it does not have the IO-return"), mFncs[f_id].at().id().c_str());
    }

    //Mvi all parameters
    for(int iPrm = 0; iPrm < p_cnt; iPrm++) fPrmst[iPrm] = cdMvi(fPrmst[iPrm]);

    //Get parameters. Add check parameters type
    for(int iPrm = 0; iPrm < p_cnt; iPrm++) {
	p_pos.push_front(fPrmst.front()->pos());
	fPrmst.front()->free();
	fPrmst.pop_front();
    }

    //Make result
    if(!proc) {
	rez = regAt(regNew());
	rez->setType(Reg::Real);
	/*if(!f_r)
	    switch(funcAt(f_id)->func().at().io(r_pos)->type()) {
		case IO::String:	rez->setType(Reg::String);	break;
		case IO::Integer:	rez->setType(Reg::Int);		break;
		case IO::Real:		rez->setType(Reg::Real);	break;
		case IO::Boolean:	rez->setType(Reg::Bool);	break;
		case IO::Object:	rez->setType(Reg::Obj);		break;
	    }*/
    }

    //Make code
    uint16_t addr;
    prg += proc ? (uint8_t)Reg::CProc : (uint8_t)Reg::CFunc;
    prg += (int8_t)f_id;
    prg += (uint8_t)p_cnt;
    addr = proc ? 0 : rez->pos(); prg.append((char*)&addr,sizeof(uint16_t));
    addr = f_r ? f_r->pos() : 0; prg.append((char*)&addr,sizeof(uint16_t));
    for(unsigned iPrm = 0; iPrm < p_pos.size(); iPrm++)
    { addr = p_pos[iPrm]; prg.append((char*)&addr,sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdIntFnc( int fOff, int pCnt, bool proc )
{
    Reg *rez = NULL;
    deque<int> pPos;

    //Mvi all parameters
    for(int iPrm = 0; iPrm < pCnt; iPrm++) fPrmst[iPrm] = cdMvi(fPrmst[iPrm]);

    //Get parameters
    for(int iPrm = 0; iPrm < pCnt; iPrm++) {
	pPos.push_front(fPrmst.front()->pos());
	fPrmst.front()->free();
	fPrmst.pop_front();
    }

    //Make result
    if(!proc) (rez=regAt(regNew()))->setType(Reg::Real);

    //Make the code
    uint16_t addr;
    prg += (uint8_t)Reg::IFunc;
    addr = fOff; prg.append((char*)&addr, sizeof(uint16_t));
    prg += (uint8_t)pCnt;
    addr = proc ? 0 : rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    for(unsigned iPrm = 0; iPrm < pPos.size(); iPrm++)
    { addr = pPos[iPrm]; prg.append((char*)&addr, sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdObjFnc( Reg *obj, const string &fNm, int p_cnt )
{
    if(fNm.size() > 254)throw TError(nodePath().c_str(), _("Function name is longer than 254."));
    if(p_cnt > 255)	throw TError(nodePath().c_str(), _("Object function contains more than 255 parameters."));

    Reg *rez = NULL;
    deque<int> p_pos;

    obj = cdMvi(obj);

    //Mvi all parameters
    for(int iPrm = 0; iPrm < p_cnt; iPrm++) fPrmst[iPrm] = cdMvi(fPrmst[iPrm]);
    //Get parameters
    for(int iPrm = 0; iPrm < p_cnt; iPrm++) {
	p_pos.push_front(fPrmst.front()->pos());
	fPrmst.front()->free();
	fPrmst.pop_front();
    }
    obj->free();
    rez = regAt(regNew());
    rez->setType(Reg::Dynamic);

    //Make the code
    uint16_t addr;
    prg += (uint8_t)Reg::CFuncObj;
    addr = obj->pos(); prg.append((char*)&addr, sizeof(uint16_t));
    prg += (uint8_t)fNm.size();
    prg += (uint8_t)p_cnt;
    addr = rez->pos(); prg.append((char*)&addr, sizeof(uint16_t));

    prg += fNm;

    for(unsigned iPrm = 0; iPrm < p_pos.size(); iPrm++)
    { addr = p_pos[iPrm]; prg.append((char*)&addr, sizeof(uint16_t)); }

    return rez;
}

Reg *Func::cdProp( Reg *obj, const string &sprp, Reg *dprp )
{
    uint16_t addr;
    Reg *ro = obj;
    if(!ro->objEl()) { ro = cdMove(NULL, cdMvi(ro), false); ro->setObjEl(); }

    if(!dprp) {
	prg += (uint8_t)Reg::OPrpSt;
	addr = ro->pos(); prg.append((char*)&addr, sizeof(uint16_t));
	prg += (uint8_t)sprp.size();
	prg += sprp;
    }
    else {
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
    switch(vl.type()) {
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
	case TVariant::String: {
	    if(prop == "length")	return (int64_t)vl.getS().size();
	    int sid = s2i(prop);
	    if(sid < 0 || sid >= (int)vl.getS().size()) return "";
	    return vl.getS().substr(sid,1);
	}
	default: return TVariant();
    }
    return TVariant();
}

TVariant Func::oFuncCall( TVariant &vl, const string &prop, vector<TVariant> &prms )
{
    try {
	switch(vl.type()) {
	    case TVariant::Null:
		if(prop == "isEVal")	return true;
		return false;
	    case TVariant::Object:	return vl.getO().at().funcCall(prop, prms);
	    case TVariant::Boolean:
		// bool isEVal( ); bool isNaN( ); - check value to "EVAL"
		if(prop == "isEVal" || prop == "isNaN")	return (vl.getB() == EVAL_BOOL);
		// string toString( ) - performs the value as the string “true” or “false”
		if(prop == "toString")	return string(vl.getB() ? "true" : "false");
		// real toReal() - read this Boolean as a real number
		if(prop == "toReal")	return vl.getR();
		// int toInt() - read this Boolean as an integer number
		if(prop == "toInt")	return (int64_t)vl.getI();
		return false;
		//throw TError(nodePath().c_str(),_("Boolean type has not the property '%s' or there are not enough parameters for it."),prop.c_str());
	    case TVariant::Integer:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal" || prop == "isNaN")	return (vl.getI() == EVAL_INT);
	    case TVariant::Real:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal")	return (vl.getR() == EVAL_REAL);
		// bool isNaN( ) - check the real for Not A Number
		if(prop == "isNaN")	return (vl.getR() == EVAL_REAL || isnan(vl.getR()));
		// string toExponential(int numbs = -1) - return the string of the number, formatted in exponential notation, 
		//      and with the number of significant digits <numbs>
		//  numbs - number of significant digits, if is missing the number of digits will have as much as needed
		if(prop == "toExponential") {
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
		if(prop == "toFixed") {
		    double val = vl.getR();
		    bool sign = (val < 0 || (prms.size() >= 3 && prms[2].getB()));
		    int n = prms.size() ? vmax(0,vmin(20,prms[0].getI())) : 0;
		    int w = (prms.size() >= 2) ? vmin(100,prms[1].getI()) : 0;
		    return TSYS::strMess((sign?"%+0*.*f":"%0*.*f"),w,n,val);
		}
		// string toPrecision(int prec = -1) - return the string of the formatted number with the number of significant digits <prec>
		//  prec - number of significant digits
		if(prop == "toPrecision") {
		    int n = prms.size() ? vmax(1,vmin(21,prms[0].getI())) : -1;
		    if(n < 0)	return TSYS::strMess("%g",vl.getR());
		    return TSYS::strMess("%.*g",n,vl.getR());
		}
		// string toString(int base = 10, int len = -1, bool sign = false) - return the string of the formatted number of integer type for
		//	minimum length <len> and strong sign present <sign>.
		//  base - representation base (2-36);
		//  len - minimum string number length (fill by 0);
		//  sign - strong sign present flag.
		if(prop == "toString") {
		    const char *dsymb = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		    int n = 10, w = -1;
		    int64_t val = vl.getI();
		    if(prms.size()) n = vmax(2, vmin(36,prms[0].getI()));
		    bool sign = (n == 10 && (val < 0 || (prms.size() >= 3 && prms[2].getB())));
		    if(prms.size() >= 2) w = vmin(100,prms[1].getI()) - (int)sign;
		    string rez;
		    if(n == 10) {
			for(int64_t c_vl = llabs(val); c_vl || rez.empty() || (w > 0 && (int)rez.size() < w); c_vl = c_vl/n)
			    rez += dsymb[c_vl%n];
			if(val < 0 || sign) rez += (val >= 0) ? "+" : "-";
		    }
		    else for(uint64_t c_vl = val; c_vl || rez.empty() || (w > 0 && (int)rez.size() < w); c_vl = c_vl/n)
			rez += dsymb[c_vl%n];
		    return TSYS::strEncode(rez, TSYS::Reverse);
		}
		// real toReal() - read this integer-real as a real number
		if(prop == "toReal") return vl.getR();
		// int toInt() - read this integer-real as an integer number
		if(prop == "toInt") return (int64_t)vl.getI();
		return false;
		//throw TError(nodePath().c_str(),_("Integer or real type has not the property '%s' or there are not enough parameters for it."),prop.c_str());
	    case TVariant::String:
		// bool isEVal( ) - check value to "EVAL"
		if(prop == "isEVal")	return (vl.getS() == EVAL_STR);
		// bool isNaN( bool whole = true ) - check the string for Not A Number
		if(prop == "isNaN")
		    return !TRegExp((prms.size()&&!prms[0].getB())?"^ *[+-]?\\d*\\.?\\d*(|[eE][+-]*\\d+)":"^ *[+-]?\\d*\\.?\\d*(|[eE][+-]*\\d+)$").test(vl.getS());
		// string charAt( int symb, string type = "") - extracts from the string the symbol <symb> of the type <type>
		//  symb - symbol position, changed to the next symbol position for UTF-8
		//  type  - symbol type, (""-ASCII and raw one byte code, UTF-8, UTF-16, UTF-32)
		if(prop == "charAt" && prms.size()) {
		    int n, n_;
		    n = n_ = prms[0].getI();
		    string tp = (prms.size() >= 2) ? TSYS::strEncode(prms[1].getS(),TSYS::ToLower) : "";
		    string s = vl.getS();
		    if(n < 0 || n >= (int)s.size()) return string("");
		    if(tp == "utf-8") {
			int l = Mess->getUTF8(s, n);
			if((l=Mess->getUTF8(s,n))) n += l; else n += 1;
			prms[0].setI(n); prms[0].setModify();
			return l ? s.substr(n_, l) : string("");
		    }
		    if(tp == "utf-16") return s.substr(n, 2);
		    if(tp == "utf-32") return s.substr(n, 4);
		    return s.substr(n, 1);
		}
		// int charCodeAt( int symb, string type = "" ) - extracts the symbol code <symb> from the string of the type <type>
		//  symb - symbol position, changed to the next symbol position for UTF-8
		//  type  - symbol type, (""-ASCII and raw one byte code, UTF-8, UTF-16, UTF-16LE, UTF-16BE, UTF-32, UTF-32LE, UTF-32BE)
		if(prop == "charCodeAt" && prms.size()) {
		    int n = prms[0].getI();
		    string tp = (prms.size() >= 2) ? TSYS::strEncode(prms[1].getS(),TSYS::ToLower) : "";
		    string s = vl.getS();
		    if(n < 0 || n >= (int)s.size())	return (int64_t)EVAL_INT;
		    if(tp == "utf-8") {
			int l;
			int32_t symb;
			if((l=Mess->getUTF8(s,n,&symb))) n += l; else n += 1;
			prms[0].setI(n); prms[0].setModify();
			return l ? (int64_t)symb : (int64_t)EVAL_INT;
		    }
		    if(tp.find("utf-16") == 0) {
			if((s.size()-n) < 2) s.resize(s.size()+1, 0);
			if(tp.find("be") != string::npos)
			    return (int64_t)TSYS::i16_BE(TSYS::getUnalign16(s.data()+n));
			return (int64_t)TSYS::i16_LE(TSYS::getUnalign16(s.data()+n));
		    }
		    if(tp.find("utf-32") == 0) {
			if((s.size()-n) < 4) s.resize(s.size()+3, 0);
			if(tp.find("be") != string::npos)
			    return (int64_t)TSYS::i32_BE(TSYS::getUnalign32(s.data()+n));
			return (int64_t)TSYS::i32_LE(TSYS::getUnalign32(s.data()+n));
		    }
		    return (int64_t)(unsigned char)vl.getS()[n];
		}
		// string concat( string val1, string val2, ... ) - returns a new string formed by joining the values <val1> etc
		//  val1, val2 - appended values
		if(prop == "concat" && prms.size()) {
		    string rez = vl.getS();
		    for(unsigned iP = 0; iP < prms.size(); iP++)
			rez += prms[iP].getS();
		    return rez;
		}
		// int indexOf( string substr, int start ) - returns the position of the required string <substr> in the original
		//       row from the position <start>
		//  substr - requested substring value
		//  start - start position for search
		if(prop == "indexOf" && prms.size()) {
		    size_t sp = 0;
		    if(prms.size() > 1) sp = vmax(0,vmin(vl.getS().size()-1,(unsigned)prms[1].getI()));
		    sp = vl.getS().find(prms[0].getS(),sp);
		    return (sp==string::npos) ? -1 : (int)sp;
		}
		// int lastIndexOf( string substr, int start ) - returns the position of the search string <substr> in the original
		//       one beginning from the position of <start> when searching from the end
		//  substr - requested substring value
		//  start - start position for search from the end
		if(prop == "lastIndexOf" && prms.size()) {
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
		if(prop == "search" && prms.size()) {
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
		if(prop == "match" && prms.size()) {
		    AutoHD<TRegExp> re;
		    if(prms[0].type() == TVariant::String) re = new TRegExp(prms[0].getS(), (prms.size()>=2)?prms[1].getS():string());
		    else if(prms[0].type() != TVariant::Object || (re=prms[0].getO()).freeStat()) return -1;
		    return re.at().match(vl.getS(), true);
		}
		// string slice( int beg, int end ) - return the string extracted from the original one starting from the <beg> position
		//       and ending be the <end>
		//  beg - begin position
		//  end - end position
		if((prop == "slice" || prop == "substring") && prms.size()) {
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
		// Array split( string sep, int limit ) - return the array of strings separated by <sep> with the limit of the number of elements <limit>.
		//  sep - items separator
		//  limit - items limit
		// Array split( RegExp pat, int limit ) - return the array of strings separated by RegExp pattern <pat> with the limit of the number of elements <limit>.
		//  pat - regular expression pattern.
		if(prop == "split" && prms.size()) {
		    //  Use RegExp for split
		    if(prms[0].type() == TVariant::Object && !AutoHD<TRegExp>(prms[0].getO()).freeStat())
			return AutoHD<TRegExp>(prms[0].getO()).at().split(vl.getS(),(prms.size()>=2)?prms[1].getI():0);

		    //  Use simple string separator
		    TArrayObj *rez = new TArrayObj();
		    for(size_t posB = 0, posC, iP = 0; true; iP++) {
			if(prms.size() > 1 && rez->size() >= prms[1].getI()) break;
			posC = vl.getS().find(prms[0].getS(),posB);
			if(posC != posB) rez->arSet(iP, vl.getS().substr(posB,posC-posB));
			if(posC == string::npos) break;
			posB = posC + prms[0].getS().size();
		    }
		    return rez;
		}
		// string insert( int pos, string substr ) - insert substring <substr> into this string's position <pos>
		//  pos - position for insert
		//  substr - substring for insert
		if(prop == "insert" && prms.size() >= 2)
		    return vl.getS().insert(vmax(0,vmin(vl.getS().size(),(unsigned)prms[0].getI())), prms[1].getS() );
		// string replace( int pos, int n, string str ) - replace substring into position <pos> and length <n> to string <str>.
		//  pos - position for start replace
		//  n - number symbols for replace
		//  str - string for replace
		// string replace( string substr, string str ) - replace all substrings <substr> to string <str>.
		//  substr - substring into all string
		// string replace( RegExp pat, string str ) - replace substrings by pattern <pat> to string <str>.
		//  pat - regular expression pattern
		if(prop == "replace" && prms.size() >= 2) {
		    string cstr = vl.getS();
		    //  Replace simple substrings by other string
		    if(prms[0].type() == TVariant::String && prms[1].type() == TVariant::String)
			for(size_t cpos = 0; (cpos=cstr.find(prms[0].getS(),cpos)) != string::npos; cpos += prms[1].getS().size())
			    cstr.replace(cpos,prms[0].getS().size(),prms[1].getS());
		    //  Replace substrings by RegExp patern by other string
		    else if(prms[0].type() == TVariant::Object && !AutoHD<TRegExp>(prms[0].getO()).freeStat())
			cstr = AutoHD<TRegExp>(prms[0].getO()).at().replace(cstr,prms[1].getS());
		    //  Simple - direct replace
		    if(prms.size() >= 3 && prms[2].type() == TVariant::String) {
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
		// real toReal( ) - convert this string to real number
		if(prop == "toReal") return s2r(vl.getS());
		// int toInt( int base = 0 ) - convert this string to integer number
		//  base - radix of subject sequence
		if(prop == "toInt") return (int64_t)strtoll(vl.getS().c_str(), NULL, (prms.size()>=1?prms[0].getI():10));
		// string parse( int pos, string sep = ".", int off = 0, bool mergeSepSymb = false ) - get token with number <pos> from the string, separated by <sep>
		//       and from offset <off>
		//  pos - item position
		//  sep - items separator
		//  off - start position
		if(prop == "parse" && prms.size()) {
		    int off = (prms.size() >= 3) ? prms[2].getI() : 0;
		    string rez = TSYS::strParse(vl.getS(), prms[0].getI(), (prms.size()>=2) ? prms[1].getS() : ".", &off, (prms.size()>=4)?prms[3].getB():false);
		    if(prms.size() >= 3) { prms[2].setI(off); prms[2].setModify(); }
		    return rez;
		}
		// string parseEnd( int pos, string sep = ".", int off = {length}, bool mergeSepSymb = false ) - get token with number <pos> from the string end, separated by <sep>
		//       and from offset <off>
		//  pos - item position
		//  sep - items separator
		//  off - start position
		if(prop == "parseEnd" && prms.size()) {
		    int off = (prms.size() >= 3) ? prms[2].getI() : vl.getS().size();
		    string rez = TSYS::strParseEnd(vl.getS(), prms[0].getI(), (prms.size()>=2) ? prms[1].getS() : ".", &off, (prms.size()>=4)?prms[3].getB():false);
		    if(prms.size() >= 3) { prms[2].setI(off); prms[2].setModify(); }
		    return rez;
		}
		// string parseLine( int pos, int off = 0 ) - get line with number <pos> from the string and from offset <off>
		//  pos - item position
		//  off - start position
		if(prop == "parseLine" && prms.size()) {
		    int off = (prms.size() >= 2) ? prms[1].getI() : 0;
		    string rez = TSYS::strLine(vl.getS(), prms[0].getI(), &off);
		    if(prms.size() >= 2) { prms[1].setI(off); prms[1].setModify(); }
		    return rez;
		}
		// string parsePath( int pos, int offCmptbl = 0, int off = 0 ) - get path token with number <pos> from the string and from offset <off>
		//  pos - item position
		//  offCmptbl - start position, for the compatibility - finish position to '/'
		//  off - start position - finish position to the next token begin
		if(prop == "parsePath" && prms.size()) {
		    bool isOff = (prms.size() >= 3);
		    bool isOffCmptbl = (!isOff && prms.size() >= 2);
		    int off = isOff ? prms[2].getI() : (isOffCmptbl ? prms[1].getI() : 0);
		    string rez = TSYS::pathLev(vl.getS(), prms[0].getI(), true, (isOffCmptbl?&off:NULL), (isOff?&off:NULL));
		    if(isOff) { prms[2].setI(off); prms[2].setModify(); }
		    else if(isOffCmptbl) { prms[1].setI(off); prms[1].setModify(); }
		    return rez;
		}
		// string parsePathEnd( int pos, int off = {length} ) - get path token with number <pos> from the string end and from offset <off>
		//  pos - item position
		//  off - start position
		if(prop == "parsePathEnd" && prms.size()) {
		    int off = (prms.size() >= 2) ? prms[1].getI() : vl.getS().size();
		    string rez = TSYS::pathLevEnd(vl.getS(), prms[0].getI(), true, &off);
		    if(prms.size() >= 2) { prms[1].setI(off); prms[1].setModify(); }
		    return rez;
		}
		// string path2sep( string sep = "." ) - convert path into this string to separated by <sep> string.
		//  sep - item separator
		if(prop == "path2sep")
		    return TSYS::path2sepstr(vl.getS(), (prms.size() && prms[0].getS().size()) ? prms[0].getS()[0] : '.');
		// string trim( string cfg = " \n\t\r" ) - string trimming at begin and end for symbols <cfg>.
		//  cfg - trimming symbols
		if(prop == "trim") return sTrm(vl.getS(), prms.size() ? prms[0].getS() : " \n\t\r");

		return false;
		//throw TError(nodePath().c_str(),_("String type has not the property '%s' or there are not enough parameters for it."),prop.c_str());
	    default:	break;
	}
	return false;
	//throw TError(nodePath().c_str(),_("Unknown type '%d' for the property '%s'."),vl.type(),prop.c_str());
    } catch(TError &err) {
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%s", err.mess.c_str());
    }

    return false;
}

TVariant Func::getVal( TValFunc *io, RegW &rg, bool fObj )
{
    TVariant vl(EVAL_REAL);

    //Get base value
    switch(rg.type()) {
	case Reg::Bool:		vl = rg.val().b;	break;
	case Reg::Int:		vl = rg.val().i;	break;
	case Reg::Real:		vl = rg.val().r;	break;
	case Reg::String:	vl = *rg.val().s;	break;
	case Reg::Var:
	    switch(io->ioType(rg.val().io)) {
		case IO::Boolean:	vl = io->getB(rg.val().io);	break;
		case IO::Integer:	vl = io->getI(rg.val().io);	break;
		case IO::Real:		vl = io->getR(rg.val().io);	break;
		case IO::String:	vl = io->getS(rg.val().io);	break;
		case IO::Object:	vl = io->getO(rg.val().io);	break;
		default: vl = EVAL_BOOL;
	    }
	    break;
	case Reg::PrmAttr:
	    switch(rg.val().pA->at().fld().type()) {
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

    for(unsigned iP = 0; iP < rg.props().size(); iP++) {
	if(fObj && iP == (rg.props().size()-1)) break;
	if(vl.isNull())	return TVariant();	//Null //return false;
	vl = oPropGet(vl, rg.props()[iP]);
    }

    return vl;
}

string Func::getValS( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type()) {
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
	switch(rg.type()) {
	    case Reg::Bool:	return (rg.val().b != EVAL_BOOL) ? (bool)rg.val().b : EVAL_INT;
	    case Reg::Int:	return rg.val().i;
	    case Reg::Real:	return (rg.val().r != EVAL_REAL) ? (int64_t)rg.val().r : EVAL_INT;
	    case Reg::String:	return ((*rg.val().s) != EVAL_STR) ? atoll(rg.val().s->c_str()) : EVAL_INT;
	    case Reg::Var:	return io->getI(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getI();
	    case Reg::Obj:	return 1;
	    case Reg::Function:	return rg.val().f->freeStat() ? 0 : 1;
	    default:	break;
	}
    else return getVal(io,rg).getI();

    return EVAL_INT;
}

double Func::getValR( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Bool:	return (rg.val().b != EVAL_BOOL) ? (bool)rg.val().b : EVAL_REAL;
	    case Reg::Int:	return (rg.val().i != EVAL_INT) ? rg.val().i : EVAL_REAL;
	    case Reg::Real:	return rg.val().r;
	    case Reg::String:	return ((*rg.val().s) != EVAL_STR) ? s2r(*rg.val().s) : EVAL_REAL;
	    case Reg::Var:	return io->getR(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getR();
	    case Reg::Obj:	return 1;
	    case Reg::Function:	return rg.val().f->freeStat() ? 0 : 1;
	    default:	break;
	}
    else return getVal(io,rg).getR();

    return EVAL_REAL;
}

char Func::getValB( TValFunc *io, RegW &rg )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Bool:	return rg.val().b;
	    case Reg::Int:	return (rg.val().i != EVAL_INT) ? (bool)rg.val().i : EVAL_BOOL;
	    case Reg::Real:	return (rg.val().r != EVAL_REAL) ? (bool)rg.val().r : EVAL_BOOL;
	    case Reg::String:	return ((*rg.val().s) != EVAL_STR) ? (bool)s2i(*rg.val().s) : EVAL_BOOL;
	    case Reg::Var:	return io->getB(rg.val().io);
	    case Reg::PrmAttr:	return rg.val().pA->at().getB();
	    case Reg::Obj:	return true;
	    case Reg::Function:	return !rg.val().f->freeStat();
	    default:	break;
	}
    else return getVal(io,rg).getB();

    return EVAL_BOOL;
}

AutoHD<TVarObj> Func::getValO( TValFunc *io, RegW &rg )
{
    if(rg.props().empty()) {
	switch(rg.type()) {
	    case Reg::Obj:	return *rg.val().o;
	    case Reg::Var:
		if(io->ioType(rg.val().io) == IO::Object) return io->getO(rg.val().io);
	    default:	break;
	}
	throw TError(nodePath().c_str(),_("Obtaining an object from a non-object register"));
    }
    else return getVal(io,rg).getO();
}

void Func::setVal( TValFunc *io, RegW &rg, const TVariant &val )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Var:
		switch(val.type()) {
		    case TVariant::Boolean:	io->setB(rg.val().io,val.getB());	break;
		    case TVariant::Integer:	io->setI(rg.val().io,val.getI());	break;
		    case TVariant::Real:	io->setR(rg.val().io,val.getR());	break;
		    case TVariant::String:	io->setS(rg.val().io,val.getS());	break;
		    case TVariant::Object:	io->setO(rg.val().io,val.getO());	break;
		    default:			io->setB(rg.val().io,EVAL_BOOL);
		}
		break;
	    case Reg::PrmAttr:
		switch(val.type()) {
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
		switch(val.type()) {
		    case TVariant::Boolean:	rg = val.getB();	break;
		    case TVariant::Integer:	rg = val.getI();	break;
		    case TVariant::Real:	rg = val.getR();	break;
		    case TVariant::String:	rg = val.getS();	break;
		    case TVariant::Object:	rg = val.getO();	break;
		    default:			rg = (char)EVAL_BOOL;	break;
		}
		break;
	}
    else if(rg.type() == Reg::Obj) {
	TVariant vl(*rg.val().o);
	for(unsigned iP = 0; iP < rg.props().size(); iP++)
	    if(iP < (rg.props().size()-1)) vl = vl.getO().at().propGet(rg.props()[iP]);
	    else vl.getO().at().propSet(rg.props()[iP], val);
    }
}

void Func::setValS( TValFunc *io, RegW &rg, const string &val )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Var:	io->setS(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setS(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValI( TValFunc *io, RegW &rg, int64_t val )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Var:	io->setI(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setI(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValR( TValFunc *io, RegW &rg, double val )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Var:	io->setR(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setR(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValB( TValFunc *io, RegW &rg, char val )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Var:	io->setB(rg.val().io, val);	break;
	    case Reg::PrmAttr:	rg.val().pA->at().setB(val);	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io, rg, val);
}

void Func::setValO( TValFunc *io, RegW &rg, AutoHD<TVarObj> val )
{
    if(rg.props().empty())
	switch(rg.type()) {
	    case Reg::Var:	io->setO(rg.val().io,val);	break;
	    case Reg::PrmAttr:	break;
	    default:		if(!rg.vConst()) rg = val;	break;
	}
    else setVal(io,rg,val);
}

void Func::calc( TValFunc *val )
{
    fRes().resRequestR();
    if(!startStat()) { fRes().resRelease(); return; }

    //Exec calc
    try {
	ExecData dt = { SYS->sysTm(), 0 };
	exec(val, (const uint8_t*)prg.c_str(), dt);
	if(dt.flg&0x08) throw TError(nodePath().c_str(),_("Function is interrupted by an error"));
    } catch(...) { fRes().resRelease(); throw; }
    fRes().resRelease();
}

void Func::exec( TValFunc *val, const uint8_t *cprg, ExecData &dt )
{
    RegW *reg = (RegW*)val->exCtx;

    while(!(dt.flg&0x01)) {
	//Calc time control mechanism
	if(SYS->sysTm() > (dt.startTm+mMaxCalcTm)) {
	    mess_err(nodePath().c_str(), _("Calculation time has been exceeded %d > %d+%d"), SYS->sysTm(), dt.startTm, mMaxCalcTm);
	    dt.flg |= 0x09;
	    return;
	}
	//Calc operation
	switch(*cprg) {
	    case Reg::EndFull: dt.flg |= 0x01;
	    case Reg::End: return;
	    // MVI codes
	    case Reg::MviB: {
		struct SCode { uint8_t cod; uint16_t reg; char val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load bool %d to reg %d.", cprg, ptr->val, ptr->reg);
#endif
		reg[ptr->reg] = ptr->val;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviI: {
		struct SCode { uint8_t cod; uint16_t reg; int64_t val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load integer %d to reg %d.", cprg, ptr->val, ptr->reg);
#endif
		reg[ptr->reg] = ptr->val;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviR: {
		struct SCode { uint8_t cod; uint16_t reg; double val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load real %f to reg %d.", cprg, ptr->val, ptr->reg);
#endif
		reg[ptr->reg] = ptr->val;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviS: {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t len; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load string %s(%d) to reg %d.", cprg, string((const char*)(cprg+sizeof(SCode)),ptr->len).c_str(), ptr->len, ptr->reg);
#endif
		reg[ptr->reg] = string((const char*)(cprg+sizeof(SCode)),ptr->len);
		cprg += sizeof(SCode)+ptr->len; continue;
	    }
	    case Reg::MviObject: {
		struct SCode { uint8_t cod; uint16_t reg; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load object to reg %d.", cprg, ptr->reg);
#endif
		reg[ptr->reg] = AutoHD<TVarObj>(new TVarObj());
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviArray: {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t numb; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load array elements %d to reg %d.", cprg, ptr->numb, ptr->reg);
#endif
		TArrayObj *ar = new TArrayObj();
		//  Fill array by empty elements number
		if(ptr->numb == 1) {
		    int itN = fmin(limUserIts_N,getValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode))]));
		    try { for(int iP = 0; iP < itN; iP++) ar->arSet(iP, EVAL_REAL); }
		    catch(...) {	//Remove the problematic array and create an empty one, for very big arrays mostly
			delete ar; ar = new TArrayObj();
			mess_err(nodePath().c_str(), _("Exception of creating %d items of the array."), itN);
		    }
		}
		//  Fill array by parameters
		else
		    for(int iP = 0; iP < ptr->numb; iP++)
			switch(reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))].vType(this)) {
			    case Reg::Bool:	ar->arSet(iP, getValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))]));	break;
			    case Reg::Int:	ar->arSet(iP, getValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))]));	break;
			    case Reg::Real:	ar->arSet(iP, getValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))]));	break;
			    case Reg::String:	ar->arSet(iP, getValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))]));	break;
			    case Reg::Obj:	ar->arSet(iP, getVal(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))]));	break;
			    default:	break;
			}
		reg[ptr->reg] = AutoHD<TVarObj>(ar);
		cprg += sizeof(SCode) + ptr->numb*sizeof(uint16_t); continue;
	    }
	    case Reg::MviRegExp: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t expr; uint16_t arg; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load RegExpr to reg %d = (%d,%d).", cprg, ptr->rez, ptr->expr, ptr->arg);
#endif
		reg[ptr->rez] = AutoHD<TVarObj>(new TRegExp(getValS(val,reg[ptr->expr]), getValS(val,reg[ptr->arg])));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MviSysObject: {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t len; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load system object %s(%d) to reg %d.", cprg, string((const char*)(cprg+sizeof(SCode)),ptr->len).c_str(), ptr->len, ptr->reg);
#endif
		reg[ptr->reg] = AutoHD<TVarObj>(new TCntrNodeObj(SYS->nodeAt(string((const char*)(cprg+sizeof(SCode)),ptr->len),0,'.'),val->user()));
		cprg += sizeof(SCode)+ptr->len; continue;
	    }
	    case Reg::MviFuncArg: {
		struct SCode { uint8_t cod; uint16_t reg; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Load the function arguments object to reg %d.", cprg, ptr->reg);
#endif
		reg[ptr->reg] = AutoHD<TVarObj>(new TFuncArgsObj(*val));
		cprg += sizeof(SCode); continue;
	    }
	    // Assign codes
	    case Reg::Ass: {
		struct SCode { uint8_t cod; uint16_t toR; uint16_t fromR; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Assign from %d to %d.", cprg, ptr->fromR, ptr->toR);
#endif
		if(reg[ptr->toR].type() == Reg::Function) {
		    val->ctxSet(EXT_F_LIM+ptr->toR, NULL);
		    reg[ptr->toR].val().f->free();

		    string path = getValS(val, reg[ptr->fromR]), ns;
		    AutoHD<TCntrNode> nd = SYS->nodeAt(path, 0, '.', 0, true);
		    for(int off = 0; (nd.freeStat() || !dynamic_cast<TFunction*>(&nd.at())) && (ns=TSYS::strParse(mUsings,0,";",&off)).size(); )
			nd = SYS->nodeAt(ns+"."+path, 0, '.', 0, true);
		    if(!nd.freeStat() && dynamic_cast<TFunction*>(&nd.at())) *reg[ptr->toR].val().f = nd;
		}
		else if(!reg[ptr->toR].props().size())
		    switch(reg[ptr->fromR].vType(this)) {
			case Reg::Bool:	  setValB(val, reg[ptr->toR], getValB(val,reg[ptr->fromR]));	break;
			case Reg::Int:	  setValI(val, reg[ptr->toR], getValI(val,reg[ptr->fromR]));	break;
			case Reg::Real:	  setValR(val, reg[ptr->toR], getValR(val,reg[ptr->fromR]));	break;
			case Reg::String: setValS(val, reg[ptr->toR], getValS(val,reg[ptr->fromR]));	break;
			default:	  setVal(val, reg[ptr->toR], getVal(val,reg[ptr->fromR]));	break;
		    }
		else setVal(val, reg[ptr->toR], getVal(val,reg[ptr->fromR]));
		cprg += sizeof(SCode); continue;
	    }
	    // Mov codes
	    case Reg::Mov: {
		struct SCode { uint8_t cod; uint16_t toR; uint16_t fromR; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Move from %d to %d.", cprg, ptr->fromR, ptr->toR);
#endif
		switch(reg[ptr->fromR].vType(this)) {
		    case Reg::Bool:	reg[ptr->toR] = getValB(val,reg[ptr->fromR]);	break;
		    case Reg::Int:	reg[ptr->toR] = getValI(val,reg[ptr->fromR]);	break;
		    case Reg::Real:	reg[ptr->toR] = getValR(val,reg[ptr->fromR]);	break;
		    case Reg::String:	reg[ptr->toR] = getValS(val,reg[ptr->fromR]);	break;
		    case Reg::Obj:	reg[ptr->toR] = getVal(val,reg[ptr->fromR]);	break;
		    default:	break;
		}
		cprg += sizeof(SCode); continue;
	    }
	    // Delete objects and its properties
	    case Reg::Delete: {
		struct SCode { uint8_t cod; uint16_t r; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Delete %d.", cprg, ptr->r);
#endif
		if(reg[ptr->r].props().empty())
		    switch(reg[ptr->r].type()) {
			case Reg::Obj:
			    if(!reg[ptr->r].vConst()) reg[ptr->r] = EVAL_REAL;
			    break;
			case Reg::Var:
			    if(val->ioType(reg[ptr->r].val().io) == IO::Object)
				setValO(val, reg[ptr->r], AutoHD<TVarObj>(new TVarObj()));
			    break;
			case Reg::Function:
			    val->ctxSet(EXT_F_LIM+ptr->r, NULL);
			    reg[ptr->r].val().f->free();
			    break;
			default: break;
		    }
		else {
		    TVariant obj = getVal(val, reg[ptr->r], true);
		    if(obj.type() == TVariant::Object) obj.getO().at().propClear(reg[ptr->r].props().back());
		}
		cprg += sizeof(SCode); continue;
	    }
	    // Load properties for object
	    case Reg::OPrpSt: {
		struct SCode { uint8_t cod; uint16_t reg; uint8_t len; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Set object's %d properties to string len %d(%s).", cprg, ptr->reg, ptr->len, string((const char*)(cprg+sizeof(SCode)),ptr->len).c_str());
#endif
		reg[ptr->reg].props().push_back(string((const char*)(cprg+sizeof(SCode)),ptr->len));
		cprg += sizeof(SCode) + ptr->len; continue;
	    }
	    case Reg::OPrpDin: {
		struct SCode { uint8_t cod; uint16_t reg; uint16_t val; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Set object's %d properties to register's %d value.", cprg, ptr->reg, ptr->val);
#endif
		reg[ptr->reg].props().push_back(getValS(val,reg[ptr->val]));
		cprg += sizeof(SCode);  continue;
	    }
	    // Binary operations
	    case Reg::Add: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d + %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this)) {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    reg[ptr->rez] = getValR(val,reg[ptr->a1]) + getValR(val,reg[ptr->a2]);
			    break;
			case Reg::String: {
			    if(ptr->rez == ptr->a1 && reg[ptr->rez].type() == Reg::String)
				reg[ptr->rez].val().s->append(getValS(val,reg[ptr->a2]));
			    else reg[ptr->rez] = getValS(val,reg[ptr->a1]) + getValS(val,reg[ptr->a2]);
			    break;
			}
			default:
			    throw TError(nodePath().c_str(), _("Type %d, which is not supported by the 'Add' operation."), reg[ptr->a1].vType(this));
		    }
		else {
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type()) {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    reg[ptr->rez] = op1.getR() + getValR(val,reg[ptr->a2]);	break;
			case TVariant::String:
			    reg[ptr->rez] = op1.getS() + getValS(val,reg[ptr->a2]);	break;
			default:
			    throw TError(nodePath().c_str(), _("Type %d, which is not supported by the 'Add' operation."), op1.type());
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::AddAss: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d := %d + %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this)) {
			case Reg::Bool: case Reg::Int: case Reg::Real:
			    setValR(val, reg[ptr->rez], getValR(val,reg[ptr->a1]) + getValR(val,reg[ptr->a2]));
			    break;
			case Reg::String:
			    if(ptr->rez == ptr->a1 && reg[ptr->rez].type() == Reg::String)
				reg[ptr->rez].val().s->append(getValS(val,reg[ptr->a2]));
			    else setValS(val, reg[ptr->rez], getValS(val,reg[ptr->a1]) + getValS(val,reg[ptr->a2]));
			    break;
			default:
			    throw TError(nodePath().c_str(), _("Type %d, which is not supported by the 'Add' operation."), reg[ptr->a1].vType(this));
		    }
		else {
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type()) {
			case TVariant::Boolean: case TVariant::Integer: case TVariant::Real:
			    setValR(val, reg[ptr->rez], op1.getR() + getValR(val,reg[ptr->a2])); break;
			case TVariant::String:
			    setValS(val, reg[ptr->rez], op1.getS() + getValS(val,reg[ptr->a2])); break;
			default:
			    throw TError(nodePath().c_str(), _("Type %d, which is not supported by the 'Add' operation."), op1.type());
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Sub: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d - %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) - getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::SubAss: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d := %d - %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		setValR(val, reg[ptr->rez], getValR(val,reg[ptr->a1]) - getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Mul: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d * %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) * getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::MulAss: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d := %d * %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		setValR(val, reg[ptr->rez], getValR(val,reg[ptr->a1]) * getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Div: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d / %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) / getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::DivAss: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d := %d / %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		setValR(val, reg[ptr->rez], getValR(val,reg[ptr->a1]) / getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::RstI: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d %% %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		int64_t div = getValI(val, reg[ptr->a2]);
		reg[ptr->rez] = div ? (getValI(val,reg[ptr->a1]) % div) : 0;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitOr: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d | %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) | getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitAnd: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d & %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) & getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitXor: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d ^ %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) ^ getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitShLeft: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d << %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) << getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitShRight: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d >> %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValI(val,reg[ptr->a1]) >> getValI(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LOr: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d || %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = (getValB(val,reg[ptr->a1])==1) || (getValB(val,reg[ptr->a2])==1);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LCOr: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d c|| %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		if(getValB(val,reg[ptr->a1]) != true) {
		    exec(val, cprg+sizeof(SCode), dt);
		    reg[ptr->rez] = (getValB(val,reg[ptr->a2])==true);
		}
		else reg[ptr->rez] = true;
		cprg += ptr->end; continue;
	    }
	    case Reg::LAnd: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d && %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = (getValB(val,reg[ptr->a1])==1) && (getValB(val,reg[ptr->a2])==1);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LCAnd: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d c&& %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		if(getValB(val,reg[ptr->a1]) == true) {
		    exec(val, cprg+sizeof(SCode), dt);
		    reg[ptr->rez] = (getValB(val,reg[ptr->a2])==true);
		}
		else reg[ptr->rez] = false;
		cprg += ptr->end; continue;
	    }
	    case Reg::LT: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d < %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) < getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::GT: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d > %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) > getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::LEQ: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d <= %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) <= getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::GEQ: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d >= %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a1]) >= getValR(val,reg[ptr->a2]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::EQU: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d == %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this)) {
			case Reg::String: reg[ptr->rez] = getValS(val,reg[ptr->a1]) == getValS(val,reg[ptr->a2]);	break;
			default: reg[ptr->rez] = getValR(val,reg[ptr->a1]) == getValR(val,reg[ptr->a2]);		break;
		    }
		else {
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type()) {
			case TVariant::String: reg[ptr->rez] = op1.getS() == getValS(val,reg[ptr->a2]);	break;
			default: reg[ptr->rez] = op1.getR() == getValR(val,reg[ptr->a2]);		break;
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::NEQ: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = %d != %d.", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		if(!reg[ptr->a1].props().size())
		    switch(reg[ptr->a1].vType(this)) {
			case Reg::String: reg[ptr->rez] = getValS(val,reg[ptr->a1]) != getValS(val,reg[ptr->a2]);	break;
			default: reg[ptr->rez] = getValR(val,reg[ptr->a1]) != getValR(val,reg[ptr->a2]);		break;
		    }
		else {
		    TVariant op1 = getVal(val,reg[ptr->a1]);
		    switch(op1.type()) {
			case TVariant::String: reg[ptr->rez] = op1.getS() != getValS(val,reg[ptr->a2]);	break;
			default: reg[ptr->rez] = op1.getR() != getValR(val,reg[ptr->a2]);		break;
		    }
		}
		cprg += sizeof(SCode); continue;
	    }
	    // Unary operations
	    case Reg::Not: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = !%d.", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = !getValB(val, reg[ptr->a]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::BitNot: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = ~%d.", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = ~getValI(val, reg[ptr->a]);
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::Neg: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: %d = -%d.", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = -getValR(val,reg[ptr->a]);
		cprg += sizeof(SCode); continue;
	    }
	    // Condition
	    case Reg::If: {
		struct SCode { uint8_t cod; uint16_t cond; uint16_t toFalse; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "%ph: Condition %d: %d|%d|%d.", cprg, ptr->cond, sizeof(SCode), ptr->toFalse, ptr->end);
#endif
		if(getValB(val,reg[ptr->cond]))	exec(val, cprg+sizeof(SCode), dt);
		else if(ptr->toFalse != ptr->end) exec(val, cprg+ptr->toFalse, dt);
		cprg += ptr->end;
		continue;
	    }
	    case Reg::Cycle: {
		struct SCode { uint8_t cod; uint16_t cond; uint16_t body; uint16_t after; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "%ph: Cycle %d: %d|%d|%d|%d.", cprg, ptr->cond, sizeof(SCode), ptr->body, ptr->after, ptr->end);
#endif
		while(!(dt.flg&0x01)) {
		    exec(val, cprg+sizeof(SCode), dt);
		    if(!getValB(val,reg[ptr->cond])) break;
		    dt.flg &= ~0x06;
		    exec(val, cprg+ptr->body, dt);
		    //Check break and continue operators
		    if(dt.flg&0x02)	{ dt.flg = 0; break; }
		    else if(dt.flg&0x04)dt.flg = 0;

		    if(ptr->after) exec(val, cprg+ptr->after, dt);
		}
		cprg += ptr->end;
		continue;
	    }
	    case Reg::CycleObj: {
		struct SCode { uint8_t cod; uint16_t obj; uint16_t body; uint16_t val; uint16_t end; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "%ph: CycleObj %d: %d|%d|%d.", cprg, ptr->obj, ptr->body, ptr->val, ptr->end);
#endif
		TVariant obj = getVal(val, reg[ptr->obj]);
		if(obj.type() == TVariant::Object) {
		    vector<string> pLs;
		    obj.getO().at().propList(pLs);
		    for(unsigned iL = 0; iL < pLs.size() && !(dt.flg&0x01); iL++) {
			setValS(val, reg[ptr->val], pLs[iL]);
			dt.flg &= ~0x06;
			exec(val, cprg + ptr->body, dt);
			//Check break and continue operators
			if(dt.flg&0x02)		{ dt.flg = 0; break; }
			else if(dt.flg&0x04)	dt.flg = 0;
		    }
		}
		cprg += ptr->end;
		continue;
	    }
	    case Reg::Break:	dt.flg |= 0x03;	continue;
	    case Reg::Continue:	dt.flg |= 0x05;	continue;
	    // Buildin functions
	    case Reg::FSin: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=sin(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = sin(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FCos: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=cos(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = cos(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTan: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=tan(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = tan(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FSinh: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=sinh(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = sinh(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FCosh: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=cosh(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = cosh(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTanh: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=tanh(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = tanh(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAsin: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=asin(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = asin(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAcos: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=acos(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = acos(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAtan: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=atan(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = atan(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FRand: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=rand(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = getValR(val,reg[ptr->a])*(double)rand()/(double)RAND_MAX;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FLg: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=lg(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = log10(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FLn: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=ln(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = log(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FExp: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=exp(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = exp(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FPow: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=pow(%d,%d).", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = pow(getValR(val,reg[ptr->a1]),getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FMin: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=min(%d,%d).", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = vmin(getValR(val,reg[ptr->a1]),getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FMax: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a1; uint16_t a2; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=max(%d,%d).", cprg, ptr->rez, ptr->a1, ptr->a2);
#endif
		reg[ptr->rez] = vmax(getValR(val,reg[ptr->a1]),getValR(val,reg[ptr->a2]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FSqrt: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=sqrt(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = sqrt(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FAbs: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=abs(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = fabs(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FSign: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=sign(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = (getValR(val,reg[ptr->a])>=0)?1:-1;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FCeil: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=ceil(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = ceil(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FFloor: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=floor(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = floor(getValR(val,reg[ptr->a]));
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTypeOf: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=typeof(%d).", cprg, ptr->rez, ptr->a);
#endif
		string rez = "undefined";
		TVariant vl = getVal(val, reg[ptr->a]);
		switch(vl.type()) {
		    case TVariant::Null:	rez = "null";	break;
		    case TVariant::Boolean:	rez = "boolean";break;
		    case TVariant::Integer:	rez = "int";	break;
		    case TVariant::Real:	rez = "real";	break;
		    case TVariant::String:	rez = "string";	break;
		    case TVariant::Object:	rez = vl.getO().at().objName(); break;
		}
		reg[ptr->rez] = rez;
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::FTr: {
		struct SCode { uint8_t cod; uint16_t rez; uint16_t a; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "%ph: Function %d=tr(%d).", cprg, ptr->rez, ptr->a);
#endif
		reg[ptr->rez] = Mess->translGetLU(getValS(val,reg[ptr->a]), val->lang(), val->user(), "uapi:"+val->func()->stor());
		cprg += sizeof(SCode); continue;
	    }
	    case Reg::CProc:
	    case Reg::CFunc: {
		struct SCode { uint8_t cod; int8_t f; uint8_t n; uint16_t rez; uint16_t f_r; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;

		bool fromR = (ptr->f < 0);
		if(!fromR || (reg[ptr->f_r].type() == Reg::Function && !reg[ptr->f_r].val().f->freeStat())) {
		    int ctxId = fromR ? EXT_F_LIM+ptr->f_r : ptr->f;
		    TValFunc *vfnc = val->ctxGet(ctxId);
		    if(!vfnc) {
			vfnc = new TValFunc("JavaLikeFuncCalc", fromR ? &reg[ptr->f_r].val().f->at() : &mFncs[ptr->f].at());
			vfnc->setUser(val->user()); vfnc->setLang(val->lang());
			val->ctxSet(ctxId, vfnc);
		    }
#ifdef OSC_DEBUG
		    if(mess_lev() == TMess::Debug)
			mess_debug(nodePath().c_str(), "%ph: Call function/procedure %d = %s(%d).", cprg, ptr->rez, vfnc->func()->id().c_str(), ptr->n);
#endif
		    //  Get return position
		    int r_pos, iP, p_p;
		    for(r_pos = 0; r_pos < vfnc->func()->ioSize(); r_pos++)
			if(vfnc->ioFlg(r_pos)&IO::Return) break;
		    //  Process parameters
		    for(iP = p_p = 0; true; iP++) {
			p_p = (iP>=r_pos)?iP+1:iP;
			if(p_p >= vfnc->func()->ioSize()) break;
			//   Set default value
			if(iP >= ptr->n)	{ vfnc->setS(p_p,vfnc->func()->io(p_p)->def()); continue; }
			switch(vfnc->ioType(p_p)) {
			    case IO::String:	vfnc->setS(p_p,getValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))])); break;
			    case IO::Integer:	vfnc->setI(p_p,getValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))])); break;
			    case IO::Real:	vfnc->setR(p_p,getValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))])); break;
			    case IO::Boolean:	vfnc->setB(p_p,getValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))])); break;
			    case IO::Object:	vfnc->setO(p_p,getValO(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))])); break;
			}
		    }
		    //  Make calc
		    vfnc->calc(val->user());
		    //  Process outputs
		    for(iP = 0; iP < ptr->n; iP++) {
			p_p = (iP>=r_pos)?iP+1:iP;
			if(p_p >= vfnc->func()->ioSize()) break;
			if(vfnc->ioFlg(p_p)&IO::Output)
			    switch(vfnc->ioType(p_p)) {
				case IO::String:  setValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))],vfnc->getS(p_p)); break;
				case IO::Integer: setValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))],vfnc->getI(p_p)); break;
				case IO::Real:	  setValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))],vfnc->getR(p_p)); break;
				case IO::Boolean: setValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))],vfnc->getB(p_p)); break;
				case IO::Object:  setValO(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iP*sizeof(uint16_t))],vfnc->getO(p_p)); break;
			    }
		    }
		    //  Set return
		    if(ptr->cod == Reg::CFunc)
			switch(vfnc->ioType(r_pos)) {
			    case IO::String:	reg[ptr->rez] = vfnc->getS(r_pos); break;
			    case IO::Integer:	reg[ptr->rez] = vfnc->getI(r_pos); break;
			    case IO::Real:	reg[ptr->rez] = vfnc->getR(r_pos); break;
			    case IO::Boolean:	reg[ptr->rez] = vfnc->getB(r_pos); break;
			    case IO::Object:	reg[ptr->rez] = vfnc->getO(r_pos); break;
			}
		}

		cprg += sizeof(SCode) + ptr->n*sizeof(uint16_t); continue;
	    }
	    case Reg::CFuncObj: {
		struct SCode { uint8_t cod; uint16_t obj; uint8_t nmLn; uint8_t n; uint16_t rez; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "%ph: Call object's function %d = %d.%s(%d).",
			cprg, ptr->rez, ptr->obj, string((const char*)(cprg+sizeof(SCode)),ptr->nmLn).c_str(), ptr->n);
#endif
		TVariant obj = getVal(val, reg[ptr->obj]);

		//Prepare inputs
		vector<TVariant> prms; prms.reserve(ptr->n);
		for(int iP = 0; iP < ptr->n; iP++)
		    prms.push_back(getVal(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+ptr->nmLn+iP*sizeof(uint16_t))]));

		//Call
		TVariant rez = oFuncCall(obj, string((const char*)(cprg+sizeof(SCode)),ptr->nmLn), prms);
		//if(obj.isModify()) setVal(val,reg[ptr->obj],obj,true);
		//Process outputs
		for(unsigned iP = 0; iP < prms.size(); iP++)
		    if(prms[iP].isModify())
			switch(prms[iP].type()) {
			    case TVariant::Boolean:	setValB(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+ptr->nmLn+iP*sizeof(uint16_t))],prms[iP].getB());	break;
			    case TVariant::Integer:	setValI(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+ptr->nmLn+iP*sizeof(uint16_t))],prms[iP].getI());	break;
			    case TVariant::Real:	setValR(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+ptr->nmLn+iP*sizeof(uint16_t))],prms[iP].getR());	break;
			    case TVariant::String:	setValS(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+ptr->nmLn+iP*sizeof(uint16_t))],prms[iP].getS());	break;
			    case TVariant::Object:	setValO(val,reg[TSYS::getUnalign16(cprg+sizeof(SCode)+ptr->nmLn+iP*sizeof(uint16_t))],prms[iP].getO());	break;
			    default:	break;
			}
		//Process return
		switch(rez.type()) {
		    case TVariant::Boolean:	reg[ptr->rez] = rez.getB();	break;
		    case TVariant::Integer:	reg[ptr->rez] = rez.getI();	break;
		    case TVariant::Real:	reg[ptr->rez] = rez.getR();	break;
		    case TVariant::String:	reg[ptr->rez] = rez.getS();	break;
		    case TVariant::Object:	reg[ptr->rez] = rez.getO();	break;
		    default:	break;
		}

		cprg += sizeof(SCode) + ptr->nmLn + ptr->n*sizeof(uint16_t); continue;
	    }
	    case Reg::IFuncDef: {
		struct SCode { uint8_t cod; uint16_t sz; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "%ph: Internal function definition pass in long %d.", cprg, ptr->sz);
#endif
		cprg += ptr->sz; continue;
	    }
	    case Reg::IFunc: {
		struct SCode { uint8_t cod; uint16_t off; uint8_t n; uint16_t rez; } __attribute__((packed));
		const struct SCode *ptr = (const struct SCode *)cprg;
		struct SFCode { uint8_t cod; uint16_t sz; uint8_t n; uint16_t rez; } __attribute__((packed));
		const struct SFCode *ptrF = (const struct SFCode *)(prg.data()+ptr->off);

#ifdef OSC_DEBUG
		if(mess_lev() == TMess::Debug)
		    mess_debug(nodePath().c_str(), "%ph: Call internal function/procedure %d = %d(%d).", cprg, ptr->rez, ptr->off, ptr->n);
#endif

#ifdef IN_F_SHARE_REGS
		if(dt.inFShareRegs.size() > IN_F_REC_LIM)
		    mess_err(nodePath().c_str(), _("You have too deep recursion of the internal function call."));
		else {
		    // Saving the function registers
		    dt.inFShareRegs.push_back(vector<RegW>());
		    dt.inFShareRegs.back().reserve(mShareRegs.size());
		    for(unsigned iIt = 0; iIt < mShareRegs.size(); iIt++) {
			unsigned iRg = mShareRegs[iIt];
			dt.inFShareRegs.back().push_back(reg[iRg]);
			/* Initiating the automatic variables in EVAL
			if(mRegs[iRg]->name().size()) {
			    int iF = 0;
			    for( ; iF < vmin(ptrF->n, ptr->n); iF++)
				if(iRg == TSYS::getUnalign16((const uint8_t*)ptrF+sizeof(SFCode)+iF*sizeof(uint16_t)) ||
					iRg == TSYS::getUnalign16(cprg+sizeof(SCode)+iF*sizeof(uint16_t)))
				    break;
			    if(iF >= vmin(ptrF->n,ptr->n)) reg[iRg] = RegW();
			}*/
		    }
#else
		vector<int> &inFRIds = mInFncRegs[ptr->off];
		vector< vector<RegW> > &inFRegs = dt.inFRegs[ptr->off];

		if(inFRegs.size() > IN_F_REC_LIM)
		    mess_err(nodePath().c_str(), _("You have too deep recursion of the internal function call."));
		else {
		    // Saving the function registers
		    inFRegs.push_back(vector<RegW>());
		    if(inFRegs.size() > 1) {
			inFRegs.back().reserve(inFRIds.size());
			for(unsigned iIt = 0; iIt < inFRIds.size(); iIt++)
			    inFRegs.back().push_back(reg[inFRIds[iIt]]);
		    }
#endif

		    // Processing the parameters
		    for(int iF = 0; iF < ptrF->n; iF++) {
			uint16_t dP = TSYS::getUnalign16((const uint8_t*)ptrF+sizeof(SFCode)+iF*sizeof(uint16_t));
			if(iF >= ptr->n) reg[dP] = TVariant();
			else {
			    uint16_t sP = TSYS::getUnalign16(cprg+sizeof(SCode)+iF*sizeof(uint16_t));
			    //  Prevent the same registers processing
			    if(sP != dP) reg[dP] = getVal(val, reg[sP]);
			}
		    }

		    // Make calc
		    exec(val, (const uint8_t*)ptrF+sizeof(SFCode)+ptrF->n*sizeof(uint16_t), dt);

		    dt.flg = 0;	//Clean up all flags
		    // Processing the outputs
		    for(int iF = 0; iF < vmin(ptrF->n,ptr->n); iF++)
			setVal(val, reg[TSYS::getUnalign16(cprg+sizeof(SCode)+iF*sizeof(uint16_t))],
				    getVal(val, reg[TSYS::getUnalign16((const uint8_t*)ptrF+sizeof(SFCode)+iF*sizeof(uint16_t))]));

		    // Restoring the function registers
#ifdef IN_F_SHARE_REGS
		    for(unsigned iIt = 0; iIt < mShareRegs.size(); iIt++) {
			unsigned iRg = mShareRegs[iIt];
			//  Prevent restoring the function arguments
			int iF = 0;
			for( ; iF < ptr->n; iF++)
			    if(iRg == TSYS::getUnalign16(cprg+sizeof(SCode)+iF*sizeof(uint16_t)))
				break;
			if(iF >= ptr->n) reg[iRg] = dt.inFShareRegs.back()[iIt];
		    }
		    dt.inFShareRegs.pop_back();
#else
		    if(inFRegs.size() > 1)
			for(unsigned iIt = 0; iIt < inFRIds.size(); iIt++) {
			    unsigned iRg = inFRIds[iIt];
			    //  Prevent restoring the function arguments
			    int iF = 0;
			    for( ; iF < ptr->n; iF++)
				if(iRg == TSYS::getUnalign16(cprg+sizeof(SCode)+iF*sizeof(uint16_t)))
				    break;
			    if(iF >= ptr->n) reg[iRg] = inFRegs.back()[iIt];
			}
		    inFRegs.pop_back();
#endif

		    // Return rezult
		    if(ptr->rez) reg[ptr->rez] = getVal(val, reg[ptrF->rez]);
		}

		cprg += sizeof(SCode) + ptr->n*sizeof(uint16_t); continue;
	    }
	    default:
		setStart(false);
		throw TError(nodePath().c_str(),_("Error operation %c(%xh). Function '%s' stopped."),*cprg,*cprg,id().c_str());
	}
    }
}

void Func::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TFunction::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function: ")+name(),/*owner().DB().empty()?R_R_R_:*/RWRWR_,"root",SDAQ_ID,1,
	    "doc","User_API|Documents/User_API");
	if(owner().DB().size())
	    ctrMkNode("fld",opt,-1,"/func/st/timestamp",_("Date of modification"),R_R_R_,"root",SDAQ_ID,1,"tp","time");
	if(startStat())
	    ctrMkNode("fld",opt,-1,"/func/st/compileSt",_("Compilation state"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/func/cfg/NAME",_("Name"),owner().DB().empty()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"tp","str","len",i2s(limObjNm_SZ).c_str());
	ctrMkNode("fld",opt,-1,"/func/cfg/DESCR",_("Description"),owner().DB().empty()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "tp","str","cols","100","rows","5");
	ctrMkNode("fld",opt,-1,"/func/cfg/START",_("To start"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	ctrMkNode("fld",opt,-1,"/func/cfg/MAXCALCTM",_("Maximum calculation time, seconds"),RWRWR_,"root",SDAQ_ID,3,
	    "tp","dec","min","0","max","3600");
	if(ctrMkNode("area",opt,-1,"/io",_("Program"))) {
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),RWRWR_,"root",SDAQ_ID,1,"s_com","add,del,ins,move")) {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Identifier"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),RWRWR_,"root",SDAQ_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d",
			IO::Real,IO::Integer,IO::Boolean,IO::String,IO::String|(IO::TransltText<<8),IO::String|(IO::FullText<<8),IO::String|((IO::FullText|IO::TransltText)<<8),IO::Object).c_str(),
		    "sel_list",_("Real;Integer;Boolean;String;String (translate);Text;Text (translate);Object"));
		ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),RWRWR_,"root",SDAQ_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d",IO::Default,IO::Output,IO::Return).c_str(),
		    "sel_list",_("Input;Output;Return"));
		ctrMkNode("list",opt,-1,"/io/io/4",_("Hidden"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/io/io/5",_("Default"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	    if(!owner().DB().empty())
		ctrMkNode("fld",opt,-1,"/io/prog_tr",cfg("PR_TR").fld().descr().c_str(),RWRW__,"root",SDAQ_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/io/prog",cfg("FORMULA").fld().descr().c_str(),RWRW__,"root",SDAQ_ID,3,"tp","str","rows","10","SnthHgl","1");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/func/st/timestamp" && ctrChkNode(opt)) opt->setText(i2s(timeStamp()));
    else if(a_path == "/func/st/compileSt" && ctrChkNode(opt))
	opt->setText(TSYS::strMess(_("Size source=%s, bytecode=%s; Registers totally=%d, constants=%d, internal functions'=%d; Functions external=%d, internal=%d."),
	    TSYS::cpct2str(prog().size()).c_str(),TSYS::cpct2str(prg.size()).c_str(),mRegs.size(),cntrCnst,cntrInFRegs,mFncs.size(),cntrInF));
    else if(a_path == "/func/cfg/NAME" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setName(opt->text());
    else if(a_path == "/func/cfg/DESCR" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setDescr(opt->text());
    else if(a_path == "/func/cfg/START") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(toStart()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setToStart(s2i(opt->text()));
    }
    else if(a_path == "/func/cfg/MAXCALCTM") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(maxCalcTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setMaxCalcTm(s2i(opt->text()));
    }
    else if(a_path == "/io/io") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    XMLNode *nId	= ctrMkNode("list",opt,-1,"/io/io/0","",RWRWR_);
	    XMLNode *nNm	= ctrMkNode("list",opt,-1,"/io/io/1","",RWRWR_);
	    XMLNode *nType	= ctrMkNode("list",opt,-1,"/io/io/2","",RWRWR_);
	    XMLNode *nMode	= ctrMkNode("list",opt,-1,"/io/io/3","",RWRWR_);
	    XMLNode *nHide	= ctrMkNode("list",opt,-1,"/io/io/4","",RWRWR_);
	    XMLNode *nDef	= ctrMkNode("list",opt,-1,"/io/io/5","",RWRWR_);
	    for(int id = 0; id < ioSize(); id++) {
		if(nId)   nId->childAdd("el")->setText(io(id)->id());
		if(nNm)   nNm->childAdd("el")->setText(io(id)->name());
		if(nType) nType->childAdd("el")->setText(i2s(io(id)->type()|((io(id)->flg()&(IO::FullText|IO::TransltText))<<8)));
		if(nMode) nMode->childAdd("el")->setText(i2s(io(id)->flg()&(IO::Output|IO::Return)));
		if(nHide) nHide->childAdd("el")->setText(io(id)->hide()?"1":"0");
		if(nDef)  nDef->childAdd("el")->setText(io(id)->def());
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    IO *ioPrev = ioSize() ? io(ioSize()-1) : NULL;
	    if(ioPrev) ioAdd(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~SysAttr)));
	    else ioAdd(new IO("new",_("New IO"),IO::Real,IO::Default));
	}
	if(ctrChkNode(opt,"ins",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int row = s2i(opt->attr("row"));
	    IO *ioPrev = row ? io(row-1) : NULL;
	    if(ioPrev) ioIns(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~SysAttr)), row);
	    else ioIns(new IO("new",_("New IO"),IO::Real,IO::Default), row);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	ioDel(s2i(opt->attr("row")));
	if(ctrChkNode(opt,"move",RWRWR_,"root",SDAQ_ID,SEC_WR))	ioMove(s2i(opt->attr("row")), s2i(opt->attr("to")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int row = s2i(opt->attr("row"));
	    int col = s2i(opt->attr("col"));
	    if((col == 0 || col == 1) && !opt->text().size())
		throw TError(nodePath().c_str(),_("Empty value is not allowed."));
	    switch(col) {
		case 0:	io(row)->setId(opt->text());	break;
		case 1:	io(row)->setName(opt->text());	break;
		case 2:
		    io(row)->setType((IO::Type)(s2i(opt->text())&0xFF));
		    io(row)->setFlg(io(row)->flg()^((io(row)->flg()^(s2i(opt->text())>>8))&(IO::FullText|IO::TransltText)));
		    break;
		case 3:	io(row)->setFlg(io(row)->flg()^((io(row)->flg()^s2i(opt->text()))&(IO::Output|IO::Return)));	break;
		case 4:	io(row)->setHide(s2i(opt->text()));	break;
		case 5:	io(row)->setDef(opt->text());	break;
	    }
	    if(!owner().DB().empty()) modif();
	}
    }
    else if(a_path == "/io/prog_tr") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(progTr()));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))	setProgTr(s2i(opt->text()));
    }
    else if(a_path == "/io/prog") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(prog());
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))	{ setProg(opt->text()); progCompile(); }
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SDAQ_ID,SEC_RD)) mod->compileFuncSynthHighl("JavaScript",*opt);
    }
    else TFunction::cntrCmdProc(opt);
}

//*************************************************
//* Reg                                           *
//*************************************************
Reg::~Reg( )	{ setType(Free); }

Reg &Reg::operator=( const Reg &irg )
{
    setType(irg.type());
    switch(type()) {
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
    switch(mTp) {
	case Reg::String:	delete el.s;	break;
	case Reg::Obj:		delete el.o;	break;
	case Reg::PrmAttr:	delete el.pA;	break;
	default:	break;
    }
    //Set new type
    switch(tp) {
	case Reg::String:	el.s = new string;		break;
	case Reg::Obj:		el.o = new AutoHD<TVarObj>;	break;
	case Reg::PrmAttr:	el.pA = new AutoHD<TVal>;	break;
	default:	break;
    }
    mTp = tp;
}

Reg::Type Reg::vType( Func *fnc )
{
    switch(type()) {
	case Free: return Int;
	case Var:
	    switch(fnc->io(val().io)->type()) {
		case IO::String:	return String;
		case IO::Boolean:	return Bool;
		case IO::Integer:	return Int;
		case IO::Real:		return Real;
		case IO::Object:	return Obj;
	    }
	case PrmAttr:
	    switch(val().pA->at().fld().type()) {
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

void Reg::free( )
{
    if(lock()) return;

    setType(Free);
    mNm.clear();
    mObjEl = mLock = false;
}

//*************************************************
//* RegW : Work register                          *
//*************************************************
RegW::RegW( ) : mTp(Reg::Free), mConst(false)
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter("DAQ:JavaLikeCalc:RegW", 1);
}

RegW::RegW( const RegW &iRW ) : mTp(Reg::Free), mConst(false)
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter("DAQ:JavaLikeCalc:RegW", 1);

    operator=(iRW);
}

RegW::~RegW( )
{
    setType(Reg::Free);

    if(mess_lev() == TMess::Debug) SYS->cntrIter("DAQ:JavaLikeCalc:RegW", -1);
}

RegW &RegW::operator=( const RegW &iRW )
{
    setType(iRW.type());

    switch(iRW.type()) {
	case Reg::Free: case Reg::Dynamic:	break;
	case Reg::Bool:	el.b = iRW.el.b;	break;
	case Reg::Int:	el.i = iRW.el.i;	break;
	case Reg::Real:	el.r = iRW.el.r;	break;
	case Reg::String: *el.s = *iRW.el.s;	break;
	case Reg::Obj:	*el.o = *iRW.el.o;	break;
	case Reg::Var:	el.io = iRW.el.io;	break;
	case Reg::PrmAttr: *el.pA = *iRW.el.pA;	break;
	case Reg::Function: *el.f = *iRW.el.f;	break;
    }

    mPrps = iRW.mPrps;
    mConst = iRW.mConst;

    return *this;
}

void RegW::operator=( const TVariant &ivar )
{
    switch(ivar.type()) {
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
    switch(mTp) {
	case Reg::String:	delete el.s;	break;
	case Reg::Obj:		delete el.o;	break;
	case Reg::PrmAttr:	delete el.pA;	break;
	case Reg::Function:	delete el.f;	break;
	default:	break;
    }
    //Set new type
    switch(tp) {
	case Reg::String:	el.s = new string;		break;
	case Reg::Obj:		el.o = new AutoHD<TVarObj>(new TVarObj()); break;
	case Reg::PrmAttr:	el.pA = new AutoHD<TVal>;	break;
	case Reg::Function:	el.f = new AutoHD<TFunction>;	break;
	default:	break;
    }
    mTp = tp;
}

Reg::Type RegW::vType( Func *fnc )
{
    switch(type()) {
	case Reg::Free: return Reg::Int;
	case Reg::Var:
	    switch(fnc->io(val().io)->type()) {
		case IO::String:	return Reg::String;
		case IO::Boolean:	return Reg::Bool;
		case IO::Integer:	return Reg::Int;
		case IO::Real:		return Reg::Real;
		case IO::Object:	return Reg::Obj;
	    }
	    break;
	case Reg::PrmAttr:
	    switch(val().pA->at().fld().type()) {
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

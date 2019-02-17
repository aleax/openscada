
//OpenSCADA module DAQ.JavaLikeCalc file: virtual.cpp
/***************************************************************************
 *   Copyright (C) 2005-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>

#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>

#include "freefunc.h"
#include "virtual.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"JavaLikeCalc"
#define MOD_NAME	_("Calculator on the Java-like language")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define SUB_TYPE	"LIB"
#define MOD_VER		"4.1.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides a calculator and libraries engine on the Java-like language.\
 The user can create and modify functions and their libraries.")
#define LICENSE		"GPL2"
//*************************************************

JavaLikeCalc::TpContr *JavaLikeCalc::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_JavaLikeCalc_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_JavaLikeCalc_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))	return new JavaLikeCalc::TpContr(source);
	return NULL;
    }
}

using namespace JavaLikeCalc;

//*************************************************
//* TpContr                                      *
//*************************************************
TpContr::TpContr( string src ) : TTypeDAQ(MOD_ID), mSafeTm(10)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, src);

    mLib = grpAdd("lib_");
}

TpContr::~TpContr( )
{
    nodeDelAll();
}

void TpContr::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("HighPriority");
}

string TpContr::modInfo( const string &iname )
{
    string name = TSYS::strParse(iname, 0, ":");

    if(name == "HighPriority")	return "1";

    return TModule::modInfo(name);
}

void TpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable( flag );

    //Controller db structure
    fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"60","system"));
    fldAdd(new TFld("FUNC",_("Controller function or DAQ-template"),TFld::String,TFld::NoFlag,"40"));
    fldAdd(new TFld("SCHEDULE",_("Calculation schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Priority of the calculation task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("ITER",_("Number of iterations in single calculation"),TFld::Integer,TFld::NoFlag,"2","1","1;99"));

    //Controller value db structure
    val_el.fldAdd(new TFld("ID",_("IO ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    val_el.fldAdd(new TFld("VAL",_("IO value"),TFld::String,TFld::NoFlag,"10000"));

    //Add parameter types
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("FLD",_("Data fields"),TFld::String,TFld::FullText|TCfg::NoVal,"300"));

    //Lib's db structure
    lb_el.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    lb_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    lb_el.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::TransltText,"300"));
    lb_el.fldAdd(new TFld("DB",_("Database"),TFld::String,TFld::NoFlag,"30"));

    //Function's structure
    fnc_el.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    fnc_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    fnc_el.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::TransltText,"300"));
    fnc_el.fldAdd(new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1","1"));
    fnc_el.fldAdd(new TFld("MAXCALCTM",_("Maximum calculate time, seconds"),TFld::Integer,TFld::NoFlag,"4","10","0;3600"));
    fnc_el.fldAdd(new TFld("PR_TR",_("Completely translate the program"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fnc_el.fldAdd(new TFld("FORMULA",_("Program"),TFld::String,TFld::TransltText,"1000000"));
    fnc_el.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //Function's IO structure
    fncio_el.fldAdd(new TFld("F_ID",_("Function ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    fncio_el.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    fncio_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    fncio_el.fldAdd(new TFld("TYPE",_("Type"),TFld::Integer,TFld::NoFlag,"1"));
    fncio_el.fldAdd(new TFld("MODE",_("Mode"),TFld::Integer,TFld::NoFlag,"1"));
    fncio_el.fldAdd(new TFld("DEF",_("Default value"),TFld::String,TFld::TransltText,"20"));
    fncio_el.fldAdd(new TFld("HIDE",_("Hide"),TFld::Boolean,TFld::NoFlag,"1"));
    fncio_el.fldAdd(new TFld("POS",_("Position"),TFld::Integer,TFld::NoFlag,"3"));

    //Init named constant table
    double rvl;
    rvl = M_PI; mConst.push_back(NConst(TFld::Real,"pi",string((char*)&rvl,sizeof(rvl))));
    rvl = M_E;  mConst.push_back(NConst(TFld::Real,"e",string((char*)&rvl,sizeof(rvl))));
    rvl = EVAL_REAL;
    mConst.push_back(NConst(TFld::Real,"EVAL_REAL",string((char*)&rvl,sizeof(rvl))));
    mConst.push_back(NConst(TFld::Real,"EVAL",string((char*)&rvl,sizeof(rvl))));
    int64_t ivl;
    ivl = EVAL_INT; mConst.push_back(NConst(TFld::Integer,"EVAL_INT",string((char*)&ivl,sizeof(ivl))));
    ivl = EVAL_BOOL; mConst.push_back(NConst(TFld::Boolean,"EVAL_BOOL",string((char*)&ivl,1)));

    mConst.push_back(NConst(TFld::String,"EVAL_STR",EVAL_STR));

    //Init buildin functions list
    mBFunc.push_back(BFunc("sin",Reg::FSin,1));
    mBFunc.push_back(BFunc("cos",Reg::FCos,1));
    mBFunc.push_back(BFunc("tan",Reg::FTan,1));
    mBFunc.push_back(BFunc("sinh",Reg::FSinh,1));
    mBFunc.push_back(BFunc("cosh",Reg::FCosh,1));
    mBFunc.push_back(BFunc("tanh",Reg::FTanh,1));
    mBFunc.push_back(BFunc("asin",Reg::FAsin,1));
    mBFunc.push_back(BFunc("acos",Reg::FAcos,1));
    mBFunc.push_back(BFunc("atan",Reg::FAtan,1));
    mBFunc.push_back(BFunc("rand",Reg::FRand,1));
    mBFunc.push_back(BFunc("lg",Reg::FLg,1));
    mBFunc.push_back(BFunc("ln",Reg::FLn,1));
    mBFunc.push_back(BFunc("exp",Reg::FExp,1));
    mBFunc.push_back(BFunc("pow",Reg::FPow,2));
    mBFunc.push_back(BFunc("min",Reg::FMin,2));
    mBFunc.push_back(BFunc("max",Reg::FMax,2));
    mBFunc.push_back(BFunc("sqrt",Reg::FSqrt,1));
    mBFunc.push_back(BFunc("abs",Reg::FAbs,1));
    mBFunc.push_back(BFunc("sign",Reg::FSign,1));
    mBFunc.push_back(BFunc("ceil",Reg::FCeil,1));
    mBFunc.push_back(BFunc("floor",Reg::FFloor,1));
    mBFunc.push_back(BFunc("typeof",Reg::FTypeOf,1));
    mBFunc.push_back(BFunc("tr",Reg::FTr,1));
}

TController *TpContr::ContrAttach( const string &name, const string &daq_db )	{ return new Contr(name,daq_db,this); }

bool TpContr::compileFuncLangs( vector<string> *ls )
{
    if(ls) {
	ls->clear();
	ls->push_back("JavaScript");
    }

    return true;
}

void TpContr::compileFuncSynthHighl( const string &lang, XMLNode &shgl )
{
    if(lang == "JavaScript") {
	shgl.setAttr("font","Courier");
	//shgl.childAdd("rule")->setAttr("expr","(\"\"|\".*[^\\\\](|\\\\{2}|\\\\{4}|\\\\{6}|\\\\{8})\")")->setAttr("min","1")->setAttr("color","darkgreen")->
	shgl.childAdd("rule")->setAttr("expr","(\"(|\\\\{2}|\\\\{4}|\\\\{6}|\\\\{8})\"|\".*[^\\\\](|\\\\{2}|\\\\{4}|\\\\{6}|\\\\{8})\")")->setAttr("min","1")->setAttr("color","darkgreen")->
	     childAdd("rule")->setAttr("expr","\\\\([xX][a-zA-Z0-9]{2}|[0-7]{3}|.{1})")->setAttr("color","green")->setAttr("font_weight","1");
	shgl.childAdd("blk")->setAttr("beg","/\\*")->setAttr("end","\\*/")->setAttr("color","gray")->setAttr("font_italic","1");
	shgl.childAdd("rule")->setAttr("expr","//.*$")->setAttr("color","gray")->setAttr("font_italic","1");
	shgl.childAdd("rule")->setAttr("expr","\\b(if|else|for|while|using|new|delete|break|continue|return|function|Array|Object|RegExp)\\b")->setAttr("color","darkblue")->setAttr("font_weight","1");
	shgl.childAdd("rule")->setAttr("expr","\\b(var|in)(?=\\s+\\w)")->setAttr("color","darkblue")->setAttr("font_weight","1");
	shgl.childAdd("rule")->setAttr("expr","(\\?|\\:)")->setAttr("color","darkblue")->setAttr("font_weight","1");
	shgl.childAdd("rule")->setAttr("expr","\\b(0[xX][0-9a-fA-F]*|[0-9]*\\.?[0-9]+|[0-9]*\\.?[0-9]+[eE][-+]?[0-9]*|true|false)\\b")->setAttr("color","darkorange");
	shgl.childAdd("rule")->setAttr("expr","(\\=|\\!|\\+|\\-|\\>|\\<|\\*|\\/|\\%|\\||\\&|\\^|\\~)")->setAttr("color","darkblue")->setAttr("font_weight","1");
	shgl.childAdd("rule")->setAttr("expr","(\\;|\\,|\\{|\\}|\\[|\\]|\\(|\\))")->setAttr("color","blue");
    }
}

string TpContr::compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text, const string &usings, int maxCalcTm )
{
    if(lang != "JavaScript") throw TError(nodePath().c_str(),_("Compilation with the help of the program language %s is not supported."),lang.c_str());
    if(!lbPresent("sys_compile")) lbReg(new Lib("sys_compile","",""));

    //Function id generation for "<auto>" or call nodePath() for it
    string funcId = fnc_cfg.id();
    ResAlloc res(parseRes(), true);
    if(funcId == "<auto>") {
	funcId = "Auto";
	for(int iP = 1; lbAt("sys_compile").at().present(funcId); ++iP) funcId = TSYS::strMess("Auto%d",iP);
    } else funcId = fnc_cfg.nodePath('_', true);

    // Connect or use allowed compiled function object
    if(!lbAt("sys_compile").at().present(funcId)) lbAt("sys_compile").at().add(funcId, "");
    res.release();

    AutoHD<Func> func = lbAt("sys_compile").at().at(funcId);
    if(maxCalcTm > 0) func.at().setMaxCalcTm(maxCalcTm);

    //Try for hot config fields changing of the work function
    if(func.at().use() && func.at().startStat())
	try {
	    ((TFunction&)func.at()).operator=(fnc_cfg);
	    if(prog_text == func.at().prog()) return func.at().nodePath(0, true);
	} catch(TError &err) {
	    func.at().setStart(true);
	    throw;
	}
    //Standard compile
    bool started = func.at().startStat();
    string  prevProc = func.at().prog(),
	    prevUsings = func.at().usings();
    try {
	if(started) func.at().setStart(false);
	func.at().setProg(prog_text.c_str());
	func.at().setUsings(usings);
	((TFunction&)func.at()).operator=(fnc_cfg);
	func.at().setStart(true);
	func.at().modifClr();
    } catch(TError &err) {
	if(!func.at().use()) {
	    func.free();
	    lbAt("sys_compile").at().del(funcId.c_str());
	}
	// Try of the restoring the previous procedure executing
	else if(started)
	    try {
		func.at().setProg(prevProc);
		func.at().setUsings(prevUsings);
		func.at().setStart(true);
	    } catch(TError&) { }

	throw TError((nodePath()+"sys_compile/"+funcId).c_str(), _("Error compiling: %s"), err.mess.c_str());
    }

    return func.at().nodePath(0, true);
}

void TpContr::load_( )
{
    //Load parameters from command line

    //Load parameters
    setSafeTm(s2i(TBDS::genDBGet(nodePath()+"SafeTm",i2s(safeTm()))));

    //Load function's libraries
    try {
	// Search and create new libraries
	TConfig cEl(&elLib());
	//cEl.cfgViewAll(false);
	vector<string> dbLs;
	map<string, bool> itReg;
	vector<vector<string> > full;

	// Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int libCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+libTable(),nodePath()+"lib",libCnt++,cEl,false,&full); ) {
		string l_id = cEl.cfg("ID").getS();
		if(!lbPresent(l_id)) {
		    lbReg(new Lib(l_id.c_str(),"",(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]));
		    try {
			lbAt(l_id).at().load(&cEl);
			//lbAt(l_id).at().setStart(true);		//Do not try start into the loading but possible broblems like into openscada --help
		    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
		}
		itReg[l_id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    lbList(dbLs);
	    for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
		if(itReg.find(dbLs[iIt]) == itReg.end() && SYS->chkSelDB(lbAt(dbLs[iIt]).at().DB()))
		    lbUnreg(dbLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Error loading for function libraries."));
    }
}

void TpContr::save_( )
{
    //Save parameters
    TBDS::genDBSet(nodePath()+"SafeTm",i2s(safeTm()));
}

void TpContr::modStart( )
{
    vector<string> lst;

    //Start functions
    lbList(lst);
    for(unsigned iLb = 0; iLb < lst.size(); iLb++)
	lbAt(lst[iLb]).at().setStart(true);

    TTypeDAQ::modStart();
}

void TpContr::modStop( )
{
    //Stop and disable all JavaLike-controllers
    vector<string> lst;
    list(lst);
    for(unsigned iL = 0; iL < lst.size(); iL++)
	at(lst[iL]).at().disable();

    //Stop functions
    lbList(lst);
    for(unsigned iLb = 0; iLb < lst.size(); iLb++)
	lbAt(lst[iLb]).at().setStart(false);
}

void TpContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTypeDAQ::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/lib_",_("Library"),RWRWR_,"root",SDAQ_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/prm",MOD_ID))
	    ctrMkNode("fld",opt,-1,"/prm/safeTm",_("Safety timeout, seconds"),RWRWR_,"root",SDAQ_ID,3,"tp","dec","min","0","max","3600");
	if(ctrMkNode("area",opt,1,"/libs",_("Functions libraries")))
	    ctrMkNode("list",opt,-1,"/libs/lb",_("Libraries"),RWRWR_,"root",SDAQ_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","lib_","idSz",OBJ_ID_SZ);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/safeTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(safeTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSafeTm(s2i(opt->text()));
    }
    else if(a_path == "/br/lib_" || a_path == "/libs/lb") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    vector<string> lst;
	    lbList(lst);
	    for(unsigned iA = 0; iA < lst.size(); iA++)
		opt->childAdd("el")->setAttr("id",lst[iA])->setText(lbAt(lst[iA]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))	lbReg(new Lib(TSYS::strEncode(opt->attr("id"),TSYS::oscdID).c_str(),opt->text().c_str(),"*.*"));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	lbUnreg(opt->attr("id"),1);
    }
    else TTypeDAQ::cntrCmdProc(opt);
}

NConst *TpContr::constGet( const char *nm )
{
    for(unsigned i_cst = 0; i_cst < mConst.size(); i_cst++)
	if(mConst[i_cst].name == nm) return &mConst[i_cst];
	    return NULL;
}

BFunc *TpContr::bFuncGet( const char *nm )
{
    for(unsigned i_bf = 0; i_bf < mBFunc.size(); i_bf++)
	if(mBFunc[i_bf].name == nm) return &mBFunc[i_bf];
	    return NULL;
}

//*************************************************
//* Contr: Controller object                      *
//*************************************************
Contr::Contr( string name_c, const string &daq_db, ::TElem *cfgelem ) :
    ::TController(name_c, daq_db, cfgelem), TPrmTempl::Impl(this, ("JavaLikeCalc_"+name_c).c_str()),
    prcSt(false), callSt(false), endrunReq(false), isDAQTmpl(false), chkLnkNeed(false),
    mPrior(cfg("PRIOR").getId()), mIter(cfg("ITER").getId()), idFreq(-1), idStart(-1), idStop(-1), mPer(0)
{
    cfg("PRM_BD").setS("JavaLikePrm_"+name_c);
}

Contr::~Contr( )
{

}

TCntrNode &Contr::operator=( const TCntrNode &node )
{
    TController::operator=(node);

    Contr *src_n = const_cast<Contr*>(dynamic_cast<const Contr*>(&node));
    if(!src_n || !src_n->enableStat() || !enableStat()) return *this;

    //IO values copy
    for(int iIO = 0; iIO < src_n->ioSize(); iIO++)
	if(isDAQTmpl && src_n->ioFlg(iIO)&TPrmTempl::CfgLink)
	    lnkAddrSet(iIO, src_n->lnkAddr(iIO));
	else setS(iIO, src_n->getS(iIO));

    if(isDAQTmpl) chkLnkNeed = initLnks();

    return *this;
}

void Contr::postDisable( int flag )
{
    try {
	if(flag) {
	    //Delete IO value's table
	    string db = DB()+"."+TController::id()+"_val";
	    SYS->db().at().open(db);
	    SYS->db().at().close(db,true);
	}
    } catch(TError &err) { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }

    TController::postDisable(flag);
}

string Contr::getStatus( )
{
    string val = TController::getStatus();

    if(isDAQTmpl)	val += _("From DAQ template. ");
    if(startStat() && !redntUse()) {
	if(callSt)	val += TSYS::strMess(_("Calculation. "));
	if(period())	val += TSYS::strMess(_("Calculation with the period: %s. "),tm2s(1e-9*period()).c_str());
	else val += TSYS::strMess(_("Next calculation by the cron '%s'. "),atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	val += TSYS::strMess(_("Spent time: %s[%s]."), tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(),
						       tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str());
    }

    return val;
}

void Contr::enable_( )
{
    isDAQTmpl = false;
    string  lfnc = TSYS::strSepParse(fnc(), 0, '.'),
	    wfnc = TSYS::strSepParse(fnc(), 1, '.');
    if(wfnc == _("{NewFunction}")) throw TError(nodePath().c_str(), _("Enter your new function name instead '%s'!"), wfnc.c_str());
    if(lfnc.empty() || wfnc.empty() || !(mod->lbPresent(lfnc) || SYS->daq().at().tmplLibPresent(lfnc)))
	throw TError(nodePath().c_str(),_("Function or DAQ template '%s' is not present or empty."), fnc().c_str());

    //Try JavaLikeCalc function
    if(mod->lbPresent(lfnc) && mod->lbAt(lfnc).at().present(wfnc))
	setFunc(&mod->lbAt(lfnc).at().at(wfnc).at());
    //Try DAQ template
    else if(SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc)) {
	setFunc(&SYS->daq().at().tmplLibAt(lfnc).at().at(wfnc).at().func().at());
	isDAQTmpl = true;
    }
    //Create new JavaLikeCalc function
    else if(mod->lbPresent(lfnc)) {
	mess_info(nodePath().c_str(), _("Creating a new function '%s'."), fnc().c_str());
	mod->lbAt(lfnc).at().add(wfnc);
	setFunc(&mod->lbAt(lfnc).at().at(wfnc).at());
    }
    else throw TError(nodePath().c_str(), _("Error path of the function or DAQ template '%s'."), fnc().c_str());

    try {
	setVfName(id()+"_JavaLikeCntr");
	loadFunc();
    }
    catch(TError &err) {
	mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	mess_warning(nodePath().c_str(), _("Error loading function and its IO."));
    }
}

void Contr::disable_( )
{
    cleanLnks(true);
}

void Contr::load_( )
{
    //TController::load_();

    loadFunc();
}

void Contr::loadFunc( bool onlyVl )
{
    if(func()) {
	if(!onlyVl && !isDAQTmpl) ((Func*)func())->load();

	// Init attrubutes
	if(isDAQTmpl)	addLinksAttrs();

	//Creating special IO
	if(!isDAQTmpl) {
	    if(func()->ioId("f_frq") < 0)	func()->ioIns(new IO("f_frq",_("Frequency of calculation of the function (Hz)"),IO::Real,0,"1000",false),0);
	    if(func()->ioId("f_start") < 0)	func()->ioIns(new IO("f_start",_("Function start flag"),IO::Boolean,0,"0",false),1);
	    if(func()->ioId("f_stop") < 0)	func()->ioIns(new IO("f_stop",_("Function stop flag"),IO::Boolean,0,"0",false),2);
	    if(func()->ioId("this") < 0)	func()->ioIns(new IO("this",_("Link to the object of this controller"),IO::Object,0,"0",false),3);
	}

	//Load values
	TConfig cfg(&mod->elVal());
	string bd_tbl = id()+"_val";
	string bd = DB()+"."+bd_tbl;
	vector<vector<string> > full;

	for(int fldCnt = 0; SYS->db().at().dataSeek(bd,mod->nodePath()+bd_tbl,fldCnt++,cfg,false,&full); ) {
	    int ioId = func()->ioId(cfg.cfg("ID").getS());
	    if(ioId < 0 || (!isDAQTmpl && (func()->io(ioId)->flg()&Func::SysAttr))) continue;
	    if(!isDAQTmpl) setS(ioId, cfg.cfg("VAL").getS());
	    else {
		if(func()->io(ioId)->flg()&TPrmTempl::CfgLink)
		    lnkAddrSet(ioId, cfg.cfg("VAL").getS(TCfg::ExtValOne));	//Force no translated
		else if(func()->io(ioId)->type() != IO::String) setS(ioId, cfg.cfg("VAL").getS(TCfg::ExtValOne));		//Force no translated
		else setS(ioId, cfg.cfg("VAL").getS());
	    }
	}
	if(isDAQTmpl) chkLnkNeed = initLnks();
    }
}

void Contr::postIOCfgChange( )
{
    TValFunc::postIOCfgChange();

    loadFunc(true);
}

void Contr::save_( )
{
    TController::save_();

    if(func() != NULL) {
	if(!isDAQTmpl)	((Func*)func())->save();

	//Save values
	TConfig cfg(&mod->elVal());
	string bd_tbl = id()+"_val";
	string val_bd = DB()+"."+bd_tbl;
	for(int iio = 0; iio < ioSize(); iio++) {
	    if(!isDAQTmpl && func()->io(iio)->flg()&Func::SysAttr) continue;
	    cfg.cfg("ID").setS(func()->io(iio)->id());
	    if(!isDAQTmpl) cfg.cfg("VAL").setS(getS(iio));
	    else {
		cfg.cfg("VAL").setNoTransl(!(func()->io(iio)->type()==IO::String && !(func()->io(iio)->flg()&TPrmTempl::CfgLink)));
		if(func()->io(iio)->flg()&TPrmTempl::CfgLink)	cfg.cfg("VAL").setS(lnkAddr(iio));
		else cfg.cfg("VAL").setS(getS(iio));
	    }
	    SYS->db().at().dataSet(val_bd, mod->nodePath()+bd_tbl, cfg);
	}

	//Clear VAL
	vector<vector<string> > full;
	cfg.cfgViewAll(false);
	for(int fldCnt = 0; SYS->db().at().dataSeek(val_bd,mod->nodePath()+bd_tbl,fldCnt++,cfg,false,&full); )
	    if(ioId(cfg.cfg("ID").getS()) < 0) {
		if(!SYS->db().at().dataDel(val_bd,mod->nodePath()+bd_tbl,cfg,true,false,true))	break;
		if(full.empty()) fldCnt--;
	    }
    }
}

void Contr::start_( )
{
    callSt = false;
    ((Func *)func())->setStart(true);

    //Link to special atributes
    idFreq	= ioId("f_frq");
    idStart	= ioId("f_start");
    idStop	= ioId("f_stop");
    int id_this = ioId("this");
    if(id_this >= 0) setO(id_this, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));

    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    //Start the request data task
    SYS->taskCreate(nodePath('.',true), mPrior, Contr::Task, this);
}

void Contr::stop_( )
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrunReq);
}

void *Contr::Task( void *icntr )
{
    Contr &cntr = *(Contr *)icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    bool is_start = true;
    bool is_stop  = false;
    int64_t t_cnt, t_prev = TSYS::curTime();

    while(true) {
	if(!cntr.redntUse()) {
	    if(cntr.chkLnkNeed)	cntr.chkLnkNeed = cntr.initLnks(true);

	    cntr.callSt = true;
	    t_cnt = TSYS::curTime();
	    //Setting special IO
	    if(cntr.idFreq >= 0)  cntr.setR(cntr.idFreq, cntr.period()?((float)cntr.iterate()*1e9/(float)cntr.period()):(-1e-6*(t_cnt-t_prev)));
	    if(cntr.idStart >= 0) cntr.setB(cntr.idStart, is_start || cntr.isChangedProg(true));
	    if(cntr.idStop >= 0)  cntr.setB(cntr.idStop, is_stop);

	    //Get input links
	    if(cntr.isDAQTmpl) cntr.inputLinks();

	    for(int iIt = 0; iIt < cntr.mIter; iIt++)
		try {
		    cntr.setMdfChk(true);
		    cntr.calc();
		}
		catch(TError &err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str() );
		    mess_err(cntr.nodePath().c_str(),_("Error calculating controller function."));
		}

	    //Put output links
	    if(cntr.isDAQTmpl) cntr.outputLinks();
	    t_prev = t_cnt;
	    cntr.callSt = false;
	}

	if(is_stop) break;
	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
	if(cntr.endrunReq) is_stop = true;
	is_start = false;
	cntr.modif();
    }

    cntr.prcSt = false;

    return NULL;
}

void Contr::redntDataUpdate( )
{
    TController::redntDataUpdate( );

    //Request for template's attributes values
    XMLNode req("get"); req.setAttr("path",nodePath(0,true)+"/%2fserv%2ffncAttr");

    //Send request to first active station for this controller
    if(owner().owner().rdStRequest(workId(),req).empty()) return;

    //Redirect respond to local controller
    req.setName("set")->setAttr("path","/%2fserv%2ffncAttr");
    cntrCmd(&req);
}


TParamContr *Contr::ParamAttach( const string &name, int type )	{ return new Prm(name,&owner().tpPrmAt(type)); }

void Contr::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/") {
	if(a_path == "/serv/fncAttr") {
	    if(!startStat() || !func()) throw TError(nodePath().c_str(),_("Function is missing or not started."));
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
		for(int iA = 0; iA < ioSize(); iA++)
		    opt->childAdd("a")->setAttr("id",func()->io(iA)->id())->setText(getS(iA));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		for(unsigned iA = 0; iA < opt->childSize(); iA++) {
		    int io_id = -1;
		    if(opt->childGet(iA)->name() != "a" || (io_id=ioId(opt->childGet(iA)->attr("id"))) < 0) continue;
		    setS(io_id,opt->childGet(iA)->text());
		}
	}
	else TController::cntrCmdProc(opt);
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	opt->childGet(0)->setAttr("doc", "Documents/User_API|Documents/User_API");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/FUNC",cfg("FUNC").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","sel_ed","select","/cntr/flst");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	if(enableStat())
	    ctrMkNode("comm",opt,-1,"/cntr/cfg/toFunc",_("Go to the used function"),R_R_R_,"root",SDAQ_ID,1,"tp","lnk");
	if(enableStat() && ctrMkNode("area",opt,-1,"/fnc",_("Calculation"))) {
	    if(ctrMkNode("table",opt,-1,"/fnc/io",_("Data"),RWRWR_,"root",SDAQ_ID,1,"s_com",(isDAQTmpl?"":"add,del,ins,move")/*,"rows","15"*/)) {
		ctrMkNode("list",opt,-1,"/fnc/io/0",_("Identifier"),isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/fnc/io/1",_("Name"),isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/fnc/io/2",_("Type"),isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d;%d;%d;%d",IO::Real,IO::Integer,IO::Boolean,IO::String,IO::String|(IO::FullText<<8),IO::Object).c_str(),
		    "sel_list",_("Real;Integer;Boolean;String;Text;Object"));
		ctrMkNode("list",opt,-1,"/fnc/io/3",_("Attribute mode"),isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d",IO::Default,IO::Output,IO::Return).c_str(),
		    "sel_list",_("Read only;Read and Write;Read and Write"));
		ctrMkNode("list",opt,-1,"/fnc/io/4",_("Value"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	    if(!isDAQTmpl)
		ctrMkNode("fld",opt,-1,"/fnc/prog",_("Program"),RWRW__,"root",SDAQ_ID,3,"tp","str","rows","10","SnthHgl","1");
	}
	if(isDAQTmpl && func()) TPrmTempl::Impl::cntrCmdProc(opt);
	return;
    }

    //Process command to page
    if(a_path == "/cntr/cfg/toFunc" && enableStat() && ctrChkNode(opt,"get",R_R_R_,"root",SDAQ_ID,SEC_RD))
	opt->setText(string("/DAQ/")+(isDAQTmpl?"tmplb_":MOD_ID "/lib_")+TSYS::strParse(fnc(),0,".")+"/"+TSYS::strParse(fnc(),1,"."));
    else if(a_path == "/cntr/flst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText("");
	vector<string> lls, ls;
	//Functions
	mod->lbList(lls);
	for(unsigned iL = 0; iL < lls.size(); iL++) {
	    if(lls[iL] == "sys_compile") continue;
	    opt->childAdd("el")->setText(lls[iL]+"."+_("{NewFunction}"));
	    mod->lbAt(lls[iL]).at().list(ls);
	    for(unsigned iT = 0; iT < ls.size(); iT++)
		opt->childAdd("el")->setText(lls[iL]+"."+ls[iT]);
	}
	opt->childAdd("el")->setText("");
	//Templates
	SYS->daq().at().tmplLibList(lls);
	for(unsigned iL = 0; iL < lls.size(); iL++) {
	    SYS->daq().at().tmplLibAt(lls[iL]).at().list(ls);
	    for(unsigned iT = 0; iT < ls.size(); iT++)
		opt->childAdd("el")->setText(lls[iL]+"."+ls[iT]);
	}
    }
    else if(a_path == "/fnc/io" && enableStat()) {
	if(ctrChkNode(opt,"get",isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/fnc/io/0","",isDAQTmpl?R_R_R_:RWRWR_);
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/fnc/io/1","",isDAQTmpl?R_R_R_:RWRWR_);
	    XMLNode *n_type	= ctrMkNode("list",opt,-1,"/fnc/io/2","",isDAQTmpl?R_R_R_:RWRWR_);
	    XMLNode *n_mode	= ctrMkNode("list",opt,-1,"/fnc/io/3","",isDAQTmpl?R_R_R_:RWRWR_);
	    XMLNode *n_val	= ctrMkNode("list",opt,-1,"/fnc/io/4","",RWRWR_);

	    for(int id = 0; id < func()->ioSize(); id++) {
		if(n_id)	n_id->childAdd("el")->setText(func()->io(id)->id());
		if(n_nm)	n_nm->childAdd("el")->setText(func()->io(id)->name());
		if(n_type)	n_type->childAdd("el")->setText(i2s(func()->io(id)->type()|((func()->io(id)->flg()&IO::FullText)<<8)));
		if(n_mode)	n_mode->childAdd("el")->setText(i2s(func()->io(id)->flg()&(IO::Output|IO::Return)));
		if(n_val)	n_val->childAdd("el")->setText(getS(id));
	    }
	}
	if(ctrChkNode(opt,"add",isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    IO *ioPrev = ((Func*)func())->ioSize() ? ((Func*)func())->io(((Func*)func())->ioSize()-1) : NULL;
	    if(ioPrev) ((Func*)func())->ioAdd(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~Func::SysAttr)));
	    else ((Func*)func())->ioAdd(new IO("new","New IO",IO::Real,IO::Default));
	    modif();
	}
	if(ctrChkNode(opt,"ins",isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int row = s2i(opt->attr("row"));
	    IO *ioPrev = row ? ((Func*)func())->io(row-1) : NULL;
	    if(ioPrev) ((Func*)func())->ioIns(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~Func::SysAttr)), row);
	    else ((Func*)func())->ioIns(new IO("new","New IO",IO::Real,IO::Default), row);
	    modif();
	}
	if(ctrChkNode(opt,"del",isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int row = s2i(opt->attr("row"));
	    if(func()->io(row)->flg()&Func::SysAttr)
		throw TError(nodePath().c_str(),_("Deleting a locked attribute is not allowed."));
	    ((Func *)func())->ioDel(row);
	    modif();
	}
	if(ctrChkNode(opt,"move",isDAQTmpl?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_WR))
	{ ((Func *)func())->ioMove( s2i(opt->attr("row")), s2i(opt->attr("to")) ); modif(); }
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int row = s2i(opt->attr("row"));
	    int col = s2i(opt->attr("col"));
	    if((col == 0 || col == 1) && !opt->text().size())
		throw TError(nodePath().c_str(),_("Empty value is not allowed."));
	    if(!isDAQTmpl && func()->io(row)->flg()&Func::SysAttr)
		throw TError(nodePath().c_str(),_("Changing the locked attribute is not allowed."));
	    switch(col) {
		case 0:
		    if(isDAQTmpl)	break;
		    func()->io(row)->setId(opt->text());
		    break;
		case 1:
		    if(isDAQTmpl)	break;
		    func()->io(row)->setName(opt->text());
		    break;
		case 2:
		    if(isDAQTmpl)	break;
		    func()->io(row)->setType((IO::Type)(s2i(opt->text())&0xFF));
                    func()->io(row)->setFlg(func()->io(row)->flg()^((func()->io(row)->flg()^(s2i(opt->text())>>8))&IO::FullText));
		    break;
		case 3:
		    if(isDAQTmpl)	break;
		    func()->io(row)->setFlg(func()->io(row)->flg()^((s2i(opt->text())^func()->io(row)->flg())&(IO::Output|IO::Return)));
		    break;
		case 4:
		    setS(row, opt->text());
		    if(isDAQTmpl) lnkOutput(row, opt->text());
		    break;
	    }
	    modif();
	    if(!((Func *)func())->owner().DB().empty()) ((Func *)func())->modif();
	}
    }
    else if(a_path == "/fnc/prog" && enableStat() && !isDAQTmpl) {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(((Func *)func())->prog());
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	    ((Func *)func())->setProg(opt->text().c_str());
	    ((Func *)func())->progCompile();
	    modif();
	}
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SDAQ_ID,SEC_RD))	mod->compileFuncSynthHighl("JavaScript",*opt);
    }
    else if(isDAQTmpl && func() && TPrmTempl::Impl::cntrCmdProc(opt))	;
    else TController::cntrCmdProc(opt);
}

//*************************************************
//* Prm                                           *
//*************************************************
Prm::Prm( string name, TTypeParam *tp_prm ) :
    TParamContr(name,tp_prm), v_el(name)
{

}

Prm::~Prm( )
{
    nodeDelAll();
}

void Prm::postEnable( int flag )
{
    TParamContr::postEnable( flag );
    if(!vlElemPresent(&v_el)) vlElemAtt(&v_el);
}

void Prm::enable( )
{
    if(enableStat())  return;

    //Check and delete no used fields
    for(unsigned i_fld = 0; i_fld < v_el.fldSize(); ) {
	string fel;
	for(int io_off = 0; (fel=TSYS::strSepParse(cfg("FLD").getS(),0,'\n',&io_off)).size(); )
	    if(TSYS::strSepParse(fel,0,':') == v_el.fldAt(i_fld).reserve()) break;
	if(fel.empty())
	    try {
		v_el.fldDel(i_fld);
		continue;
	    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	i_fld++;
    }

    //Init elements
    vector<string> pls;
    string mio, ionm, aid, anm;
    for(int ioOff = 0, ioOff1 = 0; (mio=TSYS::strSepParse(cfg("FLD").getS(),0,'\n',&ioOff)).size(); ) {
	if(mio[0] == '#') continue;
	ioOff1 = 0;
	ionm	= TSYS::strSepParse(mio, 0, ':', &ioOff1);
	aid	= TSYS::strSepParse(mio, 0, ':', &ioOff1);
	anm	= TSYS::strSepParse(mio, 0, ':', &ioOff1);
	if(aid.empty()) aid = ionm;

	int io_id = ((Contr &)owner()).ioId(ionm);
	if(io_id < 0 ||
		(((Contr &)owner()).isDAQTmpl && !(((Contr &)owner()).ioFlg(io_id)&(TPrmTempl::AttrRead|TPrmTempl::AttrFull))))
	    continue;

	unsigned flg = TVal::DirWrite|TVal::DirRead;
	if(((Contr &)owner()).ioFlg(io_id)&IO::FullText)		flg |= TFld::FullText;
	if((((Contr &)owner()).isDAQTmpl && !(((Contr &)owner()).ioFlg(io_id)&TPrmTempl::AttrFull)) ||
		(!((Contr &)owner()).isDAQTmpl && !(((Contr &)owner()).ioFlg(io_id)&(IO::Output|IO::Return))))
	    flg |= TFld::NoWrite;
	TFld::Type	tp  = TFld::type(((Contr &)owner()).ioType(io_id));
	if(!v_el.fldPresent(aid) || v_el.fldAt(v_el.fldId(aid)).type() != tp || v_el.fldAt(v_el.fldId(aid)).flg() != flg) {
	    if(v_el.fldPresent(aid)) v_el.fldDel(v_el.fldId(aid));
	    v_el.fldAdd(new TFld(aid.c_str(),"",tp,flg));
	}

	int el_id = v_el.fldId(aid);
	v_el.fldAt(el_id).setDescr( anm.empty() ? ((Contr &)owner()).func()->io(io_id)->name() : anm );
	v_el.fldAt(el_id).setReserve( ionm );

	pls.push_back(aid);
    }

    //Check and delete no used attrs
    for(unsigned i_fld = 0, i_p; i_fld < v_el.fldSize(); ) {
	for(i_p = 0; i_p < pls.size(); i_p++)
	    if(pls[i_p] == v_el.fldAt(i_fld).name()) break;
	if(i_p >= pls.size())
	    try {
		v_el.fldDel(i_fld);
		continue;
	    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	i_fld++;
    }

    TParamContr::enable();
}

void Prm::disable()
{
    if(!enableStat()) return;

    TParamContr::disable();
}

Contr &Prm::owner( ) const	{ return (Contr&)TParamContr::owner(); }

void Prm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat())	return;

    //Send to active reserve station
    if(owner().redntUse()) {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //Direct write
    try {
	int io_id = ((Contr &)owner()).ioId(vo.fld().reserve());
	if(io_id < 0) disable();
	else if(!((Contr&)owner()).lnkOutput(io_id,vl)) ((Contr&)owner()).set(io_id, vl);
    } catch(TError &err) { disable(); }
}

void Prm::vlGet( TVal &val )
{
    if(val.name() == "err") {
	if(!owner().startStat())val.setS(_("2:Calculation is stopped"),0,true);
	else if(!enableStat())	val.setS(_("1:Parameter is disabled"),0,true);
	else val.setS("0",0,true);
	return;
    }
    if(owner().redntUse()) return;
    try {
	int io_id = ((Contr &)owner()).ioId(val.fld().reserve());
	if(io_id < 0) disable();
	else val.set(enableStat()?owner().get(io_id):EVAL_STR,0,true);
    } catch(TError &err) { disable(); }
}

void Prm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr);
    val.arch().at().setPeriod(SYS->archive().at().valPeriod()*1000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

void Prm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/FLD",cfg("FLD").fld().descr(),RWRWR_,"root",SDAQ_ID,2,"SnthHgl","1",
	    "help",_("List of configuration of the attributes. List must be written by lines in the format: \"{io}[:{aid}[:{anm}]]\"\n"
	    "Where:\n"
	    "  io - IO of the computable function;\n"
	    "  aid - identifier of the created attribute;\n"
	    "  anm - name of the created attribute.\n"
	    "If 'aid' or 'anm' is not set then they will be generated from the selected IO of the function."));
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/FLD" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	opt->childAdd("rule")->setAttr("expr","^#[^\n]*")->setAttr("color","gray")->setAttr("font_italic","1");
	opt->childAdd("rule")->setAttr("expr","^[^:]*")->setAttr("color","darkblue");
	opt->childAdd("rule")->setAttr("expr","\\:")->setAttr("color","blue");
    }
    else TParamContr::cntrCmdProc(opt);
}

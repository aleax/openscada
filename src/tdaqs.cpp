
//OpenSCADA system file: tdaqs.cpp
/***************************************************************************
 *   Copyright (C) 2003-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include "tsys.h"
#include "tmess.h"
#include "tcontroller.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tdaqs.h"

using namespace OSCADA;

//*************************************************
//* TDAQS                                         *
//*************************************************
TDAQS::TDAQS( ) : TSubSYS(SDAQ_ID,_("Data Acquisition"),true), mElErr("Error"), mRdRestDtTm(1)
{
    mTmpLib = grpAdd("tmplb_");

    //Templates lib db structure
    mElLib.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mElLib.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mElLib.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"1000"));
    mElLib.fldAdd(new TFld("DB",_("Data base"),TFld::String,TFld::NoFlag,"30"));

    //Template DB structure
    mElTmpl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mElTmpl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mElTmpl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"1000"));
    mElTmpl.fldAdd(new TFld("MAXCALCTM",_("Maximum calculate time, seconds"),TFld::Integer,TFld::NoFlag,"4","10","0;3600"));
    mElTmpl.fldAdd(new TFld("PR_TR",_("Allow program translation"),TFld::Boolean,TFld::NoFlag,"1","0"));
    mElTmpl.fldAdd(new TFld("PROGRAM",_("Program"),TFld::String,TFld::TransltText,"1000000"));
    mElTmpl.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //Parameter template IO DB structure
    mElTmplIO.fldAdd(new TFld("TMPL_ID",_("Template identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mElTmplIO.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mElTmplIO.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mElTmplIO.fldAdd(new TFld("TYPE",_("Value type"),TFld::Integer,TFld::NoFlag,"1"));
    mElTmplIO.fldAdd(new TFld("FLAGS",_("Flags"),TFld::Integer,TFld::NoFlag,"4"));
    mElTmplIO.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::TransltText,"50"));
    mElTmplIO.fldAdd(new TFld("POS",_("Real position"),TFld::Integer,TFld::NoFlag,"4"));

    //Error attributes
    mElErr.fldAdd(new TFld("err",_("Error"),TFld::String,TFld::NoWrite|TVal::DirRead));

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TDAQS::~TDAQS( )
{
    nodeDelAll();

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TDAQS::objName( )		{ return TSubSYS::objName()+":TDAQS"; }

void TDAQS::rdActCntrList( vector<string> &ls, bool isRun )
{
    AutoHD<TController> cntr;
    ls.clear();
    vector<string> mls, cls;
    modList(mls);
    for(unsigned iM = 0; iM < mls.size(); iM++) {
	if(!at(mls[iM]).at().redntAllow()) continue;
	at(mls[iM]).at().list(cls);
	for(unsigned iC = 0; iC < cls.size(); iC++) {
	    cntr = at(mls[iM]).at().at(cls[iC]);
	    if(cntr.at().startStat() && (!isRun || !cntr.at().redntUse(TController::Any)))
		ls.push_back(cntr.at().workId());
	}
    }
}

void TDAQS::ctrListPrmAttr( XMLNode *opt, const string &l_prm, bool toPrm, char sep, const string &pref )
{
    int c_lv = 0;
    string c_path = "", c_el;
    vector<string> ls;
    opt->childAdd("el")->setText(pref+c_path);

    AutoHD<TCntrNode> DAQnd = this;
    const char *c_grp = "mod_";
    for(int c_off = 0; (sep && (c_el=TSYS::strSepParse(l_prm,0,sep,&c_off)).size()) ||
		   (!sep && (c_el=TSYS::pathLev(l_prm,0,true,&c_off)).size()); ++c_lv)
    {
	c_path += sep ? (c_lv?sep+c_el:c_el) : "/"+c_el;
	DAQnd = DAQnd.at().nodeAt(c_grp+c_el, 0, sep, 0, true);
	if(DAQnd.freeStat()) break;
	opt->childAdd("el")->setText(pref+c_path);
	c_grp = (c_lv == 0) ? "cntr_" : "prm_";
    }
    if(sep && c_lv) c_path += sep;
    if(!DAQnd.freeStat()) {
	DAQnd.at().chldList(DAQnd.at().grpId(c_grp), ls, true);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(pref+c_path+(sep?ls[i_l]:("/"+ls[i_l])));

	//Get attributes
	if(!toPrm && strcmp(c_grp,"prm_") == 0) {
	    DAQnd.at().chldList(DAQnd.at().grpId("a_"), ls, true);
	    if(ls.size()) opt->childAdd("el")->setText(_("=== Attributes ==="));
	    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
		opt->childAdd("el")->setText(pref+c_path+(sep?ls[i_l]:("/"+ls[i_l])));
	}
    }
}

void TDAQS::load_( )
{
    //Load parameters from command line
    if(s2i(SYS->cmdOpt("h")) || s2i(SYS->cmdOpt("help"))) fprintf(stdout, "%s", optDescr().c_str());

    map<string, bool>	itReg;
    vector<vector<string> > full;

    //Load templates libraries of parameter
    try {
	// Search and create new libraries
	TConfig cEl(&elLib());
	//cEl.cfgViewAll(false);
	vector<string> dbLs;

	// Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int libCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+tmplLibTable(),nodePath()+"tmplib",libCnt++,cEl,false,&full); ) {
		string l_id = cEl.cfg("ID").getS();
		if(!tmplLibPresent(l_id)) tmplLibReg(new TPrmTmplLib(l_id.c_str(),"",(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]));
		tmplLibAt(l_id).at().load(&cEl);
		itReg[l_id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    tmplLibList(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(tmplLibAt(dbLs[i_it]).at().DB()))
		    tmplLibUnreg(dbLs[i_it]);
        }
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Load template's libraries error."));
    }

    //Load parameters
    try {
	AutoHD<TTypeDAQ> wmod;
	vector<string> modLs, dbLs;

	modList(modLs);
	for(unsigned iMd = 0; iMd < modLs.size(); iMd++) {
	    wmod = at(modLs[iMd]);
	    TConfig gCfg(&wmod.at());
	    //gCfg.cfgViewAll(false);
	    itReg.clear();

	    // Search into DB and create new controllers
	    SYS->db().at().dbList(dbLs, true);
	    dbLs.push_back(DB_CFG);
	    for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
		for(int fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+subId()+"_"+wmod.at().modId(),wmod.at().nodePath()+"DAQ",fldCnt++,gCfg,false,&full); ) {
		    string mId = gCfg.cfg("ID").getS();
		    try {
			if(!wmod.at().present(mId)) wmod.at().add(mId,(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
			wmod.at().at(mId).at().load(&gCfg);
			itReg[mId] = true;
		    } catch(TError &err) {
			mess_err(err.cat.c_str(), "%s", err.mess.c_str());
			mess_sys(TMess::Error, _("Add controller '%s' error."), mId.c_str());
		    }
		}

	    //  Check for remove items removed from DB
	    wmod.at().list(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(wmod.at().at(dbLs[i_it]).at().DB()))
		    wmod.at().del(dbLs[i_it]);
	}
    } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    //Load parameters from config-file and SYS DB
    setRdRestDtTm(s2r(TBDS::genDBGet(nodePath()+"RdRestDtTm",r2s(rdRestDtTm()))));
}

void TDAQS::save_( )
{
    //Save parameters to SYS DB
    TBDS::genDBSet(nodePath()+"RdRestDtTm", r2s(rdRestDtTm()));
}

TVariant TDAQS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // bool funcCall(string progLang, TVarObj args, string prog, string fixId = "") -
    //    Call function text <prog> whith arguments <args> for program language <progLang>
    //    and with the fixed identifier <fixId> (automatic for this empty). Return "true" on a well call.
    //    For the fixed function recreate you need change the program or clean up <fixId> by the function original id.
    //  progLang - program procedure language or the stored function's address, after call with <store>;
    //  args - function arguments;
    //  prog - function text;
    //  fixId - two direction field of fixed identifier of the function;
    //          for the field empty the function id will be automatic and destroy at end,
    //          else the id will used on the function creation and replaced by an address to it.
    if(iid == "funcCall" && prms.size() >= 3 && prms[1].type() == TVariant::Object) {
	string fixId = (prms.size() >= 4) ? prms[3].getS() : "";
	string faddr;
	AutoHD<TFunction> wFnc; if(fixId.size()) wFnc = SYS->nodeAt(fixId, 0, 0, 0, true);
	AutoHD<TVarObj> args = prms[1].getO();
	vector<string> als; args.at().propList(als);
	TVariant aVal;

	try {
	    //New call environment preparing or modifying presented
	    if(wFnc.freeStat() || prms[2].getS() != wFnc.at().prog()) {
		// Update present function
		if(!wFnc.freeStat()) { wFnc.at().setStart(false); wFnc.at().setProg(prms[2].getS()); wFnc.at().setStart(true); }
		else {
		    string langMod = TSYS::strParse(prms[0].getS(), 0, ".");
		    if(!modPresent(langMod)) return false;

		    //Prepare arguments structure
		    TFunction argStr(fixId.size()?"uf_"+TSYS::strEncode(fixId,TSYS::oscdID):"<auto>");
		    for(unsigned iA = 0; iA < als.size(); iA++) {
			aVal = args.at().propGet(als[iA]);
			IO::Type tp = IO::String;
			switch(aVal.type()) {
			    case TVariant::Boolean:	tp = IO::Boolean;	break;
			    case TVariant::Integer:	tp = IO::Integer;	break;
			    case TVariant::Real:	tp = IO::Real;		break;
			    case TVariant::String:	tp = IO::String;	break;
			    case TVariant::Object:	tp = IO::Object;	break;
			    default:	break;
			}
			argStr.ioAdd(new IO(als[iA].c_str(),als[iA].c_str(),tp,IO::Default));
		    }

		    //Get function id and compile.
		    faddr = at(langMod).at().compileFunc(TSYS::strParse(prms[0].getS(),1,"."), argStr, prms[2].getS());
		    wFnc = SYS->nodeAt(faddr);
		}
	    }
	    //Prepare and execute
	    TValFunc wCtx("UserFunc", &wFnc.at(), true, user);

	    // Load values
	    for(unsigned iA = 0; iA < als.size(); iA++)
		switch((aVal=args.at().propGet(als[iA])).type()) {
		    case TVariant::Boolean:	wCtx.setB(iA, aVal.getB());	break;
		    case TVariant::Integer:	wCtx.setI(iA, aVal.getI());	break;
		    case TVariant::Real:	wCtx.setR(iA, aVal.getR());	break;
		    case TVariant::String:	wCtx.setS(iA, aVal.getS());	break;
		    case TVariant::Object:	wCtx.setO(iA, aVal.getO());	break;
		    default:	break;
		}

	    // Call function
	    wCtx.calc();

	    // Place the call results and remove function object.
	    for(int iA = 0; iA < wCtx.ioSize(); iA++)
		switch(wCtx.ioType(iA)) {
		    case IO::Boolean:	args.at().propSet(als[iA], wCtx.getB(iA));	break;
		    case IO::Integer:	args.at().propSet(als[iA], wCtx.getI(iA));	break;
		    case IO::Real:	args.at().propSet(als[iA], wCtx.getR(iA));	break;
		    case IO::String:	args.at().propSet(als[iA], wCtx.getS(iA));	break;
		    case IO::Object:	args.at().propSet(als[iA], wCtx.getO(iA));	break;
		    default:	break;
		}

	    // Remove compiled function object
	    wCtx.setFunc(NULL);
		wFnc.free();
		if(faddr.size()) {
		    if(fixId.size()) { prms[3].setS(faddr); prms[3].setModify(); }
		    else SYS->nodeDel(faddr);
		}

	    return true;
	} catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

	return false;
    }

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TDAQS::unload( )
{
    TSubSYS::unload();

    mRdRes.lock(true);
    mRdRestDtTm = 1;
    mRdCntr.clear();
    mRdRes.unlock();
}

void TDAQS::subStart( )
{
    vector<string> m_l, tmpl_lst;

    bool reply   = false;
    int  try_cnt = 0;

    do {
	//Start template's libraries
	tmplLibList(tmpl_lst);
	for(unsigned i_lb = 0; i_lb < tmpl_lst.size(); i_lb++)
	    try { tmplLibAt(tmpl_lst[i_lb]).at().start(true); }
	    catch(TError &err) {
		if(try_cnt) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Start template library '%s' error."), tmpl_lst[i_lb].c_str());
		}
		reply = true;
	    }

	//Enable controllers
	modList(m_l);
	for(unsigned i_m = 0; i_m < m_l.size(); i_m++) {
	    vector<string> c_l;
	    at(m_l[i_m]).at().list(c_l);
	    for(unsigned i_c = 0; i_c < c_l.size(); i_c++) {
		AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
		if(/*!cntr.at().enableStat() &&*/ cntr.at().toEnable())
		    try{ cntr.at().enable(); }
		    catch(TError &err) {
			if(try_cnt) {
			    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
			    mess_sys(TMess::Error, _("Enable controller '%s' error."), (m_l[i_m]+"."+c_l[i_c]).c_str());
			}
			reply = true;
		    }
	    }
	}
	try_cnt++;
    } while(reply && try_cnt < 2);

    //Archive subsystem start
    if(!SYS->archive().at().subStartStat() || !SYS->stopSignal()) SYS->archive().at().subStart();

    //Controllers start
    TSubSYS::subStart();
}

void TDAQS::subStop( )
{
    vector<string> m_l;

    //Stop
    modList(m_l);
    for(unsigned i_m = 0; i_m < m_l.size(); i_m++) {
	vector<string> c_l;
	at(m_l[i_m]).at().list(c_l);
	for(unsigned i_c = 0; i_c < c_l.size(); i_c++) {
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if(cntr.at().startStat())
		try{ cntr.at().stop(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Stop controller '%s' error."), (m_l[i_m]+"."+c_l[i_c]).c_str());
		}
	}
    }
    //Disable
    for(unsigned i_m = 0; i_m < m_l.size(); i_m++) {
	vector<string> c_l;
	at(m_l[i_m]).at().list(c_l);
	for(unsigned i_c = 0; i_c < c_l.size(); i_c++) {
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if(cntr.at().enableStat())
		try{ cntr.at().disable(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Disable controller '%s' error."), (m_l[i_m]+"."+c_l[i_c]).c_str());
		}
	}
    }

    //Stop template's libraries
    tmplLibList(m_l);
    for(unsigned i_lb = 0; i_lb < m_l.size(); i_lb++)
	tmplLibAt(m_l[i_lb]).at().start(false);

    TSubSYS::subStop();
}

AutoHD<TCntrNode> TDAQS::daqAt( const string &path, char sep, bool noex, bool waitForAttr ) const
{
    string cEl;
    AutoHD<TCntrNode> DAQnd = const_cast<TDAQS*>(this);
    const char *c_grp = "mod_";
    for(int c_off = 0, c_lv = 0; (sep && (cEl=TSYS::strSepParse(path,0,sep,&c_off)).size()) ||
		   (!sep && (cEl=TSYS::pathLev(path,0,true,&c_off)).size()); ++c_lv)
    {
	bool lastEl = (c_lv > 2 && c_off >= (int)path.size());
	if(waitForAttr && lastEl) c_grp = "a_";
	AutoHD<TCntrNode> tNd = DAQnd.at().nodeAt(c_grp+cEl, 0, sep, 0, true);
	if(tNd.freeStat() && !(strcmp(c_grp,"a_") != 0 && lastEl && !(tNd=DAQnd.at().nodeAt("a_"+cEl,0,sep,0,true)).freeStat())) {
	    if(noex) return AutoHD<TValue>();
	    else throw err_sys(_("No DAQ node present '%s'."), path.c_str());
	}
	c_grp = (c_lv == 0) ? "cntr_" : "prm_";
	DAQnd = tNd;
    }

    return DAQnd;
}

AutoHD<TValue> TDAQS::prmAt( const string &path, char sep, bool noex ) const
{
    AutoHD<TCntrNode> DAQnd = daqAt(path, sep, noex);
    if(DAQnd.freeStat() || !dynamic_cast<TValue*>(&DAQnd.at())) {
	if(noex) return AutoHD<TValue>();
	else throw err_sys(_("Pointed node is not parameter '%s'."), path.c_str());
    }

    return DAQnd;
}

AutoHD<TVal> TDAQS::attrAt( const string &path, char sep, bool noex ) const
{
    AutoHD<TCntrNode> DAQnd = daqAt(path, sep, noex, true);
    if(DAQnd.freeStat() || !dynamic_cast<TVal*>(&DAQnd.at())) {
	if(noex) return AutoHD<TVal>();
	else throw err_sys(_("Pointed node is not attribute '%s'."), path.c_str());
    }

    return DAQnd;
}

bool TDAQS::rdProcess( XMLNode *reqSt )
{
    if(reqSt) {
	string StId = reqSt->attr("StId");
	ResAlloc res(mRdRes, true);
	mRdCntr[StId].clear();
	for(unsigned iC = 0; iC < reqSt->childSize(); iC++)
	    if(reqSt->childGet(iC)->name() == "cntr")
		mRdCntr[StId][reqSt->childGet(iC)->attr("id")] = s2i(reqSt->childGet(iC)->attr("run"));
	return true;
    }

    //Planing controllers' run and process requests to remote run controllers
    map<string,TSYS::SStat> sts = SYS->rdSts();
    map<string,TSYS::SStat>::iterator sit;
    map<string,bool>::iterator cit;

    vector<string> cls;
    rdActCntrList(cls);
    for(unsigned iC = 0; iC < cls.size(); iC++) {
	AutoHD<TController> cntr = at(TSYS::strParse(cls[iC],0,".")).at().at(TSYS::strParse(cls[iC],1,"."));

	// Process remote run controllers, before the redundancy status change
	// !!!!: Moved here from the end
	if(cntr.at().startStat() && cntr.at().redntUse()) cntr.at().redntDataUpdate();

	// Check contoller run plane
	if(cntr.at().redntMode() == TController::Off) cntr.at().setRedntUse(false);
	else {
	    ResAlloc res(mRdRes, false);
	    if(cntr.at().redntRun() == "<high>") {
		int wLev = SYS->rdStLevel();
		for(sit = sts.begin(); sit != sts.end(); ++sit)
		    if(sit->second.isLive && (cit=mRdCntr[sit->first].find(cntr.at().workId())) != mRdCntr[sit->first].end() && cit->second)
			wLev = vmax(wLev, sit->second.lev);
		cntr.at().setRedntUse(SYS->rdStLevel() < wLev);
	    }
	    else if(cntr.at().redntRun() == "<low>") {
		int wLev = SYS->rdStLevel();
		for(sit = sts.begin(); sit != sts.end(); sit++)
		    if(sit->second.isLive && (cit=mRdCntr[sit->first].find(cntr.at().workId())) != mRdCntr[sit->first].end() && cit->second)
			wLev = vmin(wLev, sit->second.lev);
		cntr.at().setRedntUse(SYS->rdStLevel() > wLev);
	    }
	    else if(cntr.at().redntRun() == "<optimal>") {
		vector<string> cls_lc;
		rdActCntrList(cls_lc, true);
		bool remPresent = false;
		for(sit = sts.begin(); sit != sts.end(); sit++)
		    if(sit->second.isLive && (cit=mRdCntr[sit->first].find(cntr.at().workId())) != mRdCntr[sit->first].end()) {
			if(!remPresent) remPresent = cit->second;
			int aCntr = 0;
			for(map<string,bool>::iterator scit = mRdCntr[sit->first].begin(); scit != mRdCntr[sit->first].end(); scit++)
			    if(scit->second) aCntr++;
			if(((int)cls_lc.size()-aCntr) >= 0 && cit->second) break;
		    }
		cntr.at().setRedntUse(sit != sts.end());
	    }
	    else {
		for(sit = sts.begin(); sit != sts.end(); sit++)
		    if(sit->second.isLive && (cit=mRdCntr[sit->first].find(cntr.at().workId())) != mRdCntr[sit->first].end() &&
			cit->second && cntr.at().redntRun() == sit->first)
			    break;
		cntr.at().setRedntUse(sit != sts.end());
	    }
	    res.release();
	}
	cntr.free();
    }

    return true;
}

string TDAQS::rdStRequest( const string &cntr, XMLNode &req, const string &prevSt, bool toRun )
{
    string lcPath = req.attr("path");
    bool prevPresent = false;
    string rez;
    map<string, TSYS::SStat>::iterator sit;
    map<string, bool>::iterator cit;

    ResAlloc res(mRdRes, false);
    map<string, map<string,bool> > sts = mRdCntr;
    res.unlock();

    for(map<string, map<string,bool> >::iterator sit = sts.begin(); sit != sts.end(); ++sit)
	if((cit=sit->second.find(cntr)) != sit->second.end() && (!toRun || cit->second)) {
	    if(prevSt.size() && !prevPresent) {
		if(sit->first == prevSt) prevPresent = true;
		continue;
	    }
	    if((rez=SYS->rdStRequest(req,sit->first)).size()) return rez;

	    res.lock(true);
	    mRdCntr.erase(sit->first);
	    res.unlock();
	}

    if(prevSt.empty()) at(TSYS::strParse(cntr,0,".")).at().at(TSYS::strParse(cntr,1,".")).at().setRedntUse(false);

    return "";
}

string TDAQS::optDescr( )
{
    return TSYS::strMess(_(
	"=================== Subsystem \"Data acquisition\" options ================\n"
	"------------ Parameters of section '%s' in config-file -----------\n"
	"RdRestDtTm   <hour> Restore data archive depth from a reserve station after deadline.\n\n"
	),nodePath().c_str());
}

void TDAQS::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/redundant") {	//Redundant service requests
	if(ctrChkNode(opt,"st",RWRWR_,"root",SDAQ_ID,SEC_RD)) {	//State
	    opt->setAttr("inProc", "1");
	    opt->setAttr("StLevel",i2s(SYS->rdStLevel()));
	    map<string, bool> cntrLs;
	    vector<string> cls;
	    rdActCntrList(cls);
	    for(unsigned iL = 0; iL < cls.size(); iL++) cntrLs[cls[iL]] = false;
	    rdActCntrList(cls,true);
	    for(unsigned iL = 0; iL < cls.size(); iL++) cntrLs[cls[iL]] = true;
	    for(map<string,bool>::iterator cit = cntrLs.begin(); cit != cntrLs.end(); cit++)
		opt->childAdd("cntr")->setAttr("id",cit->first)->setAttr("run",cit->second?"1":"0");
	    return;
	}
    }
    else if(a_path == "/serv/PrmAttr" && ctrChkNode(opt,"list",RWRWR_,"root",SDAQ_ID,SEC_RD))	//Parameter and/or attributes list-browse
	ctrListPrmAttr(opt, opt->attr("base"), s2i(opt->attr("toPrm")), opt->attr("sep")[0], opt->attr("pref"));

    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",EVAL_STR,R_R_R_,"root","root",1,"doc",(subId()+"|Documents/"+subId()).c_str());
	ctrMkNode("grp",opt,-1,"/br/tmplb_",_("Template library"),RWRWR_,"root",SDAQ_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(SYS->rdEnable() && ctrMkNode("area",opt,0,"/redund",_("Redundancy"))) {
	    ctrMkNode("fld",opt,-1,"/redund/restDtTm",_("Depth time of restoring data at the starting, hours"),RWRWR_,"root",SDAQ_ID,1, "tp","real");
	    if(ctrMkNode("table",opt,-1,"/redund/cntr",_("Controllers"),RWRWR_,"root",SDAQ_ID,1,"key","id")) {
		ctrMkNode("list",opt,-1,"/redund/cntr/id",_("Controller"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/redund/cntr/nm",_("Name"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/redund/cntr/start",_("Run."),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/redund/cntr/rdndt",_("Redund."),RWRWR_,"root",SDAQ_ID,4, "tp","int", "dest","select",
		    "sel_id",(i2s(TController::Off)+";"+i2s(TController::Asymmetric)+";"+i2s(TController::OnlyAlarms)).c_str(),
		    "sel_list",_("Off;Asymmetric;Only alarms"));
		ctrMkNode("list",opt,-1,"/redund/cntr/prefRun",_("Pref. to run"),RWRWR_,"root",SDAQ_ID,4,"tp","str",
		    "idm","1","dest","select","select","/redund/lsMode");
		ctrMkNode("list",opt,-1,"/redund/cntr/remoted",_("Remote"),R_R_R_,"root",SDAQ_ID,1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,1,"/tpllibs",_("Template libraries")))
	    ctrMkNode("list",opt,-1,"/tpllibs/lb",_("Template libraries"),RWRWR_,"root",SDAQ_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","tmplb_","idSz",OBJ_ID_SZ);
	return;
    }

    //Process command to page
    if(a_path == "/br/tmplb_" || a_path == "/tpllibs/lb") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    vector<string> lst;
	    tmplLibList(lst);
	    for(unsigned iA = 0; iA < lst.size(); iA++)
		opt->childAdd("el")->setAttr("id",lst[iA])->setText(tmplLibAt(lst[iA]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    tmplLibReg(new TPrmTmplLib(TSYS::strEncode(opt->attr("id"),TSYS::oscdID).c_str(),opt->text().c_str(),"*.*"));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    tmplLibUnreg(opt->attr("id"),1);
    }
    else if(a_path == "/redund/restDtTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(r2s(rdRestDtTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setRdRestDtTm(s2r(opt->text()));
    }
    else if(a_path == "/redund/cntr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    XMLNode *nId	= ctrMkNode("list",opt,-1,"/redund/cntr/id","",R_R_R_,"root",SDAQ_ID);
	    XMLNode *nNm	= ctrMkNode("list",opt,-1,"/redund/cntr/nm","",R_R_R_,"root",SDAQ_ID);
	    XMLNode *nStart	= ctrMkNode("list",opt,-1,"/redund/cntr/start","",RWRWR_,"root",SDAQ_ID);
	    XMLNode *nRdndt	= ctrMkNode("list",opt,-1,"/redund/cntr/rdndt","",RWRWR_,"root",SDAQ_ID);
	    XMLNode *nPrefRun	= ctrMkNode("list",opt,-1,"/redund/cntr/prefRun","",RWRWR_,"root",SDAQ_ID);
	    XMLNode *nRem	= ctrMkNode("list",opt,-1,"/redund/cntr/remoted","",R_R_R_,"root",SDAQ_ID);

	    vector<string> mls, cls;
	    modList(mls);
	    AutoHD<TController> cntr;
	    for(unsigned iM = 0; iM < mls.size(); iM++) {
		if(!at(mls[iM]).at().redntAllow()) continue;
		at(mls[iM]).at().list(cls);
		for(unsigned iC = 0; iC < cls.size(); iC++) {
		    cntr = at(mls[iM]).at().at(cls[iC]);
		    if(nId)		nId->childAdd("el")->setText(mls[iM]+"."+cls[iC]);
		    if(nNm)		nNm->childAdd("el")->setText(cntr.at().name());
		    if(nStart)		nStart->childAdd("el")->setText(cntr.at().startStat()?"1":"0");
		    if(nRdndt)		nRdndt->childAdd("el")->setText(i2s(cntr.at().redntMode()));
		    if(nPrefRun)	nPrefRun->childAdd("el")->setText(cntr.at().redntRun());
		    if(nRem)		nRem->childAdd("el")->setText(cntr.at().redntUse(TController::Any)?"1":"0");
		}
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    string col = opt->attr("col");
	    AutoHD<TController> cntr  = at(TSYS::strSepParse(opt->attr("key_id"),0,'.')).at().
					at(TSYS::strSepParse(opt->attr("key_id"),1,'.'));
	    if(col == "start")		s2i(opt->text()) ? cntr.at().start() : cntr.at().stop();
	    else if(col == "rdndt")	cntr.at().setRedntMode((TController::Redundant)(s2i(opt->text())));
	    else if(col == "prefRun")	cntr.at().setRedntRun(opt->text());
	}
    }
    else if(a_path == "/redund/lsMode" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","<high>")->setText(_("<High level>"));
	opt->childAdd("el")->setAttr("id","<low>")->setText(_("<Low level>"));
	opt->childAdd("el")->setAttr("id","<optimal>")->setText(_("<Optimal>"));
	vector<string> sls;
	SYS->rdStList(sls);
	for(unsigned iS = 0; iS < sls.size(); iS++)
	    opt->childAdd("el")->setAttr("id",sls[iS])->setText(SYS->transport().at().extHostGet("*",sls[iS]).name);
    }
    else TSubSYS::cntrCmdProc(opt);
}

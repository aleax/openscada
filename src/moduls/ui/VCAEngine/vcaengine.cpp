
//OpenSCADA module UI.VCAEngine file: vcaengine.cpp
/***************************************************************************
 *   Copyright (C) 2006-2023 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/types.h>
#include <unistd.h>

#include <tsys.h>
#include <tmess.h>

#include "origwidg.h"
#include "vcaFuncs.h"
#include "vcaengine.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"VCAEngine"
#define MOD_NAME	trS("Engine of the visual control area")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define MOD_SUBTYPE	"VCAEngine"
#define MOD_VER		"7.11.6"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("The main engine of the visual control area.")
#define LICENSE		"GPL2"
//*************************************************

VCA::Engine *VCA::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_VCAEngine_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_VCAEngine_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new VCA::Engine(source);
	return NULL;
    }
}

using namespace VCA;

//************************************************
//* Engine                                       *
//************************************************
Engine::Engine( string name ) : TUI(MOD_ID), passAutoEn(false)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    idWlb = grpAdd("wlb_");
    idPrj = grpAdd("prj_");
    idSes = grpAdd("ses_");
    idFnc = grpAdd("vca", true);
}

Engine::~Engine( )
{
    nodeDelAll();
}

void Engine::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string Engine::modInfo( const string &name )
{
    if(name == "SubType")	return MOD_SUBTYPE;

    return TModule::modInfo(name);
}

void Engine::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(!(flag&TCntrNode::NodeConnect)) return;

    //Make lib's DB structure: Libs(__ID__, NAME, DSCR, DB_TBL, ICO)
    lbwdg_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,"30"));
    lbwdg_el.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    lbwdg_el.fldAdd(new TFld("DESCR",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    lbwdg_el.fldAdd(new TFld("DB_TBL",trS("DB table"),TFld::String,TFld::NoFlag,"30"));	//????[v1.0] Remove
    lbwdg_el.fldAdd(new TFld("ICO",trS("Icon"),TFld::String,TFld::NoFlag,"100000"));

    //Make library widgets' data container: {LibWidgetMime,ProjMime}(__ID__, MIME, DATA)
    wdgdata_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"40"));
    wdgdata_el.fldAdd(new TFld("MIME",trS("MIME"),TFld::String,TFld::NoFlag,"30"));
    wdgdata_el.fldAdd(new TFld("DATA",trS("Resource data"),TFld::String,TFld::NoFlag,"500000"));

    //Make widgets' DB structure: LibWigets(__ID__, ICO, PARENT, PROC, PROC_PER, ATTRS, TIMESTAMP)
    wdg_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"30"));
    wdg_el.fldAdd(new TFld("ICO",trS("Icon"),TFld::String,TFld::NoFlag,"100000"));
    wdg_el.fldAdd(new TFld("PARENT",trS("Parent widget"),TFld::String,TFld::NoFlag,"200"));
    wdg_el.fldAdd(new TFld("PR_TR",trS("Completely translate the procedure"),TFld::Boolean,TFld::NoFlag,"1","0"));
    wdg_el.fldAdd(new TFld("PROC",trS("Procedure text and language"),TFld::String,TFld::TransltText,"1000000"));
    wdg_el.fldAdd(new TFld("PROC_PER",trS("Period of the procedure calculating"),TFld::Integer,TFld::NoFlag,"5",i2s(Widget::PerVal_Parent).c_str()));
    wdg_el.fldAdd(new TFld("ATTRS",trS("Changed attributes"),TFld::String,TFld::NoFlag,"10000"));
    wdg_el.fldAdd(new TFld("TIMESTAMP",trS("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //Make include widgets' DB structure: {LibWidgetIncl,ProjPageWIncl}(__IDW__, __ID__, PARENT, ATTRS)
    inclwdg_el.fldAdd(new TFld("IDW",trS("Widget identifier"),TFld::String,TCfg::Key,"100"));
    inclwdg_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"30"));
    inclwdg_el.fldAdd(new TFld("PARENT",trS("Parent widget"),TFld::String,TFld::NoFlag,"200"));
    inclwdg_el.fldAdd(new TFld("PROC_PER",trS("Period of the procedure calculating"),TFld::Integer,TFld::NoFlag,"5",i2s(Widget::PerVal_Parent).c_str()));
    inclwdg_el.fldAdd(new TFld("ATTRS",trS("Changed attributes"),TFld::String,TFld::NoFlag,"10000"));

    //Make widget's IO DB structure: {LibWidgetIO,ProjPageIO}(__IDW__, __ID__, __IDC__, IO_VAL, SELF_FLG, CFG_TMPL, CFG_VAL)
    wdgio_el.fldAdd(new TFld("IDW",trS("Widget identifier"),TFld::String,TCfg::Key,"100"));
    wdgio_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"61"));
    wdgio_el.fldAdd(new TFld("IDC",trS("Child ID"),TFld::String,TCfg::Key,"30"));
    wdgio_el.fldAdd(new TFld("IO_VAL",trS("Attribute value"),TFld::String,TFld::TransltText,"100000"));
    wdgio_el.fldAdd(new TFld("SELF_FLG",trS("Attribute flags"),TFld::Integer,TFld::NoFlag,"5"));
    wdgio_el.fldAdd(new TFld("CFG_TMPL",trS("Configuration template"),TFld::String,TFld::TransltText,"30"));
    wdgio_el.fldAdd(new TFld("CFG_VAL",trS("Configuration value"),TFld::String,TFld::TransltText,"1000"));

    //Make widget's user IO DB structure: {ProjPageUserIO,LibWidgetUserIO}(__IDW__, __ID__, __IDC__, NAME, IO_TP, IO_VAL, SELF_FLG, CFG_TMPL, CFG_VAL)
    wdguio_el.fldAdd(new TFld("IDW",trS("Widget identifier"),TFld::String,TCfg::Key,"100"));
    wdguio_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"61"));
    wdguio_el.fldAdd(new TFld("IDC",trS("Child identifier"),TFld::String,TCfg::Key,"30"));
    wdguio_el.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,"100"));
    wdguio_el.fldAdd(new TFld("IO_TYPE",trS("Attribute generic flags and type"),TFld::Integer,TFld::NoFlag,"10"));
    wdguio_el.fldAdd(new TFld("IO_VAL",trS("Attribute value"),TFld::String,TFld::TransltText,"100000"));
    wdguio_el.fldAdd(new TFld("SELF_FLG",trS("Attribute flags"),TFld::Integer,TFld::NoFlag,"5"));
    wdguio_el.fldAdd(new TFld("CFG_TMPL",trS("Configuration template"),TFld::String,TFld::TransltText,"30"));
    wdguio_el.fldAdd(new TFld("CFG_VAL",trS("Configuration value"),TFld::String,TFld::TransltText,"1000"));

    //Make project's DB structure: Projs(__ID__, NAME, DSCR, DB_TBL, ICO, USER, GRP, PERMIT, PER, FLGS, STYLE)
    prj_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,"30"));
    prj_el.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    prj_el.fldAdd(new TFld("DESCR",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    prj_el.fldAdd(new TFld("DB_TBL",trS("DB table"),TFld::String,TFld::NoFlag,"30"));	//????[v1.0] Remove
    prj_el.fldAdd(new TFld("ICO",trS("Icon"),TFld::String,TFld::NoFlag,"100000"));
    prj_el.fldAdd(new TFld("USER",trS("User"),TFld::String,TFld::NoFlag,i2s(limObjID_SZ).c_str(),"root"));
    prj_el.fldAdd(new TFld("GRP",trS("Group"),TFld::String,TFld::NoFlag,i2s(limObjID_SZ).c_str(),"UI"));
    prj_el.fldAdd(new TFld("PERMIT",trS("Permission"),TFld::Integer,TFld::OctDec,"3","436"));
    prj_el.fldAdd(new TFld("PER",trS("Period of calculating, milliseconds"),TFld::Integer,TFld::NoFlag,"4","100"));
    prj_el.fldAdd(new TFld("STYLE",trS("Work style"),TFld::Integer,TFld::NoFlag,"2","-1"));
    prj_el.fldAdd(new TFld("EN_BY_NEED",trS("Enable as needed"),TFld::Boolean,TFld::NoFlag,"1","1"));

    //Make page's DB structure: ProjPages(__OWNER__, __ID__, ICO, PARENT, PROC, PROC_PER, FLGS, ATTRS, TIMESTAMP)
    page_el.fldAdd(new TFld("OWNER",trS("Owner"),TFld::String,TCfg::Key,"100"));
    page_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"30"));
    page_el.fldAdd(new TFld("ICO",trS("Icon"),TFld::String,TFld::NoFlag,"100000"));
    page_el.fldAdd(new TFld("PARENT",trS("Parent widget"),TFld::String,TFld::NoFlag,"200"));
    page_el.fldAdd(new TFld("PR_TR",trS("Completely translate the procedure"),TFld::Boolean,TFld::NoFlag,"1","0"));
    page_el.fldAdd(new TFld("PROC",trS("Procedure text and language"),TFld::String,TFld::TransltText,"1000000"));
    page_el.fldAdd(new TFld("PROC_PER",trS("Period of the procedure calculating"),TFld::Integer,TFld::NoFlag,"5",i2s(Widget::PerVal_Parent).c_str()));
    page_el.fldAdd(new TFld("FLGS",trS("Flags"),TFld::Integer,TFld::NoFlag,"1","0"));
    page_el.fldAdd(new TFld("ATTRS",trS("Changed attributes"),TFld::String,TFld::NoFlag,"10000"));
    page_el.fldAdd(new TFld("TIMESTAMP",trS("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //Make sessions' IO values of projects DB structure: ProjSess(__IDW__, __ID__, IO_VAL)
    prj_ses_el.fldAdd(new TFld("IDW",trS("Widget identifier"),TFld::String,TCfg::Key,"200"));
    prj_ses_el.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,"30"));
    prj_ses_el.fldAdd(new TFld("IO_VAL",trS("Attribute value"),TFld::String,TFld::NoFlag,"100000"));

    //Make styles' IO DB structure: PrjStls(__IDS__, __ID__, VAL)
    prjStls_el.fldAdd(new TFld("IDS","",TFld::Integer,TCfg::Key,"2"));
    prjStls_el.fldAdd(new TFld("ID","",TFld::String,TCfg::Key,"30"));
    prjStls_el.fldAdd(new TFld("VAL","",TFld::String,TFld::TransltText,"1000000"));

    //Make styles' IO DB structure: PrjStl(__ID__, V_0, ... V_{Project::StlMaximum})
    prjStl_el.fldAdd(new TFld("ID","",TFld::String,TCfg::Key,"30"));
    for(unsigned iStl = 0; iStl < Project::StlMaximum; ++iStl)
	prjStl_el.fldAdd(new TFld(("V_"+i2s(iStl)).c_str(),"",TFld::String,TFld::NoFlag,"100"));

    //Init original widgets library
    wlbAdd("originals", trS("Original widgets"), "");
    // Set default library icon
    if(TUIS::icoGet("VCA.lwdg_root",NULL,true).size())
	wlbAt("originals").at().setIco(TSYS::strEncode(TUIS::icoGet("VCA.lwdg_root"),TSYS::base64));
    // Add main original widgets
    wlbAt("originals").at().add(new OrigElFigure());
    wlbAt("originals").at().add(new OrigFormEl());
    wlbAt("originals").at().add(new OrigText());
    wlbAt("originals").at().add(new OrigMedia());
    wlbAt("originals").at().add(new OrigDiagram());
    wlbAt("originals").at().add(new OrigProtocol());
    wlbAt("originals").at().add(new OrigDocument());
    //wlbAt("originals").at().add(new OrigFunction());
    wlbAt("originals").at().add(new OrigBox());

    // User functions
    chldAdd(idFnc, new nodePresent());
    chldAdd(idFnc, new wdgList());
    chldAdd(idFnc, new attrList());
    chldAdd(idFnc, new attrGet());
    chldAdd(idFnc, new attrSet());
    chldAdd(idFnc, new sesUser());
}

void Engine::preDisable( int flag )
{
    if(startStat()) modStop();

    vector<string> ls;
    passAutoEn = true;

    //Sessions disable
    sesList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++)
	sesAt(ls[lID]).at().setEnable(false);

    //Projects disable
    prjList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++)
	prjAt(ls[lID]).at().setEnable(false);

    //Libraries disable
    wlbList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++)
	wlbAt(ls[lID]).at().setEnable(false);
    passAutoEn = false;

    TModule::preDisable(flag);
}

void Engine::load_( )
{
    int64_t d_tm = 0;

    map<string, bool>	itReg;

    passAutoEn = true;

    if(mess_lev() == TMess::Debug)	d_tm = TSYS::curTime();

    //Load widgets libraries
    try {
	// Search and create new libraries
	TConfig cEl(&elWdgLib());
	//cEl.cfgViewAll(false);
	vector<string> itLs;

	// Search in DB
	TBDS::dbList(itLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iDB = 0; iDB < itLs.size(); iDB++)
	    for(int libCnt = 0; TBDS::dataSeek(itLs[iDB]+"."+wlbTable(),nodePath()+"LIB",libCnt++,cEl,TBDS::UseCache); ) {
		string lId = cEl.cfg("ID").getS();
		if(!wlbPresent(lId)) wlbAdd(lId,"", itLs[iDB]);
		if(wlbAt(lId).at().DB() == itLs[iDB] || wlbAt(lId).at().DB().empty())
		    wlbAt(lId).at().load(&cEl);
		wlbAt(lId).at().setDB(itLs[iDB], true);
		itReg[lId] = true;
		if(mess_lev() == TMess::Debug) {
		    mess_sys(TMess::Debug, _("Time of the library '%s' loading: %f ms."), lId.c_str(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}
	    }

	// Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    wlbList(itLs);
	    for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		if(itReg.find(itLs[iIt]) == itReg.end() && SYS->chkSelDB(wlbAt(itLs[iIt]).at().DB()))
		    wlbDel(itLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error loading the widgets libraries."));
    }

    //Load projects
    try {
	// Search and create new projects
	TConfig cEl(&elProject());
	//cEl.cfgViewAll(false);
	//cEl.cfg("EN_BY_NEED").setView(true);
	vector<string> itLs;
	itReg.clear();

	// Search in DB
	TBDS::dbList(itLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iDB = 0; iDB < itLs.size(); iDB++)
	    for(int lib_cnt = 0; TBDS::dataSeek(itLs[iDB]+"."+prjTable(),nodePath()+"PRJ",lib_cnt++,cEl,TBDS::UseCache); ) {
		string prj_id = cEl.cfg("ID").getS();
		if(!prjPresent(prj_id)) {
		    prjAdd(prj_id, "", itLs[iDB]);
		    if(cEl.cfg("EN_BY_NEED").getB()) prjAt(prj_id).at().setEnableByNeed();
		}
		if(prjAt(prj_id).at().DB() == itLs[iDB]) prjAt(prj_id).at().load(&cEl);
		prjAt(prj_id).at().setDB(itLs[iDB], true);
		itReg[prj_id] = true;
		if(mess_lev() == TMess::Debug) {
		    mess_sys(TMess::Debug, _("Time of the project '%s' loading: %f ms."), prj_id.c_str(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}
	    }

	// Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    prjList(itLs);
	    for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		if(itReg.find(itLs[iIt]) == itReg.end() && SYS->chkSelDB(prjAt(itLs[iIt]).at().DB()))
		    prjDel(itLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error loading the projects."));
    }

    //Libraries enable
    vector<string> ls;
    wlbList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++) {
	wlbAt(ls[lID]).at().setEnable(true);
	if(mess_lev() == TMess::Debug) {
	    mess_sys(TMess::Debug, _("Time of the library '%s' enabling: %f ms."), ls[lID].c_str(), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}
    }

    //Projects enable
    prjList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++) {
	if(prjAt(ls[lID]).at().enableByNeed)	continue;
	prjAt(ls[lID]).at().setEnable(true);
	if(mess_lev() == TMess::Debug) {
	    mess_sys(TMess::Debug, _("Time of the project '%s' enabling: %f ms."), ls[lID].c_str(), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}
    }

    passAutoEn = false;

    //Auto-sessions load and enable
    ResAlloc res(mSesRes, true);
    XMLNode aSess("Sess");
    try {
	aSess.load(TBDS::genPrmGet(nodePath()+"AutoSess"));
	for(unsigned iN = 0; iN < aSess.childSize(); iN++) {
	    string sId	= aSess.childGet(iN)->attr("id"),
		   sPrj	= aSess.childGet(iN)->attr("prj"),
		   sUser= aSess.childGet(iN)->attr("user");
	    mSessAuto[sId] = sPrj + ":" + sUser;

	    try {
		if(!sesPresent(sId) && prjAt(sPrj).at().enable()) {
		    sesAdd(sId, sPrj);
		    sesAt(sId).at().setUser(sUser);
		    sesAt(sId).at().setBackgrnd(true);
		    sesAt(sId).at().setEnable(true);
		}
	    } catch(...) { }
	}
    } catch(...){ }
    res.release();

    modifGClr();
}

void Engine::save_( )
{
    //Auto-sessions save
    ResAlloc res(mSesRes, false);
    XMLNode aSess("Sess");
    for(map<string,string>::iterator ias = mSessAuto.begin(); ias != mSessAuto.end(); ias++)
	aSess.childAdd("it")->setAttr("id",ias->first)->
			      setAttr("prj",TSYS::strParse(ias->second,0,":"))->
			      setAttr("user",TSYS::strParse(ias->second,1,":"));
    TBDS::genPrmSet(nodePath()+"AutoSess",aSess.save());
}

void Engine::modStart( )
{
    vector<string> ls;

    TModule::modStart();

    //Auto-sessions enable
    ResAlloc res(mSesRes, true);
    for(map<string,string>::iterator si = mSessAuto.begin(); si != mSessAuto.end(); ++si)
	try {
	    string sId = si->first,
		   sPrj = TSYS::strParse(si->second,0,":"),
		   sUser = TSYS::strParse(si->second,1,":");
	    if(!sesPresent(sId) && prjAt(sPrj).at().enable()) {
		    sesAdd(sId, sPrj);
		    sesAt(sId).at().setUser(sUser);
		    sesAt(sId).at().setBackgrnd(true);
		    sesAt(sId).at().setEnable(true);
		}
	    } catch(...) { }
    res.release();

    //Start sessions
    sesList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++)
	sesAt(ls[lID]).at().setStart(true);

    runSt = true;
}

void Engine::modStop( )
{
    TModule::modStop();

    vector<string> ls;

    //Stop sessions
    sesList(ls);
    for(unsigned lID = 0; lID < ls.size(); lID++)
	sesAt(ls[lID]).at().setStart(false);

    runSt = false;
}

string Engine::wlbAdd( const string &iid, const string &inm, const string &idb )
{
    if(wlbPresent(iid))	throw err_sys(_("The widget library '%s' is already present!"), iid.c_str());
	//return "";

    return chldAdd(idWlb, new WidgetLib(TSYS::strEncode(sTrm(iid),TSYS::oscdID),inm,idb));
}

AutoHD<WidgetLib> Engine::wlbAt( const string &id ) const	{ return chldAt(idWlb,id); }

string Engine::prjAdd( const string &iid, const string &inm, const string &idb )
{
    if(prjPresent(iid))	throw err_sys(_("The project '%s' is already present!"), iid.c_str());
	//return "";

    return chldAdd(idPrj, new Project(TSYS::strEncode(sTrm(iid),TSYS::oscdID),inm,idb));
}

AutoHD<Project> Engine::prjAt( const string &id ) const	{ return chldAt(idPrj,id); }

void Engine::sesAdd( const string &iid, const string &iproj )
{
    if(sesPresent(iid))	return;
    chldAdd(idSes, new Session(iid,iproj));
}

AutoHD<Session> Engine::sesAt( const string &id ) const	{ return chldAt(idSes,id); }

AutoHD<TFunction> Engine::fAt( const string &id ) const	{ return chldAt(idFnc,id); }

void Engine::attrsLoad( Widget &w, const string &fullDB, const string &idw, const string &idc, const string &attrs, bool ldGen )
{
    string tbl = TSYS::strParseEnd(fullDB, 0, ".");

    TConfig cEl(&elWdgIO());
    cEl.cfg("IDW").setS(idw);
    cEl.cfg("IDC").setS(idc);
    cEl.cfg("CFG_TMPL").setNoTransl(false);
    cEl.cfg("CFG_VAL").setNoTransl(false);
    cEl.cfg("IO_VAL").setNoTransl(false);
    cEl.cfg("CFG_TMPL").setExtVal(true);
    cEl.cfg("CFG_VAL").setExtVal(true);
    cEl.cfg("IO_VAL").setExtVal(true);

    string tstr, tCfgVal;
    for(int off = 0; !(tstr = TSYS::strSepParse(attrs,0,';',&off)).empty(); ) {
	if(!w.attrPresent(tstr)) continue;
	AutoHD<Attr> attr = w.attrAt(tstr);

	if((ldGen && !(attr.at().flgGlob()&Attr::Generic)) ||
		(!ldGen && (attr.at().flgGlob()&Attr::Generic || (!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser))))
	    continue;

	cEl.cfg("ID").setS(tstr);
	cEl.cfg("IO_VAL").setNoTransl(!attr.at().isTransl());
	cEl.cfg("CFG_VAL").setNoTransl(!attr.at().isTransl());

	if(!TBDS::dataGet(fullDB+"_io",nodePath()+tbl+"_io",cEl,TBDS::NoException)) continue;

	unsigned selfFlg = cEl.cfg("SELF_FLG").getI();
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)selfFlg);

	tCfgVal = cEl.cfg("CFG_VAL").getS(TCfg::ExtValOne);
	attr.at().setCfgVal((!cEl.cfg("CFG_VAL").noTransl() && (selfFlg&Attr::CfgConst ||
				(selfFlg&Attr::CfgLnkIn && tCfgVal.find("val:") == 0))) ? cEl.cfg("CFG_VAL").getS() : tCfgVal);

	if(!(attr.at().flgGlob()&Attr::NotStored)) {
	    if(selfFlg&Attr::CfgConst && tCfgVal.size()) attr.at().setS(attr.at().cfgVal());
	    else attr.at().setS(/*(selfFlg&(Attr::CfgLnkIn|Attr::FromStyle)) ? cEl.cfg("IO_VAL").getS(TCfg::ExtValOne) :*/ cEl.cfg("IO_VAL").getS(), true);
	}

	attr.at().setCfgTempl((selfFlg&Attr::FromStyle)?cEl.cfg("CFG_TMPL").getS(TCfg::ExtValOne):cEl.cfg("CFG_TMPL").getS());
    }

    if(ldGen)	return;

    //Load widget's user attributes
    cEl.setElem(&elWdgUIO());
    cEl.cfg("IDW").setS(idw, true);
    cEl.cfg("IDC").setS(idc, true);
    cEl.cfg("IO_VAL").setNoTransl(false);
    cEl.cfg("CFG_TMPL").setNoTransl(false);
    cEl.cfg("CFG_VAL").setNoTransl(false);
    cEl.cfg("IO_VAL").setExtVal(true);
    cEl.cfg("CFG_TMPL").setExtVal(true);
    cEl.cfg("CFG_VAL").setExtVal(true);
    for(int fldCnt = 0; TBDS::dataSeek(fullDB+"_uio",nodePath()+tbl+"_uio",fldCnt++,cEl,TBDS::UseCache); ) {
	string sid = cEl.cfg("ID").getS();
	if(!TSYS::pathLev(sid,1).empty()) continue;

	unsigned type = cEl.cfg("IO_TYPE").getI();
	unsigned flg = type >> 4;
	type = type&0x0f;

	unsigned selfFlg = cEl.cfg("SELF_FLG").getI();

	if(!w.attrPresent(sid)) w.attrAdd(new TFld(sid.c_str(),cEl.cfg("NAME").getS().c_str(),(TFld::Type)type,flg));

	AutoHD<Attr> attr = w.attrAt(sid);

	if(!(!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser)) continue;

	tCfgVal = cEl.cfg("CFG_VAL").getS(TCfg::ExtValOne);
	attr.at().setCfgVal((Attr::isTransl(TFld::Type(type),flg,selfFlg) && (selfFlg&Attr::CfgConst ||
				(selfFlg&Attr::CfgLnkIn && tCfgVal.compare(0,4,"val:") == 0))) ? cEl.cfg("CFG_VAL").getS() : tCfgVal);

	string IO_VAL = attr.at().isTransl() /*&& !(selfFlg&(Attr::CfgLnkIn|Attr::FromStyle) || (selfFlg&Attr::CfgConst && tCfgVal.size()))*/ ?
					cEl.cfg("IO_VAL").getS() : cEl.cfg("IO_VAL").getS(TCfg::ExtValOne);
	if(selfFlg&Attr::CfgConst && tCfgVal.size()) IO_VAL = attr.at().cfgVal();

	attr.at().setS(IO_VAL);
	if(type == TFld::Integer || type == TFld::Real || (flg&(TFld::Selectable|TFld::SelEdit))) {
	    attr.at().setS(TSYS::strSepParse(IO_VAL,0,'|'));
	    attr.at().fld().setValues(TSYS::strSepParse(IO_VAL,1,'|'));
	    if(flg&(TFld::Selectable|TFld::SelEdit)) attr.at().fld().setSelNames(TSYS::strSepParse(IO_VAL,2,'|'));
	}
	//????[v1.0] Remove - temporary placed to clean up the existing DBs, to early fix from using Values and Names for unproper types.
	else if(IO_VAL.size() >= 2 && IO_VAL.compare(IO_VAL.size()-2,2,"||") == 0) attr.at().setS(IO_VAL.substr(0,IO_VAL.size()-2));

	attr.at().setFlgSelf((Attr::SelfAttrFlgs)selfFlg);	//((selfFlg&(~Attr::VizerSpec))|(attr.at().flgSelf()&Attr::VizerSpec)));

	attr.at().setCfgTempl((selfFlg&Attr::FromStyle)?cEl.cfg("CFG_TMPL").getS(TCfg::ExtValOne):cEl.cfg("CFG_TMPL").getS());

	attr.at().setCfgVal(Attr::isTransl(TFld::Type(type),flg,selfFlg)?cEl.cfg("CFG_VAL").getS():cEl.cfg("CFG_VAL").getS(TCfg::ExtValOne));
    }
}

string Engine::attrsSave( Widget &w, const string &fullDB, const string &idw, const string &idc, bool ldGen )
{
    string tbl = TSYS::strParseEnd(fullDB, 0, ".");
    string m_attrs = "";
    vector<string> als;

    w.attrList(als);
    TConfig cEl(&mod->elWdgIO()); cEl.cfg("IDW").setS(idw, true);
    cEl.cfg("IDC").setS(idc, true);
    TConfig cElu(&mod->elWdgUIO()); cElu.cfg("IDW").setS(idw, true);
    cElu.cfg("IDC").setS(idc, true);
    for(unsigned iA = 0; iA < als.size(); iA++) {
	AutoHD<Attr> attr = w.attrAt(als[iA]);
	if(!attr.at().aModif()) continue;
	if(!(!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser)) m_attrs += als[iA]+";";
	if(ldGen != (bool)(attr.at().flgGlob()&Attr::Generic)) continue;

	//Main attributes storing
	if((attr.at().flgSelf()&Attr::IsInher) || !(attr.at().flgGlob()&Attr::IsUser)) {
	    cEl.cfg("ID").setS(als[iA]);
	    if((attr.at().flgSelf()&Attr::IsInher) && (attr.at().flgSelf()&(Attr::CfgConst|Attr::CfgLnkIn|Attr::CfgLnkOut)) && attr.at().cfgVal().size())
		cEl.cfg("IO_VAL").setS("");	//!!!! Do not save the original value of the inherited and linked attributes
	    else {
		cEl.cfg("IO_VAL").setNoTransl(!(attr.at().isTransl() &&	//!!!! Due to the translation can be useful in the development
				!(/*attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::FromStyle) ||*/ (attr.at().flgSelf()&Attr::CfgConst && attr.at().cfgVal().size()))));
		cEl.cfg("IO_VAL").setS(attr.at().getS());
	    }
	    cEl.cfg("SELF_FLG").setI(attr.at().flgSelf());
	    cEl.cfg("CFG_TMPL").setNoTransl(attr.at().flgSelf()&Attr::FromStyle);
	    cEl.cfg("CFG_TMPL").setS(attr.at().cfgTempl());
	    cEl.cfg("CFG_VAL").setNoTransl(!(attr.at().isTransl(true) && (attr.at().flgSelf()&Attr::CfgConst ||
						(attr.at().flgSelf()&Attr::CfgLnkIn && attr.at().cfgVal().compare(0,4,"val:") == 0))));
	    cEl.cfg("CFG_VAL").setS(attr.at().cfgVal());
	    TBDS::dataSet(fullDB+"_io", nodePath()+tbl+"_io", cEl, TBDS::NoException);
	}
	//User attributes storing
	else if(!ldGen) {
	    cElu.cfg("ID").setS(als[iA]);
	    cElu.cfg("IO_VAL").setNoTransl(!(attr.at().isTransl() &&	//!!!! Due to the translation can be useful in the development
			!(/*attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::FromStyle) ||*/ (attr.at().flgSelf()&Attr::CfgConst && attr.at().cfgVal().size()))));
	    cElu.cfg("IO_VAL").setS(attr.at().getS());
	    if(attr.at().type() == TFld::Integer || attr.at().type() == TFld::Real || (attr.at().flgGlob()&(TFld::Selectable|TFld::SelEdit))) {
		cElu.cfg("IO_VAL").setS(cElu.cfg("IO_VAL").getS()+"|"+attr.at().fld().values());
		if(attr.at().flgGlob()&(TFld::Selectable|TFld::SelEdit)) cElu.cfg("IO_VAL").setS(cElu.cfg("IO_VAL").getS()+"|"+attr.at().fld().selNames());
	    }
	    cElu.cfg("NAME").setS(attr.at().name());
	    cElu.cfg("IO_TYPE").setI(attr.at().fld().type()+(attr.at().fld().flg()<<4));
	    cElu.cfg("SELF_FLG").setI(attr.at().flgSelf());
	    cElu.cfg("CFG_TMPL").setNoTransl(attr.at().flgSelf()&Attr::FromStyle);
	    cElu.cfg("CFG_TMPL").setS(attr.at().cfgTempl());
	    cElu.cfg("CFG_VAL").setNoTransl(!(attr.at().isTransl(true) && (attr.at().flgSelf()&Attr::CfgConst ||
						(attr.at().flgSelf()&Attr::CfgLnkIn && attr.at().cfgVal().compare(0,4,"val:") == 0))));
	    cElu.cfg("CFG_VAL").setS(attr.at().cfgVal());
	    TBDS::dataSet(fullDB+"_uio", nodePath()+tbl+"_uio", cElu, TBDS::NoException);
	}
    }

    if(!ldGen && !SYS->cfgCtx()) {
	//Clearing no present IO for main io table
	cEl.cfgViewAll(false);
	for(int fldCnt = 0; TBDS::dataSeek(fullDB+"_io",nodePath()+tbl+"_io",fldCnt++,cEl); ) {
	    string sid = cEl.cfg("ID").getS();
	    if(w.attrPresent(sid) || (idc.empty() && !TSYS::pathLev(sid,1).empty())) continue;

	    if(!TBDS::dataDel(fullDB+"_io",nodePath()+tbl+"_io",cEl,TBDS::UseAllKeys|TBDS::NoException)) break;
	    fldCnt--;
	}

	//Clear no present IO for user io table
	cElu.cfgViewAll(false);
	for(int fldCnt = 0; TBDS::dataSeek(fullDB+"_uio",nodePath()+tbl+"_uio",fldCnt++,cElu); ) {
	    string sid = cElu.cfg("ID").getS();
	    if(w.attrPresent(sid) || (idc.empty() && !TSYS::pathLev(sid,1).empty())) continue;

	    if(!TBDS::dataDel(fullDB+"_uio",nodePath()+tbl+"_uio",cElu,TBDS::UseAllKeys|TBDS::NoException)) break;
	    fldCnt--;
	}
    }

    return m_attrs;
}

void Engine::perSYSCall( unsigned int cnt )
{
    //Check for stop and remove lose and not used already project's sessions
    vector<string> sls;
    sesList(sls);
    for(unsigned iS = 0; iS < sls.size(); iS++)
	if(!sesAt(sls[iS]).at().backgrnd() && (time(NULL)-sesAt(sls[iS]).at().reqTm()) > DIS_SES_TM)
	    sesDel(sls[iS]);
}

void Engine::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path == "/serv/sess") {	//Session operation
	if(ctrChkNode(opt,"list",RWRWRW,"root",SUI_ID,SEC_RD)) {	//List session for some project
	    string prj = opt->attr("prj");
	    vector<string> ls;
	    sesList(ls);
	    for(unsigned iL = 0; iL < ls.size(); iL++)
		if(prj.empty() || sesAt(ls[iL]).at().projNm() == prj) {
		    AutoHD<Project> prj = sesAt(ls[iL]).at().parent();
		    if(SYS->security().at().access(opt->attr("user"),SEC_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()))
			opt->childAdd("el")->setText(ls[iL]);
		}
	}
	else if(ctrChkNode(opt,"connect",RWRWRW,"root",SUI_ID,SEC_WR)) {
	    string sess = opt->attr("sess");
	    string prj  = opt->attr("prj");

	    // User permission check
	    AutoHD<Project> wprj = (!sess.empty()) ? sesAt(sess).at().parent() : prjAt(prj);
	    if(!SYS->security().at().access(opt->attr("user"),SEC_RD,wprj.at().owner(),wprj.at().grp(),wprj.at().permit()))
		throw TError(nodePath().c_str(),_("Connecting to the session is not permitted for '%s'."),opt->attr("user").c_str());
	    // Connect to present session
	    if(!sess.empty()) {
		if(s2i(opt->attr("onlyMy")) && opt->attr("user") != sesAt(sess).at().user())
		    throw TError(nodePath(), _("That is not my session."));

		opt->setAttr("conId", i2s(sesAt(sess).at().connect(s2i(opt->attr("userChange")))));
		opt->setAttr("prj", sesAt(sess).at().projNm());
		if(s2i(opt->attr("userChange"))) {
		    sesAt(sess).at().setUser(opt->attr("user"));
		    sesAt(sess).at().stlCurentSet();

		    sesAt(sess).at().mess_sys(TMess::Notice, _("User was changed to '%s' on the station '%s'."),
			opt->attr("user").c_str(), opt->attr("remoteSrcAddr").size()?opt->attr("remoteSrcAddr").c_str():"LocalHost");
		}
	    }
	    // Create session
	    else if(!prj.empty()) {
		//  Prepare session name
		sess = prj;
		for(int p_cnt = 0; sesPresent(sess); p_cnt++) sess = prj + i2s(p_cnt);
		sesAdd(sess, prj);
		sesAt(sess).at().setUser(opt->attr("user"));
		sesAt(sess).at().setReqUser(opt->attr("user"));
		sesAt(sess).at().setReqLang(opt->attr("lang"));
		sesAt(sess).at().setStart(true);
		opt->setAttr("conId", i2s(sesAt(sess).at().connect()));
		opt->setAttr("sess", sess);
		sesAt(sess).at().mess_sys(TMess::Notice, _("User '%s' has been connected on '%s'."),
		    opt->attr("user").c_str(), opt->attr("remoteSrcAddr").size()?opt->attr("remoteSrcAddr").c_str():"LocalHost");
	    } else throw TError(nodePath(), _("Error arguments of the session connecting/creating."));
	    opt->setAttr("userIsRoot", SYS->security().at().access(opt->attr("user"),SEC_WR,"root","root",RWRWR_)?"1":"0");
	}
	else if(ctrChkNode(opt,"disconnect",RWRWRW,"root",SUI_ID,SEC_WR)) {
	    string sess = opt->attr("sess");
	    sesAt(sess).at().disconnect(s2i(opt->attr("conId")));
	    if(sesAt(sess).at().connects() == 0 && !sesAt(sess).at().backgrnd()) {
		sesAt(sess).at().mess_sys(TMess::Notice, _("User '%s' has been disconnected on '%s'."),
		    opt->attr("user").c_str(), opt->attr("remoteSrcAddr").size()?opt->attr("remoteSrcAddr").c_str():"LocalHost");
		sesDel(sess);
	    }
	}
	return;
    }
    else if(a_path == "/serv/wlbBr" && ctrChkNode(opt,"get")) {
	bool disIconsCW	= s2i(opt->attr("disIconsCW"));
	bool disIconsW	= s2i(opt->attr("disIconsW"));
	string item = opt->attr("item"), tVl;
	string upd_lb   = TSYS::pathLev(item, 0);
	if(upd_lb.size() && upd_lb.find("wlb_") != 0)	return;
	if(upd_lb.size() > 4)	upd_lb = upd_lb.substr(4);
	string upd_wdg  = TSYS::pathLev(item, 1);
	if(upd_wdg.size() > 4)	upd_wdg = upd_wdg.substr(4);
	string upd_wdgi = TSYS::pathLev(item, 2);
	if(upd_wdgi.size() > 4)	upd_wdgi = upd_wdgi.substr(4);

	// Widgets libraries
	vector<string> ls;
	wlbList(ls);
	for(unsigned iWlb = 0; iWlb < ls.size(); iWlb++) {
	    if(!upd_lb.empty() && upd_lb != ls[iWlb]) continue;
	    AutoHD<WidgetLib> wlb = wlbAt(ls[iWlb]);
	    XMLNode *wlbN = opt->childAdd("wlb")->setAttr("id",ls[iWlb])->setText(trD(wlb.at().name()));
	    wlbN->setAttr("doc", TUIS::docKeyGet(wlb.at().descr()));
	    if((tVl=wlb.at().ico()).size()) wlbN->childAdd("ico")->setText(tVl);

	    //  Widgets
	    vector<string> wls;
	    wlb.at().list(wls);
	    for(unsigned iW = 0; iW < wls.size(); iW++) {
		if(!upd_wdg.empty() && upd_wdg != wls[iW])	continue;
		AutoHD<LWidget> w = wlb.at().at(wls[iW]);
		XMLNode *wN = wlbN->childAdd("w")->setAttr("id",wls[iW])->setAttr("parent",w.at().parentAddr())->setText(trD(w.at().name()));
		if((tVl=disIconsW?"":w.at().ico()).size()) wN->childAdd("ico")->setText(tVl);

		//  Child widgets
		vector<string> cwls;
		w.at().wdgList(cwls);
		if(cwls.size() < 1000)
		    for(unsigned iC = 0; iC < cwls.size(); iC++) {
			if(!upd_wdgi.empty() && upd_wdgi != cwls[iC])	continue;
			AutoHD<CWidget> cw = w.at().wdgAt(cwls[iC]);
			XMLNode *cwN = wN->childAdd("cw")->setAttr("id",cwls[iC])->setText(trD(cw.at().name()));
			if((tVl=disIconsCW?"":cw.at().ico()).size()) cwN->childAdd("ico")->setText(tVl); //   (cwls.size()>=100)?"":cw.at().ico());
		    }
	    }
	}
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/prj_",_("Project"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	ctrMkNode("grp",opt,-1,"/br/wlb_",_("Widgets library"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	ctrMkNode("grp",opt,-1,"/br/ses_",_("Session"),RWRWR_,"root",SUI_ID);
	ctrMkNode("grp",opt,-1,"/br/vca",_("Functions"),R_R_R_,"root",SUI_ID,1,"idm","1");
	if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"),R_R_R_,"root",SUI_ID)) {
	    ctrMkNode("list",opt,-1,"/prm/cfg/prj",_("Project"),RWRWR_,"root",SUI_ID,5,
		"tp","br","idm","1","s_com","add,del","br_pref","prj_","idSz","30");
	    ctrMkNode("list",opt,-1,"/prm/cfg/wlb",_("Widgets libraries"),RWRWR_,"root",SUI_ID,5,
		"tp","br","idm","1","s_com","add,del","br_pref","wlb_","idSz","30");
	}
	if(ctrMkNode("area",opt,1,"/ses",_("Sessions"),R_R_R_,"root",SUI_ID)) {
	    ctrMkNode("list",opt,-1,"/ses/ses",_("Sessions"),RWRWR_,"root",SUI_ID,3,"tp","br","s_com","add,del","br_pref","ses_");
	    if(ctrMkNode("table",opt,-1,"/ses/ast",_("Auto created and started"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id"))
	    {
		ctrMkNode("list",opt,-1,"/ses/ast/id",_("Identifier"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ses/ast/proj",_("Project"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/ses/prj_ls");
		ctrMkNode("list",opt,-1,"/ses/ast/user",_("User"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/ses/usr_ls");
	    }
	}
	return;
    }

    //Process command for page
    if(a_path == "/br/prj_" || a_path == "/prm/cfg/prj") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst, lst1;
	    prjList(lst);

	    bool chkUserPerm = s2i(opt->attr("chkUserPerm"));
	    bool getChPgN = s2i(opt->attr("getChPgN"));
	    bool noName = s2i(opt->attr("noName"));
	    for(unsigned iA = 0; iA < lst.size(); iA++) {
		if(chkUserPerm) {
		    AutoHD<Project> prj = prjAt(lst[iA]);
		    if(!SYS->security().at().access(opt->attr("user"),SEC_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()))
			continue;
		}
		XMLNode *no = opt->childAdd("el")->setAttr("id", lst[iA])->setText(noName?"":trD(prjAt(lst[iA]).at().name()));
		if(getChPgN) { prjAt(lst[iA]).at().list(lst1); no->setAttr("chPgN", i2s(lst1.size())); }
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    opt->setAttr("id", prjAdd(opt->attr("id"), opt->text()));
	    prjAt(opt->attr("id")).at().setOwner(opt->attr("user"));
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	prjDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/wlb_" || a_path == "/prm/cfg/wlb") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    wlbList(lst);
	    for(unsigned iA = 0; iA < lst.size(); iA++)
		opt->childAdd("el")->setAttr("id",lst[iA])->setText(trD(wlbAt(lst[iA]).at().name()));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))	opt->setAttr("id", wlbAdd(opt->attr("id"),opt->text()));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	wlbDel(opt->attr("id"),true);
    }
    else if(a_path == "/prm/cfg/cp/cp" && ctrChkNode(opt,"set",RWRW__,"root",SUI_ID,SEC_WR))
	nodeCopy(nodePath(0,true)+opt->attr("src"), nodePath(0,true)+opt->attr("dst"), opt->attr("user"));
    else if(a_path == "/br/ses_" || a_path == "/ses/ses") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    sesList(lst);
	    bool chkUserPerm = s2i(opt->attr("chkUserPerm"));
	    bool onlyMy = s2i(opt->attr("onlyMy"));
	    for(unsigned iA = 0; iA < lst.size(); iA++) {
		if(chkUserPerm) {
		    AutoHD<Project> prj = sesAt(lst[iA]).at().parent();
		    if(!SYS->security().at().access(opt->attr("user"),SEC_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()) ||
			    (onlyMy && opt->attr("user") != sesAt(lst[iA]).at().user()))
			continue;
		}
		opt->childAdd("el")->setAttr("user",sesAt(lst[iA]).at().user())->
				     setAttr("proj",sesAt(lst[iA]).at().projNm())->
				     setText(lst[iA]);
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->text(),TSYS::oscdID);
	    if(sesPresent(vid)) throw TError(nodePath().c_str(), _("Session '%s' is already present!"), vid.c_str());
	    sesAdd(vid); sesAt(vid).at().setUser(opt->attr("user")); sesAt(vid).at().setBackgrnd(true);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	sesDel(opt->text(),true);
    }
    else if(a_path == "/ses/ast") {
	string idcol = opt->attr("col").c_str();
	string idvl = opt->attr("key_id");
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/ses/ast/id","");
	    XMLNode *n_proj	= ctrMkNode("list",opt,-1,"/ses/ast/proj","");
	    XMLNode *n_user	= ctrMkNode("list",opt,-1,"/ses/ast/user","");

	    ResAlloc res(mSesRes, false);
	    for(map<string,string>::iterator isa = mSessAuto.begin(); isa != mSessAuto.end(); isa++) {
		if(n_id)	n_id->childAdd("el")->setText(isa->first);
		if(n_proj)	n_proj->childAdd("el")->setText(TSYS::strParse(isa->second,0,":"));
		if(n_user)	n_user->childAdd("el")->setText(TSYS::strParse(isa->second,1,":"));
	    }
	    return;
	}
	ResAlloc res(mSesRes, true);
	modif();
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))		mSessAuto["NewSessId"] = "";
	else if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	mSessAuto.erase(idvl);
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(idcol == "id") {
		mSessAuto[opt->text()] = mSessAuto[idvl];
		mSessAuto.erase(idvl);
	    }
	    if(idcol == "proj")	mSessAuto[idvl] = opt->text()+":"+TSYS::strParse(mSessAuto[idvl],1,":");
	    if(idcol == "user")	mSessAuto[idvl] = TSYS::strParse(mSessAuto[idvl],0,":")+":"+opt->text();
	}
    }
    else if(a_path == "/ses/usr_ls" && ctrChkNode(opt)) {
	vector<string> ls; SYS->security().at().usrList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++) opt->childAdd("el")->setText(ls[iL]);
    }
    else if(a_path == "/ses/prj_ls" && ctrChkNode(opt)) {
	vector<string> ls; prjList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++) opt->childAdd("el")->setText(ls[iL]);
    }
    else if(a_path == "/br/vca" && ctrChkNode(opt)) {
	vector<string> lst;
	fList(lst);
	for(unsigned iF = 0; iF < lst.size(); iF++)
	    opt->childAdd("el")->setAttr("id",lst[iF])->setText(fAt(lst[iF]).at().name());
    }
    else TUI::cntrCmdProc(opt);
}

AutoHD<TCntrNode> Engine::chldAt( int8_t igr, const string &name, const string &user ) const
{
    AutoHD<TCntrNode> nd = TCntrNode::chldAt(igr, name, user);
    if(igr == idPrj && !nd.freeStat()) {
	AutoHD<Project> prj = nd;
	if(!prj.freeStat() && !prj.at().enable() && !passAutoEn && prj.at().enableByNeed) {
	    prj.at().enableByNeed = false;
	    try {
		prj.at().modifG();
		prj.at().load();
		prj.at().setEnable(true);
		prj.at().modifGClr();
	    } catch(TError &err) { }
	}
    }

    return nd;
}

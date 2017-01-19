
//OpenSCADA system module UI.VCAEngine file: vcaengine.cpp
/***************************************************************************
 *   Copyright (C) 2006-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_NAME	_("Visual control area engine")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define MOD_SUBTYPE	"VCAEngine"
#define MOD_VER		"3.9.3"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("The main visual control area engine.")
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

string Engine::modInfo( const string &name )	{ return (name=="SubType") ? MOD_SUBTYPE : TModule::modInfo(name); }

void Engine::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(!(flag&TCntrNode::NodeConnect)) return;

    //Make lib's DB structure: Libs(__ID__, NAME, DSCR, DB_TBL, ICO)
    lbwdg_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"30"));
    lbwdg_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    lbwdg_el.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300"));
    lbwdg_el.fldAdd(new TFld("DB_TBL",_("DB table"),TFld::String,TFld::NoFlag,"30"));
    lbwdg_el.fldAdd(new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"100000"));

    //Make library widgets' data container: {LibWidgetMime,ProjMime}(__ID__, MIME, DATA)
    wdgdata_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30"));
    wdgdata_el.fldAdd(new TFld("MIME",_("Mime type"),TFld::String,TFld::NoFlag,"30"));
    wdgdata_el.fldAdd(new TFld("DATA",_("Mime data"),TFld::String,TFld::NoFlag,"500000"));

    //Make widgets' DB structure: LibWigets(__ID__, ICO, PARENT, PROC, PROC_PER, ATTRS, TIMESTAMP)
    wdg_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30"));
    wdg_el.fldAdd(new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000"));
    wdg_el.fldAdd(new TFld("PARENT",_("Parent widget"),TFld::String,TFld::NoFlag,"200"));
    wdg_el.fldAdd(new TFld("PR_TR",_("Procedure translation allow"),TFld::Boolean,TFld::NoFlag,"1","0"));
    wdg_el.fldAdd(new TFld("PROC",_("Procedure text and language"),TFld::String,TCfg::TransltText,"1000000"));
    wdg_el.fldAdd(new TFld("PROC_PER",_("Procedure calculate period"),TFld::Integer,TFld::NoFlag,"5","-1"));
    wdg_el.fldAdd(new TFld("ATTRS",_("Changed attributes"),TFld::String,TFld::NoFlag,"10000","*"));
    wdg_el.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //Make include widgets' DB structure: {LibWidgetIncl,ProjPageWIncl}(__IDW__, __ID__, PARENT, ATTRS)
    inclwdg_el.fldAdd(new TFld("IDW",_("IDW"),TFld::String,TCfg::Key,"100"));
    inclwdg_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30"));
    inclwdg_el.fldAdd(new TFld("PARENT",_("Parent widget"),TFld::String,TFld::NoFlag,"200"));
    inclwdg_el.fldAdd(new TFld("ATTRS",_("Changed attributes"),TFld::String,TFld::NoFlag,"10000","*"));

    //Make widget's IO DB structure: {LibWidgetIO,ProjPageIO}(__IDW__, __ID__, __IDC__, IO_VAL, SELF_FLG, CFG_TMPL, CFG_VAL)
    wdgio_el.fldAdd(new TFld("IDW",_("Widget ID"),TFld::String,TCfg::Key,"100"));
    wdgio_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"61"));
    wdgio_el.fldAdd(new TFld("IDC",_("Child ID"),TFld::String,TCfg::Key,"30"));
    wdgio_el.fldAdd(new TFld("IO_VAL",_("Attribute value"),TFld::String,TCfg::TransltText,"100000"));
    wdgio_el.fldAdd(new TFld("SELF_FLG",_("Attribute self flags"),TFld::Integer,TFld::NoFlag,"5"));
    wdgio_el.fldAdd(new TFld("CFG_TMPL",_("Configuration template"),TFld::String,TCfg::TransltText,"30"));
    wdgio_el.fldAdd(new TFld("CFG_VAL",_("Configuration value"),TFld::String,TCfg::TransltText,"1000"));

    //Make widget's user IO DB structure: {ProjPageUserIO,LibWidgetUserIO}(__IDW__, __ID__, __IDC__, NAME, IO_TP, IO_VAL, SELF_FLG, CFG_TMPL, CFG_VAL)
    wdguio_el.fldAdd(new TFld("IDW",_("Widget ID"),TFld::String,TCfg::Key,"100"));
    wdguio_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"61"));
    wdguio_el.fldAdd(new TFld("IDC",_("Child ID"),TFld::String,TCfg::Key,"30"));
    wdguio_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"100"));
    wdguio_el.fldAdd(new TFld("IO_TYPE",_("Attribute generic flags and type"),TFld::Integer,TFld::NoFlag,"10"));
    wdguio_el.fldAdd(new TFld("IO_VAL",_("Attribute value"),TFld::String,TCfg::TransltText,"100000"));
    wdguio_el.fldAdd(new TFld("SELF_FLG",_("Attribute self flags"),TFld::Integer,TFld::NoFlag,"5"));
    wdguio_el.fldAdd(new TFld("CFG_TMPL",_("Configuration template"),TFld::String,TCfg::TransltText,"30"));
    wdguio_el.fldAdd(new TFld("CFG_VAL",_("Configuration value"),TFld::String,TCfg::TransltText,"1000"));

    //Make project's DB structure: Projs(__ID__, NAME, DSCR, DB_TBL, ICO, USER, GRP, PERMIT, PER, FLGS, STYLE)
    prj_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"30"));
    prj_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    prj_el.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300"));
    prj_el.fldAdd(new TFld("DB_TBL",_("DB table"),TFld::String,TFld::NoFlag,"30"));
    prj_el.fldAdd(new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000"));
    prj_el.fldAdd(new TFld("USER",_("User"),TFld::String,TFld::NoFlag,OBJ_ID_SZ,"root"));
    prj_el.fldAdd(new TFld("GRP",_("Group"),TFld::String,TFld::NoFlag,OBJ_ID_SZ,"UI"));
    prj_el.fldAdd(new TFld("PERMIT",_("Permission"),TFld::Integer,TFld::OctDec,"3","436"));
    prj_el.fldAdd(new TFld("PER",_("Calculate period (ms)"),TFld::Integer,TFld::NoFlag,"4","100"));
    prj_el.fldAdd(new TFld("STYLE",_("Work style"),TFld::Integer,TFld::NoFlag,"2","-1"));
    prj_el.fldAdd(new TFld("EN_BY_NEED",_("Enable by need"),TFld::Boolean,TFld::NoFlag,"1","1"));

    //Make page's DB structure: ProjPages(__OWNER__, __ID__, ICO, PARENT, PROC, PROC_PER, FLGS, ATTRS, TIMESTAMP)
    page_el.fldAdd(new TFld("OWNER",_("Owner"),TFld::String,TCfg::Key,"100"));
    page_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30"));
    page_el.fldAdd(new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000"));
    page_el.fldAdd(new TFld("PARENT",_("Parent widget"),TFld::String,TFld::NoFlag,"200"));
    page_el.fldAdd(new TFld("PR_TR",_("Procedure translation allow"),TFld::Boolean,TFld::NoFlag,"1","0"));
    page_el.fldAdd(new TFld("PROC",_("Procedure text and language"),TFld::String,TCfg::TransltText,"1000000"));
    page_el.fldAdd(new TFld("PROC_PER",_("Procedure calculate period"),TFld::Integer,TFld::NoFlag,"5","-1"));
    page_el.fldAdd(new TFld("FLGS",_("Flags"),TFld::Integer,TFld::NoFlag,"1","0"));
    page_el.fldAdd(new TFld("ATTRS",_("Changed attributes"),TFld::String,TFld::NoFlag,"10000","*"));
    page_el.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //Make sessions' IO values of projects DB structure: ProjSess(__IDW__, __ID__, IO_VAL)
    prj_ses_el.fldAdd(new TFld("IDW",_("Widget ID"),TFld::String,TCfg::Key,"200"));
    prj_ses_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30"));
    prj_ses_el.fldAdd(new TFld("IO_VAL",_("Attribute value"),TFld::String,TFld::NoFlag,"100000"));

    //Make styles' IO DB structure: PrjStl(__ID__, V_0, V_1, V_2, V_3, V_4, V_5, V_6, V_7, V_8, V_9)
    prjStl_el.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30"));
    prjStl_el.fldAdd(new TFld("V_0",_("Value 0"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_1",_("Value 1"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_2",_("Value 2"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_3",_("Value 3"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_4",_("Value 4"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_5",_("Value 5"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_6",_("Value 6"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_7",_("Value 7"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_8",_("Value 8"),TFld::String,TFld::NoFlag,"100"));
    prjStl_el.fldAdd(new TFld("V_9",_("Value 9"),TFld::String,TFld::NoFlag,"100"));

    //Init original widgets library
    wlbAdd("originals",_("Original widgets"));
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
    for(unsigned l_id = 0; l_id < ls.size(); l_id++)
	sesAt(ls[l_id]).at().setEnable(false);

    //Projects disable
    prjList(ls);
    for(unsigned l_id = 0; l_id < ls.size(); l_id++)
	prjAt(ls[l_id]).at().setEnable(false);

    //Libraries disable
    wlbList(ls);
    for(unsigned l_id = 0; l_id < ls.size(); l_id++)
	wlbAt(ls[l_id]).at().setEnable(false);
    passAutoEn = false;

    TModule::preDisable(flag);
}

void Engine::load_( )
{
    int64_t d_tm = 0;

    if(mess_lev() == TMess::Debug) d_tm = TSYS::curTime();

    map<string, bool>	itReg;
    vector<vector<string> > full;

    passAutoEn = true;

    if(mess_lev() == TMess::Debug)	d_tm = TSYS::curTime();

    //Load widgets libraries
    try {
	// Search and create new libraries
	TConfig cEl(&elWdgLib());
	//cEl.cfgViewAll(false);
	vector<string> dbLs;

	// Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int libCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+wlbTable(),nodePath()+"LIB",libCnt++,cEl,false,&full); ) {
		string lId = cEl.cfg("ID").getS();
		if(!wlbPresent(lId)) wlbAdd(lId,"",(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		wlbAt(lId).at().load(&cEl);
		itReg[lId] = true;
		if(mess_lev() == TMess::Debug) {
		    mess_sys(TMess::Debug, _("Load library '%s' time: %f ms."), lId.c_str(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}
	    }

	// Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    wlbList(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(wlbAt(dbLs[i_it]).at().DB()))
		    wlbDel(dbLs[i_it]);
	}

	/*if(mess_lev() == TMess::Debug)	d_tm = TSYS::curTime();

	// Load present libraries
	wlbList(dbLs);
	for(unsigned lId = 0; lId < dbLs.size(); lId++) {
	    wlbAt(dbLs[lId]).at().load();
	    if(mess_lev() == TMess::Debug) {
		mess_sys(TMess::Debug, _("Load library '%s' time: %f ms."), dbLs[lId].c_str(), 1e-3*(TSYS::curTime()-d_tm));
		d_tm = TSYS::curTime();
	    }
	}*/
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Load widgets libraries error."));
    }

    //Load projects
    try {
	// Search and create new projects
	TConfig cEl(&elProject());
	//cEl.cfgViewAll(false);
	//cEl.cfg("EN_BY_NEED").setView(true);
	vector<string> dbLs;
	itReg.clear();

	// Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int lib_cnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+prjTable(),nodePath()+"PRJ",lib_cnt++,cEl,false,&full); ) {
		string prj_id = cEl.cfg("ID").getS();
		if(!prjPresent(prj_id)) {
		    prjAdd(prj_id,"",(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		    if(cEl.cfg("EN_BY_NEED").getB()) prjAt(prj_id).at().setEnableByNeed();
		}
		prjAt(prj_id).at().load(&cEl);
		itReg[prj_id] = true;
		if(mess_lev() == TMess::Debug) {
		    mess_sys(TMess::Debug, _("Load project '%s' time: %f ms."), prj_id.c_str(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}
	    }

	// Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    prjList(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(prjAt(dbLs[i_it]).at().DB()))
		    prjDel(dbLs[i_it]);
	}

	/*if(mess_lev() == TMess::Debug)	d_tm = TSYS::curTime();

	// Load present projects
	prjList(dbLs);
	for(unsigned el_id = 0; el_id < dbLs.size(); el_id++) {
	    prjAt(dbLs[el_id]).at().load();
	    if(mess_lev() == TMess::Debug) {
		mess_sys(TMess::Debug, _("Load project '%s' time: %f ms."), dbLs[el_id].c_str(), 1e-3*(TSYS::curTime()-d_tm));
		d_tm = TSYS::curTime();
	    }
	}*/
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Load projects error."));
    }

    //Libraries enable
    vector<string> ls;
    wlbList(ls);
    for(unsigned l_id = 0; l_id < ls.size(); l_id++) {
	wlbAt(ls[l_id]).at().setEnable(true);
	if(mess_lev() == TMess::Debug) {
	    mess_sys(TMess::Debug, _("Enable library '%s' time: %f ms."), ls[l_id].c_str(), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}
    }

    //Projects enable
    prjList(ls);
    for(unsigned l_id = 0; l_id < ls.size(); l_id++) {
	if(prjAt(ls[l_id]).at().enableByNeed)	continue;
	prjAt(ls[l_id]).at().setEnable(true);
	if(mess_lev() == TMess::Debug) {
	    mess_sys(TMess::Debug, _("Enable project '%s' time: %f ms."), ls[l_id].c_str(), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}
    }

    passAutoEn = false;

    //Auto-sessions load and enable
    ResAlloc res(mSesRes, true);
    XMLNode aSess("Sess");
    try {
	aSess.load(TBDS::genDBGet(nodePath()+"AutoSess"));
	for(unsigned i_n = 0; i_n < aSess.childSize(); i_n++) {
	    string sId	= aSess.childGet(i_n)->attr("id"),
		   sPrj	= aSess.childGet(i_n)->attr("prj"),
		   sUser= aSess.childGet(i_n)->attr("user");
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
    TBDS::genDBSet(nodePath()+"AutoSess",aSess.save());
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
    for(unsigned l_id = 0; l_id < ls.size(); l_id++)
	sesAt(ls[l_id]).at().setStart(true);

    runSt = true;
}

void Engine::modStop( )
{
    TModule::modStop();

    vector<string> ls;

    //Stop sessions
    sesList(ls);
    for(unsigned l_id = 0; l_id < ls.size(); l_id++)
	sesAt(ls[l_id]).at().setStart(false);

    runSt = false;
}

void Engine::wlbAdd( const string &iid, const string &inm, const string &idb )
{
    if(wlbPresent(iid))	return;
    chldAdd(idWlb, new WidgetLib(iid,inm,idb));
}

AutoHD<WidgetLib> Engine::wlbAt( const string &id ) const	{ return chldAt(idWlb,id); }

void Engine::prjAdd( const string &iid, const string &inm, const string &idb )
{
    if(prjPresent(iid))	return;
    chldAdd(idPrj, new Project(iid,inm,idb));
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
    string wdb = fullDB+"_io";
    string tbl = TSYS::strSepParse(wdb,2,';');

    vector<vector<string> > full;

    /*TConfig cEl(&elWdgIO());		//Here strong sequence needs to follow!
    cEl.cfg("IDW").setS(idw, true);
    cEl.cfg("IDC").setS(idc, true);
    cEl.cfg("IO_VAL").setExtVal(true);
    cEl.cfg("CFG_VAL").setExtVal(true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(wdb,nodePath()+tbl,fldCnt++,cEl,false,&full); ) {
	string tstr = cEl.cfg("ID").getS();
	if(attrs.find(tstr+";") == string::npos || !w.attrPresent(tstr)) continue;
	AutoHD<Attr> attr = w.attrAt(tstr);

	if(idw == "FormElTests" && idc == "FormEl3") printf("TEST 01: '%s'\n", tstr.c_str());

	if((ldGen && !(attr.at().flgGlob()&Attr::Generic)) ||
		(!ldGen && (attr.at().flgGlob()&Attr::Generic || (!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser))))
	    continue;

	if(idw == "FormElTests" && idc == "FormEl3") printf("TEST 02: '%s'\n", tstr.c_str());

	attr.at().setS(attr.at().isTransl()?cEl.cfg("IO_VAL").getS():cEl.cfg("IO_VAL").getS(TCfg::ExtValOne), true);
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)cEl.cfg("SELF_FLG").getI());
	attr.at().setCfgTempl(cEl.cfg("CFG_TMPL").getS());
	attr.at().setCfgVal(attr.at().isTransl()?cEl.cfg("CFG_VAL").getS():cEl.cfg("CFG_VAL").getS(TCfg::ExtValOne));
    }*/

    TConfig cEl(&elWdgIO());
    cEl.cfg("IDW").setS(idw);
    cEl.cfg("IDC").setS(idc);

    string tstr;
    for(int off = 0; !(tstr = TSYS::strSepParse(attrs,0,';',&off)).empty(); ) {
	if(!w.attrPresent(tstr)) continue;
	AutoHD<Attr> attr = w.attrAt(tstr);

	if((ldGen && !(attr.at().flgGlob()&Attr::Generic)) ||
		(!ldGen && (attr.at().flgGlob()&Attr::Generic || (!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser))))
	    continue;

	cEl.cfg("ID").setS(tstr);
	cEl.cfg("IO_VAL").setNoTransl(!attr.at().isTransl());
	cEl.cfg("CFG_VAL").setNoTransl(!attr.at().isTransl());

	if(!SYS->db().at().dataGet(wdb,nodePath()+tbl,cEl,false,true)) continue;

	attr.at().setS(cEl.cfg("IO_VAL").getS(),true);
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)cEl.cfg("SELF_FLG").getI());
	attr.at().setCfgTempl(cEl.cfg("CFG_TMPL").getS());
	attr.at().setCfgVal(cEl.cfg("CFG_VAL").getS());
    }

    if(ldGen)	return;

    //Load widget's user attributes
    wdb = fullDB+"_uio";
    tbl = TSYS::strSepParse(wdb,2,';');
    cEl.setElem(&elWdgUIO());
    cEl.cfg("IDW").setS(idw, true);
    cEl.cfg("IDC").setS(idc, true);
    cEl.cfg("IO_VAL").setNoTransl(false);
    cEl.cfg("CFG_VAL").setNoTransl(false);
    cEl.cfg("IO_VAL").setExtVal(true);
    cEl.cfg("CFG_VAL").setExtVal(true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(wdb,nodePath()+tbl,fldCnt++,cEl,false,&full); ) {
	string sid = cEl.cfg("ID").getS();
	if(!TSYS::pathLev(sid,1).empty()) continue;
	unsigned type = cEl.cfg("IO_TYPE").getI();
	unsigned flg = type >> 4;
	type = type&0x0f;
	unsigned selfFlg = cEl.cfg("SELF_FLG").getI();
	if(!w.attrPresent(sid)) w.attrAdd(new TFld(sid.c_str(),cEl.cfg("NAME").getS().c_str(),(TFld::Type)type,flg));
	AutoHD<Attr> attr = w.attrAt(sid);
	if(!(!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser)) continue;
	string IO_VAL = Attr::isTransl(TFld::Type(type),flg) ? cEl.cfg("IO_VAL").getS() : cEl.cfg("IO_VAL").getS(TCfg::ExtValOne);
	attr.at().setS(IO_VAL);
	if(type == TFld::Integer || type == TFld::Real || (flg&(TFld::Selected|TFld::SelEdit))) {
	    attr.at().setS(TSYS::strSepParse(IO_VAL,0,'|'));
	    attr.at().fld().setValues(TSYS::strSepParse(IO_VAL,1,'|'));
	    if(flg&(TFld::Selected|TFld::SelEdit)) attr.at().fld().setSelNames(TSYS::strSepParse(IO_VAL,2,'|'));
	}
	//!!!! Temporary placed for existing DBs clean up to early fix from using Values and Names to unproper types.
	else if(IO_VAL.size() >= 2 && IO_VAL.compare(IO_VAL.size()-2,2,"||") == 0) attr.at().setS(IO_VAL.substr(0,IO_VAL.size()-2));
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)selfFlg);
	attr.at().setCfgTempl(cEl.cfg("CFG_TMPL").getS());
	attr.at().setCfgVal(Attr::isTransl(TFld::Type(type),flg,selfFlg)?cEl.cfg("CFG_VAL").getS():cEl.cfg("CFG_VAL").getS(TCfg::ExtValOne));
    }
}

string Engine::attrsSave( Widget &w, const string &fullDB, const string &idw, const string &idc, bool ldGen )
{
    string tbl = TSYS::strSepParse(fullDB,2,';');
    string m_attrs = "";
    vector<string> als;

    w.attrList(als);
    TConfig cEl(&mod->elWdgIO()); cEl.cfg("IDW").setS(idw,true);
    cEl.cfg("IDC").setS(idc,true);
    TConfig cElu(&mod->elWdgUIO()); cElu.cfg("IDW").setS(idw,true);
    cElu.cfg("IDC").setS(idc,true);
    for(unsigned i_a = 0; i_a < als.size(); i_a++) {
	AutoHD<Attr> attr = w.attrAt(als[i_a]);
	if(!attr.at().modif()) continue;
	if(!(!(attr.at().flgSelf()&Attr::IsInher) && attr.at().flgGlob()&Attr::IsUser)) m_attrs += als[i_a]+";";
	if(ldGen != (bool)(attr.at().flgGlob()&Attr::Generic)) continue;

	//Main attributes store
	if(attr.at().flgSelf()&Attr::IsInher || !(attr.at().flgGlob()&Attr::IsUser)) {
	    cEl.cfg("ID").setS(als[i_a]);
	    cEl.cfg("IO_VAL").setNoTransl(!attr.at().isTransl());
	    cEl.cfg("IO_VAL").setS(attr.at().getS());
	    cEl.cfg("SELF_FLG").setI(attr.at().flgSelf());
	    cEl.cfg("CFG_TMPL").setS(attr.at().cfgTempl());
	    cEl.cfg("CFG_VAL").setNoTransl(!attr.at().isTransl(true));
	    cEl.cfg("CFG_VAL").setS(attr.at().cfgVal());
	    SYS->db().at().dataSet(fullDB+"_io",nodePath()+tbl+"_io",cEl,false,true);
	}
	//User attributes store
	else if(!ldGen) {
	    cElu.cfg("ID").setS(als[i_a]);
	    cElu.cfg("IO_VAL").setNoTransl(!attr.at().isTransl());
	    cElu.cfg("IO_VAL").setS(attr.at().getS());
	    if(attr.at().type() == TFld::Integer || attr.at().type() == TFld::Real || (attr.at().flgGlob()&(TFld::Selected|TFld::SelEdit))) {
		cElu.cfg("IO_VAL").setS(cElu.cfg("IO_VAL").getS()+"|"+attr.at().fld().values());
		if(attr.at().flgGlob()&(TFld::Selected|TFld::SelEdit)) cElu.cfg("IO_VAL").setS(cElu.cfg("IO_VAL").getS()+"|"+attr.at().fld().selNames());
	    }
	    cElu.cfg("NAME").setS(attr.at().name());
	    cElu.cfg("IO_TYPE").setI(attr.at().fld().type()+(attr.at().fld().flg()<<4));
	    cElu.cfg("SELF_FLG").setI(attr.at().flgSelf());
	    cElu.cfg("CFG_TMPL").setS(attr.at().cfgTempl());
	    cElu.cfg("CFG_VAL").setNoTransl(!attr.at().isTransl(true));
	    cElu.cfg("CFG_VAL").setS(attr.at().cfgVal());
	    SYS->db().at().dataSet(fullDB+"_uio",nodePath()+tbl+"_uio",cElu,false,true);
	}
    }

    if(!ldGen) {
	vector<vector<string> > full;
	//Clear no present IO for main io table
	cEl.cfgViewAll(false);
	for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB+"_io",nodePath()+tbl+"_io",fldCnt++,cEl,false,&full); ) {
	    string sid = cEl.cfg("ID").getS();
	    if(w.attrPresent(sid) || (idc.empty() && !TSYS::pathLev(sid,1).empty())) continue;

	    if(!SYS->db().at().dataDel(fullDB+"_io",nodePath()+tbl+"_io",cEl,true,false,true))	break;
	    if(full.empty()) fldCnt--;
	}

	//Clear no present IO for user io table
	cElu.cfgViewAll(false);
	for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB+"_uio",nodePath()+tbl+"_uio",fldCnt++,cElu,false,&full); ) {
	    string sid = cElu.cfg("ID").getS();
	    if(w.attrPresent(sid) || (idc.empty() && !TSYS::pathLev(sid,1).empty())) continue;

	    if(!SYS->db().at().dataDel(fullDB+"_uio",nodePath()+tbl+"_uio",cElu,true,false,true)) break;
	    if(full.empty()) fldCnt--;
	}
    }

    return m_attrs;
}

void Engine::perSYSCall( unsigned int cnt )
{
    //Check for stop and remove lost and not used already project's sessions
    vector<string> sls;
    sesList(sls);
    for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	if(!sesAt(sls[i_s]).at().backgrnd() && (time(NULL)-sesAt(sls[i_s]).at().reqTm()) > DIS_SES_TM)
	    sesDel(sls[i_s]);
}

void Engine::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path"), u = opt->attr("user");

    //Service commands process
    if(a_path == "/serv/sess") {	//Session operation
	if(ctrChkNode(opt,"list",RWRWRW,"root",SUI_ID,SEC_RD)) {	//List session for some project
	    string prj = opt->attr("prj");
	    vector<string> ls;
	    sesList(ls);
	    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
		if(prj.empty() || sesAt(ls[i_l]).at().projNm() == prj) {
		    AutoHD<Project> prj = sesAt(ls[i_l]).at().parent();
		    if(SYS->security().at().access(opt->attr("user"),SEC_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()))
			opt->childAdd("el")->setText(ls[i_l]);
		}
	}
	else if(ctrChkNode(opt,"connect",RWRWRW,"root",SUI_ID,SEC_WR)) {
	    string sess = opt->attr("sess");
	    string prj  = opt->attr("prj");

	    // User permission check
	    AutoHD<Project> wprj = (!sess.empty()) ? sesAt(sess).at().parent() : prjAt(prj);
	    if(!SYS->security().at().access(opt->attr("user"),SEC_RD,wprj.at().owner(),wprj.at().grp(),wprj.at().permit()))
		throw TError(nodePath().c_str(),_("Connection to session is not permitted for '%s'."),opt->attr("user").c_str());
	    // Connect to present session
	    if(!sess.empty()) {
		opt->setAttr("conId", i2s(sesAt(sess).at().connect()));
		opt->setAttr("prj", sesAt(sess).at().projNm());
		if(s2i(opt->attr("userChange")))
		    sesAt(sess).at().parent().at().mess_sys(TMess::Notice, _("User is changed to '%s', from '%s'."),
			opt->attr("user").c_str(), opt->attr("remoteSrcAddr").size()?opt->attr("remoteSrcAddr").c_str():"LocalHost");
	    }
	    // Create session
	    else if(!prj.empty()) {
		//  Prepare session name
		sess = prj;
		for(int p_cnt = 0; sesPresent(sess); p_cnt++) sess = prj + i2s(p_cnt);
		sesAdd(sess, prj);
		sesAt(sess).at().setUser(opt->attr("user"));
		sesAt(sess).at().setStart(true);
		opt->setAttr("conId", i2s(sesAt(sess).at().connect()));
		opt->setAttr("sess", sess);
		sesAt(sess).at().parent().at().mess_sys(TMess::Notice, _("User '%s' is connected, from '%s'."),
		    opt->attr("user").c_str(), opt->attr("remoteSrcAddr").size()?opt->attr("remoteSrcAddr").c_str():"LocalHost");
	    } else throw TError(nodePath().c_str(),_("Connect/create session arguments error."));
	    opt->setAttr("userIsRoot", SYS->security().at().access(opt->attr("user"),SEC_WR,"root","root",RWRWR_)?"1":"0");
	}
	else if(ctrChkNode(opt,"disconnect",RWRWRW,"root",SUI_ID,SEC_WR)) {
	    string sess = opt->attr("sess");
	    sesAt(sess).at().disconnect(s2i(opt->attr("conId")));
	    if(sesAt(sess).at().connects() == 0 && !sesAt(sess).at().backgrnd()) {
		sesAt(sess).at().parent().at().mess_sys(TMess::Notice, _("User '%s' is disconnected, from '%s'."),
		    opt->attr("user").c_str(), opt->attr("remoteSrcAddr").size()?opt->attr("remoteSrcAddr").c_str():"LocalHost");
		sesDel(sess);
	    }
	}
	return;
    }
    else if(a_path == "/serv/wlbBr" && ctrChkNode(opt,"get")) {
	string item = opt->attr("item");
	string upd_lb   = TSYS::pathLev(item,0);
	if(upd_lb.size() > 4 && upd_lb.substr(0,4) != "wlb_")	return;
	if(upd_lb.size() > 4)	upd_lb = upd_lb.substr(4);
	string upd_wdg  = TSYS::pathLev(item,1);
	if(upd_wdg.size() > 4)	upd_wdg = upd_wdg.substr(4);
	string upd_wdgi = TSYS::pathLev(item,2);
	if(upd_wdgi.size() > 4)	upd_wdgi = upd_wdgi.substr(4);

	// Widgets libraries
	vector<string> ls;
	wlbList(ls);
	for(unsigned i_wlb = 0; i_wlb < ls.size(); i_wlb++) {
	    if(!upd_lb.empty() && upd_lb != ls[i_wlb]) continue;
	    AutoHD<WidgetLib> wlb = wlbAt(ls[i_wlb]);
	    XMLNode *wlbN = opt->childAdd("wlb")->setAttr("id",ls[i_wlb])->setText(trU(wlb.at().name(),u));
	    wlbN->childAdd("ico")->setText(wlb.at().ico());

	    //  Widgets
	    vector<string> wls;
	    wlb.at().list(wls);
	    for(unsigned i_w = 0; i_w < wls.size(); i_w++) {
		if(!upd_wdg.empty() && upd_wdg != wls[i_w])	continue;
		AutoHD<LWidget> w = wlb.at().at(wls[i_w]);
		XMLNode *wN = wlbN->childAdd("w")->setAttr("id",wls[i_w])->setAttr("parent",w.at().parentNm())->setText(trU(w.at().name(),u));
		wN->childAdd("ico")->setText(w.at().ico());

		//  Child widgets
		vector<string> cwls;
		w.at().wdgList(cwls);
		if(cwls.size() < 1000)
		    for(unsigned i_c = 0; i_c < cwls.size(); i_c++) {
			if(!upd_wdgi.empty() && upd_wdgi != cwls[i_c])	continue;
			AutoHD<CWidget> cw = w.at().wdgAt(cwls[i_c]);
			wN->childAdd("cw")->setAttr("id",cwls[i_c])->setText(trU(cw.at().name(),u))->
			    childAdd("ico")->setText((cwls.size()>=100)?"":cw.at().ico());
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
	    if(ctrMkNode("table",opt,-1,"/ses/ast",_("Auto create and start"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id"))
	    {
		ctrMkNode("list",opt,-1,"/ses/ast/id",_("ID"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ses/ast/proj",_("Project"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/ses/prj_ls");
		ctrMkNode("list",opt,-1,"/ses/ast/user",_("User"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/ses/usr_ls");
	    }
	}
	return;
    }

    //Process command for page
    if(a_path == "/br/prj_" || a_path == "/prm/cfg/prj") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    prjList(lst);
	    bool chkUserPerm = s2i(opt->attr("chkUserPerm"));
	    for(unsigned i_a = 0; i_a < lst.size(); i_a++) {
		if(chkUserPerm) {
		    AutoHD<Project> prj = prjAt(lst[i_a]);
		    if(!SYS->security().at().access(opt->attr("user"),SEC_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()))
			continue;
		}
		opt->childAdd("el")->setAttr("id",lst[i_a])->setText(trU(prjAt(lst[i_a]).at().name(),u));
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    if(prjPresent(vid)) throw TError(nodePath().c_str(), _("Project '%s' already present!"), vid.c_str());
	    prjAdd(vid, opt->text()); prjAt(vid).at().setOwner(opt->attr("user"));
	    opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	prjDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/wlb_" || a_path == "/prm/cfg/wlb") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    wlbList(lst);
	    for(unsigned i_a=0; i_a < lst.size(); i_a++)
		opt->childAdd("el")->setAttr("id",lst[i_a])->setText(trU(wlbAt(lst[i_a]).at().name(),u));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"), TSYS::oscdID);
	    if(wlbPresent(vid)) throw TError(nodePath().c_str(), _("Widgets library '%s' already present!"), vid.c_str());
	    wlbAdd(vid, opt->text()); opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	wlbDel(opt->attr("id"),true);
    }
    else if(a_path == "/prm/cfg/cp/cp" && ctrChkNode(opt,"set",RWRW__,"root",SUI_ID,SEC_WR))
	nodeCopy(nodePath(0,true)+opt->attr("src"), nodePath(0,true)+opt->attr("dst"), opt->attr("user"));
    else if(a_path == "/br/ses_" || a_path == "/ses/ses") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    sesList(lst);
	    bool chkUserPerm = s2i(opt->attr("chkUserPerm"));
	    for(unsigned i_a=0; i_a < lst.size(); i_a++) {
		if(chkUserPerm) {
		    AutoHD<Project> prj = sesAt(lst[i_a]).at().parent();
		    if(!SYS->security().at().access(opt->attr("user"),SEC_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()))
			continue;
		}
		opt->childAdd("el")->setAttr("user",sesAt(lst[i_a]).at().user())->
				     setAttr("proj",sesAt(lst[i_a]).at().projNm())->
				     setText(lst[i_a]);
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->text(),TSYS::oscdID);
	    if(sesPresent(vid)) throw TError(nodePath().c_str(), _("Session '%s' already present!"), vid.c_str());
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
	for(unsigned i_l = 0; i_l < ls.size(); i_l++) opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/ses/prj_ls" && ctrChkNode(opt)) {
	vector<string> ls; prjList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++) opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/br/vca" && ctrChkNode(opt)) {
	vector<string> lst;
	fList(lst);
	for(unsigned i_f = 0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(fAt(lst[i_f]).at().name());
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

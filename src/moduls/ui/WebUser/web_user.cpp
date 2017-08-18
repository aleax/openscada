
//OpenSCADA system module UI.WebUser file: web_user.cpp
/***************************************************************************
 *   Copyright (C) 2010-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <time.h>
#include <string.h>
#include <string>

#include <tsys.h>
#include <tmess.h>
#include <tsecurity.h>

#include "web_user.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"WebUser"
#define MOD_NAME	_("Web interface from user")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"WWW"
#define MOD_VER		"0.9.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allows you to create your own user web-interfaces in any language of OpenSCADA.")
#define LICENSE		"GPL2"
//*************************************************

WebUser::TWEB *WebUser::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_WebUser_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_WebUser_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new WebUser::TWEB(source);
	return NULL;
    }
}

using namespace WebUser;

//*************************************************
//* TWEB                                          *
//*************************************************
TWEB::TWEB( string name ) : TUI(MOD_ID), mDefPg("*")
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    //Reg export functions
    modFuncReg(new ExpFunc("void HTTP_GET(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
	"GET command processing from HTTP protocol!",(void(TModule::*)( )) &TWEB::HTTP_GET));
    modFuncReg(new ExpFunc("void HTTP_POST(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
	"POST command processing from HTTP protocol!",(void(TModule::*)( )) &TWEB::HTTP_POST));

    mPgU = grpAdd("up_");

    //User page DB structure
    mUPgEl.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mUPgEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mUPgEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    mUPgEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0") );
    mUPgEl.fldAdd(new TFld("PROG",_("Program"),TFld::String,TFld::FullText|TFld::TransltText,"1000000"));
    mUPgEl.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));
}

TWEB::~TWEB( )
{
    nodeDelAll();
}

void TWEB::uPgAdd( const string &iid, const string &db )
{
    chldAdd(mPgU, new UserPg(iid,db,&uPgEl()));
}

void TWEB::load_( )
{
    //Load DB
    // Search and create new user protocols
    try {
	TConfig gCfg(&uPgEl());
	//gCfg.cfgViewAll(false);
	vector<string> dbLs;
	map<string, bool> itReg;
	vector<vector<string> > full;

	//  Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+modId()+"_uPg",nodePath()+modId()+"_uPg",fldCnt++,gCfg,false,&full); ) {
		string id = gCfg.cfg("ID").getS();
		if(!uPgPresent(id)) uPgAdd(id,(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		uPgAt(id).at().load(&gCfg);
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    uPgList(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(uPgAt(dbLs[i_it]).at().DB()))
		    uPgDel(dbLs[i_it]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new user page error."));
    }

    setDefPg(TBDS::genDBGet(nodePath()+"DefPg",defPg()));
}

void TWEB::save_( )
{
    TBDS::genDBSet(nodePath()+"DefPg", defPg());
}

void TWEB::modStart( )
{
    vector<string> ls;
    uPgList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	if(uPgAt(ls[iN]).at().toEnable())
	    uPgAt(ls[iN]).at().setEnable(true);

    runSt = true;
}

void TWEB::modStop( )
{
    vector<string> ls;
    uPgList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	uPgAt(ls[iN]).at().setEnable(false);

    runSt = false;
}

string TWEB::httpHead( const string &rcode, int cln, const string &cnt_tp, const string &addattr )
{
    return  "HTTP/1.0 " + rcode + "\x0D\x0A"
	    "Date: " + atm2s(time(NULL),"%a, %d %b %Y %T %Z") + "\x0D\x0A"
	    "Server: " + PACKAGE_STRING + "\x0D\x0A"
	    "Accept-Ranges: bytes\x0D\x0A"
	    "Content-Length: " + i2s(cln) + "\x0D\x0A" +
	    (cnt_tp.empty()?string(""):("Content-Type: "+cnt_tp+";charset="+Mess->charset()+"\x0D\x0A")) +
	    addattr + "\x0D\x0A";
}

string TWEB::pgCreator( TProtocolIn *iprt, const string &cnt, const string &rcode, const string &httpattrs,
    const string &htmlHeadEls, const string &forceTmplFile, const string &lang )
{
    vector<TVariant> prms;
    prms.push_back(cnt); prms.push_back(rcode); prms.push_back(httpattrs); prms.push_back(htmlHeadEls); prms.push_back(forceTmplFile); prms.push_back(lang);

    return iprt->owner().objFuncCall("pgCreator", prms, "root").getS();
}

bool TWEB::pgAccess( TProtocolIn *iprt, const string &URL )
{
    vector<TVariant> prms; prms.push_back(URL);
    return iprt->owner().objFuncCall("pgAccess", prms, "root").getB();
}

#undef _
#define _(mess) mod->I18N(mess, ses.lang.c_str())

void TWEB::HTTP_GET( const string &urli, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    string rez, httpIt, tstr, sender = TSYS::strLine(iprt->srcAddr(), 0);
    AutoHD<UserPg> up, tup;
    map<string,string>::iterator prmEl;
    vector<string> sls;
    SSess ses(TSYS::strDecode(urli,TSYS::HttpURL), sender, user, vars, "");

    try {
	TValFunc funcV;

	//Find user protocol for using
	vector<string> upLs;
	uPgList(upLs);
	string uPg = TSYS::pathLev(ses.url, 0);
	if(uPg.empty()) uPg = defPg();
	for(unsigned iUp = 0; iUp < upLs.size(); iUp++) {
	    tup = uPgAt(upLs[iUp]);
	    if(!tup.at().enableStat() || tup.at().workProg().empty()) continue;
	    if(uPg == upLs[iUp]) { up = tup; break; }
	}
	if(up.freeStat()) {
	    if(uPg == "*") {
		page =	"<table class='work'>\n"
			// " <tr><td class='content'><p>"+_("Welcome to Web-users pages of OpenSCADA system.")+"</p></td></tr>\n"
			" <tr><th>"+string(_("Presented user's WEB-pages."))+"</th></tr>\n"
			" <tr><td class='content'><ul>\n";
		for(unsigned iP = 0; iP < upLs.size(); iP++)
		    if(uPgAt(upLs[iP]).at().enableStat() && pgAccess(iprt,sender+"/" MOD_ID "/"+upLs[iP]+"/"))
			page += "   <li><a href='"+upLs[iP]+"/'>"+uPgAt(upLs[iP]).at().name()+"</a></li>\n";
		page += "</ul></td></tr></table>\n";

		page = pgCreator(iprt, page, "200 OK", "", "", "", ses.lang);
		return;
	    }
	    else if(!(uPg=defPg()).empty() && uPg != "*") up = uPgAt(uPg);
	    else throw TError(nodePath().c_str(), _("Page is missing"));
	}

	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(up.at().workProg())).at());

	//Load inputs
	funcV.setS(1, "GET");
	funcV.setS(2, ses.url);
	funcV.setS(3, page);
	funcV.setS(4, sender);
	funcV.setS(5, user);
	funcV.setO(6, new TVarObj());
	for(prmEl = ses.vars.begin(); prmEl != ses.vars.end(); prmEl++)
	    funcV.getO(6).at().propSet(prmEl->first, prmEl->second);
	funcV.setO(7, new TVarObj());
	for(prmEl = ses.prm.begin(); prmEl != ses.prm.end(); prmEl++)
	    funcV.getO(7).at().propSet(prmEl->first, prmEl->second);
	funcV.setO(8, new TArrayObj());
	for(unsigned ic = 0; ic < ses.cnt.size(); ic++) {
	    XMLNodeObj *xo = new XMLNodeObj();
	    xo->fromXMLNode(ses.cnt[ic]);
	    AutoHD<TArrayObj>(funcV.getO(8)).at().propSet(i2s(ic),xo);
	}
	funcV.setO(9, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));
	funcV.setO(10, new TCntrNodeObj(AutoHD<TCntrNode>(iprt),"root"));

	//Call processing
	funcV.calc();

	//Get outputs
	rez = funcV.getS(0);
	page = funcV.getS(3);

	//HTTP properties prepare
	AutoHD<TVarObj> hVars = funcV.getO(6);
	hVars.at().propList(sls);
	bool cTp = false;
	for(unsigned iL = 0; iL < sls.size(); iL++) {
	    tstr = hVars.at().propGet(sls[iL]).getS();
	    if(sls[iL] == "Date" || sls[iL] == "Server" || sls[iL] == "Accept-Ranges" || sls[iL] == "Content-Length" ||
		((prmEl=ses.vars.find(sls[iL])) != ses.vars.end() && prmEl->second == tstr)) continue;
	    if(sls[iL] == "Content-Type") cTp = true;
	    httpIt += sls[iL] + ": " + tstr + "\x0D\x0A";
	}

	page = httpHead(rez, page.size(), (cTp?"":"text/html"), httpIt) + page;

	up.at().cntReq++;
    } catch(TError &err) {
	page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Page '%s' error: %s"),urli.c_str(),err.mess.c_str())+"</div>\n",
			       "404 Not Found", "", "", "", ses.lang);
    }
}

void TWEB::HTTP_POST( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    string rez, httpIt, tstr, sender = TSYS::strLine(iprt->srcAddr(), 0);
    AutoHD<UserPg> up, tup;
    map<string,string>::iterator prmEl;
    vector<string> sls;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL), sender, user, vars, page);

    try {
	TValFunc funcV;
	//Find user protocol for using
	vector<string> upLs;
	uPgList(upLs);
	string uPg = TSYS::pathLev(ses.url, 0);
	if(uPg.empty()) uPg = defPg();
	for(unsigned iUp = 0; iUp < upLs.size(); iUp++) {
	    tup = uPgAt(upLs[iUp]);
	    if(!tup.at().enableStat() || tup.at().workProg().empty()) continue;
	    if(uPg == upLs[iUp]) { up = tup; break; }
	}
	if(up.freeStat()) {
	    if(!(uPg=defPg()).empty() && uPg != "*") up = uPgAt(uPg);
	    else throw TError(nodePath().c_str(), _("Page is missing"));
	}
	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(up.at().workProg())).at());

	//Load inputs
	funcV.setS(1, "POST");
	funcV.setS(2, ses.url);
	funcV.setS(3, page);
	funcV.setS(4, sender);
	funcV.setS(5, user);
	funcV.setO(6, new TVarObj());
	for(prmEl = ses.vars.begin(); prmEl != ses.vars.end(); prmEl++)
	    funcV.getO(6).at().propSet(prmEl->first, prmEl->second);
	funcV.setO(7,new TVarObj());
	for(prmEl = ses.prm.begin(); prmEl != ses.prm.end(); prmEl++)
	    funcV.getO(7).at().propSet(prmEl->first, prmEl->second);
	funcV.setO(8, new TArrayObj());
	for(unsigned ic = 0; ic < ses.cnt.size(); ic++) {
	    XMLNodeObj *xo = new XMLNodeObj();
	    xo->fromXMLNode(ses.cnt[ic]);
	    AutoHD<TArrayObj>(funcV.getO(8)).at().propSet(i2s(ic), xo);
	}
	funcV.setO(9, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));
	funcV.setO(10, new TCntrNodeObj(AutoHD<TCntrNode>(iprt),"root"));

	//Call processing
	funcV.calc();

	//Get outputs
	rez = funcV.getS(0);
	page = funcV.getS(3);

	//HTTP properties prepare
	funcV.getO(6).at().propList(sls);
	bool cTp = false;
	for(unsigned iL = 0; iL < sls.size(); iL++) {
	    tstr = funcV.getO(6).at().propGet(sls[iL]).getS();
	    if(sls[iL] == "Date" || sls[iL] == "Server" || sls[iL] == "Accept-Ranges" || sls[iL] == "Content-Length" ||
		((prmEl=ses.vars.find(sls[iL])) != ses.vars.end() && prmEl->second == tstr)) continue;
	    if(sls[iL] == "Content-Type") cTp = true;
	    httpIt += prmEl->first + ": " + tstr + "\x0D\x0A";
	}

	page = httpHead(rez, page.size(), (cTp?"":"text/html"), httpIt) + page;

	up.at().cntReq++;
    } catch(TError &err) {
	page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Page '%s' error: %s"),url.c_str(),err.mess.c_str())+"</div>\n",
			       "404 Not Found", "", "", "", ses.lang);
    }
}

#undef _
#define _(mess) mod->I18N(mess)

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/up_",_("User page"),RWRWR_,"root",SUI_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,-1,"/prm/up",_("User pages"))) {
	    ctrMkNode("fld",opt,-1,"/prm/up/dfPg",_("Default page"),RWRWR_,"root",SUI_ID,4,"tp","str","idm","1","dest","select","select","/prm/up/cup");
	    ctrMkNode("list",opt,-1,"/prm/up/up",_("Pages"),RWRWR_,"root",SUI_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","up_","idSz",OBJ_ID_SZ);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/up/dfPg") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(defPg());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDefPg(opt->text());
    }
    else if(a_path == "/br/up_" || a_path == "/prm/up/up" || a_path == "/prm/up/cup") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(a_path == "/prm/up/cup")
		opt->childAdd("el")->setAttr("id","*")->setText(_("<Page index display>"));
	    vector<string> lst;
	    uPgList(lst);
	    for(unsigned iF = 0; iF < lst.size(); iF++)
		opt->childAdd("el")->setAttr("id",lst[iF])->setText(uPgAt(lst[iF]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    uPgAdd(vid); uPgAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	chldDel(mPgU,opt->attr("id"),-1,1);
    }
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* UserPrt                                       *
//*************************************************
UserPg::UserPg( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), cntReq(0), mId(cfg("ID")), mAEn(cfg("EN").getBd()), mEn(false), mTimeStamp(cfg("TIMESTAMP").getId()), mDB(idb),
    prgChOnEn(false)
{
    mId = iid;
}

UserPg::~UserPg( )
{
    try{ setEnable(false); } catch(...) { }
}

TCntrNode &UserPg::operator=( const TCntrNode &node )
{
    const UserPg *src_n = dynamic_cast<const UserPg*>(&node);
    if(!src_n) return *this;

    if(enableStat())	setEnable(false);

    //Copy parameters
    exclCopy(*src_n, "ID;");
    setDB(src_n->DB());

    return *this;
}

void UserPg::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(), owner().nodePath()+tbl(), *this, true);
}

TWEB &UserPg::owner( ) const	{ return *(TWEB*)nodePrev(); }

string UserPg::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : id();
}

string UserPg::tbl( ) const	{ return owner().modId()+"_uPg"; }

string UserPg::progLang( )
{
    string mProg = cfg("PROG").getS();
    return mProg.substr(0, mProg.find("\n"));
}

string UserPg::prog( )
{
    string mProg = cfg("PROG").getS();
    size_t lngEnd = mProg.find("\n");
    return mProg.substr((lngEnd==string::npos)?0:lngEnd+1);
}

void UserPg::setProgLang( const string &ilng )	{ cfg("PROG").setS(ilng+"\n"+prog()); modif(); }

void UserPg::setProg( const string &iprg )	{ cfg("PROG").setS(progLang()+"\n"+iprg); modif(); }

void UserPg::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(fullDB(), owner().nodePath()+tbl(), *this);
    }
}

void UserPg::save_( )
{
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(fullDB(), owner().nodePath()+tbl(), *this);
}

bool UserPg::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PROG" && enableStat())	prgChOnEn = true;
    modif();
    return true;
}

void UserPg::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntReq = 0;

    if(vl) {
	//Prepare and compile page function
	if(!prog().empty()) {
	    TFunction funcIO("upg_"+id());
	    funcIO.ioIns(new IO("rez",_("Result"),IO::String,IO::Return,"200 OK"), 0);
	    funcIO.ioIns(new IO("HTTPreq",_("HTTP request"),IO::String,IO::Default,"GET"), 1);
	    funcIO.ioIns(new IO("url",_("URL"),IO::String,IO::Default), 2);
	    funcIO.ioIns(new IO("page",_("Page"),IO::String,IO::Output), 3);
	    funcIO.ioIns(new IO("sender",_("Sender"),IO::String,IO::Default), 4);
	    funcIO.ioIns(new IO("user",_("User"),IO::String,IO::Default), 5);
	    funcIO.ioIns(new IO("HTTPvars",_("HTTP variables"),IO::Object,IO::Default), 6);
	    funcIO.ioIns(new IO("URLprms",_("URL's parameters"),IO::Object,IO::Default), 7);
	    funcIO.ioIns(new IO("cnts",_("Content items"),IO::Object,IO::Default), 8);
	    funcIO.ioIns(new IO("this",_("This object"),IO::Object,IO::Default), 9);
	    funcIO.ioIns(new IO("prt",_("Protocol's object"),IO::Object,IO::Default), 10);

	    mWorkProg = SYS->daq().at().at(TSYS::strSepParse(progLang(),0,'.')).at().
		compileFunc(TSYS::strSepParse(progLang(),1,'.'),funcIO,prog());
	} else mWorkProg = "";
    }

    mEn = vl; prgChOnEn = false;
}

string UserPg::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat()) {
	rez = _("Enabled. ");
	if(prgChOnEn) rez += TSYS::strMess(_("Modified, re-enable to apply! "));
	rez += TSYS::strMess(_("Requests %.4g."), cntReq);
    }

    return rez;
}

void UserPg::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("User page: ")+name());
	if(ctrMkNode("area",opt,-1,"/up",_("User page"))) {
	    if(ctrMkNode("area",opt,-1,"/up/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/up/st/status",_("Status"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/up/st/en_st",_("Enable"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/up/st/db",_("DB"),RWRWR_,"root",SUI_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/up/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/up/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/up/cfg",0,"root",SUI_ID,RWRWR_);
		ctrRemoveNode(opt, "/up/cfg/PROG");
		ctrRemoveNode(opt, "/up/cfg/TIMESTAMP");
	    }
	    if(ctrMkNode("area",opt,-1,"/prgm",_("Program"))) {
		ctrMkNode("fld",opt,-1,"/prgm/PROGLang",_("Program language"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/up/cfg/plangLs");
		ctrMkNode("fld",opt,-1,"/prgm/PROG",_("Program"),RWRWR_,"root",SUI_ID,4,"tp","str","SnthHgl","1","rows","10",
		    "help",_("Next attributes has defined for requests processing:\n"
			    "   'rez' - processing result (by default - 200 OK);\n"
			    "   'HTTPreq' - HTTP request method (GET,POST);\n"
			    "   'url' - requested URI;\n"
			    "   'page' - Get/Post page content;\n"
			    "   'sender' - request sender;\n"
			    "   'user' - auth user;\n"
			    "   'HTTPvars' - HTTP variables into Object;\n"
			    "   'URLprms' - URL's parameters into Object;\n"
			    "   'cnts' - content items for POST into Array<XMLNodeObj>;\n"
			    "   'this' - pointer to this page's object;\n"
			    "   'prt' - pointer to object of the input part of the HTTP protocol."));
	    }
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/up/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/up/st/en_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/up/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/up/st/timestamp" && ctrChkNode(opt))	opt->setText(i2s(timeStamp()));
    else if(a_path == "/up/cfg/plangLs" && ctrChkNode(opt)) {
	string tplng = progLang();
	int c_lv = 0;
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::strSepParse(tplng,0,'.',&c_off)).size(); c_lv++) {
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	vector<string> ls;
	switch(c_lv) {
	    case 0:
		SYS->daq().at().modList(ls);
		for(unsigned iL = 0; iL < ls.size(); )
		    if(!SYS->daq().at().at(ls[iL]).at().compileFuncLangs()) ls.erase(ls.begin()+iL);
		    else iL++;
		break;
	    case 1:
		if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
		    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(&ls);
		break;
	}
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    opt->childAdd("el")->setText(c_path+ls[iL]);
    }
    else if(a_path.substr(0,7) == "/up/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SUI_ID,RWRWR_);
    else if(a_path == "/prgm/PROGLang") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(progLang());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setProgLang(opt->text());
    }
    else if(a_path == "/prgm/PROG") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(prog());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    try {
		SYS->daq().at().at(TSYS::strParse(progLang(),0,".")).at().
				    compileFuncSynthHighl(TSYS::strParse(progLang(),1,"."),*opt);
	    } catch(...){ }
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars, const string &icontent ) :
    url(iurl), sender(isender), user(iuser), content(icontent)
{
    //URL parameters parse
    size_t prmSep = iurl.find("?");
    if(prmSep != string::npos) {
	url = iurl.substr(0,prmSep);
	string prms = iurl.substr(prmSep+1);
	string sprm;
	for(int iprm = 0; (sprm=TSYS::strSepParse(prms,0,'&',&iprm)).size(); ) {
	    if((prmSep=sprm.find("=")) == string::npos) prm[sprm] = "true";
	    else prm[sprm.substr(0,prmSep)] = sprm.substr(prmSep+1);
	}
    }

    //Variables parse
    for(size_t iV = 0, spos = 0; iV < ivars.size(); iV++)
	if((spos=ivars[iV].find(":")) != string::npos) {
	    string  var = sTrm(ivars[iV].substr(0,spos)),
		    val = sTrm(ivars[iV].substr(spos+1));
	    vars[var] = val;
	    if(var == "oscd_lang") lang = val;
	}

    //Content parse
    size_t pos = 0, spos = 0;
    const char *c_bound = "boundary=";
    const char *c_term = "\x0D\x0A";
    const char *c_end = "--";
    string boundary = vars["Content-Type"];
    if(boundary.empty() || (pos=boundary.find(c_bound,0)) == string::npos) return;
    pos += strlen(c_bound);
    boundary = boundary.substr(pos,boundary.size()-pos);
    if(boundary.empty()) return;

    for(pos = 0; true; ) {
	pos = content.find(boundary,pos);
	if(pos == string::npos || content.compare(pos+boundary.size(),2,c_end) == 0) break;
	pos += boundary.size()+strlen(c_term);

	cnt.push_back(XMLNode("Content"));

	// Get properties
	while(pos < content.size()) {
	    string c_head = content.substr(pos, content.find(c_term,pos)-pos);
	    pos += c_head.size()+strlen(c_term);
	    if(c_head.empty()) break;
	    if((spos=c_head.find(":")) == string::npos) return;
	    cnt[cnt.size()-1].setAttr(sTrm(c_head.substr(0,spos)), sTrm(c_head.substr(spos+1)));
	}

	if(pos >= content.size()) return;
	cnt[cnt.size()-1].setText(content.substr(pos,content.find(string(c_term)+c_end+boundary,pos)-pos));
    }
}

#undef _
#define _(mess) mod->I18N(mess, lang.c_str())

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
    list.push_back("Auth");
}

string TWEB::modInfo( const string &iname )
{
    string  name = TSYS::strParse(iname, 0, ":"),
	    lang = TSYS::strParse(iname, 1, ":");

    if(name == "SubType")	return SUB_TYPE;
    if(name == "Auth")		return "0";

    if(lang.size()) {
	if(name == "Name")	return _("Web interface from user");
	if(name == "Author")	return _("Roman Savochenko");
	if(name == "Description") return _("Allows you to create your own user web-interfaces in any language of OpenSCADA.");
    }

    return TModule::modInfo(name);
}

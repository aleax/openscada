
//OpenSCADA module UI.WebUser file: web_user.cpp
/***************************************************************************
 *   Copyright (C) 2010-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_NAME	_("User WWW-page")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"WWW"
#define MOD_VER		"1.0.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides for creating your own web-pages on internal OpenSCADA language.")
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
    mUPgEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mUPgEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mUPgEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    mUPgEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0") );
    mUPgEl.fldAdd(new TFld("PROG",_("Procedure"),TFld::String,TFld::FullText|TFld::TransltText,"1000000"));
    mUPgEl.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //User page data IO DB structure
    mUPgIOEl.fldAdd(new TFld("PG_ID",_("User page ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mUPgIOEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mUPgIOEl.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::TransltText,"100"));
}

TWEB::~TWEB( )
{
    nodeDelAll();
}

string TWEB::uPgAdd( const string &iid, const string &db )
{
    UserPg *obj = new UserPg(iid, db, &uPgEl());
    chldAdd(mPgU, obj);

    return obj->id();
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
	mess_err(nodePath().c_str(),_("Error searching and creating a new user page."));
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
	    try { uPgAt(ls[iN]).at().setEnable(true); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error starting the WWW-page '%s'."), ls[iN].c_str());
	    }

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

    return iprt->objFuncCall("pgCreator", prms, "root").getS();
}

bool TWEB::pgAccess( TProtocolIn *iprt, const string &URL )
{
    vector<TVariant> prms; prms.push_back(URL);
    return iprt->objFuncCall("pgAccess", prms, "root").getB();
}

#undef _
#define _(mess) mod->I18N(mess, ses.lang.c_str())

void TWEB::HTTP_GET( const string &urli, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    string rez, sender = TSYS::strLine(iprt->srcAddr(), 0);
    AutoHD<UserPg> up, tup;


    SSess ses(TSYS::strDecode(urli,TSYS::HttpURL), sender, user, vars, page);

    try {
	//Find user protocol for using
	vector<string> upLs;
	uPgList(upLs);
	string uPg = TSYS::pathLev(ses.url, 0);
	if(uPg.empty()) uPg = defPg();
	for(unsigned iUp = 0; iUp < upLs.size(); iUp++) {
	    tup = uPgAt(upLs[iUp]);
	    if(!tup.at().enableStat() /*|| tup.at().workProg().empty()*/) continue;
	    if(uPg == upLs[iUp]) { up = tup; break; }
	}
	if(up.freeStat()) {
	    if(uPg == "*") {
		page =	"<table class='work'>\n"
			// " <tr><td class='content'><p>"+_("Welcome to the WWW-pages of the OpenSCADA users.")+"</p></td></tr>\n"
			" <tr><th>"+string(_("Present WWW-pages of the users."))+"</th></tr>\n"
			" <tr><td class='content'><ul>\n";
		for(unsigned iP = 0; iP < upLs.size(); iP++)
		    if(uPgAt(upLs[iP]).at().enableStat() && pgAccess(iprt,sender+"/" MOD_ID "/"+upLs[iP]+"/"))
			page += "   <li><a href='"+upLs[iP]+"/'>"+uPgAt(upLs[iP]).at().name()+"</a></li>\n";
		page += "</ul></td></tr></table>\n";

		page = pgCreator(iprt, page, "200 OK", "", "", "", ses.lang);
		return;
	    }
	    else if(!(uPg=defPg()).empty() && uPg != "*") up = uPgAt(uPg);
	    else throw TError(nodePath().c_str(), _("The page is not present"));
	}

	up.at().HTTP("GET", ses, iprt);

	page = ses.page;

    } catch(TError &err) {
	page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Error the page '%s': %s"),urli.c_str(),err.mess.c_str())+"</div>\n",
			       "404 Not Found", "", "", "", ses.lang);
    }
}

void TWEB::HTTP_POST( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    string rez, sender = TSYS::strLine(iprt->srcAddr(), 0);
    AutoHD<UserPg> up, tup;
    map<string,string>::iterator prmEl;
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
	    if(!tup.at().enableStat() /*|| tup.at().workProg().empty()*/) continue;
	    if(uPg == upLs[iUp]) { up = tup; break; }
	}
	if(up.freeStat()) {
	    if(!(uPg=defPg()).empty() && uPg != "*") up = uPgAt(uPg);
	    else throw TError(nodePath().c_str(), _("The page is not present"));
	}

	up.at().HTTP("POST", ses, iprt);

	page = ses.page;

    } catch(TError &err) {
	page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Error the page '%s': %s"),url.c_str(),err.mess.c_str())+"</div>\n",
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
	ctrMkNode("grp",opt,-1,"/br/up_",_("User WWW-page"),RWRWR_,"root",SUI_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,-1,"/prm/up",_("User WWW-pages"))) {
	    ctrMkNode("fld",opt,-1,"/prm/up/dfPg",_("Default WWW-page"),RWRWR_,"root",SUI_ID,4,"tp","str","idm","1","dest","select","select","/prm/up/cup");
	    ctrMkNode("list",opt,-1,"/prm/up/up",_("WWW-pages"),RWRWR_,"root",SUI_ID,5,
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
		opt->childAdd("el")->setAttr("id","*")->setText(_("<Show of index of the pages>"));
	    vector<string> lst;
	    uPgList(lst);
	    for(unsigned iF = 0; iF < lst.size(); iF++)
		opt->childAdd("el")->setAttr("id",lst[iF])->setText(uPgAt(lst[iF]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))	{ opt->setAttr("id", uPgAdd(opt->attr("id"))); uPgAt(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	chldDel(mPgU,opt->attr("id"),-1,1);
    }
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* UserPrt                                       *
//*************************************************
UserPg::UserPg( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), TPrmTempl::Impl(this, ("WebUserPg_"+iid).c_str()), cntReq(0), isDAQTmpl(false),
    mId(cfg("ID")), mAEn(cfg("EN").getBd()), mEn(false), mTimeStamp(cfg("TIMESTAMP").getId()), mDB(idb),
    ioRez(-1), ioHTTPreq(-1), ioUrl(-1), ioPage(-1), ioSender(-1), ioUser(-1),
    ioHTTPvars(-1), ioURLprms(-1), ioCnts(-1), ioThis(-1), ioPrt(-1),
    chkLnkNeed(false)
{
    mId = iid;
}

UserPg::~UserPg( )
{
    try{ setEnable(false); } catch(...) { }
}

TCntrNode &UserPg::operator=( const TCntrNode &node )
{
    UserPg *src_n = const_cast<UserPg*>(dynamic_cast<const UserPg*>(&node));
    if(!src_n) return *this;

    if(enableStat())	setEnable(false);

    //Copy parameters
    exclCopy(*src_n, "ID;");
    setDB(src_n->DB());

    if(isDAQTmpl && src_n->enableStat()) {
	setEnable(true);

	//IO values copy
	ResAlloc res(cfgRes, false);
	ResAlloc res1(src_n->cfgRes, false);
	for(int iIO = 0; iIO < src_n->ioSize(); iIO++)
	    if(src_n->ioFlg(iIO)&TPrmTempl::CfgLink) lnkAddrSet(iIO, src_n->lnkAddr(iIO));
	    else setS(iIO, src_n->getS(iIO));

	chkLnkNeed = initLnks();
    }

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

void UserPg::HTTP( const string &req, SSess &s, TProtocolIn *iprt )
{
    try {
	string tstr, httpIt;
	map<string,string>::iterator prmEl;

	MtxAlloc res1(reqRes, true);
	ResAlloc res2(cfgRes, false);

	//Load inputs
	inputLinks();
	setS(ioHTTPreq, req);
	setS(ioUrl, s.url);
	setS(ioPage, s.page);
	if(ioSender >= 0) setS(ioSender, s.sender);
	if(ioUser >= 0) setS(ioUser, s.user);
	setO(ioHTTPvars, new TVarObj());
	for(prmEl = s.vars.begin(); prmEl != s.vars.end(); prmEl++)
	    getO(ioHTTPvars).at().propSet(prmEl->first, prmEl->second);
	if(ioURLprms >= 0) {
	    setO(ioURLprms, new TVarObj());
	    for(prmEl = s.prm.begin(); prmEl != s.prm.end(); prmEl++)
		getO(ioURLprms).at().propSet(prmEl->first, prmEl->second);
	}
	if(ioCnts >= 0) {
	    setO(ioCnts, new TArrayObj());
	    for(unsigned ic = 0; ic < s.cnt.size(); ic++) {
		XMLNodeObj *xo = new XMLNodeObj();
		xo->fromXMLNode(s.cnt[ic]);
		AutoHD<TArrayObj>(getO(ioCnts)).at().propSet(i2s(ic),xo);
	    }
	}
	if(ioThis >= 0) setO(ioThis, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));
	if(ioPrt >= 0) setO(ioPrt, new TCntrNodeObj(AutoHD<TCntrNode>(iprt),"root"));

	//Call processing
	setMdfChk(true);
	calc();

	//Get outputs
	if(ioThis >= 0) setO(ioThis, new TEValObj());
	if(ioPrt >= 0) setO(ioPrt, new TEValObj());
	outputLinks();

	string rez = getS(ioRez);
	s.page = getS(ioPage);

	//HTTP properties prepare
	AutoHD<TVarObj> hVars = getO(ioHTTPvars);
	vector<string> sls;
	hVars.at().propList(sls);
	bool cTp = false;
	for(unsigned iL = 0; iL < sls.size(); iL++) {
	    tstr = hVars.at().propGet(sls[iL]).getS();
	    if(sls[iL] == "Date" || sls[iL] == "Server" || sls[iL] == "Accept-Ranges" || sls[iL] == "Content-Length" ||
		((prmEl=s.vars.find(sls[iL])) != s.vars.end() && prmEl->second == tstr)) continue;
	    if(sls[iL] == "Content-Type") cTp = true;
	    httpIt += sls[iL] + ": " + tstr + "\x0D\x0A";
	}

	s.page = mod->httpHead(rez, s.page.size(), (cTp?"":"text/html"), httpIt) + s.page;

	cntReq++;

    } catch(TError &err) {
	MtxAlloc res1(reqRes, true);
	ResAlloc res2(cfgRes, false);
	if(func() && ioThis >= 0) setO(ioThis, new TEValObj());
	if(func() && ioPrt >= 0) setO(ioPrt, new TEValObj());
	throw;
    }
}

void UserPg::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(fullDB(), owner().nodePath()+tbl(), *this);
    }

    loadIO();
}

void UserPg::loadIO( )
{
    ResAlloc res(cfgRes, false);
    if(func() && isDAQTmpl) {
	//Load IO
	vector<vector<string> > full;
	vector<string> u_pos;
	TConfig cf(&owner().uPgIOEl());
	cf.cfg("PG_ID").setS(id(), TCfg::ForceUse);
	cf.cfg("VALUE").setExtVal(true);
	for(int ioCnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",ioCnt++,cf,false,&full); ) {
	    string sid = cf.cfg("ID").getS();
	    int iid = func()->ioId(sid);
	    if(iid < 0)	continue;

	    if(func()->io(iid)->flg()&TPrmTempl::CfgLink) lnkAddrSet(iid, cf.cfg("VALUE").getS());
	    else setS(iid, cf.cfg("VALUE").getS());
	}
	chkLnkNeed = initLnks();
    }
}

void UserPg::save_( )
{
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(fullDB(), owner().nodePath()+tbl(), *this);

    saveIO();
}

void UserPg::saveIO( )
{
    ResAlloc res(cfgRes, false);
    if(func() && isDAQTmpl) {
	//Save IO
	TConfig cf(&owner().uPgIOEl());
	cf.cfg("PG_ID").setS(id(), true);
	for(int iIO = 0; iIO < func()->ioSize(); iIO++) {
	    if(iIO == ioRez || iIO == ioHTTPreq || iIO == ioUrl || iIO == ioPage || iIO == ioSender || iIO == ioUser ||
		iIO == ioHTTPvars || iIO == ioURLprms || iIO == ioCnts || iIO == ioThis || iIO == ioPrt ||
		func()->io(iIO)->flg()&TPrmTempl::LockAttr) continue;
	    cf.cfg("ID").setS(func()->io(iIO)->id());
	    cf.cfg("VALUE").setNoTransl(func()->io(iIO)->type() != IO::String || (func()->io(iIO)->flg()&TPrmTempl::CfgLink));
	    if(func()->io(iIO)->flg()&TPrmTempl::CfgLink) cf.cfg("VALUE").setS(lnkAddr(iIO));  //f->io(iIO)->rez());
	    else cf.cfg("VALUE").setS(getS(iIO));
	    SYS->db().at().dataSet(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cf);
	}

	//Clear IO
	vector<vector<string> > full;
	cf.cfgViewAll(false);
	for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",fldCnt++,cf,false,&full); ) {
	    string sio = cf.cfg("ID").getS();
	    if(func()->ioId(sio) < 0) {
		if(!SYS->db().at().dataDel(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cf,true,false,true)) break;
		if(full.empty()) fldCnt--;
	    }
	}
    }
}

bool UserPg::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PROG") {
	string lfnc = TSYS::strParse(progLang(), 0, "."), wfnc = TSYS::strParse(progLang(), 1, ".");
	isDAQTmpl = SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc);
    }
    modif();
    return true;
}

void UserPg::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntReq = 0;

    ResAlloc res(cfgRes, true);

    if(vl) {
	//Connect to a DAQ template or prepare and compile a function of the input part
	string lfnc = TSYS::strParse(progLang(), 0, "."), wfnc = TSYS::strParse(progLang(), 1, ".");
	isDAQTmpl = SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc);
	if(isDAQTmpl) {
	    setFunc(&SYS->daq().at().tmplLibAt(lfnc).at().at(wfnc).at().func().at());
	    addLinksAttrs();
	    // Checking for requiered conditions to the template
	    try {
		if((ioRez=func()->ioId("rez")) >= 0 && func()->io(ioRez)->type() != IO::String)		ioRez = -1;
		if((ioHTTPreq=func()->ioId("HTTPreq")) >= 0 && func()->io(ioHTTPreq)->type() != IO::String)	ioHTTPreq = -1;
		if((ioUrl=func()->ioId("url")) >= 0 && func()->io(ioUrl)->type() != IO::String)		ioUrl = -1;
		if((ioPage=func()->ioId("page")) >= 0 && func()->io(ioPage)->type() != IO::String)	ioPage = -1;
		if((ioSender=func()->ioId("sender")) >= 0 && func()->io(ioSender)->type() != IO::String)ioSender = -1;
		if((ioUser=func()->ioId("user")) >= 0 && func()->io(ioUser)->type() != IO::String)	ioUser = -1;
		if((ioHTTPvars=func()->ioId("HTTPvars")) >= 0 && func()->io(ioHTTPvars)->type() != IO::Object)	ioHTTPvars = -1;
		if((ioURLprms=func()->ioId("URLprms")) >= 0 && func()->io(ioURLprms)->type() != IO::Object)	ioURLprms = -1;
		if((ioCnts=func()->ioId("cnts")) >= 0 && func()->io(ioCnts)->type() != IO::Object)	ioCnts = -1;
		if((ioThis=func()->ioId("this")) >= 0 && func()->io(ioThis)->type() != IO::Object)	ioThis = -1;
		if((ioPrt=func()->ioId("prt")) >= 0 && func()->io(ioPrt)->type() != IO::Object)		ioPrt = -1;

		if(!(ioRez >= 0 && ioHTTPreq >= 0 && ioUrl >= 0 && ioPage >= 0 && ioHTTPvars >=0))
		    throw err_sys(_("The template '%s' does not have one or more required attribute in the needed type: rez=%d, HTTPreq=%d, url=%d, HTTPvars=%d.\n"
			"See to the documentation and append their!"),
			progLang().c_str(), ioRez, ioHTTPreq, ioUrl, ioHTTPvars);
	    } catch(TError &err) { setFunc(NULL); throw; }
	}
	else {
	    TFunction funcIO("upg_"+id());
	    ioRez	= funcIO.ioAdd(new IO("rez",_("Result"),IO::String,IO::Return,"200 OK"));
	    ioHTTPreq	= funcIO.ioAdd(new IO("HTTPreq",_("HTTP request"),IO::String,IO::Default,"GET"));
	    ioUrl	= funcIO.ioAdd(new IO("url",_("URL"),IO::String,IO::Default));
	    ioPage	= funcIO.ioAdd(new IO("page",_("WWW-page"),IO::String,IO::Output));
	    ioSender	= funcIO.ioAdd(new IO("sender",_("Sender"),IO::String,IO::Default));
	    ioUser	= funcIO.ioAdd(new IO("user",_("User"),IO::String,IO::Default));
	    ioHTTPvars	= funcIO.ioAdd(new IO("HTTPvars",_("HTTP variables"),IO::Object,IO::Default));
	    ioURLprms	= funcIO.ioAdd(new IO("URLprms",_("URL's parameters"),IO::Object,IO::Default));
	    ioCnts	= funcIO.ioAdd(new IO("cnts",_("Content items"),IO::Object,IO::Default));
	    ioThis	= funcIO.ioAdd(new IO("this",_("This object"),IO::Object,IO::Default));
	    ioPrt	= funcIO.ioAdd(new IO("prt",_("Protocol's object"),IO::Object,IO::Default));

	    string workProg = SYS->daq().at().at(TSYS::strSepParse(progLang(),0,'.')).at().
		compileFunc(TSYS::strSepParse(progLang(),1,'.'),funcIO,prog());
	    setFunc(&((AutoHD<TFunction>)SYS->nodeAt(workProg)).at());
	}

	//Load IO
	loadIO();
    }
    else setFunc(NULL);

    mEn = vl;
}

string UserPg::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat()) {
	rez = _("Enabled. ");
	rez += TSYS::strMess(_("Requests %.4g."), cntReq);
    }

    return rez;
}

void UserPg::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("User WWW-page: ")+name());
	if(ctrMkNode("area",opt,-1,"/up",_("User WWW-page"))) {
	    if(ctrMkNode("area",opt,-1,"/up/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/up/st/status",_("Status"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/up/st/en_st",_("Enabled"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/up/st/db",_("DB"),RWRWR_,"root",SUI_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/up/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/up/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/up/cfg",0,"root",SUI_ID,RWRWR_);
		ctrRemoveNode(opt, "/up/cfg/PROG");
		ctrRemoveNode(opt, "/up/cfg/TIMESTAMP");
		ctrMkNode("fld",opt,-1,"/up/cfg/PROGLang",_("Procedure language or the DAQ-template"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/plang/list");
	    }
	    ResAlloc res(cfgRes, false);
	    if(ctrMkNode("area",opt,-1,"/prgm",_("Procedure"))) {
		if(func() && chkLnkNeed) chkLnkNeed = initLnks(true);
		if(func() && ctrMkNode("table",opt,-1,"/prgm/io",_("IO"),RWRW__,"root",SPRT_ID,1,"rows","5")) {
		    ctrMkNode("list",opt,-1,"/prgm/io/id",_("Identifier"),R_R___,"root",SPRT_ID,1, "tp","str");
		    ctrMkNode("list",opt,-1,"/prgm/io/nm",_("Name"),R_R___,"root",SPRT_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/prgm/io/tp",_("Type"),R_R___,"root",SPRT_ID,5,"tp","dec","idm","1","dest","select",
			"sel_id",TSYS::strMess("%d;%d;%d;%d;%d",IO::Real,IO::Integer,IO::Boolean,IO::String,IO::Object).c_str(),
			"sel_list",_("Real;Integer;Boolean;String;Object"));
		    ctrMkNode("list",opt,-1,"/prgm/io/vl",_("Value"),RWRW__,"root",SPRT_ID,1,"tp","str");
		}
		if(!isDAQTmpl)
		    ctrMkNode("fld",opt,-1,"/prgm/PROG",_("Procedure"),(enableStat()?R_R_R_:RWRWR_),"root",SUI_ID,4,"tp","str","SnthHgl","1","rows","10",
			"help",_("Next attributes are defined to process the requests:\n"
			    "   'rez' - processing result (by default - 200 OK);\n"
			    "   'HTTPreq' - HTTP-request method (GET,POST);\n"
			    "   'url' - requested URI;\n"
			    "   'page' - Get/Post page's content;\n"
			    "   'sender' - request sender;\n"
			    "   'user' - authenticated user;\n"
			    "   'HTTPvars' - HTTP variables into Object;\n"
			    "   'URLprms' - URL's parameters into Object;\n"
			    "   'cnts' - content items for POST into Array<XMLNodeObj>;\n"
			    "   'this' - pointer to object of this page;\n"
			    "   'prt' - pointer to object of the input part of the HTTP protocol."));
		else if(func()) TPrmTempl::Impl::cntrCmdProc(opt, "/prgm/cfg");
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
    else if(a_path == "/up/cfg/PROGLang") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(progLang());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setProgLang(opt->text());
    }
    else if(a_path == "/plang/list" && ctrChkNode(opt)) {
	TCntrNode::cntrCmdProc(opt);
	opt->childAdd("el")->setText("");
	//Templates
	vector<string> lls, ls;
	SYS->daq().at().tmplLibList(lls);
	for(unsigned iL = 0; iL < lls.size(); iL++) {
	    SYS->daq().at().tmplLibAt(lls[iL]).at().list(ls);
	    for(unsigned iT = 0; iT < ls.size(); iT++)
		opt->childAdd("el")->setText(lls[iL]+"."+ls[iT]);
	}
    }
    else if(a_path.substr(0,7) == "/up/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SUI_ID,RWRWR_);
    else if(a_path.find("/prgm") == 0) {
	ResAlloc res(cfgRes, false);
	if(func() && a_path == "/prgm/io") {
	    if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD)) {
		XMLNode *nId   = ctrMkNode("list",opt,-1,"/prgm/io/id","");
		XMLNode *nNm   = ctrMkNode("list",opt,-1,"/prgm/io/nm","");
		XMLNode *nType = ctrMkNode("list",opt,-1,"/prgm/io/tp","");
		XMLNode *nVal  = ctrMkNode("list",opt,-1,"/prgm/io/vl","");

		for(int id = 0; id < func()->ioSize(); id++) {
		    if(nId)	nId->childAdd("el")->setText(func()->io(id)->id());
		    if(nNm)	nNm->childAdd("el")->setText(func()->io(id)->name());
		    if(nType)	nType->childAdd("el")->setText(i2s(func()->io(id)->type()));
		    if(nVal)	nVal->childAdd("el")->setText(getS(id));
		}
	    }
	    if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR)) {
		int row = s2i(opt->attr("row"));
		string col = opt->attr("col");
		if(col == "vl") {
		    setS(row, opt->text());
		    lnkOutput(row, opt->text());
		}
		modif();
	    }
	}
	else if(a_path == "/prgm/PROG") {
	    if(ctrChkNode(opt,"get",(enableStat()?R_R_R_:RWRWR_),"root",SUI_ID,SEC_RD))	opt->setText(prog());
	    if(ctrChkNode(opt,"set",(enableStat()?R_R_R_:RWRWR_),"root",SUI_ID,SEC_WR))	setProg(opt->text());
	    if(ctrChkNode(opt,"SnthHgl",(enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID,SEC_RD))
		try {
		    SYS->daq().at().at(TSYS::strParse(progLang(),0,".")).at().
					compileFuncSynthHighl(TSYS::strParse(progLang(),1,"."),*opt);
		} catch(...){ }
	}
	else if(a_path.find("/prgm/cfg") == 0 && isDAQTmpl && func()) TPrmTempl::Impl::cntrCmdProc(opt, "/prgm/cfg");
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
	if(name == "Name")	return MOD_NAME;
	if(name == "Author")	return AUTHORS;
	if(name == "Description") return DESCRIPTION;
    }

    return TModule::modInfo(name);
}

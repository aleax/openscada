
//OpenSCADA file: tsecurity.cpp
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include <errno.h>
#include <unistd.h>

#include "tsys.h"
#include "tmess.h"
#include "tsecurity.h"

#if defined(HAVE_CRYPT_H)
# include <crypt.h>
#endif

using namespace OSCADA;

//*************************************************
//* TSecurity					  *
//*************************************************
TSecurity::TSecurity( ) : TSubSYS(SSEC_ID,_("Security"), false)
{
    mUsr = TCntrNode::grpAdd("usr_");
    mGrp = TCntrNode::grpAdd("grp_");

    //User BD structure
    userEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    userEl.fldAdd(new TFld("DESCR",_("Full name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    userEl.fldAdd(new TFld("LONGDESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"1000"));
    userEl.fldAdd(new TFld("PASS",_("Password"),TFld::String,0,"100"));
    userEl.fldAdd(new TFld("LANG",_("Language"),TFld::String,0,"50"));
    userEl.fldAdd(new TFld("PICTURE",_("User picture"),TFld::String,0,"100000"));

    //Group BD structure
    grpEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    grpEl.fldAdd(new TFld("DESCR",_("Full name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    grpEl.fldAdd(new TFld("LONGDESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"1000"));
    grpEl.fldAdd(new TFld("USERS",_("Users"),TFld::String,0,"1000000"));
}

void TSecurity::postEnable( int flag )
{
    TSubSYS::postEnable(flag);
}

TSecurity::~TSecurity( )
{
    nodeDelAll();
}

void TSecurity::usrGrpList( const string &name, vector<string> &list )
{
    vector<string> gls;
    list.clear();
    grpList(gls);
    for(unsigned iG = 0; iG < gls.size(); iG++)
	if(name == "root" || grpAt(gls[iG]).at().user(name))
	    list.push_back(gls[iG]);
}

string TSecurity::usrAdd( const string &name, const string &idb )
{
    string nm = chldAdd(mUsr, new TUser(TSYS::strEncode(sTrm(name),TSYS::oscdID),idb,&userEl));
    if(grpPresent("users")) grpAt("users").at().userAdd(nm);

    return nm;
}

void TSecurity::usrDel( const string &name, bool complete )
{
    if(usrAt(name).at().sysItem())	throw err_sys(_("System user removal is not possible."));
    chldDel(mUsr, name, -1, complete?NodeRemove:NodeNoFlg);
}

string TSecurity::grpAdd( const string &name, const string &idb )
{
    return chldAdd(mGrp, new TGroup(TSYS::strEncode(sTrm(name),TSYS::oscdID),idb,&grpEl));
}

void TSecurity::grpDel( const string &name, bool complete )
{
    if(grpAt(name).at().sysItem())	throw err_sys(_("System group removal is not possible."));
    chldDel(mGrp, name, -1, complete?NodeRemove:NodeNoFlg);
}

char TSecurity::access( const string &user, char mode, const string &owner, const string &groups, int access )
{
    char rez = 0;

    //Check owner permision
    if(user == "root" || user == owner) rez = ((access&0700)>>6)&mode;
    //Check other permision
    if(rez == mode)	return rez;
    rez |= (access&07)&mode;
    if(rez == mode)	return rez;
    //Check groups permision
    bool grpAccs = false;
    grpAccs = grpAt("root").at().user(user);
    string grp;
    for(int off = 0; !grpAccs && (grp=TSYS::strParse(groups,0,",",&off)).size(); )
	grpAccs = (grpPresent(grp) && grpAt(grp).at().user(user));
    if(grpAccs) rez |= ((access&070)>>3)&mode;

    //if(grpAt("root").at().user(user) || (grpPresent(group) && grpAt(group).at().user(user))) rez |= ((access&070)>>3)&mode;

    return rez;
}

void TSecurity::load_( )
{
    //Load commandline data

    //Load parameters

    //Load DB
    string	name;
    map<string, bool>	itReg;

    // Search and create new users
    try {
	TConfig g_cfg(&userEl);
	//g_cfg.cfgViewAll(false);
	vector<string> itLs;

	//  Search new into DB and Config-file
	TBDS::dbList(itLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    for(int fld_cnt = 0; TBDS::dataSeek(itLs[iIt]+"."+subId()+"_user",nodePath()+subId()+"_user",fld_cnt++,g_cfg,TBDS::UseCache); ) {
		name = g_cfg.cfg("NAME").getS();
		if(!usrPresent(name)) usrAdd(name, itLs[iIt]);
		if(usrAt(name).at().DB() == itLs[iIt]) usrAt(name).at().load(&g_cfg);
		usrAt(name).at().setDB(itLs[iIt], true);
		itReg[name] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    usrList(itLs);
	    for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		if(itReg.find(itLs[iIt]) == itReg.end() && SYS->chkSelDB(usrAt(itLs[iIt]).at().DB()))
		    usrDel(itLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and creating new users."));
    }

    // Search and create new user groups
    try {
	TConfig g_cfg(&grpEl);
	//g_cfg.cfgViewAll(false);
	vector<string> itLs;
	itReg.clear();

	//  Search new into DB and Config-file
	TBDS::dbList(itLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    for(int fld_cnt = 0; TBDS::dataSeek(itLs[iIt]+"."+subId()+"_grp",nodePath()+subId()+"_grp",fld_cnt++,g_cfg,TBDS::UseCache); ) {
		name = g_cfg.cfg("NAME").getS();
		if(!grpPresent(name)) grpAdd(name, itLs[iIt]);
		if(grpAt(name).at().DB() == itLs[iIt]) grpAt(name).at().load(&g_cfg);
		grpAt(name).at().setDB(itLs[iIt], true);
		itReg[name] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    grpList(itLs);
	    for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		if(itReg.find(itLs[iIt]) == itReg.end() && SYS->chkSelDB(grpAt(itLs[iIt]).at().DB()))
		    grpDel(itLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and creating new user groups."));
    }

    //Add surely users, groups and set their parameters, if its not loaded
    // Administrator
    if(!usrPresent("root")) {
	usrAdd("root");
	usrAt("root").at().setDescr(_("Administrator (superuser)!!!"));
	usrAt("root").at().setPass("openscada");
    }
    usrAt("root").at().setSysItem(true);
    // Simple user
    if(!usrPresent("user")) {
	usrAdd("user");
	usrAt("user").at().setDescr(_("Simple user."));
	usrAt("user").at().setPass("user");
    }
    usrAt("user").at().setSysItem(true);
    // Administrators group
    if(!grpPresent("root")) {
	grpAdd("root");
	grpAt("root").at().setDescr(_("Administrators group."));
	grpAt("root").at().userAdd("root");
    }
    grpAt("root").at().setSysItem(true);
    // Simple users group
    if(!grpPresent("users")) {
	grpAdd("users");
	grpAt("users").at().setDescr(_("Users group."));
	grpAt("users").at().userAdd("user");
    }
    grpAt("users").at().setSysItem(true);
}

string TSecurity::optDescr( )
{
    return TSYS::strMess(_(
	"======================= Subsystem \"Security\" options ====================\n\n"
	)) + TSubSYS::optDescr();
}

TVariant TSecurity::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    // int access(string user, int mode, string owner, string group, int access)
    //      - Check for <user> access to resource what owned by <owner> and <group> and <access> for <mode>.
    //  user - user for access check;
    //  mode - access mode (4-R, 2-W, 1-X);
    //  owner - resource owner;
    //  group - resource group;
    //  access - resource access mode (RWXRWXRWX - 0777).
    if(iid == "access" && prms.size() >= 5)
	return (int)access(prms[0].getS(), prms[1].getI(), prms[2].getS(), prms[3].getS(), prms[4].getI());

    return TCntrNode::objFuncCall(iid, prms, user_lang);
}

void TSecurity::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/usr_",_("User"),RWRWR_,"root",SSEC_ID,1,"idSz",i2s(limObjID_SZ).c_str());
	ctrMkNode("grp",opt,-1,"/br/grp_",_("Group"),RWRWR_,"root",SSEC_ID,1,"idSz",i2s(limObjID_SZ).c_str());
	if(ctrMkNode("area",opt,1,"/usgr",_("Users and groups"))) {
	    ctrMkNode("list",opt,-1,"/usgr/users",_("Users"),RWRWR_,"root",SSEC_ID,4,
		"tp","br", "s_com","add,del", "br_pref","usr_", "idSz",i2s(limObjID_SZ).c_str());
	    ctrMkNode("list",opt,-1,"/usgr/grps",_("Groups"),RWRWR_,"root",SSEC_ID,4,
		"tp","br", "s_com","add,del", "br_pref","grp_", "idSz",i2s(limObjID_SZ).c_str());
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/usr_" || a_path == "/usgr/users") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    vector<string> list;
	    usrList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setText(list[iA]);
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SSEC_ID,SEC_WR))	opt->setText(usrAdd(opt->text()));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SSEC_ID,SEC_WR))	usrDel(opt->text(), true);
    }
    else if(a_path == "/br/grp_" || a_path == "/usgr/grps") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    vector<string> list;
	    grpList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setText(list[iA]);
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SSEC_ID,SEC_WR))	opt->setText(grpAdd(opt->text()));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SSEC_ID,SEC_WR))	grpDel(opt->text(), true);
    }
    else TSubSYS::cntrCmdProc(opt);
}

//*************************************************
//* TUser                                         *
//*************************************************
TUser::TUser( const string &nm, const string &idb, TElem *el ) : TConfig(el), mName(cfg("NAME")), mLang(cfg("LANG")),
    mDB(idb), mSysIt(false)
{
    mName = nm;

#if defined(HAVE_CRYPT_H)
    setPass("");
#endif
}

TUser::~TUser( )
{

}

TCntrNode &TUser::operator=( const TCntrNode &node )
{
    const TUser *src_n = dynamic_cast<const TUser*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "NAME;");
    setDB(src_n->DB());

    return *this;
}

void TUser::setPass( const string &n_pass )
{
    char *tRez = NULL;
    string tPass = n_pass;
#if defined(HAVE_CRYPT_H)
    string salt = "$1$"+name();		//Use MD5
# if defined(__USE_GNU) && !defined(__UCLIBC__)
    crypt_data data;
    data.initialized = 0;
    tRez = crypt_r(n_pass.c_str(), salt.c_str(), &data);
    if(!tRez)	throw TError(_("Error crypt_r(): %s (%d)"), strerror(errno), errno);
    tPass = tRez;
# else
    MtxAlloc cRes(*SYS->commonLock("crypt"), true);
    tRez = crypt(n_pass.c_str(), salt.c_str());
    if(!tRez)	throw TError(_("Error crypt_r(): %s (%d)"), strerror(errno), errno);
    tPass = tRez;
# endif
#endif
    cfg("PASS").setS(tPass);
}

bool TUser::auth( const string &ipass, string *hash )
{
    char *tRez = NULL;
#if defined(HAVE_CRYPT_H)
    string pass = cfg("PASS").getS();
    string salt = (pass.compare(0,3,"$1$") == 0) ? "$1$"+name() : name();	//Check for MD5 or the old method
    if(hash && ipass.find(SEC_HASH_MAGIC) == 0)
	return (ipass.compare(strlen(SEC_HASH_MAGIC),pass.size(),pass) == 0);
# if defined(__USE_GNU) && !defined(__UCLIBC__)
    crypt_data data;
    data.initialized = 0;
    if(hash) {
	tRez = crypt_r(ipass.c_str(), salt.c_str(), &data);
	if(!tRez) { mess_sys(TMess::Error, _("Error crypt_r(): %s (%d)"), strerror(errno), errno); return false; }
	*hash = tRez;
    }
    tRez = crypt_r(ipass.c_str(), salt.c_str(), &data);
    if(!tRez) { mess_sys(TMess::Error, _("Error crypt_r(): %s (%d)"), strerror(errno), errno); return false; }
    return (pass == tRez);
# else
    MtxAlloc cRes(*SYS->commonLock("crypt"), true);
    if(hash) {
	tRez = crypt(ipass.c_str(), salt.c_str());
	if(!tRez) { mess_sys(TMess::Error, _("Error crypt(): %s (%d)"), strerror(errno), errno); return false; }
	*hash = tRez;
    }
    tRez = crypt(ipass.c_str(), salt.c_str());
    if(!tRez) { mess_sys(TMess::Error, _("Error crypt(): %s (%d)"), strerror(errno), errno); return false; }
    return (pass == tRez);
# endif
#else
    return (ipass == cfg("PASS").getS());
#endif
}

int TUser::permitCmpr( const string &user )
{
    if(owner().access(user,SEC_WR,"root","root",RWRWR_))
	return owner().access(name(),SEC_WR,"root","root",RWRWR_) ? 0 : 1;

    int rez = -1;
    try {
	AutoHD<TUser> secUser = owner().usrAt(user);
	vector<string> gList;
	owner().usrGrpList(name(), gList);
	rez = 0;
	// Checking the lesser or equal the pointed user
	for(unsigned iG = 0; !rez && iG < gList.size(); iG++)
	    if(!owner().grpAt(gList[iG]).at().user(user)) rez = -1;
	if(!rez) {
	    // Checking the greater or equal the pointed user
	    owner().usrGrpList(user, gList);
	    for(unsigned iG = 0; !rez && iG < gList.size(); iG++)
		if(!owner().grpAt(gList[iG]).at().user(name())) rez = 1;
	}
    } catch(TError&) { }

    return rez;
}

void TUser::postDisable( int flag )
{
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor), owner().nodePath()+tbl(), *this, TBDS::UseAllKeys);
	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }

    //Remove the user from the groups
    vector<string> gls;
    owner().usrGrpList(name(), gls);
    for(unsigned iG = 0; iG < gls.size(); iG++)
	owner().grpAt(gls[iG]).at().userDel(name());
}

TSecurity &TUser::owner( ) const	{ return *(TSecurity*)nodePrev(); }

string TUser::tbl( ) const		{ return string(owner().subId())+"_user"; }

bool TUser::cfgChange( TCfg &co, const TVariant &pc )
{
#if defined(HAVE_CRYPT_H)
    //Check password at it loading and changing for plain one to generate its hash
    if(co.name() == "PASS" && co.getS() != pc.getS() &&
	    co.getS().compare(0,3+vmin(8,name().size()),"$1$"+name().substr(0,vmin(8,name().size()))) != 0)
	setPass(co.getS());
#endif

    modif();
    return true;
}

void TUser::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), owner().nodePath()+tbl(), *this);
}

void TUser::save_( )
{
    TBDS::dataSet(fullDB(), owner().nodePath()+tbl(), *this);
    setDB(DB(), true);

    //Save used groups
    vector<string> ls;
    owner().grpList(ls);
    for(unsigned iG = 0; iG < ls.size(); iG++)
	owner().grpAt(ls[iG]).at().save();
}

TVariant TUser::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    // bool auth( string pass ) - authenticate the user for <pass>
    if(iid == "auth" && prms.size()) { return auth(prms[0].getS()); }
    // Array groups( ) - groups list of the user.
    else if(iid == "groups") {
	TArrayObj *rez = new TArrayObj();
	try {
	    vector<string> ls;
	    owner().grpList(ls);
	    for(unsigned iG = 0, iGa = 0; iG < ls.size(); iG++)
		if(owner().grpAt(ls[iG]).at().user(name()))
		    rez->arSet(iGa++, ls[iG]);
	} catch(...){ }

	return rez;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, TSYS::strLine(user_lang,0), RWRWR_, name()+":"+SSEC_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user_lang);
}

void TUser::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("User ")+name(),RWRWR_,name().c_str(),SSEC_ID);
	if(picture().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/prm",_("User"))) {
	    TConfig::cntrCmdMake(opt,"/prm",0,name().c_str(),SSEC_ID,RWRWR_);
	    //if(!Mess->translDyn()) ctrRemoveNode(opt,"/prm/LANG");
	    ctrMkNode("fld",opt,-1,"/prm/LANG",EVAL_STR,RWRWR_,name().c_str(),SSEC_ID,2,
		"dest","sel_ed", "sel_list",Mess->langBase().c_str());
	    ctrMkNode("img",opt,-1,"/prm/PICTURE",EVAL_STR,RWRWR_,name().c_str(),SSEC_ID,1,"v_sz","100");
	    ctrMkNode("fld",opt,-1,"/prm/PASS",EVAL_STR,RWRW__,name().c_str(),SSEC_ID);
	    if(ctrMkNode("table",opt,-1,"/prm/grps",_("Groups"),RWRWR_,"root",SSEC_ID,1,"key","grp")) {
		ctrMkNode("list",opt,-1,"/prm/grps/grp",_("Group"),R_R_R_,"root",SSEC_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/grps/vl",_("Include"),RWRWR_,"root",SSEC_ID,1,"tp","bool");
	    }
	    ctrMkNode("fld",opt,-1,"/prm/db",_("User DB"),RWRWR_,"root",SSEC_ID,4,
		"tp","str","dest","select","select","/db/list",
		"help",(string(TMess::labStor())+"\n"+TMess::labStorGen()).c_str());
	    if(DB(true).size())
		ctrMkNode("comm",opt,-1,"/prm/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SSEC_ID);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/auth" && ctrChkNode(opt)) {
	string hash;
	opt->setText(i2s(auth(opt->attr("password"),&hash)));
	if(hash.size()) opt->setAttr("hash", hash);
    }
    else if(a_path == "/prm/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSEC_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SSEC_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/ico" && ctrChkNode(opt)) opt->setText(picture());
    else if(a_path == "/prm/PASS") {
	if(ctrChkNode(opt,"get",RWRW__,name().c_str(),SSEC_ID,SEC_RD))	opt->setText("**********");
	if(ctrChkNode(opt,"set",RWRW__,name().c_str(),SSEC_ID,SEC_WR))	setPass(opt->text());
    }
    else if(a_path == "/prm/grps") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    // Prepare headers
	    XMLNode *grp = ctrMkNode("list",opt,-1,"/prm/grps/grp","",R_R_R_);
	    XMLNode *vl  = ctrMkNode("list",opt,-1,"/prm/grps/vl","",RWRWR_);
	    vector<string> ls;
	    owner().grpList(ls);
	    for(unsigned iG = 0; iG < ls.size(); iG++) {
		if(grp)	grp->childAdd("el")->setText(ls[iG]);
		if(vl)	vl->childAdd("el")->setText(i2s(owner().grpAt(ls[iG]).at().user(name())));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSEC_ID,SEC_WR)) {
	    if(s2i(opt->text())) owner().grpAt(opt->attr("key_grp")).at().userAdd(name());
	    else owner().grpAt(opt->attr("key_grp")).at().userDel(name());
	    modif();
	}
    }
    else if(a_path.compare(0,4,"/prm") == 0)
	TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,1), name().c_str(), SSEC_ID, RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TGroup					  *
//*************************************************
TGroup::TGroup( const string &nm, const string &idb, TElem *el ) : TConfig(el), mName(cfg("NAME")), mDB(idb), mSysIt(false)
{
    mName = nm;
}

TGroup::~TGroup( )
{

}

TCntrNode &TGroup::operator=( const TCntrNode &node )
{
    const TGroup *src_n = dynamic_cast<const TGroup*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "NAME;");
    setDB(src_n->DB());

    return *this;
}

void TGroup::postDisable( int flag )
{
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor), owner().nodePath()+tbl(), *this, TBDS::UseAllKeys);
	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

TSecurity &TGroup::owner( ) const	{ return *(TSecurity*)nodePrev(); }

string TGroup::tbl( ) const		{ return owner().subId()+"_grp"; }

void TGroup::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), owner().nodePath()+tbl(), *this);
}

void TGroup::save_( )
{
    TBDS::dataSet(fullDB(), owner().nodePath()+tbl(), *this);
    setDB(DB(), true);
}

bool TGroup::user( const string &inm )
{
    if(owner().usrPresent(inm)) {
	string val;
	for(int off = 0; (val=TSYS::strParse(users(),0,";",&off)).size() || off < (int)users().size(); )
	    if(val == inm) return true;
    }
    return false;
}

void TGroup::userAdd( const string &name )
{
    if(name.size() && !user(name)) cfg("USERS").setS(users()+name+";");
}

void TGroup::userDel( const string &name )
{
    string tUsrs = users();
    size_t pos = tUsrs.find(name+";", 0);
    if(pos != string::npos)
	cfg("USERS").setS(tUsrs.erase(pos, name.size()+1));
}

TVariant TGroup::objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang )
{
    // bool user( string nm ) - check for the user including to the group.
    if(id == "user" && prms.size())	return user(prms[0].getS());

    //Configuration functions call
    TVariant cfRez = objFunc(id, prms, TSYS::strLine(user_lang,0), RWRWR_, "root:" SSEC_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(id, prms, user_lang);
}

void TGroup::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Group ")+name(),RWRWR_,"root",SSEC_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Group"))) {
	    TConfig::cntrCmdMake(opt,"/prm",0,"root",SSEC_ID,RWRWR_);
	    ctrMkNode("list",opt,-1,"/prm/USERS",EVAL_STR,RWRWR_,"root",SSEC_ID,1,"s_com","add,del");
	    ctrMkNode("fld",opt,-1,"/prm/db",_("User group DB"),RWRWR_,"root",SSEC_ID,4,
		"tp","str","dest","select","select","/db/list",
		"help",(string(TMess::labStor())+"\n"+TMess::labStorGen()).c_str());
	    if(DB(true).size())
		ctrMkNode("comm",opt,-1,"/prm/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SSEC_ID);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSEC_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SSEC_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/prm/USERS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    string val;
	    for(int off = 0; (val=TSYS::strParse(users(),0,";",&off)).size() || off < (int)users().size(); )
		opt->childAdd("el")->setText(val);
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SSEC_ID,SEC_WR))	userAdd(opt->text());
	if(ctrChkNode(opt,"del",RWRWR_,"root",SSEC_ID,SEC_WR))	userDel(opt->text());
    }
    else if(a_path.compare(0,4,"/prm") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,1), "root", SSEC_ID, RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}


//OpenSCADA system file: tsecurity.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <unistd.h>
#include <crypt.h>

#include "tsys.h"
#include "tmess.h"
#include "tsecurity.h"

using namespace OSCADA;

//*************************************************
//* TSecurity					  *
//*************************************************
TSecurity::TSecurity( ) : TSubSYS(SSEC_ID,_("Security"),false)
{
    m_usr = TCntrNode::grpAdd("usr_");
    m_grp = TCntrNode::grpAdd("grp_");

    //User BD structure
    user_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    user_el.fldAdd(new TFld("DESCR",_("Full name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    user_el.fldAdd(new TFld("LONGDESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"1000"));
    user_el.fldAdd(new TFld("PASS",_("Password"),TFld::String,0,"100"));
    user_el.fldAdd(new TFld("LANG",_("Language"),TFld::String,0,"15"));
    user_el.fldAdd(new TFld("PICTURE",_("User picture"),TFld::String,0,"100000"));

    //Group BD structure
    grp_el.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    grp_el.fldAdd(new TFld("DESCR",_("Full name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    grp_el.fldAdd(new TFld("LONGDESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"1000"));
    grp_el.fldAdd(new TFld("USERS",_("Users"),TFld::String,0,"200"));
}

void TSecurity::postEnable(int flag)
{
    if(!(flag&TCntrNode::NodeRestore)) {
	//Add surely users, groups and set their parameters
	// Administrator
	usrAdd("root");
	usrAt("root").at().setDescr(_("Administrator (superuser)!!!"));
	usrAt("root").at().setSysItem(true);
	usrAt("root").at().setPass("openscada");
	// Simple user
	usrAdd("user");
	usrAt("user").at().setDescr(_("Simple user."));
	usrAt("user").at().setSysItem(true);
	usrAt("user").at().setPass("user");
	// Administrators group
	grpAdd("root");
	grpAt("root").at().setDescr(_("Administrators group."));
	grpAt("root").at().setSysItem(true);
	grpAt("root").at().userAdd("root");
	// Simple users group
	grpAdd("users");
	grpAt("users").at().setDescr(_("Users group."));
	grpAt("users").at().setSysItem(true);
	grpAt("users").at().userAdd("user");
	grpAt("users").at().userAdd("root");
    }

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
    for(unsigned i_g = 0; i_g < gls.size(); i_g++)
	if(grpAt(gls[i_g]).at().user(name))
	    list.push_back(gls[i_g]);
}

void TSecurity::usrAdd( const string &name, const string &idb )
{
    chldAdd(m_usr,new TUser(name,idb,&user_el));
    if(grpPresent("users")) grpAt("users").at().userAdd(name);
}

void TSecurity::usrDel( const string &name, bool complete )
{
    if(usrAt(name).at().sysItem())	throw TError(nodePath().c_str(),_("Removal of system user is inadmissible."));
    chldDel(m_usr,name,-1,complete);
}

void TSecurity::grpAdd( const string &name, const string &idb )
{
    chldAdd(m_grp,new TGroup(name,idb,&grp_el));
}

void TSecurity::grpDel( const string &name, bool complete )
{
    if(grpAt(name).at().sysItem())	throw TError(nodePath().c_str(),_("Removal of system group is inadmissible.")); 
    chldDel(m_grp,name,-1,complete);
}

char TSecurity::access( const string &user, char mode, const string &owner, const string &group, int access )
{
    char rez = 0;

    //Check owner permision
    if(user == "root" || user == owner) rez = ((access&0700)>>6)&mode;
    //Check other permision
    if(rez == mode)	return rez;
    rez |= (access&07)&mode;
    if(rez == mode)	return rez;
    //Check groupe permision
    if(grpAt(group).at().user(user) || grpAt("root").at().user(user)) rez|=((access&070)>>3)&mode;

    return rez;
}

void TSecurity::load_( )
{
    //Load commandline data
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters

    //Load DB
    string	name;
    map<string, bool>	itReg;

    // Search and create new users
    try {
	TConfig g_cfg(&user_el);
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;

	//  Search new into DB and Config-file
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_user",nodePath()+subId()+"_user",fld_cnt++,g_cfg); )
	    {
		name = g_cfg.cfg("NAME").getS();
		if(!usrPresent(name))	usrAdd(name,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[name] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    usrList(db_ls);
	    for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(usrAt(db_ls[i_it]).at().DB()))
		    usrDel(db_ls[i_it]);
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new users error."));
    }

    // Search and create new user groups
    try {
	TConfig g_cfg(&grp_el);
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;
	itReg.clear();

	//  Search new into DB and Config-file
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_grp",nodePath()+subId()+"_grp",fld_cnt++,g_cfg); )
	    {
		name = g_cfg.cfg("NAME").getS();
		if(!grpPresent(name))	grpAdd(name,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[name] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    grpList(db_ls);
	    for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(grpAt(db_ls[i_it]).at().DB()))
		    grpDel(db_ls[i_it]);
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new user's groups error."));
    }
}

string TSecurity::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= Subsystem \"Security\" options ====================\n\n"
	),nodePath().c_str());

    return buf;
}

TVariant TSecurity::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
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

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TSecurity::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/usr_",_("User"),RWRWR_,"root",SSEC_ID,1,"idSz",OBJ_ID_SZ);
	ctrMkNode("grp",opt,-1,"/br/grp_",_("Group"),RWRWR_,"root",SSEC_ID,1,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,1,"/usgr",_("Users and groups"))) {
	    ctrMkNode("list",opt,-1,"/usgr/users",_("Users"),RWRWR_,"root",SSEC_ID,4,"tp","br","s_com","add,del","br_pref","usr_","idSz",OBJ_ID_SZ);
	    ctrMkNode("list",opt,-1,"/usgr/grps",_("Groups"),RWRWR_,"root",SSEC_ID,4,"tp","br","s_com","add,del","br_pref","grp_","idSz",OBJ_ID_SZ);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/usr_" || a_path == "/usgr/users") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    vector<string> list;
	    usrList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setText(list[i_a]);
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SSEC_ID,SEC_WR))	usrAdd(TSYS::strEncode(opt->text(),TSYS::oscdID));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SSEC_ID,SEC_WR))	usrDel(opt->text(),true);
    }
    else if(a_path == "/br/grp_" || a_path == "/usgr/grps") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    vector<string> list;
	    grpList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setText(list[i_a]);
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SSEC_ID,SEC_WR))	grpAdd(TSYS::strEncode(opt->text(),TSYS::oscdID));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SSEC_ID,SEC_WR))	grpDel(opt->text(),true);
    }
    else TSubSYS::cntrCmdProc(opt);
}

//*************************************************
//* TUser                                         *
//*************************************************
TUser::TUser( const string &nm, const string &idb, TElem *el ) : TConfig(el), mName(cfg("NAME")), mLang(cfg("LANG")),
    m_db(idb), m_sysIt(false)
{
    mName = nm;
}

TUser::~TUser( )
{

}

TCntrNode &TUser::operator=( TCntrNode &node )
{
    TUser *src_n = dynamic_cast<TUser*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "NAME;");
    setDB(src_n->m_db);

    return *this;
}

void TUser::setPass( const string &n_pass )
{
    crypt_data data;
    data.initialized = 0;
    string salt = "$1$"+name();		//Use MD5
    cfg("PASS").setS(crypt_r(n_pass.c_str(),salt.c_str(),&data));
}

bool TUser::auth( const string &ipass )
{
    crypt_data data;
    data.initialized = 0;
    string pass = cfg("PASS").getS();
    string salt = (pass.compare(0,3,"$1$") == 0) ? "$1$"+name() : name();	//Check for MD5 or old method
    return (pass == crypt_r(ipass.c_str(),salt.c_str(),&data));
}

void TUser::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);

    //Remove the user from the groups
    vector<string> gls;
    owner().usrGrpList(name(), gls);
    for(unsigned i_g = 0; i_g < gls.size(); i_g++)
	owner().grpAt(gls[i_g]).at().userDel(name());
}

TSecurity &TUser::owner( )	{ return *(TSecurity*)nodePrev(); }

string TUser::tbl( )		{ return string(owner().subId())+"_user"; }

void TUser::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TUser::save_( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);

    //Save used groups
    vector<string> ls;
    owner().grpList(ls);
    for(unsigned i_g = 0; i_g < ls.size(); i_g++)
	owner().grpAt(ls[i_g]).at().save();
}

TVariant TUser::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
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
	    if(!Mess->translDyn()) ctrRemoveNode(opt,"/prm/LANG");
	    ctrMkNode("img",opt,-1,"/prm/PICTURE",EVAL_STR,RWRWR_,name().c_str(),SSEC_ID,1,"v_sz","100");
	    ctrMkNode("fld",opt,-1,"/prm/PASS",EVAL_STR,RWRW__,name().c_str(),SSEC_ID);
	    if(ctrMkNode("table",opt,-1,"/prm/grps",_("Groups"),RWRWR_,"root",SSEC_ID,1,"key","grp")) {
		ctrMkNode("list",opt,-1,"/prm/grps/grp",_("Group"),R_R_R_,"root",SSEC_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/grps/vl",_("Include"),RWRWR_,"root",SSEC_ID,1,"tp","bool");
	    }
	    ctrMkNode("fld",opt,-1,"/prm/db",_("User DB"),RWRWR_,"root",SSEC_ID,4,
		"tp","str","dest","select","select","/db/list","help",TMess::labDB());
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/auth" && ctrChkNode(opt))	opt->setText(i2s(auth(opt->attr("password"))));
    else if(a_path == "/prm/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSEC_ID,SEC_WR))	setDB(opt->text());
    }
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
	    for(unsigned i_g = 0; i_g < ls.size(); i_g++) {
		if(grp)	grp->childAdd("el")->setText(ls[i_g]);
		if(vl)	vl->childAdd("el")->setText(i2s(owner().grpAt(ls[i_g]).at().user(name())));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSEC_ID,SEC_WR)) {
	    if(s2i(opt->text())) owner().grpAt(opt->attr("key_grp")).at().userAdd(name());
	    else owner().grpAt(opt->attr("key_grp")).at().userDel(name());
	    modif();
	}
    }
    else if(a_path.compare(0,4,"/prm") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,1), name().c_str(), SSEC_ID, RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TGroup					  *
//*************************************************
TGroup::TGroup( const string &nm, const string &idb, TElem *el ) : TConfig(el), mName(cfg("NAME")), m_db(idb), m_sysIt(false)
{
    mName = nm;
}

TGroup::~TGroup( )
{

}

TCntrNode &TGroup::operator=( TCntrNode &node )
{
    TGroup *src_n = dynamic_cast<TGroup*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "NAME;");
    setDB(src_n->m_db);

    return *this;
}

void TGroup::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);
}

TSecurity &TGroup::owner( )	{ return *(TSecurity*)nodePrev(); }

string TGroup::tbl( )		{ return owner().subId()+"_grp"; }

void TGroup::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TGroup::save_( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

bool TGroup::user( const string &inm )
{
    if(owner().usrPresent(inm)) {
	string val;
	for(int off = 0; (val=TSYS::strSepParse(users(),0,';',&off)).size(); )
	    if(val == inm) return true;
    }
    return false;
}

void TGroup::userAdd( const string &name )
{
    if(!user(name)) cfg("USERS").setS(users()+name+";");
    modif();
}

void TGroup::userDel( const string &name )
{
    string tUsrs = users();
    size_t pos = tUsrs.find(name+";", 0);
    if(pos != string::npos) {
	cfg("USERS").setS(tUsrs.erase(pos, name.size()+1));
	modif();
    }
}

TVariant TGroup::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
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
		"tp","str","dest","select","select","/db/list","help",TMess::labDB());
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSEC_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/USERS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSEC_ID,SEC_RD)) {
	    string val;
	    for(int off = 0; (val=TSYS::strSepParse(users(),0,';',&off)).size(); )
		opt->childAdd("el")->setText(val);
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SSEC_ID,SEC_WR))	userAdd(opt->text());
	if(ctrChkNode(opt,"del",RWRWR_,"root",SSEC_ID,SEC_WR))	userDel(opt->text());
    }
    else if(a_path.compare(0,4,"/prm") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,1), "root", SSEC_ID, RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}

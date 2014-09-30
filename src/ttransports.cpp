
//OpenSCADA system file: ttransports.cpp
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

#include <string.h>

#include "tsys.h"
#include "tmess.h"
#include "tmodule.h"
#include "tprotocols.h"
#include "ttransports.h"

using namespace OSCADA;

//************************************************
//* TTransportS					 *
//************************************************
TTransportS::TTransportS( ) : TSubSYS(STR_ID, "Transports", true)
{
    //Input transport BD structure
    el_in.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    el_in.fldAdd(new TFld("MODULE",_("Transport type"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    el_in.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    el_in.fldAdd(new TFld("DESCRIPT",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"500"));
    el_in.fldAdd(new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"100"));
    el_in.fldAdd(new TFld("PROT",_("Transport protocol"),TFld::String,TFld::NoFlag,i2s(s2i(OBJ_ID_SZ)*3).c_str()));
    el_in.fldAdd(new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1"));

    //Output transport BD structure
    el_out.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    el_out.fldAdd(new TFld("MODULE",_("Transport type"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    el_out.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    el_out.fldAdd(new TFld("DESCRIPT",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"500"));
    el_out.fldAdd(new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"100"));
    el_out.fldAdd(new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1"));

    //External hosts' connection DB struct
    el_ext.fldAdd(new TFld("OP_USER",_("Open user"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    el_ext.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    el_ext.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    el_ext.fldAdd(new TFld("TRANSP",_("Transport"),TFld::String,0,OBJ_ID_SZ));
    el_ext.fldAdd(new TFld("ADDR",_("Transport address"),TFld::String,0,"50"));
    el_ext.fldAdd(new TFld("USER",_("Request user"),TFld::String,0,OBJ_ID_SZ));
    el_ext.fldAdd(new TFld("PASS",_("Request password"),TFld::String,0,"30"));
}

TTransportS::~TTransportS( )
{

}

void TTransportS::inTrList( vector<string> &ls )
{
    ls.clear();

    vector<string> t_ls, m_ls;
    modList(t_ls);
    for(unsigned i_tp = 0; i_tp < t_ls.size(); i_tp++) {
	at(t_ls[i_tp]).at().inList(m_ls);
	for(unsigned i_t = 0; i_t < m_ls.size(); i_t++)
	    ls.push_back(t_ls[i_tp]+"."+m_ls[i_t]);
    }
}

void TTransportS::outTrList( vector<string> &ls )
{
    ls.clear();

    vector<string> t_ls, m_ls;
    modList(t_ls);
    for(unsigned i_tp = 0; i_tp < t_ls.size(); i_tp++) {
	at(t_ls[i_tp]).at().outList(m_ls);
	for(unsigned i_t = 0; i_t < m_ls.size(); i_t++)
	    ls.push_back(t_ls[i_tp]+"."+m_ls[i_t]);
    }
}

string TTransportS::extHostsDB( )	{ return SYS->workDB()+".CfgExtHosts"; }

void TTransportS::load_( )
{
    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters from config-file

    //Load DB
    string id, type;
    map<string, bool>	itReg;
    // Search and create new input transports
    try {
	TConfig c_el(&el_in);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//  Search new into DB and Config-file
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_in",nodePath()+subId()+"_in",fld_cnt++,c_el); )
	    {
		id   = c_el.cfg("ID").getS();
		type = c_el.cfg("MODULE").getS();
		if(modPresent(type) && !at(type).at().inPresent(id))
		    at(type).at().inAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    vector<string> m_ls;
	    modList(m_ls);
	    for(unsigned i_m = 0; i_m < m_ls.size(); i_m++) {
		at(m_ls[i_m]).at().inList(db_ls);
		for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		    if(itReg.find(m_ls[i_m]+"."+db_ls[i_it]) == itReg.end() && SYS->chkSelDB(at(m_ls[i_m]).at().inAt(db_ls[i_it]).at().DB()))
			at(m_ls[i_m]).at().inDel(db_ls[i_it]);
	    }
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new input transports error.")); 
    }

    // Search and create new output transports
    try {
	TConfig c_el(&el_out);
	c_el.cfgViewAll(false);
	vector<string> tdb_ls, db_ls;
	itReg.clear();

	//  Search new into DB and Config-file
	SYS->db().at().dbList(db_ls, true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_out",nodePath()+subId()+"_out",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODULE").getS();
		if(modPresent(type) && !at(type).at().outPresent(id))
		    at(type).at().outAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    vector<string> m_ls;
	    modList(m_ls);
	    for(unsigned i_m = 0; i_m < m_ls.size(); i_m++) {
		at(m_ls[i_m]).at().outList(db_ls);
		for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		    if(itReg.find(m_ls[i_m]+"."+db_ls[i_it]) == itReg.end() && SYS->chkSelDB(at(m_ls[i_m]).at().outAt(db_ls[i_it]).at().DB()))
			at(m_ls[i_m]).at().outDel(db_ls[i_it]);
	    }
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new input transports error."));
    }

    // Load external hosts
    try {
	TConfig c_el(&el_ext);
	for(int fld_cnt = 0; SYS->db().at().dataSeek(extHostsDB(),nodePath()+"ExtTansp",fld_cnt++,c_el,true); )
	{
	    ExtHost host("", "");
	    host.user_open	= c_el.cfg("OP_USER").getS();
	    host.id		= c_el.cfg("ID").getS();
	    host.name		= c_el.cfg("NAME").getS();
	    host.transp		= c_el.cfg("TRANSP").getS();
	    host.addr		= c_el.cfg("ADDR").getS();
	    host.user		= c_el.cfg("USER").getS();
	    host.pass		= c_el.cfg("PASS").getS();
	    extHostSet(host);
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and load external hosts DB error."));
    }
}

void TTransportS::save_( )
{
    //Save external transports
    ResAlloc res(extHostRes, false);
    TConfig c_el(&el_ext);
    for(unsigned i_h = 0; i_h < extHostLs.size(); i_h++) {
	c_el.cfg("OP_USER").setS(extHostLs[i_h].user_open);
	c_el.cfg("ID").setS(extHostLs[i_h].id);
	c_el.cfg("NAME").setS(extHostLs[i_h].name);
	c_el.cfg("TRANSP").setS(extHostLs[i_h].transp);
	c_el.cfg("ADDR").setS(extHostLs[i_h].addr);
	c_el.cfg("USER").setS(extHostLs[i_h].user);
	c_el.cfg("PASS").setS(extHostLs[i_h].pass);
	SYS->db().at().dataSet(extHostsDB(),nodePath()+"ExtTansp",c_el);
    }
    //Clear external transports
    c_el.cfgViewAll(false);
    for(int fld_cnt = 0; SYS->db().at().dataSeek(extHostsDB(),nodePath()+"ExtTansp",fld_cnt++,c_el,true); )
	if(!extHostGet(c_el.cfg("OP_USER").getS(),c_el.cfg("ID").getS()).id.size())
	{
	    SYS->db().at().dataDel(extHostsDB(),nodePath()+"ExtTansp",c_el,true,true,true);
	    fld_cnt--;
	}
}

void TTransportS::subStart( )
{
    mess_info(nodePath().c_str(),_("Start subsystem."));

    vector<string> t_lst, o_lst;
    modList(t_lst);
    for(unsigned i_t = 0; i_t < t_lst.size(); i_t++) {
	AutoHD<TTipTransport> mod = modAt(t_lst[i_t]);
	o_lst.clear();
	mod.at().inList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++)
	    try {
		AutoHD<TTransportIn> in = mod.at().inAt(o_lst[i_o]);
		if(!in.at().startStat() && in.at().toStart()) in.at().start();
	    }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Start input transport '%s' error."),o_lst[i_o].c_str());
	    }

	o_lst.clear();
	mod.at().outList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++)
	    try {
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if(!out.at().startStat() && out.at().toStart()) out.at().start();
	    }
	    catch(TError err) {
	        mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Start output transport '%s' error."),o_lst[i_o].c_str());
	    }
    }

    //Controllers start
    TSubSYS::subStart( );
}

void TTransportS::subStop( )
{
    mess_info(nodePath().c_str(),_("Stop subsystem."));

    vector<string> t_lst, o_lst;
    modList(t_lst);
    for(unsigned i_t = 0; i_t < t_lst.size(); i_t++) {
	AutoHD<TTipTransport> mod = modAt(t_lst[i_t]);
	o_lst.clear();
	mod.at().inList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++)
	    try {
		AutoHD<TTransportIn> in = mod.at().inAt(o_lst[i_o]);
		if(in.at().startStat()) in.at().stop();
	    }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Stop input transport '%s' error."),o_lst[i_o].c_str());
	    }
	o_lst.clear();
	mod.at().outList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++)
	    try {
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if(out.at().startStat()) out.at().stop();
	    }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Stop output transport '%s' error."),o_lst[i_o].c_str());
	    }
    }

    TSubSYS::subStop();
}

string TTransportS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= Subsystem \"Transports\" options ==================\n\n"
	),nodePath().c_str());

    return buf;
}

void TTransportS::extHostList( const string &user, vector<string> &list, bool andSYS )
{
    list.clear();
    ResAlloc res(extHostRes, false);
    for(unsigned i_h = 0; i_h < extHostLs.size(); i_h++)
	if(!user.size() || user == extHostLs[i_h].user_open || (andSYS && extHostLs[i_h].user_open == "*"))
	{
	    bool itSet = false;
	    for(vector<string>::iterator iL = list.begin(); (!user.size() || andSYS) && !itSet && iL != list.end(); ++iL)
		itSet = (*iL == extHostLs[i_h].id);
	    if(!itSet) list.push_back(extHostLs[i_h].id);
	}
}

bool TTransportS::extHostPresent( const string &user, const string &iid )
{
    ResAlloc res(extHostRes,false);
    for(unsigned i_h = 0; i_h < extHostLs.size(); i_h++)
	if((!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == iid)
	    return true;

    return false;
}

void TTransportS::extHostSet( const ExtHost &host, bool andSYS )
{
    ResAlloc res(extHostRes, true);
    int usrHstId = -1, sysHstId = -1;
    for(int i_h = 0; i_h < (int)extHostLs.size() && (usrHstId < 0 || sysHstId < 0); i_h++)
	if(extHostLs[i_h].id == host.id)
	{
	    if(host.mode < 0) { if(host.user_open == extHostLs[i_h].user_open) { usrHstId = i_h; break; } }
	    else if(extHostLs[i_h].user_open == host.user_open)	usrHstId = i_h;
	    else if(extHostLs[i_h].user_open == "*")		sysHstId = i_h;
	}
    if(host.mode < 0 || !andSYS) {
	if(usrHstId < 0) extHostLs.push_back(host);
	else extHostLs[usrHstId] = host;
    }
    else {
	//Append or update
	if(host.mode == ExtHost::User || host.mode == ExtHost::UserSystem) {
	    if(usrHstId < 0) extHostLs.push_back(host);
	    else extHostLs[usrHstId] = host;
	}
	if(host.mode == ExtHost::System || host.mode == ExtHost::UserSystem) {
	    if(sysHstId < 0) { extHostLs.push_back(host); extHostLs.back().user_open = "*"; }
	    else { extHostLs[sysHstId] = host; extHostLs[sysHstId].user_open = "*"; }
	}
	//Remove
	if(host.mode == ExtHost::User && sysHstId >= 0) extHostLs.erase(extHostLs.begin() + sysHstId);
	if(host.mode == ExtHost::System && usrHstId >= 0 && usrHstId != sysHstId) extHostLs.erase(extHostLs.begin() + usrHstId);
    }

    modif();
}

void TTransportS::extHostDel( const string &user, const string &id, bool andSYS )
{
    ResAlloc res(extHostRes, true);
    for(unsigned i_h = 0; i_h < extHostLs.size(); )
	if(extHostLs[i_h].id == id &&
		(!user.size() || user == extHostLs[i_h].user_open || (andSYS && extHostLs[i_h].user_open == "*")))
	    extHostLs.erase(extHostLs.begin()+i_h);
	else i_h++;
    modif();
}

TTransportS::ExtHost TTransportS::extHostGet( const string &user, const string &id, bool andSYS )
{
    ResAlloc res(extHostRes, false);
    ExtHost eh(user, "");
    for(unsigned i_h = 0; i_h < extHostLs.size(); i_h++)
	if(extHostLs[i_h].id == id &&
		(user.empty() || user == extHostLs[i_h].user_open || (andSYS && extHostLs[i_h].user_open == "*")))
	{
	    if(eh.mode < 0) {
		eh = extHostLs[i_h];
		eh.mode = (eh.user_open == "*") ? ExtHost::System : ExtHost::User;
	    }
	    else if(eh.user_open != extHostLs[i_h].user_open) { eh.mode = ExtHost::UserSystem; break; }
	}
    if(eh.user_open == "*" && user.size() && user != eh.user_open) eh.user_open = user;
    return eh;
}

AutoHD<TTransportOut> TTransportS::extHost( TTransportS::ExtHost host, const string &pref )
{
    if(!host.id.size() || !modPresent(host.transp))
	throw TError(nodePath().c_str(), _("Remote host '%s' error!"), host.id.c_str());

    if(!at(host.transp).at().outPresent(pref+host.id)) at(host.transp).at().outAdd(pref+host.id);
    if(at(host.transp).at().outAt(pref+host.id).at().addr() != host.addr) {
	at(host.transp).at().outAt(pref+host.id).at().setAddr(host.addr);
	at(host.transp).at().outAt(pref+host.id).at().stop();
    }

    return at(host.transp).at().outAt(pref+host.id);
}

int TTransportS::cntrIfCmd( XMLNode &node, const string &senderPref, const string &user )
{
    int path_off = 0;
    string path = node.attr("path");
    string station = TSYS::pathLev(path,0,false,&path_off);
    if(station.empty()) station = SYS->id();
    else node.setAttr("path",path.substr(path_off));

    if(station == SYS->id()) {
	node.setAttr("user",(user.empty()?"root":user));
	SYS->cntrCmd(&node);
	node.setAttr("path",path);
	return s2i(node.attr("rez"));
    }

    //Connect to transport
    TTransportS::ExtHost host = extHostGet((user.empty()?"*":user),station);
    AutoHD<TTransportOut> tr = extHost(host,senderPref);
    if(!tr.at().startStat()) tr.at().start(s2i(node.attr("conTm")));

    node.setAttr("rqDir","0")->setAttr("rqUser",host.user)->setAttr("rqPass",host.pass);
    tr.at().messProtIO(node,"SelfSystem");
    node.setAttr("path",path);

    return s2i(node.attr("rez"));
}

void TTransportS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),R_R_R_) &&
	    ctrMkNode("table",opt,-1,"/sub/ehost",_("External hosts poll"),RWRWRW,"root",STR_ID,2,"s_com","add,del","key","id"))
	{
	    ctrMkNode("list",opt,-1,"/sub/ehost/id",_("Id"),RWRWRW,"root",STR_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/sub/ehost/name",_("Name"),RWRWRW,"root",STR_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/sub/ehost/transp",_("Transport"),RWRWRW,"root",STR_ID,4,"tp","str",
		"idm","1","dest","select","select","/sub/transps");
	    ctrMkNode("list",opt,-1,"/sub/ehost/addr",_("Address"),RWRWRW,"root",STR_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/sub/ehost/user",_("User"),RWRWRW,"root",STR_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/sub/ehost/pass",_("Password"),RWRWRW,"root",STR_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/sub/ehost/mode",_("Mode"),RWRW__,"root",STR_ID,4,"tp","int","dest","select",
		"sel_id",TSYS::strMess("%d;%d;%d",ExtHost::User,ExtHost::System,ExtHost::UserSystem).c_str(),
		"sel_list",_("User;System;User and System"));
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user");
    if(a_path == "/sub/transps" && ctrChkNode(opt)) {
	vector<string>  list;
	modList(list);
	for(unsigned i_a = 0; i_a < list.size(); i_a++)
	    opt->childAdd("el")->setAttr("id",list[i_a])->setText(modAt(list[i_a]).at().modName());
    }
    else if(a_path == "/sub/ehost") {
	bool sysHostAcs = SYS->security().at().access(u,SEC_WR,"root",STR_ID,RWRWR_);
	if(ctrChkNode(opt,"get",RWRWRW,"root",STR_ID,SEC_RD)) {
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/sub/ehost/id","",RWRWRW,"root",STR_ID);
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/sub/ehost/name","",RWRWRW,"root",STR_ID);
	    XMLNode *n_tr	= ctrMkNode("list",opt,-1,"/sub/ehost/transp","",RWRWRW,"root",STR_ID);
	    XMLNode *n_addr	= ctrMkNode("list",opt,-1,"/sub/ehost/addr","",RWRWRW,"root",STR_ID);
	    XMLNode *n_user	= ctrMkNode("list",opt,-1,"/sub/ehost/user","",RWRWRW,"root",STR_ID);
	    XMLNode *n_pass	= ctrMkNode("list",opt,-1,"/sub/ehost/pass","",RWRWRW,"root",STR_ID);
	    XMLNode *n_mode	= sysHostAcs ? ctrMkNode("list",opt,-1,"/sub/ehost/mode","",RWRW__,"root",STR_ID) : NULL;

	    vector<string> list;
	    extHostList(u, list, n_mode);
	    for(unsigned i_h = 0; i_h < list.size(); i_h++) {
		ExtHost host = extHostGet(u, list[i_h], n_mode);
		if(n_id)	n_id->childAdd("el")->setText(host.id);
		if(n_nm)	n_nm->childAdd("el")->setText(trU(host.name,u));
		if(n_tr)	n_tr->childAdd("el")->setText(host.transp);
		if(n_addr)	n_addr->childAdd("el")->setText(host.addr);
		if(n_user)	n_user->childAdd("el")->setText(host.user);
		if(n_pass)	n_pass->childAdd("el")->setText(host.pass.size()?"*******":"");
		if(n_mode)	n_mode->childAdd("el")->setText(i2s(host.mode));
	    }
	}
	if(ctrChkNode(opt,"add",RWRWRW,"root",STR_ID,SEC_WR))	extHostSet(ExtHost(u,"newHost",_("New external host"),"","",u));
	if(ctrChkNode(opt,"del",RWRWRW,"root",STR_ID,SEC_WR))	extHostDel(u, opt->attr("key_id"), sysHostAcs);
	if(ctrChkNode(opt,"set",RWRWRW,"root",STR_ID,SEC_WR)) {
	    string col   = opt->attr("col");
	    ExtHost host = extHostGet(u, opt->attr("key_id"), sysHostAcs);
	    if(col == "id") {
		host.id = opt->text();
		extHostDel(u, opt->attr("key_id"), sysHostAcs);
	    }
	    else if(col == "name")  host.name = trSetU(host.name,u,opt->text());
	    else if(col == "transp")host.transp = opt->text();
	    else if(col == "addr")  host.addr = opt->text();
	    else if(col == "user")  host.user = opt->text();
	    else if(col == "pass")  host.pass = opt->text();
	    else if(col == "mode")  host.mode = s2i(opt->text());
	    extHostSet(host, sysHostAcs);
	}
    }
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTipTransport                                *
//************************************************
TTipTransport::TTipTransport( const string &id ) : TModule(id)
{
    mIn = grpAdd("in_");
    mOut = grpAdd("out_");
}

TTipTransport::~TTipTransport()
{
    nodeDelAll();
}

TTransportS &TTipTransport::owner( )	{ return (TTransportS&)TModule::owner(); }

void TTipTransport::inAdd( const string &name, const string &idb )	{ chldAdd(mIn, In(name,idb)); }

void TTipTransport::outAdd( const string &name, const string &idb )	{ chldAdd(mOut, Out(name,idb)); }

void TTipTransport::cntrCmdProc( XMLNode *opt )
{
    vector<string> list;
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/in_",_("Input transport"),RWRWR_,"root",STR_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	ctrMkNode("grp",opt,-1,"/br/out_",_("Output transport"),RWRWR_,"root",STR_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/tr",_("Transports"))) {
	    ctrMkNode("list",opt,-1,"/tr/in",_("Input"),RWRWR_,"root",STR_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","in_","idSz",OBJ_ID_SZ);
	    ctrMkNode("list",opt,-1,"/tr/out",_("Output"),RWRWR_,"root",STR_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","out_","idSz",OBJ_ID_SZ);
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/in_" || a_path == "/tr/in") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD)) {
	    inList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(inAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",STR_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    inAdd(vid); inAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",STR_ID,SEC_WR))	inDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/out_" || a_path == "/tr/out") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD)) {
	    outList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(outAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",STR_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    outAdd(vid); outAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",STR_ID,SEC_WR))	outDel(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* TTransportIn				 *
//************************************************
TTransportIn::TTransportIn( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), run_st(false), mId(cfg("ID")), mStart(cfg("START").getBd()), mDB(idb)
{
    mId = iid;
}

TTransportIn::~TTransportIn( )
{
    try{ stop(); }catch(...){ }
}

void TTransportIn::preEnable(int flag)
{
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

void TTransportIn::postDisable(int flag)
{
    if(flag) SYS->db().at().dataDel(fullDB(),SYS->transport().at().nodePath()+tbl(),*this,true);
}

bool TTransportIn::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR" && co.getS() != pc.getS()) stop();	//By the address change and reconnect need ordinary
    modif();

    return true;
}

TCntrNode &TTransportIn::operator=( TCntrNode &node )
{
    TTransportIn *src_n = dynamic_cast<TTransportIn*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "ID;");
    cfg("MODULE").setS(owner().modId());
    setDB(src_n->mDB);

    return *this;
}

TTipTransport &TTransportIn::owner( )	{ return *(TTipTransport*)nodePrev(); }

string TTransportIn::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TTransportIn::workId( )		{ return owner().modId()+"."+id(); }

string TTransportIn::tbl( )		{ return owner().owner().subId()+"_in"; }

string TTransportIn::protocol( )	{ return TSYS::strParse(protocolFull(),0,"."); }

string TTransportIn::getStatus( )	{ return startStat() ? _("Started. ") : _("Stoped. "); }

void TTransportIn::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    SYS->db().at().dataGet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
}

void TTransportIn::save_( )
{
    SYS->db().at().dataSet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
}

void TTransportIn::stop( )
{
    //Remove assigned output transports
    ResAlloc resN(nodeRes(), true);
    string oTrId;
    while(mAssTrO.size()) {
	oTrId = mAssTrO.back().at().id();
	mAssTrO.pop_back();
	try { owner().outDel(oTrId); }
	catch(TError er) { mess_err(nodePath().c_str(), _("Delete node error: %s"), er.mess.c_str()); }
    }
}

vector<AutoHD<TTransportOut> > TTransportIn::assTrs( )
{
    vector<AutoHD<TTransportOut> > rez;
    ResAlloc resN(nodeRes(), false);
    rez = mAssTrO;
    resN.unlock();

    return rez;
}

string TTransportIn::assTrO( const string &addr )
{
    ResAlloc resN(nodeRes(), true);
    string oTrId;
    int trFor = -1;
    //Find proper for replace and clean up stopped transports
    for(int i_ass = 0; i_ass < (int)mAssTrO.size(); i_ass++) {
	if(!mAssTrO[i_ass].freeStat() && mAssTrO[i_ass].at().startStat()) continue;
	if(trFor < 0) trFor = i_ass;
	else {
	    oTrId = mAssTrO[i_ass].at().id();
	    mAssTrO[i_ass].free();
	    try {
		owner().outDel(oTrId);
		mAssTrO.erase(mAssTrO.begin()+i_ass);
		i_ass--;
	    }
	    catch(TError er) {
		mAssTrO[i_ass] = owner().outAt(oTrId);
		mess_err(nodePath().c_str(), _("Delete node error: %s"), er.mess.c_str());
	    }
	}
    }

    //Create new assigned transport
    if(trFor < 0) {
	string assTrNm = "inAss"+id()+"_0";
	while(owner().outPresent(assTrNm)) assTrNm = TSYS::strLabEnum(assTrNm);
	owner().outAdd(assTrNm);
	mAssTrO.push_back(owner().outAt(assTrNm));
	trFor = mAssTrO.size()-1;
    }
    mAssTrO[trFor].at().setAddr(addr);
    mAssTrO[trFor].at().setName("");
    mAssTrO[trFor].at().setDscr("");
    mAssTrO[trFor].at().setPrm1(0);
    mAssTrO[trFor].at().setPrm2(0);
    mAssTrO[trFor].at().modifGClr();
    try{ mAssTrO[trFor].at().start(); }
    catch(TError er) { mess_err(nodePath().c_str(), _("Delete node error: %s"), er.mess.c_str()); }

    return mAssTrO[trFor].at().id();
}

TVariant TTransportIn::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // string writeTo(string sender, string mess) - send the message <mess> to sender <sender>
    //  sender - sender address
    //  mess - message for send
    if(iid == "writeTo" && prms.size() >= 2) {
	try { return writeTo(prms[0].getS(), prms[1].getS()); }	catch(TError) { }
	return 0;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TTransportIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Input transport: ")+name()).c_str(),RWRWR_,"root",STR_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),R_R_R_,"root",STR_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",STR_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB"),RWRWR_,"root",STR_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",STR_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/MODULE");
		ctrMkNode2("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,RWRWR_,"root",STR_ID,"dest","select","select","/prm/cfg/p_mod",NULL);
	    }
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/status" && ctrChkNode(opt))		opt->setText(getStatus());
    else if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(run_st?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	s2i(opt->text())?start():stop();
    }
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/cfg/p_mod" && ctrChkNode(opt)) {
	vector<string> list;
	int c_lv = 0;
	string c_path = "", c_el;
	for(int c_off = 0; (c_el=TSYS::strSepParse(protocolFull(),0,'.',&c_off)).size(); c_lv++)
	{
	    opt->childAdd("el")->setText(c_path);
	    c_path += c_lv ? "."+c_el : c_el;
	}
	opt->childAdd("el")->setText(c_path);
	if(c_lv == 0) SYS->protocol().at().modList(list);
	else {
	    c_path += ".";
	    SYS->protocol().at().at(protocol()).at().itemListIn(list,protocolFull());
	}
	for(unsigned i_a=0; i_a < list.size(); i_a++)
	    opt->childAdd("el")->setText(c_path+list[i_a]);
    }
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",STR_ID,RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* TTransportOut                                *
//************************************************
TTransportOut::TTransportOut( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), run_st(false), mId(cfg("ID")), mStart(cfg("START").getBd()),
    mDB(idb), mPrm1(0), mPrm2(0)
{
    mId = iid;
}

TTransportOut::~TTransportOut( )
{
    try{ stop(); }catch(...){ }
}

TCntrNode &TTransportOut::operator=( TCntrNode &node )
{
    TTransportOut *src_n = dynamic_cast<TTransportOut*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "ID;");
    cfg("MODULE").setS(owner().modId());
    setDB(src_n->mDB);

    return *this;
}

TTipTransport &TTransportOut::owner( )	{ return *(TTipTransport*)nodePrev(); }

string TTransportOut::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TTransportOut::workId( )		{ return owner().modId()+"."+id(); }

string TTransportOut::tbl( )		{ return owner().owner().subId()+"_out"; }

void TTransportOut::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(),SYS->transport().at().nodePath()+tbl(),*this,true);
}

bool TTransportOut::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR") stop();	//By the address change and reconnect need ordinary
    modif();

    return true;
}

string TTransportOut::getStatus( )	{ return startStat() ? _("Started. ") : _("Stoped. "); }

void TTransportOut::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    SYS->db().at().dataGet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
}

void TTransportOut::save_( )
{
    SYS->db().at().dataSet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
}

void TTransportOut::preEnable( int flag )
{
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

void TTransportOut::messProtIO( XMLNode &io, const string &prot )
{
    if(!SYS->protocol().at().modPresent(prot))
	throw TError(nodePath().c_str(),_("Transport protocol '%s' no present"),prot.c_str());
    SYS->protocol().at().at(prot).at().outMess(io, *this);
}

TVariant TTransportOut::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // string messIO(string mess, real timeOut = 0) - sending the message <mess> through the transport with the waiting timeout <timeOut>
    //  mess - message text for send
    //  timeOut - connection timeout, in seconds
    if(iid == "messIO" && prms.size() >= 1 && prms[0].type() != TVariant::Object) {
	string rez;
	char buf[STR_BUF_LEN];
	try {
	    if(!startStat()) start();
	    int resp_len = messIO(prms[0].getS().data(), prms[0].getS().size(), buf, sizeof(buf), (prms.size()>=2) ? (int)(1e3*prms[1].getR()) : 0);
	    rez.assign(buf,resp_len);
	} catch(TError) { return ""; }

	return rez;
    }
    // int messIO(XMLNodeObj req, string prt) - sending the request <req> to the protocol <prt> for the implementation of a connection
    //      session through the transport by means of protocol.
    //  req - request into XML-tree
    //  prt - protocol name
    else if(iid == "messIO" && prms.size() >= 2 && !AutoHD<XMLNodeObj>(prms[0].getO()).freeStat()) {
	try {
	    XMLNode req;
	    if(!startStat()) start();
	    AutoHD<XMLNodeObj>(prms[0].getO()).at().toXMLNode(req);
	    messProtIO(req,prms[1].getS());
	    AutoHD<XMLNodeObj>(prms[0].getO()).at().fromXMLNode(req);
	} catch(TError err) { return err.mess; }
	return 0;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TTransportOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Output transport: ")+name()).c_str(),RWRWR_,"root",STR_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),R_R_R_,"root",STR_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",STR_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB"),RWRWR_,"root",STR_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",STR_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/MODULE");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/req",_("Request"),RWRW__,"root",STR_ID)) {
	    ctrMkNode("fld",opt,-1,"/req/tm",_("Time"),R_R___,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/req/mode",_("Mode"),RWRW__,"root",STR_ID,4,"tp","dec","dest","select",
		"sel_id","0;1;2;3","sel_list",_("Binary;Text(LF);Text(CR);Text(CR/LF)"));
	    ctrMkNode("fld",opt,-1,"/req/toTmOut",_("Wait timeout"),RWRWR_,"root",STR_ID,2,"tp","bool","help",
		_("Sign for expect by timeout when a response is received.\n"
		  "Many systems in response to various protocols (HTTP) are send the response data in several pieces.\n"
		  "Without this flag will be received and displayed only the first piece.\n"
		  "When this flag will be set all the pieces awaiting an answer, until the lack of data during the timeout the transport elapsed ."));
	    ctrMkNode("comm",opt,-1,"/req/send",_("Send"),RWRW__,"root",STR_ID);
	    ctrMkNode("fld",opt,-1,"/req/req",_("Request"),RWRW__,"root",STR_ID,3,"tp","str","cols","90","rows","5");
	    ctrMkNode("fld",opt,-1,"/req/answ",_("Answer"),RWRW__,"root",STR_ID,3,"tp","str","cols","90","rows","5");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/status" && ctrChkNode(opt))		opt->setText(getStatus());
    else if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(run_st?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	s2i(opt->text())?start():stop();
    }
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",STR_ID,RWRWR_);
    else if(a_path == "/req/tm" && ctrChkNode(opt,"get",R_R___,"root",STR_ID,SEC_RD))
	opt->setText(TBDS::genDBGet(owner().nodePath()+"ReqTm","0",opt->attr("user")));
    else if(a_path == "/req/mode") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"ReqMode","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"ReqMode",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/req/toTmOut") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"ToTmOut","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"ToTmOut",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/req/req") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"ReqReq","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR)) {
	    int mode = s2i(TBDS::genDBGet(owner().nodePath()+"ReqMode","0",opt->attr("user")));
	    switch(mode) {
		case 0:
		    TBDS::genDBSet(owner().nodePath()+"ReqReq",
			TSYS::strDecode(TSYS::strEncode(opt->text(),TSYS::Bin),TSYS::Bin," "), opt->attr("user"));
		    break;
		default:
		    TBDS::genDBSet(owner().nodePath()+"ReqReq",opt->text(), opt->attr("user"));
		    break;
	    }
	}
    }
    else if(a_path == "/req/answ") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"ReqAnsw","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"ReqAnsw",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/req/send" && ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR)) {
	string answ;
	int mode = s2i(TBDS::genDBGet(owner().nodePath()+"ReqMode","0",opt->attr("user")));
	string req = TBDS::genDBGet(owner().nodePath()+"ReqReq","",opt->attr("user"));

	switch(mode) {
	    case 0:	req = TSYS::strEncode(req,TSYS::Bin);	break;
	    case 1:	//TEXT(LF)
		for(size_t i_p = 0; (i_p=req.find("\n",i_p)) != string::npos; i_p++)
		    req.replace(i_p, strlen("\n"), "\x0A");
		req = TSYS::strEncode(req, TSYS::ShieldSimb);
		break;
	    case 2:	//TEXT(CR)
		for(size_t i_p = 0; (i_p=req.find("\n",i_p)) != string::npos; i_p++)
		    req.replace(i_p, strlen("\n"), "\x0D");
		req = TSYS::strEncode(req, TSYS::ShieldSimb);
		break;
	    case 3:	//TEXT(CR/LF)
		for(size_t i_p = 0; (i_p=req.find("\n",i_p)) != string::npos; i_p+=2)
		    req.replace(i_p, strlen("\n"), "\x0D\x0A");
		req = TSYS::strEncode(req, TSYS::ShieldSimb);
		break;
	}

	int64_t stm = TSYS::curTime();
	try {
	    char buf[STR_BUF_LEN];
	    if(!startStat()) start();
	    ResAlloc resN(nodeRes(), true);
	    int resp_len = messIO(req.data(),req.size(),buf,sizeof(buf),0,true);
	    answ.assign(buf,resp_len);

	    bool ToTmOut = (bool)s2i(TBDS::genDBGet(owner().nodePath()+"ToTmOut","0",opt->attr("user")));
	    while(ToTmOut && resp_len > 0 && ((TSYS::curTime()-stm)/1000000) < STD_INTERF_TM) {
		try{ resp_len = messIO(NULL,0,buf,sizeof(buf),0,true); } catch(TError err) { break; }
		answ.append(buf,resp_len);
	    }

	    TBDS::genDBSet(owner().nodePath()+"ReqTm",tm2s(TSYS::curTime()-stm),opt->attr("user"));
	    TBDS::genDBSet(owner().nodePath()+"ReqAnsw",(mode==0)?TSYS::strDecode(answ,TSYS::Bin," "):answ,opt->attr("user"));
	}
	catch(TError err) {
	    TBDS::genDBSet(owner().nodePath()+"ReqTm",tm2s(TSYS::curTime()-stm),opt->attr("user"));
	    throw;
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

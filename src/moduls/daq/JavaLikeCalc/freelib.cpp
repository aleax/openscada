
//OpenSCADA module DAQ.JavaLikeCalc file: freelib.cpp
/***************************************************************************
 *   Copyright (C) 2005-2020 by Roman Savochenko, <rom_as@oscada.org>      *
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


#include <tsys.h>
#include <tmess.h>

#include "freefunc.h"
#include "virtual.h"
#include "freelib.h"

using namespace JavaLikeCalc;

//*************************************************
//* Lib: Functions library                        *
//*************************************************
Lib::Lib( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elLib()), runSt(false), workLibDb(lib_db), mId(cfg("ID"))
{
    mId = id;
    cfg("NAME").setS(name);
    cfg("DB").setS(string("flb_")+id);
    mFnc = grpAdd("fnc_");
    if(DB().empty()) modifClr();
}

Lib::~Lib( )
{

}

TCntrNode &Lib::operator=( const TCntrNode &node )
{
    const Lib *src_n = dynamic_cast<const Lib*>(&node);
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;");
    workLibDb = src_n->workLibDb;

    //Functions copy
    vector<string> ls;
    src_n->list(ls);
    for(unsigned i_p = 0; i_p < ls.size(); i_p++) {
	if(!present(ls[i_p])) add(ls[i_p].c_str());
	(TCntrNode&)at(ls[i_p]).at() = (TCntrNode&)src_n->at(ls[i_p]).at();
    }
    if(src_n->startStat() && !startStat()) setStart(true);

    return *this;
}

void Lib::preDisable( int flag )
{
    setStart(false);
}

void Lib::postDisable( int flag )
{
    if(flag && DB().size()) {
	//Delete libraries record
	SYS->db().at().dataDel(DB()+"."+mod->libTable(),mod->nodePath()+"lib/",*this,true);

	//Delete function's files
	SYS->db().at().open(fullDB());
	SYS->db().at().close(fullDB(),true);

	SYS->db().at().open(fullDB()+"_io");
	SYS->db().at().close(fullDB()+"_io",true);
    }
}

string Lib::name( )
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : mId;
}

void Lib::setFullDB( const string &idb )
{
    size_t dpos = idb.rfind(".");
    workLibDb = (dpos!=string::npos) ? idb.substr(0,dpos) : "";
    cfg("DB").setS((dpos!=string::npos) ? idb.substr(dpos+1) : "");
    modifG( );
}

void Lib::load_( TConfig *icfg )
{
    if(DB().empty() || (!SYS->chkSelDB(DB())))	throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(DB()+"."+mod->libTable(), mod->nodePath()+"lib/", *this);

    //Load functions
    map<string, bool>   itReg;
    TConfig cEl(&mod->elFnc());
    vector<vector<string> > full;
    //cEl.cfgViewAll(false);
    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl(),fldCnt++,cEl,false,&full); ) {
	string fId = cEl.cfg("ID").getS();
	if(!present(fId)) add(fId);
	at(fId).at().load(&cEl);
	itReg[fId] = true;
    }

    // Check for remove items removed from DB
    if(SYS->chkSelDB(SYS->selDB(),true)) {
	vector<string> itLs;
	list(itLs);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    if(itReg.find(itLs[iIt]) == itReg.end())
		del(itLs[iIt]);
    }
}

void Lib::save_( )
{
    if(DB().empty())	return;

    SYS->db().at().dataSet(DB()+"."+mod->libTable(),mod->nodePath()+"lib/",*this);
}

void Lib::setStart( bool val )
{
    vector<string> lst;
    list(lst);
    for(unsigned iF = 0; iF < lst.size(); iF++)
	try { if(!val || at(lst[iF]).at().toStart()) at(lst[iF]).at().setStart(val); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    runSt = val;
}

void Lib::add( const string &id, const string &name )	{ chldAdd(mFnc, new Func(id,name)); }

void Lib::del( const string &id )			{ chldDel(mFnc, id); }

TVariant Lib::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    if(present(iid))	return at(iid).at().objFuncCall("call", prms, user);

    return TCntrNode::objFuncCall(iid, prms, user);
}

void Lib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	XMLNode *nd = ctrMkNode("oscada_cntr",opt,-1,"/",_("Library of the functions: ")+id(),RWRWR_,"root",SDAQ_ID);
	if(nd)	nd->setAttr("doc", TUIS::docKeyGet(descr()));
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),RWRWR_,"root",SDAQ_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,-1,"/lib",_("Library"))) {
	    if(ctrMkNode("area",opt,-1,"/lib/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/lib/st/st",_("Accessing"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		if(DB().size()) {
		    ctrMkNode("fld",opt,-1,"/lib/st/db",_("Library DB"),RWRWR_,"root",SDAQ_ID,4,
			"tp","str","dest","sel_ed","select",("/db/tblList:flb_"+id()).c_str(),
			"help",_("DB address in the format \"{DB module}.{DB name}.{Table name}\".\nTo use the main working DB, set '*.*'."));
		    ctrMkNode("fld",opt,-1,"/lib/st/timestamp",_("Date of modification"),R_R_R_,"root",SDAQ_ID,1,"tp","time");
		}
	    }
	    if(ctrMkNode("area",opt,-1,"/lib/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/lib/cfg/ID",_("Identifier"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/lib/cfg/NAME",_("Name"),DB().empty()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"tp","str","len",OBJ_NM_SZ);
		ctrMkNode("fld",opt,-1,"/lib/cfg/DESCR",_("Description"),DB().empty()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","str","cols","100","rows","5");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/func",_("Functions")))
	    ctrMkNode("list",opt,-1,"/func/func",_("Functions"),RWRWR_,"root",SDAQ_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","fnc_","idSz",OBJ_ID_SZ);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/lib/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(startStat() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setStart(s2i(opt->text()));
    }
    else if(a_path == "/lib/st/db" && DB().size()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(fullDB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setFullDB(opt->text());
    }
    else if(a_path == "/lib/st/timestamp" && ctrChkNode(opt)) {
	vector<string> tls;
	list(tls);
	time_t maxTm = 0;
	for(size_t i_t = 0; i_t < tls.size(); i_t++) maxTm = vmax(maxTm, at(tls[i_t]).at().timeStamp());
	opt->setText(i2s(maxTm));
    }
    else if(a_path == "/lib/cfg/ID" && ctrChkNode(opt))		opt->setText(id());
    else if(a_path == "/lib/cfg/NAME") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(name());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setName(opt->text());
    }
    else if(a_path == "/lib/cfg/DESCR") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(descr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setDescr(opt->text());
    }
    else if(a_path == "/br/fnc_" || a_path == "/func/func") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned iF = 0; iF < lst.size(); iF++)
		opt->childAdd("el")->setAttr("id",lst[iF])->setText(at(lst[iF]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))	add(TSYS::strEncode(opt->attr("id"),TSYS::oscdID).c_str(),opt->text().c_str());
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	chldDel(mFnc,opt->attr("id"),-1,1);
    }
    else if(a_path == "/func/ls_lib" && ctrChkNode(opt)) {
	vector<string> lst;
	opt->childAdd("el")->setAttr("id","")->setText("");
	mod->lbList(lst);
	for(unsigned i_a=0; i_a < lst.size(); i_a++)
	    opt->childAdd("el")->setAttr("id",lst[i_a])->setText(mod->lbAt(lst[i_a]).at().name());
    }
    else TCntrNode::cntrCmdProc(opt);
}

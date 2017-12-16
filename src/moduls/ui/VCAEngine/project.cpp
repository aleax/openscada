
//OpenSCADA system module UI.VCAEngine file: project.cpp
/***************************************************************************
 *   Copyright (C) 2007-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <sys/stat.h>
#include <fcntl.h>

#include <tsys.h>

#include "vcaengine.h"
#include "project.h"

using namespace VCA;

//************************************************
//* Project					 *
//************************************************
Project::Project( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elProject()), enableByNeed(false), mId(cfg("ID")), workPrjDB(lib_db), mPermit(cfg("PERMIT").getId()),
    mPer(cfg("PER").getId())/*, mFlgs(cfg("FLGS").getId())*/, mStyleIdW(cfg("STYLE").getId()),
    mEnable(false), mFuncM(true)
{
    mId = id;
    cfg("NAME").setS(name);
    cfg("DB_TBL").setS(string("prj_")+id);
    mPage= grpAdd("pg_");
}

Project::~Project( )
{

}

TCntrNode &Project::operator=( const TCntrNode &node )
{
    const Project *src_n = dynamic_cast<const Project*>(&node);
    if(!src_n) return *this;

    //Copy generic configuration
    exclCopy(*src_n, "ID;");
    cfg("DB_TBL").setS("prj_"+id());
    workPrjDB = src_n->workPrjDB;

    if(!src_n->enable()) return *this;
    if(!enable()) setEnable(true);

    //Mime data copy
    vector<string> pls;
    src_n->mimeDataList(pls);
    string mimeType, mimeData;
    for(unsigned i_m = 0; i_m < pls.size(); i_m++) {
	src_n->mimeDataGet(pls[i_m], mimeType, &mimeData);
	mimeDataSet(pls[i_m], mimeType, mimeData);
    }

    //Copy included pages
    src_n->list(pls);
    for(unsigned i_p = 0; i_p < pls.size(); i_p++) {
	if(!present(pls[i_p])) add(pls[i_p],"");
	(TCntrNode&)at(pls[i_p]).at() = (TCntrNode&)src_n->at(pls[i_p]).at();
    }

    return *this;
}

void Project::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore) setEnable(true);
}

void Project::preDisable( int flag )
{
    if(enable()) setEnable(false);
}

void Project::postDisable( int flag )
{
    if(flag) {
	//Delete libraries record
	SYS->db().at().dataDel(DB()+"."+mod->prjTable(), mod->nodePath()+"PRJ/", *this, true);

	//Delete function's files
	// Delete widgets table
	SYS->db().at().open(fullDB());
	SYS->db().at().close(fullDB(),true);
	// Delete attributes table
	SYS->db().at().open(fullDB()+"_io");
	SYS->db().at().close(fullDB()+"_io",true);
	// Delete users attributes table
	SYS->db().at().open(fullDB()+"_uio");
	SYS->db().at().close(fullDB()+"_uio",true);
	// Delete include widgets table
	SYS->db().at().open(fullDB()+"_incl");
	SYS->db().at().close(fullDB()+"_incl",true);
	// Delete mime-data table
	SYS->db().at().open(fullDB()+"_mime");
	SYS->db().at().close(fullDB()+"_mime",true);
	// Delete session's table
	SYS->db().at().open(fullDB()+"_ses");
	SYS->db().at().close(fullDB()+"_ses",true);
	// Delete style's table
	SYS->db().at().open(fullDB()+"_stl");
	SYS->db().at().close(fullDB()+"_stl",true);
    }
}

string Project::name( ) const
{
    string rezs = cfg("NAME").getS();
    return rezs.size() ? rezs : mId;
}

string Project::owner( ) const	{ return SYS->security().at().usrPresent(cfg("USER").getS()) ? cfg("USER").getS() : string("root"); }

string Project::grp( ) const	{ return SYS->security().at().grpPresent(cfg("GRP").getS()) ? cfg("GRP").getS() : string("UI"); }

void Project::setOwner( const string &it )
{
    cfg("USER").setS(it);
    //Update librarie's group
    if(SYS->security().at().grpAt("UI").at().user(it)) setGrp("UI");
    else {
	vector<string> gls;
	SYS->security().at().usrGrpList(owner(),gls);
	setGrp(gls.size()?gls[0]:"UI");
    }
    modif();
}

void Project::setFullDB( const string &it )
{
    size_t dpos = it.rfind(".");
    workPrjDB = (dpos!=string::npos) ? it.substr(0,dpos) : "";
    cfg("DB_TBL").setS((dpos!=string::npos) ? it.substr(dpos+1) : "");
    modifG();
}

void Project::load_( TConfig *icfg )
{
    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

    if(!SYS->chkSelDB(DB())) throw TError();

    mess_debug(nodePath().c_str(), _("Load project."));

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(DB()+"."+mod->prjTable(), mod->nodePath()+"PRJ/", *this);

    vector<vector<string> > full;

    //Create new pages
    map<string, bool> itReg;
    TConfig cEl(&mod->elPage());
    //cEl.cfgViewAll(false);
    cEl.cfg("OWNER").setS("/"+id(), true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl()+"/",fldCnt++,cEl,false,&full); ) {
	string fId = cEl.cfg("ID").getS();
	if(!present(fId)) add(fId, "", "");
	at(fId).at().load(&cEl);
	itReg[fId] = true;
    }

    //Check for remove items removed from the DB
    if(!SYS->selDB().empty()) {
	vector<string> it_ls;
	list(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		del(it_ls[i_it]);
    }

    mOldDB = TBDS::realDBName(DB());

    //Load styles
    ResAlloc res(mStRes, true);
    TConfig cStl(&mod->elPrjStl());
    string svl;
    vector<string> vlst;
    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB()+"_stl",nodePath()+tbl()+"_stl",fldCnt++,cStl,false,&full); ) {
	vlst.clear();
	for(int iS = 0; iS < 10; iS++) {
	    svl = cStl.cfg(TSYS::strMess("V_%d",iS)).getS();
	    if(svl.empty()) break;
	    vlst.push_back(svl);
	}
	mStProp[cStl.cfg("ID").getS()] = vlst;
    }
}

void Project::save_( )
{
    SYS->db().at().dataSet(DB()+"."+mod->prjTable(),mod->nodePath()+"PRJ/",*this);

    //Check for need copy mime data and sessions data to other DB and same copy
    if(!mOldDB.empty() && mOldDB != TBDS::realDBName(DB())) {
	// Mime data copy
	vector<string> pls;
	mimeDataList(pls,mOldDB);
	string mimeType, mimeData;
	for(unsigned i_m = 0; i_m < pls.size(); i_m++) {
	    mimeDataGet(pls[i_m], mimeType, &mimeData, mOldDB);
	    mimeDataSet(pls[i_m], mimeType, mimeData, DB());
	}
	// Session's data copy
	vector<vector<string> > full;
	string wtbl = tbl()+"_ses";
	TConfig cEl(&mod->elPrjSes());
	for(int fldCnt = 0; SYS->db().at().dataSeek(mOldDB+"."+wtbl,"",fldCnt,cEl,false,&full); fldCnt++)
	    SYS->db().at().dataSet(DB()+"."+wtbl,"",cEl);
    }

    mOldDB = TBDS::realDBName(DB());

    //Save styles
    ResAlloc res(mStRes, false);
    TConfig cStl(&mod->elPrjStl());
    for(map<string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++) {
	cStl.cfg("ID").setS(iStPrp->first);
	for(unsigned iS = 0; iS < iStPrp->second.size() && iS < 10; iS++)
	    cStl.cfg(TSYS::strMess("V_%d",iS)).setS(iStPrp->second[iS]);
	SYS->db().at().dataSet(fullDB()+"_stl",nodePath()+tbl()+"_stl",cStl);
    }

    // Check for removed properties
    vector<vector<string> > full;
    res.request(true);
    cStl.cfgViewAll(false);
    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB()+"_stl",nodePath()+tbl()+"_stl",fldCnt++,cStl,false,&full); )
	if(mStProp.find(cStl.cfg("ID").getS()) == mStProp.end()) {
	    if(!SYS->db().at().dataDel(fullDB()+"_stl",nodePath()+tbl()+"_stl",cStl,false,false,true))	break;
	    if(full.empty()) fldCnt--;
	}
}

void Project::setEnable( bool val )
{
    if(val == enable()) return;

    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

    mess_debug(nodePath().c_str(),val ? _("Enable project.") : _("Disable project."));

    vector<string> f_lst;
    list(f_lst);
    for(unsigned i_ls = 0; i_ls < f_lst.size(); i_ls++)
	try{ at(f_lst[i_ls]).at().setEnable(val); }
	catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    mEnable = val;
}

void Project::add( const string &id, const string &name, const string &orig )
{
    if(present(id)) return;
    chldAdd(mPage, new Page(id,orig));
    at(id).at().setName(name);
}

void Project::add( Page *iwdg )
{
    if(present(iwdg->id())) delete iwdg;
    else chldAdd(mPage, iwdg);
}

AutoHD<Page> Project::at( const string &id ) const	{ return chldAt(mPage,id); }

void Project::mimeDataList( vector<string> &list, const string &idb ) const
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfgViewAll(false);

    list.clear();
    vector<vector<string> > full;
    for(int fldCnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,mod->nodePath()+wtbl,fldCnt,cEl,false,&full); fldCnt++)
	list.push_back(cEl.cfg("ID").getS());
}

bool Project::mimeDataGet( const string &iid, string &mimeType, string *mimeData, const string &idb ) const
{
    bool is_file = (iid.compare(0,5,"file:")==0);
    bool is_res  = (iid.compare(0,4,"res:")==0);

    if(!is_file) {
	//Get resource file from DB
	string dbid = is_res ? iid.substr(4) : iid;
	string wtbl = tbl()+"_mime";
	string wdb  = idb.empty() ? DB() : idb;
	TConfig cEl(&mod->elWdgData());
	if(!mimeData) cEl.cfg("DATA").setView(false);
	cEl.cfg("ID").setS( dbid );
	if(SYS->db().at().dataGet(wdb+"."+wtbl,mod->nodePath()+wtbl,cEl,false,true)) {
	    mimeType = cEl.cfg("MIME").getS();
	    if(mimeData) *mimeData = cEl.cfg("DATA").getS();
	    return true;
	}
    }
    if(!is_res) {
	//Get resource file from file system
	string filepath = is_file ? iid.substr(5) : iid;
	int len;
	char buf[STR_BUF_LEN];
	string rez;
	int hd = open(filepath.c_str(),O_RDONLY);
	if(hd == -1)  return false;
	if(lseek(hd,0,SEEK_END) > 100*1024*1024) { close(hd); return false; }
	lseek(hd,0,SEEK_SET);

	while((len=read(hd,buf,sizeof(buf))) > 0) rez.append(buf,len);
	close(hd);

	mimeType = TUIS::mimeGet(filepath, rez);
	if(mimeData) *mimeData = TSYS::strEncode(rez,TSYS::base64);
	return true;
    }

    return false;
}

void Project::mimeDataSet( const string &iid, const string &mimeType, const string &mimeData, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid);
    cEl.cfg("MIME").setS(mimeType);
    if(!mimeData.size()) cEl.cfg("DATA").setView(false);
    else cEl.cfg("DATA").setS(mimeData);
    SYS->db().at().dataSet(wdb+"."+wtbl,mod->nodePath()+wtbl,cEl,false,true);
}

void Project::mimeDataDel( const string &iid, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid, true);
    SYS->db().at().dataDel(wdb+"."+wtbl, mod->nodePath()+wtbl, cEl, false, false, true);
}

void Project::stlList( vector<string> &ls )
{
    ls.clear();

    ResAlloc res(mStRes, false);
    map< string, vector<string> >::iterator iStPrp = mStProp.find("<Styles>");
    if(iStPrp == mStProp.end()) return;
    for(unsigned ist = 0; ist < iStPrp->second.size(); ist++)
	ls.push_back(TSYS::strSepParse(iStPrp->second[ist],0,';'));
}

int Project::stlSize( )
{
    ResAlloc res(mStRes, false);
    map< string, vector<string> >::iterator iStPrp = mStProp.find("<Styles>");
    if(iStPrp != mStProp.end()) return iStPrp->second.size();

    return 0;
}

void Project::stlCurentSet( int sid )
{
    mStyleIdW = (sid < 0 || sid >= stlSize()) ? -1 : sid;
    modif();
}

string Project::stlGet( int sid )
{
    ResAlloc res(mStRes, false);
    map< string, vector<string> >::iterator iStPrp = mStProp.find("<Styles>");
    if(iStPrp == mStProp.end() || sid < 0 || sid >= (int)iStPrp->second.size()) return "";

    return iStPrp->second[sid];
}

void Project::stlSet( int sid, const string &stl )
{
    ResAlloc res(mStRes, true);
    map< string, vector<string> >::iterator iStPrp = mStProp.find("<Styles>");
    if(iStPrp == mStProp.end() || sid < 0 || sid >= (int)iStPrp->second.size()) return;
    iStPrp->second[sid] = stl;
    modif();
}

void Project::stlPropList( vector<string> &ls )
{
    ls.clear();
    ResAlloc res(mStRes, false);
    for(map<string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
	if(iStPrp->first != "<Styles>")
	    ls.push_back(iStPrp->first);
}

string Project::stlPropGet( const string &pid, const string &def, int sid )
{
    ResAlloc res(mStRes, false);
    if(sid < 0) sid = stlCurent();
    if(pid.empty() || sid < 0 || sid >= stlSize() || pid == "<Styles>") return def;

    map<string, vector<string> >::iterator iStPrp = mStProp.find(pid);
    if(iStPrp != mStProp.end()) return iStPrp->second[sid];
    vector<string> vl;
    for(int i_v = 0; i_v < stlSize(); i_v++) vl.push_back(def);
    res.request(true);
    mStProp[pid] = vl;
    modif();

    return def;
}

bool Project::stlPropSet( const string &pid, const string &vl, int sid )
{
    ResAlloc res(mStRes, true);
    if(sid < 0) sid = stlCurent();
    if(pid.empty() || sid < 0 || sid >= stlSize() || pid == "<Styles>") return false;
    map<string, vector<string> >::iterator iStPrp = mStProp.find(pid);
    if(iStPrp == mStProp.end()) return false;
    iStPrp->second[sid] = vl;
    modif();

    return true;
}

string Project::catsPat( )	{ return /* "("+mod->nodePath()+*/"/ses_"+id()+"\\d*"; }

void Project::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Project: ")+id(),RWRWR_,"root",SUI_ID);
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ctrMkNode("area",opt,-1,"/obj",_("Project"))) {
	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enabled"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/db",_("Project DB"),RWRWR_,"root",SUI_ID,4,
		    "tp","str","dest","sel_ed","select",("/db/tblList:prj_"+id()).c_str(),
		    "help",_("DB address in format [<DB module>.<DB name>.<Table name>].\nFor use main work DB set '*.*'."));
		ctrMkNode("fld",opt,-1,"/obj/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/obj/cfg/id",_("Id"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/descr",_("Description"),RWRWR_,"root",SUI_ID,3,"tp","str","cols","100","rows","3");
		ctrMkNode("img",opt,-1,"/obj/cfg/ico",_("Icon"),RWRWR_,"root",SUI_ID,2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/obj/cfg/owner",_("Owner and group"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/obj/u_lst");
		ctrMkNode("fld",opt,-1,"/obj/cfg/grp","",RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/obj/g_lst");
		ctrMkNode("fld",opt,-1,"/obj/cfg/u_a",_("Access"),RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/cfg/g_a","",RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/cfg/o_a","",RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/cfg/per",_("Calculate period"),RWRWR_,"root",SUI_ID,2,"tp","dec",
		    "help",_("Project's session calculate period on milliseconds."));
		ctrMkNode("fld",opt,-1,"/obj/cfg/toEnByNeed",_("Enable by need"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/page",_("Pages"))) {
	    ctrMkNode("fld",opt,-1,"/page/nmb",_("Number"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/page/page",_("Pages"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","pg_","idSz","30");
	}
	if(ctrMkNode("area",opt,-1,"/mime",_("Mime data")))
	    if(ctrMkNode("table",opt,-1,"/mime/mime",_("Mime data"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/mime/mime/id",_("Id"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/tp",_("Mime type"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/dt",_("Data"),RWRWR_,"root",SUI_ID,2,"tp","str","dest","data");
	    }
	if(ctrMkNode("area",opt,-1,"/style",_("Styles"))) {
	    ctrMkNode("fld",opt,-1,"/style/style",_("Style"),RWRWR_,"root",SUI_ID,3,"tp","dec","dest","select","select","/style/stLst");
	    if(stlCurent() >= 0 && stlCurent() < stlSize()) {
		ctrMkNode("fld",opt,-1,"/style/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		if(ctrMkNode("table",opt,-1,"/style/props",_("Properties"),RWRWR_,"root",SUI_ID,2,"s_com","del","key","id")) {
		    ctrMkNode("list",opt,-1,"/style/props/id",_("Id"),R_R_R_,"root",SUI_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/style/props/vl",_("Value"),RWRWR_,"root",SUI_ID,1,"tp","str");
		}
		ctrMkNode("comm",opt,-1,"/style/erase",_("Erase"),RWRWR_,"root",SUI_ID);
	    }
	}
	if(ctrMkNode("area",opt,-1,"/mess",_("Diagnostics"))) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time"),RWRW__,"root",SUI_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size",_("Size, seconds"),RWRW__,"root",SUI_ID,1,"tp","dec");
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SUI_ID)) {
		ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),R_R___,"root",SUI_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/mess/mess/0a",_("mcsec"),R_R___,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),R_R___,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mess/mess/2",_("Lev."),R_R___,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/3",_("Message"),R_R___,"root",SUI_ID,1,"tp","str");
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user"), l = opt->attr("lang");
    if(a_path == "/obj/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/obj/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(fullDB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setFullDB(opt->text());
    }
    else if(a_path == "/obj/st/timestamp" && ctrChkNode(opt)) {
	vector<string> tls;
	list(tls);
	time_t maxTm = 0;
	for(unsigned i_t = 0; i_t < tls.size(); i_t++) maxTm = vmax(maxTm, at(tls[i_t]).at().timeStamp());
	opt->setText(i2s(maxTm));
    }
    else if(a_path == "/obj/cfg/owner") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(owner());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setOwner(opt->text());
    }
    else if(a_path == "/obj/cfg/grp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(grp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setGrp(opt->text());
    }
    else if(a_path == "/obj/cfg/u_a" || a_path == "/obj/cfg/g_a" || a_path == "/obj/cfg/o_a") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(a_path == "/obj/cfg/u_a")	opt->setText(i2s((permit()>>6)&0x7));
	    if(a_path == "/obj/cfg/g_a")	opt->setText(i2s((permit()>>3)&0x7));
	    if(a_path == "/obj/cfg/o_a")	opt->setText(i2s(permit()&0x7));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(a_path == "/obj/cfg/u_a")	setPermit((permit()&(~(0x07<<6)))|(s2i(opt->text())<<6));
	    if(a_path == "/obj/cfg/g_a")	setPermit((permit()&(~(0x07<<3)))|(s2i(opt->text())<<3));
	    if(a_path == "/obj/cfg/o_a")	setPermit((permit()&(~0x07))|s2i(opt->text()));
	}
    }
    else if(a_path == "/obj/cfg/ico" || a_path == "/ico") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(ico());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setIco(opt->text());
    }
    else if(a_path == "/obj/cfg/id" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID))	opt->setText(id());
    else if(a_path == "/obj/cfg/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trLU(name(),l,u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(trSetLU(name(),l,u,opt->text()));
    }
    else if(a_path == "/obj/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trLU(descr(),l,u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(trSetLU(descr(),l,u,opt->text()));
    }
    else if(a_path == "/obj/cfg/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(period()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setPeriod(s2i(opt->text()));
    }
    else if(a_path == "/obj/cfg/toEnByNeed") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(toEnByNeed()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setToEnByNeed(s2i(opt->text()));
    }
    else if(a_path == "/br/pg_" || a_path == "/page/page") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(trLU(at(lst[i_f]).at().name(),l,u));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    if(present(vid)) throw TError(nodePath().c_str(), _("Page '%s' already present!"), vid.c_str());
	    add(vid, opt->text());
	    at(vid).at().setOwner(opt->attr("user"));
	    at(vid).at().manCrt = true;
	    opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	del(opt->attr("id"),true);
    }
    else if(a_path == "/page/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	list(c_list);
	unsigned e_c = 0;
	for(unsigned i_p = 0; i_p < c_list.size(); i_p++)
	    if(at(c_list[i_p]).at().enable()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else if(a_path == "/obj/u_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/obj/g_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrGrpList(owner(), ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/mime/mime") {
	// Request data
	string idmime = opt->attr("key_id");
	string idcol  = opt->attr("col");

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(!idmime.empty() && idcol == "dt" && s2i(opt->attr("data"))) {
		string mimeType, mimeData;
		if(mimeDataGet("res:"+idmime, mimeType, &mimeData)) opt->setText(mimeData);
	    }
	    else {
		XMLNode *n_id = ctrMkNode("list",opt,-1,"/mime/mime/id","");
		XMLNode *n_tp = ctrMkNode("list",opt,-1,"/mime/mime/tp","");
		XMLNode *n_dt = ctrMkNode("list",opt,-1,"/mime/mime/dt","");

		vector<string> lst;
		string mimeType;
		mimeDataList(lst);
		for(unsigned i_el = 0; i_el < lst.size(); i_el++)
		    if(mimeDataGet("res:"+lst[i_el],mimeType)) {
			if(n_id) n_id->childAdd("el")->setText(lst[i_el]);
			if(n_tp) n_tp->childAdd("el")->setText(TSYS::strSepParse(mimeType,0,';'));
			if(n_dt) n_dt->childAdd("el")->setText(TSYS::strSepParse(mimeType,1,';'));
		    }
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))	mimeDataSet("newMime", "file/unknown;0", "");
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	mimeDataDel(opt->attr("key_id"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    // Request data
	    if(idcol == "id") {
		string mimeType, mimeData;
		// Copy mime data to new record
		if(mimeDataGet("res:"+idmime, mimeType, &mimeData)) {
		    mimeDataSet(opt->text(), TUIS::mimeGet(idmime,mimeData,mimeType), mimeData);
		    mimeDataDel(idmime);
		}
	    }
	    else if(idcol == "tp") {
		string mimeType;
		// Copy mime data to new record
		if(mimeDataGet("res:"+idmime, mimeType))
		    mimeDataSet(idmime, opt->text()+";"+TSYS::strSepParse(mimeType,1,';'), "");
	    }
	    else if(idcol == "dt") {
		string mimeType;
		if(!mimeDataGet("res:"+idmime, mimeType)) mimeType = TUIS::mimeGet(idmime, TSYS::strDecode(opt->text(),TSYS::base64));
		mimeDataSet(idmime, TSYS::strSepParse(mimeType,0,';')+";"+r2s((float)opt->text().size()/1024,6),opt->text());
	    }
	}
    }
    else if(a_path == "/style/style") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(stlCurent()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(s2i(opt->text()) >= -1) stlCurentSet(s2i(opt->text()));
	    else {
		ResAlloc res(mStRes, true);
		map< string, vector<string> >::iterator iStPrp;
		for(iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
		    if(iStPrp->first == "<Styles>") continue;
		    else if(iStPrp->second.size()) iStPrp->second.push_back(iStPrp->second[iStPrp->second.size()-1]);
		    else mStProp.erase(iStPrp--);
		iStPrp = mStProp.find("<Styles>");
		if(iStPrp == mStProp.end()) mStProp["<Styles>"] = vector<string>(1,_("New style"));
		else iStPrp->second.push_back(_("New style"));
		mStyleIdW = mStProp["<Styles>"].size()-1;
		modif();
	    }
	}
    }
    else if(a_path == "/style/stLst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","-1")->setText(_("No style"));
	for(int iSt = 0; iSt < stlSize(); iSt++)
	    opt->childAdd("el")->setAttr("id", i2s(iSt))->setText(TSYS::strSepParse(stlGet(iSt),0,';'));
	if(stlSize() < 10) opt->childAdd("el")->setAttr("id","-2")->setText(_("Create new style"));
    }
    else if(a_path == "/style/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::strSepParse(stlGet(stlCurent()),0,';'));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string sprv = stlGet(stlCurent());
	    stlSet(stlCurent(),opt->text());
	}
    }
    else if(a_path == "/style/props") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD) && stlCurent() >=0 && stlCurent() < stlSize())
	{
	    XMLNode *n_id = ctrMkNode("list",opt,-1,"/style/props/id","");
	    XMLNode *n_vl = ctrMkNode("list",opt,-1,"/style/props/vl","");

	    ResAlloc res(mStRes, false);
	    for(map< string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
	    {
		if(iStPrp->first == "<Styles>") continue;
		if(n_id)	n_id->childAdd("el")->setText(iStPrp->first);
		if(n_vl)	n_vl->childAdd("el")->setText(iStPrp->second[stlCurent()]);
	    }
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    ResAlloc res(mStRes, true);
	    if(mStProp.find(opt->attr("key_id")) != mStProp.end()) { mStProp.erase(opt->attr("key_id")); modif(); }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR) && opt->attr("col") == "vl" && stlCurent() >=0 && stlCurent() < stlSize())
	{
	    ResAlloc res( mStRes, true );
	    map< string, vector<string> >::iterator iStPrp = mStProp.find(opt->attr("key_id"));
	    if(iStPrp != mStProp.end()) { iStPrp->second[stlCurent()] = opt->text(); modif(); }
	}
    }
    else if(a_path == "/style/erase" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR) && stlCurent() >=0 && stlCurent() < stlSize())
    {
	ResAlloc res( mStRes, true );
	map< string, vector<string> >::iterator iStPrp;
	for(iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
	    iStPrp->second.erase(iStPrp->second.begin()+stlCurent());
	stlCurentSet(-1);
    }
    else if(a_path == "/mess/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) {
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text())) opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SUI_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"messTm",(s2i(opt->text())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"messSize","600",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/mess" && ctrChkNode(opt,"get",R_R___,"root",SUI_ID)) {
	vector<TMess::SRec> rec;
	time_t gtm = s2i(TBDS::genDBGet(mod->nodePath()+"messTm","0",opt->attr("user")));
	if(!gtm) gtm = time(NULL);
	int gsz = s2i(TBDS::genDBGet(mod->nodePath()+"messSize","600",opt->attr("user")));
	SYS->archive().at().messGet(gtm-gsz, gtm, rec, "/("+catsPat()+")/", TMess::Info, "");

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SUI_ID);
	XMLNode *n_tmu  = ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SUI_ID);
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SUI_ID);
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SUI_ID);
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SUI_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--) {
	    if(n_tm)	n_tm->childAdd("el")->setText(i2s(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(i2s(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(i2s(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }

    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* Page: Project's page                         *
//************************************************
Page::Page( const string &iid, const string &isrcwdg ) : Widget(iid), TConfig(&mod->elPage()), manCrt(false),
    mFlgs(cfg("FLGS").getId()), mProcPer(cfg("PROC_PER").getId()), mTimeStamp(cfg("TIMESTAMP").getId())
{
    cfg("ID").setS(id());
    cfg("PROC").setExtVal(true);

    mPage = grpAdd("pg_");

    setParentNm(isrcwdg);
    setNodeFlg(TCntrNode::SelfSaveForceOnChild);
}

Page::~Page( )
{

}

TCntrNode &Page::operator=( const TCntrNode &node )
{
    const Page *src_n = dynamic_cast<const Page*>(&node);
    if(!src_n) return Widget::operator=(node);

    if(!src_n->enable()) return *this;

    //Copy generic configuration
    setPrjFlags(src_n->prjFlags());

    //Widget copy
    Widget::operator=(node);

    //Include widgets copy
    vector<string> els;
    src_n->pageList(els);
    // Call recursive only for separated branches copy and for prevent to included copy
    if(path().find(src_n->path()+"/") != 0)
	for(unsigned i_p = 0; i_p < els.size(); i_p++) {
	    if(!pagePresent(els[i_p])) pageAdd(els[i_p], "");
	    (TCntrNode&)pageAt(els[i_p]).at() = (TCntrNode&)src_n->pageAt(els[i_p]).at();
	}

    return *this;
}

Page *Page::ownerPage( ) const	{ return nodePrev(true) ? dynamic_cast<Page*>(nodePrev()) : NULL; }

Project *Page::ownerProj( ) const
{
    Page *own = ownerPage();
    if(own) return own->ownerProj();
    if(nodePrev(true)) return dynamic_cast<Project*>(nodePrev());
    return NULL;
}

string Page::path( ) const	{ return ownerFullId(true)+"/pg_"+id(); }

string Page::ownerFullId( bool contr ) const
{
    Page *own = ownerPage( );
    if(own) return own->ownerFullId(contr)+(contr?"/pg_":"/")+own->id();
    return string(contr?"/prj_":"/")+ownerProj()->id();
}

int Page::timeStamp( )
{
    int curTm = mTimeStamp;
    vector<string> ls;
    pageList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	curTm = vmax(curTm, pageAt(ls[i_l]).at().timeStamp());

    return curTm;
}

void Page::postEnable( int flag )
{
    //Call parent method
    Widget::postEnable(flag);

    //Add main attributes
    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("pgOpen",_("Page: open state"),TFld::Boolean,TFld::NoFlag));
	attrAdd(new TFld("pgNoOpenProc",_("Page: process not opened"),TFld::Boolean,TFld::NoFlag));
	attrAdd(new TFld("pgGrp",_("Page: group"),TFld::String,TFld::NoFlag,"","","","",i2s(A_PG_GRP).c_str()));
	attrAdd(new TFld("pgOpenSrc",_("Page: open source"),TFld::String,TFld::NoFlag,"","","","",i2s(A_PG_OPEN_SRC).c_str()));
    }

    //Set owner key for this page
    cfg("OWNER").setS(ownerFullId());

    //Set default parent for parent template page
    if(ownerPage() && (ownerPage()->prjFlags()&Page::Template)) setParentNm("..");
}

void Page::postDisable( int flag )
{
    if(flag) {
	string db  = ownerProj()->DB();
	string tbl = ownerProj()->tbl();

	//Remove from library table
	SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, *this, true);

	//Remove widget's IO from library IO table
	TConfig cEl(&mod->elWdgIO());
	cEl.cfg("IDW").setS(path(),true);
	SYS->db().at().dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", cEl);

	//Remove widget's user IO from library IO table
	cEl.setElem(&mod->elWdgUIO());
	cEl.cfg("IDW").setS(path(),true);
	SYS->db().at().dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", cEl);

	//Remove widget's included widgets from library include table
	cEl.setElem(&mod->elInclWdg());
	cEl.cfg("IDW").setS(path(),true);
	SYS->db().at().dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", cEl);
    }
}

bool Page::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PR_TR") cfg("PROC").setNoTransl(!calcProgTr());
    modif();
    return true;
}

string Page::ico( ) const
{
    if(cfg("ICO").getS().size())return cfg("ICO").getS();
    if(!parent().freeStat())	return parent().at().ico();
    return "";
}

void Page::setParentNm( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
    if(ownerPage() && (ownerPage()->prjFlags()&Page::Template) && !(ownerPage()->prjFlags()&Page::Container))
	cfg("PARENT").setS("..");
    modif();
}

string Page::calcId( )
{
    if(proc().empty()) {
	if(!parent().freeStat()) return parent().at().calcId();
	return "";
    }
    if(ownerPage()) return ownerPage()->calcId()+"_"+id();
    return "P_"+ownerProj()->id()+"_"+id();
}

string Page::calcLang( ) const
{
    if(proc().empty() && !parent().freeStat()) return parent().at().calcLang();

    string iprg = proc();
    if(iprg.find("\n") == string::npos) {
	iprg = iprg+"\n";
	cfg("PROC").setS(iprg);
    }
    return iprg.substr(0,iprg.find("\n"));
}

bool Page::calcProgTr( )	{ return (!proc().size() && !parent().freeStat()) ? parent().at().calcProgTr() : cfg("PR_TR"); }

string Page::calcProg( ) const
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcProg();

    string iprg = proc();
    size_t lng_end = iprg.find("\n");
    if(lng_end == string::npos) lng_end = 0;
    else lng_end++;
    return iprg.substr(lng_end);
}

string Page::calcProgStors( const string &attr )
{
    string rez = parent().freeStat() ? "" : parent().at().calcProgStors(attr);
    if(((attr.size() && attrAt(attr).at().modif()) || (!attr.size() && proc().size())) && rez.find(ownerProj()->DB()) == string::npos)
	rez = ownerProj()->DB() + ";" + rez;
    return rez;
}

int Page::calcPer( ) const	{ return (mProcPer < 0 && !parent().freeStat()) ? parent().at().calcPer() : mProcPer; }

void Page::setCalcLang( const string &ilng )	{ cfg("PROC").setS(ilng.empty() ? "" : ilng+"\n"+calcProg()); }

void Page::setCalcProgTr( bool vl )
{
    if(!proc().size() && !parent().freeStat())	parent().at().setCalcProgTr(vl);
    else cfg("PR_TR") = vl;
}

void Page::setCalcProg( const string &iprg )	{ cfg("PROC").setS(calcLang()+"\n"+iprg); }

void Page::setPrjFlags( int val )
{
    int dif = mFlgs^val;
    if(dif&Page::Empty) {
	//Clear page
	setParentNm("");
	if(enable()) {
	    setEnable(false);
	    setEnable(true);
	}
    }
    mFlgs = val;
    modif();
}

void Page::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(ownerProj()->DB())) throw TError();

    setStlLock(true);

    //Load generic widget's data
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl();
    string tbl_io = tbl+"_io";
    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,*this);
    setParentNm(cfg("PARENT").getS());

    //Inherit modify attributes
    vector<string> als;
    attrList(als);
    string tAttrs = cfg("ATTRS").getS();
    for(unsigned i_a = 0; i_a < als.size(); i_a++) {
	if(!attrPresent(als[i_a])) continue;
	AutoHD<Attr> attr = attrAt(als[i_a]);
	if(attr.at().modif() && tAttrs.find(als[i_a]+";") == string::npos) {
	    attr.at().setModif(0);
	    inheritAttr(als[i_a]);
	}
    }

    //Load generic attributes
    mod->attrsLoad(*this, db+"."+tbl, path(), "", tAttrs, true);

    //Create new pages
    map<string, bool>	itReg;
    vector<vector<string> > full;
    TConfig cEl(&mod->elPage());
    //cEl.cfgViewAll(false);
    cEl.cfg("OWNER").setS(ownerFullId()+"/"+id(), true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fldCnt++,cEl,false,&full); ) {
	string fId = cEl.cfg("ID").getS();
	if(!pagePresent(fId))
	    try { pageAdd(fId, "", ""); }
	    catch(TError &err) { mess_err(err.cat.c_str(),err.mess.c_str()); }
	pageAt(fId).at().load(&cEl);
	itReg[fId] = true;
    }

    //Check for remove items removed from DB
    if(!SYS->selDB().empty()) {
	vector<string> it_ls;
	pageList(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		pageDel(it_ls[i_it]);
    }

    //Load present pages
    vector<string> f_lst;
    pageList(f_lst);
    for(unsigned i_ls = 0; i_ls < f_lst.size(); i_ls++)
	pageAt(f_lst[i_ls]).at().load();

    //Load all widget attributes
    loadIO();

    setStlLock(false);
}

void Page::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerProj()->DB()+"."+ownerProj()->tbl(), path(), "", cfg("ATTRS").getS());

    //Load cotainer widgets
    if(!isContainer()) return;
    map<string, bool>   itReg;
    vector<vector<string> > full;
    TConfig cEl(&mod->elInclWdg());
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl()+"_incl";
    cEl.cfg("IDW").setS(path(), true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fldCnt++,cEl,false,&full); ) {
	string sid  = cEl.cfg("ID").getS();
	if(cEl.cfg("PARENT").getS() == "<deleted>") {
	    if(wdgPresent(sid))	wdgDel(sid);
	    continue;
	}
	if(!wdgPresent(sid))
	    try{ wdgAdd(sid, "", ""); }
	    catch(TError &err){ mess_err(err.cat.c_str(),err.mess.c_str()); }

	wdgAt(sid).at().load(&cEl);
	itReg[sid] = true;
    }

    // Check for remove items removed from the DB
    if(!SYS->selDB().empty()) {
	vector<string> it_ls;
	wdgList(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		wdgDel(it_ls[i_it]);
    }
}

void Page::save_( )
{
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl();

    //Save generic attributes
    cfg("ATTRS").setS(mod->attrsSave(*this, db+"."+tbl, path(), "", true));

    //Save generic widget's data
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,*this);

    //Save widget's attributes
    saveIO();
}

void Page::saveIO( )
{
    if(!enable()) return;

    //Save widget's attributes
    mod->attrsSave(*this, ownerProj()->DB()+"."+ownerProj()->tbl(), path(), "");
}

void Page::wClear( )
{
    Widget::wClear();
    cfg("PROC").setS("");
    cfg("ATTRS").setS("");
}

void Page::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    if(prjFlags()&Page::Empty) cfg("PARENT").setS("root");

    Widget::setEnable(val);
    if(val && !parent().freeStat() && parent().at().rootId() != "Box") {
	Widget::setEnable(false);
	throw TError(nodePath().c_str(),_("For page can use only Box-based widgets!"));
    }

    if(val) {
	attrAdd(new TFld("pgOpen",_("Page: open state"),TFld::Boolean,TFld::NoFlag));
	attrAdd(new TFld("pgNoOpenProc",_("Page: process not opened"),TFld::Boolean,TFld::NoFlag));
    }

    //Enable/disable included pages
    vector<string> ls;
    pageList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++)
        try { pageAt(ls[iL]).at().setEnable(val); }
	catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //Include widgets link update on the parrent change
    if(val) {
	bool lnkUpdt = (mParentNmPrev.size() && parentNm() != mParentNmPrev);
	vector<string> lst;
	wdgList(lst, true);
	for(unsigned iL = 0; iL < lst.size(); iL++)
	    try {
		AutoHD<Widget> iw = wdgAt(lst[iL]);
		if(lnkUpdt && iw.at().parentNm().compare(0,mParentNmPrev.size()+1,mParentNmPrev+"/") == 0) {
		    iw.at().setParentNm(parentNm()+iw.at().parentNm().substr(mParentNmPrev.size()));
		    iw.at().setEnable(true);
		}
		else if(manCrt) iw.at().modifClr();
	    } catch(TError &err) { }
	mParentNmPrev = parentNm();
    }
}

void Page::wdgAdd( const string &wid, const string &name, const string &ipath, bool force )
{
    if(!isContainer())  throw TError(nodePath().c_str(),_("Widget is not container!"));
    if(wdgPresent(wid)) return;

    bool toRestoreInher = false;

    //Check for label <deleted>
    if(!force) {
	string db = ownerProj()->DB();
	string tbl = ownerProj()->tbl() + "_incl";
	TConfig cEl(&mod->elInclWdg());
	cEl.cfg("IDW").setS(path());
	cEl.cfg("ID").setS(wid);
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,cEl,false,true) && cEl.cfg("PARENT").getS() == "<deleted>") {
	    SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, true, false, true);
	    toRestoreInher = parent().at().wdgPresent(wid);
	    /*if(!parent().at().wdgPresent(wid))
		SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, true, false, true);
	    else throw TError(nodePath().c_str(),
		_("You try to create widget with name '%s' of the widget that was early deleted as an inherited from the base container!"), wid.c_str());*/
	}
    }

    //Same widget addition or restoring
    if(toRestoreInher) {
	inheritIncl(wid);
	wdgAt(wid).at().setEnable(true);
    }
    else {
	chldAdd(inclWdg, new PageWdg(wid,ipath));
	wdgAt(wid).at().setName(name);
    }

    //Call heritors include widgets update
    for(unsigned i_h = 0; i_h < mHerit.size(); i_h++)
	if(mHerit[i_h].at().enable())
	    mHerit[i_h].at().inheritIncl(wid);

    if(toRestoreInher)	throw TError("warning", _("Restoring '%s' from the base container!"), wid.c_str());
}

AutoHD<Widget> Page::wdgAt( const string &wdg, int lev, int off ) const
{
    //Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerProj()->nodeAt(wdg,1); }
	catch(TError &err) { return AutoHD<Widget>(); }

    int offt = off;
    string iw = TSYS::pathLev(wdg,lev,true,&offt);
    if(iw.compare(0,3,"pg_") == 0) {
	if(pagePresent(iw.substr(3))) return pageAt(iw.substr(3)).at().wdgAt(wdg, 0, offt);
	else return AutoHD<Widget>();
    }

    return Widget::wdgAt(wdg, lev, off);
}

void Page::pageAdd( const string &id, const string &name, const string &orig )
{
    if(pagePresent(id)) return;
    if(!(prjFlags()&(Page::Container|Page::Template)))
	throw TError(nodePath().c_str(),_("Page is not container or template!"));
    chldAdd(mPage, new Page(id,orig));
    pageAt(id).at().setName(name);
}

void Page::pageAdd( Page *iwdg )
{
    if(pagePresent(iwdg->id()))	delete iwdg;
    if(!(prjFlags()&(Page::Container|Page::Template))) {
	delete iwdg;
	throw TError(nodePath().c_str(),_("Page is not container or template!"));
    }
    else chldAdd(mPage,iwdg);
}

AutoHD<Page> Page::pageAt( const string &id ) const	{ return chldAt(mPage,id); }

void Page::resourceList( vector<string> &ls )
{
    //Append to the map for doublets remove
    map<string,bool> sortLs;
    for(unsigned iL = 0; iL < ls.size(); iL++) sortLs[ls[iL]] = true;
    ownerProj()->mimeDataList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++) sortLs[ls[iL]] = true;
    ls.clear();
    for(map<string,bool>::iterator iL = sortLs.begin(); iL != sortLs.end(); ++iL) ls.push_back(iL->first);

    if(!parent().freeStat()) parent().at().resourceList(ls);
}

string Page::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;

    if(!ownerProj()->mimeDataGet(id,mimeType,&mimeData) && !parent().freeStat())
	mimeData = parent().at().resourceGet(id, &mimeType);
    if(mime) *mime = mimeType;

    return mimeData;
}

void Page::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf && !(nodeFlg()&SelfModifyS)) modifClr();
}

TVariant Page::vlGet( Attr &a )
{
    if(a.owner() == this) {
	if(a.id() == "owner") {
	    short perm = attrAt("perm").at().getI(true);
	    if(!(perm&01000)) return a.getS(true);
	    Page *oP = ownerPage();
	    return oP ? oP->attrAt("owner").at().getS() : ownerProj()->owner()+":"+ownerProj()->grp();
	}
	else if(a.id() == "perm") {
	    short perm = a.getI(true);
	    if(!(perm&01000)) return perm;
	    Page *oP = ownerPage();
	    return (oP?oP->attrAt("perm").at().getI():ownerProj()->permit())|01000;
	}
    }
    return Widget::vlGet(a);
}

TVariant Page::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    if(stlLock()) return vl;
    string pid = sTrm(a.cfgTempl());
    if(pid.empty()) pid = a.id();
    if(!wr) return ownerProj()->stlPropGet(pid, vl.getS());
    if(ownerProj()->stlPropSet(pid,vl.getS())) return TVariant();
    return vl;
}

bool Page::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Project page: ")+path(),RWRWR_,"root",SUI_ID);
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")) && ctrMkNode("area",opt,-1,"/wdg/cfg",_("Configuration"))) {
	    if((prjFlags()&Page::Empty) || (ownerPage() && (ownerPage()->prjFlags()&Page::Template) && !(ownerPage()->prjFlags()&Page::Container)))
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("fld",opt,10,"/wdg/st/pgTp",_("Page type"),RWRWR_,"root",SUI_ID,4,"tp","str","idm","1","dest","select","select","/wdg/st/pgTpLst");
	    ctrMkNode("fld",opt,-1,"/wdg/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	}
	if(prjFlags()&(Page::Template|Page::Container)) {
	    if(ctrMkNode("area",opt,1,"/page",_("Pages"))) {
		ctrMkNode("fld",opt,-1,"/page/nmb",_("Number"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/page/page",_("Pages"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","pg_","idSz","30");
	    }
	    if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
		ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user"), l = opt->attr("lang");
    if(a_path == "/wdg/w_lst" && ctrChkNode(opt) && ownerPage() && (ownerPage()->prjFlags()&Page::Template))
	opt->childIns(0,"el")->setText("..");
    else if(a_path == "/wdg/st/pgTp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(prjFlags()&(Page::Container|Page::Template|Page::Empty)));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    setPrjFlags(prjFlags()^((prjFlags()^s2i(opt->text()))&(Page::Container|Page::Template|Page::Empty)));
    }
    else if(a_path == "/wdg/st/pgTpLst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","0")->setText(_("Standard"));
	opt->childAdd("el")->setAttr("id", i2s(Page::Container))->setText(_("Container"));
	opt->childAdd("el")->setAttr("id", i2s(Page::Container|Page::Empty))->setText(_("Logical container"));
	opt->childAdd("el")->setAttr("id", i2s(Page::Template))->setText(_("Template"));
	opt->childAdd("el")->setAttr("id", i2s(Page::Container|Page::Template))->setText(_("Container and template"));
    }
    else if(a_path == "/wdg/st/timestamp" && ctrChkNode(opt)) opt->setText(i2s(timeStamp()));
    else if(a_path == "/br/pg_" || a_path == "/page/page") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    pageList(lst);
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(trLU(pageAt(lst[i_f]).at().name(),l,u));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"), TSYS::oscdID);
	    if(pagePresent(vid)) throw TError(nodePath().c_str(), _("Page '%s' already present!"), vid.c_str());
	    pageAdd(vid, opt->text());
	    pageAt(vid).at().setOwner(opt->attr("user"));
	    pageAt(vid).at().manCrt = true;
	    opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	pageDel(opt->attr("id"),true);
    }
    else if(a_path == "/page/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	pageList(c_list);
	unsigned e_c = 0;
	for(unsigned i_p = 0; i_p < c_list.size(); i_p++)
	    if(pageAt(c_list[i_p]).at().enable()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

void Page::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	if(!parent().freeStat()) {
	    cntrCmdLinks(opt);
	    cntrCmdProcess(opt);
	}
	return;
    }

    //Process command to page
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt) || (parent( ).freeStat() ? false : cntrCmdLinks(opt) || cntrCmdProcess(opt))))
	TCntrNode::cntrCmdProc(opt);
}

bool Page::cntrCmdLinks( XMLNode *opt, bool lnk_ro )
{
    //Get page info
    if(opt->name() == "info")	return Widget::cntrCmdLinks(opt, lnk_ro);

    //Process command to page
    string a_path = opt->attr("path");
    if((a_path.compare(0,14,"/links/lnk/pl_") == 0 || a_path.compare(0,14,"/links/lnk/ls_") == 0) && ctrChkNode(opt)) {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size()) srcwdg = wdgAt(nwdg);
	else nattr = nwdg;

	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
	if(!(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut))) {
	    if(!is_pl) throw TError(nodePath().c_str(),_("Variable is not link"));
	    vector<string> a_ls;
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    srcwdg.at().attrList(a_ls);
	    unsigned i_a;
	    for(i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[i_a]; break; }
	    if(i_a >= a_ls.size()) throw TError(nodePath().c_str(),_("Variable is not link"));
	}

	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();

	// Link interface process
	int c_lv = 0;
	string obj_tp = TSYS::strSepParse(m_prm,0,':')+":";
	if(obj_tp.empty() || !(obj_tp == "val:" || obj_tp == "prm:" || obj_tp == "wdg:" || obj_tp == "arh:")) {
	    if(!is_pl) opt->childAdd("el")->setText(_("val:Constant value"));
	    opt->childAdd("el")->setText("prm:");
	    opt->childAdd("el")->setText("wdg:");
	    if(!is_pl && srcwdg.at().attrAt(nattr).at().flgGlob()&Attr::Address)
		opt->childAdd("el")->setText("arh:");
	}
        // Link elements process
	else {
	    int c_off = obj_tp.size();
	    vector<string> ls;
	    string c_path = obj_tp, cEl;
	    opt->childAdd("el")->setText("");

	    try {
		if(obj_tp == "prm:") {
		    m_prm = m_prm.substr(4);
		    if(is_pl && !SYS->daq().at().attrAt(m_prm,0,true).freeStat()) m_prm = m_prm.substr(0,m_prm.rfind("/"));
		    SYS->daq().at().ctrListPrmAttr(opt, m_prm, is_pl, 0, "prm:");
		}
		else if(obj_tp == "wdg:") {
		    opt->childAdd("el")->setText(c_path);
		    bool isAbs = (m_prm.compare(obj_tp.size(),1,"/") == 0);
		    for( ;(cEl=TSYS::pathLev(m_prm,0,true,&c_off)).size(); c_lv++) {
			c_path += ((c_lv||isAbs)?"/":"")+cEl;
			opt->childAdd("el")->setText(c_path);
		    }
		    if(!c_lv)  opt->childAdd("el")->setText(c_path+"/prj_"+ownerProj()->id());
		    else if(c_lv == 1 && isAbs) {
			ownerProj()->list(ls);
			if(ls.size()) opt->childAdd("el")->setText(_("=== Pages ==="));
			for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/pg_":"pg_")+ls[iL]);
			return true;
		    }

		    AutoHD<Widget> wnd = srcwdg.at().wdgAt(c_path.substr(4),0);
		    if(!wnd.freeStat()) {
			if(!isAbs && dynamic_cast<Widget*>(wnd.at().nodePrev())) opt->childAdd("el")->setText(c_path+(c_lv?"/..":".."));
			if(dynamic_cast<Page*>(&wnd.at())) {
			    ((AutoHD<Page>)wnd).at().pageList(ls);
			    if(ls.size()) opt->childAdd("el")->setText(_("=== Pages ==="));
			    for(unsigned iL = 0; iL < ls.size(); iL++)
				opt->childAdd("el")->setText(c_path+(c_lv?"/pg_":"pg_")+ls[iL]);
			}
			wnd.at().wdgList(ls, true);
			if(ls.size()) opt->childAdd("el")->setText(_("=== Widgets ==="));
			for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/wdg_":"wdg_")+ls[iL]);
			if(!is_pl) {
			    wnd.at().attrList(ls);
			    if(ls.size()) opt->childAdd("el")->setText(_("=== Attributes ==="));
			    for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/a_":"a_")+ls[iL]);
			}
		    }
		}
		else if(m_prm == "arh:") {
		    SYS->archive().at().valList(ls);
		    for(unsigned iL = 0; iL < ls.size(); iL++)
			opt->childAdd("el")->setText(c_path+ls[iL]);
		}
	    } catch(TError &err) { }
	}
    }
    else return Widget::cntrCmdLinks(opt, lnk_ro);

    return true;
}

//************************************************
//* PageWdg: Container stored widget             *
//************************************************
PageWdg::PageWdg( const string &iid, const string &isrcwdg ) : Widget(iid), TConfig(&mod->elInclWdg())
{
    cfg("ID").setS(id());
    mLnk = true;
    setParentNm(isrcwdg);
}

PageWdg::~PageWdg( )
{

}

TCntrNode &PageWdg::operator=( const TCntrNode &node )
{
    if(ownerPage().parentNm() == ".." && ownerPage().parent().at().wdgPresent(id())) {
	setParentNm(ownerPage().parent().at().path()+"/wdg_"+id());
	setEnable(true);
    }

    Widget::operator=(node);

    return *this;
}

Page &PageWdg::ownerPage( ) const	{ return *(Page*)nodePrev(); }

void PageWdg::postEnable( int flag )
{
    //Call parent method
    Widget::postEnable(flag);
    //Set parent page for this widget
    cfg("IDW").setS(ownerPage().path());
}

void PageWdg::preDisable( int flag )
{
    if(flag)	ChldResrv = !((flag>>8)&0x10) && !parent().freeStat() && parent().at().isLink();

    Widget::preDisable(flag);
}

void PageWdg::postDisable( int flag )
{
    if(flag) {
	string db  = ownerPage().ownerProj()->DB();
	string tbl = ownerPage().ownerProj()->tbl();

	//Remove from library table
	if(ChldResrv) {
	    cfg("PARENT").setS("<deleted>");
	    SYS->db().at().dataSet(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this);
	}
	else SYS->db().at().dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this, true);

	//Remove widget's work and users IO from library IO table
	string tAttrs = cfg("ATTRS").getS();

	TConfig cEl(&mod->elWdgIO());
	cEl.cfg("IDW").setS(ownerPage().path(), true); cEl.cfg("IDC").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", cEl);
	cEl.setElem(&mod->elWdgUIO());
	cEl.cfg("IDW").setS(ownerPage().path(), true); cEl.cfg("IDC").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", cEl);
    }
}

AutoHD<Widget> PageWdg::wdgAt( const string &wdg, int lev, int off ) const
{
    //Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerPage().ownerProj()->nodeAt(wdg,1); }
	catch(TError &err) { return AutoHD<Widget>(); }

    return Widget::wdgAt(wdg, lev, off);
}

string PageWdg::path( ) const	{ return ownerPage().path()+"/wdg_"+id(); }

string PageWdg::ico( ) const	{ return parent().freeStat() ? "" : parent().at().ico(); }

void PageWdg::setParentNm( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
}

void PageWdg::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    Widget::setEnable(val);

    //Disable heritors widgets
    if(val)
	for(unsigned i_h = 0; i_h < ownerPage().herit().size(); i_h++)
	    if(ownerPage().herit()[i_h].at().wdgPresent(id()) && !ownerPage().herit()[i_h].at().wdgAt(id()).at().enable())
		try { ownerPage().herit()[i_h].at().wdgAt(id()).at().setEnable(true); }
		catch(...) { mess_err(nodePath().c_str(),_("Inheriting widget '%s' enable error."),id().c_str()); }
}

string PageWdg::calcId( )	{ return parent().freeStat() ? "" : parent().at().calcId(); }

string PageWdg::calcLang( ) const	{ return parent().freeStat() ? "" : parent().at().calcLang(); }

string PageWdg::calcProg( ) const	{ return parent().freeStat() ? "" : parent().at().calcProg(); }

string PageWdg::calcProgStors( const string &attr ) { return parent().freeStat() ? "" : parent().at().calcProgStors(attr); }

int PageWdg::calcPer( ) const	{ return parent().freeStat() ? 0 : parent().at().calcPer(); }

void PageWdg::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(ownerPage().ownerProj()->DB())) throw TError();

    setStlLock(true);

    //Load generic widget's data
    string db  = ownerPage().ownerProj()->DB();
    string tbl = ownerPage().ownerProj()->tbl()+"_incl";
    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(db+"."+tbl, mod->nodePath()+tbl, *this);

    //Inherit modify attributes
    vector<string> als;
    attrList(als);
    string tAttrs = cfg("ATTRS").getS();
    for(unsigned i_a = 0; i_a < als.size(); i_a++) {
	if(!attrPresent(als[i_a])) continue;
	AutoHD<Attr> attr = attrAt(als[i_a]);
	if(attr.at().modif() && tAttrs.find(als[i_a]+";") == string::npos) {
	    attr.at().setModif(0);
	    inheritAttr(als[i_a]);
	}
    }

    //Load generic attributes
    mod->attrsLoad(*this, db+"."+ownerPage().ownerProj()->tbl(), ownerPage().path(), id(), tAttrs, true);

    //Load all other attributes
    loadIO();

    setStlLock(false);
}

void PageWdg::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerPage().ownerProj()->DB()+"."+ownerPage().ownerProj()->tbl(), ownerPage().path(), id(), cfg("ATTRS").getS());
}

void PageWdg::save_( )
{
    string db  = ownerPage().ownerProj()->DB();
    string tbl = ownerPage().ownerProj()->tbl();

    //Save generic attributes
    cfg("ATTRS").setS(mod->attrsSave(*this, db+"."+tbl, ownerPage().path(), id(), true));

    //Save generic widget's data
    SYS->db().at().dataSet(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this);

    //Save widget's attributes
    saveIO();
}

void PageWdg::saveIO( )
{
    if(!enable()) return;

    //> Save widget's attributes
    mod->attrsSave(*this, ownerPage().ownerProj()->DB()+"."+ownerPage().ownerProj()->tbl(), ownerPage().path(), id());
}

void PageWdg::wClear( )
{
    Widget::wClear();
    cfg("ATTRS").setS("");
}

void PageWdg::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf && !(nodeFlg()&SelfModifyS)) modifClr();
}

void PageWdg::resourceList( vector<string> &ls )
{
    ownerPage().resourceList(ls);
    if(!parent().freeStat()) parent().at().resourceList(ls);
}

string PageWdg::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;

    if((mimeData=ownerPage().resourceGet(id,&mimeType)).empty() && !parent().freeStat())
	mimeData = parent().at().resourceGet(id, &mimeType);
    if(mime) *mime = mimeType;

    return mimeData;
}

void PageWdg::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget link: ")+id(),RWRWR_,"root",SUI_ID);
	return;
    }
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt)))
	TCntrNode::cntrCmdProc(opt);
}

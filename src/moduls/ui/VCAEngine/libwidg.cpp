
//OpenSCADA system module UI.VCAEngine file: libwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vcaengine.h"
#include "libwidg.h"

using namespace VCA;

//************************************************
//* WidgetLib: Widgets library                   *
//************************************************
WidgetLib::WidgetLib( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elWdgLib()), mId(cfg("ID")), work_lib_db(lib_db), mEnable(false), passAutoEn(false)
{
    mId = id;
    cfg("NAME").setS(name);
    cfg("DB_TBL").setS(string("wlb_")+id);
    m_wdg = grpAdd("wdg_",(id=="originals")?true:false);
}

WidgetLib::~WidgetLib( )
{

}

TCntrNode &WidgetLib::operator=( TCntrNode &node )
{
    WidgetLib *src_n = dynamic_cast<WidgetLib*>(&node);
    if(!src_n) return *this;

    //Copy generic configuration
    exclCopy(*src_n, "ID;");
    cfg("DB_TBL").setS("wlb_"+id());
    work_lib_db = src_n->work_lib_db;

    if(!src_n->enable()) return *this;
    if(!enable()) setEnable(true);

    vector<string> pls;
    //Mime data copy
    src_n->mimeDataList(pls);
    string mimeType, mimeData;
    for(unsigned i_m = 0; i_m < pls.size(); i_m++) {
	src_n->mimeDataGet(pls[i_m], mimeType, &mimeData);
	mimeDataSet(pls[i_m], mimeType, mimeData);
    }

    // Copy include pages
    src_n->list(pls);
    for(unsigned i_p = 0; i_p < pls.size(); i_p++) {
	if(!present(pls[i_p])) add(pls[i_p],"");
	(TCntrNode&)at(pls[i_p]).at() = (TCntrNode&)src_n->at(pls[i_p]).at();
    }

    return *this;
}

void WidgetLib::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore)	setEnable(true);
}

void WidgetLib::preDisable( int flag )
{
    if(enable()) setEnable(false);
}

void WidgetLib::postDisable( int flag )
{
    if(flag) {
	//Delete libraries record
	SYS->db().at().dataDel(DB()+"."+mod->wlbTable(),mod->nodePath()+"LIB/",*this,true);

	//Delete function's files
	// Delete widgets table
	SYS->db().at().open(fullDB());
	SYS->db().at().close(fullDB(),true);
	// Delete attributes table
	SYS->db().at().open(fullDB()+"_io");
	SYS->db().at().close(fullDB()+"_io",true);
	// Delete user attributes table
	SYS->db().at().open(fullDB()+"_uio");
	SYS->db().at().close(fullDB()+"_uio",true);
	// Delete include widgets table
	SYS->db().at().open(fullDB()+"_incl");
	SYS->db().at().close(fullDB()+"_incl",true);
	// Delete mime-data table
	SYS->db().at().open(fullDB()+"_mime");
	SYS->db().at().close(fullDB()+"_mime",true);
    }
}

AutoHD<TCntrNode> WidgetLib::chldAt( int8_t igr, const string &name, const string &user )
{
    AutoHD<TCntrNode> nd = TCntrNode::chldAt(igr, name, user);
    if(igr == m_wdg && !nd.freeStat()) {
	AutoHD<LWidget> lwdg = nd;
	if(!lwdg.freeStat() && !lwdg.at().enable() && !passAutoEn && lwdg.at().enableByNeed) {
	    lwdg.at().enableByNeed = false;
	    try {
		lwdg.at().load(true);
		lwdg.at().setEnable(true);
		lwdg.at().modifGClr();
	    }
	    catch(TError err) { }
	}
    }

    return nd;
}

string WidgetLib::name( )
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : mId;
}

void WidgetLib::setFullDB( const string &it )
{
    size_t dpos = it.rfind(".");
    work_lib_db = (dpos!=string::npos) ? it.substr(0,dpos) : "";
    cfg("DB_TBL").setS((dpos!=string::npos) ? it.substr(dpos+1) : "");
    modifG();
}

void WidgetLib::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    mess_info(nodePath().c_str(),_("Load widget library."));

    SYS->db().at().dataGet(DB()+"."+mod->wlbTable(),mod->nodePath()+"LIB/",*this);

    passAutoEn = true;

    //Create new widgets
    map<string, bool>   itReg;
    TConfig c_el(&mod->elWdg());
    c_el.cfgViewAll(false);
    for(int fld_cnt = 0; SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl(),fld_cnt++,c_el); ) {
	string f_id = c_el.cfg("ID").getS();
	if(!present(f_id)) { add(f_id,"",""); at(f_id).at().setEnableByNeed(); }
	itReg[f_id] = true;
    }

    // Check for remove items removed from DB
    if(!SYS->selDB().empty()) {
	vector<string> it_ls;
	list(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		del(it_ls[i_it]);
    }

    passAutoEn = false;

    mOldDB = TBDS::realDBName(DB());
}

void WidgetLib::save_( )
{
    mess_info(nodePath().c_str(),_("Save widget library."));

    SYS->db().at().dataSet(DB()+"."+mod->wlbTable(),mod->nodePath()+"LIB/",*this);

    //Check for need copy mime data to other DB and same copy
    if(!mOldDB.empty() && mOldDB != TBDS::realDBName(DB())) {
	vector<string> pls;
	mimeDataList(pls,mOldDB);
	string mimeType, mimeData;
	for(unsigned i_m = 0; i_m < pls.size(); i_m++) {
	    mimeDataGet(pls[i_m], mimeType, &mimeData, mOldDB);
	    mimeDataSet(pls[i_m], mimeType, mimeData, DB());
	}
    }

    mOldDB = TBDS::realDBName(DB());
}

void WidgetLib::setEnable( bool val )
{
    if(val == enable())	return;

    mess_info(nodePath().c_str(),val ? _("Enable widgets library.") : _("Disable widgets library."));

    passAutoEn = true;

    vector<string> f_lst;
    list(f_lst);
    for(unsigned i_ls = 0; i_ls < f_lst.size(); i_ls++) {
	if(at(f_lst[i_ls]).at().enableByNeed)	continue;
	try { at(f_lst[i_ls]).at().setEnable(val); }
	catch(TError err)
	{ mess_err(nodePath().c_str(),_("Enable/disable widget '%s' error %s."),f_lst[i_ls].c_str(),err.mess.c_str()); }
    }

    passAutoEn = false;

    mEnable = val;
}

void WidgetLib::mimeDataList( vector<string> &list, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig c_el(&mod->elWdgData());
    c_el.cfgViewAll(false);

    list.clear();
    for(int fld_cnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,mod->nodePath()+wtbl,fld_cnt,c_el); fld_cnt++)
	list.push_back(c_el.cfg("ID").getS());
}

bool WidgetLib::mimeDataGet( const string &iid, string &mimeType, string *mimeData, const string &idb )
{
    bool is_file = (iid.compare(0,5,"file:")==0);
    bool is_res  = (iid.compare(0,4,"res:")==0);

    if(!is_file) {
	//Get resource file from DB
	string dbid = is_res ? iid.substr(4) : iid;
	string wtbl = tbl()+"_mime";
	string wdb  = idb.empty() ? DB() : idb;
	TConfig c_el(&mod->elWdgData());
	if(!mimeData) c_el.cfg("DATA").setView(false);
	c_el.cfg("ID").setS( dbid );
	if(SYS->db().at().dataGet(wdb+"."+wtbl,mod->nodePath()+wtbl,c_el,false,true)) {
	    mimeType = c_el.cfg("MIME").getS();
	    if(mimeData) *mimeData = c_el.cfg("DATA").getS();
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
	if(hd == -1)	return false;

	//Check for size limit
	if(lseek(hd,0,SEEK_END) > 100*1024*1024) { close(hd); return false; }
	lseek(hd,0,SEEK_SET);

	while((len=read(hd,buf,sizeof(buf))) > 0) rez.append(buf,len);
	close(hd);

	mimeType = ((filepath.rfind(".") != string::npos) ? filepath.substr(filepath.rfind(".")+1)+";" : "file/unknown;")+i2s(rez.size());
	if(mimeData) *mimeData = TSYS::strEncode(rez,TSYS::base64);

	return true;
    }

    return false;
}

void WidgetLib::mimeDataSet( const string &iid, const string &mimeType, const string &mimeData, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig c_el(&mod->elWdgData());
    c_el.cfg("ID").setS(iid);
    c_el.cfg("MIME").setS(mimeType);
    if(!mimeData.size()) c_el.cfg("DATA").setView(false);
    else c_el.cfg("DATA").setS(mimeData);
    SYS->db().at().dataSet(wdb+"."+wtbl, mod->nodePath()+wtbl, c_el, false, true);
}

void WidgetLib::mimeDataDel( const string &iid, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig c_el(&mod->elWdgData());
    c_el.cfg("ID").setS(iid,true);
    SYS->db().at().dataDel(wdb+"."+wtbl, mod->nodePath()+wtbl, c_el, false, false, true);
}

void WidgetLib::add( const string &id, const string &name, const string &orig )
{
    if(present(id))	return;
    chldAdd(m_wdg,new LWidget(id,orig));
    at(id).at().setName(name);
}

void WidgetLib::add( LWidget *iwdg )
{
    if(present(iwdg->id()))	delete iwdg;
    else chldAdd(m_wdg,iwdg);
}

AutoHD<LWidget> WidgetLib::at( const string &id )	{ return chldAt(m_wdg,id); }

void WidgetLib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widgets library: ")+id(),RWRWR_,"root",SUI_ID);
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ctrMkNode("area",opt,-1,"/obj",_("Library"))) {
	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enable"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/db",_("Library DB"),RWRWR_,"root",SUI_ID,4,
		    "tp","str","dest","sel_ed","select",("/db/tblList:wlb_"+id()).c_str(),
		    "help",_("DB address in format [<DB module>.<DB name>.<Table name>].\nFor use main work DB set '*.*'."));
		ctrMkNode("fld",opt,-1,"/obj/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/obj/cfg/id",_("Id"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,2,"tp","str","len",OBJ_NM_SZ);
		ctrMkNode("fld",opt,-1,"/obj/cfg/descr",_("Description"),RWRWR_,"root",SUI_ID,3,"tp","str","cols","100","rows","3");
		ctrMkNode("img",opt,-1,"/obj/cfg/ico",_("Icon"),RWRWR_,"root",SUI_ID,2,"v_sz","64","h_sz","64");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widgets")))
	    ctrMkNode("list",opt,-1,"/wdg/wdg",_("Widgets"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","wdg_","idSz","30");
	if(ctrMkNode("area",opt,-1,"/mime",_("Mime data")))
	    if(ctrMkNode("table",opt,-1,"/mime/mime",_("Mime data"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/mime/mime/id",_("Id"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/tp",_("Mime type"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/dt",_("Data"),RWRWR_,"root",SUI_ID,2,"tp","str","dest","data");
	    }
	return;
    }

    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user");
    if(a_path == "/obj/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(atoi(opt->text().c_str()));
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
    else if(a_path == "/obj/cfg/ico" || a_path == "/ico") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(ico());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setIco(opt->text());
    }
    else if(a_path == "/obj/cfg/id" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID)) opt->setText(id());
    else if(a_path == "/obj/cfg/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trU(name(),u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(trSetU(name(),u,opt->text()));
    }
    else if(a_path == "/obj/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trU(descr(),u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(trSetU(descr(),u,opt->text()));
    }
    else if(a_path == "/br/wdg_" || a_path == "/wdg/wdg") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned i_f=0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(trU(at(lst[i_f]).at().name(),u));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    add(vid,opt->text().c_str()); at(vid).at().setOwner(opt->attr("user"));
	    opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR)) del(opt->attr("id"),true);
    }
    else if(a_path == "/mime/mime") {
	// Request data
	string idmime = opt->attr("key_id");
	string idcol  = opt->attr("col");

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(!idmime.empty() && idcol == "dt" && atoi(opt->attr("data").c_str())) {
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
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))	mimeDataSet("newMime","media/unknown;0","");
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	mimeDataDel(opt->attr("key_id"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    // Request data
	    if(idcol == "id") {
		string mimeType, mimeData;
		// Copy mime data to new record
		if(mimeDataGet("res:"+idmime, mimeType, &mimeData)) {
		    mimeDataSet(opt->text(), mimeType, mimeData);
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
		if(!mimeDataGet("res:"+idmime, mimeType)) {
		    size_t extP = idmime.rfind(".");
		    if(extP == string::npos || extP == 0 || extP == (idmime.size()-1)) mimeType = "media/unknown";
		    else { mimeType = "media/"+idmime.substr(extP+1); idmime = idmime.substr(0,extP); }
		}
		mimeDataSet(idmime, TSYS::strSepParse(mimeType,0,';')+";"+r2s((float)opt->text().size()/1024,6),opt->text());
	    }
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* LWidget: Library stored widget               *
//************************************************
LWidget::LWidget( const string &iid, const string &isrcwdg ) :
	Widget(iid), TConfig(&mod->elWdg()), enableByNeed(false), m_proc_per(cfg("PROC_PER").getId()), mTimeStamp(cfg("TIMESTAMP").getId())
{
    cfg("ID").setS(id());

    setParentNm(isrcwdg);
    setNodeFlg(TCntrNode::SelfSaveForceOnChild);
}

LWidget::~LWidget( )
{

}

WidgetLib &LWidget::ownerLib( )	{ return *(WidgetLib*)nodePrev(); }

void LWidget::postDisable( int flag )
{
    if(flag) {
	string db  = ownerLib().DB();
	string tbl = ownerLib().tbl();

	//Remove from library table
	SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, *this, true);

	//Remove widget's IO from library IO table
	TConfig c_el(&mod->elWdgIO());
	c_el.cfg("IDW").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", c_el);

	//Remove widget's user IO from library IO table
	c_el.setElem(&mod->elWdgUIO());
	c_el.cfg("IDW").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", c_el);

	//Remove widget's included widgets from library include table
	c_el.setElem(&mod->elInclWdg());
	c_el.cfg("IDW").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", c_el);
    }
}

string LWidget::path( )		{ return "/wlb_"+ownerLib().id()+"/wdg_"+id(); }

string LWidget::ico( )
{
    string rIco = cfg("ICO").getS();
    if(rIco.size()) return rIco;
    if(!parent().freeStat())  return parent().at().ico();
    return "";
}

string LWidget::calcId( )
{
    if(proc().empty()) {
	if(!parent().freeStat()) return parent().at().calcId( );
	return "";
    }
    return "L_"+ownerLib().id()+"_"+id();
}

string LWidget::calcLang( )
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcLang();

    string iprg = proc();
    if(iprg.find("\n") == string::npos) {
	iprg = iprg+"\n";
	cfg("PROC").setS(iprg);
    }
    return iprg.substr(0,iprg.find("\n"));
}

string LWidget::calcProg( )
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcProg();

    string iprg = proc();
    size_t lng_end = iprg.find("\n");
    if(lng_end == string::npos) lng_end=0;
    else lng_end++;
    return iprg.substr(lng_end);
}

string LWidget::calcProgStors( const string &attr )
{
    string rez = parent().freeStat() ? "" : parent().at().calcProgStors(attr);
    if((attr.size() && attrAt(attr).at().modif()) || (!attr.size() && proc().size()) && rez.find(ownerLib().DB()) == string::npos)
	rez = ownerLib().DB() + ";" + rez;
    return rez;
}

int LWidget::calcPer( )
{
    if(m_proc_per < 0 && !parent().freeStat())	return parent().at().calcPer();
    return m_proc_per;
}

void LWidget::setCalcLang( const string &ilng )
{
    cfg("PROC").setS(ilng.empty() ? "" : ilng+"\n"+calcProg());
    modif();
}

void LWidget::setCalcProg( const string &iprg )
{
    cfg("PROC").setS(calcLang()+"\n"+iprg);
    modif();
}

void LWidget::setCalcPer( int vl )
{
    m_proc_per = vl;
    modif();
}

void LWidget::setParentNm( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
    modif();
}

void LWidget::setEnable( bool val )
{
    if(enable() == val) return;

    Widget::setEnable(val);

    //Include widgets link update on the parrent change
    if(val) {
	if(mParentNmPrev.size() && parentNm() != mParentNmPrev) {
	    vector<string> lst;
	    wdgList(lst, true);
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		try {
		    AutoHD<Widget> iw = wdgAt(lst[i_l]);
		    if(iw.at().parentNm().compare(0,mParentNmPrev.size()+1,mParentNmPrev+"/") == 0) {
			iw.at().setParentNm(parentNm()+iw.at().parentNm().substr(mParentNmPrev.size()));
			iw.at().setEnable(true);
		    }
		}
		catch(TError err) { }
	}
	mParentNmPrev = parentNm();
    }
}

void LWidget::setEnableByNeed( )
{
    enableByNeed = true;
    modifClr();
}

void LWidget::load_( )
{
    if(!SYS->chkSelDB(ownerLib().DB())) throw TError();

    //Load generic widget's data
    string db  = ownerLib().DB();
    string tbl = ownerLib().tbl();
    SYS->db().at().dataGet(db+"."+tbl, mod->nodePath()+tbl, *this);

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
    mod->attrsLoad(*this, db+"."+tbl, id(), "", tAttrs, true);

    //Load all other attributes
    loadIO();
}

void LWidget::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerLib().DB()+"."+ownerLib().tbl(), id(), "", cfg("ATTRS").getS());

    //Load cotainer widgets
    if(!isContainer()) return;
    map<string, bool>   itReg;
    TConfig c_el(&mod->elInclWdg());
    string db  = ownerLib().DB();
    string tbl = ownerLib().tbl()+"_incl";
    c_el.cfg("IDW").setS(id(),true);
    for(int fld_cnt = 0; SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el); ) {
	string sid  = c_el.cfg("ID").getS();
	if(c_el.cfg("PARENT").getS() == "<deleted>") {
	    if(wdgPresent(sid))	wdgDel(sid);
	    continue;
	}
	if(!wdgPresent(sid))
	    try{ wdgAdd(sid,"",""); }
	    catch(TError err){ mess_err(err.cat.c_str(),err.mess.c_str()); }

	wdgAt(sid).at().load();
	itReg[sid] = true;
    }

    //Check for remove items removed from DB
    if(!SYS->selDB().empty()) {
	vector<string> it_ls;
	wdgList(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		wdgDel(it_ls[i_it]);
    }
}

void LWidget::save_( )
{
    string db  = ownerLib().DB();
    string tbl = ownerLib().tbl();

    //Save generic attributes
    cfg("ATTRS").setS(mod->attrsSave(*this, db+"."+tbl, id(), "", true));

    //Save generic widget's data
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(db+"."+tbl, mod->nodePath()+tbl, *this);

    //Save widget's attributes
    saveIO();
}

void LWidget::saveIO( )
{
    if(!enable()) return;

    //Save widget's attributes
    mod->attrsSave(*this, ownerLib().DB()+"."+ownerLib().tbl(), id(), "");
}

void LWidget::wClear( )
{
    Widget::wClear();

    cfg("ATTRS").setS("");
    cfg("PROC").setS("");
}

void LWidget::wdgAdd( const string &wid, const string &name, const string &path, bool force )
{
    if(!isContainer())  throw TError(nodePath().c_str(),_("Widget is not container!"));
    if(wdgPresent(wid)) return;

    //Check for label <deleted>
    if(!force) {
	string db  = ownerLib().DB();
	string tbl = ownerLib().tbl()+"_incl";
	TConfig c_el( &mod->elInclWdg() );
	c_el.cfg("IDW").setS(id());
	c_el.cfg("ID").setS(wid);
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true) && c_el.cfg("PARENT").getS() == "<deleted>") {
	    if(!parent().at().wdgPresent(wid))	SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, c_el, true, false, true);
	    else throw TError(nodePath().c_str(),_("You try to create widget with name '%s' of the widget that was the early inherited and deleted from base container!"),wid.c_str());
	}
    }

    //Same widget add
    chldAdd(inclWdg,new CWidget(wid,path));
    wdgAt(wid).at().setName(name);

    //Call heritors include widgets update
    for(unsigned i_h = 0; i_h < m_herit.size(); i_h++)
	if(m_herit[i_h].at().enable())
	    m_herit[i_h].at().inheritIncl(wid);
}

AutoHD<CWidget> LWidget::wdgAt( const string &wdg )	{ return Widget::wdgAt(wdg); }

void LWidget::resourceList( vector<string> &ls )
{
    //Append to the map for doublets remove
    map<string,bool> sortLs;
    for(unsigned i_l = 0; i_l < ls.size(); i_l++) sortLs[ls[i_l]] = true;
    ownerLib().mimeDataList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++) sortLs[ls[i_l]] = true;
    ls.clear();
    for(map<string,bool>::iterator i_l = sortLs.begin(); i_l != sortLs.end(); ++i_l) ls.push_back(i_l->first);

    if(!parent().freeStat()) parent().at().resourceList(ls);
}

string LWidget::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;

    if(!ownerLib().mimeDataGet(id,mimeType,&mimeData) && !parent().freeStat())
	mimeData = parent().at().resourceGet(id, &mimeType);
    if(mime) *mime = mimeType;

    return mimeData;
}

void LWidget::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr( attr );
    if(!mdf)	modifClr( );
}

void LWidget::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	ctrMkNode("fld",opt,-1,"/wdg/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	cntrCmdAttributes(opt);
	cntrCmdLinks(opt);
	cntrCmdProcess(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Library widget: ")+id());
	return;
    }
    if(cntrCmdGeneric(opt) || cntrCmdAttributes(opt) || cntrCmdLinks(opt) || cntrCmdProcess(opt)) ;
    else if(opt->attr("path") == "/wdg/st/timestamp" && ctrChkNode(opt)) opt->setText(i2s(timeStamp()));
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* CWidget: Container stored widget             *
//************************************************
CWidget::CWidget( const string &iid, const string &isrcwdg ) : Widget(iid), TConfig(&mod->elInclWdg())
{
    cfg("ID").setS(id());
    m_lnk = true;
    setParentNm(isrcwdg);
}

CWidget::~CWidget( )
{

}

string CWidget::path( )		{ return "/wlb_"+ownerLWdg().ownerLib().id()+"/wdg_"+ownerLWdg().id()+"/wdg_"+id(); }

LWidget &CWidget::ownerLWdg()	{ return *(LWidget*)nodePrev(); }

void CWidget::postEnable( int flag )
{
    //Call parent methos
    Widget::postEnable(flag);
    //Set container widget id
    cfg("IDW").setS(ownerLWdg().id());
}

void CWidget::preDisable( int flag )
{
    if(flag)	ChldResrv = !((flag>>8)&0x10) && !parent().freeStat() && parent().at().isLink();

    Widget::preDisable(flag);
}

void CWidget::postDisable( int flag )
{
    if(flag) {
	string db  = ownerLWdg().ownerLib().DB();
	string tbl = ownerLWdg().ownerLib().tbl();

	//Remove from library table
	if(ChldResrv) {
	    cfg("PARENT").setS("<deleted>");
	    SYS->db().at().dataSet(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this);
	}
	else SYS->db().at().dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this, true);

	//Remove widget's work and users IO from library IO table
	string tAttrs = cfg("ATTRS").getS();

	TConfig c_el(&mod->elWdgIO());
	c_el.cfg("IDW").setS(ownerLWdg().id(), true); c_el.cfg("IDC").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", c_el);
	c_el.setElem(&mod->elWdgUIO());
	c_el.cfg("IDW").setS(ownerLWdg().id(), true); c_el.cfg("IDC").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", c_el);
    }
}

string CWidget::ico( )	{ return parent().freeStat() ? "" : parent().at().ico(); }

void CWidget::setParentNm( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
    modif();
}

void CWidget::setEnable( bool val )
{
    if(enable() == val) return;

    Widget::setEnable(val);

    //Enable heritors widgets
    if(val)
	for(unsigned i_h = 0; i_h < ownerLWdg().herit().size(); i_h++)
	    if(!ownerLWdg().herit()[i_h].at().wdgAt(id()).at().enable() && ownerLWdg().herit()[i_h].at().wdgPresent(id()))
		try { ownerLWdg().herit()[i_h].at().wdgAt(id()).at().setEnable(true); }
		catch(...) { mess_err(nodePath().c_str(),_("Inheriting widget '%s' enable error."),id().c_str()); }
}

string CWidget::calcId( )	{ return parent().freeStat() ? "" : parent().at().calcId(); }

string CWidget::calcLang( )	{ return parent().freeStat() ? "" : parent().at().calcLang(); }

string CWidget::calcProg( )	{ return parent().freeStat() ? "" : parent().at().calcProg(); }

string CWidget::calcProgStors( const string &attr ){ return parent().freeStat() ? "" : parent().at().calcProgStors(attr); }

int CWidget::calcPer( )		{ return parent().freeStat() ? 0 : parent().at().calcPer(); }

void CWidget::load_( )
{
    if(!SYS->chkSelDB(ownerLWdg().ownerLib().DB())) throw TError();

    //Load generic widget's data
    string db  = ownerLWdg().ownerLib().DB();
    string tbl = ownerLWdg().ownerLib().tbl()+"_incl";
    SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,*this);

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
    mod->attrsLoad(*this, db+"."+ownerLWdg().ownerLib().tbl(), ownerLWdg().id(), id(), tAttrs, true);

    //Load all other attributes
    loadIO();
}

void CWidget::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerLWdg().ownerLib().DB()+"."+ownerLWdg().ownerLib().tbl(), ownerLWdg().id(), id(), cfg("ATTRS").getS());
}

void CWidget::save_( )
{
    string db  = ownerLWdg().ownerLib().DB();
    string tbl = ownerLWdg().ownerLib().tbl();

    //Save generic attributes
    cfg("ATTRS").setS(mod->attrsSave(*this, db+"."+tbl, ownerLWdg().id(), id(), true));

    //Save generic widget's data
    SYS->db().at().dataSet(db+"."+tbl+"_incl",mod->nodePath()+tbl+"_incl",*this);

    //Save widget's attributes
    saveIO();
}

void CWidget::saveIO( )
{
    if(!enable()) return;

    mod->attrsSave(*this, ownerLWdg().ownerLib().DB()+"."+ownerLWdg().ownerLib().tbl(), ownerLWdg().id(), id());
}

void CWidget::wClear( )
{
    Widget::wClear();
    cfg("ATTRS").setS("");
}

void CWidget::resourceList( vector<string> &ls )
{
    ownerLWdg().resourceList(ls);
    if(!parent().freeStat()) parent().at().resourceList(ls);
}

string CWidget::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;

    if((mimeData=ownerLWdg().resourceGet(id,&mimeType)).empty() && !parent().freeStat())
	mimeData = parent().at().resourceGet(id, &mimeType);
    if(mime) *mime = mimeType;

    return mimeData;
}

void CWidget::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf)  modifClr( );
}

void CWidget::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Link to widget: ")+id(),RWRWR_,"root",SUI_ID);
	return;
    }
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt)))
	TCntrNode::cntrCmdProc(opt);
}

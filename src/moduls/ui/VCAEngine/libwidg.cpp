
//OpenSCADA system module UI.VCAEngine file: libwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
    TConfig(&mod->elWdgLib()), mId(cfg("ID")), workLibDB(lib_db), mEnable(false), passAutoEn(false)
{
    mId = id;
    cfg("NAME").setS(name);
    cfg("DB_TBL").setS(string("wlb_")+id);
    mWdg = grpAdd("wdg_",(id=="originals")?true:false);
}

WidgetLib::~WidgetLib( )
{

}

TCntrNode &WidgetLib::operator=( const TCntrNode &node )
{
    const WidgetLib *src_n = dynamic_cast<const WidgetLib*>(&node);
    if(!src_n) return *this;

    //Copy generic configuration
    exclCopy(*src_n, "ID;");
    cfg("DB_TBL").setS("wlb_"+id());
    workLibDB = src_n->workLibDB;

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

AutoHD<TCntrNode> WidgetLib::chldAt( int8_t igr, const string &name, const string &user ) const
{
    AutoHD<TCntrNode> nd = TCntrNode::chldAt(igr, name, user);
    if(igr == mWdg && !nd.freeStat()) {
	AutoHD<LWidget> lwdg = nd;
	if(!lwdg.freeStat() && !lwdg.at().enable() && !passAutoEn && lwdg.at().enableByNeed) {
	    lwdg.at().enableByNeed = false;
	    try {
		lwdg.at().modifG();
		lwdg.at().load();
		lwdg.at().setEnable(true);
		lwdg.at().modifGClr();
	    } catch(TError &err) { }
	}
    }

    return nd;
}

string WidgetLib::name( ) const
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : mId;
}

void WidgetLib::setFullDB( const string &it )
{
    size_t dpos = it.rfind(".");
    workLibDB = (dpos!=string::npos) ? it.substr(0,dpos) : "";
    cfg("DB_TBL").setS((dpos!=string::npos) ? it.substr(dpos+1) : "");
    modifG();
}

void WidgetLib::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    mess_debug(nodePath().c_str(),_("Load widget library."));

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(DB()+"."+mod->wlbTable(),mod->nodePath()+"LIB/",*this);

    passAutoEn = true;

    //Create new widgets
    map<string, bool>	itReg;
    vector<vector<string> > full;
    TConfig cEl(&mod->elWdg());
    //cEl.cfgViewAll(false);
    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl(),fldCnt++,cEl,false,&full); ) {
	string fId = cEl.cfg("ID").getS();
	if(!present(fId)) { add(fId, "", ""); at(fId).at().setEnableByNeed(); }
	at(fId).at().load(&cEl);
	itReg[fId] = true;
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
    mess_debug(nodePath().c_str(),_("Save widget library."));

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

void WidgetLib::setEnable( bool val, bool force )
{
    if(val == enable())	return;

    mess_debug(nodePath().c_str(),val ? _("Enable widgets library.") : _("Disable widgets library."));

    passAutoEn = true;

    vector<string> f_lst;
    list(f_lst);
    for(unsigned i_ls = 0; i_ls < f_lst.size(); i_ls++) {
	if(at(f_lst[i_ls]).at().enableByNeed)	continue;
	try { at(f_lst[i_ls]).at().setEnable(val); }
	catch(TError &err) { mess_err(nodePath().c_str(),_("Enable/disable widget '%s' error %s."),f_lst[i_ls].c_str(),err.mess.c_str()); }
    }

    passAutoEn = false;

    mEnable = val;
}

void WidgetLib::mimeDataList( vector<string> &list, const string &idb ) const
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

bool WidgetLib::mimeDataGet( const string &iid, string &mimeType, string *mimeData, const string &idb ) const
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
	char buf[STR_BUF_LEN];
	string rez;
	int hd = open(filepath.c_str(), O_RDONLY);
	if(hd == -1)	return false;

	//Check for size limit
	if(lseek(hd,0,SEEK_END) > 100*1024*1024) { close(hd); return false; }
	lseek(hd, 0, SEEK_SET);
	for(int len; (len=read(hd,buf,sizeof(buf))) > 0; ) rez.append(buf, len);
	close(hd);

	mimeType = TUIS::mimeGet(filepath, rez);
	if(mimeData) *mimeData = TSYS::strEncode(rez,TSYS::base64);

	return true;
    }

    return false;
}

void WidgetLib::mimeDataSet( const string &iid, const string &mimeType, const string &mimeData, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid);
    cEl.cfg("MIME").setS(mimeType);
    if(!mimeData.size()) cEl.cfg("DATA").setView(false);
    else cEl.cfg("DATA").setS(mimeData);
    SYS->db().at().dataSet(wdb+"."+wtbl, mod->nodePath()+wtbl, cEl, false, true);
}

void WidgetLib::mimeDataDel( const string &iid, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid,true);
    SYS->db().at().dataDel(wdb+"."+wtbl, mod->nodePath()+wtbl, cEl, false, false, true);
}

void WidgetLib::add( const string &id, const string &name, const string &orig )
{
    if(present(id))	return;
    chldAdd(mWdg, new LWidget(id,orig));
    at(id).at().setName(name);
}

void WidgetLib::add( LWidget *iwdg )
{
    if(present(iwdg->id()))	delete iwdg;
    else chldAdd(mWdg, iwdg);
}

AutoHD<LWidget> WidgetLib::at( const string &id ) const	{ return chldAt(mWdg, id); }

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
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enabled"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/db",_("Library DB"),RWRWR_,"root",SUI_ID,4,
		    "tp","str","dest","sel_ed","select",("/db/tblList:wlb_"+id()).c_str(),
		    "help",_("DB address in format [<DB module>.<DB name>.<Table name>].\nFor use main work DB set '*.*'."));
		ctrMkNode("fld",opt,-1,"/obj/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/obj/cfg/id",_("Identifier"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,2,"tp","str","len",OBJ_NM_SZ);
		ctrMkNode("fld",opt,-1,"/obj/cfg/descr",_("Description"),RWRWR_,"root",SUI_ID,3,"tp","str","cols","100","rows","3");
		ctrMkNode("img",opt,-1,"/obj/cfg/ico",_("Icon"),RWRWR_,"root",SUI_ID,2,"v_sz","64","h_sz","64");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widgets")))
	    ctrMkNode("list",opt,-1,"/wdg/wdg",_("Widgets"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","wdg_","idSz","30");
	if(ctrMkNode("area",opt,-1,"/mime",_("Mime data")))
	    if(ctrMkNode("table",opt,-1,"/mime/mime",_("Mime data"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/mime/mime/id",_("Identifier"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/tp",_("Mime type"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/dt",_("Data"),RWRWR_,"root",SUI_ID,2,"tp","str","dest","data");
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
    else if(a_path == "/obj/cfg/ico" || a_path == "/ico") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(ico());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setIco(opt->text());
    }
    else if(a_path == "/obj/cfg/id" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID)) opt->setText(id());
    else if(a_path == "/obj/cfg/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trLU(name(),l,u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(trSetLU(name(),l,u,opt->text()));
    }
    else if(a_path == "/obj/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trLU(descr(),l,u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(trSetLU(descr(),l,u,opt->text()));
    }
    else if(a_path == "/br/wdg_" || a_path == "/wdg/wdg") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned i_f=0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(trLU(at(lst[i_f]).at().name(),l,u));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    if(present(vid)) throw TError(nodePath().c_str(), _("Widget '%s' already present!"), vid.c_str());
	    add(vid, opt->text()); at(vid).at().setOwner(opt->attr("user"));
	    opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR)) del(opt->attr("id"),true);
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
		if(mimeDataGet("res:"+idmime,mimeType,&mimeData)) {
		    mimeDataSet(opt->text(), TUIS::mimeGet(idmime,mimeData,mimeType), mimeData);
		    mimeDataDel(idmime);
		}
	    }
	    else if(idcol == "tp") {
		string mimeType;
		// Copy mime data to new record
		if(mimeDataGet("res:"+idmime,mimeType))
		    mimeDataSet(idmime, opt->text()+";"+TSYS::strSepParse(mimeType,1,';'), "");
	    }
	    else if(idcol == "dt") {
		string mimeType;
		if(!mimeDataGet("res:"+idmime,mimeType)) mimeType = TUIS::mimeGet(idmime, TSYS::strDecode(opt->text(),TSYS::base64));
		mimeDataSet(idmime, TSYS::strSepParse(mimeType,0,';')+";"+r2s((float)opt->text().size()/1024,6),opt->text());
	    }
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* LWidget: Library stored widget               *
//************************************************
LWidget::LWidget( const string &iid, const string &isrcwdg ) : Widget(iid), TConfig(&mod->elWdg()),
    enableByNeed(false), mProcPer(cfg("PROC_PER").getId()), mTimeStamp(cfg("TIMESTAMP").getId()), mFuncM(true)
{
    cfg("ID").setS(id());
    cfg("PROC").setExtVal(true);

    setParentNm(isrcwdg);
    setNodeFlg(TCntrNode::SelfSaveForceOnChild);
}

LWidget::~LWidget( )
{

}

WidgetLib &LWidget::ownerLib( ) const	{ return *(WidgetLib*)nodePrev(); }

void LWidget::postDisable( int flag )
{
    if(flag) {
	string db  = ownerLib().DB();
	string tbl = ownerLib().tbl();

	//Remove from library table
	SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, *this, true);

	//Remove widget's IO from library IO table
	TConfig cEl(&mod->elWdgIO());
	cEl.cfg("IDW").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", cEl);

	//Remove widget's user IO from library IO table
	cEl.setElem(&mod->elWdgUIO());
	cEl.cfg("IDW").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", cEl);

	//Remove widget's included widgets from library include table
	cEl.setElem(&mod->elInclWdg());
	cEl.cfg("IDW").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", cEl);
    }
}

string LWidget::path( ) const	{ return "/wlb_"+ownerLib().id()+"/wdg_"+id(); }

string LWidget::ico( ) const
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

string LWidget::calcLang( ) const
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcLang();

    string iprg = proc();
    if(iprg.find("\n") == string::npos) {
	iprg = iprg+"\n";
	cfg("PROC").setS(iprg);
    }
    return iprg.substr(0,iprg.find("\n"));
}

bool LWidget::calcProgTr( )	{ return (!proc().size() && !parent().freeStat()) ? parent().at().calcProgTr() : cfg("PR_TR"); }

string LWidget::calcProg( ) const
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcProg();

    string iprg = proc();
    size_t lng_end = iprg.find("\n");
    if(lng_end == string::npos) lng_end = 0;
    else lng_end++;
    return iprg.substr(lng_end);
}

string LWidget::calcProgStors( const string &attr )
{
    string rez = parent().freeStat() ? "" : parent().at().calcProgStors(attr);
    if(((attr.size() && attrAt(attr).at().modif()) || (!attr.size() && proc().size())) && rez.find(ownerLib().DB()) == string::npos)
	rez = ownerLib().DB() + ";" + rez;
    return rez;
}

int LWidget::calcPer( ) const	{ return (mProcPer < 0 && !parent().freeStat()) ? parent().at().calcPer() : mProcPer; }

void LWidget::setCalcLang( const string &ilng )	{ cfg("PROC").setS(ilng.empty() ? "" : ilng+"\n"+calcProg()); }

void LWidget::setCalcProgTr( bool vl )
{
    if(!proc().size() && !parent().freeStat())	parent().at().setCalcProgTr(vl);
    else cfg("PR_TR") = vl;
}

void LWidget::setCalcProg( const string &iprg )	{ cfg("PROC").setS(calcLang()+"\n"+iprg); }

void LWidget::setParentNm( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
}

void LWidget::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

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
		} catch(TError &err) { }
	}
	mParentNmPrev = parentNm();
    }
}

void LWidget::load_( TConfig *icfg )
{
    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

    if(!SYS->chkSelDB(ownerLib().DB())) throw TError();

    //Load generic widget's data
    string db  = ownerLib().DB();
    string tbl = ownerLib().tbl();

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
    mod->attrsLoad(*this, db+"."+tbl, id(), "", tAttrs, true);

    //Load all other attributes
    loadIO();
}

void LWidget::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerLib().DB()+"."+ownerLib().tbl(), id(), "", cfg("ATTRS").getS());

    //Force Active, not inherited and not modified attributes, mostly for init the primitives ones.
    /*vector<string> aLs;
    map<string, bool> aPrc;
    for(bool act = true; parent().freeStat() && act; ) {
	act = false;
	attrList(aLs);
	for(unsigned iA = 0; iA < aLs.size(); ++iA) {
	    AutoHD<Attr> aO = attrAt(aLs[iA]);
	    if((aO.at().flgGlob()&Attr::Active) && !aO.at().modif() && !aPrc[aLs[iA]]) {
		aO.at().set(aO.at().get(true), true); aO.at().setModif(0);
		aPrc[aLs[iA]] = act = true;
	    }
	}
    }*/

    //Load cotainer widgets
    if(!isContainer()) return;
    map<string, bool>   itReg;
    vector<vector<string> > full;
    TConfig cEl(&mod->elInclWdg());
    string db  = ownerLib().DB();
    string tbl = ownerLib().tbl()+"_incl";
    cEl.cfg("IDW").setS(id(),true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fldCnt++,cEl,false,&full); ) {
	string sid  = cEl.cfg("ID").getS();
	if(cEl.cfg("PARENT").getS() == "<deleted>") {
	    if(wdgPresent(sid))	wdgDel(sid);
	    continue;
	}
	if(!wdgPresent(sid))
	    try{ wdgAdd(sid,"",""); }
	    catch(TError &err) { mess_err(err.cat.c_str(),err.mess.c_str()); }

	wdgAt(sid).at().load(&cEl);
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
    if(!isContainer())	throw TError(nodePath().c_str(),_("Widget is not container!"));
    if(wdgPresent(wid))	return;

    bool toRestoreInher = false;

    //Check for label <deleted>
    if(!force) {
	string db  = ownerLib().DB();
	string tbl = ownerLib().tbl()+"_incl";
	TConfig cEl( &mod->elInclWdg() );
	cEl.cfg("IDW").setS(id());
	cEl.cfg("ID").setS(wid);
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,cEl,false,true) && cEl.cfg("PARENT").getS() == "<deleted>") {
	    SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, true, false, true);
	    toRestoreInher = parent().at().wdgPresent(wid);
	}
    }

    //Same widget addition or restoring
    if(toRestoreInher) {
	inheritIncl(wid);
	wdgAt(wid).at().setEnable(true);
    }
    else {
	chldAdd(inclWdg, new CWidget(wid,path));
	wdgAt(wid).at().setName(name);
    }

    //Call heritors include widgets update
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().inheritIncl(wid);

    if(toRestoreInher)	throw TError("warning", _("Restoring '%s' from the base container!"), wid.c_str());
}

AutoHD<CWidget> LWidget::wdgAt( const string &wdg ) const	{ return Widget::wdgAt(wdg); }

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

bool LWidget::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PR_TR") cfg("PROC").setNoTransl(!calcProgTr());
    modif();
    return true;
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
    mLnk = true;
    setParentNm(isrcwdg);
}

CWidget::~CWidget( )
{

}

string CWidget::path( ) const	{ return "/wlb_"+ownerLWdg().ownerLib().id()+"/wdg_"+ownerLWdg().id()+"/wdg_"+id(); }

LWidget &CWidget::ownerLWdg( ) const	{ return *(LWidget*)nodePrev(); }

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

	TConfig cEl(&mod->elWdgIO());
	cEl.cfg("IDW").setS(ownerLWdg().id(), true); cEl.cfg("IDC").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", cEl);
	cEl.setElem(&mod->elWdgUIO());
	cEl.cfg("IDW").setS(ownerLWdg().id(), true); cEl.cfg("IDC").setS(id(), true);
	SYS->db().at().dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", cEl);
    }
}

string CWidget::ico( ) const	{ return parent().freeStat() ? "" : parent().at().ico(); }

void CWidget::setParentNm( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
    modif();
}

void CWidget::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    Widget::setEnable(val);

    //Enable heritors widgets
    if(val)
	for(unsigned iH = 0; iH < ownerLWdg().herit().size(); iH++)
	    if(!ownerLWdg().herit()[iH].at().wdgAt(id()).at().enable() && ownerLWdg().herit()[iH].at().wdgPresent(id()))
		try { ownerLWdg().herit()[iH].at().wdgAt(id()).at().setEnable(true); }
		catch(...) { mess_err(nodePath().c_str(),_("Inheriting widget '%s' enable error."),id().c_str()); }
}

string CWidget::calcId( )	{ return parent().freeStat() ? "" : parent().at().calcId(); }

string CWidget::calcLang( ) const	{ return parent().freeStat() ? "" : parent().at().calcLang(); }

string CWidget::calcProg( ) const	{ return parent().freeStat() ? "" : parent().at().calcProg(); }

string CWidget::calcProgStors( const string &attr ){ return parent().freeStat() ? "" : parent().at().calcProgStors(attr); }

int CWidget::calcPer( ) const	{ return parent().freeStat() ? 0 : parent().at().calcPer(); }

void CWidget::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(ownerLWdg().ownerLib().DB())) throw TError();

    //Load generic widget's data
    string db  = ownerLWdg().ownerLib().DB();
    string tbl = ownerLWdg().ownerLib().tbl()+"_incl";

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,*this);

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

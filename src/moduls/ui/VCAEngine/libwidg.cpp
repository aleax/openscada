
//OpenSCADA module UI.VCAEngine file: libwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2021 by Roman Savochenko, <roman@oscada.org>       *
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
#include <algorithm>

#include "vcaengine.h"
#include "libwidg.h"

using namespace VCA;

//************************************************
//* WidgetLib: Widgets library                   *
//************************************************
WidgetLib::WidgetLib( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elWdgLib()), mId(cfg("ID")), mDB(lib_db), mEnable(false), passAutoEn(false)
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
    setDB(src_n->DB());
    mDB_MimeSrc = src_n->fullDB();

    if(!src_n->enable()) return *this;
    if(!enable()) setEnable(true);

    // Copying the included widgets
    vector<string> pls;
    src_n->list(pls);
    for(unsigned iP = 0; iP < pls.size(); iP++) {
	if(!present(pls[iP])) add(pls[iP], "");
	(TCntrNode&)at(pls[iP]).at() = (TCntrNode&)src_n->at(pls[iP]).at();
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
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	//Delete libraries record
	SYS->db().at().dataDel(DB(flag&NodeRemoveOnlyStor)+"."+mod->wlbTable(),mod->nodePath()+"LIB", *this, TBDS::UseAllKeys);

	//Delete function's files
	SYS->db().at().dataDelTbl(fullDB(flag&NodeRemoveOnlyStor), mod->nodePath()+tbl());
	SYS->db().at().dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_io", mod->nodePath()+tbl()+"_io");
	SYS->db().at().dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_uio", mod->nodePath()+tbl()+"_uio");
	SYS->db().at().dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_incl", mod->nodePath()+tbl()+"_incl");
	SYS->db().at().dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_mime", mod->nodePath()+tbl()+"_mime");

	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
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

string WidgetLib::getStatus( )
{
    string rez = enable() ? _("Enabled. ") : _("Disabled. ");

    vector<string> tls;
    list(tls);
    int cnt = 0;
    time_t maxTm = 0;
    for(unsigned iT = 0; iT < tls.size(); iT++) {
	cnt += at(tls[iT]).at().herit().size();
	maxTm = vmax(maxTm, at(tls[iT]).at().timeStamp());
    }
    rez += TSYS::strMess(_("Used: %d. "), cnt);
    rez += TSYS::strMess(_("Date of modification: %s. "), atm2s(maxTm).c_str());

    return rez;
}

void WidgetLib::setFullDB( const string &vl )
{
    int off = vl.size();
    cfg("DB_TBL").setS(TSYS::strParseEnd(vl,0,".",&off));
    setDB(vl.substr(0,off+1));
}

void WidgetLib::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    mess_debug(nodePath().c_str(),_("Loading widgets library."));

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(DB()+"."+mod->wlbTable(), mod->nodePath()+"LIB", *this);

    passAutoEn = true;

    //Create new widgets
    map<string, bool>	itReg;
    TConfig cEl(&mod->elWdg());
    //cEl.cfgViewAll(false);
    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl(),fldCnt++,cEl,TBDS::UseCache); ) {
	string fId = cEl.cfg("ID").getS();
	if(!present(fId)) { add(fId, "", ""); at(fId).at().setEnableByNeed(); }
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

    passAutoEn = false;
}

void WidgetLib::save_( )
{
    mess_debug(nodePath().c_str(),_("Saving widgets library."));

    SYS->db().at().dataSet(DB()+"."+mod->wlbTable(), mod->nodePath()+"LIB", *this);

    //Mime data copy
    if(mDB_MimeSrc.size() || DB(true).size()) {
	if(mDB_MimeSrc.empty()) mDB_MimeSrc = DB(true);
	vector<string> pls;
	mimeDataList(pls, mDB_MimeSrc);
	string mimeType, mimeData;
	for(unsigned iM = 0; iM < pls.size(); iM++) {
	    mimeDataGet(pls[iM], mimeType, &mimeData, mDB_MimeSrc);
	    mimeDataSet(pls[iM], mimeType, mimeData, DB());
	}
	mDB_MimeSrc = "";
    }

    setDB(DB(), true);
}

void WidgetLib::setEnable( bool val, bool force )
{
    if(val == enable())	return;

    mess_debug(nodePath().c_str(),val ? _("Enabling widgets library.") : _("Disabling widgets library."));

    passAutoEn = true;

    vector<string> f_lst;
    list(f_lst);
    for(unsigned i_ls = 0; i_ls < f_lst.size(); i_ls++) {
	if(at(f_lst[i_ls]).at().enableByNeed)	continue;
	try { at(f_lst[i_ls]).at().setEnable(val); }
	catch(TError &err) { mess_err(nodePath().c_str(),_("Error enabling/disabling widget '%s': %s."),f_lst[i_ls].c_str(),err.mess.c_str()); }
    }

    passAutoEn = false;

    mEnable = val;
}

void WidgetLib::mimeDataList( vector<string> &list, const string &idb ) const
{
    string wdb = DB(), wtbl;
    if(idb.size()) wdb = TBDS::dbPart(idb), wtbl = TBDS::dbPart(idb, true);
    wtbl = (wtbl.empty()?tbl():wtbl) + "_mime";

    TConfig cEl(&mod->elWdgData());
    cEl.cfgViewAll(false);

    list.clear();
    for(int fldCnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,mod->nodePath()+wtbl,fldCnt,cEl,TBDS::UseCache); fldCnt++)
	if(std::find(list.begin(),list.end(),cEl.cfg("ID").getS()) == list.end())
	    list.push_back(cEl.cfg("ID").getS());
}

bool WidgetLib::mimeDataGet( const string &iid, string &mimeType, string *mimeData, const string &idb, int off, int *size ) const
{
    bool is_file = (iid.compare(0,5,"file:")==0);
    bool is_res  = (iid.compare(0,4,"res:")==0);

    if(!is_file) {
	//Get resource file from DB
	string dbid = is_res ? iid.substr(4) : iid;

	string wdb = DB(), wtbl;
	if(idb.size()) wdb = TBDS::dbPart(idb), wtbl = TBDS::dbPart(idb, true);
	wtbl = (wtbl.empty()?tbl():wtbl) + "_mime";

	TConfig cEl(&mod->elWdgData());
	if(!mimeData) cEl.cfg("DATA").setView(false);
	cEl.cfg("ID").setS(dbid);
	if(SYS->db().at().dataGet(wdb+"."+wtbl,mod->nodePath()+wtbl,cEl,TBDS::NoException)) {
	    mimeType = cEl.cfg("MIME").getS();
	    if(mimeData) *mimeData = cEl.cfg("DATA").getS();
	    return true;
	}
    }
    if(!is_res) {
	//Get resource file from the file system
	string filepath = is_file ? iid.substr(5) : iid;
	char buf[prmStrBuf_SZ];

	mimeType = TUIS::mimeGet(filepath, "");
	if(!mimeData)	return true;
	*mimeData = "";

	int hd = open(filepath.c_str(), O_RDONLY);
	if(hd == -1)	return false;

	int f_size = lseek(hd, 0, SEEK_END);
	int partSz = f_size;

	if(off >= 0) {	//Partially reading
	    off = vmin(f_size, off);
	    partSz = vmin(limUserFile_SZ, vmin(f_size, off + ((size && *size)?*size:f_size)));
	    if(size) *size = f_size;
	    lseek(hd, off, SEEK_SET);
	} else lseek(hd, 0, SEEK_SET);

	for(int len = 0; (len=read(hd,buf,vmin(sizeof(buf),partSz-mimeData->size()))) > 0; )
	    mimeData->append(buf, len);

	close(hd);

	*mimeData = TSYS::strEncode(*mimeData, TSYS::base64);

	return true;
    }

    return false;
}

void WidgetLib::mimeDataSet( const string &iid, const string &mimeType, const string &mimeData, const string &idb )
{
    string wdb = DB(), wtbl;
    if(idb.size()) wdb = TBDS::dbPart(idb), wtbl = TBDS::dbPart(idb, true);
    wtbl = (wtbl.empty()?tbl():wtbl) + "_mime";

    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid);
    cEl.cfg("MIME").setS(mimeType);
    if(!mimeData.size()) cEl.cfg("DATA").setView(false);
    else cEl.cfg("DATA").setS(mimeData);
    SYS->db().at().dataSet(wdb+"."+wtbl, mod->nodePath()+wtbl, cEl, TBDS::NoException);
}

void WidgetLib::mimeDataDel( const string &iid, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid,true);
    SYS->db().at().dataDel(wdb+"."+wtbl, mod->nodePath()+wtbl, cEl, TBDS::NoException);
}

string WidgetLib::add( const string &iid, const string &name, const string &orig )
{
    if(present(iid))	throw err_sys(_("The widget '%s' is already present!"), iid.c_str());
	//return "";

    string id = chldAdd(mWdg, new LWidget(TSYS::strEncode(sTrm(iid),TSYS::oscdID),orig));
    at(id).at().setName(name);

    return id;
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
	XMLNode *nd = ctrMkNode("oscada_cntr",opt,-1,"/",_("Widgets library: ")+id(),RWRWR_,"root",SUI_ID);
	if(nd)	nd->setAttr("doc", TUIS::docKeyGet(descr()));
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ctrMkNode("area",opt,-1,"/obj",_("Library"))) {
	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/obj/st/status",_("Status"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enabled"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/db",_("Library DB"),RWRWR_,"root",SUI_ID,4,
		    "tp","str","dest","sel_ed","select",("/db/tblList:wlb_"+id()).c_str(),
		    "help",_("Storage address in the format \"{DB module}.{DB name}.{Table name}\".\nTo use the Generic Storage, set '*.*.{Table name}'."));
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/obj/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SUI_ID);
		ctrMkNode("fld",opt,-1,"/obj/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
		ctrMkNode("fld",opt,-1,"/obj/st/use",_("Used"),R_R_R_,"root",SUI_ID,1,"tp","dec");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/obj/cfg/id",_("Identifier"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,2,"tp","str","len",i2s(limObjNm_SZ).c_str());
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
    if(a_path == "/obj/st/status" && ctrChkNode(opt))		opt->setText(getStatus());
    else if(a_path == "/obj/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/obj/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(fullDB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setFullDB(opt->text());
    }
    else if(a_path == "/obj/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SUI_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/obj/st/timestamp" && ctrChkNode(opt)) {
	vector<string> tls;
	list(tls);
	time_t maxTm = 0;
	for(unsigned iT = 0; iT < tls.size(); iT++)
	    maxTm = vmax(maxTm, at(tls[iT]).at().timeStamp());
	opt->setText(i2s(maxTm));
    }
    else if(a_path == "/obj/st/use" && ctrChkNode(opt)) {
	vector<string> tls;
	list(tls);
	int cnt = 0;
	for(unsigned iT = 0; iT < tls.size(); iT++)
	    cnt += at(tls[iT]).at().herit().size();
	opt->setText(i2s(cnt));
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
	    for(unsigned iF = 0; iF < lst.size(); iF++)
		opt->childAdd("el")->setAttr("id",lst[iF])->setText(trLU(at(lst[iF]).at().name(),l,u));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) { opt->setAttr("id", add(opt->attr("id"),opt->text())); at(opt->attr("id")).at().setOwner(opt->attr("user")); }
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
		for(unsigned iEl = 0; iEl < lst.size(); iEl++)
		    if(mimeDataGet("res:"+lst[iEl],mimeType)) {
			if(n_id) n_id->childAdd("el")->setText(lst[iEl]);
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

TCntrNode &LWidget::operator=( const TCntrNode &node )
{
    Widget::operator=(node);

    //Removing the inherited but missed widgets on the source
    const LWidget *src_n = dynamic_cast<const LWidget*>(&node);
    if(src_n) {
	vector<string> wls;
	wdgList(wls);
	for(unsigned iW = 0; iW < wls.size(); iW++)
	    if(!src_n->wdgPresent(wls[iW])) wdgDel(wls[iW], true);
    }

    return *this;
}

WidgetLib &LWidget::ownerLib( ) const	{ return *(WidgetLib*)nodePrev(); }

void LWidget::postDisable( int flag )
{
    if(flag&NodeRemove) {
	string db  = ownerLib().DB();
	string tbl = ownerLib().tbl();

	//Remove from library table
	SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, *this, TBDS::UseAllKeys);

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

string LWidget::getStatus( )
{
    string rez = Widget::getStatus();
    rez += TSYS::strMess(_("Date of modification: %s. "), atm2s(timeStamp()).c_str());
    if(calcProg().size()) {
	rez += _("Calculating procedure: ");
	if(!parent().freeStat() && parent().at().calcProg().size() && calcProg() != parent().at().calcProg())
	    rez += _("!!redefined!!");
	else if(!parent().freeStat() && parent().at().calcProg().size())
	    rez += _("inherited");
	else rez += _("presented");
	rez += ". ";
    }

    return rez;
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

    return TSYS::strLine(proc(), 0);
}

string LWidget::calcProg( ) const
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcProg();

    string iprg = proc();
    int off = 0;
    TSYS::strLine(iprg, 0, &off);
    return iprg.substr(off);
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
    for(unsigned iA = 0; iA < als.size(); iA++) {
	if(!attrPresent(als[iA])) continue;
	AutoHD<Attr> attr = attrAt(als[iA]);
	if(attr.at().modif() && tAttrs.find(als[iA]+";") == string::npos) {
	    attr.at().setModif(0);
	    inheritAttr(als[iA]);
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
    TConfig cEl(&mod->elInclWdg());
    string db  = ownerLib().DB();
    string tbl = ownerLib().tbl()+"_incl";
    cEl.cfg("IDW").setS(id(),true);
    for(int fldCnt = 0; SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fldCnt++,cEl,TBDS::UseCache); ) {
	string sid  = cEl.cfg("ID").getS();
	if(cEl.cfg("PARENT").getS() == "<deleted>") {
	    if(wdgPresent(sid))	wdgDel(sid);
	    continue;
	}
	if(!wdgPresent(sid))
	    try{ wdgAdd(sid, "", ""); }
	    catch(TError &err) { mess_err(err.cat.c_str(),err.mess.c_str()); }

	wdgAt(sid).at().load(&cEl);
	itReg[sid] = true;
    }

    //Check for remove items removed from DB
    if(SYS->chkSelDB(SYS->selDB(),true)) {
	vector<string> itLs;
	wdgList(itLs);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    if(itReg.find(itLs[iIt]) == itReg.end())
		wdgDel(itLs[iIt]);
    }
}

void LWidget::save_( )
{
    if(enableByNeed)	return;

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
    if(wdgPresent(wid))	throw err_sys(_("The widget '%s' is already present!"), wid.c_str());
	//return;

    bool toRestoreInher = false;

    //Check for label <deleted>
    if(!force) {
	string db  = ownerLib().DB();
	string tbl = ownerLib().tbl()+"_incl";
	TConfig cEl( &mod->elInclWdg() );
	cEl.cfg("IDW").setS(id());
	cEl.cfg("ID").setS(wid);
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,cEl,TBDS::NoException) && cEl.cfg("PARENT").getS() == "<deleted>") {
	    SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, TBDS::UseAllKeys|TBDS::NoException);
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
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().inheritIncl(wid);
    res.unlock();

    if(toRestoreInher)
	throw TError(TError::Core_CntrWarning, nodePath().c_str(), _("Restoring '%s' from the base container!"), wid.c_str());
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

string LWidget::resourceGet( const string &id, string *mime, int off, int *size )
{
    string mimeType, mimeData;

    if(!ownerLib().mimeDataGet(id,mimeType,&mimeData,"",off,size) && !parent().freeStat())
	mimeData = parent().at().resourceGet(id, &mimeType, off, size);
    if(mime) *mime = mimeType;

    return mimeData;
}

void LWidget::procChange( bool src )
{
    if(!src && proc().size()) return;

    //Update heritors' procedures
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().procChange(false);
}

void LWidget::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf)	modifClr();
}

bool LWidget::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PR_TR") cfg("PROC").setNoTransl(!calcProgTr());

    if(co.getS() == pc.getS()) return true;
    if(co.name() == "PROC") procChange();
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
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Library widget: ")+id(),RWRWR_,"root",SUI_ID,1,"doc", "User_API|Documents/User_API");
	return;
    }
    if(cntrCmdGeneric(opt) || cntrCmdAttributes(opt) || cntrCmdLinks(opt) || cntrCmdProcess(opt)) ;
    else if(opt->attr("path") == "/wdg/st/timestamp" && ctrChkNode(opt)) opt->setText(i2s(timeStamp()));
    else TCntrNode::cntrCmdProc(opt);
}

TVariant LWidget::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    //To register the property on the project side
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; !wr && iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().stlReq(a, vl, wr);

    return vl;
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

TCntrNode &CWidget::operator=( const TCntrNode &node )
{
    Widget::operator=(node);

    if(attrPresent("geomX") && ownerLWdg().attrPresent("geomW"))
	attrAt("geomX").at().setR(fmax(0,
	    fmin(ownerLWdg().attrAt("geomW").at().getR()-attrAt("geomW").at().getR()*attrAt("geomXsc").at().getR(),
		attrAt("geomX").at().getR())));
    if(attrPresent("geomY") && ownerLWdg().attrPresent("geomH"))
	attrAt("geomY").at().setR(fmax(0,
	    fmin(ownerLWdg().attrAt("geomH").at().getR()-attrAt("geomH").at().getR()*attrAt("geomYsc").at().getR(),
		attrAt("geomY").at().getR())));

    return *this;
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
    if(flag&NodeRemove) {
	string db  = ownerLWdg().ownerLib().DB();
	string tbl = ownerLWdg().ownerLib().tbl();

	//Remove from library table
	if(ChldResrv) {
	    cfg("PARENT").setS("<deleted>");
	    SYS->db().at().dataSet(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this);
	}
	else SYS->db().at().dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this, TBDS::UseAllKeys);

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
		catch(...) { mess_err(nodePath().c_str(),_("Error enabling the inheriting widget '%s'."),id().c_str()); }
}

string CWidget::calcId( )	{ return parent().freeStat() ? "" : parent().at().calcId(); }

string CWidget::calcLang( ) const	{ return parent().freeStat() ? "" : parent().at().calcLang(); }

string CWidget::calcProg( ) const	{ return parent().freeStat() ? "" : parent().at().calcProg(); }

string CWidget::calcProgStors( const string &attr )
{
    string rez = parent().freeStat() ? "" : parent().at().calcProgStors(attr);
    if(attr.size() && attrAt(attr).at().modif() && rez.find(ownerLWdg().ownerLib().DB()) == string::npos)
	rez = ownerLWdg().ownerLib().DB() + ";" + rez;

    return rez;
}

int CWidget::calcPer( ) const	{ return parent().freeStat() ? 0 : parent().at().calcPer(); }

void CWidget::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(ownerLWdg().ownerLib().DB())) throw TError();

    //Load generic widget's data
    string db  = ownerLWdg().ownerLib().DB();
    string tbl = ownerLWdg().ownerLib().tbl()+"_incl";

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(db+"."+tbl, mod->nodePath()+tbl, *this);

    //Inherit modify attributes
    vector<string> als;
    attrList(als);
    string tAttrs = cfg("ATTRS").getS();
    for(unsigned iA = 0; iA < als.size(); iA++) {
	if(!attrPresent(als[iA])) continue;
	AutoHD<Attr> attr = attrAt(als[iA]);
	if(attr.at().modif() && tAttrs.find(als[iA]+";") == string::npos) {
	    attr.at().setModif(0);
	    inheritAttr(als[iA]);
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
    SYS->db().at().dataSet(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this);

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

string CWidget::resourceGet( const string &id, string *mime, int off, int *size )
{
    string mimeType, mimeData;

    if((mimeData=ownerLWdg().resourceGet(id,&mimeType,off,size)).empty() && !parent().freeStat())
	mimeData = parent().at().resourceGet(id, &mimeType, off, size);
    if(mime) *mime = mimeType;

    return mimeData;
}

void CWidget::procChange( bool src )
{
    //Update heritors' procedures
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().procChange(false);
}

void CWidget::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf) modifClr( );
}

void CWidget::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	ctrMkNode("oscada_cntr", opt, -1, "/", TSYS::strMess(_("Link to the widget '%s'."), id().c_str()).c_str(), RWRWR_, "root", SUI_ID);
	return;
    }
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt)))
	TCntrNode::cntrCmdProc(opt);
}

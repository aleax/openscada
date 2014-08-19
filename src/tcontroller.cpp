
//OpenSCADA system file: tcontroller.cpp
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

#include "tsys.h"
#include "tmess.h"
#include "ttiparam.h"
#include "tcontroller.h"

using namespace OSCADA;

//*************************************************
//* TController					  *
//*************************************************
TController::TController( const string &id_c, const string &daq_db, TElem *cfgelem ) :
    TConfig(cfgelem), en_st(false), run_st(false),
    mId(cfg("ID")), mMessLev(cfg("MESS_LEV")), mAEn(cfg("ENABLE").getBd()), mAStart(cfg("START").getBd()),
    mDB(daq_db), mRedntUse(false)
{
    mId = id_c;
    mPrm = grpAdd("prm_");
}

TController::~TController( )
{
    nodeDelAll();
}

string TController::objName( )	{ return TCntrNode::objName()+":TController"; }

string TController::DAQPath( )	{ return owner().DAQPath()+"."+id(); }

TCntrNode &TController::operator=( TCntrNode &node )
{
    TController *src_n = dynamic_cast<TController*>(&node);
    if(!src_n) return *this;

    //Individual DB names store
    vector<string> dbNms;
    for(unsigned i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++)
	dbNms.push_back(owner().tpPrmAt(i_tp).DB(this));

    //Configuration copy
    exclCopy(*src_n, "ID;");
    setDB(src_n->mDB);

    //Individual DB names restore
    for(unsigned i_tp = 0; i_tp < owner().tpPrmSize() && i_tp < dbNms.size(); i_tp++)
	owner().tpPrmAt(i_tp).setDB(this, dbNms[i_tp]);

    //Parameters copy
    if(src_n->enableStat()) {
	if(!enableStat()) enable();
	vector<string> prm_ls;
	src_n->list(prm_ls);
	for(unsigned i_p = 0; i_p < prm_ls.size(); i_p++) {
	    if(!owner().tpPrmPresent(src_n->at(prm_ls[i_p]).at().type().name)) continue;
	    if(!present(prm_ls[i_p])) add(prm_ls[i_p], owner().tpPrmToId(src_n->at(prm_ls[i_p]).at().type().name));
	    (TCntrNode&)at(prm_ls[i_p]).at() = (TCntrNode&)src_n->at(prm_ls[i_p]).at();
	    //if(toEnable() && !enableStat()) enable();
	}
    }

    return *this;
}

void TController::preDisable(int flag)
{
    if(startStat())	stop();
    if(enableStat())	disable();
}

void TController::postDisable( int flag )
{
    if(flag) {
	//Delete DB record
	SYS->db().at().dataDel(fullDB(),owner().nodePath()+"DAQ",*this,true);

	//Delete parameter's tables
	for(unsigned i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++) {
	    string tbl = DB()+"."+owner().tpPrmAt(i_tp).DB(this);
	    SYS->db().at().open(tbl);
	    SYS->db().at().close(tbl,true);
	}
    }
}

TTipDAQ &TController::owner( )	{ return *(TTipDAQ*)nodePrev(); }

string TController::workId( )	{ return owner().modId()+"."+id(); }

string TController::name( )	{ string nm = cfg("NAME").getS(); return nm.empty() ? id() : nm; }

void TController::setName( const string &nm )		{ cfg("NAME").setS(nm);  }

string TController::descr( )	{ return cfg("DESCR").getS(); }

void TController::setDescr( const string &dscr )	{ cfg("DESCR").setS(dscr); }

string TController::tbl( )	{ return owner().owner().subId()+"_"+owner().modId(); }

string TController::getStatus( )
{
    string rez, mess;
    if(startStat()) {
	rez = string("0:")+_("Started. ");
	if(owner().redntAllow() && redntUse()) {
	    mess = _("Getting data from remote station: ");
	    string rSt = mRedntSt.getVal();
	    if(!rSt.empty()) {
		if(rSt.find(mess) == string::npos) {
		    int rOff = 0;
		    rez.replace(0,1,TSYS::strSepParse(rSt,0,':',&rOff));
		    mess.append(rSt.substr(rOff));
		}
		else mess = _("Your redundancy configuration wrong and controller often enable-disable redundancy!");
	    }
	    rez += mess;
	}
    }
    else if(enableStat()) rez = string("1:")+_("Enabled. ");
    else rez = string("2:")+_("Disabled. ");

    return rez;
}

void TController::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    mess_info(nodePath().c_str(),_("Load controller's configurations!"));

    bool en_st_prev = en_st, run_st_prev = run_st;

    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+"DAQ",*this);

    LoadParmCfg();

    if(!en_st && en_st_prev) enable();
    if(!run_st && run_st_prev) start();
}

void TController::save_( )
{
    mess_info(nodePath().c_str(),_("Save controller's configurations!"));

    //Update type controller bd record
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+"DAQ",*this);
}

void TController::start( )
{
    //Enable if no enabled
    if(run_st)	return;
    if(!en_st)	enable();

    mess_info(nodePath().c_str(),_("Start controller!"));

    //First archives synchronization
    if(owner().redntAllow() && redntMode()) redntDataUpdate();

    //Start for children
    start_();

    run_st = true;
}

void TController::stop( )
{
    if(!run_st)	return;

    mess_info(nodePath().c_str(),_("Stop controller!"));

    //Stop for children
    stop_();

    run_st = false;
}

void TController::enable( )
{
    if(!en_st) {
	mess_info(nodePath().c_str(),_("Enable controller!"));

	//Enable for children
	enable_();

	mRedntUse = owner().redntAllow() && (bool)redntMode();
    }

    bool enErr = false;
    //Enable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned i_prm = 0; i_prm < prm_list.size(); i_prm++)
	if(at(prm_list[i_prm]).at().toEnable())
	    try{ at(prm_list[i_prm]).at().enable(); }
	    catch(TError err) {
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Enable parameter '%s' error."),prm_list[i_prm].c_str());
		enErr = true;
	    }

    //Set enable stat flag
    en_st = true;

    if(enErr) throw TError(nodePath().c_str(),_("Some parameters enable error."));
}

void TController::disable( )
{
    if(!en_st) return;

    //Stop if runed
    if(run_st) stop();

    mess_info(nodePath().c_str(),_("Disable controller!"));

    //Disable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned i_prm = 0; i_prm < prm_list.size(); i_prm++)
	if(at(prm_list[i_prm]).at().enableStat())
	    try{ at(prm_list[i_prm]).at().disable(); }
	    catch(TError err) {
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Disable parameter '%s' error."),prm_list[i_prm].c_str());
	    }

    //Disable for children
    disable_();

    //Clear enable flag
    en_st = false;
}

void TController::LoadParmCfg( )
{
    map<string, bool>	itReg;

    //Search and create new parameters
    for(unsigned i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++) {
	if(owner().tpPrmAt(i_tp).DB(this).empty()) continue;
	try {
	    TConfig c_el(&owner().tpPrmAt(i_tp));
	    c_el.cfgViewAll(false);
	    c_el.cfg("OWNER").setS("", TCfg::ForceUse);

	    // Search new into DB and Config-file
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(DB()+"."+owner().tpPrmAt(i_tp).DB(this),
					   owner().nodePath()+owner().tpPrmAt(i_tp).DB(this),fld_cnt++,c_el); )
	    {
		try {
		    string shfr = c_el.cfg("SHIFR").getS();
		    if(!present(shfr))	add(shfr, i_tp);
		    itReg[shfr] = true;
		}
		catch(TError err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Add parameter '%s' error."),c_el.cfg("SHIFR").getS().c_str());
		}
	    }
	}
	catch(TError err) {
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Search and create new parameters error."));
	}
    }

    //Check for remove items removed from DB
    if(!SYS->selDB().empty()) {
	vector<string> it_ls;
	list(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		del(it_ls[i_it]);
    }

    //Force load present parameters
    vector<string> prm_ls;
    list(prm_ls);
    for(unsigned i_p = 0; i_p < prm_ls.size(); i_p++) {
	at(prm_ls[i_p]).at().modifG();
	at(prm_ls[i_p]).at().load();
    }
}

void TController::add( const string &name, unsigned type )	{ chldAdd(mPrm, ParamAttach(name,type)); }

TParamContr *TController::ParamAttach( const string &name, int type)	{ return new TParamContr(name, &owner().tpPrmAt(type)); }

TController::Redundant TController::redntMode( )	{ return (TController::Redundant)cfg("REDNT").getI(); }

void TController::setRedntMode( Redundant vl )		{ cfg("REDNT").setI(vl); modif(); }

string TController::redntRun( )				{ return cfg("REDNT_RUN").getS(); }

void TController::setRedntRun( const string &vl )	{ cfg("REDNT_RUN").setS(vl); modif(); }

void TController::setRedntUse( bool vl )
{
    if(mRedntUse == vl) return;
    mRedntUse = vl;
}

void TController::redntDataUpdate( )
{
    vector<string> pls;
    list(pls);

    //Prepare group request to parameters
    AutoHD<TParamContr> prm;
    XMLNode req("CntrReqs"); req.setAttr("path",nodePath(0,true));
    req.childAdd("get")->setAttr("path","/%2fcntr%2fst%2fstatus");
    for(int i_p = 0; i_p < (int)pls.size(); i_p++) {
	prm = at(pls[i_p]);
	if(!prm.at().enableStat()) { pls.erase(pls.begin()+i_p); i_p--; continue; }

	XMLNode *prmNd = req.childAdd("get")->setAttr("path","/prm_"+pls[i_p]+"/%2fserv%2fattr");

	// Prepare individual attributes list
	prmNd->setAttr("sepReq", "1");

	// Check attributes last present data time into archives
	vector<string> listV;
	prm.at().vlList(listV);
	unsigned rC = 0;
	for(unsigned iV = 0; iV < listV.size(); iV++) {
	    AutoHD<TVal> vl = prm.at().vlAt(listV[iV]);
	    if(!vl.at().arch().freeStat() || vl.at().reqFlg()) { prmNd->childAdd("el")->setAttr("id",listV[iV]); rC++; }
	    if(!vl.at().arch().freeStat())
		prmNd->childAdd("ael")->setAttr("id",listV[iV])->setAttr("tm",ll2s(vmax(vl.at().arch().at().end(""),
						    TSYS::curTime()-(int64_t)(3.6e9*owner().owner().rdRestDtTm()))));
	}
	if(rC > listV.size()/2) {
	    prmNd->childClear("el");
	    prmNd->setAttr("sepReq", "0");
	}
    }

    //Send request to first active station for this controller
    if(owner().owner().rdStRequest(workId(),req).empty()) return;

    //Write data to parameters
    if(req.childSize()) mRedntSt.setVal(req.childGet(0)->text());
    for(unsigned i_p = 0; i_p < pls.size(); i_p++) {
	prm = at(pls[i_p]);
	for(unsigned i_a = 0; i_a < req.childGet(i_p+1)->childSize(); i_a++) {
	    XMLNode *aNd = req.childGet(i_p+1)->childGet(i_a);
	    if(!prm.at().vlPresent(aNd->attr("id"))) continue;
	    AutoHD<TVal> vl = prm.at().vlAt(aNd->attr("id"));

	    if(aNd->name() == "el")
	    { vl.at().setS(aNd->text(),atoll(aNd->attr("tm").c_str()),true); vl.at().setReqFlg(false); }
	    else if(aNd->name() == "ael" && !vl.at().arch().freeStat() && aNd->childSize())
	    {
		int64_t btm = atoll(aNd->attr("tm").c_str());
		int64_t per = atoll(aNd->attr("per").c_str());
		TValBuf buf(vl.at().arch().at().valType(),0,per,false,true);
		for(unsigned i_v = 0; i_v < aNd->childSize(); i_v++)
		    buf.setS(aNd->childGet(i_v)->text(),btm+per*i_v);
		vl.at().arch().at().setVals(buf,buf.begin(),buf.end(),"");
	    }
	}
    }
}

string TController::catsPat( )
{
    return "^al"+owner().modId()+":"+id()+"(\\.|$)|^"+nodePath();
    //return "/^(al"+owner().modId()+":"+id()+"(\\.|$)|"+nodePath()+")/";
}

void TController::alarmSet( const string &mess, int lev, const string &prm )
{
    message(("al"+owner().modId()+":"+id()+(prm.size()?("."+prm):"")).c_str(), lev, mess.c_str());
}

TVariant TController::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // string name() - get controller name.
    if(iid == "name")	return name();
    // string descr() - get controller description.
    if(iid == "descr")	return descr();
    // string status() - get controller status.
    if(iid == "status")	return getStatus();
    // bool alarmSet(string mess, int lev = -5, string prm = "") - set alarm to message <mess> and level <lev> for parameter <prm>.
    if(iid == "alarmSet" && prms.size() >= 1) {
	alarmSet(prms[0].getS(), (prms.size() >= 2) ? prms[1].getI() : -TMess::Crit, (prms.size() >= 3) ? prms[2].getS() : "");
	return true;
    }
    // bool enable(bool newSt = EVAL) - get enable status or change it by argument 'newSt' assign.
    if(iid == "enable") {
	if(prms.size())	{ prms[0].getB() ? enable() : disable(); }
	return enableStat();
    }
    // bool start(bool newSt = EVAL) - get start status or change it by argument 'newSt' assign.
    if(iid == "start") {
	if(prms.size())	{ prms[0].getB() ? start() : stop(); }
	return startStat();
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TController::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/mess" && ctrChkNode(opt,"get")) {
	vector<TMess::SRec> rez;
	time_t	tm	= strtoul(opt->attr("tm").c_str(), 0, 10);
	if(!tm)	{ tm = time(NULL)-1; opt->setAttr("tm", i2s(tm+1)); }	//-1 for waranty all curent date get without doubles
	time_t	tm_grnd	= strtoul(opt->attr("tm_grnd").c_str(), 0, 10);
	int	lev	= s2i(opt->attr("lev"));
	SYS->archive().at().messGet(tm_grnd, tm, rez, "/("+catsPat()+")/", lev, "");
	for(unsigned i_r = 0; i_r < rez.size(); i_r++)
	    opt->childAdd("el")->
		setAttr("time", u2s(rez[i_r].time))->
		setAttr("utime", u2s(rez[i_r].utime))->
		setAttr("cat", rez[i_r].categ)->
		setAttr("lev", i2s(rez[i_r].level))->
		setText(rez[i_r].mess);
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Controller: ")+name(),RWRWR_,"root",SDAQ_ID);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/cntr",_("Controller"))) {
	    if(ctrMkNode("area",opt,-1,"/cntr/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/cntr/st/status",_("Status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cntr/st/en_st",_("Enable"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/cntr/st/run_st",_("Run"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/cntr/st/db",_("Controller DB"),RWRWR_,"root",SDAQ_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/cntr/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/cntr/cfg",0,"root",SDAQ_ID,RWRWR_);
		ctrRemoveNode(opt,"/cntr/cfg/MESS_LEV");
		ctrRemoveNode(opt,"/cntr/cfg/REDNT");
		ctrRemoveNode(opt,"/cntr/cfg/REDNT_RUN");
	    }
	}
	if(owner().tpPrmSize()) {
	    ctrMkNode("grp",opt,-1,"/br/prm_",_("Parameter"),RWRWR_,"root",SDAQ_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	    if(ctrMkNode("area",opt,-1,"/prm",_("Parameters"))) {
		if(owner().tpPrmSize() > 1)
		    ctrMkNode("fld",opt,-1,"/prm/t_prm",_("To add parameters"),RWRW__,"root",SDAQ_ID,3,"tp","str","dest","select","select","/prm/t_lst");
		ctrMkNode("fld",opt,-1,"/prm/nmb",_("Number"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/prm",_("Parameters"),RWRWR_,"root",SDAQ_ID,5,"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","prm_","idSz",OBJ_ID_SZ);
	    }
	}
	if(ctrMkNode("area",opt,-1,"/mess",_("Diagnostics"))) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time"),RWRW__,"root",SDAQ_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size",_("Size (s)"),RWRW__,"root",SDAQ_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/lvl",_("Level"),RWRW__,"root",SDAQ_ID,5,"tp","dec","dest","select",
		"sel_id","0;1;2;3;4;5;6;7",
		"sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)"),
		"help",_("Get messages for level more and equal it.\n"
			 "Also affect to specific-diagnostic messages generation by data source."));
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SDAQ_ID)) {
		ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),R_R___,"root",SDAQ_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/mess/mess/0a",_("mcsec"),R_R___,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),R_R___,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mess/mess/2",_("Lev."),R_R___,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/3",_("Message"),R_R___,"root",SDAQ_ID,1,"tp","str");
	    }
	}
	return;
    }

    //Process command to page
    vector<string> c_list;

    if(a_path == "/cntr/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/prm/nmb" && ctrChkNode(opt)) {
	list(c_list);
	unsigned e_c = 0;
	for(unsigned i_a = 0; i_a < c_list.size(); i_a++)
	    if(at(c_list[i_a]).at().enableStat()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else if(a_path == "/prm/t_prm" && owner().tpPrmSize()) {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(owner().nodePath()+"addType",owner().tpPrmAt(0).name,opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR) )
	    TBDS::genDBSet(owner().nodePath()+"addType",opt->text(),opt->attr("user"));
    }
    else if((a_path == "/br/prm_" || a_path == "/prm/prm") && owner().tpPrmSize()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    list(c_list);
	    for(unsigned i_a = 0; i_a < c_list.size(); i_a++) {
		XMLNode *cN = opt->childAdd("el")->setAttr("id",c_list[i_a])->setText(at(c_list[i_a]).at().name());
		if(!s2i(opt->attr("recurs"))) continue;
		cN->setName(opt->name())->setAttr("path",TSYS::strEncode(opt->attr("path"),TSYS::PathEl))->setAttr("recurs","1");
		at(c_list[i_a]).at().cntrCmd(cN);
		cN->setName("el")->setAttr("path","")->setAttr("rez","")->setAttr("recurs","")->setText("");
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    add(vid,owner().tpPrmToId(TBDS::genDBGet(owner().nodePath()+"addType",owner().tpPrmAt(0).name,opt->attr("user"))));
	    at(vid).at().setName(opt->text());
	    opt->setAttr("id", vid);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	del(opt->attr("id"),true);
    }
    else if(a_path == "/prm/t_lst" && owner().tpPrmSize() && ctrChkNode(opt,"get",R_R_R_)) {
	for(unsigned i_a=0; i_a < owner().tpPrmSize(); i_a++)
	    opt->childAdd("el")->setAttr("id",owner().tpPrmAt(i_a).name)->setText(owner().tpPrmAt(i_a).descr);
    }
    else if(a_path == "/cntr/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/cntr/st/en_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(en_st?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	s2i(opt->text()) ? enable() : disable();
    }
    else if(a_path == "/cntr/st/run_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(run_st?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	s2i(opt->text()) ? start() : stop();
    }
    else if(a_path.substr(0,9) == "/cntr/cfg") {
	TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SDAQ_ID, RWRWR_);
	if(ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEC_WR))
	    for(unsigned i_t = 0; i_t < owner().tpPrmSize(); i_t++)
		if(owner().tpPrmAt(i_t).mDB == TSYS::pathLev(a_path,2))
		{ modifG(); break; }
    }
    else if(a_path == "/mess/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) {
	    opt->setText(TBDS::genDBGet(SYS->daq().at().nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text())) opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	    TBDS::genDBSet(SYS->daq().at().nodePath()+"messTm",(s2i(opt->text())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(SYS->daq().at().nodePath()+"messSize","60",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    TBDS::genDBSet(SYS->daq().at().nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/lvl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(mMessLev.getS());
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	mMessLev = opt->text();
    }
    else if(a_path == "/mess/mess" && ctrChkNode(opt,"get",R_R___,"root",SDAQ_ID)) {
	vector<TMess::SRec> rec;
	time_t gtm = s2i(TBDS::genDBGet(SYS->daq().at().nodePath()+"messTm","0",opt->attr("user")));
	if(!gtm) gtm = time(NULL);
	int gsz = s2i(TBDS::genDBGet(SYS->daq().at().nodePath()+"messSize","60",opt->attr("user")));
	SYS->archive().at().messGet(gtm-gsz, gtm, rec, "/("+catsPat()+")/", messLev(), "");

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SDAQ_ID);
	XMLNode *n_tmu  = ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SDAQ_ID);
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SDAQ_ID);
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SDAQ_ID);
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SDAQ_ID);
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

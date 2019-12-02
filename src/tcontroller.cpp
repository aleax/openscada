
//OpenSCADA file: tcontroller.cpp
/***************************************************************************
 *   Copyright (C) 2003-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include "ttypeparam.h"
#include "tcontroller.h"

using namespace OSCADA;

//*************************************************
//* TController					  *
//*************************************************
TController::TController( const string &id_c, const string &daq_db, TElem *cfgelem ) :
    TConfig(cfgelem), enSt(false), runSt(false),
    mId(cfg("ID")), mMessLev(cfg("MESS_LEV")), mAEn(cfg("ENABLE").getBd()), mAStart(cfg("START").getBd()),
    mDB(daq_db), mRdSt(dataRes()), mRdUse(true), mRdFirst(true)
{
    mId = id_c;
    mPrm = grpAdd("prm_");

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TController::~TController( )
{
    nodeDelAll();

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TController::objName( )	{ return TCntrNode::objName()+":TController"; }

string TController::DAQPath( )	{ return owner().DAQPath()+"."+id(); }

TCntrNode &TController::operator=( const TCntrNode &node )
{
    const TController *src_n = dynamic_cast<const TController*>(&node);
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
	vector<string> prmLs;
	src_n->list(prmLs);
	for(unsigned iP = 0; iP < prmLs.size(); iP++) {
	    if(!owner().tpPrmPresent(src_n->at(prmLs[iP]).at().type().name)) continue;
	    if(!present(prmLs[iP])) add(prmLs[iP], owner().tpPrmToId(src_n->at(prmLs[iP]).at().type().name));
	    (TCntrNode&)at(prmLs[iP]).at() = (TCntrNode&)src_n->at(prmLs[iP]).at();
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
	SYS->db().at().dataDel(fullDB(), owner().nodePath()+"DAQ", *this, true);

	//Delete parameter's tables
	for(unsigned i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++) {
	    string tbl = DB()+"."+owner().tpPrmAt(i_tp).DB(this);
	    SYS->db().at().open(tbl);
	    SYS->db().at().close(tbl,true);
	}
    }
}

TTypeDAQ &TController::owner( ) const	{ return *(TTypeDAQ*)nodePrev(); }

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
	rez = string("0:")+_("Running. ");
	if(owner().redntAllow() && redntUse()) {
	    mess = _("Acquisition data from a remote station: ");
	    string rSt = mRdSt.getVal();
	    if(!rSt.empty()) {
		if(rSt.find(mess) == string::npos) {
		    int rOff = 0;
		    rez.replace(0, 1, TSYS::strSepParse(rSt,0,':',&rOff));
		    mess.append(rSt.substr(rOff));
		}
		else mess = _("Your redundancy settings are incorrect and the controller often enables-disables the redundancy!");
	    }
	    rez += mess;
	}
    }
    else if(enableStat()) rez = string("1:")+_("Enabled. ");
    else rez = string("2:")+_("Disabled. ");

    return rez;
}

void TController::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    mess_sys(TMess::Info, _("Controller configuration loading."));

    bool enSt_prev = enSt, runSt_prev = runSt;

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(fullDB(), owner().nodePath()+"DAQ", *this);
    }

    mRdUse = owner().redntAllow() && (bool)redntMode();

    LoadParmCfg();

    if(!enSt && enSt_prev)	enable();
    if(!runSt && runSt_prev)	start();
}

void TController::save_( )
{
    mess_sys(TMess::Info, _("Controller configuration saving."));

    //Update type controller bd record
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+"DAQ",*this);
}

void TController::start( )
{
    //Enable if no enabled
    if(runSt)	return;
    if(!enSt)	enable();

    mess_sys(TMess::Info, _("Controller starting."));

    //First archives synchronization
    if(owner().redntAllow() && redntMode()) redntDataUpdate();

    //Start for children
    start_();

    runSt = true;
}

void TController::stop( )
{
    if(!runSt)	return;

    mess_sys(TMess::Info, _("Controller stopping."));

    //Stop for children
    stop_();

    runSt = false;
}

void TController::enable( )
{
    if(!enSt) {
	mess_sys(TMess::Info, _("Controller enabling."));

	//Enable for children
	enable_();
    }

    bool enErr = false;
    //Enable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned iPrm = 0; iPrm < prm_list.size(); iPrm++)
	if(at(prm_list[iPrm]).at().toEnable())
	    try{ at(prm_list[iPrm]).at().enable(); }
	    catch(TError &err) {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Warning, _("Error turning on the parameter '%s'."), prm_list[iPrm].c_str());
		enErr = true;
	    }

    //Set enable stat flag
    enSt = true;

    if(enErr) throw err_sys(_("Error turning on some parameters."));
}

void TController::disable( )
{
    if(!enSt) return;

    //Stop if runed
    if(runSt) stop();

    mess_sys(TMess::Info, _("Controller disabling."));

    //Disable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned iPrm = 0; iPrm < prm_list.size(); iPrm++)
	if(at(prm_list[iPrm]).at().enableStat())
	    try{ at(prm_list[iPrm]).at().disable(); }
	    catch(TError &err) {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Warning, _("Error turning off the parameter '%s'."), prm_list[iPrm].c_str());
	    }

    //Disable for children
    disable_();

    //Clear enable flag
    enSt = false;
}

void TController::LoadParmCfg( )
{
    map<string, bool>	itReg;
    vector<vector<string> > full;

    //Search and create new parameters
    for(unsigned iTp = 0; iTp < owner().tpPrmSize(); iTp++) {
	if(owner().tpPrmAt(iTp).DB(this).empty()) continue;
	try {
	    TConfig cEl(&owner().tpPrmAt(iTp));
	    //cEl.cfgViewAll(false);
	    cEl.cfg("OWNER").setS("", TCfg::ForceUse);

	    // Search new into DB and Config-file
	    for(int fldCnt = 0; SYS->db().at().dataSeek(DB()+"."+owner().tpPrmAt(iTp).DB(this),
					   owner().nodePath()+owner().tpPrmAt(iTp).DB(this),fldCnt++,cEl,false,&full); )
	    {
		try {
		    string shfr = cEl.cfg("SHIFR").getS();
		    if(!present(shfr))	add(shfr, iTp);
		    at(shfr).at().load(&cEl);
		    itReg[shfr] = true;
		} catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Warning, _("Error adding the parameter '%s'."), cEl.cfg("SHIFR").getS().c_str());
		}
	    }
	} catch(TError &err) {
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_sys(TMess::Error, _("Error finding and creating new parameters."));
	}
    }

    //Check for remove items removed from DB
    if(SYS->chkSelDB(SYS->selDB(),true)) {
	vector<string> itLs;
	list(itLs);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    if(itReg.find(itLs[iIt]) == itReg.end())
		del(itLs[iIt]);
    }

    //Force load present parameters
    vector<string> prmLs;
    list(prmLs);
    for(unsigned iP = 0; iP < prmLs.size(); iP++) {
	at(prmLs[iP]).at().modifG();
	at(prmLs[iP]).at().load();
    }
}

string TController::add( const string &iid, unsigned type )
{
    return chldAdd(mPrm, ParamAttach(TSYS::strEncode(sTrm(iid),TSYS::oscdID),type));
}

TParamContr *TController::ParamAttach( const string &iid, int type)	{ return new TParamContr(iid, &owner().tpPrmAt(type)); }

void TController::redntDataUpdate( )
{
    vector<RedntStkEl> hst;

    //Prepare a group of a hierarchy request to the parameters
    AutoHD<TParamContr> prm, prmC;
    XMLNode req("CntrReqs"); req.setAttr("path", nodePath());
    req.childAdd("get")->setAttr("path", "/%2fcntr%2fst%2fstatus");

    hst.push_back(RedntStkEl());
    list(hst.back().ls);
    string addr;
    while(true) {
	if(hst.back().pos >= hst.back().ls.size()) {
	    if(!hst.back().addr.size()) break;
	    hst.pop_back(); hst.back().pos++;
	    prm = AutoHD<TParamContr>(hst.back().addr.size()?dynamic_cast<TParamContr*>(prm.at().nodePrev(true)):NULL);
	    continue;
	}
	prmC = prm.freeStat() ? at(hst.back().ls[hst.back().pos]) : prm.at().at(hst.back().ls[hst.back().pos]);
	addr = hst.back().addr + "/prm_"+hst.back().ls[hst.back().pos];
	if(prmC.at().enableStat()) {
	    XMLNode *prmNd = req.childAdd("get")->setAttr("path", addr + "/%2fserv%2fattr");

	    // Prepare individual attributes list
	    prmNd->setAttr("sepReq", "1")->setAttr("prcTm", i2s(prmC.at().mRdPrcTm));

	    // Check attributes last present data time into archives
	    vector<string> listV;
	    prmC.at().vlList(listV);
	    unsigned rC = 0;
	    for(unsigned iV = 0; iV < listV.size(); iV++) {
		AutoHD<TVal> vl = prmC.at().vlAt(listV[iV]);
		if(!vl.at().arch().freeStat())
		    prmNd->childAdd("ael")->setAttr("id",listV[iV])->
					    setAttr("tm",ll2s(vmax(vl.at().arch().at().end(""),
								   TSYS::curTime()-(int64_t)(3.6e9*owner().owner().rdRestDtTm()))));
		if(!vl.at().arch().freeStat() || vl.at().reqFlg()) { prmNd->childAdd("el")->setAttr("id",listV[iV]); rC++; }
	    }
	    if(rC > listV.size()/2) { prmNd->childClear("el"); prmNd->setAttr("sepReq", "0"); }
	    //if(s2i(prmNd->attr("sepReq")) && !prmNd->childSize()) req.childDel(prmNd);
	}
	hst.push_back(RedntStkEl(addr));
	prmC.at().list(hst.back().ls);
	prm = prmC;
    }

    //Send request to the first active station for this controller object
    if(owner().owner().rdStRequest(workId(),req,"",!mRdFirst).empty()) return;
    mRdFirst = false;

    //Write the requested data to the parameters
    for(unsigned iP = 0; iP < req.childSize(); iP++) {
	XMLNode *p = req.childGet(iP);
	addr = p->attr("path");
	if(addr == "/%2fcntr%2fst%2fstatus") { mRdSt.setVal(p->text()); continue; }
	size_t aPos = addr.rfind("/"); addr = (aPos == string::npos) ? "" : addr.substr(0, aPos);
	if((prm=nodeAt(addr,0,0,0,true)).freeStat()) continue;
	prm.at().mRdPrcTm = s2i(p->attr("prcTm"));
	for(unsigned iA = 0; iA < p->childSize(); iA++) {
	    XMLNode *aNd = p->childGet(iA);
	    AutoHD<TVal> vl;
	    if(prm.at().vlPresent(aNd->attr("id"))) vl = prm.at().vlAt(aNd->attr("id"));

	    if(aNd->name() == "el" && !vl.freeStat()) { vl.at().setS(aNd->text(),atoll(aNd->attr("tm").c_str()),true); vl.at().setReqFlg(false); }
	    else if(aNd->name() == "ael" && !vl.freeStat() && !vl.at().arch().freeStat() && aNd->childSize()) {
		int64_t btm = atoll(aNd->attr("tm").c_str());
		int64_t per = atoll(aNd->attr("per").c_str());
		TValBuf buf(vl.at().arch().at().valType(),0,per,false,true);
		for(unsigned i_v = 0; i_v < aNd->childSize(); i_v++)
		    buf.setS(aNd->childGet(i_v)->text(),btm+per*i_v);
		vl.at().arch().at().setVals(buf, buf.begin(), buf.end(), "");
	    }
	    else if(aNd->name() == "del" && prm.at().dynElCntr()) {
		MtxAlloc res(prm.at().dynElCntr()->resEl(), true);
		TFld::Type tp = (TFld::Type)s2i(aNd->attr("type"));
		unsigned flg = s2i(aNd->attr("flg"));
		if(vl.freeStat()) prm.at().dynElCntr()->fldAdd(new TFld(aNd->attr("id").c_str(),aNd->attr("name").c_str(),tp,flg,"","",
									aNd->attr("values").c_str(),aNd->attr("selNames").c_str()));
		else {
		    unsigned aId = prm.at().dynElCntr()->fldId(aNd->attr("id"), true);
		    prm.at().dynElCntr()->fldAt(aId).setDescr(aNd->attr("name"));
		    prm.at().dynElCntr()->fldAt(aId).setFlg(prm.at().dynElCntr()->fldAt(aId).flg()^((prm.at().dynElCntr()->fldAt(aId).flg()^flg)&(TFld::Selectable|TFld::SelEdit)));
		    prm.at().dynElCntr()->fldAt(aId).setValues(aNd->attr("values"));
		    prm.at().dynElCntr()->fldAt(aId).setSelNames(aNd->attr("selNames"));
		}
	    }
	}
    }
}

string TController::catsPat( )
{
    return "^al"+owner().modId()+":"+id()+"(\\.|$)|^"+nodePath();
    //return "/^(al"+owner().modId()+":"+id()+"(\\.|$)|"+nodePath()+")/";
}

void TController::alarmSet( const string &mess, int lev, const string &prm, bool force )
{
    if(force || !redntUse(TController::Any)) {
	string	pId = TSYS::strLine(prm, 0);
	string	pNm = TSYS::strLine(prm, 1);
	string	aCat = "al" + owner().modId() + ":" + id();
	if(pId.size()) aCat += "." + pId;
	string	aMess = mess;
	if(aMess.size()) {
	    pId = name();
	    if(pNm.size()) pId += " > " + pNm;
	    aMess = pId + ((prm.size() && !pNm.size())?" > ":": ") + aMess;
	}

	//Checking for presence at set and missing at unset
	if(!force) {
	    vector<TMess::SRec> recs;
	    SYS->archive().at().messGet(0, SYS->sysTm(), recs, aCat, -1, ALRM_ARCH_NM);
	    if((lev >= 0 && !recs.size()) || (lev < 0 && recs.size() && aMess == recs[0].mess && lev == recs[0].level))
		return;
	}

	message(aCat.c_str(), lev, aMess.c_str());
    }
}

TVariant TController::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // string name( ) - get controller name.
    if(iid == "name")	return name();
    // string descr( ) - get controller description.
    if(iid == "descr")	return descr();
    // string status( ) - get controller status.
    if(iid == "status")	return getStatus();
    // bool alarmSet( string mess, int lev = -5, string prm = "", bool force = false ) -
    //		set alarm to message <mess> and level <lev> for parameter <prm> and omit the presence control at <force>.
    if(iid == "alarmSet" && prms.size() >= 1) {
	alarmSet(prms[0].getS(), (prms.size() >= 2) ? prms[1].getI() : -TMess::Crit,
	    (prms.size() >= 3) ? prms[2].getS() : "", (prms.size() >= 4) ? prms[3].getB() : false);
	return true;
    }
    // bool enable( bool newSt = EVAL ) - get enable status or change it by argument 'newSt' assign.
    if(iid == "enable") {
	if(prms.size())	{ prms[0].getB() ? enable() : disable(); }
	return enableStat();
    }
    // bool start( bool newSt = EVAL ) - get start status or change it by argument 'newSt' assign.
    if(iid == "start") {
	if(prms.size())	{ prms[0].getB() ? start() : stop(); }
	return startStat();
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user, RWRWR_, "root:" SDAQ_ID);
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
	//-1 for waranty all curent date get without doubles and losses
	if(!tm)	{ tm = redntUse(TController::Any) ? SYS->archive().at().rdTm() : (time_t)(TSYS::curTime()/1000000) - 1; opt->setAttr("tm", i2s(tm)); }
	time_t	tm_grnd	= strtoul(opt->attr("tm_grnd").c_str(), 0, 10);
	int	lev	= s2i(opt->attr("lev"));
	SYS->archive().at().messGet(tm_grnd, tm, rez, "/("+catsPat()+")/", lev, "");
	for(unsigned iR = 0; iR < rez.size(); iR++)
	    opt->childAdd("el")->
		setAttr("time", u2s(rez[iR].time))->
		setAttr("utime", u2s(rez[iR].utime))->
		setAttr("cat", rez[iR].categ)->
		setAttr("lev", i2s(rez[iR].level))->
		setText(rez[iR].mess);
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Controller: ")+name(),RWRWR_,"root",SDAQ_ID);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/cntr",_("Controller"))) {
	    if(ctrMkNode("area",opt,-1,"/cntr/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/cntr/st/status",_("Status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cntr/st/enSt",_("Enabled"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/cntr/st/runSt",_("Running"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
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
		    ctrMkNode("fld",opt,-1,"/prm/t_prm",_("Add parameters to the type"),RWRW__,"root",SDAQ_ID,3,"tp","str","dest","select","select","/prm/t_lst");
		ctrMkNode("fld",opt,-1,"/prm/nmb",_("Number"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/prm",_("Parameters"),RWRWR_,"root",SDAQ_ID,5,"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","prm_","idSz",OBJ_ID_SZ);
	    }
	}
	if(ctrMkNode("area",opt,-1,"/mess",_("Diagnostics"))) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time, size (seconds) and level"),RWRW__,"root",SDAQ_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size","",RWRW__,"root",SDAQ_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/lvl","",RWRW__,"root",SDAQ_ID,5,"tp","dec","dest","select",
		"sel_id","0;1;2;3;4;5;6;7",
		"sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)"),
		"help",_("Display messages for larger or even levels.\n"
			 "Also affects to specific-diagnostic messages generation by the data sources."));
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SDAQ_ID)) {
		ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),R_R___,"root",SDAQ_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/mess/mess/0a",_("mcsec"),R_R___,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),R_R___,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mess/mess/2",_("Level"),R_R___,"root",SDAQ_ID,1,"tp","dec");
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
	    opt->setAttr("id", add(opt->attr("id"),owner().tpPrmToId(TBDS::genDBGet(owner().nodePath()+"addType",owner().tpPrmAt(0).name,opt->attr("user")))));
	    at(opt->attr("id")).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	del(opt->attr("id"), true);
    }
    else if(a_path == "/prm/t_lst" && owner().tpPrmSize() && ctrChkNode(opt,"get",R_R_R_)) {
	for(unsigned i_a=0; i_a < owner().tpPrmSize(); i_a++)
	    opt->childAdd("el")->setAttr("id",owner().tpPrmAt(i_a).name)->setText(owner().tpPrmAt(i_a).descr);
    }
    else if(a_path == "/cntr/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/cntr/st/enSt") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(enSt?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	s2i(opt->text()) ? enable() : disable();
    }
    else if(a_path == "/cntr/st/runSt") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(runSt?"1":"0");
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
	    if(!s2i(opt->text())) opt->setText(i2s(TSYS::curTime()/1000000));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	    TBDS::genDBSet(SYS->daq().at().nodePath()+"messTm",(s2i(opt->text())>=TSYS::curTime()/1000000)?"0":opt->text(),opt->attr("user"));
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
	if(!gtm) gtm = (time_t)(TSYS::curTime()/1000000);
	int gsz = s2i(TBDS::genDBGet(SYS->daq().at().nodePath()+"messSize","60",opt->attr("user")));
	SYS->archive().at().messGet(gtm-gsz, gtm, rec, "/("+catsPat()+")/", messLev(), "");

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SDAQ_ID);
	XMLNode *n_tmu  = ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SDAQ_ID);
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SDAQ_ID);
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SDAQ_ID);
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SDAQ_ID);
	for(int iRec = rec.size()-1; iRec >= 0; iRec--) {
	    if(n_tm)	n_tm->childAdd("el")->setText(i2s(rec[iRec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(i2s(rec[iRec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[iRec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(i2s(rec[iRec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[iRec].mess);
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

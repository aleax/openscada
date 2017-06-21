
//OpenSCADA system module DAQ.DAQGate file: daq_gate.cpp
/***************************************************************************
 *   Copyright (C) 2007-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <signal.h>

#include <tsys.h>
#include <ttypeparam.h>

#include "daq_gate.h"

//******************************************************
//* Modul info!                                        *
#define MOD_ID		"DAQGate"
#define MOD_NAME	_("Data sources gate")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"1.7.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allows you to perform the locking of the data sources of the remote OpenSCADA stations in the local ones.")
#define LICENSE		"GPL2"
//******************************************************

DAQGate::TTpContr *DAQGate::mod;  //Pointer for direct access to main module object

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_DAQGate_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_DAQGate_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new DAQGate::TTpContr(source);
	return NULL;
    }
}

using namespace DAQGate;

//******************************************************
//* TTpContr                                           *
//******************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr( )	{ }

void TTpContr::load_( )
{
    //Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's DB structure
    fldAdd(new TFld("PRM_BD",_("Parameters cache table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PERIOD",_("Gather data period (s)"),TFld::Integer,TFld::NoFlag,"6","0","0;100"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"4","10","1;1000"));
    fldAdd(new TFld("TM_REST_DT",_("Restore data depth time (hour)"),TFld::Real,TFld::NoFlag,"6.2","1","0;12"));
    fldAdd(new TFld("GATH_MESS_LEV",_("Gather messages level"),TFld::Integer,TFld::Selected,"1","1",
	"-1;0;1;2;3;4;5;6;7",_("==Disable==;Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)")));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Integer,TFld::NoFlag,"4","0","0;1000"));
    fldAdd(new TFld("STATIONS",_("Remote stations list"),TFld::String,TFld::FullText,"100"));
    fldAdd(new TFld("CNTRPRM",_("Remote cotrollers and parameters list"),TFld::String,TFld::FullText,"200"));
    fldAdd(new TFld("ALLOW_DEL_PA",_("Allow automatic remove parameters and attributes"),TFld::Boolean,TFld::NoFlag,"1","0"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("PRM_ADDR",_("Remote parameter address"),TFld::String,TFld::FullText|TCfg::NoVal,"100",""));
    tpPrmAt(t_prm).fldAdd(new TFld("ATTRS",_("Attributes configuration cache"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
    tpPrmAt(t_prm).fldAdd(new TFld("STATS",_("Presenting for stations"),TFld::String,TCfg::NoVal,"10000",""));
    //Set to read only
    for(unsigned iSz = 0; iSz < tpPrmAt(t_prm).fldSize(); iSz++)
	tpPrmAt(t_prm).fldAt(iSz).setFlg(tpPrmAt(t_prm).fldAt(iSz).flg()|TFld::NoWrite);
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db ) { return new TMdContr(name, daq_db, this); }

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem), enRes(true),
    mSched(cfg("SCHEDULE")), mMessLev(cfg("GATH_MESS_LEV")), mRestDtTm(cfg("TM_REST_DT").getRd()),
    mSync(cfg("SYNCPER").getId()), mPerOld(cfg("PERIOD").getId()), mRestTm(cfg("TM_REST").getId()),
    mPrior(cfg("PRIOR").getId()), mAllowToDelPrmAttr(cfg("ALLOW_DEL_PA").getBd()),
    prcSt(false), callSt(false), endrunReq(false), alSt(-1), mPer(1e9)
{
    cfg("PRM_BD").setS(MOD_ID"Prm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus();

    if(startStat() && !redntUse()) {
	if(callSt)	val += TSYS::strMess(_("Call now. "));
	if(period())	val += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-9*period()).c_str());
	else val += TSYS::strMess(_("Call next by cron '%s'. "),atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	val += TSYS::strMess(_("Spent time: %s[%s]. "),
		tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str());
	bool isWork = false;
	for(unsigned iSt = 0; iSt < mStatWork.size(); iSt++)
	    if(mStatWork[iSt].second.cntr > -1)
		val += TSYS::strMess(_("Station '%s' error, restoring in %.3g s."),mStatWork[iSt].first.c_str(),mStatWork[iSt].second.cntr);
	    else {
		val += TSYS::strMess(_("Requests to station '%s': %.6g."),mStatWork[iSt].first.c_str(),-mStatWork[iSt].second.cntr);
		isWork = true;
	    }
	if(!isWork) val.replace(0,1,"10");
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    //TController::load_();

    //Check for get old period method value
    if(mPerOld) { cfg("SCHEDULE").setS(i2s(mPerOld)); mPerOld = 0; }
}

void TMdContr::enable_( )
{
    string statV, cpEl, daqTp, cntrId, prmId, pIt, cntrPath, prmPath;
    vector<string> prmLs, prmLs1, gPrmLs;
    XMLNode req("list");

    //Clear present parameters configuration
    //!!!! Disabled by store the station configuration and no actual more only for first level parameters
    //list(prmLs);
    //for(unsigned iP = 0; iP < prmLs.size(); iP++) at(prmLs[iP]).at().setStats("");

    //Stations list update
    if(!mStatWork.size())
	for(int stOff = 0; (statV=TSYS::strSepParse(cfg("STATIONS").getS(),0,'\n',&stOff)).size(); )
	    mStatWork.push_back(pair<string,StHd>(statV,StHd()));

    //Remote station scaning. Controllers and parameters scaning
    for(unsigned iSt = 0; iSt < mStatWork.size(); iSt++)
	for(int cpOff = 0; (cpEl=TSYS::strSepParse(cfg("CNTRPRM").getS(),0,'\n',&cpOff)).size(); )
	    try {
		// Parse parameters list
		int pOff = 0;
		daqTp  = TSYS::strParse(cpEl,0,".",&pOff);
		cntrId = TSYS::strParse(cpEl,0,".",&pOff);
		if(daqTp.empty() || cntrId.empty()) continue;

		//  Parse parameter's path
		for(prmPath = prmId = ""; (pIt=TSYS::strParse(cpEl,0,".",&pOff)).size(); ) {
		    if(prmId.size()) prmPath += "prm_"+prmId+"/";
		    prmId = pIt;
		}

		// Get the top parameters list
		prmLs.clear();
		if(!prmId.empty() && prmId != "*") prmLs.push_back(daqTp+"/"+cntrId+"/"+prmPath+"prm_"+prmId);	//Concrete parameter to root
		else {	//Parameters group to root
		    req.clear()->setName("get")->setAttr("path", "/"+mStatWork[iSt].first+"/DAQ/"+daqTp+"/"+cntrId+"/"+prmPath+"%2fbr%2fprm_");
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(), "%s", req.text().c_str());
		    else for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++)
			prmLs.push_back(daqTp+"/"+cntrId+"/"+prmPath+"prm_"+req.childGet(i_ch)->attr("id"));
		}

		if(messLev() == TMess::Debug)
		    mess_debug_(nodePath().c_str(), _("Enable: station '%s' processing item '%s' for parameters %d."),
						mStatWork[iSt].first.c_str(), cpEl.c_str(), prmLs.size());

		// Process root parameters
		for(unsigned iP = 0; iP < prmLs.size(); iP++) {
		    //  The root parameter ID, NAME and included parameters list request
		    XMLNode *nT = req.clear()->setName("CntrReqs")->setAttr("path","/"+mStatWork[iSt].first+"/DAQ/"+prmLs[iP]);
		    nT->childAdd("get")->setAttr("path","%2fprm%2fcfg%2fSHIFR");
		    nT->childAdd("get")->setAttr("path","%2fprm%2fcfg%2fNAME");
		    nT->childAdd("get")->setAttr("path","%2fbr%2fprm_")->setAttr("recurs","1");
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
		    if(s2i(req.attr("rez"))) continue;

		    prmId = req.childGet(0)->text();
		    AutoHD<TMdPrm> curP, curW;

		    //  Find for the parameter
		    list(prmLs1);
		    unsigned iP1 = 0;
		    while(iP1 < prmLs1.size() && at(prmLs1[iP1]).at().prmAddr() != prmLs[iP]) iP1++;
		    if(iP1 >= prmLs1.size()) {
			while(present(prmId) && at(prmId).at().prmAddr().size()) prmId = TSYS::strLabEnum(prmId);
			if(!present(prmId)) add(prmId, owner().tpPrmToId("std"));
			curP = at(prmId);
			curP.at().setName(req.childGet(1)->text());
			curP.at().setPrmAddr(prmLs[iP]);
		    } else curP = at(prmId);

		    if(!curP.at().enableStat()) {
			curP.at().enable();
			if(enableStat()) curP.at().load();
		    }
		    curP.at().setStats(mStatWork[iSt].first);
		    gPrmLs.push_back(curP.at().ownerPath(true));

		    //  Process included parameters
		    string prmPath = prmLs[iP], prmPathW;//, iPrmLs;
		    XMLNode *prmN = req.childGet(2), *prmW;
		    vector<SPrmsStack> stack;
		    for(unsigned i_ip = 0; true; ) {
			if(i_ip >= prmN->childSize()) {
			    //   Pop from the stack
			    if(stack.size()) {
				prmN = stack.back().nd; i_ip = stack.back().pos+1; curP = stack.back().prm;
				stack.pop_back();
				continue;
			    }
			    else break;
			}

			prmW = prmN->childGet(i_ip);
			prmId = prmW->attr("id");
			prmPathW = prmPath+"/prm_"+prmId;

			//   Find for the parameter
			curP.at().list(prmLs1);
			unsigned iP1 = 0;
			while(iP1 < prmLs1.size() && curP.at().at(prmLs1[iP1]).at().prmAddr() != prmPathW) iP1++;
			if(iP1 >= prmLs1.size()) {
			    while(curP.at().present(prmId)) prmId = TSYS::strLabEnum(prmId);
			    curP.at().add(prmId, owner().tpPrmToId("std"));
			    curW = curP.at().at(prmId);
			    curW.at().setName(prmW->text());
			    curW.at().setPrmAddr(prmPathW);
			} else curW = curP.at().at(prmId);

			if(!curW.at().enableStat()) {
			    curW.at().enable();
			    if(enableStat()) curW.at().load();
			}
			curW.at().setStats(mStatWork[iSt].first);
			gPrmLs.push_back(curW.at().ownerPath(true));

			//   Next level process
			if(prmW->childSize()) {
			    stack.push_back(SPrmsStack(prmN,i_ip,curP));
			    prmN = prmW; i_ip = 0; curP = curW;
			}else i_ip++;
		    }
		}
	    } catch(TError &err) { if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), "%s", err.mess.c_str()); }

    //Removing remotely missed parameters in case all remote stations active status by the actual list
    if(mAllowToDelPrmAttr) {
	bool prmChkToDel = true;
	for(unsigned iSt = 0; prmChkToDel && iSt < mStatWork.size(); iSt++)
	    if(mStatWork[iSt].second.cntr >= 0) prmChkToDel = false;
	if(prmChkToDel && enableStat()) {
	    MtxAlloc res(enRes, true);
	    for(unsigned iPrm = 0, iGPrm = 0; iPrm < pHd.size(); ) {
		for(iGPrm = 0; iGPrm < gPrmLs.size() && gPrmLs[iGPrm] != pHd[iPrm].at().ownerPath(true); ) iGPrm++;
		if(iGPrm >= gPrmLs.size()) {
		    string pId = pHd[iPrm].at().id();
		    try {
			TParamContr *pCntr = dynamic_cast<TParamContr*>(pHd[iPrm].at().nodePrev());
			if(pCntr) pCntr->del(pId, TParamContr::RM_NoArch);
			else del(pId, TParamContr::RM_NoArch);
			continue;
		    } catch(TError &err) {
			mess_err(err.cat.c_str(),"%s",err.mess.c_str());
			if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),
			    _("Deletion parameter '%s' is error but it no present on the configuration or remote station."),pId.c_str());
		    }
		}
		iPrm++;
	    }
	}
    }
}

void TMdContr::disable_( )
{
    pHd.clear();
    mStatWork.clear();
    alSt = -1;
}

void TMdContr::start_( )
{
    if(prcSt) return;

    //mStatWork.clear();
    //enable_();	//Comment for prevent long time start in no connection present case

    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    //Clear stations request counter
    for(unsigned iSt = 0; iSt < mStatWork.size(); iSt++) mStatWork[iSt].second.cntr = -1;

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    if(!prcSt) return;

    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrunReq);

    //Connection alarm clear
    alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),owner().modId().c_str(),id().c_str(),_("STOP")), TMess::Info);
    alSt = -1;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE" && startStat())
	mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    return true;
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned iPrm;

    MtxAlloc res(enRes, true);
    for(iPrm = 0; iPrm < pHd.size(); iPrm++)
	if(&pHd[iPrm].at() == prm) break;

    if(val && iPrm >= pHd.size())	pHd.push_back(prm);
    if(!val && iPrm < pHd.size())	pHd.erase(pHd.begin()+iPrm);
}

void *TMdContr::Task( void *icntr )
{
    map<string,float>::iterator sti;
    TMdContr &cntr = *(TMdContr *)icntr;
    int64_t tCnt, tPrev = TSYS::curTime();
    double syncCnt = 0;
    unsigned int div = 0;

    cntr.endrunReq = false;
    cntr.prcSt = true;
    bool firstCall = true;

    for(unsigned int itCnt = 0; !cntr.endrunReq; itCnt++) {
	if(cntr.redntUse()) {
	    //Request to the redundant host by messages
	    if(cntr.mMessLev.getI() >= 0) {
		XMLNode req("CntrReqs");
		for(unsigned iSt = 0; iSt < cntr.mStatWork.size(); iSt++) {
		    int tm_grnd = cntr.mStatWork[iSt].second.lstMess["<<redundant>>"];
		    XMLNode *reqCh = req.childAdd("get")->setAttr("path", "/sub_Archive/%2fserv%2fmess")->
				setAttr("tm_grnd", i2s(tm_grnd))->
				setAttr("cat", cntr.mStatWork[iSt].first+":*")->setAttr("lev",cntr.mMessLev);
		    // Alarms force request
		    if(!tm_grnd && cntr.mMessLev.getI() >= 0) reqCh->setAttr("lev", i2s(-cntr.mMessLev.getI()));
		    else reqCh->setAttr("lev", cntr.mMessLev.getS());
		}

		SYS->daq().at().rdStRequest(cntr.workId(), req);

		// Result messages processing
		for(unsigned iSt = 0; iSt < cntr.mStatWork.size() && iSt < req.childSize(); iSt++) {
		    XMLNode *prmNd = req.childGet(iSt);
		    for(unsigned iM = 0; iM < prmNd->childSize(); iM++) {
			XMLNode *m = prmNd->childGet(iM);
			SYS->archive().at().messPut(s2i(m->attr("time")), s2i(m->attr("utime")), m->attr("cat"), s2i(m->attr("lev")), m->text());
		    }
		    for(map<string, time_t>::iterator iLM = cntr.mStatWork[iSt].second.lstMess.begin();
								iLM != cntr.mStatWork[iSt].second.lstMess.end(); ++iLM)
			iLM->second = iLM->second ? s2i(prmNd->attr("tm")) : SYS->sysTm()-3600*cntr.restDtTm();
		}
		TSYS::sysSleep(SYS->rdTaskPer());
	    }
	    else TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
	    continue;
	}

	cntr.callSt = true;
	tCnt = TSYS::curTime();

	try {
	    //Allow stations presenting
	    bool isAccess = false, needEnable = false;
	    for(unsigned iSt = 0; iSt < cntr.mStatWork.size(); iSt++) {
		if(firstCall)	cntr.mStatWork[iSt].second.cntr = 0;	//Reset counter for connection alarm state update
		if(cntr.mStatWork[iSt].second.cntr > 0)
		    cntr.mStatWork[iSt].second.cntr = vmax(0,cntr.mStatWork[iSt].second.cntr-1e-6*(tCnt-tPrev));
		if(cntr.mStatWork[iSt].second.cntr <= 0)	isAccess = true;
		if(cntr.mStatWork[iSt].second.cntr == 0)	needEnable = true;	//!!!! May be only for all == 0 stations
	    }
	    if(!isAccess) { tPrev = tCnt; TSYS::sysSleep(1); continue; }
	    else {
		if(cntr.syncPer()) {	//Enable sync
		    div = cntr.period() ? vmax(2,(unsigned int)(cntr.syncPer()/(1e-9*cntr.period()))) : 0;
		    if(syncCnt <= 0) syncCnt = cntr.syncPer();
		    syncCnt = vmax(0, syncCnt-1e-6*(tCnt-tPrev));
		}
		else { div = 0; syncCnt = 1; }	//Disable sync

		//Parameters list update
		if(firstCall || needEnable || (!div && syncCnt <= 0) || (div && itCnt > div && (itCnt%div) == 0))
		    try { cntr.enable_(); } catch(TError &err) { }

		MtxAlloc resPrm(cntr.enRes, true);

		//Mark no process
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
		    TMdPrm &pO = cntr.pHd[iP].at();
		    if(!pO.isSynced || (!div && syncCnt <= 0) || (div && itCnt > div && (((itCnt+iP)%div) == 0))) pO.sync();
		    pO.isPrcOK = false;
		}

		//Station's cycle
		for(unsigned iSt = 0; iSt < cntr.mStatWork.size(); iSt++) {
		    if(cntr.mStatWork[iSt].second.cntr > 0) continue;
		    XMLNode req("CntrReqs"); req.setAttr("path", "/"+cntr.mStatWork[iSt].first+"/DAQ/");
		    map<string, bool> cntrLstMA;
		    string scntr;

		    //Put attributes requests
		    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
			TMdPrm &prm = cntr.pHd[iP].at();
			if(prm.isPrcOK) continue;
			for(int c_off = 0; (scntr=TSYS::strSepParse(prm.stats(),0,';',&c_off)).size(); ) {
			    if(scntr != cntr.mStatWork[iSt].first) continue;
			    string aMod	= TSYS::pathLev(prm.prmAddr(), 0);
			    string aCntr = TSYS::pathLev(prm.prmAddr(), 1);
			    cntrLstMA[aMod+"/"+aCntr] = true;

			    XMLNode *prmNd = req.childAdd("get")->setAttr("lcPs",i2s(iP))->setAttr("path",prm.prmAddr()+"/%2fserv%2fattr");
			    prmNd->setAttr("hostTm", !cntr.restDtTm() ? "1" : "0");

			    // Prepare individual attributes list
			    bool sepReq = !prm.isEVAL && ((!div && syncCnt > 0) || (div && ((itCnt+iP)%div)));
			    prmNd->setAttr("sepReq", sepReq ? "1" : "0");
			    if(!cntr.restDtTm() && !sepReq) continue;

			    vector<string> listV;
			    prm.vlList(listV);
			    unsigned rC = 0;
			    for(unsigned iV = 0; iV < listV.size(); iV++) {
				AutoHD<TVal> vl = prm.vlAt(listV[iV]);
				if(sepReq && (!vl.at().arch().freeStat() || vl.at().reqFlg())) {
				    prmNd->childAdd("el")->setAttr("id",listV[iV]);
				    rC++;
				}
				if(!vl.at().arch().freeStat())
				    prmNd->childAdd("ael")->setAttr("id",listV[iV])->
					setAttr("tm", ll2s(vmax(vl.at().arch().at().end(""),TSYS::curTime()-(int64_t)(3.6e9*cntr.restDtTm()))));
			    }
			    if(sepReq && !prmNd->childSize()) { req.childDel(prmNd); prm.isPrcOK = true; }	//Pass request and mark by processed
			    if(sepReq && rC > listV.size()/2) {
				prmNd->childClear("el");
				prmNd->setAttr("sepReq", "0");
			    }
			}
		    }

		    //Requests to the controllers messages prepare
		    if(cntr.mMessLev.getI() >= 0)	//Else disabled
			for(map<string,bool>::iterator i_c = cntrLstMA.begin(); i_c != cntrLstMA.end(); ++i_c) {
			    int tm_grnd = cntr.mStatWork[iSt].second.lstMess[i_c->first];
			    XMLNode *reqCh = req.childAdd("get")->setAttr("path", "/"+i_c->first+"/%2fserv%2fmess")->setAttr("tm_grnd", i2s(tm_grnd));
			    if(!tm_grnd && cntr.mMessLev.getI() >= 0)	//Alarms force request
				reqCh->setAttr("lev", i2s(-cntr.mMessLev.getI()));
			    else reqCh->setAttr("lev", cntr.mMessLev.getS());
			}

		    if(!req.childSize()) continue;

		    //Same request
		    if(cntr.cntrIfCmd(req)) {
			if(cntr.messLev() == TMess::Debug) mess_debug_(cntr.nodePath().c_str(), "%s", req.text().c_str());
			continue;
		    }

		    //Result process
		    for(unsigned i_r = 0; i_r < req.childSize(); ++i_r) {
			XMLNode *prmNd = req.childGet(i_r);
			if(s2i(prmNd->attr("err"))) continue;
			string aMod	= TSYS::pathLev(prmNd->attr("path"), 0);
			string aCntr	= TSYS::pathLev(prmNd->attr("path"), 1);
			string pId	= TSYS::pathLev(prmNd->attr("path"), 2);
			if(pId == "/serv/mess") {
			    for(unsigned i_m = 0; i_m < prmNd->childSize(); i_m++) {
				XMLNode *m = prmNd->childGet(i_m);
				SYS->archive().at().messPut(s2i(m->attr("time")), s2i(m->attr("utime")),
				    cntr.mStatWork[iSt].first+":"+m->attr("cat"), s2i(m->attr("lev")), m->text());
			    }
			    cntr.mStatWork[iSt].second.lstMess[aMod+"/"+aCntr] =
				cntr.mStatWork[iSt].second.lstMess[aMod+"/"+aCntr] ? s2i(prmNd->attr("tm")) :
										    SYS->sysTm()-3600*cntr.restDtTm();
			    cntr.mStatWork[iSt].second.lstMess["<<redundant>>"] =
				cntr.mStatWork[iSt].second.lstMess["<<redundant>>"] ? s2i(prmNd->attr("tm")) :
										    SYS->sysTm()-3600*cntr.restDtTm();
			}
			else {
			    TMdPrm &prm = cntr.pHd[s2i(prmNd->attr("lcPs"))].at();
			    if(prm.isPrcOK) continue;
			    prm.isPrcOK = true;
			    prm.isEVAL = false;

			    for(unsigned iA = 0; iA < prmNd->childSize(); iA++) {
				XMLNode *aNd = prmNd->childGet(iA);
				if(!prm.vlPresent(aNd->attr("id"))) continue;
				AutoHD<TVal> vl = prm.vlAt(aNd->attr("id"));

				if(aNd->name() == "el") {
				    vl.at().setS(aNd->text(),cntr.restDtTm()?atoll(aNd->attr("tm").c_str()):0,true);
				    vl.at().setReqFlg(false);
				}
				else if(aNd->name() == "ael" && !vl.at().arch().freeStat() && aNd->childSize()) {
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
		}

		//Mark not processed parameters to EVAL
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
		    TMdPrm &prm = cntr.pHd[iP].at();
		    if(prm.isPrcOK || prm.isEVAL) continue;
		    vector<string> vLs;
		    prm.elem().fldList(vLs);
		    for(unsigned i_v = 0; i_v < vLs.size(); i_v++) {
			if(vLs[i_v] == "SHIFR" || vLs[i_v] == "NAME" || vLs[i_v] == "DESCR") continue;
			AutoHD<TVal> vl = prm.vlAt(vLs[i_v]);
			if(vl.at().getS() == EVAL_STR) continue;
			vl.at().setS(EVAL_STR, vl.at().arch().freeStat() ? 0 :
			    vmin(TSYS::curTime(),vmax(vl.at().arch().at().end(""),TSYS::curTime()-(int64_t)(3.6e9*cntr.restDtTm()))), true);
		    }
		    prm.vlAt("err").at().setS(_("10:Data are not allowed."),0,true);
		    prm.isEVAL = true;
		}

		resPrm.unlock();
	    }
	    firstCall = false;
	} catch(TError &err)	{ mess_err(err.cat.c_str(),err.mess.c_str()); }

	//Calc acquisition process time
	tPrev = tCnt;
	cntr.callSt = false;

	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prcSt = false;

    return NULL;
}

string TMdContr::catsPat( )
{
    string curPat = TController::catsPat();

    string statV;
    for(int stOff = 0; (statV=TSYS::strSepParse(cfg("STATIONS").getS(),0,'\n',&stOff)).size(); )
	curPat += "|^"+statV+":";

    return curPat;
}

int TMdContr::cntrIfCmd( XMLNode &node )
{
    string reqStat = TSYS::pathLev(node.attr("path"), 0);

    bool stPresent = false;
    for(unsigned iSt = 0; iSt < mStatWork.size(); iSt++)
	if(mStatWork[iSt].first == reqStat) {
	    stPresent = true;
	    if(mStatWork[iSt].second.cntr > 0) break;
	    try {
		node.setAttr("conTm", enableStat()?"":"1000");	//Set one second timeout to disabled controller for start procedure speed up.
		int rez = SYS->transport().at().cntrIfCmd(node, MOD_ID+id());
		if(alSt != 0) {
		    alSt = 0;
		    alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),owner().modId().c_str(),id().c_str(),_("OK")), TMess::Info);
		}
		mStatWork[iSt].second.cntr -= 1;
		return rez;
	    } catch(TError &err) {
		if(alSt <= 0) {
		    alSt = 1;
		    alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source '%s': %s."),owner().modId().c_str(),id().c_str(),
						mStatWork[iSt].first.c_str(),TRegExp(":","g").replace(err.mess,"=").c_str()));
		}
		if(callSt) mStatWork[iSt].second.cntr = mRestTm;
		throw;
	    }
	}
    if(!stPresent) node.setAttr("err", TSYS::strMess(_("11:Station missed '%s'."),reqStat.c_str()));

    return s2i(node.attr("err"));
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SCHEDULE",mSched.fld().descr(),RWRWR_,"root",SDAQ_ID,
	    "dest","sel_ed", "sel_list",TMess::labSecCRONsel(), "help",TMess::labSecCRON(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID, "help",TMess::labTaskPrior(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/TM_REST_DT",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero for disable archive access."), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SYNCPER",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero for disable periodic sync."), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/STATIONS",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "help",_("Remote OpenSCADA stations' identifiers list used into it controller."), "rows", "2", NULL);
	ctrMkNode2("fld",opt,-2,"/cntr/cfg/SEL_STAT",_("Append station"),enableStat()?0:RWRW__,"root",SDAQ_ID,
	    "dest","select", "select","/cntr/cfg/SEL_STAT_lst", NULL);
	ctrMkNode2("comm",opt,-2,"/cntr/cfg/host_lnk",_("Go to remote stations list configuration"),enableStat()?0:RWRW__,"root",SDAQ_ID,
	    "tp","lnk", NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/CNTRPRM",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "help",_("Remote OpenSCADA full controller's or separated controller's parameters list. Address example:\n"
		     "  System.AutoDA - for controller;\n"
		     "  System.AutoDA.UpTimeStation - for controller's parameter."), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/CPRM_TREE",_("Parameters tree"),(enableStat()&&(!startStat()))?RWRW__:0,"root",SDAQ_ID,
	    "dest","select", "select","/cntr/cfg/CPRM_lst", NULL);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/SEL_STAT") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) opt->setText("");
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR) && opt->text().size()) {
	    string vLs = cfg("STATIONS"), vS;
	    for(int off = 0; (vS=TSYS::strLine(vLs,0,&off)).size(); )
		if(TSYS::strParse(vS,0,":") == opt->text())
		    break;
	    if(vS.empty()) cfg("STATIONS") = vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+opt->text();
	}
    }
    else if(a_path == "/cntr/cfg/SEL_STAT_lst" && ctrChkNode(opt)) {
	vector<TTransportS::ExtHost> list;
	SYS->transport().at().extHostList("*", list);
	for(unsigned iL = 0; iL < list.size(); iL++) opt->childAdd("el")->setText(list[iL].id);
    }
    else if(a_path == "/cntr/cfg/CPRM_TREE") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) opt->setText(TBDS::genDBGet(owner().nodePath()+"selCPRM","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	    if(opt->text() == _("<<Append current>>")) {
		string vLs = cfg("CNTRPRM"), vS, setVl = TBDS::genDBGet(owner().nodePath()+"selCPRM","",opt->attr("user"));
		for(int off = 0; (vS=TSYS::strLine(vLs,0,&off)).size(); )
		    if(TSYS::strParse(vS,0,":") == setVl)
			break;
		if(vS.empty()) cfg("CNTRPRM") = vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+setVl;
	    }
	    else TBDS::genDBSet(owner().nodePath()+"selCPRM",TSYS::pathLev(opt->text(),0),opt->attr("user"));
	}
    }
    else if(a_path == "/cntr/cfg/CPRM_lst" && ctrChkNode(opt))
	for(unsigned iSt = 0; enableStat() && iSt < mStatWork.size(); iSt++) {
	    if(mStatWork[iSt].second.cntr > 0) continue;
	    XMLNode req("list");
	    req.setAttr("path", "/"+mStatWork[iSt].first+"/DAQ/%2fserv%2fPrmAttr")->
		setAttr("base", TBDS::genDBGet(owner().nodePath()+"selCPRM","",opt->attr("user")))->
		setAttr("toPrm","1")->setAttr("sep",".");
	    if(cntrIfCmd(req) == 0) {
		*opt = req;
		opt->childIns(0,"el")->setText(_("<<Append current>>"));
		break;
	    }
	}
    else if(a_path == "/cntr/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) opt->setText("/Transport");
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name,tp_prm), isPrcOK(false), isEVAL(true), isSynced(false), pEl("w_attr"),
    mPrmAddr(cfg("PRM_ADDR")), mStats(cfg("STATS"))
{
    setToEnable(true);
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    //if(vlCfg())	setVlCfg(NULL);		//!!!! For prevent SHIFR and other lost at the parameter restore, by lock, after it deletion try
    if(!vlElemPresent(&pEl))	vlElemAtt(&pEl);
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    owner().prmEn(this, true);	//Put to process
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    owner().prmEn(this, false);	//Remove from process

    TParamContr::disable();

    //Set EVAL to the parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::setStats( const string &vl )
{
    if(vl.empty()) { mStats = ""; return; }

    string scntr;
    for(int off = 0; (scntr=TSYS::strSepParse(mStats,0,';',&off)).size(); )
	if(scntr == vl) return;
    mStats.setS(mStats.getS()+vl+";");
}

void TMdPrm::load_( )
{
    //Load from cache
    //TParamContr::load_();

    //Restore attributes from the cache
    try {
	XMLNode attrsNd;
	attrsNd.load(cfg("ATTRS").getS());
	for(unsigned iEl = 0; iEl < attrsNd.childSize(); iEl++) {
	    XMLNode *aEl = attrsNd.childGet(iEl);
	    if(vlPresent(aEl->attr("id"))) continue;
	    pEl.fldAdd(new TFld(aEl->attr("id").c_str(),aEl->attr("nm").c_str(),(TFld::Type)s2i(aEl->attr("tp")),
		s2i(aEl->attr("flg")),"","",aEl->attr("vals").c_str(),aEl->attr("names").c_str()));
	    //vlAt(aEl->attr("id")).at().setS(aEl->text());
	}
    } catch(TError &err) { }

    //Sync attributes list
    sync();
}

void TMdPrm::save_( )
{
    //Prepare attributes cache configuration
    XMLNode attrsNd("Attrs");
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned iEl = 0; iEl < ls.size(); iEl++) {
	AutoHD<TVal> vl = vlAt(ls[iEl]);
	attrsNd.childAdd("a")->setAttr("id", ls[iEl])->
			       setAttr("nm", vl.at().fld().descr())->
			       setAttr("tp", i2s(vl.at().fld().type()))->
			       setAttr("flg", i2s(vl.at().fld().flg()))->
			       setAttr("vals", vl.at().fld().values())->
			       setAttr("names", vl.at().fld().selNames());
			       //setText(vl.at().getS());
    }
    cfg("ATTRS").setS(attrsNd.save(XMLNode::BrAllPast));

    //Save to cache
    TParamContr::save_();
}

void TMdPrm::sync( )
{
    //Request and update attributes list
    string scntr;
    XMLNode req("CntrReqs");
    for(int c_off = 0; (scntr=TSYS::strSepParse(stats(),0,';',&c_off)).size(); )
	try {
	    vector<string> als;
	    req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr());
	    req.childAdd("get")->setAttr("path","%2fprm%2fcfg%2fNAME");
	    req.childAdd("get")->setAttr("path","%2fprm%2fcfg%2fDESCR");
	    req.childAdd("list")->setAttr("path","%2fserv%2fattr");
	    if(owner().cntrIfCmd(req))	throw TError(req.attr("mcat").c_str(),req.text().c_str());

	    setName(req.childGet(0)->text());
	    setDescr(req.childGet(1)->text());

	    // Check and create new attributes
	    for(unsigned iA = 0; iA < req.childGet(2)->childSize(); iA++) {
		XMLNode *ael = req.childGet(2)->childGet(iA);
		als.push_back(ael->attr("id"));
		if(vlPresent(ael->attr("id")))	continue;
		TFld::Type tp = (TFld::Type)s2i(ael->attr("tp"));
		pEl.fldAdd(new TFld(ael->attr("id").c_str(),ael->attr("nm").c_str(),tp,
		    (s2i(ael->attr("flg"))&(TFld::Selected|TFld::NoWrite|TFld::HexDec|TFld::OctDec|TFld::FullText))|TVal::DirWrite|TVal::DirRead,
		    "","",ael->attr("vals").c_str(),ael->attr("names").c_str()));
		modif(true);
	    }

	    // Check for remove attributes
	    for(int iP = 0; owner().mAllowToDelPrmAttr && iP < (int)pEl.fldSize(); iP++) {
		unsigned iL;
		for(iL = 0; iL < als.size(); iL++)
		    if(pEl.fldAt(iP).name() == als[iL])
			break;
		if(iL >= als.size())
		    try{ pEl.fldDel(iP); iP--; modif(true); }
		    catch(TError &err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
	    }
	    isSynced = true;
	    return;
	} catch(TError &err) { continue; }
}

void TMdPrm::vlGet( TVal &val )
{
    if(val.name() == "err" && (!enableStat() || !owner().startStat())) TParamContr::vlGet(val);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setI(EVAL_INT, 0, true); return; }
    if(vl.isEVal() || vl == pvl) return;

    XMLNode req("set");

    //Send to active reserve station
    if(owner().redntUse()) {
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(), req);
	return;
    }

    //Direct write
    string scntr;
    for(int c_off = 0; (scntr=TSYS::strSepParse(stats(),0,';',&c_off)).size(); )
	try {
	    req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr()+"/%2fserv%2fattr")->
		childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	    if(owner().cntrIfCmd(req))	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	} catch(TError &err) { continue; }
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat())	return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

TVal* TMdPrm::vlNew( )	{ return new TMdVl(); }

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path.substr(0,6) == "/serv/") { TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm");

	if(ctrMkNode("area",opt,0,"/prm",_("Parameter"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		if(owner().enableStat())
		    ctrMkNode("fld",opt,-1,"/prm/st/en",_("Enable"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/id",_("Id"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/nm",_("Name"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
	    }
	    XMLNode *cfgN = ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"));
	    if(cfgN) {
		// Get remote parameter's config section
		string scntr;
		XMLNode req("info");
		for(int c_off = 0; (scntr=TSYS::strSepParse(stats(),0,';',&c_off)).size(); )
		    try {
			req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr()+"/%2fprm%2fcfg");
			if(owner().cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),req.text().c_str());
			break;
		    } catch(TError &err) { continue; }
		if(req.childSize()) {
		    *cfgN = *req.childGet(0);
		    cfgN->setAttr("dscr",_("Remote station configuration"));
		}
	    }
	}
	return;
    }
    //Process command to page
    if(a_path == "/prm/st/type" && ctrChkNode(opt))		opt->setText(type().descr);
    else if(a_path == "/prm/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(!owner().enableStat()) throw TError(nodePath().c_str(),_("Controller is not started!"));
	    else s2i(opt->text())?enable():disable();
	}
    }
    else if(a_path == "/prm/st/id" && ctrChkNode(opt))	opt->setText(id());
    else if(a_path == "/prm/st/nm" && ctrChkNode(opt))	opt->setText(name());
    else if(a_path.compare(0,4,"/prm") == 0) {
	//Request to remote host
	string scntr;
	for(int c_off = 0; (scntr=TSYS::strSepParse(stats(),0,';',&c_off)).size(); )
	    try {
		opt->setAttr("path",scntr+"/DAQ/"+prmAddr()+"/"+TSYS::strEncode(a_path,TSYS::PathEl));
		if(owner().cntrIfCmd(*opt)) TValue::cntrCmdProc(opt);
	    } catch(TError &err) { continue; }
	opt->setAttr("path",a_path);
    }
    else TParamContr::cntrCmdProc(opt);
}

//******************************************************
//* TMdVl                                              *
//******************************************************
TMdPrm &TMdVl::owner( ) const	{ return *(dynamic_cast<TMdPrm*>(nodePrev())); }

void TMdVl::cntrCmdProc( XMLNode *opt )
{
    if(!arch().freeStat()) { TVal::cntrCmdProc(opt); return; }

    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/val" && owner().owner().restDtTm()) {	//Values access
	// Request to remote station
	string scntr;
	for(int c_off = 0; (scntr=TSYS::strSepParse(owner().stats(),0,';',&c_off)).size(); )
	    try {
		opt->setAttr("path",scntr+"/DAQ/"+owner().prmAddr()+"/a_"+name()+"/"+TSYS::strEncode(a_path,TSYS::PathEl));
		if(!owner().owner().cntrIfCmd(*opt)) break;
	    } catch(TError &err) { continue; }
	opt->setAttr("path",a_path);
	return;
    }

    TVal::cntrCmdProc(opt);
}

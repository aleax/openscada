
//OpenSCADA system module DAQ.DAQGate file: daq_gate.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <ttiparam.h>

#include "daq_gate.h"

//******************************************************
//* Modul info!                                        *
#define MOD_ID		"DAQGate"
#define MOD_NAME	_("Data sources gate")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.9.5"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow to make gate data sources of remote OpenSCADA station to local OpenSCADA station.")
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
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTpContr::~TTpContr( )	{ }

void TTpContr::load_( )
{
    //Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //Controler's DB structure
    fldAdd(new TFld("PRM_BD",_("Parameters cache table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PERIOD",_("Gather data period (s)"),TFld::Integer,TFld::NoFlag,"6","0","0;100"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"3","30","1;1000"));
    fldAdd(new TFld("TM_REST_DT",_("Restore data depth time (hour)"),TFld::Real,TFld::NoFlag,"6.2","1","0;12"));
    fldAdd(new TFld("GATH_MESS_LEV",_("Gather messages level"),TFld::Integer,TFld::Selected,"1","1",
	"-1;0;1;2;3;4;5;6;7",_("==Disable==;Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)")));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","0","0;1000"));
    fldAdd(new TFld("STATIONS",_("Remote stations list"),TFld::String,TFld::FullText,"100"));
    fldAdd(new TFld("CNTRPRM",_("Remote cotrollers and parameters list"),TFld::String,TFld::FullText,"200"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("PRM_ADDR",_("Remote parameter address"),TFld::String,TFld::FullText|TCfg::NoVal,"100",""));
    tpPrmAt(t_prm).fldAdd(new TFld("ATTRS",_("Attributes configuration cache"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
    //Set to read only
    for(unsigned i_sz = 0; i_sz < tpPrmAt(t_prm).fldSize(); i_sz++)
	tpPrmAt(t_prm).fldAt(i_sz).setFlg(tpPrmAt(t_prm).fldAt(i_sz).flg()|TFld::NoWrite);
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db ) { return new TMdContr(name, daq_db, this); }

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem),
    mSched(cfg("SCHEDULE")), mMessLev(cfg("GATH_MESS_LEV")), mSync(cfg("SYNCPER").getRd()),
    mRestDtTm(cfg("TM_REST_DT").getRd()), mPerOld(cfg("PERIOD").getId()),
    mRestTm(cfg("TM_REST").getId()), mPrior(cfg("PRIOR").getId()),
    prcSt(false), call_st(false), endrunReq(false), alSt(-1), mPer(1e9), tmGath(0)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&enRes, &attrM);
    pthread_mutexattr_destroy(&attrM);

    cfg("PRM_BD").setS(MOD_ID"Prm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();

    pthread_mutex_destroy(&enRes);
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus();

    if(startStat() && !redntUse()) {
	if(call_st)	val += TSYS::strMess(_("Call now. "));
	if(period())	val += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-3*period()).c_str());
	else val += TSYS::strMess(_("Call next by cron '%s'. "),tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	val += TSYS::strMess(_("Spent time: %s. "),tm2s(tmGath).c_str());
	bool isWork = false;
	for(unsigned i_st = 0; i_st < mStatWork.size(); i_st++)
	    if(mStatWork[i_st].second.cntr > -1)
		val += TSYS::strMess(_("Station '%s' error, restoring in %.3g s."),mStatWork[i_st].first.c_str(),mStatWork[i_st].second.cntr);
	    else {
		val += TSYS::strMess(_("Requests to station '%s': %.6g."),mStatWork[i_st].first.c_str(),-mStatWork[i_st].second.cntr);
		isWork = true;
	    }
	if(!isWork) val.replace(0,1,"10");
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    TController::load_();

    //Check for get old period method value
    if(mPerOld) { cfg("SCHEDULE").setS(i2s(mPerOld)); mPerOld = 0; }
}

void TMdContr::enable_( )
{
    string statV, cpEl, daqTp, cntrId, prmId, pIt, cntrPath, prmPath;
    vector<string> prmLs, prmLs1, gPrmLs;
    XMLNode req("list");

    //Clear present parameters configuration
    list(prmLs);
    for(unsigned i_p = 0; i_p < prmLs.size(); i_p++) at(prmLs[i_p]).at().setStats("");

    //Station list update
    if(!mStatWork.size())
	for(int stOff = 0; (statV=TSYS::strSepParse(cfg("STATIONS").getS(),0,'\n',&stOff)).size(); )
	    mStatWork.push_back(pair<string,StHd>(statV,StHd()));

    //Remote station scaning. Controllers and parameters scaning
    for(unsigned i_st = 0; i_st < mStatWork.size(); i_st++)
	for(int cpOff = 0; (cpEl=TSYS::strSepParse(cfg("CNTRPRM").getS(),0,'\n',&cpOff)).size(); )
	    try {
		// Parse parameters list
		int pOff = 0;
		daqTp  = TSYS::strParse(cpEl,0,".",&pOff);
		cntrId = TSYS::strParse(cpEl,0,".",&pOff);
		if(daqTp.empty() || cntrId.empty()) continue;

		//  Parse parameter's path
		for(prmPath = prmId = ""; (pIt=TSYS::strParse(cpEl,0,".",&pOff)).size(); )
		{
		    if(prmId.size()) prmPath += "prm_"+prmId+"/";
		    prmId = pIt;
		}

		// Get top parameters list
		prmLs.clear();
		if(!prmId.empty() && prmId != "*") prmLs.push_back(daqTp+"/"+cntrId+"/"+prmPath+"prm_"+prmId);	//Concrete parameter to root
		else {	//Parameters group to root
		    req.clear()->setName("get")->setAttr("path","/"+mStatWork[i_st].first+"/DAQ/"+daqTp+"/"+cntrId+"/"+prmPath+"%2fbr%2fprm_");
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
		    else for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++)
			prmLs.push_back(daqTp+"/"+cntrId+"/"+prmPath+"prm_"+req.childGet(i_ch)->attr("id"));
		}

		// Process root parameters
		for(unsigned i_p = 0; i_p < prmLs.size(); i_p++) {
		    //  The root parameter ID, NAME and included parameters list request
		    XMLNode *nT = req.clear()->setName("CntrReqs")->setAttr("path","/"+mStatWork[i_st].first+"/DAQ/"+prmLs[i_p]);
		    nT->childAdd("get")->setAttr("path","%2fprm%2fcfg%2fSHIFR");
		    nT->childAdd("get")->setAttr("path","%2fprm%2fcfg%2fNAME");
		    nT->childAdd("get")->setAttr("path","%2fbr%2fprm_")->setAttr("recurs","1");
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
		    if(s2i(req.attr("rez"))) continue;

		    prmId = req.childGet(0)->text();
		    AutoHD<TMdPrm> curP, curW;

		    //  Find for the parameter
		    list(prmLs1);
		    unsigned i_p1 = 0;
		    while(i_p1 < prmLs1.size() && at(prmLs1[i_p1]).at().prmAddr() != prmLs[i_p]) i_p1++;
		    if(i_p1 >= prmLs1.size()) {
			while(present(prmId) && at(prmId).at().prmAddr().size()) prmId = TSYS::strLabEnum(prmId);
			if(!present(prmId)) add(prmId, owner().tpPrmToId("std"));
			curP = at(prmId);
			curP.at().setName(req.childGet(1)->text());
			curP.at().setPrmAddr(prmLs[i_p]);
		    } else curP = at(prmId);
		    if(!curP.at().enableStat()) {
			curP.at().enable();
			if(enableStat()) curP.at().load();
		    }
		    curP.at().setStats(mStatWork[i_st].first);
		    gPrmLs.push_back(curP.at().ownerPath(true));

		    //  Process included parameters
		    string prmPath = prmLs[i_p], prmPathW;//, iPrmLs;
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
			unsigned i_p1 = 0;
			while(i_p1 < prmLs1.size() && curP.at().at(prmLs1[i_p1]).at().prmAddr() != prmPathW) i_p1++;
			if(i_p1 >= prmLs1.size()) {
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
			curW.at().setStats(mStatWork[i_st].first);
			gPrmLs.push_back(curW.at().ownerPath(true));

			//   Next level process
			if(prmW->childSize()) {
			    stack.push_back(SPrmsStack(prmN,i_ip,curP));
			    prmN = prmW; i_ip = 0; curP = curW;
			}else i_ip++;
		    }
		}
	    }
	    catch(TError err) { if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), "%s", err.mess.c_str()); }

    //Removing remotely missed parameters in case all remote stations active status by actual list
    bool prmChkToDel = true;
    for(unsigned i_st = 0; prmChkToDel && i_st < mStatWork.size(); i_st++)
	if(mStatWork[i_st].second.cntr >= 0) prmChkToDel = false;
    if(prmChkToDel && enableStat()) {
	MtxAlloc res(enRes, true);
	for(unsigned i_prm = 0, i_gPrm = 0; i_prm < pHd.size(); ) {
	    for(i_gPrm = 0; i_gPrm < gPrmLs.size() && gPrmLs[i_gPrm] != pHd[i_prm].at().ownerPath(true); ) i_gPrm++;
	    if(i_gPrm >= gPrmLs.size()) {
		string pId = pHd[i_prm].at().id();
		try {
		    TParamContr *pCntr = dynamic_cast<TParamContr*>(pHd[i_prm].at().nodePrev());
		    if(pCntr) pCntr->del(pId, true);
		    else del(pId, true);
		    continue;
		}
		catch(TError err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),
			    _("Deletion parameter '%s' is error but it no present on configuration or remote station."),pId.c_str());
		}
	    }
	    i_prm++;
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
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

    //Clear stations request counter
    for(unsigned i_st = 0; i_st < mStatWork.size(); i_st++) mStatWork[i_st].second.cntr = -1;

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    if(!prcSt) return;

    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrunReq);

    //Connection alarm clear
    alarmSet(TSYS::strMess(_("DAQ.%s: connect to data source: %s."),id().c_str(),_("STOP")),TMess::Info);
    alSt = -1;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE" && startStat())
	mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

    return true;
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
	if(&pHd[i_prm].at() == prm) break;

    if(val && i_prm >= pHd.size())	pHd.push_back(prm);
    if(!val && i_prm < pHd.size())	pHd.erase(pHd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    map<string,float>::iterator sti;
    TMdContr &cntr = *(TMdContr *)icntr;
    int64_t t_cnt, t_prev = TSYS::curTime();
    double syncCnt = 0;
    unsigned int div = 0;

    cntr.endrunReq = false;
    cntr.prcSt = true;
    bool firstCall = true;

    for(unsigned int it_cnt = 0; !cntr.endrunReq; it_cnt++) {
	if(cntr.redntUse()) { TSYS::sysSleep(STD_WAIT_DELAY*1e-3); continue; }

	cntr.call_st = true;
	t_cnt = TSYS::curTime();

	try {
	    //Allow stations presenting
	    bool isAccess = false, needEnable = false;
	    for(unsigned i_st = 0; i_st < cntr.mStatWork.size(); i_st++) {
		if(firstCall)	cntr.mStatWork[i_st].second.cntr = 0;	//Reset counter for connection alarm state update
		if(cntr.mStatWork[i_st].second.cntr > 0)
		    cntr.mStatWork[i_st].second.cntr = vmax(0,cntr.mStatWork[i_st].second.cntr-1e-6*(t_cnt-t_prev));
		if(cntr.mStatWork[i_st].second.cntr <= 0)	isAccess = true;
		if(cntr.mStatWork[i_st].second.cntr == 0)	needEnable = true;	//!!!! May be only for all == 0 stations
	    }
	    if(!isAccess) { t_prev = t_cnt; TSYS::sysSleep(1); continue; }
	    else {
		if(cntr.syncPer() > 0.1) {	//Enable sync
		    div = cntr.period() ? vmax(2,(unsigned int)(cntr.syncPer()/(1e-9*cntr.period()))) : 0;
		    if(syncCnt <= 0) syncCnt = cntr.syncPer();
		    syncCnt = vmax(0, syncCnt-1e-6*(t_cnt-t_prev));
		}
		else { div = 0; syncCnt = 1; }	//Disable sync

		//Parameters list update
		if(firstCall || needEnable || (!div && syncCnt <= 0) || (div && it_cnt > div && (it_cnt%div) == 0))
		    try { cntr.enable_(); } catch(TError err) { }

		MtxAlloc resPrm(cntr.enRes, true);

		//Mark no process
		for(unsigned i_p = 0; i_p < cntr.pHd.size(); i_p++) {
		    TMdPrm &pO = cntr.pHd[i_p].at();
		    if(!pO.isSynced || (!div && syncCnt <= 0) || (div && it_cnt > div && (((it_cnt+i_p)%div) == 0))) pO.sync();
		    pO.isPrcOK = false;
		}

		//Station's cycle
		for(unsigned i_st = 0; i_st < cntr.mStatWork.size(); i_st++) {
		    if(cntr.mStatWork[i_st].second.cntr > 0) continue;
		    XMLNode req("CntrReqs"); req.setAttr("path", "/"+cntr.mStatWork[i_st].first+"/DAQ/");
		    map<string, bool> cntrLstMA;
		    string scntr;

		    //Put attributes requests
		    for(unsigned i_p = 0; i_p < cntr.pHd.size(); i_p++) {
			TMdPrm &prm = cntr.pHd[i_p].at();
			if(prm.isPrcOK) continue;
			for(int c_off = 0; (scntr=TSYS::strSepParse(prm.stats(),0,';',&c_off)).size(); )
			{
			    if(scntr != cntr.mStatWork[i_st].first) continue;
			    string aMod	= TSYS::pathLev(prm.prmAddr(), 0);
			    string aCntr = TSYS::pathLev(prm.prmAddr(), 1);
			    cntrLstMA[aMod+"/"+aCntr] = true;

			    XMLNode *prmNd = req.childAdd("get")->setAttr("lcPs",i2s(i_p))->setAttr("path",prm.prmAddr()+"/%2fserv%2fattr");
			    prmNd->setAttr("hostTm", !cntr.restDtTm() ? "1" : "0");

			    // Prepare individual attributes list
			    bool sepReq = !prm.isEVAL && ((!div && syncCnt > 0) || (div && ((it_cnt+i_p)%div)));
			    prmNd->setAttr("sepReq", sepReq ? "1" : "0");
			    if(!cntr.restDtTm() && !sepReq) continue;

			    vector<string> listV;
			    prm.vlList(listV);
			    unsigned rC = 0;
			    for(unsigned iV = 0; iV < listV.size(); iV++) {
				AutoHD<TVal> vl = prm.vlAt(listV[iV]);
				if(sepReq && (!vl.at().arch().freeStat() || vl.at().reqFlg()))
				{
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
			for(map<string,bool>::iterator i_c = cntrLstMA.begin(); i_c != cntrLstMA.end(); ++i_c)
			{
			    int tm_grnd = cntr.mStatWork[i_st].second.lstMess[i_c->first];
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
			if(atoi(prmNd->attr("err").c_str())) continue;
			string aMod	= TSYS::pathLev(prmNd->attr("path"), 0);
			string aCntr	= TSYS::pathLev(prmNd->attr("path"), 1);
			string pId	= TSYS::pathLev(prmNd->attr("path"), 2);
			if(pId == "/serv/mess") {
			    for(unsigned i_m = 0; i_m < prmNd->childSize(); i_m++) {
				XMLNode *m = prmNd->childGet(i_m);
				SYS->archive().at().messPut(atoi(m->attr("time").c_str()), atoi(m->attr("utime").c_str()),
				    cntr.mStatWork[i_st].first+":"+m->attr("cat"), atoi(m->attr("lev").c_str()), m->text());
			    }
			    cntr.mStatWork[i_st].second.lstMess[aMod+"/"+aCntr] =
				cntr.mStatWork[i_st].second.lstMess[aMod+"/"+aCntr] ? atoi(prmNd->attr("tm").c_str()) :
										    SYS->sysTm() - 3600*cntr.restDtTm();
			}
			else {
			    TMdPrm &prm = cntr.pHd[s2i(prmNd->attr("lcPs"))].at();
			    if(prm.isPrcOK) continue;
			    prm.isPrcOK = true;
			    prm.isEVAL = false;

			    for(unsigned i_a = 0; i_a < prmNd->childSize(); i_a++) {
				XMLNode *aNd = prmNd->childGet(i_a);
				if(!prm.vlPresent(aNd->attr("id"))) continue;
				AutoHD<TVal> vl = prm.vlAt(aNd->attr("id"));

				if(aNd->name() == "el") {
				    vl.at().setS(aNd->text(),cntr.restDtTm()?atoll(aNd->attr("tm").c_str()):0,true);
				    vl.at().setReqFlg(false);
				}
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
		}

		//Mark no processed parameters to EVAL
		for(unsigned i_p = 0; i_p < cntr.pHd.size(); i_p++) {
		    TMdPrm &prm = cntr.pHd[i_p].at();
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
		    prm.vlAt("err").at().setS(_("10:Data not allow."),0,true);
		    prm.isEVAL = true;
		}

		resPrm.unlock();
	    }
	    firstCall = false;
	}
	catch(TError err)	{ mess_err(err.cat.c_str(),err.mess.c_str()); }

	//Calc acquisition process time
	t_prev = t_cnt;
	cntr.tmGath = TSYS::curTime()-t_cnt;
	cntr.call_st = false;

	TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
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
    string reqStat = TSYS::pathLev(node.attr("path"),0);

    for(unsigned i_st = 0; i_st < mStatWork.size(); i_st++)
	if(mStatWork[i_st].first == reqStat) {
	    if(mStatWork[i_st].second.cntr > 0) break;
	    try {
		node.setAttr("conTm",enableStat()?"":"1000");	//Set one second timeout to disabled controller for start procedure speed up.
		int rez = SYS->transport().at().cntrIfCmd(node,MOD_ID+id());
		if(alSt != 0) {
		    alSt = 0;
		    alarmSet(TSYS::strMess(_("DAQ.%s: connect to data source: %s."), id().c_str(), _("OK")), TMess::Info);
		}
		mStatWork[i_st].second.cntr -= 1;
		return rez;
	    }
	    catch(TError err) {
		if(alSt <= 0) {
		    alSt = 1;
		    alarmSet(TSYS::strMess(_("DAQ.%s: connect to data source '%s': %s."),
			    id().c_str(), mStatWork[i_st].first.c_str(), TRegExp(":","g").replace(err.mess,"=").c_str()));
		}
		if(call_st) mStatWork[i_st].second.cntr = mRestTm;
		throw;
	    }
	}

    return atoi(node.attr("err").c_str());
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SCHEDULE",mSched.fld().descr(),RWRWR_,"root",SDAQ_ID,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON(),NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,"help",TMess::labTaskPrior(),NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/TM_REST_DT",EVAL_STR,RWRWR_,"root",SDAQ_ID,"help",_("Zero for disable archive access."),NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SYNCPER",EVAL_STR,RWRWR_,"root",SDAQ_ID,"help",_("Zero for disable periodic sync."),NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/STATIONS",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "help",_("Remote OpenSCADA stations' identifiers list used into it controller."),NULL);
	ctrMkNode2("fld",opt,-2,"/cntr/cfg/SEL_STAT",_("Append station"),enableStat()?0:RWRW__,"root",SDAQ_ID,
	    "dest","select","select","/cntr/cfg/SEL_STAT_lst",NULL);
	ctrMkNode2("comm",opt,-2,"/cntr/cfg/host_lnk",_("Go to remote stations list configuration"),enableStat()?0:RWRW__,"root",SDAQ_ID,
	    "tp","lnk",NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/CNTRPRM",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "help",_("Remote OpenSCADA full controller's or separated controller's parameters list. Address example:\n"
		     "  System.AutoDA - for controller;\n"
		     "  System.AutoDA.UpTimeStation - for controller's parameter."),NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/CPRM_TREE",_("Parameters tree"),(enableStat()&&(!startStat()))?RWRW__:0,"root",SDAQ_ID,
	    "dest","select","select","/cntr/cfg/CPRM_lst",NULL);
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
	vector<string> list;
	SYS->transport().at().extHostList("*", list);
	for(unsigned i_l = 0; i_l < list.size(); i_l++) opt->childAdd("el")->setText(list[i_l]);
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
	for(unsigned i_st = 0; enableStat() && i_st < mStatWork.size(); i_st++) {
	    if(mStatWork[i_st].second.cntr > 0) continue;
	    XMLNode req("list");
	    req.setAttr("path", "/"+mStatWork[i_st].first+"/DAQ/%2fserv%2fPrmAttr")->
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
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : TParamContr(name,tp_prm), isPrcOK(false), isEVAL(true), isSynced(false), p_el("w_attr"),
    mPrmAddr(cfg("PRM_ADDR"))
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
    if(vlCfg())	setVlCfg(NULL);
    if(!vlElemPresent(&p_el))	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

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

    //Set EVAL to parameter attributes
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
    mStats += vl+";";
}

void TMdPrm::load_( )
{
    //Load from cache
    TParamContr::load_();

    //Restore attributes from cache
    try {
	XMLNode attrsNd;
	attrsNd.load(cfg("ATTRS").getS());
	for(unsigned i_el = 0; i_el < attrsNd.childSize(); i_el++) {
	    XMLNode *aEl = attrsNd.childGet(i_el);
	    if(vlPresent(aEl->attr("id"))) continue;
	    p_el.fldAdd(new TFld(aEl->attr("id").c_str(),aEl->attr("nm").c_str(),(TFld::Type)atoi(aEl->attr("tp").c_str()),
		atoi(aEl->attr("flg").c_str()),"","",aEl->attr("vals").c_str(),aEl->attr("names").c_str()));
	    //vlAt(aEl->attr("id")).at().setS(aEl->text());
	}
    }
    catch(TError err) { }

    //Sync attributes list
    sync();
}

void TMdPrm::save_( )
{
    //Prepare attributes cache configuration
    XMLNode attrsNd("Attrs");
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++) {
	AutoHD<TVal> vl = vlAt(ls[i_el]);
	attrsNd.childAdd("a")->setAttr("id", ls[i_el])->
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
	    for(unsigned i_a = 0; i_a < req.childGet(2)->childSize(); i_a++) {
		XMLNode *ael = req.childGet(2)->childGet(i_a);
		als.push_back(ael->attr("id"));
		if(vlPresent(ael->attr("id")))	continue;
		TFld::Type tp = (TFld::Type)atoi(ael->attr("tp").c_str());
		p_el.fldAdd( new TFld( ael->attr("id").c_str(),ael->attr("nm").c_str(),tp,
		    (atoi(ael->attr("flg").c_str())&(TFld::Selected|TFld::NoWrite|TFld::HexDec|TFld::OctDec|TFld::FullText))|TVal::DirWrite|TVal::DirRead,
		    "","",ael->attr("vals").c_str(),ael->attr("names").c_str()) );
		modif(true);
	    }

	    // Check for remove attributes
	    for(int i_p = 0; i_p < (int)p_el.fldSize(); i_p++) {
		unsigned i_l;
		for(i_l = 0; i_l < als.size(); i_l++)
		    if(p_el.fldAt(i_p).name() == als[i_l])
			break;
		if(i_l >= als.size())
		    try{ p_el.fldDel(i_p); i_p--; modif(true); }
		    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
	    }
	    isSynced = true;
	    return;
	}catch(TError err) { continue; }
}

void TMdPrm::vlGet( TVal &val )
{
    if(val.name() == "err" && (!enableStat() || !owner().startStat())) TParamContr::vlGet(val);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	vo.setI(EVAL_INT, 0, true);
    if(vl.isEVal() || vl == pvl) return;

    XMLNode req("set");

    //Send to active reserve station
    if(owner().redntUse()) {
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //Direct write
    string scntr;
    for(int c_off = 0; (scntr=TSYS::strSepParse(stats(),0,';',&c_off)).size(); )
	try {
	    req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr()+"/%2fserv%2fattr")->
		childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	    if(owner().cntrIfCmd(req))	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	}catch(TError err) { continue; }
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
		    }catch(TError err) { continue; }
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
	    else atoi(opt->text().c_str())?enable():disable();
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
	    }catch(TError err) { continue; }
	opt->setAttr("path",a_path);
    }
    else TParamContr::cntrCmdProc(opt);
}

//******************************************************
//* TMdVl                                              *
//******************************************************
TMdPrm &TMdVl::owner( )	{ return *(dynamic_cast<TMdPrm*>(nodePrev())); }

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
	    }
	    catch(TError err) { continue; }
	opt->setAttr("path",a_path);
	return;
    }

    TVal::cntrCmdProc(opt);
}


//OpenSCADA module DAQ.DAQGate file: daq_gate.cpp
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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
#define MOD_NAME	trS("Data sources gate")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"2.10.2"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("Allows to locate data sources of the remote OpenSCADA stations to local ones.")
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
    fldAdd(new TFld("PRM_BD",trS("Table of parameters cache"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",trS("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",trS("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",trS("Timeout of restore, seconds"),TFld::Integer,TFld::NoFlag,"4","10","1;1000"));
    fldAdd(new TFld("TM_REST_DT",trS("Depth time of restore data, hours"),TFld::Real,TFld::NoFlag,"6.2","1","0;12"));
    fldAdd(new TFld("WR_ASYNCH",trS("Asynchronous writing"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("GATH_MESS_LEV",trS("Level of requested messages"),TFld::Integer,TFld::Selectable,"1","1",
	"-1;0;1;2;3;4;5;6;7",_("==Disable==;Debug (0);Information (1[X]);Notice (2[X]);Warning (3[X]);Error (4[X]);Critical (5[X]);Alert (6[X]);Emergency (7[X])")));
    fldAdd(new TFld("SYNCPER",trS("Synchronization time with the remote station, seconds"),TFld::Integer,TFld::NoFlag,"4","0","-1;1000"));
    fldAdd(new TFld("STATIONS",trS("Remote station"),TFld::String,TFld::NoFlag,"100"));
    fldAdd(new TFld("CNTRPRM",trS("List of the remote controller objects and parameters"),TFld::String,TFld::FullText,"200"));
    fldAdd(new TFld("ALLOW_DEL_PA",trS("Allow the automatic removal of parameters and attributes"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("CNTR_TO_VPRM",trS("Placing different controllers to the separate virtual parameters"),TFld::Boolean,TFld::NoFlag,"1","0"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("PRM_ADDR",trS("Remote parameter address"),TFld::String,TFld::FullText|TCfg::NoVal,"100",""));
    tpPrmAt(t_prm).fldAdd(new TFld("ATTRS",trS("Attributes configuration cache"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
    tpPrmAt(t_prm).fldAdd(new TFld("STATS",trS("Presence at the stations"),TFld::String,TCfg::NoVal,"10000",""));
    //Set to read only
    //for(unsigned iSz = 0; iSz < tpPrmAt(t_prm).fldSize(); iSz++)
    //	tpPrmAt(t_prm).fldAt(iSz).setFlg(tpPrmAt(t_prm).fldAt(iSz).flg()|TFld::NoWrite);
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db ) { return new TMdContr(name, daq_db, this); }

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem), enRes(true),
    mSched(cfg("SCHEDULE")), mStat(cfg("STATIONS")), mMessLev(cfg("GATH_MESS_LEV")), mRestDtTm(cfg("TM_REST_DT").getRd()),
    mSync(cfg("SYNCPER").getId()), mRestTm(cfg("TM_REST").getId()), mPrior(cfg("PRIOR").getId()),
    mAsynchWr(cfg("WR_ASYNCH").getBd()), mAllowToDelPrmAttr(cfg("ALLOW_DEL_PA").getBd()), mPlaceCntrToVirtPrm(cfg("CNTR_TO_VPRM").getBd()),
    prcSt(false), callSt(false), syncSt(false), syncForce(false), endrunReq(false), alSt(-1), mPer(1e9)
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
	if(syncSt)	val += TSYS::strMess(_("Sync. "));
	else {
	    if(callSt)	val += TSYS::strMess(_("Acquisition. "));
	    if(period())	val += TSYS::strMess(_("Acquisition with the period %s. "),tm2s(1e-9*period()).c_str());
	    else val += TSYS::strMess(_("Next acquisition by the cron '%s'. "),atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	}
	val += TSYS::strMess(_("Spent time %s[%s]. "),
	    tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str());

	bool isWork = false;
	for(map<string,StHd>::iterator st = mStatWork.begin(); st != mStatWork.end(); ++st)
	    if(st->second.cntr > -1)
		val += TSYS::strMess(_("Error '%s', restoring in %.3g s."), st->first.c_str(), st->second.cntr);
	    else {
		int inWrBuf = 0;
		st->second.aWrRes.lock();
		for(map<string, map<string,string> >::iterator iPrm = st->second.asynchWrs.begin(); iPrm != st->second.asynchWrs.end(); ++iPrm)
		    inWrBuf += iPrm->second.size();
		st->second.aWrRes.unlock();

		val += TSYS::strMess(_("Requests to '%s' - %.6g; "), st->first.c_str(), -st->second.cntr);
		val += TSYS::strMess(_("read %g values, %g archive's, %g messages; "), st->second.numR, st->second.numRA, st->second.numRM);
		val += TSYS::strMess(_("wrote %g values, %g messages, in the buffer %d. "), st->second.numW, st->second.numWM, inWrBuf);
		isWork = true;
	    }
	if(!isWork) val.replace(0, 1, "10");
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    //TController::load_();

    //Conversion the stations list format
    if(mStat.getS().size() && mStat.getS().find("\n") != string::npos) {
	string statV, statFin;
	for(int stOff = 0; (statV=TSYS::strLine(mStat,0,&stOff)).size(); )
	    statFin += (statFin.size()?";":"") + statV;
	mStat = statFin;
    }
}

void TMdContr::enable_( )	{ sync(); }

//Remote controller-source data sync for only the parameters at <onlyPrmLs> or complete - the parameters and them attributes, in the condition:
// - loaded parameters from the cache is means synced;
// - enabling means of the parameter list sync only at syncPer() >= 0 or missing the parameters, with the present parameters mark for not synced
// - starting means of just starting in the automatic mode
//      and for the manual mode: the parameter list sync only at syncPer() >= 0 or missing the parameters, with the present parameters mark for not synced
//   - just after starting the parameters sync if performed for not synced ones (scheduled in enabling and the manual starting)
//   - the parameter list and the parameters themself is synced periodically in the syncPer() > 0
//   - the complete sync is performed at the reconnection and syncPer() >= 0
void TMdContr::sync( bool onlyPrmLs )
{
    string statV, cpEl, daqTp, cntrId, prmId, pIt, cntrPath, prmPath;
    vector<string> prmLs, prmLs1;
    map<string, vector<string> > gPrmLs;
    XMLNode req("list");

    //Stations list update
    if(!mStatWork.size())
	for(int stOff = 0; (statV=TSYS::strParse(mStat,0,";",&stOff)).size(); )
	    mStatWork[statV] = StHd();

    list(prmLs);
    bool toSync = (syncPer() >= 0 || prmLs.empty());
    prmLs.clear();

    syncSt = toSync;

    //Remote station scaning. Controllers and parameters scaning
    for(map<string,StHd>::iterator st = mStatWork.begin(); st != mStatWork.end() && toSync; ++st)
	for(int cpOff = 0; (cpEl=TSYS::strParse(cfg("CNTRPRM").getS(),0,"\n",&cpOff)).size(); )
	    try {
		// Parse parameters list
		int pOff = 0;
		daqTp  = TSYS::strParse(cpEl,0,".",&pOff);
		cntrId = TSYS::strParse(cpEl,0,".",&pOff);
		if(daqTp.empty() || cntrId.empty()) continue;

		//  Parse the parameter path
		for(prmPath = prmId = ""; (pIt=TSYS::strParse(cpEl,0,".",&pOff)).size(); ) {
		    if(prmId.size()) prmPath += "prm_"+prmId+"/";
		    prmId = pIt;
		}

		// Get the top parameters list
		prmLs.clear();
		if(!prmId.empty() && prmId != "*") prmLs.push_back(daqTp+"/"+cntrId+"/"+prmPath+"prm_"+prmId);	//Concrete parameter to the root
		else {	//Parameters group to the root
		    req.clear()->setName("get")->setAttr("path", "/"+st->first+"/DAQ/"+daqTp+"/"+cntrId+"/"+prmPath+"%2fbr%2fprm_");
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(), "%s", req.text().c_str());
		    else for(unsigned iCh = 0; iCh < req.childSize(); iCh++)
			prmLs.push_back(daqTp+"/"+cntrId+"/"+prmPath+"prm_"+req.childGet(iCh)->attr("id"));
		}

		if(messLev() == TMess::Debug)
		    mess_debug_(nodePath().c_str(), _("Enabling: station '%s' processing item '%s' for parameters %d."),
						st->first.c_str(), cpEl.c_str(), prmLs.size());

		// Processing the root parameters
		for(unsigned iP = 0; iP < prmLs.size(); iP++) {
		    //  The root parameter ID, NAME and included parameters list request
		    XMLNode *nT = req.clear()->setName("CntrReqs")->setAttr("path","/"+st->first+"/DAQ/"+prmLs[iP]);
		    nT->childAdd("get")->setAttr("path","%2fprm%2fcfg%2fSHIFR");
		    nT->childAdd("get")->setAttr("path","%2fprm%2fcfg%2fNAME");
		    nT->childAdd("get")->setAttr("path","%2fbr%2fprm_")->setAttr("recurs","1");
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
		    if(s2i(req.attr("rez")) || s2i(req.childGet(0)->attr("rez"))) continue;

		    prmId = req.childGet(0)->text();
		    AutoHD<TMdPrm> curP, curW;

		    //  Finding up for the virtual parameter of representing the remote controller object
		    if(mPlaceCntrToVirtPrm) {
			list(prmLs1);
			unsigned iP1 = 0;
			while(iP1 < prmLs1.size() && at(prmLs1[iP1]).at().prmAddr() != cntrId) iP1++;
			if(iP1 >= prmLs1.size()) {
			    while(present(cntrId) && at(cntrId).at().prmAddr().size()) cntrId = TSYS::strLabEnum(cntrId);
			    if(!present(cntrId)) add(cntrId, owner().tpPrmToId("std"));
			    curP = at(cntrId);
			    //curP.at().setName(_("DAQ-controller ")+cntrId);
			    curP.at().setPrmAddr(cntrId);
			} else curP = at(cntrId);

			if(!curP.at().enableStat()) curP.at().enable();
			else if(!onlyPrmLs) {
			    if(startStat()) curP.at().sync();
			    else curP.at().isSynced = false;
			}

			//   Placing the parameter to the virtual parameter
			curP.at().list(prmLs1);
			iP1 = 0;
			while(iP1 < prmLs1.size() && curP.at().at(prmLs1[iP1]).at().prmAddr() != prmLs[iP]) iP1++;
			if(iP1 >= prmLs1.size()) {
			    while(curP.at().present(prmId) && curP.at().at(prmId).at().prmAddr().size()) prmId = TSYS::strLabEnum(prmId);
			    if(!curP.at().present(prmId)) curP.at().add(prmId, owner().tpPrmToId("std"));
			    curP = curP.at().at(prmId);
			    curP.at().setName(req.childGet(1)->text());
			    curP.at().setPrmAddr(prmLs[iP]);
			} else curP = curP.at().at(prmId);
		    }
		    //  Finding up for the parameter directly
		    else {
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
		    }

		    curP.at().setStat(st->first);
		    gPrmLs[st->first].push_back(curP.at().ownerPath(true));
		    if(!curP.at().enableStat()) curP.at().enable();
		    else if(!onlyPrmLs) {
			if(startStat()) curP.at().sync();
			else curP.at().isSynced = false;
		    }

		    //  Processing the included parameters
		    string prmPath = prmLs[iP], prmPathW = prmPath, prmPathW_;
		    XMLNode *prmN = req.childGet(2), *prmW;
		    vector<SPrmsStack> stack;
		    for(unsigned iIp = 0; true; ) {
			if(iIp >= prmN->childSize()) {
			    //   Pop from the stack
			    if(stack.size()) {
				prmN = stack.back().nd; iIp = stack.back().pos+1; curP = stack.back().prm; prmPathW = stack.back().path;
				stack.pop_back();
				continue;
			    }
			    else break;
			}

			prmW = prmN->childGet(iIp);
			prmId = prmW->attr("id");
			prmPathW_ = prmPathW + "/prm_" + prmId;

			//   Finding for the parameter
			curP.at().list(prmLs1);
			unsigned iP1 = 0;
			while(iP1 < prmLs1.size() && curP.at().at(prmLs1[iP1]).at().prmAddr() != prmPathW_) iP1++;
			if(iP1 >= prmLs1.size()) {
			    while(curP.at().present(prmId) && curP.at().at(prmId).at().prmAddr().size()) prmId = TSYS::strLabEnum(prmId);
			    curP.at().add(prmId, owner().tpPrmToId("std"));
			    curW = curP.at().at(prmId);
			    curW.at().setName(prmW->text());
			    curW.at().setPrmAddr(prmPathW_);
			} else curW = curP.at().at(prmId);

			curW.at().setStat(st->first);
			gPrmLs[st->first].push_back(curW.at().ownerPath(true));
			if(!curW.at().enableStat()) curW.at().enable();
			else if(!onlyPrmLs) {
			    if(startStat()) curW.at().sync();
			    else curW.at().isSynced = false;
			}

			//   Next level process
			if(prmW->childSize()) {
			    stack.push_back(SPrmsStack(prmN,iIp,curP,prmPathW));
			    prmN = prmW; iIp = 0; curP = curW; prmPathW = prmPathW_;
			} else iIp++;
		    }
		}
	    } catch(TError &err) { if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), "%s", err.mess.c_str()); }

    //Processing the stations list where the parameters are available and removing for completely missed parameters remotely
    if(toSync) {
	//Checking for the parameters list and not only for the enabled-processed since the missing ones are disabled now,
	//  also removing the empty group parameters
	vector< vector<string> > prmSt;
	list(prmLs);
	prmSt.push_back(prmLs);
	AutoHD<TMdPrm> curP;
	bool isEmpty, toRemove;
	while(true) {
	    // Already processed in the depth
	    if(!prmSt.back().size()) {
		prmSt.pop_back();
		if(prmSt.size() == 0) break;	//The processing finished
		curP = (prmSt.size() == 1) ? at(prmSt.back().back()) :
					     AutoHD<TMdPrm>((TMdPrm*)curP.at().nodePrev()).at().at(prmSt.back().back());
	    }
	    // Unwrapping the item tree to the end
	    else while(true) {
		curP = (prmSt.size() == 1) ? at(prmSt.back().back()) : curP.at().at(prmSt.back().back());
		curP.at().list(prmLs);
		if(!prmLs.size()) break;
		prmSt.push_back(prmLs);
	    }

	    // Empty is mostly virtual parameters
	    isEmpty = (!curP.at().prmAddr().size() || !TSYS::pathLev(curP.at().prmAddr(),1).size());

	    // Checking of the parameter missing on correspond station
	    for(map<string,StHd>::iterator st = mStatWork.begin(); !isEmpty && st != mStatWork.end(); ++st) {
		if(st->second.cntr >= 0) continue;	//!!!! No connection with that station now
		toRemove = true;
		for(unsigned iGPrm = 0; toRemove && iGPrm < gPrmLs[st->first].size(); ++iGPrm)
		    toRemove = (gPrmLs[st->first][iGPrm] != curP.at().ownerPath(true));
		if(toRemove) curP.at().setStat(st->first, true);
	    }

	    // Checking for the station complete removing
	    for(int stOff = 0; !isEmpty && (statV=TSYS::strParse(curP.at().stats(),0,";",&stOff)).size(); )
		if(mStatWork.find(statV) == mStatWork.end())
		    curP.at().setStat(statV, true);

	    // Removing the parameters with no station
	    if(isEmpty) curP.at().list(prmLs);
	    toRemove  = mAllowToDelPrmAttr && ((!isEmpty && curP.at().stats().empty()) || (isEmpty && !prmLs.size()));

	    if(prmSt.size() == 1) curP.free();
	    else curP = AutoHD<TMdPrm>((TMdPrm*)curP.at().nodePrev());

	    if(toRemove) {
		try {
		    if(prmSt.size() == 1) del(prmSt.back().back(), NodeRemove|TParamContr::NodeRemove_NoArch);
		    else curP.at().del(prmSt.back().back(), NodeRemove|TParamContr::NodeRemove_NoArch);
		} catch(TError &err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    if(messLev() == TMess::Debug)
			mess_debug_(nodePath().c_str(),
			    _("Error deleting parameter '%s' but it is not present on the configuration or remote station."),
			    prmSt.back().back().c_str());
		}
	    }

	    // Removing from the stack back for the processed items
	    prmSt.back().pop_back();
	}

	/*//Only enabled parameters processing
	MtxAlloc res(enRes, true);
	for(unsigned iPrm = 0, iGPrm = 0; iPrm < pHd.size(); ) {
	    // Checking of the parameter missing on correspond station
	    for(map<string,StHd>::iterator st = mStatWork.begin(); st != mStatWork.end(); ++st) {
		if(st->second.cntr >= 0) continue;	//!!!! No connection with that station now
		for(iGPrm = 0; iGPrm < gPrmLs[st->first].size() && gPrmLs[st->first][iGPrm] != pHd[iPrm].at().ownerPath(true); ++iGPrm) ;
		if(iGPrm >= gPrmLs[st->first].size()) pHd[iPrm].at().setStat(st->first, true);
	    }

	    // Checking for the station complete removing
	    for(int stOff = 0; (statV=TSYS::strParse(pHd[iPrm].at().stats(),0,";",&stOff)).size(); )
		if(mStatWork.find(statV) == mStatWork.end())
		    pHd[iPrm].at().setStat(statV, true);

	    // Removing the parameters with no station
	    if(mAllowToDelPrmAttr && pHd[iPrm].at().stats().empty()) {
		string pId = pHd[iPrm].at().id();
		try {
		    TParamContr *pCntr = dynamic_cast<TParamContr*>(pHd[iPrm].at().nodePrev());
		    if(pCntr) pCntr->del(pId, NodeRemove|TParamContr::NodeRemove_NoArch);
		    else del(pId, NodeRemove|TParamContr::NodeRemove_NoArch);
		    continue;
		} catch(TError &err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),
			_("Error deleting parameter '%s' but it is not present on the configuration or remote station."),pId.c_str());
		}
	    }
	    iPrm++;
	}*/
    }

    syncSt = false;
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

    //Clearing station parameters
    for(map<string,StHd>::iterator st = mStatWork.begin(); st != mStatWork.end(); ++st) {
	st->second.cntr = -1;
	st->second.numR = st->second.numRA = st->second.numW = st->second.numRM = st->second.numWM = 0;
 
	// Asynchronous writings buffer clean up
	st->second.aWrRes.lock();
	st->second.asynchWrs.clear();
	st->second.aWrRes.unlock();
    }

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    if(!prcSt) return;

    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrunReq);

    //Connection alarm clear
    alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("STOP")), TMess::Info);
    alSt = -1;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE")
	mPer = TSYS::strParse(cron(),1," ").empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

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

    string stLs = cntr.mStat;

    for(unsigned int itCnt = 0; !cntr.endrunReq; itCnt++) {
	if(cntr.redntUse()) {
	    if(cntr.syncForce)
		try { cntr.sync(); cntr.syncForce = false; }
		catch(TError &err) { }

	    //Just using the standard message archives redundancy
	    TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
	    continue;
	}

	cntr.callSt = true;
	tCnt = TSYS::curTime();

	try {
	    //Allow stations presenting
	    bool isAccess = false, needToResync = false;
	    for(map<string,StHd>::iterator st = cntr.mStatWork.begin(); st != cntr.mStatWork.end(); ++st) {
		if(firstCall)	st->second.cntr = 0;	//Reset counter for connection alarm state update
		if(st->second.cntr > 0) st->second.cntr = vmax(0, st->second.cntr-1e-6*(tCnt-tPrev));
		if(st->second.cntr <= 0) isAccess = true;
		if(st->second.cntr == 0) needToResync = true;	//?!?! Maybe only for all == 0 stations
	    }
	    if(!isAccess) { tPrev = tCnt; TSYS::taskSleep(cntr.period()?cntr.period():1e9); continue; }
	    else {
		if(cntr.syncPer() > 0) {	//Enable sync
		    div = cntr.period() ? vmax(2,(unsigned int)(cntr.syncPer()/(1e-9*cntr.period()))) : 0;
		    if(syncCnt <= 0) syncCnt = cntr.syncPer();
		    syncCnt = vmax(0, syncCnt-1e-6*(tCnt-tPrev));
		} else { div = 0; syncCnt = 1; }	//Disable sync

		//Parameters list update
		if(cntr.syncForce || (!firstCall && needToResync) || (!div && syncCnt <= 0) || (div && itCnt > div && (itCnt%div) == 0))
		    try {
			cntr.syncSt = true;
			cntr.sync(!(cntr.syncForce || needToResync));
			cntr.syncForce = false;
		    } catch(TError &err) { }

		MtxAlloc resPrm(cntr.enRes, true);

		//Mark no process
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
		    TMdPrm &pO = cntr.pHd[iP].at();
		    if(!pO.isSynced || (!div && syncCnt <= 0) || (div && itCnt > div && (((itCnt+iP)%div) == 0))) {
			cntr.syncSt = true;
			pO.sync();
		    }
		    pO.isPrcOK = false;
		}

		cntr.syncSt = false;

		//Acquisition cycle of the stations
		//for(map<string,StHd>::iterator st = cntr.mStatWork.begin(); st != cntr.mStatWork.end(); ++st) {
		string statV;
		bool someLive = false;
		for(int stOff = 0; (statV=TSYS::strParse(stLs,0,";",&stOff)).size(); ) {
		    map<string,StHd>::iterator st = cntr.mStatWork.find(statV);
		    if(st == cntr.mStatWork.end() || st->second.cntr > 0) continue;

		    StHd &stO = st->second;
		    XMLNode req("CntrReqs"); req.setAttr("path", "/"+st->first+"/DAQ/");

		    // Writing the asynchronous writings' buffer
		    if(stO.asynchWrs.size()) {
			//  Getting the buffer to process
			MtxAlloc resAsWr(stO.aWrRes, true);
			map<string, map<string,string> > aWrs = stO.asynchWrs;
			stO.asynchWrs.clear();
			resAsWr.unlock();

			//  Processing
			int vlToWr = 0, mToWr = 0;
			for(map<string, map<string,string> >::iterator iPrm = aWrs.begin(); iPrm != aWrs.end(); ++iPrm) {
			    //   Ready requests processing
			    if(iPrm->first == "<ReadyReqs>") {
				for(map<string,string>::iterator iReq = iPrm->second.begin(); iReq != iPrm->second.end(); ++iReq, ++mToWr)
				    req.childAdd(iReq->second)->load(iReq->first);
				continue;
			    }

			    //   Same parameters
			    req.childAdd("set")->setAttr("path", iPrm->first+"/%2fserv%2fattr");
			    for(map<string,string>::iterator iAttr = iPrm->second.begin(); iAttr != iPrm->second.end(); ++iAttr, ++vlToWr)
				req.childGet(-1)->childAdd("el")->setAttr("id",iAttr->first)->setText(iAttr->second);
			}

			// Same request
			try {
			    if(!cntr.cntrIfCmd(req)) { aWrs.clear(); stO.numW += vlToWr; stO.numWM += mToWr; }
			    else if(cntr.messLev() == TMess::Debug) mess_debug_(cntr.nodePath().c_str(), "%s", req.text().c_str());
			} catch(TError&) { }

			req.childClear();

			// Return back not wrote values
			if(aWrs.size()) {
			    resAsWr.lock();
			    if(stO.asynchWrs.empty()) stO.asynchWrs = aWrs;
			    else {
				for(map<string, map<string,string> >::iterator iPrm = aWrs.begin(); iPrm != aWrs.end(); ++iPrm)
				    if(stO.asynchWrs.find(iPrm->first) == stO.asynchWrs.end()) stO.asynchWrs[iPrm->first] = iPrm->second;
				    else {
					for(map<string,string>::iterator iAttr = iPrm->second.begin(); iAttr != iPrm->second.end(); ++iAttr)
					    if(stO.asynchWrs[iPrm->first].find(iAttr->first) == stO.asynchWrs[iPrm->first].end())
						stO.asynchWrs[iPrm->first][iAttr->first] = iAttr->second;
				    }
			    }
			    resAsWr.unlock();
			}
		    }

		    if(stO.cntr > 0) continue;
		    map<string, bool> cntrLstMA;
		    string scntr;

		    //Put attributes requests
		    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
			TMdPrm &prm = cntr.pHd[iP].at();
			if(prm.isPrcOK) continue;
			for(int cOff = 0; (scntr=TSYS::strParse(prm.stats(),0,";",&cOff)).size(); ) {
			    if(scntr != st->first) continue;
			    string aMod	= TSYS::pathLev(prm.prmAddr(), 0);
			    string aCntr = TSYS::pathLev(prm.prmAddr(), 1);
			    cntrLstMA[aMod+"/"+aCntr] = true;

			    XMLNode *prmNd = req.childAdd("get")->setAttr("lcPs",i2s(iP))->setAttr("path",prm.prmAddr()+"/%2fserv%2fattr");
			    prmNd->setAttr("hostTm", !cntr.restDtTm() ? "1" : "");

			    // Prepare individual attributes list
			    bool sepReq = !prm.isEVAL && ((!div && syncCnt > 0) || (div && ((itCnt+iP)%div)));
			    prmNd->setAttr("sepReq", sepReq ? "1" : "");
			    if(!cntr.restDtTm() && !sepReq) continue;

			    vector<string> listV;
			    prm.vlList(listV);
			    unsigned rC = 0;
			    for(unsigned iV = 0; iV < listV.size(); iV++) {
				if(listV[iV] == "SHIFR") continue;
				AutoHD<TVal> vl = prm.vlAt(listV[iV]);
				if(sepReq && (!vl.at().arch().freeStat() || vl.at().reqFlg())) {
				    prmNd->childAdd("el")->setAttr("id",listV[iV]);
				    rC++;
				}
				if(!vl.at().arch().freeStat())
				    prmNd->childAdd("ael")->setAttr("id", listV[iV])->
					setAttr("tm", ll2s(vmax(vl.at().arch().at().end(""),TSYS::curTime()-(int64_t)(3.6e9*cntr.restDtTm()))));
			    }
			    if(sepReq && !prmNd->childSize()) { req.childDel(prmNd); prm.isPrcOK = true; }	//Pass request and mark by processed
			    if(sepReq && rC > listV.size()/2) {
				prmNd->childClear("el");
				prmNd->setAttr("sepReq", "");
			    }
			}
		    }

		    //Messages requesting for ones of the remote controller in the stages
		    //1. Active alarms requesting, for "lev" < 0, "tm_grnd" = 0
		    //2. History (no alarms set) messages requesting, from "tm_grnd" = cntr.restDtTm() and to "tm" = the last message time of stage 1
		    //3... Actual messages requesting, from "tm_grnd" = the last message time
		    if(cntr.mMessLev.getI() >= 0)	//Else disabled
			for(map<string,bool>::iterator iC = cntrLstMA.begin(); iC != cntrLstMA.end(); ++iC) {
			    int tm_grnd = stO.lstMess[iC->first].time;
			    XMLNode *reqCh = req.childAdd("get")->
						setAttr("path", "/"+iC->first+"/%2fserv%2fmess")->
						// Limiting for depth of the requesting messages up to the restoring time or one hour
						setAttr("tm_grnd", i2s(vmax(SYS->sysTm()-3600*vmax(1,cntr.restDtTm()),tm_grnd)));

			    //Alarms force request
			    if(!tm_grnd && cntr.mMessLev.getI() >= 0)
				reqCh->setAttr("lev", i2s(-cntr.mMessLev.getI()))->setAttr("tm_grnd", "");
			    //Normal request
			    else {
				reqCh->setAttr("lev", cntr.mMessLev.getS());
				// First initial request
				if(stO.lstMess[iC->first].categ.empty() && !stO.lstMess[iC->first].mess.empty())
				    reqCh->setAttr("tm", stO.lstMess[iC->first].mess);
			    }
			}

		    if(!(req.childSize() || someLive)) continue;	//Polling also the redundant station for live

		    //Same request
		    try {
			if(cntr.cntrIfCmd(req)) {
			    if(cntr.messLev() == TMess::Debug) mess_debug_(cntr.nodePath().c_str(), "%s", req.text().c_str());
			    continue;
			}
		    } catch(TError&) { continue; }

		    someLive = true;

		    //Result process
		    for(unsigned iR = 0; iR < req.childSize(); ++iR) {
			XMLNode *prmNd = req.childGet(iR);
			if(s2i(prmNd->attr("err"))) continue;
			string aMod	= TSYS::pathLev(prmNd->attr("path"), 0);
			string aCntr	= TSYS::pathLev(prmNd->attr("path"), 1);
			string pId	= TSYS::pathLev(prmNd->attr("path"), 2);
			string tVl;
			if(pId == "/serv/mess") {
			    TMess::SRec &lstRdMess = stO.lstMess[aMod+"/"+aCntr];
			    TMess::SRec lstRdMess_;
			    vector<TMess::SRec> mess;
			    for(unsigned iM = 0; iM < prmNd->childSize(); iM++) {
				XMLNode *m = prmNd->childGet(iM);
				mess.push_back(TMess::SRec(s2i(m->attr("time")),s2i(m->attr("utime")),
						    st->first+":"+m->attr("cat"),s2i(m->attr("lev")),m->text()));
				if(mess.back().time >= lstRdMess_.time) {
				    lstRdMess_ = mess.back();
				    if(lstRdMess_ == lstRdMess) mess.pop_back();
				}
			    }

			    // The bottom time border processing
			    bool isHistReq = false;
			    if(!lstRdMess.time)
				lstRdMess = TMess::SRec(SYS->sysTm()-3600*cntr.restDtTm(), 0, "", 0, cntr.restDtTm()?prmNd->attr("tm"):"");
			    else {
				//  First initial request for not active alarms
				if((isHistReq=(lstRdMess.categ.empty() && !lstRdMess.mess.empty()))) lstRdMess.mess = "";

				if(lstRdMess_.time > lstRdMess.time) lstRdMess = lstRdMess_;
				else if(lstRdMess_.time) lstRdMess = TMess::SRec(lstRdMess_.time+1);

				if(mess_lev() == TMess::Debug)
				    mess_debug(cntr.nodePath().c_str(), "Gate proceeding for '%s': %s: %d",
					(aMod+"/"+aCntr).c_str(), TSYS::atime2str(stO.lstMess[aMod+"/"+aCntr].time).c_str(), prmNd->childSize());
			    }

			    SYS->archive().at().messPut(mess, isHistReq?ARCH_NOALRM:"");	//No alarms setting at the initial
			    stO.numRM += mess.size();
			}
			else {
			    TMdPrm &prm = cntr.pHd[s2i(prmNd->attr("lcPs"))].at();
			    if(prm.isPrcOK) continue;
			    prm.isPrcOK = true;
			    prm.isEVAL = false;

			    for(unsigned iA = 0; iA < prmNd->childSize(); iA++) {
				XMLNode *aNd = prmNd->childGet(iA);
				tVl = aNd->attr("id");
				if(tVl == "SHIFR")	continue;

				if(!prm.vlPresent(tVl)) continue;
				AutoHD<TVal> vl = prm.vlAt(tVl);
				if(aNd->name() == "el") {
				    vl.at().setS(aNd->text(), cntr.restDtTm()?atoll(aNd->attr("tm").c_str()):0, true);
				    vl.at().setReqFlg(false);
				    stO.numR++;
				}
				else if(aNd->name() == "ael" && !vl.at().arch().freeStat() && aNd->childSize()) {
				    int64_t btm = atoll(aNd->attr("tm").c_str());
				    int64_t per = atoll(aNd->attr("per").c_str());
				    TValBuf buf(vl.at().arch().at().valType(), 0, per, false, true);
				    for(unsigned iV = 0; iV < aNd->childSize(); iV++) {
					buf.setS(aNd->childGet(iV)->text(), btm+per*iV);
					stO.numRA++;
				    }
				    vl.at().arch().at().setVals(buf, buf.begin(), buf.end(), "");
				}
			    }
			}
		    }
		}

		//Mark not processed parameters as EVAL
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
		    TMdPrm &prm = cntr.pHd[iP].at();
		    if((someLive && prm.isPrcOK) || prm.isEVAL) continue;
		    vector<string> vLs;
		    prm.elem().fldList(vLs);
		    for(unsigned iV = 0; iV < vLs.size(); iV++) {
			if(vLs[iV] == "SHIFR" || vLs[iV] == "NAME" || vLs[iV] == "DESCR") continue;
			AutoHD<TVal> vl = prm.vlAt(vLs[iV]);
			if(vl.at().getS() == EVAL_STR) continue;
			vl.at().setS(EVAL_STR, vl.at().arch().freeStat() ? 0 :
			    vmin(TSYS::curTime(),vmax(vl.at().arch().at().end(""),TSYS::curTime()-(int64_t)(3.6e9*cntr.restDtTm()))), true);
		    }
		    prm.vlAt("err").at().setS(_("10:Data not available."),0,true);
		    prm.isEVAL = true;
		}

		resPrm.unlock();
	    }
	} catch(TError &err)	{ /*mess_err(err.cat.c_str(),err.mess.c_str());*/ }

	//Calc acquisition process time
	tPrev = tCnt;
	cntr.callSt = firstCall = false;

	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prcSt = false;

    return NULL;
}

string TMdContr::catsPat( )
{
    string curPat = TController::catsPat();

    string statV, stLs = mStat;
    for(int stOff = 0; (statV=TSYS::strParse(stLs,0,";",&stOff)).size(); )
	curPat += "|^"+statV+":";

    return curPat;
}

void TMdContr::messSet( const string &mess, int lev, const string &type2Code, const string &prm, const string &cat )
{
    TController::messSet(mess, lev, type2Code, prm, cat);

    if(mMessLev.getI() < 0 || prm.empty() || type2Code.size() < 2) return;

    AutoHD<TMdPrm> sprm;
    if((sprm=SYS->daq().at().prmAt(DAQPath()+"."+prm,'.',true)).freeStat() && (sprm=nodeAt(prm,0,'.',0,true)).freeStat())
	return;

    //Sending the message to the remote stations
    string scntr;
    for(int cOff = 0; (scntr=TSYS::strParse(sprm.at().stats(),0,";",&cOff)).size(); )
	try {
	    map<string,TMdContr::StHd>::iterator st = mStatWork.find(scntr);
	    if(st == mStatWork.end())	continue;

	    int tOff = 0; TSYS::pathLev(sprm.at().prmAddr(), 1, true, NULL, &tOff);
	    string  dModCntr = sprm.at().prmAddr().substr(0, tOff),
		    dPrm = sprm.at().prmAddr().substr(tOff);
	    XMLNode req("set");
	    req.setAttr("path", "/"+scntr+"/DAQ/"+dModCntr+"%2fserv%2fmess")->
		setAttr("type2Code", type2Code.substr(0,type2Code.size()-2)+(SYS->prjNm().size()?SYS->prjNm():SYS->id())+"("+id()+"):"+type2Code.substr(type2Code.size()-2))->
		setAttr("lev", i2s(lev))->setAttr("cat", cat)->setAttr("prm", TSYS::path2sepstr(dPrm))->
		setText(mess);

	    if(mAsynchWr) {
		st->second.aWrRes.lock();
		req.setAttr("path", dModCntr+"%2fserv%2fmess");
		st->second.asynchWrs["<ReadyReqs>"][req.save()] = "set";
		st->second.aWrRes.unlock();
	    }
	    else {
		if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),req.text().c_str());
		st->second.numWM++;
	    }
	} catch(TError &err) { continue; }
}

int TMdContr::cntrIfCmd( XMLNode &node, bool noConnect )
{
    string reqStat = TSYS::pathLev(node.attr("path"), 0);

    bool stPresent = false;
    for(map<string,StHd>::iterator st = mStatWork.begin(); st != mStatWork.end(); ++st)
	if(st->first == reqStat) {
	    MtxAlloc reqSt(st->second.reqM, true);
	    if(st->second.cntr > 0 || (st->second.cntr > -1 && noConnect)) break;
	    stPresent = true;
	    try {
		node.setAttr("conTm", enableStat()?"":"1000");	//Set one second timeout to disabled controller for start procedure speed up.
		int rez = SYS->transport().at().cntrIfCmd(node, MOD_ID+id());
		if(alSt != 0) {
		    alSt = 0;
		    alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("OK")), TMess::Info);
		}
		st->second.cntr -= 1;
		return rez;
	    } catch(TError &err) {
		if(alSt <= 0) {
		    alSt = 1;
		    alarmSet(TSYS::strMess(_("Connection to the data source '%s': %s."),
						st->first.c_str(), TRegExp(":","g").replace(err.mess,"=").c_str()));
		}
		if(callSt) st->second.cntr = mRestTm;
		throw;
	    }
	}
    if(!stPresent) node.setAttr("err", i2s(TError::Tr_UnknownHost)+":"+TSYS::strMess(_("Station missed '%s'."),reqStat.c_str()));

    return s2i(node.attr("err"));
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/runSt",EVAL_STR,RWRWR_,"root",SDAQ_ID,1,
	    "help",_("Manual restart of the enabled controller object causes the force resync at the sync period >= 0.\n"
		"Restart to refresh the removed source data configuration."));
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,RWRWR_,"root",SDAQ_ID,
	    "dest","sel_ed", "sel_list",TMess::labSecCRONsel().c_str(), "help",TMess::labSecCRON().c_str(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID, "help",TMess::labTaskPrior().c_str(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/TM_REST_DT",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero to disable the access to the remote archive."), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/WR_ASYNCH",EVAL_STR,RWRWR_,"root",SDAQ_ID,
	    "help",_("Enables of writing in the common acquisition cycle for the buffer of accumulated values, by one request.\n"
		    "Useful for slow and not reliable networks-connections since the writing buffer stores data before the connection restore and the real writing."), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SYNCPER",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero to disable the periodic sync and -1 for the sync disabling at the start also."), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/STATIONS",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "dest","select", "select","/cntr/cfg/SEL_STAT_lst",
	    "help",_("List of remote OpenSCADA station IDs used in this controller."), "rows", "2", NULL);
	ctrMkNode2("comm",opt,-4,"/cntr/cfg/host_lnk",_("Go to configuration of the remote stations list"),enableStat()?0:RWRW__,"root",SDAQ_ID,
	    "tp","lnk", NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/CNTRPRM",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "help",_("List of remote OpenSCADA full controller objects or individual controller parameters. Address example:\n"
		     "  System.AutoDA - for a controller object;\n"
		     "  System.AutoDA.UpTimeStation - for a controller parameter."), NULL);
	ctrMkNode2("fld",opt,-3,"/cntr/cfg/CPRM_TREE",_("Parameters tree"),(enableStat()&&(!startStat()))?RWRW__:0,"root",SDAQ_ID,
	    "dest","select", "select","/cntr/cfg/CPRM_lst", NULL);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/st/runSt" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR) && s2i(opt->text()) && enableStat()) {
	syncForce = true;
	start();
    }
    else if(a_path == "/cntr/cfg/SEL_STAT_lst" && ctrChkNode(opt)) {
	vector<TTransportS::ExtHost> list;
	SYS->transport().at().extHostList("*", list);
	for(unsigned iL = 0; iL < list.size(); iL++)
	    if(!TRegExp("(^|;)"+list[iL].id+"(;|$)","m").test(mStat))
		opt->childAdd("el")->setText((mStat.getS().size()?mStat.getS()+";":"")+list[iL].id);
	opt->childAdd("el")->setText("");
    }
    else if(a_path == "/cntr/cfg/CPRM_TREE") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) opt->setText(TBDS::genPrmGet(owner().nodePath()+"selCPRM","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	    if(opt->text() == _("<<Append current>>")) {
		string vLs = cfg("CNTRPRM"), vS, setVl = TBDS::genPrmGet(owner().nodePath()+"selCPRM","",opt->attr("user"));
		for(int off = 0; (vS=TSYS::strLine(vLs,0,&off)).size(); )
		    if(TSYS::strParse(vS,0,":") == setVl)
			break;
		if(vS.empty()) cfg("CNTRPRM") = vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+setVl;
	    }
	    else TBDS::genPrmSet(owner().nodePath()+"selCPRM", TSYS::pathLev(opt->text(),0), opt->attr("user"));
	}
    }
    else if(a_path == "/cntr/cfg/CPRM_lst" && ctrChkNode(opt))
	for(map<string,StHd>::iterator st = mStatWork.begin(); enableStat() && st != mStatWork.end(); ++st) {
	    if(st->second.cntr > 0) continue;
	    XMLNode req("list");
	    req.setAttr("path", "/"+st->first+"/DAQ/%2fserv%2fPrmAttr")->
		setAttr("base", TBDS::genPrmGet(owner().nodePath()+"selCPRM","",opt->attr("user")))->
		setAttr("toPrm","1")->setAttr("sep",".");
	    if(cntrIfCmd(req) == 0) {
		*opt = req;
		opt->setName("get")->childIns(0,"el")->setText(_("<<Append current>>"));
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

TCntrNode &TMdPrm::operator=( const TCntrNode &node )
{
    TParamContr::operator=(node);

    mPrmAddr = "";

    return *this;
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    //if(vlCfg())	setVlCfg(NULL);		//!!!! To prevent SHIFR and other losses at the parameter restore, by lock, after it deletion try
    if(!vlElemPresent(&pEl))	vlElemAtt(&pEl);
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    loadIO();

    TParamContr::enable();

    owner().prmEn(this, true);	//Put to process

    if(owner().syncPer() >= 0) {
	if(!owner().startStat()) isSynced = false;	//Schedule the sync at running
	else if(!isSynced) sync();			//Sync for the attributes list
    }
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    owner().prmEn(this, false);	//Remove from process

    TParamContr::disable();

    //Set EVAL to the parameter attributes
    /*vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);*/
}

void TMdPrm::setStat( const string &vl, bool toRemove )
{
    if(vl.empty()) {
	if(toRemove && mStats.getS().size()) { mStats = ""; modif(); }
	return;
    }

    string scntrs, scntr;
    for(int off = 0; (scntr=TSYS::strParse(mStats,0,";",&off)).size(); )
	if(scntr != vl) scntrs += (scntrs.size()?";":"") + scntr;
    if(!toRemove) scntrs += (scntrs.size()?";":"") + vl;

    if(scntrs != mStats.getS()) modif();

    mStats = scntrs;
}

void TMdPrm::load_( )
{
    //TParamContr::load_();

    loadIO();
}

void TMdPrm::loadIO( )
{
    //Restoring the attributes from the cache
    try {
	XMLNode attrsNd;
	attrsNd.load(cfg("ATTRS").getS());
	for(unsigned iEl = 0; iEl < attrsNd.childSize(); iEl++) {
	    XMLNode *aEl = attrsNd.childGet(iEl);
	    if(vlPresent(aEl->attr("id"))) continue;
	    pEl.fldAdd(new TFld(aEl->attr("id").c_str(),aEl->attr("nm").c_str(),(TFld::Type)s2i(aEl->attr("tp")),
		s2i(aEl->attr("flg")),"","",aEl->attr("vals").c_str(),aEl->attr("names").c_str()));
	    vlAt("err").at().setS(_("10:Data not available."), 0, true);
	    //vlAt(aEl->attr("id")).at().setS(aEl->text());
	}
	if(attrsNd.childSize())	isSynced = true;
    } catch(TError &err) { }
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

    //Save to the cache
    TParamContr::save_();
}

void TMdPrm::sync( )
{
    bool toDisable = false;
    int rez = 0;
    //Request and update attributes list
    string scntr;
    XMLNode req("CntrReqs");
    for(int cOff = 0; (scntr=TSYS::strParse(stats(),0,";",&cOff)).size(); )
	try {
	    vector<string> als;
	    req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr());
	    req.childAdd("get")->setAttr("path","%2fprm%2fcfg%2fNAME");
	    req.childAdd("get")->setAttr("path","%2fprm%2fcfg%2fDESCR");
	    req.childAdd("list")->setAttr("path","%2fserv%2fattr");
	    if((rez=owner().cntrIfCmd(req)) == TError::Tr_Connect || rez == TError::Tr_UnknownHost)
		throw TError(req.attr("mcat").c_str(), req.text().c_str());
	    else if(rez || s2i(req.attr("rez")) || s2i(req.childGet(0)->attr("rez"))) {
		toDisable = true;
		break; /*continue;*/	//!!!!: No more sense in continue parameters processing which to be disabled
	    }

	    setName(req.childGet(0)->text());
	    setDescr(req.childGet(1)->text());

	    // Check and create new attributes
	    for(unsigned iA = 0; iA < req.childGet(2)->childSize(); iA++) {
		XMLNode *ael = req.childGet(2)->childGet(iA);
		als.push_back(ael->attr("id"));

		if(vlPresent(ael->attr("id")))	continue;
		TFld::Type tp = (TFld::Type)s2i(ael->attr("tp"));
		pEl.fldAdd(new TFld(ael->attr("id").c_str(),ael->attr("nm").c_str(),tp,
		    (s2i(ael->attr("flg"))&(TFld::Selectable|TFld::NoWrite|TFld::HexDec|TFld::OctDec|TFld::FullText))|TVal::DirWrite|TVal::DirRead,
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

    //!!!! Disable the missed parameters on the remote hosts
    if(toDisable && !isSynced) {
	//mess_warning(nodePath().c_str(), _("The parameter disabling in the reason '%d: %s' ..."), rez, req.save().c_str());
	disable();
    }
}

void TMdPrm::vlGet( TVal &vl )
{
    if(!enableStat() || !owner().startStat()) {
	if(vl.name() == "err") TParamContr::vlGet(vl);
	else vl.setI(EVAL_INT, 0, true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setI(EVAL_INT, 0, true); return; }
    if(vl.isEVal() || vl == pvl) return;

    //Send to active reserve station
    if(vlSetRednt(vo,vl,pvl))	return;

    string scntr;

    for(int cOff = 0; (scntr=TSYS::strParse(stats(),0,";",&cOff)).size(); ) {
	map<string,TMdContr::StHd>::iterator st = owner().mStatWork.find(scntr);
	if(st == owner().mStatWork.end())	continue;

	//Registering for the later common writing in the asynchronous mode and pass updating the just writed values
	if(owner().mAsynchWr) {
	    st->second.aWrRes.lock();
	    st->second.asynchWrs[prmAddr()][vo.name()] = vl.getS();
	    st->second.aWrRes.unlock();
	}
	//Direct write
	else try {
	    XMLNode req("set");
	    req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr()+"/%2fserv%2fattr")->
		childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	    if(owner().cntrIfCmd(req))	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	    st->second.numW++;
	} catch(TError &err) { continue; }
    }
}

bool TMdPrm::cfgChange( TCfg &co, const TVariant &pc )
{
    TParamContr::cfgChange(co, pc);

    if(enableStat() && owner().startStat() && co.getS() != pc.getS() && (co.fld().name() == "NAME" || co.fld().name() == "DESCR")) {
	XMLNode req("set");

	//Send to active reserve station
	if(owner().redntUse()) {
	    req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",co.fld().name())->setText(co.getS());
	    SYS->daq().at().rdStRequest(owner().workId(), req);
	}
	//Direct write
	else {
	    string scntr;
	    for(int cOff = 0; (scntr=TSYS::strParse(stats(),0,";",&cOff)).size(); )
		try {
		    req.clear()->setAttr("path",scntr+"/DAQ/"+prmAddr()+"/%2fserv%2fattr")->
			childAdd("el")->setAttr("id",co.fld().name())->setText(co.getS());
		    if(owner().cntrIfCmd(req))	throw TError(req.attr("mcat").c_str(), req.text().c_str());
		} catch(TError &err) { continue; }
	}
    }

    return true;
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat())	return;
    val.arch().at().setSrcMode(TVArchive::DAQAttr);
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

TVal* TMdPrm::vlNew( )	{ return new TMdVl(); }

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path.find("/serv/") == 0) { TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	bool isEmpty = (!prmAddr().size() || !TSYS::pathLev(prmAddr(),1).size());

	TParamContr::cntrCmdProc(opt);
	ctrRemoveNode(opt, "/prm");
	if(isEmpty) { ctrRemoveNode(opt,"/val"); ctrRemoveNode(opt, "/arch"); }

	if(ctrMkNode("area",opt,0,"/prm",_("Parameter"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State of the local representative parameter"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		if(owner().enableStat())
		    ctrMkNode("fld",opt,-1,"/prm/st/en",_("Enabled"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/id",_("Identifier"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/nm",_("Name"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/stats",_("Stations"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/addr",_("Address in remote source"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
	    }
	    XMLNode *cfgN = ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"));
	    if(cfgN && !isEmpty) {
		// Get remote parameter's config section
		string scntr;
		XMLNode req("info");
		for(int cOff = 0; (scntr=TSYS::strParse(stats(),0,";",&cOff)).size(); )
		    try {
			req.clear()->setAttr("path", scntr+"/DAQ/"+prmAddr()+"/%2fprm%2fcfg");
			if(owner().cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),req.text().c_str());
			break;
		    } catch(TError &err) { continue; }
		if(req.childSize()) {
		    *cfgN = *req.childGet(0);
		    cfgN->setAttr("dscr",_("Configuration of the remote parameter"));
		}
	    }
	    if(isEmpty && cfgN) ctrRemoveNode(opt, "/prm/cfg");
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
    else if(a_path == "/prm/st/stats" && ctrChkNode(opt)) opt->setText(mStats.getS());
    else if(a_path == "/prm/st/addr" && ctrChkNode(opt))  opt->setText(mPrmAddr.getS());
    else if(a_path == "/prm/cfg/SHIFR" || a_path == "/prm/cfg/NAME" || a_path == "/prm/cfg/DESCR")
	TParamContr::cntrCmdProc(opt);
    else if(a_path.compare(0,4,"/prm") == 0) {
	//Request to remote host
	string scntr;
	for(int cOff = 0; (scntr=TSYS::strParse(stats(),0,";",&cOff)).size(); )
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
	// Requesting the remote station
	string scntr;
	for(int cOff = 0; (scntr=TSYS::strParse(owner().stats(),0,";",&cOff)).size(); )
	    try {
		opt->setAttr("path",scntr+"/DAQ/"+owner().prmAddr()+"/a_"+name()+"/"+TSYS::strEncode(a_path,TSYS::PathEl));
		if(!owner().owner().cntrIfCmd(*opt,true)) break;
	    } catch(TError &err) { continue; }
	opt->setAttr("path", a_path);
	return;
    }

    TVal::cntrCmdProc(opt);
}

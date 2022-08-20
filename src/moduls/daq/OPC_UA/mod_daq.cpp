
//OpenSCADA module DAQ.OPC_UA file: mod_daq.cpp
/***************************************************************************
 *   Copyright (C) 2009-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include "mod_daq.h"

OPC_UA::TTpContr *OPC_UA::mod;	//Pointer for direct access to DAQ module

using namespace OPC_UA;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(DAQ_ID)
{
    mod = this;

    modInfoMainSet(DAQ_NAME, DAQ_TYPE, DAQ_MVER, DAQ_AUTOR, DAQ_DESCR, DAQ_LICENSE, name);
}

TTpContr::~TTpContr( )	{ }

void TTpContr::load_( )	{ }

void TTpContr::save_( )	{ }

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",trS("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PRM_BD_L",trS("Logical parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",trS("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",trS("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",trS("Restore timeout, seconds"),TFld::Integer,TFld::NoFlag,"4","10","1;3600"));
    fldAdd(new TFld("SYNCPER",trS("Sync inter remote station period, seconds"),TFld::Integer,TFld::NoFlag,"4","60","0;1000"));
    fldAdd(new TFld("EndPoint",trS("End point"),TFld::String,TFld::NoFlag,"50","opc.tcp://localhost"));
    fldAdd(new TFld("SecPolicy",trS("Security policy"),TFld::String,TFld::Selectable,"20","None","None;Basic128Rsa15;Basic256",_("None;Basic128Rsa15;Basic256")));
    fldAdd(new TFld("SecMessMode",trS("Message security mode"),TFld::Integer,TFld::Selectable,"1","1",
	TSYS::strMess("%d;%d;%d",MS_None,MS_Sign,MS_SignAndEncrypt).c_str(),_("None;Sign;Sign&Encrypt")));
    fldAdd(new TFld("Cert",trS("Certificate (PEM)"),TFld::String,TFld::FullText,"10000"));
    fldAdd(new TFld("PvKey",trS("Private key (PEM)"),TFld::String,TFld::FullText,"10000"));
    fldAdd(new TFld("AuthUser",trS("Auth: user"),TFld::String,TFld::NoFlag,"20"));
    fldAdd(new TFld("AuthPass",trS("Auth: password"),TFld::String,TFld::NoFlag,"20"));
    fldAdd(new TFld("UseRead",trS("Use the \"Read\" function"),TFld::Boolean,TFld::NoFlag,"1","1"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("ND_LS",trS("Nodes list"),TFld::String,TFld::FullText|TCfg::NoVal,"1000",""));
    // Logical parameter type by the DAQ parameter template
    t_prm = tpParmAdd("logic", "PRM_BD_L", _("Logical"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("TMPL",trS("Parameter template"),TFld::String,TCfg::NoVal,"50",""));
    // Parameter template IO DB structure
    elPrmIO.fldAdd(new TFld("PRM_ID",trS("Parameter ID"),TFld::String,TCfg::Key,i2s(limObjID_SZ).c_str()));
    elPrmIO.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,i2s(limObjID_SZ*1.5).c_str()));
    elPrmIO.fldAdd(new TFld("VALUE",trS("Value"),TFld::String,TFld::NoFlag,"200"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) : TController(name_c,daq_db,cfgelem), enRes(true),
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mSync(cfg("SYNCPER")),
    mEndP(cfg("EndPoint")), mSecPol(cfg("SecPolicy")), mSecMessMode(cfg("SecMessMode")), mCert(cfg("Cert")), mPvKey(cfg("PvKey")),
    mAuthUser(cfg("AuthUser")), mAuthPass(cfg("AuthPass")), restTm(cfg("TM_REST").getId()), mUseRead(cfg("UseRead").getBd()),
    mPer(1e9), prcSt(false), callSt(false), alSt(-1), mBrwsVar(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder)),
    acqErr(dataRes()), tmDelay(0), servSt(0)
{
    cfg("PRM_BD").setS("OPC_UA_Prm_"+id());
    cfg("PRM_BD_L").setS("OPC_UA_PrmL_"+id());
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

void TMdContr::postDisable( int flag )
{
    try {
	if(flag&(NodeRemove|NodeRemoveOnlyStor))
	    TBDS::dataDelTbl(DB(flag&NodeRemoveOnlyStor)+"."+cfg("PRM_BD_L").getS()+"_io",
				owner().nodePath()+cfg("PRM_BD_L").getS()+"_io");
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    TController::postDisable(flag);
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	if(!prcSt) rez += TSYS::strMess(_("Task terminated! "));
	if(tmDelay > -1) {
	    rez += (acqErr.getVal().size()?acqErr.getVal():string(_("Error connecting."))) + " ";
	    if(tmDelay) rez += TSYS::strMess(_("Restoring in %.6g s. "), tmDelay);
	    rez.replace(0, 1, i2s(TError::Tr_Connect));
	}
	else {
	    if(callSt)	rez += TSYS::strMess(_("Acquisition. "));
	    if(period()) rez += TSYS::strMess(_("Acquisition with the period %s. "), tm2s(1e-9*period()).c_str());
	    else rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "), atm2s(TSYS::cron(cron()), "%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time %s[%s], requests %.6g. "),
		tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str(), -tmDelay);
	    OPCAlloc res(mtxData, true);
	    rez += TSYS::strMess(_("Secure channel %u, token %u, lifetime %s; Request ID %u, handle %u; Session %s. "),
		sess.secChnl, sess.secToken, tm2s(1e-3*sess.secLifeTime-1e-6*(curTime()-sess.secChnlOpenTm)).c_str(), sess.sqReqId, sess.reqHndl, sess.sesId.c_str());
	    if(sess.mSubScr.size())
		rez += TSYS::strMess(_("Subscription %u, publishes %u, keep alive %s; Monitored items %u. "),
		    sess.mSubScr[0].subScrId, sess.mPublSeqs.size(),
		    tm2s(OpcUa_ClntPublishResentCntr*1e-3*sess.mSubScr[0].maxKeepAliveCnt*sess.mSubScr[0].publInterval-1e-6*(curTime()-sess.mSubScr[0].lstPublTm)).c_str(),
		    sess.mSubScr[0].mItems.size());
	    res.unlock();
	    if(servSt) rez.replace(0, 1, TSYS::strMess("0x%x",servSt));
	}
    }
    return rez;
}

TTpContr &TMdContr::owner( ) const	{ return *(TTpContr*)nodePrev(); }

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name, &owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    if(SYS->cfgCtx() && toEnable() && !enableStat())	enable();
}

void TMdContr::enable_( )
{
    string trName = "Sockets.out_OPCUA"+id();
    tr = SYS->transport().at().nodeAt(trName, 0, '.', 0, true);
    if(tr.freeStat()) {
	SYS->transport().at().at(TSYS::strParse(trName,0,".")).at().outAdd(TSYS::strParse(trName,1,".").substr(4));
	tr = SYS->transport().at().nodeAt(trName, 0, '.');
	tr.at().setDscr(TSYS::strMess(_("OPC UA automatic created transport for '%s' controller."),id().c_str()));
	tr.at().modifClr();
    }
    enSt = true;
    setEndPoint(endPoint());

    if(sess.mSubScr.empty())	sess.mSubScr.push_back(Subscr(this));	//Creation one subscription object
}

void TMdContr::disable_( )
{
    sess.mSubScr.clear();

    tr.free();

    //Clear the processing parameters list
    enRes.lock(); pHd.clear(); enRes.unlock();
}

void TMdContr::start_( )
{
    if(prcSt) return;

    servSt = 0;
    tmDelay = 0;

    //Start the gathering data task
    if(!prcSt) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true));

    alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("STOP")), TMess::Info);
    alSt = -1;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    try {
	if(co.fld().name() == "SCHEDULE")
	    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;
	else if(co.name() == "EndPoint" && enableStat()) {
	    tr.at().setAddr("TCP:"+epParse()); tr.at().modifClr();
	    ResAlloc res(resOPC, false);
	    SecuritySetting ss("", -1);
	    if(epLst.find(co.getS()) != epLst.end()) ss = epLst[co.getS()];
	    res.release();

	    if(ss.policy.size() && mSecPol.fld().values().find(ss.policy) != string::npos) {
		setSecPolicy(ss.policy);
		setSecMessMode(ss.messageMode);
	    }

	    XML_N req("opc.tcp");

	    //Send FindServers request for EndPoints list provide
	    req.setAttr("id", "FindServers");
	    reqService(req);

	    res.request(true);
	    epLst.clear();
	    for(unsigned iS = 0; iS < req.childSize(); iS++)
		for(unsigned iD = 0; iD < req.childGet(iS)->childSize(); iD++)
		    epLst[req.childGet(iS)->childGet(iD)->text()] = SecuritySetting();
	    res.release();

	    //Send GetEndpoints request for EndPoints list provide
	    req.clear()->setAttr("id", "GetEndpoints");
	    reqService(req);

	    res.request(true);
	    for(unsigned iCh = 0; iCh < req.childSize(); iCh++) {
		XML_N *xep = req.childGet(iCh);
		string ep = xep->attr("endpointUrl");
		if(epLst.find(ep) != epLst.end()) ep += "/"+TSYS::strParse(xep->attr("securityPolicyUri"),1,"#")+"/"+xep->attr("securityMode");
		epLst[ep] = SecuritySetting(TSYS::strParse(xep->attr("securityPolicyUri"),1,"#"), s2i(xep->attr("securityMode")));
	    }
	}
	else if(co.name() == "SecPolicy") {
	    if(co.getS() == "None" && secMessMode() != MS_None)	setSecMessMode(MS_None);
	    if(co.getS() != "None" && secMessMode() == MS_None)	setSecMessMode(MS_Sign);
	}
	else if(co.name() == "SecMessMode" &&
		((co.getI() != MS_None && secPolicy() == "None") || (co.getI() == MS_None && secPolicy() != "None")))
	    return false;
	else if(co.name() == "AuthUser" || co.name() == "AuthPass") {
	    XML_N req("opc.tcp");
	    req.clear()->setAttr("id", "CloseSession");
	    reqService(req);
	}
    } catch(...) { }

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

bool TMdContr::connect( int8_t est )
{
    if(tr.freeStat()) return false;

    if(est == 0) tr.at().stop();
    else if(est > 0) tr.at().start();

    return tr.at().startStat();
}

int TMdContr::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time )
{
    if(!connect()) connect(true);
    try {
	//?!?! Check for the timeout error or not
	return tr.at().messIO(obuf, len_ob, ibuf, len_ib, time);
    } catch(TError&) { return 0; }
}

void TMdContr::protIO( XML_N &io )
{
    MtxAlloc resN(tr.at().reqRes(), true);
    if(messLev() == TMess::Debug) io.setAttr("debug", "1");
    try { Client::protIO(io); }
    catch(TError &er)
    { io.setAttr("err", TSYS::strMess("0x%x:%s:%s", OpcUa_BadInvalidArgument, _("Remote host error"), er.mess.c_str())); }
}

void TMdContr::reqService( XML_N &io )
{
    if(tr.freeStat())	return;

    ResAlloc res(resOPC, true);
    io.setAttr("err", "");

    try { tr.at().start(); }
    catch(TError &err) { io.setAttr("err", TSYS::strMess("0x%x:%s",OpcUa_BadCommunicationError,err.mess.c_str())); return; }

    Client::reqService(io);
    if(io.attr("err").empty()) tmDelay--;
}

TVariant TMdContr::getValMIt( unsigned mItId, uint32_t *st )
{
    uint32_t ndSt = 0;

    OPCAlloc resDt(mtxData, true);
    Subscr::MonitItem *mIt = (mItId < sess.mSubScr[0].mItems.size()) ? &sess.mSubScr[0].mItems[mItId] : NULL;
    if(!mIt) ndSt = OpcUa_BadMonitoredItemIdInvalid;
    else if(mUseRead)
	ndSt = !str2uint(mIt->val.attr("nodata")) ? str2uint(mIt->val.attr("Status")) : OpcUa_BadMonitoredItemIdInvalid;
    else ndSt = (!mIt->active || str2uint(mIt->val.attr("nodata"))) ? OpcUa_BadMonitoredItemIdInvalid :
			(ndSt=mIt->st) ? ndSt : str2uint(mIt->val.attr("Status"));

    if(!mIt || ndSt) {
	if(st) *st = ndSt;
	return EVAL_REAL;
    }
    else {
	unsigned varTp = 0;
	if((varTp=s2i(mIt->val.attr("VarTp")))&OpcUa_Array) {
	    TArrayObj *curArr = new TArrayObj();
	    string vEl;
	    switch(varTp&OpcUa_VarMask) {
		case OpcUa_Boolean:
		    for(int off = 0, iEl = 0; (vEl=TSYS::strLine(mIt->val.text(),0,&off)).size(); iEl++)
			curArr->arSet(iEl, bool(s2i(vEl)));
		    break;
		case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16:
		case OpcUa_Int32: case OpcUa_UInt32: case OpcUa_Int64: case OpcUa_UInt64:
		    for(int off = 0, iEl = 0; (vEl=TSYS::strLine(mIt->val.text(),0,&off)).size(); iEl++)
			curArr->arSet(iEl, (int64_t)s2ll(vEl));
		    break;
		case OpcUa_Float: case OpcUa_Double:
		    for(int off = 0, iEl = 0; (vEl=TSYS::strLine(mIt->val.text(),0,&off)).size(); iEl++)
			curArr->arSet(iEl, s2r(vEl));
		    break;
		default:
		    for(int off = 0, iEl = 0; (vEl=TSYS::strLine(mIt->val.text(),0,&off)).size(); iEl++)
			curArr->arSet(iEl, vEl);
		    break;
	    }
	    return curArr;
	} else return mIt->val.text();
    }

    if(st) *st = ndSt;

    return EVAL_REAL;
}

TVariant TMdContr::getVal( const string &iaddr, MtxString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty())
	    err = TSYS::strMess("%d:%s", TError::Tr_Connect, acqErr.getVal().c_str());
	return EVAL_REAL;
    }

    string	nId = TSYS::strLine(iaddr, 2);
    if(nId.empty())	return EVAL_REAL;

    uint32_t ndSt = 0;
    TVariant rez = getValMIt(str2uint(nId), &ndSt);

    if(ndSt && err.getVal().empty())
	err = TSYS::strMess(_("Error '%s': 0x%x"), TSYS::strLine(iaddr,0).c_str(), ndSt);

    return rez;
}

bool TMdContr::setVal( const TVariant &vl, const string &iaddr, MtxString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", TError::Tr_Connect, acqErr.getVal().c_str());
	return false;
    }
    if(vl.isEVal())	return true;

    XML_N req("opc.tcp");

    string wrVl;
    AutoHD<TArrayObj> arr;
    if(vl.type() == TVariant::Object && !(arr=vl.getO()).freeStat())
	for(size_t iA = 0; iA < arr.at().arSize(); iA++) wrVl += arr.at().arGet(iA).getS() + "\n";
    else wrVl = vl.getS();
    req.setAttr("id", "Write")->
	childAdd("node")->setAttr("nodeId", TSYS::strLine(iaddr,0))->
			  setAttr("attributeId", i2s(AId_Value))->
			  setAttr("VarTp", TSYS::strLine(iaddr,1))->
			  setText(wrVl);
    reqService(req);
    if(!req.attr("err").empty()) { if(err.getVal().empty()) err = req.attr("err"); return false; }
    else if(strtol(req.childGet(0)->attr("Status").c_str(),NULL,0)) {
	if(err.getVal().empty()) err = TSYS::strMess(_("Write error status: %s"), req.childGet(0)->attr("Status").c_str());
	return false;
    }

    return true;
}

string TMdContr::epParse( string *uri )
{
    if(endPoint().compare(0,10,"opc.tcp://") != 0) return "";
    size_t uriPos = endPoint().find("/", 10);
    if(uri) *uri = (uriPos != string::npos) ? endPoint().substr(uriPos) : "";

    string addr = endPoint().substr(10, (uriPos==string::npos) ? uriPos : (uriPos-10));
    return s2i(TSYS::strParse(addr,1,":")) ? addr : TSYS::strParse(addr,0,":")+":4840";
}

string TMdContr::authData( )
{
    return (mAuthUser.getS().size() && mAuthPass.getS().size()) ? mAuthUser.getS()+"\n"+mAuthPass.getS() : "";
}

string TMdContr::applicationUri( )	{ return "urn:"+SYS->host()+":OpenSCADA:DAQ.OPC_UA"; }

string TMdContr::productUri( )		{ return "urn:OpenSCADA:DAQ.OPC_UA";/*PACKAGE_SITE;*/ }

string TMdContr::applicationName( )	{ return "OpenSCADA.OPC-UA Client"; }

void TMdContr::debugMess( const string &mess )	{ mess_debug_(nodePath().c_str(), "%s", mess.c_str()); }

void *TMdContr::Task( void *icntr )
{
    const TSYS::STask &tsk = TSYS::taskDescr();
    TMdContr &cntr = *(TMdContr*)icntr;

    cntr.prcSt = true;

    bool isStart = true;
    bool isStop  = false;

    //Initiate the publish period and register the server status attribute reading
    if(cntr.period())	cntr.sess.mSubScr[0].publInterval = 1e-6*cntr.period();
    unsigned ServerStatusMIt = cntr.sess.mSubScr[0].monitoredItemAdd(OpcUa_Server_ServerStatus_State);

    while(true) {
	try {
	    if(!cntr.redntUse()) {
	    if(cntr.tmDelay > 0) {
		cntr.callSt = false;
		//Get data from blocks to parameters or calc for logical type parameters
		MtxAlloc prmRes(cntr.enRes, true);
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		    cntr.pHd[iP].at().upValStd();
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		    cntr.pHd[iP].at().upValLog(isStart, isStop,
		    (isStart||isStop) ? DAQ_APER_FRQ : (tsk.period()?(1/tsk.period()):1e9/vmax(1e9/DAQ_APER_FRQ,cntr.period())));
		prmRes.unlock();

		cntr.tmDelay = vmax(0, cntr.tmDelay-(cntr.period()?(1e-9*(float)cntr.period()):1));

		if(isStop) break;

		TSYS::taskSleep(cntr.period()?cntr.period():1e9);

		if(TSYS::taskEndRun()) isStop = true;
		isStart = false;
		continue;
	    }

	    cntr.callSt = true;

	    if(!cntr.mUseRead && !cntr.sess.mSubScr[0].isActivated())	cntr.sess.mSubScr[0].activate(true);

	    if(!isStart && !isStop && !TSYS::taskEndRun()) {
		string tErr = cntr.poll(cntr.mUseRead);
		if(tErr.size()) cntr.setCntrDelay(tErr);
	    }

	    if(cntr.tmDelay > 0)	continue;

	    //Getting the controller-server status
	    cntr.servSt = cntr.getValMIt(ServerStatusMIt).getI();

	    //Get data from blocks to parameters or calc for logical type parameters
	    MtxAlloc prmRes(cntr.enRes, true);
	    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		cntr.pHd[iP].at().upValStd();
	    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		cntr.pHd[iP].at().upValLog(isStart, isStop,
		    (isStart||isStop) ? DAQ_APER_FRQ : (tsk.period()?(1/tsk.period()):1e9/vmax(1e9/DAQ_APER_FRQ,cntr.period())));
	    isStart = false;
	    prmRes.unlock();

	    //Generic acquisition alarm generate
	    if(cntr.tmDelay <= 0 && !TSYS::taskEndRun()) {
		if(cntr.alSt != 0) {
		    cntr.alSt = 0;
		    cntr.alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("OK")), TMess::Info);
		}
		cntr.tmDelay--;
	    }

	    cntr.callSt = false;

	    }	// !cntr.redntUse()

	} catch(TError &err) { cntr.setCntrDelay(TSYS::strMess(_("10:Generic error: %s."), err.mess.c_str())); /*mess_err(err.cat.c_str(), err.mess.c_str());*/ }

	if(isStop) break;

	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());

	if(TSYS::taskEndRun()) isStop = true;
    }

    //Closing the subscription and session ...
    if(TSYS::taskEndRun())
	try {
	    if(!cntr.mUseRead) cntr.sess.mSubScr[0].activate(false);

	    XML_N req("opc.tcp"); req.setAttr("id", "CloseALL");
	    cntr.reqService(req);
	} catch(TError &err){ mess_err(err.cat.c_str(), err.mess.c_str()); }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::setCntrDelay( const string &err )
{
    if(alSt <= 0) {
	alSt = 1;
	alarmSet(TSYS::strMess(_("Connection to the data source: %s."),TRegExp(":","g").replace(err,"=").c_str()));
    }
    acqErr = err;
    tmDelay = restTm;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/runSt",EVAL_STR,RWRWR_,"root",SDAQ_ID,1,
	    "help",_("Manual restart of the enabled controller object causes the force reformation of the monitored items list.\n"
		    "Restart to apply the removed PLC links in run."));
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/EndPoint",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "dest","sel_ed", "select","/cntr/cfg/elLst", NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "tp","str", "dest","sel_ed", "sel_list",TMess::labSecCRONsel().c_str(), "help",TMess::labSecCRON().c_str(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID, "help",TMess::labTaskPrior().c_str(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SYNCPER",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero for disable periodic sync."), NULL);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SecPolicy",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SecMessMode",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	if(startStat()) { ctrRemoveNode(opt,"/cntr/cfg/Cert"); ctrRemoveNode(opt,"/cntr/cfg/PvKey"); }
	else {
	    ctrMkNode2("fld",opt,-1,"/cntr/cfg/Cert",EVAL_STR,RWRW__,"root",SDAQ_ID, "tp","str", "cols","90", "rows","7", NULL);
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/PvKey",EVAL_STR,RWRW__,"root",SDAQ_ID);
	}
	ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthUser",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthPass",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/UseRead",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID,
	    "help", _("Otherwise there is activated and used the Publish (asynchronous) data acquisition service of the OPC-UA protocol."), NULL);
	if(enableStat() && ctrMkNode("area",opt,-1,"/ndBrws",_("Server nodes browser"))) {
	    ctrMkNode2("fld",opt,-1,"/ndBrws/nd",_("Node"),RWRWR_,"root",SDAQ_ID, "tp","str", "dest","select", "select","/ndBrws/ndLst", NULL);
	    if(ctrMkNode("table",opt,-1,"/ndBrws/attrs",_("Attributes"),R_R_R_,"root",SDAQ_ID)) {
		ctrMkNode2("list",opt,-1,"/ndBrws/attrs/0",_("Attribute"),R_R_R_,"root",SDAQ_ID, "tp","str", NULL);
		ctrMkNode2("list",opt,-1,"/ndBrws/attrs/1",_("Value"),R_R_R_,"root",SDAQ_ID, "tp","str", NULL);
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/st/runSt" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR) && s2i(opt->text()) && enableStat()) {
	// Deactivate the main session to reset/clear the monitored items
	if(sess.mSubScr.size() && sess.mSubScr[0].isActivated())
	    sess.mSubScr[0].activate(false);

	// Reloading the parameters' data
	vector<string> pls;
	list(pls);

	for(unsigned iP = 0; iP < pls.size(); iP++)
	    if(at(pls[iP]).at().enableStat())
		at(pls[iP]).at().loadDATA(true);

	// Now same starting
	start();
    }
    else if(a_path == "/cntr/cfg/AuthPass" && ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))
	opt->setText(string(mAuthPass.getS().size(),'*'));
    else if(a_path == "/cntr/cfg/elLst" && ctrChkNode(opt)) {
	ResAlloc res(resOPC, false);
	for(map<string, SecuritySetting>::iterator iEp = epLst.begin(); iEp != epLst.end(); iEp++)
	    opt->childAdd("el")->setText(iEp->first);
    }
    else if(enableStat() && a_path == "/ndBrws/nd") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(mBrwsVar);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	mBrwsVar = opt->text();
    }
    else if(enableStat() && a_path == "/ndBrws/attrs" && ctrChkNode(opt)) {
	XMLNode *n_attr	= ctrMkNode("list",opt,-1,"/ndBrws/attrs/0","",R_R_R_);
	XMLNode *n_val	= ctrMkNode("list",opt,-1,"/ndBrws/attrs/1","",R_R_R_);

	// Prepare request for all typical
	string cNodeId = i2s(OpcUa_RootFolder);
	size_t stC = mBrwsVar.rfind(")");
	size_t stP = mBrwsVar.rfind("(", stC);
	if(stP != string::npos && stC != string::npos) cNodeId = TSYS::strDecode(mBrwsVar.substr(stP+1,stC-stP-1));

	XML_N req("opc.tcp"); req.setAttr("id", "Read")->setAttr("timestampsToReturn", i2s(TS_NEITHER));
	for(int iA = 1; iA <= 22; iA++)
	    req.childAdd("node")->setAttr("nodeId", cNodeId)->setAttr("attributeId", i2s(iA));
	reqService(req);
	if(!req.attr("err").empty()) {
	    //mess_err(nodePath().c_str(), "%s", req.attr("err").c_str());
	    throw TError(nodePath().c_str(), "%s", req.attr("err").c_str());
	}

	// Get result
	for(unsigned iA = 0; iA < req.childSize(); iA++) {
	    if(strtol(req.childGet(iA)->attr("Status").c_str(),NULL,0)) continue;
	    string nANm = _("Unknown");
	    string nAVl = req.childGet(iA)->text();
	    switch(iA+1) {
		case AId_NodeId: nANm = _("NodeId");	break;
		case AId_NodeClass:
		    nANm = _("NodeClass");
		    switch(s2i(nAVl)) {
			case NC_Object:		nAVl = _("Object");	break;
			case NC_Variable:	nAVl = _("Variable");	break;
			case NC_Method:		nAVl = _("Method");	break;
			case NC_ObjectType:	nAVl = _("ObjectType");	break;
			case NC_VariableType:	nAVl = _("VariableType"); break;
			case NC_ReferenceType:	nAVl = _("ReferenceType"); break;
			case NC_DataType:	nAVl = _("DataType");	break;
			case NC_View:		nAVl = _("View");	break;
		    }
		    break;
		case AId_BrowseName:	nANm = _("BrowseName");	break;
		case AId_DisplayName:	nANm = _("DisplayName");break;
		case AId_Descr:		nANm = _("Description");break;
		case AId_WriteMask:	nANm = _("WriteMask");	break;
		case AId_UserWriteMask:	nANm = _("UserWriteMask");	break;
		case AId_IsAbstract:	nANm = _("IsAbstract");	break;
		case AId_Symmetric:	nANm = _("Symmetric");	break;
		case AId_InverseName:	nANm = _("InverseName");	break;
		case AId_ContainsNoLoops:nANm = _("ContainsNoLoops");	break;
		case AId_EventNotifier:	nANm = _("EventNotifier");	break;
		case AId_Value:		nANm = _("Value");	break;
		case AId_DataType: {
		    nANm = _("DataType");
		    XML_N reqTp("opc.tcp");
		    reqTp.setAttr("id", "Read")->setAttr("timestampsToReturn", i2s(TS_NEITHER))->
			childAdd("node")->setAttr("nodeId", nAVl)->setAttr("attributeId", i2s(AId_BrowseName));
		    reqService(reqTp);
		    if(!reqTp.attr("err").empty()) mess_err(nodePath().c_str(), "%s", reqTp.attr("err").c_str());
		    else if(reqTp.childSize()) nAVl = reqTp.childGet(0)->text();
		    break;
		}
		case AId_ValueRank:	nANm = _("ValueRank");	break;
		case AId_ArrayDimensions: nANm = _("ArrayDimensions");	break;
		case AId_AccessLevel: {
		    nANm = _("AccessLevel");
		    char cRW = s2i(nAVl);
		    nAVl = "";
		    if(cRW&ACS_Read)		nAVl += _("Readable, ");
		    if(cRW&ACS_Write)		nAVl += _("Writable, ");
		    if(cRW&ACS_HistRead)	nAVl += _("History readable, ");
		    if(cRW&ACS_HistWrite)	nAVl += _("History writable, ");
		    if(cRW&ACS_SemChange)	nAVl += _("Semantic change, ");
		    break;
		}
		case AId_UserAccessLevel: {
		    nANm = _("UserAccessLevel");
		    char cRW = s2i(nAVl);
		    nAVl = "";
		    if(cRW&ACS_Read)		nAVl += _("Readable, ");
		    if(cRW&ACS_Write)		nAVl += _("Writable, ");
		    if(cRW&ACS_HistRead)	nAVl += _("History readable, ");
		    if(cRW&ACS_HistWrite)	nAVl += _("History writable, ");
		    break;
		}
		case AId_MinimumSamplingInterval: nANm = _("MinimumSamplingInterval");	break;
		case AId_Historizing:		nANm = _("Historizing");	break;
		case AId_Executable:		nANm = _("Executable");		break;
		case AId_UserExecutable:	nANm = _("UserExecutable");	break;
	    }
	    if(n_attr)	n_attr->childAdd("el")->setText(nANm);
	    if(n_val)	n_val->childAdd("el")->setText(nAVl);
	}
    }
    else if(enableStat() && a_path == "/ndBrws/ndLst" && ctrChkNode(opt)) {
	// Get current node references by call browse
	string cNodeId = i2s(OpcUa_RootFolder);
	size_t stC = mBrwsVar.rfind(")");
	size_t stP = mBrwsVar.rfind("(", stC);
	if(stP != string::npos && stC != string::npos) cNodeId = TSYS::strDecode(mBrwsVar.substr(stP+1,stC-stP-1));
	XML_N req("opc.tcp"); req.setAttr("id","Browse");
	req.childAdd("node")->setAttr("nodeId",cNodeId)->
			      setAttr("referenceTypeId",i2s(OpcUa_HierarchicalReferences))->
			      setAttr("browseDirection",i2s(BD_BOTH))->
			      setAttr("resultMask",i2s(/*0x3f*/RdRm_IsForward|RdRm_BrowseName));
	try{ reqService(req); } catch(TError&) { opt->childAdd("el")->setText(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder)); return; }
	if(!req.attr("err").empty() || !req.childSize() || !req.childGet(0)->childSize()) {
	    if(!req.attr("err").empty()) mess_err(nodePath().c_str(), "%s", req.attr("err").c_str());
	    opt->childAdd("el")->setText(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder));
	    return;
	}
	XML_N *rn = req.childGet(0);

	// Process inverse references
	bool invRefPr = false;
	string tStr;
	for(unsigned iN = 0; iN < rn->childSize(); iN++) {
	    if(s2i(rn->childGet(iN)->attr("isForward"))) continue;
	    opt->childAdd("el")->setText(((tStr=rn->childGet(iN)->attr("browseName")).size()?tStr:rn->childGet(iN)->attr("displayName"))+
		" ("+TSYS::strEncode(rn->childGet(iN)->attr("nodeId"),TSYS::Custom,"()")+")");
	    invRefPr = true;
	}
	if(!invRefPr && mBrwsVar != TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder))
	    opt->childAdd("el")->setText(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder));

	// Append self address
	opt->childAdd("el")->setText(mBrwsVar);
	// Process forward references
	for(unsigned iN = 0; iN < rn->childSize(); iN++) {
	    if(!s2i(rn->childGet(iN)->attr("isForward"))) continue;
	    opt->childAdd("el")->setText(((tStr=rn->childGet(iN)->attr("browseName")).size()?tStr:rn->childGet(iN)->attr("displayName"))+
		" ("+TSYS::strEncode(rn->childGet(iN)->attr("nodeId"),TSYS::Custom,"()")+")");
	}
    }
    else TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name,tp_prm), acqErr(dataRes()), pEl("w_attr"), lCtx(NULL)
{
    acqErr.setVal("");
    if(isLogic()) lCtx = new TLogCtx(this, name+"_OPCUAPrm");
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
    if(lCtx) delete lCtx;
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&pEl))	vlElemAtt(&pEl);
}

void TMdPrm::postDisable( int flag )
{
    TParamContr::postDisable(flag);

    if(flag&NodeRemove && isLogic()) {
	string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";
	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(id(), true);
	TBDS::dataDel(io_bd, owner().owner().nodePath()+type().DB(&owner())+"_io", cfg);
    }
}

TCntrNode &TMdPrm::operator=( const TCntrNode &node )
{
    TParamContr::operator=(node);

    const TMdPrm *src_n = dynamic_cast<const TMdPrm*>(&node);
    if(!src_n || !src_n->enableStat() || !enableStat() || !isLogic() || !lCtx || !lCtx->func()) return *this;

    //IO values copy
    for(int iIO = 0; iIO < src_n->lCtx->ioSize(); iIO++)
	if(src_n->lCtx->ioFlg(iIO)&TPrmTempl::CfgLink)
	    lCtx->lnkAddrSet(iIO, src_n->lCtx->lnkAddr(iIO));
	else lCtx->setS(iIO, src_n->lCtx->getS(iIO));

    lCtx->chkLnkNeed = lCtx->initLnks();
    modif();

    return *this;
}

bool TMdPrm::isStd( ) const		{ return (type().name == "std"); }

bool TMdPrm::isLogic( ) const		{ return (type().name == "logic"); }

void TMdPrm::setType( const string &tpId )
{
    if(lCtx) { delete lCtx; lCtx = NULL; }

    TParamContr::setType(tpId);

    if(isLogic()) lCtx = new TLogCtx(this, name()+"_OPCUAPrm");
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat()) return;

    TParamContr::enable();

    loadDATA();

    owner().prmEn(this, true);
}

void TMdPrm::loadDATA( bool incl )
{
    XML_N req("opc.tcp");
    map<string, bool> als;

    //Parse OPC-UA attributes and convert to string list for standard type parameter
    if(isStd()) {
	//Nodes list process and parameter's attributes creation
	string aIt, aNd, aFlg, aId, aNm;
	for(int off = 0; (aIt=TSYS::strLine(ndList(),0,&off)).size() || off < (int)ndList().size(); ) {
	    if(aIt.empty() || aIt[0] == '#') continue;

	    int itOff = 0;
	    aNd = TSYS::strParse(aIt, 0, "|", &itOff);
	    aFlg = TSYS::strParse(aIt, 0, "|", &itOff);
	    aId = TSYS::strParse(aIt, 0, "|", &itOff);
	    aNm = TSYS::strParse(aIt, 0, "|", &itOff);

	    string svtp;
	    TFld::Type vtp = TFld::Integer;
	    unsigned vflg = 0;

	    // Attribute creating from the hints
	    if(aFlg.size()) {
		//  Value type prepare
		if(aFlg.find("b") != string::npos)	{ vtp = TFld::Boolean; svtp = i2s(OpcUa_Boolean); }
		else if(aFlg.find("i") != string::npos)	{ vtp = TFld::Integer; svtp = i2s(OpcUa_Int64); }
		else if(aFlg.find("f") != string::npos)	{ vtp = TFld::Real; svtp = i2s(OpcUa_Double); }
		else if(aFlg.find("s") != string::npos)	{ vtp = TFld::String; svtp = i2s(OpcUa_String); }
		else if(aFlg.find("o") != string::npos)	vtp = TFld::Object;

		//  Flags prepare
		vflg = (aFlg.find("r") != string::npos && aFlg.find("w") != string::npos) ? TVal::DirWrite|TVal::DirRead :
			((aFlg.find("w") != string::npos) ? TVal::DirWrite : TFld::NoWrite|TVal::DirRead);

		//  Prepare attribute id
		if(aId.empty())
		    aId = TSYS::strEncode((isdigit(aNd[0])?TSYS::strParse(aNd,1,":"):aNd), TSYS::oscdID);
	    }
	    // Attribute creating from the server information
	    else {
		// Request the node class
		req.clear()->setAttr("id", "Read")->setAttr("timestampsToReturn", i2s(TS_NEITHER));
		req.childAdd("node")->setAttr("nodeId", aNd)->setAttr("attributeId", i2s(AId_NodeClass));
		req.childAdd("node")->setAttr("nodeId", aNd)->setAttr("attributeId", i2s(AId_BrowseName));
		req.childAdd("node")->setAttr("nodeId", aNd)->setAttr("attributeId", i2s(AId_DisplayName));
		req.childAdd("node")->setAttr("nodeId", aNd)->setAttr("attributeId", i2s(AId_Value));
		req.childAdd("node")->setAttr("nodeId", aNd)->setAttr("attributeId", i2s(AId_AccessLevel));
		owner().reqService(req);
		//if(!req.attr("err").empty()) {
		    //mess_err(nodePath().c_str(), "%s", req.attr("err").c_str());
		    //return req.attr("err");
		//}
		if(strtol(req.childGet(0)->attr("Status").c_str(),NULL,0))	continue;

		// Variable node's attribute creation
		if(s2i(req.childGet(0)->text()) == NC_Variable && s2i(req.childGet(4)->text())&ACS_Read) {
		    svtp = req.childGet(3)->attr("VarTp");
		    //  Value type prepare
		    vtp = TFld::String;
		    if(s2i(svtp)&OpcUa_Array) vtp = TFld::Object;
		    else switch(s2i(svtp)&OpcUa_VarMask) {
			case OpcUa_Boolean:							vtp = TFld::Boolean;	break;
			case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16:
			case OpcUa_Int32: case OpcUa_UInt32: case OpcUa_Int64: case OpcUa_UInt64:vtp = TFld::Integer;	break;
			case OpcUa_Float: case OpcUa_Double:					vtp = TFld::Real;	break;
		    }

		    //  Flags prepare
		    vflg = TVal::DirWrite|TVal::DirRead;
		    if(!(s2i(req.childGet(4)->text())&ACS_Write))	vflg |= TFld::NoWrite;

		    //  Prepare attribute id
		    if(aId.empty()) {
			//   Checking for doubles
			unsigned iA = 0;
			while(iA < pEl.fldSize() && TSYS::strLine(pEl.fldAt(iA).reserve(),0) != aNd) iA++;
			if(iA < pEl.fldSize())	aId = pEl.fldAt(iA).name();
			else {
			    //   ID creating
			    aId = TSYS::strEncode((isdigit(req.childGet(1)->text()[0])?TSYS::strParse(req.childGet(1)->text(),1,":"):req.childGet(1)->text()), TSYS::oscdID);
			    while(vlPresent(aId)) aId = TSYS::strLabEnum(aId);
			}
		    }
		} else continue;
	    }

	    if(aNm.empty())	aNm = aId;

	    if(!pEl.fldPresent(aId) || pEl.fldAt(pEl.fldId(aId)).type() != vtp) {
		if(pEl.fldPresent(aId)) pEl.fldDel(pEl.fldId(aId));
		pEl.fldAdd(new TFld(aId.c_str(),"",vtp,TFld::NoFlag));
	    }

	    int el_id = pEl.fldId(aId);

	    pEl.fldAt(el_id).setFlg(vflg);
	    pEl.fldAt(el_id).setDescr(aNm);

	    unsigned clntHndl = owner().sess.mSubScr[0].monitoredItemAdd(NodeId::fromAddr(aNd));

	    pEl.fldAt(el_id).setReserve(aNd+"\n"+svtp+"\n"+u2s(clntHndl));

	    als[aId] = true;

	    //>> Browse request for get child nodes
	    /*req.setAttr("id","Browse")->childAdd("node")->setAttr("nodeId",cNodeId)->setAttr("browseDirection",i2s(TProt::BD_BOTH));
	    reqOPC(req);
	    if( !req.attr("err").empty() || !req.childSize() ) throw TError(nodePath().c_str(),"%s",req.attr("err").c_str());
	    XMLNode *rn = req.childGet(0);*/
	}
    }
    //Template's function connect for logical type parameter
    else if(isLogic() && lCtx && lCtx->func())	lCtx->chkLnkNeed = lCtx->initLnks(true);
    else if(isLogic() && lCtx && !lCtx->func())
	try {
	    //vector<string> als;
	    string m_tmpl = cfg("TMPL").getS();
	    if(!lCtx->func() && m_tmpl.size()) {
		lCtx->setFunc(&SYS->daq().at().tmplLibAt(TSYS::strParse(m_tmpl,0,".")).at().
						      at(TSYS::strParse(m_tmpl,1,".")).at().func().at());
		lCtx->setVfName(id()+"_OPC_UA_tmplprm");
	    }
	    // Init attrubutes
	    if(lCtx->func()) {
		lCtx->addLinksAttrs(&pEl);

		// Load IO at enabling
		loadIO(true);

		// Init links
		lCtx->chkLnkNeed = lCtx->initLnks(/*true*/);	//!!!! Do not reconnect but that can be done in loadIO() early

		// Init system attributes identifiers
		lCtx->idFreq  = lCtx->ioId("f_frq");
		lCtx->idStart = lCtx->ioId("f_start");
		lCtx->idStop  = lCtx->ioId("f_stop");
		lCtx->idErr   = lCtx->ioId("f_err");
		lCtx->idSh    = lCtx->ioId("SHIFR");
		lCtx->idNm    = lCtx->ioId("NAME");
		lCtx->idDscr  = lCtx->ioId("DESCR");
		int idThis    = lCtx->ioId("this");
		if(idThis >= 0) lCtx->setO(idThis, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));

		// First call
		if(owner().startStat() && !owner().redntUse()) upValLog(true, false, DAQ_APER_FRQ);
	    }
	} catch(TError &err) { disable(); throw; }

    //Find for delete attribute
    for(int iP = 0; isStd() && iP < (int)pEl.fldSize(); iP++)
	if(als.find(pEl.fldAt(iP).name()) == als.end())
	    try {
		// Unregister from monitor
		//owner().sess.mSubScr[0].monitoredItemDel(str2uint(TSYS::strLine(pEl.fldAt(iP).reserve(),2)));

		pEl.fldDel(iP); iP--;
	    } catch(TError &err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }

    //Call the included paramers' data reload
    if(incl) {
	vector<string> prmLs;
	list(prmLs);
	for(unsigned iP = 0; iP < prmLs.size(); iP++)
	    at(prmLs[iP]).at().loadDATA(incl);
    }
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    if(lCtx && lCtx->func() && lCtx->func()) {
	//Waiting the ordinal calculation finish
	for(int iTm = 0; lCtx->isCalc() && iTm < prmWait_TM/prmWait_DL; iTm++) SYS->sysSleep(prmWait_DL);
	//Termination the calculation
	while(lCtx->isCalc()) { SYS->taskSendSIGALRM(owner().nodePath('.',true)); SYS->sysSleep(prmWait_DL); }
    }

    owner().prmEn(this, false);
    if(lCtx && lCtx->func() && owner().startStat() && !owner().redntUse()) upValLog(false, true, DAQ_APER_FRQ);

    TParamContr::disable();

    //Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned iEl = 0; iEl < ls.size(); iEl++)
	vlAt(ls[iEl]).at().setS(EVAL_STR, 0, true);

    //Template's function disconnect
    if(lCtx)	lCtx->cleanLnks(true);
}

void TMdPrm::load_( )
{
    if(SYS->cfgCtx() && toEnable() && !enableStat())	enable();
    loadIO();
}

void TMdPrm::loadIO( bool force )
{
    if(!enableStat() || !isLogic() || !lCtx || !lCtx->func()) return;

    //Load IO and init links
    TConfig cfg(&mod->prmIOE());
    cfg.cfg("PRM_ID").setS(ownerPath(true));
    cfg.cfg("VALUE").setExtVal(true);
    string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";

    //IO values loading and links set, by seek
    for(int iIO = 0; iIO < lCtx->ioSize(); iIO++) {
	cfg.cfg("ID").setS(lCtx->func()->io(iIO)->id());
	if(!TBDS::dataGet(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg,TBDS::NoException)) continue;
	if(lCtx->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
	    lCtx->lnkAddrSet(iIO, cfg.cfg("VALUE").getS(TCfg::ExtValOne));	//Force to no translation
	else if(lCtx->func()->io(iIO)->type() != IO::String || !(lCtx->func()->io(iIO)->flg()&IO::TransltText))
	    lCtx->setS(iIO, cfg.cfg("VALUE").getS(TCfg::ExtValOne));		//Force to no translation
	else lCtx->setS(iIO, cfg.cfg("VALUE").getS());
    }
    lCtx->chkLnkNeed = lCtx->initLnks();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
    saveIO();
}

void TMdPrm::saveIO( )
{
    if(!enableStat() || !isLogic() || !lCtx || !lCtx->func()) return;

    //Save IO and init links
    TConfig cfg(&mod->prmIOE());
    cfg.cfg("PRM_ID").setS(ownerPath(true));
    string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";
    for(int iIO = 0; iIO < lCtx->func()->ioSize(); iIO++) {
	cfg.cfg("ID").setS(lCtx->func()->io(iIO)->id());
	cfg.cfg("VALUE").setNoTransl(!(lCtx->func()->io(iIO)->type() == IO::String &&
	    (lCtx->func()->io(iIO)->flg()&IO::TransltText) && !(lCtx->func()->io(iIO)->flg()&TPrmTempl::CfgLink)));
	if(lCtx->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
	    cfg.cfg("VALUE").setS(lCtx->lnkAddr(iIO));
	else cfg.cfg("VALUE").setS(lCtx->getS(iIO));
	TBDS::dataSet(io_bd, owner().owner().nodePath()+type().DB(&owner())+"_io", cfg);
    }
}

void TMdPrm::upValStd( )
{
    if(!isStd()) return;

    MtxString w_err(dataRes());
    AutoHD<TVal> pVal;
    vector<string> ls;

    elem().fldList(ls);
    for(unsigned iEl = 0; iEl < ls.size(); iEl++) {
	pVal = vlAt(ls[iEl]);
	if(!(pVal.at().fld().flg()&TVal::DirRead) || (pVal.at().fld().flg()&TVal::Dynamic)) continue;
	pVal.at().set(owner().getVal(pVal.at().fld().reserve(),w_err), 0, true);
    }

    //Alarm set
    acqErr.setVal(w_err.getVal());
}

void TMdPrm::upValLog( bool first, bool last, double frq )
{
    if(!isLogic() || !lCtx->func())	return;

    AutoHD<TVal> pVal;
    vector<string> ls;

    try {
	if(lCtx->chkLnkNeed && !first && !last)	lCtx->chkLnkNeed = lCtx->initLnks();

	//Set fixed system attributes
	if(lCtx->idFreq >= 0)	lCtx->setR(lCtx->idFreq, frq);
	if(lCtx->idStart >= 0)	lCtx->setB(lCtx->idStart, lCtx->isChangedProg(true) || first);
	if(lCtx->idStop >= 0)	lCtx->setB(lCtx->idStop, last);
	if(lCtx->idSh >= 0)	lCtx->setS(lCtx->idSh, id());
	if(lCtx->idNm >= 0)	lCtx->setS(lCtx->idNm, name());
	if(lCtx->idDscr >= 0)	lCtx->setS(lCtx->idDscr, descr());

	//Get input links
	lCtx->inputLinks();

	//Calc template
	lCtx->setMdfChk(true);
	lCtx->calc();
	if(SYS->modifCalc()) modif();

	//Put output links
	lCtx->outputLinks();

	//Put fixed system attributes
	if(lCtx->idNm >= 0 && lCtx->ioMdf(lCtx->idNm)) setName(lCtx->getS(lCtx->idNm));
	if(lCtx->idDscr >= 0 && lCtx->ioMdf(lCtx->idDscr)) setDescr(lCtx->getS(lCtx->idDscr));

	//Attribute's values update
	elem().fldList(ls);
	for(unsigned iEl = 0; iEl < ls.size(); iEl++) {
	    int id_lnk = lCtx->lnkId(ls[iEl]);
	    if(id_lnk >= 0 && !lCtx->lnkActive(id_lnk)) id_lnk = -1;
	    pVal = vlAt(ls[iEl]);
	    if(pVal.at().fld().flg()&TVal::Dynamic)	continue;
	    if(id_lnk < 0) pVal.at().set(lCtx->get(lCtx->ioId(ls[iEl])), 0, true);
	    else pVal.at().set(lCtx->lnkInput(id_lnk), 0, true);
	}
    } catch(TError &err) {
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Error of the calculation template."));
    }

    acqErr.setVal("");	//But it is not used for the type
}

TVariant TMdPrm::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    //bool attrAdd( string id, string name, string tp = "real", string selValsNms = "" ) - attribute <id> and <name> for type <tp> add.
    //  id, name - new attribute id and name;
    //  tp - attribute type [boolean | integer | real | string | text | object] + selection mode [sel | seled] + read only [ro];
    //  selValsNms - two lines with values in first and it's names in first (separated by ";").
    if(iid == "attrAdd" && prms.size() >= 1) {
	if(!enableStat() || !isLogic())	return false;
	TFld::Type tp = TFld::Real;
	string stp, stp_ = (prms.size() >= 3) ? prms[2].getS() : "real";
	stp.resize(stp_.length());
	std::transform(stp_.begin(), stp_.end(), stp.begin(), ::tolower);
	if(stp.find("boolean") != string::npos)		tp = TFld::Boolean;
	else if(stp.find("integer") != string::npos)	tp = TFld::Integer;
	else if(stp.find("real") != string::npos)	tp = TFld::Real;
	else if(stp.find("string") != string::npos ||
	    stp.find("text") != string::npos)		tp = TFld::String;
	else if(stp.find("object") != string::npos)	tp = TFld::Object;

	unsigned flg = TVal::Dynamic;
	if(stp.find("sel") != string::npos)	flg |= TFld::Selectable;
	if(stp.find("seled") != string::npos)	flg |= TFld::SelEdit;
	if(stp.find("text") != string::npos)	flg |= TFld::FullText;
	if(stp.find("ro") != string::npos)	flg |= TFld::NoWrite;

	string	sVals = (prms.size() >= 4) ? prms[3].getS() : "";
	string	sNms = TSYS::strLine(sVals, 1);
	sVals = TSYS::strLine(sVals, 0);

	MtxAlloc res(elem().resEl(), true);
	unsigned aId = elem().fldId(prms[0].getS(), true);
	if(aId < elem().fldSize()) {
	    if(prms.size() >= 2 && prms[1].getS().size()) elem().fldAt(aId).setDescr(prms[1].getS());
	    elem().fldAt(aId).setFlg(elem().fldAt(aId).flg()^((elem().fldAt(aId).flg()^flg)&(TFld::Selectable|TFld::SelEdit|TFld::FullText|TFld::NoWrite)));
	    elem().fldAt(aId).setValues(sVals);
	    elem().fldAt(aId).setSelNames(sNms);
	    elem().fldAt(aId).setLen(SYS->sysTm());
	}
	else if(!vlPresent(prms[0].getS()))
	    elem().fldAdd(new TFld(prms[0].getS().c_str(),prms[(prms.size()>=2)?1:0].getS().c_str(),tp,flg,
				    i2s(SYS->sysTm()).c_str(),"",sVals.c_str(),sNms.c_str()));
	return true;
    }
    //bool attrDel( string id ) - attribute <id> remove.
    if(iid == "attrDel" && prms.size() >= 1) {
	if(!enableStat() || !isLogic())	return false;
	MtxAlloc res(elem().resEl(), true);
	unsigned aId = elem().fldId(prms[0].getS(), true);
	if(aId == elem().fldSize())	return false;
	try { elem().fldDel(aId); } catch(TError&) { return false; }
	return true;
    }

    return TParamContr::objFuncCall(iid, prms, user_lang);
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat()) {
	if(val.name() == "err") {
	    if(!enableStat())		val.setS(_("1:Parameter disabled."),0,true);
	    else if(!owner().startStat()) val.setS(_("2:Acquisition stopped."),0,true);
	} else val.setS(EVAL_STR, 0, true);
	return;
    }

    if(owner().redntUse()) return;

    if(val.name() == "err") {
	if(owner().tmDelay > -1) val.setS(_("10:Error of connection or no response."), 0, true);
	else if(!acqErr.getVal().empty()) val.setS(acqErr.getVal(), 0, true);
	else if(lCtx && lCtx->idErr >= 0) val.setS(lCtx->getS(lCtx->idErr), 0, true);
	else val.setS("0", 0, true);
	/*else {
	    //Checking the remote attributes for the error status
	    uint32_t firstErr = 0;
	    vector<uint32_t> astls;
	    //MtxAlloc res(enRes, true);
	    for(unsigned iA = 0; iA < pEl.fldSize(); iA++) {
		astls.push_back(pEl.fldAt(iA).len());
		if(pEl.fldAt(iA).len() && !firstErr) firstErr = pEl.fldAt(iA).len();
	    }
	    //res.unlock();
	    string aLs;
	    for(unsigned iA = 0; iA < astls.size(); iA++) aLs += TSYS::strMess(":0x%x",astls[iA]);
	    val.setS(TSYS::strMess(_("0x%x: Attribute's errors %s"),firstErr,aLs.c_str()),0,true);
	}*/
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setS(EVAL_STR, 0, true); return; }

    if(vl.isEVal() || vl == pvl)return;

    //Send to active reserve station
    bool isRdnt = vlSetRednt(vo, vl, pvl);

    //Direct write
    bool wrRez = false;
    // Standard type request
    if(isStd() && !isRdnt)
	wrRez = owner().setVal(vl, vo.fld().reserve(), acqErr);
    // Logical type request
    else if(isLogic()) {
	int id_lnk = lCtx->lnkId(vo.name());
	if(id_lnk >= 0 && !lCtx->lnkActive(id_lnk)) id_lnk = -1;
	if(id_lnk < 0) { lCtx->set(lCtx->ioId(vo.name()), vl); wrRez = true; }
	else if(!isRdnt) wrRez = lCtx->lnkOutput(id_lnk, vl);
    }
    if(!wrRez && !isRdnt) vo.setS(EVAL_STR, 0, true);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	if(isStd()) {
	    ctrMkNode("fld",opt,-1,"/prm/cfg/ND_LS",EVAL_STR,(owner().startStat()&&enableStat())?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"rows","8","SnthHgl","1",
		"help",_("Variables and it containers (Objects) list. All variables will put into the parameter attributes list.\n"
		    "Variables write by separate lines in the form \"{ns}:{id}[|[{flg}][|{id}[|{name}]]]\".\n"
		    "Where:\n"
		    "  ns  - name space, number; zero value can be omitted;\n"
		    "  id  - node identifier by number, string, bytes string and GUID;\n"
		    "  flg - flags of no requesting the metadata on the server:\n"
		    "        value type (b-Boolean,i-Integer,f-Float,s-String,o-Object), read/write mode (r-read, w-write);\n"
		    "  id  - identifier of the created attribute;\n"
		    "  name - name of the created attribute.\n"
		    "Example:\n"
		    "  84 - root folder;\n"
		    "  3:\"BasicDevices2\"||var - basic devices node in the name space 3 and the string view with the force attribute ID;\n"
		    "  4:\"61626364\"||var|Variable - node in the names space 4 and the byte string view with the force attribute ID and name;\n"
		    "  4:{40d95ab0-50d6-46d3-bffd-f55639b853d4}|irw|var|Variable - node in the names space 4 and the GUID view with no requesting\n"
		    "        the server metadata for RW Integer and the force attribute ID and name."));
	    ctrMkNode2("fld",opt,-1,"/prm/cfg/SEL_NDS",_("Node append"),(owner().startStat()&&enableStat())?0:RWRW__,"root",SDAQ_ID,
		"dest","select", "select","/prm/cfg/SEL_NDS_lst", NULL);
	}
	if(isLogic()) {
	    ctrMkNode("fld",opt,-1,"/prm/cfg/TMPL",EVAL_STR,RWRW__,"root",SDAQ_ID,3,"tp","str","dest","select","select","/prm/tmplList");
	    if(enableStat() && lCtx->func())	lCtx->cntrCmdProc(opt);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(isStd() && a_path == "/prm/cfg/ND_LS" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	opt->childAdd("rule")->setAttr("expr","^#[^\n]*")->setAttr("color","gray")->setAttr("font_italic","1");
	opt->childAdd("rule")->setAttr("expr","\"(\\\\\"|[^\"])*\"")->setAttr("color","darkgreen");
	opt->childAdd("rule")->setAttr("expr","\\{[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\\}")->setAttr("color","darkblue");
	opt->childAdd("rule")->setAttr("expr","\\b(0[xX][0-9a-fA-F]*|[0-9]*)\\b")->setAttr("color","darkorange");
	opt->childAdd("rule")->setAttr("expr","\\|[bifsorw]*|")->setAttr("color","red");
	/*opt->childAdd("rule")->setAttr("expr","\\:")->setAttr("color","blue");*/
    }
    else if(isStd() && a_path == "/prm/cfg/SEL_NDS") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"selND","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	    //Read for type check
	    string cNodeId;
	    size_t stC = opt->text().rfind(")"), stP = opt->text().rfind("(", stC);
	    bool isSet = false, isStructVar = false;
	    if(stP != string::npos && stC != string::npos) {
		cNodeId = TSYS::strDecode(opt->text().substr(stP+1,stC-stP-1));
		XML_N req("opc.tcp"); req.setAttr("id", "Read")->setAttr("timestampsToReturn", i2s(TS_NEITHER));
		req.childAdd("node")->setAttr("nodeId", cNodeId)->setAttr("attributeId", i2s(AId_NodeClass));
		req.childAdd("node")->setAttr("nodeId", cNodeId)->setAttr("attributeId", i2s(AId_DataType));
		try { owner().reqService(req); } catch(TError&) { }
		if(req.attr("err").empty() && req.childSize() && s2i(req.childGet(0)->text()) == NC_Variable) {
		    if(req.childSize() > 1 && s2i(req.childGet(1)->text()) == OpcUa_Structure)	isStructVar = true;
		    string nLs = ndList(), nS;
		    for(int off = 0; (nS=TSYS::strLine(nLs,0,&off)).size(); )
			if(TSYS::strParse(nS,0,"|") == cNodeId) break;
		    if(nS.empty()) setNdList(nLs+((nLs.size() && nLs[nLs.size()-1] != '\n')?"\n":"")+cNodeId);
		    isSet = true;
		}
	    }
	    if(!isSet || isStructVar)
		TBDS::genPrmSet(nodePath()+"selND", opt->text(), opt->attr("user"));
	}
    }
    else if(isStd() && a_path == "/prm/cfg/SEL_NDS_lst" && ctrChkNode(opt)) {
	string selNd = TBDS::genPrmGet(nodePath()+"selND",TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder), opt->attr("user"));
	size_t stC = selNd.rfind(")"), stP = selNd.rfind("(", stC);
	string cNodeId = (stP != string::npos && stC != string::npos) ? TSYS::strDecode(selNd.substr(stP+1,stC-stP-1)) : i2s(OpcUa_RootFolder);

	XML_N req("opc.tcp"); req.setAttr("id","Browse");
	req.childAdd("node")->setAttr("nodeId", cNodeId)->
			      setAttr("referenceTypeId", i2s(OpcUa_HierarchicalReferences))->
			      setAttr("browseDirection", i2s(BD_BOTH))->
			      setAttr("resultMask", i2s(RdRm_IsForward|RdRm_BrowseName));
	try{ owner().reqService(req); }
	catch(TError&) { opt->childAdd("el")->setText(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder)); return; }
	if(!req.attr("err").empty() || !req.childSize() || !req.childGet(0)->childSize()) {
	    opt->childAdd("el")->setText(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder));
	    return;
	}
	XML_N *rn = req.childGet(0);

	// Process inverse references
	bool invRefPr = false;
	string tStr;
	for(unsigned iN = 0; iN < rn->childSize(); iN++) {
	    if(s2i(rn->childGet(iN)->attr("isForward"))) continue;
	    opt->childAdd("el")->setText(((tStr=rn->childGet(iN)->attr("browseName")).size()?tStr:rn->childGet(iN)->attr("displayName"))+
		" ("+TSYS::strEncode(rn->childGet(iN)->attr("nodeId"),TSYS::Custom,"()")+")");
	    invRefPr = true;
	}
	if(!invRefPr && cNodeId != i2s(OpcUa_RootFolder)) opt->childAdd("el")->setText(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder));

	// Append self address
	opt->childAdd("el")->setText(selNd);
	// Process forward references
	for(unsigned iN = 0; iN < rn->childSize(); iN++) {
	    if(!s2i(rn->childGet(iN)->attr("isForward"))) continue;
	    opt->childAdd("el")->setText(((tStr=rn->childGet(iN)->attr("browseName")).size()?tStr:rn->childGet(iN)->attr("displayName"))+
		" ("+TSYS::strEncode(rn->childGet(iN)->attr("nodeId"),TSYS::Custom,"()")+")");
	}
    }
    else if(isLogic() && a_path == "/prm/cfg/TMPL" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	cfg("TMPL").setS(opt->text());
	disable();
	modif();
    }
    else if(isLogic() && enableStat() && lCtx->func() && lCtx->cntrCmdProc(opt))	;
    else TParamContr::cntrCmdProc(opt);
}

//***************************************************
//* Logical type parameter's context                *
TMdPrm::TLogCtx::TLogCtx( TCntrNode *iobj, const string &name ) : TPrmTempl::Impl(iobj, name.c_str()),
    chkLnkNeed(false), idFreq(-1), idStart(-1), idStop(-1), idErr(-1), idSh(-1), idNm(-1), idDscr(-1)
{

}

bool TMdPrm::TLogCtx::lnkInit( int num, bool toRecnt )
{
    //Common link forms
    if(!TPrmTempl::Impl::lnkInit(num,toRecnt))	return false;

    MtxAlloc res(lnkRes, true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it == lnks.end() || it->second.addrSpec.size())	return false;

    NodeId lN = NodeId::fromAddr(it->second.addr, true);
    if(!lN.isNull()) {
	unsigned clntHndl = ((TMdPrm*)obj)->owner().sess.mSubScr[0].monitoredItemAdd(lN);
	int vTp = 0;
	switch(func()->io(num)->type()) {
	    case IO::String:	vTp = OpcUa_String; break;
	    case IO::Integer:	vTp = OpcUa_Int64;  break;
	    case IO::Real:	vTp = OpcUa_Double; break;
	    case IO::Boolean:	vTp = OpcUa_Boolean;break;
	    default: vTp = 0;
	}
	it->second.addrSpec = it->second.addr+"\n"+i2s(vTp)+"\n"+u2s(clntHndl);
	return true;
    }

    return false;
}

bool TMdPrm::TLogCtx::lnkActive( int num )
{
    MtxAlloc res(lnkRes, true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it != lnks.end() && it->second.addrSpec.size())	return true;
    return TPrmTempl::Impl::lnkActive(num);
}

TVariant TMdPrm::TLogCtx::lnkInput( int num )
{
    MtxAlloc res(lnkRes, true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it == lnks.end()) return EVAL_REAL;
    string addrSpec = it->second.addrSpec;
    res.unlock();

    return addrSpec.size() ? ((TMdPrm*)obj)->owner().getVal(addrSpec, ((TMdPrm*)obj)->acqErr) : TPrmTempl::Impl::lnkInput(num);
}

bool TMdPrm::TLogCtx::lnkOutput( int num, const TVariant &vl )
{
    MtxAlloc res(lnkRes, true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it == lnks.end()) return false;
    string addrSpec = it->second.addrSpec;
    res.unlock();

    if(addrSpec.size()) ((TMdPrm*)obj)->owner().setVal(vl, addrSpec, ((TMdPrm*)obj)->acqErr);
    else return TPrmTempl::Impl::lnkOutput(num, vl);

    return true;
}

void TMdPrm::TLogCtx::cleanLnks( bool andFunc )
{
    TPrmTempl::Impl::cleanLnks(andFunc);
    idFreq = idStart = idStop = idErr = idSh = idNm = idDscr = -1;
}

string TMdPrm::TLogCtx::lnkHelp( )
{
    return _("Special address format:\n"
	"OPC-UA node writes in the form \"{ns}:{id}\", where:\n"
	"    ns - name space, number; zero value can be omitted;\n"
	"    id - node identifier by number, string, bytes string and GUID.\n"
	"Examples:\n"
	"    84 - root directory;\n"
	"    3:\"BasicDevices2\" - basic devices node in the name space 3 and the string view;\n"
	"    4:\"61626364\" - node in the names space 4 and the byte string view;\n"
	"    4:{40d95ab0-50d6-46d3-bffd-f55639b853d4} - node in the names space 4 and the GUID view.\n\n"
	"Common address format:\n") + TPrmTempl::Impl::lnkHelp();
}

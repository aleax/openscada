
//OpenSCADA module DAQ.OPC_UA file: mod_daq.cpp
/***************************************************************************
 *   Copyright (C) 2009-2021 by Roman Savochenko, <roman@oscada.org>       *
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
    fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",_("Restore timeout, seconds"),TFld::Integer,TFld::NoFlag,"4","10","1;3600"));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period, seconds"),TFld::Integer,TFld::NoFlag,"4","60","0;1000"));
    fldAdd(new TFld("EndPoint",_("End point"),TFld::String,TFld::NoFlag,"50","opc.tcp://localhost"));
    fldAdd(new TFld("SecPolicy",_("Security policy"),TFld::String,TFld::Selectable,"20","None","None;Basic128Rsa15;Basic256",_("None;Basic128Rsa15;Basic256")));
    fldAdd(new TFld("SecMessMode",_("Message security mode"),TFld::Integer,TFld::Selectable,"1","1",
	TSYS::strMess("%d;%d;%d",MS_None,MS_Sign,MS_SignAndEncrypt).c_str(),_("None;Sign;Sign&Encrypt")));
    fldAdd(new TFld("Cert",_("Certificate (PEM)"),TFld::String,TFld::FullText,"10000"));
    fldAdd(new TFld("PvKey",_("Private key (PEM)"),TFld::String,TFld::FullText,"10000"));
    fldAdd(new TFld("AuthUser",_("Auth: user"),TFld::String,TFld::NoFlag,"20"));
    fldAdd(new TFld("AuthPass",_("Auth: password"),TFld::String,TFld::NoFlag,"20"));
    fldAdd(new TFld("UseRead",_("Use the \"Read\" function"),TFld::Boolean,TFld::NoFlag,"1","1"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("ND_LS",_("Nodes list"),TFld::String,TFld::FullText|TCfg::NoVal,"1000",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) : TController(name_c,daq_db,cfgelem), enRes(true),
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mRestTm(cfg("TM_REST")), mSync(cfg("SYNCPER")),
    mEndP(cfg("EndPoint")), mSecPol(cfg("SecPolicy")), mSecMessMode(cfg("SecMessMode")), mCert(cfg("Cert")), mPvKey(cfg("PvKey")),
    mAuthUser(cfg("AuthUser")), mAuthPass(cfg("AuthPass")), mUseRead(cfg("UseRead").getBd()),
    mPer(1e9), prcSt(false), callSt(false), mPCfgCh(false), alSt(-1), mBrwsVar(TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder)),
    acqErr(dataRes()), tmDelay(0), servSt(0)
{
    cfg("PRM_BD").setS("OPC_UA_Prm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::epParse( string *uri )
{
    if(endPoint().compare(0,10,"opc.tcp://") != 0) return "";
    size_t uriPos = endPoint().find("/", 10);
    if(uri) *uri = (uriPos != string::npos) ? endPoint().substr(uriPos) : "";

    string addr = endPoint().substr(10, (uriPos==string::npos) ? uriPos : (uriPos-10));
    return s2i(TSYS::strParse(addr,1,":")) ? addr : TSYS::strParse(addr,0,":")+":4840";
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();

    if(startStat() && !redntUse()) {
	if(tmDelay > -1) {
	    rez += TSYS::strMess(_("Connection error. Restoring in %.6g s."), tmDelay);
	    rez.replace(0, 1, "10");
	}
	else {
	    if(callSt)	rez += TSYS::strMess(_("Acquisition. "));
	    if(period()) rez += TSYS::strMess(_("Acquisition with the period: %s. "), tm2s(1e-9*period()).c_str());
	    else rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "), atm2s(TSYS::cron(cron()), "%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s[%s]. Requests %.6g."),
		tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str(), -tmDelay);
	    if(servSt) rez.replace(0, 1, TSYS::strMess("0x%x",servSt));
	}
    }
    return rez;
}

string TMdContr::authData( )
{
    return (mAuthUser.getS().size() && mAuthPass.getS().size()) ? mAuthUser.getS()+"\n"+mAuthPass.getS() : "";
}

void TMdContr::reqService( XML_N &io )
{
    if(tr.freeStat())	return;

    ResAlloc res(nodeRes(), true);
    io.setAttr("err", "");

    try { tr.at().start(); }
    catch(TError &err) { io.setAttr("err", TSYS::strMess("0x%x:%s",OpcUa_BadCommunicationError,err.mess.c_str())); return; }

    Client::reqService(io);
    if(io.attr("err").empty()) tmDelay--;
}

string TMdContr::applicationUri( )	{ return "urn:"+SYS->host()+":OpenSCADA:DAQ.OPC_UA"; }

string TMdContr::productUri( )		{ return "urn:OpenSCADA:DAQ.OPC_UA";/*PACKAGE_SITE;*/ }

string TMdContr::applicationName( )	{ return "OpenSCADA.OPC-UA Client"; }

bool TMdContr::connect( int8_t est )
{
    if(tr.freeStat()) return false;

    if(est == 0) tr.at().stop();
    else if(est > 0) tr.at().start();

    return tr.at().startStat();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name, &owner().tpPrmAt(type)); }

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

    if(mSubScr.empty())	mSubScr.push_back(Subscr(this));	//Creation one subscription object
}

void TMdContr::disable_( )
{
    mSubScr.clear();

    tr.free();
}

void TMdContr::start_( )
{
    //Establish connection
    //try { tr.at().start(); } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

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

void TMdContr::protIO( XML_N &io )
{
    MtxAlloc resN(tr.at().reqRes(), true);
    if(messLev() == TMess::Debug) io.setAttr("debug", "1");
    try { Client::protIO(io); }
    catch(TError &er)
    { io.setAttr("err", TSYS::strMess("0x%x:%s:%s", OpcUa_BadInvalidArgument, _("Remote host error"), er.mess.c_str())); }
}

int TMdContr::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time )
{
    if(!connect()) connect(true);
    try {
    //!!!! Check for the timeout error or not
	return tr.at().messIO(obuf, len_ob, ibuf, len_ib, time);
    } catch(TError&) { return 0; }
}

void TMdContr::debugMess( const string &mess )	{ mess_debug_(nodePath().c_str(), "%s", mess.c_str()); }

void TMdContr::prmEn( const string &id, bool val )
{
    MtxAlloc res(enRes, true);

    unsigned iPrm;
    for(iPrm = 0; iPrm < pHd.size(); iPrm++)
	if(pHd[iPrm].at().id() == id) break;

    if(val && iPrm >= pHd.size()) pHd.push_back(at(id));
    if(!val && iPrm < pHd.size()) pHd.erase(pHd.begin()+iPrm);
    if(startStat()) mPCfgCh = true;
}

void *TMdContr::Task( void *icntr )
{
    vector<string>	als;
    string		nId;
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.prcSt = true;
    bool firstCall = true;

    if(cntr.period())	cntr.mSubScr[0].publInterval = 1e-6*cntr.period();

    XML_N req("opc.tcp"); req.setAttr("id", "Read")->setAttr("timestampsToReturn", i2s(TS_NEITHER));

    try {
	for(unsigned int it_cnt = cntr.pHd.size(); !TSYS::taskEndRun(); it_cnt++) {
	    if(cntr.redntUse())	{ TSYS::taskSleep(1e9); continue; }
	    if(cntr.tmDelay > 0){ TSYS::taskSleep(1e9); cntr.tmDelay = vmax(0,cntr.tmDelay-1); continue; }

	    cntr.callSt = true;

	    if(!cntr.mUseRead) {
		if(!cntr.mSubScr[0].isActivated()) cntr.mSubScr[0].activate(true);

		cntr.poll();

		// Parameters updating
		AutoHD<TVal> vl;
		uint32_t ndSt = 0;
		MtxAlloc res(cntr.enRes, true);
		for(unsigned iP = 0; iP < cntr.pHd.size(); ++iP) {
		    cntr.pHd[iP].at().vlList(als);
		    for(unsigned iA = 0; iA < als.size(); ++iA) {
			vl = cntr.pHd[iP].at().vlAt(als[iA]);
			nId = TSYS::strLine(vl.at().fld().reserve(), 2);
			if(nId.empty())	continue;

			XML_N *mIt = ((ndSt=str2uint(nId)) < cntr.mSubScr[0].mItems.size()) ? &cntr.mSubScr[0].mItems[ndSt] : NULL;
			ndSt = (!mIt || !mIt->attr("statusCode").size()) ? OpcUa_BadMonitoredItemIdInvalid :
				   (ndSt=str2uint(mIt->attr("statusCode"))) ? ndSt : str2uint(mIt->attr("Status"));
			vl.at().setS((!mIt||ndSt)?EVAL_STR:mIt->text(), 0, true);
			vl.at().fld().setLen(ndSt);
		    }
		}

		res.unlock();

		firstCall = false;
		cntr.callSt = false;

		TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
		continue;
	    }

	    unsigned int div = cntr.period() ? (unsigned int)(cntr.syncPer()/(1e-9*cntr.period())) : 0;

	    MtxAlloc res(cntr.enRes, true);
	    if(!req.childSize() || cntr.mPCfgCh || cntr.tmDelay == 0 || (div && (it_cnt%div) < cntr.pHd.size())) {
		if(div && (it_cnt%div) < cntr.pHd.size()) cntr.pHd[it_cnt%div].at().attrPrc();

		//Prepare nodes list
		req.childClear();
		req.childAdd("node")->setAttr("prmId", "OPC_UA_Server")->setAttr("prmAttr", "ServerStatus_State")->
		    setAttr("nodeId", i2s(OpcUa_Server_ServerStatus_State))->setAttr("attributeId", i2s(AId_Value));
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
		    if(firstCall || cntr.tmDelay == 0) cntr.pHd[iP].at().attrPrc();
		    cntr.pHd[iP].at().vlList(als);
		    for(unsigned iA = 0; iA < als.size(); iA++) {
			nId = TSYS::strLine(cntr.pHd[iP].at().vlAt(als[iA]).at().fld().reserve(), 0);
			if(nId.empty())	continue;
			req.childAdd("node")->setAttr("prmId", cntr.pHd[iP].at().id())->
			    setAttr("prmAttr", als[iA])->setAttr("nodeId", nId)->setAttr("attributeId", i2s(AId_Value));
		    }
		}
		cntr.mPCfgCh = false;
	    }
	    res.unlock();

	    cntr.reqService(req);

	    //Place results
	    bool isErr = !req.attr("err").empty();
	    int ndSt = 0;
	    AutoHD<TVal> vl;
	    res.lock();
	    for(unsigned iC = 0, iP = 0, varTp = 0; iC < req.childSize() && iP < cntr.pHd.size(); iC++) {
		XML_N *cnX = req.childGet(iC);
		if(cnX->attr("prmId") == "OPC_UA_Server" && cnX->attr("prmAttr") == "ServerStatus_State")
		{ cntr.servSt = strtoul(cnX->text().c_str(),NULL,10); continue; }
		while(cnX->attr("prmId") != cntr.pHd[iP].at().id()) iP++;
		if(iP >= cntr.pHd.size()) break;
		if(cntr.pHd[iP].at().vlPresent(cnX->attr("prmAttr"))) {
		    ndSt = strtol(cnX->attr("Status").c_str(),NULL,0);
		    vl = cntr.pHd[iP].at().vlAt(cnX->attr("prmAttr"));
		    if((varTp=s2i(cnX->attr("VarTp")))&OpcUa_Array && !isErr && !ndSt) {
			TArrayObj *curArr = new TArrayObj();
			string vEl;
			switch(varTp&OpcUa_VarMask) {
			    case OpcUa_Boolean:
				for(int off = 0, iEl = 0; (vEl=TSYS::strLine(cnX->text(),0,&off)).size(); iEl++)
				    curArr->arSet(iEl, bool(s2i(vEl)));
				break;
			    case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16:
			    case OpcUa_Int32: case OpcUa_UInt32: case OpcUa_Int64: case OpcUa_UInt64:
				for(int off = 0, iEl = 0; (vEl=TSYS::strLine(cnX->text(),0,&off)).size(); iEl++)
				    curArr->arSet(iEl, (int64_t)s2ll(vEl));
				break;
			    case OpcUa_Float: case OpcUa_Double:
				for(int off = 0, iEl = 0; (vEl=TSYS::strLine(cnX->text(),0,&off)).size(); iEl++)
				    curArr->arSet(iEl, s2r(vEl));
				break;
			    default:
				for(int off = 0, iEl = 0; (vEl=TSYS::strLine(cnX->text(),0,&off)).size(); iEl++)
				    curArr->arSet(iEl, vEl);
				break;
			}
			vl.at().setO(curArr, 0, true);
		    }
		    else vl.at().setS((isErr||ndSt)?EVAL_STR:cnX->text(), 0, true);
		    vl.at().fld().setLen(ndSt);
		}
	    }
	    if(isErr) {
		cntr.acqErr.setVal(req.attr("err"));
		//mess_err(cntr.nodePath().c_str(), "%s", cntr.acqErr.getVal().c_str());
		if(cntr.alSt <= 0) {
		    cntr.alSt = 1;
		    cntr.alarmSet(TSYS::strMess(_("Connection to the data source: %s."),TRegExp(":","g").replace(cntr.acqErr.getVal(),"=").c_str()));
		}
		cntr.tmDelay = cntr.restTm();
		continue;
	    }
	    else {
		cntr.acqErr.setVal("");
		if(cntr.alSt != 0) {
		    cntr.alSt = 0;
		    cntr.alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("OK")), TMess::Info);
		}
	    }
	    res.unlock();

	    firstCall = false;
	    cntr.callSt = false;

	    TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
	}
    } catch(TError &err){ mess_err(err.cat.c_str(), err.mess.c_str()); }

    //Closing the subscription and session ...
    if(TSYS::taskEndRun())
	try {
	    if(!cntr.mUseRead) cntr.mSubScr[0].activate(false);

	    req.childClear();
	    req.setAttr("id", "CloseALL");
	    cntr.reqService(req);
	} catch(TError &err){ mess_err(err.cat.c_str(), err.mess.c_str()); }

    cntr.prcSt = false;

    return NULL;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    try {
	if(co.fld().name() == "SCHEDULE")
	    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;
	else if(co.name() == "EndPoint" && enableStat()) {
	    tr.at().setAddr("TCP:"+epParse()); tr.at().modifClr();
	    ResAlloc res(nodeRes(), false);
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
	    for(unsigned i_s = 0; i_s < req.childSize(); i_s++)
		for(unsigned i_d = 0; i_d < req.childGet(i_s)->childSize(); i_d++)
		    epLst[req.childGet(i_s)->childGet(i_d)->text()] = SecuritySetting();
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

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/EndPoint",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "dest","sel_ed", "select","/cntr/cfg/elLst", NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "tp","str", "dest","sel_ed", "sel_list",TMess::labSecCRONsel(), "help",TMess::labSecCRON(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID, "help",TMess::labTaskPrior(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SYNCPER",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero for disable periodic sync."), NULL);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SecPolicy",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SecMessMode",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/Cert",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID, "tp","str", "cols","90", "rows","7", NULL);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PvKey",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthUser",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthPass",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/UseRead",EVAL_STR,startStat()?R_R___:RWRW__,"root",SDAQ_ID);
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
    if(a_path == "/cntr/cfg/AuthPass" && ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))
	opt->setText(string(mAuthPass.getS().size(),'*'));
    else if(a_path == "/cntr/cfg/elLst" && ctrChkNode(opt)) {
	ResAlloc res(nodeRes(), false);
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
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name,tp_prm), pEl("w_attr")
{

}

TMdPrm::~TMdPrm( )	{ nodeDelAll(); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&pEl))	vlElemAtt(&pEl);
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    attrPrc();

    owner().prmEn(id(), true);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    //Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

string TMdPrm::attrPrc( )
{
    MtxAlloc res(enRes);

    bool srchOK = false;
    XML_N req("opc.tcp");
    vector<string> als;

    //Nodes list process and parameter's attributes creation
    string snd;
    for(int off = 0; (snd=TSYS::strLine(ndList(),0,&off)).size() || off < ndList().size(); ) {
	if(snd.empty() || snd[0] == '#') continue;
	// Request for node class request
	req.clear()->setAttr("id", "Read")->setAttr("timestampsToReturn", i2s(TS_NEITHER));
	req.childAdd("node")->setAttr("nodeId", snd)->setAttr("attributeId", i2s(AId_NodeClass));
	req.childAdd("node")->setAttr("nodeId", snd)->setAttr("attributeId", i2s(AId_BrowseName));
	req.childAdd("node")->setAttr("nodeId", snd)->setAttr("attributeId", i2s(AId_DisplayName));
	req.childAdd("node")->setAttr("nodeId", snd)->setAttr("attributeId", i2s(AId_Value));
	req.childAdd("node")->setAttr("nodeId", snd)->setAttr("attributeId", i2s(AId_AccessLevel));
	owner().reqService(req);
	if(!req.attr("err").empty()) {
	    //mess_err(nodePath().c_str(), "%s", req.attr("err").c_str());
	    return req.attr("err");
	}
	if(strtol(req.childGet(0)->attr("Status").c_str(),NULL,0))	continue;

	// Variable node's attribute creation
	if(s2i(req.childGet(0)->text()) == NC_Variable && s2i(req.childGet(4)->text())&ACS_Read) {
	    als.push_back(snd);
	    srchOK = false;

	    //  Value type prepare
	    TFld::Type vtp = TFld::String;
	    if(s2i(req.childGet(3)->attr("VarTp"))&OpcUa_Array) vtp = TFld::Object;
	    else switch(s2i(req.childGet(3)->attr("VarTp"))&OpcUa_VarMask) {
		case OpcUa_Boolean:								vtp = TFld::Boolean;	break;
		case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16:
		case OpcUa_Int32: case OpcUa_UInt32: case OpcUa_Int64: case OpcUa_UInt64:	vtp = TFld::Integer;	break;
		case OpcUa_Float: case OpcUa_Double:						vtp = TFld::Real;	break;
	    }

	    res.lock();
	    // Find for already presented attribute
	    for(unsigned iA = 0; iA < pEl.fldSize() && !srchOK; iA++)
		if(TSYS::strLine(pEl.fldAt(iA).reserve(),0) == snd) {
		    if(pEl.fldAt(iA).type() != vtp)
			try { pEl.fldDel(iA); break; }
			catch(TError &err) { }
		    srchOK = true;
		}

	    // Create new attribute
	    if(!srchOK) {
		//  Prepare attribute id
		string aid = TSYS::strEncode((isdigit(req.childGet(1)->text()[0])?TSYS::strParse(req.childGet(1)->text(),1,":"):req.childGet(1)->text()), TSYS::oscdID);
		while(vlPresent(aid))	aid = TSYS::strLabEnum(aid);

		//  Value type prepare
		TFld::Type vtp = TFld::String;
		if(s2i(req.childGet(3)->attr("VarTp"))&OpcUa_Array) vtp = TFld::Object;
		else switch(s2i(req.childGet(3)->attr("VarTp"))&OpcUa_VarMask) {
		    case OpcUa_Boolean:								vtp = TFld::Boolean;	break;
		    case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16:
		    case OpcUa_Int32: case OpcUa_UInt32: case OpcUa_Int64: case OpcUa_UInt64:	vtp = TFld::Integer;	break;
		    case OpcUa_Float: case OpcUa_Double:					vtp = TFld::Real;	break;
		}

		//  Browse name
		string aNm = req.childGet(2)->text();
		size_t nmPos = aNm.find(":");
		if(nmPos!=string::npos) aNm.erase(0, nmPos+1);

		//  Flags prepare
		unsigned vflg = TVal::DirWrite;
		if(!(s2i(req.childGet(4)->text())&ACS_Write))	vflg |= TFld::NoWrite;

		//  Register to monitor
		unsigned clntHndl = owner().mSubScr[0].monitoredItemAdd(NodeId::fromAddr(snd));

		pEl.fldAdd(new TFld(aid.c_str(),aNm.c_str(),vtp,vflg,"","","","",
		    (snd+"\n"+req.childGet(3)->attr("VarTp")+"\n"+u2s(clntHndl)).c_str()));
	    }
	    res.unlock();
	}

	//>> Browse request for get child nodes
	/*req.setAttr("id","Browse")->childAdd("node")->setAttr("nodeId",cNodeId)->setAttr("browseDirection",i2s(TProt::BD_BOTH));
	reqOPC(req);
	if( !req.attr("err").empty() || !req.childSize() ) throw TError(nodePath().c_str(),"%s",req.attr("err").c_str());
	XMLNode *rn = req.childGet(0);*/
    }

    //Find for delete attribute
    res.lock();
    for(unsigned iA = 0, iP; iA < pEl.fldSize(); ) {
	for(iP = 0; iP < als.size(); ++iP)
	    if(TSYS::strLine(pEl.fldAt(iA).reserve(),0) == als[iP])	break;
	if(iP >= als.size())
	    try {
		//  Unregister from monitor
		//owner().mSubScr[0].monitoredItemDel(str2uint(TSYS::strLine(pEl.fldAt(iA).reserve(),2)));

		pEl.fldDel(iA);
		continue;
	    } catch(TError &err) { }
	++iA;
    }

    return "";
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")  { TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ND_LS",EVAL_STR,(owner().startStat()&&enableStat())?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"rows","8","SnthHgl","1",
	    "help",_("Variables and it containers (Objects) list. All variables will put into the parameter attributes list.\n"
		"Variables wrote by separated lines into format: [ns:id].\n"
		"Where:\n"
		"  ns - names scope, number, zero value can missed;\n"
		"  id - node identifier, number, string, bytes string and GUID.\n"
		"Example:\n"
		"  84 - root folder;\n"
		"  3:\"BasicDevices2\" - basic devices node in the names scope 3 and string view;\n"
		"  4:\"61626364\" - node in the names scope 4 and byte string view;\n"
		"  4:{40d95ab0-50d6-46d3-bffd-f55639b853d4} - node in the names scope 4 and GUID view."));
	ctrMkNode2("fld",opt,-1,"/prm/cfg/SEL_NDS",_("Node append"),(owner().startStat()&&enableStat())?0:RWRW__,"root",SDAQ_ID,
	    "dest","select", "select","/prm/cfg/SEL_NDS_lst", NULL);
	return;
    }

    //Process command to page
    if(a_path == "/prm/cfg/ND_LS" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	opt->childAdd("rule")->setAttr("expr","^#[^\n]*")->setAttr("color","gray")->setAttr("font_italic","1");
	opt->childAdd("rule")->setAttr("expr","\"(\\\\\"|[^\"])*\"")->setAttr("color","darkgreen");
	opt->childAdd("rule")->setAttr("expr","\\{[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\\}")->setAttr("color","darkblue");
	opt->childAdd("rule")->setAttr("expr","\\b(0[xX][0-9a-fA-F]*|[0-9]*)\\b")->setAttr("color","darkorange");
	opt->childAdd("rule")->setAttr("expr","\\:")->setAttr("color","blue");
    }
    else if(a_path == "/prm/cfg/SEL_NDS") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) opt->setText(TBDS::genDBGet(nodePath()+"selND","",opt->attr("user")));
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
			if(nS == cNodeId) break;
		    if(nS.empty()) { setNdList(nLs+((nLs.size() && nLs[nLs.size()-1] != '\n')?"\n":"")+cNodeId); isSet = true; }
		}
	    }
	    if(!isSet || isStructVar)	TBDS::genDBSet(nodePath()+"selND", opt->text(), opt->attr("user"));
	}
    }
    else if(a_path == "/prm/cfg/SEL_NDS_lst" && ctrChkNode(opt)) {
	string selNd = TBDS::genDBGet(nodePath()+"selND",TSYS::strMess(_("Root folder (%d)"),OpcUa_RootFolder), opt->attr("user"));
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
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlGet( TVal &val )
{
    if(val.name() != "err")	return;

    if(!enableStat() || !owner().startStat()) {
	if(!enableStat())		val.setS(_("1:Parameter disabled."),0,true);
	else if(!owner().startStat())	val.setS(_("2:Acquisition stopped."),0,true);
	return;
    }
    if(owner().redntUse()) return;

    if(!owner().acqErr.getVal().empty()) val.setS(owner().acqErr.getVal(),0,true);
    else {
	//Check remote attributes for error status
	uint32_t firstErr = 0;
	vector<uint32_t> astls;
	MtxAlloc res(enRes, true);
	for(unsigned iA = 0; iA < pEl.fldSize(); iA++) {
	    astls.push_back(pEl.fldAt(iA).len());
	    if(pEl.fldAt(iA).len() && !firstErr) firstErr = pEl.fldAt(iA).len();
	}
	res.unlock();
	string aLs;
	for(unsigned iA = 0; iA < astls.size(); iA++) aLs += TSYS::strMess(":0x%x",astls[iA]);
	val.setS(TSYS::strMess(_("0x%x: Attribute's errors %s"),firstErr,aLs.c_str()),0,true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setS(EVAL_STR, 0, true); return; }

    //Send to active reserve station
    if(vlSetRednt(vo,vl,pvl))	return;

    if(vl.isEVal() || vl == pvl)return;

    //Direct write
    XML_N req("opc.tcp");

    string wrVl;
    AutoHD<TArrayObj> arr;
    if(vl.type() == TVariant::Object && !(arr=vl.getO()).freeStat())
	for(size_t iA = 0; iA < arr.at().arSize(); iA++) wrVl += arr.at().arGet(iA).getS() + "\n";
    else wrVl = vl.getS();
    req.setAttr("id", "Write")->
	childAdd("node")->setAttr("nodeId", TSYS::strLine(vo.fld().reserve(),0))->
			  setAttr("attributeId", i2s(AId_Value))->
			  setAttr("VarTp", TSYS::strLine(vo.fld().reserve(),1))->
			  setText(wrVl);
    owner().reqService(req);
    if(!req.attr("err").empty()) mess_err(nodePath().c_str(), "%s", req.attr("err").c_str());
    else if(strtol(req.childGet(0)->attr("Status").c_str(),NULL,0))
	mess_err(nodePath().c_str(), "Write error status: %s", req.childGet(0)->attr("Status").c_str());
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

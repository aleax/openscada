
//OpenSCADA module DAQ.MMS file: module.cpp
/***************************************************************************
 *   Copyright (C) 2013-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include "module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"MMS"
#define MOD_NAME	_("MMS(IEC-9506)")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"1.4.6"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("MMS(IEC-9506) client implementation.")
#define LICENSE		"GPL2"
//*************************************************

ModMMS::TTpContr *ModMMS::mod;  //Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new ModMMS::TTpContr(source);
	return NULL;
    }
}

using namespace ModMMS;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr( )	{ }

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",_("Restore timeout, seconds"),TFld::Integer,TFld::NoFlag,"4","10","1;3600"));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period, seconds"),TFld::Integer,TFld::NoFlag,"4","0","0;1000"));
    fldAdd(new TFld("ADDR",_("Server address"),TFld::String,TFld::NoFlag,"50","localhost:102"));
    fldAdd(new TFld("VARS_RD_REQ",_("Variables into read request"),TFld::Integer,TFld::NoFlag,"3","100","1;9999"));
    fldAdd(new TFld("COTP_DestTSAP",_("Destination TSAP"),TFld::Integer,TFld::NoFlag,"3","512","0;65535"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("VAR_LS",_("Variables list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

void TTpContr::load_( )	{ }

void TTpContr::save_( )	{ }

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTypeDAQ::cntrCmdProc(opt);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    TTypeDAQ::cntrCmdProc(opt);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem ) : TController(name_c,daq_db,cfgelem), enRes(true), cntrRes(true),
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mRestTm(cfg("TM_REST")), mSync(cfg("SYNCPER")), mAddr(cfg("ADDR")), mVarsRdReq(cfg("VARS_RD_REQ")),
    mPer(1e9), prcSt(false), callSt(false), isReload(false), alSt(-1), acq_err(dataRes()), tmDelay(0)
{
    cfg("PRM_BD").setS("MMSPrm_"+name_c);

    //ParameterCBB
    string prms;
    MMS::setBS(prms, MMS::SupportOpts::str1);
    MMS::setBS(prms, MMS::SupportOpts::str2);
    MMS::setBS(prms, MMS::SupportOpts::vnam);
    MMS::setBS(prms, MMS::SupportOpts::valt);
    MMS::setBS(prms, MMS::SupportOpts::vadr);
    MMS::setBS(prms, MMS::SupportOpts::tpy);
    MMS::setBS(prms, MMS::SupportOpts::vlis);
    setCallParameterCBB(prms);

    //ServicesSupported
    prms = "";
    MMS::setBS(prms, MMS::SupportServs::status);
    MMS::setBS(prms, MMS::SupportServs::getNameList);
    MMS::setBS(prms, MMS::SupportServs::identify);
    MMS::setBS(prms, MMS::SupportServs::read);
    MMS::setBS(prms, MMS::SupportServs::write);
    MMS::setBS(prms, MMS::SupportServs::getVariableAccessAttributes);
    MMS::setBS(prms, MMS::SupportServs::getCapabilityList);
    setCallServicesSupported(prms);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	if(tmDelay >= 0) {
	    rez += (tmDelay == 0) ? TSYS::strMess(_("No activity data.")) : TSYS::strMess(_("Connection error. Restoring in %.6g s."), tmDelay);
	    rez.replace(0, 1, "10");
	}
	else {
	    if(callSt)	rez += TSYS::strMess(_("Acquisition. "));
	    if(period()) rez += TSYS::strMess(_("Acquisition with the period: %s. "), tm2s(1e-9*period()).c_str());
	    else rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "), atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s[%s]. Requests %.6g."),
		tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str(), -tmDelay);
	}
    }

    return rez;
}

void TMdContr::regVar( const string &vl, const string &opts )
{
    MtxAlloc res(enRes, true);
    if(mVars.find(vl) == mVars.end()) mVars[vl] = VarStr();

    //Options process and set
    size_t pos;
    if(opts.find("s") != string::npos) mVars[vl].single = true;
    if((pos=opts.find("/")) < opts.size()-2) mVars[vl].div = s2i(opts.substr(pos+1,2));
}

void TMdContr::reqService( MMS::XML_N &io )
{
    MtxAlloc res(cntrRes, true);
    io.setAttr("err", "");

    try { tr.at().start((enableStat() && !isReload)?0:1000); }
    catch(TError &err) { io.setAttr("err", TSYS::strMess("10:%s",err.mess.c_str())); reset(); return; }

    Client::reqService(io);
    if(io.attr("err").empty()) tmDelay--;
    else reset();
}

void TMdContr::protIO( MMS::XML_N &io )
{
    MtxAlloc resN(tr.at().reqRes(), true);
    if(messLev() == TMess::Debug) io.setAttr("debug", "1");
    try { Client::protIO(io); }
    catch(TError &er) { io.setAttr("err", TSYS::strMess("%s:%s", _("Remote host error"), er.mess.c_str())); }
}

int TMdContr::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib )
{
    return tr.at().messIO(obuf, len_ob, ibuf, len_ib, ((enableStat() && !isReload)?0:1000));
}

void TMdContr::debugMess( const string &mess )	{ mess_debug_(nodePath().c_str(), "%s", mess.c_str()); }

string TMdContr::getNameList( const string &domain )
{
    string rez;

    //Check cache
    MtxAlloc res(dataRes(), true);
    map<string, NamesCacheEl>::iterator cEl = namesCache.find(domain);
    if(cEl != namesCache.end() && (!syncPer() || SYS->sysTm() < (cEl->second.tm+syncPer())) && (cEl->second.nms.size() || (startStat() && !connOK())))
	return cEl->second.nms;
    res.unlock();

    if(domain.empty()) {	//Get domains list
	MMS::XML_N reqDom("MMS");
	reqDom.setAttr("id", "getNameList")->setAttr("objectClass", i2s(MMS::OCL_Domain));
	reqService(reqDom);
	for(unsigned i_d = 0; reqDom.attr("err").empty() && i_d < reqDom.childSize(); i_d++)
	    rez += reqDom.childGet(i_d)->text() + "\n";
    }
    else {			//Get names list
	MMS::XML_N reqVar("MMS");
	reqVar.setAttr("moreFollows", "");
	string continueAfter;
	do {
	    reqVar.clear()->setAttr("id", "getNameList")->setAttr("objectClass", i2s(MMS::OCL_NmVar))->setAttr("continueAfter", continueAfter);
	    if(domain != "*") reqVar.setAttr("domainSpecific", domain);
	    reqService(reqVar);
	    for(unsigned i_v = 0; reqVar.attr("err").empty() && i_v < reqVar.childSize(); i_v++)
		rez += domain + "/" + (continueAfter=reqVar.childGet(i_v)->text()) + "\n";
	} while(s2i(reqVar.attr("moreFollows")));
    }

    res.lock();
    namesCache[domain] = NamesCacheEl(SYS->sysTm(), rez);

    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name, &owner().tpPrmAt(type)); }

void TMdContr::enable_( )
{
    string trName = "Sockets.out_MMS"+id();
    tr = SYS->transport().at().nodeAt(trName, 0, '.', 0, true);
    if(tr.freeStat()) {
	SYS->transport().at().at(TSYS::strParse(trName,0,".")).at().outAdd(TSYS::strParse(trName,1,".").substr(4));
	tr = SYS->transport().at().nodeAt(trName, 0, '.');
	tr.at().setDscr(TSYS::strMess(_("MMS automatic created transport for '%s' controller."),id().c_str()));
    }
    tr.at().setAddr("TCP:"+addr());

    reset();	//MMS coneection state reset
}

void TMdContr::disable_( )
{
    tr.free();
    mVars.clear();

    MtxAlloc res(dataRes(), true);
    namesCache.clear();
}

void TMdContr::start_( )
{
    reset();	//MMS coneection state reset

    tmDelay = 0;

    //Clear data blocks
    mVars.clear();

    //Reenable parameters
    try {
	vector<string> pls;
	list(pls);

	isReload = true;
	for(unsigned i_p = 0; i_p < pls.size(); i_p++)
	    if(at(pls[i_p]).at().enableStat()) at(pls[i_p]).at().enable();
	isReload = false;
    } catch(TError&) { isReload = false; throw; }

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true));

    alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("STOP")), TMess::Info);
    alSt = -1;

    //Set EVal
    MtxAlloc res(enRes, true);
    for(unsigned i_p = 0; i_p < pHD.size(); i_p++)
	pHD[i_p].at().setEval();
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHD.size(); i_prm++)
	if(&pHD[i_prm].at() == prm) break;

    if(val && i_prm >= pHD.size())	pHD.push_back(prm);
    if(!val && i_prm < pHD.size())	pHD.erase(pHD.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    vector<string> als;
    string	nId;
    TMdContr	&cntr = *(TMdContr *)icntr;

    bool firstCall = true;
    cntr.prcSt = true;

    for(unsigned int it_cnt = cntr.pHD.size(); !TSYS::taskEndRun(); it_cnt++) {
	if(cntr.redntUse()) { TSYS::taskSleep(1e9); continue; }
	if(cntr.tmDelay > 0){ TSYS::taskSleep(1e9); cntr.tmDelay = vmax(0,cntr.tmDelay-1); continue; }

	//Prepare and read block variables
	MMS::XML_N valCtr("MMS"), *value = NULL;
	valCtr.setAttr("id", "read");
	MtxAlloc res(cntr.enRes, true);
	bool isErr = valCtr.attr("err").size();
	unsigned viCnt = 0;
	for(map<string,VarStr>::iterator vi = cntr.mVars.begin(); true; ++vi, ++viCnt) {
	    // Send the request
	    if(vi == cntr.mVars.end() || valCtr.childSize() >= cntr.mVarsRdReq.getI() ||
		(valCtr.childSize() && (vi->second.single || s2i(valCtr.childGet(valCtr.childSize()-1)->attr("single")))))
	    {
		if(!valCtr.childSize())	break;
		if(!isErr) {
		    cntr.reqService(valCtr);
		    if((isErr=valCtr.attr("err").size())) {
			cntr.acq_err.setVal(valCtr.attr("err"));
			mess_err(cntr.nodePath().c_str(), "%s", cntr.acq_err.getVal().c_str());
			if(cntr.alSt <= 0) {
			    cntr.alSt = 1;
			    cntr.alarmSet(TSYS::strMess(_("Connection to the data source: %s."),
				TRegExp(":","g").replace(cntr.acq_err.getVal(),"=").c_str()));
			}
			cntr.tmDelay = cntr.restTm();
		    }
		    else {
			cntr.acq_err.setVal("");
			if(cntr.alSt != 0) {
			    cntr.alSt = 0;
			    cntr.alarmSet(TSYS::strMess(_("Connection to the data source: %s."),_("OK")), TMess::Info);
			}
		    }
		}

		//  Process result
		for(unsigned i_ch = 0; i_ch < valCtr.childSize(); i_ch++) {
		    value = valCtr.childGet(i_ch);
		    nId = (value->attr("domainId").size()?value->attr("domainId"):"*")+"/"+value->attr("itemId");
		    if(isErr || value->attr("err").size()) value = NULL;
		    if(!value) { cntr.mVars[nId].val = TVariant(EVAL_REAL); continue; }
		    switch(s2i(value->attr("tp"))) {
			case MMS::VT_Bool: case MMS::VT_Int: case MMS::VT_UInt:
			case MMS::VT_Float:
			case MMS::VT_BitString: case MMS::VT_OctString: case MMS::VT_VisString:
			    cntr.mVars[nId].val = value->text();
			    break;
			case MMS::VT_Array: case MMS::VT_Struct: {		//?!?! Need for test
			    vector<StackTp> stack;
			    TArrayObj *curArr = new TArrayObj();
			    cntr.mVars[nId].val = curArr;
			    MMS::XML_N *curValue = value;
			    for(unsigned i_v = 0; true; ) {
				if(i_v >= curValue->childSize()) {
				    if(stack.size()) {
					curArr = stack.back().arr;
					curValue = stack.back().vl;
					i_v = stack.back().inPos+1;
					stack.pop_back();
					continue;
				    }
				    else break;
				}

				MMS::XML_N *itValue = curValue->childGet(i_v);
				switch(s2i(itValue->attr("tp"))) {
				    case MMS::VT_Bool: curArr->arSet(i_v, bool(s2i(itValue->text())));	break;
				    case MMS::VT_Int: case MMS::VT_UInt:
					curArr->arSet(i_v, (int64_t)atoll(itValue->text().c_str()));
					break;
				    case MMS::VT_Float: curArr->arSet(i_v, s2r(itValue->text()));	break;
				    case MMS::VT_BitString: case MMS::VT_OctString: case MMS::VT_VisString:
					curArr->arSet(i_v, itValue->text());
					break;
				    case MMS::VT_Array: case MMS::VT_Struct: {
					stack.push_back(StackTp(curArr,curValue,i_v));
					curValue = itValue;
					TArrayObj *itArr = new TArrayObj();
					curArr->arSet(i_v, itArr);
					curArr = itArr;
					i_v = 0;
					continue;
				    }
				}
				i_v++;
			    }
			    break;
			}
			//case MMS_UTC_TIME: pVal.at().setI(MmsValue_toUnixTimestamp(value), 0, true);		break;
			default:
			    mess_err(cntr.nodePath().c_str(), _("Value '%s' type '%s' read is not implemented."),
								nId.c_str(), value->attr("tp").c_str());
			    break;
		    }
		}

		valCtr.childClear();
		if(vi == cntr.mVars.end()) break;
	    }

	    if(firstCall || !vi->second.div || !((it_cnt+viCnt)%vi->second.div)) {
		value = valCtr.childAdd("it")->setAttr("single", i2s(vi->second.single))->setAttr("itemId", TSYS::pathLev(vi->first,1));
		if(TSYS::pathLev(vi->first,0) != "*") value->setAttr("domainId", TSYS::pathLev(vi->first,0));
	    }
	}

	//Update controller's data
	cntr.callSt = true;
	unsigned int div = cntr.period() ? (unsigned int)(cntr.syncPer()/(1e-9*cntr.period())) : 0;
	bool forceUpd = (firstCall || cntr.tmDelay == 0);
	for(unsigned i_p = 0; i_p < cntr.pHD.size() && !cntr.redntUse() && !TSYS::taskEndRun(); i_p++)
	    try {
		cntr.pHD[i_p].at().vlList(als);
		if((forceUpd && (i_p == 0 || cntr.tmDelay < 0)) || (div && (it_cnt%div) < cntr.pHD.size())) {
		    string aPrcErr = cntr.pHD[i_p].at().attrPrc();
		    if(!cntr.mVars.size()) {
			cntr.acq_err.setVal(aPrcErr);
			if(cntr.alSt <= 0) {
			    cntr.alSt = 1;
			    cntr.alarmSet(TSYS::strMess(_("Connection to the data source: %s."),
						(aPrcErr.size()?TRegExp(":","g").replace(cntr.acq_err.getVal(),"=").c_str():_("No data"))));
			}
			cntr.tmDelay = cntr.restTm();
		    }
		}

		//Attributes update
		res.lock();
		for(unsigned i_a = 0; i_a < als.size(); i_a++) {
		    AutoHD<TVal> pVal = cntr.pHD[i_p].at().vlAt(als[i_a]);
		    if(!(nId=TSYS::strLine(pVal.at().fld().reserve(),0)).empty()) pVal.at().set(cntr.mVars[nId].val, 0, true);
		}
		res.unlock();
	    } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

	cntr.callSt = firstCall = false;
	res.unlock();

	if(TSYS::taskEndRun()) break;
	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prcSt = false;

    return NULL;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    try {
	if(co.fld().name() == "SCHEDULE")
	    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;
	else if(co.name() == "ADDR" && enableStat()) tr.at().setAddr("TCP:"+co.getS());
    } catch(...) { }

    return true;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    "dest","sel_ed", "sel_list",TMess::labSecCRONsel(), "help",TMess::labSecCRON(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID, "help",TMess::labTaskPrior(), NULL);
	ctrMkNode2("fld",opt,-1,"/cntr/cfg/SYNCPER",EVAL_STR,RWRWR_,"root",SDAQ_ID, "help",_("Zero for disable periodic sync."), NULL);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name,tp_prm), p_el("w_attr")
{

}

TMdPrm::~TMdPrm( )	{ nodeDelAll(); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat() && !owner().isReload) return;
    TParamContr::enable();

    attrPrc();

    owner().prmEn(this, true);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(this, false);

    TParamContr::disable();

    setEval();
}

string TMdPrm::attrPrc( )
{
    if(owner().startStat() && !owner().connOK()) return "";

    vector<string> als;

    string conErr;
    string varLs = varList(), itS, var, opts, typeS, aid, anm;

    MMS::XML_N valCntr("MMS"), *value = valCntr.setAttr("id","read")->childAdd("it");
    for(int off = 0; (itS=TSYS::strLine(varLs,0,&off)).size(); ) {
	if(itS[0] == '#') continue;
	int offIt = 0;
	var = TSYS::strParse(itS,0,":",&offIt);

	opts = TSYS::strParse(itS,0,":",&offIt);
	unsigned flg = (opts.find("w") != string::npos) ? unsigned(TVal::DirWrite) : unsigned(TFld::NoWrite);

	typeS = TSYS::strParse(itS,0,":",&offIt);

	aid = TSYS::strParse(itS,0,":",&offIt);
	aid = TSYS::strEncode((aid.empty()?TSYS::pathLev(var,1):aid), TSYS::oscdID);
	if(vlPresent(aid)) for(aid += "_1"; vlPresent(aid); ) aid = TSYS::strLabEnum(aid);

	anm = TSYS::strParse(itS,0,":",&offIt);
	if(anm.empty()) anm = TSYS::pathLev(var, 1);

	// Find for already presented attribute
	unsigned srchPos;
	for(srchPos = 0; srchPos < p_el.fldSize(); srchPos++)
	    if(TSYS::strLine(p_el.fldAt(srchPos).reserve(),0) == var) break;
	if(srchPos < p_el.fldSize()) {
	    p_el.fldAt(srchPos).setFlg(flg);	//RW change update
	    p_el.fldAt(srchPos).setDescr(anm);	//Name change update
	    als.push_back(var);
	    owner().regVar(var, opts);
	    continue;
	}

	// Get type from config
	int vtp = -1, vMMStp = MMS::VT_Float;
	if(typeS.size()) {
	    if(typeS.find("bool") == 0)		{ vtp = TFld::Boolean; vMMStp = MMS::VT_Bool; }
	    else if(typeS.find("real") == 0)	{ vtp = TFld::Real; vMMStp = MMS::VT_Float; }
	    else if(typeS.find("int") == 0)	{ vtp = TFld::Integer; vMMStp = MMS::VT_Int; }
	    else if(typeS.find("string") == 0)	{ vtp = TFld::String; vMMStp = MMS::VT_VisString; }
	    else if(typeS.find("struct") == 0)	{ vtp = TFld::Object; vMMStp = MMS::VT_Struct; }
	    else if(typeS.find("array") == 0)	{ vtp = TFld::Object; vMMStp = MMS::VT_Array; }
	}

	// Request value for the type obtain
	if(vtp < 0) {
	    if(s2i(conErr) == TError::Tr_Connect)	continue;
	    value->setAttr("itemId", TSYS::pathLev(var,1));
	    if(TSYS::pathLev(var,0) != "*") value->setAttr("domainId", TSYS::pathLev(var,0));
	    owner().reqService(valCntr);
	    if((conErr=valCntr.attr("err")).size() || value->attr("err").size()) continue;
	    switch((vMMStp=s2i(value->attr("tp")))) {
		case MMS::VT_Bool:	vtp = TFld::Boolean;		break;
		case MMS::VT_Int: case MMS::VT_UInt: vtp = TFld::Integer;	break;
		case MMS::VT_Float:	vtp = TFld::Real;		break;
		case MMS::VT_BitString: case MMS::VT_VisString: case MMS::VT_OctString: vtp = TFld::String;	break;
		case MMS::VT_Struct:	vtp = TFld::Object;		break;
		case MMS::VT_Array: vtp = TFld::Object;			break;
		default: mess_err(nodePath().c_str(), _("Value type '%s' is not implemented for '%s'."), value->attr("tp").c_str(), var.c_str());
	    }
	}

	// Create new attribute
	if(vtp >= 0) {
	    p_el.fldAdd(new TFld(aid.c_str(),anm.c_str(),(TFld::Type)vtp,flg,"","","","",(var+"\n"+i2s(vMMStp)).c_str()));
	    als.push_back(var);
	    owner().regVar(var, opts);
	}
    }

    //Find for delete attribute
    for(unsigned i_a = 0, i_p; i_a < p_el.fldSize(); ) {
	for(i_p = 0; i_p < als.size(); i_p++)
	    if(TSYS::strLine(p_el.fldAt(i_a).reserve(),0) == als[i_p]) break;
	if(i_p >= als.size())
	    try{ p_el.fldDel(i_a); continue; } catch(TError &err) { }
	i_a++;
    }

    return conErr;
}

void TMdPrm::setEval( )
{
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.compare(0,6,"/serv/") == 0) { TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode2("fld",opt,-1,"/prm/cfg/VAR_LS",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,"SnthHgl","1","help",
	    _("Attributes configuration list. List must be written by lines in format: \"{MMS_domain}/{MMS_var}[:{opt}[:{tp}[:{id}[:{name}]]]]\".\n"
	      "Where:\n"
	      "  {MMS_domain} - MMS domain or '*' for global.\n"
	      "  {MMS_var} - MMS variable name into domain or global.\n"
	      "  {opt} - Options:\n"
	      "    w - writable;\n"
	      "    s - single request;\n"
	      "    /{NN} - call at cycle even to number {N} [2...99].\n"
	      "  {tp} - force type:\n"
	      "    bool - boolean;\n"
	      "    real - real;\n"
	      "    int - integer;\n"
	      "    string - string;\n"
	      "    struct - structure;\n"
	      "    array - array;\n"
	      "  {id} - force attribute ID.\n"
	      "  {name} - force attribute name."),NULL);
	if(!owner().startStat() || owner().connOK())
	    ctrMkNode("fld",opt,-1,"/prm/cfg/SEL_VAR",_("Variable append"),enableStat()?0:RWRW__,"root",SDAQ_ID,2,"dest","select","select","/prm/cfg/SEL_VAR_lst");
	return;
    }

    //Process command to page
    if(a_path == "/prm/cfg/VAR_LS" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	opt->childAdd("rule")->setAttr("expr","^#[^\n]*")->setAttr("color","gray")->setAttr("font_italic","1");
	opt->childAdd("rule")->setAttr("expr","^[^:]+($|:[^:]*($|:[^:]*($|:[^:]+($|:.+))))")->setAttr("color","green")->
	    childAdd("rule")->setAttr("expr","^[^:]+($|:[^:]*($|:[^:]*))")->setAttr("color","darkorange")->
		childAdd("rule")->setAttr("expr","^[^:]+($|:[^:]*)")->setAttr("color","red")->
		    childAdd("rule")->setAttr("expr","^[^:]+")->setAttr("color","blue");
    }
    else if(a_path == "/prm/cfg/SEL_VAR") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"selVAR","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	    if(TSYS::pathLev(opt->text(),1).size()) {
		string vLs = varList(), vS;
		for(int off = 0; (vS=TSYS::strLine(vLs,0,&off)).size(); )
		    if(TSYS::strParse(vS,0,":") == opt->text())
			break;
		if(vS.empty()) setVarList(vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+opt->text());
	    }
	    TBDS::genPrmSet(owner().nodePath()+"selVAR",TSYS::pathLev(opt->text(),0),opt->attr("user"));
	}
    }
    else if(a_path == "/prm/cfg/SEL_VAR_lst" && ctrChkNode(opt)) {
	string	selVAR = TSYS::pathLev(TBDS::genPrmGet(owner().nodePath()+"selVAR","",opt->attr("user")), 0);
	string	lst = owner().getNameList(selVAR), lstEl;
	opt->childAdd("el")->setText(selVAR.empty()?"*":"");
	for(int off = 0; (lstEl=TSYS::strLine(lst,0,&off)).size(); ) opt->childAdd("el")->setText(lstEl);

	/*if(selVAR.empty()) {	//Get domain list
	    MMS::XML_N reqDom("MMS");
	    reqDom.setAttr("id", "getNameList")->setAttr("objectClass", i2s(MMS::OCL_Domain));
	    owner().reqService(reqDom);
	    opt->childAdd("el")->setText("*");
	    for(unsigned i_d = 0; reqDom.attr("err").empty() && i_d < reqDom.childSize(); i_d++)
		opt->childAdd("el")->setText(reqDom.childGet(i_d)->text());
	}
	else {			//Get names list
	    opt->childAdd("el")->setText("");
	    MMS::XML_N reqVar("MMS");
	    reqVar.setAttr("moreFollows", "");
	    string continueAfter;
	    do {
		reqVar.clear()->setAttr("id", "getNameList")->setAttr("objectClass", i2s(MMS::OCL_NmVar))->setAttr("continueAfter", continueAfter);
		if(selVAR != "*") reqVar.setAttr("domainSpecific", selVAR);
		owner().reqService(reqVar);
		for(unsigned i_v = 0; reqVar.attr("err").empty() && i_v < reqVar.childSize(); i_v++)
		    opt->childAdd("el")->setText(selVAR+"/"+(continueAfter=reqVar.childGet(i_v)->text()));
	    } while(s2i(reqVar.attr("moreFollows")));
	}*/
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod((int64_t)(owner().period()*1000000));
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
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

    if(!owner().acq_err.getVal().empty()) val.setS(owner().acq_err.getVal(),0,true);
    else val.setS("0",0,true);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setS(EVAL_STR, 0, true); return; }

    //Send to active reserve station
    if(vlSetRednt(vo,vl,pvl))	return;

    if(vl.isEVal() || vl == pvl)return;

    int off = 0;
    string nId = TSYS::strLine(vo.fld().reserve(), 0, &off);
    int vTp = s2i(TSYS::strLine(vo.fld().reserve(),0,&off));

    MMS::XML_N valCtr("MMS");
    MMS::XML_N *value = valCtr.setAttr("id", "write")->
		    childAdd("it")->setAttr("itemId", TSYS::pathLev(nId,1))->setAttr("dataType", i2s(vTp));
    if(TSYS::pathLev(nId,0) != "*") value->setAttr("domainId", TSYS::pathLev(nId,0));
    switch(vTp) {
	case MMS::VT_Array: case MMS::VT_Struct: {	//?!?! Need for test
	    TArrayObj *curArr = NULL;
	    if(vl.type() != TVariant::Object || !(curArr=dynamic_cast<TArrayObj*>(&vl.getO().at()))) break;
	    vector<TMdContr::StackTp> stack;
	    MMS::XML_N *curValue = value;
	    for(unsigned i_v = 0; true; ) {
		if(i_v >= curArr->arSize()) {
		    if(stack.size()) {
			curArr = stack.back().arr;
			curValue = stack.back().vl;
			i_v = stack.back().inPos+1;
			stack.pop_back();
			continue;
		    }
		    else break;
		}

		MMS::XML_N *itValue = curValue->childAdd("it");
		TVariant itArr = curArr->arGet(i_v);
		switch(itArr.type())
		{
		    case TVariant::Boolean: itValue->setAttr("dataType",i2s(MMS::VT_Bool))->setText(itArr.getS());	break;
		    case TVariant::Integer: itValue->setAttr("dataType",i2s(MMS::VT_Int))->setText(itArr.getS());	break;
		    case TVariant::Real: itValue->setAttr("dataType",i2s(MMS::VT_Float))->setText(itArr.getS());	break;
		    case TVariant::String: itValue->setAttr("dataType",i2s(MMS::VT_VisString))->setText(itArr.getS());	break;
		    case TVariant::Object:
			if(!dynamic_cast<TArrayObj*>(&itArr.getO().at())) itValue->parent()->childDel(itValue);
			else {
			    stack.push_back(TMdContr::StackTp(curArr,curValue,i_v));
			    curValue = itValue;
			    curArr = (TArrayObj*)(&itArr.getO().at());
			    i_v = 0;
			    continue;
			}
		    default:	break;
		}
		i_v++;
	    }
	    break;
	}
	default: value->setText(vl.getS());
    }
    owner().reqService(valCtr);
    if(!valCtr.attr("err").empty()) {
	vo.setS(EVAL_STR, 0, true);
	if(owner().messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),_("Write to '%s' error: %s."),nId.c_str(),value->attr("err").c_str());
    }
}

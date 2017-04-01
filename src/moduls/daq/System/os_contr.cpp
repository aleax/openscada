
//OpenSCADA system module DAQ.System file: os_contr.cpp
/***************************************************************************
 *   Copyright (C) 2005-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttransports.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include "da_cpu.h"
#include "da_mem.h"
#include "da_uptime.h"
#include "da_hddtemp.h"
#include "da_sensors.h"
#include "da_smart.h"
#include "da_hddstat.h"
#include "da_netstat.h"
#include "da_ups.h"
#include "da_fs.h"
#include "os_contr.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"System"
#define MOD_NAME	_("System DA")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"2.1.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides data acquisition from the OS. Supported OS Linux data sources: HDDTemp, Sensors, Uptime, Memory, CPU, UPS etc.")
#define LICENSE		"GPL2"
//*************************************************

SystemCntr::TTpContr *SystemCntr::mod;  //Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_System_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_System_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new SystemCntr::TTpContr(source);
	return NULL;
    }
}

using namespace SystemCntr;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr( )
{
    nodeDelAll();
    for(unsigned iDA = 0; iDA < mDA.size(); iDA++)	delete mDA[iDA];
    mDA.clear();
}

void TTpContr::load_( )
{
    //Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Init DA sources
    daReg(new CPU());
    daReg(new Mem());
    daReg(new Sensors());
    daReg(new Hddtemp());
    daReg(new UpTime());
    daReg(new HddSmart());
    daReg(new HddStat());
    daReg(new NetStat());
    daReg(new UPS());
    daReg(new FS());

    //Controler's bd structure
    fldAdd(new TFld("AUTO_FILL",_("Auto create active DA"),TFld::Integer,TFld::Selected,"1","0","0;1;2;3",_("Manual;Fast sources;Slow sources;All sources")));
    fldAdd(new TFld("PRM_BD",_("System parameters table"),TFld::String,TFld::NoFlag,"30","system"));
    fldAdd(new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","0","0;10000"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));

    //Parameter type bd structure
    // Make enumerated
    string el_id,el_name,el_def;
    vector<string> list;
    daList(list);
    for(unsigned iLs = 0; iLs < list.size(); iLs++) {
	if(iLs == 0)	el_def = list[iLs];
	el_id += list[iLs]+";";
	el_name = el_name+_(daGet(list[iLs])->name().c_str())+";";
    }
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("TYPE",_("System part"),TFld::String,TFld::Selected|TCfg::NoVal,"10",el_def.c_str(),el_id.c_str(),el_name.c_str()));
    tpPrmAt(t_prm).fldAdd(new TFld("SUBT" ,"",TFld::String,TFld::Selected|TCfg::NoVal|TFld::SelfFld,"255"));
    tpPrmAt(t_prm).fldAdd(new TFld("ADD_PRMS",_("Additional parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

void TTpContr::daList( vector<string> &da )
{
    da.clear();
    for(unsigned iDA = 0; iDA < mDA.size(); iDA++)
	da.push_back(mDA[iDA]->id());
}

void TTpContr::daReg( DA *da )	{ mDA.push_back(da); }

DA *TTpContr::daGet( const string &da )
{
    for(unsigned iDA = 0; iDA < mDA.size(); iDA++)
	if(mDA[iDA]->id() == da) return mDA[iDA];

    return NULL;
}

void TTpContr::perSYSCall( unsigned int cnt )
{
    //Recheck auto-controllers to create parameters for new devices
    vector<string> clist;
    list(clist);
    for(unsigned iC = 0; iC < clist.size(); iC++)
	at(clist[iC]).at().devUpdate();
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem) : TController(name_c,daq_db,cfgelem),
    mPerOld(cfg("PERIOD").getId()), mPrior(cfg("PRIOR").getId()),
    prcSt(false), callSt(false), endrunReq(false), mPer(1e9)
{
    cfg("PRM_BD").setS("OSPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	if(callSt)	rez += TSYS::strMess(_("Call now. "));
	if(period())	rez += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-9*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "), atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s[%s]. "),
		tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str());
    }
    return rez;
}

void TMdContr::devUpdate( )
{
    int aFill = cfg("AUTO_FILL").getI();
    if(enableStat() && aFill) {
	vector<string> list;
	mod->daList(list);
	for(unsigned iL = 0; iL < list.size(); iL++)
	    if((mod->daGet(list[iL])->isSlow() && aFill&0x02) || (!mod->daGet(list[iL])->isSlow() && aFill&0x01))
		mod->daGet(list[iL])->makeActiveDA(this);
    }
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    //TController::load_();

    //Check for get old period method value
    if(mPerOld) { cfg("SCHEDULE").setS(r2s(mPerOld/1e3)); mPerOld = 0; }
}

void TMdContr::enable_( )
{
    enSt = true;
    devUpdate();
}

void TMdContr::start_( )
{
    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,1e9*s2r(cron())) : 0;

    //Start the request data task
    if(!prcSt) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    if(prcSt) SYS->taskDestroy(nodePath('.',true), &endrunReq);

    //Set Eval for parameters
    ResAlloc res(enRes, true);
    for(unsigned iPrm = 0; iPrm < pHd.size(); iPrm++)
	pHd[iPrm].at().setEval();
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res(enRes, true);
    unsigned iPrm;
    for(iPrm = 0; iPrm < pHd.size(); iPrm++)
	if(pHd[iPrm].at().id() == id) break;

    if(val && iPrm >= pHd.size())	pHd.push_back(at(id));
    if(!val && iPrm < pHd.size())	pHd.erase(pHd.begin()+iPrm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    while(!cntr.endrunReq) {
	if(!cntr.redntUse()) {
	    //Update controller's data
	    cntr.enRes.resRequestR();
	    cntr.callSt = true;
	    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		try { cntr.pHd[iP].at().getVal(); }
		catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	    cntr.callSt = false;
	    cntr.enRes.resRelease();
	}

	TSYS::taskSleep((int64_t)cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	return;
    }
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) :
    TParamContr(name,tp_prm), daData(NULL), mAuto(false), mDA(NULL)
{

}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);

    vector<string> list;
    mod->daList(list);
    if(list.size())	cfg("TYPE").setS(list[0]);
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
    setType("");
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;
    cfg("TYPE").setS(cfg("TYPE").getS());
    TParamContr::enable();
    ((TMdContr&)owner()).prmEn(id(), true);	//Put to process
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;
    ((TMdContr&)owner()).prmEn(id(), false);	//Remove from process
    setEval();
    TParamContr::disable();
}

void TMdPrm::load_( TConfig *cfg )
{
    if(!mAuto)	TParamContr::load_(cfg);
}

void TMdPrm::save_( )
{
    if(!mAuto)	TParamContr::save_();

    //Save archives
    vector<string> aLs;
    vlList(aLs);
    for(unsigned iA = 0; iA < aLs.size(); iA++)
	if(!vlAt(aLs[iA]).at().arch().freeStat())
	    vlAt(aLs[iA]).at().arch().at().save();
}

void TMdPrm::vlGet( TVal &val )
{
    if(val.name() == "err") {
	if(!owner().startStat())val.setS(_("2:Controller stopped"), 0, true);
	else if(!enableStat())	val.setS(_("1:Parameter disabled"), 0, true);
	else if(daErr.size())	val.setS(daErr, 0, true);
	else val.setS("0", 0, true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setI(EVAL_INT, 0, true); return; }

    //Send to active reserve station
    if(owner().redntUse()) {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //Direct write
    try { if(mDA) mDA->vlSet(this, vo, vl, pvl); }
    catch(TError &err) {
	mess_err(nodePath().c_str(),_("Write value to attribute '%s' error: %s"),vo.name().c_str(),err.mess.c_str());
	vo.setS(pvl.getS(), 0, true);
    }
}

void TMdPrm::getVal( )
{
    if(mDA) mDA->getVal(this);
}

void TMdPrm::setEval( )
{
    if(!mDA)	return;

    vector<string> als;
    mDA->fldList(als);
    if(als.size()) {
	for(unsigned iA = 0; iA < als.size(); iA++)
	    if(vlPresent(als[iA]))
		vlAt(als[iA]).at().setS(EVAL_STR,0,true);
    }
    else {
	vlList(als);
	for(unsigned iA = 0; iA < als.size(); iA++)
	    if(!(als[iA] == "SHIFR" || als[iA] == "OWNER" || als[iA] == "NAME" || als[iA] == "DESCR" || als[iA] == "err"))
		vlAt(als[iA]).at().setS(EVAL_STR,0,true);
    }
}

void TMdPrm::vlElemAtt( TElem *ValEl )	{ TValue::vlElemAtt(ValEl); }

void TMdPrm::vlElemDet( TElem *ValEl )	{ TValue::vlElemDet(ValEl); }

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? (int64_t)owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

void TMdPrm::setType( const string &da_id )
{
    if(mDA && da_id == mDA->id())	return;

    //Free previous type
    if(mDA) {
	mDA->deInit(this);
	vlElemDet(mDA);
	mDA = NULL;
    }

    //Create new type
    try {
	if(da_id.size() && (mDA=mod->daGet(da_id))) {
	    daErr = "";
	    vlElemAtt(mDA);
	    mDA->init(this);
	}
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }
}

string TMdPrm::addPrm( const string &prm, const string &def )
{
    string rez;
    XMLNode prmNd;
    try {
	prmNd.load(cfg("ADD_PRMS").getS());
	string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
	if(!sa.size())	return (rez=prmNd.attr(prm)).empty() ? def : rez;
	//Internal node
	for(unsigned i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
		return (rez=prmNd.childGet(i_n)->attr(sa)).empty() ? def : rez;
    } catch(...) { }

    return def;
}

void TMdPrm::setAddPrm( const string &prm, const string &val )
{
    XMLNode prmNd("cfg");
    try { prmNd.load(cfg("ADD_PRMS").getS()); } catch(...) { }

    if(addPrm(prm) != val) modif();
    string sobj = TSYS::strParse(prm, 0, ":"), sa = TSYS::strParse(prm, 1, ":");
    if(!sa.size()) prmNd.setAttr(prm, val);

    //Internal node
    else {
	unsigned i_n;
	for(i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
	    { prmNd.childGet(i_n)->setAttr(sa,val); break; }
	if(i_n >= prmNd.childSize())
	    prmNd.childAdd(sobj)->setAttr(sa,val);
    }

    cfg("ADD_PRMS").setS(prmNd.save(XMLNode::BrAllPast));
    autoC(false);
}

bool TMdPrm::cfgChange( TCfg &co, const TVariant &pc )
{
    //Change TYPE parameter
    if(co.name() == "TYPE") { setType(co.getS()); return true; }
    if(mDA) mDA->cfgChange(co, pc);
    if(!autoC()) modif();
    return true;
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/ADD_PRMS");
	if(mDA) mDA->cntrCmdProc(this, opt);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(mDA && mDA->cntrCmdProc(this,opt)) ;
    else TParamContr::cntrCmdProc(opt);
}

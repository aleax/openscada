
//OpenSCADA system module DAQ.System file: os_contr.cpp
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <ttiparam.h>
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
#include "os_contr.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"System"
#define MOD_NAME	_("System DA")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"1.7.5"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow operation system data acquisition. Support OS Linux data sources:\
 HDDTemp, Sensors, Uptime, Memory, CPU, UPS and other.")
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

TTpContr::~TTpContr( )
{
    nodeDelAll();
    for(unsigned i_da = 0; i_da < mDA.size(); i_da++)	delete mDA[i_da];
    mDA.clear();
}

void TTpContr::load_( )
{
    //Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

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

    //Controler's bd structure
    fldAdd(new TFld("AUTO_FILL",_("Auto create active DA"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("PRM_BD",_("System parameters table"),TFld::String,TFld::NoFlag,"30","system"));
    fldAdd(new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","0","0;10000"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Request task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));

    //Parameter type bd structure
    // Make enumerated
    string el_id,el_name,el_def;
    vector<string> list;
    daList(list);
    for(unsigned i_ls = 0; i_ls < list.size(); i_ls++) {
	if(i_ls == 0)	el_def = list[i_ls];
	el_id += list[i_ls]+";";
	el_name = el_name+_(daGet(list[i_ls])->name().c_str())+";";
    }
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("TYPE",_("System part"),TFld::String,TFld::Selected|TCfg::NoVal,"10",el_def.c_str(),el_id.c_str(),el_name.c_str()));
    tpPrmAt(t_prm).fldAdd(new TFld("SUBT" ,"",TFld::String,TFld::Selected|TCfg::NoVal|TFld::SelfFld,"255"));
    tpPrmAt(t_prm).fldAdd(new TFld("ADD_PRMS",_("Additional parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

void TTpContr::daList( vector<string> &da )
{
    da.clear();
    for(unsigned i_da = 0; i_da < mDA.size(); i_da++)
	da.push_back(mDA[i_da]->id());
}

void TTpContr::daReg( DA *da )	{ mDA.push_back(da); }

DA *TTpContr::daGet( const string &da )
{
    for(unsigned i_da = 0; i_da < mDA.size(); i_da++)
	if(mDA[i_da]->id() == da) return mDA[i_da];

    return NULL;
}

void TTpContr::perSYSCall( unsigned int cnt )
{
    //Recheck auto-controllers to create parameters for new devices
    vector<string> clist;
    list(clist);
    for(unsigned i_c = 0; i_c < clist.size(); i_c++)
	at(clist[i_c]).at().devUpdate();
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem) : TController(name_c,daq_db,cfgelem),
    mPerOld(cfg("PERIOD").getId()), mPrior(cfg("PRIOR").getId()),
    prc_st(false), call_st(false), endrun_req(false), mPer(1e9), tm_calc(0)
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
	if(call_st)	rez += TSYS::strMess(_("Call now. "));
	if(period())	rez += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-3*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "), tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s. "), tm2s(tm_calc).c_str());
    }
    return rez;
}

void TMdContr::devUpdate( )
{
    if(enableStat() && cfg("AUTO_FILL").getB()) {
	vector<string> list;
	mod->daList(list);
	for(unsigned i_l = 0; i_l < list.size(); i_l++)
	    mod->daGet(list[i_l])->makeActiveDA(this);
    }
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    TController::load_();

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
    if(!prc_st) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.',true), &endrun_req);

    //Set Eval for parameters
    ResAlloc res(en_res, true);
    for(unsigned i_prm = 0; i_prm < p_hd.size(); i_prm++)
	p_hd[i_prm].at().setEval();
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res(en_res, true);
    unsigned i_prm;
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while(!cntr.endrun_req) {
	if(!cntr.redntUse()) {
	    cntr.call_st = true;
	    //Update controller's data
	    try {
		int64_t t_cnt = TSYS::curTime();

		cntr.en_res.resRequestR();
		for(unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++)
		    cntr.p_hd[i_p].at().getVal();
		cntr.en_res.resRelease();

		cntr.tm_calc = TSYS::curTime()-t_cnt;
	    } catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	    cntr.call_st = false;
	}

	TSYS::taskSleep((int64_t)cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
    }

    cntr.prc_st = false;

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
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
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

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

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

void TMdPrm::load_( )
{
    if(!mAuto)	TParamContr::load_();
}

void TMdPrm::save_( )
{
    if(!mAuto)	TParamContr::save_();

    //> Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
	if(!vlAt(a_ls[i_a]).at().arch().freeStat())
	    vlAt(a_ls[i_a]).at().arch().at().save();
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
    catch(TError err) {
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
	for(unsigned i_a = 0; i_a < als.size(); i_a++)
	    if(vlPresent(als[i_a]))
		vlAt(als[i_a]).at().setS(EVAL_STR,0,true);
    }
    else {
	vlList(als);
	for(unsigned i_a = 0; i_a < als.size(); i_a++)
	    if(!(als[i_a] == "SHIFR" || als[i_a] == "OWNER" || als[i_a] == "NAME" || als[i_a] == "DESCR" || als[i_a] == "err"))
		vlAt(als[i_a]).at().setS(EVAL_STR,0,true);
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
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }
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
    } catch(...){ }

    return def;
}

void TMdPrm::setAddPrm( const string &prm, const string &val )
{
    XMLNode prmNd("cfg");
    try { prmNd.load(cfg("ADD_PRMS").getS()); } catch(...){ }

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

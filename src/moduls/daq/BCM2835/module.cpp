
//OpenSCADA system module DAQ.BCM2835 file: module.cpp
/***************************************************************************
 *   Copyright (C) 2016 by Roman Savochenko, <rom_as@oscada.org>           *
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

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include <bcm2835.h>

#include "module.h"

//*************************************************
//* Module info!                                  *
#define MOD_ID		"BCM2835"
#define MOD_NAME	_("BCM 2835")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.1.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Broadcom BCM 2835 GPIO and other. Mostly for and used in Raspberry Pi.")
#define LICENSE		"GPL2"
//*************************************************

ModBCM::TTpContr *ModBCM::mod;	//Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int nMod ) {
	if(nMod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source ) {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new ModBCM::TTpContr(source);
	return NULL;
    }
}

using namespace ModBCM;

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

}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    //fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    //fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));

    //Parameter types and it's bd structure form
    int tPrm = tpParmAdd("std", "PRM_BD", _("Standard")/*, true*/);
    tpPrmAt(tPrm).fldAdd(new TFld("MOD_PRMS",_("Module addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem ) :
    ::TController(name_c,daq_db,cfgelem)/*, prcSt(false), callSt(false), tmGath(0),
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR"))*/
{
    cfg("PRM_BD").setS("TmplPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

/*string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	if(callSt)	rez += TSYS::strMess(_("Call now. "));
	if(period())	rez += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-3*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "), tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s."), tm2s(tmGath).c_str());
    }
    return rez;
}*/

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    if(!bcm2835_init())
	throw TError(nodePath().c_str(),_("Init bcm2835 library error whether that is not bcm2835 or there is not access."));
}

void TMdContr::disable_( )
{
    bcm2835_close();
}

void TMdContr::start_( )
{
    //Schedule process
    //mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

    //Start the gathering data task
    //SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    //SYS->taskDestroy(nodePath('.',true));
}

/*void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned iPrm;

    MtxAlloc res(enRes.mtx(), true);
    for(iPrm = 0; iPrm < pHD.size(); iPrm++)
	if(&pHD[iPrm].at() == prm) break;

    if(val && iPrm >= pHD.size())	pHD.push_back(prm);
    if(!val && iPrm < pHD.size())	pHD.erase(pHD.begin()+iPrm);
}*/

/*void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.prcSt = true;

    while(!TSYS::taskEndRun()) {
	int64_t t_cnt = TSYS::curTime();

	//Update controller's data
	cntr.enRes.lock();
	cntr.callSt = true;
	for(unsigned i_p = 0; i_p < cntr.pHD.size() && !cntr.redntUse(); i_p++)
	    try {
		//!!! Process parameter code
	    } catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	cntr.callSt = false;
	cntr.enRes.unlock();
	cntr.tmGath = TSYS::curTime() - t_cnt;

	TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
    }

    cntr.prcSt = false;

    return NULL;
}*/

/*void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	return;
    }

    //Process command to page
    TController::cntrCmdProc(opt);
}*/

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name, tp_prm)
{

}

TMdPrm::~TMdPrm( )
{
    disable();
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&elem())) vlElemAtt(&elem());
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    //Attributes populate
    //pEl.fldAdd(new TFld("gpio0","GPIO 0: v1P1.3",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //pEl.fldAdd(new TFld("gpio1","GPIO 1: v1P1.5",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio2","GPIO 2: {v2P1,J8}.3",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio3","GPIO 3: {v2P1,J8}.5",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio4","GPIO 4: 7",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio5","GPIO 5: {v2P1,J8}.29",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio6","GPIO 6: {v2P1,J8}.31",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio7","GPIO 7: 26",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio8","GPIO 8: 24",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio9","GPIO 9: 21",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio10","GPIO 10: 19",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio11","GPIO 11: 23",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio12","GPIO 12: {v2P1,J8}.32",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio13","GPIO 13: {v2P1,J8}.33",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio14","GPIO 14: 8",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio15","GPIO 15: 10",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio16","GPIO 16: {v2P1,J8}.36",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio17","GPIO 17: 11",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio18","GPIO 18: 12",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio19","GPIO 19: {v2P1,J8}.35",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio20","GPIO 20: {v2P1,J8}.38",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio21","GPIO 21: v1P1.13,{v2P1,J8}.40",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio22","GPIO 22: 15",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio23","GPIO 23: 16",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio24","GPIO 24: 11",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio25","GPIO 25: 22",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio26","GPIO 26: {v2P1,J8}.37",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio27","GPIO 27: {v2P1,J8}.13",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio28","GPIO 28: P5.3",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio29","GPIO 29: P5.4",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio30","GPIO 30: P5.5",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    pEl.fldAdd(new TFld("gpio31","GPIO 31: P5.6",TFld::Boolean,TVal::DirRead|TVal::DirWrite));

    //Init to direction and the reverse flag load
    vector<string> ls;
    pEl.fldList(ls);
    for(int iEl = 0; iEl < ls.size(); iEl++) {
	if(ls[iEl].compare(0,4,"gpio") != 0) continue;
	int pin = atoi(ls[iEl].c_str()+4);
	AutoHD<TVal> cVl = vlAt(ls[iEl]);
	if(s2i(modPrm(TSYS::strMess("GPIOout%d",pin)))) {
	    cVl.at().fld().setFlg(cVl.at().fld().flg()&(~TFld::NoWrite));
	    //bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	}
	else {
	    cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
	    //bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
	}
	cVl.at().fld().setReserve(modPrm(TSYS::strMess("GPIOrev%d",pin)));
    }

    TParamContr::enable();



    //owner().prmEn(this, true);
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    //owner().prmEn(this, false);

    TParamContr::disable();

    //Set EVAL to the parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int iEl = 0; iEl < ls.size(); iEl++)
	vlAt(ls[iEl]).at().setS(EVAL_STR, 0, true);
}

string TMdPrm::modPrm( const string &prm, const string &def )
{
    string rez;
    XMLNode prmNd;
    try {
	prmNd.load(cfg("MOD_PRMS").getS());
	string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
	if(!sa.size())	return (rez=prmNd.attr(prm)).empty()?def:rez;
	//Internal node
	for(unsigned i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
	return (rez=prmNd.childGet(i_n)->attr(sa)).empty()?def:rez;
    } catch(...){ }

    return def;
}

void TMdPrm::setModPrm( const string &prm, const string &val )
{
    XMLNode prmNd("ModCfg");
    try { prmNd.load(cfg("MOD_PRMS").getS()); } catch(...){ }

    if(modPrm(prm) != val) modif();
    string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
    if(!sa.size()) prmNd.setAttr(prm,val);
    //Internal node
    else {
	unsigned i_n;
	for(i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
	    { prmNd.childGet(i_n)->setAttr(sa,val); break; }
	if(i_n >= prmNd.childSize())
	    prmNd.childAdd(sobj)->setAttr(sa,val);
    }

    cfg("MOD_PRMS").setS(prmNd.save(XMLNode::BrAllPast));
}

void TMdPrm::vlGet( TVal &vo )
{
    if(!enableStat() || !owner().startStat()) {
	if(vo.name() == "err") {
	    if(!enableStat())			vo.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())	vo.setS(_("2:Acquisition/access is stopped."),0,true);
	}
	else vo.setR(EVAL_REAL, 0, true);
	return;
    }

    if(vo.name().compare(0,4,"gpio") == 0)
	vo.setB(bcm2835_gpio_lev(bool(s2i(vo.name().substr(4)))^bool(s2i(vo.fld().reserve()))), 0, true);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setR(EVAL_REAL, 0, true); return; }

    if(vl.isEVal() || vl == pvl) return;

    if(vo.name().compare(0,4,"gpio") == 0)
	bcm2835_gpio_write(s2i(vo.name().substr(4)), (vl.getB()^bool(s2i(vo.fld().reserve()))));
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr);
    val.arch().at().setPeriod(1000000/*(int64_t)(owner().period()*1000000)*/);	//One second
    val.arch().at().setHardGrid(true);
    //val.arch().at().setHighResTm(true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    string rez;

    //Get the page information
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrRemoveNode(opt, "/prm/cfg/MOD_PRMS");
	if(enableStat() && ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    vector<string> ls;
	    elem().fldList(ls);
	    for(int iL = 0; iL < ls.size(); iL++) {
		if(ls[iL].compare(0,4,"gpio") != 0) continue;
		int pin = atoi(ls[iL].c_str()+4);
		ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/gpioOut%d",pin).c_str(),TSYS::strMess(_("GPIO %d output and reverse"),pin).c_str(),
		    owner().startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/gpioRev%d",pin).c_str(),"",
		    owner().startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	}
	return;
    }

    //Process commands to the page
    string a_path = opt->attr("path");
    if(enableStat() && a_path.compare(0,12,"/cfg/gpioOut") == 0) {
	int pin = atoi(a_path.c_str()+12);
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(modPrm(TSYS::strMess("GPIOout%d",pin)));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm(TSYS::strMess("GPIOout%d",pin), opt->text());
    }
    else if(enableStat() && a_path.compare(0,12,"/cfg/gpioRev") == 0) {
	int pin = atoi(a_path.c_str()+12);
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(modPrm(TSYS::strMess("GPIOrev%d",pin)));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm(TSYS::strMess("GPIOrev%d",pin), opt->text());
    }
    else TParamContr::cntrCmdProc(opt);
}

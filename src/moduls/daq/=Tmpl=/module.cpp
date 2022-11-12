
//OpenSCADA module DAQ.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// System includings - add need ones
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// OpenSCADA API includings - add need ones
#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

// Own includings of the module - add need ones
#include "module.h"

//*************************************************
//* Module info!                                  *
#define MOD_ID		"Tmpl"
#define MOD_NAME	trS("DAQ template")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		trS("MyName MyFamily")
#define DESCRIPTION	trS("DAQ's subsystem template module.")
#define LICENSE		"MyLicense"
//*************************************************

DAQTmpl::TTpContr *DAQTmpl::mod;

// Required section for binding OpenSCADA core to this module, It gives information and creates module root object - do not change
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_Tmpl_module( int nMod )
#else
    TModule::SAt module( int nMod )
#endif
    {
	if(nMod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new DAQTmpl::TTpContr(source);
	return NULL;
    }
}

using namespace DAQTmpl;

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

    //DB structure of the controller object
    fldAdd(new TFld("PRM_BD",trS("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",trS("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",trS("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    //???? Append here the PLC specific configuration fields stored in DB

    //Parameter types and them DB structures
    int tPrm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(tPrm).fldAdd(new TFld("OID_LS",trS("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100",""));
    //???? Append here the parameter type specific configuration fields stored in DB

    //???? Append here other parameter types at your need
}

TController *TTpContr::ContrAttach( const string &id, const string &daq_db )	{ return new TMdContr(id, daq_db, this); }

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
    TController(name_c,daq_db,cfgelem), mPrior(cfg("PRIOR").getId()), mPer(1e9), prcSt(false), callSt(false), tmGath(0)
{
    cfg("PRM_BD").setS("TmplPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();

    //???? Append here the status of the controller object for its specific information
    if(startStat() && !redntUse()) {
	if(!prcSt)	rez += TSYS::strMess(_("Task terminated! "));
	if(callSt)	rez += TSYS::strMess(_("Acquisition. "));
	if(period())	rez += TSYS::strMess(_("Acquisition with the period %s. "), tm2s(1e-9*period()).c_str());
	else rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "), atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time %s."), tm2s(1e-6*tmGath).c_str());
    }
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &id, int type ) { return new TMdPrm(id, &owner().tpPrmAt(type)); }

void TMdContr::start_( )
{
    //Starting the gathering data task
    if(!prcSt) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stopping the gathering data task
    if(prcSt) SYS->taskDestroy(nodePath('.',true));
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned iPrm;

    MtxAlloc res(enRes, true);
    for(iPrm = 0; iPrm < pHd.size(); iPrm++)
	if(&pHd[iPrm].at() == prm) break;

    if(val && iPrm >= pHd.size()) pHd.push_back(prm);
    if(!val && iPrm < pHd.size()) pHd.erase(pHd.begin()+iPrm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.prcSt = true;

    while(!TSYS::taskEndRun()) {
	cntr.callSt = true;
	int64_t tCnt = TSYS::curTime();

	//Updating the controller data
	MtxAlloc res(cntr.enRes, true);
	for(unsigned iP = 0; iP < cntr.pHd.size() && !cntr.redntUse(); iP++)
	    try {
		//???? Place here your data acquisition code or call correspond functions of the parameter
	    } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	res.unlock();
	cntr.tmGath = TSYS::curTime() - tCnt;
	cntr.callSt = false;

	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel().c_str(),"help",TMess::labSecCRON().c_str());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior().c_str());
	return;
    }
    //Processing for commands to the page
    TController::cntrCmdProc(opt);
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE")
	mPer = TSYS::strSepParse(cron(), 1, ' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    return true;
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
    if(!vlElemPresent(&pEl)) vlElemAtt(&pEl);
}

TMdContr &TMdPrm::owner( ) const { return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    owner().prmEn(this, true);

    //???? Append here for the specific initialisation of the parameter type with filling the dynamic attributes in <pEl>
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    owner().prmEn(this, false);

    TParamContr::disable();

    //Set EVAL to the parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);

    //???? Append here for the specific data releasing of the parameter type
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    //???? Correct for specific of your data
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}


//OpenSCADA system module DAQ.System file: os_contr.cpp
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
    for(unsigned i_da = 0; i_da < m_da.size(); i_da++)	delete m_da[i_da];
    m_da.clear();
}

void TTpContr::load_( )
{
    //> Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Init DA sources
    daReg(new CPU());
    daReg(new Mem());
    daReg(new Sensors());
    daReg(new Hddtemp());
    daReg(new UpTime());
    daReg(new HddSmart());
    daReg(new HddStat());
    daReg(new NetStat());
    daReg(new UPS());

    //> Controler's bd structure
    fldAdd(new TFld("AUTO_FILL",_("Auto create active DA"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("PRM_BD",_("System parameters table"),TFld::String,TFld::NoFlag,"30","system"));
    fldAdd(new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","0","0;10000"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Request task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));

    //> Parameter type bd structure
    //>> Make enumerated
    string el_id,el_name,el_def;
    vector<string> list;
    daList(list);
    for(unsigned i_ls = 0; i_ls < list.size(); i_ls++)
    {
	if(i_ls == 0)	el_def = list[i_ls];
	el_id += list[i_ls]+";";
	el_name = el_name+_(daGet(list[i_ls])->name().c_str())+";";
    }
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("TYPE",_("System part"),TFld::String,TFld::Selected|TCfg::NoVal,"10",el_def.c_str(),el_id.c_str(),el_name.c_str()));
    tpPrmAt(t_prm).fldAdd(new TFld("SUBT" ,"",TFld::String,TFld::Selected|TCfg::NoVal|TFld::SelfFld,"255"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

void TTpContr::daList( vector<string> &da )
{
    da.clear();
    for(unsigned i_da = 0; i_da < m_da.size(); i_da++)
	da.push_back(m_da[i_da]->id());
}

void TTpContr::daReg( DA *da )	{ m_da.push_back(da); }

DA *TTpContr::daGet( const string &da )
{
    for(unsigned i_da = 0; i_da < m_da.size(); i_da++)
	if(m_da[i_da]->id() == da) return m_da[i_da];

    return NULL;
}

void TTpContr::perSYSCall( unsigned int cnt )
{
    //> Recheck auto-controllers to create parameters for new devices
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
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse())
    {
	if(call_st)	rez += TSYS::strMess(_("Call now. "));
	if(period())	rez += TSYS::strMess(_("Call by period: %s. "),TSYS::time2str(1e-3*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "),TSYS::time2str(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s. "),TSYS::time2str(tm_calc).c_str());
    }
    return rez;
}

void TMdContr::devUpdate( )
{
    if(cfg("AUTO_FILL").getB())
    {
	vector<string> list;
	mod->daList(list);
	for(unsigned i_l = 0; i_l < list.size(); i_l++)
	    mod->daGet(list[i_l])->makeActiveDA(this);
    }
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::load_( )
{
    if(!SYS->chkSelDB(DB())) return;

    TController::load_();

    //> Check for get old period method value
    if(mPerOld) { cfg("SCHEDULE").setS(TSYS::real2str(mPerOld/1e3)); mPerOld = 0; }
}

void TMdContr::enable_( )
{
    devUpdate();
}

void TMdContr::start_( )
{
    //> Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,1e9*atof(cron().c_str())) : 0;

    //> Start the request data task
    if(!prc_st) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.',true), &endrun_req);

    //> Set Eval for parameters
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

    while(!cntr.endrun_req)
    {
	if(!cntr.redntUse())
	{
	    cntr.call_st = true;
	    //> Update controller's data
	    try
	    {
		int64_t t_cnt = TSYS::curTime();

		cntr.en_res.resRequestR();
		for(unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++)
		    cntr.p_hd[i_p].at().getVal();
		cntr.en_res.resRelease();

		cntr.tm_calc = TSYS::curTime()-t_cnt;
	    }
	    catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
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
    if(opt->name() == "info")
    {
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
    TParamContr(name,tp_prm), daData(NULL), m_auto(false), m_da(NULL)
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
    if(!m_auto)	TParamContr::load_();
}

void TMdPrm::save_( )
{
    if(!m_auto) TParamContr::save_();

    //> Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
	if(!vlAt(a_ls[i_a]).at().arch().freeStat())
	    vlAt(a_ls[i_a]).at().arch().at().save();
}

void TMdPrm::vlGet( TVal &val )
{
    if(val.name() == "err")
    {
	if(!owner().startStat())val.setS(_("2:Controller stopped"), 0, true);
	else if(!enableStat())	val.setS(_("1:Parameter disabled"), 0, true);
	else if(daErr.size())	val.setS(daErr, 0, true);
	else val.setS("0", 0, true);
    }
}

void TMdPrm::getVal( )
{
    if(m_da) m_da->getVal(this);
}

void TMdPrm::setEval( )
{
    if(!m_da)	return;

    vector<string> als;
    m_da->fldList(als);
    for(unsigned i_a = 0; i_a < als.size(); i_a++)
	if(vlPresent(als[i_a]))
	    vlAt(als[i_a]).at().setS(EVAL_STR,0,true);
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
    if(m_da && da_id == m_da->id())	return;

    //> Free previous type
    if(m_da)
    {
	m_da->deInit(this);
	vlElemDet(m_da);
	m_da = NULL;
    }

    //> Create new type
    try
    {
	if(da_id.size() && (m_da=mod->daGet(da_id)))
	{
	    daErr = "";
	    vlElemAtt(m_da);
	    m_da->init(this);
	}
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{
    //> Change TYPE parameter
    if(i_cfg.name() == "TYPE") { setType(i_cfg.getS()); return true; }
    if(m_da) m_da->cfgChange(i_cfg);
    if(!autoC()) modif();
    return true;
}

//OpenSCADA module DAQ.Fastwel file: module.cpp
/***************************************************************************
 *   Copyright (C) 2014-2016 by Maxim Kochetkov                            *
 *   fido_max@inbox.ru                                                     *
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

#include "module.h"

//*************************************************
//* Module info!                                  *
#define MOD_ID		"Fastwel"
#define MOD_NAME	_("Fastwel IO")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.1.22"
#define AUTHORS		_("Maxim Kochetkov")
#define DESCRIPTION	_("Fastwel IO FBUS client implementation")
#define LICENSE		"GPL2"
//*************************************************

ModFastwel::TTpContr * ModFastwel::mod;	//Pointer for direct access to the module

//!!! Required section for binding OpenSCADA core to this module. It provides information and create module root object.
//!!! Do not remove this section!
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_Fastwel_module(int n_mod)
#else
    TModule::SAt module(int n_mod)
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }
#ifdef MOD_INCL
    TModule *daq_Fastwel_attach(const TModule::SAt & AtMod, const string & source)
#else
    TModule *attach(const TModule::SAt & AtMod, const string & source)
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE)) return new ModFastwel::TTpContr(source);
	return NULL;
    }
}

using namespace ModFastwel;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr(string name) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr()
{

}

void TTpContr::load_()
{
    FBUS_Start();
}

void TTpContr::save_()
{

}

void TTpContr::FBUS_Start()
{

    // modif();

    ResAlloc res(FBUSRes, true);
    if(FBUS_initOK) FBUS_finish();
    if(fbusInitialize() != FBUS_RES_OK) {
	throw TError(nodePath().c_str(), _("FBUS init failed."));
    } else {
	FBUS_initOK = true;
	for(int i = 0; i < FBUS_MAX_NET; i++) {
	    hNet[i] = FBUS_INVALID_HANDLE;
	}
	FBUS_fbusGetVersion();
    }
}

void TTpContr::FBUS_finish()
{
    ResAlloc res(FBUSRes, true);
    fbusDeInitialize();
    FBUS_initOK = false;
}

void TTpContr::FBUS_fbusGetVersion()
{
    ResAlloc res(FBUSRes, true);
    fbusGetVersion(&verMajor, &verMinor);
    mModVers=TSYS::strMess("%s FBUS: %d.%d", MOD_VER, verMajor, verMinor);
}

void TTpContr::FBUS_fbusOpen(int n)
{
    if(hNet[n] == FBUS_INVALID_HANDLE) {
	if(fbusOpen(n, &hNet[n]) != FBUS_RES_OK) {
	    hNet[n] = FBUS_INVALID_HANDLE;
	    throw TError(nodePath().c_str(), _("FBUS open net #%d failed."), n);
	}
    }
}

void TTpContr::FBUS_fbusClose(int n)
{
    if(hNet[n] != FBUS_INVALID_HANDLE) {
	if(fbusClose(n) != FBUS_RES_OK) {
	    throw TError(nodePath().c_str(), _("FBUS open net #%d failed."), n);
	} else {
	    hNet[n] = FBUS_INVALID_HANDLE;
	}
    }
}

void TTpContr::FBUS_fbusRescan(int n)
{
    if(fbusRescan(hNet[n], &(modCount[n])) != FBUS_RES_OK) {
	throw TError(nodePath().c_str(), _("FBUS rescan net #%d failed."), n);
    }
}

void TTpContr::FBUS_fbusGetNodeDescription(int n, int id, PFIO_MODULE_DESC modDesc)
{
    if(fbusGetNodeDescription(hNet[n], id, modDesc, sizeof(*modDesc)) != FBUS_RES_OK) {
	throw TError(nodePath().c_str(), _("FBUS GetNodeDescription net #%d, #id%d  failed."), n, id);
    }
}

int TTpContr::FBUS_fbusReadInputs(int n, int id, void *Buf, size_t offset, size_t size)
{
    return fbusReadInputs(hNet[n], id, Buf, offset, size);
}

int TTpContr::FBUS_fbusSetNodeSpecificParameters(int n, int id, void *Buf, size_t offset, size_t size)
{
    return fbusSetNodeSpecificParameters(hNet[n], id, Buf, offset, size);
}

int TTpContr::FBUS_fbusGetNodeSpecificParameters(int n, int id, void *Buf, size_t offset, size_t size)
{
    return fbusGetNodeSpecificParameters(hNet[n], id, Buf, offset, size);
}

int TTpContr::FBUS_fbusGetNodeCommonParameters(int n, int id, PFIO_MODULE_COMMON_CONF Buf, size_t size)
{
    return fbusGetNodeCommonParameters(hNet[n], id, Buf, size);
}

int TTpContr::FBUS_fbusSetNodeCommonParameters(int n, int id, PFIO_MODULE_COMMON_CONF Buf, size_t size)
{
    return fbusSetNodeCommonParameters(hNet[n], id, Buf, size);
}

int TTpContr::FBUS_fbusWriteConfig(int n, int id)
{
    return fbusWriteConfig(hNet[n], id);
}

int TTpContr::FBUS_fbusReadConfig(int n, int id)
{
    return fbusReadConfig(hNet[n], id);
}

int TTpContr::FBUS_fbusSaveConfig(int n, int id)
{
    return fbusSaveConfig(hNet[n], id);
}

int TTpContr::FBUS_fbusWriteOutputs(int n, int id, void *Buf, size_t offset, size_t size)
{
    return fbusWriteOutputs(hNet[n], id, Buf, offset, size);
}

void TTpContr::postEnable(int flag)
{
    TTypeDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD_DIM762", _("DIM762 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_DIM716", _("DIM716 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_DIM718", _("DIM718 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_AIM791", _("AIM791 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_AIM726", _("AIM726 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_AIM730", _("AIM730 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_AIM725", _("AIM725 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_DIM765", _("DIM765 Parameters table"), TFld::String, TFld::NoFlag, "30", ""));

    fldAdd(new TFld("SCHEDULE", _("Acquisition schedule"), TFld::String, TFld::NoFlag, "100", "1"));
    fldAdd(new TFld("PRIOR", _("Priority of the acquisition task"), TFld::Integer, TFld::NoFlag, "2", "0", "-1;199"));
    fldAdd(new TFld("NET_ID", _("Network number"), TFld::Integer, TFld::NoFlag, "0", "0", "0;63"));

    //> Parameter DIM762 bd structure
    int t_prm = tpParmAdd("DIM762", "PRM_BD_DIM762", _("DIM762"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("DI_DEBOUNCE", _("Debounce"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "0;1;2", _("No;200us;3ms")));
    tpPrmAt(t_prm).fldAdd(new TFld("DI_COUNT", _("Enable counting"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    //> Parameter DIM716 bd structure
    t_prm = tpParmAdd("DIM716", "PRM_BD_DIM716", _("DIM716"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("DI_DEBOUNCE", _("Debounce"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "0;1;2", _("No;200us;3ms")));
    tpPrmAt(t_prm).fldAdd(new TFld("DI_COUNT", _("Enable counting"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    //> Parameter DIM718 bd structure
    t_prm = tpParmAdd("DIM718", "PRM_BD_DIM718", _("DIM718"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));

    //> Parameter AIM791 bd structure
    t_prm = tpParmAdd("AIM791", "PRM_BD_AIM791", _("AIM791"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_RANGE", _("Input range"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "0;1;2", _("0..5mA;0..20mA;4..20mA")));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_SCANRATE", _("Scan Rate"), TFld::Integer, TCfg::NoVal, "3", "1", "1;250"));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_FILTER", _("Filter depth"), TFld::Integer, TCfg::NoVal, "3", "0", "0;255"));

    //> Parameter AIM726 bd structure
    t_prm = tpParmAdd("AIM726", "PRM_BD_AIM726", _("AIM726"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_FILTER", _("Filter depth"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "0;1:2", _("50Hz;500Hz;1000Hz")));

    //> Parameter AIM730 bd structure
    t_prm = tpParmAdd("AIM730", "PRM_BD_AIM730", _("AIM730"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("AO_RANGE", _("Output range"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "0;1", _("0..20mA;4..20mA")));

    //> Parameter AIM725 bd structure
    t_prm = tpParmAdd("AIM725", "PRM_BD_AIM725", _("AIM725"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    tpPrmAt(t_prm).fldAdd(
	    new TFld("AI_RANGE", _("Sensor range"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "13;0;1;2;3;4;5;7;14;8;9;10;11;12",
		    _("PT50;PT100;PT200;PT500;PT1000;NI100;NI120;CU100;CU50;0-150 Ohm;0-300 Ohm;0-600 Ohm;0-1500 Ohm; 0-3000 Ohm")));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_TYPE", _("Sensor type"), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0", "0;1", _("2-wire;3-wire")));

    t_prm = tpParmAdd("DIM765_766", "PRM_BD_DIM765", _("DIM765/766"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;63"));
    for(int i = 0; i < 8; i++) {
	tpPrmAt(t_prm).fldAdd(
		new TFld(TSYS::strMess("DI_TYPE_%d", i).c_str(), TSYS::strMess(_("Type %d"), i).c_str(), TFld::Integer, TFld::Selectable | TCfg::NoVal, "1", "0",
			"0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;255",
			_("No break detection;1K8;2K2;2K7;3K3;3K9;4K7;5K6;6K8;8K2;10K;12K;15K;18K;20K;22K;24K;27K;30K;33K;Type 2")));
	tpPrmAt(t_prm).fldAdd(
		new TFld(TSYS::strMess("DI_DEBOUNCE_ON_%d", i).c_str(), TSYS::strMess(_("Debounce on %d"), i).c_str(), TFld::Integer,
			TFld::Selectable | TCfg::NoVal, "1", "0", "0;1;2;3;4;5;6;7;8;9", _("OFF;1.4ms;2.8ms;4.2ms;9.8ms;2"
				"0ms;50ms;100ms;200ms;300ms")));
	tpPrmAt(t_prm).fldAdd(
		new TFld(TSYS::strMess("DI_DEBOUNCE_OFF_%d", i).c_str(), TSYS::strMess(_("Debounce off %d"), i).c_str(), TFld::Integer,
			TFld::Selectable | TCfg::NoVal, "1", "0", "0;1;2;3;4;5;6;7;8;9", _("OFF;1.4ms;2.8ms;4.2ms;9.8ms;20ms;50ms;100ms;200ms;300ms")));

    }
}

TController *TTpContr::ContrAttach(const string & name, const string & daq_db)
{
    return new TMdContr(name, daq_db, this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr(string name_c, const string & daq_db, ::TElem * cfgelem) :
	::TController(name_c, daq_db, cfgelem), prcSt(false), callSt(false), endrunReq(false), tmGath(0), enRes(true), dataRes(true),
	mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mNet(cfg("NET_ID")), mPer(1e9)
{
    cfg("PRM_BD_DIM762").setS("FBUSPrmDIM762_" + name_c);
    cfg("PRM_BD_DIM716").setS("FBUSPrmDIM716_" + name_c);
    cfg("PRM_BD_DIM718").setS("FBUSPrmDIM718_" + name_c);
    cfg("PRM_BD_AIM791").setS("FBUSPrmAIM791_" + name_c);
    cfg("PRM_BD_AIM726").setS("FBUSPrmAIM726_" + name_c);
    cfg("PRM_BD_AIM730").setS("FBUSPrmAIM730_" + name_c);
    cfg("PRM_BD_AIM725").setS("FBUSPrmAIM725_" + name_c);
    cfg("PRM_BD_DIM765").setS("FBUSPrmDIM765_" + name_c);
}

TMdContr::~TMdContr()
{
    if(startStat()) stop();
}

void TMdContr::GetNodeDescription(int id, PFIO_MODULE_DESC modDesc)
{
    mod->FBUS_fbusGetNodeDescription(mNet, id, modDesc);
}

int TMdContr::ReadInputs(int id, void *buf, size_t offset, size_t size)
{
    return mod->FBUS_fbusReadInputs(mNet, id, buf, offset, size);
}

int TMdContr::WriteOutputs(int id, void *buf, size_t offset, size_t size)
{
    return mod->FBUS_fbusWriteOutputs(mNet, id, buf, offset, size);
}

int TMdContr::SetNodeSpecificParameters(int id, void *buf, size_t offset, size_t size)
{
    return mod->FBUS_fbusSetNodeSpecificParameters(mNet, id, buf, offset, size);
}

int TMdContr::GetNodeSpecificParameters(int id, void *buf, size_t offset, size_t size)
{
    return mod->FBUS_fbusGetNodeSpecificParameters(mNet, id, buf, offset, size);
}

int TMdContr::GetNodeCommonParameters(int id, PFIO_MODULE_COMMON_CONF buf, size_t size)
{
    return mod->FBUS_fbusGetNodeCommonParameters(mNet, id, buf, size);
}

int TMdContr::SetNodeCommonParameters(int id, PFIO_MODULE_COMMON_CONF buf, size_t size)
{
    return mod->FBUS_fbusSetNodeCommonParameters(mNet, id, buf, size);
}

int TMdContr::WriteConfig(int id)
{
    return mod->FBUS_fbusWriteConfig(mNet, id);
}

int TMdContr::ReadConfig(int id)
{
    return mod->FBUS_fbusReadConfig(mNet, id);
}

int TMdContr::SaveConfig(int id)
{
    return mod->FBUS_fbusSaveConfig(mNet, id);
}

string TMdContr::getStatus()
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	if(callSt) rez += TSYS::strMess(_("Acquisition. "));
	if(period())
	    rez += TSYS::strMess(_("Acquisition with the period %s. "), tm2s(1e-9*period()).c_str());
	else
	    rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "), atm2s(TSYS::cron(cron()), "%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time %s."), tm2s(1e-6*tmGath).c_str());
    }
    return rez;
}

TParamContr *TMdContr::ParamAttach(const string & name, int type)
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::enable_()
{
    mod->FBUS_fbusOpen(mNet);
    mod->FBUS_fbusRescan(mNet);
}

void TMdContr::start_()
{
    //Start the gathering data task
    SYS->taskCreate(nodePath('.', true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_()
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.', true), &endrunReq);
    mod->FBUS_fbusClose(mNet);
}

void TMdContr::prmEn(const string & id, bool val)
{
    int i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size()) p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size()) p_hd.erase(p_hd.begin() + i_prm);
}

void *TMdContr::Task(void *icntr)
{
    TMdContr & cntr = *(TMdContr *) icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    while(!cntr.endrunReq) {
	int64_t t_cnt = TSYS::curTime();

	//> Update controller's data
	//!!! Your code for gather data
	MtxAlloc prmRes(cntr.enRes, true);
	cntr.callSt = true;
	for(unsigned i_p = 0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++)
	    try {
		cntr.p_hd[i_p].at().getVals();
	    } catch (TError err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    }
	cntr.callSt = false;
	prmRes.unlock();
	cntr.tmGath = TSYS::curTime() - t_cnt;

	//!!! Wait for next iteration
	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc(XMLNode * opt)
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld", opt, -1, "/cntr/cfg/SCHEDULE", EVAL_STR, startStat() ? R_R_R_ : RWRWR_, "root", SDAQ_ID, 3, "dest", "sel_ed",
		"sel_list",TMess::labSecCRONsel().c_str(), "help",TMess::labSecCRON().c_str());
	ctrMkNode("fld", opt, -1, "/cntr/cfg/PRIOR", EVAL_STR, startStat() ? R_R_R_ : RWRWR_, "root", SDAQ_ID, 1,
		"help",TMess::labTaskPrior().c_str());
	return;
    }
    //Process command to page
    TController::cntrCmdProc(opt);
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE")
	mPer = TSYS::strSepParse(cron(), 1, ' ').empty() ? vmax(0, (int64_t ) (1e9 * s2r(cron()))) : 0;

    return true;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm(string name, TTypeParam * tp_prm) :
	TParamContr(name, tp_prm), p_el("w_attr"), mID(cfg("DEV_ID")), mState(StateUndef)
{

}

TMdPrm::~TMdPrm()
{
//!!! Call for prevent access to data the object from included nodes on destruction.
    nodeDelAll();
}

void TMdPrm::postEnable(int flag)
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el)) vlElemAtt(&p_el);
}

TMdContr & TMdPrm::owner( ) const
{
    return (TMdContr &) TParamContr::owner();
}

bool TMdPrm::InitDI(DIM_CONFIGURATION* pConfig)
{
    bool fConfig;
    for(unsigned i_p = 0; i_p < nDI; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("DI%d", i_p).c_str(), TSYS::strMess("DI%d", i_p).c_str(), TFld::Boolean, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));
    }
    p_el.fldAdd(new TFld("C0", "C0", TFld::Integer, TFld::NoWrite | TVal::DirRead, "", "", "", "", ""));
    p_el.fldAdd(new TFld("C1", "C1", TFld::Integer, TFld::NoWrite | TVal::DirRead, "", "", "", "", ""));
    if(pConfig->debounce != cfg("DI_DEBOUNCE").getI()) {
	fConfig = true;
	pConfig->debounce = cfg("DI_DEBOUNCE").getI();
    }
    if(pConfig->enableCounting != cfg("DI_COUNT").getI()) {
	fConfig = true;
	pConfig->enableCounting = cfg("DI_COUNT").getI();
    }
    return fConfig;
}
bool TMdPrm::InitDIM718(DIM718_CONFIGURATION* pConfig)
{
    bool fConfig = false;
    mDOState = 0;
    for(unsigned i_p = 0; i_p < nDO; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("DO%d", i_p).c_str(), TSYS::strMess("DO%d", i_p).c_str(), TFld::Boolean, TVal::DirRead | TVal::DirWrite, "", "", "", "",
			""));
    }
    return fConfig;
}

bool TMdPrm::InitAIM726(AIM726_CONFIGURATION* pConfig)
{
    bool fConfig = false;
    kAI = 40.0 / 8388607;
    for(unsigned i_p = 0; i_p < nAI; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("AI%d", i_p).c_str(), TSYS::strMess("AI%d", i_p).c_str(), TFld::Double, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));
    }
    if(pConfig->filterOptions0 != cfg("AI_FILTER").getI()) {
	fConfig = true;
	pConfig->filterOptions0 = cfg("AI_FILTER").getI();
    }
    if(pConfig->filterOptions1 != cfg("AI_FILTER").getI()) {
	fConfig = true;
	pConfig->filterOptions1 = cfg("AI_FILTER").getI();
    }
    return fConfig;
}

bool TMdPrm::InitAIM730(AIM730_CONFIGURATION* pConfig)
{
    bool fConfig = false;
    for(unsigned i_p = 0; i_p < nAO; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("AO%d", i_p).c_str(), TSYS::strMess("AO%d", i_p).c_str(), TFld::Double, TVal::DirRead | TVal::DirWrite, "", "", "", "",
			""));
    }
    if(pConfig->outputRange0 != cfg("AO_RANGE").getI()) {
	fConfig = true;
	pConfig->outputRange0 = cfg("AO_RANGE").getI();
    }
    if(pConfig->outputRange1 != cfg("AO_RANGE").getI()) {
	fConfig = true;
	pConfig->outputRange1 = cfg("AO_RANGE").getI();
    }
    switch(cfg("AO_RANGE").getI()) {
    case 0:
	kAO = 20.0 / 65535;
	dAO = 0;
	break;
    case 1:
	kAO = 16.0 / 65535;
	dAO = 4;
	break;
    }
    return fConfig;
}

bool TMdPrm::InitAIM791(AIM791_CONFIGURATION* pConfig)
{
    bool fConfig = false;
    for(unsigned i_p = 0; i_p < nAI; i_p++) {
	if(pConfig->channelRanges[i_p] != cfg("AI_RANGE").getI()) {
	    fConfig = true;
	    pConfig->channelRanges[i_p] = cfg("AI_RANGE").getI();
	}
	if(pConfig->lowLimit[i_p] != 0) {
	    fConfig = true;
	    pConfig->lowLimit[i_p] = 0;
	}
	if(pConfig->highLimit[i_p] != 0xFFFF) {
	    fConfig = true;
	    pConfig->highLimit[i_p] = 0xFFFF;
	}
    }

    switch(cfg("AI_RANGE").getI()) {
    case 0:
	kAI = 5.125 / 65535;
	break;
    case 1:
    case 2:
	kAI = 20.5 / 65535;
	break;
    }
    for(unsigned i_p = 0; i_p < nAI; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("AI%d", i_p).c_str(), TSYS::strMess("AI%d", i_p).c_str(), TFld::Double, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));
    }
    return fConfig;
}

bool TMdPrm::InitAIM725(AIM725_CONFIGURATION* pConfig)
{
    bool fConfig = false;
    if(pConfig->scanRate != 0) {
	fConfig = true;
	pConfig->scanRate = 0;
    }
    if(pConfig->channels_selection != AIM724_5_USE_BOTH_CHANNELS) {
	fConfig = true;
	pConfig->channels_selection = AIM724_5_USE_BOTH_CHANNELS;
    }
    if(pConfig->inputRange != cfg("AI_RANGE").getI()) {
	fConfig = true;
	pConfig->inputRange = cfg("AI_RANGE").getI();
    }
    if(pConfig->connectionType != cfg("AI_TYPE").getI()) {
	fConfig = true;
	pConfig->connectionType = cfg("AI_TYPE").getI();
    }
    kAI = 1;
    for(unsigned i_p = 0; i_p < nAI; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("AI%d", i_p).c_str(), TSYS::strMess("AI%d", i_p).c_str(), TFld::Double, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));
    }
    return fConfig;
}

bool TMdPrm::InitDIM7656(DIM7656_CONFIGURATION* pConfig, FIO_MODULE_TYPE type)
{
    bool fConfig = false;
    if(pConfig->shunt0 != cfg("DI_TYPE_0").getI()) {
	fConfig = true;
	pConfig->shunt0 = cfg("DI_TYPE_0").getI();
    }
    if(pConfig->shunt1 != cfg("DI_TYPE_1").getI()) {
	fConfig = true;
	pConfig->shunt1 = cfg("DI_TYPE_1").getI();
    }
    if(pConfig->shunt2 != cfg("DI_TYPE_2").getI()) {
	fConfig = true;
	pConfig->shunt2 = cfg("DI_TYPE_2").getI();
    }
    if(pConfig->shunt3 != cfg("DI_TYPE_3").getI()) {
	fConfig = true;
	pConfig->shunt3 = cfg("DI_TYPE_3").getI();
    }
    if(pConfig->shunt4 != cfg("DI_TYPE_4").getI()) {
	fConfig = true;
	pConfig->shunt4 = cfg("DI_TYPE_4").getI();
    }
    if(pConfig->shunt5 != cfg("DI_TYPE_5").getI()) {
	fConfig = true;
	pConfig->shunt5 = cfg("DI_TYPE_5").getI();
    }
    if(pConfig->shunt6 != cfg("DI_TYPE_6").getI()) {
	fConfig = true;
	pConfig->shunt6 = cfg("DI_TYPE_6").getI();
    }
    if(pConfig->shunt7 != cfg("DI_TYPE_7").getI()) {
	fConfig = true;
	pConfig->shunt7 = cfg("DI_TYPE_7").getI();
    }

    if(pConfig->switchOnDelay0 != cfg("DI_DEBOUNCE_ON_0").getI()) {
	fConfig = true;
	pConfig->switchOnDelay0 = cfg("DI_DEBOUNCE_ON_0").getI();
    }
    if(pConfig->switchOnDelay1 != cfg("DI_DEBOUNCE_ON_1").getI()) {
	fConfig = true;
	pConfig->switchOnDelay1 = cfg("DI_DEBOUNCE_ON_1").getI();
    }
    if(pConfig->switchOnDelay2 != cfg("DI_DEBOUNCE_ON_2").getI()) {
	fConfig = true;
	pConfig->switchOnDelay2 = cfg("DI_DEBOUNCE_ON_2").getI();
    }
    if(pConfig->switchOnDelay3 != cfg("DI_DEBOUNCE_ON_3").getI()) {
	fConfig = true;
	pConfig->switchOnDelay3 = cfg("DI_DEBOUNCE_ON_3").getI();
    }
    if(pConfig->switchOnDelay4 != cfg("DI_DEBOUNCE_ON_4").getI()) {
	fConfig = true;
	pConfig->switchOnDelay4 = cfg("DI_DEBOUNCE_ON_4").getI();
    }
    if(pConfig->switchOnDelay5 != cfg("DI_DEBOUNCE_ON_5").getI()) {
	fConfig = true;
	pConfig->switchOnDelay5 = cfg("DI_DEBOUNCE_ON_5").getI();
    }
    if(pConfig->switchOnDelay6 != cfg("DI_DEBOUNCE_ON_6").getI()) {
	fConfig = true;
	pConfig->switchOnDelay6 = cfg("DI_DEBOUNCE_ON_6").getI();
    }
    if(pConfig->switchOnDelay7 != cfg("DI_DEBOUNCE_ON_7").getI()) {
	fConfig = true;
	pConfig->switchOnDelay7 = cfg("DI_DEBOUNCE_ON_7").getI();
    }

    if(pConfig->switchOffDelay0 != cfg("DI_DEBOUNCE_OFF_0").getI()) {
	fConfig = true;
	pConfig->switchOffDelay0 = cfg("DI_DEBOUNCE_OFF_0").getI();
    }
    if(pConfig->switchOffDelay1 != cfg("DI_DEBOUNCE_OFF_1").getI()) {
	fConfig = true;
	pConfig->switchOffDelay1 = cfg("DI_DEBOUNCE_OFF_1").getI();
    }
    if(pConfig->switchOffDelay2 != cfg("DI_DEBOUNCE_OFF_2").getI()) {
	fConfig = true;
	pConfig->switchOffDelay2 = cfg("DI_DEBOUNCE_OFF_2").getI();
    }
    if(pConfig->switchOffDelay3 != cfg("DI_DEBOUNCE_OFF_3").getI()) {
	fConfig = true;
	pConfig->switchOffDelay3 = cfg("DI_DEBOUNCE_OFF_3").getI();
    }
    if(pConfig->switchOffDelay4 != cfg("DI_DEBOUNCE_OFF_4").getI()) {
	fConfig = true;
	pConfig->switchOffDelay4 = cfg("DI_DEBOUNCE_OFF_4").getI();
    }
    if(pConfig->switchOffDelay5 != cfg("DI_DEBOUNCE_OFF_5").getI()) {
	fConfig = true;
	pConfig->switchOffDelay5 = cfg("DI_DEBOUNCE_OFF_5").getI();
    }
    if(pConfig->switchOffDelay6 != cfg("DI_DEBOUNCE_OFF_6").getI()) {
	fConfig = true;
	pConfig->switchOffDelay6 = cfg("DI_DEBOUNCE_OFF_6").getI();
    }
    if(pConfig->switchOffDelay7 != cfg("DI_DEBOUNCE_OFF_7").getI()) {
	fConfig = true;
	pConfig->switchOffDelay7 = cfg("DI_DEBOUNCE_OFF_7").getI();
    }

    kAI = 7.4655;
    if(type == FIO_MODULE_DIM765) {
	kAIC = 4.0161;
    } else {
	kAIC = 3.7422;
    }

    for(unsigned i_p = 0; i_p < nDI; i_p++) {
	p_el.fldAdd(
		new TFld(TSYS::strMess("DI%d", i_p).c_str(), TSYS::strMess("DI%d", i_p).c_str(), TFld::Boolean, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));
	p_el.fldAdd(
		new TFld(TSYS::strMess("BREAK%d", i_p).c_str(), TSYS::strMess(_("Break %d"), i_p).c_str(), TFld::Boolean, TFld::NoWrite | TVal::DirRead, "", "",
			"", "", ""));
	p_el.fldAdd(
		new TFld(TSYS::strMess("V%d", i_p).c_str(), TSYS::strMess("V%d", i_p).c_str(), TFld::Double, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));
	p_el.fldAdd(
		new TFld(TSYS::strMess("I%d", i_p).c_str(), TSYS::strMess("I%d", i_p).c_str(), TFld::Double, TFld::NoWrite | TVal::DirRead, "", "", "", "",
			""));

    }
    return fConfig;
}

void TMdPrm::enable()
{
    if(enableStat()) return;

    TParamContr::enable();
//Delete DAQ parameter's attributes
    for(unsigned i_f = 0; i_f < p_el.fldSize();) {
	try {
	    p_el.fldDel(i_f);
	    continue;
	} catch (TError err) {
	    mess_warning(err.cat.c_str(), err.mess.c_str());
	}
	i_f++;
    }
    bool fConfig = false;
    owner().prmEn(id(), true);
    try {
	owner().GetNodeDescription(mID, &mModDesc);
	owner().ReadConfig(mID);
	owner().GetNodeSpecificParameters(mID, mModConfig, 0, mModDesc.specificRwSize);
	owner().GetNodeCommonParameters(mID, &mModComConfig, sizeof(mModComConfig));
	if(mModComConfig.inputSync != FBUS_UNDEFINED_SYNC_ID) {
	    fConfig = true;
	    mModComConfig.inputSync = FBUS_UNDEFINED_SYNC_ID;
	}
	if(mModComConfig.outputSync != FBUS_UNDEFINED_SYNC_ID) {
	    fConfig = true;
	    mModComConfig.outputSync = FBUS_UNDEFINED_SYNC_ID;
	}
	mTypeName = mModDesc.typeName;
	mState = StateWork;
	switch(mModDesc.type) {
	case FIO_MODULE_DIM762:
	    nDI = 8;
	    fConfig |= InitDI((DIM_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_DIM716:
	    nDI = 2;
	    fConfig |= InitDI((DIM_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_DIM718:
	    nDO = 8;
	    fConfig |= InitDIM718((DIM718_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_AIM726:
	    nAI = 2;
	    fConfig |= InitAIM726((AIM726_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_AIM730:
	    nAO = 2;
	    fConfig |= InitAIM730((AIM730_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_AIM791:
	    nAI = 8;
	    fConfig |= InitAIM791((AIM791_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_AIM725:
	    nAI = 2;
	    fConfig |= InitAIM725((AIM725_CONFIGURATION *) mModConfig);
	    break;
	case FIO_MODULE_DIM765:
	case FIO_MODULE_DIM766:
	    nDI = 8;
	    nAI = 8;
	    fConfig |= InitDIM7656((DIM7656_CONFIGURATION *) mModConfig, mModDesc.type);
	    break;

	default:
	    mState = StateWrongType;
	    break;
	}
	if(fConfig) {
	    owner().SetNodeSpecificParameters(mID, mModConfig, 0, mModDesc.specificRwSize);
	    owner().SetNodeCommonParameters(mID, &mModComConfig, sizeof(mModComConfig));
	    owner().WriteConfig(mID);
	    owner().SaveConfig(mID);
	}
    } catch (TError err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	disable();
    }

}

void TMdPrm::disable()
{
    if(!enableStat()) return;

    owner().prmEn(id(), false);

    TParamContr::disable();

//> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);
}

void TMdPrm::vlGet(TVal & val)
{
    if(!enableStat() || !owner().startStat()) {
	if(val.name() == "err") {
	    if(!enableStat())
		val.setS(_("1:Parameter disabled."), 0, true);
	    else if(!owner().startStat()) val.setS(_("2:Acquisition stopped."), 0, true);
	} else
	    val.setS(EVAL_STR, 0, true);
	return;
    }

    if(owner().redntUse()) return;

    if(val.name() == "err") {
	switch(mState) {
	case StateWork:
	    val.setS(_("0: Normal"), 0, true);
	    break;
	case StateWrongType:
	    val.setS(TSYS::strMess(_("3: Wrong type: %s"), mModDesc.typeName), 0, true);
	    break;
	}

    }
}

void TMdPrm::getVals()
{
    uint8_t buf[256];
    if(mState == StateWork) {
	if(owner().ReadInputs(mID, buf, 0, mModDesc.inputsSize) == FBUS_RES_OK) {
	    switch(mModDesc.type) {
	    case FIO_MODULE_DIM762:
	    case FIO_MODULE_DIM716:
		for(unsigned i_p = 0; i_p < nDI; i_p++) {
		    vlAt(TSYS::strMess("DI%d", i_p).c_str()).at().setB(((((DIM_INPUTS_COUNTERS *) buf)->inputStates) >> i_p) & 1, 0, true);
		}
		vlAt("C0").at().setI((((DIM_INPUTS_COUNTERS *) buf)->counter0), 0, true);
		vlAt("C1").at().setI((((DIM_INPUTS_COUNTERS *) buf)->counter1), 0, true);
		break;
	    case FIO_MODULE_AIM726:
		vlAt(TSYS::strMess("AI0").c_str()).at().setR(((AIM726_INPUTS *) buf)->input0 * kAI, 0, true);
		vlAt(TSYS::strMess("AI1").c_str()).at().setR(((AIM726_INPUTS *) buf)->input1 * kAI, 0, true);

		break;
	    case FIO_MODULE_AIM730:
		vlAt(TSYS::strMess("AO0").c_str()).at().setR(((AIM730_INPUTS *) buf)->outputValue0 * kAO + dAO, 0, true);
		vlAt(TSYS::strMess("AO1").c_str()).at().setR(((AIM730_INPUTS *) buf)->outputValue1 * kAO + dAO, 0, true);
		break;
	    case FIO_MODULE_AIM791:
		for(unsigned i_p = 0; i_p < nAI; i_p++) {
		    vlAt(TSYS::strMess("AI%d", i_p).c_str()).at().setR(((AIM791_INPUTS *) buf)->values[i_p] * kAI, 0, true);
		}
		break;
	    case FIO_MODULE_AIM725:
		if(((((AIM725_INPUTS *) buf)->diagnostics) & 0x0D) == 0) {
		    vlAt(TSYS::strMess("AI0").c_str()).at().setR(((AIM725_INPUTS *) buf)->channel0, 0, true);
		} else {
		    vlAt(TSYS::strMess("AI0").c_str()).at().setR(EVAL_REAL, 0, true);
		}
		if(((((AIM725_INPUTS *) buf)->diagnostics) & 0x0E) == 0) {
		    vlAt(TSYS::strMess("AI1").c_str()).at().setR(((AIM725_INPUTS *) buf)->channel1, 0, true);
		} else {
		    vlAt(TSYS::strMess("AI1").c_str()).at().setR(EVAL_REAL, 0, true);
		}
		break;
	    case FIO_MODULE_DIM765:
	    case FIO_MODULE_DIM766:
		for(unsigned i_p = 0; i_p < nDI; i_p++) {
		    vlAt(TSYS::strMess("DI%d", i_p).c_str()).at().setB(((((DIM7656_INPUTS *) buf)->channelsState) >> i_p) & 1, 0, true);
		    vlAt(TSYS::strMess("BREAK%d", i_p).c_str()).at().setB(((((DIM7656_INPUTS *) buf)->wireBreaks) >> i_p) & 1, 0, true);
		}
		vlAt(TSYS::strMess("V0").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v0 * kAI, 0, true);
		vlAt(TSYS::strMess("V1").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v1 * kAI, 0, true);
		vlAt(TSYS::strMess("V2").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v2 * kAI, 0, true);
		vlAt(TSYS::strMess("V3").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v3 * kAI, 0, true);
		vlAt(TSYS::strMess("V4").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v4 * kAI, 0, true);
		vlAt(TSYS::strMess("V5").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v5 * kAI, 0, true);
		vlAt(TSYS::strMess("V6").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v6 * kAI, 0, true);
		vlAt(TSYS::strMess("V7").c_str()).at().setR(((DIM7656_INPUTS *) buf)->v7 * kAI, 0, true);

		vlAt(TSYS::strMess("I0").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i0 * kAIC, 0, true);
		vlAt(TSYS::strMess("I1").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i1 * kAIC, 0, true);
		vlAt(TSYS::strMess("I2").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i2 * kAIC, 0, true);
		vlAt(TSYS::strMess("I3").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i3 * kAIC, 0, true);
		vlAt(TSYS::strMess("I4").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i4 * kAIC, 0, true);
		vlAt(TSYS::strMess("I5").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i5 * kAIC, 0, true);
		vlAt(TSYS::strMess("I6").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i6 * kAIC, 0, true);
		vlAt(TSYS::strMess("I7").c_str()).at().setR(((DIM7656_INPUTS *) buf)->i7 * kAIC, 0, true);
		break;

	    default:
		break;
	    }
	}
    }
}

void TMdPrm::vlSet(TVal & vo, const TVariant & vl, const TVariant & pvl)
{
    MtxAlloc prmRes(owner().enRes, true);
    uint8_t buf[256];
    uint16_t i;
    if(!enableStat() || !owner().startStat()) {
	vo.setS(EVAL_STR, 0, true);
	return;
    }

    if(vl.isEVal() || vl == pvl) return;

    //Send to active reserve station
    if(vlSetRednt(vo,vl,pvl))	return;

    switch(mModDesc.type) {
    case FIO_MODULE_DIM718:
	if(vo.name().compare(0, 2, "DO") == 0) {
	    if(vl.getB()) {
		mDOState |= 1 << s2i(vo.name().substr(2, vo.name().size() - 2));
	    } else {
		mDOState &= ~(1 << s2i(vo.name().substr(2, vo.name().size() - 2)));
	    }
	    owner().WriteOutputs(mID, &mDOState, 0, 1);
	}

	break;
    case FIO_MODULE_AIM730:
	if(vo.name().compare(0, 2, "AO") == 0) {
	    for(int i = 0; i < nAO; i++) {
		switch(i) {
		case 0:
		    if(s2i(vo.name().substr(2, vo.name().size() - 2)) == 0) {
			((AIM730_OUTPUTS *) buf)->output0 = (vl.getR() - dAO) / kAO;
		    } else {
			((AIM730_OUTPUTS *) buf)->output0 = (vlAt(TSYS::strMess("AO0").c_str()).at().getR() - dAO) / kAO;
		    }
		    break;
		case 1:
		    if(s2i(vo.name().substr(2, vo.name().size() - 2)) == 1) {
			((AIM730_OUTPUTS *) buf)->output1 = (vl.getR() - dAO) / kAO;
		    } else {
			((AIM730_OUTPUTS *) buf)->output1 = (vlAt(TSYS::strMess("AO1").c_str()).at().getR() - dAO) / kAO;
		    }
		    break;
		}
	    }
	    owner().WriteOutputs(mID, buf, 0, nAO * 2);
	}
	break;
    default:
	break;

    }
}

void TMdPrm::cntrCmdProc(XMLNode * opt)
{
//> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0, 6) == "/serv/") {
	TParamContr::cntrCmdProc(opt);
	return;
    }

//> Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	//ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }

//> Process command to page
    else
	TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake(TVal & val)
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::DAQAttr);
    val.arch().at().setPeriod((int64_t) (owner().period() * 1000000));
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

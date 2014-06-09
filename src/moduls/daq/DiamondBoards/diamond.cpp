
//OpenSCADA system module DAQ.DiamondBoards file: diamond.cpp
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

#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <tsys.h>
#include <ttiparam.h>

#include "diamond.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DiamondBoards"
#define MOD_NAME	_("Diamond DAQ boards")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"2.0.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow access to 'Diamond Systems' DAQ boards. Include main support for generic boards.")
#define LICENSE		"GPL2"
//*************************************************

Diamond::TTpContr *Diamond::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new Diamond::TTpContr(source);
	return NULL;
    }
}

using namespace Diamond;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID), mInit(false)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //> Known devices typical configuration
    devs[DSC_DMM16]	= DevFeature("DMM-16", 0x10, 4, 0, 1, 1);
    devs[DSC_DMM16].setAITypes(string("0x00;0x01;0x02;0x20;0x21;0x22;0x23;0x30;0x31;0x32\n")+
        _("[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 10]V;[0, 5]V;[0, 2.5]V"));
    devs[DSC_RMM]	= DevFeature("Ruby-MM", 0, 8, 3);
    //devs[DSC_TMM] ??
    devs[DSC_OPMM]	= DevFeature("Opal-MM", 0, 0, 0, 1, 1);
    devs[DSC_DMM]	= DevFeature("DMM", 0x0C0010, 2, 0, 1, 1);
    devs[DSC_DMM].setAITypes(string("0x00;0x01;0x02;0x03;0x04;0x05;0x10;0x11;0x12;0x13;0x14\n")+
	_("[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1, +1]V;[-0.5, +0.5]V;+-Custom;[0, 10]V;[0, 5]V;[0, 2]V;[0, 1]V;+Custom"));
    //devs[DSC_SMM] ??
    //devs[DSC_DMM] Replaced by DSC_GPIO11_DIO and DSC_GPIO21 for DIO
    //devs[DSC_QMM] ??
    //devs[DSC_ZMM] ??
    devs[DSC_PMM]	= DevFeature("Pearl-MM", 0, 0, 0, 0, 2);
    devs[DSC_OMM]	= DevFeature("Onyx-MM", 0, 0, 6);
    devs[DSC_RMM416]	= DevFeature("Ruby-MM-416", 0, 0x1004, 3);
    devs[DSC_DMM32]	= DevFeature("DMM-32(-AT)", 0xC820, 4, 3);
    devs[DSC_DMM32].aiSzFIFO = 512;
    devs[DSC_DMM32].setAITypes(string("0x00;0x01;0x02;0x03;0x20;0x21;0x22;0x23;0x30;0x31;0x32;0x33\n")+
        _("[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-0.625, +0.625]V;[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;"
          "[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V"));
    //devs[DSC_EMMDIO] ??
    devs[DSC_RMM1612]	= DevFeature("Ruby-MM-1612", 0, 16, 3);
    devs[DSC_DMMAT]	= DevFeature("DMM-AT", 0x0C6410, 2, 0, 1, 1);
    devs[DSC_DMMAT].aiSzFIFO = 512;
    devs[DSC_DMMAT].setAITypes(string("0x00;0x01;0x02;0x03;0x10;0x11;0x12;0x13;0x20;0x21;0x22;0x23\n")+
	_("[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-0.625, +0.625]V;"
	  "[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V;[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V"));
    devs[DSC_DMM16AT]	= DevFeature("DMM-16-AT", 0x6410, 4, 0, 1, 1);
    devs[DSC_DMM16AT].aiSzFIFO = 512;
    devs[DSC_DMM16AT].setAITypes(string("0x00;0x01;0x02;0x03;0x10;0x11;0x12;0x13;0x20;0x21;0x22;0x23\n")+
	_("[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-0.625, +0.625]V;"
	  "[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V;[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V"));
    devs[DSC_IR104]	= DevFeature("IR104", 0, 0, 0, 3, 3);	// Not full ports 20 DI, 20 DO. Specific processing
    //devs[DSC_EMM8] ??
    devs[DSC_PROM]	= DevFeature("Prometheus", 0x6410, 4, 3);
    devs[DSC_PROM].setAITypes(string("0x00;0x01;0x02;0x03;0x20;0x21;0x22;0x23;0x30;0x31;0x32\n")+
        _("[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 8.3]V;[0, 5]V;[0, 2.5]V"));
    devs[DSC_HERCEBX]	= DevFeature("Hercules EBX", 0xFA20, 4, 5);
    devs[DSC_HERCEBX].aiSzFIFO = 2048;
    devs[DSC_HERCEBX].setAITypes(string("0x00;0x01;0x02;0x03;0x10;0x11;0x12;0x13\n")+
        _("[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V"));
    //devs[DSC_CPT] ??
    //devs[DSC_DMM48] ??
    devs[DSC_OMMDIO]	= DevFeature("Onyx-MM-DIO", 0, 0, 6);
    devs[DSC_MRC]	= DevFeature("Mercator", 0, 0, 3);				//DSC_GPIO no configure direction need
    devs[DSC_ATHENA]	= DevFeature("Athena", 0x6410, 4, 3);
    devs[DSC_ATHENA].setAITypes(devs[DSC_PROM].aiTypes);
    //devs[DSC_METIS] ??
    devs[DSC_DMM32X]	= DevFeature("DMM-32x(-AT)", 0xFA20, 4, 3);
    devs[DSC_DMM32X].aiSzFIFO = 1024;
    devs[DSC_DMM32X].setAITypes(devs[DSC_DMM32].aiTypes);
    //devs[DSC_ELEKTRA] ??
    devs[DSC_GPIO11_DIO] = DevFeature("GPIO-MM-11[12](DIO)", 0, 0, 6, 0/*1*/, 0/*1*/);	//Unknown DIO, DI and DO ports order!
    devs[DSC_GPIO21]	= DevFeature("GPIO-MM-21", 0, 0, 12);				//DSC_GPIO no configure direction need
    devs[DSC_PSD]	= DevFeature("Poseidon", 0xFA20, 4, 3);
    devs[DSC_PSD].aiSzFIFO = 1024;
    devs[DSC_PSD].setAITypes(string("0x00;0x01;0x02;0x03;0x20;0x21;0x22;0x23;0x30;0x31;0x32;0x33\n")+
        _("[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-0.625, +0.625]V;"
          "[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V"));
    devs[DSC_ATHENAII]	= DevFeature("Athena-II", 0x6410, 4, 3);
    devs[DSC_ATHENAII].aiSzFIFO = 2048;
    devs[DSC_ATHENAII].setAITypes(string("0x00;0x01;0x02;0x03;0x11;0x12;0x13\n")+
        _("[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V"));
    devs[DSC_DMM32DX]	= DevFeature("DMM-32dx(-AT)", 0xFA20, 4, 3);
    devs[DSC_DMM32DX].setAITypes(devs[DSC_DMM32].aiTypes);
    devs[DSC_DMM32DX].aiSzFIFO = 1024;
    devs[DSC_HELIOS]	= DevFeature("Helios", 0x6410, 4, 5);
    devs[DSC_HELIOS].aiSzFIFO = 2048;
    devs[DSC_HELIOS].setAITypes(string("0x00;0x01;0x02;0x03;0x11;0x12;0x13\n")+
        _("[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V"));
    devs[DSC_HELIOS].setAOTypes(string("0x00;0x01;0x40;0x41;0x42\n")+
        _("[0, 5]V;[0, 10]V;[-2.5, +2.5]V;[-5, +5]V;[-10, +10]V"));
    devs[DSC_NEPTUNE]	= DevFeature("Neptune", 0xFA20, 4, 4);
    devs[DSC_NEPTUNE].aiSzFIFO = 1024;
    devs[DSC_NEPTUNE].setAITypes(string("0x00;0x01;0x02;0x03;0x20;0x21;0x22;0x23;0x30;0x31;0x32;0x33\n")+
        _("[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[-0.625, +0.625]V;"
          "[-10, +10]V;[-5, +5]V;[-2.5, +2.5]V;[-1.25, +1.25]V;[0, 10]V;[0, 5]V;[0, 2.5]V;[0, 1.25]V"));
}

TTpContr::~TTpContr()
{
    //> Free DSCAD
    if(drvInitOk())	dscFree();
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Init DSCAD
    if(dscInit(DSC_VERSION) != DE_NONE)	mess_err(mod->nodePath().c_str(), _("dscInit error."));
    else mInit = true;

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));

    //> Parameter type bd structure
    TTipParam &tPrm = tpPrmAt(tpParmAdd("std", "PRM_BD", _("Standard")));
    tPrm.fldAdd(new TFld("TP",_("Board: type"),TFld::Integer,TCfg::NoVal,"3","25"));
    tPrm.fldAdd(new TFld("ADDR",_("Board: address"),TFld::Integer,TCfg::NoVal|TFld::HexDec,"3","640"));
    tPrm.fldAdd(new TFld("INT",_("Board: interrupt"),TFld::Integer,TCfg::NoVal,"2","5"));
    tPrm.fldAdd(new TFld("S_RATE",_("Sample rate (single channel)"),TFld::Integer,TCfg::NoVal,"7","0"));
    tPrm.fldAdd(new TFld("ASYNCH_RD",_("Asynchronous read"),TFld::Boolean,TCfg::NoVal,"1","0"));
    tPrm.fldAdd(new TFld("AI_VAL",_("AI value mode"),TFld::Integer,TCfg::NoVal|TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d",TMdPrm::AIM_CODE,TMdPrm::AIM_PERC,TMdPrm::AIM_VOLT).c_str(),_("Code;%;Voltage")));
    tPrm.fldAdd(new TFld("PRMS",_("Addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"1000"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
    TController(name_c,daq_db,cfgelem), mPrior(cfg("PRIOR").getId()), mSched(cfg("SCHEDULE")),
    mPer(1000000000), prcSt(false), callSt(false)
{
    cfg("PRM_BD").setS("DiamPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );

    if(startStat() && !redntUse())
    {
        if(callSt)	val += TSYS::strMess(_("Call now. "));
        if(period())	val += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-3*period()).c_str());
        else val += TSYS::strMess(_("Call next by cron '%s'. "), tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
        val += TSYS::strMess(_("Spent time: %s. "), tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str());
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    if(prcSt)	return;

    //> Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

    //> Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this, 10);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true));
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res(enRes, true);

    unsigned i_prm;
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
        if(pHd[i_prm].at().id() == id)	break;

    if(val && i_prm >= pHd.size()) pHd.push_back(at(id));
    if(!val && i_prm < pHd.size()) pHd.erase(pHd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr*)icntr;

    try
    {
        for(bool isStart = true, isStop = false; true; )
        {
            if(!cntr.redntUse())
            {
                //> Update controller's data
                cntr.callSt = true;
                ResAlloc res(cntr.enRes, false);
                for(unsigned i_p = 0; i_p < cntr.pHd.size(); i_p++) cntr.pHd[i_p].at().getVals("", isStart, isStop);
                res.release();
                cntr.callSt = false;
            }

            cntr.prcSt = true;

            //> Calc next work time and sleep
            if(isStop)	break;
            TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())), &cntr.mLag);
            if(TSYS::taskEndRun()) isStop = true;
    	    if(!cntr.redntUse()) isStart = false;
        }
    }
    catch(TError err) { mess_err(err.cat.c_str(), err.mess.c_str()); }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
        TController::cntrCmdProc(opt);
        ctrMkNode("fld", opt, -1, "/cntr/cfg/SCHEDULE", mSched.fld().descr(), startStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 4,
            "tp","str", "dest","sel_ed", "sel_list",TMess::labSecCRONsel(), "help",TMess::labSecCRON());
        ctrMkNode("fld", opt, -1, "/cntr/cfg/PRIOR", cfg("PRIOR").fld().descr(), startStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 1, "help",TMess::labTaskPrior());
        return;
    }
    //> Process command to page
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), pEl("w_attr"), mTP(cfg("TP").getId()), mADDR(cfg("ADDR").getId()), mINT(cfg("INT").getId()),
    mS_RATE(cfg("S_RATE").getId()), mAImode(cfg("AI_VAL").getId()), asynchRd(cfg("ASYNCH_RD").getBd()), aiScInt(0), prevTrans(0)
{
    dscaioint.sample_values = NULL;
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&pEl)) vlElemAtt(&pEl);
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat())
    {
        if(val.name() == "err")
        {
            if(!enableStat()) val.setS(_("1:Parameter is disabled."), 0, true);
            else if(!owner().startStat()) val.setS(_("2:Acquisition is stopped."), 0, true);
        }
        else val.setS(EVAL_STR, 0, true);
        return;
    }

    if(owner().redntUse()) return;

    ResAlloc res(devRes, true);
    if(val.name() == "err")
    {
        if(!acqErr.getVal().empty())	val.setS(acqErr.getVal(), 0, true);
        else if(dscs.op_type == OP_TYPE_INT)
	    val.setS(TSYS::strMess(_("0:AI acquisition into interrupt mode; SampleRate=%g; Drift=%gs; Overflows=%u; LostCycles=%u; SRateCor=%u."),
		dscaioint.conversion_rate,st_drift,st_overflows,st_lostcycles,st_sRateCor), 0, true);
        else val.setS("0", 0, true);
    }
    else if(!asynchRd) getVals(val.name());
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat()) vo.setS(EVAL_STR, 0, true);

    if(vl.isEVal() || vl == pvl) return;

    //> Send to active reserve station
    if(owner().redntUse())
    {
        XMLNode req("set");
        req.setAttr("path", nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id", vo.name())->setText(vl.getS());
        SYS->daq().at().rdStRequest(owner().workId(), req);
        return;
    }

    //> Direct write
    ResAlloc res(devRes, true);
    BYTE rez = DE_NONE;
    string errRez;
    if(vo.name().compare(0,2,"ao") == 0)
    {
	int acnl = atoi(vo.name().c_str()+2);
    	int ao_cfg = vo.fld().reserve().size() ? strtol(vo.fld().reserve().c_str(),NULL,0) : -1;
	if(ao_cfg > 0)
	{
	    DSCDASETTINGS dasettings;
	    memset(&dasettings, 0, sizeof(DSCDASETTINGS));
	    dasettings.gain	=  ao_cfg&0x0F;
	    dasettings.polarity	= (ao_cfg&0x10) ? TRUE : FALSE;
	    dasettings.range	= (ao_cfg&0x20) ? TRUE : FALSE;
	    dasettings.daPolEn	= (ao_cfg&0x40) ? TRUE : FALSE;
	    dscDASetSettings(dscb, &dasettings);
	}
	int res = (dev.AO>>8)&0xFF;
	if(!res) res = 12;
	if((rez=dscDAConvert(dscb,acnl,(int)(vmax(0,vmin(100,vl.getR()))*((1<<res)-1)/100))) != DE_NONE)
	{ errRez = errDSC("dscDAConvert"); vo.setR(EVAL_REAL, 0, true); }
    }
    else if(vo.name().compare(0,2,"do") == 0)
    {
	int i_ch = 0, i_p = 0;
	if(sscanf((vo.name().c_str()+2),"%d_%d",&i_ch,&i_p) != 2) return;
	bool setVl = vl.getB()^((dInOutRev[(dev.DI&0xFF)+i_ch]>>i_p)&1);
	if(mTP == DSC_IR104)	rez = dscSetRelay(dscb, (i_ch*i_p)+1, setVl);
	else rez = dscDIOOutputBit(dscb, i_ch, i_p, setVl);
	if(rez != DE_NONE) { errRez = errDSC((mTP==DSC_IR104)?"dscSetRelay":"dscDIOOutputBit"); vo.setB(EVAL_BOOL, 0, true); }
    }

    if(errRez.size())	{ acqErr.setVal(errRez); mess_err(nodePath().c_str(), "%s", errRez.c_str()); }
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    ERRPARAMS errorParams;
    string chnId, chnNm;
    vector<string> als;

    if(enableStat()) return;

    //> Check inited of Diamond API
    if(!mod->drvInitOk()) throw TError(nodePath().c_str(),_("DSC driver is not initialized!"));
    dev = mod->devs[mTP];
    if(dev.name.empty())  throw TError(nodePath().c_str(),_("Select device %d error!"),mTP);
    dev.AI = (dev.AI&0xFFFF00) | vmin(dev.AI&0xFF, atoi(modPrm("modAI",TSYS::int2str(dev.AI&0xFF)).c_str()));

    ResAlloc res(devRes, true);

    //> Init Board
    DSCCB dsccb;
    memset(&dsccb, 0 ,sizeof(DSCCB));
    dsccb.base_address = mADDR;
    dsccb.int_level = mINT;
    //if(mTP == DSC_HELIOS) dsccb.DAC_Config = 1 ; // for 16 bit mode DAC.
    if(dscInitBoard(mTP,&dsccb,&dscb) != DE_NONE)
    {
	dscGetLastError(&errorParams);
	throw TError(nodePath().c_str(), _("dscInitBoard '%s'(%xh) error: %s %s"),
	    dev.name.c_str(), mADDR, dscGetErrorString(errorParams.ErrCode), errorParams.errstring);
    }

    TParamContr::enable();

    //> AI processing
    aiScInt = atoi(modPrm("AI_SCAN_INT","0").c_str());
    for(unsigned i_a = 0; i_a < (dev.AI&0xFF); i_a++)
    {
        chnId = TSYS::strMess("ai%d",i_a); chnNm = TSYS::strMess(_("Analog input %d"),i_a);
	TFld::Type chnTp = (mAImode == 0) ? TFld::Integer : TFld::Real;

	//>> Try remove AI attribute for different type
	unsigned cFldId = pEl.fldId(chnId, true);
	if(cFldId < pEl.fldSize() && pEl.fldAt(cFldId).type() != chnTp)
	    try{ pEl.fldDel(cFldId); }
	    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }

	pEl.fldAt(pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),chnTp,TFld::NoWrite|TVal::DirRead))).
	    setReserve(modPrm(TSYS::strMess("AI_TP%d",i_a),"0x00"));
	als.push_back(chnId);
    }

    //> AO processing
    for(unsigned i_a = 0; i_a < (dev.AO&0xFF); i_a++)
    {
        chnId = TSYS::strMess("ao%d",i_a); chnNm = TSYS::strMess(_("Analog output %d"),i_a);
        pEl.fldAt(pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TVal::DirWrite))).
                setReserve(dev.aoTypes.size() ? modPrm(TSYS::strMess("AO_TP%d",i_a),"0x00") : string(""));
        als.push_back(chnId);
    }

    //>> Set DIO config
    //> DIO processing
    if(dev.DIO)
    {
        int directDIO = atoi(modPrm("DirectDIO").c_str());
	BYTE cfgBytes[10];
	memset(cfgBytes, 0, sizeof(cfgBytes));

        for(unsigned i_ch = 0; i_ch < (dev.DIO&0xFF); i_ch++)
        {
            dInOutRev[i_ch] = atoi(modPrm("dIORev"+TSYS::int2str(i_ch)).c_str());
            bool curDIOOut = directDIO&(1<<i_ch);
            //> Prepare specific board configuration
	    switch(mTP)
	    {
		case DSC_RMM: case DSC_ATHENA: case DSC_NEPTUNE:
		case DSC_DMM32: case DSC_DMM32X: 	//Use only mode 0
		    cfgBytes[0] |= 0x80;
		    if(!curDIOOut)
			switch(i_ch)
			{
			    case 0:	cfgBytes[0] |= 0x10;	break;
			    case 1:	cfgBytes[0] |= 0x02;	break;
			    case 2:	cfgBytes[0] |= 0x09;	break;
			}
		    break;
		case DSC_OMM: case DSC_OMMDIO:
		    cfgBytes[0] |= 0x80;
		    cfgBytes[1] |= 0x80;
		    if(!curDIOOut)
			switch(i_ch)
			{
			    case 0:	cfgBytes[0] |= 0x10;	break;
			    case 1:	cfgBytes[0] |= 0x02;	break;
			    case 2:	cfgBytes[0] |= 0x09;	break;
			    case 3:	cfgBytes[1] |= 0x10;	break;
			    case 4:	cfgBytes[1] |= 0x02;	break;
			    case 5:	cfgBytes[1] |= 0x09;	break;
			}
		    break;
		case DSC_GPIO11_DIO: case DSC_GPIO21:	break;	//No configure need
		case DSC_HERCEBX:
		    if(!curDIOOut)	cfgBytes[0] |= (1<<i_ch);
		    break;
		case DSC_HELIOS:
		    cfgBytes[0] = 0;
		    switch(i_ch)
		    {
			case 0:	if(!curDIOOut) cfgBytes[1] |= 0x10;	break;
			case 1:	if(!curDIOOut) cfgBytes[1] |= 0x08;	break;
			case 2:	if(!curDIOOut) cfgBytes[1] |= 0x03;	break;
			case 3:
			    cfgBytes[2] = 1;
			    cfgBytes[3] = curDIOOut ? 0x00 : 0xFF;
			    dscDIOSetConfig(dscb, cfgBytes+2);
			    break;
			case 4:
			    cfgBytes[2] = 2;
			    cfgBytes[3] = curDIOOut ? 0x00 : 0xFF;
			    dscDIOSetConfig(dscb, cfgBytes+2);
			    break;
		    }
		    break;
		default: throw TError(nodePath().c_str(),_("Board's '%s' DIO configuration unsupported."),dev.name.c_str());
	    }
            //> Attributes create
            if((directDIO>>i_ch)&1)
                for(int i_o = 0; i_o < 8; i_o++)
                {
                    chnId = TSYS::strMess("do%d_%d",i_ch,i_o); chnNm = TSYS::strMess(_("Digital out %d.%d"),i_ch,i_o);
                    pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite|TVal::DirRead));
                    als.push_back(chnId);
                }
            else
                for(int i_i = 0; i_i < 8; i_i++)
                {
                    chnId = TSYS::strMess("di%d_%d",i_ch,i_i); chnNm = TSYS::strMess(_("Digital input %d.%d"),i_ch,i_i);
                    pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite|TVal::DirRead));
                    als.push_back(chnId);
                }
	}
	//> Set specific board configuration
	dscDIOSetConfig(dscb, cfgBytes);
    }

    //> DI and DO processing
    for(unsigned i_ch = 0; i_ch < ((dev.DI&0xFF)+(dev.DO&0xFF)); i_ch++)
    {
        //> Reverse configuration load
        dInOutRev[i_ch] = atoi(modPrm("dIORev"+TSYS::int2str(i_ch)).c_str());

        //> Attributes create
        if(i_ch < (dev.DI&0xFF))
            for(int i_i = 0; i_i < 8; i_i++)
            {
                chnId = TSYS::strMess("di%d_%d",i_ch,i_i); chnNm = TSYS::strMess(_("Digital input %d.%d"),i_ch,i_i);
                pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite|TVal::DirRead));
                als.push_back(chnId);
            }
        else
            for(int i_o = 0; i_o < 8; i_o++)
            {
                chnId = TSYS::strMess("do%d_%d",i_ch-(dev.DI&0xFF),i_o); chnNm = TSYS::strMess(_("Digital out %d.%d"),i_ch-(dev.DI&0xFF),i_o);
                pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite|TVal::DirRead));
                als.push_back(chnId);
            }
    }

    //> Check for delete DAQ parameter's attributes
    for(int i_p = 0; i_p < (int)pEl.fldSize(); i_p++)
    {
        unsigned i_l;
        for(i_l = 0; i_l < als.size(); i_l++)
            if(pEl.fldAt(i_p).name() == als[i_l])
                break;
        if(i_l >= als.size())
            try{ pEl.fldDel(i_p); i_p--; }
            catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    if(owner().startStat()) getVals("", true);

    owner().prmEn(id(), true);
}

void TMdPrm::disable()
{
    if(!enableStat()) return;

    owner().prmEn(id(), false);

    if(owner().startStat()) getVals("", false, true);

    TParamContr::disable();

    ResAlloc res(devRes, true);
    dscFreeBoard(dscb);
}

void TMdPrm::getVals( const string &atr, bool start, bool stop )
{
    if(!enableStat())	return;

    vector<string> als;
    if(atr.empty() && asynchRd)
    {
        if(!asynchRd)	return;
        vlList(als);
    }
    else als.push_back(atr);

    ResAlloc res(devRes, true);

    string errRez;
    BYTE rez = DE_NONE;
    BYTE wBt, wPort = 255;
    DSCADSETTINGS dscadsettings;
    memset(&dscadsettings, 0, sizeof(DSCADSETTINGS));

    if(atr.empty())	//Call from task
    {
	int wPer = owner().period()/1000;

	//> Interrupt mode check and start
	if(start)
	{
	    dscs.op_type = OP_TYPE_NONE;
	    if(dscaioint.sample_values) { free(dscaioint.sample_values); dscaioint.sample_values = NULL; }
	    //>> Period limit (sample time:60s)
	    if(dev.AI&0xFF && dev.AI&0xFF00 && mINT > 0 && mS_RATE > 0 && wPer > (2*1000000*(dev.AI&0xFF)/mS_RATE) && wPer < 60000000ll)
	    {
		//>> Channel mode set
		dscadsettings.current_channel = 0;
    		int aiCfg		= strtol(vlAt("ai0").at().fld().reserve().c_str(), NULL, 0);
		dscadsettings.gain	= aiCfg&0x0F;
		dscadsettings.polarity	= (aiCfg&0x10) ? TRUE : FALSE;
		dscadsettings.range	= (aiCfg&0x20) ? TRUE : FALSE;
		dscadsettings.addiff	= (aiCfg&0x40) ? TRUE : FALSE;
		//dscadsettings.load_cal	= TRUE;
		dscadsettings.scan_interval = aiScInt;
		//>> Adapte to sample rate by maximum
		/*map<int,int> ScIntMds;
		ScIntMds[4] = SCAN_INTERVAL_4;
		ScIntMds[5] = SCAN_INTERVAL_5;
		ScIntMds[9] = SCAN_INTERVAL_9;
		ScIntMds[10] = SCAN_INTERVAL_10;
		ScIntMds[15] = SCAN_INTERVAL_15;
		ScIntMds[20] = SCAN_INTERVAL_20;
		for(map<int,int>::reverse_iterator i_m = ScIntMds.rbegin(); i_m != ScIntMds.rend(); i_m++)
		    if(i_m->first < (1000000/(mS_RATE*(dev.AI&0xFF))))
		    {
			dscadsettings.scan_interval = vmax(dscadsettings.scan_interval, i_m->second);
			break;
		    }*/
		//>> Scan interval adapting to set first minimum from
		if(dscADSetSettings(dscb,&dscadsettings) != DE_NONE) mess_err(nodePath().c_str(), errDSC("dscADSetSettings").c_str());

		//>> Interrupt mode start
		ERRPARAMS errorParams;
		mS_RATE = vmin(mS_RATE, ((dev.AI>>8)&0xFF)*1000);

		memset(&dscaioint, 0, sizeof(DSCAIOINT));
		dscaioint.conversion_rate = 2*(mS_RATE/2);
		dscaioint.num_conversions = 2*wPer*(int64_t)dscaioint.conversion_rate*(dev.AI&0xFF)/1000000;
		dscaioint.cycle = TRUE;
		dscaioint.internal_clock = TRUE;
		dscaioint.low_channel = 0;
		dscaioint.high_channel = (dev.AI&0xFF)-1;
		dscaioint.external_gate_enable = FALSE;
		dscaioint.internal_clock_gate = FALSE;
		dscaioint.dump_threshold = dscaioint.num_conversions/4;			//Set to 0.5 from process period
		while(dscaioint.dump_threshold > 65535) dscaioint.dump_threshold /= 2;	//Intermediately limited to "WORD" < 65536
		dscaioint.fifo_enab = TRUE;
		unsigned int fifoDepth = dev.aiSzFIFO;
		while(fifoDepth > 10 && ((dscaioint.dump_threshold%fifoDepth) || (fifoDepth%(dscaioint.high_channel+1)))) fifoDepth--;	//>> Align FIFO size to conversions number
		dscaioint.fifo_depth = fifoDepth;
		if(owner().messLev() == TMess::Debug)
            	    mess_debug_(nodePath().c_str(), _("AI interrupt: Init for: ConvRate=%g; NumbConv=%lu; FIFOdepth=%d; dump_threshold=%lu; scan_interval=%d."),
			dscaioint.conversion_rate, dscaioint.num_conversions, dscaioint.fifo_depth, dscaioint.dump_threshold, dscadsettings.scan_interval);
		dscaioint.sample_values = (DSCSAMPLE*)malloc(sizeof(DSCSAMPLE) * dscaioint.num_conversions);
		dscs.transfers = 0;
                dscs.op_type = OP_TYPE_INT;
		if(dscADScanInt(dscb,&dscaioint) != DE_NONE)
		{
		    dscGetLastError(&errorParams);
		    mess_err(nodePath().c_str(), _("Interrupt mode disabled by dscADScanInt error: %s %s"), dscGetErrorString(errorParams.ErrCode), errorParams.errstring);
		    dscs.op_type = OP_TYPE_NONE;
		    free(dscaioint.sample_values); dscaioint.sample_values = NULL;
		}
		else
		{
		    prevTrans = dscs.transfers = dscs.overflows = st_overflows = st_lostcycles = st_sRateCor = st_drift = 0;
            	    dscs.op_type = OP_TYPE_INT;
		    cTm = diffTm = 0;
		}
	    }
	}

	//>> Interrupt mode stop
	if(stop)
	{
	    if(dscs.op_type != OP_TYPE_NONE) { dscCancelOp(dscb); dscs.op_type = OP_TYPE_NONE; }
	    if(dscaioint.sample_values)	{ free(dscaioint.sample_values); dscaioint.sample_values = NULL; }
	}

	//> Samples buffer process into interrup mode
	if(!start && !stop && dscs.op_type == OP_TYPE_INT)
	{
	    dscGetStatus(dscb, &dscs);
	    unsigned aiSz = (dscaioint.high_channel+1);
	    DWORD curTrans = (dscs.transfers/aiSz)*aiSz;
	    int64_t dtSz = 1000000ll*(curTrans+((curTrans<prevTrans)?dscaioint.num_conversions:0)-prevTrans)/(aiSz*dscaioint.conversion_rate);
	    int64_t curTime = TSYS::curTime();
	    if(!cTm) cTm = diffTm = curTime-dtSz;
	    st_drift = 1e-6*(curTime-(cTm+dtSz));

	    if(owner().messLev() == TMess::Debug)
		mess_debug_(nodePath().c_str(), _("AI interrupt: Cycle: ConvRate=%g; transfers=%lu; prev_transfers=%lu; total_transfers=%lu; overflows=%lu; lag=%lld; drift=%gs."),
		    dscaioint.conversion_rate, dscs.transfers, prevTrans, dscs.total_transfers, dscs.overflows, owner().lag(), st_drift);

	    //> FIFO overflows
	    if(dscs.overflows > st_overflows)
	    {
		cTm += 1000000ll*(dscs.overflows-st_overflows)*dscaioint.fifo_depth/(aiSz*dscaioint.conversion_rate);
		st_drift = 1e-6*(curTime-(cTm+dtSz));
		mess_warning(nodePath().c_str(), _("AI interrupt: Overflows '%d' corrected to %lld! Drift=%gs."),
		    (dscs.overflows-st_overflows), 1000000ll*(dscs.overflows-st_overflows)*dscaioint.fifo_depth/(aiSz*dscaioint.conversion_rate), st_drift);
		st_overflows = dscs.overflows;
	    }
	    //> Controller cycles lost or big differ
	    else if(owner().lag()/owner().period() || fabs(st_drift) > (10*(float)wPer/1000000))
	    {
		cTm = diffTm = curTime-dtSz;
		st_drift = 1e-6*(curTime-(cTm+dtSz));
		mess_warning(nodePath().c_str(), _("AI interrupt: Lost cycles '%d'. Fast data time corrected! Drift=%gs."),
		    owner().lag()/owner().period(), st_drift);
		st_lostcycles += owner().lag()/owner().period();
	    }
	    //> Check for sample rate correction to real value
	    else if(fabs(st_drift) > ((float)wPer/1000000+(float)dscaioint.dump_threshold/(aiSz*dscaioint.conversion_rate)))
	    {
		if(owner().messLev() == TMess::Debug)
		    mess_debug_(nodePath().c_str(), _("AI interrupt: Correction: dt=%gs; dtSz=%lld; per=%d; crmult=%g."),
			st_drift, dtSz, wPer, -1e6*st_drift/(curTime-diffTm));
		dscaioint.conversion_rate = TSYS::realRound(dscaioint.conversion_rate*(1-1e6*st_drift/(curTime-diffTm)), 0, true);
		mess_warning(nodePath().c_str(), _("AI interrupt: Sample rate change to %g! Env: drift=%gs."), dscaioint.conversion_rate, st_drift);
		dtSz = 1000000ll*(curTrans+((curTrans<prevTrans)?dscaioint.num_conversions:0)-prevTrans)/(aiSz*dscaioint.conversion_rate);
		diffTm = cTm = curTime-dtSz;
		st_drift = 1e-6*(curTime-(cTm+dtSz));
		st_sRateCor++;
	    }

	    int aiCfg	= strtol(vlAt("ai0").at().fld().reserve().c_str(), NULL, 0);
	    map<int, DevFeature::rng>::iterator aiRng = dev.aiRngs.find(aiCfg);
	    int ADres	= (dev.AI>>16)&0xFF;
	    if(!ADres) ADres = 16;

	    for(unsigned i_a = 0; prevTrans != curTrans && i_a < aiSz; i_a++)
	    {
		AutoHD<TVal> ai = vlAt(TSYS::strMess("ai%d",i_a));
		//>> Place data to archive
		if(!ai.at().arch().freeStat() && ai.at().arch().at().srcMode() == TVArchive::PassiveAttr)
		{
		    AutoHD<TVArchive> arch = ai.at().arch();
		    int64_t wTm = cTm;
		    int code;
		    double rVal = 0;
		    for(DWORD i_s = prevTrans+i_a; true; )
		    {
			code = dscaioint.sample_values[i_s];
			if(ADres >= 16) code += 32768;
			switch(mAImode)
			{
			    case AIM_CODE: arch.at().setI(code, wTm);	break;
			    case AIM_PERC:
				rVal = 100*(double)code/(1<<ADres);
				arch.at().setR(rVal, wTm);
				break;
			    case AIM_VOLT:
				rVal = (aiRng != dev.aiRngs.end()) ?
				    aiRng->second.min+(((double)code)/(1<<ADres))*(aiRng->second.max-aiRng->second.min) : code;
				arch.at().setR(rVal, wTm);
				break;
			}
			i_s += aiSz;
			if(i_s >= dscaioint.num_conversions) i_s = i_a;
			if(i_s/aiSz == curTrans/aiSz) break;
			wTm += 1000000/dscaioint.conversion_rate;
		    }
		    //>> Last value of the buffer as last attribute value
		    switch(mAImode)
		    {
			case AIM_CODE:	ai.at().setI(code, wTm, true);	break;
			default:	ai.at().setR(rVal, wTm, true);	break;	//% and Voltage
		    }
		}
		//>> Place current data
		else
		{
		    DWORD i_s = ((curTrans == 0)?dscaioint.num_conversions:curTrans)-aiSz+i_a;
		    int code = dscaioint.sample_values[i_s];
		    if(ADres >= 16) code += 32768;
		    switch(mAImode)
		    {
			case AIM_CODE: ai.at().setI(code, 0, true);	break;
			case AIM_PERC: ai.at().setR(100*(double)code/(1<<ADres), 0, true);	break;
			case AIM_VOLT:
			    ai.at().setR((aiRng != dev.aiRngs.end()) ?
				aiRng->second.min+(((double)code)/(1<<ADres))*(aiRng->second.max-aiRng->second.min) : code, 0, true);
			    break;
		    }
		}
	    }
	    dtSz = 1000000ll*(curTrans+((curTrans<prevTrans)?dscaioint.num_conversions:0)-prevTrans)/(aiSz*dscaioint.conversion_rate);
	    cTm += dtSz;

	    prevTrans = curTrans;
	}
    }

    if(atr.empty() && !asynchRd)   return;

    //> Direct channels process
    for(unsigned i_a = 0; i_a < als.size(); i_a++)
    {
        AutoHD<TVal> val = vlAt(als[i_a]);

        if(als[i_a].compare(0,2,"ai") == 0)
        {
	    if(dscs.op_type == OP_TYPE_INT)	continue;	//Processing generic for all analog inputs by interrupt mode
	    dscadsettings.current_channel = atoi(als[i_a].c_str()+2);
    	    int aiCfg	= strtol(val.at().fld().reserve().c_str(), NULL, 0);
	    dscadsettings.gain		= aiCfg&0x0F;
	    dscadsettings.polarity	= (aiCfg&0x10) ? TRUE : FALSE;
	    dscadsettings.range		= (aiCfg&0x20) ? TRUE : FALSE;
	    dscadsettings.addiff	= (aiCfg&0x40) ? TRUE : FALSE;
	    //dscadsettings.scan_interval = aiScInt;	//Need only for scan and interrupt sampling
	    //dscadsettings.load_cal	= TRUE;
	    if(dscADSetSettings(dscb,&dscadsettings) != DE_NONE){ errRez = errDSC("dscADSetSettings"); val.at().setR(EVAL_REAL, 0, true); }
	    DSCSAMPLE smpl;
	    if(dscADSample(dscb,&smpl) != DE_NONE)		{ errRez = errDSC("dscADSample"); val.at().setR(EVAL_REAL, 0, true); }
	    else
	    {
		int ADres = (dev.AI>>16)&0xFF;
		if(!ADres) ADres = 16;
		int code = (ADres<16) ? smpl : (int)smpl+32768;
		switch(mAImode)
		{
		    case AIM_CODE: val.at().setI(code, 0, true);	break;
		    case AIM_PERC: val.at().setR(100*(double)code/(1<<ADres), 0, true);	break;
		    case AIM_VOLT:
		    {
			map<int, DevFeature::rng>::iterator aiRng = dev.aiRngs.find(aiCfg);
			val.at().setR((aiRng != dev.aiRngs.end()) ?
				aiRng->second.min+(((double)code)/(1<<ADres))*(aiRng->second.max-aiRng->second.min) : code, 0, true);
			break;
		    }
		}
	    }
        }
        else if(als[i_a].compare(0,2,"di") == 0 || als[i_a].compare(0,2,"do") == 0)
        {
	    int i_ch = 0, i_p = 0;
    	    if(sscanf((als[i_a].c_str()+2),"%d_%d",&i_ch,&i_p) != 2) return;
	    BYTE i_bt;

	    if(mTP == DSC_IR104)
	    {
		if(als[i_a].compare(0,2,"di") == 0)
		    rez = dscIR104OptoInput(dscb, (i_ch*i_p)+1, &i_bt);
		else rez = dscIR104RelayInput(dscb, (i_ch*i_p)+1, &i_bt);
	    }
	    else
	    {
		if(wPort != i_ch) { rez = dscDIOInputByte(dscb, i_ch, &wBt); wPort = i_ch; }
		i_bt = (wBt>>i_p)&1;
	    }
            if(rez != DE_NONE)	{ errRez = errDSC((mTP==DSC_IR104)?"dscIR104OptoInput":"dscDIOInputBit"); val.at().setB(EVAL_BOOL, 0, true); }
            else val.at().setB((i_bt^(dInOutRev[i_ch]>>i_p))&1, 0, true);
        }
    }

    acqErr.setVal(errRez);
    if(errRez.size()) mess_err(nodePath().c_str(), "%s", errRez.c_str());
}

string TMdPrm::modPrm( const string &prm, const string &def )
{
    string rez;
    XMLNode prmNd;
    try
    {
        prmNd.load(cfg("PRMS").getS());
        string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
        if(!sa.size())  return (rez=prmNd.attr(prm)).empty()?def:rez;
        //> Internal node
        for(unsigned i_n = 0; i_n < prmNd.childSize(); i_n++)
            if(prmNd.childGet(i_n)->name() == sobj)
                return (rez=prmNd.childGet(i_n)->attr(sa)).empty()?def:rez;
    } catch(...){ }

    return def;
}

void TMdPrm::setModPrm( const string &prm, const string &val )
{
    XMLNode prmNd("ModCfg");
    try { prmNd.load(cfg("PRMS").getS()); } catch(...){ }

    if(modPrm(prm) != val) modif();
    string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
    if(!sa.size()) prmNd.setAttr(prm,val);
    //> Internal node
    else
    {
        unsigned i_n;
        for(i_n = 0; i_n < prmNd.childSize(); i_n++)
            if(prmNd.childGet(i_n)->name() == sobj)
            { prmNd.childGet(i_n)->setAttr(sa,val); break; }
        if(i_n >= prmNd.childSize())
            prmNd.childAdd(sobj)->setAttr(sa,val);
    }

    cfg("PRMS").setS(prmNd.save(XMLNode::BrAllPast));
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")  { TParamContr::cntrCmdProc(opt); return; }

    DevFeature tdev = mod->devs[mTP];

    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld", opt, -1, "/prm/cfg/TP", cfg("TP").fld().descr(), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 2,
	    "dest","select", "select","/prm/cfg/brdLst");
	ctrMkNode("fld", opt, -1, "/prm/cfg/ADDR", cfg("ADDR").fld().descr(), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID);
	if(tdev.AI&0xFF && tdev.AI&0xFF00)
	{
	    ctrMkNode("fld", opt, -1, "/prm/cfg/INT", cfg("INT").fld().descr(), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID);
	    ctrMkNode("fld", opt, -1, "/prm/cfg/S_RATE", cfg("S_RATE").fld().descr(), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID);
	}
	else
	{
	    ctrRemoveNode(opt, "/prm/cfg/INT");
	    ctrRemoveNode(opt, "/prm/cfg/S_RATE");
	}
	ctrMkNode("fld", opt, -1, "/prm/cfg/AI_VAL", cfg("AI_VAL").fld().descr(), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID);
        ctrMkNode("fld", opt, -1, "/prm/cfg/ASYNCH_RD", cfg("ASYNCH_RD").fld().descr(), RWRWR_, "root", SDAQ_ID);
        ctrRemoveNode(opt, "/prm/cfg/PRMS");
        if(tdev.AI)
	    ctrMkNode("fld", opt, -1, "/prm/cfg/modAI", _("AI numbers process"), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 3,
		"tp","dec", "min","0", "max",TSYS::int2str(tdev.AI&0xFF).c_str());

        //>> Configuration page: AI type, DIO direction and DIO inversion
        if(tdev.name.size() && ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
        {
	    //> AI processing
	    if(tdev.AI&0xFF)
	    {
		if(tdev.AI&0xFF00)	//FIFO mode support
		    ctrMkNode("fld", opt, -1, "/cfg/aiScInt", _("Analog inputs scan interval (us)"), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 4, "tp","dec", "dest","select",
			"sel_id",TSYS::strMess("%d;%d;%d;%d;%d;%d",SCAN_INTERVAL_4,SCAN_INTERVAL_5,SCAN_INTERVAL_9,SCAN_INTERVAL_10,SCAN_INTERVAL_15,SCAN_INTERVAL_20).c_str(),
			"sel_list","4;5;9;10;15;20");

        	for(int i_v = 0; i_v < vmin(atoi(modPrm("modAI",TSYS::int2str(dev.AI&0xFF)).c_str()), (tdev.AI&0xFF)); i_v++)
        	{
		    XMLNode *tnd = ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/inTp%d",i_v).c_str(),
			    TSYS::strMess((i_v==0 && tdev.AI&0xFF00)?_("Input %d type (all inputs)"):_("Input %d type"),i_v).c_str(),
			    enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","hex");
		    if(tdev.aiTypes.size()) tnd->setAttr("dest","select")->
			setAttr("sel_id",TSYS::strParse(tdev.aiTypes,0,"\n"))->
			setAttr("sel_list",TSYS::strParse(tdev.aiTypes,1,"\n"));
		    if(dscs.op_type == OP_TYPE_INT)	break;
		}
	    }
	    //> AO processing
            for(int i_v = 0; tdev.aoTypes.size() && i_v < (tdev.AO&0xFF); i_v++)
		ctrMkNode("fld", opt, -1, TSYS::strMess("/cfg/outTp%d",i_v).c_str(),
			TSYS::strMess(_("Output %d type"),i_v).c_str(), enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID,
			4, "tp","hex", "dest","select", "sel_id",TSYS::strParse(tdev.aoTypes,0,"\n").c_str(),
							"sel_list",TSYS::strParse(tdev.aoTypes,1,"\n").c_str());
            //>> DIO processing
            for(unsigned i_ch = 0; i_ch < (tdev.DIO&0xFF); i_ch++)
            {
                ctrMkNode("fld", opt, -1, TSYS::strMess("/cfg/chnOut%d",i_ch).c_str(), TSYS::strMess(_("DIO %d: out"),i_ch).c_str(),
                    enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 1, "tp","bool");
                for(unsigned i_n = 0; i_n < 8; i_n++)
                    ctrMkNode("fld", opt, -1, TSYS::strMess("/cfg/nRevs%d_%d",i_ch,i_n).c_str(),
                        (i_n==0)?TSYS::strMess(_("DIO %d: reverse"),i_ch).c_str():"",
                        enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 1, "tp","bool");
            }
            //>> DI and DO processing
            for(unsigned i_ch = 0; i_ch < ((tdev.DI&0xFF)+(tdev.DO&0xFF)); i_ch++)
                for(unsigned i_n = 0; i_n < 8; i_n++)
                    ctrMkNode("fld", opt, -1, TSYS::strMess("/cfg/nRevs%d_%d",i_ch,i_n).c_str(), (i_n==0) ?
                        ((i_ch < (tdev.DI&0xFF)) ? TSYS::strMess(_("DI %d reverse"),i_ch).c_str() :
                                            	   TSYS::strMess(_("DO %d reverse"),i_ch-(tdev.DI&0xFF)).c_str()) : "",
                                            		enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 1, "tp","bool");
        }
        return;
    }

    //> Process command to page
    //>> Generic AI CNTR channels processing limit set configuration
    if(a_path.compare(0,12,"/prm/cfg/mod") == 0)
    {
        if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(modPrm(a_path.substr(9), TSYS::int2str(tdev.AI&0xFF)));
        if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm(a_path.substr(9), opt->text());
    }
    else if(a_path == "/prm/cfg/brdLst" && ctrChkNode(opt))
	for(map<int, DevFeature>::iterator id = mod->devs.begin(); id != mod->devs.end(); ++id)
	    opt->childAdd("el")->setAttr("id", TSYS::int2str(id->first))->setText(id->second.name);
    //>> AI processing
    else if(tdev.AI && a_path == "/cfg/aiScInt")
    {
        if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(modPrm("AI_SCAN_INT","0"));
        if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) setModPrm("AI_SCAN_INT", opt->text());
    }
    else if(tdev.AI && a_path.compare(0,9,"/cfg/inTp") == 0)
    {
        if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(modPrm("AI_TP"+a_path.substr(9),"0x00"));
        if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) setModPrm("AI_TP"+a_path.substr(9), opt->text());
    }
    //>> AO processing
    else if(tdev.AO && a_path.compare(0,10,"/cfg/outTp") == 0)
    {
        if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(modPrm("AO_TP"+a_path.substr(10),"0x00"));
        if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) setModPrm("AO_TP"+a_path.substr(10), opt->text());
    }
    //> DIO, DI and DO processing
    else if(tdev.DIO || tdev.DI || tdev.DO)
    {
        if(a_path.compare(0,11,"/cfg/chnOut") == 0)
        {
            int rout = atoi(a_path.c_str()+11);
            if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(atoi(modPrm("DirectDIO").c_str())&(1<<rout)?"1":"0");
            if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
                setModPrm("DirectDIO",TSYS::int2str(atoi(opt->text().c_str()) ? atoi(modPrm("DirectDIO").c_str()) | (1<<rout) :
                                                                                atoi(modPrm("DirectDIO").c_str()) & ~(1<<rout)));
        }
        else if(a_path.compare(0,10,"/cfg/nRevs") == 0)
        {
            int i_ch = 0, i_n = 0;
            sscanf(a_path.c_str(),"/cfg/nRevs%d_%d",&i_ch,&i_n);
            int chVl = atoi(modPrm("dIORev"+TSYS::int2str(i_ch)).c_str());
            if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText((chVl&(1<<i_n))?"1":"0");
            if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
                setModPrm("dIORev"+TSYS::int2str(i_ch), TSYS::int2str(atoi(opt->text().c_str()) ? (chVl|(1<<i_n)) : (chVl & ~(1<<i_n))));
        }
        else TParamContr::cntrCmdProc(opt);
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    if(val.name().compare(0,2,"ai") == 0 && dscs.op_type == OP_TYPE_INT)
    {
	val.arch().at().setSrcMode(TVArchive::PassiveAttr);
	val.arch().at().setPeriod(1000000/dscaioint.conversion_rate);
    }
    else if(asynchRd)
    {
	val.arch().at().setSrcMode(TVArchive::PassiveAttr);
	val.arch().at().setPeriod(owner().period() ? (int64_t)owner().period()/1000 : 1000000);
    }
    else
    {
	val.arch().at().setSrcMode(TVArchive::ActiveAttr);
	val.arch().at().setPeriod(SYS->archive().at().valPeriod()*1000);
    }
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

string TMdPrm::errDSC( const string &func )
{
    ERRPARAMS errorParams;
    dscGetLastError(&errorParams);
    string rez = TSYS::strMess(_("%s error: %s %s"), func.c_str(), dscGetErrorString(errorParams.ErrCode), errorParams.errstring);
    mess_err(nodePath().c_str(), "%s", rez.c_str());
    return rez;
}

//*************************************************
//* DevFeature                                    *
//*************************************************
void DevFeature::setAITypes( const string &vl )
{
    aiTypes = vl;
    string idxs = TSYS::strLine(vl, 0),
	   vals = TSYS::strLine(vl, 1),
	   curIdx, curVal;
    for(int offIdx = 0, offVl = 0; (curIdx=TSYS::strParse(idxs,0,";",&offIdx)).size() && (curVal=TSYS::strParse(vals,0,";",&offVl)).size(); )
    {
	float rMin = 0, rMax = 0;
    	if(sscanf(curVal.c_str(),"[%f,%f]",&rMin,&rMax) == 2 && rMax > rMin)
    	    aiRngs[strtol(curIdx.c_str(),NULL,0)] = (rng){ rMin, rMax };
    }
}

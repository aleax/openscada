
//OpenSCADA system module DAQ.ICP_DAS file: ICP_module.cpp
/***************************************************************************
 *   Copyright (C) 2010-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <signal.h>

#include <tsys.h>
#include <ttiparam.h>

extern "C"
{
#include "msw.h"
}

#include "da_LP_8x.h"
#include "da_87x.h"
#include "da_ISA.h"
#include "ICP_module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"ICP_DAS"
#define MOD_NAME	_("ICP DAS hardware")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"1.0.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow implementation for 'ICP DAS' hardware support.\
 Include main I-87xxx DCON modules, I-8xxx fast modules and boards on ISA bus.")
#define LICENSE		"GPL2"
//*************************************************

ICP_DAS_DAQ::TTpContr *ICP_DAS_DAQ::mod;  //Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new ICP_DAS_DAQ::TTpContr(source);
	return NULL;
    }
}

using namespace ICP_DAS_DAQ;

//******************************************************
//* TTpContr                                           *
//******************************************************
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
    for(unsigned i_da = 0; i_da < m_da.size(); i_da++)
	delete m_da[i_da];
    m_da.clear();
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Init DA sources
    daReg(new da_LP_8x());
    daReg(new da_87x());
    daReg(new da_ISA());

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("BUS",_("Bus"),TFld::Integer,TFld::Selected,"2","1","-1;0;1;2;3;4;5;6;7;8;9;10",
	    _("ISA;COM 1 (Master LP-8xxx);COM 1;COM 2;COM 3;COM 4;COM 5;COM 6;COM 7;COM 8;COM 9;COM 10")));
    fldAdd(new TFld("TR_OSCD",_("Transport"),TFld::String,TFld::NoFlag,i2s(2*atoi(OBJ_ID_SZ)+5).c_str(),TrIcpDasNm));
    fldAdd(new TFld("BAUD",_("Baudrate"),TFld::Integer,TFld::Selected,"6","115200",
	"300;600;1200;2400;4800;9600;19200;38400;57600;115200;230400;460800;500000;576000;921600",
	"300;600;1200;2400;4800;9600;19200;38400;57600;115200;230400;460800;500000;576000;921600"));
    fldAdd(new TFld("LP_PRMS",_("LinPAC parameters"),TFld::String,TFld::FullText,"1000"));
    fldAdd(new TFld("REQ_TRY",_("Serial request tries"),TFld::Integer,TFld::NoFlag,"1","1","1;10"));

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("MOD_TP",_("Module type"),TFld::String,TFld::HexDec|TCfg::NoVal,"20","-"));
    tpPrmAt(t_prm).fldAdd(new TFld("MOD_ADDR",_("Module address"),TFld::Integer,TCfg::NoVal,"3","0","0;255"));
    tpPrmAt(t_prm).fldAdd(new TFld("MOD_SLOT",_("Module slot"),TFld::Integer,TCfg::NoVal,"2","1","1;11"));
    tpPrmAt(t_prm).fldAdd(new TFld("MOD_PRMS",_("Module addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

void TTpContr::load_( )
{
    //> Load parameters from command line

}

void TTpContr::save_( )
{

}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

void TTpContr::daReg( DA *da )
{
    m_da.push_back(da);
}

void TTpContr::daTpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl )
{
    tpl.clear();
    if(ntpl) ntpl->clear();
    for(unsigned i_da = 0; i_da < m_da.size(); i_da++)
	m_da[i_da]->tpList(prm, tpl, ntpl);
}

DA *TTpContr::daGet( TMdPrm *prm )
{
    for(unsigned i_da = 0; prm->modTp.getS().size() && i_da < m_da.size(); i_da++)
    {
	vector<string> tpl;
	m_da[i_da]->tpList(prm,tpl);
	for(unsigned i_t = 0; i_t < tpl.size(); i_t++)
	    if(tpl[i_t] == prm->modTp.getS())
		return m_da[i_da];
    }

    return NULL;
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, TElem *cfgelem) :
	TController(name_c, daq_db, cfgelem),
	mPrior(cfg("PRIOR").getId()), mBus(cfg("BUS").getId()),
	mBaud(cfg("BAUD").getId()), connTry(cfg("REQ_TRY").getId()), mSched(cfg("SCHEDULE")), mTrOscd(cfg("TR_OSCD")),
	mPer(100000000), prcSt(false), call_st(false), endRunReq(false), tm_gath(0), mCurSlot(-1), numReq(0), numErr(0), numErrResp(0)
{
    cfg("PRM_BD").setS("ICPDASPrm_"+name_c);
    cfg("BUS").setI(1);
}

TMdContr::~TMdContr()
{
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus();

    if(startStat() && !redntUse())
    {
	if(call_st)	val += TSYS::strMess(_("Call now. "));
	if(period())	val += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-3*period()).c_str());
	else val += TSYS::strMess(_("Call next by cron '%s'. "),tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	val += TSYS::strMess(_("Spent time: %s. Serial requests %g, errors %g. "), tm2s(tm_gath).c_str(), numReq, numErr);
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name, &owner().tpPrmAt(type)); }

void TMdContr::disable_( )	{ }

void TMdContr::start_( )
{
    if(prcSt)	return;

    if(mBus == 0)
    {
	ResAlloc res(pBusRes, true);
	if(Open_SlotAll() > 0) throw TError(nodePath().c_str(), _("Open All LP-slots error."));
	if(Open_Slot(9) > 0) { Close_SlotAll(); throw TError(nodePath().c_str(), _("Open LP-slot 9 error.")); }
    }

    try
    {
	if(mBus >= 0)
	{
	    if(trOscd() == TrIcpDasNm)
	    {
		if(Open_Com((mBus?mBus:1), mBus?mBaud:115200, Data8Bit, NonParity, OneStopBit) > 0)
		    throw TError(nodePath().c_str(), _("Open COM%d port error."), (mBus?mBus:1));
	    }
	    else tr = SYS->transport().at().at(TSYS::strParse(trOscd(),0,".")).at().outAt(TSYS::strParse(trOscd(),1,"."));
	}

	numReq = numErr = numErrResp = 0;

	//> Schedule process
	mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

	//> Start the gathering data task
	SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this, 10);
    }
    catch(TError err)
    {
	if(mBus == 0)	{ Close_Slot(9); Close_SlotAll(); }
	throw;
    }
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endRunReq);

    //tr.free();
    if(mBus >= 0)
    {
	if(trOscd() == TrIcpDasNm) Close_Com(mBus?mBus:1);
	else tr.free();
    }
    if(mBus == 0) { pBusRes.resRequestW(); Close_Slot(9); Close_SlotAll(); pBusRes.resRelease(); }
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if((co.name() == "BUS" || co.name() == "BAUD") && startStat()) stop();

    return true;
}

string TMdContr::DCONCRC( string str )
{
    unsigned char CRC = 0;
    for(unsigned i = 0; i < str.size(); i++) CRC += (unsigned char)str[i];
    return TSYS::strMess("%02X",CRC);
}

string TMdContr::prmLP( const string &prm )
{
    XMLNode prmNd;
    try { prmNd.load(cfg("LP_PRMS").getS()); return prmNd.attr(prm); } catch(...){ }

    return "";
}

void TMdContr::setPrmLP( const string &prm, const string &vl )
{
    XMLNode prmNd("prms");
    try { prmNd.load(cfg("LP_PRMS").getS()); } catch(...){ }
    prmNd.setAttr(prm,vl);
    cfg("LP_PRMS").setS(prmNd.save(XMLNode::BrAllPast));
    modif();
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res( en_res, true );

    unsigned i_prm;
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    float wTm = 0;

    TMdContr &cntr = *(TMdContr*)icntr;

    cntr.endRunReq = false;

    //Init watchdog and get previous state
    if(cntr.mBus == 0) wTm = atof(cntr.prmLP("wTm").c_str());

    try
    {
	while(!cntr.endRunReq)
	{
	    if(!cntr.redntUse())
	    {
		cntr.call_st = true;
		int64_t t_cnt = TSYS::curTime();

		//Update controller's data
		ResAlloc res(cntr.en_res, false);
		for(unsigned i_p = 0; i_p < cntr.p_hd.size(); i_p++) cntr.p_hd[i_p].at().getVals();
		res.release();

		//Calc acquisition process time
		cntr.tm_gath = TSYS::curTime()-t_cnt;
		cntr.call_st = false;
	    }

	    //Watchdog timer process
	    if(cntr.mBus == 0 && cntr.period() && wTm > 0)
	    {
		ResAlloc res(cntr.reqRes, true);
		int wTmSet = 1e3*vmax(1.5e-9*cntr.period(), wTm);
		EnableWDT(wTmSet);
		if(cntr.messLev() == TMess::Debug) mess_debug_(cntr.nodePath().c_str(), _("Set watchdog to %d ms."), wTmSet);
		res.release();
	    }

	    cntr.prcSt = true;

	    //Calc next work time and sleep
	    TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
	}
    }
    catch(TError err)	{ mess_err(err.cat.c_str(), err.mess.c_str()); }

    //Watchdog timer disable
    if(cntr.mBus == 0 && wTm > 0) { ResAlloc res(cntr.reqRes, true); DisableWDT(); res.release(); }

    cntr.prcSt = false;

    return NULL;
}

string TMdContr::serReq( string req, char mSlot, bool CRC )
{
    string err;
    numReq++;

    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("REQ -> '%s'"), req.c_str());

    //Request by ICP DAS serial API
    if(mBus == 0 && mSlot != mCurSlot)	{ pBusRes.resRequestW(); ChangeToSlot(mSlot); mCurSlot = mSlot; pBusRes.resRelease(); }

    //Request by OpenSCADA output transport
    if(bus() >= 0 && trOscd() != TrIcpDasNm)
    {
	string rez;
	try
	{
	    if(!tr.at().startStat()) tr.at().start();
	    if(CRC) req += DCONCRC(req);
	    req += "\r";
	    char buf[1000];

	    ResAlloc resN(tr.at().nodeRes(), true);
	    for(int i_tr = 0; i_tr < vmax(1,vmin(10,connTry)); i_tr++)
	    {
		int resp_len = tr.at().messIO(req.data(), req.size(), buf, sizeof(buf), 0, true);
		rez.assign(buf, resp_len);
		// Wait tail
		while(resp_len && (rez.size() < 2 || rez[rez.size()-1] != '\r'))
		{
		    try{ resp_len = tr.at().messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError er){ break; }
		    rez.append(buf, resp_len);
		}
		if(rez.size() < 2 || rez[rez.size()-1] != '\r') { err = _("13:Error respond: Not full."); continue; }
		rez = rez.substr(0,rez.size()-1);
		if(CRC)
		{
		    if(strtol(rez.substr(rez.size()-2).c_str(),NULL,16) != strtol(DCONCRC(rez.substr(0,rez.size()-2)).c_str(),NULL,16))
		    { err = _("21:Invalid module CRC."); continue; }
		    rez = rez.substr(0,rez.size()-2);
		}

		if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("RESP -> '%s'"), rez.c_str());

		return rez;
	    }
	} catch(TError er) { err = "10:" + er.mess; }

	if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("ERR -> '%s': %s"), rez.c_str(), err.c_str());

	numErr++;
	return "";
    }

    //Request by ICP DAS serial API
    ResAlloc res(reqRes, true);

    WORD wT, rez;
    char szReceive[255]; szReceive[0] = 0;

    for(int i_tr = 0; i_tr < vmax(1,vmin(10,connTry)); i_tr++)
	if(!(rez=Send_Receive_Cmd(mBus?mBus:1,(char*)req.c_str(),szReceive,1,CRC,&wT)))
	{
	    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("RESP -> '%s'"), szReceive);
	    return szReceive;
	}
	else err = TSYS::strMess(_("13:Send_Receive_Cmd() error: %d."), rez);

    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("ERR -> '%s': %s"), szReceive, err.c_str());

    numErr++;
    return "";
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/LP_PRMS");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/BUS",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	if(mBus < 0) ctrRemoveNode(opt,"/cntr/cfg/TR_OSCD");
	else ctrMkNode("fld",opt,-1,"/cntr/cfg/TR_OSCD",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","select","select","/cntr/cfg/trLst");
	if(mBus <= 0 || trOscd() != TrIcpDasNm) ctrRemoveNode(opt,"/cntr/cfg/BAUD");
	if(mBus < 0) ctrRemoveNode(opt,"/cntr/cfg/REQ_TRY");
	if(mBus == 0 && ctrMkNode("area",opt,-1,"/LPcfg","LinPAC"))
	    ctrMkNode("fld",opt,-1,"/LPcfg/wTm",_("Watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,1,"tp","real");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt))
    {
	vector<string> sls;
	opt->childAdd("el")->setText(TrIcpDasNm);
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(mBus == 0 && a_path == "/LPcfg/wTm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(prmLP("wTm"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setPrmLP("wTm",opt->text());
    }
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name, tp_prm),
    p_el("w_attr"), extPrms(NULL), modTp(cfg("MOD_TP")), modAddr(cfg("MOD_ADDR").getId()), modSlot(cfg("MOD_SLOT").getId()),
    endRunReq(false), prcSt(false), wTm(0), clcCnt(0), da(NULL)
{
    for(int i_c = 0; i_c < 10; i_c++) dInOutRev[i_c] = 0;
}

TMdPrm::~TMdPrm( )
{
    disable();
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    if(!da) throw TError(nodePath().c_str(), _("No a device selected."));

    TParamContr::enable();

    wTm = vmin(25.5,vmax(0,atof(modPrm("wTm").c_str())));
    acq_err = "";

    vector<string> als;
    da->enable(this, als);

    //> Check for delete DAQ parameter's attributes
    for(int i_p = 0; i_p < (int)p_el.fldSize(); i_p++)
    {
	unsigned i_l;
	for(i_l = 0; i_l < als.size(); i_l++)
	    if(p_el.fldAt(i_p).name() == als[i_l])
		break;
	if(i_l >= als.size())
	    try{ p_el.fldDel(i_p); i_p--; }
	    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    owner().prmEn(id(), true);
}

void TMdPrm::disable()
{
    if(!enableStat())  return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);

    //> Stop fast task
    if(prcSt) SYS->taskDestroy(nodePath('.',true), &endRunReq);

    if(da) da->disable(this);
}

void TMdPrm::getVals( )
{
    if(da) da->getVal(this);
}

string TMdPrm::modPrm( const string &prm, const string &def )
{
    string rez;
    XMLNode prmNd;
    try
    {
	prmNd.load(cfg("MOD_PRMS").getS());
	string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
	if(!sa.size())	return (rez=prmNd.attr(prm)).empty()?def:rez;
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
    try { prmNd.load(cfg("MOD_PRMS").getS()); } catch(...){ }

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

    cfg("MOD_PRMS").setS(prmNd.save(XMLNode::BrAllPast));
}

bool TMdPrm::cfgChange( TCfg &co, const TVariant &pc )
{
    TParamContr::cfgChange(co, pc);

    if((co.name() == "MOD_TP" || co.name() == "MOD_ADDR" || co.name() == "MOD_SLOT") && enableStat()) disable();

    if(co.name() == "MOD_TP")	da = mod->daGet(this);

    return true;
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat())
    {
	if(val.name() == "err")
	{
	    if(!enableStat())			val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition is stopped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

    if(owner().redntUse()) return;

    if(val.name() == "err")
    {
	if(acq_err.getVal().empty())	val.setS("0",0,true);
	else				val.setS(acq_err.getVal(),0,true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setI(EVAL_INT, 0, true); return; }

    //> Send to active reserve station
    if(owner().redntUse())
    {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }
    //> Direct write
    try { if(da) da->vlSet(this, vo, vl, pvl); }
    catch(TError err)
    {
	mess_err(nodePath().c_str(),_("Write value to attribute '%s' error: %s"),vo.name().c_str(),err.mess.c_str());
	vo.setS(pvl.getS(), 0, true);
    }
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
    string rez;

    //Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/MOD_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_TP",EVAL_STR,(enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID,2,
	    "dest","select","select","/prm/cfg/modLst");
	ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_SLOT",EVAL_STR,(enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_ADDR",EVAL_STR,(enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID);
	if(owner().mBus != 0) ctrRemoveNode(opt, "/prm/cfg/MOD_SLOT");
	if(owner().mBus <= 0) ctrRemoveNode(opt, "/prm/cfg/MOD_ADDR");
	if(da) da->cntrCmdProc(this,opt);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/modLst" && ctrChkNode(opt))
    {
	vector<string> tid, tnm;
	mod->daTpList(this, tid, &tnm);
	opt->childAdd("el")->setAttr("id","-")->setText(_("<No select>"));
	for(int i_t = 0; i_t < vmin(tid.size(),tnm.size()); i_t++)
	    opt->childAdd("el")->setAttr("id",tid[i_t])->setText(tnm[i_t]);
    }
    else if(da && da->cntrCmdProc(this,opt)) ;
    else TParamContr::cntrCmdProc(opt);
}

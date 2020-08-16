
//OpenSCADA module DAQ.LogicLev file: logiclev.cpp
/***************************************************************************
 *   Copyright (C) 2006-2020 by Roman Savochenko, <roman@oscada.org>       *
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
#include <algorithm>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include "logiclev.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"LogicLev"
#define MOD_NAME	_("Logical level")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"2.3.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides the pure logical level of the DAQ parameters.")
#define LICENSE		"GPL2"
//*************************************************

LogicLev::TTpContr *LogicLev::mod;	//Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_LogicLev_module( int nMod )
#else
    TModule::SAt module( int nMod )
#endif
    {
	if(nMod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_LogicLev_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new LogicLev::TTpContr(source);
	return NULL;
    }
}

using namespace LogicLev;

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

void TTpContr::load_( )
{
    //Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable( flag );

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameters table by template"),TFld::String,TFld::NoFlag,"40",""));
    fldAdd(new TFld("PRM_BD_REFL",_("Parameters table for reflection"),TFld::String,TFld::NoFlag,"50",""));
    fldAdd(new TFld("SCHEDULE",_("Schedule of the calculation"),TFld::String,TFld::NoFlag,"100", "1"));
    fldAdd(new TFld("PRIOR",_("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));

    //Parameter type bd structure
    // Standard parameter type by template
    int t_prm = tpParmAdd("std", "PRM_BD", _("Logical"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("PRM",_("Parameter template"),TFld::String,TCfg::NoVal,"100",""));
    //  Logical level parameter IO BD structure
    elPrmIO.fldAdd(new TFld("PRM_ID",_("Parameter ID"),TFld::String,TCfg::Key,i2s(s2i(OBJ_ID_SZ)*6).c_str()));
    elPrmIO.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,i2s(s2i(OBJ_ID_SZ)*1.5).c_str()));
    elPrmIO.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::TransltText,"1000000"));

    // A parameter direct reflection
    t_prm = tpParmAdd("pRefl", "PRM_BD_REFL", _("Parameter reflection"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("PSRC",_("Source parameter"),TFld::String,TCfg::NoVal,"100",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name,daq_db,this); }

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) : ::TController(name_c,daq_db,cfgelem), enRes(true),
    mPrior(cfg("PRIOR").getId()), prcSt(false), callSt(false), endrunReq(false), mPer(1e9)
{
    cfg("PRM_BD").setS("LogLevPrm_"+name_c);
    cfg("PRM_BD_REFL").setS("LogLevPrmRefl_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

void TMdContr::postDisable(int flag)
{
    TController::postDisable(flag);
    try {
	if(flag) {
	    //Delete parameter's io table
	    string tbl = DB()+"."+cfg("PRM_BD").getS()+"_io";
	    SYS->db().at().open(tbl);
	    SYS->db().at().close(tbl, true);
	}
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if(startStat() && !redntUse()) {
	if(callSt)	rez += TSYS::strMess(_("Calculation. "));
	if(period())	rez += TSYS::strMess(_("Calculation with the period: %s. "), tm2s(1e-9*period()).c_str());
	else rez += TSYS::strMess(_("Next calculation by the cron '%s'. "), atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s[%s]. "),
	    tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str());
    }
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(SYS->cfgCtx() && toEnable() && !enableStat())	enable();
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
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE")
	mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,1e9*s2r(cron())) : 0;

    return true;
}

void TMdContr::prmEn( TMdPrm *p, bool val )
{
    MtxAlloc res(enRes, true);

    unsigned iPrm;
    for(iPrm = 0; iPrm < pHd.size(); iPrm++)
	if(&pHd[iPrm].at() == p) break;

    if(val && iPrm >= pHd.size())	pHd.push_back(p);
    if(!val && iPrm < pHd.size())	pHd.erase(pHd.begin()+iPrm);
}

void *TMdContr::Task( void *icntr )
{
    const TSYS::STask &tsk = TSYS::taskDescr();
    TMdContr &cntr = *(TMdContr*)icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    bool isStart = true;
    bool isStop  = false;
    int64_t tCnt1 = 0, tCnt2 = 0;

    while(true) {
	//Update controller's data
	if(!cntr.redntUse()) {
	    if(cntr.messLev() == TMess::Debug) tCnt1 = TSYS::curTime();
	    cntr.enRes.lock();
	    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		try {
		    cntr.pHd[iP].at().calc(isStart, isStop, tsk.period()?(1/tsk.period()):(cntr.period()?1e9/cntr.period():1));
		    if(cntr.messLev() == TMess::Debug) {
			tCnt2 = TSYS::curTime();
			cntr.pHd[iP].at().tmCalc = 1e-6*(tCnt2-tCnt1);
			cntr.pHd[iP].at().tmCalcMax = vmax(cntr.pHd[iP].at().tmCalcMax, cntr.pHd[iP].at().tmCalc);
			tCnt1 = tCnt2;
		    }
		} catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	    isStart = false;
	    cntr.enRes.unlock();
	}

	if(isStop) break;
	TSYS::taskSleep((int64_t)cntr.period(), cntr.period() ? "" : cntr.cron());
	if(cntr.endrunReq)	isStop = true;
    }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::redntDataUpdate( )
{
    TController::redntDataUpdate();

    vector<RedntStkEl> hst;

    //Prepare a group of a hierarchy request to the parameters
    AutoHD<TParamContr> prm, prmC;
    XMLNode req("CntrReqs"); req.setAttr("path",nodePath(0,true));

    hst.push_back(RedntStkEl());
    list(hst.back().ls);
    string addr;
    while(true) {
	if(hst.back().pos >= hst.back().ls.size()) {
	    if(!hst.back().addr.size()) break;
	    hst.pop_back(); hst.back().pos++;
	    prm = AutoHD<TParamContr>(hst.back().addr.size()?dynamic_cast<TParamContr*>(prm.at().nodePrev(true)):NULL);
	    continue;
	}
	prmC = prm.freeStat() ? TController::at(hst.back().ls[hst.back().pos]) : prm.at().at(hst.back().ls[hst.back().pos]);
	addr = hst.back().addr + "/prm_"+hst.back().ls[hst.back().pos];
	if(prmC.at().enableStat()) req.childAdd("get")->setAttr("path", addr + "/%2fserv%2ftmplAttr");
	hst.push_back(RedntStkEl(addr));
	prmC.at().list(hst.back().ls);
	prm = prmC;
    }

    //Send request to first active station for this controller
    if(owner().owner().rdStRequest(workId(),req).empty()) return;

    //Redirect respond to the local parameters
    req.setAttr("path", "/");
    for(unsigned iPrm = 0; iPrm < req.childSize(); ) {
	if(s2i(req.childGet(iPrm)->attr("err"))) { req.childDel(iPrm); continue; }
	req.childGet(iPrm)->setName("set")->setAttr("reforwardRedundOff", "1");
	iPrm++;
    }
    cntrCmd(&req);
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),/*startStat()?R_R_R_:*/RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	return;
    }
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) :
    TParamContr(name,tp_prm), tmCalc(0), tmCalcMax(0), prmRefl(NULL), pEl("w_attr"), chkLnkNeed(false),
    idFreq(-1), idStart(-1), idStop(-1), idErr(-1), idSh(-1), idNm(-1), idDscr(-1)
{
    setType(type().name);
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

TCntrNode &TMdPrm::operator=( const TCntrNode &node )
{
    TParamContr::operator=(node);

    const TMdPrm *src_n = dynamic_cast<const TMdPrm*>(&node);
    if(!src_n || !src_n->enableStat() || !enableStat() || !isStd() || !tmpl->func()) return *this;

    //IO values copy
    for(int iIO = 0; iIO < src_n->tmpl->func()->ioSize(); iIO++)
	if(src_n->tmpl->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
	    tmpl->lnkAddrSet(iIO, src_n->tmpl->lnkAddr(iIO));
	else tmpl->setS(iIO, src_n->tmpl->getS(iIO));

    // Init links
    chkLnkNeed = tmpl->initLnks();

    return *this;
}

bool TMdPrm::isStd( ) const	{ return (type().name == "std"); }

bool TMdPrm::isPRefl( ) const	{ return (type().name == "pRefl"); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&pEl)) vlElemAtt(&pEl);
}

void TMdPrm::postDisable( int flag )
{
    TParamContr::postDisable(flag);

    if(flag) {
	string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";
	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(ownerPath(true), true);
	SYS->db().at().dataDel(io_bd, owner().owner().nodePath()+type().DB(&owner())+"_io", cfg);
    }
}

void TMdPrm::setType( const string &tpId )
{
    //Free old mode
    if(isPRefl() && prmRefl)	{ delete prmRefl; prmRefl = NULL; }
    else if(isStd() && tmpl)	{ delete tmpl; tmpl = NULL; }

    TParamContr::setType(tpId);

    //Init/update new mode
    if(isPRefl() && !prmRefl)	prmRefl = new AutoHD<TValue>;
    else if(isStd() && !tmpl)	tmpl = new TPrmTempl::Impl(this, id()+"_LogicLevPrm");
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    tmCalc = tmCalcMax = 0;
    bool isProc = false, isFullEn = !enableStat();
    if(isFullEn) TParamContr::enable();

    vector<string> als;

    try {
	if(isPRefl()) {
	    vector<string> list;
	    if(prmRefl->freeStat()) *prmRefl = SYS->daq().at().prmAt(cfg("PSRC").getS(), '.', true);
	    if(!prmRefl->freeStat()) {
		prmRefl->at().vlList(list);
		for(unsigned iL = 0; iL < list.size(); iL++) {
		    if(!vlPresent(list[iL]))
			pEl.fldAdd(new TFld(list[iL].c_str(),prmRefl->at().vlAt(list[iL]).at().fld().descr().c_str(),
			    prmRefl->at().vlAt(list[iL]).at().fld().type(),
			    TVal::DirWrite|TVal::DirRead|(prmRefl->at().vlAt(list[iL]).at().fld().flg()&TFld::NoWrite)));
		    als.push_back(list[iL]);
		}

		isProc = true;
	    }
	}
	else if(isStd() && !tmpl->func()) {
	    //unsigned fId = 0;
	    string prm = cfg("PRM").getS();
	    if(!tmpl->func() && prm.size()) {
		tmpl->setFunc(&SYS->daq().at().tmplLibAt(TSYS::strSepParse(prm,0,'.')).at().
					       at(TSYS::strSepParse(prm,1,'.')).at().func().at());
		tmpl->setVfName(id()+"_tmplprm");
	    }
	    // Init attrubutes
	    if(tmpl->func()) {
		tmpl->addLinksAttrs(&pEl);

		chkLnkNeed = tmpl->initLnks();

		//Load IO
		loadIO(true);

		//Init system attributes identifiers
		idFreq	= tmpl->ioId("f_frq");
		idStart	= tmpl->ioId("f_start");
		idStop	= tmpl->ioId("f_stop");
		idErr	= tmpl->ioId("f_err");
		idSh	= tmpl->ioId("SHIFR");
		idNm	= tmpl->ioId("NAME");
		idDscr	= tmpl->ioId("DESCR");
		int idThis = tmpl->ioId("this");
		if(idThis >= 0) tmpl->setO(idThis, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));

		isProc = true;
	    }
	}
    } catch(...) { disable(); throw; }

    //Check for delete DAQ parameter's attributes
    for(int iP = 0; isPRefl() && isProc && iP < (int)pEl.fldSize(); iP++) {
	unsigned iL;
	for(iL = 0; iL < als.size(); iL++)
	    if(pEl.fldAt(iP).name() == als[iL])
		break;
	if(iL >= als.size())
	    try{ pEl.fldDel(iP); iP--; }
	    catch(TError &err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    if(isStd() && isFullEn && owner().startStat()) calc(true, false, 0);
    if(isFullEn) owner().prmEn(this, true);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(this, false);

    if(isPRefl() && prmRefl) prmRefl->free();
    else if(isStd() && tmpl) {
	if(owner().startStat()) calc(false, true, 0);
	tmpl->cleanLnks(true);
    }

    idFreq = idStart = idStop = idErr = -1;

    TParamContr::disable();
}

void TMdPrm::load_( )
{
    if(SYS->cfgCtx() && toEnable() && !enableStat())	enable();
    if(enableStat()) loadIO();
}

void TMdPrm::loadIO( bool force )
{
    //Load IO and init links
    if(isStd() && tmpl->func()) {
	//if(owner().startStat() && !force) { modif(true); return; }	//Load/reload IO context only allow for stoped controlers for prevent throws

	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(ownerPath(true), TCfg::ForceUse);
	cfg.cfg("VALUE").setExtVal(true);
	string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";

	//IO values loading and links set, by seek
	for(int fldCnt = 0; SYS->db().at().dataSeek(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",fldCnt++,cfg,false,true); ) {
	    int iIO = tmpl->func()->ioId(cfg.cfg("ID").getS());
	    if(iIO < 0) continue;
	    if(tmpl->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
		tmpl->lnkAddrSet(iIO, cfg.cfg("VALUE").getS(TCfg::ExtValOne));	//Force no translated
	    else if(tmpl->func()->io(iIO)->type() != IO::String)
		tmpl->setS(iIO, cfg.cfg("VALUE").getS(TCfg::ExtValOne));	//Force no translated
	    else tmpl->setS(iIO, cfg.cfg("VALUE").getS());
	}
	chkLnkNeed = tmpl->initLnks();
    }
}

void TMdPrm::save_( )
{
    TParamContr::save_();

    if(enableStat()) saveIO();
}

void TMdPrm::saveIO( )
{
    //Save IO and init links
    if(isStd() && tmpl->func()) {
	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(ownerPath(true));
	string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";

	for(int iIO = 0; iIO < tmpl->func()->ioSize(); iIO++) {
	    cfg.cfg("ID").setS(tmpl->func()->io(iIO)->id());
	    cfg.cfg("VALUE").setNoTransl(!(tmpl->func()->io(iIO)->type()==IO::String && !(tmpl->func()->io(iIO)->flg()&TPrmTempl::CfgLink)));
	    if(tmpl->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
		cfg.cfg("VALUE").setS(tmpl->lnkAddr(iIO));
	    else cfg.cfg("VALUE").setS(tmpl->getS(iIO));
	    SYS->db().at().dataSet(io_bd, owner().owner().nodePath()+type().DB(&owner())+"_io",cfg);
	}
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat()) {
	if(val.name() == "err") {
	    if(!enableStat()) val.setS(_("1:Parameter disabled."), 0, true);
	    else if(!owner().startStat()) val.setS(_("2:Calculation stopped."), 0, true);
	}
	else val.setS(EVAL_STR, 0, true);
	return;
    }

    if(owner().redntUse()) return;

    if(val.name() != "err") {
	try {
	    if(isPRefl() && !prmRefl->freeStat()) val.set(prmRefl->at().vlAt(val.name()).at().get(), 0, true);
	    else if(isStd() && tmpl->func() && (idErr < 0 || tmpl->getS(idErr) != EVAL_STR)) {
		int id_lnk = tmpl->lnkId(val.name());
		if(id_lnk >= 0 && !tmpl->lnkActive(id_lnk)) id_lnk = -1;
		if(id_lnk < 0) val.set(tmpl->get(tmpl->ioId(val.name())), 0, true);
		else val.set(tmpl->lnkInput(id_lnk), 0, true);
	    }
	} catch(TError &err) { }
    }
    else {
	if(isStd() && tmpl->func() && idErr >= 0) {
	    if(tmpl->getS(idErr) != EVAL_STR) val.setS(tmpl->getS(idErr), 0, true);
	} else val.setS("0", 0, true);
	if(owner().messLev() == TMess::Debug && (idErr < 0 || tmpl->getS(idErr) != EVAL_STR))
	    val.setS(val.getS(NULL,true)+": "+TSYS::strMess(_("Spent time %s[%s]"),tm2s(tmCalc).c_str(),tm2s(tmCalcMax).c_str()), 0, true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setS(EVAL_STR, 0, true); return; }

    //Send to active reserve station
    if(vlSetRednt(vo,vl,pvl))	return;

    //Direct write
    try {
	if(isPRefl() && !prmRefl->freeStat()) prmRefl->at().vlAt(vo.name()).at().set(vl);
	else if(isStd() && tmpl->func()) {
	    int id_lnk = tmpl->lnkId(vo.name());
	    if(id_lnk >= 0 && !tmpl->lnkActive(id_lnk)) id_lnk = -1;
	    ResAlloc cres(calcRes, true);
	    if(id_lnk < 0) tmpl->set(tmpl->ioId(vo.name()), vl);
	    else tmpl->lnkOutput(id_lnk, vl);
	}
    } catch(TError &err) { }
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr);
    val.arch().at().setPeriod(SYS->archive().at().valPeriod()*1000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

TVariant TMdPrm::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //bool attrAdd( string id, string name, string tp = "real", string selValsNms = "" ) - attribute <id> and <name> for type <tp> add.
    //  id, name - new attribute id and name;
    //  tp - attribute type [boolean | integer | real | string | text | object] + selection mode [sel | seled] + read only [ro];
    //  selValsNms - two lines with values in first and it's names in first (separated by ";").
    if(iid == "attrAdd" && prms.size() >= 1) {
	if(!enableStat() || !isStd())	return false;
	TFld::Type tp = TFld::Real;
	string stp, stp_ = (prms.size() >= 3) ? prms[2].getS() : "real";
	stp.resize(stp_.length());
	std::transform(stp_.begin(), stp_.end(), stp.begin(), ::tolower);
	if(stp.find("boolean") != string::npos)		tp = TFld::Boolean;
	else if(stp.find("integer") != string::npos)	tp = TFld::Integer;
	else if(stp.find("real") != string::npos)	tp = TFld::Real;
	else if(stp.find("string") != string::npos ||
		stp.find("text") != string::npos)	tp = TFld::String;
	else if(stp.find("object") != string::npos)	tp = TFld::Object;

	unsigned flg = TVal::Dynamic;
	if(stp.find("sel") != string::npos)	flg |= TFld::Selectable;
	if(stp.find("seled") != string::npos)	flg |= TFld::SelEdit;
	if(stp.find("text") != string::npos)	flg |= TFld::FullText;
	if(stp.find("ro") != string::npos)	flg |= TFld::NoWrite;

	string	sVals = (prms.size() >= 4) ? prms[3].getS() : "";
	string	sNms = TSYS::strLine(sVals, 1);
	sVals = TSYS::strLine(sVals, 0);

	MtxAlloc res(pEl.resEl(), true);
	unsigned aId = pEl.fldId(prms[0].getS(), true);
	if(aId < pEl.fldSize()) {
	    if(prms.size() >= 2 && prms[1].getS().size()) pEl.fldAt(aId).setDescr(prms[1].getS());
	    pEl.fldAt(aId).setFlg(pEl.fldAt(aId).flg()^((pEl.fldAt(aId).flg()^flg)&(TFld::Selectable|TFld::SelEdit|TFld::FullText|TFld::NoWrite)));
	    pEl.fldAt(aId).setValues(sVals);
	    pEl.fldAt(aId).setSelNames(sNms);
	    pEl.fldAt(aId).setLen(SYS->sysTm());
	}
	else if(!vlPresent(prms[0].getS()))
	    pEl.fldAdd(new TFld(prms[0].getS().c_str(),prms[(prms.size()>=2)?1:0].getS().c_str(),tp,flg,
				i2s(SYS->sysTm()).c_str(),"",sVals.c_str(),sNms.c_str()));
	return true;
    }
    //bool attrDel( string id ) - attribute <id> remove.
    if(iid == "attrDel" && prms.size() >= 1) {
	if(!enableStat() || !isStd())	return false;
	MtxAlloc res(pEl.resEl(), true);
	unsigned aId = pEl.fldId(prms[0].getS(), true);
	if(aId == pEl.fldSize())	return false;
	try { pEl.fldDel(aId); } catch(TError&) { return false; }
	return true;
    }

    return TParamContr::objFuncCall(iid, prms, user);
}

void TMdPrm::calc( bool first, bool last, double frq )
{
    if(isPRefl() && (!first || prmRefl->freeStat())) enable();

    if(!isStd() || !tmpl->func()) return;
    try {
	ResAlloc cres(calcRes, true);
	if(chkLnkNeed) chkLnkNeed = tmpl->initLnks();

	//Set fixed system attributes
	if(idFreq >= 0)	tmpl->setR(idFreq, frq);
	if(idStart >= 0)tmpl->setB(idStart, tmpl->isChangedProg(true) || first);
	if(idStop >= 0)	tmpl->setB(idStop, last);
	if(idSh >= 0)	tmpl->setS(idSh, id());
	if(idNm >= 0)	tmpl->setS(idNm, name());
	if(idDscr >= 0)	tmpl->setS(idDscr, descr());

	//Get input links
	tmpl->inputLinks();

	//Calc template
	tmpl->setMdfChk(true);
	tmpl->calc();
	if(SYS->modifCalc()) modif();

	//Put output links
	tmpl->outputLinks();

	//Put fixed system attributes
	if(idNm >= 0 && tmpl->ioMdf(idNm))	setName(tmpl->getS(idNm));
	if(idDscr >= 0 && tmpl->ioMdf(idDscr))	setDescr(tmpl->getS(idDscr));
    } catch(TError &err) {
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Error calculating template."));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/") {
	if(a_path == "/serv/tmplAttr") {
	    if(!isStd() || !tmpl->func()) throw TError(nodePath().c_str(),_("Error or non-template parameter."));
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
		for(int iA = 0; iA < tmpl->ioSize(); iA++)
		    if(iA != idFreq && iA != idStart && iA != idStop /*&& iA != idErr*/ && iA != idSh && iA != idNm && iA != idDscr &&
			    tmpl->func()->io(iA)->id() != "this")
			opt->childAdd("ta")->setAttr("id",tmpl->func()->io(iA)->id())->setText(tmpl->getS(iA));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		for(unsigned iA = 0; iA < opt->childSize(); iA++) {
		    int ioId = -1;
		    if(opt->childGet(iA)->name() != "ta" || (ioId=tmpl->ioId(opt->childGet(iA)->attr("id"))) < 0) continue;
		    tmpl->setS(ioId, opt->childGet(iA)->text());
		}
	}
	else TParamContr::cntrCmdProc(opt);
	return;
    }

    vector<string> list;
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	if(isPRefl()) ctrMkNode("fld",opt,-1,"/prm/cfg/PSRC",cfg("PSRC").fld().descr(),RWRW__,"root",SDAQ_ID,3,
				"tp","str", "dest","sel_ed", "select","/prm/cfg/prmp_lst");
	else if(isStd()) ctrMkNode("fld",opt,-1,"/prm/cfg/PRM",cfg("PRM").fld().descr(),RWRW__,"root",SDAQ_ID,3,
				    "tp","str", "dest","select", "select","/prm/tmplList");
	if(isStd() && tmpl->func())	tmpl->TPrmTempl::Impl::cntrCmdProc(opt);
	return;
    }

    //Process command to page
    if(isPRefl() && a_path == "/prm/cfg/PSRC") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) {
	    string prmVal = cfg("PSRC").getS();
	    if(!SYS->daq().at().prmAt(TSYS::strParse(prmVal,0,"#"),'.',true).freeStat()) prmVal += " (+)";
	    opt->setText(prmVal);
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	    try {
		if(enableStat()) disable();
		string prmValm = TSYS::strParse(opt->text(), 0, " ");
		//Link to self exclude
		if(prmValm == owner().owner().modId()+"."+owner().id()+"."+id())
		    prmValm = owner().owner().modId()+"."+owner().id();
		cfg("PSRC").setS(prmValm);
	    } catch(...) { disable(); throw; }
    }
    else if(isStd() && a_path == "/prm/cfg/PRM") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD))	opt->setText(cfg("PRM").getS());
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	    try {
		if(enableStat()) disable();
		cfg("PRM").setS(opt->text());
	    } catch(...) { disable(); throw; }
    }
    else if(a_path == "/prm/cfg/prmp_lst" && ctrChkNode(opt)) SYS->daq().at().ctrListPrmAttr(opt, cfg("PSRC").getS(), true, '.');
    else if(isStd() && tmpl->func() && tmpl->TPrmTempl::Impl::cntrCmdProc(opt))	;
    else TParamContr::cntrCmdProc(opt);
}

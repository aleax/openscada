
//OpenSCADA system module DAQ.OneWire file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <ttiparam.h>
#include <tdaqs.h>

#include <owcapi.h>

#include "module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"OneWire"
#define MOD_NAME	_("1Wire devices")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.1.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("1Wire devices support by OWFS (http://owfs.org).")
#define LICENSE		"GPL2"
//*************************************************

ModOneWire::TTpContr *ModOneWire::mod;  //Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_OneWire_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_OneWire_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new ModOneWire::TTpContr(source);
	return NULL;
    }
}

using namespace ModOneWire;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID), OW_initOK(false)
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
    setOWFSPrms("");	//For free OWFS resources
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("BUS",_("1Wire bus"),TFld::Integer,TFld::NoFlag,"2","-1","-1;99"));
    fldAdd(new TFld("Simult",_("Simultaneous temperature request"),TFld::Real,TCfg::NoVal,"3.2","0"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV",_("Device"),TFld::String,TCfg::NoVal,"16",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db ) { return new TMdContr(name, daq_db, this); }

void TTpContr::load_( )
{
    //Load parameters from config-file
    setOWFSPrms(TBDS::genDBGet(nodePath()+"OWFSPrms",OWFSPrms()));
}

void TTpContr::save_( )
{
    //Save parameters to config-file
    TBDS::genDBSet(nodePath()+"OWFSPrms",OWFSPrms());
}

void TTpContr::setOWFSPrms( const string &vl )
{
    mOWFSPrms = vl;
    modif();

    ResAlloc res(OWFSRes, true);
    if(OW_initOK) { OW_finish(); OW_initOK = false; }
    if(vl.size()) {
	optind = opterr = 0;	//Global optget variables clear
	OW_initOK = (OW_init(vl.c_str())==0);
    }
}

string TTpContr::OWFSGet( const string &path )
{
    ResAlloc res(OWFSRes, true);
    if(!OW_initOK) return "";

    string rez;
    char *buf;
    size_t s;
    if(OW_get(path.c_str(),&buf,&s) >= 0) {
	rez = buf;
	free(buf);
    }

    return rez;
}

int TTpContr::OWFSPut( const string &path, const string &val )
{
    ResAlloc res(OWFSRes, true);
    if(!OW_initOK) return -1;

    int rez = OW_put(path.c_str(),val.data(),val.size());
    return rez;
}

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTipDAQ::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm","OWFS")) {
	    ctrMkNode("fld",opt,-1,"/prm/OW_initOK",_("OWFS OK"),R_R_R_,"root",SDAQ_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/prm/OWFSPrms",_("OWFS parameters"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/OW_initOK" && ctrChkNode(opt,"get")) opt->setText(OW_initOK?"1":"0");
    else if(a_path == "/prm/OWFSPrms") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(OWFSPrms());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setOWFSPrms(opt->text());
    }
    else TTipDAQ::cntrCmdProc(opt);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem ) : TController(name_c,daq_db,cfgelem),
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mBus(cfg("BUS")), mSimult(cfg("Simult")),
    prcSt(false), callSt(false), tmGath(0)
{
    cfg("PRM_BD").setS("OneWirePrm_"+name_c);
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
	if(period())	rez += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-3*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "),tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s."),tm2s(tmGath).c_str());
    }

    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type ) { return new TMdPrm(name, &owner().tpPrmAt(type)); }

void TMdContr::enable_( )
{
    if(OWFSBusLs().find(i2s(bus())+";") == string::npos)
	throw TError(nodePath().c_str(),_("Bus error or is not selected."));
}

void TMdContr::start_( )
{
    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true),NULL,10,true);

    //Set EVal
    ResAlloc res(en_res, false);
    for(unsigned i_p = 0; i_p < p_hd.size(); i_p++) p_hd[i_p].at().setEval();
}

void TMdContr::prmEn( const string &id, bool val )
{
    unsigned i_prm;

    ResAlloc res(en_res, true);
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

string TMdContr::OWFSBusLs( )
{
    if(enableStat()) return "";
    string rez, curVl, fsLs = mod->OWFSGet("/");
    for(int off = 0; (curVl=TSYS::strParse(fsLs,0,",",&off)).size(); )
	if(curVl.compare(0,4,"bus.") == 0)
	    rez += i2s(atoi(curVl.c_str()+4))+";";

    return rez;
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.prcSt = true;

    while(!TSYS::taskEndRun()) {
	int64_t t_cnt = TSYS::curTime();

	//Write simultaneous/temperature for all temperature chips prepare
	if(cntr.mSimult.getR() > 0.1) {
	    mod->OWFSPut("/bus."+i2s(cntr.bus())+"/simultaneous/temperature","1");
	    TSYS::sysSleep(cntr.mSimult.getR());
	}

	//Update controller's data
	cntr.en_res.resRequestR( );
	cntr.callSt = true;
	for(unsigned i_p = 0; i_p < cntr.p_hd.size() && !cntr.redntUse() && !TSYS::taskEndRun(); i_p++)
	    try { cntr.p_hd[i_p].at().getVals(); }
	    catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	cntr.callSt = false;
	cntr.en_res.resRelease();
	cntr.tmGath = TSYS::curTime()-t_cnt;

	if(TSYS::taskEndRun()) break;
	TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
    }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",mSched.fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",mPrior.fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/BUS",mBus.fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","select","sel_list",OWFSBusLs().c_str(),"help",_("1Wire bus number according OWFS order configuration."));
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    /*if(a_path == "/cntr/cfg/BUSLst" && ctrChkNode(opt,"get")) opt->setText(OWFSBusLs());
    else */
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), mDev(cfg("DEV"))
{

}

TMdPrm::~TMdPrm( )
{
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
    TParamContr::enable();

    vector<string> als;

    //Get device's files list
    string rez, curVl, fsLs = mod->OWFSGet("/bus."+i2s(owner().bus())+"/"+dev());
    for(int off = 0; (curVl=TSYS::strParse(fsLs,0,",",&off)).size(); ) {
	if(curVl == "address")		p_el.fldAdd(new TFld(curVl.c_str(),_("Address"),TFld::String,TFld::NoWrite));
	else if(curVl == "type")	p_el.fldAdd(new TFld(curVl.c_str(),_("Type"),TFld::String,TFld::NoWrite));
	else if(curVl == "power")	p_el.fldAdd(new TFld(curVl.c_str(),_("Power"),TFld::Boolean,TFld::NoWrite));
	else if(curVl == "temperature")	p_el.fldAdd(new TFld(curVl.c_str(),_("Temperature"),TFld::Real,TFld::NoWrite));
	else continue;
	als.push_back(curVl);
    }

    //Check for delete DAQ parameter's attributes
    for(int i_p = 0; i_p < (int)p_el.fldSize(); i_p++) {
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

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    setEval();
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

string TMdPrm::OWFSDevLs( )
{
    if(owner().bus() < 0 || enableStat()) return "";
    string rez, curVl, fsLs = mod->OWFSGet("/bus."+i2s(owner().bus())+"/");
    for(int off = 0; (curVl=TSYS::strParse(fsLs,0,",",&off)).size(); )
	if(isdigit(curVl[0]))
	    rez += curVl.substr(0,curVl.size()-1)+";";

    return rez;
}

void TMdPrm::getVals( )
{
    vector<string> als;
    vlList(als);
    string tvl;

    for(unsigned i_a = 0; i_a < als.size(); i_a++) {
	AutoHD<TVal> val = vlAt(als[i_a]);
	if((val.at().get().isEVal() && (als[i_a] == "address" || als[i_a] == "type" || als[i_a] == "power")) ||
	    als[i_a] == "temperature")
	{
	    tvl = mod->OWFSGet("/bus."+i2s(owner().bus())+"/"+dev()+"/"+als[i_a]);
	    val.at().setS(tvl.size()?tvl:EVAL_STR, 0, true);
	}
    }
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
    if(a_path.compare(0,6,"/serv/") == 0)	{ TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/DEV",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","select","sel_list",OWFSDevLs().c_str(),"help",_("Device code on controller's bus according to 1Wire IC."));
	return;
    }

    //Process command to page
    /*if(a_path == "/prm/cfg/DEVLst" && ctrChkNode(opt,"get")) opt->setText(OWFSDevLs());
    else */
    TParamContr::cntrCmdProc(opt);
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


//OpenSCADA system module DAQ.Comedi file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012 by Roman Savochenko                                *
 *   rom_as@oscada.org                                                     *
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

#include <getopt.h>
#include <unistd.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Comedi"
#define MOD_NAME	_("DAQ boards by Comedi")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.1.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("ISA, PCI, PCMCIA, USB DAQ boards collection by Comedi(http://www.comedi.org).")
#define LICENSE		"GPL2"
//*************************************************

ModComedi::TTpContr *ModComedi::mod;  //Pointer for direct access to module

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
	    return new ModComedi::TTpContr(source);
	return NULL;
    }
}

using namespace ModComedi;

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

TTpContr::~TTpContr()
{

}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TTpContr::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTpContr::save_( )
{

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PERIOD",_("Gather data period (s)"),TFld::Integer,TFld::NoFlag,"3","1","0;100"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("ADDR",_("Board's device address)"),TFld::String,TCfg::NoVal,"100",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0),
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId())
{
    cfg("PRM_BD").setS("ComediPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if(startStat() && !redntUse()) rez += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    //> Start the gathering data task
    if(!prc_st) SYS->taskCreate(nodePath('.',true), m_prior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.',true), &endrun_req);
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
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

    while( !cntr.endrun_req )
    {
	int64_t t_cnt = TSYS::curTime();

	//> Update controller's data
	cntr.en_res.resRequestR( );
	for( unsigned i_p=0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++ )
	    try
	    {
		//!!! Process parameter code
	    }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resRelease( );
	cntr.tm_gath = 1e-3*(TSYS::curTime()-t_cnt);

	TSYS::taskSleep((int64_t)(1e9*cntr.period()));
    }

    cntr.prc_st = false;

    return NULL;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), devH(NULL)
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if(enableStat()) return;

    TParamContr::enable();

    devH = comedi_open(cfg("ADDR").getS().c_str());
    if(!devH)	throw TError(nodePath().c_str(), "%s", comedi_strerror(comedi_errno()));

    owner().prmEn(id(), true);
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
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);

    if(devH) comedi_close(devH);
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",cfg("OID_LS").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }

    //> Process command to page
    if(a_path == "/prm/cfg/OID_LS" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
    {
	if(enableStat())	throw TError(nodePath().c_str(),"Parameter is enabled.");
//	parseOIDList(opt->text());
    }
    else TParamContr::cntrCmdProc(opt);
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

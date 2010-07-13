
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module DAQ.Tmpl file: mod_tmpl.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

//!!! System's includings. Add need for your module includings.
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

//!!! Self your module's includings. Add need for your module includings.
#include "mod_tmpl.h"

//!!! Module's meta-information. Change for your module.
//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Tmpl"
#define MOD_NAME	"DAQ template"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.0.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"DAQ's subsystem template module."
#define LICENSE		"GPL"
//*************************************************

ModTmpl::TTpContr *ModTmpl::mod;  //Pointer for direct access to module

//!!! Required section for binding OpenSCADA core to this module. It give information and create module root object.
//!!! Not remove this section!
extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new ModTmpl::TTpContr( source );
	return NULL;
    }
}

//!!! Include for default enter to your module namespace.
using namespace ModTmpl;

//*************************************************
//* TTpContr                                      *
//*************************************************
//!!! Constructor for Root module object.
TTpContr::TTpContr( string name )
{
    //!!! Load module meta-information to root object. Don't change it!
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //!!! Init shortcut to module root object. Don't change it!
    mod		= this;
}

//!!! Destructor for Root module object.
TTpContr::~TTpContr()
{

}

//!!! Module's comandline options for print help function.
string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

//!!! Processing virtual function for load Root module to DB
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

//!!! Processing virtual function for save Root module to DB
void TTpContr::save_( )
{

}

//!!! Post-enable processing virtual function
void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Integer,TFld::NoFlag,"3","1","0;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100","") );
}

//!!! Processing virtual functions for self object-controller creation.
TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
//!!! Constructor for DAQ-subsystem controller object.
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0),
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId())
{
    cfg("PRM_BD").setS("TmplPrm_"+name_c);
}

//!!! Destructor for DAQ-subsystem controller object.
TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

//!!! Status processing function for DAQ-controllers
string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if( startStat() && !redntUse( ) ) rez += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    return rez;
}

//!!! Processing virtual functions for self object-parameter creation.
TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

//!!! Processing virtual functions for start DAQ-controller
void TMdContr::start_( )
{
    //> Start the gathering data task
    if( !prc_st ) SYS->taskCreate( nodePath('.',true), m_prior, TMdContr::Task, this, &prc_st );
}

//!!! Processing virtual functions for stop DAQ-controller
void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if( prc_st ) SYS->taskDestroy( nodePath('.',true), &prc_st, &endrun_req );
}

//!!! Parameters register function, on time it enable, for fast processing into background task.
void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )	p_hd.erase(p_hd.begin()+i_prm);
}

//!!! Background task's function for periodic data acquisition.
void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while( !cntr.endrun_req )
    {
	long long t_cnt = TSYS::curTime();

	//> Update controller's data
	//!!! Your code for gather data
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

	//!!! Wait for next iteration
	TSYS::taskSleep((long long)(1e9*cntr.period()));
    }

    cntr.prc_st = false;

    return NULL;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
//!!! Constructor for DAQ-subsystem parameter object.
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), p_el("w_attr")
{

}

//!!! Destructor for DAQ-subsystem parameter object.
TMdPrm::~TMdPrm( )
{
    //!!! Call for prevent access to data the object from included nodes on destruction.
    nodeDelAll();
}

//!!! Post-enable processing virtual function
void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

//!!! Direct link to parameter's owner controller
TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

//!!! Processing virtual functions for enable parameter
void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    owner().prmEn( id(), true );
}

//!!! Processing virtual functions for disable parameter
void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

//!!! Processing virtual functions for load parameter from DB
void TMdPrm::load_( )
{
    TParamContr::load_();
}

//!!! Processing virtual functions for save parameter to DB
void TMdPrm::save_( )
{
    TParamContr::save_();
}

//!!! Processing virtual function for OpenSCADA control interface comands
void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if( a_path.substr(0,6) == "/serv/" )  { TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if( opt->name() == "info" )	
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",cfg("OID_LS").fld().descr(),enableStat()?0444:0664);
	return;
    }

    //> Process command to page
    if( a_path == "/prm/cfg/OID_LS" && ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )
    {
	if( enableStat() )	throw TError(nodePath().c_str(),"Parameter is enabled.");
//	parseOIDList(opt->text());
    }
    else TParamContr::cntrCmdProc(opt);
}

//!!! Processing virtual function for setup archive's parameters which associated with the parameter on time archive creation
void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

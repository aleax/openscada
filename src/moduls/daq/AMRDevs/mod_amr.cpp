
//OpenSCADA system module DAQ.AMRDevs file: mod_amr.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
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

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "mod_amr.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"AMRDevs"
#define MOD_NAME	"AMR devices"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.0.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow access to automatic meter reading devices. Supported devices: "
#define LICENSE		"GPL"
//*************************************************

AMRDevs::TTpContr *AMRDevs::mod;  //Pointer for direct access to module

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
	    return new AMRDevs::TTpContr( source );
	return NULL;
    }
}

using namespace AMRDevs;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name )
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

TTpContr::~TTpContr()
{

}

void TTpContr::load_( )
{

}

void TTpContr::save_( )
{

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("SCHEDULE",_("Calc schedule"),TFld::String,TFld::NoFlag,"100","1") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );
    fldAdd( new TFld("BUS_ADDR",_("Bus transport's address"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("TM_REQ",_("Connection timeout (ms)"),TFld::Integer,TFld::NoFlag,"5","0") );
    fldAdd( new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"3","30") );
    fldAdd( new TFld("REQ_TRY",_("Request tries"),TFld::Integer,TFld::NoFlag,"1","3","1;10") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("DEV_TP",_("Device type"),TFld::String,TCfg::NoVal,"20") );
    tpPrmAt(t_prm).fldAdd( new TFld("DEV_ADDR",_("Device address"),TFld::String,TCfg::NoVal,"50") );
    tpPrmAt(t_prm).fldAdd( new TFld("DEV_PRMS",_("Device addon parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"1000") );
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
	mSched(cfg("SCHEDULE").getSd()), mPrior(cfg("PRIOR").getId()), mAddr(cfg("BUS_ADDR").getSd()),
	reqTm(cfg("TM_REQ").getId()), restTm(cfg("TM_REST").getId()), connTry(cfg("REQ_TRY").getId())
{
    cfg("PRM_BD").setS("AMRDevsPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );
    if( startStat() && !redntUse( ) )
    {
	if( period() ) val += TSYS::strMess(_("Call by period %g s. "),(1e-9*period()));
	else val += TSYS::strMess(_("Call by cron '%s'. "),cron().c_str());
	val += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    }
    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    if( prc_st ) return;

    SYS->transport().at().at("Serial").at().outAt(mAddr).at().start();

    //> Start the gathering data task
    SYS->taskCreate( nodePath('.',true), mPrior, TMdContr::Task, this, &prc_st );
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if( prc_st ) SYS->taskDestroy( nodePath('.',true), &prc_st, &endrun_req );
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while( !cntr.endrun_req )
    {
	long long t_cnt = TSYS::curTime();

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

	TSYS::taskSleep(cntr.period(),cntr.period()?0:TSYS::cron(cntr.cron()));
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/BUS_ADDR",cfg("BUS_ADDR").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/cntr/cfg/trLst");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),0664,"root","DAQ",4,"tp","str","dest","sel_ed",
	    "sel_list",_("1;1e-3;* * * * *;10 * * * *;10-20 2 */2 * *"),
	    "help",_("Schedule is writed in seconds periodic form or in standard Cron form.\n"
		    "Seconds form is one real number (1.5, 1e-3).\n"
		    "Cron it is standard form '* * * * *'. Where:\n"
		    "  - minutes (0-59);\n"
		    "  - hours (0-23);\n"
		    "  - days (1-31);\n"
		    "  - month (1-12);\n"
		    "  - week day (0[sunday]-6)."));
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/trLst" && ctrChkNode(opt) )
    {
	vector<string> sls;
	if( SYS->transport().at().modPresent("Serial") )
	    SYS->transport().at().at("Serial").at().outList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), devTp(cfg("DEV_TP").getSd()), devAddr(cfg("DEV_ADDR").getSd()), devPrms(cfg("DEV_PRMS").getSd())
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
    if( enableStat() )	return;

    TParamContr::enable();

    owner().prmEn( id(), true );
}

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
    //> Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/DEV_TP",cfg("DEV_TP").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/devLst");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/devLst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","FLowTC_UGTAA55")->setText("FLowTEC UGT‑AA55");
	opt->childAdd("el")->setAttr("id","Ergomera")->setText("Ergomera");
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod( owner().period() ? owner().period()/1e3 : 1000000 );
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}


//OpenSCADA system module DAQ.OPC_UA file: mod_daq.cpp
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

#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "mod_daq.h"

OPC_UA::TTpContr *OPC_UA::mod;  //Pointer for direct access to DAQ module

using namespace OPC_UA;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name )
{
    mId		= DAQ_ID;
    mName	= DAQ_NAME;
    mType	= DAQ_TYPE;
    mVers	= DAQ_MVER;
    mAutor	= DAQ_AUTOR;
    mDescr	= DAQ_DESCR;
    mLicense	= DAQ_LICENSE;
    mSource	= name;

    mod		= this;
}

TTpContr::~TTpContr()
{

}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	DAQ_TYPE,DAQ_ID,nodePath().c_str());

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
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Integer,TFld::NoFlag,"3","1","0;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100","") );
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
    cfg("PRM_BD").setS("OPC_UA_Prm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if( startStat() && !redntUse( ) ) rez += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    //> Start the gathering data task
    if( !prc_st ) SYS->taskCreate( nodePath('.',true), m_prior, TMdContr::Task, this, &prc_st );
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


		//????
	    }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resRelease( );
	cntr.tm_gath = 1e-3*(TSYS::curTime()-t_cnt);

	TSYS::taskSleep((long long)(1e9*cntr.period()));
    }

    cntr.prc_st = false;

    return NULL;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), p_el("w_attr")
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
    //> Service commands process
    string a_path = opt->attr("path");
    if( a_path.substr(0,6) == "/serv/" )  { TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	return;
    }

    TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

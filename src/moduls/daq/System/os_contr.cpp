
//OpenSCADA system module DAQ.System file: os_contr.cpp
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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
#include <getopt.h>
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
#include "os_contr.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"System"
#define MOD_NAME	"System DA"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"1.7.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow operation system data acquisition. Support OS Linux data sources: HDDTemp, Sensors, Uptime, Memory, CPU and other."
#define LICENSE		"GPL"
//*************************************************

SystemCntr::TTpContr *SystemCntr::mod;  //Pointer for direct access to module

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
	    return new SystemCntr::TTpContr( source );
	return NULL;
    }
}

using namespace SystemCntr;

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
    nodeDelAll();
    for(int i_da = 0; i_da < m_da.size(); i_da++ )	delete m_da[i_da];
    m_da.clear();
}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void TTpContr::load_( )
{
    //- Load parameters from command line -
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
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //- Init DA sources -
    daReg( new CPU() );
    daReg( new Mem() );
    daReg( new Sensors() );
    daReg( new Hddtemp() );
    daReg( new UpTime() );
    daReg( new HddSmart() );
    daReg( new HddStat() );
    daReg( new NetStat() );

    //- Controler's bd structure -
    fldAdd( new TFld("AUTO_FILL",_("Auto create active DA"),TFld::Boolean,TFld::NoFlag,"1","0") );
    fldAdd( new TFld("PRM_BD",_("System parameteres table"),TFld::String,TFld::NoFlag,"30","system") );
    fldAdd( new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","1000","0;10000") );
    fldAdd( new TFld("PRIOR",_("Request task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );

    //- Parameter type bd structure -
    //-- Make enumerated --
    string el_id,el_name,el_def;
    vector<string> list;
    daList(list);
    for( int i_ls = 0; i_ls < list.size(); i_ls++ )
    {
	if( i_ls == 0 )	el_def = list[i_ls];
	el_id+=list[i_ls]+";";
	el_name=el_name+_(daGet(list[i_ls])->name().c_str())+";";
    }
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",_("System part"),TFld::String,TFld::Selected|TCfg::NoVal,"10",el_def.c_str(),el_id.c_str(),el_name.c_str()) );
    tpPrmAt(t_prm).fldAdd( new TFld("SUBT" ,"",TFld::String,TFld::Selected|TCfg::NoVal|TFld::SelfFld,"10") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return( new TMdContr(name,daq_db,this));
}

void TTpContr::daList( vector<string> &da )
{
    da.clear();
    for(int i_da = 0; i_da < m_da.size(); i_da++ )
	da.push_back(m_da[i_da]->id());
}

void TTpContr::daReg( DA *da )
{
    m_da.push_back(da);
}

DA *TTpContr::daGet( const string &da )
{
    for(int i_da = 0; i_da < m_da.size(); i_da++ )
	if( m_da[i_da]->id() == da ) return m_da[i_da];

    return NULL;
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false),
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), tm_calc(0.0)
{
    cfg("PRM_BD").setS("OSPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if( startStat() && !redntUse( ) ) rez += TSYS::strMess(_("Get data %.6g ms. "),tm_calc);
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::enable_(  )
{
    if( cfg("AUTO_FILL").getB() )
    {
	vector<string> list;
	mod->daList(list);
	for( int i_l = 0; i_l < list.size(); i_l++ )
	    mod->daGet(list[i_l])->makeActiveDA(this);
    }
}

void TMdContr::start_( )
{
    //- Start the request data task -
    if( !prc_st )
    {
	pthread_attr_t pthr_attr;
	pthread_attr_init(&pthr_attr);
	struct sched_param prior;
	if( m_prior && SYS->user() == "root" )
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_RR);
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	prior.__sched_priority=m_prior;
	pthread_attr_setschedparam(&pthr_attr,&prior);

	pthread_create(&procPthr,&pthr_attr,TMdContr::Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait(prc_st, true, nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),_("Acquisition task no started!"));
    }
}

void TMdContr::stop_( )
{
    //- Stop the request and calc data task -
    if( prc_st )
    {
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),_("Acquisition task no stoped!"));
	pthread_join( procPthr, NULL );
    }
    //- Set Eval for parameters -
    ResAlloc res(en_res,true);
    for( int i_prm = 0; i_prm < p_hd.size(); i_prm++ )
        p_hd[i_prm].at().setEval();
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )
	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )
	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while( !cntr.endrun_req )
    {
	if( !cntr.redntUse( ) )
	{
	    //> Update controller's data
	    try
	    {
		long long t_cnt = TSYS::curTime();

		cntr.en_res.resRequestR();
		for( unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++ )
		    cntr.p_hd[i_p].at().getVal();
		cntr.en_res.resRelease();

		cntr.tm_calc = 1e-3*(TSYS::curTime()-t_cnt);
	    } catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }
	}

	TSYS::taskSleep((long long)cntr.period()*1000000);
    }

    cntr.prc_st = false;

    return NULL;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), m_da(NULL), m_auto(false)
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
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if( enableStat() )	return;
    cfg("TYPE").setS(cfg("TYPE").getS());
    TParamContr::enable();
    ((TMdContr&)owner()).prmEn( id(), true );	//Put to process
}

void TMdPrm::disable( )
{
    if( !enableStat() )  return;
    ((TMdContr&)owner()).prmEn( id(), false );	//Remove from process
    setEval( );
    //setType("");
    TParamContr::disable();
}

void TMdPrm::load_( )
{
    if(!m_auto)	TParamContr::load_();
}

void TMdPrm::save_( )
{
    if( !m_auto ) TParamContr::save_();

    //> Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for( int i_a = 0; i_a < a_ls.size(); i_a++ )
	if( !vlAt(a_ls[i_a]).at().arch().freeStat() )
	    vlAt(a_ls[i_a]).at().arch().at().save();
}

void TMdPrm::vlGet( TVal &val )
{
    if( val.name() == "err" )
    {
	if( !owner().startStat() ) val.setS(_("2:Controller stoped"),0,true);
	else if( !enableStat() )   val.setS(_("1:Parameter disabled"),0,true);
	else val.setS("0",0,true);
    }
}

void TMdPrm::getVal( )
{
    if( m_da )	m_da->getVal(this);
}

void TMdPrm::setEval( )
{
    if( !m_da )	return;

    vector<string> als;
    m_da->fldList(als);
    for( int i_a = 0; i_a < als.size(); i_a++ )
	if( vlPresent(als[i_a]) )
	    vlAt(als[i_a]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(((long long)owner().period())*1000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

void TMdPrm::setType( const string &da_id )
{
    if( m_da && da_id == m_da->id() )	return;

    //- Free previous type -
    if( m_da )
    {
	m_da->deInit(this);
	vlElemDet(m_da);
	m_da = NULL;
    }

    //- Create new type -
    try
    {
	if(da_id.size())
	{
	    m_da = mod->daGet(da_id);
	    if(m_da)
	    {
		vlElemAtt(m_da);
		m_da->init(this);
	    }
	}
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{
    //- Change TYPE parameter -
    if( i_cfg.name() == "TYPE" )
    {
	setType(i_cfg.getS());
	return true;
    }
    if( !autoC( ) ) modif();
    return true;
}

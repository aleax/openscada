/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#include <resalloc.h>
#include <tmess.h>
#include <ttransports.h>
#include <ttiparam.h>
#include <tcontrollers.h>

#include "da_cpu.h"
#include "da_mem.h"
#include "da_uptime.h"
#include "da_smart.h"
#include "da_sensors.h"
#include "sys.h"

//============ Modul info! =====================================================
#define MOD_ID      "OperationSystem"
#define MOD_NAME    "Operation system DA"
#define MOD_TYPE    "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.7.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow operation system data acquisition. Support OS Linux data sources: HDDTemp, LMSensors, Uptime, Memory, CPU"
#define LICENSE     "GPL"
//==============================================================================

SystemCntr::TTpContr *SystemCntr::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
    	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	SystemCntr::TTpContr *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = SystemCntr::mod = new SystemCntr::TTpContr( source );

	return ( self_addr );
    }
}

using namespace SystemCntr;

//======================================================================
//==== TTpContr ======================================================== 
//======================================================================
TTpContr::TTpContr( string name )  
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;    
}

TTpContr::~TTpContr()
{    

}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
    
    return(buf);
}

void TTpContr::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //========== Load parameters from config file =============
    //Check present DA and make the system data controller include parameters
    //Make Auto controller
    if( !present("AutoSYS") )
    {
        add("AutoSYS",TBDS::SName("","","CntrAutoSYS"));
	at("AutoSYS").at().name(I18N("Auto init controller"));
	at("AutoSYS").at().cfg("ENABLE").setB(true);
	at("AutoSYS").at().cfg("START").setB(true);
    }
}

void TTpContr::postEnable( )
{    
    TModule::postEnable();

    //Init DA sources
    daReg(new CPU());
    daReg(new Mem());
    daReg(new Lmsensors());
    daReg(new Hddtemp());
    daReg(new UpTime());			    

    //==== Controler's bd structure ====    
    fldAdd( new TFld("PRM_BD",I18N("System parameteres table"),TFld::String,0,"30","system") );
    fldAdd( new TFld("PERIOD",I18N("The request period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    //==== Parameter type bd structure ====
    //Make enumerated
    string el_id,el_name,el_def;
    vector<string> list;
    daList(list);
    for( int i_ls = 0; i_ls < list.size(); i_ls++ )
    {
	if( i_ls == 0 )	el_def = list[i_ls];
	el_id+=list[i_ls]+";";
	el_name+=I18Ns(daGet(list[i_ls])->name())+";";    
    }   
    int t_prm = tpParmAdd("All","PRM_BD",I18N("All parameters"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("System part"),TFld::String,FLD_SELECT|FLD_NOVAL|FLD_PREV,"10",el_def.c_str(),el_id.c_str(),el_name.c_str()) );
    tpPrmAt(t_prm).fldAdd( new TFld("SUBT" ,"",TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV|FLD_SELF,"2") );
}

TController *TTpContr::ContrAttach( const string &name, const TBDS::SName &bd)
{
    return( new TMdContr(name,bd,this));    
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

//======================================================================
//==== TMdContr 
//======================================================================

TMdContr::TMdContr( string name_c, const TBDS::SName &bd, ::TElem *cfgelem) :
	::TController(name_c,bd,cfgelem), endrun(false), period(cfg("PERIOD").getId())
{    
    en_res = ResAlloc::resCreate();
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
    ResAlloc::resDelete(en_res);
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return(new TMdPrm(name,&owner().tpPrmAt(type)));
}

void TMdContr::enable_(  )
{
    if( id() == "AutoSYS" )
    {
	vector<string> list;
	mod->daList(list);
	for( int i_l = 0; i_l < list.size(); i_l++ )
	    mod->daGet(list[i_l])->makeActiveDA(this);	    
    }
}

void TMdContr::load( )
{
    TController::load();
}

void TMdContr::save( )
{
    TController::save();
}

void TMdContr::start( )
{      
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    vector<string> 	list_p;

    TController::start();

    if( !run_st )
    {
	//---- Enable parameters ----
	list(list_p);	
	for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	    if( at(list_p[i_prm]).at().toEnable() )
		at(list_p[i_prm]).at().enable();
	//    p_hd.push_back( at(list_p[i_prm],name()+"_start") );
	//---- Start process task ----
	pthread_attr_init(&pthr_attr);
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait( run_st, true,nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),"Controller no started!");
    }    
}

void TMdContr::stop( )
{  
    vector<string> 	list_p;

    TController::stop();

    if( run_st )
    {
	//---- Stop process task ----
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),"Controller no stoped!");
	pthread_join(pthr_tsk, NULL);
	
	//---- Disable params ----
	list(list_p);
        for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
            if( at(list_p[i_prm]).at().enableStat() )
                at(list_p[i_prm]).at().disable();
    }
} 

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
        if( p_hd[i_prm].at().name() == id ) break;
    
    if( val && i_prm >= p_hd.size() )
        p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )
        p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task(void *contr)
{
    struct itimerval mytim;             //Interval timer
    TMdContr *cntr = (TMdContr *)contr;

#if OSC_DEBUG
    Mess->put(cntr->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif

    if(cntr->period == 0) return(NULL);

    //Start interval timer
    mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
    mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;
    setitimer(ITIMER_REAL,&mytim,NULL);
    
    cntr->run_st = true;  cntr->endrun = false;
    while( !cntr->endrun )
    {
	ResAlloc::resRequestR(cntr->en_res);
	for(unsigned i_p=0; i_p < cntr->p_hd.size(); i_p++)
	    cntr->p_hd[i_p].at().getVal();
	ResAlloc::resReleaseR(cntr->en_res);
	    
	pause();
    }
    
    //Stop interval timer
    mytim.it_interval.tv_sec = mytim.it_interval.tv_usec = 0;
    mytim.it_value.tv_sec    = mytim.it_value.tv_usec    = 0;
    setitimer(ITIMER_REAL,&mytim,NULL);
    
    cntr->run_st = false;

    return(NULL);    
}

//======================================================================
//==== TMdPrm 
//======================================================================

TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : TParamContr(name,tp_prm), m_da(NULL)
{

}

void TMdPrm::postEnable()
{
    TParamContr::postEnable();
    
    vector<string> list;
    mod->daList(list);
    if(list.size())
	cfg("TYPE").setS(list[0]);
}


TMdPrm::~TMdPrm( )
{    

}

void TMdPrm::enable()
{
    if( enableStat() )	return;    
    cfgChange(cfg("TYPE"));
    TParamContr::enable();
    ((TMdContr&)owner()).prmEn( name(), true );	//Put to process
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;
    ((TMdContr&)owner()).prmEn( name(), false );      //Remove from process 
    setType("");
    TParamContr::disable();
}

void TMdPrm::preDisable( int flag )
{
    setType("");
}

void TMdPrm::vlGet( TVal &val )
{
    
}

void TMdPrm::getVal()
{
    if( m_da )	m_da->getVal(this);
}

void TMdPrm::setType( const string &da_id )
{
    if( m_da && da_id == m_da->id() )	return;
        
    //Free previous type
    if( m_da )
    {	
	m_da->deInit(this);
	vlDetElem(m_da);
	m_da = NULL;
    }

    //Create new type
    try 
    { 	
	if(da_id.size())
	{
	    m_da = mod->daGet(da_id); 
	    vlAttElem(m_da);
	    m_da->init(this);
	}
    }
    catch(TError err) { }
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{        
    //Change TYPE parameter
    if( i_cfg.name() == "TYPE" )
    {
	setType(i_cfg.getS());
       	return true;       
    }    
    //Change SUBTYPE parameter
    else 
    {	
	if( m_da ) m_da->chCfg( this, i_cfg );
       	return true;       
    }    
    return false;
}



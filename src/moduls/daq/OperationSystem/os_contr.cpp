
//OpenSCADA system module DAQ.OperationSystem file: os_contr.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

//============ Modul info! =====================================================
#define MOD_ID      "OperationSystem"
#define MOD_NAME    "Operation system DA"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "1.2.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow operation system data acquisition. Support OS Linux data sources: HDDTemp, Sensors, Uptime, Memory, CPU and other."
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
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;    
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
}

void TTpContr::postEnable( )
{    
    TModule::postEnable();

    //Init DA sources
    daReg( new CPU() );
    daReg( new Mem() );
    daReg( new Sensors() );
    daReg( new Hddtemp() );
    daReg( new UpTime() );
    daReg( new HddSmart() );
    daReg( new HddStat() );
    daReg( new NetStat() );

    //==== Controler's bd structure ====    
    fldAdd( new TFld("AUTO_FILL",I18N("Auto create active DA"),TFld::Bool,FLD_NOFLG,"1","false") );
    fldAdd( new TFld("PRM_BD",I18N("System parameteres table"),TFld::String,FLD_NOFLG,"30","system") );
    fldAdd( new TFld("PERIOD",I18N("Request data period (ms)"),TFld::Dec,FLD_NOFLG,"5","1000","0;10000") );
    fldAdd( new TFld("PRIOR",I18N("Request task priority"),TFld::Dec,FLD_NOFLG,"2","0","0;100") );
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
    int t_prm = tpParmAdd("std","PRM_BD",I18N("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("System part"),TFld::String,FLD_SELECT|FLD_NOVAL|FLD_PREV,"10",el_def.c_str(),el_id.c_str(),el_name.c_str()) );
    tpPrmAt(t_prm).fldAdd( new TFld("SUBT" ,"",TFld::String,FLD_SELECT|FLD_NOVAL|FLD_SELF,"10") );
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

//======================================================================
//==== TMdContr 
//======================================================================

TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), 
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), tm_calc(0.0)
{    
    en_res = ResAlloc::resCreate();
    cfg("PRM_BD").setS("OSPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
    
    ResAlloc::resDelete(en_res);    
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

void TMdContr::load( )
{
    TController::load( );
}

void TMdContr::save( )
{
    TController::save();
}

void TMdContr::start( )
{      
    TController::start();

    if( run_st ) return;
    
    //Start the request data task
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
            throw TError(nodePath().c_str(),mod->I18N("Acquisition task no started!"));    
    }
    
    //---- Enable parameters ----
    vector<string>      list_p;
    list(list_p);
    for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
        if( at(list_p[i_prm]).at().toEnable() )
    	    at(list_p[i_prm]).at().enable();
	    
    run_st = true;
}

void TMdContr::stop( )
{  
    TController::stop();

    if( !run_st ) return;
    
    //Stop the request and calc data task
    if( prc_st )
    {
        endrun_req = true;
        pthread_kill( procPthr, SIGALRM );
        if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
            throw TError(nodePath().c_str(),mod->I18N("Acquisition task no stoped!"));
        pthread_join( procPthr, NULL );
    }
    
    //---- Disable params ----
    vector<string> list_p;
    list(list_p);
    for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
        if( at(list_p[i_prm]).at().enableStat() )
            at(list_p[i_prm]).at().disable();

    run_st = false;	    
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
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    TMdContr &cntr = *(TMdContr *)icntr;
    
    cntr.endrun_req = false;
    cntr.prc_st = true;
    
    while(!cntr.endrun_req)
    {
	//Update controller's data
	try
	{
	    unsigned long long t_cnt = SYS->shrtCnt();
	    
	    ResAlloc::resRequestR(cntr.en_res);
	    for(unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++)
		cntr.p_hd[i_p].at().getVal();
	    ResAlloc::resReleaseR(cntr.en_res);
		
	    cntr.tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());	
	} catch(TError err)
	{ Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str() ); }    
    
        //Calc next work time and sleep
        clock_gettime(CLOCK_REALTIME,&get_tm);
        work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/((long long)cntr.m_per*1000000) + 1)*(long long)cntr.m_per*1000000;
	if(last_tm == work_tm)  work_tm+=(long long)cntr.m_per*1000000; //Fix early call
	last_tm = work_tm;
        get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
        clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);
    }
    
    cntr.prc_st = false;
    
    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/ctm",mod->I18N("Calk time (msek)"),0444,"root","root",1,"tp","real");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/ctm" && ctrChkNode(opt) )	opt->text(TSYS::real2str(tm_calc));
    else TController::cntrCmdProc(opt);
}						    

//======================================================================
//==== TMdPrm 
//======================================================================

TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), m_da(NULL), m_auto(false)
{

}

void TMdPrm::postEnable()
{
    TParamContr::postEnable();
    
    vector<string> list;
    mod->daList(list);
    if(list.size())	cfg("TYPE").setS(list[0]);
}


TMdPrm::~TMdPrm( )
{
    nodeDelAll();    
}

void TMdPrm::enable()
{
    if( enableStat() )	return;    
    cfgChange(cfg("TYPE"));
    TParamContr::enable();
    ((TMdContr&)owner()).prmEn( id(), true );	//Put to process
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;
    ((TMdContr&)owner()).prmEn( id(), false );      //Remove from process 
    if( m_da )	m_da->setEVAL(this);
    //setType("");
    TParamContr::disable();
}

void TMdPrm::load( )
{
    if(!m_auto)	TParamContr::load();
}

void TMdPrm::save( )
{
    if(!m_auto) TParamContr::save();
}

void TMdPrm::vlGet( TVal &val )
{
    if( val.name() == "err" )
    {
	if( !owner().startStat() ) val.setS(mod->I18N("2:Controller stoped"),0,true);
	else if( !enableStat() )   val.setS(mod->I18N("1:Parameter disabled"),0,true);
	else val.setS("0",0,true);
    }
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
	vlElemDet(m_da);
	m_da = NULL;
    }

    //Create new type
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
    catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str() ); }
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{   
    //Change TYPE parameter
    if( i_cfg.name() == "TYPE" )
    {
	setType(i_cfg.getS());
       	return true;       
    }    
    return false;
}



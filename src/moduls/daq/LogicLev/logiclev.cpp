
//OpenSCADA system module DAQ.LogicLev file: logiclev.cpp
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
#include <ttiparam.h>
#include <tdaqs.h>

#include "logiclev.h"

//============ Modul info! =====================================================
#define MOD_ID      "LogicLev"
#define MOD_NAME    "Logic level"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.5.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow logic level paramers."
#define LICENSE     "GPL"
//==============================================================================

LogicLev::TTpContr *LogicLev::mod;  //Pointer for direct access to module

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

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	LogicLev::TTpContr *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = LogicLev::mod = new LogicLev::TTpContr( source );

	return self_addr;
    }
}

using namespace LogicLev;

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
    
    return buf;
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

    //==== Controler's bd structure ====    
    fldAdd( new TFld("PRM_BD",I18N("Parameteres table"),TFld::String,FLD_NOFLG,"30","") );
    fldAdd( new TFld("PERIOD",I18N("Request data period (ms)"),TFld::Dec,FLD_NOFLG,"5","1000","0;10000") );
    fldAdd( new TFld("PRIOR",I18N("Request task priority"),TFld::Dec,FLD_NOFLG,"2","0","0;100") );
    //==== Parameter type bd structure ====
    int t_prm = tpParmAdd("std","PRM_BD",I18N("Logical"));
    tpPrmAt(t_prm).fldAdd( new TFld("MODE",I18N("Mode"),TFld::Dec,FLD_NOVAL,"1","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("PRM","",TFld::String,FLD_NOVAL,"100","") );
    //Logical level parameter IO BD structure
    el_prm_io.fldAdd( new TFld("PRM_ID",I18N("Parameter ID"),TFld::String,FLD_KEY,"20") );
    el_prm_io.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_prm_io.fldAdd( new TFld("VALUE",I18N("Value"),TFld::String,0,"200") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//======================================================================
//==== TMdContr 
//======================================================================
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_calc(0),
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId())
{    
    en_res = ResAlloc::resCreate();
    cfg("PRM_BD").setS("LogLevPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
    
    ResAlloc::resDelete(en_res);    
}

void TMdContr::postDisable(int flag)
{
    TController::postDisable(flag);
    try
    {
	if( flag )
        {
    	    //Delete parameter's io table
            string tbl = genBD()+"."+cfg("PRM_BD").getS()+"_io";
            SYS->db().at().open(tbl);
            SYS->db().at().close(tbl,true);
        }
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
}
																				

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
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
	long long t_cnt = SYS->shrtCnt();
	//Update controller's data
	ResAlloc::resRequestR(cntr.en_res);
	for(unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++)
	    try{ cntr.p_hd[i_p].at().calc(); }
	    catch(TError err)
    	    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
	ResAlloc::resReleaseR(cntr.en_res);    
	cntr.tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    
        //Calc next work time and sleep
        clock_gettime(CLOCK_REALTIME,&get_tm);
        work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/((long long)cntr.m_per*1000000) + 1)*(long long)cntr.m_per*1000000;
	if(last_tm == work_tm)	work_tm+=(long long)cntr.m_per*1000000;	//Fix early call
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
	ctrMkNode("fld",opt,-1,"/cntr/st/calc_tm",Mess->I18N("Calc template functions time (ms)"),0444,"root","root",1,"tp","real");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/calc_tm" && ctrChkNode(opt) )	opt->text(TSYS::real2str(tm_calc));
    else TController::cntrCmdProc(opt);
}

//======================================================================
//==== TMdPrm 
//======================================================================
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), p_el("w_attr"), prm_refl(NULL), m_wmode(TMdPrm::Free), chk_lnk_need(false),
    m_mode(cfg("MODE").getId()), m_prm(cfg("PRM").getSd())
{
    moderes = ResAlloc::resCreate( );
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();    
    
    ResAlloc::resDelete( moderes );
}

void TMdPrm::postEnable( )
{
    TParamContr::postEnable( );
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

void TMdPrm::postDisable(int flag)
{
    TParamContr::postDisable(flag);
    
    try
    {
        if( flag )
        {
            string io_bd = owner().genBD()+"."+owner().cfg(type().BD()).getS()+"_io";
            TConfig cfg(&mod->prmIOE());
	    cfg.cfg("PRM_ID").setS(id());
	    cfg.cfg("ID").setS("");
	    SYS->db().at().dataDel(io_bd,owner().owner().nodePath()+owner().cfg(type().BD()).getS()+"_io",cfg);
        }
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,"%s",err.mess.c_str()); }
}

void TMdPrm::enable()
{
    if( enableStat() )	return;    

    TParamContr::enable();

    try
    {
        mode((TMdPrm::Mode)m_mode,m_prm);
        loadIO();
    }
    catch(...){ disable(); throw; }
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;
    
    mode(TMdPrm::Free);
    
    TParamContr::disable();
}

void TMdPrm::load( )
{
    TParamContr::load();
    try
    {
	mode(mode(),m_prm);
	loadIO();
    }catch(...){ }    
}

void TMdPrm::loadIO()
{
    //Load IO and init links
    if( mode() == TMdPrm::Template )
    {
        TConfig cfg(&mod->prmIOE());
        cfg.cfg("PRM_ID").setS(id());
        string io_bd = owner().genBD()+"."+owner().cfg(type().BD()).getS()+"_io";

	for( int i_io = 0; i_io < tmpl->val.ioSize(); i_io++ )
        {
            int attr_id = tmpl->tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
            if( attr_id < 0 ) continue;
            
	    cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
            if(!SYS->db().at().dataGet(io_bd,owner().owner().nodePath()+owner().cfg(type().BD()).getS()+"_io",cfg))
            continue;
            if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link )
                lnk(lnkId(i_io)).prm_attr = cfg.cfg("VALUE").getS();
            else tmpl->val.setS(i_io,cfg.cfg("VALUE").getS());
        }
        initTmplLnks();
    }
}

void TMdPrm::save( )
{
    TParamContr::save();
    saveIO();
}

void TMdPrm::saveIO()
{
    //Save IO and init links
    if( mode() == TMdPrm::Template )
    {
        TConfig cfg(&mod->prmIOE());
        cfg.cfg("PRM_ID").setS(id());
        string io_bd = owner().genBD()+"."+owner().cfg(type().BD()).getS()+"_io";
			    
        for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
        {
            int attr_id = tmpl->tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
            if( attr_id < 0 ) continue;
            cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
            if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link )
                cfg.cfg("VALUE").setS(lnk(lnkId(i_io)).prm_attr);
            else cfg.cfg("VALUE").setS(tmpl->val.getS(i_io));
            SYS->db().at().dataSet(io_bd,owner().owner().nodePath()+owner().cfg(type().BD()).getS()+"_io",cfg);
        }
    }
}

void TMdPrm::mode( TMdPrm::Mode md, const string &prm )
{
    vector<string> list;
    
    if( !enableStat() ) return;

    ResAlloc res(moderes,true);

    //Free old mode
    if( md != mode() || prm != m_wprm )
    {
        if( mode() == TMdPrm::DirRefl )
        {
            for(int i_f = 0; i_f < p_el.fldSize(); i_f++ )
		if( vlAt(p_el.fldAt(i_f).name()).at().nodeUse() == 1 )
		{
		    p_el.fldDel(i_f);
		    i_f--;
		}
            delete prm_refl;
            prm_refl = NULL;
        }
        else if( mode() == TMdPrm::Template )
	{           
	    owner().prmEn( id(), false );
	    for(int i_f = 0; i_f < p_el.fldSize(); i_f++ )
                if( vlAt(p_el.fldAt(i_f).name()).at().nodeUse() == 1 )
                {
                    p_el.fldDel(i_f);
            	    i_f--;
	        }
    	    delete tmpl;
            tmpl = NULL;
        }
    }
    
    //Init/update new mode
    if( md == TMdPrm::DirRefl )
    {
        if( !prm_refl ) prm_refl = new AutoHD<TValue>;
        try
	{
            prm_refl->free();
            if( !dynamic_cast<TValue*>(&SYS->nodeAt(prm,0,'.').at()) )
                throw TError(nodePath().c_str(),mod->I18N("Do not select parameter."));
            *prm_refl = SYS->nodeAt(prm,0,'.');
            prm_refl->at().vlList(list);
            for( int i_l = 0; i_l < list.size(); i_l++ )
                if( !vlPresent(list[i_l]) )
                    p_el.fldAdd( new TFld(list[i_l].c_str(),prm_refl->at().vlAt(list[i_l]).at().fld().descr().c_str(),
                        prm_refl->at().vlAt(list[i_l]).at().fld().type(),FLD_DWR|FLD_DRD|(prm_refl->at().vlAt(list[i_l]).at().fld().flg())&FLD_NWR) );
        }catch(...)
	{
    	    m_wmode = Free;
    	    delete prm_refl;
            prm_refl = NULL;
            throw;
        }
    }
    else if( md == TMdPrm::Template )
    {
        bool to_make = false;
        if( !tmpl )     tmpl = new STmpl;
        try
        {
    	    tmpl->tpl = SYS->daq().at().nodeAt(prm,0,'.');
            if( !tmpl->val.func() )
            {
                tmpl->val.func(&tmpl->tpl.at().func().at());
                tmpl->val.name(id()+"_tmplprm");
                to_make = true;
            }
	    //Set mode
            m_wmode = md;
            //Init attrubutes
            for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
            {
        	int attr_id = tmpl->tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
	        if( attr_id < 0 )       continue;
	        if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link && lnkId(i_io) < 0 )
	            tmpl->lnk.push_back(SLnk(i_io,tmpl->tpl.at().attr(attr_id).accs));
	        if( tmpl->tpl.at().attr(attr_id).attr != TPrmTempl::NoAttr &&
	            !vlPresent(tmpl->val.func()->io(i_io)->id()) )
	        {
                    TFld::Type tp;
                    unsigned char flg = FLD_DWR|FLD_DRD;
                    
		    switch( tmpl->val.ioType(i_io) )
                    {
                        case IO::String:        tp = TFld::String;      break;
                        case IO::Integer:       tp = TFld::Dec;         break;
                	case IO::Real:  	tp = TFld::Real;        break;
                        case IO::Boolean:       tp = TFld::Bool;        break;
                    }
                    if( tmpl->tpl.at().attr(attr_id).attr == TPrmTempl::ReadOnly )
                        flg|=FLD_NWR;
                    p_el.fldAdd( new TFld(tmpl->val.func()->io(i_io)->id().c_str(),
                        tmpl->val.func()->io(i_io)->name().c_str(),tp,flg) );
                }
                if( to_make && (tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::PublConst ||
                	tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Const ) )
                    tmpl->val.setS(i_io,tmpl->tpl.at().attr(attr_id).val);
            }
            //Init links
            initTmplLnks();
    	    
	    //Set to process
            owner().prmEn( id(), true );
        }catch(TError err)
	{
    	    m_wmode = Free;
            delete tmpl;
            tmpl = NULL;
            throw;
        }
    }
    m_wmode = md;
    m_wprm = prm;
}

void TMdPrm::initTmplLnks()
{
    if( mode() != TMdPrm::Template )    return;
    //Init links
    chk_lnk_need = false;
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
    {
        try
	{
            lnk(i_l).aprm.free();
            if( dynamic_cast<TVal *>(&SYS->nodeAt(lnk(i_l).prm_attr,0,'.').at()) )
            {
                lnk(i_l).aprm = SYS->nodeAt(lnk(i_l).prm_attr,0,'.');
                //Init value
                tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).aprm.at().getS());
            }
        }catch(TError err){ chk_lnk_need = true; }
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if( !enableStat() )
    {
        if( val.name() == "err" ) val.setS(mod->I18N("1:Parameter had disabled."),0,true);
	else val.setS(EVAL_STR,0,true);
        return;
    }
    try
    {
	ResAlloc res(moderes,false);
        if( mode() == TMdPrm::DirRefl )
            switch(val.fld().type())
            {
                case TFld::String:
                    val.setS(prm_refl->at().vlAt(val.name()).at().getS(),0,true);
                    break;
                case TFld::Dec:
                    val.setI(prm_refl->at().vlAt(val.name()).at().getI(),0,true);
                    break;
                case TFld::Real:
                    val.setR(prm_refl->at().vlAt(val.name()).at().getR(),0,true);
                    break;
                case TFld::Bool:
                    val.setB(prm_refl->at().vlAt(val.name()).at().getB(),0,true);
                    break;
            }
        else if( mode() == TMdPrm::Template )
        {
            int id_lnk = lnkId(val.name());
            if( id_lnk >= 0 && lnk(id_lnk).aprm.freeStat() )
                id_lnk=-1;
            switch(val.fld().type())
            {
                case TFld::String:
                    if( id_lnk < 0 ) val.setS(tmpl->val.getS(tmpl->val.ioId(val.name())),0,true);
            	    else val.setS(lnk(id_lnk).aprm.at().getS(),0,true);
	            break;
	        case TFld::Dec:
	            if( id_lnk < 0 ) val.setI(tmpl->val.getI(tmpl->val.ioId(val.name())),0,true);
	            else val.setI(lnk(id_lnk).aprm.at().getI(),0,true);
                    break;
                case TFld::Real:
            	    if( id_lnk < 0 ) val.setR(tmpl->val.getR(tmpl->val.ioId(val.name())),0,true);
            	    else val.setR(lnk(id_lnk).aprm.at().getR(),0,true);
                    break;
                case TFld::Bool:
                    if( id_lnk < 0 ) val.setB(tmpl->val.getB(tmpl->val.ioId(val.name())),0,true);
                    else val.setB(lnk(id_lnk).aprm.at().getB(),0,true);
                    break;
            }
        }
    }catch(TError err)
    { if( val.name() == "err" ) val.setS("0",0,true); }    
}

void TMdPrm::vlSet( TVal &val )
{
    try
    {
	ResAlloc res(moderes,false);
	if( mode() == TMdPrm::DirRefl )
            switch(val.fld().type())
            {
        	case TFld::String:
                    prm_refl->at().vlAt(val.name()).at().setS(val.getS(0,true));
                    break;
        	case TFld::Dec:
                    prm_refl->at().vlAt(val.name()).at().setI(val.getI(0,true));
                    break;
                case TFld::Real:
                    prm_refl->at().vlAt(val.name()).at().setR(val.getR(0,true));
                    break;
                case TFld::Bool:
                    prm_refl->at().vlAt(val.name()).at().setB(val.getB(0,true));
                    break;
            }
        else if( mode() == TMdPrm::Template )
	{
            int id_lnk = lnkId(val.name());
            if( id_lnk >= 0 && lnk(id_lnk).aprm.freeStat())
                id_lnk=-1;
            switch(val.fld().type())
            {
        	case TFld::String:
	    	    if( id_lnk < 0 ) tmpl->val.setS(tmpl->val.ioId(val.name()),val.getS(0,true));
                    else lnk(id_lnk).aprm.at().setS(val.getS(0,true));
                    break;
                case TFld::Dec:
                    if( id_lnk < 0 ) tmpl->val.setI(tmpl->val.ioId(val.name()),val.getI(0,true));
                    else lnk(id_lnk).aprm.at().setI(val.getI(0,true));
                    break;
        	case TFld::Real:
                    if( id_lnk < 0 ) tmpl->val.setR(tmpl->val.ioId(val.name()),val.getR(0,true));
                    else lnk(id_lnk).aprm.at().setR(val.getR(0,true));
                    break;
                case TFld::Bool:
                    if( id_lnk < 0 ) tmpl->val.setB(tmpl->val.ioId(val.name()),val.getB(0,true));
                    else lnk(id_lnk).aprm.at().setB(val.getB(0,true));
                    break;
            }
        }
    }catch(TError err) {  }
}
																																																				    
int TMdPrm::lnkSize()
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),mod->I18N("Parameter disabled or no template based."));
    return tmpl->lnk.size();
}
		
int TMdPrm::lnkId( int id )
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),mod->I18N("Parameter disabled or no template based."));
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
        if( lnk(i_l).io_id == id )
            return i_l;
    return -1;
}
							
int TMdPrm::lnkId( const string &id )
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),mod->I18N("Parameter disabled or no template based."));
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
        if( tmpl->val.func()->io(lnk(i_l).io_id)->id() == id )
            return i_l;
    return -1;
}
												
TMdPrm::SLnk &TMdPrm::lnk( int num )
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),mod->I18N("Parameter disabled or no template based."));
    if( num < 0 || num >= tmpl->lnk.size() )
	throw TError(nodePath().c_str(),mod->I18N("Parameter id error."));
    return tmpl->lnk[num];
}

void TMdPrm::calc()
{
    if( mode() != TMdPrm::Template )    return;
    try
    {
	ResAlloc res(moderes,false);
	if(chk_lnk_need) initTmplLnks();
        //Get input links
        for( int i_l = 0; i_l < lnkSize(); i_l++ )
        {
            if( lnk(i_l).aprm.freeStat() || lnk(i_l).mode != TPrmTempl::Link )
            continue;
        
	    switch(tmpl->val.ioType(lnk(i_l).io_id))
    	    {
                case IO::String:
                    tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).aprm.at().getS());
            	    break;
                case IO::Integer:
                    tmpl->val.setI(lnk(i_l).io_id,lnk(i_l).aprm.at().getI());
                    break;
                case IO::Real:
                    tmpl->val.setR(lnk(i_l).io_id,lnk(i_l).aprm.at().getR());
                    break;
                case IO::Boolean:
                    tmpl->val.setB(lnk(i_l).io_id,lnk(i_l).aprm.at().getB());
                    break;
            }
        }
	//Calc template
        tmpl->val.calc();
        //Put output links
        for( int i_l = 0; i_l < lnkSize(); i_l++ )
        {
            if( lnk(i_l).aprm.freeStat() || lnk(i_l).mode != TPrmTempl::Link ||
                lnk(i_l).aprm.at().fld().flg()&FLD_NWR )
        	    continue;
            
	    switch(tmpl->val.ioType(lnk(i_l).io_id))
    	    {
                case IO::String:
                    lnk(i_l).aprm.at().setS(tmpl->val.getS(lnk(i_l).io_id));
                    break;
                case IO::Integer:
                    lnk(i_l).aprm.at().setI(tmpl->val.getI(lnk(i_l).io_id));
                    break;
                case IO::Real:
                    lnk(i_l).aprm.at().setR(tmpl->val.getR(lnk(i_l).io_id));
                    break;
                case IO::Boolean:
                    lnk(i_l).aprm.at().setB(tmpl->val.getB(lnk(i_l).io_id));
                    break;
            }
        }
    }catch(TError err)
    {
        Mess->put(err.cat.c_str(),TMess::Warning,"%s",err.mess.c_str());
        Mess->put(nodePath().c_str(),TMess::Warning,mod->I18N("Error calc template."));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    vector<string> list;
    //Get page info
    if( opt->name() == "info" )
    {
        TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/MODE",cfg("MODE").fld().descr(),0660,"root","root",3,"tp","str","dest","select","select","/cfg/mode_lst");
	ctrMkNode("fld",opt,-1,"/prm/cfg/PRM",cfg("PRM").fld().descr(),0660,"root","root",3,"tp","str","dest","sel_ed","select","/cfg/prmp_lst");
        if( mode() == TMdPrm::Template )
        {
            ctrMkNode("area",opt,-1,"/cfg",mod->I18N("Template config"));
            ctrMkNode("fld",opt,-1,"/cfg/attr_only",mod->I18N("Only atributes show"),0664,"root","root",1,"tp","bool");
            ctrMkNode("area",opt,-1,"/cfg/prm",mod->I18N("Parameters"));
            for( int i_io = 0; i_io < tmpl->val.ioSize(); i_io++ )
            {
                int attr_id = tmpl->tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
                if( attr_id < 0 )   continue;
    		//Check select param
                if( !atoi(TBDS::genDBGet(nodePath()+"onlAttr","0",opt->attr("user")).c_str()) && 
			tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link && tmpl->tpl.at().attr(attr_id).val.size() )
                {
                    string nprm = TSYS::strSepParse(tmpl->tpl.at().attr(attr_id).val,0,'|');
                    //Check already to present parameters
                    bool f_ok = false;
                    for( int i_l = 0; i_l < list.size(); i_l++ )
                        if( list[i_l] == nprm )
                        { f_ok = true; break; }
                    if(!f_ok)
                    {
                        ctrMkNode("fld",opt,-1,(string("/cfg/prm/pr_")+TSYS::int2str(i_io)).c_str(),nprm,0664,"root","root",
			    3,"tp","str","dest","sel_ed","select",(string("/cfg/prm/pl_")+TSYS::int2str(i_io)).c_str());
                        list.push_back(nprm);
                    }
                }
                else
                {
                    char *tip = "str";
                    if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
                        switch(tmpl->val.ioType(i_io))
                        {
                            case IO::String:    tip = "str";    break;
                            case IO::Integer:   tip = "dec";    break;
                            case IO::Real:      tip = "real";   break;
                    	    case IO::Boolean:   tip = "bool";   break;
	                }
	            if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link ||
                        tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
                    {
                        XMLNode *eln = ctrMkNode("fld",opt,-1,(string("/cfg/prm/el_")+TSYS::int2str(i_io)).c_str(),
                            tmpl->val.func()->io(i_io)->name(),0664,"root","root",1,"tp",tip);
                        if( eln && tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link )
                            eln->attr_("dest","sel_ed")->attr("select",string("/cfg/prm/ls_")+TSYS::int2str(i_io));
                    }
                }
            }
        }
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/MODE" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
    	try
	{ 
	    m_mode = atoi(opt->text().c_str());
	    mode( (TMdPrm::Mode)m_mode, m_prm = "" ); 
	} catch(...) { disable(); throw; }
    else if( a_path == "/prm/cfg/PRM" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
        try
        { 
	    m_prm = opt->text();
	    mode( (TMdPrm::Mode)m_mode, m_prm ); 
	} catch(...){ disable(); throw; }    
    else if( a_path == "/cfg/mode_lst" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->attr("id",TSYS::int2str(TMdPrm::Free))->text(mod->I18N("Free parametr"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TMdPrm::DirRefl))->text(mod->I18N("Direct reflection"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TMdPrm::Template))->text(mod->I18N("Template"));
    }
    else if( a_path == "/cfg/prmp_lst" && ctrChkNode(opt) )
    {
        int c_lv = 0;
        string c_path = "";
        while(TSYS::strSepParse(m_prm,c_lv,'.').size())
        {
    	    opt->childAdd("el")->text(c_path);
	    if( c_lv ) c_path+=".";
            c_path = c_path+TSYS::strSepParse(m_prm,c_lv,'.');
            c_lv++;
        }
    	opt->childAdd("el")->text(c_path);
        if( c_lv != 0 ) c_path += ".";
    	SYS->daq().at().nodeAt(c_path,0,'.').at().nodeList(list);
        for( unsigned i_a=0; i_a < list.size(); i_a++ )
            opt->childAdd("el")->text(c_path+list[i_a]);
    }
    else if( a_path == "/cfg/attr_only" && mode() == TMdPrm::Template )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"onlAttr","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	TBDS::genDBSet(nodePath()+"onlAttr",opt->text(),opt->attr("user"));
    }
    else if( a_path.substr(0,12) == "/cfg/prm/pr_" && mode() == TMdPrm::Template )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	
    	{
    	    int i_lnk = lnkId(atoi(a_path.substr(12).c_str()));
            int i_cnt = 0;
            string e_vl;
            if( dynamic_cast<TVal *>(&SYS->nodeAt(lnk(i_lnk).prm_attr,0,'.').at()) )
                while(TSYS::strSepParse(lnk(i_lnk).prm_attr,i_cnt+1,'.').size())
                {
                    if(i_cnt)e_vl+=".";
                    e_vl+=TSYS::strSepParse(lnk(i_lnk).prm_attr,i_cnt++,'.');
                }
            else e_vl = lnk(i_lnk).prm_attr;
            opt->text(e_vl);
        }
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
        {
            bool only_no_set = true;
            string no_set;
            string p_nm = TSYS::strSepParse(tmpl->tpl.at().attr(lnk(lnkId(atoi(a_path.substr(12).c_str()))).io_id).val,0,'|');
            string p_vl = opt->text();
            bool it_val = dynamic_cast<TValue *>(&SYS->nodeAt(p_vl,0,'.').at());
            for( int i_l = 0; i_l < lnkSize(); i_l++ )
                if( p_nm == TSYS::strSepParse(tmpl->tpl.at().attr(lnk(i_l).io_id).val,0,'|') )
        	{
		    lnk(i_l).prm_attr = p_vl;
		    if( it_val )
		    {
		        string p_attr = TSYS::strSepParse(tmpl->tpl.at().attr(lnk(i_l).io_id).val,1,'|');
		        if( dynamic_cast<TValue &>(SYS->nodeAt(p_vl,0,'.').at()).vlPresent(p_attr) )
		        { lnk(i_l).prm_attr+= "."+p_attr; only_no_set = false; }
		        else no_set+=p_attr+",";
		    }
		}
            if( it_val )
	    {
	        if( only_no_set )	throw TError(nodePath().c_str(),mod->I18N("Parameter have not only atributes!"));
	        else if( no_set.size() )throw TError(nodePath().c_str(),mod->I18N("Parameter have not atributes: %s !"),no_set.c_str());
		mode( (TMdPrm::Mode)m_mode, m_prm );
	    }
	}
    }	
    else if( a_path.substr(0,12) == "/cfg/prm/pl_" && mode() == TMdPrm::Template && ctrChkNode(opt) )
    {
        string l_prm = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
        int c_lv = 0;
        string c_path = "";
        opt->childAdd("el")->text(c_path);
        while(TSYS::strSepParse(l_prm,c_lv,'.').size())
        {
            if( c_lv ) c_path+=".";
            c_path = c_path+TSYS::strSepParse(l_prm,c_lv,'.');
            if( !dynamic_cast<TVal *>(&SYS->nodeAt(c_path,0,'.').at()) )
                opt->childAdd("el")->text(c_path);
            c_lv++;
	}
        if(c_lv) c_path+=".";
	if( !dynamic_cast<TValue *>(&SYS->nodeAt(c_path,0,'.').at()) )
            SYS->nodeAt(c_path,0,'.').at().nodeList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
            opt->childAdd("el")->text(c_path+list[i_a]);
    }
    else if( a_path.substr(0,12) == "/cfg/prm/el_" && mode() == TMdPrm::Template )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
        {
            int i_io = atoi(a_path.substr(12).c_str());
            int attr_id = tmpl->tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
    	    if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link )
		opt->text(lnk(lnkId(i_io)).prm_attr);
	    else if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
		opt->text(tmpl->val.getS(i_io));
        }
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
        {
    	    int i_io = atoi(a_path.substr(12).c_str());
            int attr_id = tmpl->tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
    	    if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::Link )
            {
                lnk(lnkId(i_io)).prm_attr = opt->text();
                mode( (TMdPrm::Mode)m_mode, m_prm );
            }
            else if( tmpl->tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
		tmpl->val.setS(i_io,opt->text());
        }
    }
    else if( a_path.substr(0,12) == "/cfg/prm/ls_" && mode() == TMdPrm::Template && ctrChkNode(opt) )
    {
	string l_prm = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
        int c_lv = 0;
        string c_path = "";
    	opt->childAdd("el")->text(c_path);
        while(TSYS::strSepParse(l_prm,c_lv,'.').size())
        {
    	    if( c_lv ) c_path+=".";
	    c_path = c_path+TSYS::strSepParse(l_prm,c_lv,'.');
	    opt->childAdd("el")->text(c_path);
	    c_lv++;
        }
        if(c_lv) c_path+=".";
        SYS->nodeAt(l_prm,0,'.').at().nodeList(list);
        for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	    opt->childAdd("el")->text(c_path+list[i_a]);
    }
    else TParamContr::cntrCmdProc(opt);
}

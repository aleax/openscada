
//OpenSCADA system module DAQ.BlockCalc file: virtual.cpp
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

#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>

#include <terror.h>
#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>
#include <tconfig.h>
#include <tvalue.h>
#include <ttiparam.h>
#include "virtual.h"

//============ Modul info! =====================================================
#define MOD_ID      "BlockCalc"
#define MOD_NAME    "Block based calculator"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.9.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow block based calculator."
#define LICENSE     "GPL"
//==============================================================================

Virtual::TipContr *Virtual::mod;

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
	Virtual::TipContr *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = Virtual::mod = new Virtual::TipContr( source );

	return ( self_addr );
    }
}

using namespace Virtual;

//======================================================================
//==== TipContr ======================================================== 
//======================================================================

TipContr::TipContr( string name ) 
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

TipContr::~TipContr()
{    
    nodeDelAll();
}

string TipContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
        "======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
    return(buf);
}

void TipContr::modLoad()
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
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
}

void TipContr::postEnable( int flag )
{    
    TModule::postEnable( flag );
    
    //Controllers BD structure
    fldAdd( new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30","system") );
    fldAdd( new TFld("BLOCK_SH",_("Block's table"),TFld::String,TFld::NoFlag,"30","block") );	
    fldAdd( new TFld("PERIOD",_("Calc period (ms)"),TFld::Integer,TFld::NoFlag,"5","1000","0;10000") );
    fldAdd( new TFld("PRIOR",_("Calc task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("PER_DB",_("Sync db period (s)"),TFld::Integer,TCfg::Prevent,"5","0","0;3600") );
    fldAdd( new TFld("ITER",_("Iteration number into calc period"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );
    
    //Add parameter types
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("BLK",_("Block"),TFld::String,TCfg::NoVal,"10") );
    tpPrmAt(t_prm).fldAdd( new TFld("IO",_("IOs(Sep - ';')"),TFld::String,TCfg::NoVal,"50") );
    
    //Blok's db structure
    blk_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"10") );
    blk_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"20") );
    blk_el.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::NoFlag,"100") );
    blk_el.fldAdd( new TFld("FUNC",_("Function"),TFld::String,TFld::NoFlag,"75") );
    blk_el.fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,TFld::NoFlag,"1","0") );
    blk_el.fldAdd( new TFld("PROC",_("To process"),TFld::Boolean,TFld::NoFlag,"1","0") );
    
    //IO blok's db structure
    blkio_el.fldAdd( new TFld("BLK_ID",_("Blok's ID"),TFld::String,TCfg::Key,"10") );
    blkio_el.fldAdd( new TFld("ID",_("IO ID"),TFld::String,TCfg::Key,"10") );
    blkio_el.fldAdd( new TFld("TLNK",_("Link's type"),TFld::Integer,TFld::NoFlag,"2") );
    blkio_el.fldAdd( new TFld("LNK",_("Link"),TFld::String,TFld::NoFlag,"50") );
    blkio_el.fldAdd( new TFld("VAL",_("Link's value"),TFld::String,TFld::NoFlag,"20") );
}

void TipContr::preDisable(int flag)
{
    vector<string> lst;
    list(lst);
    for(int i_cnt = 0; i_cnt < lst.size(); i_cnt++)
	if( at(lst[i_cnt]).at().startStat() )
	    at(lst[i_cnt]).at().stop( );
    for(int i_cnt = 0; i_cnt < lst.size(); i_cnt++)
	if( at(lst[i_cnt]).at().enableStat() )
    	    at(lst[i_cnt]).at().disable( );
}

TController *TipContr::ContrAttach( const string &name, const string &daq_db )
{
    return new Contr(name,daq_db,this);
}

void TipContr::loadBD()
{
    
}

void TipContr::saveBD()
{

}

void TipContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TTipDAQ::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else TTipDAQ::cntrCmdProc(opt);
}

//======================================================================
//==== Contr 
//======================================================================
Contr::Contr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    ::TController(name_c, daq_db, cfgelem), prc_st(false), endrun_req(false), sync_st(false), tm_calc(0.0),
    m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), m_iter(cfg("ITER").getId()), m_dbper(cfg("PER_DB").getId())
{
    cfg("PRM_BD").setS("BlckCalcPrm_"+name_c);
    cfg("BLOCK_SH").setS("BlckCalcBlcks_"+name_c);
    hd_res = ResAlloc::resCreate();
    m_bl = grpAdd("blk_");
    
    //Create sync DB timer
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = TaskDBSync;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&sncDBTm);			    
}

Contr::~Contr()
{
    timer_delete(sncDBTm);
    ResAlloc::resDelete(hd_res);
}

void Contr::postDisable(int flag)
{
    if( run_st ) stop();
    try
    {
        if( flag )
        {
	    //Delete parameter's tables
	    string wbd = DB()+"."+cfg("BLOCK_SH").getS();
	    SYS->db().at().open(wbd);	
	    SYS->db().at().close(wbd,true);
	    
	    wbd=wbd+"_io";
	    SYS->db().at().open(wbd); 	
	    SYS->db().at().close(wbd,true);
	}
    }catch(TError err)
    { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }	    
    
    TController::postDisable(flag);
}

void Contr::load( )
{
    TController::load( );
    
    loadV();
}

void Contr::save( )
{
    TController::save();

    saveV();
}

void Contr::enable_( )
{
    //Enable blocks 
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().toEnable() )
	try{ blkAt(lst[i_l]).at().enable(true); }
	catch(TError err)
	{ 
	    mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); 
	    mess_warning(nodePath().c_str(),_("Enable block <%s> error."),lst[i_l].c_str());
	}
}

void Contr::disable_( )
{
    //Disable blocks
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().enable() )
        try{ blkAt(lst[i_l]).at().enable(false); }
	    catch(TError err)
	    { 
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); 
		mess_warning(nodePath().c_str(),_("Enable block <%s> error."),lst[i_l].c_str());
	    }
}

void Contr::start_( )
{   
    //Make process all bloks
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().enable() && blkAt(lst[i_l]).at().toProcess() )	
	try{ blkAt(lst[i_l]).at().process(true); }
	catch(TError err)
	{ 
	    mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); 
	    mess_warning(nodePath().c_str(),_("Process block <%s> error."),lst[i_l].c_str());
	}
    
    //Start the request and calc data task
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
		    
        pthread_create(&calcPthr,&pthr_attr,Contr::Task,this);
        pthread_attr_destroy(&pthr_attr);
        if( TSYS::eventWait(prc_st, true, nodePath()+"start",5) )
            throw TError(nodePath().c_str(),_("Acquisition task no started!"));
    }	    

    //Start interval timer for periodic thread creating of DB syncing
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_dbper;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(sncDBTm, 0, &itval, NULL);
}

void Contr::stop_( )
{  
    //Stop the request and calc data task
    if( prc_st )
    {
	endrun_req = true;
        pthread_kill( calcPthr, SIGALRM );
        if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
            throw TError(nodePath().c_str(),_("Acquisition task no stoped!"));
        pthread_join( calcPthr, NULL );
    }

    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(sncDBTm, 0, &itval, NULL);
    if( TSYS::eventWait( sync_st, false, nodePath()+"sync_stop",5) )
        throw TError(nodePath().c_str(),_("Controller sync DB no stoped!"));
	
    //Make deprocess all blocks
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().process() )
    	    blkAt(lst[i_l]).at().process(false);
} 

void Contr::loadV( )
{
    TConfig c_el(&mod->blockE());
    c_el.cfgViewAll(false);	    
    string bd = DB()+"."+cfg("BLOCK_SH").getS();
    
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(bd,mod->nodePath()+cfg("BLOCK_SH").getS(),fld_cnt++,c_el) )
    {
        string id = c_el.cfg("ID").getS();
        if( !chldPresent(m_bl,id) )
        {
            blkAdd(id);
            ((TConfig &)blkAt(id).at()) = c_el;
            //if( blkAt(id).at().toEnable() ) blkAt(id).at().enable(true);
        }
	blkAt(id).at().load();
	c_el.cfg("ID").setS("");
    }
}

void Contr::saveV( )
{
    vector<string> lst;
	
    // Save messages bd
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        blkAt(lst[i_l]).at().save();
}

void Contr::freeV( )
{
    vector<string> lst;
	
    // Save messages bd
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )    
    {
	try{ blkDel(lst[i_l]); }
	catch(TError){ throw TError(nodePath().c_str(),_("Can't delete block <%s>."),lst[i_l].c_str()); }
    }
}

void *Contr::Task( void *icontr )
{
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    Contr &cntr = *(Contr *)icontr;    
    
    cntr.endrun_req = false;
    cntr.prc_st = true;
    
    bool is_start = true;
    bool is_stop  = false;
    
    while(true)
    {   
	//Check calk time
        unsigned long long t_cnt = SYS->shrtCnt();
	
	ResAlloc::resRequestR(cntr.hd_res);
	for(unsigned i_it = 0; i_it < cntr.m_iter; i_it++)
	    for(unsigned i_blk = 0; i_blk < cntr.clc_blks.size(); i_blk++)
	    {
	        try{ cntr.clc_blks[i_blk].at().calc(is_start,is_stop); }
	        catch(TError err)
	        { 
	    	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    string blck = cntr.clc_blks[i_blk].at().id();
		    mess_err(cntr.nodePath().c_str(),_("Block <%s> calc error."),blck.c_str());
		    if( cntr.clc_blks[i_blk].at().errCnt() < 10 ) continue;
		    ResAlloc::resReleaseR(cntr.hd_res);
		    mess_err(cntr.nodePath().c_str(),_("Block <%s> stoped."),blck.c_str());
		    cntr.blkAt(blck).at().process(false);			
		    ResAlloc::resRequestR(cntr.hd_res);
		}
	    }
	ResAlloc::resReleaseR(cntr.hd_res);	
	
	cntr.tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
	
	if(is_stop) break;
		
	//Calc next work time and sleep
        clock_gettime(CLOCK_REALTIME,&get_tm);
        work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/((long long)cntr.m_per*1000000) + 1)*(long long)cntr.m_per*1000000;
	if(work_tm == last_tm)	work_tm+=(long long)cntr.m_per*1000000;	//Fix early call
	last_tm = work_tm;
        get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
	clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);
	
	if(cntr.endrun_req) is_stop = true;
        is_start = false;
    }
    
    cntr.prc_st = false;
    
    return NULL;
}

void Contr::TaskDBSync(union sigval obj)
{
    Contr *cntr = (Contr *)obj.sival_ptr;
    if( cntr->sync_st )  return;
    cntr->sync_st = true;
    
    try{ cntr->saveV( ); }
    catch(TError err) 
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); 
	mess_err(cntr->nodePath().c_str(),_("Block save error."));
    }
    
    cntr->sync_st = false;
}

TParamContr *Contr::ParamAttach( const string &name, int type )
{
    return new Prm(name,&owner().tpPrmAt(type));
}

void Contr::blkAdd( const string &iid )
{
    if( chldPresent(m_bl,iid) ) return;
    chldAdd(m_bl,new Block( iid, this ));
}

void Contr::blkProc( const string & id, bool val )
{
    unsigned i_blk;

    ResAlloc res(hd_res,true); 
    for( i_blk = 0; i_blk < clc_blks.size(); i_blk++)
        if( clc_blks[i_blk].at().id() == id ) break;		
    
    if( val && i_blk >= clc_blks.size() )
	clc_blks.push_back(blkAt(id));
    if( !val && i_blk < clc_blks.size()	)
	clc_blks.erase(clc_blks.begin()+i_blk);
}	

void Contr::copyBlock( const string &from_id, const string &cntr_id, const string &to_id, const string &to_name )
{
    string contr = cntr_id;
    string toid = to_id;
    string toname = to_name;
    
    if( !blkPresent(from_id) )
        throw TError(nodePath().c_str(),_("Block <%s> no present."),from_id.c_str());

    if( !contr.size() )	contr = id();
    if( !toid.size() )  toid  = blkAt(from_id).at().id();
    if( !toname.size() )toname = blkAt(from_id).at().name();
    
    if( !mod->present(contr) )
        throw TError(nodePath().c_str(),_("Controller <%s> no present."),contr.c_str());
    if( mod->at(contr).at().blkPresent(toid) )
        throw TError(nodePath().c_str(),_("Block <%s:%s> already present."),contr.c_str(),toid.c_str());
	
    //Make new function
    mod->at(contr).at().blkAdd(toid);
    mod->at(contr).at().blkAt(toid).at() = blkAt(from_id).at();
    mod->at(contr).at().blkAt(toid).at().name(to_name);
}

bool Contr::cfgChange( TCfg &cfg )
{
    if( startStat() )
    {
	struct itimerspec itval;
	if( cfg.fld().name() == "PER_DB" )
	{
	    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_dbper;
    	    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
	    timer_settime(sncDBTm, 0, &itval, NULL);
	}
    }
    return true;
}

void Contr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TController::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/blk_",_("Block"),0440,"root","root",1,"list","/scheme/sch");
	if(ctrMkNode("area",opt,-1,"/scheme",_("Blocks scheme")))
	{
	    ctrMkNode("fld",opt,-1,"/scheme/ctm",_("Calk time (usek)"),0444,"root","root",1,"tp","real");
	    ctrMkNode("list",opt,-1,"/scheme/sch",_("Blocks"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","blk_");
	    if(ctrMkNode("comm",opt,-1,"/scheme/copy",_("Copy block"),0440))
	    {
		ctrMkNode("fld",opt,-1,"/scheme/copy/blk",_("Block"),0660,"root","root",4,"tp","str","idm","1","dest","select","select","/scheme/ls_blck");
    		ctrMkNode("fld",opt,-1,"/scheme/copy/cntr",_("To controller"),0660,"root","root",4,"tp","str","idm","1","dest","select","select","/scheme/ls_cntr");
    		ctrMkNode("fld",opt,-1,"/scheme/copy/id",_("Name as"),0660,"root","root",2,"tp","str","len","10");
    		ctrMkNode("fld",opt,-1,"/scheme/copy/nm","",0660,"root","root",1,"tp","str");
	    }
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/scheme/ctm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_calc));
    else if( a_path == "/scheme/sch" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> lst;
	    blkList(lst);
	    for( unsigned i_f=0; i_f < lst.size(); i_f++ )
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(blkAt(lst[i_f]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )		
	{
	    blkAdd(opt->attr("id"));
	    blkAt(opt->attr("id")).at().name(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	chldDel(m_bl,opt->attr("id"),-1,1);
    }	
    else if( a_path == "/scheme/ls_blck" && ctrChkNode(opt) )
    {
	vector<string> lst;
        blkList(lst);
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
            opt->childAdd("el")->setAttr("id",lst[i_f])->setText(blkAt(lst[i_f]).at().name());
    }
    else if( a_path == "/scheme/ls_cntr" && ctrChkNode(opt) )
    {
	vector<string> lst;
        opt->childAdd("el")->setAttr("id","")->setText("");
        mod->list(lst);
    	for( unsigned i_a=0; i_a < lst.size(); i_a++ )
            opt->childAdd("el")->setAttr("id",lst[i_a])->setText(mod->at(lst[i_a]).at().name());
    }
    else if( a_path == "/scheme/copy" && ctrChkNode(opt,"set",0440) )
        copyBlock(ctrId(opt,"blk")->text(),ctrId(opt,"cntr")->text(), ctrId(opt,"id")->text(), ctrId(opt,"nm")->text());
    else TController::cntrCmdProc(opt);
}	

//======================================================================
//==== Prm 
//======================================================================
Prm::Prm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), v_el(name)
{

}

Prm::~Prm()
{
    nodeDelAll();
}

void Prm::postEnable( int flag )
{
    TParamContr::postEnable( flag );
    if(!vlElemPresent(&v_el))	vlElemAtt(&v_el);
}

void Prm::enable()
{
    if( enableStat() )  return;
    
    m_blck = cfg("BLK").getS();
    
    if( !((Contr &)owner()).blkPresent(m_blck) )
        throw TError(nodePath().c_str(),_("Block not connected."));
    
    //Init elements
    int io_cnt = 0;
    while(TSYS::strSepParse(cfg("IO").getS(),io_cnt,';').size())
    {
	string mio = TSYS::strSepParse(cfg("IO").getS(),io_cnt,';');
        unsigned flg = TVal::DirWrite|TVal::DirRead;
        TFld::Type    tp  = TFld::String;
        int           io_id = ((Contr &)owner()).blkAt(m_blck).at().ioId(mio);
        if(io_id >= 0)
	{
	    //if( ((Contr &)owner()).blkAt(m_blck).at().ioMode(io_id) != IO::Input )
	    //	flg |= Fld::NoWrite;
	    switch( ((Contr &)owner()).blkAt(m_blck).at().ioType(io_id) )
	    {
		case IO::String: 	tp = TFld::String; 	break;
		case IO::Integer:	tp = TFld::Integer;	break;
		case IO::Real:		tp = TFld::Real;	break;
		case IO::Boolean:	tp = TFld::Boolean;	break;
	    }
	    if( !v_el.fldPresent(mio) || 
		v_el.fldAt(v_el.fldId(mio)).type() != tp || 
		v_el.fldAt(v_el.fldId(mio)).flg() != flg )
	    {
		if(v_el.fldPresent(mio)) v_el.fldDel(v_el.fldId(mio));
		v_el.fldAdd( new TFld(mio.c_str(),((Contr &)owner()).blkAt(m_blck).at().func()->io(io_id)->name().c_str(),tp,flg) );
    	    }
	}
	io_cnt++;
    }
    
    //Check and delete no used fields
    for(int i_fld = 0; i_fld < v_el.fldSize(); i_fld++)
    {
        string fel;
        int io_cnt = 0;
        while( (fel = TSYS::strSepParse(cfg("IO").getS(),io_cnt++,';')).size() )
    	    if( fel == v_el.fldAt(i_fld).name() ) break;
        if( !fel.size() )
        {
            v_el.fldDel(i_fld);
            i_fld--;
	}
    }	
    
    TParamContr::enable();
}

void Prm::disable()
{
    if( !enableStat() )  return;
    
    TParamContr::disable();
}				

void Prm::vlSet( TVal &val )
{
    if( !enableStat() )	return;
    try
    {
        int io_id = ((Contr &)owner()).blkAt(m_blck).at().ioId(val.name());
	if( io_id < 0 ) disable();
	else
	{
	    switch(val.fld().type())
	    {
		case TFld::String: 		    
		    ((Contr &)owner()).blkAt(m_blck).at().setS(io_id,val.getS(0,true));
		    break;
		case TFld::Integer:
		    ((Contr &)owner()).blkAt(m_blck).at().setI(io_id,val.getI(0,true));
		    break;
		case TFld::Real:
		    ((Contr &)owner()).blkAt(m_blck).at().setR(io_id,val.getR(0,true));
		    break;
		case TFld::Boolean:
		    ((Contr &)owner()).blkAt(m_blck).at().setB(io_id,val.getB(0,true));
		    break;
	    }
	}
    }catch(TError err) { disable(); }
}

void Prm::vlGet( TVal &val )
{
    if( val.name() == "err" )
    {    
	if( !owner().startStat() )             
	    val.setS(_("2:Controller stoped"),0,true);
	else if( !enableStat() )   		   
	    val.setS(_("1:Parameter disabled"),0,true);
	else if( !owner().blkPresent(m_blck) ) 
	    val.setS(_("3:Block no present"),0,true);
	else if( !owner().blkAt(m_blck).at().enable() )
    	    val.setS(_("4:Block disabled"),0,true);
	else if( !owner().blkAt(m_blck).at().process() )
    	    val.setS(_("5:Block no process"),0,true);
	else val.setS("0",0,true);	
	return;
    }

    try
    {
	if( !enableStat() ) return;
	int io_id = owner().blkAt(m_blck).at().ioId(val.name());
	if( io_id < 0 )	disable();
	else
	    switch(val.fld().type())
	    {
		case TFld::String: 
		    val.setS(enableStat()?owner().blkAt(m_blck).at().getS(io_id):EVAL_STR,0,true); break;
		case TFld::Integer:
		    val.setI(enableStat()?owner().blkAt(m_blck).at().getI(io_id):EVAL_INT,0,true); break;
		case TFld::Real:
		    val.setR(enableStat()?owner().blkAt(m_blck).at().getR(io_id):EVAL_REAL,0,true);break;
		case TFld::Boolean:
		    val.setB(enableStat()?owner().blkAt(m_blck).at().getB(io_id):EVAL_BOOL,0,true);break;
	    }
    }catch(TError err) { disable(); }
}

void Prm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/BLK",cfg("BLK").fld().descr(),0660,"root","root",3,"tp","str","dest","select","select","/prm/cfg/BLK_list");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");	    
    if( a_path == "/prm/cfg/BLK_list" && ctrChkNode(opt) )
    {
        vector<string> list;
        ((Contr &)owner()).blkList(list);
        for( unsigned i_f=0; i_f < list.size(); i_f++ )
    	    opt->childAdd("el")->setAttr("id",list[i_f])->setText(owner().blkAt(list[i_f]).at().name());
    }
    else TParamContr::cntrCmdProc(opt);
}

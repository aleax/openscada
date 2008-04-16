
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
#include <tmess.h>
#include <tconfig.h>
#include <tvalue.h>
#include <ttiparam.h>
#include "virtual.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID      "BlockCalc"
#define MOD_NAME    "Block based calculator"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "1.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow block based calculator."
#define LICENSE     "GPL"
//************************************************

Virtual::TipContr *Virtual::mod;

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
	    return new Virtual::TipContr( source );
	return NULL;
    }
}

using namespace Virtual;

//************************************************
//* TipContr - BlockCalc type controller         *
//************************************************ 
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
    
    mod		= this;
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
    tpPrmAt(t_prm).fldAdd( new TFld("IO",_("Blocks' IOs"),TFld::String,TFld::FullText|TCfg::NoVal,"200") );
    
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

void TipContr::copy( const string &src, const string &dst )
{
    string s_el = TSYS::strSepParse(src,1,'.');
    string d_el = TSYS::strSepParse(dst,1,'.');
    string t_el;
    vector<string>  el_ls, tmp_ls;
    //- Check parameters -
    //if( !s_el.empty() && d_el.empty() ) d_el = s_el;
    if( !s_el.empty() && !d_el.empty() && ( s_el.size() <= 4 || d_el.size() <= 4 ||
	    (s_el.substr(0,4) != "prm_" && s_el.substr(0,4) != "blk_") || 
	    s_el.substr(0,4) != d_el.substr(0,4) ) )
        throw TError(nodePath().c_str(),_("Parameters record error."));	

    //- Connect to source and destination DB -
    AutoHD<Contr> s_cntr = at(TSYS::strSepParse(src,0,'.'));
    AutoHD<Contr> d_cntr = at(TSYS::strSepParse(dst,0,'.'));
    //- Controllers list prepare -
    if( s_el.empty() || s_el == "blk_*" )
    {
	s_cntr.at().blkList(tmp_ls);
	for( int i_el = 0; i_el < tmp_ls.size(); i_el++ )
	    el_ls.push_back("blk_"+tmp_ls[i_el]);
    }
    if( s_el.empty() || s_el == "prm_*" )
    {
	s_cntr.at().list(tmp_ls);
	for( int i_el = 0; i_el < tmp_ls.size(); i_el++ )
	    el_ls.push_back("prm_"+tmp_ls[i_el]);
    }
    if( el_ls.empty() ) el_ls.push_back(s_el);
    if( el_ls.size() > 1 && !d_el.empty() )
        throw TError(nodePath().c_str(),_("Copy several elements to one element not allow."));
    //- Controllers list process -
    for( int i_l = 0; i_l < el_ls.size(); i_l++ )
    {
	s_el = el_ls[i_l].substr(4);
	t_el = d_el.empty()?s_el:d_el.substr(4);
	if( el_ls[i_l].substr(0,4) == "blk_" )
	{
	    if( !d_cntr.at().blkPresent(t_el) )	d_cntr.at().blkAdd(t_el);
	    d_cntr.at().blkAt(t_el).at() = s_cntr.at().blkAt(s_el).at();
	}
	else if( el_ls[i_l].substr(0,4) == "prm_" )
	{
	    if( !d_cntr.at().present(t_el) )	d_cntr.at().add(t_el,tpPrmToId("std"));
	    d_cntr.at().at(t_el).at() = s_cntr.at().at(s_el).at();
	}
    }
} 

void TipContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TTipDAQ::cntrCmdProc(opt);
	if( ctrMkNode("area",opt,1,"/copy",_("Copy")) && 
		ctrMkNode("comm",opt,-1,"/copy/copy",_("Copy controller/block/parameter"),0660) )
	{
	    ctrMkNode("fld",opt,-1,"/copy/copy/src",_("Source"),0660,"root","root",3,"tp","str","dest","sel_ed","select","/copy/cntrls");
    	    ctrMkNode("fld",opt,-1,"/copy/copy/dst",_("Destination"),0660,"root","root",3,"tp","str","dest","sel_ed","select","/copy/cntrls");
	}		
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/copy/copy" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	copy( ctrId(opt,"src")->text(), ctrId(opt,"dst")->text() );
    else if( a_path == "/copy/cntrls" && ctrChkNode(opt) )
    {
	vector<string> lst;
        list(lst);
    	for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	{
            opt->childAdd("el")->setText(lst[i_a]);
            opt->childAdd("el")->setText(lst[i_a]+".prm_*");
            opt->childAdd("el")->setText(lst[i_a]+".blk_*");
	}
    }    
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else TTipDAQ::cntrCmdProc(opt);
}

//************************************************
//* Contr - Blocks and parameters container      *
//************************************************ 
Contr::Contr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    ::TController(name_c, daq_db, cfgelem), prc_st(false), endrun_req(false), sync_st(false), tm_calc(0.0),
    m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), m_iter(cfg("ITER").getId()), m_dbper(cfg("PER_DB").getId())
{
    cfg("PRM_BD").setS("BlckCalcPrm_"+name_c);
    cfg("BLOCK_SH").setS("BlckCalcBlcks_"+name_c);
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
	try{ blkAt(lst[i_l]).at().setEnable(true); }
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
        try{ blkAt(lst[i_l]).at().setEnable(false); }
	    catch(TError err)
	    { 
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); 
		mess_warning(nodePath().c_str(),_("Enable block <%s> error."),lst[i_l].c_str());
	    }
}

void Contr::start_( )
{   
    //- Make process all bloks -
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().enable() && blkAt(lst[i_l]).at().toProcess() )	
	try{ blkAt(lst[i_l]).at().setProcess(true); }
	catch(TError err)
	{ 
	    mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); 
	    mess_warning(nodePath().c_str(),_("Process block <%s> error."),lst[i_l].c_str());
	}
    
    //- Start the request and calc data task -
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

    //- Start interval timer for periodic thread creating of DB syncing -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_dbper;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(sncDBTm, 0, &itval, NULL);
}

void Contr::stop_( )
{  
    //- Stop the request and calc data task -
    if( prc_st )
    {
	endrun_req = true;
        pthread_kill( calcPthr, SIGALRM );
        if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
            throw TError(nodePath().c_str(),_("Acquisition task no stoped!"));
        pthread_join( calcPthr, NULL );
    }

    //- Stop interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(sncDBTm, 0, &itval, NULL);
    if( TSYS::eventWait( sync_st, false, nodePath()+"sync_stop",5) )
        throw TError(nodePath().c_str(),_("Controller sync DB no stoped!"));
	
    //- Make deprocess all blocks -
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().process() )
    	    blkAt(lst[i_l]).at().setProcess(false);
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
	
	cntr.hd_res.resRequestR( );
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
		    cntr.hd_res.resReleaseR( );
		    mess_err(cntr.nodePath().c_str(),_("Block <%s> stoped."),blck.c_str());
		    cntr.blkAt(blck).at().setProcess(false);			
		    cntr.hd_res.resRequestR( );
		}
	    }
	cntr.hd_res.resReleaseR( );
	
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
	ctrMkNode("grp",opt,-1,"/br/blk_",_("Block"),0664,"root","root",1,"s_com","add,del");
	if(ctrMkNode("area",opt,-1,"/scheme",_("Blocks scheme")))
	{
	    ctrMkNode("fld",opt,-1,"/scheme/ctm",_("Calk time (usek)"),0444,"root","root",1,"tp","real");
	    ctrMkNode("list",opt,-1,"/scheme/sch",_("Blocks"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","blk_");
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/scheme/ctm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_calc,6));
    else if( a_path == "/br/blk_" || a_path == "/scheme/sch" )
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
	    blkAt(opt->attr("id")).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	chldDel(m_bl,opt->attr("id"),-1,1);
    }
    else TController::cntrCmdProc(opt);
}	

//************************************************
//* Prm - parameters for access to data          *
//*       of calced blocks                       *
//************************************************  
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

    //- Check and delete no used fields -
    for(int i_fld = 0; i_fld < v_el.fldSize(); i_fld++)
    {
        string fel;
        for( int io_off = 0; (fel=TSYS::strSepParse(cfg("IO").getS(),0,'\n',&io_off)).size(); )
    	    if( TSYS::strSepParse(fel,0,':') == v_el.fldAt(i_fld).reserve() ) break;
        if( fel.empty() )	
	{
            v_el.fldDel(i_fld);
            i_fld--;
	}
    }	

    //- Init elements -
    AutoHD<Block> blk;
    int io;
    string mio, ioaddr, ioblk, ioid, aid, anm;
    for( int io_off = 0; (mio=TSYS::strSepParse(cfg("IO").getS(),0,'\n',&io_off)).size(); )
    {
	ioaddr = TSYS::strSepParse(mio,0,':');
	ioblk  = TSYS::strSepParse(ioaddr,0,'.');
	ioid   = TSYS::strSepParse(ioaddr,1,'.');
	aid    = TSYS::strSepParse(mio,1,':');
	anm    = TSYS::strSepParse(mio,2,':');
	if( aid.empty() ) aid = ioblk+"_"+ioid;
	if( !((Contr&)owner()).blkPresent(ioblk) ) continue;
	blk = ((Contr&)owner()).blkAt(ioblk);
	if( (io=blk.at().ioId(ioid)) < 0 )	continue;
	
        unsigned 	flg    = TVal::DirWrite|TVal::DirRead;
        TFld::Type	tp     = TFld::String;
	switch( blk.at().ioType(io) )
	{
	    case IO::String: 	tp = TFld::String; 	break;
	    case IO::Integer:	tp = TFld::Integer;	break;
	    case IO::Real:	tp = TFld::Real;	break;
	    case IO::Boolean:	tp = TFld::Boolean;	break;
	}
	if( !v_el.fldPresent(aid) || v_el.fldAt(v_el.fldId(aid)).type() != tp || v_el.fldAt(v_el.fldId(aid)).flg() != flg )
	{
	    if(v_el.fldPresent(aid)) v_el.fldDel(v_el.fldId(aid));
	    v_el.fldAdd( new TFld(aid.c_str(),anm.empty() ? blk.at().func()->io(io)->name().c_str() : anm.c_str(),tp,flg,"","","","",ioaddr.c_str()) );
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
    	AutoHD<Block> blk = ((Contr &)owner()).blkAt(TSYS::strSepParse(val.fld().reserve(),0,'.'));
        int io_id = blk.at().ioId(TSYS::strSepParse(val.fld().reserve(),1,'.'));
	if( io_id < 0 ) disable();
	else
	    switch(val.fld().type())
	    {
		case TFld::String:	blk.at().setS(io_id,val.getS(0,true));	break;
		case TFld::Integer:	blk.at().setI(io_id,val.getI(0,true));	break;
		case TFld::Real:	blk.at().setR(io_id,val.getR(0,true));	break;
		case TFld::Boolean:	blk.at().setB(io_id,val.getB(0,true));	break;
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
	else val.setS("0",0,true);	
	return;
    }

    try
    {
	if( !enableStat() ) return;	
     	AutoHD<Block> blk = ((Contr &)owner()).blkAt(TSYS::strSepParse(val.fld().reserve(),0,'.'));
        int io_id = blk.at().ioId(TSYS::strSepParse(val.fld().reserve(),1,'.'));        
	if( io_id < 0 )	disable();
	else
	    switch(val.fld().type())
	    {
		case TFld::String: 	val.setS( enableStat() ? blk.at().getS(io_id) : EVAL_STR, 0, true );	break;
		case TFld::Integer:	val.setI( enableStat() ? blk.at().getI(io_id) : EVAL_INT, 0, true ); 	break;
		case TFld::Real:	val.setR( enableStat() ? blk.at().getR(io_id) : EVAL_REAL, 0, true );	break;
		case TFld::Boolean:	val.setB( enableStat() ? blk.at().getB(io_id) : EVAL_BOOL, 0, true );	break;
	    }
    }catch(TError err) { disable(); }
}

void Prm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
	val.arch().at().setSrcMode(TVArchive::ActiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(1000000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( false );
}

/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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
#include <tparam.h>
#include <tparams.h>
#include <ttiparam.h>
#include "virtual.h"

//============ Modul info! =====================================================
#define MOD_ID      "BlockCalc"
#define MOD_NAME    "Block based calculator"
#define MOD_TYPE    "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.7.1"
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

    snprintf(buf,sizeof(buf),I18N(
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

void TipContr::postEnable()
{    
    TModule::postEnable();
    
    //Controllers BD structure
    fldAdd( new TFld("PRM_BD",I18N("Parameters table"),TFld::String,0,"30","system") );
    fldAdd( new TFld("BLOCK_SH",I18N("Block's table"),TFld::String,0,"30","block") );	
    fldAdd( new TFld("PERIOD",I18N("Calc period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    fldAdd( new TFld("ITER",I18N("Iteration number into calc period"),TFld::Dec,0,"2","1","0;99") );
    
    //Add parameter types
    int t_prm = tpParmAdd("std","PRM_BD",I18N("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("BLK",I18N("Block"),TFld::String,FLD_NOVAL,"10") );
    tpPrmAt(t_prm).fldAdd( new TFld("IO",I18N("IOs(Sep - ';')"),TFld::String,FLD_NOVAL,"50") );
    
    //Blok's db structure
    blk_el.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"10") );
    blk_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"20") );
    blk_el.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"100") );
    blk_el.fldAdd( new TFld("FUNC",Mess->I18N("Function"),TFld::String,0,"75") );
    blk_el.fldAdd( new TFld("EN",Mess->I18N("To enable"),TFld::Bool,0,"1","false") );
    blk_el.fldAdd( new TFld("PROC",Mess->I18N("To process"),TFld::Bool,0,"1","false") );
    
    //IO blok's db structure
    blkio_el.fldAdd( new TFld("BLK_ID",Mess->I18N("Blok's ID"),TFld::String,FLD_KEY,"10") );
    blkio_el.fldAdd( new TFld("ID",Mess->I18N("IO ID"),TFld::String,FLD_KEY,"10") );
    blkio_el.fldAdd( new TFld("TLNK",Mess->I18N("Link's type"),TFld::Dec,0,"2") );
    blkio_el.fldAdd( new TFld("LNK",Mess->I18N("Link"),TFld::String,0,"50") );
    blkio_el.fldAdd( new TFld("VAL",Mess->I18N("Link's value"),TFld::String,0,"20") );
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

TController *TipContr::ContrAttach( const string &name, const TBDS::SName &bd)
{
    return( new Contr(name,bd,this));
}

void TipContr::loadBD()
{
    
}

void TipContr::saveBD()
{

}

//================== Controll functions ========================
void TipContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TTipController::cntrCmd_( a_path, opt, cmd );
	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );
	else TTipController::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
	TTipController::cntrCmd_( a_path, opt, cmd );
}

//======================================================================
//==== Contr 
//======================================================================

Contr::Contr( string name_c, const TBDS::SName &bd, ::TElem *cfgelem) :
    ::TController(name_c, bd, cfgelem), endrun(false), tm_calc(0.0),
    m_per(cfg("PERIOD").getId()), m_iter(cfg("ITER").getId())
{
    cfg("PRM_BD").setS(name_c+"_prm");
    cfg("BLOCK_SH").setS(name_c+"_blocks");
    hd_res = ResAlloc::resCreate();
    m_bl = grpAdd("blk_");        
}

Contr::~Contr()
{
    ResAlloc::resDelete(hd_res);
}

void Contr::postDisable(int flag)
{
    if( run_st ) stop();
    try
    {
        if( flag )
        {
	    AutoHD<TBDS> bds = owner().owner().owner().db();
	    //Delete parameter's tables
    	    bool to_open = false;
	    if( !((TTipBD &)bds.at().modAt(BD().tp).at()).openStat(BD().bd) )
	    {
	        to_open = true;
	        ((TTipBD &)bds.at().modAt(BD().tp).at()).open(BD().bd,false);
	    }
	    ((TTipBD &)bds.at().modAt(BD().tp).at()).at(BD().bd).at().del(cfg("BLOCK_SH").getS());
	    ((TTipBD &)bds.at().modAt(BD().tp).at()).at(BD().bd).at().del(cfg("BLOCK_SH").getS()+"_io");
	    if( to_open ) ((TTipBD &)bds.at().modAt(BD().tp).at()).close(BD().bd);
	}
    }catch(TError err)
    { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }	    
    
    TController::postDisable(flag);
}

void Contr::load( )
{
    TController::load();
    
    if( en_st )	loadV();
}

void Contr::save( )
{
    TController::save();

    if( en_st ) saveV();
}

void Contr::start( )
{   
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    
    TController::start();
    
    if( !run_st ) 
    {   
	//Make process all bloks
        vector<string> lst;
	blkList(lst);
	for( int i_l = 0; i_l < lst.size(); i_l++ )
            if( blkAt(lst[i_l]).at().toEnable() )
	    try{ blkAt(lst[i_l]).at().enable(true); }
	    catch(TError err){ Mess->put(nodePath().c_str(),TMess::Warning,err.mess.c_str()); }
    	for( int i_l = 0; i_l < lst.size(); i_l++ )
    	    if( blkAt(lst[i_l]).at().toProcess() )	
	    try{ blkAt(lst[i_l]).at().process(true); }
	    catch(TError err){ Mess->put(nodePath().c_str(),TMess::Warning,err.mess.c_str()); }
        
	//Make process task
	pthread_attr_init(&pthr_attr);
	if(SYS->user() == "root")
	{
	    prior.__sched_priority=10;
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_FIFO);
	    pthread_attr_setschedparam(&pthr_attr,&prior);
	    
	    Mess->put(nodePath().c_str(),TMess::Info,mod->I18N("Start into realtime mode!"));
	}
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),mod->I18N("Controller no started!"));
	    
	//Enable parameters
        vector<string> prm_list;
        list(prm_list);
        for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
    	    if( at(prm_list[i_prm]).at().toEnable() )
	    try{ at(prm_list[i_prm]).at().enable(); }
	    catch(TError err){ Mess->put(nodePath().c_str(),TMess::Warning,err.mess.c_str()); }
    }	
}

void Contr::stop( )
{  
    TController::stop();

    //Make process all bloks
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        if( blkAt(lst[i_l]).at().process() )
    	    blkAt(lst[i_l]).at().process(false);					

    if( run_st )
    {
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
    	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
    	    throw TError(nodePath().c_str(),mod->I18N("Controller no stoped!"));
	pthread_join(pthr_tsk, NULL);	
    }
} 

void Contr::enable_( )
{
    try
    { 
	loadV( );
	 
	vector<string> lst;
        blkList(lst);
        for( int i_l = 0; i_l < lst.size(); i_l++ )
            if( blkAt(lst[i_l]).at().toEnable() )
                blkAt(lst[i_l]).at().enable(true);
    }
    catch(TError err)
    { 
	Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Warning,mod->I18N("Error load blocks."));
    }
}

void Contr::disable_( )
{
    try{ freeV( ); }
    catch(TError err) { enable_( ); throw; }
}

void Contr::loadV( )
{
    TConfig c_el(&mod->blockE());	    
    TBDS::SName bd = BD();
    bd.tbl = cfg("BLOCK_SH").getS();
    
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(bd,mod->nodePath()+bd.tbl,fld_cnt++,c_el) )
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
	catch(TError){ throw TError(nodePath().c_str(),mod->I18N("Can't delete block <%s>."),lst[i_l].c_str()); }
    }
}

void *Contr::Task(void *contr)
{
    struct itimerval mytim;             //Interval timer
    Contr *cntr = (Contr *)contr;

#if OSC_DEBUG
    Mess->put(cntr->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif	

    try
    {
	if(cntr->m_per == 0) return(NULL);
	if(cntr->m_iter <= 0) cntr->m_iter = 1; 

	//Start interval timer
	mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->m_per*1000;
	mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->m_per*1000;
	setitimer(ITIMER_REAL,&mytim,NULL);
    
	cntr->run_st = true;  
	cntr->endrun = false;
	
	while( !cntr->endrun )
	{
	    //Check calk time
            unsigned long long t_cnt = SYS->shrtCnt();
	
	    ResAlloc::resRequestR(cntr->hd_res);
	    for(unsigned i_it = 0; i_it < cntr->m_iter; i_it++)
		for(unsigned i_blk = 0; i_blk < cntr->clc_blks.size(); i_blk++)
		{
		    try{ cntr->clc_blks[i_blk].at().calc(); }
		    catch(TError err)
		    { 
			Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str());
			if( cntr->clc_blks[i_blk].at().errCnt() < 10 ) continue;
			string blck = cntr->clc_blks[i_blk].at().id();
			ResAlloc::resReleaseR(cntr->hd_res);
			Mess->put(cntr->nodePath().c_str(),TMess::Error,mod->I18N("Block <%s> stoped."),blck.c_str());
			cntr->blkAt(blck).at().process(false);			
			ResAlloc::resRequestR(cntr->hd_res);
		    }
		}
	    ResAlloc::resReleaseR(cntr->hd_res);	
		
	    cntr->tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
	    pause();
	}
    } catch(TError err) 
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }

    cntr->clc_blks.clear();	//Clear calk blocks
    
    //Stop interval timer
    mytim.it_interval.tv_sec = mytim.it_interval.tv_usec = 0;
    mytim.it_value.tv_sec    = mytim.it_value.tv_usec    = 0;
    setitimer(ITIMER_REAL,&mytim,NULL);
    
    cntr->run_st = false;
    
    return(NULL);
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

//======================================================================
//==== Contr
//======================================================================
void Contr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TController::cntrCmd_( a_path, opt, cmd );

	ctrMkNode("area",opt,a_path.c_str(),"/scheme",owner().I18N("Blocks scheme"));
	ctrMkNode("fld",opt,a_path.c_str(),"/scheme/ctm",owner().I18N("Calk time (usek)"),0444,0,0,"real");
	ctrMkNode("list",opt,a_path.c_str(),"/scheme/sch",owner().I18N("Blocks"),0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del")->attr_("br_pref","blk_");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/scheme/ctm" )  ctrSetR( opt, tm_calc );
	else if( a_path == "/scheme/sch" )
	{
	    vector<string> list_el;
	    blkList(list_el);
	    opt->childClean();
	    for( unsigned i_f=0; i_f < list_el.size(); i_f++ )
		ctrSetS( opt, blkAt(list_el[i_f]).at().name(), list_el[i_f].c_str() );
	}
	else TController::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/scheme/sch" )
	{
	    if( opt->name() == "add" )		
	    {
		blkAdd(opt->attr("id"));
		blkAt(opt->attr("id")).at().name(opt->text());
	    }
	    else if( opt->name() == "del" )	chldDel(m_bl,opt->attr("id"),-1,1);
	}
	else TController::cntrCmd_( a_path, opt, cmd );	
    }
}

//======================================================================
//==== Prm 
//====================================================================== 

Prm::Prm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), v_el(name)
{

}

void Prm::postEnable()
{
    TParamContr::postEnable();
    vlAttElem(&v_el);
}

void Prm::preDisable(int flag)
{
    vlDetElem(&v_el);
    
    TParamContr::preDisable(flag);
}

void Prm::enable()
{
    if( enableStat() )  return;
    
    m_blck = cfg("BLK").getS();
    
    if( !((Contr &)owner()).blkPresent(m_blck) )
        throw TError(nodePath().c_str(),mod->I18N("Block not connected."));
    
    //Init elements
    int io_cnt = 0;
    while(TSYS::strSepParse(cfg("IO").getS(),io_cnt,';').size())
    {
	string mio = TSYS::strSepParse(cfg("IO").getS(),io_cnt,';');
        unsigned char flg = FLD_DWR|FLD_DRD;
        TFld::Type    tp  = TFld::String;
        int           io_id = ((Contr &)owner()).blkAt(m_blck).at().ioId(mio);
        if(io_id >= 0)
	{
	    //if( ((Contr &)owner()).blkAt(m_blck).at().ioMode(io_id) != IO::Input )
	    //	flg |= FLD_NWR;
	    switch( ((Contr &)owner()).blkAt(m_blck).at().ioType(io_id) )
	    {
		case IO::String: 	tp = TFld::String; 	break;
		case IO::Integer:	tp = TFld::Dec;		break;
		case IO::Real:		tp = TFld::Real;	break;
		case IO::Boolean:	tp = TFld::Bool;	break;    
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
		    ((Contr &)owner()).blkAt(m_blck).at().setS(io_id,val.getS(NULL,true));
		    break;
		case TFld::Dec:
		    ((Contr &)owner()).blkAt(m_blck).at().setI(io_id,val.getI(NULL,true));
		    break;
		case TFld::Real:
		    ((Contr &)owner()).blkAt(m_blck).at().setR(io_id,val.getR(NULL,true));
		    break;
		case TFld::Bool:
		    ((Contr &)owner()).blkAt(m_blck).at().setB(io_id,val.getB(NULL,true));
		    break;
	    }
	}
    }catch(TError err) { disable(); }
}

void Prm::vlGet( TVal &val )
{
    int io_id = ((Contr &)owner()).blkAt(m_blck).at().ioId(val.name());
    if( io_id < 0 )
	if( val.name() == "err" || val.name() == "err_mess" )
	{	    
	    if( !((Contr &)owner()).blkPresent(m_blck) )
	    {
		if( val.name() == "err" ) val.setB(true,NULL,true);
		else 	val.setS(mod->I18N("Block no present"),NULL,true);
	    }
	    else if( !((Contr &)owner()).blkAt(m_blck).at().enable() )
	    {
		if( val.name() == "err" ) val.setB(true,NULL,true);
                else    val.setS(mod->I18N("Block disabled"),NULL,true);
	    }
	    else if( !((Contr &)owner()).blkAt(m_blck).at().process() )
            {
                if( val.name() == "err" ) val.setB(true,NULL,true);
                else    val.setS(mod->I18N("Block no process"),NULL,true);
            }
	    else TParamContr::vlGet( val );
	    return;
	}

    if( !enableStat() )	return;
    try
    {
	if( io_id < 0 )	disable();
	else
	{
	    switch(val.fld().type())
	    {
		case TFld::String: 
		    val.setS(((Contr &)owner()).blkAt(m_blck).at().getS(io_id),NULL,true);
		    break;
		case TFld::Dec:
		    val.setI(((Contr &)owner()).blkAt(m_blck).at().getI(io_id),NULL,true);
		    break;
		case TFld::Real:
		    val.setR(((Contr &)owner()).blkAt(m_blck).at().getR(io_id),NULL,true); 
		    break;
		case TFld::Bool:
		    val.setB(((Contr &)owner()).blkAt(m_blck).at().getB(io_id),NULL,true);
		    break;
	    }
	}
    }catch(TError err) { disable(); }
}

void Prm::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
        TParamContr::cntrCmd_( a_path, opt, cmd );
		
        ctrId(opt,"/prm/cfg/BLK")->attr_("dest","select")->attr_("select","/prm/cfg/BLK_list");
    }
    else if( cmd==TCntrNode::Get )
    {
        if( a_path == "/prm/cfg/BLK_list" )
        {
            vector<string> list;
            ((Contr &)owner()).blkList(list);
            opt->childClean();
            for( unsigned i_f=0; i_f < list.size(); i_f++ )
        	ctrSetS( opt, ((Contr &)owner()).blkAt(list[i_f]).at().name(), list[i_f].c_str() );
	}
        else TParamContr::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
	TParamContr::cntrCmd_( a_path, opt, cmd );
}

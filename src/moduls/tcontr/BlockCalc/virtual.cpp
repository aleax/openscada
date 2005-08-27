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
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow block based calculator."
#define LICENSE     "GPL"
//==============================================================================

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
	    self_addr = new Virtual::TipContr( source );

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
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;    
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

void TipContr::modConnect( )
{    
    TModule::modConnect( );
    
    //Controllers BD structure
    fldAdd( new TFld("PERIOD",I18N("Calc period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    fldAdd( new TFld("ITER",I18N("Iteration number into calc period"),TFld::Dec,0,"2","1","0;99") );
    fldAdd( new TFld("BLOCK_SH",I18N("Block's table"),TFld::String,0,"10","block") );
    
    //loadCfg(elem,sizeof(elem)/sizeof(SFld));
    
    //Add parameter types
    
    //Blok's db structure
    blk_el.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"10") );
    blk_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"20") );
    blk_el.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"100") );
    blk_el.fldAdd( new TFld("FUNC_LIB",Mess->I18N("Function's library"),TFld::String,0,"10") );
    blk_el.fldAdd( new TFld("FUNC_ID",Mess->I18N("Function's id"),TFld::String,0,"10") );
    blk_el.fldAdd( new TFld("EN",Mess->I18N("To enable"),TFld::Bool,0,"1","false") );
    blk_el.fldAdd( new TFld("PROC",Mess->I18N("To process"),TFld::Bool,0,"1","false") );
    
    //IO blok's db structure
    blkio_el.fldAdd( new TFld("BLK_ID",Mess->I18N("Blok's ID"),TFld::String,FLD_KEY,"10") );
    blkio_el.fldAdd( new TFld("ID",Mess->I18N("IO ID"),TFld::String,FLD_KEY,"10") );
    blkio_el.fldAdd( new TFld("TLNK",Mess->I18N("Link's type"),TFld::Dec,0,"2") );
    blkio_el.fldAdd( new TFld("O1",Mess->I18N("Object level 1"),TFld::String,0,"20") );
    blkio_el.fldAdd( new TFld("O2",Mess->I18N("Object level 2"),TFld::String,0,"20") );
    blkio_el.fldAdd( new TFld("O3",Mess->I18N("Object level 3"),TFld::String,0,"20") );
    blkio_el.fldAdd( new TFld("VAL",Mess->I18N("Link's value"),TFld::String,0,"20") );
}

TController *TipContr::ContrAttach( const string &name, const TBDS::SName &bd)
{
    return( new Contr(name,bd,this,this));
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

AutoHD<TCntrNode> TipContr::ctrAt1( const string &br )
{
    return TTipController::ctrAt1( br );
}

//======================================================================
//==== Contr 
//======================================================================

Contr::Contr( string name_c, const TBDS::SName &bd, ::TTipController *tcntr, ::TElem *cfgelem) :
    ::TController(name_c, bd, tcntr, cfgelem), endrun(false), tm_calc(0.0),
    m_per(cfg("PERIOD").getId()), m_iter(cfg("ITER").getId())
{
    hd_res = ResAlloc::resCreate();
    m_bl = grpAdd();        
}

Contr::~Contr()
{
    if( run_st ) stop();
    ResAlloc::resDelete(hd_res);
}

void Contr::load_( )
{
    if( en_st )	loadV();
}

void Contr::save_( )
{
    if( en_st ) saveV();
}

void Contr::start_( )
{   
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    
    if( !run_st ) 
    {   
	//Make process all bloks
        vector<string> lst;
	blkList(lst);
    	for( int i_l = 0; i_l < lst.size(); i_l++ )
    	    if( blkAt(lst[i_l]).at().enable() && blkAt(lst[i_l]).at().toProcess() )
	        blkAt(lst[i_l]).at().process(true);		    
        
	//Make process task
	pthread_attr_init(&pthr_attr);
	if(SYS->user() == "root")
	{
	    prior.__sched_priority=10;
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_FIFO);
	    pthread_attr_setschedparam(&pthr_attr,&prior);
	    
	    Mess->put(nodePath().c_str(),TMess::Info,"Start into realtime mode!");
	}
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),"Controller no started!");    	    
    }	
}

void Contr::stop_( )
{  
    if( run_st )
    {
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
    	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
    	    throw TError(nodePath().c_str(),"Controller no stoped!");
	pthread_join(pthr_tsk, NULL);	
	
	//Make process all bloks
	vector<string> lst;
	blkList(lst);
	for( int i_l = 0; i_l < lst.size(); i_l++ )
	    if( blkAt(lst[i_l]).at().process() )
	        blkAt(lst[i_l]).at().process(false);					    
    }
} 

void Contr::enable_( )
{
    loadV( );
}

void Contr::disable_( )
{
    freeV( );
}

void Contr::loadV( )
{
    int fld_cnt = 0;
    TConfig c_el(&((TipContr &)owner()).blockE());
	    
    TBDS::SName bd = BD();
    bd.tbl = cfg("BLOCK_SH").getS();
    AutoHD<TTable> tbl = owner().owner().owner().db().at().open(bd);
    while( tbl.at().fieldSeek(fld_cnt++,c_el) )
    {
        string id = c_el.cfg("ID").getS();
        if( !chldAvoid(m_bl,id) )
        {
            blkAdd(id);
            ((TConfig &)blkAt(id).at()) = c_el;
            if( blkAt(id).at().toEnable() )
        	blkAt(id).at().enable(true);
        }
	blkAt(id).at().load();
    }
    tbl.free();
    owner().owner().owner().db().at().close(bd);
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
        blkDel(lst[i_l]);
}

void *Contr::Task(void *contr)
{
    int    i_sync=0;
    
    struct itimerval mytim;             //Interval timer
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    Contr *cntr = (Contr *)contr;

#if OSC_DEBUG
    Mess->put(cntr->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif	

    try
    {
	if(cntr->m_per == 0) return(NULL);
	if(cntr->m_iter <= 0) cntr->m_iter = 1; 

	mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->m_per*1000;
	mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->m_per*1000;
    
	signal(SIGALRM,wakeup);
	setitimer(ITIMER_REAL,&mytim,NULL);
    
	cntr->run_st = true;  cntr->endrun = false;
	time_t1=times(NULL);    
	
	while( !cntr->endrun )
	{
	    //Check calk time
            unsigned long long t_cnt = SYS->shrtCnt();
	
	    ResAlloc::resRequestR(cntr->hd_res);
	    for(unsigned i_blk = 0; i_blk < cntr->clc_blks.size(); i_blk++)
		cntr->clc_blks[i_blk].at().calc();
	    ResAlloc::resReleaseR(cntr->hd_res);	
		
	    cntr->tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
	    pause();
	}
    } catch(TError err) 
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }

    cntr->clc_blks.clear();	//Clear calk blocks
    cntr->run_st = false;
    
    return(NULL);
}

TParamContr *Contr::ParamAttach( const string &name, int type )
{
    return(new Prm(name,&owner().tpPrmAt(type)));
}

void Contr::blkAdd( const string &iid )
{
    if( chldAvoid(m_bl,iid) ) return;
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
	    attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_blk_");
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
	    if( opt->name() == "add" )		blkAdd(opt->text());
	    else if( opt->name() == "del" )	chldDel(m_bl,opt->text(),-1,1);
	}
	else TController::cntrCmd_( a_path, opt, cmd );	
    }
}

AutoHD<TCntrNode> Contr::ctrAt1( const string &br )
{
    if( br.substr(0,5) == "_blk_" )	return blkAt(TSYS::strEncode(br.substr(5),TSYS::PathEl));
    else return TController::ctrAt1( br );
}

//======================================================================
//==== Prm 
//====================================================================== 

Prm::Prm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm)
{

}

Prm::~Prm( )
{    
    
}

void Prm::vlSet( int id_elem )
{
    Mess->put(nodePath().c_str(),TMess::Warning,"Direct set value an element command!");
}

void Prm::vlGet( int id_elem )
{
    Mess->put(nodePath().c_str(),TMess::Warning,"Direct get value of an element comand!");
}

void Prm::load( )
{
    
}


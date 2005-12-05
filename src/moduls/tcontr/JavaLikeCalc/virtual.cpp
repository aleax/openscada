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

#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>

#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>
#include <ttiparam.h>

#include "freelib.h"
#include "freefunc.h"
#include "virtual.h"

//============ Modul info! =====================================================
#define MOD_ID      "JavaLikeCalc"
#define MOD_NAME    "Java-like based calculator"
#define MOD_TYPE    "Controller"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.8.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow java-like based calculator and function's libraries engine. User can create and modify function and libraries."
#define LICENSE     "GPL"
//==============================================================================

JavaLikeCalc::TipContr *JavaLikeCalc::mod;

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
	JavaLikeCalc::TipContr *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = JavaLikeCalc::mod = new JavaLikeCalc::TipContr( source );

	return ( self_addr );
    }
}

using namespace JavaLikeCalc;

//===================== Complex1 functions library =========================
TipContr::TipContr( string src ) : m_bd("","","vLibFunc")
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= src;
    
    parse_res = ResAlloc::resCreate();
}

TipContr::~TipContr()
{
    nodeDelAll();
    ResAlloc::resDelete(parse_res);
}

void TipContr::preDisable(int flag)
{
    //stop();
    while( free_libs.size() )
    {
        owner().owner().func().at().unreg(free_libs[0]);
        free_libs.erase(free_libs.begin());
    }
}

void TipContr::postEnable( )
{
    TModule::postEnable( );
    
    //Controller db structure
    fldAdd( new TFld("PRM_BD",I18N("Parameters table"),TFld::String,0,"30","system") );
    fldAdd( new TFld("FUNC",I18N("Controller's function (<lib>.<func>)"),TFld::String,0,"20") );
    fldAdd( new TFld("PERIOD",I18N("Calc period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    fldAdd( new TFld("ITER",I18N("Iteration number into calc period"),TFld::Dec,0,"2","1","0;99") );
        
    //Controller value db structure
    val_el.fldAdd( new TFld("ID",Mess->I18N("IO ID"),TFld::String,FLD_KEY,"10") );
    val_el.fldAdd( new TFld("VAL",Mess->I18N("IO value"),TFld::String,0,"20") );

    //Add parameter types
    int t_prm = tpParmAdd("All","PRM_BD",I18N("Parameters"));
    tpPrmAt(t_prm).fldAdd( new TFld("FLD",I18N("Data fields(Sep - ';')"),TFld::String,FLD_NOVAL,"50") );		

    //Lib's db structure
    lb_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"10") );
    lb_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,0,"50") );
    lb_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,0,"300") );
    lb_el.fldAdd( new TFld("BD_TP",I18N("Data base type"),TFld::String,0,"20") );
    lb_el.fldAdd( new TFld("BD_NM",I18N("Data base name"),TFld::String,0,"50") );
    lb_el.fldAdd( new TFld("BD_TBL",I18N("Data base table"),TFld::String,0,"20") );

    //Function's structure
    fnc_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"10") );
    fnc_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,0,"50") );
    fnc_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,0,"300") );
    fnc_el.fldAdd( new TFld("FORMULA",I18N("Formula"),TFld::String,0,"1000") );

    //Function's IO structure
    fncio_el.fldAdd( new TFld("F_ID",I18N("Function ID"),TFld::String,FLD_KEY,"10") );
    fncio_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"10") );
    fncio_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,0,"50") );
    fncio_el.fldAdd( new TFld("TYPE",I18N("Type"),TFld::Dec,0,"1") );
    fncio_el.fldAdd( new TFld("MODE",I18N("Mode"),TFld::Dec,0,"1") );
    fncio_el.fldAdd( new TFld("DEF",I18N("Default value"),TFld::String,0,"20") );
    fncio_el.fldAdd( new TFld("VECT",I18N("Vector"),TFld::String,0,"10") );
    fncio_el.fldAdd( new TFld("HIDE",I18N("Hide"),TFld::Bool,0,"1") );
    fncio_el.fldAdd( new TFld("POS",I18N("Position"),TFld::Dec,0,"3") );
    
    //Init named constant table
    m_const.push_back(NConst("pi",3.14159265358l));
    m_const.push_back(NConst("e",2.71828182845l));
    
    //Init buildin functions list
    m_bfunc.push_back(BFunc("sin",Reg::FSin,1));
    m_bfunc.push_back(BFunc("cos",Reg::FCos,1));
    m_bfunc.push_back(BFunc("tan",Reg::FTan,1));
    m_bfunc.push_back(BFunc("sinh",Reg::FSinh,1));
    m_bfunc.push_back(BFunc("cosh",Reg::FCosh,1));
    m_bfunc.push_back(BFunc("tanh",Reg::FTanh,1));
    m_bfunc.push_back(BFunc("asin",Reg::FAsin,1));
    m_bfunc.push_back(BFunc("acos",Reg::FAcos,1));
    m_bfunc.push_back(BFunc("atan",Reg::FAtan,1));
    m_bfunc.push_back(BFunc("rand",Reg::FRand,1));
    m_bfunc.push_back(BFunc("lg",Reg::FLg,1));
    m_bfunc.push_back(BFunc("ln",Reg::FLn,1));
    m_bfunc.push_back(BFunc("exp",Reg::FExp,1));
    m_bfunc.push_back(BFunc("pow",Reg::FPow,2));
    m_bfunc.push_back(BFunc("sqrt",Reg::FSqrt,1));
    m_bfunc.push_back(BFunc("abs",Reg::FAbs,1));
    m_bfunc.push_back(BFunc("sign",Reg::FSign,1));
    m_bfunc.push_back(BFunc("ceil",Reg::FCeil,1));
    m_bfunc.push_back(BFunc("floor",Reg::FFloor,1));
}

TController *TipContr::ContrAttach( const string &name, const TBDS::SName &bd )
{
    return( new Contr(name,bd,this));
}

bool TipContr::present( const string &lib )
{
    for(int i_lb=0; i_lb < free_libs.size(); i_lb++ )
	if( free_libs[i_lb] == lib ) return true;
    return false;
}

TBDS::SName TipContr::BD()
{
    return owner().owner().nameDBPrep(m_bd);
}

void TipContr::modLoad( )
{
    try
    {
	TConfig c_el(&elLib());
	AutoHD<TTable> tbl = SYS->db().at().open(BD());
	int fld_cnt = 0;
	while( SYS->db().at().dataSeek(tbl,nodePath()+"lib/",fld_cnt++,c_el) )
        {
	    string l_id = c_el.cfg("ID").getS();
	    
	    int f_lb;
	    for( f_lb = 0; f_lb < free_libs.size(); f_lb++ )		
		if( free_libs[f_lb] == l_id )	break;		
	    if( f_lb >= free_libs.size() )
	    {
		Lib *lb = new Lib(l_id.c_str(),this);
		//*(TConfig *)lb = c_el;
    		SYS->func().at().reg(lb);
		free_libs.push_back(l_id);
	    }
	    ((Lib &)SYS->func().at().at(l_id).at()).load();
	    
	    c_el.cfg("ID").setS("");
	}
	if(!tbl.freeStat())
	{
	    tbl.free();
	    SYS->db().at().close(BD());
	}	
    }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void TipContr::modSave()
{   
    for( int l_id = 0; l_id < free_libs.size(); l_id++ )
	((Lib &)owner().owner().func().at().at(free_libs[l_id]).at()).save();
}  

void TipContr::modStart( )
{
    //Start functions
    for(int i_lb=0; i_lb < free_libs.size(); i_lb++ )
	owner().owner().func().at().at(free_libs[i_lb]).at().start(true);
    //Enable and start all JavaLike-controllers
    vector<string> lst;
    list(lst);
    for(int i_l=0; i_l<lst.size(); i_l++)
	TTipController::at(lst[i_l]).at().start( );
}

void TipContr::modStop( )
{
    //Stop and disable all JavaLike-controllers 
    vector<string> lst;
    list(lst);
    for(int i_l=0; i_l<lst.size(); i_l++)
	TTipController::at(lst[i_l]).at().disable( );
    //Stop functions    
    for(int i_lb=0; i_lb < free_libs.size(); i_lb++ )
        owner().owner().func().at().at(free_libs[i_lb]).at().start(false);
}

void TipContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TTipController::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrInsNode("area",1,opt,a_path.c_str(),"/libs",I18N("Functions' Libraries"));
	ctrMkNode("list",opt,a_path.c_str(),"/libs/lb",I18N("Libraries"),0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_lb_");
	ctrMkNode("comm",opt,a_path.c_str(),"/libs/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/libs/save",Mess->I18N("Save"),0550);		    	
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/libs/lb" )
	{
	    opt->childClean();
	    for( unsigned i_a=0; i_a < free_libs.size(); i_a++ )
		ctrSetS( opt, owner().owner().func().at().at(free_libs[i_a]).at().name(), free_libs[i_a].c_str() );
	}
	else TTipController::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/libs/lb" )
	{
	    if( opt->name() == "add" )
	    {
		SYS->func().at().reg(new Lib(opt->attr("id").c_str(),this,opt->text().c_str()));
		free_libs.push_back(opt->attr("id"));
	    }
	    else if( opt->name() == "del" )
	    {
		SYS->func().at().unreg(opt->attr("id"),1);
		for(int i_el = 0; i_el < free_libs.size(); i_el++)
		    if( free_libs[i_el] == opt->attr("id") )
		    {
			free_libs.erase(free_libs.begin()+i_el);
			break;
		    }
	    }
	}
	else if( a_path == "/libs/load" )	modLoad();
	else if( a_path == "/libs/save" )	modSave();
	else TTipController::cntrCmd_( a_path, opt, cmd );
    }
}

AutoHD<TCntrNode> TipContr::ctrAt( const string &a_path )
{
    if( a_path.substr(0,4) == "_lb_" )	return owner().owner().func().at().at(TSYS::strEncode(a_path.substr(4),TSYS::PathEl));
    else return TTipController::ctrAt(a_path);
}

NConst *TipContr::constGet( const char *nm )
{
    for( int i_cst = 0; i_cst < m_const.size(); i_cst++)
        if( m_const[i_cst].name == nm ) return &m_const[i_cst];
    	    return NULL;
}

BFunc *TipContr::bFuncGet( const char *nm )
{
    for( int i_bf = 0; i_bf < m_bfunc.size(); i_bf++)
	if( m_bfunc[i_bf].name == nm ) return &m_bfunc[i_bf];
	    return NULL;
}

//===================================================================
//================ Controller object ================================
//===================================================================
Contr::Contr( string name_c, const TBDS::SName &bd, ::TElem *cfgelem) :
    ::TController(name_c, bd, cfgelem), TValFunc(name_c.c_str()), endrun(false),
    m_per(cfg("PERIOD").getId()), m_iter(cfg("ITER").getId()), m_fnc(cfg("FUNC").getSd())
{
    cfg("PRM_BD").setS(name_c+"_prm");
    dimens(true);
}
		
Contr::~Contr()
{

}

void Contr::postDisable(int flag)
{
    if( run_st ) stop();
    try
    {
        if( flag )
        {
            //Delete IO value's table
            bool to_open = false;
            if( !((TTipBD &)SYS->db().at().modAt(BD().tp).at()).openStat(BD().bd) )
            {
                to_open = true;
                ((TTipBD &)SYS->db().at().modAt(BD().tp).at()).open(BD().bd,false);
            }
            ((TTipBD &)SYS->db().at().modAt(BD().tp).at()).at(BD().bd).at().del(TController::id()+"_val");
            if( to_open ) ((TTipBD &)SYS->db().at().modAt(BD().tp).at()).close(BD().bd);
        }
    }catch(TError err)
    { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
    
    TController::postDisable(flag);
}

void Contr::enable_( )
{
    if( !mod->present(TSYS::strSepParse(m_fnc,0,'.')) )
	throw TError(nodePath().c_str(),mod->I18N("Functions library <%s> no present. Please, create functions library!"),TSYS::strSepParse(m_fnc,0,'.').c_str());
    if( !mod->at(TSYS::strSepParse(m_fnc,0,'.')).at().present(TSYS::strSepParse(m_fnc,1,'.')) )
    {
	Mess->put(nodePath().c_str(),TMess::Info,mod->I18N("Create new function <%s>."),m_fnc.c_str());
	mod->at(TSYS::strSepParse(m_fnc,0,'.')).at().add(TSYS::strSepParse(m_fnc,1,'.').c_str());
    }
    func( &SYS->func().at().at(TSYS::strSepParse(m_fnc,0,'.')).at().at(TSYS::strSepParse(m_fnc,1,'.')).at() );
    try{ load( ); }
    catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
}

void Contr::disable_( )
{
    func(NULL);
}

void Contr::load( )
{
    TController::load();
    
    if( func() != NULL )
    { 
	((Func *)func())->load();    
	
	//Load values
	TConfig cfg(&mod->elVal());	
	TBDS::SName bd = BD();
	bd.tbl = TController::id()+"_val";		
	AutoHD<TTable> tbl = SYS->db().at().open(bd);
	
	int fld_cnt=0;	
	while( SYS->db().at().dataSeek(tbl,mod->nodePath()+bd.tbl,fld_cnt++,cfg) )
	{
	    if( func()->ioId(cfg.cfg("ID").getS()) >= 0 )
		setS(func()->ioId(cfg.cfg("ID").getS()),cfg.cfg("VAL").getS());
	    cfg.cfg("ID").setS("");	
	}	
	if( !tbl.freeStat() )
        {	    
	    tbl.free();
            SYS->db().at().close(bd);
	}
    }
}
    
void Contr::save( )
{
    TController::save();
    
    if( func() != NULL )
    {
	((Func *)func())->save();    
	
	//Save values
	TConfig cfg(&mod->elVal());
	TBDS::SName val_bd = BD();
        val_bd.tbl = TController::id()+"_val";
		
        AutoHD<TTable> tbl = SYS->db().at().open(val_bd,true);
	if( !tbl.freeStat() )
        {		
	    for( int iio = 0; iio < ioSize(); iio++ )
	    {
		cfg.cfg("ID").setS(func()->io(iio)->id());
		cfg.cfg("VAL").setS(getS(iio));
		tbl.at().fieldSet(cfg);
	    }
	    //Clear VAL
    	    int fld_cnt=0;
	    cfg.cfg("ID").setS("");
	    while( tbl.at().fieldSeek(fld_cnt++,cfg) )
	    {
        	if( ioId(cfg.cfg("ID").getS()) < 0 )
        	{ 
		    tbl.at().fieldDel(cfg); 
		    fld_cnt--; 
		}
		cfg.cfg("ID").setS("");
	    }
		
	    tbl.free();
    	    SYS->db().at().close(val_bd);			
	}	    
    }
}

void Contr::start( )
{
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
	
    TController::start();    
    
    if( !run_st )
    {
	((Func *)func())->start( true );
    
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
	        at(prm_list[i_prm]).at().enable();							        
    }
}

void Contr::stop( )
{
    TController::stop();

    if( run_st )
    {
        endrun = true;
        pthread_kill(pthr_tsk, SIGALRM);
        if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
            throw TError(nodePath().c_str(),mod->I18N("Controller no stoped!"));
        pthread_join(pthr_tsk, NULL);
    }
}

void *Contr::Task(void *contr)
{
    int    i_sync=0;
    
    struct itimerval mytim;             //Interval timer
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    Contr *cntr = (Contr *)contr;
		    
#if OSC_DEBUG
    Mess->put(cntr->nodePath().c_str(),TMess::Debug,mod->I18N("Thread <%d> started!"),getpid() );
#endif
			
    try
    {
        if(cntr->m_per == 0) return(NULL);
        if(cntr->m_iter <= 0) cntr->m_iter = 1;

        //Start interval timer
        mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->m_per*1000;
        mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->m_per*1000;
        setitimer(ITIMER_REAL,&mytim,NULL);
			
        cntr->run_st = true;  cntr->endrun = false;
        time_t1=times(NULL);
	
        while( !cntr->endrun )
        {
	    cntr->calc();
            pause();
        }
    } catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }
    
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

//======================================================================
//==== Contr
//======================================================================
void Contr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
        TController::cntrCmd_( a_path, opt, cmd );
	
	ctrMkNode("area",opt,a_path.c_str(),"/fnc",mod->I18N("Calcing"));
        ctrMkNode("fld",opt,a_path.c_str(),"/fnc/flib",mod->I18N("Use function"),0664,0,0,"str")->
	    attr_("dest","select")->attr_("select","/fnc/libs");
	ctrMkNode("fld",opt,a_path.c_str(),"/fnc/func","",0664,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/fnc/lnk",mod->I18N("Go to function"),0550,0,0,"lnk");
	if( enableStat() )
	{
	    ctrMkNode("fld",opt,a_path.c_str(),"/fnc/clc_tm",mod->I18N("Calc time (mks)"),0444,0,0,"real");
	    ctrMkNode("table",opt,a_path.c_str(),"/fnc/io",mod->I18N("Data"),0664,0,0)->attr_("s_com","add,del,ins,move")->
		attr_("rows","15");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/0",Mess->I18N("Id"),0664,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/1",Mess->I18N("Name"),0664,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/2",Mess->I18N("Type"),0664,0,0,"str")->
		attr_("idm","1")->attr_("dest","select")->attr_("select","/fnc/tp");
    	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/3",Mess->I18N("Mode"),0664,0,0,"str")->
    		attr_("idm","1")->attr_("dest","select")->attr_("select","/fnc/md");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/4",mod->I18N("Value"),0664,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/fnc/prog",mod->I18N("Programm"),0664,0,0,"str")->
        	attr_("cols","90")->attr_("rows","10");	
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/fnc/flib" )	ctrSetS( opt, TSYS::strSepParse(m_fnc,0,'.') );
	else if( a_path == "/fnc/func" )ctrSetS( opt, TSYS::strSepParse(m_fnc,1,'.') );
	else if( a_path == "/fnc/libs" )
	{
            opt->childClean();
            for( unsigned i_a=0; i_a < mod->freeLibList().size(); i_a++ )
                ctrSetS( opt, SYS->func().at().at(mod->freeLibList()[i_a]).at().name(), mod->freeLibList()[i_a].c_str());
	}
	else if( a_path == "/fnc/lnk" )	opt->text("d_Functions/d_"+TSYS::strSepParse(m_fnc,0,'.')+"/d_"+TSYS::strSepParse(m_fnc,1,'.') );
	else if( enableStat() )
	{
	    if( a_path == "/fnc/clc_tm" )	ctrSetR(opt,calcTm( ));
	    else if( a_path == "/fnc/io" )
    	    {	
        	XMLNode *n_id  	= ctrId(opt,"0");
        	XMLNode *n_nm  	= ctrId(opt,"1");
        	XMLNode *n_type	= ctrId(opt,"2");
        	XMLNode *n_mode	= ctrId(opt,"3");
        	XMLNode *n_val	= ctrId(opt,"4");
	    
        	for( int id = 0; id < func()->ioSize(); id++ )
		{
		    ctrSetS(n_id,func()->io(id)->id());
	    	    ctrSetS(n_nm,func()->io(id)->name());
	    	    //Make type
            	    if( func()->io(id)->type() == IO::Integer )	ctrSetS(n_type,"int");
        	    else if( func()->io(id)->type() == IO::Real )   ctrSetS(n_type,"real");
            	    else if( func()->io(id)->type() == IO::Boolean )ctrSetS(n_type,"bool");
            	    else if( func()->io(id)->type() == IO::String ) ctrSetS(n_type,"str");
            	    //Make mode
            	    if( func()->io(id)->mode() == IO::Output )      ctrSetS(n_mode,"out");
		    else if( func()->io(id)->mode() == IO::Return ) ctrSetS(n_mode,"ret");
		    else if( func()->io(id)->mode() == IO::Input )  ctrSetS(n_mode,"in");
		    //Value
		    ctrSetS(n_val,getS(id));
		}	    	    	
	    }
	    else if( a_path == "/fnc/tp" )
	    {
    		ctrSetS( opt, Mess->I18N("Real"), "real" );
		ctrSetS( opt, Mess->I18N("Integer"), "int" );
		ctrSetS( opt, Mess->I18N("Boolean"), "bool" );
		ctrSetS( opt, Mess->I18N("String"), "str" );
	    }
	    else if( a_path == "/fnc/md" )
	    {
		ctrSetS( opt, Mess->I18N("Input"), "in" );
            	ctrSetS( opt, Mess->I18N("Output"), "out" );
		ctrSetS( opt, Mess->I18N("Return"), "ret" );
	    }
    	    else if( a_path == "/fnc/prog" ) ctrSetS( opt, ((Func *)func())->prog() );
	    else TController::cntrCmd_( a_path, opt, cmd );
	}															
        else TController::cntrCmd_( a_path, opt, cmd );	
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/fnc/flib" )	m_fnc = ctrGetS( opt )+"."+TSYS::strSepParse(m_fnc,1,'.');
	else if( a_path == "/fnc/func" )m_fnc = TSYS::strSepParse(m_fnc,0,'.')+"."+ctrGetS( opt );
	else if( enableStat() )
	{
	    if( a_path == "/fnc/io" )
            {
                if( opt->name() == "add" )          ((Func *)func())->ioAdd( new IO("new","New IO",IO::Real,IO::Input) );
        	else if( opt->name() == "ins" )     ((Func *)func())->ioIns( new IO("new","New IO",IO::Real,IO::Input), atoi(opt->attr("row").c_str()) );
                else if( opt->name() == "del" )     ((Func *)func())->ioDel( atoi(opt->attr("row").c_str()) );
        	else if( opt->name() == "move" )    ((Func *)func())->ioMove( atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str()) );
                else if( opt->name() == "set" )
		{
            	    int row = atoi(opt->attr("row").c_str());
		    int col = atoi(opt->attr("col").c_str());
            	    if( (col == 0 || col == 1) && !opt->text().size() )
                	throw TError(nodePath().c_str(),"Empty value no valid.");
                    if( col == 0 ) 	func()->io(row)->id(ctrGetS(opt));
                    else if( col == 1 )	func()->io(row)->name(ctrGetS(opt));
		    else if( col == 2 )
		    {
	        	if( ctrGetS(opt) == "real" )		func()->io(row)->type(IO::Real);
                	else if( ctrGetS(opt) == "int" )	func()->io(row)->type(IO::Integer);
			else if( ctrGetS(opt) == "bool" )   	func()->io(row)->type(IO::Boolean);
                	else if( ctrGetS(opt) == "str" )	func()->io(row)->type(IO::String);
                    }
		    else if( col == 3 )
	            {
	                if( ctrGetS(opt) == "in" )          	func()->io(row)->mode(IO::Input);
	                else if( ctrGetS(opt) == "out" )	func()->io(row)->mode(IO::Output);
	                else if( ctrGetS(opt) == "ret" )    	func()->io(row)->mode(IO::Return);
	            }
		    else if( col == 4 )	setS(row,ctrGetS(opt));
		}
	    }	
	    else if( a_path == "/fnc/prog" )
	    {
		((Func *)func())->prog(ctrGetS(opt).c_str());
		((Func *)func())->progCompile();
	    }
	    else TController::cntrCmd_( a_path, opt, cmd );
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
    
    //Init elements    
    int fld_cnt = 0;
    while(TSYS::strSepParse(cfg("FLD").getS(),fld_cnt,';').size())
    {
	string dfld = TSYS::strSepParse(cfg("FLD").getS(),fld_cnt,';');
	unsigned char flg = FLD_DWR|FLD_DRD;
	TFld::Type    tp  = TFld::String;
	int           io_id = ((Contr &)owner()).ioId(dfld);
	if(io_id >= 0)
	{
	    //if( ((Contr &)owner()).ioMode(io_id) != IO::Input )
	    //	flg |= FLD_NWR;
	    switch( ((Contr &)owner()).ioType(io_id) )
	    {
    		case IO::String:        tp = TFld::String;      break;
    		case IO::Integer:       tp = TFld::Dec;         break;
    		case IO::Real:          tp = TFld::Real;        break;
    		case IO::Boolean:       tp = TFld::Bool;        break;
    	    }
	    if( !v_el.fldPresent(dfld) ||
    		v_el.fldAt(v_el.fldId(dfld)).type() != tp ||
    		v_el.fldAt(v_el.fldId(dfld)).flg() != flg )		
	    {
		if(v_el.fldPresent(dfld)) v_el.fldDel(v_el.fldId(dfld));
    		v_el.fldAdd( new TFld(dfld.c_str(),((Contr &)owner()).func()->io(io_id)->name().c_str(),tp,flg) );
	    }	    
	}	    
	fld_cnt++;    
    }
    
    //Check and delete no used fields
    for(int i_fld = 0; i_fld < v_el.fldSize(); i_fld++)
    {
	string fel;  
	int fld_cnt = 0;
	while( (fel = TSYS::strSepParse(cfg("FLD").getS(),fld_cnt++,';')).size() )
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
    if( !enableStat() ) return;
    try
    {
        int io_id = ((Contr &)owner()).ioId(val.name());
        if( io_id < 0 ) disable();
        else
        {
            switch(val.fld().type())
            {
                case TFld::String:
                    ((Contr &)owner()).setS(io_id,val.getS(NULL,true));
	            break;
	        case TFld::Dec:
	            ((Contr &)owner()).setI(io_id,val.getI(NULL,true));
                    break;
                case TFld::Real:
                    ((Contr &)owner()).setR(io_id,val.getR(NULL,true));
                    break;
                case TFld::Bool:
                    ((Contr &)owner()).setB(io_id,val.getB(NULL,true));
            	    break;
            }
        }
    }catch(TError err) { disable(); }
}
		
void Prm::vlGet( TVal &val )
{
    if( !enableStat() ) return;
    try
    {
        int io_id = ((Contr &)owner()).ioId(val.name());
        if( io_id < 0 ) disable();
        else
        {
            switch(val.fld().type())
            {
                case TFld::String:
                    val.setS(((Contr &)owner()).getS(io_id),NULL,true);
                    break;
                case TFld::Dec:
                    val.setI(((Contr &)owner()).getI(io_id),NULL,true);
            	    break;
                case TFld::Real:
                    val.setR(((Contr &)owner()).getR(io_id),NULL,true);
            	    break;
        	case TFld::Bool:
		    val.setB(((Contr &)owner()).getB(io_id),NULL,true);
		    break;
	    }						
        }
    }catch(TError err) { disable(); }
}
																																		    

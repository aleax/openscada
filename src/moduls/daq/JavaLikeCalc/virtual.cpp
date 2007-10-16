
//OpenSCADA system module DAQ.JavaLikeCalc file: virtual.cpp
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

#include <signal.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/times.h>

#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>

#include "freefunc.h"
#include "virtual.h"

//============ Modul info! =====================================================
#define MOD_ID      "JavaLikeCalc"
#define MOD_NAME    "Java-like based calculator"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.9.5"
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
TipContr::TipContr( string src ) //: lib_dbs(".")
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= src;
    
    m_lib = grpAdd("lib_");
}

TipContr::~TipContr()
{
    nodeDelAll();
}

void TipContr::postEnable( int flag )
{
    TModule::postEnable( flag );
    
    //Controller db structure
    fldAdd( new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30","system") );
    fldAdd( new TFld("FUNC",_("Controller's function"),TFld::String,TFld::NoFlag,"20") );
    fldAdd( new TFld("PERIOD",_("Calc period (ms)"),TFld::Integer,TFld::NoFlag,"7","1000","0;1000000") );
    fldAdd( new TFld("PRIOR",_("Calc task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("PER_DB",_("Sync db period (s)"),TFld::Integer,TCfg::Prevent,"5","0","0;3600") );
    fldAdd( new TFld("ITER",_("Iteration number into calc period"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );
        
    //Controller value db structure
    val_el.fldAdd( new TFld("ID",_("IO ID"),TFld::String,TCfg::Key,"10") );
    val_el.fldAdd( new TFld("VAL",_("IO value"),TFld::String,TFld::NoFlag,"20") );

    //Add parameter types
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("FLD",_("Data fields(Sep - ';')"),TFld::String,TCfg::NoVal,"50") );

    //Lib's db structure
    lb_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    lb_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    lb_el.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::NoFlag,"300") );
    lb_el.fldAdd( new TFld("DB",_("Data base"),TFld::String,TFld::NoFlag,"30") );

    //Function's structure
    fnc_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"50") );
    fnc_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    fnc_el.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::NoFlag,"300") );
    fnc_el.fldAdd( new TFld("MAXCALCTM",_("Maximum calc time"),TFld::Integer,TFld::NoFlag,"3","10","0;999") );
    fnc_el.fldAdd( new TFld("FORMULA",_("Formula"),TFld::String,TFld::NoFlag,"1000") );

    //Function's IO structure
    fncio_el.fldAdd( new TFld("F_ID",_("Function ID"),TFld::String,TCfg::Key,"50") );
    fncio_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    fncio_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    fncio_el.fldAdd( new TFld("TYPE",_("Type"),TFld::Integer,TFld::NoFlag,"1") );
    fncio_el.fldAdd( new TFld("MODE",_("Mode"),TFld::Integer,TFld::NoFlag,"1") );
    fncio_el.fldAdd( new TFld("DEF",_("Default value"),TFld::String,TFld::NoFlag,"20") );
    fncio_el.fldAdd( new TFld("HIDE",_("Hide"),TFld::Boolean,TFld::NoFlag,"1") );
    fncio_el.fldAdd( new TFld("POS",_("Position"),TFld::Integer,TFld::NoFlag,"3") );
    
    //Init named constant table
    double rvl;
    rvl = 3.14159265358l; m_const.push_back(NConst(TFld::Real,"pi",string((char*)&rvl,sizeof(rvl))));
    rvl = 2.71828182845l; m_const.push_back(NConst(TFld::Real,"e",string((char*)&rvl,sizeof(rvl))));
    rvl = EVAL_REAL; m_const.push_back(NConst(TFld::Real,"EVAL_REAL",string((char*)&rvl,sizeof(rvl))));
    int ivl;
    ivl = EVAL_INT; m_const.push_back(NConst(TFld::Integer,"EVAL_INT",string((char*)&ivl,sizeof(ivl))));
    ivl = EVAL_BOOL; m_const.push_back(NConst(TFld::Integer,"EVAL_BOOL",string((char*)&ivl,sizeof(ivl))));
    
    m_const.push_back(NConst(TFld::String,"EVAL_STR",EVAL_STR));
    
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

TController *TipContr::ContrAttach( const string &name, const string &daq_db )
{
    return new Contr(name,daq_db,this);
}

void TipContr::compileFuncLangs( vector<string> &ls )
{
    ls.clear();
    ls.push_back("JavaScript");    
}

string TipContr::compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text )
{
    if( lang != "JavaScript" )	throw TError(nodePath().c_str(),_("Compile the program language %s is no support."));
    if(!lbPresent("sys_compile"))	lbReg( new Lib("sys_compile","","") );
    if(!lbAt("sys_compile").at().present(fnc_cfg.id()))
	lbAt("sys_compile").at().add(fnc_cfg.id().c_str(),"");
    AutoHD<Func> func = lbAt("sys_compile").at().at(fnc_cfg.id());
    ((TFunction&)func.at()).operator=(fnc_cfg);
    func.at().prog(prog_text.c_str());
    try
    {
	if(func.at().startStat()) func.at().start(false);
	func.at().start(true); 
    }
    catch(TError err)
    {
	func.free();
	lbAt("sys_compile").at().del(fnc_cfg.id().c_str());
	throw TError(nodePath().c_str(),_("Compile error: %s\n"),err.mess.c_str());
    }
    return func.at().nodePath();
}

string TipContr::optDescr( )
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
				
    return buf;
}

void TipContr::modLoad( )
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

    //========== Load parameters =============

    //=========== Load function's libraries =============
    try
    {
	//Search and create new libraries
	TConfig c_el(&elLib());
	c_el.cfgViewAll(false);
	vector<string> tdb_ls, db_ls;	
	
	//- Search into DB -
	SYS->db().at().modList(tdb_ls);
	for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd=tdb_ls[i_tp]+"."+db_ls[i_db];
		int lib_cnt = 0;
		while(SYS->db().at().dataSeek(wbd+"."+libTable(),"",lib_cnt++,c_el) )
	        {
		    string l_id = c_el.cfg("ID").getS();		    
		    if(!lbPresent(l_id)) lbReg(new Lib(l_id.c_str(),"",(wbd==SYS->workDB())?"*.*":wbd));		
		    c_el.cfg("ID").setS("");		
		}
	    }	
	}
	
	//- Search into config file -
	int lib_cnt = 0;
	while(SYS->db().at().dataSeek("",nodePath()+"lib/",lib_cnt++,c_el) )
	{
	    string l_id = c_el.cfg("ID").getS();		    
	    if(!lbPresent(l_id)) lbReg(new Lib(l_id.c_str(),"","*.*"));		
	    c_el.cfg("ID").setS("");		
	}	
	
	//- Load present libraries -
	lbList(tdb_ls);
        for( int l_id = 0; l_id < tdb_ls.size(); l_id++ )
    	    lbAt(tdb_ls[l_id]).at().load();
    }catch( TError err )
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Load function's libraries error.")); 
    }
}

void TipContr::modSave()
{   
    //========== Save parameters =============

    //=========== Save function's libraries =============
    vector<string> ls;
    lbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	lbAt(ls[l_id]).at().save();
}  

void TipContr::modStart( )
{
    vector<string> lst;
    //Start functions
    lbList(lst);
    for(int i_lb=0; i_lb < lst.size(); i_lb++ )
	lbAt(lst[i_lb]).at().start(true);

    TTipDAQ::modStart( );	
}

void TipContr::modStop( )
{
    //Stop and disable all JavaLike-controllers 
    vector<string> lst;
    list(lst);
    for(int i_l=0; i_l<lst.size(); i_l++)
	at(lst[i_l]).at().disable( );
    //Stop functions    
    lbList(lst);
    for(int i_lb=0; i_lb < lst.size(); i_lb++ )
	lbAt(lst[i_lb]).at().start(false);
}

void TipContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TTipDAQ::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/lib_",_("Library"),0440,"root","root",1,"list","/libs/lb");
	if(ctrMkNode("area",opt,1,"/libs",_("Functions' Libraries")))
	{
	    ctrMkNode("list",opt,-1,"/libs/lb",_("Libraries"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","lib_");
	    ctrMkNode("comm",opt,-1,"/libs/load",_("Load"),0660);
    	    ctrMkNode("comm",opt,-1,"/libs/save",_("Save"),0660);
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/libs/lb" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> lst;
    	    lbList(lst);
    	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
    		opt->childAdd("el")->setAttr("id",lst[i_a])->setText(lbAt(lst[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	lbReg(new Lib(opt->attr("id").c_str(),opt->text().c_str(),"*.*"));
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	lbUnreg(opt->attr("id"),1);
    }
    else if( a_path == "/libs/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	modLoad();
    else if( a_path == "/libs/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	modSave();
    else TTipDAQ::cntrCmdProc(opt);
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
Contr::Contr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    ::TController(name_c, daq_db, cfgelem), TValFunc(name_c.c_str(),NULL,false), prc_st(false), 
    endrun_req(false), sync_st(false),
    m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), m_dbper(cfg("PER_DB").getId()), 
    m_iter(cfg("ITER").getId()), m_fnc(cfg("FUNC").getSd())
{
    cfg("PRM_BD").setS("JavaLikePrm_"+name_c);
    setDimens(true);

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
    try
    {
        if( flag )
        {
            //Delete IO value's table
	    string db = DB()+"."+TController::id()+"_val";
	    SYS->db().at().open(db);
	    SYS->db().at().close(db,true);
        }
    }catch(TError err)
    { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }
    
    TController::postDisable(flag);
}

void Contr::enable_( )
{
    if( !mod->lbPresent(TSYS::strSepParse(m_fnc,0,'.')) )
	throw TError(nodePath().c_str(),_("Functions library <%s> no present. Please, create functions library!"),TSYS::strSepParse(m_fnc,0,'.').c_str());
    if( !mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().present(TSYS::strSepParse(m_fnc,1,'.')) )
    {
	mess_info(nodePath().c_str(),_("Create new function <%s>."),m_fnc.c_str());
	mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().add(TSYS::strSepParse(m_fnc,1,'.').c_str());
    }
    setFunc( &mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().at(TSYS::strSepParse(m_fnc,1,'.')).at() );
    try{ loadFunc( ); }
    catch(TError err)
    { 
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Load function and its io error."));
    }
}

void Contr::disable_( )
{
    setFunc(NULL);
}

void Contr::load( )
{
    TController::load( );
    
    loadFunc( );
}

void Contr::loadFunc( )
{    
    if( func() != NULL )
    { 
	((Func *)func())->load();    
	
	//Load values
	TConfig cfg(&mod->elVal());	
	string bd_tbl = TController::id()+"_val";
	string bd = DB()+"."+bd_tbl;
	
	int fld_cnt=0;	
	while( SYS->db().at().dataSeek(bd,mod->nodePath()+bd_tbl,fld_cnt++,cfg) )
	{
	    if( func()->ioId(cfg.cfg("ID").getS()) >= 0 )
		setS(func()->ioId(cfg.cfg("ID").getS()),cfg.cfg("VAL").getS());
	    cfg.cfg("ID").setS("");	
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
        string bd_tbl = TController::id()+"_val";
        string val_bd = DB()+"."+bd_tbl;	
	for( int iio = 0; iio < ioSize(); iio++ )
        {
	    cfg.cfg("ID").setS(func()->io(iio)->id());
            cfg.cfg("VAL").setS(getS(iio));
	    SYS->db().at().dataSet(val_bd,mod->nodePath()+bd_tbl,cfg);
	}
	//Clear VAL
	int fld_cnt=0;
        cfg.cfg("ID").setS("");
	cfg.cfgViewAll(false);
	while( SYS->db().at().dataSeek(val_bd,mod->nodePath()+bd_tbl,fld_cnt++,cfg) )
        {
	    if( ioId(cfg.cfg("ID").getS()) < 0 )
	    {
		SYS->db().at().dataDel(val_bd,mod->nodePath()+bd_tbl,cfg);
	        fld_cnt--;
	    }
	    cfg.cfg("ID").setS("");
	}
    }
}

void Contr::start_( )
{
    ((Func *)func())->start( true );
    
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
	    
        pthread_create(&procPthr,&pthr_attr,Contr::Task,this);
        pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait(prc_st, true, nodePath()+"start",5) )
            throw TError(nodePath().c_str(),_("Acquisition task no started!"));
    }
    
    //Start interval timer for periodic thread creating
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
        pthread_kill( procPthr, SIGALRM );
        if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
            throw TError(nodePath().c_str(),_("Acquisition task no stoped!"));
        pthread_join( procPthr, NULL );
    }
    
    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
        itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(sncDBTm, 0, &itval, NULL);
    if( TSYS::eventWait( sync_st, false, nodePath()+"sync_stop",5) )
        throw TError(nodePath().c_str(),_("Controller sync DB no stoped!"));
}

void *Contr::Task( void *icntr )
{
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    Contr &cntr = *(Contr *)icntr;
	
    cntr.endrun_req = false;
    cntr.prc_st = true;
		
    while(!cntr.endrun_req)
    {	
	for( int i_it = 0; i_it < cntr.m_iter; i_it++ )
	    try{ cntr.calc(); } 
	    catch(TError err) 
	    { 
		mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); 
		mess_err(cntr.nodePath().c_str(),_("Calc controller's function error."));
	    }
	
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

void Contr::TaskDBSync(union sigval obj)
{
    Contr *cntr = (Contr *)obj.sival_ptr;
    if( cntr->sync_st )  return;
    cntr->sync_st = true;
	    
    try{ cntr->save( ); }
    catch(TError err) 
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str() );
	mess_err(cntr->nodePath().c_str(),_("Save controller error."));
    }
    
    cntr->sync_st = false;
}

TParamContr *Contr::ParamAttach( const string &name, int type )
{
    return new Prm(name,&owner().tpPrmAt(type));
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
	ctrMkNode("fld",opt,-1,"/cntr/cfg/FUNC",cfg("FUNC").fld().descr(),0660,"root","root",3,"tp","str","dest","sel_ed","select","/cntr/flst");
	if( enableStat() && ctrMkNode("area",opt,-1,"/fnc",_("Calcing")) )
	{
	    ctrMkNode("fld",opt,-1,"/fnc/clc_tm",_("Calc time (mks)"),0444,"root","root",1,"tp","real");
	    if(ctrMkNode("table",opt,-1,"/fnc/io",_("Data"),0664,"root","root",2,"s_com","add,del,ins,move","rows","15"))
	    {
		ctrMkNode("list",opt,-1,"/fnc/io/0",_("Id"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/fnc/io/1",_("Name"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/fnc/io/2",_("Type"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/fnc/tp");
    		ctrMkNode("list",opt,-1,"/fnc/io/3",_("Mode"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/fnc/md");
		ctrMkNode("list",opt,-1,"/fnc/io/4",_("Value"),0664,"root","root",1,"tp","str");
	    }
	    ctrMkNode("fld",opt,-1,"/fnc/prog",_("Programm"),0664,"root","root",3,"tp","str","cols","90","rows","10");
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/flst" && ctrChkNode(opt) )
    {
	vector<string> lst;
        int c_lv = 0;
        string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
        for( int c_off = 0; (c_el=TSYS::strSepParse(m_fnc,0,'.',&c_off)).size(); c_lv++ )
	{
            c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
        }
        if(c_lv) c_path+=".";
        switch(c_lv)
        {
	    case 0:	mod->lbList(lst); break;
            case 1:
                if( mod->lbPresent(TSYS::strSepParse(m_fnc,0,'.')) )
	    	    mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().list(lst);
		break;
	}
        for( unsigned i_a=0; i_a < lst.size(); i_a++ )
    	    opt->childAdd("el")->setText(c_path+lst[i_a]);
    }
    else if( a_path == "/fnc/clc_tm" && enableStat() && ctrChkNode(opt) )	opt->setText(TSYS::real2str(calcTm( ),6));
    else if( a_path == "/fnc/io" && enableStat() )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
    	{	
    	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/fnc/io/0","",0664);
    	    XMLNode *n_nm  	= ctrMkNode("list",opt,-1,"/fnc/io/1","",0664);
	    XMLNode *n_type	= ctrMkNode("list",opt,-1,"/fnc/io/2","",0664);
    	    XMLNode *n_mode	= ctrMkNode("list",opt,-1,"/fnc/io/3","",0664);
    	    XMLNode *n_val	= ctrMkNode("list",opt,-1,"/fnc/io/4","",0664);
	    
    	    for( int id = 0; id < func()->ioSize(); id++ )
	    {
	        if(n_id)	n_id->childAdd("el")->setText(func()->io(id)->id());
		if(n_nm)	n_nm->childAdd("el")->setText(func()->io(id)->name());
		if(n_type)	n_type->childAdd("el")->setText(TSYS::int2str(func()->io(id)->type()));
		if(n_mode)	n_mode->childAdd("el")->setText(TSYS::int2str(func()->io(id)->flg()&(IO::Output|IO::Return)));
		if(n_val)	n_val->childAdd("el")->setText(getS(id));
	    }	    	    	
	}
        if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	((Func *)func())->ioAdd( new IO("new","New IO",IO::Real,IO::Default) );
        if( ctrChkNode(opt,"ins",0664,"root","root",SEQ_WR) )	((Func *)func())->ioIns( new IO("new","New IO",IO::Real,IO::Default), atoi(opt->attr("row").c_str()) );
        if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	((Func *)func())->ioDel( atoi(opt->attr("row").c_str()) );
        if( ctrChkNode(opt,"move",0664,"root","root",SEQ_WR) )	((Func *)func())->ioMove( atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str()) );
        if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
            int row = atoi(opt->attr("row").c_str());
	    int col = atoi(opt->attr("col").c_str());
            if( (col == 0 || col == 1) && !opt->text().size() )
            	throw TError(nodePath().c_str(),_("Empty value no valid."));
	    switch(col)	
	    {
		case 0:	func()->io(row)->setId(opt->text());	break;
		case 1:	func()->io(row)->setName(opt->text());	break;
		case 2:	func()->io(row)->setType((IO::Type)atoi(opt->text().c_str()));	break;
		case 3:	func()->io(row)->setFlg( func()->io(row)->flg()^((atoi(opt->text().c_str())^func()->io(row)->flg())&(IO::Output|IO::Return)) );	break;
		case 4:	setS(row,opt->text());	break;
	    }
	}
    }	
    else if( a_path == "/fnc/tp" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Real))->setText(_("Real"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Integer))->setText(_("Integer"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Boolean))->setText(_("Boolean"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::String))->setText(_("String"));
    }
    else if( a_path == "/fnc/md" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Default))->setText(_("Input"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Output))->setText(_("Output"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Return))->setText(_("Return"));
    }
    else if( a_path == "/fnc/prog" && enableStat() ) 
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(((Func *)func())->prog());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    ((Func *)func())->prog(opt->text().c_str());
	    ((Func *)func())->progCompile();
	}
    }
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
    if(!vlElemPresent(&v_el)) vlElemAtt(&v_el);
}

void Prm::enable()
{
    if( enableStat() )  return;
    
    //Init elements    
    string dfld;
    for( int fld_off = 0; (dfld=TSYS::strSepParse(cfg("FLD").getS(),0,';',&fld_off)).size(); )
    {
	unsigned flg = TVal::DirWrite|TVal::DirRead;
	TFld::Type    tp  = TFld::String;
	int           io_id = ((Contr &)owner()).ioId(dfld);
	if(io_id >= 0)
	{
	    //if( ((Contr &)owner()).ioMode(io_id) != IO::Input )
	    //	flg |= Fld::NoWrite;
	    switch( ((Contr &)owner()).ioType(io_id) )
	    {
    		case IO::String:        tp = TFld::String;      break;
    		case IO::Integer:       tp = TFld::Integer;	break;
    		case IO::Real:          tp = TFld::Real;        break;
    		case IO::Boolean:       tp = TFld::Boolean;	break;
    	    }
	    if( !v_el.fldPresent(dfld) ||
    		v_el.fldAt(v_el.fldId(dfld)).type() != tp ||
    		v_el.fldAt(v_el.fldId(dfld)).flg() != flg )		
	    {
		if(v_el.fldPresent(dfld)) v_el.fldDel(v_el.fldId(dfld));
    		v_el.fldAdd( new TFld(dfld.c_str(),((Contr &)owner()).func()->io(io_id)->name().c_str(),tp,flg) );
	    }
	}
    }
    
    //Check and delete no used fields
    for(int i_fld = 0; i_fld < v_el.fldSize(); i_fld++)
    {
	string fel;  
	for( int fld_off = 0; (fel = TSYS::strSepParse(cfg("FLD").getS(),0,';',&fld_off)).size(); )
	    if( fel == v_el.fldAt(i_fld).name() ) break;	
	if( fel.empty() )	{ v_el.fldDel(i_fld); i_fld--; }
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
                    ((Contr &)owner()).setS(io_id,val.getS(0,true));
	            break;
	        case TFld::Integer:
	            ((Contr &)owner()).setI(io_id,val.getI(0,true));
                    break;
                case TFld::Real:
                    ((Contr &)owner()).setR(io_id,val.getR(0,true));
                    break;
                case TFld::Boolean:
                    ((Contr &)owner()).setB(io_id,val.getB(0,true));
            	    break;
            }
        }
    }catch(TError err) { disable(); }
}
		
void Prm::vlGet( TVal &val )
{
    if( val.name() == "err" )
    {
        if( !owner().startStat() ) val.setS(_("2:Controller stoped"),0,true);
        else if( !enableStat() )   val.setS(_("1:Parameter disabled"),0,true);
        else val.setS("0",0,true);
	return;
    }
    try
    {
        int io_id = ((Contr &)owner()).ioId(val.name());
        if( io_id < 0 ) disable();
        else
        {
            switch(val.fld().type())
            {
                case TFld::String:	
		    val.setS(enableStat()?owner().getS(io_id):EVAL_STR,0,true);	break;
                case TFld::Integer:
                    val.setI(enableStat()?owner().getI(io_id):EVAL_INT,0,true); break;
                case TFld::Real:
                    val.setR(enableStat()?owner().getR(io_id):EVAL_REAL,0,true);break;
        	case TFld::Boolean:
		    val.setB(enableStat()?owner().getB(io_id):EVAL_BOOL,0,true);break;
	    }						
        }
    }catch(TError err) { disable(); }
}

void Prm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(((long long)owner().period())*1000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}
																																		    

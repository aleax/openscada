
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
#include <resalloc.h>
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
    
    parse_res = ResAlloc::resCreate();
}

TipContr::~TipContr()
{
    nodeDelAll();
    ResAlloc::resDelete(parse_res);
}

void TipContr::postEnable( )
{
    TModule::postEnable( );
    
    //Controller db structure
    fldAdd( new TFld("PRM_BD",I18N("Parameters table"),TFld::String,FLD_NOFLG,"30","system") );
    fldAdd( new TFld("FUNC",I18N("Controller's function"),TFld::String,FLD_NOFLG,"20") );
    fldAdd( new TFld("PERIOD",I18N("Calc period (ms)"),TFld::Dec,FLD_PREV,"5","1000","0;10000") );
    fldAdd( new TFld("PER_DB",I18N("Sync db period (s)"),TFld::Dec,FLD_PREV,"5","0","0;3600") );
    fldAdd( new TFld("ITER",I18N("Iteration number into calc period"),TFld::Dec,FLD_NOFLG,"2","1","0;99") );
        
    //Controller value db structure
    val_el.fldAdd( new TFld("ID",Mess->I18N("IO ID"),TFld::String,FLD_KEY,"10") );
    val_el.fldAdd( new TFld("VAL",Mess->I18N("IO value"),TFld::String,FLD_NOFLG,"20") );

    //Add parameter types
    int t_prm = tpParmAdd("std","PRM_BD",I18N("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("FLD",I18N("Data fields(Sep - ';')"),TFld::String,FLD_NOVAL,"50") );		

    //Lib's db structure
    lb_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"10") );
    lb_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,FLD_NOFLG,"50") );
    lb_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,FLD_NOFLG,"300") );
    lb_el.fldAdd( new TFld("DB",I18N("Data base"),TFld::String,FLD_NOFLG,"30") );

    //Function's structure
    fnc_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"10") );
    fnc_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,FLD_NOFLG,"50") );
    fnc_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,FLD_NOFLG,"300") );
    fnc_el.fldAdd( new TFld("FORMULA",I18N("Formula"),TFld::String,FLD_NOFLG,"1000") );

    //Function's IO structure
    fncio_el.fldAdd( new TFld("F_ID",I18N("Function ID"),TFld::String,FLD_KEY,"10") );
    fncio_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,FLD_KEY,"10") );
    fncio_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,FLD_NOFLG,"50") );
    fncio_el.fldAdd( new TFld("TYPE",I18N("Type"),TFld::Dec,FLD_NOFLG,"1") );
    fncio_el.fldAdd( new TFld("MODE",I18N("Mode"),TFld::Dec,FLD_NOFLG,"1") );
    fncio_el.fldAdd( new TFld("DEF",I18N("Default value"),TFld::String,FLD_NOFLG,"20") );
    fncio_el.fldAdd( new TFld("VECT",I18N("Vector"),TFld::String,FLD_NOFLG,"10") );
    fncio_el.fldAdd( new TFld("HIDE",I18N("Hide"),TFld::Bool,FLD_NOFLG,"1") );
    fncio_el.fldAdd( new TFld("POS",I18N("Position"),TFld::Dec,FLD_NOFLG,"3") );
    
    //Init named constant table
    double rvl;
    rvl = 3.14159265358l; m_const.push_back(NConst(TFld::Real,"pi",string((char*)&rvl,sizeof(rvl))));
    rvl = 2.71828182845l; m_const.push_back(NConst(TFld::Real,"e",string((char*)&rvl,sizeof(rvl))));
    rvl = EVAL_REAL; m_const.push_back(NConst(TFld::Real,"EVAL_REAL",string((char*)&rvl,sizeof(rvl))));
    int ivl;
    ivl = EVAL_INT; m_const.push_back(NConst(TFld::Dec,"EVAL_INT",string((char*)&ivl,sizeof(ivl))));
    ivl = EVAL_BOOL; m_const.push_back(NConst(TFld::Dec,"EVAL_BOOL",string((char*)&ivl,sizeof(ivl))));
    
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
    return( new Contr(name,daq_db,this));
}

string TipContr::optDescr( )
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),I18N(
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
	vector<string> tdb_ls, db_ls;	
	SYS->db().at().modList(tdb_ls);
	for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd=tdb_ls[i_tp]+"."+db_ls[i_db];
		int lib_cnt = 0;
		while(SYS->db().at().dataSeek(wbd+"."+libTable(),nodePath()+"lib/",lib_cnt++,c_el) )
	        {
		    string l_id = c_el.cfg("ID").getS();		    
		    if(!lbPresent(l_id)) lbReg(new Lib(l_id.c_str(),"",(wbd==SYS->workDB())?"*.*":wbd));		
		    c_el.cfg("ID").setS("");		
		}
	    }	
	}
	//Load present libraries
	lbList(tdb_ls);
        for( int l_id = 0; l_id < tdb_ls.size(); l_id++ )
    	    lbAt(tdb_ls[l_id]).at().load();
    }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
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
    //Enable and start all JavaLike-controllers
    list(lst);
    for(int i_l=0; i_l < lst.size(); i_l++)
	at(lst[i_l]).at().start( );
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

void TipContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> lst;
    if( cmd==TCntrNode::Info )
    {
	int bd_gr = SYS->db().at().subSecGrp();
	TTipDAQ::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrInsNode("area",1,opt,a_path.c_str(),"/libs",I18N("Functions' Libraries"));
	ctrMkNode("list",opt,a_path.c_str(),"/libs/lb",I18N("Libraries"),0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del")->attr_("br_pref","lib_");
	ctrMkNode("comm",opt,a_path.c_str(),"/libs/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/libs/save",Mess->I18N("Save"),0550);		    	
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/libs/lb" )
	{
	    opt->childClean();
	    lbList(lst);
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
		ctrSetS( opt, lbAt(lst[i_a]).at().name(), lst[i_a].c_str() );
	}
	else TTipDAQ::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/libs/lb" )
	{
	    if( opt->name() == "add" )
		lbReg(new Lib(opt->attr("id").c_str(),opt->text().c_str(),":"));
	    else if( opt->name() == "del" )
		lbUnreg(opt->attr("id"),1);
	}
	else if( a_path == "/libs/load" )	modLoad();
	else if( a_path == "/libs/save" )	modSave();
	else TTipDAQ::cntrCmd_( a_path, opt, cmd );
    }
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
    ::TController(name_c, daq_db, cfgelem), TValFunc(name_c.c_str(),NULL,false), prc_st(false), sync_st(false),
    m_per(cfg("PERIOD").getId()), m_dbper(cfg("PER_DB").getId()), m_iter(cfg("ITER").getId()), 
    m_fnc(cfg("FUNC").getSd())
{
    cfg("PRM_BD").setS(name_c+"_prm");
    dimens(true);

    //Create calc timer
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
    //Create sync DB timer
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = TaskDBSync;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&sncDBTm);
}
		
Contr::~Contr()
{
    timer_delete(sncDBTm);
    timer_delete(tmId);
}

void Contr::postDisable(int flag)
{
    if( run_st ) stop();
    try
    {
        if( flag )
        {
            //Delete IO value's table
	    string db = genBD()+"."+TController::id()+"_val";
	    SYS->db().at().open(db);
	    SYS->db().at().close(db,true);
        }
    }catch(TError err)
    { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
    
    TController::postDisable(flag);
}

void Contr::enable_( )
{
    if( !mod->lbPresent(TSYS::strSepParse(m_fnc,0,'.')) )
	throw TError(nodePath().c_str(),mod->I18N("Functions library <%s> no present. Please, create functions library!"),TSYS::strSepParse(m_fnc,0,'.').c_str());
    if( !mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().present(TSYS::strSepParse(m_fnc,1,'.')) )
    {
	Mess->put(nodePath().c_str(),TMess::Info,mod->I18N("Create new function <%s>."),m_fnc.c_str());
	mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().add(TSYS::strSepParse(m_fnc,1,'.').c_str());
    }
    func( &mod->lbAt(TSYS::strSepParse(m_fnc,0,'.')).at().at(TSYS::strSepParse(m_fnc,1,'.')).at() );
    try{ load( ); }
    catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
}

void Contr::disable_( )
{
    func(NULL);
}

void Contr::load( )
{
    TController::load( );
    
    if( func() != NULL )
    { 
	((Func *)func())->load();    
	
	//Load values
	TConfig cfg(&mod->elVal());	
	string bd_tbl = TController::id()+"_val";
	string bd = genBD()+"."+bd_tbl;
	
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
        string val_bd = genBD()+"."+bd_tbl;	
	for( int iio = 0; iio < ioSize(); iio++ )
        {
	    cfg.cfg("ID").setS(func()->io(iio)->id());
            cfg.cfg("VAL").setS(getS(iio));
	    SYS->db().at().dataSet(val_bd,mod->nodePath()+bd_tbl,cfg);
	}
	//Clear VAL
	int fld_cnt=0;
        cfg.cfg("ID").setS("");		    
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

void Contr::start( )
{
    TController::start();
    
    if( run_st ) return;

    ((Func *)func())->start( true );
    
    //Start interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = (m_per*1000000)/1000000000;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = (m_per*1000000)%1000000000;
    timer_settime(tmId, 0, &itval, NULL);
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_dbper;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(sncDBTm, 0, &itval, NULL);    
    
    //Enable parameters
    vector<string> prm_list;
    list(prm_list);
    for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
        if( at(prm_list[i_prm]).at().toEnable() )
    	    at(prm_list[i_prm]).at().enable();
	    
    run_st = true;
}

void Contr::stop( )
{
    TController::stop();

    if( !run_st ) return;
    
    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
        itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st, false, nodePath()+"stop",5) )
        throw TError(nodePath().c_str(),mod->I18N("Controller no stoped!"));
    timer_settime(sncDBTm, 0, &itval, NULL);
    if( TSYS::eventWait( sync_st, false, nodePath()+"sync_stop",5) )
        throw TError(nodePath().c_str(),mod->I18N("Controller sync DB no stoped!"));	

    //Disable parameters
    vector<string> prm_list;
    list(prm_list);
    for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
        if( at(prm_list[i_prm]).at().enableStat() )
            at(prm_list[i_prm]).at().disable();

    run_st = false;	
}

void Contr::Task(union sigval obj)
{
    Contr *cntr = (Contr *)obj.sival_ptr;
    if( cntr->prc_st )  return;
    cntr->prc_st = true;
    
    try{ cntr->calc(); } 
    catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }    	
    
    cntr->prc_st = false;
}

void Contr::TaskDBSync(union sigval obj)
{
    Contr *cntr = (Contr *)obj.sival_ptr;
    if( cntr->sync_st )  return;
    cntr->sync_st = true;
	    
    try{ cntr->save( ); }
    catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }
    
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
        if( cfg.fld().name() == "PERIOD" )
        {
            itval.it_interval.tv_sec = itval.it_value.tv_sec = (m_per*1000000)/1000000000;
            itval.it_interval.tv_nsec = itval.it_value.tv_nsec = (m_per*1000000)%1000000000;
            timer_settime(tmId, 0, &itval, NULL);
        }
        else if( cfg.fld().name() == "PER_DB" )
        {
            itval.it_interval.tv_sec = itval.it_value.tv_sec = m_dbper;
            itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
            timer_settime(sncDBTm, 0, &itval, NULL);
	}
    }
    return true;
}

void Contr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> lst;
    if( cmd==TCntrNode::Info )
    {
        TController::cntrCmd_( a_path, opt, cmd );
	
	ctrId(opt,"/cntr/cfg/FUNC")->attr_("dest","sel_ed")->attr_("select","/cntr/flst");
	if( enableStat() )
        {
	    ctrMkNode("area",opt,a_path.c_str(),"/fnc",mod->I18N("Calcing"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/fnc/clc_tm",mod->I18N("Calc time (mks)"),0444,0,0,"real");
	    ctrMkNode("table",opt,a_path.c_str(),"/fnc/io",mod->I18N("Data"),0664,0,0)->attr_("s_com","add,del,ins,move")->
		attr_("rows","15");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/0",Mess->I18N("Id"),0664,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/1",Mess->I18N("Name"),0664,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/2",Mess->I18N("Type"),0664,0,0,"dec")->
		attr_("idm","1")->attr_("dest","select")->attr_("select","/fnc/tp");
    	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/3",Mess->I18N("Mode"),0664,0,0,"dec")->
    		attr_("idm","1")->attr_("dest","select")->attr_("select","/fnc/md");
	    ctrMkNode("list",opt,a_path.c_str(),"/fnc/io/4",mod->I18N("Value"),0664,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/fnc/prog",mod->I18N("Programm"),0664,0,0,"str")->
        	attr_("cols","90")->attr_("rows","10");	
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/cntr/flst" )
	{
	    int c_lv = 0;
	    string c_path = "";
	    opt->childClean();
	    ctrSetS( opt, c_path );
	    while(TSYS::strSepParse(m_fnc,c_lv,'.').size())
	    {
                if( c_lv ) c_path+=".";
                c_path = c_path+TSYS::strSepParse(m_fnc,c_lv,'.');
		ctrSetS( opt, c_path );
		c_lv++;
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
                ctrSetS( opt, c_path+lst[i_a]);
	}
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
		    ctrSetI(n_type,func()->io(id)->type());
		    ctrSetI(n_mode,func()->io(id)->mode());
		    ctrSetS(n_val,getS(id));
		}	    	    	
	    }
	    else if( a_path == "/fnc/tp" )
	    {
    		ctrSetS( opt, Mess->I18N("Real"), TSYS::int2str(IO::Real).c_str() );
		ctrSetS( opt, Mess->I18N("Integer"), TSYS::int2str(IO::Integer).c_str() );
		ctrSetS( opt, Mess->I18N("Boolean"), TSYS::int2str(IO::Boolean).c_str() );
		ctrSetS( opt, Mess->I18N("String"), TSYS::int2str(IO::String).c_str() );
	    }
	    else if( a_path == "/fnc/md" )
	    {
		ctrSetS( opt, Mess->I18N("Input"), TSYS::int2str(IO::Input).c_str() );
            	ctrSetS( opt, Mess->I18N("Output"), TSYS::int2str(IO::Output ).c_str() );
		ctrSetS( opt, Mess->I18N("Return"), TSYS::int2str(IO::Return ).c_str() );
	    }
    	    else if( a_path == "/fnc/prog" ) ctrSetS( opt, ((Func *)func())->prog() );
	    else TController::cntrCmd_( a_path, opt, cmd );
	}															
        else TController::cntrCmd_( a_path, opt, cmd );	
    }
    else if( cmd==TCntrNode::Set )
    {
	if( enableStat() )
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
		    switch(col)	
		    {
			case 0:	func()->io(row)->id(ctrGetS(opt));	break;
			case 1:	func()->io(row)->name(ctrGetS(opt));	break;
			case 2:	func()->io(row)->type((IO::Type)ctrGetI(opt));	break;
			case 3:	func()->io(row)->mode((IO::Mode)ctrGetI(opt));	break;
			case 4:	setS(row,ctrGetS(opt));	break;
		    }
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

}

void Prm::postEnable()
{
    TParamContr::postEnable();
    
    vlAttElem(&v_el);
}
    
void Prm::preDisable(int flag)
{
    TParamContr::preDisable(flag);
    vlDetElem(&v_el);
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
                    ((Contr &)owner()).setS(io_id,val.getS(0,true));
	            break;
	        case TFld::Dec:
	            ((Contr &)owner()).setI(io_id,val.getI(0,true));
                    break;
                case TFld::Real:
                    ((Contr &)owner()).setR(io_id,val.getR(0,true));
                    break;
                case TFld::Bool:
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
        if( !owner().startStat() ) val.setS(mod->I18N("2:Controller stoped"),0,true);
        else if( !enableStat() )   val.setS(mod->I18N("1:Parameter disabled"),0,true);
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
                case TFld::Dec:
                    val.setI(enableStat()?owner().getI(io_id):EVAL_INT,0,true); break;
                case TFld::Real:
                    val.setR(enableStat()?owner().getR(io_id):EVAL_REAL,0,true);break;
        	case TFld::Bool:
		    val.setB(enableStat()?owner().getB(io_id):EVAL_BOOL,0,true);break;
	    }						
        }
    }catch(TError err) { disable(); }
}
																																		    

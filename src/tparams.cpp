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
#include <signal.h>
#include <sys/time.h>
 
#include "resalloc.h"
#include "tsys.h"
#include "tparamcontr.h"
#include "tparams.h"

TParamS::TParamS( ) : 
    TSubSYS("Params","Parameters",false), m_per(1000), tm_calc(0.0), 
    run_st(false), endrun(false),
    m_bd_prm("","","ParamsLogic"), m_bd_tmpl("","","TmplPrmLogic")
{
    m_prm = grpAdd("prm_");
    m_tpl = grpAdd("tpl_");
    clc_res = ResAlloc::resCreate();

    //Base parameter BD structure
    el_prm.fldAdd( new TFld("SHIFR",Mess->I18N("ID"),TFld::String,FLD_KEY|FLD_NWR,"20") );
    el_prm.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
    el_prm.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"200") );
    el_prm.fldAdd( new TFld("EN",Mess->I18N("To enable"),TFld::Bool,FLD_NOVAL,"1","false") );
    el_prm.fldAdd( new TFld("MODE",Mess->I18N("Mode"),TFld::Dec,FLD_NOVAL,"1","0") );
    el_prm.fldAdd( new TFld("PRM","",TFld::String,FLD_NOVAL,"50","") );

    //Logical level parameter IO BD structure
    el_prm_io.fldAdd( new TFld("PRM_ID",Mess->I18N("Parameter ID"),TFld::String,FLD_KEY,"10") );
    el_prm_io.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"10") );
    el_prm_io.fldAdd( new TFld("VALUE",Mess->I18N("Value"),TFld::String,0,"50") );
	
    //Logical level parameter template BD structure
    el_tmpl.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_tmpl.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
    el_tmpl.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"200") );
    el_tmpl.fldAdd( new TFld("FUNC",Mess->I18N("Structure function"),TFld::String,0,"75") );
    
    //Logical level parameter template IO BD structure
    el_tmpl_io.fldAdd( new TFld("TMPL_ID",Mess->I18N("Template ID"),TFld::String,FLD_KEY,"10") );
    el_tmpl_io.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"10") );
    el_tmpl_io.fldAdd( new TFld("ATTR_MODE",Mess->I18N("Attribute mode"),TFld::Dec,0,"1") );
    el_tmpl_io.fldAdd( new TFld("ACCS_MODE",Mess->I18N("Access mode"),TFld::Dec,0,"1") );
    el_tmpl_io.fldAdd( new TFld("VALUE",Mess->I18N("Value"),TFld::String,0,"50") );
}

TParamS::~TParamS(  )
{
    nodeDelAll();
    ResAlloc::resDelete(clc_res);    
}

string TParamS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"========================= The Parameters subsystem options =====================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"PrmClk      <period>    Calc parameter template's algoritmes period (ms);\n"
	"PrmBD       <fullname>  Messages bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	"TmplBD      <fullname>  Value bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"
	),nodePath().c_str());
				
    return(buf);
}

void TParamS::subLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
        {"help"    ,0,NULL,'h'},
        {NULL      ,0,NULL,0  }
    };

    optind=0,opterr=0;
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
    try{ m_per = atoi(TBDS::genDBGet(nodePath()+"PrmClk").c_str()); }
    catch(...) {  }
    try
    {
        string opt = TBDS::genDBGet(nodePath()+"PrmBD");
        m_bd_prm.tp    = TSYS::strSepParse(opt,0,':');
        m_bd_prm.bd    = TSYS::strSepParse(opt,1,':');
        m_bd_prm.tbl   = TSYS::strSepParse(opt,2,':');
    }catch(...) {  }
    try
    {
        string opt = TBDS::genDBGet(nodePath()+"TmplBD");
        m_bd_tmpl.tp     = TSYS::strSepParse(opt,0,':');
        m_bd_tmpl.bd     = TSYS::strSepParse(opt,1,':');
        m_bd_tmpl.tbl    = TSYS::strSepParse(opt,2,':');
    }catch(...) {  }													

    //Load DB
    loadTemplates();
    loadParams();
}

void TParamS::subSave( )
{
    //========== Save parameters =============
    TBDS::genDBSet(nodePath()+"PrmClk",TSYS::int2str(m_per));
    TBDS::genDBSet(nodePath()+"PrmBD",m_bd_prm.tp+":"+m_bd_prm.bd+":"+m_bd_prm.tbl);
    TBDS::genDBSet(nodePath()+"TmplBD",m_bd_tmpl.tp+":"+m_bd_tmpl.bd+":"+m_bd_tmpl.tbl);
    
    //Save DB
    saveTemplates();
    saveParams();
}

void TParamS::subStart( )
{
    vector<string> el_list;
    //Enable templates
    tplList(el_list);
    for( int i_el = 0; i_el < el_list.size(); i_el++ )
	try{ tplAt(el_list[i_el]).at().enable(true); }
	catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }    

    //Enable parameters
    list(el_list);
    for( int i_el = 0; i_el < el_list.size(); i_el++ )
        if( at(el_list[i_el]).at().toEnable() )
	    try{ at(el_list[i_el]).at().enable(); }
	    catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
	    
    //Make process task
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
        throw TError(nodePath().c_str(),Mess->I18N("Parameters clock no started!"));    	
}

void TParamS::subStop( )
{
    vector<string> el_list;
    
    if( run_st )
    {
        endrun = true;
        pthread_kill(pthr_tsk, SIGALRM);
        if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
            throw TError(nodePath().c_str(),Mess->I18N("Parameters clock no stoped!"));
        pthread_join(pthr_tsk, NULL);
    }
    
    //Disable parameters
    list(el_list);
    for( int i_el = 0; i_el < el_list.size(); i_el++ )
        if( at(el_list[i_el]).at().enableStat() )
            try{ at(el_list[i_el]).at().disable(); }
	    catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
	    
    //Disable templates
    tplList(el_list);
    for( int i_el = 0; i_el < el_list.size(); i_el++ )
        try{ tplAt(el_list[i_el]).at().enable(false); }
        catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
}

void TParamS::loadParams()
{
    //Load params
    try
    {
        TConfig c_el(&el_prm);
    
        int fld_cnt = 0;
        while( SYS->db().at().dataSeek(prmB(),nodePath()+"Prm/", fld_cnt++,c_el) )
        {
            string id = c_el.cfg("SHIFR").getS();
		    
            if( !present(id) )
            {
                add(id);
        	((TConfig &)at(id).at()) = c_el;
            }
            else at(id).at().load();
            c_el.cfg("SHIFR").setS("");
        }
    }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void TParamS::loadTemplates()
{
    //Load param templates
    try
    {
        TConfig c_el(&el_tmpl);
    
        int fld_cnt = 0;
        while( SYS->db().at().dataSeek(tmplB(),nodePath()+"Tmpl/", fld_cnt++,c_el) )
        {
            string id = c_el.cfg("ID").getS();
		    
            if( !tplPresent(id) )
            {
                tplAdd(id);
        	((TConfig &)tplAt(id).at()) = c_el;
            }
            else tplAt(id).at().load();
            c_el.cfg("ID").setS("");
        }
    }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void TParamS::saveParams()
{
    vector<string> ls;
    // Save parameters
    list(ls);
    for( int i_t = 0; i_t < ls.size(); i_t++ )
	at(ls[i_t]).at().save();
}

void TParamS::saveTemplates()
{
    vector<string> ls;
    // Save parameter templates
    tplList(ls);
    for( int i_t = 0; i_t < ls.size(); i_t++ )
        tplAt(ls[i_t]).at().save();
}

TBDS::SName TParamS::prmB()
{
    return owner().nameDBPrep(m_bd_prm);
}

TBDS::SName TParamS::tmplB()
{
    return owner().nameDBPrep(m_bd_tmpl);
}

void TParamS::add( const string &id )
{
    chldAdd(m_prm,new TParam(id,&el_prm));
}
   
void TParamS::tplAdd( const string &tpl )
{
    chldAdd(m_tpl,new TPrmTempl(tpl,&el_tmpl));
}

void *TParamS::Task(void *contr)
{
    struct itimerval mytim;             //Interval timer
    TParamS *prms = (TParamS *)contr;

    if(prms->m_per == 0) return(NULL);
			    
#if OSC_DEBUG
    Mess->put(prms->nodePath().c_str(),TMess::Debug,Mess->I18N("Logical level parameters thread <%d> started!"),getpid() );
#endif

    try
    {
        //Start interval timer
        mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = prms->m_per*1000;
        mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = prms->m_per*1000;
        setitimer(ITIMER_REAL,&mytim,NULL);
	
	prms->run_st = true;  
	prms->endrun = false;
	
	while( !prms->endrun )
	{
	    //Check calk time
	    unsigned long long t_cnt = SYS->shrtCnt();
	    
	    ResAlloc::resRequestR(prms->clc_res);
	    for(unsigned i_prm = 0; i_prm < prms->clc_prm.size(); i_prm++)
	    {
		try{ prms->clc_prm[i_prm].at().calc(); }
		catch(TError err)
		{ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	    }
	    ResAlloc::resReleaseR(prms->clc_res);
	        
	    prms->tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
	    pause();
	}    
    } catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }
	    
    //Stop interval timer
    mytim.it_interval.tv_sec = mytim.it_interval.tv_usec = 0;
    mytim.it_value.tv_sec    = mytim.it_value.tv_usec    = 0;
    setitimer(ITIMER_REAL,&mytim,NULL);
		
    prms->run_st = false;
		    
    return(NULL);
}

void TParamS::prmCalc( const string & id, bool val )
{
    unsigned i_prm;
    
    ResAlloc res(clc_res,true);
    for( i_prm = 0; i_prm < clc_prm.size(); i_prm++)
        if( clc_prm[i_prm].at().id() == id ) break;

    if( val && i_prm >= clc_prm.size() )
        clc_prm.push_back(at(id));
    if( !val && i_prm < clc_prm.size() )
        clc_prm.erase(clc_prm.begin()+i_prm);		    
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TParamS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> ls;
    
    if( cmd==TCntrNode::Info )
    {
	TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrInsNode("area",0,opt,a_path.c_str(),"/prm",Mess->I18N("Parameters"),0440);
	if( !SYS->shrtDBNm( ) || m_bd_prm.tp.size() || m_bd_prm.bd.size() )
	{
    	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/t_bd",Mess->I18N("Parameters BD (module:bd:table)"),0660,0,0,"str")->
		attr_("dest","select")->attr_("select","/prm/b_mod");
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/bd","",0660,0,0,"str");
            ctrMkNode("fld",opt,a_path.c_str(),"/prm/tbl","",0660,0,0,"str");
        }
	else ctrMkNode("fld",opt,a_path.c_str(),"/prm/tbl",Mess->I18N("Parameters table"),0660,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/prm/ls",Mess->I18N("Parameters"),0664,0,0,"br")->	
    	    attr_("idm","1")->attr_("s_com","add,del")->attr_("br_pref","prm_");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/clk",Mess->I18N("Calc parameters period (ms)"),0664,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/ctm",Mess->I18N("Calk time (usek)"),0444,0,0,"real");
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/prm/save",Mess->I18N("Save"),0550);		    
	ctrInsNode("area",1,opt,a_path.c_str(),"/tmpl",Mess->I18N("Templates"),0440);
	if( !SYS->shrtDBNm( ) || m_bd_tmpl.tp.size() || m_bd_tmpl.bd.size() )
	{
    	    ctrMkNode("fld",opt,a_path.c_str(),"/tmpl/t_bd",Mess->I18N("Parameter templates BD (module:bd:table)"),0660,0,0,"str")->
		attr_("dest","select")->attr_("select","/prm/b_mod");
	    ctrMkNode("fld",opt,a_path.c_str(),"/tmpl/bd","",0660,0,0,"str");
            ctrMkNode("fld",opt,a_path.c_str(),"/tmpl/tbl","",0660,0,0,"str");
        }
	else ctrMkNode("fld",opt,a_path.c_str(),"/tmpl/tbl",Mess->I18N("Parameter templates table"),0660,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/tmpl/ls",Mess->I18N("Templates"),0664,0,0,"br")->
    	    attr_("idm","1")->attr_("s_com","add,del")->attr_("br_pref","tpl_");
	ctrMkNode("comm",opt,a_path.c_str(),"/tmpl/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/tmpl/save",Mess->I18N("Save"),0550);
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/t_bd" )	ctrSetS( opt, m_bd_prm.tp );
        else if( a_path == "/prm/bd" )  ctrSetS( opt, m_bd_prm.bd );
        else if( a_path == "/prm/tbl" )	ctrSetS( opt, m_bd_prm.tbl );
        else if( a_path == "/prm/b_mod" )
        {
            opt->childClean();
            owner().db().at().modList(ls);
            ctrSetS( opt, "" ); //Default DB
            for( unsigned i_a=0; i_a < ls.size(); i_a++ )
                ctrSetS( opt, ls[i_a] );
        }														
	else if( a_path == "/prm/ls" )
        {
            opt->childClean();
            list(ls);
            for( unsigned i_a=0; i_a < ls.size(); i_a++ )
        	ctrSetS( opt, at(ls[i_a]).at().name(), ls[i_a].c_str() );
        }
	else if( a_path == "/prm/clk" )		ctrSetI( opt, m_per );
	else if( a_path == "/prm/ctm" )		ctrSetR( opt, tm_calc );
	else if( a_path == "/tmpl/t_bd" )	ctrSetS( opt, m_bd_tmpl.tp );
        else if( a_path == "/tmpl/bd" )  	ctrSetS( opt, m_bd_tmpl.bd );
        else if( a_path == "/tmpl/tbl" )	ctrSetS( opt, m_bd_tmpl.tbl );	
	else if( a_path == "/tmpl/ls" )
        {
            opt->childClean();
            tplList(ls);
            for( unsigned i_a=0; i_a < ls.size(); i_a++ )
        	ctrSetS( opt, tplAt(ls[i_a]).at().name(), ls[i_a].c_str() );
        }									    
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }	
    else if( cmd==TCntrNode::Set )
    {	
	if( a_path == "/prm/t_bd" )	m_bd_prm.tp = ctrGetS(opt);
        else if( a_path == "/prm/bd" )  m_bd_prm.bd = ctrGetS(opt);
        else if( a_path == "/prm/tbl" ) m_bd_prm.tbl = ctrGetS(opt);
	else if( a_path == "/prm/ls" )
        {
            if( opt->name() == "add" )
	    { 
		add(opt->attr("id"));
                at(opt->attr("id")).at().name(opt->text());				
	    }
            else if( opt->name() == "del" )     chldDel(m_prm,opt->attr("id"),-1,1);
        }
	else if( a_path == "/prm/clk" )         m_per = ctrGetI(opt);
	else if( a_path == "/prm/load" )    	loadParams();
        else if( a_path == "/prm/save" )	saveParams();		
	else if( a_path == "/tmpl/t_bd" )	m_bd_tmpl.tp = ctrGetS(opt);
        else if( a_path == "/tmpl/bd" )  	m_bd_tmpl.bd = ctrGetS(opt);
        else if( a_path == "/tmpl/tbl" ) 	m_bd_tmpl.tbl = ctrGetS(opt);
	else if( a_path == "/tmpl/ls" )
        {
            if( opt->name() == "add" )
	    { 
		tplAdd(opt->attr("id"));
                tplAt(opt->attr("id")).at().name(opt->text());				
	    }
            else if( opt->name() == "del" )     chldDel(m_tpl,opt->attr("id"),-1,1);
        }
	else if( a_path == "/tmpl/load" )	loadTemplates();
	else if( a_path == "/tmpl/save" )       saveTemplates();
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
}

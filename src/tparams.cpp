
//OpenSCADA system file: tparams.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
#include <sys/times.h>
 
#include "resalloc.h"
#include "tsys.h"
#include "tparamcontr.h"
#include "tparams.h"

TParamS::TParamS( ) : 
    TSubSYS("Params","Parameters",false), m_per(1000), tm_calc(0.0), prc_st(false)    
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
    el_prm.fldAdd( new TFld("PRM","",TFld::String,FLD_NOVAL,"100","") );

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
    
    //Create calc timer
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
}

TParamS::~TParamS(  )
{
    timer_delete(tmId);
    nodeDelAll();
    ResAlloc::resDelete(clc_res);    
}

string TParamS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"========================= Subsystem \"Parameters\" options =====================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"PrmClk  <period>       Calc parameter template's algoritmes period (ms).\n\n"
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
    m_per = atoi(TBDS::genDBGet(nodePath()+"PrmClk",TSYS::int2str(m_per)).c_str());

    //Load DB
    loadTemplates();
    loadParams();
}

void TParamS::subSave( )
{
    //========== Save parameters =============
    TBDS::genDBSet(nodePath()+"PrmClk",TSYS::int2str(m_per));
    
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
	    
    //Start interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = (m_per*1000000)/1000000000;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = (m_per*1000000)%1000000000;
    timer_settime(tmId, 0, &itval, NULL);
}

void TParamS::subStop( )
{
    vector<string> el_list;
    
    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec = 
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);	
    if( TSYS::eventWait( prc_st, false, nodePath()+"stop",5) )
        throw TError(nodePath().c_str(),Mess->I18N("Parameters calc thread no stoped!"));
    
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
	vector<string> tdb_ls, db_ls;
	
	//Search and create new parameters
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
        {
            SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
            for( int i_db = 0; i_db < db_ls.size(); i_db++ )
            {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
                int fld_cnt=0;
    		while( SYS->db().at().dataSeek(wbd+"."+subId(),nodePath()+"Prm/",fld_cnt++,c_el) )
    		{
        	    string id = c_el.cfg("SHIFR").getS();
        	    if( !present(id) ) add(id,(wbd==SYS->workDB())?"*.*":wbd);
        	    c_el.cfg("SHIFR").setS("");
    		}
	    }
	}
	
	//Load present parameters
	list(tdb_ls);
	for( int i_p = 0; i_p < tdb_ls.size(); i_p++ )
	    at(tdb_ls[i_p]).at().load();
	    
    }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void TParamS::loadTemplates()
{
    //Load param templates
    try
    {
        TConfig c_el(&el_tmpl);
	vector<string> tdb_ls, db_ls;
	
	//Search and create new parameter templates
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
        {
            SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
            for( int i_db = 0; i_db < db_ls.size(); i_db++ )
            {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
                int fld_cnt=0;    
    		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_tmpl",nodePath()+"Tmpl/", fld_cnt++,c_el) )
    		{
        	    string id = c_el.cfg("ID").getS();
        	    if( !tplPresent(id) ) tplAdd(id,(wbd==SYS->workDB())?"*.*":wbd);
        	    c_el.cfg("ID").setS("");
    		}
	    }
	}
	
	//Load parameter templates
	tplList(tdb_ls);
	for( int i_pt = 0; i_pt < tdb_ls.size(); i_pt++ )
    	    tplAt(tdb_ls[i_pt]).at().load();
	
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

void TParamS::add( const string &id, const string &idb )
{
    chldAdd(m_prm,new TParam(id,idb,&el_prm));
}
   
void TParamS::tplAdd( const string &tpl, const string &idb )
{
    chldAdd(m_tpl,new TPrmTempl(tpl,idb,&el_tmpl));
}

void TParamS::Task(union sigval obj)
{
    TParamS *prms = (TParamS *)obj.sival_ptr;
    if( prms->prc_st )	return;
    prms->prc_st = true;
    
    //Calc and check calk time
    try
    {
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
    } catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }
    
    prms->prc_st = false;
}

void TParamS::setPrmCalc( const string & id, bool val )
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

void TParamS::calcPer( int per )
{
    m_per = per;
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = (m_per*1000000)/1000000000;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = (m_per*1000000)%1000000000;
    timer_settime(tmId, 0, &itval, NULL);
    timer_gettime(tmId, &itval);
    m_per = itval.it_interval.tv_sec*1000+itval.it_interval.tv_nsec/1000000;
}

void TParamS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> ls;
    
    if( cmd==TCntrNode::Info )
    {
	TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrMkNode("area",opt,0,a_path.c_str(),"/prm",Mess->I18N("Parameters"),0440);
	ctrMkNode("list",opt,-1,a_path.c_str(),"/prm/ls",Mess->I18N("Parameters"),0664,0,0,4,"tp","br","idm","1","s_com","add,del","br_pref","prm_");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/clk",Mess->I18N("Calc parameters period (ms)"),0664,0,0,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/ctm",Mess->I18N("Calk time (usek)"),0444,0,0,1,"tp","real");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/save",Mess->I18N("Save"),0550);		    
	ctrMkNode("area",opt,1,a_path.c_str(),"/tmpl",Mess->I18N("Templates"),0440);
	ctrMkNode("list",opt,-1,a_path.c_str(),"/tmpl/ls",Mess->I18N("Templates"),0664,0,0,4,"tp","br","idm","1","s_com","add,del","br_pref","tpl_");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/tmpl/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,-1,a_path.c_str(),"/tmpl/save",Mess->I18N("Save"),0550);
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/ls" )
        {
            opt->childClean();
            list(ls);
            for( unsigned i_a=0; i_a < ls.size(); i_a++ )
        	ctrSetS( opt, at(ls[i_a]).at().name(), ls[i_a].c_str() );
        }
	else if( a_path == "/prm/clk" )	ctrSetI( opt, m_per );
	else if( a_path == "/prm/ctm" )	ctrSetR( opt, tm_calc );	
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
	if( a_path == "/prm/ls" )
        {
            if( opt->name() == "add" )
	    { 
		add(opt->attr("id"));
                at(opt->attr("id")).at().name(opt->text());				
	    }
            else if( opt->name() == "del" )     chldDel(m_prm,opt->attr("id"),-1,1);
        }
	else if( a_path == "/prm/clk" )         calcPer(ctrGetI(opt));
	else if( a_path == "/prm/load" )    	loadParams();
        else if( a_path == "/prm/save" )	saveParams();		
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

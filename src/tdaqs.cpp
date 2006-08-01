
//OpenSCADA system file: tdaqs.cpp
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

#include "tsys.h"
#include "tmess.h"
#include "tcontroller.h"
//#include "ttipdaq.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tdaqs.h"


TDAQS::TDAQS( ) : TSubSYS("DAQ","Data acquisition",true), el_err("Error")
{
    m_tmplib = grpAdd("tmplb_");
    
    //Lib's db structure
    lb_el.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"10") );
    lb_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,FLD_NOFLG,"50") );
    lb_el.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,FLD_NOFLG,"300") );
    lb_el.fldAdd( new TFld("DB",Mess->I18N("Data base"),TFld::String,FLD_NOFLG,"30") );
    
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
    
    //Error atributes
    el_err.fldAdd( new TFld("err",Mess->I18N("Error"),TFld::String,FLD_NWR|FLD_DRD) );
}

TDAQS::~TDAQS( )
{
    nodeDelAll();
}

void TDAQS::preDisable(int flag)
{
    subStop();
}

void TDAQS::subLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"       ,0,NULL,'h'},
	{NULL         ,0,NULL,0  }
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

    //=========== Load parameter templates libraries =============
    try
    {
        //Search and create new libraries
        TConfig c_el(&elLib());
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
                while(SYS->db().at().dataSeek(wbd+"."+tmplLibTable(),"",lib_cnt++,c_el) )
                {
                    string l_id = c_el.cfg("ID").getS();
                    if(!tmplLibPresent(l_id)) tmplLibReg(new TPrmTmplLib(l_id.c_str(),"",(wbd==SYS->workDB())?"*.*":wbd));
                    c_el.cfg("ID").setS("");
                }
            }
        }
	
        //- Search into config file -
        int lib_cnt = 0;
	while(SYS->db().at().dataSeek("",nodePath()+"tmplib",lib_cnt++,c_el) )
        {
            string l_id = c_el.cfg("ID").getS();
            if(!tmplLibPresent(l_id)) tmplLibReg(new TPrmTmplLib(l_id.c_str(),"","*.*"));
            c_el.cfg("ID").setS("");
        }
			    
        //- Load present libraries -
        tmplLibList(tdb_ls);
        for( int l_id = 0; l_id < tdb_ls.size(); l_id++ )
            tmplLibAt(tdb_ls[l_id]).at().load();
    }catch( TError err )
    {
        Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Load template's libraries error."));
    }
    
    //========== Load parameters =============
    try
    {
	AutoHD<TTipDAQ> wmod;
	vector<string> mod_ls, tdb_ls, db_ls;
	
	modList(mod_ls);
	for( int i_md = 0; i_md < mod_ls.size(); i_md++ )
	{
	    wmod = at(mod_ls[i_md]);
	    TConfig g_cfg(&wmod.at());
	
	    //Search into DB and create new controllers
	    SYS->db().at().modList(tdb_ls);	
	    for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
    	    {
        	SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
        	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
        	{
		    string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		    int fld_cnt=0;
		    while( SYS->db().at().dataSeek(wbd+"."+subId()+"_"+wmod.at().modId(),"",fld_cnt++,g_cfg) )
		    {
			string m_id = g_cfg.cfg("ID").getS();
			try
            		{
			    if( !wmod.at().present(m_id) )
				wmod.at().add(m_id,(wbd==SYS->workDB())?"*.*":wbd);
			}catch(TError err) 
			{ 
			    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
			    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Add controller <%s> error."),m_id.c_str());
			}
            		g_cfg.cfg("ID").setS("");
		    }
		}
	    }
	    //Search into config file and create new controllers
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek("",wmod.at().nodePath()+"DAQ/",fld_cnt++,g_cfg) )
	    {
		string m_id = g_cfg.cfg("ID").getS();
		try
    		{
		    if( !wmod.at().present(m_id) )
			wmod.at().add(m_id,"*.*");
		}catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Add controller <%s> error."),m_id.c_str());
		}
            	g_cfg.cfg("ID").setS("");
	    }	    
	    
	    //Load present controllers
	    wmod.at().list(tdb_ls);
	    for( int i_c = 0; i_c < tdb_ls.size(); i_c++ )
		wmod.at().at(tdb_ls[i_c]).at().load();
	}
    }catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
	
    //Load modules
    TSubSYS::subLoad( );
}

void TDAQS::subSave(  )
{	
    //========== Save parameters =============

    //Save all controllers    
    vector<string> m_l, c_l;
    modList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    try{ at(m_l[i_m]).at().at(c_l[i_c]).at().save( ); }
	    catch(TError err) 
	    { 
		Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Save controller <%s> error."),(m_l[i_m]+"."+c_l[i_c]).c_str());
	    }
	}
    }
    
    //=========== Save template's libraries =============
    vector<string> ls;
    tmplLibList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
        tmplLibAt(ls[l_id]).at().save();
    
    //Save modules
    TSubSYS::subSave( );					    
}

void TDAQS::subStart(  )         
{
    vector<string> m_l, tmpl_lst;
    
    modList(m_l);    
    //Enable controllers
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if( !cntr.at().enableStat() && cntr.at().toEnable() )
	        try{ cntr.at().enable(); }
		catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Enable controller <%s> error."),(m_l[i_m]+"."+c_l[i_c]).c_str());
		}
	}
    }  

    //Start template's libraries
    tmplLibList(tmpl_lst);
    for(int i_lb=0; i_lb < tmpl_lst.size(); i_lb++ )
        tmplLibAt(tmpl_lst[i_lb]).at().start(true);

    //Start controllers
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
        at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if( !cntr.at().startStat() && cntr.at().toStart() )
		try{ cntr.at().start( ); }
		catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Start controller <%s> error."),(m_l[i_m]+"."+c_l[i_c]).c_str());
		}
	}
    }
    
    //Start template's libraries
    tmplLibList(m_l);
    for(int i_lb=0; i_lb < m_l.size(); i_lb++ )
        tmplLibAt(m_l[i_lb]).at().start(true);
    
    TSubSYS::subStart( );
}

void TDAQS::subStop( )
{
    vector<string> m_l;
    
    //Stop
    modList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if( cntr.at().startStat() )
		try{ cntr.at().stop( ); }
		catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Stop controller <%s> error."),(m_l[i_m]+"."+c_l[i_c]).c_str()); 
		}
	}
    }
    //Disable
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {	
	vector<string> c_l;
        at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
        {
            AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
            if( cntr.at().enableStat() )
        	try{ cntr.at().disable( ); }
        	catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Disable controller <%s> error."),(m_l[i_m]+"."+c_l[i_c]).c_str());
		}
        }
    }
    
    //Stop template's libraries
    tmplLibList(m_l);
    for(int i_lb=0; i_lb < m_l.size(); i_lb++ )
        tmplLibAt(m_l[i_lb]).at().start(false);
	
    TSubSYS::subStop( );
}

string TDAQS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"=================== Subsystem \"Data acquisition\" options ================\n"
	"------------ Parameters of section <%s> in config file -----------\n\n"
	),nodePath().c_str());

    return(buf);
}

void TDAQS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/tmplb_",Mess->I18N("Template library"),0444,"root","root",1,"list","/tpllibs/lb");
	ctrMkNode("area",opt,0,"/bd",Mess->I18N("Subsystem"),0440);
	ctrMkNode("comm",opt,-1,"/bd/load_bd",Mess->I18N("Load"),0440);
	ctrMkNode("comm",opt,-1,"/bd/upd_bd",Mess->I18N("Save"),0440);
	ctrMkNode("area",opt,1,"/tpllibs",Mess->I18N("Template libraries"));
	ctrMkNode("list",opt,-1,"/tpllibs/lb",Mess->I18N("Template libraries"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","tmplb_");
	ctrMkNode("fld",opt,-1,"/help/g_help",Mess->I18N("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/tpllibs/lb" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
        {
	    vector<string> lst;
            tmplLibList(lst);
            for( unsigned i_a=0; i_a < lst.size(); i_a++ )
		opt->childAdd("el")->attr("id",lst[i_a])->text(tmplLibAt(lst[i_a]).at().name());
        }
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
            tmplLibReg(new TPrmTmplLib(opt->attr("id").c_str(),opt->text().c_str(),"*.*"));
        if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )
	    tmplLibUnreg(opt->attr("id"),1);
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )		opt->text(optDescr());
    else if( a_path == "/bd/load_bd" && ctrChkNode(opt,"set",0440,"root","root",SEQ_RD) )	subLoad();
    else if( a_path == "/bd/upd_bd" && ctrChkNode(opt,"set",0440,"root","root",SEQ_RD) )	subSave();
    else TSubSYS::cntrCmdProc(opt);
}

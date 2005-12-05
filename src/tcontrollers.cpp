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

#include "tsys.h"
#include "tmess.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tcontrollers.h"


TControllerS::TControllerS( ) 
	: TSubSYS("Controller","Controllers",true), m_bd("","","Controllers")
{
    fldAdd( new TFld("NAME","Controller's name.",TFld::String,FLD_KEY,"20") );
    fldAdd( new TFld("MODUL","Module(plugin) of type controler.",TFld::String,FLD_KEY,"20") );
    fldAdd( new TFld("BDTYPE","Type controller's BD.",TFld::String,0,"20") );
    fldAdd( new TFld("BDNAME","Name controller's BD.",TFld::String,0,"50") );
    fldAdd( new TFld("TABLE","Name controller's Table.",TFld::String,0,"20","ContrTbl") );
}

TControllerS::~TControllerS(  )
{

}

void TControllerS::preDisable(int flag)
{
    subStop();
}

void TControllerS::subLoad( )
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
    
    //========== Load parameters =============
    string opt;  
    try
    { 
	string opt = TBDS::genDBGet(nodePath()+"GenBD");
	m_bd.tp	= TSYS::strSepParse(opt,0,':');
	m_bd.bd = TSYS::strSepParse(opt,1,':');
	m_bd.tbl= TSYS::strSepParse(opt,2,':');
    }
    catch(...) {  }
    
    //Load DB
    try
    {
	TConfig g_cfg(this);
	AutoHD<TTable> tbl = SYS->db().at().open(BD());
	
	int fld_cnt=0;
	while( SYS->db().at().dataSeek(tbl,nodePath()+"Contr/", fld_cnt++,g_cfg) )
	{
	    try
	    {
		SName CntrS(g_cfg.cfg("MODUL").getS().c_str(), g_cfg.cfg("NAME").getS().c_str());
		TBDS::SName n_bd(g_cfg.cfg("BDTYPE").getS().c_str(), g_cfg.cfg("BDNAME").getS().c_str(), g_cfg.cfg("TABLE").getS().c_str());
		
		if( !((TTipController &)modAt(CntrS.tp).at()).present(CntrS.obj) )
		    ((TTipController &)modAt(CntrS.tp).at()).add(CntrS.obj,n_bd);
		((TTipController &)modAt(CntrS.tp).at()).at(CntrS.obj).at().load();
	    }
	    catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	    g_cfg.cfg("MODUL").setS("");
	    g_cfg.cfg("NAME").setS("");		    
	}
	if(!tbl.freeStat())
        {
	    tbl.free();	
	    SYS->db().at().close(BD());
	}
    }catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    
    //Load modules
    TSubSYS::subLoad( );
}

void TControllerS::subSave(  )
{	
    //========== Save parameters =============
    TBDS::genDBSet(nodePath()+"GenBD",m_bd.tp+":"+m_bd.bd+":"+m_bd.tbl);

    //Save all controllers    
    vector<string> m_l;
    modList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)modAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    try{ ((TTipController &)modAt(m_l[i_m]).at()).at(c_l[i_c]).at().save( ); }
	    catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	}
    }
    //Save modules
    TSubSYS::subSave( );					    
}

TBDS::SName TControllerS::BD() 
{ 
    return owner().nameDBPrep(m_bd); 
}

void TControllerS::subStart(  )         
{
    vector<string> m_l;
    modList(m_l);
    //Enable controllers
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)modAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)modAt(m_l[i_m]).at()).at(c_l[i_c]);	    
	    if( !cntr.at().enableStat() && cntr.at().toEnable() )
	        try{ cntr.at().enable(); }
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	}
    }
    //Start controllers
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
        ((TTipController &)modAt(m_l[i_m]).at()).list(c_l);		
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)modAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if( !cntr.at().startStat() && cntr.at().toStart() )
		try{ cntr.at().start( ); }
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	}
    }
    TSubSYS::subStart( );
}

void TControllerS::subStop( )
{
    vector<string> m_l;
    modList(m_l);
    //Stop
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)modAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)modAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if( cntr.at().startStat() )
		try{ cntr.at().stop( ); }
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }	    	
	}
    }
    //Disable
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {	
	vector<string> c_l;
        ((TTipController &)modAt(m_l[i_m]).at()).list(c_l);		
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
        {
            AutoHD<TController> cntr = ((TTipController &)modAt(m_l[i_m]).at()).at(c_l[i_c]);
            if( cntr.at().enableStat() )
        	try{ cntr.at().disable( ); }
        	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
        }
    }
    TSubSYS::subStop( );
}

string TControllerS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"======================== The controller subsystem options =================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"GenBD     <fullname>       generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"
	),nodePath().c_str());

    return(buf);
}


//================== Controll functions ========================
void TControllerS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrInsNode("area",0,opt,a_path.c_str(),"/bd",Mess->I18N("Subsystem"),0440);
	if( !SYS->shrtDBNm( ) || m_bd.tp.size() || m_bd.bd.size() )
	{	    
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/t_bd",Mess->I18N("BD (module:bd:table)"),0660,0,0,"str")->
		attr_("dest","select")->attr_("select","/bd/b_mod");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/bd","",0660,0,0,"str");	    
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/tbl","",0660,0,0,"str");
	}
	else ctrMkNode("fld",opt,a_path.c_str(),"/bd/tbl",Mess->I18N("Table"),0660,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/bd/load_bd",Mess->I18N("Load"));
	ctrMkNode("comm",opt,a_path.c_str(),"/bd/upd_bd",Mess->I18N("Save"));
	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/bd/t_bd" )     ctrSetS( opt, m_bd.tp );
	else if( a_path == "/bd/bd" )  ctrSetS( opt, m_bd.bd );
	else if( a_path == "/bd/tbl" ) ctrSetS( opt, m_bd.tbl );
	else if( a_path == "/bd/b_mod" )
	{
	    vector<string> list;	
	    owner().db().at().modList(list);
	    opt->childClean();
	    ctrSetS( opt, "" );
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
	else if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bd/t_bd" )       	m_bd.tp    = ctrGetS( opt );
	else if( a_path == "/bd/bd" )  		m_bd.bd    = ctrGetS( opt );
	else if( a_path == "/bd/tbl" )		m_bd.tbl   = ctrGetS( opt );
	else if( a_path == "/bd/load_bd" )	subLoad();
	else if( a_path == "/bd/upd_bd" )	subSave();
	else TSubSYS::cntrCmd_( a_path, opt, cmd );	
    }
}


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
#include "tkernel.h"
#include "tmessage.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tcontrollers.h"


const char *TControllerS::o_name = "TControllerS";

TControllerS::TControllerS( TKernel *app ) 
	: TGRPModule(app,"Controller"), m_bd("", "", "generic") 
{
    s_name = "Controllers";

    fldAdd( new TFld("NAME","Controller's name.",T_STRING|F_KEY,"20") );
    fldAdd( new TFld("MODUL","Module(plugin) of type controler.",T_STRING|F_KEY,"20") );
    fldAdd( new TFld("BDTYPE","Type controller's BD.",T_STRING,"20","direct_dbf") );
    fldAdd( new TFld("BDNAME","Name controller's BD.",T_STRING,"50","./DATA") );
    fldAdd( new TFld("TABLE","Name controller's Table.",T_STRING,"20","contr.dbf") );    
}

TControllerS::~TControllerS(  )
{
    gmdStop();    
}

void TControllerS::gmdInit( )
{
    loadBD();
}

TBDS::SName TControllerS::BD() 
{ 
    return owner().nameDBPrep(m_bd); 
}

void TControllerS::gmdStart(  )         
{
    vector<string> m_l;
    gmdList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)gmdAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)gmdAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if( !cntr.at().startStat() && cntr.at().toStart() )
		try{ cntr.at().start( ); }
		catch(TError err) { mPutS("SYS",MESS_ERR,err.what()); }
	}
    }							    
}

void TControllerS::gmdStop( )
{
    vector<string> m_l;
    gmdList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)gmdAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)gmdAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if( cntr.at().startStat() )
		try{ cntr.at().stop( ); }
		catch(TError err) { mPutS("SYS",MESS_ERR,err.what()); }
	}
    }							    
}

string TControllerS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"======================== The controller subsystem options =================\n"
	"    --TCModPath = <path>   set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path  <path>           set modules <path>;\n"
    	"GenBD     <fullname>       generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"
	),gmdName().c_str());

    return(buf);
}

void TControllerS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"       ,0,NULL,'h'},
	{"TCModPath"  ,1,NULL,'m'},
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'm': DirPath  = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TControllerS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    string opt;
  
    try{ DirPath = gmdCfgNode()->childGet("id","mod_path")->text(); }
    catch(...) {  }
    
    try
    { 
	string opt = gmdCfgNode()->childGet("id","GenBD")->text(); 
	m_bd.tp	= TSYS::strSepParse(opt,0,':');
	m_bd.bd = TSYS::strSepParse(opt,1,':');
	m_bd.tbl= TSYS::strSepParse(opt,2,':');
    }
    catch(...) {  }
}

void TControllerS::loadBD()
{
    vector<string> list_el;
    TConfig g_cfg(this);        
    
    try
    {
	AutoHD<TTable> tbl = owner().BD().open(BD());
	int fld_cnt=0;
        while( tbl.at().fieldSeek(fld_cnt++,g_cfg) )
	{
	    try
	    {
		SName CntrS(g_cfg.cfg("MODUL").getS().c_str(), g_cfg.cfg("NAME").getS().c_str());
		TBDS::SName n_bd(g_cfg.cfg("BDTYPE").getS().c_str(), g_cfg.cfg("BDNAME").getS().c_str(), g_cfg.cfg("TABLE").getS().c_str());
		
		((TTipController &)gmdAt(CntrS.tp).at()).add(CntrS.obj,n_bd);
		AutoHD<TController> ctr = ((TTipController &)gmdAt(CntrS.tp).at()).at(CntrS.obj);
		ctr.at().load();
		if( !ctr.at().enableStat() && ctr.at().toEnable() ) 
		    ctr.at().enable(); 
	    }
	    catch(TError err) { mPutS("SYS",MESS_ERR,err.what()); }
	}
	tbl.free();	
	owner().BD().close(BD());
    }catch(TError err) { mPutS("SYS",MESS_ERR,err.what()); }    
}

void TControllerS::saveBD(  )
{	
    //Save all controllers    
    vector<string> m_l;
    gmdList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)gmdAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    try{ ((TTipController &)gmdAt(m_l[i_m]).at()).at(c_l[i_c]).at().save( ); }
	    catch(TError err) { mPutS("SYS",MESS_ERR,err.what()); }
	}
    }							    
}

//================== Controll functions ========================
void TControllerS::ctrStat_( XMLNode *inf )
{
    char *dscr="dscr";

    TGRPModule::ctrStat_( inf );
    
    char *i_cntr = 
    	"<area id='a_bd' acs='0440'>"
	 "<fld id='t_bd' acs='0660' tp='str' dest='select' select='/a_bd/b_mod'/>"
	 "<fld id='bd' acs='0660' tp='str'/>"
	 "<fld id='tbl' acs='0660' tp='str'/>"
	 "<comm id='load_bd'/>"
	 "<comm id='upd_bd'/>"
	"</area>";
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("Subsystem"));
    if( owner().genDB( ) )
    {
	n_add->childGet(0)->attr("acs","0");
	n_add->childGet(1)->attr("acs","0");
	n_add->childGet(2)->attr(dscr,Mess->I18N("Table"));
    }
    else n_add->childGet(0)->attr(dscr,Mess->I18N("BD (module:bd:table)"));
    n_add->childGet(3)->attr(dscr,Mess->I18N("Load from BD"));
    n_add->childGet(4)->attr(dscr,Mess->I18N("Save to BD"));

    //Insert to Help
    char *i_help = "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";

    n_add = inf->childGet("id","help")->childAdd();
    n_add->load(i_help);
    n_add->attr(dscr,Mess->I18N("Options help"));                
}

void TControllerS::ctrDinGet_( const string &a_path, XMLNode *opt )
{    
    if( a_path == "/a_bd/t_bd" )     ctrSetS( opt, m_bd.tp );
    else if( a_path == "/a_bd/bd" )  ctrSetS( opt, m_bd.bd );
    else if( a_path == "/a_bd/tbl" ) ctrSetS( opt, m_bd.tbl );
    else if( a_path == "/a_bd/b_mod" )
    {
	vector<string> list;	
	owner().BD().gmdList(list);
	opt->childClean();
	ctrSetS( opt, "" );
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );
    }
    else if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
    else TGRPModule::ctrDinGet_( a_path, opt );
}

void TControllerS::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_bd/t_bd" )       	m_bd.tp    = ctrGetS( opt );
    else if( a_path == "/a_bd/bd" )  	m_bd.bd    = ctrGetS( opt );
    else if( a_path == "/a_bd/tbl" )	m_bd.tbl   = ctrGetS( opt );
    else if( a_path == "/a_bd/load_bd" )	loadBD();
    else if( a_path == "/a_bd/upd_bd" )	saveBD();
    else TGRPModule::ctrDinSet_( a_path, opt );
}


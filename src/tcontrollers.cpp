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


SFld TControllerS::gen_elem[] =
{
    {"NAME"    ,"Controller's name."               ,T_STRING,""           ,"20"},
    {"MODUL"   ,"Module(plugin) of type controler.",T_STRING,""           ,"20"},
    {"BDTYPE"  ,"Type controller's BD."            ,T_STRING,"direct_dbf" ,"20"},
    {"BDNAME"  ,"Name controller's BD."            ,T_STRING,"./DATA"     ,"50"},
    {"TABLE"   ,"Name controller's Table."         ,T_STRING,"contr.dbf"  ,"20"}
};

const char *TControllerS::o_name = "TControllerS";

TControllerS::TControllerS( TKernel *app ) 
	: TElem(""), TGRPModule(app,"Controller"), m_bd("direct_dbf", "./DATA", "generic.dbf") 
{
    s_name = "Controllers";
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SFld); i++) elAdd(&gen_elem[i]);    
}

TControllerS::~TControllerS(  )
{
    gmdStop();
    
    /*
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
    	try{ del( m_list[i_m] ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    */
}

void TControllerS::gmdInit( )
{
    loadBD();
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
	    if( cntr.at().toStart() )
		try{ cntr.at().start( ); }
		catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
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
		catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	}
    }							    
}

string TControllerS::opt_descr( )
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath  = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TControllerS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    string opt;
  
    try{ DirPath = gmdXMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    
    try
    { 
	string opt = gmdXMLCfgNode()->get_child("id","GenBD")->get_text(); 
    	int pos = 0;
        m_bd.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd.tp.size() ) m_bd.tp = Owner().DefBDType;
    if( !m_bd.bd.size() ) m_bd.bd = Owner().DefBDName;	    
}

void TControllerS::loadBD()
{
    TConfig *g_cfg = new TConfig(this);
    try
    {
	g_cfg->cfLoadAllValBD( Owner().BD().open(m_bd).at() );
	Owner().BD().close(m_bd);
    //Create controller 
	for(unsigned i_cfg = 0; i_cfg < g_cfg->cfSize(); i_cfg++)
	    try
	    {
		SCntrS CntrS(g_cfg->cfg("MODUL", i_cfg).getS(), g_cfg->cfg("NAME", i_cfg).getS());
		SBDS n_bd(g_cfg->cfg("BDTYPE", i_cfg).getS(), g_cfg->cfg("BDNAME", i_cfg).getS(), g_cfg->cfg("TABLE", i_cfg).getS());
		((TTipController &)gmdAt(CntrS.tp).at()).add(CntrS.obj,n_bd);

		AutoHD<TController> ctr = ((TTipController &)gmdAt(CntrS.tp).at()).at(CntrS.obj);
		if( ctr.at().toEnable() ) ctr.at().enable(); 
	    }
	    catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    }catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    delete g_cfg;
}

void TControllerS::saveBD(  )
{
    AutoHD<TTable> tbl = Owner().BD().open( m_bd, true );
    TConfig *g_cfg = new TConfig(this);    
    g_cfg->cfLoadAllValBD( tbl.at() );  //Load temp config
    //Clean all BD
    tbl.at().clean(); 		//Clean BD
    elUpdateBDAttr( tbl.at() );	//Update BD struct
    tbl.at().save();		//Save BD
    tbl.free();
    Owner().BD().close(m_bd);
    //Clean controller type BD
    for(unsigned i_cfg = 0; i_cfg < g_cfg->cfSize(); i_cfg++)
	try
	{
	    SBDS nm_bd(g_cfg->cfg("BDTYPE", i_cfg).getS(), g_cfg->cfg("BDNAME", i_cfg).getS(), g_cfg->cfg("TABLE", i_cfg).getS());
	    AutoHD<TTable> tbl = Owner().BD().open( nm_bd );
	    tbl.at().clean();                    //Clean BD
	    tbl.at().save();                     //Save BD
	    tbl.free();
	    Owner().BD().close(nm_bd);
	}
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    delete g_cfg;
    
    //Save all controllers    
    vector<string> m_l;
    gmdList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)gmdAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)gmdAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if( cntr.at().toStart() )
		try{ cntr.at().save( true ); }
		catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	}
    }							    
}

void TControllerS::gmdDel( const string &name )
{
    vector<string> c_l;
    ((TTipController &)gmdAt(name).at()).list(c_l);
    for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	((TTipController &)gmdAt(name).at()).del(c_l[i_c]);

    TGRPModule::gmdDel( name );
}

//================== Controll functions ========================
void TControllerS::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";

    TGRPModule::ctr_fill_info( inf );
    
    char *i_cntr = 
    	"<area id='a_bd' acs='0440'>"
	" <fld id='t_bd' acs='0660' tp='str' dest='select' select='/a_bd/b_mod'/>"
	" <fld id='bd' acs='0660' tp='str'/>"
	" <fld id='tbl' acs='0660' tp='str'/>"
	" <comm id='load_bd'/>"
	" <comm id='upd_bd'/>"
	" <list id='b_mod' tp='str' hide='1'/>"
	"</area>";
    
    XMLNode *n_add = inf->ins_child(0);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("BD (module:bd:table)"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Load"));
    n_add->get_child(4)->set_attr(dscr,Mess->I18N("Save"));

    //Insert to Help
    char *i_help = "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";

    n_add = inf->get_child("id","help")->add_child();
    n_add->load_xml(i_help);
    n_add->set_attr(dscr,Mess->I18N("Options help"));                
}

void TControllerS::ctr_din_get_( const string &a_path, XMLNode *opt )
{    
    if( a_path == "/a_bd/t_bd" )     ctr_opt_setS( opt, m_bd.tp );
    else if( a_path == "/a_bd/bd" )  ctr_opt_setS( opt, m_bd.bd );
    else if( a_path == "/a_bd/tbl" ) ctr_opt_setS( opt, m_bd.tbl );
    else if( a_path == "/a_bd/b_mod" )
    {
	vector<string> list;	
	Owner().BD().gmdList(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }
    else if( a_path == "/help/g_help" ) ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}

void TControllerS::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_bd/t_bd" )       m_bd.tp    = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/bd" )    m_bd.bd    = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/tbl" )   m_bd.tbl   = ctr_opt_getS( opt );
    else TGRPModule::ctr_din_set_( a_path, opt );
}

void TControllerS::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_bd/load_bd" )     loadBD();
    else if( a_path == "/a_bd/upd_bd" ) saveBD();
    else TGRPModule::ctr_cmd_go_( a_path, fld, rez );
}


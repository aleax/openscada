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
#include "tmodule.h"
#include "tprotocols.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================

SFld TTransportS::gen_elem[] =
{
    {"NAME"    ,"Transport name."               ,T_STRING       ,""     ,"20"},
    {"DESCRIPT","Transport description."        ,T_STRING       ,""     ,"50"},    
    {"MODULE"  ,"Type transport (module name)." ,T_STRING       ,""     ,"20"},
    {"ADDR"    ,"Transport address."            ,T_STRING       ,""     ,"50"},
    {"PROT"    ,"Assign transport protocol."    ,T_STRING       ,""     ,"20"},
    {"TYPE"    ,"Transport type (Input;Output).",T_BOOL|T_SELECT,"false","1" ,"false;true","Input;Output"}
};

const char *TTransportS::o_name = "TTransportS";

TTransportS::TTransportS( TKernel *app ) 
    : TGRPModule(app,"Transport"), TElem(""), m_bd("","","transport.dbf") 
{
    s_name = "Transports"; 
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SFld); i++) elAdd(&gen_elem[i]);
}

TTransportS::~TTransportS(  )
{
    /*
    vector<STrS> list;
    in_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ in_del( list[i_m] ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }

    out_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ out_del( list[i_m] ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }    
    */
}

void TTransportS::gmdInit( )
{
    loadBD();
}

void TTransportS::gmdStart( )
{    
    vector<string> t_lst, o_lst;
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TModule> mod = gmdAt(t_lst[i_t]);
	((TTipTransport &)mod.at()).in_list(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try{((TTipTransport &)mod.at()).in_at(o_lst[i_o]).at().start();}catch(...){}
	((TTipTransport &)mod.at()).out_list(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try{((TTipTransport &)mod.at()).out_at(o_lst[i_o]).at().start();}catch(...){}
    }
}

void TTransportS::gmdStop( )
{   
    vector<string> t_lst, o_lst;
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TModule> mod = gmdAt(t_lst[i_t]);
	((TTipTransport &)mod.at()).in_list(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try{((TTipTransport &)mod.at()).in_at(o_lst[i_o]).at().stop();}catch(...){}
	((TTipTransport &)mod.at()).out_list(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try{((TTipTransport &)mod.at()).out_at(o_lst[i_o]).at().stop();}catch(...){}
    }
}

string TTransportS::opt_descr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"======================= The transport subsystem options ===================\n"
	"    --TRMPath=<path>  Set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"mod_path  <path>      set path to modules;\n"
	"GenBD     <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"
	),gmdName().c_str());

    return(buf);
}

void TTransportS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{"TRMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTransportS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    string opt;

    try{ DirPath = gmdXMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    
    try
    {
    	opt = gmdXMLCfgNode()->get_child("id","GenBD")->get_text(); 
	int pos = 0;
        m_bd.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd.tp.size() ) m_bd.tp = Owner().DefBDType;
    if( !m_bd.bd.size() ) m_bd.bd = Owner().DefBDName;
}

void TTransportS::loadBD( )
{ 
    TConfig *c_el;
    string name,type,module;
    
    try
    {
	AutoHD<TTable> tbl = Owner().BD().open(m_bd);
	for( int i_ln = 0; i_ln < tbl.at().nLines(); i_ln++ )
	{
	    TConfig c_el(this);
	    c_el.cfLoadValBD(i_ln,tbl.at());
	    name   = c_el.cfg("NAME").getS();
	    module = c_el.cfg("MODULE").getS();
	    type   = c_el.cfg("TYPE").getSEL();
	
	    try
	    {
		AutoHD<TModule> mod = gmdAt(module);
		if( type == "Input" )
		{
		    try{ ((TTipTransport &)mod.at()).in_add(name);}catch(...){}		    
		    ((TTipTransport &)mod.at()).in_at(name).at().cfLoadValBD(i_ln,tbl.at());
		}
		else if( type == "Output" )
		{
		    try{ ((TTipTransport &)mod.at()).out_add(name);}catch(...){}		    
		    ((TTipTransport &)mod.at()).out_at(name).at().cfLoadValBD(i_ln,tbl.at());
		}
	    }catch(TError err){ m_put_s("SYS",MESS_ERR,err.what()); }	    
	}
	tbl.free();
	Owner().BD().close(m_bd);
    }catch(...){}
}

void TTransportS::saveBD( )
{    
    vector<string> t_lst, o_lst;

    AutoHD<TTable> tbl = Owner().BD().open(m_bd,true);
    tbl.at().clean();
    elUpdateBDAttr( tbl.at() );
    gmdList(t_lst);
    
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TModule> mod = gmdAt(t_lst[i_t]);
	((TTipTransport &)mod.at()).in_list(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    ((TTipTransport &)mod.at()).in_at(o_lst[i_o]).at().cfSaveValBD(-1,tbl.at());
	((TTipTransport &)mod.at()).out_list(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    ((TTipTransport &)mod.at()).out_at(o_lst[i_o]).at().cfSaveValBD(-1,tbl.at());
    }
    tbl.at().save();
    tbl.free();
    Owner().BD().close(m_bd);
}

void TTransportS::gmdDel( const string &name )
{
    /*
    vector<STrS> list;
    in_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) in_del( list[i_m] );

    out_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) out_del( list[i_m] );
    */

    TGRPModule::gmdDel( name );
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TTransportS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    
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

void TTransportS::ctr_din_get_( const string &a_path, XMLNode *opt )
{    
    if( a_path == "/a_bd/t_bd" )   	ctr_opt_setS( opt, m_bd.tp );
    else if( a_path == "/a_bd/bd" )	ctr_opt_setS( opt, m_bd.bd );
    else if( a_path == "/a_bd/tbl" )	ctr_opt_setS( opt, m_bd.tbl );
    else if( a_path == "/a_bd/b_mod" )
    {
	vector<string> list;
	Owner().BD().gmdList(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }
    else if( a_path == "/help/g_help" )	ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}

void TTransportS::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_bd/t_bd" )	m_bd.tp    = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/bd" )	m_bd.bd    = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/tbl" )	m_bd.tbl   = ctr_opt_getS( opt );
    else TGRPModule::ctr_din_set_( a_path, opt );
}

void TTransportS::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_bd/load_bd" )	loadBD();
    else if( a_path == "/a_bd/upd_bd" )	saveBD();
    else TGRPModule::ctr_cmd_go_( a_path, fld, rez );
}


//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::TTipTransport() : m_hd_in(o_name), m_hd_out(o_name)
{

}
    
TTipTransport::~TTipTransport()
{
    vector<string> list;

    m_hd_in.lock();
    in_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	in_del(list[i_ls]);

    m_hd_out.lock();
    out_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	out_del(list[i_ls]);
}

void TTipTransport::in_add( const string &name )
{
    TTransportIn *tr_in = In( name );
    try{ m_hd_in.obj_add( tr_in, &tr_in->name() ); }
    catch(TError err) { delete tr_in; }
}

void TTipTransport::out_add( const string &name )
{
    TTransportOut *tr_out = Out(name);
    try{ m_hd_out.obj_add( tr_out, &tr_out->name() ); }
    catch(TError err) { delete tr_out; }
}

//================== Controll functions ========================
void TTipTransport::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<area id='tr'>"
	" <list id='in' s_com='add,del' tp='br' mode='att'/>"
	" <list id='out' s_com='add,del' tp='br' mode='att'/>"
	"</area>";
    char *dscr="dscr";
    
    TModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->ins_child(0);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Transports"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Input"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Output"));
}

void TTipTransport::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    vector<string> list;
    
    if( a_path == "/tr/in" )
    {
	in_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a ); 	
    }
    else if( a_path == "/tr/out" )
    {
	out_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a ); 	
    }   
    else TModule::ctr_din_get_( a_path, opt );
}

void TTipTransport::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,6) == "/tr/in" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      in_add(t_c->get_text());
		else if(t_c->get_attr("do") == "del") in_del(t_c->get_text());
	    }
	}
    else if( a_path.substr(0,7) == "/tr/out" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      out_add(t_c->get_text());
		else if(t_c->get_attr("do") == "del") out_del(t_c->get_text());
	    }
	}
    else TModule::ctr_din_set_( a_path, opt );
}

AutoHD<TContr> TTipTransport::ctr_at1( const string &a_path )
{
    if( a_path.substr(0,6) == "/tr/in" )       return in_at(ctr_path_l(a_path,2));
    else if( a_path.substr(0,7) == "/tr/out" ) return out_at(ctr_path_l(a_path,2));
    else return TModule::ctr_at1(a_path);
}
//================================================================
//=========== TTransportIn =======================================
//================================================================
const char *TTransportIn::o_name = "TTransportIn";

TTransportIn::TTransportIn( const string &name, TTipTransport *owner ) : 
    m_owner(owner), TConfig((TTransportS *)&(owner->Owner())), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCRIPT").getS()), m_addr(cfg("ADDR").getS()), m_prot(cfg("PROT").getS())
{
    m_name = name;
    cfg("TYPE").setSEL("Input");
    cfg("MODULE").setS(Owner().modName());
}
    
TTransportIn::~TTransportIn()
{
    
}

//================== Controll functions ========================
void TTransportIn::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
	"<oscada_cntr>"
	" <area id='prm'>"
	"  <fld id='name' acs='0664' tp='str'/>"
	"  <fld id='dscr' acs='0664' tp='str'/>"
	"  <fld id='addr' acs='0664' tp='str'/>"
	"  <fld id='prot' acs='0664' tp='str' dest='select' select='/prm/p_mod'/>"
	"  <fld id='r_st' acs='0664' tp='bool'/>"
	"  <list id='p_mod' tp='str' hide='1'/>"
	" </area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Input transport: ")+name());
    XMLNode *n_add = inf->get_child(0);
    n_add->set_attr(dscr,Mess->I18N("Transport"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Name"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Full name"));
    n_add->get_child(2)->set_attr(dscr,Mess->I18N("Address"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Protocol"));
    n_add->get_child(4)->set_attr(dscr,Mess->I18N("Runing"));
}

void TTransportIn::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )     	ctr_opt_setS( opt, m_name );
    else if( a_path == "/prm/dscr" )  ctr_opt_setS( opt, m_lname );
    else if( a_path == "/prm/addr" )  ctr_opt_setS( opt, m_addr );
    else if( a_path == "/prm/prot" )  ctr_opt_setS( opt, m_prot );
    else if( a_path == "/prm/r_st" )  ctr_opt_setB( opt, run_st );
    else if( a_path == "/prm/p_mod" )
    {
	vector<string> list;	
	Owner().Owner().Owner().Protocol().gmdList(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }    
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TTransportIn::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )     	m_name  = ctr_opt_getS( opt );
    else if( a_path == "/prm/dscr" )  m_lname = ctr_opt_getS( opt );
    else if( a_path == "/prm/addr" )  m_addr  = ctr_opt_getS( opt );
    else if( a_path == "/prm/prot" )  m_prot  = ctr_opt_getS( opt );
    else if( a_path == "/prm/r_st" )	{ if( ctr_opt_getB( opt ) ) start(); else stop(); }    
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
const char *TTransportOut::o_name = "TTransportOut";

TTransportOut::TTransportOut( const string &name, TTipTransport *owner ) : 
    m_owner(owner), TConfig((TTransportS *)&(owner->Owner())), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCRIPT").getS()), m_addr(cfg("ADDR").getS()) 
{ 
    m_name = name;
    cfg("TYPE").setSEL("Output");
    cfg("MODULE").setS(Owner().modName());
}

TTransportOut::~TTransportOut()
{
    
}

//================== Controll functions ========================
void TTransportOut::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='prm'>"
	"  <fld id='name' acs='0664' tp='str'/>"
	"  <fld id='dscr' acs='0664' tp='str'/>"
	"  <fld id='addr' acs='0664' tp='str'/>"
	"  <fld id='r_st' acs='0664' tp='bool'/>"
	" </area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Output transport: ")+name());
    XMLNode *n_add = inf->get_child(0);
    n_add->set_attr(dscr,Mess->I18N("Transport"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Name"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Full name"));
    n_add->get_child(2)->set_attr(dscr,Mess->I18N("Address"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Runing"));
}

void TTransportOut::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )  	ctr_opt_setS( opt, m_name );
    else if( a_path == "/prm/dscr" )	ctr_opt_setS( opt, m_lname );
    else if( a_path == "/prm/addr" )	ctr_opt_setS( opt, m_addr );
    else if( a_path == "/prm/r_st" )	ctr_opt_setB( opt, run_st );
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TTransportOut::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )     	m_name  = ctr_opt_getS( opt );
    else if( a_path == "/prm/dscr" )	m_lname = ctr_opt_getS( opt );
    else if( a_path == "/prm/addr" )	m_addr  = ctr_opt_getS( opt );
    else if( a_path == "/prm/r_st" )	{ if( ctr_opt_getB( opt ) ) start(); else stop(); }
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}


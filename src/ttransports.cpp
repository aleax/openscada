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
const char *TTransportS::o_name = "TTransportS";

TTransportS::TTransportS( TKernel *app ) : 
    TGRPModule(app,"Transport"), m_bd_in("","","transp_in.dbf"), m_bd_out("","","transp_out.dbf")    
{
    s_name = "Transports"; 
    
    //Input transport BD structure    
    el_in.fldAdd( new TFld("NAME","Transport name.",T_STRING|F_KEY,"20") );
    el_in.fldAdd( new TFld("DESCRIPT","Transport description.",T_STRING,"50") );
    el_in.fldAdd( new TFld("MODULE","Type transport (module name).",T_STRING,"20") );
    el_in.fldAdd( new TFld("ADDR","Transport address.",T_STRING,"50") );
    el_in.fldAdd( new TFld("PROT","Assign transport protocol.",T_STRING,"20") );
    el_in.fldAdd( new TFld("START","Start archive",T_BOOL,"1") );
    
    //Output transport BD structure
    el_out.fldAdd( new TFld("NAME","Transport name.",T_STRING|F_KEY,"20") );
    el_out.fldAdd( new TFld("DESCRIPT","Transport description.",T_STRING,"50") );
    el_out.fldAdd( new TFld("MODULE","Type transport (module name).",T_STRING,"20") );
    el_out.fldAdd( new TFld("ADDR","Transport address.",T_STRING,"50") );
    el_out.fldAdd( new TFld("START","Start archive",T_BOOL,"1") );
}

TTransportS::~TTransportS(  )
{

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
	AutoHD<TTipTransport> mod = gmdAt(t_lst[i_t]);
	o_lst.clear();
	mod.at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportIn> in = mod.at().inAt(o_lst[i_o]);
		if( !in.at().startStat() && in.at().toStart() ) 
		    in.at().start();
	     }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }
	     
	o_lst.clear();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if( !out.at().startStat() && out.at().toStart() ) 
		    out.at().start();
	     }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }
    }
}

void TTransportS::gmdStop( )
{   
    vector<string> t_lst, o_lst;
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipTransport> mod = gmdAt(t_lst[i_t]);
	o_lst.clear();
	mod.at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportIn> in = mod.at().inAt(o_lst[i_o]);
		if( in.at().startStat() ) in.at().stop();
	     }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }
	o_lst.clear();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {	
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if( out.at().startStat() ) out.at().stop();
	     }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }
    }
}

string TTransportS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"======================= The transport subsystem options ===================\n"
	"    --TRMPath=<path>  Set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"mod_path  <path>      set path to modules;\n"
	"InBD      <fullname>  Input transports bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	"OutBD     <fullname>  Output transports bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTransportS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    string opt;

    try{ DirPath = gmdCfgNode()->childGet("id","mod_path")->text(); }
    catch(...) {  }
    
    try
    {
    	opt = gmdCfgNode()->childGet("id","InBD")->text(); 
	int pos = 0;
        m_bd_in.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_in.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_in.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd_in.tp.size() ) m_bd_in.tp = owner().DefBDType;
    if( !m_bd_in.bd.size() ) m_bd_in.bd = owner().DefBDName;
    
    try
    {
    	opt = gmdCfgNode()->childGet("id","OutBD")->text(); 
	int pos = 0;
        m_bd_out.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_out.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_out.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd_out.tp.size() ) m_bd_out.tp = owner().DefBDType;
    if( !m_bd_out.bd.size() ) m_bd_out.bd = owner().DefBDName;
}

void TTransportS::loadBD( )
{ 
    string name,type;
    //vector<string> list_el;

    //Load input transports
    try
    {
	TConfig c_el(&el_in);
	AutoHD<TTable> tbl = owner().BD().open(m_bd_in);	
	
	int fld_cnt = 0;
	while( tbl.at().fieldSeek(fld_cnt++,c_el) )
	{
	    name = c_el.cfg("NAME").getS();
	    type = c_el.cfg("MODULE").getS();
	    
	    AutoHD<TTipTransport> mod = gmdAt(type);	    
	    if( !mod.at().inAvoid(name) )
	    {
		mod.at().inAdd(name);		
		((TConfig &)mod.at().inAt(name).at()) = c_el;
	    }
	    else mod.at().inAt(name).at().load();
	}
	tbl.free();
	owner().BD().close(m_bd_in);	
    }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }            
    
    //Load output transports
    //list_el.clear();
    try
    {
	TConfig c_el(&el_out);
	AutoHD<TTable> tbl = owner().BD().open(m_bd_out);	
	
	int fld_cnt = 0;
	while( tbl.at().fieldSeek(fld_cnt++,c_el) )
	{
	    name = c_el.cfg("NAME").getS();
	    type = c_el.cfg("MODULE").getS();
	    
	    AutoHD<TTipTransport> mod = gmdAt(type);	    
	    if( !mod.at().outAvoid(name) )
	    { 
		mod.at().outAdd(name);
		((TConfig &)mod.at().outAt(name).at()) = c_el;
	    }
	    else mod.at().outAt(name).at().load();
	}
	tbl.free();
	owner().BD().close(m_bd_out);	
    }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }            
}

void TTransportS::saveBD( )
{    
    vector<string> t_lst, o_lst;

    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipTransport> mod = gmdAt(t_lst[i_t]);
	mod.at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().inAt(o_lst[i_o]).at().save();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().outAt(o_lst[i_o]).at().save();
    }    
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TTransportS::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctrStat_( inf );
    
    char *i_cntr = 
    	"<area id='bd' acs='0440'>"
	 "<fld id='i_tbd' acs='0660' tp='str' dest='select' select='/bd/b_mod'/>"
	 "<fld id='i_bd' acs='0660' tp='str'/>"
	 "<fld id='i_tbl' acs='0660' tp='str'/>"
	 "<fld id='o_tbd' acs='0660' tp='str' dest='select' select='/bd/b_mod'/>"
	 "<fld id='o_bd' acs='0660' tp='str'/>"
	 "<fld id='o_tbl' acs='0660' tp='str'/>"
	 "<comm id='load_bd'/>"
	 "<comm id='upd_bd'/>"
	"</area>";    
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("Subsystem"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("Input transports BD (module:bd:table)"));
    n_add->childGet(3)->attr(dscr,Mess->I18N("Output transports BD (module:bd:table)"));
    n_add->childGet(6)->attr(dscr,Mess->I18N("Load from BD"));
    n_add->childGet(7)->attr(dscr,Mess->I18N("Save to BD"));

    //Insert to Help
    char *i_help = "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    n_add = inf->childGet("id","help")->childAdd();    
    n_add->load(i_help);
    n_add->attr(dscr,Mess->I18N("Options help"));
}

void TTransportS::ctrDinGet_( const string &a_path, XMLNode *opt )
{    
    if( a_path == "/bd/i_tbd" )   	ctrSetS( opt, m_bd_in.tp );
    else if( a_path == "/bd/i_bd" )	ctrSetS( opt, m_bd_in.bd );
    else if( a_path == "/bd/i_tbl" )	ctrSetS( opt, m_bd_in.tbl );
    else if( a_path == "/bd/o_tbd" )   	ctrSetS( opt, m_bd_out.tp );
    else if( a_path == "/bd/o_bd" )	ctrSetS( opt, m_bd_out.bd );
    else if( a_path == "/bd/o_tbl" )	ctrSetS( opt, m_bd_out.tbl );
    else if( a_path == "/bd/b_mod" )
    {
	vector<string> list;
	owner().BD().gmdList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );
    }
    else if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
    else TGRPModule::ctrDinGet_( a_path, opt );
}

void TTransportS::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/bd/i_tbd" )		m_bd_in.tp	= ctrGetS( opt );
    else if( a_path == "/bd/i_bd" )	m_bd_in.bd    	= ctrGetS( opt );
    else if( a_path == "/bd/i_tbl" )	m_bd_in.tbl   	= ctrGetS( opt );
    else if( a_path == "/bd/o_tbd" )	m_bd_out.tp    	= ctrGetS( opt );
    else if( a_path == "/bd/o_bd" )	m_bd_out.bd    	= ctrGetS( opt );
    else if( a_path == "/bd/o_tbl" )	m_bd_out.tbl   	= ctrGetS( opt );
    else if( a_path == "/bd/load_bd" ) 	loadBD();
    else if( a_path == "/bd/upd_bd" )   saveBD();
    else TGRPModule::ctrDinSet_( a_path, opt );
}

//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::TTipTransport()
{
    m_in = grpAdd();
    m_out = grpAdd();
}
    
TTipTransport::~TTipTransport()
{  
    delAll();
}

void TTipTransport::inAdd( const string &name )
{ 
    if( chldAvoid(m_in,name) ) return;
    chldAdd(m_in,In(name)); 
}

void TTipTransport::outAdd( const string &name )
{ 
    if( chldAvoid(m_out,name) )	return;
    chldAdd(m_out,Out(name)); 
}

//================== Controll functions ========================
void TTipTransport::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<area id='tr'>"
	 "<list id='in' s_com='add,del' tp='br' mode='att' br_pref='_in_'/>"
	 "<list id='out' s_com='add,del' tp='br' mode='att' br_pref='_out_'/>"
	"</area>";
    char *dscr="dscr";
    
    TModule::ctrStat_( inf );
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("Transports"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("Input"));
    n_add->childGet(1)->attr(dscr,Mess->I18N("Output"));
}

void TTipTransport::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    vector<string> list;
    
    if( a_path == "/tr/in" )
    {
	inList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] ); 	
    }
    else if( a_path == "/tr/out" )
    {
	outList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] ); 	
    }   
    else TModule::ctrDinGet_( a_path, opt );
}

void TTipTransport::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,6) == "/tr/in" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")    	inAdd(t_c->text());
		else if(t_c->attr("do") == "del")	chldDel(m_in,t_c->text(),-1,1);
	    }
	}
    else if( a_path.substr(0,7) == "/tr/out" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")     	outAdd(t_c->text());
		else if(t_c->attr("do") == "del")	chldDel(m_out,t_c->text(),-1,1);
	    }
	}
    else TModule::ctrDinSet_( a_path, opt );
}

AutoHD<TCntrNode> TTipTransport::ctrAt1( const string &br )
{
    if(br.substr(0,4)=="_in_")		return inAt(br.substr(4));
    else if(br.substr(0,5)=="_out_")	return outAt(br.substr(5));
    //if( a_path.substr(0,6) == "/tr/in" )       return inAt(pathLev(a_path,2));
    //else if( a_path.substr(0,7) == "/tr/out" ) return outAt(pathLev(a_path,2));
    else return TModule::ctrAt1(br);
}
//================================================================
//=========== TTransportIn =======================================
//================================================================
const char *TTransportIn::o_name = "TTransportIn";

TTransportIn::TTransportIn( const string &name, TTipTransport *n_owner ) : 
    m_owner(n_owner), TConfig(&((TTransportS &)n_owner->owner()).inEl()), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCRIPT").getS()), m_addr(cfg("ADDR").getS()), 
    m_prot(cfg("PROT").getS()), m_start(cfg("START").getB())
{
    m_name = name;
    cfg("MODULE").setS(owner().modName());
}
    
TTransportIn::~TTransportIn()
{
    
}

void TTransportIn::postDisable(int flag)
{
    try
    {
        if( flag )
        {
            TBDS &bds = owner().owner().owner().BD();
	    bds.open(((TTransportS &)owner().owner()).inBD()).at().fieldDel(*this);
	    bds.close(((TTransportS &)owner()).inBD());
        }
    }catch(TError err)
    { owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}									    

void TTransportIn::load()
{
    TBDS &bds = owner().owner().owner().BD();
    bds.open( ((TTransportS &)owner().owner()).inBD() ).at().fieldGet(*this);
    bds.close( ((TTransportS &)owner().owner()).inBD() );
}

void TTransportIn::save( )
{
    TBDS &bds = owner().owner().owner().BD();
    bds.open( ((TTransportS &)owner().owner()).inBD(), true ).at().fieldSet(*this);
    bds.close( ((TTransportS &)owner().owner()).inBD() );
}

void TTransportIn::preEnable()
{ 
    try{ load(); }catch(...){ }
}

//================== Controll functions ========================
void TTransportIn::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
	"<oscada_cntr>"
	 "<area id='prm'>"
	  "<area id='st'>"
	   "<fld id='st' acs='0664' tp='bool'/>"
	  "</area>"
	  "<area id='cfg'>"
	   "<fld id='name' acs='0664' tp='str'/>"
	   "<fld id='dscr' acs='0664' tp='str'/>"
	   "<fld id='addr' acs='0664' tp='str'/>"
	   "<fld id='prot' acs='0664' tp='str' dest='select' select='/prm/cfg/p_mod'/>"
	   "<fld id='start' acs='0664' tp='bool'/>"
	   "<comm id='load' acs='0550'/>"
	   "<comm id='save' acs='0550'/>"
	  "</area>"
	 "</area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load( i_cntr );
    inf->text(Mess->I18N("Input transport: ")+name());
    XMLNode *n_add = inf->childGet(0);
    n_add->attr(dscr,Mess->I18N("Transport"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("State"));
    n_add->childGet(0)->childGet(0)->attr(dscr,Mess->I18N("Runing"));    
    n_add = n_add->childGet(1);
    n_add->attr(dscr,Mess->I18N("Config"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("Name"));
    n_add->childGet(1)->attr(dscr,Mess->I18N("Full name"));
    n_add->childGet(2)->attr(dscr,Mess->I18N("Address"));
    n_add->childGet(3)->attr(dscr,Mess->I18N("Protocol"));
    n_add->childGet(4)->attr(dscr,Mess->I18N("To start"));
    n_add->childGet(5)->attr(dscr,Mess->I18N("Load from BD"));
    n_add->childGet(6)->attr(dscr,Mess->I18N("Save to BD"));
}

void TTransportIn::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )  		ctrSetB( opt, run_st );
    else if( a_path == "/prm/cfg/name" )	ctrSetS( opt, m_name );
    else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, m_lname );
    else if( a_path == "/prm/cfg/addr" )	ctrSetS( opt, m_addr );
    else if( a_path == "/prm/cfg/prot" )	ctrSetS( opt, m_prot );
    else if( a_path == "/prm/cfg/start" )	ctrSetB( opt, m_start );
    else if( a_path == "/prm/cfg/p_mod" )
    {
	vector<string> list;	
	owner().owner().owner().Protocol().gmdList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );
    }    
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TTransportIn::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )	{ if( ctrGetB( opt ) ) start(); else stop(); }    
    else if( a_path == "/prm/cfg/name" )   	m_name  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/dscr" )  	m_lname = ctrGetS( opt );
    else if( a_path == "/prm/cfg/addr" )  	m_addr  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/prot" )  	m_prot  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
    else if( a_path == "/prm/cfg/load" )	load();
    else if( a_path == "/prm/cfg/save" ) 	save();
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
const char *TTransportOut::o_name = "TTransportOut";

TTransportOut::TTransportOut( const string &name, TTipTransport *n_owner ) : 
    m_owner(n_owner), TConfig(&((TTransportS &)n_owner->owner()).outEl()), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCRIPT").getS()), m_addr(cfg("ADDR").getS()), 
    m_start(cfg("START").getB())
{ 
    m_name = name;
    cfg("MODULE").setS(owner().modName());
}

TTransportOut::~TTransportOut()
{
    
}

void TTransportOut::postDisable(int flag)
{
    try
    {
        if( flag )
        {
    	    TBDS &bds = owner().owner().owner().BD();
            bds.open(((TTransportS &)owner().owner()).outBD()).at().fieldDel(*this);
    	    bds.close(((TTransportS &)owner()).outBD());
        }
    }catch(TError err)
    { owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}									    
	
void TTransportOut::load()
{
    TBDS &bds = owner().owner().owner().BD();
    bds.open( ((TTransportS &)owner().owner()).outBD() ).at().fieldGet(*this);
    bds.close( ((TTransportS &)owner().owner()).outBD() );
}

void TTransportOut::save()
{
    TBDS &bds = owner().owner().owner().BD();
    bds.open( ((TTransportS &)owner().owner()).outBD(), true ).at().fieldSet(*this);
    bds.close( ((TTransportS &)owner().owner()).outBD() );
}

void TTransportOut::preEnable()
{ 
    try{ load(); }catch(...){ }
}

//================== Controll functions ========================
void TTransportOut::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='prm'>"
	  "<area id='st'>"
	   "<fld id='st' acs='0664' tp='bool'/>"
	  "</area>"
	  "<area id='cfg'>"
	   "<fld id='name' acs='0664' tp='str'/>"
	   "<fld id='dscr' acs='0664' tp='str'/>"
	   "<fld id='addr' acs='0664' tp='str'/>"
	   "<fld id='start' acs='0664' tp='bool'/>"
	   "<comm id='load' acs='0550'/>"
	   "<comm id='save' acs='0550'/>"    
	  "</area>"
	 "</area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load( i_cntr );
    inf->text(Mess->I18N("Output transport: ")+name());
    XMLNode *n_add = inf->childGet(0);
    n_add->attr(dscr,Mess->I18N("Transport"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("State"));
    n_add->childGet(0)->childGet(0)->attr(dscr,Mess->I18N("Runing"));    
    n_add = n_add->childGet(1);
    n_add->attr(dscr,Mess->I18N("Config"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("Name"));
    n_add->childGet(1)->attr(dscr,Mess->I18N("Full name"));
    n_add->childGet(2)->attr(dscr,Mess->I18N("Address"));
    n_add->childGet(3)->attr(dscr,Mess->I18N("To start"));
    n_add->childGet(4)->attr(dscr,Mess->I18N("Load from BD"));
    n_add->childGet(5)->attr(dscr,Mess->I18N("Save to BD"));
}

void TTransportOut::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )		ctrSetB( opt, run_st );
    else if( a_path == "/prm/cfg/name" ) 	ctrSetS( opt, m_name );
    else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, m_lname );
    else if( a_path == "/prm/cfg/addr" )	ctrSetS( opt, m_addr );
    else if( a_path == "/prm/cfg/start" )	ctrSetB( opt, m_start );
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TTransportOut::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )		{ if( ctrGetB( opt ) ) start(); else stop(); }
    if( a_path == "/prm/cfg/name" )     	m_name  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/dscr" )	m_lname = ctrGetS( opt );
    else if( a_path == "/prm/cfg/addr" )	m_addr  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
    else if( a_path == "/prm/cfg/load" )  	load();
    else if( a_path == "/prm/cfg/save" ) 	save();	
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

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
#include "tmodule.h"
#include "tprotocols.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================
TTransportS::TTransportS( TSYS *app ) : 
    TSubSYS(app,"Transport","Transports",true), m_bd_in("","","transp_in"), m_bd_out("","","transp_out")
{
    //Input transport BD structure
    el_in.fldAdd( new TFld("NAME","Transport name.",TFld::String,FLD_KEY,"20") );
    el_in.fldAdd( new TFld("DESCRIPT","Transport description.",TFld::String,0,"50") );
    el_in.fldAdd( new TFld("MODULE","Type transport (module name).",TFld::String,0,"20") );
    el_in.fldAdd( new TFld("ADDR","Transport address.",TFld::String,0,"50") );
    el_in.fldAdd( new TFld("PROT","Assign transport protocol.",TFld::String,0,"20") );
    el_in.fldAdd( new TFld("START","Start archive",TFld::Bool,0,"1") );
    
    //Output transport BD structure
    el_out.fldAdd( new TFld("NAME","Transport name.",TFld::String,FLD_KEY,"20") );
    el_out.fldAdd( new TFld("DESCRIPT","Transport description.",TFld::String,0,"50") );
    el_out.fldAdd( new TFld("MODULE","Type transport (module name).",TFld::String,0,"20") );
    el_out.fldAdd( new TFld("ADDR","Transport address.",TFld::String,0,"50") );
    el_out.fldAdd( new TFld("START","Start archive",TFld::Bool,0,"1") );
}

TTransportS::~TTransportS(  )
{

}

void TTransportS::subLoad( )
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
    
    //========== Load parameters from config file =============
    string opt;
    try
    {
    	opt = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","InBD")->text(); 
	m_bd_in.tp  = TSYS::strSepParse(opt,0,':');
	m_bd_in.bd  = TSYS::strSepParse(opt,1,':');
	m_bd_in.tbl = TSYS::strSepParse(opt,2,':');
    }
    catch(...) {  }
    
    try
    {
    	opt = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","OutBD")->text(); 
	m_bd_out.tp  = TSYS::strSepParse(opt,0,':');
        m_bd_out.bd  = TSYS::strSepParse(opt,1,':');
        m_bd_out.tbl = TSYS::strSepParse(opt,2,':');			
    }
    catch(...) {  }
    
    //============== Load DB =======================
    int fld_cnt;
    string name,type;
    //Load input transports
    try
    {
	TConfig c_el(&el_in);	
	AutoHD<TTable> tbl = owner().db().at().open(inBD());
	
	fld_cnt = 0;
	while( owner().db().at().dataSeek(tbl,nodePath()+"In/",fld_cnt++,c_el) )
	{
	    name = c_el.cfg("NAME").getS();
	    type = c_el.cfg("MODULE").getS();
	    
	    AutoHD<TTipTransport> mod = modAt(type);	    
	    if( !mod.at().inPresent(name) )
	    {
		mod.at().inAdd(name);		
		((TConfig &)mod.at().inAt(name).at()) = c_el;
	    }
	    else mod.at().inAt(name).at().load();
	}
	if(!tbl.freeStat())
	{
	    tbl.free();
	    owner().db().at().close(inBD());	
	}
    }catch( TError err ){ Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }            
    
    //Load output transports
    try
    {
	TConfig c_el(&el_out);
	AutoHD<TTable> tbl = owner().db().at().open(outBD());	
	
	fld_cnt = 0;
	while( owner().db().at().dataSeek(tbl,nodePath()+"Out/",fld_cnt++,c_el) )
	{
	    name = c_el.cfg("NAME").getS();
	    type = c_el.cfg("MODULE").getS();
	    
	    AutoHD<TTipTransport> mod = modAt(type);	    
	    if( !mod.at().outPresent(name) )
	    { 
		mod.at().outAdd(name);
		((TConfig &)mod.at().outAt(name).at()) = c_el;
	    }
	    else mod.at().outAt(name).at().load();
	}
	if(!tbl.freeStat())
	{	
	    tbl.free();
	    owner().db().at().close(outBD());
	}	
    }catch( TError err ){ Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }            
    
    //=================== Load modules =======================
    TSubSYS::subLoad( );
}

void TTransportS::subSave( )
{    
    vector<string> t_lst, o_lst;

    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipTransport> mod = modAt(t_lst[i_t]);
	mod.at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().inAt(o_lst[i_o]).at().save();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().outAt(o_lst[i_o]).at().save();
    }    
}

TBDS::SName TTransportS::inBD() 
{ 
    return owner().nameDBPrep(m_bd_in);
}

TBDS::SName TTransportS::outBD()
{ 
    return owner().nameDBPrep(m_bd_out); 
}	

void TTransportS::subStart( )
{    
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipTransport> mod = modAt(t_lst[i_t]);
	o_lst.clear();
	mod.at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportIn> in = mod.at().inAt(o_lst[i_o]);
		if( !in.at().startStat() && in.at().toStart() ) 
		    in.at().start();
	    }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	     
	o_lst.clear();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if( !out.at().startStat() && out.at().toStart() ) 
		    out.at().start();
	     }catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    }
}

void TTransportS::subStop( )
{   
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipTransport> mod = modAt(t_lst[i_t]);
	o_lst.clear();
	mod.at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportIn> in = mod.at().inAt(o_lst[i_o]);
		if( in.at().startStat() ) in.at().stop();
	     }catch( TError err ){ Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
	o_lst.clear();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {	
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if( out.at().startStat() ) out.at().stop();
	     }catch( TError err ){ Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
    }
}

string TTransportS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"======================= The transport subsystem options ===================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"InBD      <fullname>  Input transports bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	"OutBD     <fullname>  Output transports bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"
	),nodePath().c_str());

    return(buf);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TTransportS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	TSubSYS::cntrCmd_( a_path, opt, cmd );	//Call parent
	
	ctrInsNode("area",0,opt,a_path.c_str(),"/bd",Mess->I18N("Subsystem"),0440);
	if( owner().genDB( ) )
	{
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/i_tbl",Mess->I18N("Input transports table"),0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/o_tbl",Mess->I18N("Output transports table"),0660,0,0,"str");
	}
	else
	{
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/i_tbd",Mess->I18N("Input transports BD (module:bd:table)"),0660,0,0,"str")->
		attr_("dest","select")->attr_("select","/bd/b_mod");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/i_bd","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/i_tbl","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/o_tbd",Mess->I18N("Output transports BD (module:bd:table)"),0660,0,0,"str")->
		attr_("dest","select")->attr_("select","/bd/b_mod");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/o_bd","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/bd/o_tbl","",0660,0,0,"str");
	}
	ctrMkNode("comm",opt,a_path.c_str(),"/bd/load_bd",Mess->I18N("Load"));
	ctrMkNode("comm",opt,a_path.c_str(),"/bd/upd_bd",Mess->I18N("Save"));
	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }    
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/bd/i_tbd" )   		ctrSetS( opt, m_bd_in.tp );
	else if( a_path == "/bd/i_bd" )		ctrSetS( opt, m_bd_in.bd );
	else if( a_path == "/bd/i_tbl" )	ctrSetS( opt, m_bd_in.tbl );
	else if( a_path == "/bd/o_tbd" )   	ctrSetS( opt, m_bd_out.tp );
	else if( a_path == "/bd/o_bd" )		ctrSetS( opt, m_bd_out.bd );
	else if( a_path == "/bd/o_tbl" )	ctrSetS( opt, m_bd_out.tbl );
	else if( a_path == "/bd/b_mod" )
	{
	    vector<string> list;
	    owner().db().at().modList(list);
	    opt->childClean();
	    ctrSetS( opt, "" );
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
	else if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bd/i_tbd" )		m_bd_in.tp	= ctrGetS( opt );
	else if( a_path == "/bd/i_bd" )		m_bd_in.bd    	= ctrGetS( opt );
	else if( a_path == "/bd/i_tbl" )	m_bd_in.tbl   	= ctrGetS( opt );
	else if( a_path == "/bd/o_tbd" )	m_bd_out.tp    	= ctrGetS( opt );
	else if( a_path == "/bd/o_bd" )		m_bd_out.bd    	= ctrGetS( opt );
	else if( a_path == "/bd/o_tbl" )	m_bd_out.tbl   	= ctrGetS( opt );
	else if( a_path == "/bd/load_bd" ) 	subLoad();
	else if( a_path == "/bd/upd_bd" )   	subSave();
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
}

//================================================================
//=========== TTipTransport ======================================
//================================================================
TTipTransport::TTipTransport()
{
    m_in = grpAdd();
    m_out = grpAdd();
}
    
TTipTransport::~TTipTransport()
{  
    nodeDelAll();
}

void TTipTransport::inAdd( const string &name )
{ 
    if( chldPresent(m_in,name) ) return;
    chldAdd(m_in,In(name)); 
}

void TTipTransport::outAdd( const string &name )
{ 
    if( chldPresent(m_out,name) )	return;
    chldAdd(m_out,Out(name)); 
}

//================== Controll functions ========================
void TTipTransport::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	TModule::cntrCmd_( a_path, opt, cmd );	//Call parent
	
	ctrInsNode("area",0,opt,a_path.c_str(),"/tr",Mess->I18N("Transports"));
	ctrMkNode("list",opt,a_path.c_str(),"/tr/in",Mess->I18N("Input"),0664,0,0,"br")->
	    attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_in_");
	ctrMkNode("list",opt,a_path.c_str(),"/tr/out",Mess->I18N("Output"),0664,0,0,"br")->
	    attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_out_");
    }    
    else if( cmd==TCntrNode::Get )
    {
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
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/tr/in" )
	{
	    if( opt->name() == "add" )		inAdd(opt->text());
	    else if( opt->name() == "del" )	chldDel(m_in,opt->text(),-1,1);
	}
	else if( a_path == "/tr/out" )
	{
	    if( opt->name() == "add" )		outAdd(opt->text());
	    else if( opt->name() == "del" )	chldDel(m_out,opt->text(),-1,1);
	}
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
}

AutoHD<TCntrNode> TTipTransport::ctrAt( const string &br )
{
    if(br.substr(0,4)=="_in_")		return inAt(br.substr(4));
    else if(br.substr(0,5)=="_out_")	return outAt(br.substr(5));
    else return TModule::ctrAt(br);
}
//================================================================
//=========== TTransportIn =======================================
//================================================================
TTransportIn::TTransportIn( const string &name, TTipTransport *n_owner ) : 
    TCntrNode(n_owner), TConfig(&((TTransportS &)n_owner->owner()).inEl()), run_st(false),
    m_name(cfg("NAME").getSd()), m_lname(cfg("DESCRIPT").getSd()), m_addr(cfg("ADDR").getSd()), 
    m_prot(cfg("PROT").getSd()), m_start(cfg("START").getBd())
{
    m_name = name;
    cfg("MODULE").setS(owner().modId());
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
            AutoHD<TBDS> bds = owner().owner().owner().db();
	    bds.at().open(((TTransportS &)owner().owner()).inBD()).at().fieldDel(*this);
	    bds.at().close(((TTransportS &)owner().owner()).inBD());
        }
    }catch(TError err)
    { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
}									    

void TTransportIn::load()
{
    AutoHD<TBDS> bds = owner().owner().owner().db();
    bds.at().open( ((TTransportS &)owner().owner()).inBD() ).at().fieldGet(*this);
    bds.at().close( ((TTransportS &)owner().owner()).inBD() );
}

void TTransportIn::save( )
{
    AutoHD<TBDS> bds = owner().owner().owner().db();
    bds.at().open( ((TTransportS &)owner().owner()).inBD(), true ).at().fieldSet(*this);
    bds.at().close( ((TTransportS &)owner().owner()).inBD() );
}

void TTransportIn::preEnable()
{ 
    try{ load(); }catch(...){ }
}

//================== Controll functions ========================
void TTransportIn::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",(Mess->I18N("Input transport: ")+name()).c_str());
	ctrMkNode("area",opt,a_path.c_str(),"/prm",Mess->I18N("Transport"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/name",Mess->I18N("Name"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/dscr",Mess->I18N("Full name"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/addr",Mess->I18N("Address"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/prot",Mess->I18N("Protocol"),0664,0,0,"str")->
	    attr_("dest","select")->attr_("select","/prm/cfg/p_mod");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/start",Mess->I18N("To start"),0664,0,0,"bool");
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
    }    
    else if( cmd==TCntrNode::Get )
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
	    owner().owner().owner().protocol().at().modList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}    
	else throw TError(name().c_str(),"Branch <%s> error",a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/st" )		(ctrGetB( opt ))?start():stop();
	else if( a_path == "/prm/cfg/name" )   	m_name  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/dscr" )  	m_lname = ctrGetS( opt );
	else if( a_path == "/prm/cfg/addr" )  	m_addr  = ctrGetS( opt );
	    else if( a_path == "/prm/cfg/prot" )m_prot  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
	else if( a_path == "/prm/cfg/load" )	load();
	else if( a_path == "/prm/cfg/save" ) 	save();
	else throw TError(name().c_str(),"Branch <%s> error",a_path.c_str());
    }
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
TTransportOut::TTransportOut( const string &name, TTipTransport *n_owner ) : 
    TCntrNode(n_owner), TConfig(&((TTransportS &)n_owner->owner()).outEl()), run_st(false),
    m_name(cfg("NAME").getSd()), m_lname(cfg("DESCRIPT").getSd()), m_addr(cfg("ADDR").getSd()), 
    m_start(cfg("START").getBd())
{ 
    m_name = name;
    cfg("MODULE").setS(owner().modId());
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
    	    AutoHD<TBDS> bds = owner().owner().owner().db();
	    bds.at().open(((TTransportS &)owner().owner()).outBD()).at().fieldDel(*this);
    	    bds.at().close(((TTransportS &)owner().owner()).outBD());
        }
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}									    
	
void TTransportOut::load()
{
    AutoHD<TBDS> bds = owner().owner().owner().db();
    bds.at().open( ((TTransportS &)owner().owner()).outBD() ).at().fieldGet(*this);
    bds.at().close( ((TTransportS &)owner().owner()).outBD() );
}

void TTransportOut::save()
{
    AutoHD<TBDS> bds = owner().owner().owner().db();
    bds.at().open( ((TTransportS &)owner().owner()).outBD(), true ).at().fieldSet(*this);
    bds.at().close( ((TTransportS &)owner().owner()).outBD() );
}

void TTransportOut::preEnable()
{ 
    try{ load(); }catch(...){ }
}

//================== Controll functions ========================
void TTransportOut::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",(Mess->I18N("Output transport: ")+name()).c_str());
	ctrMkNode("area",opt,a_path.c_str(),"/prm",Mess->I18N("Transport"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/name",Mess->I18N("Name"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/dscr",Mess->I18N("Full name"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/addr",Mess->I18N("Address"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/cfg/start",Mess->I18N("To start"),0664,0,0,"bool");
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
    }    
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/st" )		ctrSetB( opt, run_st );
	else if( a_path == "/prm/cfg/name" ) 	ctrSetS( opt, m_name );
	else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, m_lname );
	else if( a_path == "/prm/cfg/addr" )	ctrSetS( opt, m_addr );
	else if( a_path == "/prm/cfg/start" )	ctrSetB( opt, m_start );
	else throw TError(name().c_str(),"Branch <%s> error",a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/st" )		(ctrGetB( opt ))?start():stop();
	else if( a_path == "/prm/cfg/name" )   	m_name  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/dscr" )	m_lname = ctrGetS( opt );
	else if( a_path == "/prm/cfg/addr" )	m_addr  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
	else if( a_path == "/prm/cfg/load" )  	load();
	else if( a_path == "/prm/cfg/save" ) 	save();	
	else throw TError(name().c_str(),"Branch <%s> error",a_path.c_str());
    }
}


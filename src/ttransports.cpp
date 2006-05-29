
//OpenSCADA system file: ttransports.cpp
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
#include "tmodule.h"
#include "tprotocols.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================
TTransportS::TTransportS( ) : TSubSYS("Transport","Transports",true)
{
    //Input transport BD structure
    el_in.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_in.fldAdd( new TFld("MODULE",Mess->I18N("Transport type"),TFld::String,FLD_KEY,"20") );
    el_in.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,FLD_NOFLG,"50") );
    el_in.fldAdd( new TFld("DESCRIPT",Mess->I18N("Description"),TFld::String,FLD_NOFLG,"500") );
    el_in.fldAdd( new TFld("ADDR",Mess->I18N("Address"),TFld::String,FLD_NOFLG,"50") );
    el_in.fldAdd( new TFld("PROT",Mess->I18N("Transport protocol"),TFld::String,FLD_NOFLG,"20") );
    el_in.fldAdd( new TFld("START",Mess->I18N("To start"),TFld::Bool,FLD_NOFLG,"1") );
    
    //Output transport BD structure
    el_out.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_out.fldAdd( new TFld("MODULE",Mess->I18N("Transport type"),TFld::String,FLD_KEY,"20") );
    el_out.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,FLD_NOFLG,"50") );
    el_out.fldAdd( new TFld("DESCRIPT",Mess->I18N("Description"),TFld::String,FLD_NOFLG,"500") );
    el_out.fldAdd( new TFld("ADDR",Mess->I18N("Address"),TFld::String,FLD_NOFLG,"50") );
    el_out.fldAdd( new TFld("START",Mess->I18N("To start"),TFld::Bool,FLD_NOFLG,"1") );
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
    
    //============== Load DB =======================
    string id,type;
    //Search and create new input transports
    try
    {
	TConfig c_el(&el_in);
	vector<string> tdb_ls, db_ls;
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];		
		int fld_cnt=0;
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_in",nodePath()+"In/",fld_cnt++,c_el) )
		{
		    id   = c_el.cfg("ID").getS();
		    type = c_el.cfg("MODULE").getS();	    
		    if( !at(type).at().inPresent(id) ) 
			at(type).at().inAdd(id,(wbd==SYS->workDB())?"*.*":wbd);	    
		    c_el.cfg("ID").setS("");
		    c_el.cfg("MODULE").setS("");
		}
	    }
	}
	
    }catch( TError err ){ Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }            
    
    //Search and create new output transports
    try
    {
	TConfig c_el(&el_out);
	vector<string> tdb_ls, db_ls;

        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		int fld_cnt=0;	
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_out",nodePath()+"Out/",fld_cnt++,c_el) )
		{
		    id = c_el.cfg("ID").getS();
		    type = c_el.cfg("MODULE").getS();	    
		    if( !at(type).at().outPresent(id) ) 
			at(type).at().outAdd(id,(wbd==SYS->workDB())?"*.*":wbd);	    
		    c_el.cfg("ID").setS("");
		    c_el.cfg("MODULE").setS("");
		}
	    }
	}
    }catch( TError err ){ Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }            
    
    //Load present transports
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
        at(t_lst[i_t]).at().inList(o_lst);
        for( int i_o = 0; i_o < o_lst.size(); i_o++ )
            at(t_lst[i_t]).at().inAt(o_lst[i_o]).at().load();
        at(t_lst[i_t]).at().outList(o_lst);
        for( int i_o = 0; i_o < o_lst.size(); i_o++ )
    	    at(t_lst[i_t]).at().outAt(o_lst[i_o]).at().load();
    }
    
    //=================== Load modules =======================
    TSubSYS::subLoad( );
}

void TTransportS::subSave( )
{    
    vector<string> t_lst, o_lst;
    
    //========== Save parameters =============

    //========== Save transports =============
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	at(t_lst[i_t]).at().inList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    at(t_lst[i_t]).at().inAt(o_lst[i_o]).at().save();
	at(t_lst[i_t]).at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    at(t_lst[i_t]).at().outAt(o_lst[i_o]).at().save();
    }    
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
    	"======================= Subsystem \"Transports\" options ==================\n"
	"------------ Parameters of section <%s> in config file -----------\n\n"
	),nodePath().c_str());

    return(buf);
}

void TTransportS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	TSubSYS::cntrCmd_( a_path, opt, cmd );	//Call parent
	
	ctrMkNode("area",opt,0,a_path.c_str(),"/bd",Mess->I18N("Subsystem"),0440);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/bd/load_bd",Mess->I18N("Load"));
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/bd/upd_bd",Mess->I18N("Save"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,3,"tp","str","cols","90","rows","5");
    }    
    else if( cmd==TCntrNode::Get )
    {	    
	if( a_path == "/help/g_help" )		ctrSetS( opt, optDescr() );       
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bd/load_bd" ) 		subLoad();
	else if( a_path == "/bd/upd_bd" )   	subSave();
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
}

//================================================================
//=========== TTipTransport ======================================
//================================================================
TTipTransport::TTipTransport()
{
    m_in = grpAdd("in_");
    m_out = grpAdd("out_");
}
    
TTipTransport::~TTipTransport()
{  
    nodeDelAll();
}

void TTipTransport::inAdd( const string &name, const string &idb )
{ 
    if( chldPresent(m_in,name) ) return;
    chldAdd(m_in,In(name,idb)); 
}

void TTipTransport::outAdd( const string &name, const string &idb )
{ 
    if( chldPresent(m_out,name) )	return;
    chldAdd(m_out,Out(name,idb)); 
}

void TTipTransport::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	TModule::cntrCmd_( a_path, opt, cmd );	//Call parent
	
	ctrMkNode("area",opt,0,a_path.c_str(),"/tr",Mess->I18N("Transports"));
	ctrMkNode("list",opt,-1,a_path.c_str(),"/tr/in",Mess->I18N("Input"),0664,0,0,4,"tp","br","idm","1","s_com","add,del","br_pref","in_");
	ctrMkNode("list",opt,-1,a_path.c_str(),"/tr/out",Mess->I18N("Output"),0664,0,0,4,"tp","br","idm","1","s_com","add,del","br_pref","out_");
    }    
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/tr/in" )
	{
	    inList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt,inAt(list[i_a]).at().name(),list[i_a].c_str()); 	
	}
	else if( a_path == "/tr/out" )
	{
	    outList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt,outAt(list[i_a]).at().name(),list[i_a].c_str());
	}   
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/tr/in" )
	{
	    if( opt->name() == "add" )		
	    {
		inAdd(opt->attr("id"));
		inAt(opt->attr("id")).at().name(opt->text());
	    }
	    else if( opt->name() == "del" )	chldDel(m_in,opt->attr("id"),-1,1);
	}
	else if( a_path == "/tr/out" )
	{
	    if( opt->name() == "add" )		
	    {		
		outAdd(opt->attr("id"));
		outAt(opt->attr("id")).at().name(opt->text());
	    }
	    else if( opt->name() == "del" )	chldDel(m_out,opt->attr("id"),-1,1);
	}
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
}

//================================================================
//=========== TTransportIn =======================================
//================================================================
TTransportIn::TTransportIn( const string &iid, const string &idb, TElem *el ) : 
    TConfig(el), run_st(false), m_bd(idb), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_dscr(cfg("DESCRIPT").getSd()), m_addr(cfg("ADDR").getSd()), m_prot(cfg("PROT").getSd()), 
    m_start(cfg("START").getBd())
{
    m_id = iid;
}
    
TTransportIn::~TTransportIn()
{
    
}

string TTransportIn::name()
{
    return (m_name.size())?m_name:m_id;
}

string TTransportIn::BD()
{
    return m_bd+"."+owner().owner().subId()+"_in";
}

void TTransportIn::postDisable(int flag)
{
    try
    {
        if( flag ) SYS->db().at().dataDel(BD(),SYS->transport().at().nodePath()+"In/",*this);
    }catch(TError err)
    { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
}									    

void TTransportIn::load( )
{
    SYS->db().at().dataGet(BD(),SYS->transport().at().nodePath()+"In/",*this);
}

void TTransportIn::save( )
{
    SYS->db().at().dataSet(BD(),SYS->transport().at().nodePath()+"In/",*this);
}

void TTransportIn::preEnable()
{ 
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

void TTransportIn::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",(Mess->I18N("Input transport: ")+name()).c_str());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("Transport"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/bd",Mess->I18N("Transport DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/id",cfg("ID").fld().descr(),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/name",cfg("NAME").fld().descr(),0664,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/dscr",cfg("DESCRIPT").fld().descr(),0664,0,0,3,"tp","str","cols","50","rows","3");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/prot",cfg("PROT").fld().descr(),0664,0,0,3,"tp","str","dest","select","select","/prm/cfg/p_mod");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/start",cfg("START").fld().descr(),0664,0,0,1,"tp","bool");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
    }    
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/st" )  		ctrSetB( opt, run_st );
	else if( a_path == "/prm/st/bd" )      	ctrSetS( opt, m_bd );
	else if( a_path == "/prm/cfg/id" )    	ctrSetS( opt, id() );
	else if( a_path == "/prm/cfg/name" )	ctrSetS( opt, name() );
	else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, dscr() );
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
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/st" )		(ctrGetB( opt ))?start():stop();
	else if( a_path == "/prm/st/bd" )      	m_bd = ctrGetS(opt);
	else if( a_path == "/prm/cfg/name" )   	m_name  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/dscr" )  	m_dscr  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/addr" )  	m_addr  = ctrGetS( opt );
	    else if( a_path == "/prm/cfg/prot" )m_prot  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
	else if( a_path == "/prm/cfg/load" )	load();
	else if( a_path == "/prm/cfg/save" ) 	save();
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
TTransportOut::TTransportOut( const string &iid, const string &idb, TElem *el ) : 
    TConfig(el), m_bd(idb), run_st(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_dscr(cfg("DESCRIPT").getSd()), m_addr(cfg("ADDR").getSd()), m_start(cfg("START").getBd())
{ 
    m_id = iid;
}

TTransportOut::~TTransportOut()
{
    
}

string TTransportOut::name()
{
    return (m_name.size())?m_name:m_id;
}

string TTransportOut::BD()
{
    return m_bd+"."+owner().owner().subId()+"_out";
}

void TTransportOut::postDisable(int flag)
{
    try
    {
        if( flag ) SYS->db().at().dataDel(BD(),SYS->transport().at().nodePath()+"Out/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}									    
	
void TTransportOut::load( )
{
    SYS->db().at().dataGet(BD(),SYS->transport().at().nodePath()+"Out/",*this);
}

void TTransportOut::save()
{
    SYS->db().at().dataSet(BD(),SYS->transport().at().nodePath()+"Out/",*this);
}

void TTransportOut::preEnable()
{ 
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

void TTransportOut::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",(Mess->I18N("Output transport: ")+name()).c_str());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("Transport"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/bd",Mess->I18N("Transport DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/id",cfg("ID").fld().descr(),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/name",cfg("NAME").fld().descr(),0664,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/dscr",cfg("DESCRIPT").fld().descr(),0664,0,0,3,"tp","str","cols","50","rows","3");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/start",cfg("START").fld().descr(),0664,0,0,1,"tp","bool");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
    }    
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/st" )		ctrSetB( opt, run_st );
	else if( a_path == "/prm/st/bd" )      	ctrSetS( opt, m_bd );
	else if( a_path == "/prm/cfg/id" )    	ctrSetS( opt, id() );
	else if( a_path == "/prm/cfg/name" ) 	ctrSetS( opt, name() );
	else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, dscr() );
	else if( a_path == "/prm/cfg/addr" )	ctrSetS( opt, m_addr );
	else if( a_path == "/prm/cfg/start" )	ctrSetB( opt, m_start );
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/st" )		(ctrGetB( opt ))?start():stop();
	else if( a_path == "/prm/st/bd" )      	m_bd 	= ctrGetS(opt);
	else if( a_path == "/prm/cfg/name" )   	m_name  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/dscr" )	m_dscr  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/addr" )	m_addr  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
	else if( a_path == "/prm/cfg/load" )  	load();
	else if( a_path == "/prm/cfg/save" ) 	save();	
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}


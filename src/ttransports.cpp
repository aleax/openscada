
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
    el_in.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_in.fldAdd( new TFld("MODULE",_("Transport type"),TFld::String,TCfg::Key,"20") );
    el_in.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    el_in.fldAdd( new TFld("DESCRIPT",_("Description"),TFld::String,TFld::NoFlag,"500") );
    el_in.fldAdd( new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"50") );
    el_in.fldAdd( new TFld("PROT",_("Transport protocol"),TFld::String,TFld::NoFlag,"20") );
    el_in.fldAdd( new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1") );
    
    //Output transport BD structure
    el_out.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_out.fldAdd( new TFld("MODULE",_("Transport type"),TFld::String,TCfg::Key,"20") );
    el_out.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    el_out.fldAdd( new TFld("DESCRIPT",_("Description"),TFld::String,TFld::NoFlag,"500") );
    el_out.fldAdd( new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"50") );
    el_out.fldAdd( new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1") );
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
	
	//- Search into DB -
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];		
		int fld_cnt=0;
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_in","",fld_cnt++,c_el) )
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
	
	//- Search into config file -
	int fld_cnt=0;
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_in",fld_cnt++,c_el) )
	{
	    id   = c_el.cfg("ID").getS();
	    type = c_el.cfg("MODULE").getS();	    
	    if( !at(type).at().inPresent(id) ) 
		at(type).at().inAdd(id,"*.*");	    
	    c_el.cfg("ID").setS("");
	    c_el.cfg("MODULE").setS("");
	}
	
    }catch( TError err )
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new input transports error.")); 
    }
    
    //Search and create new output transports
    try
    {
	TConfig c_el(&el_out);
	vector<string> tdb_ls, db_ls;
	
	//- Search into DB -
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		int fld_cnt=0;	
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_out","",fld_cnt++,c_el) )
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
	//- Search into config file -
	int fld_cnt=0;	
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_out",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("MODULE").getS();	    
	    if( !at(type).at().outPresent(id) ) 
		at(type).at().outAdd(id,"*.*");
	    c_el.cfg("ID").setS("");
	    c_el.cfg("MODULE").setS("");
	}
    }catch( TError err )
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new input transports error."));
    }
    
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
    mess_info(nodePath().c_str(),_("Start subsystem."));

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
	    }catch( TError err )
	    { 
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Start input transport <%s> error."),o_lst[i_o].c_str());
	    }
	     
	o_lst.clear();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if( !out.at().startStat() && out.at().toStart() ) 
		    out.at().start();
	    }catch( TError err )
	    { 
	        mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Start output transport <%s> error."),o_lst[i_o].c_str());
	    }
    }
}

void TTransportS::subStop( )
{
    mess_info(nodePath().c_str(),_("Stop subsystem."));
   
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
	    }catch( TError err )
	    { 
	        mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Stop input transport <%s> error."),o_lst[i_o].c_str());
	    }
	o_lst.clear();
	mod.at().outList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {	
		AutoHD<TTransportOut> out = mod.at().outAt(o_lst[i_o]);
		if( out.at().startStat() ) out.at().stop();
	    }catch( TError err )
	    { 
		mess_err(err.cat.c_str(),"%s",err.mess.c_str()); 
		mess_err(nodePath().c_str(),_("Stop output transport <%s> error."),o_lst[i_o].c_str());
	    }
    }
}

string TTransportS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
    	"======================= Subsystem \"Transports\" options ==================\n"
	"------------ Parameters of section <%s> in config file -----------\n\n"
	),nodePath().c_str());

    return(buf);
}

void TTransportS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),0440))
	{
	    ctrMkNode("comm",opt,-1,"/sub/load_db",_("Load"),0660);
	    ctrMkNode("comm",opt,-1,"/sub/upd_db",_("Save"),0660);
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","10");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else if( a_path == "/sub/load_db" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	subLoad();
    else if( a_path == "/sub/upd_db" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	subSave();
    else TSubSYS::cntrCmdProc(opt);
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

void TTipTransport::cntrCmdProc( XMLNode *opt )
{
    vector<string> list;
    //Get page info
    if( opt->name() == "info" )
    {
        TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/in_",_("Input transport"),0444,"root","root",1,"list","/tr/in");
	ctrMkNode("grp",opt,-1,"/br/out_",_("Output transport"),0444,"root","root",1,"list","/tr/out");
	if(ctrMkNode("area",opt,0,"/tr",_("Transports")))
	{
	    ctrMkNode("list",opt,-1,"/tr/in",_("Input"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","in_");
	    ctrMkNode("list",opt,-1,"/tr/out",_("Output"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","out_");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/tr/in" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    inList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(inAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
	{
	    inAdd(opt->attr("id"));
	    inAt(opt->attr("id")).at().name(opt->text());	
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	inDel(opt->attr("id"),true);
    }
    else if( a_path == "/tr/out" )
    {
    	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    outList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(outAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
	{
	    outAdd(opt->attr("id"));
	    outAt(opt->attr("id")).at().setName(opt->text());	
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	outDel(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}
	
//================================================================
//=========== TTransportIn =======================================
//================================================================
TTransportIn::TTransportIn( const string &iid, const string &idb, TElem *el ) : 
    TConfig(el), run_st(false), m_db(idb), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
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

string TTransportIn::tbl( )
{
    return owner().owner().subId()+"_in";
}

void TTransportIn::postDisable(int flag)
{
    try
    {
        if( flag ) SYS->db().at().dataDel(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}									    

void TTransportIn::load( )
{
    SYS->db().at().dataGet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportIn::save( )
{
    SYS->db().at().dataSet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportIn::preEnable(int flag)
{ 
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

void TTransportIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Input transport: ")+name()).c_str());
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB (module.db)"),0660,"root","root",1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/name",cfg("NAME").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCRIPT").fld().descr(),0664,"root","root",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/prot",cfg("PROT").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/p_mod");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",cfg("START").fld().descr(),0664,"root","root",1,"tp","bool");
		ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660);
		ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660);
	    }
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?start():stop();
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(m_db);
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	m_db = opt->text();
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/prm/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_name = opt->text();
    }	
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )  	opt->setText(dscr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_dscr = opt->text();
    }
    else if( a_path == "/prm/cfg/addr" )
    {
    	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(m_addr);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_addr = opt->text();
    }
    else if( a_path == "/prm/cfg/prot" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(m_prot);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_prot = opt->text();
    }
    else if( a_path == "/prm/cfg/start" )
    {
    	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(m_start?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_start = atoi(opt->text().c_str());
    }
    else if( a_path == "/prm/cfg/p_mod" && ctrChkNode(opt) )
    {
	vector<string> list;	
	SYS->protocol().at().modList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    opt->childAdd("el")->setText(list[i_a]);
    }
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	load();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) ) 	save();
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
TTransportOut::TTransportOut( const string &iid, const string &idb, TElem *el ) : 
    TConfig(el), m_db(idb), run_st(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
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

string TTransportOut::tbl( )
{
    return owner().owner().subId()+"_out";
}

void TTransportOut::postDisable(int flag)
{
    try
    {
        if( flag ) SYS->db().at().dataDel(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}									    
	
void TTransportOut::load( )
{
    SYS->db().at().dataGet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportOut::save()
{
    SYS->db().at().dataSet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportOut::preEnable(int flag)
{ 
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

void TTransportOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Output transport: ")+name()).c_str());
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB (module.db)"),0660,"root","root",1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/name",cfg("NAME").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCRIPT").fld().descr(),0664,"root","root",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",cfg("START").fld().descr(),0664,"root","root",1,"tp","bool");
		ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660);
		ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660);
	    }
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?start():stop();
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(m_db);
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	m_db = opt->text();
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/prm/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_name = opt->text();
    }	
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )  	opt->setText(dscr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_dscr = opt->text();
    }
    else if( a_path == "/prm/cfg/addr" )
    {
    	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(m_addr);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_addr = opt->text();
    }
    else if( a_path == "/prm/cfg/start" )
    {
    	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(m_start?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_start = atoi(opt->text().c_str());
    }
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	load();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) ) 	save();
}


//OpenSCADA system file: ttransports.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

//************************************************
//* TTransportS					 *
//************************************************
TTransportS::TTransportS( ) : TSubSYS("Transport","Transports",true)
{
    //- Input transport BD structure -
    el_in.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_in.fldAdd( new TFld("MODULE",_("Transport type"),TFld::String,TCfg::Key,"20") );
    el_in.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    el_in.fldAdd( new TFld("DESCRIPT",_("Description"),TFld::String,TFld::NoFlag,"500") );
    el_in.fldAdd( new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"50") );
    el_in.fldAdd( new TFld("PROT",_("Transport protocol"),TFld::String,TFld::NoFlag,"20") );
    el_in.fldAdd( new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1") );

    //- Output transport BD structure -
    el_out.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_out.fldAdd( new TFld("MODULE",_("Transport type"),TFld::String,TCfg::Key,"20") );
    el_out.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    el_out.fldAdd( new TFld("DESCRIPT",_("Description"),TFld::String,TFld::NoFlag,"500") );
    el_out.fldAdd( new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"50") );
    el_out.fldAdd( new TFld("START",_("To start"),TFld::Boolean,TFld::NoFlag,"1") );

    //- External hosts' conection DB struct -
    el_ext.fldAdd( new TFld("OP_USER",_("Open user"),TFld::String,TCfg::Key,"20") );
    el_ext.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_ext.fldAdd( new TFld("NAME",_("Name"),TFld::String,0,"50") );
    el_ext.fldAdd( new TFld("TRANSP",_("Transport"),TFld::String,0,"20") );
    el_ext.fldAdd( new TFld("ADDR",_("Transport address"),TFld::String,0,"50") );
    el_ext.fldAdd( new TFld("USER",_("Request user"),TFld::String,0,"20") );
    el_ext.fldAdd( new TFld("PASS",_("Request password"),TFld::String,0,"30") );
}

TTransportS::~TTransportS(  )
{

}

string TTransportS::extHostsDB()
{
    return SYS->workDB()+".CfgExtHosts";
}

void TTransportS::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
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

    //- Load parameters from config file -

    //- Load DB -
    string id,type;
    //-- Search and create new input transports --
    try
    {
	TConfig c_el(&el_in);
	c_el.cfgViewAll(false);
	vector<string> db_ls;
	
	//--- Search into DB ---
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_in","",fld_cnt++,c_el) )
	    {
		id   = c_el.cfg("ID").getS();
		type = c_el.cfg("MODULE").getS();
		if( !at(type).at().inPresent(id) )
		    at(type).at().inAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		c_el.cfg("ID").setS("");
		c_el.cfg("MODULE").setS("");
	    }
	}

	//--- Search into config file ---
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

    //-- Search and create new output transports --
    try
    {
	TConfig c_el(&el_out);
	c_el.cfgViewAll(false);
	vector<string> tdb_ls, db_ls;

	//--- Search into DB ---
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_out","",fld_cnt++,c_el) )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODULE").getS();
		if( !at(type).at().outPresent(id) )
		    at(type).at().outAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		c_el.cfg("ID").setS("");
		c_el.cfg("MODULE").setS("");
	    }
	}
	//--- Search into config file ---
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

    //-- Load external hosts --
    try
    {
        TConfig c_el(&el_ext);
        int fld_cnt = 0;
        while( SYS->db().at().dataSeek(extHostsDB(),nodePath()+"ExtTansp/",fld_cnt++,c_el) )
        {
	    ExtHost host("","","","","","","");
	    host.user_open	= c_el.cfg("OP_USER").getS();
	    host.id		= c_el.cfg("ID").getS();
	    host.name		= c_el.cfg("NAME").getS();
	    host.transp		= c_el.cfg("TRANSP").getS();
	    host.addr		= c_el.cfg("ADDR").getS();
	    host.user		= c_el.cfg("USER").getS();
	    host.pass		= c_el.cfg("PASS").getS();
	    extHostSet(host);
	    c_el.cfg("OP_USER").setS("");
	    c_el.cfg("ID").setS("");
	}
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and load external hosts DB is error."));
    }
}

void TTransportS::save_( )
{
    //- Save external transports -
    ResAlloc res(extHostRes,false);
    TConfig c_el(&el_ext);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
    {
	c_el.cfg("OP_USER").setS(extHostLs[i_h].user_open);
	c_el.cfg("ID").setS(extHostLs[i_h].id);
	c_el.cfg("NAME").setS(extHostLs[i_h].name);
	c_el.cfg("TRANSP").setS(extHostLs[i_h].transp);
	c_el.cfg("ADDR").setS(extHostLs[i_h].addr);
	c_el.cfg("USER").setS(extHostLs[i_h].user);
	c_el.cfg("PASS").setS(extHostLs[i_h].pass);
	SYS->db().at().dataSet(extHostsDB(),nodePath()+"ExtTansp/",c_el);
    }
    //-- Clear external transports --
    int fld_cnt=0;
    c_el.cfg("OP_USER").setS("");
    c_el.cfg("ID").setS("");
    c_el.cfgViewAll(false);
    while( SYS->db().at().dataSeek(extHostsDB(),nodePath()+"ExtTansp/",fld_cnt++,c_el) )
    {
	if( !extHostGet(c_el.cfg("OP_USER").getS(),c_el.cfg("ID").getS()).id.size() )
	{
	    SYS->db().at().dataDel(extHostsDB(),nodePath()+"ExtTansp/",c_el);
	    fld_cnt--;
	}
	c_el.cfg("OP_USER").setS("");
	c_el.cfg("ID").setS("");
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

void TTransportS::extHostList( const string &user, vector<string> &list )
{
    list.clear();
    ResAlloc res(extHostRes,false);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
	if( !user.size() || user == extHostLs[i_h].user_open )
	    list.push_back(extHostLs[i_h].id);
}

bool TTransportS::extHostPresent( const string &user, const string &iid )
{
    ResAlloc res(extHostRes,false);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
	if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == iid )
	    return true;
    return false;
}

void TTransportS::extHostSet( const ExtHost &host )
{
    ResAlloc res(extHostRes,true);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
	if( host.user_open == extHostLs[i_h].user_open && extHostLs[i_h].id == host.id )
	{ extHostLs[i_h] = host; modif(); return; }
    extHostLs.push_back(host);
    modif();
}

void TTransportS::extHostDel( const string &user, const string &id )
{
    ResAlloc res(extHostRes,true);
    for(int i_h = 0; i_h < extHostLs.size(); )
	if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == id )
	    extHostLs.erase(extHostLs.begin()+i_h);
	else i_h++;
    modif();
}

TTransportS::ExtHost TTransportS::extHostGet( const string &user, const string &id )
{
    ResAlloc res(extHostRes,false);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
	if( (user.empty() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == id )
	    return extHostLs[i_h];
    return ExtHost(user,"","","","","","");
}

AutoHD<TTransportOut> TTransportS::extHost( TTransportS::ExtHost host, const string &pref )
{
    if( !host.id.size() || !modPresent(host.transp) )
	throw TError(nodePath().c_str(),_("Remote host error!"));

    if( !at(host.transp).at().outPresent(pref+host.id) )
	at(host.transp).at().outAdd(pref+host.id);
    if( at(host.transp).at().outAt(pref+host.id).at().addr() != host.addr )
    {
	at(host.transp).at().outAt(pref+host.id).at().setAddr(host.addr);
	at(host.transp).at().outAt(pref+host.id).at().stop();
    }

    return at(host.transp).at().outAt(pref+host.id);
}

void TTransportS::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),0440))
	{
	    ctrMkNode("fld",opt,-1,"/sub/sysHosts",_("System's external hosts"),0660,"root","root",1,"tp","bool");
	    if(ctrMkNode("table",opt,-1,"/sub/ehost",_("External hosts poll"),0666,"root","root",2,"s_com","add,del","key","id"))
	    {
		ctrMkNode("list",opt,-1,"/sub/ehost/id",_("Id"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/sub/ehost/name",_("Name"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/sub/ehost/transp",_("Transport"),0666,"root","root",4,"tp","str","idm","1","dest","select","select","/sub/transps");
	        ctrMkNode("list",opt,-1,"/sub/ehost/addr",_("Address"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/sub/ehost/user",_("User"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/sub/ehost/pass",_("Password"),0666,"root","root",1,"tp","str");
	    }
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","10");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else if( a_path == "/sub/sysHosts" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(sysHost()) );
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	setSysHost( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/sub/transps" && ctrChkNode(opt) )
    {
	vector<string>  list;
	modList(list);
	for( int i_a = 0; i_a < list.size(); i_a++ )
	    opt->childAdd("el")->setAttr("id",list[i_a])->setText(modAt(list[i_a]).at().modName());
    }
    else if( a_path == "/sub/ehost" )
    {
	TConfig c_el(&el_ext);
	c_el.cfg("OP_USER").setS(opt->attr("user"));
	if( ctrChkNode(opt,"get",0666,"root","root",SEQ_RD) )
	{
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/sub/ehost/id","",0666);
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/sub/ehost/name","",0666);
	    XMLNode *n_tr	= ctrMkNode("list",opt,-1,"/sub/ehost/transp","",0666);
	    XMLNode *n_addr	= ctrMkNode("list",opt,-1,"/sub/ehost/addr","",0666);
	    XMLNode *n_user	= ctrMkNode("list",opt,-1,"/sub/ehost/user","",0666);
	    XMLNode *n_pass	= ctrMkNode("list",opt,-1,"/sub/ehost/pass","",0666);

	    vector<string> list;
	    extHostList(sysHost()?"*":opt->attr("user"),list);
	    for(int i_h = 0; i_h < list.size(); i_h++)
	    {
		ExtHost host = extHostGet(sysHost()?"*":opt->attr("user"),list[i_h]);
		if( n_id )	n_id->childAdd("el")->setText(host.id);
		if( n_nm )	n_nm->childAdd("el")->setText(host.name);
		if( n_tr )	n_tr->childAdd("el")->setText(host.transp);
		if( n_addr )	n_addr->childAdd("el")->setText(host.addr);
		if( n_user )	n_user->childAdd("el")->setText(host.user);
		if( n_pass )	n_pass->childAdd("el")->setText(host.pass.size()?"*******":"");
	    }
	}
	if( ctrChkNode(opt,"add",0666,"root","root",SEQ_WR) )
	    extHostSet(ExtHost(sysHost()?"*":opt->attr("user"),"newHost","New external host","","",opt->attr("user"),""));
	if( ctrChkNode(opt,"del",0666,"root","root",SEQ_WR) )
	    extHostDel(sysHost()?"*":opt->attr("user"),opt->attr("key_id") );
	if( ctrChkNode(opt,"set",0666,"root","root",SEQ_WR) )
	{
	    string col  = opt->attr("col");
	    ExtHost host = extHostGet(sysHost()?"*":opt->attr("user"),opt->attr("key_id"));
	    if( col == "id" )
	    {
		host.id = opt->text();
		extHostDel(sysHost()?"*":opt->attr("user"),opt->attr("key_id"));
	    }
	    else if( col == "name" )	host.name = opt->text();
	    else if( col == "transp" )	host.transp = opt->text();
	    else if( col == "addr" )	host.addr = opt->text();
	    else if( col == "user" )	host.user = opt->text();
	    else if( col == "pass" )	host.pass = opt->text();
	    extHostSet(host);
	}
    }
    else TSubSYS::cntrCmdProc(opt);
}


//************************************************
//* TTipTransport                                *
//************************************************
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
    if( chldPresent(m_out,name) ) return;
    chldAdd(m_out,Out(name,idb));
}

void TTipTransport::cntrCmdProc( XMLNode *opt )
{
    vector<string> list;
    //- Get page info -
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/in_",_("Input transport"),0664,"root","root",1,"idm","1");
	ctrMkNode("grp",opt,-1,"/br/out_",_("Output transport"),0664,"root","root",1,"idm","1");
	if(ctrMkNode("area",opt,0,"/tr",_("Transports")))
	{
	    ctrMkNode("list",opt,-1,"/tr/in",_("Input"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","in_");
	    ctrMkNode("list",opt,-1,"/tr/out",_("Output"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","out_");
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/br/in_" || a_path == "/tr/in" )
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
	    inAt(opt->attr("id")).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	inDel(opt->attr("id"),true);
    }
    else if( a_path == "/br/out_" || a_path == "/tr/out" )
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

//************************************************
//* TTransportIn				 *
//************************************************
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

TCntrNode &TTransportIn::operator=( TCntrNode &node )
{
    TTransportIn *src_n = dynamic_cast<TTransportIn*>(&node);
    if( !src_n ) return *this;

    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    cfg("MODULE").setS(owner().modId());
    m_id = tid;
    setDB(src_n->m_db);

    return *this;
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

string TTransportIn::getStatus( )
{
    return startStat() ? _("Started. ") : _("Stoped. ");
}

void TTransportIn::load_( )
{
    SYS->db().at().dataGet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportIn::save_( )
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
    //- Get page info -
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Input transport: ")+name()).c_str(),0664,"root","root");
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB"),0664,"root","root",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/name",cfg("NAME").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCRIPT").fld().descr(),0664,"root","root",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/prot",cfg("PROT").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/p_mod");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",cfg("START").fld().descr(),0664,"root","root",1,"tp","bool");
	    }
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/status" && ctrChkNode(opt) )		opt->setText(getStatus());
    else if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?start():stop();
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(DB());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDB(opt->text());
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/prm/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setName(opt->text());
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )  	opt->setText(dscr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDscr(opt->text());
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(addr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setAddr(opt->text());
    }
    else if( a_path == "/prm/cfg/prot" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(protocol());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setProtocol(opt->text());
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/p_mod" && ctrChkNode(opt) )
    {
	vector<string> list;
	SYS->protocol().at().modList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    opt->childAdd("el")->setText(list[i_a]);
    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* TTransportOut                                *
//************************************************
TTransportOut::TTransportOut( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), m_db(idb), run_st(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()),
    m_dscr(cfg("DESCRIPT").getSd()), m_addr(cfg("ADDR").getSd()), m_start(cfg("START").getBd()),
    m_prm1(0), m_prm2(0)
{
    m_id = iid;
}

TTransportOut::~TTransportOut()
{

}

TCntrNode &TTransportOut::operator=( TCntrNode &node )
{
    TTransportOut *src_n = dynamic_cast<TTransportOut*>(&node);
    if( !src_n ) return *this;

    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    cfg("MODULE").setS(owner().modId());
    m_id = tid;
    setDB(src_n->m_db);

    return *this;
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

string TTransportOut::getStatus( )
{
    return startStat() ? _("Started. ") : _("Stoped. ");
}

void TTransportOut::load_( )
{
    SYS->db().at().dataGet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportOut::save_()
{
    SYS->db().at().dataSet(fullDB(),SYS->transport().at().nodePath()+tbl(),*this);
}

void TTransportOut::preEnable(int flag)
{
    cfg("MODULE").setS(owner().modId());
    try{ load(); }catch(...){ }
}

string TTransportOut::messProtIO( const string &in, const string &prot )
{
    if( !SYS->protocol().at().modPresent(prot) )
	throw TError(nodePath().c_str(),_("Transport protocol '%s' no present"),prot.c_str());
    return SYS->protocol().at().at(prot).at().outMess( in, *this );
}

void TTransportOut::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Output transport: ")+name()).c_str(),0664,"root","root");
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB"),0664,"root","root",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/name",cfg("NAME").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCRIPT").fld().descr(),0664,"root","root",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",cfg("START").fld().descr(),0664,"root","root",1,"tp","bool");
	    }
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/status" && ctrChkNode(opt) )		opt->setText(getStatus());
    else if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?start():stop();
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(DB());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDB(opt->text());
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/prm/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setName(opt->text());
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(dscr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDscr(opt->text());
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(addr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setAddr(opt->text());
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else TCntrNode::cntrCmdProc(opt);
}

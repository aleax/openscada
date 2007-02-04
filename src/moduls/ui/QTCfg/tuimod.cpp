
//OpenSCADA system module UI.QTCfg file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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
#include <sys/types.h>
#include <unistd.h>       

#include <QIcon>
#include <QMessageBox>

#include <resalloc.h>
#include <tsys.h>
#include <tmess.h>
#include "qtcfg.h"
#include "selfwidg.h"
#include "tuimod.h"

#include "xpm/oscada_cfg.xpm"

//============ Modul info! =====================================================
#define MOD_ID      "QTCfg"
#define MOD_NAME    "System configurator (QT)"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "QT"
#define VERSION     "1.2.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow the QT based OpenSCADA system configurator."
#define LICENSE     "GPL"
//==============================================================================

QTCFG::TUIMod *QTCFG::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
    	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	QTCFG::TUIMod *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = QTCFG::mod = new QTCFG::TUIMod( source );       

	return self_addr;
    }    
}

using namespace QTCFG;

//==============================================================================
//================= QTCFG::TUIMod =============================================
//==============================================================================

TUIMod::TUIMod( string name ) : start_path(string("/")+SYS->id())
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
    
    extHostRes = ResAlloc::resCreate( );
    
    //Public export functions
    modFuncReg( new ExpFunc("QIcon icon();","Module QT-icon",(void(TModule::*)( )) &TUIMod::icon) );
    modFuncReg( new ExpFunc("QMainWindow *openWindow();","Start QT GUI.",(void(TModule::*)( )) &TUIMod::openWindow) );
    
    //External hosts' conection DB struct
    el_ext.fldAdd( new TFld("OP_USER",I18N("Open user"),TFld::String,TCfg::Key,"20") ); 
    el_ext.fldAdd( new TFld("ID",I18N("ID"),TFld::String,TCfg::Key,"20") );
    el_ext.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,0,"50") );
    el_ext.fldAdd( new TFld("TRANSP",I18N("Transport"),TFld::String,0,"20") );
    el_ext.fldAdd( new TFld("ADDR",I18N("Transport address"),TFld::String,0,"50") );
    el_ext.fldAdd( new TFld("USER",I18N("Request user"),TFld::String,0,"20") );
    el_ext.fldAdd( new TFld("PASS",I18N("Request password"),TFld::String,0,"30") );
}

TUIMod::~TUIMod()
{
    if( run_st ) modStop();
    
    ResAlloc::resDelete(extHostRes);
}

string TUIMod::extTranspBD()
{
    return SYS->workDB()+".CfgExtHosts";
}

string TUIMod::modInfo( const string &name )
{
    if( name == "SubType" ) return SUB_TYPE;
    else return TModule::modInfo( name);
}

void TUIMod::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"StartPath  <path>    Configurator start path.\n"
	"StartUser  <user>    No password requested start user.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TUIMod::modLoad( )
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

    //========== Load parameters from config file and DB =============
    start_path = TBDS::genDBGet(nodePath()+"StartPath",start_path);
    start_user = TBDS::genDBGet(nodePath()+"StartUser",start_user);
    try
    {
	TConfig c_el(&el_ext);
	int fld_cnt = 0;
        while( SYS->db().at().dataSeek(extTranspBD(),nodePath()+"ExtTansp/",fld_cnt++,c_el) )
        {
	    ExtHost host("","","","","","","");
	    host.user_open 	= c_el.cfg("OP_USER").getS();
	    host.id	= c_el.cfg("ID").getS();
	    host.name	= c_el.cfg("NAME").getS();
	    host.transp	= c_el.cfg("TRANSP").getS();
	    host.addr	= c_el.cfg("ADDR").getS();
	    host.user	= c_el.cfg("USER").getS();
	    host.pass	= c_el.cfg("PASS").getS();
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

void TUIMod::modSave( )
{
    //========== Save parameters to DB =============
    TBDS::genDBSet(nodePath()+"StartPath",start_path);
    TBDS::genDBSet(nodePath()+"StartUser",start_user);
    
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
	SYS->db().at().dataSet(extTranspBD(),nodePath()+"ExtTansp/",c_el);
    }
    //Clear IO
    int fld_cnt=0;
    c_el.cfg("OP_USER").setS("");
    c_el.cfg("ID").setS("");
    while( SYS->db().at().dataSeek(extTranspBD(),nodePath()+"ExtTansp/",fld_cnt++,c_el) )
    {	
        if( !extHostGet(c_el.cfg("OP_USER").getS(),c_el.cfg("ID").getS()).id.size() )
        {
    	    SYS->db().at().dataDel(extTranspBD(),nodePath()+"ExtTansp/",c_el);
            fld_cnt--;
        }
	c_el.cfg("OP_USER").setS("");
	c_el.cfg("ID").setS("");
    }    
}

void TUIMod::postEnable( )
{
    TModule::postEnable( );
}

QIcon TUIMod::icon()
{
    return QIcon(oscada_cfg_xpm);
}

QMainWindow *TUIMod::openWindow()
{
    string user_open = startUser();
    if(!SYS->security().at().usrPresent(user_open))
	while(true)
	{ 
	    vector<string> u_list;
	    DlgUser *d_usr = new DlgUser( );
	    SYS->security().at().usrList(u_list);
	    d_usr->user(u_list);
	    int rez = d_usr->exec();
	    string dl_user   = d_usr->user().toAscii().data();
	    string dl_passwd = d_usr->password().toAscii().data();
	    delete d_usr;
	
	    if(!rez) return NULL;
	
	    if( !SYS->security().at().usrPresent(dl_user) || !SYS->security().at().usrAt(dl_user).at().auth(dl_passwd) )
	    {
		QMessageBox::warning(NULL,_("QT Configurator of OpenSCADA"),_("Auth wrong!!!"));
		continue;
	    }
	    user_open = dl_user;
	    break;
	}
    return new ConfApp(user_open);
}

void TUIMod::modStart()
{
    run_st = true;
}

void TUIMod::modStop()
{   
    int i_w;
    for( i_w = 0; i_w < cfapp.size(); i_w++ )
        if( cfapp[i_w] ) emit cfapp[i_w]->close();//deleteLater();// close();

    //Wait real windows close 
    do 
	for( i_w = 0; i_w < cfapp.size(); i_w++ ) 
	    if( cfapp[i_w] ) break;
    while(i_w<cfapp.size());
    struct timespec tm = {0,500000000};
    nanosleep(&tm,NULL);
    
    run_st = false;
}

void TUIMod::regWin( QMainWindow *win )
{
    int i_w;
    for( i_w = 0; i_w < cfapp.size(); i_w++ )
        if( cfapp[i_w] == NULL ) break;
    if( i_w == cfapp.size() )	cfapp.push_back(NULL);	
    cfapp[i_w] = win;
}

void TUIMod::unregWin( QMainWindow *win )
{
    for( int i_w = 0; i_w < cfapp.size(); i_w++ )
	if( cfapp[i_w] == win )	cfapp[i_w] = NULL;
}

void TUIMod::extHostList(const string &user, vector<string> &list)
{
    list.clear();
    ResAlloc res(extHostRes,false);    
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
	if( !user.size() || user == extHostLs[i_h].user_open )
	    list.push_back(extHostLs[i_h].id);
}

bool TUIMod::extHostPresent(const string &user, const string &iid)
{
    ResAlloc res(extHostRes,false);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
        if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == iid )
	    return true;
    return false;	
}

void TUIMod::extHostSet(const ExtHost &host)
{
    ResAlloc res(extHostRes,true);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
        if( host.user_open == extHostLs[i_h].user_open && extHostLs[i_h].id == host.id )
	{ extHostLs[i_h] = host; return; }
    extHostLs.push_back(host);
}

void TUIMod::extHostDel(const string &user, const string &id)
{
    ResAlloc res(extHostRes,true);    
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
        if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == id )
	{
	    extHostLs.erase(extHostLs.begin()+i_h);	    
	    i_h--;
	}
}

ExtHost TUIMod::extHostGet(const string &user, const string &id)
{
    ResAlloc res(extHostRes,false);
    for(int i_h = 0; i_h < extHostLs.size(); i_h++)
        if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == id )
	    return extHostLs[i_h];
    return ExtHost(user,"","","","","","");
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TUI::cntrCmdProc(opt);
        if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_path",_("Configurator start path"),0664,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Configurator start user"),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/u_lst");
	    if(ctrMkNode("table",opt,-1,"/prm/cfg/ehost",_("External hosts connect"),0666,"root","root",2,"s_com","add,del","key","id"))
	    {
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/id",_("Id"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/name",_("Name"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/transp",_("Transport"),0666,"root","root",4,"tp","str","idm","1","dest","select","select","/prm/cfg/transps");
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/addr",_("Address"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/user",_("User"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/pass",_("Password"),0666,"root","root",1,"tp","str");
	    }
    	    ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660);
    	    ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660);	
	}
        ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");    
    if( a_path == "/prm/cfg/start_path" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(start_path);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	start_path = opt->text();
    }
    else if( a_path == "/prm/cfg/start_user" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(start_user);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	start_user = opt->text();
    }
    else if( a_path == "/prm/cfg/ehost" )
    {
	if( ctrChkNode(opt,"get",0666,"root","root",SEQ_RD) )
        {
            XMLNode *n_id	= ctrMkNode("list",opt,-1,"/prm/cfg/ehost/id","",0666);
            XMLNode *n_nm       = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/name","",0666);
            XMLNode *n_tr    	= ctrMkNode("list",opt,-1,"/prm/cfg/ehost/transp","",0666);
            XMLNode *n_addr     = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/addr","",0666);
            XMLNode *n_user     = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/user","",0666);
            XMLNode *n_pass	= ctrMkNode("list",opt,-1,"/prm/cfg/ehost/pass","",0666);
	    
	    vector<string> list;
	    extHostList(opt->attr("user"),list);		
	    for(int i_h = 0; i_h < list.size(); i_h++)
	    {
		ExtHost host = extHostGet(opt->attr("user"),list[i_h]);
		if(n_id)	n_id->childAdd("el")->text(host.id);
		if(n_nm)	n_nm->childAdd("el")->text(host.name);
		if(n_tr)	n_tr->childAdd("el")->text(host.transp);
		if(n_addr)	n_addr->childAdd("el")->text(host.addr);
		if(n_user)	n_user->childAdd("el")->text(host.user);
		if(n_pass)	n_pass->childAdd("el")->text(host.pass.size()?"*******":"");
	    }
	}
	if( ctrChkNode(opt,"add",0666,"root","root",SEQ_WR) )
	    extHostSet(ExtHost(opt->attr("user"),"newHost","New external host","","",opt->attr("user"),""));
	if( ctrChkNode(opt,"del",0666,"root","root",SEQ_WR) )
	    extHostDel(opt->attr("user"),opt->attr("key_id") );
	if( ctrChkNode(opt,"set",0666,"root","root",SEQ_WR) )
	{
	    string col  = opt->attr("col");	    
	    ExtHost host = extHostGet(opt->attr("user"),opt->attr("key_id"));
	    if( col == "id" )		
	    {
		host.id = opt->text();
		extHostDel(opt->attr("user"),opt->attr("key_id"));
	    }
	    else if( col == "name" )	host.name = opt->text();
	    else if( col == "transp" )	host.transp = opt->text();
	    else if( col == "addr" )	host.addr = opt->text();
	    else if( col == "user" )	host.user = opt->text();
	    else if( col == "pass" )	host.pass = opt->text();
	    extHostSet(host);
	}
    }
    else if( a_path == "/prm/cfg/transps" && ctrChkNode(opt) )
    {
	vector<string>	list;
	SYS->transport().at().modList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
            opt->childAdd("el")->attr("id",list[i_a])->text(SYS->transport().at().modAt(list[i_a]).at().modName());
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->text(optDescr());
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )  modLoad();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )  modSave();
    else if( a_path == "/prm/cfg/u_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	opt->childAdd("el")->text("");
	for(int i_u = 0; i_u < ls.size(); i_u++)
	    opt->childAdd("el")->text(ls[i_u]);
    }
    else TUI::cntrCmdProc(opt);
}

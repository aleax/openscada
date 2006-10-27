
//OpenSCADA system file: tsecurity.cpp
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
#include "tsecurity.h"

TSecurity::TSecurity( ) : 
    TSubSYS("Security","Security",false)
{
    m_usr = TCntrNode::grpAdd("usr_");
    m_grp = TCntrNode::grpAdd("grp_");
    
    //User BD structure
    user_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,FLD_KEY,"20") );
    user_el.fldAdd( new TFld("DESCR",Mess->I18N("Full name"),TFld::String,0,"50") );
    user_el.fldAdd( new TFld("PASS",Mess->I18N("Password"),TFld::String,0,"20") );
    user_el.fldAdd( new TFld("GRP",Mess->I18N("Default group"),TFld::String,0,"20") );
    //Group BD structure
    grp_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,FLD_KEY,"20") );
    grp_el.fldAdd( new TFld("DESCR",Mess->I18N("Full name"),TFld::String,0,"50") );
    grp_el.fldAdd( new TFld("USERS",Mess->I18N("Users"),TFld::String,0,"200") );
}

void TSecurity::postEnable()
{
    //Add surely users, groups and set parameters
    usrAdd("root");
    usrAt("root").at().lName(Mess->I18N("Administrator (superuser)!!!"));
    usrAt("root").at().pass("openscada");

    grpAdd("root");
    grpAt("root").at().lName(Mess->I18N("Administrators group."));
    
    TSubSYS::postEnable();
}

TSecurity::~TSecurity(  )
{
    nodeDelAll();
}

void TSecurity::usrAdd( const string &name, const string &idb )
{    
    if( !chldPresent(m_usr,name) )	chldAdd(m_usr,new TUser(name,idb,&user_el));
}

void TSecurity::grpAdd( const string &name, const string &idb )
{
    if( !chldPresent(m_grp,name) )	chldAdd(m_grp,new TGroup(name,idb,&grp_el));
}

char TSecurity::access( const string &user, char mode, const string &owner, const string &group, int access )
{
    char rez = 0;

    AutoHD<TUser> r_usr = usrAt(user);
    // Check owner permision
    if( user == "root" || user == owner )	
	rez = ((access&0700)>>6)&mode;
    // Check other permision
    if( rez == mode )	return rez;
    rez|=(access&07)&mode;
    if( rez == mode )   return rez;
    // Check groupe permision
    if( group == r_usr.at().grp() || grpAt(group).at().user(user) || grpAt("root").at().user(user) )
	rez|=((access&070)>>3)&mode;

    return rez;
}

void TSecurity::subLoad( )
{
    //========== Load commandline data ==================
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
    //========== Load parametrs ==================

    //================ Load DB ==================
    string 	name;
    
    //Search and create new users
    try
    {
	TConfig g_cfg(&user_el);	
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
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_user","",fld_cnt++,g_cfg) )
		{
		    name = g_cfg.cfg("NAME").getS();
		    if( !usrPresent(name) )	usrAdd(name,(wbd==SYS->workDB())?"*.*":wbd);
		    g_cfg.cfg("NAME").setS("");
		}
	    }
	}
	
	//- Search into config file -
	int fld_cnt=0;	
	while( SYS->db().at().dataSeek("",nodePath()+"User/",fld_cnt++,g_cfg) )
	{
	    name = g_cfg.cfg("NAME").getS();
	    if( !usrPresent(name) )	usrAdd(name,"*.*");
	    g_cfg.cfg("NAME").setS("");
	}
    }catch(TError err)
    { 
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Search and create new users error."));
    }
    
    //Search and create new user groups
    try
    {
	TConfig g_cfg(&grp_el);
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
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_grp","",fld_cnt++,g_cfg) )
		{
		    name = g_cfg.cfg("NAME").getS();
		    if( !grpPresent(name) )	grpAdd(name,(wbd==SYS->workDB())?"*.*":wbd);
		    g_cfg.cfg("NAME").setS("");	
		}
	    }
	}
	
	//- Search into config file -
        int fld_cnt=0;		
	while( SYS->db().at().dataSeek("",nodePath()+"Grp/",fld_cnt++,g_cfg) )
	{
	    name = g_cfg.cfg("NAME").getS();
	    if( !grpPresent(name) )	grpAdd(name,"*.*");
	    g_cfg.cfg("NAME").setS("");	
	}
    }catch(TError err)
    { 
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Search and create new user's groups error."));
    }
    
    //Load present user and groups
    vector<string> list;
    usrList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
        usrAt(list[i_l]).at().load();
    grpList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
        grpAt(list[i_l]).at().load();	
}

void TSecurity::subSave( )
{
    vector<string> list;
    
    //========== Save parametrs ==================
    
    // Save users to bd
    usrList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
	usrAt(list[i_l]).at().save();
    
    // Save groups to bd
    grpList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
	grpAt(list[i_l]).at().save();
}

string TSecurity::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"======================= Subsystem \"Security\" options ====================\n"
	"------------ Parameters of section <%s> in config file -----------\n\n"
	),nodePath().c_str());
    
    return buf;
}

void TSecurity::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/usr_",Mess->I18N("User"),0444,"root","root",1,"list","/usgr/users");
	ctrMkNode("grp",opt,-1,"/br/grp_",Mess->I18N("Group"),0444,"root","root",1,"list","/usgr/grps");	
        ctrMkNode("area",opt,0,"/bd",Mess->I18N("Subsystem"),0440,"root",subId().c_str());
    	ctrMkNode("comm",opt,-1,"/bd/load_bd",Mess->I18N("Load"),0440,"root",subId().c_str());
    	ctrMkNode("comm",opt,-1,"/bd/upd_bd",Mess->I18N("Save"),0440,"root",subId().c_str());
    	ctrMkNode("area",opt,1,"/usgr",Mess->I18N("Users and groups"));
    	ctrMkNode("list",opt,-1,"/usgr/users",Mess->I18N("Users"),0664,"root",subId().c_str(),3,"tp","br","s_com","add,del","br_pref","usr_");
    	ctrMkNode("list",opt,-1,"/usgr/grps",Mess->I18N("Groups"),0664,"root",subId().c_str(),3,"tp","br","s_com","add,del","br_pref","grp_");
    	ctrMkNode("fld",opt,-1,"/help/g_help",Mess->I18N("Options help"),0440,"root",subId().c_str(),3,"tp","str","cols","90","rows","10");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root",subId().c_str()) )	opt->text(optDescr());
    else if( a_path == "/usgr/users" )
    {
	if( ctrChkNode(opt,"get",0664,"root",subId().c_str(),SEQ_RD) )
	{
	    vector<string> list;   
    	    usrList(list);
    	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	        opt->childAdd("el")->text(list[i_a]);
    	}
	if( ctrChkNode(opt,"add",0664,"root",subId().c_str(),SEQ_WR) )	usrAdd(opt->text());
	if( ctrChkNode(opt,"del",0664,"root",subId().c_str(),SEQ_WR) )	chldDel(m_usr,opt->text(),-1,1);
    }
    else if( a_path == "/usgr/grps" )
    {
	if( ctrChkNode(opt,"get",0664,"root",subId().c_str(),SEQ_RD) )
	{
	    vector<string> list;   
    	    grpList(list);
    	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	        opt->childAdd("el")->text(list[i_a]);
    	}
	if( ctrChkNode(opt,"add",0664,"root",subId().c_str(),SEQ_WR) )	grpAdd(opt->text());
	if( ctrChkNode(opt,"del",0664,"root",subId().c_str(),SEQ_WR) )	chldDel(m_grp,opt->text(),-1,1);	
    }
    else if( a_path == "/bd/load_bd" && ctrChkNode(opt,"set",0440,"root",subId().c_str(),SEQ_RD) )	subLoad();
    else if( a_path == "/bd/upd_bd" && ctrChkNode(opt,"set",0440,"root",subId().c_str(),SEQ_RD) )     	subSave();
    else TSubSYS::cntrCmdProc(opt);
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
    
TUser::TUser( const string &nm, const string &idb, TElem *el ) : 
    TConfig(el), m_bd(idb), m_lname(cfg("DESCR").getSd()), m_pass(cfg("PASS").getSd()), 
    m_name(cfg("NAME").getSd()), m_grp(cfg("GRP").getSd())
{
    m_name = nm;
}

TUser::~TUser(  )
{

}

bool TUser::auth( const string &ipass )
{ 
    return (m_pass == ipass)?true:false; 
}

void TUser::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(BD(),owner().nodePath()+"User/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
}

string TUser::BD()
{
    return m_bd+"."+owner().subId()+"_user";
}

void TUser::load( )
{
    SYS->db().at().dataGet(BD(),owner().nodePath()+"User/",*this);
}

void TUser::save( )
{
    SYS->db().at().dataSet(BD(),owner().nodePath()+"User/",*this);
}

void TUser::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",Mess->I18Ns("User ")+name());
	ctrMkNode("area",opt,-1,"/prm",Mess->I18N("User"));
	ctrMkNode("fld",opt,-1,"/prm/name",cfg("NAME").fld().descr(),0444,"root","root",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/dscr",cfg("DESCR").fld().descr(),0664,name().c_str(),owner().subId().c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/bd",Mess->I18N("User DB (module.db)"),0664,"root",SYS->db().at().subId().c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/grp",cfg("GRP").fld().descr(),0664,"root",owner().subId().c_str(),3,"tp","str","dest","select","select","/prm/grps");
	ctrMkNode("fld",opt,-1,"/prm/pass",cfg("PASS").fld().descr(),0660,name().c_str(),owner().subId().c_str(),1,"tp","str");
	ctrMkNode("comm",opt,-1,"/prm/load",Mess->I18N("Load"),0440,"root",owner().subId().c_str());
	ctrMkNode("comm",opt,-1,"/prm/save",Mess->I18N("Save"),0440,"root",owner().subId().c_str());
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/bd" )
    {
	if( ctrChkNode(opt,"get",0664,"root",SYS->db().at().subId().c_str(),SEQ_RD) )	opt->text(m_bd);
	if( ctrChkNode(opt,"set",0664,"root",SYS->db().at().subId().c_str(),SEQ_WR) )	m_bd = opt->text();
    }
    else if( a_path == "/prm/name" && ctrChkNode(opt) )	opt->text(name());
    else if( a_path == "/prm/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,name().c_str(),owner().subId().c_str(),SEQ_RD) )	opt->text(lName());
	if( ctrChkNode(opt,"set",0664,name().c_str(),owner().subId().c_str(),SEQ_WR) )	lName(opt->text());
    }
    else if( a_path == "/prm/grp" )
    {
	if( ctrChkNode(opt,"get",0664,"root",owner().subId().c_str(),SEQ_RD) )	opt->text(grp());
	if( ctrChkNode(opt,"set",0664,"root",owner().subId().c_str(),SEQ_WR) )	grp(opt->text());
    }	
    else if( a_path == "/prm/pass" )
    {
	if( ctrChkNode(opt,"get",0660,name().c_str(),owner().subId().c_str(),SEQ_RD) )	opt->text("**********");
	if( ctrChkNode(opt,"set",0660,name().c_str(),owner().subId().c_str(),SEQ_WR) )	pass(opt->text());
    }
    else if( a_path == "/prm/grps" && ctrChkNode(opt,"get",0444,name().c_str(),owner().subId().c_str(),SEQ_RD) )
    {
	vector<string> list;	
        owner().grpList(list);
        for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	    opt->childAdd("el")->text(list[i_a]);
    }
    else if( a_path == "/prm/load" && ctrChkNode(opt,"set",0440,"root",owner().subId().c_str(),SEQ_RD) ) load();
    else if( a_path == "/prm/save" && ctrChkNode(opt,"set",0440,"root",owner().subId().c_str(),SEQ_RD) ) save();
}	    

//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
TGroup::TGroup( const string &nm, const string &idb, TElem *el ) : 
    TConfig(el), m_bd(idb), m_lname(cfg("DESCR").getSd()), m_usrs(cfg("USERS").getSd()), 
    m_name(cfg("NAME").getSd())
{
    m_name = nm;
}

TGroup::~TGroup(  )
{

}

void TGroup::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(BD(),owner().nodePath()+"Grp/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
}									    

string TGroup::BD()
{
    return m_bd+"."+owner().subId()+"_grp";
}

void TGroup::load( )
{
    SYS->db().at().dataGet(BD(),owner().nodePath()+"Grp/",*this);
}

void TGroup::save( )
{
    SYS->db().at().dataSet(BD(),owner().nodePath()+"Grp/",*this);
}

bool TGroup::user( const string &inm )
{
    if( owner().usrPresent(inm) )
    {
	if( owner().usrAt(inm).at().grp() == name() )	return true;
        int pos = 0;
	string val;
	while( (val=TSYS::strSepParse(m_usrs,pos++,';')).size() )
	    if(val==inm) return true;
    }
    return false;
}

void TGroup::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",Mess->I18Ns("Group ")+name());	
	ctrMkNode("area",opt,-1,"/prm",Mess->I18N("Group"));
	ctrMkNode("fld",opt,-1,"/prm/name",cfg("NAME").fld().descr(),0444,"root",owner().subId().c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/dscr",cfg("DESCR").fld().descr(),0664,"root",owner().subId().c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/bd",Mess->I18N("User group DB (module.db)"),0660,"root","root",1,"tp","str");
	ctrMkNode("list",opt,-1,"/prm/users",cfg("USERS").fld().descr(),0664,"root",owner().subId().c_str(),2,"tp","str","s_com","add,del");
	ctrMkNode("comm",opt,-1,"/prm/load",Mess->I18N("Load"),0440,"root",owner().subId().c_str());
	ctrMkNode("comm",opt,-1,"/prm/save",Mess->I18N("Save"),0440,"root",owner().subId().c_str());
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/bd" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->text(m_bd);
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	m_bd = opt->text();
    }	
    else if( a_path == "/prm/name" && ctrChkNode(opt,"get",0444,"root",owner().subId().c_str(),SEQ_RD) )	opt->text(name());
    else if( a_path == "/prm/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",owner().subId().c_str(),SEQ_RD) )	opt->text(lName());
	if( ctrChkNode(opt,"set",0664,"root",owner().subId().c_str(),SEQ_WR) )	lName(opt->text());
    }
    else if( a_path == "/prm/users" )
    {
	if( ctrChkNode(opt,"get",0664,"root",owner().subId().c_str(),SEQ_RD) )
	{
	    int pos = 0;
	    string val;
	    while( (val=TSYS::strSepParse(m_usrs,pos++,';')).size() )
		opt->childAdd("el")->text(val);
	}
	if( ctrChkNode(opt,"add",0664,"root",owner().subId().c_str(),SEQ_WR) )
	    m_usrs+=opt->text()+";";
	if( ctrChkNode(opt,"del",0664,"root",owner().subId().c_str(),SEQ_WR) )
	{
	    int pos = m_usrs.find(opt->text()+";",0);
	    if(pos != string::npos) m_usrs.erase(pos,opt->text().size()+1);
	}
    }	
    else if( a_path == "/prm/load" && ctrChkNode(opt,"set",0440,"root",owner().subId().c_str(),SEQ_RD) ) load();
    else if( a_path == "/prm/save" && ctrChkNode(opt,"set",0440,"root",owner().subId().c_str(),SEQ_RD) ) save();
}


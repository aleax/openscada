
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

#include <unistd.h>
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
    user_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::Key,"20") );
    user_el.fldAdd( new TFld("DESCR",_("Full name"),TFld::String,0,"50") );
    user_el.fldAdd( new TFld("PASS",_("Password"),TFld::String,0,"20") );
    user_el.fldAdd( new TFld("PICTURE",_("User picture"),TFld::String,0,"100000") );
    //Group BD structure
    grp_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::Key,"20") );
    grp_el.fldAdd( new TFld("DESCR",_("Full name"),TFld::String,0,"50") );
    grp_el.fldAdd( new TFld("USERS",_("Users"),TFld::String,0,"200") );
}

void TSecurity::postEnable(int flag)
{
    if(!(flag&TCntrNode::NodeRestore))
    {
	//- Add surely users, groups and set their parameters -
	//-- Administrator --
	usrAdd("root");
	usrAt("root").at().setLName(_("Administrator (superuser)!!!"));
	usrAt("root").at().setSysItem(true);
        usrAt("root").at().setPass("openscada");
	//-- Simple user --
	usrAdd("user");
	usrAt("user").at().setLName(_("Simple user."));
	usrAt("user").at().setSysItem(true);
	usrAt("user").at().setPass("user");
	//-- Administrators group --
	grpAdd("root");
	grpAt("root").at().setLName(_("Administrators group."));
	grpAt("root").at().setSysItem(true);
        grpAt("root").at().userAdd("root");
	//-- Simple users group --
        grpAdd("users");
        grpAt("users").at().setLName(_("Users group."));
        grpAt("users").at().setSysItem(true);
        grpAt("users").at().userAdd("user");
        grpAt("users").at().userAdd("root");
    }
    
    TSubSYS::postEnable(flag);
}

TSecurity::~TSecurity(  )
{
    nodeDelAll();
}

void TSecurity::usrGrpList( const string &name, vector<string> &list )
{
    vector<string> gls;
    list.clear();
    grpList(gls);
    for(int i_g = 0; i_g < gls.size(); i_g++)
	if( grpAt(gls[i_g]).at().user(name) )	
	    list.push_back(gls[i_g]);
}

void TSecurity::usrAdd( const string &name, const string &idb )
{    
    if( chldPresent(m_usr,name) )	return;
    chldAdd(m_usr,new TUser(name,idb,&user_el));
    if(grpPresent("users")) grpAt("users").at().userAdd(name);
}

void TSecurity::usrDel( const string &name, bool complete )       
{
    if(usrAt(name).at().sysItem())	throw TError(nodePath().c_str(),_("Removal of system user is inadmissible."));
    chldDel(m_usr,name,-1,complete); 
}

void TSecurity::grpAdd( const string &name, const string &idb )
{
    if( !chldPresent(m_grp,name) )	chldAdd(m_grp,new TGroup(name,idb,&grp_el));
}

void TSecurity::grpDel( const string &name, bool complete )
{
    if(grpAt(name).at().sysItem())	throw TError(nodePath().c_str(),_("Removal of system group is inadmissible.")); 
    chldDel(m_grp,name,-1,complete);
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
    if( grpAt(group).at().user(user) || grpAt("root").at().user(user) )
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
	g_cfg.cfgViewAll(false);
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
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_user",fld_cnt++,g_cfg) )
	{
	    name = g_cfg.cfg("NAME").getS();
	    if( !usrPresent(name) )	usrAdd(name,"*.*");
	    g_cfg.cfg("NAME").setS("");
	}
    }catch(TError err)
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new users error."));
    }
    
    //Search and create new user groups
    try
    {
	TConfig g_cfg(&grp_el);
	g_cfg.cfgViewAll(false);
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
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_grp",fld_cnt++,g_cfg) )
	{
	    name = g_cfg.cfg("NAME").getS();
	    if( !grpPresent(name) )	grpAdd(name,"*.*");
	    g_cfg.cfg("NAME").setS("");	
	}
    }catch(TError err)
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new user's groups error."));
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
    snprintf(buf,sizeof(buf),_(
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
	ctrMkNode("grp",opt,-1,"/br/usr_",_("User"),0444,"root","root",1,"list","/usgr/users");
	ctrMkNode("grp",opt,-1,"/br/grp_",_("Group"),0444,"root","root",1,"list","/usgr/grps");	
        if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),0440,"root",subId().c_str()))
	{
    	    ctrMkNode("comm",opt,-1,"/sub/load_db",_("Load"),0660,"root",subId().c_str());
    	    ctrMkNode("comm",opt,-1,"/sub/upd_db",_("Save"),0660,"root",subId().c_str());
	}
    	if(ctrMkNode("area",opt,1,"/usgr",_("Users and groups")))
	{
    	    ctrMkNode("list",opt,-1,"/usgr/users",_("Users"),0664,"root",subId().c_str(),3,"tp","br","s_com","add,del","br_pref","usr_");
    	    ctrMkNode("list",opt,-1,"/usgr/grps",_("Groups"),0664,"root",subId().c_str(),3,"tp","br","s_com","add,del","br_pref","grp_");
	}
    	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root",subId().c_str(),3,"tp","str","cols","90","rows","10");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root",subId().c_str()) )	opt->setText(optDescr());
    else if( a_path == "/usgr/users" )
    {
	if( ctrChkNode(opt,"get",0664,"root",subId().c_str(),SEQ_RD) )
	{
	    vector<string> list;   
    	    usrList(list);
    	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	        opt->childAdd("el")->setText(list[i_a]);
    	}
	if( ctrChkNode(opt,"add",0664,"root",subId().c_str(),SEQ_WR) )	usrAdd(opt->text());
	if( ctrChkNode(opt,"del",0664,"root",subId().c_str(),SEQ_WR) )	usrDel(opt->text(),true);
    }
    else if( a_path == "/usgr/grps" )
    {
	if( ctrChkNode(opt,"get",0664,"root",subId().c_str(),SEQ_RD) )
	{
	    vector<string> list;   
    	    grpList(list);
    	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	        opt->childAdd("el")->setText(list[i_a]);
    	}
	if( ctrChkNode(opt,"add",0664,"root",subId().c_str(),SEQ_WR) )	grpAdd(opt->text());
	if( ctrChkNode(opt,"del",0664,"root",subId().c_str(),SEQ_WR) )	grpDel(opt->text(),true);	
    }
    else if( a_path == "/sub/load_db" && ctrChkNode(opt,"set",0660,"root",subId().c_str(),SEQ_WR) )	subLoad();
    else if( a_path == "/sub/upd_db" && ctrChkNode(opt,"set",0660,"root",subId().c_str(),SEQ_WR) )     	subSave();
    else TSubSYS::cntrCmdProc(opt);
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
    
TUser::TUser( const string &nm, const string &idb, TElem *el ) : 
    TConfig(el), m_db(idb), m_lname(cfg("DESCR").getSd()), m_pass(cfg("PASS").getSd()), 
    m_name(cfg("NAME").getSd()), m_pict(cfg("PICTURE").getSd()), m_sysIt(false)
{
    m_name = nm;
}

TUser::~TUser(  )
{

}

void TUser::setPass( const string &n_pass )
{ 
    m_pass = crypt(n_pass.c_str(),name().c_str()); 
}

bool TUser::auth( const string &ipass )
{ 
    return (m_pass == crypt(ipass.c_str(),name().c_str()))?true:false; 
}

void TUser::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this);
	//Remove user from groups
	vector<string> gls;
	owner().usrGrpList(name(),gls);
	for(int i_g = 0; i_g < gls.size(); i_g++)
	    owner().grpAt(gls[i_g]).at().userDel(name());    
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TUser::tbl( )
{
    return owner().subId()+"_user";
}

void TUser::load( )
{
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TUser::save( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TUser::cntrCmdProc( XMLNode *opt )
{
    string grp = owner().subId();
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("User ")+name());
	if(picture().size()) ctrMkNode("img",opt,-1,"/ico","",0444);
	if(ctrMkNode("area",opt,-1,"/prm",_("User")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/name",cfg("NAME").fld().descr(),0444,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/dscr",cfg("DESCR").fld().descr(),0664,name().c_str(),grp.c_str(),1,"tp","str");
	    ctrMkNode("img",opt,-1,"/prm/pct",cfg("PICTURE").fld().descr(),0664,name().c_str(),grp.c_str(),1,"v_sz","100");
	    ctrMkNode("fld",opt,-1,"/prm/db",_("User DB (module.db)"),0664,"root",SYS->db().at().subId().c_str(),1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/pass",cfg("PASS").fld().descr(),0660,name().c_str(),grp.c_str(),1,"tp","str");
	    ctrMkNode("table",opt,-1,"/prm/grps",_("Groups"),0444,"root",grp.c_str(),1,"key","grp");
	    ctrMkNode("list",opt,-1,"/prm/grps/grp",_("Group"),0444,"root",grp.c_str(),1,"tp","str");
    	    ctrMkNode("list",opt,-1,"/prm/grps/vl",_("Include"),0444,"root",grp.c_str(),1,"tp","bool");
	    ctrMkNode("comm",opt,-1,"/prm/load",_("Load"),0660,name().c_str(),grp.c_str());
	    ctrMkNode("comm",opt,-1,"/prm/save",_("Save"),0660,name().c_str(),grp.c_str());
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root",SYS->db().at().subId().c_str(),SEQ_RD) )	opt->setText(m_db);
	if( ctrChkNode(opt,"set",0664,"root",SYS->db().at().subId().c_str(),SEQ_WR) )	m_db = opt->text();
    }
    else if( a_path == "/prm/name" && ctrChkNode(opt) )	opt->setText(name());
    else if( a_path == "/prm/pct" || a_path == "/ico" )
    {
	if( ctrChkNode(opt,"get",0664,name().c_str(),owner().subId().c_str(),SEQ_RD) )	opt->setText(picture());
	if( ctrChkNode(opt,"set",0664,name().c_str(),owner().subId().c_str(),SEQ_WR) )	setPicture(opt->text());
    }
    else if( a_path == "/prm/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,name().c_str(),owner().subId().c_str(),SEQ_RD) )	opt->setText(lName());
	if( ctrChkNode(opt,"set",0664,name().c_str(),owner().subId().c_str(),SEQ_WR) )	setLName(opt->text());
    }
    else if( a_path == "/prm/pass" )
    {
	if( ctrChkNode(opt,"get",0660,name().c_str(),owner().subId().c_str(),SEQ_RD) )	opt->setText("**********");
	if( ctrChkNode(opt,"set",0660,name().c_str(),owner().subId().c_str(),SEQ_WR) )	setPass(opt->text());
    }
    else if( a_path == "/prm/grps" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )
	{
	    //Prepare headers
	    XMLNode *grp = ctrMkNode("list",opt,-1,"/prm/grps/grp","",0440);
	    XMLNode *vl  = ctrMkNode("list",opt,-1,"/prm/grps/vl","",0660);
	    vector<string> ls;
	    owner().grpList(ls);
	    for(int i_g = 0; i_g < ls.size(); i_g++)
	    {
		if(grp)	grp->childAdd("el")->setText(ls[i_g]);
		if(vl)	vl->childAdd("el")->setText(TSYS::int2str(owner().grpAt(ls[i_g]).at().user(name())));
	    }	    
	}
	/*if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )
	{
	    if(atoi(opt->text().c_str()))
		owner().grpAt(opt->attr("key_grp")).at().userAdd(name());
	    else owner().grpAt(opt->attr("key_grp")).at().userDel(name());
	}*/   
    }
    else if( a_path == "/prm/load" && ctrChkNode(opt,"set",0660,name().c_str(),owner().subId().c_str(),SEQ_WR) ) 
	load();
    else if( a_path == "/prm/save" && ctrChkNode(opt,"set",0660,name().c_str(),owner().subId().c_str(),SEQ_WR) ) 
	save();
}	    

//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
TGroup::TGroup( const string &nm, const string &idb, TElem *el ) : 
    TConfig(el), m_db(idb), m_lname(cfg("DESCR").getSd()), m_usrs(cfg("USERS").getSd()), 
    m_name(cfg("NAME").getSd()), m_sysIt(false)
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
	    SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}									    

string TGroup::tbl( )
{
    return owner().subId()+"_grp";
}

void TGroup::load( )
{
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TGroup::save( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

bool TGroup::user( const string &inm )
{
    if( owner().usrPresent(inm) )
    {
        int pos = 0;
	string val;
	while( (val=TSYS::strSepParse(m_usrs,pos++,';')).size() )
	    if(val==inm) return true;
    }
    return false;
}

void TGroup::userAdd( const string &name )
{
    if(!user(name)) m_usrs+=name+";";
}

void TGroup::userDel( const string &name )
{
    int pos = m_usrs.find(name+";",0);
    if(pos != string::npos) m_usrs.erase(pos,name.size()+1);
}

void TGroup::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Group ")+name());	
	ctrMkNode("area",opt,-1,"/prm",_("Group"));
	ctrMkNode("fld",opt,-1,"/prm/name",cfg("NAME").fld().descr(),0444,"root",owner().subId().c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/dscr",cfg("DESCR").fld().descr(),0664,"root",owner().subId().c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/db",_("User group DB (module.db)"),0660,"root",SYS->db().at().subId().c_str(),1,"tp","str");
	ctrMkNode("list",opt,-1,"/prm/users",cfg("USERS").fld().descr(),0664,"root",owner().subId().c_str(),2,"tp","str","s_com","add,del");
	ctrMkNode("comm",opt,-1,"/prm/load",_("Load"),0660,"root",owner().subId().c_str());
	ctrMkNode("comm",opt,-1,"/prm/save",_("Save"),0660,"root",owner().subId().c_str());
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/db" )
    {
	if( ctrChkNode(opt,"get",0660,"root",SYS->db().at().subId().c_str(),SEQ_RD) )	opt->setText(m_db);
	if( ctrChkNode(opt,"set",0660,"root",SYS->db().at().subId().c_str(),SEQ_WR) )	m_db = opt->text();
    }	
    else if( a_path == "/prm/name" && ctrChkNode(opt,"get",0444,"root",owner().subId().c_str(),SEQ_RD) )	
	opt->setText(name());
    else if( a_path == "/prm/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",owner().subId().c_str(),SEQ_RD) )	opt->setText(lName());
	if( ctrChkNode(opt,"set",0664,"root",owner().subId().c_str(),SEQ_WR) )	setLName(opt->text());
    }
    else if( a_path == "/prm/users" )
    {
	if( ctrChkNode(opt,"get",0664,"root",owner().subId().c_str(),SEQ_RD) )
	{
	    int pos = 0;
	    string val;
	    while( (val=TSYS::strSepParse(m_usrs,pos++,';')).size() )
		opt->childAdd("el")->setText(val);
	}
	if( ctrChkNode(opt,"add",0664,"root",owner().subId().c_str(),SEQ_WR) )	userAdd(opt->text());
	if( ctrChkNode(opt,"del",0664,"root",owner().subId().c_str(),SEQ_WR) )	userDel(opt->text());
    }	
    else if( a_path == "/prm/load" && ctrChkNode(opt,"set",0660,"root",owner().subId().c_str(),SEQ_WR) ) load();
    else if( a_path == "/prm/save" && ctrChkNode(opt,"set",0660,"root",owner().subId().c_str(),SEQ_WR) ) save();
}


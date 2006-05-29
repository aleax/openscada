
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
    user_el.fldAdd( new TFld("ID",Mess->I18N("Identificator"),TFld::Dec,0,"3") );
    user_el.fldAdd( new TFld("PASS",Mess->I18N("Password"),TFld::String,0,"20") );
    user_el.fldAdd( new TFld("GRP",Mess->I18N("Default group"),TFld::String,0,"20") );
    //Group BD structure
    grp_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,FLD_KEY,"20") );
    grp_el.fldAdd( new TFld("DESCR",Mess->I18N("Full name"),TFld::String,0,"50") );
    grp_el.fldAdd( new TFld("ID",Mess->I18N("Identificator"),TFld::Dec,0,"3") );
    grp_el.fldAdd( new TFld("USERS",Mess->I18N("Users"),TFld::String,0,"50") );					    
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

int TSecurity::usrAdd( const string &name, const string &idb )
{    
    if( chldPresent(m_usr,name) ) return usr(name);
    int uid = usr_id_f();
    chldAdd(m_usr,new TUser(name,idb,uid,&user_el));
    
    return uid;
}

int TSecurity::grpAdd( const string &name, const string &idb )
{
    if( chldPresent(m_grp,name) ) return grp(name);
    int gid = grp_id_f();
    chldAdd(m_grp,new TGroup(name,idb,gid,&grp_el));
    
    return gid;
}

unsigned TSecurity::usr_id_f()
{
    unsigned id = 0;
    vector<string> list;
    usrList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( usrAt(list[i_l]).at().id() == id ){ id++; i_l=-1; }
    return id;
}

unsigned TSecurity::grp_id_f()
{
    unsigned id = 0;
    vector<string> list;
    grpList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( grpAt(list[i_l]).at().id() == id ){ id++; i_l=-1; }
    return id;
}

string TSecurity::usr( int id )
{
    vector<string> list;
    
    usrList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( usrAt(list[i_l]).at().id() == id ) return(list[i_l]);
    return "";
}

int TSecurity::usr( const string &sid )
{
    return usrAt(sid).at().id();
}

int TSecurity::grp( const string &sid )
{
    return grpAt(sid).at().id();
}    

string TSecurity::grp( int id )
{
    vector<string> list;
    
    grpList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( grpAt(list[i_l]).at().id() == id ) return(list[i_l]);
    return "";
}

bool TSecurity::access( const string &user, char mode, int owner, int group, int access )
{
    bool rez = false;

    try
    {
    	AutoHD<TUser> r_usr = usrAt(user);
	// Check owner permision
	if( r_usr.at().id() == 0 || r_usr.at().id() == owner )
	    if( ((mode&SEQ_RD)?access&0400:true) && 
		((mode&SEQ_WR)?access&0200:true) && 
		((mode&SEQ_XT)?access&0100:true) )
	    rez = true; 
	// Check other permision
	if( !rez && ((mode&SEQ_RD)?access&0004:true) && 
	    ((mode&SEQ_WR)?access&0002:true) && 
	    ((mode&SEQ_XT)?access&0001:true) )
	    rez = true; 	
	// Check groupe permision
	if( !rez )
	{
	    string n_grp = grp(group);
	    if( n_grp.size() )
	    {
		if( (n_grp == r_usr.at().grp() || grpAt(n_grp).at().user(user) || grpAt("root").at().user(user)) &&
		    ((mode&SEQ_RD)?access&0040:true) && 
		    ((mode&SEQ_WR)?access&0020:true) && 
		    ((mode&SEQ_XT)?access&0010:true) )
		    rez = true;
	    }
	}	
    }catch(...){  }

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
	
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
        {
    	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
            for( int i_db = 0; i_db < db_ls.size(); i_db++ )
            {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
                int fld_cnt=0;	
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_user",nodePath()+"User/",fld_cnt++,g_cfg) )
		{
		    name = g_cfg.cfg("NAME").getS();
		    if( !usrPresent(name) )	usrAdd(name,(wbd==SYS->workDB())?"*.*":wbd);
		    g_cfg.cfg("NAME").setS("");
		}
	    }
	}
    }catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    
    //Search and create new user groups
    try
    {
	TConfig g_cfg(&grp_el);
	vector<string> tdb_ls, db_ls;
	
        SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
        {
    	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
            for( int i_db = 0; i_db < db_ls.size(); i_db++ )
            {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
                int fld_cnt=0;		
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_grp",nodePath()+"Grp/",fld_cnt++,g_cfg) )
		{
		    name = g_cfg.cfg("NAME").getS();
		    if( !grpPresent(name) )	grpAdd(name,(wbd==SYS->workDB())?"*.*":wbd);
		    g_cfg.cfg("NAME").setS("");	
		}
	    }
	}
    }catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    
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


//================== Controll functions ========================
void TSecurity::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    int bd_gr, my_gr;
    vector<string> list;
    
    switch(cmd)
    {
	case TCntrNode::Info:
	    bd_gr = SYS->db().at().subSecGrp();
	    my_gr = subSecGrp();	    
	    TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent
	    
    	    ctrMkNode("area",opt,0,a_path.c_str(),"/bd",Mess->I18N("Subsystem"),0440,0,my_gr);
    	    ctrMkNode("comm",opt,-1,a_path.c_str(),"/bd/load_bd",Mess->I18N("Load"),0440,0,my_gr);
    	    ctrMkNode("comm",opt,-1,a_path.c_str(),"/bd/upd_bd",Mess->I18N("Save"),0440,0,my_gr);
    	    ctrMkNode("area",opt,1,a_path.c_str(),"/usgr",Mess->I18N("Users and groups"));
    	    ctrMkNode("list",opt,-1,a_path.c_str(),"/usgr/users",Mess->I18N("Users"),0664,0,my_gr,3,"tp","br","s_com","add,del","br_pref","usr_");
    	    ctrMkNode("list",opt,-1,a_path.c_str(),"/usgr/grps",Mess->I18N("Groups"),0664,0,my_gr,2,"tp","br","br_pref","grp_");
    	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,my_gr,3,"tp","str","cols","90","rows","5");
	    break;
	case TCntrNode::Get:
    	    if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
    	    else if( a_path == "/usgr/users" )
    	    {
    		usrList(list);
    		opt->childClean();
    		for( unsigned i_a=0; i_a < list.size(); i_a++ )
    		    ctrSetS( opt, list[i_a] ); 	
    	    }
    	    else if( a_path == "/usgr/grps" )
    	    {
    		grpList(list);
    		opt->childClean();
    		for( unsigned i_a=0; i_a < list.size(); i_a++ )
    		    ctrSetS( opt, list[i_a] ); 	
    	    }    
	    else TSubSYS::cntrCmd_( a_path, opt, cmd );
	    break;
	case TCntrNode::Set:
	    if( a_path == "/usgr/users" )
	    {
		if( opt->name() == "add" )     	usrAdd(opt->text());
		else if(opt->name() == "del")	chldDel(m_usr,opt->text(),-1,1);
	    }
	    else if( a_path == "/bd/load_bd" )	subLoad();
	    else if( a_path == "/bd/upd_bd" )	subSave();
	    else if( a_path == "/usgr/grps" )
	    {
		if( opt->name() == "add" )     	grpAdd(opt->text());
		else if(opt->name() == "del")	chldDel(m_grp,opt->text(),-1,1);
	    }
	    else TSubSYS::cntrCmd_( a_path, opt, cmd );
	    break;
    }
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
    
TUser::TUser( const string &nm, const string &idb, unsigned id, TElem *el ) : 
    TConfig(el), m_bd(idb), m_lname(cfg("DESCR").getSd()), m_pass(cfg("PASS").getSd()), 
    m_name(cfg("NAME").getSd()), m_id(cfg("ID").getId()), m_grp(cfg("GRP").getSd())
{
    m_name = nm;
    m_id = id;
}

TUser::~TUser(  )
{

}

void TUser::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(BD(),owner().nodePath()+"User/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
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

void TUser::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
	int my_gr = owner().subSecGrp();
	
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18Ns("User ")+name());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("User"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/name",cfg("NAME").fld().descr(),0664,m_id,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/dscr",cfg("DESCR").fld().descr(),0664,m_id,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/bd",Mess->I18N("User DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/grp",cfg("GRP").fld().descr(),0664,0,my_gr,3,"tp","str","dest","select","select","/prm/grps");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/id",cfg("ID").fld().descr(),0444,0,my_gr,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/pass",cfg("PASS").fld().descr(),0660,m_id,my_gr,1,"tp","str");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/load",Mess->I18N("Load"),0440,0,my_gr);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/save",Mess->I18N("Save"),0440,0,my_gr);
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/bd" ) 		ctrSetS( opt, m_bd );
	else if( a_path == "/prm/name" )	ctrSetS( opt, name() );
	else if( a_path == "/prm/dscr" )	ctrSetS( opt, lName() );
	else if( a_path == "/prm/grp" )		ctrSetS( opt, grp() );
	else if( a_path == "/prm/id" )		ctrSetI( opt, id() );
	else if( a_path == "/prm/pass" )	ctrSetS( opt, "**********" );
	else if( a_path == "/prm/grps" )  
	{
	    owner().grpList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/bd" )      		m_bd = ctrGetS(opt);
	else if( a_path == "/prm/name" )	name( ctrGetS( opt ) );
	else if( a_path == "/prm/dscr" )	lName( ctrGetS( opt ) );
	else if( a_path == "/prm/grp" )		grp( ctrGetS( opt ) );
	else if( a_path == "/prm/id" ) 		id( ctrGetI( opt ) );
	else if( a_path == "/prm/pass" )	pass( ctrGetS( opt ) );
	else if( a_path == "/prm/load" )	load();
	else if( a_path == "/prm/save" )	save();	
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}

//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
    
TGroup::TGroup( const string &nm, const string &idb, unsigned id, TElem *el ) : 
    TConfig(el), m_bd(idb), m_lname(cfg("DESCR").getSd()), m_usrs(cfg("USERS").getSd()), 
    m_name(cfg("NAME").getSd()), m_id(cfg("ID").getId())
{
    m_name = nm;
    m_id = id;    
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
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
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
    if( owner().usrAt(inm).at().grp() == name() || m_usrs.find(inm,0) != string::npos ) 
	return true;
    return false;
}

void TGroup::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
	int my_gr = owner().subSecGrp();
	
	TCntrNode::cntrCmd_(a_path,opt,cmd);
	
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18Ns("Group ")+name());	
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("Group"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/name",cfg("NAME").fld().descr(),0664,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/dscr",cfg("DESCR").fld().descr(),0664,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/bd",Mess->I18N("User group DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/id",cfg("ID").fld().descr(),0444,0,my_gr,1,"tp","dec");
	ctrMkNode("list",opt,-1,a_path.c_str(),"/prm/users",cfg("USERS").fld().descr(),0664,0,my_gr,4,"tp","str","s_com","add,del","dest","select","select","/prm/usrs");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/load",Mess->I18N("Load"),0440,0,my_gr);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/save",Mess->I18N("Save"),0440,0,my_gr);
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/bd" ) 		ctrSetS( opt, m_bd );
	else if( a_path == "/prm/name" )	ctrSetS( opt, name() );
	else if( a_path == "/prm/dscr" )	ctrSetS( opt, lName() );
	else if( a_path == "/prm/id" )    	ctrSetI( opt, id() );
	else if( a_path == "/prm/users" )
	{
	    int pos = 0,c_pos;
	    opt->childClean();
	    do
	    {
		c_pos = m_usrs.find(";",pos);
		string val = m_usrs.substr(pos,c_pos-pos);
		if( val.size() ) ctrSetS( opt, val );
		pos = c_pos+1;
	    }while(c_pos != string::npos);
	}
	else if( a_path == "/prm/usrs" )  
	{
	    vector<string> list;
	    owner().usrList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/bd" )      		m_bd = ctrGetS(opt);    
	else if( a_path == "/prm/name" )       	name(ctrGetS( opt ));
	else if( a_path == "/prm/dscr" )  	lName(ctrGetS( opt ));
	else if( a_path == "/prm/id" )    	id(ctrGetI( opt ));
	else if( a_path == "/prm/users" )
	{
	    if( opt->name() == "add" )
	    {
		if(m_usrs.size()) m_usrs=m_usrs+";";
		m_usrs=m_usrs+opt->text();
	    }
	    if( opt->name() == "del" )
	    {
		int pos = m_usrs.find(string(";")+opt->text(),0);
		if(pos != string::npos) 
		    m_usrs.erase(pos,opt->text().size()+1);
		else                    
		    m_usrs.erase(m_usrs.find(opt->text(),0),opt->text().size()+1);
	    }
	}
	else if( a_path == "/prm/load" )	load();
	else if( a_path == "/prm/save" )	save();	
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}

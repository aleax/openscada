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
#include "tsequrity.h"

const char *TSequrity::o_name = "TSequrity";
const char *TSequrity::s_name = "Sequrity";

TSequrity::TSequrity( TKernel *app ) : 
    m_owner(app), m_bd_usr("", "", "seq_usr"), m_bd_grp("", "", "seq_grp")
{
    m_usr = TCntrNode::grpAdd();
    m_grp = TCntrNode::grpAdd();
    nodeEn();
    
    //User BD structure
    user_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),T_STRING|F_KEY,"20") );
    user_el.fldAdd( new TFld("DESCR",Mess->I18N("Full name"),T_STRING,"50") );
    user_el.fldAdd( new TFld("ID",Mess->I18N("Identificator"),T_DEC,"3") );
    user_el.fldAdd( new TFld("PASS",Mess->I18N("Password"),T_STRING,"20") );
    user_el.fldAdd( new TFld("GRP",Mess->I18N("Default group"),T_STRING,"20") );
    //Group BD structure
    grp_el.fldAdd( new TFld("NAME",Mess->I18N("Name"),T_STRING|F_KEY,"20") );
    grp_el.fldAdd( new TFld("DESCR",Mess->I18N("Full name"),T_STRING,"50") );
    grp_el.fldAdd( new TFld("ID",Mess->I18N("Identificator"),T_DEC,"3") );
    grp_el.fldAdd( new TFld("USERS",Mess->I18N("Users"),T_STRING,"50") );
        
    //Add surely users, groups and set parameters
    usrAdd("root");
    usrAt("root").at().lName("Administrator (superuser)!!!"); 
    usrAt("root").at().pass("openscada"); 
    
    grpAdd("root");
    grpAt("root").at().lName("Administrators group.");     
}

TSequrity::~TSequrity(  )
{
    delAll();
}
	
string TSequrity::name()
{
    return(Mess->I18N((char *)s_name)); 
}

TBDS::SName TSequrity::userBD()
{ 
    return owner().nameDBPrep(m_bd_usr); 
}

TBDS::SName TSequrity::grpBD() 
{ 
    return owner().nameDBPrep(m_bd_grp);
}	

void TSequrity::usrAdd( const string &name )
{    
    if( chldAvoid(m_usr,name) ) return;
    chldAdd(m_usr,new TUser(this,name,usr_id_f(),&user_el)); 
}

void TSequrity::grpAdd( const string &name )
{
    if( chldAvoid(m_grp,name) ) return;
    chldAdd(m_grp,new TGroup(this,name,grp_id_f(),&grp_el)); 
}

unsigned TSequrity::usr_id_f()
{
    unsigned id = 0;
    vector<string> list;
    usrList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( usrAt(list[i_l]).at().id() == id ){ id++; i_l=-1; }
    return(id);
}

unsigned TSequrity::grp_id_f()
{
    unsigned id = 0;
    vector<string> list;
    grpList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( grpAt(list[i_l]).at().id() == id ){ id++; i_l=-1; }
    return(id);
}

string TSequrity::usr( int id )
{
    vector<string> list;
    
    usrList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( usrAt(list[i_l]).at().id() == id ) return(list[i_l]);
    return("");
}

string TSequrity::grp( int id )
{
    vector<string> list;
    
    grpList(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( grpAt(list[i_l]).at().id() == id ) return(list[i_l]);
    return("");
}

bool TSequrity::access( const string &user, char mode, int owner, int group, int access )
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
		if( (n_grp == r_usr.at().grp() || grpAt(n_grp).at().user(user)) &&
		    ((mode&SEQ_RD)?access&0040:true) && 
		    ((mode&SEQ_WR)?access&0020:true) && 
		    ((mode&SEQ_XT)?access&0010:true) )
		    rez = true;
	    }
	}	
    }catch(...){  }

    return(rez);
}

XMLNode *TSequrity::cfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = owner().cfgNode()->childGet("section",i_k++);
	if( t_n->attr("id") == s_name ) return( t_n );
    }
}

void TSequrity::init( )
{
    loadBD();
}

string TSequrity::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"======================= The Sequrity subsystem options =====================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"UserBD  <fullname>  User bd, recorded:  \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	"GrpBD   <fullname>  Group bd, recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"),s_name);
    
    return(buf);
}

void TSequrity::checkCommandLine(  )
{
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s:Read commandline options!",s_name);
#endif
	
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
    
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_DEBUG,"%s:Read commandline options ok!",s_name);
#endif
}

void TSequrity::updateOpt()
{
    string opt;

    try
    {
    	opt = cfgNode()->childGet("id","UserBD")->text(); 
        m_bd_usr.tp  = TSYS::strSepParse(opt,0,':');
        m_bd_usr.bd  = TSYS::strSepParse(opt,1,':');
	m_bd_usr.tbl = TSYS::strSepParse(opt,2,':');
    }
    catch(...) {  }    
    
    try
    {
    	opt = cfgNode()->childGet("id","GrpBD")->text(); 
        m_bd_grp.tp  = TSYS::strSepParse(opt,0,':');
        m_bd_grp.bd  = TSYS::strSepParse(opt,1,':');
	m_bd_grp.tbl = TSYS::strSepParse(opt,2,':');
    }
    catch(...) {  }    
}

void TSequrity::loadBD( )
{
    int 	fld_cnt;
    string 	name;
    AutoHD<TTable> tbl;    
    
    // Load users from bd
    try
    {
	TConfig g_cfg(&user_el);
        fld_cnt=0;
	AutoHD<TTable> tbl = owner().BD().open(userBD());
        while( tbl.at().fieldSeek(fld_cnt++,g_cfg) )
	{
	    name = g_cfg.cfg("NAME").getS();
	    if( !usrAvoid(name) )
	    {
		usrAdd(name);
		((TConfig &)usrAt(name).at()) = g_cfg;
	    }
            else usrAt(name).at().load();
	}
	tbl.free();
	owner().BD().close(userBD());   
    }catch(...){}
    
    // Load groups from bd
    try
    {
	TConfig g_cfg(&grp_el);
        fld_cnt=0;
	AutoHD<TTable> tbl = owner().BD().open(grpBD());
        while( tbl.at().fieldSeek(fld_cnt++,g_cfg) )
	{
	    name = g_cfg.cfg("NAME").getS();
	    if( !grpAvoid(name) )
	    { 
		grpAdd(name);
		((TConfig &)grpAt(name).at()) = g_cfg;
	    }
            else grpAt(name).at().load();	
	}
	tbl.free();
	owner().BD().close(grpBD());
    }catch(...){}
}

void TSequrity::saveBD( )
{
    vector<string> list;
    
    // Save users to bd
    usrList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
	usrAt(list[i_l]).at().save();
    
    // Save groups to bd
    for( int i_l = 0; i_l < list.size(); i_l++ )
	grpAt(list[i_l]).at().save();
}

//================== Controll functions ========================
void TSequrity::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='bd' acs='0440'>"
	  "<fld id='u_t_bd' acs='0660' tp='str' dest='select' select='/bd/b_mod'/>"
	  "<fld id='u_bd' acs='0660' tp='str'/>"
	  "<fld id='u_tbl' acs='0660' tp='str'/>"
	  "<fld id='g_t_bd' acs='0660' tp='str' dest='select' select='/bd/b_mod'/>"
	  "<fld id='g_bd' acs='0660' tp='str'/>"
	  "<fld id='g_tbl' acs='0660' tp='str'/>"
	  "<comm id='load_bd'/>"
	  "<comm id='upd_bd'/>"
	 "</area>"    
	 "<area id='usgr'>"
	  "<list id='users' s_com='add,del' tp='br' mode='att' br_pref='_usr_'/>"
	  "<list id='grps' s_com='add,del' tp='br' mode='att' br_pref='_grp_'/>"
	 "</area>"
	 "<area id='help'>"
	  "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
	 "</area>"
	"</oscada_cntr>";
    char *dscr = "dscr";
    
    inf->load( i_cntr );
    inf->text(Mess->I18N("Sequrity subsystem"));
    //bd
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Subsystem"));
    if( owner().genDB( ) )
    {
        c_nd->childGet(0)->attr("acs","0");
        c_nd->childGet(1)->attr("acs","0");
        c_nd->childGet(2)->attr(dscr,Mess->I18N("User table"));
        c_nd->childGet(3)->attr("acs","0");
        c_nd->childGet(4)->attr("acs","0");
        c_nd->childGet(5)->attr(dscr,Mess->I18N("Group table"));
    }
    else
    {   
	c_nd->childGet(0)->attr(dscr,Mess->I18N("User BD (module:bd:table)"));
	c_nd->childGet(3)->attr(dscr,Mess->I18N("Group BD (module:bd:table)"));
    }
    c_nd->childGet(6)->attr(dscr,Mess->I18N("Load from BD"));
    c_nd->childGet(7)->attr(dscr,Mess->I18N("Save to BD"));
    //usgr
    c_nd = inf->childGet(1);
    c_nd->attr(dscr,Mess->I18N("Users and groups"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Users"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Groups"));    
    //help
    c_nd = inf->childGet(2);
    c_nd->attr(dscr,Mess->I18N("Help"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Options help"));
}

void TSequrity::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    vector<string> list;
    
    if( a_path == "/bd/u_t_bd" )     ctrSetS( opt, m_bd_usr.tp );
    else if( a_path == "/bd/u_bd" )  ctrSetS( opt, m_bd_usr.bd );
    else if( a_path == "/bd/u_tbl" ) ctrSetS( opt, m_bd_usr.tbl );
    else if( a_path == "/bd/g_t_bd" )ctrSetS( opt, m_bd_grp.tp );
    else if( a_path == "/bd/g_bd" )  ctrSetS( opt, m_bd_grp.bd );
    else if( a_path == "/bd/g_tbl" ) ctrSetS( opt, m_bd_grp.tbl );
    else if( a_path == "/bd/b_mod" )
    {
	owner().BD().gmdList(list);
	opt->childClean();
	ctrSetS( opt, "" );
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );
    }
    else if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
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
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TSequrity::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/bd/u_t_bd" )     	m_bd_usr.tp  = ctrGetS( opt );
    else if( a_path == "/bd/u_bd" )  	m_bd_usr.bd  = ctrGetS( opt );
    else if( a_path == "/bd/u_tbl" ) 	m_bd_usr.tbl = ctrGetS( opt );
    else if( a_path == "/bd/g_t_bd" )	m_bd_grp.tp  = ctrGetS( opt );
    else if( a_path == "/bd/g_bd" )  	m_bd_grp.bd  = ctrGetS( opt );
    else if( a_path == "/bd/g_tbl" )	m_bd_grp.tbl = ctrGetS( opt );
    else if( a_path.substr(0,11) == "/usgr/users" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      	usrAdd(t_c->text());
		else if(t_c->attr("do") == "del")	chldDel(m_usr,t_c->text(),-1,1);
	    }
	}
    else if( a_path == "/bd/load_bd" )	loadBD();
    else if( a_path == "/bd/upd_bd" )	saveBD();
    else if( a_path.substr(0,10) == "/usgr/grps" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      	grpAdd(t_c->text());
		else if(t_c->attr("do") == "del")	chldDel(m_grp,t_c->text(),-1,1);
	    }
	}
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

AutoHD<TCntrNode> TSequrity::ctrAt1( const string &br )
{
    if( br.substr(0,5) == "_usr_" )		return usrAt(br.substr(5));
    else if( br.substr(0,5) == "_grp_" ) 	return grpAt(br.substr(5));
    else throw TError("<{%s}> Branch %s error!",__func__,br.c_str());
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
    
TUser::TUser( TSequrity *owner, const string &nm, unsigned id, TElem *el ) : 
    m_owner(owner), TConfig(el),
    m_lname(cfg("DESCR").getS()), m_pass(cfg("PASS").getS()), m_name(cfg("NAME").getS()), 
    m_id(cfg("ID").getI()), m_grp(cfg("GRP").getS())
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
        {
            TBDS &bds = owner().owner().BD();
            bds.open(owner().userBD()).at().fieldDel(*this);
            bds.close(owner().userBD());
        }
    }catch(TError err)
    { owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}
									    

void TUser::load( )
{
    TBDS &bds  = owner().owner().BD();
    bds.open( owner().userBD() ).at().fieldGet(*this);
    bds.close(owner().userBD());
}

void TUser::save( )
{
    TBDS &bds  = owner().owner().BD();
    bds.open( owner().userBD(), true ).at().fieldSet(*this);
    bds.close(owner().userBD());
}
//==============================================================
//================== Controll functions ========================
//==============================================================
void TUser::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='prm'>"
	  "<fld id='name' acs='0644' tp='str'/>"
	  "<fld id='dscr' acs='0644' tp='str'/>"
	  "<fld id='grp' acs='0644' tp='str' dest='select' select='/prm/grps'/>"
	  "<fld id='id' acs='0644' tp='dec'/>"
	  "<fld id='pass' acs='0600' tp='str'/>"
	  "<comm id='load' acs='0550'/>"
	  "<comm id='save' acs='0550'/>"    
	 "</area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load( i_cntr );
    inf->text(Mess->I18Ns("User ")+name());
    //prm
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("User"));
    c_nd->childGet(0)->attr(dscr,cfg("NAME").fld().descr());
    c_nd->childGet(0)->attr("own",TSYS::int2str(m_id));
    c_nd->childGet(1)->attr(dscr,cfg("DESCR").fld().descr());
    c_nd->childGet(1)->attr("own",TSYS::int2str(m_id));
    c_nd->childGet(2)->attr(dscr,cfg("GRP").fld().descr());
    c_nd->childGet(3)->attr(dscr,cfg("ID").fld().descr());
    c_nd->childGet(4)->attr(dscr,cfg("PASS").fld().descr());
    c_nd->childGet(4)->attr("own",TSYS::int2str(m_id));
    c_nd->childGet(5)->attr(dscr,Mess->I18N("Load from BD"));
    c_nd->childGet(6)->attr(dscr,Mess->I18N("Save to BD"));
}

void TUser::ctrDinGet_( const string &a_path, XMLNode *opt )
{        
    if( a_path == "/prm/name" )       ctrSetS( opt, name() );
    else if( a_path == "/prm/dscr" )  ctrSetS( opt, lName() );
    else if( a_path == "/prm/grp" )   ctrSetS( opt, grp() );
    else if( a_path == "/prm/id" )    ctrSetI( opt, id() );
    else if( a_path == "/prm/pass" )  ctrSetS( opt, "**********" );
    else if( a_path == "/prm/grps" )  
    {
	vector<string> list;
	owner().grpList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );
    }
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

void TUser::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )       	name( ctrGetS( opt ) );
    else if( a_path == "/prm/dscr" )  	lName( ctrGetS( opt ) );
    else if( a_path == "/prm/grp" )   	grp( ctrGetS( opt ) );
    else if( a_path == "/prm/id" )    	id( ctrGetI( opt ) );
    else if( a_path == "/prm/pass" )  	pass( ctrGetS( opt ) );
    else if( a_path == "/prm/load" )	load();
    else if( a_path == "/prm/save" ) 	save();	
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
    
TGroup::TGroup( TSequrity *owner, const string &nm, unsigned id, TElem *el ) : 
    m_owner(owner), TConfig(el),
    m_lname(cfg("DESCR").getS()), m_usrs(cfg("USERS").getS()), m_name(cfg("NAME").getS()), m_id(cfg("ID").getI())
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
        {
            TBDS &bds = owner().owner().BD();
            bds.open(owner().grpBD()).at().fieldDel(*this);
            bds.close(owner().grpBD());
        }
    }catch(TError err)
    { owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}									    

void TGroup::load( )
{
    TBDS &bds  = owner().owner().BD();
    bds.open( owner().grpBD() ).at().fieldGet(*this);
    bds.close(owner().grpBD());
}

void TGroup::save( )
{
    TBDS &bds  = owner().owner().BD();
    bds.open( owner().grpBD(), true ).at().fieldSet(*this);
    bds.close(owner().grpBD());
}

bool TGroup::user( const string &name )
{
    if( m_usrs.find(name,0) != string::npos ) return(true);
    return(false);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TGroup::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='prm'>"
	  "<fld id='name' acs='0644' tp='str'/>"
	  "<fld id='dscr' acs='0644' tp='str'/>"
	  "<fld id='id' acs='0644' tp='dec'/>"
	  "<list id='users' acs='0644' tp='str' s_com='add,del' dest='select' select='/prm/usrs'/>"
	  "<comm id='load' acs='0550'/>"
	  "<comm id='save' acs='0550'/>"    
	 "</area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load( i_cntr );
    inf->text(Mess->I18Ns("Group ")+name());
    //prm
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Group"));
    c_nd->childGet(0)->attr(dscr,cfg("NAME").fld().descr());
    c_nd->childGet(1)->attr(dscr,cfg("DESCR").fld().descr());
    c_nd->childGet(2)->attr(dscr,cfg("ID").fld().descr());
    c_nd->childGet(3)->attr(dscr,cfg("USERS").fld().descr());
    c_nd->childGet(4)->attr(dscr,Mess->I18N("Load from BD"));
    c_nd->childGet(5)->attr(dscr,Mess->I18N("Save to BD"));
}

void TGroup::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )       ctrSetS( opt, name() );
    else if( a_path == "/prm/dscr" )  ctrSetS( opt, lName() );
    else if( a_path == "/prm/id" )    ctrSetI( opt, id() );
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
}

void TGroup::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/name" )       	name(ctrGetS( opt ));
    else if( a_path == "/prm/dscr" )  	lName(ctrGetS( opt ));
    else if( a_path == "/prm/id" )    	id(ctrGetI( opt ));
    else if( a_path.substr(0,12) == "/prm/users" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")
		{
		    if(m_usrs.size()) m_usrs=m_usrs+";";
		    m_usrs=m_usrs+t_c->text();
		}
		else if(t_c->attr("do") == "del") 
		{
		    int pos = m_usrs.find(string(";")+t_c->text(),0);
		    if(pos != string::npos) 
			m_usrs.erase(pos,t_c->text().size()+1);
		    else                    
			m_usrs.erase(m_usrs.find(t_c->text(),0),t_c->text().size()+1);
		}
	    }
	}
    else if( a_path == "/prm/load" )	load();
    else if( a_path == "/prm/save" )	save();	
}

#include <getopt.h>

#include "xml.h"
#include "tsys.h"
#include "tmessage.h"
#include "tsequrity.h"

SCfgFld TSequrity::gen_elem[] =
{
    {"NAME"    ,"Name."             ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"DESCR"   ,"Description."      ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},    
    {"ID"      ,"Identificator."    ,CFG_T_INT                 ,"","",""           ,"3" ,""          ,"%d"},    
    {"PASS/UID","Password for user or UIDs.",CFG_T_STRING      ,"","",""           ,"50",""          ,"%s"},
    {"TYPE"    ,"Type (User;Group).",CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","User;Group"}
};

const char *TSequrity::o_name = "TSequrity";
const char *TSequrity::s_name = "Sequrity";
const char *TSequrity::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_bd' dscr='Generic controll.' acs='0440'>"
    "  <fld id='t_bd' dscr='Type BD' acs='0660' tp='str' dest='select' select='a_bd/b_mod'/>"
    "  <fld id='bd' dscr='BD' acs='0660' tp='str'/>"
    "  <fld id='tbl' dscr='Table' acs='0660' tp='str'/>"
    "  <fld id='g_help' dscr='Options help' acs='0440' tp='str' cols='90' rows='5'/>"
    "  <comm id='load_bd' dscr='Load BD'/>"
    "  <comm id='upd_bd' dscr='Update BD'/>"
    "  <list id='b_mod' tp='str' hide='1'/>"
    " </area>"    
    " <area id='a_usgr' dscr='Users and groups'>"
    "  <list id='users' dscr='Users' s_com='add,del' tp='br' mode='att'/>"    
    "  <list id='grps' dscr='Groups' s_com='add,del' tp='br' mode='att'/>"    
    " </area>"    
    "</oscada_cntr>";

TSequrity::TSequrity( TKernel *app ) : 
    owner(app), TContr( i_cntr ), m_hd_usr(o_name), m_hd_grp(o_name), m_bd("", "", "sequrity.dbf")
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) cfe_Add(&gen_elem[i]);
	
    //Add surely users, groups and set parameters
    usr_add("root");
    unsigned hd = usr_att("root");
    usr_at(hd).Descr("Administrator (superuser)!!!");
    usr_at(hd).Pass("openscada");
    usr_det(hd);    
    
    grp_add("root");
    hd = grp_att("root");
    grp_at(hd).Descr("Administrators group.");
    grp_det(hd);    
}

TSequrity::~TSequrity(  )
{
    vector<string> list;

    //Free groups
    m_hd_grp.lock();
    grp_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
        grp_del(list[i_ls]);    	
    //Free users list
    m_hd_usr.lock();
    usr_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	usr_del(list[i_ls]);    
}

void TSequrity::usr_add( string name )
{    
    TUser *user = new TUser(this,name,usr_id_f());
    try{ m_hd_usr.obj_add( user, &user->Name() ); }
    catch(TError err) {	delete user; }
}

void TSequrity::grp_add( string name )
{
    TGroup *grp = new TGroup(this,name,grp_id_f());
    try{ m_hd_grp.obj_add( grp, &grp->Name() ); }
    catch(TError err) {	delete grp; }
}

unsigned TSequrity::usr_id_f()
{
    unsigned id = 0;
    vector<string> list;
    usr_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = usr_att( list[i_l] );
	if( usr_at(hd).Id() == id ) { id++; i_l=-1; }
	usr_det(hd);	    
    }
    return(id);
}

unsigned TSequrity::grp_id_f()
{
    unsigned id = 0;
    vector<string> list;
    grp_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = grp_att( list[i_l] );
	if( grp_at(hd).Id() == id ) { id++; i_l=-1; }
	grp_det(hd);	    
    }
    return(id);
}

string TSequrity::usr( int id )
{
    vector<string> list;
    
    usr_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = usr_att( list[i_l] );
	if( usr_at(hd).Id() == id ) { usr_det(hd); return(list[i_l]); }
	usr_det(hd);	    
    }
    return("");
}

string TSequrity::grp( int id )
{
    vector<string> list;
    
    grp_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = grp_att( list[i_l] );
	if( grp_at(hd).Id() == id ) { grp_det(hd); return(list[i_l]); }
	grp_det(hd);	    
    }
    return("");
}

bool TSequrity::access( string user, char mode, int owner, int group, int access )
{
    bool p_r = false, p_w = false, p_x = false;

    try
    {
    	int hd = usr_att(user);
	// Check owner permision
	if( usr_at(hd).Id() == 0 || usr_at(hd).Id() == owner )
	    if( ((mode&SEQ_RD)?access&0400:true) && 
		((mode&SEQ_WR)?access&0200:true) && 
		((mode&SEQ_XT)?access&0100:true) )
	    { usr_det(hd); return(true); }	
	usr_det(hd);
	// Check other permision
	if( ((mode&SEQ_RD)?access&0004:true) && 
	    ((mode&SEQ_WR)?access&0002:true) && 
	    ((mode&SEQ_XT)?access&0001:true) )
	    return(true); 	
	// Check groupe permision
	string n_grp = grp(group);
	if( n_grp.size() )
	{
	    int g_hd = grp_att(n_grp);
	    if( grp_at(g_hd).user(user) &&
		((mode&SEQ_RD)?access&0040:true) && 
		((mode&SEQ_WR)?access&0020:true) && 
		((mode&SEQ_XT)?access&0010:true) )
	    {
		grp_det(g_hd);
		return(true);
	    }
	    grp_det(hd);
	}	
    }catch(...){ return(false); }

    return(false);
}

XMLNode *TSequrity::XMLCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = Owner().XMLCfgNode()->get_child("section",i_k++);
	if( t_n->get_attr("id") == Name() ) return( t_n );
    }
}

void TSequrity::Init( )
{
    LoadBD();
}

string TSequrity::opt_descr( )
{
    string rez;
    rez = rez +	
	"========================= "+Name()+" subsystem options ====================\n"+
	"------------------ Section parameters of config file ----------------------\n"+
	"GenBD  <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n";
    
    return(rez);
}

void TSequrity::CheckCommandLine(  )
{
#if OSC_DEBUG
    Owner().m_put("DEBUG",MESS_INFO,"%s:Read commandline options!",Name().c_str());
#endif
	
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Owner().m_put("DEBUG",MESS_DEBUG,"%s:Read commandline options ok!",Name().c_str());
#endif
}

void TSequrity::UpdateOpt()
{
    string opt;

    try
    {
    	opt = XMLCfgNode()->get_child("id","GenBD")->get_text(); 
	int pos = 0;
        m_bd.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	if( !m_bd.tp.size() ) m_bd.tp = Owner().DefBDType;
	if( !m_bd.bd.size() ) m_bd.bd = Owner().DefBDName;
    }
    catch(...) {  }    
}

void TSequrity::LoadBD( )
{
    TConfig *c_el;
    string name,type;
   
    try
    {
	SHDBD b_hd = Owner().BD().open( m_bd );    
	for( int i_ln = 0; i_ln < Owner().BD().at(b_hd).NLines(); i_ln++ )
	{
	    c_el = new TConfig(this);
	    c_el->cf_LoadValBD(i_ln,Owner().BD().at(b_hd));
	    name = c_el->cf_Get_S("NAME");
	    type = c_el->cf_Get_SEL("TYPE");
	    delete c_el;	
	    
	    if( type == "User" )
	    {
		try{usr_add(name);}catch(...){}
		int hd = usr_att(name);
		usr_at(hd).cf_LoadValBD(i_ln,Owner().BD().at(b_hd));
		usr_det(hd);
	    }
	    else if( type == "Group" )
	    {
		try{grp_add(name);}catch(...){}
		int hd = grp_att(name);
		grp_at(hd).cf_LoadValBD(i_ln,Owner().BD().at(b_hd));
		grp_det(hd);
	    }
	}
	Owner().BD().close(b_hd);
    }catch(...){}
}

void TSequrity::UpdateBD( )
{
    vector<string> list;
    SHDBD b_hd;
    
    try{ b_hd = Owner().BD().open( m_bd ); }
    catch(...) { b_hd = Owner().BD().open( m_bd,true ); }
    Owner().BD().at(b_hd).Clean();
    cfe_UpdateBDAttr( Owner().BD().at(b_hd) );
    usr_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	int hd = usr_att(list[i_l]);
	usr_at(hd).cf_SaveValBD(-1,Owner().BD().at(b_hd));
	usr_det( hd );
    }
    grp_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	int hd = grp_att(list[i_l]);
	grp_at(hd).cf_SaveValBD(-1,Owner().BD().at(b_hd));
	grp_det( hd );
    }
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TSequrity::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(Name()+" subsystem.");
}

void TSequrity::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bd" )     ctr_opt_setS( opt, m_bd.tp );
	else if( t_id == "bd" )  ctr_opt_setS( opt, m_bd.bd );
	else if( t_id == "tbl" ) ctr_opt_setS( opt, m_bd.tbl );
	else if( t_id == "b_mod" )
	{
	    Owner().BD().gmd_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a );
	}
	else if( t_id == "g_help" ) ctr_opt_setS( opt, opt_descr() );       
    }   
    else if( t_id == "a_usgr" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "users" )
	{
    	    usr_list(list);
 	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 	
	}
	else if( t_id == "grps" )
	{
    	    grp_list(list);
 	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 	
	}    
    }
}

void TSequrity::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bd" )     m_bd.tp  = ctr_opt_getS( opt );
	else if( t_id == "bd" )  m_bd.bd  = ctr_opt_getS( opt );
	else if( t_id == "tbl" ) m_bd.tbl = ctr_opt_getS( opt );
    }   
    else if( t_id == "a_usgr" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "users" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      usr_add(t_c->get_text());
		    else if(t_c->get_attr("do") == "del") usr_del(t_c->get_text());
		}
	    }
	else if( t_id == "grps" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      grp_add(t_c->get_text());
		    else if(t_c->get_attr("do") == "del") grp_del(t_c->get_text());
		}
	    }
    }
}

void TSequrity::ctr_cmd_go( string a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "load_bd" )     LoadBD();
	else if( t_id == "upd_bd" ) UpdateBD();
    }
}

unsigned TSequrity::ctr_att( string br ) 
{ 
    string t_id = ctr_path_l(br,0);
    if( t_id == "a_usgr" )
    {
	t_id = ctr_path_l(br,1);
	if( t_id == "users" )     return( usr_att( ctr_path_l(br,2) ) ); 
	else if( t_id == "grps" ) return( grp_att( ctr_path_l(br,2) ) ); 
    }
}

void TSequrity::ctr_det( string br, unsigned hd ) 
{ 
    string t_id = ctr_path_l(br,0);
    if( t_id == "a_usgr" )
    {
	t_id = ctr_path_l(br,1);
	if( t_id == "users" )     usr_det( hd ); 
	else if( t_id == "grps" ) grp_det( hd ); 
    }     
}

TContr &TSequrity::ctr_at( string br, unsigned hd )  
{
    string t_id = ctr_path_l(br,0);
    if( t_id == "a_usgr" )
    {
	t_id = ctr_path_l(br,1);
	if( t_id == "users" )     return( (TContr&)usr_at(hd) ); 
	else if( t_id == "grps" ) return( (TContr&)grp_at(hd) ); 
    }            
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
const char *TUser::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_prm' dscr='Parameters'>"
    "  <fld id='name' dscr='Name' acs='0644' tp='str'/>"
    "  <fld id='dscr' dscr='Full name' acs='0644' tp='str'/>"
    "  <fld id='id' dscr='Identificator' acs='0644' tp='dec'/>"
    "  <comm id='pass' dscr='Set password' acs='0500'>"
    "   <fld id='ps' dscr='Password' tp='str'/>"
    "  </comm>"
    " </area>"
    "</oscada_cntr>";
    
TUser::TUser( TSequrity *owner, string name, unsigned id ) : 
    m_owner(owner), TContr( i_cntr ), TConfig(owner),
    m_lname(cf_Get_S("DESCR")), m_pass(cf_Get_S("PASS/UID")), m_name(cf_Get_S("NAME")), m_id(cf_Get_I_("ID"))
{
    Name(name);
    Id(id);
    cf_Set_SEL("TYPE","User");
}

TUser::~TUser(  )
{

}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TUser::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(string("User ")+Name());
    
    XMLNode *u_ar = ctr_id(inf,"a_prm");    
    for( int i_ch = 0; i_ch < u_ar->get_child_count(); i_ch++ )
	u_ar->get_child(i_ch)->set_attr("own",TSYS::int2str(m_id),true);
}

void TUser::ctr_din_get_( string a_path, XMLNode *opt )
{    
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       ctr_opt_setS( opt, Name() );
    	else if( t_id == "dscr" )  ctr_opt_setS( opt, Descr() );
    	else if( t_id == "id" )    ctr_opt_setI( opt, Id() );
    }
}

void TUser::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       Name( ctr_opt_getS( opt ) );
    	else if( t_id == "dscr" )  Descr( ctr_opt_getS( opt ) );
    	else if( t_id == "id" )    Id( ctr_opt_getI( opt ) );
    }
}

void TUser::ctr_cmd_go( string a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    	if( ctr_path_l(a_path,1) == "pass" ) Pass( ctr_opt_getS(ctr_id(fld,"ps")) );
}
//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
const char *TGroup::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_prm' dscr='Parameters'>"
    "  <fld id='name' dscr='Name' acs='0664' tp='str'/>"
    "  <fld id='dscr' dscr='Full name' acs='0664' tp='str'/>"
    "  <fld id='id' dscr='Identificator' acs='0664' tp='dec'/>"
    "  <list id='users' dscr='Users' acs='0664' tp='str' s_com='add,del' dest='select' select='a_prm/usrs'/>"
    "  <list id='usrs' tp='str' hide='1'/>"
    " </area>"
    "</oscada_cntr>";
    
TGroup::TGroup( TSequrity *owner, string name, unsigned id ) : 
    m_owner(owner), TContr( i_cntr ), TConfig(owner),
    m_lname(cf_Get_S("DESCR")), m_usrs(cf_Get_S("PASS/UID")), m_name(cf_Get_S("NAME")), m_id(cf_Get_I_("ID"))
{
    Name(name);
    Id(id);
    cf_Set_SEL("TYPE","Group");
}

TGroup::~TGroup(  )
{

}

bool TGroup::user( string name )
{
    if( m_usrs.find(name,0) != string::npos ) return(true);
    return(false);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TGroup::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(string("Group ")+Name());
}

void TGroup::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;

    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       ctr_opt_setS( opt, Name() );
    	else if( t_id == "dscr" )  ctr_opt_setS( opt, Descr() );
    	else if( t_id == "id" )    ctr_opt_setI( opt, Id() );
    	else if( t_id == "users" )
	{
	    int pos = 0,c_pos,i_us=0;
	    do
	    {
		c_pos = m_usrs.find(";",pos);
		string val = m_usrs.substr(pos,c_pos-pos);
		if( val.size() ) ctr_opt_setS( opt, val, i_us++ );
		pos = c_pos+1;
	    }while(c_pos != string::npos);
	}
    	else if( t_id == "usrs" )  
	{
	    Owner().usr_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a );
	}
    }
}

void TGroup::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       Name(ctr_opt_getS( opt ));
    	else if( t_id == "dscr" )  Descr(ctr_opt_getS( opt ));
    	else if( t_id == "id" )    Id(ctr_opt_getI( opt ));
    	else if( t_id == "users" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")
		    {
			if(m_usrs.size()) m_usrs=m_usrs+";";
			m_usrs=m_usrs+t_c->get_text();
		    }
		    else if(t_c->get_attr("do") == "del") 
		    {
		        int pos = m_usrs.find(string(";")+t_c->get_text(),0);
			if(pos != string::npos) 
			    m_usrs.erase(pos,t_c->get_text().size()+1);
			else                    
			    m_usrs.erase(m_usrs.find(t_c->get_text(),0),t_c->get_text().size()+1);
		    }
		}
	    }
    }
}


#include <getopt.h>
#include <string>
//#include <strstream>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tsequrity.h"
#include "web_cfg.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_cfg"
#define NAME_TYPE   "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define VERSION     "0.3.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web config interface for http protocol"
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    SAtMod module( int n_mod )
    {
    	SAtMod AtMod;

	if(n_mod==0)
	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.name  = "";

	return( AtMod );
    }

    TModule *attach( SAtMod &AtMod, string source )
    {
	WebCfg::TWEB *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new WebCfg::TWEB( source );       

	return ( self_addr );
    }    
}

using namespace WebCfg;

//==============================================================================
//================= WebCfg::TWEB ===============================================
//==============================================================================
SExpFunc TWEB::ExpFuncLc[] =
{
    {"HttpGet",(void(TModule::*)( )) &TWEB::HttpGet,"void HttpGet(string &url, string &page, string &sender, vector<string> &vars);",
     "Process Get comand from http protocol's!",10,0},
    {"HttpPost",(void(TModule::*)( )) &TWEB::HttpPost,"void HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein);",
     "Process Post comand from http protocol's!",10,0}     
};

TWEB::TWEB( string name ) : m_t_auth(10)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;

    ExpFunc   = (SExpFunc *)ExpFuncLc;
    NExpFunc  = sizeof(ExpFuncLc)/sizeof(SExpFunc);

    m_res = TSYS::ResCreate( );
}

TWEB::~TWEB()
{
    TSYS::WResRequest(m_res);
    while( m_auth.size() )
    { 
	delete m_auth[0]; 
	m_auth.erase(m_auth.begin()); 
    }
    TSYS::WResRelease(m_res);
    
    TSYS::ResDelete( m_res );
}

string TWEB::mod_info( const string name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TWEB::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

void TWEB::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "ses_t_life=<time>       set sesion time life, minets (default 10);\n"    
    "\n",NAME_MODUL,NAME_MODUL);
}

void TWEB::mod_CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TWEB::mod_UpdateOpt()
{
    try{ m_t_auth = atoi( mod_XMLCfgNode()->get_child("ses_t_life")->get_text().c_str() ); }
    catch(...) {  }
}

char *TWEB::w_head =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html>\n"
    "<head>\n"
    "<title>OpenSCADA configuration web modul!</title>\n"
    "</head>\n";

char *TWEB::w_head_ =
    "</html>\n";

char *TWEB::w_body =
    "<body bgcolor=\"#2A4547\" text=\"#ffffff\" link=\"#3366ff\" vlink=\"#339999\" alink=\"#33ccff\">\n"
    "<h1 align=\"center\"><font color=\"#ffff00\"> OpenSCADA configuration web modul!</font></h1>\n"
    "<hr width=\"100%\" size=\"3\"><br>\n";

char *TWEB::w_body_ =
    "<hr width=\"100%\" size=\"3\">\n"
    "</body>\n";        

void TWEB::HttpGet(string &url, string &page, string &sender, vector<string> &vars )
{
    page = page+w_head+w_body;
    
    string ses_user = check_ses( atoi(get_cookie( "oscd_u_id", vars ).c_str()) );
    if( ses_user.size() ) get_info( url, page, *SYS, string("/")+NAME_MODUL, ses_user );
    else 	          get_auth( page );
    page = page+w_body_+w_head_;
}

void TWEB::get_info( string &url, string &page, TContr &cntr, string path, string ses_user )
{         
    XMLNode *node = cntr.ctr_info();
    if( url.size() > 1 ) 
    {	
        int n_dir = url.find("/",1); 
	if( n_dir == string::npos ) 
	{
	    url=url+"/";
	    n_dir = url.find("/",1); 
	}
    	string br_s = url.substr(1,n_dir-1);
	XMLNode *brs;
	try{ brs = node->get_child("branchs"); }
	catch(...)
	{ 
	    page =  page+"Error! Child <branchs> no avoid."; 	    
    	    delete node;
    	    return;
	}

	int i_br=0;
	
	cntr.ctr_din_get(brs);
	while(true)
	{
	    try
	    {
		XMLNode *br = brs->get_child("br",i_br++);
		if( br->get_attr("id") == br_s )
		{
		    string n_url = url.substr(n_dir,url.size()-n_dir); 
		    if( brs->get_attr("mode") == "att" )
		    {
			unsigned hd = cntr.ctr_att( br );			
			get_info( n_url, page, cntr.ctr_at(hd), path+"/"+br_s, ses_user ); 
			cntr.ctr_det(hd);
		    }
		    else if( brs->get_attr("mode") == "at" )
			get_info( n_url, page, cntr.ctr_at(br), path+"/"+br_s, ses_user ); 
		    break;
		}
	    }
	    catch(TError err)
	    { 
    		page =  page+"Error! Branch "+br_s+" no avoid."+err.what(); 	    
		delete node;
	    	return;
	    } 
	}       
	delete node;
	return;
    }
    
    page = page+ "<h2 align=\"center\"><font color=aqua>"+node->get_text()+"</font></h2>\n";

    // Draw Config elements
    page = page+"<table width=100% border=2><tr><td>";    
    page = page+"<h3 align=\"left\"><font color=moccasin>Parameters</font></h3>\n";    
    page = page+"<form action=\""+path+"\" method=\"POST\" enctype=\"multipart/form-data\">\n"; 
    get_cfg( *node, *node, cntr, page, ses_user );
    page = page+"<input type=\"submit\" name=\"cfg_apply\" value=\"Apply\"><br>\n";    // Submit button 
    page = page+"</form>";
    page = page+"</td></tr></table>";
    
    // Draw generic commands
    page = page+"<table width=100% border=2><tr><td>";
    page = page+"<h3 align=\"left\"><font color=moccasin>Metods</font></h3>\n";    
    get_cmd( *node, *node, cntr, page, path, ses_user );        
    page = page+"</td></tr></table>";
    
    // Draw branchs
    page = page+"<table width=100% border=2><tr><td>";
    page = page+"<h3 align=\"left\"><font color=moccasin>Branches</font></h3>\n";
    get_branch( *node, *node, cntr, page, path, ses_user );
    page = page+"</td></tr></table>\n";
    
    page = page+"<hr width=\"100%\" size=\"2\">\n";
    
    // Draw path and license
    page = page+"<table width=100%><tr>\n"+
	    "<td width=50% valign=top align=left>"+path+"</td>\n"+
            "<td align=right width=50%>Version: "+VERSION+"<br>Autors: "+AUTORS+"<br>License: "+LICENSE+"</td>\n"+
	    "</tr></table>\n";
    
    delete node;
}
	    
void TWEB::get_cfg( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user )
{
    unsigned i_cf,c_cfg;    
    
    for( i_cf = 0, c_cfg = 0; i_cf < node.get_child_count(); i_cf++)
    {
	XMLNode *t_s = node.get_child(i_cf);
	if( t_s->get_name() == "configs" && chk_access(t_s, ses_user, SEQ_RD) )
	{
	    if(c_cfg++ == 0) page = page+"<ul>";
	    page =  page+
		    "<li><font size=\"+1\"><b><i>"+
		    t_s->get_text()+
		    "</b></i></font></li>\n";
	    page = page+"<table><tbody>\n";		
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( (t_c->get_name() == "fld" || t_c->get_name() == "list") && chk_access(t_c, ses_user, SEQ_RD) )
		{
		    page = page+"<tr><td>"+t_c->get_attr("dscr")+":</td><td>";
    		    get_val(root,*t_c,cntr,page,ses_user);
		    page = page+"</td>";
		}
	    }
	    page = page+"</tbody></table><br>\n";
	    
    	    get_cfg( root, *t_s, cntr, page, ses_user);
	}
    }
    if(c_cfg > 0) page = page+"</ul>\n";
}

void TWEB::get_branch( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string ses_user )
{
    int i_br, c_cfg;      //section counter

    for( i_br = 0, c_cfg = 0; i_br < node.get_child_count(); i_br++)
    {
	XMLNode *t_s = node.get_child(i_br);
	if( t_s->get_name() == "branchs" && chk_access(t_s, ses_user, SEQ_RD) )
	{
	    cntr.ctr_din_get(t_s);
	    if(c_cfg++ == 0) page = page+"<ul>";
	    page =  page+"<li><font size=\"+1\"><b><i>"+t_s->get_attr("dscr")+"</b></i></font></li>\n";			
	    page = page+"<ul>\n";
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( t_c->get_name() == "br" && chk_access(t_c, ses_user, SEQ_RD) )
		    page = page+ "<li> <a href=\""+path+"/"+t_c->get_attr("id")+"\">"+
			   t_c->get_attr("id")+":"+t_c->get_attr("dscr")+"</a> </li>";
	    }
	    page = page+"</ul>\n";
	    page = page+"<h4 align=\"left\"><font color=moccasin>Branch metods</font></h4>\n";
	    get_cmd( root, *t_s, cntr, page, path, ses_user );
	}
    }
    if(c_cfg > 0) page = page+"</ul>\n";
}

void TWEB::get_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string ses_user )
{
    int i_cmd, c_cfg;      //section counter

    for( i_cmd = 0, c_cfg = 0; i_cmd < node.get_child_count(); i_cmd++ )
    {
	XMLNode *t_s = node.get_child(i_cmd);
	if( t_s->get_name() == "comm" && chk_access(t_s, ses_user, SEQ_RD) )
	{
	    if(c_cfg++ == 0) page = page+"<ul>";
	    page = page+"<form action='"+path+"' method='POST' enctype='multipart/form-data'>\n"; 
	    page = page+"<li><input type='hidden' name='o_comm' value='o_comm'>\n";
	    page = page+"<input type='submit' name='"+t_s->get_attr("id")+"' value='"+t_s->get_attr("dscr")+"'>\n";
	    page = page+"(";
	    for( unsigned i_el=0, f_cfg=0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( t_c->get_name() == "fld" )
		{
		    if(f_cfg++ > 0) page = page+",";
		    page = page+t_c->get_attr("dscr")+": ";
    		    get_val(root,*t_c,cntr,page,ses_user);
		}
	    }
	    page = page+")\n";
	    page = page+"</li>\n";
	    page = page+"</form>\n"; 
	}
    }
    if(c_cfg > 0) page = page+"</ul>\n";
}

void TWEB::get_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user )
{
    if( node.get_name() == "fld" )
    {
	cntr.ctr_din_get(&node);
	if( node.get_attr("dest") == "select" )
	{
	    XMLNode *x_lst = cntr.ctr_id(&root,node.get_attr("select"));
	    page = page+ "<select name=\""+node.get_attr("id")+"\">";
	    if( x_lst->get_name() == "branchs" )
	    {
		for( unsigned i_br = 0; i_br < x_lst->get_child_count(); i_br++ ) 
		    if( x_lst->get_child(i_br)->get_name() == "br")
			page = page+"<option value=\""+x_lst->get_child(i_br)->get_attr("id")+"\">"+
				    x_lst->get_child(i_br)->get_attr("id")+"</option>";
	    }
	    else if( x_lst->get_name() == "list" )
	    {
		for( unsigned i_el = 0; i_el < x_lst->get_child_count(); i_el++ ) 
		    if( x_lst->get_child(i_el)->get_name() == "el")
		    { }
	    }			
	    page = page+ "</select>\n";
	}
	else
	{
	    if( chk_access(&node, ses_user, SEQ_WR) )
	    {
    		if( node.get_attr("tp") == "bool" )
    		{
    		    page = page+"<input type=\"checkbox\" name=\""+node.get_attr("id")+"\"";
    		    if( node.get_text() == "true" ) page=page+" checked";
    		    page = page + ">\n";
    		}
    		else
    		{
    		    page = page+"<input type=\"text\" name=\""+node.get_attr("id")+"\" value=\""+node.get_text()+"\"";
    		    // addon parameters
    		    string val_t = node.get_attr("len");
    		    if( val_t.size() ) page = page + " maxlength="+val_t;			
    		    page = page + ">\n";
    		}
	    } else page = page + "<b>"+node.get_text()+"</b>\n";
	}
    }
    else if( node.get_name() == "list" )
    {
	cntr.ctr_din_get(&node);
	page = page+"<ul>";		    
	for( unsigned i_lel = 0; i_lel < node.get_child_count(); i_lel++)		    
	    if( node.get_child(i_lel)->get_name() == "el" && node.get_child(i_lel)->get_attr("hide") != "1" )
    		page = page+"<il>"+node.get_child(i_lel)->get_text()+"</il>";
	page = page+"</ul>\n";
    }
}

char TWEB::get_auth( string &page )
{
    page = page+"<table border=2 width=40% align='center'>"+
	       "<tr bgcolor=#9999ff><td><b> Enter to module</b></td></tr>\n"+
	       "<tr bgcolor=#cccccc> <td align=center><table cellpadding=3>"+
	       "<form method=POST action='"+string("/")+NAME_MODUL+"' enctype='multipart/form-data'>\n"+
	       "<tr> <td><b>User name</b></td> <td><input type=text name=user size=20></td> </tr>\n"+
	       "<tr> <td><b>Password</b></td> <td><input type=password name=pass size=20></td> </tr>\n"+
	       "<tr> <td colspan=2 align=center><input type=submit name='auth_enter' value='Enter'>\n"+
	       "<input type=reset name='clean' value='Clean'>\n"+
	       "</td> </tr>\n"+
	       "</form>\n"+
	       "</table></td></tr></table>\n";
}

void TWEB::HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein )
{
    bool my = false, err = false;
    int  kz;
    string ses_user;
    
    page = page+ w_head+w_body;
    // Check autentification POST request
    if( !my )
    {
	kz = post_auth( page, contein, ses_user );    
	if( kz&0x01 ) my  = true;
	if( kz&0x02 ) err = true;
    }
    // Check avoid sesion
    if( !my )
    {
	ses_user = check_ses( atoi(get_cookie( "oscd_u_id", vars ).c_str()) );
	if( ses_user.size() )
	{
	    kz = post_info( url, page, *SYS, string("/")+NAME_MODUL, ses_user, contein );
	    if( kz&0x01 ) my  = true;
	    if( kz&0x02 ) err = true;
	}
	else 
	{ 
	    get_auth( page ); 
	    my  = true; 
	    err = true; 
	}
    }
    //Request error
    if( !my )       page = page+ "<h3 align='center'><font color='red'> Post request broken! </font></h3><p>\n";
    else if( !err ) get_info( url, page, *SYS, string("/")+NAME_MODUL, ses_user );
    page = page+ w_body_+w_head_;
}

int TWEB::post_info( string &url, string &page, TContr &cntr, string path, string ses_user, string &contein )
{         
    int kz=0;
    
    XMLNode *node = cntr.ctr_info();
    if( url.size() > 1 ) 
    {	
        int n_dir = url.find("/",1); 
	if( n_dir == string::npos ) 
	{
	    url=url+"/";
	    n_dir = url.find("/",1); 
	}
    	string br_s = url.substr(1,n_dir-1);
	XMLNode *brs;
	try{ brs = node->get_child("branchs"); }
	catch(...)
	{ 
	    page = page+"Error! Child <branchs> no avoid."; 	    
    	    delete node;
    	    return( 0x01|0x02 );
	}

	int i_br=0;
	
	cntr.ctr_din_get(brs);
	while(true)
	{
	    try
	    {
		XMLNode *br = brs->get_child("br",i_br++);
		if( br->get_attr("id") == br_s )
		{
		    string n_url = url.substr(n_dir,url.size()-n_dir); 
		    if( brs->get_attr("mode") == "att" )
		    {
			unsigned hd = cntr.ctr_att( br );			
			kz = post_info( n_url, page, cntr.ctr_at(hd), path+"/"+br_s, ses_user, contein ); 
			cntr.ctr_det(hd);
		    }
		    else if( brs->get_attr("mode") == "at" )
			kz = post_info( n_url, page, cntr.ctr_at(br), path+"/"+br_s, ses_user, contein ); 
			
		    delete node;
		    return(kz);
		}
	    }
	    catch(TError err)
	    { 
    		page =  page+"Error! Branch "+br_s+" no avoid."+err.what(); 	    
		delete node;
	    	return( 0x01|0x02 );
	    } 
	}       
    }
    //Check configs post
    if( !kz )
    {
	vector<string> names, vals;
	cont_frm_data( contein, names, vals );
	for( unsigned i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	    if( names[i_cnt] == "cfg_apply" )
	    {
	    	kz = post_cfg( *node, *node, cntr, page, ses_user, contein );
		break;
	    }
    }
    //Check commands post
    if( !kz ) kz = post_cmd( *node, *node, cntr, page, ses_user, contein );
    //Check command for branch post
    if( !kz )
	for( int i_br = 0; i_br < node->get_child_count(); i_br++)
	{
	    XMLNode *t_s = node->get_child(i_br);
	    if( t_s->get_name() == "branchs" )
	       	kz = post_cmd( *node, *t_s, cntr, page, ses_user, contein );
	    if( kz ) break;
	}
    
    return( kz );
}

int TWEB::post_cfg( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein )
{
    unsigned i_cf,c_cfg;    

    for( i_cf = 0, c_cfg = 0; i_cf < node.get_child_count(); i_cf++)
    {
	XMLNode *t_s = node.get_child(i_cf);
	if( t_s->get_name() == "configs" && chk_access(t_s, ses_user, SEQ_RD) )
	{
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( (t_c->get_name() == "fld" || t_c->get_name() == "list") && chk_access(t_c, ses_user, SEQ_RD|SEQ_WR) )
		{
    		    if( post_val(root,*t_c,cntr,page,ses_user,contein) ) c_cfg++;
		    else return( 0x01|0x02 );                 //Error
		}
	    }
    	    int kz = post_cfg( root, *t_s, cntr, page, ses_user,contein);
	    if(kz&0x02) return(kz);
	}
    }    
    return( 0x01 );
}

int TWEB::post_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein )
{ 
    int i_cnt;      
    vector<string> names, vals;
    cont_frm_data( contein, names, vals );

    for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	if( names[i_cnt] == "o_comm" ) break;
    if( i_cnt >= names.size() ) return(0);    //No command

    for( int i_cmd = 0; i_cmd < node.get_child_count(); i_cmd++ )
    {
	XMLNode *t_s = node.get_child(i_cmd);
	if( t_s->get_name() == "comm" )
	{
	    for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
		if( names[i_cnt] == t_s->get_attr("id") ) break;
	    if( i_cnt >= names.size() ) continue;    //Next command
	    // Prepare command options
	    for( int i_prm = 0; i_prm < t_s->get_child_count(); i_prm++ )
	    {
		XMLNode *t_c = t_s->get_child(i_prm);
		if( t_c->get_name() == "fld" )
		    prepare_val( root, *t_c, cntr, page, ses_user, contein, false );
	    }	    
	    try
	    { 	    
		m_put("CONTROL",MESS_INFO,"%s| Put command <%s:%s>",
		    	ses_user.c_str(),
			t_s->get_attr("id").c_str(),
			t_s->get_attr("dscr").c_str());
		cntr.ctr_cmd_go(t_s);
		return( 0x01 );
	    }
	    catch(TError err)
	    {
		page = page+err.what();	
		return(0x01|0x02);
	    }
	}
    }
    return( 0 );
}

bool TWEB::post_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein )
{
    if( !prepare_val( root, node, cntr, page, ses_user, contein, true ) ) return(true);
    string kz = cntr.chk_opt_val(&node,true);
    if( !kz.size() )
    {
	try
	{ 	    
	    m_put("CONTROL",MESS_INFO,"%s| Change <%s:%s> to %s",
		    ses_user.c_str(),
		    node.get_attr("id").c_str(),
		    node.get_attr("dscr").c_str(),
		    node.get_text().c_str());
	    cntr.ctr_din_set(&node); 
	    return(true); 
	}
	catch(TError err)
	{
	    page = page+err.what();	
	    return(false);
	}
    }
    page = page+kz;
    return(false);
}

bool TWEB::prepare_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein, bool compare )
{
    unsigned i_cnt;
    string   val;
    vector<string> names, vals;
    cont_frm_data( contein, names, vals );    
    
    if( node.get_attr("tp") == "bool" )
    {    	
	for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	    if( names[i_cnt] == node.get_attr("id") ) break;
	if( i_cnt < names.size() ) val = "true";
	else                       val = "false";
	if( compare )
	{
	    cntr.ctr_din_get(&node); 
	    if( node.get_text() == val) return(false);   //No change fld 
	}
    }
    else
    {
	for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	    if( names[i_cnt] == node.get_attr("id") ) break;
	if( i_cnt < names.size() )
	{
	    val = vals[i_cnt];
	    if( compare )
	    {
		cntr.ctr_din_get(&node); 
		if( node.get_text() == val) return(false);   //No change fld 
	    }
	}
	else return(false);
    } 
    node.set_text(val);

    return(true);
}

int TWEB::post_auth( string &page, string &contein, string &user )
{
    unsigned i_cnt; 
    //Check Auth entry
    vector<string> names, vals;
    cont_frm_data( contein, names, vals );
    for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	if( names[i_cnt] == "auth_enter" ) break;	
    if( i_cnt < names.size() )
    {
	string pass;
	//Get user name
	for( i_cnt = 0; i_cnt < names.size(); i_cnt++ ) 
	    if( names[i_cnt] == "user" ) user = vals[i_cnt];
	//Get user password
	for( i_cnt = 0; i_cnt < names.size(); i_cnt++ ) 
	    if( names[i_cnt] == "pass" ) pass = vals[i_cnt];
	try
	{ 
	    int hd = Owner().Owner().Sequrity().usr_att(user); 
	    if( Owner().Owner().Sequrity().usr_at(hd).Auth(pass) )
	    {
		char t_buf[20];
	    
		snprintf( t_buf,sizeof(t_buf),"%d",open_ses(user) );
		//strstream r_val();
		page = string("HTTP/1.0 200 OK\nContent-type: text/html\nSet-Cookie: oscd_u_id=")+t_buf+"; path=/;\n\n";
		page = page+ w_head+w_body;
		Owner().Owner().Sequrity().usr_det(hd); 	    
		return( 0x01 );
	    }
	    Owner().Owner().Sequrity().usr_det(hd); 	    
	}
	catch(TError err){ m_put("SYS",MESS_WARNING,"Auth %s!",err.what().c_str()); }
	
	page = page+ "<h3 align='center'><font color='red'> Auth wrong. Retry please.</font></h3><p>\n";
	get_auth( page );
	return( 0x01|0x02 );
    }    
    return( 0x00 );
}

int TWEB::open_ses( string name )
{    
    //Check sesion and close old sesion
    TSYS::RResRequest(m_res);
    for( int i_s = 0; i_s < m_auth.size(); i_s++ )
	if( time(NULL) > (m_auth[i_s]->t_auth+m_t_auth*60) ) 
	{
	    delete m_auth[i_s];
	    m_auth.erase(m_auth.begin() + i_s);
	    i_s--;
	}
    TSYS::RResRelease(m_res);
    //Make new sesion
    SAuth *Auth = new SAuth();
    Auth->t_auth = time(NULL);
    Auth->name   = name;
    Auth->id_ses = rand();

    TSYS::WResRequest(m_res);
    m_auth.push_back( Auth );
    TSYS::WResRelease(m_res);
	    
    return(Auth->id_ses);
}

string TWEB::check_ses( int id )
{    
    string t_str;
    //Check sesion and close old sesion
    TSYS::RResRequest(m_res);
    for( unsigned i_s = 0; i_s < m_auth.size(); i_s++ )
	if( time(NULL) > (m_auth[i_s]->t_auth+m_t_auth*60) ) 
	{
	    delete m_auth[i_s];
	    m_auth.erase(m_auth.begin() + i_s);
	}
    //Find id sesion
    for( unsigned i_s = 0; i_s < m_auth.size(); i_s++ )
	if( m_auth[i_s]->id_ses == id )
	{
	    t_str = m_auth[i_s]->name; 
	    m_auth[i_s]->t_auth = time(NULL); 
	}
    TSYS::RResRelease(m_res);
    
    return( t_str );
}

void TWEB::cont_frm_data( const string &content, vector<string> &name, vector<string> &val )
{
    char *c_bound = "boundary=";
    char *c_term = "\r\n";
    char *c_end = "--";
    char *c_fd = "Content-Disposition: form-data;";
    char *c_name = "name=\"";    
    
    int i_bnd = content.find(c_bound,0)+strlen(c_bound);    
    if( i_bnd == string::npos ) return;
    int pos = content.find(c_term,i_bnd);
    if( pos == string::npos ) return;
    string boundary = content.substr(i_bnd,pos-i_bnd);
    pos += strlen(c_term);
    while(true)
    {
	pos = content.find(boundary,pos);
	if( pos == string::npos || content.substr(pos+boundary.size(),2) == "--" ) break;
	pos += boundary.size()+strlen(c_term);
	string c_head = content.substr(pos, content.find(c_term,pos)-pos);
        if( c_head.find(c_fd,0) == string::npos ) continue;
	//Get name
	i_bnd = c_head.find(c_name,0)+strlen(c_name);
	string c_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);	
	//Get value
	pos += c_head.size()+(2*strlen(c_term));
	string c_val  = content.substr(pos, content.find(c_term,pos)-pos);
	//Save
	name.push_back(c_name);
	val.push_back(c_val);
    }    
}

string TWEB::get_cookie( string name, vector<string> &vars )
{
    for( unsigned i_var = 0; i_var < vars.size(); i_var++)
	if( vars[i_var].substr(0, vars[i_var].find(":",0)) == "Cookie" )
	{
	    int i_beg = vars[i_var].find(name+"=",0);
	    if( i_beg == string::npos ) return("");
	    i_beg += name.size()+1;
	    return( vars[i_var].substr(i_beg,vars[i_var].find(";",i_beg)-i_beg) );
	}
    return("");
}

bool TWEB::chk_access( XMLNode *fld, string user, char mode )
{
    string s_acc = fld->get_attr("acs");
    string s_own = fld->get_attr("own");
    string s_grp = fld->get_attr("grp");
    if( !s_acc.size() ) s_acc = "0777";
    int accs = strtol(s_acc.c_str(),NULL,8);
    if( !s_own.size() ) s_own = "0";        //root
    int own = atoi(s_own.c_str());
    if( !s_grp.size() ) s_grp = "0";        //root
    int grp = atoi(s_grp.c_str());

    return( Owner().Owner().Sequrity().access( user, mode, own, grp, accs) );
}

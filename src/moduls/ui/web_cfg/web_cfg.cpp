#include <time.h>
#include <getopt.h>
#include <string>

#include <config.h>
#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include <tsequrity.h>

#include "web_cfg.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_cfg"
#define NAME_TYPE   "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define MOD_VERSION "0.6.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Configuration web modul."
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
const char *TWEB::i_cntr = 
    "<area id='bs'>"
    " <fld id='lf_tm' acs='0660' tp='dec'/>"
    " <fld id='o_help' acs='0440' tp='str' cols='90' rows='5'/>"
    "</area>";

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
    Vers      = MOD_VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;

    ExpFunc   = (SExpFunc *)ExpFuncLc;
    NExpFunc  = sizeof(ExpFuncLc)/sizeof(SExpFunc);

    m_res = ResAlloc::ResCreate( );
    
    // Init html headers
    w_head_ = "</html>\n";
    w_body_ = "<hr width=\"100%\" size=\"3\">\n"
    	"</body>\n";    
}

TWEB::~TWEB()
{
    ResAlloc res(m_res,true);
    while( m_auth.size() )
    { 
	delete m_auth[0]; 
	m_auth.erase(m_auth.begin()); 
    }
    res.release();
    
    ResAlloc::ResDelete( m_res );
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

string TWEB::opt_descr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),I18N(
	"=========================== The module options ===========================\n"
	"------------ Parameters of module <%s> in config file ------------\n"
	"ses_t_life <time>      set the sesion time life, minets (default 10);\n"
	),NAME_MODUL);

    return(buf);
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TWEB::mod_UpdateOpt()
{
    try{ m_t_auth = atoi( mod_XMLCfgNode()->get_child("ses_t_life")->get_text().c_str() ); }
    catch(...) {  }
}

string TWEB::w_ok( )
{
    return("HTTP/1.0 200 OK\nContent-type: text/html; charset="+Mess->charset()+"\n\n");
}

string TWEB::w_head( )
{
    return("<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'>\n"
	"<html>\n"
	"<head>\n"
    	"<meta HTTP-EQUIV='Content-Type' CONTENT='text/html; charset="+Mess->charset()+"'>\n"
    	"<title>"+PACKAGE_NAME+". "+I18N(DESCRIPTION)+
	"</title>\n"
	"</head>\n");
}

string TWEB::w_body( )
{
    return(static_cast<string>("<body bgcolor=#818181 text=#000000 link=#3366ff vlink=#339999 alink=#33ccff>\n"
    	"<h1 align='center'><font color=#ffff00>")+PACKAGE_NAME+". "+I18N(DESCRIPTION)+"</font></h1>\n"
    	"<hr width='100%' size='3'><br>\n");
}

void TWEB::HttpGet(string &url, string &page, string &sender, vector<string> &vars )
{
    page = w_ok()+w_head()+w_body();
    
    try
    {
	url = url_encode(url);
	if( SYS->ctr_path_l(url,0) == "about" ) get_about(page);
	else
	{
	    string ses_user = check_ses( atoi(get_cookie( "oscd_u_id", vars ).c_str()) );
	    if( ses_user.size() ) get_info( url, page, *SYS, static_cast<string>("/")+NAME_MODUL, ses_user, sender );
	    else                  get_auth( url, page );
	}
    }catch(TError err) 
    { 
	m_put_s("SYS",MESS_CRIT,err.what()); 
	post_mess(page,"System error: "+err.what(),3);	
    }
    
    down_colont( url, page, sender, vars );
    page = page+w_body_+w_head_;
}

void TWEB::get_about( string &page )
{
    page = page+"<h1 align='center'><font color='White'><i>"+I18N("About")+"</i></font></h1>\n"
	"<table border='1' align='center'>\n"
	"<TR bgcolor='#8EE5EE' align='center'><TD>"+PACKAGE+" "+VERSION+"</TD></TR>\n"
	"<TR bgcolor='#cccccc'><TD>"+I18N("Open Supervisory Control And Data Acquisition")+"<br>\n"
	"<font color='Blue'>"+I18N("License: ")+"</font>GPL<br>\n"
	"<font color='Blue'>"+I18N("Autors: ")+"</font>Roman Savochenko<br>\n"
	"</TD></TR></table><br>\n"   
	"<table border='1' align='center'>\n"
	"<TR bgcolor='#8EE5EE' align='center'><TD>"+NAME_MODUL+" "+MOD_VERSION+"</TD></TR>\n"
	"<TR bgcolor='#cccccc'><TD>"+I18N(DESCRIPTION)+"<br>"
	"<font color='Blue'>"+I18N("License: ")+"</font>"+I18N(LICENSE)+"<br>"
       	"<font color='Blue'>"+I18N("Autors: ")+"</font>"+I18N(AUTORS)+"<br>"
        "</TD></TR>\n</table><br>\n";             
}

void TWEB::get_info( string &url, string &page, TContr &cntr, string path, string ses_user, string &sender )
{         
    XMLNode node;
    cntr.ctr_info(node);
    if( url.size() > 1 ) 
    {	
        int n_dir = url.find("/",1); 
	if( n_dir == string::npos ) 
	{
	    url=url+"/";
	    n_dir = url.find("/",1); 
	}
    	string br_s = url.substr(1,n_dir-1);
    	string br_p;
	//Find branch list
	XMLNode *br_list = &node;
	int an_dir = 1;
	try
	{ 
	    br_p = url_encode(br_s,true);
    	    while(true)
    	    {
    		int t_dir = br_p.find("/",an_dir);
    		if( t_dir == string::npos ) throw TError("Branch <%s> error!",br_p.c_str());
    		br_list = cntr.ctr_id(br_list,br_p.substr(an_dir,t_dir-an_dir));
    		an_dir = t_dir+1;
    		if(br_list->get_name() == "list" && br_list->get_attr("tp")=="br") break;
    	    }
	    
     	    string n_url = url.substr(n_dir,url.size()-n_dir);  
    	    if( br_list->get_attr("mode") == "att" )
    	    {
    		unsigned hd = cntr.ctr_att( br_p );			
    		get_info( n_url, page, cntr.ctr_at(br_p,hd), path+"/"+br_s, ses_user, sender ); 
    		cntr.ctr_det(br_p,hd);
    	    }
    	    else get_info( n_url, page, cntr.ctr_at(br_p), path+"/"+br_s, ses_user, sender );
	}
	catch(TError err) { post_mess(page,"URL: "+err.what(),3); } 	
	return;
    }
    
    
    get_head( node, cntr, page, path, ses_user, sender );
    get_area( node, node, cntr, page, path,"/" , ses_user );       
}

void TWEB::get_head( XMLNode &root, TContr &cntr, string &page, string path, string ses_user, string &sender )
{
    page = page+ "<table width='100%' align='center' border=1 bgcolor=#6495ED><tr>\n"
	"<td width='10%' align='center' bgcolor=#cccccc nowrap>\n"
	"<a href='/"+NAME_MODUL+"'>"+I18N("Root page")+"</a><br>\n"
	"<a href='"+path+"'>"+I18N("Curent page")+"</a><br>\n"
	"<a href='"+path.substr(0,path.rfind("/"))+"'>"+I18N("Previos page")+"</a><br>\n"
	"<a href='/"+NAME_MODUL+"/about'>"+I18N("About")+"</a><br>\n"
	"</td>\n"
	"<td align='center' bgcolor=#cccccc><font size='+3'><b><i>"+root.get_text()+"</i></b></font></td>\n"
	"<td width='120' align='left'";
    if(ses_user == "root") page = page + " bgcolor=red";
    else                   page = page + " bgcolor=LawnGreen";
    page = page+" nowrap>"+I18N("user:")+" <b>"+ses_user+"</b><br>"+I18N("from:")+" <b>"+sender+"</b>\n"
	"<form action='"+path+"' method='POST' enctype='multipart/form-data'>\n"
	"<input name='auth_ch' type='submit' value='"+I18N("Change user")+"'/>\n"
	"</form></td>\n"
	"</tr></table><br>\n";    
}

void TWEB::get_area( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string path, string a_path, string ses_user )
{
    unsigned i_cf,c_cfg;    
    bool     wr;
    string   area_path;
    
    for( i_cf = 0; i_cf < node.get_child_count(); i_cf++)
    {
	XMLNode *t_s = node.get_child(i_cf);
	if( t_s->get_name() == "area" && chk_access(t_s, ses_user, SEQ_RD) )
	{
	    wr = false;
	    area_path = a_path+t_s->get_attr("id")+'/';
	    page = page+"<table width=100% border=2 bgcolor=#A9A9A9>\n"
		"<tr bgcolor=#9999ff><td><font size='+1'><b><i><u>"+t_s->get_attr("dscr")+
		"</u></b></i></font><br></td></tr>\n";
	    
	    page = page+"<tr bgcolor=#CCCCCC><td>";
	    //Get area's fields
	    c_cfg = 0;
	    bool f_open = false;
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( !chk_access(t_c, ses_user, SEQ_RD) ) continue;
		if( t_c->get_name() == "fld" )
		{
		    // First element
		    if(c_cfg++ == 0)
		    {
			page = page+"<form action='"+path+"' method='POST' enctype='multipart/form-data'>\n" 
			    "<table><tbody>\n";
		    }
		    // Start full element
		    if( t_c->get_attr("dscr").size() )
		    {
			if(f_open) page = page+"</td></tr>";
			page = page+"<tr><td valign='top'>"+t_c->get_attr("dscr")+":</td><td>";
			f_open = true;
		    }
    		    wr |= get_val(root,*t_c,cntr,page,path,area_path+t_c->get_attr("id"),ses_user);
		}		
	    }
	    if(f_open) page = page+"</td></tr>";
	    if(c_cfg > 0)
	    {
		page = page+"</tbody></table><br>\n";
		if(wr) page = page+"<input type='submit' name='apply:"+area_path+"' value='"+I18N("Apply")+"'>\n";    // Submit button 
		page = page+"</form>\n"
		    "<hr width='100%' size='1'>\n";
	    }

	    //Get area's lists
	    c_cfg = 0;
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( !chk_access(t_c, ses_user, SEQ_RD) ) continue;
		if( t_c->get_name() == "list" && t_c->get_attr("hide") != "1" )
		{
		    c_cfg++;		    
		    page = page+t_c->get_attr("dscr")+":<br>";
    		    get_val(root,*t_c,cntr,page,path,area_path+t_c->get_attr("id"),ses_user);
		}
	    }	    
	    if(c_cfg > 0) page = page+"<hr width='100%' size='1'>\n";
	    
	    //Get area's tables
	    c_cfg = 0;
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
		XMLNode *t_c = t_s->get_child(i_el);
		if( !chk_access(t_c, ses_user, SEQ_RD) ) continue;
		if( t_c->get_name() == "table" )
		{
		    c_cfg++;		    
		    page = page+t_c->get_attr("dscr")+":<br>";
    		    get_val(root,*t_c,cntr,page,path,area_path+t_c->get_attr("id"),ses_user);
		}
	    }	    
	    if(c_cfg > 0) page = page+"<hr width='100%' size='1'>\n";
	    
            //Get area's commands
	    for( unsigned i_el = 0; i_el < t_s->get_child_count(); i_el++)
	    {
    		// First element
		XMLNode *t_c = t_s->get_child(i_el);
		if( !chk_access(t_c, ses_user, SEQ_RD) ) continue;
		if( t_c->get_name() == "comm" )
		    get_cmd(root,*t_c,cntr,page,path,area_path+t_c->get_attr("id"),ses_user);        
	    }	    
	    
    	    get_area( root, *t_s, cntr, page, path, area_path, ses_user);
	    page = page+"</td></tr></table><br>\n";
	}
    }
}

void TWEB::get_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string a_path, string ses_user )
{
    page = page+"<form action='"+path+"' method='POST' enctype='multipart/form-data'>\n" 
	"<input type='submit' name='comm:"+a_path+"' value='"+node.get_attr("dscr")+"'>\n";    
    int f_cfg=0;
    for( unsigned i_el=0; i_el < node.get_child_count(); i_el++)
    {
	XMLNode *t_c = node.get_child(i_el);
	if( t_c->get_name() == "fld" )
	{
	    if(f_cfg++ > 0) page = page+", ";
	    else            page = page+"<br>("; 
	    page = page+t_c->get_attr("dscr")+":";
	    get_val(root,*t_c,cntr,page,path,a_path,ses_user);
	}
    }
    if(f_cfg > 0) page = page+")<br>\n";

    page = page+"</form>\n"; 
}

bool TWEB::get_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string path, string a_path, string ses_user )
{
    bool wr = false;
    if( chk_access(&node, ses_user, SEQ_WR) ) wr = true;
    if( node.get_name() == "fld" )
    {
	cntr.ctr_din_get(a_path, &node);
 	if( node.get_attr("dest") == "select" && wr )
	{
	    XMLNode *x_lst = cntr.ctr_id(&root,node.get_attr("select"));
	    cntr.ctr_din_get( node.get_attr("select"), x_lst );	    
	    page = page+ "<select name='"+node.get_attr("id")+"'>";
	    if( x_lst->get_name() == "list" )
		for( unsigned i_el = 0, c_el = 0; i_el < x_lst->get_child_count(); i_el++ ) 
		    if( x_lst->get_child(i_el)->get_name() == "el")
		    {
			if( x_lst->get_attr("mode") == "st" )
			    page = page+"<option value='"+TSYS::int2str(c_el++)+"'";
			else
			    page = page+"<option value='"+x_lst->get_child(i_el)->get_text()+"'";
			if( x_lst->get_child(i_el)->get_text() == node.get_text() ) page = page+" selected";
			page = page+">"+x_lst->get_child(i_el)->get_text()+"</option>";
		    }
	    page = page+ "</select>\n";
	}
	else
	{
	    if( node.get_attr("tp") == "bool" )
	    {
		page = page+"<input type='checkbox' name='"+node.get_attr("id")+"'";
		if( node.get_text() == "true" ) page=page+" checked";
		if( !wr ) page=page+" disabled";
		page = page + ">\n";
	    }
	    else
	    {
		// Draw text area 
		if( node.get_attr("tp") == "str" && (node.get_attr("rows").size() || node.get_attr("cols").size()) )
		{
		    page = page+"<textarea cols='"+node.get_attr("cols")+"' rows='"+node.get_attr("rows")+"'";
		    if( !wr ) page=page+" readonly";
		    page = page+">"+mess2html(node.get_text())+"</textarea>\n";
		}
		else if( node.get_attr("tp") == "time" )
		{
		    struct tm *tm_tm;
		    time_t tm_t;
		    if( node.get_text().size() ) tm_t = strtol(node.get_text().c_str(),NULL,16);
		    else                         tm_t = time(NULL);
                    tm_tm = localtime(&tm_t);
		    if( !wr )
		    {
			char *c_tm = ctime( &tm_t );
			for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
			    if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
			page=page+"<b>"+c_tm+"</b>";
		    }
		    else
		    {
			string s_id = node.get_attr("id");		    
			page = page+"<input type='text' name='"+s_id+"_d' value='"+TSYS::int2str(tm_tm->tm_mday)+"' maxlength=2 size=2>\n";
			page = page+"<input type='text' name='"+s_id+"_ms' value='"+TSYS::int2str(tm_tm->tm_mon+1)+"' maxlength=2 size=2>\n";
			page = page+"<input type='text' name='"+s_id+"_y' value='"+TSYS::int2str(tm_tm->tm_year+1900)+"' maxlength=4 size=4>\n";
			page = page+ " , ";
			page = page+"<input type='text' name='"+s_id+"_h' value='"+TSYS::int2str(tm_tm->tm_hour)+"' maxlength=2 size=2>\n";
			page = page+"<input type='text' name='"+s_id+"_m' value='"+TSYS::int2str(tm_tm->tm_min)+"' maxlength=2 size=2>\n";
			page = page+"<input type='text' name='"+s_id+"_s' value='"+TSYS::int2str(tm_tm->tm_sec)+"' maxlength=2 size=2>\n";
		    }
		}		
		else
		{
		    // Other fields
		    if( !wr ) page=page+"<b>"+node.get_text()+"</b>";
		    else
		    {
			page = page+"<input type='text' name='"+node.get_attr("id")+"' value='"+node.get_text()+"'";
			// addon parameters
			int val_n = atoi(node.get_attr("len").c_str());
			if( val_n > 0 ) page = page + " maxlength="+TSYS::int2str(val_n)+" size="+TSYS::int2str((val_n>50)?50:val_n);			
			else if( node.get_attr("tp") == "dec" || 
				 node.get_attr("tp") == "hex" || 
				 node.get_attr("tp") == "oct" ||
				 node.get_attr("tp") == "real" )
			    page = page + " size=5";
			page = page + ">\n";
		    }
		}
	    }
	}
    }
    else if( node.get_name() == "list" )
    {       
	if( node.get_attr("tp") == "br" || wr )
	    page = page+"<form action='"+path+"' method='POST' enctype='multipart/form-data'>\n";
	    
	cntr.ctr_din_get(a_path,&node);
	int p_size = node.get_child_count();
	p_size = (p_size > 20)?20:(p_size < 4)?4:p_size;
	page = page+"<select name='"+node.get_attr("id")+"' size='"+TSYS::int2str(p_size)+"'>\n";
	for( unsigned i_lel = 0; i_lel < node.get_child_count(); i_lel++)
	{
	    XMLNode *t_c = node.get_child(i_lel);
	    if( t_c->get_name() == "el" )
		page = page+"<option value='"+t_c->get_attr("id")+":"+t_c->get_text()+"'>"+t_c->get_text()+"</option>\n";
	}
	page = page+"</select><br>\n";
	if( node.get_attr("tp") == "br" )	
	    page = page+"<input type='submit' name='list:"+a_path+"/go' value='"+I18N("Go")+"'><br>\n";   //Go branch command
 	if( wr )
	{
	    bool p_edit = false;
	    page = page+"<br>\n";
	    if( !node.get_attr("s_com").size() || node.get_attr("s_com").find("add") != string::npos )
	    {
    		page = page+"<input type='submit' name='list:"+a_path+"/add' value='"+I18N("Add")+"'>\n";    //Add element to list            
		p_edit = true;		
	    }
	    if( !node.get_attr("s_com").size() || node.get_attr("s_com").find("ins") != string::npos )
	    {
    		page = page+"<input type='submit' name='list:"+a_path+"/ins' value='"+I18N("Insert")+"'>\n"; //Insert element to list            
		p_edit = true;		
	    }
	    if( !node.get_attr("s_com").size() || node.get_attr("s_com").find("edit") != string::npos )
	    {
		page = page+"<input type='submit' name='list:"+a_path+"/edit' value='"+I18N("Edit")+"'>\n";   //Edit list element 
		p_edit = true;		
	    }	    
	    if( p_edit )
	    {
		node.set_name("fld");
		node.set_attr("id","ener_f");
		get_val( root, node, cntr, page, node.get_attr("id"),"", ses_user );
	    	page = page+"<br>\n";
	    }
	    
	    if( !node.get_attr("s_com").size() || node.get_attr("s_com").find("del") != string::npos )
		page = page+"<input type='submit' name='list:"+a_path+"/del' value='"+I18N("Del")+"'>\n";    //Del element from list	    
	}
	if( node.get_attr("tp") == "br" || wr )
	    page = page+"</form>\n";
    }
    else if( node.get_name() == "table" )
    {
	if( node.get_attr("tp") == "flow" )
	{
            //Get table's commands
	    for( unsigned i_el = 0; i_el < node.get_child_count(); i_el++)
	    {
		XMLNode *t_c = node.get_child(i_el);
		if( t_c->get_name() == "comm" )
		    get_cmd(root,*t_c,cntr,page,path,a_path+"/"+t_c->get_attr("id"),ses_user);        
	    }
	}	
    }
    else if( node.get_name() == "value" )
    {
    }
    else if( node.get_name() == "conf" )
    {
    }
    
    return(wr);
}

void TWEB::get_auth( string &url, string &page )
{
    page = page+"<table border=2 width=40% align='center'>"
	       "<tr bgcolor=#9999ff><td><b>"+I18N("Enter to module")+"</b></td></tr>\n"
	       "<tr bgcolor=#cccccc> <td align=center><table cellpadding=3>"
	       "<form method=POST action='/"+NAME_MODUL+url+"' enctype='multipart/form-data'>\n"
	       "<tr> <td><b>"+I18N("User name")+"</b></td> <td><input type=text name=user size=20></td> </tr>\n"
	       "<tr> <td><b>"+I18N("Password")+"</b></td> <td><input type=password name=pass size=20></td> </tr>\n"
	       "<tr> <td colspan=2 align=center><input type=submit name='auth_enter' value='"+I18N("Enter")+"'>\n"
	       "<input type=reset name='clean' value='"+I18N("Clean")+"'>\n"
	       "</td> </tr>\n"
	       "</form>\n"
	       "</table></td></tr></table>\n";
}

void TWEB::HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein )
{
    bool my = false, err = false;
    int  kz;
    string ses_user;
    
    page = w_ok()+w_head()+w_body();
    try
    {
	url = url_encode(url);
	// Check autentification POST request
	if( !my )
	{
	    kz = post_auth( url, page, vars, contein, ses_user );    
	    if( kz&0x01 ) my  = true;
	    if( kz&0x02 ) err = true;
	}
	// Check avoid sesion
	if( !my )
	{
	    ses_user = check_ses( atoi(get_cookie( "oscd_u_id", vars ).c_str()) );
	    if( ses_user.size() )
	    {
		kz = post_info( url, page, *SYS, static_cast<string>("/")+NAME_MODUL, ses_user, sender, contein, vars );
		if( kz&0x01 ) my  = true;
		if( kz&0x02 ) err = true;
	    }
	    else 
	    { 
		get_auth( url, page ); 
		my  = true; 
		err = true; 
	    }
	}
	//Request error
	if( !my )       post_mess(page,"Post request broken!",3);
	else if( !err ) get_info( url, page, *SYS, static_cast<string>("/")+NAME_MODUL, ses_user, sender );
    }catch(TError err) 
    { 
	m_put_s("SYS",MESS_CRIT,err.what()); 
	post_mess(page,"System error: "+err.what(),3);	
    }
    
    down_colont( url, page, sender, vars );
    page = page+w_body_+w_head_;
}

int TWEB::post_info( string &url, string &page, TContr &cntr, string path, string ses_user, string &sender , string &contein, vector<string> &vars )
{         
    int kz=0;

    XMLNode node;
    cntr.ctr_info(node);
    if( url.size() > 1 ) 
    {	
        int n_dir = url.find("/",1); 
	if( n_dir == string::npos ) 
	{
	    url=url+"/";
	    n_dir = url.find("/",1); 
	}
    	string br_s = url.substr(1,n_dir-1);
    	string br_p;
        //Find branch list
	XMLNode *br_list = &node;
	int an_dir = 1;
	try
	{ 
	    br_p = url_encode(br_s,true);
    	    while(true)
    	    {
    		int t_dir = br_p.find("/",an_dir);
    		if( t_dir == string::npos ) throw TError("Branch <%s> error!",br_p.c_str());
    		br_list = cntr.ctr_id(br_list,br_p.substr(an_dir,t_dir-an_dir));
    		an_dir = t_dir+1;
    		if(br_list->get_name() == "list" && br_list->get_attr("tp")=="br") break;
    	    }
	    
     	    string n_url = url.substr(n_dir,url.size()-n_dir);  
    	    if( br_list->get_attr("mode") == "att" )
    	    {
    		unsigned hd = cntr.ctr_att( br_p );			
		kz = post_info( n_url, page, cntr.ctr_at(br_p, hd), path+"/"+br_s, ses_user, sender, contein, vars);  
    		cntr.ctr_det(br_p, hd);
    	    }
    	    else kz = post_info( n_url, page, cntr.ctr_at(br_p), path+"/"+br_s, ses_user, sender, contein, vars );  
	}
	catch(TError err) 
	{ 
    	    post_mess(page,"URL: "+err.what(),3);
    	    return(0x01|0x02); 
	} 	
	return(kz);
    }
    
    //Parse post category and path to area
    string prs_cat, prs_path;
    vector<string> names, vals;
    cont_frm_data( contein, vars, names, vals );
    unsigned i_el;
    for( i_el = 0; i_el < names.size(); i_el++)
	if( names[i_el].find("apply:",0) == 0 || 
	    names[i_el].find("comm:",0) == 0 ||
	    names[i_el].find("br:",0) == 0 ||
	    names[i_el].find("list:",0) == 0 ) break;
    if( i_el < names.size() )
    {
	int psep = names[i_el].find(":",0);
	prs_cat  = names[i_el].substr(0,psep);
	prs_path = names[i_el].substr(psep+1,names[i_el].size()-psep+1);
       	names.erase(names.begin()+i_el);
       	vals.erase(vals.begin()+i_el);
    }
    return(post_area( node, node, cntr, page, ses_user, sender, names, vals, path, prs_cat, prs_path ));
}

int TWEB::post_area( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &sender, vector<string> &name, vector<string> &val, string path, string prs_cat, string prs_path, int level )
{
    if( !cntr.ctr_path_l(prs_path,level).size() ) return(0x00);
    try
    {
	XMLNode *t_nd = cntr.ctr_id(&node, cntr.ctr_path_l(prs_path,level));
	if( prs_cat == "apply" && !cntr.ctr_path_l(prs_path,level+1).size() )
	    return( post_val(root,*t_nd,cntr,page,ses_user,name,val,prs_path) );
	else if( prs_cat == "comm" && !cntr.ctr_path_l(prs_path,level+1).size() )
	{
	    if( node.get_name() == "table" )
	    {
		int kz = post_cmd(root,*t_nd,node,cntr,page,ses_user,name,val,prs_path);
		if( kz&0x02 ) return(kz);		
		//Restore path to object (no command)
		//string a_path;
		//for( int i_p = 0; i_p < level; i_p++) a_path = a_path+"/"+cntr.ctr_path_l(prs_path,i_p);
		get_head( root, cntr, page, path, ses_user, sender );
		return( post_table(root,node,cntr,page,ses_user,name,val,path,prs_path.substr(0,prs_path.rfind("/"))) );
	    }
	    else return(post_cmd(root,*t_nd,*t_nd,cntr,page,ses_user,name,val,prs_path));
	}
    	else if( prs_cat == "list" && t_nd->get_name() == "list" )
	    return( post_list(root,*t_nd,cntr,page,ses_user,name,val,path,prs_path) );
	return(post_area( root, *t_nd, cntr, page, ses_user, sender, name, val, path, prs_cat, prs_path, ++level ));
    }
    catch(TError err)
    {
	post_mess(page,"Error! "+err.what(),3);
	return(0x01|0x02);
    }	
}

int TWEB::post_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &name, vector<string> &val, string prs_path)
{
    // Free no changed elements (polimorfic problem fix)    
    for( unsigned i_cf = 0; i_cf < node.get_child_count(); i_cf++)
    {
	XMLNode *t_c = node.get_child(i_cf);
	if( (t_c->get_name() == "fld") && chk_access(t_c, ses_user, SEQ_RD|SEQ_WR) )
	{
	    if( t_c->get_attr("tp") != "bool" && !prepare_val( root, *t_c, cntr, page, ses_user,name,val,prs_path, true ) )
		for( unsigned i_cnt = 0; i_cnt < name.size(); i_cnt++ )
		    if( name[i_cnt] == t_c->get_attr("id") )
	    	    {
			name.erase( name.begin()+i_cnt );
			val.erase( val.begin()+i_cnt );
			break;
		    }
	}
    }    
    // Check and modify
    for( unsigned i_cf = 0; i_cf < node.get_child_count(); i_cf++)
    {
	XMLNode *t_c = node.get_child(i_cf);
	if( (t_c->get_name() == "fld") && chk_access(t_c, ses_user, SEQ_RD|SEQ_WR) )
	{
	    if( !prepare_val( root, *t_c, cntr, page, ses_user,name,val,prs_path, true ) ) 
		continue;
	    string kz = cntr.chk_opt_val(t_c,true);
	    if( !kz.size() )
	    {
		try
		{ 	    
		    m_put("CONTROL",MESS_INFO,"%s| Change <%s:%s> to %s",
		    	ses_user.c_str(),
			t_c->get_attr("id").c_str(),
			t_c->get_attr("dscr").c_str(),
			t_c->get_text().c_str());
		    cntr.ctr_din_set(prs_path+t_c->get_attr("id"),t_c); 
		    continue; 
		}
		catch(TError err)
		{
		    post_mess(page,err.what(),3);	
		    return(0x01|0x02);
		}
	    }
	    else 
	    {
		post_mess(page,kz,2);
		return(0x01|0x02);
	    }
	}
    }    
    return(0x01);
}

int TWEB::post_cmd( XMLNode &root, XMLNode &node, XMLNode &rez, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string prs_path )
{ 
    // Prepare command options
    try
    { 	    
	for( int i_prm = 0; i_prm < node.get_child_count(); i_prm++ )
	{
	    XMLNode *t_c = node.get_child(i_prm);
	    if( t_c->get_name() == "fld" )
	    prepare_val( root, *t_c, cntr, page, ses_user, names, vals, "", false );
	    string kz = cntr.chk_opt_val(t_c,true);
	    if( kz.size() ) throw TError(kz);
	}	    
	m_put("CONTROL",MESS_INFO,"%s| Put command <%s:%s>",
	    ses_user.c_str(),
	    node.get_attr("id").c_str(),
	    node.get_attr("dscr").c_str());
	cntr.ctr_cmd_go(prs_path,&node,&rez);
	return( 0x01 );
    }
    catch(TError err)
    {
    	post_mess(page,err.what(),3);	
	return(0x01|0x02);
    }
}

int TWEB::post_list( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string path, string prs_path )
{ 
    string l_path = "/", nd_nm, l_com, ener_f;
    //Get command name and path
    int c_pos = 1;
    while(true)
    {
	int t_pos = prs_path.find("/",c_pos);
	if( t_pos == string::npos )
	{
	    l_com = prs_path.substr(c_pos,t_pos-c_pos);
	    break;
	}
	l_path = l_path + prs_path.substr(c_pos,t_pos-c_pos)+"/";
	c_pos = t_pos+1;	
    }
    //Get new element value
    for( int i_le = 0; i_le < names.size(); i_le++ )
	if( names[i_le] == "ener_f" ) 
	{
	    ener_f = vals[i_le];		
	    break;
	}
    //Command 
    if( l_com == "go" || l_com == "ins" || l_com == "edit" || l_com == "del" )
    {
	//Check select list element
	string l_el;
	for( int i_le = 0; i_le < names.size(); i_le++ )
	    if( names[i_le] == node.get_attr("id") ) 
	    {
		l_el = vals[i_le];		
		break;
	    }
	if( !l_el.size() )
	{
	    post_mess(page,"No select list element for list <"+node.get_attr("dscr")+">!",2);
	    return( 0x01|0x02 );
	}
	c_pos = l_el.find(":",0);
	string i_el = l_el.substr(0,c_pos);
	l_el = l_el.substr(c_pos+1,l_el.size()-c_pos+1);
	
	if( l_com == "go" )
	{ 
	    string url;
	    if( node.get_attr("mode") == "at" || node.get_attr("mode") == "st")
		url = path+"/"+url_code(l_path+i_el,true);
	    else
		url = path+"/"+url_code(l_path+l_el,true);
            page = page + "<META HTTP-EQUIV='Refresh' CONTENT='0; URL="+url+"'>\n";
	    post_mess( page, "Go to <"+url+"> !",1);
	    return( 0x01|0x02 );  //No error. That no draw curent page
	}
	else if( l_com == "del" )
	{
	    XMLNode *n_el = node.add_child("el");
	    n_el->set_attr("do",l_com,true);
	    n_el->set_attr("id",i_el,true);
	    n_el->set_text(l_el);
	    m_put("CONTROL",MESS_INFO,"%s| Put command %s(%s) to <%s:%s>",
		ses_user.c_str(), 
		l_com.c_str(), l_el.c_str(),
		node.get_attr("id").c_str(), 
		node.get_attr("dscr").c_str());
	}
	else if( l_com == "ins" || l_com == "edit" )
	{
	    XMLNode *n_el = node.add_child("el");
	    n_el->set_attr("do",l_com,true);
	    n_el->set_attr("id",i_el,true);
	    n_el->set_text(ener_f);
	    m_put("CONTROL",MESS_INFO,"%s| Put command %s(%s) to <%s:%s>",
		ses_user.c_str(), 
		l_com.c_str(), ener_f.c_str(),
		node.get_attr("id").c_str(), 
		node.get_attr("dscr").c_str());
	}	
    }
    else if( l_com == "add" )
    {
	XMLNode *n_el = node.add_child("el");
	n_el->set_attr("do",l_com,true);
	n_el->set_text(ener_f);
	m_put("CONTROL",MESS_INFO,"%s| Put command %s(%s) to <%s:%s>",
    	    ses_user.c_str(), 
	    l_com.c_str(), ener_f.c_str(),
	    node.get_attr("id").c_str(), 
	    node.get_attr("dscr").c_str());    
    }
    cntr.ctr_din_set(l_path,&node); 

    return( 0x01 );
} 

int TWEB::post_table( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string path, string prs_path )
{		
    int  cols=0;
    
    page = page + "<table border=2>\n";
    page = page + "<tr bgcolor=#9999ff><td>"+node.get_attr("dscr")+"</td></tr>\n";
    page = page + "<tr bgcolor=#cccccc><td><br>\n";
    get_val(root,node,cntr,page,path,prs_path,ses_user);
	
    page = page + "<br><table border=1>\n";
    //Draw head
    page = page + "<tr bgcolor=#FFA07A>";
    for( int i_cl=0; i_cl < node.get_child_count(); i_cl++)
    {
	XMLNode *t_c = node.get_child(i_cl);
	if( t_c->get_name() == "list" )
	{
	    cols++;
	    page = page + "<td>"+mess2html(t_c->get_attr("dscr"))+"</td>";
	}
    }
    page = page + "</tr>\n";
    
    int i_ln=0;
    while(true)
    {
	try
	{
    	    page = page + "<tr bgcolor=#cccccc>";
    	    for( int i_cl=0; i_cl < cols; i_cl++)
    	    {
		XMLNode *x_lst = cntr.ctr_id(&node,TSYS::int2str(i_cl)); 
		XMLNode *x_el  = cntr.ctr_id(x_lst,TSYS::int2str(i_ln));
		if( x_lst->get_attr("tp") == "time" )
		{
		    time_t tm_t = strtol(x_el->get_text().c_str(),NULL,16);
	    	    char *c_tm = ctime( &tm_t );
	    	    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
    			if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
		    page = page + "<td nowrap>";
		    page=page+c_tm;
		}
		else 
		{
		    page = page + "<td>";
		    page = page + mess2html(x_el->get_text());
		}
		page = page + "</td>";
	    }
	    page = page + "</tr>\n";
	}
	catch(...){ break; }
	i_ln++;
    }
    page = page + "</table>\n";
    
    page = page + "</td></tr></table>\n";

    return(0x01|0x02);  //No error! Draw table.
}

void TWEB::post_mess( string &page, string mess, int type )
{
    page = page+"<table border=2 width=40% align='center'><tbody>\n";
    if(type == 2 )      
	page = page+"<tr bgcolor=yellow><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == 3 ) 
	page = page+"<tr bgcolor=red><td align='center'><b>Error!</b></td></tr>\n";
    else page = page+"<tr bgcolor=#9999ff><td align='center'><b>Message!</b></td></tr>\n"; 
    page = page+"<tr bgcolor='#cccccc'> <td align='center'>"+mess2html(mess)+"</td></tr>\n";
    page = page+"</tbody></table>\n";
}

bool TWEB::prepare_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string prs_path, bool compare )
{
    unsigned i_cnt;
    string   val;
    
    if( node.get_attr("tp") == "bool" )
    {    	
	for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	    if( names[i_cnt] == node.get_attr("id") ) break;
	if( i_cnt < names.size() ) val = "true";
	else                       val = "false";
	if( compare )
	{
	    cntr.ctr_din_get(prs_path+node.get_attr("id"),&node); 
	    if( node.get_text() == val) return(false);   //No change fld 
	}
    }
    else if( node.get_attr("tp") == "time" )
    {
	struct tm *tm_tm = new tm;
	string s_id = node.get_attr("id");
	
	for( i_cnt = 0, val = ""; i_cnt < names.size(); i_cnt++ )
	{
	    if( names[i_cnt] == s_id+"_d" )       tm_tm->tm_mday = atoi( vals[i_cnt].c_str() );
	    else if( names[i_cnt] == s_id+"_ms" ) tm_tm->tm_mon  = atoi( vals[i_cnt].c_str() )-1;
	    else if( names[i_cnt] == s_id+"_y" )  tm_tm->tm_year = atoi( vals[i_cnt].c_str() )-1900;
	    else if( names[i_cnt] == s_id+"_h" )  tm_tm->tm_hour = atoi( vals[i_cnt].c_str() );
	    else if( names[i_cnt] == s_id+"_m" )  tm_tm->tm_min  = atoi( vals[i_cnt].c_str() );
	    else if( names[i_cnt] == s_id+"_s" )  tm_tm->tm_sec  = atoi( vals[i_cnt].c_str() );	    
	}
	val = TSYS::int2str(mktime(tm_tm),C_INT_HEX);
	if( compare )
	{
	    cntr.ctr_din_get(prs_path+node.get_attr("id"),&node); 
	    if( node.get_text() == val) return(false);   //No change time 
	}

	delete tm_tm;
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
		cntr.ctr_din_get(prs_path+node.get_attr("id"),&node); 
		if( node.get_text() == val) return(false);   //No change fld 
	    }
	}
	else return(false);
    } 
    node.set_text(val);

    return(true);
}


int TWEB::post_auth( string &url, string &page, vector<string> &vars, string &contein, string &user )
{
    unsigned i_cnt; 
    //Check Auth entry
    vector<string> names, vals;
    cont_frm_data( contein, vars, names, vals );
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
		page = static_cast<string>("HTTP/1.0 200 OK\nContent-type: text/html\nSet-Cookie: oscd_u_id=")+TSYS::int2str(open_ses(user))+"; path=/;\n\n";
		page = page+w_head()+w_body();
		Owner().Owner().Sequrity().usr_det(hd); 	    
		return( 0x01 );
	    }
	    Owner().Owner().Sequrity().usr_det(hd); 	    
	}
	catch(TError err){ m_put("SYS",MESS_WARNING,"Auth %s!",err.what().c_str()); }
	
	post_mess(page,"Auth wrong! Retry please.",3);
	page = page+"\n";
	get_auth( url, page );
	return( 0x01|0x02 );
    }    
    
    for( i_cnt = 0; i_cnt < names.size(); i_cnt++ )
	if( names[i_cnt] == "auth_ch" ) break;	
    if( i_cnt < names.size() )
    {
	get_auth( url, page );
	return( 0x01|0x02 );    
    }
    
    return( 0x00 );
}

int TWEB::open_ses( string name )
{    
    SAuth *Auth;    
    bool  n_ses = true;
    //Check sesion and close old sesion
    ResAlloc res(m_res,false);
    for( int i_s = 0; i_s < m_auth.size(); i_s++ )
	if( time(NULL) > (m_auth[i_s]->t_auth+m_t_auth*60) ) 
	{
	    delete m_auth[i_s];
	    m_auth.erase(m_auth.begin() + i_s);
	    i_s--;
	}
    //Find opened sesion
    for( int i_s = 0; i_s < m_auth.size(); i_s++ )
	if( name == m_auth[i_s]->name ) 
	{
	    Auth = m_auth[i_s];
	    Auth->t_auth = time(NULL);
	    n_ses = false;
	    break;
	}
    res.release();
    
    if(n_ses)
    {
	//Make new sesion    
	Auth = new SAuth();
	Auth->t_auth = time(NULL);
	Auth->name   = name;
	Auth->id_ses = rand();

	res.request(m_res,true);
	m_auth.push_back( Auth );
	res.release( );
    }
	    
    return(Auth->id_ses);
}

string TWEB::check_ses( int id )
{    
    string t_str;
    //Check sesion and close old sesion
    ResAlloc res(m_res,false);
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
    
    return( t_str );
}

void TWEB::cont_frm_data( const string &content, vector<string> &vars, vector<string> &name, vector<string> &val )
{
    int pos = 0, i_bnd;
    string boundary;
    char *c_bound = "boundary=";
    char *c_term = "\r\n";
    char *c_end = "--";
    char *c_fd = "Content-Disposition: form-data;";
    char *c_name = "name=\"";    
    char *c_file = "filename=\"";    
    
    for( int i_vr = 0; i_vr < vars.size(); i_vr++ )
	if( vars[i_vr].substr(0,vars[i_vr].find(":",0)) == "Content-Type" )
	{
	    int pos = vars[i_vr].find(c_bound,0)+strlen(c_bound);
	    boundary = vars[i_vr].substr(pos,vars[i_vr].size()-pos);
	}
    if( !boundary.size() ) return;
    
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
	i_bnd = c_head.find(c_file,0);
	if( i_bnd == string::npos )
	{	
    	    //Get value
    	    pos += c_head.size()+(2*strlen(c_term));
	    if(pos >= content.size()) break;
    	    string c_val  = content.substr(pos, content.find(c_term,pos)-pos);
    	    //Save
    	    name.push_back(c_name);
    	    val.push_back(c_val);
	}
	else
	{
    	    name.push_back(c_name);
	    i_bnd += strlen(c_file);
    	    val.push_back(c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd));
	}
	//m_put("DEBUG",MESS_DEBUG,"%s:%s",name[name.size()-1].c_str(),val[val.size()-1].c_str());
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

string TWEB::mess2html( string mess )
{    
    string rez,s_tmp;
    int i_pr, i_c;
    
    for( i_c = 0, i_pr = 0; i_c < mess.size(); i_c++ )
    {
	if(mess[i_c] == '>')      s_tmp = "&gt;";
	else if(mess[i_c] == '<') s_tmp = "&lt;";
	else if(mess[i_c] == '"') s_tmp = "&quot;";
	else if(mess[i_c] == '&') s_tmp = "&amp;";
	if( s_tmp.size() )
	{
	    rez = rez+mess.substr(i_pr,i_c-i_pr)+s_tmp;
	    s_tmp = "";
	    i_pr=i_c+1;
	}
    }
    rez = rez+mess.substr(i_pr,i_c-i_pr);

    return(rez);	
}

string TWEB::url_code( string url, bool contr )
{
    char buf[4];
    for( int i_c = 0; i_c < url.size(); i_c++ )
    {
	if(url[i_c] == '/' && contr)      url[i_c] = ':';	
	else if(url[i_c] == ' ') url.replace(i_c,1,"%20");
	else if(url[i_c]&0x80)
	{
	    snprintf(buf,sizeof(buf),"%%%2X",(unsigned char)url[i_c]);
	    url.replace(i_c,1,buf);
	}
    }

    return(url);	
}

string TWEB::url_encode( string url, bool contr )
{
    int n_pos, o_pos = 0;
    string rez;
    
    while(true)
    {
	n_pos = url.find("%",o_pos);
	if( n_pos == string::npos )
	{	 
	    rez=rez+url.substr(o_pos,n_pos-o_pos);
	    break;
	}
	rez=rez+url.substr(o_pos,n_pos-o_pos);
	rez=rez+(char)strtol(url.substr(n_pos+1,2).c_str(),NULL,16);
	o_pos=n_pos+3;
    }
    if( contr )
    	for( int i_c = 0; i_c < rez.size(); i_c++ )
    	    if(rez[i_c] == ':') rez[i_c] = '/';
	
    return(rez);
}

void TWEB::down_colont( string &url, string &page, string &sender, vector<string> &vars )
{
    // Draw path
    page = page+"<hr width='100%' size='2'>"+url+"<br>\n";
}

//================== Controll functions ========================
void TWEB::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TUI::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,I18N("Self modul"));
    n_add->get_child(0)->set_attr(dscr,I18N("Life time of auth sesion(min)"));
    n_add->get_child(1)->set_attr(dscr,I18N("Options help"));
}

void TWEB::ctr_din_get_( string a_path, XMLNode *opt )
{
    TUI::ctr_din_get_( a_path, opt );

    string t_id = ctr_path_l(a_path,0);
    if( t_id == "bs" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "lf_tm" )       ctr_opt_setI( opt, m_t_auth );
	else if( t_id == "o_help" ) ctr_opt_setS( opt, opt_descr() );       
    }
}

void TWEB::ctr_din_set_( string a_path, XMLNode *opt )
{
    TUI::ctr_din_set_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "bs" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "lf_tm" ) m_t_auth = ctr_opt_getI( opt );
    }
}

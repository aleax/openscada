#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "web_info.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_info"
#define NAME_TYPE   "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define VERSION     "0.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web info interface for http protocol"
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
	WebInfo::TWEB *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new WebInfo::TWEB( source );

	return ( self_addr );
    }
}

using namespace WebInfo;

//==============================================================================
//================ WebInfo::TWEB ===============================================
//==============================================================================
SExpFunc TWEB::ExpFuncLc[] =
{
    {"HttpGet",(void(TModule::*)( )) &TWEB::HttpGet,"void HttpGet(string &url, string &page, string &sender, vector<string> &vars );",
     "Process Get comand from http protocol's!",10,0},
    {"HttpPost",(void(TModule::*)( )) &TWEB::HttpPost,"void HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein);",
     "Process Post comand from http protocol's!",10,0}     
};

TWEB::TWEB( string name )
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
}

TWEB::~TWEB()
{

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

char *TWEB::w_head =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html> <head>\n"
    "  <title>OpenSCADA information web modul!</title>\n"
    " </head>\n";

char *TWEB::w_head_ =
    "</html>\n";

char *TWEB::w_body =
    " <body bgcolor=\"#2A4547\" text=\"#ffffff\" link=\"#3366ff\" vlink=\"#339999\" alink=\"#33ccff\">\n"
    "  <h1 align=\"center\"><font color=\"#ffff00\"> OpenSCADA information web modul!</font></h1>\n"
    "  <hr width=\"100%\" size=\"2\">\n";

char *TWEB::w_body_ =
    " </body>\n";        

void TWEB::HttpGet(string &url, string &page, string &sender, vector<string> &vars )
{
    get_info(url, page, *SYS, string("/")+NAME_MODUL );
}

void TWEB::get_info( string &url, string &page, TContr &cntr, string path )
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
			get_info( n_url, page, cntr.ctr_at(hd), path+"/"+br_s ); 
			cntr.ctr_det(hd);
		    }
		    else if( brs->get_attr("mode") == "at" )
			get_info( n_url, page, cntr.ctr_at(br), path+"/"+br_s ); 
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
    
    page = page+ w_head+w_body;
    page = page+ "<h2 align=\"center\"><font color=aqua>"
               + node->get_text()
	       + "</font></h2>\n";
	       
    page = page+"<table width=100% border=2><tr><td>";
    page = page+"<h3 align=\"left\"><font color=moccasin>Parameters</font></h3>\n";
    get_cfg( *node, cntr, page );
    page = page+"</td></tr></table>";
    
    page = page+"<table width=100% border=2><tr><td>";
    page = page+"<h3 align=\"left\"><font color=moccasin>Metods</font></h3>\n";
    get_cmd( *node, page );    
    page = page+"</td></tr></table>";
    
    page = page+"<table width=100% border=2><tr><td>";
    page = page+"<h3 align=\"left\"><font color=moccasin>Branches</font></h3>\n";
    get_branch( *node, cntr, page, path );
    page = page+"</td></tr></table>";
    
    page = page+"<hr width=\"100%\" size=\"2\">\n";
    
    page = page+"<table width=100%><tr><td width=50% valign=top align=left>"+path+"</td>";
    page = page+"<td align=right width=50%>Version: "+VERSION+"<br>Autors: "+AUTORS+"<br>License: "+LICENSE+"</td>";

    page = page+ w_body_+w_head_;
    
    delete node;
}
	    
void TWEB::get_cfg( XMLNode &node, TContr &cntr, string &page )
{
    unsigned i_cf,c_cfg;
    
    for( i_cf = 0, c_cfg = 0; i_cf < node.get_child_count(); i_cf++)
    {
	XMLNode *t_s = node.get_child(i_cf);
	if( t_s->get_name() == "configs" )
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
		if( t_c->get_name() == "fld" )
		{
		    cntr.ctr_din_get(t_c);
		    page = page+
			"<tr><td>"+t_c->get_attr("dscr")+":</td>"+
			"<td><b>" +t_c->get_text()+"</b></td></tr>\n";
		}
		else if( t_c->get_name() == "list" )
		{
		    cntr.ctr_din_get(t_c);
		    page = page+"<tr><td>"+t_c->get_attr("dscr")+":</td><td><ul>";		    
		    for( unsigned i_lel = 0; i_lel < t_c->get_child_count(); i_lel++)		    
			if( t_c->get_child(i_lel)->get_name() == "el" && t_c->get_child(i_lel)->get_attr("hide") != "1" )
			    page = page+"<il>"+t_c->get_child(i_lel)->get_text()+"</il>";
		    page = page+"</ul></td></tr>\n";
		}
	    }
	    page = page+"</tbody></table><br>\n";
	    
    	    get_cfg(*t_s, cntr, page);
	}
    }
    if(c_cfg > 0) page = page+"</ul>\n";
}

void TWEB::get_branch( XMLNode &node, TContr &cntr, string &page, string &path )
{
    int s_cfg = 0;      //section counter

    try
    {	
    	while( true )
	{	    
    	    XMLNode *t_s = node.get_child("branchs",s_cfg++);
	    cntr.ctr_din_get(t_s);
	    if(s_cfg==1) page = page+"<ul>";
    	    try
    	    {
		page =  page+
			"<li><font size=\"+1\"><b><i>"+
			t_s->get_attr("dscr")+
			"</b></i></font></li>\n";			
		page = page+"<ul>\n";
		
		int f_cfg = 0;
		while(true)
    		{
		    XMLNode *t_c = t_s->get_child("br",f_cfg++);
		    page = page+ "<li> <a href=\""+path+"/"+t_c->get_attr("id")+"\">"+t_c->get_attr("id")+":"+t_c->get_attr("dscr")+"</a> </li>";
		    //page = page+ "<li>"+t_c->get_attr("id")+":"+t_c->get_attr("dscr")+"</li>";
		}
	    }catch(...){ page = page+"</ul>\n"; }	    
	    page = page+"<h4 align=\"left\"><font color=moccasin>Branch metods</font></h4>\n";
	    get_cmd( *t_s, page);
	}
    }
    catch(...){ if(s_cfg > 1) page = page+"</ul>\n"; }
}

void TWEB::get_cmd( XMLNode &node, string &page )
{
    int s_cfg = 0;      //section counter

    try
    {	
    	while( true )
	{	    
    	    XMLNode *t_s = node.get_child("comm",s_cfg++);
	    if(s_cfg==1) page = page+"<ul>";
	    page = page+"<li><font size=\"+1\"><b><i>"+t_s->get_attr("dscr")+"</b></i></font>";
	    page = page+" : "+t_s->get_attr("id");
    	    try
    	    {
		page = page+"(";
		int f_cfg = 0;
		while(true)
    		{
		    XMLNode *t_c = t_s->get_child("fld",f_cfg++);
		    if(f_cfg > 1) page = page+",";
		    page = page+" "+t_c->get_attr("id")+" ";
		}			
	    }catch(...){ page = page+")\n"; }
	    page = page+"</li>\n";
	}
    }
    catch(...){ if(s_cfg > 1) page = page+"</ul>\n"; }
}

void TWEB::HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein )
{

}


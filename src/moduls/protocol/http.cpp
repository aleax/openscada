
#include <getopt.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tmodule.h"
#include "../../tuis.h"
#include "http.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "http"
#define NAME_TYPE   "Protocol"
#define VER_TYPE    VER_PROT
#define VERSION     "0.6.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Http OpenScada input protocol for web configurator."
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
	pr_http::TProt *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
    	    self_addr = new pr_http::TProt( source );

	return ( self_addr );
    }
}

using namespace pr_http;

//================================================================
//=========== TProt ==============================================
//================================================================
TProt::TProt( string name )
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TProt::~TProt()
{

}

string TProt::opt_descr( )
{
    string rez;

    rez=rez+
	"=================== "+NAME_MODUL+" module options =======================\n"+
    	"---------------------- Module parameters of config file ------------------\n";

    return(rez);
}

void TProt::mod_CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL, 0 }
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

void TProt::mod_UpdateOpt(  )
{

}

TProtocolIn *TProt::in_open( string name )
{
    return( new TProtIn(name,this) );
}

//================================================================
//=========== TProtIn ============================================
//================================================================
TProtIn::TProtIn( string name, TProtocol *owner) : TProtocolIn(name,owner), m_nofull(false)
{

}

TProtIn::~TProtIn()
{

}

char *TProtIn::ok_response =
    "HTTP/1.0 200 OK\n"
    "Content-type: text/html\n"
    "\n";

char *TProtIn::bad_request_response =
    "HTTP/1.0 400 Bad Request\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Bad Request</h1>\n"
    "  <p>This server did not undersand your request.</p>\n"
    " </body>\n"
    "</html>\n";
    
char *TProtIn::not_found_response_template = 
    "HTTP/1.0 404 Not Found\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Not Found</h1>\n"    
    "  <p> %s </p>\n"
    " </body>\n"
    "</html>\n";

char *TProtIn::bad_method_response_template =
    "HTTP/1.0 501 Method Not Implemented\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Method Not Implemented</h1>\n"
    "  <p>The method %s is not implemented by this server.</p>\n"
    " </body>\n"
    "</html>\n";


bool TProtIn::mess(string &request, string &answer, string sender )
{
    char buf[1024];
    int hd = -1; 
    string req;
    vector<string> vars;    
    
    //Continue with full request
    if( m_nofull ) 
    {    
	request = m_buf+request;
	m_nofull = false;
    }
    m_buf=request;  //Save request to bufer    

    
    answer = "";
    if( request.size() > 0 )
    {
	int    pos = 0;
	request[request.size()] = '\0';
	//Mess->put("DEBUG",MESS_DEBUG,"Content: <%s>!",request.c_str());
	
	//Parse first record
	req     = request.substr(0,request.find("\n",0)-1);
	string method   = req.substr(pos,req.find(" ",pos)-pos); pos = req.find(" ",pos)+1;
	string url      = req.substr(pos,req.find(" ",pos)-pos); pos = req.find(" ",pos)+1;
	string protocol = req.substr(pos,req.find(" ",pos)-pos); pos = req.find(" ",pos)+1;	
	
	//Parse all next records to content
	request = request.substr(request.find("\n",0)+1);	
	req     = request.substr(0,request.find("\n",0)-1);
	do
	{
    	    pos = 0;
    	    string var = req.substr(pos,req.find(":",pos)-pos); pos = req.find(":",pos)+1;
    	    if( var == "Content-Type" )
	    {		
		//Check full post message
		pos = req.find("boundary=",pos)+strlen("boundary=");
		string bound = req.substr(pos,req.find("\n",pos)-pos);
		pos = request.find("Content-Length:",0);
		if( pos == string::npos ) { m_nofull = true; break; }       //Request no full
		pos += strlen("Content-Length:");
		int c_lng = atoi(request.substr(pos,request.find("\n",pos)-pos).c_str());
		pos = request.find(bound,pos);		
		if( pos == string::npos || c_lng > (request.size()-pos+2) )		
		{ 
		    m_nofull = true; 
		    break; 
		}       //Request no full		
	    }
	    vars.push_back( req );	        	    
	    request = request.substr(request.find("\n",0)+1);	
    	    req     = request.substr(0,request.find("\n",0)-1);
	}
	while( request.size() && req.size() );
	if( m_nofull ) return(m_nofull);
	
	//Check protocol version	
	if( protocol != "HTTP/1.0" && protocol != "HTTP/1.1" )
	{
	    answer = bad_request_response;
	    return(m_nofull);
	}
	TUIS &ui = Owner().Owner().Owner().UI();
	if( url[0] != '/' ) url[0] = '/';
	string name_mod = url.substr(1,url.find("/",1)-1);
	
	//Get UI modul
        try
	{ 
	    hd = ui.gmd_att(name_mod);
	    if( ui.gmd_at( hd ).mod_info("SubType") != "WWW" )
		throw TError("%s: find no WWW subtype module!",NAME_MODUL);
	}
	catch(TError err)
	{    	    
	    if( hd >= 0 ) ui.gmd_det(hd);
	    index(answer); 
	    return(m_nofull);
	}
	
	//Check metods
	int n_dir = url.find("/",1);
	if( n_dir == string::npos ) url = "/";
	else                        url = url.substr(n_dir,url.size()-n_dir);
	if( method == "GET" )
	{
	    void(TModule::*HttpGet)(string &url, string &page, string &sender, vector<string> &vars);
	    char *n_f = "HttpGet";

	    try
	    {
		ui.gmd_at( hd ).mod_GetFunc(n_f,(void (TModule::**)()) &HttpGet);
		answer = ok_response;
		((&ui.gmd_at( hd ))->*HttpGet)(url,answer,sender,vars);
		ui.gmd_at( hd ).mod_FreeFunc(n_f);
		//Mess->put("DEBUG",MESS_DEBUG,"Get Content: <%s>!",request.c_str());
	    }
	    catch(TError err)
	    {
       		ui.gmd_det(hd); 
		index(answer);  
    		return(m_nofull);
	    }
	}
	else if( method == "POST" ) 
	{
	    void(TModule::*HttpPost)(string &url, string &page, string &sender, vector<string> &vars, string &contein );
	    char *n_f = "HttpPost";

	    try
	    {
		ui.gmd_at( hd ).mod_GetFunc(n_f,(void (TModule::**)()) &HttpPost);
		answer = ok_response;
		((&ui.gmd_at( hd ))->*HttpPost)(url,answer,sender,vars,request);
		ui.gmd_at( hd ).mod_FreeFunc(n_f);
		//Owner().m_put("DEBUG",MESS_DEBUG,"Post Content: <%s>!",request.c_str());
	    }
	    catch(TError err)
	    {
       		ui.gmd_det(hd); 
		index(answer);  
    		return(m_nofull);
	    }
	}
	else
	{
	    snprintf(buf,sizeof(buf),bad_method_response_template,method.c_str());
	    answer = buf;
	    m_wait = false;
	}
	ui.gmd_det(hd);
    }

    return(m_nofull);
}

char *TProtIn::w_head =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html> <head>\n"
    "  <title>OpenSCADA!</title>\n"
    " </head>\n";

char *TProtIn::w_head_ =
    "</html>\n";

char *TProtIn::w_body =
    "<body bgcolor=\"#2A4547\" text=\"#ffffff\" link=\"#3366ff\" vlink=\"#339999\" alink=\"#33ccff\">\n"
    "<h1 align=\"center\"><font color=\"#ffff00\">OpenSCADA!</font></h1>\n"
    "<hr width=\"100%\" size=\"2\">\n"
    "<br><br>\n";

char *TProtIn::w_body_ =
    "<hr width=\"100%\" size=\"2\">\n"
    "</body>\n";         

void TProtIn::index( string &answer )
{ 
    answer = answer+w_head+w_body;
    answer = answer+"<table border=2 align='center' width=40% bgcolor='#A9A9A9'>\n"+
	    "<tr bgcolor=#9999ff><td><b>Avoid web modules</b></td></tr>\n"+
	    "<tr bgcolor=#cccccc><td><ul>";
    vector<string> list;
    TUIS &ui = Owner().Owner().Owner().UI();
    ui.gmd_list(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = ui.gmd_att(list[i_l]);
	if( ui.gmd_at( hd ).mod_info("SubType") == "WWW" )
    	    answer = answer+"<li><a href='"+list[i_l]+"'>"+ui.gmd_at( hd ).mod_info("Descript")+"</a></li>\n"; 
	ui.gmd_det(hd);
    }     
    answer = answer+"</ul></td></tr></table>"+w_body_+w_head_;
}


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
#define VERSION     "0.1.0"
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

void TProt::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
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
	    case 'h': pr_opt_descr(stdout); break;
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
TProtIn::TProtIn( string name, TProtocol *owner) : TProtocolIn(name,owner)
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

/* 
 * In structure:
 *  GET /index.html HTTP/1.0
 *  Host: 209.123.34.12
 *  User-Agent: Mozilla/5.0
 *  ........
 * Out structure
 *  HTTP/1.0 200 OK
 *  Date: Sat, 12 Jul 2003 13:20:20 GMT
 *  Server: Apache 1.3.5
 *  Content-Length: 132
 *  Content-type: text/html
 *
 *  ........
 *
 * Want add metods: POST, PUT, LINK, TRACE ...
 */    
void TProtIn::mess(string &request, string &answer )
{
    char buf[1024];
    int hd = -1; 
    
    answer = "";
    if( request.size() > 0 )
    {
	int    pos = 0;

	request[request.size()] = '\0';
	request = request.substr(0,request.find("\n",0)-1);
	string method   = request.substr(pos,request.find(" ",pos)-pos); pos = request.find(" ",pos)+1;
	string url      = request.substr(pos,request.find(" ",pos)-pos); pos = request.find(" ",pos)+1;
	string protocol = request.substr(pos,request.find(" ",pos)-pos); pos = request.find(" ",pos)+1;	
	//May by want read a info of header	
	if( protocol != "HTTP/1.0" && protocol != "HTTP/1.1" )
	{
	    answer = bad_request_response;
	    return;
	}
	TUIS &ui = Owner().Owner().Owner().UI();
	if( url[0] != '/' ) url[0] = '/';
	string name_mod = url.substr(1,url.find("/",1)-1);
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
	    return;
	}	  
	if( method == "GET" ) 
	{
	    void(TModule::*HttpGet)(string &url, string &page);
	    char *n_f = "HttpGet";

	    try
	    {
		ui.gmd_at( hd ).mod_GetFunc(n_f,(void (TModule::**)()) &HttpGet);
		int n_dir = url.find("/",1);
		if( n_dir == string::npos ) url = "/";
		else                        url = url.substr(n_dir,url.size()-n_dir);
		answer = ok_response;
		((&ui.gmd_at( hd ))->*HttpGet)(url,answer);
		ui.gmd_at( hd ).mod_FreeFunc(n_f);
	    }
	    catch(TError err)
	    {
       		ui.gmd_det(hd); 
		index(answer);  
		return;
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
}

char *TProtIn::w_head =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html> <head>\n"
    "  <title>OpenSCADA!</title>\n"
    " </head>\n";

char *TProtIn::w_head_ =
    "</html>\n";

char *TProtIn::w_body =
    " <body bgcolor=\"#2A4547\" text=\"#ffffff\" link=\"#3366ff\" vlink=\"#339999\" alink=\"#33ccff\">\n"
    "  <h1 align=\"center\"><font color=\"#ffff00\">OpenSCADA!</font></h1>\n"
    "  <hr width=\"100%\" size=\"1\">\n"
    "  <br><br>\n";

char *TProtIn::w_body_ =
    " </body>\n";         

void TProtIn::index( string &answer )
{ 
    answer = answer+w_head+w_body;
    answer = answer+"<h2 align=\"center\"><font color=aqua>Avoid web modules</font></h2>";
    
    vector<string> list;
    TUIS &ui = Owner().Owner().Owner().UI();
    ui.gmd_list(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = ui.gmd_att(list[i_l]);
	if( ui.gmd_at( hd ).mod_info("SubType") == "WWW" )
    	    answer = answer+"<h3 align=\"center\"><a href=\""+list[i_l]+"\">"+ui.gmd_at( hd ).mod_info("Descript")+"</a> </h3>"; 
	ui.gmd_det(hd);
    }     
    answer = answer+w_body_+w_head_;
}

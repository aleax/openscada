
#include <getopt.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tmodule.h"
#include "http.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "http"
#define NAME_TYPE   "Protocol"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Http OpenScada input protocol for web configurator."
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

TProtHttp::TProtHttp(char *name) 
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

    ExpFunc   = NULL; // (SExpFunc *)ExpFuncLc;
    NExpFunc  = 0; // sizeof(ExpFuncLc)/sizeof(SExpFunc);
}

TProtHttp::~TProtHttp()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TProtHttp *self_addr;
    if(n_mod==0) self_addr = new TProtHttp( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

void TProtHttp::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TProtHttp::mod_CheckCommandLine( )
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


char *TProtHttp::ok_response =
    "HTTP/1.0 200 OK\n"
    "Content-type: text/html\n"
    "\n";

char *TProtHttp::bad_request_response =
    "HTTP/1.0 400 Bad Request\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Bad Request</h1>\n"
    "  <p>This server did not undersand your request.</p>\n"
    " </body>\n"
    "</html>\n";
    
char *TProtHttp::not_found_response_template = 
    "HTTP/1.0 404 Not Found\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Not Found</h1>\n"
    "  <p>The request URL %s was not found on this server.</p>\n"
    " </body>\n"
    "</html>\n";

char *TProtHttp::bad_method_response_template =
    "HTTP/1.0 501 Method Not Implemented\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Method Not Implemented</h1>\n"
    "  <p>The method %s is not implemented by this server.</p>\n"
    " </body>\n"
    "</html>\n";
    
void TProtHttp::in_mess(string &request, string &answer )
{
    char buf[1024];
    
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
	    answer = bad_request_response;
	else if(method != "GET" )
	{
	    snprintf(buf,sizeof(buf),bad_method_response_template,method.c_str());
	    answer = buf;
	}
	else 
	{
	    if( url[0] == '/' && url.find("/",1) == string::npos )
	    {
    		snprintf(buf,sizeof(buf),not_found_response_template,url.c_str());
    		answer = buf;
	    }
	}
    }
}



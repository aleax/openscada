#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "web_dbg.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_dbg"
#define NAME_TYPE   "Special"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web debug interface for http protocol"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TSP_WEB_DBG::ExpFuncLc[] =
{
    {"HttpGet",(void(TModule::*)( )) &TSP_WEB_DBG::HttpGet,"void HttpGet(string &url, string &page);",
     "Process Get comand from http protocol's!",10,0}
};

TSP_WEB_DBG::TSP_WEB_DBG(char *name)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

    ExpFunc   = (SExpFunc *)ExpFuncLc;
    NExpFunc  = sizeof(ExpFuncLc)/sizeof(SExpFunc);
}

TSP_WEB_DBG::~TSP_WEB_DBG()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TSP_WEB_DBG *self_addr;
    if(n_mod==0) self_addr = new TSP_WEB_DBG( FName );
    else         self_addr = NULL;
    return ( self_addr );
}


void TSP_WEB_DBG::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TSP_WEB_DBG::mod_CheckCommandLine(  )
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

char *TSP_WEB_DBG::w_head =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html> <head>\n"
    "  <title>OpenSCADA debug web modul!</title>\n"
    " </head>\n";

char *TSP_WEB_DBG::w_head_ =
    "</html>\n";

char *TSP_WEB_DBG::w_body =
    " <body bgcolor=\"#330033\" text=\"#ffffff\" link=\"#3366ff\" vlink=\"#339999\" alink=\"#33ccff\">\n"
    "  <h1 align=\"center\"><font color=\"#ffff00\"> Welcome to OpenSCADA debug web modul!</font></h1>\n"
    "  <hr width=\"100%\" size=\"2\">\n"
    "  <br><br>\n";

char *TSP_WEB_DBG::w_body_ =
    " </body>\n";    

void TSP_WEB_DBG::HttpGet(string &url, string &page)
{
    //char buf[1024];

    //snprintf(buf,sizeof(buf),ok_response,url.c_str());
    page = page+w_head+w_body+w_body_+w_head_;    
}


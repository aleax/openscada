#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "web_info.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_info"
#define NAME_TYPE   "Special"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web info interface for http protocol"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TSP_WEB_INFO::ExpFuncLc[] =
{
    {"HttpGet",(void(TModule::*)( )) &TSP_WEB_INFO::HttpGet,"void HttpGet(string &url, string &page);",
     "Process Get comand from http protocol's!",10,0}
};

TSP_WEB_INFO::TSP_WEB_INFO(char *name)
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

TSP_WEB_INFO::~TSP_WEB_INFO()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TSP_WEB_INFO *self_addr;
    if(n_mod==0) self_addr = new TSP_WEB_INFO( FName );
    else         self_addr = NULL;
    return ( self_addr );
}


void TSP_WEB_INFO::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TSP_WEB_INFO::mod_CheckCommandLine(  )
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

char *TSP_WEB_INFO::mess =
    "<html>\n"
    " <body>\n"
    "  <h1> Welcome to OpenSCADA web info modul! </h1>\n"
    "  <p> Request \"%s\" !!! </p>\n"
    " </body>\n"
    "</html>\n";

void TSP_WEB_INFO::HttpGet(string &url, string &page)
{
    char buf[1024];

    snprintf(buf,sizeof(buf),mess,url.c_str());
    page = page + buf;    
}


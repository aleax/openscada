#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "web_info.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_info"
#define NAME_TYPE   "UI"
#define SUB_TYPE    "WWW"
#define VERSION     "0.0.2"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web info interface for http protocol"
#define LICENSE     "GPL"
//==============================================================================
extern "C"
{
    TModule *attach( char *FName, int n_mod )
    {
	WebInfo::TWEB *self_addr;
	if(n_mod==0) self_addr = new WebInfo::TWEB( FName );
	else         self_addr = NULL;
	return ( self_addr );
    }
}

using namespace WebInfo;

//==============================================================================
//================ WebInfo::TWEB ===============================================
//==============================================================================
SExpFunc TWEB::ExpFuncLc[] =
{
    {"HttpGet",(void(TModule::*)( )) &TWEB::HttpGet,"void HttpGet(string &url, string &page);",
     "Process Get comand from http protocol's!",10,0}
};

TWEB::TWEB(char *name)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = name;

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

char *TWEB::mess =
    "<html>\n"
    " <body>\n"
    "  <h1> Welcome to OpenSCADA web info modul! </h1>\n"
    "  <p> Request \"%s\" !!! </p>\n"
    " </body>\n"
    "</html>\n";

void TWEB::HttpGet(string &url, string &page)
{
    char buf[1024];

    snprintf(buf,sizeof(buf),mess,url.c_str());
    page = page + buf;    
}



#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tmodule.h"
#include "self.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "self"
#define NAME_TYPE   "Protocol"
#define VERSION     "0.0.2"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Self OpenScada protocol, support generic functions."
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

TProtSelf::TProtSelf(char *name) 
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);
}

TProtSelf::~TProtSelf()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TProtSelf *self_addr;
    if(n_mod==0) self_addr = new TProtSelf( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

void TProtSelf::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TProtSelf::mod_CheckCommandLine( )
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

void TProtSelf::in_mess(string &request, string &answer )
{
    if( request == "time" )
    {
	time_t tm = time(NULL);
	answer = asctime(localtime(&tm));
    }
    else answer = "ERROR: request no support!\n";
}


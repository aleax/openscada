/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tkernel.h"
#include "../../tmessage.h"
#include "socket.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "socket"
#define NAME_TYPE   "Transport"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Transport based for inet, unix sockets. inet socket support TCP and UDP"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

TTransSock::TTransSock(char *name)
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

TTransSock::~TTransSock()
{
    free(FileName);
}

TModule *attach( char *FName, int n_mod )
{
    TTransSock *self_addr;
    if(n_mod==0) self_addr = new TTransSock( FName );
    else         self_addr = NULL;
    return static_cast< TModule *>( self_addr );
}

void TTransSock::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== Module %s options =============================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TTransSock::CheckCommandLine( char **argv, int argc )
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
	next_opt=getopt_long(argc,(char * const *)argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTransSock::UpdateOpt()
{

}

TTransportIn *TTransSock::In(string name, string address )
{
    return(new TSocketIn(name,address));
}

TTransportOut *TTransSock::Out(string name, string address )
{
    return(new TSocketOut(name,address));
}


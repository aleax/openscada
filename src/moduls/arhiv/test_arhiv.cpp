/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "test_arhiv.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_arhiv"
#define NAME_TYPE   "Arhiv"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "test"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

TArhivTest::TArhivTest(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

//    ExpFunc   = NULL; // (SExpFunc *)ExpFuncLc;
//    NExpFunc  = 0; // sizeof(ExpFuncLc)/sizeof(SExpFunc);
#if OSC_DEBUG
    Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
}

TArhivTest::~TArhivTest()
{
#if OSC_DEBUG
    Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TArhivTest *self_addr;
    if(n_mod==0) self_addr = new TArhivTest( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

void TArhivTest::mod_info( const string & name, string & info )
{
    info.erase();
    TModule::mod_info(name,info);
}


void TArhivTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}



void TArhivTest::mod_CheckCommandLine( ) 
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

void TArhivTest::mod_init( void *param )
{
    TModule::mod_init( param );
}


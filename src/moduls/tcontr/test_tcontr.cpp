/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "test_tcontr.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_tcontr"
#define NAME_TYPE   "Controller"
#define VERSION     "0.1"
#define AUTORS      "Roman_Savochenko"
#define DESCRIPTION "test"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );


TTContrTest::TTContrTest(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

    ExpFunc   = NULL; // (SExpFunc *)ExpFuncLc;
    NExpFunc  = 0;    // sizeof(ExpFuncLc)/sizeof(SExpFunc);
#if OSC_DEBUG
    Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
}

TTContrTest::~TTContrTest()
{
#if OSC_DEBUG
    Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TTContrTest *self_addr;
    if(n_mod==0) self_addr = new TTContrTest( FName );
    else         self_addr = NULL;
    return ( self_addr );
}


void TTContrTest::mod_info( const string & name, string & info )
{
    info.erase();
    TModule::mod_info(name,info);
}




void TTContrTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}



void TTContrTest::mod_CheckCommandLine(  )
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

void TTContrTest::mod_init( void *param )
{
    TModule::mod_init( param );
}


/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "test_special.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_special"
#define NAME_TYPE   "Special"
#define VERSION     "0.1"
#define AUTORS      "Roman_Savochenko"
#define DESCRIPTION "test"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

TSpecialTest::TSpecialTest(char *name) : TModule()
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
#if debug
    App->Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
}

TSpecialTest::~TSpecialTest()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TSpecialTest *self_addr;
    if(n_mod==0) self_addr = new TSpecialTest( FName );
    else         self_addr = NULL;
    return ( self_addr );
}


void TSpecialTest::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
}




void TSpecialTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}



void TSpecialTest::CheckCommandLine(  )
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
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TSpecialTest::init( void *param )
{
    TModule::init( param );
}


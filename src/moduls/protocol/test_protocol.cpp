/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../gener/tmodule.h"
#include "test_protocol.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_protocol"
#define NAME_TYPE   "Protocol"
#define VERSION     "0.1"
#define AUTORS      "Roman_Savochenko"
#define DESCRIPTION "test"
#define LICENSE     "LGPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

TProtocolTest::TProtocolTest(char *name) : TModule()
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

TProtocolTest::~TProtocolTest()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TProtocolTest *self_addr;
    if(n_mod==0) self_addr = new TProtocolTest( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

int TProtocolTest::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
    
    return(0);
}




void TProtocolTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}



void TProtocolTest::CheckCommandLine(  )
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

int TProtocolTest::init( void *param )
{
    CheckCommandLine();
    TModule::init( param );
}


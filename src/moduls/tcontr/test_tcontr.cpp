/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../gener/tmodule.h"

#define NAME_MODUL  "test_tcontr"
#define NAME_TYPE   "Controller"
#define VERSION     "0.1"
#define AUTORS      "Roman_Savochenko"
#define DESCRIPTION "test"

extern "C" TModule *attach(char *FName);

class TTContrTest: public TModule
{
    public:
	TTContrTest(char *name);
	virtual ~TTContrTest();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
};


TTContrTest::TTContrTest(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    FileName  = strdup(name);
}

TTContrTest::~TTContrTest()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach(char *FName)
{
    
    TTContrTest *self_addr = new TTContrTest(FName);
    return(self_addr);
}

int TTContrTest::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
    
    return(0);
}




void TTContrTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}



void TTContrTest::CheckCommandLine(  )
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

int TTContrTest::init( )
{
    CheckCommandLine();
    TModule::init();
}


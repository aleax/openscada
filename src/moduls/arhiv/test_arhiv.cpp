/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../gener/tmodule.h"

#define NAME_MODUL  "test_arhiv"
#define NAME_TYPE   "Arhiv"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "test"

extern "C" TModule *attach(char *FName);

class TArhivTest: public TModule
{
    public:
	TArhivTest(char *name);
	virtual ~TArhivTest();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
//	char *FileName;
};


TArhivTest::TArhivTest(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    FileName  = strdup(name);
}

TArhivTest::~TArhivTest()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach(char *FName)
{
    
    TArhivTest *self_addr = new TArhivTest(FName);
    return(self_addr);
}

int TArhivTest::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
    
    return(0);
}




void TArhivTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}



void TArhivTest::CheckCommandLine(  )
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

int TArhivTest::init( )
{
    CheckCommandLine();
    TModule::init();
}


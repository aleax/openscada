/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../gener/tmodule.h"

#define NAME_MODUL  "test_bd"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.2"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Test"

extern "C" TModule *attach(char *FName);


class TBDtest1: public TModule
{
    public:
	TBDtest1(char *name);
	virtual ~TBDtest1();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	char *FileName;
};


TBDtest1::TBDtest1(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    FileName  = strdup(name);
}

TBDtest1::~TBDtest1()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach(char *FName)
{
    
    TBDtest1 *self_addr = new TBDtest1(FName);
    return(self_addr);
}


int TBDtest1::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);

    return(MOD_NO_ERR);
}




void TBDtest1::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
#if debug
    "    --ModPrm1        Test modul param;\n"
#endif    
    "\n",NAME_MODUL);
}



void TBDtest1::CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
#if debug
	{"ModPrm1"    ,0,NULL,'m'},
#endif    
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
#if debug
	    case 'm': App->Mess->put(1,"Test Mod1 Prm OK!"); break;
#endif    
	    case -1 : break;
	}
    } while(next_opt != -1);
}

int TBDtest1::init( )
{
    CheckCommandLine();
    TModule::init();
    return(MOD_NO_ERR);
}


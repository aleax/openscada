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
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "test"

extern "C" TModule *attach(char *FName);


class TBDtest2: public TModule
{
    public:
	TBDtest2(char *name);
	virtual ~TBDtest2();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
//	char *FileName;
};


TBDtest2::TBDtest2(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    FileName  = strdup(name);
}

TBDtest2::~TBDtest2()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach(char *FName)
{
    
    TBDtest2 *self_addr = new TBDtest2(FName);
    return(self_addr);
}

int TBDtest2::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);

    return(MOD_NO_ERR);
}




void TBDtest2::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
#if debug
    "    --ModPrm2        Test modul param;\n"
#endif    
    "\n",NAME_MODUL);
}



void TBDtest2::CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
#if debug
	{"ModPrm2"    ,0,NULL,'m'},
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
	    case 'm': App->Mess->put(1,"Test Mod2 Prm OK!"); break;
#endif    
	    case -1 : break;
	}
    } while(next_opt != -1);
}

int TBDtest2::init( )
{
    CheckCommandLine();
    TModule::init();
}


/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "test_bd2.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_bd2"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "First a test modul for debug to OpenScada progect!"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TBDtest2::ExpFuncLc[]=
{ 
    {"OpenBD" ,(void (TModule::*)()) &TBDtest2::OpenBD  ,"int OpenBD( string name );"  ,"Open BD <name>"},
    {"CloseBD",(void (TModule::*)()) &TBDtest2::CloseBD ,"int CloseBD( int hd );"      ,"Close BD <hd>" }
};

TBDtest2::TBDtest2(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    FileName  = strdup(name);

    ExpFunc  = (SExpFunc *)ExpFuncLc;
    NExpFunc = sizeof(ExpFuncLc)/sizeof(SExpFunc);

#if debug
    App->Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
    
}

TBDtest2::~TBDtest2()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TBDtest2 *self_addr;
    if(n_mod==0) self_addr = new TBDtest2( FName );
    else         self_addr = NULL;
    return ( self_addr );
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
    "------------------ %s options ---------------------------------------\n"
#if debug
    "    --ModPrm        Test modul param;\n"
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
	{"ModPrm"    ,0,NULL,'m'},
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
	    case 'm': App->Mess->put(1,"Test Mod Prm OK!"); break;
#endif    
	    case -1 : break;
	}
    } while(next_opt != -1);
}

int TBDtest2::init( )
{
    CheckCommandLine();
    TModule::init();
    return(MOD_NO_ERR);
}


int TBDtest2::OpenBD( string name )
{
    App->Mess->put(1,"Test call Open BD: %s !",name.c_str());
    return(-1);
}

int TBDtest2::CloseBD( int hd )
{
    App->Mess->put(1,"Test call Close BD: %d !",hd);
    return(-1);
}


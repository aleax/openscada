/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "test_bd1.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_bd"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.2"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "First a test modul for debug to OpenScada progect!"
#define LICENSE     "LGPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TBDtest1::ExpFuncLc[]=
{ 
    {"OpenBD" ,(void (TModule::*)()) &TBDtest1::OpenBD  ,"int OpenBD( string name );"  ,"Open BD <name>"},
    {"CloseBD",(void (TModule::*)()) &TBDtest1::CloseBD ,"int CloseBD( int hd );"      ,"Close BD <hd>" }
};

TBDtest1::TBDtest1(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;    
    FileName  = strdup(name);

    ExpFunc  = (SExpFunc *)ExpFuncLc;
    NExpFunc = sizeof(ExpFuncLc)/sizeof(SExpFunc);

#if debug
    App->Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
    
}

TBDtest1::~TBDtest1()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TBDtest1 *self_addr;
    if(n_mod==0) self_addr = new TBDtest1( FName );
    else         self_addr = NULL;
    return ( self_addr );
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
    "------------------ %s options ---------------------------------------\n"
#if debug
    "    --ModPrm        Test modul param;\n"
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

int TBDtest1::init( void *param )
{
    CheckCommandLine();
    TModule::init( param );
    return(MOD_NO_ERR);
}


int TBDtest1::OpenBD( string name )
{
    App->Mess->put(1,"Test call Open BD: %s !",name.c_str());
    return(-1);
}

int TBDtest1::CloseBD( int hd )
{
    App->Mess->put(1,"Test call Close BD: %d !",hd);
    return(-1);
}


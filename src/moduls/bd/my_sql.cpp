/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "my_sql.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "my_sql"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Support MY_SQL BD!"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TMY_SQL::ExpFuncLc[]=
{ 
    {"OpenBD" ,(void (TModule::*)()) &TMY_SQL::OpenBD  ,"int OpenBD( string name );"  ,"Open BD <name>"},
    {"CloseBD",(void (TModule::*)()) &TMY_SQL::CloseBD ,"int CloseBD( int hd );"      ,"Close BD <hd>" }
};

TMY_SQL::TMY_SQL(char *name) : TModule()
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
}

TMY_SQL::~TMY_SQL()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TMY_SQL *self_addr;
    if(n_mod==0) self_addr = new TMY_SQL( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

void TMY_SQL::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
}


void TMY_SQL::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "------------------ %s options ---------------------------------------\n"
    "\n",NAME_MODUL);
}

void TMY_SQL::CheckCommandLine(  )
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

void TMY_SQL::init( void *param )
{
    TModule::init( param );
}


int TMY_SQL::OpenBD( string name )
{
    App->Mess->put(1,"Test call Open BD: %s !",name.c_str());
    return(-1);
}

int TMY_SQL::CloseBD( int hd )
{
    App->Mess->put(1,"Test call Close BD: %d !",hd);
    return(-1);
}


/* Test Modul
** ==============================================================
*/

#include <getopt.h>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../../tconfig.h"
#include "../../tcontroller.h"
#include "../gener/tmodule.h"
#include "virtual.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "virtual"
#define NAME_TYPE   "Controller"
#define VERSION     "0.1"
#define AUTORS      "Roman_Savochenko"
#define DESCRIPTION "Virtual controller my be used how internal controller or instrument for GUI"
#define LICENSE     "LGPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

//==== Desribe bd fields ====

SRecStr TVirtual::RStr[] =
{
    {""},
    {"virtual controller"},
    {"VRT_AN"},
    {"VRT_DG"}
};

SRecNumb TVirtual::RNumb[] =
{
    {0., 10000., 1000., 0, 0},
    {0.,    99.,    1., 0, 0}
};

SElem TVirtual::elem[] =
{
    {"NAME"   ,"Short name of controller."        ,CFGTP_STRING,20,-1,"",&RStr[0],NULL     ,NULL},
    {"LNAME"  ,"Description of controller."       ,CFGTP_STRING,50,-1,"",&RStr[1],NULL     ,NULL},
    {"PRM_BD1","Name BD for ANALOG parameteres."  ,CFGTP_STRING,20,-1,"",&RStr[2],NULL     ,NULL},
    {"PRM_BD2","Name BD for DIGIT parameteres."   ,CFGTP_STRING,20,-1,"",&RStr[3],NULL     ,NULL},
    {"PERIOD" ,"Pooled period (ms)."              ,CFGTP_NUMBER,5 ,-1,"",NULL    ,&RNumb[0],NULL},
    {"ITER"   ,"Number of a iterations at period.",CFGTP_NUMBER,2 ,-1,"",NULL    ,&RNumb[1],NULL}
};

//===========================

TVirtual::TVirtual(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

//    ExpFunc   = NULL; // (SExpFunc *)ExpFuncLc;
//    NExpFunc  = 0; // sizeof(ExpFuncLc)/sizeof(SExpFunc);
#if debug
    App->Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
}

TVirtual::~TVirtual()
{
#if debug
    App->Mess->put(1,"Run destructor moduls %s file %s is OK!",NAME_MODUL,FileName);
#endif
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TVirtual *self_addr;
    if(n_mod==0) self_addr = new TVirtual( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

int TVirtual::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
    
    return(0);
}

int TVirtual::PutCommand( string command, int id_cntr )
{
#if debug
    App->Mess->put(1, "Command: <%s> to controller <%d>!",command.c_str(),id_cntr);
#endif
    if(command == "INIT")        return(InitContr(id_cntr));
    else if(command == "DEINIT")
    {
//	    SaveBDContr( atoi( param.c_str() ) );
//	    SaveBDParams( atoi( param.c_str() ) );
    }
    else return(-1);

    return(0);   
}

	
void TVirtual::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}

void TVirtual::CheckCommandLine(  )
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

int TVirtual::init( void *param )
{
    data = (STContr *)param;
//    LoadCntrCfg();
    CheckCommandLine();
    TModule::init( param );
}

int TVirtual::InitContr(int id)
{
    int i;

//    LoadBDContr( atoi( param.c_str() ) );
//    LoadBDParams( atoi( param.c_str() ) );
//    LoadBD

#if debug
    App->Mess->put(1, "Init controller: <%d>, bd <%s>!",id,data->contr[id]->bd.c_str());
#endif
    return(i);    
}


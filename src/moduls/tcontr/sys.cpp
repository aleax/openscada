/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <unistd.h>

#include "../../terror.h"
#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tcontrollers.h"
#include "sys.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "SysContr"
#define NAME_TYPE   "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "System controller used for monitoring and control OS"
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    SAtMod module( int n_mod )
    {
	SAtMod AtMod;

	if(n_mod==0)
	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
    	    AtMod.name  = "";

	return( AtMod );
    }

    TModule *attach( SAtMod &AtMod, string source )
    {
	SystemCntr::TTpContr *self_addr = NULL;

    	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new SystemCntr::TTpContr( source );

	return ( self_addr );
    }
    /*
    TModule *attach( char *FName, int n_mod )
    {
	SystemCntr::TTpContr *self_addr;
	if(n_mod==0) self_addr = new SystemCntr::TTpContr( FName );
	else         self_addr = NULL;
	return static_cast< TModule *>( self_addr );
    }
    */
}

using namespace SystemCntr;

//======================================================================
//==== TTpContr ======================================================== 
//======================================================================

TTpContr::TTpContr( string name )  
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;    
}

TTpContr::~TTpContr()
{    

}

void TTpContr::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== Module %s options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}


void TTpContr::mod_CheckCommandLine( )
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
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTpContr::mod_UpdateOpt( )
{

}

void TTpContr::mod_connect( )
{    
    TModule::mod_connect( );
}

TController *TTpContr::ContrAttach(string name, SBDS bd)
{
    return( new TMdContr(name,bd,this,this));    
}

//======================================================================
//==== TMdContr 
//======================================================================

TMdContr::TMdContr( string name_c, SBDS bd, ::TTipController *tcntr, ::TConfigElem *cfgelem) :
	::TController(name_c,bd,tcntr,cfgelem)
{

}

TMdContr::~TMdContr()
{

}

void TMdContr::Load( )
{
    TController::Load( );
}

void TMdContr::Save( )
{
    TController::Save( );
}

void TMdContr::Free( )
{
    TController::Free();
}

void TMdContr::Start( )
{      
    TController::Start();
}

void TMdContr::Stop( )
{  
    TController::Stop();    
} 

/*
void TMdContr::Enable( )
{
    TController::Enable( );
}

void TMdContr::Disable( )
{
    TController::Disable( );
}
*/



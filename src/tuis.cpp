#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tuis.h"

//================================================================
//================== TUIS ========================================
//================================================================
const char *TUIS::o_name = "TUIS";
	
TUIS::TUIS( TKernel *app ) : TGRPModule(app,"UI")
{

}

void TUIS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Protocol options ================================\n"
    "    --GUIModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TUIS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );

    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"GUIModPath" ,1,NULL,'m'},
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TUIS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();

}

void TUIS::gmd_Start( )
{
    for(unsigned i_sp = 0; i_sp < Moduls.size(); i_sp++)
	if(Moduls[i_sp] != TO_FREE) ((TUI *)Moduls[i_sp])->Start( );
}

void TUIS::gmd_Stop( )
{
    for(unsigned i_sp = 0; i_sp < Moduls.size(); i_sp++)
    	if(Moduls[i_sp] != TO_FREE) ((TUI *)Moduls[i_sp])->Start( );
}

//================================================================
//================== TUI =========================================
//================================================================
const char *TUI::o_name = "TUI";



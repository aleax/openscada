#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tguis.h"

//================================================================
//================== TGUIS =======================================
//================================================================
const char *TGUIS::o_name = "TGUIS";
	
TGUIS::TGUIS( TKernel *app ) : TGRPModule(app,"GUI")
{

}

void TGUIS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Protocol options ================================\n"
    "    --GUIModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TGUIS::gmd_CheckCommandLine( )
{
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

void TGUIS::gmd_UpdateOpt()
{

}

void TGUIS::gmd_Start( )
{
    for(unsigned i_sp = 0; i_sp < Moduls.size(); i_sp++)
	if(Moduls[i_sp] != TO_FREE) ((TGUI *)Moduls[i_sp])->Start( );
}

void TGUIS::gmd_Stop( )
{
    for(unsigned i_sp = 0; i_sp < Moduls.size(); i_sp++)
    	if(Moduls[i_sp] != TO_FREE) ((TGUI *)Moduls[i_sp])->Start( );
}

//================================================================
//================== TGUI ========================================
//================================================================
const char *TGUI::o_name = "TGUI";



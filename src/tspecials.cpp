
#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tspecials.h"

//================================================================
//=========== TSpecialS ==========================================
//================================================================
const char *TSpecialS::o_name = "TSpecialS";

TSpecialS::TSpecialS( TKernel *app ) : TGRPModule(app,"Special") 
{

}

void TSpecialS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Special options =================================\n"
    "    --SPModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TSpecialS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"SPModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
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

void TSpecialS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();

}

void TSpecialS::gmd_Start( )
{
    for(unsigned i_sp = 0; i_sp < Moduls.size(); i_sp++)
	if(Moduls[i_sp] != TO_FREE) ((TSpecial *)Moduls[i_sp])->Start( ); 
}              

void TSpecialS::gmd_Stop( )
{
    for(unsigned i_sp = 0; i_sp < Moduls.size(); i_sp++)
	if(Moduls[i_sp] != TO_FREE) ((TSpecial *)Moduls[i_sp])->Stop( ); 
}

//================================================================
//=========== TSpecial ===========================================
//================================================================
const char *TSpecial::o_name = "TSpecial";



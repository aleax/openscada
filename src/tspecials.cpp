
#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tspecials.h"

TSpecialS::TSpecialS( TKernel *app ) : TGRPModule(app,"Special") 
{

}

void TSpecialS::Start(  )
{

}

void TSpecialS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Special options =================================\n"
    "    --SPModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TSpecialS::CheckCommandLine( )
{
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

void TSpecialS::UpdateOpt()
{

}


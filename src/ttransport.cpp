#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "ttransport.h"

TTransport::TTransport(  ) : TGRPModule("Transport")
{

}

void TTransport::Start(  )
{

}

void TTransport::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --PRModPath=<path>  Set moduls <path>;\n"
    "\n",NameTMod().c_str());
}

void TTransport::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"PRModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TTransport::UpdateOpt()
{

}



#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tarhive.h"

 TArhive::TArhive(  ) : TGRPModule("Arhiv") 
{

}


 TArhive::~TArhive(  )
{

}


void TArhive::Start(  )
{

}


string TArhive::GetListArhivTip(  )
{

}


int TArhive::GetVal( string NameArhiv, string NameParm, string BegTime, int EndTime, int pereodic, char * buf )
{

}

void TArhive::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Arhive options ==================================\n"
    "    --ArhModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TArhive::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"ArhModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
    };

    optind=0,opterr=0;	
    do
    {
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath=strdup(optarg); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}



#include <getopt.h>

#include "tkernel.h"
#include "tmessage.h"
#include "tarhives.h"

 TArhiveS::TArhiveS( TKernel *app ) : TGRPModule(app,"Arhiv") 
{

}


 TArhiveS::~TArhiveS(  )
{

}

/*
void TArhiveS::Start(  )
{

}

string TArhiveS::GetListArhivTip(  )
{

}

int TArhiveS::GetVal( string NameArhiv, string NameParm, string BegTime, int EndTime, int pereodic, char * buf )
{

}*/

void TArhiveS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Arhive options ==================================\n"
    "    --ArhModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TArhiveS::CheckCommandLine( char **argv, int argc )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"ArhModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
    };

    optind=0,opterr=0;	
    do
    {
	next_opt=getopt_long(argc,argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TArhiveS::UpdateOpt()
{


}


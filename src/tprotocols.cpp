
#include <getopt.h>

#include "tkernel.h"
#include "tmessage.h"
#include "tprotocols.h"

TProtocolS::TProtocolS( TKernel *app ) : TGRPModule(app,"Protocol")
{

}

/*
int TProtocolS::Request( string BufReq, string BufReply )
{

}

void TProtocolS::Start(  )
{

}
*/
void TProtocolS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "======================= %s options ===============================\n"
    "    --PRCModPath=<path>  Set moduls <path>;\n"
    "\n",NameTMod().c_str());
}

void TProtocolS::CheckCommandLine( char **argv, int argc )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"PRCModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;	
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
}    

void TProtocolS::UpdateOpt()
{

}



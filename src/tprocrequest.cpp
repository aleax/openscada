
#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tprocrequest.h"

TProcRequest::TProcRequest(  ) : TGRPModule("ProcRequest")
{

}


int TProcRequest::Request( string BufReq, string BufReply )
{

}


void TProcRequest::Init(  )
{
    string StrPath;

    CheckCommandLine();
    LoadAll(StrPath+App->ModPath+","+DirPath);
    InitAll();
}


void TProcRequest::Start(  )
{

}

void TProcRequest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "======================= ProcRequest options ===============================\n"
    "    --PRCModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TProcRequest::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"PRCModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;	
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
}    





#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tprotocols.h"

//================================================================
//=========== TProtocolS =========================================
//================================================================
const char *TProtocolS::o_name = "TProtocolS";

TProtocolS::TProtocolS( TKernel *app ) : TGRPModule(app,"Protocol")
{

}

void TProtocolS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "======================= %s options ===============================\n"
    "    --PRCModPath=<path>  Set moduls <path>;\n"
    "\n",gmd_Name().c_str());
}



void TProtocolS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
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
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
    	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}    

void TProtocolS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();

}

//================================================================
//=========== TProtocol ==========================================
//================================================================
const char *TProtocol::o_name = "TProtocol";

TProtocol::TProtocol()
{

}

TProtocol::~TProtocol()
{

}



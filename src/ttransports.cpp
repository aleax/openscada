#include <getopt.h>

#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================

const char *TTransportS::o_name = "TTransportS";
const char *TTransportS::n_opt  = "transport";

TTransportS::TTransportS( TKernel *app ) : TGRPModule(app,"Transport")
{

}

TTransportS::~TTransportS(  )
{
    for(unsigned i_m = 0; i_m < TTransport.size(); i_m++) DelM(i_m);
}
	
void TTransportS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --TRMPath=<path>  Set moduls <path>;\n"
    "------------------ Fields <%s> sections of config file ----------------\n"
    "modules_path=<path>    set path to modules;\n"	    
    "\n",NameTMod().c_str(),n_opt);
}

void TTransportS::CheckCommandLine( char **argv, int argc )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TRMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
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

void TTransportS::UpdateOpt()
{
    try{ DirPath = owner->GetOpt(n_opt,"modules_path"); } catch(...){  }
}

int TTransportS::AddM( TModule *modul )
{
    int hd=TGRPModule::AddM(modul);
    if(hd < 0) return(hd);
    if(hd == (int)TTransport.size()) TTransport.push_back( static_cast< TTipTransport *>(modul) );
    else if(TTransport[hd]==TO_FREE) TTransport[hd] = static_cast< TTipTransport *>(modul);
    TTransport[hd]->owner = this;
    return(hd);
}

int TTransportS::DelM( int hd )
{
    if(hd >= TTransport.size() || TTransport[hd]==TO_FREE) return(-1);
    TTransport[hd] = TO_FREE;
    return(TGRPModule::DelM(hd));
}

//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::~TTipTransport()
{
    
}

//================================================================
//=========== TTransportIn =======================================
//================================================================
const char *TTransportIn::o_name = "TTransportIn";

TTransportIn::~TTransportIn()
{
    
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
const char *TTransportOut::o_name = "TTransportOut";

TTransportOut::~TTransportOut()
{
    
}


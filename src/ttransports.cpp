#include <getopt.h>

#include "tsys.h"
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

void TTransportS::DelM( unsigned hd )
{
    if(hd >= TTransport.size() || TTransport[hd]==TO_FREE) 
    	throw TError("%s: Module header %d error!",o_name,hd);	    
    TTransport[hd] = TO_FREE;
    TGRPModule::DelM(hd);
}

//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::TTipTransport()
{
    hd_res = SYS->ResCreate();
}
    
TTipTransport::~TTipTransport()
{
    SYS->ResRequest(hd_res);
    for(unsigned id=0; id < i_tr.size(); id++)
	if(i_tr[id].use > 0) { delete i_tr[id].tr; i_tr[id].use = 0; }
    for(unsigned id=0; id < o_tr.size(); id++)
	if(o_tr[id].use > 0) { delete o_tr[id].tr; o_tr[id].use = 0; }
    SYS->ResDelete(hd_res);    
}

TTransportIn *TTipTransport::atIn( unsigned int id )
{
    if(id > i_tr.size() || i_tr[id].use <= 0 ) 
	throw TError("%s: input transport id = %d - error!",o_name,id);
    return(i_tr[id].tr);
}


TTransportOut *TTipTransport::atOut( unsigned int id )
{
    if(id > o_tr.size() || o_tr[id].use <= 0 ) 
	throw TError("%s: output transport id = %d - error!",o_name,id);
    return(o_tr[id].tr);
}

unsigned TTipTransport::OpenIn(string name, string address )
{
    unsigned id;
    
    TTransportIn *tr_in = In(name,address);
    SYS->ResRequest(hd_res);
    for(id=0; id < i_tr.size(); id++) if(i_tr[id].use > 0 && i_tr[id].tr == tr_in ) break;
    if(id < i_tr.size()) i_tr[id].use++;
    else
    {
	for(id=0; id < i_tr.size(); id++) 
	    if(i_tr[id].use <= 0) break;
	if(id == i_tr.size()) i_tr.push_back();
	i_tr[id].use = 1;
	i_tr[id].tr  = tr_in;
    }    
    SYS->ResRelease(hd_res);
}

void TTipTransport::CloseIN( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > i_tr.size() || i_tr[id].use <= 0 )
    {
	SYS->ResRelease(hd_res);
	throw TError("%s: input transport id = %d - error!",o_name,id);
    }
    if((--i_tr[id].use) <= 0 ) delete i_tr[id].tr;
    SYS->ResRelease(hd_res);
}

unsigned TTipTransport::OpenOut(string name, string address )
{
    unsigned id;
    
    TTransportOut *tr_out = Out(name,address);
    SYS->ResRequest(hd_res);
    for(id=0; id < o_tr.size(); id++) if(o_tr[id].use > 0 && o_tr[id].tr == tr_out ) break;
    if(id < o_tr.size()) o_tr[id].use++;
    else
    {
	for(id=0; id < o_tr.size(); id++) 
	    if(o_tr[id].use <= 0) break;
	if(id == o_tr.size()) o_tr.push_back();
	o_tr[id].use = 1;
	o_tr[id].tr  = tr_out;
    }    
    SYS->ResRelease(hd_res);
}

void TTipTransport::CloseOut( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > o_tr.size() || o_tr[id].use <= 0 ) 
    {
	SYS->ResRelease(hd_res);	
	throw TError("%s: output transport id = %d - error!",o_name,id);
    }
    if((--o_tr[id].use) <= 0 ) delete o_tr[id].tr;    
    SYS->ResRelease(hd_res);	
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


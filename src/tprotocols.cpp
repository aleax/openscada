
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

TProtocol::TProtocol() : m_hd(o_name)
{

}

TProtocol::~TProtocol()
{
    m_hd.lock();
    SYS->event_wait( m_hd.obj_free(), true, string(o_name)+": input protocols are closing...." );
}

unsigned TProtocol::open( string name )
{
    TProtocolIn *t_prt = in_open(name);
    try { m_hd.obj_add( t_prt, &t_prt->Name() ); }
    catch(TError err) { delete t_prt; }
    return( m_hd.hd_att( t_prt->Name() ) );
}

void TProtocol::close( unsigned hd )
{
    string name = at(hd).Name();
    m_hd.hd_det( hd );
    if( !m_hd.obj_use( name ) )
	delete (TProtocolIn *)m_hd.obj_del( name );
}

//================================================================
//=========== TProtocolIn ========================================
//================================================================
const char *TProtocolIn::o_name = "TProtocolIn";

TProtocolIn::TProtocolIn( string name, TProtocol *owner ) : m_name(name), m_wait(false), m_owner(owner)
{

}

TProtocolIn::~TProtocolIn()
{

}


#include <getopt.h>
#include <signal.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../ttransports.h"
#include "test_transp.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_transp"
#define NAME_TYPE   "Special"
#define SUB_TYPE    "TEST"
#define VERSION     "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Transport test module: IO mess, ... ."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule *attach( char *FName, int n_mod )
    {
	TranspTest::TTest *self_addr;
	if(n_mod==0) self_addr = new TranspTest::TTest( FName );
	else         self_addr = NULL;
	return ( self_addr );
    }
}

using namespace TranspTest;

//==============================================================================
//================= BDTest::TTest ==============================================
//==============================================================================
TTest::TTest(char *name)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = name;
}

TTest::~TTest()
{

}

string TTest::mod_info( const string name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TTest::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

void TTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TTest::mod_CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTest::mod_UpdateOpt( )
{

}

void TTest::Start(  )
{
    signal(SIGPIPE,SYS->sighandler);
    
    Mess->put("TEST",MESS_DEBUG,"***** Begin <%s> test block *****",NAME_MODUL);
    
    int len;
    char *buf = (char *)malloc(200);
    string comm = "time";
    TTransportS &trans = Owner().Owner().Transport();    
    try
    {
	SHDTr HDTr = trans.out_att( STrS("socket","TCP2") );
	try { len = trans.out_at( HDTr ).IOMess((char *)comm.c_str(),comm.size(),buf,199,1); }
	catch(...) { trans.out_det( HDTr ); throw; }
       	trans.out_det( HDTr );       
        buf[len] = 0; 
        Mess->put("TEST",MESS_DEBUG,"%s: TCP Put <%s>. Get: <%s>",NAME_MODUL,comm.c_str(),buf);
    } catch(TError error) { Mess->put("TEST",MESS_DEBUG,"%s: %s",NAME_MODUL,error.what().c_str()); }
    try
    {
	SHDTr HDTr = trans.out_att( STrS("socket","UNIX2") );
	try{ len = trans.out_at( HDTr ).IOMess((char *)comm.c_str(),comm.size(),buf,199,1); }
	catch(...) { trans.out_det( HDTr ); throw; }
       	trans.out_det( HDTr );       
	buf[len] = 0; 
	Mess->put("TEST",MESS_DEBUG,"%s: UNIX Put <%s>. Get: <%s>",NAME_MODUL,comm.c_str(),buf);
    } catch(TError error) { Mess->put("TEST",MESS_DEBUG,"%s: %s",NAME_MODUL,error.what().c_str()); }
    try
    {
	SHDTr HDTr = trans.out_att( STrS("socket","UDP2") );
	try{ len = trans.out_at( HDTr ).IOMess((char *)comm.c_str(),comm.size(),buf,199,1); }
	catch(...) { trans.out_det( HDTr ); throw; }
       	trans.out_det( HDTr );       
	buf[len] = 0; 
	Mess->put("TEST",MESS_DEBUG,"%s: UDP Put <%s>. Get: <%s>",NAME_MODUL,comm.c_str(),buf);
    } catch(TError error) { Mess->put("TEST",MESS_DEBUG,"%s: %s",NAME_MODUL,error.what().c_str()); }
    free(buf);
    //trans.UpdateBD();
    
    Mess->put("TEST",MESS_DEBUG,"***** End <%s> test block *****",NAME_MODUL);
}


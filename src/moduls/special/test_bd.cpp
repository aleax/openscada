#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tbds.h"
#include "test_bd.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_bd"
#define NAME_TYPE   "Special"
#define SUB_TYPE    "TEST"
#define VERSION     "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD test module: Open BD, ... ."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule *attach( char *FName, int n_mod )
    {
	BDTest::TTest *self_addr;
	if(n_mod==0) self_addr = new BDTest::TTest( FName );
	else         self_addr = NULL;
	return ( self_addr );
    }
}

using namespace BDTest;

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
    Mess->put("TEST",MESS_DEBUG,"***** Begin <%s> test block *****",NAME_MODUL);
    TBDS &bd = Owner().Owner().BD();    
    //------------------- Test MySQL BD -----------------------
    try
    {
	SHDBD t_hd = bd.open( SBDS("my_sql",";;;oscada;;/var/lib/mysql/mysql.sock;","generic"), true);
	Mess->put("TEST",MESS_DEBUG,"%s: Open table hd = %d",NAME_MODUL,t_hd);
	string val = bd.at(t_hd).GetCodePage( );
	Mess->put("TEST",MESS_DEBUG,"%s: table val = %s",NAME_MODUL,val.c_str());
	bd.close(t_hd);
    }catch(TError error)
    { Mess->put("TEST",MESS_DEBUG,"%s: %s",NAME_MODUL,error.what().c_str()); }
    Mess->put("TEST",MESS_DEBUG,"***** End <%s> test block *****",NAME_MODUL);
}



#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tmodule.h"
#include "self.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "self"
#define NAME_TYPE   "Protocol"
#define VER_TYPE    VER_PROT
#define VERSION     "0.0.2"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Self OpenScada protocol, support generic functions."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    SAtMod module( int n_mod )
    {
	SAtMod AtMod;

	if(n_mod==0)
    	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
    	    AtMod.name  = "";

	return( AtMod );
    }

    TModule *attach( SAtMod &AtMod, string source )
    {
	pr_self::TProt *self_addr = NULL;

    	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new pr_self::TProt( source );

	return ( self_addr );
    }
}

using namespace pr_self;

TProt::TProt( string name ) 
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TProt::~TProt()
{

}

void TProt::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TProt::mod_CheckCommandLine( )
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

TProtocolIn *TProt::in_open( string name )
{
    return( new TProtIn(name,this) );
}

//================================================================
//=========== TProtIn ============================================
//================================================================
TProtIn::TProtIn( string name, TProtocol *owner) : TProtocolIn( name, owner )
{

}

TProtIn::~TProtIn()
{

}

void TProtIn::mess(string &request, string &answer )
{
    if( request == "time" )
    {
	time_t tm = time(NULL);
	char *c_tm = ctime( &tm );
	for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
	    if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
	answer = c_tm;
    }
    else answer = "ERROR: request no support!\n";
}



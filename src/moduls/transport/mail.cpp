/* Test Modul
** ==============================================================
*/

#include <unistd.h>
#include <getopt.h>
#include <string>
#include <errno.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tmodule.h"
#include "mail.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "mail"
#define NAME_TYPE   "Transport"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Transport based to mail protocols: IMAP, POP, SMTP"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

//==============================================================================
//== TTransMail ================================================================
//==============================================================================

TTransMail::TTransMail(char *name) 
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

    ExpFunc   = NULL; // (SExpFunc *)ExpFuncLc;
    NExpFunc  = 0; // sizeof(ExpFuncLc)/sizeof(SExpFunc);
}

TTransMail::~TTransMail()
{
    free(FileName);
}

TModule *attach( char *FName, int n_mod )
{
    TTransMail *self_addr;
    if(n_mod==0) self_addr = new TTransMail( FName );
    else         self_addr = NULL;
    return static_cast< TModule *>( self_addr );
}

void TTransMail::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TTransMail::mod_CheckCommandLine(  )
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
 
void TTransMail::mod_UpdateOpt()
{
    
}

TTransportIn *TTransMail::In(string name, string address, string prot )
{
    TMailIn *sock = new TMailIn(name,address,prot,this);
    return(sock);
}

TTransportOut *TTransMail::Out(string name, string address )
{
    return(new TMailOut(name,address));
}

//==============================================================================
//== TMailIn =================================================================
//==============================================================================

TMailIn::TMailIn(string name, string address, string prot, TTipTransport *owner) 
    : TTransportIn(name,address,prot,owner)
{
    
}

TMailIn::~TMailIn()
{
    
}

//==============================================================================
//== TMailOut ================================================================
//==============================================================================

TMailOut::TMailOut(string name, string address) : TTransportOut(name,address)
{
    
}

TMailOut::~TMailOut()
{
    
}


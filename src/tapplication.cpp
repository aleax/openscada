
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include <iostream>
#include <new>

#include "../config.h"
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tprotocol.h"
#include "tprocrequest.h"
#include "tarhive.h"
#include "tcontrollers.h"
#include "tspecial.h"
#include "tparams.h"
#include "tgui.h"
#include "tmodschedul.h"


TApplication::TApplication( int argi, char ** argb ) 
            : d_level(8), UserName(strdup(getenv("USER"))), argc(argi), argv((const char **)argb),
	      log_dir(2), ModPath("./"), InternCharset("UTF8")	    
{
     
    //auto_ptr<TMessage> Mess (new TMessage());
    Param    = new TParamS();
    Mess     = new TMessage();
    BD 	     = new TBD();
    Protocol = new TProtocol();
    ProcRequest = new TProcRequest();
    Arhive   = new TArhive();
    Controller  = new TControllerS();
    Special  = new TSpecial();
    GUI      = new TGUI();
    ModSchedul  = new TModSchedul();

    ModSchedul->RegGroupM(BD);
    ModSchedul->RegGroupM(Controller);    
    ModSchedul->RegGroupM(Arhive);
    ModSchedul->RegGroupM(Protocol);
    ModSchedul->RegGroupM(ProcRequest);
    ModSchedul->RegGroupM(Special);    
    ModSchedul->RegGroupM(GUI);    
}

TApplication::~TApplication()
{
#if debug 
    Mess->put(0, "Server close!");
#endif
    delete ModSchedul;
    delete GUI;
    delete Special;
    delete Controller;
    delete Arhive;
    delete ProcRequest;
    delete Protocol;
    delete BD;
    delete Mess;
    delete Param;
}

int TApplication::run()
{
#if debug 
    Mess->put(0, "Server start!");
#endif

    try
    {
	CheckCommandLine();
	
	ModSchedul->CheckCommandLine(); 
	ModSchedul->LoadAll();
	ModSchedul->InitAll();	

	CheckCommandLine(true);   //check help, error and exit
	
	ModSchedul->StartAll();	
	ModSchedul->StartSched();	
    } 
    catch(std::bad_alloc exception) 
    { Mess->put(7,"Возникло исключение %s",(char *)exception.what()); return(-1); }
    catch(...)
    { return(-2); }
    //Start signal listen
    return(Mess->Start());
}

int TApplication::RegFunc( char * NameTipM, char * NameMod, char * NameFunc, void * addr, string NameSrcMod )
{

}

int TApplication::ReloadBD( string mode )
{

}


int TApplication::SaveBD(  )
{

}


void TApplication::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "****************************************\n"
    "**** %s v%s (Linux). ****\n" 
    "****************************************\n\n"   
    "===========================================================================\n"
    "============================ General options ==============================\n"
    "===========================================================================\n"
    "-h, --help           Info message for server's options;\n"
    "-d, --debug=<level>  Set <level> debug (0-8);\n"
    "    --log=<direct>   Set direction a log and other info;\n" 
    "			    <direct> & 1 - syslogd;\n"
    "			    <direct> & 2 - stdout;\n"
    "			    <direct> & 4 - stderr;\n"
    "    --ModPath=<path> Set moduls <path>: \"/var/os/modules/,./mod/\"\n"
    "\n",PACKAGE,VERSION);
}


void TApplication::CheckCommandLine( bool mode )
{
    int i,next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{"debug"   ,1,NULL,'d'},
	{"log"     ,1,NULL,'l'},
	{"ModPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(argc,(char * const *)argv,short_opt,long_opt,NULL);
	if(mode==false)
	{
    	    switch(next_opt)
    	    {
    		case 'h': pr_opt_descr(stdout); break;
    		case 'd': i=atoi(optarg); if(i>=0&&i<=8) d_level=i; break;
    		case 'l': log_dir=atoi(optarg); break;
    		case 'm': ModPath=strdup(optarg); break;
    		case -1 : break;
    	    }
	}
	else if(next_opt=='h') exit(0);
    } while(next_opt != -1);
/*  
    if(optind < argc) 
    {
	if(mode==false)
	{
	    fprintf(stdout,"Error Option\n");	
	    pr_opt_descr(stdout);
	}
	else exit(0);
    }
*/    
}




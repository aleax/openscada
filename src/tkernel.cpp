
#include <stdlib.h>
#include <langinfo.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <new>

#include "../config.h"
#include "terror.h"
#include "tmessage.h"
#include "tbds.h"
#include "ttransports.h"
#include "tprotocols.h"
#include "tarhives.h"
#include "tcontrollers.h"
#include "tspecials.h"
#include "tparams.h"
#include "tuis.h"
#include "tmodschedul.h"
#include "tsys.h"
#include "tkernel.h"

TMessage   *Mess;
TSYS       *SYS;

const char *TKernel::n_opt = "generic";
const char *TKernel::o_name = "TKernel";

TKernel::TKernel( string name ) : ModPath("./"), DefBDType(""), DefBDName(""), m_name(name)
{
    Mess->put("INFO",MESS_INFO,"%s kernel <%s> create!",PACKAGE,m_name.c_str());
    
    //auto_ptr<TMessage> Mess (new TMessage());
    param    = new TParamS(this);
    bd 	     = new TBDS(this);
    transport = new TTransportS(this);
    protocol = new TProtocolS(this);
    arhive   = new TArhiveS(this);
    controller  = new TControllerS(this);
    special  = new TSpecialS(this);
    ui       = new TUIS(this);

    modschedul  = new TModSchedul(this);
    ModSchedul().RegGroupM(bd);
    ModSchedul().RegGroupM(transport);
    ModSchedul().RegGroupM(protocol);
    ModSchedul().RegGroupM(controller);    
    ModSchedul().RegGroupM(arhive);
    ModSchedul().RegGroupM(special);    
    ModSchedul().RegGroupM(ui);    
}

TKernel::~TKernel()
{
    Mess->put("INFO",MESS_INFO,"%s kernel <%s> destroy!",PACKAGE,m_name.c_str());
    
    delete modschedul;
    delete ui;
    delete param;
    delete arhive;
    delete controller;
    delete protocol;
    delete transport;
    delete special;
    delete bd;
}

int TKernel::run()
{
    Mess->put("INFO",MESS_INFO,"%s kernel <%s> start!",PACKAGE,m_name.c_str());

    try
    {
	CheckCommandLine();
	UpdateOpt();
	
	ModSchedul().CheckCommandLine(); 
	ModSchedul().UpdateOpt();
	
	ModSchedul().LoadAll();
	CheckCommandLine(true);   //check help, error and exit
	
	ModSchedul().InitAll();	
	ModSchedul().StartAll();	
	ModSchedul().StartSched();	
    } 
    catch(TError error) 
    { 
	Mess->put("SYS",MESS_CRIT,"%s: %s Run exception: %s",o_name,m_name.c_str(),error.what().c_str()); 
	return(-1); 
    }
    catch(...)
    { return(-2); }
    //Start signal listen
    return(0);
}


void TKernel::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============================  Kernel options ==============================\n"
    "    --ModPath=<path>   Set modules <path>: \"/var/os/modules/,./mod/\"\n"
    "--------------- Fields <%s> sections of config file -------------------\n"
    "mod_path=<path>         set path to shared libs;\n"
    "mod_auto=<list>         name automatic loaded,  attached and started shared libs <direct_dbf.so;virt.so>\n"
    "DefaultBD = <type:name> set default bd type and bd name (next, may use only table name);\n"
    "\n",n_opt);
}


void TKernel::CheckCommandLine( bool mode )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read commandline options!",o_name);
#endif
	
    int next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"ModPath"  ,1,NULL,'m'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	if(mode==false)
	{
    	    switch(next_opt)
    	    {
    		case 'h': pr_opt_descr(stdout); break;
    		case 'm': ModPath = optarg; break;
    		case -1 : break;
    	    }
	}
	else if(next_opt == 'h') throw TError("Comandline help print!");
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

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Read commandline options ok!",o_name);
#endif
}

void TKernel::UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read config options!",o_name);
#endif

    string opt;
    
    try{ ModPath = XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }

    try
    {
	opt = XMLCfgNode()->get_child("mod_auto")->get_text();
	if( opt.size() )
	{
	    int i_beg = -1;
    	    auto_m_list.clear();
	    do
	    {
		auto_m_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
		i_beg = opt.find(";",i_beg+1);
	    } while(i_beg != (int)string::npos);
	}
    }
    catch(...) {  }
    
    try
    {
	opt = XMLCfgNode()->get_child("DefaultBD")->get_text();
	if( opt.size() )
    	{
    	    int pos = 0;
    	    DefBDType = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    	    DefBDName = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	}
    }
    catch(...) {  }

    ModSchedul().UpdateOpt();
    ModSchedul().UpdateOptMod();    
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Read config options ok!",o_name);
#endif
}

XMLNode *TKernel::XMLCfgNode()
{    
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = SYS->XMLCfgNode()->get_child("kernel",i_k++); 
	if( t_n->get_attr("id") == m_name) return( t_n );
    }
}



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

TKernel::TKernel( string name ) : ModPath("./"), DefBDType(""), DefBDName(""), m_name(name)
{
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
#if OSC_DEBUG 
    Mess->put(0,"%s kernel <%s> stop!",PACKAGE,m_name.c_str());
#endif
    delete modschedul;
    delete ui;
    delete special;
    delete controller;
    delete arhive;
    delete protocol;
    delete transport;
    delete bd;
    delete param;
}

int TKernel::run()
{
#if OSC_DEBUG 
    Mess->put(0,"%s kernel <%s> start!",PACKAGE,m_name.c_str());
#endif

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
    { Mess->put(7,"Go exception: %s",error.what().c_str()); return(-1); }
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
    "mod_path=<path>         set path to modules;\n"
    "mod_allow=<list>        name allowed modules for attach <direct_dbf.so;virt.so>\n"
    "                         (free list - allow all modules);\n"
    "mod_deny=<list>         name denyed modules for attach <direct_dbf.so;virt.so>;\n"
    "                         (free list - allow all modules);\n"
    "DefaultBD = <type:name> set default type and name bd (next, may use anly table name);\n"
    "\n",n_opt);
}


void TKernel::CheckCommandLine( bool mode )
{
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

void TKernel::UpdateOpt()
{
    string opt;
    
    try{ ModPath = XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }

    try
    {
	opt = XMLCfgNode()->get_child("mod_allow")->get_text();
	if( opt.size() )
	{
	    int i_beg = -1;
    	    allow_m_list.clear();
	    do
	    {
		allow_m_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
		i_beg = opt.find(";",i_beg+1);
	    } while(i_beg != (int)string::npos);
	}
    }
    catch(...) {  }

    try
    {
	opt = XMLCfgNode()->get_child("mod_deny")->get_text();
	if( opt.size() )
    	{
    	    int i_beg = -1;
    	    deny_m_list.clear();
    	    do
    	    {
    		deny_m_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
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
    
    //if( SYS->GetOpt(n_opt,"modules_path",opt) ) ModPath = opt;
    /*
    allow_m_list.clear();
    if( SYS->GetOpt(n_opt,"mod_allow",opt) && opt.size() )
    {
	int i_beg = -1;
	do
	{
	    allow_m_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
	    i_beg = opt.find(";",i_beg+1);
	} while(i_beg != (int)string::npos);
    }
    
    deny_m_list.clear();
    if( SYS->GetOpt(n_opt,"mod_deny",opt) && opt.size() )
    {
	int i_beg = -1;
	do
	{
	    deny_m_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
	    i_beg = opt.find(";",i_beg+1);
	} while(i_beg != (int)string::npos);
    }
    if( SYS->GetOpt(n_opt,"DefaultBD",opt) && opt.size() )
    {
	int pos = 0;
	DefBDType = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	DefBDName = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    */
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


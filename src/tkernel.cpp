
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
#include "tguis.h"
#include "tmodschedul.h"
#include "tsys.h"
#include "tkernel.h"

TMessage   *Mess;
TSYS       *SYS;

const char *TKernel::n_opt = "generic";

TKernel::TKernel(  ) : ModPath("./"), dir_cfg(false), DefBDType(""), DefBDName("")
{
    //auto_ptr<TMessage> Mess (new TMessage());
    Param    = new TParamS(this);
    BD 	     = new TBDS(this);
    Transport = new TTransportS(this);
    Protocol = new TProtocolS(this);
    Arhive   = new TArhiveS(this);
    Controller  = new TControllerS(this);
    Special  = new TSpecialS(this);
    GUI      = new TGUIS(this);

    ModSchedul  = new TModSchedul(this);
    ModSchedul->RegGroupM(BD);
    ModSchedul->RegGroupM(Transport);
    ModSchedul->RegGroupM(Protocol);
    ModSchedul->RegGroupM(Controller);    
    ModSchedul->RegGroupM(Arhive);
    ModSchedul->RegGroupM(Special);    
    ModSchedul->RegGroupM(GUI);    
}

TKernel::~TKernel()
{
#if OSC_DEBUG 
    Mess->put(0,"%s kernel stop!",PACKAGE);
#endif
    delete ModSchedul;
    delete GUI;
    delete Special;
    delete Controller;
    delete Arhive;
    delete Protocol;
    delete Transport;
    delete BD;
    delete Param;
}

int TKernel::run()
{
#if OSC_DEBUG 
    Mess->put(0,"%s kernel start!",PACKAGE);
#endif

    try
    {
	CheckCommandLine();
	UpdateOpt();
	
	ModSchedul->CheckCommandLine(); 
	ModSchedul->UpdateOpt();
	
	ModSchedul->LoadAll();
	ModSchedul->CheckCommandLineMod(); 
	ModSchedul->UpdateOptMod();
	CheckCommandLine(true);   //check help, error and exit
	
	ModSchedul->InitAll();	
	ModSchedul->StartAll();	
	ModSchedul->StartSched();	
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
    "modules_path=<path>     set path to modules;\n"
    "mod_allow=<list>        name allowed modules for attach <direct_dbf.so;virt.so>\n"
    "                        (free list - allow all modules);\n"
    "mod_deny=<list>         name denyed modules for attach <direct_dbf.so;virt.so>;\n"
    "                        (free list - allow all modules);\n"
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
    
    if( SYS->GetOpt(n_opt,"modules_path",opt) ) ModPath = opt;

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
}

/*
string TKernel::GetOpt(string section, string opt)
{
    int line  = 0,  // file's line
	f_cnt,      // file cntr 
	i;          // generic index;
    bool f_beg  = false,  // command begin
         f_sect = false;  // section ok
    string str; 
    char *func = "GetOpt";
    
    int hd = open(Conf_File.c_str(),O_RDONLY);
    if(hd < 0) throw TError("%s: no config file!",func);
    int cf_sz = lseek(hd,0,SEEK_END);
    lseek(hd,0,SEEK_SET);
    char *buf = (char *)malloc(cf_sz);
    read(hd,buf,cf_sz);
    close(hd);

    for(f_cnt = 0; f_cnt < cf_sz; f_cnt++)
    {
	if(buf[f_cnt] == '[' && f_beg == false )
	{   
	    if( f_sect == true ) { free(buf); throw TError("%s: option <%s> no avoid!",func,opt.c_str()); }
	    for(i = (++f_cnt); i < cf_sz && buf[i]!=']'; i++);
	    if(i >= cf_sz){ free(buf); throw TError("%s: Config file error (line: %d)!",func,line); }
	    str.assign(&buf[f_cnt],i-f_cnt);
	    if(str == section) f_sect = true;
	    f_cnt = i+1;
	}
	if(buf[f_cnt] == 0x20 || buf[f_cnt] == 0x09) continue;
	if(buf[f_cnt] == 0x0A || buf[f_cnt] == 0x0D) 
	{ 
	    if(buf[f_cnt] == 0x0A) line++; 
	    f_beg = false; 
	    continue; 
	}
	if(buf[f_cnt] == '#')
	{
	    for(; f_cnt < cf_sz && buf[f_cnt] != 0x0A && buf[f_cnt] != 0x0D; f_cnt++);
	    f_cnt--;
	    continue;
	}
	if(f_sect == true && f_beg == false )
	{
	    int last_i, first_i;
	    bool first;
	    for( last_i = i = f_cnt; i < cf_sz && buf[i] != '=' && buf[i] != 0x0A && buf[i] != 0x0D; i++)
		if(buf[i] != 0x20 && buf[i] != 0x09) last_i = i;
	    if(i >= cf_sz || buf[i] == 0x0A || buf[i] == 0x0D )
	    { free(buf); throw TError("%s: config file error (line: %d)!",func,line); }
	    //if(buf[i] == 0x0A || buf[i] == 0x0D) { f_cnt=i-1; continue; }
	    str.assign(&buf[f_cnt],last_i-f_cnt+1);
	    if(str != opt) { f_beg = true; f_cnt=i; continue; }
	    f_cnt = i+1; first = false;
	    for(first_i = last_i = i = f_cnt; i < cf_sz && buf[i] != 0x0A && buf[i] != 0x0D; i++)
		if(buf[i] != 0x20 && buf[i] != 0x09) 
		{ 
		    if(first == false) { first_i = i; first = true; } 
		    last_i = i; 
		}
	    if(first == true) str.assign(&buf[first_i],last_i-first_i+1);
	    else              str = "";
	    free(buf);

	    return(str);	    
	}
	f_beg = true;
    }
    free(buf);    
    if(f_sect == true) throw TError("%s: option <%s> no avoid!",func,opt.c_str());
    throw TError("%s: section <%s> no avoid!",func,section.c_str());
}
*/


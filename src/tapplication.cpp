
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
#include "tbd.h"
#include "tprotocol.h"
#include "tprocrequest.h"
#include "tarhive.h"
#include "tcontrollers.h"
#include "tspecial.h"
#include "tparams.h"
#include "tgui.h"
#include "tmodschedul.h"
#include "tapplication.h"

const char *TApplication::n_opt = "generic";

TApplication::TApplication( int argi, char ** argb, char **env ) 
            : d_level(8), User(getenv("USER")), argc(argi), envp((const char **)env), argv((const char **)argb),
	      log_dir(2), ModPath("./"), IO_Char_Set("UTF8"), dir_cfg(false), Conf_File("./oscada.conf") 
{
//    IO_Char_Set = nl_langinfo(CODESET);
     
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
    Mess->put(0,"%s close!",PACKAGE);
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
    Mess->put(0,"%s start!",PACKAGE);
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
    { Mess->put(7,"Возникло исключение %s",error.what().c_str()); return(-1); }
    catch(...)
    { return(-2); }
    //Start signal listen
    return(Mess->Start());
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
    "-h, --help             Info message for server's options;\n"
    "-d, --debug=<level>    Set <level> debug (0-8);\n"
    "    --log=<direct>     Set direction a log and other info;\n" 
    "			      <direct> & 1 - syslogd;\n"
    "			      <direct> & 2 - stdout;\n"
    "			      <direct> & 4 - stderr;\n"
    "    --Config=<path>    Config file path;\n"
    "    --ModPath=<path>   Set modules <path>: \"/var/os/modules/,./mod/\"\n"
    "    --IOCharset=<name> Set io charset;\n"
    //"    --DirConf          Enable direct access to BD for configs, without save it to memory.\n"
    "--------------- Fields <%s> sections of config file -------------------\n"
    "debug=<level>          set debug level (0-8);\n"
    "target_log=<direction> set direction a log and other info;\n"
    "modules_path=<path>    set path to modules;\n"
    "io_chrset=<charset>    set io charset;\n"
    "\n",PACKAGE,VERSION,n_opt);
}


void TApplication::CheckCommandLine( bool mode )
{
    int i,next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"debug"    ,1,NULL,'d'},
	{"log"      ,1,NULL,'l'},
	{"ModPath"  ,1,NULL,'m'},
	{"IOCharset",1,NULL,'c'},
	{"Config"   ,1,NULL,'f'},
	//{"DirConf"  ,0,NULL,'i'},
	{NULL       ,0,NULL,0  }
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
    		case 'd': i = atoi(optarg); if(i>=0&&i<=8) d_level=i; break;
    		case 'l': log_dir = atoi(optarg); break;
    		case 'm': ModPath = optarg; break;
    		case 'c': IO_Char_Set = optarg; break;
    		case 'f': Conf_File = optarg; break;
    		//case 'i': dir_cfg = true; break;
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

void TApplication::UpdateOpt()
{
    string opt;
    
    try
    {
	opt = GetOpt(n_opt,"debug");
	int i = atoi(opt.c_str()); 
	if(i>=0&&i<=8) d_level=i;
    }catch(...){ }
    try{ log_dir     = atoi(GetOpt(n_opt,"target_log").c_str()); } catch(...){  }
    try{ ModPath     = GetOpt(n_opt,"modules_path"); }             catch(...){  }
    try{ IO_Char_Set = GetOpt(n_opt,"io_charset"); }               catch(...){  }
}

string TApplication::GetOpt(string section, string opt)
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
	    for(i = f_cnt; i < cf_sz && buf[i] != '=' && buf[i] != 0x0A && buf[i] != 0x0D; i++);
	    if(i >= cf_sz || buf[i] == 0x0A || buf[i] == 0x0D )
	    { free(buf); throw TError("%s: config file error (line: %d)!",func,line); }
	    //if(buf[i] == 0x0A || buf[i] == 0x0D) { f_cnt=i-1; continue; }
	    str.assign(&buf[f_cnt],i-f_cnt);
	    if(str != opt) { f_beg = true; f_cnt=i; continue; }
	    f_cnt = i+1;
	    for(i = f_cnt; i < cf_sz && buf[i] != 0x0A && buf[i] != 0x0D; i++);
	    str.assign(&buf[f_cnt],i-f_cnt);
	    free(buf);
	    return(str);	    
	}
	f_beg = true;
    }
    free(buf);    
    if(f_sect == true) throw TError("%s: option <%s> no avoid!",func,opt.c_str());
    throw TError("%s: section <%s> no avoid!",func,section.c_str());
}



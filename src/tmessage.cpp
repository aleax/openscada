#include <sys/types.h>
#include <sys/wait.h>

#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <iconv.h>
#include <langinfo.h>
#include <getopt.h>
#include <stdlib.h>

#include "terror.h"
#include "tsys.h"
#include "tmessage.h"

const char *TMessage::n_opt  = "generic";

TMessage::TMessage(  ) : stop_signal(0), IOCharSet("UTF8"), d_level(8), log_dir(2)
{
    openlog("OpenScada",0,LOG_USER);
    SetCharset(nl_langinfo(CODESET));
    CheckCommandLine();
    UpdateOpt();
}

void TMessage::sighandler( int signal )
{
    if(signal == SIGINT) 
    { 
//	Mess->put(3,"Have get a Interrupt signal. No stop server!");
	Mess->stop_signal=signal; 
    }
    else if(signal == SIGTERM) 
    { 
	Mess->put(3,"Have get a Terminate signal. Server been stoped!"); 
	Mess->stop_signal=signal; 
    }
    else if(signal == SIGCHLD)
    {
	int status;
	pid_t pid = wait(&status);
	if(!WIFEXITED(status))
	    Mess->put(3,"Stop child process %d!",pid);
    }	
}

TMessage::~TMessage(  )
{
    closelog();
}

int TMessage::Start(  )
{
    struct sigaction sa;
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler= sighandler;
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGCHLD,&sa,NULL);
    
    while(1)	
	if(stop_signal) break;   
       	else            sleep(100); 

//    sa.sa_handler= SIG_DFL;
//    sigaction(SIGINT,&sa,NULL);
//    sigaction(SIGTERM,&sa,NULL);        

    return(stop_signal);       
}

// Уровень отладки (d_level) может изменяться в пределах 0-8 включительно:
// 0 - не выводить никаких сообщений вообще 
// 8 - максимальный уровень отладки
// Уровень сообщения (level) характерезует его приоритетность и изменяется в пределах 0-7:
// 0 - отладочный уровень;
// 7 - уровень высочайшей степени аварии;

void TMessage::put( int level, char * fmt,  ... )
{
    char str[256];                  //!!!!
    va_list argptr;

    va_start (argptr,fmt);
    vsprintf(str,fmt,argptr);
    va_end(argptr);
    if(level<0) level=0; if(level>7) level=7;
    if(level>=(8-d_level)) 
    {
	int level_sys=LOG_DEBUG;
	if(level<1)       level_sys=LOG_DEBUG;
	else if(level==1) level_sys=LOG_INFO;
	else if(level==2) level_sys=LOG_NOTICE;
	else if(level==3) level_sys=LOG_WARNING;
	else if(level==4) level_sys=LOG_ERR;
	else if(level==5) level_sys=LOG_CRIT;
	else if(level==6) level_sys=LOG_ALERT;
	else if(level==7) level_sys=LOG_EMERG;
	if(log_dir&1) syslog(level_sys,str);
	if(log_dir&2) fprintf(stdout,"%s \n",str);
	if(log_dir&4) fprintf(stderr,"%s \n",str);
    }
}

int TMessage::SconvIn(const char *fromCH, string & buf)
{
    return( Sconv(fromCH, IOCharSet.c_str(), buf) );
}    
int TMessage::SconvOut(const char *toCH, string & buf)
{
    return( Sconv( IOCharSet.c_str(), toCH , buf) );
}

int TMessage::Sconv(const char *fromCH, const char *toCH, string & buf)
{
    //Make convert to blocks 100 bytes !!!    
    char   *ibuf, outbuf[100], *obuf;
    size_t ilen, olen;
    iconv_t hd;
    
    hd = iconv_open(toCH, fromCH);
    if( hd == (iconv_t)(-1) ) return(-1);
    
    string t_buf = buf; 
    buf.erase();
    ibuf = (char *)t_buf.c_str();
    ilen = t_buf.size();
    
    while(ilen)
    {
	obuf = outbuf;
	olen = sizeof(outbuf)-1;
	iconv(hd,&ibuf,&ilen,&obuf,&olen);
	buf.append(outbuf,sizeof(outbuf)-1-olen);
    }
    iconv_close(hd);
    
    return(0);
}

void TMessage::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============================ Message options ==============================\n"
    "-d, --debug=<level>    Set <level> debug (0-8);\n"
    "    --log=<direct>     Set direction a log and other info;\n"
    "                         <direct> & 1 - syslogd;\n"
    "                         <direct> & 2 - stdout;\n"
    "                         <direct> & 4 - stderr;\n"
    "    --IOCharset=<name> Set io charset;\n"
    "--------------- Fields <%s> sections of config file -------------------\n"
    "debug      = <level>     set <level> debug (0-8);\n"
    "target_log = <direction> set direction a log and other info;\n"
    "                           <direct> & 1 - syslogd;\n"
    "                           <direct> & 2 - stdout;\n"
    "                           <direct> & 4 - stderr;\n"
    "io_chrset  = <charset>   set io charset;\n"
    "\n",n_opt);
}

void TMessage::CheckCommandLine( )
{
    int i,next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"debug"    ,1,NULL,'d'},
	{"log"      ,1,NULL,'l'},
	{"IOCharset",1,NULL,'c'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'd': i = atoi(optarg); if(i>=0&&i<=8) SetDLevel(i); break;
	    case 'l': SetLogDir(atoi(optarg)); break;
	    case 'c': SetCharset(optarg); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TMessage::UpdateOpt()
{
    string opt;

    try
    {
	int i = atoi(SYS->XMLCfgNode()->get_child("debug")->get_text().c_str());
	if( i >= 0 && i <= 8 ) SetDLevel(i);
    }catch(...) { }
    try{ SetLogDir(atoi(SYS->XMLCfgNode()->get_child("target_log")->get_text().c_str())); }
    catch(...) { }
    try{ SetCharset(SYS->XMLCfgNode()->get_child("io_charset")->get_text()); }
    catch(...) { }
    
    /*
    if( SYS->GetOpt(n_opt,"debug",opt) )
    {
	int i = atoi(opt.c_str());
	if(i>=0&&i<=8) SetDLevel(i);
    }
    */    
    //if( SYS->GetOpt(n_opt,"target_log",opt) )               SetLogDir(atoi(opt.c_str()));
    //if( SYS->GetOpt(n_opt,"io_charset",opt) && opt.size() ) SetCharset(opt); 
}


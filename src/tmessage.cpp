
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "tmessage.h"
#include "tapplication.h"

TMessage::TMessage(  ) : stop_signal(0)
{
    openlog("OpenScada",0,LOG_USER);    
}

void TMessage::sighandler( int signal )
{
    if(signal==SIGINT) 
    { 
//	App->Mess->put(3,"Have get a Interrupt signal. No stop server!");
	App->Mess->stop_signal=signal; 
    }
    if(signal==SIGTERM) 
    { 
	App->Mess->put(3,"Have get a Terminate signal. Server been stoped!"); 
	App->Mess->stop_signal=signal; 
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
    
    while(1) 
	if(stop_signal) return(stop_signal); else sleep(100);	
}

// Уровень отладки (App->d_level) может изменяться в пределах 0-8 включительно:
// 0 - не выводить никаких сообщений вообще 
// 8 - максимальный уровень отладки
// Уровень сообщения (level) характерезует его приоритетность и изменяется в пределах 0-7:
// 0 - отладочный уровень;
// 7 - уровень высочайшей степени аварии;

void TMessage::put( int level, char * fmt,  ... )
{
    char str[256];
    va_list argptr;

    va_start (argptr,fmt);
    vsprintf(str,fmt,argptr);
    va_end(argptr);
    if(level<0) level=0; if(level>7) level=7;
    if(level>=(8-App->d_level)) 
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
	if(App->log_dir&1) syslog(level_sys,str);
	if(App->log_dir&2) fprintf(stdout,"%s \n",str);
	if(App->log_dir&4) fprintf(stderr,"%s \n",str);
    }
}



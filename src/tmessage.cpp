#include <sys/types.h>

#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <iconv.h>
#include <langinfo.h>
#include <getopt.h>
#include <stdlib.h>
#include <locale.h>

#include "terror.h"
#include "tsys.h"
#include "tarhives.h"
#include "tmessage.h"

const char *TMessage::o_name = "TMessage";

TMessage::TMessage(  ) : IOCharSet("UTF8"), m_d_level(0), log_dir(2), head_buf(0)
{
    openlog("OpenScada",0,LOG_USER);
    setlocale(LC_ALL,"");
    charset(nl_langinfo(CODESET));
 
    m_res = TSYS::ResCreate( );
    mess_buf_len( 10 );
}


TMessage::~TMessage(  )
{
    TSYS::ResDelete( m_res );
    closelog();
}


// Уровень отладки (m_d_level) может изменяться в пределах 0-8 включительно:
// 0 - не выводить никаких сообщений вообще 
// 8 - максимальный уровень отладки
// Уровень сообщения (level) характерезует его приоритетность и изменяется в пределах 0-7:
// 0 - отладочный уровень;
// 7 - уровень высочайшей степени аварии;
void TMessage::put( string categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    put_s( categ, level, str );
}

void TMessage::put_s( string categ, int level, string mess )
{
    if(level<0) level=0; if(level>7) level=7;
    if(level>=(8-m_d_level)) 
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
	string s_mess = categ + "| " + mess;
	if(log_dir&1) syslog(level_sys,s_mess.c_str());
	if(log_dir&2) fprintf(stdout,"%s \n",s_mess.c_str());
	if(log_dir&4) fprintf(stderr,"%s \n",s_mess.c_str());
	
    	TSYS::WResRequest(m_res);	
	m_buf[head_buf].time  = time(NULL);
	m_buf[head_buf].categ = categ;
	m_buf[head_buf].level = level;
	m_buf[head_buf].mess  = mess;
	if( ++head_buf >= m_buf.size() ) head_buf = 0;
    	TSYS::WResRelease(m_res);	
    }
}

void TMessage::get( time_t b_tm, time_t e_tm, vector<SBufRec> & recs, string category, char level )
{
    recs.clear();
    
    TSYS::RResRequest(m_res);	
    int i_buf = head_buf;
    while(true)
    {
	if( m_buf[i_buf].time >= b_tm && m_buf[i_buf].time != 0 && m_buf[i_buf].time < e_tm &&
		( !category.size() || category == m_buf[i_buf].categ ) && m_buf[i_buf].level >= level )
	    recs.push_back(m_buf[i_buf]);
	if( ++i_buf >= m_buf.size() ) i_buf = 0;
    	if(i_buf == head_buf) break;	    
    }
    TSYS::RResRelease(m_res);	
}

int TMessage::SconvIn( string fromCH, string & buf)
{
    return( Sconv(fromCH, IOCharSet, buf) );
}    
int TMessage::SconvOut( string toCH, string & buf)
{
    return( Sconv( IOCharSet, toCH , buf) );
}

int TMessage::Sconv( string fromCH, string toCH, string & buf)
{
    //Make convert to blocks 100 bytes !!!    
    char   *ibuf, outbuf[100], *obuf;
    size_t ilen, olen;
    iconv_t hd;
    
    hd = iconv_open(toCH.c_str(), fromCH.c_str());
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

//void TMessage::opt_descr( FILE * stream )
string TMessage::opt_descr( )
{
    string rez;

    rez = rez +
    	"============================ Message options ==============================\n"+
	"-d, --debug=<level>    Set <level> debug (0-8);\n"+
    	"    --log=<direct>     Set direction a log and other info;\n"+
    	"                         <direct> & 1 - syslogd;\n"+
    	"                         <direct> & 2 - stdout;\n"+
    	"                         <direct> & 4 - stderr;\n"+
    	"    --IOCharset=<name> Set io charset;\n"+
    	"------------------ Section fields of config file --------------------\n"+
    	"debug      = <level>     set <level> debug (0-8);\n"+
    	"target_log = <direction> set direction a log and other info;\n"+
    	"                           <direct> & 1 - syslogd;\n"+
    	"                           <direct> & 2 - stdout;\n"+
    	"                           <direct> & 4 - stderr;\n"+
    	"io_chrset  = <charset>   set io charset;\n"+
    	"mess_buf   = <len>       set messages buffer len;\n\n";
	
    return(rez);
}

void TMessage::CheckCommandLine( )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"(%s)Read commandline options!",o_name);
#endif

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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'd': i = atoi(optarg); if(i>=0&&i<=8) d_level(i); break;
	    case 'l': log_direct(atoi(optarg)); break;
	    case 'c': charset(optarg); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"(%s)Read commandline options ok!",o_name);
#endif
}

void TMessage::UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"(%s)Read config options!",o_name);
#endif

    string opt;

    try
    {
	int i = atoi(SYS->XMLCfgNode()->get_child("debug")->get_text().c_str());
	if( i >= 0 && i <= 8 ) d_level(i);
    }catch(...) {  }
    //}catch( TError err ) { put("SYS",MESS_DEBUG,"MESS:%s",err.what().c_str()); }
    try{ log_direct(atoi(SYS->XMLCfgNode()->get_child("target_log")->get_text().c_str())); }
    catch(...) { }
    try{ charset(SYS->XMLCfgNode()->get_child("io_charset")->get_text()); }
    catch(...) { }    
    try{ mess_buf_len( atoi( SYS->XMLCfgNode()->get_child("mess_buf")->get_text().c_str() ) ); }
    catch(...) { }    
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"(%s)Read config options ok!",o_name);
#endif
}

void TMessage::mess_buf_len(int len)
{
    TSYS::WResRequest(m_res);	
    while( m_buf.size() > len )
    {
	m_buf.erase( m_buf.begin() + head_buf );
	if( head_buf >= m_buf.size() ) head_buf = 0;
    }
    while( m_buf.size() < len )
	m_buf.insert( m_buf.begin() + head_buf, SBufRec() );
    TSYS::WResRelease(m_res);	
}


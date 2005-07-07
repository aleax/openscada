/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
#include <libintl.h>

#include "../config.h"
#include "terror.h"
#include "tsys.h"
#include "tarchives.h"
#include "tmessage.h"

const char *TMess::o_name = "TMess";

TMess::TMess(  ) : IOCharSet("UTF8"), m_d_level(7), log_dir(3), head_buf(0)
{
    openlog(PACKAGE,0,LOG_USER);
    setlocale(LC_ALL,"");
    setlocale(LC_NUMERIC,"C");	//For true float type conversion
    IOCharSet = nl_langinfo(CODESET);

    bindtextdomain(PACKAGE,LOCALEDIR);
    textdomain(PACKAGE);	    
 
    m_res = ResAlloc::resCreate( );
    mess_buf_len( 10 );
}


TMess::~TMess(  )
{
    ResAlloc::resDelete( m_res );
    closelog();
}


// Debug level (m_d_level) may changed пределах 0-8 включительно:
// 0 - не выводить никаких сообщений вообще 
// 8 - максимальный уровень отладки
// Уровень сообщения (level) характерезует его приоритетность и изменяется в пределах 0-7:
// 0 - отладочный уровень;
// 7 - уровень высочайшей степени аварии;
void TMess::put( const string &categ, Type level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    put_s( categ, level, str );
}

void TMess::put_s( const string &categ, Type level, const string &mess )
{
    level = (level<Debug)?Debug:(level>Emerg)?Emerg:level;
    if(level>=(8-m_d_level)) 
    {
	int level_sys = LOG_DEBUG;
	if(level == Debug )    	level_sys = LOG_DEBUG;
	else if(level == Info )	level_sys = LOG_INFO;
	else if(level == Notice )	level_sys = LOG_NOTICE;
	else if(level == Warning )	level_sys = LOG_WARNING;
	else if(level == Error )	level_sys = LOG_ERR;
	else if(level == Crit )	level_sys = LOG_CRIT;
	else if(level == Allert )	level_sys = LOG_ALERT;
	else if(level == Emerg )	level_sys = LOG_EMERG;
	string s_mess = categ + "| " + mess;
	if(log_dir&1) syslog(level_sys,s_mess.c_str());
	if(log_dir&2) fprintf(stdout,"%s \n",s_mess.c_str());
	if(log_dir&4) fprintf(stderr,"%s \n",s_mess.c_str());
    }
    //Put to message buffer
    ResAlloc res(m_res,true);
    m_buf[head_buf].time  = time(NULL);
    m_buf[head_buf].categ = categ;
    m_buf[head_buf].level = level;
    m_buf[head_buf].mess  = mess;
    if( ++head_buf >= m_buf.size() ) head_buf = 0;    
}

void TMess::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category, Type level )
{
    recs.clear();
    
    ResAlloc res(m_res,false);
    int i_buf = head_buf;
    while(true)
    {
	if( m_buf[i_buf].time >= b_tm && m_buf[i_buf].time != 0 && m_buf[i_buf].time < e_tm &&
		( !category.size() || category == m_buf[i_buf].categ ) && m_buf[i_buf].level >= level )
	    recs.push_back(m_buf[i_buf]);
	if( ++i_buf >= m_buf.size() ) i_buf = 0;
    	if(i_buf == head_buf) break;	    
    }
}

string TMess::lang( )
{
    if( getenv("LANGUAGE") )		return getenv("LANGUAGE");
    else if( getenv("LC_MESSAGES") )	return getenv("LC_MESSAGES");
    else if( getenv("LANG") )		return getenv("LANG");
    else return("C");
}

void TMess::lang( const string &lng )
{
    if( getenv("LANGUAGE") ) setenv("LANGUAGE", lng.c_str(), 1);
    else setenv("LC_MESSAGES", lng.c_str(), 1);
    setlocale(LC_ALL,"");

    IOCharSet = nl_langinfo(CODESET);
}

string TMess::Sconv( const string &fromCH, const string &toCH, const string &mess)
{
    //Make convert to blocks 100 bytes !!!    
    string buf = ""; 
    char   *ibuf, outbuf[100], *obuf;
    size_t ilen, olen;
    iconv_t hd;
    
    hd = iconv_open(toCH.c_str(), fromCH.c_str());
    if( hd == (iconv_t)(-1) ) return("Error iconv");
    
    ibuf = (char *)mess.c_str();
    ilen = mess.size();
    
    while(ilen)
    {
	obuf = outbuf;
	olen = sizeof(outbuf)-1;
	iconv(hd,&ibuf,&ilen,&obuf,&olen);
	buf.append(outbuf,sizeof(outbuf)-1-olen);
    }
    iconv_close(hd);
    
    return(buf);
}

char *TMess::I18N( char *mess, char *d_name )
{
    return( dgettext(d_name, mess) );
}

void TMess::load()
{
    //======================= Load params from command line =========================
    int i,next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"debug"    ,1,NULL,'d'},
	{"log"      ,1,NULL,'l'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'd': i = atoi(optarg); if(i>=0&&i<=8) d_level(i); break;
	    case 'l': log_direct(atoi(optarg)); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
    //======================= Load params config file =========================
    string opt;
    try
    {
	int i = atoi(SYS->cfgNode()->childGet("id","debug")->text().c_str());
	if( i >= 0 && i <= 8 ) d_level(i);
    }catch(...) {  }
    try{ log_direct(atoi(SYS->cfgNode()->childGet("id","target_log")->text().c_str())); }
    catch(...) { }
    try{ mess_buf_len( atoi( SYS->cfgNode()->childGet("id","mess_buf")->text().c_str() ) ); }
    catch(...) { }    
}

void TMess::mess_buf_len(int len)
{
    ResAlloc res(m_res,true);
    while( m_buf.size() > len )
    {
	m_buf.erase( m_buf.begin() + head_buf );
	if( head_buf >= m_buf.size() ) head_buf = 0;
    }
    while( m_buf.size() < len )
	m_buf.insert( m_buf.begin() + head_buf, TMess::SRec() );
}


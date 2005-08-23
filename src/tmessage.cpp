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
#include "resalloc.h"
#include "tarchives.h"
#include "tmessage.h"

const char *TMess::o_name = "TMess";

TMess::TMess(  ) : IOCharSet("UTF8"), m_mess_level(0), log_dir(3), head_buf(0)
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

void TMess::put( const char *categ, Type level, const char *fmt,  ... )
{
    char mess[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(mess,sizeof(mess),fmt,argptr);
    va_end(argptr);
    
    level = (level<Debug)?Debug:(level>Emerg)?Emerg:level;
    if(level<messLevel()) return; 
    
    int level_sys;
    switch(level)
    {
	case Debug:	level_sys = LOG_DEBUG;	break;
	case Info:	level_sys = LOG_INFO;	break;
	case Notice:	level_sys = LOG_NOTICE;	break;
	case Warning:	level_sys = LOG_WARNING;break;
	case Error:	level_sys = LOG_ERR;	break;
	case Crit:	level_sys = LOG_CRIT;	break;
	case Allert:	level_sys = LOG_ALERT;	break;
	case Emerg:	level_sys = LOG_EMERG;	break;
	default: 	level_sys = LOG_DEBUG;
    }
    string s_mess = TSYS::int2str(level) + "|" + categ + " | " + mess;
    if(log_dir&1) syslog(level_sys,s_mess.c_str());
    if(log_dir&2) fprintf(stdout,"%s \n",s_mess.c_str());
    if(log_dir&4) fprintf(stderr,"%s \n",s_mess.c_str());
    
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

const char *TMess::I18N( const char *mess, const char *d_name )
{
    return( dgettext(d_name, mess) );
}

bool TMess::chkPattern( const string &val, const string &patt )
{
    bool mult_s = false;
    int v_cnt = 0, p_cnt = 0;
    int v_bck = -1, p_bck = -1;
    

    while(true)
    {
	if( p_cnt >= patt.size() )	return true;
	if( patt[p_cnt] == '?' ) 	{ v_cnt++; p_cnt++; mult_s = false; continue; }
	if( patt[p_cnt] == '*' )	{ p_cnt++; mult_s = true; v_bck = -1; continue; }
	if( patt[p_cnt] == '\\' ) 	p_cnt++;
	if( v_cnt >= val.size() )       break;
	if( patt[p_cnt] == val[v_cnt] )	
	{ 
	    if(mult_s && v_bck < 0 ) 	{ v_bck = v_cnt+1; p_bck = p_cnt; }
	    v_cnt++; p_cnt++;
	}
	else
	{
	    if( mult_s ) 
	    {
		if( v_bck >= 0 ) { v_cnt = v_bck; p_cnt = p_bck; v_bck = -1; }
		else v_cnt++;
	    }
	    else break;
	}
    }
    return false;
}

void TMess::load()
{
    //======================= Load params from command line =========================
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"mess_lev" ,1,NULL,'d'},
	{"log"      ,1,NULL,'l'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': return;
	    case 'd': i = atoi(optarg); if(i>=0&&i<=7) messLevel(i); break;
	    case 'l': log_direct(atoi(optarg)); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
    //======================= Load params config file =========================
    string opt;
    try
    {
	int i = atoi(TCntrNode::ctrId(&SYS->cfgRoot(),SYS->nodePath())->childGet("id","mess_lev")->text().c_str());
	if( i >= 0 && i <= 7 ) messLevel(i);
    }catch(...) {  }
    try{ log_direct(atoi(TCntrNode::ctrId(&SYS->cfgRoot(),SYS->nodePath())->childGet("id","target_log")->text().c_str())); }
    catch(...) { }
    try{ mess_buf_len( atoi(TCntrNode::ctrId(&SYS->cfgRoot(),SYS->nodePath())->childGet("id","mess_buf")->text().c_str() ) ); }
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


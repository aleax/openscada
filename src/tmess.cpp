
//OpenSCADA system file: tmess.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
#include <errno.h>

#include "../config.h"
#include "tsys.h"
#include "resalloc.h"
#include "tmess.h"

//*************************************************
//* TMess                                         *
//*************************************************
TMess::TMess(  ) : IOCharSet("UTF-8"), m_mess_level(0), log_dir(0x2)
{
    openlog(PACKAGE,0,LOG_USER);
    setlocale(LC_ALL,"");
    setlocale(LC_NUMERIC,"C");	//For true float type conversion
    IOCharSet = nl_langinfo(CODESET);

    bindtextdomain(PACKAGE,LOCALEDIR);
    textdomain(PACKAGE);
}

TMess::~TMess(  )
{
    closelog();
}

void TMess::setMessLevel( int level )  
{ 
    m_mess_level = level;
    SYS->modif();
}

void TMess::setLogDirect( int dir )
{ 
    log_dir = dir;
    SYS->modif();
}

void TMess::put( const char *categ, Type level, const char *fmt,  ... )
{
    char mess[STR_BUF_LEN];
    va_list argptr;

    va_start(argptr,fmt);
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
	case Alert:	level_sys = LOG_ALERT;	break;
	case Emerg:	level_sys = LOG_EMERG;	break;
	default: 	level_sys = LOG_DEBUG;
    }
    string s_mess = TSYS::int2str(level) + "|" + categ + " | " + mess;
    if(log_dir&1) syslog(level_sys,TSYS::strEncode(s_mess,TSYS::FormatPrint).c_str());
    if(log_dir&2) fprintf(stdout,"%s \n",s_mess.c_str());
    if(log_dir&4) fprintf(stderr,"%s \n",s_mess.c_str());
    if((log_dir&8) && SYS->present("Archive") )	SYS->archive().at().messPut(time(NULL),categ,level,mess);
}

void TMess::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category, Type level )
{
    if(log_dir&8) SYS->archive().at().messGet(b_tm,e_tm,recs,category,level);
}

string TMess::lang( )
{
    if( getenv("LANGUAGE") )		return getenv("LANGUAGE");    
    else if( getenv("LC_MESSAGES") )	return getenv("LC_MESSAGES");
    else if( getenv("LANG") )		return getenv("LANG");
    else return("C");
}

void TMess::setLang( const string &lng )
{
    if( getenv("LANGUAGE") ) setenv("LANGUAGE", lng.c_str(), 1);
    else setenv("LC_MESSAGES", lng.c_str(), 1);
    setlocale(LC_ALL,"");

    IOCharSet = nl_langinfo(CODESET);
    setlocale(LC_NUMERIC,"C");
}

string TMess::codeConv( const string &fromCH, const string &toCH, const string &mess)
{
    //- Make convert to blocks 100 bytes !!! -
    string buf = ""; 
    char   *ibuf, outbuf[100], *obuf;
    size_t ilen, olen;
    iconv_t hd;
    
    if( fromCH == toCH ) return mess;
    
    hd = iconv_open(toCH.c_str(), fromCH.c_str());
    if( hd == (iconv_t)(-1) )
    {
	mess_crit("IConv",_("Error iconv open: %s"),strerror(errno));
    	return mess;
    }
        
    ibuf = (char *)mess.c_str();
    ilen = mess.size();
    
    while(ilen)
    {
	obuf = outbuf;
	olen = sizeof(outbuf)-1;
	size_t rez = iconv(hd,&ibuf,&ilen,&obuf,&olen);
	if( rez == (size_t)(-1) && errno != E2BIG )
	{
	    mess_crit("IConv",_("Error input sequence convert: %s"),strerror(errno));
	    mess_debug("IConv",_("Error converting from %s to %s for message: %s"),fromCH.c_str(),toCH.c_str(),mess.c_str());
	    buf = mess;
	    break;
	}
	if( obuf > outbuf )
	    buf.append(outbuf,obuf-outbuf);
    }
    iconv_close(hd);
    
    return buf;
}

const char *TMess::I18N( const char *mess, const char *d_name )
{
    return dgettext(d_name, mess);
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
    //- Load params from command line -
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"MessLev"  ,1,NULL,'d'},
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
	    case 'd': i = atoi(optarg); if(i>=0&&i<=7) setMessLevel(i); break;
	    case 'l': setLogDirect(atoi(optarg)); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
    //- Load params config file -
    i = atoi(TBDS::genDBGet(SYS->nodePath()+"MessLev",TSYS::int2str(messLevel()),"root",SYS->sysOptCfg()).c_str());
    if( i >= 0 && i <= 7 ) setMessLevel(i);
    setLogDirect(atoi(TBDS::genDBGet(SYS->nodePath()+"LogTarget",TSYS::int2str(logDirect()),"root",SYS->sysOptCfg()).c_str()));
}

void TMess::save()
{
    TBDS::genDBSet(SYS->nodePath()+"MessLev",TSYS::int2str(messLevel()));
    TBDS::genDBSet(SYS->nodePath()+"LogTarget",TSYS::int2str(logDirect()));
}

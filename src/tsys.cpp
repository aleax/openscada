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
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>

#include "../config.h"
#include "terror.h"
#include "tmessage.h"
#include "tkernel.h"
#include "tsys.h"

const char *TSYS::o_name = "TSYS";
const char *TSYS::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_base'>"
    "  <fld id='stat' acs='0444' tp='str'/>"
    "  <fld id='prog' acs='0444' tp='str'/>"
    "  <fld id='ver' acs='0444' tp='str'/>"
    "  <fld id='host' acs='0444' tp='str'/>"
    "  <fld id='user' acs='0444' tp='str'/>"
    "  <fld id='sys' acs='0444' tp='str'/>"
    " </area>"
    " <area id='a_kern'>"
    "  <list id='k_br' acs='0774' s_com='add,del' tp='br' mode='att'/>"
    " </area>"
    " <area id='a_gen' acs='0440'>"
    "  <fld id='config' acs='0660' com='1' tp='str' dest='file'/>"
    "  <fld id='cr_file_perm' acs='0660' cfg='1' tp='oct' len='3'/>"
    "  <fld id='cr_dir_perm' acs='0660' cfg='1' tp='oct' len='3'/>"
    "  <fld id='in_charset' acs='0440' tp='str'/>"
    "  <fld id='lang' acs='0660' tp='str'/>"
    "  <fld id='debug' acs='0660' com='1' cfg='1' tp='dec' min='0' max='8'/>"
    "  <fld id='m_buf_l' acs='0660' com='1' cfg='1' tp='dec' min='10'/>"
    "  <fld id='log_sysl' acs='0660' com='1' cfg='1' tp='bool'/>"
    "  <fld id='log_stdo' acs='0660' com='1' cfg='1' tp='bool'/>"
    "  <fld id='log_stde' acs='0660' com='1' cfg='1' tp='bool'/>"
    "  <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    "  <comm id='upd_opt'/>"
    "  <comm id='quit'/>"
    "  <table id='mess' tp='flow' acs='0440'>"
    "   <comm id='view'>"
    "    <fld id='beg' tp='time'/>"
    "    <fld id='end' tp='time'/>"
    "    <fld id='cat' tp='str'/>"
    "    <fld id='lvl' tp='dec' min='0' max='7'/>"
    "   </comm>"
    "   <list id='0' tp='time'/>"
    "   <list id='1' tp='str'/>"
    "   <list id='2' tp='dec'/>"
    "   <list id='3' tp='str'/>"
    "  </table>"
    " </area>"
    "</oscada_cntr>";


TSYS::TSYS( int argi, char ** argb, char **env ) : Conf_File("./oscada.xml"), m_station("default"), stat_n(NULL), 
    User(getenv("USER")),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0), 
    m_cr_f_perm(0644), m_cr_d_perm(0755), m_kern(o_name)
{
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGCHLD,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
    
    CheckCommandLine();
    UpdateOpt();    
}

TSYS::~TSYS(  )
{
    vector<string> list;
    
    m_kern.lock();
    kern_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
    kern_del(list[i_ls]);		    
}


string TSYS::int2str( int val, char view )
{
    char buf[STR_BUF_LEN];
    if( view == C_INT_DEC )      snprintf(buf,sizeof(buf),"%d",val); 
    else if( view == C_INT_OCT ) snprintf(buf,sizeof(buf),"%o",val); 
    else if( view == C_INT_HEX ) snprintf(buf,sizeof(buf),"%x",val);

    return(buf);
}

string TSYS::real2str( double val )
{
    //ostringstream oStr;
    //oStr<<id;
    //el->set_attr("id",oStr.str(),true);
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),"%g",val); 

    return(buf);
}

XMLNode *TSYS::XMLCfgNode() 
{ 
    if(!stat_n) throw TError("%s: XML config error or no avoid!",o_name);
    return(stat_n); 
}

string TSYS::opt_descr( )
{
    char s_buf[STR_BUF_LEN];
    utsname buf;

    uname(&buf);
    snprintf(s_buf,sizeof(s_buf),Mess->I18N(
	"***************************************************************************\n"
	"********** %s v%s (%s-%s). *********\n"
	"***************************************************************************\n\n"
	"===========================================================================\n"
	"======================== The general station options ======================\n"
	"===========================================================================\n"
	"-h, --help             Info message about The station options;\n"
	"    --Config=<path>    Config file path;\n"
	"    --Station=<name>   Station name;\n"
	"-d, --debug=<level>    Set <level> debug (0-8);\n"
    	"    --log=<direct>     Set direction a log and other info;\n"
    	"                         <direct> & 1 - syslogd;\n"
    	"                         <direct> & 2 - stdout;\n"
    	"                         <direct> & 4 - stderr;\n"
    	"----------- The config file station <%s> parameters -----------\n"
    	"debug      <level>     set <level> debug (0-8);\n"
    	"target_log <direction> set direction a log and other info;\n"
    	"                           <direct> & 1 - syslogd;\n"
    	"                           <direct> & 2 - stdout;\n"
    	"                           <direct> & 4 - stderr;\n"
    	"mess_buf   <len>       set messages buffer len;\n"
	"cr_file_perm <perm>    Permision of created files (default 0644);\n"
	"cr_dir_perm  <perm>    Permision of created directories (default 0755);\n\n"),
	PACKAGE_NAME,VERSION,buf.sysname,buf.release,Station().c_str());
	
    return(s_buf);
}

void TSYS::CheckCommandLine( )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"(%s)Read commandline options!",o_name);
#endif	

    int next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"Config"   ,1,NULL,'f'},
	{"Station"  ,1,NULL,'s'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(argc,(char * const *)argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'f': Conf_File = optarg; break;
	    case 's': m_station = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"(%s)Read commandline options ok!",o_name);
#endif	
}

void TSYS::UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"(%s)Read config options!",o_name);
#endif	

    stat_n = NULL;
    int hd = open(Conf_File.c_str(),O_RDONLY);
    if(hd > 0)
    {
	int cf_sz = lseek(hd,0,SEEK_END);
	lseek(hd,0,SEEK_SET);
	char *buf = (char *)malloc(cf_sz+1);
	read(hd,buf,cf_sz);
	buf[cf_sz] = 0;
	close(hd);
	string s_buf = buf;
	free(buf);
	try
	{
	    root_n.load_xml(s_buf);
	    if( root_n.get_name() == "OpenScada" )
	    {
	    	int i_n = 0;
		while( true )
	    	{
	    	    try
	    	    {
	    		XMLNode *t_n = root_n.get_child("station",i_n++); 
	    		if( t_n->get_attr("id") == m_station ) 
	    		{
	    		    stat_n = t_n;
	    		    break;		
	    		}
	    	    }
	    	    catch(...){ break; }
		}
	    }
	}
	catch( TError err ) { Mess->put("SYS",MESS_WARNING,"(%s)%s",o_name, err.what().c_str() ); }
    }    
    
    //All system parameters
    try{ m_cr_f_perm = strtol( XMLCfgNode()->get_child("id","cr_file_perm")->get_text().c_str(),NULL,8); }
    catch(...) {  }
    try{ m_cr_d_perm = strtol( XMLCfgNode()->get_child("id","cr_dir_perm")->get_text().c_str(),NULL,8); }
    catch(...) {  }

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"(%s)Read config options ok!",o_name);
#endif	
}

void TSYS::SetTaskTitle(const char *fmt, ...)
{
    /*
    va_list argptr;
    int i,envpsize=0;
    char *LastArgv, buf[256], *p;      //!!!!

    for (i = 0; envp[i] != NULL; i++) envpsize += strlen(envp[i]) + 1;
    while (i > 0 && (envp[i - 1] < argv[0] || envp[i - 1] > (argv[argc - 1] +
	strlen(argv[argc - 1]) + 1 + envpsize))) i--;
    if (i > 0) LastArgv = (char *)envp[i - 1] + strlen(envp[i - 1]);
    else       LastArgv = (char *)argv[argc - 1] + strlen(argv[argc - 1]);

    va_start(argptr, fmt);
    vsnprintf(buf,sizeof(buf), fmt, argptr);
    va_end(argptr);
    i = strlen(buf);
    if(i > LastArgv - argv[0] - 2)
    {
	i = LastArgv - argv[0] - 2;
	buf[i] = '\0';
    }
    strcpy((char *)argv[0], buf);
    p = (char *)argv[0]+i;
    while(p < LastArgv) *p++ = '\0';
    argv[1] = NULL;
    */
}
					
int TSYS::Start(  )
{
    int i_cnt = 0;    
    
    ScanCfgFile( true );	
    while(1)	
    {
	if(stop_signal) break;   
	if( ++i_cnt > 10*1000/STD_WAIT_DELAY )  //10 second
	{
	    i_cnt = 0;
    	    ScanCfgFile( );	
	}
       	usleep( STD_WAIT_DELAY*1000 ); 
    }

    return(stop_signal);       
}

void TSYS::sighandler( int signal )
{
    if(signal == SIGINT) 
    { 
//	Mess->put(3,"Have get a Interrupt signal. No stop server!");
	SYS->stop_signal=signal; 
    }
    else if(signal == SIGTERM) 
    { 
	Mess->put("SYS",MESS_WARNING,"(%s)Have get a Terminate signal. Server been stoped!",o_name); 
	SYS->stop_signal=signal; 
    }
    else if(signal == SIGCHLD)
    {
	int status;
	pid_t pid = wait(&status);
	if(!WIFEXITED(status))
	    Mess->put("SYS",MESS_INFO,"(%s)Free child process %d!",o_name,pid);
    }	
    else if(signal == SIGPIPE)
	Mess->put("SYS",MESS_WARNING,Mess->I18N("(%s)Broken PIPE signal!"),o_name);
}

void TSYS::kern_add( const string &name )
{
    TKernel *kern = new TKernel( name );
    try{ m_kern.obj_add( kern, &kern->Name() ); }
    catch(TError err) { delete kern; }
}

void TSYS::kern_del( const string &name )
{ 
    delete (TKernel *)m_kern.obj_del( name ); 
}	    

void TSYS::ScanCfgFile( bool first )
{
    static string cfg_fl;
    static struct stat f_stat;
    
    struct stat f_stat_t;
    bool   up = false;

    if(cfg_fl == CfgFile())
    {
	stat(cfg_fl.c_str(),&f_stat_t);
	if( f_stat.st_mtime != f_stat_t.st_mtime ) up = true;
    }
    else up = true;
    cfg_fl = CfgFile();
    stat(cfg_fl.c_str(),&f_stat);
    if(up == true && !first )
    {
	UpdateOpt();
	Mess->UpdateOpt();
	
	vector<string> list;
	kern_list( list );
	for( unsigned i_kern = 0; i_kern < list.size(); i_kern++)
    	    kern_at(list[i_kern]).at().UpdateOpt();	    
    }    
}

string TSYS::FixFName( const string &fname ) const
{
    string tmp;
    char   buf[1024];   //!!!!
    
    for( string::const_iterator it = fname.begin(); it != fname.end(); it++ )
    {	
	if( *(it) == '.' && *(it+1) == '.' && *(it+2) == '/')
	{
	    string cur = getcwd(buf,sizeof(buf));
	    if(chdir("..") != 0) return("");
	    tmp += getcwd(buf,sizeof(buf));
	    chdir(cur.c_str());       	
	    it++;
	}
	else if( *(it) == '.' && *(it+1) == '/' ) 
	    tmp += getcwd(buf,sizeof(buf));
	else tmp += *(it);
    }
    return tmp;
}

bool TSYS::event_wait( bool &m_mess_r_stat, bool exempl, const string &loc, time_t tm )
{
    time_t t_tm, s_tm;
    
    t_tm = s_tm = time(NULL);
    while( m_mess_r_stat != exempl )
    {
	time_t c_tm = time(NULL);
	//Check timeout
	if( tm && ( c_tm > s_tm+tm) )
	{
	    Mess->put("SYS",MESS_CRIT,"(%s)Go timeout %s!!!!",o_name,loc.c_str());
    	    return(true);
	}
	//Make messages
	if( c_tm > t_tm+1 )  //1sec 
	{
	    t_tm = c_tm;
	    Mess->put("SYS",MESS_INFO,"(%s) %s",o_name,loc.c_str());
	}
	usleep(STD_WAIT_DELAY*1000);
    }
    return(false);
}

//================== Controll functions ========================

void TSYS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    char buf[STR_BUF_LEN];
    
    inf->load_xml( i_cntr );
    snprintf(buf,sizeof(buf),Mess->I18N("%s station: %s"),PACKAGE_NAME,m_station.c_str());
    inf->set_text(buf);
    //a_base
    XMLNode *c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Base information"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Station"));
    c_nd->get_child(1)->set_attr(dscr,Mess->I18N("Programm"));
    c_nd->get_child(2)->set_attr(dscr,Mess->I18N("Version"));
    c_nd->get_child(3)->set_attr(dscr,Mess->I18N("Host name"));
    c_nd->get_child(4)->set_attr(dscr,Mess->I18N("Operated user"));
    c_nd->get_child(5)->set_attr(dscr,Mess->I18N("Operation system"));    
    //a_gen
    c_nd = inf->get_child(2);
    c_nd->set_attr(dscr,Mess->I18N("Station control"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Config file"));
    c_nd->get_child(1)->set_attr(dscr,Mess->I18N("Make files permissions(default 0644)"));
    c_nd->get_child(2)->set_attr(dscr,Mess->I18N("Make directories permissions(default 0755)"));
    c_nd->get_child(3)->set_attr(dscr,Mess->I18N("Internal charset"));
    c_nd->get_child(4)->set_attr(dscr,Mess->I18N("Language"));
    c_nd->get_child(5)->set_attr(dscr,Mess->I18N("Debug level"));
    c_nd->get_child(6)->set_attr(dscr,Mess->I18N("The ring message buffer elements number"));
    c_nd->get_child(7)->set_attr(dscr,Mess->I18N("Direct messages to syslog"));
    c_nd->get_child(8)->set_attr(dscr,Mess->I18N("Direct messages to stdout"));    
    c_nd->get_child(9)->set_attr(dscr,Mess->I18N("Direct messages to stderr"));    
    c_nd->get_child(10)->set_attr(dscr,Mess->I18N("Options help"));        
    c_nd->get_child(11)->set_attr(dscr,Mess->I18N("Update options(from config)"));
    c_nd->get_child(12)->set_attr(dscr,Mess->I18N("Quit"));    
    //mess
    c_nd = c_nd->get_child(13);
    c_nd->set_attr(dscr,Mess->I18N("Messages"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("View"));
    c_nd->get_child(1)->set_attr(dscr,Mess->I18N("Time"));
    c_nd->get_child(2)->set_attr(dscr,Mess->I18N("Category"));
    c_nd->get_child(3)->set_attr(dscr,Mess->I18N("Level"));
    c_nd->get_child(4)->set_attr(dscr,Mess->I18N("Message"));
    //view
    c_nd = c_nd->get_child(0);
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Begin"));
    c_nd->get_child(1)->set_attr(dscr,Mess->I18N("End"));
    c_nd->get_child(2)->set_attr(dscr,Mess->I18N("Category"));
    c_nd->get_child(3)->set_attr(dscr,Mess->I18N("Level"));
    //a_kern
    c_nd = inf->get_child(1);
    c_nd->set_attr(dscr,Mess->I18N("Kernels"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Avoid kernels"));
}

void TSYS::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    utsname buf;
    uname(&buf);
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_base" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "host" )              ctr_opt_setS( opt, buf.nodename );
	else if( t_id == "sys" )          ctr_opt_setS( opt, string(buf.sysname)+"-"+buf.release );
	else if( t_id == "user" )         ctr_opt_setS( opt, User );
	else if( t_id == "prog" )         ctr_opt_setS( opt, PACKAGE_NAME );
	else if( t_id == "ver" )          ctr_opt_setS( opt, VERSION );
	else if( t_id == "stat" )         ctr_opt_setS( opt, m_station );
    }
    else if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "config" )            ctr_opt_setS( opt, Conf_File );
	else if( t_id == "cr_file_perm" ) ctr_opt_setI( opt, m_cr_f_perm );
	else if( t_id == "cr_dir_perm" )  ctr_opt_setI( opt, m_cr_d_perm );
	else if( t_id == "debug" )        ctr_opt_setI( opt, Mess->d_level() );
	else if( t_id == "lang" )         ctr_opt_setS( opt, Mess->lang() );
	else if( t_id == "in_charset" )   ctr_opt_setS( opt, Mess->charset() );
	else if( t_id == "m_buf_l" )      ctr_opt_setI( opt, Mess->mess_buf_len() );
	else if( t_id == "log_sysl" )     ctr_opt_setB( opt, (Mess->log_direct()&0x01)?true:false );
	else if( t_id == "log_stdo" )     ctr_opt_setB( opt, (Mess->log_direct()&0x02)?true:false );
	else if( t_id == "log_stde" )     ctr_opt_setB( opt, (Mess->log_direct()&0x04)?true:false );
	else if( t_id == "g_help" )       ctr_opt_setS( opt, opt_descr() );       
    }
    else if( t_id == "a_kern" )
    {
    	if( ctr_path_l(a_path,1) == "k_br" )
    	{
    	    vector<string> list;
    	    kern_list(list);
 	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 
       	}
    }
}

void TSYS::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "config" )       Conf_File = ctr_opt_getS( opt );
	else if( t_id == "cr_file_perm" ) m_cr_f_perm = ctr_opt_getI( opt );
	else if( t_id == "cr_dir_perm" )  m_cr_d_perm = ctr_opt_getI( opt );
	else if( t_id == "debug" )        Mess->d_level( ctr_opt_getI( opt ) );
	else if( t_id == "lang" )         Mess->lang(ctr_opt_getS( opt ) );
	else if( t_id == "m_buf_l" )      Mess->mess_buf_len( ctr_opt_getI( opt ) );
	else if( t_id == "log_sysl" )     
	{
	    if( ctr_opt_getB( opt ) ) Mess->log_direct( Mess->log_direct()|0x01 );
	    else                      Mess->log_direct( Mess->log_direct()&(~0x01) );
	}
	else if( t_id == "log_stdo" )     
	{
	    if( ctr_opt_getB( opt ) ) Mess->log_direct( Mess->log_direct()|0x02 );
	    else                      Mess->log_direct( Mess->log_direct()&(~0x02) );
	}
	else if( t_id == "log_stde" )     
	{
	    if( ctr_opt_getB( opt ) ) Mess->log_direct( Mess->log_direct()|0x04 );
	    else                      Mess->log_direct( Mess->log_direct()&(~0x04) );
	}
    }
    else if( t_id == "a_kern" && ctr_path_l(a_path,1) == "k_br" )
    {
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      kern_add(t_c->get_text());
		else if(t_c->get_attr("do") == "del") kern_del(t_c->get_text());
	    }
	}
    }
}

void TSYS::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);
    
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);    
    	if( t_id == "quit" )         stop_signal=1;
	else if( t_id == "upd_opt" ) 
	{
	    UpdateOpt();
	    Mess->UpdateOpt();
	}
	else if( t_id == "mess" && ctr_path_l(a_path,2) == "view" )     
	{
	    vector<SBufRec> rec;
	    Mess->get(  ctr_opt_getI(ctr_id(fld,"beg")),
			ctr_opt_getI(ctr_id(fld,"end")),
			rec,
			ctr_opt_getS(ctr_id(fld,"cat")),
			ctr_opt_getI(ctr_id(fld,"lvl")) );

	    XMLNode *n_tm   = ctr_id(rez,"0");
	    XMLNode *n_cat  = ctr_id(rez,"1");
	    XMLNode *n_lvl  = ctr_id(rez,"2");
	    XMLNode *n_mess = ctr_id(rez,"3");
	    for( int i_rec = 0; i_rec < rec.size(); i_rec++)
	    {
	       	ctr_opt_setI(n_tm,rec[i_rec].time,i_rec);
	       	ctr_opt_setS(n_cat,rec[i_rec].categ,i_rec);
	       	ctr_opt_setI(n_lvl,rec[i_rec].level,i_rec);
	       	ctr_opt_setS(n_mess,rec[i_rec].mess,i_rec);
	    }
	}
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
const char *ResAlloc::o_name = "ResAlloc";

vector<SSem> ResAlloc::sems;

ResAlloc::ResAlloc( unsigned id ) : m_id(id), m_wr(0)
{

}

ResAlloc::ResAlloc( unsigned id, bool write ) : m_id(id), m_wr(0) 
{
    request( write );
}

ResAlloc::~ResAlloc( )
{
    if( m_wr & 0x01 ) release();        
}

void ResAlloc::request( bool write, long tm )
{
    if( m_wr & 0x01 ) throw TError("%s: a resource alloced!", o_name);    
    m_wr |= 0x01;    
    
    if( write ) 
    {
	m_wr |= 0x02;
	WResRequest(m_id, tm);
    }
    else
    {
	m_wr &= ~0x02;
	RResRequest(m_id, tm);	
    }
}

    
void ResAlloc::release()
{
    if( !(m_wr&0x01) ) throw TError("%s: a resource didn't alloc!", o_name);    
    m_wr &= ~0x01;    
    if( m_wr&0x02 ) WResRelease(m_id);
    else            RResRelease(m_id);	
}

unsigned ResAlloc::ResCreate( unsigned val )
{
    unsigned i_sem;
    
    for(i_sem = 0; i_sem < sems.size(); i_sem++)
	if( !sems[i_sem].use ) break;
    if( i_sem == sems.size() ) sems.push_back( SSem() );
    if( sem_init(&sems[i_sem].sem,0,val) != 0 )
	throw TError("%s: error open semaphor!", o_name);
    sems[i_sem].use = true;   
    sems[i_sem].del = false;   
    sems[i_sem].rd_c = 0;   

    return(i_sem);
}

void ResAlloc::ResDelete( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("%s: error delete semaphor %d!", o_name, res);
    
    sems[res].del = true;
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
    sem_destroy( &sems[res].sem );
    sems[res].use = false;   
}

void ResAlloc::WResRequest( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <w> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
}

void ResAlloc::WResRelease( unsigned res )
{
    if(res >= sems.size() || !sems[res].use )
	throw TError("%s: error <w> release semaphor %d!", o_name, res);
    sem_post( &sems[res].sem );
}

void ResAlloc::RResRequest( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <r> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    sems[res].rd_c++;   
    sem_post( &sems[res].sem );
}

void ResAlloc::RResRelease( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("%s: error <r> release semaphor %d!", o_name, res);
    if( sems[res].rd_c > 0 ) sems[res].rd_c--;   
}


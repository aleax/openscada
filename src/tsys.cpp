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
#include <errno.h>
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

//m_station("work_station")
TSYS::TSYS( int argi, char ** argb, char **env ) : m_confFile("/etc/oscada.xml"), stat_n(NULL), 
    m_user(getenv("USER")),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0), 
    m_cr_f_perm(0644), m_cr_d_perm(0755), m_beg(time(NULL)), m_end(time(NULL))
{
    m_kern = grpAdd();
    nodeEn();
    //Init system clock
    m_sysclc = shrtCnt( );
    usleep(100000);
    m_sysclc = 10*(shrtCnt( )-m_sysclc);
    
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGCHLD,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
    
    checkCommandLine();
    updateOpt();
}

TSYS::~TSYS(  )
{

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
    //el->attr("id",oStr.str(),true);
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),"%g",val); 

    return(buf);
}

XMLNode *TSYS::cfgNode() 
{ 
    if(!stat_n) throw TError(Mess->I18N("Config <%s> error or no avoid!"),m_confFile.c_str());
    return(stat_n); 
}

string TSYS::optDescr( )
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
	"workdir    <path>	set the station work directory;\n"
    	"debug      <level>     set <level> debug (0-8);\n"
    	"target_log <direction> set direction a log and other info;\n"
    	"                           <direct> & 1 - syslogd;\n"
    	"                           <direct> & 2 - stdout;\n"
    	"                           <direct> & 4 - stderr;\n"
    	"mess_buf   <len>       set messages buffer len;\n\n"),
	PACKAGE_NAME,VERSION,buf.sysname,buf.release,station().c_str());
	
    return(s_buf);
}

void TSYS::checkCommandLine( )
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'f': m_confFile = optarg; break;
	    case 's': m_station = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"(%s)Read commandline options ok!",o_name);
#endif	
}

void TSYS::updateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"(%s)Read config options!",o_name);
#endif	

    stat_n = NULL;
    int hd = open(m_confFile.c_str(),O_RDONLY);
    if( hd < 0 ) 
	Mess->put("SYS",MESS_ERR,Mess->I18N("Config <%s> error: %s"),m_confFile.c_str(),strerror(errno));
    else
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
	    root_n.load(s_buf);
	    if( root_n.name() == "OpenScada" )
	    {
		for( int i_st = root_n.childSize()-1; i_st >= 0; i_st--)
	            if( root_n.childGet(i_st)->name() == "station" )
		    {
			stat_n = root_n.childGet(i_st);
			if( stat_n->attr("id") == m_station ) break;							
		    }
		if(stat_n && stat_n->attr("id") != m_station )
		{ 
		    Mess->put("SYS",MESS_CRIT,Mess->I18N("Station <%s> config no avoid. Use <%s> station config!"),
			m_station.c_str(), stat_n->attr("id").c_str() );
		    m_station = stat_n->attr("id");
		}
	    }
	}
	catch( TError err ) { Mess->put("SYS",MESS_ERR,"(%s)%s",o_name, err.what().c_str() ); }
    }
    
    //All system parameters
    try{ chdir( cfgNode()->childGet("id","workdir")->text().c_str() ); }
    catch(...) {  }

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"(%s)Read config options ok!",o_name);
#endif	
}

/*
void TSYS::SetTaskTitle(const char *fmt, ...)
{    
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
}
*/
					
int TSYS::start(  )
{
    int i_cnt = 0;    
    
    cfgFileScan( true );	
    while(1)	
    {
	if(stop_signal) break;   
	if( ++i_cnt > 10*1000/STD_WAIT_DELAY )  //10 second
	{
	    i_cnt = 0;
    	    cfgFileScan( );	
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

void TSYS::kAdd( const string &name )
{
    if( chldAvoid(m_kern,name) )	return;
    chldAdd(m_kern,new TKernel(name));
}

void TSYS::kDel( const string &name )
{ 
    chldDel(m_kern,name);
}	    

void TSYS::cfgFileScan( bool first )
{
    static string cfg_fl;
    static struct stat f_stat;
    
    struct stat f_stat_t;
    bool   up = false;

    if(cfg_fl == cfgFile())
    {
	stat(cfg_fl.c_str(),&f_stat_t);
	if( f_stat.st_mtime != f_stat_t.st_mtime ) up = true;
    }
    else up = true;
    cfg_fl = cfgFile();
    stat(cfg_fl.c_str(),&f_stat);
    if(up == true && !first )
    {
	updateOpt();
	Mess->updateOpt();
	
	vector<string> list;
	kList( list );
	for( unsigned i_kern = 0; i_kern < list.size(); i_kern++)
    	    kAt(list[i_kern]).at().updateOpt();	    
    }    
}

string TSYS::fNameFix( const string &fname )
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


bool TSYS::eventWait( bool &m_mess_r_stat, bool exempl, const string &loc, time_t tm )
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

string TSYS::strSepParse( const string &path, int level, char sep )
{
    int an_dir = 0, t_lev = 0;
    while(true)
    {
        int t_dir = path.find(sep,an_dir);
		    
        if( t_lev++ == level )	return( path.substr(an_dir,t_dir-an_dir) );
        if( t_dir == string::npos ) return("");
        an_dir = t_dir+1;
    }
}		

//================== Controll functions ========================
void TSYS::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='gen' acs='0440'>"
	  "<fld id='workdir' acs='0440' tp='str'/>"
	  "<fld id='config' acs='0660' tp='str' dest='file'/>"
	  "<fld id='in_charset' acs='0440' tp='str'/>"
	  "<fld id='lang' acs='0660' tp='str'/>"
	  "<fld id='debug' acs='0660' tp='dec' len='1' min='0' max='8'/>"
	  "<comm id='upd_opt'/>"
	 "</area>"
	 "<area id='mess'>"	
	  "<fld id='m_buf_l' acs='0660' tp='dec' min='10'/>"
	  "<fld id='log_sysl' acs='0660' tp='bool'/>"
	  "<fld id='log_stdo' acs='0660' tp='bool'/>"
	  "<fld id='log_stde' acs='0660' tp='bool'/>"
	  "<fld id='v_beg' tp='time'/>"
	  "<fld id='v_end' tp='time'/>"
	  "<fld id='v_cat' tp='str'/>"
	  "<fld id='v_lvl' tp='dec' min='0' max='7'/>"
	  "<table id='mess' acs='0440'>"
	   "<list id='0' tp='time'/>"
	   "<list id='1' tp='str'/>"
	   "<list id='2' tp='dec'/>"
	   "<list id='3' tp='str'/>"
	  "</table>"
	 "</area>"
         "<area id='kern'>"
          "<list id='br' acs='0774' s_com='add,del' tp='br' mode='att' br_pref='_'/>"
         "</area>"				
	 "<area id='hlp'>"
	  "<area id='s_inf'>"
	   "<fld id='stat' acs='0444' tp='str'/>"
	   "<fld id='prog' acs='0444' tp='str'/>"
	   "<fld id='ver' acs='0444' tp='str'/>"
	   "<fld id='host' acs='0444' tp='str'/>"
	   "<fld id='user' acs='0444' tp='str'/>"
	   "<fld id='sys' acs='0444' tp='str'/>"
	  "</area>"
	  "<fld id='g_help' acs='0444' tp='str' cols='90' rows='5'/>"	
	 "</area>"
	"</oscada_cntr>";
    char buf[STR_BUF_LEN];
    
    inf->load( i_cntr );
    snprintf(buf,sizeof(buf),Mess->I18N("%s station: %s"),PACKAGE_NAME,m_station.c_str());
    inf->text(buf);
    
    //gen
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Station"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Work directory"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Config file"));
    c_nd->childGet(2)->attr(dscr,Mess->I18N("Internal charset"));
    c_nd->childGet(3)->attr(dscr,Mess->I18N("Language"));
    c_nd->childGet(4)->attr(dscr,Mess->I18N("Debug level"));
    c_nd->childGet(5)->attr(dscr,Mess->I18N("Update options(from config)"));
    
    //mess
    c_nd = inf->childGet(1);
    c_nd->attr(dscr,Mess->I18N("Station messages"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Message buffer size"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Messages to syslog"));
    c_nd->childGet(2)->attr(dscr,Mess->I18N("Messages to stdout"));    
    c_nd->childGet(3)->attr(dscr,Mess->I18N("Messages to stderr"));    
    c_nd->childGet(4)->attr(dscr,Mess->I18N("Begin"));
    c_nd->childGet(5)->attr(dscr,Mess->I18N("End"));
    c_nd->childGet(6)->attr(dscr,Mess->I18N("Category"));
    c_nd->childGet(7)->attr(dscr,Mess->I18N("Level"));
    
    c_nd = c_nd->childGet(8);
    c_nd->attr(dscr,Mess->I18N("Messages"));    
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Time"));    
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Category"));
    c_nd->childGet(2)->attr(dscr,Mess->I18N("Level"));   
    c_nd->childGet(3)->attr(dscr,Mess->I18N("Message"));     
	
    //kern
    c_nd = inf->childGet(2);
    c_nd->attr(dscr,Mess->I18N("Kernels"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Kernels"));
    
    //hlp
    c_nd = inf->childGet(3);
    c_nd->attr(dscr,Mess->I18N("Help"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Options help"));        
    
    //base
    c_nd = c_nd->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Station information"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Station"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Programm"));
    c_nd->childGet(2)->attr(dscr,Mess->I18N("Version"));
    c_nd->childGet(3)->attr(dscr,Mess->I18N("Host name"));
    c_nd->childGet(4)->attr(dscr,Mess->I18N("System user"));
    c_nd->childGet(5)->attr(dscr,Mess->I18N("Operation system"));    
}

void TSYS::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    char cbuf[STR_BUF_LEN];
    utsname buf;
    uname(&buf);    

    if( a_path == "/gen/workdir" )      ctrSetS( opt, getcwd(cbuf,sizeof(cbuf)) );    
    else if( a_path == "/gen/config" )	ctrSetS( opt, m_confFile );
    else if( a_path == "/gen/debug" )	ctrSetI( opt, Mess->d_level() );
    else if( a_path == "/gen/lang" )   	ctrSetS( opt, Mess->lang() );
    else if( a_path == "/gen/in_charset" )	ctrSetS( opt, Mess->charset() );
    else if( a_path == "/mess/m_buf_l" )	ctrSetI( opt, Mess->mess_buf_len() );
    else if( a_path == "/mess/log_sysl" )ctrSetB( opt, (Mess->log_direct()&0x01)?true:false );
    else if( a_path == "/mess/log_stdo" )ctrSetB( opt, (Mess->log_direct()&0x02)?true:false );
    else if( a_path == "/mess/log_stde" )ctrSetB( opt, (Mess->log_direct()&0x04)?true:false );
    else if( a_path == "/mess/v_beg" )	ctrSetI( opt, m_beg );
    else if( a_path == "/mess/v_end" )	ctrSetI( opt, m_end );
    else if( a_path == "/mess/v_cat" )	ctrSetS( opt, m_cat );
    else if( a_path == "/mess/v_lvl" )	ctrSetI( opt, m_lvl );
    else if( a_path == "/mess/mess" )
    {
	vector<TMessage::SRec> rec;
	Mess->get(  m_beg, m_end, rec, m_cat, m_lvl );
	
	XMLNode *n_tm   = ctrId(opt,"0");
	XMLNode *n_cat  = ctrId(opt,"1");
	XMLNode *n_lvl  = ctrId(opt,"2");
	XMLNode *n_mess = ctrId(opt,"3");
	for( int i_rec = 0; i_rec < rec.size(); i_rec++)
	{
	    ctrSetI(n_tm,rec[i_rec].time);
	    ctrSetS(n_cat,rec[i_rec].categ);
	    ctrSetI(n_lvl,rec[i_rec].level);
	    ctrSetS(n_mess,rec[i_rec].mess);
	}        
    }
    else if( a_path == "/kern/br" )
    {
	vector<string> list;
	kList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] ); 
    }
    else if( a_path == "/hlp/s_inf/host" )     	ctrSetS( opt, buf.nodename );
    else if( a_path == "/hlp/s_inf/sys" )      	ctrSetS( opt, string(buf.sysname)+"-"+buf.release );
    else if( a_path == "/hlp/s_inf/user" )     	ctrSetS( opt, m_user );
    else if( a_path == "/hlp/s_inf/prog" )     	ctrSetS( opt, PACKAGE_NAME );
    else if( a_path == "/hlp/s_inf/ver" )      	ctrSetS( opt, VERSION );
    else if( a_path == "/hlp/s_inf/stat" )     	ctrSetS( opt, m_station );
    else if( a_path == "/hlp/g_help" )    	ctrSetS( opt, optDescr() );       
    else throw TError("(%s:%s) Branch <%s> error",o_name,__func__,a_path.c_str());
}

void TSYS::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/gen/config" )       	m_confFile = ctrGetS( opt );
    else if( a_path == "/gen/debug" )        	Mess->d_level( ctrGetI( opt ) );
    else if( a_path == "/gen/lang" )         	Mess->lang(ctrGetS( opt ) );
    else if( a_path == "/mess/m_buf_l" )      	Mess->mess_buf_len( ctrGetI( opt ) );
    else if( a_path == "/mess/log_sysl" )     
    {
	if( ctrGetB( opt ) )	Mess->log_direct( Mess->log_direct()|0x01 );
	else                      	Mess->log_direct( Mess->log_direct()&(~0x01) );
    }
    else if( a_path == "/mess/log_stdo" )     
    {
	if( ctrGetB( opt ) ) 	Mess->log_direct( Mess->log_direct()|0x02 );
	else                      	Mess->log_direct( Mess->log_direct()&(~0x02) );
    }
    else if( a_path == "/mess/log_stde" )     
    {
	if( ctrGetB( opt ) ) 	Mess->log_direct( Mess->log_direct()|0x04 );
	else                      	Mess->log_direct( Mess->log_direct()&(~0x04) );
    }
    else if( a_path == "/gen/upd_opt" ) 
    {
	updateOpt();
	Mess->updateOpt();
    }
    else if( a_path == "/mess/v_beg" )	m_beg = ctrGetI(opt);
    else if( a_path == "/mess/v_end" )  m_end = ctrGetI(opt);
    else if( a_path == "/mess/v_cat" )  m_cat = ctrGetS(opt);
    else if( a_path == "/mess/v_lvl" )  m_lvl = ctrGetI(opt);
    else if( a_path.substr(0,10) == "/kern/br" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      kAdd(t_c->text());
		else if(t_c->attr("do") == "del") kDel(t_c->text());
	    }
	}
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

AutoHD<TCntrNode> TSYS::ctrAt1( const string &br )
{ 
    if(br.substr(0,1)=="_")	return kAt( br.substr(1) );
    else			throw TError("<{%s}> Branch %s error!",__func__,br.c_str());
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
	resRequestW(m_id, tm);
    }
    else
    {
	m_wr &= ~0x02;
	resRequestR(m_id, tm);	
    }
}

    
void ResAlloc::release()
{
    if( !(m_wr&0x01) ) throw TError("%s: a resource didn't alloc!", o_name);    
    m_wr &= ~0x01;    
    if( m_wr&0x02 ) resReleaseW(m_id);
    else            resReleaseR(m_id);	
}

unsigned ResAlloc::resCreate( unsigned val )
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

void ResAlloc::resDelete( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("%s: error delete semaphor %d!", o_name, res);
    
    sems[res].del = true;
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
    sem_destroy( &sems[res].sem );
    sems[res].use = false;   
}

void ResAlloc::resRequestW( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <w> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
}

void ResAlloc::resReleaseW( unsigned res )
{
    if(res >= sems.size() || !sems[res].use )
	throw TError("%s: error <w> release semaphor %d!", o_name, res);
    sem_post( &sems[res].sem );
}

void ResAlloc::resRequestR( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <r> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    sems[res].rd_c++;   
    sem_post( &sems[res].sem );
}

void ResAlloc::resReleaseR( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("%s: error <r> release semaphor %d!", o_name, res);
    if( sems[res].rd_c > 0 ) sems[res].rd_c--;   
}


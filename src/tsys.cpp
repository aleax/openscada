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

TSYS::TSYS( int argi, char ** argb, char **env ) : m_confFile("/etc/oscada.xml"), stat_n(NULL), 
    m_user("root"),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0), 
    m_cr_f_perm(0644), m_cr_d_perm(0755), m_beg(time(NULL)), m_end(time(NULL)), m_cat(""), m_lvl(0)
{
    if( getenv("USER") )	
	m_user = getenv("USER");
    
    m_kern = grpAdd();
    nodeEn();
    
    //Init system clock
    clkCalc();
    
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGCHLD,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
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

bool TSYS::strEmpty( const string &val )
{
    if( !val.size() )	return true;
    for( int i_s = 0; i_s < val.size(); i_s++ )
	if( val[i_s] != ' ' && val[i_s] != '\n' && val[i_s] != '\t' )
	    return false;
    
    return true;
}

XMLNode *TSYS::cfgNode() 
{ 
    if(!stat_n) throw TError(Mess->I18N("(SYS)Config file <%s> error or no avoid!"),m_confFile.c_str());
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
	"    --demon            Start to demon mode;\n"
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

void TSYS::load()
{
    //================ Load parameters from commandline =========================
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
    
    //================ Load parameters from config file =========================
    stat_n = NULL;
    int hd = open(m_confFile.c_str(),O_RDONLY);
    if( hd < 0 ) 
	Mess->put("SYS",TMess::Error,Mess->I18N("Config file <%s> error: %s"),m_confFile.c_str(),strerror(errno));
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
	    if( root_n.name() == "OpenSCADA" )
	    {
		for( int i_st = root_n.childSize()-1; i_st >= 0; i_st--)
	            if( root_n.childGet(i_st)->name() == "station" )
		    {
			stat_n = root_n.childGet(i_st);
			if( stat_n->attr("id") == m_station ) break;							
		    }
		if(stat_n && stat_n->attr("id") != m_station )
		{ 
		    Mess->put("SYS",TMess::Error,Mess->I18N("Station <%s> config no avoid. Use <%s> station config!"),
			m_station.c_str(), stat_n->attr("id").c_str() );
		    m_station = stat_n->attr("id");
		}
	    }
	}
	catch( TError err ) { Mess->put("SYS",TMess::Error,"(SYS)%s", err.what().c_str() ); }
    }
    
    //All system parameters
    try{ chdir( cfgNode()->childGet("id","workdir")->text().c_str() ); }
    catch(...) {  }
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
	    clkCalc( );
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
	Mess->put("SYS",TMess::Warning,"Have get a Terminate signal. Server been stoped!"); 
	SYS->stop_signal=signal; 
    }
    else if(signal == SIGCHLD)
    {
	int status;
	pid_t pid = wait(&status);
	if(!WIFEXITED(status))
	    Mess->put("SYS",TMess::Info,"Free child process %d!",pid);
    }	
    else if(signal == SIGPIPE)
	Mess->put("SYS",TMess::Warning,Mess->I18N("Broken PIPE signal!"));
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
	load();
	Mess->load();
	
	vector<string> list;
	kList( list );
	for( unsigned i_kern = 0; i_kern < list.size(); i_kern++)
    	    kAt(list[i_kern]).at().load();
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
	    Mess->put("SYS",TMess::Crit,"Go timeout %s!!!!",loc.c_str());
    	    return(true);
	}
	//Make messages
	if( c_tm > t_tm+1 )  //1sec 
	{
	    t_tm = c_tm;
	    Mess->put_s("SYS",TMess::Info,loc);
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

string TSYS::pathLev( const string &path, int level, bool encode )
{
    int an_dir = 0, t_lev = 0;
    while(path[an_dir]=='/') an_dir++;
    while(true)
    {
        int t_dir = path.find("/",an_dir);
			
        if( t_lev++ == level )
        {
            if( encode ) return( strEncode(path.substr(an_dir,t_dir-an_dir),TSYS::PathEl) );
            return( path.substr(an_dir,t_dir-an_dir) );
        }
	if( t_dir == string::npos ) return("");
        an_dir = t_dir;
        while( an_dir < path.size() && path[an_dir]=='/') an_dir++;
    }
}

string TSYS::strCode( const string &in, TSYS::Code tp )
{
    string sout = in;
    
	if( tp == TSYS::Path )
	{
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		if( sout[i_sz] == '/' ) sout[i_sz] = ':';
	}
	else if( tp == TSYS::PathEl )
	{
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )	    
		if( sout[i_sz] == '/' )     	{ sout.replace(i_sz,1,"%2f"); i_sz+=2; }
		else if( sout[i_sz] == ':' )	{ sout.replace(i_sz,1,"%3a"); i_sz+=2; }
		else if( sout[i_sz] == '%' )	{ sout.replace(i_sz,1,"%25"); i_sz++; }
	}
	else if( tp == TSYS::HttpURL )
	{	
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )	    
		if( sout[i_sz] == '%' )		{ sout.replace(i_sz,1,"%25"); i_sz++; }
		else if( sout[i_sz] == ' ')	{ sout.replace(i_sz,1,"%20"); i_sz+=2; }
		else if( sout[i_sz]&0x80 )
		{
		    char buf[4];
		    snprintf(buf,sizeof(buf),"%%%2X",(unsigned char)sout[i_sz]);
		    sout.replace(i_sz,1,buf);
		    i_sz+=2;
		}	
	}
	else if( tp == TSYS::Html )
	{
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )	    
	        if( sout[i_sz] == '>' )    	{ sout.replace(i_sz,1,"&gt;"); i_sz+=3; }
	        else if( sout[i_sz] == '<' ) 	{ sout.replace(i_sz,1,"&lt;"); i_sz+=3; }
	        else if( sout[i_sz] == '"' )    { sout.replace(i_sz,1,"&quot;"); i_sz+=5; }
	        else if( sout[i_sz] == '&' ) 	{ sout.replace(i_sz,1,"&amp;"); i_sz+=4; }
	}
	else if( tp == TSYS::JavaSc )
        {
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
                if( sout[i_sz] == '\n' )        { sout.replace(i_sz,1,"\\n"); i_sz++; }	
	}
								    
    return(sout);
}

string TSYS::strEncode( const string &in, TSYS::Code tp )
{
    int n_pos=0;
    string path = in;

    if( tp == TSYS::Path )
    {	
	for( unsigned i_sz = 0; i_sz < path.size(); i_sz++ )
    	    if( path[i_sz] == ':' ) path[i_sz] = '/';
    }
    else if( tp == TSYS::PathEl || tp == TSYS::HttpURL )
    {
        while(true)
        {
            n_pos = path.find("%",n_pos);
            if( n_pos == string::npos ) break;
            if( path[n_pos+1] == '%' ) path.replace(n_pos,2,"%");
    	    else path.replace(n_pos,3,string("")+(char)strtol(path.substr(n_pos+1,2).c_str(),NULL,16));
            n_pos+=1;
        }
    }
	    
    return(path);
}

long TSYS::TZ()
{
    return sysconf(_SC_CLK_TCK);
}

//================== Controll functions ========================
void TSYS::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    char buf[STR_BUF_LEN];
    
    if( cmd==TCntrNode::Info )
    {
	snprintf(buf,sizeof(buf),Mess->I18N("%s station: %s"),PACKAGE_NAME,m_station.c_str());
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",buf);
	ctrMkNode("area",opt,a_path.c_str(),"/gen",Mess->I18N("Station"),0440);
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/workdir",Mess->I18N("Work directory"),0440,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/config",Mess->I18N("Config file"),0660,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/in_charset",Mess->I18N("Internal charset"),0440,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/lang",Mess->I18N("Language"),0660,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/debug",Mess->I18N("Debug level"),0660,0,0,"dec")->
	    attr_("len","1")->attr_("min","0")->attr_("max","8");
	ctrMkNode("comm",opt,a_path.c_str(),"/gen/load",Mess->I18N("Load system"));
	ctrMkNode("area",opt,a_path.c_str(),"/mess",Mess->I18N("Station messages"));
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/m_buf_l",Mess->I18N("Message buffer size"),0660,0,0,"dec")->
	    attr_("min","10");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/log_sysl",Mess->I18N("Messages to syslog"),0660,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/log_stdo",Mess->I18N("Messages to stdout"),0660,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/log_stde",Mess->I18N("Messages to stderr"),0660,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/v_beg",Mess->I18N("Begin"),0664,0,0,"time");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/v_end",Mess->I18N("End"),0664,0,0,"time");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/v_cat",Mess->I18N("Category"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/v_lvl",Mess->I18N("Level"),0664,0,0,"dec")->
	    attr_("min","0")->attr_("max","7");
	ctrMkNode("table",opt,a_path.c_str(),"/mess/mess",Mess->I18N("Messages"),0440);
	ctrMkNode("list",opt,a_path.c_str(),"/mess/mess/0",Mess->I18N("Time"),0440,0,0,"time");
	ctrMkNode("list",opt,a_path.c_str(),"/mess/mess/1",Mess->I18N("Category"),0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/mess/mess/2",Mess->I18N("Level"),0440,0,0,"dec");
	ctrMkNode("list",opt,a_path.c_str(),"/mess/mess/3",Mess->I18N("Message"),0440,0,0,"str");
	ctrMkNode("area",opt,a_path.c_str(),"/kern",Mess->I18N("Kernels"));
	ctrMkNode("list",opt,a_path.c_str(),"/kern/br",Mess->I18N("Kernels"),0664,0,0,"br")->
	    attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_");
	ctrMkNode("area",opt,a_path.c_str(),"/hlp",Mess->I18N("Help"));
	ctrMkNode("area",opt,a_path.c_str(),"/hlp/s_inf",Mess->I18N("Station information"));
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/stat",Mess->I18N("Station"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/prog",Mess->I18N("Programm"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/ver",Mess->I18N("Version"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/host",Mess->I18N("Host name"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/user",Mess->I18N("System user"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/sys",Mess->I18N("Operation system"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/frq",Mess->I18N("Frequency (MHZ)"),0444,0,0,"real");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/g_help",Mess->I18N("Options help"),0444,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	utsname ubuf;
	uname(&ubuf);    

	if( a_path == "/gen/workdir" )		ctrSetS( opt, getcwd(buf,sizeof(buf)) );    
	else if( a_path == "/gen/config" )	ctrSetS( opt, m_confFile );
	else if( a_path == "/gen/debug" )	ctrSetI( opt, Mess->d_level() );
	else if( a_path == "/gen/lang" )   	ctrSetS( opt, Mess->lang() );
	else if( a_path == "/gen/in_charset" )	ctrSetS( opt, Mess->charset() );
	else if( a_path == "/mess/m_buf_l" )	ctrSetI( opt, Mess->mess_buf_len() );
	else if( a_path == "/mess/log_sysl" )	ctrSetB( opt, (Mess->log_direct()&0x01)?true:false );
	else if( a_path == "/mess/log_stdo" )	ctrSetB( opt, (Mess->log_direct()&0x02)?true:false );
	else if( a_path == "/mess/log_stde" )	ctrSetB( opt, (Mess->log_direct()&0x04)?true:false );
	else if( a_path == "/mess/v_beg" )	ctrSetI( opt, m_beg );
	else if( a_path == "/mess/v_end" )	ctrSetI( opt, m_end );
	else if( a_path == "/mess/v_cat" )	ctrSetS( opt, m_cat );
	else if( a_path == "/mess/v_lvl" )	ctrSetI( opt, m_lvl );
	else if( a_path == "/mess/mess" )
	{
	    vector<TMess::SRec> rec;
	    Mess->get( m_beg, m_end, rec, m_cat, (TMess::Type)m_lvl );
	    
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
	else if( a_path == "/hlp/s_inf/host" ) 	ctrSetS( opt, ubuf.nodename );
	else if( a_path == "/hlp/s_inf/sys" )  	ctrSetS( opt, string(ubuf.sysname)+"-"+ubuf.release );
	else if( a_path == "/hlp/s_inf/user" ) 	ctrSetS( opt, m_user );
	else if( a_path == "/hlp/s_inf/prog" ) 	ctrSetS( opt, PACKAGE_NAME );
	else if( a_path == "/hlp/s_inf/ver" )  	ctrSetS( opt, VERSION );
	else if( a_path == "/hlp/s_inf/stat" ) 	ctrSetS( opt, m_station );
	else if( a_path == "/hlp/s_inf/frq" ) 	ctrSetR( opt, (float)sysClk()/1000000. );
	else if( a_path == "/hlp/g_help" )	ctrSetS( opt, optDescr() );       
	else throw TError("(SYS)Branch <%s> error",a_path.c_str());	    
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/gen/config" )		m_confFile = ctrGetS( opt );
	else if( a_path == "/gen/debug" )       Mess->d_level( ctrGetI( opt ) );
	else if( a_path == "/gen/lang" )        Mess->lang(ctrGetS( opt ) );
	else if( a_path == "/mess/m_buf_l" )    Mess->mess_buf_len( ctrGetI( opt ) );
	else if( a_path == "/mess/log_sysl" )	
	    Mess->log_direct( (ctrGetB( opt )?Mess->log_direct()|0x01:Mess->log_direct()&(~0x01)) );
	else if( a_path == "/mess/log_stdo" )     
	    Mess->log_direct( (ctrGetB( opt )?Mess->log_direct()|0x02:Mess->log_direct()&(~0x02)) );
	else if( a_path == "/mess/log_stde" )     
	    Mess->log_direct( (ctrGetB( opt )?Mess->log_direct()|0x04:Mess->log_direct()&(~0x04)) );
	else if( a_path == "/gen/load" ) 
	{
	    load();
	    Mess->load();
	}
	else if( a_path == "/mess/v_beg" )	m_beg = ctrGetI(opt);
	else if( a_path == "/mess/v_end" )  	m_end = ctrGetI(opt);
	else if( a_path == "/mess/v_cat" )  	m_cat = ctrGetS(opt);
	else if( a_path == "/mess/v_lvl" )  	m_lvl = ctrGetI(opt);
	else if( a_path == "/kern/br" )
	{
	    if( opt->name() == "add" )		kAdd(opt->text());
	    else if( opt->name() == "del" )	kDel(opt->text());
	}
	else throw TError("(SYS)Branch %s error",a_path.c_str());	    
    }		
}

AutoHD<TCntrNode> TSYS::ctrAt1( const string &br )
{ 
    if(br.substr(0,1)=="_")	return kAt( br.substr(1) );
    else			throw TError("(SYS)Branch %s error!",br.c_str());
}

//==============================================================
//================== Controll functions ========================
//==============================================================
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
    if( m_wr & 0x01 ) throw TError("(ResAlloc)Resource already alloced!");    
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
    if( !(m_wr&0x01) ) throw TError("(ResAlloc)Resource didn't allocate!");    
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
	throw TError("(ResAlloc)Error open semaphor!");
    sems[i_sem].use = true;   
    sems[i_sem].del = false;   
    sems[i_sem].rd_c = 0;   

    return(i_sem);
}

void ResAlloc::resDelete( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("(ResAlloc)Error delete semaphor %d!", res);
    
    sems[res].del = true;
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
    sem_destroy( &sems[res].sem );
    sems[res].use = false;   
}

void ResAlloc::resRequestW( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("(ResAlloc)Error <w> request semaphor %d!", res);
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
}

void ResAlloc::resReleaseW( unsigned res )
{
    if(res >= sems.size() || !sems[res].use )
	throw TError("(ResAlloc)Error <w> release semaphor %d!", res);
    sem_post( &sems[res].sem );
}

void ResAlloc::resRequestR( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("(ResAlloc)Error <r> request semaphor %d!", res);
    sem_wait( &sems[res].sem );
    sems[res].rd_c++;   
    sem_post( &sems[res].sem );
}

void ResAlloc::resReleaseR( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("(ResAlloc)Error <r> release semaphor %d!", res);
    if( sems[res].rd_c > 0 ) sems[res].rd_c--;   
}


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
#include <stdarg.h>
#include <stdlib.h>
#include <langinfo.h>

#include "../config.h"
#include "terror.h"
#include "tmessage.h"
#include "tsys.h"

//Continuously access variable
TMess 	*Mess;
TSYS  	*SYS;

TSYS::TSYS( int argi, char ** argb, char **env ) : m_confFile("/etc/oscada.xml"), 
    m_user("root"),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0), 
    m_beg(time(NULL)), m_end(time(NULL)), m_cat(""), m_lvl(0),
    m_genDB(true), DefBDType(""), DefBDName("")
{
    m_subst = grpAdd();
    nodeEn();
    
    Mess = new TMess();

    if( getenv("USER") ) m_user = getenv("USER");
    
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
    nodeDelAll();
    
    delete Mess;
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
	"    --mess_lev=<level> Set messages <level> (0-7);\n"
    	"    --log=<direct>     Set direction a log and other info;\n"
    	"                         <direct> & 1 - syslogd;\n"
    	"                         <direct> & 2 - stdout;\n"
    	"                         <direct> & 4 - stderr;\n"
    	"----------- The config file station <%s> parameters -----------\n"
	"workdir    <path>	set the station work directory;\n"
    	"mess_lev   <level>     set messages <level> (0-7);\n"
    	"target_log <direction> set direction a log and other info;\n"
    	"                           <direct> & 1 - syslogd;\n"
    	"                           <direct> & 2 - stdout;\n"
    	"                           <direct> & 4 - stderr;\n"
    	"mess_buf   <len>       set messages buffer len;\n"
    	"DefaultBD <type:name>  set default bd type and bd name (next, may use only table name);\n\n"),
	PACKAGE_NAME,VERSION,buf.sysname,buf.release,nodePath().c_str());
	
    return(s_buf);
}

bool TSYS::cfgFileLoad()
{	
    bool cmd_help = false;    

    //================ Load parameters from commandline =========================
    int next_opt;
    char *short_opt="h";
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
	    case 'h': 
		fprintf(stdout,optDescr().c_str()); 
		Mess->messLevel(7);
		cmd_help = true; 
		break;
	    case 'f': m_confFile = optarg; break;
	    case 's': m_station = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //Load config file
    int hd = open(m_confFile.c_str(),O_RDONLY);
    if( hd < 0 ) 
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Config file <%s> error: %s"),m_confFile.c_str(),strerror(errno));
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
	try{ root_n.load(s_buf); }
	catch( TError err ) { Mess->put((nodePath()+err.cat).c_str(),TMess::Error, err.mess.c_str() ); }
    }
    
    return cmd_help;
}

void TSYS::load()
{
    bool cmd_help = cfgFileLoad();
    
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Load!"));   
    
    //All system parameters
    string opt;        
    try{ chdir( ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","workdir")->text().c_str() ); }
    catch(...) {  }
    try
    {
	opt = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","DefaultBD")->text();
	DefBDType = TSYS::strSepParse(opt,0,':');
	DefBDName = TSYS::strSepParse(opt,1,':');
    }
    catch(...) {  }
    
    Mess->load();	//Messages load

    if(!avoid("BD"))	add(new TBDS(this));
    if(!avoid("func"))	add(new TFunctionS(this));
    if(!avoid("params"))	add(new TParamS(this));
    if(!avoid("sequrity"))	add(new TSequrity(this));
    if(!avoid("Transport"))	add(new TTransportS(this));
    if(!avoid("Protocol"))	add(new TProtocolS(this));
    if(!avoid("Archive"))	add(new TArchiveS(this));
    if(!avoid("Controller"))	add(new TControllerS(this));
    if(!avoid("Special"))	add(new TSpecialS(this));
    if(!avoid("UI"))	add(new TUIS(this));
    if(!avoid("m_shed"))
    {	
	add(new TModSchedul(this));    
    	//Load modules
    	modSchedul().at().subLoad();
    	modSchedul().at().loadLibS();
    }

    //================== Load subsystems and modules ============    
    vector<string> lst;
    list(lst);
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
        try{ at(lst[i_a]).at().subLoad(); }
	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }

    Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Load OK!"));
    
    if( cmd_help ) throw TError(nodePath().c_str(),"Command line help call.");
}

int TSYS::start(  )
{
    vector<string> lst;
    list(lst);
    
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Start!"));
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	try{ at(lst[i_a]).at().subStart(); }
	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Start OK!"));
    
    cfgFileScan( true );	
    int i_cnt = 0;    
    while(!stop_signal)	
    {
	if( ++i_cnt > 10*1000/STD_WAIT_DELAY )  //10 second
	{
	    i_cnt = 0;
	    clkCalc( );
    	    cfgFileScan( );	
	}
       	usleep( STD_WAIT_DELAY*1000 ); 
    }
    
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Stop!"));    
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	try{ at(lst[i_a]).at().subStop(); }
	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Stop OK!"));

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
	Mess->put(SYS->nodePath().c_str(),TMess::Warning,"Have get a Terminate signal. Server been stoped!"); 
	SYS->stop_signal=signal; 
    }
    else if(signal == SIGCHLD)
    {
	int status;
	pid_t pid = wait(&status);
	if(!WIFEXITED(status))
	    Mess->put(SYS->nodePath().c_str(),TMess::Info,"Free child process %d!",pid);
    }	
    else if(signal == SIGPIPE)
	Mess->put(SYS->nodePath().c_str(),TMess::Warning,Mess->I18N("Broken PIPE signal!"));
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
	
	vector<string> lst;
	list( lst );
	for( unsigned i_sub = 0; i_sub < lst.size(); i_sub++)
    	    at(lst[i_sub]).at().subLoad();
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
	    Mess->put(loc.c_str(),TMess::Crit,"Timeouted !!!");
    	    return(true);
	}
	//Make messages
	if( c_tm > t_tm+1 )  //1sec 
	{
	    t_tm = c_tm;
	    Mess->put(loc.c_str(),TMess::Info,"Wait event...");
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
void TSYS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    char buf[STR_BUF_LEN];
    
    if( cmd==TCntrNode::Info )
    {
	snprintf(buf,sizeof(buf),Mess->I18N("%s station: %s"),PACKAGE_NAME,m_station.c_str());
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",buf);
	ctrMkNode("area",opt,a_path.c_str(),"/gen",Mess->I18N("Station"),0440);	
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/self",Mess->I18N("Use generic DB"),0664,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/def_tp_bd",Mess->I18N("Default bd(module:bd)"),0664,0,0,"str")->
	    attr_("dest","select")->attr_("select","/gen/b_mod");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/def_bd","",0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/workdir",Mess->I18N("Work directory"),0440,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/config",Mess->I18N("Config file"),0660,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/in_charset",Mess->I18N("Internal charset"),0440,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/lang",Mess->I18N("Language"),0660,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/gen/load",Mess->I18N("Load system"));
	ctrMkNode("area",opt,a_path.c_str(),"/mess",Mess->I18N("Station messages"));
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/m_buf_l",Mess->I18N("Message buffer size"),0660,0,0,"dec")->
	    attr_("min","10");
	ctrMkNode("fld",opt,a_path.c_str(),"/mess/level",Mess->I18N("Messages level"),0660,0,0,"dec")->
            attr_("len","1")->attr_("min","0")->attr_("max","7");
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
	ctrMkNode("area",opt,a_path.c_str(),"/subs",Mess->I18N("Subsystems"));
	ctrMkNode("list",opt,a_path.c_str(),"/subs/br",Mess->I18N("Subsystems"),0555,0,0,"br")->
	    attr_("mode","att")->attr_("br_pref","_");
	ctrMkNode("area",opt,a_path.c_str(),"/hlp",Mess->I18N("Help"));
	ctrMkNode("area",opt,a_path.c_str(),"/hlp/s_inf",Mess->I18N("Station information"));
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/stat",Mess->I18N("Station"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/prog",Mess->I18N("Programm"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/ver",Mess->I18N("Version"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/host",Mess->I18N("Host name"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/user",Mess->I18N("System user"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/sys",Mess->I18N("Operation system"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/frq",Mess->I18N("Frequency (MHZ)"),0444,0,0,"real");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/s_inf/clk_tk",Mess->I18N("Clock ticks (HZ)"),0444,0,0,"real");
	ctrMkNode("fld",opt,a_path.c_str(),"/hlp/g_help",Mess->I18N("Options help"),0444,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	utsname ubuf;
	uname(&ubuf);    

	if( a_path == "/gen/self" )		ctrSetB( opt, m_genDB );
	else if( a_path == "/gen/def_tp_bd" )	ctrSetS( opt, DefBDType );
	else if( a_path == "/gen/def_bd" )    	ctrSetS( opt, DefBDName );     
	else if( a_path == "/gen/b_mod" )
	{
	    vector<string> list;
	    db().at().modList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
	else if( a_path == "/gen/workdir" )	ctrSetS( opt, getcwd(buf,sizeof(buf)) );    
	else if( a_path == "/gen/config" )	ctrSetS( opt, m_confFile );
	else if( a_path == "/gen/lang" )   	ctrSetS( opt, Mess->lang() );
	else if( a_path == "/gen/in_charset" )	ctrSetS( opt, Mess->charset() );	
	else if( a_path == "/mess/m_buf_l" )	ctrSetI( opt, Mess->mess_buf_len() );
	else if( a_path == "/mess/level" ) 	ctrSetI( opt, Mess->messLevel() );
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
	else if( a_path.substr(0,8) == "/subs/br" )
	{
	    vector<string> lst;
	    list(lst);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
		ctrSetS( opt, at(lst[i_a]).at().subName(), lst[i_a].c_str() );         
	}
	else if( a_path == "/hlp/s_inf/host" ) 	ctrSetS( opt, ubuf.nodename );
	else if( a_path == "/hlp/s_inf/sys" )  	ctrSetS( opt, string(ubuf.sysname)+"-"+ubuf.release );
	else if( a_path == "/hlp/s_inf/user" ) 	ctrSetS( opt, m_user );
	else if( a_path == "/hlp/s_inf/prog" ) 	ctrSetS( opt, PACKAGE_NAME );
	else if( a_path == "/hlp/s_inf/ver" )  	ctrSetS( opt, VERSION );
	else if( a_path == "/hlp/s_inf/stat" ) 	ctrSetS( opt, m_station );
	else if( a_path == "/hlp/s_inf/frq" ) 	ctrSetR( opt, (float)sysClk()/1000000. );
	else if( a_path == "/hlp/s_inf/clk_tk" )ctrSetR( opt, TZ() );
	else if( a_path == "/hlp/g_help" )	ctrSetS( opt, optDescr() );       
	else throw TError(nodePath().c_str(),"Branch <%s> error",a_path.c_str());	    
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/gen/self" )       	m_genDB = ctrGetB( opt );
	else if( a_path == "/gen/def_tp_bd" )	DefBDType = ctrGetS( opt );
	else if( a_path == "/gen/def_bd" )	DefBDName = ctrGetS( opt ); 
	else if( a_path == "/gen/config" )	m_confFile = ctrGetS( opt );
	else if( a_path == "/gen/lang" )        Mess->lang(ctrGetS( opt ) );
	else if( a_path == "/mess/m_buf_l" )    Mess->mess_buf_len( ctrGetI( opt ) );
	else if( a_path == "/mess/level" )     	Mess->messLevel( ctrGetI( opt ) );
	else if( a_path == "/mess/log_sysl" )	
	    Mess->log_direct( (ctrGetB( opt )?Mess->log_direct()|0x01:Mess->log_direct()&(~0x01)) );
	else if( a_path == "/mess/log_stdo" )     
	    Mess->log_direct( (ctrGetB( opt )?Mess->log_direct()|0x02:Mess->log_direct()&(~0x02)) );
	else if( a_path == "/mess/log_stde" )     
	    Mess->log_direct( (ctrGetB( opt )?Mess->log_direct()|0x04:Mess->log_direct()&(~0x04)) );
	else if( a_path == "/gen/load" ) 	load();
	else if( a_path == "/mess/v_beg" )	m_beg = ctrGetI(opt);
	else if( a_path == "/mess/v_end" )  	m_end = ctrGetI(opt);
	else if( a_path == "/mess/v_cat" )  	m_cat = ctrGetS(opt);
	else if( a_path == "/mess/v_lvl" )  	m_lvl = ctrGetI(opt);
	else throw TError(nodePath().c_str(),"Branch <%s> error",a_path.c_str());	    
    }		
}

AutoHD<TCntrNode> TSYS::ctrAt1( const string &br )
{ 
    if( br.substr(0,1)=="_")	return at( br.substr(1) );
    throw TError(nodePath().c_str(),"Branch <%s> error!",br.c_str());
}

TBDS::SName TSYS::nameDBPrep( const TBDS::SName &nbd )
{
    TBDS::SName bd = nbd;
    
    if( !bd.tp.size() || !bd.bd.size() || genDB() )
    {
	bd.tp = DefBDType;
	bd.bd = DefBDName;
    }
    
    return bd;
}


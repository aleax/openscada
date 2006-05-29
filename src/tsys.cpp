
//OpenSCADA system file: tsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
#include <sys/time.h>
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
#include "tmess.h"
#include "tsys.h"

//Continuously access variable
TMess 	*Mess;
TSYS  	*SYS;

TSYS::TSYS( int argi, char ** argb, char **env ) : m_confFile("/etc/oscada.xml"), m_id("EmptySt"), m_name("Empty Station"),
    m_user("root"),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0), 
    m_sysOptCfg(false), mWorkDB("")
{
    SYS = this;		//Init global access value
    m_subst = grpAdd("sub_");
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
    //signal(SIGSEGV,sighandler);
    signal(SIGABRT,sighandler);
}

TSYS::~TSYS(  )
{
    //Delete all nodes in order
    del("ModSched");
    del("UI");
    del("Special");
    del("Archive");
    del("Params");
    del("DAQ");
    del("Protocol");
    del("Transport");
    del("Security");
    del("BD");
    
    delete Mess;
}

string TSYS::int2str( int val, TSYS::IntView view )
{
    char buf[STR_BUF_LEN];
    if( view == TSYS::Dec )      snprintf(buf,sizeof(buf),"%d",val); 
    else if( view == TSYS::Oct ) snprintf(buf,sizeof(buf),"%o",val); 
    else if( view == TSYS::Hex ) snprintf(buf,sizeof(buf),"%x",val);

    return buf;
}

string TSYS::ll2str( long long val, IntView view )
{
    char buf[STR_BUF_LEN];
    if( view == TSYS::Dec )      snprintf(buf,sizeof(buf),"%lld",val);
    else if( view == TSYS::Oct ) snprintf(buf,sizeof(buf),"%llo",val);
    else if( view == TSYS::Hex ) snprintf(buf,sizeof(buf),"%llx",val);
		
    return buf;
}

string TSYS::real2str( double val )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),"%g",val); 

    return buf;
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
	"========================= The general system options ======================\n"
	"===========================================================================\n"
	"-h, --help             Info message about system options.\n"
	"    --Config=<path>    Config file path.\n"
	"    --Station=<name>   Station name.\n"
	"    --demon            Start into demon mode.\n"
	"    --MessLev=<level>  Process messages <level> (0-7).\n"
    	"    --log=<direct>     Direct messages to:\n"
    	"                         <direct> & 1 - syslogd;\n"
    	"                         <direct> & 2 - stdout;\n"
    	"                         <direct> & 4 - stderr;\n"
	"                         <direct> & 8 - archive.\n"
    	"----------- The config file station <%s> parameters -----------\n"
	"Workdir    <path>	Work directory.\n"
    	"MessLev    <level>     Messages <level> (0-7).\n"
    	"LogTarget  <direction> Direct messages to:\n"
    	"                           <direct> & 1 - syslogd;\n"
    	"                           <direct> & 2 - stdout;\n"
    	"                           <direct> & 4 - stderr;\n"
	"                           <direct> & 8 - archive.\n"
	"SysLang    <lang>	Internal language.\n"
    	"WorkDB     <Type.Name> Work DB (type and name).\n"
	"SYSOptCfg  <true>      Get system options from DB.\n\n"),
	PACKAGE_NAME,VERSION,buf.sysname,buf.release,nodePath().c_str());
		
    return s_buf;
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
	    case 's': m_id = optarg; break;
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
	try
	{ 
	    root_n.load(s_buf); 
	    if( root_n.name() == "OpenSCADA" )
	    {
		XMLNode *stat_n = NULL;
		for( int i_st = root_n.childSize()-1; i_st >= 0; i_st-- )
		    if( root_n.childGet(i_st)->name() == "station" )
		    {
			stat_n = root_n.childGet(i_st);
    			if( stat_n->attr("id") == m_id ) break;
		    }
                if( stat_n && stat_n->attr("id") != m_id )
                {
		    Mess->put(nodePath().c_str(),TMess::Warning,Mess->I18N("Station <%s> into config file no present. Use <%s> station config!"),
                        m_id.c_str(), stat_n->attr("id").c_str() );
  		    m_id 	= stat_n->attr("id");
		    m_name 	= Mess->codeConvIn( "UTF8",stat_n->attr("name"));
		}
		if( !stat_n )	root_n.clear();
	    } else root_n.clear();
	    if( !root_n.childSize() )
		Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Config <%s> error!"),m_confFile.c_str());
	}
	catch( TError err ) { Mess->put((nodePath()+err.cat).c_str(),TMess::Error, err.mess.c_str() ); }
    }
    
    return cmd_help;
}

void TSYS::cfgPrmLoad()
{
    //System parameters
    m_sysOptCfg = atoi(TBDS::genDBGet(nodePath()+"SYSOptCfg",TSYS::int2str(m_sysOptCfg),true).c_str());
    chdir(TBDS::genDBGet(nodePath()+"Workdir","",sysOptCfg()).c_str());
    
    mWorkDB = TBDS::genDBGet(nodePath()+"WorkDB","*.*",sysOptCfg());
}

void TSYS::load()
{
    static bool first_load = true;    
    
    if(first_load)
    {
	add(new TBDS());
	add(new TSecurity());
	add(new TTransportS());
	add(new TProtocolS());
	add(new TDAQS());
	add(new TParamS());
	add(new TArchiveS());
	add(new TSpecialS());
	add(new TUIS());
	add(new TModSchedul());
    }
    
    bool cmd_help = cfgFileLoad();
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Load!"));    
    cfgPrmLoad();
    Mess->load();	//Messages load
    
    if(first_load)
    {
    	//Load modules
    	modSchedul().at().subLoad();
    	modSchedul().at().loadLibS();
	//Second load for load from generic DB	
	cfgPrmLoad();
	Mess->load();
    }

    //================== Load subsystems and modules ============    
    vector<string> lst;
    list(lst);
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
        try{ at(lst[i_a]).at().subLoad(); }
	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    
    Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Load OK!"));
    
    if( cmd_help ) throw TError(nodePath().c_str(),"Command line help call.");
    first_load = false;
}

void TSYS::save( )
{
    char buf[STR_BUF_LEN];
    
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Save!"));
    
    //System parameters
    getcwd(buf,sizeof(buf));
    TBDS::genDBSet(SYS->nodePath()+"Workdir",buf);
    TBDS::genDBSet(SYS->nodePath()+"WorkDB",mWorkDB);
    
    Mess->save();       //Messages load
    
    vector<string> lst;
    list(lst);
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
        try{ at(lst[i_a]).at().subSave(); }
        catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }			    
    
    Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Save OK!"));
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
    for( int i_a=lst.size()-1; i_a >= 0; i_a-- )
	try{ at(lst[i_a]).at().subStop(); }
	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Stop OK!"));

    return(stop_signal);       
}

void TSYS::stop( )    
{ 
    stop_signal = SIGINT;
}

void TSYS::sighandler( int signal )
{
    switch(signal)
    {
	case SIGINT: 
    	    //Mess->put(3,"Have get a Interrupt signal. No stop server!");
	    SYS->stop_signal=signal; 
	    break;
	case SIGTERM:
	    Mess->put(SYS->nodePath().c_str(),TMess::Warning,"Have get a Terminate signal. Server been stoped!"); 
	    SYS->stop_signal=signal;
	    break;
	case SIGCHLD:
	{
	    int status;
	    pid_t pid = wait(&status);
	    if(!WIFEXITED(status) && pid > 0)
		Mess->put(SYS->nodePath().c_str(),TMess::Info,Mess->I18N("Free child process %d!"),pid);
	}
	case SIGPIPE:	
	    //Mess->put(SYS->nodePath().c_str(),TMess::Warning,Mess->I18N("Broken PIPE signal!"));
	    break;
	case SIGSEGV:
	    Mess->put(SYS->nodePath().c_str(),TMess::Emerg,Mess->I18N("Segmentation fault signal!"));
	    break;
	case SIGABRT:
	    Mess->put(SYS->nodePath().c_str(),TMess::Emerg,Mess->I18N("OpenSCADA aborted!"));
	    break;
	case SIGALRM:	break;    
	default:
	    Mess->put(SYS->nodePath().c_str(),TMess::Warning,Mess->I18N("Unknown signal %d!"),signal);
    }
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

long long TSYS::curTime()
{
    timeval cur_tm;
    gettimeofday(&cur_tm,NULL);
    return (long long)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;
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
    return false;
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

string TSYS::strCode( const string &in, TSYS::Code tp, const string &symb )
{
    string sout = in;
    
    switch(tp)	
    {
	case TSYS::Path:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		if( sout[i_sz] == '/' ) sout[i_sz] = ':';
	    break;	
	case TSYS::PathEl:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		switch(sout[i_sz])
		{
		    case '/': sout.replace(i_sz,1,"%2f"); i_sz+=2; break;
		    case ':': sout.replace(i_sz,1,"%3a"); i_sz+=2; break;
		    case '%': sout.replace(i_sz,1,"%25"); i_sz+=2; break;
		}
	    break;	
	case TSYS::HttpURL:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		switch(sout[i_sz])
		{
		    case '%': sout.replace(i_sz,1,"%25"); i_sz+=2; break;
		    case ' ': sout.replace(i_sz,1,"%20"); i_sz+=2; break;
		    case '\t': sout.replace(i_sz,1,"%09"); i_sz+=2; break;
		    default:
			if( sout[i_sz]&0x80 )
			{
			    char buf[4];
		    	    snprintf(buf,sizeof(buf),"%%%02X",(unsigned char)sout[i_sz]);
			    sout.replace(i_sz,1,buf);
			    i_sz+=2;
			    break;
			}
		}	
	    break;	
	case TSYS::Html:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		switch(sout[i_sz])
		{ 
		    case '>': sout.replace(i_sz,1,"&gt;"); i_sz+=3; break;
		    case '<': sout.replace(i_sz,1,"&lt;"); i_sz+=3; break;
		    case '"': sout.replace(i_sz,1,"&quot;"); i_sz+=5; break;
		    case '&': sout.replace(i_sz,1,"&amp;"); i_sz+=4; break;
		    case '\'': sout.replace(i_sz,1,"&#039;"); i_sz+=5; break;			     
		}
	    break;	
	case TSYS::JavaSc:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
                if( sout[i_sz] == '\n' )        { sout.replace(i_sz,1,"\\n"); i_sz++; }	
	    break;
	case TSYS::SQL:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		switch(sout[i_sz])
		{
		    case '\'': sout.replace(i_sz,1,"\\'"); i_sz++; break;
		    case '\"': sout.replace(i_sz,1,"\\\""); i_sz++; break;
		    case '`':  sout.replace(i_sz,1,"\\`"); i_sz++; break;
		    case '\\': sout.replace(i_sz,1,"\\\\"); i_sz++; break;
		}	    	
            break;
	case TSYS::Custom:
	    for( int i_sz = 0; i_sz < sout.size(); i_sz++ )
		for( int i_smb = 0; i_smb < symb.size(); i_smb++ )
		    if( sout[i_sz] == symb[i_smb] )
		    {
			char buf[4];
                        sprintf(buf,"%%%02X",(unsigned char)sout[i_sz]);
                        sout.replace(i_sz,1,buf);
                        i_sz+=2;
			break;
		    }
	    break;	    
	case TSYS::base64:
	{
	    sout = "";
	    char *base64alph = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	    for( int i_sz = 0; i_sz < in.size(); i_sz+=3 )
	    {
		if(i_sz && !(i_sz%57))	sout.push_back('\n');
		sout.push_back(base64alph[(unsigned char)in[i_sz]>>2]);
		if((i_sz+1) >= in.size()) 
		{
		    sout.push_back(base64alph[((unsigned char)in[i_sz]&0x03)<<4]);
		    sout+="==";
		}
		else
		{
		    sout.push_back(base64alph[(((unsigned char)in[i_sz]&0x03)<<4)|((unsigned char)in[i_sz+1]>>4)]);
		    if((i_sz+2) >= in.size()) 
		    {
			sout.push_back(base64alph[((unsigned char)in[i_sz+1]&0x0F)<<2]);
			sout.push_back('=');
		    }
		    else 
		    {
			sout.push_back(base64alph[(((unsigned char)in[i_sz+1]&0x0F)<<2)|((unsigned char)in[i_sz+2]>>6)]);
			sout.push_back(base64alph[(unsigned char)in[i_sz+2]&0x3F]);
		    }
		}
	    }
	    break;
	}
    }
    return sout;
}

unsigned char TSYS::getBase64Code(unsigned char asymb) 
{
    switch(asymb)
    {
	case 'A' ... 'Z':	return asymb-(unsigned char)'A';
	case 'a' ... 'z':	return 26+asymb-(unsigned char)'a';
	case '0' ... '9':	return 52+asymb-(unsigned char)'0';
	case '+': return 62;		    
	case '/': return 63;		
    }
}

string TSYS::strEncode( const string &in, TSYS::Code tp )
{
    int n_pos=0;
    string sout = in;

    switch(tp)
    {
	case TSYS::Path:
	    for( unsigned i_sz = 0; i_sz < sout.size(); i_sz++ )
		if( sout[i_sz] == ':' ) sout[i_sz] = '/';
	    break;
	case TSYS::PathEl: case TSYS::HttpURL: case TSYS::Custom:
	    while(true)
	    {
		n_pos = sout.find("%",n_pos);
		if( n_pos == string::npos ) break;
		if( sout[n_pos+1] == '%' ) sout.replace(n_pos,2,"%");
		else sout.replace(n_pos,3,string("")+(char)strtol(sout.substr(n_pos+1,2).c_str(),NULL,16));
		n_pos+=1;
	    }
	    break;
	case TSYS::base64:
	    sout = "";
	    for( int i_sz = 0; i_sz < in.size(); )
	    {
		if(in[i_sz] == '\n')	i_sz+=sizeof('\n');
		if((i_sz+3) < in.size())
		    if( in[i_sz+1] != '=' )
		    {
			char w_code1 = TSYS::getBase64Code(in[i_sz+1]);
			sout.push_back((TSYS::getBase64Code(in[i_sz])<<2)|(w_code1>>4));
			if( in[i_sz+2] != '=' )
			{
			    char w_code2 = TSYS::getBase64Code(in[i_sz+2]);
			    sout.push_back((w_code1<<4)|(w_code2>>2));
			    if( in[i_sz+3] != '=' )
				sout.push_back((w_code2<<6)|TSYS::getBase64Code(in[i_sz+3]));
			}
		    }
		i_sz+=4;
	    }
	    break;
    }
	    
    return sout;
}

long TSYS::HZ()
{
    return sysconf(_SC_CLK_TCK);
}

//================== Controll functions ========================
void TSYS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    char buf[STR_BUF_LEN];
    
    if( cmd==TCntrNode::Info )
    {
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    
	snprintf(buf,sizeof(buf),Mess->I18N("%s station: \"%s\""),PACKAGE_NAME,name().c_str());
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",buf);
	if(TUIS::presentIco(id()))
    	    ctrMkNode("img",opt,-1,a_path.c_str(),"/ico","",0444);
	ctrMkNode("area",opt,-1,a_path.c_str(),"/gen",Mess->I18N("Station"),0444);		
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/stat",Mess->I18N("Station"),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/prog",Mess->I18N("Programm"),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/ver",Mess->I18N("Version"),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/host",Mess->I18N("Host name"),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/user",Mess->I18N("System user"),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/sys",Mess->I18N("Operation system"),0444,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/frq",Mess->I18N("Frequency (MHZ)"),0444,0,0,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/clk_res",Mess->I18N("Realtime clock resolution (msec)"),0444,0,0,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/in_charset",Mess->I18N("Internal charset"),0440,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/config",Mess->I18N("Config file"),0440,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/workdir",Mess->I18N("Work directory"),0664,0,0,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/wrk_db",Mess->I18N("Work DB (module.bd)"),0660,0,db().at().subSecGrp(),1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/lang",Mess->I18N("Language"),0664,0,0,1,"tp","str");
 	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/m_lev",Mess->I18N("Least message level"),0664,0,0,4,"tp","dec","len","1","min","0","max","7");
 	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/log_sysl",Mess->I18N("Messages to syslog"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/log_stdo",Mess->I18N("Messages to stdout"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/log_stde",Mess->I18N("Messages to stderr"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/gen/log_arch",Mess->I18N("Messages to archive"),0664,0,0,1,"tp","bool");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/gen/load",Mess->I18N("Load system"),0440);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/gen/save",Mess->I18N("Save system"),0440);
	ctrMkNode("area",opt,-1,a_path.c_str(),"/subs",Mess->I18N("Subsystems"));
	ctrMkNode("list",opt,-1,a_path.c_str(),"/subs/br",Mess->I18N("Subsystems"),0555,0,0,2,"tp","br","br_pref","sub_");
	ctrMkNode("area",opt,-1,a_path.c_str(),"/hlp",Mess->I18N("Help"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/hlp/g_help",Mess->I18N("Options help"),0440,0,0,3,"tp","str","cols","90","rows","7");
    }
    else if( cmd==TCntrNode::Get )
    {
	utsname ubuf;
	uname(&ubuf);
	
	if( a_path == "/ico" )
	{
	    string itp;
            opt->text(TSYS::strCode(TUIS::getIco(id(),&itp),TSYS::base64));
            opt->attr("type",itp);	
	}	
	else if( a_path == "/gen/host" ) 		ctrSetS( opt, ubuf.nodename );
	else if( a_path == "/gen/sys" )  	ctrSetS( opt, string(ubuf.sysname)+"-"+ubuf.release );
	else if( a_path == "/gen/user" ) 	ctrSetS( opt, m_user );
	else if( a_path == "/gen/prog" ) 	ctrSetS( opt, PACKAGE_NAME );
	else if( a_path == "/gen/ver" )  	ctrSetS( opt, VERSION );
	else if( a_path == "/gen/stat" ) 	ctrSetS( opt, name() );
	else if( a_path == "/gen/frq" )		ctrSetR( opt, (float)sysClk()/1000000. );
	else if( a_path == "/gen/clk_res" )	
	{
	    struct timespec tmval;
	    clock_getres(CLOCK_REALTIME,&tmval);
	    ctrSetR( opt, (float)tmval.tv_nsec/1000000. );
	}
	else if( a_path == "/gen/in_charset" )	ctrSetS( opt, Mess->charset() );
	else if( a_path == "/gen/config" )	ctrSetS( opt, m_confFile );
	else if( a_path == "/gen/wrk_db" )    	ctrSetS( opt, mWorkDB );  
	else if( a_path == "/gen/workdir" )     ctrSetS( opt, getcwd(buf,sizeof(buf)) );
	else if( a_path == "/gen/lang" )   	ctrSetS( opt, Mess->lang() );	
	else if( a_path == "/gen/m_lev" )	ctrSetI( opt, Mess->messLevel() );
	else if( a_path == "/gen/log_sysl" )	ctrSetB( opt, Mess->logDirect()&0x01 );
	else if( a_path == "/gen/log_stdo" )	ctrSetB( opt, Mess->logDirect()&0x02 );
	else if( a_path == "/gen/log_stde" )	ctrSetB( opt, Mess->logDirect()&0x04 );
	else if( a_path == "/gen/log_arch" )   ctrSetB( opt, Mess->logDirect()&0x08 );
	else if( a_path.substr(0,8) == "/subs/br" )
	{
	    vector<string> lst;
	    list(lst);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
		ctrSetS( opt, at(lst[i_a]).at().subName(), lst[i_a].c_str() );         
	}
	else if( a_path == "/hlp/g_help" )	ctrSetS( opt, optDescr() );       
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/gen/wrk_db" )		mWorkDB = ctrGetS( opt ); 
	else if( a_path == "/gen/workdir" )	chdir(ctrGetS( opt ).c_str());
	else if( a_path == "/gen/lang" )        Mess->lang(ctrGetS( opt ) );
	else if( a_path == "/gen/m_lev" )       Mess->messLevel( ctrGetI( opt ) );
	else if( a_path == "/gen/log_sysl" )	Mess->logDirect( ctrGetB(opt)?Mess->logDirect()|0x01:Mess->logDirect()&(~0x01) );
	else if( a_path == "/gen/log_stdo" )	Mess->logDirect( ctrGetB(opt)?Mess->logDirect()|0x02:Mess->logDirect()&(~0x02) );
	else if( a_path == "/gen/log_stde" )	Mess->logDirect( ctrGetB(opt)?Mess->logDirect()|0x04:Mess->logDirect()&(~0x04) );
	else if( a_path == "/gen/log_arch" )	Mess->logDirect( ctrGetB(opt)?Mess->logDirect()|0x08:Mess->logDirect()&(~0x08) );
	else if( a_path == "/gen/load" ) 	load();
	else if( a_path == "/gen/save" ) 	save();
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }		
}


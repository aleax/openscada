
//OpenSCADA system file: tsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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


#include <syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <langinfo.h>
#include <zlib.h>

#include "../config.h"
#include "terror.h"
#include "tmess.h"
#include "tsys.h"

using namespace OSCADA;

//Continuously access variable
TMess	*OSCADA::Mess;
TSYS	*OSCADA::SYS;
bool TSYS::finalKill = false;

TSYS::TSYS( int argi, char ** argb, char **env ) :
    mConfFile("/etc/oscada.xml"), mId("EmptySt"), mName(_("Empty Station")), mIcoDir("./icons/"), mModDir("./"),
    mUser("root"),argc(argi), envp((const char **)env), argv((const char **)argb), mStopSignal(0), mMultCPU(false),
    mWorkDB(""), mSaveAtExit(false), mSavePeriod(0)
{
    finalKill = false;
    SYS = this;		//Init global access value
    mSubst = grpAdd("sub_",true);
    nodeEn();

    Mess = new TMess();

    if( getenv("USER") ) mUser = getenv("USER");

    //> Init system clock
    clkCalc();

    //> Multi CPU allow check
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1,&cpuset);
    mMultCPU = !pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    //> Set signal handlers
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    //signal(SIGCHLD,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
    //signal(SIGFPE,sighandler);
    //signal(SIGSEGV,sighandler);
    signal(SIGABRT,sighandler);
}

TSYS::~TSYS(  )
{
    finalKill = true;

    //Delete all nodes in order
    del("ModSched");
    del("UI");
    del("Special");
    del("Archive");
    del("DAQ");
    del("Protocol");
    del("Transport");
    del("Security");
    del("BD");

    delete Mess;
}

string TSYS::host( )
{
    utsname ubuf; uname(&ubuf);
    return ubuf.nodename;
}

string TSYS::workDir( )
{
    char buf[STR_BUF_LEN];
    return getcwd(buf,sizeof(buf));
}

void TSYS::setWorkDir( const string &wdir )
{
    if( chdir(wdir.c_str()) != 0 )
	mess_warning(nodePath().c_str(),_("Change work directory to '%s' error: %s. Perhaps current directory already set correct to '%s'."),
	    wdir.c_str(),strerror(errno),workDir().c_str());
    modif( );
}

string TSYS::int2str( int val, TSYS::IntView view )
{
    char buf[STR_BUF_LEN];
    if( view == TSYS::Dec )		snprintf(buf,sizeof(buf),"%d",val);
    else if( view == TSYS::Oct )	snprintf(buf,sizeof(buf),"%o",val);
    else if( view == TSYS::Hex )	snprintf(buf,sizeof(buf),"%x",val);

    return buf;
}

string TSYS::uint2str( unsigned val, IntView view )
{
    char buf[STR_BUF_LEN];
    if( view == TSYS::Dec )		snprintf(buf,sizeof(buf),"%u",val);
    else if( view == TSYS::Oct )	snprintf(buf,sizeof(buf),"%o",val);
    else if( view == TSYS::Hex )	snprintf(buf,sizeof(buf),"%x",val);

    return buf;
}

string TSYS::ll2str( long long val, IntView view )
{
    char buf[STR_BUF_LEN];
    if( view == TSYS::Dec )		snprintf(buf,sizeof(buf),"%lld",val);
    else if( view == TSYS::Oct )	snprintf(buf,sizeof(buf),"%llo",val);
    else if( view == TSYS::Hex )	snprintf(buf,sizeof(buf),"%llx",val);

    return buf;
}

string TSYS::real2str( double val, int prec, char tp )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),(tp=='g')?"%.*g":"%.*f",prec,val);

    return buf;
}

string TSYS::addr2str( void *addr )
{
    char buf[sizeof(void*)*2+3];
    snprintf(buf,sizeof(buf),"%p",addr);

    return buf;
}

void *TSYS::str2addr( const string &str )
{
    return (void *)strtoul(str.c_str(),NULL,16);
}

string TSYS::strNoSpace( const string &val )
{
    int beg = -1, end = -1;

    for( int i_s = 0; i_s < val.size(); i_s++ )
	if( val[i_s] != ' ' && val[i_s] != '\n' && val[i_s] != '\t' )
	{
	    if( beg < 0 ) beg = i_s;
	    end = i_s;
	}

    return (beg>=0) ? val.substr(beg,end-beg+1) : "";
}

string TSYS::strMess( const char *fmt, ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    return str;
}

string TSYS::optDescr( )
{
    utsname buf;
    uname(&buf);
    return TSYS::strMess(_(
	"***************************************************************************\n"
	"********** %s v%s (%s-%s). *********\n"
	"***************************************************************************\n\n"
	"===========================================================================\n"
	"========================= The general system options ======================\n"
	"===========================================================================\n"
	"-h, --help             Info message about system options.\n"
	"    --Config=<path>    Config file path.\n"
	"    --Station=<id>     Station identifier.\n"
	"    --demon            Start into demon mode.\n"
	"    --MessLev=<level>  Process messages <level> (0-7).\n"
	"    --log=<direct>     Direct messages to:\n"
	"                         <direct> & 1 - syslogd;\n"
	"                         <direct> & 2 - stdout;\n"
	"                         <direct> & 4 - stderr;\n"
	"                         <direct> & 8 - archive.\n"
	"----------- The config file station <%s> parameters -----------\n"
	"StName     <nm>	Station name.\n"
	"WorkDB     <Type.Name> Work DB (type and name).\n"
	"Workdir    <path>	Work directory.\n"
	"IcoDir     <path>	Icons directory.\n"
	"ModDir     <path>	Modules directory.\n"
	"MessLev    <level>     Messages <level> (0-7).\n"
	"LogTarget  <direction> Direct messages to:\n"
	"                           <direct> & 1 - syslogd;\n"
	"                           <direct> & 2 - stdout;\n"
	"                           <direct> & 4 - stderr;\n"
	"                           <direct> & 8 - archive.\n"
	"Lang2CodeBase <lang>	Base language for variable texts translation, two symbols code.\n"
	"SaveAtExit <true>      Save system at exit.\n"
	"SavePeriod <sec>	Save system period.\n\n"),
	PACKAGE_NAME,VERSION,buf.sysname,buf.release,nodePath().c_str());
}

bool TSYS::cfgFileLoad()
{
    bool cmd_help = false;

    //================ Load parameters from commandline =========================
    int next_opt;
    const char *short_opt="h";
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
		fprintf(stdout,"%s",optDescr().c_str());
		Mess->setMessLevel(7);
		cmd_help = true;
		break;
	    case 'f': mConfFile = optarg; break;
	    case 's': mId = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //Load config file
    int hd = open(mConfFile.c_str(),O_RDONLY);
    if( hd < 0 ) mess_err(nodePath().c_str(),_("Config file <%s> error: %s"),mConfFile.c_str(),strerror(errno));
    else
    {
	string s_buf;
	int cf_sz = lseek(hd,0,SEEK_END);
	if(cf_sz > 0)
	{
	    lseek(hd,0,SEEK_SET);
	    char *buf = (char *)malloc(cf_sz+1);
	    read(hd,buf,cf_sz);
	    buf[cf_sz] = 0;
	    s_buf = buf;
	    free(buf);
	}
	close(hd);

	try
	{
	    ResAlloc res(nodeRes(),true);
	    rootN.load(s_buf);
	    if( rootN.name() == "OpenSCADA" )
	    {
		XMLNode *stat_n = NULL;
		for( int i_st = rootN.childSize()-1; i_st >= 0; i_st-- )
		    if( rootN.childGet(i_st)->name() == "station" )
		    {
			stat_n = rootN.childGet(i_st);
			if( stat_n->attr("id") == mId ) break;
		    }
		if( stat_n && stat_n->attr("id") != mId )
		{
                    mess_warning(nodePath().c_str(),_("Station <%s> is not present in the config file. Use <%s> station config!"),
			mId.c_str(), stat_n->attr("id").c_str() );
		    mId	= stat_n->attr("id");
		}
		if( !stat_n )	rootN.clear();
	    } else rootN.clear();
	    if( !rootN.childSize() )
		mess_err(nodePath().c_str(),_("Config <%s> error!"),mConfFile.c_str());
	}
	catch( TError err ) { mess_err(nodePath().c_str(),_("Load config file error: %s"),err.mess.c_str() ); }
    }

    return cmd_help;
}

void TSYS::cfgPrmLoad()
{
    //System parameters
    mName = TBDS::genDBGet(nodePath()+"StName",name(),"root",TBDS::UseTranslate);
    mWorkDB = TBDS::genDBGet(nodePath()+"WorkDB","*.*","root",TBDS::OnlyCfg);
    setWorkDir( TBDS::genDBGet(nodePath()+"Workdir").c_str() );
    setIcoDir( TBDS::genDBGet(nodePath()+"IcoDir",icoDir()) );
    setModDir( TBDS::genDBGet(nodePath()+"ModDir",modDir()) );
    setSaveAtExit( atoi(TBDS::genDBGet(nodePath()+"SaveAtExit","0").c_str()) );
    setSavePeriod( atoi(TBDS::genDBGet(nodePath()+"SavePeriod","0").c_str()) );
}

void TSYS::load_()
{
    static bool first_load = true;

    bool cmd_help = cfgFileLoad();
    mess_info(nodePath().c_str(),_("Load!"));
    cfgPrmLoad();
    Mess->load();	//Messages load

    if( first_load )
    {
	//> Create subsystems
	add( new TBDS() );
	add( new TSecurity() );
	add( new TTransportS() );
	add( new TProtocolS() );
	add( new TDAQS() );
	add( new TArchiveS() );
	add( new TSpecialS() );
	add( new TUIS() );
	add( new TModSchedul() );

	//> Load modules
	modSchedul().at().load();
	if( !modSchedul().at().loadLibS() )
	{
	    mess_err(nodePath().c_str(),_("No one module is loaded. Your configuration broken!"));
	    stop();
	}

	//> First DB subsystem load
	db().at().load();
	if( !cmd_help ) modSchedul().at().modifG();	// For try reload from DB

	//> Second load for load from generic DB
	Mess->load();
	cfgPrmLoad();
    }

    //> Direct load subsystems and modules
    vector<string> lst;
    list(lst);
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	try { at(lst[i_a]).at().load(); }
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Error load subsystem <%s>."),lst[i_a].c_str());
	}

    if( cmd_help ) stop();
    first_load = false;
}

void TSYS::save_( )
{
    char buf[STR_BUF_LEN];

    mess_info(nodePath().c_str(),_("Save!"));

    //> System parameters
    getcwd(buf,sizeof(buf));
    TBDS::genDBSet(nodePath()+"StName",mName,"root",TBDS::UseTranslate);
    TBDS::genDBSet(nodePath()+"Workdir",buf);
    TBDS::genDBSet(nodePath()+"IcoDir",icoDir());
    TBDS::genDBSet(nodePath()+"ModDir",modDir());
    TBDS::genDBSet(nodePath()+"SaveAtExit",TSYS::int2str(saveAtExit()));
    TBDS::genDBSet(nodePath()+"SavePeriod",TSYS::int2str(savePeriod()));

    Mess->save();	//Messages load
}

int TSYS::start(  )
{
    vector<string> lst;
    list(lst);

    mess_info(nodePath().c_str(),_("Start!"));
    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	try { at(lst[i_a]).at().subStart(); }
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Error start subsystem <%s>."),lst[i_a].c_str());
	}

    cfgFileScan( true );

    mess_info(nodePath().c_str(),_("Final started!"));

    unsigned int i_cnt = 1;
    while( !mStopSignal )
    {
	//> CPU frequency calc
	if( !(i_cnt%(10*1000/STD_WAIT_DELAY)) )	clkCalc( );
	//> Config file change periodic check
	if( !(i_cnt%(10*1000/STD_WAIT_DELAY)) )	cfgFileScan( );
	//> Periodic shared libraries checking
	if( modSchedul( ).at().chkPer() && !(i_cnt%(modSchedul( ).at().chkPer()*1000/STD_WAIT_DELAY)) )
	    modSchedul( ).at().libLoad(modDir(),true);
	//> Old tables closing
	if( !(i_cnt%(10*1000/STD_WAIT_DELAY)) )	db().at().closeOldTables();
	//> Periodic changes saving to DB
	if( savePeriod() && !(i_cnt%(savePeriod()*1000/STD_WAIT_DELAY)) )
	    save();

	usleep( STD_WAIT_DELAY*1000 );
	i_cnt++;
    }

    mess_info(nodePath().c_str(),_("Stop!"));
    if( saveAtExit() || savePeriod() )	save();
    for( int i_a=lst.size()-1; i_a >= 0; i_a-- )
	try { at(lst[i_a]).at().subStop(); }
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Error stop subsystem <%s>."),lst[i_a].c_str());
	}

    return mStopSignal;
}

void TSYS::stop( )
{
    mStopSignal = SIGUSR1;
}

bool TSYS::chkSelDB( const string& wDB )
{
    if( selDB().empty() ) return true;
    if( selDB() == "<cfg>" && (wDB == "<cfg>" || TBDS::realDBName(wDB) == workDB()) ) return true;
    if( SYS->selDB( ) == TBDS::realDBName(wDB) ) return true;
    return false;
}

void TSYS::sighandler( int signal )
{
    switch(signal)
    {
	case SIGINT:
	    SYS->mStopSignal=signal;
	    break;
	case SIGTERM:
	    mess_warning(SYS->nodePath().c_str(),_("The Terminate signal is received.. Server is being stoped!"));
	    SYS->mStopSignal=signal;
	    break;
	case SIGFPE:
	    mess_warning(SYS->nodePath().c_str(),_("Floating point exception is catched!"));
            exit(1);
	    break;
	case SIGCHLD:
	{
	    int status;
	    pid_t pid = wait(&status);
	    if(!WIFEXITED(status) && pid > 0)
		mess_info(SYS->nodePath().c_str(),_("Free child process %d!"),pid);
	    break;
	}
	case SIGPIPE:
	    //mess_warning(SYS->nodePath().c_str(),_("Broken PIPE signal!"));
	    break;
	case SIGSEGV:
	    mess_emerg(SYS->nodePath().c_str(),_("Segmentation fault signal!"));
	    break;
	case SIGABRT:
	    mess_emerg(SYS->nodePath().c_str(),_("OpenSCADA is aborted!"));
	    break;
	case SIGALRM:	break;
	default:
	    mess_warning(SYS->nodePath().c_str(),_("Unknown signal %d!"),signal);
    }
}

void TSYS::cfgFileScan( bool first )
{
    struct stat f_stat;

    if( stat(cfgFile().c_str(),&f_stat) != 0 ) return;
    bool up = false;
    if( rootCfgFl != cfgFile() || rootFlTm != f_stat.st_mtime ) up = true;
    rootCfgFl = cfgFile();
    rootFlTm = f_stat.st_mtime;

    if( up && !first )
    {
	modifG();
	setSelDB("<cfg>");
	load();
	setSelDB("");
    }
}

long long TSYS::curTime()
{
    timeval cur_tm;
    gettimeofday(&cur_tm,NULL);
    return (long long)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;
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
	    mess_crit(loc.c_str(),_("Timeouted !!!"));
	    return true;
	}
	//Make messages
	if( c_tm > t_tm+1 )  //1sec
	{
	    t_tm = c_tm;
	    mess_info(loc.c_str(),_("Wait event..."));
	}
	usleep(STD_WAIT_DELAY*1000);
    }
    return false;
}

string TSYS::strSepParse( const string &path, int level, char sep, int *off )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    int t_dir;

    if( an_dir >= path.size() ) return "";
    while(true)
    {
	t_dir = path.find(sep,an_dir);
	if( t_dir == string::npos )
	{
	    if( off ) *off = path.size();
	    return (t_lev == level) ? path.substr(an_dir) : "";
	}
	else if( t_lev == level )
	{
	    if( off ) *off = t_dir+1;
	    return path.substr(an_dir,t_dir-an_dir);
	}
	an_dir = t_dir+1;
	t_lev++;
    }
    return "";
}

string TSYS::strParse( const string &path, int level, const string &sep, int *off, bool mergeSepSymb )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    int t_dir;

    if( an_dir >= path.size() || sep.empty() ) return "";
    while(true)
    {
	t_dir = path.find(sep,an_dir);
	if( t_dir == string::npos )
	{
	    if( off ) *off = path.size();
	    return (t_lev == level) ? path.substr(an_dir) : "";
	}
	else if( t_lev == level )
	{
	    if( off ) *off = t_dir+sep.size();
	    return path.substr(an_dir,t_dir-an_dir);
	}
	if( mergeSepSymb && sep.size() == 1 )
	    for( an_dir = t_dir; an_dir < path.size() && path[an_dir] == sep[0]; ) an_dir++;
	else an_dir = t_dir+sep.size();
	t_lev++;
    }
    return "";
}

string TSYS::pathLev( const string &path, int level, bool encode, int *off )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    int t_dir;

    //- First separators pass -
    while( an_dir<path.size() && path[an_dir]=='/' ) an_dir++;
    if( an_dir >= path.size() ) return "";
    //- Path level process -
    while(true)
    {
	t_dir = path.find("/",an_dir);
	if( t_dir == string::npos )
	{
	    if( off ) *off = path.size();
	    return (t_lev == level) ? ( encode ? TSYS::strDecode(path.substr(an_dir),TSYS::PathEl) : path.substr(an_dir) ) : "";
	}
	else if( t_lev == level )
	{
	    if( off ) *off = t_dir;
	    return encode ? TSYS::strDecode(path.substr(an_dir,t_dir-an_dir),TSYS::PathEl) : path.substr(an_dir,t_dir-an_dir);
	}
	an_dir = t_dir;
	t_lev++;
	while( an_dir<path.size() && path[an_dir]=='/' ) an_dir++;
    }
}

string TSYS::path2sepstr( const string &path, char sep )
{
    string rez, curv;
    int off = 0;
    while( !(curv=TSYS::pathLev(path,0,false,&off)).empty() )
	rez+=curv+sep;
    if(!rez.empty())	rez.resize(rez.size()-1);

    return rez;
}

string TSYS::sepstr2path( const string &str, char sep )
{
    string rez, curv;
    int off = 0;
    while( !(curv=TSYS::strSepParse(str,0,sep,&off)).empty() )
	rez+="/"+curv;

    return rez;
}

string TSYS::strEncode( const string &in, TSYS::Code tp, const string &symb )
{
    int i_sz;
    string sout;

    switch(tp)
    {
	case TSYS::PathEl:
	    sout = in;
	    for( i_sz = 0; i_sz < sout.size(); i_sz++ )
		switch( sout[i_sz] )
		{
		    case '/': sout.replace(i_sz,1,"%2f"); i_sz+=2; break;
		    case '%': sout.replace(i_sz,1,"%25"); i_sz+=2; break;
		}
	    break;
	case TSYS::HttpURL:
	    sout = in;
	    for( i_sz = 0; i_sz < sout.size(); i_sz++ )
		switch( sout[i_sz] )
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
	    sout.reserve(in.size()+10);
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		switch( in[i_sz] )
		{
		    case '>':	sout+="&gt;";	break;
		    case '<':	sout+="&lt;";	break;
		    case '"':	sout+="&quot;";	break;
		    case '&':	sout+="&amp;";	break;
		    case '\'':	sout+="&apos;";	break;
		    default:	sout+=in[i_sz];
		}
	    break;
	case TSYS::JavaSc:
	    sout.reserve(in.size()+10);
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		switch( in[i_sz] )
		{
		    case '\n':	sout+="\\n";	break;
		    default:	sout+=in[i_sz];
		}
	    break;
	case TSYS::SQL:
	    sout.reserve(in.size()+10);
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		switch( in[i_sz] )
		{
		    case '\'':	sout+="\\'";	break;
		    case '\"':	sout+="\\\"";	break;
		    case '`':	sout+="\\`";	break;
		    case '\\':	sout+="\\\\";	break;
		    default:	sout+=in[i_sz];
		}
	    break;
	case TSYS::Custom:
	    sout.reserve(in.size()+10);
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
	    {
		int i_smb;
		for( i_smb = 0; i_smb < symb.size(); i_smb++ )
		    if( in[i_sz] == symb[i_smb] )
		    {
			char buf[4];
			sprintf(buf,"%%%02X",(unsigned char)in[i_sz]);
			sout += buf;
			break;
		    }
		if( i_smb >= symb.size() ) sout+=in[i_sz];
	    }
	    break;
	case TSYS::base64:
	{
	    sout.reserve(in.size()+in.size()/4+in.size()/57+10);
	    const char *base64alph = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	    for( i_sz = 0; i_sz < in.size(); i_sz+=3 )
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
	case TSYS::FormatPrint:
	    sout = in;
	    for( i_sz = 0; i_sz < sout.size(); i_sz++ )
		if( sout[i_sz] == '%' ) { sout.replace(i_sz,1,"%%"); i_sz++; }
	    break;
	case TSYS::oscdID:
	    sout.reserve(in.size());
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		switch( in[i_sz] )
		{
		    case ' ': case '/': case '\\': case '&': case '(':
		    case ')': case '[': case ']': case '!': case '~':
		    case '`': case '@': case '%': case '^': case '-':
		    case '+': case '=': case '*': case '{': case '}':
		    case ':': case ';': case '"': case '\'': case '<':
		    case '>': case '?': case '.': case ',':
			sout+="_";	break;
		    default:	sout+=in[i_sz];
		}
	    break;
	case TSYS::Bin:
	{
	    string svl, evl;
	    sout.reserve(in.size());
	    for( int off = 0; (svl=TSYS::strSepParse(in,0,'\n',&off)).size(); )
		for( int offE = 0; (evl=TSYS::strSepParse(svl,0,' ',&offE)).size(); )
		    sout+=(char)strtol(evl.c_str(),NULL,16);
	    break;
	}
	case TSYS::Reverse:
	    for( i_sz = in.size()-1; i_sz >= 0; i_sz-- ) sout += in[i_sz];
	    break;
	case TSYS::ShieldSimb:
	    sout.reserve(in.size());
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		if( in[i_sz] == '\\' && i_sz < (in.size()-1) )
		{
		    switch( in[i_sz+1] )
		    {
			case 'a':	sout += '\a';	break;
			case 'b':	sout += '\b';	break;
			case 'f':	sout += '\f';	break;
			case 'n':	sout += '\n';	break;
			case 'r':	sout += '\r';	break;
			case 't':	sout += '\t';	break;
			case 'v':	sout += '\v';	break;
			default:	sout += in[i_sz+1];
		    }
		    i_sz++;
		}else sout += in[i_sz];
	    break;
    }
    return sout;
}

unsigned char TSYS::getBase64Code(unsigned char asymb) 
{
    switch( asymb )
    {
	case 'A' ... 'Z':	return asymb-(unsigned char)'A';
	case 'a' ... 'z':	return 26+asymb-(unsigned char)'a';
	case '0' ... '9':	return 52+asymb-(unsigned char)'0';
	case '+':		return 62;
	case '/':		return 63;
    }
}

string TSYS::strDecode( const string &in, TSYS::Code tp )
{
    int i_sz;
    string sout;

    switch(tp)
    {
	case TSYS::PathEl: case TSYS::HttpURL: case TSYS::Custom:
	    sout.reserve(in.size());
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		switch( in[i_sz] )
		{
		    case '%':
			if( i_sz+2 < in.size() )
			{
			    sout+=(char)strtol(in.substr(i_sz+1,2).c_str(),NULL,16);
			    i_sz+=2;
			}else sout+=in[i_sz];
			break;
		    default: sout+=in[i_sz];
		}
	    break;
	case TSYS::base64:
	    sout.reserve(in.size());
	    for( i_sz = 0; i_sz < in.size(); )
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
	case TSYS::Bin:
	    sout.reserve(in.size());
	    for( i_sz = 0; i_sz < in.size(); i_sz++ )
		sout += TSYS::strMess(((i_sz+1)%16)?"%0.2x ":"%0.2x\n",(unsigned char)in[i_sz]);
	    break;
    }

    return sout;
}

string TSYS::strCompr( const string &in, int lev )
{
    z_stream strm;

    if( in.empty() )	return "";

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if( deflateInit(&strm,lev) != Z_OK ) return "";

    uLongf comprLen = deflateBound(&strm,in.size());
    char out[comprLen];

    strm.next_in = (Bytef*)in.data();
    strm.avail_in = (uInt)in.size();
    strm.next_out = (Bytef*)out;
    strm.avail_out = comprLen;

    if( deflate(&strm, Z_FINISH) != Z_STREAM_END )
    {
	deflateEnd(&strm);
	return "";
    }

    comprLen = strm.total_out;

    deflateEnd(&strm);

    return string(out,comprLen);
}

string TSYS::strUncompr( const string &in )
{
    int ret;
    z_stream strm;
    unsigned char out[STR_BUF_LEN];
    string rez;

    if( in.empty() )	return "";

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if( inflateInit(&strm) != Z_OK )	return "";

    strm.avail_in = in.size();
    strm.next_in = (Bytef*)in.data();
    do
    {
	strm.avail_out = sizeof(out);
	strm.next_out = out;
	ret=inflate(&strm,Z_NO_FLUSH);
	if( ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR )
	    break;
	rez.append((char*)out,sizeof(out)-strm.avail_out);
    } while( strm.avail_out == 0 );

    inflateEnd(&strm);

    if( ret != Z_STREAM_END )	return "";

    return rez;
}

long TSYS::HZ()
{
    return sysconf(_SC_CLK_TCK);
}

bool TSYS::cntrEmpty( )
{
    ResAlloc res( nodeRes(), false );
    return mCntrs.empty();
}

double TSYS::cntrGet( const string &id )
{
    ResAlloc res( nodeRes(), false );
    map<string,double>::iterator icnt = mCntrs.find(id);
    if( icnt == mCntrs.end() )	return 0;
    return icnt->second;
}

void TSYS::cntrSet( const string &id, double vl )
{
    ResAlloc res( nodeRes(), true );
    mCntrs[id] = vl;
}

void TSYS::taskCreate( const string &path, int priority, void *(*start_routine)(void *), void *arg, bool *startCntr, int wtm, pthread_attr_t *pAttr )
{
    int detachStat = 0;
    pthread_t procPthr;
    pthread_attr_t locPAttr, *pthr_attr;

    ResAlloc res(taskRes, false);
    if(mTasks.find(path) != mTasks.end() && startCntr && *startCntr)
	throw TError(nodePath().c_str(),_("Task '%s' is already created!"),path.c_str());
    res.release();

    if(pAttr) pthr_attr = pAttr;
    else
    {
	pthr_attr = &locPAttr;
	pthread_attr_init(pthr_attr);
    }
    pthread_attr_setinheritsched(pthr_attr, PTHREAD_EXPLICIT_SCHED);
    struct sched_param prior;
    prior.sched_priority = 0;

    int policy = SCHED_OTHER;
    if(priority < 0)	policy = SCHED_BATCH;
    else if(priority > 0 /*&& SYS->user() == "root"*/)	policy = SCHED_RR;
    pthread_attr_setschedpolicy( pthr_attr, policy );
    prior.sched_priority = vmax(sched_get_priority_min(policy),vmin(sched_get_priority_max(policy),priority));
    pthread_attr_setschedparam(pthr_attr,&prior);

    STask htsk;
    htsk.path = path;
    htsk.task = start_routine;
    htsk.taskArg = arg;
    pthread_attr_getdetachstate(pthr_attr,&detachStat);
    if(detachStat == PTHREAD_CREATE_DETACHED) htsk.flgs |= STask::Detached;
    int rez = pthread_create( &procPthr, pthr_attr, taskWrap, &htsk );
    if(rez == EPERM)
    {
	mess_warning(nodePath().c_str(),_("No permition for create realtime policy. Default thread is created!"));
	policy = SCHED_OTHER;
	pthread_attr_setschedpolicy( pthr_attr, policy );
	prior.sched_priority = 0;
	pthread_attr_setschedparam(pthr_attr,&prior);
	rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
    }
    if(!pAttr) pthread_attr_destroy( pthr_attr );

    if(rez) throw TError(nodePath().c_str(), _("Task creation error %d."), rez);

    if(startCntr && TSYS::eventWait(*startCntr, true, nodePath()+": "+path+": start", wtm))
	throw TError(nodePath().c_str(), _("Task '%s' is not started!"), path.c_str());

    //> Wait for thread structure initialization finish
    while(!htsk.thr) pthread_yield();

    res.request(true); mTasks[path] = htsk; res.release();
}

void TSYS::taskDestroy( const string &path, bool *startCntr, bool *endrunCntr, int wtm )
{
    ResAlloc res(taskRes,false);
    map<string,STask>::iterator it = mTasks.find(path);
    if( it == mTasks.end() )	return;//throw TError(nodePath().c_str(),_("Task '%s' is not present!"),path.c_str());
    pthread_t thr = it->second.thr;
    res.release();

    if( endrunCntr ) *endrunCntr = true;
    pthread_kill( thr, SIGALRM );

    if( startCntr && TSYS::eventWait( *startCntr, false, nodePath()+": "+path+": stop", wtm ) )
	throw TError(nodePath().c_str(),_("Task '%s' is not stopped!"),path.c_str());

    pthread_join( thr, NULL );

    res.request(true);
    mTasks.erase( it );
}

void *TSYS::taskWrap( void *stas )
{
    //> Get temporary task structure
    STask *tsk = (STask *)stas;

    //> Store call parameters
    void *(*wTask) (void *) = tsk->task;
    void *wTaskArg = tsk->taskArg;

    //> Get current policy and priority
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    tsk->policy = policy;
    tsk->prior = param.sched_priority;

    //> Load and init CPU set
    if(SYS->multCPU() && !(tsk->flgs & STask::Detached))
    {
	tsk->cpuSet = TBDS::genDBGet(SYS->nodePath()+"CpuSet:"+tsk->path);
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	string sval;
	bool cpuSetOK = false;
	for(int off = 0; (sval=TSYS::strParse(tsk->cpuSet,0,":",&off)).size(); cpuSetOK = true)
	    CPU_SET(atoi(sval.c_str()),&cpuset);
	if(cpuSetOK) pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    }
    else if(SYS->multCPU() && (tsk->flgs & STask::Detached)) tsk->cpuSet = "NA";

    //> Final set for init finish indicate
    tsk->tid = syscall(SYS_gettid);
    tsk->thr = pthread_self();

    //> Call work task
    try{ return wTask(wTaskArg); }
    catch(TError err)
    {
	mess_err(err.cat.c_str(),err.mess.c_str());
	mess_err(SYS->nodePath().c_str(),_("Task %u unexpected terminated by exeption."),tsk->thr);
	return NULL;
    }
}

void TSYS::taskSleep( long long per, time_t cron )
{
    struct timespec sp_tm;

    if( !cron )
    {
	if( !per ) per = 1000000000;
	clock_gettime(CLOCK_REALTIME,&sp_tm);
	long long pnt_tm = ( ((long long)sp_tm.tv_sec*1000000000+sp_tm.tv_nsec)/per + 1)*per;
	do
	{
	    sp_tm.tv_sec = pnt_tm/1000000000; sp_tm.tv_nsec = pnt_tm%1000000000;
	    if( clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&sp_tm,NULL) )	return;
	    clock_gettime(CLOCK_REALTIME,&sp_tm);
	}while( ((long long)sp_tm.tv_sec*1000000000+sp_tm.tv_nsec) < pnt_tm );
    }
    else
    {
	sp_tm.tv_sec = cron; sp_tm.tv_nsec = 0;
	clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&sp_tm,NULL);
    }
}

time_t TSYS::cron( const string &vl, time_t base )
{
    bool isReset = false;
    string cronEl, tEl;
    int vbeg, vend, vstep, vm;

    time_t ctm = base?base:time(NULL);
    struct tm ttm;
    localtime_r(&ctm,&ttm);
    ttm.tm_sec = 0;

reload:
    bool isReload = false;
    //> Minutes check
    cronEl = TSYS::strSepParse(vl,0,' ');
    vm = 200;
    for( int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); )
    {
	vbeg = vend = -1; vstep = 0;
	sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	if( vbeg < 0 ) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=0; vend=59; }
	if( vend < 0 ) vm = vmin(vm,vbeg+((ttm.tm_min>=vbeg)?60:0));
	else if( (vbeg=vmax(0,vbeg)) < (vend=vmin(59,vend)) )
	{
	    if( ttm.tm_min < vbeg ) vm = vmin(vm,vbeg);
	    else if( (vstep>1 && ttm.tm_min >= (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_min >= vend) )
		vm = vmin(vm,vbeg+60);
	    else if( vstep>1 ) vm = vmin( vm, vbeg + vstep*( ((ttm.tm_min+1)-vbeg)/vstep + ((((ttm.tm_min+1)-vbeg)%vstep)?1:0) ) );
	    else vm = vmin( vm, ttm.tm_min+1 );
	}
	if( vm == ttm.tm_min+1 ) break;
    }
    ttm.tm_min = vm;
    mktime(&ttm);

    //> Hours check
    cronEl = TSYS::strSepParse(vl,1,' ');
    vm = 200;
    for( int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); )
    {
	vbeg = vend = -1; vstep = 0;
	sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	if( vbeg < 0 ) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=0; vend=23; }
	if( vend < 0 ) vm = vmin(vm,vbeg+((ttm.tm_hour>vbeg)?24:0));
	else if( (vbeg=vmax(0,vbeg)) < (vend=vmin(23,vend)) )
	{
	    if( ttm.tm_hour < vbeg ) vm = vmin(vm,vbeg);
	    else if( (vstep>1 && ttm.tm_hour > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_hour > vend) )
		vm = vmin(vm,vbeg+24);
	    else if( vstep>1 ) vm = vmin( vm, vbeg + vstep*( (ttm.tm_hour-vbeg)/vstep + (((ttm.tm_hour-vbeg)%vstep)?1:0) ) );
	    else vm = vmin( vm, ttm.tm_hour );
	}
	if( vm == ttm.tm_hour ) break;
    }
    isReload = (ttm.tm_hour!=vm);
    ttm.tm_hour = vm;
    mktime(&ttm);
    if( isReload )	{ ttm.tm_min = -1; goto reload; }

    //> Day check
    cronEl = TSYS::strSepParse(vl,2,' ');
    string cronElw = TSYS::strSepParse(vl,4,' ');
    vm = 200;
    if( cronEl != "*" )
	for( int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); )
	{
	    vbeg = vend = -1; vstep = 0;
	    sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	    if( vbeg < 0 ) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=1; vend=31; }
	    if( vend < 0 ) vm = vmin(vm,vbeg+((ttm.tm_mday>vbeg)?31:0));
	    else if( (vbeg=vmax(1,vbeg)) < (vend=vmin(31,vend)) )
	    {
		if( ttm.tm_mday < vbeg ) vm = vmin(vm,vbeg);
		else if( (vstep>1 && ttm.tm_mday > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_mday > vend) )
		    vm = vmin(vm,vbeg+31);
		else if( vstep>1 ) vm = vmin( vm, vbeg + vstep*( (ttm.tm_mday-vbeg)/vstep + (((ttm.tm_mday-vbeg)%vstep)?1:0) ) );
		else vm = vmin( vm, ttm.tm_mday );
	    }
	    if( vm == ttm.tm_mday ) break;
        }
    if( cronEl == "*" || (cronElw != "*" && !cronElw.empty()) )
	for( int eoff = 0; (tEl=TSYS::strSepParse(cronElw,0,',',&eoff)).size(); )
	{
	    vbeg = vend = -1; vstep = 0;
	    sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	    if( vbeg < 0 ) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=0; vend=6; }
	    if( vend < 0 ) vm = vmin( vm, ttm.tm_mday - ttm.tm_wday + vbeg+((ttm.tm_wday>vbeg)?7:0) );
	    else if( (vbeg=vmax(1,vbeg)) < (vend=vmin(7,vend)) )
	    {
		if( ttm.tm_wday < vbeg ) vm = vmin( vm, ttm.tm_mday - ttm.tm_wday + vbeg );
		else if( (vstep>1 && ttm.tm_wday > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_wday > vend) )
		    vm = vmin( vm, ttm.tm_mday - ttm.tm_wday + vbeg+7 );
		else if( vstep>1 ) vm = vmin( vm, ttm.tm_mday - ttm.tm_wday + vbeg + vstep*( (ttm.tm_wday-vbeg)/vstep + (((ttm.tm_wday-vbeg)%vstep)?1:0) ) );
		else vm = vmin( vm, ttm.tm_mday );
	    }
	    if( vm == ttm.tm_mday ) break;
	}
    isReload = (ttm.tm_mday!=vm);
    if( vm <= 31 ) ttm.tm_mday = vm;
    { ttm.tm_mday = vm-31; ttm.tm_mon++; }
    mktime(&ttm);
    if( isReload )	{ ttm.tm_min = -1; ttm.tm_hour = 0; goto reload; }

    //> Month check
    cronEl = TSYS::strSepParse(vl,3,' ');
    vm = 200;
    for( int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); )
    {
	vbeg = vend = -1; vstep = 0;
	sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	if( vbeg < 0 ) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=1; vend=12; }
	if( vend < 0 ) vm = vmin(vm,vbeg+(((ttm.tm_mon+1)>vbeg)?12:0));
	else if( (vbeg=vmax(1,vbeg)) < (vend=vmin(12,vend)) )
	{
	    if( (ttm.tm_mon+1) < vbeg ) vm = vmin(vm,vbeg);
	    else if( (vstep>1 && (ttm.tm_mon+1) > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && (ttm.tm_mon+1) > vend) )
		vm = vmin(vm,vbeg+12);
	    else if( vstep>1 ) vm = vmin( vm, vbeg + vstep*( ((ttm.tm_mon+1)-vbeg)/vstep + ((((ttm.tm_mon+1)-vbeg)%vstep)?1:0) ) );
	    else vm = vmin( vm, ttm.tm_mon+1 );
	}
	if( vm == (ttm.tm_mon+1) ) break;
    }
    isReload = (ttm.tm_mon!=(vm-1));
    ttm.tm_mon = vm-1;
    mktime(&ttm);
    if( isReload )	{ ttm.tm_min = -1; ttm.tm_hour = 0; ttm.tm_mday = 1; goto reload; }

    return mktime(&ttm);
}

TVariant TSYS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // int message(string cat, int level, string mess) - formation of the system message <mess> with the category <cat>, level <level>
    //  cat - message category
    //  level - message level
    //  mess - message text
    if( iid == "message" && prms.size() >= 3 )	{ message( prms[0].getS().c_str(), (TMess::Type)prms[1].getI(), "%s", prms[2].getS().c_str() ); return 0; }
    // int messDebug(string cat, string mess) - formation of the system message <mess> with the category <cat> and the appropriate level
    //  cat - message category
    //  mess - message text
    if( iid == "messDebug" && prms.size() >= 2 ){ mess_debug( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messInfo" && prms.size() >= 2 )	{ mess_info( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messNote" && prms.size() >= 2 )	{ mess_note( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messWarning" && prms.size() >= 2 )	{ mess_warning( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messErr" && prms.size() >= 2 )	{ mess_err( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messCrit" && prms.size() >= 2 )	{ mess_crit( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messAlert" && prms.size() >= 2 ){ mess_alert( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    if( iid == "messEmerg" && prms.size() >= 2 ){ mess_emerg( prms[0].getS().c_str(), "%s", prms[1].getS().c_str() ); return 0; }
    // string system(string cmd, bool noPipe = false) - calls the console commands <cmd> of OS returning the result by the channel
    //  cmd - command text
    //  noPipe - pipe result disable for background call
    if( iid == "system" && prms.size() >= 1 )
    {
	if( prms.size() >= 2 && prms[1].getB() ) return system( prms[0].getS().c_str() );
	FILE *fp = popen(prms[0].getS().c_str(),"r");
	if( !fp ) return string("");

	char buf[STR_BUF_LEN];
	string rez;
	for( int r_cnt = 0; (r_cnt=fread(buf,1,sizeof(buf),fp)); )
	    rez.append(buf,r_cnt);

	pclose(fp);
	return rez;
    }
    // XMLNodeObj XMLNode(string name = "") - creation of the XML node object with the name <name>
    //  name - XML node name
    if( iid == "XMLNode" ) return new XMLNodeObj( (prms.size()>=1) ? prms[0].getS() : "" );
    // string cntrReq(XMLNodeObj req, string stat = "") - request of the control interface to the system via XML
    //  req - request's XML node
    //  stat - remote OpenSCADA-station for request
    if( iid == "cntrReq" && prms.size() >= 1 )
    {
	XMLNode req;
	if( !dynamic_cast<XMLNodeObj*>(prms[0].getO()) ) return string(_("1:Request is not object!"));
	((XMLNodeObj*)prms[0].getO())->toXMLNode(req);
	string path = req.attr("path");
	if( prms.size() < 2 || prms[1].getS().empty() )
	{
	    req.setAttr("user",user);
	    cntrCmd(&req);
	}
	else
	{
	    req.setAttr("path","/"+prms[1].getS()+path);
	    transport().at().cntrIfCmd(req,"cntrReq");
	    req.setAttr("path",path);
	}
	((XMLNodeObj*)prms[0].getO())->fromXMLNode(req);
	return string("0");
    }
    // int time(int usec) - returns the absolute time in seconds from the epoch of 1/1/1970 and in microseconds, if <usec> is specified
    //  usec - microseconds of time
    if( iid == "time" )
    {
	if( prms.empty() ) return (int)time(NULL);
	long long tm = curTime();
	prms[0].setI(tm%1000000); prms[0].setModify();
	return (int)(tm/1000000);
    }
    // int localtime(int fullsec, int sec, int min, int hour, int mday, int month, int year, int wday, int yday, int isdst) 
    //      - returns the full date based on the absolute time in seconds <fullsec> from the epoch 1.1.1970
    //  fullsec - source time ins seconds from the epoch 1.1.1970
    //  sec - seconds
    //  min - minutes
    //  hour - hours
    //  mday - days of the month
    //  month - months
    //  year - years
    //  wday - days in the week
    //  yday - days in the year
    //  isdst - sign of summer time
    if( iid == "localtime" && prms.size() >= 2 )
    {
	time_t tm_t = prms[0].getI();
	struct tm tm_tm;
	localtime_r(&tm_t,&tm_tm);

	prms[1].setI(tm_tm.tm_sec); prms[1].setModify();
	if( prms.size() >= 3 ) { prms[2].setI(tm_tm.tm_min); prms[2].setModify(); }
	if( prms.size() >= 4 ) { prms[3].setI(tm_tm.tm_hour); prms[3].setModify(); }
	if( prms.size() >= 5 ) { prms[4].setI(tm_tm.tm_mday); prms[4].setModify(); }
	if( prms.size() >= 6 ) { prms[5].setI(tm_tm.tm_mon); prms[5].setModify(); }
	if( prms.size() >= 7 ) { prms[6].setI(1900+tm_tm.tm_year); prms[6].setModify(); }
	if( prms.size() >= 8 ) { prms[7].setI(tm_tm.tm_wday); prms[7].setModify(); }
	if( prms.size() >= 9 ) { prms[8].setI(tm_tm.tm_yday); prms[8].setModify(); }
	if( prms.size() >= 10 ) { prms[9].setI(tm_tm.tm_isdst); prms[9].setModify(); }
	return 0;
    }
    // string strftime(int sec, string form = "%Y-%m-%d %H:%M:%S") - converts an absolute time <sec> to the string of the desired format <form>
    //  sec - time ins seconds from the epoch 1.1.1970
    //  form - result string format
    if( iid == "strftime" && !prms.empty() )
    {
	time_t tm_t = prms[0].getI();
	struct tm tm_tm;
	localtime_r(&tm_t,&tm_tm);
	char buf[1000];
	int rez = strftime( buf, sizeof(buf), (prms.size()>=2) ? prms[1].getS().c_str() : "%Y-%m-%d %H:%M:%S", &tm_tm );
	return (rez>0) ? string(buf,rez) : "";
    }
    // int strptime(int str, string form = "%Y-%m-%d %H:%M:%S") - returns the time in seconds from the epoch of 1/1/1970,
    //      based on the string record of time <str>, in accordance with the specified template <form>
    //  str - source time in string
    //  form - string's time template in format POSIX-function "strptime"
    if( iid == "strptime" && !prms.empty() )
    {
	struct tm stm;
	stm.tm_isdst = -1;
	strptime( prms[0].getS().c_str(), (prms.size()>=2) ? prms[1].getS().c_str() : "%Y-%m-%d %H:%M:%S", &stm );
	return (int)mktime(&stm);
    }
    // int cron(string cronreq, int base = 0) - returns the time, planned in the format of the standard Cron <cronreq>,
    //      beginning from basic time <base> or from the current, if the basic is not specified
    //  cronreq - shedule in standard Cron format
    //  base - base time
    if( iid == "cron" && !prms.empty() )
	return (int)cron( prms[0].getS(), (prms.size()>=2) ? prms[1].getI() : 0 );
    // string strFromCharCode(int char1, int char2, int char3, ...) - string creation from symbol's codes
    //  char1, char2. char3 - symbol's codes
    if( iid == "strFromCharCode" )
    {
	string rez;
	for( int i_p = 0; i_p < prms.size(); i_p++ )
	    rez += (unsigned char)prms[i_p].getI();
	return rez;
    }

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TSYS::cntrCmdProc( XMLNode *opt )
{
    char buf[STR_BUF_LEN];

    //Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	snprintf(buf,sizeof(buf),_("%s station: \"%s\""),PACKAGE_NAME,name().c_str());
	ctrMkNode("oscada_cntr",opt,-1,"/",buf,R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/sub_",_("Subsystem"),R_R_R_,"root","root",1,"idm","1");
	if(TUIS::icoPresent(id())) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/gen",_("Station"),R_R_R_))
	{
	    ctrMkNode("fld",opt,-1,"/gen/id",_("ID"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/stat",_("Station"),RWRWR_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/prog",_("Program"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/ver",_("Version"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/host",_("Host name"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/user",_("System user"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/sys",_("Operation system"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/frq",_("Frequency (MHZ)"),R_R_R_,"root","root",1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/gen/clk_res",_("Realtime clock resolution (msec)"),R_R_R_,"root","root",1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/gen/in_charset",_("Internal charset"),R_R___,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/config",_("Config file"),R_R___,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/workdir",_("Work directory"),RWRW__,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/icodir",_("Icons directory"),RWRW__,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/moddir",_("Modules directory"),RWRW__,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/wrk_db",_("Work DB"),RWRWR_,"root",SDB_ID,4,"tp","str","dest","select","select","/db/list",
		"help",_("Work DB address in format [<DB module>.<DB name>].\nChange it field if you want save or reload all system from other DB."));
	    ctrMkNode("fld",opt,-1,"/gen/saveExit",_("Save system at exit"),RWRWR_,"root",SDB_ID,2,"tp","bool",
		"help",_("Select for automatic system saving to DB on exit."));
	    ctrMkNode("fld",opt,-1,"/gen/savePeriod",_("Save system period"),RWRWR_,"root",SDB_ID,2,"tp","dec",
		"help",_("Use no zero period (seconds) for periodic saving of changed systems parts to DB."));
	    ctrMkNode("fld",opt,-1,"/gen/lang",_("Language"),RWRWR_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/baseLang",_("Text variable's base language"),RWRWR_,"root","root",5,"tp","str","len","2","dest","sel_ed","select","/gen/baseLangLs",
		"help",_("Multilangual for variable texts support enabling by base language selection."));
	    if(ctrMkNode("area",opt,-1,"/gen/mess",_("Messages"),R_R_R_))
	    {
		ctrMkNode("fld",opt,-1,"/gen/mess/lev",_("Least level"),RWRWR_,"root","root",3,
		    "tp","dec","len","1","help",_("Least messages level which process by the system."));
		ctrMkNode("fld",opt,-1,"/gen/mess/log_sysl",_("To syslog"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/gen/mess/log_stdo",_("To stdout"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/gen/mess/log_stde",_("To stderr"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/gen/mess/log_arch",_("To archive"),RWRWR_,"root","root",1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/subs",_("Subsystems")))
	    ctrMkNode("list",opt,-1,"/subs/br",_("Subsystems"),R_R_R_,"root","root",3,"idm","1","tp","br","br_pref","sub_");
	if(ctrMkNode("area",opt,-1,"/tasks",_("Tasks"),R_R___))
	    if(ctrMkNode("table",opt,-1,"/tasks/tasks",_("Tasks"),RWRW__,"root","root",2,"key","path",
		"help",!multCPU()?"":_("For CPU set use processors numbers string separated by symbol ':'.\n"
				       "CPU numbers started from 0.")))
	    {
		ctrMkNode("list",opt,-1,"/tasks/tasks/path",_("Path"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/thrd",_("Thread"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/tid",_("TID"),R_R___,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/tasks/tasks/plc",_("Policy"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/prior",_("Prior."),R_R___,"root","root",1,"tp","dec");
		if( multCPU( ) )
		    ctrMkNode("list",opt,-1,"/tasks/tasks/cpuSet",_("CPU set"),RWRW__,"root","root",1,"tp","str");
	    }
	if( !cntrEmpty() && ctrMkNode("area",opt,-1,"/cntr",_("Counters")) )
	    if( ctrMkNode("table",opt,-1,"/cntr/cntr",_("Counters"),R_R___,"root","root") )
	    {
		ctrMkNode("list",opt,-1,"/cntr/cntr/id","ID",R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/cntr/cntr/vl",_("Value"),R_R___,"root","root",1,"tp","real");
	    }
	if( ctrMkNode("area",opt,-1,"/hlp",_("Help"),R_R___) )
	    ctrMkNode("fld",opt,-1,"/hlp/g_help",_("Options help"),R_R___,"root","root",3,"tp","str","cols","90","rows","10");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ico" && ctrChkNode(opt))
    {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(id(),&itp),TSYS::base64));
	opt->setAttr("tp",itp);
    }
    else if(a_path == "/gen/host" && ctrChkNode(opt))	opt->setText(host());
    else if(a_path == "/gen/sys" && ctrChkNode(opt))
    {
	utsname ubuf; uname(&ubuf);
	opt->setText(string(ubuf.sysname)+"-"+ubuf.release);
    }
    else if(a_path == "/gen/user" && ctrChkNode(opt))	opt->setText(mUser);
    else if(a_path == "/gen/prog" && ctrChkNode(opt))	opt->setText(PACKAGE_NAME);
    else if(a_path == "/gen/ver" && ctrChkNode(opt))	opt->setText(VERSION);
    else if(a_path == "/gen/id" && ctrChkNode(opt))	opt->setText(id());
    else if(a_path == "/gen/stat")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(name());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setName(opt->text());
    }
    else if(a_path == "/gen/frq" && ctrChkNode(opt))	opt->setText(TSYS::real2str((float)sysClk()/1000000.,6));
    else if(a_path == "/gen/clk_res" && ctrChkNode(opt))
    {
	struct timespec tmval;
	clock_getres(CLOCK_REALTIME,&tmval);
	opt->setText(TSYS::real2str((float)tmval.tv_nsec/1000000.,4));
    }
    else if(a_path == "/gen/in_charset" && ctrChkNode(opt))	opt->setText(Mess->charset());
    else if(a_path == "/gen/config" && ctrChkNode(opt))		opt->setText(mConfFile);
    else if(a_path == "/gen/wrk_db" )
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(mWorkDB);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setWorkDB(opt->text());
    }
    else if(a_path == "/gen/saveExit")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText( int2str(saveAtExit()) );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setSaveAtExit( atoi(opt->text().c_str()) );
    }
    else if(a_path == "/gen/savePeriod")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText( int2str(savePeriod()) );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setSavePeriod( atoi(opt->text().c_str()) );
    }
    else if(a_path == "/gen/workdir")
    {
	if(ctrChkNode(opt,"get",R_R___,"root","root",SEC_RD))	opt->setText(workDir());
	if(ctrChkNode(opt,"set",R_R___,"root","root",SEC_WR))	setWorkDir(opt->text().c_str());
    }
    else if(a_path == "/gen/icodir")
    {
	if(ctrChkNode(opt,"get",R_R___,"root","root",SEC_RD))	opt->setText(icoDir());
	if(ctrChkNode(opt,"set",R_R___,"root","root",SEC_WR))	setIcoDir(opt->text().c_str());
    }
    else if(a_path == "/gen/moddir")
    {
	if(ctrChkNode(opt,"get",R_R___,"root","root",SEC_RD))	opt->setText(modDir());
	if(ctrChkNode(opt,"set",R_R___,"root","root",SEC_WR))	setModDir(opt->text().c_str());
    }
    else if(a_path == "/gen/lang")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(Mess->lang());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLang(opt->text());
    }
    else if(a_path == "/gen/baseLang")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(Mess->lang2CodeBase());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLang2CodeBase(opt->text());
    }
    else if(a_path == "/gen/baseLangLs" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setText(Mess->lang2Code());
	if(!Mess->lang2CodeBase().empty() && Mess->lang2CodeBase() != Mess->lang2Code())
	    opt->childAdd("el")->setText(Mess->lang2CodeBase());
	opt->childAdd("el")->setText("");
    }
    else if(a_path == "/gen/mess/lev")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(TSYS::int2str(Mess->messLevel()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setMessLevel(atoi(opt->text().c_str()));
    }
    else if(a_path == "/gen/mess/log_sysl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x01)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect( atoi(opt->text().c_str())?Mess->logDirect()|0x01:Mess->logDirect()&(~0x01) );
    }
    else if(a_path == "/gen/mess/log_stdo")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x02)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect( atoi(opt->text().c_str())?Mess->logDirect()|0x02:Mess->logDirect()&(~0x02) );
    }
    else if(a_path == "/gen/mess/log_stde")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x04)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect( atoi(opt->text().c_str())?Mess->logDirect()|0x04:Mess->logDirect()&(~0x04) );
    }
    else if(a_path == "/gen/mess/log_arch")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x08)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect( atoi(opt->text().c_str())?Mess->logDirect()|0x08:Mess->logDirect()&(~0x08) );
    }
    else if((a_path == "/br/sub_" || a_path == "/subs/br") && ctrChkNode(opt,"get",R_R_R_,"root","root",SEC_RD))
    {
	vector<string> lst;
	list(lst);
	for(unsigned i_a=0; i_a < lst.size(); i_a++)
	    opt->childAdd("el")->setAttr("id",lst[i_a])->setText(at(lst[i_a]).at().subName());
    }
    else if(a_path == "/tasks/tasks")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root","root"))
	{
	    XMLNode *n_path	= ctrMkNode("list",opt,-1,"/tasks/tasks/path","",R_R___,"root","root");
	    XMLNode *n_thr	= ctrMkNode("list",opt,-1,"/tasks/tasks/thrd","",R_R___,"root","root");
	    XMLNode *n_tid	= ctrMkNode("list",opt,-1,"/tasks/tasks/tid","",R_R___,"root","root");
	    XMLNode *n_plc	= ctrMkNode("list",opt,-1,"/tasks/tasks/plc","",R_R___,"root","root");
	    XMLNode *n_prior	= ctrMkNode("list",opt,-1,"/tasks/tasks/prior","",R_R___,"root","root");
	    XMLNode *n_cpuSet	= (multCPU() ? ctrMkNode("list",opt,-1,"/tasks/tasks/cpuSet","",RWRW__,"root","root") : NULL);

	    ResAlloc res(taskRes,false);
	    for(map<string,STask>::iterator it = mTasks.begin(); it != mTasks.end(); it++)
	    {
		if(n_path)	n_path->childAdd("el")->setText(it->first);
		if(n_thr)	n_thr->childAdd("el")->setText(TSYS::uint2str(it->second.thr));
		if(n_tid)	n_tid->childAdd("el")->setText(TSYS::int2str(it->second.tid));
		if(n_plc)	n_plc->childAdd("el")->setText((it->second.policy==SCHED_RR)?_("Round-robin"):((it->second.policy==SCHED_BATCH)?_("Style \"batch\""):_("Standard")));
		if(n_prior)	n_prior->childAdd("el")->setText(TSYS::int2str(it->second.prior));
		if(n_cpuSet)	n_cpuSet->childAdd("el")->setText(it->second.cpuSet);
	    }
	}
	if(multCPU() && ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR) && opt->attr("col") == "cpuSet")
	{
	    ResAlloc res(taskRes,true);
	    map<string,STask>::iterator it = mTasks.find(opt->attr("key_path"));
	    if(it == mTasks.end()) throw TError(nodePath().c_str(),_("No present task '%s'."));
	    if(it->second.flgs & STask::Detached) return;

	    it->second.cpuSet = opt->text();

	    cpu_set_t cpuset;
	    CPU_ZERO(&cpuset);
	    string sval;
	    for(int off = 0; (sval=TSYS::strParse(it->second.cpuSet,0,":",&off)).size(); )
		CPU_SET(atoi(sval.c_str()),&cpuset);
	    int rez = pthread_setaffinity_np(it->second.thr, sizeof(cpu_set_t), &cpuset);
	    res.release();
	    TBDS::genDBSet(nodePath()+"CpuSet:"+it->first,opt->text());
	    if(rez == EINVAL && opt->text().size()) throw TError(nodePath().c_str(),_("Set no allowed processors."));
	    if(rez && opt->text().size()) throw TError(nodePath().c_str(),_("CPU set for thread error."));
	}
    }
    if(!cntrEmpty() && a_path == "/cntr/cntr" && ctrChkNode(opt,"get",R_R___,"root","root"))
    {
	XMLNode *n_id	= ctrMkNode("list",opt,-1,"/cntr/cntr/id","",R_R___,"root","root");
	XMLNode *n_vl	= ctrMkNode("list",opt,-1,"/cntr/cntr/vl","",R_R___,"root","root");

	ResAlloc res( nodeRes(), false );
	for(map<string,double>::iterator icnt = mCntrs.begin(); icnt != mCntrs.end(); icnt++)
	{
	    if(n_id)	n_id->childAdd("el")->setText(icnt->first);
	    if(n_vl)	n_vl->childAdd("el")->setText(TSYS::real2str(icnt->second));
	}
    }
    else if(a_path == "/hlp/g_help" && ctrChkNode(opt,"get",R_R___,"root","root",SEC_RD)) opt->setText(optDescr());
    else TCntrNode::cntrCmdProc(opt);
}

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>

#include "../config.h"
#include "terror.h"
#include "tmessage.h"
#include "tkernel.h"
#include "tsys.h"

const char *TSYS::o_name = "TSYS";
const char *TSYS::n_opt  = "generic";

TSYS::TSYS( int argi, char ** argb, char **env ) : Conf_File("./oscada.xml"), m_station("default"), stat_n(NULL), 
    User(getenv("USER")),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0)
{
    setlocale(LC_ALL,"");	
    CheckCommandLine();
    UpdateOpt();    
}

TSYS::~TSYS(  )
{
    vector<string> list;
    KernList(list);
    for( unsigned i_krn = 0; i_krn < list.size(); i_krn++ )
        KernRemove(list[i_krn]);    
}

unsigned TSYS::ResCreate( unsigned val )
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

void TSYS::ResDelete( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("%s: error delete semaphor %d!", o_name, res);
    
    sems[res].del = true;
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(100000);
    sem_destroy( &sems[res].sem );
    sems[res].use = false;   
}

void TSYS::WResRequest( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <w> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(100000);
}

void TSYS::WResRelease( unsigned res )
{
    if(res >= sems.size() || !sems[res].use )
	throw TError("%s: error <w> release semaphor %d!", o_name, res);
    sem_post( &sems[res].sem );
}

void TSYS::RResRequest( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <r> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    sems[res].rd_c++;   
    sem_post( &sems[res].sem );
}

void TSYS::RResRelease( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("%s: error <r> release semaphor %d!", o_name, res);
    if( sems[res].rd_c > 0 ) sems[res].rd_c--;   
}

XMLNode *TSYS::XMLCfgNode() 
{ 
    if(!stat_n) throw TError("%s: XML config error or no avoid!",o_name);
    return(stat_n); 
}

void TSYS::pr_opt_descr( FILE * stream )
{
    utsname buf;

    uname(&buf);
    fprintf(stream,
    "****************************************\n"
    "**** %s v%s (%s-%s). ****\n"
    "****************************************\n\n"
    "===========================================================================\n"
    "============================ General options ==============================\n"
    "===========================================================================\n"
    "-h, --help             Info message for server's options;\n"
    "    --Config=<path>    Config file path;\n"
    "    --Station=<name>   Station name;\n"
    "--------------- Fields <%s> sections of config file -------------------\n"
    "\n",PACKAGE,VERSION,buf.sysname,buf.release,n_opt);
}

void TSYS::CheckCommandLine( )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read commandline options!",o_name);
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
	    case 'h': pr_opt_descr(stdout); break;
	    case 'f': Conf_File = optarg; break;
	    case 's': m_station = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Read commandline options ok!",o_name);
#endif	
}

void TSYS::UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read config options!",o_name);
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
	catch( TError err ) { Mess->put("SYS",MESS_WARNING,"%s:%s",o_name, err.what().c_str() ); }
    }    
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Read config options ok!",o_name);
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
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGCHLD,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
    /*
    struct sigaction sa;
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler= sighandler;
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGCHLD,&sa,NULL);
    sigaction(SIGALRM,&sa,NULL);
    sigaction(SIGPIPE,&sa,NULL);
    */
    while(1)	
    {
	if(stop_signal) break;   
	ScanCfgFile( );	
       	sleep(10); 
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
	Mess->put("SYS",MESS_WARNING,"Have get a Terminate signal. Server been stoped!"); 
	SYS->stop_signal=signal; 
    }
    else if(signal == SIGCHLD)
    {
	int status;
	pid_t pid = wait(&status);
	if(!WIFEXITED(status))
	    Mess->put("SYS",MESS_INFO,"Free child process %d!",pid);
    }	
    else if(signal == SIGPIPE)
	Mess->put("SYS",MESS_WARNING,"%s: Broken PIPE signal allow!",o_name);
}

void TSYS::KernList( vector<string> & list ) const
{
    list.clear();
    for( unsigned i_krn = 0; i_krn < m_kern.size(); i_krn++ )
	list.push_back(m_kern[i_krn]->Name());	
}

TKernel &TSYS::KernMake( const string name )
{
    m_kern.push_back( new TKernel( name ) );
    return( *m_kern[m_kern.size()-1] );
}

void TSYS::KernRemove( const string name )
{
    for( vector<TKernel *>::iterator it = m_kern.begin(); it != m_kern.end(); it++ )
	if( (*it)->Name() == name )
	{
	    delete *it;
	    m_kern.erase(it);
	    return;
	}
    throw TError("%s: kernel %s no avoid!",o_name,name.c_str());
}

TKernel &TSYS::at( const string name ) const
{
    for( vector<TKernel *>::const_iterator it = m_kern.begin(); it != m_kern.end(); it++ )
	if( (*it)->Name() == name )
	    return( *(*it) );
}

void TSYS::ScanCfgFile( )
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
    if(up == true)
    {
	UpdateOpt();
	Mess->UpdateOpt();
	for( unsigned i_kern = 0; i_kern < m_kern.size(); i_kern++)
	    m_kern[i_kern]->UpdateOpt();	
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


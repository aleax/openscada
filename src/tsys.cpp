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
	if(sems[i_sem] == TO_FREE) break;
    if( i_sem == sems.size() ) sems.push_back(new sem_t);
    else                       sems[i_sem] = new sem_t;
    if( sem_init(sems[i_sem],0,val) != 0 )
    {
	delete sems[i_sem]; sems[i_sem] = TO_FREE;
	throw TError("%s: error open semaphor!", o_name);
    }

    return(i_sem);
}

void TSYS::ResDelete( unsigned res )
{
    if(res >= sems.size() || sems[res] == TO_FREE)
	throw TError("%s: error semaphor id = %d!", o_name, res);
    sem_destroy(sems[res]);
    delete sems[res]; 
    sems[res] = TO_FREE;
}

void TSYS::ResRequest( unsigned res, long tm )
{
    if(res >= sems.size() || sems[res] == TO_FREE)
	throw TError("%s: error semaphor id = %d!", o_name, res);
    sem_wait(sems[res]);
}

void TSYS::ResRelease( unsigned res )
{
    if(res >= sems.size() || sems[res] == TO_FREE)
	throw TError("%s: error semaphor id = %d!", o_name, res);
    sem_post(sems[res]);
}

/*
bool TSYS::GetOpt(string section, string opt, string &value, unsigned entry, bool excep)
{
    int line  = 0,  // file's line
	f_cnt,      // file cntr
	i;          // generic index
    unsigned i_entry=0;   // counter entry
    bool f_beg  = false,  // command begin
 	 f_sect = false;  // section ok
    string str;
    char *func = "GetOpt";

    int hd = open(Conf_File.c_str(),O_RDONLY);
    if(hd < 0)
    { 
	if(excep) throw TError("%s: no config file!",func);
	else      return(false);
    }
    int cf_sz = lseek(hd,0,SEEK_END);
    lseek(hd,0,SEEK_SET);
    char *buf = (char *)malloc(cf_sz);
    read(hd,buf,cf_sz);
    close(hd);

    for(f_cnt = 0; f_cnt < cf_sz; f_cnt++)
    {
	if(buf[f_cnt] == '[' && f_beg == false )
	{
	    if( f_sect == true ) 
	    { 
		free(buf); 
	        if(excep) throw TError("%s: option <%s> no avoid!",func,opt.c_str()); 
		else      return(false);		
	    }
	    for(i = (++f_cnt); i < cf_sz && buf[i]!=']'; i++);
	    if(i >= cf_sz)
	    { 
		free(buf); 
	        if(excep) throw TError("%s: Config file error (line: %d)!",func,line); 
		else      return(false);
	    }
	    str.assign(&buf[f_cnt],i-f_cnt);
	    if(str == section)
	    { 
		f_sect = true;
		i_entry = 0;
	    }
	    f_cnt = i+1;
	}
	if(buf[f_cnt] == 0x20 || buf[f_cnt] == 0x09) continue;
	if(buf[f_cnt] == 0x0A || buf[f_cnt] == 0x0D)
	{
	    if(buf[f_cnt] == 0x0A) line++;
	    f_beg = false;
	    continue;
	}
    	if(buf[f_cnt] == '#')
	{
            for(; f_cnt < cf_sz && buf[f_cnt] != 0x0A && buf[f_cnt] != 0x0D; f_cnt++);
	    f_cnt--;
	    continue;
	}
	if(f_sect == true && f_beg == false )
	{
	    int last_i, first_i;
	    bool first;
	    for( last_i = i = f_cnt; i < cf_sz && buf[i] != '=' && buf[i] != 0x0A && buf[i] != 0x0D; i++)
	    if(buf[i] != 0x20 && buf[i] != 0x09) last_i = i;
	    if(i >= cf_sz || buf[i] == 0x0A || buf[i] == 0x0D )
	    { 
		free(buf); 
		if(excep) throw TError("%s: config file error (line: %d)!",func,line); 
		else      return(false);
	    }
	    //if(buf[i] == 0x0A || buf[i] == 0x0D) { f_cnt=i-1; continue; }
	    str.assign(&buf[f_cnt],last_i-f_cnt+1);
    	    if(str != opt || (str == opt && (++i_entry) != entry) ) 
	    { 
		f_beg = true; 
		f_cnt=i; 
		continue; 
	    }
	    f_cnt = i+1; first = false;
	    for(first_i = last_i = i = f_cnt; i < cf_sz && buf[i] != 0x0A && buf[i] != 0x0D; i++)
		if(buf[i] != 0x20 && buf[i] != 0x09)
		{
		    if(first == false) { first_i = i; first = true; }
                    last_i = i;
		}
	    // separate string "value"
	    if( first_i != last_i && buf[first_i] == '"' && buf[last_i] == '"' )
	    {   
		first_i++;
		last_i--;
	    }		
	    if(first == true) str.assign(&buf[first_i],last_i-first_i+1);
	    else              str = "";
	    free(buf);

	    value = str;
	    return(true);
	}
    	f_beg = true;
    }
    free(buf);
    if(f_sect == true)
	if(excep) throw TError("%s: option <%s> no avoid!",func,opt.c_str());
    if(excep) throw TError("%s: section <%s> no avoid!",func,section.c_str());
    return(false);
}
*/

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
}

void TSYS::UpdateOpt()
{
    stat_n = NULL;
    //int hd = open("./oscada.xml",O_RDONLY);
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
	catch( TError err ) { Mess->put("SYS",MESS_DEBUG,"%s:%s",o_name, err.what().c_str() ); }
    }    
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
    vsprintf(buf, fmt, argptr);
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
    struct sigaction sa;
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler= sighandler;
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGCHLD,&sa,NULL);
    
    while(1)	
    {
	if(stop_signal) break;   
	ScanCfgFile( );	
       	sleep(10); 
    }

//    sa.sa_handler= SIG_DFL;
//    sigaction(SIGINT,&sa,NULL);
//    sigaction(SIGTERM,&sa,NULL);        

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


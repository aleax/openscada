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
const char *TSYS::n_opt  = "generic";
const char *TSYS::i_cntr = 
	"<obj>"
	" <configs> Base informations:"
	"  <fld id='host' dscr='Host name' acs='0444' tp='str'/>"
	"  <fld id='user' dscr='Operated user' acs='0444' tp='str'/>"
	"  <fld id='sys' dscr='Station system' acs='0444' tp='str'/>"
	" </configs>"
	" <configs acs='0440'> Generic parameters:"
	"  <fld id='config' dscr='Config file' acs='0660' com='1' tp='str' dest='file'/>"
        "  <fld id='cr_file_perm' dscr='Make files permissions(default 0644)' acs='0660' cfg='1' tp='oct' len='3'/>"
        "  <fld id='cr_dir_perm' dscr='Make directories permissions(default 0755)' acs='0660' cfg='1' tp='oct' len='3'/>"
	"  <configs> Message parameters:"	
	"   <fld id='debug' dscr='Debug level' acs='0660' com='1' cfg='1' tp='dec' min='0' max='8'/>"	
	"   <fld id='in_charset' dscr='Internal charset(default UTF8)' acs='0660' com='1' cfg='1' tp='str'/>"	
	"   <fld id='m_buf_l' dscr='The ring message buffer elements number' acs='0660' com='1' cfg='1' tp='dec' min='10'/>"	
	"   <fld id='log_sysl' dscr='Direct messages to syslog' acs='0660' com='1' cfg='1' tp='bool'/>"	
	"   <fld id='log_stdo' dscr='Direct messages to stdout' acs='0660' com='1' cfg='1' tp='bool'/>"	
	"   <fld id='log_stde' dscr='Direct messages to stderr' acs='0660' com='1' cfg='1' tp='bool'/>"	
	"  </configs>"
	" </configs>"
 	" <comm id='upd_opt' dscr='Update options(from config)'/>"       
	" <comm id='quit' dscr='Quit'/>"
	" <branchs id='kern' dscr='Kernels:' mode='att'>"
	"  <comm id='add' dscr='Add kernel'>"
	"   <fld id='name' dscr='Kernel name' tp='str'/>"
	"  </comm>"
	"  <comm id='del' dscr='Delete kernel'>"
	"   <fld id='name' dscr='Kernel name' tp='str' dest='select' select='kern'/>"
	"  </comm>"
        " </branchs>"
	"</obj>";

vector<SSem> TSYS::sems;

TSYS::TSYS( int argi, char ** argb, char **env ) : Conf_File("./oscada.xml"), m_station("default"), stat_n(NULL), 
    User(getenv("USER")),argc(argi), envp((const char **)env), argv((const char **)argb), stop_signal(0), 
    m_cr_f_perm(0644), m_cr_d_perm(0755), m_kern(o_name), TContr( i_cntr )
{
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
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
    sem_destroy( &sems[res].sem );
    sems[res].use = false;   
}

void TSYS::WResRequest( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("%s: error <w> request semaphor %d!", o_name, res);
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
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
    " cr_file_perm = <perm> Permision of created files (default 0644);\n"
    " cr_dir_perm  = <perm> Permision of created directories (default 0755);\n"
    "\n",PACKAGE,VERSION,buf.sysname,buf.release,n_opt);
}

void TSYS::CheckCommandLine( )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"*:(%s)Read commandline options!",o_name);
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
    Mess->put("DEBUG",MESS_DEBUG,"*:(%s)Read commandline options ok!",o_name);
#endif	
}

void TSYS::UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"*:(%s)Read config options!",o_name);
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
	catch( TError err ) { Mess->put("SYS",MESS_WARNING,"*:(%s)%s",o_name, err.what().c_str() ); }
    }    
    
    //All system parameters
    try{ m_cr_f_perm = strtol( XMLCfgNode()->get_child("cr_file_perm")->get_text().c_str(),NULL,8); }
    catch(...) {  }
    try{ m_cr_d_perm = strtol( XMLCfgNode()->get_child("cr_dir_perm")->get_text().c_str(),NULL,8); }
    catch(...) {  }

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"*:(%s)Read config options ok!",o_name);
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
    
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGCHLD,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
    
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
	Mess->put("SYS",MESS_WARNING,"*:(%s)Have get a Terminate signal. Server been stoped!",o_name); 
	SYS->stop_signal=signal; 
    }
    else if(signal == SIGCHLD)
    {
	int status;
	pid_t pid = wait(&status);
	if(!WIFEXITED(status))
	    Mess->put("SYS",MESS_INFO,"*:(%s)Free child process %d!",o_name,pid);
    }	
    else if(signal == SIGPIPE)
	Mess->put("SYS",MESS_WARNING,"*:(%s)Broken PIPE signal allow!",o_name);
}

void TSYS::kern_add( string name )
{
    TKernel *kern = new TKernel( name );
    try{ m_kern.obj_add( kern, &kern->Name() ); }
    catch(TError err) { delete kern; }
}

void TSYS::kern_del( string name )
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
	{
    	    int k_hd = kern_att( list[i_kern] );	    
	    kern_at(k_hd).UpdateOpt();	
	    kern_det(k_hd);
	}
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

bool TSYS::event_wait( bool &m_mess_r_stat, bool exempl, string loc, time_t tm )
{
    time_t t_tm, s_tm;
    
    t_tm = s_tm = time(NULL);
    while( m_mess_r_stat != exempl )
    {
	time_t c_tm = time(NULL);
	//Check timeout
	if( tm && ( c_tm > s_tm+tm) )
	{
	    Mess->put("SYS",MESS_CRIT,"*:(%s)Go timeout %s!!!!",o_name,loc.c_str());
    	    return(true);
	}
	//Make messages
	if( c_tm > t_tm+1 )  //1sec 
	{
	    t_tm = c_tm;
	    Mess->put("SYS",MESS_INFO,"*:(%s) %s",o_name,loc.c_str());
	}
	usleep(STD_WAIT_DELAY*1000);
    }
    return(false);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TSYS::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(string("OpenSCADA station: ")+m_station);
}

void TSYS::ctr_din_get( XMLNode *opt )
{
    utsname buf;
    uname(&buf);
 
    if( opt->get_name() == "branchs" )
    {
	vector<string> list;
	kern_list(list);
	ctr_br_putlist(opt,list);
    }
    string t_id = opt->get_attr("id");
    //if( t_id == "station" )    ctr_opt_setS( opt, m_station );
    if( t_id == "host" )              ctr_opt_setS( opt, buf.nodename );
    else if( t_id == "sys" )          ctr_opt_setS( opt, string(buf.sysname)+"-"+buf.release );
    else if( t_id == "user" )         ctr_opt_setS( opt, User );
    else if( t_id == "config" )       ctr_opt_setS( opt, Conf_File );
    else if( t_id == "cr_file_perm" ) ctr_opt_setI( opt, m_cr_f_perm );
    else if( t_id == "cr_dir_perm" )  ctr_opt_setI( opt, m_cr_d_perm );
    else if( t_id == "debug" )        ctr_opt_setI( opt, Mess->d_level() );
    else if( t_id == "in_charset" )   ctr_opt_setS( opt, Mess->charset() );
    else if( t_id == "m_buf_l" )      ctr_opt_setI( opt, Mess->mess_buf_len() );
    else if( t_id == "log_sysl" )     ctr_opt_setB( opt, (Mess->log_direct()&0x01)?true:false );
    else if( t_id == "log_stdo" )     ctr_opt_setB( opt, (Mess->log_direct()&0x02)?true:false );
    else if( t_id == "log_stde" )     ctr_opt_setB( opt, (Mess->log_direct()&0x04)?true:false );
}

void TSYS::ctr_din_set( XMLNode *opt )
{
    string t_id = opt->get_attr("id");
    
    //if( t_id == "station" )           m_station = ctr_opt_getS( opt );
    if( t_id == "config" )       Conf_File = ctr_opt_getS( opt );
    else if( t_id == "cr_file_perm" ) m_cr_f_perm = ctr_opt_getI( opt );
    else if( t_id == "cr_dir_perm" )  m_cr_d_perm = ctr_opt_getI( opt );
    else if( t_id == "debug" )        Mess->d_level( ctr_opt_getI( opt ) );
    else if( t_id == "in_charset" )   Mess->charset( ctr_opt_getS( opt ) );
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

void TSYS::ctr_cmd_go( XMLNode *fld )
{
    string t_id = fld->get_attr("id");
    if( t_id == "quit" )         stop_signal=1;
    else if( t_id == "upd_opt" ) UpdateOpt();
    else if( t_id == "add" )     kern_add(ctr_opt_getS(ctr_id(fld,"name")));
    else if( t_id == "del" )     kern_del(ctr_opt_getS(ctr_id(fld,"name")));    
}

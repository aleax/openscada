#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#include <string>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tarhives.h"
#include "tparams.h"
#include "tparam.h"
#include "tcontroller.h"
#include "tparamcontr.h"
#include "tcontrollers.h"
#include "ttipcontroller.h"
#include "tprotocols.h"
#include "ttransports.h"
#include "tspecials.h"
#include "tvalue.h"
#include "tmodschedul.h"

const char *TModSchedul::o_name = "TModSchedul";
const char *TModSchedul::s_name = "Modules sheduler";
const char *TModSchedul::i_cntr = 
	"<oscada_cntr>"
	" <area id='a_ms' dscr='Subsystem control.' acs='0440'>"
	"  <fld id='mod_path' com='1' cfg='1' dest='dir' tp='str'/>"
	"  <list id='mod_auto' tp='str' dest='file'/>"
	"  <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
	" </area>"
	"</oscada_cntr>";

TModSchedul::TModSchedul( TKernel *app ) : m_stat(false), owner(app), m_mod_path("./")
{
    hd_res = TSYS::ResCreate();
}

TModSchedul::~TModSchedul(  )
{
    DeinitAll();
    if( m_stat ) 
    {
    	m_endrun = true;
	SYS->event_wait( m_stat, false, string(o_name)+": The modules scheduler thread is stoping....");
	pthread_join( pthr_tsk, NULL );
    }
    
    //Detach all share libs 
    TSYS::WResRequest(hd_res);    
    for( unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh]->hd )
	{
    	    while( SchHD[i_sh]->use.size() )
	    {	
		string n_mod = grpmod[SchHD[i_sh]->use[0].id_tmod]->gmd_at( SchHD[i_sh]->use[0].id_mod ).mod_Name();
	       	grpmod[SchHD[i_sh]->use[0].id_tmod]->gmd_det( SchHD[i_sh]->use[0].id_mod );
	       	grpmod[SchHD[i_sh]->use[0].id_tmod]->gmd_del( n_mod );
		SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
	    }
	    dlclose(SchHD[i_sh]->hd);
	    SchHD[i_sh]->hd = NULL;
	}	
    TSYS::WResRelease(hd_res);    
    
    TSYS::ResDelete(hd_res);
}

string TModSchedul::Name()
{ 
    return(Mess->I18N((char *)s_name)); 
}

string TModSchedul::opt_descr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"=================== The module sheduler subsystem options =================\n"
    	"    --ModPath=<path>   set modules <path>: \"/var/os/modules/,./mod/\"\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path  <path>       set path to shared libs;\n"
    	"mod_auto  <list>       names of automatic loaded, attached and started shared libs <direct_dbf.so;virt.so>\n"
	),s_name);
    
    return(buf);
}

void TModSchedul::StartSched( )
{ 
    if( m_stat ) return;
    pthread_attr_t      pthr_attr;

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,TModSchedul::SchedTask,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait( m_stat, true, string(o_name)+": The modules scheduler thread is starting....",5) )
    	throw TError("%s: The modules scheduler thread no started!",o_name);
}

void *TModSchedul::SchedTask(void *param)
{
    int cntr = 0;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

    TModSchedul  *shed = (TModSchedul *)param;
    
    shed->m_stat   = true;
    shed->m_endrun = false;
    
#if OSC_DEBUG
    shed->Owner().m_put("DEBUG",MESS_DEBUG,"%s:Thread <%d>!",s_name,getpid() );
#endif
//    setenv("_","OpenScada: test",1);
//    Owner().SetTaskTitle("TEST");
    do 
    {	
	try
	{
	    if( ++cntr >= 10*1000/STD_WAIT_DELAY ) //10 second
	    {
		cntr = 0;
		
		shed->Load(shed->m_mod_path,-1,true);
		for(unsigned i_gm=0; i_gm < shed->grpmod.size(); i_gm++)
		    shed->Load(shed->grpmod[i_gm]->gmd_ModPath(),i_gm,true);
	    }
	} catch(TError err){ shed->Owner().m_put("SYS",MESS_ERR,"%s:%s",s_name,err.what().c_str()); }
	usleep(STD_WAIT_DELAY*1000);
    } while( !shed->m_endrun );
    shed->m_stat   = false;

    return(NULL);
}

int TModSchedul::RegGroupM(TGRPModule *gmod)
{
    if(gmod == NULL) return(-1);
    for(unsigned i_grmd = 0; i_grmd < grpmod.size(); i_grmd++)
	if(grpmod[i_grmd] == gmod) return(i_grmd);
    grpmod.push_back(gmod);

    return(grpmod.size()-1);
}

int TModSchedul::UnRegGroupM(TGRPModule *gmod)
{
    if(gmod == NULL) return(-1);
    for(unsigned i_grmd = 0; i_grmd < grpmod.size(); i_grmd++)
	if(grpmod[i_grmd] == gmod)
	{
	    grpmod.erase(grpmod.begin()+i_grmd);
	    return(0);
	}

    return(-2);
}

XMLNode *TModSchedul::XMLCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = Owner().XMLCfgNode()->get_child("section",i_k++);
	if( t_n->get_attr("id") == s_name ) return( t_n );
    }
}

void TModSchedul::CheckCommandLine(  )
{
#if OSC_DEBUG
    Owner().m_put("DEBUG",MESS_INFO,"%s:Read commandline options!",s_name);
#endif
	
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"ModPath"  ,1,NULL,'m'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': m_mod_path = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Owner().m_put("DEBUG",MESS_DEBUG,"%s:Read commandline options ok!",s_name);
#endif

    // Check all subsystems
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmd_CheckCommandLine( );
}

void TModSchedul::CheckCommandLineMod(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmd_CheckCommandLineMods();
}

void TModSchedul::UpdateOpt()
{
    try{ m_mod_path = XMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    
    try
    {
	string opt = XMLCfgNode()->get_child("id","mod_auto")->get_text();
	if( opt.size() )
	{
	    int i_beg = -1;
    	    m_am_list.clear();
	    do
	    {
		m_am_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
		i_beg = opt.find(";",i_beg+1);
	    } while(i_beg != (int)string::npos);
	}
    }
    catch(...) {  }
    
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmd_UpdateOpt();
}

void TModSchedul::UpdateOptMod()
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmd_UpdateOptMods();
}

void TModSchedul::LoadAll(  )
{
    Load(m_mod_path,-1,false);
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	Load(grpmod[i_gm]->gmd_ModPath(),i_gm,false);
}

void TModSchedul::InitAll(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmd_Init( );
}

void TModSchedul::DeinitAll(  )
{
//  for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
//	grpmod[i_gm]->gmd_Deinit( );
}

void TModSchedul::StartAll(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
    {
	try{ grpmod[i_gm]->gmd_Start( ); }
	catch(...){ }
    }
}

void TModSchedul::ScanDir( const string &Paths, vector<string> &files, bool new_f ) const
{
    string NameMod, Path;

    files.clear();
    

    int ido, id=-1;
    do
    {
        ido=id+1; id = Paths.find(",",ido);

        dirent *scan_dirent;
        Path=Paths.substr(ido,id-ido);
        if(Path.size() <= 0) continue;
	
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_INFO,"%s:Scan dir <%s> !",s_name,Path.c_str());
#endif  

	// Convert to absolutly path
        Path = SYS->FixFName(Path);

        DIR *IdDir = opendir(Path.c_str());
        if(IdDir == NULL) continue;

        while((scan_dirent = readdir(IdDir)) != NULL)
        {
	    if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
            NameMod=Path+"/"+scan_dirent->d_name;
            if( CheckFile(NameMod, new_f) ) files.push_back(NameMod); 
        }
        closedir(IdDir);
	
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_DEBUG,"%s:Scan dir <%s> ok !",s_name,Path.c_str());
#endif    
	
    } while(id != (int)string::npos);
}

bool TModSchedul::CheckFile( const string &name, bool new_f ) const
{
    struct stat file_stat;
    string NameMod;

    stat(name.c_str(),&file_stat);

    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return(false);
    if( access(name.c_str(),F_OK|R_OK) != 0 )      return(false);
    NameMod=name;
    
    void *h_lib = dlopen(name.c_str(),RTLD_GLOBAL|RTLD_LAZY);
    if(h_lib == NULL)
    {
        Owner().m_put("SYS",MESS_WARNING,"%s:SO %s error: %s !",s_name,name.c_str(),dlerror());
        return(false);
    }
    else dlclose(h_lib);        
    
    if(new_f)
	for(unsigned i_sh=0; i_sh < SchHD.size(); i_sh++)
	    if(SchHD[i_sh]->name == name && SchHD[i_sh]->m_tm == file_stat.st_mtime) 
		return(false);

    return(true);
}

int  TModSchedul::RegSO( const string &name )
{
    struct stat file_stat;
    
    TSYS::WResRequest(hd_res);    
    stat(name.c_str(),&file_stat);
    unsigned i_sh;
    for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
       	if( SchHD[i_sh]->name == name ) 
	{
	    SchHD[i_sh]->m_tm = file_stat.st_mtime;   
	    TSYS::WResRelease(hd_res);
	    return(i_sh);    
	}
    SchHD.push_back( new SHD );	
    SchHD[i_sh]->hd   = NULL;   
    SchHD[i_sh]->m_tm = file_stat.st_mtime;   
    SchHD[i_sh]->name = name;       
    TSYS::WResRelease(hd_res);
    
    return(i_sh);    
}

void TModSchedul::UnregSO( const string &name )
{
    TSYS::WResRequest(hd_res);    
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    if( SchHD[i_sh]->hd ) DetSO( name );
	    delete SchHD[i_sh];
	    SchHD.erase(SchHD.begin()+i_sh);
	    TSYS::WResRelease(hd_res);
	    return;
	}
    TSYS::WResRelease(hd_res);
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}
    
void TModSchedul::AttSO( const string &name, bool full, int dest )
{
    TSYS::WResRequest(hd_res);    
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    if( SchHD[i_sh]->hd ) 
	    {
	    	TSYS::WResRelease(hd_res);
		throw TError("%s: SO <%s> already attached!",o_name,name.c_str());	    
	    }
	    
	    void *h_lib = dlopen(name.c_str(),RTLD_GLOBAL|RTLD_LAZY);	    
	    if( !h_lib )
	    {
	    	TSYS::WResRelease(hd_res);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());	    
	    }
	    
	    //Connect to module function
	    SAtMod (*module)( int );
	    (void *)module = dlsym(h_lib,"module");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
	    	TSYS::WResRelease(hd_res);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());
	    }    
	    
	    //Connect to attach function	    
	    TModule *(*attach)( SAtMod &, string );
	    (void *)attach = dlsym(h_lib,"attach");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
	    	TSYS::WResRelease(hd_res);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());
	    }    
	    
	    struct stat file_stat;
	    stat(name.c_str(),&file_stat);
    
	    int n_mod=0, add_mod=0;
	    SAtMod AtMod;
	    while( (AtMod = (module)( n_mod++ )).name.size() )
	    {
		for( unsigned i_grm = 0; i_grm < grpmod.size(); i_grm++)
		{
		    if(dest >= 0) i_grm = dest;
		    if( AtMod.type == grpmod[i_grm]->gmd_Name() )
		    { 
			unsigned hd;
			//Check type module version
			if( AtMod.t_ver != grpmod[i_grm]->gmd_Ver() )
			{
			    Owner().m_put("SYS",MESS_WARNING,"%s:%s for type <%s> no support module version: %d!",
				s_name,AtMod.name.c_str(),AtMod.type.c_str(),AtMod.t_ver);
			    break;
			}
			//Check avoid module
			try{ hd = grpmod[i_grm]->gmd_att( AtMod.name ); }
			catch(TError)
			{
			    //Attach new module
			    TModule *LdMod = (attach)( AtMod, name );
			    if( LdMod == NULL )
			    {
				Owner().m_put("SYS",MESS_WARNING,"%s:Attach module <%s> error!",s_name,AtMod.name.c_str());
				break;
			    }
			    //Add atached module
			    grpmod[i_grm]->gmd_add(LdMod);
			    hd = grpmod[i_grm]->gmd_att(LdMod->mod_Name());
			    SUse t_suse = { i_grm, hd };
			    SchHD[i_sh]->use.push_back( t_suse );
			    if(full)
			    {
				grpmod[i_grm]->gmd_Init();
				grpmod[i_grm]->gmd_Start();
			    }
			    add_mod++;
			    break;
			}
			grpmod[i_grm]->gmd_det( hd );
			Owner().m_put("SYS",MESS_WARNING,"%s:Module %s already avoid!",s_name,AtMod.name.c_str());		    
		    }
		    if(dest >= 0) break;
		}
	    }
	    if(add_mod == 0) dlclose(h_lib);	    
	    else SchHD[i_sh]->hd = h_lib;
	    TSYS::WResRelease(hd_res);
	    return;
	}
    TSYS::WResRelease(hd_res);
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}

void TModSchedul::DetSO( const string &name )
{
    TSYS::WResRequest(hd_res);    
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
    {
       	if( SchHD[i_sh]->name == name && SchHD[i_sh]->hd )
	{
    	    while( SchHD[i_sh]->use.size() )
	    {
		string n_mod = grpmod[SchHD[i_sh]->use[0].id_tmod]->gmd_at( SchHD[i_sh]->use[0].id_mod ).mod_Name();
	       	grpmod[SchHD[i_sh]->use[0].id_tmod]->gmd_det( SchHD[i_sh]->use[0].id_mod );
	       	grpmod[SchHD[i_sh]->use[0].id_tmod]->gmd_del( n_mod );
		SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
	    }
	    dlclose(SchHD[i_sh]->hd);
	    SchHD[i_sh]->hd = NULL;
	    TSYS::WResRelease(hd_res);
	    return;
	}
    }
    TSYS::WResRelease(hd_res);
    throw TError("%s: SO %s no avoid!",o_name,name.c_str());
}

bool TModSchedul::CheckAuto( const string &name) const
{
    if( m_am_list.size() == 1 && m_am_list[0] == "*") return(true);
    else 
	for( unsigned i_au = 0; i_au < m_am_list.size(); i_au++)
	    if( name == SYS->FixFName( m_am_list[i_au] ) ) return(true);
    return(false);
}

void TModSchedul::ListSO( vector<string> &list )
{  
    TSYS::RResRequest(hd_res);    
    list.clear();
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	list.push_back( SchHD[i_sh]->name );
    TSYS::RResRelease(hd_res);
}

SHD TModSchedul::SO( string name )
{
    TSYS::RResRequest(hd_res);    
    name = SYS->FixFName(name);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    TSYS::RResRelease(hd_res);
	    return *SchHD[i_sh];
	}
    TSYS::RResRelease(hd_res);
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}

void TModSchedul::Load( string name, int dest, bool full)
{
    vector<string> files;

    ScanDir( name, files, true );
    for(unsigned i_f = 0; i_f < files.size(); i_f++)
    {
	unsigned i_sh;
	bool st_auto = CheckAuto(files[i_f]);
    	for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	    if( SchHD[i_sh]->name == files[i_f] ) break;
	if(i_sh < SchHD.size())
	{
	    if(st_auto) DetSO(files[i_f]);
	}
	RegSO(files[i_f]);	
	if(st_auto) 
	{
	    try{ AttSO(files[i_f],full,dest); }
	    catch( TError err ){ Owner().m_put("SYS",MESS_WARNING,"%s:%s",s_name,err.what().c_str()); }
	}
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModSchedul::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    XMLNode *c_nd;
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Module sheduler subsystem"));
    //a_ms
    c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Subsystem control"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Path to shared libs(modules)"));
    c_nd->get_child(1)->set_attr(dscr,Mess->I18N("List of auto conected shared libs(modules)"));
    c_nd->get_child(2)->set_attr(dscr,Mess->I18N("Options help"));
}

void TModSchedul::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_ms" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "mod_path" )    ctr_opt_setS( opt, m_mod_path );
	else if( t_id == "mod_auto" )
	    for( unsigned i_a=0; i_a < m_am_list.size(); i_a++ )
		ctr_opt_setS( opt, m_am_list[i_a], i_a );
	else if( t_id == "g_help" ) ctr_opt_setS( opt, opt_descr() );       
    }
} 

void TModSchedul::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_ms" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "mod_path" ) m_mod_path = ctr_opt_getS( opt );
	else if( t_id == "mod_auto" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)
    	    {
    		XMLNode *t_c = opt->get_child(i_el);
    		if( t_c->get_name() == "el")
    		{
    		    if(t_c->get_attr("do") == "add")      
			m_am_list.push_back(t_c->get_text());
		    else if(t_c->get_attr("do") == "ins") 
			m_am_list.insert(m_am_list.begin()+atoi(t_c->get_attr("id").c_str()),t_c->get_text()); 
		    else if(t_c->get_attr("do") == "edit") 
			m_am_list[atoi(t_c->get_attr("id").c_str())] = t_c->get_text();  
    		    else if(t_c->get_attr("do") == "del") 
			m_am_list.erase(m_am_list.begin()+atoi(t_c->get_attr("id").c_str()));
    		}
    	    }
    }
}

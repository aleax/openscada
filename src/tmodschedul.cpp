#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>

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

TModSchedul::TModSchedul( TKernel *app ) : m_stat(false), owner(app)
{
    hd_res = SYS->ResCreate();
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
    SYS->WResRequest(hd_res);    
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
    SYS->WResRelease(hd_res);    
    
    SYS->ResDelete(hd_res);
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
    Mess->put("DEBUG",MESS_DEBUG,"%s: Thread <%d>!",shed->o_name,getpid() );
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
		
		shed->Load(shed->owner->ModPath,-1,true);
		for(unsigned i_gm=0; i_gm < shed->grpmod.size(); i_gm++)
		    shed->Load(shed->grpmod[i_gm]->gmd_ModPath(),i_gm,true);
	    }
	} catch(TError err){ Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
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

void TModSchedul::CheckCommandLine(  )
{
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
    Load(Owner().ModPath,-1,false);
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
    	Mess->put("DEBUG",MESS_INFO,"Scan dir <%s> !", Path.c_str());
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
    	Mess->put("DEBUG",MESS_DEBUG,"Scan dir <%s> ok !", Path.c_str());
#endif    
	
    } while(id != (int)string::npos);
}

bool TModSchedul::CheckFile( const string &name, bool new_f ) const
{
    struct stat file_stat;
    string NameMod;

    stat(name.c_str(),&file_stat);

    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return(false);
    if( access(name.c_str(),F_OK|R_OK|X_OK) != 0 )      return(false);
    NameMod=name;
    
    void *h_lib = dlopen(name.c_str(),RTLD_GLOBAL|RTLD_LAZY);
    if(h_lib == NULL)
    {
        Mess->put("SYS",MESS_WARNING,"%s: SO %s error: %s !",o_name,name.c_str(),dlerror());
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
    
    SYS->WResRequest(hd_res);    
    stat(name.c_str(),&file_stat);
    unsigned i_sh;
    for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
       	if( SchHD[i_sh]->name == name ) 
	{
	    SchHD[i_sh]->m_tm = file_stat.st_mtime;   
	    SYS->WResRelease(hd_res);
	    return(i_sh);    
	}
    SchHD.push_back( new SHD );	
    SchHD[i_sh]->hd   = NULL;   
    SchHD[i_sh]->m_tm = file_stat.st_mtime;   
    SchHD[i_sh]->name = name;       
    SYS->WResRelease(hd_res);
    
    return(i_sh);    
}

void TModSchedul::UnregSO( const string &name )
{
    SYS->WResRequest(hd_res);    
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    if( SchHD[i_sh]->hd ) DetSO( name );
	    delete SchHD[i_sh];
	    SchHD.erase(SchHD.begin()+i_sh);
	    SYS->WResRelease(hd_res);
	    return;
	}
    SYS->WResRelease(hd_res);
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}
    
void TModSchedul::AttSO( const string &name, bool full, int dest )
{
    SYS->WResRequest(hd_res);    
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    if( SchHD[i_sh]->hd ) 
	    {
	    	SYS->WResRelease(hd_res);
		throw TError("%s: SO <%s> already attached!",o_name,name.c_str());	    
	    }
	    
	    void *h_lib = dlopen(name.c_str(),RTLD_GLOBAL|RTLD_LAZY);	    
	    if( !h_lib )
	    {
	    	SYS->WResRelease(hd_res);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());	    
	    }
	    
	    //Connect to module function
	    SAtMod (*module)( int );
	    (void *)module = dlsym(h_lib,"module");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
	    	SYS->WResRelease(hd_res);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());
	    }    
	    
	    //Connect to attach function	    
	    TModule *(*attach)( SAtMod &, string );
	    (void *)attach = dlsym(h_lib,"attach");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
	    	SYS->WResRelease(hd_res);
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
			    Mess->put("SYS",MESS_WARNING,"%s:%s: for type <%s> no support module version: %d!",
				o_name,AtMod.name.c_str(),AtMod.type.c_str(),AtMod.t_ver);
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
				Mess->put("SYS",MESS_WARNING,"%s: Attach module <%s> error!",o_name,AtMod.name.c_str());
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
			Mess->put("SYS",MESS_WARNING,"%s: Module %s already avoid!",o_name,AtMod.name.c_str());		    
		    }
		    if(dest >= 0) break;
		}
	    }
	    if(add_mod == 0) dlclose(h_lib);	    
	    else SchHD[i_sh]->hd = h_lib;
	    SYS->WResRelease(hd_res);
	    return;
	}
    SYS->WResRelease(hd_res);
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}

void TModSchedul::DetSO( const string &name )
{
    SYS->WResRequest(hd_res);    
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
	    SYS->WResRelease(hd_res);
	    return;
	}
    }
    SYS->WResRelease(hd_res);
    throw TError("%s: SO %s no avoid!",o_name,name.c_str());
}

bool TModSchedul::CheckAuto( const string &name) const
{
    if( Owner().auto_m_list.size() == 1 && Owner().auto_m_list[0] == "*") return(true);
    else 
	for( unsigned i_au = 0; i_au < Owner().auto_m_list.size(); i_au++)
	    if( name == SYS->FixFName( Owner().auto_m_list[i_au] ) ) return(true);
    return(false);
}

void TModSchedul::ListSO( vector<string> &list )
{  
    SYS->RResRequest(hd_res);    
    list.clear();
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	list.push_back( SchHD[i_sh]->name );
    SYS->RResRelease(hd_res);
}

SHD TModSchedul::SO( string name )
{
    SYS->RResRequest(hd_res);    
    name = SYS->FixFName(name);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    SYS->RResRelease(hd_res);
	    return *SchHD[i_sh];
	}
    SYS->RResRelease(hd_res);
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
	    catch( TError err ){ Mess->put("SYS",MESS_WARNING,"%s: %s",o_name,err.what().c_str()); }
	}
    }
}


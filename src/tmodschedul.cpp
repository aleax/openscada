#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>

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

TModSchedul::TModSchedul( TKernel *app ) : work(false), owner(app)
{

}

TModSchedul::~TModSchedul(  )
{
    DeinitAll();
    work=false;
    sleep(1);
}

void TModSchedul::StartSched( )
{ 
    pthread_attr_t      pthr_attr;
    
    work=true;    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,TModSchedul::SchedTask,this);
    pthread_attr_destroy(&pthr_attr);
}

void *TModSchedul::SchedTask(void *param)
{
    TModSchedul  *shed = (TModSchedul *)param;
//    setenv("_","OpenScada: test",1);
//    Owner().SetTaskTitle("TEST");
    
    do 
    {	
   	shed->Load(shed->owner->ModPath,-1);
       	for(unsigned i_gm=0; i_gm < shed->grpmod.size(); i_gm++)
    	    shed->Load(shed->grpmod[i_gm]->gmd_ModPath(),i_gm);
	shed->CheckOptFile();	    

	sleep(10);
    } while(shed->work==true);

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
    Load(Owner().ModPath,-1);
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	Load(grpmod[i_gm]->gmd_ModPath(),i_gm);
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

void TModSchedul::Load( string path, int dest)
{
    string Mod, Mods;
    int ido, id=-1;

    ScanDir( path, Mods );
    do
    {
        ido=id+1; id = Mods.find(",",ido);
        Mod=Mods.substr(ido,id-ido);
        if(Mod.size()<=0) continue;
        if(CheckFile((char *)Mod.c_str(),true) == true) 
	    AddShLib((char *)Mod.c_str(),dest);
    } while(id != (int)string::npos);
}

void TModSchedul::ScanDir( const string & Paths, string & Mods )
{
    string NameMod, Path;
    char   buf[256];           //!!!!

    int ido, id=-1;
    do
    {
        ido=id+1; id = Paths.find(",",ido);

        dirent *scan_dirent;
        Path=Paths.substr(ido,id-ido);
        if(Path.size() <= 0) continue;
	// Convert to absolutly path
        getcwd(buf,sizeof(buf));
        if(chdir(Path.c_str()) != 0) continue;
        Path=buf;
        getcwd(buf,sizeof(buf));
        chdir(Path.c_str());
        Path=buf;

#if OSC_DEBUG
        Mess->put(0,"Open dir <%s> !", Path.c_str());
#endif
        DIR *IdDir = opendir(Path.c_str());
        if(IdDir == NULL) continue;

        while((scan_dirent = readdir(IdDir)) != NULL)
        {
            NameMod=Path+"/"+scan_dirent->d_name;
	    if(Owner().allow_m_list.size())
	    {
		unsigned i;
		for(i=0; i < Owner().allow_m_list.size(); i++)
		    if(Owner().allow_m_list[i] == scan_dirent->d_name) break;
		if(i == Owner().allow_m_list.size()) continue;
	    }
	    else
	    {
		unsigned i;
		for(i=0; i < Owner().deny_m_list.size(); i++)
		    if(Owner().deny_m_list[i] == scan_dirent->d_name) break;
		if(i < Owner().deny_m_list.size()) continue;		
	    }
            if(CheckFile((char *)NameMod.c_str(),false) != true) continue;
            if(Mods.find(NameMod) == string::npos ) Mods=Mods+NameMod+",";
        }
        closedir(IdDir);
    } while(id != (int)string::npos);
}
	    
bool TModSchedul::CheckFile(char * name, bool new_f)
{
    struct stat file_stat;
    string NameMod;

    stat(name,&file_stat);

    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return(false);
    if( access(name,F_OK|R_OK|X_OK) != 0 )      return(false);
    NameMod=name;

    
    void *h_lib = dlopen(name,RTLD_GLOBAL|RTLD_LAZY);
    if(h_lib == NULL)
    {
        Mess->put(2, "File %s error: %s !",name,dlerror());
        return(false);
    }
    else dlclose(h_lib);
    
    if(new_f)
	for(unsigned i_sh=0; i_sh < SchHD.size(); i_sh++)
	    if(SchHD[i_sh].path == name && SchHD[i_sh].modif == file_stat.st_mtime) 
		return(false);

    return(true);
}

int TModSchedul::AddShLib( char *name, int dest )
{
    struct stat file_stat;
    string NameTMod;
    TModule *LdMod;
    int n_mod, add_mod, id;

    if( CheckFile(name,true) != true ) return(0);
    
    //Find updates share lib and delete shared lib's modules
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if(SchHD[i_sh].path == name)
	{
	    try{ grpmod[SchHD[i_sh].use[0].id_tmod]->gmd_DelM( SchHD[i_sh].use[0].id_mod );	}
	    catch(...){ }
	    UnRegMod_ShLb( SchHD[i_sh].use[0].id_tmod, SchHD[i_sh].use[0].id_mod );
	    i_sh =0; 
	}
    
    void *h_lib = dlopen(name,RTLD_GLOBAL|RTLD_LAZY);
    TModule *(*attach)(char *, int);
    (void *)attach = dlsym(h_lib,"attach");
    if(dlerror() != NULL)
    {
        Mess->put(2, "File %s error: %s !",name,dlerror());
        dlclose(h_lib);
        return(0);
    }
    
    n_mod=0, add_mod=0;
    while((LdMod = (attach)(name, n_mod++ )) != NULL )
    {
        NameTMod = LdMod->mod_info("Type");
	if(dest < 0)
	{
	    for( unsigned i_grm=0; i_grm < grpmod.size(); i_grm++)
		if(NameTMod == grpmod[i_grm]->gmd_NameTMod())
		{ 
		    id = grpmod[i_grm]->gmd_AddM(LdMod);
		    if(id >= 0)
		    {
    			RegMod_ShLb(h_lib, name, file_stat.st_mtime, i_grm, id );
			add_mod++;
		    }
		    break;
		}
	}
	else
	{
	    if(NameTMod == grpmod[dest]->gmd_NameTMod())
	    { 
		id = grpmod[dest]->gmd_AddM(LdMod);
		if(id >= 0)
		{
		    RegMod_ShLb(h_lib, name, file_stat.st_mtime, dest, id );
		    add_mod++;
		}
	    } 
	}
    }
    if(add_mod == 0) dlclose(h_lib);

    return(add_mod);
}

int TModSchedul::RegMod_ShLb(const void* hd, char *path, time_t modif, int id_tmod, int id_mod )
{
    SUse use_t = { id_tmod, id_mod };
    //Add to alredy registry share lib
    for(unsigned i=0; i < SchHD.size(); i++)
        if(SchHD[i].path == path)
        {
            for(unsigned i_use=0; i_use < SchHD[i].use.size(); i_use++)
		if(SchHD[i].use[i_use].id_tmod == id_tmod && SchHD[i].use[i_use].id_mod == id_mod)
		    return(i);
	    SchHD[i].use.push_back(use_t);
            return(i);
        }
    //Regystry new share lib
    SHD hd_t;
    hd_t.hd = (void *)hd;
    hd_t.use.push_back(use_t);
    hd_t.modif = modif;
    hd_t.path  = path;
    SchHD.push_back( hd_t );

    return( SchHD.size() - 1 );
}

int TModSchedul::UnRegMod_ShLb(int id_tmod, int id_mod)
{
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	for(unsigned i_use=0; i_use < SchHD[i_sh].use.size(); i_use++)
	    if(SchHD[i_sh].use[i_use].id_tmod == id_tmod && SchHD[i_sh].use[i_use].id_mod == id_mod)
	    {
		SchHD[i_sh].use.erase(SchHD[i_sh].use.begin()+i_use);
		if(SchHD[i_sh].use.size() == 0)
		{ 
		    dlclose(SchHD[i_sh].hd);
		    SchHD.erase(SchHD.begin()+i_sh);
		    return(0);
		}
	    }
    return(-1);
}

void TModSchedul::CheckOptFile( )
{
    static string cfg_fl;
    static struct stat f_stat;
    
    struct stat f_stat_t;
    bool   up = false;

    if(cfg_fl == SYS->CfgFile())
    {
	stat(cfg_fl.c_str(),&f_stat_t);
	if( f_stat.st_mtime != f_stat_t.st_mtime ) up = true;
    }
    else up = true;
    if(cfg_fl.size()==0)
    {
    	cfg_fl = SYS->CfgFile();
	stat(cfg_fl.c_str(),&f_stat);
	return;
    }
    cfg_fl = SYS->CfgFile();
    stat(cfg_fl.c_str(),&f_stat);
    if(up == true)
    {
	Owner().UpdateOpt();
	UpdateOpt();
	UpdateOptMod();
    }    
}



#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>

#include "tapplication.h"
#include "tmessage.h"
#include "./moduls/gener/tmodule.h"
#include "tgrpmodule.h"



TGRPModule::TGRPModule( char *NameT ) : NameType(NameT), DirPath("")
{

}

TGRPModule::~TGRPModule(  )
{

}

int TGRPModule::LoadAll(const string & Paths )
{
    string Mod, Mods;
    int ido, id=-1;

    ScanDir( Paths, Mods );
    do 
    {
	ido=id+1; id = Mods.find(",",ido);
	Mod=Mods.substr(ido,id-ido);
	if(Mod.size()<=0) continue;
	AddShLib((char *)Mod.c_str());
    } while(id<string::npos); 

    return(0);
}

int TGRPModule::InitAll( )
{
    for(int i=0;i<Moduls.size();i++) 
	if(Moduls[i]->stat == GRM_ST_OCCUP) Moduls[i]->modul->init(NULL);    
}


int TGRPModule::List( string & moduls )
{
    moduls.erase();
    for(int i=0;i<Moduls.size();i++) 
	if(Moduls[i]->stat == GRM_ST_OCCUP) moduls=moduls+Moduls[i]->name+',';
    return(0);
}

/*
int TGRPModule::RegFunc( string NameFunc, void * addr, string SrcModName, string NameMod )
{

}
*/
/*
int TGRPModule::PutCom(char * NameMod, string command )
{

}

int TGRPModule::PutCom(int idMod, string command )
{

}
*/

int TGRPModule::AddShLib( char *name )
{
    struct stat file_stat;
    string NameTMod;
    TModule *LdMod;
    int n_mod, add_mod, id;

    if( CheckFile(name) != true ) return(0);

    void *h_lib = dlopen(name,RTLD_GLOBAL|RTLD_LAZY);
    TModule *(*attach)(char *, int);
    (void *)attach = dlsym(h_lib,"attach");
    if(dlerror() != NULL)  
    { 
    	App->Mess->put(2, "File %s error: %s !",name,dlerror());
	dlclose(h_lib); 
	return(0); 
    }
    n_mod=0, add_mod=0;	
    while((LdMod = (attach)(name, n_mod++ )) != NULL )
    {
	LdMod->info("NameType",NameTMod);
	if(NameTMod != NameType) { delete LdMod; continue; }
	if((id=AddM(LdMod)) >= 0) 
	{
	    Moduls[id]->id_hd = RegHDShLb(h_lib, name, file_stat.st_mtime );
//	    App->Mess->put(0, "TEST!");
	    add_mod++;
	}
    }
    return(add_mod);
}

// Add modul 

int TGRPModule::AddM( TModule *modul )
{
    string NameMod, NameTMod;
    
    //---  Check names and version ---

    modul->info("NameType",NameTMod);
    modul->info("NameModul",NameMod);
    for(int i=0;i < Moduls.size(); i++)
    {
	if(Moduls[i]->stat == GRM_ST_FREE) continue;
	if( Moduls[i]->name == NameMod )
	{
	    int major, major1, minor, minor1;
	    modul->Version(major,minor);
    	    Moduls[i]->modul->Version(major1,minor1);

	    if(major>major1 || (major==major1 && minor > minor1))
	    {
//Пересмотреть для вызова функции Upgrade или при запуске проверять состояние модуля
//чтоб обновлять не инициализированный модуль автоматом
		delete Moduls[i]->modul;
		if( Moduls[i]->id_hd >= 0 ) FreeHDshLb(Moduls[i]->id_hd);

		Moduls[i]->name  = NameMod;
		Moduls[i]->id_hd = -1;
		Moduls[i]->modul = modul;
		Moduls[i]->stat  = GRM_ST_OCCUP; 
#if debug 
		App->Mess->put(0, "Update modul is ok!");
#endif	
		return(i);
	    }
	}
    }

    int i;
    for( i=0 ;i < Moduls.size(); i++)
	if(Moduls[i]->stat == GRM_ST_FREE ) break;
    if(i == Moduls.size()) Moduls.push_back(new SModul);
    Moduls[i]->name  = NameMod;
    Moduls[i]->modul    = modul;
    Moduls[i]->resource = 10;    //in future well be assigned a modul's
    Moduls[i]->access   = 0;
    Moduls[i]->id_hd    = -1;
    Moduls[i]->stat     = GRM_ST_OCCUP; 
#if debug 
    App->Mess->put(0, "Add modul %s is ok! Type %s .",NameMod.c_str(),NameTMod.c_str());
#endif	
    return(i);
}

int TGRPModule::DelM( int hd )
{
    if(hd >= Moduls.size() || Moduls[hd]->stat == GRM_ST_FREE ) return(-1);
    
    if( Moduls[hd]->id_hd >= 0 ) FreeHDshLb( Moduls[hd]->id_hd );
    Moduls[hd]->stat = GRM_ST_FREE;
    return(0);
}

int TGRPModule:: RegHDShLb(const void* hd, char *path, time_t modif )
{
    for(int i=0; i < SchHD.size(); i++) 
	if(SchHD[i]->hd == hd)
	{ 
	    SchHD[i]->use++;
	    return(i);
	}
    int i;
    for( i=0;i < SchHD.size(); i++ )
	if(SchHD[i]->use <= 0) break;
    if(i == SchHD.size()) 
    {
	SchHD.push_back(new SHD);
	SchHD[i]->use = 0;
    }

    SchHD[i]->hd = (void *)hd;
    SchHD[i]->use++;
    SchHD[i]->modif = modif;
    SchHD[i]->path.assign(path);
}

int TGRPModule::FreeHDshLb(int id)
{
    if(id >= SchHD.size() || SchHD[id]->use <= 0 || SchHD[id]->hd==NULL) return(-1);
    if(--(SchHD[id]->use) == 0) dlclose(SchHD[id]->hd);
    return(0);
}

void TGRPModule::ScanDir( const string & Paths, string & Mods )
{
    string NameMod, Path;
    
    int ido, id=-1;
    do 
    {
	ido=id+1; id = Paths.find(",",ido);

	dirent *scan_dirent;
	Path=Paths.substr(ido,id-ido);
	if(Path.size()<=0) continue;
#if debug 
    	App->Mess->put(0, "%s: Open dir <%s> !",NameType,Path.c_str());
#endif
	DIR *IdDir = opendir(Path.c_str());
	if(IdDir == NULL) continue;
	
	while(scan_dirent=readdir(IdDir)) 
	{
   	    NameMod.assign(Path+"/"+scan_dirent->d_name);
	    if(CheckFile((char *)NameMod.c_str()) != true) continue;
	    if(Mods.find(NameMod) == string::npos ) Mods=Mods+NameMod+",";
	}
	closedir(IdDir);
    } while(id<string::npos); 
}
   
bool TGRPModule::CheckFile(char * name)
{
    struct stat file_stat;
    string NameMod;

    stat(name,&file_stat);
    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return(false);
    if( access(name,F_OK|R_OK|X_OK) != 0 )      return(false);
    NameMod.assign(name);
    if(NameMod.find("OpenScadaServ") != string::npos)  return(false);

    void *h_lib = dlopen(name,RTLD_GLOBAL|RTLD_LAZY);
    if(h_lib == NULL)
    { 
    	App->Mess->put(2, "File %s error: %s !",name,dlerror());
	return(false);  
    }
    else dlclose(h_lib);

    return(true);
}

int TGRPModule::name_to_id(string & name)
{
    for(int i=0; i<Moduls.size(); i++)
    {
	if( Moduls[i]->stat == GRM_ST_FREE ) continue;
	if( Moduls[i]->name == name )        return(i);
    }
    return(-1);
}

int TGRPModule::MUse(unsigned int id)
{
    if(id >= Moduls.size() || Moduls[id]->stat != GRM_ST_OCCUP || Moduls[id]->resource <= 0 ) return(-1);
    Moduls[id]->access++; Moduls[id]->resource--; 
    return(0);    
}

int TGRPModule::MUse(unsigned int id, char * func, void (TModule::**offptr)())
{
    if(id >= Moduls.size() || Moduls[id]->stat != GRM_ST_OCCUP || Moduls[id]->resource <= 0 ) return(-1);
    if(Moduls[id]->modul->GetFunc(func, offptr) == MOD_ERR)   return(-1);
    Moduls[id]->access++; Moduls[id]->resource--; 
    
    return(0);    
}

int TGRPModule::MFree(unsigned int id)
{
    if(id >= Moduls.size() || Moduls[id]->stat != GRM_ST_OCCUP ) return(-1);
    Moduls[id]->access--; Moduls[id]->resource++; 
    return(0);    
}


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
	AddM((char *)Mod.c_str());
    } while(id<string::npos); 

    return(0);
}

int TGRPModule::InitAll( )
{
    for(int i=0;i<Moduls.size();i++) Moduls[i]->modul->init();    
}


string TGRPModule::List(  )
{

}


int TGRPModule::RegFunc( string NameFunc, void * addr, string SrcModName, string NameMod )
{

}


int TGRPModule::PutCom(char * NameMod, string command )
{

}

int TGRPModule::PutCom(int idMod, string command )
{

}

bool TGRPModule::AddM(char *name)
{
    bool kz;
    struct stat file_stat;
    string NameTMod, NameMod, NameMod1;

    if( CheckFile(name) != true ) return(false);
    NameMod.assign(name);

    void *h_lib = dlopen(name,RTLD_GLOBAL|RTLD_LAZY);
    TModule *(*attach)(char *name);  
    (void *)attach = dlsym(h_lib,"attach");
    if(dlerror() != NULL)  
    { 
    	App->Mess->put(2, "File %s error: %s !",name,dlerror());
	dlclose(h_lib); 
	return(false); 
    }
    TModule *LdMod = (attach)(name);
    LdMod->info("NameType",NameTMod);
    if(NameTMod.find(NameType) == string::npos) 
    { delete LdMod; dlclose(h_lib); return(false); }

//---  Check names and version ---
    LdMod->info("NameModul",NameMod);
    for(int i=0;i < Moduls.size(); i++)
    {
	Moduls[i]->modul->info("NameModul",NameMod1);
	if(NameMod1.find(NameMod) != string::npos)
	{
	    int major, major1, minor, minor1;
	    LdMod->Version(major,minor);
	    Moduls[i]->modul->Version(major1,minor1);

	    if(major>major1 || (major==major1 && minor > minor1))
	    {
//Пересмотреть для вызова функции Upgrade или при запуске проверять состояние модуля
//чтоб обновлять не инициализированный модуль автоматом
		delete Moduls[i]->modul;
		dlclose(Moduls[i]->hd);

		Moduls[i]->modul = LdMod;
		Moduls[i]->hd    = h_lib;
		Moduls[i]->path  = new string(name);
		Moduls[i]->modif = file_stat.st_mtime;
#if debug 
	    	App->Mess->put(0, "Update modul is ok!");
#endif	
		return(false);
	    }
	}
    }
//-------------- Test --------------
//    int (TModule::*OpenBD)(string name );
//    int (TModule::*CloseBD)(int hd);
//    LdMod->GetFunc("OpenBD",  (void (TModule::**)()) &OpenBD);
//    LdMod->GetFunc("CloseBD", (void (TModule::**)()) &CloseBD);
//    (LdMod->*CloseBD)( (LdMod->*OpenBD)("BDfile") );
//----------------------------------
    SModul *Modul= new SModul;
    Modul->modul = LdMod;
    Modul->hd    = h_lib;
    Modul->path = new string(name);
    Modul->resource = 0;
    Modul->access   = 0;
    Modul->modif    = file_stat.st_mtime;
    Moduls.push_back(Modul);
#if debug 
    App->Mess->put(0, "Add modul %s is ok! Type %s .",name,NameTMod.c_str());
#endif	
    return(true);
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
    string NameMod;
    
    for(int i=0; i<Moduls.size(); i++)
    {
	Moduls[i]->modul->info("NameModul",NameMod);
	if(NameMod.compare(name) == 0) return(i);
    }
    return(-1);
}


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
    //struct sched_param  prior;

    
    //==== Test ====
    Mess->put(1,"***** Begin test block from <void TModSchedul::StartSched( )> *****");   
    //Owner().Special().gmd_DelM( 2 );
    
    //------------------- Test MySQL BD -----------------------
    int t_hd = -1;
    try
    {
	//t_hd = Owner().BD().OpenTable("my_sql","server.diya.org;roman;;oscada;3306;/var/lib/mysql/mysql.sock;","generic");    
	t_hd = Owner().BD().OpenTable("my_sql",";;;oscada;;/var/lib/mysql/mysql.sock;","generic",true);    
    }catch(TError error)
    { Mess->put(1,"Open table error: %s",error.what().c_str()); }
    Mess->put(1,"Open table hd = %d",t_hd);
    string val = Owner().BD().at_tbl(t_hd).GetCodePage( );
    Mess->put(1,"table val = %s",val.c_str());
    Owner().BD().CloseTable(t_hd);    
    //---------------- Configs element's test ----------------
    try
    {
	vector<string> list_el;
	Owner().Param()[Owner().Param().NameToHd("TEST_VirtualC")]->at()->cf_ListEl(list_el);
        // Owner().Param->at( Owner().Param->NameToHd("TEST_VirtualC") )->at()->cf_ListEl(list_el);
	Mess->put(1,"Config Elements: %d",list_el.size());
	for(unsigned i=0; i< list_el.size(); i++)
	    Mess->put(1,"Element: %s",list_el[i].c_str());
    } catch(TError error) 
    { 
	Mess->put(1,"Error: %s",error.what().c_str());   
    }
    //---------------- Values element's test ----------------
    try
    {
	STime tm = {0,0};
	vector<string> list_el;
	Owner().Param()[Owner().Param().NameToHd("TEST_VirtualC")]->at()->vl_Elem()->vle_List(list_el);
	Mess->put(1,"Elements: %d",list_el.size());
	Owner().Param()[Owner().Param().NameToHd("TEST_VirtualC")]->at()->vl_SetI(0,30,tm);
	Mess->put(1,"Max Scale %f!",Owner().Param()[Owner().Param().NameToHd("TEST_VirtualC")]->at()->vl_GetR(0,tm,V_MAX));
	Mess->put(1,"Min Scale %f!",Owner().Param()[Owner().Param().NameToHd("TEST_VirtualC")]->at()->vl_GetR(0,tm,V_MIN));
	for(unsigned i=0; i< list_el.size(); i++)
            Mess->put(1,"Element: %s: %f",list_el[i].c_str(),Owner().Param()[Owner().Param().NameToHd("TEST_VirtualC")]->at()->vl_GetR(i,tm));
    } catch(TError error) 
    {      
	Mess->put(1,"Error: %s",error.what().c_str());   
    }    
    
    vector<string> list_ct,list_c,list_pt,list_pc;
    //----------------- Socket's test ----------------------------
    char *buf = (char *)malloc(200);
    try
    {
	int len;
	string comm = "time";
    	len = Owner().Transport().at_out(Owner().Transport().NameOutToId("TCP2"))->IOMess((char *)comm.c_str(),comm.size(),buf,199,1);
       	buf[len] = 0; Mess->put(1,"TCP Put <%s>. Get: <%s>",comm.c_str(),buf);    	 
	len = Owner().Transport().at_out(Owner().Transport().NameOutToId("UNIX2"))->IOMess((char *)comm.c_str(),comm.size(),buf,199,1);
       	buf[len] = 0; Mess->put(1,"UNIX Put <%s>. Get: <%s>",comm.c_str(),buf);       	
    	len = Owner().Transport().at_out(Owner().Transport().NameOutToId("UDP2"))->IOMess((char *)comm.c_str(),comm.size(),buf,199,1);
       	buf[len] = 0; Mess->put(1,"UDP Put <%s>. Get: <%s>",comm.c_str(),buf);       	
    } catch(TError error) { Mess->put(1,"Error sock: %s",error.what().c_str()); }
    free(buf);
    //Owner().Controller->AddContr("test3","virtual_v1","virt_c");
    //Owner().Controller->at("test3")->Add("ANALOG","TEST_VirtualC",-1);
    //Owner().Controller->at("test3")->Del("ANALOG","TEST_VirtualC");
    //Owner().Controller->DelContr("test3");
    //Owner().Controller->UpdateBD();    
    //Owner().Transport->UpdateBD();    
    /*
    Owner().Controller->List(list_ct);
    Mess->put(1,"Controller types: %d",list_ct.size());
    for(int i=0; i < list_ct.size(); i++)
    {
	try
	{
    	    Mess->put(1,"Controller type: <%s>",list_ct[i].c_str());

    	    Owner().Controller->at_tp(list_ct[i])->ListTpPrm(list_pt);
    	    Mess->put(1,"Types param's: %d",list_pt.size());
    	    for(int ii=0; ii < list_pt.size(); ii++)
		Mess->put(1,"Type: <%s>",list_pt[ii].c_str());

	    Owner().Controller->at_tp(list_ct[i])->List(list_c);
	    Mess->put(1,"Controllers: %d",list_c.size());
	    for(int ii=0; ii < list_c.size(); ii++)
	    {
		Mess->put(1,"Controller: <%s>",list_c[ii].c_str());
		for(int i_pt=0; i_pt < list_pt.size(); i_pt++)
		{
		    Owner().Controller->at(list_c[ii])->List(list_pt[i_pt],list_pc);
		    Mess->put(1,"%s Parameters: %d",list_pt[i_pt].c_str(),list_pc.size());
		    for(int iii=0; iii < list_pc.size(); iii++)
			Mess->put(1,"Parameter: <%s>",list_pc[iii].c_str());
		}
	    }
	}
	catch(TError err){ }
    }
    
    //---------------- All parameter's list ----------------
    Owner().Param->List(list_pc);
    Mess->put(1,"Params: %d",list_pc.size());
    for(unsigned i=0; i < list_pc.size(); i++)
	Mess->put(1,"Param: <%s>",list_pc[i].c_str());
    */	    
    Mess->put(1,"***** End test block from <void TModSchedul::StartSched( )> *****");
    //==============    
    
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
	grpmod[i_gm]->gmd_InitAll( );
}

void TModSchedul::DeinitAll(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmd_DeinitAll( );
}

void TModSchedul::StartAll(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
    {
	try{ grpmod[i_gm]->gmd_StartAll( ); }
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


#include <unistd.h>

#include <string>

#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tarhive.h"
#include "tparams.h"
#include "tparam.h"
#include "tcontroller.h"
#include "tcontrollers.h"
#include "ttipcontroller.h"
#include "tprocrequest.h"
#include "tprotocol.h"
#include "tspecial.h"
#include "tmodschedul.h"

TModSchedul::TModSchedul(  ) : work(false)
{

}

TModSchedul::~TModSchedul(  )
{

}

void TModSchedul::Start( )
{
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;

    //==== Test ====
    vector<string> list_ct,list_c,list_pt,list_pc;

    //App->Controller->AddContr("test3","virtual_v1","virt_c");
    //App->Controller->at("test3")->Add("ANALOG","TEST_VirtualC",-1);
    //App->Controller->at("test3")->Del("ANALOG","TEST_VirtualC");
    //App->Controller->DelContr("test3");
    //App->Controller->UpdateBD();    

    App->Controller->List(list_ct);
    App->Mess->put(1,"Controller types: %d",list_ct.size());
    for(int i=0; i < list_ct.size(); i++)
    {
	App->Mess->put(1,"Controller type: <%s>",list_ct[i].c_str());

	App->Controller->at_tp(list_ct[i])->ListTpPrm(list_pt);
	App->Mess->put(1,"Types param's: %d",list_pt.size());
	for(int ii=0; ii < list_pt.size(); ii++)
    	    App->Mess->put(1,"Type: <%s>",list_pt[ii].c_str());

	App->Controller->at_tp(list_ct[i])->List(list_c);
	App->Mess->put(1,"Controllers: %d",list_c.size());
	for(int ii=0; ii < list_c.size(); ii++)
	{
    	    App->Mess->put(1,"Controller: <%s>",list_c[ii].c_str());
	    //App->Controller->PutCntrComm("LOAD_CFG",list_c[ii]);
    	    for(int i_pt=0; i_pt < list_pt.size(); i_pt++)
    	    {
		App->Controller->at(list_c[ii])->List(list_pt[i_pt],list_pc);
	    	App->Mess->put(1,"%s Parameters: %d",list_pt[i_pt].c_str(),list_pc.size());
		for(int iii=0; iii < list_pc.size(); iii++)
		{
		    App->Mess->SconvOut("KOI8-U",list_pc[iii]);
		    App->Mess->put(1,"Parameter: <%s>",list_pc[iii].c_str());
		}
	    }
	}
    }

    App->Param->List(list_pc);
    App->Mess->put(1,"Params: %d",list_pc.size());
    for(int i=0; i < list_pc.size(); i++)
    {
	App->Mess->SconvOut("KOI8-U",list_pc[i]);
	App->Mess->put(1,"Param: <%s>",list_pc[i].c_str());
    }
    //==============
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    //pthread_attr_setschedparam(&pthr_attr,&prior);
    pthread_create(&pthr_tsk,&pthr_attr,TModSchedul::SchedTask,NULL);
    pthread_attr_destroy(&pthr_attr);

    work=true;    
}

void *TModSchedul::SchedTask(void *param)
{
    do
    {	
#if debug
	App->Mess->put(1,"Call scheduler!");
#endif
	string NameMod;
	if(App->ModSchedul->FindNewMod(NameMod)==true)
	    App->ModSchedul->UpdateMod(NameMod);
	sleep(10);
    } while(App->ModSchedul->work==true);
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

void TModSchedul::UpdateMod(string & Mods )
{

}

bool TModSchedul::FindNewMod(string & Mods )
{
    string Mod, Path;
    int ido, id=-1;

    Path.erase(); Path=Path+App->ModPath+","+App->BD->DirPath;
    App->BD->ScanDir( Path, Mods );
    Path.erase(); Path=Path+App->ModPath+","+App->Arhive->DirPath;
    App->Arhive->ScanDir( Path, Mods );
    Path.erase(); Path=Path+App->ModPath+","+App->Controller->DirPath;
    App->Controller->ScanDir( Path, Mods );
    Path.erase(); Path=Path+App->ModPath+","+App->ProcRequest->DirPath;
    App->ProcRequest->ScanDir( Path, Mods );
    Path.erase(); Path=Path+App->ModPath+","+App->Protocol->DirPath;
    App->Protocol->ScanDir( Path, Mods );
    Path.erase(); Path=Path+App->ModPath+","+App->Special->DirPath;
    App->Special->ScanDir( Path, Mods );

#if debug
    App->Mess->put(1,"All moduls: %s!",Mods.c_str());
#endif

/*    do 
    {
	ido=id+1; id = Mods.find(";",ido);
	Mod=Mods.substr(ido,id-ido);
	if(Mod.size()<=0) continue;
	AddM(Mod.c_str());
    } while(id<string::npos); 
*/
    return(true);
}



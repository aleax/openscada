#include <unistd.h>

#include <string>

#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tarhive.h"
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

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
//    pthread_attr_setschedparam(&pthr_attr,&prior);
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
    Path.erase(); Path=Path+App->ModPath+","+App->TipController->DirPath;
    App->TipController->ScanDir( Path, Mods );
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



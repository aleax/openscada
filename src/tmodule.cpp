
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "tmodule.h"


TModule::TModule( ) : stat(SMOD_PRESENT)
{

}

TModule::~TModule(  )
{
}

int TModule::init( )
{
#if debug 
    string Nm;
    App->Mess->put(1, "=====================================");
    info("NameModul",Nm);
    App->Mess->put(1, "=Name: %s !",Nm.c_str());
    info("NameType",Nm);
    App->Mess->put(1, "=Type name: %s !",Nm.c_str());
    info("NameFile",Nm);
    App->Mess->put(1, "=Name file: %s !",Nm.c_str());
    info("Version",Nm);
    App->Mess->put(1, "=Version: %s !",Nm.c_str());
    info("Autors",Nm);
    App->Mess->put(1, "=Autors: %s !",Nm.c_str());
    info("DescrMod",Nm);
    App->Mess->put(1, "=Description: %s !",Nm.c_str());
    info("ListExpFunc",Nm);
    App->Mess->put(1, "=Export Functions: %s !",Nm.c_str());
    App->Mess->put(1, "=====================================");
#endif
}

int TModule::deinit( )
{
    stat=SMOD_PRESENT;

    return(MOD_NO_ERR);
}

int TModule::GetFunc( string NameFunc, void (TModule::**offptr)() )
{
    for(int i=0; i < NExpFunc; i++)
    	if(NameFunc.find(ExpFunc[i].NameFunc) != string::npos)
	{ 
	    *offptr = ExpFunc[i].ptr;
	    return(MOD_NO_ERR); 
	}
    return(MOD_ERR);
}

int TModule::PutCommand( string command )
{
#if debug 
    App->Mess->put(1, "Command: %s !",command.c_str());
#endif
}
}

int TModule::info( const string & name, string & info )
{
    int cnt=0;
    if( name.find("NameModul")!= string::npos ) { info=info+NameModul; if(cnt++) info=info+","; }
    if( name.find("NameFile") != string::npos ) { info=info+FileName;  if(cnt++) info=info+","; }
    if( name.find("NameType") != string::npos ) { info=info+NameType;  if(cnt++) info=info+","; }
    if( name.find("Version")  != string::npos ) { info=info+Vers;      if(cnt++) info=info+","; }
    if( name.find("Autors")   != string::npos ) { info=info+Autors;    if(cnt++) info=info+","; }
    if( name.find("DescrMod") != string::npos ) { info=info+DescrMod;  if(cnt++) info=info+","; }
    if( name.find("Status")   != string::npos )   
    {
	switch(stat)
	{
	    case 0: info+="PRESENT "; break;
	    case 1: info+="READY ";   break;
	    case 2: info+="TEST ";    break;
	    case 3: info+="RUN ";     break;
	    default: info+="ERROR ";  break;
	}
	if(cnt++) info=info+",";
    }   
    if(name.find("ListExpFunc") != string::npos )
    {
	for(int i=0; i < NExpFunc; i++) info=info+ ExpFunc[i].NameFunc+" ";
	if(cnt++) info=info+",";
    }
    if(name.find("PrototipExpFunc") != string::npos )
    {
	for(int i=0; i < NExpFunc; i++)
	    if(name.find(ExpFunc[i].NameFunc) != string::npos) info=info+ ExpFunc[i].prototip+" ";
	if(cnt++) info=info+",";
    }
    if(name.find("DesriptExpFunc") != string::npos )
    {
	for(int i=0; i < NExpFunc; i++)
	    if(name.find(ExpFunc[i].NameFunc) != string::npos) info=info+ ExpFunc[i].descript+" ";
	if(cnt++) info=info+",";
    }
}

void TModule::Version( int & mayor, int & minor )
{
    string inf;
//    (*infoM)("Version",inf);
    info("Version",inf);
    mayor = atoi(inf.substr(0,inf.find(".")).c_str());
    minor = atoi(inf.substr(inf.find(".")+1,inf.size()).c_str());
}


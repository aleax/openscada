
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>

#include "terror.h"
#include "tapplication.h"
#include "tmessage.h"
#include "tmodule.h"

const char *TModule::o_name = "TModule";

TModule::TModule( ) : stat(SMOD_PRESENT), FileName(""), NameModul(""), NameType(""), Vers(""),
		    Autors(""), DescrMod(""), License(""), ExpFunc(NULL), NExpFunc(0)
{

}

TModule::~TModule(  )
{
}

void TModule::init( void *param )
{
#if OSC_DEBUG 
    string Nm;
    App->Mess->put(1, "-------------------------------------");
    info("NameModul",Nm);
    App->Mess->put(1, "| Name: %s",Nm.c_str());
    info("NameType",Nm);
    App->Mess->put(1, "| Type name: %s",Nm.c_str());
    info("NameFile",Nm);
    App->Mess->put(1, "| Name file: %s",Nm.c_str());
    info("Version",Nm);
    App->Mess->put(1, "| Version: %s",Nm.c_str());
    info("Autors",Nm);
    App->Mess->put(1, "| Autors: %s",Nm.c_str());
    info("DescrMod",Nm);
    App->Mess->put(1, "| Description: %s",Nm.c_str());
    info("ListExpFunc",Nm);
    App->Mess->put(1, "| Export Functions: %s",Nm.c_str());
    info("License",Nm);
    App->Mess->put(1, "| License: %s",Nm.c_str());
    App->Mess->put(1, "-------------------------------------");
#endif
    stat=SMOD_READY;
}

void TModule::deinit( )
{
    stat=SMOD_PRESENT;
}

void TModule::GetFunc( string NameFunc, void (TModule::**offptr)() )
{
    for(int i=0; i < NExpFunc; i++)
    	if(NameFunc.find(ExpFunc[i].NameFunc) != string::npos)
	{ 
	    if(ExpFunc[i].resource <= 0) throw TError("%s: no function %s resource!",o_name,NameFunc.c_str());
	    *offptr = ExpFunc[i].ptr;
	    ExpFunc[i].resource--;
	    ExpFunc[i].access++;	    
	    return; 
	}
    throw TError("%s: no function %s in module!",o_name,NameFunc.c_str());        
}

void TModule::FreeFunc( string NameFunc )
{
    for(int i=0; i < NExpFunc; i++)
    	if(NameFunc.find(ExpFunc[i].NameFunc) != string::npos)
	{
	    if(ExpFunc[i].access <= 0) throw TError("%s: no requested function %s!",o_name,NameFunc.c_str());
	    ExpFunc[i].resource++;
	    ExpFunc[i].access--;
	    return; 
	}
    throw TError("%s: no function %s in module!",o_name,NameFunc.c_str());        
}

void TModule::info( const string & name, string & info )
{
    info.clear();
    int cnt=0;
    if( name=="NameModul" )      info=NameModul;
    else if( name=="NameFile" )  info=FileName;
    else if( name=="NameType" )  info=NameType;
    else if( name=="Version" )   info=Vers;
    else if( name=="Autors" )    info=Autors;
    else if( name=="DescrMod" )  info=DescrMod;
    else if( name=="License" )   info=License;
    else if( name=="Status" )  
	switch(stat)
	{
	    case 0:  info="PRESENT"; break;
	    case 1:  info="READY";   break;
	    case 2:  info="TEST";    break;
	    case 3:  info="RUN ";    break;
	    default: info="ERROR ";  break;
	}
    else if(name=="ListExpFunc" )
	for(int i=0; i < NExpFunc; i++) info=info+ExpFunc[i].NameFunc+" ";
    else  if(name=="PrototipExpFunc" )
	for(int i=0; i < NExpFunc; i++)
	    if(name.find(ExpFunc[i].NameFunc) != string::npos) 
	    { info=info+ExpFunc[i].prototip+" "; break; }
    else if(name=="DesriptExpFunc")
	for(int i=0; i < NExpFunc; i++)
	    if(name.find(ExpFunc[i].NameFunc) != string::npos) 
	    { info=info+ExpFunc[i].descript+" "; break; }
}

void TModule::Version( int & mayor, int & minor )
{
    string inf;
//    (*infoM)("Version",inf);
    info("Version",inf);
    mayor = atoi(inf.substr(0,inf.find(".")).c_str());
    minor = atoi(inf.substr(inf.find(".")+1,inf.size()).c_str());
}


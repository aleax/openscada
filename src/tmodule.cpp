
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>

#include "terror.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"

const char *TModule::o_name = "TModule";
const char *TModule::l_info[] = 
    {"Modul","Type","File","Version","Autors","Descript","License","FuncList","FuncPrototip","FuncDescr"};

TModule::TModule( ) : stat(SMOD_PRESENT), FileName(""), NameModul(""), NameType(""), Vers(""),
		    Autors(""), DescrMod(""), License(""), ExpFunc(NULL), NExpFunc(0)
{

}

TModule::~TModule(  )
{
}

void TModule::mod_init( void *param )
{
#if OSC_DEBUG 
    Mess->put(1, "Init module <%s>!",NameModul);
    Mess->put(1, "-------------------------------------");
    vector<string> list;
    mod_info( list );
    for( int i_opt = 0; i_opt < list.size(); i_opt++)
    	Mess->put(1, "| %s: %s",list[i_opt].c_str(),mod_info(list[i_opt]).c_str());
    Mess->put(1, "-------------------------------------");
    /*    
    string Nm;
    Mess->put(1, "-------------------------------------");
    mod_info("NameModul",Nm);  Mess->put(1, "| Name: %s",Nm.c_str());
    mod_info("NameType",Nm);   Mess->put(1, "| Type name: %s",Nm.c_str());
    mod_info("NameFile",Nm);   Mess->put(1, "| Name file: %s",Nm.c_str());
    mod_info("Version",Nm);    Mess->put(1, "| Version: %s",Nm.c_str());
    mod_info("Autors",Nm);     Mess->put(1, "| Autors: %s",Nm.c_str());
    mod_info("DescrMod",Nm);   Mess->put(1, "| Description: %s",Nm.c_str());
    mod_info("ListExpFunc",Nm);Mess->put(1, "| Export Functions: %s",Nm.c_str());
    mod_info("License",Nm);    Mess->put(1, "| License: %s",Nm.c_str());
    Mess->put(1, "-------------------------------------");
    */
#endif
    stat=SMOD_READY;
}

void TModule::mod_deinit( )
{
    stat=SMOD_PRESENT;
}

void TModule::mod_GetFunc( string NameFunc, void (TModule::**offptr)() )
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

void TModule::mod_FreeFunc( string NameFunc )
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

void TModule::mod_info( vector<string> &list )
{
    for( int i_opt = 0; i_opt < sizeof(l_info)/sizeof(char *); i_opt++ )
    	list.push_back( l_info[i_opt] );
}

string TModule::mod_info( const string name )
{
    string info;
    
    if( name == l_info[0] )      info=NameModul;
    else if( name == l_info[1] ) info=NameType;
    else if( name == l_info[2] ) info=FileName;
    else if( name == l_info[3] ) info=Vers;
    else if( name == l_info[4] ) info=Autors;
    else if( name == l_info[5] ) info=DescrMod;
    else if( name == l_info[6] ) info=License;
    else if( name  == l_info[7] )
	for(int i=0; i < NExpFunc; i++) info=info+ExpFunc[i].NameFunc+" ";
    else if( name == l_info[8] )
	for(int i=0; i < NExpFunc; i++)
	    if(name.find(ExpFunc[i].NameFunc) != string::npos) 
	    { info=info+ExpFunc[i].prototip+" "; break; }
    else if( name == l_info[9] )
	for(int i=0; i < NExpFunc; i++)
	    if(name.find(ExpFunc[i].NameFunc) != string::npos) 
	    { info=info+ExpFunc[i].descript+" "; break; }
    
    return(info);
}


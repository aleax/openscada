
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
    {"Modul","Type","File","Version","Autors","Descript","License"};

TModule::TModule( ) : FileName(""), NameModul(""), NameType(""), Vers(""),
		    Autors(""), DescrMod(""), License(""), ExpFunc(NULL), NExpFunc(0), owner(NULL)
{

}

TModule::~TModule(  )
{
}

void TModule:: mod_connect( TGRPModule *owner ) 
{ 
    TModule::owner=owner;  
    mod_connect( );
}

void TModule::mod_connect(  )
{
    mod_CheckCommandLine( );
    mod_UpdateOpt( );    
#if OSC_DEBUG 
    Mess->put(1, "%s: connect module <%s>!",o_name,NameModul);
#endif    
}

void TModule::mod_ListFunc( vector<string> &list )
{
    list.clear();
    for(int i=0; i < NExpFunc; i++) 
	list.push_back(ExpFunc[i].NameFunc);
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

void TModule::mod_Func( string name, SFunc &func )
{
    for(int i=0; i < NExpFunc; i++)
    	if( name == ExpFunc[i].NameFunc )
	{ 
	    func.prototip = ExpFunc[i].prototip;
	    func.descript = ExpFunc[i].descript;
	    func.resource = ExpFunc[i].resource;
	    func.access   = ExpFunc[i].access;
	    return; 
	}
    throw TError("%s: no function %s in module!",o_name,name.c_str());        
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
    
    return(info);
}


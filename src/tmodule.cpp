
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <dlfcn.h>

#include "tsys.h"
#include "terror.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tgrpmodule.h"
#include "tmodule.h"

const char *TModule::o_name = "TModule";
const char *TModule::l_info[] = 
    {"Modul","Type","Source","Version","Autors","Descript","License"};
const char *TModule::i_cntr = 
	"<oscada_cntr>"
	"</oscada_cntr>";  

TModule::TModule( ) : 
	Source(""), NameModul(""), NameType(""), Vers(""), Autors(""), DescrMod(""), 
	License(""), ExpFunc(NULL), NExpFunc(0), owner(NULL), TContr( i_cntr ) 
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
#if OSC_DEBUG 
    m_put_s("DEBUG",MESS_DEBUG,"Connect module!");
#endif    

    mod_CheckCommandLine( );
    mod_UpdateOpt( );    
    
#if OSC_DEBUG 
    m_put_s("DEBUG",MESS_DEBUG,"Connect module ok!");
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
    else if( name == l_info[2] ) info=Source;
    else if( name == l_info[3] ) info=Vers;
    else if( name == l_info[4] ) info=Autors;
    else if( name == l_info[5] ) info=DescrMod;
    else if( name == l_info[6] ) info=License;
    
    return(info);
}

XMLNode *TModule::mod_XMLCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = Owner().gmd_XMLCfgNode()->get_child("module",i_k++);
	if( t_n->get_attr("id") == mod_Name() ) return( t_n );
    }
}

void TModule::mod_CheckCommandLine( )
{ 
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_INFO,"Read commandline options!");
#endif
};

void TModule::mod_UpdateOpt()
{ 
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_INFO,"Read config options!");
#endif    
};    

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModule::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(string("Module: "+mod_Name()));    
}

void TModule::ctr_din_get( XMLNode *opt )
{

}

void TModule::ctr_din_set( XMLNode *opt )
{

}

//==============================================================
//================== Message functions ========================
//==============================================================
void TModule::m_put( string categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    m_put_s( categ, level, str );
}

void TModule::m_put_s( string categ, int level, string mess )
{
    Owner().m_put_s( categ, level, mod_Name()+":"+mess );
}



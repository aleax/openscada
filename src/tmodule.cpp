
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <libintl.h>

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
    " <area id='a_info'>"
    " </area>"
    "</oscada_cntr>";  

TModule::TModule( ) : 
	Source(""), NameModul(""), NameType(""), Vers(""), Autors(""), DescrMod(""), 
	License(""), ExpFunc(NULL), NExpFunc(0), owner(NULL)
{

}

TModule::~TModule(  )
{

}

void TModule:: mod_connect( TGRPModule *owner ) 
{ 
    TModule::owner=owner;  
    lc_id = string("oscd_")+NameModul;
    bindtextdomain(lc_id.c_str(),LOCALEDIR);
    
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
    else if( name == l_info[1] ) info=I18Ns(NameType);
    else if( name == l_info[2] ) info=Source;
    else if( name == l_info[3] ) info=I18Ns(Vers);
    else if( name == l_info[4] ) info=I18Ns(Autors);
    else if( name == l_info[5] ) info=I18Ns(DescrMod);
    else if( name == l_info[6] ) info=I18Ns(License);
    
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
    char *dscr = "dscr";    
    vector<string> list;
    
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Module: ")+mod_Name());    
    XMLNode *x_ar = inf->get_child(0);
    x_ar->set_attr(dscr,Mess->I18N("Module information"));
    
    mod_info(list);
    for( int i_l = 0; i_l < list.size(); i_l++)
    {
        XMLNode *x_fld = x_ar->add_child("fld");
	x_fld->set_attr("id",list[i_l]);
	x_fld->set_attr("dscr",I18Ns(list[i_l]));
	x_fld->set_attr("acs","0444");
	x_fld->set_attr("tp","str");
    }
}

void TModule::ctr_din_get_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_info" )
       	ctr_opt_setS( opt, mod_info(ctr_path_l(a_path,1)) );       
}

void TModule::ctr_din_set_( string a_path, XMLNode *opt )
{

}

//================== Message functions ========================
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

//================== Translate functions ======================
char *TModule::I18N( char *mess )   
{ 
    char *rez = Mess->I18N(mess,(char *)(lc_id.c_str()));
    if( !strcmp(mess,rez) ) rez = Mess->I18N(mess);
    return( rez ); 
}

string TModule::I18Ns( string mess ) 
{ 
    return(I18N((char *)(mess.c_str())));
}



/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
    {"Modul","Name","Type","Source","Version","Autors","Descript","License"};

TModule::TModule( ) : 
	Source(""), mId(""), mName(""), mType(""), Vers(""), Autors(""), DescrMod(""), 
	License(""), ExpFunc(NULL), NExpFunc(0), owner(NULL)
{

}

TModule::~TModule(  )
{

}

void TModule::mod_connect( TGRPModule *owner ) 
{ 
    TModule::owner=owner;  
    lc_id = string("oscd_")+mId;
    bindtextdomain(lc_id.c_str(),LOCALEDIR);
    
    mod_connect( );
}

void TModule::mod_connect(  )
{
#if OSC_DEBUG 
    m_put_s("DEBUG",MESS_DEBUG,"Connect module!");
#endif    

    modCheckCommandLine( );
    modUpdateOpt( );    
    
#if OSC_DEBUG 
    m_put_s("DEBUG",MESS_DEBUG,"Connect module ok!");
#endif    
}

void TModule::modListFunc( vector<string> &list )
{
    list.clear();
    for(int i=0; i < NExpFunc; i++) 
	list.push_back(ExpFunc[i].NameFunc);
}

void TModule::modGetFunc( const string &NameFunc, void (TModule::**offptr)() )
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

void TModule::modFreeFunc( const string &NameFunc )
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

void TModule::modFunc( const string &name, SFunc &func )
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

void TModule::modInfo( vector<string> &list )
{
    for( int i_opt = 0; i_opt < sizeof(l_info)/sizeof(char *); i_opt++ )
    	list.push_back( l_info[i_opt] );
}

string TModule::modInfo( const string &name )
{
    string info;
    
    if( name == l_info[0] )      info=mId;
    else if( name == l_info[1] ) info=I18Ns(mName);
    else if( name == l_info[2] ) info=I18Ns(mType);
    else if( name == l_info[3] ) info=Source;
    else if( name == l_info[4] ) info=I18Ns(Vers);
    else if( name == l_info[5] ) info=I18Ns(Autors);
    else if( name == l_info[6] ) info=I18Ns(DescrMod);
    else if( name == l_info[7] ) info=I18Ns(License);
    
    return(info);
}

XMLNode *TModule::modXMLCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = Owner().gmdXMLCfgNode()->get_child("module",i_k++);
	if( t_n->get_attr("id") == modName() ) return( t_n );
    }
}

void TModule::modCheckCommandLine( )
{ 
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_INFO,"Read commandline options!");
#endif
};

void TModule::modUpdateOpt()
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
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='help'>"
	"  <area id='m_inf'/>"
	" </area>"
	"</oscada_cntr>";  
    char *dscr = "dscr";    
    
    vector<string> list;
    
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Module: ")+modName());    
    XMLNode *x_ar = inf->get_child(0);
    x_ar->set_attr(dscr,Mess->I18N("Help"));    
    x_ar = x_ar->get_child(0);    
    x_ar->set_attr(dscr,Mess->I18N("Module information"));
    
    modInfo(list);
    for( int i_l = 0; i_l < list.size(); i_l++)
    {
        XMLNode *x_fld = x_ar->add_child("fld");
	x_fld->set_attr("id",list[i_l]);
	x_fld->set_attr("dscr",I18Ns(list[i_l]));
	x_fld->set_attr("acs","0444");
	x_fld->set_attr("tp","str");
    }
    
}

void TModule::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,11) == "/help/m_inf" ) ctr_opt_setS( opt, modInfo(ctr_path_l(a_path,2)) ); 
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TModule::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TModule::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}    

AutoHD<TContr> TModule::ctr_at1( const string &a_path )
{
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}
	    

//================== Message functions ========================
void TModule::m_put( const string &categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    m_put_s( categ, level, str );
}

void TModule::m_put_s( const string &categ, int level, const string &mess )
{
    Owner().m_put_s( categ, level, modName()+":"+mess );
}

//================== Translate functions ======================
char *TModule::I18N( char *mess )   
{ 
    char *rez = Mess->I18N(mess,(char *)(lc_id.c_str()));
    if( !strcmp(mess,rez) ) rez = Mess->I18N(mess);
    return( rez ); 
}

string TModule::I18Ns( const string &mess ) 
{ 
    return(I18N((char *)(mess.c_str())));
}



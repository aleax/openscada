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
	License(""), ExpFunc(NULL), NExpFunc(0), m_owner(NULL)
{

}

TModule::~TModule(  )
{

}

void TModule::modConnect( TGRPModule *owner ) 
{ 
    m_owner=owner;  
    lc_id = string("oscd_")+mId;
    bindtextdomain(lc_id.c_str(),LOCALEDIR);
    
    modConnect( );
}

void TModule::modConnect(  )
{
#if OSC_DEBUG 
    mPutS("DEBUG",MESS_DEBUG,"Connect module!");
#endif    

    modCheckCommandLine( );
    modUpdateOpt( );    
    
#if OSC_DEBUG 
    mPutS("DEBUG",MESS_DEBUG,"Connect module ok!");
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

XMLNode *TModule::modCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = owner().gmdCfgNode()->childGet("module",i_k++);
	if( t_n->attr("id") == modName() ) return( t_n );
    }
}

void TModule::modCheckCommandLine( )
{ 
#if OSC_DEBUG
    mPutS("DEBUG",MESS_INFO,"Read commandline options!");
#endif
};

void TModule::modUpdateOpt()
{ 
#if OSC_DEBUG
    mPutS("DEBUG",MESS_INFO,"Read config options!");
#endif    
};    

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModule::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='help'>"
	"  <area id='m_inf'/>"
	" </area>"
	"</oscada_cntr>";  
    char *dscr = "dscr";    
    
    vector<string> list;
    
    inf->load( i_cntr );
    inf->text(Mess->I18N("Module: ")+modName());    
    XMLNode *x_ar = inf->childGet(0);
    x_ar->attr(dscr,Mess->I18N("Help"));    
    x_ar = x_ar->childGet(0);    
    x_ar->attr(dscr,Mess->I18N("Module information"));
    
    modInfo(list);
    for( int i_l = 0; i_l < list.size(); i_l++)
    {
        XMLNode *x_fld = x_ar->childAdd("fld");
	x_fld->attr("id",list[i_l]);
	x_fld->attr("dscr",I18Ns(list[i_l]));
	x_fld->attr("acs","0444");
	x_fld->attr("tp","str");
    }
    
}

void TModule::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,11) == "/help/m_inf" ) ctrSetS( opt, modInfo(pathLev(a_path,2)) ); 
    else throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TModule::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

AutoHD<TContr> TModule::ctrAt1( const string &a_path )
{
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}
	    

//================== Message functions ========================
void TModule::mPut( const string &categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    mPutS( categ, level, str );
}

void TModule::mPutS( const string &categ, int level, const string &mess )
{
    owner().mPutS( categ, level, modName()+":"+mess );
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



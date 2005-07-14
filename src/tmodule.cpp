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
	License(""), m_owner(NULL)
{

}

TModule::~TModule(  )
{
    //Clean export function list
    for(int i=0; i < m_efunc.size(); i++)
        delete m_efunc[i];
}

string TModule::modName()	
{ 
    return I18Ns(mName); 
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
    mPutS("DEBUG",TMess::Debug,"Connect module!");
#endif    

    //modCheckCommandLine( );
    //modUpdateOpt( );    
    
#if OSC_DEBUG 
    mPutS("DEBUG",TMess::Debug,"Connect module ok!");
#endif    
}

void TModule::modFuncList( vector<string> &list )
{
    list.clear();    
    for(int i=0; i < m_efunc.size(); i++)
        list.push_back(m_efunc[i]->prot);
}

TModule::ExpFunc &TModule::modFunc( const string &prot )
{
    for(int i=0; i < m_efunc.size(); i++)
	if( m_efunc[i]->prot == prot ) return *m_efunc[i];
    throw TError("%s: no function <%s> in module!",o_name,prot.c_str());        
}	

void TModule::modFunc( const string &prot, void (TModule::**offptr)() )
{
    *offptr = modFunc(prot).ptr;
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
	if( t_n->attr("id") == modId() ) return( t_n );
    }
}

string TModule::cfgNodeName()
{
    return owner().cfgNodeName()+modId()+"/";
}    

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModule::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {	
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Module: ")+modId());
	ctrMkNode("area",opt,a_path.c_str(),"/help",Mess->I18N("Help"));
	ctrMkNode("area",opt,a_path.c_str(),"/help/m_inf",Mess->I18N("Module information"));
    	
	modInfo(list);
	for( int i_l = 0; i_l < list.size(); i_l++)
	    ctrMkNode("fld",opt,a_path.c_str(),(string("/help/m_inf/")+list[i_l]).c_str(),I18Ns(list[i_l]),0444,0,0,"str");	
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path.substr(0,11) == "/help/m_inf" )	ctrSetS( opt, modInfo(TSYS::pathLev(a_path,2)) ); 
	else throw TError("(Module)Branch %s error",a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
	throw TError("(Module)Branch %s error",a_path.c_str());
}

AutoHD<TCntrNode> TModule::ctrAt1( const string &a_path )
{
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}
	    

//================== Message functions ========================
void TModule::mPut( const string &categ, TMess::Type level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    mPutS( categ, level, str );
}

void TModule::mPutS( const string &categ, TMess::Type level, const string &mess )
{
    owner().mPutS( categ, level, modId()+":"+mess );
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



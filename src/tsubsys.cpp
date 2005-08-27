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

#include <stdarg.h>
#include <unistd.h>

#include "tsys.h"
#include "tmess.h"
#include "tsubsys.h"

TSubSYS::TSubSYS( TSYS *app, char *id, char *name, bool modi ) : 
	TCntrNode(app),m_id(id), m_name(name), m_mod_sys(modi), m_mod(-1)
{
    if(subModule()) m_mod = grpAdd();
}

TSubSYS::~TSubSYS(  )
{
    nodeDelAll();
}

string TSubSYS::subName()
{
    return Mess->I18Ns(m_name);
}    

void TSubSYS::modAdd( TModule *modul )
{
    if( !subModule() ) throw TError(nodePath().c_str(),"No modules subsystem!");
    if( chldAvoid(m_mod,modul->modId()) ) return;
    chldAdd(m_mod,modul);
    modul->modConnect();    
#if OSC_DEBUG 
    Mess->put(nodePath().c_str(),TMess::Debug,"-------------------------------------");
    vector<string> list;
    modul->modInfo( list );
    for( unsigned i_opt = 0; i_opt < list.size(); i_opt++)
    	Mess->put(nodePath().c_str(),TMess::Debug,"| %s: %s",modul->I18N(list[i_opt].c_str()),modul->modInfo(list[i_opt]).c_str());
    Mess->put(nodePath().c_str(),TMess::Debug,"-------------------------------------");
#endif
}

void TSubSYS::modDel( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),"No modules subsystem!");
#if OSC_DEBUG 
    Mess->put((nodePath()+name).c_str(),TMess::Info,Mess->I18N("Disconnect modul!"));
#endif
    chldDel(m_mod,name); 
#if OSC_DEBUG 
    Mess->put((nodePath()+name).c_str(),TMess::Debug,Mess->I18N("Disconnect modul ok!"));
#endif
}

void TSubSYS::subLoad( ) 
{
    if( !subModule() )	return;
    vector<string> list;
    modList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
        modAt(list[i_m]).at().modLoad( ); 
}

void TSubSYS::subStart( ) 
{ 
    if( !subModule() )	return;
    vector<string> list;
    modList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
        modAt(list[i_m]).at().modStart( );
}

void TSubSYS::subStop( ) 
{ 
    if( !subModule() )	return;
    vector<string> list;
    modList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
        modAt(list[i_m]).at().modStop( );
}		

//==============================================================
//================== Controll functions ========================
//==============================================================
void TSubSYS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Subsystem: ")+subName());
	if( subModule() )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/mod",Mess->I18N("Modules"));
	    ctrMkNode("list",opt,a_path.c_str(),"/mod/br",Mess->I18N("Modules"),0555,0,0,"br")->
		attr_("idm","1")->attr_("mode","att")->attr_("br_pref","_");
	}
	ctrMkNode("area",opt,a_path.c_str(),"/help",Mess->I18N("Help"));
    }
    else if( cmd==TCntrNode::Get )
    {
	if( subModule() && a_path == "/mod/br" )
	{
	    vector<string> list;
	    modList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, modAt(list[i_a]).at().modName(), list[i_a].c_str() );         
	}
	else throw TError(nodePath().c_str(),"Branch <%s> error!",a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
	throw TError(nodePath().c_str(),"Branch <%s> error!",a_path.c_str());	
}

AutoHD<TCntrNode> TSubSYS::ctrAt1( const string &br )
{
    if( subModule() && br.substr(0,1)=="_")	return modAt( br.substr(1) );
    throw TError(nodePath().c_str(),"Branch <%s> error!",br.c_str());
}



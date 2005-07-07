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
#include "tkernel.h"
#include "tmessage.h"
#include "tgrpmodule.h"

TGRPModule::TGRPModule( TKernel *app, char *id, char *name ) : 
	m_id(id), m_name(name), m_owner(app)
{
    m_mod = grpAdd();
    nodeEn();
}

TGRPModule::~TGRPModule(  )
{
    delAll();
}

string TGRPModule::gmdName()
{
    return Mess->I18Ns(m_name);
}    

void TGRPModule::gmdAdd( TModule *modul )
{
    if( chldAvoid(m_mod,modul->modId()) ) return;
    chldAdd(m_mod,modul);
    modul->modConnect(this);    
#if OSC_DEBUG 
    mPutS("DEBUG",TMess::Debug,"-------------------------------------");
    vector<string> list;
    modul->modInfo( list );
    for( unsigned i_opt = 0; i_opt < list.size(); i_opt++)
    	mPut("DEBUG",TMess::Debug,"| %s: %s",list[i_opt].c_str(),modul->modInfo(list[i_opt]).c_str());
    mPutS("DEBUG",TMess::Debug,"-------------------------------------");
#endif
}

void TGRPModule::gmdDel( const string &name )
{
#if OSC_DEBUG 
    mPut("DEBUG",TMess::Info,"Disconnect modul <%s>!",name.c_str() );
#endif
    chldDel(m_mod,name); 
#if OSC_DEBUG 
    mPut("DEBUG",TMess::Debug,"Disconnect modul <%s> ok!",name.c_str() );
#endif
}

void TGRPModule::gmdLoad( ) 
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
        gmdAt(list[i_m]).at().modLoad( ); 
}

void TGRPModule::gmdStart( ) 
{ 
    vector<string> list;
    gmdList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
        gmdAt(list[i_m]).at().modStart( );
}

void TGRPModule::gmdStop( ) 
{ 
    vector<string> list;
    gmdList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
        gmdAt(list[i_m]).at().modStop( );
}		

string TGRPModule::cfgNodeName()
{
    return owner().cfgNodeName()+gmdId()+"/";
}

XMLNode *TGRPModule::gmdCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = owner().cfgNode()->childGet("section",i_k++);
	if( t_n->attr("id") == gmdId() ) return( t_n );
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TGRPModule::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Subsystem: ")+gmdName());
	ctrMkNode("area",opt,a_path.c_str(),"/mod",Mess->I18N("Modules"));
	ctrMkNode("list",opt,a_path.c_str(),"/mod/br",Mess->I18N("Modules"),0555,0,0,"br")->
	    attr_("idm","1")->attr_("mode","att")->attr_("br_pref","_");
	ctrMkNode("area",opt,a_path.c_str(),"/help",Mess->I18N("Help"));
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/mod/br" )
	{
	    vector<string> list;
	    gmdList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, gmdAt(list[i_a]).at().modName(), list[i_a].c_str() );         
	}
	else throw TError("(Module)Branch %s error!",a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
	throw TError("(Module)Branch %s error!",a_path.c_str());	
}

AutoHD<TCntrNode> TGRPModule::ctrAt1( const string &br )
{
    if(br.substr(0,1)=="_")	return gmdAt( br.substr(1) );
    throw TError("(Module)Branch %s error!",br.c_str());
}

//==============================================================
//================== Message functions ========================
//==============================================================
void TGRPModule::mPut( const string &categ, TMess::Type level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    mPutS( categ, level, str );
}

void TGRPModule::mPutS( const string &categ, TMess::Type level, const string &mess )
{
    owner().mPutS( categ, level, gmdName()+":"+mess );
}



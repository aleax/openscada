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

const char *TGRPModule::o_name = "TGRPModule";
 

TGRPModule::TGRPModule( TKernel *app, char *NameT ) : 
	nameType(NameT), DirPath(""), m_owner(app)
{
    m_mod = grpAdd();
    nodeEn();
}

TGRPModule::~TGRPModule(  )
{
    delAll();
}

string TGRPModule::name()
{
    return Mess->I18Ns(s_name);
}    

void TGRPModule::gmdAdd( TModule *modul )
{
    if( chldAvoid(m_mod,modul->modName()) ) return;
    chldAdd(m_mod,modul);
    modul->modConnect(this);    
#if OSC_DEBUG 
    mPutS("DEBUG",MESS_DEBUG,"-------------------------------------");
    vector<string> list;
    modul->modInfo( list );
    for( unsigned i_opt = 0; i_opt < list.size(); i_opt++)
    	mPut("DEBUG",MESS_DEBUG,"| %s: %s",list[i_opt].c_str(),modul->modInfo(list[i_opt]).c_str());
    mPutS("DEBUG",MESS_DEBUG,"-------------------------------------");
#endif
}

void TGRPModule::gmdDel( const string &name )
{
#if OSC_DEBUG 
    mPut("DEBUG",MESS_INFO,"Disconnect modul <%s>!",name.c_str() );
#endif
    chldDel(m_mod,name); 
#if OSC_DEBUG 
    mPut("DEBUG",MESS_DEBUG,"Disconnect modul <%s> ok!",name.c_str() );
#endif
}

void TGRPModule::gmdCheckCommandLineMods()
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
	gmdAt(list[i_m]).at().modCheckCommandLine( );
}

void TGRPModule::gmdUpdateOptMods()
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
	gmdAt(list[i_m]).at().modUpdateOpt();
}

XMLNode *TGRPModule::gmdCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = owner().cfgNode()->childGet("section",i_k++);
	if( t_n->attr("id") == gmdName() ) return( t_n );
    }
}

void TGRPModule::gmdCheckCommandLine( )
{
#if OSC_DEBUG
    mPutS("DEBUG",MESS_INFO,"Read commandline options!" );
#endif
}

void TGRPModule::gmdUpdateOpt()
{
#if OSC_DEBUG
    mPutS("DEBUG",MESS_INFO,"Read config options!");
#endif
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TGRPModule::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    char *i_cntr = 
	"<oscada_cntr>"
	" <area id='mod'>"
	"  <fld id='m_path' tp='str' acs='0660' dest='dir'/>"
	"  <list id='br' tp='br' acs='0555' mode='att' br_pref='_'/>"
        " </area>"
	" <area id='help'/>"
	"</oscada_cntr>"; 

    inf->load( i_cntr );
    inf->text(Mess->I18N("Subsystem: ")+name());    
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Modules"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Modules path"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Modules"));
    inf->childGet(1)->attr(dscr,Mess->I18N("Help"));
}

void TGRPModule::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( a_path == "/mod/m_path" )
    {
        if( cmd==TCntrNode::Get )	ctrSetS( opt, DirPath );
	else if( cmd==TCntrNode::Set )	DirPath = ctrGetS( opt );
    }
    else if( a_path == "/mod/br" && cmd==TCntrNode::Get )
    {
	vector<string> list;
	gmdList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );         
    }
}

AutoHD<TCntrNode> TGRPModule::ctrAt1( const string &br )
{
    if(br.substr(0,1)=="_")	return gmdAt( br.substr(1) );
    throw TError("<{%s}> Branch %s error!",__func__,br.c_str());
}

//==============================================================
//================== Message functions ========================
//==============================================================
void TGRPModule::mPut( const string &categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    mPutS( categ, level, str );
}

void TGRPModule::mPutS( const string &categ, int level, const string &mess )
{
    owner().mPutS( categ, level, gmdName()+":"+mess );
}



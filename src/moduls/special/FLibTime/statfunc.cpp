/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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


#include <tsys.h>

#include "timefnc.h"
#include "statfunc.h"

//============ Modul info! =====================================================
#define MOD_ID      "FLibTime"
#define MOD_NAME    "Time function's lib"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.5.0"
//==============================================================================

FLibTime::Lib *FLibTime::st_lib;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	FLibTime::Lib *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    FLibTime::st_lib = self_addr = new FLibTime::Lib( source );

	return ( self_addr );
    }
}

using namespace FLibTime;

//Complex1 functions library
Lib::Lib( string src )
{
    //== Set modul info! ============
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= "Roman Savochenko";
    mDescr  	= "Allow time static function library.";
    mLicense   	= "GPL";
    mSource    	= src;
    
    m_fnc = grpAdd("fnc_");
}

Lib::~Lib()
{

}

void Lib::postEnable( )
{
    TModule::postEnable( );
        
    //Reg functions
    if( !present("date") )      reg( new TmDate() );
    if( !present("time") )      reg( new TmTime() );
    if( !present("ctime") )	reg( new TmCtime() );
}

void Lib::modStart( )
{
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().start(true);
    run_st = true;	
}
	    
void Lib::modStop( )
{
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
        at(lst[i_l]).at().start(false);
    run_st = false;
}

void Lib::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> lst;
    
    switch(cmd)
    {
	case TCntrNode::Info:
	    TSpecial::cntrCmd_( a_path, opt, cmd );
	
	    ctrMkNode("list",opt,a_path.c_str(),"/prm/func",Mess->I18N("Functions"),0664,0,0,"br")->
    		attr_("idm","1")->attr_("br_pref","fnc_");
	    break;
	case TCntrNode::Get:
	    if( a_path == "/prm/func" )
    	    {
        	list(lst);
        	opt->childClean();
    		for( unsigned i_f=0; i_f < lst.size(); i_f++ )
            	    ctrSetS( opt, at(lst[i_f]).at().name(), lst[i_f].c_str() );
    	    }
	    else TSpecial::cntrCmd_( a_path, opt, cmd );
	    break;
	case TCntrNode::Set:
	    TSpecial::cntrCmd_( a_path, opt, cmd );
    }
}

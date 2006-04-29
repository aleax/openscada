
//OpenSCADA system module Special.FLibMath file: statfunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

#include "stdmath.h"
#include "statfunc.h"

//============ Modul info! =====================================================
#define MOD_ID      "FLibMath"
#define MOD_NAME    "Math function's lib"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.5.0"
//==============================================================================

FLibMath::Lib *FLibMath::st_lib;

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
	FLibMath::Lib *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    FLibMath::st_lib = self_addr = new FLibMath::Lib( source );

	return ( self_addr );
    }
}

using namespace FLibMath;

//Complex1 functions library
Lib::Lib( string src )
{
    //== Set modul info! ============
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= "Roman Savochenko";
    mDescr  	= "Allow mathematic static function library.";
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
    if( !present("acos") )      reg( new MathAcos() );
    if( !present("asin") )      reg( new MathAsin() );
    if( !present("atan") )      reg( new MathAtan() );
    if( !present("cos") )       reg( new MathCos() );
    if( !present("cosh") )      reg( new MathCosh() );
    if( !present("exp") )       reg( new MathExp() );
    if( !present("ln") )        reg( new MathLn() );
    if( !present("lg") )        reg( new MathLg() );
    if( !present("pow") )       reg( new MathPow() );
    if( !present("sin") )       reg( new MathSin() );
    if( !present("sinh") )      reg( new MathSinh() );
    if( !present("sqrt") )      reg( new MathSqrt() );
    if( !present("tan") )       reg( new MathTan() );
    if( !present("tanh") )      reg( new MathTanh() );
    if( !present("ceil") )      reg( new MathCeil() );
    if( !present("abs") )       reg( new MathAbs() );
    if( !present("floor") )     reg( new MathFloor() );
    if( !present("rand") )      reg( new MathRand() );
    if( !present("if") )        reg( new MathIf() );
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
	
	    ctrMkNode("list",opt,a_path.c_str(),"/prm/func",I18N("Functions"),0664,0,0,"br")->
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

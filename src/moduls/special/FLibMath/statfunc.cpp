
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

FLibMath::Lib *FLibMath::mod;

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
	    FLibMath::mod = self_addr = new FLibMath::Lib( source );

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

void Lib::postEnable( int flag )
{
    TModule::postEnable( flag );
    
    if( flag&TCntrNode::NodeRestore )	return;
    //Reg functions
    reg( new MathAcos() );
    reg( new MathAsin() );
    reg( new MathAtan() );
    reg( new MathCos() );
    reg( new MathCosh() );
    reg( new MathExp() );
    reg( new MathLn() );
    reg( new MathLg() );
    reg( new MathPow() );
    reg( new MathSin() );
    reg( new MathSinh() );
    reg( new MathSqrt() );
    reg( new MathTan() );
    reg( new MathTanh() );
    reg( new MathCeil() );
    reg( new MathAbs() );
    reg( new MathFloor() );
    reg( new MathRand() );
    reg( new MathIf() );
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

void Lib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TSpecial::cntrCmdProc(opt);
        ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),0444,"root","root",1,"list","/prm/func");
        ctrMkNode("list",opt,-1,"/prm/func",_("Functions"),0444,"root","root",3,"tp","br","idm","1","br_pref","fnc_");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/func" && ctrChkNode(opt) )
    {
        vector<string> lst;
        list(lst);
        for( unsigned i_f=0; i_f < lst.size(); i_f++ )
            opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
    }
    else TSpecial::cntrCmdProc(opt);
}

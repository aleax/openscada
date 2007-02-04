
//OpenSCADA system module Special.FLibComplex1 file: statfunc.cpp
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

#include "libcompl1.h"
#include "statfunc.h"

//============ Modul info! =====================================================
#define MOD_ID      "FLibComplex1"
#define MOD_NAME    "Complex1 function's lib"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.9.0"
//==============================================================================

FLibComplex1::Lib *FLibComplex1::mod;

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
	FLibComplex1::Lib *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    FLibComplex1::mod = self_addr = new FLibComplex1::Lib( source );

	return ( self_addr );
    }
}

using namespace FLibComplex1;

//Complex1 functions library
Lib::Lib( string src )
{    
    //== Set modul info! ============
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= "Roman Savochenko";
    mDescr  	= "Allow static function library Complex1 (SCADA Complex1 functions).";
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
    if( !present("digitBlock") )reg( new DigitBlock() );
    if( !present("sum") )       reg( new Sum() );
    if( !present("mult") )      reg( new Mult() );
    if( !present("multDiv") )   reg( new MultDiv() );
    if( !present("exp") )       reg( new Exp() );
    if( !present("pow") )       reg( new Pow() );
    if( !present("cond_lt") )	reg( new Cond1() );
    if( !present("cond_gt") )	reg( new Cond2() );
    if( !present("cond_full") ) reg( new Cond3() );
    if( !present("select") )    reg( new Select() );
    if( !present("increment") ) reg( new Increm() );
    if( !present("div") )       reg( new Divider() );
    if( !present("pid") )       reg( new PID() );
    if( !present("alarm") )     reg( new Alarm() );
    if( !present("flow") )      reg( new Flow() );
    if( !present("sum_mult") )  reg( new SumMult() );
    if( !present("sum_div") )   reg( new SumDiv() );
    if( !present("lag") )       reg( new Lag() );    
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
            opt->childAdd("el")->attr("id",lst[i_f])->text(at(lst[i_f]).at().name());
    }
    else TSpecial::cntrCmdProc(opt);
}

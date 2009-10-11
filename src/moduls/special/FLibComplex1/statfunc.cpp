
//OpenSCADA system module Special.FLibComplex1 file: statfunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2009 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"FLibComplex1"
#define MOD_NAME	"Complex1 function's lib"
#define MOD_TYPE	"Special"
#define VER_TYPE	VER_SPC
#define SUB_TYPE	"LIB"
#define VERSION		"1.0.4"
//*************************************************

FLibComplex1::Lib *FLibComplex1::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new FLibComplex1::Lib( source );
	return NULL;
    }
}

using namespace FLibComplex1;

//*************************************************
//* Lib: Complex1 functions library               *
//*************************************************
Lib::Lib( string src )
{
    //- Set modul info! -
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= "Roman Savochenko";
    mDescr	= "Allow static function library Complex1 (SCADA Complex1 functions).";
    mLicense	= "GPL";
    mSource	= src;

    mod		= this;

    m_fnc = grpAdd("fnc_");
}

Lib::~Lib( )
{

}

void Lib::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeRestore )	return;

    //> Reg functions
    reg( new DigitBlock() );
    reg( new Sum() );
    reg( new Mult() );
    reg( new MultDiv() );
    reg( new Exp() );
    reg( new Pow() );
    reg( new Cond1() );
    reg( new Cond2() );
    reg( new Cond3() );
    reg( new Select() );
    reg( new Increm() );
    reg( new Divider() );
    reg( new PID() );
    reg( new Alarm() );
    reg( new Flow() );
    reg( new SumMult() );
    reg( new SumDiv() );
    reg( new Lag() );

    //> Enable functions
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().setStart(true);
}

void Lib::modStart( )
{
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().setStart(true);
    run_st = true;
}

void Lib::modStop( )
{
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().setStart(false);
    run_st = false;
}

void Lib::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TSpecial::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),0444,"root","root",1,"idm","1");
	ctrMkNode("list",opt,-1,"/prm/func",_("Functions"),0444,"root","root",3,"tp","br","idm","1","br_pref","fnc_");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( (a_path == "/br/fnc_" || a_path == "/prm/func") && ctrChkNode(opt) )
    {
	vector<string> lst;
	list(lst);
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
    }
    else TSpecial::cntrCmdProc(opt);
}

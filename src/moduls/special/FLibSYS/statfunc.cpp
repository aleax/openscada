
//OpenSCADA system module Special.FLibSYS file: statfunc.cpp
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

#include "sysfnc.h"
#include "timefnc.h"
#include "xmlfnc.h"
#include "varchfnc.h"
#include "statfunc.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"FLibSYS"
#define MOD_NAME	"System API functions"
#define MOD_TYPE	"Special"
#define MOD_AUTOR	"Roman Savochenko"
#define MOD_DESCR	"Allow system API functions library of the user programming area."
#define MOD_LICENSE	"GPL"
#define VER_TYPE	VER_SPC
#define SUB_TYPE	"LIB"
#define VERSION		"0.9.0"
//*************************************************

FLibSYS::Lib *FLibSYS::mod;

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
	    return new FLibSYS::Lib( source );
	return NULL;
    }
}

using namespace FLibSYS;

//*************************************************
//* Lib: Complex1 functions library               *
//*************************************************
Lib::Lib( string src )
{
    //> Set modul info!
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= MOD_AUTOR;
    mDescr	= MOD_DESCR;
    mLicense	= MOD_LICENSE;
    mSource	= src;

    mod		= this;

    m_fnc = grpAdd("fnc_",true);
}

Lib::~Lib()
{

}

void Lib::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeRestore )	return;

    //> Reg functions
    reg( new sysCall() );
    reg( new dbReqSQL() );
    reg( new xmlNode() );
    reg( new xmlCntrReq() );
    reg( new vArh() );
    reg( new vArhBuf() );

    reg( new tmDate() );
    reg( new tmTime() );
    reg( new tmFStr() );
    reg( new tmStr2Tm() );
    reg( new tmCron() );

    reg( new messGet() );
    reg( new messPut() );

    reg( new strSize() );
    reg( new strSubstr() );
    reg( new strInsert() );
    reg( new strReplace() );
    reg( new strParse() );
    reg( new strParsePath() );
    reg( new strPath2Sep() );
    reg( new strEnc2HTML() );
    reg( new strEnc2Bin() );
    reg( new strDec4Bin() );
    reg( new real2str() );
    reg( new int2str() );
    reg( new str2real() );
    reg( new str2int() );

    reg( new floatSplitWord() );
    reg( new floatMergeWord() );
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
    //> Get page info
    if( opt->name() == "info" )
    {
	TSpecial::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),0444,"root","root",1,"idm","1");
	ctrMkNode("list",opt,-1,"/prm/func",_("Functions"),0444,"root","root",3,"tp","br","idm","1","br_pref","fnc_");
	return;
    }
    //> Process command to page
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

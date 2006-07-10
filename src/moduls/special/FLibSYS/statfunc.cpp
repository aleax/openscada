
//OpenSCADA system module Special.FLibSYS file: statfunc.cpp
/***************************************************************************
 *   Copyright (C) 2006-2006 by Roman Savochenko                           *
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


#include <resalloc.h>
#include <tsys.h>

#include "sysfnc.h"
#include "statfunc.h"

//============ Modul info! =====================================================
#define MOD_ID		"FLibSYS"
#define MOD_NAME    	"System API functions"
#define MOD_TYPE    	"Special"
#define MOD_AUTOR	"Roman Savochenko"
#define MOD_DESCR	"Allow time static function library."
#define MOD_LICENSE	"GPL"
#define VER_TYPE    	VER_SPC
#define SUB_TYPE    	"LIB"
#define VERSION     	"0.1.0"
//==============================================================================

FLibSYS::Lib *FLibSYS::mod;

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
	FLibSYS::Lib *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    FLibSYS::mod = self_addr = new FLibSYS::Lib( source );

	return ( self_addr );
    }
}

using namespace FLibSYS;

//Complex1 functions library
Lib::Lib( string src )
{
    //== Set modul info! ============
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= MOD_AUTOR;
    mDescr  	= MOD_DESCR;
    mLicense   	= MOD_LICENSE;
    mSource    	= src;
    
    m_fnc = grpAdd("fnc_");
    aval_res = ResAlloc::resCreate();
}

Lib::~Lib()
{
    ResAlloc::resDelete(aval_res);
}

void Lib::postEnable( )
{
    TModule::postEnable( );
        
    //Reg functions    
    if( !present("avalOpen") )	reg( new avalOpen() );
    if( !present("avalClose") )	reg( new avalClose() );
    if( !present("avalGetI") )	reg( new avalGetI() );    
    if( !present("avalGetR") )	reg( new avalGetR() );    
    if( !present("avalGetB") )	reg( new avalGetB() );    
    if( !present("avalGetS") )	reg( new avalGetS() );    
    if( !present("avalSetI") )	reg( new avalSetI() );    
    if( !present("avalSetR") )	reg( new avalSetR() );    
    if( !present("avalSetB") )	reg( new avalSetB() );    
    if( !present("avalSetS") )	reg( new avalSetS() );    
    if( !present("messPut") )	reg( new messPut() );
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

    varchFree( );	//Used value archives free
    	
    run_st = false;
}

int Lib::varchOpen( const string &inm )
{
    AutoHD<TVArchive> arch;
    ResAlloc res(aval_res,true);
    if( dynamic_cast<TVal *>(&SYS->nodeAt(inm,0,'.').at()) )
	arch = dynamic_cast<TVal&>(SYS->nodeAt(inm,0,'.').at()).arch();
    else if( dynamic_cast<TVArchive *>(&SYS->nodeAt(inm,0,'.').at()) )
        arch = SYS->nodeAt(inm,0,'.');
    if( arch.freeStat() ) return -1;
    int i_id;
    for( i_id = 0; i_id < aval_id_lst.size(); i_id++ )
	if( aval_id_lst[i_id].freeStat() ) 
	{
	    aval_id_lst[i_id] = arch; 
	    break;
	}
    if( i_id >= aval_id_lst.size() )
	aval_id_lst.push_back(arch);
	
    return i_id;	
}

void Lib::varchClose( int id )
{
    ResAlloc res(aval_res,true);
    if( id < aval_id_lst.size() )
	aval_id_lst[id].free();
}

AutoHD<TVArchive> Lib::varch( int id )
{
    AutoHD<TVArchive> rez;
    ResAlloc res(aval_res,false);
    if( id < aval_id_lst.size() )
	rez = aval_id_lst[id];
    return rez;
}

void Lib::varchFree( )
{
    ResAlloc res(aval_res,true);
    for( int i_id = 0; i_id < aval_id_lst.size(); i_id++ )
        if( !aval_id_lst[i_id].freeStat() )
	    aval_id_lst[i_id].free();
}

void Lib::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> lst;
    
    if( cmd == TCntrNode::Info )
    {
        TSpecial::cntrCmd_( a_path, opt, cmd );
	
	ctrMkNode("list",opt,-1,a_path.c_str(),"/prm/func",I18N("Functions"),0664,0,0,3,"tp","br","idm","1","br_pref","fnc_");
    }
    else if( cmd == TCntrNode::Get )
    {
	if( a_path == "/prm/func" )
    	{
    	    list(lst);
    	    opt->childClean();
    	    for( unsigned i_f=0; i_f < lst.size(); i_f++ )
            	ctrSetS( opt, at(lst[i_f]).at().name(), lst[i_f].c_str() );
    	}
	else TSpecial::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd == TCntrNode::Set )
	TSpecial::cntrCmd_( a_path, opt, cmd );
}

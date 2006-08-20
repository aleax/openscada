
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
#include "timefnc.h"
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
    vbf_res  = ResAlloc::resCreate();
}

Lib::~Lib()
{
    ResAlloc::resDelete(aval_res);
    ResAlloc::resDelete(vbf_res);
}

void Lib::postEnable( )
{
    TModule::postEnable( );
        
    //Reg functions    
    if( !present("varhOpen") )	reg( new varhOpen() );
    if( !present("varhClose") )	reg( new varhClose() );
    if( !present("varhBeg") )	reg( new varhBeg() );
    if( !present("varhEnd") )	reg( new varhEnd() );
    if( !present("varhGetVal") )reg( new varhGetVal() );
    if( !present("varhSetVal") )reg( new varhSetVal() );
    if( !present("vbufOpen") )	reg( new FLibSYS::vbufOpen() );
    if( !present("vbufClose") )	reg( new FLibSYS::vbufClose() );
    if( !present("vbufBeg") )	reg( new vbufBeg() );
    if( !present("vbufEnd") )	reg( new vbufEnd() );
    if( !present("vbufGetI") )	reg( new vbufGetI() );
    if( !present("vbufGetR") )	reg( new vbufGetR() );
    if( !present("vbufGetB") )	reg( new vbufGetB() );
    if( !present("vbufGetS") )	reg( new vbufGetS() );
    if( !present("vbufSetI") )	reg( new vbufSetI() );
    if( !present("vbufSetR") )	reg( new vbufSetR() );
    if( !present("vbufSetB") )	reg( new vbufSetB() );
    if( !present("vbufSetS") )	reg( new vbufSetS() );
    if( !present("messPut") )	reg( new messPut() );
    if( !present("tmDate") )    reg( new tmDate() );
    if( !present("tmTime") )    reg( new tmTime() );
    if( !present("tmCtime") )   reg( new tmCtime() );
    if( !present("tmStr2Tm") )	reg( new tmStr2Tm() );
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
    vbfFree( );		//Value buffers free
    	
    run_st = false;
}

int Lib::varchOpen( const string &inm )
{
    int i_id;
    
    AutoHD<TVArchive> arch;
    ResAlloc res(aval_res,true);
    try
    {
	if( dynamic_cast<TVal *>(&SYS->nodeAt(inm,0,'.').at()) )
	    arch = dynamic_cast<TVal&>(SYS->nodeAt(inm,0,'.').at()).arch();
	else if( dynamic_cast<TVArchive *>(&SYS->nodeAt(inm,0,'.').at()) )
    	    arch = SYS->nodeAt(inm,0,'.');
	if( arch.freeStat() ) return -1;
	for( i_id = 0; i_id < aval_id_lst.size(); i_id++ )
	    if( aval_id_lst[i_id].freeStat() ) 
	    {
		aval_id_lst[i_id] = arch; 
		break;
	    }
	if( i_id >= aval_id_lst.size() )
	    aval_id_lst.push_back(arch);
    }catch(TError err){	return -1; }
	
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

int Lib::vbufOpen( TFld::Type vtp, int isz, int ipr, bool ihgrd, bool ihres )
{
    ResAlloc res(vbf_res,true);
    
    TValBuf *vb = new TValBuf(vtp,isz,ipr,ihgrd,ihres); 
    if( !vb )	return -1;
    
    int i_id;
    for( i_id = 0; i_id < vbf_id_lst.size(); i_id++ )
        if( !vbf_id_lst[i_id] )
        {
	    vbf_id_lst[i_id] = vb;
            break;
        }
    if( i_id >= vbf_id_lst.size() )	vbf_id_lst.push_back(vb);
    
    return i_id;								        
}

void Lib::vbufClose( int id )
{
    ResAlloc res(vbf_res,true);
    if( id < vbf_id_lst.size() )	
    {
	delete vbf_id_lst[id];
	vbf_id_lst[id] = NULL;
    }
}

TValBuf *Lib::vbuf( int id )
{
    ResAlloc res(vbf_res,false);
    if( id < vbf_id_lst.size() )	
	return vbf_id_lst[id];
    return NULL;
}			

void Lib::vbfFree( )
{
    ResAlloc res(vbf_res,true);
    for( int i_id = 0; i_id < vbf_id_lst.size(); i_id++ )
	if( vbf_id_lst[i_id] )	delete vbf_id_lst[i_id];
    vbf_id_lst.clear();
}	

void Lib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TSpecial::cntrCmdProc(opt);
        ctrMkNode("grp",opt,-1,"/br/fnc_",Mess->I18N("Function"),0444,"root","root",1,"list","/prm/func");
        ctrMkNode("list",opt,-1,"/prm/func",I18N("Functions"),0444,"root","root",3,"tp","br","idm","1","br_pref","fnc_");
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

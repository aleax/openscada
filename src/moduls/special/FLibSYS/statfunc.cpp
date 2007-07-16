
//OpenSCADA system module Special.FLibSYS file: statfunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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

#include "sysfnc.h"
#include "timefnc.h"
#include "statfunc.h"

//============ Modul info! =====================================================
#define MOD_ID		"FLibSYS"
#define MOD_NAME    	"System API functions"
#define MOD_TYPE    	"Special"
#define MOD_AUTOR	"Roman Savochenko"
#define MOD_DESCR	"Allow system API functions library of the user programming area."
#define MOD_LICENSE	"GPL"
#define VER_TYPE    	VER_SPC
#define SUB_TYPE    	"LIB"
#define VERSION     	"0.3.0"
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
}

Lib::~Lib()
{

}

void Lib::postEnable( int flag )
{
    TModule::postEnable( flag );
    
    if( flag&TCntrNode::NodeRestore )	return;        
    //Reg functions    
    reg( new varhOpen() );
    reg( new varhBufOpen() );
    reg( new varhClose() );
    reg( new varhBeg() );
    reg( new varhEnd() );
    reg( new varhCopyBuf() );
    reg( new varhGetI() );
    reg( new varhGetR() );
    reg( new varhGetB() );
    reg( new varhGetS() );
    reg( new varhSetI() );
    reg( new varhSetR() );
    reg( new varhSetB() );
    reg( new varhSetS() );
    reg( new messPut() );
    reg( new tmDate() );
    reg( new tmTime() );
    reg( new tmCtime() );
    reg( new tmStr2Tm() );
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

    varchFree( );	//Used value archives and buffers free
    	
    run_st = false;
}

int Lib::varchOpen( const string &inm )
{
    int i_id;
    
    AutoHD<TVArchive> arch;
    ResAlloc res(varch_res,true);
    try
    {
	if( dynamic_cast<TVal *>(&SYS->nodeAt(inm,0,'.').at()) )
	    arch = dynamic_cast<TVal&>(SYS->nodeAt(inm,0,'.').at()).arch();
	else if( dynamic_cast<TVArchive *>(&SYS->nodeAt(inm,0,'.').at()) )
    	    arch = SYS->nodeAt(inm,0,'.');
	if( arch.freeStat() ) return -1;
	for( i_id = 0; i_id < varch_lst.size(); i_id++ )
	    if( !varch_lst[i_id].arch ) 
	    {
		varch_lst[i_id].arch = new AutoHD<TVArchive>(arch);
		varch_lst[i_id].isArch = true;
		break;
	    }
	if( i_id >= varch_lst.size() )
	{
	    SVarch varch_el;
	    varch_el.arch = new AutoHD<TVArchive>(arch);
	    varch_el.isArch = true;
	    varch_lst.push_back(varch_el);
	}
    }catch(TError err){	return -1; }
	
    return i_id;	
}

void Lib::varchClose( int id )
{
    ResAlloc res(varch_res,true);
    if( id >= 0 && id < varch_lst.size() && varch_lst[id].arch )
    {
	if(varch_lst[id].isArch)	delete varch_lst[id].arch;
	else delete varch_lst[id].buf;
	varch_lst[id].arch = NULL;
    }
}

bool Lib::isArch(int id)
{
    ResAlloc res(varch_res,false);
    if( id >= 0 && id < varch_lst.size() )	return varch_lst[id].isArch;
    return false;
}

AutoHD<TVArchive> Lib::varch( int id )
{
    AutoHD<TVArchive> rez;
    ResAlloc res(varch_res,false);
    if( id >= 0 && id < varch_lst.size() && varch_lst[id].isArch )
	rez = *varch_lst[id].arch;
    return rez;
}

void Lib::varchFree( )
{
    ResAlloc res(varch_res,true);
    for( int i_id = 0; i_id < varch_lst.size(); i_id++ )
        if( varch_lst[i_id].isArch )	delete varch_lst[i_id].arch;
        else delete varch_lst[i_id].buf;
    varch_lst.clear();    	
}

int Lib::varchBufOpen( TFld::Type vtp, int isz, int ipr, bool ihgrd, bool ihres )
{
    ResAlloc res(varch_res,true);
    
    TValBuf *vb = new TValBuf(vtp,isz,ipr,ihgrd,ihres); 
    if( !vb )	return -1;
    
    int i_id;
    for( i_id = 0; i_id < varch_lst.size(); i_id++ )
        if( !varch_lst[i_id].buf )
        {
	    varch_lst[i_id].buf = vb;
	    varch_lst[i_id].isArch = false;
            break;
        }
    if( i_id >= varch_lst.size() )
    {
        SVarch varch_el;
	varch_el.buf = vb;
        varch_el.isArch = false;
	varch_lst.push_back(varch_el);
    }
    
    return i_id;								        
}

TValBuf *Lib::vbuf( int id )
{
    ResAlloc res(varch_res,false);
    if( id >= 0 && id < varch_lst.size() && !varch_lst[id].isArch )
	return varch_lst[id].buf;
    return NULL;
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

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


#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include "freelib.h"
#include "freefunclibs.h"

//============ Modul info! =====================================================
#define MOD_ID      "freefunc"
#define MOD_NAME    "Free functions"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow free function libraries engine. User can create and modify function and libraries."
#define LICENSE     "GPL"
//==============================================================================

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
	FreeFunc::Libs *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new FreeFunc::Libs( source );

	return ( self_addr );
    }
}

using namespace FreeFunc;

//===================== Complex1 functions library =========================
Libs::Libs( string src ) : m_bd("","","vLibFunc")
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= src;
    
    parse_res = ResAlloc::resCreate();
}

Libs::~Libs()
{
    //stop();
    while( free_libs.size() )
    {
        owner().owner().func().unreg(free_libs[0]);
        free_libs.erase(free_libs.begin());
    }
    ResAlloc::resDelete(parse_res);
}

void Libs::modLoad( )
{
    loadBD();
}

void Libs::modConnect( )
{
    TModule::modConnect( );

    //Lib's db structure
    lb_el.fldAdd( new TFld("ID",I18N("ID"),T_STRING|F_KEY,"10") );
    lb_el.fldAdd( new TFld("NAME",I18N("Name"),T_STRING,"20") );
    lb_el.fldAdd( new TFld("DESCR",I18N("Description"),T_STRING,"300") );
    lb_el.fldAdd( new TFld("BD_TP",I18N("Data base type"),T_STRING,"20") );
    lb_el.fldAdd( new TFld("BD_NM",I18N("Data base name"),T_STRING,"20") );
    lb_el.fldAdd( new TFld("BD_TBL",I18N("Data base table"),T_STRING,"20") );

    //Function's structure
    fnc_el.fldAdd( new TFld("ID",I18N("ID"),T_STRING|F_KEY,"10") );
    fnc_el.fldAdd( new TFld("NAME",I18N("Name"),T_STRING,"50") );
    fnc_el.fldAdd( new TFld("DESCR",I18N("Description"),T_STRING,"300") );
    fnc_el.fldAdd( new TFld("FORMULA",I18N("Formula"),T_STRING,"1000") );

    //Function's IO structure
    fncio_el.fldAdd( new TFld("F_ID",I18N("Function ID"),T_STRING|F_KEY,"10") );
    fncio_el.fldAdd( new TFld("ID",I18N("ID"),T_STRING|F_KEY,"10") );
    fncio_el.fldAdd( new TFld("NAME",I18N("Name"),T_STRING,"50") );
    fncio_el.fldAdd( new TFld("TYPE",I18N("Type"),T_DEC,"1") );
    fncio_el.fldAdd( new TFld("MODE",I18N("Mode"),T_DEC,"1") );
    fncio_el.fldAdd( new TFld("DEF",I18N("Default value"),T_STRING,"20") );
    fncio_el.fldAdd( new TFld("VECT",I18N("Vector"),T_STRING,"10") );
    fncio_el.fldAdd( new TFld("HIDE",I18N("Hide"),T_BOOL,"1") );
    fncio_el.fldAdd( new TFld("POS",I18N("Position"),T_DEC,"3") );
}

bool Libs::avoid( const string &lib )
{
    for(int i_lb=0; i_lb < free_libs.size(); i_lb++ )
	if( free_libs[i_lb] == lib ) return true;
    return false;
}

TBDS::SName Libs::BD()
{
    return owner().owner().nameDBPrep(m_bd);
}

void Libs::loadBD()
{
    try
    {
	TConfig c_el(&elLib());
	AutoHD<TTable> tbl = owner().owner().db().open(BD());
	int fld_cnt = 0;
	while( owner().owner().db().dataSeek(tbl,cfgNodeName()+"lib/", fld_cnt++,c_el) )
        {
	    string l_id = c_el.cfg("ID").getS();
	    
	    int f_lb;
	    for( f_lb = 0; f_lb < free_libs.size(); f_lb++ )		
		if( free_libs[f_lb] == l_id )	break;		
	    if( f_lb >= free_libs.size() )
	    {
		Lib *lb = new Lib(l_id.c_str(),this);
		//*(TConfig *)lb = c_el;
    		owner().owner().func().reg(lb);
		free_libs.push_back(l_id);
	    }
	    ((Lib &)owner().owner().func().at(l_id).at()).load();
	}
	if(!tbl.freeStat())
	{
	    tbl.free();
	    owner().owner().db().close(BD());
	}	
    }catch( TError err ){ mPutS("SYS",TMess::Error,err.what()); }
}

void Libs::saveBD()
{   
    for( int l_id = 0; l_id < free_libs.size(); l_id++ )
	((Lib &)owner().owner().func().at(free_libs[l_id]).at()).save();
}  

void Libs::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TSpecial::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrInsNode("area",1,opt,a_path.c_str(),"/mod",I18N(MOD_NAME));
	ctrMkNode("area",opt,a_path.c_str(),"/mod/libs",I18N("Libraries"));
	ctrMkNode("list",opt,a_path.c_str(),"/mod/libs/lb",I18N("Libraries"),0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_lb_");
	ctrMkNode("comm",opt,a_path.c_str(),"/mod/load",Mess->I18N("Load from BD"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/mod/save",Mess->I18N("Save to BD"),0550);		    	
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/mod/libs/lb" )
	{
	    opt->childClean();
	    for( unsigned i_a=0; i_a < free_libs.size(); i_a++ )
		ctrSetS( opt, owner().owner().func().at(free_libs[i_a]).at().name(), free_libs[i_a].c_str() );
	}
	else TSpecial::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/mod/libs/lb" )
	{
	    if( opt->name() == "add" )
	    {
		owner().owner().func().reg(new Lib(opt->attr("id").c_str(),this,opt->text().c_str()));
		free_libs.push_back(opt->attr("id"));
	    }
	    else if( opt->name() == "del" )
	    {
		owner().owner().func().unreg(opt->attr("id"));
		for(int i_el = 0; i_el < free_libs.size(); i_el++)
		    if( free_libs[i_el] == opt->attr("id") )
		    {
			free_libs.erase(free_libs.begin()+i_el);
			break;
		    }
	    }
	}
	else if( a_path == "/mod/load" )	loadBD();
	else if( a_path == "/mod/save" )	saveBD();
	else TSpecial::cntrCmd_( a_path, opt, cmd );
    }
}

AutoHD<TCntrNode> Libs::ctrAt1( const string &a_path )
{
    if( a_path.substr(0,4) == "_lb_" )	return owner().owner().func().at(TSYS::strEncode(a_path.substr(4),TSYS::PathEl));
    else return TSpecial::ctrAt1(a_path);
}


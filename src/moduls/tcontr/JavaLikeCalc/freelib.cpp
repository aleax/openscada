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
#include <tmess.h>
#include "freefunc.h"
#include "virtual.h"
#include "freelib.h"

using namespace JavaLikeCalc;

//================ Functions library ==================
Lib::Lib( const char *id, TipContr *own, const char *name ) : 
    TConfig(&own->elLib()), TLibFunc(id), m_owner(own), 
    m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()), 
    m_bd_tp(cfg("BD_TP").getSd()), m_bd_nm(cfg("BD_NM").getSd()), m_bd_tbl(cfg("BD_TBL").getSd())
{
    cfg("ID").setS(id);
    m_bd_tbl = string("lib_")+id;
    m_name = name;
}

Lib::~Lib()
{
    
}

void Lib::preDisable(int flag)
{
    start(false);
}
    
void Lib::postDisable(int flag)
{    
    if( flag )
    {
	//Delete libraries record
	AutoHD<TBDS> bd = owner().owner().owner().db();
	bd.at().open(owner().BD()).at().fieldDel(*this);
	bd.at().close(owner().BD());
	
	//Delete function's files	
	bool to_open = false;
	if( !((TTipBD &)bd.at().modAt(BD().tp).at()).openStat(BD().bd) )
	{
	    to_open = true;
	    ((TTipBD &)bd.at().modAt(BD().tp).at()).open(BD().bd,false);
	}
	((TTipBD &)bd.at().modAt(BD().tp).at()).at(BD().bd).at().del(BD().tbl);
	((TTipBD &)bd.at().modAt(BD().tp).at()).at(BD().bd).at().del(BD().tbl+"_io");
	if( to_open ) ((TTipBD &)bd.at().modAt(BD().tp).at()).close(BD().bd);
    }
}

void Lib::load( )
{
    AutoHD<TBDS> bd = owner().owner().owner().db();
    bd.at().open(owner().BD()).at().fieldGet(*this);
    bd.at().close(owner().BD());

    //Load functions
    TConfig c_el(&owner().elFnc());
    AutoHD<TTable> tbl = bd.at().open(BD());

    int fld_cnt = 0;
    while( bd.at().dataSeek(tbl,nodePath()+"_fnc/", fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();
        
	if( !present(f_id) )
        {
	    Func *n_fnc = new Func(f_id.c_str(),this);
	    //*(TConfig *)n_fnc = c_el;
	    reg(n_fnc);
	}
        ((Func &)at(f_id).at()).load();
    }
    if(!tbl.freeStat())
    {
	tbl.free();
	bd.at().close(BD());
    }    
}

void Lib::save( )
{    
    AutoHD<TBDS> bd = owner().owner().owner().db();
    bd.at().open(owner().BD(),true).at().fieldSet(*this);
    bd.at().close(owner().BD());

    //Save functions
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
	((Func &)at(f_lst[i_ls]).at()).save();    
}

void Lib::copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name )
{
    string lib = l_id;
    string toid = to_id;
    string toname = to_name;
    
    if( !present(f_id) )
	throw TError(nodePath().c_str(),"Function <%s> no present.",f_id.c_str());
	
    if( !lib.size() )	lib    = id();
    if( !toid.size() )	toid   = at(f_id).at().id();
    if( !toname.size() )toname = at(f_id).at().name();
    
    if( !owner().present(lib) )
	throw TError(nodePath().c_str(),"Library <%s> no present.",lib.c_str());
    if( owner().owner().owner().func().at().at(lib).at().present(toid) )
	throw TError(nodePath().c_str(),"Function <%s:%s> already present.",lib.c_str(),toid.c_str());
    //Make new function	
    Func *n_fnc = new Func(toid.c_str(),this);
    (*n_fnc) = ((Func&)at(f_id).at());
    n_fnc->name(to_name.c_str());    
    ((Lib&)owner().owner().owner().func().at().at(lib).at()).reg(n_fnc);
}

void Lib::add( const char *id, const char *name )
{
    reg(new Func(id,this,name));
}

void Lib::del( const char *id )
{
    unreg(id);
}

TBDS::SName Lib::BD()
{
    return owner().owner().owner().nameDBPrep(TBDS::SName(m_bd_tp.c_str(),m_bd_nm.c_str(),m_bd_tbl.c_str()),true);
}

TipContr &Lib::owner()
{
    return *m_owner;
}

void Lib::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
	TLibFunc::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/name",Mess->I18N("Name"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/descr",Mess->I18N("Description"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/bd_tp",mod->I18N("Library BD (module:bd:table)"),0660,0,0,"str")->
    	    attr_("idm","1")->attr_("dest","select")->attr_("select","/lib/cfg/b_mod");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/bd_nm","",0660,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/bd_tbl","",0660,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/lib/cfg/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/lib/cfg/save",Mess->I18N("Save"),0550);
	ctrMkNode("list",opt,a_path.c_str(),"/func/func",Mess->I18N("Functions"),0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del,edit")->attr_("mode","att")->attr_("br_pref","_");
	ctrMkNode("comm",opt,a_path.c_str(),"/func/copy",mod->I18N("Copy function"),0440);
	ctrMkNode("fld",opt,a_path.c_str(),"/func/copy/fnc",mod->I18N("Function"),0660,0,0,"str")->
	    attr_("idm","1")->attr_("dest","select")->attr_("select","/func/func");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/copy/lib",mod->I18N("To library"),0660,0,0,"str")->
	    attr_("idm","1")->attr_("dest","select")->attr_("select","/func/ls_lib");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/copy/id",mod->I18N("Name as"),0660,0,0,"str")->attr_("len","10");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/copy/nm","",0660,0,0,"str");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/lib/cfg/bd_tp" )	ctrSetS(opt,m_bd_tp);
	else if( a_path == "/lib/cfg/bd_nm" )	ctrSetS(opt,m_bd_nm);
	else if( a_path == "/lib/cfg/bd_tbl" )	ctrSetS(opt,m_bd_tbl);
	else if( a_path == "/lib/cfg/b_mod" )
	{
	    opt->childClean();
	    owner().owner().owner().db().at().modList(list);
	    ctrSetS( opt, "", "" );
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    		ctrSetS( opt, owner().owner().owner().db().at().modAt(list[i_a]).at().modName(), list[i_a].c_str() );
	}
	else if( a_path == "/func/ls_lib" )
	{
	    opt->childClean();
	    ctrSetS( opt, "", "" );
	    for( unsigned i_a=0; i_a < owner().freeLibList().size(); i_a++ )
		ctrSetS( opt, owner().owner().owner().func().at().at(owner().freeLibList()[i_a]).at().name(), 
		    owner().freeLibList()[i_a].c_str() );
	}
	else TLibFunc::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/lib/cfg/name" )		m_name 	= ctrGetS(opt);
	else if( a_path == "/lib/cfg/descr" )	m_descr = ctrGetS(opt);
	else if( a_path == "/lib/cfg/bd_tp" )	m_bd_tp	= ctrGetS(opt);
	else if( a_path == "/lib/cfg/bd_nm" )	m_bd_nm	= ctrGetS(opt);
	else if( a_path == "/lib/cfg/bd_tbl" )	m_bd_tbl= ctrGetS(opt);
	else if( a_path == "/func/func" )
	{
	    if( opt->name() == "add" )		reg( new Func(opt->attr("id").c_str(),this,opt->text().c_str()) );
	    else if( opt->name() == "del" )	chldDel(m_fnc,opt->attr("id"),-1,1);
	    else if( opt->name() == "edit" )	
	    {
		((Func&)at(opt->attr("p_id")).at()).chID(opt->attr("id").c_str());
		((Func&)at(opt->attr("id")).at()).name(opt->text().c_str());
		((Func&)at(opt->attr("id")).at()).save();
	    }
	}
	else if( a_path == "/func/copy" )	
	    copyFunc(ctrGetS(ctrId(opt,"fnc")),ctrGetS(ctrId(opt,"lib")), ctrGetS(ctrId(opt,"id")), ctrGetS(ctrId(opt,"nm")));
	else if( a_path == "/lib/cfg/load" )	load();
	else if( a_path == "/lib/cfg/save" )	save();
	else TLibFunc::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
}    																									    



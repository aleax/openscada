/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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
Lib::Lib( const char *id, const char *name ) : 
    TConfig(&mod->elLib()), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_bd_tp(cfg("BD_TP").getSd()), m_bd_nm(cfg("BD_NM").getSd()), 
    m_bd_tbl(cfg("BD_TBL").getSd())
{
    m_id = id;
    m_bd_tbl = string("lib_")+id;
    m_name = name;
    m_fnc = grpAdd("fnc_");
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
	SYS->db().at().dataDel(mod->BD(),mod->nodePath()+"lib/",*this);
	
	//Delete function's files	
	bool to_open = false;
	if( !((TTipBD &)SYS->db().at().modAt(BD().tp).at()).openStat(BD().bd) )
	{
	    to_open = true;
	    ((TTipBD &)SYS->db().at().modAt(BD().tp).at()).open(BD().bd,false);
	}
	((TTipBD &)SYS->db().at().modAt(BD().tp).at()).at(BD().bd).at().del(BD().tbl);
	((TTipBD &)SYS->db().at().modAt(BD().tp).at()).at(BD().bd).at().del(BD().tbl+"_io");
	if( to_open ) ((TTipBD &)SYS->db().at().modAt(BD().tp).at()).close(BD().bd);
    }
}

void Lib::load( )
{
    SYS->db().at().dataGet(mod->BD(),mod->nodePath()+"lib/",*this);

    //Load functions
    TConfig c_el(&mod->elFnc());
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(BD(),nodePath()+"fnc/", fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();
        
	if( !present(f_id) )	add(f_id.c_str());
        at(f_id).at().load();
	
	c_el.cfg("ID").setS("");
    }
}

void Lib::save( )
{    
    SYS->db().at().dataSet(mod->BD(),mod->nodePath()+"lib/",*this);

    //Save functions
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
	at(f_lst[i_ls]).at().save();
}

void Lib::start( bool val )
{
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
        at(lst[i_f]).at().start(val);
	    
    run_st = val;
}

void Lib::copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name )
{
    string lib = l_id;
    string toid = to_id;
    string toname = to_name;
    
    if( !present(f_id) )
	throw TError(nodePath().c_str(),mod->I18N("Function <%s> no present."),f_id.c_str());
	
    if( !lib.size() )	lib    = id();
    if( !toid.size() )	toid   = at(f_id).at().id();
    if( !toname.size() )toname = at(f_id).at().name();
    
    if( !mod->lbPresent(lib) )
	throw TError(nodePath().c_str(),mod->I18N("Library <%s> no present."),lib.c_str());
    if( mod->lbAt(lib).at().present(toid) )
	throw TError(nodePath().c_str(),mod->I18N("Function <%s:%s> already present."),lib.c_str(),toid.c_str());
    //Make new function	
    mod->lbAt(lib).at().add(toid.c_str());
    mod->lbAt(lib).at().at(toid).at() = at(f_id).at();
    mod->lbAt(lib).at().at(toid).at().name(to_name.c_str());
}

void Lib::add( const char *id, const char *name )
{
    chldAdd(m_fnc,new Func(id,name));
}

void Lib::del( const char *id )
{
    chldDel(m_fnc,id);
}

TBDS::SName Lib::BD()
{
    return SYS->nameDBPrep(TBDS::SName(m_bd_tp.c_str(),m_bd_nm.c_str(),m_bd_tbl.c_str()));
}

void Lib::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> lst;
    
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",mod->I18N("Function's library: ")+id());
	ctrMkNode("area",opt,a_path.c_str(),"/lib",mod->I18N("Library"));
	ctrMkNode("area",opt,a_path.c_str(),"/lib/st",mod->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/st/st",mod->I18N("Accessing"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/lib/cfg",mod->I18N("Config"));
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/id",mod->I18N("Id"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/name",mod->I18N("Name"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/descr",mod->I18N("Description"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/bd",mod->I18N("Library BD (module:bd:table)"),0660,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/lib/cfg/load",mod->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/lib/cfg/save",mod->I18N("Save"),0550);
	ctrMkNode("area",opt,a_path.c_str(),"/func",mod->I18N("Functions"));
	ctrMkNode("list",opt,a_path.c_str(),"/func/func",mod->I18N("Functions"),0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del,edit")->attr_("br_pref","fnc_");
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
	if( a_path == "/lib/st/st" )            ctrSetB( opt, run_st );
        else if( a_path == "/lib/cfg/id" )      ctrSetS( opt, id() );
	else if( a_path == "/lib/cfg/name" )    ctrSetS( opt, name() );
        else if( a_path == "/lib/cfg/descr" )   ctrSetS( opt, descr() );
	else if( a_path == "/lib/cfg/bd" )	ctrSetS(opt,m_bd_tp+":"+m_bd_nm+":"+m_bd_tbl);
	else if( a_path == "/func/func" )
        {
            list(lst);
            opt->childClean();
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                ctrSetS( opt, at(lst[i_f]).at().name(), lst[i_f].c_str() );
        }
	else if( a_path == "/func/ls_lib" )
	{
	    opt->childClean();
	    ctrSetS( opt, "", "" );
	    mod->lbList(lst);
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
		ctrSetS( opt, mod->lbAt(lst[i_a]).at().name(), lst[i_a].c_str() );
	}
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/lib/st/st" )    	start(ctrGetB(opt));
	else if( a_path == "/lib/cfg/name" )	m_name 	= ctrGetS(opt);
	else if( a_path == "/lib/cfg/descr" )	m_descr = ctrGetS(opt);
	else if( a_path == "/lib/cfg/bd" )	
	{
	     m_bd_tp = TSYS::strSepParse(ctrGetS(opt),0,':');
	     m_bd_nm = TSYS::strSepParse(ctrGetS(opt),1,':');
	     m_bd_tbl= TSYS::strSepParse(ctrGetS(opt),2,':');
	}	
	else if( a_path == "/func/func" )
	{
	    if( opt->name() == "add" )		add(opt->attr("id").c_str(),opt->text().c_str());
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
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }
}    																									    



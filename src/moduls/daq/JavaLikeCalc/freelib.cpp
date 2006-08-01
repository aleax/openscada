
//OpenSCADA system module DAQ.JavaLikeCalc file: freelib.cpp
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
#include <tmess.h>
#include "freefunc.h"
#include "virtual.h"
#include "freelib.h"

using namespace JavaLikeCalc;

//================ Functions library ==================
Lib::Lib( const char *id, const char *name, const string &lib_db ) : 
    TConfig(&mod->elLib()), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_db(cfg("DB").getSd()), work_lib_db(lib_db)
{
    m_id = id;
    m_name = name;
    m_db = string("lib_")+id;
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
	SYS->db().at().dataDel(work_lib_db+"."+mod->libTable(),mod->nodePath()+"lib/",*this);
	
	//Delete function's files
	SYS->db().at().open(BD());
	SYS->db().at().close(BD(),true);

	SYS->db().at().open(BD()+"_io");
	SYS->db().at().close(BD()+"_io",true);
    }
}

string Lib::name()   
{ 
    return (m_name.size())?m_name:m_id;
}

void Lib::load( )
{
    SYS->db().at().dataGet(work_lib_db+"."+mod->libTable(),mod->nodePath()+"lib/",*this);

    //Load functions
    TConfig c_el(&mod->elFnc());
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(BD(),mod->nodePath()+tbl()+"/", fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();
        
	if( !present(f_id) )	add(f_id.c_str());
        at(f_id).at().load();
	
	c_el.cfg("ID").setS("");
    }
}

void Lib::save( )
{    
    SYS->db().at().dataSet(work_lib_db+"."+mod->libTable(),mod->nodePath()+"lib/",*this);

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

string Lib::BD()
{
    return work_lib_db+'.'+m_db;
}

void Lib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {	
    	ctrMkNode("oscada_cntr",opt,-1,"/",mod->I18N("Function's library: ")+id());
	ctrMkNode("branches",opt,-1,"/br","",0444);
	ctrMkNode("grp",opt,-1,"/br/fnc_",Mess->I18N("Function"),0444,"root","root",1,"list","/func/func");
	ctrMkNode("area",opt,-1,"/lib",mod->I18N("Library"));
	ctrMkNode("area",opt,-1,"/lib/st",mod->I18N("State"));
	ctrMkNode("fld",opt,-1,"/lib/st/st",mod->I18N("Accessing"),0664,"root","root",1,"tp","bool");
	ctrMkNode("fld",opt,-1,"/lib/st/bd",mod->I18N("Library BD (module.bd.table)"),0660,"root","root",1,"tp","str");
	ctrMkNode("area",opt,-1,"/lib/cfg",mod->I18N("Config"));
	ctrMkNode("fld",opt,-1,"/lib/cfg/id",mod->I18N("Id"),0444,"root","root",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/lib/cfg/name",mod->I18N("Name"),0664,"root","root",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/lib/cfg/descr",mod->I18N("Description"),0664,"root","root",3,"tp","str","cols","50","rows","3");
	ctrMkNode("comm",opt,-1,"/lib/cfg/load",mod->I18N("Load"),0440);
        ctrMkNode("comm",opt,-1,"/lib/cfg/save",mod->I18N("Save"),0440);
	ctrMkNode("area",opt,-1,"/func",mod->I18N("Functions"));
	ctrMkNode("list",opt,-1,"/func/func",mod->I18N("Functions"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del,edit","br_pref","fnc_");
	ctrMkNode("comm",opt,-1,"/func/copy",mod->I18N("Copy function"),0440);
	ctrMkNode("fld",opt,-1,"/func/copy/fnc",mod->I18N("Function"),0660,"root","root",4,"tp","str","idm","1","dest","select","select","/func/func");
	ctrMkNode("fld",opt,-1,"/func/copy/lib",mod->I18N("To library"),0660,"root","root",4,"tp","str","idm","1","dest","select","select","/func/ls_lib");
	ctrMkNode("fld",opt,-1,"/func/copy/id",mod->I18N("Name as"),0660,"root","root",2,"tp","str","len","10");
	ctrMkNode("fld",opt,-1,"/func/copy/nm","",0660,"root","root",1,"tp","str");
        opt->attr("rez","0");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/lib/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	start(atoi(opt->text().c_str()));
    }
    else if( a_path == "/lib/st/bd" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->text(work_lib_db+"."+m_db);
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	{
	    work_lib_db = TSYS::strSepParse(opt->text(),0,'.')+"."+TSYS::strSepParse(opt->text(),1,'.');
            m_db = TSYS::strSepParse(opt->text(),2,'.');
	}
    }
    else if( a_path == "/lib/cfg/id" && ctrChkNode(opt) )	opt->text(id());
    else if( a_path == "/lib/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_name = opt->text();
    }
    else if( a_path == "/lib/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(descr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_descr = opt->text();
    }	
    else if( a_path == "/func/func" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
        {
	    vector<string> lst;
            list(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->attr("id",lst[i_f])->text(at(lst[i_f]).at().name());
        }
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	add(opt->attr("id").c_str(),opt->text().c_str());
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	chldDel(m_fnc,opt->attr("id"),-1,1);
	if( ctrChkNode(opt,"edit",0664,"root","root",SEQ_WR) )
        {
	    at(opt->attr("p_id")).at().chID(opt->attr("id").c_str());
	    at(opt->attr("id")).at().name(opt->text().c_str());
	    at(opt->attr("id")).at().save();
	}
    }	
    else if( a_path == "/func/ls_lib" && ctrChkNode(opt) )
    {
	vector<string> lst;
	opt->childAdd("el")->attr("id","")->text("");
	mod->lbList(lst);
	for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	    opt->childAdd("el")->attr("id",lst[i_a])->text(mod->lbAt(lst[i_a]).at().name());
    }
    else if( a_path == "/func/copy" && ctrChkNode(opt,"set",0440) )	
	copyFunc(ctrId(opt,"fnc")->text(),ctrId(opt,"lib")->text(), ctrId(opt,"id")->text(), ctrId(opt,"nm")->text());
    else if( a_path == "/lib/cfg/load" && ctrChkNode(opt,"set",0440) )	load();
    else if( a_path == "/lib/cfg/save" && ctrChkNode(opt,"set",0440) )	save();
}

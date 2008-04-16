
//OpenSCADA system module DAQ.JavaLikeCalc file: freelib.cpp
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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
#include <tmess.h>
#include "freefunc.h"
#include "virtual.h"
#include "freelib.h"

using namespace JavaLikeCalc;

//*************************************************
//* Lib: Functions library                        *
//*************************************************
Lib::Lib( const char *id, const char *name, const string &lib_db ) : 
    TConfig(&mod->elLib()), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_db(cfg("DB").getSd()), work_lib_db(lib_db)
{
    m_id = id;
    m_name = name;
    m_db = string("flb_")+id;
    m_fnc = grpAdd("fnc_");
}

Lib::~Lib( )
{
    
}

void Lib::preDisable( int flag )
{
    setStart(false);
}

void Lib::postDisable( int flag )
{   
    if( flag && DB().size() )
    {
	//- Delete libraries record -
	SYS->db().at().dataDel(DB()+"."+mod->libTable(),mod->nodePath()+"lib/",*this);
	
	//- Delete function's files -
	SYS->db().at().open(fullDB());
	SYS->db().at().close(fullDB(),true);

	SYS->db().at().open(fullDB()+"_io");
	SYS->db().at().close(fullDB()+"_io",true);
    }
}

string Lib::name( )
{ 
    return (m_name.size())?m_name:m_id;
}

void Lib::load( )
{
    if( !DB().size() )	return;
    
    SYS->db().at().dataGet(work_lib_db+"."+mod->libTable(),mod->nodePath()+"lib/",*this);

    //- Load functions -
    TConfig c_el(&mod->elFnc());
    c_el.cfgViewAll(false);
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl(), fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();
        
	if( !present(f_id) )	add(f_id.c_str());
        at(f_id).at().load();
	
	c_el.cfg("ID").setS("");
    }
}

void Lib::save( )
{   
    if( !DB().size() )    return;
 
    SYS->db().at().dataSet(work_lib_db+"."+mod->libTable(),mod->nodePath()+"lib/",*this);

    //- Save functions -
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
	at(f_lst[i_ls]).at().save();
}

void Lib::setStart( bool val )
{
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
        at(lst[i_f]).at().setStart(val);
	    
    run_st = val;
}

void Lib::copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name )
{
    string lib = l_id;
    string toid = to_id;
    string toname = to_name;
    
    if( !present(f_id) )
	throw TError(nodePath().c_str(),_("Function <%s> no present."),f_id.c_str());
	
    if( !lib.size() )	lib    = id();
    if( !toid.size() )	toid   = at(f_id).at().id();
    if( !toname.size() )toname = at(f_id).at().name();
    
    if( !mod->lbPresent(lib) )
	throw TError(nodePath().c_str(),_("Library <%s> no present."),lib.c_str());
    if( mod->lbAt(lib).at().present(toid) )
	throw TError(nodePath().c_str(),_("Function <%s:%s> already present."),lib.c_str(),toid.c_str());

    //- Make new function -
    mod->lbAt(lib).at().add(toid.c_str());
    mod->lbAt(lib).at().at(toid).at() = at(f_id).at();
    mod->lbAt(lib).at().at(toid).at().setName(to_name.c_str());
}

void Lib::add( const char *id, const char *name )
{
    chldAdd(m_fnc,new Func(id,name));
}

void Lib::del( const char *id )
{
    chldDel(m_fnc,id);
}

void Lib::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {	
    	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function's library: ")+id());
	if(ctrMkNode("branches",opt,-1,"/br","",0444))
	    ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),0664,"root","root",1,"s_com","add,del");
	if(ctrMkNode("area",opt,-1,"/lib",_("Library")))
	{
	    if(ctrMkNode("area",opt,-1,"/lib/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/lib/st/st",_("Accessing"),0664,"root","root",1,"tp","bool");
		if(DB().size())
		    ctrMkNode("fld",opt,-1,"/lib/st/db",_("Library DB (module.db.table)"),0660,"root","root",1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/lib/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/lib/cfg/id",_("Id"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/lib/cfg/name",_("Name"),0664,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/lib/cfg/descr",_("Description"),0664,"root","root",3,"tp","str","cols","50","rows","3");
		ctrMkNode("comm",opt,-1,"/lib/cfg/load",_("Load"),0660);
    		ctrMkNode("comm",opt,-1,"/lib/cfg/save",_("Save"),0660);
	    }
	}
	if(ctrMkNode("area",opt,-1,"/func",_("Functions")))
	{
	    ctrMkNode("list",opt,-1,"/func/func",_("Functions"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","fnc_");
	    if(ctrMkNode("comm",opt,-1,"/func/copy",_("Copy function"),0660))
	    {
		ctrMkNode("fld",opt,-1,"/func/copy/fnc",_("Function"),0660,"root","root",4,"tp","str","idm","1","dest","select","select","/func/func");
		ctrMkNode("fld",opt,-1,"/func/copy/lib",_("To library"),0660,"root","root",4,"tp","str","idm","1","dest","select","select","/func/ls_lib");
		ctrMkNode("fld",opt,-1,"/func/copy/id",_("Name as"),0660,"root","root",2,"tp","str","len","10");
		ctrMkNode("fld",opt,-1,"/func/copy/nm","",0660,"root","root",1,"tp","str");
	    }
	}
        return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/lib/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setStart(atoi(opt->text().c_str()));
    }
    else if( a_path == "/lib/st/db" && DB().size() )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(work_lib_db+"."+m_db);
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	{
	    work_lib_db = TSYS::strSepParse(opt->text(),0,'.')+"."+TSYS::strSepParse(opt->text(),1,'.');
            m_db = TSYS::strSepParse(opt->text(),2,'.');
	}
    }
    else if( a_path == "/lib/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/lib/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_name = opt->text();
    }
    else if( a_path == "/lib/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(descr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_descr = opt->text();
    }	
    else if( a_path == "/br/fnc_" || a_path == "/func/func" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
        {
	    vector<string> lst;
            list(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
        }
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	add(opt->attr("id").c_str(),opt->text().c_str());
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	chldDel(m_fnc,opt->attr("id"),-1,1);
    }	
    else if( a_path == "/func/ls_lib" && ctrChkNode(opt) )
    {
	vector<string> lst;
	opt->childAdd("el")->setAttr("id","")->setText("");
	mod->lbList(lst);
	for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	    opt->childAdd("el")->setAttr("id",lst[i_a])->setText(mod->lbAt(lst[i_a]).at().name());
    }
    else if( a_path == "/func/copy" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	
	copyFunc(ctrId(opt,"fnc")->text(),ctrId(opt,"lib")->text(), ctrId(opt,"id")->text(), ctrId(opt,"nm")->text());
    else if( a_path == "/lib/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	load();
    else if( a_path == "/lib/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	save();
}

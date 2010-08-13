
//OpenSCADA system module DAQ.JavaLikeCalc file: freelib.cpp
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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
    TConfig(&mod->elLib()), mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDescr(cfg("DESCR").getSd()),
    mDB(cfg("DB").getSd()), mProgTr(cfg("PROG_TR").getBd()), work_lib_db(lib_db)
{
    mId = id;
    mName = name;
    mDB = string("flb_")+id;
    mFnc = grpAdd("fnc_");
    if( DB().empty() )	modifClr();
}

Lib::~Lib( )
{

}

TCntrNode &Lib::operator=( TCntrNode &node )
{
    Lib *src_n = dynamic_cast<Lib*>(&node);
    if( !src_n ) return *this;

    //- Configuration copy -
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    mId = tid;
    work_lib_db = src_n->work_lib_db;

    //- Functions copy -
    vector<string> ls;
    src_n->list(ls);
    for( int i_p = 0; i_p < ls.size(); i_p++ )
    {
	if( !present(ls[i_p]) ) add(ls[i_p].c_str());
	(TCntrNode&)at(ls[i_p]).at() = (TCntrNode&)src_n->at(ls[i_p]).at();
    }
    if( src_n->startStat() && !startStat() )	setStart(true);

    return *this;
}

void Lib::preDisable( int flag )
{
    setStart(false);
}

void Lib::postDisable( int flag )
{
    if( flag && DB().size() )
    {
	//> Delete libraries record
	SYS->db().at().dataDel(DB()+"."+mod->libTable(),mod->nodePath()+"lib/",*this,true);

	//> Delete function's files
	SYS->db().at().open(fullDB());
	SYS->db().at().close(fullDB(),true);

	SYS->db().at().open(fullDB()+"_io");
	SYS->db().at().close(fullDB()+"_io",true);
    }
}

string Lib::name( )
{
    return (mName.size())?mName:mId;
}

void Lib::setFullDB( const string &idb )
{
    work_lib_db = TSYS::strSepParse(idb,0,'.')+"."+TSYS::strSepParse(idb,1,'.');
    mDB = TSYS::strSepParse(idb,2,'.');
    modifG( );
}

void Lib::load_( )
{
    if( DB().empty() || (!SYS->chkSelDB(DB())) )	return;

    SYS->db().at().dataGet(DB()+"."+mod->libTable(),mod->nodePath()+"lib/",*this);

    //> Load functions
    TConfig c_el(&mod->elFnc());
    c_el.cfgViewAll(false);
    for( int fld_cnt = 0; SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl(), fld_cnt++,c_el); )
    {
	string f_id = c_el.cfg("ID").getS();
	if( !present(f_id) )	add(f_id.c_str());
	at(f_id).at().load();
    }
}

void Lib::save_( )
{
    if( DB().empty() )    return;

    SYS->db().at().dataSet(DB()+"."+mod->libTable(),mod->nodePath()+"lib/",*this);
}

void Lib::setStart( bool val )
{
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
	try{ at(lst[i_f]).at().setStart(val); }
	catch( TError err ) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    run_st = val;
}

void Lib::add( const char *id, const char *name )
{
    chldAdd(mFnc,new Func(id,name));
}

void Lib::del( const char *id )
{
    chldDel(mFnc,id);
}

void Lib::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function's library: ")+id(),0664,"root","root");
	if(ctrMkNode("branches",opt,-1,"/br","",0444))
	    ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),0664,"root","root",2,"idm","1","idSz","20");
	if(ctrMkNode("area",opt,-1,"/lib",_("Library")))
	{
	    if(ctrMkNode("area",opt,-1,"/lib/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/lib/st/st",_("Accessing"),0664,"root","root",1,"tp","bool");
		if(DB().size())
		    ctrMkNode("fld",opt,-1,"/lib/st/db",_("Library DB"),0664,"root","root",4,"tp","str","dest","sel_ed","select","/db/tblList",
			"help",_("DB address in format [<DB module>.<DB name>.<Table name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/lib/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/lib/cfg/id",_("Id"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/lib/cfg/name",_("Name"),DB().empty()?0444:0664,"root","root",2,"tp","str","len","50");
		ctrMkNode("fld",opt,-1,"/lib/cfg/descr",_("Description"),DB().empty()?0444:0664,"root","root",3,"tp","str","cols","100","rows","5");
		if( !DB().empty() )
		    ctrMkNode("fld",opt,-1,"/lib/cfg/progTr",_("Program's text translation"),0664,"root","root",1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/func",_("Functions")))
	    ctrMkNode("list",opt,-1,"/func/func",_("Functions"),0664,"root","root",5,"tp","br","idm","1","s_com","add,del","br_pref","fnc_","idSz","20");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/lib/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText( startStat() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	setStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/lib/st/db" && DB().size() )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEC_RD) )	opt->setText( fullDB() );
	if( ctrChkNode(opt,"set",0660,"root","root",SEC_WR) )	setFullDB( opt->text() );
    }
    else if( a_path == "/lib/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/lib/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText( name() );
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	setName( opt->text() );
    }
    else if( a_path == "/lib/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText( descr() );
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	setDescr( opt->text() );
    }
    else if( a_path == "/lib/cfg/progTr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText( TSYS::int2str(progTr()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	setProgTr( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/br/fnc_" || a_path == "/func/func" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )
	{
	    vector<string> lst;
	    list(lst);
	    for( unsigned i_f=0; i_f < lst.size(); i_f++ )
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEC_WR) )	add(TSYS::strEncode(opt->attr("id"),TSYS::oscdID).c_str(),opt->text().c_str());
	if( ctrChkNode(opt,"del",0664,"root","root",SEC_WR) )	chldDel(mFnc,opt->attr("id"),-1,1);
    }
    else if( a_path == "/func/ls_lib" && ctrChkNode(opt) )
    {
	vector<string> lst;
	opt->childAdd("el")->setAttr("id","")->setText("");
	mod->lbList(lst);
	for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	    opt->childAdd("el")->setAttr("id",lst[i_a])->setText(mod->lbAt(lst[i_a]).at().name());
    }
    else TCntrNode::cntrCmdProc(opt);
}

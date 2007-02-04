
//OpenSCADA system module UI.VCSEngine file: libwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <tsys.h>

#include "origwidg.h"
#include "vcaengine.h"
#include "libwidg.h"

using namespace VCA;

//================ Functions library ==================
LibWdg::LibWdg( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elWdgLib()), m_enable(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()),
    m_descr(cfg("DESCR").getSd()), m_ico(cfg("ICO").getSd()), m_dbt(cfg("DB_TBL").getSd()), 
    m_user(cfg("USER").getSd()), m_grp(cfg("GRP").getSd()), m_permit(cfg("PERMIT").getId()), work_lib_db(lib_db)
{
    m_id = id;
    m_name = name;
    m_user = "user";
    m_grp  = "users";
    m_permit = 0664;
    m_dbt = string("wlb_")+id;
    m_wdg = grpAdd("wdg_");
}
			
LibWdg::~LibWdg()
{

}

void LibWdg::preDisable( int flag )
{
    if( enable() ) enable(false);
}

void LibWdg::postDisable(int flag)
{
    if( flag )
    {
        //- Delete libraries record -
        SYS->db().at().dataDel(DB()+"."+mod->wlbTable(),mod->nodePath()+"wlb/",*this);
			    
        //- Delete function's files -
	//-- Delete widgets table --
        SYS->db().at().open(fullDB());
        SYS->db().at().close(fullDB(),true);
	//-- Delete attributes table --
        SYS->db().at().open(fullDB()+"_io");
        SYS->db().at().close(fullDB()+"_io",true);
	//-- Delete include widgets table --
	SYS->db().at().open(fullDB()+"_incl");
	SYS->db().at().close(fullDB()+"_incl",true);
    }
}

string LibWdg::name()
{
    return (m_name.size())?m_name:m_id;
}

string LibWdg::user()
{ 
    return SYS->security().at().usrPresent(m_user)?m_user:"user"; 
}

string LibWdg::grp()
{ 
    return SYS->security().at().grpPresent(m_grp)?m_grp:"users"; 
}

void LibWdg::user( const string &it )   
{ 
    m_user = it;
    //Update librarie's group
    vector<string> gls;
    SYS->security().at().usrGrpList(user(),gls);
    grp(gls.size()?gls[0]:"users");
}

void LibWdg::load( )
{
    SYS->db().at().dataGet(DB()+"."+mod->wlbTable(),mod->nodePath()+"lib/",*this);
    
    //Load widgets
    TConfig c_el(&mod->elWdg());
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl()+"/", fld_cnt++,c_el) )
    {
        string f_id = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");	
        if( !present(f_id) )	add(f_id,"",c_el.cfg("ORIGWDG").getS());
        at(f_id).at().load();
    }
}
							    
void LibWdg::save( )
{
    SYS->db().at().dataSet(DB()+"."+mod->wlbTable(),mod->nodePath()+"lib/",*this);
								
    //Save widgets
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        at(f_lst[i_ls]).at().save();
}

void LibWdg::enable( bool val )
{
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        at(f_lst[i_ls]).at().enable(val);
	
    m_enable = val;
}

void LibWdg::add( const string &id, const string &name, const string &orig )
{
    Widget *wdg = mod->origGet(orig,id);
    if(!wdg)	throw TError(mod->nodePath().c_str(),_("Creation widget <%s> error!"),id.c_str());
    wdg->name(name);
    chldAdd(m_wdg,wdg);
}
    
void LibWdg::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget's library: ")+id());
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",0444);	
        if(ctrMkNode("branches",opt,-1,"/br","",0444))
	    ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),R_R_R_,"root","root",1,"list","/wdg/wdg");
        if(ctrMkNode("area",opt,-1,"/lib",_("Library")))
	{
    	    if(ctrMkNode("area",opt,-1,"/lib/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/lib/st/en",_("Enable"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
    		ctrMkNode("fld",opt,-1,"/lib/st/db",_("Library BD (module.db.table)"),RWR_R_,user().c_str(),grp().c_str(),1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/lib/cfg",_("Config")))
	    {
    		ctrMkNode("fld",opt,-1,"/lib/cfg/id",_("Id"),R_R_R_,user().c_str(),grp().c_str(),1,"tp","str");
    		ctrMkNode("fld",opt,-1,"/lib/cfg/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
    		ctrMkNode("fld",opt,-1,"/lib/cfg/descr",_("Description"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","50","rows","3");
		ctrMkNode("img",opt,-1,"/lib/cfg/ico",_("Icon"),permit(),user().c_str(),grp().c_str(),2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/lib/cfg/user",_("User and group"),RWRWR_,"root","root",3,"tp","str","dest","select","select","/lib/u_lst");
    		ctrMkNode("fld",opt,-1,"/lib/cfg/grp","",0644,user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/lib/g_lst");
		ctrMkNode("fld",opt,-1,"/lib/cfg/u_a",_("Access: user-grp-other"),RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/lib/a_lst");
		ctrMkNode("fld",opt,-1,"/lib/cfg/g_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/lib/a_lst");	
		ctrMkNode("fld",opt,-1,"/lib/cfg/o_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/lib/a_lst");		
    		ctrMkNode("comm",opt,-1,"/lib/cfg/load",_("Load"),permit(),user().c_str(),grp().c_str());
    		ctrMkNode("comm",opt,-1,"/lib/cfg/save",_("Save"),permit(),user().c_str(),grp().c_str());
	    }
	}
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widgets")))
	{
	    ctrMkNode("fld",opt,-1,"/wdg/tlb",_("Create widget"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/wdg/t_lst");
    	    ctrMkNode("list",opt,-1,"/wdg/wdg",_("Widgets"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","wdg_");
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/lib/st/en" )
    {
        if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),grp().c_str(),SEQ_RD) ) opt->text(TSYS::int2str(enable()));
        if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),grp().c_str(),SEQ_WR) ) enable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/lib/st/db" )
    {
	if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(fullDB());
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )
	{
            work_lib_db = TSYS::strSepParse(opt->text(),0,'.')+"."+TSYS::strSepParse(opt->text(),1,'.');
    	    m_dbt = TSYS::strSepParse(opt->text(),2,'.');
	}
    }    
    else if( a_path == "/lib/cfg/user" )
    {
        if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->text(user());
        if( ctrChkNode(opt,"set",RWRWR_,"root","root",SEQ_WR) )	user(opt->text());
    }
    else if( a_path == "/lib/cfg/grp" )
    {
        if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )	opt->text(grp());
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )	grp(opt->text());
    }
    else if( a_path == "/lib/cfg/u_a" || a_path == "/lib/cfg/g_a" || a_path == "/lib/cfg/o_a" )
    {
	if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    if( a_path == "/lib/cfg/u_a" )	opt->text(TSYS::int2str((m_permit>>6)&0x7));
	    if( a_path == "/lib/cfg/g_a" )	opt->text(TSYS::int2str((m_permit>>3)&0x7));
	    if( a_path == "/lib/cfg/o_a" )	opt->text(TSYS::int2str(m_permit&0x7));
	}
	if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    if( a_path == "/lib/cfg/u_a" )	m_permit=(m_permit&(~(0x07<<6)))|(atoi(opt->text().c_str())<<6);
	    if( a_path == "/lib/cfg/g_a" )	m_permit=(m_permit&(~(0x07<<3)))|(atoi(opt->text().c_str())<<3);
	    if( a_path == "/lib/cfg/o_a" )	m_permit=(m_permit&(~0x07))|atoi(opt->text().c_str());
	}
    }
    else if( a_path == "/lib/cfg/ico" || a_path == "/ico" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(ico());
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   ico(opt->text());
    }
    else if( a_path == "/lib/cfg/id" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str()) ) opt->text(id());
    else if( a_path == "/lib/cfg/name" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(name());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   name(opt->text());
    }
    else if( a_path == "/lib/cfg/descr" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(descr());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   descr(opt->text());
    }
    else if( a_path == "/lib/cfg/load" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )  load();
    else if( a_path == "/lib/cfg/save" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )  save();
    else if( a_path == "/wdg/wdg" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
    	    vector<string> lst;
            list(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->attr("id",lst[i_f])->text(at(lst[i_f]).at().name());
        }
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    add(opt->attr("id").c_str(),opt->text().c_str(),TBDS::genDBGet(nodePath()+"addWdgType","UserEl",opt->attr("user")));
	    at(opt->attr("id")).at().user(opt->attr("user"));
	}
        if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) del(opt->attr("id"),true);
    }    
    else if( a_path == "/wdg/tlb" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
            opt->text(TBDS::genDBGet(nodePath()+"addWdgType","UserEl",opt->attr("user")));
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    TBDS::genDBSet(nodePath()+"addWdgType",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/lib/u_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;
        SYS->security().at().usrList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->text(ls[i_l]);
    }
    else if( a_path == "/lib/g_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;	
        SYS->security().at().usrGrpList(user(), ls );
	for(int i_l = 0; i_l < ls.size(); i_l++)
    	    opt->childAdd("el")->text(ls[i_l]);
    }
    else if( a_path == "/lib/a_lst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->attr("id","0")->text(_("No access"));
	opt->childAdd("el")->attr("id","4")->text(_("Use(open)"));
	opt->childAdd("el")->attr("id","2")->text(_("Modify"));
	opt->childAdd("el")->attr("id","6")->text(_("Full"));	
    }
    else if( a_path == "/wdg/t_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;
        mod->origList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->text(ls[i_l]);
    }    
}


//OpenSCADA system module UI.VCAEngine file: project.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko
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

#include "vcaengine.h"
#include "project.h"

using namespace VCA;

//************************************************
//* Project                              	 *
//************************************************
Project::Project( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elProject()), m_enable(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()),
    m_descr(cfg("DESCR").getSd()), m_ico(cfg("ICO").getSd()), m_dbt(cfg("DB_TBL").getSd()),
    m_user(cfg("USER").getSd()), m_grp(cfg("GRP").getSd()), m_permit(cfg("PERMIT").getId()),
    work_prj_db(lib_db)
{
    m_id = id;
    m_name = name;
    m_dbt = string("prj_")+id;
    m_page= grpAdd("pg_");
}

Project::~Project( )
{

}

void Project::postEnable( int flag )
{
    if( flag&TCntrNode::NodeRestore )	setEnable(true);
}

void Project::preDisable( int flag )
{
    if( enable() )  setEnable(false);
}

void Project::postDisable( int flag )
{
    if( flag )
    {
        //- Delete libraries record -
        SYS->db().at().dataDel(DB()+"."+mod->prjTable(),mod->nodePath()+"prj/",*this);

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

string Project::name( )
{
    return (m_name.size())?m_name:m_id;
}

string Project::user( )
{
    return SYS->security().at().usrPresent(m_user)?m_user:"root";
}

string Project::grp( )
{
    return SYS->security().at().grpPresent(m_grp)?m_grp:"UI";
}

void Project::setUser( const string &it )
{
    m_user = it;
    //Update librarie's group
    if(SYS->security().at().grpAt("UI").at().user(it))
	setGrp("UI");
    else
    {
	vector<string> gls;
	SYS->security().at().usrGrpList(user(),gls);
	setGrp(gls.size()?gls[0]:"UI");
    }
}

void Project::setFullDB( const string &it )
{
    work_prj_db = TSYS::strSepParse(it,0,'.')+"."+TSYS::strSepParse(it,1,'.');
    m_dbt = TSYS::strSepParse(it,2,'.');
}

void Project::load( )
{
    SYS->db().at().dataGet(DB()+"."+mod->prjTable(),mod->nodePath()+"prj/",*this);

    //Create new pages 
    TConfig c_el(&mod->elPage());
    c_el.cfgViewAll(false);
    int fld_cnt = 0;
    c_el.cfg("OWNER").setS("/"+id());
    while( SYS->db().at().dataSeek(fullDB(),mod->nodePath()+tbl()+"/", fld_cnt++,c_el) )
    {
        string f_id = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");
        if( !present(f_id) )	add(f_id,"","");
    }
    //Load present pages
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        at(f_lst[i_ls]).at().load();
}

void Project::save( )
{
    SYS->db().at().dataSet(DB()+"."+mod->prjTable(),mod->nodePath()+"prj/",*this);

    //Save widgets
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        at(f_lst[i_ls]).at().save();
}

void Project::setEnable( bool val )
{
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        at(f_lst[i_ls]).at().setEnable(val);

    m_enable = val;
}

void Project::add( const string &id, const string &name, const string &orig )
{
    if( present(id) )	return;
    chldAdd(m_page,new Page(id,orig));
    at(id).at().setName(name);
}

void Project::add( Page *iwdg )
{
    if( present(iwdg->id()) )	delete iwdg;
    else chldAdd(m_page,iwdg);
}

AutoHD<Page> Project::at( const string &id )
{
    return chldAt(m_page,id);
}

void Project::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Project: ")+id());
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
        if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root","UI",1,"list","/page/page");
        if(ctrMkNode("area",opt,-1,"/obj",_("Project")))
	{
    	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enable"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
    		ctrMkNode("fld",opt,-1,"/obj/st/db",_("Project DB (module.db.table)"),RWR_R_,user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/user",_("User and group"),RWRWR_,"root","root",3,"tp","str","dest","select","select","/obj/u_lst");
    		ctrMkNode("fld",opt,-1,"/obj/st/grp","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/obj/g_lst");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Config")))
	    {
    		ctrMkNode("fld",opt,-1,"/obj/cfg/id",_("Id"),R_R_R_,user().c_str(),grp().c_str(),1,"tp","str");
    		ctrMkNode("fld",opt,-1,"/obj/cfg/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
    		ctrMkNode("fld",opt,-1,"/obj/cfg/descr",_("Description"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","50","rows","3");
		ctrMkNode("img",opt,-1,"/obj/cfg/ico",_("Icon"),permit(),user().c_str(),grp().c_str(),2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/obj/cfg/u_a",_("Access: user-grp-other"),RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/obj/a_lst");
		ctrMkNode("fld",opt,-1,"/obj/cfg/g_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/obj/a_lst");
		ctrMkNode("fld",opt,-1,"/obj/cfg/o_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/obj/a_lst");
    		ctrMkNode("comm",opt,-1,"/obj/cfg/load",_("Load"),permit(),user().c_str(),grp().c_str());
    		ctrMkNode("comm",opt,-1,"/obj/cfg/save",_("Save"),permit(),user().c_str(),grp().c_str());
	    }
	}
	if(ctrMkNode("area",opt,-1,"/page",_("Pages")))
    	    ctrMkNode("list",opt,-1,"/page/page",_("Pages"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","pg_");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/obj/st/en" )
    {
        if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(TSYS::int2str(enable()));
        if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),grp().c_str(),SEQ_WR) ) setEnable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/obj/st/db" )
    {
	if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )	opt->setText(fullDB());
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )	setFullDB(opt->text());
    }
    else if( a_path == "/obj/st/user" )
    {
        if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->setText(user());
        if( ctrChkNode(opt,"set",RWRWR_,"root","root",SEQ_WR) )	setUser(opt->text());
    }
    else if( a_path == "/obj/st/grp" )
    {
        if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )	opt->setText(grp());
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )	setGrp(opt->text());
    }
    else if( a_path == "/obj/cfg/u_a" || a_path == "/obj/cfg/g_a" || a_path == "/obj/cfg/o_a" )
    {
	if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    if( a_path == "/obj/cfg/u_a" )	opt->setText(TSYS::int2str((m_permit>>6)&0x7));
	    if( a_path == "/obj/cfg/g_a" )	opt->setText(TSYS::int2str((m_permit>>3)&0x7));
	    if( a_path == "/obj/cfg/o_a" )	opt->setText(TSYS::int2str(m_permit&0x7));
	}
	if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    if( a_path == "/obj/cfg/u_a" )	m_permit=(m_permit&(~(0x07<<6)))|(atoi(opt->text().c_str())<<6);
	    if( a_path == "/obj/cfg/g_a" )	m_permit=(m_permit&(~(0x07<<3)))|(atoi(opt->text().c_str())<<3);
	    if( a_path == "/obj/cfg/o_a" )	m_permit=(m_permit&(~0x07))|atoi(opt->text().c_str());
	}
    }
    else if( a_path == "/obj/cfg/ico" || a_path == "/ico" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->setText(ico());
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   setIco(opt->text());
    }
    else if( a_path == "/obj/cfg/id" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str()) ) opt->setText(id());
    else if( a_path == "/obj/cfg/name" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->setText(name());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   setName(opt->text());
    }
    else if( a_path == "/obj/cfg/descr" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->setText(descr());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   setDescr(opt->text());
    }
    else if( a_path == "/obj/cfg/load" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )  load();
    else if( a_path == "/obj/cfg/save" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )  save();
    else if( a_path == "/page/page" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
    	    vector<string> lst;
            list(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
        }
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    add(opt->attr("id").c_str(),opt->text().c_str());
	    at(opt->attr("id")).at().setUser(opt->attr("user"));
	}
        if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) del(opt->attr("id"),true);
    }
    else if( a_path == "/obj/u_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;
        SYS->security().at().usrList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->setText(ls[i_l]);
    }
    else if( a_path == "/obj/g_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;
        SYS->security().at().usrGrpList(user(), ls );
	for(int i_l = 0; i_l < ls.size(); i_l++)
    	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if( a_path == "/obj/a_lst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("No access"));
	opt->childAdd("el")->setAttr("id","4")->setText(_("Use(open)"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("Modify"));
	opt->childAdd("el")->setAttr("id","6")->setText(_("Full"));
    }
}

//************************************************
//* Project's page                             *
//************************************************
Page::Page( const string &id, const string &isrcwdg ) :
        Widget(id,isrcwdg), TConfig(&mod->elPage()),
        m_ico(cfg("ICO").getSd()), m_proc(cfg("PROC").getSd()), m_flgs(cfg("FLGS").getId()),
	m_user(cfg("USER").getSd()), m_grp(cfg("GRP").getSd()), m_permit(cfg("PERMIT").getId())
{
    cfg("ID").setS(id);

    m_page = grpAdd("pg_");
    
    setParentNm(isrcwdg);
}

Page::~Page( )
{

}

Page *Page::ownerPage( )
{
    return dynamic_cast<Page*>(nodePrev());
}

Project *Page::ownerProj( )
{
    Page *own = ownerPage( );
    if( own )	return own->ownerProj();
    return dynamic_cast<Project*>(nodePrev());
}

string Page::ownerFullId( bool contr )
{
    Page *own = ownerPage( );
    if( own )	return own->ownerFullId(contr)+(contr?"/pg_":"/")+own->id();
    return string(contr?"/prj_":"/")+ownerProj()->id();
}

void Page::postEnable( int flag )
{
    //- Set owner key for this page -
    cfg("OWNER").setS(ownerFullId());
    
    //- Set default value for id -
    attrAt("id").at().setS(ownerFullId(true)+"/pg_"+id());
    attrAt("id").at().setModifVal(0);
    attrAt("id").at().setModifCfg(0);

    //- Set default parent for parent template page -    
    if( ownerPage() && ownerPage()->prjFlag()&Page::Template )
	setParentNm("..");
    
    //- Call parent method -
    Widget::postEnable(flag);
}

void Page::postDisable( int flag )
{
    if( flag )
    {
        string fullDB = ownerProj()->fullDB();
        string tbl = ownerProj()->tbl();

        //Remove from library table
        SYS->db().at().dataDel(fullDB,mod->nodePath()+tbl,*this);
	//Remove widget's IO from library IO table
        TConfig c_el(&mod->elWdgIO());
        c_el.cfg("IDW").setS(ownerFullId()+"/"+id());
        c_el.cfg("ID").setS("");
        SYS->db().at().dataDel(fullDB+"_io",mod->nodePath()+tbl+"_io",*this);
    }
}

string Page::name( )
{
    return (attrAt("name").at().getS().size())?attrAt("name").at().getS():m_id;
}

void Page::setName( const string &inm )      
{ 
    attrAt("name").at().setS(inm);
}

string Page::descr( )
{    
    return attrAt("dscr").at().getS();
}

void Page::setDescr( const string &idscr )   
{ 
    attrAt("dscr").at().setS(idscr);
}

string Page::ico( )
{
    if( m_ico.size() )          return m_ico;
    if( !parent().freeStat() )  return parent().at().ico();
    return "";
}

string Page::user( )
{
    return SYS->security().at().usrPresent(m_user)?m_user:Widget::user( );
}

void Page::setUser( const string &iuser )
{
    m_user = iuser;
    //Group update
    if(SYS->security().at().grpAt("UI").at().user(iuser))
        setGrp("UI");
    else
    {
        vector<string> gls;
        SYS->security().at().usrGrpList(user(),gls);
        setGrp(gls.size()?gls[0]:Widget::grp());
    }
}

string Page::grp( )
{
    return SYS->security().at().grpPresent(m_grp)?m_grp:Widget::grp( );
}

void Page::setParentNm( const string &isw )
{
    cfg("PARENT").setS(isw);
    Widget::setParentNm(isw);
}

string Page::calcLang( )
{
    if( !m_proc.size() && !parent().freeStat() )
        return parent().at().calcLang();

    string iprg = m_proc;
    if(iprg.find("\n") == string::npos)
    {
	iprg = iprg+"\n";
	m_proc = iprg;
    }
    return iprg.substr(0,iprg.find("\n"));
}

string Page::calcProg( )
{
    if( !m_proc.size() && !parent().freeStat() )
        return parent().at().calcProg();

    string iprg = m_proc;
    int lng_end = iprg.find("\n");
    if(lng_end == string::npos) lng_end=0;
    else lng_end++;
    return iprg.substr(lng_end);
}

void Page::setCalcLang( const string &ilng )
{
    m_proc = m_proc.replace(0,m_proc.find("\n"),ilng);
}

void Page::setCalcProg( const string &iprg )
{
    string tmp_prg = m_proc;
    int lng_end = tmp_prg.find("\n");
    if(lng_end == string::npos)
    {
        tmp_prg=tmp_prg+"\n";
        lng_end=tmp_prg.find("\n");
    }
    m_proc = tmp_prg.replace(lng_end+1,string::npos,iprg);
}

bool Page::isContainer( )
{
    if( !parent().freeStat() )    return parent().at().isContainer();
    return false;
}

void Page::load( )
{    
    //- Load generic widget's data -
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl();
    SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,*this);
    setParentNm(cfg("PARENT").getS());
    
    //- Create new pages -
    TConfig c_el(&mod->elPage());
    c_el.cfgViewAll(false);
    int fld_cnt = 0;
    c_el.cfg("OWNER").setS(ownerFullId()+"/"+id());
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
        string f_id = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");
        if( !pagePresent(f_id) ) pageAdd(f_id,"","");
    }
    //- Load present pages -
    vector<string> f_lst;
    pageList(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        pageAt(f_lst[i_ls]).at().load();
    
    //- Load widget attributes -
    loadIO();
}

void Page::loadIO( )
{
    //if( !enable() ) return;
    
    //- Load widget's attributes -
    TConfig c_el(&mod->elWdgIO());

    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl()+"_io";

    int fld_cnt=0;
    c_el.cfg("IDW").setS(ownerFullId()+"/"+id());
    c_el.cfg("ID").setS("");
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
	string sid = c_el.cfg("ID").getS();
	unsigned flg = c_el.cfg("IO_TYPE").getI();
	c_el.cfg("ID").setS("");

        if(!attrPresent(sid))
        {
	    if((flg>>4)&Attr::IsUser)
                attrAdd( new TFld(sid.c_str(),c_el.cfg("NAME").getS().c_str(),(TFld::Type)(flg&0x0f),flg>>4) );
	    else continue;
        }

	AutoHD<Attr> attr = attrAt(sid);
	
        if((flg>>4)&Attr::IsUser)
	{
	    attr.at().setS(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),0,'|'));
	    attr.at().fld().setValues(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),1,'|'));
	    attr.at().fld().setSelNames(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),2,'|'));
	}
	else attr.at().setS(c_el.cfg("IO_VAL").getS());
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)c_el.cfg("SELF_FLG").getI());
	attr.at().setCfgTempl(c_el.cfg("CFG_TMPL").getS());
	attr.at().setCfgVal(c_el.cfg("CFG_VAL").getS());
	
	//attr.at().setModifVal(0);
        //attr.at().setModifCfg(0);
    }
    
    //- Load cotainer widgets -
    if( !enable() || !isContainer() ) return;
    c_el.elem(&mod->elInclWdg());
    c_el.cfgViewAll(false);
    tbl=ownerProj()->tbl()+"_incl";
    c_el.cfg("IDW").setS(ownerFullId()+"/"+id());
    c_el.cfg("ID").setS("");
    fld_cnt=0;
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
        string sid  = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");
        if( !wdgPresent(sid) ) wdgAdd(sid,"","");
        wdgAt(sid).at().load();
    }
}

void Page::save( )
{
    vector<string> ls;
    
    //- Save generic widget's data -
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl();
    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,*this);

    //- Save include pages -
    pageList(ls);
    for( int i_l = 0; i_l < ls.size(); i_l++ )
	pageAt(ls[i_l]).at().save();

    //- Save widget's attributes -
    saveIO();

    //- Save cotainer widgets -
    if(isContainer())
    {
	wdgList(ls);
	for( int i_l = 0; i_l < ls.size(); i_l++ )
	    wdgAt(ls[i_l]).at().save();
    }
}

void Page::saveIO( )
{
    TConfig c_el(&mod->elWdgIO());
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl()+"_io";

    //Save allow IO
    vector<string> atrr_ls;
    attrList(atrr_ls);
    c_el.cfg("IDW").setS(ownerFullId()+"/"+id());
    for( int i_io = 0; i_io < atrr_ls.size(); i_io++ )
    {
	AutoHD<Attr> attr = attrAt(atrr_ls[i_io]);
	if( attr.at().modifVal() || attr.at().modifCfg() )
	{
	    c_el.cfg("ID").setS( atrr_ls[i_io] );
	    c_el.cfg("IO_TYPE").setI(0);
 	    if(attr.at().fld().flg()&Attr::IsUser)
	    {
		c_el.cfg("IO_VAL").setS(attr.at().getS()+"|"+
					attr.at().fld().values()+"|"+
					attr.at().fld().selNames());
		c_el.cfg("NAME").setS(attr.at().name());
		c_el.cfg("IO_TYPE").setI(attr.at().fld().type()+(attr.at().fld().flg()<<4));
    	    }
	    else c_el.cfg("IO_VAL").setS(attr.at().getS());
	    c_el.cfg("SELF_FLG").setI(attr.at().flgSelf());
	    c_el.cfg("CFG_TMPL").setS(attr.at().cfgTempl());
	    c_el.cfg("CFG_VAL").setS(attr.at().cfgVal());

	    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,c_el);
	    //attr.at().setModifVal(0);
	    //attr.at().setModifCfg(0);
	}
    }

    //Clear no present IO
    int fld_cnt=0;
    c_el.cfg("ID").setS("");
    c_el.cfgViewAll(false);
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
        string sid = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");
	if( isContainer() && wdgPresent(TSYS::strSepParse(sid,0,'_')) )  continue;
	//Erase record from table
        if(!attrPresent(sid))
        {
            SYS->db().at().dataDel(db+"."+tbl,mod->nodePath()+tbl,c_el);
            fld_cnt--;
        }
    }
}

void Page::setEnable( bool val )
{
    if( enable() == val ) return;
    
    Widget::setEnable(val);
    
    //- Enable/disable included pages -
    vector<string>      ls;
    pageList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
        pageAt(ls[i_l]).at().setEnable(val);
}

void Page::wdgAdd( const string &wid, const string &name, const string &path )
{
    if( !isContainer() )  throw TError(nodePath().c_str(),_("No container widget!"));
    if( wdgPresent(wid) ) return;

    chldAdd(inclWdg,new PageWdg(wid,path));
    wdgAt(wid).at().setName(name);
}

AutoHD<PageWdg> Page::wdgAt( const string &wdg )
{
    return Widget::wdgAt(wdg);
}

void Page::pageAdd( const string &id, const string &name, const string &orig )
{
    if( pagePresent(id) )return;
    chldAdd(m_page,new Page(id,orig));
    pageAt(id).at().setName(name);
}

void Page::pageAdd( Page *iwdg )
{
    if( pagePresent(iwdg->id()) )	delete iwdg;
    else chldAdd(m_page,iwdg);
}

AutoHD<Page> Page::pageAt( const string &id )
{
    return chldAt(m_page,id);
}

string Page::resourceGet( const string &id, string *mime )
{
    string mimeType;
    
    string mimeData = parent().at().resourceGet( id, &mimeType );
    if( mime )	*mime = mimeType;
    
    return mimeData;
}

void Page::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        cntrCmdGeneric(opt);
	if( !parent( ).freeStat() )
	{
    	    cntrCmdAttributes(opt);
    	    cntrCmdLinks(opt);
    	    cntrCmdProcess(opt);
	}
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Project page: ")+ownerFullId()+"/"+id());
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")) && ctrMkNode("area",opt,-1,"/wdg/cfg",_("Config")))
        {
	    if( ownerPage() && ownerPage()->prjFlag()&Page::Template &&
			       !(ownerPage()->prjFlag()&Page::Container) )
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),0444,user().c_str(),grp().c_str(),1,"tp","str");
	    ctrMkNode("fld",opt,7,"/wdg/cfg/pageCont",_("Page is container"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
	    ctrMkNode("fld",opt,8,"/wdg/cfg/pageTmpl",_("Page is template"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
	}
	if( prjFlag()&(Page::Template|Page::Container) )
	{
	    if(ctrMkNode("area",opt,1,"/page",_("Pages")))
    		ctrMkNode("list",opt,-1,"/page/page",_("Pages"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","pg_");
	    if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
		ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root","UI",1,"list","/page/page");
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");    
    if( cntrCmdGeneric(opt) || 
	(parent( ).freeStat() ? false : cntrCmdAttributes(opt) || cntrCmdLinks(opt) || cntrCmdProcess(opt)) )
    {
	if( a_path == "/wdg/w_lst" && ctrChkNode(opt) && ownerPage() && ownerPage()->prjFlag()&Page::Template )
	    opt->childIns(0,"el")->setText("..");    
    }
    else if( a_path == "/wdg/cfg/pageCont" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) 
	    opt->setText((prjFlag()&Page::Container)?"1":"0");
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) 
	    setPrjFlag(atoi(opt->text().c_str()) ? prjFlag()|Page::Container : prjFlag()&(~Page::Container));
    }
    else if( a_path == "/wdg/cfg/pageTmpl" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	    opt->setText((prjFlag()&Page::Template)?"1":"0");
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    setPrjFlag(atoi(opt->text().c_str()) ? prjFlag()|Page::Template : prjFlag()&(~Page::Template));
    }
    else if( a_path == "/page/page" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
    	    vector<string> lst;
            pageList(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->setAttr("id",lst[i_f])->setText(pageAt(lst[i_f]).at().name());
        }
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    pageAdd(opt->attr("id").c_str(),opt->text().c_str());
	    pageAt(opt->attr("id")).at().setUser(opt->attr("user"));
	}
        if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) pageDel(opt->attr("id"),true);
    }
}


//************************************************
//* Container stored widget                      *
//************************************************
PageWdg::PageWdg( const string &id, const string &isrcwdg ) :
        Widget(id,isrcwdg), TConfig(&mod->elInclWdg())
{
    cfg("ID").setS(id);
    m_lnk = true;

    setParentNm(isrcwdg);
}

PageWdg::~PageWdg( )
{

}

Page &PageWdg::owner()
{
    return *(Page*)nodePrev();
}

void PageWdg::postEnable( int flag )
{
    //- Set parent page for this widget -
    cfg("IDW").setS(owner().id());
    //- Set identifier -
    attrAt("id").at().setS(owner().ownerFullId(true)+"/pg_"+owner().id()+"/wdg_"+id());
    attrAt("id").at().setModifVal(0);
    attrAt("id").at().setModifCfg(0);
    //- Call parent method -
    Widget::postEnable(flag);
}

void PageWdg::postDisable(int flag)
{
    if( flag )
    {
        string fullDB = owner().ownerProj()->fullDB();
        string tbl    = owner().ownerProj()->tbl();

        //Remove from library table
        SYS->db().at().dataDel(fullDB+"_incl",mod->nodePath()+tbl+"_incl",*this);
        //Remove widget's IO from library IO table
        TConfig c_el(&mod->elWdgIO());
	c_el.cfgViewAll(false);
        c_el.cfg("IDW").setS(owner().ownerFullId()+"/"+owner().id());
        c_el.cfg("ID").setS("");
        int fld_cnt=0;
        while( SYS->db().at().dataSeek(fullDB+"_io",mod->nodePath()+tbl+"_io",fld_cnt++,c_el) )
        {
            if(c_el.cfg("ID").getS().find(id()+"_")==0)
            {
                SYS->db().at().dataDel(fullDB+"_io",mod->nodePath()+tbl+"_io",c_el);
                fld_cnt--;
            }
            c_el.cfg("ID").setS("");
        }
    }
}

string PageWdg::name( )
{
    return (attrAt("name").at().getS().size())?attrAt("name").at().getS():m_id;
}

void PageWdg::setName( const string &inm )      
{ 
    attrAt("name").at().setS(inm);
}

string PageWdg::descr( )
{    
    return attrAt("dscr").at().getS();
}

void PageWdg::setDescr( const string &idscr )   
{ 
    attrAt("dscr").at().setS(idscr);
}

string PageWdg::ico( )
{
    if( !parent().freeStat() )  return parent().at().ico();
    return "";
}

string PageWdg::user( )
{
    return owner().user();
}

string PageWdg::grp( )
{
    return owner().grp( );
}

short PageWdg::permit( )
{
    return owner().permit( );
}

void PageWdg::setParentNm( const string &isw )
{
    cfg("PARENT").setS(isw);
    Widget::setParentNm(isw);
}

string PageWdg::calcLang( )
{
    if( !parent().freeStat() )    return parent().at().calcLang();
    return "";
}

string PageWdg::calcProg( )
{
    if( !parent().freeStat() )    return parent().at().calcProg();
    return "";
}

void PageWdg::load( )
{
    //Load generic widget's data
    string db  = owner().ownerProj()->DB();
    string tbl = owner().ownerProj()->tbl()+"_incl";
    SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,*this);
    setParentNm(cfg("PARENT").getS());

    //Load widget's attributes
    loadIO();
}

void PageWdg::loadIO( )
{
    TConfig c_el(&mod->elWdgIO());

    string db  = owner().ownerProj()->DB();
    string tbl = owner().ownerProj()->tbl()+"_io";

    int fld_cnt=0;
    c_el.cfg("IDW").setS(owner().ownerFullId()+"/"+owner().id());
    c_el.cfg("ID").setS("");
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
	string sid = c_el.cfg("ID").getS();
	unsigned flg = c_el.cfg("IO_TYPE").getI();
	c_el.cfg("ID").setS("");

        if( sid.find(id()+"_")==0 ) sid.erase(0,id().size()+1);
        else continue;

        if(!attrPresent(sid))
        {
	    if((flg>>4)&Attr::IsUser)
                attrAdd( new TFld(sid.c_str(),c_el.cfg("NAME").getS().c_str(),(TFld::Type)(flg&0x0f),flg>>4) );
	    else continue;
        }
	
	AutoHD<Attr> attr = attrAt(sid);

        if((flg>>4)&Attr::IsUser)
	{
	    attr.at().setS(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),0,'|'));
	    attr.at().fld().setValues(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),1,'|'));
	    attr.at().fld().setSelNames(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),2,'|'));
	}
	else attr.at().setS(c_el.cfg("IO_VAL").getS());

	attr.at().setFlgSelf((Attr::SelfAttrFlgs)c_el.cfg("SELF_FLG").getI());
	attr.at().setCfgTempl(c_el.cfg("CFG_TMPL").getS());
	attr.at().setCfgVal(c_el.cfg("CFG_VAL").getS());

	//attr.at().setModifVal(0);
	//attr.at().setModifCfg(0);
    }
}

void PageWdg::save( )
{
    //Save generic widget's data
    string db  = owner().ownerProj()->DB();
    string tbl = owner().ownerProj()->tbl()+"_incl";
    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,*this);

    //Save widget's attributes
    saveIO();
}

void PageWdg::saveIO( )
{
    TConfig c_el(&mod->elWdgIO());
    string db  = owner().ownerProj()->DB();
    string tbl = owner().ownerProj()->tbl()+"_io";

    //Save allow IO
    vector<string> atrr_ls;
    attrList(atrr_ls);
    c_el.cfg("IDW").setS(owner().ownerFullId()+"/"+owner().id());
    for( int i_io = 0; i_io < atrr_ls.size(); i_io++ )
    {
	AutoHD<Attr> attr = attrAt(atrr_ls[i_io]);    
	if( attr.at().modifVal() || attr.at().modifCfg() )
	{
	    c_el.cfg("ID").setS( id()+"_"+atrr_ls[i_io] );
	    c_el.cfg("IO_TYPE").setI(0);
 	    if(attr.at().fld().flg()&Attr::IsUser)
	    {
		c_el.cfg("IO_VAL").setS(attr.at().getS()+"|"+
					attr.at().fld().values()+"|"+
					attr.at().fld().selNames());
		c_el.cfg("NAME").setS(attr.at().name());
		c_el.cfg("IO_TYPE").setI(attr.at().fld().type()+(attr.at().fld().flg()<<4));
    	    }
	    else c_el.cfg("IO_VAL").setS(attr.at().getS());
	    c_el.cfg("SELF_FLG").setI(attr.at().flgSelf());
	    c_el.cfg("CFG_TMPL").setS(attr.at().cfgTempl());
	    c_el.cfg("CFG_VAL").setS(attr.at().cfgVal());

	    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,c_el);
	    //attr.at().setModifVal(0);
	    //attr.at().setModifCfg(0);
	}
    }

    //Clear no present IO
    int fld_cnt=0;
    c_el.cfg("ID").setS("");
    c_el.cfgViewAll(false);
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
        string sid = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");

        if(sid.find(id()+"_")==0)   sid.erase(0,id().size()+1);
	else continue;

        //Erase record from table
        if(!attrPresent(sid))
        {
            SYS->db().at().dataDel(db+"."+tbl,mod->nodePath()+tbl,c_el);
            fld_cnt--;
        }
    }
}

string PageWdg::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;
    
    mimeData = parent().at().resourceGet( id, &mimeType );    
    if( mime )	*mime = mimeType;
    
    return mimeData;
}

void PageWdg::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        cntrCmdGeneric(opt);
        cntrCmdAttributes(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget link: ")+id());
        return;
    }
    cntrCmdGeneric(opt) || cntrCmdAttributes(opt);
}

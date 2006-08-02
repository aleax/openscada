
//OpenSCADA system file: tprmtmpl.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
 
#include "tsys.h"
#include "tprmtmpl.h"

TPrmTempl::TPrmTempl( const char *iid, const char *iname ) : 
    TConfig(&SYS->daq().at().tplE()), m_en(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_func(cfg("FUNC").getSd())
{
    m_id = iid;
    m_name = iname;
}

TPrmTempl::~TPrmTempl(  )
{

}

void TPrmTempl::postDisable(int flag)
{
    try
    {
        if( flag )
	{
            SYS->db().at().dataDel(owner().BD(),owner().owner().nodePath()+owner().tbl(),*this);
	    //Delete template's IO
	    TConfig cfg(&owner().owner().tplIOE());
	    cfg.cfg("TMPL_ID").setS(id());
	    cfg.cfg("ID").setS("");
	    SYS->db().at().dataDel(owner().BD()+"_io",owner().owner().nodePath()+owner().tbl()+"_io/",cfg);
	}
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,"%s",err.mess.c_str()); }
}

string TPrmTempl::name()
{ 
    return (m_name.size())?m_name:m_id;
}

void TPrmTempl::enable( bool vl )
{
    if( vl )
    {
	if( !func().freeStat() )
	{
	    m_en = vl;
	    attrUp();
	}    
    }
    else
    {
	m_en = vl;
	m_io.clear();
    }
}

AutoHD<TFunction> TPrmTempl::func()
{
    return SYS->nodeAt(m_func,0,'.');
}

void TPrmTempl::attrUp()
{
    if( enable() )
    {
	//Create io template configs
	AutoHD<TFunction> w_fnc = func(); 
	for( int i_io = 0; i_io < w_fnc.at().ioSize(); i_io++ )
	    if( attrId(w_fnc.at().io(i_io)->id()) < 0 )
		m_io.push_back(SIOPrm(w_fnc.at().io(i_io)->id(),TPrmTempl::NoAttr,TPrmTempl::Const,w_fnc.at().io(i_io)->def()));
	//Remove io template configs
	for( int i_io = 0; i_io < attrSize(); i_io++ )	
	    if( w_fnc.at().ioId(attr(i_io).id) < 0 )
	    {
		m_io.erase(m_io.begin()+i_io);	
		i_io--;
	    }    
    
	//Load template configs from DB
	TConfig cfg(&owner().owner().tplIOE());
	cfg.cfg("TMPL_ID").setS(id());
	
	for( int i_io = 0; i_io < attrSize(); i_io++ )
	{ 
	    cfg.cfg("ID").setS(attr(i_io).id);
	    if(!SYS->db().at().dataGet(owner().BD()+"_io",owner().owner().nodePath()+owner().tbl()+"_io/",cfg))
	        continue;
	    attr(i_io).attr = (TPrmTempl::AttrMode)cfg.cfg("ATTR_MODE").getI();
	    attr(i_io).accs = (TPrmTempl::AccMode)cfg.cfg("ACCS_MODE").getI();
	    attr(i_io).val  = cfg.cfg("VALUE").getS();
	}
    }
}

void TPrmTempl::attrSave()
{
    //Save template configs to DB
    TConfig cfg(&owner().owner().tplIOE());
    cfg.cfg("TMPL_ID").setS(id());
    
    for( int i_io = 0; i_io < attrSize(); i_io++ )
    {
	cfg.cfg("ID").setS(attr(i_io).id);
	cfg.cfg("ATTR_MODE").setI(attr(i_io).attr);
	cfg.cfg("ACCS_MODE").setI(attr(i_io).accs);
	cfg.cfg("VALUE").setS(attr(i_io).val);
	SYS->db().at().dataSet(owner().BD()+"_io",owner().owner().nodePath()+owner().tbl()+"_io/",cfg);
    }
}

int TPrmTempl::attrId( const string &id )
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	if( m_io[i_io].id == id )
	    return i_io;
    return -1;
}

TPrmTempl::SIOPrm &TPrmTempl::attr( int id )
{
    if( id < 0 || id >= m_io.size() )
	throw TError(nodePath().c_str(),Mess->I18N("Parameter id error."));
    return m_io[id];
}

void TPrmTempl::load( )
{
    SYS->db().at().dataGet(owner().BD(),owner().owner().nodePath()+owner().tbl(),*this);
    attrUp();
}
    
void TPrmTempl::save( )
{
    SYS->db().at().dataSet(owner().BD(),owner().owner().nodePath()+owner().tbl(),*this);
    attrSave();    
}	

void TPrmTempl::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",Mess->I18N("Parameter template: ")+name());
	ctrMkNode("area",opt,-1,"/tpl",Mess->I18N("Template"));
	ctrMkNode("area",opt,-1,"/tpl/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,"/tpl/st/en",Mess->I18N("Enable"),0664,"root","root",1,"tp","bool");		
	ctrMkNode("area",opt,-1,"/tpl/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,-1,"/tpl/cfg/id",Mess->I18N("Id"),0444,"root","root",1,"tp","str");
        ctrMkNode("fld",opt,-1,"/tpl/cfg/name",Mess->I18N("Name"),0664,"root","root",1,"tp","str");
        ctrMkNode("fld",opt,-1,"/tpl/cfg/descr",Mess->I18N("Description"),0664,"root","root",3,"tp","str","cols","60","rows","4");
	ctrMkNode("fld",opt,-1,"/tpl/cfg/fncp",Mess->I18N("Function"),enable()?0444:0664,"root","root",3,"tp","str","dest","sel_ed","select","/tpl/cfg/fncp_list");
	ctrMkNode("comm",opt,-1,"/tpl/cfg/f_lnk",Mess->I18N("Go to function"),0440,"root","root",1,"tp","lnk");
	ctrMkNode("comm",opt,-1,"/tpl/cfg/load",Mess->I18N("Load"),0440);
        ctrMkNode("comm",opt,-1,"/tpl/cfg/save",Mess->I18N("Save"),0440);
	if( enable() )
	{
	    ctrMkNode("area",opt,-1,"/cfg",Mess->I18N("Configuration"));
	    ctrMkNode("table",opt,-1,"/cfg/io",Mess->I18N("IO"),0664,"root","root",1,"rows","15");
	    ctrMkNode("list",opt,-1,"/cfg/io/0",Mess->I18N("Id"),0444,"root","root",1,"tp","str");
    	    ctrMkNode("list",opt,-1,"/cfg/io/1",Mess->I18N("Name"),0444,"root","root",1,"tp","str");
    	    ctrMkNode("list",opt,-1,"/cfg/io/2",Mess->I18N("Attribute"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/cfg/attr_mods");
    	    ctrMkNode("list",opt,-1,"/cfg/io/3",Mess->I18N("Access"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/cfg/accs_mods");
    	    ctrMkNode("list",opt,-1,"/cfg/io/4",Mess->I18N("Value"),0664,"root","root",1,"tp","str");
	}
	return;
    }
    //Process command to page
    vector<string> list;    
    string a_path = opt->attr("path");
    if( a_path == "/tpl/st/en" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(enable()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	enable(atoi(opt->text().c_str()));
    }	
    else if( a_path == "/tpl/cfg/id" && ctrChkNode(opt) )	opt->text(id());
    else if( a_path == "/tpl/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )  	opt->text(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	name(opt->text());
    }	
    else if( a_path == "/tpl/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(descr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	descr(opt->text());
    }
    else if( a_path == "/tpl/cfg/fncp" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(m_func);
	if( ctrChkNode(opt,"set",enable()?0444:0664,"root","root",SEQ_WR) )	
	{
	    m_func = opt->text();
            try{ attrUp(); } catch(...){ }
	}
    }
    else if( a_path == "/tpl/cfg/fncp_list" && ctrChkNode(opt) )
    {
        int c_lv = 0;
        string c_path = "";
        while(TSYS::strSepParse(m_func,c_lv,'.').size())
        {
            opt->childAdd("el")->text(c_path);
            if( c_lv ) c_path+=".";
            c_path = c_path+TSYS::strSepParse(m_func,c_lv,'.');
    	    c_lv++;
        }
        opt->childAdd("el")->text(c_path);
        if( c_lv != 0 ) c_path += ".";
        SYS->nodeAt(c_path,0,'.').at().nodeList(list);
        for( unsigned i_a=0; i_a < list.size(); i_a++ )
            opt->childAdd("el")->text(c_path+list[i_a]);
    }
    else if( a_path == "/tpl/cfg/f_lnk" && ctrChkNode(opt,"get",0440) )
    {
	    int c_lv = 0;
	    string path = "/";
	    while(TSYS::strSepParse(m_func,c_lv,'.').size())
	        path+=TSYS::strSepParse(m_func,c_lv++,'.')+"/";
	    opt->text(path);
    }	    
    else if( a_path == "/cfg/io" && enable() )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
    	{
	    XMLNode *n_id   = ctrMkNode("list",opt,-1,"/cfg/io/0","");
    	    XMLNode *n_nm   = ctrMkNode("list",opt,-1,"/cfg/io/1","");
    	    XMLNode *n_attr = ctrMkNode("list",opt,-1,"/cfg/io/2","",0664);
    	    XMLNode *n_accs = ctrMkNode("list",opt,-1,"/cfg/io/3","",0664);
    	    XMLNode *n_val  = ctrMkNode("list",opt,-1,"/cfg/io/4","",0664);

	    for( int id = 0; id < attrSize(); id++ )
    	    {
                if(n_id) 	n_id->childAdd("el")->text(attr(id).id);
                if(n_nm) 	n_nm->childAdd("el")->text(func().at().io(func().at().ioId(attr(id).id))->name());
		if(n_attr)	n_attr->childAdd("el")->text(TSYS::int2str(attr(id).attr));
		if(n_accs)	n_accs->childAdd("el")->text(TSYS::int2str(attr(id).accs));
		if(n_val)	n_val->childAdd("el")->text(attr(id).val);
	    }
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    int row = atoi(opt->attr("row").c_str());
	    switch(atoi(opt->attr("col").c_str()))
	    {
		case 2:	attr(row).attr = (TPrmTempl::AttrMode)atoi(opt->text().c_str());	break;
		case 3:	attr(row).accs = (TPrmTempl::AccMode)atoi(opt->text().c_str());	break;
		case 4: attr(row).val  = opt->text();	break;
	    }
	}
    }	 
    else if( a_path == "/cfg/attr_mods" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->attr("id",TSYS::int2str(TPrmTempl::NoAttr))->text(Mess->I18N("No attribute"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TPrmTempl::ReadOnly))->text(Mess->I18N("Read only"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TPrmTempl::FullAccess))->text(Mess->I18N("Full access"));
    }	    
    else if( a_path == "/cfg/accs_mods" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->attr("id",TSYS::int2str(TPrmTempl::Const))->text(Mess->I18N("Constant"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TPrmTempl::PublConst))->text(Mess->I18N("Public constant"));	
	opt->childAdd("el")->attr("id",TSYS::int2str(TPrmTempl::Link))->text(Mess->I18N("Link"));
    }
    else if( a_path == "/tpl/cfg/load" && ctrChkNode(opt,"set",0440) )	load();
    else if( a_path == "/tpl/cfg/save" && ctrChkNode(opt,"set",0440) )	save();
}
	    
//=================== TPrmTmplLib =======================
TPrmTmplLib::TPrmTmplLib( const char *id, const char *name, const string &lib_db ) :
    TConfig(&SYS->daq().at().elLib()), run_st(false), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_db(cfg("DB").getSd()), work_lib_db(lib_db)
{
    m_id = id;
    m_name = name;
    m_db = string("tmplib_")+id;
    m_ptmpl = grpAdd("tmpl_");
}
			
TPrmTmplLib::~TPrmTmplLib()
{

}
			
void TPrmTmplLib::preDisable(int flag)
{
    start(false);
}

void TPrmTmplLib::postDisable(int flag)
{
    if( flag )
    {
        //Delete libraries record
        SYS->db().at().dataDel(work_lib_db+"."+owner().tmplLibTable(),owner().nodePath()+"tmplib",*this);
			
        //Delete temlate librarie's DBs
        SYS->db().at().open(BD());
        SYS->db().at().close(BD(),true);
						
        SYS->db().at().open(BD()+"_io");
        SYS->db().at().close(BD()+"_io",true);
    }
}
								    
string TPrmTmplLib::name()
{
    return (m_name.size())?m_name:m_id;
}
									
void TPrmTmplLib::load( )
{
    SYS->db().at().dataGet(work_lib_db+"."+owner().tmplLibTable(),owner().nodePath()+"tmplib",*this);

    //Load templates
    TConfig c_el(&owner().tplE());
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(BD(),owner().nodePath()+tbl(), fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();

        if( !present(f_id) )    add(f_id.c_str());
        at(f_id).at().load();
        c_el.cfg("ID").setS("");
    }
}

void TPrmTmplLib::save( )
{
    SYS->db().at().dataSet(work_lib_db+"."+owner().tmplLibTable(),owner().nodePath()+"tmplib",*this);
    
    //Save functions
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
        at(f_lst[i_ls]).at().save();
}
			    
void TPrmTmplLib::start( bool val )
{
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
        at(lst[i_f]).at().enable(val);
						
    run_st = val;
}

void TPrmTmplLib::add( const char *id, const char *name )
{
    chldAdd(m_ptmpl,new TPrmTempl(id,name));
}
	
string TPrmTmplLib::BD()
{
    return work_lib_db+'.'+m_db;
}

void TPrmTmplLib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",Mess->I18N("Parameter templates library: ")+id());
	ctrMkNode("branches",opt,-1,"/br","",0444);
	ctrMkNode("grp",opt,-1,"/br/tmpl_",Mess->I18N("Opened DB"),0444,"root","root",1,"list","/tmpl/tmpl");
        ctrMkNode("area",opt,-1,"/lib",Mess->I18N("Library"));
        ctrMkNode("area",opt,-1,"/lib/st",Mess->I18N("State"));
        ctrMkNode("fld",opt,-1,"/lib/st/st",Mess->I18N("Accessing"),0664,"root","root",1,"tp","bool");
        ctrMkNode("fld",opt,-1,"/lib/st/bd",Mess->I18N("Library BD (module.bd.table)"),0660,"root","root",1,"tp","str");
        ctrMkNode("area",opt,-1,"/lib/cfg",Mess->I18N("Config"));
        ctrMkNode("fld",opt,-1,"/lib/cfg/id",Mess->I18N("Id"),0444,"root","root",1,"tp","str");
        ctrMkNode("fld",opt,-1,"/lib/cfg/name",Mess->I18N("Name"),0664,"root","root",1,"tp","str");
        ctrMkNode("fld",opt,-1,"/lib/cfg/descr",Mess->I18N("Description"),0664,"root","root",3,"tp","str","cols","50","rows","3");
        ctrMkNode("comm",opt,-1,"/lib/cfg/load",Mess->I18N("Load"),0440);
        ctrMkNode("comm",opt,-1,"/lib/cfg/save",Mess->I18N("Save"),0440);
        ctrMkNode("area",opt,-1,"/tmpl",Mess->I18N("Parameter templates"));
        ctrMkNode("list",opt,-1,"/tmpl/tmpl",Mess->I18N("Templates"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","tmpl_");
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
    else if( a_path == "/tmpl/tmpl" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> lst;
    	    list(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
        	opt->childAdd("el")->attr("id",lst[i_f])->text(at(lst[i_f]).at().name());
        }
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	add(opt->attr("id").c_str(),opt->text().c_str());
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	del(opt->attr("id").c_str(),true);
    }
    else if( a_path == "/lib/cfg/load" && ctrChkNode(opt,"set",0440) )	load();
    else if( a_path == "/lib/cfg/save" && ctrChkNode(opt,"set",0440) )	save();
}

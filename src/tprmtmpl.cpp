
//OpenSCADA system file: tprmtmpl.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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
 
#include "tsys.h"
#include "tprmtmpl.h"

//*************************************************
//* TPrmTempl                                     *
//*************************************************
TPrmTempl::TPrmTempl( const char *iid, const char *iname ) : 
    TFunction(string("tmpl_")+iid), TConfig(&SYS->daq().at().tplE()), 
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_prog(cfg("PROGRAM").getSd())
{
    m_id = iid;
    m_name = iname;
}

TPrmTempl::~TPrmTempl(  )
{

}

void TPrmTempl::postEnable( int flag )
{
    //- Create default IOs -
    if( flag&TCntrNode::NodeConnect )
    {
	ioIns( new IO("f_frq",_("Function calculate frequency (Hz)"),IO::Real,TPrmTempl::LockAttr,"1000",false),0);
	ioIns( new IO("f_start",_("Function start flag"),IO::Boolean,TPrmTempl::LockAttr,"0",false),1);
	ioIns( new IO("f_stop",_("Function stop flag"),IO::Boolean,TPrmTempl::LockAttr,"0",false),2);
	ioIns( new IO("f_err",_("Function error"),IO::String,TPrmTempl::LockAttr,"0",false),3);
    }
}

void TPrmTempl::postDisable(int flag)
{
    try
    {
        if( flag )
	{
            SYS->db().at().dataDel(owner().fullDB(),owner().owner().nodePath()+owner().tbl(),*this);
	    //- Delete template's IO -
	    TConfig cfg(&owner().owner().tplIOE());
	    cfg.cfg("TMPL_ID").setS(id());
	    cfg.cfg("ID").setS("");
	    SYS->db().at().dataDel(owner().fullDB()+"_io",owner().owner().nodePath()+owner().tbl()+"_io/",cfg);
	}
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TPrmTempl::name()
{ 
    return (m_name.size())?m_name:m_id;
}

string TPrmTempl::progLang()
{
    if(m_prog.find("\n")==string::npos)	m_prog=m_prog+"\n";
    return m_prog.substr(0,m_prog.find("\n"));    
}

string TPrmTempl::prog()
{
    int lng_end = m_prog.find("\n");
    if(lng_end == string::npos)	lng_end=0;
    else lng_end++;
    return m_prog.substr(lng_end);
}
 
void TPrmTempl::setProgLang( const string &ilng )
{
    if(startStat()) setStart(false);
    
    m_prog.replace(0,m_prog.find("\n"),ilng);
}

void TPrmTempl::setProg( const string &iprg )
{
    if(startStat()) setStart(false);

    int lng_end = m_prog.find("\n");
    if(lng_end == string::npos)
    {	
	m_prog=m_prog+"\n";
	lng_end=m_prog.find("\n");
    }
    m_prog.replace(lng_end+1,string::npos,iprg);
}

void TPrmTempl::setStart( bool vl )
{
    if( startStat() == vl ) return;
    if(vl)
    {
	//Check old compile function	????
    
	//- Compile new function -
	if(prog().size())
	    work_prog = SYS->daq().at().at(TSYS::strSepParse(progLang(),0,'.')).at().
				        compileFunc(TSYS::strSepParse(progLang(),1,'.'),*this,prog());
    }
    TFunction::setStart(vl);
}

AutoHD<TFunction> TPrmTempl::func()
{
    if(!startStat())	throw TError(nodePath().c_str(),_("Tempate is disabled."));
    if(!prog().size())	return AutoHD<TFunction>(this);
    return SYS->nodeAt(work_prog,1);
}

void TPrmTempl::load( )
{
    //- Self load -
    SYS->db().at().dataGet(owner().fullDB(),owner().owner().nodePath()+owner().tbl(),*this);
    
    //- Load IO -
    vector<string> u_pos;
    TConfig cfg(&owner().owner().tplIOE());
    cfg.cfg("TMPL_ID").setS(id());
    int io_cnt = 0;    
    while(SYS->db().at().dataSeek(owner().fullDB()+"_io",owner().owner().nodePath()+owner().tbl()+"_io",io_cnt++,cfg) )
    {
	string sid = cfg.cfg("ID").getS();
	cfg.cfg("ID").setS("");
	
	//- Position storing -
	int pos = cfg.cfg("POS").getI();	
	while( u_pos.size() <= pos )	u_pos.push_back("");
	u_pos[pos] = sid;	

	int iid = ioId(sid);
	if(iid < 0)
	    ioIns( new IO(sid.c_str(),cfg.cfg("NAME").getS().c_str(),(IO::Type)cfg.cfg("TYPE").getI(),cfg.cfg("FLAGS").getI(),
			cfg.cfg("VALUE").getS().c_str(),false), pos );
	else
	{
	    io(iid)->setName(cfg.cfg("NAME").getS());
	    io(iid)->setType((IO::Type)cfg.cfg("TYPE").getI());
	    io(iid)->setFlg(cfg.cfg("FLAGS").getI());
	    io(iid)->setDef(cfg.cfg("VALUE").getS());
	}
    }
    //- Position fixing -
    for( int i_p = 0; i_p < u_pos.size(); i_p++ )
    {
	if( u_pos[i_p].empty() ) continue;
	int iid = ioId(u_pos[i_p]);
	if( iid != i_p ) 
	    try{ ioMove(iid,i_p); } catch(...){ }
    }
}
    
void TPrmTempl::save( )
{
    string w_db = owner().fullDB();
    string w_cfgpath = owner().owner().nodePath()+owner().tbl();
    //- Self save -
    SYS->db().at().dataSet(w_db,w_cfgpath,*this);

    //- Save IO -
    TConfig cfg(&owner().owner().tplIOE());
    cfg.cfg("TMPL_ID").setS(id());
    for(int i_io = 0; i_io < ioSize(); i_io++)
    {
	if( io(i_io)->id() == "f_frq" || io(i_io)->id() == "f_start" || 
		io(i_io)->id() == "f_stop" || io(i_io)->id() == "f_err" ) continue;
	cfg.cfg("ID").setS(io(i_io)->id());
	cfg.cfg("NAME").setS(io(i_io)->name());
    	cfg.cfg("TYPE").setI(io(i_io)->type());
	cfg.cfg("FLAGS").setI(io(i_io)->flg());
    	cfg.cfg("VALUE").setS(io(i_io)->def());
	cfg.cfg("POS").setI(i_io);
    	SYS->db().at().dataSet(w_db+"_io",w_cfgpath+"_io",cfg);
    }
    //- Clear IO -
    int fld_cnt=0;
    cfg.cfg("ID").setS("");
    cfg.cfgViewAll(false);
    while( SYS->db().at().dataSeek(w_db+"_io",w_cfgpath+"_io",fld_cnt++,cfg ) )
    {
	string sio = cfg.cfg("ID").getS( );
        if( ioId(sio) < 0 || sio == "f_frq" || sio == "f_start" || 
		sio == "f_stop" || sio == "f_err" )
        {
            SYS->db().at().dataDel(w_db+"_io",w_cfgpath+"_io",cfg);
            fld_cnt--;
        }
        cfg.cfg("ID").setS("");
    }
}	

void TPrmTempl::preIOCfgChange()
{
    if(startStat()) setStart(false);
}

void TPrmTempl::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter template: ")+name());
	if(ctrMkNode("area",opt,-1,"/tmpl",_("Template")))
	{
 	    if(ctrMkNode("area",opt,-1,"/tmpl/st",_("State")))
    		ctrMkNode("fld",opt,-1,"/tmpl/st/st",_("Accessing"),0664,"root","root",1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/tmpl/cfg",_("Config")))
	    { 
		ctrMkNode("fld",opt,-1,"/tmpl/cfg/id",_("Id"),0444,"root","root",1,"tp","str");
	    	ctrMkNode("fld",opt,-1,"/tmpl/cfg/name",_("Name"),0664,"root","root",1,"tp","str");
	    	ctrMkNode("fld",opt,-1,"/tmpl/cfg/descr",_("Description"),0664,"root","root",3,"tp","str","cols","70","rows","4");
    	    	ctrMkNode("comm",opt,-1,"/tmpl/cfg/load",_("Load"),0660);
	    	ctrMkNode("comm",opt,-1,"/tmpl/cfg/save",_("Save"),0660);
	    }
	}
	if( ctrMkNode("area",opt,-1,"/io",_("IO")) )
	{
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),0664,"root","root",2,"s_com","add,del,ins,move","rows","15"))
	    {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Id"),0664,"root","root",1,"tp","str");
    		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/io/tp");
                ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/io/md");
    		ctrMkNode("list",opt,-1,"/io/io/4",_("Attribute"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/io/attr_mods");
    		ctrMkNode("list",opt,-1,"/io/io/5",_("Access"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/io/accs_mods");
    		ctrMkNode("list",opt,-1,"/io/io/6",_("Value"),0664,"root","root",1,"tp","str");
	    }
	    ctrMkNode("fld",opt,-1,"/io/prog_lang",_("Programm language"),0664,"root","root",3,"tp","str","dest","sel_ed","select","/io/plang_ls");
	    ctrMkNode("fld",opt,-1,"/io/prog",_("Programm"),0664,"root","root",3,"tp","str","cols","90","rows","8");
	}
	return;
    }
    
    //- Process command to page -
    vector<string> list;    
    string a_path = opt->attr("path");
    if( a_path == "/tmpl/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(startStat()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setStart(atoi(opt->text().c_str()));
    } 
    else if( a_path == "/tmpl/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/tmpl/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setName(opt->text());
    }
    else if( a_path == "/tmpl/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(descr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDescr(opt->text());
    }
    else if( a_path == "/io/io" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
    	{
	    XMLNode *n_id   = ctrMkNode("list",opt,-1,"/io/io/0","");
    	    XMLNode *n_nm   = ctrMkNode("list",opt,-1,"/io/io/1","");
	    XMLNode *n_type = ctrMkNode("list",opt,-1,"/io/io/2","");
	    XMLNode *n_mode = ctrMkNode("list",opt,-1,"/io/io/3","");
    	    XMLNode *n_attr = ctrMkNode("list",opt,-1,"/io/io/4","");
    	    XMLNode *n_accs = ctrMkNode("list",opt,-1,"/io/io/5","");
    	    XMLNode *n_val  = ctrMkNode("list",opt,-1,"/io/io/6","");

	    for( int id = 0; id < ioSize(); id++ )
	    {
    		if(n_id)        n_id->childAdd("el")->setText(io(id)->id());
		if(n_nm)        n_nm->childAdd("el")->setText(io(id)->name());
		if(n_type)      n_type->childAdd("el")->setText(TSYS::int2str(io(id)->type()));
		if(n_mode)      n_mode->childAdd("el")->setText(TSYS::int2str(io(id)->flg()&(IO::Output|IO::Return)));
		if(n_attr)	n_attr->childAdd("el")->setText(TSYS::int2str(io(id)->flg()&(TPrmTempl::AttrRead|TPrmTempl::AttrFull)));
		if(n_accs)	n_accs->childAdd("el")->setText(TSYS::int2str(io(id)->flg()&(TPrmTempl::CfgPublConst|TPrmTempl::CfgLink)));
		if(n_val)	n_val->childAdd("el")->setText(io(id)->def());
	    }
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )   ioAdd( new IO("new",_("New IO"),IO::Real,IO::Default) );
	if( ctrChkNode(opt,"ins",0664,"root","root",SEQ_WR) )   ioIns( new IO("new",_("New IO"),IO::Real,IO::Default), atoi(opt->attr("row").c_str()) );
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )
	{
	    int row = atoi(opt->attr("row").c_str());    
	    if(io(row)->flg()&TPrmTempl::LockAttr)
		throw TError(nodePath().c_str(),_("Deleting lock atribute in not allow."));
	    ioDel( atoi(opt->attr("row").c_str()) );
	}
	if( ctrChkNode(opt,"move",0664,"root","root",SEQ_WR) )  ioMove( atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    int row = atoi(opt->attr("row").c_str());
	    int col = atoi(opt->attr("col").c_str());
	    if( (col == 0 || col == 1) && !opt->text().size() )
    		throw TError(nodePath().c_str(),_("Empty value no valid."));
	    if( io(row)->flg()&TPrmTempl::LockAttr )
		throw TError(nodePath().c_str(),_("Change lock atribute in not allow."));
	    switch(col)
	    {
		case 0:	io(row)->setId(opt->text());	break;
		case 1:	io(row)->setName(opt->text());	break;
		case 2:	io(row)->setType((IO::Type)atoi(opt->text().c_str()));	break;
		case 3:	io(row)->setFlg(io(row)->flg()^((io(row)->flg()^atoi(opt->text().c_str()))&(IO::Output|IO::Return)));		break;
		case 4:	io(row)->setFlg(io(row)->flg()^((io(row)->flg()^atoi(opt->text().c_str()))&(TPrmTempl::AttrRead|TPrmTempl::AttrFull)));	break;
		case 5:	io(row)->setFlg(io(row)->flg()^((io(row)->flg()^atoi(opt->text().c_str()))&(TPrmTempl::CfgPublConst|TPrmTempl::CfgLink)));	break;
		case 6:	io(row)->setDef(opt->text());	break;
	    }
	}
    }
    else if( a_path == "/io/prog_lang" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(progLang());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )   setProgLang(opt->text());
    }
    else if( a_path == "/io/prog" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )   opt->setText(prog());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )   setProg(opt->text()); 
    }
    else if( a_path == "/io/plang_ls" && ctrChkNode(opt) )
    {
	string tplng = progLang();
        int c_lv = 0;
        string c_path = "", c_el;
        opt->childAdd("el")->setText(c_path);
        for( int c_off = 0; (c_el=TSYS::strSepParse(tplng,0,'.',&c_off)).size(); c_lv++ )
        {
            c_path += c_lv ? "."+c_el : c_el;
            opt->childAdd("el")->setText(c_path);
        }
        if(c_lv) c_path+=".";
        vector<string>  ls;
        switch(c_lv)
        {
            case 0:	SYS->daq().at().modList(ls);	break;
            case 1:	
                if( SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')) )
            	    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(ls);
                break;
        }
	for(int i_l = 0; i_l < ls.size(); i_l++)
    	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if( a_path == "/io/tp" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Real))->setText(_("Real"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Integer))->setText(_("Integer"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Boolean))->setText(_("Boolean"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::String))->setText(_("String"));
    }
    else if( a_path == "/io/md" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Default))->setText(_("Input"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Output))->setText(_("Output"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Return))->setText(_("Return"));
    }    
    else if( a_path == "/io/attr_mods" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Default))->setText(_("No attribute"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TPrmTempl::AttrRead))->setText(_("Read only"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TPrmTempl::AttrFull))->setText(_("Full access"));
    }	    
    else if( a_path == "/io/accs_mods" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(IO::Default))->setText(_("Constant"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TPrmTempl::CfgPublConst))->setText(_("Public constant"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TPrmTempl::CfgLink))->setText(_("Link"));
    }
    else if( a_path == "/tmpl/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	load();
    else if( a_path == "/tmpl/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	save();
}

//*************************************************
//* TPrmTmplLib                                   *
//*************************************************
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
        //- Delete libraries record -
        SYS->db().at().dataDel(work_lib_db+"."+owner().tmplLibTable(),owner().nodePath()+"tmplib",*this);
			
        //- Delete temlate librarie's DBs -
        SYS->db().at().open(fullDB());
        SYS->db().at().close(fullDB(),true);
						
        SYS->db().at().open(fullDB()+"_io");
        SYS->db().at().close(fullDB()+"_io",true);
    }
}
								    
string TPrmTmplLib::name()
{
    return (m_name.size())?m_name:m_id;
}
									
void TPrmTmplLib::load( )
{
    SYS->db().at().dataGet(work_lib_db+"."+owner().tmplLibTable(),owner().nodePath()+"tmplib",*this);

    //- Load templates -
    TConfig c_el(&owner().tplE());
    c_el.cfgViewAll(false);
    int fld_cnt = 0;
    while( SYS->db().at().dataSeek(fullDB(),owner().nodePath()+tbl(), fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");

        if( !present(f_id) )    add(f_id.c_str());
        at(f_id).at().load();
    }
}

void TPrmTmplLib::save( )
{
    SYS->db().at().dataSet(work_lib_db+"."+owner().tmplLibTable(),owner().nodePath()+"tmplib",*this);
    
    //- Save functions -
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
        at(lst[i_f]).at().setStart(val);
						
    run_st = val;
}

void TPrmTmplLib::add( const char *id, const char *name )
{
    chldAdd(m_ptmpl,new TPrmTempl(id,name));
}
	
void TPrmTmplLib::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter templates library: ")+id());
	if(ctrMkNode("branches",opt,-1,"/br","",0444))
	    ctrMkNode("grp",opt,-1,"/br/tmpl_",_("Opened DB"),0444,"root","root",1,"list","/tmpl/tmpl");
        if(ctrMkNode("area",opt,-1,"/lib",_("Library")))
	{
    	    if(ctrMkNode("area",opt,-1,"/lib/st",_("State")))
	    {
    		ctrMkNode("fld",opt,-1,"/lib/st/st",_("Accessing"),0664,"root","root",1,"tp","bool");
    		ctrMkNode("fld",opt,-1,"/lib/st/db",_("Library BD (module.bd.table)"),0660,"root","root",1,"tp","str");
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
        if(ctrMkNode("area",opt,-1,"/tmpl",_("Parameter templates")))
	    ctrMkNode("list",opt,-1,"/tmpl/tmpl",_("Templates"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","tmpl_");
        return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/lib/st/st" )
    {
    	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	start(atoi(opt->text().c_str()));
    }
    else if( a_path == "/lib/st/db" )
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
    else if( a_path == "/tmpl/tmpl" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> lst;
    	    list(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
        	opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
        }
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	add(opt->attr("id").c_str(),opt->text().c_str());
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	del(opt->attr("id").c_str(),true);
    }
    else if( a_path == "/lib/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	load();
    else if( a_path == "/lib/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	save();
}

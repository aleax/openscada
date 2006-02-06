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
 
//#include "xml.h"
#include "tsys.h"
//#include "tparamcontr.h"
//#include "tparam.h"
#include "tprmtmpl.h"

TPrmTempl::TPrmTempl( const string &iid, TElem *cf_el ) : 
    TConfig(cf_el), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_descr(cfg("DESCR").getSd()), m_func(cfg("FUNC").getSd())
{
    m_id = m_name = iid;
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
            SYS->db().at().dataDel(SYS->param().at().tmplB(),SYS->param().at().nodePath()+"Tmpl/",*this);
	    //Delete template's IO
	    TBDS::SName io_bd = SYS->param().at().tmplB();
            io_bd.tbl += "_io";
	    TConfig cfg(&owner().tplIOE());
	    cfg.cfg("TMPL_ID").setS(id());
	    cfg.cfg("ID").setS("");
	    SYS->db().at().dataDel(io_bd,SYS->param().at().nodePath()+"TmplIO/",cfg);
	}
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
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
    if( dynamic_cast<TFunction *>(&SYS->nodeAt(m_func,0,'.').at()) )
	return SYS->nodeAt(m_func,0,'.');
    throw TError(nodePath().c_str(),"Function error.");
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
	TConfig cfg(&owner().tplIOE());
	cfg.cfg("TMPL_ID").setS(id());
	TBDS::SName io_bd = SYS->param().at().tmplB();
        io_bd.tbl += "_io";		
	
	for( int i_io = 0; i_io < attrSize(); i_io++ )
	    try
	    { 
		cfg.cfg("ID").setS(attr(i_io).id);
		SYS->db().at().dataGet(io_bd,SYS->param().at().nodePath()+"TmplIO/",cfg);
		attr(i_io).attr = (TPrmTempl::AttrMode)cfg.cfg("ATTR_MODE").getI();
		attr(i_io).accs = (TPrmTempl::AccMode)cfg.cfg("ACCS_MODE").getI();
		attr(i_io).val  = cfg.cfg("VALUE").getS();
	    }
	    catch(TError err){  }
    }
}

void TPrmTempl::attrSave()
{
    //Save template configs to DB
    TConfig cfg(&owner().tplIOE());
    cfg.cfg("TMPL_ID").setS(id());
    TBDS::SName io_bd = SYS->param().at().tmplB();
    io_bd.tbl += "_io";
    
    for( int i_io = 0; i_io < attrSize(); i_io++ )
    {
	cfg.cfg("ID").setS(attr(i_io).id);
	cfg.cfg("ATTR_MODE").setI(attr(i_io).attr);
	cfg.cfg("ACCS_MODE").setI(attr(i_io).accs);
	cfg.cfg("VALUE").setS(attr(i_io).val);
	SYS->db().at().dataSet(io_bd,SYS->param().at().nodePath()+"TmplIO/",cfg);
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
    SYS->db().at().dataGet(SYS->param().at().tmplB(),SYS->param().at().nodePath()+"Tmpl/",*this);
    attrUp();
}
    
void TPrmTempl::save( )
{
    SYS->db().at().dataSet(SYS->param().at().tmplB(),SYS->param().at().nodePath()+"Tmpl/",*this);
    attrSave();    
}	

void TPrmTempl::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {
        ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Parameter template: ")+name());
	ctrMkNode("area",opt,a_path.c_str(),"/tpl",Mess->I18N("Template"));
	ctrMkNode("area",opt,a_path.c_str(),"/tpl/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/tpl/st/en",Mess->I18N("Enable"),0664,0,0,"bool");		
	ctrMkNode("area",opt,a_path.c_str(),"/tpl/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,a_path.c_str(),"/tpl/cfg/id",Mess->I18N("Id"),0444,0,0,"str");
        ctrMkNode("fld",opt,a_path.c_str(),"/tpl/cfg/name",Mess->I18N("Name"),0664,0,0,"str");
        ctrMkNode("fld",opt,a_path.c_str(),"/tpl/cfg/descr",Mess->I18N("Description"),0664,0,0,"str")->
            attr_("cols","60")->attr_("rows","4");
	ctrMkNode("fld",opt,a_path.c_str(),"/tpl/cfg/fncp",Mess->I18N("Function"),0664,0,0,"str")->
	            attr_("dest","sel_ed")->attr("select","/tpl/cfg/fncp_list");
	ctrMkNode("comm",opt,a_path.c_str(),"/tpl/cfg/f_lnk",Mess->I18N("Go to function"),0550,0,0,"lnk");
	ctrMkNode("comm",opt,a_path.c_str(),"/tpl/cfg/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/tpl/cfg/save",Mess->I18N("Save"),0550);
	if( enable() )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/cfg",Mess->I18N("Configuration"));
	    ctrMkNode("table",opt,a_path.c_str(),"/cfg/io",Mess->I18N("IO"),0664,0,0)->attr_("rows","15");
	    ctrMkNode("list",opt,a_path.c_str(),"/cfg/io/0",Mess->I18N("Id"),0444,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/cfg/io/1",Mess->I18N("Name"),0444,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/cfg/io/2",Mess->I18N("Attribute"),0664,0,0,"dec")->
		attr_("idm","1")->attr_("dest","select")->attr_("select","/cfg/attr_mods");
	    ctrMkNode("list",opt,a_path.c_str(),"/cfg/io/3",Mess->I18N("Access"),0664,0,0,"dec")->
		attr_("idm","1")->attr_("dest","select")->attr_("select","/cfg/accs_mods");
	    ctrMkNode("list",opt,a_path.c_str(),"/cfg/io/4",Mess->I18N("Value"),0664,0,0,"str");
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/tpl/st/en" )		ctrSetB( opt, enable() );
	else if( a_path == "/tpl/cfg/id" )     	ctrSetS( opt, id() );
        else if( a_path == "/tpl/cfg/name" )   	ctrSetS( opt, name() );
        else if( a_path == "/tpl/cfg/descr" )  	ctrSetS( opt, descr() );
	else if( a_path == "/tpl/cfg/fncp" )	ctrSetS( opt, m_func );
	else if( a_path == "/tpl/cfg/fncp_list" )
	{
            opt->childClean();
            int c_lv = 0;
            string c_path = "";
            while(TSYS::strSepParse(m_func,c_lv,'.').size())
            {
                ctrSetS( opt, c_path );
                if( c_lv ) c_path+=".";
                c_path = c_path+TSYS::strSepParse(m_func,c_lv,'.');
        	c_lv++;
            }
            ctrSetS( opt, c_path );
            if( c_lv != 0 ) c_path += ".";
            SYS->nodeAt(c_path,0,'.').at().nodeList(list);
            for( unsigned i_a=0; i_a < list.size(); i_a++ )
                ctrSetS( opt, c_path+list[i_a]);
	}	
	else if( a_path == "/tpl/cfg/f_lnk" )
	{
	    int c_lv = 0;
	    string path = "/";
	    while(TSYS::strSepParse(m_func,c_lv,'.').size())
	        path+=TSYS::strSepParse(m_func,c_lv++,'.')+"/";
	    opt->text(path);
	}
	else if( enable() )
	{
	    if( a_path == "/cfg/io" )
    	    {
	        XMLNode *n_id   = ctrId(opt,"0");
        	XMLNode *n_nm   = ctrId(opt,"1");
        	XMLNode *n_attr = ctrId(opt,"2");
        	XMLNode *n_accs = ctrId(opt,"3");
        	XMLNode *n_val  = ctrId(opt,"4");	
	    
		for( int id = 0; id < attrSize(); id++ )
        	{
            	    ctrSetS(n_id,attr(id).id);
            	    ctrSetS(n_nm,func().at().io(func().at().ioId(attr(id).id))->name());
		    ctrSetI(n_attr,attr(id).attr);
		    ctrSetI(n_accs,attr(id).accs);
		    ctrSetS(n_val,attr(id).val);
		}
	    } 
	    else if( a_path == "/cfg/attr_mods" )
            {
		opt->childClean();
		ctrSetS( opt,Mess->I18N("No attribute"),TSYS::int2str(TPrmTempl::NoAttr).c_str());
		ctrSetS( opt,Mess->I18N("Read only"),TSYS::int2str(TPrmTempl::ReadOnly).c_str());
		ctrSetS( opt,Mess->I18N("Full access"),TSYS::int2str(TPrmTempl::FullAccess).c_str());
	    }	    
	    else if( a_path == "/cfg/accs_mods" )
	    {	
		opt->childClean();
		ctrSetS( opt,Mess->I18N("Constant"),TSYS::int2str(TPrmTempl::Const).c_str());
		ctrSetS( opt,Mess->I18N("Public constant"),TSYS::int2str(TPrmTempl::PublConst).c_str());
		ctrSetS( opt,Mess->I18N("Link"),TSYS::int2str(TPrmTempl::Link).c_str());
	    }	
	    else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());								    
	}
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/tpl/st/en" )            enable(ctrGetB(opt));
	else if( a_path == "/tpl/cfg/name" )   	name(ctrGetS(opt));
        else if( a_path == "/tpl/cfg/descr" )   descr(ctrGetS(opt));
	else if( a_path == "/tpl/cfg/fncp" )    
	{ 
	    m_func = ctrGetS( opt ); 
	    try{ attrUp(); } catch(...){ }
	}
	else if( a_path == "/tpl/cfg/load" )    load();
        else if( a_path == "/tpl/cfg/save" )    save();
	else if( a_path == "/cfg/io" )
        {
	    int row = atoi(opt->attr("row").c_str());
	    switch(atoi(opt->attr("col").c_str()))
	    {
		case 2:	attr(row).attr = (TPrmTempl::AttrMode)ctrGetI(opt);	break;
		case 3:	attr(row).accs = (TPrmTempl::AccMode)ctrGetI(opt);	break;
		case 4: attr(row).val  = ctrGetS(opt);	break;
	    }
	}
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }
}

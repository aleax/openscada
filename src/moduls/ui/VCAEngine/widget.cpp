
//OpenSCADA system module UI.VCAEngine file: widget.cpp
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

#include <resalloc.h>
#include <tsys.h>

#include "vcaengine.h"
#include "widget.h"

using namespace VCA;

//************************************************
//* Widget                                       *
//************************************************
Widget::Widget( const string &id, const string &isrcwdg ) :
        m_enable(false), m_lnk(false), m_id(id), m_parent_nm(isrcwdg)
{
    attr_res = ResAlloc::resCreate();

    inclWdg = grpAdd("wdg_");

    attr_cfg = new TElem;
    attr_cfg->valAtt(this);
    
    //- Add main attributes -
    attrAdd( new TFld("id",_("Id"),TFld::String,TFld::NoWrite,"","") );
    attrAdd( new TFld("name",_("Name"),TFld::String,TFld::NoFlag,"","") );
    attrAdd( new TFld("dscr",_("Description"),TFld::String,TFld::FullText,"","") );
    attrAdd( new TFld("en",_("Enabled"),TFld::Boolean,TFld::NoFlag,"","1") );
    attrAdd( new TFld("active",_("Active"),TFld::Boolean,Attr::Prevent,"","0") );
    attrAdd( new TFld("geomX",_("Geometry:x"),TFld::Integer,TFld::NoFlag,"","0","0;10000") );
    attrAdd( new TFld("geomY",_("Geometry:y"),TFld::Integer,TFld::NoFlag,"","0","0;10000") );
    attrAdd( new TFld("geomW",_("Geometry:width"),TFld::Integer,TFld::NoFlag,"","100","0;10000") );
    attrAdd( new TFld("geomH",_("Geometry:height"),TFld::Integer,TFld::NoFlag,"","100","0;10000") );
    attrAdd( new TFld("geomZ",_("Geometry:z"),TFld::Integer,TFld::NoFlag,"","0","-1000000;1000000") );
    attrAdd( new TFld("geomMargin",_("Geometry:margin"),TFld::Integer,TFld::NoFlag,"","5","0;1000") );
}

Widget::~Widget()
{
    TAttrMap::iterator p;
    while( (p=attrs.begin())!=attrs.end() )
    {
        delete p->second;
        attrs.erase(p);
    }
    attr_cfg->valDet(this);
    delete attr_cfg;

    ResAlloc::resDelete(attr_res);
}

void Widget::postEnable(int flag)
{
    if( flag&TCntrNode::NodeRestore )	setEnable(true);
}

void Widget::preDisable(int flag)
{
    if( enable() )  setEnable(false);
}

Widget &Widget::operator=(Widget &wdg)
{
    vector<string> als;
    attrList(als);
    for(int i_a = 0; i_a < als.size(); i_a++)
	if( als[i_a]!="id" && wdg.attrPresent(als[i_a]) && !attrAt(als[i_a]).at().modifVal() )
	{
	    attrAt(als[i_a]).at().setS(wdg.attrAt(als[i_a]).at().getS());
	    attrAt(als[i_a]).at().setFlgSelf(wdg.attrAt(als[i_a]).at().flgSelf());
	    //- No inherit calc flag for links -
	    if( isLink() ) 
		attrAt(als[i_a]).at().setFlgSelf((Attr::SelfAttrFlgs)(attrAt(als[i_a]).at().flgSelf()&(~Attr::ProcAttr)));
	    attrAt(als[i_a]).at().setCfgTempl(wdg.attrAt(als[i_a]).at().cfgTempl());
	    attrAt(als[i_a]).at().setCfgVal(wdg.attrAt(als[i_a]).at().cfgVal());
	    attrAt(als[i_a]).at().setModifVal(0);
	    attrAt(als[i_a]).at().setModifCfg(0);
	}

    return *this;
}

string Widget::rootId( )
{
    if( !parent().freeStat() )  return parent().at().rootId();
    return "";
}

bool Widget::enable( )
{ 
    return m_enable; 
}

void Widget::setEnable( bool val )
{
    if( enable() == val ) return;

    if( val ) 
    {
	if( parentNm().size() && parentNm() != "root" )
	{
	    if( parentNm() == ".." )
		m_parent=AutoHD<TCntrNode>(nodePrev());
	    else m_parent=mod->nodeAt(parentNm());
	    //- Check for enable parent widget and enable if not -
	    if( !parent().at().enable() )	parent().at().setEnable(true);
	    //- Inherit attribute -
    	    parent().at().inherit(this);
	}
	m_enable = true;
        //- Load self values from DB -
        loadIO();
    }
    if(!val)
    {
        //- Free inherit conteiner's widgets
        //????
        //- Free inherit attributes -
        vector<string>  ls;
        attrList(ls);
        for(int i_l = 0; i_l < ls.size(); i_l++)
            if(attrAt(ls[i_l]).at().flgGlob()&Attr::IsInher)
                attrDel( ls[i_l] );
        //Disconnect parent widget
        m_parent.free();
    }

    //- Enable/disable process widgets from container -
    vector<string>	ls;
    wdgList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
        wdgAt(ls[i_l]).at().setEnable(val);

    m_enable = val;
}

AutoHD<Widget> Widget::parent( )
{ 
    return m_parent; 
}

void Widget::inherit( Widget *wdg )
{
    //- Inherit attributes to Widget 'wdg' -
    //-- Create no present attributes --
    vector<string>  ls;
    attrList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++)
        if( !wdg->attrPresent(ls[i_l]) )
        {
            TFld *fel = new TFld(attrAt(ls[i_l]).at().fld());
            //Clear user attribute flag and set inherit flag
            fel->setFlg(fel->flg()&(~(Attr::IsUser))|Attr::IsInher);
            wdg->attrAdd(fel);
        }
    //-- Copy attribute's values --    
    (*wdg) = (*this);
    
    //- Create no present include widgets for no link and container widgets -
    if( !wdg->isLink() && isContainer() )
    {	
	wdgList(ls);
	for( int i_w = 0; i_w < ls.size(); i_w++ )
	    if( !wdg->wdgPresent(ls[i_w]) )
		wdg->wdgAdd(ls[i_w],"",wdgAt(ls[i_w]).at().path());
    }
}

void Widget::attrList( vector<string> &list )
{
    list.clear();
    ResAlloc res(attr_res,false);
    attr_cfg->fldList(list);
}

bool Widget::attrPresent( const string &n_val )
{
    ResAlloc res(attr_res,false);
    TAttrMap::iterator p=attrs.find(n_val);
    if(p==attrs.end())	return false;
    return true;
}

void Widget::attrAdd( TFld *attr )
{
    if(attrPresent(attr->name()))
    {
	delete attr;
        throw TError(nodePath().c_str(),_("Attribut %s already present."),attr->name().c_str());
    }
    attr_cfg->fldAdd(attr);
}

void Widget::attrDel( const string &attr )
{
    if(attrPresent(attr))
        attr_cfg->fldDel(attr_cfg->fldId(attr));
}

AutoHD<Attr> Widget::attrAt( const string &n_val )
{
    ResAlloc res(attr_res,false);
    TAttrMap::iterator p=attrs.find(n_val);
    if(p==attrs.end())	throw TError(nodePath().c_str(),_("Attribute %s no present!"),n_val.c_str());
    return  AutoHD<Attr>(p->second);
}

bool Widget::attrChange( Attr &cfg )
{
    if( cfg.owner() == this ) ResAlloc::resReleaseR(attr_res);	//?!?! Dangerous
    if( !parent().freeStat() )	return parent().at().attrChange(cfg);
    if( parentNm( ) != "root" )	return false;

    if( cfg.id() == "active" )
    {
	if(cfg.getB())
	{
	    if( !cfg.owner()->attrPresent("evProc") )
		cfg.owner()->attrAdd( new TFld("evProc",_("Events process"),TFld::String,TFld::FullText,"200") );
	    if( !cfg.owner()->attrPresent("event") )
		cfg.owner()->attrAdd( new TFld("event",_("Events"),TFld::String,TFld::NoFlag,"200") );
	}	
	else
	{
	    if( cfg.owner()->attrPresent("evProc") )	cfg.owner()->attrDel("evProc");
	    if( cfg.owner()->attrPresent("event") )	cfg.owner()->attrDel("event");
	}
    }
    
    return true;
}

void Widget::wdgList( vector<string> &list )
{
    chldList(inclWdg,list);
}

bool Widget::wdgPresent( const string &wdg )
{
    return chldPresent(inclWdg,wdg);
}

void Widget::wdgAdd( const string &wid, const string &name, const string &path )
{
    if( !isContainer() )  throw TError(nodePath().c_str(),_("No container widget!"));
    if( wdgPresent(wid) ) return;

    chldAdd(inclWdg,new Widget(wid,path));
    wdgAt(wid).at().setName(name);
}

void Widget::wdgDel( const string &wid, bool full )
{
    chldDel(inclWdg,wid,-1,full);
}

AutoHD<Widget> Widget::wdgAt( const string &wdg )
{
    return chldAt(inclWdg,wdg);
}

void Widget::addFld( TElem *el, unsigned id )
{
    if( el == attr_cfg )
    {
	ResAlloc res(attr_res,true);
	attrs.insert( std::pair<string,Attr*>(el->fldAt(id).name(),new Attr(el->fldAt(id),this)) );
    }
    //else TConfig::addFld(el,id);
}

void Widget::delFld( TElem *el, unsigned id )
{
    if( el == attr_cfg )
    {
	ResAlloc res(attr_res,true);
	TAttrMap::iterator p=attrs.find(el->fldAt(id).name());
	if(p==attrs.end())  return;
        delete p->second;
	attrs.erase(p);
    }
    //else TConfig::delFld(el,id);
}

void Widget::detElem( TElem *el )
{

}

bool Widget::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget: ")+id());
        if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")))
	{
	    if(ctrMkNode("area",opt,-1,"/wdg/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enable"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/wdg/st/user",_("User and group"),RWRWR_,"root","UI",3,"tp","str","dest","select","select","/wdg/u_lst");
		ctrMkNode("fld",opt,-1,"/wdg/st/grp","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/wdg/g_lst");
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/wdg/w_lst");
		if(!parent().freeStat())
		    ctrMkNode("comm",opt,-1,"/wdg/st/goparent",_("Go to parent"),permit(),user().c_str(),grp().c_str(),1,"tp","lnk");
	    }
	    if(ctrMkNode("area",opt,-1,"/wdg/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/cfg/id",_("Id"),R_R_R_,user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/descr",_("Description"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","50","rows","3");
                ctrMkNode("img",opt,-1,"/wdg/cfg/ico",_("Icon"),permit(),user().c_str(),grp().c_str(),2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/u_a",_("Access: user-grp-other"),RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/g_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/o_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		ctrMkNode("comm",opt,-1,"/wdg/cfg/load",_("Load"),permit(),user().c_str(),grp().c_str());
		ctrMkNode("comm",opt,-1,"/wdg/cfg/save",_("Save"),permit(),user().c_str(),grp().c_str());
	    }
	}
        if(isContainer() && (!isLink()) && ctrMkNode("area",opt,-1,"/inclwdg",_("Include widgets")))
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","wdg_");
	if(isContainer() && (!isLink()) && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
            ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),R_R_R_,"root","root",1,"list","/inclwdg/wdg");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",0444);	    
	return true;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/wdg/st/en" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(TSYS::int2str(enable()));
        if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),grp().c_str(),SEQ_WR) ) setEnable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/wdg/st/parent" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(parentNm());
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setParentNm(opt->text());
    }
    else if( a_path == "/wdg/st/goparent" && 
	    ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) && !parent().freeStat() )
        opt->setText(parent().at().nodePath(0,true));
    else if( a_path == "/wdg/st/user" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->setText(user());
	if( ctrChkNode(opt,"set",RWRWR_,"root","root",SEQ_WR) )	setUser(opt->text());
    }
    else if( a_path == "/wdg/st/grp" )
    {
        if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )	opt->setText(grp());
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) ) 	setGrp(opt->text());
    }
    else if( a_path == "/wdg/cfg/u_a" || a_path == "/wdg/cfg/g_a" || a_path == "/wdg/cfg/o_a" )
    {
        if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )
        {
            if( a_path == "/wdg/cfg/u_a" )       opt->setText(TSYS::int2str((permit()>>6)&0x7));
            if( a_path == "/wdg/cfg/g_a" )       opt->setText(TSYS::int2str((permit()>>3)&0x7));
            if( a_path == "/wdg/cfg/o_a" )       opt->setText(TSYS::int2str(permit()&0x7));
        }
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )
        {
            if( a_path == "/wdg/cfg/u_a" ) setPermit( (permit()&(~(0x07<<6)))|(atoi(opt->text().c_str())<<6) );
            if( a_path == "/wdg/cfg/g_a" ) setPermit( (permit()&(~(0x07<<3)))|(atoi(opt->text().c_str())<<3) );
            if( a_path == "/wdg/cfg/o_a" ) setPermit( (permit()&(~0x07))|atoi(opt->text().c_str()) );
        }
    }
    else if( a_path == "/wdg/cfg/ico" || a_path == "/ico" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(ico());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setIco(opt->text());
    }
    else if( a_path == "/wdg/cfg/id" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str()) )	
	opt->setText(id());
    else if( a_path == "/wdg/cfg/name" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(name());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setName(opt->text());
    }
    else if( a_path == "/wdg/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(descr());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setDescr(opt->text());
    }
    else if( a_path == "/wdg/cfg/load" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )	load();
    else if( a_path == "/wdg/cfg/save" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )	save();
    else if( a_path == "/wdg/u_lst" && ctrChkNode(opt) )
    {
        vector<string> ls;
        SYS->security().at().usrList(ls);
        for(int i_l = 0; i_l < ls.size(); i_l++)
        opt->childAdd("el")->setText(ls[i_l]);
    }
    else if( a_path == "/wdg/g_lst" && ctrChkNode(opt) )
    {
        vector<string> ls;
        SYS->security().at().usrGrpList(user(), ls );
        for(int i_l = 0; i_l < ls.size(); i_l++)
        opt->childAdd("el")->setText(ls[i_l]);
    }
    else if( a_path == "/wdg/a_lst" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->setAttr("id","0")->setText(_("No access"));
        opt->childAdd("el")->setAttr("id","4")->setText(_("Use(open)"));
        opt->childAdd("el")->setAttr("id","2")->setText(_("Modify"));
        opt->childAdd("el")->setAttr("id","6")->setText(_("Full"));
    }
    else if( a_path == "/wdg/w_lst" && ctrChkNode(opt) )
    {
        int c_lv = 0;
        string c_path = "";
        string lnk = parentNm();

	opt->childAdd("el")->setText(c_path);
	while(TSYS::pathLev(lnk,c_lv).size())
	{
	    c_path = c_path+"/"+TSYS::pathLev(lnk,c_lv);
	    opt->childAdd("el")->setText(c_path);
	    c_lv++;
	}
        //if(c_lv) c_path+=".";
        vector<string>  ls;
        switch(c_lv)
        {
            case 0: mod->nodeList(ls);   break;
            case 1:
	        //if( mod->nodeAt(TSYS::pathLev(lnk,0)) )
                mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeList(ls);
                break;
        }
        for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->setText(c_path+"/"+ls[i_l]);
    }    
    else if( a_path == "/inclwdg/wdg" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
            vector<string>  lst;
    	    wdgList(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->setAttr("id",lst[i_f])->setText(wdgAt(lst[i_f]).at().name());
        }
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
            wdgAdd(opt->attr("id").c_str(),opt->text(),"");
	if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) wdgDel(opt->attr("id").c_str(),true);
    }        
    else return false;
    
    return true;
}

bool Widget::cntrCmdAttributes( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    { 
     	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes")))
	{
	    //Properties form create
	    vector<string> list_a;
	    attrList(list_a);
	    for( unsigned i_el = 0; i_el < list_a.size(); i_el++ )
		attrAt(list_a[i_el]).at().fld().cntrCmdMake(opt,"/attr",-1,user().c_str(),grp().c_str(),permit());
	}
	return true;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path.substr(0,5) == "/attr" && TSYS::pathLev(a_path,1).size() > 4 && TSYS::pathLev(a_path,1).substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
    {
        AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1).substr(4));
        for( int i_a=0; i_a < attr.at().fld().selNm().size(); i_a++ )
	    opt->childAdd("el")->setText(attr.at().fld().selNm()[i_a]);
    }
    else if( a_path.substr(0,6) == "/attr/" )
    {
	AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1));
	if( ctrChkNode(opt,"get",(attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
	    if( attr.at().fld().flg()&TFld::Selected )	opt->setText(attr.at().getSEL());
            else                                    	opt->setText(attr.at().getS());
	}
	if( ctrChkNode(opt,"set",(attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    if( attr.at().fld().flg()&TFld::Selected )	attr.at().setSEL(opt->text());
            else                                    	attr.at().setS(opt->text());
	}
    }
    else return false;
    
    return true;
}

bool Widget::cntrCmdLinks( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
 	if(ctrMkNode("area",opt,-1,"/links",_("Links")))
	{
	    ctrMkNode("fld",opt,-1,"/links/showAttr",_("Atributes show"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links")))
	    {
		vector<string> incllist, alist, list;
		wdgList(incllist);
		for( int i_w = -1; i_w < (int)incllist.size(); i_w++ )
		{
		    AutoHD<Widget> wdg;
		    if( i_w < 0 ) wdg = AutoHD<Widget>(this);
		    else wdg = wdgAt(incllist[i_w]);
		    wdg.at().attrList(alist);
		    for( int i_a = 0; i_a < alist.size(); i_a++ )
            	    {
			string grpprm;
		        string idprm = alist[i_a];
			string nprm  = wdg.at().attrAt(alist[i_a]).at().name();
			if( i_w >= 0 )
			{
			    idprm.insert(0,incllist[i_w]+".");
			    nprm.insert(0,wdg.at().name()+".");
			}
			
			if( !(wdg.at().attrAt(alist[i_a]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst)) ) continue;
			//Get attributes
			bool shwAttr = atoi(opt->attr("showAttr").c_str()) || 
				       atoi(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")).c_str());
			bool shwTmpl = wdg.at().attrAt(alist[i_a]).at().cfgTempl().size();
			if( shwTmpl )	grpprm = TSYS::strSepParse(wdg.at().attrAt(alist[i_a]).at().cfgTempl(),0,'|');
			
                	//Check select param
                	if( shwTmpl && !shwAttr )
                	{
                    	    nprm = grpprm;
			    if( i_w >= 0 ) nprm.insert(0,wdg.at().name()+".");
			    
	            	    //Check already to present parameters
	            	    bool f_ok = false;
	            	    for( int i_l = 0; i_l < list.size(); i_l++ )
				if( list[i_l] == nprm ) { f_ok = true; break; }
                    	    if(!f_ok)
                    	    {
                    		ctrMkNode("fld",opt,-1,(string("/links/lnk/pr_")+idprm).c_str(),
				    nprm,permit(),user().c_str(),grp().c_str(),
                            	    3,"tp","str","dest","sel_ed","select",(string("/links/lnk/pl_")+idprm).c_str());
                    		list.push_back(nprm);
                    	    }
			}
			else 
			{
			    if( wdg.at().attrAt(alist[i_a]).at().flgSelf()&Attr::CfgConst )
 			    	ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),
				    nprm,permit(),user().c_str(),grp().c_str(),2,"tp","str","elGrp",grpprm.c_str());
			    else
				ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),
				    nprm,permit(),user().c_str(),grp().c_str(),
				    4,"tp","str","dest","sel_ed",
				    "select",(string("/links/lnk/ls_")+idprm).c_str(),"elGrp",grpprm.c_str());
			}
		    }		
		}
	    }
	}
	return true;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/links/showAttr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    TBDS::genDBSet(mod->nodePath()+"showAttr",opt->text(),opt->attr("user"));
    }
    else if( a_path.substr(0,14) == "/links/lnk/pr_" )
    {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if( nattr.size() )	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;	
	
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    string cfg_val = srcwdg.at().attrAt(nattr).at().cfgVal();
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,'.');
	    
	    int c_lvl = 0;
	    while(TSYS::strSepParse(cfg_val,c_lvl,'.').size())	c_lvl++;
	    if( (obj_tp == "P" && c_lvl==5) || (obj_tp == "W" && c_lvl==3) )
		opt->setText(cfg_val.substr(0,cfg_val.rfind(".")));
	    else opt->setText(cfg_val);
	}
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    vector<string> a_ls;
	    string no_set;
	    bool noonly_no_set = true;
	    string cfg_val = opt->text();	    
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,'.');
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    int c_lvl = 0;
	    while(TSYS::strSepParse(cfg_val,c_lvl,'.').size())  c_lvl++;

	    srcwdg.at().attrList(a_ls);

	    AutoHD<TValue> prm;
	    if( obj_tp == "P" && c_lvl==4 )
		prm = SYS->daq().at().at(TSYS::strSepParse(cfg_val,1,'.')).at().
				      at(TSYS::strSepParse(cfg_val,2,'.')).at().
				      at(TSYS::strSepParse(cfg_val,3,'.'));
	    AutoHD<Widget> dstwdg;
	    if( obj_tp == "W" && c_lvl==2 )
	    {
		string nwdg = TSYS::strSepParse(cfg_val,1,'.');
		if( nwdg=="self" ) dstwdg = this;
		else	dstwdg = wdgAt(nwdg);
	    }

	    for( int i_a = 0; i_a < a_ls.size(); i_a++ )
		if( p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') )
		{
		    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val);
		    string p_attr = TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),1,'|');
		    if(!prm.freeStat() || !dstwdg.freeStat())
		    {
			if( (!prm.freeStat() && prm.at().vlPresent(p_attr)) ||
			    (!dstwdg.freeStat() && dstwdg.at().attrPresent(p_attr)) )
		    	{
			    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val+"."+p_attr);
			    noonly_no_set = false;
			}
			else no_set+=p_attr+",";
		    }
		}
	    if(!prm.freeStat() || !dstwdg.freeStat())
	    {
		if( noonly_no_set )     throw TError(nodePath().c_str(),_("Destination have not only atributes!"));
                else if( no_set.size() )throw TError(nodePath().c_str(),_("Destination have not atributes: %s !"),no_set.c_str());
	    }
	}
    }
    else if( (a_path.substr(0,14) == "/links/lnk/pl_" || a_path.substr(0,14) == "/links/lnk/ls_")
	    && ctrChkNode(opt) )
    {
	AutoHD<Widget> srcwdg(this);
    	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if( nattr.size() ) srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	
	if( !(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut)) )
	    throw TError(nodePath().c_str(),_("Variable is not link"));
	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();
	
	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
        int c_lv;
	string c_path = "";
	opt->childAdd("el")->setText(c_path);
	string obj_tp = TSYS::strSepParse(m_prm,0,'.');
	for( c_lv = 0; TSYS::strSepParse(m_prm,c_lv,'.').size(); c_lv++ )
	{
	    if( is_pl && ((obj_tp=="P" && c_lv>3) || (obj_tp=="W" && c_lv)) )	break;
	    if( c_lv ) c_path += ".";
	    c_path = c_path+TSYS::strSepParse(m_prm,c_lv,'.');
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	string prm1 = TSYS::strSepParse(m_prm,1,'.');
	string prm2 = TSYS::strSepParse(m_prm,2,'.');
	string prm3 = TSYS::strSepParse(m_prm,3,'.');
	vector<string>	ls;
	switch(c_lv)
	{
	    case 0: ls.push_back("V"); ls.push_back("P"); ls.push_back("W"); break;
	    case 1:
		if( obj_tp == "V" ) ls.push_back("Constant value");
		if( obj_tp == "P" ) SYS->daq().at().modList(ls);
		if( obj_tp == "W" )
		{
		    wdgList(ls);
		    ls.push_back("self");
		}
		break;
	    case 2:
		if( obj_tp == "P" && SYS->daq().at().modPresent(prm1) )
		    SYS->daq().at().at(prm1).at().list(ls);
		if( !is_pl && obj_tp == "W" )
		{
		    AutoHD<Widget> wdg;
		    if( prm1 == "self")	wdg = this;
                    else wdg = wdgAt(prm1);
		    if( !wdg.freeStat() ) wdg.at().attrList(ls);
		}
		break;
	    case 3:
		if( obj_tp == "P" && SYS->daq().at().modPresent(prm1)
			          && SYS->daq().at().at(prm1).at().present(prm2) )
		    SYS->daq().at().at(prm1).at().at(prm2).at().list(ls);
		break;
	    case 4:
 		if( !is_pl && obj_tp=="P" && SYS->daq().at().modPresent(prm1)
			                  && SYS->daq().at().at(prm1).at().present(prm2)
			                  && SYS->daq().at().at(prm1).at().at(prm2).at().present(prm3) )
		    SYS->daq().at().at(prm1).at().at(prm2).at().at(prm3).at().vlList(ls);
		break;
	}
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if( a_path.substr(0,14) == "/links/lnk/el_" )
    {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if( nattr.size() )	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    if( srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst )
		opt->setText(srcwdg.at().attrAt(nattr).at().getS());
	    else opt->setText(srcwdg.at().attrAt(nattr).at().cfgVal());
	}
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    if( srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst )
		srcwdg.at().attrAt(nattr).at().setS(opt->text());
	    else srcwdg.at().attrAt(nattr).at().setCfgVal(opt->text());
	}
    }
    else return false;

    return true;
}

bool Widget::cntrCmdProcess( XMLNode *opt )
{
    string wattr;
    
    if( isLink() ) return false;
    //Get page info
    if( opt->name() == "info" )
    { 
 	if(ctrMkNode("area",opt,-1,"/proc",_("Process")))
	{
	    ctrMkNode("fld",opt,-1,"/proc/wdg",_("Widget"),permit(),user().c_str(),grp().c_str(),4,"tp","str","dest","select","select","/proc/w_lst");
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if( wattr == "." || !wdgPresent(wattr) )	wattr = ".";	    
            if(ctrMkNode("table",opt,-1,"/proc/attr",_("Attributes"),permit(),user().c_str(),grp().c_str(),2,"s_com",((wattr==".")?"add,del":""),"key","id"))
            {
        	ctrMkNode("list",opt,-1,"/proc/attr/id",_("Id"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
	        ctrMkNode("list",opt,-1,"/proc/attr/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
        	ctrMkNode("list",opt,-1,"/proc/attr/type",_("Type"),permit(),user().c_str(),grp().c_str(),4,"tp","dec","idm","1","dest","select","select","/proc/tp_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/wa",_("Work area"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/proc",_("Process"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("list",opt,-1,"/proc/attr/cfg",_("Config"),permit(),user().c_str(),grp().c_str(),4,"tp","dec","idm","1","dest","select","select","/proc/lnk_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/cfgtmpl",_("Config template"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/proc/calc",_("Calculate")))
	    {
		ctrMkNode("fld",opt,-1,"/proc/calc/progLng",_("Programm language"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/proc/calc/plangLs");
		ctrMkNode("fld",opt,-1,"/proc/calc/prog",_("Programm"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","90","rows","8");
	    }
	}
	return true;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/proc/wdg" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if( wattr != "." && !wdgPresent(wattr) )	wattr = ".";
	    opt->setText(wattr);
	}
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) 
	    TBDS::genDBSet(mod->nodePath()+"wdgAttr",opt->text(),opt->attr("user"));
    }    
    else if( a_path == "/proc/w_lst" && ctrChkNode(opt) )
    {
        vector<string> lst;
        wdgList(lst);
        opt->childAdd("el")->setAttr("id",".")->setText("<Self>");
        for( unsigned i_f=0; i_f < lst.size(); i_f++ )
            opt->childAdd("el")->setAttr("id",lst[i_f])->setText(wdgAt(lst[i_f]).at().name());
    }    
    else if( a_path == "/proc/attr" )
    {
	wattr = opt->attr("wdg");
	if( wattr.empty() )
    	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	if( wattr != "." && !wdgPresent(wattr) )	wattr = ".";
	
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
    	    XMLNode *n_id      = ctrMkNode("list",opt,-1,"/proc/attr/id","");
	    XMLNode *n_name    = ctrMkNode("list",opt,-1,"/proc/attr/name","");
	    XMLNode *n_type    = ctrMkNode("list",opt,-1,"/proc/attr/type","");
	    XMLNode *n_wa      = ctrMkNode("list",opt,-1,"/proc/attr/wa","");
	    XMLNode *n_proc    = ctrMkNode("list",opt,-1,"/proc/attr/proc","");
	    XMLNode *n_cfg     = ctrMkNode("list",opt,-1,"/proc/attr/cfg","");
	    XMLNode *n_cfgtmpl = ctrMkNode("list",opt,-1,"/proc/attr/cfgtmpl","");

	    AutoHD<Widget> wdg;
	    vector<string> lst;	    
	    if( wattr == "." )	wdg = AutoHD<Widget>(this);
	    else wdg = wdgAt(wattr);

	    wdg.at().attrList(lst);
	    for(int i_el = 0; i_el < lst.size(); i_el++)
	    {
		if( n_id )	n_id->childAdd("el")->setText(lst[i_el]);
		if( n_name )    n_name->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().name());
		if( n_type )	n_type->childAdd("el")->setText(TSYS::int2str(wdg.at().attrAt(lst[i_el]).at().fld().type()+
				    ((wdg.at().attrAt(lst[i_el]).at().fld().flg()&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
		if( n_wa )	n_wa->childAdd("el")->setText( wdg.at().attrAt(lst[i_el]).at().fld().values()+"|"+
							    wdg.at().attrAt(lst[i_el]).at().fld().selNames());
		if( n_proc )	n_proc->childAdd("el")->setText(TSYS::int2str(wdg.at().attrAt(lst[i_el]).at().flgSelf()&Attr::ProcAttr));
		if( n_cfg )	n_cfg->childAdd("el")->setText(TSYS::int2str(wdg.at().attrAt(lst[i_el]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst)));
		if( n_cfgtmpl )	n_cfgtmpl->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().cfgTempl());
	    }
	}
        if( wattr == "." && ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            attrAdd(new TFld("newAttr",_("New attribute"),TFld::String,Attr::IsUser));
            attrAt("newAttr").at().setModifCfg(1);
        }
        if( wattr == "." && ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            if(!attrPresent(opt->attr("key_id")))
                throw TError(nodePath().c_str(),_("Delete a include widget's element is error."));
            if(attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsUser) attrDel(opt->attr("key_id"));
            else throw TError(nodePath().c_str(),_("Delete a not user element is error."));
        }
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            //Request data
            string idattr = opt->attr("key_id");
            string idcol  = opt->attr("col");

	    if( wattr == "." && (idcol == "id" || idcol == "name" || idcol == "type" || idcol == "wa") )
	    {
                string          tnm     = attrAt(idattr).at().name();
                TFld::Type      ttp     = attrAt(idattr).at().fld().type();
                unsigned        tflg    = attrAt(idattr).at().fld().flg();

                if(!attrPresent(idattr) || !(attrAt(idattr).at().fld().flg()&Attr::IsUser) )
            	    throw TError(nodePath().c_str(),_("Change a no user attribute is no permit"));
                if( idcol == "id" )
                {
                    attrAdd( new TFld(opt->text().c_str(),tnm.c_str(),ttp,tflg|Attr::IsUser) );
            	    attrAt(opt->text().c_str()).at().setModifCfg(1);
                    attrDel(idattr);
            	}
            	else if( idcol == "name" )      attrAt(idattr).at().fld().setDescr(opt->text().c_str());
                else if( idcol == "type" )
                {
                    TFld::Type  tp  = (TFld::Type)(atoi(opt->text().c_str())&0x0f);
            	    unsigned    flg = (atoi(opt->text().c_str())>>4)|Attr::IsUser;
	            if(tp!=ttp || (tflg^flg)&TFld::Selected )
	            {
	                attrDel(idattr);
	                attrAdd( new TFld(idattr.c_str(),tnm.c_str(),tp,tflg^((tflg^flg)&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address|Attr::IsUser))) );
	                attrAt(idattr.c_str()).at().setModifCfg(1);
	            }
	            else if( (tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address) )
	                attrAt(idattr).at().fld().setFlg(tflg^((tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address)));
	        }
		else if( idcol == "wa" )
		{
		    attrAt(idattr).at().fld().setValues(TSYS::strSepParse(opt->text(),0,'|'));
		    attrAt(idattr).at().fld().setSelNames(TSYS::strSepParse(opt->text(),1,'|'));
		}
	    }
	    else
	    {
		AutoHD<Attr> attr;
		if( wattr == "." ) attr = attrAt(idattr);
		else attr = wdgAt(wattr).at().attrAt(idattr);

		Attr::SelfAttrFlgs sflg = attr.at().flgSelf();

		if( idcol == "proc" )
		{
		    Attr::SelfAttrFlgs stflg = atoi(opt->text().c_str())?Attr::ProcAttr:(Attr::SelfAttrFlgs)0;
		    attr.at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::ProcAttr)) );
		}
		else if( idcol == "cfg" )
        	{
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)atoi(opt->text().c_str());
		    if( (sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst) )
			attr.at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst))) );
		}
		else if( idcol == "cfgtmpl" )	attr.at().setCfgTempl(opt->text());
	    }
	}
    }
    else if( a_path == "/proc/calc/progLng" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(calcLang());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setCalcLang(opt->text());
    }
    else if( a_path == "/proc/calc/prog" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(calcProg());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setCalcProg(opt->text());
    }
    else if( a_path == "/proc/calc/plangLs" && ctrChkNode(opt) )
    {
	string tplng = calcLang();
        int c_lv = 0;
        string c_path = "";
	opt->childAdd("el")->setText(c_path);
        while(TSYS::strSepParse(tplng,c_lv,'.').size())
        {
            if( c_lv ) c_path+=".";
            c_path = c_path+TSYS::strSepParse(tplng,c_lv,'.');
            opt->childAdd("el")->setText(c_path);
            c_lv++;
        }
        if(c_lv) c_path+=".";
        vector<string>  ls;
        switch(c_lv)
        {
            case 0:     SYS->daq().at().modList(ls);    break;
            case 1:
                if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
                    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(ls);
                break;
        }
        for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if( a_path == "/proc/tp_ls" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Boolean))->setText(_("Boolean"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Integer))->setText(_("Integer"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Real))->setText(_("Real"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String))->setText(_("String"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Integer+(TFld::Selected<<4)))->setText(_("Select integer"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Real+(TFld::Selected<<4)))->setText(_("Select real"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String+(TFld::Selected<<4)))->setText(_("Select string"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String+(Attr::Color<<4)))->setText(_("Color"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String+(Attr::Image<<4)))->setText(_("Image"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String+(Attr::Font<<4)))->setText(_("Font"));
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String+(Attr::Address<<4)))->setText(_("Address"));
    }
    else if( a_path == "/proc/lnk_ls" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->setAttr("id",TSYS::int2str(0))->setText(_("Not"));
 	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgConst))->setText(_("Constant"));        
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgLnkIn))->setText(_("Input link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgLnkOut))->setText(_("Output link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgLnkIn|Attr::CfgLnkOut))->setText(_("Full link"));
    } 
    else return false;

    return true;
}

//************************************************
//* Widget atribute                              *
//************************************************
Attr::Attr( TFld &ifld, Widget *owner ) : m_owner(owner), vl_modif(0), cfg_modif(0), self_flg((SelfAttrFlgs)0)
{
    //Chek for self field for dinamic elements
    if( ifld.flg()&TFld::SelfFld )
    {
        m_fld = new TFld();
        *m_fld = ifld;
    }
    else m_fld = &ifld;

    switch(fld().type())
    {
        case TFld::String:
    	    m_val.s_val    = new string;
            *(m_val.s_val) = fld().def();
            break;
        case TFld::Integer:	m_val.i_val = atoi(m_fld->def().c_str());       break;
        case TFld::Real:        m_val.r_val = atof(m_fld->def().c_str());       break;
        case TFld::Boolean:	m_val.b_val = atoi(m_fld->def().c_str());       break;
    }
}

Attr::~Attr(  )
{
    if( fld().type() == TFld::String )	delete m_val.s_val;
    if( fld().flg()&TFld::SelfFld )	delete m_fld;
}

string Attr::id()
{
    return fld().name();
}

string Attr::name()
{
    return fld().descr();
}

TFld::Type Attr::type()
{
    return fld().type();
}

int Attr::flgGlob()
{
    return fld().flg();
}

void Attr::setFlgSelf( SelfAttrFlgs flg )
{
    if(self_flg == flg)	return;
    self_flg = flg;
    cfg_modif++;
}

string Attr::getSEL( )
{
    if( !(fld().flg()&TFld::Selected) )
	throw TError("Cfg",_("Element type no select!"));
    switch( fld().type() )
    {
        case TFld::String:      return fld().selVl2Nm(*m_val.s_val);
        case TFld::Integer:	return fld().selVl2Nm(m_val.i_val);
        case TFld::Real:        return fld().selVl2Nm(m_val.r_val);
        case TFld::Boolean:	return fld().selVl2Nm(m_val.b_val);
    }
}

string Attr::getS( )
{
    switch(fld().type())
    {
	case TFld::String:      return *m_val.s_val;
        case TFld::Integer:	return TSYS::int2str(m_val.i_val);
        case TFld::Real:        return TSYS::real2str(m_val.r_val);
        case TFld::Boolean:	return TSYS::int2str(m_val.b_val);
    }
}

double Attr::getR( )
{
    switch(fld().type())
    {
        case TFld::String:      return atof(m_val.s_val->c_str());
        case TFld::Integer:	return m_val.i_val;
        case TFld::Real:        return m_val.r_val;
        case TFld::Boolean:	return m_val.b_val;
    }
}

int Attr::getI( )
{
    switch(fld().type())
    {
	case TFld::String:      return atoi(m_val.s_val->c_str());
        case TFld::Integer:	return m_val.i_val;
        case TFld::Real:        return (int)m_val.r_val;
        case TFld::Boolean:	return m_val.b_val;
    }
}

bool Attr::getB( )
{
    switch(fld().type())
    {
        case TFld::String:      return atoi(m_val.s_val->c_str());
        case TFld::Integer:	return m_val.i_val;
        case TFld::Real:        return (int)m_val.r_val;
        case TFld::Boolean:	return m_val.b_val;
    }
}

void Attr::setSEL( const string &val )
{
    if( !(fld().flg()&TFld::Selected) )
        throw TError("Cfg",_("Element type no select!"));
    switch( fld().type() )
    {
	case TFld::String:      setS( fld().selNm2VlS(val) );  break;
        case TFld::Integer:	setI( fld().selNm2VlI(val) );  break;
        case TFld::Real:        setR( fld().selNm2VlR(val) );  break;
        case TFld::Boolean:	setB( fld().selNm2VlB(val) );  break;
    }
}

void Attr::setS( const string &val )
{
    switch( fld().type() )
    {
	case TFld::String:
	    if( *(m_val.s_val) == val )	break;
            if( fld().flg()&Attr::Prevent )
            {
                string t_str = *(m_val.s_val);
        	*(m_val.s_val) = val;
                if( !m_owner->attrChange(*this) )
            	    *(m_val.s_val) = t_str;
            }
    	    else *(m_val.s_val) = val;
	    vl_modif++;
            break;
        case TFld::Integer:	setI( atoi(val.c_str()) );      break;
	case TFld::Real:	setR( atof(val.c_str()) );      break;
        case TFld::Boolean:	setB( atoi(val.c_str()) );      break;
    }
}

void Attr::setR( double val )
{
    switch( fld().type() )
    {
        case TFld::String:      setS(TSYS::real2str(val));      break;
        case TFld::Integer:	setI( (int)val );       break;
        case TFld::Real:
    	    if( !(fld().flg()&TFld::Selected) && fld().selValR()[0] < fld().selValR()[1] )
    	    {
                val = vmax(val,fld().selValR()[0]);
                val = vmin(val,fld().selValR()[1]);
            }
	    if( m_val.r_val == val )	break;
            if( fld().flg()&Attr::Prevent )
            {
                double t_val = m_val.r_val;
                m_val.r_val = val;
                if( !m_owner->attrChange(*this) )
            	    m_val.r_val = t_val;
	    }
            else m_val.r_val = val;
	    vl_modif++;
            break;
	case TFld::Boolean:	setB( val );    break;
    }
}

void Attr::setI( int val )
{
    switch( fld().type() )
    {
        case TFld::String:      setS(TSYS::int2str(val));       break;
        case TFld::Integer:
            if( !(fld().flg()&TFld::Selected) && fld().selValI()[0] < fld().selValI()[1] )
            {
    		val = vmax(val,m_fld->selValI()[0]);
		val = vmin(val,m_fld->selValI()[1]);
            }
	    if( m_val.i_val == val )	break;
            if( fld().flg()&Attr::Prevent )
            {
                int t_val = m_val.i_val;
                m_val.i_val = val;
                if( !m_owner->attrChange(*this) )
            	    m_val.i_val = t_val;
	    }
            else m_val.i_val = val;
	    vl_modif++;
            break;
        case TFld::Real:        setR( val );    break;
        case TFld::Boolean:	setB( val );    break;
    }
}

void Attr::setB( bool val )
{
    switch( fld().type() )
    {
        case TFld::String:      setS(TSYS::int2str(val));       break;
        case TFld::Integer:    	setI( val );    break;
        case TFld::Real:        setR( val );    break;
        case TFld::Boolean:
	    if( m_val.b_val == val )	break;
    	    if( fld().flg()&Attr::Prevent )
            {
                bool t_val = m_val.b_val;
                m_val.b_val = val;
                if( !m_owner->attrChange(*this) )
                    m_val.b_val = t_val;
            }
    	    else m_val.b_val = val;
	    vl_modif++;
        break;
    }
}

void Attr::setCfgTempl(const string &vl)
{
    if(cfg_tmpl == vl) return;
    cfg_tmpl = vl;
    cfg_modif++;
}

void Attr::setCfgVal( const string &vl )
{
    if( cfg_val == vl )	return;
    cfg_val = vl;
    cfg_modif++;
}

void Attr::AHDConnect()
{
    ResAlloc::resRequestR(m_owner->attr_res);
}

void Attr::AHDDisConnect()
{
    ResAlloc::resReleaseR(m_owner->attr_res);
}


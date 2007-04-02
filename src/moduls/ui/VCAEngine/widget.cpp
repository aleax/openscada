
//OpenSCADA system module UI.VCSEngine file: widget.cpp
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
    if( flag&TCntrNode::NodeConnect )
    {	
	attrAdd( new TFld("id",_("Id"),TFld::String,TFld::NoWrite,"","") );
	attrAdd( new TFld("name",_("Name"),TFld::String,TFld::NoFlag,"","") );
	attrAdd( new TFld("dscr",_("Description"),TFld::String,TFld::FullText,"","") );
	attrAdd( new TFld("en",_("Enabled"),TFld::Boolean,TFld::NoFlag,"","1") );
	attrAdd( new TFld("active",_("Active"),TFld::Boolean,TFld::NoFlag,"","0") );
	attrAdd( new TFld("geomX",_("Geometry:x"),TFld::Integer,TFld::NoFlag,"","0","0;10000") );
	attrAdd( new TFld("geomY",_("Geometry:y"),TFld::Integer,TFld::NoFlag,"","0","0;10000") );
	attrAdd( new TFld("geomW",_("Geometry:width"),TFld::Integer,TFld::NoFlag,"","100","0;10000") );
	attrAdd( new TFld("geomH",_("Geometry:height"),TFld::Integer,TFld::NoFlag,"","100","0;10000") );
	attrAdd( new TFld("geomZ",_("Geometry:z"),TFld::Integer,TFld::NoFlag,"","0","-1000000;1000000") );
	attrAdd( new TFld("geomMargin",_("Geometry:margin"),TFld::Integer,TFld::NoFlag,"","5","0;1000") );
    }
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
	if(wdg.attrPresent(als[i_a]))
	{
	    attrAt(als[i_a]).at().setS(wdg.attrAt(als[i_a]).at().getS());
	    attrAt(als[i_a]).at().modifVal(0);
	    attrAt(als[i_a]).at().modifCfg(0);
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
	if( parentNm() != "root")
	{
    	    //- Connect to parent widget -
    	    m_parent=mod->wlbAt(TSYS::strSepParse(parentNm(),0,'.')).at().
            	    at(TSYS::strSepParse(parentNm(),1,'.'));
	    //- Check for enable parent widget and enable if not -
	    if( !parent().at().enable() )	parent().at().setEnable(true);
	    //- Inherit attribute -
    	    attrInherit(this);
    	    //- Copy inherit attribute's values from parent widget -
    	    (*this) = parent().at();
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

    //- Enable process widgets from container -
    if(isContainer())
    {
	vector<string>	ls;
	wdgList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++ )
	    wdgAt(ls[i_l]).at().setEnable(val);
    }

    m_enable = val;
}

AutoHD<Widget> Widget::parent( )
{ 
    return m_parent; 
}

void Widget::attrInherit( Widget *wdg )
{
    if( !parent().freeStat() )    parent().at().attrInherit(wdg);
    if( wdg == this )   return;

    //- Inherit attributes to Widget 'wdg' -
    //-- Create no present attributes --
    vector<string>  ls;
    attrList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++)
        if(!wdg->attrPresent(ls[i_l]))
        {
            TFld *fel = new TFld(attrAt(ls[i_l]).at().fld());
            //Clear user attribute flag and set inherit flag
            fel->setFlg(fel->flg()&(~(Attr::IsUser))|Attr::IsInher);
            wdg->attrAdd( fel );
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
    if(!parent().freeStat())  return parent().at().attrChange(cfg);
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

void Widget::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget: ")+id());
	if(isContainer() && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
            ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),R_R_R_,"root","root",1,"list","/inclwdg/wdg");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",0444);
        if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")))
	{
	    if(ctrMkNode("area",opt,-1,"/wdg/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enable"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/wdg/st/user",_("User and group"),RWRWR_,"root","UI",3,"tp","str","dest","select","select","/wdg/u_lst");
		ctrMkNode("fld",opt,-1,"/wdg/st/grp","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/wdg/g_lst");
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/wdg/w_lst");
		if(parentNm() != "root")
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
    	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes")))
	{
	    //Properties form create
	    vector<string> list_a;
	    attrList(list_a);
	    for( unsigned i_el = 0; i_el < list_a.size(); i_el++ )
		attrAt(list_a[i_el]).at().fld().cntrCmdMake(opt,"/attr",-1,user().c_str(),grp().c_str(),permit());
	}
	if(ctrMkNode("area",opt,-1,"/links",_("Links")))
	{
	    ctrMkNode("fld",opt,-1,"/links/onlyAttr",_("Only atributes show"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links")))
	    {
		vector<string> alist, list;
		attrList(alist);
		for( int i_a = 0; i_a < alist.size(); i_a++ )
                {
		    if( !(attrAt(alist[i_a]).at().flgSelf()&(Attr::LnkIn|Attr::LnkOut)) ) continue;
                    //Check select param
                    if( attrAt(alist[i_a]).at().lnkTempl().size() && !atoi(TBDS::genDBGet(mod->nodePath()+"onlAttr","0",opt->attr("user")).c_str()) )
                    {
                        string nprm = TSYS::strSepParse(attrAt(alist[i_a]).at().lnkTempl(),0,'|');
	                //Check already to present parameters
	                bool f_ok = false;
	                for( int i_l = 0; i_l < list.size(); i_l++ )
			    if( list[i_l] == nprm ) { f_ok = true; break; }
                        if(!f_ok)
                        {
                    	    ctrMkNode("fld",opt,-1,(string("/links/lnk/pr_")+alist[i_a]).c_str(),nprm,permit(),user().c_str(),grp().c_str(),
                                3,"tp","str","dest","sel_ed","select",(string("/links/lnk/pl_")+alist[i_a]).c_str());
                    	    list.push_back(nprm);
                        }
		    }
		    else ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+alist[i_a]).c_str(),
				    attrAt(alist[i_a]).at().fld().name(),permit(),user().c_str(),grp().c_str(),
				    3,"tp","str","dest","sel_ed","select",(string("/links/lnk/ls_")+alist[i_a]).c_str());
		}
	    }
	}
	if(!isLink() && ctrMkNode("area",opt,-1,"/attrcfg",_("Attributes configuration")))
	{
            if(ctrMkNode("table",opt,-1,"/attrcfg/attr",_("Attributes"),permit(),user().c_str(),grp().c_str(),2,"s_com","add,del","key","id"))
            {
        	ctrMkNode("list",opt,-1,"/attrcfg/attr/id",_("Id"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
	        ctrMkNode("list",opt,-1,"/attrcfg/attr/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
        	ctrMkNode("list",opt,-1,"/attrcfg/attr/type",_("Type"),permit(),user().c_str(),grp().c_str(),4,"tp","dec","idm","1","dest","select","select","/attrcfg/tp_ls");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/wa",_("Work area"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/calc",_("Calcul"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/lnk",_("Link"),permit(),user().c_str(),grp().c_str(),4,"tp","dec","idm","1","dest","select","select","/attrcfg/lnk_ls");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/lnktmpl",_("Link template"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/attrcfg/calc",_("Calculate")))
	    {
		ctrMkNode("fld",opt,-1,"/attrcfg/calc/progLng",_("Programm language"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/attrcfg/calc/plangLs");
		ctrMkNode("fld",opt,-1,"/attrcfg/calc/prog",_("Programm"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","90","rows","8");
	    }
	}
        if(isContainer()&&(!isLink())&&ctrMkNode("area",opt,-1,"/inclwdg",_("Include widgets")))
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","wdg_");
        return;
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
    else if( a_path == "/wdg/st/goparent" && ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        opt->setText("/UI/mod_"+mod->modId()+"/wlb_"+TSYS::strSepParse(parentNm(),0,'.')+"/wdg_"+TSYS::strSepParse(parentNm(),1,'.'));
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
    else if( a_path.substr(0,5) == "/attr" && TSYS::pathLev(a_path,1).size() > 4 && TSYS::pathLev(a_path,1).substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
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
    else if( a_path == "/links/onlyAttr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"onlAttr","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    TBDS::genDBSet(mod->nodePath()+"onlAttr",opt->text(),opt->attr("user"));
    }
    else if( a_path.substr(0,14) == "/links/lnk/pr_" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    string lnk_val = attrAt(a_path.substr(14)).at().lnkVal();
	    int c_lvl = 0;
	    while(TSYS::strSepParse(lnk_val,c_lvl,'.').size())	c_lvl++;
	    if( (TSYS::strSepParse(lnk_val,0,'.') == "P" && c_lvl==5) ||
		    (TSYS::strSepParse(lnk_val,0,'.') == "W" && c_lvl==3) )
		opt->setText(lnk_val.substr(0,lnk_val.rfind(".")));
	    else opt->setText(lnk_val);
	}
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    /*vector<string> a_ls;
	    string no_set;
	    bool noonly_no_set = true;
	    string lnk_val = opt->text();
	    string p_nm = TSYS::strSepParse(attrAt(a_path.substr(14)).at().lnkTempl(),0,'|');
	    int c_lvl = 0;
	    while(TSYS::strSepParse(lnk_val,c_lvl,'.').size())  c_lvl++;

	    attrList(a_ls);

	    AutoHD<TValue> prm;
	    if(TSYS::strSepParse(lnk_val,0,'.') == "P" && c_lvl==4)
		prm = SYS->daq().at().at(TSYS::strSepParse(lnk_val,1,'.')).at().at(TSYS::strSepParse(lnk_val,2,'.')).at().
			at(TSYS::strSepParse(lnk_val,3,'.'));
	    AutoHD<Widget> wdg;
	    if(TSYS::strSepParse(lnk_val,0,'.') == "W" && c_lvl==2)
	    {
		string nwdg = TSYS::strSepParse(lnk_val,1,'.');
		if(nwdg=="base") wdg = isLink() ? nodePrev() : this;
		else		 wdg = isLink() ? wdgAt(nwdg) : ownWdg().wdgAt(nwdg);
	    }

	    for( int i_a = 0; i_a < a_ls.size(); i_a++ )
		if( p_nm == TSYS::strSepParse(attrAt(a_ls[i_a]).at().lnkTempl(),0,'|') )
		{
		    attrAt(a_ls[i_a]).at().lnkVal(lnk_val);
		    string p_attr = TSYS::strSepParse(attrAt(a_ls[i_a]).at().lnkTempl(),1,'|');
		    if(!prm.freeStat() || !wdg.freeStat())
		    {
			if( (!prm.freeStat() && prm.at().vlPresent(p_attr)) ||
			    (!wdg.freeStat() && wdg.at().attrPresent(p_attr)) )
		    	{
			    attrAt(a_ls[i_a]).at().lnkVal(lnk_val+"."+p_attr);
			    noonly_no_set = false;
			}
			else no_set+=p_attr+",";
		    }
		}
	    if(!prm.freeStat() || !wdg.freeStat())
	    {
		if( noonly_no_set )     throw TError(nodePath().c_str(),_("Parameter have not only atributes!"));
                else if( no_set.size() )throw TError(nodePath().c_str(),_("Parameter have not atributes: %s !"),no_set.c_str());
	    }*/
	}
    }
    else if( (a_path.substr(0,14) == "/links/lnk/pl_" || a_path.substr(0,14) == "/links/lnk/ls_")
	    && ctrChkNode(opt) )
    {
	/*string m_prm = attrAt(a_path.substr(14)).at().lnkVal();
	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
        int c_lv;
	string c_path = "";
	opt->childAdd("el")->text(c_path);
	string obj_tp = TSYS::strSepParse(m_prm,0,'.');
	for( c_lv = 0; TSYS::strSepParse(m_prm,c_lv,'.').size(); c_lv++ )
	{
	    if( is_pl && ((obj_tp=="P" && c_lv>3) || (obj_tp=="W" && c_lv)) )	break;
	    if( c_lv ) c_path+=".";
	    c_path = c_path+TSYS::strSepParse(m_prm,c_lv,'.');
	    opt->childAdd("el")->text(c_path);
	}
	if(c_lv) c_path+=".";
	string prm1 = TSYS::strSepParse(m_prm,1,'.');
	string prm2 = TSYS::strSepParse(m_prm,2,'.');
	string prm3 = TSYS::strSepParse(m_prm,3,'.');
	vector<string>	ls;
	switch(c_lv)
	{
	    case 0:	ls.push_back("P"); ls.push_back("W"); break;
	    case 1:
		if(obj_tp=="P")	SYS->daq().at().modList(ls);
		if(obj_tp=="W")
		{
		    if(libLocated()) wdgList(ls);
		    else ownWdg().wdgList(ls);
		    ls.push_back("base");
		}
		break;
	    case 2:
		if(obj_tp=="P" && SYS->daq().at().modPresent(prm1))
		    SYS->daq().at().at(prm1).at().list(ls);
		if(!is_pl && obj_tp=="W")
		{
		    AutoHD<Widget> wdg;
		    if(prm1=="base") wdg = libLocated() ? this : &ownWdg();
                    else             wdg = libLocated() ? wdgAt(prm1) : ownWdg().wdgAt(prm1);
		    if(!wdg.freeStat())	wdg.at().attrList(ls);
		}
		break;
	    case 3:
		if(obj_tp=="P" && SYS->daq().at().modPresent(prm1)
			&& SYS->daq().at().at(prm1).at().present(prm2))
		    SYS->daq().at().at(prm1).at().at(prm2).at().list(ls);
		break;
	    case 4:
 		if(!is_pl && obj_tp=="P" && SYS->daq().at().modPresent(prm1)
			&& SYS->daq().at().at(prm1).at().present(prm2)
			&& SYS->daq().at().at(prm1).at().at(prm2).at().present(prm3) )
		    SYS->daq().at().at(prm1).at().at(prm2).at().at(prm3).at().vlList(ls);
		break;
	}
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->text(c_path+ls[i_l]);*/
    }
    else if( a_path.substr(0,14) == "/links/lnk/el_" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	    opt->setText(attrAt(a_path.substr(14)).at().lnkVal());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    attrAt(a_path.substr(14)).at().lnkVal(opt->text());
    }
    else if( !isLink() && a_path == "/attrcfg/attr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
    	    XMLNode *n_id      = ctrMkNode("list",opt,-1,"/attrcfg/attr/id","");
	    XMLNode *n_name    = ctrMkNode("list",opt,-1,"/attrcfg/attr/name","");
	    XMLNode *n_type    = ctrMkNode("list",opt,-1,"/attrcfg/attr/type","");
	    XMLNode *n_wa      = ctrMkNode("list",opt,-1,"/attrcfg/attr/wa","");
	    XMLNode *n_calc    = ctrMkNode("list",opt,-1,"/attrcfg/attr/calc","");
	    XMLNode *n_lnk     = ctrMkNode("list",opt,-1,"/attrcfg/attr/lnk","");
	    XMLNode *n_lnktmpl = ctrMkNode("list",opt,-1,"/attrcfg/attr/lnktmpl","");

	    vector<string> lst;
	    //Self widget's attributes
	    attrList(lst);
	    for(int i_el = 0; i_el < lst.size(); i_el++)
	    {
		if( n_id )	n_id->childAdd("el")->setText(lst[i_el]);
		if( n_name )    n_name->childAdd("el")->setText(attrAt(lst[i_el]).at().name());
		if( n_type )	n_type->childAdd("el")->setText(TSYS::int2str(attrAt(lst[i_el]).at().fld().type()+
				    ((attrAt(lst[i_el]).at().fld().flg()&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
		if( n_wa )	n_wa->childAdd("el")->setText( attrAt(lst[i_el]).at().fld().values()+"|"+
							    attrAt(lst[i_el]).at().fld().selNames());
		if( n_calc )	n_calc->childAdd("el")->setText(TSYS::int2str(attrAt(lst[i_el]).at().flgSelf()&Attr::AttrCalc));
		if( n_lnk )	n_lnk->childAdd("el")->setText(TSYS::int2str(attrAt(lst[i_el]).at().flgSelf()&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob)));
		if( n_lnktmpl )	n_lnktmpl->childAdd("el")->setText(attrAt(lst[i_el]).at().lnkTempl());
	    }
	    //Include widget's attributes
	    if(isContainer())
	    {
		wdgList(lst);
		for(int i_aw = 0; i_aw < lst.size(); i_aw++)
		{
		    vector<string> alst;
		    wdgAt(lst[i_aw]).at().attrList(alst);
		    for(int i_ai = 0; i_ai < alst.size(); i_ai++)
		    {
			if( n_id )	n_id->childAdd("el")->setText(lst[i_aw]+"_"+alst[i_ai]);
			if( n_name )	n_name->childAdd("el")->setText(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().name());
			if( n_type )    n_type->childAdd("el")->setText(TSYS::int2str(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().type()+
                                    	    ((wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().flg()&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
			if( n_wa )	n_wa->childAdd("el")->setText( wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().values()+"|"+
								    wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().selNames());
			if( n_calc )	n_calc->childAdd("el")->setText(TSYS::int2str(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().flgSelf()&Attr::AttrCalc));
			if( n_lnk )	n_lnk->childAdd("el")->setText(TSYS::int2str(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().flgSelf()&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob)));
			if( n_lnktmpl )	n_lnktmpl->childAdd("el")->setText(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().lnkTempl());
		    }
		}
	    }
	}
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            attrAdd(new TFld("newAttr",_("New attribute"),TFld::String,Attr::IsUser));
            attrAt("newAttr").at().modifCfg(1);
        }
        if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
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

	    if( idcol == "id" || idcol == "name" || idcol == "type" || idcol == "wa" )
	    {
                string          tnm     = attrAt(idattr).at().name();
                TFld::Type      ttp     = attrAt(idattr).at().fld().type();
                unsigned        tflg    = attrAt(idattr).at().fld().flg();

                if(!attrPresent(idattr) || !(attrAt(idattr).at().fld().flg()&Attr::IsUser) )
            	    throw TError(nodePath().c_str(),_("Change a no user attribute is no permit"));
                if( idcol == "id" )
                {
                    attrAdd( new TFld(opt->text().c_str(),tnm.c_str(),ttp,tflg|Attr::IsUser) );
            	    attrAt(opt->text().c_str()).at().modifCfg(1);
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
	                attrAt(idattr.c_str()).at().modifCfg(1);
	            }
	            else if( (tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address) )
	                attrAt(idattr).at().fld().setFlg(attrAt(idattr).at().fld().flg()^((tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address)));
	        }
		else if( idcol == "wa" )
		{
		    attrAt(idattr).at().fld().setValues(TSYS::strSepParse(opt->text().c_str(),0,'|'));
		    attrAt(idattr).at().fld().setSelNames(TSYS::strSepParse(opt->text().c_str(),1,'|'));
		}
	    }
	    else
	    {
		AutoHD<Attr> attr;
		if(attrPresent(idattr) || !isContainer() )
		    attr = attrAt(idattr);
		else attr = wdgAt(TSYS::strSepParse(idattr,0,'_')).at().attrAt(TSYS::strSepParse(idattr,1,'_'));

		Attr::SelfAttrFlgs sflg = attr.at().flgSelf();

		if( idcol == "calc" )
		{
		    Attr::SelfAttrFlgs stflg = atoi(opt->text().c_str())?Attr::AttrCalc:(Attr::SelfAttrFlgs)0;
		    attr.at().flgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::AttrCalc)) );
		}
		else if( idcol == "lnk" )
        	{
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)atoi(opt->text().c_str());
		    if( (sflg^stflg)&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob) )
			attr.at().flgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob))) );
		}
		else if( idcol == "lnktmpl" )	attr.at().lnkTempl(opt->text());
	    }
	}
    }
    else if( a_path == "/attrcfg/calc/progLng" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(calcLang());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setCalcLang(opt->text());
    }
    else if( a_path == "/attrcfg/calc/prog" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(calcProg());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setCalcProg(opt->text());
    }
    else if( a_path == "/attrcfg/calc/plangLs" && ctrChkNode(opt) )
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
    else if( a_path == "/attrcfg/tp_ls" && ctrChkNode(opt) )
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
    else if( a_path == "/attrcfg/lnk_ls" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->setAttr("id",TSYS::int2str(0))->setText(_("No link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::LnkIn))->setText(_("Local input link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::LnkOut))->setText(_("Local output link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::LnkIn|Attr::LnkOut))->setText(_("Local full link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::LnkIn|Attr::LnkGlob))->setText(_("Global input link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::LnkOut|Attr::LnkGlob))->setText(_("Global output link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob))->setText(_("Global full link"));
    }
    else if( isContainer() && (!isLink()) && a_path == "/inclwdg/wdg" )
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
    else if( a_path == "/wdg/w_lst" && ctrChkNode(opt) )
    {
        int c_lv = 0;
        string c_path = "";
        string lnk = parentNm();

	opt->childAdd("el")->setText(c_path);
	while(TSYS::strSepParse(lnk,c_lv,'.').size())
	{
	    if( c_lv ) c_path+=".";
	    c_path = c_path+TSYS::strSepParse(lnk,c_lv,'.');
	    opt->childAdd("el")->setText(c_path);
	    c_lv++;
	}
        if(c_lv) c_path+=".";
        vector<string>  ls;
        switch(c_lv)
        {
            case 0: mod->wlbList(ls);   break;
            case 1:
	        if( mod->wlbPresent(TSYS::strSepParse(lnk,0,'.')) )
                    mod->wlbAt(TSYS::strSepParse(lnk,0,'.')).at().list(ls);
                break;
        }
        for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
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

void Attr::flgSelf( SelfAttrFlgs flg )
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

void Attr::lnkTempl(const string &vl)
{
    if(lnk_tmpl == vl) return;
    lnk_tmpl = vl;
    cfg_modif++;
}

void Attr::lnkVal( const string &vl )
{
    if(lnk_val == vl) return;
    lnk_val = vl;
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



//OpenSCADA system module UI.VCAEngine file: widget.cpp
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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

#include <resalloc.h>
#include <tsys.h>

#include "vcaengine.h"
#include "widget.h"

using namespace VCA;

//************************************************
//* Widget                                       *
//************************************************
Widget::Widget( const string &id, const string &isrcwdg ) :
        m_enable(false), m_lnk(false), m_id(id.substr(0,30)), m_parent_nm(isrcwdg)
{
    inclWdg = grpAdd("wdg_");
    attrId  = grpAdd("a_");

    attr_cfg.valAtt(this);
}

Widget::~Widget()
{
    nodeDelAll( );
    attr_cfg.valDet(this);
}

TCntrNode &Widget::operator=( TCntrNode &node )
{
    Widget *src_n = dynamic_cast<Widget*>(&node);
    if( !src_n ) return *this;

    if( !src_n->enable() ) return *this;

    //- Parent link copy -
    if( parentNm() != src_n->parentNm() && enable() ) setEnable(false);
    setParentNm(src_n->parentNm());
    if( !enable() ) setEnable(true);

    //- Copy generic configuration -
    if( src_n->parent().freeStat() || src_n->name() != src_n->parent().at().name() )	setName(src_n->name());
    if( src_n->parent().freeStat() || src_n->descr() != src_n->parent().at().descr() )	setDescr(src_n->descr());
    if( src_n->parent().freeStat() || src_n->ico() != src_n->parent().at().ico() )	setIco(src_n->ico());
    setUser(src_n->user());
    setGrp(src_n->grp());
    setPermit(src_n->permit());
    if( src_n->parent().freeStat() || src_n->calcLang() != src_n->parent().at().calcLang() )	setCalcLang(src_n->calcLang());
    if( src_n->parent().freeStat() || src_n->calcProg() != src_n->parent().at().calcProg() )	setCalcProg(src_n->calcProg());
    if( src_n->parent().freeStat() || src_n->calcPer() != src_n->parent().at().calcPer() )	setCalcPer(src_n->calcPer());

    //- Copy attributes -
    vector<string> els;
    src_n->attrList(els);
    AutoHD<Attr> attr, pattr;
    for( int i_a = 0; i_a < els.size(); i_a++ )
    {
  	if( !attrPresent(els[i_a]) )
	{
	    attrAdd( new TFld(src_n->attrAt(els[i_a]).at().fld()) );
	    attrAt(els[i_a]).at().setModif(1);
	}
	if( els[i_a]=="id" || els[i_a]=="path" ) continue;
       	attr  = attrAt(els[i_a]);
	pattr = src_n->attrAt(els[i_a]);
	attr.at().setFlgSelf(pattr.at().flgSelf());
    	switch(attr.at().type())
	{
	    case TFld::Boolean:	attr.at().setB(pattr.at().getB());	break;
	    case TFld::Integer:	attr.at().setI(pattr.at().getI());	break;
	    case TFld::Real:	attr.at().setR(pattr.at().getR());	break;
	    case TFld::String:	attr.at().setS(pattr.at().getS());	break;
	}
	attr.at().setCfgTempl(pattr.at().cfgTempl());
	attr.at().setCfgVal(pattr.at().cfgVal());
    }

    //- Include widgets copy -
    if( !isLink( ) && src_n->isContainer() )
    {
    	src_n->wdgList(els);
      	for( int i_w = 0; i_w < els.size(); i_w++ )
	{
	    if( !wdgPresent(els[i_w]) )	wdgAdd(els[i_w],"","");
	    (TCntrNode&)wdgAt(els[i_w]).at() = (TCntrNode&)src_n->wdgAt(els[i_w]).at();
	}
    }
    
    return *this;
}

void Widget::postEnable(int flag)
{
    if( flag&TCntrNode::NodeRestore )	setEnable(true);
    if( flag&TCntrNode::NodeConnect )
    {
	//- Add main attributes -
	attrAdd( new TFld("id",_("Id"),TFld::String,TFld::NoWrite,"","","","","1") );
        attrAdd( new TFld("name",_("Name"),TFld::String,TFld::NoFlag,"","","","","2") );
	attrAdd( new TFld("dscr",_("Description"),TFld::String,TFld::FullText,"","","","","3") );
        attrAdd( new TFld("path",_("Path"),TFld::String,TFld::NoWrite,"","","","","4") );
	attrAdd( new TFld("en",_("Enabled"),TFld::Boolean,TFld::NoFlag,"","1","","","5") );
	attrAdd( new TFld("active",_("Active"),TFld::Boolean,Attr::Active,"","0","","","6") );
	attrAdd( new TFld("geomX",_("Geometry:x"),TFld::Real,TFld::NoFlag,"","0","0;10000","","7") );
	attrAdd( new TFld("geomY",_("Geometry:y"),TFld::Real,TFld::NoFlag,"","0","0;10000","","8") );
	attrAdd( new TFld("geomW",_("Geometry:width"),TFld::Real,TFld::NoFlag,"","100","0;10000","","9") );
	attrAdd( new TFld("geomH",_("Geometry:height"),TFld::Real,TFld::NoFlag,"","100","0;10000","","10") );
	attrAdd( new TFld("geomXsc",_("Geometry:x scale"),TFld::Real,TFld::NoFlag,"","1","0.1;10","","13") );
	attrAdd( new TFld("geomYsc",_("Geometry:y scale"),TFld::Real,TFld::NoFlag,"","1","0.1;10","","14") );
	attrAdd( new TFld("geomZ",_("Geometry:z"),TFld::Integer,TFld::NoFlag,"","0","0;1000000","","11") );
	attrAdd( new TFld("geomMargin",_("Geometry:margin"),TFld::Integer,TFld::NoFlag,"","0","0;1000","","12") );
	attrAdd( new TFld("evProc",_("Events process"),TFld::String,TFld::FullText,"200") );
    }
    
    attrAt("id").at().setS(id());	attrAt("id").at().setModif(0);
    attrAt("path").at().setS(path());	attrAt("path").at().setModif(0);
}

void Widget::preDisable( int flag )
{
    //- Delete heritors widgets -
    while( herit().size() )	mod->nodeDel(herit()[0].at().path(),0,flag|0x10);
    
    //- Disable widget -
    if( enable() )  setEnable(false);
}

string Widget::rootId( )
{
    if( !parent().freeStat() )  return parent().at().rootId();
    return "";
}

string Widget::name( )
{
    return (attrAt("name").at().getS().size())?attrAt("name").at().getS():m_id;
}
    
void Widget::setName( const string &inm )
{
    attrAt("name").at().setS(inm);
}

string Widget::descr( )
{
    return attrAt("dscr").at().getS();
}
    
void Widget::setDescr( const string &idscr )
{
    attrAt("dscr").at().setS(idscr);
}

bool Widget::isContainer( )
{
    if( !parent().freeStat() )    return parent().at().isContainer();
    return false;
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
	    try
	    {
		if( parentNm() == ".." ) m_parent=AutoHD<TCntrNode>(nodePrev());
		else m_parent=mod->nodeAt(parentNm());
		//- Check for enable parent widget and enable if not -
		if( !parent().at().enable() )	parent().at().setEnable(true);
		//- Inherit -
    		inheritAttr( );
    		inheritIncl( );
		//- Register of heritater -
		parent().at().heritReg(this);
		
	    }catch(TError err) 
	    { 
		mess_err(err.cat.c_str(),err.mess.c_str());
		m_parent.free(); 
		throw;
	    }
	}	
	m_enable = true;
        //- Load self values from DB -
        loadIO();
    }
    if(!val)
    {
	//- Disable heritors widgets -
	for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	    if( m_herit[i_h].at().enable( ) )
		try { m_herit[i_h].at().setEnable(false); }
		catch(...)
		{ mess_err(nodePath().c_str(),_("Heritors widget <%s> disable error"),m_herit[i_h].at().id().c_str()); }    
        
	//- Free inherit attributes -
        vector<string>  ls;
        attrList(ls);
        for(int i_l = 0; i_l < ls.size(); i_l++)
            if( attrAt(ls[i_l]).at().flgGlob()&Attr::IsInher )
                attrDel( ls[i_l] );

	if(!m_parent.freeStat()) 
	{
	    //- Unregister heritater -	
	    parent().at().heritUnreg(this);
    	    //- Disconnect parent widget -
    	    m_parent.free();
	}
    }

    //- Enable/disable process widgets from container -
    vector<string>	ls;
    wdgList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
	if( val != wdgAt(ls[i_l]).at().enable() )
	    try { wdgAt(ls[i_l]).at().setEnable(val); }
	    catch(...)
	    { mess_err(nodePath().c_str(),_("Child widget <%s> enable/disable error"),ls[i_l].c_str()); }

    m_enable = val;
}

AutoHD<Widget> Widget::parent( )
{ 
    return m_parent;
}

AutoHD<Widget> Widget::parentNoLink( )
{
    if( parent().at().isLink() ) return parent().at().parentNoLink( );
    return parent();
}

void Widget::heritReg( Widget *wdg )
{
    //- Search already herited widget -
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( &m_herit[i_h].at() == wdg )	return;
    m_herit.push_back(AutoHD<Widget>(wdg));
}

void Widget::heritUnreg( Widget *wdg )
{
    //- Search herited widget -
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( &m_herit[i_h].at() == wdg )	
	{
	    m_herit.erase(m_herit.begin()+i_h);
	    return;
	}
}

void Widget::inheritAttr( const string &iattr )
{
    if( parent().freeStat() )	return;

    //- Create no present attributes -
    vector<string>  ls;
    if( !iattr.empty() && parent().at().attrPresent(iattr) )
	ls.push_back(iattr);
    else parent().at().attrList(ls);

    AutoHD<Attr> attr, pattr;
    for(int i_l = 0; i_l < ls.size(); i_l++)
    {
        if( !attrPresent(ls[i_l]) )
        {
	    if( parent().at().attrAt(ls[i_l]).at().flgGlob()&Attr::Mutable ) continue;
            TFld *fel = new TFld(parent().at().attrAt(ls[i_l]).at().fld());
            //-- Clear user attribute flag and set inherit flag --
            fel->setFlg(fel->flg()|Attr::IsInher);
            attrAdd(fel);
        }
	if( ls[i_l]=="id" || ls[i_l]=="path" || attrAt(ls[i_l]).at().modif() )	continue;
	attr  = attrAt(ls[i_l]);
	pattr = parent().at().attrAt(ls[i_l]);
	attr.at().setFlgSelf(pattr.at().flgSelf());
	switch(attr.at().type())
	{
	    case TFld::Boolean:	attr.at().setB(pattr.at().getB(),true);	break;
	    case TFld::Integer:	attr.at().setI(pattr.at().getI(),true);	break;
	    case TFld::Real:	attr.at().setR(pattr.at().getR(),true);	break;
	    case TFld::String:	attr.at().setS(pattr.at().getS(),true);	break;
	}	    
	//-- No inherit calc flag for links --
	if( isLink() && !parent().at().isLink() )
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)(attr.at().flgSelf()&(~Attr::ProcAttr)));
	attr.at().setCfgTempl(pattr.at().cfgTempl());
	attr.at().setCfgVal(pattr.at().cfgVal());
	attr.at().setModif(0);
    }
}

void Widget::inheritIncl( const string &iwdg )
{    
    AutoHD<Widget> parw = parent();
    while( !parw.freeStat() && parw.at().isLink() )
	parw = parw.at().parent();
    if( parw.freeStat() ) return;

    //- Create no present include widgets for no link and container widgets -
    vector<string>  ls;
    if( !iwdg.empty() && parw.at().wdgPresent(iwdg) )
	ls.push_back(iwdg);
    else parw.at().wdgList(ls);
    for( int i_w = 0; i_w < ls.size(); i_w++ )
	if( !wdgPresent(ls[i_w]) )
    	    wdgAdd(ls[i_w],"",parw.at().wdgAt(ls[i_w]).at().path());
}

void Widget::attrAdd( TFld *attr, int pos )
{
    string anm = attr->name();
    if(attrPresent(anm))
    {	
	delete attr;
        throw TError(nodePath().c_str(),_("Attribut %s already present."),anm.c_str());
    }
    attr_cfg.fldAdd(attr,pos);
}

void Widget::attrDel( const string &attr )
{
    if( !attrPresent(attr) )	return;
    int flg = attrAt(attr).at().flgGlob();
    attr_cfg.fldDel(attr_cfg.fldId(attr));
    //- Delete from inheritant wigets -
    if( !(flg&Attr::Mutable) )    
	for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	    if( m_herit[i_h].at().enable( ) )
    		m_herit[i_h].at().attrDel( attr );
}

bool Widget::attrChange( Attr &cfg, void *prev )
{
    //- Process Active attribute's mode
    if( cfg.flgGlob()&Attr::Active && prev && !parent().freeStat() )	parent().at().attrChange(cfg,prev);
    if( cfg.owner() != this )	return false;
    
    //- Update heritors attributes -
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( m_herit[i_h].at().enable( ) )
    	    m_herit[i_h].at().inheritAttr(cfg.id());    
    
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
    
    //- Call heritors include widgets update -
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( m_herit[i_h].at().enable( ) )
    	    m_herit[i_h].at().inheritIncl(wid);
}

void Widget::wdgDel( const string &wid, bool full )
{
    if( wdgPresent(wid) ) chldDel(inclWdg,wid,-1,full);
}

AutoHD<Widget> Widget::wdgAt( const string &wdg )
{
    return chldAt(inclWdg,wdg);
}

void Widget::addFld( TElem *el, unsigned iid )
{
    chldAdd(attrId,new Attr(el->fldAt(iid)));
}

void Widget::delFld( TElem *el, unsigned iid )
{
    if( nodeMode() == TCntrNode::Enable && chldPresent(attrId,el->fldAt(iid).name()) )
        chldDel(attrId,el->fldAt(iid).name());
}

void Widget::detElem( TElem *el )
{

}

bool Widget::cntrCmdServ( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/0" )	//Attribute's access
    {
	if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )		//Get values
	{
    	    vector<string> ls;
    	    attrList(ls);
	    AutoHD<Attr> attr;
    	    for( int i_l = 0; i_l < ls.size(); i_l++ )
	    {
		attr = attrAt(ls[i_l]);
        	opt->childAdd("el")->setAttr("id",ls[i_l].c_str())->
			    	     setAttr("pos",attr.at().fld().reserve())->
				     setText(attr.at().getS());
	    }
	}
	else if( ctrChkNode(opt,"set",RWRWRW,"root","root",SEQ_WR) )	//Set values
	    for( int i_ch = 0; i_ch < opt->childSize(); i_ch++ )
	        attrAt(opt->childGet(i_ch)->attr("id")).at().setS(opt->childGet(i_ch)->text());
	return true;
    }
    return false;
}

bool Widget::cntrCmdGeneric( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget: ")+id(),permit(),user().c_str(),grp().c_str());
        if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")))
	{
	    if(ctrMkNode("area",opt,-1,"/wdg/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enable"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/wdg/st/user",_("User and group"),(isLink()?R_R_R_:RWRWR_),"root","UI",3,"tp","str","dest","select","select","/wdg/u_lst");
		ctrMkNode("fld",opt,-1,"/wdg/st/grp","",(isLink()?R_R_R_:RWR_R_),user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/wdg/g_lst");
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/wdg/w_lst");
		if(!parent().freeStat())
		    ctrMkNode("comm",opt,-1,"/wdg/st/goparent",_("Go to parent"),permit(),user().c_str(),grp().c_str(),1,"tp","lnk");
	    }
	    if(ctrMkNode("area",opt,-1,"/wdg/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/cfg/id",_("Id"),R_R_R_,"root","UI",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/type",_("Type"),R_R_R_,"root","UI",1,"tp","str");		
		ctrMkNode("fld",opt,-1,"/wdg/cfg/root",_("Root"),R_R_R_,"root","UI",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/path",_("Path"),R_R_R_,"root","UI",1,"tp","str");		
		ctrMkNode("fld",opt,-1,"/wdg/cfg/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/descr",_("Description"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","50","rows","3");
                ctrMkNode("img",opt,-1,"/wdg/cfg/ico",_("Icon"),permit(),user().c_str(),grp().c_str(),2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/u_a",_("Access: user-grp-other"),(isLink()?R_R_R_:RWR_R_),user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/g_a","",(isLink()?R_R_R_:RWR_R_),user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/o_a","",(isLink()?R_R_R_:RWR_R_),user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		ctrMkNode("comm",opt,-1,"/wdg/cfg/load",_("Load"),permit(),user().c_str(),grp().c_str());
		ctrMkNode("comm",opt,-1,"/wdg/cfg/save",_("Save"),permit(),user().c_str(),grp().c_str());
	    }
	}
        if(isContainer() && (!isLink()) && ctrMkNode("area",opt,-1,"/inclwdg",_("Include widgets")))
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","wdg_");
	if(isContainer() && (!isLink()) && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
            ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),permit(),user().c_str(),grp().c_str(),1,"idm","1");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",0444);	    
	return true;
    }
    
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/wdg/res" && ctrChkNode(opt) )	//Service command for resources request
    {
	string mime;
	opt->setText(resourceGet(opt->attr("id"),&mime));
	opt->setAttr("mime",mime);
    }    
    else if( a_path == "/wdg/st/en" )
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
    else if( a_path == "/wdg/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/wdg/cfg/type" && ctrChkNode(opt) )	opt->setText(type());
    else if( a_path == "/wdg/cfg/root" && ctrChkNode(opt) )	opt->setText(rootId());
    else if( a_path == "/wdg/cfg/path" && ctrChkNode(opt) )	
	opt->setText((isLink()&&atoi(opt->attr("resLink").c_str())) ? parentNoLink().at().path() : path());	
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
        string c_path = "", c_el;
        string lnk = parentNm();

	opt->childAdd("el")->setText(c_path);
	for( int c_off = 0; (c_el=TSYS::pathLev(lnk,0,true,&c_off)).size(); c_lv++ )
	{
	    c_path += "/"+c_el;
	    opt->childAdd("el")->setText(c_path);
	}
        vector<string>  ls;
        switch(c_lv)
        {
            case 0: mod->nodeList(ls);   break;
            case 1:
		if( TSYS::pathLev(lnk,0) != ".." )
            	    mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeList(ls);
                break;
        }
        for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->setText(c_path+"/"+ls[i_l]);
    }    
    else if( a_path == "/br/wdg_" || a_path == "/inclwdg/wdg" )
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
    //- Get page info -
    if( opt->name() == "info" )
    { 
     	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes")))
	{
	    //-- Properties form create --
	    vector<string> list_a;
	    attrList(list_a);
	    for( unsigned i_el = 0; i_el < list_a.size(); i_el++ )
	    {
		XMLNode *el = attrAt(list_a[i_el]).at().fld().cntrCmdMake(opt,"/attr",-1,
					    user().c_str(),grp().c_str(),permit());
		if( el ) el->setAttr("wdgFlg",TSYS::int2str(attrAt(list_a[i_el]).at().flgGlob()));
	    }
	}
	return true;
    }
    
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path.substr(0,5) == "/attr" && 
	    TSYS::pathLev(a_path,1).size() > 4 && 
	    TSYS::pathLev(a_path,1).substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
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
            else 
	    {
		//printf("Set attr %s to %s\n",attr.at().id().c_str(),opt->text().c_str());
		attr.at().setS(opt->text());
	    }
	}
    }
    else return false;
    
    return true;
}

bool Widget::cntrCmdLinks( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
 	if(ctrMkNode("area",opt,-1,"/links",_("Links")))
	{
	    ctrMkNode("fld",opt,-1,"/links/showAttr",_("Atributes show"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links")))
	    {
		bool shwAttr = atoi(opt->attr("showAttr").c_str()) ||
			       atoi(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")).c_str());
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
			string nprm  = wdg.at().attrAt(alist[i_a]).at().id();
			if( i_w >= 0 )
			{
			    idprm.insert(0,incllist[i_w]+".");
			    nprm.insert(0,wdg.at().id()+".");
			}
			
			if( !(wdg.at().attrAt(alist[i_a]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst)) ||
			    (!shwAttr && wdg.at().attrAt(alist[i_a]).at().flgSelf()&Attr::CfgConst) ) continue;
			//-- Get attributes --
			bool shwTmpl = wdg.at().attrAt(alist[i_a]).at().cfgTempl().size();
			if( shwTmpl )	grpprm = TSYS::strSepParse(wdg.at().attrAt(alist[i_a]).at().cfgTempl(),0,'|');
			
                	//-- Check select param --
                	if( shwTmpl && !shwAttr )
                	{
                    	    nprm = grpprm;
			    if( i_w >= 0 ) nprm.insert(0,wdg.at().id()+".");
			    
	            	    //-- Check already to present parameters --
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
    
    //- Process command to page -
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
    	vector<string> a_ls;
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if( nattr.size() )	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');	
	
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    string cfg_val = srcwdg.at().attrAt(nattr).at().cfgVal();
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,':')+":";
	    string rez     = _("Custom");	    

	    int c_lvl = 0;
	    bool custom = false;	    
	    if( obj_tp == "prm:" || obj_tp == "wdg:" )
	    {		
	        for( int c_off = obj_tp.size(); TSYS::pathLev(cfg_val,0,true,&c_off).size(); c_lvl++ );
		if( (obj_tp == "prm:" && c_lvl==4) || (obj_tp == "wdg:" && c_lvl==2) )
		    cfg_val.resize(cfg_val.rfind("/"));
	    }else custom = true;
		
	    string sel;
	    srcwdg.at().attrList(a_ls);
	    rez += ": ";
	    for( int i_a = 0; i_a < a_ls.size(); i_a++ )
	        if( p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') && !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst) )
	        {
	    	    sel = srcwdg.at().attrAt(a_ls[i_a]).at().cfgVal();
		    if( !custom && sel.find(cfg_val) != 0 ) custom = true;
		    rez += sel+", ";
		}
	    if( cfg_val.empty() )	rez = "";
	    else if( !custom ) 		rez = cfg_val;
	    
	    opt->setText(rez);	    
	}
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    bool noonly_no_set = true;
	    string no_set;
	    string cfg_val = opt->text();
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,':')+":";
	    string cfg_addr = (obj_tp.size()<cfg_val.size()) ? cfg_val.substr(obj_tp.size()) : "";

	    int c_lvl = 0;
	    for( int c_off = 0; TSYS::pathLev(cfg_addr,0,true,&c_off).size(); c_lvl++ ) ;

	    AutoHD<TValue> prm;
	    if( obj_tp == "prm:" && c_lvl==3 )	prm = SYS->daq().at().nodeAt(cfg_addr);		
	    AutoHD<Widget> dstwdg;
	    if( obj_tp == "wdg:" && c_lvl==1 )
	    {
		string nwdg = TSYS::pathLev(cfg_addr,0);
		if( nwdg=="self" ) dstwdg = this;
		else	dstwdg = wdgAt(nwdg);
	    }

	    srcwdg.at().attrList(a_ls);
	    for( int i_a = 0; i_a < a_ls.size(); i_a++ )
		if( p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') && !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst) )
		{
		    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val);
		    string p_attr = TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),1,'|');
		    if(!prm.freeStat() || !dstwdg.freeStat())
		    {
			if( (!prm.freeStat() && prm.at().vlPresent(p_attr)) ||
			    (!dstwdg.freeStat() && dstwdg.at().attrPresent(p_attr)) )
		    	{
			    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val+"/"+p_attr);
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

	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");	
	if( !(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut)) )
	{
	    if( !is_pl ) throw TError(nodePath().c_str(),_("Variable is not link"));
	    vector<string> a_ls;
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    srcwdg.at().attrList(a_ls);	
	    int i_a;
	    for( i_a = 0; i_a < a_ls.size(); i_a++ )
	        if( p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') && !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst) )		    
	        { nattr = a_ls[i_a]; break; }
	    if( i_a >= a_ls.size() ) throw TError(nodePath().c_str(),_("Variable is not link"));
	}

	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();
	//-- Link interface process --
        int c_lv = 0;
	string obj_tp = TSYS::strSepParse(m_prm,0,':')+":";
	if( obj_tp.empty() || !(obj_tp == "val:" || obj_tp == "prm:" || obj_tp == "wdg:" || obj_tp == "addr:") )
	{
	    if( !is_pl )
	    {
		opt->childAdd("el")->setText(_("val:Constant value"));
		opt->childAdd("el")->setText("addr:");
	    }
	    opt->childAdd("el")->setText("prm:");
	    opt->childAdd("el")->setText("wdg:");
	}
	//-- Link elements process --
	else
	{
	    int c_off = obj_tp.size();
	    string c_path = obj_tp, c_el;
	    opt->childAdd("el")->setText("");
	    opt->childAdd("el")->setText(c_path);
	    for( ; (c_el=TSYS::pathLev(m_prm,0,true,&c_off)).size(); c_lv++ )
	    {
		if( is_pl && ((obj_tp=="prm:" && c_lv>=3) || (obj_tp=="wdg:" && c_lv>=1)) ) break;
		c_path += "/"+c_el;
		opt->childAdd("el")->setText(c_path);
	    }
	    vector<string> ls;
	    c_off = obj_tp.size();
	    //--- Address interface elements list process ---
	    if( obj_tp == "addr:" && !is_pl )
		SYS->nodeAt(m_prm,0,0,c_off).at().nodeList(ls);
	    //--- Other interface elements list process ---
	    else
	    {
		string prm1 = TSYS::pathLev(m_prm,0,true,&c_off);
		string prm2 = TSYS::pathLev(m_prm,0,true,&c_off);
		string prm3 = TSYS::pathLev(m_prm,0,true,&c_off);
		switch(c_lv)
		{
		    case 0:
			if( obj_tp == "prm:" ) SYS->daq().at().modList(ls);
			if( obj_tp == "wdg:" )
			{
			    wdgList(ls);
			    ls.push_back("self");
			}
			break;
		    case 1:
			if( obj_tp == "prm:" && SYS->daq().at().modPresent(prm1) )
			    SYS->daq().at().at(prm1).at().list(ls);
			if( !is_pl && obj_tp == "wdg:" )
			{
			    AutoHD<Widget> wdg;
			    if( prm1 == "self")	wdg = this;
                	    else wdg = wdgAt(prm1);
			    if( !wdg.freeStat() ) wdg.at().attrList(ls);
			}
			break;
		    case 2:
			if( obj_tp == "prm:" && SYS->daq().at().modPresent(prm1)
			          && SYS->daq().at().at(prm1).at().present(prm2) )
			    SYS->daq().at().at(prm1).at().at(prm2).at().list(ls);
			break;
		    case 3:
 			if( !is_pl && obj_tp=="prm:" && SYS->daq().at().modPresent(prm1)
			          && SYS->daq().at().at(prm1).at().present(prm2)
			          && SYS->daq().at().at(prm1).at().at(prm2).at().present(prm3) )
			SYS->daq().at().at(prm1).at().at(prm2).at().at(prm3).at().vlList(ls);
		    break;
		}
	    }
	    for(int i_l = 0; i_l < ls.size(); i_l++)
		opt->childAdd("el")->setText(c_path+"/"+ls[i_l]);
	}
    }
    else if( a_path.substr(0,14) == "/links/lnk/el_" )
    {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if( nattr.size() )	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	    opt->setText(srcwdg.at().attrAt(nattr).at().cfgVal());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    srcwdg.at().attrAt(nattr).at().setCfgVal(opt->text());	
	    if( srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst )
		srcwdg.at().attrAt(nattr).at().setS(opt->text());
	}
    }
    else return false;

    return true;
}

bool Widget::cntrCmdProcess( XMLNode *opt )
{
    string wattr;
    
    if( isLink() ) return false;
    
    //- Get page info -
    if( opt->name() == "info" )
    { 
 	if(ctrMkNode("area",opt,-1,"/proc",_("Process")))
	{
	    ctrMkNode("fld",opt,-1,"/proc/wdg",_("Widget"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/proc/w_lst");
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if( !wdgPresent(wattr) )	wattr = ".";
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
		ctrMkNode("fld",opt,-1,"/proc/calc/per",_("Programm calc period (ms)"),permit(),user().c_str(),grp().c_str(),1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/proc/calc/prog",_("Programm"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","90","rows","8");
	    }
	}
	return true;
    }
    
    //- Process command to page -
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
        opt->childAdd("el")->setText(".");	
        for( unsigned i_f=0; i_f < lst.size(); i_f++ )
            opt->childAdd("el")->setText(lst[i_f]);
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

	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    vector<string> lst;	    
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
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
            wdg.at().attrAdd( new TFld("newAttr",_("New attribute"),TFld::String,Attr::IsUser) );
	    wdg.at().attrAt("newAttr").at().setS(EVAL_STR);
            wdg.at().attrAt("newAttr").at().setModif(1);
        }
        if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
            if( !wdg.at().attrPresent(opt->attr("key_id")) )
                throw TError(nodePath().c_str(),_("Delete a include widget's element is error."));
            if( !(wdg.at().attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsInher) && 
		    wdg.at().attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsUser )
		wdg.at().attrDel(opt->attr("key_id"));
            else throw TError(nodePath().c_str(),_("Delete a not user element is error."));
        }
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            //-- Request data --
            string idattr = opt->attr("key_id");
            string idcol  = opt->attr("col");
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);

	    if( idcol == "id" || idcol == "name" || idcol == "type" || idcol == "wa" )
	    {
                string          tnm     = wdg.at().attrAt(idattr).at().name();
                TFld::Type      ttp     = wdg.at().attrAt(idattr).at().fld().type();
                unsigned        tflg    = wdg.at().attrAt(idattr).at().fld().flg();

                if( !(!(tflg&Attr::IsInher) && tflg&Attr::IsUser) )
            	    throw TError(nodePath().c_str(),_("Change a no user attribute is no permit"));
                if( idcol == "id" )
                {
                    wdg.at().attrAdd( new TFld(opt->text().c_str(),tnm.c_str(),ttp,tflg|Attr::IsUser) );
            	    wdg.at().attrAt(opt->text()).at().setModif(1);
		    wdg.at().attrAt(opt->text()).at().setS(wdg.at().attrAt(idattr).at().getS());
		    wdg.at().attrAt(opt->text()).at().setFlgSelf(wdg.at().attrAt(idattr).at().flgSelf());
		    wdg.at().attrAt(opt->text()).at().setCfgTempl(wdg.at().attrAt(idattr).at().cfgTempl());
		    wdg.at().attrAt(opt->text()).at().setCfgVal(wdg.at().attrAt(idattr).at().cfgVal());
                    wdg.at().attrDel(idattr);
            	}
            	else if( idcol == "name" )      wdg.at().attrAt(idattr).at().fld().setDescr(opt->text().c_str());
                else if( idcol == "type" )
                {
                    TFld::Type  tp  = (TFld::Type)(atoi(opt->text().c_str())&0x0f);
            	    unsigned    flg = (atoi(opt->text().c_str())>>4)|Attr::IsUser;

	            if( tp!=ttp || (tflg^flg)&TFld::Selected )
	            {
			string tvl  	= wdg.at().attrAt(idattr).at().getS();
			Attr::SelfAttrFlgs sflgs = wdg.at().attrAt(idattr).at().flgSelf();
			string tmpl   	= wdg.at().attrAt(idattr).at().cfgTempl();
			string cfgval 	= wdg.at().attrAt(idattr).at().cfgVal();
	                wdg.at().attrDel(idattr);
	                wdg.at().attrAdd( new TFld(idattr.c_str(),tnm.c_str(),tp,tflg^((tflg^flg)&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address|Attr::IsUser))) );
			wdg.at().attrAt(idattr).at().setS(tvl);
			wdg.at().attrAt(idattr).at().setFlgSelf(sflgs);
			wdg.at().attrAt(idattr).at().setCfgVal(cfgval);
			wdg.at().attrAt(idattr).at().setCfgTempl(tmpl);			
	                wdg.at().attrAt(idattr).at().setModif(1);
	            }
	            else if( (tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address) )
	                wdg.at().attrAt(idattr).at().fld().setFlg(tflg^((tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address)));
	        }
		else if( idcol == "wa" )
		{
		    wdg.at().attrAt(idattr).at().fld().setValues(TSYS::strSepParse(opt->text(),0,'|'));
		    wdg.at().attrAt(idattr).at().fld().setSelNames(TSYS::strSepParse(opt->text(),1,'|'));
		}
	    }
	    else
	    {
		if( idcol == "proc" )
		{
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = atoi(opt->text().c_str())?Attr::ProcAttr:(Attr::SelfAttrFlgs)0;
		    wdg.at().attrAt(idattr).at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::ProcAttr)) );
		}
		else if( idcol == "cfg" )
        	{
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();		
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)atoi(opt->text().c_str());
		    if( (sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst) )
			wdg.at().attrAt(idattr).at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst))) );
		}
		else if( idcol == "cfgtmpl" )	wdg.at().attrAt(idattr).at().setCfgTempl(opt->text());
	    }
	}
    }
    else if( a_path == "/proc/calc/progLng" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(calcLang());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) setCalcLang(opt->text());
    }
    else if( a_path == "/proc/calc/per" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) 
	    opt->setText(TSYS::int2str(calcPer()));
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    setCalcPer(atoi(opt->text().c_str()));
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
        opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Integer+(Attr::DataTime<<4)))->setText(_("DataTime"));
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
//* Attr: Widget atribute                        *
//************************************************
Attr::Attr( TFld &ifld ) : m_modif(0), self_flg((SelfAttrFlgs)0)
{
    //- Chek for self field for dinamic elements -
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

Widget *Attr::owner()
{ 
    return (Widget *)nodePrev(); 
}

int Attr::flgGlob()
{
    return fld().flg();
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
        case TFld::Integer:	return (m_val.i_val!=EVAL_INT) ? TSYS::int2str(m_val.i_val) : EVAL_STR;
        case TFld::Real:        return (m_val.r_val!=EVAL_REAL) ? TSYS::real2str(m_val.r_val) : EVAL_STR;
        case TFld::Boolean:	return (m_val.b_val!=EVAL_BOOL) ? TSYS::int2str((bool)m_val.b_val) : EVAL_STR;
	case TFld::String:      return *m_val.s_val;
    }
}

int Attr::getI( )
{
    switch(fld().type())
    {
	case TFld::String:      return ((*m_val.s_val)!=EVAL_STR) ? atoi(m_val.s_val->c_str()) : EVAL_INT;
        case TFld::Real:        return (m_val.r_val!=EVAL_REAL) ? (int)m_val.r_val : EVAL_INT;
        case TFld::Boolean:	return (m_val.b_val!=EVAL_BOOL) ? (bool)m_val.b_val : EVAL_INT;
        case TFld::Integer:	return m_val.i_val;
    }
}

double Attr::getR( )
{
    switch(fld().type())
    {
        case TFld::String:      return ((*m_val.s_val)!=EVAL_STR) ? atof(m_val.s_val->c_str()) : EVAL_REAL;
        case TFld::Integer:	return (m_val.i_val!=EVAL_INT) ? m_val.i_val : EVAL_REAL;
        case TFld::Boolean:	return (m_val.b_val!=EVAL_BOOL) ? (bool)m_val.b_val : EVAL_REAL;
        case TFld::Real:        return m_val.r_val;
    }
}

char Attr::getB( )
{
    switch(fld().type())
    {
        case TFld::String:      return ((*m_val.s_val)!=EVAL_STR) ? (bool)atoi(m_val.s_val->c_str()) : EVAL_BOOL;
        case TFld::Integer:	return (m_val.i_val!=EVAL_INT) ? (bool)m_val.i_val : EVAL_BOOL;
        case TFld::Real:        return (m_val.r_val!=EVAL_REAL) ? (bool)m_val.r_val : EVAL_BOOL;
        case TFld::Boolean:	return m_val.b_val;
    }
}

void Attr::setSEL( const string &val, bool strongPrev )
{
    if( !(fld().flg()&TFld::Selected) )
        throw TError("Cfg",_("Element type no select!"));
    switch( fld().type() )
    {
	case TFld::String:      setS( fld().selNm2VlS(val), strongPrev );  break;
        case TFld::Integer:	setI( fld().selNm2VlI(val), strongPrev );  break;
        case TFld::Real:        setR( fld().selNm2VlR(val), strongPrev );  break;
        case TFld::Boolean:	setB( fld().selNm2VlB(val), strongPrev );  break;
    }
}

void Attr::setS( const string &val, bool strongPrev )
{
    switch( fld().type() )
    {
        case TFld::Integer:	setI( (val!=EVAL_STR) ? atoi(val.c_str()) : EVAL_INT, strongPrev );	break;
	case TFld::Real:	setR( (val!=EVAL_STR) ? atof(val.c_str()) : EVAL_REAL, strongPrev );	break;
        case TFld::Boolean:	setB( (val!=EVAL_STR) ? (bool)atoi(val.c_str()) : EVAL_BOOL, strongPrev );	break;    
	case TFld::String:
	{
	    if( !strongPrev && *(m_val.s_val) == val )	break;	    
            string t_str = *(m_val.s_val);
    	    *(m_val.s_val) = val;
            if( !owner()->attrChange(*this,&t_str) )	    
                *(m_val.s_val) = t_str;
	    else 
	    {	
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
            break;
	}
    }
}

void Attr::setI( int val, bool strongPrev )
{
    switch( fld().type() )
    {
        case TFld::String:      setS( (val!=EVAL_INT) ? TSYS::int2str(val) : EVAL_STR, strongPrev );	break;
        case TFld::Real:        setR( (val!=EVAL_INT) ? val : EVAL_REAL, strongPrev );    break;
        case TFld::Boolean:	setB( (val!=EVAL_INT) ? (bool)val : EVAL_BOOL, strongPrev );    break;
        case TFld::Integer:
	{
            if( !(fld().flg()&TFld::Selected) && fld().selValI()[0] < fld().selValI()[1] )
            {
    		val = vmax(val,m_fld->selValI()[0]);
		val = vmin(val,m_fld->selValI()[1]);
            }
	    if( !strongPrev && m_val.i_val == val )	break;
            int t_val = m_val.i_val;
            m_val.i_val = val;
            if( !owner()->attrChange(*this,&t_val) )
                m_val.i_val = t_val;
	    else 
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
            break;
	}
    }
}

void Attr::setR( double val, bool strongPrev )
{
    switch( fld().type() )
    {
        case TFld::String:      setS( (val!=EVAL_REAL) ? TSYS::real2str(val) : EVAL_STR, strongPrev );	break;
        case TFld::Integer:	setI( (val!=EVAL_REAL) ? (int)val : EVAL_INT, strongPrev );	break;
	case TFld::Boolean:	setB( (val!=EVAL_REAL) ? (bool)val : EVAL_BOOL, strongPrev );    break;
        case TFld::Real:
	{
    	    if( !(fld().flg()&TFld::Selected) && fld().selValR()[0] < fld().selValR()[1] )
    	    {
                val = vmax(val,fld().selValR()[0]);
                val = vmin(val,fld().selValR()[1]);
            }
	    if( !strongPrev && m_val.r_val == val )	break;
            double t_val = m_val.r_val;
            m_val.r_val = val;
            if( !owner()->attrChange(*this,&t_val) )
                m_val.r_val = t_val;
	    else
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
            break;
	}
    }
}

void Attr::setB( char val, bool strongPrev )
{
    switch( fld().type() )
    {
        case TFld::String:      setS( (val!=EVAL_BOOL) ? TSYS::int2str((bool)val) : EVAL_STR, strongPrev );	break;
        case TFld::Integer:    	setI( (val!=EVAL_BOOL) ? (bool)val : EVAL_INT, strongPrev );	break;
        case TFld::Real:        setR( (val!=EVAL_BOOL) ? (bool)val : EVAL_REAL, strongPrev );	break;
        case TFld::Boolean:
	{
	    if( !strongPrev && m_val.b_val == val )	break;
            bool t_val = m_val.b_val;
            m_val.b_val = val;
            if( !owner()->attrChange(*this,&t_val) )
                m_val.b_val = t_val;
	    else 
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
	}
        break;
    }
}

void Attr::setCfgTempl( const string &vl )
{
    if( cfg_tmpl == vl ) return;
    string t_tmpl = cfg_tmpl;
    cfg_tmpl = vl;
    if( !owner()->attrChange(*this,NULL) )	cfg_tmpl = t_tmpl;
    else
    {
	unsigned imdf = owner()->modifVal(*this);
	m_modif = imdf ? imdf : m_modif+1;
    }
}

void Attr::setCfgVal( const string &vl )
{
    if( cfg_val == vl ) return;
    string t_val = cfg_val;
    cfg_val = vl;
    if( !owner()->attrChange(*this,NULL) )	cfg_val = t_val;
    else
    {
	unsigned imdf = owner()->modifVal(*this);
	m_modif = imdf ? imdf : m_modif+1;
    }
}

void Attr::setFlgSelf( SelfAttrFlgs flg )
{
    if( self_flg == flg )	return;
    SelfAttrFlgs t_flg = self_flg;
    self_flg = flg;
    if( !owner()->attrChange(*this,NULL) )	self_flg = t_flg;
    else
    {
	unsigned imdf = owner()->modifVal(*this);
	m_modif = imdf ? imdf : m_modif+1;
    }
}

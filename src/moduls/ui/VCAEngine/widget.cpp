
//OpenSCADA system module UI.VCAEngine file: widget.cpp
/***************************************************************************
 *   Copyright (C) 2006-2009 by Roman Savochenko                           *
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

#include <errno.h>

#include <resalloc.h>
#include <tsys.h>

#include "vcaengine.h"
#include "widget.h"

using namespace VCA;


//************************************************
//* Widget                                       *
//************************************************
Widget::Widget( const string &id, const string &isrcwdg ) :
    mId(id), mEnable(false), m_lnk(false), mStlLock(false), mParentNm(isrcwdg), attrAtLockCnt(0)
{
    inclWdg = grpAdd("wdg_");

    //> Attributes mutex create
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mtxAttr, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

Widget::~Widget()
{
    //> Remove attributes
    pthread_mutex_lock(&mtxAttr);
    map<string,Attr*>::iterator p;
    while((p = mAttrs.begin()) != mAttrs.end())
    {
	delete p->second;
	mAttrs.erase(p);
    }
    pthread_mutex_unlock(&mtxAttr);

    //> Attributes mutex destroy
    pthread_mutex_destroy(&mtxAttr);
}

TCntrNode &Widget::operator=( TCntrNode &node )
{
    Widget *src_n = dynamic_cast<Widget*>(&node);
    if( !src_n ) return *this;

    if( !src_n->enable() ) return *this;

    //> Parent link copy
    if( src_n->parentNm() != path() )
    {
	if( parentNm() != src_n->parentNm() && enable() ) setEnable(false);
	setParentNm( src_n->parentNm() );
    }
    if( !enable() ) setEnable(true);

    //> Copy generic configuration
    if( src_n->parent().freeStat() || src_n->name() != src_n->parent().at().name() )	setName(src_n->name());
    if( src_n->parent().freeStat() || src_n->descr() != src_n->parent().at().descr() )	setDescr(src_n->descr());
    if( src_n->parent().freeStat() || src_n->ico() != src_n->parent().at().ico() )	setIco(src_n->ico());
    setOwner( src_n->owner() );
    setGrp( src_n->grp() );
    setPermit( src_n->permit() );
    if( src_n->parent().freeStat() || src_n->calcLang() != src_n->parent().at().calcLang() )	setCalcLang(src_n->calcLang());
    if( src_n->parent().freeStat() || src_n->calcProg() != src_n->parent().at().calcProg() )	setCalcProg(src_n->calcProg());
    if( src_n->parent().freeStat() || src_n->calcPer() != src_n->parent().at().calcPer() )	setCalcPer(src_n->calcPer());

    //> Copy attributes
    vector<string> els;
    src_n->attrList( els );
    AutoHD<Attr> attr, pattr;
    for( int i_a = 0; i_a < els.size(); i_a++ )
    {
	pattr = src_n->attrAt(els[i_a]);
	if( !attrPresent(els[i_a]) )
	{
	    bool isInher = pattr.at().flgSelf()&Attr::IsInher;
	    attrAdd( isInher ? &src_n->attrAt(els[i_a]).at().fld() : new TFld(src_n->attrAt(els[i_a]).at().fld()), -1, isInher );
	    attrAt(els[i_a]).at().setModif(1);
	}
	attr  = attrAt(els[i_a]);
	if( attr.at().flgGlob( )&Attr::DirRead ) continue;
	attr.at().setFlgSelf(pattr.at().flgSelf());
	switch( attr.at().type() )
	{
	    case TFld::Boolean:	attr.at().setB(pattr.at().getB());	break;
	    case TFld::Integer:	attr.at().setI(pattr.at().getI());	break;
	    case TFld::Real:	attr.at().setR(pattr.at().getR());	break;
	    case TFld::String:	attr.at().setS(pattr.at().getS());	break;
	}
	attr.at().setCfgTempl( pattr.at().cfgTempl() );
	attr.at().setCfgVal( pattr.at().cfgVal() );
    }

    //> Include widgets copy
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

void Widget::postEnable( int flag )
{
    if( flag&TCntrNode::NodeRestore )	setEnable(true);
    if( flag&TCntrNode::NodeConnect )
    {
	//> Add main attributes
	attrAdd( new TFld("id",_("Id"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic) );
	attrAdd( new TFld("path",_("Path"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic) );
	attrAdd( new TFld("parent",_("Parent"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic) );
	attrAdd( new TFld("owner",_("Owner"),TFld::String,TFld::NoStrTransl|Attr::Generic,"","root:UI") );
	attrAdd( new TFld("perm",_("Access"),TFld::Integer,TFld::OctDec|TFld::Selected|Attr::Generic,"","0664",
	    "0;0400;0440;0444;0600;0640;0644;0660;0664;0666",
	    _("No access;R_____;R_R___;R_R_R_;RW____;RWR___;RWR_R_;RWRW__;RWRWR_;RWRWRW")) );
	attrAdd( new TFld("root",_("Root"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic,"","","","","1") );
	attrAdd( new TFld("name",_("Name"),TFld::String,Attr::Generic) );
	attrAdd( new TFld("dscr",_("Description"),TFld::String,TFld::FullText|Attr::Generic) );
	attrAdd( new TFld("en",_("Enabled"),TFld::Boolean,Attr::Generic,"","1","","","5") );
	attrAdd( new TFld("active",_("Active"),TFld::Boolean,Attr::Active,"","0","","","6") );
	attrAdd( new TFld("geomX",_("Geometry:x"),TFld::Real,Attr::Generic,"","0","0;10000","","7") );
	attrAdd( new TFld("geomY",_("Geometry:y"),TFld::Real,Attr::Generic,"","0","0;10000","","8") );
	attrAdd( new TFld("geomW",_("Geometry:width"),TFld::Real,Attr::Generic,"","100","0;10000","","9") );
	attrAdd( new TFld("geomH",_("Geometry:height"),TFld::Real,Attr::Generic,"","100","0;10000","","10") );
	attrAdd( new TFld("geomXsc",_("Geometry:x scale"),TFld::Real,Attr::Generic,"","1","0.1;100","","13") );
	attrAdd( new TFld("geomYsc",_("Geometry:y scale"),TFld::Real,Attr::Generic,"","1","0.1;100","","14") );
	attrAdd( new TFld("geomZ",_("Geometry:z"),TFld::Integer,Attr::Generic,"","0","0;1000000","","11") );
	attrAdd( new TFld("geomMargin",_("Geometry:margin"),TFld::Integer,Attr::Generic,"","0","0;1000","","12") );
	attrAdd( new TFld("tipTool",_("Tip:tool"),TFld::String,Attr::Generic,"","","","","15") );
	attrAdd( new TFld("tipStatus",_("Tip:status"),TFld::String,Attr::Generic,"","","","","16") );
	attrAdd( new TFld("contextMenu",_("Context menu"),TFld::String,TFld::FullText|Attr::Generic,"","","","","17") );
	attrAdd( new TFld("evProc",_("Events process"),TFld::String,TFld::FullText,"200") );
    }
}

void Widget::preDisable( int flag )
{
    //> Delete heritors widgets
    while( herit().size() )	mod->nodeDel( herit()[0].at().path(), 0, 0x10, true );

    //> Disable widget
    if( enable() )  setEnable(false);
}

string Widget::rootId( )
{
    if( !parent().freeStat() )  return parent().at().rootId();
    return "";
}

string Widget::name( )
{
    return (attrAt("name").at().getS().size()) ? attrAt("name").at().getS() : mId;
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

string Widget::owner( )
{
    return TSYS::strParse(attrAt("owner").at().getS(),0,":");
}

void Widget::setOwner( const string &iown )
{
    attrAt("owner").at().setS(iown+":"+grp());
    //> Group update
    if(SYS->security().at().grpAt("UI").at().user(iown)) setGrp("UI");
    else
    {
	vector<string> gls;
	SYS->security().at().usrGrpList(owner(),gls);
	setGrp(gls.size()?gls[0]:Widget::grp());
    }
}

string Widget::grp( )
{
    return TSYS::strParse(attrAt("owner").at().getS(),1,":");
}

void Widget::setGrp( const string &igrp )
{
    attrAt("owner").at().setS(owner()+":"+igrp);
}

short Widget::permit( )
{
    return attrAt("perm").at().getI();
}

void Widget::setPermit( short iperm )
{
    attrAt("perm").at().setI(iperm);
}

bool Widget::isContainer( )
{
    if( !parent().freeStat() )    return parent().at().isContainer();
    return false;
}

string Widget::path( )
{
    Widget *ownW = dynamic_cast<Widget*>(nodePrev());
    if( ownW )	return ownW->path()+"/wdg_"+mId;

    return mId;
}

string Widget::calcId( )
{
    Widget *ownW = dynamic_cast<Widget*>(nodePrev());
    if( ownW )	return ownW->calcId()+"_"+mId;

    return mId;
}

bool Widget::enable( )
{
    return mEnable;
}

void Widget::setEnable( bool val )
{
    if( enable() == val ) return;

    if( val )
    {
	if( parentNm() != "root" )
	{
	    try
	    {
		if( TSYS::strNoSpace(parentNm()).empty() || parentNm() == path() )
		    throw TError(nodePath().c_str(),"%s",_("Empty parent or parent identical self!"));
		if( parentNm() == ".." ) mParent = AutoHD<TCntrNode>(nodePrev());
		else mParent = mod->nodeAt(parentNm());

		//> Check for enable parent widget and enable if not
		if( !parent().at().enable() )	parent().at().setEnable(true);

		//> Inherit
		inheritAttr( );
		inheritIncl( );

		//> Register of heritater
		parent().at().heritReg(this);
	    }catch(TError err)
	    {
		mess_err(nodePath().c_str(),_("Widget enable error: %s"),err.mess.c_str());
		mParent.free();
		throw;
	    }
	}
	mEnable = true;

	//> Load self values from DB
	loadIO( );
    }
    if( !val )
    {
	//> Free no base attributes and restore base
	vector<string>  ls;
	attrList(ls);
	for( int i_l = 0; i_l < ls.size(); i_l++ )
	{
	    if( !(attrAt(ls[i_l]).at().flgGlob()&Attr::Generic) ) attrDel( ls[i_l], true );
	    //else if( attrAt(ls[i_l]).at().flgSelf( )&Attr::IsInher )
	    //	attrAt(ls[i_l]).at().setFld( new TFld(attrAt(ls[i_l]).at().fld()), false );
	}

	//> Disable heritors widgets
	for( int i_h = 0; i_h < herit().size(); i_h++ )
	    if( herit()[i_h].at().enable( ) )
		try { herit()[i_h].at().setEnable(false); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Ingeriting widget <%s> disable error"),herit()[i_h].at().id().c_str());
		}

	if( !mParent.freeStat() )
	{
	    //> Unregister heritater
	    parent().at().heritUnreg(this);
	    //> Disconnect parent widget
	    mParent.free();
	}
    }

    //> Enable/disable process widgets from container
    vector<string> ls;
    wdgList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
	if( val != wdgAt(ls[i_l]).at().enable() )
	    try { wdgAt(ls[i_l]).at().setEnable(val); }
	    catch( TError err )
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Child widget <%s> enable/disable error"),ls[i_l].c_str());
	    }

    mEnable = val;
}

void Widget::setParentNm( const string &isw )
{
    if( enable() && mParentNm != isw ) setEnable(false);
    mParentNm = isw;
}

AutoHD<Widget> Widget::parent( )
{
    return mParent;
}

AutoHD<Widget> Widget::parentNoLink( )
{
    if( parent().at().isLink() ) return parent().at().parentNoLink( );
    return parent();
}

void Widget::heritReg( Widget *wdg )
{
    //> Search already herited widget
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( &m_herit[i_h].at() == wdg )	return;
    m_herit.push_back(AutoHD<Widget>(wdg));
}

void Widget::heritUnreg( Widget *wdg )
{
    //> Search herited widget
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( &m_herit[i_h].at() == wdg )	
	{
	    m_herit.erase(m_herit.begin()+i_h);
	    return;
	}
}

void Widget::inheritAttr( const string &iattr )
{
    bool loadDef = parent().freeStat();

    //> Create no present attributes
    vector<string>  ls;
    if( iattr.empty() )	(loadDef ? attrList(ls) : parent().at().attrList(ls));
    else
    {
	if( !loadDef && parent().at().attrPresent(iattr) )	ls.push_back(iattr);
	else if( attrPresent(iattr) )	{ loadDef = true; ls.push_back(iattr); }
	else return;
    }

    setStlLock(true);
    if( !loadDef ) parent().at().setStlLock(true);

    //> Configuration inherit
    AutoHD<Attr> attr, pattr;
    for( int i_l = 0; i_l < ls.size(); i_l++ )
    {
	if( !attrPresent(ls[i_l]) )
	{
	    if( loadDef ) continue;
	    if( parent().at().attrAt(ls[i_l]).at().flgGlob()&Attr::Mutable )	continue;
	    attrAdd( &parent().at().attrAt(ls[i_l]).at().fld(), -1, true );
	}
	attr  = attrAt(ls[i_l]);
	if( loadDef )
	{
	    attr.at().setS( attr.at().fld().def(), attr.at().flgGlob()&Attr::Active );
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)0);
	    attr.at().setCfgTempl("");
	    attr.at().setCfgVal("");
	    attr.at().setModif(0);
	    continue;
	}
	pattr = parent().at().attrAt(ls[i_l]);
	if( !(attr.at().flgSelf()&Attr::IsInher) /*&& !(attr.at().flgGlob()&Attr::Mutable)*/ ) attr.at().setFld(&pattr.at().fld(),true);
	if( attr.at().modif() && !(attr.at().flgSelf()&Attr::SessAttrInh) )	continue;
	attr.at().setFlgSelf( (Attr::SelfAttrFlgs)pattr.at().flgSelf() );
	if( !(attr.at().flgGlob( )&Attr::DirRead) )
	{
	    bool active = attr.at().flgGlob()&Attr::Active;
	    switch( attr.at().type() )
	    {
		case TFld::Boolean:	attr.at().setB( pattr.at().getB(), active );	break;
		case TFld::Integer:	attr.at().setI( pattr.at().getI(), active );	break;
		case TFld::Real:	attr.at().setR( pattr.at().getR(), active );	break;
		case TFld::String:	attr.at().setS( pattr.at().getS(), active );	break;
	    }
	}
	//>> No inherit calc flag for links
	if( isLink() && !parent().at().isLink() )
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)(attr.at().flgSelf()&(~Attr::ProcAttr)));
	attr.at().setCfgTempl(pattr.at().cfgTempl());
	attr.at().setCfgVal(pattr.at().cfgVal());
	attr.at().setModif(0);
    }

    setStlLock(false);
    if( !loadDef ) parent().at().setStlLock(false);
}

void Widget::inheritIncl( const string &iwdg )
{
    AutoHD<Widget> parw = parent();
    while( !parw.freeStat() && parw.at().isLink() )
	parw = parw.at().parent();
    if( parw.freeStat() ) return;

    //> Create no present include widgets for no link and container widgets
    vector<string>  ls;
    if( !iwdg.empty() && parw.at().wdgPresent(iwdg) )
	ls.push_back(iwdg);
    else parw.at().wdgList(ls);
    for( int i_w = 0; i_w < ls.size(); i_w++ )
	if( !wdgPresent(ls[i_w]) )
	    try{ wdgAdd(ls[i_w],"",parw.at().wdgAt(ls[i_w]).at().path()); }
	    catch(TError err){ mess_err(err.cat.c_str(),err.mess.c_str()); }
}

void Widget::wClear( )
{
    //> Generic clear
    setIco("");

    //> Inherit modify attributes
    vector<string> ls;
    attrList( ls );
    for( int i_a = 0; i_a < ls.size(); i_a++ )
    {
	if( !attrPresent(ls[i_a]) ) continue;
	AutoHD<Attr> attr = attrAt(ls[i_a]);
	if( attr.at().modif() )
	{
	    attr.at().setModif(0);
	    inheritAttr(ls[i_a]);
	}
    }
    //> Check for included widgets
    if( isContainer( ) && !isLink( ) )
    {
	AutoHD<Widget> parw = parent();
	while( !parw.freeStat() && parw.at().isLink() )
	    parw = parw.at().parent();
	if( !parw.freeStat() )
	{
	    //> Check for widget's deletion
	    wdgList(ls);
	    for( int i_w = 0; i_w < ls.size(); i_w++ )
		if( !parw.at().wdgPresent(ls[i_w]) )
		    wdgDel(ls[i_w],true);

	    //> No present widget's add and clear call
	    parw.at().wdgList(ls);
	    for( int i_w = 0; i_w < ls.size(); i_w++ )
		if( !wdgPresent(ls[i_w]) )
		    try{ wdgAdd(ls[i_w],"",parw.at().wdgAt(ls[i_w]).at().path(),true); }
		    catch(TError err){ mess_err(err.cat.c_str(),err.mess.c_str()); }
		else wdgAt(ls[i_w]).at().wClear();
	}
    }
    modif();
}

void Widget::attrList( vector<string> &list )
{
    pthread_mutex_lock(&mtxAttr);
    list.clear();
    list.reserve(mAttrs.size());
    for( map<string, Attr* >::iterator p = mAttrs.begin(); p != mAttrs.end(); ++p )
    {
	while( p->second->mOi >= list.size() )	list.push_back("");
	list[p->second->mOi] = p->first;
    }
    pthread_mutex_unlock(&mtxAttr);
}

void Widget::attrAdd(TFld *attr, int pos, bool inher)
{
    string anm = attr->name();

    if(attrPresent(anm) || TSYS::strNoSpace(anm).empty())
    {
	if( !inher ) delete attr;
	return;
    }
    try
    {
	pthread_mutex_lock(&mtxAttr);
	map<string, Attr* >::iterator p;
	Attr *a = new Attr(attr,inher);
	a->mOwner = this;
	pos = (pos < 0 || pos > mAttrs.size()) ? mAttrs.size() : pos;
	a->mOi = pos;
	for(p = mAttrs.begin(); p != mAttrs.end(); p++)
	    if( p->second->mOi >= pos ) p->second->mOi++;
	mAttrs.insert(std::pair<string,Attr*>(a->id(),a));
    }
    catch(...){ }
    pthread_mutex_unlock(&mtxAttr);
}

void Widget::attrDel( const string &attr, bool allInher  )
{
    if( !attrPresent(attr) )	return;

    //> Delete from inheritant wigets
    if( !(attrAt(attr).at().flgGlob()&Attr::Mutable) || allInher )
	for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	    if( m_herit[i_h].at().enable( ) )
		m_herit[i_h].at().attrDel( attr );

    //> Self delete
    try
    {
	pthread_mutex_lock(&mtxAttr);

	map<string, Attr* >::iterator p = mAttrs.find(attr);
	if(p == mAttrs.end())	throw TError(nodePath().c_str(),_("Attribute <%s> is not present!"), attr.c_str());
	int pos = p->second->mOi;
	for( map<string, Attr* >::iterator p1 = mAttrs.begin(); p1 != mAttrs.end(); ++p1 )
	    if( p1->second->mOi > pos ) p1->second->mOi--;
	delete p->second;
	mAttrs.erase(p);
    }
    catch(...){ }
    pthread_mutex_unlock(&mtxAttr);
}

bool Widget::attrPresent(const string &attr)
{
    pthread_mutex_lock(&mtxAttr);
    bool rez = (mAttrs.find(attr) != mAttrs.end());
    pthread_mutex_unlock(&mtxAttr);
    return rez;
}

AutoHD<Attr> Widget::attrAt(const string &attr)
{
    int rez = pthread_mutex_lock(&mtxAttr);
    if(rez && (rez != EDEADLK || !attrAtLockCnt))
	throw TError(nodePath().c_str(),_("Attribute attach access error: %d."),rez);

    map<string, Attr* >::iterator p = mAttrs.find(attr);
    if(p == mAttrs.end())
	throw TError(nodePath().c_str(),_("Attribute <%s> is not present!"), attr.c_str());

    return AutoHD<Attr>(p->second);
}

int  Widget::attrPos(const string &inm)
{
    return attrAt(inm).at().mOi;
}

void Widget::calc( Widget *base )
{
    if( !parent().freeStat() ) parent().at().calc(base);
}

bool Widget::attrChange( Attr &cfg, TVariant prev )
{
    //> Process Active attribute's mode
    if( cfg.flgGlob()&Attr::Active && !prev.isNull() && !parent().freeStat() )	parent().at().attrChange(cfg,prev);
    if( cfg.owner() != this )	return false;

    //> Update heritors attributes
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( m_herit[i_h].at().enable( ) )
	    m_herit[i_h].at().inheritAttr(cfg.id());

    return true;
}

void Widget::wdgList( vector<string> &list )
{
    chldList( inclWdg, list );
}

bool Widget::wdgPresent( const string &wdg )
{
    return chldPresent( inclWdg, wdg );
}

void Widget::wdgAdd( const string &wid, const string &name, const string &path, bool force )
{
    if( !isContainer() )  throw TError(nodePath().c_str(),_("Widget is not container!"));
    if( wdgPresent(wid) ) return;

    chldAdd( inclWdg, new Widget(wid,path) );
    wdgAt(wid).at().setName(name);

    //> Call heritors include widgets update
    for( int i_h = 0; i_h < m_herit.size(); i_h++ )
	if( m_herit[i_h].at().enable( ) )
	    m_herit[i_h].at().inheritIncl(wid);
}

void Widget::wdgDel( const string &wid, bool full )
{
    if( wdgPresent(wid) )	chldDel( inclWdg, wid, -1, full, true );
}

AutoHD<Widget> Widget::wdgAt( const string &wdg )
{
    return chldAt( inclWdg, wdg );
}

TVariant Widget::vlGet( Attr &a )
{
    if( a.id() == "id" )	return TVariant(id());
    else if( a.id() == "path" )	return TVariant(path());
    else if( a.id() == "root" )	return TVariant(rootId());
    else if( a.id() == "parent" )	return TVariant(parentNm());

    return TVariant();
}

TVariant Widget::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    if( !stlLock() && dynamic_cast<Widget*>(nodePrev()) )
	return ((Widget*)nodePrev())->stlReq( a, vl, wr );
    return vl;
}

bool Widget::cntrCmdServ( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //> Service commands process -
    if(a_path == "/serv/attr")	//Attribute's access
    {
	if(ctrChkNode(opt,"get",RWRWRW,"root",SUI_ID,SEC_RD))		//Get values
	{
	    vector<string> ls;
	    attrList(ls);
	    AutoHD<Attr> attr;
	    for(int i_l = 0; i_l < ls.size(); i_l++)
	    {
		attr = attrAt(ls[i_l]);
		if(attr.at().flgGlob()&Attr::IsUser)	continue;
		opt->childAdd("el")->setAttr("id",ls[i_l].c_str())->
				     setAttr("p",attr.at().fld().reserve())->
				     setText(attr.at().getS());
	    }
	}
	else if(ctrChkNode(opt,"set",RWRWRW,"root",SUI_ID,SEC_WR))	//Set values
	    for(int i_ch = 0; i_ch < opt->childSize(); i_ch++)
	        attrAt(opt->childGet(i_ch)->attr("id")).at().setS(opt->childGet(i_ch)->text());
    }
    else if(a_path == "/serv/attrBr" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID,SEC_RD))	//Get attributes all updated elements' of the branch
    {
	//>> Self attributes put
	vector<string> ls;
	attrList(ls);
	AutoHD<Attr> attr;
	for(int i_l = 0; i_l < ls.size(); i_l++)
	{
	    attr = attrAt(ls[i_l]);
	    if(attr.at().flgGlob()&Attr::IsUser)	continue;
	    opt->childAdd("el")->setAttr("id",ls[i_l].c_str())->
			     setAttr("p",attr.at().fld().reserve())->
			     setText(attr.at().getS());
	}
	//>> Child widgets process
	if(enable())
	{
	    AutoHD<Widget> iwdg;
	    vector<string>	lst;
	    if(isLink())
	    {
		parentNoLink().at().wdgList(lst);
		opt->setAttr("lnkPath",parentNoLink().at().path());
	    }
	    else wdgList(lst);
	    for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    {
		if(!isLink()) iwdg = wdgAt(lst[i_f]);
		else iwdg = parentNoLink().at().wdgAt(lst[i_f]);
		XMLNode *wn = opt->childAdd("get")->setAttr("path",a_path);
		iwdg.at().cntrCmdServ(wn);
		wn->setName("w")->attrDel("path")->attrDel("rez")->setAttr("id",lst[i_f]);
	    }
	}
    }
    else return false;

    return true;
}

bool Widget::cntrCmdGeneric( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget: ")+id(),RWRWR_,"root",SUI_ID);
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")))
	{
	    if(ctrMkNode("area",opt,-1,"/wdg/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enable"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/wdg/w_lst");
		if(!parent().freeStat())
		    ctrMkNode("comm",opt,-1,"/wdg/st/goparent",_("Go to parent"),RWRWR_,"root",SUI_ID,1,"tp","lnk");
	    }
	    if(ctrMkNode("area",opt,-1,"/wdg/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/cfg/id",_("Id"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/type",_("Type"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/root",_("Root"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/path",_("Path"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/descr",_("Description"),RWRWR_,"root",SUI_ID,3,"tp","str","cols","100","rows","4");
		ctrMkNode("img",opt,-1,"/wdg/cfg/ico",_("Icon"),(isLink()?R_R_R_:RWRWR_),"root",SUI_ID,2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/owner",_("Owner and group"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/wdg/u_lst");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/grp","",RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/wdg/g_lst");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/u_a",_("Access"),RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/wdg/cfg/g_a","",RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/wdg/cfg/o_a","",RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("comm",opt,-1,"/wdg/cfg/clear",_("Clear widget's changing"),RWRWR_,"root",SUI_ID);
	    }
	}
	if(isContainer() && (!isLink()) && ctrMkNode("area",opt,-1,"/inclwdg",_("Include widgets")))
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","wdg_","idSz","30");
	if(isContainer() && (!isLink()) && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/res" && ctrChkNode(opt))	//Service command for resources request
    {
	string mime;
	opt->setText(resourceGet(opt->attr("id"),&mime));
	opt->setAttr("mime",mime);
    }
    else if(a_path == "/wdg/st/en")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(atoi(opt->text().c_str()));
    }
    else if(a_path == "/wdg/st/parent")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(parentNm());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setParentNm(opt->text());
    }
    else if(a_path == "/wdg/st/goparent" && ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD) && !parent().freeStat())
	opt->setText(parent().at().nodePath(0,true));
    else if(a_path == "/wdg/cfg/owner")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(owner());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setOwner(opt->text());
    }
    else if(a_path == "/wdg/cfg/grp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(grp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setGrp(opt->text());
    }
    else if(a_path == "/wdg/cfg/u_a" || a_path == "/wdg/cfg/g_a" || a_path == "/wdg/cfg/o_a")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	{
	    if(a_path == "/wdg/cfg/u_a")	opt->setText(TSYS::int2str((permit()>>6)&0x7));
	    if(a_path == "/wdg/cfg/g_a")	opt->setText(TSYS::int2str((permit()>>3)&0x7));
	    if(a_path == "/wdg/cfg/o_a")	opt->setText(TSYS::int2str(permit()&0x7));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	{
	    if(a_path == "/wdg/cfg/u_a")	setPermit((permit()&(~(0x07<<6)))|(atoi(opt->text().c_str())<<6));
	    if(a_path == "/wdg/cfg/g_a")	setPermit((permit()&(~(0x07<<3)))|(atoi(opt->text().c_str())<<3));
	    if(a_path == "/wdg/cfg/o_a")	setPermit((permit()&(~0x07))|atoi(opt->text().c_str()));
	}
    }
    else if(a_path == "/wdg/cfg/ico" || a_path == "/ico")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(ico());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setIco(opt->text());
    }
    else if(a_path == "/wdg/cfg/id" && ctrChkNode(opt))		opt->setText(id());
    else if(a_path == "/wdg/cfg/type" && ctrChkNode(opt))	opt->setText(type());
    else if(a_path == "/wdg/cfg/root" && ctrChkNode(opt))	opt->setText(rootId());
    else if(a_path == "/wdg/cfg/path" && ctrChkNode(opt))
	opt->setText((isLink()&&atoi(opt->attr("resLink").c_str())) ? parentNoLink().at().path() : path());
    else if(a_path == "/wdg/cfg/name")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(name());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(opt->text());
    }
    else if(a_path == "/wdg/cfg/descr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(descr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(opt->text());
    }
    else if(a_path == "/wdg/cfg/clear" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID))
    {
	if(opt->attr("attr").empty()) wClear();
	else if(attrAt(opt->attr("attr")).at().modif())
	{
	    attrAt(opt->attr("attr")).at().setModif(0);
	    inheritAttr(opt->attr("attr"));
	    modif();
	}
    }
    else if(a_path == "/wdg/u_lst" && ctrChkNode(opt))
    {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/wdg/g_lst" && ctrChkNode(opt))
    {
	vector<string> ls;
	SYS->security().at().usrGrpList(owner(), ls );
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/wdg/w_lst" && ctrChkNode(opt))
    {
	int c_lv = 0;
	string c_path = "", c_el;
	string lnk = parentNm();

	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::pathLev(lnk,0,true,&c_off)).size(); c_lv++)
	{
	    c_path += "/"+c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	vector<string>  ls;
	try
	{
	    switch(c_lv)
	    {
		case 0: mod->nodeList(ls);   break;
		case 1:
		    if(TSYS::pathLev(lnk,0) != "..")
			mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeList(ls);
		    break;
		case 2:
		    if(TSYS::pathLev(lnk,0) != "..")
			mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeAt(TSYS::pathLev(lnk,1)).at().nodeList(ls,"wdg_");
		    break;
	    }
	    for(int i_l = 0; i_l < ls.size(); i_l++)
		opt->childAdd("el")->setText(c_path+"/"+ls[i_l]);
	}catch(TError err) { }
    }
    else if(a_path == "/br/wdg_" || a_path == "/inclwdg/wdg")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	{
	    bool chkUserPerm = atoi(opt->attr("chkUserPerm").c_str());
	    if(!chkUserPerm || SYS->security().at().access(opt->attr("user"),SEC_RD,owner(),grp(),permit()))
	    {
		vector<string>  lst;
		wdgList(lst);
		for(unsigned i_f=0; i_f < lst.size(); i_f++)
		{
		    AutoHD<Widget> iwdg = wdgAt(lst[i_f]);
		    if(chkUserPerm && !SYS->security().at().access(opt->attr("user"),SEC_RD,iwdg.at().owner(),iwdg.at().grp(),iwdg.at().permit()))
			continue;
		    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(iwdg.at().name());
		}
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))
	    wdgAdd(TSYS::strEncode(opt->attr("id"),TSYS::oscdID).c_str(),opt->text(),"");
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	wdgDel(opt->attr("id").c_str(),true);
    }
    else return false;

    return true;
}

bool Widget::cntrCmdAttributes( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes")))
	{
	    //>> Properties form create
	    vector<string> list_a;
	    attrList(list_a);
	    for(unsigned i_el = 0; i_el < list_a.size(); i_el++)
	    {
		XMLNode *el = attrAt(list_a[i_el]).at().fld().cntrCmdMake(opt,"/attr",-1,"root",SUI_ID,RWRWR_);
		if(el) el->setAttr("len","")->setAttr("wdgFlg",TSYS::int2str(attrAt(list_a[i_el]).at().flgGlob()))->
			   setAttr("modif",TSYS::uint2str(attrAt(list_a[i_el]).at().modif()));
	    }
	}
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/attr/")
    {
	AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1));
	if(ctrChkNode(opt,"get",(attr.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SUI_ID,SEC_RD))
	{
	    if(attr.at().fld().flg()&TFld::Selected)	opt->setText(attr.at().getSEL());
	    else opt->setText(attr.at().getS());
	}
	if(ctrChkNode(opt,"set",(attr.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SUI_ID,SEC_WR))
	{
	    if(attr.at().fld().flg()&TFld::Selected)	{ attr.at().setSEL(opt->text()); opt->setText(attr.at().getSEL()); }
	    else { attr.at().setS(opt->text()); opt->setText(attr.at().getS()); }
	}
    }
    else return false;

    return true;
}

bool Widget::cntrCmdLinks( XMLNode *opt, bool lnk_ro )
{
    //> Get page info
    if(opt->name() == "info")
    {
	if(ctrMkNode("area",opt,-1,"/links",_("Links")))
	{
	    ctrMkNode("fld",opt,-1,"/links/showAttr",_("Attributes show"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links")))
	    {
		bool shwAttr =	atoi(opt->attr("showAttr").c_str()) ||
				atoi(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")).c_str());
		vector<string> incllist, alist, list;
		wdgList(incllist);
		for(int i_w = -1; i_w < (int)incllist.size(); i_w++)
		{
		    AutoHD<Widget> wdg;
		    if(i_w < 0) wdg = AutoHD<Widget>(this);
		    else wdg = wdgAt(incllist[i_w]);
		    wdg.at().attrList(alist);
		    for(int i_a = 0; i_a < alist.size(); i_a++)
		    {
			string grpprm;
			string idprm = alist[i_a];
			string nprm  = wdg.at().attrAt(alist[i_a]).at().id();
			if(i_w >= 0)
			{
			    idprm.insert(0,incllist[i_w]+".");
			    nprm.insert(0,wdg.at().id()+".");
			}

			if(!(wdg.at().attrAt(alist[i_a]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst)) ||
			    (!shwAttr && wdg.at().attrAt(alist[i_a]).at().flgSelf()&Attr::CfgConst)) continue;
			//>> Get attributes
			bool shwTmpl = wdg.at().attrAt(alist[i_a]).at().cfgTempl().size();
			if(shwTmpl)	grpprm = TSYS::strSepParse(wdg.at().attrAt(alist[i_a]).at().cfgTempl(),0,'|');

			//>> Check select param
			if(shwTmpl && !shwAttr)
			{
			    nprm = grpprm;
			    if(i_w >= 0) nprm.insert(0,wdg.at().id()+".");

			    //>> Check already to present parameters
			    bool f_ok = false;
			    for(int i_l = 0; i_l < list.size(); i_l++)
				if(list[i_l] == nprm) { f_ok = true; break; }
			    if(!f_ok)
			    {
				ctrMkNode("fld",opt,-1,(string("/links/lnk/pr_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,
				    3,"tp","str","dest","sel_ed","select",(string("/links/lnk/pl_")+idprm).c_str());
				list.push_back(nprm);
			    }
			}
			else
			{
			    XMLNode *nel = NULL;
			    if(wdg.at().attrAt(alist[i_a]).at().flgSelf()&Attr::CfgConst)
				nel = ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,2,"tp","str","elGrp",grpprm.c_str());
			    else
				nel = ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),
				    nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,4,"tp","str","dest","sel_ed","select",(string("/links/lnk/ls_")+idprm).c_str(),"elGrp",grpprm.c_str());
			    if(nel && atoi(opt->attr("inclValue").c_str())) nel->setText(wdg.at().attrAt(alist[i_a]).at().cfgVal());
			}
		    }
		}
	    }
	}
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/links/showAttr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"showAttr",opt->text(),opt->attr("user"));
    }
    else if(a_path.substr(0,14) == "/links/lnk/pr_")
    {
	vector<string> a_ls;
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size())	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	//> Search first not config field if default field is config.
	if(srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst)
	{
	    srcwdg.at().attrList(a_ls);
	    for(int i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') && !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[i_a]; break; }
	}

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
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
	    else if( !custom )		rez = cfg_val;

	    opt->setText(rez);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	{
	    bool noonly_no_set = true;
	    string no_set;
	    string cfg_val = opt->text();
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,':')+":";
	    string cfg_addr = (obj_tp.size()<cfg_val.size()) ? cfg_val.substr(obj_tp.size()) : "";

	    int c_lvl = 0;
	    for(int c_off = 0; TSYS::pathLev(cfg_addr,0,true,&c_off).size(); c_lvl++) ;

	    AutoHD<TValue> prm;
	    if(obj_tp == "prm:" && c_lvl==3)	prm = SYS->daq().at().nodeAt(cfg_addr);
	    AutoHD<Widget> dstwdg;
	    if(obj_tp == "wdg:" && c_lvl==1)
	    {
		string nwdg = TSYS::pathLev(cfg_addr,0);
		if(nwdg=="self") dstwdg = this;
		else	dstwdg = wdgAt(nwdg);
	    }

	    srcwdg.at().attrList(a_ls);
	    for(int i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') && !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{
		    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val);
		    string p_attr = TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),1,'|');
		    if(!prm.freeStat() || !dstwdg.freeStat())
		    {
			if((!prm.freeStat() && prm.at().vlPresent(p_attr)) ||
			    (!dstwdg.freeStat() && dstwdg.at().attrPresent(p_attr)))
			{
			    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val+"/"+p_attr);
			    noonly_no_set = false;
			}
			else no_set+=p_attr+",";
		    }
		}
	    if(!prm.freeStat() || !dstwdg.freeStat())
	    {
		if(noonly_no_set)	throw TError(nodePath().c_str(),_("Destination has no any necessary attribute!"));
		else if(no_set.size())	throw TError(nodePath().c_str(),_("Destination has no attributes: %s !"),no_set.c_str());
	    }
	}
    }
    else if((a_path.substr(0,14) == "/links/lnk/pl_" || a_path.substr(0,14) == "/links/lnk/ls_") && ctrChkNode(opt))
    {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size()) srcwdg = wdgAt(nwdg);
	else nattr = nwdg;

	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
	if(!(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut)))
	{
	    if(!is_pl) throw TError(nodePath().c_str(),_("Variable is not link"));
	    vector<string> a_ls;
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    srcwdg.at().attrList(a_ls);
	    int i_a;
	    for(i_a = 0; i_a < a_ls.size(); i_a++)
	        if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') && !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
	        { nattr = a_ls[i_a]; break; }
	    if(i_a >= a_ls.size()) throw TError(nodePath().c_str(),_("Variable is not link"));
	}

	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();

	//>> Link interface process
	int c_lv = 0;
	string obj_tp = TSYS::strSepParse(m_prm,0,':')+":";
	if(obj_tp.empty() || !(obj_tp == "val:" || obj_tp == "prm:" || obj_tp == "wdg:"))
	{
	    if(!is_pl) opt->childAdd("el")->setText(_("val:Constant value"));
	    opt->childAdd("el")->setText("prm:");
	    opt->childAdd("el")->setText("wdg:");
	}
	//>> Link elements process
	else
	{
	    int c_off = obj_tp.size();
	    string c_path = obj_tp, c_el;
	    opt->childAdd("el")->setText("");
	    opt->childAdd("el")->setText(c_path);
	    for( ;(c_el=TSYS::pathLev(m_prm,0,true,&c_off)).size(); c_lv++)
	    {
		if(is_pl && ((obj_tp=="prm:" && c_lv>=3) || (obj_tp=="wdg:" && c_lv>=1))) break;
		c_path += "/"+c_el;
		opt->childAdd("el")->setText(c_path);
	    }
	    vector<string> ls;
	    c_off = obj_tp.size();
	
	    string prm1 = TSYS::pathLev(m_prm,0,true,&c_off);
	    string prm2 = TSYS::pathLev(m_prm,0,true,&c_off);
	    string prm3 = TSYS::pathLev(m_prm,0,true,&c_off);
	    switch(c_lv)
	    {
		case 0:
		    if(obj_tp == "prm:") SYS->daq().at().modList(ls);
		    if(obj_tp == "wdg:")
		    {
			wdgList(ls);
			ls.push_back("self");
		    }
		    break;
		case 1:
		    if(obj_tp == "prm:" && SYS->daq().at().modPresent(prm1))
			SYS->daq().at().at(prm1).at().list(ls);
		    if(!is_pl && obj_tp == "wdg:")
		    {
			AutoHD<Widget> wdg;
			if(prm1 == "self")	wdg = this;
			else wdg = wdgAt(prm1);
			if(!wdg.freeStat()) wdg.at().attrList(ls);
		    }
		    break;
		case 2:
		    if(obj_tp == "prm:" && SYS->daq().at().modPresent(prm1)
			      && SYS->daq().at().at(prm1).at().present(prm2))
			SYS->daq().at().at(prm1).at().at(prm2).at().list(ls);
		    break;
		case 3:
		    if(!is_pl && obj_tp=="prm:" && SYS->daq().at().modPresent(prm1)
			      && SYS->daq().at().at(prm1).at().present(prm2)
			      && SYS->daq().at().at(prm1).at().at(prm2).at().present(prm3))
			SYS->daq().at().at(prm1).at().at(prm2).at().at(prm3).at().vlList(ls);
		    break;
	    }
	    for(int i_l = 0; i_l < ls.size(); i_l++)
		opt->childAdd("el")->setText(c_path+"/"+ls[i_l]);
	}
    }
    else if(a_path.substr(0,14) == "/links/lnk/el_")
    {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size())	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	
	if(ctrChkNode(opt,"get",RWRWR_,"root","UI",SEC_RD))
	    opt->setText(srcwdg.at().attrAt(nattr).at().cfgVal());
	if(ctrChkNode(opt,"set",RWRWR_,"root","UI",SEC_WR))
	{
	    srcwdg.at().attrAt(nattr).at().setCfgVal(opt->text());
	    if(srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst)
		srcwdg.at().attrAt(nattr).at().setS(opt->text());
	}
    }
    else return false;

    return true;
}

bool Widget::cntrCmdProcess( XMLNode *opt )
{
    string wattr;

    if(isLink()) return false;

    //> Get page info
    if(opt->name() == "info")
    {
	if(ctrMkNode("area",opt,-1,"/proc",_("Process")))
	{
	    ctrMkNode("fld",opt,-1,"/proc/wdg",_("Widget"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/proc/w_lst");
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if(!wdgPresent(wattr))	wattr = ".";
	    if(ctrMkNode("table",opt,-1,"/proc/attr",_("Attributes"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id"))
	    {
		ctrMkNode("list",opt,-1,"/proc/attr/id",_("Id"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/type",_("Type"),RWRWR_,"root",SUI_ID,4,"tp","dec","idm","1","dest","select","select","/proc/tp_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/wa",_("Work area"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/proc",_("Process"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/proc/attr/cfg",_("Config"),RWRWR_,"root",SUI_ID,4,"tp","dec","idm","1","dest","select","select","/proc/lnk_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/cfgtmpl",_("Config template"),RWRWR_,"root",SUI_ID,1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/proc/calc",_("Calculate")))
	    {
		ctrMkNode("fld",opt,-1,"/proc/calc/progLng",_("Programm language"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/proc/calc/plangLs");
		ctrMkNode("fld",opt,-1,"/proc/calc/per",_("Calc period (ms)"),RWRWR_,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/proc/calc/prog",_("Programm"),RWRWR_,"root",SUI_ID,3,"tp","str","rows","10","SnthHgl","1");
	    }
	}
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/proc/wdg")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	{
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if(wattr != "." && !wdgPresent(wattr))	wattr = ".";
	    opt->setText(wattr);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"wdgAttr",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/proc/w_lst" && ctrChkNode(opt))
    {
	vector<string> lst;
	wdgList(lst);
	opt->childAdd("el")->setText(".");
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setText(lst[i_f]);
    }
    else if(a_path == "/proc/attr")
    {
	wattr = opt->attr("wdg");
	if(wattr.empty())
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	if(wattr != "." && !wdgPresent(wattr))	wattr = ".";

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	{
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/proc/attr/id","");
	    XMLNode *n_name	= ctrMkNode("list",opt,-1,"/proc/attr/name","");
	    XMLNode *n_type	= ctrMkNode("list",opt,-1,"/proc/attr/type","");
	    XMLNode *n_wa	= ctrMkNode("list",opt,-1,"/proc/attr/wa","");
	    XMLNode *n_proc	= ctrMkNode("list",opt,-1,"/proc/attr/proc","");
	    XMLNode *n_cfg	= ctrMkNode("list",opt,-1,"/proc/attr/cfg","");
	    XMLNode *n_cfgtmpl	= ctrMkNode("list",opt,-1,"/proc/attr/cfgtmpl","");

	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    vector<string> lst;
	    wdg.at().attrList(lst);
	    for(int i_el = 0; i_el < lst.size(); i_el++)
	    {
		if(n_id)	n_id->childAdd("el")->setText(lst[i_el]);
		if(n_name)	n_name->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().name());
		if(n_type)	n_type->childAdd("el")->setText(TSYS::int2str(wdg.at().attrAt(lst[i_el]).at().fld().type()+
				    ((wdg.at().attrAt(lst[i_el]).at().fld().flg()&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
		if(n_wa)	n_wa->childAdd("el")->setText( wdg.at().attrAt(lst[i_el]).at().fld().values()+"|"+
							    wdg.at().attrAt(lst[i_el]).at().fld().selNames());
		if(n_proc)	n_proc->childAdd("el")->setText(TSYS::int2str(wdg.at().attrAt(lst[i_el]).at().flgSelf()&Attr::ProcAttr));
		if(n_cfg)	n_cfg->childAdd("el")->setText(TSYS::int2str(wdg.at().attrAt(lst[i_el]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle)));
		if(n_cfgtmpl)	n_cfgtmpl->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().cfgTempl());
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))
	{
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    wdg.at().attrAdd( new TFld("newAttr",_("New attribute"),TFld::String,Attr::IsUser) );
	    wdg.at().attrAt("newAttr").at().setS(EVAL_STR);
	    wdg.at().attrAt("newAttr").at().setModif(1);
	    wdg.at().modif();
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))
	{
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    if( !wdg.at().attrPresent(opt->attr("key_id")) )
		throw TError(nodePath().c_str(),_("Deleting the enclosed widget's elements error."));
	    if( !(wdg.at().attrAt(opt->attr("key_id")).at().flgSelf()&Attr::IsInher) &&
		    wdg.at().attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsUser )
		wdg.at().attrDel(opt->attr("key_id"));
	    else throw TError(nodePath().c_str(),_("Deletint the not user element error."));
	    wdg.at().modif();
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	{
	    //>> Request data
	    string idattr = opt->attr("key_id");
	    string idcol  = opt->attr("col");
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);

	    if(idcol == "id" || idcol == "name" || idcol == "type" || idcol == "wa")
	    {
		string		tid	= idattr;
		string		tnm	= wdg.at().attrAt(idattr).at().name();
		TFld::Type	ttp	= wdg.at().attrAt(idattr).at().fld().type();
		unsigned	tflg	= wdg.at().attrAt(idattr).at().fld().flg();
		string		tvals	= wdg.at().attrAt(idattr).at().fld().values();
		string		tsels	= wdg.at().attrAt(idattr).at().fld().selNames();
		Attr::SelfAttrFlgs sflgs = wdg.at().attrAt(idattr).at().flgSelf();

		if(!(!(sflgs&Attr::IsInher) && tflg&Attr::IsUser))
		    throw TError(nodePath().c_str(),_("Changing of not user attribute is no permited"));

		string tvl	= wdg.at().attrAt(idattr).at().getS();
		string tmpl	= wdg.at().attrAt(idattr).at().cfgTempl();
		string cfgval	= wdg.at().attrAt(idattr).at().cfgVal();

		if(idcol == "id")
		{
		    tid = opt->text();
		    if(wdg.at().attrPresent(tid)) throw TError(nodePath().c_str(),_("New attribute's ID '%s' already present."),tid.c_str());
		}
		else if(idcol == "name")	tnm = opt->text();
		else if(idcol == "type")
		{
		    ttp = (TFld::Type)(atoi(opt->text().c_str())&0x0f);
		    tflg = tflg^((tflg^((atoi(opt->text().c_str())>>4)|Attr::IsUser))&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address));
		}
		else if(idcol == "wa")
		{
		    tvals = TSYS::strSepParse(opt->text(),0,'|');
		    tsels = TSYS::strSepParse(opt->text(),1,'|');
		}
		wdg.at().attrDel(idattr);
		wdg.at().attrAdd( new TFld(tid.c_str(),tnm.c_str(),ttp,tflg,"","",tvals.c_str(),tsels.c_str()) );
		wdg.at().attrAt(tid).at().setS(tvl);
		wdg.at().attrAt(tid).at().setFlgSelf(sflgs);
		wdg.at().attrAt(tid).at().setCfgVal(cfgval);
		wdg.at().attrAt(tid).at().setCfgTempl(tmpl);
		wdg.at().attrAt(tid).at().setModif(1);
	    }
	    else
	    {
		if(idcol == "proc")
		{
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = atoi(opt->text().c_str())?Attr::ProcAttr:(Attr::SelfAttrFlgs)0;
		    wdg.at().attrAt(idattr).at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::ProcAttr)) );
		}
		else if(idcol == "cfg")
		{
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)atoi(opt->text().c_str());
		    if((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle))
			wdg.at().attrAt(idattr).at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle))) );
		}
		else if(idcol == "cfgtmpl")	wdg.at().attrAt(idattr).at().setCfgTempl(opt->text());
	    }
	    wdg.at().modif();
	}
    }
    else if(a_path == "/proc/calc/progLng")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(calcLang());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcLang(opt->text());
    }
    else if(a_path == "/proc/calc/per")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(calcPer()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcPer(atoi(opt->text().c_str()));
    }
    else if(a_path == "/proc/calc/prog")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(calcProg());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
	    try
	    {
		SYS->daq().at().at(TSYS::strParse(calcLang(),0,".")).at().
				compileFuncSynthHighl(TSYS::strParse(calcLang(),1,"."),*opt);
	    } catch(...){ }
    }
    else if(a_path == "/proc/calc/plangLs" && ctrChkNode(opt))
    {
	string tplng = calcLang();
	int c_lv = 0;
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::strSepParse(tplng,0,'.',&c_off)).size(); c_lv++)
	{
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	vector<string>  ls;
	switch(c_lv)
	{
	    case 0:
		SYS->daq().at().modList(ls);
		for(int i_l = 0; i_l < ls.size(); i_l++)
		    if(!SYS->daq().at().at(ls[i_l]).at().compileFuncLangs())
		    { ls.erase(ls.begin()+i_l); i_l--; }
		break;
	    case 1:
		if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
		    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(&ls);
		break;
	}
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if(a_path == "/proc/tp_ls" && ctrChkNode(opt))
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
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Integer+(Attr::DateTime<<4)))->setText(_("DateTime"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String+(Attr::Address<<4)))->setText(_("Address"));
    }
    else if(a_path == "/proc/lnk_ls" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0))->setText(_("Not"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgConst))->setText(_("Constant"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgLnkIn))->setText(_("Input link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgLnkOut))->setText(_("Output link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::CfgLnkIn|Attr::CfgLnkOut))->setText(_("Full link"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(Attr::FromStyle))->setText(_("From style"));
    }
    else return false;

    return true;
}

//************************************************
//* Attr: Widget attribute                       *
//************************************************
Attr::Attr( TFld *ifld, bool inher ) : mFld(NULL), m_modif(0), self_flg((SelfAttrFlgs)0), mOwner(NULL)
{
    setFld( ifld, inher );
}

Attr::~Attr(  )
{
    setFld( NULL, false );
}

void Attr::setFld( TFld *fld, bool inher )
{
    if( mFld && (!fld || fld->type() != TFld::String) && mFld->type() == TFld::String && m_val.s_val ) delete m_val.s_val;

    if( fld && (!mFld || fld->type() != mFld->type()) )
	switch( fld->type() )
	{
	    case TFld::String:
		m_val.s_val = NULL;
		if( fld->flg()&Attr::DirRead ) break;
		m_val.s_val    = new string();
		*m_val.s_val = fld->def();
		break;
	    case TFld::Integer:
		m_val.i_val = strtol(fld->def().c_str(),NULL,(fld->flg()&TFld::HexDec)?16:((fld->flg()&TFld::OctDec)?8:10));
		break;
	    case TFld::Real:	m_val.r_val = atof(fld->def().c_str());	break;
	    case TFld::Boolean:	m_val.b_val = atoi(fld->def().c_str());	break;
	}

    if( mFld )
    {
	mFld->setLen(mFld->len()-1);
	if( !mFld->len() ) delete mFld;
	mFld = NULL;
    }
    mFld = fld;
    if( mFld && !inher )	mFld->setLen(1);
    else if( mFld && inher )	mFld->setLen(mFld->len()+1);
    self_flg = inher ? self_flg|Attr::IsInher : self_flg & ~Attr::IsInher;
}

const string &Attr::id( )	{ return fld().name(); }

string Attr::name( )		{ return fld().descr(); }

TFld::Type Attr::type( )	{ return fld().type(); }

int Attr::flgGlob( )		{ return fld().flg(); }

string Attr::getSEL( bool sys )
{
    if( !(fld().flg()&TFld::Selected) ) throw TError("Cfg",_("Element type is not selected!"));
    switch( fld().type() )
    {
	case TFld::String:	return fld().selVl2Nm(getS(sys));
	case TFld::Integer:	return fld().selVl2Nm(getI(sys));
	case TFld::Real:	return fld().selVl2Nm(getR(sys));
	case TFld::Boolean:	return fld().selVl2Nm(getB(sys));
    }
}

string Attr::getS( bool sys )
{
    if( flgGlob()&Attr::DirRead )	return owner()->vlGet(*this).getS();
    if( flgSelf()&Attr::FromStyle && !sys )	return owner()->stlReq(*this,getS(true),false).getS();

    switch( fld().type() )
    {
	case TFld::Integer:	return (m_val.i_val != EVAL_INT) ? TSYS::int2str(m_val.i_val) : EVAL_STR;
	case TFld::Real:	return (m_val.r_val != EVAL_REAL) ? TSYS::real2str(m_val.r_val) : EVAL_STR;
	case TFld::Boolean:	return (m_val.b_val != EVAL_BOOL) ? TSYS::int2str((bool)m_val.b_val) : EVAL_STR;
	case TFld::String:	return *m_val.s_val;
    }
}

int Attr::getI( bool sys )
{
    if( flgGlob()&Attr::DirRead )	return owner()->vlGet(*this).getI();
    if( flgSelf()&Attr::FromStyle && !sys )	return owner()->stlReq(*this,getI(true),false).getI();
    switch( fld().type() )
    {
	case TFld::String:	return (*m_val.s_val != EVAL_STR) ? atoi(m_val.s_val->c_str()) : EVAL_INT;
	case TFld::Real:	return (m_val.r_val != EVAL_REAL) ? (int)m_val.r_val : EVAL_INT;
	case TFld::Boolean:	return (m_val.b_val != EVAL_BOOL) ? (bool)m_val.b_val : EVAL_INT;
	case TFld::Integer:	return m_val.i_val;
    }
}

double Attr::getR( bool sys )
{
    if( flgGlob()&Attr::DirRead )	return owner()->vlGet(*this).getR();
    if( flgSelf()&Attr::FromStyle && !sys )	return owner()->stlReq(*this,getR(true),false).getR();
    switch( fld().type() )
    {
	case TFld::String:	return (*m_val.s_val != EVAL_STR) ? atof(m_val.s_val->c_str()) : EVAL_REAL;
	case TFld::Integer:	return (m_val.i_val != EVAL_INT) ? m_val.i_val : EVAL_REAL;
	case TFld::Boolean:	return (m_val.b_val != EVAL_BOOL) ? (bool)m_val.b_val : EVAL_REAL;
	case TFld::Real:	return m_val.r_val;
    }
}

char Attr::getB( bool sys )
{
    if( flgGlob()&Attr::DirRead )	return owner()->vlGet(*this).getB();
    if( flgSelf()&Attr::FromStyle && !sys )	return owner()->stlReq(*this,getB(true),false).getB();
    switch( fld().type() )
    {
	case TFld::String:	return (*m_val.s_val != EVAL_STR) ? (bool)atoi(m_val.s_val->c_str()) : EVAL_BOOL;
	case TFld::Integer:	return (m_val.i_val != EVAL_INT) ? (bool)m_val.i_val : EVAL_BOOL;
	case TFld::Real:	return (m_val.r_val != EVAL_REAL) ? (bool)m_val.r_val : EVAL_BOOL;
	case TFld::Boolean:	return m_val.b_val;
    }
}

void Attr::setSEL( const string &val, bool strongPrev, bool sys )
{
    if( flgGlob( )&Attr::DirRead ) return;
    if( !(fld().flg()&TFld::Selected) )
	throw TError("Cfg",_("Element type is not selected!"));
    switch( fld().type() )
    {
	case TFld::String:	setS( fld().selNm2VlS(val), strongPrev, sys );	break;
	case TFld::Integer:	setI( fld().selNm2VlI(val), strongPrev, sys );	break;
	case TFld::Real:	setR( fld().selNm2VlR(val), strongPrev, sys );	break;
	case TFld::Boolean:	setB( fld().selNm2VlB(val), strongPrev, sys );	break;
    }
}

void Attr::setS( const string &val, bool strongPrev, bool sys )
{
    if( flgGlob( )&Attr::DirRead ) return;
    switch( fld().type() )
    {
	case TFld::Integer:	setI( (val!=EVAL_STR) ? atoi(val.c_str()) : EVAL_INT, strongPrev, sys );	break;
	case TFld::Real:	setR( (val!=EVAL_STR) ? atof(val.c_str()) : EVAL_REAL, strongPrev, sys );	break;
	case TFld::Boolean:	setB( (val!=EVAL_STR) ? (bool)atoi(val.c_str()) : EVAL_BOOL, strongPrev, sys );	break;
	case TFld::String:
	{
	    if( (!strongPrev && *m_val.s_val == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()) )	break;
	    string t_str = *m_val.s_val;
	    *m_val.s_val = val;
	    if( !sys && !owner()->attrChange(*this,TVariant(t_str)) )	*m_val.s_val = t_str;
	    else
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
	    break;
	}
    }
}

void Attr::setI( int val, bool strongPrev, bool sys )
{
    if( flgGlob( )&Attr::DirRead ) return;
    switch( fld().type() )
    {
	case TFld::String:	setS( (val!=EVAL_INT) ? TSYS::int2str(val) : EVAL_STR, strongPrev, sys );	break;
	case TFld::Real:	setR( (val!=EVAL_INT) ? val : EVAL_REAL, strongPrev, sys );			break;
	case TFld::Boolean:	setB( (val!=EVAL_INT) ? (bool)val : EVAL_BOOL, strongPrev, sys );		break;
	case TFld::Integer:
	{
	    if( !(fld().flg()&TFld::Selected) && fld().selValI()[0] < fld().selValI()[1] )
		val = vmin(fld().selValI()[1],vmax(fld().selValI()[0],val));
	    if( (!strongPrev && m_val.i_val == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()) )	break;
	    int t_val = m_val.i_val;
	    m_val.i_val = val;
	    if( !sys && !owner()->attrChange(*this,TVariant(t_val)) )	m_val.i_val = t_val;
	    else
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
	    break;
	}
    }
}

void Attr::setR( double val, bool strongPrev, bool sys )
{
    if( flgGlob( )&Attr::DirRead ) return;
    switch( fld().type() )
    {
	case TFld::String:	setS( (val!=EVAL_REAL) ? TSYS::real2str(val) : EVAL_STR, strongPrev, sys );	break;
	case TFld::Integer:	setI( (val!=EVAL_REAL) ? (int)val : EVAL_INT, strongPrev, sys );		break;
	case TFld::Boolean:	setB( (val!=EVAL_REAL) ? (bool)val : EVAL_BOOL, strongPrev, sys );		break;
	case TFld::Real:
	{
	    if( !(fld().flg()&TFld::Selected) && fld().selValR()[0] < fld().selValR()[1] )
		val = vmin(fld().selValR()[1],vmax(fld().selValR()[0],val));
	    if( (!strongPrev && m_val.r_val == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()) )	break;
	    double t_val = m_val.r_val;
	    m_val.r_val = val;
	    if( !sys && !owner()->attrChange(*this,TVariant(t_val)) )	m_val.r_val = t_val;
	    else
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
	    break;
	}
    }
}

void Attr::setB( char val, bool strongPrev, bool sys )
{
    if( flgGlob( )&Attr::DirRead ) return;
    switch( fld().type() )
    {
	case TFld::String:	setS( (val!=EVAL_BOOL) ? TSYS::int2str((bool)val) : EVAL_STR, strongPrev, sys );	break;
	case TFld::Integer:	setI( (val!=EVAL_BOOL) ? (bool)val : EVAL_INT, strongPrev, sys );			break;
	case TFld::Real:	setR( (val!=EVAL_BOOL) ? (bool)val : EVAL_REAL, strongPrev, sys );			break;
	case TFld::Boolean:
	{
	    if( (!strongPrev && m_val.b_val == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()) )	break;
	    bool t_val = m_val.b_val;
	    m_val.b_val = val;
	    if( !sys && !owner()->attrChange(*this,TVariant(t_val)) )
		m_val.b_val = t_val;
	    else
	    {
		unsigned imdf = owner()->modifVal(*this);
		m_modif = imdf ? imdf : m_modif+1;
	    }
	    break;
	}
    }
}

string Attr::cfgTempl( )
{
    return cfg.substr(0,cfg.find("\n"));
}

string Attr::cfgVal( )
{
    int sepp = cfg.find("\n");
    return (sepp!=string::npos) ? cfg.substr(sepp+1) : "";
}

void Attr::setCfgTempl( const string &vl )
{
    string t_tmpl = cfgTempl();
    if( t_tmpl == vl ) return;
    cfg = vl+"\n"+cfgVal();
    if( !owner()->attrChange(*this,TVariant()) ) cfg = t_tmpl+"\n"+cfgVal();
    else
    {
	unsigned imdf = owner()->modifVal(*this);
	m_modif = imdf ? imdf : m_modif+1;
    }
}

void Attr::setCfgVal( const string &vl )
{
    string t_val = cfgVal();
    if( t_val == vl ) return;
    cfg = cfgTempl()+"\n"+vl;
    if( !owner()->attrChange(*this,TVariant()) ) cfg = cfgTempl()+"\n"+t_val;
    else
    {
	unsigned imdf = owner()->modifVal(*this);
	m_modif = imdf ? imdf : m_modif+1;
    }
}

void Attr::setFlgSelf( SelfAttrFlgs flg )
{
    if( self_flg == flg )	return;
    SelfAttrFlgs t_flg = (SelfAttrFlgs)self_flg;
    self_flg = (flg & ~Attr::IsInher) | (t_flg&Attr::IsInher);
    if( !owner()->attrChange(*this,TVariant()) )	self_flg = t_flg;
    else
    {
	unsigned imdf = owner()->modifVal(*this);
	m_modif = imdf ? imdf : m_modif+1;
    }
}

void Attr::AHDConnect( )
{
    owner()->attrAtLockCnt++;
}

void Attr::AHDDisConnect( )
{
    owner()->attrAtLockCnt--;
    if(!owner()->attrAtLockCnt) pthread_mutex_unlock(&owner()->mtxAttr);
}

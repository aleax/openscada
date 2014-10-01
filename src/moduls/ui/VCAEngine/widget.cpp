
//OpenSCADA system module UI.VCAEngine file: widget.cpp
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include "types.h"
#include "widget.h"

using namespace VCA;

//************************************************
//* Widget                                       *
//************************************************
pthread_mutex_t Widget::mtxAttrCon = PTHREAD_MUTEX_INITIALIZER;

Widget::Widget( const string &id, const string &isrcwdg ) :
    mId(id), mEnable(false), m_lnk(false), mStlLock(false), BACrtHoldOvr(false), mParentNm(isrcwdg)
{
    inclWdg = grpAdd("wdg_");

    //Attributes mutex create
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mtxAttrM, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

Widget::~Widget( )
{
    //Remove attributes
    pthread_mutex_lock(&mtxAttr());
    map<string,Attr*>::iterator p;
    while((p = mAttrs.begin()) != mAttrs.end()) {
	for(int i_c = 0; i_c < 100 && p->second->mConn; i_c++)	TSYS::sysSleep(0.01);
	if(p->second->mConn) mess_err(nodePath().c_str(),_("Attribute '%s' was not free. Force delete!"),p->first.c_str());
	delete p->second;
	mAttrs.erase(p);
    }
    pthread_mutex_unlock(&mtxAttr());

    //Attributes mutex destroy
    pthread_mutex_destroy(&mtxAttrM);
}

TCntrNode &Widget::operator=( TCntrNode &node )
{
    Widget *src_n = dynamic_cast<Widget*>(&node);
    if(!src_n) return *this;

    if(!src_n->enable()) return *this;

    //Parent link copy
    if(src_n->parentNm() != path() && parentNm().empty()) {
	if(parentNm() != src_n->parentNm() && enable()) setEnable(false);
	setParentNm(src_n->parentNm());
    }
    if(!enable()) setEnable(true);

    //Copy generic configuration
    if(src_n->parent().freeStat() || src_n->name() != src_n->parent().at().name())	setName(src_n->name());
    if(src_n->parent().freeStat() || src_n->descr() != src_n->parent().at().descr())	setDescr(src_n->descr());
    if(src_n->parent().freeStat() || src_n->ico() != src_n->parent().at().ico())	setIco(src_n->ico());
    setOwner(src_n->owner());
    setGrp(src_n->grp());
    setPermit(src_n->permit());
    if(src_n->parent().freeStat() || src_n->calcLang() != src_n->parent().at().calcLang())	setCalcLang(src_n->calcLang());
    if(src_n->parent().freeStat() || src_n->calcProg() != src_n->parent().at().calcProg())	setCalcProg(src_n->calcProg());
    if(src_n->parent().freeStat() || src_n->calcPer() != src_n->parent().at().calcPer())	setCalcPer(src_n->calcPer());

    //Copy attributes
    vector<string> els;
    src_n->attrList( els );
    AutoHD<Attr> attr, pattr;
    for(unsigned i_a = 0; i_a < els.size(); i_a++) {
	pattr = src_n->attrAt(els[i_a]);
	if(!attrPresent(els[i_a])) {
	    bool isInher = pattr.at().flgSelf()&Attr::IsInher;
	    attrAdd(isInher ? &src_n->attrAt(els[i_a]).at().fld() : new TFld(src_n->attrAt(els[i_a]).at().fld()), -1, isInher);
	    attrAt(els[i_a]).at().setModif(1);
	}
	attr  = attrAt(els[i_a]);
	attr.at().setFlgSelf(pattr.at().flgSelf());
	if(!(attr.at().flgGlob()&Attr::DirRead))
	    switch(attr.at().type())
	    {
		case TFld::Boolean:	attr.at().setB(pattr.at().getB());	break;
		case TFld::Integer:	attr.at().setI(pattr.at().getI());	break;
		case TFld::Real:	attr.at().setR(pattr.at().getR());	break;
		case TFld::String:	attr.at().setS(pattr.at().getS());	break;
		default: break;
	    }
	attr.at().setCfgTempl(pattr.at().cfgTempl());
	attr.at().setCfgVal(pattr.at().cfgVal());
    }

    //Include widgets copy
    if(!isLink() && src_n->isContainer()) {
	src_n->wdgList(els);
	for(unsigned i_w = 0; i_w < els.size(); i_w++) {
	    if(!wdgPresent(els[i_w]))	wdgAdd(els[i_w],"","");
	    (TCntrNode&)wdgAt(els[i_w]).at() = (TCntrNode&)src_n->wdgAt(els[i_w]).at();
	}
    }

    return *this;
}

void Widget::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore) setEnable(true);
    if(flag&TCntrNode::NodeConnect && !BACrtHoldOvr) {
	//Add main attributes
	attrAdd(new TFld("id",_("Id"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic));
	attrAdd(new TFld("path",_("Path"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic));
	attrAdd(new TFld("parent",_("Parent"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic));
	attrAdd(new TFld("owner",_("Owner"),TFld::String,TFld::NoStrTransl|Attr::Generic,"","root:UI"));
	attrAdd(new TFld("perm",_("Access"),TFld::Integer,TFld::OctDec|TFld::Selected|Attr::Generic,"","0664",
	    "0;0400;0440;0444;0600;0640;0644;0660;0664;0666",
	    _("No access;R_____;R_R___;R_R_R_;RW____;RWR___;RWR_R_;RWRW__;RWRWR_;RWRWRW")));
	attrAdd(new TFld("root",_("Root"),TFld::String,TFld::NoWrite|Attr::DirRead|Attr::Generic,"","","","",i2s(A_ROOT).c_str()));
	attrAdd(new TFld("name",_("Name"),TFld::String,Attr::Generic));
	attrAdd(new TFld("dscr",_("Description"),TFld::String,TFld::FullText|Attr::Generic));
	attrAdd(new TFld("en",_("Enable"),TFld::Boolean,Attr::Generic,"","1","","",i2s(A_EN).c_str()));
	attrAdd(new TFld("active",_("Active"),TFld::Boolean,Attr::Active,"","0","","",i2s(A_ACTIVE).c_str()));
	attrAdd(new TFld("geomX",_("Geometry:x"),TFld::Real,Attr::Generic,"","0","-10000;10000","",i2s(A_GEOM_X).c_str()));
	attrAdd(new TFld("geomY",_("Geometry:y"),TFld::Real,Attr::Generic,"","0","-10000;10000","",i2s(A_GEOM_Y).c_str()));
	attrAdd(new TFld("geomW",_("Geometry:width"),TFld::Real,Attr::Generic,"","100","0;10000","",i2s(A_GEOM_W).c_str()));
	attrAdd(new TFld("geomH",_("Geometry:height"),TFld::Real,Attr::Generic,"","100","0;10000","",i2s(A_GEOM_H).c_str()));
	attrAdd(new TFld("geomXsc",_("Geometry:x scale"),TFld::Real,Attr::Generic,"","1","0.1;100","",i2s(A_GEOM_X_SC).c_str()));
	attrAdd(new TFld("geomYsc",_("Geometry:y scale"),TFld::Real,Attr::Generic,"","1","0.1;100","",i2s(A_GEOM_Y_SC).c_str()));
	attrAdd(new TFld("geomZ",_("Geometry:z"),TFld::Integer,Attr::Generic,"","0","-1000000;1000000","",i2s(A_GEOM_Z).c_str()));
	attrAdd(new TFld("geomMargin",_("Geometry:margin"),TFld::Integer,Attr::Generic,"","0","0;1000","",i2s(A_GEOM_MARGIN).c_str()));
	attrAdd(new TFld("tipTool",_("Tip:tool"),TFld::String,Attr::Generic,"","","","",i2s(A_TIP_TOOL).c_str()));
	attrAdd(new TFld("tipStatus",_("Tip:status"),TFld::String,Attr::Generic,"","","","",i2s(A_TIP_STATUS).c_str()));
	attrAdd(new TFld("contextMenu",_("Context menu"),TFld::String,TFld::FullText|Attr::Generic,"","","","",i2s(A_CTX_MENU).c_str()));
	attrAdd(new TFld("evProc",_("Events process"),TFld::String,TFld::FullText|TFld::NoStrTransl,"200"));
    }
}

void Widget::preDisable( int flag )
{
    //Delete heritors widgets
    while(herit().size()) mod->nodeDel(herit()[0].at().path(), 0, 0x10);

    //Disable widget
    if(enable()) setEnable(false);
}

string Widget::rootId( )	{ return parent().freeStat() ? "" : parent().at().rootId(); }

string Widget::name( )
{
    string wnm = attrAt("name").at().getS();
    return wnm.size() ? wnm : mId;
}

void Widget::setName( const string &inm )	{ attrAt("name").at().setS(inm); }

string Widget::descr( )		{ return attrAt("dscr").at().getS(); }

void Widget::setDescr( const string &idscr )	{ attrAt("dscr").at().setS(idscr); }

string Widget::owner( )		{ return TSYS::strParse(attrAt("owner").at().getS(),0,":"); }

void Widget::setOwner( const string &iown )
{
    attrAt("owner").at().setS(iown+":"+grp());
    //Group update
    if(SYS->security().at().grpAt("UI").at().user(iown)) setGrp("UI");
    else {
	vector<string> gls;
	SYS->security().at().usrGrpList(owner(),gls);
	setGrp(gls.size()?gls[0]:Widget::grp());
    }
}

string Widget::grp( )		{ return TSYS::strParse(attrAt("owner").at().getS(),1,":"); }

void Widget::setGrp( const string &igrp )	{ attrAt("owner").at().setS(owner()+":"+igrp); }

short Widget::permit( )		{ return attrAt("perm").at().getI(); }

void Widget::setPermit( short iperm )		{ attrAt("perm").at().setI(iperm); }

bool Widget::isContainer( )	{ return parent().freeStat() ? false : parent().at().isContainer(); }

string Widget::path( )
{
    Widget *ownW = dynamic_cast<Widget*>(nodePrev());
    if(ownW) return ownW->path()+"/wdg_"+mId;

    return mId;
}

string Widget::calcId( )
{
    Widget *ownW = dynamic_cast<Widget*>(nodePrev());
    if(ownW) return ownW->calcId()+"_"+mId;

    return mId;
}

bool Widget::enable( )		{ return mEnable; }

void Widget::setEnable( bool val )
{
    if(enable() == val) return;

    if(val) {
	if(parentNm() != "root") {
	    try {
		if(TSYS::strNoSpace(parentNm()).empty() || parentNm() == path())
		    throw TError(nodePath().c_str(),_("Empty parent or parent identical self!"));
		if(parentNm() == "..") mParent = AutoHD<TCntrNode>(nodePrev());
		else mParent = mod->nodeAt(parentNm());

		if(isLink() && dynamic_cast<Widget*>(nodePrev()) && mParent.at().path() == ((Widget*)nodePrev())->path())
		{
		    mParent.free();
		    throw TError(nodePath().c_str(),_("Parent identical to owner for link!"));
		}

		//Check for enable parent widget and enable if not
		if(!parent().at().enable()) parent().at().setEnable(true);

		//Inherit
		inheritAttr();
		inheritIncl();

		//Register of heritater
		parent().at().heritReg(this);
	    }
	    catch(TError err) {
		mess_err(nodePath().c_str(),_("Widget enable error: %s"),err.mess.c_str());
		mParent.free();
		if(BACrtHoldOvr) { BACrtHoldOvr = false; postEnable(TCntrNode::NodeConnect); }
		mEnable = false;
		throw;
	    }
	}
	mEnable = true;
	BACrtHoldOvr = false;

	//Load self values from DB
	loadIO();
    }
    if(!val) {
	disable(this);

	//Free no base attributes and restore base
	vector<string> ls;
	attrList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    if(!(attrAt(ls[i_l]).at().flgGlob()&Attr::Generic)) attrDel(ls[i_l], true);

	//Disable heritors widgets
	for(unsigned i_h = 0; i_h < herit().size(); )
	    if(herit()[i_h].at().enable())
		try { herit()[i_h].at().setEnable(false); }
		catch(TError err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Inheriting widget '%s' disable error."),herit()[i_h].at().id().c_str());
		    i_h++;
		}
	    else i_h++;

	if(!mParent.freeStat()) {
	    //Unregister heritater
	    parent().at().heritUnreg(this);
	    //Disconnect parent widget
	    mParent.free();
	}
    }

    //Enable/disable process widgets from container
    vector<string> ls;
    wdgList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	if(val != wdgAt(ls[i_l]).at().enable())
	    try { wdgAt(ls[i_l]).at().setEnable(val); }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Child widget '%s' enable/disable error."),ls[i_l].c_str());
	    }

    mEnable = val;
}

void Widget::setParentNm( const string &isw )
{
    if(enable() && mParentNm != isw) setEnable(false);
    mParentNm = isw;
}

AutoHD<Widget> Widget::parent( )	{ return mParent; }

AutoHD<Widget> Widget::parentNoLink( )	{ return parent().at().isLink() ? parent().at().parentNoLink() : parent(); }

void Widget::heritReg( Widget *wdg )
{
    //Search already herited widget
    for(unsigned i_h = 0; i_h < m_herit.size(); i_h++)
	if(&m_herit[i_h].at() == wdg)	return;
    m_herit.push_back(AutoHD<Widget>(wdg));
}

void Widget::heritUnreg( Widget *wdg )
{
    //Search herited widget
    for(unsigned i_h = 0; i_h < m_herit.size(); i_h++)
	if(&m_herit[i_h].at() == wdg) {
	    m_herit.erase(m_herit.begin()+i_h);
	    return;
	}
}

void Widget::inheritAttr( const string &iattr )
{
    bool loadDef = parent().freeStat();

    //Create no present attributes
    vector<string>  ls;
    if(iattr.empty())	(loadDef ? attrList(ls) : parent().at().attrList(ls));
    else {
	if(!loadDef && parent().at().attrPresent(iattr)) ls.push_back(iattr);
	else if(attrPresent(iattr)) { loadDef = true; ls.push_back(iattr); }
	else return;
    }

    setStlLock(true);
    if(!loadDef) parent().at().setStlLock(true);

    //Configuration inherit
    AutoHD<Attr> attr, pattr;
    for(unsigned i_l = 0; i_l < ls.size(); i_l++) {
	pattr.free();
	if(!attrPresent(ls[i_l])) {
	    if(loadDef) continue;
	    pattr = parent().at().attrAt(ls[i_l]);
	    if(pattr.at().flgGlob()&Attr::Mutable) continue;
	    attrAdd(&pattr.at().fld(), -1, true);
	}
	attr = attrAt(ls[i_l]);
	if(loadDef) {
	    attr.at().setS(attr.at().fld().def(), attr.at().flgGlob()&Attr::Active);
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)0);
	    attr.at().setCfgTempl("");
	    attr.at().setCfgVal("");
	    attr.at().setModif(0);
	    continue;
	}
	if(pattr.freeStat()) pattr = parent().at().attrAt(ls[i_l]);
	if(!(attr.at().flgSelf()&Attr::IsInher)) attr.at().setFld(&pattr.at().fld(),true);
	if(attr.at().modif() && !(attr.at().flgSelf()&Attr::SessAttrInh)) continue;
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)pattr.at().flgSelf());
	if(!(attr.at().flgGlob()&Attr::DirRead))
	    switch(attr.at().type())
	    {
		case TFld::Boolean:	attr.at().setB(pattr.at().getB(), attr.at().flgGlob()&Attr::Active);	break;
		case TFld::Integer:	attr.at().setI(pattr.at().getI(), attr.at().flgGlob()&Attr::Active);	break;
		case TFld::Real:	attr.at().setR(pattr.at().getR(), attr.at().flgGlob()&Attr::Active);	break;
		case TFld::String:	attr.at().setS(pattr.at().getS(), attr.at().flgGlob()&Attr::Active);	break;
		default: break;
	    }
	// No inherit calc flag for links
	if(isLink() && !parent().at().isLink())
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)(attr.at().flgSelf()&(~Attr::ProcAttr)));
	attr.at().setCfgTempl(pattr.at().cfgTempl());
	attr.at().setCfgVal(pattr.at().cfgVal());
	attr.at().setModif(0);
    }

    setStlLock(false);
    if(!loadDef) parent().at().setStlLock(false);
}

void Widget::inheritIncl( const string &iwdg )
{
    AutoHD<Widget> parw = parent();
    while(!parw.freeStat() && parw.at().isLink()) parw = parw.at().parent();
    if(parw.freeStat()) return;

    //Create no present include widgets for no link and container widgets
    vector<string>  ls;
    if(!iwdg.empty() && parw.at().wdgPresent(iwdg)) ls.push_back(iwdg);
    else parw.at().wdgList(ls);
    for(unsigned i_w = 0; i_w < ls.size(); i_w++)
	if(!wdgPresent(ls[i_w]))
	    try{ wdgAdd(ls[i_w],"",parw.at().wdgAt(ls[i_w]).at().path(),true); }
	    catch(TError err){ mess_err(err.cat.c_str(),err.mess.c_str()); }
}

void Widget::wClear( )
{
    //Generic clear
    setIco("");

    //Inherit modified attributes
    vector<string> ls;
    attrList(ls);
    for(unsigned i_a = 0; i_a < ls.size(); i_a++) {
	if(!attrPresent(ls[i_a])) continue;
	AutoHD<Attr> attr = attrAt(ls[i_a]);
	if(attr.at().modif()) {
	    attr.at().setModif(0);
	    inheritAttr(ls[i_a]);
	}
    }
    //Check for included widgets
    if(isContainer() && !isLink()) {
	AutoHD<Widget> parw = parent();
	while(!parw.freeStat() && parw.at().isLink())
	    parw = parw.at().parent();
	if(!parw.freeStat()) {
	    //Check for widget's deletion
	    wdgList(ls);
	    for(unsigned i_w = 0; i_w < ls.size(); i_w++)
		if(!parw.at().wdgPresent(ls[i_w]))
		    wdgDel(ls[i_w],true);

	    //No present widget's add and clear call
	    parw.at().wdgList(ls);
	    for(unsigned i_w = 0; i_w < ls.size(); i_w++)
		if(!wdgPresent(ls[i_w]))
		    try{ wdgAdd(ls[i_w],"",parw.at().wdgAt(ls[i_w]).at().path(),true); }
		    catch(TError err){ mess_err(err.cat.c_str(),err.mess.c_str()); }
		else wdgAt(ls[i_w]).at().wClear();
	}
    }
    modif();
}

void Widget::wChDown( const string &ia )
{
    AutoHD<Widget> parw = parent();
    if(parw.freeStat()) return;

    //Generic clear
    if(ia.empty() && ico() != parw.at().ico()) { parw.at().setIco(ico()); setIco(""); }

    //Put down modified attributes
    vector<string> ls;
    if(ia.empty()) attrList(ls); else ls.push_back(ia);
    AutoHD<Attr> attr;
    for(unsigned i_a = 0; i_a < ls.size(); i_a++) {
	if(!attrPresent(ls[i_a]) || !(attr=attrAt(ls[i_a])).at().modif()) continue;
	//!!!! Check for type and generic flags changes
	if(!parw.at().attrPresent(ls[i_a])) {
	    if(!(attr.at().flgGlob()&Attr::IsUser)) continue;
	    parw.at().attrAdd(new TFld(ls[i_a].c_str(),"",attr.at().type(),attr.at().fld().flg()));
	}
	AutoHD<Attr> attrP = parw.at().attrAt(ls[i_a]);
	attrP.at().set(attr.at().get());
	attrP.at().setFlgSelf(attr.at().flgSelf());
	attrP.at().setCfgVal(attr.at().cfgVal());
	attrP.at().setCfgTempl(attr.at().cfgTempl());
	attr.at().setModif(0);
	inheritAttr(ls[i_a]);
    }

    //Check for included widgets
    if(ia.empty() && isContainer() && !isLink()) {
	//Check for widget's deletion
	wdgList(ls);
	for(unsigned i_w = 0; i_w < ls.size(); i_w++) {
	    if(!parw.at().wdgPresent(ls[i_w]))
		try {
		    parw.at().wdgAdd(ls[i_w],"","");
		    (TCntrNode&)parw.at().wdgAt(ls[i_w]).at() = (TCntrNode&)wdgAt(ls[i_w]).at();
		}
		catch(TError err){ mess_err(err.cat.c_str(),err.mess.c_str()); }
	    else wdgAt(ls[i_w]).at().wChDown();
	}

	//No present widget's add and clear call
	parw.at().wdgList(ls);
	for(unsigned i_w = 0; i_w < ls.size(); i_w++)
	    if(!wdgPresent(ls[i_w]))
		parw.at().wdgDel(ls[i_w],true);
    }

    modif();
}

void Widget::attrList( vector<string> &list )
{
    pthread_mutex_lock(&mtxAttr());
    list.clear();
    list.reserve(mAttrs.size());
    for(map<string, Attr* >::iterator p = mAttrs.begin(); p != mAttrs.end(); ++p)
    {
	while(p->second->mOi >= list.size())	list.push_back("");
	list[p->second->mOi] = p->first;
    }
    pthread_mutex_unlock(&mtxAttr());
}

void Widget::attrAdd( TFld *attr, int pos, bool inher, bool forceMdf )
{
    string anm = attr->name();

    if(attrPresent(anm) || TSYS::strNoSpace(anm).empty()) {
	if(!inher) delete attr;
	return;
    }
    try {
	pthread_mutex_lock(&mtxAttr());
	if(mAttrs.size() >= ((1<<ATTR_OI_DEPTH)-1)) {
	    if(!inher) delete attr;
	    pthread_mutex_unlock(&mtxAttr());
	    mess_err(nodePath().c_str(),_("Adding a new attribute '%s' number more to %d!"),anm.c_str(),(1<<ATTR_OI_DEPTH)-1);
	    return;
	}
	map<string, Attr* >::iterator p;
	Attr *a = new Attr(attr, inher);
	a->mOwner = this;
	pos = (pos < 0 || pos > (int)mAttrs.size()) ? (int)mAttrs.size() : pos;
	a->mOi = pos;
	for(p = mAttrs.begin(); p != mAttrs.end(); p++)
	    if(p->second->mOi >= pos) p->second->mOi++;
	mAttrs.insert(std::pair<string,Attr*>(a->id(),a));
	//Set modif for new attribute reload allow
	if(forceMdf) a->setModif(modifVal(*a));
    }
    catch(...){ }
    pthread_mutex_unlock(&mtxAttr());
}

void Widget::attrDel( const string &attr, bool allInher  )
{
    if(!attrPresent(attr)) return;

    //Delete from inheritant wigets
    if(!(attrAt(attr).at().flgGlob()&Attr::Mutable) || allInher)
	for(unsigned i_h = 0; i_h < m_herit.size(); i_h++)
	    if(m_herit[i_h].at().enable())
		m_herit[i_h].at().attrDel(attr);

    //Self delete
    try {
	pthread_mutex_lock(&mtxAttr());
	map<string, Attr* >::iterator p = mAttrs.find(attr);
	if(p == mAttrs.end())	throw TError(nodePath().c_str(), _("Attribute '%s' is not present!"), attr.c_str());
	for(int i_c = 0; i_c < 100 && p->second->mConn; i_c++)	TSYS::sysSleep(0.01);
	if(p->second->mConn) throw TError(nodePath().c_str(), _("Deleted attribute '%s' was not free!"), attr.c_str());

	int pos = p->second->mOi;
	for(map<string, Attr* >::iterator p1 = mAttrs.begin(); p1 != mAttrs.end(); ++p1)
	    if(p1->second->mOi > pos) p1->second->mOi--;
	delete p->second;
	mAttrs.erase(p);
    }
    catch(...){ }
    pthread_mutex_unlock(&mtxAttr());
}

bool Widget::attrPresent( const string &attr )
{
    pthread_mutex_lock(&mtxAttr());
    bool rez = (mAttrs.find(attr) != mAttrs.end());
    pthread_mutex_unlock(&mtxAttr());
    return rez;
}

AutoHD<Attr> Widget::attrAt( const string &attr, int lev )
{
    //Local atribute request
    if(lev < 0) {
	pthread_mutex_lock(&mtxAttr());
	map<string, Attr* >::iterator p = mAttrs.find(attr);
	if(p == mAttrs.end()) {
	    pthread_mutex_unlock(&mtxAttr());
	    throw TError(nodePath().c_str(),_("Attribute '%s' is not present!"), attr.c_str());
	}
	AutoHD<Attr> rez(p->second);
	pthread_mutex_unlock(&mtxAttr());
	return rez;
    }
    //Process by full path
    AutoHD<Attr> an;
    size_t waSep = attr.rfind("/");
    if(waSep == string::npos) return attrPresent(attr) ? attrAt(attr) : an;
    string anm = attr.substr(waSep+1);
    if(anm.compare(0,2,"a_") == 0) anm = anm.substr(2);
    AutoHD<Widget> wn = wdgAt(attr.substr(0,waSep),lev);
    if(wn.freeStat() || !wn.at().attrPresent(anm)) return an;
    return wn.at().attrAt(anm);
}

int Widget::attrPos( const string &inm )	{ return attrAt(inm).at().mOi; }

void Widget::disable( Widget *base )
{
    if(!parent().freeStat()) parent().at().disable(base);
}

void Widget::calc( Widget *base )
{
    if(!parent().freeStat()) parent().at().calc(base);
}

TVariant Widget::objFuncCall_w( const string &id, vector<TVariant> &prms, const string &user, Widget *src )
{
    if(!parent().freeStat()) return parent().at().objFuncCall_w(id, prms, user, src);

    return TVariant();
}

bool Widget::attrChange( Attr &cfg, TVariant prev )
{
    //Process Active attribute's mode
    if(cfg.flgGlob()&Attr::Active && !prev.isNull() && !parent().freeStat())	parent().at().attrChange(cfg,prev);
    if(cfg.owner() != this) return false;

    //Update heritors attributes
    for(unsigned i_h = 0; i_h < m_herit.size(); i_h++)
	if(m_herit[i_h].at().enable())
	    m_herit[i_h].at().inheritAttr(cfg.id());

    return true;
}

void Widget::wdgList( vector<string> &list, bool fromLnk )
{
    if(fromLnk && isLink()) parent().at().wdgList(list);
    else chldList(inclWdg, list);
}

bool Widget::wdgPresent( const string &wdg )	{ return chldPresent(inclWdg, wdg); }

void Widget::wdgAdd( const string &wid, const string &name, const string &path, bool force )
{
    if(!isContainer())  throw TError(nodePath().c_str(),_("Widget is not container!"));
    if(wdgPresent(wid)) return;

    chldAdd(inclWdg, new Widget(wid,path));
    wdgAt(wid).at().setName(name);

    //Call heritors include widgets update
    for(unsigned i_h = 0; i_h < m_herit.size(); i_h++)
	if(m_herit[i_h].at().enable())
	    m_herit[i_h].at().inheritIncl(wid);
}

AutoHD<Widget> Widget::wdgAt( const string &wdg, int lev, int off )
{
    if(lev < 0) return chldAt(inclWdg, wdg);

    AutoHD<Widget> rez;
    string iw = TSYS::pathLev(wdg,lev,true,&off);
    if(iw.compare(0,4,"wdg_") == 0) iw = iw.substr(4);
    if(iw.empty())	rez = AutoHD<Widget>(this);
    else if(iw == "..") {
	if(dynamic_cast<Widget*>(nodePrev())) rez = ((Widget*)nodePrev())->wdgAt(wdg, 0, off);
    }
    else if(isLink()) {
	if(parent().at().wdgPresent(iw)) rez = parent().at().wdgAt(iw).at().wdgAt(wdg, 0, off);
    }
    else if(wdgPresent(iw)) rez = wdgAt(iw).at().wdgAt(wdg, 0, off);

    return rez;
}

string Widget::helpImg( )
{
    return _("Image name in form \"[src:]name\", where:\n"
	    "  \"src\" - image source:\n"
	    "    file - direct from local file by path;\n"
	    "    res - from DB mime resources table.\n"
	    "  \"name\" - file path or resource mime Id.\n"
	    "Examples:\n"
	    "  \"res:backLogo\" - from DB mime resources table for Id \"backLogo\";\n"
	    "  \"backLogo\" - like previous;\n"
	    "  \"file:/var/tmp/backLogo.png\" - from local file by path \"/var/tmp/backLogo.png\".");
}

string Widget::helpColor( )
{
    return _("Color name form \"color[-alpha]\", where:\n"
	    "  \"color\" - standard color name or digital view of three hexadecimal digit's number form \"#RRGGBB\";\n"
	    "  \"alpha\" - alpha channel level (0-255).\n"
	    "Examples:\n"
	    "  \"red\" - solid red color;\n"
	    "  \"#FF0000\" - solid red color by digital code;\n"
	    "  \"red-127\" - half transparent red color.");
}

string Widget::helpFont( )
{
    return _("Font name form \"{family} {size} {bold} {italic} {underline} {strike}\", where:\n"
	    "  \"family\" - font family, for spaces use symbol '_', like: \"Arial\", \"Courier\", \"Times_New_Roman\";\n"
	    "  \"size\" - font size in pixels;\n"
	    "  \"bold\" - font bold (0 or 1);\n"
	    "  \"italic\" - font italic (0 or 1);\n"
	    "  \"underline\" - font underlined (0 or 1);\n"
	    "  \"strike\" - font striked (0 or 1).\n"
	    "Examples:\n"
	    "  \"Arial 10 1 0 0 0\" - Arial font size 10 pixels and bolded.");
}

TVariant Widget::vlGet( Attr &a )
{
    if(a.owner() == this) {
	if(a.id() == "id")		return TVariant(id());
	else if(a.id() == "path")	return TVariant(path());
	else if(a.id() == "root")	return TVariant(rootId());
	else if(a.id() == "parent")	return TVariant(parentNm());
    }

    if(!parent().freeStat())		return parent().at().vlGet(a);

    return TVariant();
}

TVariant Widget::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    return (!stlLock() && dynamic_cast<Widget*>(nodePrev())) ? ((Widget*)nodePrev())->stlReq(a, vl, wr) : vl;
}

bool Widget::eventProc( const string &ev, Widget *src )
{
    return parent().freeStat() ? false : parent().at().eventProc(ev, src?src:this);
}

bool Widget::cntrCmdServ( XMLNode *opt )
{
    string tNm;
    string a_path = opt->attr("path"), u = opt->attr("user");
    //Service commands process
    if(a_path == "/serv/attr") {	//Attribute's access
	if(ctrChkNode(opt,"get",RWRWRW,"root",SUI_ID,SEC_RD)) {		//Get values
	    AutoHD<Attr> attr;
	    if(!opt->childSize()) {
		vector<string> ls;
		attrList(ls);
		for(unsigned i_l = 0; i_l < ls.size(); i_l++) {
		    attr = attrAt(ls[i_l]);
		    if(attr.at().flgGlob()&Attr::IsUser) continue;
		    opt->childAdd("el")->setAttr("id",ls[i_l].c_str())->
					 setAttr("p",attr.at().fld().reserve())->
					 setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
		}
	    }
	    else
		for(unsigned i_l = 0; i_l < opt->childSize(); i_l++)
		    if(attrPresent(tNm=opt->childGet(i_l)->attr("id"))) {
			attr = attrAt(tNm);
			opt->childGet(i_l)->setAttr("p",attr.at().fld().reserve())->
					    setAttr("act",(attr.at().flgGlob()&Attr::Active)?"1":"0")->
					    setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
		    }
	}
	else if(ctrChkNode(opt,"set",RWRWRW,"root",SUI_ID,SEC_WR))	//Set values
	    for(unsigned i_ch = 0; i_ch < opt->childSize(); i_ch++)
		try{ attrAt(opt->childGet(i_ch)->attr("id")).at().setS(opt->childGet(i_ch)->text()); }
		catch(TError) { }
    }
    else if(a_path == "/serv/attrBr" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID,SEC_RD))	//Get attributes all updated elements' of the branch
    {
	// Self attributes put
	vector<string> ls;
	attrList(ls);
	AutoHD<Attr> attr;
	for(unsigned i_l = 0; i_l < ls.size(); i_l++) {
	    attr = attrAt(ls[i_l]);
	    if(attr.at().flgGlob()&Attr::IsUser) continue;
	    opt->childAdd("el")->setAttr("id",ls[i_l].c_str())->
			     setAttr("p",attr.at().fld().reserve())->
			     setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
	}
	// Child widgets process
	if(enable()) {
	    AutoHD<Widget> iwdg;
	    vector<string>	lst;
	    if(isLink()) {
		parentNoLink().at().wdgList(lst);
		opt->setAttr("lnkPath",parentNoLink().at().path());
	    }
	    else wdgList(lst);
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++) {
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
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget: ")+id(),RWRWR_,"root",SUI_ID);
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widget"))) {
	    if(ctrMkNode("area",opt,-1,"/wdg/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enable"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/wdg/w_lst");
		if(!parent().freeStat())
		    ctrMkNode("comm",opt,-1,"/wdg/st/goparent",_("Go to parent"),RWRWR_,"root",SUI_ID,1,"tp","lnk");
	    }
	    if(ctrMkNode("area",opt,-1,"/wdg/cfg",_("Configuration"))) {
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
		ctrMkNode("comm",opt,-1,"/wdg/cfg/clear",_("Clear the widget changes"),RWRWR_,"root",SUI_ID);
		ctrMkNode("comm",opt,-1,"/wdg/cfg/chDown",_("Put down to the parent for the widget changes"),RWRWR_,"root",SUI_ID);
	    }
	}
	if(isContainer() && (!isLink()) && ctrMkNode("area",opt,-1,"/inclwdg",_("Include widgets"))) {
	    ctrMkNode("fld",opt,-1,"/inclwdg/nmb",_("Number"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","wdg_","idSz","30");
	}
	if(isContainer() && (!isLink()) && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user");
    if(a_path == "/wdg/res" && ctrChkNode(opt)) {	//Service command for resources request
	string mime;
	opt->setText(resourceGet(opt->attr("id"),&mime));
	opt->setAttr("mime",mime);
    }
    else if(a_path == "/wdg/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/wdg/st/parent") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(parentNm());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setParentNm(opt->text());
    }
    else if(a_path == "/wdg/st/goparent" && ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD) && !parent().freeStat())
	opt->setText(parent().at().nodePath(0,true));
    else if(a_path == "/wdg/cfg/owner") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(owner());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setOwner(opt->text());
    }
    else if(a_path == "/wdg/cfg/grp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(grp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setGrp(opt->text());
    }
    else if(a_path == "/wdg/cfg/u_a" || a_path == "/wdg/cfg/g_a" || a_path == "/wdg/cfg/o_a") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(a_path == "/wdg/cfg/u_a")	opt->setText(i2s((permit()>>6)&0x7));
	    if(a_path == "/wdg/cfg/g_a")	opt->setText(i2s((permit()>>3)&0x7));
	    if(a_path == "/wdg/cfg/o_a")	opt->setText(i2s(permit()&0x7));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(a_path == "/wdg/cfg/u_a")	setPermit((permit()&(~(0x07<<6)))|(s2i(opt->text())<<6));
	    if(a_path == "/wdg/cfg/g_a")	setPermit((permit()&(~(0x07<<3)))|(s2i(opt->text())<<3));
	    if(a_path == "/wdg/cfg/o_a")	setPermit((permit()&(~0x07))|s2i(opt->text()));
	}
    }
    else if(a_path == "/wdg/cfg/ico" || a_path == "/ico") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(ico());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setIco(opt->text());
    }
    else if(a_path == "/wdg/cfg/id" && ctrChkNode(opt))		opt->setText(id());
    else if(a_path == "/wdg/cfg/type" && ctrChkNode(opt))	opt->setText(type());
    else if(a_path == "/wdg/cfg/root" && ctrChkNode(opt))	opt->setText(rootId());
    else if(a_path == "/wdg/cfg/path" && ctrChkNode(opt))
	opt->setText((isLink()&&s2i(opt->attr("resLink"))) ? parentNoLink().at().path() : path());
    else if(a_path == "/wdg/cfg/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trU(name(),u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(trSetU(name(),u,opt->text()));
    }
    else if(a_path == "/wdg/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trU(descr(),u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(trSetU(descr(),u,opt->text()));
    }
    else if(a_path == "/wdg/cfg/clear" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID)) {
	if(opt->attr("attr").empty()) wClear();
	else if(attrAt(opt->attr("attr")).at().modif()) {
	    attrAt(opt->attr("attr")).at().setModif(0);
	    inheritAttr(opt->attr("attr"));
	    modif();
	}
    }
    else if(a_path == "/wdg/cfg/chDown" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID)) wChDown(opt->attr("attr"));
    else if(a_path == "/wdg/u_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/wdg/g_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrGrpList(owner(), ls );
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/wdg/w_lst" && ctrChkNode(opt)) {
	int c_lv = 0;
	string c_path = "", c_el;
	string lnk = parentNm();

	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::pathLev(lnk,0,true,&c_off)).size(); c_lv++) {
	    c_path += "/"+c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	vector<string>	ls, tls;
	try {
	    switch(c_lv) {
		case 0:
		    ls.clear();
		    mod->nodeList(tls,"wlb_");
		    for(unsigned i_v = 0; i_v < tls.size(); i_v++) ls.push_back(tls[i_v]);
		    mod->nodeList(tls,"prj_");
		    for(unsigned i_v = 0; i_v < tls.size(); i_v++) ls.push_back(tls[i_v]);
		    //mod->nodeList(ls);
		    break;
		case 1:
		    if(TSYS::pathLev(lnk,0) != "..")
			mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeList(ls);
		    break;
		case 2:
		    if(TSYS::pathLev(lnk,0) != "..")
			mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeAt(TSYS::pathLev(lnk,1)).at().nodeList(ls,"wdg_");
		    break;
	    }
	    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
		opt->childAdd("el")->setText(c_path+"/"+ls[i_l]);
	}catch(TError err) { }
    }
    else if(a_path == "/br/wdg_" || a_path == "/inclwdg/wdg") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    bool chkUserPerm = s2i(opt->attr("chkUserPerm"));
	    if(!chkUserPerm || SYS->security().at().access(opt->attr("user"),SEC_RD,owner(),grp(),permit())) {
		vector<string>  lst;
		wdgList(lst);
		for(unsigned i_f = 0; i_f < lst.size(); i_f++) {
		    AutoHD<Widget> iwdg = wdgAt(lst[i_f]);
		    if(chkUserPerm && !SYS->security().at().access(opt->attr("user"),SEC_RD,iwdg.at().owner(),iwdg.at().grp(),iwdg.at().permit()))
			continue;
		    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(trU(iwdg.at().name(),u));
		}
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string sid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    wdgAdd(sid.c_str(),opt->text(),"");
	    opt->setAttr("id", sid);
	    //GeomZ set to include widgets number
	    if(wdgPresent(sid)) {
		vector<string> c_list;
		wdgList(c_list);
		wdgAt(sid).at().attrAt("geomZ").at().setI(c_list.size()-1);
	    }
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	wdgDel(opt->attr("id").c_str(),true);
    }
    else if(a_path == "/inclwdg/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	wdgList(c_list);
	unsigned e_c = 0;
	for(unsigned i_w = 0; i_w < c_list.size(); i_w++)
	    if(wdgAt(c_list[i_w]).at().enable()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else return false;

    return true;
}

bool Widget::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    if(!parent().freeStat()) return parent().at().cntrCmdAttributes(opt, src);

    //Get page info
    if(opt->name() == "info") {
	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes"))) {
	    // Properties form create
	    vector<string> list_a;
	    src->attrList(list_a);
	    for(unsigned i_el = 0; i_el < list_a.size(); i_el++) {
		AutoHD<Attr> attr = src->attrAt(list_a[i_el]);
		XMLNode *el = attr.at().fld().cntrCmdMake(opt,"/attr",-1,"root",SUI_ID,RWRWR_);
		if(el) {
		    el->setAttr("len","")->setAttr("wdgFlg",i2s(attr.at().flgGlob()))->
			setAttr("modif",u2s(attr.at().modif()))->setAttr("p",attr.at().fld().reserve());
		    if(list_a[i_el] == "path")		el->setAttr("help",_("Path to the widget."));
		    else if(list_a[i_el] == "parent")	el->setAttr("help",_("Path to parent widget."));
		    else if(list_a[i_el] == "owner")	el->setAttr("help",_("The widget owner and group in form \"[owner]:[group]\"."));
		    else if(list_a[i_el] == "perm")
			el->setAttr("help",_("Permission to the widget in form \"[user][group][other]\".\n"
					     "Where \"user\", \"group\" and \"other\" is:\n"
					     "  \"__\" - no any access;\n"
					     "  \"R_\" - read only;\n"
					     "  \"RW\" - read and write."));
		    else if(list_a[i_el] == "evProc")
			el->setAttr("SnthHgl","1")->
			    setAttr("help",_("Direct events processing for pages manipulation in form:\n"
					     "      \"[event]:[evSrc]:[com]:[prm]\". Where:\n"
					     "  \"event\" - waiting event;\n"
					     "  \"evSrc\" - event source;\n"
					     "  \"com\" - command of a session (open, next, prev);\n"
					     "  \"prm\" - command parameter, where used:\n"
					     "      pg_so - direct name of the desired page with the prefix;\n"
					     "      1 - name of a new page in a general way, without a prefix;\n"
					     "      * - the page is taken from the name of a previous page;\n"
					     "      $ - points the place of the opened page relative.\n"
					     "Examples:\n"
					     "  ws_BtPress:/prev:prev:/pg_so/*/*/$\n"
					     "  ws_BtPress:/next:next:/pg_so/*/*/$\n"
					     "  ws_BtPress:/go_mn:open:/pg_so/*/mn/*\n"
					     "  ws_BtPress:/go_graph:open:/pg_so/*/ggraph"));
		    else switch(s2i(attr.at().fld().reserve())) {
			case A_ROOT:
			    el->setAttr("help",_("Primitive identifier in the widget ground."));
			    break;
			case A_CTX_MENU:
			    el->setAttr("SnthHgl","1")->
				setAttr("help",_("Context menu in form strings list: \"[ItName]:[Signal]\".\n"
						 "Where:\n"
						 "  \"ItName\" - item name;\n"
						 "  \"Signal\" - signal name and result signal name is \"usr_[Signal]\"."));
			    break;
		    }
		    if(attr.at().type() == TFld::String && attr.at().flgGlob()&Attr::Image)
			el->setAttr("dest","sel_ed")->setAttr("select","/attrImg/sel_"+list_a[i_el]);
		}
	    }
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user");
    if(a_path.compare(0,6,"/attr/") == 0) {
	AutoHD<Attr> attr = src->attrAt(TSYS::pathLev(a_path,1));
	if(ctrChkNode(opt,"get",(attr.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
	else if(ctrChkNode(opt,"set",(attr.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SUI_ID,SEC_WR)) {
	    attr.at().setS(attr.at().isTransl()?trSetU(attr.at().getS(),u,opt->text()):opt->text());
	    opt->setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
	}
	else if(attr.at().id() == "contextMenu" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    opt->childAdd("rule")->setAttr("expr", ":.*$")->setAttr("color", "darkorange");
	    opt->childAdd("rule")->setAttr("expr", "^.*:")->setAttr("color", "darkgreen");
	}
	else if(attr.at().id() == "evProc" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    opt->childAdd("rule")->setAttr("expr",":(open|next|prev):")->setAttr("color","darkgreen");
	    /*opt->childAdd("rule")->setAttr("expr",":([^:]+|\\z)")->setAttr("color","darkblue")->
		 childAdd("rule")->setAttr("expr","\\w+")->setAttr("color","blue");*/
	    opt->childAdd("rule")->setAttr("expr","^[^:]*:")->setAttr("color","darkorange");
	}
    }
    else if(a_path.compare(0,13,"/attrImg/sel_") == 0 && ctrChkNode(opt)) {
	string a_val = src->attrAt(a_path.substr(13)).at().getS();
	if(a_val == "res:") {
	    vector<string> ls;
	    src->resourceList(ls);
	    for(unsigned i_t = 0; i_t < ls.size(); i_t++)
		opt->childAdd("el")->setText("res:"+ls[i_t]);
	}
	else if(a_val.compare(0,5,"file:") == 0) {
	    TSYS::ctrListFS(opt, a_val.substr(5), "png;jpeg;jpg;gif;pcx;mng;svg;");
	    for(unsigned i_t = 0; i_t < opt->childSize(); i_t++)
		opt->childGet(i_t)->setText("file:"+opt->childGet(i_t)->text());
	}
	opt->childIns(0,"el")->setText("res:");
	opt->childIns(1,"el")->setText("file:");

    }
    else return false;

    return true;
}

bool Widget::cntrCmdLinks( XMLNode *opt, bool lnk_ro )
{
    //Get page info
    if(opt->name() == "info") {
	if(ctrMkNode("area",opt,-1,"/links",_("Links"))) {
	    ctrMkNode("fld",opt,-1,"/links/showAttr",_("Attributes show"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links"))) {
		bool shwAttr =	s2i(opt->attr("showAttr")) ||
				s2i(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")));
		vector<string> incllist, alist, list;
		wdgList(incllist);
		for(int i_w = -1; i_w < (int)incllist.size(); i_w++) {
		    AutoHD<Widget> wdg;
		    if(i_w < 0) wdg = AutoHD<Widget>(this);
		    else wdg = wdgAt(incllist[i_w]);
		    wdg.at().attrList(alist);
		    for(unsigned i_a = 0; i_a < alist.size(); i_a++) {
			string grpprm;
			string idprm = alist[i_a];
			string nprm  = wdg.at().attrAt(alist[i_a]).at().id();
			if(i_w >= 0) {
			    idprm.insert(0,incllist[i_w]+".");
			    nprm.insert(0,wdg.at().id()+".");
			}

			if(!(wdg.at().attrAt(alist[i_a]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst)) ||
			    (!shwAttr && wdg.at().attrAt(alist[i_a]).at().flgSelf()&Attr::CfgConst)) continue;
			// Get attributes
			bool shwTmpl = wdg.at().attrAt(alist[i_a]).at().cfgTempl().size();
			if(shwTmpl)	grpprm = TSYS::strSepParse(wdg.at().attrAt(alist[i_a]).at().cfgTempl(),0,'|');

			// Check select param
			if(shwTmpl && !shwAttr) {
			    nprm = grpprm;
			    if(i_w >= 0) nprm.insert(0,wdg.at().id()+".");

			    // Check already to present parameters
			    bool f_ok = false;
			    for(unsigned i_l = 0; i_l < list.size() && !f_ok; i_l++)
				if(list[i_l] == nprm) f_ok = true;
			    if(!f_ok) {
				ctrMkNode("fld",opt,-1,(string("/links/lnk/pr_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,
				    3,"tp","str","dest","sel_ed","select",(string("/links/lnk/pl_")+idprm).c_str());
				list.push_back(nprm);
			    }
			}
			else {
			    XMLNode *nel = NULL;
			    if(wdg.at().attrAt(alist[i_a]).at().flgSelf()&Attr::CfgConst)
				nel = ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,2,"tp","str","elGrp",grpprm.c_str());
			    else
				nel = ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),
				    nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,4,"tp","str","dest","sel_ed","select",(string("/links/lnk/ls_")+idprm).c_str(),"elGrp",grpprm.c_str());
			    if(nel && s2i(opt->attr("inclValue"))) {
				nel->setText(wdg.at().attrAt(alist[i_a]).at().cfgVal());
				if(wdg.at().attrAt(alist[i_a]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
				    ((nel->text().compare(0,4,"prm:") == 0 && !SYS->daq().at().attrAt(TSYS::strParse(nel->text().substr(4),0,"#"),0,true).freeStat()) ||
				     (nel->text().compare(0,4,"wdg:") == 0 && !wdg.at().attrAt(nel->text().substr(4),0).freeStat()) ||
				     (nel->text().compare(0,4,"arh:") == 0 && SYS->archive().at().valPresent(nel->text().substr(4)))))
				    nel->setText(nel->text() + " (+)");
			    }
			}
		    }
		}
	    }
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/links/showAttr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"showAttr",opt->text(),opt->attr("user"));
    }
    else if(a_path.compare(0,14,"/links/lnk/pr_") == 0) {
	vector<string> a_ls;
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size())	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	// Search first not config field if default field is config.
	if(srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst) {
	    srcwdg.at().attrList(a_ls);
	    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[i_a]; break; }
	}

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    string cfg_val = srcwdg.at().attrAt(nattr).at().cfgVal();
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,':')+":";
	    string rez     = _("Custom");

	    bool custom = false, lnkOK = false;
	    if(obj_tp == "prm:" || obj_tp == "wdg:") {
		if((obj_tp == "prm:" && !SYS->daq().at().attrAt(cfg_val.substr(4),0,true).freeStat()) ||
		    (obj_tp == "wdg:" && !srcwdg.at().attrAt(cfg_val.substr(4),0).freeStat()))
		    { cfg_val.resize(cfg_val.rfind("/")); lnkOK = true; }
	    }else custom = true;

	    string sel;
	    srcwdg.at().attrList(a_ls);
	    rez += ": ";
	    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{
		    sel = srcwdg.at().attrAt(a_ls[i_a]).at().cfgVal();
		    if(!custom && sel.find(cfg_val) != 0) custom = true;
		    rez += sel+", ";
		}
	    if(cfg_val.empty())	rez = "";
	    else if(!custom) {
		rez = cfg_val;
		if(lnkOK) rez += " (+)";
	    }

	    opt->setText(rez);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string no_set;
	    string cfg_val = TSYS::strParse(opt->text(), 0, " ");
	    string obj_tp  = TSYS::strSepParse(cfg_val,0,':')+":";
	    string cfg_addr = (obj_tp.size()<cfg_val.size()) ? cfg_val.substr(obj_tp.size()) : "";

	    AutoHD<TValue> prm;
	    if(obj_tp == "prm:")	prm = SYS->daq().at().prmAt(cfg_addr,0,true);
	    AutoHD<Widget> dstwdg;
	    if(obj_tp == "wdg:" && cfg_addr.size())	dstwdg = srcwdg.at().wdgAt(cfg_addr,0);

	    srcwdg.at().attrList(a_ls);
	    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{
		    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val);
		    string p_attr = TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),1,'|');
		    if(!prm.freeStat() || !dstwdg.freeStat()) {
			if((!prm.freeStat() && prm.at().vlPresent(p_attr)) ||
				(!dstwdg.freeStat() && dstwdg.at().attrPresent(p_attr)))
			    srcwdg.at().attrAt(a_ls[i_a]).at().setCfgVal(cfg_val+((obj_tp == "wdg:")?"/a_":"/")+p_attr);
			else no_set += p_attr+",";
		    }
		}
	}
    }
    else if((a_path.compare(0,14,"/links/lnk/pl_") == 0 || a_path.compare(0,14,"/links/lnk/ls_") == 0) && ctrChkNode(opt)) {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size()) srcwdg = wdgAt(nwdg);
	else nattr = nwdg;

	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
	if(!(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut))) {
	    if(!is_pl) throw TError(nodePath().c_str(),_("Variable is not link"));
	    vector<string> a_ls;
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    srcwdg.at().attrList(a_ls);
	    unsigned i_a;
	    for(i_a = 0; i_a < a_ls.size(); i_a++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[i_a]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[i_a]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[i_a]; break; }
	    if(i_a >= a_ls.size()) throw TError(nodePath().c_str(),_("Variable is not link"));
	}

	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();

	// Link interface process
	int c_lv = 0;
	string obj_tp = TSYS::strSepParse(m_prm,0,':')+":";
	if(obj_tp.empty() || !(obj_tp == "val:" || obj_tp == "prm:" || obj_tp == "wdg:" || obj_tp == "arh:"))
	{
	    if(!is_pl) opt->childAdd("el")->setText(_("val:Constant value"));
	    opt->childAdd("el")->setText("prm:");
	    opt->childAdd("el")->setText("wdg:");
	    if(!is_pl && srcwdg.at().attrAt(nattr).at().flgGlob()&Attr::Address)
		opt->childAdd("el")->setText("arh:");
	}
	// Link elements process
	else {
	    int c_off = obj_tp.size();
	    vector<string> ls;
	    string c_path = obj_tp, c_el;
	    opt->childAdd("el")->setText("");

	    try {
		if(obj_tp == "prm:") {
		    m_prm = m_prm.substr(4);
		    if(is_pl && !SYS->daq().at().attrAt(m_prm,0,true).freeStat()) m_prm = m_prm.substr(0,m_prm.rfind("/"));
		    SYS->daq().at().ctrListPrmAttr(opt, m_prm, is_pl, 0, "prm:");
		}
		else if(obj_tp == "wdg:") {
		    opt->childAdd("el")->setText(c_path);
		    for( ;(c_el=TSYS::pathLev(m_prm,0,true,&c_off)).size(); c_lv++) {
			c_path += (c_lv?"/":"")+c_el;
			opt->childAdd("el")->setText(c_path);
		    }

		    AutoHD<Widget> wnd = srcwdg.at().wdgAt(c_path.substr(4),0);
		    if(!wnd.freeStat()) {
			if(dynamic_cast<Widget*>(wnd.at().nodePrev())) opt->childAdd("el")->setText(c_path+(c_lv?"/..":".."));
			wnd.at().wdgList(ls, true);
			if(ls.size()) opt->childAdd("el")->setText(_("=== Widgets ==="));
			for(unsigned i_l = 0; i_l < ls.size(); i_l++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/wdg_":"wdg_")+ls[i_l]);
			if(!is_pl) {
			    wnd.at().attrList(ls);
			    if(ls.size()) opt->childAdd("el")->setText(_("=== Attributes ==="));
			    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
				opt->childAdd("el")->setText(c_path+(c_lv?"/a_":"a_")+ls[i_l]);
			}
		    }
		}
		else if(m_prm == "arh:") {
		    SYS->archive().at().valList(ls);
		    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
			opt->childAdd("el")->setText(c_path+ls[i_l]);
		}
	    }catch(TError err) { }
	}
    }
    else if(a_path.compare(0,14,"/links/lnk/el_") == 0) {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size())	srcwdg = wdgAt(nwdg);
	else nattr = nwdg;

	if(ctrChkNode(opt,"get",RWRWR_,"root","UI",SEC_RD)) {
	    opt->setText(srcwdg.at().attrAt(nattr).at().cfgVal());
	    if(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
		    ((opt->text().compare(0,4,"prm:") == 0 && !SYS->daq().at().attrAt(TSYS::strParse(opt->text().substr(4),0,"#"),0,true).freeStat()) ||
		     (opt->text().compare(0,4,"wdg:") == 0 && !srcwdg.at().attrAt(opt->text().substr(4),0).freeStat()) ||
		     (opt->text().compare(0,4,"arh:") == 0 && SYS->archive().at().valPresent(opt->text().substr(4)))))
		opt->setText(opt->text() + " (+)");
	}
	else if(ctrChkNode(opt,"set",RWRWR_,"root","UI",SEC_WR)) {
	    srcwdg.at().attrAt(nattr).at().setCfgVal(opt->text());
	    if(srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst)	srcwdg.at().attrAt(nattr).at().setS(opt->text());
	    else if(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut))
		srcwdg.at().attrAt(nattr).at().setCfgVal((opt->text().compare(0,4,"val:")==0)?opt->text():TSYS::strParse(opt->text(),0," "));
	}
    }
    else return false;

    return true;
}

bool Widget::cntrCmdProcess( XMLNode *opt )
{
    string wattr;

    if(isLink()) return false;

    //Get page info
    if(opt->name() == "info") {
	if(ctrMkNode("area",opt,-1,"/proc",_("Process"))) {
	    ctrMkNode("fld",opt,-1,"/proc/wdg",_("Widget"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/proc/w_lst");
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if(!wdgPresent(wattr))	wattr = ".";
	    if(ctrMkNode("table",opt,-1,"/proc/attr",_("Attributes"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/proc/attr/id",_("Id"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/type",_("Type"),RWRWR_,"root",SUI_ID,4,"tp","dec","idm","1","dest","select","select","/proc/tp_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/wa",_("Work area"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/proc",_("Process"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/proc/attr/cfg",_("Configuration"),RWRWR_,"root",SUI_ID,4,"tp","dec","idm","1","dest","select","select","/proc/lnk_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/cfgtmpl",_("Configuration template"),RWRWR_,"root",SUI_ID,1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/proc/calc",_("Calculate"))) {
		ctrMkNode("fld",opt,-1,"/proc/calc/progLng",_("Program language"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/proc/calc/plangLs");
		ctrMkNode("fld",opt,-1,"/proc/calc/per",_("Calculate period (ms)"),RWRWR_,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/proc/calc/prog",_("Program"),RWRWR_,"root",SUI_ID,3,"tp","str","rows","10","SnthHgl","1");
	    }
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/proc/wdg") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if(wattr != "." && !wdgPresent(wattr))	wattr = ".";
	    opt->setText(wattr);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"wdgAttr",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/proc/w_lst" && ctrChkNode(opt)) {
	vector<string> lst;
	wdgList(lst);
	opt->childAdd("el")->setText(".");
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setText(lst[i_f]);
    }
    else if(a_path == "/proc/attr") {
	wattr = opt->attr("wdg");
	if(wattr.empty())
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	if(wattr != "." && !wdgPresent(wattr))	wattr = ".";

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
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
	    for(unsigned i_el = 0; i_el < lst.size(); i_el++) {
		if(n_id)	n_id->childAdd("el")->setText(lst[i_el]);
		if(n_name)	n_name->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().name());
		if(n_type)	n_type->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().fld().type()+
				    ((wdg.at().attrAt(lst[i_el]).at().fld().flg()&(TFld::FullText|TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
		if(n_wa)	n_wa->childAdd("el")->setText( wdg.at().attrAt(lst[i_el]).at().fld().values()+"|"+
							    wdg.at().attrAt(lst[i_el]).at().fld().selNames());
		if(n_proc)	n_proc->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().flgSelf()&Attr::ProcAttr));
		if(n_cfg)	n_cfg->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle)));
		if(n_cfgtmpl)	n_cfgtmpl->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().cfgTempl());
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    wdg.at().attrAdd( new TFld("newAttr",_("New attribute"),TFld::String,Attr::IsUser) );
	    //wdg.at().attrAt("newAttr").at().setS(EVAL_STR);
	    wdg.at().attrAt("newAttr").at().setModif(1);
	    wdg.at().modif();
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    if(!wdg.at().attrPresent(opt->attr("key_id")))
		throw TError(nodePath().c_str(),_("Deleting the enclosed widget's elements error."));
	    if(!(wdg.at().attrAt(opt->attr("key_id")).at().flgSelf()&Attr::IsInher) &&
		    wdg.at().attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsUser)
		wdg.at().attrDel(opt->attr("key_id"));
	    else throw TError(nodePath().c_str(),_("Deleting the not user element error."));
	    wdg.at().modif();
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    // Request data
	    string idattr = opt->attr("key_id");
	    string idcol  = opt->attr("col");
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);

	    if(idcol == "id" || idcol == "type") {
		string		tid	= idattr;
		string		tnm	= wdg.at().attrAt(idattr).at().name();
		TFld::Type	ttp	= wdg.at().attrAt(idattr).at().fld().type();
		unsigned	tflg	= wdg.at().attrAt(idattr).at().fld().flg();
		string		tvals	= wdg.at().attrAt(idattr).at().fld().values();
		string		tsels	= wdg.at().attrAt(idattr).at().fld().selNames();
		Attr::SelfAttrFlgs sflgs = wdg.at().attrAt(idattr).at().flgSelf();

		if(!(!(sflgs&Attr::IsInher) && tflg&Attr::IsUser))
		    throw TError(nodePath().c_str(),_("Changing of not user attribute is no permitted"));

		string tvl	= wdg.at().attrAt(idattr).at().getS();
		string tmpl	= wdg.at().attrAt(idattr).at().cfgTempl();
		string cfgval	= wdg.at().attrAt(idattr).at().cfgVal();

		if(idcol == "id") {
		    tid = opt->text();
		    if(wdg.at().attrPresent(tid)) throw TError(nodePath().c_str(),_("New attribute's ID '%s' already present."),tid.c_str());
		}
		else if(idcol == "type") {
		    ttp = (TFld::Type)(s2i(opt->text())&0x0f);
		    tflg = tflg^((tflg^((s2i(opt->text())>>4)|Attr::IsUser))&(TFld::FullText|TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address));
		}
		wdg.at().attrDel(idattr);
		wdg.at().attrAdd(new TFld(tid.c_str(),tnm.c_str(),ttp,tflg,"","",tvals.c_str(),tsels.c_str()));
		wdg.at().attrAt(tid).at().setS(tvl);
		wdg.at().attrAt(tid).at().setFlgSelf(sflgs);
		wdg.at().attrAt(tid).at().setCfgVal(cfgval);
		wdg.at().attrAt(tid).at().setCfgTempl(tmpl);
		wdg.at().attrAt(tid).at().setModif(1);
	    }
	    else {
		if(idcol == "name") {
		    if(!(!(wdg.at().attrAt(idattr).at().flgSelf()&Attr::IsInher) && wdg.at().attrAt(idattr).at().fld().flg()&Attr::IsUser))
			throw TError(nodePath().c_str(),_("Changing of not user attribute is no permitted"));
		    wdg.at().attrAt(idattr).at().fld().setDescr(opt->text());
		}
		else if(idcol == "wa") {
		    wdg.at().attrAt(idattr).at().fld().setValues(TSYS::strSepParse(opt->text(),0,'|'));
		    wdg.at().attrAt(idattr).at().fld().setSelNames(TSYS::strSepParse(opt->text(),1,'|'));
		}
		else if(idcol == "proc") {
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = s2i(opt->text())?Attr::ProcAttr:(Attr::SelfAttrFlgs)0;
		    wdg.at().attrAt(idattr).at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::ProcAttr)) );
		}
		else if(idcol == "cfg") {
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)s2i(opt->text());
		    if((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle))
			wdg.at().attrAt(idattr).at().setFlgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle))) );
		}
		else if(idcol == "cfgtmpl")	wdg.at().attrAt(idattr).at().setCfgTempl(opt->text());
	    }
	    wdg.at().modif();
	}
    }
    else if(a_path == "/proc/calc/progLng") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(calcLang());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcLang(opt->text());
    }
    else if(a_path == "/proc/calc/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(calcPer()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcPer(s2i(opt->text()));
    }
    else if(a_path == "/proc/calc/prog") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(calcProg());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
	    try {
		SYS->daq().at().at(TSYS::strParse(calcLang(),0,".")).at().
				compileFuncSynthHighl(TSYS::strParse(calcLang(),1,"."),*opt);
	    } catch(...){ }
    }
    else if(a_path == "/proc/calc/plangLs" && ctrChkNode(opt)) {
	string tplng = calcLang();
	int c_lv = 0;
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::strSepParse(tplng,0,'.',&c_off)).size(); c_lv++) {
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path += ".";
	vector<string> ls;
	switch(c_lv) {
	    case 0:
		SYS->daq().at().modList(ls);
		for(unsigned i_l = 0; i_l < ls.size(); )
		    if(!SYS->daq().at().at(ls[i_l]).at().compileFuncLangs()) ls.erase(ls.begin()+i_l);
		    else i_l++;
		break;
	    case 1:
		if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
		    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(&ls);
		break;
	}
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if(a_path == "/proc/tp_ls" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(TFld::Boolean))->setText(_("Boolean"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Integer))->setText(_("Integer"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Real))->setText(_("Real"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String))->setText(_("String"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Object))->setText(_("Object"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Integer+(TFld::Selected<<4)))->setText(_("Select integer"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Real+(TFld::Selected<<4)))->setText(_("Select real"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(TFld::Selected<<4)))->setText(_("Select string"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(TFld::FullText<<4)))->setText(_("Text"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Color<<4)))->setText(_("Color"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Image<<4)))->setText(_("Image"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Font<<4)))->setText(_("Font"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Integer+(Attr::DateTime<<4)))->setText(_("DateTime"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Address<<4)))->setText(_("Address"));
    }
    else if(a_path == "/proc/lnk_ls" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(0))->setText(_("Not"));
	opt->childAdd("el")->setAttr("id",i2s(Attr::CfgConst))->setText(_("Constant"));
	opt->childAdd("el")->setAttr("id",i2s(Attr::CfgLnkIn))->setText(_("Input link"));
	opt->childAdd("el")->setAttr("id",i2s(Attr::CfgLnkOut))->setText(_("Output link"));
	opt->childAdd("el")->setAttr("id",i2s(Attr::CfgLnkIn|Attr::CfgLnkOut))->setText(_("Full link"));
	opt->childAdd("el")->setAttr("id",i2s(Attr::FromStyle))->setText(_("From style"));
    }
    else return false;

    return true;
}

//************************************************
//* Attr: Widget attribute                       *
//************************************************
Attr::Attr( TFld *ifld, bool inher ) : mFld(NULL), mModif(0), self_flg((SelfAttrFlgs)0), mConn(0), mOwner(NULL)
{
    setFld(ifld, inher);
}

Attr::~Attr( )
{
    setFld(NULL, false);
}

void Attr::setFld( TFld *fld, bool inher )
{
    if(owner()) pthread_mutex_lock(&owner()->mtxAttr());
    //Free for previous type
    if(mFld && (!fld || fld->type() != mFld->type()))
	switch(mFld->type()) {
	    case TFld::String:
		if(mVal.s) delete mVal.s;
		break;
	    case TFld::Object:
		if(mVal.o) delete mVal.o;
		break;
	    default: break;
	}

    //Alloc for new type
    if(fld && (!mFld || fld->type() != mFld->type()))
	switch(fld->type()) {
	    case TFld::String:
		mVal.s = NULL;
		if(fld->flg()&Attr::DirRead) break;
		mVal.s = new string();
		*mVal.s = fld->def();
		break;
	    case TFld::Integer:
		mVal.i = strtoll(fld->def().c_str(),NULL,(fld->flg()&TFld::HexDec)?16:((fld->flg()&TFld::OctDec)?8:10));
		break;
	    case TFld::Real:	mVal.r = s2r(fld->def());	break;
	    case TFld::Boolean:	mVal.b = s2i(fld->def());	break;
	    case TFld::Object:
		mVal.o = NULL;
		if(fld->flg()&Attr::DirRead) break;
		mVal.o = new AutoHD<TVarObj>(new TVarObj);
		break;
	    default: break;
	}

    if(mFld) {
	mFld->setLen(mFld->len()-1);
	if(!mFld->len()) delete mFld;
	mFld = NULL;
    }
    mFld = fld;
    if(mFld && !inher)	mFld->setLen(1);
    else if(mFld && inher)	mFld->setLen(mFld->len()+1);
    self_flg = inher ? self_flg|Attr::IsInher : self_flg & ~Attr::IsInher;
    if(owner()) pthread_mutex_unlock(&owner()->mtxAttr());
}

const string &Attr::id( )	{ return fld().name(); }

string Attr::name( )		{ return fld().descr(); }

TFld::Type Attr::type( )	{ return fld().type(); }

int Attr::flgGlob( )		{ return fld().flg(); }

string Attr::getSEL( bool sys )
{
    if(!(fld().flg()&TFld::Selected)) throw TError("Cfg",_("Element type is not selected!"));
    switch(fld().type()) {
	case TFld::String:	return fld().selVl2Nm(getS(sys));
	case TFld::Integer:	return fld().selVl2Nm(getI(sys));
	case TFld::Real:	return fld().selVl2Nm(getR(sys));
	case TFld::Boolean:	return fld().selVl2Nm(getB(sys));
	default: break;
    }
    return EVAL_STR;
}

TVariant Attr::get( bool sys )
{
    switch(fld().type()) {
	case TFld::Integer:	return getI(sys);
	case TFld::Real:	return getR(sys);
	case TFld::Boolean:	return getB(sys);
	case TFld::String:	return getS(sys);
	case TFld::Object:	return getO(sys);
	default: break;
    }
    return EVAL_STR;
}

string Attr::getS( bool sys )
{
    if(flgGlob()&Attr::DirRead)	return owner()->vlGet(*this).getS();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this,getS(true),false).getS();
    switch(fld().type()) {
	case TFld::Integer:	{ int64_t tvl = getI(sys); return (tvl != EVAL_INT) ? ll2s(tvl) : EVAL_STR; }
	case TFld::Real:	{ double tvl = getR(sys); return (tvl != EVAL_REAL) ? r2s(tvl) : EVAL_STR; }
	case TFld::Boolean:	{ char tvl = getB(sys); return (tvl != EVAL_BOOL) ? i2s((bool)tvl) : EVAL_STR; }
	case TFld::Object:	{ AutoHD<TVarObj> tvl = getO(sys); return (tvl.at().objName() != "EVAL") ? tvl.at().getStrXML() : EVAL_STR; }
	case TFld::String: {
	    pthread_mutex_lock(&owner()->mtxAttr());
	    string tvl = *mVal.s;
	    pthread_mutex_unlock(&owner()->mtxAttr());
	    return tvl;
	}
	default: break;
    }

    return EVAL_STR;
}

int64_t Attr::getI( bool sys )
{
    if(flgGlob()&Attr::DirRead)	return owner()->vlGet(*this).getI();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this,getI(true),false).getI();
    switch(fld().type()) {
	case TFld::String:	{ string tvl = getS(sys); return (tvl != EVAL_STR) ? s2ll(tvl) : EVAL_INT; }
	case TFld::Real:	{ double tvl = getR(sys); return (tvl != EVAL_REAL) ? (int64_t)tvl : EVAL_INT; }
	case TFld::Boolean:	{ char tvl = getB(sys); return (tvl != EVAL_BOOL) ? (bool)tvl : EVAL_INT; }
	case TFld::Integer:	return mVal.i;
	default: break;
    }

    return EVAL_INT;
}

double Attr::getR( bool sys )
{
    if(flgGlob()&Attr::DirRead)	return owner()->vlGet(*this).getR();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this,getR(true),false).getR();
    switch(fld().type()) {
	case TFld::String:	{ string tvl = getS(sys); return (tvl != EVAL_STR) ? s2r(tvl) : EVAL_REAL; }
	case TFld::Integer:	{ int64_t tvl = getI(sys); return (tvl != EVAL_INT) ? tvl : EVAL_REAL; }
	case TFld::Boolean:	{ char tvl = getB(sys); return (tvl != EVAL_BOOL) ? (bool)tvl : EVAL_REAL; }
	case TFld::Real:	return mVal.r;
	default: break;
    }

    return EVAL_REAL;
}

char Attr::getB( bool sys )
{
    if(flgGlob()&Attr::DirRead) return owner()->vlGet(*this).getB();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this,getB(true),false).getB();
    switch(fld().type()) {
	case TFld::String:	{ string tvl = getS(sys); return (tvl != EVAL_STR) ? (bool)s2i(tvl) : EVAL_BOOL; }
	case TFld::Integer:	{ int64_t tvl = getI(sys); return (tvl != EVAL_INT) ? (bool)tvl : EVAL_BOOL; }
	case TFld::Real:	{ double tvl = getR(sys); return (tvl != EVAL_REAL) ? (bool)tvl : EVAL_BOOL; }
	case TFld::Boolean:	return mVal.b;
	default: break;
    }

    return EVAL_BOOL;
}

AutoHD<TVarObj> Attr::getO( bool sys )
{
    if(flgGlob()&Attr::DirRead)	return owner()->vlGet(*this).getO();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this,getO(true),false).getO();
    if(fld().type() != TFld::Object) return new TEValObj;
    pthread_mutex_lock(&owner()->mtxAttr());
    AutoHD<TVarObj> tvl = *mVal.o;
    pthread_mutex_unlock(&owner()->mtxAttr());

    return tvl;
}

void Attr::setSEL( const string &val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead) return;
    if(!(fld().flg()&TFld::Selected)) throw TError("Cfg",_("Element type is not selected!"));
    switch(fld().type()) {
	case TFld::String:	setS(fld().selNm2VlS(val), strongPrev, sys);	break;
	case TFld::Integer:	setI(fld().selNm2VlI(val), strongPrev, sys);	break;
	case TFld::Real:	setR(fld().selNm2VlR(val), strongPrev, sys);	break;
	case TFld::Boolean:	setB(fld().selNm2VlB(val), strongPrev, sys);	break;
	default: break;
    }
}

void Attr::set( const TVariant &val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead) return;
    switch(fld().type()) {
	case TFld::Integer:	setI(val.getI(), strongPrev, sys);	break;
	case TFld::Real:	setR(val.getR(), strongPrev, sys);	break;
	case TFld::Boolean:	setB(val.getB(), strongPrev, sys);	break;
	case TFld::String:	setS(val.getS(), strongPrev, sys);	break;
	case TFld::Object:	setO(val.getO(), strongPrev, sys);	break;
	default: break;
    }
}

void Attr::setS( const string &val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead) return;
    switch(fld().type()) {
	case TFld::Integer:	setI((val!=EVAL_STR) ? s2ll(val) : EVAL_INT, strongPrev, sys);	break;
	case TFld::Real:	setR((val!=EVAL_STR) ? s2r(val) : EVAL_REAL, strongPrev, sys);	break;
	case TFld::Boolean:	setB((val!=EVAL_STR) ? (bool)s2i(val) : EVAL_BOOL, strongPrev, sys);	break;
	case TFld::Object:
	    setO((val!=EVAL_STR) ? TVarObj::parseStrXML(val, NULL, getO()) : AutoHD<TVarObj>(new TEValObj), strongPrev, sys);
	    break;
	case TFld::String: {
	    if((!strongPrev && *mVal.s == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull())) break;
	    pthread_mutex_lock(&owner()->mtxAttr());
	    string t_str = *mVal.s;
	    *mVal.s = val;
	    pthread_mutex_unlock(&owner()->mtxAttr());
	    if(!sys && !owner()->attrChange(*this,TVariant(t_str))) {
		pthread_mutex_lock(&owner()->mtxAttr());
		*mVal.s = t_str;
		pthread_mutex_unlock(&owner()->mtxAttr());
	    }
	    else {
		unsigned imdf = owner()->modifVal(*this);
		mModif = imdf ? imdf : mModif+1;
	    }
	    break;
	}
	default: break;
    }
}

void Attr::setI( int64_t val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead) return;
    switch(fld().type()) {
	case TFld::String:	setS((val!=EVAL_INT) ? ll2s(val) : EVAL_STR, strongPrev, sys);	break;
	case TFld::Real:	setR((val!=EVAL_INT) ? val : EVAL_REAL, strongPrev, sys);		break;
	case TFld::Boolean:	setB((val!=EVAL_INT) ? (bool)val : EVAL_BOOL, strongPrev, sys);		break;
	case TFld::Object:	if(val == EVAL_INT) setO(new TEValObj(), strongPrev, sys);	break;
	case TFld::Integer: {
	    if(!(fld().flg()&TFld::Selected) && fld().selValI()[0] < fld().selValI()[1])
		val = vmin(fld().selValI()[1],vmax(fld().selValI()[0],val));
	    if((!strongPrev && mVal.i == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()))	break;
	    int64_t t_val = mVal.i;
	    mVal.i = val;
	    if(!sys && !owner()->attrChange(*this,TVariant(t_val)))	mVal.i = t_val;
	    else {
		unsigned imdf = owner()->modifVal(*this);
		mModif = imdf ? imdf : mModif+1;
	    }
	    break;
	}
	default: break;
    }
}

void Attr::setR( double val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead) return;
    switch(fld().type()) {
	case TFld::String:	setS((val!=EVAL_REAL) ? r2s(val) : EVAL_STR, strongPrev, sys);	break;
	case TFld::Integer:	setI((val!=EVAL_REAL) ? (int64_t)val : EVAL_INT, strongPrev, sys);	break;
	case TFld::Boolean:	setB((val!=EVAL_REAL) ? (bool)val : EVAL_BOOL, strongPrev, sys);break;
	case TFld::Object:	if(val == EVAL_REAL) setO(new TEValObj(), strongPrev, sys);	break;
	case TFld::Real: {
	    if(!(fld().flg()&TFld::Selected) && fld().selValR()[0] < fld().selValR()[1])
		val = vmin(fld().selValR()[1],vmax(fld().selValR()[0],val));
	    if((!strongPrev && mVal.r == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()))		break;
	    double t_val = mVal.r;
	    mVal.r = val;
	    if(!sys && !owner()->attrChange(*this,TVariant(t_val)))	mVal.r = t_val;
	    else {
		unsigned imdf = owner()->modifVal(*this);
		mModif = imdf ? imdf : mModif+1;
	    }
	    break;
	}
	default: break;
    }
}

void Attr::setB( char val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead) return;
    switch(fld().type()) {
	case TFld::String:	setS((val!=EVAL_BOOL) ? i2s((bool)val) : EVAL_STR, strongPrev, sys);	break;
	case TFld::Integer:	setI((val!=EVAL_BOOL) ? (bool)val : EVAL_INT, strongPrev, sys);		break;
	case TFld::Real:	setR((val!=EVAL_BOOL) ? (bool)val : EVAL_REAL, strongPrev, sys);	break;
	case TFld::Object:	if(val == EVAL_BOOL) setO(new TEValObj(), strongPrev, sys);		break;
	case TFld::Boolean: {
	    if((!strongPrev && mVal.b == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull()))	break;
	    bool t_val = mVal.b;
	    mVal.b = val;
	    if(!sys && !owner()->attrChange(*this,TVariant(t_val))) mVal.b = t_val;
	    else {
		unsigned imdf = owner()->modifVal(*this);
		mModif = imdf ? imdf : mModif+1;
	    }
	    break;
	}
	default: break;
    }
}

void Attr::setO( AutoHD<TVarObj> val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::DirRead)	return;
    switch(fld().type()) {
	case TFld::String:	setS(val.at().getStrXML(), strongPrev, sys);break;
	case TFld::Integer: case TFld::Real: case TFld::Boolean:
				setB(true, strongPrev, sys);		break;
	case TFld::Object: {
	    if((!strongPrev && *mVal.o == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull())) break;
	    pthread_mutex_lock(&owner()->mtxAttr());
	    AutoHD<TVarObj> t_obj = *mVal.o;
	    *mVal.o = val;
	    pthread_mutex_unlock(&owner()->mtxAttr());
	    if(!sys && !owner()->attrChange(*this,TVariant(t_obj))) {
		pthread_mutex_lock(&owner()->mtxAttr());
		*mVal.o = t_obj;
		pthread_mutex_unlock(&owner()->mtxAttr());
	    }
	    else {
		unsigned imdf = owner()->modifVal(*this);
		mModif = imdf ? imdf : mModif+1;
		return;
	    }
	    break;
	}
	default: break;
    }
}

string Attr::cfgTempl( )
{
    pthread_mutex_lock(&owner()->mtxAttr());
    string tvl = cfg.substr(0,cfg.find("\n"));
    pthread_mutex_unlock(&owner()->mtxAttr());
    return tvl;
}

string Attr::cfgVal( )
{
    pthread_mutex_lock(&owner()->mtxAttr());
    size_t sepp = cfg.find("\n");
    string tvl = (sepp!=string::npos) ? cfg.substr(sepp+1) : "";
    pthread_mutex_unlock(&owner()->mtxAttr());
    return tvl;
}

/*bool Attr::isTransl( TFld::Type tp, int flgGlb, int flgSelf )
{
    return (tp == TFld::String &&
	!(flgGlb&(TFld::NoStrTransl|Attr::DirRead|Attr::Image|Attr::DateTime|Attr::Color|Attr::Font|Attr::Address)) &&
	(flgSelf == -1 || flgSelf&(Attr::CfgConst|Attr::CfgLnkIn)));
}

bool Attr::isTransl( bool cfg )	{ return Attr::isTransl(type(), flgGlob(), (cfg?flgSelf():-1)); }*/

void Attr::setCfgTempl( const string &vl )
{
    string t_tmpl = cfgTempl();
    if(t_tmpl == vl) return;
    pthread_mutex_lock(&owner()->mtxAttr());
    cfg = vl+"\n"+cfgVal();
    pthread_mutex_unlock(&owner()->mtxAttr());
    if(!owner()->attrChange(*this,TVariant())) {
	pthread_mutex_lock(&owner()->mtxAttr());
	cfg = t_tmpl+"\n"+cfgVal();
	pthread_mutex_unlock(&owner()->mtxAttr());
    }
    else {
	unsigned imdf = owner()->modifVal(*this);
	mModif = imdf ? imdf : mModif+1;
    }
}

void Attr::setCfgVal( const string &vl )
{
    string t_val = cfgVal();
    if(t_val == vl) return;
    pthread_mutex_lock(&owner()->mtxAttr());
    cfg = cfgTempl()+"\n"+vl;
    pthread_mutex_unlock(&owner()->mtxAttr());
    if(!owner()->attrChange(*this,TVariant())) {
	pthread_mutex_lock(&owner()->mtxAttr());
	cfg = cfgTempl()+"\n"+t_val;
	pthread_mutex_unlock(&owner()->mtxAttr());
    }
    else {
	unsigned imdf = owner()->modifVal(*this);
	mModif = imdf ? imdf : mModif+1;
    }
}

void Attr::setFlgSelf( SelfAttrFlgs flg )
{
    if(self_flg == flg)	return;
    SelfAttrFlgs t_flg = (SelfAttrFlgs)self_flg;
    self_flg = (flg & ~Attr::IsInher) | (t_flg&Attr::IsInher);
    if(!owner()->attrChange(*this,TVariant()))	self_flg = t_flg;
    else {
	unsigned imdf = owner()->modifVal(*this);
	mModif = imdf ? imdf : mModif+1;
    }
}

void Attr::AHDConnect( )
{
    pthread_mutex_lock(&owner()->mtxAttrCon);
    if(mConn < ((1<<ATTR_CON_DEPTH)-1)) mConn++;
    else mess_err(owner()->nodePath().c_str(),_("Connections to attribute '%s' more to %d!"),id().c_str(),(1<<ATTR_CON_DEPTH)-1);
    pthread_mutex_unlock(&owner()->mtxAttrCon);
}

bool Attr::AHDDisConnect( )
{
    pthread_mutex_lock(&owner()->mtxAttrCon);
    if(mConn > 0) mConn--;
    else mess_err(owner()->nodePath().c_str(),_("Disconnection from attribute '%s' more to connections!"),id().c_str());
    pthread_mutex_unlock(&owner()->mtxAttrCon);
    return false;
}

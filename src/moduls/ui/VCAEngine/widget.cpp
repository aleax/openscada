
//OpenSCADA module UI.VCAEngine file: widget.cpp
/***************************************************************************
 *   Copyright (C) 2006-2021 by Roman Savochenko, <roman@oscada.org>       *
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
    mId(id), mEnable(false), mLnk(false), mStlLock(false), BACrtHoldOvr(false), mParentNm(isrcwdg), mtxAttrM(true)
{
    inclWdg = grpAdd("wdg_");
}

Widget::~Widget( )
{
    //Remove attributes
    mtxAttr().lock();
    map<string,Attr*>::iterator p;
    while((p = mAttrs.begin()) != mAttrs.end()) {
	for(int iC = 0; iC < 100 && p->second->mConn; iC++)	TSYS::sysSleep(0.01);
	if(p->second->mConn) mess_err(nodePath().c_str(),_("The attribute '%s' is not released. Forced removal!"),p->first.c_str());
	delete p->second;
	mAttrs.erase(p);
    }
    mtxAttr().unlock();
}

TCntrNode &Widget::operator=( const TCntrNode &node )
{
    const Widget *srcN = dynamic_cast<const Widget*>(&node);
    if(!srcN) return *this;

    if(!srcN->enable()) return *this;

    //Parent link copy
    if(srcN->parentNm() != path() && parentNm().empty()) {
	if(parentNm() != srcN->parentNm() && enable()) setEnable(false);
	setParentNm(srcN->parentNm());
    }
    if(!enable()) setEnable(true);

    //Copy generic configuration
    if(srcN->parent().freeStat() || (srcN->name() != srcN->parent().at().name() && srcN->name() != srcN->id()))
	setName(srcN->name());
    if(srcN->parent().freeStat() || srcN->descr() != srcN->parent().at().descr()) setDescr(srcN->descr());
    if(srcN->parent().freeStat() || srcN->ico() != srcN->parent().at().ico())	setIco(srcN->ico());
    setOwner(srcN->owner());
    setGrp(srcN->grp());
    setPermit(srcN->permit());
    if(srcN->parent().freeStat() || srcN->calcLang() != srcN->parent().at().calcLang())	setCalcLang(srcN->calcLang());
    if(srcN->parent().freeStat() || srcN->calcProg() != srcN->parent().at().calcProg())	setCalcProg(srcN->calcProg());
    if(srcN->parent().freeStat() || srcN->calcPer() != srcN->parent().at().calcPer())	setCalcPer(srcN->calcPer());

    //Copy attributes
    vector<string> els;
    srcN->attrList(els);
    AutoHD<Attr> attr, pattr;
    for(unsigned iA = 0; iA < els.size(); iA++) {
	if(!(pattr=srcN->attrAt(els[iA])).at().modif()) continue;	//Do not copy not modified attributes
	if(!attrPresent(els[iA])) {
	    bool isInher = pattr.at().flgSelf()&Attr::IsInher;
	    attrAdd(isInher ? &srcN->attrAt(els[iA]).at().fld() : new TFld(srcN->attrAt(els[iA]).at().fld()), -1, isInher);
	    attrAt(els[iA]).at().setModif(1);
	}
	attr  = attrAt(els[iA]);
	attr.at().setFlgSelf(pattr.at().flgSelf());
	if(!(attr.at().flgGlob()&(Attr::OnlyRead|Attr::NotStored)))
	    switch(attr.at().type()) {
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
    if(!isLink() && srcN->isContainer()) {
	srcN->wdgList(els);
	for(unsigned iW = 0; iW < els.size(); iW++) {
	    if(!wdgPresent(els[iW]))	wdgAdd(els[iW],"","");
	    (TCntrNode&)wdgAt(els[iW]).at() = (TCntrNode&)srcN->wdgAt(els[iW]).at();
	}
    }

    return *this;
}

void Widget::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore) setEnable(true);
    if(flag&TCntrNode::NodeConnect && !BACrtHoldOvr) {
	//Add main attributes
	attrAdd(new TFld("id",_("Identifier"),TFld::String,TFld::NoWrite|Attr::OnlyRead|Attr::Generic));
	attrAdd(new TFld("path",_("Path"),TFld::String,TFld::NoWrite|Attr::OnlyRead|Attr::Generic));
	attrAdd(new TFld("parent",_("Parent"),TFld::String,TFld::NoWrite|Attr::OnlyRead|Attr::Generic));
	attrAdd(new TFld("owner",_("Owner"),TFld::String,Attr::Generic|Attr::PreRead,"","root:UI"));
	attrAdd(new TFld("perm",_("Access"),TFld::Integer,TFld::OctDec|TFld::Selectable|Attr::Generic|Attr::PreRead,"","01000",
	    "0;0400;0440;0444;0600;0640;0644;0660;0664;0666;"
	    "01000;01400;01440;01444;01600;01640;01644;01660;01664;01666",
	    _("No access;R_____;R_R___;R_R_R_;RW____;RWR___;RWR_R_;RWRW__;RWRWR_;RWRWRW;"
	      "Inheritance;Inherit.(R_____);Inherit.(R_R___);Inherit.(R_R_R_);Inherit.(RW____);"
	      "Inherit.(RWR___);Inherit.(RWR_R_);Inherit.(RWRW__);Inherit.(RWRWR_);Inherit.(RWRWRW)")));
	attrAdd(new TFld("root",_("Root"),TFld::String,TFld::NoWrite|Attr::OnlyRead|Attr::Generic,"","","","",i2s(A_ROOT).c_str()));
	attrAdd(new TFld("name",_("Name"),TFld::String,TFld::TransltText|Attr::Generic));
	attrAdd(new TFld("dscr",_("Description"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Generic));
	attrAdd(new TFld("en",_("Enabled"),TFld::Boolean,Attr::Generic,"","1","","",i2s(A_EN).c_str()));
	attrAdd(new TFld("active",_("Active"),TFld::Boolean,Attr::Active,"","0","","",i2s(A_ACTIVE).c_str()));
	attrAdd(new TFld("geomX",_("Geometry: x"),TFld::Real,Attr::Generic,"","0","-10000;10000","",i2s(A_GEOM_X).c_str()));
	attrAdd(new TFld("geomY",_("Geometry: y"),TFld::Real,Attr::Generic,"","0","-10000;10000","",i2s(A_GEOM_Y).c_str()));
	attrAdd(new TFld("geomW",_("Geometry: width"),TFld::Real,Attr::Generic,"","100","0;10000","",i2s(A_GEOM_W).c_str()));
	attrAdd(new TFld("geomH",_("Geometry: height"),TFld::Real,Attr::Generic,"","100","0;10000","",i2s(A_GEOM_H).c_str()));
	attrAdd(new TFld("geomXsc",_("Geometry: x scale"),TFld::Real,Attr::Generic,"","1","0.01;100","",i2s(A_GEOM_X_SC).c_str()));
	attrAdd(new TFld("geomYsc",_("Geometry: y scale"),TFld::Real,Attr::Generic,"","1","0.01;100","",i2s(A_GEOM_Y_SC).c_str()));
	attrAdd(new TFld("geomZ",_("Geometry: z"),TFld::Integer,Attr::Generic,"","0","-1000000;1000000","",i2s(A_GEOM_Z).c_str()));
	attrAdd(new TFld("geomMargin",_("Geometry: margin"),TFld::Integer,Attr::Generic,"","0","0;1000","",i2s(A_GEOM_MARGIN).c_str()));
	attrAdd(new TFld("tipTool",_("Tip: tool"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Generic,"","","","",i2s(A_TIP_TOOL).c_str()));
	attrAdd(new TFld("tipStatus",_("Tip: status"),TFld::String,TFld::TransltText|Attr::Generic,"","","","",i2s(A_TIP_STATUS).c_str()));
	attrAdd(new TFld("contextMenu",_("Context menu"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Generic,"","","","",i2s(A_CTX_MENU).c_str()));
	attrAdd(new TFld("evProc",_("Events processing"),TFld::String,TFld::FullText,"200"));
    }
}

void Widget::preDisable( int flag )
{
    //Delete heritors widgets
    while(herit().size()) mod->nodeDel(herit()[0].at().path(), 0, 0x10);

    //Disable widget
    if(enable()) setEnable(false);

    //Force unlinking
    if(!mParent.freeStat()) {
	//Unregister heritater
	mParent.at().heritUnreg(this);

	//Disconnect parent widget
	mParent.free();
    }
}

string Widget::rootId( ) const	{ return parent().freeStat() ? "" : parent().at().rootId(); }

string Widget::name( ) const
{
    string wnm = "";
    try { wnm = attrAt("name").at().getS(); } catch(TError&) { }
    return wnm.size() ? wnm : mId;
}

void Widget::setName( const string &inm )
{
    attrAt("name").at().setS((inm==mId)?"":inm);
}

string Widget::descr( ) const
{
    try { return attrAt("dscr").at().getS(); }
    catch(TError&) { }

    return "";
}

void Widget::setDescr( const string &idscr )	{ attrAt("dscr").at().setS(idscr); }

string Widget::owner( ) const
{
    try { return TSYS::strParse(attrAt("owner").at().getS(), 0, ":"); }
    catch(TError&) { }

    return "";
}

void Widget::setOwner( const string &iown )
{
    attrAt("owner").at().setS(iown+":"+grp());
    //Group update
    if(SYS->security().at().grpAt("UI").at().user(iown)) setGrp("UI");
    else {
	vector<string> gls;
	SYS->security().at().usrGrpList(owner(), gls);
	setGrp(gls.size()?gls[0]:Widget::grp());
    }
}

string Widget::grp( ) const
{
    try { return TSYS::strParse(attrAt("owner").at().getS(), 1, ":"); }
    catch(TError&) { }

    return "";
}

void Widget::setGrp( const string &igrp )	{ attrAt("owner").at().setS(owner()+":"+igrp); }

short Widget::permit( ) const
{
    try { return attrAt("perm").at().getI(); }
    catch(TError&) { }

    return 0;
}

void Widget::setPermit( short iperm )		{ attrAt("perm").at().setI(iperm); }

string Widget::getStatus( )
{
    string rez = enable() ? _("Enabled. ") : _("Disabled. ");
    rez += TSYS::strMess(_("Used: %d. "), herit().size());

    return rez;
}

bool Widget::isContainer( ) const	{ return parent().freeStat() ? false : parent().at().isContainer(); }

string Widget::path( ) const
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

bool Widget::enable( ) const	{ return mEnable; }

void Widget::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    if(val) {
	if(parentNm() != "root") {
	    try {
		linkToParent();

		//Check for enable parent widget and enable if not
		if(!parent().at().enable()) parent().at().setEnable(true);

		//Inherit
		inheritAttr();
		inheritIncl();
	    } catch(TError &err) {
		mess_err(nodePath().c_str(),_("Error enabling the widget: %s"),err.mess.c_str());
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
	mess_sys(TMess::Debug, _("Disabling the widget."));

	//Disable heritors widgets.
	//!!!! Before all but else the heritors lost some attributes
	for(unsigned iH = 0; iH < herit().size(); )
	    if(herit()[iH].at().enable())
		try { herit()[iH].at().setEnable(false); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Error disabling the inheriting widget '%s'."),herit()[iH].at().id().c_str());
		    iH++;
		}
	    else iH++;

	disable(this);

	//Free no base attributes and restore base
	vector<string> ls;
	attrList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    if(!(attrAt(ls[iL]).at().flgGlob()&Attr::Generic)) attrDel(ls[iL], true);

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
    for(unsigned iL = 0; iL < ls.size(); iL++)
	if(val != wdgAt(ls[iL]).at().enable())
	    try { wdgAt(ls[iL]).at().setEnable(val); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Error enabling/disabling the child widget '%s'."),ls[iL].c_str());
	    }

    mEnable = val;
}

void Widget::linkToParent( )
{
    if(sTrm(parentNm()).empty() || parentNm() == path())
	throw TError(nodePath().c_str(),_("Parent item is empty or equal to itself!"));
    if(!mParent.freeStat()) ;	//connected early due to the parent name/address specific
    else if(parentNm() == "..") mParent = AutoHD<TCntrNode>(nodePrev());
    else mParent = mod->nodeAt(parentNm());

    if(isLink() && dynamic_cast<Widget*>(nodePrev()) && mParent.at().path() == ((Widget*)nodePrev())->path()) {
	mParent.free();
	throw TError(nodePath().c_str(),_("Parent is identical to the owner for the link!"));
    }

    //Register of heritater
    mParent.at().heritReg(this);
}

void Widget::setParentNm( const string &isw )
{
    if(enable() && mParentNm != isw) setEnable(false);
    mParentNm = isw;
}

AutoHD<Widget> Widget::parent( ) const	{ return mParent; }

AutoHD<Widget> Widget::parentNoLink( )	{ return parent().at().isLink() ? parent().at().parentNoLink() : parent(); }

void Widget::heritReg( Widget *wdg )
{
    //Search for already registered widget-heritator
    ResAlloc res(mHeritRes, true);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(&mHerit[iH].at() == wdg)	return;
    mHerit.push_back(AutoHD<Widget>(wdg));
}

void Widget::heritUnreg( Widget *wdg )
{
    //Search the widget-heritator
    ResAlloc res(mHeritRes, true);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(&mHerit[iH].at() == wdg) {
	    mHerit.erase(mHerit.begin()+iH);
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
    for(unsigned iL = 0; iL < ls.size(); iL++) {
	pattr.free();
	if(!attrPresent(ls[iL])) {
	    if(loadDef) continue;
	    pattr = parent().at().attrAt(ls[iL]);
	    if(pattr.at().flgGlob()&Attr::Mutable) continue;
	    attrAdd(&pattr.at().fld(), -1, true);
	}

	attr = attrAt(ls[iL]);
	if(loadDef) {
	    if(!(attr.at().flgGlob()&Attr::NotStored))
		attr.at().setS(attr.at().fld().def(), attr.at().flgGlob()&Attr::Active);
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)0);
	    attr.at().setCfgTempl("");
	    attr.at().setCfgVal("");
	    attr.at().setModif(0);
	    continue;
	}
	if(pattr.freeStat()) pattr = parent().at().attrAt(ls[iL]);
	if(!(attr.at().flgSelf()&Attr::IsInher)) attr.at().setFld(&pattr.at().fld(), true);
	if(attr.at().modif() && !(attr.at().flgSelf()&Attr::SessAttrInh) && !dynamic_cast<SessWdg*>(this)) {
	    //Force inheritance flags processing
	    //!!!! Attr::ProcAttr also added
	    int frcInherAtr = Attr::CfgConst | Attr::CfgLnkIn | Attr::CfgLnkOut | Attr::FromStyle | Attr::ProcAttr | Attr::VizerSpec;
	    if((pattr.at().flgSelf()&frcInherAtr) && (attr.at().flgSelf()&frcInherAtr) != (pattr.at().flgSelf()&frcInherAtr)) {
		attr.at().setFlgSelf((Attr::SelfAttrFlgs)((attr.at().flgSelf() & ~frcInherAtr) | (pattr.at().flgSelf() & frcInherAtr)), true);
		modif(true);
	    }
	    continue;
	}

	//All configuration before, to prepare
	attr.at().setFlgSelf((Attr::SelfAttrFlgs)pattr.at().flgSelf());
	if(isLink() && !parent().at().isLink())	// No inherit calc flag for links
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)(attr.at().flgSelf()&(~Attr::ProcAttr)));
	attr.at().setCfgTempl(pattr.at().cfgTempl());
	attr.at().setCfgVal(pattr.at().cfgVal());

	//The value in the end
	if(!(attr.at().flgGlob()&(Attr::OnlyRead|Attr::NotStored)))
	    switch(attr.at().type()) {
		case TFld::Boolean:	attr.at().setB(pattr.at().getB(), attr.at().flgGlob()&Attr::Active);	break;
		case TFld::Integer:	attr.at().setI(pattr.at().getI(), attr.at().flgGlob()&Attr::Active);	break;
		case TFld::Real:	attr.at().setR(pattr.at().getR(), attr.at().flgGlob()&Attr::Active);	break;
		case TFld::String:	attr.at().setS(pattr.at().getS(), attr.at().flgGlob()&Attr::Active);	break;
		default: break;
	    }

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
    vector<string> ls;
    if(!iwdg.empty() && parw.at().wdgPresent(iwdg)) ls.push_back(iwdg);
    else parw.at().wdgList(ls);
    for(unsigned iW = 0; iW < ls.size(); iW++)
	if(!wdgPresent(ls[iW]))
	    try { wdgAdd(ls[iW],"",parw.at().wdgAt(ls[iW]).at().path(),true); }
	    catch(TError &err) { mess_err(err.cat.c_str(),err.mess.c_str()); }
}

void Widget::wClear( )
{
    //Generic clear
    setIco("");

    //Inherit modified attributes
    vector<string> ls;
    attrList(ls);
    for(unsigned iA = 0; iA < ls.size(); iA++) {
	if(!attrPresent(ls[iA])) continue;
	AutoHD<Attr> attr = attrAt(ls[iA]);
	if(attr.at().modif()) {
	    attr.at().setModif(0);
	    inheritAttr(ls[iA]);
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
	    for(unsigned iW = 0; iW < ls.size(); iW++)
		if(!parw.at().wdgPresent(ls[iW]))
		    wdgDel(ls[iW],true);

	    //No present widget's add and clear call
	    parw.at().wdgList(ls);
	    for(unsigned iW = 0; iW < ls.size(); iW++)
		if(!wdgPresent(ls[iW]))
		    try {
			wdgAdd(ls[iW], "", parw.at().wdgAt(ls[iW]).at().path(), true);
			wdgAt(ls[iW]).at().setEnable(true);
		    }
		    catch(TError &err) { mess_err(err.cat.c_str(),err.mess.c_str()); }
		else wdgAt(ls[iW]).at().wClear();
	}
    }
    modif();
}

string Widget::wChDown( const string &ia )
{
    AutoHD<Widget> parw = parent();

    //Link process
    if(isLink()) {
	Widget *ownW = dynamic_cast<Widget*>(nodePrev(true));
	parw = ownW->parent();

	// Check for the recurrence inheritance prevent
	for(AutoHD<Widget> iParw = this; !iParw.freeStat(); iParw = iParw.at().parent())
	    if(iParw.at().path() == parw.at().path())
		throw TError(nodePath().c_str(), _("Impossible to lower down the changes of the widget '%s' to '%s' but it has the cyclic inheritance!"),
			path().c_str(), parw.at().path().c_str());

	// Add and copy it from the source
	if(!parw.at().wdgPresent(id()))
	    try {
		string pName = parentNm();
		if(pName.find(parw.at().path()+"/") == 0) pName = parent().at().parentNm();
		parw.at().wdgAdd(id(), "", pName);
		(TCntrNode&)parw.at().wdgAt(id()).at() = (TCntrNode&)*this;
	    } catch(TError &err) { mess_err(err.cat.c_str(),err.mess.c_str()); }
	parw = parw.at().wdgAt(id());
	// Relink original to source
	if(parentNm() != parw.at().path()) { setParentNm(parw.at().path()); setEnable(true); }
    }
    if(parw.freeStat()) return "";

    //Generic clear
    if(ia.empty() && ico() != parw.at().ico()) { parw.at().setIco(ico()); setIco(""); }

    //Put down modified attributes
    vector<string> ls;
    if(ia.empty()) attrList(ls); else ls.push_back(ia);
    AutoHD<Attr> attr;
    for(unsigned iA = 0; iA < ls.size(); iA++) {
	if(!attrPresent(ls[iA]) || !(attr=attrAt(ls[iA])).at().modif()) continue;
	//!!!! Check for type and generic flags changes
	if(!parw.at().attrPresent(ls[iA])) {
	    if(!(attr.at().flgGlob()&Attr::IsUser)) continue;
	    parw.at().attrAdd(new TFld(ls[iA].c_str(),"",attr.at().type(),attr.at().fld().flg()));
	}
	AutoHD<Attr> attrP = parw.at().attrAt(ls[iA]);
	attrP.at().set(attr.at().get());
	attrP.at().setFlgSelf(attr.at().flgSelf());
	attrP.at().setCfgVal(attr.at().cfgVal());
	attrP.at().setCfgTempl(attr.at().cfgTempl());
	attr.at().setModif(0);
	inheritAttr(ls[iA]);
    }

    //Check for included widgets
    if(ia.empty() && isContainer() && !isLink()) {
	//Check for widget's upadte/add
	wdgList(ls);
	for(unsigned iW = 0; iW < ls.size(); iW++) wdgAt(ls[iW]).at().wChDown();

	//No present widget's delete
	parw.at().wdgList(ls);
	for(unsigned iW = 0; iW < ls.size(); iW++)
	    if(!wdgPresent(ls[iW]))
		parw.at().wdgDel(ls[iW],true);
    }

    modif();

    return parw.at().path();
}

void Widget::attrList( vector<string> &list ) const
{
    const_cast<Widget*>(this)->mtxAttr().lock();
    list.clear();
    list.reserve(mAttrs.size());
    for(map<string, Attr* >::const_iterator p = mAttrs.begin(); p != mAttrs.end(); ++p) {
	while(p->second->mOi >= list.size())	list.push_back("");
	list[p->second->mOi] = p->first;
    }
    const_cast<Widget*>(this)->mtxAttr().unlock();
}

void Widget::attrAdd( TFld *attr, int pos, bool inher, bool forceMdf, bool allInher )
{
    string anm = attr->name();

    if(attrPresent(anm) || sTrm(anm).empty()) {
	if(!inher) delete attr;
	return;
    }
    try {
	mtxAttr().lock();
	if(mAttrs.size() >= ((1<<ATTR_OI_DEPTH)-1)) {
	    if(!inher) delete attr;
	    mtxAttr().unlock();
	    mess_err(nodePath().c_str(),_("Adding a new attribute '%s' exceeds the number %d!"),anm.c_str(),(1<<ATTR_OI_DEPTH)-1);
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

	if(a->id().compare(0,3,"vs_") == 0)
	    a->setFlgSelf((Attr::SelfAttrFlgs)(a->flgSelf()|Attr::VizerSpec), true);

	//Set modif for new attribute reload allow
	if(forceMdf) a->setModif(modifVal(*a));
    } catch(...) { }
    mtxAttr().unlock();

    //Update heritors' attributes
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; allInher && iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().inheritAttr(anm);
}

void Widget::attrDel( const string &attr, bool allInher  )
{
    if(!attrPresent(attr)) return;

    //Delete from inheritant wigets
    if(!(attrAt(attr).at().flgGlob()&Attr::Mutable) || allInher) {
	ResAlloc res(mHeritRes);
	for(unsigned iH = 0; iH < mHerit.size(); iH++)
	    if(mHerit[iH].at().enable())
		mHerit[iH].at().attrDel(attr);
    }

    //Self delete
    try {
	mtxAttr().lock();
	map<string, Attr* >::iterator p = mAttrs.find(attr);
	if(p == mAttrs.end())	throw TError(nodePath().c_str(), _("Attribute '%s' is not present."), attr.c_str());
	for(int iC = 0; iC < 100 && p->second->mConn; iC++)	TSYS::sysSleep(0.01);
	if(p->second->mConn) throw TError(nodePath().c_str(), _("Deleting attribute '%s' has not been released."), attr.c_str());

	int pos = p->second->mOi;
	for(map<string, Attr* >::iterator p1 = mAttrs.begin(); p1 != mAttrs.end(); ++p1)
	    if(p1->second->mOi > pos) p1->second->mOi--;
	delete p->second;
	mAttrs.erase(p);
    } catch(...) { }
    mtxAttr().unlock();
}

bool Widget::attrPresent( const string &attr ) const
{
    const_cast<Widget*>(this)->mtxAttr().lock();
    bool rez = (mAttrs.find(attr) != mAttrs.end());
    const_cast<Widget*>(this)->mtxAttr().unlock();
    return rez;
}

AutoHD<Attr> Widget::attrAt( const string &attr, int lev ) const
{
    //Local atribute request
    if(lev < 0) {
	const_cast<Widget*>(this)->mtxAttr().lock();
	map<string, Attr* >::const_iterator p = mAttrs.find(attr);
	if(p == mAttrs.end()) {
	    const_cast<Widget*>(this)->mtxAttr().unlock();
	    throw TError(nodePath().c_str(),_("Attribute '%s' is not present."), attr.c_str());
	}
	AutoHD<Attr> rez(p->second);
	const_cast<Widget*>(this)->mtxAttr().unlock();
	return rez;
    }
    //Process by full path
    AutoHD<Attr> an;
    size_t waSep = attr.rfind("/");
    string anm = (waSep == string::npos) ? attr : attr.substr(waSep+1);
    if(anm.compare(0,2,"a_") == 0) anm = anm.substr(2);
    if(waSep == string::npos) return attrPresent(anm) ? attrAt(anm) : an;
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
    if(cfg.flgGlob()&Attr::Active && !prev.isNull() && !parent().freeStat())
	parent().at().attrChange(cfg, prev);
    if(cfg.owner() != this) return false;

    //Update heritors attributes
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().inheritAttr(cfg.id());

    return true;
}

void Widget::wdgList( vector<string> &list, bool fromLnk ) const
{
    if(fromLnk && isLink()) parent().at().wdgList(list);
    else chldList(inclWdg, list);
}

bool Widget::wdgPresent( const string &wdg ) const	{ return chldPresent(inclWdg, wdg); }

void Widget::wdgAdd( const string &wid, const string &name, const string &path, bool force )
{
    if(!isContainer())  throw TError(nodePath().c_str(),_("The widget is not a container!"));
    if(wdgPresent(wid)) return;

    chldAdd(inclWdg, new Widget(wid,path));
    wdgAt(wid).at().setName(name);

    //Call heritors include widgets update
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().inheritIncl(wid);
}

AutoHD<Widget> Widget::wdgAt( const string &wdg, int lev, int off ) const
{
    if(lev < 0) return chldAt(inclWdg, wdg);

    AutoHD<Widget> rez;
    string iw = TSYS::pathLev(wdg, lev, true, &off);
    if(iw.compare(0,4,"wdg_") == 0) iw = iw.substr(4);
    if(iw.empty())	rez = AutoHD<Widget>(const_cast<Widget*>(this));
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
    return _("Image name in the form \"[{src}:]{name}\", where:\n"
	    "  \"src\" - image source:\n"
	    "    file - directly from a local file by the path;\n"
	    "    res - from table of DB-mime resources.\n"
	    "  \"name\" - file path or mime-resource identifier.\n"
	    "Examples:\n"
	    "  \"res:backLogo\" - from the table of DB-mime resources for the ID \"backLogo\";\n"
	    "  \"backLogo\" - like to the previous;\n"
	    "  \"file:/var/tmp/backLogo.png\" - from a local file by the path \"/var/tmp/backLogo.png\".");
}

string Widget::helpColor( )
{
    return _("Color name in the form \"{color}[-{alpha}]\", where:\n"
	    "  \"color\" - standard color name or its numeric representation \"#RRGGBB\";\n"
	    "  \"alpha\" - alpha-channel level [0...255], where 0 - fully transparent.\n"
	    "Examples:\n"
	    "  \"red\" - solid red color;\n"
	    "  \"#FF0000\" - solid red color by the numeric representation;\n"
	    "  \"red-127\" - half transparent red color.");
}

string Widget::helpFont( )
{
    return _("Font name in the form \"{family} {size} {bold} {italic} {underline} {strike}\", where:\n"
	    "  \"family\" - font family, for spaces use symbol '_', like to: \"Arial\", \"Courier\", \"Times_New_Roman\";\n"
	    "  \"size\" - font size in pixels;\n"
	    "  \"bold\" - font bold (0 or 1);\n"
	    "  \"italic\" - font italic (0 or 1);\n"
	    "  \"underline\" - font underlined (0 or 1);\n"
	    "  \"strike\" - font striked (0 or 1).\n"
	    "Examples:\n"
	    "  \"Arial 10 1 0 0 0\" - Arial font, size 10 pixels and bolded.");
}

TVariant Widget::vlGet( Attr &a )
{
    if(a.owner() == this) {
	if(a.id() == "id")		return TVariant(id());
	else if(a.id() == "path")	return TVariant(path());
	else if(a.id() == "root")	return TVariant(rootId());
	else if(a.id() == "parent")	return TVariant(parentNm());
	else if(a.id() == "owner") {
	    short perm = attrAt("perm").at().getI(true);
	    if(!(perm&01000)) return a.getS(true);
	    Widget *ownW = dynamic_cast<Widget*>(nodePrev());
	    return ownW ? ownW->attrAt("owner").at().getS() : "root:UI";
	}
	else if(a.id() == "perm") {
	    short perm = a.getI(true);
	    if(!(perm&01000)) return perm;
	    Widget *ownW = dynamic_cast<Widget*>(nodePrev());
	    return (ownW?ownW->attrAt("perm").at().getI():RWRWR_)|01000;
	}
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
    string a_path = opt->attr("path"), u = opt->attr("user"), l = opt->attr("lang");
    //Service commands process
    if(a_path == "/serv/attr") {	//Attribute's access
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {		//Get values
	    AutoHD<Attr> attr;
	    if(!opt->childSize()) {
		vector<string> ls;
		attrList(ls);
		for(unsigned iL = 0; iL < ls.size(); iL++) {
		    attr = attrAt(ls[iL]);
		    if(attr.at().flgGlob()&Attr::IsUser && !(attr.at().flgSelf()&Attr::VizerSpec)) continue;
		    opt->childAdd("el")->setAttr("id",ls[iL].c_str())->
					 setAttr("p",attr.at().fld().reserve())->
					 setText(attr.at().isTransl()?trLU(attr.at().getS(),l,u):attr.at().getS());
		}
	    }
	    else
		for(unsigned iL = 0; iL < opt->childSize(); iL++)
		    if(attrPresent(tNm=opt->childGet(iL)->attr("id"))) {
			attr = attrAt(tNm);
			opt->childGet(iL)->setAttr("p",attr.at().fld().reserve())->
					    setAttr("act",(attr.at().flgGlob()&Attr::Active)?"1":"0")->
					    setText(attr.at().isTransl()?trLU(attr.at().getS(),l,u):attr.at().getS());
		    }
	}
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	//Set values
	    for(unsigned iCh = 0; iCh < opt->childSize(); iCh++)
		try{ attrAt(opt->childGet(iCh)->attr("id")).at().setS(opt->childGet(iCh)->text()); }
		catch(TError&) { }
    }
    else if(a_path == "/serv/attrBr" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID,SEC_RD)) {	//Get attributes all updated elements' of the branch
	// Self attributes put
	vector<string> ls;
	attrList(ls);
	AutoHD<Attr> attr;
	for(unsigned iL = 0; iL < ls.size(); iL++) {
	    attr = attrAt(ls[iL]);
	    if(attr.at().flgGlob()&Attr::IsUser && !(attr.at().flgSelf()&Attr::VizerSpec)) continue;
	    opt->childAdd("el")->setAttr("id",ls[iL].c_str())->
			     setAttr("p",attr.at().fld().reserve())->
			     setText(attr.at().isTransl()?trLU(attr.at().getS(),l,u):attr.at().getS());
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
		ctrMkNode("fld",opt,-1,"/wdg/st/status",_("Status"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enabled"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),RWRWR_,"root",SUI_ID,3,"tp","str", "dest","sel_ed", "select","/wdg/w_lst");
		if(!parent().freeStat())
		    ctrMkNode("comm",opt,-1,"/wdg/st/goparent",_("Go to the parent"),RWRWR_,"root",SUI_ID,1,"tp","lnk");
		ctrMkNode("fld",opt,-1,"/wdg/st/use",_("Used"),R_R_R_,"root",SUI_ID,1,"tp","dec");
	    }
	    if(ctrMkNode("area",opt,-1,"/wdg/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/wdg/cfg/id",_("Identifier"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/type",_("Type"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/root",_("Root"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/path",_("Path"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/descr",_("Description"),RWRWR_,"root",SUI_ID,3,"tp","str","cols","100","rows","4");
		ctrMkNode("img",opt,-1,"/wdg/cfg/ico",_("Icon"),(isLink()?R_R_R_:RWRWR_),"root",SUI_ID,2,"v_sz","64","h_sz","64");
		ctrMkNode("comm",opt,-1,"/wdg/cfg/clear",_("Clear the widget changes"),RWRWR_,"root",SUI_ID);
		ctrMkNode("comm",opt,-1,"/wdg/cfg/chDown",_("Lower down the widget changes to its parent"),RWRWR_,"root",SUI_ID);
	    }
	}
	if(isContainer() && (!isLink()) && ctrMkNode("area",opt,-1,"/inclwdg",_("Included widgets"))) {
	    ctrMkNode("fld",opt,-1,"/inclwdg/nmb",_("Number"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","wdg_","idSz","30");
	}
	if(isContainer() && (!isLink()) && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user"), l = opt->attr("lang");
    if(a_path == "/wdg/res" && ctrChkNode(opt)) {	//Service command for resources request
	string mime;
	int size = s2i(opt->attr("size"));
	opt->setText(resourceGet(opt->attr("id"),&mime,s2i(opt->attr("off")),&size));
	opt->setAttr("mime", mime)->setAttr("size", i2s(size));
    }
    else if(a_path == "/wdg/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/wdg/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/wdg/st/use" && ctrChkNode(opt))		opt->setText(i2s(herit().size()));
    else if(a_path == "/wdg/st/parent") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(parentNm());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setParentNm(opt->text());
    }
    else if(a_path == "/wdg/st/goparent" && ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD) && !parent().freeStat())
	opt->setText(parent().at().nodePath(0,true));
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
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trLU(name(),l,u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(trSetLU(name(),l,u,opt->text()));
    }
    else if(a_path == "/wdg/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trLU(descr(),l,u));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(trSetLU(descr(),l,u,opt->text()));
    }
    else if(a_path == "/wdg/cfg/clear" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID)) {
	if(opt->attr("attr").empty()) wClear();
	else if(attrAt(opt->attr("attr")).at().modif()) {
	    attrAt(opt->attr("attr")).at().setModif(0);
	    inheritAttr(opt->attr("attr"));
	    modif();
	}
    }
    else if(a_path == "/wdg/cfg/chDown" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID))
	opt->setAttr("chParent", wChDown(opt->attr("attr")));
    else if(a_path == "/wdg/u_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    opt->childAdd("el")->setText(ls[iL]);
    }
    else if(a_path == "/wdg/g_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrGrpList(owner(), ls );
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    opt->childAdd("el")->setText(ls[iL]);
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
		//case 1:
		default:
		    if(TSYS::pathLev(lnk,0) != "..") mod->nodeAt(lnk).at().nodeList(ls);
			//mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeList(ls);
		    break;
		/*case 2:
		    if(TSYS::pathLev(lnk,0) != "..")
			mod->nodeAt(TSYS::pathLev(lnk,0)).at().nodeAt(TSYS::pathLev(lnk,1)).at().nodeList(ls,"wdg_");
		    break;*/
	    }
	    for(unsigned iL = 0; iL < ls.size(); iL++)
		opt->childAdd("el")->setText(c_path+"/"+ls[iL]);
	} catch(TError &err) { }
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
		    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(trLU(iwdg.at().name(),l,u));
		}
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string sid = TSYS::strEncode(opt->attr("id"), TSYS::oscdID);
	    if(wdgPresent(sid)) throw TError(nodePath().c_str(), _("Widget '%s' is already present!"), sid.c_str());
	    wdgAdd(sid, opt->text(), "");
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
	for(unsigned iW = 0; iW < c_list.size(); iW++)
	    if(wdgAt(c_list[iW]).at().enable()) e_c++;
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
		    else if(list_a[i_el] == "parent")	el->setAttr("help",_("Path to the parent widget."));
		    else if(list_a[i_el] == "owner")	el->setAttr("help",_("Owner and groups (separated by ',') of the widget in the form \"{owner}:{groups}\"."));
		    else if(list_a[i_el] == "perm")
			el->setAttr("help",_("Widget permission in the form \"{user}{group}{other}\".\n"
					     "Where, \"user\", \"group\" and \"other\" is:\n"
					     "  \"__\" - no access;\n"
					     "  \"R_\" - read only;\n"
					     "  \"RW\" - read and write."));
		    else if(list_a[i_el] == "evProc")
			el->setAttr("SnthHgl","1")->
			    setAttr("help",_("Direct events processing for manipulating pages in the form:\n"
					     "      \"{event}:{evSrc}:{com}:{prm}\". Where:\n"
					     "  \"event\" - waiting event;\n"
					     "  \"evSrc\" - event source;\n"
					     "  \"com\" - command of the session (open, next, prev);\n"
					     "  \"prm\" - command parameter, where used:\n"
					     "      pg_so - direct name of the desired page, with the prefix;\n"
					     "      1 - name of a new page in the common path, without a prefix;\n"
					     "      * - the page name is taken from the name of the previous open page;\n"
					     "      $ - indicates the position of the relative selection of the page to open.\n"
					     "Examples:\n"
					     "  ws_BtPress:/prev:prev:/pg_so/*/*/$\n"
					     "  ws_BtPress:/next:next:/pg_so/*/*/$\n"
					     "  ws_BtPress:/go_mn:open:/pg_so/*/mn/*\n"
					     "  ws_BtPress:/go_graph:open:/pg_so/*/ggraph"));
		    else switch(s2i(attr.at().fld().reserve())) {
			case A_ROOT:
			    el->setAttr("help",_("Primitive identifier that underlie the widget."));
			    break;
			case A_CTX_MENU:
			    el->setAttr("SnthHgl","1")->
				setAttr("help",_("Context menu in the form of the strings list: \"{ItName}:{Signal}\".\n"
						 "Where:\n"
						 "  \"ItName\" - item name;\n"
						 "  \"Signal\" - signal name, where the result signal name is \"usr_{Signal}\"."));
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
    string a_path = opt->attr("path"), u = opt->attr("user"), l = opt->attr("lang");
    if(a_path.compare(0,6,"/attr/") == 0) {
	AutoHD<Attr> attr = src->attrAt(TSYS::pathLev(a_path,1));
	if(ctrChkNode(opt,"get",(attr.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(attr.at().isTransl()?trLU(attr.at().getS(),l,u):attr.at().getS());
	else if(ctrChkNode(opt,"set",(attr.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SUI_ID,SEC_WR)) {
	    attr.at().setS(attr.at().isTransl()?trSetLU(attr.at().getS(),l,u,opt->text()):opt->text());
	    opt->setText(attr.at().isTransl()?trLU(attr.at().getS(),l,u):attr.at().getS());
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
	    TSYS::ctrListFS(opt, a_val.substr(5), "png;jpeg;jpg;gif;pcx;mng;svg;mp3;ogg;wav;avi;mov;mpg4;ogv;mp4");
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
	    ctrMkNode("fld",opt,-1,"/links/showAttr",_("Show attributes"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links"))) {
		bool shwAttr =	s2i(opt->attr("showAttr")) ||
				s2i(TBDS::genDBGet(mod->nodePath()+"showAttr","0",opt->attr("user")));
		vector<string> incllist, alist, list;
		wdgList(incllist);
		for(int iW = -1; iW < (int)incllist.size(); iW++) {
		    AutoHD<Widget> wdg;
		    if(iW < 0) wdg = AutoHD<Widget>(this);
		    else wdg = wdgAt(incllist[iW]);
		    wdg.at().attrList(alist);
		    for(unsigned iA = 0; iA < alist.size(); iA++) {
			string grpprm;
			string idprm = alist[iA];
			string nprm  = wdg.at().attrAt(alist[iA]).at().id();
			if(iW >= 0) {
			    idprm.insert(0,incllist[iW]+".");
			    nprm.insert(0,wdg.at().id()+".");
			}

			if(!(wdg.at().attrAt(alist[iA]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst)) ||
			    (!shwAttr && wdg.at().attrAt(alist[iA]).at().flgSelf()&Attr::CfgConst)) continue;
			// Get attributes
			bool shwTmpl = wdg.at().attrAt(alist[iA]).at().cfgTempl().size();
			if(shwTmpl)	grpprm = TSYS::strSepParse(wdg.at().attrAt(alist[iA]).at().cfgTempl(),0,'|');

			// Check select param
			if(shwTmpl && !shwAttr) {
			    nprm = grpprm;
			    if(iW >= 0) nprm.insert(0,wdg.at().id()+".");

			    // Check already to present parameters
			    bool f_ok = false;
			    for(unsigned iL = 0; iL < list.size() && !f_ok; iL++)
				if(list[iL] == nprm) f_ok = true;
			    if(!f_ok) {
				ctrMkNode("fld",opt,-1,(string("/links/lnk/pr_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,
				    3,"tp","str", "dest","sel_ed", "select",(string("/links/lnk/pl_")+idprm).c_str());
				list.push_back(nprm);
			    }
			}
			else {
			    XMLNode *nel = NULL;
			    if(wdg.at().attrAt(alist[iA]).at().flgSelf()&Attr::CfgConst)
				nel = ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,
					2,"tp","str","elGrp",grpprm.c_str());
			    else
				nel = ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+idprm).c_str(),nprm,(lnk_ro?R_R_R_:RWRWR_),"root",SUI_ID,
					4,"tp","str","dest","sel_ed","select",(string("/links/lnk/ls_")+idprm).c_str(),"elGrp",grpprm.c_str());
			    if(nel && s2i(opt->attr("inclValue"))) {
				nel->setText(wdg.at().attrAt(alist[iA]).at().cfgVal());
				if(wdg.at().attrAt(alist[iA]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
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
	if(nattr.size()) srcwdg = wdgAt(nwdg);
	else nattr = nwdg;
	string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	// Search first not config field if default field is config.
	if(srcwdg.at().attrAt(nattr).at().flgSelf()&Attr::CfgConst) {
	    srcwdg.at().attrList(a_ls);
	    for(unsigned iA = 0; iA < a_ls.size(); iA++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[iA]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[iA]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[iA]; break; }
	}

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    string cfg_val = srcwdg.at().attrAt(nattr).at().cfgVal();
	    string obj_tp  = (cfg_val.size() >= 4) ? cfg_val.substr(0,4) : ""; //TSYS::strSepParse(cfg_val,0,':')+":";

	    bool custom = false, lnkOK = false;
	    if(obj_tp == "prm:" || obj_tp == "wdg:") {
		if(cfg_val.rfind("/") != string::npos &&
		    ((obj_tp == "prm:" && !SYS->daq().at().attrAt(cfg_val.substr(4),0,true).freeStat()) ||
		    (obj_tp == "wdg:" && !srcwdg.at().attrAt(cfg_val.substr(4),0).freeStat())))
		{ cfg_val.resize(cfg_val.rfind("/")); lnkOK = true; }
	    } else custom = true;

	    string sel, rez;
	    srcwdg.at().attrList(a_ls);
	    for(unsigned iA = 0; iA < a_ls.size(); iA++)
		if(srcwdg.at().attrAt(a_ls[iA]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
		    p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[iA]).at().cfgTempl(),0,'|'))
		{
		    sel = srcwdg.at().attrAt(a_ls[iA]).at().cfgVal();
		    rez += (rez.size()?", ":"")+sel;
		    if(!custom && sel.size() && sel.find(cfg_val) != 0) custom = true;
		    if(!custom && !lnkOK) {
			string sel_tp  = (sel.size() >= 4) ? sel.substr(0,4) : "";
			if(sel.rfind("/") != string::npos &&
				((sel_tp == "prm:" && !SYS->daq().at().attrAt(sel.substr(4),0,true).freeStat()) ||
				(sel_tp == "wdg:" && !srcwdg.at().attrAt(sel.substr(4),0).freeStat())))
			    lnkOK = true;
		    }
		}
	    if(!custom) {
		rez = cfg_val;
		if(lnkOK) rez += " (+)";
	    }
	    else if(rez.size())
		rez = (cfg_val.size() < 4) ? cfg_val : string(_("Custom"))+": "+rez;

	    opt->setText(rez);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    string no_set;
	    string cfg_val = TSYS::strParse(opt->text(), 0, " ");
	    string obj_tp  = (cfg_val.size() >= 4) ? cfg_val.substr(0,4) : ""; //TSYS::strSepParse(cfg_val,0,':')+":";
	    string cfg_addr = (obj_tp.size()<cfg_val.size()) ? cfg_val.substr(obj_tp.size()) : "";

	    AutoHD<TValue> prm;
	    if(obj_tp == "prm:") prm = SYS->daq().at().prmAt(cfg_addr,0,true);
	    AutoHD<Widget> dstwdg;
	    if(obj_tp == "wdg:" && cfg_addr.size()) dstwdg = srcwdg.at().wdgAt(cfg_addr,0);

	    srcwdg.at().attrList(a_ls);
	    for(unsigned iA = 0; iA < a_ls.size(); iA++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[iA]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[iA]).at().flgSelf()&Attr::CfgConst))
		{
		    srcwdg.at().attrAt(a_ls[iA]).at().setCfgVal(cfg_val);
		    string p_attr = TSYS::strSepParse(srcwdg.at().attrAt(a_ls[iA]).at().cfgTempl(), 1, '|');
		    if(!prm.freeStat() || !dstwdg.freeStat()) {
			if((!prm.freeStat() && prm.at().vlPresent(p_attr)) ||
				(!dstwdg.freeStat() && dstwdg.at().attrPresent(p_attr)))
			    srcwdg.at().attrAt(a_ls[iA]).at().setCfgVal(cfg_val+((obj_tp == "wdg:")?"/a_":"/")+p_attr);
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
	    if(!is_pl) throw TError(nodePath().c_str(),_("The variable is not a link"));
	    vector<string> a_ls;
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    srcwdg.at().attrList(a_ls);
	    unsigned iA;
	    for(iA = 0; iA < a_ls.size(); iA++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[iA]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[iA]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[iA]; break; }
	    if(iA >= a_ls.size()) throw TError(nodePath().c_str(),_("The variable is not a link"));
	}

	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();

	// Link interface process
	int c_lv = 0;
	string obj_tp = (m_prm.size() >= 4) ? m_prm.substr(0,4) : ""; //TSYS::strSepParse(m_prm,0,':')+":";
	if(obj_tp.empty() || !(obj_tp == "val:" || obj_tp == "prm:" || obj_tp == "wdg:" || obj_tp == "arh:")) {
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
			c_path += (c_lv?"/":"") + c_el;
			opt->childAdd("el")->setText(c_path);
		    }

		    AutoHD<Widget> wnd = srcwdg.at().wdgAt(c_path.substr(4),0);
		    if(!wnd.freeStat()) {
			if(dynamic_cast<Widget*>(wnd.at().nodePrev())) opt->childAdd("el")->setText(c_path+(c_lv?"/..":".."));
			wnd.at().wdgList(ls, true);
			if(ls.size()) opt->childAdd("el")->setText(_("=== Widgets ==="));
			for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/wdg_":"wdg_")+ls[iL]);
			if(!is_pl) {
			    wnd.at().attrList(ls);
			    if(ls.size()) opt->childAdd("el")->setText(_("=== Attributes ==="));
			    for(unsigned iL = 0; iL < ls.size(); iL++)
				opt->childAdd("el")->setText(c_path+(c_lv?"/a_":"a_")+ls[iL]);
			}
		    }
		}
		else if(m_prm == "arh:") {
		    SYS->archive().at().valList(ls);
		    for(unsigned iL = 0; iL < ls.size(); iL++)
			opt->childAdd("el")->setText(c_path+ls[iL]);
		}
	    } catch(TError &err) { }
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
	if(ctrMkNode("area",opt,-1,"/proc",_("Processing"))) {
	    ctrMkNode("fld",opt,-1,"/proc/wdg",_("Widget"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/proc/w_lst");
	    wattr = TBDS::genDBGet(mod->nodePath()+"wdgAttr",".",opt->attr("user"));
	    if(!wdgPresent(wattr))	wattr = ".";
	    if(ctrMkNode("table",opt,-1,"/proc/attr",_("Attributes"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/proc/attr/id",_("Identifier"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/type",_("Type"),RWRWR_,"root",SUI_ID,4,"tp","dec","idm","1","dest","select","select","/proc/tp_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/wa",_("Work area"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/proc/attr/proc",_("Processing"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/proc/attr/cfg",_("Configuration"),RWRWR_,"root",SUI_ID,4,"tp","dec","idm","1","dest","select","select","/proc/lnk_ls");
		ctrMkNode("list",opt,-1,"/proc/attr/cfgtmpl",_("Configuration template"),RWRWR_,"root",SUI_ID,1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/proc/calc",_("Calculation"))) {
		ctrMkNode("fld",opt,-1,"/proc/calc/per",_("Period of the calculating, milliseconds"),RWRWR_,"root",SUI_ID,2,"tp","dec",
		    "help",_("Use -1 (< 0) if you want to use the parent widget/page/project calculating period."));
		ctrMkNode("fld",opt,-1,"/proc/calc/progLng",_("Procedure language"),RWRWR_,"root",SUI_ID,4,"tp","str", "dest","sel_ed", "select","/plang/list",
		    "help",_("Select the blank language to reset the widget procedure and language to the parent widget."));
		if(calcProg().size() && !(!parent().freeStat() && calcProg() == parent().at().calcProg()))
		    ctrMkNode("fld",opt,-1,"/proc/calc/prog_tr",_("Completely translate the procedure"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		if(calcLang().size())
		    ctrMkNode("fld",opt,-1,"/proc/calc/prog",_("Procedure"),RWRWR_,"root",SUI_ID,3,"tp","str","rows","10","SnthHgl","1");
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
		if(n_type) {
		    if(wdg.at().attrAt(lst[i_el]).at().fld().flg()&TFld::Selectable)
			n_type->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().fld().type()+
				    ((wdg.at().attrAt(lst[i_el]).at().fld().flg()&(TFld::Selectable))<<4)));
		    else
			n_type->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().fld().type()+
				    ((wdg.at().attrAt(lst[i_el]).at().fld().flg()&(TFld::FullText|TFld::TransltText|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
		}
		if(n_wa)	n_wa->childAdd("el")->setText( wdg.at().attrAt(lst[i_el]).at().fld().values()+"|"+
							    wdg.at().attrAt(lst[i_el]).at().fld().selNames());
		if(n_proc)	n_proc->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().flgSelf()&Attr::ProcAttr));
		if(n_cfg)	n_cfg->childAdd("el")->setText(i2s(wdg.at().attrAt(lst[i_el]).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle)));
		if(n_cfgtmpl)	n_cfgtmpl->childAdd("el")->setText(wdg.at().attrAt(lst[i_el]).at().cfgTempl());
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    string newAId = "newAttr", newANm = _("New attribute");
	    while(wdg.at().attrPresent(newAId)) {
		newAId = TSYS::strLabEnum(newAId);
		newANm = TSYS::strLabEnum(newANm);
	    }
	    wdg.at().attrAdd(new TFld(newAId.c_str(),newANm.c_str(),TFld::String,Attr::IsUser), -1, false, false, true);
	    //wdg.at().attrAt(newAId).at().setS(EVAL_STR);
	    wdg.at().attrAt(newAId).at().setModif(1);
	    wdg.at().modif();
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    AutoHD<Widget> wdg = (wattr==".")?AutoHD<Widget>(this):wdgAt(wattr);
	    if(!wdg.at().attrPresent(opt->attr("key_id")))
		throw TError(nodePath().c_str(), _("Attribute '%s' is not present."), opt->attr("key_id").c_str());
	    if(!(wdg.at().attrAt(opt->attr("key_id")).at().flgSelf()&Attr::IsInher) &&
		    wdg.at().attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsUser)
		wdg.at().attrDel(opt->attr("key_id"));
	    else throw TError(nodePath().c_str(), _("Unable to delete non-user attribute '%s'."), opt->attr("key_id").c_str());
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
		    throw TError(nodePath().c_str(), _("Unable to change non-user attribute '%s'."), idattr.c_str());

		string tvl	= wdg.at().attrAt(idattr).at().getS();
		string tmpl	= wdg.at().attrAt(idattr).at().cfgTempl();
		string cfgval	= wdg.at().attrAt(idattr).at().cfgVal();

		if(idcol == "id") {
		    tid = opt->text();
		    if(!tid.size()) throw err_sys(_("The new attribute identifier is empty!"));
		    if(wdg.at().attrPresent(tid)) throw err_sys(_("The new attribute identifier '%s' is already present!"), tid.c_str());
		}
		else if(idcol == "type") {
		    ttp = (TFld::Type)(s2i(opt->text())&0x0f);
		    tflg = tflg^((tflg^((s2i(opt->text())>>4)|Attr::IsUser))&(TFld::FullText|TFld::TransltText|TFld::Selectable|Attr::Color|Attr::Image|Attr::Font|Attr::Address));
		}
		wdg.at().attrDel(idattr);
		wdg.at().attrAdd(new TFld(tid.c_str(),tnm.c_str(),ttp,tflg,"","",tvals.c_str(),tsels.c_str()), -1, false, false, true);
		wdg.at().attrAt(tid).at().setS(tvl);
		wdg.at().attrAt(tid).at().setFlgSelf((Attr::SelfAttrFlgs)((sflgs&(~Attr::VizerSpec))|(wdg.at().attrAt(tid).at().flgSelf()&Attr::VizerSpec)));
		wdg.at().attrAt(tid).at().setCfgVal(cfgval);
		wdg.at().attrAt(tid).at().setCfgTempl(tmpl);
		wdg.at().attrAt(tid).at().setModif(1);
	    }
	    else {
		if(idcol == "name") {
		    if(!(!(wdg.at().attrAt(idattr).at().flgSelf()&Attr::IsInher) && wdg.at().attrAt(idattr).at().fld().flg()&Attr::IsUser))
			throw TError(nodePath().c_str(), _("Unable to change non-user attribute '%s'."), idattr.c_str());
		    wdg.at().attrAt(idattr).at().fld().setDescr(opt->text());
		}
		else if(idcol == "wa") {
		    wdg.at().attrAt(idattr).at().fld().setValues(TSYS::strSepParse(opt->text(),0,'|'));
		    wdg.at().attrAt(idattr).at().fld().setSelNames(TSYS::strSepParse(opt->text(),1,'|'));
		}
		else if(idcol == "proc") {
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = s2i(opt->text())?Attr::ProcAttr:(Attr::SelfAttrFlgs)0;
		    wdg.at().attrAt(idattr).at().setFlgSelf((Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::ProcAttr)));
		}
		else if(idcol == "cfg") {
		    Attr::SelfAttrFlgs sflg =  wdg.at().attrAt(idattr).at().flgSelf();
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)s2i(opt->text());
		    if((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle))
			wdg.at().attrAt(idattr).at().setFlgSelf((Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::CfgLnkIn|Attr::CfgLnkOut|Attr::CfgConst|Attr::FromStyle))));
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
    else if(a_path == "/proc/calc/prog_tr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(calcProgTr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcProgTr(s2i(opt->text()));
    }
    else if(a_path == "/proc/calc/prog") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    opt->setText(calcProg());
	    if(!parent().freeStat() && parent().at().calcProg().size())	opt->setAttr("inherited", "1");
	    if(!parent().freeStat() && parent().at().calcProg().size() && calcProg() != parent().at().calcProg())
		opt->setAttr("redefined", "1");
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCalcProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
	    try {
		SYS->daq().at().at(TSYS::strParse(calcLang(),0,".")).at().
				compileFuncSynthHighl(TSYS::strParse(calcLang(),1,"."),*opt);
	    } catch(...) { }
    }
    else if(a_path == "/proc/tp_ls" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(TFld::Boolean))->setText(_("Boolean"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Integer))->setText(_("Integer"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Real))->setText(_("Real"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String))->setText(_("String"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(TFld::TransltText<<4)))->setText(_("String (translate)"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Object))->setText(_("Object"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Integer+(TFld::Selectable<<4)))->setText(_("Integer numbers selection"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Real+(TFld::Selectable<<4)))->setText(_("Real numbers selection"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(TFld::Selectable<<4)))->setText(_("Strings selection"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(TFld::FullText<<4)))->setText(_("Text"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+((TFld::FullText|TFld::TransltText)<<4)))->setText(_("Text (translate)"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Color<<4)))->setText(_("Color"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Image<<4)))->setText(_("Image"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Font<<4)))->setText(_("Font"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::Integer+(Attr::DateTime<<4)))->setText(_("DateTime"));
	opt->childAdd("el")->setAttr("id",i2s(TFld::String+(Attr::Address<<4)))->setText(_("Address"));
    }
    else if(a_path == "/proc/lnk_ls" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(0))->setText(_("No"));
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
Attr::Attr( TFld *ifld, bool inher ) : mFld(NULL), mModif(0), mFlgSelf((SelfAttrFlgs)0), mConn(0), mOwner(NULL)
{
    setFld(ifld, inher);

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:VCAEngine:Attr", 1);
}

Attr::~Attr( )
{
    setFld(NULL, false);

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:VCAEngine:Attr", -1);
}

void Attr::setFld( TFld *fld, bool inher )
{
    if(owner()) owner()->mtxAttr().lock();
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
		if(fld->flg()&Attr::OnlyRead) break;
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
		if(fld->flg()&Attr::OnlyRead) break;
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
    mFlgSelf = inher ? mFlgSelf|Attr::IsInher : mFlgSelf & ~Attr::IsInher;
    if(owner()) owner()->mtxAttr().unlock();
}

const string &Attr::id( ) const	{ return fld().name(); }

string Attr::name( ) const	{ return fld().descr(); }

TFld::Type Attr::type( )	{ return fld().type(); }

int Attr::flgGlob( )		{ return fld().flg(); }

string Attr::getSEL( bool sys )
{
    if(!(fld().flg()&TFld::Selectable)) throw TError("Cfg", _("Element type is not selective!"));
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
    if(flgGlob()&Attr::OnlyRead || (flgGlob()&Attr::PreRead && !sys)) return owner()->vlGet(*this).getS();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this, getS(true), false).getS();
    switch(fld().type()) {
	case TFld::Integer:	{ int64_t tvl = getI(sys); return (tvl != EVAL_INT) ? ll2s(tvl) : EVAL_STR; }
	case TFld::Real:	{ double tvl = getR(sys); return (tvl != EVAL_REAL) ? r2s(tvl) : EVAL_STR; }
	case TFld::Boolean:	{ char tvl = getB(sys); return (tvl != EVAL_BOOL) ? i2s((bool)tvl) : EVAL_STR; }
	case TFld::Object:	{ AutoHD<TVarObj> tvl = getO(sys); return (tvl.at().objName() != "EVAL") ? tvl.at().getStrXML() : EVAL_STR; }
	case TFld::String: {
	    owner()->mtxAttr().lock();
	    string tvl = *mVal.s;
	    owner()->mtxAttr().unlock();
	    return tvl;
	}
	default: break;
    }

    return EVAL_STR;
}

int64_t Attr::getI( bool sys )
{
    if(flgGlob()&Attr::OnlyRead || (flgGlob()&Attr::PreRead && !sys)) return owner()->vlGet(*this).getI();
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
    if(flgGlob()&Attr::OnlyRead || (flgGlob()&Attr::PreRead && !sys)) return owner()->vlGet(*this).getR();
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
    if(flgGlob()&Attr::OnlyRead || (flgGlob()&Attr::PreRead && !sys)) return owner()->vlGet(*this).getB();
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
    if(flgGlob()&Attr::OnlyRead || (flgGlob()&Attr::PreRead && !sys)) return owner()->vlGet(*this).getO();
    if(flgSelf()&Attr::FromStyle && !sys) return owner()->stlReq(*this,getO(true),false).getO();
    if(fld().type() != TFld::Object) return new TEValObj;
    owner()->mtxAttr().lock();
    AutoHD<TVarObj> tvl = *mVal.o;
    owner()->mtxAttr().unlock();

    return tvl;
}

void Attr::setSEL( const string &val, bool strongPrev, bool sys )
{
    if(flgGlob()&Attr::OnlyRead) return;
    if(!(fld().flg()&TFld::Selectable)) throw TError("Cfg",_("Element type is not selective!"));
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
    if(flgGlob()&Attr::OnlyRead) return;
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
    if(flgGlob()&Attr::OnlyRead) return;
    switch(fld().type()) {
	case TFld::Integer:	setI((val!=EVAL_STR) ? s2ll(val) : EVAL_INT, strongPrev, sys);	break;
	case TFld::Real:	setR((val!=EVAL_STR) ? s2r(val) : EVAL_REAL, strongPrev, sys);	break;
	case TFld::Boolean:	setB((val!=EVAL_STR) ? (bool)s2i(val) : EVAL_BOOL, strongPrev, sys);	break;
	case TFld::Object:
	    setO((val!=EVAL_STR) ? TVarObj::parseStrXML(val,NULL,getO()) : AutoHD<TVarObj>(new TEValObj), strongPrev, sys);
	    break;
	case TFld::String: {
	    if((!strongPrev && *mVal.s == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull())) break;
	    owner()->mtxAttr().lock();
	    string t_str = *mVal.s;
	    *mVal.s = val;
	    owner()->mtxAttr().unlock();
	    if(!sys && !owner()->attrChange(*this,TVariant(t_str))) {
		owner()->mtxAttr().lock();
		*mVal.s = t_str;
		owner()->mtxAttr().unlock();
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
    if(flgGlob()&Attr::OnlyRead) return;
    switch(fld().type()) {
	case TFld::String:	setS((val!=EVAL_INT) ? ll2s(val) : EVAL_STR, strongPrev, sys);	break;
	case TFld::Real:	setR((val!=EVAL_INT) ? val : EVAL_REAL, strongPrev, sys);		break;
	case TFld::Boolean:	setB((val!=EVAL_INT) ? (bool)val : EVAL_BOOL, strongPrev, sys);		break;
	case TFld::Object:	if(val == EVAL_INT) setO(new TEValObj(), strongPrev, sys);	break;
	case TFld::Integer: {
	    if(!(fld().flg()&TFld::Selectable) && fld().selValI()[0] < fld().selValI()[1])
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
    if(flgGlob()&Attr::OnlyRead) return;
    switch(fld().type()) {
	case TFld::String:	setS((val!=EVAL_REAL) ? r2s(val) : EVAL_STR, strongPrev, sys);	break;
	case TFld::Integer:	setI((val!=EVAL_REAL) ? (int64_t)val : EVAL_INT, strongPrev, sys);	break;
	case TFld::Boolean:	setB((val!=EVAL_REAL) ? (bool)val : EVAL_BOOL, strongPrev, sys);break;
	case TFld::Object:	if(val == EVAL_REAL) setO(new TEValObj(), strongPrev, sys);	break;
	case TFld::Real: {
	    if(!(fld().flg()&TFld::Selectable) && fld().selValR()[0] < fld().selValR()[1])
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
    if(flgGlob()&Attr::OnlyRead) return;
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
    if(flgGlob()&Attr::OnlyRead)	return;
    switch(fld().type()) {
	case TFld::String:	setS(val.at().getStrXML(), strongPrev, sys);break;
	case TFld::Integer: case TFld::Real: case TFld::Boolean:
				setB(true, strongPrev, sys);		break;
	case TFld::Object: {
	    if((!strongPrev && *mVal.o == val) ||
		(flgSelf()&Attr::FromStyle && !sys && owner()->stlReq(*this,val,true).isNull())) break;
	    owner()->mtxAttr().lock();
	    AutoHD<TVarObj> t_obj = *mVal.o;
	    *mVal.o = val;
	    owner()->mtxAttr().unlock();
	    if(!sys && !owner()->attrChange(*this,TVariant(t_obj))) {
		owner()->mtxAttr().lock();
		*mVal.o = t_obj;
		owner()->mtxAttr().unlock();
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
    owner()->mtxAttr().lock();
    string tvl = cfg.substr(0,cfg.find("\n"));
    owner()->mtxAttr().unlock();
    return tvl;
}

string Attr::cfgVal( )
{
    owner()->mtxAttr().lock();
    size_t sepp = cfg.find("\n");
    string tvl = (sepp!=string::npos) ? cfg.substr(sepp+1) : "";
    owner()->mtxAttr().unlock();
    return tvl;
}

void Attr::setCfgTempl( const string &vl )
{
    string t_tmpl = cfgTempl();
    if(t_tmpl == vl) return;
    owner()->mtxAttr().lock();
    cfg = vl+"\n"+cfgVal();
    owner()->mtxAttr().unlock();
    if(!owner()->attrChange(*this,TVariant())) {
	owner()->mtxAttr().lock();
	cfg = t_tmpl+"\n"+cfgVal();
	owner()->mtxAttr().unlock();
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
    owner()->mtxAttr().lock();
    cfg = cfgTempl() + "\n" + vl;
    owner()->mtxAttr().unlock();

    if(!owner()->attrChange(*this,TVariant())) {
	owner()->mtxAttr().lock();
	cfg = cfgTempl() + "\n" + t_val;
	owner()->mtxAttr().unlock();
    }
    else {
	unsigned imdf = owner()->modifVal(*this);
	mModif = imdf ? imdf : mModif+1;
    }
}

void Attr::setFlgSelf( SelfAttrFlgs flg, bool sys )
{
    if(mFlgSelf == flg)	return;
    SelfAttrFlgs t_flg = (SelfAttrFlgs)mFlgSelf;
    mFlgSelf = (flg & ~Attr::IsInher) | (t_flg&Attr::IsInher);
    if(sys) return;
    if(!owner()->attrChange(*this,TVariant()))	mFlgSelf = t_flg;
    else {
	unsigned imdf = owner()->modifVal(*this);
	mModif = imdf ? imdf : mModif+1;
    }
}

void Attr::AHDConnect( )
{
    owner()->dataRes().lock();
    bool noLim = (mConn < ((1<<ATTR_CON_DEPTH)-1));
    if(noLim) mConn++;
    owner()->dataRes().unlock();
    if(!noLim) mess_err(owner()->nodePath().c_str(),_("Connects to the attribute '%s' is more than %d!"),id().c_str(),(1<<ATTR_CON_DEPTH)-1);
}

bool Attr::AHDDisConnect( )
{
    owner()->dataRes().lock();
    bool noLim = (mConn > 0);
    if(noLim) mConn--;
    owner()->dataRes().unlock();
    if(!noLim) mess_err(owner()->nodePath().c_str(),_("Disconnects from the attribute '%s' more than connections!"),id().c_str());
    return false;
}

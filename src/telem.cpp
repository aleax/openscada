
//OpenSCADA system file: telem.cpp
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include "tmess.h"
#include "tbds.h"
#include "telem.h"

using namespace OSCADA;

//*************************************************
//* TElem                                         *
//*************************************************
TElem::TElem( const string &name ) : mName(name), mResEl(true)
{

}

TElem::~TElem( )
{
    while(cont.size())	cont[0]->detElem(this);
    fldClear();
}

int TElem::fldAdd( TFld *fld, int id )
{
    MtxAlloc res(mResEl, true);

    //Find dublicates
    for(unsigned iF = 0; iF < elem.size(); iF++)
	if(elem[iF]->name() == fld->name())
	{ delete fld; return iF; }
    if(id > (int)elem.size() || id < 0) id = elem.size();
    elem.insert(elem.begin()+id, fld);

    //Add value and set them default
    for(unsigned cfgI = 0; cfgI < cont.size(); cfgI++)
	cont[cfgI]->addFld(this, id);

    return id;
}

void TElem::fldDel( unsigned int id )
{
    MtxAlloc res(mResEl, true);
    if(id >= elem.size()) throw TError("Elem",_("Id error!"));
    for(unsigned cfgI = 0; cfgI < cont.size(); cfgI++)
	cont[cfgI]->delFld(this, id);
    delete elem[id];
    elem.erase(elem.begin()+id);
}

void TElem::valAtt( TValElem *cnt )
{
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < cont.size(); i++)
	if(cont[i] == cnt) throw TError("Elem",_("The element container is already attached!"));
    cont.push_back(cnt);
}

void TElem::valDet( TValElem *cnt )
{
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < cont.size(); i++)
	if(cont[i] == cnt) { cont.erase(cont.begin()+i); break; }
}

unsigned TElem::fldId( const string &name, bool noex )
{
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < elem.size(); i++)
	if(elem[i]->name() == name) return i;
    res.unlock();

    if(noex) return fldSize();
    throw TError("Elem",_("Element '%s' is not present!"),name.c_str());
}

bool TElem::fldPresent( const string &name )
{
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < elem.size(); i++)
	if(elem[i]->name() == name) return true;
    return false;
}

void TElem::fldList( vector<string> &list )
{
    list.clear();
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < elem.size(); i++)
	list.push_back(elem[i]->name());
}

TFld &TElem::fldAt( unsigned int id )
{
    if(id >= elem.size()) throw TError("Elem",_("Id error!"));
    return *elem[id];
}

void TElem::fldClear( )
{
    while(elem.size())	fldDel(0);
}

//*************************************************
//* TFld - field of element                       *
//*************************************************
TFld::TFld( ) : mType(TFld::Integer), mFlg(0)
{
    mSel = NULL;
    mVal.s = NULL;
}

TFld::TFld( TFld &ifld, const char *name ) : mLen(0), mDec(0), mType(TFld::Integer), mFlg(0)
{
    mSel	= NULL;
    mVal.s	= NULL;

    mName	= name ? string(name) : ifld.name();
    mDescr	= ifld.descr();
    mType	= ifld.type();
    mFlg	= ifld.flg();
    mDef	= ifld.def();
    mRes	= ifld.reserve();
    mLen	= ifld.len();
    mDec	= ifld.dec();

    setValues(ifld.values());
    setSelNames(ifld.selNames());
}

TFld::TFld( const char *name, const char *descr, TFld::Type itype, unsigned iflg,
            const char *valLen, const char *valDef, const char *val_s, const char *n_Sel, const char *ires ) :
    mType(TFld::Integer), mFlg(0)
{
    mSel   = NULL;
    mVal.s = NULL;

    mName  = name;
    mDescr = descr;
    mType  = itype;
    mFlg   = iflg;
    mDef   = valDef;
    mRes   = ires;

    int ilen = 0, idec = 0;
    sscanf(valLen,"%d.%d",&ilen,&idec);
    mLen = ilen;
    mDec = idec;

    setValues(val_s);
    setSelNames(n_Sel);
}

TFld::~TFld( )
{
    if(mSel)	delete mSel;
    if(mVal.s != NULL)
	switch(type()) {
	    case TFld::String:	delete mVal.s;	break;
	    case TFld::Integer:	delete mVal.i;	break;
	    case TFld::Real:	delete mVal.r;	break;
	    case TFld::Boolean:	delete mVal.b;	break;
	    default: break;
	}
}

TFld::Type TFld::type( IO::Type tp )
{
    switch(tp) {
	case IO::String:  return String;
	case IO::Integer: return Integer;
	case IO::Real:    return Real;
	case IO::Boolean: return Boolean;
	case IO::Object:  return Object;
	default: break;
    }
    return String;
}

IO::Type TFld::typeIO( )
{
    switch(type()) {
	case Boolean:	return IO::Boolean;
	case Integer:	return IO::Integer;
	case Real:	return IO::Real;
	case String:	return IO::String;
	case Object:	return IO::Object;
	default: break;
    }
    return IO::String;
}

void TFld::setFlg( unsigned iflg )
{
    unsigned ch_flg = iflg^flg();
    if(ch_flg&SelfFld)	iflg = iflg^(ch_flg&SelfFld);
    if(ch_flg&Selected)	iflg = iflg^(ch_flg&Selected);
    mFlg = iflg;
}

string TFld::values( )
{
    if(mVal.s == NULL) return "";

    string rez;
    switch(type()) {
	case TFld::String:
	    for(unsigned i_el = 0; i_el < mVal.s->size(); i_el++)
		rez = rez + (*mVal.s)[i_el] + ";";
	    break;
	case TFld::Integer:
	    if(flg()&TFld::Selected || (mVal.i->size() == 2 && (*mVal.i)[0] < (*mVal.i)[1]))
		for(unsigned i_el = 0; i_el < mVal.i->size(); i_el++)
		    rez = rez + i2s((*mVal.i)[i_el]) + ";";
	    break;
	case TFld::Real:
	    if(flg()&TFld::Selected || (mVal.i->size() == 2 && (*mVal.i)[0] < (*mVal.i)[1]))
		for(unsigned i_el = 0; i_el < mVal.r->size(); i_el++)
		    rez = rez + r2s((*mVal.r)[i_el],6) + ";";
	    break;
	case TFld::Boolean:
	    for(unsigned i_el = 0; i_el < mVal.b->size(); i_el++)
		rez = rez + i2s((*mVal.b)[i_el]) + ";";
	    break;
	default: break;
    }
    return rez.size()?rez.substr(0,rez.size()-1):"";
}

string TFld::selNames( )
{
    if(mSel == NULL) return "";

    string rez;
    for(unsigned i_el = 0; i_el < mSel->size(); i_el++)
	rez = rez+(*mSel)[i_el]+";";

    return rez.size()?rez.substr(0,rez.size()-1):"";
}

void TFld::setValues( const string &vls )
{
    //Set value list
    if(flg()&TFld::Selected) {
	//Count alements amount
	int i_lvl = 0, i_off = 0;
	while(TSYS::strSepParse(vls,0,';',&i_off).size()) i_lvl++;

	switch(type()) {
	    case TFld::String:
		if(!mVal.s)	mVal.s = new vector<string>;
		mVal.s->resize(i_lvl,"");
		break;
	    case TFld::Integer:
		if(!mVal.i)	mVal.i = new vector<int>;
		mVal.i->resize(i_lvl,0);
		break;
	    case TFld::Real:
		if(!mVal.r)	mVal.r = new vector<double>;
		mVal.r->resize(i_lvl,0);
		break;
	    case TFld::Boolean:
		if(!mVal.b)	mVal.b = new vector<bool>;
		mVal.b->resize(i_lvl,false);
		break;
	    default: break;
	}
	//Get elements
	for(int i = 0, i_off=0; i < i_lvl; i++) {
	    string s_el = TSYS::strSepParse(vls,0,';',&i_off);
	    switch(type()) {
		case TFld::String:  (*mVal.s)[i] = s_el;		break;
		case TFld::Integer: (*mVal.i)[i] = strtol(s_el.c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));	break;
		case TFld::Real:    (*mVal.r)[i] = s2r(s_el);	break;
		case TFld::Boolean: (*mVal.b)[i] = s2i(s_el);	break;
		default: break;
	    }
	}
    }
    else
	switch(type()) {
	    case TFld::Integer:
		if(!mVal.i)	mVal.i = new vector<int>;
		mVal.i->resize(2,0);
		(*mVal.i)[0] = strtol(TSYS::strSepParse(vls,0,';').c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));
		(*mVal.i)[1] = strtol(TSYS::strSepParse(vls,1,';').c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));
		break;
	    case TFld::Real:
		if(!mVal.r)	mVal.r = new vector<double>;
		mVal.r->resize(2,0);
		(*mVal.r)[0] = s2r(TSYS::strSepParse(vls,0,';'));
		(*mVal.r)[1] = s2r(TSYS::strSepParse(vls,1,';'));
		break;
	    default: break;
	}
}

void TFld::setSelNames( const string &slnms )
{
    //Set value list
    if(!(flg()&TFld::Selected)) return;

    int i_lvl = 0;
    for(int i_off = 0; TSYS::strSepParse(slnms,0,';',&i_off).size(); i_lvl++);

    if(!mSel)  mSel = new vector<string>;
    mSel->resize(i_lvl,"");

    for(int i = 0, i_off = 0; i < i_lvl; i++)
	(*mSel)[i] = TSYS::strSepParse(slnms,0,';',&i_off);
}

const vector<string> &TFld::selValS( )
{
    if(flg()&TFld::Selected && type() == TFld::String) return *mVal.s;
    throw TError("Field",_("Field is not String."));
}

const vector<int> &TFld::selValI( )
{
    if(type() == TFld::Integer) return *mVal.i;
    throw TError("Field",_("Field is not Integer."));
}

const vector<double> &TFld::selValR( )
{
    if(type() == TFld::Real) return *mVal.r;
    throw TError("Field",_("Field is not Real."));
}

const vector<bool> &TFld::selValB( )
{
    if(flg()&TFld::Selected && type() == TFld::Boolean) return *mVal.b;
    throw TError("Field",_("Field is not Boolean."));
}

const vector<string> &TFld::selNm( )
{
    if(mSel && flg()&TFld::Selected) return *mSel;
    throw TError("Field",_("Field is not select type!"));
}

TFld &TFld::operator=( TFld &fld )
{
    //Free old
    if(mSel)	delete mSel;
    if(mVal.s != NULL)
	switch(type()) {
	    case TFld::String:	delete mVal.s;	break;
	    case TFld::Integer:	delete mVal.i;	break;
	    case TFld::Real:	delete mVal.r;	break;
	    case TFld::Boolean:	delete mVal.b;	break;
	    default: break;
	}
    //Create new
    mName	= fld.name();
    mDescr	= fld.descr();
    mLen	= fld.len();
    mFlg	= fld.flg();
    mType	= fld.type();
    mDef	= fld.def();
    //mVals  = fld.vals();

    //Copy select and values border
    if(flg()&TFld::Selected) {
	mSel  = new vector<string>;
	*mSel = fld.selNm();
	switch(type()) {
	    case TFld::String:	mVal.s = new vector<string>;	*(mVal.s) = fld.selValS();	break;
	    case TFld::Integer:	mVal.i = new vector<int>;	*(mVal.i) = fld.selValI();	break;
	    case TFld::Real:	mVal.r = new vector<double>;	*(mVal.r) = fld.selValR();	break;
	    case TFld::Boolean:	mVal.b = new vector<bool>;	*(mVal.b) = fld.selValB();	break;
	    default: break;
	}
    }
    else
	switch(type()) {
	    case TFld::Integer:	mVal.i = new vector<int>;	*(mVal.i) = fld.selValI();	break;
	    case TFld::Real:	mVal.r = new vector<double>;	*(mVal.r) = fld.selValR();	break;
	    default: break;
	}
    return *this;
}

string TFld::selVl2Nm( const string &val )
{
    if(flg()&TFld::Selected && type() == TFld::String) {
	int sz = vmin(mSel->size(), mVal.s->size());
	if(!sz) return _("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*mVal.s)[i_val] == val) break;
	if(i_val >= sz) return val;
	return (*mSel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%s'."), val.c_str());
}

string TFld::selVl2Nm( int64_t val )
{
    if(flg()&TFld::Selected && type() == TFld::Integer) {
	int sz = vmin(mSel->size(), mVal.i->size());
	if(!sz) return _("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*mVal.i)[i_val] == val) break;
	if(i_val >= sz) return i2s(val);
	return (*mSel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%d'."), val);
}

string TFld::selVl2Nm( double val )
{
    if(flg()&TFld::Selected && type() == TFld::Real) {
	int sz = vmin(mSel->size(), mVal.r->size());
	if(!sz) return _("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*mVal.r)[i_val] == val) break;
	if(i_val >= sz) return r2s(val);
	return (*mSel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%f'."), val);
}

string TFld::selVl2Nm( bool val )
{
    if(flg()&TFld::Selected && type() == TFld::Boolean) {
	int sz = vmin(mSel->size(), mVal.b->size());
	if(!sz) return _("Empty");
	int i_val;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*mVal.b)[i_val] == val) break;
	if(i_val >= sz) return i2s(val);
	return (*mSel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%d'."), val);
}

string TFld::selNm2VlS( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::String)
	for(unsigned i_val = 0; i_val < vmin(mSel->size(), mVal.s->size()); i_val++)
	    if(name == (*mSel)[i_val])
		return (*mVal.s)[i_val];
    return name;
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

int64_t TFld::selNm2VlI( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::Integer)
	for(unsigned i_val = 0; i_val < vmin(mSel->size(), mVal.i->size()); i_val++)
	    if(name == (*mSel)[i_val])
		return (*mVal.i)[i_val];
    return s2i(name);
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

double TFld::selNm2VlR( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::Real)
	for(unsigned i_val = 0; i_val < vmin(mSel->size(), mVal.r->size()); i_val++)
	    if(name == (*mSel)[i_val])
		return (*mVal.r)[i_val];
    return s2r(name);
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

bool TFld::selNm2VlB( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::Boolean)
	for(unsigned i_val = 0; i_val < vmin(mSel->size(), mVal.b->size()); i_val++)
	    if(name == (*mSel)[i_val])
		return (*mVal.b)[i_val];
    return s2i(name);
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

XMLNode *TFld::cntrCmdMake( XMLNode *opt, const string &path, int pos, const string &user, const string &grp, int perm )
{
    XMLNode *n_e = TCntrNode::ctrMkNode("fld",opt,pos,(path+"/"+name()).c_str(),descr(),
	    (flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),1,"len",i2s(len()).c_str());
    if(n_e) {
	if(flg()&TFld::Selected)
	    n_e->setAttr("tp","str")->setAttr("len","")->setAttr("dest",(flg()&TFld::SelEdit)?"sel_ed":"select")->
		setAttr("sel_id",values())->setAttr("sel_list",selNames());
	else switch(type()) {
	    case TFld::String:
		n_e->setAttr("tp","str");
		if(flg()&FullText)	n_e->setAttr("cols","100")->setAttr("rows","4");
		break;
	    case TFld::Integer:	n_e->setAttr("tp",(flg()&HexDec)?"hex":((flg()&OctDec)?"oct":"dec"));	break;
	    case TFld::Real:	n_e->setAttr("tp","real");	break;
	    case TFld::Boolean:	n_e->setAttr("tp","bool");	break;
	    case TFld::Object:	n_e->setAttr("tp","str")->setAttr("cols","100")->setAttr("rows","4");	break;
	    default: break;
	}
    }
    return n_e;
}

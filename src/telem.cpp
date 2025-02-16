
//OpenSCADA file: telem.cpp
/***************************************************************************
 *   Copyright (C) 2003-2025 by Roman Savochenko, <roman@oscada.org>       *
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

TElem::TElem( const TElem &src ) : mName(src.mName), mResEl(true)
{
    operator=(src);
}

TElem &TElem::operator=( const TElem &src )
{
    fldClear();

    for(unsigned iEl = 0; iEl < src.fldSize(); iEl++)
	fldAdd(new TFld(src.fldAt(iEl)));

    return *this;
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
    if(id >= elem.size()) throw TError("Elem", _("Error ID!"));
    for(unsigned cfgI = 0; cfgI < cont.size(); cfgI++)
	cont[cfgI]->delFld(this, id);
    delete elem[id];
    elem.erase(elem.begin()+id);
}

void TElem::valAtt( TValElem *cnt )
{
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < cont.size(); i++)
	if(cont[i] == cnt) throw TError("Elem", _("Container element already connected!"));
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
    throw TError(TError::Core_NoNode, "Elem", _("Element '%s' is not present!"), name.c_str());
}

bool TElem::fldPresent( const string &name )
{
    MtxAlloc res(mResEl, true);
    for(unsigned i = 0; i < elem.size(); i++)
	if(elem[i]->name() == name) return true;
    return false;
}

void TElem::fldList( vector<string> &list ) const
{
    list.clear();
    MtxAlloc res(const_cast<ResMtx&>(mResEl), true);
    for(unsigned i = 0; i < elem.size(); i++)
	list.push_back(elem[i]->name());
}

TFld &TElem::fldAt( unsigned int id ) const
{
    if(id >= elem.size()) throw TError("Elem", _("Error ID!"));
    return *elem[id];
}

void TElem::fldClear( )
{
    while(elem.size())	fldDel(0);
}

//*************************************************
//* TFld - field of element                       *
//*************************************************
TFld::TFld( ) : mType(TFld::Integer), mFlg(0), mVals(NULL), mSels(NULL)
{

}

TFld::TFld( const TFld &ifld, const char *name ) : mLen(0), mDec(0), mType(TFld::Integer), mFlg(0), mVals(NULL), mSels(NULL)
{
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

TFld::TFld( const char *name, const string &descr, TFld::Type itype, unsigned iflg,
            const char *valLen, const char *valDef, const string &val_s, const string &n_Sel, const char *ires ) :
    mType(TFld::Integer), mFlg(0), mVals(NULL), mSels(NULL)
{
    mName  = name;
    mDescr = descr;
    mType  = itype;
    mFlg   = iflg;
    mDef   = valDef;
    mRes   = ires;

    int ilen = 0, idec = 0;
    sscanf(valLen, "%d.%d", &ilen, &idec);
    mLen = ilen;
    mDec = idec;

    setValues(val_s);
    setSelNames(n_Sel);
}

TFld::~TFld( )
{
    if(mVals)	delete mVals;
    if(mSels)	delete mSels;
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

IO::Type TFld::typeIO( ) const
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
    if(ch_flg&(SelfFld|Selectable)) iflg = flg()^(ch_flg&(SelfFld|Selectable));
    mFlg = iflg;
}

void TFld::setValues( const string &vls )
{
    if(vls.size()) {
	if(!mVals) mVals = new string;
	*mVals = vls;
    }
    else if(mVals) { delete mVals; mVals = NULL; }
}

void TFld::setSelNames( const string &slnms )
{
    if(slnms.size()) {
	if(!mSels) mSels = new string;
	*mSels = slnms;
    }
    else if(mSels) { delete mSels; mSels = NULL; }
}

TFld &TFld::operator=( const TFld &fld )
{
    mName	= fld.name();
    mDescr	= fld.descr();
    mLen	= fld.len();
    mFlg	= fld.flg();
    mType	= fld.type();
    mDef	= fld.def();
    setValues(fld.values());
    setSelNames(fld.selNames());

    return *this;
}

string TFld::lenS( ) const	{ return i2s(len())+"."+i2s(dec()); }

XMLNode *TFld::cntrCmdMake( TCntrNode *cntrO, XMLNode *opt, const string &path, int pos, const string &owner, const string &group, int perm )
{
    string dscr = trD(descr());
    int dOff = 0; string dscr1 = TSYS::strLine(dscr, 0, &dOff);

    XMLNode *nE = cntrO->ctrMkNode("fld",opt,pos,(path+"/"+name()).c_str(),dscr1,
	    (flg()&TFld::NoWrite)?(perm&~_W_W_W):perm,owner.c_str(),group.c_str(),1, "len",i2s(len()).c_str());
    if(nE) {
	if(dOff < (int)dscr.size())
	    nE->setAttr("help", dscr.substr(dOff));
	if(flg()&TFld::Selectable) {
	    nE->setAttr("tp","str")->setAttr("len","")->setAttr("dest",(flg()&TFld::SelEdit)?"sel_ed":"select");
	    if(trD(selNames()).empty())	nE->setAttr("sel_list", trD(values()));		//!!!! With treating empty selNames()
	    else nE->setAttr("sel_id",trD(values()))->setAttr("sel_list",trD(selNames()));
	}
	else switch(type()) {
	    case TFld::String:
		nE->setAttr("tp","str");
		if(flg()&FullText)	nE->/*setAttr("cols","100")->*/setAttr("rows","4");
		break;
	    case TFld::Integer:	nE->setAttr("tp",(flg()&HexDec)?"hex":((flg()&OctDec)?"oct":"dec"));	break;
	    case TFld::Real:	nE->setAttr("tp","real");	break;
	    case TFld::Boolean:	nE->setAttr("tp","bool");	break;
	    case TFld::Object:	nE->setAttr("tp","str")->setAttr("cols","100")->setAttr("rows","4");	break;
	    default: break;
	}
    }
    return nE;
}

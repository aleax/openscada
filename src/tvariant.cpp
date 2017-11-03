
//OpenSCADA system file: tvariant.cpp
/***************************************************************************
 *   Copyright (C) 2010-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <pcre.h>

#include <tsys.h>
#include "tvariant.h"

using namespace OSCADA;

//*************************************************
//* TVariant                                      *
//*************************************************
TVariant::TVariant( ) : mType(Null), mModify(false), mFixedTp(false)
{
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( bool ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setB(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( char ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setB(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( int ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setI(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( int64_t ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setI(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( double ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setR(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( const string &ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setS(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( const char *ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setS(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( AutoHD<TVarObj> ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setO(ivl);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( TVarObj *ivl ) : mType(Null), mModify(false), mFixedTp(false)
{
    setO(AutoHD<TVarObj>(ivl));
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::TVariant( const TVariant &var ) : mType(Null), mModify(false), mFixedTp(false)
{
    operator=(var);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVariant::~TVariant( )
{
    setType(Null);
    //if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

void TVariant::setType( Type tp, bool fix, bool stdStringOmit )
{
    mFixedTp = fix;
    if(tp == type()) return;

    //Free
    switch(mType) {
	case String:
	    if(mStdString) delete val.s;
	    else if(mSize >= sizeof(val.sMini))	free(val.sPtr);
	    mSize = 0;
	    break;
	case Object:
	    if(val.o) delete val.o;
	    val.o = NULL;
	    break;
	default: break;
    }

    //Create
    mType = tp;
    switch(mType) {
	/*case Boolean:	setB(EVAL_BOOL);	break;
	case Integer:	setI(EVAL_INT);		break;
	case Real:	setR(EVAL_REAL);	break;*/
	case String:	mSize = 0; val.sMini[mSize] = 0; mStdString = false; mStdStringOmit = stdStringOmit; /*setS(EVAL_STR);*/	break;
	case Object:	val.o = new AutoHD<TVarObj>();	break;
	default: break;
    }
}

bool TVariant::operator==( const TVariant &vr ) const
{
    if(vr.type() == type())
	switch(type()) {
	    case Boolean:	return (vr.getB()==getB());
	    case Integer:	return (vr.getI()==getI());
	    case Real:		return (vr.getR()==getR());
	    case String:	return (vr.getS()==getS());
	    case Object:	return (vr.getO()==getO());
	    default: break;
	}

    return false;
}

bool TVariant::operator!=( const TVariant &vr ) const	{ return !operator==(vr); }

TVariant &TVariant::operator=( const TVariant &vr )
{
    switch(vr.type()) {
	case Null:
	    if(!mFixedTp) setType(Null);
	    else setS("");
	    break;
	case Boolean:	setB(vr.getB());	break;
	case Integer:	setI(vr.getI());	break;
	case Real:	setR(vr.getR());	break;
	case String:	setS(vr.getS()); mStdStringOmit = vr.mStdStringOmit;	break;
	case Object:	setO(vr.getO());	break;
    }

    return *this;
}

bool TVariant::isEVal( ) const
{
    switch(type()) {
	case String:	return (getS()==EVAL_STR);
	case Integer:	return (getI()==EVAL_INT);
	case Real:	return (getR()==EVAL_REAL);
	case Boolean:	return (getB()==EVAL_BOOL);;
	case Object:	return (getO().at().objName() == "EVAL");
	default: break;
    }
    return true;
}

char TVariant::getB( ) const
{
    switch(type()) {
	case Null:	return false;
	case String:	{ string tvl = getS(); return (tvl==EVAL_STR) ? EVAL_BOOL : (bool)s2i(tvl); }
	case Integer:	{ int64_t tvl = getI(); return (tvl==EVAL_INT) ? EVAL_BOOL : (bool)tvl; }
	case Real:	{ double tvl = getR(); return (tvl==EVAL_REAL) ? EVAL_BOOL : (bool)tvl; }
	case Object:	{ AutoHD<TVarObj> tvl = getO(); return (tvl.at().objName() == "EVAL") ? EVAL_BOOL : true; }
	case Boolean:	return val.b;
	default: break;
    }
    return EVAL_BOOL;
}

int64_t TVariant::getI( ) const
{
    switch(type()) {
	case String:	{ string tvl = getS(); return (tvl==EVAL_STR) ? EVAL_INT : s2ll(tvl); }
	case Real:	{ double tvl = getR(); return (tvl==EVAL_REAL) ? EVAL_INT : (int64_t)tvl; }
	case Boolean:	{ char tvl = getB();   return (tvl==EVAL_BOOL) ? EVAL_INT : tvl; }
	case Object:	{ AutoHD<TVarObj> tvl = getO(); return (tvl.at().objName() == "EVAL") ? EVAL_INT : 1; }
	case Integer:	return val.i;
	default: break;
    }
    return EVAL_INT;
}

double TVariant::getR( ) const
{
    switch(type()) {
	case String:	{ string tvl = getS(); return (tvl==EVAL_STR) ? EVAL_REAL : s2r(tvl); }
	case Integer:	{ int64_t tvl = getI();return (tvl==EVAL_INT) ? EVAL_REAL : tvl; }
	case Boolean:	{ char tvl = getB();   return (tvl==EVAL_BOOL) ? EVAL_REAL : tvl; }
	case Object:	{ AutoHD<TVarObj> tvl = getO(); return (tvl.at().objName() == "EVAL") ? EVAL_REAL : 1; }
	case Real:	return val.r;
	default: break;
    }
    return EVAL_REAL;
}

string TVariant::getS( ) const
{
    switch(type()) {
	case Integer:	{ int64_t tvl = getI();return (tvl==EVAL_INT) ? EVAL_STR : ll2s(tvl); }
	case Real:	{ double tvl = getR(); return (tvl==EVAL_REAL) ? EVAL_STR : r2s(tvl); }
	case Boolean:	{ char tvl = getB();   return (tvl==EVAL_BOOL) ? EVAL_STR : i2s(tvl); }
	case Object:	{ AutoHD<TVarObj> tvl = getO(); return (tvl.at().objName() == "EVAL") ? EVAL_STR : tvl.at().getStrXML(); }
	case String:
	    if(mStdString)	return *val.s;
	    if(mSize < sizeof(val.sMini)) return string(val.sMini, mSize);
	    return string(val.sPtr, mSize);
	default: break;
    }
    return EVAL_STR;
}

AutoHD<TVarObj>	TVariant::getO( ) const
{
    if(type() != Object)	return new TEValObj();
    if(val.o->freeStat())	*val.o = AutoHD<TVarObj>(new TEValObj());
    return *val.o;
}

void TVariant::setB( char ivl )
{
    if(type() != Boolean && !mFixedTp) setType(Boolean);
    switch(type()) {
	case String:	setS((ivl==EVAL_BOOL) ? EVAL_STR : i2s(ivl));	break;
	case Integer:	setI((ivl==EVAL_BOOL) ? EVAL_INT : ivl);	break;
	case Real:	setR((ivl==EVAL_BOOL) ? EVAL_REAL : ivl);	break;
	case Boolean:	val.b = ivl;					break;
	default: break;
    }
}

void TVariant::setI( int64_t ivl )
{
    if(type() != Integer && !mFixedTp) setType(Integer);
    switch(type()) {
	case String:	setS((ivl==EVAL_INT) ? EVAL_STR : ll2s(ivl));	break;
	case Integer:	val.i = ivl;					break;
	case Real:	setR((ivl==EVAL_INT) ? EVAL_REAL : ivl);	break;
	case Boolean:	setB((ivl==EVAL_INT) ? EVAL_BOOL : (bool)ivl);	break;
	default: break;
    }
}

void TVariant::setR( double ivl )
{
    if(type() != Real && !mFixedTp) setType(Real);
    switch(type()) {
	case String:	setS((ivl==EVAL_REAL) ? EVAL_STR : r2s(ivl));		break;
	case Integer:	setI((ivl==EVAL_REAL) ? EVAL_INT : (int64_t)ivl);	break;
	case Real:	val.r = ivl;						break;
	case Boolean:	setB((ivl==EVAL_REAL) ? EVAL_BOOL : (bool)ivl);		break;
	default: break;
    }
}

void TVariant::setS( const string &ivl )
{
    if(type() != String && !mFixedTp) setType(String);
    switch(type()) {
	case String:
	    if(ivl.size() < sizeof(val.sMini)) {	//Minimum fixed area
		if(mStdString) { delete val.s; mStdString = false; }
		else if(mSize >= sizeof(val.sMini)) free(val.sPtr);
		memcpy(val.sMini, ivl.data(), ivl.size());
		val.sMini[ivl.size()] = 0;
		mSize = ivl.size();
	    }
	    //!!!! Direct memory allocation mostly used now for constant strings like to key TCfg for prevent its "const char *" pointer changing.
	    //     Maybe further there has a sense to use STL string also but check it to equal for the assign and then the pointer changing omit.
	    else if(mStdStringOmit) {		//For middle blocks up to STR_BUF_LEN
		if(ivl.size() > 30000000)	throw TError("TVariant", _("Too big string forced to the not STL string (> 30 MB)!"));
		if(mStdString) { delete val.s; mStdString = false; }
		if(mSize < sizeof(val.sMini))	val.sPtr = (char*)malloc(ivl.size()+1);
		else if(ivl.size() != mSize)	{
		    char *sPtrTmp = val.sPtr;
		    val.sPtr = (char*)realloc(val.sPtr, ivl.size()+1);
		    if(sPtrTmp && !val.sPtr) free(sPtrTmp);
		}
		if(!val.sPtr) {
		    mSize = 0;
		    val.sMini[mSize] = 0;
		    throw TError("TVariant", _("Memory alloc for big string length (%d) error!"), ivl.size());
		}
		memcpy(val.sPtr, ivl.data(), ivl.size());
		val.sPtr[ivl.size()] = 0;
		mSize = ivl.size();
	    }
	    else {					//Standard string for too big strings
		if(!mStdString) {
		    if(mSize >= sizeof(val.sMini)) free(val.sPtr);
		    val.s = new string;
		    mSize = 0; mStdString = true;
		}
		*val.s = ivl;
	    }
	    break;
	case Integer:	setI((ivl==EVAL_STR) ? EVAL_INT : s2ll(ivl));	break;
	case Real:	setR((ivl==EVAL_STR) ? EVAL_REAL : s2r(ivl));	break;
	case Boolean:	setB((ivl==EVAL_STR) ? EVAL_BOOL : (bool)s2i(ivl));	break;
	case Object:	setO((ivl==EVAL_STR) ? AutoHD<TVarObj>(new TEValObj()) : TVarObj::parseStrXML(ivl,NULL,getO())); break;
	default: break;
    }
}

void TVariant::setO( AutoHD<TVarObj> ivl )
{
    if(type() != Object && !mFixedTp) setType(Object);
    *val.o = ivl;
}

void TVariant::setO( TVarObj *ival )
{
    setO(AutoHD<TVarObj>(ival));
}

//***********************************************************
//* TVarObj                                                 *
//*   Variable object, by default included properties only  *
//***********************************************************
TVarObj::TVarObj( ) : mUseCnt(0), dataM(true)
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TVarObj::~TVarObj( )
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

void TVarObj::AHDConnect( )
{
    dataM.lock();
    ++mUseCnt;
    dataM.unlock();
}

bool TVarObj::AHDDisConnect( )
{
    dataM.lock();
    bool dblDscNct = (mUseCnt == 0);
    if(mUseCnt) mUseCnt--;
    bool toFree = (mUseCnt == 0 && !dblDscNct);
    dataM.unlock();

    if(dblDscNct) mess_err("TVarObj", _("Double disconnection try!"));

    return toFree;
}

void TVarObj::propList( vector<string> &ls )
{
    ls.clear();
    dataM.lock();
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	ls.push_back(ip->first);
    dataM.unlock();
}

TVariant TVarObj::propGet( const string &id )
{
    dataM.lock();
    TVariant rez;
    map<string,TVariant>::iterator vit = mProps.find(id);
    if(vit != mProps.end()) rez = vit->second;
    dataM.unlock();

    return rez;
}

TVariant TVarObj::propGet( const string &ids, char sep )
{
    TVariant obj = this;
    string tid;
    if(sep) for(int off = 0; (tid=TSYS::strSepParse(ids,0,sep,&off)).size() && obj.type() == TVariant::Object; )
	obj = obj.getO().at().propGet(tid);
    else for(int off = 0; (tid=TSYS::pathLev(ids,0,true,&off)).size() && obj.type() == TVariant::Object; )
	obj = obj.getO().at().propGet(tid);

    return tid.size() ? TVariant((char)EVAL_BOOL) : obj;
}

void TVarObj::propSet( const string &id, TVariant val )
{
    dataM.lock();
    mProps[id] = val;
    dataM.unlock();
}

void TVarObj::propSet( const string &ids, char sep, TVariant val )
{
    TVariant obj = this;
    string tid;
    int off = 0;
    if(sep) while(obj.type() == TVariant::Object && (tid=TSYS::strSepParse(ids,0,sep,&off)).size() && off < (int)ids.size())
	obj = obj.getO().at().propGet(tid);
    else while(obj.type() == TVariant::Object && (tid=TSYS::pathLev(ids,0,true,&off)).size() && off < (int)ids.size())
	obj = obj.getO().at().propGet(tid);
    if(tid.size() && off >= (int)ids.size())	obj.getO().at().propSet(tid, val);
}

void TVarObj::propClear( const string &ids )
{
    dataM.lock();
    if(ids.size()) mProps.erase(ids);
    else mProps.clear();
    dataM.unlock();
}

string TVarObj::getStrXML( const string &oid )
{
    string nd("<TVarObj");
    if(!oid.empty()) nd += " p='" + oid + "'";
    nd += ">\n";

    dataM.lock();
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	switch(ip->second.type()) {
	    case TVariant::String: nd += "<str p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</str>\n";	break;
	    case TVariant::Integer:nd += "<int p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</int>\n";	break;
	    case TVariant::Real:   nd += "<real p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</real>\n";	break;
	    case TVariant::Boolean:nd += "<bool p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</bool>\n";	break;
	    case TVariant::Object: nd += ip->second.getO().at().getStrXML(ip->first); break;
	    default: break;
	}
    dataM.unlock();
    nd += "</TVarObj>\n";

    return nd;
}

AutoHD<TVarObj> TVarObj::parseStrXML( const string &str, XMLNode *nd, AutoHD<TVarObj> prev )
{
    XMLNode oTree;
    if(!nd) {
	try { oTree.load(str, 0, Mess->charset()); }
	catch(TError &err) { return prev; }
	nd = &oTree;
    }

    //Different objects process
    if(nd->name() == "TVarObj") {
	TVarObj *rez = new TVarObj;
	for(unsigned iCh = 0; iCh < nd->childSize(); iCh++) {
	    XMLNode *cNd = nd->childGet(iCh);
	    if(cNd->name() == "str")		rez->mProps[cNd->attr("p")] = cNd->text();
	    else if(cNd->name() == "int")	rez->mProps[cNd->attr("p")] = (int64_t)s2ll(cNd->text());
	    else if(cNd->name() == "real")	rez->mProps[cNd->attr("p")] = s2r(cNd->text());
	    else if(cNd->name() == "bool")	rez->mProps[cNd->attr("p")] = (char)s2i(cNd->text());
	    else if(cNd->name() == "TVarObj")	rez->mProps[cNd->attr("p")] = TVarObj::parseStrXML("", cNd);
	    else if(cNd->name() == "TEValObj")	rez->mProps[cNd->attr("p")] = TEValObj::parseStrXML(cNd);
	    else if(cNd->name() == "TArrayObj")	rez->mProps[cNd->attr("p")] = TArrayObj::parseStrXML(cNd);
	    else if(TSYS::strParse(cNd->name(),0,":") == "XMLNodeObj")
		rez->mProps[cNd->attr("p")] = XMLNodeObj::parseStrXML(cNd);
	}
	return rez;
    }
    else if(nd->name() == "TEValObj")	return TEValObj::parseStrXML(nd);
    else if(nd->name() == "TArrayObj")	return TArrayObj::parseStrXML(nd);
    else if(TSYS::strParse(nd->name(),0,":") == "XMLNodeObj")	return XMLNodeObj::parseStrXML(nd);

    return prev;
}

TVariant TVarObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // bool isEVal() - return "false" for EVAL detect
    if(id == "isEVal")	return false;

    throw TError(TSYS::strMess("Obj%s",objName().c_str()).c_str(), _("Function '%s' error or not enough parameters."), id.c_str());
}

//*****************************************************************
//* TEValObj                                                      *
//*   Special EVal object — Scalar bool, int, real, string analog *
//*****************************************************************
TEValObj::TEValObj( )
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), 1);
}

TEValObj::~TEValObj( )
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), -1);
}

TVariant TEValObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // bool isEVal() - return "true" for EVAL detect
    if(id == "isEVal")	return true;

    return TVarObj::funcCall(id, prms);
}

string TEValObj::getStrXML( const string &oid )
{
    string nd("<TEValObj");
    if(!oid.empty()) nd += " p='" + oid + "'";
    nd += " />\n";
    return nd;
}

AutoHD<TVarObj> TEValObj::parseStrXML( XMLNode *nd )	{ return new TEValObj; }

//***********************************************************
//* TArrayObj                                                *
//*   Array object included indexed properties               *
//***********************************************************
TArrayObj::TArrayObj( )
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), 1);
}

TArrayObj::~TArrayObj( )
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), -1);
}

TVariant TArrayObj::propGet( const string &id )
{
    if(id == "length")			return (int)mEls.size();
    if(id.size() && isdigit(id[0]))	return arGet(s2i(id));
    return TVarObj::propGet(id);
}

void TArrayObj::propSet( const string &id, TVariant val )
{
    if(id.size() && isdigit(id[0]))	arSet(s2i(id), val);
    else TVarObj::propSet(id,val);
}

string TArrayObj::getStrXML( const string &oid )
{
    string nd("<TArrayObj");
    if(!oid.empty()) nd = nd + " p='" + oid + "'";
    nd = nd + ">\n";

    dataM.lock();
    //Array items process
    for(unsigned ip = 0; ip < mEls.size(); ip++)
	switch(mEls[ip].type()) {
	    case TVariant::String: nd += "<str>"+TSYS::strEncode(mEls[ip].getS(),TSYS::Html)+"</str>\n";	break;
	    case TVariant::Integer:nd += "<int>"+TSYS::strEncode(mEls[ip].getS(),TSYS::Html)+"</int>\n";	break;
	    case TVariant::Real:   nd += "<real>"+TSYS::strEncode(mEls[ip].getS(),TSYS::Html)+"</real>\n";	break;
	    case TVariant::Boolean:nd += "<bool>"+TSYS::strEncode(mEls[ip].getS(),TSYS::Html)+"</bool>\n";	break;
	    case TVariant::Object: nd += mEls[ip].getO().at().getStrXML(); break;
	    default: break;
	}
    //Object's properties process
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	switch(ip->second.type()) {
	    case TVariant::String: nd += "<str p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</str>\n"; break;
	    case TVariant::Integer:nd += "<int p='"+ip->first+"'>"+ip->second.getS()+"</int>\n";	break;
	    case TVariant::Real:   nd += "<real p='"+ip->first+"'>"+ip->second.getS()+"</real>\n";	break;
	    case TVariant::Boolean:nd += "<bool p='"+ip->first+"'>"+ip->second.getS()+"</bool>\n";	break;
	    case TVariant::Object: nd += ip->second.getO().at().getStrXML(ip->first);			break;
	    default: break;
	}
    dataM.unlock();

    nd += "</TArrayObj>\n";

    return nd;
}

AutoHD<TVarObj> TArrayObj::parseStrXML( XMLNode *nd )
{
    TArrayObj *rez = new TArrayObj;
    for(unsigned iCh = 0; iCh < nd->childSize(); iCh++) {
	XMLNode *cNd = nd->childGet(iCh);
	string p = cNd->attr("p");
	TVariant vl;
	if(cNd->name() == "str")		vl = cNd->text();
	else if(cNd->name() == "int")		vl = (int64_t)s2ll(cNd->text());
	else if(cNd->name() == "real")		vl = s2r(cNd->text());
	else if(cNd->name() == "bool")		vl = (char)s2i(cNd->text());
	else vl = TVarObj::parseStrXML("", cNd);
	if(!vl.isNull()) {
	    if(p.size()) rez->mProps[p] = vl;
	    else rez->mEls.push_back(vl);
	}
    }
    return rez;
}

TVariant TArrayObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // string join(string sep = ",") - join items to string
    //  sep - items separator
    if(id == "join" || id == "toString" || id == "valueOf") {
	string rez, sep = prms.size() ? prms[0].getS() : ",";
	dataM.lock();
	for(unsigned i_e = 0; i_e < mEls.size(); i_e++)
	    rez += (i_e?sep:"")+mEls[i_e].getS();
	dataM.unlock();
	return rez;
    }
    // TArrayObj concat(TArrayObj arr) - concatenate array
    //  arr - source array
    if(id == "concat" && prms.size() && prms[0].type() == TVariant::Object && !AutoHD<TArrayObj>(prms[0].getO()).freeStat()) {
	dataM.lock();
	TArrayObj *sArr = (TArrayObj*)&prms[0].getO().at();
	for(unsigned iP = 0; iP < sArr->mEls.size(); iP++) mEls.push_back(sArr->mEls[iP]);
	dataM.unlock();
	return this;
    }
    // int push(ElTp var, ...) - push variables to array
    //  var - variable
    if(id == "push" && prms.size()) {
	dataM.lock();
	for(unsigned i_p = 0; i_p < prms.size(); i_p++) mEls.push_back(prms[i_p]);
	dataM.unlock();
	return (int)mEls.size();
    }
    // ElTp pop( ) - pop variable from array
    if(id == "pop") {
	if(mEls.empty()) return TVariant(); //throw TError("ArrayObj", _("Array is empty."));
	dataM.lock();
	TVariant val = mEls.back();
	mEls.pop_back();
	dataM.unlock();
	return val;
    }
    // Array reverse( ) - reverse array's items order
    if(id == "reverse") {
	dataM.lock();
	reverse(mEls.begin(),mEls.end());
	dataM.unlock();
	return this;
    }
    // ElTp shift( ) - shift array's items upward
    if(id == "shift") {
	if(mEls.empty()) return TVariant(); //throw TError("ArrayObj", _("Array is empty."));
	dataM.lock();
	TVariant val = mEls.front();
	mEls.erase(mEls.begin());
	dataM.unlock();
	return val;
    }
    // int unshift(ElTp var, ...) - shift items to array upward
    //  var - variable
    if(id == "unshift" && prms.size()) {
	dataM.lock();
	for(unsigned i_p = 0; i_p < prms.size(); i_p++) mEls.insert(mEls.begin()+i_p, prms[i_p]);
	dataM.unlock();
	return (int)mEls.size();
    }
    // Array slice(int beg, int end) - get array part from positon <beg> to <end> (exclude)
    //  beg - begin position
    //  end - end position
    if(id == "slice" && prms.size()) {
	dataM.lock();
	int beg = prms[0].getI();
	if(beg < 0) beg = mEls.size()+beg;
	beg = vmax(beg,0);
	int end = mEls.size();
	if(prms.size() >= 2) end = prms[1].getI();
	if(end < 0) end = mEls.size()+end;
	end = vmin(end, (int)mEls.size());
	TArrayObj *rez = new TArrayObj();
	for(int i_p = beg; i_p < end; i_p++)
	    rez->arSet(i_p-beg, mEls[i_p]);
	dataM.unlock();
	return rez;
    }
    // Array splice(int beg, int remN, ElTp val1, ElTp val2, ...) - insert, remove or replace array's items
    //  beg - start position
    //  remN - removed items number
    //  val1, val2, ... - values for insert
    if(id == "splice" && prms.size() >= 1) {
	dataM.lock();
	int beg = vmax(0, prms[0].getI());
	int cnt = (prms.size()>1) ? prms[1].getI() : mEls.size();
	//  Delete elements
	TArrayObj *rez = new TArrayObj();
	for(int i_c = 0; i_c < cnt && beg < (int)mEls.size(); i_c++) {
	    rez->arSet(i_c, mEls[beg]);
	    mEls.erase(mEls.begin()+beg);
	}
	//  Insert elements
	for(unsigned i_c = 2; i_c < prms.size() && beg <= (int)mEls.size(); i_c++)
	    mEls.insert(mEls.begin()+beg+i_c-2,prms[i_c]);
	dataM.unlock();
	return rez;
    }
    // double sum(int beg, int end) - sum of the array part from positon <beg> to <end> (exclude)
    //  beg - begin position
    //  end - end position
    if(id == "sum" && prms.size()) {
	dataM.lock();
	int beg = prms[0].getI();
	if(beg < 0) beg = mEls.size()+beg;
	beg = vmax(beg, 0);
	int end = mEls.size();
	if(prms.size() >= 2) end = prms[1].getI();
	if(end < 0) end = mEls.size()+end;
	end = vmin(end, (int)mEls.size());
	double rez = 0;
	for(int iP = beg; iP < end; iP++) rez += mEls[iP].getR();
	dataM.unlock();
	return rez;
    }
    // Array sort( ) - lexicographic items sorting
    if(id == "sort") {
	dataM.lock();
	sort(mEls.begin(),mEls.end(),compareLess);
	dataM.unlock();
	return this;
    }

    return TVarObj::funcCall(id, prms);
}

TVariant TArrayObj::arGet( int vid )
{
    TVariant rez;
    dataM.lock();
    if(vid >= 0 && vid < (int)mEls.size()) rez = mEls[vid];
    dataM.unlock();
    return rez;
}

void TArrayObj::arSet( int vid, TVariant val )
{
    //if(vid < 0) return;//throw TError("ArrayObj", _("Negative id is not allow for array."));
    dataM.lock();
    if(vid < 0) vid = mEls.size();
    while(vid >= (int)mEls.size()) mEls.push_back(TVariant());
    mEls[vid] = val;
    dataM.unlock();
}

bool TArrayObj::compareLess( const TVariant &v1, const TVariant &v2 )	{ return v1.getS() < v2.getS(); }

//***********************************************************
//* TRegExp                                                 *
//*   Regular expression object                             *
//***********************************************************
TRegExp::TRegExp( const string &rule, const string &flg ) :
    lastIndex(0), pattern(rule), global(false), ignoreCase(false), multiline(false), isSimplePat(false), UTF8(false),
    regex(NULL), vSz(90), capv(NULL)
{
    setPattern(rule, flg);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), 1);
}

TRegExp::~TRegExp( )
{
    if(capv)	delete [] capv;
    if(regex)	pcre_free((pcre*)regex);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), -1);
}

void TRegExp::setPattern( const string &rule, const string &flg )
{
    //Global properties init
    global = (flg.find('g')!=string::npos);
    ignoreCase = (flg.find('i')!=string::npos);
    multiline = (flg.find('m')!=string::npos);
    UTF8 = Mess->isUTF8() || (flg.find('u')!=string::npos);
    isSimplePat = false;
    pattern = rule;
    if(flg.find('p') != string::npos) {
	isSimplePat = !(rule.size() > 2 && rule[0] == '/' && rule[rule.size()-1] == '/');
	if(!isSimplePat) pattern = rule.substr(1,rule.size()-2);
    }

    //Check for free
    if(isSimplePat && capv)	{ delete [] capv; capv = NULL; }
    if(regex)			{ pcre_free((pcre*)regex); regex = NULL; }

    //Alloc for regexp
    if(!isSimplePat && pattern.size()) {
	const char *terr;
	int erroff;
	regex = pcre_compile(pattern.c_str(), PCRE_DOTALL|(UTF8?PCRE_UTF8:0)|(ignoreCase?PCRE_CASELESS:0)|(multiline?PCRE_MULTILINE:0),
	    &terr, &erroff, NULL);
	if(!regex) err = terr;
	else if(!capv) capv = new int[90];
    }
}

TArrayObj *TRegExp::match( const string &vl, bool all )
{
    TArrayObj *rez = new TArrayObj();
    if(!regex) return rez;

    if(all && global)
	for(int curPos = 0, iN = 0; pcre_exec((pcre*)regex,NULL,vl.data(),vl.size(),curPos,0,capv,vSz) > 0 && capv[1] > capv[0]; curPos = capv[1], iN++)
	    rez->arSet(iN, string(vl.data()+capv[0],capv[1]-capv[0]));
    else {
	int n = pcre_exec((pcre*)regex, NULL, vl.data(), vl.size(), (global?lastIndex:0), 0, capv, vSz);
	for(int iN = 0; iN < n; iN++)
	    rez->arSet(iN, string(vl.data()+capv[iN*2],capv[iN*2+1]-capv[iN*2]));
	if(global) lastIndex = (n>0) ? capv[1] : 0;
	if(n > 0) { rez->propSet("index", capv[0]); rez->propSet("input", vl); }
    }

    return rez;
}

string TRegExp::replace( const string &vl, const string &str )
{
    string rez = vl, repl;
    if(!regex) return rez;
    for(int curPos = 0, n; (!curPos || global) && (n=pcre_exec((pcre*)regex,NULL,rez.data(),rez.size(),curPos,0,capv,vSz)) > 0 && capv[1] > capv[0];
	curPos = capv[0]+repl.size())
    {
	repl = substExprRepl(str, rez, capv, n);
	rez.replace(capv[0], capv[1]-capv[0], repl);
    }
    return rez;
}

TArrayObj *TRegExp::split( const string &vl, int limit )
{
    TArrayObj *rez = new TArrayObj();
    if(!regex) return rez;
    int curPos = 0, iN = 0;
    for(int se = 0; (se=pcre_exec((pcre*)regex,NULL,vl.data(),vl.size(),curPos,0,capv,vSz)) > 0 && capv[1] > capv[0] && (!limit || iN < limit);
		curPos = capv[1])
    {
	rez->arSet(iN++, string(vl.data()+curPos,capv[0]-curPos));
	for(int iSe = 1; iSe < se && (!limit || iN < limit); iSe++)
	    rez->arSet(iN++, string(vl.data()+capv[iSe*2],capv[iSe*2+1]-capv[iSe*2]));
    }
    if(curPos <= (int)vl.size() && (!limit || iN < limit)) rez->arSet(iN++, string(vl.data()+curPos,vl.size()-curPos));
    return rez;
}

bool TRegExp::test( const string &vl )
{
    //Check by simple pattern
    if(isSimplePat) {
	bool mult_s = false;
	int v_cnt = 0, p_cnt = 0;
	int v_bck = -1, p_bck = -1;

	while(true) {
	    if(p_cnt >= (int)pattern.size() ) return true;
	    if(pattern[p_cnt] == '?')	{ v_cnt++; p_cnt++; mult_s = false; continue; }
	    if(pattern[p_cnt] == '*')	{ p_cnt++; mult_s = true; v_bck = -1; continue; }
	    if(pattern[p_cnt] == '\\')	p_cnt++;
	    if(v_cnt >= (int)vl.size())  break;
	    if(pattern[p_cnt] == vl[v_cnt]) {
		if(mult_s && v_bck < 0 )	{ v_bck = v_cnt+1; p_bck = p_cnt; }
		v_cnt++; p_cnt++;
	    }
	    else {
		if(mult_s) {
		    if(v_bck >= 0) { v_cnt = v_bck; p_cnt = p_bck; v_bck = -1; }
		    else v_cnt++;
		}
		else break;
	    }
	}
	return false;
    }
    //Check by regular expression
    if(!regex) return false;
    int n = pcre_exec((pcre*)regex, NULL, vl.data(), vl.size(), (global?lastIndex:0), 0, capv, vSz);
    if(global) lastIndex = (n>0) ? capv[1] : 0;
    return (n>0);
}

int TRegExp::search( const string &vl, int off )
{
    if(!regex) return -1;
    int n = pcre_exec((pcre*)regex, NULL, vl.data(), vl.size(), off, 0, capv, vSz);
    return (n>0) ? capv[0] : -1;
}

string TRegExp::substExprRepl( const string &str, const string &val, int *icapv, int n )
{
    string rez = str;
    for(size_t cpos = 0; n > 0 && (cpos=rez.find("$",cpos)) != string::npos && cpos < (rez.size()-1); )
	switch(rez[cpos+1]) {
	    case '$':	rez.replace(cpos,2,"$"); cpos++; break;
	    case '`':	rez.replace(cpos,2,val,0,icapv[0]); cpos += icapv[0]; break;
	    case '\'':	rez.replace(cpos,2,val,icapv[1],val.size()-icapv[1]);cpos += (val.size()-icapv[1]); break;
	    case '&':	rez.replace(cpos,2,val,icapv[0],(icapv[1]-icapv[0])); cpos += icapv[1]; break;
	    default: {
		int nd = isdigit(rez[cpos+1]) ? 1 : 0;
		if(nd && cpos < (rez.size()-2) && isdigit(rez[cpos+2])) nd++;
		int subexp = s2i(string(rez.data()+cpos+1,nd));
		string replVl;
		if(subexp > 0 && subexp < n) replVl.assign(val,icapv[subexp*2],icapv[subexp*2+1]-icapv[subexp*2]);
		rez.replace(cpos,nd+1,replVl);
		cpos += replVl.size();
	    }
	}
    return rez;
}

TVariant TRegExp::propGet( const string &id )
{
    if(id == "source")		return pattern;
    if(id == "global")		return (bool)global;
    if(id == "ignoreCase")	return (bool)ignoreCase;
    if(id == "multiline")	return (bool)multiline;
    if(id == "UTF8")		return (bool)UTF8;
    if(id == "lastIndex")	return lastIndex;
    return TVariant();
}

void TRegExp::propSet( const string &id, TVariant val )
{
    if(id == "lastIndex")	lastIndex = val.getI();
}

TVariant TRegExp::funcCall( const string &id, vector<TVariant> &prms )
{
    // Array exec(string val) - call match for string 'val'. Return matched substring (0) and subexpressions (>0) array.
    //    Set array property "index" to matched substring position.
    //    Set array property "input" to source match string.
    //  val - matched string
    if(id == "exec" && prms.size() && prms[0].type() == TVariant::String) return match(prms[0].getS());
    // bool test(string val) - call match for string 'val'. Return "true" for match OK.
    //  val - matched string
    if(id == "test" && prms.size() && prms[0].type() == TVariant::String) return test(prms[0].getS());

    return TVarObj::funcCall(id, prms);
}

string TRegExp::getStrXML( const string &oid )
{
    string nd("<TRegExp");
    if(!oid.empty()) nd += " p='"+oid+"'";
    nd += ">\n";
    nd += "<rule>"+TSYS::strEncode(pattern,TSYS::Html)+"</rule>\n";
    nd = nd+"<flg>"+(global?"g":"")+(ignoreCase?"i":"")+(multiline?"m":"")+"</flg>\n";
    nd += "</TRegExp>\n";

    return nd;
}

//*************************************************
//* XMLNodeObj - XML node object                  *
//*************************************************
XMLNodeObj::XMLNodeObj( const string &name ) : mName(name), parent(NULL)
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), 1);
}

XMLNodeObj::~XMLNodeObj( )
{
    while(childSize()) childDel(0);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), -1);
}

string XMLNodeObj::name( )
{
    dataM.lock();
    string rez = mName;
    dataM.unlock();
    return rez;
}

string XMLNodeObj::text( bool full )
{
    dataM.lock();
    string rez = mText;
    for(unsigned iCh = 0; full && iCh < mChilds.size(); iCh++)
	rez += mChilds[iCh].at().text(full);
    dataM.unlock();
    return rez;
}

void XMLNodeObj::setName( const string &vl )
{
    dataM.lock();
    mName = vl;
    dataM.unlock();
}

void XMLNodeObj::setText( const string &vl )
{
    dataM.lock();
    mText = vl;
    dataM.unlock();
}

void XMLNodeObj::childAdd( AutoHD<XMLNodeObj> nd )
{
    if(&nd.at() == this) return;
    dataM.lock();
    mChilds.push_back(nd);
    nd.at().parent = this;
    dataM.unlock();
}

void XMLNodeObj::childIns( unsigned id, AutoHD<XMLNodeObj> nd )
{
    if(&nd.at() == this) return;
    dataM.lock();
    //if(id < 0) id = mChilds.size();
    id = vmin(id, mChilds.size());
    mChilds.insert(mChilds.begin()+id,nd);
    nd.at().parent = this;
    dataM.unlock();
}

void XMLNodeObj::childDel( unsigned id )
{
    if(/*id < 0 || */id >= mChilds.size()) throw TError("XMLNodeObj", _("Deletion child '%d' error."), id);
    dataM.lock();
    if(mChilds[id].at().parent == this) mChilds[id].at().parent = NULL;
    mChilds.erase(mChilds.begin()+id);
    dataM.unlock();
}

AutoHD<XMLNodeObj> XMLNodeObj::childGet( unsigned id )
{
    if(/*id < 0 || */id >= mChilds.size()) throw TError("XMLNodeObj", _("Child '%d' is not allow."), id);
    dataM.lock();
    AutoHD<XMLNodeObj> rez = mChilds[id];
    dataM.unlock();
    return rez;
}

AutoHD<XMLNodeObj> XMLNodeObj::childGet( const string &name, unsigned num )
{
    dataM.lock();
    AutoHD<XMLNodeObj> rez;
    for(int iCh = 0, iN = 0; iCh < (int)mChilds.size(); iCh++)
	if(strcasecmp(mChilds[iCh].at().name().c_str(),name.c_str()) == 0 && (iN++) == (int)num)
	    rez = mChilds[iCh];
    dataM.unlock();
    if(rez.freeStat()) throw TError("XMLNodeObj", _("Child %s:%d is not found!"), name.c_str(), num);
    return rez;
}

string XMLNodeObj::getStrXML( const string &oid )
{
    string nd("<XMLNodeObj:"+name());
    if(!oid.empty()) nd += " p='"+oid+"'";
    dataM.lock();
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	nd += " "+ip->first+"=\""+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"\"";
    nd += ">"+TSYS::strEncode(text(),TSYS::Html)+"\n";
    for(unsigned iCh = 0; iCh < mChilds.size(); iCh++)
	nd += mChilds[iCh].at().getStrXML();
    dataM.unlock();
    nd += "</XMLNodeObj:"+name()+">\n";

    return nd;
}

AutoHD<TVarObj> XMLNodeObj::parseStrXML( XMLNode *nd )
{
    XMLNodeObj *rez = new XMLNodeObj(TSYS::strParse(nd->name(),1,":"));
    rez->mText = nd->text();

    //Attributes process
    vector<string> lst;
    nd->attrList(lst);
    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
	rez->mProps[lst[i_l]] = nd->attr(lst[i_l]);

    //Child nodes process
    for(unsigned iCh = 0; iCh < nd->childSize(); iCh++)
	rez->mChilds.push_back(XMLNodeObj::parseStrXML(nd->childGet(iCh)));

    return rez;
}

TVariant XMLNodeObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // string name( ) - node name
    if(id == "name")	return name();
    // string text( bool full = false ) - node text
    if(id == "text")	return text(prms.size()&&prms[0].getB());
    // string attr(string id) - get node attribute
    //  id - attribute identifier
    if(id == "attr" && prms.size()) {
	TVariant avl = propGet(prms[0].getS());
	if(avl.isEVal() || avl.isNull()) return "";
	return avl;
    }
    // XMLNodeObj setName(string vl) - set node name
    //  vl - value for node name
    if(id == "setName" && prms.size())	{ setName(prms[0].getS()); return this; }
    // XMLNodeObj setText(string vl) - set node text
    //  vl - value for node text
    if(id == "setText" && prms.size())	{ setText(prms[0].getS()); return this; }
    // XMLNodeObj setAttr(string id, string vl) - set attribute to value
    //  id - attribute identifier
    //  vl - value for attribute
    if(id == "setAttr" && prms.size() >= 2)	{ propSet(prms[0].getS(),prms[1].getS()); return this; }
    // XMLNodeObj clear( bool full = false ) - clear the node for the childs remove, text and attributes for <full>.
    //  full - full clear include attributes.
    if(id == "clear") {
	while(childSize()) childDel(0);
	setText("");
	if(prms.size() && prms[0].getB()) propClear();
	return this;
    }
    // int childSize( ) - return childs counter for node
    if(id == "childSize")	return (int)childSize();
    // XMLNodeObj childAdd(ElTp no = XMLNodeObj) - add node <no> as child to the node
    // XMLNodeObj childAdd(string nd) - add node with name <nd>
    //  no - node object or name for new node
    if(id == "childAdd") {
	AutoHD<XMLNodeObj> no;
	if(prms.size() && prms[0].type() == TVariant::Object && !(no=prms[0].getO()).freeStat()) ;
	else if(prms.size()) no = new XMLNodeObj(prms[0].getS());
	else no = new XMLNodeObj();
	childAdd(no);
	return AutoHD<TVarObj>(no);
    }
    // XMLNodeObj childIns(int id, ElTp no = XMLNodeObj) - insert node <no> as child to the node
    // XMLNodeObj childIns(int id, string nd) - insert node with name <nd>
    //  id - insert position
    //  no - node object or name for new node
    if(id == "childIns" && prms.size()) {
	AutoHD<XMLNodeObj> no;
	if(prms.size() > 1 && prms[1].type() == TVariant::Object && !(no=prms[1].getO()).freeStat()) ;
	else if(prms.size() > 1) no = new XMLNodeObj(prms[1].getS());
	else no = new XMLNodeObj();
	childIns(prms[0].getI(), no);
	return AutoHD<TVarObj>(no);
    }
    // XMLNodeObj childDel(int id) - remove child node from position <id>
    //  id - child node position
    if(id == "childDel" && prms.size())	{ childDel(prms[0].getI()); return this; }
    // XMLNodeObj childGet(int id) - get node from position <id>
    // XMLNodeObj childGet(string name, int num = 0) - get node by name <name> and number <num>
    //  id - child node position;
    //  name - node/tag name;
    //  num - node/tag number <num>.
    if(id == "childGet" && prms.size()) {
	if(prms[0].type() == TVariant::String)
	    return AutoHD<TVarObj>(childGet(prms[0].getS(), (prms.size()>1)?prms[1].getI():0));
	return AutoHD<TVarObj>(childGet(prms[0].getI()));
    }
    // XMLNodeObj parent() - get parent node
    if(id == "parent")	return !parent ? TVariant(false) : TVariant(parent);
    // string load(string str, bool file = false, int flg = 0, string cp = "UTF-8") - load XML tree from XML-stream from string or file
    //  str - source stream string or file name, for <file> = true;
    //  file - load XML-tree from file (true) or stram (false);
    //  flg - node's load flags:
    //    0x01 - text and comments load into separated nodes "<*>" and "<!>";
    //    0x02 - no remove spaces for begin and end tag's text.
    //  cp - source codepage.
    if(id == "load" && prms.size()) {
	XMLNode nd;
	//  Load from file
	if(prms.size() >= 2 && prms[1].getB()) {
	    string s_buf;
	    int hd = open(prms[0].getS().c_str(), O_RDONLY);
	    if(hd < 0) return TSYS::strMess(_("2:Open file '%s' error: %s"), prms[0].getS().c_str(), strerror(errno));
	    bool fOK = true;
	    int cf_sz = lseek(hd, 0, SEEK_END);
	    if(cf_sz > 0) {
		lseek(hd, 0, SEEK_SET);
		char buf[STR_BUF_LEN];
		for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) s_buf.append(buf, len);
		fOK = s_buf.size();
	    }
	    close(hd);
	    if(!fOK) return TSYS::strMess(_("3:Load file '%s' error."), prms[0].getS().c_str());

	    try{ nd.load(s_buf, ((prms.size()>=3)?prms[2].getI():0), ((prms.size()>=4)?prms[3].getS():Mess->charset())); }
	    catch(TError &err) { return "1:"+err.mess; }
	}
	//  Load from string
	else
	    try{ nd.load(prms[0].getS(), ((prms.size()>=3)?prms[2].getI():0), Mess->charset()); }
	    catch(TError &err) { return "1:"+err.mess; }
	fromXMLNode(nd);
	return string("0");
    }
    // string save(int flg = 0, string path = "", string cp = "UTF-8") - save XML-tree to string or file stream
    //  flg - save options:
    //   0x01 - interrupt the string before the opening tag;
    //   0x02 - interrupt the string after the opening tag;
    //   0x04 - interrupt the string after a closing tag;
    //   0x08 - interrupt the string after the text;
    //   0x10 - interrupt the string after the instruction;
    //   0x1E - interrupt the string after all.
    //  path - file path for save to file
    //  cp - target codepage
    if(id == "save") {
	XMLNode nd;
	toXMLNode(nd);
	string s_buf = nd.save(((prms.size()>=1)?prms[0].getI():0), (prms.size()>=3)?prms[2].getS():Mess->charset());
	//  Save to file
	if(prms.size() >= 2) {
	    int hd = open(prms[1].getS().c_str(), O_RDWR|O_CREAT|O_TRUNC, 0664);
	    if(hd < 0)	return "";
	    bool fOK = (write(hd,s_buf.data(),s_buf.size()) == (int)s_buf.size());
	    close(hd);
	    if(!fOK)	return "";
	}
	return s_buf;
    }
    // XMLNodeObj getElementBy( string val, string attr = "id" ) - get element from the tree by attribute <attr> value <val>.
    //  val - attribute value for find;
    //  attr - attribute name for find it value.
    if(id == "getElementBy" && prms.size()) {
	AutoHD<XMLNodeObj> rez = getElementBy(((prms.size() >= 2) ? prms[1].getS() : "id"), prms[0].getS());
	if(rez.freeStat()) return TVariant();
	return AutoHD<TVarObj>(rez);
    }
    // TArrayObj<XMLNodeObj> getElementsBy( string tag, string attrVal = "", string attr = "id" ) -
    //    get elements array from the tree by <tag> and attribute <attr> value <attrVal>.
    //  tag - tag name, empty for all;
    //  attrVal - attribute value to find, empty for pass;
    //  attr - attribute name to find it value.
    if(id == "getElementsBy" && prms.size()) {
	TArrayObj *rez = new TArrayObj();
	getElementsBy(prms[0].getS(), ((prms.size() >= 3) ? prms[2].getS() : "id"), ((prms.size() >= 2)?prms[1].getS():""), rez);
	return rez;
    }

    return TVarObj::funcCall(id, prms);
}

void XMLNodeObj::toXMLNode( XMLNode &nd )
{
    nd.clear();
    nd.setName(name())->setText(text());
    dataM.lock();
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	nd.setAttr(ip->first,ip->second.getS());
    for(unsigned iCh = 0; iCh < mChilds.size(); iCh++)
	mChilds[iCh].at().toXMLNode(*nd.childAdd());
    dataM.unlock();
}

void XMLNodeObj::fromXMLNode( XMLNode &nd )
{
    while(childSize()) childDel(0);

    setName(nd.name());
    setText(nd.text());

    vector<string> alst;
    nd.attrList(alst);
    for(unsigned i_a = 0; i_a < alst.size(); i_a++)
	propSet(alst[i_a], nd.attr(alst[i_a]));

    for(unsigned iCh = 0; iCh < nd.childSize(); iCh++) {
	XMLNodeObj *xn = new XMLNodeObj();
	childAdd(xn);
	xn->fromXMLNode(*nd.childGet(iCh));
    }
}

AutoHD<XMLNodeObj> XMLNodeObj::getElementBy( const string &attr, const string &val )
{
    if(propGet(attr).getS() == val)	return this;

    AutoHD<XMLNodeObj> rez;
    for(unsigned iCh = 0; rez.freeStat() && iCh < childSize(); iCh++)
	rez = childGet(iCh).at().getElementBy(attr, val);

    return rez;
}

void XMLNodeObj::getElementsBy( const string &tag, const string &attr, const string &val, TArrayObj *rez )
{
    if(!tag.size() && (!attr.size() || !val.size()))	return;
    if((!tag.size() || tag == name()) && (!attr.size() || !val.size() || propGet(attr).getS() == val)) rez->arSet(-1, this);

    dataM.lock();
    for(unsigned iCh = 0; iCh < childSize(); iCh++)
	childGet(iCh).at().getElementsBy(tag, attr, val, rez);
    dataM.unlock();
}

//***********************************************************
//* TCntrNodeObj                                            *
//*   TCntrNode object for access to system's objects       *
//***********************************************************
TCntrNodeObj::TCntrNodeObj( AutoHD<TCntrNode> ind, const string &iuser ) : mUser(iuser)
{
    cnd = ind;

    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), 1);
}

TCntrNodeObj::~TCntrNodeObj( )
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(TVarObj::objName()+":"+objName(), -1);
}

string TCntrNodeObj::objName( )
{
    if(cnd.freeStat())	return "TCntrNodeObj:Free";
    return cnd.at().objName();
}

TVariant TCntrNodeObj::propGet( const string &id )
{
    if(cnd.freeStat()) return TVariant();
    try {
	AutoHD<TCntrNode> nnd = cnd.at().nodeAt(id);
	return new TCntrNodeObj(nnd,user());
    } catch(...) { }

    TVariant rez = cnd.at().objPropGet(id);
    if(rez.isNull()) return TVariant();
    return rez;
}

void TCntrNodeObj::propSet( const string &id, TVariant val )
{
    if(cnd.freeStat()) return;
    cnd.at().objPropSet(id,val);
}

string TCntrNodeObj::getStrXML( const string &oid )	{ return "<TCntrNodeObj path=\""+cnd.at().nodePath()+"\"/>"; }

TVariant TCntrNodeObj::funcCall( const string &id, vector<TVariant> &prms )
{
    if(cnd.freeStat()) throw TError("TCntrNodeObj", _("The object don't attached to node of OpenSCADA tree."));
    try{ return cnd.at().objFuncCall(id, prms, user()); } catch(TError&){ }
    return TVarObj::funcCall(id, prms);
}

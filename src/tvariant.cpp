
//OpenSCADA system file: tvariant.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#include <tsys.h>
#include "tvariant.h"

using namespace OSCADA;

//*************************************************
//* TVariant                                      *
//*************************************************
TVariant::TVariant( )				{ vl.assign(1,(char)TVariant::Null); }

TVariant::TVariant( char ivl )			{ vl.assign(1,(char)TVariant::Null); setB(ivl); }

TVariant::TVariant( int ivl )			{ vl.assign(1,(char)TVariant::Null); setI(ivl); }

TVariant::TVariant( double ivl )		{ vl.assign(1,(char)TVariant::Null); setR(ivl); }

TVariant::TVariant( const string &ivl )		{ vl.assign(1,(char)TVariant::Null); setS(ivl); }

TVariant::TVariant( TVarObj *ivl )		{ vl.assign(1,(char)TVariant::Null); setO(ivl); }

TVariant::TVariant( const TVariant &var )	{ operator=(var); }

TVariant::TVariant( const char *var )		{ operator=(string(var)); }

TVariant::~TVariant( )				{ setType(TVariant::Null); }

void TVariant::setType( Type tp )
{
    if( tp == type() )	return;

    if( type() == TVariant::Object && getO() && !getO()->disconnect() ) delete getO( );

    switch(tp)
    {
	case TVariant::Boolean:
	    vl.assign(1,(char)TVariant::Boolean);
	    vl.reserve(1+sizeof(char));
	    break;
	case TVariant::Integer:
	    vl.assign(1,(char)TVariant::Integer);
	    vl.reserve(1+sizeof(int));
	    break;
	case TVariant::Real:
	    vl.assign(1,(char)TVariant::Real);
	    vl.reserve(1+sizeof(double));
	    break;
	case TVariant::String:
	    vl.assign(1,(char)TVariant::String);
	    break;
	case TVariant::Object:
	    vl.assign(1,(char)TVariant::Object);
	    break;
	default: break;
    }
}

void TVariant::setModify( bool iv )
{
    vl[0] = iv ? vl[0]|0x80 : vl[0]&(~0x80);
}

bool TVariant::operator==( TVariant &vr )	{ return vr.vl == vl; }

TVariant &TVariant::operator=( const TVariant &vr )
{
    setType(TVariant::Null);
    vl = vr.vl;
    if( type() == TVariant::Object ) getO()->connect();
    return *this;
}

char TVariant::getB( ) const
{
    switch(type())
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_BOOL : (bool)atoi(getS().c_str());
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_BOOL : (bool)getI();
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_BOOL : (bool)getR();
	case TVariant::Boolean:	return vl[1];
	case TVariant::Object:	return true;
	default: break;
    }
    return EVAL_BOOL;
}

int TVariant::getI( ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_INT: atoi(getS().c_str());
	case TVariant::Integer:	return TSYS::getUnalignInt(vl.data()+1);
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_INT : (int)getR();
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_INT : getB();
	case TVariant::Object:	return 1;
	default: break;
    }
    return EVAL_INT;
}

double TVariant::getR( ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_REAL : atof(getS().c_str());
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_REAL : getI();
	case TVariant::Real:	return TSYS::getUnalignDbl(vl.data()+1);
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_REAL : getB();
	case TVariant::Object:	return 1;
	default: break;
    }
    return EVAL_REAL;
}

string TVariant::getS( ) const
{
    switch( type() )
    {
	case TVariant::String:	return vl.substr(1);
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_STR : TSYS::int2str(getI());
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_STR : TSYS::real2str(getR());
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_STR : TSYS::int2str(getB());
	case TVariant::Object:	return getO()->getStrXML();
	default: break;
    }
    return EVAL_STR;
}

TVarObj	*TVariant::getO( ) const
{
    if( type() != TVariant::Object ) throw TError("TVariant",_("Variable not object!"));
    TVarObj *rez = (TVarObj*)TSYS::str2addr(vl.substr(1));
    if( !rez ) throw TError("TVariant",_("Zero object using try!"));
    return rez;
}

void TVariant::setB( char ivl )
{
    if( type() != TVariant::Boolean )	setType(TVariant::Boolean);
    vl.replace(1,string::npos,(char*)&ivl,sizeof(char));
}

void TVariant::setI( int ivl )
{
    if( type() != TVariant::Integer )	setType(TVariant::Integer);
    vl.replace(1,string::npos,(char*)&ivl,sizeof(int));
}

void TVariant::setR( double ivl )
{
    if( type() != TVariant::Real )	setType(TVariant::Real);
    vl.replace(1,string::npos,(char*)&ivl,sizeof(double));
}

void TVariant::setS( const string &ivl )
{
    if( type() != TVariant::String )	setType(TVariant::String);
    vl.replace(1,string::npos,ivl);
}

void TVariant::setO( TVarObj *val )
{
    if( type() == TVariant::Object && getO() && !getO()->disconnect() ) delete getO( );
    if( type() != TVariant::Object )	setType(TVariant::Object);
    vl.replace(1,string::npos,TSYS::addr2str(val));
    if( val ) val->connect();
}

//***********************************************************
//* TVarObj                                                 *
//*   Variable object, by default included properties       *
//***********************************************************
TVarObj::TVarObj( ) : mUseCnt(0)
{

}

TVarObj::~TVarObj( )
{

}

int TVarObj::connect( )
{
    return ++mUseCnt;
}

int TVarObj::disconnect( )
{
    if( mUseCnt ) mUseCnt--;
    return mUseCnt;
}

void TVarObj::propList( vector<string> &ls )
{
    ls.clear();
    for( map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++ )
	ls.push_back(ip->first);
}

TVariant TVarObj::propGet( const string &id )
{
    map<string,TVariant>::iterator vit = mProps.find(id);
    if(vit == mProps.end()) return TVariant();

    return vit->second;
}

void TVarObj::propSet( const string &id, TVariant val )		{ mProps[id] = val; }

string TVarObj::getStrXML( const string &oid )
{
    string nd("<TVarObj");
    if(!oid.empty()) nd += " p='" + oid + "'";
    nd += ">\n";
    for( map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++ )
	switch( ip->second.type() )
	{
	    case TVariant::String:	nd += "<str p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</str>\n"; break;
	    case TVariant::Integer:	nd += "<int p='"+ip->first+"'>"+ip->second.getS()+"</int>\n"; break;
	    case TVariant::Real:	nd += "<real p='"+ip->first+"'>"+ip->second.getS()+"</real>\n"; break;
	    case TVariant::Boolean:	nd += "<bool p='"+ip->first+"'>"+ip->second.getS()+"</bool>\n"; break;
	    case TVariant::Object:	nd += ip->second.getO()->getStrXML(ip->first); break;
	    default: break;
	}
    nd += "</TVarObj>\n";

    return nd;
}

TVariant TVarObj::funcCall( const string &id, vector<TVariant> &prms )
{
    throw TError("VarObj",_("Function '%s' error or not enough parameters."),id.c_str());
}

//***********************************************************
//* TArrayObj                                                *
//*   Array object included indexed properties               *
//***********************************************************
TVariant TArrayObj::propGet( const string &id )
{
    if(id == "length") return (int)mEls.size();
    if(id.size() && isdigit(id[0]))
    {
	int vid = atoi(id.c_str());
	if(vid >= 0 && vid < (int)mEls.size()) return mEls[vid];
    }
    return TVarObj::propGet(id);
}

void TArrayObj::propSet( const string &id, TVariant val )
{
    if(id.size() && isdigit(id[0]))
    {
	int vid = atoi(id.c_str());
	if(vid < 0) throw TError("ArrayObj",_("Negative id is not allow for array."));
	while(vid >= (int)mEls.size()) mEls.push_back(TVariant());
	mEls[vid] = val;
    }
    else TVarObj::propSet(id,val);
}

string TArrayObj::getStrXML( const string &oid )
{
    string nd("<TArrayObj");
    if( !oid.empty() ) nd = nd + " p='" + oid + "'";
    nd = nd + ">\n";
    //> Array items process
    for(unsigned ip = 0; ip < mEls.size(); ip++)
	switch(mEls[ip].type())
	{
	    case TVariant::String:	nd += "<str>"+TSYS::strEncode(mEls[ip].getS(),TSYS::Html)+"</str>\n"; break;
	    case TVariant::Integer:	nd += "<int>"+mEls[ip].getS()+"</int>\n"; break;
	    case TVariant::Real:	nd += "<real>"+mEls[ip].getS()+"</real>\n"; break;
	    case TVariant::Boolean:	nd += "<bool>"+mEls[ip].getS()+"</bool>\n"; break;
	    case TVariant::Object:	nd += mEls[ip].getO()->getStrXML(); break;
	    default: break;
	}
    //> Object's properties process
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	switch(ip->second.type())
	{
	    case TVariant::String:	nd += "<str p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</str>\n"; break;
	    case TVariant::Integer:	nd += "<int p='"+ip->first+"'>"+ip->second.getS()+"</int>\n"; break;
	    case TVariant::Real:	nd += "<real p='"+ip->first+"'>"+ip->second.getS()+"</real>\n"; break;
	    case TVariant::Boolean:	nd += "<bool p='"+ip->first+"'>"+ip->second.getS()+"</bool>\n"; break;
	    case TVariant::Object:	nd += ip->second.getO()->getStrXML(ip->first); break;
	    default: break;
	}

    nd += "</TArrayObj>\n";

    return nd;
}

TVariant TArrayObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // string join(string sep = ",") - join items to string
    //  sep - items separator
    if( id == "join" || id == "toString" || id == "valueOf" )
    {
	string rez, sep = prms.size() ? prms[0].getS() : ",";
	for(unsigned i_e = 0; i_e < mEls.size(); i_e++)
	    rez += (i_e?sep:"")+mEls[i_e].getS();
	return rez;
    }
    // TArrayObj concat(TArrayObj arr) - concatenate array
    //  arr - source array
    if( id == "concat" && prms.size() && prms[0].type() == TVariant::Object && dynamic_cast<TArrayObj*>(prms[0].getO()) )
    {
	for( int i_p = 0; i_p < prms[0].getO()->propGet("length").getI(); i_p++ )
	    mEls.push_back(prms[0].getO()->propGet(TSYS::int2str(i_p)));
	return this;
    }
    // int push(ElTp var, ...) - push variables to array
    //  var - variable
    if( id == "push" && prms.size() )
    {
	for(unsigned i_p = 0; i_p < prms.size(); i_p++) mEls.push_back(prms[i_p]);
	return (int)mEls.size();
    }
    // ElTp pop( ) - pop variable from array
    if( id == "pop" )
    {
	if( mEls.empty() ) throw TError("ArrayObj",_("Array is empty."));
	TVariant val = mEls.back();
	mEls.pop_back();
	return val;
    }
    // Array reverse( ) - reverse array's items order
    if( id == "reverse" )		{ reverse(mEls.begin(),mEls.end()); return this; }
    // ElTp shift( ) - shift array's items upward
    if( id == "shift" )
    {
	if( mEls.empty() ) throw TError("ArrayObj",_("Array is empty."));
	TVariant val = mEls.front();
	mEls.erase(mEls.begin());
	return val;
    }
    // int unshift(ElTp var, ...) - shift items to array upward
    //  var - variable
    if( id == "unshift" && prms.size() )
    {
	for(unsigned i_p = 0; i_p < prms.size(); i_p++) mEls.insert(mEls.begin()+i_p,prms[i_p]);
	return (int)mEls.size();
    }
    // Array slice(int beg, int end) - get array part from positon <beg> to <end>
    //  beg - begin position
    //  end - end position
    if( id == "slice" && prms.size() )
    {
	int beg = prms[0].getI();
	if( beg < 0 ) beg = mEls.size()-1+beg;
	int end = prms.size()-1;
	if( prms.size()>=2 ) end = prms[1].getI();
	if( end < 0 ) end = mEls.size()-1+end;
	end = vmin(end,(int)mEls.size()-1);
	TArrayObj *rez = new TArrayObj();
	for( int i_p = beg; i_p <= end; i_p++ )
	    rez->propSet( TSYS::int2str(i_p-beg), prms[i_p] );
	return rez;
    }
    // Array splice(int beg, int remN, ElTp val1, ElTp val2, ...) - insert, remove or replace array's items
    //  beg - start position
    //  remN - removed items number
    //  val1, val2 - values for insert
    if( id == "splice" && prms.size() >= 1 )
    {
	int beg = vmax(0,prms[0].getI());
	int cnt = (prms.size()>1) ? prms[1].getI() : mEls.size();
	//> Delete elements
	TArrayObj *rez = new TArrayObj();
	for(int i_c = 0; i_c < cnt && beg < (int)mEls.size(); i_c++)
	{
	    rez->propSet( TSYS::int2str(i_c), mEls[beg] );
	    mEls.erase(mEls.begin()+beg);
	}
	//> Insert elements
	for(unsigned i_c = 2; i_c < prms.size(); i_c++)
	    mEls.insert(mEls.begin()+beg+i_c-2,prms[i_c]);
	return rez;
    }
    // Array sort( ) - lexicographic items sorting
    if( id == "sort" )
    {
	sort(mEls.begin(),mEls.end(),compareLess);
	return this;
    }

    throw TError("ArrayObj",_("Function '%s' error or not enough parameters."),id.c_str());
}

bool TArrayObj::compareLess( const TVariant &v1, const TVariant &v2 )
{
    return v1.getS() < v2.getS();
}

//***********************************************************
//* TRegExp                                                 *
//*   Regular expression object                             *
//***********************************************************
TRegExp::TRegExp( const string &rule, const string &flg ) : lastIndex(0), pattern(rule), regex(NULL), vSz(90), capv(NULL)
{
    global = (flg.find('g')!=string::npos);
    ignoreCase = (flg.find('i')!=string::npos);
    multiline = (flg.find('m')!=string::npos);

    const char *terr;
    int erroff;
    regex = pcre_compile(pattern.c_str(),PCRE_DOTALL|(ignoreCase?PCRE_CASELESS:0)|(multiline?PCRE_MULTILINE:0),&terr,&erroff,NULL);
    if(!regex) err = terr;
    else capv = new int[90];
}

TRegExp::~TRegExp( )
{
    if(capv)	delete [] capv;
    if(regex)	pcre_free(regex);
}

TArrayObj *TRegExp::match( const string &vl, bool all )
{
    TArrayObj *rez = new TArrayObj();
    if(!regex) return rez;

    if(all && global)
	for(int curPos = 0, i_n = 0; pcre_exec(regex,NULL,vl.data(),vl.size(),curPos,0,capv,vSz) > 0; curPos = capv[1], i_n++)
	    rez->propSet(TSYS::int2str(i_n), string(vl.data()+capv[0],capv[1]-capv[0]));
    else
    {
	int n = pcre_exec(regex, NULL, vl.data(), vl.size(), (global?lastIndex:0), 0, capv, vSz);
	for(int i_n = 0; i_n < n; i_n++)
    	    rez->propSet(TSYS::int2str(i_n), string(vl.data()+capv[i_n*2],capv[i_n*2+1]-capv[i_n*2]));
	if(global) lastIndex = (n>0) ? capv[1] : 0;
	if(n > 0) { rez->propSet("index",capv[0]); rez->propSet("input",vl); }
    }
    return rez;
}

string TRegExp::replace( const string &vl, const string &str )
{
    string rez = vl, repl;
    if(!regex) return rez;
    for(int curPos = 0, n; (!curPos || global) && (n=pcre_exec(regex,NULL,rez.data(),rez.size(),curPos,0,capv,vSz)) > 0; curPos = capv[0]+repl.size())
    {
	repl = substExprRepl(str,rez,capv,n);
	rez.replace(capv[0],capv[1]-capv[0],repl);
    }
    return rez;
}

TArrayObj *TRegExp::split( const string &vl, int limit )
{
    TArrayObj *rez = new TArrayObj();
    if(!regex) return rez;
    int curPos = 0, i_n = 0;
    for(int se = 0; (se=pcre_exec(regex,NULL,vl.data(),vl.size(),curPos,0,capv,vSz)) > 0 && (!limit || i_n < limit); curPos = capv[1])
    {
	rez->propSet(TSYS::int2str(i_n++), string(vl.data()+curPos,capv[0]-curPos));
	for(int i_se = 1; i_se < se && (!limit || i_n < limit); i_se++)
    	    rez->propSet(TSYS::int2str(i_n++), string(vl.data()+capv[i_se*2],capv[i_se*2+1]-capv[i_se*2]));
    }
    if(curPos <= (int)vl.size() && (!limit || i_n < limit)) rez->propSet(TSYS::int2str(i_n++), string(vl.data()+curPos,vl.size()-curPos));
    return rez;
}

bool TRegExp::test( const string &vl )
{
    if(!regex) return false;
    int n = pcre_exec(regex, NULL, vl.data(), vl.size(), (global?lastIndex:0), 0, capv, vSz);
    if(global) lastIndex = (n>0) ? capv[1] : 0;
    return (n>0);
}

int TRegExp::search( const string &vl )
{
    if(!regex) return -1;
    int n = pcre_exec(regex, NULL, vl.data(), vl.size(), 0, 0, capv, vSz);
    return (n>0) ? capv[0] : -1;
}

string TRegExp::substExprRepl( const string &str, const string &val, int *capv, int n )
{
    string rez = str;
    for(size_t cpos = 0; n > 0 && (cpos=rez.find("$",cpos)) != string::npos && cpos < (rez.size()-1); )
	switch(rez[cpos+1])
	{
	    case '$':	rez.replace(cpos,2,"$"); cpos++; break;
	    case '`':	rez.replace(cpos,2,val,0,capv[0]); cpos += capv[0]; break;
	    case '\'':	rez.replace(cpos,2,val,capv[1],val.size()-capv[1]);cpos += (val.size()-capv[1]); break;
	    case '&':	rez.replace(cpos,2,val,capv[0],(capv[1]-capv[0])); cpos += capv[1]; break;
	    default:
	    {
		int nd = isdigit(rez[cpos+1]) ? 1 : 0;
		if(nd && cpos < (rez.size()-2) && isdigit(rez[cpos+2])) nd++;
		int subexp = atoi(string(rez.data()+cpos+1,nd).c_str());
		string replVl;
		if(subexp > 0 && subexp < n) replVl.assign(val,capv[subexp*2],capv[subexp*2+1]-capv[subexp*2]);
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
    throw TError("RegExp",_("Function '%s' error or not enough parameters."),id.c_str());
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
XMLNodeObj::XMLNodeObj(const string &name) : mName(name), parent(NULL)
{

}

XMLNodeObj::~XMLNodeObj( )
{
    while(childSize()) childDel(0);
}

void XMLNodeObj::childAdd(XMLNodeObj *nd)
{
    mChilds.push_back(nd);
    nd->parent = this;
    nd->connect();
}

void XMLNodeObj::childIns(unsigned id, XMLNodeObj *nd)
{
    if(id < 0) id = mChilds.size();
    id = vmin(id,mChilds.size());
    mChilds.insert(mChilds.begin()+id,nd);
    nd->parent = this;
    nd->connect();
}

void XMLNodeObj::childDel( unsigned id )
{
    if(id < 0 || id >= mChilds.size()) throw TError("XMLNodeObj",_("Deletion child '%d' error."),id);
    if(!mChilds[id]->disconnect()) delete mChilds[id];
    mChilds.erase(mChilds.begin()+id);
}

XMLNodeObj *XMLNodeObj::childGet(unsigned id)
{
    if(id < 0 || id >= mChilds.size()) throw TError("XMLNodeObj",_("Child '%d' is not allow."),id);
    return mChilds[id];
}

string XMLNodeObj::getStrXML(const string &oid)
{
    string nd("<XMLNodeObj:"+name());
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	nd += " "+ip->first+"=\""+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"\"";
    nd += ">"+TSYS::strEncode(text(),TSYS::Html)+"\n";
    for(unsigned i_ch = 0; i_ch < mChilds.size(); i_ch++)
	nd += mChilds[i_ch]->getStrXML();
    nd += "</XMLNodeObj:"+name()+">\n";

    return nd;
}

TVariant XMLNodeObj::funcCall(const string &id, vector<TVariant> &prms)
{
    // string name( ) - node name
    if(id == "name")	return name();
    // string text( ) - node text
    if(id == "text")	return text();
    // string attr(string id) - get node attribute
    //  id - attribute identifier
    if(id == "attr" && prms.size())	return propGet(prms[0].getS()).getS();
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
    // int childSize( ) - return childs counter for node
    if(id == "childSize")	return (int)childSize();
    // XMLNodeObj childAdd(ElTp no = XMLNodeObj) - add node <no> as child to the node
    //  no - node object or name for new node
    if(id == "childAdd")
    {
	XMLNodeObj *no = NULL;
	if(prms.size() && prms[0].type() == TVariant::Object && dynamic_cast<XMLNodeObj*>(prms[0].getO()))
	    no = (XMLNodeObj*)prms[0].getO();
	else if(prms.size()) no = new XMLNodeObj(prms[0].getS());
	else no = new XMLNodeObj();
	childAdd(no);
	return no;
    }
    // XMLNodeObj childIns(int id, ElTp no = XMLNodeObj) - insert node <no> as child to the node
    //  id - insert position
    //  no - node object or name for new node
    if(id == "childIns" && prms.size())
    {
	XMLNodeObj *no = NULL;
	if(prms.size() > 1 && prms[1].type() == TVariant::Object && dynamic_cast<XMLNodeObj*>(prms[1].getO()))
	    no = (XMLNodeObj*)prms[1].getO();
	else if(prms.size() > 1) no = new XMLNodeObj(prms[1].getS());
	else no = new XMLNodeObj();
	childIns(prms[0].getI(), no);
	return no;
    }
    // XMLNodeObj childDel(int id) - remove child node from position <id>
    //  id - child node position
    if(id == "childDel" && prms.size())	{ childDel(prms[0].getI()); return this; }
    // XMLNodeObj childGet(int id) - get node from position <id>
    //  id - child node position
    if(id == "childGet" && prms.size())	return childGet(prms[0].getI());
    // XMLNodeObj parent() - get parent node
    if(id == "parent")	return parent ? TVariant(parent) : TVariant(false);
    // string load(string str, bool file = false, bool sepTextNodes = false) - load XML tree from XML-stream from string or file
    //  str - source stream string or file name, for <file> = true
    //  file - load XML-tree from file (true) or stram (false)
    //  sepTextNodes - nodes text load into separated nodes "<*>"
    if(id == "load" && prms.size())
    {
	XMLNode nd;
	//> Load from file
	if(prms.size() >= 2 && prms[1].getB())
	{
	    string s_buf;
	    int hd = open(prms[0].getS().c_str(),O_RDONLY);
	    if(hd < 0) return TSYS::strMess(_("2:Open file <%s> error: %s"),prms[0].getS().c_str(),strerror(errno));
	    int cf_sz = lseek(hd,0,SEEK_END);
	    if(cf_sz > 0)
	    {
		lseek(hd,0,SEEK_SET);
		char *buf = (char *)malloc(cf_sz+1);
		read(hd,buf,cf_sz);
		buf[cf_sz] = 0;
		s_buf = buf;
		free(buf);
	    }
	    close(hd);

	    try{ nd.load(s_buf, ((prms.size()>=3)?prms[2].getB():false)); }
	    catch(TError err) { return "1:"+err.mess; }
	}
	//> Load from string
	else
	    try{ nd.load(prms[0].getS(), ((prms.size()>=3)?prms[2].getB():false)); }
	    catch(TError err) { return "1:"+err.mess; }
	fromXMLNode(nd);
	return string("0");
    }
    // string save(int opt = 0, string path = "") - save XML-tree to string or file stream
    //  opt - save options:
    //   0x01 - interrupt the string before the opening tag;
    //   0x02 - interrupt the string after the opening tag;
    //   0x04 - interrupt the string after a closing tag;
    //   0x08 - interrupt the string after the text;
    //   0x10 - interrupt the string after the instruction;
    //   0x1E - interrupt the string after all.
    //  path - file path for save to file
    if( id == "save" )
    {
	XMLNode nd;
	toXMLNode(nd);
	string s_buf = nd.save( (prms.size()>=1)?prms[0].getI():0 );
	//> Save to file
	if( prms.size() >= 2 )
	{
	    int hd = open( prms[1].getS().c_str(), O_RDWR|O_CREAT|O_TRUNC, 0664 );
	    if( hd < 0 ) return string("");
	    write(hd,s_buf.data(),s_buf.size());
	}
	return s_buf;
    }
    // XMLNodeObj getElementBy( string val, string attr = "id" ) - get element from the tree by attribute <attr> value <val>.
    //  val - attribute value for find;
    //  attr - attribute name for find it value.
    if( id == "getElementBy" && prms.size())
    {
	XMLNodeObj *rez = getElementBy(((prms.size() >= 2) ? prms[1].getS() : "id"), prms[0].getS());
	if(!rez) return TVariant();
	return rez;
    }

    throw TError("XMLNodeObj",_("Function '%s' error or not enough parameters."),id.c_str());
}

void XMLNodeObj::toXMLNode(XMLNode &nd)
{
    nd.clear();
    nd.setName(name())->setText(text());
    for(map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++)
	nd.setAttr(ip->first,ip->second.getS());
    for(unsigned i_ch = 0; i_ch < mChilds.size(); i_ch++)
	mChilds[i_ch]->toXMLNode(*nd.childAdd());
}

void XMLNodeObj::fromXMLNode(XMLNode &nd)
{
    while(childSize()) childDel(0);

    setName(nd.name());
    setText(nd.text());

    vector<string> alst;
    nd.attrList(alst);
    for(unsigned i_a = 0; i_a < alst.size(); i_a++)
	propSet(alst[i_a],nd.attr(alst[i_a]));

    for(unsigned i_ch = 0; i_ch < nd.childSize(); i_ch++)
    {
	XMLNodeObj *xn = new XMLNodeObj();
	childAdd(xn);
	xn->fromXMLNode(*nd.childGet(i_ch));
    }
}

XMLNodeObj *XMLNodeObj::getElementBy( const string &attr, const string &val )
{
    if(propGet(attr).getS() == val)	return this;

    XMLNodeObj *rez = NULL;
    for(unsigned i_ch = 0; !rez && i_ch < childSize(); i_ch++)
	rez = childGet(i_ch)->getElementBy(attr,val);

    return rez;
}

//***********************************************************
//* TCntrNodeObj                                            *
//*   TCntrNode object for access to system's objects       *
//***********************************************************
TCntrNodeObj::TCntrNodeObj( AutoHD<TCntrNode> ind, const string &iuser ) : mUser(iuser)
{
    cnd = ind;
}

TVariant TCntrNodeObj::propGet( const string &id )
{
    if(cnd.freeStat()) return TVariant();
    try
    {
	AutoHD<TCntrNode> nnd = cnd.at().nodeAt(id);
	return new TCntrNodeObj(nnd,user());
    }
    catch(...) { }

    TVariant rez = cnd.at().objPropGet(id);
    if(rez.isNull()) return TVariant();
    return rez;
}

void TCntrNodeObj::propSet( const string &id, TVariant val )
{
    if(cnd.freeStat()) return;
    cnd.at().objPropSet(id,val);
}

string TCntrNodeObj::getStrXML(const string &oid)
{
    return "<TCntrNodeObj path=\""+cnd.at().nodePath()+"\"/>";
}

TVariant TCntrNodeObj::funcCall( const string &id, vector<TVariant> &prms )
{
    if(cnd.freeStat()) throw TError("TCntrNodeObj",_("The object don't attached to node of OpenSCADA tree."));
    return cnd.at().objFuncCall(id, prms, user());
}

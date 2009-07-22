
//OpenSCADA system file: tvariant.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

#include <stdlib.h>

#include <tsys.h>
#include "tvariant.h"

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

TVariant::~TVariant( )				{ setType(TVariant::Null); }

void TVariant::setType( Type tp )
{
    if( tp == type() )	return;

    if( type() == TVariant::Object && !getO()->disconnect() ) delete getO( );

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
    }
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
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_BOOL : (bool)atoi(getS().c_str());
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_BOOL : (bool)getI();
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_BOOL : (bool)getR();
	case TVariant::Boolean:	return vl[1];
    }
    return EVAL_BOOL;
}

int TVariant::getI( ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_INT: atoi(getS().c_str());
	case TVariant::Integer:	return *(int*)(vl.data()+1);
	case TVariant::Real:	return (getR()==EVAL_REAL) ? EVAL_INT : (int)getR();
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_INT : getB();
    }
    return EVAL_INT;
}

double TVariant::getR( ) const
{
    switch( type() )
    {
	case TVariant::String:	return (getS()==EVAL_STR) ? EVAL_REAL : atof(getS().c_str());
	case TVariant::Integer:	return (getI()==EVAL_INT) ? EVAL_REAL : getI();
	case TVariant::Real:	return *(double*)(vl.data()+1);
	case TVariant::Boolean:	return (getB()==EVAL_BOOL) ? EVAL_REAL : getB();
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
    }
    return EVAL_STR;
}

TVarObj	*TVariant::getO( ) const
{
    if( type() != TVariant::Object ) throw TError("TVariant",_("Variable not object!"));
    return (TVarObj*)TSYS::str2addr(vl.substr(1));
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
    if( type() == TVariant::Object && !getO()->disconnect() ) delete getO( );
    if( type() != TVariant::Object )	setType(TVariant::Object);
    vl.replace(1,string::npos,TSYS::addr2str(val));
    val->connect();
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

TVariant TVarObj::propGet( const string &id )
{
    map<string,TVariant>::iterator vit = mProps.find(id);
    if( vit == mProps.end() ) return TVariant();

    return vit->second;
}

void TVarObj::propSet( const string &id, TVariant val )		{ mProps[id] = val; }

string TVarObj::getStrXML( const string &oid )
{
    string nd("<TVarObj");
    if( !oid.empty() ) nd += " p='" + oid + "'";
    nd += ">\n";
    for( map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++ )
	switch( ip->second.type() )
	{
	    case TVariant::String:	nd += "<str p='"+ip->first+"'>"+TSYS::strEncode(ip->second.getS(),TSYS::Html)+"</str>\n"; break;
	    case TVariant::Integer:	nd += "<int p='"+ip->first+"'>"+ip->second.getS()+"</int>\n"; break;
	    case TVariant::Real:	nd += "<real p='"+ip->first+"'>"+ip->second.getS()+"</real>\n"; break;
	    case TVariant::Boolean:	nd += "<bool p='"+ip->first+"'>"+ip->second.getS()+"</bool>\n"; break;
	    case TVariant::Object:	nd += ip->second.getO()->getStrXML(ip->first); break;
	}
    nd += "</TVarObj>\n";

    return nd;
}

TVariant TVarObj::funcCall( const string &id, vector<TVariant> &prms )
{
    throw TError("VarObj",_("Function '%s' error or not enough parameters."),id.c_str());
}



//***********************************************************
//* TAreaObj                                                *
//*   Area object included indexed properties               *
//***********************************************************
TVariant TAreaObj::propGet( const string &id )
{
    if( id == "length" ) return (int)mEls.size();

    int vid = atoi(id.c_str());
    if( vid < 0 || vid >= mEls.size() ) throw TError("AreaObj",_("Array element '%d' is not allow."),vid);
    return mEls[vid];
}

void TAreaObj::propSet( const string &id, TVariant val )
{
    int vid = atoi(id.c_str());
    if( vid < 0 ) throw TError("AreaObj",_("Negative id is not allow for array."));
    while( vid >= mEls.size() ) mEls.push_back(TVariant());
    mEls[vid] = val;
}

string TAreaObj::getStrXML( const string &oid )
{
    string nd("<TAreaObj");
    if( !oid.empty() ) nd = nd + " p='" + oid + "'";
    nd = nd + ">\n";
    for( int ip = 0; ip < mEls.size(); ip++ )
	switch( mEls[ip].type() )
	{
	    case TVariant::String:	nd += "<str>"+TSYS::strEncode(mEls[ip].getS(),TSYS::Html)+"</str>\n"; break;
	    case TVariant::Integer:	nd += "<int>"+mEls[ip].getS()+"</int>\n"; break;
	    case TVariant::Real:	nd += "<real>"+mEls[ip].getS()+"</real>\n"; break;
	    case TVariant::Boolean:	nd += "<bool>"+mEls[ip].getS()+"</bool>\n"; break;
	    case TVariant::Object:	nd += mEls[ip].getO()->getStrXML(); break;
	}
    nd += "</TAreaObj>\n";

    return nd;
}

TVariant TAreaObj::funcCall( const string &id, vector<TVariant> &prms )
{
    if( id == "join" || id == "toString" || id == "valueOf" )
    {
	string rez, sep = prms.size() ? prms[0].getS() : ",";
	for( int i_e = 0; i_e < mEls.size(); i_e++ )
	    rez += (i_e?sep:"")+mEls[i_e].getS();
	return rez;
    }
    if( id == "concat" && prms.size() && prms[0].type() == TVariant::Object && dynamic_cast<TAreaObj*>(prms[0].getO()) )
    {
	for( int i_p = 0; i_p < prms[0].getO()->propGet("length").getI(); i_p++ )
	    mEls.push_back(prms[0].getO()->propGet(TSYS::int2str(i_p)));
	return this;
    }
    if( id == "push" && prms.size() )
    {
	for( int i_p = 0; i_p < prms.size(); i_p++ ) mEls.push_back(prms[i_p]);
	return (int)mEls.size();
    }
    if( id == "pop" )
    {
	if( mEls.empty() ) throw TError("AreaObj",_("Array is empty."));
	TVariant val = mEls.back();
	mEls.pop_back();
	return val;
    }
    if( id == "reverse" )		{ reverse(mEls.begin(),mEls.end()); return this; }
    if( id == "shift" )
    {
	if( mEls.empty() ) throw TError("AreaObj",_("Array is empty."));
	TVariant val = mEls.front();
	mEls.erase(mEls.begin());
	return val;
    }
    if( id == "unshift" && prms.size() )
    {
	for( int i_p = 0; i_p < prms.size(); i_p++ ) mEls.insert(mEls.begin()+i_p,prms[i_p]);
	return (int)mEls.size();
    }
    if( id == "slice" && prms.size() )
    {
	int beg = prms[0].getI();
	if( beg < 0 ) beg = mEls.size()-1+beg;
	int end = prms.size()-1;
	if( prms.size()>=2 ) end = prms[1].getI();
	if( end < 0 ) end = mEls.size()-1+end;
	end = vmin(end,mEls.size()-1);
	TAreaObj *rez = new TAreaObj();
	for( int i_p = beg; i_p <= end; i_p++ )
	    rez->propSet( TSYS::int2str(i_p-beg), prms[i_p] );
	return rez;
    }
    if( id == "splice" && prms.size() >= 1 )
    {
	int beg = vmax(0,prms[0].getI());
	int cnt = (prms.size()>1) ? prms[1].getI() : mEls.size();
	//> Delete elements
	TAreaObj *rez = new TAreaObj();
	for( int i_c = 0; i_c < cnt && beg < mEls.size(); i_c++ )
	{
	    rez->propSet( TSYS::int2str(i_c), mEls[beg] );
	    mEls.erase(mEls.begin()+beg);
	}
	//> Insert elements
	for( int i_c = 2; i_c < prms.size(); i_c++ )
	    mEls.insert(mEls.begin()+beg+i_c-2,prms[i_c]);
	return rez;
    }
    if( id == "sort" )
    {
	sort(mEls.begin(),mEls.end(),compareLess);
	return this;
    }

    throw TError("AreaObj",_("Function '%s' error or not enough parameters."),id.c_str());
}

bool TAreaObj::compareLess( const TVariant &v1, const TVariant &v2 )
{
    return v1.getS() < v2.getS();
}

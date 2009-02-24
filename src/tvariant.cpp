
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
TVariant::TVariant( )
{
    vl.assign(1,(char)TVariant::Null);
}

TVariant::TVariant( bool ivl )
{
    vl.assign(1,(char)TVariant::Null);
    setB(ivl);
}

TVariant::TVariant( int ivl )
{
    vl.assign(1,(char)TVariant::Null);
    setI(ivl);
}

TVariant::TVariant( double ivl )
{
    vl.assign(1,(char)TVariant::Null);
    setR(ivl);
}

TVariant::TVariant( string ivl )
{
    vl.assign(1,(char)TVariant::Null);
    setS(ivl);
}

bool TVariant::operator==( TVariant &vr )
{
    return vr.vl == vl;
}

TVariant &TVariant::operator=( TVariant &vr )
{
    vl = vr.vl;
    return *this;
}

bool TVariant::getB( bool def )
{
    switch( type() )
    {
	case TVariant::String:	return atoi(getS().c_str());
	case TVariant::Integer:	return getI();
	case TVariant::Real:	return (int)getR();
	case TVariant::Boolean:	return (bool)vl[1];
	default: return def;
    }
}

int TVariant::getI( int def )
{
    switch( type() )
    {
	case TVariant::String:	return atoi(getS().c_str());
	case TVariant::Integer:	return *(int*)(vl.data()+1);
	case TVariant::Real:	return (int)getR();
	case TVariant::Boolean:	return getB();
	default: return def;
    }
}

double TVariant::getR( double def )
{
    switch( type() )
    {
	case TVariant::String:	return atof(getS().c_str());
	case TVariant::Integer:	return getI();
	case TVariant::Real:	return *(double*)(vl.data()+1);
	case TVariant::Boolean:	return getB();
	default: return def;
    }
}

string TVariant::getS( const string &def )
{
    switch( type() )
    {
	case TVariant::String:	return vl.substr(1);
	case TVariant::Integer:	return TSYS::int2str(getI());
	case TVariant::Real:	return TSYS::real2str(getR());
	case TVariant::Boolean:	return TSYS::int2str(getB());
	default: return def;
    }
}

void TVariant::setB( bool ivl )
{
    if( type() != TVariant::Boolean )
    {
	vl.assign(1,(char)TVariant::Boolean);
	vl.reserve(1+sizeof(bool));
    }
    vl.replace(1,string::npos,(char*)&ivl,sizeof(bool));
}

void TVariant::setI( int ivl )
{
    if( type() != TVariant::Integer )
    {
	vl.assign(1,(char)TVariant::Integer);
	vl.reserve(1+sizeof(int));
    }
    vl.replace(1,string::npos,(char*)&ivl,sizeof(int));
}

void TVariant::setR( double ivl )
{
    if( type() != TVariant::Real )
    {
	vl.assign(1,(char)TVariant::Real);
	vl.reserve(1+sizeof(double));
    }
    vl.replace(1,string::npos,(char*)&ivl,sizeof(double));
}

void TVariant::setS( const string &ivl )
{
    if( type() != TVariant::String )
    {
	vl.assign(1,(char)TVariant::String);
    }
    vl.replace(1,string::npos,ivl);
}


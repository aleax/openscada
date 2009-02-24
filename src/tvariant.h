
//OpenSCADA system file: tvariant.h
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

#ifndef TVARIANT_H
#define TVARIANT_H

#include <string>

using std::string;


//*************************************************
//* TVariant                                      *
//*************************************************
class TVariant
{
    public:
	//Data
	enum Type
	{
	    Null	= 0,
	    Boolean	= 1,
	    Integer	= 2,
	    Real	= 3,
	    String	= 4
	};

	//Methods
	TVariant( );
	TVariant( bool ivl );
	TVariant( int ivl );
	TVariant( double ivl );
	TVariant( string ivl );

	~TVariant( )	{ }

	bool operator==( TVariant &vr );
	TVariant &operator=( TVariant &vr );

	bool isNull( )	{ return (vl[0]==TVariant::Null); }
	Type type( )	{ return (Type)vl[0]; }

	bool	getB( bool def = false );
	int	getI( int def = 0 );
	double	getR( double def = 0 );
	string	getS( const string &def = "" );

	void setB( bool val );
	void setI( int val );
	void setR( double val );
	void setS( const string &val );

    private:
	//Attributes
	string vl;
};

#endif // TVARIANT_H

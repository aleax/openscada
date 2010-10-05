
//OpenSCADA system file: terror.h
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
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

#ifndef TERROR_H
#define TERROR_H

#include <string>

using std::string;

namespace OSCADA
{

//*************************************************
//* TError                                        *
//*************************************************
class TError
{
    public:
	//Methods
	TError( const char *cat, const char *fmt, ... );
	TError( int cod, const char *cat, const char *fmt, ... );

	//Attributes
	int	cod;
	string	cat;
	string	mess;
};

}

#endif // TERROR_H

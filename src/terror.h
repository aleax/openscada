
//OpenSCADA file: terror.h
/***************************************************************************
 *   Copyright (C) 2003-2018,2020 by Roman Savochenko, <roman@oscada.org>  *
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
	//Data
	enum Codes {
	    NoCode = 0, NoError = 0,
	    Core_CntrWarning = 1,
	    Core_CntrError = 2,
	    Core_RWLock_EDEADLK = 3,
	    Core_TaskCrt = 4,
	    Tr_Connect = 10,
	    Tr_UnknownHost = 11,
	    DB_SQL_Connect = 20,
	    DB_SQL_Query = 21,
	    Arch_Val_OldBufVl = 30,
	    EXT = 100
	};

	//Methods
	TError( ) : cod(NoCode)	{ }
	TError( const char *cat, const char *mess, ... );
	TError( int cod, const char *cat, const char *mess, ... );

	//Attributes
	int	cod;
	string	cat;
	string	mess;
};

}

#endif // TERROR_H

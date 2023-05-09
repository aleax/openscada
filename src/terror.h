
//OpenSCADA file: terror.h
/***************************************************************************
 *   Copyright (C) 2003-2023 by Roman Savochenko, <roman@oscada.org>       *
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
	    Core_NoNode = 5,
	    Tr_Connect = 10,
	    Tr_UnknownHost = 11,
	    Tr_ErrTransport = 12,
	    Tr_ErrResponse = 13,
	    Tr_ErrDevice = 14,
	    DB_SQL_Connect = 20,
	    DB_SQL_Query = 21,
	    DB_TrRemoved = 22,
	    DB_ReadOnly = 23,
	    Arch_Val_OldBufVl = 30,
	    Arch_Val_DblVSrc = 31,
	    EXT = 100
	};

	//Methods
	TError( ) : cod(NoCode)	{ }
	TError( const char *cat, const char *mess, ... );
	TError( int cod, const char *cat, const char *mess, ... );
	TError( const string &icat, const string &imess ) : cod(NoCode), cat(icat), mess(imess)	{ }
	TError( int icod, const string &icat, const string &imess ) : cod(icod), cat(icat), mess(imess)	{ }

	//Attributes
	int	cod;
	string	cat;
	string	mess;
};

}

#endif // TERROR_H

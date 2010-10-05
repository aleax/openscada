
//OpenSCADA system file: ttiparam.h
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


#ifndef TTIPARAM_H
#define TTIPARAM_H

#include <string>
#include <vector>

#include "tconfig.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TTipParam                                     *
//*************************************************
class TTipParam : public TElem
{
    public:
	//Methods
	TTipParam( const char *iid, const char *iname, const char *idb ) :
	    name(iid), descr(iname), db(idb)	{ };
	
	//Attributes
	string name;
	string descr;
	string db;
};

}

#endif // TTIPARAM_H

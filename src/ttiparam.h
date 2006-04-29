
//OpenSCADA system file: ttiparam.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

class TTipParam : public TElem
{
    public:
	TTipParam( const char *id, const char *name, const char *db );
	~TTipParam(  );
	
	string name()  { return(_name); }
	string lName() { return(_descr); }
	string BD()    { return(_bd); }
	
    private:
	string _name;
	string _descr;
	string _bd;
};

#endif // TTIPARAM_H


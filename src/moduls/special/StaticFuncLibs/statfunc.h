/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#ifndef STATFUNC_H
#define STATFUNC_H

#include <string>
#include <vector>

#include <tspecials.h>

using std::string;
using std::vector;

namespace StatFunc
{

//Complex1 functions library
class Libs : public TSpecial
{
    public:
	Libs( string src );
	~Libs();
	
	void modStart( );
        void modStop( );		

    private:
	void postEnable( );
};

extern Libs *st_lib;

} //End namespace StatFunc

#endif //STATFUNC_H


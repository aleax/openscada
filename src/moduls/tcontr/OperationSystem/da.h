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
 
#ifndef DA_H
#define DA_H

#include <tcontroller.h>

namespace SystemCntr
{
class TMdPrm;

class DA: public ::TElem
{
    public:
        DA( ) : TElem("da_el") { }
		    
	virtual string id() = 0;
	virtual string name() = 0;
	
        virtual void init( TMdPrm *prm ) 	{ }
	virtual void deInit( TMdPrm *prm )	{ }
        virtual void getVal( TMdPrm *prm )	{ }
	
	virtual void makeActiveDA( TController *a_cntr )	{ }
};

} //End namespace 

#endif //DA_H


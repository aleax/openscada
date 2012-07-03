
//OpenSCADA system module DAQ.System file: da_cpu.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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

#ifndef DA_CPU_H
#define DA_CPU_H

#include "da.h"

namespace SystemCntr
{

//*************************************************
//* CPU                                           *
//*************************************************
class CPU: public DA
{
    public:
	//Methods
	CPU( );
	~CPU( );

	string id( )	{ return "CPU"; }
	string name( )	{ return _("CPU"); }

	void init( TMdPrm *prm );
	void deInit( TMdPrm *prm );
	void getVal( TMdPrm *prm );

	void makeActiveDA( TMdContr *a_cntr );

    private:
	//Attributes
	struct tval
	{
	    long user;
	    long nice;
	    long sys;
	    long idle;
	};
};

} //End namespace

#endif //DA_CPU_H

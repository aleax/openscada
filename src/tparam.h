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

#ifndef TPARAM_H
#define TPARAM_H

#include <string>
#include <vector>

#include "tcontrollers.h"

using std::string;
using std::vector;

class SCntrS;
class SHDCntr;

struct SParam
{
    SHDCntr c_hd;
    int     p_hd;
};

class TParamContr;

class TParam 
{
    /** Public methods: */
    public:
	TParam( SCntrS cntr, string name, TParamS *prms );

	~TParam(  );

	string &Name() { return(name); }
	// Registering controller's param
    	int Reg( SCntrS cntr, string name );
	// Unregistering controller's param
	int UnReg( SCntrS cntr, string name );

	TParamContr &at();    

    	TParamS &Owner() { return( *owner ); }
    private:

    private:
	string name;
	vector<SParam> PrmC;
	int    work;
	int    hd_res;
    
	TParamS  *owner;
	static const char *o_name;
};

#endif // TPARAM_H

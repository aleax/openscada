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

class TParamContr;
class TParamS;

class TParam : public TCntrNode
{
    /** Public methods: */
    public:
	TParam( TControllerS::SName cntr, const string &name, TParamS *prms );
	~TParam(  );

	string &name() { return(m_name); }
	
	// Reg/Unreg controller's param
    	int reg( TControllerS::SName cntr, const string &name );
	int unreg( TControllerS::SName cntr, const string &name );

	TParamContr &at();    

    	TParamS &owner() { return *(TParamS*)nodePrev(); }
	
    protected:
	string nodeName(){ return m_name; }
	
    private:
	struct SEl
	{
	    AutoHD<TController> ctr;
	    AutoHD<TParamContr> prm;
    	};				

    private:
	string m_name;
	vector<SEl> PrmC;
	int    work;
	int    hd_res;
};

#endif // TPARAM_H

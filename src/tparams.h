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

#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
#include <vector>

#include "tconfig.h"
#include "tparam.h"
#include "tcontrollers.h"

using std::string;
using std::vector;

class TParam;
class TParamContr;

class TParamS : public TSubSYS, public TConfig
{
    /** Public methods: */
    public:
	TParamS( );    
	~TParamS( );

	//Parameters
	void list( vector<string> &list )	{ chldList(m_prm,list); }
	bool present( const string &param )	{ return chldPresent(m_prm,param); }
	void add( TControllerS::SName cntr, const string &param );
	void del( TControllerS::SName cntr, const string &param );
	AutoHD<TParam> at( const string &name, const string &who = "" )
	{ return chldAt(m_prm,name); }	    
             
    /** Private methods: */
    private:
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
    /**Attributes: */
    private:
	int	m_prm;
};

#endif // TPARAMS_H

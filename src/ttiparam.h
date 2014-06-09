
//OpenSCADA system file: ttiparam.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include "tvalue.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TTipParam                                     *
//*************************************************
class TController;
class TParamContr;

class TTipParam : public TElem
{
    public:
	//Methods
	TTipParam( const char *iid, const char *iname, const char *idb = "", bool i_isPrmCntr = false );

	virtual void create( TParamContr *prm )			{ }
	virtual void destroy( TParamContr *prm )		{ }

	virtual string DB( TController *cntr );
	void setDB( TController *cntr, const string &vl );

	virtual void enable( TParamContr *prm )			{ }
	virtual void disable( TParamContr *prm )		{ }

	virtual bool cfgChange( TParamContr *prm, TCfg &cfg )	{ return true; }

	virtual void vlGet( TParamContr *prm, TVal &vo )	{ }
	virtual void vlSet( TParamContr *prm, TVal &vo, const TVariant &vl, const TVariant &pvl )	{ }
        virtual void vlArchMake( TParamContr *prm, TVal &val )	{ }
	virtual void getVals( TParamContr *prm )		{ }

	virtual bool cntrCmdProc( TParamContr *prm, XMLNode *opt )	{ return false; }

	//Attributes
	string	name;
	string	descr;
	string	mDB;
	bool	isPrmCntr;	//The parameter type also container for other parameters
};

}

#endif // TTIPARAM_H

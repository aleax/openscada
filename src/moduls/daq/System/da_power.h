
//OpenSCADA module DAQ.System file: da_power.h
/***************************************************************************
 *   Copyright (C) 2023 by Roman Savochenko, <roman@oscada.org>            *
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

#ifndef DA_POWER_H
#define DA_POWER_H

#include "da.h"

#define DIR_PS "/sys/class/power_supply/"

namespace SystemCntr
{

//*************************************************
//* Power                                        *
//*************************************************
class Power: public DA
{
    public:
	//Methods
	Power( )	{ }
	~Power( )	{ }

	bool isDynamic( ) { return true; }

	string id( )	{ return "Power"; }
	string name( )	{ return _("Power"); }

	void getVal( TMdPrm *prm );
	void vlGet( TMdPrm *prm, TVal &val );
	void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	void dList( vector<string> &list, TMdPrm *prm = NULL );
	void makeActiveDA( TMdContr *aCntr, const string &dIdPref = "", const string &dNmPref = "" )
	{ DA::makeActiveDA(aCntr, id(), name()); }

    private:
	//Methods
	bool devChkAccess( const string &dev, const string &file, const string &acs = "r" );
	string devRead( const string &dev, const string &file );
};

} //End namespace

#endif //DA_POWER_H

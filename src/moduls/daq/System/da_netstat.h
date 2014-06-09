
//OpenSCADA system module DAQ.System file: da_netstat.h
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
 
#ifndef DA_NETSTAT_H
#define DA_NETSTAT_H

#include "da.h"

namespace SystemCntr
{

//*************************************************
//* NetStat                                       *
//*************************************************
class NetStat: public DA
{
    public:
	//Methods
	NetStat( );
	~NetStat( );

	string id( )	{ return "netstat"; }
	string name( )	{ return _("Net statistic"); }

	void init( TMdPrm *prm );
	void getVal( TMdPrm *prm );

	void makeActiveDA( TMdContr *a_cntr );

    private:
	//Attributes
	void dList( vector<string> &list, bool part = false );
};

} //End namespace

#endif //DA_NETSTAT_H



//OpenSCADA system module DAQ.ICP_DAS file: da_87x.h
/***************************************************************************
 *   Copyright (C) 2012 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef DA_87X_H
#define DA_87X_H

#include "da.h"

namespace ICP_DAS_DAQ
{

//*************************************************
//* da_87x                                         *
//*************************************************
class da_87x: public DA
{
    public:
	//Methods
	da_87x( );
	~da_87x( );

	string id( )	{ return "87x"; }
	string name( )	{ return _("I-8700, I-7000 serial bus"); }

	void tpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl = NULL );

	void enable( TMdPrm *prm, vector<string> &als );
	void disable( TMdPrm *prm );

	void getVal( TMdPrm *prm );
	void vlSet( TMdPrm *prm, TVal &valo, const TVariant &pvl );

	bool cntrCmdProc( TMdPrm *prm, XMLNode *opt );
};

} //End namespace

#endif //DA_87X_H

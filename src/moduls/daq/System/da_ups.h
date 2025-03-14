
//OpenSCADA module DAQ.System file: da_ups.h
/***************************************************************************
 *   Copyright (C) 2014-2024 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef DA_UPS_H
#define DA_UPS_H

#include "da.h"

namespace SystemCntr
{

//*************************************************
//* NUT                                           *
//*************************************************
class UPS: public DA
{
    public:
	//Methods
	UPS( );
	~UPS( );

	bool isDynamic( ) { return true; }
	bool isSlow( )	{ return true; }

	string id( )	{ return "ups"; }
	string name( )	{ return _("UPS"); }

	void init( TMdPrm *prm, bool update = false );
	void deInit( TMdPrm *prm );

	bool cntrCmdProc( TMdPrm *prm, XMLNode *opt );
	void cfgChange( TMdPrm *prm, TCfg &co, const TVariant &pc );

	void getVal( TMdPrm *prm );
	void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	void dList( vector<string> &list, TMdPrm *prm = NULL );
	void makeActiveDA( TMdContr *aCntr, const string &dIdPref = "", const string &dNmPref = "" )
	{ DA::makeActiveDA(aCntr, "UPS", name()); }

    private:
	//Methods
	string reqUPS( const string &addr, const string &req, const string &debCat = "" );
	string upsList( const string &addr );

	//Attributes
	string	tTr, nTr;
	ResMtx	reqRes;
};

} //End namespace

#endif //DA_UPS_H

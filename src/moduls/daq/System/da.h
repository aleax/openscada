
//OpenSCADA module DAQ.System file: da.h
/***************************************************************************
 *   Copyright (C) 2005-2024 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef DA_H
#define DA_H

#include <tcontroller.h>

#define SEP_SUBID "|"

using namespace OSCADA;

namespace SystemCntr
{

//*************************************************
//* DA                                            *
//*************************************************
class TMdPrm;
class TMdContr;

class DA: public TElem
{
    public:
	//Methods
	DA( ) : TElem("da_el")	{ }
	virtual ~DA( )		{ }

	virtual string id( ) = 0;
	virtual string name( ) = 0;

	virtual bool isDynamic( )	{ return false; }	//the source is dynamic,
								//  that is the attributes list is different for different subtypes
	virtual bool isSlow( )		{ return false; }	//the source is slow
	virtual bool hasSubTypes( )	{ return true; }	//the source has subtypes

	virtual void updGen( bool cntr = false )	{ }	//the generic updating call in whether the Controller Cycle for <cntr>
								//  or in System one

	virtual void dList( vector<string> &list, TMdPrm *prm = NULL )	{ }
	virtual void makeActiveDA( TMdContr *aCntr, const string &dIdPref = "", const string &dNmPref = "" );

	virtual void init( TMdPrm *prm, bool update = false );
	virtual void deInit( TMdPrm *prm );

	virtual void getVal( TMdPrm *prm )	{ }
	virtual void vlGet( TMdPrm *prm, TVal &val )	{ }
	virtual void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl )	{ }

	virtual bool cntrCmdProc( TMdPrm *prm, XMLNode *opt )	{ return false; }
	virtual void cfgChange( TMdPrm *prm, TCfg &co, const TVariant &pc )	{ }
};

} //End namespace

#endif //DA_H

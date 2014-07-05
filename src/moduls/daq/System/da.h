
//OpenSCADA system module DAQ.System file: da.h
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

#ifndef DA_H
#define DA_H

#include <tcontroller.h>

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

	virtual void init( TMdPrm *prm ) 	{ }
	virtual void deInit( TMdPrm *prm )	{ }

	virtual bool cntrCmdProc( TMdPrm *prm, XMLNode *opt )	{ return false; }
	virtual void cfgChange( TCfg &co, const TVariant &pc )	{ }

	virtual void getVal( TMdPrm *prm )	{ }
	virtual void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl )	{ }

	virtual void makeActiveDA( TMdContr *a_cntr )	{ }
};

} //End namespace

#endif //DA_H


//OpenSCADA system module DAQ.ICP_DAS file: da.h
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko                           *
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

#ifndef DA_H
#define DA_H

using namespace OSCADA;

namespace ICP_DAS_DAQ
{

//*************************************************
//* DA                                            *
//*************************************************
class TMdPrm;

class DA
{
    public:
	//Methods
	DA( )			{ }
	virtual ~DA( )		{ }

	virtual string id( ) = 0;
	virtual string name( ) = 0;

	virtual void tpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl = NULL ) = 0;

	virtual void enable( TMdPrm *prm, vector<string> &als )	{ }
	virtual void disable( TMdPrm *prm )			{ }

	virtual void getVal( TMdPrm *prm )			{ }
	virtual void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl )	{ }

	virtual bool cntrCmdProc( TMdPrm *prm, XMLNode *opt )	{ return false; }
};

} //End namespace

#endif //DA_H

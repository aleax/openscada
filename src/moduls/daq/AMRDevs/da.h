
//OpenSCADA system module DAQ.AMRDevs file: da.h
/***************************************************************************
 *   Copyright (C) 2010-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

namespace AMRDevs
{

//*************************************************
//* DA                                            *
//*************************************************
class TMdPrm;
class TMdContr;

class DA
{
    public:
	//Methods
	DA( TMdPrm *prm ) : mPrm(prm)	{ }
	virtual ~DA( )			{ }

	virtual void getVals( )		{ }

	virtual bool cntrCmdProc( XMLNode *opt )	{ return false; }

    protected:
	//Attributes
	TMdPrm *mPrm;
};

} //End namespace

#endif //DA_H

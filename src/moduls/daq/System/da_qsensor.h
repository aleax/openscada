
//OpenSCADA system module DAQ.System file: da_qsensor.h
/***************************************************************************
 *   Copyright (C) 2018 by Roman Savochenko, <rom_as@oscada.org>           *
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

#ifndef DA_SENSOR_H
#define DA_SENSOR_H

#include "da.h"

namespace SystemCntr
{

//*************************************************
//* QSensor                                        *
//*************************************************
class QSensor: public DA
{
    public:
	//Methods
	QSensor( );
	~QSensor( );

	string id( )	{ return "QSensor"; }
	string name( )	{ return _("Qt sensors"); }

	void init( TMdPrm *prm );
	void deInit( TMdPrm *prm );
	void getVal( TMdPrm *prm );

	void makeActiveDA( TMdContr *aCntr );

    protected:
	//Methods
	TVariant getSensors( TMdContr &cntr );

    private:
	//Attributes
	TVariant	sens;
};

} //End namespace

#endif //DA_SENSOR_H

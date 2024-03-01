
//OpenSCADA module DAQ.System file: da_qsensor.cpp
/***************************************************************************
 *   Copyright (C) 2018-2023 by Roman Savochenko, <roman@oscada.org>       *
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

#include <map>

#include <tsys.h>

#include "os_contr.h"
#include "da_qsensor.h"

using namespace SystemCntr;
using std::map;

//*************************************************
//* QSensor                                        *
//*************************************************
QSensor::QSensor( )	{ }

QSensor::~QSensor( )	{ }

void QSensor::getVal( TMdPrm *prm )
{
    TVariant sens = getSensors();
    if(sens.type() != TVariant::Object)	return;

    TVariant s;
    bool findOK = false;
    for(int iS = 0; iS < sens.getO().at().propGet("length").getI() && !findOK; iS++)
	findOK = (s=sens.getO().at().propGet(i2s(iS))).getO().at().propGet("type").getS() == prm->cfg("SUBT").getS();
    if(!findOK || s.type() != TVariant::Object)	return;

    if(!prm->vlPresent("st"))
	((TElem*)prm->daData)->fldAdd(new TFld("st",trS("State"),TFld::String,TFld::NoWrite));
    prm->vlAt("st").at().setS(TSYS::strMess(_("Connected=%d; Active=%d; Busy=%d; Error=%d"),
			    (int)s.getO().at().propGet("connected").getI(),
			    (int)s.getO().at().propGet("active").getI(),
			    (int)s.getO().at().propGet("busy").getI(),
			    (int)s.getO().at().propGet("error").getI()), 0, true);

    for(int iP = 0; iP < s.getO().at().propGet("length").getI(); ++iP) {
	TVariant v = s.getO().at().propGet(i2s(iP));
	if(!prm->vlPresent("pr"+i2s(iP))) {
	    TFld::Type tp;
	    switch(v.type()) {
		case TVariant::Boolean:	tp = TFld::Boolean;	break;
		case TVariant::Integer:	tp = TFld::Integer;	break;
		case TVariant::Real:	tp = TFld::Real;	break;
		default: tp = TFld::String;
	    }
	    ((TElem*)prm->daData)->fldAdd(new TFld(("pr"+i2s(iP)).c_str(),(_("Property ")+i2s(iP)).c_str(),tp,TFld::NoWrite));
	}
	prm->vlAt("pr"+i2s(iP)).at().setS(v.getS(), 0, true);
    }
}

void QSensor::dList( vector<string> &list, TMdPrm *prm )
{
    TVariant sens = getSensors();
    if(sens.type() != TVariant::Object)	return;

    for(int iP = 0; iP < sens.getO().at().propGet("length").getI(); ++iP)
	list.push_back(sens.getO().at().propGet(i2s(iP)).getO().at().propGet("type").getS());
}

TVariant QSensor::getSensors( )
{
    if(sens.type() == TVariant::Object)	return sens;

    AutoHD<TCntrNode> QTStarter = SYS->nodeAt("/UI/QTStarter", 0, 0, 0, true);
    if(QTStarter.freeStat())	sens = TVariant();
    else {
	vector<TVariant> prms;
	sens = QTStarter.at().objFuncCall("sensors", prms, "root");
    }

    return sens;
}

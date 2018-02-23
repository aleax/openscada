
//OpenSCADA system module DAQ.System file: da_qsensor.cpp
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

#include <map>

#include <tsys.h>

#include "os_contr.h"
#include "da_qsensor.h"

using namespace SystemCntr;
using std::map;

//*************************************************
//* QSensor                                        *
//*************************************************
QSensor::QSensor( )
{

}

QSensor::~QSensor( )
{

}

void QSensor::init( TMdPrm *prm )
{
    prm->daData = new TElem();
    prm->vlElemAtt((TElem*)prm->daData);

    //Create config
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setDescr("");

    TVariant sens = getSensors(prm->owner());
    string ifls;
    for(int iP = 0; iP < sens.getO().at().propGet("length").getI(); ++iP)
	ifls += sens.getO().at().propGet(i2s(iP)).getO().at().propGet("type").getS()+";";
    c_subt.fld().setValues(ifls);
    c_subt.fld().setSelNames(ifls);
    if(ifls.size() && !TRegExp("(^|;)"+c_subt.getS()+";").test(ifls)) c_subt.setS(TSYS::strParse(ifls,0,";"));
}

void QSensor::deInit( TMdPrm *prm )
{
    prm->vlElemDet((TElem*)prm->daData);
    delete (TElem*)prm->daData;
    prm->daData = NULL;
}

void QSensor::getVal( TMdPrm *prm )
{
    TVariant sens = getSensors(prm->owner());
    if(sens.type() != TVariant::Object)	return;

    TVariant s;
    bool findOK = false;
    for(int iS = 0; iS < sens.getO().at().propGet("length").getI() && !findOK; iS++)
	findOK = (s=sens.getO().at().propGet(i2s(iS))).getO().at().propGet("type").getS() == prm->cfg("SUBT").getS();
    if(!findOK || s.type() != TVariant::Object)	return;

    if(!prm->vlPresent("st"))
	((TElem*)prm->daData)->fldAdd(new TFld("st",_("State"),TFld::String,TFld::NoWrite));
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

TVariant QSensor::getSensors( TMdContr &cntr )
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

void QSensor::makeActiveDA( TMdContr *aCntr )
{
    TVariant sens = getSensors(*aCntr);
    if(sens.type() != TVariant::Object)	return;

    map<string, int>	tSens;
    for(int iS = 0; iS < sens.getO().at().propGet("length").getI(); iS++)
	tSens[sens.getO().at().propGet(i2s(iS)).getO().at().propGet("type").getS()] = iS;

    vector<string> pLs;
    aCntr->list(pLs);
    for(unsigned iP = 0; iP < pLs.size(); iP++) {
	AutoHD<TMdPrm> p = aCntr->at(pLs[iP]);
	if(p.at().cfg("TYPE").getS() == id() && tSens.find(p.at().cfg("SUBT").getS()) != tSens.end())
	    tSens.erase(p.at().cfg("SUBT").getS());
    }

    for(map<string,int>::iterator iS = tSens.begin(); iS != tSens.end(); ++iS) {
	aCntr->add(iS->first, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(iS->first);
	dprm.at().setName("QSensor: "+sens.getO().at().propGet(i2s(iS->second)).getO().at().propGet("identifier").getS());
	dprm.at().setDescr(sens.getO().at().propGet(i2s(iS->second)).getO().at().propGet("description").getS());
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS(iS->first);
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}

/***************************************************************************
 *   Copyright (C) 2011-2015 by Maxim Kochetkov                            *
 *   fido_max@inbox.ru                                                     *
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
#ifndef DA_ACCOUNT_H
#define DA_ACCOUNT_H

#include "da.h"

namespace FT3
{
    class B_ACCOUNT: public DA
    {
    public:
	//Methods
	B_ACCOUNT(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~B_ACCOUNT();
	uint16_t ID;
	uint16_t count_n;
	bool with_params;
	void AddChannel(uint8_t iid);
	void saveIO(void);
	void loadIO(bool force = false);
	void tmHandler(void);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	class SACchannel
	{
	public:
	    SACchannel(uint8_t iid, DA* owner) :
		    da(owner),
		    id(iid),
		    State(TSYS::strMess("state_%d", id + 1), TSYS::strMess(_("State %d"), id + 1)),
		    Value(TSYS::strMess("value_%d", id + 1), TSYS::strMess(_("Value %d"), id + 1)),
		    Period(TSYS::strMess("period_%d", id + 1), TSYS::strMess(_("Measure period %d"), id + 1)),
		    Sens(TSYS::strMess("sens_%d", id + 1), TSYS::strMess(_("Sensitivity %d"), id + 1)),
		    MinW(TSYS::strMess("minW_%d", id + 1), TSYS::strMess(_("Warning minimum %d"), id + 1)),
		    MaxW(TSYS::strMess("maxW_%d", id + 1), TSYS::strMess(_("Warning maximum %d"), id + 1)),
		    MinA(TSYS::strMess("minA_%d", id + 1), TSYS::strMess(_("Alarm minimum %d"), id + 1)),
		    MaxA(TSYS::strMess("maxA_%d", id + 1), TSYS::strMess(_("Alarm maximum %d"), id + 1)),
		    Sensors(TSYS::strMess("sensors_%d", id + 1), TSYS::strMess(_("Sensors ID %d"), id + 1)),
		    ServiceQ(TSYS::strMess("serviceQ_%d", id + 1), TSYS::strMess(_("Service flow %d"), id + 1)),
		    Hour(TSYS::strMess("hour_%d", id + 1), TSYS::strMess(_("Contract hour %d"), id + 1)),
		    HourlyQ(TSYS::strMess("hourlyQ_%d", id + 1), TSYS::strMess(_("Hourly flow %d"), id + 1)),
		    Counter(TSYS::strMess("counter_%d", id + 1), TSYS::strMess(_("Counter Q %d"), id + 1)),
		    HourQ(TSYS::strMess("hourQ_%d", id + 1), TSYS::strMess(_("Hour Q %d"), id + 1)),
		    HourdP(TSYS::strMess("hourdP_%d", id + 1), TSYS::strMess(_("Hour dP %d"), id + 1)),
		    HourT(TSYS::strMess("hourT_%d", id + 1), TSYS::strMess(_("Hour T %d"), id + 1)),
		    HourP(TSYS::strMess("hourP_%d", id + 1), TSYS::strMess(_("Hour P %d"), id + 1)),
		    HourE(TSYS::strMess("hourE_%d", id + 1), TSYS::strMess(_("Hour E %d"), id + 1)),
		    StartDate(TSYS::strMess("startDate_%d", id + 1), TSYS::strMess(_("StartDate %d"), id + 1)),
		    EndDate(TSYS::strMess("endDate_%d", id + 1), TSYS::strMess(_("EndDate %d"), id + 1)),
		    AvgQ(TSYS::strMess("avgQ_%d", id + 1), TSYS::strMess(_("Avg Q %d"), id + 1)),
		    AvgdP(TSYS::strMess("avgdP_%d", id + 1), TSYS::strMess(_("Avg dP %d"), id + 1)),
		    AvgT(TSYS::strMess("avgT_%d", id + 1), TSYS::strMess(_("Avg T %d"), id + 1)),
		    AvgP(TSYS::strMess("avgP_%d", id + 1), TSYS::strMess(_("Avg P %d"), id + 1)),
		    AvgE(TSYS::strMess("avgE_%d", id + 1), TSYS::strMess(_("Avg E %d"), id + 1)),
		    PeriodQ(TSYS::strMess("periodQ_%d", id + 1), TSYS::strMess(_("Periodically Q %d"), id + 1)),
		    Density(TSYS::strMess("density_%d", id + 1), TSYS::strMess(_("Density %d"), id + 1)),
		    Asperity(TSYS::strMess("asperity_%d", id + 1), TSYS::strMess(_("Equivalence asperity %d"), id + 1)),
		    ConcentrN(TSYS::strMess("concentrN_%d", id + 1), TSYS::strMess(_("N concentration %d"), id + 1)),
		    ConcentrCO(TSYS::strMess("concentrCO_%d", id + 1), TSYS::strMess(_("CO concentration %d"), id + 1)),
		    DiameterM(TSYS::strMess("diameterM_%d", id + 1), TSYS::strMess(_("Membrane diameter %d"), id + 1)),
		    FactorM(TSYS::strMess("factorM_%d", id + 1), TSYS::strMess(_("Membrane factor %d"), id + 1)),
		    DiameterP(TSYS::strMess("diameterP_%d", id + 1), TSYS::strMess(_("Pipe diameter %d"), id + 1)),
		    FactorP(TSYS::strMess("factorP_%d", id + 1), TSYS::strMess(_("Pipe factor %d"), id + 1)),
		    MethodM(TSYS::strMess("methodM_%d", id + 1), TSYS::strMess(_("Measure method %d"), id + 1)),
		    TestdP(TSYS::strMess("testdP_%d", id + 1), TSYS::strMess(_("Test dP/TI %d"), id + 1)),
		    TestT(TSYS::strMess("testT_%d", id + 1), TSYS::strMess(_("Test T %d"), id + 1)),
		    TestP(TSYS::strMess("testP_%d", id + 1), TSYS::strMess(_("Test P %d"), id + 1)),
		    TestQ(TSYS::strMess("testQ_%d", id + 1), TSYS::strMess(_("Test Q %d"), id + 1)),
		    RadiusM(TSYS::strMess("radiusM_%d", id + 1), TSYS::strMess(_("Membrane radius %d"), id + 1)),
		    PressureA(TSYS::strMess("pressureA_%d", id + 1), TSYS::strMess(_("Atmospheric pressure %d"), id + 1)),
		    dP(TSYS::strMess("dP_%d", id + 1), TSYS::strMess(_("dP %d"), id + 1)),
		    T(TSYS::strMess("T_%d", id + 1), TSYS::strMess(_("T %d"), id + 1)),
		    P(TSYS::strMess("P_%d", id + 1), TSYS::strMess(_("P %d"), id + 1)),
		    E(TSYS::strMess("E_%d", id + 1), TSYS::strMess(_("E %d"), id + 1))
	    {
	    }
	    DA* da;
	    uint8_t id;

	    ui8Data State, Period, Hour, MethodM;
	    ui32Data StartDate, EndDate, Sensors;

	    flData Value, Sens, MinW, MaxW, MinA, MaxA,  ServiceQ, HourlyQ, Counter, HourQ, HourdP, HourT, HourP, HourE, AvgQ, AvgdP, AvgT, AvgP, AvgE,
		    PeriodQ, Density, Asperity, ConcentrN, ConcentrCO, DiameterM, FactorM, DiameterP, FactorP, TestdP, TestT, TestP, TestQ, RadiusM, PressureA,
		    dP, T, P, E;
	    uint8_t SetNewPeriod(uint8_t addr, uint16_t prmID, uint8_t *val);
	};
	vector<SACchannel> data;
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 43;
	    } else {
		return data.size() * 2;
	    }
	}
	int lnkId(const string &id)
	{
	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) return i_l * 43;
		    if(data[i_l].Value.lnk.prmName == id) return i_l * 43 + 1;
		    if(data[i_l].Period.lnk.prmName == id) return i_l * 43 + 2;
		    if(data[i_l].Sens.lnk.prmName == id) return i_l * 43 + 3;
		    if(data[i_l].MinW.lnk.prmName == id) return i_l * 43 + 4;
		    if(data[i_l].MaxW.lnk.prmName == id) return i_l * 43 + 5;
		    if(data[i_l].MinA.lnk.prmName == id) return i_l * 43 + 6;
		    if(data[i_l].MaxA.lnk.prmName == id) return i_l * 43 + 7;
		    if(data[i_l].Sensors.lnk.prmName == id) return i_l * 43 + 8;
		    if(data[i_l].ServiceQ.lnk.prmName == id) return i_l * 43 + 9;
		    if(data[i_l].Hour.lnk.prmName == id) return i_l * 43 + 10;
		    if(data[i_l].HourlyQ.lnk.prmName == id) return i_l * 43 + 11;
		    if(data[i_l].Counter.lnk.prmName == id) return i_l * 43 + 12;
		    if(data[i_l].HourQ.lnk.prmName == id) return i_l * 43 + 13;
		    if(data[i_l].HourdP.lnk.prmName == id) return i_l * 43 + 14;
		    if(data[i_l].HourT.lnk.prmName == id) return i_l * 43 + 15;
		    if(data[i_l].HourP.lnk.prmName == id) return i_l * 43 + 16;
		    if(data[i_l].HourE.lnk.prmName == id) return i_l * 43 + 17;
		    if(data[i_l].AvgQ.lnk.prmName == id) return i_l * 43 + 18;
		    if(data[i_l].AvgdP.lnk.prmName == id) return i_l * 43 + 19;
		    if(data[i_l].AvgT.lnk.prmName == id) return i_l * 43 + 20;
		    if(data[i_l].AvgP.lnk.prmName == id) return i_l * 43 + 21;
		    if(data[i_l].AvgE.lnk.prmName == id) return i_l * 43 + 22;
		    if(data[i_l].PeriodQ.lnk.prmName == id) return i_l * 43 + 23;
		    if(data[i_l].Density.lnk.prmName == id) return i_l * 43 + 24;
		    if(data[i_l].Asperity.lnk.prmName == id) return i_l * 43 + 25;
		    if(data[i_l].ConcentrN.lnk.prmName == id) return i_l * 43 + 26;
		    if(data[i_l].ConcentrCO.lnk.prmName == id) return i_l * 43 + 27;
		    if(data[i_l].DiameterM.lnk.prmName == id) return i_l * 43 + 28;
		    if(data[i_l].FactorM.lnk.prmName == id) return i_l * 43 + 29;
		    if(data[i_l].DiameterP.lnk.prmName == id) return i_l * 43 + 30;
		    if(data[i_l].FactorP.lnk.prmName == id) return i_l * 43 + 31;
		    if(data[i_l].MethodM.lnk.prmName == id) return i_l * 43 + 32;
		    if(data[i_l].TestdP.lnk.prmName == id) return i_l * 43 + 33;
		    if(data[i_l].TestT.lnk.prmName == id) return i_l * 43 + 34;
		    if(data[i_l].TestP.lnk.prmName == id) return i_l * 43 + 35;
		    if(data[i_l].TestQ.lnk.prmName == id) return i_l * 43 + 36;
		    if(data[i_l].RadiusM.lnk.prmName == id) return i_l * 43 + 37;
		    if(data[i_l].PressureA.lnk.prmName == id) return i_l * 43 + 38;
		    if(data[i_l].dP.lnk.prmName == id) return i_l * 43 + 39;
		    if(data[i_l].T.lnk.prmName == id) return i_l * 43 + 40;
		    if(data[i_l].P.lnk.prmName == id) return i_l * 43 + 41;
		    if(data[i_l].E.lnk.prmName == id) return i_l * 43 + 42;
		}
	    } else {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) return i_l * 2;
		    if(data[i_l].Value.lnk.prmName == id) return i_l * 2 + 1;
		}
	    }
	    return -1;
	}
	SLnk &lnk(int num)
	{
	    int k;
	    if(with_params) {
		k = 43;
	    } else {
		k = 2;
	    }
	    switch(num % k) {
	    case 0:
		return data[num / k].State.lnk;
	    case 1:
		return data[num / k].Value.lnk;
	    case 2:
		return data[num / k].Period.lnk;
	    case 3:
		return data[num / k].Sens.lnk;
	    case 4:
		return data[num / k].MinW.lnk;
	    case 5:
		return data[num / k].MaxW.lnk;
	    case 6:
		return data[num / k].MinA.lnk;
	    case 7:
		return data[num / k].MaxA.lnk;
	    case 8:
		return data[num / k].Sensors.lnk;
	    case 9:
		return data[num / k].ServiceQ.lnk;
	    case 10:
		return data[num / k].Hour.lnk;
	    case 11:
		return data[num / k].HourlyQ.lnk;
	    case 12:
		return data[num / k].Counter.lnk;
	    case 13:
		return data[num / k].HourQ.lnk;
	    case 14:
		return data[num / k].HourdP.lnk;
	    case 15:
		return data[num / k].HourT.lnk;
	    case 16:
		return data[num / k].HourP.lnk;
	    case 17:
		return data[num / k].HourE.lnk;
	    case 18:
		return data[num / k].AvgQ.lnk;
	    case 19:
		return data[num / k].AvgdP.lnk;
	    case 20:
		return data[num / k].AvgT.lnk;
	    case 21:
		return data[num / k].AvgP.lnk;
	    case 22:
		return data[num / k].AvgE.lnk;
	    case 23:
		return data[num / k].PeriodQ.lnk;
	    case 24:
		return data[num / k].Density.lnk;
	    case 25:
		return data[num / k].Asperity.lnk;
	    case 26:
		return data[num / k].ConcentrN.lnk;
	    case 27:
		return data[num / k].ConcentrCO.lnk;
	    case 28:
		return data[num / k].DiameterM.lnk;
	    case 29:
		return data[num / k].FactorM.lnk;
	    case 30:
		return data[num / k].DiameterP.lnk;
	    case 31:
		return data[num / k].FactorP.lnk;
	    case 32:
		return data[num / k].MethodM.lnk;
	    case 33:
		return data[num / k].TestdP.lnk;
	    case 34:
		return data[num / k].TestT.lnk;
	    case 35:
		return data[num / k].TestP.lnk;
	    case 36:
		return data[num / k].TestQ.lnk;
	    case 37:
		return data[num / k].RadiusM.lnk;
	    case 38:
		return data[num / k].PressureA.lnk;
	    case 39:
		return data[num / k].dP.lnk;
	    case 40:
		return data[num / k].T.lnk;
	    case 41:
		return data[num / k].P.lnk;
	    case 42:
		return data[num / k].E.lnk;

	    }
	}
    };

} //End namespace

#endif //DA_ACCOUNT_H

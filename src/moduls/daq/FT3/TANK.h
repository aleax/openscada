/***************************************************************************
 *   Copyright (C) 2011-2016 by Maxim Kochetkov                            *
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
#ifndef DA_TANK_H
#define DA_TANK_H

#include "da.h"

namespace FT3
{

    class KA_TANK: public DA
    {
    public:
	//Methods
	KA_TANK(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~KA_TANK();
	uint16_t ID;
	uint16_t count_n;
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void saveIO(void);
	void loadIO(bool force = false);
	void tmHandler(void);
 	uint16_t config;
	class SKATANKchannel
	{
	public:
	    SKATANKchannel(uint8_t iid, DA* owner) : da(owner),
		    id(iid),
		    State(TSYS::strMess("state_%d", id + 1), TSYS::strMess(_("State %d"), id + 1)),
		    Function(TSYS::strMess("function_%d", id + 1), TSYS::strMess(_("Function %d"), id + 1)),
		    TCSens1(TSYS::strMess("TCSens1_%d", id + 1), TSYS::strMess(_("Sens1 TC %d"), id + 1)),
		    TCSens1Not(TSYS::strMess("TCSens1Not_%d", id + 1), TSYS::strMess(_("Sens1 TC Not %d"), id + 1)),
		    TCSens2(TSYS::strMess("TCSens2_%d", id + 1), TSYS::strMess(_("Sens2 TC %d"), id + 1)),
		    TCSens2Not(TSYS::strMess("TCSens2Not_%d", id + 1), TSYS::strMess(_("Sens2 TC Not %d"), id + 1)),
		    TULight(TSYS::strMess("TULight_%d", id + 1), TSYS::strMess(_("TU Light Alarm %d"), id + 1)),
		    TUSound(TSYS::strMess("TUSound_%d", id + 1), TSYS::strMess(_("TU Sound Alarm %d"), id + 1)),
		    TURlOff(TSYS::strMess("TURlOff_%d", id + 1), TSYS::strMess(_("TU Relay Off %d"), id + 1)),
		    TimeDelay(TSYS::strMess("TimeDelay_%d", id + 1), TSYS::strMess(_("Delay time %d"), id + 1)),
		    TimeFalseAlarm(TSYS::strMess("TimeFalseAlarm_%d", id + 1), TSYS::strMess(_("False alarm time %d"), id + 1)),
		    StateTCSens1(TSYS::strMess("StateTCSens1_%d", id + 1), TSYS::strMess(_("State Sens1 TC %d"), id + 1)),
		    StateTCSens1Not(TSYS::strMess("StateTCSens1Not_%d", id + 1), TSYS::strMess(_("State Sens1 TC Not %d"), id + 1)),
		    StateTCSens2(TSYS::strMess("StateTCSens2_%d", id + 1), TSYS::strMess(_("State Sens2 TC %d"), id + 1)),
		    StateTCSens2Not(TSYS::strMess("StateTCSens2Not_%d", id + 1), TSYS::strMess(_("State Sens2 TC Not %d"), id + 1))
	    {
	    }
	    DA* da;
	    uint8_t id;

	    ui8Data State, Function;
	    ui16Data TULight, TUSound, TURlOff, StateTCSens1, StateTCSens1Not;
	    ui16Data TimeDelay, TimeFalseAlarm, StateTCSens2, StateTCSens2Not;
	    ui16Data TCSens1, TCSens1Not, TCSens2, TCSens2Not;

	    void UpdateParam(uint16_t ID, uint8_t cl);
	    uint8_t SetNewParam(uint8_t addr, uint16_t prmID, uint8_t *val);
	    uint8_t SetNewState(uint8_t addr, uint16_t prmID, uint8_t *val);
	    uint8_t SetNewFunction(uint8_t addr, uint16_t prmID, uint8_t *val);
	};
	vector<SKATANKchannel> data;
	void AddTANKchannel(uint8_t iid);
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 15;
	    } else {
		return data.size() * 2;
	    }
	}
	int lnkId(const string &id)
	{

	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) return i_l * 15;
		    if(data[i_l].Function.lnk.prmName == id) return i_l * 15 + 1;
		    if(data[i_l].TCSens1.lnk.prmName == id) return i_l * 15 + 2;
		    if(data[i_l].TCSens1Not.lnk.prmName == id) return i_l * 15 + 3;
		    if(data[i_l].TCSens2.lnk.prmName == id) return i_l * 15 + 4;
		    if(data[i_l].TCSens2Not.lnk.prmName == id) return i_l * 15 + 5;
		    if(data[i_l].TULight.lnk.prmName == id) return i_l * 15 + 6;
		    if(data[i_l].TUSound.lnk.prmName == id) return i_l * 15 + 7;
		    if(data[i_l].TURlOff.lnk.prmName == id) return i_l * 15 + 8;
		    if(data[i_l].TimeDelay.lnk.prmName == id) return i_l * 15 + 9;
		    if(data[i_l].TimeFalseAlarm.lnk.prmName == id) return i_l * 15 + 10;
		    if(data[i_l].StateTCSens1.lnk.prmName == id) return i_l * 15 + 11;
		    if(data[i_l].StateTCSens1Not.lnk.prmName == id) return i_l * 15 + 12;
		    if(data[i_l].StateTCSens2.lnk.prmName == id) return i_l * 15 + 13;
		    if(data[i_l].StateTCSens2Not.lnk.prmName == id) return i_l * 15 + 14;
		}
	    } else {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) {
			return i_l * 2;
		    }
		    if(data[i_l].Function.lnk.prmName == id) {
			return i_l * 2 + 1;
		    }
		}
	    }
	    return -1;
	}
	SLnk &lnk(int num)
	{
	    int k;
	    if(with_params) {
		k = 15;
	    } else {
		k = 2;
	    }
	    switch(num % k) {
	    case 0:
		return data[num / k].State.lnk;
	    case 1:
		return data[num / k].Function.lnk;
	    case 2:
		return data[num / k].TCSens1.lnk;
	    case 3:
		return data[num / k].TCSens1Not.lnk;
	    case 4:
		return data[num / k].TCSens2.lnk;
	    case 5:
		return data[num / k].TCSens2Not.lnk;
	    case 6:
		return data[num / k].TULight.lnk;
	    case 7:
		return data[num / k].TUSound.lnk;
	    case 8:
		return data[num / k].TURlOff.lnk;
	    case 9:
		return data[num / k].TimeDelay.lnk;
	    case 10:
		return data[num / k].TimeFalseAlarm.lnk;
	    case 11:
		return data[num / k].StateTCSens1.lnk;
	    case 12:
		return data[num / k].StateTCSens1Not.lnk;
	    case 13:
		return data[num / k].StateTCSens2.lnk;
	    case 14:
		return data[num / k].StateTCSens2Not.lnk;
	    }
	}

    private:
	bool with_params;
	vector<SDataRec> chan_err;

    };

} //End namespace

#endif //DA_TANK_H

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
#ifndef DA_GNS_H
#define DA_GNS_H

#include "da.h"

namespace FT3
{
    class KA_GNS: public DA
    {
    public:
	//Methods
	KA_GNS(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~KA_GNS();
	uint16_t ID;
	uint16_t count_n;
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void saveIO(void);
	void loadIO(bool force = false);
	void tmHandler(void);
 	uint16_t config;
	class SKANSchannel
	{
	public:
	    SKANSchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), State(TSYS::strMess("state_%d", id + 1).c_str(), TSYS::strMess(_("State %d"), id + 1).c_str()),
		    Function(TSYS::strMess("function_%d", id + 1).c_str(), TSYS::strMess(_("Function %d"), id + 1).c_str()),
		    TUOn(TSYS::strMess("TUOn_%d", id + 1).c_str(), TSYS::strMess(_("TU on %d"), id + 1).c_str()),
		    TUOff(TSYS::strMess("TUOff_%d", id + 1).c_str(), TSYS::strMess(_("TU off %d"), id + 1).c_str()),
		    TUStop(TSYS::strMess("TUstop_%d", id + 1).c_str(), TSYS::strMess(_("TU stop %d"), id + 1).c_str()),
		    TURemote(TSYS::strMess("TUremote_%d", id + 1).c_str(), TSYS::strMess(_("TU remote %d"), id + 1).c_str()),
		    TUManual(TSYS::strMess("TUmanual_%d", id + 1).c_str(), TSYS::strMess(_("TU manual %d"), id + 1).c_str()),
		    TimeOn(TSYS::strMess("TimeOn_%d", id + 1).c_str(), TSYS::strMess(_("On time %d"), id + 1).c_str()),
		    TimeOff(TSYS::strMess("TimeOff_%d", id + 1).c_str(), TSYS::strMess(_("Off time %d"), id + 1).c_str()),
		    TimeStop(TSYS::strMess("timeStop_%d", id + 1).c_str(), TSYS::strMess(_("Stop time %d"), id + 1).c_str()),
		    TimeRemote(TSYS::strMess("timeRemote_%d", id + 1).c_str(), TSYS::strMess(_("Remote time %d"), id + 1).c_str()),
		    TimeManual(TSYS::strMess("timeManual_%d", id + 1).c_str(), TSYS::strMess(_("Manual time %d"), id + 1).c_str()),
		    TCOn(TSYS::strMess("TCOn_%d", id + 1).c_str(), TSYS::strMess(_("On TC %d"), id + 1).c_str()),
		    TCOff(TSYS::strMess("TCOff_%d", id + 1).c_str(), TSYS::strMess(_("Off TC %d"), id + 1).c_str()),
		    TCMode(TSYS::strMess("tcMode_%d", id + 1).c_str(), TSYS::strMess(_("Mode TC %d"), id + 1).c_str()),
		    Time(TSYS::strMess("time_%d", id + 1).c_str(), TSYS::strMess(_("Work time %d"), id + 1).c_str())
	    {
	    }
	    DA* da;
	    uint8_t id;

	    ui8Data State, Function;
	    ui16Data TUOn, TUOff, TUStop, TURemote, TUManual;
	    ui16Data TimeOn, TimeOff, TimeStop, TimeRemote, TimeManual;
	    ui16Data TCOn, TCOff, TCMode;
	    ui32Data Time;

	    void UpdateTUParam(uint16_t ID, uint8_t cl);
	    void UpdateTCParam(uint16_t ID, uint8_t cl);
	    uint8_t SetNewTUParam(uint8_t addr, uint16_t prmID, uint8_t *val);
	    uint8_t SetNewTCParam(uint8_t addr, uint16_t prmID, uint8_t *val);
	};
	vector<SKANSchannel> data;
	void AddNSChannel(uint8_t iid);
	//void AddAttr(SLnk& param, TFld::Type type, unsigned flg, const string& ex);
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 16;
	    } else {
		return data.size() * 2;
	    }
	}
	int lnkId(const string &id)
	{

	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) return i_l * 16;
		    if(data[i_l].Function.lnk.prmName == id) return i_l * 16 + 1;
		    if(data[i_l].TUOn.lnk.prmName == id) return i_l * 16 + 2;
		    if(data[i_l].TUOff.lnk.prmName == id) return i_l * 16 + 3;
		    if(data[i_l].TUStop.lnk.prmName == id) return i_l * 16 + 4;
		    if(data[i_l].TURemote.lnk.prmName == id) return i_l * 16 + 5;
		    if(data[i_l].TUManual.lnk.prmName == id) return i_l * 16 + 6;
		    if(data[i_l].TimeOn.lnk.prmName == id) return i_l * 16 + 7;
		    if(data[i_l].TimeOff.lnk.prmName == id) return i_l * 16 + 8;
		    if(data[i_l].TimeStop.lnk.prmName == id) return i_l * 16 + 9;
		    if(data[i_l].TimeRemote.lnk.prmName == id) return i_l * 16 + 10;
		    if(data[i_l].TimeManual.lnk.prmName == id) return i_l * 16 + 11;
		    if(data[i_l].TCOn.lnk.prmName == id) return i_l * 16 + 12;
		    if(data[i_l].TCOff.lnk.prmName == id) return i_l * 16 + 13;
		    if(data[i_l].TCMode.lnk.prmName == id) return i_l * 16 + 14;
		    if(data[i_l].Time.lnk.prmName == id) return i_l * 16 + 15;
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
		k = 16;
	    } else {
		k = 2;
	    }
	    switch(num % k) {
	    case 0:
		return data[num / k].State.lnk;
	    case 1:
		return data[num / k].Function.lnk;
	    case 2:
		return data[num / k].TUOn.lnk;
	    case 3:
		return data[num / k].TUOff.lnk;
	    case 4:
		return data[num / k].TUStop.lnk;
	    case 5:
		return data[num / k].TURemote.lnk;
	    case 6:
		return data[num / k].TUManual.lnk;
	    case 7:
		return data[num / k].TimeOn.lnk;
	    case 8:
		return data[num / k].TimeOff.lnk;
	    case 9:
		return data[num / k].TimeStop.lnk;
	    case 10:
		return data[num / k].TimeRemote.lnk;
	    case 11:
		return data[num / k].TimeManual.lnk;
	    case 12:
		return data[num / k].TCOn.lnk;
	    case 13:
		return data[num / k].TCOff.lnk;
	    case 14:
		return data[num / k].TCMode.lnk;
	    case 15:
		return data[num / k].Time.lnk;
	    }
	}

    private:
	bool with_params;
	vector<SDataRec> chan_err;

    };

} //End namespace

#endif //DA_GNS_H

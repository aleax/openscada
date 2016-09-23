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
#ifndef DA_GZD_H
#define DA_GZD_H

#include "da.h"

namespace FT3
{
#define vt_5TU       0
#define vt_6TU       1

    enum eKA_GZD_VS
    {
	vs_AWR = 0,       // авария
	vs_OFF = 1,       // закрыто(выкл.)
	vs_ON = 2,       // открыто(вкл.)
	vs_03 = 3,
	vs_04 = 4,
	vs_05 = 5,
	vs_06 = 6

    };
    enum eKA_GZD_State
    {
	KA_GZD_Error = 0x0, KA_GZD_Normal = 0x1
    };
    class KA_GZD: public DA
    {
    public:
	//Methods
	KA_GZD(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params, uint32_t v_type);
	~KA_GZD();
	uint16_t ID;
	uint16_t count_n;
	uint32_t valve_type;
	uint16_t GetState(void);
	uint16_t SetParams(void);
	uint16_t RefreshData(void);
	uint16_t RefreshParams(void);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void saveIO(void);
	void loadIO(bool force = false);
	void saveParam(void);
	void loadParam(void);
	void tmHandler(void);
	uint16_t config;
	class SKAZDchannel
	{
	public:
	    SKAZDchannel(uint8_t iid, bool w_params, uint32_t v_type, DA* owner) :
		    valve_type(v_type), da(owner), id(iid), State(TSYS::strMess("state_%d", id + 1), TSYS::strMess(_("State %d"), id + 1)),
		    Function(TSYS::strMess("function_%d", id + 1), TSYS::strMess(_("Function %d"), id + 1)),
		    TUOpen(TSYS::strMess("TUopen_%d", id + 1), TSYS::strMess(_("TU open %d"), id + 1)),
		    TUClose(TSYS::strMess("TUclose_%d", id + 1), TSYS::strMess(_("TU close %d"), id + 1)),
		    TUStop(TSYS::strMess("TUstop_%d", id + 1), TSYS::strMess(_("TU stop %d"), id + 1)),
		    TURemote(TSYS::strMess("TUremote_%d", id + 1), TSYS::strMess(_("TU remote %d"), id + 1)),
		    TUManual(TSYS::strMess("TUmanual_%d", id + 1), TSYS::strMess(_("TU manual %d"), id + 1)),
		    TUStopEx(TSYS::strMess("TUstopEx_%d", id + 1), TSYS::strMess(_("TU stop Ex %d"), id + 1)),
		    TimeOpen(TSYS::strMess("timeOpen_%d", id + 1), TSYS::strMess(_("Open time %d"), id + 1)),
		    TimeClose(TSYS::strMess("timeClose_%d", id + 1), TSYS::strMess(_("Close time %d"), id + 1)),
		    TimeStop(TSYS::strMess("timeStop_%d", id + 1), TSYS::strMess(_("Stop time %d"), id + 1)),
		    TimeStopEx(TSYS::strMess("timeStopEx_%d", id + 1), TSYS::strMess(_("Stop time Ex %d"), id + 1)),
		    TimeRemote(TSYS::strMess("timeRemote_%d", id + 1), TSYS::strMess(_("Remote time %d"), id + 1)),
		    TimeManual(TSYS::strMess("timeManual_%d", id + 1), TSYS::strMess(_("Manual time %d"), id + 1)),
		    TCOpen(TSYS::strMess("tcOpen_%d", id + 1), TSYS::strMess(_("Open TC %d"), id + 1)),
		    TCClose(TSYS::strMess("tcClose_%d", id + 1), TSYS::strMess(_("Close TC %d"), id + 1)),
		    TCMode(TSYS::strMess("tcMode_%d", id + 1), TSYS::strMess(_("Mode TC %d"), id + 1)),
		    TCOpenErr(TSYS::strMess("tcOpenErr_%d", id + 1), TSYS::strMess(_("Open error TC %d"), id + 1)),
		    TCCloseErr(TSYS::strMess("tcCloseErr_%d", id + 1), TSYS::strMess(_("Close error TC %d"), id + 1))
	    {
		if(w_params) {
		    switch(v_type) {
		    case vt_5TU:
			n_link = 17;
			break;
		    case vt_6TU:
			n_link = 19;
			break;
		    default:
			n_link = 2;
			break;
		    }
		} else {
		    n_link = 2;
		}
	    }
	    DA* da;
	    uint8_t id;
	    uint32_t n_link;
	    uint32_t valve_type;

	    ui8Data State, Function;
	    ui16Data TUOpen, TUClose, TUStop, TUStopEx, TURemote, TUManual;
	    ui16Data TimeOpen, TimeClose, TimeStop, TimeStopEx, TimeRemote, TimeManual;
	    ui16Data TCOpen, TCClose, TCMode, TCOpenErr, TCCloseErr;

	    void UpdateTUParam(uint16_t ID, uint8_t cl);
	    void UpdateTCParam(uint16_t ID, uint8_t cl);
	    uint8_t SetNewTUParam(uint8_t addr, uint16_t prmID, uint8_t *val);
	    uint8_t SetNewTCParam(uint8_t addr, uint16_t prmID, uint8_t *val);
	    uint8_t SetNewState(uint8_t addr, uint16_t prmID, uint8_t *val);
	    uint8_t SetNewFunction(uint8_t addr, uint16_t prmID, uint8_t *val);
	};
	vector<SKAZDchannel> data;
	void AddZDChannel(uint8_t iid);
	int lnkSize()
	{
	    return data.size() * data[0].n_link;
	}
	int lnkId(const string &id)
	{
	    for(int i_l = 0; i_l < data.size(); i_l++) {
		uint32_t k = data[i_l].n_link;
		if(data[i_l].State.lnk.prmName == id) return i_l * k;
		if(data[i_l].Function.lnk.prmName == id) return i_l * k + 1;
		if(with_params) {
		    if(data[i_l].TUOpen.lnk.prmName == id) return i_l * k + 2;
		    if(data[i_l].TUClose.lnk.prmName == id) return i_l * k + 3;
		    if(data[i_l].TUStop.lnk.prmName == id) return i_l * k + 4;
		    if(data[i_l].TURemote.lnk.prmName == id) return i_l * k + 5;
		    if(data[i_l].TUManual.lnk.prmName == id) return i_l * k + 6;
		    if(data[i_l].TimeOpen.lnk.prmName == id) return i_l * k + 7;
		    if(data[i_l].TimeClose.lnk.prmName == id) return i_l * k + 8;
		    if(data[i_l].TimeStop.lnk.prmName == id) return i_l * k + 9;
		    if(data[i_l].TimeRemote.lnk.prmName == id) return i_l * k + 10;
		    if(data[i_l].TimeManual.lnk.prmName == id) return i_l * k + 11;
		    if(data[i_l].TCOpen.lnk.prmName == id) return i_l * k + 12;
		    if(data[i_l].TCClose.lnk.prmName == id) return i_l * k + 13;
		    if(data[i_l].TCMode.lnk.prmName == id) return i_l * k + 14;
		    if(data[i_l].TCOpenErr.lnk.prmName == id) return i_l * k + 15;
		    if(data[i_l].TCCloseErr.lnk.prmName == id) return i_l * k + 16;
		    if(valve_type == vt_6TU) {
			if(data[i_l].TUStopEx.lnk.prmName == id) return i_l * k + 17;
			if(data[i_l].TimeStopEx.lnk.prmName == id) return i_l * k + 18;
		    }

		}
	    }
	    return -1;
	}
	SLnk &lnk(int num)
	{
	    uint32_t k = data[0].n_link;
	    switch(num % k) {
	    case 0:
		return data[num / k].State.lnk;
	    case 1:
		return data[num / k].Function.lnk;
	    case 2:
		return data[num / k].TUOpen.lnk;
	    case 3:
		return data[num / k].TUClose.lnk;
	    case 4:
		return data[num / k].TUStop.lnk;
	    case 5:
		return data[num / k].TURemote.lnk;
	    case 6:
		return data[num / k].TUManual.lnk;
	    case 7:
		return data[num / k].TimeOpen.lnk;
	    case 8:
		return data[num / k].TimeClose.lnk;
	    case 9:
		return data[num / k].TimeStop.lnk;
	    case 10:
		return data[num / k].TimeRemote.lnk;
	    case 11:
		return data[num / k].TimeManual.lnk;
	    case 12:
		return data[num / k].TCOpen.lnk;
	    case 13:
		return data[num / k].TCClose.lnk;
	    case 14:
		return data[num / k].TCMode.lnk;
	    case 15:
		return data[num / k].TCOpenErr.lnk;
	    case 16:
		return data[num / k].TCCloseErr.lnk;
	    case 17:
		return data[num / k].TUStopEx.lnk;
	    case 18:
		return data[num / k].TimeStopEx.lnk;
	    }
	}

    private:
	bool with_params;
	vector<SDataRec> chan_err;

    };

} //End namespace

#endif //DA_GZD_H

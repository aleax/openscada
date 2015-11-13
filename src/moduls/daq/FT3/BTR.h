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
#ifndef DA_BTR_H
#define DA_BTR_H

#include "da.h"

namespace FT3
{
    class KA_BTU: public DA
    {
    public:
	//Methods
	KA_BTU(TMdPrm& prm, uint16_t id, uint16_t nu, bool has_params);
	~KA_BTU();
	uint16_t ID;
	uint16_t count_nu;
	bool with_params;
	uint16_t config;
	void AddChannel(uint8_t iid);
	void saveIO(void);
	void loadIO(bool force = false);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	void setTU(uint8_t);
	uint8_t runTU(uint8_t);
	string getStatus(void);
	void tmHandler(void);
	class SKATUchannel
	{
	public:
	    SKATUchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), iTY(0), Line(TSYS::strMess("Line_%d", id + 1), TSYS::strMess(_("Line %d"), id + 1))
	    {
		for(int i = 0; i < 16; i++) {
		    Time.push_back(ui16Data(TSYS::strMess("Time%d_%d", i + 1, id + 1), TSYS::strMess(_("Time #%d %d"), i + 1, id + 1)));
		}
	    }
	    DA* da;
	    uint8_t id;
	    ui16Data Line;
	    uint8_t iTY;
	    vector<ui16Data> Time;
	};
	vector<SKATUchannel> TUdata;
	int lnkSize()
	{
	    if(with_params) {
		return TUdata.size() * 17;
	    } else {
		return TUdata.size() * 1;
	    }
	}
	int lnkId(const string &id)
	{
	    if(with_params) {
		for(int i_l = 0; i_l < TUdata.size(); i_l++) {
		    if(TUdata[i_l].Line.lnk.prmName == id) return i_l * 17;
		    for(int i = 0; i < 16; i++) {
			if(TUdata[i_l].Time[i].lnk.prmName == id) return i_l * 17 + i + 1;
		    }
		}
	    } else {
		for(int i_l = 0; i_l < TUdata.size(); i_l++) {
		    if(TUdata[i_l].Line.lnk.prmName == id) return i_l;
		}
	    }
	    return -1;
	}
	SLnk &lnk(int num)
	{
	    int k;
	    if(with_params) {
		k = 17;
	    } else {
		k = 1;
	    }
	    switch(num % k) {
	    case 0:
		return TUdata[num / k].Line.lnk;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
		return TUdata[num / k].Time[num % k - 1].lnk;
	    }
	}
    }
    ;

    class B_BTR: public DA
    {
    public:
	//Methods
	B_BTR(TMdPrm& prm, uint16_t id, uint16_t nu, uint16_t nr, bool has_params);
	~B_BTR();
	uint16_t ID;
	uint16_t count_nu;
	uint16_t count_nr;
	uint8_t currTU;
	bool with_params;
	void AddTUChannel(uint8_t iid);
	void AddTRChannel(uint8_t iid);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	void setTU(uint8_t);
	void runTU(uint8_t);
	string getStatus(void);
	void saveIO(void);
	void loadIO(bool force = false);
	void tmHandler(void);
	class STRchannel
	{
	public:
	    STRchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), Value(TSYS::strMess("value_%d", id + 1), TSYS::strMess(_("Value %d"), id + 1))
	    {
	    }
	    DA* da;
	    uint8_t id;

	    flData Value;
	};
	class STUchannel
	{
	public:
	    STUchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), On(TSYS::strMess("on_%d", id + 1), TSYS::strMess(_("On %d"), id + 1)),
		    Off(TSYS::strMess("off_%d", id + 1), TSYS::strMess(_("Off %d"), id + 1)),
		    Run(TSYS::strMess("run_%d", id + 1), TSYS::strMess(_("Run %d"), id + 1)),
		    Reset(TSYS::strMess("reset_%d", id + 1), TSYS::strMess(_("Reset %d"), id + 1)),
		    Time(TSYS::strMess("time_%d", id + 1), TSYS::strMess(_("Time %d"), id + 1)),
		    TC(TSYS::strMess("tc_%d", id + 1), TSYS::strMess(_("TC %d"), id + 1)),
		    ExTime(TSYS::strMess("extime_%d", id + 1), TSYS::strMess(_("ExTime %d"), id + 1))

	    {
	    }
	    DA* da;
	    uint8_t id;

	    ui8Data On, Off, Run, Reset;
	    ui16Data TC;
	    ui16Data Time;
	    ui8Data ExTime;
	};
	vector<STRchannel> TRdata;
	vector<STUchannel> TUdata;
	int lnkSize()
	{
	    if(with_params) {
		return TUdata.size() * 7 + TRdata.size();
	    } else {
		return TUdata.size() * 4 + TRdata.size();
	    }
	}
	int lnkId(const string &id)
	{
	    if(with_params) {
		for(int i_l = 0; i_l < TUdata.size(); i_l++) {
		    if(TUdata[i_l].On.lnk.prmName == id) return i_l * 7;
		    if(TUdata[i_l].Off.lnk.prmName == id) return i_l * 7 + 1;
		    if(TUdata[i_l].Run.lnk.prmName == id) return i_l * 7 + 2;
		    if(TUdata[i_l].Reset.lnk.prmName == id) return i_l * 7 + 3;
		    if(TUdata[i_l].Time.lnk.prmName == id) return i_l * 7 + 4;
		    if(TUdata[i_l].TC.lnk.prmName == id) return i_l * 7 + 5;
		    if(TUdata[i_l].ExTime.lnk.prmName == id) return i_l * 7 + 6;
		}
		for(int i_l = 0; i_l < TRdata.size(); i_l++) {
		    if(TRdata[i_l].Value.lnk.prmName == id) return i_l + TUdata.size() * 7;
		}
	    } else {
		for(int i_l = 0; i_l < TUdata.size(); i_l++) {
		    if(TUdata[i_l].On.lnk.prmName == id) return i_l * 4;
		    if(TUdata[i_l].Off.lnk.prmName == id) return i_l * 4 + 1;
		    if(TUdata[i_l].Run.lnk.prmName == id) return i_l * 4 + 2;
		    if(TUdata[i_l].Reset.lnk.prmName == id) return i_l * 4 + 3;
		}
		for(int i_l = 0; i_l < TRdata.size(); i_l++) {
		    if(TRdata[i_l].Value.lnk.prmName == id) return i_l + TUdata.size() * 4;
		}
	    }

	    return -1;
	}
	SLnk &lnk(int num)
	{
	    if(with_params) {
		if((TUdata.size() > 0) && ((TUdata.size() * 7) > num)) {
		    switch(num % 7) {
		    case 0:
			return TUdata[num / 7].On.lnk;
		    case 1:
			return TUdata[num / 7].Off.lnk;
		    case 2:
			return TUdata[num / 7].Run.lnk;
		    case 3:
			return TUdata[num / 7].Reset.lnk;
		    case 4:
			return TUdata[num / 7].Time.lnk;
		    case 5:
			return TUdata[num / 7].TC.lnk;
		    case 6:
			return TUdata[num / 7].ExTime.lnk;
		    }
		} else {
		    return TRdata[num - TUdata.size() * 7].Value.lnk;
		}
	    } else {
		if((TUdata.size() > 0) && ((TUdata.size() * 4) > num)) {
		    switch(num % 4) {
		    case 0:
			return TUdata[num / 4].On.lnk;
		    case 1:
			return TUdata[num / 4].Off.lnk;
		    case 2:
			return TUdata[num / 4].Run.lnk;
		    case 3:
			return TUdata[num / 4].Reset.lnk;

		    }
		} else {
		    return TRdata[num - TUdata.size() * 4].Value.lnk;
		}
	    }

	}
    };

} //End namespace

#endif //DA_BTR_H

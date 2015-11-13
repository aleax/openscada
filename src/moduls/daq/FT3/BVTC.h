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
#ifndef DA_BVTC_H
#define DA_BVTC_H

#include "da.h"

namespace FT3
{
    class KA_BVTC: public DA
    {
    public:
	//Methods
	KA_BVTC(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~KA_BVTC();
	uint16_t ID;
	uint16_t count_n;
	bool with_params;
	void AddChannel(uint8_t iid);
	void saveIO(void);
	void loadIO(bool force = false);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void tmHandler(void);
	uint16_t config;
	class SKATCchannel
	{
	public:
	    SKATCchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), Value(TSYS::strMess("TC_%d", id + 1), TSYS::strMess(_("TC %d"), id + 1)),
		    Period(TSYS::strMess("Period_%d", id + 1), TSYS::strMess(_("Period TC %d"), id + 1)),
		    Count(TSYS::strMess("Count_%d", id + 1), TSYS::strMess(_("Count TC %d"), id + 1))
	    {
	    }
	    DA* da;
	    uint8_t id;
	    ui8Data Value, Period, Count;
	};
	vector<SKATCchannel> data;
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 3;
	    } else {
		return data.size();
	    }
	}
	int lnkId(const string &id)
	{
	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].Value.lnk.prmName == id) return i_l * 3;
		    if(data[i_l].Period.lnk.prmName == id) return i_l * 3 + 1;
		    if(data[i_l].Count.lnk.prmName == id) return i_l * 3 + 2;
		}
	    } else {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].Value.lnk.prmName == id) {
			return i_l;
		    }
		}
	    }
	    return -1;
	}
	SLnk &lnk(int num)
	{
	    int k;
	    if(with_params) {
		k = 3;
	    } else {
		k = 1;
	    }
	    switch(num % k) {
	    case 0:
		return data[num / k].Value.lnk;
	    case 1:
		return data[num / k].Period.lnk;
	    case 2:
		return data[num / k].Count.lnk;
	    }
	}
    };

    class B_BVTC: public DA
    {
    public:
	//Methods
	B_BVTC(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~B_BVTC();
	uint16_t ID;
	uint16_t count_n;
	bool with_params;
	void AddChannel(uint8_t iid);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void saveIO(void);
	void loadIO(bool force = false);
	void tmHandler(void);
	class STCchannel
	{
	public:
	    STCchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), Value(TSYS::strMess("TC_%d", id + 1), TSYS::strMess(_("TC %d"), id + 1)),
		    Mask(TSYS::strMess("Mask_%d", id + 1), TSYS::strMess(_("Mask %d"), id + 1))
	    {
	    }
	    DA* da;
	    uint8_t id;
	    ui8Data Value, Mask;
	};
	vector<STCchannel> data;
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 2;
	    } else {
		return data.size();
	    }
	}
	int lnkId(const string &id)
	{
	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].Value.lnk.prmName == id) {
			return i_l * 2;
		    }
		    if(data[i_l].Mask.lnk.prmName == id) {
			return i_l * 2 + 1;
		    }
		}
	    } else {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].Value.lnk.prmName == id) {
			return i_l;
		    }
		}
	    }
	    return -1;
	}
	SLnk &lnk(int num)
	{
	    if(with_params) {
		switch(num % 2) {
		case 0:
		    return data[num / 2].Value.lnk;
		case 1:
		    return data[num / 2].Mask.lnk;
		}
	    } else
		return data[num].Value.lnk;
	}
    };

} //End namespace

#endif //DA_BVTC_H

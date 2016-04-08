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
#ifndef DA_ODOR_H
#define DA_ODOR_H

#include "da.h"

namespace FT3
{
    class B_ODOR: public DA
    {
    public:
	//Methods
	B_ODOR(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~B_ODOR();
	uint16_t ID;
	uint16_t count_n;
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
	class SPchannel
	{
	public:
	    SPchannel(uint8_t iid, DA* owner) :
		    da(owner), id(iid), State(TSYS::strMess("state_%d", id + 1), TSYS::strMess(_("State %d"), id + 1)),
		    Value(TSYS::strMess("value_%d", id + 1), TSYS::strMess(_("Value %d"), id + 1)),
		    Period(TSYS::strMess("period_%d", id + 1), TSYS::strMess(_("Measure period %d"), id + 1)),
		    Sens(TSYS::strMess("sens_%d", id + 1), TSYS::strMess(_("Sensitivity %d"), id + 1)),
		    Count(TSYS::strMess("count_%d", id + 1), TSYS::strMess(_("Count %d"), id + 1)),
		    Vdrop(TSYS::strMess("vdrop_%d", id + 1), TSYS::strMess(_("Volume a drop %d"), id + 1))
	    {
	    }
	    DA* da;
	    uint8_t id;

	    ui8Data State, Period;

	    flData Value, Sens, Vdrop;
	    ui32Data Count;
	};
	vector<SPchannel> data;
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 6;
	    } else {
		return data.size() * 2;
	    }
	}
	int lnkId(const string &id)
	{

	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) return i_l * 6;
		    if(data[i_l].Value.lnk.prmName == id) return i_l * 6 + 1;
		    if(data[i_l].Period.lnk.prmName == id) return i_l * 6 + 2;
		    if(data[i_l].Sens.lnk.prmName == id) return i_l * 6 + 3;
		    if(data[i_l].Count.lnk.prmName == id) return i_l * 6 + 4;
		    if(data[i_l].Vdrop.lnk.prmName == id) return i_l * 6 + 5;
		}
	    } else {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) {
			return i_l * 2;
		    }
		    if(data[i_l].Value.lnk.prmName == id) {
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
		k = 6;
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
		return data[num / k].Count.lnk;
	    case 5:
		return data[num / k].Vdrop.lnk;

	    }
	}

    private:
	bool with_params;
	vector<SDataRec> chan_err;
    };

} //End namespace

#endif //DA_ODOR_H

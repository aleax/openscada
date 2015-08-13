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
#ifndef DA_BVI_H
#define DA_BVI_H

#include "da.h"

namespace FT3
{
    class B_BVI: public DA
    {
    public:
	//Methods
	B_BVI(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params, bool has_ext_period);
	~B_BVI();
	uint16_t ID;
	uint16_t count_n;
	void AddChannel(uint8_t iid);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void saveIO(void);
	void loadIO(bool force = false);
	void tmHandler(void);
	class STIchannel
	{
	public:
	    STIchannel(uint8_t iid, DA* owner) : da(owner),
		    id(iid), State(TSYS::strMess("state_%d", id + 1).c_str(), TSYS::strMess(_("State %d"), id + 1).c_str()),
		    Value(TSYS::strMess("value_%d", id + 1).c_str(), TSYS::strMess(_("Value %d"), id + 1).c_str()),
		    Period(TSYS::strMess("period_%d", id + 1).c_str(), TSYS::strMess(_("Measure period %d"), id + 1).c_str()),
		    Sens(TSYS::strMess("sens_%d", id + 1).c_str(), TSYS::strMess(_("Sensitivity %d"), id + 1).c_str()),
		    Count(TSYS::strMess("count_%d", id + 1).c_str(), TSYS::strMess(_("Count %d"), id + 1).c_str()),
		    Factor(TSYS::strMess("factor_%d", id + 1).c_str(), TSYS::strMess(_("Range factor %d"), id + 1).c_str()),
		    Dimension(TSYS::strMess("dimens_%d", id + 1).c_str(), TSYS::strMess(_("Dimension %d"), id + 1).c_str())

	    {
	    }
	    DA* da;
	    uint8_t id;

	    ui8Data State, Dimension;

	    ui16Data Period;
	    flData Value, Sens, Factor;
	    ui32Data Count;
	};
	vector<STIchannel> data;
	int lnkSize()
	{
	    if(with_params) {
		return data.size() * 7;
	    } else {
		return data.size() * 2;
	    }
	}
	int lnkId(const string &id)
	{
	    if(with_params) {
		for(int i_l = 0; i_l < data.size(); i_l++) {
		    if(data[i_l].State.lnk.prmName == id) return i_l * 7;
		    if(data[i_l].Value.lnk.prmName == id) return i_l * 7 + 1;
		    if(data[i_l].Period.lnk.prmName == id) return i_l * 7 + 2;
		    if(data[i_l].Sens.lnk.prmName == id) return i_l * 7 + 3;
		    if(data[i_l].Count.lnk.prmName == id) return i_l * 7 + 4;
		    if(data[i_l].Factor.lnk.prmName == id) return i_l * 7 + 5;
		    if(data[i_l].Dimension.lnk.prmName == id) return i_l * 7 + 6;
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
		k = 7;
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
		return data[num / k].Factor.lnk;
	    case 6:
		return data[num / k].Dimension.lnk;

	    }
	}
    private:
	bool with_params, ext_period;
	vector<SDataRec> chan_err;

    };

} //End namespace

#endif //DA_BVI_H

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
#ifndef DA_BUC_H
#define DA_BUC_H

#include "da.h"

namespace FT3
{
    enum eKA_BUC_State
    {
	KA_BUC_HardReset = 0x0,
	KA_BUC_Normal = 0x1,
	KA_BUC_SoftReset = 0x2,
	KA_BUC_Setup = 0x80,
	KA_BUC_Start = 0x81,
    };

    class KA_BUC: public DA
    {
    public:
	//Methods
 	KA_BUC(TMdPrm& prm, uint16_t id);
 	~KA_BUC();
 	uint16_t ID;
 	uint16_t clockID;
 	uint16_t Task(uint16_t);
	uint16_t GetState(void);
	uint16_t SetupClock(void);
	uint16_t Start(void);
	uint16_t RefreshData(void);
 	uint16_t HandleEvent(int64_t tm, uint8_t *);
 	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
 	uint8_t cmdSet(uint8_t * req, uint8_t addr);
 	uint16_t setVal(TVal &val);
 	void vlGet(TVal &val);
 	string getStatus(void);
 	void tmHandler(void);

 	uint16_t modification;
 	uint16_t config;
 	uint16_t clockconfig;
 	uint8_t state;
 	uint8_t clockstate;
 	uint8_t s_state;
	int lnkSize()
	{
	    return 0;
	}
	int lnkId(const string &id)
	{
	    return -1;
	}
    };

    class B_BUC: public DA
    {
    public:
	//Methods
	B_BUC(TMdPrm& prm, uint16_t id, uint16_t modif);
	~B_BUC();
	uint16_t ID;
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t tm, uint8_t *);
	uint8_t cmdGet(uint16_t prmID, uint8_t * out);
	uint8_t cmdSet(uint8_t * req, uint8_t addr);
	uint16_t setVal(TVal &val);
	string getStatus(void);
	void tmHandler(void);

	uint8_t months[12];

	uint16_t mod_KP; //-----------модификация КП
	uint8_t state; //------------состояние
	uint8_t stateWatch; //-------состояние: 0 - норма, 1 - не установлен
	uint8_t s_tm; //-------------адрес установившей станции
	uint8_t wt1; //--------------задержка после включения передатчика модема
	uint8_t wt2; //--------------задержка перед выключением передатчика модема
	uint8_t s_wt1; //------------адрес станции установившей параметр
	uint8_t s_wt2; //------------адрес станции установившей параметр
	int lnkSize()
	{
	    return 0;
	}
	int lnkId(const string &id)
	{
	    return -1;
	}
    };

} //End namespace

//*************************************************
//* BUC                                           *
//*************************************************
#endif //DA_BUC_H

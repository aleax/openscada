//OpenSCADA system module DAQ.BUC file: BUC.cpp
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

#include <sys/times.h>

#include <tsys.h>

#include "mod_FT3.h"
#include "BUC.h"

using namespace FT3;

KA_BUC::KA_BUC(TMdPrm& prm, uint16_t id) :
	DA(prm), ID(id), clockID(id + 1), modification(0), state(0), clockstate(0), s_state(0), config(0x2800 | (2 << 10)), clockconfig(0x01 | (1 << 10))
{
    mTypeFT3 = KA;
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TVal::DirWrite));
    fld->setReserve("0:0");
    mPrm.p_el.fldAdd(fld = new TFld("config", _("Configuration"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:1");
    mPrm.p_el.fldAdd(fld = new TFld("modification", _("Modification"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:2");
}

KA_BUC::~KA_BUC()
{

}

string KA_BUC::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

void KA_BUC::tmHandler(void)
{

}

uint16_t KA_BUC::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 9;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0); //state
	*((uint16_t *) (Msg.D + 2)) = PackID(ID, 0, 1); //configuration
	*((uint16_t *) (Msg.D + 4)) = PackID(ID, 0, 2); //modification
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		mPrm.vlAt("state").at().setI(Msg.D[8], 0, true);
		mPrm.vlAt("config").at().setI(TSYS::getUnalign16(Msg.D + 13), 0, true);
		mPrm.vlAt("modification").at().setI(TSYS::getUnalign16(Msg.D + 19), 0, true);
		if(mPrm.vlAt("state").at().getI(0, true) != 1) {
		    if(Task(TaskSet) == 1) {
			rc = 1;
		    }
		} else {
		    rc = 1;
		}
	    }

	}
	if(rc) NeedInit = false;
	break;
    case TaskIdle:
	if(mPrm.vlAt("state").at().getI(0, true) != 0) {
	    rc = 2;
	}
	break;
    }
    return rc;
}

uint16_t KA_BUC::HandleEvent(uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    //if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    if(ft3ID.g == ID) {
	switch(ft3ID.k) {
	case 0:
	    switch(ft3ID.n) {
	    case 0:
		mPrm.vlAt("state").at().setI(D[3], 0, true);
		l = 4;
		break;
	    case 1:
		mPrm.vlAt("config").at().setI(TSYS::getUnalign16(D + 2), 0, true);
		l = 4;
		break;
	    case 2:
		mPrm.vlAt("modification").at().setI(TSYS::getUnalign16(D + 2), 0, true);
		l = 4;
		break;

	    }
	    break;
	}
    }
    if(ft3ID.g == clockID) {
	/*	switch(ft3ID.k) {
	 case 0:
	 switch(ft3ID.n) {
	 case 0:
	 mPrm.vlAt("state").at().setI(D[3], 0, true);
	 l = 4;
	 break;
	 case 1:
	 mPrm.vlAt("config").at().setI(TSYS::getUnalign16(D + 2), 0, true);
	 l = 4;
	 break;
	 case 2:
	 mPrm.vlAt("modification").at().setI(TSYS::getUnalign16(D + 2), 0, true);
	 l = 4;
	 break;

	 }
	 break;
	 }*/
    }
    return l;
}

uint16_t KA_BUC::setVal(TVal &val)
{
    int off = 0;
    FT3ID ft3ID;
    ft3ID.k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер объекта
    ft3ID.n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер параметра
    ft3ID.g = ID;
    tagMsg Msg;
    Msg.L = 0;
    Msg.C = SetData;
    *((uint16_t *) Msg.D) = PackID(ft3ID);
    switch(ft3ID.k) {
    case 0:
	switch(ft3ID.n) {
	case 0:
	    tagMsg Msg;
	    Msg.L = 6;
	    Msg.D[2] = val.get(NULL, true).getI();
	    break;
	}
	break;
    }
    return 0;
}

uint8_t KA_BUC::cmdGet(uint16_t prmID, uint8_t * out)
{
//    mess_info("KA_BUC", "cmdGet %04X", prmID);
    FT3ID ft3ID = UnpackID(prmID);
//    mess_info("KA_BUC", "ID ft3ID g k n ", ID, ft3ID.g, ft3ID.k, ft3ID.n);
    time_t rawtime;
//    if(ft3ID.g != ID) return 0;
    uint l = 0;
    if(ft3ID.g == ID) {
	switch(ft3ID.k) {
	case 0:
	    switch(ft3ID.n) {
	    case 0:
		out[0] = s_state;
		out[1] = state;
		l = 2;
		break;
	    case 1:
		out[0] = config & 0xFF;
		out[1] = (config >> 8) & 0xFF;
		l = 2;
		break;
	    case 2:
		out[0] = modification & 0xFF;
		out[1] = (modification >> 8) & 0xFF;
		l = 2;
		break;
	    }
	    break;
	}
    }
    if(ft3ID.g == clockID) {
	switch(ft3ID.k) {
	case 0:
	    switch(ft3ID.n) {
	    case 0:
		out[0] = clockstate;
		l = 1;
		break;
	    case 1:
		out[0] = clockconfig & 0xFF;
		out[1] = (clockconfig >> 8) & 0xFF;
		l = 2;
		break;
	    case 2:
		out[0] = s_state;
		time(&rawtime);
		mPrm.owner().Time_tToDateTime(out + 1, rawtime);
		l = 6;
		break;
	    case 3:
		time(&rawtime);
		mPrm.owner().Time_tToDateTime(out, rawtime);
		l = 5;
		break;
	    }
	    break;
	}
    }
    return l;
}
uint8_t KA_BUC::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    //if(ft3ID.g != ID) return 0;
    uint8_t l = 0;
    time_t rawtime;
    if(ft3ID.g == ID) {
	switch(ft3ID.k) {
	case 0:
	    switch(ft3ID.k) {
	    case 0:
		if(req[2] & 0x81) {
		    if(state != req[2]) {
			if(req[2] == 1) {
			    if((state == 0x80) || (state == 0x02)) {
				state = 1;
			    } else {
				state = 0x81;
			    }
			} else {
			    state = req[2];
			}
		    }
		    s_state = addr;
		    uint8_t E[2] = { addr, state };
		    mPrm.owner().PushInBE(1, 2, prmID, E);
		    l = 3;
		}
		break;
	    }
	    break;
	}
    }
    if(ft3ID.g == clockID) {
	switch(ft3ID.k) {
	case 0:
	    switch(ft3ID.n) {
	    case 2:
		s_state = addr;
		state = clockstate = 1;
		rawtime = mPrm.owner().DateTimeToTime_t(req + 2);
		stime(&rawtime);
		l = 7;
		break;
	    }
	    break;
	}
    }

    return l;
}

B_BUC::B_BUC(TMdPrm& prm, uint16_t id) :
	DA(prm), ID(id), mod_KP(0), state(0), stateWatch(0), s_tm(0), wt1(0), wt2(0), s_wt1(0), s_wt2(0)
{
    mTypeFT3 = GRS;
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));

    fld->setReserve("0:0");
    mPrm.p_el.fldAdd(fld = new TFld("mod", _("Modification"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:1");
    mPrm.p_el.fldAdd(fld = new TFld("sttimer", _("Timer state"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("1:0");
    mPrm.p_el.fldAdd(fld = new TFld("curdt", _("Current datetime"), TFld::String, TVal::DirWrite));
    fld->setReserve("1:1");
    mPrm.p_el.fldAdd(fld = new TFld("stopdt", _("Stop datetime"), TFld::String, TFld::NoWrite));
    fld->setReserve("1:2");
    mPrm.p_el.fldAdd(fld = new TFld("dl1", _("Delay 1"), TFld::Integer, TVal::DirWrite));
    fld->setReserve("2:1");
    mPrm.p_el.fldAdd(fld = new TFld("dl2", _("Delay 2"), TFld::Integer, TVal::DirWrite));
    fld->setReserve("2:2");

}

B_BUC::~B_BUC()
{

}

string B_BUC::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

void B_BUC::tmHandler(void)
{

}

uint16_t B_BUC::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 17;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0); //состояние
	*((uint16_t *) (Msg.D + 2)) = PackID(ID, 0, 1); //модификация
	*((uint16_t *) (Msg.D + 4)) = PackID(ID, 1, 0); //состояние таймера
	*((uint16_t *) (Msg.D + 6)) = PackID(ID, 1, 1); //текущее время
	*((uint16_t *) (Msg.D + 8)) = PackID(ID, 1, 2); //время останова
	*((uint16_t *) (Msg.D + 10)) = PackID(ID, 2, 1); //задержка 1
	*((uint16_t *) (Msg.D + 12)) = PackID(ID, 2, 2); //задержка 2
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		mPrm.vlAt("state").at().setI(Msg.D[7], 0, true);
		mPrm.vlAt("mod").at().setI(TSYS::getUnalign16(Msg.D + 12), 0, true);
		mPrm.vlAt("sttimer").at().setI(Msg.D[18], 0, true);
		time_t t = mPrm.owner().DateTimeToTime_t(Msg.D + 24);
		mPrm.vlAt("curdt").at().setS(TSYS::time2str(t, "%d.%m.%Y %H:%M:%S"), 0, true);
		t = mPrm.owner().DateTimeToTime_t(Msg.D + 33);
		mPrm.vlAt("stopdt").at().setS(TSYS::time2str(t, "%d.%m.%Y %H:%M:%S"), 0, true);
		mPrm.vlAt("dl1").at().setI(Msg.D[43], 0, true);
		mPrm.vlAt("dl2").at().setI(Msg.D[49], 0, true);
		if(mPrm.vlAt("state").at().getI(0, true) != 0) {
		    if(Task(TaskSet) == 1) {
			rc = 1;
		    }
		} else {
		    rc = 1;
		}
	    }

	}
	if(rc) NeedInit = false;
	break;
    case TaskSet:
	time_t rawtime;
	time(&rawtime);
	Msg.L = 10;
	Msg.C = SetData;
	*((uint16_t *) Msg.D) = PackID(ID, 1, 1); //текущее время
	mPrm.owner().Time_tToDateTime(Msg.D + 2, rawtime);
	mPrm.owner().Transact(&Msg);
	if(Msg.C == GOOD2) {
	    rc = 1;
	}
	break;
    case TaskIdle:
	if(mPrm.vlAt("state").at().getI(0, true) != 0) {
	    rc = 2;
	}
	break;
    }
    return rc;
}

uint16_t B_BUC::HandleEvent(uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    switch(ft3ID.k) {
    case 0:
	switch(ft3ID.n) {
	case 0:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt("mod").at().setI(TSYS::getUnalign16(D + 2), 0, true);
	    l = 4;
	    break;
	}
	break;
    case 1:
	switch(ft3ID.n) {
	time_t t;
    case 0:
	mPrm.vlAt("sttimer").at().setI(D[2], 0, true);
	l = 3;
	break;
    case 1:
	t = mPrm.owner().DateTimeToTime_t(D + 3);
	mPrm.vlAt("curdt").at().setS(TSYS::time2str(t, "%d.%m.%Y %H:%M:%S"), 0, true);
	l = 8;
	break;
    case 2:
	t = mPrm.owner().DateTimeToTime_t(D + 2);
	mPrm.vlAt("stopdt").at().setS(TSYS::time2str(t, "%d.%m.%Y %H:%M:%S"), 0, true);
	l = 7;
	break;
	}
	break;
    case 2:
	switch(ft3ID.n) {
	case 1:
	    mPrm.vlAt("dl1").at().setI(D[3], 0, true);
	    l = 4;
	    break;
	case 2:
	    mPrm.vlAt("dl2").at().setI(D[3], 0, true);
	    l = 4;
	    break;
	}
	break;
    }
    return l;
}

uint16_t B_BUC::setVal(TVal &val)
{
    int off = 0;
    FT3ID ft3ID;
    ft3ID.k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер объекта
    ft3ID.n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер параметра
    ft3ID.g = ID;
    tagMsg Msg;
    Msg.L = 0;
    Msg.C = SetData;
    *((uint16_t *) Msg.D) = PackID(ft3ID);
    switch(ft3ID.k) {
    case 1:
	switch(ft3ID.n) {
	case 1:
	    struct tm tm_tm;

	    strptime(val.get(NULL, true).getS().c_str(), "%d.%m.%Y %H:%M:%S", &tm_tm);

	    Msg.L = 10;
	    mPrm.owner().Time_tToDateTime(Msg.D + 2, mktime(&tm_tm));
	    break;
	}
	break;
    case 2:
	switch(ft3ID.n) {
	case 1:
	case 2:
	    tagMsg Msg;
	    Msg.L = 6;
	    Msg.D[2] = val.get(NULL, true).getI();
	    break;

	}
	break;

    }
    return 0;
}

uint8_t B_BUC::cmdGet(uint16_t prmID, uint8_t * out)
{
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    time_t rawtime;
    switch(ft3ID.k) {
    case 0:
	switch(ft3ID.n) {
	case 0:
	    out[0] = state;
	    l = 1;
	    break;
	case 1:
	    out[0] = mod_KP & 0xFF;
	    out[1] = (mod_KP >> 8) & 0xFF;
	    l = 2;
	    break;
	}
	break;

    case 1:
	switch(ft3ID.n) {
	case 0:
	    out[0] = stateWatch;
	    l = 1;
	    break;
	case 1:
	    out[0] = s_tm;
	    time(&rawtime);
	    mPrm.owner().Time_tToDateTime(out + 1, rawtime);
	    l = 6;
	    break;
	case 2:
	    time(&rawtime);
	    mPrm.owner().Time_tToDateTime(out, rawtime);
	    l = 5;
	    break;
	}
	break;
    case 2:
	switch(ft3ID.n) {
	/*		case 0:
	 out[0] = s_tlg;
	 do
	 z[l + 3] = tlg_buf[l];
	 while (tlg_buf[l++]);
	 l += 3;
	 break;*/
	case 1:
	    out[0] = s_wt1;
	    out[1] = wt1;
	    l = 2;
	    break;
	case 2:
	    out[0] = s_wt2;
	    out[1] = wt2;
	    l = 2;
	    break;
	}
	break;
    }
    return l;
}
uint8_t B_BUC::cmdSet(uint8_t * req, uint8_t addr)
{
    return 0;
}

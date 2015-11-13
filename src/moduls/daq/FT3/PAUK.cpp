//OpenSCADA system module DAQ.FT3 file: PAUK.cpp
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
#include "PAUK.h"

using namespace FT3;

B_PAUK::B_PAUK(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params)
{
    mTypeFT3 = GRS;
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    mPrm.p_el.fldAdd(fld = new TFld("selection", _("Valve select"), TFld::Integer, TVal::DirWrite));
    fld->setReserve("0:1");
    mPrm.p_el.fldAdd(fld = new TFld("execution", _("Execution"), TFld::Integer, TVal::DirWrite));
    fld->setReserve("0:2");
    mPrm.p_el.fldAdd(fld = new TFld("control", _("Control"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:4");
    mPrm.p_el.fldAdd(fld = new TFld("errors", _("Errors"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:5");

    for(int i = 1; i <= count_n; i++) {
	mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("state_%d", i).c_str(), TSYS::strMess(_("Valve state %d"), i).c_str(), TFld::Integer, TFld::NoWrite));
	fld->setReserve(TSYS::strMess("%d:0", i));

	if(with_params) {
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("time_%d", i).c_str(), TSYS::strMess(_("Persistence time %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:1", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("astime_%d", i).c_str(), TSYS::strMess(_("Persistence astime %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:2", i));
	}
    }
}

B_PAUK::~B_PAUK()
{

}

string B_PAUK::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

uint16_t B_PAUK::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 13;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0);
	*((uint16_t *) (Msg.D + 2)) = PackID(ID, 0, 1);
	*((uint16_t *) (Msg.D + 4)) = PackID(ID, 0, 2);
	*((uint16_t *) (Msg.D + 6)) = PackID(ID, 0, 4);
	*((uint16_t *) (Msg.D + 8)) = PackID(ID, 0, 5);
	if(mPrm.owner().DoCmd(&Msg)) {
	    if(Msg.C == GOOD3) {
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 9;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = PackID(ID, i, 0); //состояние крана
			*((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 1); //время выдержки
			*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 2); //доп время выдержки
			if(mPrm.owner().DoCmd(&Msg)) {
			    if(Msg.C == GOOD3) {
				rc = 1;
			    } else {
				rc = 0;
				break;
			    }
			} else {
			    rc = 0;
			    break;
			}
		    }
		} else {
		    rc = 1;
		}
	    }
	}
	if(rc) NeedInit = false;
	break;
    }
    return rc;
}
uint16_t B_PAUK::HandleEvent(int64_t tm, uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    switch (ft3ID.k) {
    case 0:
	switch(ft3ID.n) {
	case 0:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt(TSYS::strMess("selection").c_str()).at().setI(D[3], tm, true);
	    l = 4;
	    break;
	case 2:
	    mPrm.vlAt(TSYS::strMess("execution").c_str()).at().setI(D[3], tm, true);
	    l = 4;
	    break;
	case 3:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3 + count_n / 2;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(((D[2 + (j + 1) / 2]) >> ((j % 2) * 4)) & 0x0F, tm, true);
	    }
	    l = 11;
	    break;
	case 4:
	    mPrm.vlAt(TSYS::strMess("control").c_str()).at().setI(D[2], tm, true);
	    l = 3;
	    break;
	case 5:
	    mPrm.vlAt(TSYS::strMess("errors").c_str()).at().setI(D[2], tm, true);
	    l = 3;
	    break;

	}
	break;
    default:
	if(ft3ID.k && (ft3ID.k <= count_n)) {
	    switch(ft3ID.n) {
	    case 0:
		mPrm.vlAt(TSYS::strMess("state_%d", ft3ID.k).c_str()).at().setI(D[2], tm, true);
		l = 3;
		break;

	    case 1:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("time_%d", ft3ID.k).c_str()).at().setI(D[3], tm, true);
		}
		l = 4;
		break;
	    case 2:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("astime_%d", ft3ID.k).c_str()).at().setI(D[3], tm, true);
		    ;
		}
		l = 4;
		break;

	    }
	}
	break;
    }
    return l;
}

uint16_t B_PAUK::setVal(TVal &val)
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
    switch(ft3ID.n) {
    case 1:
    case 2:
	Msg.L = 6;
	Msg.D[2] = val.get(NULL, true).getI();
	if((ft3ID.n == 2) && (Msg.D[2] != 0x55)) {
	    Msg.D[2] = 0;
	}
	mPrm.owner().DoCmd(&Msg);
	break;
    }

    return 0;
}

//---------------------------------------------------------------------------

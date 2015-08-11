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
	DA(prm), ID(id << 12), count_n(n), with_params(has_params) //, numReg(0)
{
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
	*((uint16_t *) Msg.D) = ID | (0 << 6) | (0); //состояние
	*((uint16_t *) (Msg.D + 2)) = ID | (0 << 6) | (1); //выбор крана
	*((uint16_t *) (Msg.D + 4)) = ID | (0 << 6) | (2); //исполнение
	*((uint16_t *) (Msg.D + 6)) = ID | (0 << 6) | (4); //управление
	*((uint16_t *) (Msg.D + 8)) = ID | (0 << 6) | (5); //ошибки
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		//mess_info(mPrm.nodePath().c_str(),_("Data"));
		mPrm.vlAt("state").at().setI(Msg.D[7], 0, true);
		mPrm.vlAt("selection").at().setI(Msg.D[12], 0, true);
		mPrm.vlAt("execution").at().setI(Msg.D[18], 0, true);
		mPrm.vlAt("control").at().setI(Msg.D[24], 0, true);
		mPrm.vlAt("errors").at().setI(Msg.D[29], 0, true);

		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 9;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = ID | (i << 6) | (0); //состояние крана
			*((uint16_t *) (Msg.D + 2)) = ID | (i << 6) | (1); //время выдержки
			*((uint16_t *) (Msg.D + 4)) = ID | (i << 6) | (2); //доп время выдержки

			if(mPrm.owner().Transact(&Msg)) {
			    if(Msg.C == GOOD3) {
				mPrm.vlAt(TSYS::strMess("state_%d", i).c_str()).at().setI(Msg.D[7], 0, true);
				mPrm.vlAt(TSYS::strMess("time_%d", i).c_str()).at().setI(Msg.D[13], 0, true);
				mPrm.vlAt(TSYS::strMess("astime_%d", i).c_str()).at().setI(Msg.D[19], 0, true);
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
uint16_t B_PAUK::HandleEvent(uint8_t * D)
{
    if((TSYS::getUnalign16(D) & 0xF000) != ID) return 0;
    uint16_t l = 0;
    uint16_t k = (TSYS::getUnalign16(D) >> 6) & 0x3F; // номер объекта
    uint16_t n = TSYS::getUnalign16(D) & 0x3F;  // номер параметра

    switch(k) {
    case 0:
	switch(n) {
	case 0:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt(TSYS::strMess("selection").c_str()).at().setI(D[3], 0, true);
	    l = 4;
	    break;
	case 2:
	    mPrm.vlAt(TSYS::strMess("execution").c_str()).at().setI(D[3], 0, true);
	    l = 4;
	    break;
	case 3:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3 + count_n / 2;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(((D[2 + (j + 1) / 2]) >> ((j % 2) * 4)) & 0x0F, 0, true);
	    }
	    l = 11;
	    break;
	case 4:
	    mPrm.vlAt(TSYS::strMess("control").c_str()).at().setI(D[2], 0, true);
	    l = 3;
	    break;
	case 5:
	    mPrm.vlAt(TSYS::strMess("errors").c_str()).at().setI(D[2], 0, true);
	    l = 3;
	    break;

	}
	break;
    default:
	if(k && (k <= count_n)) {
	    switch(n) {
	    case 0:
		mPrm.vlAt(TSYS::strMess("state_%d", k).c_str()).at().setI(D[2], 0, true);
		l = 3;
		break;

	    case 1:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("time_%d", k).c_str()).at().setI(D[3], 0, true);
		}
		l = 4;
		break;
	    case 2:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("astime_%d", k).c_str()).at().setI(D[3], 0, true);
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
    uint16_t k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер объекта
    uint16_t n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер параметра
    uint16_t addr = ID | (k << 6) | n;
    tagMsg Msg;
    switch(n) {
    case 1:
    case 2:
	Msg.L = 6;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	Msg.D[2] = val.get(NULL, true).getI();
	if((n == 2) && (Msg.D[2] != 0x55)) {
	    Msg.D[2] = 0;
	}
	mPrm.owner().Transact(&Msg);
	break;
    }

    return 0;
}

//---------------------------------------------------------------------------

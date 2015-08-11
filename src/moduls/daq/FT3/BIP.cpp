//OpenSCADA system module DAQ.FT3 file: BIP.cpp
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
#include "BIP.h"

using namespace FT3;

B_BIP::B_BIP(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id << 12), count_n(n), with_params(has_params)
{
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");

    for(int i = 1; i <= count_n; i++) {
	mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("state_%d", i).c_str(), TSYS::strMess(_("State %d"), i).c_str(), TFld::Integer, TFld::NoWrite));
	fld->setReserve(TSYS::strMess("%d:0", i));
	mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("value_%d", i).c_str(), TSYS::strMess(_("Value %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	fld->setReserve(TSYS::strMess("%d:1", i));
	if(with_params) {
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("period_%d", i).c_str(), TSYS::strMess(_("Measure period %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:2", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("sens_%d", i).c_str(), TSYS::strMess(_("Sensitivity %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:3", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("minW_%d", i).c_str(), TSYS::strMess(_("Warning minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:4", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("maxW_%d", i).c_str(), TSYS::strMess(_("Warning maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:4", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("minA_%d", i).c_str(), TSYS::strMess(_("Alarm minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("maxA_%d", i).c_str(), TSYS::strMess(_("Alarm maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("dimens_%d", i).c_str(), TSYS::strMess(_("Dimension %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("minPV_%d", i).c_str(), TSYS::strMess(_("PV minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:7", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("maxPV_%d", i).c_str(), TSYS::strMess(_("PV maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:7", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("minF_%d", i).c_str(), TSYS::strMess(_("Minimum frequency %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:8", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("maxF_%d", i).c_str(), TSYS::strMess(_("Maximum frequency %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:8", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("factor_%d", i).c_str(), TSYS::strMess(_("Factor %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:9", i));
	}
    }

}

B_BIP::~B_BIP()
{

}

string B_BIP::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

uint16_t B_BIP::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 5;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = ID | (0 << 6) | (0); //состояние
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		mPrm.vlAt("state").at().setI(Msg.D[7], 0, true);
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 21;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = ID | (i << 6) | (1); //Значение
			*((uint16_t *) (Msg.D + 2)) = ID | (i << 6) | (2); //Период измерений
			*((uint16_t *) (Msg.D + 4)) = ID | (i << 6) | (3); //Чувствительность
			*((uint16_t *) (Msg.D + 6)) = ID | (i << 6) | (4); //min max предупредительный
			*((uint16_t *) (Msg.D + 8)) = ID | (i << 6) | (5); //min max аварийный
			*((uint16_t *) (Msg.D + 10)) = ID | (i << 6) | (6); //Размерность
			*((uint16_t *) (Msg.D + 12)) = ID | (i << 6) | (7); //min max ФВ
			*((uint16_t *) (Msg.D + 14)) = ID | (i << 6) | (8); //min max частоты
			*((uint16_t *) (Msg.D + 16)) = ID | (i << 6) | (9); //Коэффициент
			if(mPrm.owner().Transact(&Msg)) {
			    if(Msg.C == GOOD3) {
				mPrm.vlAt(TSYS::strMess("state_%d", i).c_str()).at().setI(Msg.D[7], 0, true);
				mPrm.vlAt(TSYS::strMess("value_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 8), 0, true);
				mPrm.vlAt(TSYS::strMess("period_%d", i).c_str()).at().setI(Msg.D[17], 0, true);
				mPrm.vlAt(TSYS::strMess("sens_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 23), 0, true);
				mPrm.vlAt(TSYS::strMess("minW_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 32), 0, true);
				mPrm.vlAt(TSYS::strMess("maxW_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 36), 0, true);
				mPrm.vlAt(TSYS::strMess("minA_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 45), 0, true);
				mPrm.vlAt(TSYS::strMess("maxA_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 49), 0, true);
				mPrm.vlAt(TSYS::strMess("dimens_%d", i).c_str()).at().setI(Msg.D[58], 0, true);
				mPrm.vlAt(TSYS::strMess("minPV_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 64), 0, true);
				mPrm.vlAt(TSYS::strMess("maxPV_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 68), 0, true);
				mPrm.vlAt(TSYS::strMess("minF_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 77), 0, true);
				mPrm.vlAt(TSYS::strMess("maxF_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 81), 0, true);
				mPrm.vlAt(TSYS::strMess("factor_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 90), 0, true);
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

uint16_t B_BIP::HandleEvent(uint8_t * D)
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
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], 0, true);
		mPrm.vlAt(TSYS::strMess("value_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), 0, true);
	    }
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
		mPrm.vlAt(TSYS::strMess("state_%d", k).c_str()).at().setI(D[2], 0, true);
		mPrm.vlAt(TSYS::strMess("value_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		l = 7;
		break;
	    case 2:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("period_%d", k).c_str()).at().setI(D[3], 0, true);
		}
		l = 4;
		break;
	    case 3:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("sens_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 4:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minW_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxW_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 5:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 6:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("dimens_%d", k).c_str()).at().setI(D[3], 0, true);
		}
		l = 4;
		break;
	    case 7:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minPV_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxPV_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 8:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minF_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxF_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 9:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("factor_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    }
	}
	break;
    }
    return l;
}

uint16_t B_BIP::setVal(TVal &val)
{
    int off = 0;
    uint16_t k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер объекта
    uint16_t n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер параметра
    uint16_t addr = ID | (k << 6) | n;
    tagMsg Msg;
    switch(n) {
    case 2:
    case 6:
	Msg.L = 6;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	Msg.D[2] = val.get(NULL, true).getI();
	mPrm.owner().Transact(&Msg);
	break;
    case 3:
    case 9:
	Msg.L = 9;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	mPrm.owner().Transact(&Msg);
	break;
    case 4:
	Msg.L = 13;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minW_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxW_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 5:
	Msg.L = 13;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minA_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxA_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 7:
	Msg.L = 13;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minPV_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxPV_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 8:
	Msg.L = 13;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minF_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxF_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    }
    return 0;
}

//---------------------------------------------------------------------------

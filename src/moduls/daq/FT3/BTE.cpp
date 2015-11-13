//OpenSCADA system module DAQ.BTE file: BTE.cpp
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
#include "BTE.h"

using namespace FT3;

B_BTE::B_BTE(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params)

{
    mTypeFT3 = GRS;
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
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("alarms_%d", i).c_str(), TSYS::strMess(_("Alarms %d"), i).c_str(), TFld::Boolean, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:4", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("minW_%d", i).c_str(), TSYS::strMess(_("Warning minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("maxW_%d", i).c_str(), TSYS::strMess(_("Warning maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("minA_%d", i).c_str(), TSYS::strMess(_("Alarm minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("maxA_%d", i).c_str(), TSYS::strMess(_("Alarm maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("Ki_%d", i).c_str(), TSYS::strMess(_("Ki %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:7", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("Ku_%d", i).c_str(), TSYS::strMess(_("Ku %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:8", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("A_%d", i).c_str(), TSYS::strMess(_("A %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:9", i));
	}
    }

}

B_BTE::~B_BTE()
{

}

string B_BTE::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

uint16_t B_BTE::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 5;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0); //state
	if(mPrm.owner().DoCmd(&Msg)) {
	    if(Msg.C == GOOD3) {
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 21;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = PackID(ID, i, 1);
			*((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 2);
			*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 3);
			*((uint16_t *) (Msg.D + 6)) = PackID(ID, i, 4);
			*((uint16_t *) (Msg.D + 8)) = PackID(ID, i, 5);
			*((uint16_t *) (Msg.D + 10)) = PackID(ID, i, 6);
			*((uint16_t *) (Msg.D + 12)) = PackID(ID, i, 7);
			*((uint16_t *) (Msg.D + 14)) = PackID(ID, i, 8);
			*((uint16_t *) (Msg.D + 16)) = PackID(ID, i, 9);
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

uint16_t B_BTE::HandleEvent(int64_t tm, uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    switch(ft3ID.k) {
    case 0:
	switch(ft3ID.n) {
	case 0:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], tm, true);
		mPrm.vlAt(TSYS::strMess("value_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), tm, true);
	    }
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
		mPrm.vlAt(TSYS::strMess("state_%d", ft3ID.k).c_str()).at().setI(D[2], tm, true);
		mPrm.vlAt(TSYS::strMess("value_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);

		l = 7;
		break;
	    case 2:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("period_%d", ft3ID.k).c_str()).at().setI(TSYS::getUnalign16(D + 3), tm, true);
		}
		l = 5;
		break;
	    case 3:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("sens_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		}
		l = 7;
		break;
	    case 4:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("alarms_%d", ft3ID.k).c_str()).at().setB(D[3], tm, true);
		}
		l = 4;
		break;
	    case 5:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minW_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		    mPrm.vlAt(TSYS::strMess("maxW_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), tm, true);
		}
		l = 11;
		break;
	    case 6:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minA_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		    mPrm.vlAt(TSYS::strMess("maxA_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), tm, true);
		}
		l = 11;
		break;
	    case 7:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("Ki_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		}
		l = 7;
		break;
	    case 8:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("Ku_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		}
		l = 7;
		break;
	    case 9:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("A_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		}
		l = 7;
		break;
	    }
	}
	break;
    }
    return l;
}

uint16_t B_BTE::setVal(TVal &val)
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
    case 2:
	Msg.L = 7;
	*(uint16_t *) (Msg.D + 2) = (uint16_t) val.get(NULL, true).getR();
	break;
    case 4:
	Msg.L = 6;
	Msg.D[2] = val.get(NULL, true).getB();
	break;
    case 3:
    case 7:
    case 8:
    case 9:
	Msg.L = 9;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	break;
    case 5:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minW_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxW_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 6:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minA_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxA_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    }
    if(Msg.L) mPrm.owner().DoCmd(&Msg);
    return 0;
}

//---------------------------------------------------------------------------

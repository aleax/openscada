//OpenSCADA system module DAQ.BVI file: BVI.cpp
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
#include "BVI.h"

using namespace FT3;

B_BVI::B_BVI(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params, bool has_ext_period) :
	DA(prm), ID(id), count_n(n), with_params(has_params), ext_period(has_ext_period)
{
    mTypeFT3 = GRS;
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    for(int i = 0; i < count_n; i++) {
	AddChannel(i);
    }
    loadIO(true);
}

B_BVI::~B_BVI()
{

}

void B_BVI::AddChannel(uint8_t iid)
{
    data.push_back(STIchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:0", iid + 1));
    AddAttr(data.back().Value.lnk, TFld::Real, TFld::NoWrite, TSYS::strMess("%d:1", iid + 1));
    if(with_params) {
	AddAttr(data.back().Period.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Sens.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:3", iid + 1));
	AddAttr(data.back().Count.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().Factor.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:5", iid + 1));
	AddAttr(data.back().Dimension.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:6", iid + 1));
    }
}

string B_BVI::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}
void B_BVI::loadIO(bool force)
{
    //Load links
//    mess_info("B_BVT::loadIO", "");
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws
    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].Value.lnk);
	loadLnk(data[i].Period.lnk);
	loadLnk(data[i].Sens.lnk);
	loadLnk(data[i].Count.lnk);
	loadLnk(data[i].Factor.lnk);
	loadLnk(data[i].Dimension.lnk);
    }

}

void B_BVI::saveIO()
{
    //Save links
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].Value.lnk);
	saveLnk(data[i].Period.lnk);
	saveLnk(data[i].Sens.lnk);
	saveLnk(data[i].Count.lnk);
	saveLnk(data[i].Factor.lnk);
	saveLnk(data[i].Dimension.lnk);
    }
}

void B_BVI::tmHandler(void)
{
    for(int i = 0; i < count_n; i++) {
	uint8_t tmpui8;
	union
	{
	    uint8_t b[4];
	    float f;
	} tmpfl, tmpfl1;
	if(with_params) {
	    if (ext_period) {
		UpdateParamW(data[i].Period, PackID(ID, (i + 1), 2), 1);
	    } else {
		UpdateParam8(data[i].Period, PackID(ID, (i + 1), 2), 1);
	    }
	    UpdateParamFl(data[i].Sens, PackID(ID, (i + 1), 3), 1);
	    UpdateParam32(data[i].Count, PackID(ID, (i + 1), 4), 0);
	    UpdateParamFl(data[i].Factor, PackID(ID, (i + 1), 5), 1);
	    UpdateParam8(data[i].Dimension, PackID(ID, (i + 1), 6), 1);
	}
	UpdateParamFlState(data[i].Value, data[i].State, data[i].Sens, PackID(ID, (i + 1), 1), 2);
    }
    NeedInit = false;
}

uint16_t B_BVI::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 5;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0); //состояние
	if(mPrm.owner().DoCmd(&Msg)) {
	    if(Msg.C == GOOD3) {
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 15;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = PackID(ID, i, 1); //Значение ТИ
			*((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 2); //Период измерений
			*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 3); //Чувствительность
			*((uint16_t *) (Msg.D + 6)) = PackID(ID, i, 4); //Счетчик импульсов
			*((uint16_t *) (Msg.D + 8)) = PackID(ID, i, 5); //Коэффициент
			*((uint16_t *) (Msg.D + 10)) = PackID(ID, i, 6); //Размерность
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
		}
	    }
	}
	if(rc) NeedInit = false;
	break;
    }
    return rc;
}
uint16_t B_BVI::HandleEvent(int64_t tm, uint8_t * D)
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
		mPrm.vlAt(TSYS::strMess("TI_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), tm, true);
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
		mPrm.vlAt(TSYS::strMess("TI_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);

		l = 7;
		break;
	    case 2:
		if(with_params) {
		    if (ext_period) {
			mPrm.vlAt(TSYS::strMess("period_%d", ft3ID.k).c_str()).at().setI(TSYS::getUnalign16(D + 3), tm, true);
			l = 5;
		    } else {
			mPrm.vlAt(TSYS::strMess("period_%d", ft3ID.k).c_str()).at().setI(D[3], tm, true);
			l = 4;
		    }
		}
		break;
	    case 3:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("sens_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		}
		l = 7;
		break;
	    case 4:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("countP_%d", ft3ID.k).c_str()).at().setI(TSYS::getUnalign32(D + 3), tm, true);
		}
		l = 7;
		break;
	    case 5:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("factor_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		}
		l = 7;
		break;
	    case 6:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("dimens_%d", ft3ID.k).c_str()).at().setI(D[3], tm, true);
		}
		l = 4;
		break;
	    }
	}
	break;
    }
    return l;
}
uint8_t B_BVI::cmdGet(uint16_t prmID, uint8_t * out)
{
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 0:
	    //state
	    out[0] = 0;
	    l = 1;
	    break;
	case 1:

	    out[0] = 0;
	    l = 1;
	    //value
	    for(uint8_t i = 0; i < count_n; i++) {
		out[i * 5 + 1] = data[i].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[i * 5 + 2 + j] = data[i].Value.b_vl[j];
		l += 5;
	    }
	    break;
	}
    } else {
	if(ft3ID.k <= count_n) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = data[ft3ID.k - 1].State.vl;
		l = 1;
		break;
	    case 1:
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Value.b_vl[j];
		l = 5;
		break;
	    case 2:
		out[0] = data[ft3ID.k - 1].Period.s;
		if(ext_period) {
		    out[1] = data[ft3ID.k - 1].Period.vl;
		    out[2] = data[ft3ID.k - 1].Period.vl >> 8;
		    l = 3;
		} else {
		    out[1] = data[ft3ID.k - 1].Period.vl;
		    l = 2;
		}
		break;
	    case 3:
		out[0] = data[ft3ID.k - 1].Sens.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Sens.b_vl[j];
		l = 5;
		break;
	    case 4:
		out[0] = data[ft3ID.k - 1].Count.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Count.b_vl[j];
		l = 5;
		break;
	    case 5:
		out[0] = data[ft3ID.k - 1].Factor.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].Factor.b_vl[j];
		}
		l = 5;
		break;
	    case 6:
		out[0] = data[ft3ID.k - 1].Dimension.s;
		out[1] = data[ft3ID.k - 1].Dimension.vl;
		l = 2;
		break;
	    }
	}
    }
    return l;
}

uint8_t B_BVI::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", k, n);
    if((ft3ID.k > 0) && (ft3ID.k <= count_n)) {
	switch(ft3ID.n) {
	case 2:
	    if(ext_period) {
		l = SetNewWVal(data[ft3ID.k - 1].Period, addr, prmID, req[2]);
	    } else {
		l = SetNew8Val(data[ft3ID.k - 1].Period, addr, prmID, req[2]);
	    }
	    break;
	case 3:
	    l = SetNewflVal(data[ft3ID.k - 1].Sens, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 4:
	    l = SetNew32Val(data[ft3ID.k - 1].Count, addr, prmID, TSYS::getUnalign32(req + 2));
	    break;
	case 5:
	    l = SetNewflVal(data[ft3ID.k - 1].Factor, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 6:
	    l = SetNew8Val(data[ft3ID.k - 1].Dimension, addr, prmID, req[2]);
	    break;
	}
    }
    return l;
}
uint16_t B_BVI::setVal(TVal &val)
{
    int off = 0;
    FT3ID ft3ID;
    ft3ID.k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0);
    ft3ID.n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0);
    ft3ID.g = ID;
    tagMsg Msg;
    Msg.L = 0;
    Msg.C = SetData;
    *((uint16_t *) Msg.D) = PackID(ft3ID);
    switch(ft3ID.n) {
    case 2:
	if(ext_period) {
	    Msg.L = 7;
	    *(uint16_t *) (Msg.D + 2) = (uint16_t) val.get(NULL, true).getI();
	} else {
	    Msg.L = 6;
	    Msg.D[2] = val.get(NULL, true).getI();
	}
	break;
    case 6:
	Msg.L = 6;
	Msg.D[2] = val.get(NULL, true).getI();
	break;
    case 3:
    case 5:
	Msg.L = 9;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	break;
    case 4:
	Msg.L = 9;
	*(uint32_t *) (Msg.D + 2) = val.get(NULL, true).getI();
	break;
    }
    if(Msg.L) mPrm.owner().DoCmd(&Msg);
    return 0;
}

//---------------------------------------------------------------------------

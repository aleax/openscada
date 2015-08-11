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
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");

    for(int i = 0; i < count_n; i++) {
	data.push_back(STIchannel(i));
	mPrm.p_el.fldAdd(fld = new TFld(data[i].State.lnk.prmName.c_str(), data[i].State.lnk.prmDesc.c_str(), TFld::Integer, TFld::NoWrite));
	fld->setReserve(TSYS::strMess("%d:0", i + 1));
	mPrm.p_el.fldAdd(fld = new TFld(data[i].Value.lnk.prmName.c_str(), data[i].Value.lnk.prmDesc.c_str(), TFld::Real, TFld::NoWrite));
	fld->setReserve(TSYS::strMess("%d:1", i + 1));
	if(with_params) {
	    mPrm.p_el.fldAdd(fld = new TFld(data[i].Period.lnk.prmName.c_str(), data[i].Period.lnk.prmDesc.c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:2", i + 1));
	    mPrm.p_el.fldAdd(fld = new TFld(data[i].Sens.lnk.prmName.c_str(), data[i].Sens.lnk.prmDesc.c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:3", i + 1));
	    mPrm.p_el.fldAdd(fld = new TFld(data[i].Count.lnk.prmName.c_str(), data[i].Count.lnk.prmDesc.c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:4", i + 1));
	    mPrm.p_el.fldAdd(fld = new TFld(data[i].Factor.lnk.prmName.c_str(), data[i].Factor.lnk.prmDesc.c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i + 1));
	    mPrm.p_el.fldAdd(fld = new TFld(data[i].Dimension.lnk.prmName.c_str(), data[i].Dimension.lnk.prmDesc.c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i + 1));
	}
    }
    loadIO(true);
}

B_BVI::~B_BVI()
{

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

    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk, io_bd, io_table, cfg);
	loadLnk(data[i].Value.lnk, io_bd, io_table, cfg);
	loadLnk(data[i].Period.lnk, io_bd, io_table, cfg);
	loadLnk(data[i].Sens.lnk, io_bd, io_table, cfg);
	loadLnk(data[i].Count.lnk, io_bd, io_table, cfg);
	loadLnk(data[i].Factor.lnk, io_bd, io_table, cfg);
	loadLnk(data[i].Dimension.lnk, io_bd, io_table, cfg);
    }

}

void B_BVI::saveIO()
{
    //Save links
    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk, io_bd, io_table, cfg);
	saveLnk(data[i].Value.lnk, io_bd, io_table, cfg);
	saveLnk(data[i].Period.lnk, io_bd, io_table, cfg);
	saveLnk(data[i].Sens.lnk, io_bd, io_table, cfg);
	saveLnk(data[i].Count.lnk, io_bd, io_table, cfg);
	saveLnk(data[i].Factor.lnk, io_bd, io_table, cfg);
	saveLnk(data[i].Dimension.lnk, io_bd, io_table, cfg);
    }
}

void B_BVI::tmHandler(void)
{
    NeedInit = false;
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
	    UpdateParam32(data[i].Count, PackID(ID, (i + 1), 4), 1);
	    UpdateParamFl(data[i].Factor, PackID(ID, (i + 1), 5), 1);
	    UpdateParam8(data[i].Dimension, PackID(ID, (i + 1), 6), 1);
	}
	UpdateParamFlState(data[i].Value, data[i].State, PackID(ID, (i + 1), 1), 1);
    }
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
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		mPrm.vlAt("state").at().setI(Msg.D[7], 0, true);
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
			if(mPrm.owner().Transact(&Msg)) {
			    if(Msg.C == GOOD3) {
				mPrm.vlAt(TSYS::strMess("state_%d", i).c_str()).at().setI(Msg.D[7], 0, true);
				mPrm.vlAt(TSYS::strMess("TI_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 8), 0, true);
				mPrm.vlAt(TSYS::strMess("period_%d", i).c_str()).at().setI(Msg.D[17], 0, true);
				mPrm.vlAt(TSYS::strMess("sens_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 23), 0, true);
				mPrm.vlAt(TSYS::strMess("countP_%d", i).c_str()).at().setI(TSYS::getUnalign32(Msg.D + 32), 0, true);
				mPrm.vlAt(TSYS::strMess("factor_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 41), 0, true);
				mPrm.vlAt(TSYS::strMess("dimens_%d", i).c_str()).at().setI(Msg.D[50], 0, true);
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
uint16_t B_BVI::HandleEvent(uint8_t * D)
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
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], 0, true);
		mPrm.vlAt(TSYS::strMess("TI_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), 0, true);
	    }
	    break;

	}
	break;
    default:
	if(ft3ID.k && (ft3ID.k <= count_n)) {
	    switch(ft3ID.n) {
	    case 0:
		mPrm.vlAt(TSYS::strMess("state_%d", ft3ID.k).c_str()).at().setI(D[2], 0, true);
		l = 3;
		break;
	    case 1:
		mPrm.vlAt(TSYS::strMess("state_%d", ft3ID.k).c_str()).at().setI(D[2], 0, true);
		mPrm.vlAt(TSYS::strMess("TI_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);

		l = 7;
		break;
	    case 2:
		if(with_params) {
		    if (ext_period) {
			mPrm.vlAt(TSYS::strMess("period_%d", ft3ID.k).c_str()).at().setI(TSYS::getUnalign16(D + 3), 0, true);
			l = 5;
		    } else {
			mPrm.vlAt(TSYS::strMess("period_%d", ft3ID.k).c_str()).at().setI(D[3], 0, true);
			l = 4;
		    }
		}
		break;
	    case 3:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("sens_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 4:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("countP_%d", ft3ID.k).c_str()).at().setI(TSYS::getUnalign32(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 5:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("factor_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 6:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("dimens_%d", ft3ID.k).c_str()).at().setI(D[3], 0, true);
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
    if((prmID & 0xF000) != ID) return 0;
    uint16_t k = (prmID >> 6) & 0x3F; // object
    uint16_t n = prmID & 0x3F;  // param
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", k, n);
    if((k > 0) && (k <= count_n)) {
	switch(n) {
	case 2:
	    if(ext_period) {
		l = SetNewWVal(data[k - 1].Period, addr, prmID, req[2]);
	    } else {
		l = SetNew8Val(data[k - 1].Period, addr, prmID, req[2]);
	    }
	    break;
	case 3:
	    l = SetNewflVal(data[k - 1].Sens, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 4:
	    l = SetNew32Val(data[k - 1].Count, addr, prmID, TSYS::getUnalign32(req + 2));
	    break;
	case 5:
	    l = SetNewflVal(data[k - 1].Factor, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 6:
	    l = SetNew8Val(data[k - 1].Dimension, addr, prmID, req[2]);
	    break;
	}
    }
    return l;
}
uint16_t B_BVI::setVal(TVal &val)
{
    int off = 0;
    uint16_t k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер объекта
    uint16_t n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер параметра
    uint16_t addr = ID | (k << 6) | n;

    tagMsg Msg;
    switch(n) {
    case 2:
	if(ext_period) {
	    Msg.L = 7;
	    Msg.C = SetData;
	    Msg.D[0] = addr & 0xFF;
	    Msg.D[1] = (addr >> 8) & 0xFF;
	    *(uint16_t *) (Msg.D + 2) = (uint16_t) val.get(NULL, true).getI();
	    mPrm.owner().Transact(&Msg);
	} else {
	    Msg.L = 6;
	    Msg.C = SetData;
	    Msg.D[0] = addr & 0xFF;
	    Msg.D[1] = (addr >> 8) & 0xFF;
	    Msg.D[2] = val.get(NULL, true).getI();
	    mPrm.owner().Transact(&Msg);
	}
	break;
    case 6:
	Msg.L = 6;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	Msg.D[2] = val.get(NULL, true).getI();
	mPrm.owner().Transact(&Msg);
	break;
    case 3:
    case 5:
	Msg.L = 9;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	mPrm.owner().Transact(&Msg);
	break;
    case 4:
	Msg.L = 9;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(uint32_t *) (Msg.D + 2) = val.get(NULL, true).getI();
	mPrm.owner().Transact(&Msg);
	break;
    }
    return 0;
}

//---------------------------------------------------------------------------

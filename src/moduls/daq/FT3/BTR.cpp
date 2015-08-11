//OpenSCADA system module DAQ.FT3 file: BTR.cpp
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
#include "BTR.h"

using namespace FT3;

KA_BTU::KA_BTU(TMdPrm& prm, uint16_t id, uint16_t nu, bool has_params) :
	DA(prm), ID(id), count_nu(nu), with_params(has_params), config(4 | (nu << 4) | (16 << 10))

{
    mTypeFT3 = KA;
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    for(int i = 0; i < count_nu; i++) {
	AddChannel(i);
    }
    loadIO(true);
}

KA_BTU::~KA_BTU()
{
    TUdata.clear();
}

void KA_BTU::AddChannel(uint8_t iid)
{
    TUdata.push_back(SKATUchannel(iid, this));
    AddAttr(TUdata.back().Line.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:0", iid + 1));
    if(with_params) {
	for(int i = 0; i < 16; i++) {
	    AddAttr(TUdata.back().Time[i].lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	}
    }
}

string KA_BTU::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

void KA_BTU::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws

    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    for(int i = 0; i < count_nu; i++) {
	loadLnk(TUdata[i].Line.lnk, io_bd, io_table, cfg);
	for(int j = 0; j < 16; j++) {
	    loadLnk(TUdata[i].Time[j].lnk, io_bd, io_table, cfg);
	}
    }

}

void KA_BTU::saveIO()
{
    //Save links
    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    for(int i = 0; i < count_nu; i++) {
	saveLnk(TUdata[i].Line.lnk, io_bd, io_table, cfg);
	for(int j = 0; j < 16; j++) {
	    saveLnk(TUdata[i].Time[j].lnk, io_bd, io_table, cfg);
	}
    }
}

void KA_BTU::tmHandler(void)
{
    NeedInit = false;
    for(int i = 0; i < count_nu; i++) {
	for(int j = 0; j < 16; j++) {
	    UpdateParamW(TUdata[i].Time[j], PackID(ID, i + 1, 1), 1);
	}
    }
}

uint16_t KA_BTU::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    return rc;
}
uint16_t KA_BTU::HandleEvent(uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    return l;
}

uint8_t KA_BTU::cmdGet(uint16_t prmID, uint8_t * out)
{
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 0:
	    out[0] = 0;
	    l = 1;
	    break;
	case 1:
	    out[0] = config & 0xFF;
	    out[1] = (config >> 8) & 0xFF;
	    l = 2;
	    break;
	case 2:
	    out[0] = 0;
	    out[1] = 0;
	    l = 2;
	    break;
	}
    } else {
	if(count_nu && (ft3ID.k <= count_nu)) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = TUdata[ft3ID.k - 1].Line.s;
		out[1] = TUdata[ft3ID.k - 1].iTY;
		l = 2;
		break;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
		out[0] = TUdata[ft3ID.k - 1].Time[ft3ID.n - 1].s;
		out[1] = TUdata[ft3ID.k - 1].Time[ft3ID.n - 1].vl;
		out[2] = TUdata[ft3ID.k - 1].Time[ft3ID.n - 1].vl >> 8;
		l = 3;
		break;
	    case 32:
		out[0] = TUdata[ft3ID.k - 1].Line.s;
		out[1] = TUdata[ft3ID.k - 1].Line.vl;
		out[1] = TUdata[ft3ID.k - 1].Line.vl >> 8;
		l = 3;
		break;
	    }
	}
    }
    return l;
}

uint8_t KA_BTU::runTU(uint8_t tu)
{
    uint8_t rc = 0;
    if(tu == 0x55) {
	for(int i = 0; i < count_nu; i++) {
	    if(TUdata[i].iTY) {
		TUdata[i].Line.lnk.aprm.at().setI(TUdata[i].Line.vl);
		TUdata[i].Line.vl = 0;
		TUdata[i].iTY = 0;
		rc = 3;
	    }
	}
    }
    if(tu == 0x0) {
	for(int i = 0; i < count_nu; i++) {
	    TUdata[i].Line.vl = 0;
	    TUdata[i].iTY = 0;
	    TUdata[i].Line.lnk.aprm.at().setI(TUdata[i].Line.vl);
	    rc = 3;
	}
    }
    return rc;
}

uint8_t KA_BTU::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    uint8_t E[2];
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 2:
	    l = runTU(req[2]);
	    if(l) {
		uint8_t E[2] = { addr, req[2]};
		mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	    }
	    break;
	}
    } else {
	if(count_nu && (ft3ID.k <= count_nu)) {
	    switch(ft3ID.n) {
	    case 0:
		TUdata[ft3ID.k - 1].Line.s = addr;
		TUdata[ft3ID.k - 1].iTY = req[2];
		TUdata[ft3ID.k - 1].Line.vl = 1 << TUdata[ft3ID.k - 1].iTY - 1;
		l = 3;
		E[0] = addr;
		E[1] = req[2];
		mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
		break;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
		l = SetNewWVal(TUdata[ft3ID.k - 1].Time[ft3ID.n - 1], addr, prmID, TSYS::getUnalign16(req + 2));
		break;
	    case 32:
		TUdata[ft3ID.k - 1].Line.s = addr;
		TUdata[ft3ID.k - 1].iTY = 1;
		TUdata[ft3ID.k - 1].Line.vl = TSYS::getUnalign16(req + 2);
		l = 3;
		E[0] = addr;
		E[1] = req[2];
		mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
		break;
	    }
	}
    }
    return l;
}

uint16_t KA_BTU::setVal(TVal &val)
{
    return 0;
}

B_BTR::B_BTR(TMdPrm& prm, uint16_t id, uint16_t nu, uint16_t nr, bool has_params) :
	DA(prm), ID(id), count_nu(nu), count_nr(nr), with_params(has_params)

{
    TFld * fld;

    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    if(count_nu) {
	mPrm.p_el.fldAdd(fld = new TFld("selection", _("Select TU"), TFld::Integer, TVal::DirWrite));
	fld->setReserve("0:1");
	mPrm.p_el.fldAdd(fld = new TFld("execution", _("Execution"), TFld::Integer, TVal::DirWrite));
	fld->setReserve("0:2");
    }

    for(int i = 0; i < count_nu; i++) {
	TUdata.push_back(STUchannel(i));
	mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].On.lnk.prmName.c_str(), TUdata[i].On.lnk.prmDesc.c_str(), TFld::Real, TFld::NoWrite));
	mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].Off.lnk.prmName.c_str(), TUdata[i].Off.lnk.prmDesc.c_str(), TFld::Real, TFld::NoWrite));
	mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].Run.lnk.prmName.c_str(), TUdata[i].Run.lnk.prmDesc.c_str(), TFld::Real, TFld::NoWrite));
	mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].Reset.lnk.prmName.c_str(), TUdata[i].Reset.lnk.prmDesc.c_str(), TFld::Real, TFld::NoWrite));
	if(with_params) {
	    mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].Time.lnk.prmName.c_str(), TUdata[i].Time.lnk.prmDesc.c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:0", i + 1));
	    mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].TC.lnk.prmName.c_str(), TUdata[i].TC.lnk.prmDesc.c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:1", i + 1));
	    mPrm.p_el.fldAdd(fld = new TFld(TUdata[i].ExTime.lnk.prmName.c_str(), TUdata[i].ExTime.lnk.prmDesc.c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:2", i + 1));
	}
    }
    for(int i = 0; i < count_nr; i++) {
	TRdata.push_back(STRchannel(i));
	mPrm.p_el.fldAdd(fld = new TFld(TRdata[i].Value.lnk.prmName.c_str(), TRdata[i].Value.lnk.prmDesc.c_str(), TFld::Real, TVal::DirWrite));
	fld->setReserve(TSYS::strMess("%d:0", i + 1 + count_nu));
    }
    loadIO(true);
}

B_BTR::~B_BTR()
{

}

string B_BTR::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

void B_BTR::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws

    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    for(int i = 0; i < count_nu; i++) {
	loadLnk(TUdata[i].On.lnk, io_bd, io_table, cfg);
	loadLnk(TUdata[i].Off.lnk, io_bd, io_table, cfg);
	loadLnk(TUdata[i].Run.lnk, io_bd, io_table, cfg);
	loadLnk(TUdata[i].Reset.lnk, io_bd, io_table, cfg);
	loadLnk(TUdata[i].Time.lnk, io_bd, io_table, cfg);
	loadLnk(TUdata[i].TC.lnk, io_bd, io_table, cfg);
	loadLnk(TUdata[i].ExTime.lnk, io_bd, io_table, cfg);
    }
    for(int i = 0; i < count_nr; i++) {
	loadLnk(TRdata[i].Value.lnk, io_bd, io_table, cfg);
    }
}

void B_BTR::saveIO()
{
//Save links
    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    for(int i = 0; i < count_nu; i++) {
	saveLnk(TUdata[i].On.lnk, io_bd, io_table, cfg);
	saveLnk(TUdata[i].Off.lnk, io_bd, io_table, cfg);
	saveLnk(TUdata[i].Run.lnk, io_bd, io_table, cfg);
	saveLnk(TUdata[i].Reset.lnk, io_bd, io_table, cfg);
	saveLnk(TUdata[i].Time.lnk, io_bd, io_table, cfg);
	saveLnk(TUdata[i].TC.lnk, io_bd, io_table, cfg);
	saveLnk(TUdata[i].ExTime.lnk, io_bd, io_table, cfg);
    }
    for(int i = 0; i < count_nr; i++) {
	saveLnk(TRdata[i].Value.lnk, io_bd, io_table, cfg);
    }
}

void B_BTR::tmHandler(void)
{
    NeedInit = false;
    for(int i = 0; i < count_nu; i++) {
	UpdateParamW(TUdata[i].Time, PackID(ID, i + 1, 0), 1);
	UpdateParamW(TUdata[i].TC, PackID(ID, i + 1, 1), 1);
	UpdateParam8(TUdata[i].ExTime, PackID(ID, i + 1, 2), 1);
    }
    for(int i = 0; i < count_nr; i++) {
	UpdateParamFl(TRdata[i].Value, PackID(ID, i + 1 + count_nu, 0), 1);
    }
}

uint16_t B_BTR::Task(uint16_t uc)
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
		if(count_nu) {
		    Msg.L = 7;
		    Msg.C = AddrReq;
		    *((uint16_t *) (Msg.D)) = PackID(ID, 0, 1); //выбор ТУ
		    *((uint16_t *) (Msg.D + 2)) = PackID(ID, 0, 2); //исполнение
		    if(mPrm.owner().Transact(&Msg)) {
			if(Msg.C == GOOD3) {
			    mPrm.vlAt("selection").at().setI(Msg.D[8], 0, true);
			    mPrm.vlAt("execution").at().setI(Msg.D[14], 0, true);
			}
		    }
		    if(with_params) {
			for(int i = 1; i <= count_nu; i++) {
			    Msg.L = 9;
			    Msg.C = AddrReq;
			    *((uint16_t *) Msg.D) = PackID(ID, i, 0); //время выдержки
			    *((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 1); //ТС
			    *((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 2); //доп время выдержки

			    if(mPrm.owner().Transact(&Msg)) {
				if(Msg.C == GOOD3) {
				    mPrm.vlAt(TSYS::strMess("time_%d", i).c_str()).at().setR(TSYS::getUnalign16(Msg.D + 8), 0, true);
				    mPrm.vlAt(TSYS::strMess("tc_%d", i).c_str()).at().setI(TSYS::getUnalign16(Msg.D + 15), 0, true);
				    mPrm.vlAt(TSYS::strMess("extime_%d", i).c_str()).at().setR(Msg.D[22], 0, true);
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
		    }
		}
		if(count_nr) {
		    for(int i = 1; i <= count_nr; i++) {
			Msg.L = 5;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = PackID(ID, i + count_nu, 0); //уставка
			if(mPrm.owner().Transact(&Msg)) {
			    if(Msg.C == GOOD3) {
				mPrm.vlAt(TSYS::strMess("value_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 8), 0, true);
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
uint16_t B_BTR::HandleEvent(uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
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
	}
    }
    if(count_nu && (ft3ID.k <= count_nu)) {
	switch(ft3ID.n) {
	case 0:
	    mPrm.vlAt(TSYS::strMess("time_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalign16(D + 3), 0, true);
	    l = 5;
	    break;

	case 1:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("tc_%d", ft3ID.k).c_str()).at().setI(TSYS::getUnalign16(D + 3), 0, true);
	    }
	    l = 5;
	    break;
	case 2:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("extime_%d", ft3ID.k).c_str()).at().setR(D[3], 0, true);
		;
	    }
	    l = 4;
	    break;

	}
    }
    if(count_nr && ((ft3ID.k > count_nu) && (ft3ID.k <= count_nr + count_nu))) {
	switch(ft3ID.n) {
	case 0:
	    mPrm.vlAt(TSYS::strMess("value_%d", ft3ID.k - count_nu).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    l = 7;
	    break;

	}
    }
    return l;
}

uint8_t B_BTR::cmdGet(uint16_t prmID, uint8_t * out)
{
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 0:
	    out[0] = 0;
	    l = 1;
	    break;
	case 1:
	    out[0] = 0;
	    out[1] = 0;
	    l = 2;
	    break;
	case 2:
	    out[0] = 0;
	    out[1] = 0;
	    l = 2;
	    break;
	}
    } else {
	if(count_nu && (ft3ID.k <= count_nu)) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = TUdata[ft3ID.k - 1].Time.s;
		out[1] = ((uint16_t) TUdata[ft3ID.k - 1].Time.vl);
		out[2] = ((uint16_t) TUdata[ft3ID.k - 1].Time.vl) >> 8;
		l = 3;
		break;
	    case 1:
		out[0] = TUdata[ft3ID.k - 1].TC.s;
		out[1] = TUdata[ft3ID.k - 1].TC.vl;
		out[2] = TUdata[ft3ID.k - 1].TC.vl >> 8;
		l = 3;
		break;
	    case 2:
		out[0] = TUdata[ft3ID.k - 1].ExTime.s;
		out[1] = ((uint16_t) TUdata[ft3ID.k - 1].ExTime.vl);
		l = 2;
		break;
	    }
	}
	if(count_nr && ((ft3ID.k > count_nu) && (ft3ID.k <= count_nr + count_nu))) {
	    out[0] = TRdata[ft3ID.k - 1 - count_nu].Value.s;
	    for(uint8_t j = 0; j < 4; j++)
		out[1 + j] = TRdata[ft3ID.k - 1 - count_nu].Value.b_vl[j];
	    l = 5;
	}
    }
    return l;
}

uint8_t B_BTR::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 1:
	    setTU(req[2]);
	    l = 3;
	    break;
	case 2:
	    runTU(req[2]);
	    l = 3;
	    break;
	}
    } else {
	if(count_nu && (ft3ID.k <= count_nu)) {
	    switch(ft3ID.n) {
	    case 0:
		l = SetNewWVal(TUdata[ft3ID.k - 1].Time, addr, prmID, TSYS::getUnalign16(req + 2));
		break;
	    case 1:
		l = 3;
	    case 2:
		l = SetNew8Val(TUdata[ft3ID.k - 1].ExTime, addr, prmID, req[2]);
	    }
	}
	if(count_nr && ((ft3ID.k > count_nu) && (ft3ID.k <= count_nr + count_nu))) {
//	    mess_info(mPrm.nodePath().c_str(), "cmdSet Val");
	    l = SetNewflVal(TRdata[ft3ID.k - 1 - count_nu].Value, addr, prmID, TSYS::getUnalignFloat(req + 2));
	}
    }
    return l;
}

void B_BTR::setTU(uint8_t tu)
{
    uint8_t vl = tu >> 7;
    uint8_t n = tu & 0x7F;
    if((n > 0) && (n < count_nu)) {
	STUchannel & TU = TUdata[n - 1];
	currTU = n;
	if(vl) {
	    if(TU.On.lnk.Connected()) {
		//on
		TU.On.lnk.aprm.at().setI(1);
		mPrm.vlAt(TU.On.lnk.prmName.c_str()).at().setI(1, 0, true);
	    }
	} else {
	    if(TUdata[n - 1].Off.lnk.Connected()) {
		//off
		TU.Off.lnk.aprm.at().setI(1);
		mPrm.vlAt(TU.Off.lnk.prmName.c_str()).at().setI(1, 0, true);
	    }
	}
    }

}
void B_BTR::runTU(uint8_t tu)
{
    if(currTU) {
	STUchannel & TU = TUdata[currTU - 1];
	if(tu == 0x55) {
	    if((!TU.Run.lnk.Connected())) {
		TU.Run.lnk.aprm.at().setI(1);
		mPrm.vlAt(TU.Run.lnk.prmName.c_str()).at().setI(1, 0, true);
		currTU = 0;
	    }
	}
    }
    if(tu == 0x0) {
	for(int i = 0; i < count_nu; i++) {
	    STUchannel & TU = TUdata[i];
	    if((!TU.Reset.lnk.Connected())) {
		TU.Reset.lnk.aprm.at().setI(1);
		mPrm.vlAt(TU.Reset.lnk.prmName.c_str()).at().setI(1, 0, true);
		currTU = 0;
	    }
	}
    }
}

uint16_t B_BTR::setVal(TVal &val)
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
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 1:
	case 2:
	    Msg.L = 6;
	    Msg.D[2] = val.get(NULL, true).getI();
	    if((ft3ID.n == 2) && (Msg.D[2] != 0x55)) {
		Msg.D[2] = 0;
	    }
	    break;
	}
    } else {
	if(count_nu && (ft3ID.k <= count_nu)) {
	    switch(ft3ID.n) {
	    case 0:
		Msg.L = 7;
		*(uint16_t *) (Msg.D + 2) = (uint16_t) (val.get(NULL, true).getR());
		break;
	    case 1:
		Msg.L = 7;
		*(uint16_t *) (Msg.D + 2) = (uint16_t) val.get(NULL, true).getI();
		break;
	    case 2:
		Msg.L = 6;
		Msg.D[2] = val.get(NULL, true).getR();
		break;
	    }
	}
	if(count_nr && ((ft3ID.k > count_nu) && (ft3ID.k <= count_nr + count_nu))) {
	    switch(ft3ID.n) {
	    case 0:
		Msg.L = 9;
		*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
		break;
	    }
	}
    }
    if(Msg.L) mPrm.owner().Transact(&Msg);
    return 0;
}

//---------------------------------------------------------------------------

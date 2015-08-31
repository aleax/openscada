//OpenSCADA system module DAQ.FT3 file: BVT.cpp
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
#include "BVT.h"

using namespace FT3;

void KA_BVT::SKATTchannel::UpdateTTParam(uint16_t ID, uint8_t cl)
{
    ui8fl tmpfl[10];
    uint8_t tmp;
    tmp = Period.Get();
    tmpfl[0].f = Sens.Get();
    tmpfl[1].f = MinS.Get();
    tmpfl[2].f = MaxS.Get();
    tmpfl[3].f = MinPV.Get();
    tmpfl[4].f = MaxPV.Get();
    tmpfl[5].f = MinA.Get();
    tmpfl[6].f = MaxA.Get();
    tmpfl[7].f = MinW.Get();
    tmpfl[8].f = MaxW.Get();
    tmpfl[9].f = Factor.Get();
    tmpfl[10].f = Adjust.Get();
    if(tmp != Period.vl || tmpfl[0].f != Sens.vl || tmpfl[1].f != MinS.vl || tmpfl[2].f != MaxS.vl || tmpfl[3].f != MinPV.vl || tmpfl[4].f != MaxPV.vl
	    || tmpfl[5].f != MinA.vl || tmpfl[6].f != MaxA.vl || tmpfl[7].f != MinW.vl || tmpfl[8].f != MaxW.vl || tmpfl[9].f != Factor.vl
	    || tmpfl[10].f != Adjust.vl) {
	Period.s = 0;
	Period.Update(tmp);
	Sens.Update(tmpfl[0].f);
	MinS.Update(tmpfl[1].f);
	MaxS.Update(tmpfl[2].f);
	MinPV.Update(tmpfl[3].f);
	MaxPV.Update(tmpfl[4].f);
	MinA.Update(tmpfl[5].f);
	MaxA.Update(tmpfl[6].f);
	MinW.Update(tmpfl[7].f);
	MaxW.Update(tmpfl[8].f);
	Factor.Update(tmpfl[9].f);
	Adjust.Update(tmpfl[10].f);
	uint8_t E[46] = { 0, tmp, tmpfl[0].b[0], tmpfl[0].b[1], tmpfl[0].b[2], tmpfl[0].b[3], tmpfl[1].b[0], tmpfl[1].b[1], tmpfl[1].b[2], tmpfl[1].b[3],
		tmpfl[2].b[0], tmpfl[2].b[1], tmpfl[2].b[2], tmpfl[2].b[3], tmpfl[3].b[0], tmpfl[3].b[1], tmpfl[3].b[2], tmpfl[3].b[3], tmpfl[4].b[0],
		tmpfl[4].b[1], tmpfl[4].b[2], tmpfl[4].b[3], tmpfl[5].b[0], tmpfl[5].b[1], tmpfl[5].b[2], tmpfl[5].b[3], tmpfl[6].b[0], tmpfl[6].b[1],
		tmpfl[6].b[2], tmpfl[6].b[3], tmpfl[7].b[0], tmpfl[7].b[1], tmpfl[7].b[2], tmpfl[7].b[3], tmpfl[8].b[0], tmpfl[8].b[1], tmpfl[8].b[2],
		tmpfl[8].b[3], tmpfl[9].b[0], tmpfl[9].b[1], tmpfl[9].b[2], tmpfl[9].b[3], tmpfl[10].b[0], tmpfl[10].b[1], tmpfl[10].b[2], tmpfl[10].b[3] };
	da->PushInBE(cl, sizeof(E), ID, E);
    }
}

uint8_t KA_BVT::SKATTchannel::SetNewTTParam(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    if((State.vl == 4) && (TSYS::getUnalignFloat(val + 9) > TSYS::getUnalignFloat(val + 5))
	    && (TSYS::getUnalignFloat(val + 17) > TSYS::getUnalignFloat(val + 13)) && (TSYS::getUnalignFloat(val + 25) > TSYS::getUnalignFloat(val + 21))
	    && (TSYS::getUnalignFloat(val + 33) > TSYS::getUnalignFloat(val + 29)) && (TSYS::getUnalignFloat(val + 37) >= 0)
	    && (0.02 > TSYS::getUnalignFloat(val + 37))) {
	if(Period.lnk.Connected() && Sens.lnk.Connected() && MinS.lnk.Connected() && MaxS.lnk.Connected() && MinPV.lnk.Connected() && MaxPV.lnk.Connected()
		&& MinA.lnk.Connected() && MaxA.lnk.Connected() && MinW.lnk.Connected() && MaxW.lnk.Connected() && Factor.lnk.Connected()
		&& Adjust.lnk.Connected()) {
	    Period.s = addr;
	    Period.Set(val[0]);
	    Sens.Set(TSYS::getUnalignFloat(val + 1));
	    MinS.Set(TSYS::getUnalignFloat(val + 5));
	    MaxS.Set(TSYS::getUnalignFloat(val + 9));
	    MinPV.Set(TSYS::getUnalignFloat(val + 13));
	    MaxPV.Set(TSYS::getUnalignFloat(val + 17));
	    MinA.Set(TSYS::getUnalignFloat(val + 21));
	    MaxA.Set(TSYS::getUnalignFloat(val + 25));
	    MinW.Set(TSYS::getUnalignFloat(val + 29));
	    MaxW.Set(TSYS::getUnalignFloat(val + 33));
	    Factor.Set(TSYS::getUnalignFloat(val + 37));
	    Adjust.Set(TSYS::getUnalignFloat(val + 41));
	    uint8_t E[46];
	    E[0] = addr;
	    memcpy(E + 1, val, 45);
	    da->PushInBE(1, sizeof(E), prmID, E);
	    return 2 + 45;
	} else {
	    return 0;
	}
    } else {
	return 0;
    }
}

KA_BVT::KA_BVT(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params), config(5 | (n << 4) | (4 << 10))
{
    mTypeFT3 = KA;
    chan_err.clear();
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    for(int i = 0; i < count_n; i++) {
	AddChannel(i);
    }
    loadIO(true);
}

KA_BVT::~KA_BVT()
{

}

void KA_BVT::AddChannel(uint8_t iid)
{
    data.push_back(SKATTchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
    AddAttr(data.back().Value.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:0", iid + 1));
    if(with_params) {
	AddAttr(data.back().Period.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Sens.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MinS.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MaxS.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MinPV.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MaxPV.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MinW.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MaxW.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MinA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().MaxA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Factor.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Adjust.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
    }
}

string KA_BVT::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос каналов:";
	for(int i = 1; i <= count_n; i++) {
	    switch(chan_err[i].state) {
	    case 0:
		rez += TSYS::strMess(" %d.", i);
		break;
	    case 2:
		rez += TSYS::strMess(" %d!!!", i);
		break;
	    }
	}
    } else {
	rez = "0: Норма";
    }
    return rez;
}

void KA_BVT::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws
    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].Value.lnk);
	loadLnk(data[i].Period.lnk);
	loadLnk(data[i].Sens.lnk);
	loadLnk(data[i].MinS.lnk);
	loadLnk(data[i].MaxS.lnk);
	loadLnk(data[i].MinPV.lnk);
	loadLnk(data[i].MaxPV.lnk);
	loadLnk(data[i].MinW.lnk);
	loadLnk(data[i].MaxW.lnk);
	loadLnk(data[i].MinA.lnk);
	loadLnk(data[i].MaxA.lnk);
	loadLnk(data[i].Factor.lnk);
	loadLnk(data[i].Adjust.lnk);
    }
}

void KA_BVT::saveIO()
{
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].Value.lnk);
	saveLnk(data[i].Period.lnk);
	saveLnk(data[i].Sens.lnk);
	saveLnk(data[i].MinS.lnk);
	saveLnk(data[i].MaxS.lnk);
	saveLnk(data[i].MinPV.lnk);
	saveLnk(data[i].MaxPV.lnk);
	saveLnk(data[i].MinW.lnk);
	saveLnk(data[i].MaxW.lnk);
	saveLnk(data[i].MinA.lnk);
	saveLnk(data[i].MaxA.lnk);
	saveLnk(data[i].Factor.lnk);
	saveLnk(data[i].Adjust.lnk);
    }
}

void KA_BVT::tmHandler(void)
{
    NeedInit = false;
    for(int i = 0; i < count_n; i++) {
	if(with_params) {
	    data[i].UpdateTTParam(PackID(ID, (i + 1), 2), 1);
	}
	UpdateParamFlState(data[i].Value, data[i].State, PackID(ID, (i + 1), 0), 0);
    }
}

uint16_t KA_BVT::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    return rc;
}
uint16_t KA_BVT::HandleEvent(uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    return l;
}

uint8_t KA_BVT::cmdGet(uint16_t prmID, uint8_t * out)
{
//    mess_info("KA_BVT", "cmdGet %04X", prmID);
    FT3ID ft3ID = UnpackID(prmID);
//    mess_info("KA_BVT", "ID %d ft3ID g%d k%d n%d ", ID, ft3ID.g, ft3ID.k, ft3ID.n);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 0:
	    //state
	    out[0] = 1;
	    l = 1;
	    break;
	case 1:
	    out[0] = config >> 8;
	    out[1] = config;
	    l = 2;
	    break;
	case 2:
	    for(uint8_t i = 0; i < count_n; i++) {
		out[i * 5] = data[i].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[i * 5 + j + 1] = data[i].Value.b_vl[j];
		l += 5;
	    }
	    break;
	}
    } else {
	if(ft3ID.k <= count_n) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Value.b_vl[j];
		l = 5;
		break;
	    case 1:
		out[0] = data[ft3ID.k - 1].State.s;
		out[1] = data[ft3ID.k - 1].State.vl;
		l = 2;
		break;
	    case 2:
		out[0] = data[ft3ID.k - 1].Period.s;
		out[1] = data[ft3ID.k - 1].Period.vl;
		l = 2;
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].Sens.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MinS.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MaxS.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MinPV.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MaxPV.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MinA.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MaxA.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MinW.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].MaxW.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].Factor.b_vl[j];
		for(uint8_t j = 0; j < 4; j++)
		    out[l++] = data[ft3ID.k - 1].Adjust.b_vl[j];
	    }

	}
    }
    return l;
}

uint8_t KA_BVT::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if(ft3ID.k <= count_n) {
	switch(ft3ID.n) {
	case 0:
	    if((data[ft3ID.k - 1].State.vl == 4)) {
		l = SetNewflVal(data[ft3ID.k - 1].Value, data[ft3ID.k - 1].State.vl, prmID, TSYS::getUnalignFloat(req + 2));
	    }
	    break;
	case 1:
	    l = SetNew8Val(data[ft3ID.k - 1].State, addr, prmID, req[2]);
	    break;
	case 2:
	    l = data[ft3ID.k - 1].SetNewTTParam(addr, prmID, req + 2);
	    break;
	}
    }
    return l;
}

uint16_t KA_BVT::setVal(TVal &val)
{
    return 0;
}

B_BVT::B_BVT(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params, bool has_k, bool has_rate) :
	DA(prm), ID(id), count_n(n), with_params(has_params), with_k(has_k), with_rate(has_rate)
{
    mTypeFT3 = GRS;
    chan_err.clear();
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    for(int i = 0; i < count_n; i++) {
	AddChannel(i);
    }
    loadIO(true);
}

B_BVT::~B_BVT()
{

}

void B_BVT::AddChannel(uint8_t iid)
{
    data.push_back(STTchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:0", iid + 1));
    AddAttr(data.back().Value.lnk, TFld::Real, TFld::NoWrite, TSYS::strMess("%d:1", iid + 1));
    if(with_params) {
	AddAttr(data.back().Period.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Sens.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:3", iid + 1));
	AddAttr(data.back().MinS.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().MaxS.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().MinPV.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:5", iid + 1));
	AddAttr(data.back().MaxPV.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:5", iid + 1));
	AddAttr(data.back().MinW.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:6", iid + 1));
	AddAttr(data.back().MaxW.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:6", iid + 1));
	AddAttr(data.back().MinA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:7", iid + 1));
	AddAttr(data.back().MaxA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:7", iid + 1));
	AddAttr(data.back().Factor.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:8", iid + 1));
	AddAttr(data.back().Dimension.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:9", iid + 1));
	if(with_k) {
	    AddAttr(data.back().CorFactor.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:10", iid + 1));
	    if(with_rate) {
		AddAttr(data.back().Rate.lnk, TFld::Real, TFld::NoWrite, TSYS::strMess("%d:11", iid + 1));
		AddAttr(data.back().Calcs.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:12", iid + 1));
		AddAttr(data.back().RateSens.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:13", iid + 1));
		AddAttr(data.back().RateLimit.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:14", iid + 1));

	    }
	}
    }
}

string B_BVT::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос каналов:";
	for(int i = 1; i <= count_n; i++) {
	    switch(chan_err[i].state) {
	    case 0:
		rez += TSYS::strMess(" %d.", i);
		break;
	    case 2:
		rez += TSYS::strMess(" %d!!!", i);
		break;
	    }
	}
    } else {
	rez = "0: Норма";
    }
    return rez;
}

void B_BVT::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws

    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].Value.lnk);
	loadLnk(data[i].Period.lnk);
	loadLnk(data[i].Sens.lnk);
	loadLnk(data[i].MinS.lnk);
	loadLnk(data[i].MaxS.lnk);
	loadLnk(data[i].MinPV.lnk);
	loadLnk(data[i].MaxPV.lnk);
	loadLnk(data[i].MinW.lnk);
	loadLnk(data[i].MaxW.lnk);
	loadLnk(data[i].MinA.lnk);
	loadLnk(data[i].MaxA.lnk);
	loadLnk(data[i].Factor.lnk);
	loadLnk(data[i].Dimension.lnk);
	loadLnk(data[i].CorFactor.lnk);
	loadLnk(data[i].Rate.lnk);
	loadLnk(data[i].Calcs.lnk);
	loadLnk(data[i].RateSens.lnk);
	loadLnk(data[i].RateLimit.lnk);
    }
}

void B_BVT::saveIO()
{
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].Value.lnk);
	saveLnk(data[i].Period.lnk);
	saveLnk(data[i].Sens.lnk);
	saveLnk(data[i].MinS.lnk);
	saveLnk(data[i].MaxS.lnk);
	saveLnk(data[i].MinPV.lnk);
	saveLnk(data[i].MaxPV.lnk);
	saveLnk(data[i].MinW.lnk);
	saveLnk(data[i].MaxW.lnk);
	saveLnk(data[i].MinA.lnk);
	saveLnk(data[i].MaxA.lnk);
	saveLnk(data[i].Factor.lnk);
	saveLnk(data[i].Dimension.lnk);
	saveLnk(data[i].CorFactor.lnk);
	saveLnk(data[i].Rate.lnk);
	saveLnk(data[i].Calcs.lnk);
	saveLnk(data[i].RateSens.lnk);
	saveLnk(data[i].RateLimit.lnk);
    }
}

void B_BVT::tmHandler(void)
{
    NeedInit = false;
    for(int i = 0; i < count_n; i++) {
	if(with_params) {
	    UpdateParam8(data[i].Period, PackID(ID, (i + 1), 2), 1);
	    UpdateParamFl(data[i].Sens, PackID(ID, (i + 1), 3), 1);
	    UpdateParam2Fl(data[i].MinS, data[i].MaxS, PackID(ID, (i + 1), 4), 1);
	    UpdateParam2Fl(data[i].MinPV, data[i].MaxPV, PackID(ID, (i + 1), 5), 1);
	    UpdateParam2Fl(data[i].MinW, data[i].MaxW, PackID(ID, (i + 1), 6), 1);
	    UpdateParam2Fl(data[i].MinA, data[i].MaxA, PackID(ID, (i + 1), 7), 1);
	    UpdateParamFl(data[i].Factor, PackID(ID, (i + 1), 8), 1);
	    UpdateParam8(data[i].Dimension, PackID(ID, (i + 1), 9), 1);
	    if(with_k) {
		UpdateParamFl(data[i].CorFactor, PackID(ID, (i + 1), 10), 1);
		if(with_rate) {
		    UpdateParamFlState(data[i].Rate, data[i].State, PackID(ID, (i + 1), 11), 1);
		    UpdateParam8(data[i].Calcs, PackID(ID, (i + 1), 12), 1);
		    UpdateParamFl(data[i].RateSens, PackID(ID, (i + 1), 13), 1);
		    UpdateParamFl(data[i].RateLimit, PackID(ID, (i + 1), 14), 1);
		}
	    }
	}
	UpdateParamFlState(data[i].Value, data[i].State, PackID(ID, (i + 1), 1), 1);
    }
}

uint16_t B_BVT::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	NeedInit = false;
	Msg.L = 5;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0); //состояние
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		mPrm.vlAt("state").at().setI(Msg.D[7], 0, true);
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			if(chan_err[i].state == 1) continue;
//			mess_info(mPrm.nodePath().c_str(), "Refreshing channel %d", i);
			Msg.L = 21;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = PackID(ID, i, 1); //Значение ТT
			*((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 2); //Период измерений
			*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 3); //Чувствительность
			*((uint16_t *) (Msg.D + 6)) = PackID(ID, i, 4); //min max датчика
			*((uint16_t *) (Msg.D + 8)) = PackID(ID, i, 5); //min max ФВ
			*((uint16_t *) (Msg.D + 10)) = PackID(ID, i, 6); //min max предупредительный
			*((uint16_t *) (Msg.D + 12)) = PackID(ID, i, 7); //min max аварийный
			*((uint16_t *) (Msg.D + 14)) = PackID(ID, i, 8); //Коэффициент расширения
			*((uint16_t *) (Msg.D + 16)) = PackID(ID, i, 9); //Размерность
			if(with_k) {
			    *((uint16_t *) (Msg.D + 18)) = PackID(ID, i, 10); //Корректирующий коэффициент
			    Msg.L += 2;
			    if(with_rate) {
				*((uint16_t *) (Msg.D + 20)) = PackID(ID, i, 11); //Средняя скорость
				*((uint16_t *) (Msg.D + 22)) = PackID(ID, i, 12); //Количество вычислений
				*((uint16_t *) (Msg.D + 24)) = PackID(ID, i, 13); //Чувствительность
				*((uint16_t *) (Msg.D + 26)) = PackID(ID, i, 14); //Предельная скорость
				Msg.L += 8;
			    }
			}

			if(mPrm.owner().Transact(&Msg)) {
			    if(Msg.C == GOOD3) {
				mPrm.vlAt(TSYS::strMess("state_%d", i).c_str()).at().setI(Msg.D[7], 0, true);
				mPrm.vlAt(TSYS::strMess("value_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 8), 0, true);
				mPrm.vlAt(TSYS::strMess("period_%d", i).c_str()).at().setI(Msg.D[17], 0, true);
				mPrm.vlAt(TSYS::strMess("sens_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 23), 0, true);
				mPrm.vlAt(TSYS::strMess("minS_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 32), 0, true);
				mPrm.vlAt(TSYS::strMess("maxS_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 36), 0, true);
				mPrm.vlAt(TSYS::strMess("minPV_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 45), 0, true);
				mPrm.vlAt(TSYS::strMess("maxPV_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 49), 0, true);
				mPrm.vlAt(TSYS::strMess("minW_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 58), 0, true);
				mPrm.vlAt(TSYS::strMess("maxW_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 62), 0, true);
				mPrm.vlAt(TSYS::strMess("minA_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 71), 0, true);
				mPrm.vlAt(TSYS::strMess("maxA_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 75), 0, true);
				mPrm.vlAt(TSYS::strMess("factor_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 84), 0, true);
				mPrm.vlAt(TSYS::strMess("dimens_%d", i).c_str()).at().setI(Msg.D[93], 0, true);
				if(with_k) {
				    mPrm.vlAt(TSYS::strMess("corfactor_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 99), 0, true);
				    if(with_rate) {
					mPrm.vlAt(TSYS::strMess("rate_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 108), 0, true);
					mPrm.vlAt(TSYS::strMess("calcs_%d", i).c_str()).at().setI(Msg.D[117], 0, true);
					mPrm.vlAt(TSYS::strMess("ratesens_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 123), 0, true);
					mPrm.vlAt(TSYS::strMess("ratelimit_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 132), 0, true);
				    }
				}
				chan_err[i].state = 1;
				rc = 1;
			    } else {
				rc = 0;
				chan_err[i].state = 2;
				NeedInit = true;
				//	break;
			    }
			} else {
			    rc = 0;
			    chan_err[i].state = 3;
			    NeedInit = true;
			    //break;
			}

		    }
		} else {
		    rc = 1;
		}
	    }
	}
	break;
    }
    return rc;
}
uint16_t B_BVT::HandleEvent(uint8_t * D)
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
		mPrm.vlAt(TSYS::strMess("value_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), 0, true);
	    }
	    break;
	case 2:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], 0, true);
		mPrm.vlAt(TSYS::strMess("ratelimit_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), 0, true);
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
		mPrm.vlAt(TSYS::strMess("value_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);

		l = 7;
		break;
	    case 2:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("period_%d", ft3ID.k).c_str()).at().setI(D[3], 0, true);
		}
		l = 4;
		break;
	    case 3:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("sens_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 4:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minS_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxS_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 5:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minPV_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxPV_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 6:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minW_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxW_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 7:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("minA_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		    mPrm.vlAt(TSYS::strMess("maxA_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
		}
		l = 11;
		break;
	    case 8:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("factor_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 9:
		if(with_params) {
		    mPrm.vlAt(TSYS::strMess("dimens_%d", ft3ID.k).c_str()).at().setI(D[3], 0, true);
		}
		l = 4;
		break;
	    case 10:
		if(with_params && with_k) {
		    mPrm.vlAt(TSYS::strMess("corfactor_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 11:
		if(with_params && with_rate) {
		    mPrm.vlAt(TSYS::strMess("rate_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 12:
		if(with_params && with_rate) {
		    mPrm.vlAt(TSYS::strMess("calcs_%d", ft3ID.k).c_str()).at().setI(D[3], 0, true);
		}
		l = 4;
		break;
	    case 13:
		if(with_params && with_rate) {
		    mPrm.vlAt(TSYS::strMess("ratesens_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    case 14:
		if(with_params && with_rate) {
		    mPrm.vlAt(TSYS::strMess("ratelimit_%d", ft3ID.k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		}
		l = 7;
		break;
	    }
	}
	break;
    }
    return l;
}

uint8_t B_BVT::cmdGet(uint16_t prmID, uint8_t * out)
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
	case 2:
	    out[0] = 0;
	    l = 1;
	    //rate
	    for(uint8_t i = 0; i < count_n; i++) {
		out[i * 5 + 1] = data[i].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[i * 5 + 2 + j] = data[i].Rate.b_vl[j];
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
		out[1] = data[ft3ID.k - 1].Period.vl;
		l = 2;
		break;
	    case 3:
		out[0] = data[ft3ID.k - 1].Sens.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Sens.b_vl[j];
		l = 5;
		break;
	    case 4:
		out[0] = data[ft3ID.k - 1].MinS.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].MinS.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].MaxS.b_vl[j];
		}
		l = 9;
		break;
	    case 5:
		out[0] = data[ft3ID.k - 1].MinPV.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].MinPV.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].MaxPV.b_vl[j];
		}
		l = 9;
		break;
	    case 6:
		out[0] = data[ft3ID.k - 1].MinW.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].MinW.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].MaxW.b_vl[j];
		}
		l = 9;
		break;
	    case 7:
		out[0] = data[ft3ID.k - 1].MinA.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].MinA.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].MaxA.b_vl[j];
		}
		l = 9;
		break;
	    case 8:
		out[0] = data[ft3ID.k - 1].Factor.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].Factor.b_vl[j];
		}
		l = 5;
		break;
	    case 9:
		out[0] = data[ft3ID.k - 1].Dimension.s;
		out[1] = data[ft3ID.k - 1].Dimension.vl;
		l = 2;
		break;
	    case 10:
		out[0] = data[ft3ID.k - 1].CorFactor.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].CorFactor.b_vl[j];
		}
		l = 5;
		break;
	    case 11:
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Rate.b_vl[j];
		l = 5;
		break;
	    case 12:
		out[0] = data[ft3ID.k - 1].Calcs.s;
		out[1] = data[ft3ID.k - 1].Calcs.vl;
		l = 2;
		break;
	    case 13:
		out[0] = data[ft3ID.k - 1].RateSens.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].RateSens.b_vl[j];
		l = 5;
		break;
	    case 14:
		out[0] = data[ft3ID.k - 1].RateLimit.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].RateLimit.b_vl[j];
		l = 5;
		break;
	    }

	}
    }
    return l;
}

uint8_t B_BVT::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if((ft3ID.k > 0) && (ft3ID.k <= count_n)) {
	switch(ft3ID.n) {
	case 2:
	    l = SetNew8Val(data[ft3ID.k - 1].Period, addr, prmID, req[2]);
	    break;
	case 3:
	    l = SetNewflVal(data[ft3ID.k - 1].Sens, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 4:
	    l = SetNew2flVal(data[ft3ID.k - 1].MinS, data[ft3ID.k - 1].MaxS, addr, prmID, TSYS::getUnalignFloat(req + 2), TSYS::getUnalignFloat(req + 6));
	    break;
	case 5:
	    l = SetNew2flVal(data[ft3ID.k - 1].MinPV, data[ft3ID.k - 1].MaxPV, addr, prmID, TSYS::getUnalignFloat(req + 2), TSYS::getUnalignFloat(req + 6));
	    break;
	case 6:
	    l = SetNew2flVal(data[ft3ID.k - 1].MinW, data[ft3ID.k - 1].MaxW, addr, prmID, TSYS::getUnalignFloat(req + 2), TSYS::getUnalignFloat(req + 6));
	    break;
	case 7:
	    l = SetNew2flVal(data[ft3ID.k - 1].MinA, data[ft3ID.k - 1].MaxA, addr, prmID, TSYS::getUnalignFloat(req + 2), TSYS::getUnalignFloat(req + 6));
	    break;
	case 8:
	    l = SetNewflVal(data[ft3ID.k - 1].Factor, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 9:
	    l = SetNew8Val(data[ft3ID.k - 1].Dimension, addr, prmID, req[2]);
	    break;
	case 10:
	    l = SetNewflVal(data[ft3ID.k - 1].CorFactor, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 12:
	    l = SetNew8Val(data[ft3ID.k - 1].Calcs, addr, prmID, req[2]);
	    break;
	case 13:
	    l = SetNewflVal(data[ft3ID.k - 1].RateSens, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 14:
	    l = SetNewflVal(data[ft3ID.k - 1].RateLimit, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	}
    }
    return l;
}

uint16_t B_BVT::setVal(TVal &val)
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
    case 9:
    case 12:
	Msg.L = 6;
	Msg.D[2] = val.get(NULL, true).getI();
	break;
    case 3:
    case 8:
    case 10:
    case 13:
    case 14:
	Msg.L = 9;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	break;
    case 4:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minS_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxS_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 5:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minPV_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxPV_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 6:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minW_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxW_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 7:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minA_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxA_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    }
    if(Msg.L) mPrm.owner().Transact(&Msg);
    return 0;
}

//---------------------------------------------------------------------------

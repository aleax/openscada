//OpenSCADA system module DAQ.FT3 file: TANK.cpp
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

#include <sys/times.h>

#include <tsys.h>

#include "mod_FT3.h"
#include "TANK.h"

using namespace FT3;

void KA_TANK::SKATANKchannel::UpdateParam(uint16_t ID, uint8_t cl)
{
    ui8w tmp[9];
    tmp[0].w = TCSens1.Get();
    tmp[1].w = TCSens1Not.Get();
    tmp[2].w = TCSens2.Get();
    tmp[3].w = TCSens2Not.Get();
    tmp[4].w = TULight.Get();
    tmp[5].w = TUSound.Get();
    tmp[6].w = TURlOff.Get();
    tmp[7].w = TimeDelay.Get();
    tmp[8].w = TimeFalseAlarm.Get();

    if(tmp[0].w != TCSens1.vl || tmp[1].w != TCSens1Not.vl || tmp[2].w != TCSens2.vl || tmp[3].w != TCSens2Not.vl || tmp[4].w != TULight.vl || tmp[5].w != TUSound.vl || tmp[6].w != TURlOff.vl || tmp[7].w != TimeDelay.vl || tmp[8].w != TimeFalseAlarm.vl) {
	TCSens1.s = 0;
	TCSens1.Update(tmp[0].w);
	TCSens1Not.Update(tmp[1].w);
	TCSens2.Update(tmp[2].w);
	TCSens2Not.Update(tmp[3].w);
	TULight.Update(tmp[4].w);
	TUSound.Update(tmp[5].w);
	TURlOff.Update(tmp[6].w);
	TimeDelay.Update(tmp[7].w);
	TimeFalseAlarm.Update(tmp[8].w);
	uint8_t E[19] = { 0, tmp[0].b[0], tmp[0].b[1], tmp[1].b[0], tmp[1].b[1], tmp[2].b[0], tmp[2].b[1], tmp[3].b[0], tmp[3].b[1], tmp[4].b[0], tmp[4].b[1], tmp[5].b[0], tmp[5].b[1], tmp[6].b[0], tmp[6].b[1], tmp[7].b[0], tmp[7].b[1], tmp[8].b[0], tmp[8].b[1] };
	da->PushInBE(cl, sizeof(E), ID, E);
    }
}

uint8_t KA_TANK::SKATANKchannel::SetNewParam(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    if(TCSens1.lnk.Connected() || TCSens1Not.lnk.Connected() || TCSens2.lnk.Connected() || TCSens2Not.lnk.Connected() || TULight.lnk.Connected() || TUSound.lnk.Connected()
	    || TURlOff.lnk.Connected() || TimeDelay.lnk.Connected() || TimeFalseAlarm.lnk.Connected()) {
	TCSens1.s = addr;
	TCSens1.Set(TSYS::getUnalign16(val));
	TCSens1Not.Set(TSYS::getUnalign16(val + 2));
	TCSens2.Set(TSYS::getUnalign16(val + 4));
	TCSens2Not.Set(TSYS::getUnalign16(val + 6));
	TULight.Set(TSYS::getUnalign16(val + 8));
	TUSound.Set(TSYS::getUnalign16(val + 10));
	TURlOff.Set(TSYS::getUnalign16(val + 12));
	TimeDelay.Set(TSYS::getUnalign16(val + 14));
	TimeFalseAlarm.Set(TSYS::getUnalign16(val + 16));

	uint8_t E[19];
	E[0] = addr;
	memcpy(E + 1, val, 18);
	da->PushInBE(1, sizeof(E), prmID, E);
	return 2 + 18;
    } else {
	return 0;
    }
}

uint8_t KA_TANK::SKATANKchannel::SetNewState(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    uint8_t rc = 0;
    if(Function.lnk.Connected() && State.lnk.Connected()) {
	if(*val < 3) {
	    State.s = addr;
	    State.Set(*val);
	    Function.Set((uint8_t)0);
	    uint8_t E[2] = { addr, State.vl };
	    da->PushInBE(1, sizeof(E), prmID, E);
	    rc = 2 + 1;
	}
    }
    return rc;
}

uint8_t KA_TANK::SKATANKchannel::SetNewFunction(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    uint8_t rc = 0;
    if(Function.lnk.Connected() && State.lnk.Connected()) {
	if(((State.vl & 0x3F) != 0x2) && *val < 2) {
	    Function.s = addr;
	    Function.Set(*val);
	    uint8_t E[2] = { addr, Function.vl };
	    da->PushInBE(1, sizeof(E), prmID, E);
	    rc = 3;
	}
    }
    return rc;
}

KA_TANK::KA_TANK(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params), config(0xF | (n << 4) | (2 << 10))
{
    mTypeFT3 = KA;
    chan_err.clear();
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");

    for(int i = 0; i < count_n; i++) {
	chan_err.insert(chan_err.end(), SDataRec());
	AddTANKchannel(i);
    }
    loadIO(true);
}

KA_TANK::~KA_TANK()
{

}

void KA_TANK::AddTANKchannel(uint8_t iid)
{
    data.push_back(SKATANKchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:0", iid + 1));
    AddAttr(data.back().Function.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:3", iid + 1));
    if(with_params) {
	AddAttr(data.back().TCSens1.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TCSens1Not.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TCSens2.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TCSens2Not.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TULight.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TUSound.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TURlOff.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TimeDelay.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TimeFalseAlarm.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().StateTCSens1.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().StateTCSens1Not.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().StateTCSens2.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().StateTCSens2Not.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
    }
}

string KA_TANK::getStatus(void)
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

void KA_TANK::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }
    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].Function.lnk);
	loadLnk(data[i].TCSens1.lnk);
	loadLnk(data[i].TCSens1Not.lnk);
	loadLnk(data[i].TCSens2.lnk);
	loadLnk(data[i].TCSens2Not.lnk);
	loadLnk(data[i].TULight.lnk);
	loadLnk(data[i].TUSound.lnk);
	loadLnk(data[i].TURlOff.lnk);
	loadLnk(data[i].TimeDelay.lnk);
	loadLnk(data[i].TimeFalseAlarm.lnk);
	loadLnk(data[i].StateTCSens1.lnk);
	loadLnk(data[i].StateTCSens1Not.lnk);
	loadLnk(data[i].StateTCSens2.lnk);
	loadLnk(data[i].StateTCSens2Not.lnk);
    }
}

void KA_TANK::saveIO(void)
{
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].Function.lnk);
	saveLnk(data[i].TCSens1.lnk);
	saveLnk(data[i].TCSens1Not.lnk);
	saveLnk(data[i].TCSens2.lnk);
	saveLnk(data[i].TCSens2Not.lnk);
	saveLnk(data[i].TULight.lnk);
	saveLnk(data[i].TUSound.lnk);
	saveLnk(data[i].TURlOff.lnk);
	saveLnk(data[i].TimeDelay.lnk);
	saveLnk(data[i].TimeFalseAlarm.lnk);
	saveLnk(data[i].StateTCSens1.lnk);
	saveLnk(data[i].StateTCSens1Not.lnk);
	saveLnk(data[i].StateTCSens2.lnk);
	saveLnk(data[i].StateTCSens2Not.lnk);
    }
}

void KA_TANK::tmHandler(void)
{
    for(int i = 0; i < count_n; i++) {
	if(with_params) {
	    data[i].UpdateParam(PackID(ID, (i + 1), 2), 1);
	}
	UpdateParam8(data[i].State, PackID(ID, (i + 1), 0), 1);
	UpdateParam8(data[i].Function, PackID(ID, (i + 1), 3), 1);
    }
    NeedInit = false;
}

uint16_t KA_TANK::Task(uint16_t uc)
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
		NeedInit = false;
		rc = 1;
		for(int i = 1; i <= count_n; i++) {
		    if(chan_err[i].state == 1) continue;
		    Msg.L = 5;
		    Msg.C = AddrReq;
		    *((uint16_t *) Msg.D) = PackID(ID, i, 0); //state
		    Msg.L += 2;
		    *((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 3); //func
		    if(with_params) {
			Msg.L += 2;
			*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 2); //params
			Msg.L += 2;
			*((uint16_t *) (Msg.D + 6)) = PackID(ID, i, 4); //TC
		    }
		    if(mPrm.owner().DoCmd(&Msg)) {
			if(Msg.C == GOOD3) {
			    chan_err[i].state = 1;
			    rc = 1;
			} else {
			    rc = 0;
			    chan_err[i].state = 2;
			    NeedInit = true;
			}
		    } else {
			rc = 0;
			chan_err[i].state = 3;
			NeedInit = true;
		    }
		}
	    } else {
		rc = 0;
		NeedInit = true;
	    }
	}
	break;
    }
    return rc;
}

uint16_t KA_TANK::HandleEvent(int64_t tm, uint8_t * D)
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
	    l = 4;
	    break;
	case 2:
	    l = 2 + count_n * 2;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j)).at().setI(D[j * 2 + 1], tm, true);
	    }
	    break;
	}
	break;
    default:
	switch(ft3ID.n) {
	case 0:
	    l = 4;
	    if(ft3ID.k > count_n) break;
	    mPrm.vlAt(TSYS::strMess("state_%d", ft3ID.k)).at().setI(D[3], tm, true);
	    break;
	case 2:
	    l = 21;
	    if(with_params) {
		if(ft3ID.k > count_n) break;
		mPrm.vlAt(TSYS::strMess("TCSens1_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("TCSens1Not_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 5), tm, true);
		mPrm.vlAt(TSYS::strMess("TCSens2_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 7), tm, true);
		mPrm.vlAt(TSYS::strMess("TCSens2Not_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 9), tm, true);
		mPrm.vlAt(TSYS::strMess("TULight_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 11), tm, true);
		mPrm.vlAt(TSYS::strMess("TUSound_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 13), tm, true);
		mPrm.vlAt(TSYS::strMess("TURlOff_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 15), tm, true);
		mPrm.vlAt(TSYS::strMess("TimeDelay_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 17), tm, true);
		mPrm.vlAt(TSYS::strMess("TimeFalseAlarm_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 19), tm, true);
	    }
	    break;
	case 3:
	    l = 4;
	    if(with_params) {
		if(ft3ID.k > count_n) break;
		mPrm.vlAt(TSYS::strMess("function_%d", ft3ID.k)).at().setI(D[3], tm, true);
	    }
	    break;
	case 4:
	    l = 11;
	    if(with_params) {
		if(ft3ID.k > count_n) break;
		mPrm.vlAt(TSYS::strMess("StateTCSens1_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("StateTCSens1Not_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 5), tm, true);
		mPrm.vlAt(TSYS::strMess("StateTCSens2_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 7), tm, true);
		mPrm.vlAt(TSYS::strMess("StateTCSens2Not_%d", ft3ID.k)).at().setI(TSYS::getUnalign16(D + 9), tm, true);
	    }
	    break;
	}
	break;
    }
    return l;
}

uint8_t KA_TANK::cmdGet(uint16_t prmID, uint8_t * out)
{
    FT3ID ft3ID = UnpackID(prmID);
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
		out[i * 2] = i;
		out[i * 2 + 1] = data[i].State.vl;
	    }
	    l = count_n * 2;
	    break;
	}
    } else {
	if(ft3ID.k <= count_n) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = data[ft3ID.k - 1].State.s;
		out[1] = data[ft3ID.k - 1].State.vl;
		l = 2;
		break;
	    case 2:
		out[l++] = data[ft3ID.k - 1].TCSens1.s;
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCSens1.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCSens1Not.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCSens2.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCSens2Not.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TULight.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TUSound.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TURlOff.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeDelay.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeFalseAlarm.b_vl[j];
		break;
	    case 3:
		out[0] = data[ft3ID.k - 1].Function.s;
		out[1] = data[ft3ID.k - 1].Function.vl;
		l = 2;
		break;
	    case 4:
		out[l++] = data[ft3ID.k - 1].StateTCSens1.s;
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].StateTCSens1.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].StateTCSens1Not.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].StateTCSens2.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].StateTCSens2Not.b_vl[j];
		break;
	    }
	}
    }
    return l;
}

uint8_t KA_TANK::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    if(ft3ID.k <= count_n) {
	switch(ft3ID.n) {
	case 0:
	    l = data[ft3ID.k - 1].SetNewState(addr, prmID, req + 2);
	    break;
	case 2:
	    l = data[ft3ID.k - 1].SetNewParam(addr, prmID, req + 2);
	    break;
	case 3:
	    l = data[ft3ID.k - 1].SetNewFunction(addr, prmID, req + 2);
	}

    }
    return l;
}

uint16_t KA_TANK::setVal(TVal &val)
{
    int off = 0;
    FT3ID ft3ID;
    ft3ID.k = s2i(TSYS::strParse(val.fld().reserve(), 0, ":", &off));
    ft3ID.n = s2i(TSYS::strParse(val.fld().reserve(), 0, ":", &off));
    ft3ID.g = ID;
    if((ft3ID.k >= 1) && (ft3ID.k <= count_n)) {
	tagMsg Msg;
	Msg.L = 0;
	Msg.C = SetData;
	Msg.L += SerializeUi16(Msg.D + Msg.L, PackID(ft3ID));
	switch(ft3ID.n) {
	case 0:case 3:
	    Msg.L += SerializeB(Msg.D + Msg.L, val.getI(0, true));
	    break;
	case 2:
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TCSens1_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TCSens1Not_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TCSens2_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TCSens2Not_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TULight_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TUSound_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TURlOff_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TimeDelay_%d", ft3ID.k)).at().getI(0, true));
	    Msg.L += SerializeUi16(Msg.D + Msg.L, mPrm.vlAt(TSYS::strMess("TimeFalseAlarm_%d", ft3ID.k)).at().getI(0, true));
	    break;
	}
	if(Msg.L > 2) {
	    Msg.L += 3;
	    mPrm.owner().DoCmd(&Msg);
	}
    }
    return 0;
}

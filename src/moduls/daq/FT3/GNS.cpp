//OpenSCADA system module DAQ.FT3 file: GNS.cpp
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
#include "GNS.h"

using namespace FT3;

void KA_GNS::SKANSchannel::UpdateTUParam(uint16_t ID, uint8_t cl)
{
    ui8w tmp[5][2];
    tmp[0][0].w = TUOn.Get();
    tmp[0][1].w = TimeOn.Get();
    tmp[1][0].w = TUOff.Get();
    tmp[1][1].w = TimeOff.Get();
    tmp[2][0].w = TUStop.Get();
    tmp[2][1].w = TimeStop.Get();
    tmp[3][0].w = TURemote.Get();
    tmp[3][1].w = TimeRemote.Get();
    tmp[4][0].w = TUManual.Get();
    tmp[4][1].w = TimeManual.Get();

    if(tmp[0][0].w != TUOn.vl || tmp[0][1].w != TimeOn.vl || tmp[1][0].w != TUOff.vl || tmp[1][1].w != TimeOff.vl || tmp[2][0].w != TUStop.vl
	    || tmp[2][1].w != TimeStop.vl || tmp[3][0].w != TUManual.vl || tmp[3][1].w != TimeManual.vl || tmp[4][0].w != TURemote.vl
	    || tmp[4][1].w != TimeRemote.vl) {
	TUOn.s = 0;
	TUOn.Update(tmp[0][0].w);
	TimeOn.Update(tmp[0][1].w);
	TUOff.Update(tmp[1][0].w);
	TimeOff.Update(tmp[1][1].w);
	TUStop.Update(tmp[2][0].w);
	TimeStop.Update(tmp[2][1].w);
	TURemote.Update(tmp[3][0].w);
	TimeRemote.Update(tmp[3][1].w);
	TUManual.Update(tmp[4][0].w);
	TimeManual.Update(tmp[4][1].w);
	uint8_t E[21] = { 0, tmp[0][0].b[0], tmp[0][0].b[1], tmp[0][1].b[0], tmp[0][1].b[1], tmp[1][0].b[0], tmp[1][0].b[1], tmp[1][1].b[0], tmp[1][1].b[1],
		tmp[2][0].b[0], tmp[2][0].b[1], tmp[2][1].b[0], tmp[2][1].b[1], tmp[3][0].b[0], tmp[3][0].b[1], tmp[3][1].b[0], tmp[3][1].b[1], tmp[4][0].b[0],
		tmp[4][0].b[1], tmp[4][1].b[0], tmp[4][1].b[1] };
	da->PushInBE(cl, sizeof(E), ID, E);
    }
}

void KA_GNS::SKANSchannel::UpdateTCParam(uint16_t ID, uint8_t cl)
{
    ui8w tmp[3];
    tmp[0].w = TCOn.Get();
    tmp[1].w = TCOff.Get();
    tmp[2].w = TCMode.Get();

    if(tmp[0].w != TCOn.vl || tmp[1].w != TCOff.vl || tmp[2].w != TCMode.vl) {
	TCOn.s = 0;
	TCOn.Update(tmp[0].w);
	TCOff.Update(tmp[1].w);
	TCMode.Update(tmp[2].w);
	uint8_t E[7] = { 0, tmp[0].b[0], tmp[0].b[1], tmp[1].b[0], tmp[1].b[1], tmp[2].b[0], tmp[2].b[1] };
	da->PushInBE(cl, sizeof(E), ID, E);
    }
}

uint8_t KA_GNS::SKANSchannel::SetNewTUParam(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    if(TUOn.lnk.Connected() || TimeOn.lnk.Connected() || TUOff.lnk.Connected() || TimeOff.lnk.Connected() || TUStop.lnk.Connected() || TimeOn.lnk.Connected() || TUManual.lnk.Connected()
	    || TimeManual.lnk.Connected() || TURemote.lnk.Connected() || TimeRemote.lnk.Connected()) {
	return 0;
    } else {
	TUOn.s = addr;
	TUOn.Set(TSYS::getUnalign16(val));
	TimeOn.Set(TSYS::getUnalign16(val + 2));
	TUOff.Set(TSYS::getUnalign16(val + 4));
	TimeOff.Set(TSYS::getUnalign16(val + 6));
	TUStop.Set(TSYS::getUnalign16(val + 8));
	TimeStop.Set(TSYS::getUnalign16(val + 10));
	TURemote.Set(TSYS::getUnalign16(val + 12));
	TimeRemote.Set(TSYS::getUnalign16(val + 14));
	TUManual.Set(TSYS::getUnalign16(val + 16));
	TimeManual.Set(TSYS::getUnalign16(val + 18));

	uint8_t E[21];
	E[0] = addr;
	memcpy(E + 1, val, 20);
	da->PushInBE(1, sizeof(E), prmID, E);
	return 2 + 20;
    }
}

uint8_t KA_GNS::SKANSchannel::SetNewTCParam(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    if(TCOn.lnk.Connected() || TCOff.lnk.Connected() || TCMode.lnk.Connected()) {
	return 0;
    } else {
	TCOn.s = addr;
	TCOn.Set(TSYS::getUnalign16(val));
	TCOff.Set(TSYS::getUnalign16(val + 2));
	TCMode.Set(TSYS::getUnalign16(val + 4));

	uint8_t E[7];
	E[0] = addr;
	memcpy(E + 1, val, 6);
	da->PushInBE(1, sizeof(E), prmID, E);
	return 2 + 10;
    }
}

KA_GNS::KA_GNS(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params), config(0xF | (n << 4) | (3 << 10))
{
    mTypeFT3 = KA;
    chan_err.clear();
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");

    for(int i = 0; i < count_n; i++) {
	chan_err.insert(chan_err.end(), SDataRec());
	AddNSChannel(i);
	/*	    data.push_back(SKANSchannel(i, this));
	 mPrm.p_el.fldAdd(fld = new TFld(data.back().State.lnk.prmName.c_str(), data[i].State.lnk.prmDesc.c_str(), TFld::Integer, TVal::DirWrite));
	 data.back().State.lnk.vlattr = mPrm.vlAt(data[i].State.lnk.prmName.c_str());
	 fld->setReserve(TSYS::strMess("%d:1", i + 1));*/
    }
    loadIO(true);
}

KA_GNS::~KA_GNS()
{

}

void KA_GNS::AddNSChannel(uint8_t iid)
{
    //SKANSchannel* t = new SKANSchannel(iid, this);
    data.push_back(SKANSchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:0", iid + 1));
    AddAttr(data.back().Function.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:3", iid + 1));
    if(with_params) {
	AddAttr(data.back().TUOn.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TUOff.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TUStop.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TURemote.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TUManual.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TimeOn.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TimeOff.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TimeStop.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TimeRemote.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TimeManual.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
	AddAttr(data.back().TCOn.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TCOff.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().TCMode.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Time.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
    }
}

/*void KA_GNS::AddAttr(SLnk& param, TFld::Type type, unsigned flg, const string& ex)
{
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld(param.prmName.c_str(), param.prmDesc.c_str(), type, flg));
    param.vlattr = mPrm.vlAt(param.prmName.c_str());
    fld->setReserve(ex);
}*/

string KA_GNS::getStatus(void)
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

void KA_GNS::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }
    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].Function.lnk);
	loadLnk(data[i].TUOn.lnk);
	loadLnk(data[i].TUOff.lnk);
	loadLnk(data[i].TUStop.lnk);
	loadLnk(data[i].TURemote.lnk);
	loadLnk(data[i].TUManual.lnk);
	loadLnk(data[i].TimeOn.lnk);
	loadLnk(data[i].TimeOff.lnk);
	loadLnk(data[i].TimeStop.lnk);
	loadLnk(data[i].TimeRemote.lnk);
	loadLnk(data[i].TimeManual.lnk);
	loadLnk(data[i].TCOn.lnk);
	loadLnk(data[i].TCOff.lnk);
	loadLnk(data[i].TCMode.lnk);
	loadLnk(data[i].Time.lnk);
    }

}

void KA_GNS::saveIO(void)
{
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].Function.lnk);
	saveLnk(data[i].TUOn.lnk);
	saveLnk(data[i].TUOff.lnk);
	saveLnk(data[i].TUStop.lnk);
	saveLnk(data[i].TURemote.lnk);
	saveLnk(data[i].TUManual.lnk);
	saveLnk(data[i].TimeOn.lnk);
	saveLnk(data[i].TimeOff.lnk);
	saveLnk(data[i].TimeStop.lnk);
	saveLnk(data[i].TimeRemote.lnk);
	saveLnk(data[i].TimeManual.lnk);
	saveLnk(data[i].TCOn.lnk);
	saveLnk(data[i].TCOff.lnk);
	saveLnk(data[i].TCMode.lnk);
	saveLnk(data[i].Time.lnk);
    }

}

void KA_GNS::tmHandler(void)
{
    for(int i = 0; i < count_n; i++) {
	if(with_params) {
	    data[i].UpdateTUParam(PackID(ID, (i + 1), 1), 1);
	    data[i].UpdateTCParam(PackID(ID, (i + 1), 2), 1);
	    UpdateParam32(data[i].Time, PackID(ID, (i + 1), 4), 2);
	    //    data[i].UpdateTTParam(PackID(ID, (i + 1), 2), 1);
	}
	UpdateParam8(data[i].State, PackID(ID, (i + 1), 0), 1);
	UpdateParam8(data[i].Function, PackID(ID, (i + 1), 3), 1);
    }
    NeedInit = false;
}

uint16_t KA_GNS::Task(uint16_t uc)
{
    uint16_t rc = 0;
    return rc;
}

uint16_t KA_GNS::HandleEvent(int64_t tm, uint8_t * D)
{
    FT3ID ft3ID = UnpackID(TSYS::getUnalign16(D));
    if(ft3ID.g != ID) return 0;
    uint16_t l = 0;
    return l;
}

uint8_t KA_GNS::cmdGet(uint16_t prmID, uint8_t * out)
{
//    mess_info("KA_GNS", "cmdGet %04X", prmID);
    FT3ID ft3ID = UnpackID(prmID);
//    mess_info("KA_GNS", "ID %d ft3ID g%d k%d n%d ", ID, ft3ID.g, ft3ID.k, ft3ID.n);
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
	    case 1:
		out[l++] = data[ft3ID.k - 1].TUOn.s;
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TUOn.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeOn.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TUOff.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeOff.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TUStop.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeStop.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TURemote.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeRemote.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TUManual.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TimeManual.b_vl[j];
		break;
	    case 2:
		out[l++] = data[ft3ID.k - 1].TCOn.s;
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCOn.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCOff.b_vl[j];
		for(uint8_t j = 0; j < 2; j++)
		    out[l++] = data[ft3ID.k - 1].TCMode.b_vl[j];
		break;
	    case 3:
		out[0] = data[ft3ID.k - 1].Function.s;
		out[1] = data[ft3ID.k - 1].Function.vl;
		l = 2;
		break;
	    case 4:
		out[0] = data[ft3ID.k - 1].Time.s;
		out[1] = data[ft3ID.k - 1].Time.b_vl[0];
		out[2] = data[ft3ID.k - 1].Time.b_vl[1];
		out[3] = data[ft3ID.k - 1].Time.b_vl[2];
		out[4] = data[ft3ID.k - 1].Time.b_vl[3];
		l = 5;
		break;

	    }

	}
    }
    return l;
}

uint8_t KA_GNS::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if(ft3ID.k <= count_n) {
	switch(ft3ID.n) {
	case 0:
	    l = SetNew8Val(data[ft3ID.k - 1].State, addr, prmID, req[2]);
	    break;
	case 1:
	    l = data[ft3ID.k - 1].SetNewTUParam(addr, prmID, req + 2);
	    break;
	case 2:
	    l = data[ft3ID.k - 1].SetNewTCParam(addr, prmID, req + 2);
	    break;
	case 3:
	    l = SetNew8Val(data[ft3ID.k - 1].Function, addr, prmID, req[2]);
	    break;
	case 4:
	    l = SetNew32Val(data[ft3ID.k - 1].Time, addr, prmID, TSYS::getUnalign16(req + 2));
	    break;
	}

    }
    return l;
}

uint16_t KA_GNS::setVal(TVal &val)
{
    return 0;
}

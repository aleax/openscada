//OpenSCADA system module DAQ.FT3 file: GKR.cpp
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
#include "GKR.h"

using namespace FT3;

B_GKR::B_GKR(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params)

{
    mTypeFT3 = GRS;
    TFld * fld;
    state = 0;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    for(int i = 0; i < count_n; i++) {
	AddChannel(i);
    }
    loadIO(true);
}

B_GKR::~B_GKR()
{

}

void B_GKR::AddChannel(uint8_t iid)
{
    data.push_back(SKRchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:0", iid + 1));
    AddAttr(data.back().On.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:1", iid + 1));
    AddAttr(data.back().Off.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:1", iid + 1));
    AddAttr(data.back().Run.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:2", iid + 1));
    AddAttr(data.back().Reset.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:2", iid + 1));
    AddAttr(data.back().Lock.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:3", iid + 1));
    AddAttr(data.back().Lubrication.lnk, TFld::Integer, TFld::NoWrite, TSYS::strMess("%d:4", iid + 1));
    if(with_params) {
	AddAttr(data.back().Time.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:5", iid + 1));
	AddAttr(data.back().ExTime.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:6", iid + 1));
	AddAttr(data.back().Time_Lub.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:7", iid + 1));
	AddAttr(data.back().Timeout_PO.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:8", iid + 1));
    }
}
string B_GKR::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

void B_GKR::loadIO(bool force)
{
    //Load links
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws

    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].On.lnk);
	loadLnk(data[i].Off.lnk);
	loadLnk(data[i].Run.lnk);
	loadLnk(data[i].Reset.lnk);
	loadLnk(data[i].Lock.lnk);
	loadLnk(data[i].Lubrication.lnk);
	loadLnk(data[i].Time.lnk);
	loadLnk(data[i].ExTime.lnk);
	loadLnk(data[i].Time_Lub.lnk);
	loadLnk(data[i].Timeout_PO.lnk);
    }
}

void B_GKR::saveIO()
{
    //Save links
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].On.lnk);
	saveLnk(data[i].Off.lnk);
	saveLnk(data[i].Run.lnk);
	saveLnk(data[i].Reset.lnk);
	saveLnk(data[i].Lock.lnk);
	saveLnk(data[i].Lubrication.lnk);
	saveLnk(data[i].Time.lnk);
	saveLnk(data[i].ExTime.lnk);
	saveLnk(data[i].Time_Lub.lnk);
	saveLnk(data[i].Timeout_PO.lnk);
    }
}

void B_GKR::tmHandler(void)
{
    for(int i = 0; i < count_n; i++) {
	data[i].UpdateState(PackID(ID, (i + 1), 0), 1);
	if(with_params) {
	    UpdateParamW(data[i].Time, PackID(ID, i + 1, 5), 1);
	    UpdateParamW(data[i].ExTime, PackID(ID, i + 1, 6), 1);
	    UpdateParamW(data[i].Time_Lub, PackID(ID, i + 1, 7), 1);
	    UpdateParamW(data[i].Timeout_PO, PackID(ID, i + 1, 8), 1);
	}
    }
    NeedInit = false;
}

uint16_t B_GKR::Task(uint16_t uc)
{
    uint16_t rc = 0;
    return rc;
}

uint16_t B_GKR::HandleEvent(int64_t tm, uint8_t * D)
{
    uint16_t l = 0;
    return l;
}

uint8_t B_GKR::cmdGet(uint16_t prmID, uint8_t * out)
{
    uint l = 0;
    uint16_t i;
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;

    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 0:
	    out[0] = state;
	    l = 1;
	    break;
	case 1:
	    for(i = 0; i <= count_n; i++) {
		out[i] = data[i].State.vl;
	    }
	    l = count_n;
	    break;
	}
    } else {
	if(count_n && (ft3ID.k <= count_n)) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = data[ft3ID.k - 1].State.vl;
		l = 1;
		break;
	    case 1:
		out[0] = data[ft3ID.k - 1].On.s;
		out[1] = data[ft3ID.k - 1].On.vl;
		l = 2;
		break;
	    case 2:
		out[0] = data[ft3ID.k - 1].Run.s;
		out[1] = data[ft3ID.k - 1].Run.vl;
		l = 2;
		break;
	    case 3:
		out[0] = data[ft3ID.k - 1].Lock.s;
		out[1] = data[ft3ID.k - 1].Lock.vl;
		l = 2;
		break;
	    case 4:
		out[0] = data[ft3ID.k - 1].Lubrication.s;
		out[1] = data[ft3ID.k - 1].Lubrication.vl;
		l = 2;
		break;
	    case 5:
		out[0] = data[ft3ID.k - 1].Time.s;
		out[1] = ((uint16_t) data[ft3ID.k - 1].Time.vl);	//msec
		out[2] = ((uint16_t) data[ft3ID.k - 1].Time.vl) >> 8;	//msec
		l = 3;
		break;
	    case 6:
		out[0] = data[ft3ID.k - 1].ExTime.s;
		out[1] = ((uint16_t) data[ft3ID.k - 1].ExTime.vl);	//msec
		out[2] = ((uint16_t) data[ft3ID.k - 1].ExTime.vl) >> 8;	//msec
		l = 3;
		break;
	    case 7:
		out[0] = data[ft3ID.k - 1].Time_Lub.s;
		out[1] = ((uint16_t) data[ft3ID.k - 1].Time_Lub.vl);	//msec
		out[2] = ((uint16_t) data[ft3ID.k - 1].Time_Lub.vl) >> 8;	//msec
		l = 3;
		break;
	    case 8:
		out[0] = data[ft3ID.k - 1].Timeout_PO.s;
		out[1] = ((uint16_t) data[ft3ID.k - 1].Timeout_PO.vl);	//msec
		out[2] = ((uint16_t) data[ft3ID.k - 1].Timeout_PO.vl) >> 8;	//msec
		l = 3;
		break;
	    }
	}
    }
    return l;
}

uint8_t B_GKR::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    if(mess_lev() == TMess::Debug) mPrm.mess_sys(TMess::Debug, "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if(ft3ID.k != 0) {
	if(count_n && (ft3ID.k <= count_n)) {
	    switch(ft3ID.n) {
	    case 1:
		setTU(ft3ID.k, req[2], addr, prmID);
		l = 3;
		break;
	    case 2:
		runTU(ft3ID.k, req[2], addr, prmID);
		l = 3;
		break;
	    case 3:
		l = SetNew8Val(data[ft3ID.k - 1].Lock, addr, prmID, req[2]);
		break;
	    case 4:
		l = SetNew8Val(data[ft3ID.k - 1].Lubrication, addr, prmID, req[2]);
		break;
	    case 5:
		l = SetNewWVal(data[ft3ID.k - 1].Time, addr, prmID, TSYS::getUnalign16(req + 2));
		break;
	    case 6:
		l = SetNewWVal(data[ft3ID.k - 1].ExTime, addr, prmID, TSYS::getUnalign16(req + 2));
		break;
	    case 7:
		l = SetNewWVal(data[ft3ID.k - 1].Time_Lub, addr, prmID, TSYS::getUnalign16(req + 2));
		break;
	    case 8:
		l = SetNewWVal(data[ft3ID.k - 1].Timeout_PO, addr, prmID, TSYS::getUnalign16(req + 2));
		break;
	    }
	}
    }
    return l;
}

void B_GKR::setTU(uint8_t k, uint8_t val, uint8_t addr, uint16_t prmID)
{
    if((k > 0) && (k < count_n)) {
	SKRchannel & TU = data[k - 1];
	if(val) {
	    if(TU.On.lnk.Connected()) {
		//on
		TU.On.lnk.aprm.at().setI(1);
		mPrm.vlAt(TU.On.lnk.prmName).at().setI(k, 0, true);
		uint8_t E[2] = { addr, val };
		PushInBE(1, sizeof(E), prmID, E);
	    }
	} else {
	    if(TU.Off.lnk.Connected()) {
		//off
		TU.Off.lnk.aprm.at().setI(1);
		mPrm.vlAt(TU.Off.lnk.prmName).at().setI(k, 0, true);
		uint8_t E[2] = { addr, val };
		PushInBE(1, sizeof(E), prmID, E);
	    }
	}
	TU.Off.vl = TU.On.vl = val;
	TU.On.s = TU.Off.s = addr;
    }
}
void B_GKR::runTU(uint8_t k, uint8_t val, uint8_t addr, uint16_t prmID)
{
    if((k > 0) && (k < count_n)) {
	SKRchannel & TU = data[k - 1];
	if((val == 0x55) && (TU.Run.lnk.Connected())) {
	    TU.Run.s = addr;
	    TU.Run.lnk.aprm.at().setI(1);
	    mPrm.vlAt(TU.Run.lnk.prmName).at().setI(0, 0, true);
	    uint8_t E[2] = { addr, 0 };
	    PushInBE(1, sizeof(E), prmID, E);
	    TU.On.vl = TU.Off.vl = 0;
	}
	if((!val) && (TU.Reset.lnk.Connected())) {
	    TU.Reset.s = addr;
	    TU.Reset.lnk.aprm.at().setI(1);
	    mPrm.vlAt(TU.Reset.lnk.prmName).at().setI(1, 0, true);
	    uint8_t E[2] = { addr, 0 };
	    PushInBE(1, sizeof(E), prmID, E);
	    TU.On.vl = TU.Off.vl = 0;
	}
    }
}

void B_GKR::SKRchannel::UpdateState(uint16_t ID, uint8_t cl)
{
    uint8_t tmpui8;
    tmpui8 = State.Get();
    if(tmpui8 != State.vl) {
	State.Update(tmpui8);
	uint8_t E[1] = { tmpui8 };
	da->PushInBE(cl, sizeof(E), ID, E);
    }
}

uint16_t B_GKR::setVal(TVal &val)
{
    return 0;
}
//---------------------------------------------------------------------------

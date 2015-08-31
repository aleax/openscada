//OpenSCADA system module DAQ.FT3 file: da.cpp
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
#include "da.h"

using namespace FT3;

void DA::AddAttr(SLnk& param, TFld::Type type, unsigned flg, const string& ex)
{
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld(param.prmName.c_str(), param.prmDesc.c_str(), type, flg));
    param.vlattr = mPrm.vlAt(param.prmName.c_str());
    fld->setReserve(ex);
}

void DA::loadLnk(SLnk& lnk)
{
    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";

    cfg.cfg("ID").setS(lnk.prmName);
    if(SYS->db().at().dataGet(io_bd, io_table, cfg, false, true)) {
	lnk.prmAttr = cfg.cfg("VALUE").getS();
	lnk.aprm = SYS->daq().at().attrAt(lnk.prmAttr, '.', true);
    }
}

void DA::saveLnk(SLnk& lnk)
{
    TConfig cfg(&mPrm.prmIOE());
    cfg.cfg("PRM_ID").setS(mPrm.ownerPath(true));
    string io_bd = mPrm.owner().DB() + "." + mPrm.typeDBName() + "_io";
    string io_table = mPrm.owner().owner().nodePath() + mPrm.typeDBName() + "_io";
    cfg.cfg("ID").setS(lnk.prmName);
    cfg.cfg("VALUE").setS(lnk.prmAttr);
    SYS->db().at().dataSet(io_bd, io_table, cfg);
}

uint8_t DA::SetNew8Val(ui8Data& d, uint8_t addr, uint16_t prmID, uint8_t val)
{
    if(d.lnk.Connected()) {
	d.s = addr;
	d.Set(val);
	uint8_t E[2] = { addr, d.vl };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 1;
    } else {
	return 0;
    }
}

uint8_t DA::SetNew8Val(ui16Data& d, uint8_t addr, uint16_t prmID, uint8_t val)
{
    if(d.lnk.Connected()) {
	d.s = addr;
	d.Set(val);
	uint8_t E[2] = { addr, val };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 1;
    } else {
	return 0;
    }
}

uint8_t DA::SetNew28Val(ui8Data& d1, ui8Data& d2, uint8_t addr, uint16_t prmID, uint8_t val1, uint8_t val2)
{
    if((d1.lnk.Connected()) && (d2.lnk.Connected())) {
	d1.s = addr;
	d1.Set(val1);
	d2.s = addr;
	d2.Set(val2);
	uint8_t E[3] = { addr, d1.vl };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 2;
    } else {
	return 0;
    }
}

uint8_t DA::SetNewWVal(ui16Data& d, uint8_t addr, uint16_t prmID, uint16_t val)
{
    if(d.lnk.Connected()) {
	d.s = addr;
	d.Set(val);
	uint8_t E[3] = { addr, d.b_vl[0], d.b_vl[1] };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 2;
    } else {
	return 0;
    }
}

uint8_t DA::SetNewflVal(flData& d, uint8_t addr, uint16_t prmID, float val)
{
    if(d.lnk.Connected()) {
	d.s = addr;
	d.Set(val);
	uint8_t E[5] = { addr, d.b_vl[0], d.b_vl[1], d.b_vl[2], d.b_vl[3] };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 4;
    } else {
	return 0;
    }
}

uint8_t DA::SetNew32Val(ui32Data& d, uint8_t addr, uint16_t prmID, uint32_t val)
{
    if(d.lnk.Connected()) {
	d.s = addr;
	d.Set(val);
	uint8_t E[5] = { addr, d.b_vl[0], d.b_vl[1], d.b_vl[2], d.b_vl[3] };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 4;
    } else {
	return 0;
    }
}

uint8_t DA::SetNew2flVal(flData& d1, flData& d2, uint8_t addr, uint16_t prmID, float val1, float val2)
{
    if(d1.lnk.Connected() && d2.lnk.Connected()) {
	d1.s = addr;
	d1.Set(val1);
	d2.s = addr;
	d2.Set(val2);
	uint8_t E[9] = { addr, d1.b_vl[0], d1.b_vl[1], d1.b_vl[2], d1.b_vl[3], d2.b_vl[0], d2.b_vl[1], d2.b_vl[2], d2.b_vl[3] };
	mPrm.owner().PushInBE(1, sizeof(E), prmID, E);
	return 2 + 4 + 4;
    } else {
	return 0;
    }
}

void DA::UpdateParam8(ui8Data& param, uint16_t ID, uint8_t cl)
{
    uint8_t tmpui8;
    tmpui8 = param.Get();
    if(tmpui8 != param.vl) {
	param.Update(tmpui8);
	uint8_t E[2] = { 0, tmpui8 };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

void DA::UpdateParam8(ui16Data& param, uint16_t ID, uint8_t cl)
{
    uint8_t tmpui8;
    tmpui8 = param.Get();
    if(tmpui8 != param.vl) {
	param.Update(tmpui8);
	uint8_t E[2] = { 0, tmpui8 };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

void DA::UpdateParamW(ui16Data& param, uint16_t ID, uint8_t cl)
{
    ui8w tmpw;
    tmpw.w = param.Get();
    if(tmpw.w != param.vl) {
	param.Update(tmpw.w);
	uint8_t E[3] = { 0, tmpw.b[0], tmpw.b[1] };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

void DA::UpdateParamFl(flData& param, uint16_t ID, uint8_t cl)
{
    ui8fl tmpfl;
    tmpfl.f = param.Get();
    if(tmpfl.f != param.vl) {
	param.Update(tmpfl.f);
	uint8_t E[5] = { 0, tmpfl.b[0], tmpfl.b[1], tmpfl.b[2], tmpfl.b[3] };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

void DA::UpdateParam32(ui32Data& param, uint16_t ID, uint8_t cl)
{
    ui832 tmp;
    tmp.ui32 = param.Get();
    if(tmp.ui32 != param.vl) {
	param.Update(tmp.ui32);
	uint8_t E[5] = { 0, tmp.b[0], tmp.b[1], tmp.b[2], tmp.b[3] };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

void DA::UpdateParamFlState(flData& param, ui8Data& state, uint16_t ID, uint8_t cl)
{
    uint8_t tmpui8;
    ui8fl tmpfl;
    tmpui8 = state.Get();
    tmpfl.f = param.Get();
    if((tmpfl.f != param.vl) || (tmpui8 != state.vl)) {
	state.Update(tmpui8);
	param.Update(tmpfl.f);
	uint8_t E[5] = { state.vl, tmpfl.b[0], tmpfl.b[1], tmpfl.b[2], tmpfl.b[3] };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

void DA::UpdateParam2Fl(flData& param1, flData& param2, uint16_t ID, uint8_t cl)
{
    ui8fl tmpfl1, tmpfl2;
    tmpfl1.f = param1.Get();
    tmpfl2.f = param2.Get();
    if(tmpfl1.f != param1.vl || tmpfl2.f != param2.vl) {
	param1.Update(tmpfl1.f);
	param2.Update(tmpfl2.f);
	uint8_t E[9] = { 0, tmpfl1.b[0], tmpfl1.b[1], tmpfl1.b[2], tmpfl1.b[3], tmpfl2.b[0], tmpfl2.b[1], tmpfl2.b[2], tmpfl2.b[3] };
	mPrm.owner().PushInBE(1, sizeof(E), ID, E);
    }
}

void DA::UpdateParam28(ui8Data& param1, ui8Data& param2, uint16_t ID, uint8_t cl)
{
    uint8_t tmp1, tmp2;
    tmp1 = param1.Get();
    tmp2 = param2.Get();
    if(tmp1 != param1.vl || tmp2 != param2.vl) {
	param1.Update(tmp1);
	param2.Update(tmp2);
	uint8_t E[3] = { 0, tmp1, tmp2 };
	mPrm.owner().PushInBE(cl, sizeof(E), ID, E);
    }
}

FT3ID DA::UnpackID(uint16_t ID)
{
    if(mess_lev() == TMess::Debug) mess_debug(mPrm.nodePath().c_str(), _("UnpackID %d"), ID);
    FT3ID rc;
    switch(mTypeFT3) {
    case GRS:
	rc.g = ID >> 12;
	rc.k = (ID >> 6) & 0x3F;
	rc.n = ID & 0x3F;
	break;
    case KA:
	rc.g = ID & 0x0F;
	rc.k = (ID >> 4) & 0x3F;
	rc.n = (ID >> 10) & 0x3F;
	break;
    }
    if(mess_lev() == TMess::Debug) mess_debug(mPrm.nodePath().c_str(), _("g %d k %d n %d"), rc.g, rc.k, rc.n);
    return rc;
}

uint16_t DA::PackID(FT3ID ID)
{
    uint16_t rc;
    switch(mTypeFT3) {
    case GRS:
	rc = ID.g << 12 | (ID.k << 6) | (ID.n);
	break;
    case KA:
	rc = ID.g | (ID.k << 4) | (ID.n << 10);
	break;
    }
    return rc;
}

uint16_t DA::PackID(uint8_t g, uint8_t k, uint8_t n)
{
    uint16_t rc;
    switch(mTypeFT3) {
    case GRS:
	rc = g << 12 | (k << 6) | (n);
	break;
    case KA:
	rc = g | (k << 4) | (n << 10);
	break;
    }
    return rc;
}

void DA::PushInBE(uint8_t type, uint8_t length, uint16_t id, uint8_t *E)
{
    mPrm.owner().PushInBE(type, length, id, E);
}

time_t DA::DateTimeToTime_t(uint8_t *d)
{
    return mPrm.owner().DateTimeToTime_t(d);
}

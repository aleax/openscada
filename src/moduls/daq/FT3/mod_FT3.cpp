//OpenSCADA system module DAQ.FT3 file: mod_FT3.cpp
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

#include <signal.h>
#include <fcntl.h>

#include <ttypeparam.h>

#include "BUC.h"
#include "BVI.h"
#include "BVTC.h"
#include "BVT.h"
#include "BIP.h"
#include "PAUK.h"
#include "ACCOUNT.h"
#include "BTR.h"
#include "BTE.h"
#include "ODOR.h"
#include "GZD.h"
#include "GNS.h"
#include "GKR.h"

#include "mod_FT3.h"
#include "FT3_prt.h"

FT3::TTpContr *FT3::mod; //Pointer for direct access to the module
extern "C"
{
    TModule::SAt module(int nMod)
    {
	if(nMod == 0)
	    return TModule::SAt(PRT_ID, PRT_TYPE, PRT_SUBVER);
	else if(nMod == 1) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach(const TModule::SAt &AtMod, const string &source)
    {
	if(AtMod == TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE))
	    return new FT3::TTpContr(source);
	else if(AtMod == TModule::SAt(PRT_ID, PRT_TYPE, PRT_SUBVER)) return new FT3::TProt(source);
	return NULL;
    }
}

using namespace FT3;

time_t FT3::GRSDateTimeToTime_t(uint8_t * D)
{
    char months[12] = { 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    struct tm * timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    uint8_t m;
    uint16_t d = TSYS::getUnalign16(D) >> 9;
    months[1] = (d & 3) ? 28 : 29;
    timeinfo->tm_year = d + 100;
    d = TSYS::getUnalign16(D) & 0x1FF;
    if(d)
	m = 1;
    else
	m = 0;
    for(int j = 0; j < 12; j++)
	if(d <= months[j])
	    break;
	else {
	    d -= months[j];
	    m++;
	}
    timeinfo->tm_mday = d;
    timeinfo->tm_mon = m - 1;
    timeinfo->tm_hour = D[2];
    d = TSYS::getUnalign16(D + 3);
    timeinfo->tm_min = d / 600;
    timeinfo->tm_sec = (d % 600) / 10;
    rawtime = mktime(timeinfo);
    return rawtime;
}

time_t FT3::KADateTimeToTime_t(uint8_t * D)
{
    struct tm * timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    uint8_t m = (TSYS::getUnalign16(D) >> 5) & 0xF;
    uint16_t d = TSYS::getUnalign16(D) >> 9;
    timeinfo->tm_year = d + 100;
    d = TSYS::getUnalign16(D) & 0x1F;
    timeinfo->tm_mday = d;
    timeinfo->tm_mon = m - 1;
    timeinfo->tm_hour = D[2];
    d = TSYS::getUnalign16(D + 3);
    timeinfo->tm_min = d >> 10;
    timeinfo->tm_sec = (d >> 4) & 0x3F;
    rawtime = mktime(timeinfo);
    return rawtime;
}

void FT3::Time_tToKADateTime(uint8_t * D, time_t time)
{
    struct tm * timeinfo;
    timeinfo = localtime(&time);
    D[0] = timeinfo->tm_mday | ((timeinfo->tm_mon + 1) << 5);
    D[1] = (timeinfo->tm_year - 100 << 1) | ((timeinfo->tm_mon + 1) >> 3);
    D[2] = timeinfo->tm_hour;
    D[3] = timeinfo->tm_sec << 4;
    D[4] = (timeinfo->tm_sec >> 4) | (timeinfo->tm_min << 2);
}

void FT3::Time_tToGRSDateTime(uint8_t * D, time_t time)
{
    struct tm * timeinfo;
    timeinfo = localtime(&time);
    uint16_t ms = timeinfo->tm_min * 600 + timeinfo->tm_sec * 10;
    D[0] = (timeinfo->tm_yday + 1) & 0xFF;
    D[1] = ((timeinfo->tm_year - 100) << 1) | ((timeinfo->tm_yday + 1) >> 8);
    D[2] = timeinfo->tm_hour;
    D[3] = ms & 0xFF;
    D[4] = ms >> 8;
}

void TFT3Channel::PushInBE(uint8_t type, uint8_t length, uint16_t id, uint8_t *E, uint8_t *DHM)
{
    uint8_t *pE = E;
    chain_BE *pCi;
    el_chBE *pBE;
    int i;
    bool fNewBE;
    uint16_t day = TSYS::getUnalign16(DHM);
    uint16_t hour = DHM[2];
    uint16_t ms100 = TSYS::getUnalign16(DHM + 3);
    switch(type) {
    case 1:
	pCi = &C1;
	break;
    case 2:
	pCi = &C2;
	break;
    default:
	return;
    }
    if(!(pCi->tail)) {
	fNewBE = true;
    } else {
	pBE = pCi->tail;
	if((pBE->BE.d != day) || (pBE->BE.h != hour)) {
	    fNewBE = true;
	} else if((sizeof(pBE->BE.mD) - pBE->BE.l) < (length + 4)) {
	    fNewBE = true;
	} else {
	    fNewBE = false;
	}

    }
    if(fNewBE) {
	if(empt.head) {
	    pBE = empt.getdel();
	} else if(C2.head) {
	    pBE = C2.getdel();
	} else {
	    pBE = C1.getdel();
	}
	pBE->BE.d = day;
	pBE->BE.h = hour;
	pBE->BE.l = 0;
	pCi->insert(pBE);
    }
    if(pBE) {
	pBE->BE.mD[pBE->BE.l++] = ms100;
	pBE->BE.mD[pBE->BE.l++] = ms100 >> 8;
	pBE->BE.mD[pBE->BE.l++] = id;
	pBE->BE.mD[pBE->BE.l++] = id >> 8;
	for(i = 0; i < length; i++) {
	    pBE->BE.mD[pBE->BE.l++] = *(pE++);
	}
    }
}

void TMdContr::PushInBE(uint8_t type, uint8_t length, uint16_t id, uint8_t *E)
{
    for(int i = 0; i < Channels.size(); i++) {
	uint8_t DHM[5];
	time_t rawtime;
	time(&rawtime);
	Time_tToDateTime(DHM, rawtime);
	Channels[i].PushInBE(type, length, id, E, DHM);
    }
}

uint8_t TMdContr::cmdGet(uint16_t prmID, uint8_t * out)
{
    uint8_t rc = 0;
    vector<string> lst;
    list(lst);
    for(int i_l = 0; i_l < lst.size(); i_l++) {
	AutoHD<TMdPrm> t = at(lst[i_l]);
	rc = t.at().cmdGet(prmID, out);
	if(rc) {
	    break;
	}
    }
    return rc;
}

uint8_t TMdContr::cmdSet(uint8_t * req, uint8_t addr)
{
    uint8_t rc = 0;
    vector<string> lst;
    list(lst);
    for(int i_l = 0; i_l < lst.size(); i_l++) {
	AutoHD<TMdPrm> t = at(lst[i_l]);
	rc = t.at().cmdSet(req, addr);
	if(rc) {
	    break;
	}
    }
    return rc;
}

bool TMdContr::ProcessMessage(tagMsg *msg, tagMsg *resp)
{
//    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Process message___"));
    uint8_t l;
    uint8_t n;
    uint16_t tm;
    uint8_t rc;
    chain_BE *pC = NULL;
    el_chBE *pBE;
    MtxAlloc res(eventRes, true);
    resp->L = 0;
    if(msg->L == 1) {
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("ProcessMessage one byte req A %02X"), msg->A);
	// One byte req from first block event
	resp->L = 1;
	resp->C = msg->A & 0x3F;
	if(Channels[1].C1.head != NULL) {
	    resp->C |= 0x40;
	}
	if(Channels[1].C2.head != NULL) {
	    resp->C |= 0x80;
	}
	return resp->L;
    }
    switch(msg->C & 0x0F) {
    case ResetChan:
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("ResetChan"));
	Channels[msg->B].FCB2 = Channels[msg->B].FCB3 = 0xFF;
	resp->L = 3;
	resp->C = Channels[msg->B].FCB3 & 0x20;
	break;
    case ReqData1:
    case ReqData2:
    case ReqData:
	if(Channels[msg->B].FCB3 != msg->C) {
	    Channels[msg->B].FCB3 = msg->C;
	    switch(msg->C & 0x0F) {
	    case ReqData1:
		pC = &(Channels[msg->B].C1);

//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("%d ReqData1 C1 %08X %08X"), msg->B, pC, pC->head);
		break;
	    case ReqData2:
		pC = &(Channels[msg->B].C2);
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("%d ReqData1 C2 %08X %08X"), msg->B, pC, pC->head);
		break;
	    default:
		if(Channels[msg->B].C1.head) {
		    pC = &(Channels[msg->B].C1);
//		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("default ReqData C1 %08X %08X"), pC, pC->head);
		} else {
		    pC = &(Channels[msg->B].C2);
//		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("default ReqData C2 %08X %08X"), pC, pC->head);
		}
	    }
	    if((pC == NULL) || (pC->head == NULL)) {
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("%d empty %08X %08X"), msg->B, pC, pC->head);
		Channels[msg->B].resp3.L = 3;
		Channels[msg->B].resp3.C = 9;
	    } else {

		pBE = pC->getdel();
		Channels[msg->B].resp3.D[0] = pBE->BE.d;
		Channels[msg->B].resp3.D[1] = pBE->BE.d >> 8;
		Channels[msg->B].resp3.D[2] = pBE->BE.h;
		for(int i = 0; i < pBE->BE.l; i++)
		    Channels[msg->B].resp3.D[i + 3] = pBE->BE.mD[i];
		Channels[msg->B].resp3.L = pBE->BE.l + 6;

		Channels[msg->B].resp3.C = 8;
		Channels[msg->B].empt.insert(pBE);
	    }
	    if(Channels[msg->B].C1.head) {
		Channels[msg->B].resp3.C |= 0x20;
	    }
	} else {
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("ReqData BAD FCB!!!"));
	}
	memcpy(resp, &Channels[msg->B].resp3, sizeof(tagMsg));
	break;
    case SetData:
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("SetData FCB2 %02X newFCB2 %02X"), Channels[msg->B].FCB2, msg->C);
	if(cfg("IGNORE_FCB2").getB() || (Channels[msg->B].FCB2 != msg->C)) {
	    Channels[msg->B].FCB2 = msg->C;
	    l = msg->L - 3;
	    if(l < 3) l = -1;
	    n = 0;
	    while(l > 1) {
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("l before %d"), l);
		rc = cmdSet(msg->D + n, msg->B);
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("rc after %d"), rc);
		if(rc) {
		    l -= rc;
		    n += rc;
		} else {
		    l = 1;
		}
	    }
//	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("l after %d"), l);
	    if(l) {
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("XXXXXXXX"));
		Channels[msg->B].resp2.L = 3;
		Channels[msg->B].resp2.C = 1;
	    } else {
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("OOOOOOOO"));
		Channels[msg->B].resp2.L = 3;
		Channels[msg->B].resp2.C = 0;
	    }
	} else {
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("SetData BAD FCB!!!"));
	}
	memcpy(resp, &Channels[msg->B].resp2, sizeof(tagMsg));
	break;
    case AddrReq:
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("AddrReq"));
	Channels[msg->B].resp3.C = 8;
	time_t rawtime;
	time(&rawtime);
	Time_tToDateTime(Channels[msg->B].resp3.D, rawtime);
	l = 0;
	n = 3;
	tm = TSYS::getUnalign16(Channels[msg->B].resp3.D + 3);
	while(l < (msg->L - 3)) {
	    uint16_t id = TSYS::getUnalign16(msg->D + l);
	    l += 2;
	    Channels[msg->B].resp3.D[n++] = tm;
	    Channels[msg->B].resp3.D[n++] = tm >> 8;
	    Channels[msg->B].resp3.D[n++] = id;
	    Channels[msg->B].resp3.D[n++] = id >> 8;
	    vector<string> lst;
	    list(lst);
	    rc = 0;
	    rc = cmdGet(id, Channels[msg->B].resp3.D + n);
	    if(rc == 0) {
		l = msg->L - 3;
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("AddrReq ID not found! %04X"), id);
		Channels[msg->B].resp3.C = 9;
		break;
	    }
	    if(rc > (mlD - n)) {
		l = msg->L - 3;
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Too much data to answer %04X"), id);
		Channels[msg->B].resp3.C = 9;
		break;
	    }
	    n += rc;
	}
	if(Channels[msg->B].resp3.C == 9) {
	    Channels[msg->B].resp3.L = 3;
	} else {
	    Channels[msg->B].resp3.L = n + 3;
	}
	memcpy(resp, &Channels[msg->B].resp3, sizeof(tagMsg));
	break;
    case ResData2:
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("ResData2"));
	//Channels[msg->B].FCB2 = 0;
	resp->L = 3;
	resp->C = 0; //Channels[msg->B].FCB3;
	while(Channels[msg->B].C2.head) {
	    Channels[msg->B].empt.insert(Channels[msg->B].C2.getdel());
	}
	break;
    }
    resp->A = msg->B;
    resp->B = devAddr;
    return resp->L;
}

//*************************************************
//* TTpContr                                      *
//*************************************************

TTpContr::TTpContr(string name) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr()
{
}

void TTpContr::load_()
{
}

void TTpContr::save_()
{
}

void TTpContr::postEnable(int flag)
{
    TTypeDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("CTRTYPE", _("Type"), TFld::String, TFld::Selected, "5", "Logic", "Logic;DAQ", _("Logic;DAQ")));
    fldAdd(new TFld("PRTTYPE", _("Protocol"), TFld::String, TFld::Selected, "5", "GRS", "GRS;KA", _("GRS;KA")));
    fldAdd(new TFld("PRM_BD_BUC", _("BUC Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BVTS", _("BVTS Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BVT", _("BVT Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BVI", _("BVI Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BIP", _("BIP Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_PAUK", _("PAUK Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BTU", _("BTU Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_ACCOUNT", _("ACCOUNT Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BTR", _("BTR Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_BTE", _("BTE Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_ODOR", _("ODOR Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_GZD", _("GZD Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PRM_BD_GNS", _("GNS Parameteres table"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("PERIOD", _("Gather data period (s)"), TFld::Integer, TFld::NoFlag, "3", "1", "0;100"));
    fldAdd(new TFld("PRIOR", _("Gather task priority"), TFld::Integer, TFld::NoFlag, "2", "0", "-1;199"));
    //fldAdd(new TFld("TO_PRTR",_("Blocs"),TFld::String,TFld::Selected,"5","BUC","BUC;BTR;BVT;BVTS;BPI",_("BUC;BTR;BVT;BVTS;BPI")));
    fldAdd(new TFld("NODE", _("Addres"), TFld::Integer, TFld::NoFlag, "2", "1", "1;63"));
    fldAdd(new TFld("ADDR", _("Transport address"), TFld::String, TFld::NoFlag, "30", ""));
    fldAdd(new TFld("NCHANNEL", _("Channels count"), TFld::Integer, TFld::NoFlag, "2", "1", "1;63"));
    fldAdd(new TFld("IGNORE_FCB2", _("Ignore FCB2"), TFld::Boolean, TFld::NoFlag, "1", "0"));
    //> Parameter type bd structure

    int t_prm = tpParmAdd("tp_BUC", "PRM_BD_BUC", _("BUC"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "0", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("MOD", _("Modification"), TFld::Integer, TFld::HexDec | TCfg::NoVal, "4", "0"));
    // tpPrmAt(t_prm).fldAdd( new TFld("STOP_TIME",_("Last stop time"),TFld::String,TCfg::Hide,"2","0","0;15") );

    t_prm = tpParmAdd("tp_BVTS", "PRM_BD_BVTS", _("BVTS"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "1", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "32;512"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_BVT", "PRM_BD_BVT", _("BVT"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "3", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;48"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_KPARAMS", _("With correcting factor"), TFld::Boolean, TCfg::NoVal, "1", "1"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_RATEPARAMS", _("With rate calculations"), TFld::Boolean, TCfg::NoVal, "1", "1"));

    t_prm = tpParmAdd("tp_BVI", "PRM_BD_BVI", _("BVI"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "6", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;64"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));
    tpPrmAt(t_prm).fldAdd(new TFld("EXT_PERIOD", _("Ext period"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_BIP", "PRM_BD_BIP", _("BIP"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "7", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;64"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_PAUK", "PRM_BD_PAUK", _("PAUK"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "12", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;64"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_BTU", "PRM_BD_BTU", _("BTU"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "2", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;64"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_ACCOUNT", "PRM_BD_ACCOUNT", _("ACCOUNT"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "4", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;64"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_BTR", "PRM_BD_BTR", _("BTR"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "11", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNTU", _("Channels count TU"), TFld::Integer, TCfg::NoVal, "3", "1", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNTR", _("Channels count TR"), TFld::Integer, TCfg::NoVal, "3", "1", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_BTE", "PRM_BD_BTE", _("BTE"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "11", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count TE"), TFld::Integer, TCfg::NoVal, "3", "1", "0;63"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_ODOR", "PRM_BD_ODOR", _("ODOR"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "10", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count"), TFld::Integer, TCfg::NoVal, "3", "1", "0;2"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_GZD", "PRM_BD_GZD", _("GZD"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "6", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count ZD"), TFld::Integer, TCfg::NoVal, "3", "1", "0;16"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    t_prm = tpParmAdd("tp_GNS", "PRM_BD_GNS", _("GNS"));
    tpPrmAt(t_prm).fldAdd(new TFld("DEV_ID", _("Device address"), TFld::Integer, TCfg::NoVal, "2", "6", "0;15"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHAN_COUNT", _("Channels count NS"), TFld::Integer, TCfg::NoVal, "3", "1", "0;16"));
    tpPrmAt(t_prm).fldAdd(new TFld("WITH_PARAMS", _("With parameters"), TFld::Boolean, TCfg::NoVal, "1", "0"));

    elPrmIO.fldAdd(new TFld("PRM_ID", _("Parameter ID"), TFld::String, TCfg::Key, i2s(atoi(OBJ_ID_SZ) * 6).c_str()));
    elPrmIO.fldAdd(new TFld("ID", _("ID"), TFld::String, TCfg::Key, OBJ_ID_SZ));
    elPrmIO.fldAdd(new TFld("VALUE", _("Value"), TFld::String, TFld::NoFlag, "200"));
}

TController *TTpContr::ContrAttach(const string &name, const string &daq_db)
{
    return new TMdContr(name, daq_db, this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, TElem *cfgelem) :
	TController(name_c, daq_db, cfgelem), prc_st(false), endrun_req(false), tm_gath(0), NeedInit(true), mPer(cfg("PERIOD").getI()),
	mPrior(cfg("PRIOR").getId())
{
    cfg("PRM_BD_BUC").setS("FT3Prm_BUC_" + name_c);
    cfg("PRM_BD_BVTS").setS("FT3Prm_BVTS_" + name_c);
    cfg("PRM_BD_BVT").setS("FT3Prm_BVT_" + name_c);
    cfg("PRM_BD_BVI").setS("FT3Prm_BVI_" + name_c);
    cfg("PRM_BD_BIP").setS("FT3Prm_BIP_" + name_c);
    cfg("PRM_BD_PAUK").setS("FT3Prm_PAUK_" + name_c);
    cfg("PRM_BD_BTU").setS("FT3Prm_BTU_" + name_c);
    cfg("PRM_BD_ACCOUNT").setS("FT3Prm_ACCOUNT_" + name_c);
    cfg("PRM_BD_BTR").setS("FT3Prm_BTR_" + name_c);
    cfg("PRM_BD_BTE").setS("FT3Prm_BTE_" + name_c);
    cfg("PRM_BD_ODOR").setS("FT3Prm_ODOR_" + name_c);
    cfg("PRM_BD_GZD").setS("FT3Prm_GZD_" + name_c);
    cfg("PRM_BD_GNS").setS("FT3Prm_GNS_" + name_c);
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&enRes, &attrM);
    pthread_mutex_init(&eventRes, &attrM);
    pthread_mutexattr_destroy(&attrM);

    MtxAlloc res(eventRes, true);

}

uint16_t TMdContr::CRC(char *data, uint16_t length)
{
    uint16_t CRC = 0, buf;
    uint16_t i, j;
    for(i = 0; i < length; i++) {
	CRC ^= ((uint8_t) data[i] << 8);
	// X16+X13+X12+X11+X10+X8+X6+X5+X2+1
	for(j = 0; j < 8; j++) {
	    buf = CRC;
	    CRC <<= 1;
	    if(buf & 0x8000) CRC ^= 0x3D65;
	}
    }
    return ~CRC;
}

bool TMdContr::DoCmd(tagMsg * pMsg)
{
    uint8_t Cmd = pMsg->C;
    if(Transact(pMsg)) {
	switch(Cmd) {
	case ReqData:
	case ReqData1:
	case ReqData2:
	case AddrReq:
	    if((pMsg->C & 0x0F) == GOOD3) {
		if(mess_lev() == TMess::Debug) {
		    string dump;
		    for(int i = 0; i < (pMsg->L - 3); i++) {
			dump += TSYS::strMess("%02X ", pMsg->D[i]);
		    }
		    mess_debug(nodePath().c_str(), _("data arrived L:%d %s"), (pMsg->L - 3), dump.c_str());
		}
		uint16_t l = pMsg->L - 6, m = 0, n = 3;
		while(l) {
		    vector<string> lst;
		    pMsg->D[3] = pMsg->D[n];
		    pMsg->D[4] = pMsg->D[n + 1];
		    l -= 2;
		    n += 2;
		    list(lst);
		    for(int i_l = 0; !m && i_l < lst.size(); i_l++) {
			AutoHD<TMdPrm> t = at(lst[i_l]);
			m = t.at().HandleEvent(((int64_t) DateTimeToTime_t(pMsg->D)) * 1000000, pMsg->D + n);

		    }
		    if(m) {
			if(!(TSYS::getUnalign16(pMsg->D + n))) pMsg->D[n + 2] = 0;
			l -= m;
			n += m;
			m = 0;
		    } else {
			if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Unhandled data  %04X at %d"), TSYS::getUnalign16(pMsg->D + n), n);
			if(mess_lev() == TMess::Debug) {
			    string dump;
			    for(int i = 0; i < (pMsg->L - 3); i++) {
				dump += TSYS::strMess("%02X ", pMsg->D[i]);
			    }
			    mess_debug(nodePath().c_str(), _("%d, %s"), (pMsg->L - 3), dump.c_str());
			}
			break;
		    }
		}
	    }

	}
	return true;
    } else {
	return false;
    }

}

bool TMdContr::Transact(tagMsg * pMsg)
{

    uint16_t l = 0;
    uint8_t Cmd = pMsg->C;
    string data_s = "";
    char io_buf[4096];
    switch(Cmd) {
    case SetData:
	pMsg->C |= (Channels[pMsg->B].FCB2 | 0x10);
	break;
    case ReqData1:
    case ReqData2:
	pMsg->C |= (Channels[pMsg->B].FCB2 | 0x10);
	break;
    case ReqData:
	if(pMsg->L != 1) pMsg->C |= (Channels[pMsg->B].FCB2 | 0x10);
	break;
    }
    pMsg->A = devAddr;
    pMsg->B = 2;
    uint16_t rc;
    MakePacket(pMsg, io_buf, &l);
    try {
	AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(cfg("ADDR").getS(), 0, '.')).at().outAt(
		TSYS::strSepParse(cfg("ADDR").getS(), 1, '.'));
	if(!tr.at().startStat()) tr.at().start();
	//> Send request
	bool errPresent = true;
	ResAlloc resN(tr.at().nodeRes(), true);
	pMsg->L = 0;

	int resp_len = tr.at().messIO(io_buf, l, io_buf, 8, 0, true);
	l = resp_len;
	while(resp_len) {
	    try {
		resp_len = tr.at().messIO(NULL, 0, io_buf + l, 8 - l, 0, true);
	    } catch (TError er) {
		resp_len = 0;
	    }
	    l += resp_len;
	}
	if((l == 8) && (3 <= (unsigned char) io_buf[2]) && (TSYS::getUnalign16(io_buf + 6) == CRC(io_buf + 2, 4))) {
	    uint16_t x = (unsigned char) io_buf[2] - 3;
	    uint16_t y = x >> 4;
	    if(x & 0x000F) y++;
	    y <<= 1; // CRC
	    y += x;

	    //> Wait tail
	    if(y) {
		do {
		    try {
			resp_len = tr.at().messIO(NULL, 0, io_buf + l, y, 0, true);
		    } catch (TError er) {
			resp_len = 0;
		    }
		    l += resp_len;
		    y -= resp_len;
		} while(resp_len);
	    }
	} else {
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("bad header found!"));
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("l %d"), l);
	    for(int i = 0; i < l; i++) {
		data_s += TSYS::int2str((uint8_t) io_buf[i], TSYS::Hex) + " ";
	    }
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("io_buf: %s"), data_s.c_str());
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("CRC %04X"), CRC(io_buf + 2, 4));

	}
	errPresent = false;
	if(l) {
	    rc = VerifyPacket(io_buf, &l);
	    if(!rc) {
		rc = ParsePacket(io_buf, l, pMsg);

		if(rc == 1) {
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Parse error %d"), rc);
		    pMsg->L = 0;
		} else {

		}

	    } else {
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Verify error %d"), rc);
		data_s = "";
		for(int i = 0; i < l; i++) {
		    data_s += TSYS::int2str((uint8_t) io_buf[i], TSYS::Hex) + " ";
		}
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("io_buf: %s"), data_s.c_str());
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("l %d"), l);
		pMsg->L = 0;
	    }

	} else {
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Receive error %d"), l);
	    pMsg->L = 0;
	}

	if(pMsg->L) switch(Cmd) {
	case Reset:
	case ResetChan:
	    Channels[pMsg->B].FCB2 = 0x20;
	    break;
	case SetData:
	    Channels[pMsg->B].FCB2 ^= 0x20;
	    break;
	case ReqData1:
	case ReqData2:
	case ReqData:

	    Channels[pMsg->B].FCB3 ^= 0x20;
	    break;

	}
    } catch (...) {
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("messIO error"));
    }
    return pMsg->L;
}

void TMdContr::MakePacket(tagMsg *msg, char *io_buf, uint16_t *len)
{
    uint16_t x, y, l, z;
    uint16_t w;
    if((msg->L == 1) && ((msg->C & 0x0F) == ReqData)) {
	//one byte req
	*io_buf = (char) (~msg->A & 0x3F) | 0x80;
	*len = 1;
    } else {
	//full packet
	*(uint16_t *) io_buf = 0x6405;
	io_buf[2] = msg->L;
	io_buf[3] = msg->C | 0x40;
	io_buf[4] = msg->A;
	io_buf[5] = msg->B;
	*(uint16_t *) (io_buf + 6) = CRC(io_buf + 2, 4);
	//CRC
	x = 0;
	y = 8;
	l = (int) msg->L - 3;
	while(x < l) {
	    z = l - x;
	    if(z > 16) z = 16;
	    w = CRC((char *) (msg->D + x), z);
	    for(z; z > 0; z--)
		io_buf[y++] = msg->D[x++];
	    io_buf[y] = (w) & 0xFF;
	    io_buf[y + 1] = (w >> 8) & 0xFF;
	    y += 2;
	}
	*len = y;
    }
}

bool TMdContr::VerCRC(char *p, uint16_t l)
{
    l -= 2;
    uint16_t leng = (uint8_t) p[2], lD;
    if(*(uint16_t*) (p + 6) != CRC(p + 2, 4)) return 0;
    if(leng > 3) {
	leng -= 3;
	lD = leng / 16;
	leng %= 16;
	for(uint8_t i = 0; i < lD; i++) {
	    if(TSYS::getUnalign16(p + 8 + ((i + 1) * 16) + (i * 2)) != CRC((p + 8 + (i * 16) + (i * 2)), 16)) return 0;
	}
	if(leng) if(TSYS::getUnalign16(p + l) != CRC((p + (l - leng)), leng)) return 0;
    }
    return 1;
}

uint16_t TMdContr::VerifyPacket(char *t, uint16_t *l)
{
    uint16_t raslen;
//    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("in VerifyPacket l %d"), *l);
    string data_s = "";
    for(int i = 0; i < *l; i++) {
	data_s += TSYS::int2str((uint8_t) t[i], TSYS::Hex) + " ";
    }
    //   if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("in VerifyPacket io_buf: %s"), data_s.c_str());
    //   if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("l %d"), *l);
    if((*l == 1)) {
	//one byte req
	return 0;
    } else {
	//full packet
	if(*l > 7) {

	    if((t[0] == 0x05) && (t[0] != 0x64)) {
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Len -%d"), Len((uint8_t )t[2]));
//		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("VerCRC -%d"), VerCRC(t, *l));
		if(!((raslen = Len((uint8_t) t[2])) == *l && VerCRC(t, *l))) {
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("good header"));
		    if(!(*l > raslen && VerCRC(t, raslen))) {
			if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("bad packet"));
			return 2;    //wrong packet
		    } else {
			if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("cutting packet"));
			*l = raslen; //corrupted end of packet
		    }
		}

	    } else {
		return 1; //no start bytes
	    }
	} else {
	    return 3; //not a packet
	}
    }
    return 0;
}

uint16_t TMdContr::ParsePacket(char *t, uint16_t l, tagMsg * msg)
{
    if(l == 1) {
	if(((*t) & 0x3F) == msg->A) {
	    msg->L = 1;
	    uint8_t tt = msg->A;
	    msg->A = msg->B;
	    msg->B = tt;
	    if((*t) & 0xC0)
		msg->C = (((*t) >> 1) & 0x20) | GOOD3;
	    else
		msg->C = BAD3;
	    return 0;
	} else {
	    if((*t) == ((uint8_t) (~msg->A & 0x3F) | 0x80)) {
		return 2;
	    }
	    return 1;
	}
    } else {
	if((msg->A == t[5]) && (msg->B == t[4])) {
	    uint16_t x, y, z;
	    y = 0;
	    x = 8;
	    msg->L = t[2];
	    msg->C = t[3] & 0xF;
	    msg->A = t[4];
	    msg->B = t[5];
	    while(x < l) {
		z = l - x;
		if(z < 18)
		    z -= 2;
		else
		    z = 16;
		for(z; z > 0; z--)
		    msg->D[y++] = t[x++];
		x += 2;
	    }
	    return 0;
	} else {
	    if((msg->A == t[4]) && (msg->B == t[5]))
		return 2;
	    else
		return 1;
	}

    }
    return 1;
}

uint16_t TMdContr::Len(uint16_t l)
{
    int lD = 0, lP;

    if(l > 3) {
	lP = l - 3;
	lD = lP / 16;
	if(lD != 0) lD *= 2;
	if((lP % 16) != 0) lD += 2;
    }
    return (l += 5 + lD);
}

TMdContr::~TMdContr()
{
    if(startStat()) stop();

//    delete[] BE;
    pthread_mutex_destroy(&enRes);
    pthread_mutex_destroy(&eventRes);
}

string TMdContr::getStatus()
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) rez += TSYS::strMess(_("Gather data time %.6g ms. "), tm_gath);
    return rez;
}

bool TMdContr::isLogic()
{
    if(cfg("CTRTYPE").getS() == "Logic") {
	return true;
    } else {
	return false;
    }
}

TParamContr *TMdContr::ParamAttach(const string &name, int type)
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::start_()
{
    nChannel = cfg("NCHANNEL").getI();
    Channels.clear();
    for(int i = 0; i <= nChannel; i++) {

	Channels.push_back(TFT3Channel());

    }
    devAddr = vmin(63, vmax(1,cfg("NODE").getI()));
    //> Start the gathering data task
    if(!prc_st) {
	if(cfg("CTRTYPE").getS() == "DAQ") {
	    SYS->taskCreate(nodePath('.', true), mPrior, TMdContr::DAQTask, this);
	} else {
	    if(cfg("CTRTYPE").getS() == "Logic") {
		SYS->taskCreate(nodePath('.', true), mPrior, TMdContr::LogicTask, this);
	    }
	}
    }
}

void TMdContr::stop_()
{
    //> Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.', true), &endrun_req);
}

void TMdContr::prmEn(TMdPrm *prm, bool val)
{
    unsigned i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
	if(&pHd[i_prm].at() == prm) break;

    if(val && i_prm >= pHd.size()) pHd.push_back(prm);
    if(!val && i_prm < pHd.size()) pHd.erase(pHd.begin() + i_prm);
}

void *TMdContr::DAQTask(void *icntr)
{
    TMdContr &cntr = *(TMdContr *) icntr;
    cntr.endrun_req = false;
    cntr.prc_st = true;
    tagMsg Msg;
    while(!cntr.endrun_req) {
	long long t_cnt = TSYS::curTime();
	MtxAlloc prmRes(cntr.enRes, true);
	if(cntr.NeedInit) {
	    Msg.L = 3;
	    Msg.C = ResetChan;
	    if(cntr.DoCmd(&Msg)) {
		Msg.L = 3;
		Msg.C = ResData2;
		if(cntr.DoCmd(&Msg)) {
		    cntr.NeedInit = false;
		}
	    }
	} else {
	    vector<string> lst;
	    cntr.list(lst);
	    for(int i_l = 0; i_l < lst.size(); i_l++) {
		AutoHD<TMdPrm> t = cntr.at(lst[i_l]);
		t.at().Task(TaskIdle);
	    }
	    Msg.L = 3;
	    Msg.C = ReqData;
	    cntr.DoCmd(&Msg);
	}
	prmRes.unlock();

	cntr.tm_gath = 1e-3 * (TSYS::curTime() - t_cnt);

	//!!! Wait for next iteration
	TSYS::taskSleep((long long) (1e9 * cntr.period()));
    }

    cntr.prc_st = false;

    return NULL;
}

void *TMdContr::LogicTask(void *icntr)
{
    TMdContr &cntr = *(TMdContr *) icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;
    tagMsg Msg;
    while(!cntr.endrun_req) {
	long long t_cnt = TSYS::curTime();
	MtxAlloc prmRes(cntr.enRes, true);
	//TODO FT3 logic handler
	vector<string> lst;
	cntr.list(lst);
	for(int i_l = 0; i_l < lst.size(); i_l++) {
	    MtxAlloc res(cntr.eventRes, true);
	    AutoHD<TMdPrm> t = cntr.at(lst[i_l]);
	    t.at().tmHandler();
	}

	prmRes.unlock();

	cntr.tm_gath = 1e-3 * (TSYS::curTime() - t_cnt);

	//!!! Wait for next iteration
	TSYS::taskSleep((long long) (1e9 * cntr.period()));
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc(XMLNode *opt)
{

    //> Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld", opt, -1, "/cntr/cfg/ADDR", cfg("ADDR").fld().descr(), enableStat() ? R_R_R_ : RWRWR_, "root", SDAQ_ID, 3, "tp", "str", "dest",
		"select", "select", "/cntr/cfg/trLst");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt)) {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    } else
	TController::cntrCmdProc(opt);
}
//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm(string name, TTypeParam *tp_prm) :
	TParamContr(name, tp_prm), p_el("w_attr"), mDA(NULL), needApply(false)
{

}

TMdPrm::~TMdPrm()
{
    nodeDelAll();
}

TElem& TMdPrm::prmIOE()
{
    return mod->prmIOE();
}

void TMdPrm::postEnable(int flag)
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el)) vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner()
{
    return (TMdContr&) TParamContr::owner();
}

void TMdPrm::enable()
{
    if(enableStat()) return;

    TParamContr::enable();
    //> Delete DAQ parameter's attributes
    for(unsigned i_f = 0; i_f < p_el.fldSize();) {
	try {
	    p_el.fldDel(i_f);
	} catch (TError &err) {
	    mess_warning(err.cat.c_str(), err.mess.c_str());
	    i_f++;
	}
    }

    //> Connect device's code
    if(owner().cfg("PRTTYPE").getS() == "KA") {
	if(type().name == "tp_BUC") mDA = new KA_BUC(*this, cfg("DEV_ID").getI());
	if(type().name == "tp_BVTS") mDA = new KA_BVTC(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_BVT") mDA = new KA_BVT(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_GZD") mDA = new KA_GZD(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_GNS") mDA = new KA_GNS(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_BTU") mDA = new KA_BTU(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
    } else {
	if(type().name == "tp_BUC") mDA = new B_BUC(*this, cfg("DEV_ID").getI(), cfg("MOD").getI());
	if(type().name == "tp_BVI") mDA = new B_BVI(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB(), cfg("EXT_PERIOD").getB());
	if(type().name == "tp_BVTS") mDA = new B_BVTC(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_BVT")
	    mDA = new B_BVT(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB(), cfg("WITH_KPARAMS").getB(),
		    cfg("WITH_RATEPARAMS").getB());
	if(type().name == "tp_BIP") mDA = new B_BIP(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_PAUK") mDA = new B_PAUK(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_BTU") mDA = new B_BTR(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), 0, cfg("WITH_PARAMS").getB());
	if(type().name == "tp_ACCOUNT") mDA = new B_ACCOUNT(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_BTR")
	    mDA = new B_BTR(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNTU").getI(), cfg("CHAN_COUNTR").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_BTE") mDA = new B_BTE(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
	if(type().name == "tp_ODOR") mDA = new B_ODOR(*this, cfg("DEV_ID").getI(), cfg("CHAN_COUNT").getI(), cfg("WITH_PARAMS").getB());
    }
    if(mDA == NULL) throw TError(nodePath().c_str(), _("No one device selected."));

    owner().prmEn(this, true);	//Put to process

    needApply = false;
}

void TMdPrm::disable()
{
    if(!enableStat()) return;

    owner().prmEn(this, false);	//Remove from process

    TParamContr::disable();

    if(mDA) delete mDA;
    mDA = NULL;

    needApply = false;
}

uint16_t TMdPrm::Task(uint16_t cod)
{
    if(mDA) {
	if(mDA->IsNeedUpdate()) mDA->Task(TaskRefresh);
	return mDA->Task(cod);
    } else {
	return 0;
    }

}

uint16_t TMdPrm::HandleEvent(time_t tm, uint8_t * D)
{
    if(mDA) {
	return mDA->HandleEvent(tm, D);
    } else {
	return 0;
    }

}

void TMdPrm::tmHandler()
{
    if(mDA) {
	mDA->tmHandler();
    } else {
	return;
    }

}

uint8_t TMdPrm::cmdGet(uint16_t prmID, uint8_t * out)
{
    if(mDA) {
	return mDA->cmdGet(prmID, out);
    } else {
	return 0;
    }
}

uint8_t TMdPrm::cmdSet(uint8_t * req, uint8_t addr)
{
    if(mDA) {
	return mDA->cmdSet(req, addr);
    } else {
	return 0;
    }
}

void TMdPrm::vlSet(TVal &vo, const TVariant &vl, const TVariant &pvl)
{
    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("TMdPrm::vlSet name %s "), vo.name().c_str());
    if(!enableStat() || !owner().startStat()) vo.setS(EVAL_STR, 0, true);

    if(vl.isEVal() || vl == pvl) return;

//Send to active reserve station
    if(owner().redntUse()) {
	XMLNode req("set");
	req.setAttr("path", nodePath(0, true) + "/%2fserv%2fattr")->childAdd("el")->setAttr("id", vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(), req);
	return;
    }
    if(mDA) {
	mDA->setVal(vo);
    } else {
	return;
    }

}

void TMdPrm::vlGet(TVal &val)
{
    if(val.name() == "err") {
	TParamContr::vlGet(val);
	string st = TSYS::strParse(val.getS(NULL, true), 0, ":");
	if(st == "1" || st == "2") {

	} else {
	    val.setS(mDA->getStatus(), 0, true);
	}
    }
}

void TMdPrm::load_()
{
    TParamContr::load_();
    if(enableStat() && mDA) mDA->loadIO();
}

void TMdPrm::save_()
{
    TParamContr::save_();
    if(enableStat() && mDA) mDA->saveIO();
}

string TMdPrm::typeDBName()
{
    return type().DB(&owner());
}

void TMdPrm::cntrCmdProc(XMLNode *opt)
{
    string a_path = opt->attr("path");
    if(a_path.substr(0, 6) == "/serv/") {
	TParamContr::cntrCmdProc(opt);
	return;
    }

//> Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	if(owner().isLogic() && ctrMkNode("area", opt, -1, "/cfg", _("Parameters configuration"))) {
	    if(ctrMkNode("area", opt, -1, "/cfg/prm", _("Parameters"))) {
		if(mDA) {
		    for(int i_io = 0; i_io < mDA->lnkSize(); i_io++) {
			ctrMkNode("fld", opt, -1, (string("/cfg/prm/pr_") + mDA->lnk(i_io).prmName).c_str(), mDA->lnk(i_io).prmDesc, RWRWR_, "root", SDAQ_ID, 3,
				"tp", "str", "dest", "sel_ed", "select", (string("/cfg/prm/pl_") + mDA->lnk(i_io).prmName).c_str());
		    }
		}

	    }

	}

	return;
    }
    if(a_path.substr(0, 12) == "/cfg/prm/pr_") {
	if(ctrChkNode(opt, "get", RWRWR_, "root", SDAQ_ID, SEC_RD)) {
	    string lnk_val = mDA->lnk(mDA->lnkId((a_path.substr(12)))).prmAttr;
	    if(!SYS->daq().at().attrAt(lnk_val, '.', true).freeStat()) {
		opt->setText(lnk_val + " (+)");
	    } else
		opt->setText(lnk_val);
	}
	if(ctrChkNode(opt, "set", RWRWR_, "root", SDAQ_ID, SEC_WR)) {
	    string no_set;
	    mDA->lnk(mDA->lnkId((a_path.substr(12)))).prmAttr = opt->text();
	    mDA->lnk(mDA->lnkId((a_path.substr(12)))).aprm = SYS->daq().at().attrAt(mDA->lnk(mDA->lnkId((a_path.substr(12)))).prmAttr, '.', true);
	    modif();
	}
    } else if((a_path.compare(0, 12, "/cfg/prm/pl_") == 0) && ctrChkNode(opt)) {
	string m_prm = mDA->lnk(mDA->lnkId((a_path.substr(12)))).prmAttr;
	;
	if(!SYS->daq().at().attrAt(m_prm, '.', true).freeStat()) m_prm = m_prm.substr(0, m_prm.rfind("."));
	SYS->daq().at().ctrListPrmAttr(opt, m_prm, false, '.');
    }
    TParamContr::cntrCmdProc(opt);

}

void TMdPrm::vlArchMake(TVal &val)
{
    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr, val.arch().at().srcData());
    val.arch().at().setPeriod((long long) (owner().period() * 1000000));
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

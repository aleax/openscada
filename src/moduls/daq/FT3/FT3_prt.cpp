//OpenSCADA system module Protocol.FT3 file: FT3_prt.cpp
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
#include <getopt.h>
#include <string.h>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

#include "mod_FT3.h"
#include "FT3_prt.h"

FT3::TProt *FT3::modPrt;

using namespace FT3;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt(string name) :
	TProtocol(PRT_ID)
{
    modPrt = this;
    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TProt::~TProt()
{
}

void TProt::load_()
{

}

void TProt::save_()
{

}

TProtocolIn *TProt::in_open(const string &name)
{
    return new TProtIn(name);
}

uint16_t TProt::CRC(const char *data, uint16_t length)
{
    uint16_t CRC = 0, buf;
    uint16_t i, j;
    for(i = 0; i < length; i++) {
	CRC ^= ((uint8_t) data[i] << 8);
	//  X16+X13+X12+X11+X10+X8+X6+X5+X2+1
	for(j = 0; j < 8; j++) {
	    buf = CRC;
	    CRC <<= 1;
	    if(buf & 0x8000) CRC ^= 0x3D65;
	}
    }

    return ~CRC;
}

void TProt::MakePacket(string &pdu, tagMsg * msg)
{
    uint16_t x, y, l, z;
    uint16_t w;
    if((msg->L == 1)) {
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("MakePacket one byte req %02X"), msg->C);
	// one byte req
	pdu += (char) msg->C;
    } else {
	// full packet
	pdu += (char) 0x05;
	pdu += (char) 0x64;
	pdu += (char) msg->L;
	pdu += (char) msg->C;
	pdu += (char) msg->A;
	pdu += (char) msg->B;
	uint16_t crc = CRC(pdu.data() + 2, 4);
	pdu += crc;
	pdu += crc >> 8;
	x = 0;
	l = (int) msg->L - 3;
	while(x < l) {
	    z = l - x;
	    if(z > 16) z = 16;
	    w = CRC((char *) (msg->D + x), z);
	    for(; z > 0; z--)
		pdu += msg->D[x++];
	    pdu += w;
	    pdu += w >> 8;
	}
    }

}

bool TProt::VerCRC(string &pdu, int l)
{
    l -= 2;
    uint8_t leng = pdu[2];
    uint16_t lD;
    if((uint16_t) TSYS::getUnalign16(pdu.data() + 6) != CRC(pdu.data() + 2, 4)) return 0;
    if(leng > 3) {
	leng -= 3;
	lD = leng / 16;
	leng %= 16;
	for(uint16_t i = 0; i < lD; i++) {
	    if((uint16_t) TSYS::getUnalign16(pdu.data() + 8 + ((i + 1) * 16) + (i * 2)) != CRC((pdu.data() + 8 + (i * 16) + (i * 2)), 16)) return 0;
	}
	if(leng) if((uint16_t) TSYS::getUnalign16(pdu.data() + l) != CRC(pdu.data() + (l - leng), leng)) return 0;
    }
    return 1;
}

//---------------------------------------------------------------------------
uint16_t TProt::VerifyPacket(string &pdu)
{
    uint16_t raslen;
    if(pdu.size() == 1) {
	//one byte req
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("VerifyPacket one byte req"));
	return 0;
    } else {
	//normal packet
	if(pdu.size() > 7) {
	    if((pdu[0] == 0x05) && (pdu[0] != 0x64)) {
		if(!((raslen = Len(pdu[2])) == pdu.size() && VerCRC(pdu, pdu.size())))
		    if(!(pdu.size() > raslen && VerCRC(pdu, raslen))) {
			if(mess_lev() == TMess::Debug)
			    mess_sys(TMess::Debug, _("VerifyPacket bad packet pdu.size:%d raslen:%d VerCRC(pdu, pdu.size()):%d VerCRC(pdu, raslen):%d"),
				    pdu.size(), raslen, VerCRC(pdu, pdu.size()), VerCRC(pdu, raslen));
			return 2; //wrong packet
		    } else {
			if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("VerifyPacket bad tail"));
			pdu.erase(raslen); //trash in tail
		    }
	    } else {
		if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("VerifyPacket no header"));
		return 1; //no start sequence
	    }
	} else {
	    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("VerifyPacket no packet"));
	    return 3; //not a packet
	}
    }
    return 0;
}
//---------------------------------------------------------------------------
uint16_t TProt::ParsePacket(string &pdu, tagMsg * msg)
{
    if(pdu.size() >= 8) {
	uint16_t x, y, z;
	y = 0;
	x = 8;
	msg->L = pdu[2];
	msg->C = pdu[3];
	msg->A = pdu[4];
	msg->B = pdu[5];
	while(x < pdu.size()) {
	    z = pdu.size() - x;
	    if(z < 18)
		z -= 2;
	    else
		z = 16;
	    for(; z > 0; z--)
		msg->D[y++] = pdu[x++];
	    x += 2;
	}
    }
    if((pdu.size() == 1) && ((pdu[0] & 0xC0) == 0x80)) {
	msg->L = 1;
	msg->C = pdu[0];
	msg->A = (~pdu[0]) & 0x3F;
	msg->B = 0;
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("ParsePacket one byte req C %02X A %02X"), msg->C, msg->A);
    }
    return 0;
}
//-------------------------------------------------------------------------------
uint16_t TProt::Len(uint8_t l)
{
    uint16_t lD = 0, lP;

    if(l > 3) {
	lP = l - 3;
	lD = lP / 16;
	if(lD != 0) lD *= 2;
	if((lP % 16) != 0) lD += 2;
    }
    return (l += 5 + lD);
}
//-------------------------------------------------------------------------------
void TProt::cntrCmdProc(XMLNode *opt)
{
    //> Get page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn(string name) :
	TProtocolIn(name)
{
    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("new TProtIn %s"), name.c_str());
}

TProtIn::~TProtIn()
{
    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("delete TProtIn"));
}

TProt &TProtIn::owner()
{
    return *(TProt*) nodePrev();
}

bool TProtIn::mess(const string &ireqst, string &answer)
{
    string reqst = ireqst;
    string data_s;
    for(int i = 0; i < reqst.size(); i++) {
	data_s += TSYS::int2str((uint8_t) reqst[i], TSYS::Hex) + " ";
    }
    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("request: %s"), data_s.c_str());
    if(modPrt->VerifyPacket(reqst) == 0) {
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("Correct packet found!"));
	tagMsg msg, msgOut;
	if(modPrt->ParsePacket(reqst, &msg) == 0) {
	    unsigned i_l;
	    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("Packet parsed!"), data_s.c_str());
	    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("L:%02d C:%02X A:%02d B:%02d "), msg.L, msg.C, msg.A, msg.B);
	    //finding DAQ
	    vector<string> lst;
	    SYS->daq().at().at("FT3").at().list(lst);
	    for(i_l = 0; i_l < lst.size(); i_l++) {
		AutoHD<TMdContr> t = SYS->daq().at().at("FT3").at().at(lst[i_l]);
		if((t.at().cfg("CTRTYPE").getS() == "Logic") && (t.at().devAddr == msg.A) && (msg.B <= t.at().nChannel)) {
		    answer = "";
		    if(t.at().ProcessMessage(&msg, &msgOut)) {

			modPrt->MakePacket(answer, &msgOut);
		    }
		}

	    }

	    if(i_l >= lst.size()) return true;
	}
    }
    return true;
}

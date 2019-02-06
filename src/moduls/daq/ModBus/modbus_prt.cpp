
//OpenSCADA module Protocol.ModBus file: modbus_prt.cpp
/***************************************************************************
 *   Copyright (C) 2008-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <string.h>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

#include "modbus_daq.h"
#include "modbus_prt.h"

ModBus::TProt *ModBus::modPrt;

using namespace ModBus;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(PRT_ID), mPrtLen(0)
{
    modPrt	= this;

    modInfoMainSet(PRT_NAME, PRT_TYPE, PRT_MVER, PRT_AUTHORS, PRT_DESCR, PRT_LICENSE, name);

    mNode = grpAdd("n_");

    //Node DB structure
    mNodeEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mNodeEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mNodeEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    mNodeEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mNodeEl.fldAdd(new TFld("ADDR",_("Address"),TFld::Integer,0,"3","1","1;247"));
    mNodeEl.fldAdd(new TFld("InTR",_("Input transport"),TFld::String,0,OBJ_ID_SZ,"*"));
    mNodeEl.fldAdd(new TFld("PRT",_("Protocol"),TFld::String,TFld::Selectable,"5","*","RTU;ASCII;TCP;*",_("RTU;ASCII;TCP/IP;All")));
    mNodeEl.fldAdd(new TFld("MODE",_("Mode"),TFld::Integer,TFld::Selectable,"1","0",
	TSYS::strMess("%d;%d;%d",Node::MD_DATA,Node::MD_GT_ND,Node::MD_GT_NET).c_str(),_("Data;Gateway node;Gateway net")));
    mNodeEl.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));
    // For "Data" mode
    mNodeEl.fldAdd(new TFld("DT_PER",_("Period of the data calculation, seconds"),TFld::Real,0,"5.3","1","0.001;99"));
    mNodeEl.fldAdd(new TFld("DT_PR_TR",_("Completely translate the procedure"),TFld::Boolean,TFld::NoFlag,"1","0"));
    mNodeEl.fldAdd(new TFld("DT_PROG",_("Procedure"),TFld::String,TFld::TransltText,"1000000"));
    // For "Gateway" mode
    mNodeEl.fldAdd(new TFld("TO_TR",_("To output transport"),TFld::String,0,OBJ_ID_SZ));
    mNodeEl.fldAdd(new TFld("TO_PRT",_("To protocol"),TFld::String,TFld::Selectable,"5","RTU","RTU;ASCII;TCP","RTU;ASCII;TCP/IP"));
    mNodeEl.fldAdd(new TFld("TO_ADDR",_("To address"),TFld::Integer,0,"3","1","1;247"));

    //Node data IO DB structure
    mNodeIOEl.fldAdd(new TFld("NODE_ID",_("Node ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mNodeIOEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mNodeIOEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mNodeIOEl.fldAdd(new TFld("TYPE",_("Value type"),TFld::Integer,TFld::NoFlag,"1"));
    mNodeIOEl.fldAdd(new TFld("FLAGS",_("Flags"),TFld::Integer,TFld::NoFlag,"4"));
    mNodeIOEl.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::TransltText,"100"));
    mNodeIOEl.fldAdd(new TFld("POS",_("Real position"),TFld::Integer,TFld::NoFlag,"4"));
}

TProt::~TProt( )
{
    nodeDelAll();
}

string TProt::nAdd( const string &iid, const string &db )
{
    Node *obj = new Node(TSYS::strEncode(sTrm(iid),TSYS::oscdID), db, &nodeEl());
    MtxAlloc res(chM(), true);
    chldAdd(mNode, obj);

    return obj->id();
}

void TProt::load_( )
{
    //Load parameters from command line

    //Load DB
    // Search and create new nodes
    try {
	TConfig gCfg(&nodeEl());
	//gCfg.cfgViewAll(false);
	vector<string> dbLs;
	map<string, bool> itReg;
	vector<vector<string> > full;

	//  Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+modId()+"_node",nodePath()+modId()+"_node",fldCnt++,gCfg,false,&full); ) {
		string id = gCfg.cfg("ID").getS();
		if(!nPresent(id)) nAdd(id, (dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		nAt(id).at().load(&gCfg);
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    nList(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(nAt(dbLs[i_it]).at().DB()))
		    nDel(dbLs[i_it]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Error of searching and a new node creation."));
    }
}

void TProt::save_( )
{

}

void TProt::modStart( )
{
    vector<string> ls;
    nList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	if(nAt(ls[iN]).at().toEnable())
	    nAt(ls[iN]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    nList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	nAt(ls[iN]).at().setEnable(false);
}

TProtocolIn *TProt::in_open( const string &name )	{ return new TProtIn(name); }

uint8_t TProt::CRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

uint8_t TProt::CRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t TProt::CRC16( const string &mbap )
{
    uint8_t hi = 0xFF;
    uint8_t lo = 0xFF;
    uint16_t index;
    for(unsigned i_b = 0; i_b < mbap.size(); i_b++) {
	index = lo^(uint8_t)mbap[i_b];
	lo = hi^CRCHi[index];
	hi = CRCLo[index];
    }
    return hi|(lo<<8);
}

uint8_t TProt::LRC( const string &mbap )
{
    uint8_t ch = 0;
    for(unsigned i_b = 0; i_b < mbap.size(); i_b++)
	ch += (uint8_t)mbap[i_b];

    return -ch;
}

string TProt::DataToASCII( const string &in )
{
    uint8_t ch;
    string rez;

    for(unsigned i = 0; i < in.size(); i++) {
	ch = (in[i]&0xF0)>>4;
	rez += (ch + ((ch<=9)?'0':('A'-10)));
	ch = in[i]&0x0F;
	rez += (ch + ((ch<=9)?'0':('A'-10)));
    }

    return rez;
}

string TProt::ASCIIToData( const string &in )
{
    uint8_t ch1, ch2;
    string rez;

    for(unsigned i = 0; i < (in.size()&(~0x01)); i += 2) {
	ch1 = in[i];
	if(ch1 >= '0' && ch1 <= '9')		ch1 -= '0';
	else if(ch1 >= 'A' && ch1 <= 'F')	ch1 -= ('A'-10);
	else					ch1 = 0;
	ch2 = ch1 << 4;
	ch1 = in[i+1];
	if(ch1 >= '0' && ch1 <= '9')		ch1 -= '0';
	else if(ch1 >= 'A' && ch1 <= 'F')	ch1 -= ('A'-10);
	else					ch1 = 0;
	rez += ch2|ch1;
    }

    return rez;
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string mbap, err, rez;
    char buf[1000];

    MtxAlloc resN(tro.reqRes(), true);

    string prt   = io.name();
    string sid   = io.attr("id");
    string debugCat = io.attr("debugCat");
    int    reqTm = s2i(io.attr("reqTm"));
    int    node  = s2i(io.attr("node"));
    int    reqTry = vmin(10, vmax(1,s2i(io.attr("reqTry"))));
    string pdu   = io.text();

    try {
	if(!tro.startStat()) tro.start();
	if(prt == "TCP") {		// Modbus/TCP protocol process
	    //Encode MBAP (Modbus Application Protocol)
	    int tid = rand();
	    mbap.reserve(pdu.size()+7);
	    mbap.append((char*)&tid,2);		//Transaction ID
	    mbap.append(2,(char)0);		//Protocol ID
	    mbap += (char)((pdu.size()+1)>>8);	//PDU size MSB
	    mbap += (char)(pdu.size()+1);	//PDU size LSB
	    mbap += (char)node;			//Unit identifier
	    mbap += pdu;

	    //Send request
	    int resp_len = tro.messIO(mbap.data(), mbap.size(), buf, sizeof(buf), reqTm);
	    rez.assign(buf,resp_len);
	    if(rez.size() < 7)	err = _("13:Error of the server response");
	    else {
		unsigned resp_sz = (unsigned short)(rez[4]<<8) | (unsigned char)rez[5];

		//Wait tail
		while(rez.size() < (resp_sz+6) && rez.size() < MODBUS_FRM_LIM) {
		    resp_len = tro.messIO(NULL, 0, buf, sizeof(buf), reqTm);
		    if(!resp_len) throw TError(nodePath().c_str(),_("Not full response"));
		    rez.append(buf, resp_len);
		}
		if(rez.size() >= MODBUS_FRM_LIM)
		    throw TError(nodePath().c_str(), _("13:Error of the response: Too large."));
		pdu = rez.substr(7);
	    }
	}
	else if(prt == "RTU") {		// Modbus/RTU protocol process
	    mbap.reserve(pdu.size()+3);
	    mbap += (uint8_t)node;		//Unit identifier
	    mbap += pdu;
	    uint16_t crc = CRC16( mbap );
	    mbap += (crc>>8);
	    mbap += crc;

	    //Send request
	    for(int i_tr = 0, resp_len = 0; i_tr < reqTry; i_tr++) {
		try {
		    resp_len = tro.messIO(mbap.data(), mbap.size(), buf, sizeof(buf), reqTm);
		    rez.assign(buf, resp_len);
		    //Wait tail
		    while(resp_len && rez.size() < MODBUS_FRM_LIM) {
			try { resp_len = tro.messIO(NULL, 0, buf, sizeof(buf)); } catch(TError &err) { break; }
			rez.append(buf, resp_len);
		    }
		} catch(TError &er) {	//By possible the send request breakdown and no response
		    if(err.empty()) err = _("14:Device error: ") + er.mess;
		    else if(err.find(er.mess) == string::npos) err += "; " + er.mess;
		    continue;
		}

		if(rez.size() < 2) { err = _("13:Error of the response: Too short."); continue; }
		if(rez.size() >= MODBUS_FRM_LIM) { err = _("13:Error of the response: Too large."); continue; }
		if(CRC16(rez.substr(0,rez.size()-2)) != (uint16_t)((rez[rez.size()-2]<<8)+(uint8_t)rez[rez.size()-1]))
		{ err = _("13:Error of the response: CRC error."); continue; }
		pdu = rez.substr(1, rez.size()-3);
		err = "";
		break;
	    }
	}
	else if(prt == "ASCII") {		// Modbus/ASCII protocol process
	    mbap.reserve(pdu.size()+2);
	    mbap += (uint8_t)node;		//Unit identifier
	    mbap += pdu;
	    mbap += LRC(mbap);
	    mbap = ":"+DataToASCII(mbap)+"\x0D\x0A";

	    //Send request
	    for(int i_tr = 0, resp_len = 0; i_tr < reqTry; i_tr++) {
		try {
		    resp_len = tro.messIO(mbap.data(), mbap.size(), buf, sizeof(buf), reqTm);
		    rez.assign(buf, resp_len);
		    //Wait tail
		    while(resp_len && (rez.size() < 3 || rez.substr(rez.size()-2,2) != "\x0D\x0A") && rez.size() < MODBUS_FRM_LIM) {
			try { resp_len = tro.messIO(NULL, 0, buf, sizeof(buf)); } catch(TError &err) { break; }
			rez.append(buf, resp_len);
		    }
		} catch(TError &er) {	//By possible the send request breakdown and no response
		    if(err.empty()) err = _("14:Device error: ") + er.mess;
		    else if(err.find(er.mess) != string::npos) err += "; " + er.mess;
		    continue;
		}

		if(rez.size() >= MODBUS_FRM_LIM) { err = _("13:Error of the response: Too large."); continue; }
		if(rez.size() < 3 || rez[0] != ':' || rez.substr(rez.size()-2,2) != "\x0D\x0A")
		{ err = _("13:Error of the response: Format error."); continue; }
		string rezEnc = ASCIIToData(rez.substr(1,rez.size()-3));
		if(LRC(rezEnc.substr(0,rezEnc.size()-1)) != (uint8_t)rezEnc[rezEnc.size()-1])
		{ err = _("13:Error of the response: LRC error."); continue; }
		pdu = rezEnc.substr(1,rezEnc.size()-2);
		err = "";
		break;
	    }
	}
	else err = TSYS::strMess(_("Error of the protocol '%s'."),prt.c_str());

	//Check response pdu
	if(err.empty()) {
	    if(pdu.size() < 2) err = _("13:Error of the response");
	    if(pdu[0]&0x80)
		switch(pdu[1]) {
		    case 0x1: err = TSYS::strMess(_("1:%02X:Function is not supported."),(unsigned char)(pdu[0]&(~0x80)));	break;
		    case 0x2: err = _("2:Request address is not allowed or the query range is too large.");	break;
		    case 0x3: err = _("3:Illegal value of the data in the request.");		break;
		    case 0x4: err = _("4:Server failure.");				break;
		    case 0x5: err = _("5:Request requires too long time for execute.");	break;
		    case 0x6: err = _("6:Server is busy.");				break;
		    case 0x7: err = _("7:Error in program function. Requested by the function 13 or 14.");	break;
		    case 0xA: case 0xB: err = _("10:Gateway problem.");			break;
		    default: err = TSYS::strMess(_("12:%02X:Unknown error."),(unsigned char)(pdu[1]));	break;
		}
	}
    } catch(TError &er) { err = _("14:Device error: ") + er.mess; }

    io.setText(err.empty()?pdu:"");
    if(!err.empty()) io.setAttr("err",err);

    //Prepare log
    if(prtLen() || debugCat.size()) {
	string mess = _("REQ -> ");
	if(prt != "ASCII") mess += TSYS::strDecode(mbap, TSYS::Bin, " ");
	else if(mbap.size() > 2) mess += mbap.substr(0,mbap.size()-2);
	mess += "\n";

	if(err.empty()) mess += _("RESP -> ");
	else mess += _("ERR -> ") + err + " -> ";
	if(prt != "ASCII") mess += TSYS::strDecode(rez, TSYS::Bin, " ");
	else if(rez.size() > 2) mess += rez.substr(0,rez.size()-2);

	if(prtLen())
	    pushPrtMess(atm2s(time(NULL))+" "+prt+": '"+sid+"' --> "+i2s(node)+"("+tro.workId()+")\n"+mess+"\n");
	if(debugCat.size()) mess_debug_(debugCat.c_str(), mess.c_str());
    }
}

void TProt::setPrtLen( int vl )
{
    MtxAlloc res(dataRes(), true);

    while((int)mPrt.size() > vl) mPrt.pop_back();

    mPrtLen = vl;
}

void TProt::pushPrtMess( const string &vl )
{
    MtxAlloc res(dataRes(), true);

    if(!prtLen()) return;

    mPrt.push_front(vl);

    while((int)mPrt.size() > prtLen())	mPrt.pop_back();
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/n_",_("Node"),RWRWR_,"root",SPRT_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/node",_("Nodes")))
	    ctrMkNode("list",opt,-1,"/node/node",_("Nodes"),RWRWR_,"root",SPRT_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","n_","idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,1,"/rep",_("Report"))) {
	    ctrMkNode("fld",opt,-1,"/rep/repLen",_("Report length"),RWRWR_,"root",SPRT_ID,4,"tp","dec","min","0","max","10000",
		"help",_("Use zero for the report disabling"));
	    if(prtLen())
		ctrMkNode("fld",opt,-1,"/rep/rep",_("Report"),R_R_R_,"root",SPRT_ID,3,"tp","str","cols","90","rows","20");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/n_" || a_path == "/node/node") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD)) {
	    vector<string> lst;
	    nList(lst);
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(nAt(lst[i_f]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))	{ opt->setAttr("id", nAdd(opt->attr("id"))); nAt(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))	chldDel(mNode,opt->attr("id"),-1,1);
    }
    else if(a_path == "/rep/repLen") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(prtLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setPrtLen(s2i(opt->text()));
    }
    else if(a_path == "/rep/rep" && ctrChkNode(opt)) {
	MtxAlloc res(dataRes(), true);
	for(unsigned iP = 0; iP < mPrt.size(); iP++)
	    opt->setText(opt->text() + mPrt[iP] + "\n");
    }
    else TProtocol::cntrCmdProc(opt);
}


//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

TProtIn::~TProtIn( )
{

}

TProt &TProtIn::owner( ) const	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &ireqst, string &answer )
{
    //Check for protocol type
    unsigned char node = 0;
    string prt, pdu;
    string reqst = ireqst;
    bool isBuf = false;

retry:
    // ASCII check
    if(reqst.size() > 3 && reqst[0] == ':' && reqst.substr(reqst.size()-2,2) == "\x0D\x0A") {
	prt = "ASCII";
	string req = modPrt->ASCIIToData(reqst.substr(1,reqst.size()-3));
	if(modPrt->LRC(req.substr(0,req.size()-1)) != (uint8_t)req[req.size()-1]) return false;
	node = req[0];
	pdu = req.substr(1, req.size()-2);
    }
    // RTU check
    else if(reqst.size() > 3 && reqst.size() <= 256 &&
	modPrt->CRC16(reqst.substr(0,reqst.size()-2)) == (uint16_t)((reqst[reqst.size()-2]<<8)+(uint8_t)reqst[reqst.size()-1]))
    {
	prt = "RTU";
	node = reqst[0];
	pdu = reqst.substr(1, reqst.size()-3);
    }
    // TCP check
    else if(reqst.size() > 7 && reqst.size() <= 260 &&
	reqst.size() == (unsigned)((unsigned short)(reqst[4]<<8)|(unsigned char)reqst[5])+6)
    {
	prt = "TCP";
	node = reqst[6];
	pdu = reqst.substr(7);
    }
    else {
	if(!isBuf && req_buf.size()) {
	    reqst = req_buf+reqst;
	    isBuf = true;
	    goto retry;
	}
	req_buf = reqst;
	if(req_buf.size() > 2048) req_buf = "";
	return true;
    }
    req_buf = "";

    vector<string> nls;
    modPrt->nList(nls);
    unsigned iN;
    for(iN = 0; iN < nls.size(); iN++)
	if(modPrt->nAt(nls[iN]).at().req(srcTr().at().workId(),prt,node,pdu)) break;
    if(iN >= nls.size()) return false;

    answer = "";
    if(prt == "TCP") {
	//Encode MBAP (Modbus Application Protocol)
	answer.reserve(pdu.size()+7);
	answer += reqst[0];			//Transaction ID MSB
	answer += reqst[1];			//Transaction ID LSB
	answer += reqst[2];			//Protocol ID MSB
	answer += reqst[3];			//Protocol ID LSB
	answer += (char)((pdu.size()+1)>>8);	//PDU size MSB
	answer += (char)(pdu.size()+1);		//PDU size LSB
	answer += (char)node;			//Unit identifier
	answer += pdu;
    }
    else if(prt == "RTU") {
	answer.reserve(pdu.size()+3);
	answer += (uint8_t)node;		//Unit identifier
	answer += pdu;
	uint16_t crc = modPrt->CRC16( answer );
	answer += crc>>8;
	answer += crc;
    }
    else if(prt == "ASCII") {
	answer.reserve(pdu.size()+2);
	answer += (uint8_t)node;		//Unit identifier
	answer += pdu;
	answer += modPrt->LRC(answer);
	answer = ":"+modPrt->DataToASCII(answer)+"\x0D\x0A";
    }

    if(owner().prtLen() && prt.size() && answer.size()) {
	string mess = atm2s(time(NULL))+" "+prt+": "+srcTr().at().workId()+
			"("+TSYS::strLine(srcAddr(),0)+") --> "+i2s(node)+"\n";
	mess += _("REQ -> ");
	if(prt != "ASCII")	mess += TSYS::strDecode(reqst, TSYS::Bin, " ");
	else if(reqst.size() > 2) mess += reqst.substr(0, reqst.size()-2);
	mess += "\n";

	mess +=_("RESP -> ");
	if(prt != "ASCII")	mess += TSYS::strDecode(answer, TSYS::Bin, " ");
	else if(answer.size() > 2) mess += answer.substr(0, answer.size()-2);

	owner().pushPrtMess(mess+"\n");
    }

    return false;
}

//*************************************************
//* Node: ModBus input protocol node.             *
//*************************************************
Node::Node( const string &iid, const string &idb, TElem *el ) :
    TFunction("ModBusNode_"+iid), TConfig(el), data(NULL), isDAQTmpl(false),
    mId(cfg("ID")), mName(cfg("NAME")), mDscr(cfg("DESCR")), mPer(cfg("DT_PER").getRd()), mAEn(cfg("EN").getBd()), mTimeStamp(cfg("TIMESTAMP").getId()),
    mEn(false), mDB(idb), prcSt(false), endRun(false), cntReq(0)
{
    mId = iid;

    cfg("MODE").setI(0);
    cfg("DT_PROG").setExtVal(true);
}

Node::~Node( )
{
    try{ setEnable(false); } catch(...) { }
    if(data) { delete data; data = NULL; }
}

TCntrNode &Node::operator=( const TCntrNode &node )
{
    Node *src_n = const_cast<Node*>(dynamic_cast<const Node*>(&node));
    if(!src_n) return *this;

    //Copy main parameters
    if(enableStat())	setEnable(false);
    exclCopy(*src_n, "ID;");
    *(TFunction*)this = *(TFunction*)src_n;
    setDB(src_n->DB());

    //Copy for current values and links (by the templates)
    if(src_n->mode() == MD_DATA && src_n->enableStat()) {
	setEnable(true);
	ResAlloc res(nRes, false);
	ResAlloc res1(src_n->nRes, false);
	for(int iIO = 0; iIO < src_n->data->func()->ioSize(); iIO++)
	    if(src_n->data->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
		data->lnkAddrSet(iIO, src_n->data->lnkAddr(iIO));
	    else data->set(iIO, src_n->data->get(iIO));
    }

    return *this;
}

void Node::postEnable( int flag )
{
    //Create default IOs
    if(flag&TCntrNode::NodeConnect) {
	ioIns(new IO("f_frq",_("Frequency of calculation of the function (Hz)"),IO::Real,TPrmTempl::LockAttr,"1000",false), 0);
	ioIns(new IO("f_start",_("Function start flag"),IO::Boolean,TPrmTempl::LockAttr,"0",false), 1);
	ioIns(new IO("f_stop",_("Function stop flag"),IO::Boolean,TPrmTempl::LockAttr,"0",false), 2);
    }
}

void Node::postDisable( int flag )
{
    if(flag) {
	SYS->db().at().dataDel(fullDB(), owner().nodePath()+tbl(), *this, true);
	TConfig cfg(&owner().nodeIOEl());
	cfg.cfg("NODE_ID").setS(id(), true);
	SYS->db().at().dataDel(fullDB()+"_io", owner().nodePath()+tbl()+"_io", cfg);
    }
}

TProt &Node::owner( ) const	{ return *(TProt*)nodePrev(); }

string Node::name( )
{
    string tNm = mName;
    return tNm.size() ? tNm : id();
}

string Node::tbl( )		{ return owner().modId()+"_node"; }

int Node::addr( ) const		{ return cfg("ADDR").getI(); }

string Node::inTransport( )	{ return cfg("InTR").getS(); }

string Node::prt( )		{ return cfg("PRT").getS(); }

Node::NodeModes Node::mode( )	{ return (Node::NodeModes)cfg("MODE").getI(); }

string Node::progLang( )
{
    string mProg = cfg("DT_PROG").getS();
    return mProg.substr(0, mProg.find("\n"));
}

string Node::prog( )
{
    string mProg = cfg("DT_PROG").getS();
    size_t lngEnd = mProg.find("\n");
    return mProg.substr(((lngEnd==string::npos)?0:lngEnd+1));
}

void Node::setProgLang( const string &ilng )
{
    cfg("DT_PROG").setS(ilng+"\n"+prog());
    modif();
}

void Node::setProg( const string &iprg )
{
    cfg("DT_PROG").setS(progLang()+"\n"+iprg);
    modif();
}

bool Node::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "DT_PR_TR") cfg("DT_PROG").setNoTransl(!progTr());
    else if(co.name() == "DT_PROG") {
	string  lfnc = TSYS::strParse(progLang(), 0, "."), wfnc = TSYS::strParse(progLang(), 1, ".");
	isDAQTmpl = SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc);
    }
    else if(co.name() == "MODE") {
	setEnable(false);
	//Hide all specific
	cfg("ADDR").setView(false); cfg("DT_PER").setView(false); cfg("DT_PR_TR").setView(false); cfg("DT_PROG").setView(false);
	cfg("TO_TR").setView(false); cfg("TO_PRT").setView(false); cfg("TO_ADDR").setView(false);

	//Show selected
	switch(co.getI()) {
	    case 0: cfg("ADDR").setView(true); cfg("DT_PER").setView(true); cfg("DT_PR_TR").setView(true); cfg("DT_PROG").setView(true);break;
	    case 1: cfg("ADDR").setView(true); cfg("TO_TR").setView(true); cfg("TO_PRT").setView(true); cfg("TO_ADDR").setView(true);	break;
	    case 2: cfg("TO_TR").setView(true); cfg("TO_PRT").setView(true);	break;
	}
    }

    modif();
    return true;
}

void Node::regCR( int id, const SIO &val, const string &tp, bool wr )
{
    bool varSec = false;
    map<int,SIO>::iterator it;
    if(tp == "C" || (varSec=(tp=="CI"))) {
	map<int,SIO> &blk = varSec ? data->coilI : (wr?data->coilW:data->coilR);
	if((it=blk.find(id)) == blk.end()) blk[id] = val;
	else mess_warning(nodePath().c_str(),
	    _("Coil(%s) %d already registered for IO#%d. IO#%d will be disabled for processing the coil %d!"),
	    tp.c_str(), id, it->second.id, val.id, id);
    }
    else if(tp == "R" || (varSec=(tp=="RI"))) {
	map<int,SIO> &blk = varSec ? data->regI : (wr?data->regW:data->regR);
	if((it=blk.find(id)) == blk.end()) blk[id] = val;
	else mess_warning(nodePath().c_str(),
	    _("Register(%s) %d already registered for IO#%d. IO#%d will be disabled for processing the register %d!"),
	    tp.c_str(), id, it->second.id, val.id, id);
    }
    else throw TError(nodePath().c_str(), _("Error of the ModBUS data type '%s'!"), tp.c_str());
}

void Node::load_( TConfig *icfg )
{
    bool en_prev = enableStat();

    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(), *this);
	//cfg("MODE").setI(cfg("MODE").getI());
    }

    loadIO();

    if(en_prev && !enableStat()) setEnable(true);
}

void Node::loadIO( )
{
    if(mode() == MD_DATA) {
	ResAlloc res(nRes, false);
	TFunction *f = data && data->func() ? data->func() : this;

	//Load IO
	vector<vector<string> > full;
	vector<string> u_pos;
	TConfig cfg(&owner().nodeIOEl());
	cfg.cfg("NODE_ID").setS(id(), TCfg::ForceUse);
	cfg.cfg("VALUE").setExtVal(true);
	for(int ioCnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",ioCnt++,cfg,false,&full); ) {
	    string sid = cfg.cfg("ID").getS();
	    int iid = f->ioId(sid);

	    if(!isDAQTmpl) {
		//Position storing
		int pos = cfg.cfg("POS").getI();
		while((int)u_pos.size() <= pos) u_pos.push_back("");
		u_pos[pos] = sid;

		int flgs = cfg.cfg("FLAGS").getI();
		if(flgs&IsLink) { flgs |= TPrmTempl::CfgLink; flgs &= ~IsLink; }

		if(iid < 0)
		    iid = ioIns(new IO(sid.c_str(),cfg.cfg("NAME").getS().c_str(),(IO::Type)cfg.cfg("TYPE").getI(),flgs|IO::Output,"",false), pos);
		else {
		    io(iid)->setName(cfg.cfg("NAME").getS());
		    io(iid)->setType((IO::Type)cfg.cfg("TYPE").getI());
		    io(iid)->setFlg(flgs|IO::Output);
		}
		cfg.cfg("VALUE").setNoTransl(io(iid)->type()!=IO::String);
		if(io(iid)->flg()&TPrmTempl::CfgLink) io(iid)->setRez(cfg.cfg("VALUE").getS());
		else io(iid)->setDef(cfg.cfg("VALUE").getS());
	    }
	    if(data && iid >= 0) {
		if(f->io(iid)->flg()&TPrmTempl::CfgLink) data->lnkAddrSet(iid, cfg.cfg("VALUE").getS());
		else data->setS(iid, cfg.cfg("VALUE").getS());
	    }
	}

	if(!isDAQTmpl) {
	    //Remove holes
	    for(unsigned iP = 0; iP < u_pos.size(); )
		if(u_pos[iP].empty()) u_pos.erase(u_pos.begin()+iP);
		else iP++;

	    //Position fixing
	    for(int iP = 0; iP < (int)u_pos.size(); iP++) {
		int iid = ioId(u_pos[iP]);
		if(iid != iP) try{ ioMove(iid,iP); } catch(...){ }
	    }
	}
    }
}

void Node::save_( )
{
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(), *this);

    saveIO();
}

void Node::saveIO( )
{
    if(mode() == MD_DATA) {
	ResAlloc res(nRes, false);
	TFunction *f = data && data->func() ? data->func() : this;
	//Save IO
	TConfig cfg(&owner().nodeIOEl());
	cfg.cfg("NODE_ID").setS(id(), true);
	for(int iIO = 0; iIO < f->ioSize(); iIO++) {
	    if(f->io(iIO)->flg()&TPrmTempl::LockAttr) continue;
	    cfg.cfg("ID").setS(f->io(iIO)->id());
	    cfg.cfg("NAME").setS(f->io(iIO)->name());
	    cfg.cfg("TYPE").setI(f->io(iIO)->type());
	    cfg.cfg("FLAGS").setI(f->io(iIO)->flg()&TPrmTempl::CfgLink);
	    cfg.cfg("POS").setI(iIO);
	    cfg.cfg("VALUE").setNoTransl(f->io(iIO)->type() != IO::String || (f->io(iIO)->flg()&TPrmTempl::CfgLink));
	    cfg.cfg("VALUE").setView(true);
	    if(f->io(iIO)->flg()&TPrmTempl::CfgLink)	//f->io(iIO)->rez());
	    { data ? cfg.cfg("VALUE").setS(data->lnkAddr(iIO)) : cfg.cfg("VALUE").setView(false); }
	    else if(data && data->func()) cfg.cfg("VALUE").setS(data->getS(iIO));
	    else cfg.cfg("VALUE").setS(f->io(iIO)->def());
	    SYS->db().at().dataSet(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cfg);
	}

	//Clear IO
	vector<vector<string> > full;
	cfg.cfgViewAll(false);
	for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",fldCnt++,cfg,false,&full); ) {
	    string sio = cfg.cfg("ID").getS();
	    if(f->ioId(sio) < 0) {
		if(!SYS->db().at().dataDel(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cfg,true,false,true)) break;
		if(full.empty()) fldCnt--;
	    }
	}
    }
}

void Node::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntReq = 0;

    ResAlloc res(nRes, true);

    //Enable node
    if(vl && mode() == MD_DATA) {
	// Data structure allocate
	if(!data) data = new SData(this, ("ModBusNode_"+id()).c_str());
	else data->rReg = data->wReg = data->rCoil = data->wCoil = data->rCoilI = data->rRegI = 0;

	string  lfnc = TSYS::strParse(progLang(), 0, "."), wfnc = TSYS::strParse(progLang(), 1, ".");
	isDAQTmpl = SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc);

	// Try the DAQ template
	if(isDAQTmpl) data->setFunc(&SYS->daq().at().tmplLibAt(lfnc).at().at(wfnc).at().func().at());
	// Compile the function
	else try {
	    if(progLang().empty()) data->setFunc(this);
	    else {
		string mWorkProg = SYS->daq().at().at(TSYS::strSepParse(progLang(),0,'.')).at().compileFunc(TSYS::strSepParse(progLang(),1,'.'),*this,prog());
		data->setFunc(&((AutoHD<TFunction>)SYS->nodeAt(mWorkProg)).at());
	    }
	} catch(TError &err) {
	    mess_err(nodePath().c_str(),_("Error compiling function on the language '%s': %s"),progLang().c_str(),err.mess.c_str());
	    throw;
	}
	// Links, registers and coins init
	data->addLinksAttrs();

	for(int iIO = 0; iIO < data->func()->ioSize(); iIO++) {
	    string ioId = data->func()->io(iIO)->id();
	    if(data->func()->io(iIO)->flg()&TPrmTempl::CfgLink) data->lnkAddrSet(iIO, data->func()->io(iIO)->rez());

	    string  atp, ai, mode, atpM, atpSub, sTmp;

	    int off = 0;
	    if(ioId[0] == 'C' || ioId[0] == 'R') {
		//  Unified mode
		if(TSYS::strParse(ioId,1,":").size()) {
		    atp	= TSYS::strParse(ioId, 0, ":", &off);
		    ai	= TSYS::strParse(ioId, 0, ":", &off);
		    mode= TSYS::strParse(ioId, 0, ":", &off);
		}
		//  Specific mode
		else {
		    for(off = 0; off < (int)ioId.size() && !isdigit(ioId[off]); off++) ;
		    atp = ioId.substr(0,off);
		    ai  = ioId.substr(off);
		    if(tolower(ioId[ioId.size()-1])=='w') mode = "w";
		}
	    }
	    else continue;

	    atpM	= TSYS::strParse(atp, 0, "_"),
	    atpSub	= TSYS::strParse(atp, 1, "_");

	    int reg = strtol(ai.c_str(), NULL, 0);
	    if(atpM[0] == 'R') {
		regCR(reg, SIO(iIO,atpSub[0],0), atpM);
		if(mode == "w") regCR(reg, SIO(iIO,atpSub[0],0), atpM, true);
		if(atpSub == "i" || atpSub == "i4" || atpSub == "f") {
		    int reg2 = (sTmp=TSYS::strParse(ai,1,",")).empty() ? (reg+1) : strtol(sTmp.c_str(),NULL,0);
		    regCR(reg2, SIO(iIO,atpSub[0],1), atpM);
		    if(mode == "w") regCR(reg2, SIO(iIO,atpSub[0],1), atpM, true);
		}
		else if(atpSub == "i8" || atpSub == "d") {
		    int reg2 = (sTmp=TSYS::strParse(ai,1,",")).empty() ? (reg+1) : strtol(sTmp.c_str(),NULL,0),
			reg3 = (sTmp=TSYS::strParse(ai,2,",")).empty() ? (reg2+1) : strtol(sTmp.c_str(),NULL,0),
			reg4 = (sTmp=TSYS::strParse(ai,3,",")).empty() ? (reg3+1) : strtol(sTmp.c_str(),NULL,0);
		    regCR(reg2, SIO(iIO,atpSub[0],1), atpM);
		    regCR(reg3, SIO(iIO,atpSub[0],2), atpM);
		    regCR(reg4, SIO(iIO,atpSub[0],3), atpM);
		    if(mode == "w") {
			regCR(reg2, SIO(iIO,atpSub[0],1), atpM, true);
			regCR(reg3, SIO(iIO,atpSub[0],2), atpM, true);
			regCR(reg4, SIO(iIO,atpSub[0],3), atpM, true);
		    }
		}
		else if(atpSub == "s") {
		    int N = (sTmp=TSYS::strParse(ai,1,",")).empty() ? 0 : vmin(100,strtol(sTmp.c_str(),NULL,0));
		    if(!N) N = 10;	//Default length 10 registers and maximum 100
		    for(int iR = reg+1; iR < (reg+N); iR++) {
			regCR(iR, SIO(iIO,atpSub[0],iR-reg), atpM);
			if(mode == "w") regCR(iR, SIO(iIO,atpSub[0],iR-reg), atpM, true);
		    }
		}
	    }
	    if(atpM[0] == 'C') {
		regCR(reg, iIO, atpM);
		if(mode == "w") regCR(reg, iIO, atpM, true);
	    }
	}

	data->chkLnkNeed = data->initLnks();

	//Load IO
	loadIO();

	// Start task
	SYS->taskCreate(nodePath('.',true), 0, Task, this);
    }
    //Disable node
    if(!vl) {
	// Stop the calc data task
	if(prcSt) SYS->taskDestroy(nodePath('.',true), &endRun);

	// Data structure delete
	if(data) { delete data; data = NULL; }
    }

    mEn = vl;
}

string Node::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat()) {
	rez = _("Enabled. ");
	switch(mode()) {
	    case MD_DATA:
		rez += TSYS::strMess(_("Spent time %s[%s]. Requests %.4g. Read registers %.4g, coils %.4g, register inputs %.4g, coil inputs %.4g.\n"
					"Wrote registers %.4g, coils %.4g."),
		    tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str(),
		    cntReq, data->rReg, data->rCoil, data->rRegI, data->rCoilI, data->wReg, data->wCoil);
		break;
	    case MD_GT_ND: case MD_GT_NET:
		rez += TSYS::strMess(_("Requests %.4g."), cntReq);
		break;
	}
    }

    return rez;
}

bool Node::req( const string &itr, const string &iprt, unsigned char inode, string &pdu )
{
    ResAlloc res(nRes, false);

    //Check for allow request
    if(!enableStat() || pdu.empty() ||
	!((inTransport() == "*" && mode() != MD_GT_NET) || inTransport() == itr) ||
	!(addr() == inode || mode() == MD_GT_NET) ||
	!(prt() == "*" || iprt == prt())) return false;

    cntReq++;

    //Data mode requests process
    if(mode() == MD_DATA)
	switch(pdu[0]) {
	    case TProt::FC_MULT_COILS: case TProt::FC_MULT_COILS_IN: {	//Read multiple coils
		int c_sz = 0;
		if(pdu.size() == 5) c_sz = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		if(c_sz < 1 || c_sz > 2000) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		pdu.assign(1, pdu[0]);
		pdu += (char)(c_sz/8+((c_sz%8)?1:0));
		pdu += string(pdu[1], (char)0);

		bool isData = false;
		map<int,SIO> &blk = (pdu[0]==0x01) ? data->coilR : data->coilI;
		map<int,SIO>::iterator itc;
		for(int i_c = c_addr; i_c < (c_addr+c_sz); i_c++) {
		    if((itc=blk.find(i_c)) != blk.end()) {
			if(data->getB(itc->second.id)) pdu[2+(i_c-c_addr)/8] |= (1<<((i_c-c_addr)%8));
			isData = true;
		    }
		}
		if(!isData)	{ pdu.assign(1, pdu[0]|0x80); pdu += 0x2; return true; }

		if(pdu[0] == 0x01) data->rCoil += c_sz; else data->rCoilI += c_sz;

		return true;
	    }
	    case TProt::FC_MULT_REGS: case TProt::FC_MULT_REGS_IN: {	//Read multiple registers
		int r_sz = 0;
		if(pdu.size() == 5) r_sz = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		if(r_sz < 1 || r_sz > 125) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		pdu.assign(1, pdu[0]);
		pdu += (char)(r_sz*2);

		bool isData = false;
		map<int,TVariant> grpVals;
		map<int,SIO> &blk = (pdu[0]==0x03) ? data->regR : data->regI;
		map<int,SIO>::iterator itr;
		for(int iR = r_addr; iR < (r_addr+r_sz); iR++) {
		    unsigned short val = 0;
		    if((itr=blk.find(iR)) != blk.end()) {
			switch(itr->second.sTp) {
			    case 'i': {
				union { uint16_t r[4]; int64_t i; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.i = grpValIt->second.getI();
				else { wl.i = data->getI(itr->second.id); grpVals[itr->second.id] = wl.i; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 'f': {
				union { uint16_t r[2]; float f; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.f = grpValIt->second.getR();
				else { wl.f = data->getR(itr->second.id); grpVals[itr->second.id] = wl.f; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 'd': {
				union { uint16_t r[4]; double d; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.d = grpValIt->second.getR();
				else { wl.d = data->getR(itr->second.id); grpVals[itr->second.id] = wl.d; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 's': {
				string valIO;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	valIO = grpValIt->second.getS();
				else { valIO = data->getS(itr->second.id); grpVals[itr->second.id] = valIO; }
				valIO.resize(vmax(0,(itr->second.pos+1)*2), 0);
				val = TSYS::i16_BE(TSYS::getUnalign16(valIO.data()+itr->second.pos*2));
				break;
			    }
			    default:	val = data->getI(itr->second.id);	break;
			}
			isData = true;
		    }
		    pdu += TSYS::strEncode(string((char*)&val,2), TSYS::Reverse);
		}
		if(!isData)	{ pdu.assign(1, pdu[0]|0x80); pdu += 0x2; return true; }

		if(pdu[0] == 0x03) data->rReg += r_sz; else data->rRegI += r_sz;

		return true;
	    }
	    case TProt::FC_SING_COILS_WR: {	//Write single coil
		if(pdu.size() != 5) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];

		map<int,SIO>::iterator ic = data->coilW.find(c_addr);
		if(ic == data->coilW.end()) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else {
		    data->setB(ic->second.id, (bool)pdu[3]);
		    data->lnkOutput(ic->second.id, (bool)pdu[3]);
		}

		data->wCoil++;

		return true;
	    }
	    case TProt::FC_SING_REGS_WR: {	//Write single register
		if(pdu.size() != 5) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];

		map<int,AutoHD<TVal> >::iterator il;
		map<int,SIO>::iterator ir = data->regW.find(r_addr);
		if(ir == data->regW.end()) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else {
		    unsigned short val = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		    switch(ir->second.sTp) {
			case 'i': {
			    union { uint16_t r[4]; int64_t i; } wl;
			    wl.i = data->getI(ir->second.id);
			    wl.r[ir->second.pos] = val;
			    data->setI(ir->second.id, wl.i);
			    data->lnkOutput(ir->second.id, wl.i);
			    break;
			}
			case 'f': {
			    union { uint16_t r[2]; float f; } wl;
			    wl.f = data->getR(ir->second.id);
			    wl.r[ir->second.pos] = val;
			    data->setR(ir->second.id, wl.f);
			    data->lnkOutput(ir->second.id, wl.f);
			    break;
			}
			case 'd': {
			    union { uint16_t r[4]; double d; } wl;
			    wl.d = data->getR(ir->second.id);
			    wl.r[ir->second.pos] = val;
			    data->setR(ir->second.id, wl.d);
			    data->lnkOutput(ir->second.id, wl.d);
			    break;
			}
			case 's': {
			    val = TSYS::i16_BE(val);
			    string valIO = data->getS(ir->second.id);
			    valIO.resize(vmax((int)valIO.size(),(ir->second.pos+1)*2), 0);
			    valIO.replace(ir->second.pos*2, 2, (char*)&val, 2);
			    data->setS(ir->second.id, valIO);
			    data->lnkOutput(ir->second.id, valIO);
			    break;
			}
			default:
			    data->setI(ir->second.id, val);
			    data->lnkOutput(ir->second.id, val);
			    break;
		    }
		}

		data->wReg++;

		return true;
	    }
	    case TProt::FC_MULT_COILS_WR: {	//Write multiple coils
		if(pdu.size() < 6) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_aSt = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		int c_aCnt = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		int bCnt = (unsigned char)pdu[5];
		if((int)pdu.size() != (6+bCnt) || bCnt < (c_aCnt/8)) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		bool noWrReg = false;
		for(int i_c = 0; i_c < c_aCnt; i_c++) {
		    map<int,SIO>::iterator ic = data->coilW.find(c_aSt+i_c);
		    if(ic == data->coilW.end()) noWrReg = true;
		    else {
			data->setB(ic->second.id, (bool)(1&(pdu[6+i_c/8]>>(i_c%8))));
			data->lnkOutput(ic->second.id, data->getB(ic->second.id));
			data->wCoil++;
		    }
		}
		if(noWrReg) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else pdu.resize(5);

		return true;
	    }
	    case TProt::FC_MULT_REGS_WR: {	//Write multiple register
		if(pdu.size() < 6) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_aSt = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		int r_aCnt = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		int bCnt = (unsigned char)pdu[5];
		if((int)pdu.size() != (6+bCnt) || bCnt < (r_aCnt*2) || r_aCnt > 123)
		{ pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		ResAlloc res(data->func()->fRes(), true);
		bool noWrReg = false;
		map<int,TVariant> grpVals;
		map<int,AutoHD<TVal> >::iterator il;
		for(int iR = 0; iR < r_aCnt; iR++) {
		    map<int,SIO>::iterator ir = data->regW.find(r_aSt+iR);
		    if(ir == data->regW.end()) noWrReg = true;
		    else {
			unsigned short val = (unsigned short)(pdu[6+iR*2]<<8) | (unsigned char)pdu[6+iR*2+1];
			switch(ir->second.sTp) {
			    case 'i': {
				union { uint16_t r[4]; int64_t i; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				wl.i = (grpValIt != grpVals.end()) ? grpValIt->second.getI() : data->getI(ir->second.id);
				wl.r[ir->second.pos] = val;
				grpVals[ir->second.id] = wl.i;
				break;
			    }
			    case 'f': {
				union { uint16_t r[2]; float f; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				wl.f = (grpValIt != grpVals.end()) ? grpValIt->second.getR() : data->getR(ir->second.id);
				wl.r[ir->second.pos] = val;
				grpVals[ir->second.id] = wl.f;
				break;
			    }
			    case 'd': {
				union { uint16_t r[4]; double d; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				wl.d = (grpValIt != grpVals.end()) ? grpValIt->second.getR() : data->getR(ir->second.id);
				wl.r[ir->second.pos] = val;
				grpVals[ir->second.id] = wl.d;
				break;
			    }
			    case 's': {
				val = TSYS::i16_BE(val);
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				string valIO = (grpValIt != grpVals.end()) ? grpValIt->second.getS() : data->getS(ir->second.id);
				valIO.resize(vmax((int)valIO.size(),(ir->second.pos+1)*2), 0);
				valIO.replace(ir->second.pos*2, 2, (char*)&val, 2);
				grpVals[ir->second.id] = valIO;
				break;
			    }
			    default:
				data->setI(ir->second.id, val);
				data->lnkOutput(ir->second.id, val);
				break;
			}
		    }
		    data->wReg++;
		}
		// Real write cached values
		for(map<int,TVariant>::iterator iG = grpVals.begin(); iG != grpVals.end(); ++iG) {
		    data->set(iG->first, iG->second);
		    data->lnkOutput(iG->first, iG->second);
		}

		if(noWrReg) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else pdu.resize(5);

		return true;
	    }
	    default:
		pdu.assign(1, pdu[0]|0x80);
		pdu += 0x1;
		return true;
	}
    //Gateway mode requests process
    else if(mode() == MD_GT_ND || mode() == MD_GT_NET) {
	try {
	    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(cfg("TO_TR").getS(),0,'.')).at().
							 outAt(TSYS::strSepParse(cfg("TO_TR").getS(),1,'.'));
	    if(!tr.at().startStat()) tr.at().start();

	    XMLNode req(cfg("TO_PRT").getS());
	    req.setAttr("id", id())->
		setAttr("node", (mode()==MD_GT_NET)?i2s(inode):cfg("TO_ADDR").getS())->
		setAttr("reqTry", "3")->
		setText(pdu);
	    tr.at().messProtIO(req, "ModBus");

	    if(!req.attr("err").empty()) { pdu.assign(1, pdu[0]|0x80); pdu += 0xA; }
	    pdu = req.text();
	} catch(TError &err) { pdu.assign(1, pdu[0]|0x80); pdu += 0xA; }

	return true;
    }

    return true;
}

void *Node::Task( void *ind )
{
    Node &nd = *(Node*)ind;

    nd.endRun = false;
    nd.prcSt = true;

    bool isStart = true;
    bool isStop  = false;

    int ioFrq = nd.data->ioId("f_frq");
    int ioStart = nd.data->ioId("f_start");
    int ioStop = nd.data->ioId("f_stop");

    for(unsigned int clc = 0; true; clc++) {
	if(SYS->daq().at().subStartStat()) {
	    if(nd.data->chkLnkNeed) nd.data->chkLnkNeed = nd.data->initLnks(true);

	    //Setting special IO
	    if(ioFrq >= 0)	nd.data->setR(ioFrq, (float)1/nd.period());
	    if(ioStart >= 0)	nd.data->setB(ioStart, isStart);
	    if(ioStop >= 0)	nd.data->setB(ioStop, isStop);

	    try {
		//Get input links
		nd.data->inputLinks();

		nd.data->setMdfChk(true);
		nd.data->calc();

		//Put output links
		nd.data->outputLinks();
	    } catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_err(nd.nodePath().c_str(), _("Error calculation the node function."));
	    }
	}

	if(isStop) break;
	TSYS::taskSleep((int64_t)(1e9*nd.period()));
	if(nd.endRun) isStop = true;
	isStart = false;
	nd.modif();
    }

    nd.prcSt = false;

    return NULL;
}

void Node::cntrCmdProc( XMLNode *opt )
{
    string  lfnc = TSYS::strSepParse(progLang(), 0, '.'),
	    wfnc = TSYS::strSepParse(progLang(), 1, '.');
    bool isDirFunc = lfnc.size() && wfnc.size() && !(SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc));

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Node: ")+name(),RWRWR_,"root",SPRT_ID);
	if(ctrMkNode("area",opt,-1,"/nd",_("Node"))) {
	    if(ctrMkNode("area",opt,-1,"/nd/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/nd/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/nd/st/en_st",_("Enabled"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/nd/st/db",_("DB"),RWRWR_,"root",SPRT_ID,4,
		    "tp","str", "dest","select", "select","/db/list", "help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/nd/st/timestamp",_("Date of modification"),R_R_R_,"root",SPRT_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/nd/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt, "/nd/cfg", 0, "root", SPRT_ID, RWRWR_);
		ctrMkNode("fld",opt,-1,"/nd/cfg/MODE",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
		// Append configuration properties
		XMLNode *xt = ctrId(opt->childGet(0),"/nd/cfg/InTR",true);
		if(xt) xt->setAttr("dest","sel_ed")->setAttr("select","/nd/cfg/ls_itr");
		xt = ctrId(opt->childGet(0),"/nd/cfg/TO_TR",true);
		if(xt) xt->setAttr("dest","sel_ed")->setAttr("select","/nd/cfg/ls_otr");
		ctrRemoveNode(opt, "/nd/cfg/DT_PROG");
		ctrRemoveNode(opt, "/nd/cfg/DT_PR_TR");
		ctrRemoveNode(opt, "/nd/cfg/TIMESTAMP");
		/*xt = ctrId(opt->childGet(0),"/nd/cfg/DT_PROG",true);
		if(xt) xt->parent()->childDel(xt);*/
		if(mode() == MD_DATA)
		    ctrMkNode("fld",opt,-1,"/nd/cfg/progLang",_("DAQ template or direct procedure language"),(enableStat()?R_R_R_:RWRWR_),"root",SPRT_ID,3,
			"tp","str", "dest","select", "select","/plang/list");
	    }
	}
	ResAlloc res(nRes, false);
	if(mode() == MD_DATA && ctrMkNode("area",opt,-1,"/dt",_("Data"))) {
	    if((isDirFunc || enableStat()) && ctrMkNode("table",opt,-1,"/dt/io",_("IO"),RWRWR_,"root",SPRT_ID,2,"s_com","add,del,ins,move","rows","15")) {
		ctrMkNode("list",opt,-1,"/dt/io/id",_("Identifier"),(enableStat()?R_R_R_:RWRWR_),"root",SPRT_ID,2, "tp","str",
		    "help",_("For the \"Id\" field, a specific ModBus data record form is provided:\n"
			 "  \"R{N}[w]\", \"RI{N}[w]\" - specific register (and input) form, can be expanded by the suffixes:\n"
			 "                \"i\"-Int32, \"f\"-Float, \"d\"-Double, \"s\"-String;\n"
			 "  \"R:{N}[:w]\", \"RI:{N}[:w]\" - classic register (and input) form, can be expanded by the suffixes:\n"
			 "                \"i4\"-Int32, \"i8\"-Int64, \"f\"-Float, \"d\"-Double, \"s\"-String;\n"
			 "  \"C{N}[w]\", \"CI{N}[w]\", \"C:{N}[:w]\", \"CI:{N}[:w]\" - coil (and input).\n"
			 "Where:\n"
			 "  {N} - ModBus data address of the device (dec, hex or octal) [0...65535];\n"
			 "  w   - optional character to indicate the writing capability.\n"
			 "Examples:\n"
			 "  \"R0x300w\" - register access;\n"
			 "  \"C100w\" - coil access, allowed to write;\n"
			 "  \"R_f200\" - get float from the registers 200 and 201;\n"
			 "  \"R_i400,300\" - get int32 from the registers 300 and 400;\n"
			 "  \"R_s15,20\" - get string (registers block) from the register 15 and size 20;\n"
			 "  \"R_i8:0x10:w\" - get and set int64 into the registers [0x10-0x13];\n"
			 "  \"R_d:0x20,0x30\" - get double float point (8 byte) from the registers [0x20,0x30-0x32]."));
		ctrMkNode("list",opt,-1,"/dt/io/nm",_("Name"),(enableStat()?R_R_R_:RWRWR_),"root",SPRT_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/dt/io/tp",_("Type"),(enableStat()?R_R_R_:RWRWR_),"root",SPRT_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d;%d;%d",IO::Real,IO::Integer,IO::Boolean,IO::String,IO::Object).c_str(),
		    "sel_list",_("Real;Integer;Boolean;String;Object"));
		ctrMkNode("list",opt,-1,"/dt/io/lnk",_("Link"),(enableStat()?R_R_R_:RWRWR_),"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/dt/io/vl",_("Value"),RWRWR_,"root",SPRT_ID,1,"tp","str");
	    }
	    if((enableStat() && !isDAQTmpl) || isDirFunc) {
		ctrMkNode("fld",opt,-1,"/dt/progTr",cfg("DT_PR_TR").fld().descr().c_str(),RWRWR_,"root",SPRT_ID,1, "tp","bool");
		ctrMkNode("fld",opt,-1,"/dt/prog",cfg("DT_PROG").fld().descr().c_str(),RWRWR_,"root",SPRT_ID,3, "tp","str", "rows","10", "SnthHgl","1");
	    }
	}
	if(data && data->func()) data->TPrmTempl::Impl::cntrCmdProc(opt, "/cfg");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/nd/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/nd/st/en_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/nd/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/nd/st/timestamp" && ctrChkNode(opt))	opt->setText(i2s(timeStamp()));
    else if(a_path == "/nd/cfg/ls_itr" && ctrChkNode(opt)) {
	if(mode() != MD_GT_NET) opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path == "/nd/cfg/ls_otr" && ctrChkNode(opt)) {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path == "/nd/cfg/progLang") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(progLang());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setProgLang(opt->text());
    }
    else if(a_path.substr(0,7) == "/nd/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SPRT_ID,RWRWR_);
    else if(a_path == "/plang/list") {
	vector<string> lls, ls;
	//Templates
	SYS->daq().at().tmplLibList(lls);
	for(unsigned iL = 0; iL < lls.size(); iL++) {
	    SYS->daq().at().tmplLibAt(lls[iL]).at().list(ls);
	    for(unsigned iT = 0; iT < ls.size(); iT++)
		opt->childAdd("el")->setText(lls[iL]+"."+ls[iT]);
	}
	TCntrNode::cntrCmdProc(opt);
    }
    else if(a_path.find("/dt") == 0 || a_path.find("/cfg") == 0) {
	ResAlloc res(nRes, false);
	if(a_path == "/dt/io") {
	    TFunction *f = data && data->func() ? data->func() : this;
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD)) {
		XMLNode *nId   = ctrMkNode("list",opt,-1,"/dt/io/id","");
		XMLNode *nNm   = ctrMkNode("list",opt,-1,"/dt/io/nm","");
		XMLNode *nType = ctrMkNode("list",opt,-1,"/dt/io/tp","");
		XMLNode *nLnk  = ctrMkNode("list",opt,-1,"/dt/io/lnk","");
		XMLNode *nVal  = ctrMkNode("list",opt,-1,"/dt/io/vl","");

		for(int id = 0; id < f->ioSize(); id++) {
		    if(nId)	nId->childAdd("el")->setText(f->io(id)->id());
		    if(nNm)	nNm->childAdd("el")->setText(f->io(id)->name());
		    if(nType)	nType->childAdd("el")->setText(i2s(f->io(id)->type()));
		    if(nLnk)	nLnk->childAdd("el")->setText((f->io(id)->flg()&TPrmTempl::CfgLink)?"1":"0");
		    if(nVal)	nVal->childAdd("el")->setText((data && data->func()) ? data->getS(id) : f->io(id)->def());
		}
	    }
	    if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR)) {
		if(enableStat()) throw TError(nodePath().c_str(),_("Turn off the node for this operation"));
		IO *ioPrev = f->ioSize() ? f->io(f->ioSize()-1) : NULL;
		if(ioPrev) f->ioAdd(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~TPrmTempl::LockAttr)));
		else f->ioAdd(new IO("new",_("New IO"),IO::Integer,IO::Output));
		modif();
	    }
	    if(ctrChkNode(opt,"ins",RWRWR_,"root",SPRT_ID,SEC_WR)) {
		if(enableStat()) throw TError(nodePath().c_str(),_("Turn off the node for this operation"));
		int row = s2i(opt->attr("row"));
		IO *ioPrev = row ? f->io(row-1) : NULL;
		if(ioPrev) f->ioIns(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~TPrmTempl::LockAttr)), row);
		else f->ioIns(new IO("new",_("New IO"),IO::Integer,IO::Output), row);
		modif();
	    }
	    if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR)) {
		if(enableStat()) throw TError(nodePath().c_str(),_("Turn off the node for this operation"));
		int row = s2i(opt->attr("row"));
		if(f->io(row)->flg()&TPrmTempl::LockAttr)
		    throw TError(nodePath().c_str(),_("Deleting a locked attribute is not allowed."));
		f->ioDel(row);
		modif();
	    }
	    if(ctrChkNode(opt,"move",RWRWR_,"root",SPRT_ID,SEC_WR)) {
		if(enableStat()) throw TError(nodePath().c_str(),_("Turn off the node for this operation"));
		f->ioMove(s2i(opt->attr("row")), s2i(opt->attr("to"))); modif();
	    }
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR)) {
		int row = s2i(opt->attr("row"));
		string col = opt->attr("col");
		if(enableStat() && col != "vl") throw TError(nodePath().c_str(),_("Turn off the node for this operation"));
		if(f->io(row)->flg()&TPrmTempl::LockAttr)	throw TError(nodePath().c_str(),_("Changing a locked attribute is not allowed."));
		if((col == "id" || col == "nm") && !opt->text().size())	throw TError(nodePath().c_str(),_("Empty value is not allowed."));
		if(col == "id")		f->io(row)->setId(opt->text());
		else if(col == "nm")	f->io(row)->setName(opt->text());
		else if(col == "tp")	f->io(row)->setType((IO::Type)s2i(opt->text()));
		else if(col == "lnk") {
		    f->io(row)->setFlg(s2i(opt->text()) ? (f->io(row)->flg()|TPrmTempl::CfgLink) : (f->io(row)->flg() & ~TPrmTempl::CfgLink));
		    f->io(row)->setDef("");
		}
		else if(col == "vl")	{
		    if(data && data->func()) {
			data->setS(row, opt->text());
			data->lnkOutput(row, opt->text());
		    }
		    else f->io(row)->setDef(opt->text());
		}
		modif();
	    }
	}
	else if(a_path == "/dt/progTr") {
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(progTr()));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setProgTr(s2i(opt->text()));
	}
	else if(a_path == "/dt/prog") {
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(prog());
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setProg(opt->text());
	    if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD))
		try {
		    SYS->daq().at().at(TSYS::strParse(progLang(),0,".")).at().
				    compileFuncSynthHighl(TSYS::strParse(progLang(),1,"."),*opt);
		} catch(...) { }
	}
	else if(data && data->func() && data->TPrmTempl::Impl::cntrCmdProc(opt,"/cfg"))	;
    }
    else TCntrNode::cntrCmdProc(opt);
}

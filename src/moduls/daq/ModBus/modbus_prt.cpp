
//OpenSCADA system module Protocol.ModBus file: modbus_prt.cpp
/***************************************************************************
 *   Copyright (C) 2008-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

    mType	= PRT_TYPE;
    mName	= PRT_NAME;
    mVers	= PRT_MVER;
    mAuthor	= PRT_AUTHORS;
    mDescr	= PRT_DESCR;
    mLicense	= PRT_LICENSE;
    mSource	= name;

    mNode = grpAdd("n_");

    //Node DB structure
    mNodeEl.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mNodeEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    mNodeEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300"));
    mNodeEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mNodeEl.fldAdd(new TFld("ADDR",_("Address"),TFld::Integer,0,"3","1","1;247"));
    mNodeEl.fldAdd(new TFld("InTR",_("Input transport"),TFld::String,0,OBJ_ID_SZ,"*"));
    mNodeEl.fldAdd(new TFld("PRT",_("Protocol"),TFld::String,TFld::Selected,"5","*","RTU;ASCII;TCP;*",_("RTU;ASCII;TCP/IP;All")));
    mNodeEl.fldAdd(new TFld("MODE",_("Mode"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d",Node::MD_DATA,Node::MD_GT_ND,Node::MD_GT_NET).c_str(),_("Data;Gateway node;Gateway net")));
    // For "Data" mode
    mNodeEl.fldAdd(new TFld("DT_PER",_("Calculate data period (s)"),TFld::Real,0,"5.3","1","0.001;99"));
    mNodeEl.fldAdd(new TFld("DT_PROG",_("Program"),TFld::String,TCfg::TransltText,"1000000"));
    // For "Gateway" mode
    mNodeEl.fldAdd(new TFld("TO_TR",_("To transport"),TFld::String,0,OBJ_ID_SZ));
    mNodeEl.fldAdd(new TFld("TO_PRT",_("To protocol"),TFld::String,TFld::Selected,"5","RTU","RTU;ASCII;TCP",_("RTU;ASCII;TCP/IP")));
    mNodeEl.fldAdd(new TFld("TO_ADDR",_("To address"),TFld::Integer,0,"3","1","1;247"));

    //Node data IO DB structure
    mNodeIOEl.fldAdd(new TFld("NODE_ID",_("Node ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mNodeIOEl.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mNodeIOEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    mNodeIOEl.fldAdd(new TFld("TYPE",_("Value type"),TFld::Integer,TFld::NoFlag,"1"));
    mNodeIOEl.fldAdd(new TFld("FLAGS",_("Flags"),TFld::Integer,TFld::NoFlag,"4"));
    mNodeIOEl.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TCfg::TransltText,"100"));
    mNodeIOEl.fldAdd(new TFld("POS",_("Real position"),TFld::Integer,TFld::NoFlag,"4"));
}

TProt::~TProt( )
{
    nodeDelAll();
}

void TProt::nAdd( const string &iid, const string &db )	{ chldAdd(mNode, new Node(iid,db,&nodeEl())); }

void TProt::load_( )
{
    //Load parameters from command line

    //Load DB
    // Search and create new nodes
    try
    {
	TConfig g_cfg(&nodeEl());
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;
	map<string, bool> itReg;

	//  Search into DB
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+modId()+"_node",nodePath()+modId()+"_node",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if(!nPresent(id)) nAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty())
	{
	    nList(db_ls);
	    for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(nAt(db_ls[i_it]).at().DB()))
		    nDel(db_ls[i_it]);
	}
    }
    catch(TError err)
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new node error."));
    }
}

void TProt::save_( )
{

}

void TProt::modStart( )
{
    vector<string> ls;
    nList(ls);
    for(unsigned i_n = 0; i_n < ls.size(); i_n++)
	if(nAt(ls[i_n]).at().toEnable())
	    nAt(ls[i_n]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    nList(ls);
    for(unsigned i_n = 0; i_n < ls.size(); i_n++)
	nAt(ls[i_n]).at().setEnable(false);
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

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
    for(unsigned i_b = 0; i_b < mbap.size(); i_b++)
    {
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

    for(unsigned i = 0; i < in.size(); i++)
    {
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

    for(unsigned i = 0; i < (in.size()&(~0x01)); i += 2)
    {
	ch2 = 0;
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

    ResAlloc resN(tro.nodeRes(), true);

    string prt   = io.name();
    string sid   = io.attr("id");
    string debugCat = io.attr("debugCat");
    int    reqTm = atoi(io.attr("reqTm").c_str());
    int    node  = atoi(io.attr("node").c_str());
    int    reqTry = vmin(10,vmax(1,atoi(io.attr("reqTry").c_str())));
    string pdu   = io.text();

    try
    {
	if(!tro.startStat()) tro.start();
	if(prt == "TCP")		// Modbus/TCP protocol process
	{
	    //Encode MBAP (Modbus Application Protocol)
	    int tid = rand();
	    mbap.reserve(pdu.size()+7);
	    mbap.append((char *)&tid,2);	//Transaction ID
	    mbap.append(2,(char)0);		//Protocol ID
	    mbap += (char)((pdu.size()+1)>>8);	//PDU size MSB
	    mbap += (char)(pdu.size()+1);	//PDU size LSB
	    mbap += (char)node;			//Unit identifier
	    mbap += pdu;

	    //Send request
	    int resp_len = tro.messIO(mbap.data(), mbap.size(), buf, sizeof(buf), reqTm, true);
	    rez.assign(buf,resp_len);
	    if(rez.size() < 7)	err = _("13:Error server respond");
	    else
	    {
		unsigned resp_sz = (unsigned short)(rez[4]<<8) | (unsigned char)rez[5];

		//Wait tail
		while(rez.size() < (resp_sz+6))
		{
		    resp_len = tro.messIO(NULL, 0, buf, sizeof(buf), reqTm, true);
		    if(!resp_len) throw TError(nodePath().c_str(),_("Not full respond"));
		    rez.append(buf, resp_len);
		}
		pdu = rez.substr(7);
	    }
	}
	else if(prt == "RTU")		// Modbus/RTU protocol process
	{
	    mbap.reserve(pdu.size()+3);
	    mbap += (uint8_t)node;		//Unit identifier
	    mbap += pdu;
	    uint16_t crc = CRC16( mbap );
	    mbap += (crc>>8);
	    mbap += crc;

	    //Send request
	    for(int i_tr = 0; i_tr < reqTry; i_tr++)
	    {
		int resp_len = tro.messIO(mbap.data(), mbap.size(), buf, sizeof(buf), reqTm, true);
		rez.assign(buf, resp_len);
		//Wait tail
		while(resp_len)
		{
		    try{ resp_len = tro.messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
		    rez.append(buf, resp_len);
		}

		if(rez.size() < 2) { err = _("13:Error respond: Too short."); continue; }
		if(CRC16(rez.substr(0,rez.size()-2)) != (uint16_t)((rez[rez.size()-2]<<8)+(uint8_t)rez[rez.size()-1]))
		{ err = _("13:Error respond: CRC check error."); continue; }
		pdu = rez.substr(1, rez.size()-3);
		err = "";
		break;
	    }
	}
	else if(prt == "ASCII")		// Modbus/ASCII protocol process
	{
	    mbap.reserve(pdu.size()+2);
	    mbap += (uint8_t)node;		//Unit identifier
	    mbap += pdu;
	    mbap += LRC(mbap);
	    mbap = ":"+DataToASCII(mbap)+"\x0D\x0A";

	    //Send request
	    for(int i_tr = 0; i_tr < reqTry; i_tr++)
	    {
		int resp_len = tro.messIO(mbap.data(), mbap.size(), buf, sizeof(buf), reqTm, true);
		rez.assign(buf, resp_len);
		//Wait tail
		while(resp_len && (rez.size() < 3 || rez.substr(rez.size()-2,2) != "\x0D\x0A"))
		{
		    try{ resp_len = tro.messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
		    rez.append(buf, resp_len);
		}

		if(rez.size() < 3 || rez[0] != ':' || rez.substr(rez.size()-2,2) != "\x0D\x0A")
		{ err = _("13:Error respond: Error format."); continue; }
		string rezEnc = ASCIIToData(rez.substr(1,rez.size()-3));
		if(LRC(rezEnc.substr(0,rezEnc.size()-1)) != (uint8_t)rezEnc[rezEnc.size()-1])
		{ err = _("13:Error respond: LRC check error."); continue; }
		pdu = rezEnc.substr(1,rezEnc.size()-2);
		err = "";
		break;
	    }
	}
	else err = TSYS::strMess(_("Protocol '%s' error."),prt.c_str());

	//Check respond pdu
	if(err.empty())
	{
	    if(pdu.size() < 2) err = _("13:Error respond");
	    if(pdu[0]&0x80)
		switch(pdu[1])
		{
		    case 0x1: err = TSYS::strMess(_("1:%02X:Function is not supported."),(unsigned char)(pdu[0]&(~0x80)));	break;
		    case 0x2: err = _("2:Requested address not allow or request area too long.");	break;
		    case 0x3: err = _("3:Illegal data value into request.");		break;
		    case 0x4: err = _("4:Server failure.");				break;
		    case 0x5: err = _("5:Request requires too long time for execute.");	break;
		    case 0x6: err = _("6:Server is busy.");				break;
		    case 0x7: err = _("7:Program function is error. By request functions 13 or 14.");	break;
		    case 0xA: case 0xB: err = _("10:Gateway problem.");			break;
		    default: err = TSYS::strMess(_("12:%02X:Unknown error."),(unsigned char)(pdu[1]));	break;
		}
	}
    }catch(TError er) { err = _("14:Device error: ") + er.mess; }

    io.setText(err.empty()?pdu:"");
    if(!err.empty()) io.setAttr("err",err);

    //Prepare log
    if(prtLen() || debugCat.size())
    {
	string mess = _("REQ -> ");
	if(prt != "ASCII") mess += TSYS::strDecode(mbap,TSYS::Bin);
	else if(mbap.size() > 2) mess += mbap.substr(0,mbap.size()-2);
	mess += "\n";

	if(err.empty()) mess += _("RESP -> ");
	else mess += _("ERR -> ") + err + " -> ";
	if(prt != "ASCII") mess += TSYS::strDecode(rez,TSYS::Bin);
	else if(rez.size() > 2) mess += rez.substr(0,rez.size()-2);

	if(prtLen())
	    pushPrtMess(tm2s(time(NULL),"")+" "+prt+": '"+sid+"' --> "+i2s(node)+"("+tro.workId()+")\n"+mess+"\n");
	if(debugCat.size()) mess_debug_(debugCat.c_str(), mess.c_str());
    }
}

void TProt::setPrtLen( int vl )
{
    ResAlloc res(nodeRes(), true);

    while((int)mPrt.size() > vl) mPrt.pop_back();

    mPrtLen = vl;
}

void TProt::pushPrtMess( const string &vl )
{
    ResAlloc res(nodeRes(), true);

    if(!prtLen()) return;

    mPrt.push_front(vl);

    while((int)mPrt.size() > prtLen())	mPrt.pop_back();
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/n_",_("Node"),RWRWR_,"root",SPRT_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/node",_("Nodes")))
	    ctrMkNode("list",opt,-1,"/node/node",_("Nodes"),RWRWR_,"root",SPRT_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","n_","idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,1,"/rep",_("Report")))
	{
	    ctrMkNode("fld",opt,-1,"/rep/repLen",_("Report length"),RWRWR_,"root",SPRT_ID,4,"tp","dec","min","0","max","10000",
		"help",_("Zero use for report disabling"));
	    if(prtLen())
		ctrMkNode("fld",opt,-1,"/rep/rep",_("Report"),R_R_R_,"root",SPRT_ID,3,"tp","str","cols","90","rows","20");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/n_" || a_path == "/node/node")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    vector<string> lst;
	    nList(lst);
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(nAt(lst[i_f]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    nAdd(vid); nAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))	chldDel(mNode,opt->attr("id"),-1,1);
    }
    else if(a_path == "/rep/repLen")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(prtLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setPrtLen(atoi(opt->text().c_str()));
    }
    else if(a_path == "/rep/rep" && ctrChkNode(opt))
    {
	ResAlloc res(nodeRes(),true);
	for(unsigned i_p = 0; i_p < mPrt.size(); i_p++)
	    opt->setText(opt->text() + mPrt[i_p] + "\n");
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

TProt &TProtIn::owner( )	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &ireqst, string &answer )
{
    //Check for protocol type
    unsigned char node = 0;
    string prt, pdu;
    string reqst = ireqst;
    bool isBuf = false;

retry:
    // ASCII check
    if(reqst.size() > 3 && reqst[0] == ':' && reqst.substr(reqst.size()-2,2) == "\x0D\x0A")
    {
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
    else
    {
	if(!isBuf && req_buf.size())
	{
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
    unsigned i_n;
    for(i_n = 0; i_n < nls.size(); i_n++)
	if(modPrt->nAt(nls[i_n]).at().req(srcTr().at().workId(),prt,node,pdu)) break;
    if(i_n >= nls.size()) return false;

    answer = "";
    if(prt == "TCP")
    {
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
    else if(prt == "RTU")
    {
	answer.reserve(pdu.size()+3);
	answer += (uint8_t)node;		//Unit identifier
	answer += pdu;
	uint16_t crc = modPrt->CRC16( answer );
	answer += crc>>8;
	answer += crc;
    }
    else if(prt == "ASCII")
    {
	answer.reserve(pdu.size()+2);
	answer += (uint8_t)node;		//Unit identifier
	answer += pdu;
	answer += modPrt->LRC(answer);
	answer = ":"+modPrt->DataToASCII(answer)+"\x0D\x0A";
    }

    if(owner().prtLen( ) && prt.size() && answer.size())
    {
	string mess = tm2s(time(NULL),"")+" "+prt+": "+srcTr().at().workId()+
			"("+TSYS::strLine(srcAddr(),0)+") --> "+i2s(node)+"\n";
	mess += _("REQ -> ");
	if(prt != "ASCII")	mess += TSYS::strDecode(reqst, TSYS::Bin);
	else if(reqst.size() > 2) mess += reqst.substr(0, reqst.size()-2);
	mess += "\n";

	mess +=_("RESP -> ");
	if(prt != "ASCII")	mess += TSYS::strDecode(answer, TSYS::Bin);
	else if(answer.size() > 2) mess += answer.substr(0, answer.size()-2);

	owner().pushPrtMess(mess+"\n");
    }

    return false;
}

//*************************************************
//* Node: ModBus input protocol node.             *
//*************************************************
Node::Node( const string &iid, const string &idb, TElem *el ) :
    TFunction("ModBusNode_"+iid), TConfig(el), data(NULL),
    mId(cfg("ID")), mName(cfg("NAME")), mDscr(cfg("DESCR")), mPer(cfg("DT_PER").getRd()), mAEn(cfg("EN").getBd()),
    mEn(false), mDB(idb), prcSt(false), endrunRun(false), cntReq(0)
{
    mId = iid;

    cfg("MODE").setI(0);
}

Node::~Node( )
{
    try{ setEnable(false); } catch(...) { }
    if(data) { delete data; data = NULL; }
}

TCntrNode &Node::operator=( TCntrNode &node )
{
    Node *src_n = dynamic_cast<Node*>(&node);
    if(!src_n) return *this;

    if(enableStat())	setEnable(false);

    //Copy parameters
    exclCopy(*src_n, "ID;");
    *(TFunction*)this = *(TFunction*)src_n;
    setDB(src_n->DB());

    return *this;
}


void Node::postEnable( int flag )
{
    //Create default IOs
    if(flag&TCntrNode::NodeConnect)
    {
	ioIns(new IO("f_frq",_("Function calculate frequency (Hz)"),IO::Real,Node::LockAttr,"1000",false), 0);
	ioIns(new IO("f_start",_("Function start flag"),IO::Boolean,Node::LockAttr,"0",false), 1);
	ioIns(new IO("f_stop",_("Function stop flag"),IO::Boolean,Node::LockAttr,"0",false), 2);
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

TProt &Node::owner( )		{ return *(TProt*)nodePrev(); }

string Node::name( )
{
    string tNm = mName;
    return tNm.size() ? tNm : id();
}

string Node::tbl( )		{ return owner().modId()+"_node"; }

int Node::addr( )		{ return cfg("ADDR").getI(); }

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

bool Node::cfgChange( TCfg &ce )
{
    if(ce.name() == "MODE")
    {
	setEnable(false);
	//Hide all specific
	cfg("ADDR").setView(false); cfg("DT_PER").setView(false); cfg("DT_PROG").setView(false);
	cfg("TO_TR").setView(false); cfg("TO_PRT").setView(false); cfg("TO_ADDR").setView(false);

	//Show selected
	switch( ce.getI() )
	{
	    case 0:	cfg("ADDR").setView(true); cfg("DT_PER").setView(true); cfg("DT_PROG").setView(true);	break;
	    case 1:	cfg("ADDR").setView(true); cfg("TO_TR").setView(true); cfg("TO_PRT").setView(true); cfg("TO_ADDR").setView(true);	break;
	    case 2:	cfg("TO_TR").setView(true); cfg("TO_PRT").setView(true);	break;
	}
    }

    modif();
    return true;
}

void Node::regCR( int id, const SIO &val, const string &tp, bool wr )
{
    map<int,SIO>::iterator it;
	if (tp == "C")
	{
	    map<int,SIO> &blk = wr ? data->coilW : data->coilR;
	    if((it=blk.find(id)) == blk.end()) blk[id] = val;
	    else mess_warning(nodePath().c_str(), _("Coil '%d' already registered for IO '%d'. IO '%d' will be disabled for process coil '%d'!"),
						    id, it->second.id, val.id, id);
	} else
	if (tp == "CI")
	{
	    map<int,SIO> &blk = data->bitR;
	    if((it=blk.find(id)) == blk.end()) blk[id] = val;
	    else mess_warning(nodePath().c_str(), _("Discrete input '%d' already registered for IO '%d'. IO '%d' will be disabled for process discrete input '%d'!"),
						    id, it->second.id, val.id, id);
	} else
	if (tp == "R")
	{
	    map<int,SIO> &blk = wr ? data->regW : data->regR;
	    if((it=blk.find(id)) == blk.end())	blk[id] = val;
	    else mess_warning(nodePath().c_str(), _("Register '%d' already registered for IO '%d'. IO '%d' will be disabled for process register '%d'!"),
						    id, it->second.id, val.id, id);
	} else
	if (tp == "RI")
	{
	    map<int,SIO> &blk = data->regIR;
	    if((it=blk.find(id)) == blk.end())	blk[id] = val;
	    else mess_warning(nodePath().c_str(), _("Input register '%d' already registered for IO '%d'. IO '%d' will be disabled for process input register '%d'!"),
						    id, it->second.id, val.id, id);
	} else
		throw TError(nodePath().c_str(), _("ModBUS data type '%s' error!"), tp.c_str());

}

void Node::load_( )
{
    bool en_prev = enableStat();

    if(!SYS->chkSelDB(DB())) throw TError();
    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(), *this);
    cfg("MODE").setI(cfg("MODE").getI());

    //Load IO
    vector<string> u_pos;
    TConfig cfg(&owner().nodeIOEl());
    cfg.cfg("NODE_ID").setS(id(),true);
    for(int io_cnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",io_cnt++,cfg); )
    {
	string sid = cfg.cfg("ID").getS();

	//Position storing
	int pos = cfg.cfg("POS").getI();
	while((int)u_pos.size() <= pos) u_pos.push_back("");
	u_pos[pos] = sid;

	int iid = ioId(sid);

	if(iid < 0)
	    iid = ioIns(new IO(sid.c_str(),cfg.cfg("NAME").getS().c_str(),(IO::Type)cfg.cfg("TYPE").getI(),cfg.cfg("FLAGS").getI(),"",false), pos);
	else
	{
	    io(iid)->setName(cfg.cfg("NAME").getS());
	    io(iid)->setType((IO::Type)cfg.cfg("TYPE").getI());
	    io(iid)->setFlg(cfg.cfg("FLAGS").getI());
	}
	if(io(iid)->flg()&Node::IsLink) io(iid)->setRez(cfg.cfg("VALUE").getS());
	else io(iid)->setDef(cfg.cfg("VALUE").getS());
    }

    //Remove holes
    for(unsigned i_p = 0; i_p < u_pos.size(); )
	if(u_pos[i_p].empty()) u_pos.erase(u_pos.begin()+i_p);
	else i_p++;

    //Position fixing
    for(int i_p = 0; i_p < (int)u_pos.size(); i_p++)
    {
	int iid = ioId(u_pos[i_p]);
	if(iid != i_p) try{ ioMove(iid,i_p); } catch(...){ }
    }

    if(en_prev && !enableStat()) setEnable(true);
}

void Node::save_( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);

    //Save IO
    TConfig cfg(&owner().nodeIOEl());
    cfg.cfg("NODE_ID").setS(id(),true);
    for(int i_io = 0; i_io < ioSize(); i_io++) {
	if(io(i_io)->flg()&Node::LockAttr) continue;
	cfg.cfg("ID").setS(io(i_io)->id());
	cfg.cfg("NAME").setS(io(i_io)->name());
	cfg.cfg("TYPE").setI(io(i_io)->type());
	cfg.cfg("FLAGS").setI(io(i_io)->flg());
	cfg.cfg("POS").setI(i_io);
	if(io(i_io)->flg()&Node::IsLink) cfg.cfg("VALUE").setS(io(i_io)->rez());
	else if(data && data->val.func()) cfg.cfg("VALUE").setS(data->val.getS(i_io));
	else cfg.cfg("VALUE").setS(io(i_io)->def());
	SYS->db().at().dataSet(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cfg);
    }

    //Clear IO
    cfg.cfgViewAll(false);
    for(int fld_cnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",fld_cnt++,cfg); )
    {
	string sio = cfg.cfg("ID").getS();
	if(ioId(sio) < 0 || io(ioId(sio))->flg()&Node::LockAttr)
	{
	    SYS->db().at().dataDel(fullDB()+"_io", owner().nodePath()+tbl()+"_io", cfg, true, false, true);
	    fld_cnt--;
	}
    }
}

void Node::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntReq = 0;

    ResAlloc res(nRes, true);

    //Enable node
    if(vl && mode() == MD_DATA)
    {
	// Data structure allocate
	if(!data) data = new SData;

	// Compile function
	try
	{
	    if(progLang().empty()) data->val.setFunc(this);
	    else
	    {
		string mWorkProg = SYS->daq().at().at(TSYS::strSepParse(progLang(),0,'.')).at().compileFunc(TSYS::strSepParse(progLang(),1,'.'),*this,prog());
		data->val.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(mWorkProg)).at());
	    }
	}
	catch(TError err)
	{
	    mess_err(nodePath().c_str(),_("Compile function by language '%s' error: %s"),progLang().c_str(),err.mess.c_str());
	    throw;
	}
	// Links, registers and coins init
	for(int i_io = 0; i_io < ioSize(); i_io++)
	{
	    IO *io_ = io(i_io);
	    string ioId = io_->id();
	    if(io_->flg()&Node::IsLink)	data->lnk[i_io] = SYS->daq().at().attrAt(io_->rez(), '.', true);
	    //  Unified mode
	    if(TSYS::strParse(ioId,1,":").size() && (ioId[0] == 'C' || ioId[0] == 'R'))
	    {
		int off = 0;
		string  atp = TSYS::strParse(ioId, 0, ":", &off),
				atp_m = TSYS::strSepParse(atp,0,'_'),
			ai  = TSYS::strParse(ioId, 0, ":", &off),
			mode= TSYS::strParse(ioId, 0, ":", &off),
			atp_sub = TSYS::strParse(atp, 1, "_"),
			sTmp;
		int reg = strtol(ai.c_str(), NULL, 0);
		if(atp_m == "R")
		{
		    regCR(reg, SIO(i_io,atp_sub[0],0), "R");
		    if(mode == "w") regCR(reg, SIO(i_io,atp_sub[0],0), "R", true);
		    if(atp_sub == "i4" || atp_sub == "f")
		    {
			int reg2 = (sTmp=TSYS::strParse(ai,1,",")).empty() ? (reg+1) : strtol(sTmp.c_str(),NULL,0);
			regCR(reg2, SIO(i_io,atp_sub[0],1), "R");
			if(mode == "w") regCR(reg2, SIO(i_io,atp_sub[0],1), "R", true);
		    }
		    else if(atp_sub == "i8" || atp_sub == "d")
		    {
			int reg2 = (sTmp=TSYS::strParse(ai,1,",")).empty() ? (reg+1) : strtol(sTmp.c_str(),NULL,0),
			    reg3 = (sTmp=TSYS::strParse(ai,2,",")).empty() ? (reg2+1) : strtol(sTmp.c_str(),NULL,0),
			    reg4 = (sTmp=TSYS::strParse(ai,3,",")).empty() ? (reg3+1) : strtol(sTmp.c_str(),NULL,0);
			regCR(reg2, SIO(i_io,atp_sub[0],1), "R");
			regCR(reg3, SIO(i_io,atp_sub[0],2), "R");
			regCR(reg4, SIO(i_io,atp_sub[0],3), "R");
			if(mode == "w")
			{
			    regCR(reg2, SIO(i_io,atp_sub[0],1), "R", true);
			    regCR(reg3, SIO(i_io,atp_sub[0],2), "R", true);
			    regCR(reg4, SIO(i_io,atp_sub[0],3), "R", true);
			}
		    }
		    else if(atp_sub == "s")
		    {
			int N = (sTmp=TSYS::strParse(ai,1,",")).empty() ? 0 : vmin(100,strtol(sTmp.c_str(),NULL,0));
			if(!N) N = 10;	//Default length 10 registers and maximum 100
			for(int i_r = N; i_r < (reg+N); i_r++)
			{
			    regCR(i_r, SIO(i_io,atp_sub[0],i_r-reg), "R");
			    if(mode == "w") regCR(i_r, SIO(i_io,atp_sub[0],i_r-reg), "R", true);
			}
		    }
		}
		if(atp_m == "RI")
		{
		    regCR(reg, SIO(i_io,atp_sub[0],0), "RI");
		    if(atp_sub == "i4" || atp_sub == "f")
		    {
			int reg2 = (sTmp=TSYS::strParse(ai,1,",")).empty() ? (reg+1) : strtol(sTmp.c_str(),NULL,0);
			regCR(reg2, SIO(i_io,atp_sub[0],1), "RI");
		    }
		    else if(atp_sub == "i8" || atp_sub == "d")
		    {
			int reg2 = (sTmp=TSYS::strParse(ai,1,",")).empty() ? (reg+1) : strtol(sTmp.c_str(),NULL,0),
			    reg3 = (sTmp=TSYS::strParse(ai,2,",")).empty() ? (reg2+1) : strtol(sTmp.c_str(),NULL,0),
			    reg4 = (sTmp=TSYS::strParse(ai,3,",")).empty() ? (reg3+1) : strtol(sTmp.c_str(),NULL,0);
			regCR(reg2, SIO(i_io,atp_sub[0],1), "RI");
			regCR(reg3, SIO(i_io,atp_sub[0],2), "RI");
			regCR(reg4, SIO(i_io,atp_sub[0],3), "RI");
		    }
		    else if(atp_sub == "s")
		    {
			int N = (sTmp=TSYS::strParse(ai,1,",")).empty() ? 0 : vmin(100,strtol(sTmp.c_str(),NULL,0));
			if(!N) N = 10;	//Default length 10 registers and maximum 100
			for(int i_r = N; i_r < (reg+N); i_r++)
			{
			    regCR(i_r, SIO(i_io,atp_sub[0],i_r-reg), "RI");
			}
		    }
		}
		if(atp_m == "C")
		{
		    regCR(reg, i_io, "C");
		    if(mode == "w") regCR(reg, i_io, "C", true);
		}
		if(atp_m == "CI")
		{
		    regCR(reg, i_io, "CI");
		}
	    }
	    //  Specific mode
	    else if(tolower(ioId[0]) == 'c' || tolower(ioId[0]) == 'r')
	    {
		bool wr = (tolower(ioId[ioId.size()-1])=='w');
		if(ioId.compare(0,3,"R_i") == 0 || ioId.compare(0,3,"R_f") == 0)
		{
		    int tca1 = strtol(ioId.c_str()+3, NULL, 0);
		    size_t secReg = ioId.find(",",3);
		    int tca2 = (secReg != string::npos) ? strtol(ioId.c_str()+secReg+1, NULL, 0) : tca1+1;
		    char sTp = (ioId.compare(0,3,"R_i") == 0) ? 'i' : 'f';
		    regCR(tca1, SIO(i_io,sTp,0), "R"); regCR(tca2, SIO(i_io,sTp,1), "R");
		    if(wr) { regCR(tca1, SIO(i_io,sTp,0), "R", true); regCR(tca2, SIO(i_io,sTp,1), "R", true); }
		}
		if(ioId.compare(0,3,"R_d") == 0)
		{
		    int tca1 = strtol(ioId.c_str()+3, NULL, 0);
		    size_t secReg = ioId.find(",",3);
		    int tca2 = (secReg != string::npos) ? strtol(ioId.c_str()+secReg+1, NULL, 0) : tca1+1;
		    if(secReg != string::npos) secReg = ioId.find(",",secReg+1);
		    int tca3 = (secReg != string::npos) ? strtol(ioId.c_str()+secReg+1, NULL, 0) : tca2+1;
		    if(secReg != string::npos) secReg = ioId.find(",",secReg+1);
		    int tca4 = (secReg != string::npos) ? strtol(ioId.c_str()+secReg+1, NULL, 0) : tca3+1;
		    regCR(tca1, SIO(i_io,'d',0), "R"); regCR(tca2, SIO(i_io,'d',1), "R");
		    regCR(tca3, SIO(i_io,'d',2), "R"); regCR(tca4, SIO(i_io,'d',3), "R");
		    if(wr)
		    {
			regCR(tca1, SIO(i_io,'d',0), "R", true); regCR(tca2, SIO(i_io,'d',1), "R", true);
			regCR(tca3, SIO(i_io,'d',2), "R", true); regCR(tca4, SIO(i_io,'d',3), "R", true);
		    }
		}
		else if(ioId.compare(0,3,"R_s") == 0)
		{
		    int tca = strtol(ioId.c_str()+3, NULL, 0);
		    size_t strSzReg = ioId.find(",",3);
		    int tcaN = (strSzReg != string::npos) ? vmin(100,strtol(ioId.c_str()+strSzReg+1,NULL,0)) : 0;
		    if(!tcaN) tcaN = 10;	//Default length 10 registers and maximum 100
		    for(int i_r = tca; i_r < (tca+tcaN); i_r++)
		    {
			regCR(i_r, SIO(i_io,'s',i_r-tca), "R");
			if(wr) regCR(i_r, SIO(i_io,'s',i_r-tca), "R", true);
		    }
		}
		else if(ioId.size() > 1 && isdigit(ioId[1]))
		{
		    int tca = strtol(ioId.data()+1, NULL, 0);
		    if(tolower(ioId[0]) == 'C')
		    {
			regCR(tca, i_io, "C");
			if(wr) regCR(tca, i_io, "C", true);
		    }
		    else
		    {
			regCR(tca, i_io, "R");
			if(wr) regCR(tca, i_io, "R", true);
		    }
		}
	    }
	}

	// Start task
	SYS->taskCreate(nodePath('.',true), 0, Task, this);
    }
    //Disable node
    if(!vl)
    {
	// Stop the calc data task
	if(prcSt) SYS->taskDestroy(nodePath('.',true), &endrunRun);

	// Data structure delete
	if(data) { delete data; data = NULL; }
    }

    mEn = vl;
}

string Node::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat())
    {
	rez = _("Enabled. ");
	switch(mode())
	{
	    case MD_DATA:
		rez += TSYS::strMess(_("Spent time: %s. Requests %.4g. Read registers %.4g, coils %.4g, discrete inputs %.4g, input registers %.4g. Writed registers %.4g, coils %.4g."),
		    tm2s(tmProc).c_str(), cntReq, data->rReg, data->rCoil, data->rBit, data->rIReg, data->wReg, data->wCoil);
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
	switch(pdu[0])
	{
	    case 0x01:	//Read multiple coils
	    {
		int c_sz = 0;
		if(pdu.size() == 5) c_sz = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		if(c_sz < 1 || c_sz > 2000) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		pdu.assign(1, pdu[0]);
		pdu += (char)(c_sz/8+((c_sz%8)?1:0));
		pdu += string(pdu[1], (char)0);

		bool isData = false;
		map<int,SIO>::iterator itc;
		for(int i_c = c_addr; i_c < (c_addr+c_sz); i_c++)
		{
		    if((itc=data->coilR.find(i_c)) != data->coilR.end())
		    {
			if(data->val.getB(itc->second.id)) pdu[2+(i_c-c_addr)/8] |= (1<<((i_c-c_addr)%8));
			isData = true;
		    }
		}
		if(!isData)	{ pdu.assign(1, pdu[0]|0x80); pdu += 0x2; return true; }

		data->rCoil += c_sz;

		return true;
	    }

	    case 0x02://Read discrete inputs
	    {
		int c_sz = 0;
		if(pdu.size() == 5) c_sz = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		if(c_sz < 1 || c_sz > 2000) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		pdu.assign(1, pdu[0]);
		pdu += (char)(c_sz/8+((c_sz%8)?1:0));
		pdu += string(pdu[1], (char)0);

		bool isData = false;
		map<int,SIO>::iterator itc;
		for(int i_c = c_addr; i_c < (c_addr+c_sz); i_c++)
		{
		    if((itc=data->bitR.find(i_c)) != data->bitR.end())
		    {
			if(data->val.getB(itc->second.id)) pdu[2+(i_c-c_addr)/8] |= (1<<((i_c-c_addr)%8));
			isData = true;
		    }
		}
		if(!isData)	{ pdu.assign(1, pdu[0]|0x80); pdu += 0x2; return true; }

		data->rBit += c_sz;

		return true;
	    }
	    case 0x03:	//Read multiple registers
	    {
		int r_sz = 0;
		if(pdu.size() == 5) r_sz = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		if(r_sz < 1 || r_sz > 125) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		pdu.assign(1, pdu[0]);
		pdu += (char)(r_sz*2);

		bool isData = false;
		map<int,TVariant> grpVals;
		map<int,SIO>::iterator itr;
		for(int i_r = r_addr; i_r < (r_addr+r_sz); i_r++)
		{
		    unsigned short val = 0;
		    if((itr=data->regR.find(i_r)) != data->regR.end())
		    {
			switch(itr->second.sTp)
			{
			    case 'i':
			    {
				union { uint16_t r[4]; int64_t i; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.i = grpValIt->second.getI();
				else { wl.i = data->val.getI(itr->second.id); grpVals[itr->second.id] = wl.i; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 'f':
			    {
				union { uint16_t r[2]; float f; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.f = grpValIt->second.getR();
				else { wl.f = data->val.getR(itr->second.id); grpVals[itr->second.id] = wl.f; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 'd':
			    {
				union { uint16_t r[4]; double d; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.d = grpValIt->second.getR();
				else { wl.d = data->val.getR(itr->second.id); grpVals[itr->second.id] = wl.d; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 's':
			    {
				string valIO;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	valIO = grpValIt->second.getS();
				else { valIO = data->val.getS(itr->second.id); grpVals[itr->second.id] = valIO; }
				valIO.resize((itr->second.pos+1)*2, 0);
				val = TSYS::getUnalign16(valIO.data()+itr->second.pos*2);
				break;
			    }
			    default:	val = data->val.getI(itr->second.id);	break;
			}
			isData = true;
		    }
		    pdu += TSYS::strEncode(string((char*)&val,2), TSYS::Reverse);
		}
		if(!isData)	{ pdu.assign(1, pdu[0]|0x80); pdu += 0x2; return true; }

		data->rReg += r_sz;

		return true;
	    }
	    case 0x04:	//Read input registers
	    {
		int r_sz = 0;
		if(pdu.size() == 5) r_sz = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		if(r_sz < 1 || r_sz > 125) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		pdu.assign(1, pdu[0]);
		pdu += (char)(r_sz*2);

		bool isData = false;
		map<int,TVariant> grpVals;
		map<int,SIO>::iterator itr;
		for(int i_r = r_addr; i_r < (r_addr+r_sz); i_r++)
		{
		    unsigned short val = 0;
		    if((itr=data->regIR.find(i_r)) != data->regIR.end())
		    {
			switch(itr->second.sTp)
			{
			    case 'i':
			    {
				union { uint16_t r[4]; int64_t i; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.i = grpValIt->second.getI();
				else { wl.i = data->val.getI(itr->second.id); grpVals[itr->second.id] = wl.i; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 'f':
			    {
				union { uint16_t r[2]; float f; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.f = grpValIt->second.getR();
				else { wl.f = data->val.getR(itr->second.id); grpVals[itr->second.id] = wl.f; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 'd':
			    {
				union { uint16_t r[4]; double d; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	wl.d = grpValIt->second.getR();
				else { wl.d = data->val.getR(itr->second.id); grpVals[itr->second.id] = wl.d; }
				val = wl.r[itr->second.pos];
				break;
			    }
			    case 's':
			    {
				string valIO;
				map<int,TVariant>::iterator grpValIt = grpVals.find(itr->second.id);
				if(grpValIt != grpVals.end())	valIO = grpValIt->second.getS();
				else { valIO = data->val.getS(itr->second.id); grpVals[itr->second.id] = valIO; }
				valIO.resize((itr->second.pos+1)*2, 0);
				val = TSYS::getUnalign16(valIO.data()+itr->second.pos*2);
				break;
			    }
			    default:	val = data->val.getI(itr->second.id);	break;
			}
			isData = true;
		    }
		    pdu += TSYS::strEncode(string((char*)&val,2), TSYS::Reverse);
		}
		if(!isData)	{ pdu.assign(1, pdu[0]|0x80); pdu += 0x2; return true; }

		data->rIReg += r_sz;

		return true;
	    }
	    case 0x05:	//Write single coil
	    {
		if(pdu.size() != 5) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];

		map<int,SIO>::iterator ic = data->coilW.find(c_addr);
		if(ic == data->coilW.end()) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else
		{
		    data->val.setB(ic->second.id, (bool)pdu[3]);
		    map<int,AutoHD<TVal> >::iterator il = data->lnk.find(ic->second.id);
		    if(il != data->lnk.end() && !il->second.freeStat()) il->second.at().setB((bool)pdu[3]);
		}

		data->wCoil++;

		return true;
	    }
	    case 0x06:	//Write single register
	    {
		if(pdu.size() != 5) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_addr = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];

		map<int,AutoHD<TVal> >::iterator il;
		map<int,SIO>::iterator ir = data->regW.find(r_addr);
		if(ir == data->regW.end()) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else
		{
		    unsigned short val = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		    switch(ir->second.sTp)
		    {
			case 'i':
			{
			    union { uint16_t r[4]; int64_t i; } wl;
			    wl.i = data->val.getI(ir->second.id);
			    wl.r[ir->second.pos] = val;
			    data->val.setI(ir->second.id, wl.i);
			    if((il=data->lnk.find(ir->second.id)) != data->lnk.end() && !il->second.freeStat())	il->second.at().setI(wl.i);
			    break;
			}
			case 'f':
			{
			    union { uint16_t r[2]; float f; } wl;
			    wl.f = data->val.getR(ir->second.id);
			    wl.r[ir->second.pos] = val;
			    data->val.setR(ir->second.id, wl.f);
			    if((il=data->lnk.find(ir->second.id)) != data->lnk.end() && !il->second.freeStat())	il->second.at().setR(wl.f);
			    break;
			}
			case 'd':
			{
			    union { uint16_t r[4]; double d; } wl;
			    wl.d = data->val.getR(ir->second.id);
			    wl.r[ir->second.pos] = val;
			    data->val.setR(ir->second.id, wl.d);
			    if((il=data->lnk.find(ir->second.id)) != data->lnk.end() && !il->second.freeStat())	il->second.at().setR(wl.d);
			    break;
			}
			case 's':
			{
			    string valIO = data->val.getS(ir->second.id);
			    valIO.resize(vmax((int)valIO.size(),(ir->second.pos+1)*2), 0);
			    valIO.replace(ir->second.pos*2,2,(char*)&val,2);
			    data->val.setS(ir->second.id, valIO);
			    if((il=data->lnk.find(ir->second.id)) != data->lnk.end() && !il->second.freeStat())	il->second.at().setS(valIO);
			    break;
			}
			default:
			    data->val.setI(ir->second.id, val);
			    if((il=data->lnk.find(ir->second.id)) != data->lnk.end() && !il->second.freeStat())	il->second.at().setI(val);
			    break;
		    }
		}

		data->wReg++;

		return true;
	    }
	    case 0x0F:	//Write multiple coils
	    {
		if(pdu.size() < 6) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int c_aSt = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		int c_aCnt = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		int bCnt = (unsigned char)pdu[5];
		if((int)pdu.size() != (6+bCnt) || bCnt < (c_aCnt/8)) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		bool noWrReg = false;
		for(int i_c = 0; i_c < c_aCnt; i_c++)
		{
		    map<int,SIO>::iterator ic = data->coilW.find(c_aSt+i_c);
		    if(ic == data->coilW.end()) noWrReg = true;
		    else
		    {
			data->val.setB(ic->second.id, (bool)(1&(pdu[6+i_c/8]>>(i_c%8))));
			map<int,AutoHD<TVal> >::iterator il = data->lnk.find(ic->second.id);
			if(il != data->lnk.end() && !il->second.freeStat()) il->second.at().setB(data->val.getB(ic->second.id));
			data->wCoil++;
		    }
		}
		if(noWrReg) { pdu.assign(1, pdu[0]|0x80); pdu += 0x2; }
		else pdu.resize(5);

		return true;
	    }
	    case 0x10:	//Write multiple register
	    {
		if(pdu.size() < 6) { pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		int r_aSt = (unsigned short)(pdu[1]<<8) | (unsigned char)pdu[2];
		int r_aCnt = (unsigned short)(pdu[3]<<8) | (unsigned char)pdu[4];
		int bCnt = (unsigned char)pdu[5];
		if((int)pdu.size() != (6+bCnt) || bCnt < (r_aCnt*2) || r_aCnt > 123)
		{ pdu.assign(1, pdu[0]|0x80); pdu += 0x3; return true; }
		ResAlloc res(data->val.func()->fRes(), true);
		bool noWrReg = false;
		map<int,TVariant> grpVals;
		map<int,AutoHD<TVal> >::iterator il;
		for(int i_r = 0; i_r < r_aCnt; i_r++)
		{
		    map<int,SIO>::iterator ir = data->regW.find(r_aSt+i_r);
		    if(ir == data->regW.end()) noWrReg = true;
		    else
		    {
			unsigned short val = (unsigned short)(pdu[6+i_r*2]<<8) | (unsigned char)pdu[6+i_r*2+1];
			switch(ir->second.sTp)
			{
			    case 'i':
			    {
				union { uint16_t r[4]; int64_t i; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				wl.i = (grpValIt != grpVals.end()) ? grpValIt->second.getI() : data->val.getI(ir->second.id);
				wl.r[ir->second.pos] = val;
				grpVals[ir->second.id] = wl.i;
				break;
			    }
			    case 'f':
			    {
				union { uint16_t r[2]; float f; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				wl.f = (grpValIt != grpVals.end()) ? grpValIt->second.getR() : data->val.getR(ir->second.id);
				wl.r[ir->second.pos] = val;
				grpVals[ir->second.id] = wl.f;
				break;
			    }
			    case 'd':
			    {
				union { uint16_t r[4]; double d; } wl;
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				wl.d = (grpValIt != grpVals.end()) ? grpValIt->second.getR() : data->val.getR(ir->second.id);
				wl.r[ir->second.pos] = val;
				grpVals[ir->second.id] = wl.d;
				break;
			    }
			    case 's':
			    {
				map<int,TVariant>::iterator grpValIt = grpVals.find(ir->second.id);
				string valIO = (grpValIt != grpVals.end()) ? grpValIt->second.getS() : data->val.getS(ir->second.id);
				valIO.resize(vmax((int)valIO.size(),(ir->second.pos+1)*2), 0);
				valIO.replace(ir->second.pos*2,2,(char*)&val,2);
				grpVals[ir->second.id] = valIO;
				break;
			    }
			    default:
				data->val.setI(ir->second.id, val);
				if((il=data->lnk.find(ir->second.id)) != data->lnk.end() && !il->second.freeStat()) il->second.at().setI(val);
				break;
			}
		    }
		    data->wReg++;
		}
		// Real write cached values
		for(map<int,TVariant>::iterator i_g = grpVals.begin(); i_g != grpVals.end(); ++i_g)
		{
		    data->val.set(i_g->first, i_g->second);
		    if((il=data->lnk.find(i_g->first)) != data->lnk.end() && !il->second.freeStat()) il->second.at().set(i_g->second);
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
    else if(mode() == MD_GT_ND || mode() == MD_GT_NET)
    {
	try
	{
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
	}catch(TError err) { pdu.assign(1, pdu[0]|0x80); pdu += 0xA; }

	return true;
    }

    return true;
}

void *Node::Task( void *ind )
{
    Node &nd = *(Node*)ind;

    nd.endrunRun = false;
    nd.prcSt = true;

    bool isStart = true;
    bool isStop  = false;

    int ioFrq = nd.data->val.ioId("f_frq");
    int ioStart = nd.data->val.ioId("f_start");
    int ioStop = nd.data->val.ioId("f_stop");

    for(unsigned int clc = 0; true; clc++)
    {
	if(SYS->daq().at().subStartStat())
	{
	    int64_t t_cnt = TSYS::curTime();

	    //Setting special IO
	    if(ioFrq >= 0) nd.data->val.setR(ioFrq, (float)1/nd.period());
	    if(ioStart >= 0) nd.data->val.setB(ioStart, isStart);
	    if(ioStop >= 0) nd.data->val.setB(ioStop, isStop);

	    try
	    {
		//Get input links
		map< int, AutoHD<TVal> >::iterator li;
		for(li = nd.data->lnk.begin(); li != nd.data->lnk.end(); li++)
		{
		    if(li->second.freeStat())
		    {
			nd.data->val.setS(li->first, EVAL_STR);
			if(!(clc%(int)vmax(1,(float)1/nd.period())))
			    li->second = SYS->daq().at().attrAt(nd.io(li->first)->rez(), '.', true);
			if(li->second.freeStat()) continue;
		    }
		    switch(nd.data->val.ioType(li->first))
		    {
			case IO::String:  nd.data->val.setS(li->first, li->second.at().getS());	break;
			case IO::Integer: nd.data->val.setI(li->first, li->second.at().getI());	break;
			case IO::Real:	  nd.data->val.setR(li->first, li->second.at().getR());	break;
			case IO::Boolean: nd.data->val.setB(li->first, li->second.at().getB());	break;
			default: break;
		    }
		}

		nd.data->val.setMdfChk(true);
		nd.data->val.calc();

		//Put output links
		for(li = nd.data->lnk.begin(); li != nd.data->lnk.end(); li++)
		    if(!li->second.freeStat() && !(li->second.at().fld().flg()&TFld::NoWrite) && nd.data->val.ioMdf(li->first))
			switch(nd.data->val.ioType(li->first))
			{
			    case IO::String:	li->second.at().setS(nd.data->val.getS(li->first));	break;
			    case IO::Integer:	li->second.at().setI(nd.data->val.getI(li->first));	break;
			    case IO::Real:	li->second.at().setR(nd.data->val.getR(li->first));	break;
			    case IO::Boolean:	li->second.at().setB(nd.data->val.getB(li->first));	break;
			    default: break;
			}
	    }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str() );
		mess_err(nd.nodePath().c_str(), _("Calculate node's function error."));
	    }

	    //Calc acquisition process time
	    nd.tmProc = TSYS::curTime()-t_cnt;
	}

	if(isStop) break;
	TSYS::taskSleep((int64_t)(1e9*nd.period()));
	if(nd.endrunRun) isStop = true;
	isStart = false;
	nd.modif();
    }

    nd.prcSt = false;

    return NULL;
}

void Node::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Node: ")+name(),RWRWR_,"root",SPRT_ID);
	if(ctrMkNode("area",opt,-1,"/nd",_("Node")))
	{
	    if(ctrMkNode("area",opt,-1,"/nd/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/nd/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/nd/st/en_st",_("Enable"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/nd/st/db",_("DB"),RWRWR_,"root",SPRT_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/nd/cfg",_("Configuration")))
	    {
		TConfig::cntrCmdMake(opt, "/nd/cfg", 0, "root", SPRT_ID, RWRWR_);
		// Append configuration properties
		XMLNode *xt = ctrId(opt->childGet(0),"/nd/cfg/InTR",true);
		if(xt) xt->setAttr("dest","sel_ed")->setAttr("select","/nd/cfg/ls_itr");
		xt = ctrId(opt->childGet(0),"/nd/cfg/TO_TR",true);
		if(xt) xt->setAttr("dest","sel_ed")->setAttr("select","/nd/cfg/ls_otr");
		xt = ctrId(opt->childGet(0),"/nd/cfg/DT_PROG",true);
		if(xt) xt->parent()->childDel(xt);
	    }
	}
	if(mode() == MD_DATA && ctrMkNode("area",opt,-1,"/dt",_("Data")))
	{
	    if(ctrMkNode("table",opt,-1,"/dt/io",_("IO"),RWRWR_,"root",SPRT_ID,3,"s_com","add,del,ins,move","rows","15",
		"help",_("For \"Id\" field provide specific ModBus data form:\n"
			 "  \"R{N}[w]\" - specific register form, can expanded by suffixes:\n"
			 "                \"i\"-Int32, \"f\"-Float, \"d\"-Double, \"s\"-String;\n"
			 "  \"R:{N}:[w]\" - classic register form, can expanded by suffixes:\n"
			 "                \"i4\"-Int32, \"i8\"-Int64, \"f\"-Float, \"d\"-Double, \"s\"-String;\n"
			 "  \"C{N}[w]\" or \"C:{N}:[w]\" - coil.\n"
			 "Where:\n"
			 "  {N} - ModBus device's data address (dec, hex or octal) [0...65535];\n"
			 "  w   - optional symbol for writing allow indicate.\n"
			 "Examples:\n"
			 "  \"R0x300w\" - register access;\n"
			 "  \"C100w\" - coin access, allow for write;\n"
			 "  \"R_f200\" - get float from registers 200 and 201;\n"
			 "  \"R_i400,300\" - get int32 from registers 300 and 400;\n"
			 "  \"R_s15,20\" - get string, registers block, from register 15 and size 20;\n"
			 "  \"R_i8:0x10:w\" - get and set int64 into registers [0x10-0x13];\n"
			 "  \"R_d:0x20,0x30\" - get double float point (8 byte) from registers [0x20,0x30-0x32].")))
	    {
		ctrMkNode("list",opt,-1,"/dt/io/id",_("Id"),RWRWR_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/dt/io/nm",_("Name"),RWRWR_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/dt/io/tp",_("Type"),RWRWR_,"root",SPRT_ID,5,"tp","dec","idm","1","dest","select",
		    "sel_id",TSYS::strMess("%d;%d;%d;%d",IO::Real,IO::Integer,IO::Boolean,IO::String).c_str(),
		    "sel_list",_("Real;Integer;Boolean;String"));
		ctrMkNode("list",opt,-1,"/dt/io/lnk",_("Link"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/dt/io/vl",_("Value"),RWRWR_,"root",SPRT_ID,1,"tp","str");
	    }
	    ctrMkNode("fld",opt,-1,"/dt/progLang",_("Program language"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","sel_ed","select","/dt/plang_ls");
	    ctrMkNode("fld",opt,-1,"/dt/prog",_("Program"),RWRWR_,"root",SPRT_ID,3,"tp","str","rows","10","SnthHgl","1");
	}
	if(mode() == MD_DATA && ctrMkNode("area",opt,-1,"/lnk",_("Links")))
	    for(int i_io = 0; i_io < ioSize(); i_io++)
		if(io(i_io)->flg()&IsLink)
		    ctrMkNode("fld",opt,-1,("/lnk/el_"+i2s(i_io)).c_str(),io(i_io)->name(),enableStat()?R_R_R_:RWRWR_,"root",SPRT_ID,
			3,"tp","str","dest","sel_ed","select",("/lnk/ls_"+i2s(i_io)).c_str());
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/nd/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/nd/st/en_st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(atoi(opt->text().c_str()));
    }
    else if(a_path == "/nd/st/db")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/nd/cfg/ls_itr" && ctrChkNode(opt))
    {
	if(mode() != MD_GT_NET) opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path == "/nd/cfg/ls_otr" && ctrChkNode(opt))
    {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path.substr(0,7) == "/nd/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SPRT_ID,RWRWR_);
    else if(a_path == "/dt/io")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    XMLNode *nId   = ctrMkNode("list",opt,-1,"/dt/io/id","");
	    XMLNode *nNm   = ctrMkNode("list",opt,-1,"/dt/io/nm","");
	    XMLNode *nType = ctrMkNode("list",opt,-1,"/dt/io/tp","");
	    XMLNode *nLnk  = ctrMkNode("list",opt,-1,"/dt/io/lnk","");
	    XMLNode *nVal  = ctrMkNode("list",opt,-1,"/dt/io/vl","");

	    for(int id = 0; id < ioSize(); id++)
	    {
		if(nId)	 nId->childAdd("el")->setText(io(id)->id());
		if(nNm)	 nNm->childAdd("el")->setText(io(id)->name());
		if(nType)nType->childAdd("el")->setText(i2s(io(id)->type()));
		if(nLnk) nLnk->childAdd("el")->setText((io(id)->flg()&Node::IsLink)?"1":"0");
		if(nVal) nVal->childAdd("el")->setText((data && data->val.func()) ? data->val.getS(id) : io(id)->def());
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    if(enableStat()) throw TError(nodePath().c_str(),_("Disable node for this operation"));
	    IO *ioPrev = ioSize() ? io(ioSize()-1) : NULL;
	    if(ioPrev) ioAdd(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~Node::LockAttr)));
	    else ioAdd(new IO("new",_("New IO"),IO::Integer,IO::Default));
	    modif();
	}
	if(ctrChkNode(opt,"ins",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    if(enableStat()) throw TError(nodePath().c_str(),_("Disable node for this operation"));
	    int row = atoi(opt->attr("row").c_str());
	    IO *ioPrev = row ? io(row-1) : NULL;
	    if(ioPrev) ioIns(new IO(TSYS::strLabEnum(ioPrev->id()).c_str(),TSYS::strLabEnum(ioPrev->name()).c_str(),ioPrev->type(),ioPrev->flg()&(~Node::LockAttr)), row);
	    else ioIns(new IO("new",_("New IO"),IO::Integer,IO::Default), row);
	    modif();
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    if(enableStat()) throw TError(nodePath().c_str(),_("Disable node for this operation"));
	    int row = atoi(opt->attr("row").c_str());
	    if(io(row)->flg()&Node::LockAttr)
		throw TError(nodePath().c_str(),_("Deleting lock attribute is not allowed."));
	    ioDel(row);
	    modif();
	}
	if(ctrChkNode(opt,"move",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    if(enableStat()) throw TError(nodePath().c_str(),_("Disable node for this operation"));
	    ioMove(atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str())); modif();
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    int row = atoi(opt->attr("row").c_str());
	    string col = opt->attr("col");
	    if(enableStat() && col != "vl") throw TError(nodePath().c_str(),_("Disable node for this operation"));
	    if(io(row)->flg()&Node::LockAttr)	throw TError(nodePath().c_str(),_("Changing locked attribute is not allowed."));
	    if((col == "id" || col == "nm") && !opt->text().size())	throw TError(nodePath().c_str(),_("Empty value is not valid."));
	    if(col == "id")		io(row)->setId(opt->text());
	    else if(col == "nm")	io(row)->setName(opt->text());
	    else if(col == "tp")	io(row)->setType((IO::Type)atoi(opt->text().c_str()));
	    else if(col == "lnk")	io(row)->setFlg(atoi(opt->text().c_str()) ? (io(row)->flg()|Node::IsLink) : (io(row)->flg() & ~Node::IsLink));
	    else if(col == "vl")	(data && data->val.func()) ? data->val.setS(row,opt->text()) : io(row)->setDef(opt->text());
	    modif();
	}
    }
    else if(a_path == "/dt/progLang")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(progLang());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setProgLang(opt->text());
    }
    else if(a_path == "/dt/prog")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(prog());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    try
	    {
		SYS->daq().at().at(TSYS::strParse(progLang(),0,".")).at().
				compileFuncSynthHighl(TSYS::strParse(progLang(),1,"."),*opt);
	    } catch(...){ }
    }
    else if(a_path == "/dt/plang_ls" && ctrChkNode(opt))
    {
	string tplng = progLang();
	int c_lv = 0;
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::strSepParse(tplng,0,'.',&c_off)).size(); c_lv++)
	{
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	vector<string>  ls;
	switch(c_lv)
	{
	    case 0:
		SYS->daq().at().modList(ls);
		for(unsigned i_l = 0; i_l < ls.size(); )
		    if(!SYS->daq().at().at(ls[i_l]).at().compileFuncLangs()) ls.erase(ls.begin()+i_l);
		    else i_l++;
		break;
	    case 1:
		if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
		    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(&ls);
		break;
	}
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if(a_path.substr(0,8) == "/lnk/ls_" && ctrChkNode(opt))
	SYS->daq().at().ctrListPrmAttr(opt, io(atoi(a_path.substr(8).c_str()))->rez(), false, '.');
    else if(a_path.substr(0,8) == "/lnk/el_")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    opt->setText(io(atoi(a_path.substr(8).c_str()))->rez());
	    if(!SYS->daq().at().attrAt(opt->text(),'.',true).freeStat()) opt->setText(opt->text()+" (+)");
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    io(atoi(a_path.substr(8).c_str()))->setRez(TSYS::strParse(opt->text(),0," "));
	    modif();
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

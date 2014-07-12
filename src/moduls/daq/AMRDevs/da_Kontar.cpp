
//OpenSCADA system module DAQ.AMRDevs file: da_Kontar.cpp
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko, <rom_as@oscada.org>           *
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

#include <fcntl.h>

#include <tsys.h>

#include "mod_amr.h"
#include "da_Kontar.h"

using namespace AMRDevs;

//*************************************************
//* Kontar					  *
//*************************************************
Kontar::Kontar( ) : TTipParam("kontar", _("Kontar"))
{
    fldAdd(new TFld("ADDR",_("Input transport"),TFld::String,TCfg::NoVal,"30"));
    fldAdd(new TFld("PASS",_("Master password"),TFld::String,TCfg::NoVal,"30","00 00 00 00 00 00 00 00"));
    fldAdd(new TFld("M_PLC",_("Master PLC serial number"),TFld::Integer,TFld::HexDec|TCfg::NoVal,"10","0"));
    fldAdd(new TFld("CNTR_NET_CFG",_("Controllers network config-file"),TFld::String,TCfg::NoVal,"100"));
    fldAdd(new TFld("PLC",_("PLC serial number"),TFld::Integer,TCfg::NoVal,"10","0"));
    fldAdd(new TFld("SET_CLC",_("Set PLC clock on different for more (s)"),TFld::Integer,TCfg::NoVal,"3","0","0;999"));
    fldAdd(new TFld("ZONE_CLC",_("PLC clock's zone"),TFld::Integer,TCfg::NoVal,"2","2","-12;12"));
}

Kontar::~Kontar( )	{ }

void Kontar::create( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    p->extPrms = new tval();
}

void Kontar::destroy( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    delete (tval*)p->extPrms;
    p->extPrms = NULL;
}

void Kontar::enable( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;

    int selPLC = ip->cfg("PLC");

    //Parse for selected controller parameters
    XMLNode *nDev = NULL, *nId = NULL, *nNm = NULL, *nLs = NULL, *nPrm = NULL;
    MtxAlloc res(p->dataM, true);
    ePrm->mBlks.clear();
    for(int i_dev = 0; (nDev=ePrm->cfg.childGet("Device",i_dev,true)); i_dev++)
    {
	if(!((nId=nDev->childGet("Number",0,true)) && atoi(nId->text().c_str()) == selPLC)) continue;

	// Get the parameters List
	for(int i_ls = 0; (nLs=nDev->childGet("List",i_ls,true)); i_ls++)
	{
	    string lsNm = (nNm=nLs->childGet("Name",0,true)) ? nNm->text() : (_("List")+i_ls);
	    for(int i_p = 0; (nPrm=nLs->childGet("Parameter",i_p,true)); i_p++)
	    {
		if(!(nId=nPrm->childGet("Address",0,true)))	continue;
		string aid = lsNm;
		string pNm;
		if((nNm=nPrm->childGet("Name",0,true))) { aid += "_"+nNm->text(); pNm = lsNm+": "+nNm->text(); }
		else { aid += "_"+nId->text(); pNm = lsNm + ": " + _("Parameter")+nId->text(); }
		aid = TSYS::strEncode(aid, TSYS::oscdID);
		string pTp = (nNm=nPrm->childGet("Type",0,true)) ? nNm->text() : "Float";
		TFld::Type tp = TFld::Real; int tpSz = 4;
		if(strcasecmp(pTp.c_str(),"short") == 0)	{ tp = TFld::Integer; tpSz = 2; }
		else if(strcasecmp(pTp.c_str(),"bool") == 0)	{ tp = TFld::Boolean; tpSz = 1; }
		else if(strcasecmp(pTp.c_str(),"time") == 0 ||
			strcasecmp(pTp.c_str(),"date") == 0)	{ tp = TFld::String; tpSz = 2; }
		bool isSt = (nNm=nPrm->childGet("IsStatic",0,true)) ? (nNm->text()!="False") : true;
		unsigned flg = isSt ? (unsigned)TFld::NoWrite : (unsigned)TVal::DirWrite;
		if(!p->els.fldPresent(aid) || p->els.fldAt(p->els.fldId(aid)).type() != tp)
		{
		    if(p->els.fldPresent(aid)) p->els.fldDel(p->els.fldId(aid));
		    p->els.fldAdd(new TFld(aid.c_str(),"",tp,flg));
		}
		int el_id = p->els.fldId(aid);
		p->els.fldAt(el_id).setDescr(pNm);
		p->els.fldAt(el_id).setReserve(pTp+":"+i2s(tpSz)+":"+nId->text());
		regVal(p, atoi(nId->text().c_str()), tpSz);
		p->als.push_back(aid);
	    }
	}
    }
}

void Kontar::regVal( TMdPrm *p, int off, int sz )
{
    if(off < 0)	return;

    //Register to acquisition block
    vector<SMemBlk> &wCnt = ((tval*)p->extPrms)->mBlks;
    unsigned i_b;
    for(i_b = 0; i_b < wCnt.size(); i_b++)
    {
	if(off < wCnt[i_b].off)
	{
	    if((wCnt[i_b].val.size()+wCnt[i_b].off-off) < MaxLenReq)
	    {
		wCnt[i_b].val.insert(0, wCnt[i_b].off-off, 0);
		wCnt[i_b].off = off;
	    }
	    else wCnt.insert(wCnt.begin()+i_b, SMemBlk(off,sz));
	}
	else if((off+sz) > (wCnt[i_b].off+(int)wCnt[i_b].val.size()))
	{
	    if((off+sz-wCnt[i_b].off) < MaxLenReq)
		wCnt[i_b].val.append((off+sz)-(wCnt[i_b].off+wCnt[i_b].val.size()), 0);
	    else continue;
	}
	break;
    }
    if(i_b >= wCnt.size()) wCnt.insert(wCnt.begin()+i_b,SMemBlk(off,sz));
}

const char *Kontar::getVal( TMdPrm *ip, int off, int dtSz )
{
    TMdPrm *p = (TMdPrm*)ip;
    tval *ePrm = (tval*)p->extPrms;

    const char *rez = NULL;

    vector<SMemBlk> &wCnt = ePrm->mBlks;
    for(unsigned i_b = 0; i_b < wCnt.size(); i_b++)
	if(off >= wCnt[i_b].off && (off+dtSz) <= (wCnt[i_b].off+(int)wCnt[i_b].val.size()))
	{
	    p->mErr = wCnt[i_b].err;
	    if(p->mErr.empty())	rez = wCnt[i_b].val.data() + (off-wCnt[i_b].off);
	    break;
	}

    return rez;
}

string Kontar::req( TMdPrm *p, string &pdu, bool passUpdate )
{
    tval *ePrm = (tval*)p->extPrms;
    string mbap, err;
    char buf[1000];

    uint32_t cntrMN = p->cfg("M_PLC").getI();

    try
    {
	//Try connect to previous transport
	AutoHD<TTipTransport> ttr = SYS->transport().at().at("Sockets");
	AutoHD<TTransportOut> trO;
	if(!(ePrm->prevTr.size() && ttr.at().outPresent(ePrm->prevTr) &&
		(trO=ttr.at().outAt(ePrm->prevTr)).at().startStat() && trO.at().prm1() == cntrMN))
	{
	    ePrm->prevTr = "";
	    trO.free();
	}

	//Find for proper transport by send master PLC serial number request
	if(trO.freeStat())
	{
	    //Same find
	    vector<AutoHD<TTransportOut> > trsO = ttr.at().inAt(p->cfg("ADDR")).at().assTrs();
	    for(unsigned i_t = 0; i_t < trsO.size() && trO.freeStat(); i_t++)
	    {
		if(!trsO[i_t].at().startStat())	continue;
		// Serial number request
		if(!trsO[i_t].at().prm1())
		    try
		    {
			int resp_len = 0;
			mbap = char(0);

			if(p->owner().messLev() == TMess::Debug)
			    mess_debug_(p->nodePath().c_str(), _("Master PLC ID Request: '%s'"),TSYS::strDecode(mbap,TSYS::Bin," ").c_str());

			if((resp_len=trsO[i_t].at().messIO(mbap.data(),mbap.size(),buf,sizeof(buf))) >= 4)
			{
			    trsO[i_t].at().setPrm1(TSYS::i32_BE(*(uint32_t*)buf));
			    trsO[i_t].at().setDscr(TSYS::strMess(_("Connection from PLC Kontar %xh."),(int)TSYS::i32_BE(*(uint32_t*)buf)));
			    if(resp_len >= 13 && (uint8_t)buf[4] == 0xC0)
			    {
				ePrm->pass = ePrm->RC5Decr(string(buf+5,8), ePrm->RC5Key(TSYS::strEncode(p->cfg("PASS").getS(),TSYS::Bin)));
				ePrm->key = ePrm->RC5Key(ePrm->pass);
				if(p->owner().messLev() == TMess::Debug)
				    mess_debug_(p->nodePath().c_str(), _("Password sequence set: '%s'."), TSYS::strDecode(ePrm->pass,TSYS::Bin," ").c_str());
			    }
			}

			if(p->owner().messLev() == TMess::Debug)
			    mess_debug_(p->nodePath().c_str(), _("Master PLC ID Response: '%s'"),TSYS::strDecode(string(buf,resp_len),TSYS::Bin," ").c_str());
		    }
		    catch(...) { }
		if(trsO[i_t].at().prm1() == cntrMN) { trO = trsO[i_t]; ePrm->prevTr = trO.at().id(); }
	    }
	}
	if(trO.freeStat()) throw TError(p->nodePath().c_str(), _("No a propper connection from PLC."));

	//Main request prepare
	mbap.reserve(pdu.size()+17);
	mbap.assign("\x7F\x00\x00\x01",4);		//???? Where the take for client
	mbap += ePrm->pass;				//Password
	mbap += (char)0;				//Unused
	uint32_t cntrN = TSYS::i32_BE(p->cfg("PLC").getI());
	mbap += string((char*)&cntrN,sizeof(cntrN));	//PLC number
	mbap += pdu;

	if(p->owner().messLev() == TMess::Debug)
	    mess_debug_(p->nodePath().c_str(), _("Request: '%s'"), TSYS::strDecode(mbap,TSYS::Bin," ").c_str());

	mbap = ePrm->RC5Encr(mbap, ePrm->key);

	ResAlloc resN(trO.at().nodeRes(), true);
	//Send request
	if(p->owner().messLev() == TMess::Debug)
	    mess_debug_(p->nodePath().c_str(), _("Request (enc): '%s'"), TSYS::strDecode(mbap,TSYS::Bin," ").c_str());

	int resp_len = trO.at().messIO(mbap.data(), mbap.size(), buf, sizeof(buf), 0, true);

	//Wait tail
	mbap.assign(buf, resp_len);
	while(resp_len)
	    try
	    {
		resp_len = trO.at().messIO(NULL, 0, buf, sizeof(buf), 0, true);
		mbap.append(buf, resp_len);
	    }
	    catch(TError err){ break; }

	if(mbap.size() < 5)	err = _("13:Error respond: Too short.");
	else
	{
	    if(TSYS::i32_BE(*(uint32_t*)mbap.data()) != cntrMN)
		err = TSYS::strMess(_("13:Unapt MasterPLC ID: %xh != %xh"),TSYS::i32_BE(*(uint32_t*)mbap.data()), cntrN);
	    else switch((uint8_t)mbap.data()[4])
	    {
		case 0xA0:	break;	//OK
		case 0xC0:
		    if(!passUpdate && mbap.size() >= 13)
		    {
			ePrm->pass = ePrm->RC5Decr(mbap.substr(5,8), ePrm->RC5Key(TSYS::strEncode(p->cfg("PASS").getS(),TSYS::Bin)));
			ePrm->key = ePrm->RC5Key(ePrm->pass);
			if(p->owner().messLev() == TMess::Debug)
			    mess_debug_(p->nodePath().c_str(), _("Password sequence set: '%s'."), TSYS::strDecode(ePrm->pass,TSYS::Bin," ").c_str());
			return req(p, pdu, true);
		    }
		    else err = TSYS::strMess(_("12:PLC: The password encripted by master key."));
		    break;
		case 0xE0:	err = TSYS::strMess(_("12:PLC: Error."));			break;
		case 0xE1:	err = TSYS::strMess(_("12:PLC: Unknown command."));		break;
		case 0xE2:	err = TSYS::strMess(_("12:PLC: Wrong command format."));	break;
		case 0xE5:	err = TSYS::strMess(_("12:PLC: Error read from slave."));	break;
		case 0xE6:	err = TSYS::strMess(_("12:PLC: Network update in process."));	break;
		case 0xD0:	//The data encripted the access password
		    if(p->owner().messLev() == TMess::Debug)
			mess_debug_(p->nodePath().c_str(), _("Response (enc): '%s'"), TSYS::strDecode(mbap,TSYS::Bin," ").c_str());
		    mbap.replace(5, string::npos, ePrm->RC5Decr(mbap.substr(5),ePrm->key));
		    break;
	    }
	    if(err.empty())
	    {
		if(p->owner().messLev() == TMess::Debug)
		    mess_debug_(p->nodePath().c_str(), _("Response: '%s'"), TSYS::strDecode(mbap,TSYS::Bin," ").c_str());
		pdu = mbap.substr(5);
	    }
	}
    }catch(TError er) { err = TSYS::strMess(_("14:Connection error - %s"),er.mess.c_str()); }

    if(!err.empty() && p->owner().messLev() == TMess::Debug)
	mess_debug_(p->nodePath().c_str(), _("Error: '%s': '%s'"),err.c_str(), TSYS::strDecode(mbap,TSYS::Bin," ").c_str());

    return err;
}

bool Kontar::cfgChange( TParamContr *ip, TCfg &cfg )
{
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;

    if(cfg.name() == "CNTR_NET_CFG")	//Reload config file
    {
	int cf_sz = 0;
	char *buf = NULL;
	int hd = open(cfg.getS().c_str(), O_RDONLY);
	if(hd >= 0 && (cf_sz=lseek(hd,0,SEEK_END)) > 0 && cf_sz < 100000)
	{
	    lseek(hd, 0, SEEK_SET);
	    buf = (char*)malloc(cf_sz+1);
	    if(read(hd,buf,cf_sz) != cf_sz) cf_sz = 0;
	}
	if(hd >= 0) close(hd);

	MtxAlloc res(p->dataM, true);
	ePrm->cfg.clear();
	try { if(cf_sz) ePrm->cfg.load(string(buf,cf_sz)); } catch(...) { }
	if(buf) free(buf);
    }

    return true;
}

void Kontar::getVals( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;
    string err;

    //Request blocks
    string pdu;
    for(unsigned i_b = 0; i_b < ePrm->mBlks.size(); i_b++)
    {
	if(err.size())	{ ePrm->mBlks[i_b].err = err; continue; }

	// Encode request
	pdu  = (char)0x67;				//Read memory
	pdu += (char)0x04;				//Memory, PARAMETER
	pdu += (char)0x00;
	pdu += (char)(ePrm->mBlks[i_b].off>>8);		//Address MSB
	pdu += (char)(ePrm->mBlks[i_b].off);		//Address LSB
	pdu += (char)(ePrm->mBlks[i_b].val.size()>>8);	//Number of registers MSB
	pdu += (char)(ePrm->mBlks[i_b].val.size());	//Number of registers LSB
	// Request to remote server
	if((err=ePrm->mBlks[i_b].err=req(p,pdu)).empty())
	{
	    if(pdu.size() < ePrm->mBlks[i_b].val.size())
		ePrm->mBlks[i_b].err = TSYS::strMess(_("15:Response PDU size (%d from %d) error."),pdu.size(),ePrm->mBlks[i_b].val.size());
	    else { ePrm->mBlks[i_b].val.assign(pdu,0,ePrm->mBlks[i_b].val.size()); p->numBytes += pdu.size(); }
	}
    }

    //Load values to attributes
    for(unsigned i_a = 0; i_a < p->els.fldSize(); i_a++)
    {
	AutoHD<TVal> val = p->vlAt(p->els.fldAt(i_a).name());
	int off = 0;
	string pTp = TSYS::strParse(val.at().fld().reserve(),0,":",&off);
	int pTpSz = atoi(TSYS::strParse(val.at().fld().reserve(),0,":",&off).c_str());
	int aoff = strtol(TSYS::strParse(val.at().fld().reserve(),0,":",&off).c_str(),NULL,0);
	const char *dt = getVal(p, aoff, pTpSz);
	switch(val.at().fld().type())
	{
	    case TFld::Boolean:	val.at().setB(dt?*dt:EVAL_BOOL, 0, true);	break;
	    case TFld::Integer:	val.at().setI(dt?(int16_t)TSYS::i16_BE(TSYS::getUnalign16(dt)):EVAL_INT, 0, true);	break;
	    case TFld::Real:
		if(!dt) val.at().setR(EVAL_REAL, 0, true);
		else
		{
		    union { uint32_t i; float f; } wl;
		    wl.i = TSYS::i32_BE(TSYS::getUnalign32(dt));
		    val.at().setR(wl.f, 0, true);
		}
		break;
	    case TFld::String:
	    {
		string vl = EVAL_STR;
		if(dt && strcasecmp(pTp.c_str(),"time") == 0)	  vl = TSYS::strMess("%d:%d", *dt, *(dt+1));
		else if(dt && strcasecmp(pTp.c_str(),"date") == 0)vl = TSYS::strMess("%d-%d", *dt, *(dt+1));
		val.at().setS(vl, 0, true);
		break;
	    }
	    default:	break;
	}
    }

    //Check and set clock at PLC
    if(err.empty() && (time(NULL)-ePrm->lstClcSet) > 60 && p->cfg("SET_CLC").getI() > 0)
    {
	pdu = (char)0x67;	//Read memory
	pdu += (char)0x04;	//Memory, PARAMETER
	pdu += (char)0x00;
	pdu += (char)0x00;	//Address MSB
	pdu += (char)0x4D;	//Address LSB
	pdu += (char)0x00;	//Number of registers MSB
	pdu += (char)0x07;	//Number of registers LSB
	// Request to remote server
	time_t sysTm = time(NULL);
	if((err=req(p,pdu)).empty() && pdu.size() >= 7)
	{
	    p->numBytes += pdu.size();

	    struct tm tm_tm;
	    gmtime_r(&sysTm, &tm_tm);
	    tm_tm.tm_sec	= atoi(i2s(pdu[0],TSYS::Hex).c_str());
	    tm_tm.tm_min	= atoi(i2s(pdu[1],TSYS::Hex).c_str());
	    tm_tm.tm_hour	= atoi(i2s(pdu[2],TSYS::Hex).c_str()) - p->cfg("ZONE_CLC").getI();
	    tm_tm.tm_mday	= atoi(i2s(pdu[4],TSYS::Hex).c_str());
	    tm_tm.tm_mon	= atoi(i2s(pdu[5],TSYS::Hex).c_str())-1;
	    tm_tm.tm_year	= 100+atoi(i2s(pdu[6],TSYS::Hex).c_str());
	    tm_tm.tm_wday	= atoi(i2s(pdu[3]-1,TSYS::Hex).c_str());
	    tm_tm.tm_yday	= -1;
	    tm_tm.tm_isdst	= -1;
	    time_t PLC_Tm = timegm(&tm_tm);
	    // Set new time
	    if(llabs(sysTm-PLC_Tm) > p->cfg("SET_CLC").getI())
	    {
		PLC_Tm = time(NULL);
		gmtime_r(&PLC_Tm, &tm_tm);
		tm_tm.tm_hour += p->cfg("ZONE_CLC").getI();
		PLC_Tm = timegm(&tm_tm);

		pdu  = (char)0x5A;	//Set clock
		pdu += (char)0x00;
		pdu += (char)strtol(i2s(tm_tm.tm_sec).c_str(),0,16);
		pdu += (char)strtol(i2s(tm_tm.tm_min).c_str(),0,16);
		pdu += (char)strtol(i2s(tm_tm.tm_hour).c_str(),0,16);
		pdu += (char)strtol(i2s(tm_tm.tm_wday+1).c_str(),0,16);
		pdu += (char)strtol(i2s(tm_tm.tm_mday).c_str(),0,16);
		pdu += (char)strtol(i2s(tm_tm.tm_mon+1).c_str(),0,16);
		pdu += (char)strtol(i2s(tm_tm.tm_year-100).c_str(),0,16);
		err = req(p, pdu);
		ePrm->lstClcSet = time(NULL);
	    }
	}
    }
}

void Kontar::vlSet( TParamContr *ip, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    TMdPrm *p = (TMdPrm *)ip;
    //tval *ePrm = (tval*)p->extPrms;

    if(!p->enableStat() || !p->owner().startStat())	vo.setS(EVAL_STR, 0, true);

    if(vl.isEVal() || vl == pvl) return;

    int off = 0;
    string pTp = TSYS::strParse(vo.fld().reserve(),0,":",&off);
    int pTpSz = atoi(TSYS::strParse(vo.fld().reserve(),0,":",&off).c_str());
    int aoff = strtol(TSYS::strParse(vo.fld().reserve(),0,":",&off).c_str(),NULL,0);

    //Value set
    string pdu;
    //Encode request
    pdu  = char(0x54);		//Write value
    pdu += char(0x07);		//Memory, PARAMETER
    pdu += char(0x00);
    pdu += char(aoff>>8);	//Address MSB
    pdu += char(aoff);		//Address LSB
    pdu += char(0x00);		//Number of registers MSB
    pdu += char(pTpSz);		//Number of registers LSB
    switch(vo.fld().type())
    {
	case TFld::Boolean:	{ pdu += char(vl.getB()); break; }
	case TFld::Integer:	{ int16_t tvl = TSYS::i16_BE(vl.getI()); pdu.append((char*)&tvl,sizeof(tvl));	break; }
	case TFld::Real:
	{
	    union { uint32_t i; float f; } wl;
	    wl.f = vl.getR();
	    wl.i = TSYS::i32_BE(wl.i);
	    pdu.append((char*)&wl.i,sizeof(wl.i));
	    break;
	}
	case TFld::String:
	{
	    int16_t tvl = 0;
	    if(strcasecmp(pTp.c_str(),"time") == 0)
		tvl = (char)atoi(TSYS::strParse(vl.getS(),0,":").c_str()) + ((char)atoi(TSYS::strParse(vl.getS(),1,":").c_str())<<8);
	    else if(strcasecmp(pTp.c_str(),"date") == 0)
		tvl = (char)atoi(TSYS::strParse(vl.getS(),0,"-").c_str()) + ((char)atoi(TSYS::strParse(vl.getS(),1,"-").c_str())<<8);
	    pdu.append((char*)&tvl, sizeof(tvl));
	    break;
	}
	default: break;
    }

    //Request to remote server
    req(p, pdu);
}

bool Kontar::cntrCmdProc( TParamContr *ip, XMLNode *opt )
{
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;

    // Get page info
    if(opt->name() == "info")
    {
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/ADDR",EVAL_STR,ip->owner().startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,
	    "dest","select","select","/prm/cfg/trLst");
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/M_PLC",EVAL_STR,RWRWR_,"root",SDAQ_ID,2,"dest","select","select","/prm/cfg/mPLCLst");
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/CNTR_NET_CFG",EVAL_STR,ip->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,
	    "dest","sel_ed","select","/prm/cfg/fileList");
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/PLC",EVAL_STR,ip->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "idm","1","dest","select","select","/prm/cfg/PLCList");
	return true;
    }

    // Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/trLst" && ip->ctrChkNode(opt,"get"))
	try
	{
	    vector<string> sls;
	    SYS->transport().at().at("Sockets").at().inList(sls);
	    for(unsigned i_s = 0; i_s < sls.size(); i_s++) opt->childAdd("el")->setText(sls[i_s]);
	} catch(TError err) { }
    else if(a_path == "/prm/cfg/mPLCLst" && ip->ctrChkNode(opt,"get"))
	try
	{
	    vector<AutoHD<TTransportOut> > trsO = SYS->transport().at().at("Sockets").at().inAt(p->cfg("ADDR")).at().assTrs();
	    for(unsigned i_t = 0; i_t < trsO.size(); i_t++)
		if(trsO[i_t].at().startStat())
		    opt->childAdd("el")->setText(u2s(trsO[i_t].at().prm1()));
	} catch(TError err) { }
    else if(a_path == "/prm/cfg/fileList" && ip->ctrChkNode(opt,"get")) TSYS::ctrListFS(opt, ip->cfg("CNTR_NET_CFG"), "xml;");
    else if(a_path == "/prm/cfg/PLCList" && ip->ctrChkNode(opt,"get"))
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("<No select>"));
	XMLNode *nDev = NULL, *nId = NULL, *nNm = NULL;
	MtxAlloc res(p->dataM, true);
	for(int i_dev = 0; (nDev=ePrm->cfg.childGet("Device",i_dev,true)); i_dev++)
	{
	    if(!(nId=nDev->childGet("Number",0,true))) continue;
	    nNm = nDev->childGet("Name",0,true);
	    opt->childAdd("el")->setAttr("id",nId->text())->setText(nNm?(nNm->text()+"("+nId->text()+")"):nId->text());
	}
    }
    else if(a_path == "/prm/cfg/CNTR_NET_CFG")
    {
	if(ip->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    opt->setText(ip->cfg("CNTR_NET_CFG"));
	    string pNm;
	    XMLNode *n = NULL;
	    MtxAlloc res(p->dataM, true);
	    if((n=ePrm->cfg.childGet("Name",0,true))) pNm = n->text();
	    if((n=ePrm->cfg.childGet("MasterDevice",0,true))) pNm += ":"+n->text();
	    if(pNm.size()) opt->setText(opt->text()+" ("+pNm+")");
	}
	if(!ip->owner().startStat() && ip->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    ip->cfg("CNTR_NET_CFG") = TSYS::strParse(opt->text(),0," ");
    }
    else return false;

    return true;
}

string Kontar::tval::RC5Encr( const string &src, const string &key )
{
    int nPad = 2;
    unsigned blocks = ((src.size() + nPad*4 - 1) / (nPad*4)) * nPad;
		//src.size()/4+((src.size()%4)?1:0);

    uint32_t cdata[blocks*2];
    for(unsigned i_s = 0; i_s < blocks; i_s++) cdata[i_s] = 0;
    memcpy(cdata, src.data(), src.size());
    if(key.size() < 20*4) return src;
    const uint32_t *keybuf = (const uint32_t *)key.data();
    int rounds = 10;

    unsigned	rc;
    uint32_t *d = cdata;
    for(unsigned h = 0; h < blocks; h++)
    {
	d[0] += keybuf[0];
	d[1] += keybuf[1];
	for(int i = 0; i < rounds*2; i += 2)
	{
	    d[0] ^= d[1];
	    rc = d[1]&31;
	    d[0] = ROTL32(d[0], rc);
	    d[0] += keybuf[i];
	    d[1] ^= d[0];
	    rc = d[0]&31;
	    d[1] = ROTL32(d[1], rc);
	    d[1] += keybuf[i+1];
	}
	d += 2;
    }

    char *ptr, *ptr1, tmp;
    for(ptr1 = ptr = (char*)cdata, rc = 0; rc < blocks*2; rc++, ptr += 4, ptr1 += 4)
    {
	tmp = *(ptr1+3);
	*(ptr1+3) = *ptr;
	*ptr = tmp;
	tmp = *(ptr1+2);
	*(ptr1+2) = *(ptr+1);
	*(ptr+1) = tmp;
    }

    return string((char*)cdata, blocks*4);
}

string Kontar::tval::RC5Decr( const string &src, const string &key )
{
    int blocks = src.size()/4 + ((src.size()%4)?1:0);
    uint32_t cdata[blocks*2];
    memcpy(cdata, src.data(), src.size());
    if(key.size() < 20*4) return src;
    const uint32_t *keybuf = (const uint32_t *)key.data();
    int rounds = 10;

    char tmp;
    union R
    {
	uint32_t l;
	char c[4];
    }R;

    uint32_t *d = cdata;
    for(int h = 0, rc; h < blocks; h++)
    {
	for(int i = rounds*2-2; i >= 0; i -= 2)
	{
	    d[1] -= keybuf[i+1];
	    rc = d[0]&31;
	    d[1] = ROTR32(d[1],rc);
	    d[1] ^= d[0];
	    d[0] -= keybuf[i];
	    rc = d[1]&31;
	    d[0] = ROTR32(d[0],rc);
	    d[0] ^= d[1];
	}
	d[0] -= keybuf[0];
	d[1] -= keybuf[1];
	R.l = d[0];
	tmp = R.c[0];
	R.c[0] = R.c[3];
	R.c[3] = tmp;
	tmp = R.c[1];
	R.c[1] = R.c[2];
	R.c[2] = tmp;
	d[0] = R.l;

	R.l = d[1];
	tmp = R.c[0];
	R.c[0] = R.c[3];
	R.c[3] = tmp;
	tmp = R.c[1];
	R.c[1] = R.c[2];
	R.c[2] = tmp;
	d[1] = R.l;
	d += 2;
    }

    return string((char*)cdata, src.size());
}

string Kontar::tval::RC5Key( const string &ikey )
{
    string key = ikey;
    key.resize(8, 0);
    uint32_t keybuf[22];
    int rounds = 10;

    uint32_t pk[2], A, B;
    uint8_t rc, xk_len, pk_len, i, num_steps;
    union K
    {
	uint32_t	l;
	char		c[4];
    }K;

    xk_len = rounds*2 + 2;
    pk_len = key.size()/4 + ((key.size()%4)?1:0);
    pk[0] = pk[1] = 0;
    K.c[3] = key[0];
    K.c[2] = key[1];
    K.c[1] = key[2];
    K.c[0] = key[3];
    pk[0] = K.l;
    K.c[3] = key[4];
    K.c[2] = key[5];
    K.c[1] = key[6];
    K.c[0] = key[7];
    pk[1] = K.l;
    for(keybuf[0] = 0xb7e15163, i = 1; i < xk_len; i++)
	keybuf[i] = keybuf[i-1]+0x9e3779b9;
    if(pk_len > xk_len) num_steps = 3*pk_len;
    else num_steps = 3*xk_len;
    for(A = B = i = 0; i < num_steps; i++)
    {
	A = keybuf[i%xk_len] = ROTL32(keybuf[i%xk_len]+A+B, 3);
	rc = (A+B)&31;
	B = pk[i%pk_len]=ROTL32(pk[i%pk_len]+A+B,rc);
    }

    return string((char*)keybuf, sizeof(keybuf));
}

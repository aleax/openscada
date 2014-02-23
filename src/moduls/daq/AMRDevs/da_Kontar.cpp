
//OpenSCADA system module DAQ.AMRDevs file: da_Kontar.cpp
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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
    fldAdd(new TFld("CNTR_NET_CFG",_("Controllers network config-file"),TFld::String,TCfg::NoVal,"100"));
    fldAdd(new TFld("PLC",_("PLC serial number"),TFld::Integer,TCfg::NoVal,"10","0"));
    fldAdd(new TFld("PASS",_("Password"),TFld::String,TCfg::NoVal,"10","12345678"));
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
		if(strcasecmp(pTp.c_str(),"int") == 0)		{ tp = TFld::Integer; tpSz = 2; }
		else if(strcasecmp(pTp.c_str(),"bool") == 0)	{ tp = TFld::Boolean; tpSz = 1; }
		else if(strcasecmp(pTp.c_str(),"time") == 0 ||
			strcasecmp(pTp.c_str(),"date") == 0)	{ tp = TFld::String; tpSz = 2; }
		if(!p->els.fldPresent(aid) || p->els.fldAt(p->els.fldId(aid)).type() != tp)
		{
		    if(p->els.fldPresent(aid)) p->els.fldDel(p->els.fldId(aid));
		    p->els.fldAdd(new TFld(aid.c_str(),"",tp,TFld::NoWrite));
		}
		int el_id = p->els.fldId(aid);
		p->els.fldAt(el_id).setDescr(pNm);
		p->els.fldAt(el_id).setReserve(pTp+":"+i2s(tpSz)+":"+nId->text());
		regVal(p, atoi(nId->text().c_str()), tpSz);
		p->als.push_back(aid);
	    }
	}
    }

    //Password to RC5-key parse
    string sPass = ip->cfg("PASS").getS(); sPass.resize(8,0);
    mess_debug_(p->nodePath().c_str(), _("Password sequence: '%s'\n"),TSYS::strDecode(sPass,TSYS::Bin).c_str());
    ePrm->rc5_key(string(8,0).data()/*sPass.data()*/, 4/*sPass.size()*/);

    //Test part for decoding
    string strBlk = TSYS::strEncode("80 40 03 b8 af af 88 0b",TSYS::Bin);
		    //TSYS::strEncode("ae f7 f9 46 2d 2e 99 c6 9c 46 ee b3 e4 37 8b 43 55 bc 50 80 73 11 30 2c",TSYS::Bin);
    mess_debug_(p->nodePath().c_str(), _("Input sequence: '%s'\n"),TSYS::strDecode(strBlk,TSYS::Bin).c_str());
    ePrm->rc5_encrypt((uint32_t*)strBlk.data(), strBlk.size()/4);
    mess_debug_(p->nodePath().c_str(), _("Encrypt result: '%s'\n"),TSYS::strDecode(strBlk,TSYS::Bin).c_str());
}

void Kontar::regVal( TMdPrm *p, int off, int sz )
{
    if(off < 0)	return;

    //> Register to acquisition block
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
	    if(p->mErr.empty())
		rez = wCnt[i_b].val.data()+(off-wCnt[i_b].off);
	    break;
	}

    return rez;
}

string Kontar::req( TMdPrm *p, string &pdu )
{
    tval *ePrm = (tval*)p->extPrms;
    string mbap, err, rez;
    char buf[1000];

    try
    {
	//Try connect to previous transport
	AutoHD<TTipTransport> ttr = SYS->transport().at().at("Sockets");
	AutoHD<TTransportOut> trO;
	if(!(ePrm->prevTr.size() && ttr.at().outPresent(ePrm->prevTr) &&
		(trO=ttr.at().outAt(ePrm->prevTr)).at().startStat() && trO.at().prm1() == p->cfg("PLC").getI()))
	    trO.free();

	//Find for proper transport by send master PLC serial number request
	if(trO.freeStat())
	{
	    vector<AutoHD<TTransportOut> > trsO = ttr.at().inAt(p->cfg("ADDR")).at().assTrs();
	    for(unsigned i_t = 0; i_t < trsO.size() && trO.freeStat(); i_t++)
	    {
		if(!trsO[i_t].at().startStat())	continue;
		// Serial number request
		if(!trsO[i_t].at().prm1())
		{
		    try
		    {
			int resp_len = 0;
			mbap = char(0);

			if(p->owner().messLev() == TMess::Debug)
			    mess_debug_(p->nodePath().c_str(), _("PLC ID Request: '%s'\n"),TSYS::strDecode(mbap,TSYS::Bin).c_str());

			if((resp_len=trsO[i_t].at().messIO(mbap.data(),mbap.size(),buf,sizeof(buf))) == 4)
			{
			    trsO[i_t].at().setPrm1(TSYS::i32_BE(*(uint32_t*)buf));
			    trsO[i_t].at().setDscr(TSYS::strMess(_("Connection from PLC Kontar %xh."),(int)TSYS::i32_BE(*(uint32_t*)buf)));
			}

			if(p->owner().messLev() == TMess::Debug)
			    mess_debug_(p->nodePath().c_str(), _("PLC ID Response: '%s'\n"),TSYS::strDecode(string(buf,resp_len),TSYS::Bin).c_str());
		    }
		    catch(...) { }
		}
		if(trsO[i_t].at().prm1() == p->cfg("PLC").getI()) trO = trsO[i_t];
	    }
	}
	if(trO.freeStat()) throw TError(p->nodePath().c_str(), _("No a propper connection from PLC."));

	//Main request prepare
	mbap.reserve(pdu.size()+17);
	mbap += string(4,0);				//???? Where the take for client
	string sPass = p->cfg("PASS").getS(); sPass.resize(8,0);
	mbap += sPass;					//Password
	mbap += (char)0;				//Unused
	uint32_t cntrN = p->cfg("PLC").getI();
	mbap += string((char*)&cntrN,sizeof(cntrN));	//PLC number
	mbap += pdu;

	ResAlloc resN(trO.at().nodeRes(), true);
	//Send request
	if(p->owner().messLev() == TMess::Debug)
	    mess_debug_(p->nodePath().c_str(), _("Request: '%s'\n"),TSYS::strDecode(mbap,TSYS::Bin).c_str());
	int resp_len = trO.at().messIO(mbap.data(), mbap.size(), buf, sizeof(buf), 0, true);
	rez.assign(buf, resp_len);
	//Wait tail
	while(resp_len)
	{
	    try{ resp_len = trO.at().messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
	    rez.append(buf, resp_len);
	}

	if(rez.size() < 5)	err = _("13:Error respond: Too short.");
	else
	{
	    if(TSYS::i32_BE(*(uint32_t*)rez.data()) != cntrN)
		err = TSYS::strMess(_("13:Unapt PLC ID: %d != %d"),TSYS::i32_BE(*(uint32_t*)rez.data()), cntrN);
	    else switch((uint8_t)rez[4])
	    {
		case 0xA0:	break;	//OK
		case 0xE0:	err = TSYS::strMess(_("12:PLC: Error."));			break;
		case 0xE1:	err = TSYS::strMess(_("12:PLC: Unknown command."));		break;
		case 0xE2:	err = TSYS::strMess(_("12:PLC: Wrong command format."));	break;
		case 0xE5:	err = TSYS::strMess(_("12:PLC: Error read from slave."));	break;
		case 0xE6:	err = TSYS::strMess(_("12:PLC: Network update in process."));	break;
		case 0xC0:	err = TSYS::strMess(_("12:PLC: The password encripted by master key."));	break;
		case 0xD0:	err = TSYS::strMess(_("12:PLC: The data encripted the access password."));	break;
	    }
	    if(err.empty())
	    {
		if(p->owner().messLev() == TMess::Debug)
		    mess_debug_(p->nodePath().c_str(), _("Response: '%s'\n"),TSYS::strDecode(rez,TSYS::Bin).c_str());
		pdu = rez.substr(5);
	    }
	}
    }catch(TError er) { err = TSYS::strMess(_("14:Connection error - %s"),er.mess.c_str()); }

    if(!err.empty() && p->owner().messLev() == TMess::Debug)
	mess_debug_(p->nodePath().c_str(), _("Error: '%s': '%s'\n"),err.c_str(), TSYS::strDecode(rez,TSYS::Bin).c_str());

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

    //Request blocks
    string pdu;
    for(unsigned i_b = 0; i_b < ePrm->mBlks.size(); i_b++)
    {
	// Encode request
	pdu = (char)0x67;				//Read memory
	pdu += (char)0x04;				//Memory, PARAMETER
	pdu += (char)0x00;
	pdu += (char)(ePrm->mBlks[i_b].off>>8);		//Address MSB
	pdu += (char)(ePrm->mBlks[i_b].off);		//Address LSB
	pdu += (char)(ePrm->mBlks[i_b].val.size()>>8);	//Number of registers MSB
	pdu += (char)(ePrm->mBlks[i_b].val.size());	//Number of registers LSB
	// Request to remote server
	ePrm->mBlks[i_b].err = req(p,pdu);
	if(ePrm->mBlks[i_b].err.empty())
	{
	    if(ePrm->mBlks[i_b].val.size() != pdu.size()) ePrm->mBlks[i_b].err = _("15:Response PDU size error.");
	    else { ePrm->mBlks[i_b].val = pdu; p->numBytes += pdu.size(); }
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
	    case TFld::Integer:	val.at().setI(dt?(int16_t)TSYS::getUnalign16(dt):EVAL_INT, 0, true);	break;
	    case TFld::Real:	val.at().setR(dt?TSYS::getUnalignFloat(dt):EVAL_REAL, 0, true);		break;
	    case TFld::String:
	    {
		string vl = EVAL_STR;
		if(dt && strcasecmp(pTp.c_str(),"time") == 0)	  vl = TSYS::strMess("%d:%d", *dt, *(dt+1));
		else if(dt && strcasecmp(pTp.c_str(),"date") == 0)vl = TSYS::strMess("%d-%d", *dt, *(dt+1));
		val.at().setS(vl, 0, true);
		break;
	    }
	}
    }
}

bool Kontar::cntrCmdProc( TParamContr *ip, XMLNode *opt )
{
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;

    // Get page info
    if(opt->name() == "info")
    {
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/ADDR",EVAL_STR,ip->owner().startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","select","select","/prm/cfg/trLst");
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/CNTR_NET_CFG",EVAL_STR,ip->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","sel_ed","select","/prm/cfg/fileList");
	ip->ctrMkNode("fld",opt,0,"/prm/cfg/PLC",EVAL_STR,ip->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"idm","1","dest","select","select","/prm/cfg/PLCList");
	return true;
    }

    // Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/trLst" && ip->ctrChkNode(opt,"get"))
    {
	try
	{
	    vector<string> sls;
	    SYS->transport().at().at("Sockets").at().inList(sls);
	    for(unsigned i_s = 0; i_s < sls.size(); i_s++) opt->childAdd("el")->setText(sls[i_s]);
	    //SYS->transport().at().at("Sockets").at().outList(sls);
	    //for(unsigned i_s = 0; i_s < sls.size(); i_s++) opt->childAdd("el")->setText("out_"+sls[i_s]);
	}
	catch(TError err) { }
    }
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

void Kontar::tval::rc5_encrypt( uint32_t *cdata, int blocks )
{
    int	rc;
    uint32_t *d = cdata;
    for(int h = 0; h < blocks; h++)
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
}

void Kontar::tval::rc5_decrypt( uint32_t *cdata, int blocks )
{
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
}

void Kontar::tval::rc5_key( const char *key, short keylen )
{
    uint32_t *cp, pk[2], A, B;
    uint8_t rc, xk_len, pk_len, i, num_steps;
    union K
    {
	uint32_t	l;
	char		c[4];
    }K;

    xk_len = rounds*2 + 2;
    pk_len = keylen/4;
    if(keylen%4) pk_len++;
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
}

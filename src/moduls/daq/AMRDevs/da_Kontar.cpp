
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
    fldAdd(new TFld("ADDR",_("Transport address"),TFld::String,TCfg::NoVal,"30"));
    fldAdd(new TFld("CNTR_NET_CFG",_("Controllers network config-file"),TFld::String,TCfg::NoVal,"100"));
    fldAdd(new TFld("PLC",_("PLC serial number"),TFld::Integer,TCfg::NoVal,"10","0"));
    fldAdd(new TFld("PASS",_("Password"),TFld::String,TCfg::NoVal,"10","12345678"));
    fldAdd(new TFld("TM_SYNC",_("Time sync (s)"),TFld::Integer,TCfg::NoVal,"10","10"));
}

Kontar::~Kontar( )
{

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

/*int64_t Kontar::getValR( int addr, ResString &err )
{
    int64_t rez = EVAL_INT;
    vector< SDataRec >	&workCnt = acqBlks;
    for(unsigned i_b = 0; i_b < workCnt.size(); i_b++)
	if((addr*2) >= workCnt[i_b].off && (addr*2+2) <= (workCnt[i_b].off+(int)workCnt[i_b].val.size()))
	{
	    err.setVal( workCnt[i_b].err.getVal() );
	    if(err.getVal().empty())
		rez = (unsigned short)(workCnt[i_b].val[addr*2-workCnt[i_b].off]<<8)|(unsigned char)workCnt[i_b].val[addr*2-workCnt[i_b].off+1];
	    break;
	}
    return rez;
}*/

/*string Kontar::req( string &pdu )
{
    string mbap, err, rez;
    char buf[1000];

    try
    {
	//> Connect to transport
	AutoHD<TTransportOut> tr = SYS->transport().at().at("Serial").at().outAt(mPrm->addr());
	if( !tr.at().startStat() ) tr.at().start();

	mbap.reserve( pdu.size()+3 );
	mbap += devAddr;		//Unit identifier
	mbap += (devAddr>>8);	//Unit identifier
	mbap += pdu;
	uint16_t crc = mod->CRC16( mbap );
	mbap += (crc>>8);
	mbap += crc;

	ResAlloc resN( tr.at().nodeRes(), true );

	//> Send request
	for( int i_tr = 0; i_tr < vmax(1,vmin(10,mPrm->owner().connTry())); i_tr++ )
	{
	    int resp_len = tr.at().messIO(mbap.data(), mbap.size(), buf, sizeof(buf), 0, true);
	    rez.assign( buf, resp_len );
	    //> Wait tail
	    while(resp_len)
	    {
		try{ resp_len = tr.at().messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
		rez.append(buf, resp_len);
	    }

	    if( rez.size() < 3 )	{ err = _("13:Error respond: Too short."); continue; }
	    if( mod->CRC16(rez.substr(0,rez.size()-2)) != (uint16_t)((rez[rez.size()-2]<<8)+(uint8_t)rez[rez.size()-1]) )
	    { err = _("13:Error respond: CRC check error."); continue; }
	    pdu = rez.substr( 2, rez.size()-4 );
	    err = "";
	    break;
	}
    }catch(TError er) { err = TSYS::strMess(_("14:Connection error - %s"),er.mess.c_str()); }

    return err;
}*/

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
    ResAlloc res(p->rData, false);
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
		p->els.fldAt(el_id).setReserve(pTp+":"+nId->text());
		regVal(p, atoi(nId->text().c_str()), tpSz);
		p->als.push_back(aid);
	    }
	}
    }
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

	ResAlloc res(p->rData, true);
	ePrm->cfg.clear();
	try { if(cf_sz) ePrm->cfg.load(string(buf,cf_sz)); } catch(...) { }
	if(buf) free(buf);
    }

    return true;
}

void Kontar::getVals( TParamContr *prm )
{
    /*string pdu;
    //> Request blocks
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
    {
	//>> Encode request PDU (Protocol Data Units)
	pdu = (char)0x3;				//Function, read multiple registers
	pdu += (char)((acqBlks[i_b].off/2)>>8);		//Address MSB
	pdu += (char)(acqBlks[i_b].off/2);		//Address LSB
	pdu += (char)((acqBlks[i_b].val.size()/2)>>8);	//Number of registers MSB
	pdu += (char)(acqBlks[i_b].val.size()/2);	//Number of registers LSB
	//>> Request to remote server
	acqBlks[i_b].err.setVal(modBusReq(pdu));
	if(acqBlks[i_b].err.getVal().empty())
	{
	    if(acqBlks[i_b].val.size() != (pdu.size()-3)) acqBlks[i_b].err.setVal(_("15:Response PDU size error."));
	    else
	    {
		acqBlks[i_b].val.replace(0,acqBlks[i_b].val.size(),pdu.data()+3,acqBlks[i_b].val.size());
		numReg += acqBlks[i_b].val.size()/2;
	    }
	}
	else if(atoi(acqBlks[i_b].err.getVal().c_str()) == 14)
	{
	    //setCntrDelay(cntr.acqBlks[i_b].err.getVal());
	    break;
	}
    }

    //> Load values to attributes
    for(unsigned i_a = 0; i_a < mPrm->p_el.fldSize(); i_a++)
    {
	AutoHD<TVal> val = mPrm->vlAt(mPrm->p_el.fldAt(i_a).name());
	int off = 0;
	string tp = TSYS::strSepParse(val.at().fld().reserve(),0,':',&off);
	int aid = strtol(TSYS::strSepParse(val.at().fld().reserve(),0,':',&off).c_str(),NULL,0);
	int vl = getValR(aid, mPrm->mErr);
	if(tp == "F")
	{
	    int vl2 = getValR(aid+1, mPrm->mErr);
	    if(vl == EVAL_INT || vl2 == EVAL_INT) val.at().setR(EVAL_REAL,0,true);
	    union { uint32_t i; float f; } wl;
	    wl.i = ((vl2&0xffff)<<16) | (vl&0xffff);
	    val.at().setR(wl.f,0,true);
	}
	else if(tp == "LI")
	{
	    int vl2 = getValR(aid+1, mPrm->mErr);
	    if(vl == EVAL_INT || vl2 == EVAL_INT) val.at().setI(EVAL_INT,0,true);
	    val.at().setI((int)(((vl2&0xffff)<<16)|(vl&0xffff)),0,true);
	}
	else val.at().setI(((vl==EVAL_INT)?EVAL_INT:(int16_t)vl), 0, true);
    }*/
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
	    for(unsigned i_s = 0; i_s < sls.size(); i_s++) opt->childAdd("el")->setText("in_"+sls[i_s]);
	    SYS->transport().at().at("Sockets").at().outList(sls);
	    for(unsigned i_s = 0; i_s < sls.size(); i_s++) opt->childAdd("el")->setText("out_"+sls[i_s]);
	}
	catch(TError err) { }
    }
    else if(a_path == "/prm/cfg/fileList" && ip->ctrChkNode(opt,"get")) TSYS::ctrListFS(opt, ip->cfg("CNTR_NET_CFG"), "xml;");
    else if(a_path == "/prm/cfg/PLCList" && ip->ctrChkNode(opt,"get"))
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("<No select>"));
	XMLNode *nDev = NULL, *nId = NULL, *nNm = NULL;
	ResAlloc res(p->rData, false);
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
	    ResAlloc res(p->rData, false);
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

void Kontar::tval::rc5_key( uint8_t *key, short keylen )
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

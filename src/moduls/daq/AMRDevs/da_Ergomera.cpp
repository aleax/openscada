
//OpenSCADA system module DAQ.AMRDevs file: da_Ergomera.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
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

#include <tsys.h>

#include "mod_amr.h"
#include "da_Ergomera.h"

using namespace AMRDevs;

//*************************************************
//* Ergomera                                      *
//*************************************************
Ergomera::Ergomera( TMdPrm *prm ) : DA(prm), numReg(0)
{
    devAddr = vmin(65535,vmax(1,atoi(mPrm->devAddr().c_str())));
    mAttrs = mPrm->extPrmGet("Attrs",true);
    mMerge = atoi(mPrm->extPrmGet("FragMerge").c_str());

    //> Parse ModBus attributes and convert to string list
    string ai, sel, atp, atp_m, atp_sub, aid, anm, awr;
    for( int ioff = 0; (sel=TSYS::strSepParse(mAttrs,0,'\n',&ioff)).size(); )
    {
	atp = TSYS::strSepParse(sel,0,':');
	if( atp.empty() ) atp = "I";
	ai  = TSYS::strSepParse(sel,1,':');
	aid = TSYS::strSepParse(sel,2,':');
	if( aid.empty() ) aid = ai;
	anm = TSYS::strSepParse(sel,3,':');
	if( anm.empty() ) anm = ai;

	if( mPrm->vlPresent(aid) && !mPrm->p_el.fldPresent(aid) ) continue;

	TFld::Type tp = TFld::Integer;
	if( atp == "F" ) tp = TFld::Real;

	if( !mPrm->p_el.fldPresent(aid) || mPrm->p_el.fldAt(mPrm->p_el.fldId(aid)).type() != tp )
	{
	    if( mPrm->p_el.fldPresent(aid) ) mPrm->p_el.fldDel(mPrm->p_el.fldId(aid));
	    mPrm->p_el.fldAdd( new TFld(aid.c_str(),"",tp,TFld::NoFlag) );
	}
	int el_id = mPrm->p_el.fldId(aid);

	unsigned flg = TFld::NoWrite;
	mPrm->p_el.fldAt(el_id).setFlg( flg );
	mPrm->p_el.fldAt(el_id).setDescr( anm );

	int reg = strtol(ai.c_str(),NULL,0);
	regVal(reg);
	if( atp == "LI" || atp == "F" ) regVal(reg+1);
	mPrm->p_el.fldAt(el_id).setReserve( atp+":"+ai );
    }
}

Ergomera::~Ergomera( )
{

}

void Ergomera::regVal( int reg )
{
    if(reg < 0)	return;

    //> Register to acquisition block
    vector< SDataRec > &workCnt = acqBlks;
    unsigned i_b;
    for(i_b = 0; i_b < workCnt.size(); i_b++)
    {
	if((reg*2) < workCnt[i_b].off)
	{
	    if((mMerge || (reg*2+2) >= workCnt[i_b].off) && (workCnt[i_b].val.size()+workCnt[i_b].off-(reg*2)) < MaxLenReq)
	    {
		workCnt[i_b].val.insert(0,workCnt[i_b].off-reg*2,0);
		workCnt[i_b].off = reg*2;
	    }
	    else workCnt.insert(workCnt.begin()+i_b,SDataRec(reg*2,2));
	}
	else if((reg*2+2) > (workCnt[i_b].off+(int)workCnt[i_b].val.size()))
	{
	    if((mMerge || reg*2 <= (workCnt[i_b].off+(int)workCnt[i_b].val.size())) && (reg*2+2-workCnt[i_b].off) < MaxLenReq)
	    {
		workCnt[i_b].val.append((reg*2+2)-(workCnt[i_b].off+workCnt[i_b].val.size()),0);
		//>> Check for allow mergin to next block
		if(!mMerge && i_b+1 < workCnt.size() && (workCnt[i_b].off+(int)workCnt[i_b].val.size()) >= workCnt[i_b+1].off)
		{
		    workCnt[i_b].val.append(workCnt[i_b+1].val,workCnt[i_b].off+workCnt[i_b].val.size()-workCnt[i_b+1].off,string::npos);
		    workCnt.erase(workCnt.begin()+i_b+1);
		}
	    }
	    else continue;
	}
	break;
    }
    if(i_b >= workCnt.size())
	workCnt.insert(workCnt.begin()+i_b,SDataRec(reg*2,2));
}

int64_t Ergomera::getValR( int addr, ResString &err )
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
}

string Ergomera::modBusReq( string &pdu )
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
}

void Ergomera::getVals( )
{
    string pdu;
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
    }
}

bool Ergomera::cntrCmdProc( XMLNode *opt )
{
    // Get page info
    if(opt->name() == "info")
    {
	mPrm->ctrMkNode("fld",opt,-1,"/prm/cfg/DEV_ADDR",mPrm->cfg("DEV_ADDR").fld().descr().c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","min","1","max","65535");
	mPrm->ctrMkNode("fld",opt,-1,"/prm/cfg/attrs",_("Attributes list"),RWRWR_,"root",SDAQ_ID,4,"tp","str","cols","100","rows","4",
	    "help",_("Attributes configuration list. List must be written by lines in format: [dt:numb:id:name]\n"
		    "Where:\n"
		    "  dt - data type: I-integer(16),LI-long integer(32),F-float;\n"
		    "  numb - register number (dec, hex or octal);\n"
		    "  id - created attribute identifier;\n"
		    "  name - created attribute name.\n"
		    "Example:\n"
		    "  'I:0x300:var:Variable' - integer access;\n"
		    "  'F:200:float:Float' - get float from registers 200 and 201;\n"
		    "  'LI:300:int32:Int32' - get int32 from registers 300 and 301."));
	mPrm->ctrMkNode("fld",opt,-1,"/prm/cfg/FragMerge",_("Registers' fragments merge"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	return true;
    }

    // Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/attrs")
    {
	if(mPrm->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(mPrm->extPrmGet("Attrs",true));
	if(mPrm->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	mPrm->extPrmSet("Attrs",opt->text(),true);
    }
    else if(a_path == "/prm/cfg/FragMerge")
    {
	if(mPrm->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(mPrm->extPrmGet("FragMerge"));
	if(mPrm->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	mPrm->extPrmSet("FragMerge",opt->text());
    }
    else return false;

    return true;
}

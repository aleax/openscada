
//OpenSCADA module DAQ.ICP_DAS file: da_LP_8x.cpp
/***************************************************************************
 *   Copyright (C) 2012-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <tsys.h>

#include "da_LP_8x.h"
#include "ICP_module.h"

extern "C"
{
#define CALLBACK
#include "lincon.h"
#include "i8014W.h"
}

using namespace ICP_DAS_DAQ;

//*************************************************
//* CPU                                           *
//*************************************************
da_LP_8x::da_LP_8x( )
{
    //Individual processing
    devs["LP-8xxx"]	= DevFeature(0);
    devs["I-8014"]	= DevFeature(0);
    devs["I-8017"]	= DevFeature(0);
    devs["I-8024"]	= DevFeature(0);
    //Unified processing
    devs["I-8037"]	= DevFeature(0, 0x0002);
    devs["I-8040"]	= DevFeature(0x0004);
    devs["I-8042"]	= DevFeature(0x0002, 0x0002);
    devs["I-8046"]	= DevFeature(0x0002);
    devs["I-8048"]	= DevFeature(0x0001);
    devs["I-8050"]	= DevFeature(0x0102, 0x0102);
    devs["I-8051"]	= DevFeature(0x0002);
    devs["I-8052"]	= DevFeature(0x0001);
    devs["I-8053"]	= DevFeature(0x0002);
    devs["I-8054"]	= DevFeature(0x0001, 0x0001);
    devs["I-8055"]	= DevFeature(0x0001, 0x0001);
    devs["I-8056"]	= DevFeature(0, 0x0002);
    devs["I-8057"]	= DevFeature(0, 0x0002);
    devs["I-8058"]	= DevFeature(0x0001);
    devs["I-8060"]	= DevFeature(0, 0x0001);
    devs["I-8063"]	= DevFeature(0x0001, 0x0001);
    devs["I-8064"]	= DevFeature(0, 0x0001);
    devs["I-8065"]	= DevFeature(0, 0x0001);
    devs["I-8066"]	= DevFeature(0, 0x0001);
    devs["I-8068"]	= DevFeature(0, 0x0001);
    devs["I-8069"]	= DevFeature(0, 0x0001);
    devs["I-8077"]	= DevFeature(0x0102, 0x0102);
}

da_LP_8x::~da_LP_8x( )
{

}

void da_LP_8x::tpList( TMdPrm *p, vector<string> &tpl, vector<string> *ntpl )
{
    //Only for LP, I-8000 parallel bus
    if(p->owner().bus() != 0)	return;
    for(map<string, DevFeature>::iterator i_d = devs.begin(); i_d != devs.end(); i_d++) {
	tpl.push_back(i_d->first);
	if(ntpl) ntpl->push_back(i_d->first);
    }
}

void da_LP_8x::enable( TMdPrm *p, vector<string> &als )
{
    string chnId, chnNm;

    if(!p->extPrms) p->extPrms = new tval();
    tval *ePrm = (tval*)p->extPrms;
    ePrm->dev = devs[p->modTp.getS()];

    if(p->modTp.getS() == "LP-8xxx") {		//Individual LP-8xxx processing
	p->pEl.fldAdd(new TFld("serial",trS("Serial number"),TFld::String,TFld::NoWrite)); als.push_back("serial");
	p->pEl.fldAdd(new TFld("SDK",trS("SDK version"),TFld::Real,TFld::NoWrite)); als.push_back("SDK");
	p->pEl.fldAdd(new TFld("DIP",trS("DIP switch"),TFld::Integer,TFld::NoWrite)); als.push_back("DIP");
	p->pEl.fldAdd(new TFld("RS",trS("Rotary switch"),TFld::Integer,TFld::NoWrite)); als.push_back("RS");
    }
    else if(p->modTp.getS() == "I-8014") {	//Individual I-8014 processing
	ePrm->prmNum = vmin(16, vmax(0,s2i(p->modPrm("cnls","8"))));
	ePrm->fastPer = s2r(p->modPrm("fastPer"));
	chnId = "dev"; p->pEl.fldAdd(new TFld(chnId.c_str(),trS("Device"),TFld::String,TFld::NoWrite)); als.push_back(chnId);
	for(int iI = 0; iI < 16; iI++) {
	    ePrm->cnlMode[iI] = s2i(p->modPrm(TSYS::strMess("cnl:%d",iI)));
	    chnId = TSYS::strMess("ai%d", iI); chnNm = TSYS::strMess(_("Input %d"), iI);
	    p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TFld::NoWrite)); als.push_back(chnId);
	}
    }
    else if(p->modTp.getS() == "I-8017") {	//Individual I-8017 processing
	ePrm->prmNum = vmin(16, vmax(0,s2i(p->modPrm("cnls","8"))));
	ePrm->fastPer = s2r(p->modPrm("fastPer"));
	for(int iI = 0; iI < 16; iI++) {
	    ePrm->cnlMode[iI] = s2i(p->modPrm(TSYS::strMess("cnl:%d",iI)));
	    chnId = TSYS::strMess("ai%d", iI); chnNm = TSYS::strMess(_("Input %d"), iI);
	    p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TFld::NoWrite)); als.push_back(chnId);
	    if(iI < 8) {
		chnId = TSYS::strMess("ha%d", iI); chnNm = TSYS::strMess(_("H/A %d"), iI);
		p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite)); als.push_back(chnId);
		chnId = TSYS::strMess("la%d", iI); chnNm = TSYS::strMess(_("L/A %d"), iI);
		p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite)); als.push_back(chnId);
	    }
	}
    }
    else if(p->modTp.getS() == "I-8024")	//> Individual I-8024 processing
	for(int i_o = 0; i_o < 8; i_o++) {
	    chnId = TSYS::strMess("ao%d",i_o); chnNm = TSYS::strMess(_("Output %d"),i_o);
	    p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TVal::DirWrite)); als.push_back(chnId);
	}
    //Other typical modules processing
    else
	// DI and DO processing
	for(unsigned i_ch = 0; i_ch < ((ePrm->dev.DI&0xFF)+(ePrm->dev.DO&0xFF)); i_ch++) {
	    //  Reverse configuration load
	    p->dInOutRev[i_ch] = s2i(p->modPrm("dIORev"+i2s(i_ch)));

	    //  Attributes create
	    if(i_ch < (ePrm->dev.DI&0xFF))
		for(int i_i = 0; i_i < 8; i_i++) {
		    chnId = TSYS::strMess("di%d_%d",i_ch,i_i); chnNm = TSYS::strMess(_("Digital input %d.%d"),i_ch,i_i);
		    p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite));
		    als.push_back(chnId);
		}
	    else
		for(int i_o = 0; i_o < 8; i_o++) {
		    chnId = TSYS::strMess("do%d_%d",i_ch-(ePrm->dev.DI&0xFF),i_o);
		    chnNm = TSYS::strMess(_("Digital out %d.%d"),i_ch-(ePrm->dev.DI&0xFF),i_o);
		    p->pEl.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite));
		    als.push_back(chnId);
		}
	}
}

void da_LP_8x::disable( TMdPrm *p )
{
    //Free module object
    if(p->extPrms) {
	delete (tval*)p->extPrms;
	p->extPrms = NULL;
    }
}

void da_LP_8x::getVal( TMdPrm *p )
{
    tval *ePrm = (tval*)p->extPrms;

    if(p->modTp.getS() == "LP-8xxx") {	//Individual LP-8xxx processing
	//Read serial number
	AutoHD<TVal> vl = p->vlAt("serial");
	if(vl.at().getS() == EVAL_STR) {
	    MtxAlloc res(p->owner().reqRes, true);
	    unsigned char sN[8];
	    Read_SN(sN);
	    res.unlock();
	    vl.at().setS(TSYS::strMess("%x%x%x%x%x%x%x%x",sN[0],sN[1],sN[2],sN[3],sN[4],sN[5],sN[6],sN[7]), 0, true);
	}

	//Read SDK version
	vl = p->vlAt("SDK");
	if(vl.at().getR() == EVAL_REAL) vl.at().setR(GetSDKversion(), 0, true);

	//Read DIP switch status
	p->owner().pBusRes.lock();
	int dpSw = (~GetDIPswitch())&0xFF;
	p->owner().pBusRes.unlock();
	p->vlAt("DIP").at().setI(dpSw, 0, true);

	//Read Rotary switch status
	int64_t val = EVAL_INT;
	int hd = open("/dev/port", O_RDONLY);
	if(hd >= 0) {
	    uint8_t rV;
	    lseek(hd, 0x300, SEEK_SET);
	    if(read(hd,&rV,1) == 1) {
		rV = (rV>>1)&0x0f;
		val = ~(((rV&0x08) >> 3) | ((rV&0x04) >> 1) | ((rV&0x02) << 1) | ((rV&0x01) << 3)) & 0x0f;
	    }
	    close(hd);
	}
	p->vlAt("RS").at().setI(val, 0, true);
    }
    else if(p->modTp.getS() == "I-8014") {	//Individual I-8014 processing
	//Check and first init for I-8014
	if(!ePrm->init)	{
	    p->owner().pBusRes.lock();
	    ePrm->init = i8014W_Init(p->modSlot) != -1;
	    // Specific parameters reading
	    if(ePrm->init) {
		int vFPGA = i8014W_GetFirmwareVer_L1(p->modSlot),
		    vLib = i8014W_GetLibVersion(),
		    vISO = i8014W_GetFirmwareVer_L2(p->modSlot),
		    jSE = i8014W_GetSingleEndJumper(p->modSlot);
		char libDate[32];
		i8014W_GetLibDate(libDate);
		if(!jSE && ePrm->prmNum > 8) ePrm->prmNum = 8;
		p->vlAt("dev").at().setS(
		    TSYS::strMess(_("Version FPGA=%04X, Library=%04X, ISO=%04X; Build=%s; Jumper SE=%d"),vFPGA,vLib,vISO,libDate,jSE),
		    0, true);
	    }
	    p->owner().pBusRes.unlock();
	}
	if(ePrm->init) {
	    //Check for I-8014 fast task (Magic, FIFO) start
	    if(ePrm->fastPer && ePrm->prmNum && !p->prcSt) SYS->taskCreate(p->nodePath('.',true), 32, fastTask, p);
	    //Get values direct
	    for(int iV = 0; iV < 16; iV++)
		if(iV >= ePrm->prmNum) p->vlAt(TSYS::strMess("ai%d",iV)).at().setR(EVAL_REAL, 0, true);
		else if(!ePrm->fastPer) {
		    MtxAlloc res(p->owner().pBusRes, true);
		    //i8014W_ReadGainOffset(p->modSlot, ePrm->cnlMode[iV], &gainValue, &offsetValue);	//Calibration
		    float fVal = 0;
		    i8014W_ReadAI(p->modSlot, 0, ePrm->cnlMode[iV], &fVal);
		    p->vlAt(TSYS::strMess("ai%d",iV)).at().setR(fVal, 0, true);
		}
	}
    }
    else if(p->modTp.getS() == "I-8017") {	//Individual I-8017 processing
	//Check for I-8017 init
	if(!ePrm->init)	{
	    p->owner().pBusRes.lock();
	    I8017_Init(p->modSlot);
	    ePrm->init = true;
	    p->owner().pBusRes.unlock();
	}
	//Check for I-8017 fast task start
	if(ePrm->fastPer && ePrm->prmNum && !p->prcSt) SYS->taskCreate(p->nodePath('.',true), 32, fastTask, p);
	//Get values direct
	for(int iV = 0; iV < 16; iV++)
	    if(iV >= ePrm->prmNum) p->vlAt(TSYS::strMess("ai%d",iV)).at().setR(EVAL_REAL,0,true);
	    else if(!ePrm->fastPer) {
		MtxAlloc res(p->owner().pBusRes, true);
		I8017_SetChannelGainMode(p->modSlot, iV, ePrm->cnlMode[iV], 0);
		p->vlAt(TSYS::strMess("ai%d",iV)).at().setR(I8017_GetCurAdChannel_Float_Cal(p->modSlot), 0, true);
	    }
    }
    //Other typical modules processing
    // DI
    else if(ePrm->dev.DI) {
	bool isErr = false;
	uint32_t val = 0;

	p->owner().pBusRes.lock();
	switch(ePrm->dev.DI>>8) {
	    case 0:     //DI_8(32)
		switch(ePrm->dev.DI&0xFF) {
		    case 1:	val = DI_8(p->modSlot);	break;
		    case 2:	val = DI_16(p->modSlot);break;
		    case 4:	val = DI_32(p->modSlot);break;
		    default:	isErr = true;
		}
		break;
	    case 1:     //DIO_DI_8(16)
		switch(ePrm->dev.DI&0xFF) {
		    case 1:	val = DIO_DI_8(p->modSlot);	break;
		    case 2:	val = DIO_DI_16(p->modSlot);	break;
		    default:	isErr = true;
		}
		break;
	    default:	isErr = true;
	}
	p->owner().pBusRes.unlock();

	for(unsigned i_ch = 0; i_ch < (ePrm->dev.DI&0xFF); i_ch++)
	    for(int i_i = 0; i_i < 8; i_i++)
		p->vlAt(TSYS::strMess("di%d_%d",i_ch,i_i)).at().setB(isErr ? EVAL_BOOL :
		    (((val>>(i_ch*8))^p->dInOutRev[i_ch])>>i_i)&1, 0, true);
    }
    // DO back read
    else if(ePrm->dev.DO) {
	bool isErr = false;

	p->owner().pBusRes.lock();
	switch(ePrm->dev.DO>>8) {
	    case 0:	//DO_8_RB(32)
		switch(ePrm->dev.DO&0xFF) {
		    case 1:	ePrm->doVal = DO_8_RB(p->modSlot);	break;
		    case 2:	ePrm->doVal = DO_16_RB(p->modSlot);	break;
		    case 4:	ePrm->doVal = DO_32_RB(p->modSlot);	break;
		    default:	isErr = true;
		}
		break;
	    case 1:	//DIO_DO_8_RB(16)
		switch(ePrm->dev.DO&0xFF) {
		    case 1:	ePrm->doVal = DIO_DO_8_RB(p->modSlot);	break;
		    case 2:	ePrm->doVal = DIO_DO_16_RB(p->modSlot);	break;
		    default:	isErr = true;
		}
		break;
	    default:    isErr = true;
	}
	p->owner().pBusRes.unlock();

	for(unsigned i_ch = 0; i_ch < (ePrm->dev.DO&0xFF); i_ch++)
	    for(int i_o = 0; i_o < 8; i_o++)
		p->vlAt(TSYS::strMess("do%d_%d",i_ch,i_o)).at().setB(isErr ? EVAL_BOOL :
		    (((ePrm->doVal>>(i_ch*8))^p->dInOutRev[(ePrm->dev.DI&0xFF)+i_ch])>>i_o)&1, 0, true);
    }
}

void da_LP_8x::vlSet( TMdPrm *p, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    tval *ePrm = (tval*)p->extPrms;

    if(p->modTp.getS() == "I-8017") {	//Individual I-8017 processing
	bool ha = (vo.name().substr(0,2) == "ha");
	bool la = (vo.name().substr(0,2) == "la");
	if(!(ha||la) || !ePrm->init) return;

	//Create previous value
	int hvl = 0, lvl = 0;
	for(int i_v = 7; i_v >= 0; i_v--) {
	    hvl = hvl << 1;
	    lvl = lvl << 1;
	    if(p->vlAt(TSYS::strMess("ha%d",i_v)).at().getB(0, true) == true) hvl |= 1;
	    if(p->vlAt(TSYS::strMess("la%d",i_v)).at().getB(0, true) == true) lvl |= 1;
	}
	p->owner().pBusRes.lock();
	I8017_SetLed(p->modSlot,(lvl<<8)|hvl);
	p->owner().pBusRes.unlock();
    }
    else if(p->modTp.getS() == "I-8024") {	//Individual I-8024 processing
	p->owner().pBusRes.lock();
	I8024_VoltageOut(p->modSlot, atoi(vo.name().c_str()+2), vl.getR());
	p->owner().pBusRes.unlock();
    }
    //Other typical modules processing
    // DO
    else if(vo.name().compare(0,2,"do") == 0 && ePrm->dev.DO) {
	uint32_t val = ePrm->doVal;
	int i_ch = 0, i_p = 0;
	if(sscanf(vo.name().c_str(),"do%d_%d",&i_ch,&i_p) != 2) return;
	if((int)vl.getB()^((p->dInOutRev[(ePrm->dev.DI&0xFF)+i_ch]>>i_p)&1))  val |= 1<<((i_ch*8)+i_p);
	else val &= ~(1<<((i_ch*8)+i_p));
	ePrm->doVal = val;
	/*for(int i_ch = (ePrm->dev.DO&0xFF)-1; i_ch >= 0; i_ch--)
	{
	    for(int i_o = 7; i_o >= 0; i_o--)
	    {
		val = val << 1;
		if(p->vlAt(TSYS::strMess("do%d_%d",i_ch,i_o)).at().getB(0, true)) val |= 1;
	    }
	    val ^= p->dInOutRev[(ePrm->dev.DI&0xFF)+i_ch];
	}*/

	p->owner().pBusRes.lock();
	switch(ePrm->dev.DO>>8) {
	    case 0:	//DO_8(32)
		switch(ePrm->dev.DO&0xFF) {
		    case 1:	DO_8(p->modSlot, val);	break;
		    case 2:	DO_16(p->modSlot, val);	break;
		    case 4:	DO_32(p->modSlot, val);	break;
		}
		break;
	    case 1:	//DIO_DO_8(16)
		switch(ePrm->dev.DO&0xFF) {
		    case 1:	DIO_DO_8(p->modSlot, val);	break;
		    case 2:	DIO_DO_16(p->modSlot, val);	break;
		}
		break;
	}
	p->owner().pBusRes.unlock();
    }

    /*else if(p->modTp.getS() == "I-8042")
    {
	bool vl = vl.getB();
	if(vl == EVAL_BOOL || vl == pvl.getB()) return;
	int chnl = atoi(vo.name().c_str()+1);

	p->owner().pBusRes.lock();
	DO_16(p->modSlot, ((vl^(p->dInOutRev[1]>>chnl))&1) ? (DO_16_RB(p->modSlot) | 0x01<<chnl) : (DO_16_RB(p->modSlot) & ~(0x01<<chnl)));
	p->owner().pBusRes.unlock();
    }*/
}

bool da_LP_8x::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    DevFeature dev = devs[p->modTp.getS()];
    tval *ePrm = (tval*)p->extPrms;

    if(opt->name() == "info") {
	if(p->modTp.getS() == "LP-8xxx") p->ctrRemoveNode(opt, "/prm/cfg/MOD_SLOT");
	//Individual I-8017 processing
	if((p->modTp.getS() == "I-8014" || p->modTp.getS() == "I-8017") && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    p->ctrMkNode("fld",opt,-1,"/cfg/prms",_("Process parameters"),RWRWR_,"root",SDAQ_ID,1, "tp","dec");
	    p->ctrMkNode("fld",opt,-1,"/cfg/fastPer",_("Fast data get period, seconds"),RWRWR_,"root",SDAQ_ID,2, "tp","real", "help",_("Use 0 for disable"));
	    if(p->ctrMkNode("area",opt,-1,"/cfg/mode",_("Mode")))
		for(int iV = 0; iV < 16; iV++)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/mode/in%d",iV).c_str(),TSYS::strMess(_("Input %d"),iV).c_str(),
			RWRWR_,"root",SDAQ_ID,4, "tp","dec", "dest","select",
			"sel_id","0;1;2;3;4",
			"sel_list",_("-10V to +10V;-5V to +5V;-2.5V to +2.5V;-1.25V to +1.25V;-20mA to +20mA (with 125 ohms resistor)"));
	}
	// DI and DO processing
	else if((dev.DI || dev.DO) && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    for(unsigned i_ch = 0; i_ch < ((dev.DI&0xFF)+(dev.DO&0xFF)); i_ch++)
		for(unsigned i_n = 0; i_n < 8; i_n++)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/nRevs%d_%d",i_ch,i_n).c_str(), (i_n==0) ?
			((i_ch < (dev.DI&0xFF)) ? TSYS::strMess(_("DI %d reverse"),i_ch).c_str() :
						  TSYS::strMess(_("DO %d reverse"),i_ch-(dev.DI&0xFF)).c_str()) : "",
			p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	}

	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(p->modTp.getS() == "I-8014" || p->modTp.getS() == "I-8017") {
	if(a_path == "/cfg/prms") {
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(ePrm?i2s(ePrm->prmNum):p->modPrm("cnls","8"));
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		p->setModPrm("cnls", ePrm ? i2s(ePrm->prmNum=s2i(opt->text())) : opt->text());
	}
	else if(a_path == "/cfg/fastPer") {
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(ePrm?r2s(ePrm->fastPer,5):p->modPrm("fastPer","0"));
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		p->setModPrm("fastPer", ePrm ? r2s(ePrm->fastPer = s2r(opt->text()),5) : opt->text());
	}
	else if(a_path.substr(0,12) == "/cfg/mode/in") {
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
		opt->setText(ePrm?i2s(ePrm->cnlMode[s2i(a_path.substr(12))]):p->modPrm("cnl:"+a_path.substr(12),"0"));
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		p->setModPrm("cnl:"+a_path.substr(12), ePrm ? i2s(ePrm->cnlMode[s2i(a_path.substr(12))]=s2i(opt->text())) : opt->text());
	}
	else return false;
    }
    //DI and DO processing
    if((dev.DI || dev.DO) && a_path.compare(0,10,"/cfg/nRevs") == 0) {
	int i_ch = 0, i_n = 0;
	sscanf(a_path.c_str(),"/cfg/nRevs%d_%d",&i_ch,&i_n);
	int chVl = s2i(p->modPrm("dIORev"+i2s(i_ch)));
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText((chVl&(1<<i_n))?"1":"0");
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    p->setModPrm("dIORev"+i2s(i_ch), i2s(s2i(opt->text()) ? (chVl|(1<<i_n)) : (chVl & ~(1<<i_n))));
    }
    else return false;

    return true;
}

void *da_LP_8x::fastTask( void *ip )
{
    TMdPrm &p = *(TMdPrm*)ip;
    tval *ePrm = (tval*)p.extPrms;
    vector< AutoHD<TVal> > cnls;

    p.endRunReq = false;
    p.prcSt = true;

    if(p.modTp.getS() == "I-8014") {	//I-8014
	//Channells prepare
	int chArr[16], gainArr[16];
	for(int iC = 0; iC < ePrm->prmNum; iC++) {
	    cnls.push_back(p.vlAt(TSYS::strMess("ai%d",iC)));
	    chArr[iC] = iC;
	    gainArr[iC] = ePrm->cnlMode[iC];
	}

	float cntOverFull = 0, cntCor = 0, acqSize = 0, curSRate = 0, retrTm;

	p.owner().pBusRes.lock();

	//Magic scan configure
	i8014W_ConfigMagicScan(p.modSlot, chArr, gainArr, cnls.size(), vmax(1,vmin(250000,1/ePrm->fastPer)), 2, 0, 0, &curSRate);
	if(p.owner().messLev() == TMess::Debug)
	    mess_debug_(p.nodePath().c_str(), "I-8014 Magic: cnls=%d; sr=%f->%f.", cnls.size(), vmax(1,vmin(250000,1/ePrm->fastPer)), curSRate);

	//Start for magic scan
	i8014W_StartMagicScan(p.modSlot);

	p.owner().pBusRes.unlock();

	//Read and place result of scanning data
	time_t	corTm = SYS->sysTm(), corDt, upStatTm = corTm, cVlTm = corTm;
	int64_t	wTm = 1000000ll*corTm, tSz;
	short	szFIFO = 4000,			//Oriented FIFO size
		rdCnt, rdCnt1, rdRez, rdData[szFIFO+16];	//Set buffer to the device's FIFO size and spare to align
	float vAI = 0;

	retrTm = vmin(1, szFIFO/(curSRate*2*cnls.size()));

	//Same scan loop
	while(!p.endRunReq) {
	    // Read available FIFO data
	    p.owner().pBusRes.lock();
	    if((rdRez=i8014W_ReadFIFO(p.modSlot,rdData,szFIFO,&rdCnt)) == FIFO_LATCHED) {
		cntOverFull++;
		i8014W_StopMagicScan(p.modSlot);
		i8014W_ReadFIFO(p.modSlot, rdData, szFIFO, &rdCnt);
		i8014W_UnLockFIFO(p.modSlot);
		i8014W_StartMagicScan(p.modSlot);
		p.owner().pBusRes.unlock();
		corTm = SYS->sysTm();
		wTm = 1000000ll*corTm;
		continue;
	    }
	    //Wait for left from the channels set align
	    else while(rdCnt%cnls.size()) {
		i8014W_ReadFIFO(p.modSlot, rdData+rdCnt, cnls.size()-(rdCnt%cnls.size()), &rdCnt1);
		if(p.owner().messLev() == TMess::Debug)
		    mess_debug_(p.nodePath().c_str(), "I-8014 Magic ReadFIFO: remnant=%d; need read=%d; readed=%d.",
			rdCnt%cnls.size(), cnls.size()-(rdCnt%cnls.size()), rdCnt1);
		rdCnt += rdCnt1;
	    }
	    p.owner().pBusRes.unlock();

	    acqSize += (float)(rdCnt*2)/1048576;
	    tSz = 1000000ll*(rdCnt/cnls.size())/(int64_t)curSRate;

	    if(rdCnt > 0 && p.owner().startStat()) {
		bool cVlSet = (SYS->sysTm() != cVlTm);
		for(unsigned iC = 0; iC < cnls.size(); iC++) {
		    // Place the readed data to OpenSCADA buffers
		    AutoHD<TVArchive> arch;
		    if(!cnls[iC].at().arch().freeStat() && (arch=cnls[iC].at().arch()).at().srcMode() == TVArchive::PassiveAttr)
			for(int64_t iT = 0; iT < tSz; iT += arch.at().period()) {
			    //i8014W_CalibrateData(p.modSlot, gainArr[iC], rdData[((rdCnt/cnls.size())*iT/tSz)*cnls.size()+iC], &vAI);
			    vAI = (10.0/(gainArr[iC]?2*gainArr[iC]:1))*(float)rdData[((rdCnt/cnls.size())*iT/tSz)*cnls.size()+iC]/32670; //Better up to 1.5%
			    arch.at().setR(vAI, wTm+iT);
			}
		    // Place the read data last value to current, may be check for period acquisition task and blocks repeat
		    if(cVlSet) {
			//i8014W_CalibrateData(p.modSlot, gainArr[iC], rdData[(rdCnt/cnls.size()-1)*cnls.size()+iC], &vAI);
			vAI = (10.0/(gainArr[iC]?2*gainArr[iC]:1))*(float)rdData[(rdCnt/cnls.size()-1)*cnls.size()+iC]/32670;
			cnls[iC].at().setR(vAI, wTm+(1000000ll*(rdCnt/cnls.size()-1))/(int64_t)curSRate, true);
		    }
		}
		if(cVlSet) cVlTm = SYS->sysTm();
	    }

	    wTm += tSz;

	    // Check for samplerate correction
	    if(abs(corDt=(SYS->sysTm()-wTm/1000000)) >= 3) {
		corDt -= (corDt>=0?2:-2);
		cntCor++;
		if(abs(corDt) < 10) {
		    if(p.owner().messLev() == TMess::Debug)
			mess_debug_(p.nodePath().c_str(), "I-8014 Magic: Sample rate correction from %g to %g for fix error %ds on time %ds.",
			    curSRate, curSRate/vmax(0.9,vmin(1.1,((float)corDt/(SYS->sysTm()-corTm)+1))), corDt, SYS->sysTm()-corTm);
		    curSRate = curSRate / vmax(0.9,vmin(1.1,((float)corDt/(SYS->sysTm()-corTm)+1)));
		}
		else if(p.owner().messLev() == TMess::Debug)
		    mess_debug_(p.nodePath().c_str(), "I-8014 Magic: Reset current time by the system time correction to %ds possible.", corDt);
		corTm = SYS->sysTm();
		wTm = 1000000ll*corTm;
	    }

	    if((SYS->sysTm()-upStatTm) > 10) {
		p.acqErr.setVal(TSYS::strMess(_("0:FIFO gather: %.2gMB. Sample rate: %g(%g cor.). Overfulls: %g. Retry time: %gs."),
		    acqSize,curSRate,cntCor,cntOverFull,retrTm));
		upStatTm = SYS->sysTm();
	    }

	    //Repeate after about half-size FIFO filling
	    if(rdCnt) retrTm = vmin(1, vmax(0,retrTm+0.1*(szFIFO/2-rdCnt)/(curSRate*cnls.size())));

	    TSYS::sysSleep(retrTm);
	}

	i8014W_StopMagicScan(p.modSlot);
    }
    else {				//I-8017
	//struct timespec sp_tm;
	int64_t	wTm = 1000000ll*SYS->sysTm(); //TSYS::curTime();

	int c_mode;

	for(int i_c = 0; i_c < ePrm->prmNum; i_c++)
	    cnls.push_back(p.vlAt(TSYS::strMess("ai%d",i_c)));

	float vbuf[cnls.size()];

	while(!p.endRunReq) {
	    p.owner().pBusRes.lock();
	    for(unsigned i_c = 0; p.owner().startStat() && i_c < cnls.size(); i_c++) {
		c_mode = ePrm->cnlMode[i_c];
		I8017_SetChannelGainMode(p.modSlot, i_c, c_mode, 0);
		vbuf[i_c] = (10.0/(c_mode?2*c_mode:1))*(float)I8017_GetCurAdChannel_Hex(p.modSlot)/8000;
	    }
	    p.owner().pBusRes.unlock();

	    for(unsigned i_c = 0; p.owner().startStat() && i_c < cnls.size(); i_c++)
		cnls[i_c].at().setR(vbuf[i_c], wTm, true);

	    // Check for more cycles loss
	    if(abs(SYS->sysTm()-wTm/1000000) >= 3) wTm = 1000000ll*SYS->sysTm();

	    //Calc next work time and sleep
	    wTm += 1000000ll*ePrm->fastPer;
	    TSYS::taskSleep(int64_t(1e9*ePrm->fastPer));	//To prevent hang at wTm more lesser to current
	    //sp_tm.tv_sec = wTm/1000000; sp_tm.tv_nsec = 1000*(wTm%1000000);
	    //clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sp_tm, NULL);
	}
    }

    p.prcSt = false;

    return NULL;
}

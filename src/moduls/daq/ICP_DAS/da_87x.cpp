
//OpenSCADA system module DAQ.ICP_DAS file: da_87x.cpp
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <tsys.h>

#include "da_87x.h"
#include "ICP_module.h"

using namespace ICP_DAS_DAQ;

//*************************************************
//* da_87x                                        *
//*************************************************
da_87x::da_87x( )
{
    devs["I-87xxx"]	= DevFeature( 0 );
    devs["I-87005"]	= DevFeature( 0x0008,	0,	0,	0x0101 );
    devs["I-87013"]	= DevFeature( 0x0004 );
    devs["I-87015"]	= DevFeature( 0x0007 );
    devs["I-87016"]	= DevFeature( 0x0002 );
    devs["I-87017"]	= DevFeature( 0x0008 );
    devs["I-87017DW"]	= DevFeature( 0x0010 );
    devs["I-87017ZW"]	= DevFeature( 0x0014 );
    devs["I-87017ZW"].aiTypes = string("7;8;9;10;11;12;13;26\n")+
	_("4mA to 20mA;-10V to 10V;-5V to 5V;-1V to 1V;-500mV to 500mV;-150mV to 150mV;-20mA to 20mA;0mA to 20mA;");
    devs["I-87018"]	= DevFeature( 0x0108 );
    devs["I-87018ZW"]	= DevFeature( 0x010A );
    devs["I-87019RW"]	= DevFeature( 0x0108 );
    devs["I-87019RW"].aiTypes = string("0;1;2;3;4;5;6;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;25\n")+
	_("-15mV to +15mV;-50mV to +50mV;-100mV to +100mV;-500mV to +500mV;-1V to +1V;-2.5V to +2.5V;"
	  "-20mA to +20mA (with 125 ohms resistor);-10V to +10V;-5V to +5V;-1V to +1V;-500mV to +500mV;"
	  "-150mV to +150mV;-20mA to +20mA (with 125 ohms resistor);J Type;K Type;T Type;E Type;R Type;"
	  "S Type;B Type;N Type;C Type;L Type;M Type;L Type (DIN43710C Type)");
    devs["I-87019ZW"]	= DevFeature( 0x010A);
    devs["I-87022"]	= DevFeature( 0,	2 );
    devs["I-87024"]	= DevFeature( 0,	4 );
    devs["I-87026"]	= DevFeature( 0,	2 );
    devs["I-87026PW"]	= DevFeature( 0x0006,	2,	0x0101,	0x0101 );
    devs["I-87028"]	= DevFeature( 0,	8 );
    devs["I-87037"]	= DevFeature( 0,	0,	0,	0x0002 );
    devs["I-87040"]	= DevFeature( 0,	0,	0x0004,	0,	32 );
    devs["I-87041"]	= DevFeature( 0,	0,	0,	0x0004 );
    devs["I-87042"]	= DevFeature( 0,	0,	0x0002,	0x0002 );
    devs["I-87046"]	= DevFeature( 0,	0,	0x0002,	0,	16 );
    devs["I-87051"]	= DevFeature( 0,	0,	0x0002,	0,	16 );
    devs["I-87052"]	= DevFeature( 0,	0,	0x0001,	0,	8 );
    devs["I-87053"]	= DevFeature( 0,	0,	0x0002,	0,	16 );
    devs["I-87054"]	= DevFeature( 0,	0,	0x0001,	0x0001,	8 );
    devs["I-87055"]	= DevFeature( 0,	0,	0x0001,	0x0001,	8 );
    devs["I-87057"]	= DevFeature( 0,	0,	0,	0x0002 );
    devs["I-87058"]	= DevFeature( 0,	0,	0x0001,	0,	8 );
    devs["I-87059"]	= DevFeature( 0,	0,	0x0001,	0,	8 );
    devs["I-87061"]	= DevFeature( 0,	0,	0,	0x0002 );
    devs["I-87063"]	= DevFeature( 0,	0,	0x0002,	0x0002,	16 );
    devs["I-87064"]	= DevFeature( 0,	0,	0,	0x0001 );
    devs["I-87065"]	= DevFeature( 0,	0,	0,	0x0001 );
    devs["I-87066"]	= DevFeature( 0,	0,	0,	0x0001 );
    devs["I-87068"]	= DevFeature( 0,	0,	0,	0x0001 );
    devs["I-87069"]	= DevFeature( 0,	0,	0,	0x0001 );
}

da_87x::~da_87x( )
{

}

void da_87x::tpList( TMdPrm *p, vector<string> &tpl, vector<string> *ntpl )
{
    //Only for I-8700 and I-7000 serial bus
    if(p->owner().bus() < 0)	return;
    for(map<string, DevFeature>::iterator i_d = devs.begin(); i_d != devs.end(); i_d++) {
	tpl.push_back(i_d->first);
	if(ntpl) ntpl->push_back(i_d->first);
    }
}

void da_87x::enable( TMdPrm *p, vector<string> &als )
{
    string chnId, chnNm;

    if(!p->extPrms) p->extPrms = new tval();
    tval *ePrm = (tval*)p->extPrms;
    ePrm->dev = getDev(p, p->modTp.getS());

    /*chnId = "modInfo"; chnNm = _("Module information");
    p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::String,TFld::NoWrite)); als.push_back(chnId);*/

    //AI processing
    if(ePrm->dev.AI) {
	if((ePrm->dev.AI>>8) == 1) {
	    chnId = "cvct"; chnNm = _("Cold-Junction Compensation(CJC) temperature");
	    p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TFld::NoWrite)); als.push_back(chnId);
	}
	for(int i_i = 0; i_i < (ePrm->dev.AI&0xFF); i_i++) {
	    chnId = TSYS::strMess("ai%d",i_i); chnNm = TSYS::strMess(_("Input %d"),i_i);
	    p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TFld::NoWrite)); als.push_back(chnId);
	    //p->p_el.fldAdd(new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite));
	    //als.push_back(TSYS::strMess("ha%d",i_i));
	    //p->p_el.fldAdd(new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite));
	    //als.push_back(TSYS::strMess("la%d",i_i));
	}
    }

    //AO processing
    for(unsigned i_a = 0; i_a < ePrm->dev.AO; i_a++) {
	chnId = TSYS::strMess("ao%d",i_a); chnNm = TSYS::strMess(_("Out %d"),i_a);
	p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TVal::DirWrite)); als.push_back(chnId);
    }

    //DI and DO processing
    for(unsigned i_ch = 0; i_ch < ((ePrm->dev.DI&0xFF)+(ePrm->dev.DO&0xFF)); i_ch++) {
	// Reverse configuration load
	p->dInOutRev[i_ch] = s2i(p->modPrm("dIORev"+i2s(i_ch)));

	// Attributes create
	if(i_ch < (ePrm->dev.DI&0xFF))
	    for(int i_i = 0; i_i < 8; i_i++) {
		chnId = TSYS::strMess("di%d_%d",i_ch,i_i); chnNm = TSYS::strMess(_("Digital input %d.%d"),i_ch,i_i);
		p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite)); als.push_back(chnId);
	    }
	else
	    for(int i_o = 0; i_o < 8; i_o++) {
		chnId = TSYS::strMess("do%d_%d",i_ch-(ePrm->dev.DI&0xFF),i_o);
		chnNm = TSYS::strMess(_("Digital out %d.%d"),i_ch-(ePrm->dev.DI&0xFF),i_o);
		p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite)); als.push_back(chnId);
	    }
    }

    //Counters processing
    for(unsigned i_c = 0; i_c < ePrm->dev.CNTR; i_c++) {
	chnId = TSYS::strMess("cntr%d",i_c); chnNm = TSYS::strMess(_("Counter %d"),i_c);
	p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TFld::NoWrite)); als.push_back(chnId);
    }

    //Watchdog timeout read
    p->wTm = s2r(p->modPrm("wTm","0"));
}

void da_87x::disable( TMdPrm *p )
{
    if(p->extPrms) {
	delete (tval *)p->extPrms;
	p->extPrms = NULL;
    }
}

void da_87x::getVal( TMdPrm *p )
{
    tval *ePrm = (tval*)p->extPrms;
    string rez = "1", tstr;
    bool CRC = s2i(p->modPrm("CRC","0"));

    //Get module info ($AAM, $AAF)
    /*if((tstr=p->vlAt("modInfo").at().getS(0, true)).empty() || tstr == EVAL_STR)
    {
	if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("~%02XM",(p->owner().bus()==0)?0:p->modAddr), p->modSlot, CRC);
	if(!rez.empty() && rez.size() > 3 && rez[0] == '!') tstr = rez.substr(3);
	if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("~%02XF",(p->owner().bus()==0)?0:p->modAddr), p->modSlot, CRC);
	if(!rez.empty() && rez.size() > 3) tstr += "("+rez.substr(3)+")";
	p->vlAt("modInfo").at().setS(rez.empty() ? EVAL_STR : tstr.c_str(), 0, true);
    }*/

    //Host watchdog processing (~AA3ETT)
    if((ePrm->dev.AO || (ePrm->dev.DO && (ePrm->dev.DO>>8)==0)) && p->wTm >= 0.1 && !rez.empty())
	rez = p->owner().serReq(TSYS::strMess("~%02X31%02X",(int)((p->owner().bus()==0)?0:p->modAddr),(int)(10*p->wTm)), p->modSlot, CRC);

    //AO back processing ($AA8N)
    for(unsigned i_v = 0; i_v < ePrm->dev.AO; i_v++) {
	if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("$%02X8%d",(int)((p->owner().bus()==0)?0:p->modAddr),i_v), p->modSlot, CRC);
	p->vlAt(TSYS::strMess("ao%d",i_v)).at().setR((rez.size() != 10 || rez[0]!='!') ? EVAL_REAL : atof(rez.data()+3), 0, true);
    }

    //AI processing
    if(ePrm->dev.AI) {
	// #AA
	if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("#%02X",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	for(unsigned i_a = 0; i_a < (ePrm->dev.AI&0xFF); i_a++)
	    p->vlAt(TSYS::strMess("ai%d",i_a)).at().setR(((1+(i_a+1)*7) > rez.size() || rez[0] != '>') ? EVAL_REAL : atof(rez.data()+1+7*i_a), 0, true);

	// $AA3
	if((ePrm->dev.AI>>8) == 1) {
	    if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("$%02X3",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	    p->vlAt("cvct").at().setR((rez.size() != 8 || rez[0] != '>') ? EVAL_REAL : atof(rez.data()+1), 0, true);
	}
    }

    //DI and DO back processing processing
    if(ePrm->dev.DI || ePrm->dev.DO) {
	if(!rez.empty() && (ePrm->dev.DI>>8) == 0)	//@AA
	    rez = p->owner().serReq(TSYS::strMess("@%02X",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	else if(!rez.empty() && (ePrm->dev.DI>>8) == 1)	//@AADI
	    rez = p->owner().serReq(TSYS::strMess("@%02XDI",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	else rez = "";

	// DO
	unsigned DOsz = (ePrm->dev.DO && (ePrm->dev.DO>>8) == 0) ? ePrm->dev.DO&0xFF : 0;
	bool isErr = ((1+DOsz*2) > rez.size() || rez[0] != '>');
	ePrm->doVal = isErr ? 0 : strtoul(rez.substr(1,DOsz*2).c_str(),NULL,16);
	for(unsigned i_ch = 0; i_ch < DOsz; i_ch++)
	    for(unsigned i_d = 0; i_d < 8; i_d++)
		p->vlAt(TSYS::strMess("do%d_%d",i_ch,i_d)).at().setB(isErr ? EVAL_BOOL :
		    (((ePrm->doVal>>(i_ch*8))^p->dInOutRev[(ePrm->dev.DI&0xFF)+i_ch])>>i_d)&1, 0, true);

	// DI
	unsigned DIsz = (ePrm->dev.DI && (ePrm->dev.DI>>8) == 0) ? ePrm->dev.DI&0xFF : 0;
	isErr = ((1+(DOsz+DIsz)*2) > rez.size() || rez[0] != '>');
	uint32_t diVal = isErr ? 0 : strtoul(rez.substr(1+DOsz*2,DIsz*2).c_str(),NULL,16);
	for(unsigned i_ch = 0; i_ch < DIsz; i_ch++)
	    for(unsigned i_d = 0; i_d < 8; i_d++)
		p->vlAt(TSYS::strMess("di%d_%d",i_ch,i_d)).at().setB(isErr ? EVAL_BOOL :
		    (((diVal>>(i_ch*8))^p->dInOutRev[i_ch])>>i_d)&1, 0, true);
    }

    //Counters processing (#AAN)
    for(unsigned i_c = 0; i_c < ePrm->dev.CNTR; i_c++) {
	if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("#%02X%d",(int)((p->owner().bus()==0)?0:p->modAddr),i_c), p->modSlot, CRC);
	p->vlAt(TSYS::strMess("cntr%d",i_c)).at().setR((rez.size() != 8 || rez[0] != '!') ? EVAL_INT : atoi(rez.data()+3), 0, true);
    }

    p->acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
}

void da_87x::vlSet( TMdPrm *p, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    string rez;
    tval *ePrm = (tval*)p->extPrms;

    if(vl.isEVal() || vl == pvl) return;

    bool CRC = s2i(p->modPrm("CRC","0"));

    /*
    if(p->modTp.getS() == "I-87019")
    {
	bool ha = (vo.name().substr(0,2) == "ha");
	bool la = (vo.name().substr(0,2) == "la");
	if(!(ha||la)) return;

	//> Create previous value
	int hvl = 0, lvl = 0;
	for(int i_v = 7; i_v >= 0; i_v--)
	{
	    hvl = hvl << 1;
	    lvl = lvl << 1;
	    if(p->vlAt(TSYS::strMess("ha%d",i_v)).at().getB() == true)	hvl |= 1;
	    if(p->vlAt(TSYS::strMess("la%d",i_v)).at().getB() == true)	lvl |= 1;
	}

	rez = p->owner().serReq(TSYS::strMess("@%02XL%02X%02X",(p->owner().bus()==0)?0:p->modAddr,lvl,hvl), p->modSlot, CRC);
	p->acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
    }*/

    //AO processing, #AAN(Data)
    if(vo.name().compare(0,2,"ao") == 0 && ePrm->dev.AO) {
	string cmd = TSYS::strMess("#%02X%d%+07.3f",(int)((p->owner().bus()==0)?0:p->modAddr),atoi(vo.name().c_str()+2),vl.getR());

	repAO:
	rez = p->owner().serReq(cmd, p->modSlot, CRC);
	// Set watchdog flag is process
	if(!rez.empty() && rez[0] == '!') {
	    p->owner().serReq(TSYS::strMess("~%02X1",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	    goto repAO;
	}
	vo.setR((rez.empty() || rez[0] != '>') ? EVAL_REAL : vl.getR(), 0, true);
	p->acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
    }

    //DO processing
    if(vo.name().compare(0,2,"do") == 0 && ePrm->dev.DO) {
	uint32_t tvl = ePrm->doVal;
	int i_ch = 0, i_p = 0;
	if(sscanf(vo.name().c_str(),"do%d_%d",&i_ch,&i_p) != 2) return;
	if((int)vl.getB()^((p->dInOutRev[(ePrm->dev.DI&0xFF)+i_ch]>>i_p)&1))	tvl |= 1<<((i_ch*8)+i_p);
	else tvl &= ~(1<<((i_ch*8)+i_p));
	/*for(int i_ch = (ePrm->dev.DO&0xFF)-1; i_ch >= 0; i_ch--)
	{
	    for(int i_o = 7; i_o >= 0; i_o--)
	    {
		tvl = tvl << 1;
		if(p->vlAt(TSYS::strMess("do%d_%d",i_ch,i_o)).at().getB(0, true)) tvl |= 1;
	    }
	    tvl ^= p->dInOutRev[(ePrm->dev.DI&0xFF)+i_ch];
	}*/

	string cmd;
	if((ePrm->dev.DO>>8) == 0)
	    cmd = TSYS::strMess(TSYS::strMess("@%%02X%%0%dX",(ePrm->dev.DO&0xFF)*2).c_str(),(int)((p->owner().bus()==0)?0:p->modAddr),tvl);
	else if((ePrm->dev.DO>>8) == 1)
	    cmd = TSYS::strMess(TSYS::strMess("@%%02XDO%%0%dX",(ePrm->dev.DO&0xFF)*2).c_str(),(int)((p->owner().bus()==0)?0:p->modAddr),tvl);

	repDO:
	rez = p->owner().serReq(cmd, p->modSlot, CRC);
	// Set watchdog flag is process
	if((ePrm->dev.DO>>8) == 0 && !rez.empty() && rez[0] == '!') {
	    p->owner().serReq(TSYS::strMess("~%02X1",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	    goto repDO;
	}

	vo.setB((rez.empty() || rez[0] != '>') ? EVAL_BOOL : tvl, 0, true);
	if(rez.size() || rez[0] == '>') ePrm->doVal = tvl;
	p->acq_err.setVal(rez.empty()?_("10:Request to module error."):((rez[0]!='>')?_("11:Respond from module error."):""));
    }
}

da_87x::DevFeature da_87x::getDev( TMdPrm *p, const string &nm )
{
    int cntrSet = 0;
    DevFeature wdev = devs[nm];
    if(nm == "I-87xxx") {
	wdev.AI = (s2i(p->modPrm("modAItp","0"))<<8) | s2i(p->modPrm("modAI","0"));
	wdev.AO = s2i(p->modPrm("modAO","0"));
	wdev.DI = (s2i(p->modPrm("modDItp","0"))<<8) | s2i(p->modPrm("modDI","0"));
	wdev.DO = (s2i(p->modPrm("modDOtp","0"))<<8) | s2i(p->modPrm("modDO","0"));
	wdev.CNTR = s2i(p->modPrm("modCNTR","0"));
    }
    else {
	if(wdev.AO && (cntrSet=s2i(p->modPrm("modAO","-1"))) >= 0)	wdev.AO = vmin(cntrSet, wdev.AO);
	if(wdev.CNTR && (cntrSet=s2i(p->modPrm("modCNTR","-1"))) >= 0)	wdev.CNTR = vmin(cntrSet, wdev.CNTR);
    }

    return wdev;
}

bool da_87x::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    string rez;
    DevFeature devOrig = devs[p->modTp.getS()];
    DevFeature dev = getDev(p, p->modTp.getS());
    tval *ePrm = (tval*)p->extPrms;
    bool CRC = s2i(p->modPrm("CRC","0"));

    if(opt->name() == "info") {
	p->ctrMkNode("fld",opt,-1,"/prm/cfg/modCRC",_("CRC control"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	//Generic "I-87xxx" and AI CNTR channels processing limit set configuration
	if(p->modTp.getS() == "I-87xxx") {
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modAI",_("AI number"),p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
		"tp","dec","min","-1","max","32");
	    if(!devOrig.AI) p->ctrMkNode("fld",opt,-1,"/prm/cfg/modAItp","",p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
		"tp","dec","dest","select","sel_id","0;1","sel_list","#AA;$AA3,#AA");
	}
	if(p->modTp.getS() == "I-87xxx" || devOrig.AO)
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modAO",_("AO number, #AAN(Data)"),p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
		"tp","dec","min","-1","max",devOrig.AO?i2s(devOrig.AO).c_str():"10");
	if(p->modTp.getS() == "I-87xxx") {
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modDI",_("DI number (ports x8)"),p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
		"tp","dec","min","-1","max","4");
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modDItp","",p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
		"tp","dec","dest","select","sel_id","0;1","sel_list","@AA;@AADI");
	}
	if(p->modTp.getS() == "I-87xxx") {
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modDO",_("DO number (ports x8)"),p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
		"tp","dec","min","-1","max","4");
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modDOtp","",p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
		"tp","dec","dest","select","sel_id","0;1","sel_list","@AA(Data);@AADODD");
	}
	if(p->modTp.getS() == "I-87xxx" || devOrig.CNTR)
	    p->ctrMkNode("fld",opt,-1,"/prm/cfg/modCNTR",_("Counters number, #AAN"),p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
		"tp","dec","min","-1","max",devOrig.CNTR?i2s(devOrig.CNTR).c_str():"32");

	//AI processing
	if(ePrm && ePrm->dev.AI && p->owner().startStat() && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	    for(int i_v = 0; i_v < (ePrm->dev.AI&0xFF); i_v++) {
		XMLNode *tnd = p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/inTp%d",i_v).c_str(),TSYS::strMess(_("Input %d type"),i_v).c_str(),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		if(ePrm->dev.aiTypes.size()) tnd->setAttr("dest","select")->
		    setAttr("sel_id","-1;"+TSYS::strParse(ePrm->dev.aiTypes,0,"\n"))->
		    setAttr("sel_list",_("Error;")+TSYS::strParse(ePrm->dev.aiTypes,1,"\n"));
	    }
	//AO and watchdog processing
	if(ePrm && ePrm->dev.AO && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    p->ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,3,"tp","real","min","0","max","25.5");
	    if(p->owner().startStat() && p->ctrMkNode("area",opt,-1,"/cfg/mod",_("Module"))) {
		p->ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		p->ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		p->ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from current"),RWRW__,"root",SDAQ_ID);
		p->ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		p->ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from current"),RWRW__,"root",SDAQ_ID);
	    }
	}
	//DI and DO processing
	if((dev.DI || dev.DO) && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    for(unsigned i_ch = 0; i_ch < ((dev.DI&0xFF)+(dev.DO&0xFF)); i_ch++)
		for(unsigned i_n = 0; i_n < 8; i_n++)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/nRevs%d_%d",i_ch,i_n).c_str(), (i_n==0) ?
			((i_ch < (dev.DI&0xFF)) ? TSYS::strMess(_("DI %d reverse"),i_ch).c_str() :
						  TSYS::strMess(_("DO %d reverse"),i_ch-(dev.DI&0xFF)).c_str()) : "",
			p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	}
	//DO watchdog processing
	if(ePrm && ePrm->dev.DO && (ePrm->dev.DO>>8) == 0 && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    p->ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,3,"tp","real","min","0","max","25.5");
	    if(p->owner().startStat() && p->ctrMkNode("area",opt,-1,"/cfg/mod",_("Module"))) {
		p->ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		p->ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		p->ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from current"),RWRW__,"root",SDAQ_ID);
		p->ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		p->ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from current"),RWRW__,"root",SDAQ_ID);
	    }
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    // Generic "I-87xxx" and AI CNTR channels processing limit set configuration
    if(a_path == "/prm/cfg/modCRC") {
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(p->modPrm("CRC","0"));
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	p->setModPrm("CRC",opt->text());
    }
    else if(a_path.compare(0,12,"/prm/cfg/mod") == 0) {
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText(p->modPrm(a_path.substr(9),(p->modTp.getS()=="I-87xxx")?"0":"-1"));
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	p->setModPrm(a_path.substr(9),opt->text());
    }
    // AI processing
    else if(ePrm && ePrm->dev.AI && p->owner().startStat() && a_path.compare(0,9,"/cfg/inTp") == 0) {
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {	//$AA8Ci
	    rez = p->owner().serReq(TSYS::strMess("$%02X8C%01X",(int)((p->owner().bus()==0)?0:p->modAddr),s2i(a_path.substr(9))), p->modSlot, CRC);
	    opt->setText((rez.size()!=8||rez[0]!='!') ? "-1" : i2s(strtol(rez.data()+6,NULL,16)));
	}
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	//$AA7CiRrr
	    p->owner().serReq(TSYS::strMess("$%02X7C%dR%02X",(int)((p->owner().bus()==0)?0:p->modAddr),s2i(a_path.substr(9)),s2i(opt->text())), p->modSlot, CRC);
    }
    // AO and watchdog processing
    else if(ePrm && ePrm->dev.AO) {
	if(a_path == "/cfg/wTm") {
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(r2s(p->wTm));
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))  p->setModPrm("wTm",r2s(p->wTm = s2r(opt->text())));
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/wSt" && p->ctrChkNode(opt)) {
	    string wSt;

	    //  ~AA0
	    rez = p->owner().serReq(TSYS::strMess("~%02X0",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	    if(rez.size() == 5 && rez[0] == '!') {
		wSt += (bool)strtol(rez.data()+3,NULL,16) ? _("Set. ") : _("Clear. ");

		//  ~AA2
		rez = p->owner().serReq(TSYS::strMess("~%02X2",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
		if(rez.size() == 6 && rez[0] == '!') {
		    wSt += (bool)strtol(string(rez.data()+3,1).c_str(),NULL,16) ? _("Enabled, ") : _("Disabled, ");
		    wSt += r2s(0.1*strtol(rez.data()+4,NULL,16))+_(" s.");
		}
	    }
	    opt->setText(wSt);
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vPon" && p->ctrChkNode(opt)) {
	    string cnt;
	    for(unsigned i_c = 0; i_c < dev.AO; i_c++) {
		//  $AA7N
		rez = p->owner().serReq(TSYS::strMess("$%02X7%d",(int)((p->owner().bus()==0)?0:p->modAddr),i_c), p->modSlot, CRC);
		if(rez.size() != 10 || rez[0] != '!') { cnt = _("Error"); break; }
		cnt = cnt + (rez.data()+3) + " ";
	    }
	    opt->setText(cnt);
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vPonSet" && p->ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	    for(unsigned i_c = 0; i_c < dev.AO; i_c++)	// $AA4N
		p->owner().serReq(TSYS::strMess("$%02X4%d",(int)((p->owner().bus()==0)?0:p->modAddr),i_c), p->modSlot, CRC);
	else if(p->owner().startStat() && a_path == "/cfg/mod/vSf" && p->ctrChkNode(opt)) {
	    string cnt;
	    for(unsigned i_c = 0; i_c < dev.AO; i_c++) {
		//  ~AA4N
		rez = p->owner().serReq(TSYS::strMess("~%02X4%d",(int)((p->owner().bus()==0)?0:p->modAddr),i_c), p->modSlot, CRC);
		if(rez.size() != 10 || rez[0] != '!') { cnt = _("Error"); break; }
		cnt = cnt + (rez.data()+3) + " ";
	    }
	    opt->setText(cnt);
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vSfSet" && p->ctrChkNode(opt,"seet",RWRW__,"root",SDAQ_ID,SEC_WR))
	    for(unsigned i_c = 0; i_c < dev.AO; i_c++)	// ~AA5N
		p->owner().serReq(TSYS::strMess("~%02X5%d",(int)((p->owner().bus()==0)?0:p->modAddr),i_c), p->modSlot, CRC);
	else return false;
    }
    // DI and DO reverse processing
    else if(dev.DI || dev.DO) {
	if(a_path.compare(0,10,"/cfg/nRevs") == 0) {
	    int i_ch = 0, i_n = 0;
	    sscanf(a_path.c_str(),"/cfg/nRevs%d_%d",&i_ch,&i_n);
	    int chVl = s2i(p->modPrm("dIORev"+i2s(i_ch)));
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText((chVl&(1<<i_n))?"1":"0");
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		p->setModPrm("dIORev"+i2s(i_ch), i2s(s2i(opt->text()) ? (chVl|(1<<i_n)) : (chVl & ~(1<<i_n))));
	}
	//  DO and watchdog processing
	else if(ePrm && ePrm->dev.DO && (ePrm->dev.DO>>8) == 0) {
	    if(a_path == "/cfg/wTm") {
		if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(r2s(p->wTm));
		if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) p->setModPrm("wTm",r2s(p->wTm = s2r(opt->text())));
	    }
	    else if(p->owner().startStat() && a_path == "/cfg/mod/wSt" && p->ctrChkNode(opt)) {
		string wSt;

		//   ~AA0
		rez = p->owner().serReq(TSYS::strMess("~%02X0",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
		if(rez.size() == 5 && rez[0] == '!') {
		    wSt += (bool)strtol(rez.data()+3,NULL,16) ? _("Set. ") : _("Clear. ");

		    //   ~AA2
		    rez = p->owner().serReq(TSYS::strMess("~%02X2",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
		    if(rez.size() == 6 && rez[0]=='!') {
			wSt += (bool)strtol(string(rez.data()+3,1).c_str(),NULL,16) ? _("Enabled, ") : _("Disabled, ");
			wSt += r2s(0.1*strtol(rez.data()+4,NULL,16))+_(" s.");
		    }
		}
		opt->setText(wSt);
	    }
	    else if(p->owner().startStat() && a_path == "/cfg/mod/vPon" && p->ctrChkNode(opt)) {
		//   ~AA4P
		rez = p->owner().serReq(TSYS::strMess("~%02X4P",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
		if(rez.size() < 7 || rez[0] != '!') opt->setText(_("Error"));
		else {
		    string cnt;
		    uint32_t vl = strtol(rez.data()+3,NULL,16);
		    for(unsigned i_o = 0; i_o < (dev.DO&0xFF)*8; i_o++) cnt += ((vl>>i_o)&0x01)?"1 ":"0 ";
		    opt->setText(cnt);
		}
	    }
	    else if(p->owner().startStat() && a_path == "/cfg/mod/vPonSet" && p->ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
		//   ~AA5P
		p->owner().serReq(TSYS::strMess("~%02X5P",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	    else if(p->owner().startStat() && a_path == "/cfg/mod/vSf" && p->ctrChkNode(opt)) {
		//   ~AA4S
		rez = p->owner().serReq(TSYS::strMess("~%02X4S",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
		if(rez.size() < 7 || rez[0] != '!') opt->setText(_("Error"));
		else {
		    string cnt;
		    int vl = strtol(rez.data()+3,NULL,16);
		    for(unsigned i_o = 0; i_o < (dev.DO&0xFF)*8; i_o++) cnt += ((vl>>i_o)&0x01)?"1 ":"0 ";
		    opt->setText(cnt);
		}
	    }
	    else if(p->owner().startStat() && a_path == "/cfg/mod/vSfSet" && p->ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
		//   ~AA5S
		p->owner().serReq(TSYS::strMess("~%02X5S",(int)((p->owner().bus()==0)?0:p->modAddr)), p->modSlot, CRC);
	    else return false;
	}
	else return false;
    }
    else return false;

    return true;
}


//OpenSCADA system module DAQ.ICP_DAS file: da_87x.cpp
/***************************************************************************
 *   Copyright (C) 2012 by Roman Savochenko                                *
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

#include <tsys.h>

#include "da_87x.h"
#include "ICP_module.h"

using namespace ICP_DAS_DAQ;

//*************************************************
//* da_87x                                        *
//*************************************************
da_87x::da_87x( )
{

}

da_87x::~da_87x( )
{

}

void da_87x::tpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl )
{
    //Only for I-8700 and I-7000 serial bus
    if(prm->owner().bus() < 0)	return;
    tpl.push_back("I-87xxx");	if(ntpl) ntpl->push_back(_("I-87xxx (generic)"));
    tpl.push_back("I-7xxx");	if(ntpl) ntpl->push_back(_("I-7xxx (generic)"));
    tpl.push_back("I-87019");	if(ntpl) ntpl->push_back("I-87019");
    tpl.push_back("I-87024");	if(ntpl) ntpl->push_back("I-87024");
    tpl.push_back("I-87057");	if(ntpl) ntpl->push_back("I-87057");
}

void da_87x::enable( TMdPrm *p, vector<string> &als )
{
    //> Make DAQ parameter's attributes
    if(p->modTp.getS() == "I-87019")
    {
        p->p_el.fldAdd(new TFld("cvct",_("Cold-Junction Compensation(CJC) temperature"),TFld::Real,TFld::NoWrite)); als.push_back("cvct");
        for(int i_i = 0; i_i < 8; i_i++)
        {
            p->p_el.fldAdd(new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite));
            als.push_back(TSYS::strMess("i%d",i_i));
            p->p_el.fldAdd(new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite));
            als.push_back(TSYS::strMess("ha%d",i_i));
            p->p_el.fldAdd(new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite));
            als.push_back(TSYS::strMess("la%d",i_i));
        }
    }
    else if(p->modTp.getS() == "I-87024")
        for(int i_o = 0; i_o < 4; i_o++)
        {
            p->p_el.fldAdd(new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Real,TVal::DirWrite));
            als.push_back(TSYS::strMess("o%d",i_o));
        }
    else if(p->modTp.getS() == "I-87057")
    {
	p->dInOutRev[0] = atoi(p->modPrm("dOutRevvl").c_str());
        for(int i_o = 0; i_o < 16; i_o++)
        {
            p->p_el.fldAdd(new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite));
            als.push_back(TSYS::strMess("o%d",i_o));
        }
    }
}

void da_87x::disable( TMdPrm *prm )
{

}

void da_87x::getVal( TMdPrm *p )
{
    string rez;
    if(p->modTp.getS() == "I-87019")
    {
	rez = p->owner().serReq(TSYS::strMess("#%02X",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
        for(int i_v = 0; i_v < 8; i_v++)
            p->vlAt(TSYS::strMess("i%d",i_v)).at().setR((rez.size() != 57 || rez[0] != '>') ? EVAL_REAL : atof(rez.data()+1+7*i_v), 0, true);

        if(!rez.empty())  rez = p->owner().serReq(TSYS::strMess("$%02X3",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
        p->vlAt("cvct").at().setR((rez.size() != 8 || rez[0] != '>') ? EVAL_REAL : atof(rez.data()+1), 0, true);
        p->acq_err.setVal(rez.empty() ? _("10:Request to module error.") : ((rez[0]!='>')?_("11:Respond from module error."):""));
    }
    else if(p->modTp.getS() == "I-87024")
    {
        //> Host watchdog processing
        rez = "1";
        if(p->wTm >= 0.1)
        {
            rez = p->owner().serReq(TSYS::strMess("~%02X31%02X",(p->owner().bus()==0)?0:p->modAddr,(int)(10*p->wTm)), p->modSlot);
            if(rez.empty()) { p->acq_err.setVal(_("10:Request to module error.")); return; }
        }

        //> Get data
        for(int i_v = 0; i_v < 4; i_v++)
        {
            if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("$%02X8%d",(p->owner().bus()==0)?0:p->modAddr,i_v), p->modSlot);
            p->vlAt(TSYS::strMess("o%d",i_v)).at().setR((rez.size() != 10 || rez[0]!='!') ? EVAL_REAL : atof(rez.data()+3), 0, true);
        }
        p->acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
    }
    else if(p->modTp.getS() == "I-87057")
    {
        //> Host watchdog processing
        rez = "1";
        if(p->wTm >= 0.1)
        {
            rez = p->owner().serReq(TSYS::strMess("~%02X31%02X",(p->owner().bus()==0)?0:p->modAddr,(int)(10*p->wTm)), p->modSlot);
            if(rez.empty()) { p->acq_err.setVal(_("10:Request to module error.")); return; }
        }

        //> Get data
        if(!rez.empty()) rez = p->owner().serReq(TSYS::strMess("$%02X6",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
        int vl = -1;
        if(!(rez.size() != 7 || rez[0] != '!'))
        {
            vl = strtoul(rez.substr(1,4).c_str(),NULL,16);
            p->acq_err.setVal("");
        }
        else p->acq_err.setVal(rez.empty()?_("10:Request to module error."):_("11:Respond from module error."));

        for(int i_v = 0; i_v < 16; i_v++)
            p->vlAt(TSYS::strMess("o%d",i_v)).at().setB((vl<0) ? EVAL_BOOL : (vl>>i_v)&0x01, 0, true);
    }
}

void da_87x::vlSet( TMdPrm *p, TVal &valo, const TVariant &pvl )
{
    string rez;
    if(p->modTp.getS() == "I-87019")
    {
        bool ha = (valo.name().substr(0,2) == "ha");
        bool la = (valo.name().substr(0,2) == "la");
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

        rez = p->owner().serReq(TSYS::strMess("@%02XL%02X%02X",(p->owner().bus()==0)?0:p->modAddr,lvl,hvl), p->modSlot);
        p->acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
    }
    else if(p->modTp.getS() == "I-87024")
    {
        double vl = valo.getR(0,true);
        if(vl == EVAL_REAL || vl == pvl.getR()) return;

        string cmd = TSYS::strMess("#%02X%d%+07.3f",(p->owner().bus()==0)?0:p->modAddr,atoi(valo.name().c_str()+1),vl);

        rep24:
        rez = p->owner().serReq(cmd, p->modSlot);
        //> Set watchdog flag is process
        if(!rez.empty() && rez[0] == '!')
        {
            p->owner().serReq(TSYS::strMess("~%02X1",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
            goto rep24;
        }
        valo.setR((rez.empty() || rez[0]!='>') ? EVAL_REAL : vl, 0, true);
        p->acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
    }
    else if(p->modTp.getS() == "I-87057")
    {
        char vl = valo.getB(0,true);
        if(!p->acq_err.getVal().empty() || vl == EVAL_BOOL || vl == pvl.getB()) return;

        int daddr = atoi(valo.name().c_str()+1);
        string cmd = TSYS::strMess("#%02X%s%d%02X",(p->owner().bus()==0)?0:p->modAddr,(daddr/8)?"B":"A",daddr%8,(vl^(p->dInOutRev[0]>>daddr))&1);

        rep57:
        rez = p->owner().serReq(cmd, p->modSlot);
        //> Set watchdog flag is process
        if(!rez.empty() && rez[0] == '!')
        {
            p->owner().serReq(TSYS::strMess("~%02X1",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
            goto rep57;
        }

        valo.setB((rez.empty() || rez[0]!='>') ? EVAL_BOOL : vl, 0, true);
        p->acq_err.setVal(rez.empty()?_("10:Request to module error."):((rez[0]!='>')?_("11:Respond from module error."):""));
    }
}

bool da_87x::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    string rez;
    if(opt->name() == "info")
    {
	if(p->modTp.getS() == "I-87019" && p->enableStat() && p->owner().startStat() && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
            for(int i_v = 0; i_v < 8; i_v++)
                p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/inTp%d",i_v).c_str(),TSYS::strMess(_("Input %d type"),i_v).c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select","/cfg/tpLst");
	else if(p->modTp.getS() == "I-87024" && p->enableStat() && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	{
	    p->ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,3,"tp","real","min","0","max","25.5");
            if(p->owner().startStat() && p->ctrMkNode("area",opt,-1,"/cfg/mod",_("Module")))
            {
                p->ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
                p->ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
                p->ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from current"),RWRW__,"root",SDAQ_ID);
                p->ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
                p->ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from current"),RWRW__,"root",SDAQ_ID);
            }
        }
	else if(p->modTp.getS() == "I-87057" && p->enableStat() && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	{
            for(int i_o = 0; i_o < 16; i_o++)
                p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/revOut%d",i_o).c_str(),i_o?"":_("Out reverse"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
            p->ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,3,"tp","real","min","0","max","25.5");
            if(p->owner().startStat() && p->ctrMkNode("area",opt,-1,"/cfg/mod",_("Module")))
            {
                p->ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
                p->ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
                p->ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from current"),RWRW__,"root",SDAQ_ID);
                p->ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
                p->ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from current"),RWRW__,"root",SDAQ_ID);
            }
        }
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(p->modTp.getS() == "I-87019")
    {
	if(p->enableStat() && p->owner().startStat() && a_path.substr(0,9) == "/cfg/inTp")
	{
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
    	    {
        	rez = p->owner().serReq(TSYS::strMess("$%02X8C%d",(p->owner().bus()==0)?0:p->modAddr,atoi(a_path.substr(9).c_str())), p->modSlot);
        	opt->setText((rez.size()!=8||rez[0]!='!') ? "-1" : TSYS::int2str(strtol(rez.data()+6,NULL,16)));
    	    }
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
        	p->owner().serReq(TSYS::strMess("$%02X7C%dR%02X",(p->owner().bus()==0)?0:p->modAddr,atoi(a_path.substr(9).c_str()),atoi(opt->text().c_str())), p->modSlot);
	}
	else if(a_path == "/cfg/tpLst" && p->ctrChkNode(opt))
	{
    	    opt->childAdd("el")->setAttr("id","-1")->setText(_("Error"));
    	    opt->childAdd("el")->setAttr("id","0")->setText(_("-15mV to +15mV"));
    	    opt->childAdd("el")->setAttr("id","1")->setText(_("-50mV to +50mV"));
    	    opt->childAdd("el")->setAttr("id","2")->setText(_("-100mV to +100mV"));
    	    opt->childAdd("el")->setAttr("id","3")->setText(_("-500mV to +500mV"));
    	    opt->childAdd("el")->setAttr("id","4")->setText(_("-1V to +1V"));
    	    opt->childAdd("el")->setAttr("id","5")->setText(_("-2.5V to +2.5V"));
    	    opt->childAdd("el")->setAttr("id","6")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
    	    opt->childAdd("el")->setAttr("id","8")->setText(_("-10V to +10V"));
    	    opt->childAdd("el")->setAttr("id","9")->setText(_("-5V to +5V"));
    	    opt->childAdd("el")->setAttr("id","10")->setText(_("-1V to +1V"));
    	    opt->childAdd("el")->setAttr("id","11")->setText(_("-500mV to +500mV"));
    	    opt->childAdd("el")->setAttr("id","12")->setText(_("-150mV to +150mV"));
    	    opt->childAdd("el")->setAttr("id","13")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
    	    opt->childAdd("el")->setAttr("id","14")->setText(_("J Type"));
    	    opt->childAdd("el")->setAttr("id","15")->setText(_("K Type"));
    	    opt->childAdd("el")->setAttr("id","16")->setText(_("T Type"));
    	    opt->childAdd("el")->setAttr("id","17")->setText(_("E Type"));
    	    opt->childAdd("el")->setAttr("id","18")->setText(_("R Type"));
    	    opt->childAdd("el")->setAttr("id","19")->setText(_("S Type"));
    	    opt->childAdd("el")->setAttr("id","20")->setText(_("B Type"));
    	    opt->childAdd("el")->setAttr("id","21")->setText(_("N Type"));
    	    opt->childAdd("el")->setAttr("id","22")->setText(_("C Type"));
    	    opt->childAdd("el")->setAttr("id","23")->setText(_("L Type"));
    	    opt->childAdd("el")->setAttr("id","24")->setText(_("M Type"));
    	    opt->childAdd("el")->setAttr("id","25")->setText(_("L Type (DIN43710C Type)"));
	}
	else return false;
    }
    else if(p->modTp.getS() == "I-87024" && p->enableStat())
    {
	if(a_path == "/cfg/wTm")
	{
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(TSYS::real2str(p->wTm));
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))  p->setModPrm("wTm",TSYS::real2str(p->wTm = atof(opt->text().c_str())));
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/wSt" && p->ctrChkNode(opt))
	{
    	    string wSt;

    	    rez = p->owner().serReq(TSYS::strMess("~%02X0",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
    	    if(rez.size() == 5 && rez[0]=='!')
    	    {
        	wSt += (bool)strtol(rez.data()+3,NULL,16) ? _("Set. ") : _("Clear. ");

        	rez = p->owner().serReq(TSYS::strMess("~%02X2",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
        	if(rez.size() == 6 && rez[0]=='!')
        	{
            	    wSt += (bool)strtol(string(rez.data()+3,1).c_str(),NULL,16) ? _("Enabled, ") : _("Disabled, ");
            	    wSt += TSYS::real2str(0.1*strtol(rez.data()+4,NULL,16))+_(" s.");
        	}
    	    }
    	    opt->setText(wSt);
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vPon" && p->ctrChkNode(opt))
	{
    	    string cnt;
    	    for(int i_c = 0; i_c < 4; i_c++)
    	    {
        	rez = p->owner().serReq( TSYS::strMess("$%02X7%d",(p->owner().bus()==0)?0:p->modAddr,i_c), p->modSlot );
        	if(rez.size() != 10 || rez[0] != '!') { cnt = _("Error"); break; }
        	cnt = cnt + (rez.data()+3) + " ";
    	    }
    	    opt->setText(cnt);
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vPonSet" && p->ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
    	    for(int i_c = 0; i_c < 4; i_c++)
        	p->owner().serReq(TSYS::strMess("$%02X4%d",(p->owner().bus()==0)?0:p->modAddr,i_c), p->modSlot);
	else if(p->owner().startStat() && a_path == "/cfg/mod/vSf" && p->ctrChkNode(opt))
	{
    	    string cnt;
    	    for(int i_c = 0; i_c < 4; i_c++)
    	    {
        	rez = p->owner().serReq(TSYS::strMess("~%02X4%d",(p->owner().bus()==0)?0:p->modAddr,i_c), p->modSlot);
        	if(rez.size() != 10 || rez[0] != '!') { cnt = _("Error"); break; }
        	cnt = cnt + (rez.data()+3) + " ";
    	    }
    	    opt->setText(cnt);
    	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vSfSet" && p->ctrChkNode(opt,"seet",RWRW__,"root",SDAQ_ID,SEC_WR))
    	    for(int i_c = 0; i_c < 4; i_c++)
        	p->owner().serReq(TSYS::strMess("~%02X5%d",(p->owner().bus()==0)?0:p->modAddr,i_c), p->modSlot);
	else return false;
    }
    else if(p->modTp.getS() == "I-87057" && p->enableStat())
    {
	if(a_path == "/cfg/wTm")
	{
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(TSYS::real2str(p->wTm));
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))  p->setModPrm("wTm",TSYS::real2str(p->wTm = atof(opt->text().c_str())));
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/wSt" && p->ctrChkNode(opt))
	{
    	    string wSt;

    	    rez = p->owner().serReq(TSYS::strMess("~%02X0",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
    	    if(rez.size() == 5 && rez[0]=='!')
    	    {
        	wSt += (bool)strtol(rez.data()+3,NULL,16) ? _("Set. ") : _("Clear. ");

        	rez = p->owner().serReq(TSYS::strMess("~%02X2",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
        	if(rez.size() == 6 && rez[0]=='!')
        	{
            	    wSt += (bool)strtol(string(rez.data()+3,1).c_str(),NULL,16) ? _("Enabled, ") : _("Disabled, ");
            	    wSt += TSYS::real2str(0.1*strtol(rez.data()+4,NULL,16))+_(" s.");
        	}
    	    }
    	    opt->setText(wSt);
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vPon" && p->ctrChkNode(opt))
	{
    	    rez = p->owner().serReq(TSYS::strMess("~%02X4P",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
    	    if(rez.size() != 7 || rez[0] != '!') opt->setText(_("Error"));
    	    else
    	    {
        	string cnt;
        	int vl = strtol(rez.data()+3,NULL,16);
        	for(int i_o = 0; i_o < 16; i_o++)   cnt += ((vl>>i_o)&0x01)?"1 ":"0 ";
        	opt->setText(cnt);
    	    }
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vPonSet" && p->ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
    	    p->owner().serReq(TSYS::strMess("~%02X5P",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
	else if(p->owner().startStat() && a_path == "/cfg/mod/vSf" && p->ctrChkNode(opt))
	{
    	    rez = p->owner().serReq( TSYS::strMess("~%02X4S",(p->owner().bus()==0)?0:p->modAddr), p->modSlot );
    	    if(rez.size() != 7 || rez[0] != '!') opt->setText(_("Error"));
    	    else
    	    {
        	string cnt;
        	int vl = strtol(rez.data()+3,NULL,16);
        	for(int i_o = 0; i_o < 16; i_o++)   cnt += ((vl>>i_o)&0x01)?"1 ":"0 ";
        	opt->setText(cnt);
    	    }
	}
	else if(p->owner().startStat() && a_path == "/cfg/mod/vSfSet" && p->ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
    	    p->owner().serReq(TSYS::strMess("~%02X5S",(p->owner().bus()==0)?0:p->modAddr), p->modSlot);
	else return false;
    }
    else return false;

    return true;
}
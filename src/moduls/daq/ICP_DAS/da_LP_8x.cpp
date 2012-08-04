
//OpenSCADA system module DAQ.ICP_DAS file: da_LP_8x.cpp
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

#include <unistd.h>
#include "lincon.h"

#include <tsys.h>

#include "da_LP_8x.h"
#include "ICP_module.h"

using namespace ICP_DAS_DAQ;

//*************************************************
//* CPU                                           *
//*************************************************
da_LP_8x::da_LP_8x( )
{

}

da_LP_8x::~da_LP_8x( )
{

}

void da_LP_8x::tpList( TMdPrm *p, vector<string> &tpl, vector<string> *ntpl )
{
    //Only for LP, I-8000 parallel bus
    if(p->owner().bus() != 0)	return;
    tpl.push_back("LP-8x8x");	if(ntpl) ntpl->push_back("LP-8x8x (PAC)");
    tpl.push_back("I-8017");	if(ntpl) ntpl->push_back("I-8017");
    tpl.push_back("I-8042");	if(ntpl) ntpl->push_back("I-8042");
}

void da_LP_8x::enable( TMdPrm *p, vector<string> &als )
{
    //> Make DAQ parameter's attributes
    if(p->modTp.getS() == "LP-8x8x")
    {
        p->p_el.fldAdd(new TFld("serial",_("Serial number"),TFld::String,TFld::NoWrite)); als.push_back("serial");
        p->p_el.fldAdd(new TFld("SDK",_("SDK version"),TFld::Real,TFld::NoWrite)); als.push_back("SDK");
        p->p_el.fldAdd(new TFld("DIP",_("DIP switch"),TFld::Integer,TFld::NoWrite)); als.push_back("DIP");
    }
    else if(p->modTp.getS() == "I-8017")
    {
	if(!p->extPrms) p->extPrms = new PrmsI8017();
        ((PrmsI8017*)p->extPrms)->prmNum = vmin(8,vmax(0,atoi(p->modPrm("cnls").c_str())));
        ((PrmsI8017*)p->extPrms)->fastPer = atof(p->modPrm("fastPer").c_str());
        for(int i_i = 0; i_i < 8; i_i++)
        {
            ((PrmsI8017*)p->extPrms)->cnlMode[i_i] = atoi(p->modPrm(TSYS::strMess("cnl:%d",i_i)).c_str());
            p->p_el.fldAdd(new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite));
            als.push_back(TSYS::strMess("i%d",i_i));
            p->p_el.fldAdd(new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite));
            als.push_back(TSYS::strMess("ha%d",i_i));
            p->p_el.fldAdd(new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite));
            als.push_back(TSYS::strMess("la%d",i_i));
        }
    }
    else if(p->modTp.getS() == "I-8042")
    {
	p->dInOutRev[0] = atoi(p->modPrm("dInRev").c_str());
	p->dInOutRev[1] = atoi(p->modPrm("dOutRevvl").c_str());
        for(int i_i = 0; i_i < 16; i_i++)
        {
            p->p_el.fldAdd(new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Boolean,TFld::NoWrite));
            als.push_back(TSYS::strMess("i%d",i_i));
        }
        for( int i_o = 0; i_o < 16; i_o++ )
        {
            p->p_el.fldAdd(new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite));
            als.push_back(TSYS::strMess("o%d",i_o));
        }
    }
}

void da_LP_8x::disable( TMdPrm *p )
{
    //> Free module object
    if(p->modTp.getS() == "I-8017")
    {
	delete ((PrmsI8017*)p->extPrms);
	p->extPrms = NULL;
    }
}

void da_LP_8x::getVal( TMdPrm *p )
{
    if(p->modTp.getS() == "LP-8x8x")
    {
        //> Read serial number
        AutoHD<TVal> vl = p->vlAt("serial");
        if(vl.at().getS() == EVAL_STR)
        {
            ResAlloc res(p->owner().reqRes, true);
            unsigned char sN[8];
            Read_SN(sN);
            res.release();
            vl.at().setS( TSYS::strMess("%x%x%x%x%x%x%x%x",sN[0],sN[1],sN[2],sN[3],sN[4],sN[5],sN[6],sN[7]), 0, true );
        }
        //> Read SDK version
        vl = p->vlAt("SDK");
        if(vl.at().getR() == EVAL_REAL) vl.at().setR(GetSDKversion(), 0, true);

        //> Read DIP switch status
        p->owner().pBusRes.resRequestW();
        int dpSw = GetDIPswitch();
        p->owner().pBusRes.resRelease();
        p->vlAt("DIP").at().setI(dpSw,0,true);
    }
    else if(p->modTp.getS() == "I-8017")
    {
        //> Check for I8017 init
        if(!((PrmsI8017*)p->extPrms)->init)
        { p->owner().pBusRes.resRequestW(); I8017_Init(p->modSlot); ((PrmsI8017*)p->extPrms)->init = true; p->owner().pBusRes.resRelease(); }
        //> Check for I8017 fast task start
        if(((PrmsI8017*)p->extPrms)->fastPer && !p->prcSt) SYS->taskCreate(p->nodePath('.',true), 32, fastTask, p);
        //> Get values direct
        for(int i_v = 0; i_v < 8; i_v++)
            if(i_v >= ((PrmsI8017*)p->extPrms)->prmNum) p->vlAt(TSYS::strMess("i%d",i_v)).at().setR(EVAL_REAL,0,true);
            else if(!((PrmsI8017*)p->extPrms)->fastPer)
            {
                ResAlloc res(p->owner().pBusRes, true);
                I8017_SetChannelGainMode(p->modSlot,i_v,((PrmsI8017*)p->extPrms)->cnlMode[i_v],0);
                p->vlAt(TSYS::strMess("i%d",i_v)).at().setR( I8017_GetCurAdChannel_Float_Cal(p->modSlot), 0, true );
            }
    }
    else if(p->modTp.getS() == "I-8042")
    {
        p->owner().pBusRes.resRequestW();
        int c_vl = DI_16(p->modSlot);
        p->owner().pBusRes.resRelease();
        for(int i_v = 0; i_v < 16; i_v++) p->vlAt(TSYS::strMess("i%d",i_v)).at().setB(((p->dInOutRev[0]^c_vl)>>i_v)&0x01, 0, true);
        p->owner().pBusRes.resRequestW();
        c_vl = DO_16_RB(p->modSlot);
        p->owner().pBusRes.resRelease();
        for(int o_v = 0; o_v < 16; o_v++) p->vlAt(TSYS::strMess("o%d",o_v)).at().setB(((p->dInOutRev[1]^c_vl)>>o_v)&0x01, 0, true);
    }
}

void da_LP_8x::vlSet( TMdPrm *p, TVal &valo, const TVariant &pvl )
{
    if(p->modTp.getS() == "I-8017")
    {
        bool ha = (valo.name().substr(0,2) == "ha");
        bool la = (valo.name().substr(0,2) == "la");
        if(!(ha||la) || !((PrmsI8017*)p->extPrms)->init) return;

        //> Create previous value
        int hvl = 0, lvl = 0;
        for(int i_v = 7; i_v >= 0; i_v--)
        {
            hvl = hvl << 1;
            lvl = lvl << 1;
            if(p->vlAt(TSYS::strMess("ha%d",i_v)).at().getB() == true) hvl |= 1;
            if(p->vlAt(TSYS::strMess("la%d",i_v)).at().getB() == true) lvl |= 1;
        }
        p->owner().pBusRes.resRequestW(1000);
        I8017_SetLed(p->modSlot,(lvl<<8)|hvl);
        p->owner().pBusRes.resRelease();
    }
    else if(p->modTp.getS() == "I-8042")
    {
        bool vl = valo.getB(0,true);
        if(vl == EVAL_BOOL || vl == pvl.getB()) return;
        int chnl = atoi(valo.name().c_str()+1);

        p->owner().pBusRes.resRequestW(1000);
        DO_16(p->modSlot, ((vl^(p->dInOutRev[1]>>chnl))&1) ? (DO_16_RB(p->modSlot) | 0x01<<chnl) : (DO_16_RB(p->modSlot) & ~(0x01<<chnl)));
        p->owner().pBusRes.resRelease();
    }
}

bool da_LP_8x::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    if(opt->name() == "info")
    {
	if(p->modTp.getS() == "I-8017" && p->enableStat() && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
        {
            p->ctrMkNode("fld",opt,-1,"/cfg/prms",_("Process parameters"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
            p->ctrMkNode("fld",opt,-1,"/cfg/fastPer",_("Fast data get period (s)"),RWRWR_,"root",SDAQ_ID,1,"tp","real");
            if(p->ctrMkNode("area",opt,-1,"/cfg/mode",_("Mode")))
                for(int i_v = 0; i_v < 8; i_v++)
                    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/mode/in%d",i_v).c_str(),TSYS::strMess(_("Input %d"),i_v).c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select","/cfg/tpLst");
        }
	else if(p->modTp.getS() == "I-8042" && p->enableStat() && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	{
            for(int i_o = 0; i_o < 16; i_o++)
                p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/revIn%d",i_o).c_str(),i_o?"":_("Inputs reverse"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
            for(int i_o = 0; i_o < 16; i_o++)
                p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/revOut%d",i_o).c_str(),i_o?"":_("Outputs reverse"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	}
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(p->modTp.getS() == "I-8017" && p->enableStat())
    {
	if(a_path == "/cfg/prms" )
	{
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(TSYS::int2str(((PrmsI8017*)p->extPrms)->prmNum));
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
        	p->setModPrm("cnls",TSYS::int2str(((PrmsI8017*)p->extPrms)->prmNum = atoi(opt->text().c_str())));
	}
	else if(a_path == "/cfg/fastPer")
	{
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(TSYS::real2str(((PrmsI8017*)p->extPrms)->fastPer,5));
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
        	p->setModPrm("fastPer",TSYS::real2str(((PrmsI8017*)p->extPrms)->fastPer = atof(opt->text().c_str()),5));
	}
	else if(a_path.substr(0,12) == "/cfg/mode/in")
	{
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(TSYS::int2str(((PrmsI8017*)p->extPrms)->cnlMode[atoi(a_path.substr(12).c_str())]));
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
        	p->setModPrm("cnl:"+a_path.substr(12),TSYS::int2str(((PrmsI8017*)p->extPrms)->cnlMode[atoi(a_path.substr(12).c_str())] = atoi(opt->text().c_str())));
	}
	else if(a_path == "/cfg/tpLst" && p->ctrChkNode(opt))
	{
    	    opt->childAdd("el")->setAttr("id","0")->setText(_("-10V to +10V"));
    	    opt->childAdd("el")->setAttr("id","1")->setText(_("-5V to +5V"));
    	    opt->childAdd("el")->setAttr("id","2")->setText(_("-2.5V to +2.5V"));
    	    opt->childAdd("el")->setAttr("id","3")->setText(_("-1.25V to +1.25V"));
    	    opt->childAdd("el")->setAttr("id","4")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
	}
	else return false;
    }
    else if(p->modTp.getS() == "I-8042" && p->enableStat())
    {
	if(a_path.substr(0,10) == "/cfg/revIn")
	{
    	    int rin = atoi(a_path.substr(10).c_str());
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText((p->dInOutRev[0]&(1<<rin))?"1":"0");
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
        	p->setModPrm("dInRev",TSYS::int2str(p->dInOutRev[0] = atoi(opt->text().c_str()) ?
        	    (p->dInOutRev[0] | (1<<rin)) : (p->dInOutRev[0] & ~(1<<rin))));
	}
	else if(a_path.substr(0,11) == "/cfg/revOut")
	{
    	    int rout = atoi(a_path.substr(11).c_str());
    	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText((p->dInOutRev[1]&(1<<rout))?"1":"0");
    	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
        	p->setModPrm("dOutRev",TSYS::int2str(p->dInOutRev[1] = atoi(opt->text().c_str()) ?
        	    (p->dInOutRev[1]|(1<<rout)) : (p->dInOutRev[1] & ~(1<<rout))));
	}
	else return false;
    }
    else return false;

    return true;
}

void *da_LP_8x::fastTask( void *iprm )
{
    TMdPrm &prm = *(TMdPrm*)iprm;

    prm.endRunReq = false;
    prm.prcSt = true;

    struct timespec sp_tm;
    int64_t wTm = TSYS::curTime();
    int c_mode;

    vector< AutoHD<TVal> > cnls;
    for(int i_c = 0; i_c < ((PrmsI8017*)prm.extPrms)->prmNum; i_c++)
        cnls.push_back(prm.vlAt(TSYS::strMess("i%d",i_c)));
    float vbuf[cnls.size()];

    while(!prm.endRunReq)
    {
        prm.owner().pBusRes.resRequestW( );
        for(unsigned i_c = 0; prm.owner().startStat() && i_c < cnls.size(); i_c++)
        {
            c_mode = ((PrmsI8017*)prm.extPrms)->cnlMode[i_c];
            I8017_SetChannelGainMode(prm.modSlot,i_c,c_mode,0);
            vbuf[i_c] = (10.0/(c_mode?2*c_mode:1))*(float)I8017_GetCurAdChannel_Hex(prm.modSlot)/8000;
        }
        prm.owner().pBusRes.resRelease( );

        for(unsigned i_c = 0; prm.owner().startStat() && i_c < cnls.size(); i_c++)
            cnls[i_c].at().setR(vbuf[i_c], wTm, true);

        //> Calc next work time and sleep
        wTm += (int64_t)(1e6*((PrmsI8017*)prm.extPrms)->fastPer);
        sp_tm.tv_sec = wTm/1000000; sp_tm.tv_nsec = 1000*(wTm%1000000);
        clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&sp_tm,NULL);
    }

    prm.prcSt = false;

    return NULL;
}

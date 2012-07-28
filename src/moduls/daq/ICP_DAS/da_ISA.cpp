
//OpenSCADA system module DAQ.ICP_DAS file: da_ISA.cpp
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

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ixisa.h"

#include <tsys.h>

#include "da_ISA.h"
#include "ICP_module.h"

using namespace ICP_DAS_DAQ;

//*************************************************
//* CPU                                           *
//*************************************************
da_ISA::da_ISA( )
{
    devs["DIO-48"]  = DevFeature(0x0203);
    devs["DIO-96"]  = DevFeature(0x0403);
    devs["DIO-144"] = DevFeature(0x0603);
}

da_ISA::~da_ISA( )
{

}

string da_ISA::modType( const string &modTp )
{
    FILE *fd_proc = fopen(IXISA_PROC_FILE, "r");
    if(fd_proc)
    {
        char rbuf[200], isadev[31], isaname[31];
        while(fgets(rbuf,sizeof(rbuf),fd_proc))
            if(sscanf(rbuf,"dev: %30s %*x %*x %30s",isadev,isaname) == 2 && modTp == isadev)
            { fclose(fd_proc); return isaname; }
        fclose(fd_proc);
    }

    return "";
}

void da_ISA::tpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl )
{
    //Only for ISA boards
    if(prm->owner().bus() != -1) return;
    FILE *fd_proc = fopen(IXISA_PROC_FILE, "r");
    if(fd_proc)
    {
        char rbuf[200], isadev[31], isaname[31];
        while(fgets(rbuf,sizeof(rbuf),fd_proc))
    	    if(sscanf(rbuf,"dev: %30s %*x %*x %30s",isadev,isaname) == 2)
            { tpl.push_back(isadev); if(ntpl) ntpl->push_back(TSYS::strMess("%s (%s)",isaname,isadev)); }
        fclose(fd_proc);
    }
}

void da_ISA::enable( TMdPrm *p, vector<string> &als )
{
    if(!p->extPrms) p->extPrms = new tval();

    //> Typical DIO, like "DIO-144" process
    if((((tval*)p->extPrms)->dev=devs[modType(p->modTp.getS())]).dio)
    {
        ((tval*)p->extPrms)->devFd = open(("/dev/"+p->modTp.getS()).c_str(), O_RDWR);
        if(((tval*)p->extPrms)->devFd >= 0)
        {
    	    int directDIO = atoi(p->modPrm("DirectDIO").c_str());

    	    for(int i_ch = 0; i_ch < (((tval*)p->extPrms)->dev.dio>>8); i_ch++)
    	    {
    		//> Set board configuration
        	ixisa_reg_t reg;
        	reg.value = (directDIO&(1<<i_ch)) ? 0x80 : 0x9b;
        	switch(i_ch)
        	{
            	    case 0: reg.id = IXISA_CN0CR;   break;
            	    case 1: reg.id = IXISA_CN1CR;   break;
            	    case 2: reg.id = IXISA_CN2CR;   break;
            	    case 3: reg.id = IXISA_CN3CR;   break;
            	    case 4: reg.id = IXISA_CN4CR;   break;
            	    case 5: reg.id = IXISA_CN5CR;   break;
        	}
        	int rez = ioctl(((tval*)p->extPrms)->devFd,IXISA_WRITE_REG,&reg);

        	//> Attributes create
        	if(directDIO&(1<<i_ch))
            	    for(int i_o = 0; i_o < (((tval*)p->extPrms)->dev.dio&0xFF)*8; i_o++)
            	    {
                	p->p_el.fldAdd(new TFld(TSYS::strMess("o%d_%d",i_ch,i_o).c_str(),TSYS::strMess(_("Out %d.%d"),i_ch,i_o).c_str(),TFld::Boolean,TVal::DirWrite));
                	als.push_back(TSYS::strMess("o%d_%d",i_ch,i_o));
            	    }
        	else
            	    for(int i_i = 0; i_i < (((tval*)p->extPrms)->dev.dio&0xFF)*8; i_i++)
            	    {
            		p->p_el.fldAdd(new TFld(TSYS::strMess("i%d_%d",i_ch,i_i).c_str(),TSYS::strMess(_("Input %d.%d"),i_ch,i_i).c_str(),TFld::Boolean,TFld::NoWrite));
                	als.push_back(TSYS::strMess("i%d_%d",i_ch,i_i));
            	    }
    	    }
    	}
    }
}

void da_ISA::disable( TMdPrm *p )
{
    if(p->extPrms)
    {
	if(((tval*)p->extPrms)->devFd >= 0) close(((tval*)p->extPrms)->devFd);
	p->extPrms = NULL;
    }
}

void da_ISA::getVal( TMdPrm *p )
{
    if(((tval*)p->extPrms)->devFd < 0) return;

    //> Typical DIO, like "DIO-144" process
    if(((tval*)p->extPrms)->dev.dio)
    {
        int directDIO = atoi(p->modPrm("DirectDIO").c_str());
        for(int i_ch = 0; i_ch < (((tval*)p->extPrms)->dev.dio>>8); i_ch++)
            for(int i_p = 0; i_p < (((tval*)p->extPrms)->dev.dio&0xFF); i_p++)
            {
                ixisa_reg_t data;
                switch(i_p)
                {
                    case 0:     //Port A
                        switch(i_ch)
                        {
                            case 0: data.id = IXISA_CN0PA;      break;
                            case 1: data.id = IXISA_CN1PA;      break;
                            case 2: data.id = IXISA_CN2PA;      break;
                            case 3: data.id = IXISA_CN3PA;      break;
                            case 4: data.id = IXISA_CN4PA;      break;
                            case 5: data.id = IXISA_CN5PA;      break;
                        }
                        break;
                    case 1:     //Port B
                        switch(i_ch)
                        {
                            case 0: data.id = IXISA_CN0PB;      break;
                            case 1: data.id = IXISA_CN1PB;      break;
                            case 2: data.id = IXISA_CN2PB;      break;
                            case 3: data.id = IXISA_CN3PB;      break;
                            case 4: data.id = IXISA_CN4PB;      break;
                            case 5: data.id = IXISA_CN5PB;      break;
                        }
                        break;
                    case 2:     //Port C
                        switch(i_ch)
                        {
                            case 0: data.id = IXISA_CN0PC;      break;
                            case 1: data.id = IXISA_CN1PC;      break;
                            case 2: data.id = IXISA_CN2PC;      break;
                            case 3: data.id = IXISA_CN3PC;      break;
                            case 4: data.id = IXISA_CN4PC;      break;
                            case 5: data.id = IXISA_CN5PC;      break;
                        }
                        break;
                }
                bool rez = ioctl(((tval*)p->extPrms)->devFd,IXISA_READ_REG,&data);
                if((directDIO>>i_ch)&1)
                    for(int i_o = 0; i_o < 8; i_o++)
                        p->vlAt(TSYS::strMess("o%d_%d",i_ch,i_p*8+i_o)).at().setB(rez?EVAL_BOOL:(data.value>>i_o)&1, 0, true);
                else
                    for(int i_i = 0; i_i < 8; i_i++)
                        p->vlAt(TSYS::strMess("i%d_%d",i_ch,i_p*8+i_i)).at().setB(rez?EVAL_BOOL:(data.value>>i_i)&1, 0, true);
            }
    }
}

void da_ISA::vlSet( TMdPrm *p, TVal &valo, const TVariant &pvl )
{
    if(((tval*)p->extPrms)->devFd < 0) return;

    //> Typical DIO, like "DIO-144" process
    if(((tval*)p->extPrms)->dev.dio)
    {
        int i_ch = 0, i_p = 0;
        if(sscanf(valo.name().c_str(),"o%d_%d",&i_ch,&i_p) != 2) return;
        i_p = i_p/8;
        ixisa_reg_t data;
        switch(i_p)
        {
            case 0:     //Port A
                switch(i_ch)
                {
                    case 0: data.id = IXISA_CN0PA;      break;
                    case 1: data.id = IXISA_CN1PA;      break;
                    case 2: data.id = IXISA_CN2PA;      break;
                    case 3: data.id = IXISA_CN3PA;      break;
                    case 4: data.id = IXISA_CN4PA;      break;
                    case 5: data.id = IXISA_CN5PA;      break;
                }
                break;
            case 1:     //Port B
                switch(i_ch)
                {
                    case 0: data.id = IXISA_CN0PB;      break;
                    case 1: data.id = IXISA_CN1PB;      break;
                    case 2: data.id = IXISA_CN2PB;      break;
                    case 3: data.id = IXISA_CN3PB;      break;
                    case 4: data.id = IXISA_CN4PB;      break;
                    case 5: data.id = IXISA_CN5PB;      break;
                }
                break;
            case 2:     //Port C
                switch(i_ch)
                {
                    case 0: data.id = IXISA_CN0PC;      break;
                    case 1: data.id = IXISA_CN1PC;      break;
                    case 2: data.id = IXISA_CN2PC;      break;
                    case 3: data.id = IXISA_CN3PC;      break;
                    case 4: data.id = IXISA_CN4PC;      break;
                    case 5: data.id = IXISA_CN5PC;      break;
                }
                break;
        }
        data.value = 0;
        for(int i_o = 7; i_o >= 0; i_o--)
        {
            data.value = data.value << 1;
            if(p->vlAt(TSYS::strMess("o%d_%d",i_ch,i_p*8+i_o)).at().getB(0, true)) data.value |= 1;
        }
        int rez = ioctl(((tval*)p->extPrms)->devFd,IXISA_WRITE_REG,&data);
    }
}

bool da_ISA::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    DevFeature dev;

    if(opt->name() == "info")
    {
	//> Typical DIO, like "DIO-144" process
	if((dev=devs[modType(p->modTp.getS())]).dio && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
            for(unsigned i_ch = 0; i_ch < (dev.dio>>8); i_ch++)
                p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/chnOut%d",i_ch).c_str(),TSYS::strMess(_("Channel %d out"),i_ch).c_str(),
            	    p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	return true;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    //> Typical DIO, like "DIO-144" process
    if((dev=devs[modType(p->modTp.getS())]).dio && a_path.compare(0,11,"/cfg/chnOut") == 0)
    {
        int rout = atoi(a_path.c_str()+11);
        if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))  opt->setText(atoi(p->modPrm("DirectDIO").c_str())&(1<<rout)?"1":"0");
        if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
            p->setModPrm("DirectDIO",TSYS::int2str(atoi(opt->text().c_str()) ? atoi(p->modPrm("DirectDIO").c_str()) | (1<<rout) :
                                                                               atoi(p->modPrm("DirectDIO").c_str()) & ~(1<<rout)));
    }
    else return false;

    return true;
}

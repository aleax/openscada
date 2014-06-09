
//OpenSCADA system module DAQ.ICP_DAS file: da_ISA.cpp
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
    devs["A-626"]	= DevFeature(0, 6, 0x0002, 0x0002);
    devs["A-628"]	= DevFeature(0, 8, 0x0202, 0x0202);
    devs["A-8111"]	= DevFeature(0x1108, 2, 0x0102, 0x0102);
    devs["A-812"]	= DevFeature(0x0110, 2, 0x0102, 0x0102);
    devs["A-821"]	= DevFeature(0x1010, 1, 0x0102, 0x0102);
    devs["A-822"]	= DevFeature(0x0110, 2, 0x0102, 0x0102);
    devs["A-823"]	= DevFeature(0x0110, 2, 0x0102, 0x0102);
    devs["A-826"]	= DevFeature(0x0110, 2, 0x0102, 0x0102);
    devs["DIO-144"]	= DevFeature(0, 0, 6);
    devs["DIO-24"]	= DevFeature(0, 0, 1);
    devs["DIO-48"]	= DevFeature(0, 0, 3);			//Real present two channels from 1 to 2
    devs["DIO-64"]	= DevFeature(0, 0, 0x0204, 0x0204);
    devs["DIO-96"]	= DevFeature(0, 0, 4);
    devs["ISO-730"]	= DevFeature(0, 0, 0x0204, 0x0204);	//Specific for isolation separated (first 2 channels)
    devs["ISO-813"]	= DevFeature(32);
    devs["ISO-C64"]	= DevFeature(0, 0, 0, 0, 0x0008);
    devs["ISO-P32C32"]	= DevFeature(0, 0, 0x0204, 0x0204);
    devs["ISO-P64"]	= DevFeature(0, 0, 0, 0x0008);
    devs["P16R16DIO"]	= DevFeature(0, 0, 0x0002, 0x0002);
    devs["P8R8DIO"]	= DevFeature(0, 0, 0x0101, 0x0101);
    devs["TMC-10"]	= DevFeature(0, 0, 0x0002, 0x0001);
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
    string chnId, chnNm;

    if(!p->extPrms) p->extPrms = new tval();
    tval *ePrm = (tval*)p->extPrms;

    ePrm->dev = devs[modType(p->modTp.getS())];
    ePrm->devFd = open(("/dev/"+p->modTp.getS()).c_str(), O_RDWR);
    if(ePrm->devFd < 0)	return;

    ResAlloc res(p->owner().pBusRes, true);

    //> AI processing
    if(ePrm->dev.AI)
    {
	ePrm->dev.aiTm = atoi(p->modPrm("aiTm","200").c_str());
	for(unsigned i_a = 0; i_a < (ePrm->dev.AI&0xFF); i_a++)
	{
	    chnId = TSYS::strMess("ai%d",i_a); chnNm = TSYS::strMess(_("Analog input %d"),i_a);
	    p->p_el.fldAt(p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Integer,TFld::NoWrite))).
		setReserve(p->modPrm(TSYS::strMess("aiGain.%d",i_a)));
	    als.push_back(chnId);
	}
	//>> Switch to software trigger/transfer mode
	ixisa_reg_t reg;
	reg.id = IXISA_ADMCR;
	if((reg.value=(ePrm->dev.AI>>8))) ioctl(ePrm->devFd, IXISA_WRITE_REG, &reg);
    }

    //> AO processing
    for(unsigned i_a = 0; i_a < ePrm->dev.AO; i_a++)
    {
	chnId = TSYS::strMess("ao%d",i_a); chnNm = TSYS::strMess(_("Analog output %d"),i_a);
	p->p_el.fldAt(p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Integer,TVal::DirWrite)));
	als.push_back(chnId);
    }

    //> DIO processing
    if(ePrm->dev.DIO)
    {
	int directDIO = atoi(p->modPrm("DirectDIO").c_str());
	for(unsigned i_ch = 0; i_ch < ePrm->dev.DIO; i_ch++)
	{
	    p->dInOutRev[i_ch] = atoi(p->modPrm("dIORev"+i2s(i_ch)).c_str());
	    //> Set board configuration
	    ixisa_reg_t reg;
	    reg.value = (directDIO&(1<<i_ch)) ? 0x80 : 0x9b;
	    if((directDIO&(1<<i_ch)) && modType(p->modTp.getS()) == "DIO-24") reg.value = 0x89;

	    if(ePrm->dev.DIO == 1) reg.id = IXISA_CR;
	    else switch(i_ch)
	    {
		case 0: reg.id = IXISA_CN0CR;   break;
		case 1: reg.id = IXISA_CN1CR;   break;
		case 2: reg.id = IXISA_CN2CR;   break;
		case 3: reg.id = IXISA_CN3CR;   break;
		case 4: reg.id = IXISA_CN4CR;   break;
		case 5: reg.id = IXISA_CN5CR;   break;
	    }
	    ioctl(ePrm->devFd, IXISA_WRITE_REG, &reg);

	    //> Attributes create
	    if((directDIO>>i_ch)&1)
		for(int i_o = 0; i_o < 24; i_o++)
		{
		    chnId = TSYS::strMess("o%d_%d",i_ch,i_o); chnNm = TSYS::strMess(_("Digital out %d.%d"),i_ch,i_o);
		    p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite));
		    als.push_back(chnId);
		}
	    else
		for(int i_i = 0; i_i < 24; i_i++)
		{
		    chnId = TSYS::strMess("i%d_%d",i_ch,i_i); chnNm = TSYS::strMess(_("Digital input %d.%d"),i_ch,i_i);
		    p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite));
		    als.push_back(chnId);
		}
	}
    }

    //> DI and DO processing
    for(unsigned i_ch = 0; i_ch < ((ePrm->dev.DI&0xFF)+(ePrm->dev.DO&0xFF)); i_ch++)
    {
	//> Reverse configuration load
	p->dInOutRev[i_ch] = atoi(p->modPrm("dIORev"+i2s(i_ch)).c_str());

	//> Attributes create
	if(i_ch < (ePrm->dev.DI&0xFF))
	    for(int i_i = 0; i_i < 8; i_i++)
	    {
		chnId = TSYS::strMess("i%d_%d",i_ch,i_i); chnNm = TSYS::strMess(_("Digital input %d.%d"),i_ch,i_i);
		p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite));
		als.push_back(chnId);
	    }
	else
	    for(int i_o = 0; i_o < 8; i_o++)
	    {
		chnId = TSYS::strMess("o%d_%d",i_ch-(ePrm->dev.DI&0xFF),i_o); chnNm = TSYS::strMess(_("Digital out %d.%d"),i_ch-(ePrm->dev.DI&0xFF),i_o);
		p->p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite));
		als.push_back(chnId);
	    }
    }
}

void da_ISA::disable( TMdPrm *p )
{
    if(p->extPrms)
    {
	tval *ePrm = (tval*)p->extPrms;
	if(ePrm->devFd >= 0) close(ePrm->devFd);
	delete (tval *)p->extPrms;
	p->extPrms = NULL;
    }
}

void da_ISA::getVal( TMdPrm *p )
{
    tval *ePrm = (tval*)p->extPrms;
    if(ePrm->devFd < 0) return;

    ResAlloc res(p->owner().pBusRes, true);

    //> AI processing
    if(ePrm->dev.AI)
    {
	ixisa_reg_t reg, rai;
	rai.id = IXISA_AI;
	rai.mode = IXISA_RM_TRIGGER;

	int pGain = -1, curGain;
	for(unsigned i_a = 0; i_a < (ePrm->dev.AI&0xFF); i_a++)
	{
	    int rez = 0;
	    AutoHD<TVal> val = p->vlAt(TSYS::strMess("ai%d",i_a));
	    //> Setup gain
	    curGain = atoi(val.at().fld().reserve().c_str());
	    if(curGain != pGain)
	    {
		reg.id = IXISA_ADGCR;
		reg.value = pGain = curGain;
		rez = ioctl(ePrm->devFd, IXISA_WRITE_REG, &reg);
	    }
	    //> Switch channel
	    reg.id = IXISA_ADMXCR;
	    reg.value = i_a;
	    if(!rez) rez = ioctl(ePrm->devFd, IXISA_WRITE_REG, &reg);
	    TSYS::sysSleep((float)ePrm->dev.aiTm*1e-6);
	    //> Read value
	    if(!rez) rez = ioctl(ePrm->devFd, IXISA_READ_REG, &rai);

	    val.at().setI(rez?EVAL_REAL:(rai.value), 0, true);
	}
    }

    //> DIO processing
    if(ePrm->dev.DIO)
    {
	int directDIO = atoi(p->modPrm("DirectDIO").c_str());
	for(unsigned i_ch = 0; i_ch < ePrm->dev.DIO; i_ch++)
	    for(int i_p = 0; i_p < 3; i_p++)
	    {
		ixisa_reg_t data;
		switch(i_p)
		{
		    case 0:	//Port A
			if(ePrm->dev.DIO == 1) data.id = IXISA_PA;
			else switch(i_ch)
			{
			    case 0: data.id = IXISA_CN0PA;	break;
			    case 1: data.id = IXISA_CN1PA;	break;
			    case 2: data.id = IXISA_CN2PA;	break;
			    case 3: data.id = IXISA_CN3PA;	break;
			    case 4: data.id = IXISA_CN4PA;	break;
			    case 5: data.id = IXISA_CN5PA;	break;
			}
			break;
		    case 1:     //Port B
			if(ePrm->dev.DIO == 1) data.id = IXISA_PB;
			else switch(i_ch)
			{
			    case 0: data.id = IXISA_CN0PB;	break;
			    case 1: data.id = IXISA_CN1PB;	break;
			    case 2: data.id = IXISA_CN2PB;	break;
			    case 3: data.id = IXISA_CN3PB;	break;
			    case 4: data.id = IXISA_CN4PB;	break;
			    case 5: data.id = IXISA_CN5PB;	break;
			}
			break;
		    case 2:     //Port C
			if(ePrm->dev.DIO == 1) data.id = IXISA_PC;
			else switch(i_ch)
			{
			    case 0: data.id = IXISA_CN0PC;	break;
			    case 1: data.id = IXISA_CN1PC;	break;
			    case 2: data.id = IXISA_CN2PC;	break;
			    case 3: data.id = IXISA_CN3PC;	break;
			    case 4: data.id = IXISA_CN4PC;	break;
			    case 5: data.id = IXISA_CN5PC;	break;
			}
			break;
		}
		int rez = ioctl(ePrm->devFd, IXISA_READ_REG, &data);
		if((directDIO>>i_ch)&1)
		    for(int i_o = 0; i_o < 8; i_o++)
			p->vlAt(TSYS::strMess("o%d_%d",i_ch,i_p*8+i_o)).at().setB(rez?EVAL_BOOL:(((p->dInOutRev[i_ch]>>(i_p*8))^data.value)>>i_o)&1, 0, true);
		else
		    for(int i_i = 0; i_i < 8; i_i++)
			p->vlAt(TSYS::strMess("i%d_%d",i_ch,i_p*8+i_i)).at().setB(rez?EVAL_BOOL:(((p->dInOutRev[i_ch]>>(i_p*8))^data.value)>>i_i)&1, 0, true);
	    }
    }

    //> DI processing
    for(int i_ch = 0; i_ch < (ePrm->dev.DI&0xFF); i_ch++)
    {
	ixisa_reg_t data;
	switch(ePrm->dev.DI>>8)
	{
	    case 0:		//IXISA_DI_A(N)
		switch(i_ch)
		{
		    case 0: data.id = IXISA_DI_A;	break;
		    case 1: data.id = IXISA_DI_B;	break;
		    case 2: data.id = IXISA_DI_C;	break;
		    case 3: data.id = IXISA_DI_D;	break;
		    case 4: data.id = IXISA_DI_E;	break;
		    case 5: data.id = IXISA_DI_F;	break;
		    case 6: data.id = IXISA_DI_G;	break;
		    case 7: data.id = IXISA_DI_H;	break;
		}
		break;
	    case 1:		//1-IXISA_DIL(H)
		if((ePrm->dev.DI&0xFF) == 1) data.id = IXISA_DI;
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_DIL;	break;
		    case 1: data.id = IXISA_DIH;	break;
		}
		break;
	    case 2:		//2-IXISA_DIO_A(N)
		if(modType(p->modTp.getS()) == "ISO-730")
		    switch(i_ch)
		    {
			case 0: data.id = IXISA_IDIO_A;	break;
			case 1: data.id = IXISA_IDIO_B;	break;
			case 2: data.id = IXISA_DIO_A;	break;
			case 3: data.id = IXISA_DIO_B;	break;
		    }
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_DIO_A;	break;
		    case 1: data.id = IXISA_DIO_B;	break;
		    case 2: data.id = IXISA_DIO_C;	break;
		    case 3: data.id = IXISA_DIO_D;	break;
		}
		break;
	}
	int rez = ioctl(ePrm->devFd, IXISA_READ_REG, &data);
	for(int i_i = 0; i_i < 8; i_i++)
	    p->vlAt(TSYS::strMess("i%d_%d",i_ch,i_i)).at().setB(rez?EVAL_BOOL:((p->dInOutRev[i_ch]^data.value)>>i_i)&1, 0, true);
    }
}

void da_ISA::vlSet( TMdPrm *p, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    tval *ePrm = (tval*)p->extPrms;
    if(ePrm->devFd < 0) return;

    ResAlloc res(p->owner().pBusRes, true);

    //> AO processing
    if(vo.name().compare(0,2,"ao") == 0 && ePrm->dev.AO)
    {
	int i_ch = atoi(vo.name().c_str()+2);
	ixisa_reg_t reg;

	if(ePrm->dev.AO == 1)	reg.id = IXISA_AO;
	else switch(i_ch)
	{
	    case 0:	reg.id = IXISA_AO0;	break;
	    case 1:	reg.id = IXISA_AO1;	break;
	    case 2:	reg.id = IXISA_AO2;	break;
	    case 3:	reg.id = IXISA_AO3;	break;
	    case 4:	reg.id = IXISA_AO4;	break;
	    case 5:	reg.id = IXISA_AO5;	break;
	    case 6:	reg.id = IXISA_AO6;	break;
	    case 7:	reg.id = IXISA_AO7;	break;
	}
	reg.value = vl.getI();
	ioctl(ePrm->devFd, IXISA_WRITE_REG, &reg);
    }

    //> DIO processing
    if(vo.name().compare(0,1,"o") == 0 && ePrm->dev.DIO)
    {
	int i_ch = 0, i_p = 0;
	if(sscanf(vo.name().c_str(),"o%d_%d",&i_ch,&i_p) != 2) return;
	i_p = i_p/8;
	ixisa_reg_t data;
	switch(i_p)
	{
	    case 0:	//Port A
		if(ePrm->dev.DIO == 1)	data.id = IXISA_PA;
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_CN0PA;	break;
		    case 1: data.id = IXISA_CN1PA;	break;
		    case 2: data.id = IXISA_CN2PA;	break;
		    case 3: data.id = IXISA_CN3PA;	break;
		    case 4: data.id = IXISA_CN4PA;	break;
		    case 5: data.id = IXISA_CN5PA;	break;
		}
		break;
	    case 1:	//Port B
		if(ePrm->dev.DIO == 1)	data.id = IXISA_PB;
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_CN0PB;	break;
		    case 1: data.id = IXISA_CN1PB;	break;
		    case 2: data.id = IXISA_CN2PB;	break;
		    case 3: data.id = IXISA_CN3PB;	break;
		    case 4: data.id = IXISA_CN4PB;	break;
		    case 5: data.id = IXISA_CN5PB;	break;
		}
		break;
	    case 2:	//Port C
		if(ePrm->dev.DIO == 1)	data.id = IXISA_PC;
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_CN0PC;	break;
		    case 1: data.id = IXISA_CN1PC;	break;
		    case 2: data.id = IXISA_CN2PC;	break;
		    case 3: data.id = IXISA_CN3PC;	break;
		    case 4: data.id = IXISA_CN4PC;	break;
		    case 5: data.id = IXISA_CN5PC;	break;
		}
		break;
	}
	data.value = 0;
	for(int i_o = 7; i_o >= 0; i_o--)
	{
	    data.value = data.value << 1;
	    if(p->vlAt(TSYS::strMess("o%d_%d",i_ch,i_p*8+i_o)).at().getB(0, true)) data.value |= 1;
	}
	data.value ^= (p->dInOutRev[i_ch]>>(i_p*8))&0xFF;
	ioctl(ePrm->devFd, IXISA_WRITE_REG, &data);
    }

    //> DO processing
    if(vo.name().compare(0,1,"o") == 0 && ePrm->dev.DO)
    {
	int i_ch = 0, i_p = 0;
	if(sscanf(vo.name().c_str(),"o%d_%d",&i_ch,&i_p) != 2) return;
	ixisa_reg_t data;

	switch(ePrm->dev.DI>>8)
	{
	    case 0:		//IXISA_DO_A(N)
		switch(i_ch)
		{
		    case 0: data.id = IXISA_DO_A;	break;
		    case 1: data.id = IXISA_DO_B;	break;
		    case 2: data.id = IXISA_DO_C;	break;
		    case 3: data.id = IXISA_DO_D;	break;
		    case 4: data.id = IXISA_DO_E;	break;
		    case 5: data.id = IXISA_DO_F;	break;
		    case 6: data.id = IXISA_DO_G;	break;
		    case 7: data.id = IXISA_DO_H;	break;
		}
		break;
	    case 1:		//1-IXISA_DOL(H)
		if((ePrm->dev.DI&0xFF) == 1)	data.id = IXISA_DO;
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_DOL;	break;
		    case 1: data.id = IXISA_DOH;	break;
		}
		break;
		case 2:		//2-IXISA_DIO_A(N)
		if(modType(p->modTp.getS()) == "ISO-730")
		    switch(i_ch)
		    {
			case 0: data.id = IXISA_IDIO_A;	break;
			case 1: data.id = IXISA_IDIO_B;	break;
			case 2: data.id = IXISA_DIO_A;	break;
			case 3: data.id = IXISA_DIO_B;	break;
		    }
		else switch(i_ch)
		{
		    case 0: data.id = IXISA_DIO_A;	break;
		    case 1: data.id = IXISA_DIO_B;	break;
		    case 2: data.id = IXISA_DIO_C;	break;
		    case 3: data.id = IXISA_DIO_D;	break;
		}
		break;
	}
	data.value = 0;
	for(int i_o = 7; i_o >= 0; i_o--)
	{
	    data.value = data.value << 1;
	    if(p->vlAt(TSYS::strMess("o%d_%d",i_ch,i_o)).at().getB(0, true)) data.value |= 1;
	}
	data.value ^= p->dInOutRev[i_ch+(ePrm->dev.DI&0xFF)];
	ioctl(ePrm->devFd, IXISA_WRITE_REG, &data);
    }
}

bool da_ISA::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    DevFeature dev = devs[modType(p->modTp.getS())];

    if(opt->name() == "info")
    {
	if((dev.AI || dev.DIO || dev.DI || dev.DO) && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	{
	    //>> AI processing
	    if(dev.AI)
	    {
		p->ctrMkNode("fld",opt,-1,"/cfg/aiTm",_("Analog input settle timeout (us)"),p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
		    "tp","dec","min","0","max","1000000");
		for(unsigned i_ch = 0; i_ch < (dev.AI&0xFF); i_ch++)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/aiG%d",i_ch).c_str(),TSYS::strMess(_("AI %d gain"),i_ch).c_str(),
			p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","dec","min","0","max","100");
	    }

	    //>> DIO processing
	    for(unsigned i_ch = 0; i_ch < dev.DIO; i_ch++)
	    {
		p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/chnOut%d",i_ch).c_str(),TSYS::strMess(_("Channel %d out"),i_ch).c_str(),
		    p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		for(unsigned i_n = 0; i_n < 24; i_n++)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/nRevs%d_%d",i_ch,i_n).c_str(),
			(i_n==0)?TSYS::strMess(_("IO %d reverse"),i_ch).c_str():"",
			p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }

	    //>> DI and DO processing
	    if(dev.DI || dev.DO)
	    {
		for(unsigned i_ch = 0; i_ch < ((dev.DI&0xFF)+(dev.DO&0xFF)); i_ch++)
		    for(unsigned i_n = 0; i_n < 8; i_n++)
			p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/nRevs%d_%d",i_ch,i_n).c_str(), (i_n==0) ?
			    ((i_ch < (dev.DI&0xFF)) ? TSYS::strMess(_("DI %d reverse"),i_ch).c_str() :
						      TSYS::strMess(_("DO %d reverse"),i_ch-(dev.DI&0xFF)).c_str()) : "",
			    p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	}
	return true;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    //>> AI processing
    if(dev.AI)
    {
	if(a_path == "/cfg/aiTm")
	{
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(i2s(atoi(p->modPrm("aiTm","200").c_str())));
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) p->setModPrm("aiTm",opt->text());
	}
	else if(a_path.compare(0,8,"/cfg/aiG") == 0)
	{
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(i2s(atoi(p->modPrm("aiGain."+a_path.substr(8)).c_str())));
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) p->setModPrm("aiGain."+a_path.substr(8),opt->text());
	}
	else return false;
    }
    //> DIO, DI and DO processing
    else if(dev.DIO || dev.DI || dev.DO)
    {
	if(a_path.compare(0,11,"/cfg/chnOut") == 0)
	{
	    int rout = atoi(a_path.c_str()+11);
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(atoi(p->modPrm("DirectDIO").c_str())&(1<<rout)?"1":"0");
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		p->setModPrm("DirectDIO",i2s(atoi(opt->text().c_str()) ? atoi(p->modPrm("DirectDIO").c_str()) | (1<<rout) :
									 atoi(p->modPrm("DirectDIO").c_str()) & ~(1<<rout)));
	}
	else if(a_path.compare(0,10,"/cfg/nRevs") == 0)
	{
	    int i_ch = 0, i_n = 0;
	    sscanf(a_path.c_str(),"/cfg/nRevs%d_%d",&i_ch,&i_n);
	    int chVl = atoi(p->modPrm("dIORev"+i2s(i_ch)).c_str());
	    if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText((chVl&(1<<i_n))?"1":"0");
	    if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		p->setModPrm("dIORev"+i2s(i_ch), i2s(atoi(opt->text().c_str()) ? (chVl|(1<<i_n)) : (chVl & ~(1<<i_n))));
	}
	else return false;
    }
    else return false;

    return true;
}


//OpenSCADA system module DAQ.AMRDevs file: da_FlowTEC.cpp
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
#include "da_FlowTEC.h"

using namespace AMRDevs;

//*************************************************
//* FlowTEC                                       *
//*************************************************
FlowTEC::FlowTEC( TMdPrm *prm ) : DA(prm), mVer(10), mThread(1)
{
    devAddr = vmin(255,vmax(1,atoi(mPrm->devAddr().c_str())));
    mVer = vmin(100,vmax(1,atoi(mPrm->extPrmGet("FirmwareVer").c_str())));
    mThread = vmin(3,vmax(1,atoi(mPrm->extPrmGet("Thread").c_str())));

    //> Values structure
    mPrm->p_el.fldAdd( new TFld("val",_("Value"),TFld::Real,TFld::NoWrite) );
    mPrm->p_el.fldAdd( new TFld("p",_("Pressure"),TFld::Real,TFld::NoWrite) );
    mPrm->p_el.fldAdd( new TFld("t",_("Temperature"),TFld::Real,TFld::NoWrite) );
    if( mVer < 35 )
    {
	mPrm->p_el.fldAdd( new TFld("curF",_("Current flow"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("qntDayStart",_("Quantity for start day"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("qntDayLast",_("Quantity for last day"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("qntStart",_("Quantity for start"),TFld::Real,TFld::NoWrite) );
    }
    else
    {
	mPrm->p_el.fldAdd( new TFld("cal",_("Caloric"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("curF",_("Current flow"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("qntDayStart",_("Quantity for start day"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("qntDayLast",_("Quantity for last day"),TFld::Real,TFld::NoWrite) );
	mPrm->p_el.fldAdd( new TFld("qntStart",_("Quantity for start"),TFld::Real,TFld::NoWrite) );
    }
}

FlowTEC::~FlowTEC( )
{

}

void FlowTEC::getVals( )
{
    char szReceive[255];

    //> Connect to transport
    AutoHD<TTransportOut> tr = SYS->transport().at().at("Serial").at().outAt(mPrm->addr());
    if(!tr.at().startStat()) tr.at().start();

    //> Prepare request
    string req, rez;
    req.reserve(8);
				//> Prefix
    req += 0xAA;		//Syncbyte
    req += (uint8_t)devAddr;	//Device address
    req += 7;			//Request size
    req += 7;			//Request function (7)

    req += (uint8_t)mThread;	//Thread
    uint16_t crc = mod->CRC16(req);
    req += (crc>>8);
    req += crc;

    //> Send request
    bool errPresent = true;
    ResAlloc resN( tr.at().nodeRes(), true );
    for( int i_tr = 0; i_tr < vmax(1,vmin(10,mPrm->owner().connTry())); i_tr++ )
    {
	try
	{
	    int resp_len = tr.at().messIO( req.data(), req.size(), szReceive, sizeof(szReceive), 0, true );
	    rez.assign( szReceive, resp_len );

	    //> Wait tail
	    while(resp_len && (rez.size() < 4 || rez.size() < (uint8_t)rez[2]))
	    {
		try{ resp_len = tr.at().messIO( NULL, 0, szReceive, sizeof(szReceive), 0, true ); } catch(TError er){ break; }
		rez.append(szReceive, resp_len);
	    }
	    if( rez.size() < 4 || rez.size() < (uint8_t)rez[2] ) continue;
	    errPresent = false;
	    break;
	}
	catch(...) { continue; }
    }

    //> Respond process
    if( errPresent )						mPrm->mErr.setVal(_("14:Connection error or respond size short."));
    else if( (uint8_t)rez[0] != 0x55 || rez[1] != req[1] )	mPrm->mErr.setVal(_("10:Respond error."));
    else if( (uint8_t)rez[3] == 0xff )				mPrm->mErr.setVal(_("11:Request broken."));
    else if( rez.size() < 40 || rez[4] != req[4] ) 		mPrm->mErr.setVal(_("15:Respond thread broken."));
    {
	mPrm->vlAt("val").at().setR(TSYS::getUnalignFloat(rez.data()+5),0,true);
	mPrm->vlAt("p").at().setR(TSYS::getUnalignFloat(rez.data()+9),0,true);
	mPrm->vlAt("t").at().setR(TSYS::getUnalignFloat(rez.data()+13),0,true);
	if( mVer < 35 )
	{
	    mPrm->vlAt("curF").at().setR(TSYS::getUnalignFloat(rez.data()+17),0,true);
	    mPrm->vlAt("qntDayStart").at().setR(TSYS::getUnalignFloat(rez.data()+21),0,true);
	    mPrm->vlAt("qntDayLast").at().setR(TSYS::getUnalignFloat(rez.data()+25),0,true);
	    mPrm->vlAt("qntStart").at().setR(TSYS::getUnalignFloat(rez.data()+29),0,true);
	}
	else
	{
	    mPrm->vlAt("cal").at().setR(TSYS::getUnalignFloat(rez.data()+17),0,true);
	    mPrm->vlAt("curF").at().setR(TSYS::getUnalignFloat(rez.data()+21),0,true);
	    mPrm->vlAt("qntDayStart").at().setR(TSYS::getUnalignFloat(rez.data()+25),0,true);
	    mPrm->vlAt("qntDayLast").at().setR(TSYS::getUnalignFloat(rez.data()+29),0,true);
	    mPrm->vlAt("qntStart").at().setR(TSYS::getUnalignFloat(rez.data()+33),0,true);
	}
    }
}

bool FlowTEC::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	mPrm->ctrMkNode("fld",opt,-1,"/prm/cfg/DEV_ADDR",mPrm->cfg("DEV_ADDR").fld().descr().c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","min","1","max","255");
	mPrm->ctrMkNode("fld",opt,-1,"/prm/cfg/Thread",_("Thread number"),RWRWR_,"root",SDAQ_ID,3,"tp","dec","min","1","max","3");
	mPrm->ctrMkNode("fld",opt,-1,"/prm/cfg/FirmWareVer",_("Firmware version"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/Thread")
    {
	if(mPrm->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(mPrm->extPrmGet("Thread"));
	if(mPrm->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	mPrm->extPrmSet("Thread",opt->text());
    }
    else if(a_path == "/prm/cfg/FirmWareVer")
    {
	if(mPrm->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(mPrm->extPrmGet("FirmwareVer"));
	if(mPrm->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	mPrm->extPrmSet("FirmwareVer",opt->text());
    }
    else return false;

    return true;
}


//OpenSCADA module DAQ.Siemens file: siemens.cpp
/***************************************************************************
 *   Copyright (C) 2006-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <signal.h>
#include <string.h>

#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include "rcsdef.h"
#include "cif_dpm.h"
#include "rcs_user.h"
#include "cif_user.h"

#include "openSocket.h"

#include "siemens.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"Siemens"
#define MOD_NAME	_("Siemens DAQ and Beckhoff")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"3.0.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides for support of data sources of Siemens PLCs by means of Hilscher CIF cards (using the MPI protocol)\
 and LibnoDave library (or the own implementation) for the rest. Also there is supported the data sources of the firm Beckhoff for the\
 protocol TwinCAT ADS/AMS due it working with data blocks also.")
#define LICENSE		"GPL2"
//************************************************

Siemens::TTpContr *Siemens::mod;	//Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod ) {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source ) {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new Siemens::TTpContr(source);
	return NULL;
    }
}

using namespace Siemens;

//************************************************
//* TTpContr                                     *
//************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(MOD_ID), drvCIF_OK(false)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr( )
{
    for(int iB = 0; iB < MAX_DEV_BOARDS; iB++)
	if(cif_devs[iB].present) DevExitBoard(iB);
    if(drvCIFOK())	DevCloseDriver();
}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's DB structure
    fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",_("Restore timeout, seconds"),TFld::Integer,TFld::NoFlag,"4","30","1;3600"));
    fldAdd(new TFld("ASINC_WR",_("Asynchronous write mode"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("TYPE",_("Connection type"),TFld::Integer,TFld::Selectable,"1","0",
	TSYS::strMess("%d;%d;%d;%d;%d",TMdContr::CIF_PB,TMdContr::ISO_TCP,TMdContr::ISO_TCP243,TMdContr::ADS,TMdContr::SELF_ISO_TCP).c_str(),
	"CIF_PB;ISO_TCP (LibnoDave);ISO_TCP243 (LibnoDave);ADS;ISO_TCP"));
    fldAdd(new TFld("ADDR",_("Remote controller address"),TFld::String,TFld::NoFlag,"100","10"));
    fldAdd(new TFld("ADDR_TR",_("Output transport"),TFld::String,TFld::NoFlag,"40"));
    fldAdd(new TFld("SLOT",_("CPU slot of the PLC"),TFld::Integer,TFld::NoFlag,"2","2","0;30"));
    fldAdd(new TFld("CIF_DEV",_("CIF board"),TFld::Integer,TFld::NoFlag,"1","0","0;3"));
    // Parameter type DB structure
    int t_prm = tpParmAdd("logic","PRM_BD",_("Logical"));
    tpPrmAt(t_prm).fldAdd( new TFld("TMPL",_("Parameter template"),TFld::String,TCfg::NoVal,"50","") );
    // Parameter template IO DB structure
    elPrmIO.fldAdd(new TFld("PRM_ID",_("Parameter ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elPrmIO.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elPrmIO.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::NoFlag,"200"));

    //CIF devices DB structure
    elCifDev.fldAdd(new TFld("ID",_("Identifier"),TFld::Integer,TCfg::Key,"1"));
    elCifDev.fldAdd(new TFld("ADDR",_("Address"),TFld::Integer,TFld::NoFlag,"3","5"));
    elCifDev.fldAdd(new TFld("SPEED",_("Speed"),TFld::Integer,TFld::NoFlag,"1","7"));

    //Clear CIF devices info
    for(int iB = 0; iB < MAX_DEV_BOARDS; iB++) {
	cif_devs[iB].present = false;
	cif_devs[iB].board = -1;
	cif_devs[iB].phAddr = 0;
	cif_devs[iB].irq = 0;
	cif_devs[iB].fwname  = _("No device");
	cif_devs[iB].fwver = "";
	cif_devs[iB].pbspeed = cif_devs[iB].pbaddr = 0;
    }
}

void TTpContr::load_( )
{
    //Load parameters from command line

    //Load CIF devices configuration
    TConfig cfg(&CIFDevE());
    string bd_tbl = modId()+"_CIFdevs";
    for(int iB = 0; iB < MAX_DEV_BOARDS; iB++) {
	cfg.cfg("ID").setI(iB);
	if(SYS->db().at().dataGet(SYS->workDB()+"."+bd_tbl,mod->nodePath()+bd_tbl,cfg,false,true)) {
	    cif_devs[iB].pbaddr = cfg.cfg("ADDR").getI();
	    cif_devs[iB].pbspeed = cfg.cfg("SPEED").getI();
	}
	if(drvCIFOK())	initCIF(iB);
    }
}

void TTpContr::save_( )
{
    //Save CIF devices configuration
    TConfig cfg(&CIFDevE());
    string bd_tbl = modId()+"_CIFdevs";
    for(int iB = 0; iB < MAX_DEV_BOARDS; iB++) {
	cfg.cfg("ID").setI(iB);
	cfg.cfg("ADDR").setI(cif_devs[iB].pbaddr);
	cfg.cfg("SPEED").setI(cif_devs[iB].pbspeed);
	SYS->db().at().dataSet(SYS->workDB()+"."+bd_tbl, mod->nodePath()+bd_tbl, cfg);
    }
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name,daq_db,this); }

bool TTpContr::drvCIFOK( )
{
    if(drvCIF_OK) return drvCIF_OK;
    drvCIF_OK = (DevOpenDriver()==DRV_NO_ERROR);

    //Load CIF boards configuration
    if(drvCIF_OK) {
	BOARD_INFO brd_info;
	memset(&brd_info,0,sizeof(BOARD_INFO));
	if(DevGetBoardInfo(&brd_info) == DRV_NO_ERROR)
	    for(int iB = 0; iB < MAX_DEV_BOARDS; iB++) {
		cif_devs[iB].present = (brd_info.tBoard[iB].usAvailable==1);
		if(!cif_devs[iB].present) continue;
		cif_devs[iB].board = brd_info.tBoard[iB].usBoard;
		cif_devs[iB].phAddr = brd_info.tBoard[iB].ulPhysicalAddress;
		cif_devs[iB].irq = brd_info.tBoard[iB].usIrq;
		unsigned char fwInfo[300];
		if(DevGetInfo(iB,GET_FIRMWARE_INFO,sizeof(FIRMWAREINFO),&fwInfo[0]) == DRV_NO_ERROR) {
		    cif_devs[iB].fwname.assign((char *)&fwInfo[0],16);
		    cif_devs[iB].fwver.assign((char *)&fwInfo[16],16);
		}
		DevInitBoard(iB);
	    }
    }
    return drvCIF_OK;
}

void TTpContr::initCIF( int dev )
{
    int sRet;
    RCS_MESSAGE tMsg;
    DPM_PLC_PARAMETER DPParameter;
    DRIVERINFO tDriverInfo;
    DPM_DIAGNOSTICS tTaskState;
    DDLM_DOWNLOAD_REQUEST *ptDownloadRequest;
    DPM_BUS_DP *ptBusDpm;

    struct Spar_bus {
	unsigned char	bMax_Retry_Limit;
	unsigned char	bTQUI;
	unsigned short	usTSL;
	unsigned char	bTSET;
	unsigned short	usMin_TSDR;
	unsigned short	usMax_TSDR;
	unsigned long	ulTTR;
	unsigned char	bG;
    };

    struct Spar_bus par_bus[] = {
	{1,0, 100, 1,11,60 ,11520,10},		//0  9.6
	{1,0, 100, 1,11,60 ,11520,10},		//1  19.2
	{1,0, 100, 1,11,60 ,11520,10},		//2  93.75
	{1,0, 100, 1,11,60 ,11520,10},		//3  187.5
	{1,0, 200, 1,11,100,11520,10},		//4  500
	{1,0, 300, 1,11,150,11520,10},		//5  1000
	{1,0, 300, 1,11,150,11520,10},		//6  1500
	{2,3, 400, 4,11,250,13824, 1},		//7  3000
	{3,6, 600, 8,11,450,13824, 1},		//8  6000
	{4,9,1000,16,11,800,13824, 1},		//9  12000
    };


    if(dev < 0 || dev > MAX_DEV_BOARDS || !drvCIFOK())
	throw TError(nodePath().c_str(), _("Error the CIF device %d or the device driver."), dev);
    if(!cif_devs[dev].present)	return;

    ResAlloc resource(cif_devs[dev].res, true);

    // Load parameters to board
    //  Running board aplications
    if((sRet = DevSetHostState(dev,HOST_READY,0L)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("Error starting up the CIF device %d (DevSetHostState(HOST_READY))."), dev);
    //  Load the protocol task parameters
    memset(&DPParameter, 0, sizeof(DPM_PLC_PARAMETER));
    DPParameter.bMode = DPM_SET_MODE_UNCONTROLLED;
    DPParameter.bFormat = 1;
    DPParameter.usWatchDogTime = 1000;
    if((sRet=DevPutTaskParameter(dev,1,sizeof(DPM_PLC_PARAMETER),&DPParameter)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("DevPutTaskParameter: error the CIF device %d."), dev);
    //   Reset CP for apply parameters
    DevReset(dev, WARMSTART, 5000L);
    //  Download the bus parameter
    //   Delete The static data base if present
    DevGetInfo(dev, GET_DRIVER_INFO, sizeof(DRIVERINFO), &tDriverInfo);
    if(tDriverInfo.bHostFlags&0x40) {
	tMsg.rx = 0;			// task = OS-Task (RCS-Task)
	tMsg.tx = 16;
	tMsg.ln = 2;
	tMsg.nr = 0;
	tMsg.a  = 0;
	tMsg.f  = 0;
	tMsg.b  = RCS_B_LOADFKT;	// command data base access
	tMsg.e  = 0;
	tMsg.d[0] = 4;			// mode clear data base
	tMsg.d[1] = 8;			// startsegment = 8
	if((sRet=DevPutMessage(dev,(MSG_STRUC*)&tMsg,200L)) != DRV_NO_ERROR)
	    throw TError(nodePath().c_str(), _("DevPutMessage: error the CIF device %d."), dev);
	if((sRet=DevGetMessage(dev,sizeof(tMsg),(MSG_STRUC *)&tMsg,200L)) != DRV_NO_ERROR)
	    throw TError(nodePath().c_str(), _("DevGetMessage: error the CIF device %d."), dev);
    }
    //   Load new bus parameters
    tMsg.rx = 3;			// task = DPM-Task
    tMsg.tx = 16;
    tMsg.ln = sizeof(DDLM_DOWNLOAD_REQUEST);
    tMsg.nr = 0;
    tMsg.a  = 0;
    tMsg.f  = 0;
    tMsg.b  = DDLM_Download;
    tMsg.e  = 0;
    ptDownloadRequest = (DDLM_DOWNLOAD_REQUEST*) &tMsg.d;
    ptDownloadRequest->bReq_Add = 0;
    ptDownloadRequest->bArea_Code = DPM_DEVICE_PRM;
    ptDownloadRequest->usAdd_Offset = 0;
    ptBusDpm = (DPM_BUS_DP*)&ptDownloadRequest->abData;
    ptBusDpm->usBus_Para_Len = sizeof(DPM_BUS_DP);
    ptBusDpm->bFDL_Add		= cif_devs[dev].pbaddr;				//Own master address
    ptBusDpm->bBaudrate		= cif_devs[dev].pbspeed;			//Specified profibus the baud rate
    ptBusDpm->usTSL		= par_bus[cif_devs[dev].pbspeed].usTSL;		//300 (37-16383);
    ptBusDpm->usMin_TSDR	= par_bus[cif_devs[dev].pbspeed].usMin_TSDR;	//11 (1-1023);
    ptBusDpm->usMax_TSDR	= par_bus[cif_devs[dev].pbspeed].usMax_TSDR;	//150 (1-1023);
    ptBusDpm->bTQUI		= par_bus[cif_devs[dev].pbspeed].bTQUI;		//0 (0-127);
    ptBusDpm->bTSET		= par_bus[cif_devs[dev].pbspeed].bTSET;		//1 (1-255);
    ptBusDpm->ulTTR		= par_bus[cif_devs[dev].pbspeed].ulTTR;		//2021 (255-...);
    ptBusDpm->bG		= par_bus[cif_devs[dev].pbspeed].bG;		//10 (1-255);
    ptBusDpm->bHSA		= 126;
    ptBusDpm->bMax_Retry_Limit	= par_bus[cif_devs[dev].pbspeed].bMax_Retry_Limit; //1 (0-7);
    ptBusDpm->Bp_Flag.bReserved = 0;
    ptBusDpm->Bp_Flag.bError_Action_Flag	= 0;
    ptBusDpm->usMin_Slave_Intervall		= 0;
    ptBusDpm->usPoll_Timeout	= 10;						//(0-65535)
    ptBusDpm->usData_Control_Time		= 120;				//(1-65535)
    ptBusDpm->bAlarm_Max	= 0;
    ptBusDpm->bMax_User_Global_Control = 0;
    ptBusDpm->abOctet[0] = 0;
    ptBusDpm->abOctet[1] = 0;
    ptBusDpm->abOctet[2] = 0;
    ptBusDpm->abOctet[3] = 0;
    tMsg.ln = sizeof(DPM_BUS_DP)+sizeof(DDLM_DOWNLOAD_REQUEST)- DPM_MAX_LEN_DATA_UNIT;
    if((sRet=DevPutMessage(dev,(MSG_STRUC *)&tMsg, 200L))!=DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("Error sending message: %d."),sRet);
    if((sRet = DevGetMessage(dev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg, 200L))!=DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("Error getting message: %d."),sRet);
    //   Wait for operation start with new parameters of PLC task
    do {
	DevGetTaskState(dev, 2, sizeof(tTaskState), &tTaskState);
	TSYS::sysSleep(1e-3);
    } while(tTaskState.bDPM_state != OPERATE);
}

void TTpContr::getLifeListPB( unsigned board, string &buffer )
{
    RCS_MESSAGE tMsg;
    RCS_TELEGRAMHEADER_10 *ptRcsTelegramheader10;
    int res;

    if(!cif_devs[board].present) throw TError(nodePath().c_str(), _("%d:The board %d is not present."), 15, board);

    ResAlloc resource(cif_devs[board].res, true);

    //Prepare request
    tMsg.rx = 7;	// task = FDL-Task
    tMsg.tx = 16;
    tMsg.ln = sizeof(RCS_TELEGRAMHEADER_10);
    tMsg.nr = 0;
    tMsg.a  = 0;
    tMsg.f  = 0;
    tMsg.b  = DDLM_Life_List;
    tMsg.e  = 0;
    ptRcsTelegramheader10 = (RCS_TELEGRAMHEADER_10*)&tMsg.d;
    ptRcsTelegramheader10->device_adr = 0;
    ptRcsTelegramheader10->data_area  = 0;
    ptRcsTelegramheader10->data_adr   = 0;
    ptRcsTelegramheader10->data_idx   = 0;
    ptRcsTelegramheader10->data_cnt   = DPM_MAX_NUM_DEVICES;
    ptRcsTelegramheader10->data_idx   = 0;
    ptRcsTelegramheader10->data_type  = TASK_TDT_STRING;
    ptRcsTelegramheader10->function   = TASK_TFC_READ;

    //Put message
    if((res=DevPutMessage(board,(MSG_STRUC*)&tMsg,500L)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("%d:Error sending request: %d."), 12, res);
    if((res=DevGetMessage(board,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg, 200L)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("%d:Error getting request: %d."), 13, res);

    buffer.assign((char*)&tMsg.d[sizeof(RCS_TELEGRAMHEADER_10)], DPM_MAX_NUM_DEVICES+1);
}

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTypeDAQ::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/mod",_("CIF"))) {
	    if(ctrMkNode("area",opt,-1,"/mod/st",_("Status")))
		ctrMkNode("fld",opt,-1,"/mod/st/drv",_("CIF driver"),R_R_R_,"root",SDAQ_ID,1,"tp","bool");
	    if(ctrMkNode("table",opt,-1,"/mod/dev",_("CIF devices"),RWRWR_,"root",SDAQ_ID)) {
		ctrMkNode("list",opt,-1,"/mod/dev/brd",_("Board"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/fwnm",_("Firmware name"),R_R_R_,"root",SDAQ_ID,1,"tp","real");
		ctrMkNode("list",opt,-1,"/mod/dev/fwver",_("Firmware version"),R_R_R_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/phAddr",_("Physical address"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/irq",_("IRQ"),R_R_R_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/addr",_("PB address"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/speed",_("PB speed"),RWRWR_,"root",SDAQ_ID,4,"tp","dec","idm","1","dest","select","select","/mod/dev/lsspd");
	    }
	}
	if(ctrMkNode("area",opt,1,"/PB","ProfiBus")) {
	    ctrMkNode("fld",opt,-1,"/PB/dev",_("CIF device"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
	    ctrMkNode("list",opt,-1,"/PB/lifels",_("Live stations list"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/mod/st/drv" && ctrChkNode(opt))	opt->setText(i2s(drvCIFOK()));
    else if(a_path == "/mod/dev") {
	if(ctrChkNode(opt)) {
	    // Fill Archives table
	    XMLNode *n_brd  = ctrMkNode("list",opt,-1,"/mod/dev/brd","");
	    XMLNode *n_fwnm = ctrMkNode("list",opt,-1,"/mod/dev/fwnm","");
	    XMLNode *n_fwver= ctrMkNode("list",opt,-1,"/mod/dev/fwver","");
	    XMLNode *n_phAddr = ctrMkNode("list",opt,-1,"/mod/dev/phAddr","");
	    XMLNode *n_irq  = ctrMkNode("list",opt,-1,"/mod/dev/irq","");
	    XMLNode *n_pba  = ctrMkNode("list",opt,-1,"/mod/dev/addr","");
	    XMLNode *n_pbspd= ctrMkNode("list",opt,-1,"/mod/dev/speed","");

	    for(int iB = 0; iB < MAX_DEV_BOARDS; iB++) {
		if(n_brd)	n_brd->childAdd("el")->setText(i2s(cif_devs[iB].board));
		if(n_fwnm)	n_fwnm->childAdd("el")->setText(cif_devs[iB].fwname);
		if(n_fwver)	n_fwver->childAdd("el")->setText(cif_devs[iB].fwver);
		if(n_phAddr)	n_phAddr->childAdd("el")->setText(i2s(cif_devs[iB].phAddr,TSYS::Hex));
		if(n_irq)	n_irq->childAdd("el")->setText(i2s(cif_devs[iB].irq));
		if(n_pba)	n_pba->childAdd("el")->setText(i2s(cif_devs[iB].pbaddr));
		if(n_pbspd)	n_pbspd->childAdd("el")->setText(i2s(cif_devs[iB].pbspeed));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int brd = s2i(opt->attr("row"));
	    string col  = opt->attr("col");
	    if(brd < 0 || brd > 3)	throw TError(nodePath().c_str(), _("Error the board %d."), brd);
	    if(col == "addr") {
		int addr = s2i(opt->text());
		cif_devs[brd].pbaddr = (addr<0) ? 0 : (addr>126)?126:addr;
	    }
	    else if(col == "speed")	cif_devs[brd].pbspeed = s2i(opt->text());
	    if(col == "addr" || col == "speed") initCIF(brd);
	    modif();
	}
    }
    else if(a_path == "/mod/dev/lsspd" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","0")->setText(_("9600Baud"));
	opt->childAdd("el")->setAttr("id","1")->setText(_("19.2kBaud"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("93.75kBaud"));
	opt->childAdd("el")->setAttr("id","3")->setText(_("187.5kBaud"));
	opt->childAdd("el")->setAttr("id","4")->setText(_("500kBaud"));
	opt->childAdd("el")->setAttr("id","6")->setText(_("1.5MBaud"));
	opt->childAdd("el")->setAttr("id","7")->setText(_("3MBaud"));
	opt->childAdd("el")->setAttr("id","8")->setText(_("6MBaud"));
	opt->childAdd("el")->setAttr("id","9")->setText(_("12MBaud"));
    }
    else if(a_path == "/PB/dev") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"lifeLsDev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"lifeLsDev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/PB/lifels" && ctrChkNode(opt)) {
	int board = s2i(TBDS::genDBGet(mod->nodePath()+"lifeLsDev","0",opt->attr("user")));
	string lifeLst;
	try {
	    getLifeListPB(board, lifeLst);
	    for(unsigned i_st = 0; i_st < lifeLst.size(); i_st++)
		switch((unsigned char)lifeLst[i_st]) {
		    case 0xFF:	opt->childAdd("el")->setText(i2s(i_st)+_(" : -------"));	break;
		    case 0x30:	opt->childAdd("el")->setText(i2s(i_st)+_(" : Active station"));	break;
		    case 0x00:	opt->childAdd("el")->setText(i2s(i_st)+_(" : Passive station"));break;
		}
	} catch(TError &err) { opt->childAdd("el")->setText(err.mess); }
    }
    else TTypeDAQ::cntrCmdProc(opt);
}

//************************************************
//* TMdContr                                     *
//************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem ) :
	::TController(name_c, daq_db, cfgelem),
	mPrior(cfg("PRIOR").getId()), mType(cfg("TYPE").getId()),
	mSlot(cfg("SLOT").getId()), mDev(cfg("CIF_DEV").getId()), restTm(cfg("TM_REST").getId()), mAssincWR(cfg("ASINC_WR").getBd()),
	prcSt(false), callSt(false), endrunReq(false), isReload(false), isInitiated(false), alSt(-1), conErr(dataRes()), mInvokeID(-1),
	di(NULL), dc(NULL), enRes(true), mPer(1e9), numR(0), numW(0), numErr(0), tmDelay(0)
{
    cfg("PRM_BD").setS("SiemensPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

void TMdContr::postDisable( int flag )
{
    TController::postDisable(flag);
    try {
	if(flag) {
	    //Delete parameter's io table
	    string tbl = DB()+"."+cfg("PRM_BD").getS()+"_io";
	    SYS->db().at().open(tbl);
	    SYS->db().at().close(tbl, true);
	}
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if(startStat() && !redntUse()) {
	if(!prcSt) rez += TSYS::strMess(_("Task is terminated! "));
	if(tmDelay > -1) {
	    rez += (conErr.getVal().size() ? conErr.getVal(): string(_("Error connecting."))) + " ";
	    if(tmDelay) rez += TSYS::strMess(_("Restoring in %.6g s. "), tmDelay);
	    rez.replace(0, 1, i2s(ConnErrCode));
	}
	else {
	    if(callSt)	rez += TSYS::strMess(_("Acquisition. "));
	    if(period())rez += TSYS::strMess(_("Acquisition with the period: %s. "), tm2s(1e-9*period()).c_str());
	    else rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "), atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s[%s]. Read %s. Wrote %s. Connection errors %g. "),
			tm2s(SYS->taskUtilizTm(nodePath('.',true))).c_str(), tm2s(SYS->taskUtilizTm(nodePath('.',true),true)).c_str(),
			TSYS::cpct2str(numR).c_str(), TSYS::cpct2str(numW).c_str(), numErr);
	}
    }

    return rez;
}

TTpContr &TMdContr::owner( ) const	{ return *(TTpContr*)nodePrev(); }

TParamContr *TMdContr::ParamAttach( const string &name, int type ) { return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    //TController::load_();
}

void TMdContr::save_( )
{
    TController::save_();
}

void TMdContr::enable_( )
{

}

void TMdContr::disable_( )
{
    //Clear acquisition data blocks and asynchronous write mode data blocks
    reqDataRes.resRequestW(true); acqBlks.clear(); reqDataRes.resRelease();
    reqDataAsWrRes.resRequestW(true); writeBlks.clear(); reqDataAsWrRes.resRelease();
}

void TMdContr::start_( )
{
    if(prcSt) return;

    connectRemotePLC(true);

    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,1e9*s2r(cron())) : 0;

    //Clear acquisition data blocks and asynchronous write mode data blocks
    reqDataRes.resRequestW(true); acqBlks.clear(); reqDataRes.resRelease();
    reqDataAsWrRes.resRequestW(true); writeBlks.clear(); reqDataAsWrRes.resRelease();

    //Reenable parameters
    try {
	vector<string> pls;
	list(pls);

	isReload = true;
	for(unsigned i_p = 0; i_p < pls.size(); i_p++)
	    if(at(pls[i_p]).at().enableStat()) at(pls[i_p]).at().enable();
	isReload = false;
    } catch(TError&) { isReload = false; throw; }

    //Counters reset
    numR = numW = numErr = 0;
    tmDelay = 0;

    //Start the request data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrunReq);

    alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),owner().modId().c_str(),id().c_str(),_("STOP")), TMess::Info);
    alSt = -1;

    //Clear proccess parameters list
    enRes.lock(); pHd.clear(); enRes.unlock();

    disconnectRemotePLC( );
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "TYPE" && startStat()) stop();

    return true;
}

void TMdContr::prmEn( const string &id, bool val )
{
    unsigned i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
	if(pHd[i_prm].at().id() == id) break;

    if(val && i_prm >= pHd.size()) pHd.push_back(at(id));
    if(!val && i_prm < pHd.size()) pHd.erase(pHd.begin()+i_prm);
}

void TMdContr::regVal( const string &iaddr, bool wr )
{
    int db = -1, off = -1;
    char tp[11];
    if(sscanf(iaddr.c_str(),"DB%d.%i.%10s",&db,&off,tp) != 3 || db < 0 || off < 0)	return;

    int ivSz = valSize(tp);	//Get real value's size

    ResAlloc res(reqDataRes, true);

    //Register to acquisition block
    unsigned iB;
    bool plcOK = false;
    for(iB = 0; iB < acqBlks.size(); iB++)
	if(db < acqBlks[iB].db) break;
	else if(acqBlks[iB].db == db) {
	    if(off < acqBlks[iB].off) {
		if((acqBlks[iB].val.size()+acqBlks[iB].off-off) < MaxLenReq) {
		    acqBlks[iB].val.insert(0, acqBlks[iB].off-off, 0);
		    acqBlks[iB].off = off;
		}
		else acqBlks.insert(acqBlks.begin()+iB,SDataRec(db,off,ivSz));
	    }
	    else if((off+ivSz) > (acqBlks[iB].off+(int)acqBlks[iB].val.size())) {
		if((off+ivSz-acqBlks[iB].off) < MaxLenReq)
		    acqBlks[iB].val.append((off+ivSz)-(acqBlks[iB].off+acqBlks[iB].val.size()), 0);
		else continue;
	    }
	    plcOK = true;
	    break;
	}
    if(!plcOK)	acqBlks.insert(acqBlks.begin()+iB, SDataRec(db,off,ivSz));
    res.release();

    //Register to asynchronous write block
    if(wr && assincWrite()) {
	ResAlloc res(reqDataAsWrRes, true);
	plcOK = false;
	for(iB = 0; iB < writeBlks.size(); iB++)
	    if(db < writeBlks[iB].db) break;
	    else if(writeBlks[iB].db == db) {
		if(off < writeBlks[iB].off) {
		    if((off+ivSz) >= writeBlks[iB].off &&
			    (writeBlks[iB].val.size()+writeBlks[iB].off-off) < MaxLenReq)
		    {
			writeBlks[iB].val.insert(0,writeBlks[iB].off-off,0);
			writeBlks[iB].off = off;
		    }
		    else writeBlks.insert(writeBlks.begin()+iB,SDataRec(db,off,ivSz));
		}
		else if((off+ivSz) > (writeBlks[iB].off+(int)writeBlks[iB].val.size())) {
		    if(off <= (writeBlks[iB].off+(int)writeBlks[iB].val.size()) &&
			    (off+ivSz-writeBlks[iB].off) < MaxLenReq) {
			writeBlks[iB].val.append((off+ivSz)-(writeBlks[iB].off+writeBlks[iB].val.size()),0);
			//Check for allow mergin to next block
			if(iB+1 < writeBlks.size() && writeBlks[iB+1].db == db &&
				(writeBlks[iB].off+(int)writeBlks[iB].val.size()) >= writeBlks[iB+1].off)
			{
			    writeBlks[iB].val.append(writeBlks[iB+1].val,writeBlks[iB].off+writeBlks[iB].val.size()-writeBlks[iB+1].off,string::npos);
			    writeBlks.erase(writeBlks.begin()+iB+1);
			}
		    }
		    else continue;
		}
		plcOK = true;
		break;
	    }
	if(!plcOK) writeBlks.insert(writeBlks.begin()+iB,SDataRec(db,off,ivSz));
	writeBlks[iB].err = _("-1:No data");
    }
}

void TMdContr::connectRemotePLC( bool initOnly )
{
    switch(mType) {
	case CIF_PB:
	    if(!(owner().cif_devs[0].present||owner().cif_devs[1].present||owner().cif_devs[2].present||owner().cif_devs[3].present))
		throw TError(nodePath().c_str(), _("No one driver or board are present."));
	    break;
	case ISO_TCP:
	case ISO_TCP243: {
	    if(initOnly) break;
	    //Dsconnect previous connection
	    if(dc && di) disconnectRemotePLC();

	    //Full Libnodave API
	    MtxAlloc res1(reqAPIRes, true);
	    _daveOSserialType fds;
	    fds.wfd = fds.rfd = openSocket(102, addr().c_str());
	    if(fds.rfd <= 0) throw TError(nodePath().c_str(), _("Error opening the remote PLC socket."));

	    ResAlloc res2(mod->resAPI, true);
	    di = daveNewInterface(fds, (char*)(string("IF")+id()).c_str(), 0,
		((mType==ISO_TCP243)?daveProtoISOTCP243:daveProtoISOTCP), daveSpeed187k);
	    dc = daveNewConnection(di, 2, 0, mSlot);
	    daveSetTimeout(di, 1500000);	//1.5s
	    if(daveConnectPLC(dc)) {
		close(fds.wfd);
		free(dc); dc = NULL;
		free(di); di = NULL;
		throw TError(nodePath().c_str(), _("Error connecting to the PLC."));
	    }
	    break;
	}
	case ADS:
	case SELF_ISO_TCP:
	    tr = SYS->transport().at().at(TSYS::strParse(addrTr(),0,".")).at().outAt(TSYS::strParse(addrTr(),1,"."));
	    //try { tr.at().start(); }
	    //catch(TError &err) { throw TError(nodePath().c_str(), _("Error connecting.")); }
	    reset();
	    break;
	default: throw TError(nodePath().c_str(), _("The connection type '%d' is not supported."), mType);
    }
}

void TMdContr::disconnectRemotePLC( )
{
    switch(mType) {
	case ISO_TCP:
	case ISO_TCP243: {
	    MtxAlloc res(reqAPIRes, true);
	    ResAlloc res2(mod->resAPI, true);
	    if(!dc || !di) break;
	    daveDisconnectPLC(dc);
	    res2.release();

	    close(di->fd.rfd);
	    free(dc); dc = NULL;
	    free(di); di = NULL;
	    break;
	}
	case ADS:
	case SELF_ISO_TCP:
	    if(tr.freeStat())	break;
	    tr.at().stop();
	    tr.free();
	    break;
    }
}

void TMdContr::getDB( unsigned n_db, long offset, string &buffer )
{
    try {
	switch(mType) {
	    case CIF_PB: {
		RCS_MESSAGE tMsg;
		int res, e_try = 4;

		if(buffer.size() > 240)			throw TError("ReadDB", _("Requested block is too big."));
		if(!owner().cif_devs[mDev].present)	throw TError("ReadDB", _("The board %d is not present."), mDev);

		ResAlloc resource(owner().cif_devs[mDev].res, true);

		do {
		    e_try--;

		    //Clear all previous data
		    while(!DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,0L))	TSYS::sysSleep(1e-3);

		    //Prepare put request
		    tMsg.rx = 3;
		    tMsg.tx = 16;
		    tMsg.ln = 8;
		    tMsg.nr = 0;
		    tMsg.a  = 0;
		    tMsg.f  = 0;
		    tMsg.b  = MPI_Read_Write_DB;
		    tMsg.e  = 0;
		    tMsg.d[0] = vmax(0,vmin(126,s2i(addr())));
		    tMsg.d[1] = (unsigned char)(offset>>8);
		    tMsg.d[2] = (unsigned char)n_db;
		    tMsg.d[3] = (unsigned char)(n_db>>8);
		    tMsg.d[4] = (unsigned char)offset;
		    tMsg.d[5] = (unsigned char)buffer.size();
		    tMsg.d[6] = TASK_TDT_UINT8;
		    tMsg.d[7] = TASK_TFC_READ;

		    //Put message to remote host
		    res = DevPutMessage(mDev, (MSG_STRUC *)&tMsg, 200L);
		    if(res == DRV_DEV_PUT_TIMEOUT) throw TError("ReadDB", _("Timeout sending request."));
		    //Get remote host's response
		    if(res == DRV_NO_ERROR) res = DevGetMessage(mDev, sizeof(RCS_MESSAGE), (MSG_STRUC *)&tMsg, 200L);
		} while((res == DRV_NO_ERROR) && (tMsg.f == 0x02 || tMsg.f == 0x39) && e_try > 0);

		//Process errors
		if(res != DRV_NO_ERROR)		throw TError("ReadDB", _("Error requesting to DB: %d."), res);
		if(res == DRV_DEV_GET_TIMEOUT)	throw TError(_("ReadDB"), _("Timeout the get request."));
		if(tMsg.f == 17)	throw TError(_("ReadDB"), _("There is no response from the remote station."));
		if(tMsg.f == 18)	throw TError(_("ReadDB"), _("Master is out of the logical token ring."));
		if(tMsg.f == 0x85)	throw TError(11, _("ReadDB"), _("Error the specified offset address or DB."));

		//printf("Get DB %d:%d DB%d.%d(%d) -- %d\n",mDev,vmax(0,vmin(126,s2i(addr()))),n_db,offset,buffer.size(),tMsg.f);

		//Close connection
		/*tMsg.rx = 3;
		tMsg.tx = 16;
		tMsg.ln = 0;
		tMsg.nr = 0;
		tMsg.a  = 0;
		tMsg.f  = 0;
		tMsg.b  = MPI_Disconnect;
		tMsg.e  = 0;
		if((res=DevPutMessage(mDev,(MSG_STRUC *)&tMsg, 200L)) != DRV_NO_ERROR)
		    throw TError(nodePath().c_str(),_("12:Error putting request: %d."),res);
		if((res=DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,200L)) != DRV_NO_ERROR)
		    throw TError(nodePath().c_str(),_("12:Error getting request: %d."),res);*/

		//Put result
		buffer.replace(0, buffer.size(), (char *)tMsg.d+8, buffer.size());
		break;
	    }
	    case ISO_TCP:
	    case ISO_TCP243: {
		//Reconnect for lost connection
		if(!dc || tmDelay >= 0) connectRemotePLC();
		int rez;
		MtxAlloc res1(reqAPIRes, true);
		ResAlloc res2(mod->resAPI, false);
		if((rez=daveReadBytes(dc,daveDB,n_db,offset,buffer.size(),NULL))) {
		    if(rez == daveResTimeout) throw TError("ReadDB", _("Error connecting."));
		    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("Error reading the block %d: %s."), n_db, daveStrerror(rez));
		    throw TError(11, "ReadDB", _("Error the DB %d: %s."), n_db, daveStrerror(rez));
		}
		buffer.replace(0, buffer.size(), (char*)dc->resultPointer, buffer.size());	//!!!! But assign() temporary the block size changes
		break;
	    }
	    case ADS: {
		MtxAlloc rRes(reqAPIRes, true);
		tr.at().start();

		char buf[1000], res[1000];

		AMS_TCP_HEAD *AmsTcpHD = (AMS_TCP_HEAD *)buf;
		AmsTcpHD->reserv = 0;
		AmsTcpHD->len = sizeof(AMS_HEAD) + sizeof(ADS_ReadWriteReq);

		AMS_HEAD *AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
		string tAddr = TSYS::strParse(addr(),0,"|");
		for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_targ[i_d] = s2i(TSYS::strParse(tAddr,0,".",&i_off));
		AmsHD->AMSPort_targ = s2i(TSYS::strParse(tAddr,1,":"));
		tAddr = TSYS::strParse(addr(),1,"|");
		for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_src[i_d] = s2i(TSYS::strParse(tAddr,0,".",&i_off));
		AmsHD->AMSPort_src = s2i(TSYS::strParse(tAddr,1,":"));
		AmsHD->com = 2;		//ADS Read
		AmsHD->stFlgs = 0x4;	//ADS command
		AmsHD->len = sizeof(ADS_ReadWriteReq);
		AmsHD->errCod = 0;
		AmsHD->InvId = 0;

		ADS_ReadWriteReq *ADSread = (ADS_ReadWriteReq *)(AmsHD+1);
		ADSread->IdGrp = n_db;
		ADSread->IdOff = offset;
		ADSread->len = buffer.size();

		//Request
		int resp_len = tr.at().messIO(buf, AmsTcpHD->len+sizeof(AMS_TCP_HEAD), res, sizeof(res));
		int full_len = resp_len;
		if(full_len < (int)sizeof(AMS_TCP_HEAD))	throw TError("ReadDB", _("Error the server response."));
		AmsTcpHD = (AMS_TCP_HEAD *)res;
		unsigned resp_sz = AmsHD->len;

		//Wait tail
		while(full_len < (int)(resp_sz+sizeof(AMS_TCP_HEAD))) {
		    resp_len = tr.at().messIO(NULL, 0, res+full_len, sizeof(res)-full_len);
		    if(!resp_len) throw TError("ReadDB", _("Not full response."));
		    full_len += resp_len;
		}

		//Check for correct result
		AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
		ADS_ReadResp *ADSreadResp = (ADS_ReadResp *)(AmsHD+1);
		resp_len = sizeof(AMS_TCP_HEAD)+sizeof(AMS_HEAD);
		if(full_len < resp_len || AmsHD->com != 2 || AmsHD->stFlgs != 0x05)	throw TError("ReadDB", _("Error the server response."));
		if(AmsHD->errCod) throw TError(11, "ReadDB", _("Error the server response: %d."), AmsHD->errCod);
		if(full_len < (resp_len+=sizeof(ADS_ReadResp)))	throw TError(12, "ReadDB", _("Error the server response."));
		if(ADSreadResp->res) throw TError(13, "ReadDB", _("Error the server response: %d."), ADSreadResp->res);
		if(ADSreadResp->len != buffer.size() || full_len < (int)(resp_len+ADSreadResp->len))
		    throw TError(14, "ReadDB", _("Error the server response."));
		buffer.replace(0, buffer.size(), res+resp_len, buffer.size());	//!!!! But assign() temporary the block size changes

		break;
	    }
	    case SELF_ISO_TCP: {
		XMLNode req("ISO-TCP");
		req.setAttr("id", "read")->setAttr("db", i2s(n_db))->setAttr("off", i2s(offset))->setAttr("size", i2s(buffer.size()));
		reqService(req);
		if(req.attr("err").size()) throw TError(s2i(req.attr("errCod")), "", "%s", req.attr("err").c_str());
		if(req.text().size() != buffer.size()) throw TError("read", _("Size of the response data block %d != %d."), req.text().size(), buffer.size());
		buffer.replace(0, buffer.size(), req.text());	//!!!! But assign() temporary the block size changes
		break;
	    }
	}
	numR += buffer.size();
    } catch(TError &er) {
	if(er.cat.size()) er.mess = er.cat + ":" + er.mess;
	if(!er.cod) er.cod = ConnErrCode;
	if(er.cod == ConnErrCode) setCntrDelay(er.mess);
	er.mess = i2s(er.cod) + ":" + er.mess;
	if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), "%s", er.mess.c_str());
	throw er;
    }
}

void TMdContr::putDB( unsigned n_db, long offset, const string &buffer )
{
    try{
	switch(mType) {
	    case CIF_PB: {
		RCS_MESSAGE tMsg;
		int res, e_try = 4;

		if(buffer.size() > 240)		throw TError("WriteDB", _("Transmitted block is too big."));
		if(!owner().cif_devs[mDev].present)	throw TError("WriteDB", _("The board %d is not present."), mDev);

		ResAlloc resource(owner().cif_devs[mDev].res, true);

		do {
		    e_try--;

		    //Clear all previous data
		    while(!DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,0L))	TSYS::sysSleep(1e-3);

		    //Prepare put request
		    tMsg.rx = 3;
		    tMsg.tx = 16;
		    tMsg.ln = buffer.size()+8;
		    tMsg.nr = 0;
		    tMsg.a  = 0;
		    tMsg.f  = 0;
		    tMsg.b  = MPI_Read_Write_DB;
		    tMsg.e  = 0;
		    tMsg.d[0] = vmax(0,vmin(126,s2i(addr())));
		    tMsg.d[1] = (unsigned char)(offset>>8);
		    tMsg.d[2] = (unsigned char)n_db;
		    tMsg.d[3] = (unsigned char)(n_db>>8);
		    tMsg.d[4] = (unsigned char)offset;
		    tMsg.d[5] = (unsigned char)buffer.size();
		    tMsg.d[6] = TASK_TDT_UINT8;
		    tMsg.d[7] = TASK_TFC_WRITE;
		    memcpy(tMsg.d+8, buffer.c_str(), buffer.size());

		    //Put message to remote host
		    res = DevPutMessage(mDev, (MSG_STRUC *)&tMsg, 200L);
		    if(res == DRV_DEV_PUT_TIMEOUT) throw TError("WriteDB", _("Timeout sending request."));
		    //Get remote host's response
		    if(res == DRV_NO_ERROR) res = DevGetMessage(mDev, sizeof(RCS_MESSAGE), (MSG_STRUC *)&tMsg, 200L);
		}
		while((res == DRV_NO_ERROR) && (tMsg.f == 0x02 || tMsg.f == 0x39) && e_try > 0);

		//printf("Put DB %d:%d DB%d.%d(%d) -- %d \n",mDev,vmax(0,vmin(126,s2i(addr()))),n_db,offset,buffer.size(),tMsg.f);

		//Process errors
		if(res != DRV_NO_ERROR)		throw TError("WriteDB", _("Error requesting to DB: %d."), res);
		if(res == DRV_DEV_GET_TIMEOUT)	throw TError("WriteDB", _("Timeout getting request."));
		if(tMsg.f == 17)	throw TError(_("WriteDB"), _("There is no response from the remote station."));
		if(tMsg.f == 18)	throw TError(_("WriteDB"), _("Master is out of the logical token ring."));
		if(tMsg.f == 0x85)	throw TError(11, _("WriteDB"), _("Error the specified offset address or DB."));
		break;
	    }
	    case ISO_TCP:
	    case ISO_TCP243: {
		//Reconnect for lost connection
		if(!dc || tmDelay >= 0) connectRemotePLC();
		int rez;
		MtxAlloc res1(reqAPIRes, true);
		ResAlloc res2(mod->resAPI, false);
		if((rez=daveWriteBytes(dc,daveDB,n_db,offset,buffer.size(),(char*)buffer.c_str()))) {
		    if(rez == daveResTimeout) throw TError("WriteDB", _("Error connecting."));
		    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("Error writing the block %d: %s."), n_db, daveStrerror(rez));
		    throw TError(11, "WriteDB", _("Error the DB %d: %s."), n_db, daveStrerror(rez));
		}
		break;
	    }
	    case ADS: {
		MtxAlloc res1(reqAPIRes, true);
		tr.at().start();

		char buf[1000], res[1000];

		AMS_TCP_HEAD *AmsTcpHD = (AMS_TCP_HEAD *)buf;
		AmsTcpHD->reserv = 0;
		AmsTcpHD->len = sizeof(AMS_HEAD) + sizeof(ADS_ReadWriteReq) + buffer.size();

		AMS_HEAD *AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
		string tAddr = TSYS::strParse(addr(), 0, "|");
		for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_targ[i_d] = s2i(TSYS::strParse(tAddr,0,".",&i_off));
		AmsHD->AMSPort_targ = s2i(TSYS::strParse(tAddr,1,":"));
		tAddr = TSYS::strParse(addr(),1,"|");
		for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_src[i_d] = s2i(TSYS::strParse(tAddr,0,".",&i_off));
		AmsHD->AMSPort_src = s2i(TSYS::strParse(tAddr,1,":"));
		AmsHD->com = 3;		//ADS Write
		AmsHD->stFlgs = 0x4;	//ADS command
		AmsHD->len = sizeof(ADS_ReadWriteReq) + buffer.size();
		AmsHD->errCod = 0;
		AmsHD->InvId = 0;

		ADS_ReadWriteReq *ADSreq = (ADS_ReadWriteReq *)(AmsHD+1);
		ADSreq->IdGrp = n_db;
		ADSreq->IdOff = offset;
		ADSreq->len = buffer.size();

		memcpy(ADSreq+1, buffer.data(), buffer.size());

		//Request
		int resp_len = tr.at().messIO(buf, AmsTcpHD->len+sizeof(AMS_TCP_HEAD), res, sizeof(res));
		int full_len = resp_len;
		if(full_len < (int)sizeof(AMS_TCP_HEAD)) throw TError("WriteDB", _("Error the server response."));
		AmsTcpHD = (AMS_TCP_HEAD *)res;
		unsigned resp_sz = AmsHD->len;
		//Wait tail
		while(full_len < (int)(resp_sz+sizeof(AMS_TCP_HEAD))) {
		    resp_len = tr.at().messIO(NULL, 0, res+full_len, sizeof(res)-full_len);
		    if(!resp_len) throw TError("WriteDB", _("Not full response."));
		    full_len += resp_len;
		}
		//Check for correct result
		AmsHD = (AMS_HEAD*)(AmsTcpHD+1);
		ADS_WriteResp *ADSwriteResp = (ADS_WriteResp *)(AmsHD+1);
		resp_len = sizeof(AMS_TCP_HEAD)+sizeof(AMS_HEAD);
		if(full_len < resp_len || AmsHD->com != 3 || AmsHD->stFlgs != 0x05) throw TError("WriteDB", _("Error the server response."));
		if(AmsHD->errCod) throw TError(11, "WriteDB", _("Error the server response: %d."), AmsHD->errCod);
		if(full_len < (resp_len+=sizeof(ADS_WriteResp)))	throw TError(12, "WriteDB", _("Error the server response."));
		if(ADSwriteResp->res) throw TError(13, "WriteDB", _("Error the server response: %d."), ADSwriteResp->res);

		break;
	    }
	    case SELF_ISO_TCP: {
		XMLNode req("ISO-TCP");
		req.setAttr("id", "write")->setAttr("db", i2s(n_db))->setAttr("off", i2s(offset))->setText(buffer);
		reqService(req);
		if(req.attr("err").size()) throw TError(s2i(req.attr("errCod")), "", "%s", req.attr("err").c_str());
		break;
	    }
	}
	numW += buffer.size();
    } catch(TError &er) {
	if(er.cat.size()) er.mess = er.cat + ":" + er.mess;
	if(!er.cod) er.cod = ConnErrCode;
	if(er.cod == ConnErrCode) setCntrDelay(er.mess);
	er.mess = i2s(er.cod) + ":" + er.mess;
	if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), "%s", er.mess.c_str());
	throw er;
    }
}

void TMdContr::reqService( XMLNode &io )
{
    MtxAlloc res(reqAPIRes, true);
    try {
	tr.at().start((enableStat() && !isReload) ? 0 : 1000);

	io.setAttr("err", "");
	if(!isInitiated) {
	    XMLNode req("ISO-TCP"); req.setAttr("id", "connect");
	    protIO(req);
	    if(!req.attr("err").empty()) throw TError(req.attr("id").c_str(), "%s", req.attr("err").c_str());

	    req.clear()->setAttr("id", "OpenS7Connection");
	    protIO(req);
	    if(!req.attr("err").empty()) throw TError(req.attr("id").c_str(), "%s", req.attr("err").c_str());

	    isInitiated = true;
	}
	protIO(io);
    } catch(TError &er) { io.setAttr("err", (er.cat.empty()?"":er.cat+":")+er.mess)->setAttr("errCod", i2s(er.cod)); }
}

void TMdContr::protIO( XMLNode &io )
{
    string tpkt, rez, rezP, rezD, err;
    int sCd = 0;
    char buf[1000];

    MtxAlloc resN(tr.at().reqRes(), true);
    try {
	if(io.name() != "ISO-TCP")	throw TError("", _("Unknown target protocol '%s'."), io.name().c_str());

						//>TPKT (RFC-1006)
	oN(tpkt, ISOTCP_TPKT_Version, 1);	// Version
	oN(tpkt, 0, 1);				// Reserved
	int tpktLenOff = tpkt.size();
	oN(tpkt, 0, 2);				// Length

	//Service's requests prepare
	if(io.attr("id") == "connect") {
						//>COTP ConnRequest (ISO-8073)
	    oN(rez, 0, 1);			// Length
	    oN(rez, COTP_CR, 1);		// PDU Type
	    oN(rez, 0, 2);			// Dest reference
	    oN(rez, 1, 2);			// Source reference
	    oN(rez, 0, 1);			// Class
						// Options
						//  TPDU size
	    oN(rez, COTP_O_SzTPDU, 1);		//   Code
	    oN(rez, 1, 1);			//   Length
	    oN(rez, 9, 1);			//   Value, 512
						//  Src TSAP
	    oN(rez, COTP_O_SrcTSAP, 1);		//   Code
	    oN(rez, 2, 1);			//   Length
	    oN(rez, 0x100, 2);			//   Value, 0x100
						//  Dest TSAP
	    oN(rez, COTP_O_DstTSAP, 1);		//   Code
	    oN(rez, 2, 1);			//   Length
	    oN(rez, 0x100|mSlot, 2);		//   Value, Function (1=PG,2=OP,3=Step7Basic) | Rack (Bit 7-5) and Slot (Bit 4-0)

	    oN(rez, rez.size()-1, 1, 0);	//>COTP real length
	    tpkt += rez;
	}
	else {
	    if(io.attr("id") == "OpenS7Connection") sCd = ISOTCP_OpenS7Connection;
	    else if(io.attr("id") == "read")	sCd = ISOTCP_Read;
	    else if(io.attr("id") == "write")	sCd = ISOTCP_Write;
	    if(!sCd) throw TError("", _("Unknown service '%s'."), io.attr("id").c_str());

						//>COTP ConnRequest (ISO-8073)
	    oN(rez, 0, 1);			// Length
	    oN(rez, COTP_DT, 1);		// PDU Type
	    oN(rez, COTP_LastDtUnit, 1);	// Class
	    oN(rez, rez.size()-1, 1, 0);	//>COTP real length
	    tpkt += rez;

	    rez = "";							//>ISO-TCP
	    switch(sCd) {
		case ISOTCP_OpenS7Connection:
		    oN(rezP, sCd, 1);					// Open S7 Connection
		    oN(rezP, 0, 1);
		    oN(rezP, 1, 2);
		    oN(rezP, 1, 2);
		    oN(rezP, vmax(s2i(io.attr("len")),1024), 2);
		    break;
		case ISOTCP_Read:
		    oN(rezP, sCd, 1);					// Read function
		    oN(rezP, 0x01120A10, 4);				// Parameters counter (1) and constant stream
		    oN(rezP, 2, 1);					// 1=single bit, 2=byte, 4=word
		    oN(rezP, s2i(io.attr("size")), 2);			// length in bytes
		    oN(rezP, s2i(io.attr("db")), 2);			// DB number
		    oN(rezP, (0x84<<24) | (s2i(io.attr("off"))<<3), 4);	// DB area (0x84) and start address in bits
		    break;
		case ISOTCP_Write:
		    oN(rezP, sCd, 1);					// Write function
		    oN(rezP, 0x01120A10, 4);				// Parameters counter (1) and constant stream
		    oN(rezP, 2, 1);					// 1=single bit, 2=byte, 4=word
		    oN(rezP, io.text().size(), 2);			// length in bytes
		    oN(rezP, s2i(io.attr("db")), 2);			// DB number
		    oN(rezP, (0x84<<24) | (s2i(io.attr("off"))<<3), 4);	// DB area (0x84) and start address in bits

		    oN(rezD, 0, 1);					// No an error
		    oN(rezD, 4, 1);					// len is in bits
		    oN(rezD, io.text().size()<<3, 2);			// len
		    rezD += io.text();
		    break;
	    }
	    //PDU handling:
	    // PDU is the central structure present in S7 communication.
	    // It is composed of a 10 or 12 byte header,a parameter block and a data block.
	    // When reading or writing values, the data field is itself composed of a data
	    // header followed by payload data
	    oN(rez, 0x32, 1);			//allways 0x32
	    oN(rez, 1, 1);			//Header type, one of 1,2,3 or 7. type 2 and 3 headers are two bytes longer.
	    oN(rez, 0, 2);			//Currently unknown. Maybe it can be used for long numbers?
	    oN(rez, mInvokeID++, 2);		//number: sequence or invoke number
	    oN(rez, rezP.size(), 2);		//plen: length of parameters which follow this header
	    oN(rez, rezD.size(), 2);		//dlen: length of data which follow the parameters
						//result[2]: only present in type 2 and 3 headers. This contains error information.
	    tpkt += rez + rezP + rezD;
	}

	if(!rez.size())	throw TError("", _("Unknown service '%s'."), io.attr("id").c_str());

	oN(tpkt, tpkt.size(), 2, tpktLenOff);			//>TPTK real length

	//Send request and wait respond
	int resp_len = messIO(tpkt.data(), tpkt.size(), buf, sizeof(buf));
	tpkt.assign(buf, resp_len);
	int off = 2;
	for( ; tpkt.size() < 4 || tpkt.size() < iN(tpkt,off,2); off = 2) {
	    if(!(resp_len=messIO(NULL,0,buf,sizeof(buf)))) throw TError("", _("Not full response."));
	    tpkt.append(buf, resp_len);
	}

	off = 2;
	if(tpkt.size() < 4 || iN(tpkt,off,2) != tpkt.size())	//<TPKT length
	    throw TError("", _("Inconsistency in response size."));

								//<COTP result
	uint8_t lenCOTP = iN(tpkt, off, 1);			// Length
	uint8_t begCOTP = off;
	if(lenCOTP < 2 || tpkt.size() < (begCOTP+lenCOTP))	throw TError("COTP", _("Short."));
	uint8_t tpPDU = iN(tpkt, off, 1);			// PDU Type

	//Service's response process
	if(io.attr("id") == "connect") {
	    if(tpPDU != COTP_CC)	throw TError("COTP", _("Error the PDU type, %d != %d."), tpPDU, COTP_CC);
	    iN(tpkt, off, 2);					// Dest reference
	    iN(tpkt, off, 2);					// Source reference
	    iN(tpkt, off, 1);					// Class
	    while((off-begCOTP) < lenCOTP) {			// Options
		iN(tpkt, off, 1);				//  Code
		uint8_t oLen = iN(tpkt, off, 1);		//  Length
		iVal(tpkt, off, oLen);
	    }
	    if((off-begCOTP) != lenCOTP)throw TError("COTP", _("Inappropriate size."));
	}
	else if(sCd) {
	    if(tpPDU != COTP_DT)	throw TError("COTP", _("Error the PDU type, %d != %d."), tpPDU, COTP_DT);
	    iN(tpkt, off, 1);					// Class
	    uint32_t tmpV;
								//<ISO-TCP
	    if(iN(tpkt,off,1) != 0x32)				//P: allways 0x32
		throw TError(_("PDU header"), _("Not started from 0x32."));
	    char iHTp = iN(tpkt, off, 1);			//Header type, one of 1,2,3 or 7. type 2 and 3 headers are two bytes longer.
	    iN(tpkt, off, 2);					//Currently unknown. Maybe it can be used for long numbers?
	    if((tmpV=iN(tpkt,off,2)) != (uint16_t)(mInvokeID-1))	//number: sequence or invoke number
		throw TError(_("PDU header"), _("Inconsistency of the response ID to his request one, %d != %d."), (uint16_t)(mInvokeID-1), tmpV);
	    uint16_t szPrm  = iN(tpkt, off, 2);			//plen: length of parameters which follow this header
	    uint16_t szData = iN(tpkt, off, 2);			//dlen: length of data which follow the parameters
	    uint16_t iErr = 0;
	    if(iHTp == 2 || iHTp == 3) iErr = iN(tpkt, off, 2);	//result[2]: only present in type 2 and 3 headers. This contains an error information.
	    if(iErr) {
		const char *errNm = "unknown error.";
		switch(iErr) {
		    case 0x8000: errNm = "function already occupied.";	break;
		    case 0x8001: errNm = "not allowed in current operating status.";	break;
		    case 0x8101: errNm = "hardware fault.";		break;
		    case 0x8103: errNm = "object access not allowed.";	break;
		    case 0x8104: errNm = "context is not supported. Step7 says:Function not implemented or error in telgram.";	break;
		    case 0x8105: errNm = "invalid address.";		break;
		    case 0x8106: errNm = "data type not supported.";	break;
		    case 0x8107: errNm = "data type not consistent.";	break;
		    case 0x810A: errNm = "object does not exist.";	break;
		    case 0x8301: errNm = "insufficient CPU memory ?";	break;
		    case 0x8402: errNm = "CPU already in RUN or already in STOP ?";	break;
		    case 0x8404: errNm = "severe error ?";		break;
		    case 0x8500: errNm = "incorrect PDU size.";		break;
		    case 0x8702: errNm = "address invalid.";		break;
		}
		throw TError(_("PDU header"), _("PLC error: [%d] %s"), (int)iErr, errNm);
	    }
	    if((off+szPrm+szData) > (int)tpkt.size()) throw TError(_("PDU header"), _("Size of the parameters + data more to the response size."));

	    switch(sCd) {
		case ISOTCP_OpenS7Connection: {
		    if(szPrm != 8) throw TError(io.attr("id").c_str(), _("Parameters part size does not match for expected value, %d."), 8);
		    if((int)iN(tpkt,off,1) != sCd) throw TError(io.attr("id").c_str(), _("Inconsistency of the response function."));
		    iN(tpkt, off, 1);
		    iN(tpkt, off, 2);
		    iN(tpkt, off, 2);
		    io.setAttr("len", i2s(iN(tpkt,off,2)));
		    break;
		}
		case ISOTCP_Read: {
		    if(szPrm != 2) throw TError(io.attr("id").c_str(), _("Parameters part size does not match for expected value, %d."), 2);
		    if((int)iN(tpkt,off,1) != sCd) throw TError(io.attr("id").c_str(), _("Inconsistency of the response function."));
		    iN(tpkt, off, 1);
		    // Data part
		    uint8_t iDErr = iN(tpkt, off, 1);
		    if(iDErr != 0xFF) {
			if(iDErr == 5) throw TError(12, io.attr("id").c_str(), _("An attempt to access outside the DB '%s', %s(%s)."),
					io.attr("db").c_str(), io.attr("off").c_str(), io.attr("size").c_str());
			else if(iDErr == 10) throw TError(11, io.attr("id").c_str(), _("The DB '%s' doesn't exist."), io.attr("db").c_str());
			else throw TError(13, io.attr("id").c_str(), _("Unknown error accessing to DB, %xh."), iDErr);
		    }
		    char iDLenTp = iN(tpkt, off, 1);
		    uint16_t iDLen = iN(tpkt, off, 2);
		    if(iDLenTp == 4) iDLen = iDLen/8;	//len is in bits, adjust
		    else if(iDLenTp == 9) ;		//len is already in bytes, ok
		    else if(iDLenTp == 3) ;		//len is in bits, but there is a byte per result bit, ok
		    else throw TError(14, io.attr("id").c_str(), _("Unknown data encoding type, %d."), iDLenTp);
		    if((off+iDLen) != (int)tpkt.size())
			throw TError(15, io.attr("id").c_str(), _("Inconsistency in the size of the data block to the response size, %d(%d)."),
								    (off+iDLen), tpkt.size());
		    io.setText(tpkt.substr(off));
		    break;
		}
		case ISOTCP_Write: {
		    if(szPrm != 2) throw TError(io.attr("id").c_str(), _("Parameters part size does not match for expected value, %d."), 2);
		    if((int)iN(tpkt,off,1) != sCd) throw TError(io.attr("id").c_str(), _("Inconsistency of the response function."));
		    iN(tpkt, off, 1);
		    // Data part
		    uint8_t iDErr = iN(tpkt, off, 1);
		    if(iDErr != 0xFF) {
			if(iDErr == 5) throw TError(12, io.attr("id").c_str(), _("An attempt to access outside the DB '%s', %s(%s)."),
					io.attr("db").c_str(), io.attr("off").c_str(), io.attr("size").c_str());
			else if(iDErr == 10) throw TError(11, io.attr("id").c_str(), _("The DB '%s' doesn't exist."), io.attr("db").c_str());
			else throw TError(13, io.attr("id").c_str(), _("Unknown error accessing to DB, %xh."), iDErr);
		    }
		    break;
		}
	    }
	}
    } catch(TError &er) { io.setAttr("err", (er.cat.empty()?"":er.cat+":")+er.mess)->setAttr("errCod", i2s(er.cod)); }
}

int TMdContr::messIO( const char *oBuf, int oLen, char *iBuf, int iLen )
{
    return tr.at().messIO(oBuf, oLen, iBuf, iLen, ((enableStat() && !isReload)?0:1000));
}

void TMdContr::reset( )
{
    isInitiated = false;
    mInvokeID = -1;
}

TVariant TMdContr::getVal( const string &iaddr, ResString &err)
{
    if(tmDelay > 0) {
	if(err.getVal().empty())
	    err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return EVAL_REAL;
    }

    int db = -1, off = -1;
    char tp[11];
    if(sscanf(iaddr.c_str(),"DB%d.%i.%10s",&db,&off,tp) != 3 || db < 0 || off < 0)	return EVAL_REAL;

    int ivSz = valSize(tp);	//Get real value's size

    ResAlloc res(reqDataRes, false);
    for(unsigned iB = 0; iB < acqBlks.size(); iB++)
	if(acqBlks[iB].db == db && off >= acqBlks[iB].off &&
	    (off+ivSz) <= (acqBlks[iB].off+(int)acqBlks[iB].val.size()))
	{
	    if(!acqBlks[iB].err.size()) {
		switch(tp[0]) {
		    case 'b':		return (bool)(acqBlks[iB].val[off-acqBlks[iB].off]&(0x01<<atoi(tp+1)));
		    case 'i':
			switch(ivSz) {
			    case 1:	return *(int8_t*)(acqBlks[iB].val.data()+(off-acqBlks[iB].off));
						//acqBlks[iB].val[off-acqBlks[iB].off];
			    case 2:	return (int16_t)TSYS::i16_BE(TSYS::getUnalign16(acqBlks[iB].val.data()+(off-acqBlks[iB].off)));
						//*(int16_t*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str();
			    case 4:	return (int32_t)TSYS::i32_BE(TSYS::getUnalign32(acqBlks[iB].val.data()+(off-acqBlks[iB].off)));
						//*(int32_t*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str();
			    case 8:	return (int64_t)TSYS::i64_BE(TSYS::getUnalign64(acqBlks[iB].val.data()+(off-acqBlks[iB].off)));
						//*(int64_t*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str();
			}
			break;
		    case 'u':
			switch(ivSz) {
			    case 1:	return *(uint8_t*)(acqBlks[iB].val.data()+(off-acqBlks[iB].off));
						//(uint8_t)acqBlks[iB].val[off-acqBlks[iB].off];
			    case 2:	return TSYS::i16_BE(TSYS::getUnalign16(acqBlks[iB].val.data()+(off-acqBlks[iB].off)));
						//(int64_t)*(uint16_t*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str();
			    case 4:	return (int64_t)TSYS::i32_BE(TSYS::getUnalign32(acqBlks[iB].val.data()+(off-acqBlks[iB].off)));
						//(int64_t)*(uint32_t*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str();
			}
			break;
		    case 'r':
			switch(ivSz) {
			    case 4:	return TSYS::floatLErev(*(float*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str());
			    case 8:	return TSYS::doubleLErev(*(double*)revers(acqBlks[iB].val.substr(off-acqBlks[iB].off,ivSz)).c_str());
			}
		    case 's':		return acqBlks[iB].val.substr(off-acqBlks[iB].off, ivSz);
		}
	    }
	    else err.setVal(acqBlks[iB].err);
	    break;
	}
    if(err.getVal().empty())	err = TSYS::strMess(_("%d:The value is not gathered."), 11);

    return EVAL_REAL;
}

void TMdContr::setVal( const TVariant &ivl, const string &iaddr, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return;
    }
    if(ivl.isEVal())	return;

    int db = -1, off = -1;
    char tp[11];
    if(sscanf(iaddr.c_str(),"DB%d.%i.%10s",&db,&off,tp) != 3 || db < 0 || off < 0)	return;

    int ivSz = valSize(tp);	//Get real value's size

    string val;
    switch(tp[0]) {
	case 'b': {
	    char tval = getVal(TSYS::strMess("DB%d.%i.u1",db,off),err).getI();
	    if((bool)(tval&(0x01<<atoi(tp+1))) != (bool)ivl.getB()) {
		tval ^= (0x01<<atoi(tp+1));
		val = tval;
	    }
	    break;
	}
	case 'i':
	case 'u': {
	    int64_t tval = ivl.getI();
	    if(tval != getVal(iaddr,err).getI())
		val = revers(string((char*)&tval,ivSz));
	    break;
	}
	case 'r':
	    if(ivSz == 4) {
		float tval = ivl.getR();
		if(tval != getVal(iaddr,err).getR())
		    val = revers(string((char*)&tval,ivSz));
	    }
	    else {
		double tval = ivl.getR();
		if(tval != getVal(iaddr,err).getR())
		    val = revers(string((char*)&tval,ivSz));
	    }
	    break;
	case 's':
	    val = ivl.getS(); val.resize(ivSz);
	    if(val == getVal(iaddr,err).getS()) val = "";
    }
    if(!val.size())	return;

    //Write data to controller or write data block
    try {
	if(!assincWrite())	putDB(db, off, val);
	else {
	    ResAlloc res(reqDataAsWrRes, false);
	    for(unsigned iB = 0; iB < writeBlks.size(); iB++)
		if(writeBlks[iB].db == db && off >= writeBlks[iB].off &&
			(off+ivSz) <= (writeBlks[iB].off+(int)writeBlks[iB].val.size()))
		{
		    writeBlks[iB].val.replace(off-writeBlks[iB].off,ivSz,val);
		    if(s2i(writeBlks[iB].err.getVal()) == -1) writeBlks[iB].err = "";
		    break;
		}
	}
	//Set to DB buffer
	ResAlloc res(reqDataRes, false);
	for(unsigned iB = 0; iB < acqBlks.size(); iB++)
	    if(acqBlks[iB].db == db && off >= acqBlks[iB].off &&
		    (off+ivSz) <= (acqBlks[iB].off+(int)acqBlks[iB].val.size()))
	    { acqBlks[iB].val.replace(off-acqBlks[iB].off,ivSz,val); break; }
    } catch(TError &cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

int TMdContr::valSize( const string &itp )
{
    if(itp.size()) {
	int ivSz = (itp.size() >= 2) ? s2i(itp.substr(1)) : 0;
	switch(itp[0]) {
	    case 's': return (ivSz <= 0) ? 10 : vmin(100,ivSz);
	    case 'i': case 'u': return (ivSz == 1 || ivSz == 2 || ivSz == 4 || ivSz == 8) ? ivSz : 2;
	    case 'r': return (ivSz == 4 || ivSz == 8) ? ivSz : 4;
	    case 'b': return 1;
	    default: break;
	}
    }

    throw TError(nodePath().c_str(), _("Error value type '%s'."), itp.c_str());
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr*)icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    bool isStart = true;
    bool isStop  = false;
    int64_t t_cnt = 0, t_prev = TSYS::curTime();

    try {
	while(true) {
	    if(cntr.tmDelay > 0) {
		//Get data from blocks to parameters or calc for logical type parameters
		MtxAlloc res1(cntr.enRes, true);
		for(unsigned iP = 0; iP < cntr.pHd.size(); iP++)
		    try { cntr.pHd[iP].at().calc(isStart, isStop, cntr.period()?1:-1); }
		    catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
		res1.unlock();

		cntr.tmDelay = vmax(0, cntr.tmDelay-1);

		if(isStop) break;

		TSYS::taskSleep(1000000000);

		if(cntr.endrunReq) isStop = true;
		isStart = false;
		continue;
	    }
	    else if(cntr.tmDelay == 0) cntr.conErr = _("Connecting.");

	    cntr.callSt = true;
	    if(!cntr.period()) t_cnt = TSYS::curTime();

	    //Update controller's data
	    //Process write data blocks
	    if(cntr.assincWrite()) {
		ResAlloc res(cntr.reqDataAsWrRes, false);
		for(unsigned iB = 0; !isStart && !isStop && !cntr.endrunReq && iB < cntr.writeBlks.size(); iB++)
		    try {
			if(cntr.redntUse()) { cntr.writeBlks[iB].err = _("-1:No data"); continue; }
			if(s2i(cntr.writeBlks[iB].err.getVal()) == -1) continue;
			cntr.putDB(cntr.writeBlks[iB].db,cntr.writeBlks[iB].off,cntr.writeBlks[iB].val);
			cntr.writeBlks[iB].err = _("-1:No data");
		    } catch(TError &err) { cntr.writeBlks[iB].err = err.mess; }
		if(cntr.tmDelay > 0) continue;
	    }
	    ResAlloc res(cntr.reqDataRes, false);
	    //Process acquisition data blocks
	    for(unsigned iB = 0; !isStart && !isStop && !cntr.endrunReq && iB < cntr.acqBlks.size(); iB++)
		try {
		    if(cntr.redntUse()) { cntr.acqBlks[iB].err = _("-1:No data"); continue; }
		    cntr.getDB(cntr.acqBlks[iB].db, cntr.acqBlks[iB].off, cntr.acqBlks[iB].val);
		    cntr.acqBlks[iB].err = "";
		} catch(TError &err) { cntr.acqBlks[iB].err = err.mess; }
	    if(cntr.tmDelay > 0) continue;
	    res.unlock();

	    //Calc parameters
	    MtxAlloc res1(cntr.enRes, true);
	    for(unsigned i_p = 0; i_p < cntr.pHd.size() && !cntr.redntUse(); i_p++)
		try{ cntr.pHd[i_p].at().calc(isStart,isStop,cntr.period()?(1e9/cntr.period()):(-1e-6*(t_cnt-t_prev))); }
		catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	    res1.unlock();

	    //Generic acquisition alarm generate
	    if(cntr.tmDelay <= 0) {
		if(cntr.alSt != 0) {
		    cntr.alSt = 0;
		    cntr.alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),cntr.owner().modId().c_str(),cntr.id().c_str(),_("OK")),
				    TMess::Info);
		}
		cntr.tmDelay--;
	    }

	    t_prev = t_cnt;
	    cntr.callSt = false;

	    if(isStop) break;

	    TSYS::taskSleep((int64_t)cntr.period(), cntr.period() ? "" : cntr.cron());

	    if(cntr.endrunReq) isStop = true;
	    isStart = false;
	}
    } catch(TError &err) { mess_err(err.cat.c_str(), err.mess.c_str()); }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::setCntrDelay( const string &err )
{
    if(alSt <= 0) {
	alSt = 1;
	alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),owner().modId().c_str(),id().c_str(),
								TRegExp(":","g").replace(err,"=").c_str()));
	reset();
	numErr++;
    }
    conErr = err;
    tmDelay = restTm;
}

void TMdContr::redntDataUpdate( )
{
    TController::redntDataUpdate();

    vector<string> pls; list(pls);

    //Request for template's attributes values
    XMLNode req("CntrReqs"); req.setAttr("path",nodePath(0,true));
    for(unsigned i_p = 0; i_p < pls.size(); i_p++) {
	if(!at(pls[i_p]).at().enableStat()) continue;
	req.childAdd("get")->setAttr("path","/prm_"+pls[i_p]+"/%2fserv%2ftmplAttr");
    }

    //Send request to first active station for this controller
    if(owner().owner().rdStRequest(workId(),req).empty()) return;

    //Redirect respond to local parameters
    req.setAttr("path","/");
    for(unsigned i_prm = 0; i_prm < req.childSize(); )
	if(s2i(req.childGet(i_prm)->attr("err"))) req.childDel(i_prm);
	else { req.childGet(i_prm)->setName("set"); i_prm++; }
    cntrCmd(&req);
}

TMdContr::SDataRec::SDataRec( int idb, int ioff, int v_rez ) : db(idb), off(ioff)
{
    val.assign(v_rez, 0);
    err = TSYS::strMess(_("%d:The value is not gathered."), 11);
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	if(!(type() == CIF_PB || type() == ISO_TCP || type() == ISO_TCP243 || type() == SELF_ISO_TCP)) ctrRemoveNode(opt,"/cntr/cfg/SLOT");
	if(type() != CIF_PB) ctrRemoveNode(opt,"/cntr/cfg/CIF_DEV");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
	    "tp","str", "dest","sel_ed", "sel_list",TMess::labSecCRONsel(), "help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1, "help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/TYPE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,
	    "help",_("Connection type, supported ones:\n"
		     "  CIF_PB - connection to S7 controllers of the firm Siemens via CIF-50PB communication processor or similar;\n"
		     "  ISO_TCP, ISO_TCP243 - connection to S7 controllers of the firm Siemens via the Ethernet network (TCP243 by CP243);\n"
		     "  ADS - TwinCAT ADS/AMS protocol for connecting to controllers of the firm Beckhoff."));
	if(type() != SELF_ISO_TCP)
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,
		"help",_("Remote controller address, for the connections:\n"
		     "  CIF_PB - controller address in the ProfiBus network, one digit 0-255;\n"
		     "  ISO_TCP, ISO_TCP243 - IP-address into the Ethernet network;\n"
		     "  ADS - network identifier and port for the target and source stations, in view\n"
		     "      \"{Target_AMSNetId}:{Target_AMSPort}|{Source_AMSNetId}:{Source_AMSPort}\"\n"
		     "      (for example: \"192.168.0.1.1.1:801|82.207.88.73.1.1:801\"), where:\n"
		     "    AMSNetId - network identifier, writes in view of six digits 0-255, for example: \"192.168.0.1.1.1\";\n"
		     "    AMSPort - port, writes in view of one digit 0-65535."));
	else ctrRemoveNode(opt, "/cntr/cfg/ADDR");
	if(type() == ADS || type() == SELF_ISO_TCP)
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR_TR",EVAL_STR,RWRWR_,"root",SDAQ_ID,3, "dest","select", "select","/cntr/cfg/trLst",
		"help",_("OpenSCADA output transport for the protocol ADS (port 48898, 801 for AMS) and ISO_TCP (port 102) for sending requests."));
	else ctrRemoveNode(opt, "/cntr/cfg/ADDR_TR");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt)) {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    } else TController::cntrCmdProc(opt);
}

const char *TMdContr::iVal( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size()) throw TError(mod->nodePath().c_str(), "Buffer size is lesser requested value.");

    return rb.data()+off-vSz;
}

uint32_t TMdContr::iN( const string &rb, int &off, uint8_t vSz )
{
    vSz = std::max(0, std::min((int)vSz,4));
    if((off+vSz) > (int)rb.size()) throw TError(mod->nodePath().c_str(), "Buffer size is lesser requested value.");
    union { uint32_t v; char c[4]; } dt;
    dt.v = 0;
    while(vSz) dt.c[--vSz] = rb[off++];

    return TSYS::i32_LE(dt.v);
}

void TMdContr::oN( string &buf, uint32_t val, uint8_t sz, int off )
{
    union { uint32_t v; char c[4]; } dt;
    dt.v = TSYS::i32_LE(val);
    if(/*sz < 0 || */sz > 4) for(sz = 4; sz > 1 && !dt.c[sz-1]; ) sz--;
    off = (off >= 0) ? std::min(off,(int)buf.size()) : buf.size();
    if((off+sz) > (int)buf.size()) buf.append(off+sz-buf.size(), char(0));
    while(sz) buf[off++] = dt.c[--sz];
}

//************************************************
//* TMdPrm                                       *
//************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) :
    TParamContr(name,tp_prm), TPrmTempl::Impl(this, name+"SiemensPrm"), pEl("cif_attr"),
    chkLnkNeed(false), idFreq(-1), idStart(-1), idStop(-1), idErr(-1), idSh(-1), idNm(-1), idDscr(-1), acqErrTm(0)
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

TCntrNode &TMdPrm::operator=( const TCntrNode &node )
{
    TParamContr::operator=(node);

    TMdPrm *src_n = const_cast<TMdPrm*>(dynamic_cast<const TMdPrm*>(&node));
    if(!src_n || !src_n->enableStat() || !enableStat() || !func()) return *this;

    //IO values copy
    for(int iIO = 0; iIO < src_n->ioSize(); iIO++)
	if(src_n->ioFlg(iIO)&TPrmTempl::CfgLink)
	    lnkAddrSet(iIO, src_n->lnkAddr(iIO));
	else setS(iIO, src_n->getS(iIO));

    chkLnkNeed = initLnks();

    return *this;
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&pEl))   vlElemAtt(&pEl);
}

void TMdPrm::postDisable( int flag )
{
    TParamContr::postDisable(flag);

    if(flag) {
	string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";
	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(id(),true);
	SYS->db().at().dataDel(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg);
    }
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat() && !owner().isReload) return;

    TParamContr::enable();
    //Template's function connect
    try {
	//vector<string> als;
	bool to_make = false;
	unsigned fId = 0;
	if(!func()) {
	    setFunc(&SYS->daq().at().tmplLibAt(TSYS::strSepParse(cfg("TMPL").getS(),0,'.')).at().
				     at(TSYS::strSepParse(cfg("TMPL").getS(),1,'.')).at().func().at());
	    to_make = true;
	}
	// Init attrubutes
	addLinksAttrs(&pEl);

	// Init links
	chkLnkNeed = initLnks();

	// Init system attributes identifiers
	idFreq  = ioId("f_frq");
	idStart = ioId("f_start");
	idStop  = ioId("f_stop");
	idErr   = ioId("f_err");
	idSh    = ioId("SHIFR");
	idNm    = ioId("NAME");
	idDscr  = ioId("DESCR");
	int idThis = ioId("this");
	if(idThis >= 0) setO(idThis, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));

	// Load IO at enabling
	if(to_make)	loadIO(true);

	// Set to process
	if(owner().startStat())	calc(true, false, 0);

    } catch(TError &err) { disable(); throw; }

    owner().prmEn(id(), true);
}

void TMdPrm::disable( )
{
    if(!enableStat()) return;

    //Unregister parameter
    owner().prmEn(id(), false);
    if(owner().startStat()) calc(false, true, 0);

    //Template's function disconnect
    cleanLnks(true);
    idFreq = idStart = idStop = idErr = idSh = idNm = idDscr = -1;

    TParamContr::disable();
}

void TMdPrm::load_( )
{
    //TParamContr::load_();
    loadIO();
}

void TMdPrm::loadIO( bool force )
{
    //Load IO and init links
    if(!enableStat())	return;
    if(owner().startStat() && !force) { modif(true); return; }	//Load/reload IO context only allow for stoped controlers for prevent throws

    TConfig cfg(&mod->prmIOE());
    cfg.cfg("PRM_ID").setS(id());
    string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";

    for(int iIO = 0; iIO < ioSize(); iIO++) {
	cfg.cfg("ID").setS(func()->io(iIO)->id());
	if(!SYS->db().at().dataGet(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg,false,true)) continue;
	if(func()->io(iIO)->flg()&TPrmTempl::CfgLink)
	    lnkAddrSet(iIO, cfg.cfg("VALUE").getS());
	else setS(iIO, cfg.cfg("VALUE").getS());
    }
    chkLnkNeed = initLnks();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
    saveIO();
}

void TMdPrm::saveIO( )
{
    //Save IO and init links
    if(!enableStat())	return;

    TConfig cfg(&mod->prmIOE());
    cfg.cfg("PRM_ID").setS(id());
    string io_bd = owner().DB() + "." + type().DB(&owner()) + "_io";

    for(int iIO = 0; iIO < func()->ioSize(); iIO++) {
	cfg.cfg("ID").setS(func()->io(iIO)->id());
	if(func()->io(iIO)->flg()&TPrmTempl::CfgLink)	cfg.cfg("VALUE").setS(lnkAddr(iIO));
	else cfg.cfg("VALUE").setS(getS(iIO));
	SYS->db().at().dataSet(io_bd, owner().owner().nodePath()+type().DB(&owner())+"_io", cfg);
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat()) {
	if(val.name() == "err") {
	    if(!enableStat())			val.setS(_("1:Parameter disabled."), 0, true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition stopped."), 0, true);
	} else val.setS(EVAL_STR, 0, true);
	return;
    }
    if(owner().redntUse()) return;
    if(val.name() != "err")
	try {
	    int id_lnk = lnkId(val.name());
	    if(id_lnk >= 0 && !lnkActive(id_lnk)) id_lnk = -1;
	    if(id_lnk < 0) val.set(get(ioId(val.name())), 0, true);
	    else val.set(lnkInput(id_lnk), 0, true);
	} catch(TError &err) { }
    else {
	if(acqErr.getVal().size()) val.setS(acqErr.getVal(), 0, true);
	else if(idErr >= 0) val.setS(getS(idErr), 0, true);
	else val.setS("0", 0, true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setS(EVAL_STR, 0, true); return; }

    //Send to active reserve station
    if(owner().redntUse()) {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //Direct write
    try {
	int id_lnk = lnkId(vo.name());
	if(id_lnk >= 0 && !lnkActive(id_lnk)) id_lnk = -1;
	if(!(vl.isEVal() || vl == pvl)) {
	    if(id_lnk < 0) set(ioId(vo.name()), vl);
	    else lnkOutput(id_lnk, vl);
	}
    } catch(TError &err) {  }
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr);
    val.arch().at().setPeriod(SYS->archive().at().valPeriod()*1000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

void TMdPrm::calc( bool first, bool last, double frq )
{
    try {
	//Proccess error hold
	if(!acqErr.getVal().empty()) {
	    time_t tm = time(NULL);
	    if(!acqErrTm)	acqErrTm = tm + 5;
	    if(tm > acqErrTm)	{ acqErr.setVal(""); acqErrTm = 0; }
	}

	if(chkLnkNeed && !first && !last) chkLnkNeed = initLnks(true);

	//Set fixed system attributes
	if(idFreq >= 0)	setR(idFreq, frq);
	if(idStart >= 0)setB(idStart, first);
	if(idStop >= 0)	setB(idStop, last);
	if(idSh >= 0)	setS(idSh, id());
	if(idNm >= 0)	setS(idNm, name());
	if(idDscr >= 0)	setS(idDscr, descr());

	//Get input links
	inputLinks();

	//Calc template
	setMdfChk(true);
	TValFunc::calc();
	modif();

	//Put output links
	outputLinks();

	//Put fixed system attributes
	if(idNm >= 0)	setName(getS(idNm));
	if(idDscr >= 0)setDescr(getS(idDscr));

    } catch(TError &err) {
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Error calculating template."));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.compare(0,6,"/serv/") == 0) {
	if(a_path == "/serv/tmplAttr") {
	    if(!enableStat() || !func()) throw TError(nodePath().c_str(), _("Parameter disabled or error."));
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
		for(int i_a = 0; i_a < ioSize(); i_a++)
		    opt->childAdd("a")->setAttr("id",func()->io(i_a)->id())->setText(getS(i_a));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		for(unsigned i_a = 0; i_a < opt->childSize(); i_a++) {
		    int io_id = -1;
		    if(opt->childGet(i_a)->name() != "a" || (io_id=ioId(opt->childGet(i_a)->attr("id"))) < 0) continue;
		    setS(io_id,opt->childGet(i_a)->text());
		}
	} else TParamContr::cntrCmdProc(opt);
	return;
    }

    vector<string> list;
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMPL",cfg("TMPL").fld().descr(),RWRW__,"root",SDAQ_ID,3,"tp","str","dest","select","select","/prm/tmplList");
	if(enableStat()) TPrmTempl::Impl::cntrCmdProc(opt);
	return;
    }
    //Process command to page
    if(a_path == "/prm/cfg/TMPL" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	cfg("TMPL").setS(opt->text());
	disable();
    }
    else if(a_path == "/cfg/only_off" && enableStat()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText(TBDS::genDBGet(mod->nodePath()+"onlOff","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) TBDS::genDBSet(mod->nodePath()+"onlOff",opt->text(),opt->attr("user"));
    }
    else if(enableStat() && a_path.substr(0,12) == "/cfg/prm/pr_") {
	int lnk_id = s2i(a_path.substr(12));
	int db = 0, off = 0;
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    string addr = lnkAddr(lnk_id);
	    if(sscanf(addr.c_str(),"DB%d.%i",&db,&off) == 2) {
		int t_off = s2i(TSYS::strParse(TSYS::strLine(func()->io(lnk_id)->def(),0), 1, "|"));
		opt->setText("DB"+i2s(db)+(((off-t_off) > 0)?"."+i2s(off-t_off):""));
	    } else TPrmTempl::Impl::cntrCmdProc(opt);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(sscanf(opt->text().c_str(),"DB%d.%i",&db,&off) >= 1) {
		string p_nm = TSYS::strParse(TSYS::strLine(func()->io(lnk_id)->def(),0), 0, "|"), tVl;
		bool set = false;
		for(int iIO = 0; iIO < ioSize(); iIO++) {
		    if(!(ioFlg(iIO)&TPrmTempl::CfgLink)) continue;
		    tVl = TSYS::strParse(TSYS::strLine(func()->io(iIO)->def(),0), 0, "|");
		    if(p_nm == tVl) {
			int coff = -1, cbit = 0;
			if(sscanf(TSYS::strParse(TSYS::strLine(func()->io(iIO)->def(),0),1,"|").c_str(),"%i.%d",&coff,&cbit)) {
			    string lAddr = "DB"+i2s(db) + "." + i2s(off+coff);
			    if(ioType(iIO) == IO::Boolean) lAddr += "." + i2s(cbit);
			    else if((tVl=TSYS::strParse(TSYS::strLine(func()->io(iIO)->def(),0),2,"|")).size())
				lAddr += "." + tVl;
			    lnkAddrSet(iIO, lAddr);
			    modif();
			    set = true;
			}
		    }
		}
		if(set) initLnks();
	    } else TPrmTempl::Impl::cntrCmdProc(opt);
	}
    }
    else if(enableStat() && TPrmTempl::Impl::cntrCmdProc(opt))	;
    else TParamContr::cntrCmdProc(opt);
}

bool TMdPrm::lnkInit( int num, bool checkNoLink )
{
    //Common link forms
    if(!TPrmTempl::Impl::lnkInit(num,checkNoLink)) return false;

    MtxAlloc res(obj->dataRes(), true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it == lnks.end() || (checkNoLink && it->second.addrSpec.size())) return false;

    it->second.addrSpec = "";
    int rez = 0, db = -1, off = -1;
    char tp[11];
    if(((rez=sscanf(it->second.addr.c_str(),"DB%d.%i.%10s",&db,&off,tp)) == 2 || rez == 3) && db >= 0 && off >= 0) {
	string stp = (rez == 2) ? TSYS::strParse(TSYS::strLine(func()->io(num)->def(),0),2,"|") : tp;
	if(stp.empty() || isdigit(stp[0]))
	    switch(ioType(num)) {
		case IO::Boolean: stp.insert(0, "b");	break;
		case IO::Integer: stp.insert(0, "i");	break;
		case IO::Real:	  stp.insert(0, "r");	break;
		case IO::String:  stp.insert(0, "s");	break;
		default:	break;
	    }
	if(stp[0] == 'b' || stp[0] == 'i' || stp[0] == 'u' || stp[0] == 'r' || stp[0] == 's') {
	    it->second.addrSpec = TSYS::strMess("DB%d.%i.%s", db, off, stp.c_str());
	    owner().regVal(it->second.addrSpec, ioFlg(num)&(IO::Output|IO::Return));

	    return true;
	}
    }

    return false;
}

bool TMdPrm::lnkActive( int num )
{
    MtxAlloc res(obj->dataRes(), true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it != lnks.end() && it->second.addrSpec.size())	return true;
    return TPrmTempl::Impl::lnkActive(num);
}

TVariant TMdPrm::lnkInput( int num )
{
    MtxAlloc res(obj->dataRes(), true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it == lnks.end()) return EVAL_REAL;

    return it->second.addrSpec.size() ?
	owner().getVal(it->second.addrSpec, acqErr) :
	TPrmTempl::Impl::lnkInput(num);
}

bool TMdPrm::lnkOutput( int num, const TVariant &vl )
{
    MtxAlloc res(obj->dataRes(), true);
    map<int,SLnk>::iterator it = lnks.find(num);
    if(it == lnks.end()) return false;

    if(it->second.addrSpec.size())
	owner().setVal(vl, it->second.addrSpec, acqErr);
    else return TPrmTempl::Impl::lnkOutput(num, vl);

    return true;
}

string TMdPrm::lnkHelp( )
{
    return _("Special address format:\n"
	"Siemens-DB address writes in the form \"DB{N}.{off}[.{BOff}|{TpSz}]\", where:\n"
	"    N   - Data Block number;\n"
	"    off - offset in the Data Block;\n"
	"    BOff- bit of the byte for Boolean;\n"
	"    TpSz- type and size for other to Boolean types in the form \"[{tp}]{sz}\", where:\n"
	"      tp - type in one symbol from the list: b-Boolean, i-Signed integer, u-Unsigned integer, r-Real, s-String;\n"
	"      sz - the type size: b=[0...7], iu=[1,2(def),4,8], r=[4(def).8], s=[0(def10)...100].\n"
	"Examples:\n"
	"    \"DB1.12.2\", \"DB1.0xC.2\", \"DB1.12.b2\" - Boolean in DB 1, offset 12 and bit 2;\n"
	"    \"DB2.24\", \"DB2.0x18.8\" - Integer or Real, taken from the template's IO type, in DB 2, offset 24, size default and 8;\n"
	"    \"DB3.36.i4\", \"DB3.0x24.r8\" - directly specified Integer and Real in DB 3, offset 36, size 4 and 8;\n"
	"    \"DB4.48.20\", \"DB4.0x30.s20\" - implicitly, from the template IO type, and directly specified String in DB 4, offset 48 and size 20.\n"
	"  Link's template, of the column \"Value\" on the template forming side, writes in the form \"{LnkName}|{OffDB[.{bit}]}[|{TpSz}]\", where:\n"
	"    LnkName - name of the group link; all references with the same name are grouped and\n"
	"              indicated as one reference to the data block or data block with the specified offset;\n"
	"    OffDB   - offset number in the data block; if you specify only the data block, when configuring the template, this offset will be specified for the parameter;\n"
	"              if the configuration of the template also specifies an offset, both offsets will be summed together;\n"
	"              the data block number and the offset can be specified both in the decimal (3245) and hexadecimal format (0xCAD);\n"
	"    bit     - bit number for Boolean;\n"
	"    TpSz    - equal to the same field above.\n"
	"  Examples:\n"
	"    \"Grp 1|0.0\" - Boolean in \"Grp 1\", offset 0 and bit 0;\n"
	"    \"Grp 2|10|1\", \"Grp 2|0xA|i1\" - Integer, from the template IO type and directly, in \"Grp 2\", offset 10 and size 1;\n"
	"    \"Grp 3|20|20\", \"Grp 3|0x13|s20\" - String, from the template IO type and directly, in \"Grp 3\", offset 20 and size 20.\n\n"
	"Common address format:\n") + TPrmTempl::Impl::lnkHelp();
}

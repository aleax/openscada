
//OpenSCADA system module DAQ.Siemens file: siemens.cpp
/***************************************************************************
 *   Copyright (C) 2006-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_NAME	_("Siemens DAQ")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"2.0.15"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides a data source PLC Siemens by means of Hilscher CIF cards, by using the MPI protocol,\
 and Libnodave library, or self, for the rest.")
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
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
	if(cif_devs[i_b].present) DevExitBoard(i_b);
    if(drvCIFOK())	DevCloseDriver();
}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's DB structure
    fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","0","0;10000"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"4","30","1;3600"));
    fldAdd(new TFld("ASINC_WR",_("Asynchronous write mode"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("TYPE",_("Connection type"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d;%d;%d",TMdContr::CIF_PB,TMdContr::ISO_TCP,TMdContr::ISO_TCP243,TMdContr::ADS,TMdContr::SELF_ISO_TCP).c_str(),
	"CIF_PB;ISO_TCP (LibnoDave);ISO_TCP243 (LibnoDave);ADS;ISO_TCP"));
    fldAdd(new TFld("ADDR",_("Remote controller address"),TFld::String,TFld::NoFlag,"100","10"));
    fldAdd(new TFld("ADDR_TR",_("Output transport"),TFld::String,TFld::NoFlag,"40"));
    fldAdd(new TFld("SLOT",_("Slot CPU"),TFld::Integer,TFld::NoFlag,"2","2","0;30"));
    fldAdd(new TFld("CIF_DEV",_("CIF board"),TFld::Integer,TFld::NoFlag,"1","0","0;3"));
    // Parameter type DB structure
    int t_prm = tpParmAdd("logic","PRM_BD",_("Logical"));
    tpPrmAt(t_prm).fldAdd( new TFld("TMPL",_("Parameter template"),TFld::String,TCfg::NoVal,"50","") );
    // Parameter template IO DB structure
    elPrmIO.fldAdd(new TFld("PRM_ID",_("Parameter ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elPrmIO.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elPrmIO.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::NoFlag,"200"));

    //CIF devices DB structure
    elCifDev.fldAdd(new TFld("ID",_("ID"),TFld::Integer,TCfg::Key,"1"));
    elCifDev.fldAdd(new TFld("ADDR",_("Address"),TFld::Integer,TFld::NoFlag,"3","5"));
    elCifDev.fldAdd(new TFld("SPEED",_("Speed"),TFld::Integer,TFld::NoFlag,"1","7"));

    //Clear CIF devices info
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++) {
	cif_devs[i_b].present = false;
	cif_devs[i_b].board = -1;
	cif_devs[i_b].phAddr = 0;
	cif_devs[i_b].irq = 0;
	cif_devs[i_b].fwname  = _("No device");
	cif_devs[i_b].fwver = "";
	cif_devs[i_b].pbspeed = cif_devs[i_b].pbaddr = 0;
    }
}

void TTpContr::load_( )
{
    //Load parameters from command line

    //Load CIF devices configuration
    TConfig cfg(&CIFDevE());
    string bd_tbl = modId()+"_CIFdevs";
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++) {
	cfg.cfg("ID").setI(i_b);
	if(SYS->db().at().dataGet(SYS->workDB()+"."+bd_tbl,mod->nodePath()+bd_tbl,cfg,false,true)) {
	    cif_devs[i_b].pbaddr = cfg.cfg("ADDR").getI();
	    cif_devs[i_b].pbspeed = cfg.cfg("SPEED").getI();
	}
	if(drvCIFOK())	initCIF(i_b);
    }
}

void TTpContr::save_( )
{
    //Save CIF devices configuration
    TConfig cfg(&CIFDevE());
    string bd_tbl = modId()+"_CIFdevs";
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++) {
	cfg.cfg("ID").setI(i_b);
	cfg.cfg("ADDR").setI(cif_devs[i_b].pbaddr);
	cfg.cfg("SPEED").setI(cif_devs[i_b].pbspeed);
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
	    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++) {
		cif_devs[i_b].present = (brd_info.tBoard[i_b].usAvailable==1);
		if(!cif_devs[i_b].present) continue;
		cif_devs[i_b].board = brd_info.tBoard[i_b].usBoard;
		cif_devs[i_b].phAddr = brd_info.tBoard[i_b].ulPhysicalAddress;
		cif_devs[i_b].irq = brd_info.tBoard[i_b].usIrq;
		unsigned char fwInfo[300];
		if(DevGetInfo(i_b,GET_FIRMWARE_INFO,sizeof(FIRMWAREINFO),&fwInfo[0]) == DRV_NO_ERROR) {
		    cif_devs[i_b].fwname.assign((char *)&fwInfo[0],16);
		    cif_devs[i_b].fwver.assign((char *)&fwInfo[16],16);
		}
		DevInitBoard(i_b);
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
	throw TError(nodePath().c_str(), _("CIF device %d or device driver error!"), dev);
    if(!cif_devs[dev].present)	return;

    ResAlloc resource(cif_devs[dev].res, true);

    // Load parameters to board
    //  Running board aplications
    if((sRet = DevSetHostState(dev,HOST_READY,0L)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("CIF device %d running (DevSetHostState(HOST_READY)) error!"), dev);
    //  Load the protocol task parameters
    memset(&DPParameter, 0, sizeof(DPM_PLC_PARAMETER));
    DPParameter.bMode = DPM_SET_MODE_UNCONTROLLED;
    DPParameter.bFormat = 1;
    DPParameter.usWatchDogTime = 1000;
    if((sRet=DevPutTaskParameter(dev,1,sizeof(DPM_PLC_PARAMETER),&DPParameter)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("CIF device %d. DevPutTaskParameter() error!"), dev);
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
	    throw TError(nodePath().c_str(), _("CIF device %d. DevPutMessage() error!"), dev);
	if((sRet=DevGetMessage(dev,sizeof(tMsg),(MSG_STRUC *)&tMsg,200L)) != DRV_NO_ERROR)
	    throw TError(nodePath().c_str(), _("CIF device %d. DevGetMessage() error!"), dev);
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
	throw TError(nodePath().c_str(), _("Sending request error: %d."),sRet);
    if((sRet = DevGetMessage(dev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg, 200L))!=DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("Getting request error: %d."),sRet);
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

    if(!cif_devs[board].present) throw TError(nodePath().c_str(), _("%d:Board %d is not present."), 15, board);

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
	throw TError(nodePath().c_str(), _("%d:Sending request error %d."), 12, res);
    if((res=DevGetMessage(board,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg, 200L)) != DRV_NO_ERROR)
	throw TError(nodePath().c_str(), _("%d:Getting request error %d."), 13, res);

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
	if(ctrMkNode("area",opt,1,"/PB",_("Profibus"))) {
	    ctrMkNode("fld",opt,-1,"/PB/dev",_("CIF device"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
	    ctrMkNode("list",opt,-1,"/PB/lifels",_("Life stations list"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
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

	    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++) {
		if(n_brd)	n_brd->childAdd("el")->setText(i2s(cif_devs[i_b].board));
		if(n_fwnm)	n_fwnm->childAdd("el")->setText(cif_devs[i_b].fwname);
		if(n_fwver)	n_fwver->childAdd("el")->setText(cif_devs[i_b].fwver);
		if(n_phAddr)	n_phAddr->childAdd("el")->setText(i2s(cif_devs[i_b].phAddr,TSYS::Hex));
		if(n_irq)	n_irq->childAdd("el")->setText(i2s(cif_devs[i_b].irq));
		if(n_pba)	n_pba->childAdd("el")->setText(i2s(cif_devs[i_b].pbaddr));
		if(n_pbspd)	n_pbspd->childAdd("el")->setText(i2s(cif_devs[i_b].pbspeed));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int brd = s2i(opt->attr("row"));
	    string col  = opt->attr("col");
	    if(brd < 0 || brd > 3)	throw TError(nodePath().c_str(), _("Board number %d error!"), brd);
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
	mPerOld(cfg("PERIOD").getId()), mPrior(cfg("PRIOR").getId()), mType(cfg("TYPE").getId()),
	mSlot(cfg("SLOT").getId()), mDev(cfg("CIF_DEV").getId()), restTm(cfg("TM_REST").getId()), mAssincWR(cfg("ASINC_WR").getBd()),
	prcSt(false), callSt(false), endrunReq(false), isReload(false), isInitiated(false), alSt(-1), conErr(dataRes()), mInvokeID(-1),
	di(NULL), dc(NULL), mPer(1e9), numR(0), numW(0), numErr(0), tmDelay(0)
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
	if(!prcSt) rez += TSYS::strMess(_("Task terminated! "));
	if(tmDelay > -1) {
	    rez += (conErr.getVal().size() ? conErr.getVal(): string(_("Connect error."))) + " ";
	    if(tmDelay) rez += TSYS::strMess(_("Restoring in %.6g s. "), tmDelay);
	    rez.replace(0, 1, i2s(ConnErrCode));
	}
	else {
	    if(callSt)	rez += TSYS::strMess(_("Call now. "));
	    if(period())rez += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-9*period()).c_str());
	    else rez += TSYS::strMess(_("Call next by cron '%s'. "), atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
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

    //Check for get old period method value
    if(mPerOld) { cfg("SCHEDULE").setS(r2s(mPerOld/1e3)); mPerOld = 0; modif(true); }
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

void TMdContr::regVal( SValData ival, IO::Type itp, bool wr )
{
    if(ival.db < 0 || ival.off < 0)	return;

    int iv_sz = valSize(itp, ival.sz);	//Get real value's size

    ResAlloc res(reqDataRes, true);

    //Register to acquisition block
    unsigned i_b;
    bool plcOK = false;
    for(i_b = 0; i_b < acqBlks.size(); i_b++)
	if(ival.db < acqBlks[i_b].db) break;
	else if(acqBlks[i_b].db == ival.db) {
	    if(ival.off < acqBlks[i_b].off) {
		if((acqBlks[i_b].val.size()+acqBlks[i_b].off-ival.off) < MaxLenReq) {
		    acqBlks[i_b].val.insert(0, acqBlks[i_b].off-ival.off, 0);
		    acqBlks[i_b].off = ival.off;
		}
		else acqBlks.insert(acqBlks.begin()+i_b,SDataRec(ival.db,ival.off,iv_sz));
	    }
	    else if((ival.off+iv_sz) > (acqBlks[i_b].off+(int)acqBlks[i_b].val.size())) {
		if((ival.off+iv_sz-acqBlks[i_b].off) < MaxLenReq)
		    acqBlks[i_b].val.append((ival.off+iv_sz)-(acqBlks[i_b].off+acqBlks[i_b].val.size()), 0);
		else continue;
	    }
	    plcOK = true;
	    break;
	}
    if(!plcOK)	acqBlks.insert(acqBlks.begin()+i_b, SDataRec(ival.db,ival.off,iv_sz));
    res.release();

    //Register to asynchronous write block
    if(wr && assincWrite()) {
	ResAlloc res(reqDataAsWrRes, true);
	plcOK = false;
	for(i_b = 0; i_b < writeBlks.size(); i_b++)
	    if(ival.db < writeBlks[i_b].db) break;
	    else if(writeBlks[i_b].db == ival.db) {
		if(ival.off < writeBlks[i_b].off) {
		    if((ival.off+iv_sz) >= writeBlks[i_b].off &&
			    (writeBlks[i_b].val.size()+writeBlks[i_b].off-ival.off) < MaxLenReq)
		    {
			writeBlks[i_b].val.insert(0,writeBlks[i_b].off-ival.off,0);
			writeBlks[i_b].off = ival.off;
		    }
		    else writeBlks.insert(writeBlks.begin()+i_b,SDataRec(ival.db,ival.off,iv_sz));
		}
		else if((ival.off+iv_sz) > (writeBlks[i_b].off+(int)writeBlks[i_b].val.size())) {
		    if(ival.off <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()) &&
			    (ival.off+iv_sz-writeBlks[i_b].off) < MaxLenReq) {
			writeBlks[i_b].val.append((ival.off+iv_sz)-(writeBlks[i_b].off+writeBlks[i_b].val.size()),0);
			//Check for allow mergin to next block
			if(i_b+1 < writeBlks.size() && writeBlks[i_b+1].db == ival.db &&
				(writeBlks[i_b].off+(int)writeBlks[i_b].val.size()) >= writeBlks[i_b+1].off)
			{
			    writeBlks[i_b].val.append(writeBlks[i_b+1].val,writeBlks[i_b].off+writeBlks[i_b].val.size()-writeBlks[i_b+1].off,string::npos);
			    writeBlks.erase(writeBlks.begin()+i_b+1);
			}
		    }
		    else continue;
		}
		plcOK = true;
		break;
	    }
	if(!plcOK) writeBlks.insert(writeBlks.begin()+i_b,SDataRec(ival.db,ival.off,iv_sz));
	writeBlks[i_b].err = _("-1:No data");
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
	    if(fds.rfd <= 0) throw TError(nodePath().c_str(), _("Open socket of remote PLC error."));

	    ResAlloc res2(mod->resAPI, true);
	    di = daveNewInterface(fds, (char*)(string("IF")+id()).c_str(), 0,
		((mType==ISO_TCP243)?daveProtoISOTCP243:daveProtoISOTCP), daveSpeed187k);
	    dc = daveNewConnection(di, 2, 0, mSlot);
	    daveSetTimeout(di, 1500000);	//1.5s
	    if(daveConnectPLC(dc)) {
		close(fds.wfd);
		free(dc); dc = NULL;
		free(di); di = NULL;
		throw TError(nodePath().c_str(), _("Connection to PLC error."));
	    }
	    break;
	}
	case ADS:
	case SELF_ISO_TCP:
	    tr = SYS->transport().at().at(TSYS::strParse(addrTr(),0,".")).at().outAt(TSYS::strParse(addrTr(),1,"."));
	    //try { tr.at().start(); }
	    //catch(TError &err) { throw TError(nodePath().c_str(), _("Connection error.")); }
	    reset();
	    break;
	default: throw TError(nodePath().c_str(), _("Connection type '%d' is not supported."), mType);
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

		if(buffer.size() > 240)			throw TError(_("ReadDB"), _("Requested block is too big."));
		if(!owner().cif_devs[mDev].present)	throw TError(_("ReadDB"), _("Board %d is not present."), mDev);

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
		    if(res == DRV_DEV_PUT_TIMEOUT) throw TError(_("ReadDB"), _("Sending request is timeouted."));
		    //Get remote host's response
		    if(res == DRV_NO_ERROR) res = DevGetMessage(mDev, sizeof(RCS_MESSAGE), (MSG_STRUC *)&tMsg, 200L);
		} while((res == DRV_NO_ERROR) && (tMsg.f == 0x02 || tMsg.f == 0x39) && e_try > 0);

		//Process errors
		if(res != DRV_NO_ERROR)		throw TError(_("ReadDB"), _("Request to DB error, %d."), res);
		if(res == DRV_DEV_GET_TIMEOUT)	throw TError(_("ReadDB"), _("Get request is timeouted."));
		if(tMsg.f == 17)	throw TError(_("ReadDB"), _("There is no response from the remote station."));
		if(tMsg.f == 18)	throw TError(_("ReadDB"), _("Master is out of the logical token ring."));
		if(tMsg.f == 0x85)	throw TError(11, _("ReadDB"), _("Specified offset address or DB error."));

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
		    throw TError(nodePath().c_str(),_("12:Put request is error %d."),res);
		if((res=DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,200L)) != DRV_NO_ERROR)
		    throw TError(nodePath().c_str(),_("12:Get request is error %d."),res);*/

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
		    if(rez == daveResTimeout) throw TError(_("ReadDB"), _("Connection error."));
		    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("Read block '%d' error, %s."), n_db, daveStrerror(rez));
		    throw TError(11, _("ReadDB"), _("DB '%d' error, %s."), n_db, daveStrerror(rez));
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
		int resp_len = tr.at().messIO(buf, AmsTcpHD->len+sizeof(AMS_TCP_HEAD), res, sizeof(res), 0, true);
		int full_len = resp_len;
		if(full_len < (int)sizeof(AMS_TCP_HEAD))	throw TError(_("ReadDB"), _("Error server respond."));
		AmsTcpHD = (AMS_TCP_HEAD *)res;
		unsigned resp_sz = AmsHD->len;

		//Wait tail
		while(full_len < (int)(resp_sz+sizeof(AMS_TCP_HEAD))) {
		    resp_len = tr.at().messIO(NULL, 0, res+full_len, sizeof(res)-full_len, 0, true);
		    if(!resp_len) throw TError(_("ReadDB"), _("Not full respond."));
		    full_len += resp_len;
		}

		//Check for correct result
		AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
		ADS_ReadResp *ADSreadResp = (ADS_ReadResp *)(AmsHD+1);
		resp_len = sizeof(AMS_TCP_HEAD)+sizeof(AMS_HEAD);
		if(full_len < resp_len || AmsHD->com != 2 || AmsHD->stFlgs != 0x05)	throw TError(_("ReadDB"), _("Error server respond."));
		if(AmsHD->errCod) throw TError(11, _("ReadDB"), _("Error server respond, %d."), AmsHD->errCod);
		if(full_len < (resp_len+=sizeof(ADS_ReadResp)))	throw TError(12, _("ReadDB"), _("Error server respond."));
		if(ADSreadResp->res) throw TError(13, _("ReadDB"), _("Error server respond, %d."), ADSreadResp->res);
		if(ADSreadResp->len != buffer.size() || full_len < (int)(resp_len+ADSreadResp->len))
		    throw TError(14, _("ReadDB"), _("Error server respond."));
		buffer.replace(0, buffer.size(), res+resp_len, buffer.size());	//!!!! But assign() temporary the block size changes

		break;
	    }
	    case SELF_ISO_TCP: {
		XMLNode req("ISO-TCP");
		req.setAttr("id", "read")->setAttr("db", i2s(n_db))->setAttr("off", i2s(offset))->setAttr("size", i2s(buffer.size()));
		reqService(req);
		if(req.attr("err").size()) throw TError(s2i(req.attr("errCod")), "", "%s", req.attr("err").c_str());
		if(req.text().size() != buffer.size()) throw TError("read", _("Reply data block size, %d != %d."), req.text().size(), buffer.size());
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

		if(buffer.size() > 240)		throw TError(_("WriteDB"), _("Transmitted block is too big."));
		if(!owner().cif_devs[mDev].present)	throw TError(_("WriteDB"), _("Board %d is not present."), mDev);

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
		    if(res == DRV_DEV_PUT_TIMEOUT) throw TError(_("WriteDB"), _("Sending request is timeouted."));
		    //Get remote host's response
		    if(res == DRV_NO_ERROR) res = DevGetMessage(mDev, sizeof(RCS_MESSAGE), (MSG_STRUC *)&tMsg, 200L);
		}
		while((res == DRV_NO_ERROR) && (tMsg.f == 0x02 || tMsg.f == 0x39) && e_try > 0);

		//printf("Put DB %d:%d DB%d.%d(%d) -- %d \n",mDev,vmax(0,vmin(126,s2i(addr()))),n_db,offset,buffer.size(),tMsg.f);

		//Process errors
		if(res != DRV_NO_ERROR)		throw TError(_("WriteDB"), _("Request to DB error, %d."), res);
		if(res == DRV_DEV_GET_TIMEOUT)	throw TError(_("WriteDB"), _("Getting request is timeouted."));
		if(tMsg.f == 17)	throw TError(_("WriteDB"), _("There is no response from the remote station."));
		if(tMsg.f == 18)	throw TError(_("WriteDB"), _("Master is out of the logical token ring."));
		if(tMsg.f == 0x85)	throw TError(11, _("WriteDB"), _("Specified offset address or DB error."));
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
		    if(rez == daveResTimeout) throw TError(_("WriteDB"), _("Connection error."));
		    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("Write block '%d' error, %s."), n_db, daveStrerror(rez));
		    throw TError(11, _("WriteDB"), _("DB '%d' error, %s."), n_db, daveStrerror(rez));
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
		int resp_len = tr.at().messIO(buf, AmsTcpHD->len+sizeof(AMS_TCP_HEAD), res, sizeof(res), 0, true);
		int full_len = resp_len;
		if(full_len < (int)sizeof(AMS_TCP_HEAD)) throw TError(_("WriteDB"), _("Error server respond."));
		AmsTcpHD = (AMS_TCP_HEAD *)res;
		unsigned resp_sz = AmsHD->len;
		//Wait tail
		while(full_len < (int)(resp_sz+sizeof(AMS_TCP_HEAD))) {
		    resp_len = tr.at().messIO(NULL, 0, res+full_len, sizeof(res)-full_len, 0, true);
		    if(!resp_len) throw TError(_("WriteDB"), _("Not full respond."));
		    full_len += resp_len;
		}
		//Check for correct result
		AmsHD = (AMS_HEAD*)(AmsTcpHD+1);
		ADS_WriteResp *ADSwriteResp = (ADS_WriteResp *)(AmsHD+1);
		resp_len = sizeof(AMS_TCP_HEAD)+sizeof(AMS_HEAD);
		if(full_len < resp_len || AmsHD->com != 3 || AmsHD->stFlgs != 0x05) throw TError(_("WriteDB"), _("Error server respond."));
		if(AmsHD->errCod) throw TError(11, _("WriteDB"), _("Error server respond, %d."), AmsHD->errCod);
		if(full_len < (resp_len+=sizeof(ADS_WriteResp)))	throw TError(12, _("WriteDB"), _("Error server respond."));
		if(ADSwriteResp->res) throw TError(13, _("WriteDB"), _("Error server respond, %d."), ADSwriteResp->res);

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

    ResAlloc resN(tr.at().nodeRes(), true);
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
	    if(!(resp_len=messIO(NULL,0,buf,sizeof(buf)))) throw TError("", _("Not full respond."));
	    tpkt.append(buf, resp_len);
	}

	off = 2;
	if(tpkt.size() < 4 || iN(tpkt,off,2) != tpkt.size())	//<TPKT length
	    throw TError("", _("Respond size is not coincidence."));

								//<COTP result
	uint8_t lenCOTP = iN(tpkt, off, 1);			// Length
	uint8_t begCOTP = off;
	if(lenCOTP < 2 || tpkt.size() < (begCOTP+lenCOTP))	throw TError("COTP", _("Short."));
	uint8_t tpPDU = iN(tpkt, off, 1);			// PDU Type

	//Service's response process
	if(io.attr("id") == "connect") {
	    if(tpPDU != COTP_CC)	throw TError("COTP", _("PDU Type error, %d != %d."), tpPDU, COTP_CC);
	    iN(tpkt, off, 2);					// Dest reference
	    iN(tpkt, off, 2);					// Source reference
	    iN(tpkt, off, 1);					// Class
	    while((off-begCOTP) < lenCOTP) {			// Options
		iN(tpkt, off, 1);				//  Code
		uint8_t oLen = iN(tpkt, off, 1);		//  Length
		iVal(tpkt, off, oLen);
	    }
	    if((off-begCOTP) != lenCOTP)throw TError("COTP", _("Inconsistent length."));
	}
	else if(sCd) {
	    if(tpPDU != COTP_DT)	throw TError("COTP", _("PDU Type error, %d != %d."), tpPDU, COTP_DT);
	    iN(tpkt, off, 1);					// Class
	    uint32_t tmpV;
								//<ISO-TCP
	    if(iN(tpkt,off,1) != 0x32)				//P: allways 0x32
		throw TError(_("PDU header"), _("No start from 0x32."));
	    char iHTp = iN(tpkt, off, 1);			//Header type, one of 1,2,3 or 7. type 2 and 3 headers are two bytes longer.
	    iN(tpkt, off, 2);					//Currently unknown. Maybe it can be used for long numbers?
	    if((tmpV=iN(tpkt,off,2)) != (uint16_t)(mInvokeID-1))	//number: sequence or invoke number
		throw TError(_("PDU header"), _("Inconsistent respond's invoke ID to the request, %d != %d."), (uint16_t)(mInvokeID-1), tmpV);
	    uint16_t szPrm  = iN(tpkt, off, 2);			//plen: length of parameters which follow this header
	    uint16_t szData = iN(tpkt, off, 2);			//dlen: length of data which follow the parameters
	    uint16_t iErr = 0;
	    if(iHTp == 2 || iHTp == 3) iErr = iN(tpkt, off, 2);	//result[2]: only present in type 2 and 3 headers. This contains an error information.
	    if((off+szPrm+szData) > (int)tpkt.size()) throw TError(_("PDU header"), _("Parameters + data parts size more to the respond size."));

	    switch(sCd) {
		case ISOTCP_OpenS7Connection: {
		    if(szPrm != 8) throw TError(io.attr("id").c_str(), _("Parameters part size is not equal to expected, %d."), 8);
		    if((int)iN(tpkt,off,1) != sCd) throw TError(io.attr("id").c_str(), _("Respond function mismatch."));
		    iN(tpkt, off, 1);
		    iN(tpkt, off, 2);
		    iN(tpkt, off, 2);
		    io.setAttr("len", i2s(iN(tpkt,off,2)));
		    break;
		}
		case ISOTCP_Read: {
		    if(szPrm != 2) throw TError(io.attr("id").c_str(), _("Parameters part size is not equal to expected, %d."), 2);
		    if((int)iN(tpkt,off,1) != sCd) throw TError(io.attr("id").c_str(), _("Respond function mismatch."));
		    iN(tpkt, off, 1);
		    // Data part
		    uint8_t iDErr = iN(tpkt, off, 1);
		    if(iDErr != 0xFF) {
			if(iDErr == 5) throw TError(12, io.attr("id").c_str(), _("Try access beyond the DB '%s' range %s(%s)."),
					io.attr("db").c_str(), io.attr("off").c_str(), io.attr("size").c_str());
			else if(iDErr == 10) throw TError(11, io.attr("id").c_str(), _("DB '%s' is not exist."), io.attr("db").c_str());
			else throw TError(13, io.attr("id").c_str(), _("Data access unknown error, %xh."), iDErr);
		    }
		    char iDLenTp = iN(tpkt, off, 1);
		    uint16_t iDLen = iN(tpkt, off, 2);
		    if(iDLenTp == 4) iDLen = iDLen/8;	//len is in bits, adjust
		    else if(iDLenTp == 9) ;		//len is already in bytes, ok
		    else if(iDLenTp == 3) ;		//len is in bits, but there is a byte per result bit, ok
		    else throw TError(14, io.attr("id").c_str(), _("Unknown data encoding type, %d."), iDLenTp);
		    if((off+iDLen) != (int)tpkt.size())
			throw TError(15, io.attr("id").c_str(), _("Inconsistent data block size to the respond size, %d(%d)."),
								    (off+iDLen), tpkt.size());
		    io.setText(tpkt.substr(off));
		    break;
		}
		case ISOTCP_Write: {
		    if(szPrm != 2) throw TError(io.attr("id").c_str(), _("Parameters part size is not equal to expected, %d."), 2);
		    if((int)iN(tpkt,off,1) != sCd) throw TError(io.attr("id").c_str(), _("Respond function mismatch."));
		    iN(tpkt, off, 1);
		    // Data part
		    uint8_t iDErr = iN(tpkt, off, 1);
		    if(iDErr != 0xFF) {
			if(iDErr == 5) throw TError(12, io.attr("id").c_str(), _("Try access beyond the DB '%s' range %s(%s)."),
					io.attr("db").c_str(), io.attr("off").c_str(), io.attr("size").c_str());
			else if(iDErr == 10) throw TError(11, io.attr("id").c_str(), _("DB '%s' is not exist."), io.attr("db").c_str());
			else throw TError(13, io.attr("id").c_str(), _("Data access unknown error, %xh."), iDErr);
		    }
		    break;
		}
	    }
	}
    } catch(TError &er) { io.setAttr("err", (er.cat.empty()?"":er.cat+":")+er.mess)->setAttr("errCod", i2s(er.cod)); }
}

int TMdContr::messIO( const char *oBuf, int oLen, char *iBuf, int iLen )
{
    return tr.at().messIO(oBuf, oLen, iBuf, iLen, ((enableStat() && !isReload)?0:1000), true);
}

void TMdContr::reset( )
{
    isInitiated = false;
    mInvokeID = -1;
}

char TMdContr::getValB( SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty())err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return EVAL_BOOL;
    }

    ResAlloc res(reqDataRes, false);
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+1) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	{
	    if(!acqBlks[i_b].err.size())
		return (bool)(acqBlks[i_b].val[ival.off-acqBlks[i_b].off]&(0x01<<ival.sz));
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty())	err = TSYS::strMess(_("%d:Value is not gathered."), 11);

    return EVAL_BOOL;
}

int64_t TMdContr::getValI( SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return EVAL_INT;
    }

    int iv_sz = valSize(IO::Integer, ival.sz);
    ResAlloc res(reqDataRes, false);
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	{
	    if(!acqBlks[i_b].err.size())
		switch(iv_sz) {
		    case 1:	return acqBlks[i_b].val[ival.off-acqBlks[i_b].off];
		    case 2:	return *(int16_t*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str();
		    case 4:	return *(int32_t*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str();
		}
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err = TSYS::strMess(_("%d:Value is not gathered."), 11);

    return EVAL_INT;
}

double TMdContr::getValR( SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return EVAL_REAL;
    }

    int iv_sz = valSize(IO::Real, ival.sz);
    ResAlloc res(reqDataRes, false);
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	{
	    if(!acqBlks[i_b].err.size())
		switch(iv_sz) {
		    case 4:	return TSYS::floatLErev(*(float*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str());
		    case 8:	return TSYS::doubleLErev(*(double*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str());
		}
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err = TSYS::strMess(_("%d:Value is not gathered."), 11);

    return EVAL_REAL;
}

string TMdContr::getValS( SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return EVAL_STR;
    }

    int iv_sz = valSize(IO::String, ival.sz);
    ResAlloc res(reqDataRes, false);
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()) )
	{
	    if(!acqBlks[i_b].err.size()) return acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz);
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err = TSYS::strMess(_("%d:Value is not gathered."), 11);

    return EVAL_STR;
}

void TMdContr::setValB( bool ivl, SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return;
    }

    int64_t val = getValI(SValData(ival.db,ival.off,1), err);
    if(val == EVAL_INT || (bool)(val&(0x01<<ival.sz)) == ivl) return;
    //Write data to controller or write data block
    val ^= (0x01<<ival.sz);
    try {
	if(!assincWrite()) putDB(ival.db,ival.off,string((char*)&val,1));
	else {
	    ResAlloc res(reqDataAsWrRes, false);
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
		    (ival.off+1) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val[ival.off-writeBlks[i_b].off] = val;
		    if(s2i(writeBlks[i_b].err.getVal()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	}
	//Set to DB buffer
	ResAlloc res(reqDataRes, false);
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+1) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    { acqBlks[i_b].val[ival.off-acqBlks[i_b].off] = val; break; }
    } catch(TError &cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

void TMdContr::setValI( int64_t ivl, SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return;
    }

    int64_t val = getValI(ival, err);
    if(val == EVAL_INT || val == ivl) return;
    //Write data to controller or write data block
    val = ivl;
    int iv_sz = valSize(IO::Integer, ival.sz);
    try {
	if(!assincWrite()) putDB(ival.db,ival.off,revers(string((char *)&val,iv_sz)));
	else {
	    ResAlloc res(reqDataAsWrRes, false);
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
			(ival.off+iv_sz) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val.replace(ival.off-writeBlks[i_b].off,iv_sz,revers(string((char *)&val,iv_sz)));
		    if(s2i(writeBlks[i_b].err.getVal()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	}
	//Set to DB buffer
	ResAlloc res(reqDataRes, false);
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    { acqBlks[i_b].val.replace(ival.off-acqBlks[i_b].off,iv_sz,revers(string((char*)&val,iv_sz))); break; }
    } catch(TError &cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

void TMdContr::setValR( double ivl, SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return;
    }

    double val = getValR(ival, err);
    if(val == EVAL_REAL || val == ivl) return;
    //Write data to controller or write data block
    val = TSYS::doubleLE(ivl);
    float val_4 = TSYS::floatLE(ivl);
    int iv_sz = valSize(IO::Real, ival.sz);
    try {
	if(!assincWrite()) putDB(ival.db,ival.off,revers(string((iv_sz==4)?(char*)&val_4:(char *)&val,iv_sz)));
	else {
	    ResAlloc res(reqDataAsWrRes, false);
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
			(ival.off+iv_sz) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val.replace(ival.off-writeBlks[i_b].off,iv_sz,revers(string(((iv_sz==4)?(char *)&val_4:(char *)&val),iv_sz)));
		    if(s2i(writeBlks[i_b].err.getVal()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	}
	//Set to DB buffer
	ResAlloc res(reqDataRes, false);
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    {
		acqBlks[i_b].val.replace(ival.off-acqBlks[i_b].off,iv_sz,revers(string(((iv_sz==4)?(char *)&val_4:(char *)&val),iv_sz)));
		break;
	    }
    } catch(TError &cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

void TMdContr::setValS( const string &ivl, SValData ival, ResString &err )
{
    if(tmDelay > 0) {
	if(err.getVal().empty()) err = TSYS::strMess("%d:%s", ConnErrCode, conErr.getVal().c_str());
	return;
    }

    string val = getValS(ival,err);
    int iv_sz = valSize(IO::String, ival.sz);
    string vali = ivl;
    vali.resize(iv_sz);
    if(val == EVAL_STR || val == vali) return;
    //Write data to controller or write data block
    try {
	if(!assincWrite())	putDB(ival.db, ival.off, vali);
	else {
	    ResAlloc res(reqDataAsWrRes, false);
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
			(ival.off+iv_sz) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val.replace(ival.off-writeBlks[i_b].off,iv_sz,vali);
		    if(s2i(writeBlks[i_b].err.getVal()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	}
	//Set to DB buffer
	ResAlloc res(reqDataRes, false);
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    { acqBlks[i_b].val.replace(ival.off-acqBlks[i_b].off,iv_sz,vali); break; }
    } catch(TError &cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

int TMdContr::valSize( IO::Type itp, int iv_sz )
{
    switch(itp) {
	case IO::String:	return (iv_sz <= 0) ? 10 : iv_sz;
	case IO::Integer:	return (iv_sz == 1 || iv_sz == 2 || iv_sz == 4) ? iv_sz : 2;
	case IO::Real:		return (iv_sz == 4 || iv_sz == 8) ? iv_sz : 4;
	case IO::Boolean:	return 1;
	default: break;
    }

    throw TError(nodePath().c_str(), _("Value type error."));
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
	    else if(cntr.tmDelay == 0) cntr.conErr = _("Connecting...");

	    cntr.callSt = true;
	    if(!cntr.period()) t_cnt = TSYS::curTime();

	    //Update controller's data
	    //Process write data blocks
	    if(cntr.assincWrite()) {
		ResAlloc res(cntr.reqDataAsWrRes, false);
		for(unsigned i_b = 0; !isStart && !isStop && !cntr.endrunReq && i_b < cntr.writeBlks.size(); i_b++)
		    try {
			if(cntr.redntUse()) { cntr.writeBlks[i_b].err = _("-1:No data"); continue; }
			if(s2i(cntr.writeBlks[i_b].err.getVal()) == -1) continue;
			cntr.putDB(cntr.writeBlks[i_b].db,cntr.writeBlks[i_b].off,cntr.writeBlks[i_b].val);
			cntr.writeBlks[i_b].err = _("-1:No data");
		    } catch(TError &err) { cntr.writeBlks[i_b].err = err.mess; }
		if(cntr.tmDelay > 0) continue;
	    }
	    ResAlloc res(cntr.reqDataRes, false);
	    //Process acquisition data blocks
	    for(unsigned i_b = 0; !isStart && !isStop && !cntr.endrunReq && i_b < cntr.acqBlks.size(); i_b++)
		try {
		    if(cntr.redntUse()) { cntr.acqBlks[i_b].err = _("-1:No data"); continue; }
		    cntr.getDB(cntr.acqBlks[i_b].db, cntr.acqBlks[i_b].off, cntr.acqBlks[i_b].val);
		    cntr.acqBlks[i_b].err = "";
		} catch(TError &err) { cntr.acqBlks[i_b].err = err.mess; }
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
    err = TSYS::strMess(_("%d:Value is not gathered."), 11);
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
	    "help",_("Connection type:\n"
		     "  CIF_PB - connection to controllers series S7, by firm Siemens, by communication unit CIF-50PB or like;\n"
		     "  ISO_TCP, ISO_TCP243 - connection to controllers series S7, by firm Siemens, by Ethernet network (TCP243 by CP243);\n"
		     "  ADS - TwinCAT ADS/AMS protocol for connection to controllers firm Beckhoff."));
	if(type() != SELF_ISO_TCP)
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,
		"help",_("Remote controller address. For connections:\n"
		     "  CIF_PB - controller address in \"Profibus\" network, digit 0-255;\n"
		     "  ISO_TCP, ISO_TCP243 - IP-address into Ethernet network;\n"
		     "  ADS - Network identifier and port for target and source stations, in view\n"
		     "      \"{Target_AMSNetId}:{Target_AMSPort}|{Source_AMSNetId}:{Source_AMSPort}\"\n"
		     "      (for example: \"192.168.0.1.1.1:801|82.207.88.73.1.1:801\"), where:\n"
		     "    AMSNetId - network identifier, write into view of six digits 0-255, for example: \"192.168.0.1.1.1\";\n"
		     "    AMSPort - port, write into view digit 0-65535."));
	else ctrRemoveNode(opt, "/cntr/cfg/ADDR");
	if(type() == ADS || type() == SELF_ISO_TCP)
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR_TR",EVAL_STR,RWRWR_,"root",SDAQ_ID,2, "dest","select", "select","/cntr/cfg/trLst");
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
    TParamContr(name,tp_prm), TValFunc(name+"SiemensPrm"), pEl("cif_attr"),
    idFreq(-1), idStart(-1), idStop(-1), idErr(-1), idSh(-1), idNm(-1), idDscr(-1), acqErrTm(0)
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
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
	vector<string> als;
	bool to_make = false;
	unsigned fId = 0;
	if(!func()) {
	    setFunc(&SYS->daq().at().tmplLibAt(TSYS::strSepParse(cfg("TMPL").getS(),0,'.')).at().
				     at(TSYS::strSepParse(cfg("TMPL").getS(),1,'.')).at().func().at());
	    to_make = true;
	}
	// Init attrubutes
	for(int i_io = 0; i_io < func()->ioSize(); i_io++) {
	    if((func()->io(i_io)->flg()&TPrmTempl::CfgLink) && lnkId(i_io) < 0)	plnk.push_back(SLnk(i_io));
	    if((func()->io(i_io)->flg()&(TPrmTempl::AttrRead|TPrmTempl::AttrFull))) {
		unsigned flg = TVal::DirWrite|TVal::DirRead;
		if(func()->io(i_io)->flg()&IO::FullText)	flg |= TFld::FullText;
		if(func()->io(i_io)->flg()&TPrmTempl::AttrRead)	flg |= TFld::NoWrite;
		TFld::Type tp = TFld::type(ioType(i_io));
		if((fId=pEl.fldId(func()->io(i_io)->id(),true)) < pEl.fldSize()) {
		    if(pEl.fldAt(fId).type() != tp)
			try{ pEl.fldDel(fId); } catch(TError &err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
		    else {
			pEl.fldAt(fId).setFlg(flg);
			pEl.fldAt(fId).setDescr(func()->io(i_io)->name().c_str());
		    }
		}

		if(!vlPresent(func()->io(i_io)->id()))
		    pEl.fldAdd(new TFld(func()->io(i_io)->id().c_str(),func()->io(i_io)->name().c_str(),tp,flg));

		als.push_back(func()->io(i_io)->id());
	    }
	    if(to_make && (func()->io(i_io)->flg()&TPrmTempl::CfgLink)) setS(i_io,"0");
	}

	// Init links
	initLnks();

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

	//Check for delete DAQ parameter's attributes
	for(int i_p = 0; i_p < (int)pEl.fldSize(); i_p++) {
	    unsigned i_l;
	    for(i_l = 0; i_l < als.size(); i_l++)
		if(pEl.fldAt(i_p).name() == als[i_l])
		    break;
	    if(i_l >= als.size())
		try{ pEl.fldDel(i_p); i_p--; }
		catch(TError &err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
	}

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
    setFunc(NULL);
    idFreq = idStart = idStop = idErr = idSh = idNm = idDscr = -1;
    plnk.clear();

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

    for(int i_io = 0; i_io < ioSize(); i_io++) {
	cfg.cfg("ID").setS(func()->io(i_io)->id());
	if(!SYS->db().at().dataGet(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg,false,true)) continue;
	if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)
	    lnk(lnkId(i_io)).dbAddr = cfg.cfg("VALUE").getS();
	else setS(i_io,cfg.cfg("VALUE").getS());
    }
    initLnks();
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

    for(int i_io = 0; i_io < func()->ioSize(); i_io++) {
	cfg.cfg("ID").setS(func()->io(i_io)->id());
	if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)
	    cfg.cfg("VALUE").setS(lnk(lnkId(i_io)).dbAddr);
	else cfg.cfg("VALUE").setS(getS(i_io));
	SYS->db().at().dataSet(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg);
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat()) {
	if(val.name() == "err") {
	    if(!enableStat())			val.setS(_("1:Parameter is disabled."), 0, true);
	    else if(!owner().startStat())	val.setS(_("2:Controller is stopped."), 0, true);
	} else val.setS(EVAL_STR, 0, true);
	return;
    }
    if(owner().redntUse()) return;
    if(val.name() != "err")
	try {
	    int id_lnk = lnkId(val.name());
	    if(id_lnk >= 0 && lnk(id_lnk).val.db < 0) id_lnk = -1;
	    if(id_lnk < 0) val.set(get(ioId(val.name())),0,true);
	    else switch(val.fld().type()) {
		case TFld::String:	val.setS(owner().getValS(lnk(id_lnk).val,acqErr),0,true);	break;
		case TFld::Integer:	val.setI(owner().getValI(lnk(id_lnk).val,acqErr),0,true);	break;
		case TFld::Real:	val.setR(owner().getValR(lnk(id_lnk).val,acqErr),0,true);	break;
		case TFld::Boolean:	val.setB(owner().getValB(lnk(id_lnk).val,acqErr),0,true);	break;
		default: break;
	    }
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
	if(id_lnk >= 0 && lnk(id_lnk).val.db < 0) id_lnk = -1;
	if(!(vl.isEVal() || vl == pvl)) {
	    if(id_lnk < 0) set(ioId(vo.name()), vl);
	    else switch(vo.fld().type()) {
		case TFld::String:	owner().setValS(vl.getS(), lnk(id_lnk).val, acqErr);	break;
		case TFld::Integer:	owner().setValI(vl.getI(), lnk(id_lnk).val, acqErr);	break;
		case TFld::Real:	owner().setValR(vl.getR(), lnk(id_lnk).val, acqErr);	break;
		case TFld::Boolean:	owner().setValB(vl.getB(), lnk(id_lnk).val, acqErr);	break;
		default: break;
	    }
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

int TMdPrm::lnkSize( )
{
    if(!enableStat()) throw TError(nodePath().c_str(), _("Parameter is disabled."));
    return plnk.size();
}

int TMdPrm::lnkId( int id )
{
    if(!enableStat()) throw TError(nodePath().c_str(), _("Parameter is disabled."));
    for(unsigned i_l = 0; i_l < plnk.size(); i_l++)
	if(lnk(i_l).ioId == id)
	    return i_l;
    return -1;
}

int TMdPrm::lnkId( const string &id )
{
    if(!enableStat()) throw TError(nodePath().c_str(), _("Parameter is disabled."));
    for(unsigned i_l = 0; i_l < plnk.size(); i_l++)
	if(func()->io(lnk(i_l).ioId)->id() == id)
	    return i_l;
    return -1;
}

TMdPrm::SLnk &TMdPrm::lnk( int num )
{
    if(!enableStat() || num < 0 || num >= (int)plnk.size())
	throw TError(nodePath().c_str(), _("Parameter is disabled or id error."));
    return plnk[num];
}

void TMdPrm::initLnks()
{
    if(!enableStat())	return;
    //Init links
    for(int i_l = 0; i_l < lnkSize(); i_l++) {
	if(ioType(lnk(i_l).ioId) == IO::Boolean) {
	    if(sscanf(lnk(i_l).dbAddr.c_str(),"DB%i.%i.%d",&lnk(i_l).val.db,&lnk(i_l).val.off,&lnk(i_l).val.sz) == 3)
		lnk(i_l).val.sz = vmax(0,vmin(7,lnk(i_l).val.sz));
	    else lnk(i_l).val.db = -1;
	}
	else {
	    if(sscanf(lnk(i_l).dbAddr.c_str(),"DB%i.%i",&lnk(i_l).val.db,&lnk(i_l).val.off) == 2)
		lnk(i_l).val.sz = s2i(TSYS::strParse(func()->io(lnk(i_l).ioId)->def(),2,"|"));
	    else lnk(i_l).val.db = -1;
	}
	// Process data
	if(lnk(i_l).val.db < 0 || lnk(i_l).val.off < 0) lnk(i_l).val.db = lnk(i_l).val.off = -1;
	else owner().regVal(lnk(i_l).val,ioType(lnk(i_l).ioId),ioFlg(lnk(i_l).ioId)&(IO::Output|IO::Return));
    }
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

	//Set fixed system attributes
	if(idFreq >= 0)	setR(idFreq, frq);
	if(idStart >= 0)setB(idStart, first);
	if(idStop >= 0)	setB(idStop, last);
	if(idSh >= 0)	setS(idSh, id());
	if(idNm >= 0)	setS(idNm, name());
	if(idDscr >= 0)	setS(idDscr, descr());

	//Get input links
	for(int i_l = 0; i_l < lnkSize(); i_l++)
	    if(lnk(i_l).val.db >= 0)
		switch(ioType(lnk(i_l).ioId)) {
		    case IO::String:	setS(lnk(i_l).ioId,owner().getValS(lnk(i_l).val,acqErr));	break;
		    case IO::Integer:	setI(lnk(i_l).ioId,owner().getValI(lnk(i_l).val,acqErr));	break;
		    case IO::Real:	setR(lnk(i_l).ioId,owner().getValR(lnk(i_l).val,acqErr));	break;
		    case IO::Boolean:	setB(lnk(i_l).ioId,owner().getValB(lnk(i_l).val,acqErr));	break;
		    case IO::Object: break;
		}

	//Calc template
	TValFunc::calc();
	modif();

	//Put output links
	for(int i_l = 0; i_l < lnkSize(); i_l++)
	    if(lnk(i_l).val.db >= 0 && ioFlg(lnk(i_l).ioId)&(IO::Output|IO::Return))
		switch(ioType(lnk(i_l).ioId)) {
		    case IO::String:	owner().setValS(getS(lnk(i_l).ioId),lnk(i_l).val,acqErr);	break;
		    case IO::Integer:	owner().setValI(getI(lnk(i_l).ioId),lnk(i_l).val,acqErr);	break;
		    case IO::Real:	owner().setValR(getR(lnk(i_l).ioId),lnk(i_l).val,acqErr);	break;
		    case IO::Boolean:	owner().setValB(getB(lnk(i_l).ioId),lnk(i_l).val,acqErr);	break;
		    case IO::Object: break;
		}

	//Put fixed system attributes
	if(idNm >= 0)	setName(getS(idNm));
	if(idDscr >= 0)setDescr(getS(idDscr));

    } catch(TError &err) {
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Error calculate template."));
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
	if(enableStat() && ctrMkNode("area",opt,-1,"/cfg",_("Template configuration"))) {
	    ctrMkNode("fld",opt,-1,"/cfg/only_off",_("Only DB offsets are to be shown"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/cfg/prm",_("Parameters")))
		for(int i_io = 0; i_io < ioSize(); i_io++) {
		    if(!(func()->io(i_io)->flg()&(TPrmTempl::CfgLink|TPrmTempl::CfgConst)))
			continue;
		    // Check select param
		    bool is_lnk = func()->io(i_io)->flg()&TPrmTempl::CfgLink;
		    if(is_lnk && func()->io(i_io)->def().size() &&
			!s2i(TBDS::genDBGet(mod->nodePath()+"onlOff","0",opt->attr("user"))))
		    {
			string nprm = TSYS::strSepParse(func()->io(i_io)->def(),0,'|');
			// Check already to present parameters
			bool f_ok = false;
			for(unsigned i_l = 0; i_l < list.size() && !f_ok; i_l++)
			    if(list[i_l] == nprm) f_ok = true;
			if(!f_ok) {
			    ctrMkNode("fld",opt,-1,(string("/cfg/prm/pr_")+i2s(i_io)).c_str(),nprm,RWRWR_,"root",SDAQ_ID,1,"tp","str");
			    list.push_back(nprm);
			}
		    }
		    else {
			const char *tip = "str";
			bool fullTxt = false;
			if(!is_lnk)
			    switch(ioType(i_io)) {
				case IO::Integer:	tip = "dec";	break;
				case IO::Real:		tip = "real";	break;
				case IO::Boolean:	tip = "bool";	break;
				case IO::String:
				    if(func()->io(i_io)->flg()&IO::FullText) fullTxt = true;
				    break;
				case IO::Object:	fullTxt = true; break;
			    }
			XMLNode *wn = ctrMkNode("fld",opt,-1,(string("/cfg/prm/el_")+i2s(i_io)).c_str(),func()->io(i_io)->name(),RWRWR_,
			    "root",SDAQ_ID,1,"tp",tip);
			if(wn && fullTxt) wn->setAttr("cols","100")->setAttr("rows","4");
		    }
		}
	}
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
    else if(a_path.compare(0,12,"/cfg/prm/pr_") == 0 && enableStat()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    int lnk_id = lnkId(s2i(a_path.substr(12)));
	    string sdb = TSYS::strSepParse(lnk(lnk_id).dbAddr, 0, '.');
	    int off = s2i(TSYS::strSepParse(lnk(lnk_id).dbAddr, 1, '.'));
	    int t_off = s2i(TSYS::strSepParse(func()->io(lnk(lnk_id).ioId)->def(),1,'|'));
	    if((off-t_off) > 0)	sdb = sdb + "." + i2s(off-t_off);
	    opt->setText(sdb);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    string p_nm, cp_nm;
	    int off, coff, cbit;
	    int lnk_id = lnkId(s2i(a_path.substr(12)));
	    string sdb = TSYS::strSepParse(opt->text(), 0, '.');
	    off = strtol(TSYS::strSepParse(opt->text(),1,'.').c_str(), NULL, 0);
	    p_nm = TSYS::strSepParse(func()->io(lnk(lnk_id).ioId)->def(),0,'|');

	    for(int i_l = 0; i_l < lnkSize(); i_l++) {
		cp_nm = TSYS::strSepParse(func()->io(lnk(i_l).ioId)->def(), 0, '|');
		sscanf(TSYS::strSepParse(func()->io(lnk(i_l).ioId)->def(),1,'|').c_str(), "%i.%d", &coff, &cbit);
		if(p_nm == cp_nm) {
		    lnk(i_l).dbAddr = sdb + "." + i2s(off+coff);
		    if(ioType(lnk(i_l).ioId) == IO::Boolean) lnk(i_l).dbAddr = lnk(i_l).dbAddr + "." + i2s(cbit);
		    modif();
		}
	    }
	    initLnks();
	}
    }
    else if(a_path.compare(0,12,"/cfg/prm/el_") == 0 && enableStat()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    int i_io = s2i(a_path.substr(12));
	    if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)		opt->setText(lnk(lnkId(i_io)).dbAddr);
	    else if(func()->io(i_io)->flg()&TPrmTempl::CfgConst)	opt->setText(getS(i_io));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    int i_io = s2i(a_path.substr(12));
	    if(func()->io(i_io)->flg()&TPrmTempl::CfgLink) {
		lnk(lnkId(i_io)).dbAddr = opt->text();
		initLnks();
	    }
	    else if(func()->io(i_io)->flg()&TPrmTempl::CfgConst) setS(i_io, opt->text());
	    modif();
	}
    }
    else TParamContr::cntrCmdProc(opt);
}

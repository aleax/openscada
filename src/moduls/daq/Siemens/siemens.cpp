
//OpenSCADA system module DAQ.Siemens file: siemens.cpp
/***************************************************************************
 *   Copyright (C) 2006-2010 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
#include <ttiparam.h>
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
#define MOD_VER		"1.4.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow data source Siemens PLC by CP of Hilscher CIF cards use MPI protocol and library Libnodave for other.")
#define LICENSE		"GPL2"
//************************************************

Siemens::TTpContr *Siemens::mod;	//Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new Siemens::TTpContr( source );
	return NULL;
    }
}

using namespace Siemens;

//************************************************
//* TTpContr                                     *
//************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID), drv_CIF_OK(false)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTpContr::~TTpContr( )
{
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
	if(cif_devs[i_b].present) DevExitBoard(i_b);
    if(drvCIFOK())	DevCloseDriver();
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's DB structure
    fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","0","0;10000"));	//!!!! Remove at further
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Request task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("ASINC_WR",_("Asynchronous write mode"),TFld::Boolean,TFld::NoFlag,"1","0"));
    fldAdd(new TFld("TYPE",_("Connection type"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d;%d",TMdContr::CIF_PB,TMdContr::ISO_TCP,TMdContr::ISO_TCP243,TMdContr::ADS).c_str(),
	"CIF_PB;ISO_TCP;ISO_TCP243;ADS"));
    fldAdd(new TFld("ADDR",_("Remote controller address"),TFld::String,TFld::NoFlag,"100","10"));
    fldAdd(new TFld("ADDR_TR",_("Output transport"),TFld::String,TFld::NoFlag,"40"));
    fldAdd(new TFld("SLOT",_("Slot CPU"),TFld::Integer,TFld::NoFlag,"2","2","0;30"));
    fldAdd(new TFld("CIF_DEV",_("CIF board"),TFld::Integer,TFld::NoFlag,"1","0","0;3"));
    //>> Parameter type DB structure
    int t_prm = tpParmAdd("logic","PRM_BD",_("Logical"));
    tpPrmAt(t_prm).fldAdd( new TFld("TMPL",_("Parameter template"),TFld::String,TCfg::NoVal,"50","") );
    //>> Parameter template IO DB structure
    el_prm_io.fldAdd(new TFld("PRM_ID",_("Parameter ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    el_prm_io.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    el_prm_io.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::NoFlag,"200"));

    //> CIF devices DB structure
    el_cif_dev.fldAdd( new TFld("ID",_("ID"),TFld::Integer,TCfg::Key,"1") );
    el_cif_dev.fldAdd( new TFld("ADDR",_("Address"),TFld::Integer,TFld::NoFlag,"3","5") );
    el_cif_dev.fldAdd( new TFld("SPEED",_("Speed"),TFld::Integer,TFld::NoFlag,"1","7") );

    //> Clear CIF devices info
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
    {
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
    //> Load parameters from command line

    //> Load CIF devices configuration
    TConfig cfg(&CIFDevE());
    string bd_tbl = modId()+"_CIFdevs";
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
    {
	cfg.cfg("ID").setI(i_b);
	if(SYS->db().at().dataGet(SYS->workDB()+"."+bd_tbl,mod->nodePath()+bd_tbl,cfg))
	{
	    cif_devs[i_b].pbaddr = cfg.cfg("ADDR").getI();
	    cif_devs[i_b].pbspeed = cfg.cfg("SPEED").getI();
	}
	if(drvCIFOK())	initCIF(i_b);
    }
}

void TTpContr::save_()
{
    //> Save CIF devices configuration
    TConfig cfg(&CIFDevE());
    string bd_tbl = modId()+"_CIFdevs";
    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
    {
	cfg.cfg("ID").setI(i_b);
	cfg.cfg("ADDR").setI(cif_devs[i_b].pbaddr);
	cfg.cfg("SPEED").setI(cif_devs[i_b].pbspeed);
	SYS->db().at().dataSet(SYS->workDB()+"."+bd_tbl,mod->nodePath()+bd_tbl,cfg);
    }
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

bool TTpContr::drvCIFOK()
{
    if(drv_CIF_OK) return drv_CIF_OK;
    drv_CIF_OK = (DevOpenDriver()==DRV_NO_ERROR);

    //> Load CIF boards configuration
    if(drv_CIF_OK)
    {
	BOARD_INFO brd_info;
	memset(&brd_info,0,sizeof(BOARD_INFO));
	if(DevGetBoardInfo(&brd_info) == DRV_NO_ERROR)
	    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
	    {
		cif_devs[i_b].present = (brd_info.tBoard[i_b].usAvailable==1);
		if(!cif_devs[i_b].present) continue;
		cif_devs[i_b].board = brd_info.tBoard[i_b].usBoard;
		cif_devs[i_b].phAddr = brd_info.tBoard[i_b].ulPhysicalAddress;
		cif_devs[i_b].irq = brd_info.tBoard[i_b].usIrq;
		unsigned char fwInfo[300];
		if(DevGetInfo(i_b,GET_FIRMWARE_INFO,sizeof(FIRMWAREINFO),&fwInfo[0]) == DRV_NO_ERROR)
		{
		    cif_devs[i_b].fwname.assign((char *)&fwInfo[0],16);
		    cif_devs[i_b].fwver.assign((char *)&fwInfo[16],16);
		}
		DevInitBoard(i_b);
	    }
    }
    return drv_CIF_OK;
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

    struct Spar_bus
    {
	unsigned char	bMax_Retry_Limit;
	unsigned char	bTQUI;
	unsigned short	usTSL;
	unsigned char	bTSET;
	unsigned short	usMin_TSDR;
	unsigned short	usMax_TSDR;
	unsigned long	ulTTR;
	unsigned char	bG;
    };

    struct Spar_bus par_bus[] =
    {
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


    if( dev < 0 || dev > MAX_DEV_BOARDS || !drvCIFOK() ) 
	throw TError( nodePath().c_str(), _("CIF device %d or device driver error!"), dev);
    if(!cif_devs[dev].present)	return;

    ResAlloc resource(cif_devs[dev].res,true);

    // Load parameters to board
    //> Running board aplications
    if( (sRet = DevSetHostState( dev, HOST_READY, 0L )) != DRV_NO_ERROR )
	throw TError( nodePath().c_str(), _("CIF device %d running (DevSetHostState(HOST_READY)) error!"), dev);
    //> Load the protocol task parameters
    memset(&DPParameter,0,sizeof(DPM_PLC_PARAMETER));
    DPParameter.bMode=DPM_SET_MODE_UNCONTROLLED;
    DPParameter.bFormat=1;
    DPParameter.usWatchDogTime=1000;
    if( (sRet = DevPutTaskParameter(dev, 1, sizeof(DPM_PLC_PARAMETER), &DPParameter )) != DRV_NO_ERROR )
	throw TError( nodePath().c_str(), _("CIF device %d. DevPutTaskParameter() error!"), dev);
    //>> Reset CP for apply parameters
    DevReset(dev, WARMSTART, 5000L);
    //> Download the bus parameter
    //>> Delete The static data base if present
    DevGetInfo(dev, GET_DRIVER_INFO, sizeof(DRIVERINFO), &tDriverInfo);
    if(tDriverInfo.bHostFlags&0x40)
    {
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
	if( (sRet=DevPutMessage(dev, (MSG_STRUC *)&tMsg, 200L)) != DRV_NO_ERROR )
	    throw TError( nodePath().c_str(), _("CIF device %d. DevPutMessage() error!"), dev);
	if( (sRet=DevGetMessage(dev,sizeof(tMsg),(MSG_STRUC *)&tMsg,200L)) != DRV_NO_ERROR )
	    throw TError( nodePath().c_str(), _("CIF device %d. DevGetMessage() error!"), dev);
    }
    //>> Load new bus parameters
    tMsg.rx = 3;			// task = DPM-Task
    tMsg.tx = 16;
    tMsg.ln = sizeof( DDLM_DOWNLOAD_REQUEST );
    tMsg.nr = 0;
    tMsg.a  = 0;
    tMsg.f  = 0;
    tMsg.b  = DDLM_Download;
    tMsg.e  = 0;
    ptDownloadRequest=(DDLM_DOWNLOAD_REQUEST*) &tMsg.d;
    ptDownloadRequest->bReq_Add = 0;
    ptDownloadRequest->bArea_Code = DPM_DEVICE_PRM;
    ptDownloadRequest->usAdd_Offset = 0;
    ptBusDpm = ( DPM_BUS_DP* )&ptDownloadRequest->abData;
    ptBusDpm->usBus_Para_Len = sizeof( DPM_BUS_DP );
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
	throw TError( nodePath().c_str(), _("Sending request error: %d."),sRet);
    if((sRet = DevGetMessage(dev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg, 200L))!=DRV_NO_ERROR)
	throw TError( nodePath().c_str(), _("Getting request error: %d."),sRet);
    //>> Wait for operation start with new parameters of PLC task
    do
    {
	DevGetTaskState(dev, 2, sizeof(tTaskState), &tTaskState);
	TSYS::sysSleep(1e-3);
    }
    while(tTaskState.bDPM_state!=OPERATE);
}

void TTpContr::getLifeListPB( unsigned board, string &buffer )
{
    RCS_MESSAGE tMsg;
    RCS_TELEGRAMHEADER_10  *ptRcsTelegramheader10;
    int res;

    if( !cif_devs[board].present )
	throw TError(nodePath().c_str(),_("15:Board %d is not present."),board); 

    ResAlloc resource(cif_devs[board].res,true);

    //> Prepare request
    tMsg.rx = 7;  // task = FDL-Task
    tMsg.tx = 16;
    tMsg.ln = sizeof( RCS_TELEGRAMHEADER_10 );
    tMsg.nr = 0;
    tMsg.a  = 0;
    tMsg.f  = 0;
    tMsg.b  = DDLM_Life_List;
    tMsg.e  = 0;
    ptRcsTelegramheader10=(RCS_TELEGRAMHEADER_10*)&tMsg.d;
    ptRcsTelegramheader10->device_adr = 0;
    ptRcsTelegramheader10->data_area  = 0;
    ptRcsTelegramheader10->data_adr   = 0;
    ptRcsTelegramheader10->data_idx   = 0;
    ptRcsTelegramheader10->data_cnt   = DPM_MAX_NUM_DEVICES;
    ptRcsTelegramheader10->data_idx   = 0;
    ptRcsTelegramheader10->data_type  = TASK_TDT_STRING;
    ptRcsTelegramheader10->function   = TASK_TFC_READ;

    //> Put message
    if((res = DevPutMessage(board,(MSG_STRUC *)&tMsg, 500L))!=DRV_NO_ERROR)
	throw TError(nodePath().c_str(),_("12:Sending request error %d."),res);
    if((res = DevGetMessage(board,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg, 200L))!=DRV_NO_ERROR)
	throw TError(nodePath().c_str(),_("13:Getting request error %d."),res);

    buffer.assign((char*)&tMsg.d[sizeof(RCS_TELEGRAMHEADER_10)],DPM_MAX_NUM_DEVICES+1);
}

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTipDAQ::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/mod",_("CIF")))
	{
	    if(ctrMkNode("area",opt,-1,"/mod/st",_("Status")))
		ctrMkNode("fld",opt,-1,"/mod/st/drv",_("CIF driver"),R_R_R_,"root",SDAQ_ID,1,"tp","bool");
	    if(ctrMkNode("table",opt,-1,"/mod/dev",_("CIF devices"),RWRWR_,"root",SDAQ_ID))
	    {
		ctrMkNode("list",opt,-1,"/mod/dev/brd",_("Board"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/fwnm",_("Firmware name"),R_R_R_,"root",SDAQ_ID,1,"tp","real");
		ctrMkNode("list",opt,-1,"/mod/dev/fwver",_("Firmware version"),R_R_R_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/phAddr",_("Physical address"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/irq",_("IRQ"),R_R_R_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/addr",_("PB address"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/speed",_("PB speed"),RWRWR_,"root",SDAQ_ID,4,"tp","dec","idm","1","dest","select","select","/mod/dev/lsspd");
	    }
	}
	if(ctrMkNode("area",opt,1,"/PB",_("Profibus")))
	{
	    ctrMkNode("fld",opt,-1,"/PB/dev",_("CIF device"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
	    ctrMkNode("list",opt,-1,"/PB/lifels",_("Life stations list"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/mod/st/drv" && ctrChkNode(opt))	opt->setText(TSYS::int2str(drvCIFOK()));
    else if(a_path == "/mod/dev")
    {
	if(ctrChkNode(opt))
	{
	    //> Fill Archives table
	    XMLNode *n_brd  = ctrMkNode("list",opt,-1,"/mod/dev/brd","");
	    XMLNode *n_fwnm = ctrMkNode("list",opt,-1,"/mod/dev/fwnm","");
	    XMLNode *n_fwver= ctrMkNode("list",opt,-1,"/mod/dev/fwver","");
	    XMLNode *n_phAddr = ctrMkNode("list",opt,-1,"/mod/dev/phAddr","");
	    XMLNode *n_irq  = ctrMkNode("list",opt,-1,"/mod/dev/irq","");
	    XMLNode *n_pba  = ctrMkNode("list",opt,-1,"/mod/dev/addr","");
	    XMLNode *n_pbspd= ctrMkNode("list",opt,-1,"/mod/dev/speed","");

	    for(int i_b = 0; i_b < MAX_DEV_BOARDS; i_b++)
	    {
		if(n_brd)	n_brd->childAdd("el")->setText(TSYS::int2str(cif_devs[i_b].board));
		if(n_fwnm)	n_fwnm->childAdd("el")->setText(cif_devs[i_b].fwname);
		if(n_fwver)	n_fwver->childAdd("el")->setText(cif_devs[i_b].fwver);
		if(n_phAddr)	n_phAddr->childAdd("el")->setText(TSYS::int2str(cif_devs[i_b].phAddr,TSYS::Hex));
		if(n_irq)	n_irq->childAdd("el")->setText(TSYS::int2str(cif_devs[i_b].irq));
		if(n_pba)	n_pba->childAdd("el")->setText(TSYS::int2str(cif_devs[i_b].pbaddr));
		if(n_pbspd)	n_pbspd->childAdd("el")->setText(TSYS::int2str(cif_devs[i_b].pbspeed));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    int brd = atoi(opt->attr("row").c_str());
	    string col  = opt->attr("col");
	    if(brd < 0 || brd > 3)	throw TError(nodePath().c_str(),_("Board number %d error!"),brd);
	    if(col == "addr")
	    {
		int addr = atoi(opt->text().c_str());
		cif_devs[brd].pbaddr = (addr<0)?0:(addr>126)?126:addr;
	    }
	    else if(col == "speed")	cif_devs[brd].pbspeed = atoi(opt->text().c_str());
	    if(col == "addr" || col == "speed") initCIF(brd);
	    modif();
	}
    }
    else if(a_path == "/mod/dev/lsspd" && ctrChkNode(opt))
    {
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
    else if(a_path == "/PB/dev")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"lifeLsDev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"lifeLsDev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/PB/lifels" && ctrChkNode(opt))
    {
	int board = atoi(TBDS::genDBGet(mod->nodePath()+"lifeLsDev","0",opt->attr("user")).c_str());
	string lifeLst;
	try
	{
	    getLifeListPB(board, lifeLst);
	    for(unsigned i_st = 0; i_st < lifeLst.size(); i_st++)
		switch((unsigned char)lifeLst[i_st])
		{
		    case 0xFF:	opt->childAdd("el")->setText(TSYS::int2str(i_st)+_(" : -------"));	break;
		    case 0x30:	opt->childAdd("el")->setText(TSYS::int2str(i_st)+_(" : Active station"));	break;
		    case 0x00:	opt->childAdd("el")->setText(TSYS::int2str(i_st)+_(" : Passive station"));	break;
		}
	}
	catch(TError err) { opt->childAdd("el")->setText(err.mess); }
    }
    else TTipDAQ::cntrCmdProc(opt);
}

//************************************************
//* TMdContr                                     *
//************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem),
	mPerOld(cfg("PERIOD").getId()), mPrior(cfg("PRIOR").getId()), mType(cfg("TYPE").getId()),
	mSlot(cfg("SLOT").getId()), mDev(cfg("CIF_DEV").getId()), mAssincWR(cfg("ASINC_WR").getBd()),
	prc_st(false), call_st(false), endrun_req(false), isReload(false), di(NULL), dc(NULL),
	mPer(1e9), tm_calc(0)
{
    cfg("PRM_BD").setS("SiemensPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(run_st) stop();
}

void TMdContr::postDisable( int flag )
{
    TController::postDisable(flag);
    try
    {
	if(flag)
	{
	    //> Delete parameter's io table
	    string tbl = DB()+"."+cfg("PRM_BD").getS()+"_io";
	    SYS->db().at().open(tbl);
	    SYS->db().at().close(tbl,true);
	}
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if(startStat() && !redntUse())
    {
	if(atoi(errCon.getVal().c_str()))
	{
	    rez += TSYS::strParse(errCon.getVal(),1,":");
	    rez.replace(0,1,TSYS::strParse(errCon,0,":"));
	}
	else
	{
	    if(call_st)	rez += TSYS::strMess(_("Call now. "));
	    if(period())rez += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-3*period()).c_str());
	    else rez += TSYS::strMess(_("Call next by cron '%s'. "),tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s. "),tm2s(tm_calc).c_str());
	}
    }
    return rez;
}

TTpContr &TMdContr::owner( )	{ return *(TTpContr*)nodePrev(); }

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::load_( )
{
    TController::load_( );

    //> Check for get old period method value
    if(mPerOld) { cfg("SCHEDULE").setS(TSYS::real2str(mPerOld/1e3)); mPerOld = 0; }
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
    //> Clear acquisition data blocks and asynchronous write mode data blocks
    acqBlks.clear();
    writeBlks.clear();
}

void TMdContr::start_( )
{
    if(prc_st) return;

    try{ connectRemotePLC(); }
    catch(TError err) { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }

    //> Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,1e9*atof(cron().c_str())) : 0;

    //> Clear acquisition data blocks and asynchronous write mode data blocks
    acqBlks.clear();
    writeBlks.clear();

    //>> Reenable parameters
    try
    {
        vector<string> pls;
        list(pls);

        isReload = true;
        for(unsigned i_p = 0; i_p < pls.size(); i_p++)
            if(at(pls[i_p]).at().enableStat()) at(pls[i_p]).at().enable();
        isReload = false;
    } catch(TError) { isReload = false; throw; }

    //> Start the request data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrun_req);

    //> Clear proccess parameters list
    pHd.clear();

    disconnectRemotePLC( );
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if(icfg.fld().name() == "TYPE")
    {
	cfg("CIF_DEV").setView(icfg.getI() == CIF_PB);
	cfg("SLOT").setView(icfg.getI() == CIF_PB || icfg.getI() == ISO_TCP || icfg.getI() == TMdContr::ISO_TCP243);
	cfg("ADDR_TR").setView(icfg.getI() == ADS);
	if(startStat())	stop();
    }

    return true;
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res(nodeRes(),true);

    unsigned i_prm;
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
	if(pHd[i_prm].at().id() == id) break;

    if(val && i_prm >= pHd.size()) pHd.push_back(at(id));
    if(!val && i_prm < pHd.size()) pHd.erase(pHd.begin()+i_prm);
}

void TMdContr::regVal( SValData ival, IO::Type itp, bool wr )
{
    if(ival.db < 0 || ival.off < 0)	return;

    int iv_sz = valSize(itp,ival.sz); //Get real value's size

    ResAlloc res(nodeRes(),true);

    //> Register to acquisition block
    unsigned i_b;
    bool plcOK = false;
    for(i_b = 0; i_b < acqBlks.size(); i_b++)
	if(ival.db < acqBlks[i_b].db) break;
	else if(acqBlks[i_b].db == ival.db)
	{
	    if(ival.off < acqBlks[i_b].off)
	    {
		if((acqBlks[i_b].val.size()+acqBlks[i_b].off-ival.off) < MaxLenReq)
		{
		    acqBlks[i_b].val.insert(0,acqBlks[i_b].off-ival.off,0);
		    acqBlks[i_b].off = ival.off;
		}
		else acqBlks.insert(acqBlks.begin()+i_b,SDataRec(ival.db,ival.off,iv_sz));
	    }
	    else if((ival.off+iv_sz) > (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    {
		if((ival.off+iv_sz-acqBlks[i_b].off) < MaxLenReq)
		    acqBlks[i_b].val.append((ival.off+iv_sz)-(acqBlks[i_b].off+acqBlks[i_b].val.size()),0);
		else continue;
	    }
	    plcOK = true;
	    break;
	}
    if(!plcOK)	acqBlks.insert(acqBlks.begin()+i_b,SDataRec(ival.db,ival.off,iv_sz));

    //> Register to asynchronous write block
    if(wr && assincWrite())
    {
	plcOK = false;
	for(i_b = 0; i_b < writeBlks.size(); i_b++)
	    if(ival.db < writeBlks[i_b].db) break;
	    else if(writeBlks[i_b].db == ival.db)
	    {
		if(ival.off < writeBlks[i_b].off)
		{
		    if((ival.off+iv_sz) >= writeBlks[i_b].off &&
			    (writeBlks[i_b].val.size()+writeBlks[i_b].off-ival.off) < MaxLenReq)
		    {
			writeBlks[i_b].val.insert(0,writeBlks[i_b].off-ival.off,0);
			writeBlks[i_b].off = ival.off;
		    }
		    else writeBlks.insert(writeBlks.begin()+i_b,SDataRec(ival.db,ival.off,iv_sz));
		}
		else if((ival.off+iv_sz) > (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    if(ival.off <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()) &&
			    (ival.off+iv_sz-writeBlks[i_b].off) < MaxLenReq)
		    {
			writeBlks[i_b].val.append((ival.off+iv_sz)-(writeBlks[i_b].off+writeBlks[i_b].val.size()),0);
			//> Check for allow mergin to next block
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

void TMdContr::connectRemotePLC( )
{
    switch(mType)
    {
	case CIF_PB:
	    if(!(owner().cif_devs[0].present||owner().cif_devs[1].present||owner().cif_devs[2].present||owner().cif_devs[3].present))
		throw TError(nodePath().c_str(),_("No one driver or board are present."));
	    break;
	case ISO_TCP:
	case ISO_TCP243:
	{
	    //> Dsconnect previous connection
	    if(dc && di) disconnectRemotePLC();

	    //> Full Libnodave API
	    ResAlloc res(reqRes, true);
	    _daveOSserialType fds;
	    fds.wfd = fds.rfd = openSocket(102, addr().c_str());
	    if(fds.rfd <= 0) throw TError(nodePath().c_str(),_("Open socket of remote PLC error."));
	    di = daveNewInterface(fds,(char*)(string("IF")+id()).c_str(),0,
		((mType==ISO_TCP243)?daveProtoISOTCP243:daveProtoISOTCP),daveSpeed187k);
	    dc = daveNewConnection(di,2,0,mSlot);
	    daveSetTimeout(di,1500000);
	    if(daveConnectPLC(dc))
	    {
		errCon = _("10:Connection error.");
		close(fds.wfd);
		delete dc;
		delete di;
		dc = NULL;
		di = NULL;
		throw TError(nodePath().c_str(),_("Connection to PLC error."));
	    }

	    //- Self OpenSCADA API -
	    //- Output transport open -
	    /*if( !SYS->transport().at().at("Sockets").at().outPresent(mod->modId()+id()) )
		SYS->transport().at().at("Sockets").at().outAdd(mod->modId()+id());
	    AutoHD<TTransportOut> tr = SYS->transport().at().at("Sockets").at().outAt(mod->modId()+id());
	    string trAddr = "TCP:"+TSYS::strSepParse(addr(),0,':')+":"+
			        (TSYS::strSepParse(addr(),1,':').empty() ? "102" : TSYS::strSepParse(addr(),1,':'));
	    tr.at().setAddr(trAddr);
	    tr.at().start();

	    //- Connect to remote PLC on ISO-TSAP protocol -
	    string pdu, mbap;
	    pdu = pdu + (char)0x11 + (char)0xE0 + (char)0x00 +
			(char)0x00 + (char)0x00 + (char)0x01 + (char)0x00 +
			(char)0xC1 + (char)0x02 + (char)0x01 + (char)0x00 +
			(char)0xC2 + (char)0x02 + (char)0x01 + (char)mSlot +
			(char)0xC0 + (char)0x01 + (char)0x09;

	    mbap = mbap + (char)0x03 + (char)0x00 +
			  (char)((pdu.size()+4)>>8) + (char)(pdu.size()+4) +
			  pdu;

	    unsigned char buf[1000];
	    int resp_len = tr.at().messIO( mbap.data(), mbap.size(), (char*)buf, sizeof(buf), 10 );
	    if( resp_len < 4 )	throw TError(nodePath().c_str(),_("PLC respond is zero or too short."));
	    int lenPDU = (buf[2]<<8)+buf[3];
	    if( lenPDU != 22 )	throw TError(nodePath().c_str(),_("Connection respond is broken."));

	    //- Neg PDU length request -
	    //_daveNegPDUlengthRequest(
	    mbap = pdu = "";
	    pdu = pdu+(char)0x02+(char)0xf0+(char)0x80;
	    //-- Start real PDU header --
	    pdu = pdu + (char)0x32 +			//P
			(char)0x01 +			//Type
			(char)0x00 +			//a
			(char)0x00 +			//b
			(char)0xFF + (char)0xFF +	//number
			(char)0x00 + (char)0x08 +	//plen
			(char)0x00 + (char)0x00;  	//dlen
	    //-- Parameters --
	    pdu = pdu + (char)0xF0 + (char)0x00 + (char)0x00 + (char)0x01 + (char)0x00 + (char)0x01 + (char)0x03 + (char)0xC0;

	    mbap = mbap+(char)0x03 + (char)0x00 +
			(char)((pdu.size()+4)>>8) + (char)(pdu.size()+4) +
			pdu;

	    resp_len = tr.at().messIO( mbap.data(), mbap.size(), (char*)buf, sizeof(buf), 10 );*/
	    //daveSetDebug(daveDebugAll);

	    break;
	}
	case ADS:
	{
	    //> Establish connection
	    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(addrTr(),0,'.')).at().outAt(TSYS::strSepParse(addrTr(),1,'.'));
	    try { tr.at().start(); }
	    catch(TError err) { errCon = _("10:Connection error."); mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

	    break;
	}
	default: throw TError(nodePath().c_str(),_("Connection type '%d' is not supported."),mType);
    }
}

void TMdContr::disconnectRemotePLC( )
{
    switch(mType)
    {
        case ISO_TCP:
        case ISO_TCP243:
	    ResAlloc res(reqRes, true);
	    if(!dc || !di) break;
	    daveDisconnectPLC(dc);
	    close(di->fd.rfd);
	    delete dc;
	    delete di;
	    dc = NULL;
	    di = NULL;
	    break;
    }
}

void TMdContr::getDB( unsigned n_db, long offset, string &buffer )
{
    switch(mType)
    {
        case CIF_PB:
	{
	    RCS_MESSAGE tMsg;
	    int res, e_try = 4;

	    if(buffer.size() > 240)		throw TError(nodePath().c_str(),_("14:Request block is too big."));
	    if(!owner().cif_devs[mDev].present)	throw TError(nodePath().c_str(),_("15:Board %d is not present."),mDev);

	    ResAlloc resource(owner().cif_devs[mDev].res,true);

	    do
	    {
		e_try--;
		//> Clear all previous data
		while(!DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,0L))	TSYS::sysSleep(1e-3);

		//> Prepare put request
		tMsg.rx = 3;
		tMsg.tx = 16;
		tMsg.ln = 8;
		tMsg.nr = 0;
		tMsg.a  = 0;
		tMsg.f  = 0;
		tMsg.b  = MPI_Read_Write_DB;
		tMsg.e  = 0;
		tMsg.d[0] = vmax(0,vmin(126,atoi(addr().c_str())));
		tMsg.d[1] = (unsigned char)(offset>>8);
		tMsg.d[2] = (unsigned char)n_db;
		tMsg.d[3] = (unsigned char)(n_db>>8);
		tMsg.d[4] = (unsigned char)offset;
		tMsg.d[5] = (unsigned char)buffer.size();
		tMsg.d[6] = TASK_TDT_UINT8;
		tMsg.d[7] = TASK_TFC_READ;

		//> Put message to remote host
		res = DevPutMessage(mDev,(MSG_STRUC *)&tMsg, 200L);
		if(res == DRV_DEV_PUT_TIMEOUT) throw TError(nodePath().c_str(),_("12:Sending request is timeouted."));
		//> Get remote host's response
		if(res == DRV_NO_ERROR) res = DevGetMessage(mDev, sizeof(RCS_MESSAGE), (MSG_STRUC *)&tMsg, 200L);
	    }
	    while((res == DRV_NO_ERROR) && (tMsg.f == 0x02 || tMsg.f == 0x39) && e_try > 0);

	    //> Process errors
	    if(res != DRV_NO_ERROR)		throw TError(nodePath().c_str(),_("19:Request to DB error %d."),res);
	    if(res == DRV_DEV_GET_TIMEOUT)	throw TError(nodePath().c_str(),_("13:Get request is timeouted."));
	    if(tMsg.f == 17)	throw TError(nodePath().c_str(),_("17:There is no response from the remote station."));
	    if(tMsg.f == 18)	throw TError(nodePath().c_str(),_("18:Master is out of the logical token ring."));
	    if(tMsg.f == 0x85)	throw TError(nodePath().c_str(),_("20:Specified offset address or DB error."));

	    //printf("Get DB %d:%d DB%d.%d(%d) -- %d\n",mDev,vmax(0,vmin(126,atoi(addr().c_str()))),n_db,offset,buffer.size(),tMsg.f);

	    //> Close connection
	    /*tMsg.rx = 3;
	    tMsg.tx = 16;
	    tMsg.ln = 0;
	    tMsg.nr = 0;
	    tMsg.a  = 0;
	    tMsg.f  = 0;
	    tMsg.b  = MPI_Disconnect;
	    tMsg.e  = 0;
	    if( (res=DevPutMessage(mDev,(MSG_STRUC *)&tMsg, 200L)) != DRV_NO_ERROR )
		throw TError(nodePath().c_str(),_("12:Put request is error %d."),res);
	    if( (res=DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,200L)) != DRV_NO_ERROR )
		throw TError(nodePath().c_str(),_("12:Get request is error %d."),res);*/

	    //> Put result
	    buffer.replace(0,buffer.size(),(char *)tMsg.d+8,buffer.size());
	    break;
	}
	case ISO_TCP:
	case ISO_TCP243:
	{
	    //> Reconnect for lost connection
	    if(!dc || atoi(errCon.getVal().c_str())) { connectRemotePLC(); errCon = ""; }
	    //> Full libnodave API
	    int rez;
	    ResAlloc res(reqRes, true);
	    if((rez = daveReadBytes(dc,daveDB,n_db,offset,buffer.size(),NULL)))
	    {
		if(rez == daveResTimeout) errCon = _("10:Connection error.");
		mess_debug(nodePath().c_str(),_("Read block '%d' error: %s"),n_db,daveStrerror(rez));
		throw TError(nodePath().c_str(),_("12:Read DB '%d' from controller error: %s"),n_db,daveStrerror(rez));
	    }
	    buffer.assign((char*)dc->resultPointer,buffer.size());

	    //> Self OpenSCADA API
	    /*string pdu, mbap;
	    pdu = pdu+(char)0x02+(char)0xf0+(char)0x80;
	    //> Start real PDU header
	    pdu = pdu + (char)0x32 +			//P
			(char)0x01 +			//Type
			(char)0x00 +			//a
			(char)0x00 +			//b
			(char)0x00 + (char)0x01 +	//number
			(char)0x00 + (char)0x0e +	//plen
			(char)0x00 + (char)0x00;  	//dlen
	    //> Parameters
	    pdu = pdu + (char)0x04 +						// FuncRead
			(char)0x01 +						// Parameters counter
			(char)0x12 + (char)0x0A + (char)0x10 +
			(char)0x02 +						// 1=single bit, 2=byte, 4=word
	    		(char)(buffer.size()>>8) + (char)buffer.size() +	// length in bytes
		        (char)(n_db>>8) + (char)n_db +				// DB number
			(char)0x84 +						// DB area
			(char)((offset*8)>>16) + (char)((offset*8)>>8) + (char)(offset*8);	// start address in bits

	    mbap = mbap+(char)0x03 + (char)0x00 +
			(char)((pdu.size()+4)>>8) + (char)(pdu.size()+4) +
			pdu;

	    AutoHD<TTransportOut> tr;
	    try{ tr = SYS->transport().at().at("Sockets").at().outAt(mod->modId()+id()); }
	    catch(...) { tr.at().stop(); throw; }

	    if( !tr.at().startStat() ){ stop(); start(); }

	    unsigned char buf[1000];
	    int resp_len = tr.at().messIO( mbap.data(), mbap.size(), (char*)buf, sizeof(buf), 10 );
	    if( resp_len < 16 )	throw TError(nodePath().c_str(),_("PLC respond is zero or too short."));

	    int pheader = 7;
	    int phlen = 10;
	    if( buf[pheader+1] == 2 || buf[pheader+1] == 3 ) phlen = 12;
	    int pparam = pheader+phlen;	//19
	    int plen  = (buf[pheader+6]<<8) + buf[pheader+7];	//2
	    int pdata = pparam+plen;	//21
	    int pdlen = (buf[pheader+8]<<8) + buf[pheader+9];	//6
	    buffer.replace(0,buffer.size(),(char*)buf+pdata,buffer.size());*/

	    break;
	}
	case ADS:
	{
	    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(addrTr(),0,'.')).at().outAt(TSYS::strSepParse(addrTr(),1,'.'));
	    ResAlloc resN(tr.at().nodeRes(), true);
	    try { if(!tr.at().startStat()) tr.at().start(); }
	    catch(TError err) { errCon = _("10:Connection error."); throw; }

	    char buf[1000], res[1000];

	    AMS_TCP_HEAD *AmsTcpHD = (AMS_TCP_HEAD *)buf;
	    AmsTcpHD->reserv = 0;
	    AmsTcpHD->len = sizeof(AMS_HEAD) + sizeof(ADS_ReadWriteReq);

	    AMS_HEAD *AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
	    string tAddr = TSYS::strParse(addr(),0,"|");
	    for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_targ[i_d] = atoi(TSYS::strParse(tAddr,0,".",&i_off).c_str());
	    AmsHD->AMSPort_targ = atoi(TSYS::strParse(tAddr,1,":").c_str());
	    tAddr = TSYS::strParse(addr(),1,"|");
	    for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_src[i_d] = atoi(TSYS::strParse(tAddr,0,".",&i_off).c_str());
	    AmsHD->AMSPort_src = atoi(TSYS::strParse(tAddr,1,":").c_str());
	    AmsHD->com = 2;		//ADS Read
	    AmsHD->stFlgs = 0x4;	//ADS command
	    AmsHD->len = sizeof(ADS_ReadWriteReq);
	    AmsHD->errCod = 0;
	    AmsHD->InvId = 0;

	    ADS_ReadWriteReq *ADSread = (ADS_ReadWriteReq *)(AmsHD+1);
	    ADSread->IdGrp = n_db;
	    ADSread->IdOff = offset;
	    ADSread->len = buffer.size();

	    //> Request
	    int resp_len;
	    try { resp_len = tr.at().messIO(buf, AmsTcpHD->len+sizeof(AMS_TCP_HEAD), res, sizeof(res), 0, true); }
	    catch(TError err) { errCon = _("10:Connection error."); throw; }
	    int full_len = resp_len;
	    if(full_len < (int)sizeof(AMS_TCP_HEAD))  throw TError(nodePath().c_str(),_("13:Error server respond"));
	    AmsTcpHD = (AMS_TCP_HEAD *)res;
	    unsigned resp_sz = AmsHD->len;

	    //> Wait tail
	    while(full_len < (int)(resp_sz+sizeof(AMS_TCP_HEAD)))
	    {
		resp_len = tr.at().messIO(NULL, 0, res+full_len, sizeof(res)-full_len, 0, true);
		if(!resp_len) throw TError(nodePath().c_str(),_("13:Not full respond"));
		full_len += resp_len;
	    }

	    //> Check for correct result
	    AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
	    ADS_ReadResp *ADSreadResp = (ADS_ReadResp *)(AmsHD+1);
	    resp_len = sizeof(AMS_TCP_HEAD)+sizeof(AMS_HEAD);
	    if(full_len < resp_len || AmsHD->com != 2 || AmsHD->stFlgs != 0x05)
		throw TError(nodePath().c_str(),_("13:Error server respond"));
	    if(AmsHD->errCod) throw TError(nodePath().c_str(),_("13:Error server respond: %d"),AmsHD->errCod);
	    if(full_len < (resp_len+=sizeof(ADS_ReadResp)))	throw TError(nodePath().c_str(),_("13:Error server '%s' respond"),"Read");
	    if(ADSreadResp->res) throw TError(nodePath().c_str(),_("13:Error server '%s' respond: %d"),"Read",ADSreadResp->res);
	    if(ADSreadResp->len != buffer.size() || full_len < (int)(resp_len+ADSreadResp->len))
		throw TError(nodePath().c_str(),_("13:Error server '%s' respond"),"Read");
	    buffer.assign(res+resp_len,buffer.size());

	    errCon = "";

	    break;
	}
    }
}

void TMdContr::putDB( unsigned n_db, long offset, const string &buffer )
{
    switch(mType)
    {
	case CIF_PB:
	{
	    RCS_MESSAGE tMsg;
	    int res, e_try = 4;

	    if(buffer.size() > 240)		throw TError(nodePath().c_str(),_("16:Transmitted block is too big."));
	    if(!owner().cif_devs[mDev].present)	throw TError(nodePath().c_str(),_("15:Board %d is not present."),mDev);

	    ResAlloc resource(owner().cif_devs[mDev].res, true);

	    do
	    {
		e_try--;

		//> Clear all previous data
		while(!DevGetMessage(mDev,sizeof(RCS_MESSAGE),(MSG_STRUC *)&tMsg,0L))	TSYS::sysSleep(1e-3);

		//> Prepare put request
		tMsg.rx = 3;
		tMsg.tx = 16;
		tMsg.ln = buffer.size()+8;
		tMsg.nr = 0;
		tMsg.a  = 0;
		tMsg.f  = 0;
		tMsg.b  = MPI_Read_Write_DB;
		tMsg.e  = 0;
		tMsg.d[0] = vmax(0,vmin(126,atoi(addr().c_str())));
		tMsg.d[1] = (unsigned char)(offset>>8);
		tMsg.d[2] = (unsigned char)n_db;
		tMsg.d[3] = (unsigned char)(n_db>>8);
		tMsg.d[4] = (unsigned char)offset;
		tMsg.d[5] = (unsigned char)buffer.size();
		tMsg.d[6] = TASK_TDT_UINT8;
		tMsg.d[7] = TASK_TFC_WRITE;
		memcpy(tMsg.d+8,buffer.c_str(),buffer.size());

		//> Put message to remote host
		res = DevPutMessage(mDev, (MSG_STRUC *)&tMsg, 200L);
		if(res == DRV_DEV_PUT_TIMEOUT) throw TError(nodePath().c_str(),_("12:Sending request is timeouted."));
		//> Get remote host's response
		if(res == DRV_NO_ERROR) res = DevGetMessage(mDev, sizeof(RCS_MESSAGE), (MSG_STRUC *)&tMsg, 200L);
	    }
	    while((res == DRV_NO_ERROR) && (tMsg.f == 0x02 || tMsg.f == 0x39) && e_try > 0);

	    //printf("Put DB %d:%d DB%d.%d(%d) -- %d \n",mDev,vmax(0,vmin(126,atoi(addr().c_str()))),n_db,offset,buffer.size(),tMsg.f);

	    //> Process errors
	    if(res != DRV_NO_ERROR)		throw TError(nodePath().c_str(),_("19:Request to DB error %d."),res);
	    if(res == DRV_DEV_GET_TIMEOUT)	throw TError(nodePath().c_str(),_("13:Getting request is timeouted."));
	    if(tMsg.f == 17)	throw TError(nodePath().c_str(),_("17:There is no response from the remote station."));
	    if(tMsg.f == 18)	throw TError(nodePath().c_str(),_("18:Master is out of the logical token ring."));
	    if(tMsg.f == 0x85)	throw TError(nodePath().c_str(),_("20:Specified offset address or DB error."));
	    break;
	}
	case ISO_TCP:
	case ISO_TCP243:
	{
	    //> Reconnect for lost connection
	    if(!dc || atoi(errCon.getVal().c_str())) { connectRemotePLC(); errCon = ""; }

	    //> Full Libnodave API
	    int rez;
	    ResAlloc res(reqRes, true);
	    if((rez = daveWriteBytes(dc,daveDB,n_db,offset,buffer.size(),(char*)buffer.c_str())))
	    {
		if(rez == daveResTimeout) errCon = _("10:Connection error.");
		mess_debug(nodePath().c_str(),_("Write block '%d' error: %s"),n_db,daveStrerror(rez));
		throw TError(nodePath().c_str(),_("12:Write DB '%d' to controller error: %s"),n_db,daveStrerror(rez));
	    }

	    //> Self OpenSCADA API
	    /*string pdu, mbap;
	    pdu = pdu+(char)0x02+(char)0xf0+(char)0x80;
	    //> Start real PDU header
	    pdu = pdu + (char)0x32 +			//P
			(char)0x01 +			//Type
			(char)0x00 +			//a
			(char)0x00 +			//b
			(char)0x00 + (char)0x00 +	//number
			(char)0x00 + (char)0x0e +	//plen
			(char)((buffer.size()+4)>>8) + (char)(buffer.size()+4);	//dlen
	    //> Parameters
	    pdu = pdu + (char)0x05 +						// FuncWrite
			(char)0x01 +						// Parameters counter
			(char)0x12 + (char)0x0A + (char)0x10 +
			(char)0x02 +						// 1=single bit, 2=byte, 4=word
			(char)(buffer.size()>>8) + (char)buffer.size() +	// length in bytes
			(char)(n_db>>8) + (char)n_db +				// DB number
			(char)0x84 +						// DB area
			(char)((offset*8)>>16) + (char)((offset*8)>>8) + (char)(offset*8);	// start address in bits
	    //> Data
	    pdu = pdu + (char)0x00 + (char)0x04 + (char)0x00 + (char)0x00+
			buffer;

	    mbap = mbap+(char)0x03 + (char)0x00 +
			(char)((pdu.size()+4)>>8) + (char)(pdu.size()+4) +
			pdu;

	    for( int j=0; j < mbap.size(); j++ )
	    {
	        if( (j&0xf)==0 )	printf("\n%x :",j);
	        printf("0x%02X ",mbap[j]);
	    }*/

	    break;
	}
	case ADS:
	{
	    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(addrTr(),0,'.')).at().outAt(TSYS::strSepParse(addrTr(),1,'.'));
	    ResAlloc resN(tr.at().nodeRes(), true);
	    try{ if(!tr.at().startStat()) tr.at().start(); }
	    catch(TError err) { errCon = _("10:Connection error."); throw; }

	    char buf[1000], res[1000];

	    AMS_TCP_HEAD *AmsTcpHD = (AMS_TCP_HEAD *)buf;
	    AmsTcpHD->reserv = 0;
	    AmsTcpHD->len = sizeof(AMS_HEAD) + sizeof(ADS_ReadWriteReq) + buffer.size();

	    AMS_HEAD *AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
	    string tAddr = TSYS::strParse(addr(),0,"|");
	    for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_targ[i_d] = atoi(TSYS::strParse(tAddr,0,".",&i_off).c_str());
	    AmsHD->AMSPort_targ = atoi(TSYS::strParse(tAddr,1,":").c_str());
	    tAddr = TSYS::strParse(addr(),1,"|");
	    for(int i_d = 0, i_off = 0; i_d < 6; i_d++) AmsHD->AMSNetId_src[i_d] = atoi(TSYS::strParse(tAddr,0,".",&i_off).c_str());
	    AmsHD->AMSPort_src = atoi(TSYS::strParse(tAddr,1,":").c_str());
	    AmsHD->com = 3;		//ADS Write
	    AmsHD->stFlgs = 0x4;	//ADS command
	    AmsHD->len = sizeof(ADS_ReadWriteReq) + buffer.size();
	    AmsHD->errCod = 0;
	    AmsHD->InvId = 0;

	    ADS_ReadWriteReq *ADSreq = (ADS_ReadWriteReq *)(AmsHD+1);
	    ADSreq->IdGrp = n_db;
	    ADSreq->IdOff = offset;
	    ADSreq->len = buffer.size();

	    memcpy(ADSreq+1,buffer.data(),buffer.size());

	    //> Request
	    int resp_len;
	    try{ resp_len = tr.at().messIO(buf, AmsTcpHD->len+sizeof(AMS_TCP_HEAD), res, sizeof(res), 0, true); }
	    catch(TError err) { errCon = _("10:Connection error."); throw; }
	    int full_len = resp_len;
	    if(full_len < (int)sizeof(AMS_TCP_HEAD)) throw TError(nodePath().c_str(),_("13:Error server respond"));
	    AmsTcpHD = (AMS_TCP_HEAD *)res;
	    unsigned resp_sz = AmsHD->len;
	    //> Wait tail
	    while(full_len < (int)(resp_sz+sizeof(AMS_TCP_HEAD)))
	    {
		resp_len = tr.at().messIO(NULL, 0, res+full_len, sizeof(res)-full_len, 0, true);
		if(!resp_len) throw TError(nodePath().c_str(),_("13:Not full respond"));
		full_len += resp_len;
	    }
	    //> Check for correct result
	    AmsHD = (AMS_HEAD *)(AmsTcpHD+1);
	    ADS_WriteResp *ADSwriteResp = (ADS_WriteResp *)(AmsHD+1);
	    resp_len = sizeof(AMS_TCP_HEAD)+sizeof(AMS_HEAD);
	    if(full_len < resp_len || AmsHD->com != 3 || AmsHD->stFlgs != 0x05)
		throw TError(nodePath().c_str(),_("13:Error server respond"));
	    if(AmsHD->errCod) throw TError(nodePath().c_str(),_("13:Error server respond: %d"),AmsHD->errCod);
	    if(full_len < (resp_len+=sizeof(ADS_WriteResp)))	throw TError(nodePath().c_str(),_("13:Error server '%s' respond"),"Write");
	    if(ADSwriteResp->res) throw TError(nodePath().c_str(),_("13:Error server '%s' respond: %d"),"Write",ADSwriteResp->res);

	    errCon = "";

	    break;
	}
    }
}

char TMdContr::getValB( SValData ival, ResString &err )
{
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+1) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	{
	    if(!acqBlks[i_b].err.size())
	        return (bool)(acqBlks[i_b].val[ival.off-acqBlks[i_b].off]&(0x01<<ival.sz));
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err.setVal( _("11:Value is not gathered.") );

    return EVAL_BOOL;
}

int64_t TMdContr::getValI( SValData ival, ResString &err )
{
    int iv_sz = valSize( IO::Integer, ival.sz );
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	{
	    if(!acqBlks[i_b].err.size())
		switch(iv_sz)
		{
		    case 1:	return acqBlks[i_b].val[ival.off-acqBlks[i_b].off];
		    case 2:	return *(int16_t*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str();
		    case 4:	return *(int32_t*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str();
		}
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err.setVal( _("11:Value is not gathered.") );

    return (int64_t)EVAL_INT;
}

double TMdContr::getValR( SValData ival, ResString &err )
{
    int iv_sz = valSize( IO::Real, ival.sz );
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	{
	    if(!acqBlks[i_b].err.size())
		switch(iv_sz)
		{
		    case 4:	return TSYS::floatLErev(*(float*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str());
		    case 8:	return TSYS::doubleLErev(*(double*)revers(acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz)).c_str());
		}
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err.setVal( _("11:Value is not gathered.") );

    return EVAL_REAL;
}

string TMdContr::getValS( SValData ival, ResString &err )
{
    int iv_sz = valSize( IO::String, ival.sz );
    for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
	    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()) )
	{
	    if(!acqBlks[i_b].err.size()) return acqBlks[i_b].val.substr(ival.off-acqBlks[i_b].off,iv_sz);
	    else err.setVal(acqBlks[i_b].err);
	    break;
	}
    if(err.getVal().empty()) err.setVal(_("11:Value is not gathered."));

    return EVAL_STR;
}

void TMdContr::setValB( bool ivl, SValData ival, ResString &err )
{
    int val = getValI(SValData(ival.db,ival.off,1),err);
    if(val == EVAL_INT || (bool)(val&(0x01<<ival.sz)) == ivl) return;
    //> Write data to controller or write data block
    val ^= (0x01<<ival.sz);
    try
    {
	if(!assincWrite()) putDB(ival.db,ival.off,string((char*)&val,1));
	else
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
		    (ival.off+1) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val[ival.off-writeBlks[i_b].off] = val;
		    if(atoi(writeBlks[i_b].err.getVal().c_str()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	//> Set to DB buffer
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+1) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    { acqBlks[i_b].val[ival.off-acqBlks[i_b].off] = val; break; }
    }
    catch(TError cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

void TMdContr::setValI( int64_t ivl, SValData ival, ResString &err )
{
    int val = getValI(ival, err);
    if(val == EVAL_INT || val == ivl) return;
    //> Write data to controller or write data block
    val = ivl;
    int iv_sz = valSize(IO::Integer, ival.sz);
    try
    {
	if(!assincWrite()) putDB(ival.db,ival.off,revers(string((char *)&val,iv_sz)));
	else
 	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
			(ival.off+iv_sz) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val.replace(ival.off-writeBlks[i_b].off,iv_sz,revers(string((char *)&val,iv_sz)));
		    if(atoi(writeBlks[i_b].err.getVal().c_str()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	//> Set to DB buffer
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    { acqBlks[i_b].val.replace(ival.off-acqBlks[i_b].off,iv_sz,revers(string((char *)&val,iv_sz))); break; }
    }
    catch(TError cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

void TMdContr::setValR( double ivl, SValData ival, ResString &err )
{
    double val = getValR(ival, err);
    if(val == EVAL_REAL || val == ivl) return;
    //> Write data to controller or write data block
    val = TSYS::doubleLE(ivl);
    float val_4 = TSYS::floatLE(ivl);
    int iv_sz = valSize(IO::Real, ival.sz);
    try
    {
	if(!assincWrite()) putDB(ival.db,ival.off,revers(string((iv_sz==4)?(char*)&val_4:(char *)&val,iv_sz)));
	else
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
			(ival.off+iv_sz) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val.replace(ival.off-writeBlks[i_b].off,iv_sz,revers(string(((iv_sz==4)?(char *)&val_4:(char *)&val),iv_sz)));
		    if(atoi(writeBlks[i_b].err.getVal().c_str()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	//> Set to DB buffer
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    {
		acqBlks[i_b].val.replace(ival.off-acqBlks[i_b].off,iv_sz,revers(string(((iv_sz==4)?(char *)&val_4:(char *)&val),iv_sz)));
		break;
	    }
    }
    catch(TError cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

void TMdContr::setValS( const string &ivl, SValData ival, ResString &err )
{
    string val = getValS(ival,err);
    int iv_sz = valSize(IO::String, ival.sz);
    string vali = ivl;
    vali.resize(iv_sz);
    if(val == EVAL_STR || val == vali) return;
    //> Write data to controller or write data block
    try
    {
	if(!assincWrite())	putDB(ival.db,ival.off,vali);
	else
	    for(unsigned i_b = 0; i_b < writeBlks.size(); i_b++)
		if(writeBlks[i_b].db == ival.db && ival.off >= writeBlks[i_b].off &&
			(ival.off+iv_sz) <= (writeBlks[i_b].off+(int)writeBlks[i_b].val.size()))
		{
		    writeBlks[i_b].val.replace(ival.off-writeBlks[i_b].off,iv_sz,vali);
		    if(atoi(writeBlks[i_b].err.getVal().c_str()) == -1) writeBlks[i_b].err = "";
		    break;
		}
	//> Set to DB buffer
	for(unsigned i_b = 0; i_b < acqBlks.size(); i_b++)
	    if(acqBlks[i_b].db == ival.db && ival.off >= acqBlks[i_b].off &&
		    (ival.off+iv_sz) <= (acqBlks[i_b].off+(int)acqBlks[i_b].val.size()))
	    { acqBlks[i_b].val.replace(ival.off-acqBlks[i_b].off,iv_sz,vali); break; }
    }
    catch(TError cerr) { if(err.getVal().empty()) err.setVal(cerr.mess); }
}

int TMdContr::valSize( IO::Type itp, int iv_sz )
{
    switch(itp)
    {
	case IO::String: 	return (iv_sz<=0)?10:iv_sz;
	case IO::Integer:	return (iv_sz==1||iv_sz==2||iv_sz==4)?iv_sz:2;
	case IO::Real:		return (iv_sz==4||iv_sz==8)?iv_sz:4;
	case IO::Boolean:	return 1;
	default: break;
    }
    throw TError(nodePath().c_str(),_("Value type error."));
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    bool is_start = true;
    bool is_stop  = false;
    int64_t t_cnt, t_prev = TSYS::curTime();

    while(true)
    {
	cntr.call_st = true;
	t_cnt = TSYS::curTime();

	//> Update controller's data
	cntr.nodeRes().resRequestR( );
	//> Process write data blocks
	if(cntr.assincWrite())
	    for(unsigned i_b = 0; !cntr.endrun_req && i_b < cntr.writeBlks.size(); i_b++)
		try
		{
		    if(cntr.redntUse()) { cntr.writeBlks[i_b].err = _("-1:No data"); continue; }
		    if(atoi(cntr.writeBlks[i_b].err.getVal().c_str()) == -1) continue;
		    cntr.putDB(cntr.writeBlks[i_b].db,cntr.writeBlks[i_b].off,cntr.writeBlks[i_b].val);
		    cntr.writeBlks[i_b].err = _("-1:No data");
		}
		catch(TError err) { cntr.writeBlks[i_b].err = err.mess; }
	//> Process acquisition data blocks
	for(unsigned i_b = 0; !cntr.endrun_req && i_b < cntr.acqBlks.size(); i_b++)
	    try
	    {
		if(cntr.redntUse()) { cntr.acqBlks[i_b].err = _("-1:No data"); continue; }
		cntr.getDB(cntr.acqBlks[i_b].db, cntr.acqBlks[i_b].off, cntr.acqBlks[i_b].val);
		cntr.acqBlks[i_b].err="";
	    }
	    catch(TError err) { cntr.acqBlks[i_b].err=err.mess; }

	//> Calc parameters
	for(unsigned i_p = 0; i_p < cntr.pHd.size() && !cntr.redntUse(); i_p++)
	    try{ cntr.pHd[i_p].at().calc(is_start,is_stop,cntr.period()?(1e9/cntr.period()):(-1e-6*(t_cnt-t_prev))); }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

	cntr.nodeRes().resRelease();
	t_prev = t_cnt;
	cntr.tm_calc = TSYS::curTime()-t_cnt;
	cntr.call_st = false;

	if(is_stop) break;

	TSYS::taskSleep((int64_t)cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));

	if(cntr.endrun_req) is_stop = true;
	is_start = false;
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::redntDataUpdate( )
{
    TController::redntDataUpdate();

    vector<string> pls; list(pls);

    //> Request for template's attributes values
    XMLNode req("CntrReqs"); req.setAttr("path",nodePath(0,true));
    for(unsigned i_p = 0; i_p < pls.size(); i_p++)
    {
	if(!at(pls[i_p]).at().enableStat()) continue;
	req.childAdd("get")->setAttr("path","/prm_"+pls[i_p]+"/%2fserv%2ftmplAttr");
    }

    //> Send request to first active station for this controller
    if(owner().owner().rdStRequest(workId(),req).empty()) return;

    //> Redirect respond to local parameters
    req.setAttr("path","/");
    for(unsigned i_prm = 0; i_prm < req.childSize(); )
	if(atoi(req.childGet(i_prm)->attr("err").c_str())) req.childDel(i_prm);
	else { req.childGet(i_prm)->setName("set"); i_prm++; }
    cntrCmd(&req);
}

TMdContr::SDataRec::SDataRec( int idb, int ioff, int v_rez ) : db(idb), off(ioff)
{
    val.assign(v_rez,0);
    err = _("11:Value not gathered.");
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/TYPE",cfg("TYPE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,
	    "help",_("Connection type:\n"
		     "  CIF_PB - connection to controllers series S7, by firm Siemens, by communication unit CIF-50PB or like;\n"
		     "  ISO_TCP, ISO_TCP243 - connection to controllers series S7, by firm Siemens, by Ethernet network (TCP243 by CP243);\n"
		     "  ADS - TwinCAT ADS/AMS protocol for connection to controllers firm Beckhoff."));
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,
	    "help",_("Remote controller address. For connections:\n"
		     "  CIF_PB - controller address in \"Profibus\" network, digit 0-255;\n"
		     "  ISO_TCP, ISO_TCP243 - IP-address into Ethernet network;\n"
		     "  ADS - Network identifier and port for target and source stations, in view\n"
		     "      \"{Target_AMSNetId}:{Target_AMSPort}|{Source_AMSNetId}:{Source_AMSPort}\"\n"
		     "      (for example: \"192.168.0.1.1.1:801|82.207.88.73.1.1:801\"), where:\n"
		     "    AMSNetId - network identifier, write into view of six digits 0-255, for example: \"192.168.0.1.1.1\";\n"
		     "    AMSPort - port, write into view digit 0-65535."));
	XMLNode *xt = ctrId(opt->childGet(0),"/cntr/cfg/ADDR_TR",true);
	if(xt) xt->setAttr("dest","select")->setAttr("select","/cntr/cfg/trLst");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt))
    {
        vector<string> sls;
        SYS->transport().at().outTrList(sls);
        for(unsigned i_s = 0; i_s < sls.size(); i_s++)
            opt->childAdd("el")->setText(sls[i_s]);
    }
    else TController::cntrCmdProc(opt);
}

//************************************************
//* TMdPrm                                       *
//************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), TValFunc(name+"SiemensPrm"), p_el("cif_attr"),
    id_freq(-1), id_start(-1), id_stop(-1), id_err(-1), id_sh(-1), id_nm(-1), id_dscr(-1), acq_err_tm(0)
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

void TMdPrm::postDisable(int flag)
{
    TParamContr::postDisable(flag);

    try
    {
	if(flag)
	{
	    string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";
	    TConfig cfg(&mod->prmIOE());
	    cfg.cfg("PRM_ID").setS(id(),true);
	    SYS->db().at().dataDel(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg);
	}
    }catch(TError err)	{ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat() && !owner().isReload) return;

    TParamContr::enable();
    //> Template's function connect
    try
    {
	vector<string> als;
	bool to_make = false;
	unsigned fId = 0;
	if(!func())
	{
	    setFunc(&SYS->daq().at().tmplLibAt(TSYS::strSepParse(cfg("TMPL").getS(),0,'.')).at().
				     at(TSYS::strSepParse(cfg("TMPL").getS(),1,'.')).at().func().at());
	    to_make = true;
	}
	//>> Init attrubutes
	for(int i_io = 0; i_io < func()->ioSize(); i_io++)
	{
	    if((func()->io(i_io)->flg()&TPrmTempl::CfgLink) && lnkId(i_io) < 0)	plnk.push_back(SLnk(i_io));
	    if((func()->io(i_io)->flg()&(TPrmTempl::AttrRead|TPrmTempl::AttrFull)))
	    {
		unsigned flg = TVal::DirWrite|TVal::DirRead;
		if(func()->io(i_io)->flg()&IO::FullText)	flg |= TFld::FullText;
		if(func()->io(i_io)->flg()&TPrmTempl::AttrRead)	flg |= TFld::NoWrite;
		TFld::Type tp = TFld::type(ioType(i_io));
		if((fId=p_el.fldId(func()->io(i_io)->id(),true)) < p_el.fldSize())
		{
		    if(p_el.fldAt(fId).type() != tp)
        		try{ p_el.fldDel(fId); }
        		catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
		    else
		    {
			p_el.fldAt(fId).setFlg(flg);
			p_el.fldAt(fId).setDescr(func()->io(i_io)->name().c_str());
		    }
        	}

		if(!vlPresent(func()->io(i_io)->id()))
		    p_el.fldAdd(new TFld(func()->io(i_io)->id().c_str(),func()->io(i_io)->name().c_str(),tp,flg));

		als.push_back(func()->io(i_io)->id());
	    }
	    if(to_make && (func()->io(i_io)->flg()&TPrmTempl::CfgLink)) setS(i_io,"0");
	}

	//>> Init links
	initLnks();

	//>> Init system attributes identifiers
	id_freq  = ioId("f_frq");
	id_start = ioId("f_start");
	id_stop  = ioId("f_stop");
	id_err   = ioId("f_err");
        id_sh    = ioId("SHIFR");
        id_nm    = ioId("NAME");
        id_dscr  = ioId("DESCR");
        int id_this = ioId("this");
        if(id_this >= 0) setO(id_this, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));

	//>> Load IO at enabling
	if(to_make)	loadIO(true);

	//> Check for delete DAQ parameter's attributes
	for(int i_p = 0; i_p < (int)p_el.fldSize(); i_p++)
	{
	    unsigned i_l;
	    for(i_l = 0; i_l < als.size(); i_l++)
		if(p_el.fldAt(i_p).name() == als[i_l])
		    break;
	    if(i_l >= als.size())
        	try{ p_el.fldDel(i_p); i_p--; }
        	catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
	}

	//>> Set to process
	if(owner().startStat())	calc(true,false,0);

    }catch(TError err) { disable(); throw; }

    owner().prmEn(id(), true);
}

void TMdPrm::disable()
{
    if(!enableStat()) return;

    //> Unregister parameter
    owner().prmEn(id(), false);
    if(owner().startStat()) calc(false,true,0);

    //> Template's function disconnect
    setFunc(NULL);
    id_freq = id_start = id_stop = id_err = id_sh = id_nm = id_dscr = -1;
    plnk.clear();

    TParamContr::disable();
}

void TMdPrm::load_( )
{
    TParamContr::load_();
    loadIO();
}

void TMdPrm::loadIO( bool force )
{
    //> Load IO and init links
    if(!enableStat())	return;
    if(owner().startStat() && !force) { modif(true); return; }	//Load/reload IO context only allow for stoped controlers for prevent throws

    TConfig cfg(&mod->prmIOE());
    cfg.cfg("PRM_ID").setS(id());
    string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";

    for(int i_io = 0; i_io < ioSize(); i_io++)
    {
	cfg.cfg("ID").setS(func()->io(i_io)->id());
	if(!SYS->db().at().dataGet(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg)) continue;
	if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)
	    lnk(lnkId(i_io)).db_addr = cfg.cfg("VALUE").getS();
	else setS(i_io,cfg.cfg("VALUE").getS());
    }
    initLnks();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
    saveIO();
}

void TMdPrm::saveIO()
{
    //> Save IO and init links
    if(!enableStat())	return;

    TConfig cfg(&mod->prmIOE());
    cfg.cfg("PRM_ID").setS(id());
    string io_bd = owner().DB()+"."+type().DB(&owner())+"_io";

    for(int i_io = 0; i_io < func()->ioSize(); i_io++)
    {
	cfg.cfg("ID").setS(func()->io(i_io)->id());
	if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)
	    cfg.cfg("VALUE").setS(lnk(lnkId(i_io)).db_addr);
	else cfg.cfg("VALUE").setS(getS(i_io));
	SYS->db().at().dataSet(io_bd,owner().owner().nodePath()+type().DB(&owner())+"_io",cfg);
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat())
    {
	if(val.name() == "err")
	{
	    if(!enableStat())			val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Controller is stopped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }
    if(owner().redntUse()) return;
    if(val.name() != "err")
	try
	{
	    int id_lnk = lnkId(val.name());
	    if(id_lnk >= 0 && lnk(id_lnk).val.db < 0) id_lnk = -1;
	    if(id_lnk < 0) val.set(get(ioId(val.name())),0,true);
	    else
		switch(val.fld().type())
		{
		    case TFld::String:	val.setS(owner().getValS(lnk(id_lnk).val,acq_err),0,true);	break;
		    case TFld::Integer:	val.setI(owner().getValI(lnk(id_lnk).val,acq_err),0,true);	break;
		    case TFld::Real:	val.setR(owner().getValR(lnk(id_lnk).val,acq_err),0,true);	break;
		    case TFld::Boolean:	val.setB(owner().getValB(lnk(id_lnk).val,acq_err),0,true);	break;
		    default: break;
		}
	}catch(TError err) { }
    else
    {
	if(acq_err.getVal().size()) val.setS(acq_err.getVal(),0,true);
	else if(id_err >= 0) val.setS(getS(id_err),0,true);
	else val.setS("0",0,true);
    }
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setS(EVAL_STR, 0, true); return; }

    //> Send to active reserve station
    if(owner().redntUse())
    {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //> Direct write
    try
    {
	int id_lnk = lnkId(vo.name());
	if(id_lnk >= 0 && lnk(id_lnk).val.db < 0) id_lnk = -1;
	if(!(vl.isEVal() || vl == pvl))
	{
	    if(id_lnk < 0) set(ioId(vo.name()), vl);
	    else
		switch(vo.fld().type())
		{
		    case TFld::String:	owner().setValS(vl.getS(), lnk(id_lnk).val, acq_err);	break;
		    case TFld::Integer:	owner().setValI(vl.getI(), lnk(id_lnk).val, acq_err);	break;
		    case TFld::Real:	owner().setValR(vl.getR(), lnk(id_lnk).val, acq_err);	break;
		    case TFld::Boolean:	owner().setValB(vl.getB(), lnk(id_lnk).val, acq_err);	break;
		    default: break;
		}
	}
    }catch(TError err) {  }
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

int TMdPrm::lnkSize()
{
    if(!enableStat()) throw TError(nodePath().c_str(),_("Parameter is disabled."));
    return plnk.size();
}

int TMdPrm::lnkId( int id )
{
    if(!enableStat()) throw TError(nodePath().c_str(),_("Parameter is disabled."));
    for(unsigned i_l = 0; i_l < plnk.size(); i_l++)
	if(lnk(i_l).io_id == id)
	    return i_l;
    return -1;
}

int TMdPrm::lnkId( const string &id )
{
    if(!enableStat()) throw TError(nodePath().c_str(),_("Parameter is disabled."));
    for(unsigned i_l = 0; i_l < plnk.size(); i_l++)
	if(func()->io(lnk(i_l).io_id)->id() == id)
	    return i_l;
    return -1;
}

TMdPrm::SLnk &TMdPrm::lnk( int num )
{
    if(!enableStat() || num < 0 || num >= (int)plnk.size())
	throw TError(nodePath().c_str(),_("Parameter is disabled or id error."));
    return plnk[num];
}

void TMdPrm::initLnks()
{
    if(!enableStat())	return;
    //> Init links
    for(int i_l = 0; i_l < lnkSize(); i_l++)
    {
	if(ioType(lnk(i_l).io_id) == IO::Boolean)
	{
	    if(sscanf(lnk(i_l).db_addr.c_str(),"DB%i.%i.%d",&lnk(i_l).val.db,&lnk(i_l).val.off,&lnk(i_l).val.sz) == 3)
		lnk(i_l).val.sz = vmax(0,vmin(7,lnk(i_l).val.sz));
	    else lnk(i_l).val.db = -1;
	}
	else
	{
	    if(sscanf(lnk(i_l).db_addr.c_str(),"DB%i.%i",&lnk(i_l).val.db,&lnk(i_l).val.off) == 2)
		lnk(i_l).val.sz = atoi(TSYS::strParse(func()->io(lnk(i_l).io_id)->def(),2,"|").c_str());
	    else lnk(i_l).val.db = -1;
	}
	//>> Process data
	if(lnk(i_l).val.db < 0 || lnk(i_l).val.off < 0) lnk(i_l).val.db = lnk(i_l).val.off = -1;
	else owner().regVal(lnk(i_l).val,ioType(lnk(i_l).io_id),ioFlg(lnk(i_l).io_id)&(IO::Output|IO::Return));
    }
}

void TMdPrm::calc( bool first, bool last, double frq )
{
    try
    {
	//> Proccess error hold
	if(!acq_err.getVal().empty())
	{
	    time_t tm = time(NULL);
	    if(!acq_err_tm)	acq_err_tm = tm+5;
	    if(tm > acq_err_tm)	{ acq_err.setVal(""); acq_err_tm = 0; }
	}

	//> Set fixed system attributes
	if(id_freq >= 0)	setR(id_freq, frq);
	if(id_start >= 0)	setB(id_start, first);
	if(id_stop >= 0)	setB(id_stop, last);
	if(id_sh >= 0)        	setS(id_sh, id());
        if(id_nm >= 0)        	setS(id_nm, name());
        if(id_dscr >= 0)      	setS(id_dscr, descr());

	//> Get input links
	for(int i_l = 0; i_l < lnkSize(); i_l++)
	    if(lnk(i_l).val.db >= 0)
		switch(ioType(lnk(i_l).io_id))
		{
		    case IO::String:
			setS(lnk(i_l).io_id,owner().getValS(lnk(i_l).val,acq_err));
			break;
		    case IO::Integer:
			setI(lnk(i_l).io_id,owner().getValI(lnk(i_l).val,acq_err));
			break;
		    case IO::Real:
			setR(lnk(i_l).io_id,owner().getValR(lnk(i_l).val,acq_err));
			break;
		    case IO::Boolean:
			setB(lnk(i_l).io_id,owner().getValB(lnk(i_l).val,acq_err));
			break;
		    case IO::Object: break;
		}
	//> Calc template
	TValFunc::calc();
	modif();

	//> Put output links
	for(int i_l = 0; i_l < lnkSize(); i_l++)
	    if(lnk(i_l).val.db >= 0 && ioFlg(lnk(i_l).io_id)&(IO::Output|IO::Return))
		switch(ioType(lnk(i_l).io_id))
		{
		    case IO::String:
			owner().setValS(getS(lnk(i_l).io_id),lnk(i_l).val,acq_err);
			break;
		    case IO::Integer:
			owner().setValI(getI(lnk(i_l).io_id),lnk(i_l).val,acq_err);
			break;
		    case IO::Real:
			owner().setValR(getR(lnk(i_l).io_id),lnk(i_l).val,acq_err);
			break;
		    case IO::Boolean:
			owner().setValB(getB(lnk(i_l).io_id),lnk(i_l).val,acq_err);
			break;
		    case IO::Object: break;
		}

	//> Put fixed system attributes
        if(id_nm >= 0)  setName(getS(id_nm));
        if(id_dscr >= 0)setDescr(getS(id_dscr));

    }catch(TError err)
    {
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Error calculate template."));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")
    {
	if(a_path == "/serv/tmplAttr")
	{
	    if(!enableStat() || !func()) throw TError(nodePath().c_str(),_("Parameter disabled or error."));
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
		for(int i_a = 0; i_a < ioSize(); i_a++)
		    opt->childAdd("a")->setAttr("id",func()->io(i_a)->id())->setText(getS(i_a));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		for(unsigned i_a = 0; i_a < opt->childSize(); i_a++)
		{
		    int io_id = -1;
		    if(opt->childGet(i_a)->name() != "a" || (io_id=ioId(opt->childGet(i_a)->attr("id"))) < 0) continue;
		    setS(io_id,opt->childGet(i_a)->text());
		}
	}
	else TParamContr::cntrCmdProc(opt);
	return;
    }

    vector<string> list;
    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMPL",cfg("TMPL").fld().descr(),RWRW__,"root",SDAQ_ID,3,"tp","str","dest","select","select","/prm/tmplList");
	if(enableStat() && ctrMkNode("area",opt,-1,"/cfg",_("Template configuration")) )
	{
	    ctrMkNode("fld",opt,-1,"/cfg/only_off",_("Only DB offsets are to be shown"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/cfg/prm",_("Parameters")))
		for(int i_io = 0; i_io < ioSize(); i_io++)
		{
		    if(!(func()->io(i_io)->flg()&(TPrmTempl::CfgLink|TPrmTempl::CfgPublConst)))
			continue;
		    //>> Check select param
		    bool is_lnk = func()->io(i_io)->flg()&TPrmTempl::CfgLink;
		    if(is_lnk && func()->io(i_io)->def().size() &&
			!atoi(TBDS::genDBGet(mod->nodePath()+"onlOff","0",opt->attr("user")).c_str()))
		    {
			string nprm = TSYS::strSepParse(func()->io(i_io)->def(),0,'|');
			//>> Check already to present parameters
			bool f_ok = false;
			for(unsigned i_l = 0; i_l < list.size() && !f_ok; i_l++)
			    if(list[i_l] == nprm) f_ok = true;
			if(!f_ok)
			{
			    ctrMkNode("fld",opt,-1,(string("/cfg/prm/pr_")+TSYS::int2str(i_io)).c_str(),nprm,RWRWR_,"root",SDAQ_ID,1,"tp","str");
			    list.push_back(nprm);
			}
		    }
		    else
		    {
			const char *tip = "str";
			bool fullTxt = false;
			if(!is_lnk)
			    switch(ioType(i_io))
			    {
				case IO::Integer:	tip = "dec";	break;
				case IO::Real:		tip = "real";	break;
				case IO::Boolean:	tip = "bool";	break;
				case IO::String:
                                    if(func()->io(i_io)->flg()&IO::FullText) fullTxt = true;
                                    break;
                                case IO::Object:	fullTxt = true; break;
			    }
			XMLNode *wn = ctrMkNode("fld",opt,-1,(string("/cfg/prm/el_")+TSYS::int2str(i_io)).c_str(),func()->io(i_io)->name(),RWRWR_,"root",SDAQ_ID,1,"tp",tip);
			if(wn && fullTxt) wn->setAttr("cols","100")->setAttr("rows","4");
		    }
		}
	}
	return;
    }
    //> Process command to page
    if(a_path == "/prm/cfg/TMPL" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
    {
	cfg("TMPL").setS(opt->text());
	disable();
    }
    else if(a_path == "/cfg/only_off" && enableStat())
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText(TBDS::genDBGet(mod->nodePath()+"onlOff","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    TBDS::genDBSet(mod->nodePath()+"onlOff",opt->text(),opt->attr("user"));
    }
    else if(a_path.substr(0,12) == "/cfg/prm/pr_" && enableStat())
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    int lnk_id = lnkId(atoi(a_path.substr(12).c_str()));
	    string sdb = TSYS::strSepParse(lnk(lnk_id).db_addr,0,'.');
	    int off = atoi(TSYS::strSepParse(lnk(lnk_id).db_addr,1,'.').c_str());
	    int t_off = atoi(TSYS::strSepParse(func()->io(lnk(lnk_id).io_id)->def(),1,'|').c_str());
	    if((off-t_off)>0)	sdb=sdb+"."+TSYS::int2str(off-t_off);
	    opt->setText(sdb);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    string p_nm, cp_nm;
	    int off, coff, cbit;
	    int lnk_id = lnkId(atoi(a_path.substr(12).c_str()));
	    string sdb = TSYS::strSepParse(opt->text(),0,'.');
	    off = strtol(TSYS::strSepParse(opt->text(),1,'.').c_str(),NULL,0);
	    p_nm = TSYS::strSepParse(func()->io(lnk(lnk_id).io_id)->def(),0,'|');

	    for(int i_l = 0; i_l < lnkSize(); i_l++)
	    {
		cp_nm = TSYS::strSepParse(func()->io(lnk(i_l).io_id)->def(),0,'|');
		sscanf(TSYS::strSepParse(func()->io(lnk(i_l).io_id)->def(),1,'|').c_str(),"%i.%d",&coff,&cbit);
		if(p_nm == cp_nm)
		{
		    lnk(i_l).db_addr = sdb+"."+TSYS::int2str(off+coff);
		    if(ioType(lnk(i_l).io_id)==IO::Boolean)
			lnk(i_l).db_addr = lnk(i_l).db_addr+"."+TSYS::int2str(cbit);
		    modif();
		}
	    }
	    initLnks();
	}
    }
    else if(a_path.substr(0,12) == "/cfg/prm/el_" && enableStat())
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    int i_io = atoi(a_path.substr(12).c_str());
	    if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)
		opt->setText(lnk(lnkId(i_io)).db_addr);
	    else if(func()->io(i_io)->flg()&TPrmTempl::CfgPublConst)
		opt->setText(getS(i_io));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    int i_io = atoi(a_path.substr(12).c_str());
	    if(func()->io(i_io)->flg()&TPrmTempl::CfgLink)
	    {
		lnk(lnkId(i_io)).db_addr = opt->text();
		initLnks();
	    }
	    else if(func()->io(i_io)->flg()&TPrmTempl::CfgPublConst) setS(i_io,opt->text());
	    modif();
	}
    }
    else TParamContr::cntrCmdProc(opt);
}

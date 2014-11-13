
//OpenSCADA system module DAQ.SMH2Gi file: module.cpp
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

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

#include <algorithm>

#include "shm.hpp"

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SMH2Gi"
#define MOD_NAME	_("Segnetics SMH2Gi")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.5.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Data acquisition and control by Segnetics SMH2Gi (http://segnetics.com/smh_2gi) hardware interfaces and modules.")
#define LICENSE		"GPL2"
//*************************************************

SMH2Gi::TTpContr *SMH2Gi::mod;  //Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_SMH2Gi_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_SMH2Gi_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new SMH2Gi::TTpContr(source);
	return NULL;
    }
}

using namespace SMH2Gi;

//*************************************************
//* SMH2Gi::TTpContr                              *
//*************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID), mMRCDirDevs(oscd_datadir_full"/SegneticsMRC")
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

TTpContr::~TTpContr()
{

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD_SHM",_("Shared memory parameters"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("PRM_BD_MRC",_("MR and MC bus parameters"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("SHM_VARS",_("Shared memory variables file"),TFld::String,TFld::NoFlag,"255","/projects/load_files.srv"));
    fldAdd(new TFld("MR_DEV",_("MR bus device"),TFld::String,TFld::NoFlag,"50","/dev/mrext"));
    fldAdd(new TFld("MC_DEV",_("MC bus device"),TFld::String,TFld::NoFlag,"50","/dev/mrint"));
    fldAdd(new TFld("REQ_TRY",_("Request tries"),TFld::Integer,TFld::NoFlag,"1","1","1;9"));

    //> Parameters' types add
    tpParmAdd(new SHMParam());
    tpParmAdd(new MRCParam());
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

void TTpContr::setMRCDirDevs( const string &vl )
{
    mMRCDirDevs = vl;
    modif();

    ResAlloc res(MRCdevsRes, true);

    //> Clear previous
    MRCdevs.clear();

    //> Scan directory for MR/MC devices *.ini files load
    DevMRCFeature devIni;
    string fn, f_data;
    dirent scan_dirent, *scan_rez = NULL;
    DIR *IdDir = opendir(mod->MRCDirDevs().c_str());
    while(IdDir && readdir_r(IdDir,&scan_dirent,&scan_rez) == 0 && scan_rez)
    {
	fn = mod->MRCDirDevs()+"/"+scan_rez->d_name;
	if(scan_rez->d_type != DT_REG || fn.compare(fn.size()-4,4,".ini") != 0) continue;
	if(devIni.load(fn))
	{
	    if(devIni.HardID) MRCdevs[devIni.HardID] = devIni;
	    else if(strcmp(scan_rez->d_name,"ain_tunes.ini") == 0) MRCdevs[-1] = devIni;
	}
    }
    if(IdDir) closedir(IdDir);
}

void TTpContr::perSYSCall( unsigned int cnt )
{
    //> Check for restart some conrollers need
    vector<string> cls, pls;
    list(cls);
    for(unsigned i_c = 0; i_c < cls.size(); i_c++)
    {
	AutoHD<TController> cntr = at(cls[i_c]);
	if(!cntr.at().startStat()) continue;
	cntr.at().list(pls);
	for(unsigned i_p = 0; i_p < pls.size(); i_p++)
	{
	    int errCode = cntr.at().at(pls[i_p]).at().vlAt("err").at().getI();
	    if(errCode == 21)	//By MR/MC bus lost
	    {
		mess_warning(cntr.at().nodePath().c_str(), _("Re-enable by Reinit flag for MC/MR module."));
		cntr.at().disable();
		cntr.at().start();
		break;
	    }
	}
    }
}

void TTpContr::load_( )
{
    //Load parameters from config-file
    setMRCDirDevs(TBDS::genDBGet(nodePath()+"MRCDirDevs",MRCDirDevs()));
}

void TTpContr::save_( )
{
    //Save parameters to config-file
    TBDS::genDBSet(nodePath()+"MRCDirDevs",MRCDirDevs());
}

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTipDAQ::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm","SMH2Gi"))
	    ctrMkNode("fld",opt,-1,"/prm/dirMRC",_("MR/MC devices *.ini files directry"),RWRWR_,"root",SDAQ_ID,3,
		"tp","str","dest","sel_ed","select","/prm/dirMRCList");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/dirMRC")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(MRCDirDevs());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setMRCDirDevs(opt->text());
    }
    else if(a_path == "/prm/dirMRCList" && ctrChkNode(opt))	TSYS::ctrListFS(opt, MRCDirDevs());
    else TTipDAQ::cntrCmdProc(opt);
}

//*************************************************
//* SMH2Gi::TMdContr                              *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    ::TController(name_c,daq_db,cfgelem), smv(NULL), m_prior(cfg("PRIOR").getId()), connTry(cfg("REQ_TRY").getId()),
    prcSt(false), callSt(false), endrunReq(false), tmGath(0)
{
    cfg("PRM_BD_SHM").setS("SMH2GiPrmSHM_"+name_c);
    cfg("PRM_BD_MRC").setS("SMH2GiPrmMRC_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if(startStat() && !redntUse())
    {
	if(callSt)	rez += TSYS::strMess(_("Call now. "));
	if(period())	rez += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-3*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "), tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s."), tm2s(tmGath).c_str());
    }
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    //> Shared memory object init
    if(!smv) smv = new Shm(cfg("SHM_VARS").getS().c_str());

    //> MC bus init by send reset to GPIO PC30
    //>> Open GPIO PC30 (/sys/class/gpio/gpio94)
    int hd = open("/sys/class/gpio/gpio94/value", O_WRONLY);
    if(hd < 0) throw TError(nodePath().c_str(), _("Error GPIO PC30 open for MC reset."));
    lseek(hd, 0, SEEK_SET);
    if(write(hd,"1",1) != 1) mess_err(nodePath().c_str(), _("Write to gpio94 error!"));
    TSYS::sysSleep(50e-3);	//50ms

    lseek(hd, 0, SEEK_SET);
    if(write(hd,"0",1) != 1) mess_err(nodePath().c_str(), _("Write to gpio94 error!"));
    TSYS::sysSleep(200e-3);	//200ms

    close(hd);

    //> MR bus init by send 250*0xCC symbols
    string req(250,0xCC);
    modBusReq(req);
    TSYS::sysSleep(60e-3);
}

void TMdContr::disable_( )
{
    //> Shared memory object free
    if(smv) { delete smv; smv = NULL; }
}

void TMdContr::start_( )
{
    //> Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

    //> Start the gathering data task
    SYS->taskCreate(nodePath('.',true), m_prior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endrunReq);

    //> Set EVal
    ResAlloc res(en_res,false);
    for(unsigned i_p = 0; i_p < p_hd.size(); i_p++)
	p_hd[i_p].at().setEval();
}

void TMdContr::prmEn( const string &id, bool val )
{
    unsigned i_prm;

    ResAlloc res(en_res,true);
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

string TMdContr::modBusReq( string &pdu, bool MC, bool broadCast )
{
    char buf[1024];
    string mbap, rez, err;

    try
    {
	//> Transport creation for MC and MR busses and connect
	AutoHD<TTransportOut> tro = SYS->transport().at().nodeAt(MC?"Serial.out_SMH2Gi_MC":"Serial.out_SMH2Gi_MR", 0, '.', 0, true);
	if(tro.freeStat())
	{
	    SYS->transport().at().at("Serial").at().outAdd(MC?"SMH2Gi_MC":"SMH2Gi_MR");
	    tro = SYS->transport().at().nodeAt(MC?"Serial.out_SMH2Gi_MC":"Serial.out_SMH2Gi_MR", 0, '.', 0, true);
	    //>> Typical parameters set
	    tro.at().setDscr(TSYS::strMess(_("Segnetics SMH2Gi automatic created transport for '%s' bus"),MC?"MC":"MR"));
	    tro.at().setAddr(MC?(cfg("MC_DEV").getS()+":230400:8N1"):(cfg("MR_DEV").getS()+":230400:8N2"));
	}

	ResAlloc resN(tro.at().nodeRes(), true);

	//> Start stoped transport
	if(!tro.at().startStat()) tro.at().start();

	//> Prepare request
	mbap.reserve(pdu.size()+2);
	mbap += pdu;
	uint16_t crc = CRC16(mbap);
	mbap += (crc>>8);
	mbap += crc;

	//> Send request
	for(int i_tr = 0; i_tr < connTry; i_tr++)
	{
	    if(messLev() == TMess::Debug)
		mess_debug_(nodePath().c_str(), _("ModBUS REQ -> '%s': %s"), tro.at().id().c_str(), TSYS::strDecode(mbap,TSYS::Bin," ").c_str());
	    int resp_len = tro.at().messIO(mbap.data(), mbap.size(), (broadCast?NULL:buf), sizeof(buf), 0, true);
	    if(broadCast) { err = ""; break; }
	    rez.assign(buf, resp_len);
	    //> Wait tail
	    while(resp_len)
	    {
		try{ resp_len = tro.at().messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
		rez.append(buf, resp_len);
	    }

	    if(rez.size() < 2) { err = _("13:Error respond: Too short."); continue; }
	    if(CRC16(rez.substr(0,rez.size()-2)) != (uint16_t)((rez[rez.size()-2]<<8)+(uint8_t)rez[rez.size()-1]))
	    { err = _("13:Error respond: CRC check error."); continue; }
	    if(messLev() == TMess::Debug)
		mess_debug_(nodePath().c_str(), _("ModBUS RESP -> '%s': %s"), tro.at().id().c_str(), TSYS::strDecode(rez,TSYS::Bin," ").c_str());
	    pdu = rez.substr(0, rez.size()-2);
	    err = "";
	    break;
	}
    }
    catch(TError ierr) { err = "10:"+ierr.mess; }

    if(messLev() >= TMess::Error && err.size()) mess_err(nodePath().c_str(), "%s", err.c_str());
    if(messLev() == TMess::Debug && err.size())
	mess_debug_(nodePath().c_str(), _("ModBUS ERR -> %s: %s"), TSYS::strDecode(mbap,TSYS::Bin," ").c_str(), err.c_str());

    return err;
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    while(!cntr.endrunReq)
    {
	int64_t t_cnt = TSYS::curTime();

	//> Update controller's data
	cntr.en_res.resRequestR( );
	cntr.callSt = true;

	//>> MR frame prepare
	if(cntr.MRWrFrm.size() != 8)
	{
	    cntr.MRWrFrm.clear();
	    for(int i_n = 0; i_n < 8; i_n++)
		cntr.MRWrFrm += (char)8;
	}

	//>> Call for get vals
	for(unsigned i_p = 0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++)
	    try { cntr.p_hd[i_p].at().getVals(); }
	    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

	//>> Send generic MR write frame
	if(cntr.MRWrFrm.size() != 8)
	{
	    string pdu;
	    //> Swap registers
	    for(int i_p = 0; i_p < (int)cntr.MRWrFrm.size()-1; i_p += 2)
	    { char t_sw = cntr.MRWrFrm[i_p]; cntr.MRWrFrm[i_p] = cntr.MRWrFrm[i_p+1]; cntr.MRWrFrm[i_p+1] = t_sw; }

	    //>> Prepare header
	    pdu = (char)0xFE;				//BroadCast
	    pdu += (char)0x10;				//Function, preset multiple registers
	    pdu += (char)0;
	    pdu += (char)0;
	    pdu += (char)((cntr.MRWrFrm.size()/2)>>8);	//Registers quantity MSB
	    pdu += (char)(cntr.MRWrFrm.size()/2);	//Registers quantity LSB
	    pdu += (char)cntr.MRWrFrm.size();		//Byte Count
	    pdu += cntr.MRWrFrm;
	    cntr.modBusReq(pdu, false, true);
	}

	cntr.callSt = false;
	cntr.en_res.resRelease( );
	cntr.tmGath = TSYS::curTime()-t_cnt;

	TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
    }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
		  "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SHM_VARS",cfg("SHM_VARS").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","sel_ed","select","/cntr/cfg/fileShmL");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/MC_DEV",cfg("MC_DEV").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","sel_ed","select","/cntr/cfg/devMCLs");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/MR_DEV",cfg("MR_DEV").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","sel_ed","select","/cntr/cfg/devMRLs");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/fileShmL" && ctrChkNode(opt))	TSYS::ctrListFS(opt, cfg("SHM_VARS").getS(), "srv;");
    else if(a_path == "/cntr/cfg/devMCLs" && ctrChkNode(opt))	TSYS::ctrListFS(opt, cfg("MC_DEV").getS(), "<chrdev>;");
    else if(a_path == "/cntr/cfg/devMRLs" && ctrChkNode(opt))	TSYS::ctrListFS(opt, cfg("MR_DEV").getS(), "<chrdev>;");
    else TController::cntrCmdProc(opt);
}

uint8_t TMdContr::CRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

uint8_t TMdContr::CRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t TMdContr::CRC16( const string &mbap )
{
    uint8_t hi = 0xFF;
    uint8_t lo = 0xFF;
    uint16_t index;
    for(unsigned i_b = 0; i_b < mbap.size(); i_b++)
    {
	index = lo^(uint8_t)mbap[i_b];
	lo = hi^CRCHi[index];
	hi = CRCLo[index];
    }
    return hi|(lo<<8);
}

//*************************************************
//* SMH2Gi::TMdPrm                                *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), extPrms(NULL)
{

}

TMdPrm::~TMdPrm( )	{ nodeDelAll(); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat()) return;

    als.clear();
    TParamContr::enable();

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
    als.clear();

    owner().prmEn(id(), true);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    setEval();
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat())
    {
	if(val.name() == "err")
	{
	    if(!enableStat())			val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition is stopped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

    if(owner().redntUse()) return;

    if(val.name() == "err")
    {
	if(acq_err.getVal().empty())	val.setS("0",0,true);
	else				val.setS(acq_err.getVal(),0,true);
    }
}

void TMdPrm::getVals( )
{
    //> Values gather process
    type().getVals(this);
}

void TMdPrm::setEval( )
{
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);
}

string TMdPrm::modPrm( const string &prm, const string &def )
{
    string rez;
    XMLNode prmNd;
    try
    {
	prmNd.load(cfg("MOD_PRMS").getS());
	string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
	if(!sa.size())  return (rez=prmNd.attr(prm)).empty()?def:rez;
	//> Internal node
	for(unsigned i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
		return (rez=prmNd.childGet(i_n)->attr(sa)).empty()?def:rez;
    } catch(...){ }

    return def;
}

void TMdPrm::setModPrm( const string &prm, const string &val )
{
    XMLNode prmNd("ModCfg");
    try { prmNd.load(cfg("MOD_PRMS").getS()); } catch(...){ }

    if(modPrm(prm) != val) modif();
    string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
    if(!sa.size()) prmNd.setAttr(prm,val);
    //> Internal node
    else
    {
	unsigned i_n;
	for(i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
	    { prmNd.childGet(i_n)->setAttr(sa,val); break; }
	if(i_n >= prmNd.childSize())
	    prmNd.childAdd(sobj)->setAttr(sa,val);
    }

    cfg("MOD_PRMS").setS(prmNd.save(XMLNode::BrAllPast));
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod((int64_t)(owner().period()*1000000));
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/MOD_PRMS");
	return;
    }
    //> Process command to page
    //string a_path = opt->attr("path");
    TParamContr::cntrCmdProc(opt);
}

//****************************************************
//* SMH2Gi::SHMParam - shared memory parameter type  *
//****************************************************
SHMParam::SHMParam( ) : TTipParam("SHM", _("Shared memory parameters"), "PRM_BD_SHM")
{
    fldAdd(new TFld("VAR_LS",_("Variables list"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
}

void SHMParam::enable( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    //> Read, check and create attributes from variables list
    string sid, sel;
    for(int ioff = 0; (sel=TSYS::strParse(p->cfg("VAR_LS").getS(),0,"\n",&ioff)).size(); )
    {
	sid = TSYS::strEncode(sel, TSYS::oscdID);
	int vtp = p->owner().smv->getType(sel.c_str());
	if(vtp < 0) continue;
	TFld::Type tp = (TFld::Type)-1;
	switch(vtp)
	{
	    case BOOL:	tp = TFld::Boolean;	break;
	    case SHORT:
	    case LONG:	tp = TFld::Integer;	break;
	    case FLOAT:	tp = TFld::Real;	break;
	}
	p->p_el.fldAdd(new TFld(sid.c_str(),sel.c_str(),tp,TVal::DirWrite,"","","","",TSYS::int2str(vtp).c_str()));
	p->als.push_back(sid);
    }
}

void SHMParam::getVals( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    string tnm, tvar, vals;
    for(int i_p = 0; i_p < (int)p->p_el.fldSize(); i_p++)
    {
	tnm = p->p_el.fldAt(i_p).name();
	AutoHD<TVal> vl = p->vlAt(tnm);
	tvar = p->p_el.fldAt(i_p).descr();
	switch(atoi(p->p_el.fldAt(i_p).reserve().c_str()))
	{
	    case BOOL:
	    {
		char rvl = p->owner().smv->getBool(tvar.c_str());
		vl.at().setB((rvl==-1)?EVAL_BOOL:rvl, 0, true);
		break;
	    }
	    case SHORT:	vl.at().setI(p->owner().smv->getShort(tvar.c_str()), 0, true);	break;
	    case LONG:	vl.at().setI(p->owner().smv->getLong(tvar.c_str()), 0, true);	break;
	    case FLOAT:	vl.at().setR(p->owner().smv->getFloat(tvar.c_str()), 0, true);	break;
	}
	if(ip->owner().messLev() == TMess::Debug) vals += tvar+"='"+vl.at().getS(0,true)+"'; ";
    }
    if(ip->owner().messLev() == TMess::Debug) mess_debug_(ip->nodePath().c_str(), _("SHM Get vals: %s"), vals.c_str());
}

void SHMParam::vlSet( TParamContr *ip, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    TMdPrm *p = (TMdPrm *)ip;

    if(!p->enableStat() || !p->owner().startStat())	vo.setS(EVAL_STR, 0, true);
    if(vl.isEVal() || vl == pvl) return;
    int rez = -2;
    string tvar = vo.fld().descr();
    switch(atoi(vo.fld().reserve().c_str()))
    {
	case BOOL:	rez = p->owner().smv->setBool(tvar.c_str(),vl.getB());	break;
	case SHORT:	rez = p->owner().smv->setShort(tvar.c_str(),vl.getI());	break;
	case LONG:	rez = p->owner().smv->setLong(tvar.c_str(),vl.getI());	break;
	case FLOAT:	rez = p->owner().smv->setFloat(tvar.c_str(),vl.getR());	break;
    }
    if(rez < 0)	mess_err(ip->nodePath().c_str(), _("SHM Set value '%s' to '%s' error."), tvar.c_str(), vl.getS().c_str());
    if(ip->owner().messLev() == TMess::Debug)
	mess_debug_(ip->nodePath().c_str(), _("SHM Set val: %s='%s': %d"), tvar.c_str(), vl.getS().c_str(), rez);
}

bool SHMParam::cntrCmdProc( TParamContr *p, XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	p->ctrMkNode("fld",opt,-1,"/prm/cfg/VAR_LS",p->cfg("VAR_LS").fld().descr(),(p->enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID);
	return true;
    }
    //> Process command to page
    //string a_path = opt->attr("path");
    return false;
}

//****************************************************
//* SMH2Gi::MRCParam - MR and MC bus parameters      *
//****************************************************
struct Inquired_t
{
    union {
	struct {				//IDs
	    unsigned short	hard:10;
	    unsigned short	soft:6;
	} ID;					//used on parameters request
	struct {				// MC alarm codes
	    unsigned short	Reserved1:3;
	    unsigned short	NoTunes:1;	// No tunes get
	    unsigned short	AOutOverload:1;	// Analog output overvoltage
	    unsigned short	Reserved2:11;
	} AlarmsMC;
	struct {				// MR alarm codes
	    unsigned short	Reserved1:3;
	    unsigned short	HighVoltage:1;	// High voltage
	    unsigned short	LowVoltage:1;	// Low voltage
	    unsigned short	AOutOverload:1;	// Analog output overvoltage
	    unsigned short	PSOverload:1;	// Internal power supply overvoltage
	    unsigned short	NoTunes:1;	// No tunes get
	    unsigned short	UnknownType:1;	// Не определен тип МР
	    unsigned short	Reserved2:7;
	} AlarmsMR;				//used on normal processing
    } proc;
    unsigned long	SerialNum:24;		// unique serial number
    unsigned long	Reinit:1;		// МР need reinit
    unsigned long	MCAinsValid:1;		// Values ain0...ain7 valid
    unsigned long	Reserved3:5;		// reserve
    unsigned long	AlarmsInID:1;		// Alarms into struct Alarms
}__attribute__((packed));

MRCParam::MRCParam( ) : TTipParam("MRC", _("MR and MC bus parameters"), "PRM_BD_MRC")
{
    fldAdd(new TFld("MOD_TP",_("Module type"),TFld::Integer,TCfg::NoVal,"10","0"));
    fldAdd(new TFld("MOD_SLOT",_("Module slot/address"),TFld::Integer,TCfg::NoVal,"2","-1","-1;7"));
    fldAdd(new TFld("MOD_PRMS",_("Module addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

void MRCParam::enable( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    if(!p->extPrms) p->extPrms = new tval();
    tval *ePrm = (tval*)p->extPrms;
    ResAlloc res(mod->MRCdevsRes, false);
    ePrm->dev = mod->MRCdevs[p->cfg("MOD_TP").getI()];
    res.release();
    try
    {
	if(!ePrm->dev.HardID)
	    throw TError(p->nodePath().c_str(),TSYS::strMess(_("No selected any module type or type '%d' error."),p->cfg("MOD_TP").getI()).c_str());

	int modSlot = p->cfg("MOD_SLOT").getI();
	string pdu, rezReq;

	//> Prepare and send tune request
	sendTune(ip);

	//> Value's attributes prepare
	ePrm->DI = ePrm->DO = ePrm->AO = ePrm->CNTR = 0;
	for(map<string, DevMRCFeature::SVal>::iterator itv = ePrm->dev.vars.begin(); itv != ePrm->dev.vars.end(); itv++)
	{
	    string p_id = TSYS::TSYS::strEncode(itv->first,TSYS::oscdID);
	    string p_name = itv->second.descr.size() ? itv->second.descr : itv->first;
	    p->p_el.fldAdd(new TFld(p_id.c_str(),p_name.c_str(),itv->second.tp,itv->second.wr?TFld::NoFlag:TFld::NoWrite));
	    p->als.push_back(p_id);
	    //>> Standard types count for MR acquisition unification
	    if(modSlot < 0) continue;
	    if(itv->first.compare(0,3,"aou") == 0) ePrm->AO = vmax(ePrm->AO, atoi(itv->first.c_str()+3)+1);
	    else if(itv->first.compare(0,3,"din") == 0) ePrm->DI = vmax(ePrm->DI, atoi(itv->first.c_str()+3)+1);
	    else if(itv->first.compare(0,3,"dou") == 0) ePrm->DO = vmax(ePrm->DO, atoi(itv->first.c_str()+3)+1);
	    else if(itv->first.compare(0,4,"cntr") == 0) ePrm->CNTR = vmax(ePrm->CNTR, atoi(itv->first.c_str()+4));
	}
	ePrm->diRev = atoi(p->modPrm("DIRev").c_str());
	ePrm->doRev = atoi(p->modPrm("DORev").c_str());

	//> Request HardID, SoftID, SN
	//>> Calculate request block size
	int bSz = sizeof(Inquired_t);
	if(modSlot >= 0) bSz += ePrm->CNTR*4 + (ePrm->DI?2:0);
	pdu = (char)max(0,modSlot);	//Slave address
	pdu += (char)0x03;		//Function, request multiple registers
	pdu += (char)0x00;		//Start address, MSB
	pdu += (char)0x00;		//Start address, LSB
	pdu += (char)((bSz/2)>>8);	//Registers quantity MSB
	pdu += (char)(bSz/2);		//Registers quantity LSB

	rezReq = p->owner().modBusReq(pdu, (modSlot<0));
	if(rezReq.size() || pdu.size() < (3+sizeof(Inquired_t)))
	    throw TError(p->nodePath().c_str(),_("Parameters request error: %s."),rezReq.c_str());
	pdu.erase(0,3);
	//Swap registers
	for(int i_p = 0; i_p < (int)pdu.size()-1; i_p += 2)
	{ char t_sw = pdu[i_p]; pdu[i_p] = pdu[i_p+1]; pdu[i_p+1] = t_sw; }
	Inquired_t *resp = (Inquired_t*)pdu.data();
	//Compare with configured module type
	if(resp->proc.ID.hard != ePrm->dev.HardID)
	    throw TError(p->nodePath().c_str(),_("Configure device %d but set %d."),ePrm->dev.HardID,resp->proc.ID.hard);

	//Save serial number for next process
	ePrm->SoftID = resp->proc.ID.soft;
	ePrm->SN = resp->SerialNum;

    }
    catch(TError err) {
	if(p->extPrms)	{ delete (tval*)p->extPrms; p->extPrms = NULL; }
	throw;
    }
}

void MRCParam::sendTune( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;
    int modSlot = p->cfg("MOD_SLOT").getI();
    string pdu, tune, rezReq;

    for(int i_t = 0; true; i_t++)
    {
	string tits = TSYS::strMess("tune%d",i_t);
	map<string, string>::iterator tit = ePrm->dev.sects["common"].find(tits);
	if(tit == ePrm->dev.sects["common"].end()) break;
	int regVal = atoi(p->modPrm(tits,TSYS::strParse(tit->second,5,"#")).c_str());
	tune += (char)(regVal>>8);	//MSB
    	tune += (char)regVal;		//LSB
    }
    pdu = (char)max(0,modSlot);		//Slave address
    pdu += (char)0x10;			//Function, preset multiple registers
    pdu += (char)0x00;			//Start address, MSB
    pdu += (char)0x00;			//Start address, LSB
    pdu += (char)((tune.size()/2)>>8);	//Registers quantity MSB
    pdu += (char)(tune.size()/2);	//Registers quantity LSB
    pdu += (char)tune.size();		//Byte Count
    pdu += tune;

    rezReq = p->owner().modBusReq(pdu, (modSlot<0));
    if(rezReq.size()) throw TError(p->nodePath().c_str(),_("Send tune request error: %s."),rezReq.c_str());
}

void MRCParam::disable( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;
    if(p->extPrms)	{ delete (tval*)p->extPrms; p->extPrms = NULL; }
}

void MRCParam::getVals( TParamContr *ip )
{
    string pdu, pduReq, data, rezReq;
    TMdPrm *p = (TMdPrm *)ip;
    tval *ePrm = (tval*)p->extPrms;
    int modSlot = p->cfg("MOD_SLOT").getI();

    //>> ID and SN attributes set
    if(p->vlPresent("id")) p->vlAt("id").at().setI(ePrm->dev.HardID, 0, true);
    if(p->vlPresent("sn")) p->vlAt("sn").at().setI(ePrm->SN, 0, true);

    if(modSlot < 0) {	//MC process
	// Read inputs
	pduReq = (char)ePrm->SN;	//SN[0]
	pduReq += (char)0x03;		//Function, read multiple registers
	pduReq += (char)(ePrm->SN>>16);	//SN[2]
	pduReq += (char)(ePrm->SN>>8);	//SN[1]
	pduReq += (char)0x00;		//Registers quantity MSB
	pduReq += (char)0x20;		//Registers quantity LSB, MC structure size 32 registers

	rezReq = p->owner().modBusReq(pduReq, (modSlot<0));
	if(rezReq.size() || pduReq.size() < 35) {
	    pduReq.resize(35);
	    if(rezReq.empty())	rezReq = _("20:PDU short.");
	}
	//else if(pduReq[0]&0x80)	rezReq = _("21:Error respond.");

	pduReq.erase(0,3);
	//> Swap registers
	for(int i_p = 0; i_p < (int)pduReq.size()-1; i_p += 2)
	{ char t_sw = pduReq[i_p]; pduReq[i_p] = pduReq[i_p+1]; pduReq[i_p+1] = t_sw; }

	Inquired_t *respHd = (Inquired_t*)pduReq.data();
	const char *off = pduReq.data() + sizeof(Inquired_t);
	if(respHd->Reinit)	rezReq = _("21:Position lost. Need reload MR/MC bus - re-enable the controller object.");

	//>> AI process
	off += sizeof(float);
	for(int i_a = 0; i_a < 8; i_a++)
	{
	    p->vlAt(TSYS::strMess("ain%d",i_a)).at().setR((rezReq.size() || !respHd->MCAinsValid)?EVAL_REAL:TSYS::getUnalignFloat(off), 0, true);
	    off += sizeof(float);
	}
	off += sizeof(float);
	//>> Counters process
	for(int i_c = 7; i_c <= 8; i_c++)
	{
	    p->vlAt(TSYS::strMess("cntr_din%d_",i_c)).at().setI(rezReq.size()?EVAL_INT:TSYS::getUnalign32(off), 0, true);
	    off += sizeof(uint32_t);
	}
	//>> Frequency process
	for(int i_c = 7; i_c <= 8; i_c++)
	{
	    p->vlAt(TSYS::strMess("freq_din%d_",i_c)).at().setI(rezReq.size()?EVAL_INT:TSYS::getUnalign32(off), 0, true);
	    off += sizeof(uint32_t);
	}
	//>> Digit inputs process
	int vl = TSYS::getUnalign16(off);
	for(int i_d = 0; i_d <= 10; i_d++)
	{
	    if(i_d == 10)	p->vlAt("cr_ack_din8_").at().setB(rezReq.size()?EVAL_BOOL:(vl>>i_d)&1, 0, true);
	    else if(i_d == 9)	p->vlAt("cr_ack_din7_").at().setB(rezReq.size()?EVAL_BOOL:(vl>>i_d)&1, 0, true);
	    else p->vlAt(TSYS::strMess("din%d",i_d)).at().setB(rezReq.size()?EVAL_BOOL:((vl^ePrm->diRev)>>i_d)&1, 0, true);
	}

	//> Send outputs
	if(!rezReq.size())
	{
	    data.clear();
	    //>> MR_broadcast_t.Offsets
	    for(int i_n = 0; i_n < 8; i_n++)
		data += (char)((i_n == vmax(0,modSlot))?8:(8+10));
	    //> Swap registers
	    for(int i_p = 0; i_p < (int)data.size()-1; i_p += 2)
	    { char t_sw = data[i_p]; data[i_p] = data[i_p+1]; data[i_p+1] = t_sw; }

	    //>> MR_broadcast_t.Data
	    //>>> Digital outputs prepare and place
	    vl = 0;
	    for(int i_d = 11; i_d >= 0; i_d--)
	    {
		vl = vl << 1;
		if(i_d == 11)		{ if(p->vlAt("crst_din8_").at().getB(0, true) == true) vl |= 1; }
		else if(i_d == 10)	{ if(p->vlAt("crst_din7_").at().getB(0, true) == true) vl |= 1; }
		else if((p->vlAt(TSYS::strMess("dou%d",i_d)).at().getB(0,true)^(ePrm->doRev>>i_d))&1) vl |= 1;
	    }
	    data += (char)(vl>>8); data += (char)vl;

	    //>>> Analog outputs place
	    for(int i_a = 0; i_a < 4; i_a++)
	    {
		vl = p->vlAt(TSYS::strMess("aou%d",i_a)).at().getI(0, true);
		if(vl == EVAL_INT) vl = 0;
		data += (char)(vl>>8); data += (char)vl;
	    }

	    //>> Prepare broadcast header
	    pdu = (char)0xFE;		//BroadCast
	    pdu += (char)0x10;		//Function, preset multiple registers
	    pdu += (char)0;
	    pdu += (char)0;
	    pdu += (char)((data.size()/2)>>8);	//Registers quantity MSB
	    pdu += (char)(data.size()/2);	//Registers quantity LSB
	    pdu += (char)data.size();	//Byte Count
	    pdu += data;
	    rezReq = p->owner().modBusReq(pdu, (modSlot<0), true);
	}

	//> Alarms process
	if(rezReq.empty() && respHd->AlarmsInID)
	{
	    if(respHd->proc.AlarmsMC.AOutOverload) rezReq += _("Analog output overload; ");
	    if(respHd->proc.AlarmsMC.NoTunes) rezReq += _("No tunes get; ");
	    if(rezReq.size()) rezReq = "21:"+rezReq;
	}
    }
    else		//> MR process
    {
	//> Read inputs
	int reStrSize = sizeof(Inquired_t) + (ePrm->DI?2:0) + ePrm->CNTR*4;
	pduReq = (char)ePrm->SN;		//SN[0]
	pduReq += (char)0x03;			//Function, read multiple registers
	pduReq += (char)(ePrm->SN>>16);		//SN[2]
	pduReq += (char)(ePrm->SN>>8);		//SN[1]
	pduReq += (char)((reStrSize/2)>>8);	//Registers quantity MSB
	pduReq += (char)(reStrSize/2);		//Registers quantity LSB, MC structure size 32 registers
	rezReq = p->owner().modBusReq(pduReq, (modSlot<0));
	if(rezReq.size() || (int)pduReq.size() < (3+reStrSize)) {
	    pduReq.resize(3+reStrSize);
	    if(rezReq.empty()) rezReq = _("20:PDU short.");
	}
	//else if(pduReq[0]&0x80)	rezReq = _("21:Error respond.");

	pduReq.erase(0,3);
	//> Swap registers
	for(int i_p = 0; i_p < (int)pduReq.size()-1; i_p += 2)
	{ char t_sw = pduReq[i_p]; pduReq[i_p] = pduReq[i_p+1]; pduReq[i_p+1] = t_sw; }

	Inquired_t *respHd = (Inquired_t*)pduReq.data();
	const char *off = pduReq.data() + sizeof(Inquired_t);
	if(respHd->Reinit)	rezReq = _("21:Position lost. Need reload MR/MC bus - re-enable the controller object.");

	//>> Counters process
	for(int i_c = 1; i_c <= ePrm->CNTR; i_c++)
	{
	    p->vlAt(TSYS::strMess("cntr%d",i_c)).at().setI(rezReq.size()?EVAL_INT:TSYS::getUnalign32(off), 0, true);
	    off += sizeof(uint32_t);
	}
	//>> Digit inputs process
	int vl = ePrm->DI ? TSYS::getUnalign16(off) : 0;
	for(int i_d = 0; i_d < ePrm->DI; i_d++)
	    p->vlAt(TSYS::strMess("din%d",i_d)).at().setB(rezReq.size()?EVAL_BOOL:((vl^ePrm->diRev)>>i_d)&1, 0, true);

	//> Send outputs
	if(!rezReq.size() && (ePrm->DO || ePrm->AO))
	{
	    data.clear();
	    //>> Digital outputs prepare and place
	    if(ePrm->DO)
	    {
		int vl = 0;
		for(int i_d = (ePrm->DO-1); i_d >= 0; i_d--)
		{
		    vl = vl << 1;
		    if((p->vlAt(TSYS::strMess("dou%d",i_d)).at().getB(0,true)^(ePrm->doRev>>i_d))&1) vl |= 1;
		}
		data += (char)vl; data += (char)(vl>>8);
	    }
	    //>> Analog outputs place
	    for(int i_a = 0; i_a < ePrm->AO; i_a++)
	    {
		vl = p->vlAt(TSYS::strMess("aou%d",i_a)).at().getI(0, true);
		if(vl == EVAL_INT) vl = 0;
		data += (char)vl; data += (char)(vl>>8);
	    }
	    //>> Append to generic MR write frame
	    if(data.size() && p->owner().MRWrFrm[modSlot] == ((modSlot<7)?p->owner().MRWrFrm[modSlot+1]:(int)p->owner().MRWrFrm.size()))
	    {
		if(p->owner().MRWrFrm[modSlot] >= (int)p->owner().MRWrFrm.size()) p->owner().MRWrFrm.append(data);
		else p->owner().MRWrFrm.insert(p->owner().MRWrFrm[modSlot], data);

		for(int i_n = modSlot+1; i_n < 8; i_n++)
		    p->owner().MRWrFrm[i_n] += data.size();
	    }
	}

	//> Alarms process
	if(rezReq.empty() && respHd->AlarmsInID)
	{
	    if(respHd->proc.AlarmsMR.HighVoltage)	rezReq += _("High voltage; ");
	    if(respHd->proc.AlarmsMR.LowVoltage)	rezReq += _("Low voltage; ");
	    if(respHd->proc.AlarmsMR.AOutOverload)	rezReq += _("Analog output overload; ");
	    if(respHd->proc.AlarmsMR.PSOverload)	rezReq += _("Internal power supply overvoltage; ");
	    if(respHd->proc.AlarmsMR.UnknownType)	rezReq += _("Unknown MR module type; ");
	    if(rezReq.size()) rezReq = "21:"+rezReq;
	}
    }

    p->acq_err.setVal(rezReq);
}

void MRCParam::vlSet( TParamContr *ip, TVal &vo, const TVariant &vl, const TVariant &pvl )
{

}

bool MRCParam::cntrCmdProc( TParamContr *ip, XMLNode *opt )
{
    TMdPrm *p = (TMdPrm *)ip;
    ResAlloc res(mod->MRCdevsRes, false);
    DevMRCFeature dMRC = mod->MRCdevs[p->cfg("MOD_TP").getI()];

    //> Get page info
    if(opt->name() == "info")
    {
	p->ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_TP",p->cfg("MOD_TP").fld().descr(),(p->enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID,2,
	    "dest","select","select","/prm/cfg/modLst");
	p->ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_SLOT",p->cfg("MOD_SLOT").fld().descr(),(p->enableStat()?R_R_R_:RWRWR_),"root",SDAQ_ID,1,
	    "help",_("-1  - for MC;\n0-7 - for MR."));
	if(dMRC.HardID && p->ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	{
	    //> Tune configuration
	    for(int i_t = 0, i_ain = 0; true; i_t++)
	    {
		string tits = TSYS::strMess("tune%d",i_t);
		map<string, string>::iterator tit = dMRC.sects["common"].find(tits);
		if(tit == dMRC.sects["common"].end()) break;
		string tp = TSYS::strParse(tit->second,1,"#"),
		    view = TSYS::strParse(tit->second,2,"#"),
		    nm = TSYS::strParse(tit->second,3,"#"),
		    help = TSYS::strParse(tit->second,4,"#"),
		    vMin = TSYS::strParse(tit->second,6,"#"),
		    vMax = TSYS::strParse(tit->second,7,"#");
		if(tp == "int")
		{
		    if(view == "ain")
		    {
			string tpLs, tpVal;
			//> Prepare AIN types list
			for(int i_aTp = 0; true; i_aTp++)
			{
			    tpVal = mod->MRCdevs[-1].sects["Types"][TSYS::strMess("Type%d",i_aTp)];
			    if(!tpVal.size()) break;
			    tpLs += tpVal+";";
			    if(mod->MRCdevs[-1].sects[tpVal][TSYS::strMess("3WIRE%d",i_ain)].size()) tpLs += tpVal+" (3WIRE);";
			    if(mod->MRCdevs[-1].sects[tpVal][TSYS::strMess("4WIRE%d",i_ain)].size()) tpLs += tpVal+" (4WIRE);";
			}
			p->ctrMkNode("fld",opt,-1,("/cfg/"+tits+"-"+TSYS::int2str(i_ain)).c_str(),TSYS::strMess("AI %d",i_ain).c_str(),(!p->enableStat() && view != "sys")?RWRWR_:R_R_R_,
			    "root",SDAQ_ID,4,"tp","str","help",help.c_str(),"dest","select","sel_list",tpLs.c_str());
			i_ain++;
			i_t += 3;
		    }
		    else
			p->ctrMkNode("fld",opt,-1,("/cfg/"+tits).c_str(),nm.c_str(),(!p->enableStat() && view != "sys")?RWRWR_:R_R_R_,
			    "root",SDAQ_ID,4,"tp","dec","min",vMin.c_str(),"max",vMax.c_str(),"help",help.c_str());
		}
		else if(tp == "intbit")
		    for(int i_tb = 0; i_tb < 16; i_tb++)
		    {
			string titbs = TSYS::strMess("tune%d-%d",i_t,i_tb);
			map<string, string>::iterator titb = dMRC.sects["common"].find(titbs);
			if(titb == dMRC.sects["common"].end()) break;
			string bnm = nm+": "+TSYS::strParse(titb->second,1,"#"),
			    bhelp = TSYS::strParse(titb->second,2,"#");
			p->ctrMkNode("fld",opt,-1,("/cfg/"+titbs).c_str(),bnm.c_str(),(!p->enableStat() && view != "sys")?RWRWR_:R_R_R_,
			    "root",SDAQ_ID,2,"tp","bool","help",bhelp.c_str());
		    }
	    }
	    //> Digital signals revers configuration
	    XMLNode *tReq = p->ctrMkNode("area",opt,-1,"/cfg/digRev",_("Digital signals reverse"));
	    for(map<string, DevMRCFeature::SVal>::iterator itv = dMRC.vars.begin(); tReq && itv != dMRC.vars.end(); itv++)
	    {
		int dN = (itv->first.size()>3) ? atoi(itv->first.c_str()+3) : -1;
		if(itv->first.compare(0,3,"din") == 0)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/digRev/DI%d",dN).c_str(),TSYS::strMess(_("DI %d"),dN).c_str(),
			p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		else if(itv->first.compare(0,3,"dou") == 0)
		    p->ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/digRev/DO%d",dN).c_str(),TSYS::strMess(_("DO %d"),dN).c_str(),
			p->enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	    if(tReq && !tReq->childSize()) tReq->parent()->childDel(tReq);
	}
	return true;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/modLst" && p->ctrChkNode(opt))
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("<No select>"));
	for(map<int, DevMRCFeature>::iterator i_d = mod->MRCdevs.begin(); i_d != mod->MRCdevs.end(); i_d++)
	{
	    if(!i_d->second.HardID) continue;
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(i_d->second.HardID))->setText(i_d->second.name);
	}
    }
    else if(a_path.compare(0,9,"/cfg/tune") == 0)
    {
	int i_t = -1, i_tb = -1;
	sscanf(a_path.c_str(),"/cfg/tune%d-%d",&i_t,&i_tb);
	string tits = TSYS::strMess("tune%d",i_t);
	map<string, string>::iterator tit = dMRC.sects["common"].find(tits);
	string tp = TSYS::strParse(tit->second,1,"#"),
	    view = TSYS::strParse(tit->second,2,"#");
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    if(view == "ain") opt->setText(p->modPrm(tits+"val",mod->MRCdevs[-1].sects["Types"]["Type0"]));
	    else
	    {
		opt->setText(TSYS::strParse(tit->second,5,"#"));
		if(view != "sys") opt->setText(p->modPrm(tits,opt->text()));
		if(i_tb >= 0) opt->setText(((atoi(opt->text().c_str())>>i_tb)&1)?"1":"0");
	    }
	}
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR) && view != "sys")
	{
	    if(view == "ain")
	    {
		p->setModPrm(tits+"val",opt->text());
		//>> Set tunes registers from config ain_tunes.ini
		char buf[31];
		int nWr = 2;
		sscanf(opt->text().c_str(),"%30s (%dWIRE)",buf,&nWr);
		string cfgStream = TSYS::strEncode(mod->MRCdevs[-1].sects[buf][TSYS::strMess("%dWIRE%d",nWr,i_tb)], TSYS::Bin);
		if(cfgStream.size() >= 8)
		    for(int i_air = 0; i_air < 4; i_air++)
			p->setModPrm("tune"+TSYS::int2str(i_t+i_air),TSYS::int2str((cfgStream[i_air*2]<<8)|cfgStream[i_air*2+1]));
	    }
	    else if(i_tb < 0) p->setModPrm(tits, opt->text());
	    else
	    {
		int cur_val = atoi(p->modPrm(tits, TSYS::strParse(tit->second,5,"#")).c_str());
		if(atoi(opt->text().c_str())) cur_val |= 1<<i_tb; else cur_val &= ~(1<<i_tb);
		p->setModPrm(tits, TSYS::int2str(cur_val));
	    }
	}
    }
    else if(a_path.compare(0,14,"/cfg/digRev/DI") == 0)
    {
	int dN = atoi(a_path.c_str()+14);
	int dVl = atoi(p->modPrm("DIRev").c_str());
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText((dVl&(1<<dN))?"1":"0");
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    p->setModPrm("DIRev", TSYS::int2str(atoi(opt->text().c_str()) ? (dVl|(1<<dN)) : (dVl & ~(1<<dN))));
    }
    else if(a_path.compare(0,14,"/cfg/digRev/DO") == 0)
    {
	int dN = atoi(a_path.c_str()+14);
	int dVl = atoi(p->modPrm("DORev").c_str());
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) opt->setText((dVl&(1<<dN))?"1":"0");
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    p->setModPrm("DORev", TSYS::int2str(atoi(opt->text().c_str()) ? (dVl|(1<<dN)) : (dVl & ~(1<<dN))));
    }
    else return false;

    return true;
}

//*************************************************
//* SMH2Gi::DevMRCFeature                         *
//*************************************************
DevMRCFeature::DevMRCFeature( const string &iniFile ) : HardID(0)
{
    load(iniFile);
}

bool DevMRCFeature::load( const string &iniFile )
{
    //> Clear all
    HardID = 0;
    name = descr = "";
    sects.clear();
    vars.clear();

    //> Parse file
    char buf[STR_BUF_LEN];
    TRegExp re("^\\s*\\[([^\\]]+)|^\\s*([^\\s;]+)\\s*=\\s*([^\\r\\n]+)"  /* "\\[([^\\]]+)\\]|(\\S+)\\s*=\\s*([^\n\r]+)" */);
    FILE *fp = fopen(iniFile.c_str(),"r");
    if(fp == NULL) return false;
    string iniGroup, iniGroupLC, propNm, propVal;
    map<string, SVal>::iterator cVar;
    while(fgets(buf,sizeof(buf),fp) != NULL)
    {
	TArrayObj *reRez = re.match(string(buf,sizeof(buf)));
	//> Group change
	if(reRez->size() == 2)
	{
	    iniGroup = reRez->propGet("1").getS();
	    iniGroupLC.resize(iniGroup.length());
	    std::transform(iniGroup.begin(), iniGroup.end(), iniGroupLC.begin(), ::tolower);
	}
	//> Property process
	else if(reRez->size() == 4 && iniGroup.size())
	{
	    propNm = reRez->propGet("2").getS(); propVal = Mess->codeConvIn("cp1251",reRez->propGet("3").getS());
	    sects[iniGroup][propNm] = propVal;
	    if(iniGroupLC == "common")
	    {
		if(propNm == "name") 		name = propVal;
		else if(propNm == "descr")	descr = propVal;
		else if(propNm == "ID")		HardID = atoi(propVal.c_str());
		else if(propNm.compare(0,3,"var") == 0)
		{
		    std::transform(propVal.begin(), propVal.end(), propVal.begin(), ::tolower);
		    vars[propVal] = SVal();
		}
	    }
	    //>> Variables parsing
	    else if((cVar=vars.find(iniGroupLC)) != vars.end())
	    {
		if(propNm == "descr" /*|| propNm == "dop_descr"*/)	cVar->second.descr = propVal;
		else if(propNm == "addr") cVar->second.addr = atoi(propVal.c_str());
		else if(propNm == "type")
		{
		    cVar->second.tp = TFld::Integer;
		    if(propVal == "real")	cVar->second.tp = TFld::Real;
		    else if(propVal == "bool")	cVar->second.tp = TFld::Boolean;
		}
		else if(propNm == "dir")	cVar->second.wr = (propVal=="w");
		else if(propNm == "visible")	cVar->second.visible = propVal;
	    }
	}
	delete reRez;
    }

    fclose(fp);

    return true;
}
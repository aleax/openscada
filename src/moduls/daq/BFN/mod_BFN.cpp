
//OpenSCADA system module DAQ.BFN file: mod_BFN.cpp
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

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "mod_BFN.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"BFN"
#define MOD_NAME	_("BFN module")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define VERSION		"0.0.1"
#define AUTORS		_("Roman Savochenko")
#define DESCRIPTION	_("BFN modules support for Viper CT/BAS from \"Big Dutchman\".")
#define LICENSE		"GPL2"
//*************************************************

ModBFN::TTpContr *ModBFN::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module(int n_mod)
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach(const TModule::SAt &AtMod, const string &source)
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))	return new ModBFN::TTpContr(source);
	return NULL;
    }
}

using namespace ModBFN;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr(string name) : TTipDAQ(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTpContr::~TTpContr( )
{

}

void TTpContr::load_( )
{

}

void TTpContr::save_( )
{

}

void TTpContr::postEnable(int flag)
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Calc schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000"));
    fldAdd(new TFld("ADDR",_("Transport address"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("USER",_("User"),TFld::String,TFld::NoFlag,"50",""));
    fldAdd(new TFld("PASS",_("Password"),TFld::String,TFld::NoFlag,"30",""));
    //fldAdd(new TFld("HOUSE",_("House"),TFld::String,TFld::NoFlag,"50",""));

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    //tpPrmAt(t_prm).fldAdd( new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100","") );
}

TController *TTpContr::ContrAttach(const string &name, const string &daq_db)
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0),
	mSched(cfg("SCHEDULE").getSd()), mPrior(cfg("PRIOR").getId()), mSync(cfg("SYNCPER").getRd()),
	mAddr(cfg("ADDR").getSd()), /*mHouse(cfg("HOUSE").getSd()),*/ mUser(cfg("USER").getSd()), mPassword(cfg("PASS").getSd())
{
    cfg("PRM_BD").setS("TmplPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) rez += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    return rez;
}

TParamContr *TMdContr::ParamAttach(const string &name, int type)
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    //> Get Houses
    XMLNode reqHouses("GetHouses");
    reqBFN(reqHouses);
    if(reqHouses.attr("err").empty())
    {
	XMLNode *houseArr = reqHouses.childGet("arrHouseProperties");
	for(int i_h = 0; i_h < houseArr->childSize(); i_h++)
	{
	    //> Get House computers
	    XMLNode reqHouseComps("GetHouseComputers");
	    reqHouseComps.childAdd("lHouseId")->setText(houseArr->childGet(i_h)->childGet("lHouseId")->text());
	    reqBFN(reqHouseComps);
	    if(reqHouseComps.attr("err").empty())
	    {
		XMLNode *compArr = reqHouseComps.childGet("arrHouseComputerProperties");
		for(int i_hc = 0; i_hc < compArr->childSize(); i_hc++)
		{
		    //> Get Code Data
		    XMLNode reqCodeData("GetCodeData");
		    reqCodeData.childAdd("lHouseComputerId")->setText(compArr->childGet(i_hc)->childGet("lHouseComputerId")->text());
		    reqBFN(reqCodeData);
		    if(reqCodeData.attr("err").empty())
		    {
			string pName = houseArr->childGet(i_h)->childGet("lHouseId")->text()+"_"+compArr->childGet(i_hc)->childGet("lHouseComputerId")->text();
			AutoHD<TMdPrm> prm;
			if(!present(pName)) add(pName,owner().tpPrmToId("std"));
			prm = at(pName); prm.at().setToEnable(true);

			XMLNode *cdArr = reqCodeData.childGet("arrCodeData");
			for(int i_cd = 0; i_cd < cdArr->childSize(); i_cd++)
			{
			    if(!prm.at().vlPresent(cdArr->childGet(i_cd)->childGet("lCodeId")->text()))
				prm.at().p_el.fldAdd(new TFld(cdArr->childGet(i_cd)->childGet("lCodeId")->text().c_str(),
							    cdArr->childGet(i_cd)->childGet("lCodeId")->text().c_str(),TFld::String,TFld::NoWrite));
			    prm.at().vlAt(cdArr->childGet(i_cd)->childGet("lCodeId")->text()).at().setS(cdArr->childGet(i_cd)->childGet("szCodeValue")->text(),0,true);
			}
		    }
		}
	    }
	}
    }
}

void TMdContr::start_( )
{
    //> Schedule process
    mPer = TSYS::strSepParse(mSched,1,' ').empty() ? vmax(0,(long long)(1e9*atof(mSched.c_str()))) : 0;

    //> Start the gathering data task
    if(!prc_st) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this, &prc_st);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.',true), &prc_st, &endrun_req);
}

void TMdContr::prmEn(const string &id, bool val)
{
    int i_prm;

    ResAlloc res(en_res,true);
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void TMdContr::reqBFN(XMLNode &io)
{
    ResAlloc res(nodeRes(), true);

    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(mAddr,0,'.')).at().outAt(TSYS::strSepParse(mAddr,1,'.'));

    XMLNode req("POST");
    req.setAttr("URI","/cgi-bin/imwl_ws.cgi");
    req.childAdd("prm")->setAttr("id","Content-Type")->setText("text/xml; charset=\"UTF-8\"");
    req.childAdd("prm")->setAttr("id","Connection")->setText("Keep-Alive");
    req.childAdd("prm")->setAttr("id","Cache-Control")->setText("no-cache");
    req.childAdd("prm")->setAttr("id","Pragma")->setText("no-cache");

    //> Prepare request
    //>> Prepare typical SOAP wrapper
    string dataReq = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
	"<SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" "
	"xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" "
	"xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" "
	"xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">"
	"<SOAP-ENV:Body SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";
    //>> Append SOAP data to IO
    string reqName = io.name();
    io.setName("SOAPSDK4:"+reqName)->setAttr("xmlns:SOAPSDK4","http://tempuri.org");
    io.childAdd("szUserName")->setText(mUser);
    io.childAdd("szPassword")->setText(mPassword);
    dataReq += io.save();
    dataReq += "</SOAP-ENV:Body></SOAP-ENV:Envelope>";
    //> Append full request
    req.setText(dataReq);
    //> Send request
    tr.at().messProtIO(req,"HTTP");
    io.setAttr("err",req.attr("err"));
    if(req.attr("err").empty())
    {
	XMLNode rez;
	rez.load(req.text());
	//printf("TEST 00\n%s\n",req.text().c_str());
	string rCod = rez.childGet("SOAP-ENV:Body")->childGet("imwlws:"+reqName+"Response")->childGet("res")->text();
	if(atoi(rCod.c_str())) io.setAttr("err",rCod);
	else 
	{
	    io = *rez.childGet("SOAP-ENV:Body")->childGet("imwlws:"+reqName+"Response");
	    io.setName(reqName+"Response");
	}
    }
}

void *TMdContr::Task(void *icntr)
{
    string aId;
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while(!cntr.endrun_req)
    {
	long long t_cnt = TSYS::curTime();

	//> Update controller's data
	cntr.en_res.resRequestR();
	for(unsigned i_p=0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++)
	    try
	    {
		//> Get current data
		XMLNode reqCodeData("GetCodeData");
		reqCodeData.childAdd("lHouseComputerId")->setText(TSYS::strParse(cntr.p_hd[i_p].at().id(),1,"_"));
		cntr.reqBFN(reqCodeData);
		if(reqCodeData.attr("err").empty())
		{
		    XMLNode *cdArr = reqCodeData.childGet("arrCodeData");
		    for(int i_cd = 0; i_cd < cdArr->childSize(); i_cd++)
		    {
			aId = cdArr->childGet(i_cd)->childGet("lCodeId")->text();
			if(!cntr.p_hd[i_p].at().vlPresent(aId))	continue;
			cntr.p_hd[i_p].at().vlAt(aId).at().setS(cdArr->childGet(i_cd)->childGet("szCodeValue")->text(),0,true);
		    }
		}
		//> Get Alarms
		XMLNode reqAlrms("GetAlarmLogDataFromLogIndex");
		reqAlrms.childAdd("lHouseComputerId")->setText(TSYS::strParse(cntr.p_hd[i_p].at().id(),1,"_"));
		reqAlrms.childAdd("lLastLogIndexFetched")->setText(TSYS::int2str(cntr.p_hd[i_p].at().curAlrmsId));
		cntr.reqBFN(reqAlrms);
		if(reqCodeData.attr("err").empty())
		{
		    cntr.p_hd[i_p].at().curAlrmsId = atoi(reqAlrms.childGet("lLastLogIndexFetched")->text().c_str());
		    printf("TEST 00: Alarms: %d : %d\n",cntr.p_hd[i_p].at().curAlrmsId,reqAlrms.childGet("arrAlarmLogData")->childSize());
		}
	    }
	    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resRelease();
	cntr.tm_gath = 1e-3*(TSYS::curTime()-t_cnt);

	TSYS::taskSleep(cntr.period(),cntr.period()?0:TSYS::cron(cntr.cron()));
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),RWRWR_,"root",SDAQ_ID,4,"tp","str","dest","sel_ed",
	    "sel_list","1;1e-3;* * * * *;10 * * * *;10-20 2 */2 * *",
	    "help",_("Schedule is writed in seconds periodic form or in standard Cron form.\n"
		"Seconds form is one real number (1.5, 1e-3).\n"
		"Cron it is standard form '* * * * *'. Where:\n"
		"  - minutes (0-59);\n"
		"  - hours (0-23);\n"
		"  - days (1-31);\n"
		"  - month (1-12);\n"
		"  - week day (0[sunday]-6)."));
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","select","/cntr/cfg/trLst");
	//ctrMkNode("fld",opt,-1,"/cntr/cfg/HOUSE",cfg("HOUSE").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","sel_ed","select","/cntr/cfg/houseLst");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/PASS" && ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(string(mPassword.size(),'*'));
    else if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt))
    {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(int i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    /*else if(a_path == "/cntr/cfg/houseLst" && ctrChkNode(opt))
    {
	if(!enableStat()) return;
	XMLNode req("SOAP-ENV:Envelope");
	req.setAttr("xmlns:SOAPSDK1","http://www.w3.org/2001/XMLSchema")->
	    setAttr("xmlns:SOAPSDK2","http://www.w3.org/2001/XMLSchema-instance")->
	    setAttr("xmlns:SOAPSDK3","http://schemas.xmlsoap.org/soap/encoding/")->
	    setAttr("xmlns:SOAP-ENV","http://schemas.xmlsoap.org/soap/envelope/");
	XMLNode *reqAuth = req.childAdd("SOAP-ENV:Body")->setAttr("SOAP-ENV:encodingStyle","http://schemas.xmlsoap.org/soap/encoding/")->
	    childAdd("SOAPSDK4:GetHouses")->setAttr("xmlns:SOAPSDK4","http://tempuri.org");
	reqAuth->childAdd("szUserName")->setText(mUser);
	reqAuth->childAdd("szPassword")->setText(mPassword);
	reqBFN(req);
	if(!atoi(req.attr("err").c_str()) && 
	    !atoi(req.childGet("SOAP-ENV:Body")->childGet("imwlws:GetHousesResponse")->childGet("res")->text().c_str()))
	{
	    XMLNode *housArr = req.childGet("SOAP-ENV:Body")->childGet("imwlws:GetHousesResponse")->childGet("arrHouseProperties");
	    for(int i_h = 0; i_h < housArr->childSize(); i_h++)
		opt->childAdd("el")->setText(housArr->childGet(i_h)->childGet("lHouseId")->text()+": "+
						housArr->childGet(i_h)->childGet("szHouseName")->text());
	}
    }*/
    else TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm(string name, TTipParam *tp_prm) :
    TParamContr(name,tp_prm), p_el("w_attr"), curAlrmsId(0)
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable(int flag)
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    owner().prmEn(id(), true);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

void TMdPrm::cntrCmdProc(XMLNode *opt)
{
    //> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	//ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",cfg("OID_LS").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }

    //> Process command to page
    /*if(a_path == "/prm/cfg/OID_LS" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
    {
	if(enableStat())	throw TError(nodePath().c_str(),"Parameter is enabled.");
//	parseOIDList(opt->text());
    }
    else*/
    TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

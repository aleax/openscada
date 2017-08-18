
//OpenSCADA system module DAQ.BFN file: mod_BFN.cpp
/***************************************************************************
 *   Copyright (C) 2010-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <ttypeparam.h>
#include <tdaqs.h>

#include "mod_BFN.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"BFN"
#define MOD_NAME	_("BFN module")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.6.8"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Big Farm Net (BFN) modules support for Viper CT/BAS and other from \"Big Dutchman\" (http://www.bigdutchman.com).")
#define LICENSE		"GPL2"
//*************************************************

ModBFN::TTpContr *ModBFN::mod;  //Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))	return new ModBFN::TTpContr(source);
	return NULL;
    }
}

using namespace ModBFN;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr(string name) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr( )
{

}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Controler's bd structure
    //fldAdd(new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000"));
    fldAdd(new TFld("ADDR",_("Transport address"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("USER",_("User"),TFld::String,TFld::NoFlag,"50",""));
    fldAdd(new TFld("PASS",_("Password"),TFld::String,TFld::NoFlag,"30",""));
    //fldAdd(new TFld("HOUSE",_("House"),TFld::String,TFld::NoFlag,"50",""));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std","",_("Standard"));
    //tpPrmAt(t_prm).fldAdd( new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100","") );
    // Set to read only
    for(unsigned i_sz = 0; i_sz < tpPrmAt(t_prm).fldSize(); i_sz++)
	tpPrmAt(t_prm).fldAt(i_sz).setFlg(tpPrmAt(t_prm).fldAt(i_sz).flg()|TFld::NoWrite);

    //Make Symbols of codes container structure
    symbCode_el.fldAdd(new TFld("ID","ID",TFld::Integer,TCfg::Key));
    symbCode_el.fldAdd(new TFld("TEXT","Text",TFld::String,TFld::TransltText,"100"));

    //Make Symbols of alarms container structure
    symbAlrm_el.fldAdd(new TFld("ID","ID",TFld::Integer,TCfg::Key));
    symbAlrm_el.fldAdd(new TFld("CODE","Code",TFld::Integer,TFld::NoFlag));
    symbAlrm_el.fldAdd(new TFld("TEXT","Text",TFld::String,TFld::TransltText,"100"));
}

string TTpContr::symbDB( )	{ return TBDS::genDBGet(nodePath()+"symbDB","*.*"); }

void TTpContr::setSymbDB( const string &idb )
{
    TBDS::genDBSet(nodePath()+"symbDB",idb);
    modif();
}

string TTpContr::getSymbolCode(const string &id)
{
    MtxAlloc res(dataRes(), true);
    map<unsigned,string>::iterator is = mSymbCode.find(s2i(id));
    if(is == mSymbCode.end()) return TSYS::strMess(_("Code %s"),id.c_str());
    return is->second;
}

TTpContr::AlrmSymb TTpContr::getSymbolAlarm(const string &id)
{
    MtxAlloc res(dataRes(), true);
    map<unsigned,AlrmSymb>::iterator is = mSymbAlrm.find(s2i(id));
    if(is == mSymbAlrm.end()) return AlrmSymb();
    return is->second;
}

void TTpContr::load_( )
{
    //Load Code symbols
    string wtbl = MOD_ID"_SymbCode";
    string wdb  = symbDB();
    TConfig c_el(&symbCode_el);
    MtxAlloc res(dataRes(), true);
    mSymbCode.clear();
    for(int fld_cnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,nodePath()+wtbl,fld_cnt,c_el); fld_cnt++)
	mSymbCode[c_el.cfg("ID").getI()] = c_el.cfg("TEXT").getS();

    //Load Alarm symbols
    wtbl = MOD_ID"_SymbAlarm";
    c_el.setElem(&symbAlrm_el);
    mSymbAlrm.clear();
    for(int fld_cnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,nodePath()+wtbl,fld_cnt,c_el); fld_cnt++)
	mSymbAlrm[c_el.cfg("ID").getI()] = AlrmSymb(c_el.cfg("TEXT").getS(),c_el.cfg("CODE").getI());
}

void TTpContr::save_( )
{
    //Save Code symbols
    string wtbl = MOD_ID"_SymbCode";
    string wdb  = symbDB();
    TConfig c_el(&symbCode_el);
    MtxAlloc res(dataRes(), true);
    for(map<unsigned,string>::iterator is = mSymbCode.begin(); is != mSymbCode.end(); is++) {
	c_el.cfg("ID").setI(is->first);
	c_el.cfg("TEXT").setS(is->second);
	SYS->db().at().dataSet(wdb+"."+wtbl, nodePath()+wtbl, c_el);
    }
    // Clear no present codes
    for(int fld_cnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,nodePath()+wtbl,fld_cnt,c_el); fld_cnt++) {
	if(mSymbCode.find(c_el.cfg("ID").getI()) != mSymbCode.end()) continue;
	SYS->db().at().dataDel(wdb+"."+wtbl, nodePath()+wtbl, c_el, true, false, true);
	fld_cnt--;
    }

    //Save Alarm symbols
    wtbl = MOD_ID"_SymbAlarm";
    c_el.setElem(&symbAlrm_el);
    for(map<unsigned,AlrmSymb>::iterator is = mSymbAlrm.begin(); is != mSymbAlrm.end(); is++) {
	c_el.cfg("ID").setI(is->first);
	c_el.cfg("CODE").setI(is->second.code);
	c_el.cfg("TEXT").setS(is->second.text);
	SYS->db().at().dataSet(wdb+"."+wtbl, nodePath()+wtbl, c_el);
    }
    // Clear no present codes
    for(int fld_cnt = 0; SYS->db().at().dataSeek(wdb+"."+wtbl,nodePath()+wtbl,fld_cnt,c_el); fld_cnt++) {
	if(mSymbAlrm.find(c_el.cfg("ID").getI()) != mSymbAlrm.end()) continue;
	SYS->db().at().dataDel(wdb+"."+wtbl, nodePath()+wtbl, c_el, true, false, true);
	fld_cnt--;
    }
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTypeDAQ::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/symbs",_("Symbols"))) {
	    ctrMkNode("fld",opt,-1,"/symbs/db",_("Symbols DB"),RWRWR_,"root",SDAQ_ID,4,
		"tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    if(ctrMkNode("table",opt,-1,"/symbs/codes",_("Codes"),RWRWR_,"root",SDAQ_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/symbs/codes/id",_("Id"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/symbs/codes/text",_("Text"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	    if(ctrMkNode("table",opt,-1,"/symbs/alrms",_("Alarms"),RWRWR_,"root",SDAQ_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/symbs/alrms/id",_("Id"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/symbs/alrms/code",_("Code"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/symbs/alrms/text",_("Text"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/symbs/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(symbDB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSymbDB(opt->text());
    }
    else if(a_path == "/symbs/codes") {
	MtxAlloc res(dataRes(), true);
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/symbs/codes/id","");
	    XMLNode *n_text	= ctrMkNode("list",opt,-1,"/symbs/codes/text","");

	    for(map<unsigned,string>::iterator is = mSymbCode.begin(); is != mSymbCode.end(); is++) {
		if(n_id)	n_id->childAdd("el")->setText(u2s(is->first));
		if(n_text)	n_text->childAdd("el")->setText(is->second);
	    }
	    return;
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(!mSymbCode.size()) mSymbCode[1] = _("New symbol for code");
	    else mSymbCode[mSymbCode.rbegin()->first+1] = _("New symbol for code");
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    mSymbCode.erase(s2i(opt->attr("key_id")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(opt->attr("col") == "id") {
		mSymbCode[s2i(opt->text())] = mSymbCode[s2i(opt->attr("key_id"))];
		mSymbCode.erase(s2i(opt->attr("key_id")));
	    }
	    else if(opt->attr("col") == "text")	mSymbCode[s2i(opt->attr("key_id"))] = opt->text();
	}
	modif();
    }
    else if(a_path == "/symbs/alrms") {
	MtxAlloc res(dataRes(), true);
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    XMLNode *n_id	= ctrMkNode("list",opt,-1,"/symbs/alrms/id","");
	    XMLNode *n_code	= ctrMkNode("list",opt,-1,"/symbs/alrms/code","");
	    XMLNode *n_text	= ctrMkNode("list",opt,-1,"/symbs/alrms/text","");

	    for(map<unsigned,AlrmSymb>::iterator is = mSymbAlrm.begin(); is != mSymbAlrm.end(); is++) {
		if(n_id)	n_id->childAdd("el")->setText(u2s(is->first));
		if(n_code)	n_code->childAdd("el")->setText(u2s(is->second.code));
		if(n_text)	n_text->childAdd("el")->setText(is->second.text);
	    }
	    return;
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(!mSymbAlrm.size()) mSymbAlrm[0] = AlrmSymb(_("New symbol for alarm"),0);
	    else mSymbAlrm[mSymbAlrm.rbegin()->first+1] = AlrmSymb(_("New symbol for alarm"),0);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    mSymbAlrm.erase(s2i(opt->attr("key_id")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(opt->attr("col") == "id") {
		mSymbAlrm[s2i(opt->text())] = mSymbAlrm[s2i(opt->attr("key_id"))];
		mSymbAlrm.erase(s2i(opt->attr("key_id")));
	    }
	    else if(opt->attr("col") == "code")	mSymbAlrm[s2i(opt->attr("key_id"))].code = s2i(opt->text());
	    else if(opt->attr("col") == "text")	mSymbAlrm[s2i(opt->attr("key_id"))].text = opt->text();
	}
	modif();
    }
    else TTypeDAQ::cntrCmdProc(opt);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, ::TElem *cfgelem) : ::TController(name_c,daq_db,cfgelem),
	mPrior(cfg("PRIOR").getId()), mSync(cfg("SYNCPER").getRd()),
	prc_st(false), acq_st(false), endrun_req(false), alSt(-1), tm_gath(0)
{
    //cfg("PRM_BD").setS("TmplPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	//Display error
	if(!acq_err.getVal().empty()) {
	    rez += TSYS::strMess(_("Error: '%s'. "),acq_err.getVal().c_str());
	    rez.replace(0,1,"10");
	}
	//Display processing
	if(acq_st) rez += TSYS::strMess(_("Call now. "));
	//Display schedule
	if(period()) rez += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-9*period()).c_str());
	else rez += TSYS::strMess(_("Call next by cron '%s'. "),atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	//Display spent time
	if(acq_err.getVal().empty()) rez += TSYS::strMess(_("Spent time: %s."),tm2s(1e-6*tm_gath).c_str());
    }

    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )	{ return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::enable_( )
{
    try {
	//Get Houses
	XMLNode reqHouses("GetHouses");
	reqBFN(reqHouses);
	if(reqHouses.attr("err").empty()) {
	    XMLNode *houseArr = reqHouses.childGet("arrHouseProperties");
	    for(unsigned i_h = 0; i_h < houseArr->childSize(); i_h++) {
		XMLNode *houseIt = houseArr->childGet(i_h);
		//Get House computers
		XMLNode reqHouseComps("GetHouseComputers");
		reqHouseComps.childAdd("lHouseId")->setText(houseIt->childGet("lHouseId")->text());
		reqBFN(reqHouseComps);
		if(reqHouseComps.attr("err").empty()) {
		    XMLNode *compArr = reqHouseComps.childGet("arrHouseComputerProperties");
		    for(unsigned i_hc = 0; i_hc < compArr->childSize(); i_hc++) {
			XMLNode *compIt = compArr->childGet(i_hc);
			//Get Code Data
			XMLNode reqCodeData("GetCodeData");
			reqCodeData.childAdd("lHouseComputerId")->setText(compIt->childGet("lHouseComputerId")->text());
			reqBFN(reqCodeData);
			if(reqCodeData.attr("err").empty()) {
			    string pName = "h"+houseIt->childGet("lHouseId")->text()+"_hc"+compIt->childGet("lHouseComputerId")->text();
			    AutoHD<TMdPrm> prm;
			    if(!present(pName)) {
				add(pName,owner().tpPrmToId("std"));
				prm = at(pName);
				prm.at().setName(houseIt->childGet("szHouseName")->text()+" ("+compIt->childGet("szComputerNameShort")->text()+")");
				string descr = _("House:\n");
				for(unsigned i_hi = 0; i_hi < houseIt->childSize(); i_hi++)
				    descr += "  "+passPrefSOAP(houseIt->childGet(i_hi)->name())+": "+houseIt->childGet(i_hi)->text()+"\n";
				descr += _("House computer:\n");
				for(unsigned i_hci = 0; i_hci < compIt->childSize(); i_hci++)
				    descr += "  "+passPrefSOAP(compIt->childGet(i_hci)->name())+": "+compIt->childGet(i_hci)->text()+"\n";
				prm.at().setDescr(descr);
			    }
			    else prm = at(pName);

			    XMLNode *cdArr = reqCodeData.childGet("arrCodeData");
			    for(unsigned i_cd = 0; i_cd < cdArr->childSize(); i_cd++) {
				XMLNode *cdIt = +cdArr->childGet(i_cd);
				string cdId = "c"+cdIt->childGet("lCodeId")->text()+"u"+cdIt->childGet("iUnitId")->text();
				if(!prm.at().vlPresent(cdId)) {
				    TFld::Type cTp = TFld::String;
				    switch(s2i(cdIt->childGet("iDataType")->text())) {
					case 0: cTp = TFld::Real;			break;
					case 2: case 3: case 6: cTp = TFld::Integer;	break;
				    }
				    prm.at().p_el.fldAdd(new TFld(cdId.c_str(),cdId.c_str(),cTp,TFld::NoWrite));
				}
				prm.at().vlAt(cdId).at().fld().setDescr(mod->getSymbolCode(cdIt->childGet("lCodeId")->text()));
				//prm.at().vlAt(cdId).at().setS(cdIt->childGet("szCodeValue")->text(),0,true);
			    }
			}
		    }
		}
	    }
	}
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void TMdContr::start_( )
{
    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    //Start the gathering data task
    if(!prc_st) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.',true), &endrun_req);

    alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),owner().modId().c_str(),id().c_str(),_("STOP")), TMess::Info);
    alSt = -1;

    //Clear errors and set EVal
    ResAlloc res(en_res,false);
    for(unsigned i_p=0; i_p < p_hd.size(); i_p++) {
	p_hd[i_p].at().acq_err.setVal("");
	p_hd[i_p].at().setEval();
    }
    acq_err.setVal("");
    tm_gath = 0;
}

void TMdContr::prmEn(const string &id, bool val)
{
    ResAlloc res(en_res,true);

    unsigned i_prm;
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void TMdContr::reqBFN(XMLNode &io)
{
    ResAlloc res(req_res, true);

    AutoHD<TTransportOut> tr;
    try{ tr = SYS->transport().at().at(TSYS::strSepParse(addr(),0,'.')).at().outAt(TSYS::strSepParse(addr(),1,'.')); }
    catch(TError &err) { throw TError(nodePath().c_str(),_("Connect to transport '%s' error."),addr().c_str()); }

    XMLNode req("POST");
    req.setAttr("URI","/cgi-bin/imwl_ws.cgi");
    req.childAdd("prm")->setAttr("id","Content-Type")->setText("text/xml; charset=\"UTF-8\"");
    req.childAdd("prm")->setAttr("id","Connection")->setText("Keep-Alive");
    req.childAdd("prm")->setAttr("id","Cache-Control")->setText("no-cache");
    req.childAdd("prm")->setAttr("id","Pragma")->setText("no-cache");

    //Prepare request
    // Prepare typical SOAP wrapper
    string dataReq = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
	"<SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" "
	"xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" "
	"xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" "
	"xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">"
	"<SOAP-ENV:Body SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";
    // Append SOAP data to IO
    string reqName = io.name();
    io.setName("SOAPSDK4:"+reqName)->setAttr("xmlns:SOAPSDK4","http://tempuri.org");
    io.childAdd("szUserName")->setText(cfg("USER").getS());
    io.childAdd("szPassword")->setText(cfg("PASS").getS());
    dataReq += io.save();
    dataReq += "</SOAP-ENV:Body></SOAP-ENV:Envelope>";
    //Append full request
    req.setText(dataReq);
    //Send request
    tr.at().messProtIO(req,"HTTP");
    io.setAttr("err",req.attr("err"));
    if(req.attr("err").empty()) {
	XMLNode rez;
	try { rez.load(req.text()); }
	catch(TError &err) { throw TError(nodePath().c_str(),_("Respond parsing error. Possible respond incomplete.")); }
	string rCod = rez.childGet("SOAP-ENV:Body")->childGet("imwlws:"+reqName+"Response")->childGet("res")->text();
	if(s2i(rCod)) io.setAttr("err",rCod);
	else {
	    io = *rez.childGet("SOAP-ENV:Body")->childGet("imwlws:"+reqName+"Response");
	    io.setName(reqName+"Response");
	}
    }
}

string TMdContr::passPrefSOAP( const string &ndName )
{
    unsigned i_c = 0;
    while(i_c < ndName.size() && islower(ndName[i_c])) i_c++;

    if(i_c < ndName.size()) return ndName.substr(i_c);
    return ndName;
}

void *TMdContr::Task( void *icntr )
{
    string aId, tErr;
    int64_t t_cnt = 0, s_cnt = 0;
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while(!cntr.endrun_req) {
	try {
	    //Host's computers list update
	    tErr = "";
	    cntr.acq_st = true;
	    if((1e-6*(TSYS::curTime()-s_cnt)) >= cntr.syncPer()) {
		XMLNode reqHouses("GetHouses");
		cntr.reqBFN(reqHouses);
		if(reqHouses.attr("err").empty()) {
		    XMLNode *houseArr = reqHouses.childGet("arrHouseProperties");
		    for(unsigned i_h = 0; !cntr.endrun_req && i_h < houseArr->childSize(); i_h++) {
			XMLNode *houseIt = houseArr->childGet(i_h);
			//Get House computers
			XMLNode reqHouseComps("GetHouseComputers");
			reqHouseComps.childAdd("lHouseId")->setText(houseIt->childGet("lHouseId")->text());
			cntr.reqBFN(reqHouseComps);
			if(reqHouseComps.attr("err").empty()) {
			    XMLNode *compArr = reqHouseComps.childGet("arrHouseComputerProperties");
			    for(unsigned i_hc = 0; !cntr.endrun_req && i_hc < compArr->childSize(); i_hc++) {
				XMLNode *compIt = compArr->childGet(i_hc);
				string pName = "h"+houseIt->childGet("lHouseId")->text()+"_hc"+compIt->childGet("lHouseComputerId")->text();
				if(cntr.present(pName)) continue;
				cntr.add(pName,cntr.owner().tpPrmToId("std"));
				AutoHD<TMdPrm> prm = cntr.at(pName);
				prm.at().setName(houseIt->childGet("szHouseName")->text()+" ("+compIt->childGet("szComputerNameShort")->text()+")");
				string descr = _("House:\n");
				for(unsigned i_hi = 0; i_hi < houseIt->childSize(); i_hi++)
				    descr += "  "+cntr.passPrefSOAP(houseIt->childGet(i_hi)->name())+": "+houseIt->childGet(i_hi)->text()+"\n";
				descr += _("House computer:\n");
				for(unsigned i_hci = 0; i_hci < compIt->childSize(); i_hci++)
				    descr += "  "+cntr.passPrefSOAP(compIt->childGet(i_hci)->name())+": "+compIt->childGet(i_hci)->text()+"\n";
				prm.at().setDescr(descr);
				prm.at().enable();
			    }
			}
		    }
		    s_cnt = TSYS::curTime();
		}
		else tErr = reqHouses.attr("err");
	    }
	    t_cnt = TSYS::curTime();

	    //Update controller's data
	    ResAlloc res(cntr.en_res,false);
	    for(unsigned i_p=0; i_p < cntr.p_hd.size() && !cntr.redntUse() && !cntr.endrun_req; i_p++) {
		//Get current data
		XMLNode reqCodeData("GetCodeData");
		reqCodeData.childAdd("lHouseComputerId")->setText(TSYS::strParse(cntr.p_hd[i_p].at().id(),1,"_hc"));
		cntr.reqBFN(reqCodeData);
		if(reqCodeData.attr("err").empty()) {
		    XMLNode *cdArr = reqCodeData.childGet("arrCodeData");
		    for(unsigned i_cd = 0; !cntr.endrun_req && i_cd < cdArr->childSize(); i_cd++) {
			XMLNode *cdIt = cdArr->childGet(i_cd);
			aId = "c"+cdIt->childGet("lCodeId")->text()+"u"+cdIt->childGet("iUnitId")->text();
			if(!cntr.p_hd[i_p].at().vlPresent(aId)) {
			    TFld::Type cTp = TFld::String;
			    switch(s2i(cdIt->childGet("iDataType")->text())) {
				case 0: cTp = TFld::Real;	break;
				case 2: case 3: case 6: cTp = TFld::Integer;break;
			    }
			    cntr.p_hd[i_p].at().p_el.fldAdd(new TFld(aId.c_str(),mod->getSymbolCode(cdIt->childGet("lCodeId")->text()).c_str(),cTp,TFld::NoWrite));
			}
			cntr.p_hd[i_p].at().vlAt(aId).at().setS(cdIt->childGet("szCodeValue")->text(),0/*s2i(cdIt->childGet("lLastUpdate")->text())*/,true);
		    }
		}
		else {
		    if(tErr.empty()) tErr = reqCodeData.attr("err");
		    //Set attributes to Eval
		    if(cntr.p_hd[i_p].at().acq_err.getVal().empty()) cntr.p_hd[i_p].at().setEval();
		}
		//Get Alarms
		XMLNode reqAlrms("GetAlarmLogDataFromLogIndex");
		reqAlrms.childAdd("lHouseComputerId")->setText(TSYS::strParse(cntr.p_hd[i_p].at().id(),1,"_hc"));
		reqAlrms.childAdd("lLastLogIndexFetched")->setText(i2s(cntr.p_hd[i_p].at().curAlrmsId));
		cntr.reqBFN(reqAlrms);
		if(reqAlrms.attr("err").empty()) {
		    cntr.p_hd[i_p].at().curAlrmsId = s2i(reqAlrms.childGet("lLastLogIndexFetched")->text());
		    XMLNode *alrmArr = reqAlrms.childGet("arrAlarmLogData");
		    for(unsigned i_a = 0; !cntr.endrun_req && i_a < alrmArr->childSize(); i_a++) {
			XMLNode *alrmIt = alrmArr->childGet(i_a);
			time_t aTm = s2i(alrmIt->childGet("lTimestamp")->text());
			string aId = alrmIt->childGet("lAlarmId")->text();
			int aEv = s2i(alrmIt->childGet("lAlarmEvent")->text());
			string aVl = alrmIt->childGet("szValue")->text();
			TTpContr::AlrmSymb aNd = mod->getSymbolAlarm(aId);
			if(aNd.text.empty()) mess_warning(cntr.nodePath().c_str(),_("Unknown alarm '%s'."),aId.c_str());
			else {
			    string mcat = TSYS::strMess("alBFN:%s.%s.%d:%s",cntr.id().c_str(),cntr.p_hd[i_p].at().id().c_str(),aNd.code,aId.c_str());
			    string mval = cntr.p_hd[i_p].at().name()+" > "+
				    (aNd.code?mod->getSymbolCode(i2s(aNd.code)):string(_("Main")))+": "+
				    aNd.text;
			    if(aEv == 0)	SYS->archive().at().messPut(aTm, 0, mcat, -TMess::Error, _("Alarm: ")+mval);
			    //else if(aEv == 1)	SYS->archive().at().messPut(aTm, 0, mcat, -TMess::Warning, _("Confirm: ")+mval);
			    else if(aEv == 2)	SYS->archive().at().messPut(aTm, 0, mcat, TMess::Info, _("Norma: ")+mval);
			}
		    }
		}
		else if(tErr.empty()) tErr = reqAlrms.attr("err");
		cntr.p_hd[i_p].at().acq_err.setVal(tErr);
	    }
	} catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); tErr = err.mess; }

	//Generic alarm generate
	if(tErr.size() && cntr.alSt <= 0) {
	    cntr.alSt = 1;
	    cntr.alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),cntr.owner().modId().c_str(),cntr.id().c_str(),
						TRegExp(":","g").replace(tErr,"=").c_str()));
	}
	else if(!tErr.size() && cntr.alSt != 0) {
	    cntr.alSt = 0;
	    cntr.alarmSet(TSYS::strMess(_("DAQ.%s.%s: connect to data source: %s."),cntr.owner().modId().c_str(),cntr.id().c_str(),_("OK")),
			    TMess::Info);
	}
	cntr.acq_err.setVal(tErr);

	cntr.tm_gath = TSYS::curTime()-t_cnt;
	cntr.acq_st = false;
	TSYS::taskSleep(cntr.period(), cntr.period() ? "" : cntr.cron());
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","select","/cntr/cfg/trLst");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/USER",cfg("USER").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PASS",cfg("PASS").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	//ctrMkNode("fld",opt,-1,"/cntr/cfg/HOUSE",cfg("HOUSE").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","sel_ed","select","/cntr/cfg/houseLst");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/PASS" && ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	opt->setText(string(cfg("PASS").getS().size(),'*'));
    else if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt)) {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
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
	reqAuth->childAdd("szUserName")->setText(cfg("USER").getS());
	reqAuth->childAdd("szPassword")->setText(cfg("PASS").getS());
	reqBFN(req);
	if(!s2i(req.attr("err")) &&
	    !s2i(req.childGet("SOAP-ENV:Body")->childGet("imwlws:GetHousesResponse")->childGet("res")->text()))
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
TMdPrm::TMdPrm(string name, TTypeParam *tp_prm) : TParamContr(name,tp_prm), curAlrmsId(0), p_el("w_attr")
{
    setToEnable(true);
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable(int flag)
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::setEval( )
{
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    owner().prmEn(id(), true);
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    setEval();
    acq_err.setVal("");
}

void TMdPrm::load_( )
{
    //TParamContr::load_();
}

void TMdPrm::save_( )
{
    //TParamContr::save_();
}

void TMdPrm::vlGet( TVal &val )
{
    if(val.name() != "err")	return;

    if(!enableStat() || !owner().startStat()) {
	if(!enableStat())		val.setS(_("1:Parameter is disabled."),0,true);
	else if(!owner().startStat())	val.setS(_("2:Acquisition is stopped."),0,true);
	return;
    }
    if(owner().redntUse())	return;

    if(!acq_err.getVal().empty())		val.setS("11:"+acq_err.getVal(),0,true);
    else if(!owner().acq_err.getVal().empty())	val.setS("10:"+owner().acq_err.getVal(),0,true);
    else val.setS("0",0,true);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(false);
}

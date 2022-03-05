
//OpenSCADA module DAQ.DCON file: DCON_client.cpp
/***************************************************************************
 *   Copyright (C) 2008-2011 by Almaz Karimov                              *
 *		   2008-2022 by Roman Savochenko, roman@oscada.org         *
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <tsys.h>
#include <ttypeparam.h>

#include "DCON_client.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DCON"
#define MOD_NAME	trS("DCON client")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"1.3.11"
#define AUTHORS		trS("Roman Savochenko, Almaz Karimov")
#define DESCRIPTION	trS("Provides an implementation of DCON-client protocol. Supports I-7000 DCON protocol.")
#define LICENSE		"GPL2"
//*************************************************

DCONDAQ::TTpContr *DCONDAQ::mod;  //Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_DCON_module(int n_mod)
#else
    TModule::SAt module(int n_mod)
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_DCON_attach(const TModule::SAt &AtMod, const string &source)
#else
    TModule *attach(const TModule::SAt &AtMod, const string &source)
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new DCONDAQ::TTpContr(source);
	return NULL;
    }
}

using namespace DCONDAQ;

//******************************************************
//* TTpContr                                           *
//******************************************************
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
    TTypeDAQ::postEnable( flag );

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",trS("Parameters table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",trS("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",trS("Priority of the acquisition task"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));
    fldAdd(new TFld("ADDR",trS("Transport"),TFld::String,TFld::NoFlag,"41",""));
    fldAdd(new TFld("REQ_TRY",trS("Request tries"),TFld::Integer,TFld::NoFlag,"1","1","1;10"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("MOD_ADDR",trS("Module address"),TFld::Integer,TFld::NoFlag|TCfg::NoVal,"20","1","0;255"));
    tpPrmAt(t_prm).fldAdd(new TFld("CRC_CTRL",trS("CRC control"),TFld::Boolean,TFld::NoFlag|TCfg::NoVal,"1","1"));
    tpPrmAt(t_prm).fldAdd(new TFld("HOST_SIGNAL",trS("Host signal"),TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;1",_("No signal;HostOK")));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_METHOD",trS("AI method"),TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;1;2;3;4;6;8;10;20;116;201;202;203;204;206;208;210;220;316",
		_("No AI;1AI (#AA);2AI (#AA);3AI (#AA);4AI (#AA);6AI (#AA);8AI (#AA);10AI (#AA);20AI (#AA);16AI (#AA^AA);1AI (#AAN);2AI (#AAN);3AI (#AAN);4AI (#AAN);6AI (#AAN);8AI (#AAN);10AI (#AANN);20AI (#AANN);16AI (#AAN^AAN)")));
    tpPrmAt(t_prm).fldAdd(new TFld("AI_RANGE",""/*_("AI range")*/,TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;1;2",_("Engineer or percent;Hexadecimal (0000 FFFF);Hexadecimal (8000 7FFF)")));
    tpPrmAt(t_prm).fldAdd(new TFld("AO_METHOD",trS("AO method"),TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;1;2;4;14",_("No AO;1AO (#AA);2AO (#AAN);4AO (#AAN);4AO (#AACN)")));
    tpPrmAt(t_prm).fldAdd(new TFld("AO_RANGE",""/*_("AO range")*/,TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;1;2;3;4;5;6;7;8;9;10",
		_("Engineer (00.000 20.000);Engineer (04.000 20.000);Engineer (00.000 10.000);Engineer (+00.000 +20.000);"
		  "Engineer (+04.000 +20.000);Engineer (+00.000 +10.000);Engineer (-10.000 +10.000);Engineer (+00.000 +05.000);"
		  "Engineer (-05.000 +05.000);Percent (+000.00 +100.00);Hexadecimal (000 FFF)")));
    tpPrmAt(t_prm).fldAdd(new TFld("DI_METHOD",trS("DI method"),TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;3;4;7;8;14;16;101;201",
		_("No DI;3DI ($AA6);4DI ($AA6);7DI ($AA6);8DI ($AA6);14DI ($AA6);16DI ($AA6);1DI (@AADI);8DI ($AA6,FF00)")) );
    tpPrmAt(t_prm).fldAdd(new TFld("DO_METHOD",trS("DO method"),TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;2;3;4;5;7;8;12;13;16;102;103;202;204;306;402;504;604;608;704;708;712)",
		_("No DO;2DO (@AA,0300);3DO (@AA,7);4DO (@AA,F);5DO (@AA,1F);7DO (@AA,7F);8DO (@AA,FF);12DO (@AA,0FFF);"
		  "13DO (@AA,1FFF);16DO (@AA,FFFF);2DO (^AADOVVV);3DO (^AADOVVV);2DO (@AADO);4DO (@AADO);6DO (@AADODD);"
		  "2DO (@AADO0D);4DO (@(^)AADO0D);4DO (@AA,0F00);8DO (@AA,FF00);4DO (#AA000F);8DO (#AA00FF);12DO (#AA000FFF)")));
    tpPrmAt(t_prm).fldAdd(new TFld("CI_METHOD",trS("CI method"),TFld::Integer,TFld::Selectable|TCfg::NoVal,"1","0",
		"0;2;3",_("No CI;2CI (#AA);3CI (#AA)")));
}

void TTpContr::load_( )
{
    //Load parameters from command line
}

void TTpContr::save_( )
{

}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db ) { return new TMdContr(name,daq_db,this); }


//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
    TController(name_c, daq_db, cfgelem), enRes(true), reqRes(true),
    mAddr(cfg("ADDR")), mPrior(cfg("PRIOR").getId()), connTry(cfg("REQ_TRY").getId()),
    prcSt(false), callSt(false), endrunReq(false), mPer(1e9), tmGath(0)
{
    cfg("PRM_BD").setS("DCONPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();

    if(startStat() && !redntUse()) {
	if(callSt)	rez += TSYS::strMess(_("Acquisition. "));
	if(period())	rez += TSYS::strMess(_("Acquisition with the period: %s. "),tm2s(1e-9*period()).c_str());
	else rez += TSYS::strMess(_("Next acquisition by the cron '%s'. "),atm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	rez += TSYS::strMess(_("Spent time: %s. "),tm2s(1e-6*tmGath).c_str());
    }

    return rez;
}


TParamContr *TMdContr::ParamAttach( const string &name, int type ) { return new TMdPrm(name, &owner().tpPrmAt(type)); }

void TMdContr::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    //TController::load_( );
}

void TMdContr::disable_( )
{

}

void TMdContr::start_( )
{
    if(prcSt)	return;

    //Fix old address format
    if(addr().size() && TSYS::strParse(addr(),1,".").empty())	mAddr = "Serial."+addr();

    //Establish connection
    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(addr(),0,'.')).at().outAt(TSYS::strSepParse(addr(),1,'.'));
    try { tr.at().start(); }
    catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    if(prcSt) SYS->taskDestroy(nodePath('.',true), &endrunReq);
}



void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
	if(&pHd[i_prm].at() == prm) break;

    if(val && i_prm >= pHd.size())	pHd.push_back(prm);
    if(!val && i_prm < pHd.size())	pHd.erase(pHd.begin()+i_prm);
}

string TMdContr::DCONCRC( string str )
{
    unsigned char CRC = 0;
    for(unsigned i = 0; i < str.size(); i++) CRC += (unsigned char)str[i];
    return TSYS::strMess("%02X",CRC);
}

string TMdContr::DCONReq( string &pdu, bool CRC, unsigned acqLen, char resOK )
{
    MtxAlloc res(reqRes, true);
    char buf[1000];
    string rez, err;

    try {
	AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(addr(),0,'.')).at().outAt(TSYS::strSepParse(addr(),1,'.'));
	if(!tr.at().startStat()) tr.at().start();
	if(CRC) pdu += DCONCRC(pdu);
	if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("REQ -> '%s'"), pdu.c_str());
	pdu += "\r";

	MtxAlloc resN(tr.at().reqRes(), true);

	for(int i_tr = 0, resp_len = 0; i_tr < vmax(1,vmin(10,connTry)); i_tr++) {
	    try {
		resp_len = tr.at().messIO(pdu.data(), pdu.size(), buf, sizeof(buf));
		rez.assign(buf,resp_len);

		//Wait tail
		while(resp_len && (rez.size() < 2 || rez[rez.size()-1] != '\r')) {
		    try{ resp_len = tr.at().messIO(NULL, 0, buf, sizeof(buf)); } catch(TError &er) { break; }
		    rez.append(buf, resp_len);
		}
	    } catch(TError &er) {	//By possible the send request breakdown and no response
		if(err.empty()) err = _("10:Transport error: ") + er.mess;
		else if(err.find(er.mess) == string::npos) err += "; " + er.mess;
		continue;
	    }
	    if(rez.size() < 2 || rez[rez.size()-1] != '\r') { err = _("13:Error respond: Not full."); continue; }
	    pdu = rez.substr(0,rez.size()-1);
	    if(!pdu.size() || (CRC && pdu.size() < 3)) { err = _("20:Respond length error."); break; }
	    if(CRC && strtol(pdu.substr(pdu.size()-2).c_str(),NULL,16) != strtol(DCONCRC(pdu.substr(0,pdu.size()-2)).c_str(),NULL,16))
	    { err = _("21:Invalid module CRC."); continue; }
	    if(acqLen) {
		if(pdu[0] != resOK) { err = _("22:Invalid module response."); continue; }
		else if(((!CRC && acqLen != pdu.size()) || (CRC && (acqLen+2) != pdu.size()))) { err = _("20:Respond length error."); break; }
	    }

	    if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("RESP -> '%s'"), pdu.c_str());
	    err = "0";
	    break;
	}
    } catch(TError &er) { err = _("10:Transport error: ")+er.mess; }

    if(err != "0") {
	if(messLev() == TMess::Debug) mess_debug_(nodePath().c_str(), _("ERR -> '%s': %s"), pdu.c_str(), err.c_str());
	pdu = "";
    }

    return err;
}

void *TMdContr::Task( void *icntr )
{
    string str, pdu, ai_txterr, ao_txterr, di_txterr, do_txterr, ci_txterr;
    int n, m, i;
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrunReq = false;
    cntr.prcSt = true;

    bool isStart = true;
    bool isStop  = false;

    try {
	while(true) {
	    if(!isStart && !isStop && !cntr.redntUse()) {
		cntr.callSt = true;
		int64_t t_cnt = TSYS::curTime();

		//Update controller's data
		MtxAlloc res(cntr.enRes, true);
		for(unsigned i_p = 0; i_p < cntr.pHd.size(); i_p++) {
		    if(cntr.endrunReq) break;
		    //Reset errors
		    ai_txterr = ao_txterr = di_txterr = do_txterr = ci_txterr = "0";

		    //AI
		    n = 0;
		    switch(cntr.pHd[i_p].at().aiMethod) {
			case 1: case 2: case 3: case 4: case 6: case 8: case 10: case 20:
			    n = cntr.pHd[i_p].at().aiMethod;
			case 116: if(!n) n = 8;
			    //#AA
			    //Request with module
			    pdu = TSYS::strMess("#%02X",(int)cntr.pHd[i_p].at().modAddr);
			    m = (cntr.pHd[i_p].at().aiRange == 0) ? 7 : 4;
			    ai_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,(m*n+1));
			    for(i = 0; i < n; i++) {
				if(ai_txterr != "0") { cntr.pHd[i_p].at().AI[i] = EVAL_REAL; continue; }
				if(m == 7) cntr.pHd[i_p].at().AI[i] = s2r(pdu.substr(i*m+1,m));
				if(m == 4) cntr.pHd[i_p].at().AI[i] = strtol(pdu.substr(i*m+1,m).c_str(),NULL,16);
				if(cntr.pHd[i_p].at().aiRange == 2 && cntr.pHd[i_p].at().AI[i] > 0x7fff)
				    cntr.pHd[i_p].at().AI[i]=cntr.pHd[i_p].at().AI[i]-0x10000;
			    }

			    if(cntr.pHd[i_p].at().aiMethod == 116) {
				//^AA
				//Request with module
				pdu = TSYS::strMess("^%02X",(int)cntr.pHd[i_p].at().modAddr);
				m = (cntr.pHd[i_p].at().aiRange == 0) ? 7 : 4;
				ai_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1);
				for(i = 0; i < n; i++) {
				    if(ai_txterr != "0") { cntr.pHd[i_p].at().AI[i] = EVAL_REAL; continue; }
				    if(m == 7) cntr.pHd[i_p].at().AI[i+8] = s2r(pdu.substr(i*m+1,m));
				    if(m == 4) cntr.pHd[i_p].at().AI[i+8] = strtol(pdu.substr(i*m+1,m).c_str(),NULL,16);
				    if(cntr.pHd[i_p].at().aiRange == 2 && cntr.pHd[i_p].at().AI[i+8] > 0x7fff)
					cntr.pHd[i_p].at().AI[i+8] = cntr.pHd[i_p].at().AI[i+8]-0x10000;
				}
			    }
			    break;
			case 201: case 202: case 203: case 204: case 206: case 208: case 210: case 220:
			    n = cntr.pHd[i_p].at().aiMethod - 200;
			case 316: if(!n) n = 8;
			    //#AAN
			    //Request with module
			    m = (cntr.pHd[i_p].at().aiRange == 0) ? 7 : 4;
			    for(i = 0; i < n; i++) {
				if((cntr.pHd[i_p].at().aiMethod == 210)||(cntr.pHd[i_p].at().aiMethod == 220))
				    pdu = TSYS::strMess("#%02X%02X",(int)cntr.pHd[i_p].at().modAddr,i);
				else pdu = TSYS::strMess("#%02X%d",(int)cntr.pHd[i_p].at().modAddr,i);
				ai_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,(m+1));
				if(ai_txterr != "0") cntr.pHd[i_p].at().AI[i] = EVAL_REAL;
				else {
				    if(m == 7) cntr.pHd[i_p].at().AI[i] = s2r(pdu.substr(1,m));
				    if(m == 4) cntr.pHd[i_p].at().AI[i] = strtol(pdu.substr(1,m).c_str(),NULL,16);
				    if(cntr.pHd[i_p].at().aiRange == 2 && cntr.pHd[i_p].at().AI[i] > 0x7fff)
					cntr.pHd[i_p].at().AI[i]=cntr.pHd[i_p].at().AI[i]-0x10000;
				}
			    }
			    if(cntr.pHd[i_p].at().aiMethod == 316) {
				//^AAN
				//Request with module
				m = (cntr.pHd[i_p].at().aiRange == 0) ? 7 : 4;
				for(i = 0; i < n; i++) {
				    pdu = TSYS::strMess("^%02X%d",(int)cntr.pHd[i_p].at().modAddr,i);
				    ai_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1);
				    if(ai_txterr != "0") cntr.pHd[i_p].at().AI[i] = EVAL_REAL;
				    else {
					if(m == 7) cntr.pHd[i_p].at().AI[i+8] = s2r(pdu.substr(1,m));
					if(m == 4) cntr.pHd[i_p].at().AI[i+8] = strtol(pdu.substr(1,m).c_str(),NULL,16);
					if(cntr.pHd[i_p].at().aiRange == 2 && cntr.pHd[i_p].at().AI[i+8] > 0x7fff)
					    cntr.pHd[i_p].at().AI[i+8] = cntr.pHd[i_p].at().AI[i+8]-0x10000;
				    }
				}
			    }
			    break;
		    }

		    //AO
		    switch(cntr.pHd[i_p].at().aoMethod) {
			case 1: case 2: case 4: case 14:
			    n = cntr.pHd[i_p].at().aoMethod;
			    if(n == 14) n = 4;
			    for(i = 0; i < n; i++) {
				//Control range
				switch(cntr.pHd[i_p].at().aoRange) {
				    case 0://Engineer (00.000 20.000)
					str = TSYS::strMess("%06.3f",vmax(0,vmin(20,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 1://Engineer (04.000 20.000)
					str = TSYS::strMess("%06.3f",vmax(4,vmin(20,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 2://Engineer (00.000 10.000)
					str = TSYS::strMess("%06.3f",vmax(0,vmin(10,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 3://Engineer (+00.000 +20.000)
					str = TSYS::strMess("%+07.3f",vmax(0,vmin(20,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 4://Engineer (+04.000 +20.000)
					str = TSYS::strMess("%+07.3f",vmax(4,vmin(20,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 5://Engineer (+00.000 +10.000)
					str = TSYS::strMess("%+07.3f",vmax(0,vmin(10,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 6://Engineer (-10.000 +10.000)
					str = TSYS::strMess("%+07.3f",vmax(-10,vmin(10,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 7://Engineer (+00.000 +05.000)
					str = TSYS::strMess("%+07.3f",vmax(0,vmin(5,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 8://Engineer (-05.000 +05.000)
					str = TSYS::strMess("%+07.3f",vmax(-5,vmin(5,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 9://Percent (+000.00 +100.00)
					str = TSYS::strMess("%+07.2f",vmax(0,vmin(100,cntr.pHd[i_p].at().AO[i])));
					break;
				    case 10://Hexadecimal (000 FFF)
					str = TSYS::strMess("%03X",(int)vmax(0,vmin(0xFFF,cntr.pHd[i_p].at().AO[i])));
					break;
				    default:    ao_txterr = _("23:Undefined range");    break;
				}
				//Request with module
				if(cntr.pHd[i_p].at().aoMethod == 1) pdu = TSYS::strMess("#%02X%s",(int)cntr.pHd[i_p].at().modAddr,str.c_str());
				else if (cntr.pHd[i_p].at().aoMethod == 14) pdu = TSYS::strMess("#%02XC%d%s",(int)cntr.pHd[i_p].at().modAddr,i,str.c_str());
				else pdu = TSYS::strMess("#%02X%d%s",(int)cntr.pHd[i_p].at().modAddr,i,str.c_str());
				if((ao_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
				else if(pdu.size() && pdu[0] == '?') ao_txterr = _("24:Module out of range");
				else if(pdu.size() && pdu[0] == '!') ao_txterr = _("25:Command ignored (host watchdog).");
			    }
		    }

		    //DI
		    unsigned int DI = 0;
		    n = 0;
		    switch(cntr.pHd[i_p].at().diMethod) {
			case 3: if(!n) n = 3;	//3DI ($AA6)
			case 4: if(!n) n = 4;	//4DI ($AA6)
			case 7: if(!n) n = 7;	//7DI ($AA6)
			case 8: if(!n) n = 8;	//8DI ($AA6)
			case 14:if(!n) n = 14;	//14DI ($AA6)
			case 16:if(!n) n = 16;	//16DI ($AA6)
			case 201:if(!n) n = 8;	//8DI ($AA6,FF00)
			    //Request with module
			    pdu = TSYS::strMess("$%02X6",(int)cntr.pHd[i_p].at().modAddr);
			    di_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,7,'!');
			    if(di_txterr == "0") DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
			    if(cntr.pHd[i_p].at().diMethod == 201) DI = DI>>8;
			    for(int i_n = 0; i_n < n; i_n++)
				cntr.pHd[i_p].at().DI[i_n] = (di_txterr == "0") ? ((DI>>i_n)&0x01) : EVAL_BOOL;
			    break;
			case 101:		//1DI (@AADI)
			    //Request with module
			    pdu = TSYS::strMess("@%02XDI",(int)cntr.pHd[i_p].at().modAddr);
			    di_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,7,'!');
			    cntr.pHd[i_p].at().DI[0] = (di_txterr == "0") ? (char)strtoul(pdu.substr(6,2).c_str(),NULL,16) : EVAL_BOOL;
			    break;
		    }

		    //DO
		    unsigned code = 0;
		    n = 0;
		    pdu = "";
		    switch(cntr.pHd[i_p].at().doMethod) {
			case 2:		//2DO (@AA,0300)
			    //Request with module
			    code = (cntr.pHd[i_p].at().DO[1]?2:0)+(cntr.pHd[i_p].at().DO[0]?1:0);
			    pdu = TSYS::strMess("@%02X%02X00",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 604:	//4DO (@AA,0F00)
			    for(unsigned i_n = 0; i_n < 4; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%02X00",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 608:	//8DO (@AA,FF00)
			    for(unsigned i_n = 0; i_n < 8; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%02X00",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 3:		//3DO (@AA,7)
			    for(unsigned i_n = 0; i_n < 3; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%01X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 4:		//4DO (@AA,F)
			    for(unsigned i_n = 0; i_n < 4; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%01X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 5:		//5DO (@AA,1F)
			    for(unsigned i_n = 0; i_n < 5; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 7:		//7DO (@AA,7F)
			    for(unsigned i_n = 0; i_n < 7; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 8:		//8DO (@AA,FF)
			    for(unsigned i_n = 0; i_n < 8; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 12:	//12DO (@AA,0FFF)
			    for(unsigned i_n = 0; i_n < 12; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%04X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 13:	//13DO (@AA,1FFF)
			    for(unsigned i_n = 0; i_n < 13; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%04X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 16:	//16DO (@AA,FFFF)
			    for(unsigned i_n = 0; i_n < 16; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02X%04X",(int)cntr.pHd[i_p].at().modAddr,code);
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 102:	//2DO (^AADOVVV)
			    //Request with module
			    pdu = TSYS::strMess("^%02XDO",(int)cntr.pHd[i_p].at().modAddr);
			    pdu += cntr.pHd[i_p].at().DO[0] ? "1" : "0";
			    pdu += cntr.pHd[i_p].at().DO[1] ? "1" : "0";
			    pdu += "0";
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 103:	//3DO (^AADOVVV)
			    pdu = TSYS::strMess("^%02XDO",(int)cntr.pHd[i_p].at().modAddr);
			    pdu += cntr.pHd[i_p].at().DO[0] ? "1" : "0";
			    pdu += cntr.pHd[i_p].at().DO[1] ? "1" : "0";
			    pdu += cntr.pHd[i_p].at().DO[2] ? "1" : "0";
			    if((do_txterr=cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1)) == "0") ;
			    else if(pdu.size() && pdu[0] == '!') do_txterr = _("25:Command ignored (host watchdog).");
			    break;
			case 202:	//2DO (@AADO)
			    n = 2;
			case 204:	//4DO (@AADO)
			    if(!n) n = 4;
			    for(int i_r = 0; i_r < n/2; i_r++) {
				//Request with module
				for(unsigned i_n = 0; i_n < 2; i_n++) if(cntr.pHd[i_p].at().DO[i_r*2+i_n]) code += (1<<i_n);
				pdu = TSYS::strMess("@%02XDO%01X%01X",(int)cntr.pHd[i_p].at().modAddr,i_r,code);
				do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,3,'!');
			    }
			    break;
			case 306:	//6DO (@AADODD)
			    //Request with module
			    for(unsigned i_n = 0; i_n < 6; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02XDO%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,3,'!');
			    break;
			case 402:	//2DO (@AADO0D)
			    for(unsigned i_n = 0; i_n < 2; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("@%02XDO%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,3,'!');
			    break;
			case 504:	//4DO (@(^)AADO0D)
			    //Request with module
			    code = (cntr.pHd[i_p].at().DO[1]?2:0)+(cntr.pHd[i_p].at().DO[0]?1:0);
			    pdu = TSYS::strMess("@%02XDO%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,3,'!');
			    //Request with module
			    code = (cntr.pHd[i_p].at().DO[3]?2:0)+(cntr.pHd[i_p].at().DO[2]?1:0);
			    pdu = TSYS::strMess("^%02XDO%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,3,'!');
			    break;
			case 704:	//4DO (#AA000F)
			    for(unsigned i_n = 0; i_n < 4; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("#%02X00%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1);
			    break;
			case 708:	//8DO (#AA00FF)
			    for(unsigned i_n = 0; i_n < 8; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("#%02X00%02X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1);
			    break;
			case 712:	//12DO (#AA000FFF)
			    for(unsigned i_n = 0; i_n < 12; i_n++) if(cntr.pHd[i_p].at().DO[i_n]) code += (1<<i_n);
			    pdu = TSYS::strMess("#%02X00%04X",(int)cntr.pHd[i_p].at().modAddr,code);
			    do_txterr = cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl,1);
			    break;
		    }

		    //CI
		    n = 0;
		    switch(cntr.pHd[i_p].at().ciMethod) {
			case 2://2CI (#AA)
			    n = 2;
			case 3://3CI (#AA)
			    if(!n) n = 3;
			    for(int i_n = 0; i_n < n; i_n++) {
				//> Request with module
				pdu = TSYS::strMess("#%02X%d",(int)cntr.pHd[i_p].at().modAddr,i_n);
				if(cntr.pHd[i_p].at().crcCtrl) pdu += cntr.DCONCRC(pdu);
				if((ci_txterr=cntr.DCONReq(pdu,false,cntr.pHd[i_p].at().crcCtrl?11:9)) == "0") {
				    if(cntr.pHd[i_p].at().crcCtrl) {
					pdu = pdu.substr(0,1) + pdu.substr(3,8) + pdu.substr(1,2);
					if(strtol(pdu.substr(9,2).c_str(),NULL,16) != strtol(cntr.DCONCRC(pdu.substr(0,9)).c_str(),NULL,16))
					{ ci_txterr=_("21:Invalid module CRC."); continue; }
				    }
				    cntr.pHd[i_p].at().CI[i_n] = strtol(pdu.substr(1,8).c_str(),NULL,16);
				}
			    }
			    break;
		    }

		    //Host signal
		    switch(cntr.pHd[i_p].at().hostSignal) {
			case 1: {	//HostOK signal
			    pdu = "~**";
			    cntr.DCONReq(pdu,cntr.pHd[i_p].at().crcCtrl);
			    break;
			}
		    }
		    cntr.pHd[i_p].at().aiErr.setVal(ai_txterr);
		    cntr.pHd[i_p].at().aoErr.setVal(ao_txterr);
		    cntr.pHd[i_p].at().diErr.setVal(di_txterr);
		    cntr.pHd[i_p].at().doErr.setVal(do_txterr);
		    cntr.pHd[i_p].at().ciErr.setVal(ci_txterr);
		}
		res.unlock();

		//Calc acquisition process time
		cntr.tmGath = TSYS::curTime()-t_cnt;
		cntr.callSt = false;
	    }

	    if(isStop) break;

	    TSYS::taskSleep((int64_t)cntr.period(), cntr.period() ? "" : cntr.cron());

	    if(cntr.endrunReq) isStop = true;
	    isStart = false;
	}
    } catch(TError &err) { mess_err(err.cat.c_str(), err.mess.c_str()); }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",mAddr.fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "tp","str","dest","select","select","/cntr/cfg/trLst");
	ctrRemoveNode(opt,"/cntr/cfg/PERIOD");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel().c_str(),"help",TMess::labSecCRON().c_str());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior().c_str());
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/trLst" && ctrChkNode(opt)) {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else TController::cntrCmdProc(opt);
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(co.fld().name() == "SCHEDULE")
	mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,1e9*s2r(cron())) : 0;

    return true;
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) :
    TParamContr(name, tp_prm), pEl("w_attr"), modAddr(cfg("MOD_ADDR").getId()), crcCtrl(cfg("CRC_CTRL").getBd()),
    hostSignal(cfg("HOST_SIGNAL").getId()), aiMethod(cfg("AI_METHOD").getId()), aiRange(cfg("AI_RANGE").getId()), aoMethod(cfg("AO_METHOD").getId()),
    aoRange(cfg("AO_RANGE").getId()), diMethod(cfg("DI_METHOD").getId()), doMethod(cfg("DO_METHOD").getId()), ciMethod(cfg("CI_METHOD").getId())
{
    aiErr.setVal("0");
    aoErr.setVal("0");
    diErr.setVal("0");
    doErr.setVal("0");
    ciErr.setVal("0");
    //Default input/output data
    for(int i = 0; i < 32; i++) {
	AI[i] = AO[i] = CI[i] = 0;
	DI[i] = DO[i] = false;
    }
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll( );
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&pEl))	vlElemAtt(&pEl);
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if(enableStat())	return;

    TParamContr::enable();

    //Delete DAQ parameter's attributes
    for(unsigned i_f = 0; i_f < pEl.fldSize(); ) {
	try { pEl.fldDel(i_f); continue; }
	catch(TError &err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
	i_f++;
    }

    //Make DAQ parameter's attributes
    unsigned itCnt = 0;
    // AI
    switch(aiMethod) {
	case 1: case 201: itCnt = 1;	break;	//1AI (#AA,#AAN)
	case 2: case 202: itCnt = 2;	break;	//2AI (#AA,#AAN)
	case 3: case 203: itCnt = 3;	break;	//3AI (#AA,#AAN)
	case 4: case 204: itCnt = 4;	break;	//4AI (#AA,#AAN)
	case 6: case 206: itCnt = 6;	break;	//6AI (#AA,#AAN)
	case 8: case 208: itCnt = 8;	break;	//8AI (#AA,#AAN)
	case 10: case 210:itCnt = 10;	break;	//10AI (#AA,#AANN)
	case 20: case 220:itCnt = 20;	break;	//20AI (#AA,#AANN)
	case 16: case 316:itCnt = 16;	break;	//16AI (#AA^AA,#AAN^AAN)
	default:	  itCnt = 0;	break;
    }
    if(itCnt) {
	string saiMethod = i2s(aiMethod);
	pEl.fldAdd(new TFld("aiErr","aiErr",TFld::String,TFld::NoWrite|TVal::DirRead,"","","","",saiMethod.c_str()));
	for(unsigned i_p = 0; i_p < itCnt; i_p++)
	    pEl.fldAdd(new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
		    TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",saiMethod.c_str()));
    }

    // AO
    string ao_d;
    switch(aoRange) {
	case 0:	ao_d = "0;20";	break;	//Engineer (00.000 20.000)
	case 1:	ao_d = "4;20";	break;	//Engineer (04.000 20.000)
	case 2:	ao_d = "0;10";	break;	//Engineer (00.000 10.000)
	case 3:	ao_d = "0;20";	break;	//Engineer (+00.000 +20.000)
	case 4:	ao_d = "4;20";	break;	//Engineer (+04.000 +20.000)
	case 5:	ao_d = "0;10";	break;	//Engineer (+00.000 +10.000)
	case 6:	ao_d = "-10;10";break;	//Engineer (-10.000 +10.000)
	case 7:	ao_d = "0;5";	break;	//Engineer (+00.000 +05.000)
	case 8:	ao_d = "-5;5";	break;	//Engineer (-05.000 +05.000)
	case 9:	ao_d = "0;100";	break;	//Percent (+000.00 +100.00)
	case 10:ao_d = "0;4095";break;	//Hexadecimal (000 FFF)
	default:ao_d = "";	break;
    }

    switch(aoMethod) {
	case 1: itCnt = 1;	break;	//1AO (#AA)
	case 2: itCnt = 2;	break;	//2AO (#AAN)
	case 4: itCnt = 4;	break;	//4AO (#AAN)
	case 14: itCnt = 4;	break;	//4AO (#AACN)
	default:itCnt = 0;	break;
    }
    if(itCnt) {
	string saoMethod = i2s(aoMethod);
	pEl.fldAdd(new TFld("aoErr","aoErr",TFld::String,TFld::NoWrite|TVal::DirRead,"","","","",saoMethod.c_str()) );
	for(unsigned i_p = 0; i_p < itCnt; i_p++)
	    pEl.fldAdd(new TFld(TSYS::strMess("AO%d",i_p).c_str(),TSYS::strMess("AO%d",i_p).c_str(),
		    TFld::Real,TVal::DirRead|TVal::DirWrite,"20","1",ao_d.c_str(),"",saoMethod.c_str())); 
    }

    // DI
    switch(diMethod) {
	case 3:	itCnt = 3;	break;	//3DI ($AA6)
	case 4:	itCnt = 4;	break;	//4DI ($AA6)
	case 7:	itCnt = 7;	break;	//7DI ($AA6)
	case 8:	itCnt = 8;	break;	//8DI ($AA6)
	case 14:itCnt = 14;	break;	//14DI ($AA6)
	case 16:itCnt = 16;	break;	//16DI ($AA6)
	case 101:itCnt = 1;	break;	//1DI (@AADI)
	case 201:itCnt = 8;	break;	//8DI ($AA6,FF00)
	default:itCnt = 0;	break;
    }
    if(itCnt) {
	string sdiMethod = i2s(diMethod);
	pEl.fldAdd(new TFld("diErr","diErr",TFld::String,TFld::NoWrite|TVal::DirRead,"","","","",sdiMethod.c_str()));
	for(unsigned i_p = 0; i_p < itCnt; i_p++)
	    pEl.fldAdd(new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
		    TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdiMethod.c_str()));
    }

    // DO
    switch(doMethod) {
	case 2:	itCnt = 2;	break;	//2DO (@AA,0300)
	case 3:	itCnt = 3;	break;	//3DO (@AA,7)
	case 4:	itCnt = 4;	break;	//4DO (@AA,F)
	case 5:	itCnt = 5;	break;	//5DO (@AA,1F)
	case 7:	itCnt = 7;	break;	//7DO (@AA,7F)
	case 8:	itCnt = 8;	break;	//8DO (@AA,FF)
	case 12:itCnt = 12;	break;	//12DO (@AA,0FFF)
	case 13:itCnt = 13;	break;	//13DO (@AA,1FFF)
	case 16:itCnt = 16;	break;	//16DO (@AA,FFFF)
	case 102:itCnt = 2;	break;	//2DO (^AADOVVV)
	case 103:itCnt = 3;	break;	//3DO (^AADOVVV)
	case 202:itCnt = 2;	break;	//2DO (@AADO)
	case 204:itCnt = 4;	break;	//4DO (@AADO)
	case 306:itCnt = 6;	break;	//@AADODD
	case 402:itCnt = 2;	break;	//@AADO0D
	case 504:itCnt = 4;	break;	//4DO (@(^)AADO0D)
	case 604:itCnt = 4;	break;	//4DO (@AA,0F00)
	case 608:itCnt = 8;	break;	//8DO (@AA,FF00)
	case 704:itCnt = 4;	break;	//4DO (#AA000F)
	case 708:itCnt = 8;	break;	//8DO (#AA00FF)
	case 712:itCnt = 12;	break;	//12DO (#AA000FFF)
	default:itCnt = 0;	break;
    }
    if(itCnt) {
	string sdoMethod = i2s(doMethod);
	pEl.fldAdd(new TFld("doErr","doErr",TFld::String,TFld::NoWrite|TVal::DirRead,"","","","",sdoMethod.c_str()) );
	for(unsigned i_p = 0; i_p < itCnt; i_p++)
	    pEl.fldAdd(new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
		    TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdoMethod.c_str()));
    }

    // CI
    switch(ciMethod) {
	case 2:	itCnt = 2;	break;	//2CI (#AA)
	case 3:	itCnt = 3;	break;	//3CI (#AA)
	default:itCnt = 0;	break;
    }
    if(itCnt) {
	string sciMethod = i2s(ciMethod);
	pEl.fldAdd(new TFld("ciErr","ciErr",TFld::String,TFld::NoWrite|TVal::DirRead,"","","","",sciMethod.c_str()));
	for(unsigned i_p = 0; i_p < itCnt; i_p++)
	    pEl.fldAdd(new TFld(TSYS::strMess("CI%d",i_p).c_str(),TSYS::strMess("CI%d",i_p).c_str(),
		    TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sciMethod.c_str()));
    }

    owner().prmEn(this, true);
}

void TMdPrm::disable()
{
    if(!enableStat())	return;

    owner().prmEn(this, false);

    TParamContr::disable();

    //Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat()) {
	if(val.name() == "err") {
	    if(!enableStat())			val.setS(_("1:Parameter disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition stopped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

    if(owner().redntUse()) return;
    if(val.name() == "aiErr")		val.setS(aiErr.getVal(),0,true);
    else if(val.name() == "aoErr")	val.setS(aoErr.getVal(),0,true);
    else if(val.name() == "diErr")	val.setS(diErr.getVal(),0,true);
    else if(val.name() == "doErr")	val.setS(doErr.getVal(),0,true);
    else if(val.name() == "ciErr")	val.setS(ciErr.getVal(),0,true);
    else if(val.name() == "err") {
	if(aiErr.getVal() != "0")	val.setS(aiErr.getVal(),0,true);
	else if(aoErr.getVal() != "0")	val.setS(aoErr.getVal(),0,true);
	else if(diErr.getVal() != "0")	val.setS(diErr.getVal(),0,true);
	else if(doErr.getVal() != "0")	val.setS(doErr.getVal(),0,true);
	else if(ciErr.getVal() != "0")	val.setS(ciErr.getVal(),0,true);
	else val.setS("0",0,true);
    }
    else if(val.name().compare(0,2,"AI") == 0) val.setR(AI[s2i(val.name().substr(2,val.name().size()-2))],0,true);
    else if(val.name().compare(0,2,"AO") == 0) val.setR(AO[s2i(val.name().substr(2,val.name().size()-2))],0,true);
    else if(val.name().compare(0,2,"DI") == 0) val.setB(DI[s2i(val.name().substr(2,val.name().size()-2))],0,true);
    else if(val.name().compare(0,2,"DO") == 0) val.setB(DO[s2i(val.name().substr(2,val.name().size()-2))],0,true);
    else if(val.name().compare(0,2,"CI") == 0) val.setR(CI[s2i(val.name().substr(2,val.name().size()-2))],0,true);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setI(EVAL_INT, 0, true); return; }

    //Send to active reserve station
    if(vlSetRednt(vo,vl,pvl))	return;

    //Direct write
    if(vo.name().compare(0,2,"AO") == 0) AO[s2i(vo.name().substr(2,vo.name().size()-2))] = vl.getR();
    if(vo.name().compare(0,2,"DO") == 0) DO[s2i(vo.name().substr(2,vo.name().size()-2))] = vl.getB();
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? (int64_t)owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

bool TMdPrm::cfgChange( TCfg &co, const TVariant &pc )
{
    TParamContr::cfgChange(co, pc);

    if(enableStat() && (co.fld().name() == "AI_METHOD" || co.fld().name() == "AO_METHOD" ||
	    co.fld().name() == "DI_METHOD" || co.fld().name() == "DO_METHOD" || co.fld().name() == "CI_METHOD"))
	disable();

    return true;
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	if(aiMethod == 0) ctrRemoveNode(opt,"/prm/cfg/AI_RANGE");
	if(aoMethod == 0) ctrRemoveNode(opt,"/prm/cfg/AO_RANGE");
	return;
    }

    //Process command to page
    TParamContr::cntrCmdProc(opt);
}

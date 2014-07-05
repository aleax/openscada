
//OpenSCADA system module DAQ.System file: da_ups.cpp
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko, <rom_as@oscada.org>           *
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

#include <tsys.h>

#include "os_contr.h"
#include "da_ups.h"

using namespace SystemCntr;

//*************************************************
//* UPS                                           *
//*************************************************
UPS::UPS( ) : tTr("Sockets"), nTr("sys_UPS")
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&reqRes, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

UPS::~UPS( )
{
    pthread_mutex_destroy(&reqRes);
}

void UPS::init( TMdPrm *prm )
{
    prm->daData = new tval();
    prm->vlElemAtt(&((tval*)prm->daData)->els);

    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setDescr(_("UPS"));
    c_subt.fld().setFlg(c_subt.fld().flg()|TFld::SelEdit);
    c_subt.setS("localhost:3493");

    string dls = upsList(c_subt.getS());
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);
    if(dls.size()) c_subt.setS(TSYS::strParse(dls,0,";"));
}

void UPS::deInit( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setFlg(c_subt.fld().flg()&~(TFld::SelEdit));

    prm->vlElemDet(&((tval*)prm->daData)->els);
    delete (tval*)prm->daData;
    prm->daData = NULL;
}

void UPS::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "SUBT") {
	string dls = upsList(co.getS());
	co.fld().setValues(dls);
	co.fld().setSelNames(dls);
    }
}

string UPS::upsList( const string &addr )
{
    string rez, host, val, c_el;
    char name[51] = "", dscr[256] = "";

    try {
	MtxAlloc res(reqRes, true);
	if((host=TSYS::strParse(TSYS::strParse(addr,0," "),1,"@")).empty()) host = TSYS::strParse(addr,0," ");
	val = reqUPS(host, "LIST UPS\x0A", (mess_lev()==TMess::Debug)?mod->nodePath():"");
	for(int off = 0, lstSec = false; (c_el=TSYS::strLine(val,0,&off)).size(); )
	    if(c_el == "BEGIN LIST UPS") lstSec = true;
	    else if(c_el == "END LIST UPS")	break;
	    else if(lstSec)
		if(sscanf(c_el.c_str(),"UPS %50s \"%255[^\"]s\"",name,dscr) == 2)
		    rez = rez + name+"@"+host+" ("+dscr+");";
    }
    catch(TError err) { /*mess_err(err.cat.c_str(),"%s",err.mess.c_str());*/ }

    return rez;
}

void UPS::getVal( TMdPrm *prm )
{
    vector<string> als;
    try {
	string c_el, addr = TSYS::strParse(prm->cfg("SUBT").getS(),0," ");
	int aOff = 0;
	string UPS = TSYS::strParse(addr, 0, "@", &aOff);
	char var[51] = "", vVal[256] = "";
	if(aOff < (int)addr.size()) {
	    MtxAlloc res(reqRes, true);

	    //Vars list and values obtain
	    string val = reqUPS(addr, "LIST VAR "+UPS+"\x0A", (prm->owner().messLev()==TMess::Debug)?prm->nodePath():"");
	    for(int off = 0, lstSec = false; (c_el=TSYS::strLine(val,0,&off)).size(); )
		if(c_el == "BEGIN LIST VAR "+UPS) lstSec = true;
		else if(c_el == "END LIST VAR "+UPS)	break;
		else if(lstSec) {
		    if(sscanf(c_el.c_str(),"VAR %*s %50s \"%255[^\"]s\"",var,vVal) != 2) continue;
		    string vid = var;
		    string aid = TSYS::strEncode(vid,TSYS::oscdID);
		    string aVal = vVal;
		    if(!prm->vlPresent(aid)) {
			// Description request
			string descr = reqUPS(addr, "GET DESC "+UPS+" "+var+"\x0A", (prm->owner().messLev()==TMess::Debug)?prm->nodePath():"");
			if(sscanf(descr.c_str(),"DESC %*s %*s \"%255[^\"]s\"",vVal) == 1 && strcmp(vVal,"Description unavailable")) descr = vVal;
			else descr = var;
			// Type request
			string stp = reqUPS(addr, "GET TYPE "+UPS+" "+var+"\x0A", (prm->owner().messLev()==TMess::Debug)?prm->nodePath():"");
			string vLen, selLs;
			size_t fPos = 0;
			unsigned flg = (stp.rfind("RW") != string::npos) ? (unsigned)TVal::DirWrite : (unsigned)TFld::NoWrite;
			if((fPos=stp.rfind("STRING:")) != string::npos) vLen = atoi(stp.c_str()+fPos+7);
			if(stp.rfind("ENUM") != string::npos) {
			    string enS = reqUPS(addr, "LIST ENUM "+UPS+" "+var+"\x0A", (prm->owner().messLev()==TMess::Debug)?prm->nodePath():"");
			    string enSel;
			    for(int offEn = 0, lstSecEn = false; (enSel=TSYS::strLine(enS,0,&offEn)).size(); )
				if(enSel == "BEGIN LIST ENUM "+UPS+" "+var)	lstSecEn = true;
				else if(enSel == "END LIST ENUM "+UPS+" "+var)	break;
				else if(lstSecEn && sscanf(enSel.c_str(),"ENUM %*s %*s \"%255[^\"]s\"",vVal) == 1) selLs = selLs + vVal + ";";

			    flg |= TFld::Selected;
			}
			// Create
			((tval*)prm->daData)->els.fldAdd(new TFld(aid.c_str(),descr.c_str(),TFld::String,flg,vLen.c_str(),"",selLs.c_str(),selLs.c_str(),vid.c_str()));
		    }
		    prm->vlAt(aid).at().setS(aVal, 0, true);
		    als.push_back(aid);
		}

	    //Commands list obtain
	    val = reqUPS(addr, "LIST CMD "+UPS+"\x0A", (prm->owner().messLev()==TMess::Debug)?prm->nodePath():"");
	    for(int off = 0, lstSec = false; (c_el=TSYS::strLine(val,0,&off)).size(); )
		if(c_el == "BEGIN LIST CMD "+UPS) lstSec = true;
		else if(c_el == "END LIST CMD "+UPS)	break;
		else if(lstSec) {
		    if(sscanf(c_el.c_str(),"CMD %*s %50s",var) != 1) continue;
		    string vid = var;
		    string aid = TSYS::strEncode(vid,TSYS::oscdID);
		    if(!prm->vlPresent(aid)) {
			// Description request
			string descr = reqUPS(addr, "GET CMDDESC "+UPS+" "+vid+"\x0A", (prm->owner().messLev()==TMess::Debug)?prm->nodePath():"");
			if(sscanf(descr.c_str(),"CMDDESC %*s %*s \"%255[^\"]s\"",vVal) == 1 && strcmp(vVal,"Description unavailable")) descr = vVal;
			else descr = var;
			// Create
			((tval*)prm->daData)->els.fldAdd(new TFld(aid.c_str(),descr.c_str(),TFld::Boolean,TVal::DirWrite,"","","","",vid.c_str()));
		    }
		    prm->vlAt(aid).at().setB(false, 0, true);
		    als.push_back(aid);
		}
	}
    }
    catch(TError err) { /*mess_err(err.cat.c_str(),"%s",err.mess.c_str());*/ }

    if(als.size()) {
	prm->daErr = "";
	//Check for delete DAQ parameter's attributes
	for(int i_p = 0; i_p < (int)fldSize(); i_p++) {
	    unsigned i_l;
	    for(i_l = 0; i_l < als.size(); i_l++)
		if(fldAt(i_p).name() == als[i_l])
		    break;
	    if(i_l >= als.size())
		try { fldDel(i_p); i_p--; }
		catch(TError err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
	}
    }
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void UPS::vlSet( TMdPrm *p, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    string vId = vo.fld().reserve();
    string addr = TSYS::strParse(p->cfg("SUBT").getS(),0," ");
    int aOff = 0;
    string UPS = TSYS::strParse(addr, 0, "@", &aOff);
    if(vId.empty() || aOff >= (int)addr.size())	return;

    MtxAlloc res(reqRes, true);

    bool OK = true;
    OK = OK && reqUPS(addr,"USERNAME "+p->addPrm("USER")+"\x0A",(p->owner().messLev()==TMess::Debug)?p->nodePath():"").compare(0,2,"OK") == 0;
    OK = OK && reqUPS(addr,"PASSWORD "+p->addPrm("PASS")+"\x0A",(p->owner().messLev()==TMess::Debug)?p->nodePath():"").compare(0,2,"OK") == 0;

    if(OK) {
	//Instant commands
	if(vo.fld().type() == TFld::Boolean) {
	    if(vl.getB()) reqUPS(addr, "INSTCMD "+UPS+" "+vId+"\x0A", (p->owner().messLev()==TMess::Debug)?p->nodePath():"");
	}
	//RW variable set
	else
	    reqUPS(addr, "SET VAR "+UPS+" "+vId+" \""+vl.getS()+"\"\x0A", (p->owner().messLev()==TMess::Debug)?p->nodePath():"");
    }

    OK = OK && reqUPS(addr, "LOGOUT\x0A", (p->owner().messLev()==TMess::Debug)?p->nodePath():"").compare(0,2,"OK") == 0;
}

string UPS::reqUPS( const string &addr, const string &req, const string &debCat )
{
    string val, host;
    char buf[1024];

    if((host=TSYS::strParse(TSYS::strParse(addr,0," "),1,"@")).empty()) host = TSYS::strParse(addr,0," ");

    //Check connect and start
    AutoHD<TTransportOut> tr;
    if(!SYS->transport().at().at(tTr).at().outPresent(nTr)) {
	SYS->transport().at().at(tTr).at().outAdd(nTr);
	tr = SYS->transport().at().at(tTr).at().outAt(nTr);
	tr.at().setName(_("UPS"));
	tr.at().setTimings("3:0.1");
    }
    else tr = SYS->transport().at().at(tTr).at().outAt(nTr);

    ResAlloc resN(tr.at().nodeRes(), true);
    if(tr.at().addr() != "TCP:"+host) tr.at().setAddr("TCP:"+host);
    if(!tr.at().startStat()) tr.at().start();

    if(debCat.size()) mess_debug_(debCat.c_str(), _("REQ -> '%s'"), req.c_str());

    //Request
    int resp_len = tr.at().messIO(req.data(), req.size(), buf, sizeof(buf), 0, true);
    val.assign(buf, resp_len);

    //Wait tail
    while(resp_len) {
	try{ resp_len = tr.at().messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
	val.append(buf, resp_len);
    }

    if(debCat.size()) mess_debug_(debCat.c_str(), _("RESP -> '%s'"), val.c_str());

    return val;
}

bool UPS::cntrCmdProc( TMdPrm *p, XMLNode *opt )
{
    if(opt->name() == "info") {
	p->ctrMkNode("fld",opt,-1,"/prm/cfg/user",_("User"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	p->ctrMkNode("fld",opt,-1,"/prm/cfg/pass",_("Password"),RWRWR_,"root",SDAQ_ID,1,"tp","str");
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/user") {
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(p->addPrm("USER"));
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	p->setAddPrm("USER", opt->text());
    }
    else if(a_path == "/prm/cfg/pass") {
	if(p->ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(string(p->addPrm("PASS").size(),'*'));
	if(p->ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	p->setAddPrm("PASS", opt->text());
    }
    else return false;

    return true;
}

void UPS::makeActiveDA( TMdContr *aCntr )
{
    string list = upsList("localhost:3493"), uEl;
    try {
	//UPS list process
	for(int off = 0; (uEl=TSYS::strParse(list,0,";",&off)).size(); ) {
	    vector<string> pLs;
	    // Find propper parameter's object
	    aCntr->list(pLs);

	    unsigned i_p;
	    for(i_p = 0; i_p < pLs.size(); i_p++) {
		AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
		if(p.at().cfg("TYPE").getS() == id() && TSYS::strParse(p.at().cfg("SUBT"),0," ") == TSYS::strParse(uEl,0," "))
		    break;
	    }
	    if(i_p < pLs.size()) continue;

	    string upsAprm = "UPS";
	    while(aCntr->present(upsAprm)) upsAprm = TSYS::strLabEnum(upsAprm);
	    aCntr->add(upsAprm, 0);
	    AutoHD<TMdPrm> dprm = aCntr->at(upsAprm);
	    dprm.at().setName(_("UPS: ")+uEl);
	    dprm.at().autoC(true);
	    dprm.at().cfg("TYPE").setS(id());
	    dprm.at().cfg("SUBT").setS(uEl);
	    dprm.at().cfg("EN").setB(true);
	    if(aCntr->enableStat()) dprm.at().enable();
	}
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

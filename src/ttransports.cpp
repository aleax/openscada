
//OpenSCADA file: ttransports.cpp
/***************************************************************************
 *   Copyright (C) 2003-2023 by Roman Savochenko, <roman@oscada.org>       *
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

#include <string.h>

#include "tsys.h"
#include "tmess.h"
#include "tmodule.h"
#include "tprotocols.h"
#include "ttransports.h"

using namespace OSCADA;

//************************************************
//* TTransportS					 *
//************************************************
TTransportS::TTransportS( ) : TSubSYS(STR_ID, true), extHostLoad(0)
{
    //Input transport BD structure
    elIn.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elIn.fldAdd(new TFld("MODULE",trS("Transport type"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elIn.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elIn.fldAdd(new TFld("DESCRIPT",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,"500"));
    elIn.fldAdd(new TFld("ADDR",trS("Address"),TFld::String,TFld::NoFlag,"100"));
    elIn.fldAdd(new TFld("PROT",trS("Transport protocol"),TFld::String,TFld::NoFlag,i2s(limObjID_SZ*10).c_str()));
    elIn.fldAdd(new TFld("START",trS("To connect"),TFld::Boolean,TFld::NoFlag,"1"));

    //Output transport BD structure
    elOut.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ*2).c_str()));
    elOut.fldAdd(new TFld("MODULE",trS("Transport type"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elOut.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elOut.fldAdd(new TFld("DESCRIPT",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,"500"));
    elOut.fldAdd(new TFld("ADDR",trS("Address"),TFld::String,TFld::NoFlag,"100"));

    //External hosts' connection DB struct
    elExt.fldAdd(new TFld("OP_USER",trS("User that opens"),TFld::String,TCfg::Key,i2s(limObjID_SZ).c_str()));
    elExt.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key,i2s(limObjID_SZ).c_str()));
    elExt.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elExt.fldAdd(new TFld("TRANSP",trS("Transport"),TFld::String,0,i2s(limObjID_SZ*3).c_str()));
    elExt.fldAdd(new TFld("ADDR",trS("Transport address"),TFld::String,0,"50"));
    elExt.fldAdd(new TFld("USER",trS("User of the requests"),TFld::String,0,i2s(limObjID_SZ).c_str()));
    elExt.fldAdd(new TFld("PASS",trS("Password of the requests"),TFld::String,0,"100"));
    elExt.fldAdd(new TFld("UpRiseLev",trS("Level of lifting"),TFld::Integer,0,"1"));
}

TTransportS::~TTransportS( )
{

}

void TTransportS::inTrList( vector<string> &ls )
{
    ls.clear();

    vector<string> tLs, mLs;
    modList(tLs);
    for(unsigned iTp = 0; iTp < tLs.size(); iTp++) {
	at(tLs[iTp]).at().inList(mLs);
	for(unsigned iT = 0; iT < mLs.size(); iT++)
	    ls.push_back(tLs[iTp]+"."+mLs[iT]);
    }
}

void TTransportS::outTrList( vector<string> &ls )
{
    ls.clear();

    vector<string> tLs, mLs;
    modList(tLs);
    for(unsigned iTp = 0; iTp < tLs.size(); iTp++) {
	at(tLs[iTp]).at().outList(mLs);
	for(unsigned iT = 0; iT < mLs.size(); iT++)
	    ls.push_back(tLs[iTp]+"."+mLs[iT]);
    }
}

AutoHD<TTransportOut> TTransportS::outAt( const string &addr )
{
    int off = 0;
    string trPath = TSYS::strParse(addr, 0, ":", &off);
    string trAddr = addr.substr(off);

    off = 0;
    string trMod = TSYS::strParse(trPath, 0, ".", &off);
    string trId = TSYS::strParse(trPath, 0, ".", &off);

    bool isIn = (trId.find(STR_IN_PREF) == 0);
    if(isIn) trId = trId.substr(strlen(STR_IN_PREF));
    else if(trId.find(STR_OUT_PREF) == 0) trId = trId.substr(strlen(STR_OUT_PREF));

    if(trMod.empty() || !modPresent(trMod) || (isIn && (!at(trMod).at().inPresent(trId) || trAddr.empty())) ||
	    (!isIn && !at(trMod).at().outPresent(trId) && trAddr.empty()) )
	throw err_sys(_("Error the output address '%s'!"), addr.c_str());

    //Input no protocol transports for the remote hosts initiative connections of they control at the connections
    if(isIn) return at(trMod).at().inAt(trId).at().associateTr(trAddr);

    //Standard output connection
    if(!at(trMod).at().outPresent(trId)) at(trMod).at().outAdd(trId);

    AutoHD<TTransportOut> rezTr = at(trMod).at().outAt(trId);
    if(trAddr.size() && rezTr.at().addr() != trAddr) {
	rezTr.at().setAddr(trAddr);
	rezTr.at().stop();
    }

    return rezTr;
}

string TTransportS::extHostsDB( )	{ return "*.*.CfgExtHosts"; }

void TTransportS::load_( )
{
    //Load parameters from command line

    //Load parameters from config-file

    //Load DB
    string id, type;
    map<string, bool>	itReg;
    // Search and create new input transports
    try {
	TConfig c_el(&elIn);
	//c_el.cfgViewAll(false);
	vector<string> itLs;

	//  Search new into DB and Config-file
	TBDS::dbList(itLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    for(int fld_cnt = 0; TBDS::dataSeek(itLs[iIt]+"."+subId()+"_in",nodePath()+subId()+"_in",fld_cnt++,c_el,TBDS::UseCache); ) {
		id   = c_el.cfg("ID").getS();
		type = c_el.cfg("MODULE").getS();
		if(!modPresent(type))	continue;
		if(!at(type).at().inPresent(id)) at(type).at().inAdd(id, itLs[iIt]);
		if(at(type).at().inAt(id).at().DB() == itLs[iIt]) at(type).at().inAt(id).at().load(&c_el);
		at(type).at().inAt(id).at().setDB(itLs[iIt], true);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    vector<string> mLs;
	    modList(mLs);
	    for(unsigned iM = 0; iM < mLs.size(); iM++) {
		at(mLs[iM]).at().inList(itLs);
		for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		    if(itReg.find(mLs[iM]+"."+itLs[iIt]) == itReg.end() && SYS->chkSelDB(at(mLs[iM]).at().inAt(itLs[iIt]).at().DB()))
			at(mLs[iM]).at().inDel(itLs[iIt]);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and creating new input transports."));
    }

    // Search and create new output transports
    try {
	TConfig c_el(&elOut);
	//c_el.cfgViewAll(false);
	vector<string> itLs;
	itReg.clear();

	//  Search new into DB and Config-file
	TBDS::dbList(itLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    for(int fld_cnt = 0; TBDS::dataSeek(itLs[iIt]+"."+subId()+"_out",nodePath()+subId()+"_out",fld_cnt++,c_el,TBDS::UseCache); ) {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODULE").getS();
		if(!modPresent(type))	continue;
		if(!at(type).at().outPresent(id)) at(type).at().outAdd(id, itLs[iIt]);
		if(at(type).at().outAt(id).at().DB() == itLs[iIt]) at(type).at().outAt(id).at().load(&c_el);
		at(type).at().outAt(id).at().setDB(itLs[iIt], true);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    vector<string> mLs;
	    modList(mLs);
	    for(unsigned iM = 0; iM < mLs.size(); iM++) {
		at(mLs[iM]).at().outList(itLs);
		for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		    if(itReg.find(mLs[iM]+"."+itLs[iIt]) == itReg.end() && SYS->chkSelDB(at(mLs[iM]).at().outAt(itLs[iIt]).at().DB()))
			at(mLs[iM]).at().outDel(itLs[iIt]);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and creating new output transports."));
    }

    // Load external hosts
    try {
	map<string, bool> regIds;
	TConfig c_el(&elExt);
	for(int fld_cnt = 0; TBDS::dataSeek(extHostsDB(),nodePath()+"ExtTansp",fld_cnt++,c_el,TBDS::UseCache); ) {
	    ExtHost host("", "");
	    host.userOpen	= c_el.cfg("OP_USER").getS();
	    host.id		= c_el.cfg("ID").getS();
	    if(regIds[host.userOpen+":"+host.id]) continue;	//Preventing the loading both config and DB
	    host.name		= c_el.cfg("NAME").getS();
	    host.transp		= c_el.cfg("TRANSP").getS();
	    host.addr		= c_el.cfg("ADDR").getS();
	    host.user		= c_el.cfg("USER").getS();
	    host.pass		= c_el.cfg("PASS").getS();
	    host.upRiseLev	= c_el.cfg("UpRiseLev").getI();
	    extHostSet(host, false, true);
	    regIds[host.userOpen+":"+host.id] = true;
	}
	extHostLoad = time(NULL);
    } catch(TError &err) {
	mess_err(err.cat.c_str()," %s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and loading external hosts."));
    }
}

void TTransportS::save_( )
{
    //Save external transports
    ResAlloc res(extHostRes, false);
    TConfig c_el(&elExt);
    for(unsigned iH = 0; iH < extHostLs.size(); iH++) {
	c_el.cfg("OP_USER").setS(extHostLs[iH].userOpen);
	c_el.cfg("ID").setS(extHostLs[iH].id);
	c_el.cfg("NAME").setS(extHostLs[iH].name);
	c_el.cfg("TRANSP").setS(extHostLs[iH].transp);
	c_el.cfg("ADDR").setS(extHostLs[iH].addr);
	c_el.cfg("USER").setS(extHostLs[iH].user);
	c_el.cfg("PASS").setS(extHostLs[iH].pass);
	c_el.cfg("UpRiseLev").setI(extHostLs[iH].upRiseLev);
	TBDS::dataSet(extHostsDB(), nodePath()+"ExtTansp", c_el);
    }
    //Clear external transports
    c_el.cfgViewAll(false);
    for(int fld_cnt = 0; TBDS::dataSeek(extHostsDB(),nodePath()+"ExtTansp",fld_cnt++,c_el); )
	if(!extHostGet(c_el.cfg("OP_USER").getS(),c_el.cfg("ID").getS()).id.size()) {
	    TBDS::dataDel(extHostsDB(), nodePath()+"ExtTansp", c_el, TBDS::UseAllKeys|TBDS::NoException);
	    fld_cnt--;
	}
}

void TTransportS::unload( )
{
    TSubSYS::unload();

    extHostRes.lock(true);
    extHostLs.clear();
    extHostRes.unlock();
}

void TTransportS::subStart( )
{
    vector<string> tLst, oLst;
    modList(tLst);
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	AutoHD<TTypeTransport> mod = modAt(tLst[iT]);
	oLst.clear();
	mod.at().inList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++)
	    try {
		AutoHD<TTransportIn> in = mod.at().inAt(oLst[iO]);
		if(!in.at().startStat() && in.at().toStart()) in.at().start();
	    } catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error starting the input transport '%s'."), oLst[iO].c_str());
	    }
    }

    //Controllers start
    TSubSYS::subStart( );
}

void TTransportS::subStop( )
{
    vector<string> tLst, oLst;
    modList(tLst);
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	AutoHD<TTypeTransport> mod = modAt(tLst[iT]);
	oLst.clear();
	mod.at().inList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++)
	    try {
		AutoHD<TTransportIn> in = mod.at().inAt(oLst[iO]);
		if(in.at().startStat()) in.at().stop();
	    } catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error stopping the input transport '%s'."), oLst[iO].c_str());
	    }
	oLst.clear();
	mod.at().outList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++)
	    try {
		AutoHD<TTransportOut> out = mod.at().outAt(oLst[iO]);
		if(out.at().startStat()) out.at().stop();
	    } catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error stopping the output transport '%s'."), oLst[iO].c_str());
	    }
    }

    TSubSYS::subStop();
}

string TTransportS::optDescr( )
{
    return TSYS::strMess(_(
	"======================= Subsystem \"Transports\" options ==================\n\n"
	)) + TSubSYS::optDescr();
}

void TTransportS::extHostList( const string &user, vector<ExtHost> &list, bool andSYS, int upRiseLev, const string &lang )
{
    list.clear();
    ResAlloc res(extHostRes, false);
    for(unsigned iH = 0; iH < extHostLs.size(); iH++)
	if(!user.size() || user == extHostLs[iH].userOpen || (andSYS && extHostLs[iH].userOpen == "*")) {
	    bool itSet = false;
	    for(vector<ExtHost>::iterator iL = list.begin(); (!user.size() || andSYS) && !itSet && iL != list.end(); ++iL)
		if((itSet=(iL->id == extHostLs[iH].id)) && iL->userOpen != extHostLs[iH].userOpen) iL->mode = ExtHost::UserSystem;
	    if(itSet) continue;
	    list.push_back(extHostLs[iH]);
	    if(list.back().mode < 0) list.back().mode = (list.back().userOpen == "*") ? ExtHost::System : ExtHost::User;
	}
    res.unlock();

    //Prepare and request to the station for it's external hosts list
    XMLNode req("get"), *nId, *nT;
    for(unsigned iH = 0, nH = list.size(), wUpRiseLev; iH < nH; iH++) {
	if(!(wUpRiseLev=(upRiseLev<0)?list[iH].upRiseLev:upRiseLev)) continue;
	req.clear()->setAttr("path", "/"+list[iH].id+"/Transport/%2fsub%2fehost")->setAttr("lang", lang)->
		     setAttr("upRiseLev", i2s(wUpRiseLev-1))->setAttr("upRiseSYS", i2s(andSYS));
	try {
	    if(cntrIfCmd(req,"" /* "UpRiseLev" */,user)) continue;
	    nId = req.childGet("id", "id", true);
	    for(unsigned iH1 = 0; nId && iH1 < nId->childSize(); ++iH1) {
		ExtHost eh(user, list[iH].id+"."+nId->childGet(iH1)->text());
		if((nT=req.childGet("id","name",true)) && iH1 < nT->childSize()) eh.name = list[iH].name + " > " + nT->childGet(iH1)->text();
		if((nT=req.childGet("id","transp",true)) && iH1 < nT->childSize()) eh.transp = nT->childGet(iH1)->text();
		if((nT=req.childGet("id","addr",true)) && iH1 < nT->childSize()) eh.addr = nT->childGet(iH1)->text();
		if((nT=req.childGet("id","user",true)) && iH1 < nT->childSize()) eh.user = nT->childGet(iH1)->text();
		if((nT=req.childGet("id","pass",true)) && iH1 < nT->childSize()) eh.pass = nT->childGet(iH1)->text();
		if((nT=req.childGet("id","mode",true)) && iH1 < nT->childSize()) eh.mode = s2i(nT->childGet(iH1)->text());
		if((nT=req.childGet("id","upRiseLev",true)) && iH1 < nT->childSize()) eh.upRiseLev = s2i(nT->childGet(iH1)->text());
		list.push_back(eh);
	    }
	} catch(TError &err) { }
    }
}

TTransportS::ExtHost TTransportS::extHostGet( const string &user, const string &id, bool andSYS )
{
    ResAlloc res(extHostRes, false);
    ExtHost eh(user, "");
    for(unsigned iH = 0; iH < extHostLs.size(); ++iH)
	if(extHostLs[iH].id == id && (user.empty() || user == extHostLs[iH].userOpen || (andSYS && extHostLs[iH].userOpen == "*"))) {
	    if(eh.mode < 0) {
		eh = extHostLs[iH];
		eh.mode = (eh.userOpen == "*") ? ExtHost::System : ExtHost::User;
	    }
	    else if(eh.userOpen != extHostLs[iH].userOpen) { eh.mode = ExtHost::UserSystem; break; }
	}
    if(eh.userOpen == "*" && user.size() && user != eh.userOpen) eh.userOpen = user;

    return eh;
}

TTransportS::ExtHost TTransportS::extHostSeek( const string &id, int lev )
{
    ResAlloc res(extHostRes, false);
    for(unsigned iH = 0; iH < extHostLs.size(); ++iH)
	if(extHostLs[iH].id == id && !(lev--))	return extHostLs[iH];

    return ExtHost("", "");
}

void TTransportS::extHostSet( const ExtHost &host, bool andSYS, bool load )
{
    ResAlloc res(extHostRes, true);
    int usrHstId = -1, sysHstId = -1;
    for(int iH = 0; iH < (int)extHostLs.size() && (usrHstId < 0 || sysHstId < 0); iH++)
	if(extHostLs[iH].id == host.id) {
	    if(host.mode < 0) { if(host.userOpen == extHostLs[iH].userOpen) { usrHstId = iH; break; } }
	    else if(extHostLs[iH].userOpen == host.userOpen)	usrHstId = iH;
	    else if(extHostLs[iH].userOpen == "*")		sysHstId = iH;
	}

    time_t saveMdf = (load && usrHstId >= 0 && extHostLs[usrHstId].mdf <= extHostLoad) ? extHostLs[usrHstId].mdf : 0;

    if(host.mode < 0 || !andSYS) {
	if(usrHstId < 0) extHostLs.push_back(host);
	else extHostLs[usrHstId] = host;
    }
    else {
	//Append or update
	if(host.mode == ExtHost::User || host.mode == ExtHost::UserSystem) {
	    if(usrHstId < 0) extHostLs.push_back(host);
	    else extHostLs[usrHstId] = host;
	}
	if(host.mode == ExtHost::System || host.mode == ExtHost::UserSystem) {
	    if(sysHstId < 0) { extHostLs.push_back(host); extHostLs.back().userOpen = "*"; }
	    else { extHostLs[sysHstId] = host; extHostLs[sysHstId].userOpen = "*"; }
	}
	//Remove
	if(host.mode == ExtHost::User && sysHstId >= 0) extHostLs.erase(extHostLs.begin() + sysHstId);
	if(host.mode == ExtHost::System && usrHstId >= 0 && usrHstId != sysHstId) extHostLs.erase(extHostLs.begin() + usrHstId);
    }

    if(saveMdf)	extHostLs[usrHstId].mdf = saveMdf;

    modif();
}

void TTransportS::extHostDel( const string &user, const string &id, bool andSYS )
{
    ResAlloc res(extHostRes, true);
    for(unsigned iH = 0; iH < extHostLs.size(); )
	if(extHostLs[iH].id == id &&
		(!user.size() || user == extHostLs[iH].userOpen || (andSYS && extHostLs[iH].userOpen == "*")))
	    extHostLs.erase(extHostLs.begin()+iH);
	else iH++;
    modif();
}

AutoHD<TTransportOut> TTransportS::extHost( TTransportS::ExtHost host, const string &pref )
{
    int off = 0;
    string trMod = TSYS::strParse(host.transp, 0, ".", &off);
    string trId = TSYS::strParse(host.transp, 0, ".", &off);
    bool isIn = (trId.find(STR_IN_PREF) == 0);
    if(isIn) trId = trId.substr(strlen(STR_IN_PREF));
    else {
	if(trId.find(STR_OUT_PREF) == 0) trId = trId.substr(strlen(STR_OUT_PREF));
	if(!trId.size()) trId = pref + host.id;
    }

    return outAt(trMod+"."+(isIn?STR_IN_PREF:STR_OUT_PREF)+trId+":"+host.addr);
}

TVariant TTransportS::objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang )
{
    // TCntrNodeObj outAt(string addr) - common-unified output transport connection at the address <addr> in the forms
    //    "{TrModule}.[out_]{TrID}[:{TrAddr}]" - typical output with automatic creation {TrID} at it missing and allowing {TrAddr};
    //    "{TrModule}.in_{TrID}:{RemConId}" - initiative input with the connection identifier in {RemConId}.
    //  TrModule - transport module, as is Sockets, SSL, Serial
    //  TrID - transport identifier
    //  TrAddr - transport specific address
    //  RemConId - remote initiative connection ID
    if(id == "outAt" && prms.size() >= 1) {
	try { return TVariant(new TCntrNodeObj(outAt(prms[0].getS()),user_lang)); } catch(TError&) { }

	return false;
    }

    return TCntrNode::objFuncCall(id, prms, user_lang);
}

int TTransportS::cntrIfCmd( XMLNode &node, const string &isenderPref, const string &iuser )
{
    int off = 0;
    string path = node.attr("path");
    string station = TSYS::pathLev(path, 0, false, &off);
    string senderPref = isenderPref;
    if(station.empty()) station = SYS->id();
    else node.setAttr("path", path.substr(off));

    if(station == SYS->id()) {
	node.setAttr("user", iuser.empty()?"root":iuser);
	SYS->cntrCmd(&node);
	node.setAttr("path", path);
	return s2i(node.attr("rez"));
    }

    //Checking to reforward
    off = 0;
    string tVl = TSYS::strParse(station, 0, ".", &off);
    if(off && off < (int)station.size()) {
	node.setAttr("reforwardHost", station.substr(off));
	senderPref += TSYS::strParse(station, 0, ".", &off);
	station = tVl;
    }

    //Connection to the transport
    off = 0;
    string user = TSYS::strLine(iuser, 0, &off), rqUser = TSYS::strLine(iuser, 0, &off), rqPass = TSYS::strLine(iuser, 0, &off);
    TTransportS::ExtHost host = extHostGet((user.empty()?"*":user), station);
    bool rqDir = (rqUser.size() && rqUser != host.user) || (rqUser == host.user && rqPass.size());
    node./*setAttr("rqDir", i2s(rqDir))->*/setAttr("rqUser", (rqDir?rqUser:host.user))->setAttr("rqPass", rqDir?rqPass:host.pass);
    AutoHD<TTransportOut> tr = extHost(host, senderPref);
    if(tr.at().startStat() && host.mdf > tr.at().startTm()) { tr.at().stop(); node.setAttr("rqAuthForce","1"); }
    if(!tr.at().startStat()) tr.at().start(s2i(node.attr("conTm")));
    if(mess_lev() == TMess::Debug) mess_debug((tr.at().nodePath()+senderPref).c_str(), "REQ: %s", node.save().c_str());

    tr.at().messProtIO(node, "SelfSystem");

    if(mess_lev() == TMess::Debug) mess_debug((tr.at().nodePath()+senderPref).c_str(), "RESP: %s", node.save().c_str());
    node.setAttr("path", path);
    //Password's hash processing
    if(!rqDir && node.attr("pHash").size()) {
	if(host.pass != (SEC_HASH_MAGIC+node.attr("pHash"))) {
	    TTransportS::ExtHost hostP("", "");
	    for(int lev = 0; (hostP=extHostSeek(station,lev)).id.size(); lev++) {
		if(hostP.user != host.user) continue;
		hostP.pass = SEC_HASH_MAGIC + node.attr("pHash");
		extHostSet(hostP);
	    }
	}
	node.setAttr("pHash", "");
    }

    return s2i(node.attr("rez"));
}

void TTransportS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),R_R_R_) &&
	    ctrMkNode2("table",opt,-1,"/sub/ehost",TSYS::strMess(_("External hosts of %s"),PACKAGE_NAME).c_str(),RWRWRW,"root",STR_ID,
		"s_com","add,del","key","id",NULL))
	{
	    ctrMkNode2("list",opt,-1,"/sub/ehost/id",_("Identifier"),RWRWRW,"root",STR_ID,"tp","str", NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/name",_("Name"),RWRWRW,"root",STR_ID,"tp","str", NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/transp",_("Transport"),RWRWRW,"root",STR_ID,
		"tp","str", "dest","select", "select","/sub/transps", NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/addr",_("Address"),RWRWRW,"root",STR_ID,"tp","str", NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/user",_("User"),RWRWRW,"root",STR_ID,"tp","str", NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/pass",_("Password"),RWRWRW,"root",STR_ID,"tp","str", NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/mode",_("Mode"),RWRW__,"root",STR_ID,"tp","dec", "dest","select",
		"sel_id",TSYS::strMess("%d;%d;%d",ExtHost::User,ExtHost::System,ExtHost::UserSystem).c_str(),
		"sel_list",_("User;System;User and System"),
		NULL);
	    ctrMkNode2("list",opt,-1,"/sub/ehost/upRiseLev",_("Level of lifting"),RWRWRW,"root",STR_ID,"tp","dec", NULL);
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path"), u = opt->attr("user");
    if(a_path.find("/sub/transps") == 0 && ctrChkNode(opt)) {
	string hostId = TSYS::strParse(a_path, 1, "-");
	if(hostId.size()) {
	    bool sysHostAcs = SYS->security().at().access(u, SEC_WR, "root", STR_ID, RWRWR_);
	    ExtHost host = extHostGet(u, hostId, sysHostAcs);
	    int cOff = 0, cLv = 0;
	    string cPath = "", cEl, pAddr = host.transp;

	    vector<string> list;
	    for(cOff = 0, cLv = 0; (cEl=TSYS::strParse(pAddr,0,".",&cOff)).size(); ++cLv) {
		cPath += cLv ? "."+cEl : cEl;
		opt->childAdd("el")->setText(cPath);
	    }
	    switch(cLv) {
		case 0:
		    SYS->transport().at().modList(list);
		    for(unsigned iS = 0; iS < list.size(); iS++)
			opt->childAdd("el")->setText(list[iS]);
		    break;
		case 1:
		    if(!SYS->transport().at().modPresent(cEl=TSYS::strParse(pAddr,0,"."))) break;
		    SYS->transport().at().at(cEl).at().outList(list);
		    for(unsigned iS = 0; iS < list.size(); iS++)
			opt->childAdd("el")->setText(cPath+"."+list[iS]);
		    SYS->transport().at().at(cEl).at().inList(list);
		    for(unsigned iS = 0; iS < list.size(); iS++)
			opt->childAdd("el")->setText(cPath+".in_"+list[iS]);
		    break;
	    }
	}
	else {
	    vector<string> list, listTrs;
	    modList(list);
	    // Automatic transports at the module
	    for(int iA = 0; iA < (int)list.size(); ++iA)
		if(at(list[iA]).at().isNetwork())
		    opt->childAdd("el")->setText(list[iA]);
		else list.erase(list.begin()+(iA--));
	    // Output transports directly
	    for(unsigned iA = 0; iA < list.size(); ++iA) {
		at(list[iA]).at().outList(listTrs);
		for(unsigned iATrs = 0; iATrs < listTrs.size(); ++iATrs)
		    opt->childAdd("el")->setText(list[iA]+"."+listTrs[iATrs]);
	    }
	    // Input no protocol transports for the remote hosts initiative connections of they control at the connections
	    for(unsigned iA = 0; iA < list.size(); ++iA) {
		at(list[iA]).at().inList(listTrs);
		for(unsigned iATrs = 0; iATrs < listTrs.size(); ++iATrs)
		    if(at(list[iA]).at().inAt(listTrs[iATrs]).at().protocols().empty())
			opt->childAdd("el")->setText(list[iA]+"."+STR_IN_PREF+listTrs[iATrs]);
	    }
	}
    }
    else if(a_path == "/sub/ehost") {
	bool sysHostAcs = SYS->security().at().access(u, SEC_WR, "root", STR_ID, RWRWR_);
	if(ctrChkNode(opt,"get",RWRWRW,"root",STR_ID,SEC_RD)) {
	    XMLNode *nId	= ctrMkNode("list",opt,-1,"/sub/ehost/id","",RWRWRW,"root",STR_ID);
	    XMLNode *nNm	= ctrMkNode("list",opt,-1,"/sub/ehost/name","",RWRWRW,"root",STR_ID);
	    XMLNode *nTr	= ctrMkNode("list",opt,-1,"/sub/ehost/transp","",RWRWRW,"root",STR_ID);
	    XMLNode *nAddr	= ctrMkNode("list",opt,-1,"/sub/ehost/addr","",RWRWRW,"root",STR_ID);
	    XMLNode *nUser	= ctrMkNode("list",opt,-1,"/sub/ehost/user","",RWRWRW,"root",STR_ID);
	    XMLNode *nPass	= ctrMkNode("list",opt,-1,"/sub/ehost/pass","",RWRWRW,"root",STR_ID);
	    XMLNode *nMode	= sysHostAcs ? ctrMkNode("list",opt,-1,"/sub/ehost/mode","",RWRW__,"root",STR_ID) : NULL;
	    XMLNode *nUpRiseLev	= ctrMkNode("list",opt,-1,"/sub/ehost/upRiseLev","",RWRWRW,"root",STR_ID);

	    vector<ExtHost> list;
	    string tVl;
	    extHostList(u, list, ((tVl=opt->attr("upRiseSYS")).size()?(bool)s2i(tVl):(bool)nMode),
				 ((tVl=opt->attr("upRiseLev")).size()?s2i(tVl):-1), opt->attr("lang"));
	    for(unsigned iH = 0; iH < list.size(); iH++) {
		ExtHost &host = list[iH];
		if(nId)	nId->childAdd("el")->setText(host.id);
		if(nNm)	nNm->childAdd("el")->setText(trD(host.name));
		if(nTr)	nTr->childAdd("el")->setAttr("dest","select")->setAttr("select","/sub/transps-"+host.id)->setText(host.transp);
		if(nAddr) {
		    if(TSYS::strParse(host.transp,1,".").find(STR_IN_PREF) == 0)
			tVl = _("Initial connection identifier to what bind for control.");
		    else if(modPresent(tVl=TSYS::strParse(host.transp,0,".")))
			tVl = at(tVl).at().outAddrHelp();
		    else tVl = "";
		    nAddr->childAdd("el")->setAttr("help",tVl)->setText(host.addr);
		}
		if(nUser) nUser->childAdd("el")->setText(host.user);
		if(nPass) nPass->childAdd("el")->setText(host.pass.size() ? "*******" : "");
		if(nMode) nMode->childAdd("el")->setText(i2s(host.mode));
		if(nUpRiseLev)	nUpRiseLev->childAdd("el")->setText(i2s(host.upRiseLev));
	    }
	}
	if(ctrChkNode(opt,"add",RWRWRW,"root",STR_ID,SEC_WR))
	    extHostSet(ExtHost(u,"newHost",trDSet("",_("New external host")),"","",u));
	if(ctrChkNode(opt,"del",RWRWRW,"root",STR_ID,SEC_WR)) {
	    if(TSYS::strParse(opt->attr("key_id"),1,".").size())
		throw err_sys(_("Lifted hosts are not available for control here!"));
	    extHostDel(u, opt->attr("key_id"), sysHostAcs);
	}
	if(ctrChkNode(opt,"set",RWRWRW,"root",STR_ID,SEC_WR)) {
	    if(TSYS::strParse(opt->attr("key_id"),1,".").size())
		throw err_sys(_("Lifted hosts are not available for control here!"));
	    string col   = opt->attr("col");
	    ExtHost host = extHostGet(u, opt->attr("key_id"), sysHostAcs);
	    if(col == "id") {
		if(extHostGet(u,opt->text(),sysHostAcs).id.size())
		    throw err_sys(_("The host '%s' is presented already!"), opt->text().c_str());
		host.id = opt->text();
		extHostDel(u, opt->attr("key_id"), sysHostAcs);
	    }
	    else if(col == "name")	host.name = trDSet(host.name, opt->text());
	    else if(col == "transp")	host.transp = opt->text();
	    else if(col == "addr")	host.addr = opt->text();
	    else if(col == "user")	host.user = opt->text();
	    else if(col == "pass") {
		if(opt->text().find(SEC_HASH_MAGIC) == 0)
		    host.pass = opt->text().substr(strlen(SEC_HASH_MAGIC));
		else host.pass = opt->text();
	    }
	    else if(col == "mode")	host.mode = s2i(opt->text());
	    else if(col == "upRiseLev")	host.upRiseLev = vmax(0, vmin(9,s2i(opt->text())));
	    host.mdf = SYS->sysTm();
	    extHostSet(host, sysHostAcs);
	}
    }
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTypeTransport                               *
//************************************************
TTypeTransport::TTypeTransport( const string &id ) : TModule(id), mOutLifeTime(0)
{
    mIn = grpAdd(STR_IN_PREF);
    mOut = grpAdd(STR_OUT_PREF);
}

TTypeTransport::~TTypeTransport()
{
    nodeDelAll();
}

TTransportS &TTypeTransport::owner( ) const	{ return (TTransportS&)TModule::owner(); }

string TTypeTransport::inAdd( const string &iid, const string &idb )
{
    return chldAdd(mIn, In(TSYS::strEncode(sTrm(iid),TSYS::oscdID),idb));
}

string TTypeTransport::outAdd( const string &iid, const string &idb )
{
    return chldAdd(mOut, Out(TSYS::strEncode(sTrm(iid),TSYS::oscdID),idb));
}

string TTypeTransport::optDescr( )
{
    return TSYS::strMess(_(
	"======================= Module <%s:%s> options =======================\n"
	"---- Parameters of the module section '%s' of the configuration file ----\n"
	"OutLifeTime  <seconds>  Output transports lifetime (by default 0 seconds), 0 to disable the function.\n\n"
	), mModType.c_str(), modId().c_str(), nodePath().c_str());
}

void TTypeTransport::cntrCmdProc( XMLNode *opt )
{
    vector<string> list;
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/" STR_IN_PREF,_("Input transport"),RWRWR_,"root",STR_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	ctrMkNode("grp",opt,-1,"/br/" STR_OUT_PREF,_("Output transport"),RWRWR_,"root",STR_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	if(ctrMkNode("area",opt,0,"/tr",_("Transports"))) {
	    ctrMkNode("list",opt,-1,"/tr/in",_("Input"),RWRWR_,"root",STR_ID,5,
		"tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref",STR_IN_PREF,"idSz",i2s(limObjID_SZ).c_str());
	    if(ctrMkNode("area",opt,-1,"/tr/out",_("Output"))) {
		ctrMkNode("list",opt,-1,"/tr/out/list",_("List"),RWRWR_,"root",STR_ID,5,
		    "tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref",STR_OUT_PREF,"idSz",i2s(limObjID_SZ).c_str());
		ctrMkNode("fld",opt,-1,"/tr/out/keepAlive",_("Lifetime, seconds"),RWRWR_,"root",STR_ID,4,"tp","dec","min","0","max","3600",
		    "help",_("Time of inactivity in the output transport for it closing/disconnection. Set to 0 to disable the transports closing!"));
	    }
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == ("/br/" STR_IN_PREF) || a_path == "/tr/in") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD)) {
	    inList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setAttr("id",list[iA])->setText(trD(inAt(list[iA]).at().name()));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",STR_ID,SEC_WR))	{ opt->setAttr("id", inAdd(opt->attr("id"))); inAt(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",STR_ID,SEC_WR))	inDel(opt->attr("id"),true);
    }
    else if(a_path == ("/br/" STR_OUT_PREF) || a_path == "/tr/out" || a_path == "/tr/out/list") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD)) {
	    outList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setAttr("id",list[iA])->setText(trD(outAt(list[iA]).at().name()));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",STR_ID,SEC_WR))	{ opt->setAttr("id", outAdd(opt->attr("id"))); outAt(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",STR_ID,SEC_WR))	outDel(opt->attr("id"),true);
    }
    if(a_path == "/tr/out/keepAlive") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(outLifeTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setOutLifeTime(s2i(opt->text()));
    }
    else TModule::cntrCmdProc(opt);
}

void TTypeTransport::load_( )
{
    //Load parameters
    setOutLifeTime(s2i(TBDS::genPrmGet(nodePath()+"OutLifeTime",i2s(outLifeTime()))));
}

void TTypeTransport::save_( )
{
    //Save parameters
    TBDS::genPrmSet(nodePath()+"OutLifeTime",i2s(outLifeTime()));
}

void TTypeTransport::perSYSCall( unsigned int cnt )
{
    //Check all output transports
    vector<string> ls;
    outList(ls);
    for(unsigned iL = 0; outLifeTime() && iL < ls.size(); iL++) {
	AutoHD<TTransportOut> outTr = outAt(ls[iL]);

	int reRs = 1;
	bool toStop = (outTr.at().startStat() && (reRs=outTr.at().reqRes().tryLock()) == 0 &&
			    (TSYS::curTime()-outTr.at().lstReqTm())/1000000 > outLifeTime());
	if(reRs == 0) outTr.at().reqRes().unlock();
	if(toStop) try { outTr.at().stop(); } catch(TError &err) { }
    }
}

//************************************************
//* TTransportIn				 *
//************************************************
TTransportIn::TTransportIn( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), runSt(false), mId(cfg("ID")), mStart(cfg("START").getBd()), mDB(idb), associateTrRes(true),
    mLogLen(0), mLogItLim(1000), mLogLstDt(0), mLogLstDtTm(0), mLogTp(TTransportS::LTP_BinaryText)
{
    mId = iid;
}

TTransportIn::~TTransportIn( )	{ }

void TTransportIn::preEnable( int flag )
{
    cfg("MODULE").setS(owner().modId());
    try{ load(); } catch(...){ }
}

void TTransportIn::postDisable( int flag )
{
    if(!(flag&NodeRemoveOnlyStor))
	try{ stop(); } catch(...){ }		//Stop at any disabling
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor), SYS->transport().at().nodePath()+tbl(), *this, TBDS::UseAllKeys);

	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

bool TTransportIn::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR" && co.getS() != pc.getS()) stop();	//By the address change and reconnect need ordinary
    modif();

    return true;
}

TCntrNode &TTransportIn::operator=( const TCntrNode &node )
{
    const TTransportIn *src_n = dynamic_cast<const TTransportIn*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "ID;");
    cfg("MODULE").setS(owner().modId());
    setDB(src_n->DB());
    load_();

    return *this;
}

TTypeTransport &TTransportIn::owner( ) const	{ return *(TTypeTransport*)nodePrev(); }

string TTransportIn::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TTransportIn::workId( )		{ return owner().modId()+"."+id(); }

string TTransportIn::tbl( ) const	{ return owner().owner().subId()+"_in"; }

string TTransportIn::getStatus( )	{ return startStat() ? _("Receiving data. ") : _("Disconnected. "); }

void TTransportIn::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
}

void TTransportIn::save_( )
{
    TBDS::dataSet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
    setDB(DB(), true);
}

void TTransportIn::stop( )
{
    //Removing the associated output transports
    MtxAlloc resN(associateTrRes, true);
    string oTrId;
    while(mAssociateTrO.size()) {
	oTrId = mAssociateTrO.back().at().id();
	mAssociateTrO.pop_back();
	try { owner().outDel(oTrId); }
	catch(TError &er) { mess_sys(TMess::Error, _("Error deletion the node: %s"), er.mess.c_str()); }
    }
}

vector<AutoHD<TTransportOut> > TTransportIn::associateTrs( bool checkForCleanDisabled )
{
    vector<AutoHD<TTransportOut> > rez;

    MtxAlloc resN(associateTrRes, true);

    //Find proper for clean up stopped transports
    if(checkForCleanDisabled)
	for(int iAss = 0; iAss < (int)mAssociateTrO.size(); iAss++) {
	    bool isFree = mAssociateTrO[iAss].freeStat();
	    if(!(isFree || !mAssociateTrO[iAss].at().startStat() ||
		    (keepAliveTm() && (TSYS::curTime()-mAssociateTrO[iAss].at().lstReqTm())/1000000 > keepAliveTm())))
		continue;
	    if(!isFree) {
		string oTrId = mAssociateTrO[iAss].at().id();
		mAssociateTrO[iAss].free();
		try { owner().outDel(oTrId); }
		catch(TError &er) {
		    mAssociateTrO[iAss] = owner().outAt(oTrId);
		    mess_sys(TMess::Error, _("Error deletion the node: %s"), er.mess.c_str());
		    continue;
		}
	    }
	    mAssociateTrO.erase(mAssociateTrO.begin()+iAss);
	    iAss--;
	}

    rez = mAssociateTrO;
    resN.unlock();

    return rez;
}

AutoHD<TTransportOut> TTransportIn::associateTr( const string &id )
{
    MtxAlloc resN(associateTrRes, true);

    for(int iAss = 0; iAss < (int)mAssociateTrO.size(); ++iAss)
	if(mAssociateTrO[iAss].at().conPrm("initConID").getS() == id && mAssociateTrO[iAss].at().startStat())
	    return mAssociateTrO[iAss];

    throw err_sys(_("The initiative connection '%s' is not presented!"), id.c_str());
}

void TTransportIn::setLogLen( int vl )
{
    MtxAlloc res(mLogRes, true);

    vl = vmax(0, vmin(10000,vl));
    if(vl && !mLogLen) mLogLstDt = 0;
    while((int)mLog.size() > vl) mLog.pop_back();

    mLogLen = vl;
}

void TTransportIn::pushLogMess( const string &vl, const string &data, int dataDir )
{
    MtxAlloc res(mLogRes, true);

    if(!logLen()) return;

    if(mLog.size() && dataDir && dataDir == mLogLstDt && (SYS->sysTm()-mLogLstDtTm) < prmWait_TM)
	mLog[0] += data;
    else {
	mLog.push_front(ll2s(TSYS::curTime()) + "\n" + vl + data);
	while((int)mLog.size() > logLen())	mLog.pop_back();
	mLogLstDtTm = SYS->sysTm();
    }
    mLogLstDt = dataDir;
}

string TTransportIn::associateTrO( const string &addr )
{
    MtxAlloc resN(associateTrRes, true);

    //Creating for new associated output transport
    string associateTrID = "inA_"+id()+"_0";
    string initConID;
    while(owner().outPresent(associateTrID)) associateTrID = TSYS::strLabEnum(associateTrID);
    owner().outAdd(associateTrID);
    mAssociateTrO.push_back(owner().outAt(associateTrID));

    mAssociateTrO.back().at().setAddr(addr);
    mAssociateTrO.back().at().setName("");
    mAssociateTrO.back().at().setDscr("");
    mAssociateTrO.back().at().clearConPrm();
    mAssociateTrO.back().at().modifGClr();
    try {
	mAssociateTrO.back().at().start();

	// Reading and registering the initial connection ID
	char buf[prmStrBuf_SZ];
	int respLen = mAssociateTrO.back().at().messIO("", 0, buf, sizeof(buf));
	initConID = string(buf, respLen);
	mAssociateTrO.back().at().setConPrm("initConID", initConID);
    } catch(TError &er) { mess_sys(TMess::Error, _("Error connection the new associated node: %s"), er.mess.c_str()); }

    //Removing already freed connections also with the same ID for the last connected
    for(int iAss = 0; iAss < (int)mAssociateTrO.size()-1; iAss++) {
	if(mAssociateTrO[iAss].at().startStat() && mAssociateTrO[iAss].at().conPrm("initConID").getS() != initConID) continue;
	// Try to disconnect and remove
	string oTrId = mAssociateTrO[iAss].at().id();
	mAssociateTrO[iAss].free();
	try {
	    owner().outDel(oTrId);
	    mAssociateTrO.erase(mAssociateTrO.begin()+iAss);
	    iAss--;
	} catch(TError &er) {
	    mAssociateTrO[iAss] = owner().outAt(oTrId);
	    mess_sys(TMess::Warning, _("Error removing the associated node/connection: %s"), er.mess.c_str());
	}
    }

    return mAssociateTrO.back().at().id();
}

TVariant TTransportIn::objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang )
{
    // string writeTo(string sender, string mess) - send the message <mess> to sender <sender>
    //  sender - sender address
    //  mess - message for send
    if(id == "writeTo" && prms.size() >= 2) {
	try { return writeTo(prms[0].getS(), prms[1].getS()); }	catch(TError&) { }
	return 0;
    }
    // string status() - the transport status
    if(id == "status") return getStatus();
    // string addr( string vl = "" ) - the transport address return, set the to no empty <vl>
    if(id == "addr") {
	if(prms.size() && prms[0].getS().size())
	    try{ setAddr(prms[0].getS()); } catch(TError&) { }
	return addr();
    }
    // TArrayObj associateTrsList() - associated output transports list to the input
    if(id == "associateTrsList" || id == "assTrsList") {
	TArrayObj *rez = new TArrayObj();
	vector<AutoHD<TTransportOut> > trs = associateTrs();
	for(unsigned iTr = 0; iTr < trs.size(); iTr++) rez->arSet(iTr, trs[iTr].at().id());
	return rez;
    }
    // TCntrNodeObj associateTr(string id) - Getting the associated output transport at that connection <id>.
    if(id == "associateTr" && prms.size() >= 1) {
	try { return TVariant(new TCntrNodeObj(associateTr(prms[0].getS()),user_lang)); } catch(TError&) { }

	return false;
    }

    return TCntrNode::objFuncCall(id, prms, user_lang);

    //Configuration functions call
    TVariant cfRez = objFunc(id, prms, user_lang, RWRWR_, "root:" STR_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(id, prms, user_lang);
}

void TTransportIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Input transport: ")+name()).c_str(),RWRWR_,"root",STR_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),R_R_R_,"root",STR_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Connect"),RWRWR_,"root",STR_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB"),RWRWR_,"root",STR_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labStor().c_str());
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/prm/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",STR_ID,
			1,"help",(DB(true)=="*.*")?TMess::labStorRemGenStor().c_str():"");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",STR_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/MODULE");
		ctrMkNode2("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,RWRWR_,"root",STR_ID,"dest","select","select","/prm/cfg/p_mod",NULL);
	    }
	}
	if(ctrMkNode("area",opt,-1,"/log",_("IO log"),R_R___,"root",STR_ID)) {
	    ctrMkNode("fld",opt,-1,"/log/logLen",_("Log length, block limit, type"),RWRW__,"root",STR_ID,4,"tp","dec", "min","0", "max","10000",
		"help",_("Use zero for the log disabling."));
	    ctrMkNode("fld",opt,-1,"/log/logItLim","",RWRW__,"root",STR_ID,3,"tp","dec", "min","1000", "max","1000000");
	    ctrMkNode("fld",opt,-1,"/log/logTp","",RWRW__,"root",STR_ID,4,"tp","dec",
		"dest","select",
		"sel_id",TSYS::strMess("%d;%d;%d",TTransportS::LTP_BinaryText,TTransportS::LTP_Binary,TTransportS::LTP_Text).c_str(),
		"sel_list",_("Binary & Text;Binary;Text"));
	    if(logLen())
		ctrMkNode("fld",opt,-1,"/log/log",_("Log"),R_R___,"root",STR_ID,4,"tp","str", "rows","20", "cols","100", "SnthHgl","1");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/status" && ctrChkNode(opt))		opt->setText(getStatus());
    else if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(runSt?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	s2i(opt->text())?start():stop();
    }
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/prm/cfg/p_mod" && ctrChkNode(opt)) {
	string curPrt = protocols();
	vector<string> mLst, itLst;
	SYS->protocol().at().modList(mLst);
	// Single protocol directly
	for(unsigned iM = 0; curPrt.size() && iM < mLst.size(); iM++) {
	    SYS->protocol().at().at(mLst[iM]).at().itemListIn(itLst);
	    opt->childAdd("el")->setText(mLst[iM]);
	    for(unsigned iIt = 0; iIt < itLst.size(); iIt++)
		opt->childAdd("el")->setText(mLst[iM]+"."+itLst[iIt]);
	}
	opt->childAdd("el")->setText("");
	// Combinated protocol
	for(unsigned iM = 0; iM < mLst.size(); iM++) {
	    SYS->protocol().at().at(mLst[iM]).at().itemListIn(itLst);
	    if(!TRegExp("(^|;)"+mLst[iM]+"(;|$)","m").test(curPrt))
		opt->childAdd("el")->setText((curPrt.size()?curPrt+";":"")+mLst[iM]);
	    for(unsigned iIt = 0; iIt < itLst.size(); iIt++)
		if(!TRegExp("(^|;)"+mLst[iM]+"\\."+itLst[iIt]+"(;|$)","m").test(curPrt))
		    opt->childAdd("el")->setText((curPrt.size()?curPrt+";":"")+mLst[iM]+"."+itLst[iIt]);
	}
	opt->childAdd("el")->setText("");
    }
    else if(a_path.find("/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",STR_ID,RWRWR_);
    else if(a_path == "/log/logLen") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(i2s(logLen()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setLogLen(s2i(opt->text()));
    }
    else if(a_path == "/log/logItLim") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(i2s(logItLim()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setLogItLim(s2i(opt->text()));
    }
    else if(a_path == "/log/logTp") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(i2s(mLogTp));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	mLogTp = s2i(opt->text());
    }
    else if(a_path == "/log/log") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD)) {
	    MtxAlloc res(mLogRes, true);
	    time_t stTm = time(NULL);
	    string resLog;
	    for(int iL = mLog.size()-1; iL >= 0 && (time(NULL)-stTm) <= prmWait_TM; iL--) {
		int off = 0;
		int64_t itTm   = s2ll(TSYS::strLine(mLog[iL],0,&off));
		string  itDscr = TSYS::strLine(mLog[iL], 0, &off);
		resLog += TSYS::strMess("[%s.%06d] ",atm2s(itTm/1000000,"%Y-%m-%dT%H:%M:%S").c_str(),itTm%1000000) + itDscr;
		if(mLogTp == TTransportS::LTP_Binary || mLogTp == TTransportS::LTP_BinaryText)
		    resLog += ((off<(int)mLog[iL].size())?"\n"+TSYS::strDecode(mLog[iL].substr(off,logItLim()),TSYS::Bin,(mLogTp==TTransportS::LTP_Binary)?" ":"<text>"):"")
			   + (((mLog[iL].size()-off)>logItLim())?(string("\n...<")+_("CUT")+" "+TSYS::cpct2str(mLog[iL].size()-off-logItLim())+">"):string("")) + "\n\n";
		else if(mLogTp == TTransportS::LTP_Text)
		    resLog += ((off<(int)mLog[iL].size())?"\n"+mLog[iL].substr(off,logItLim()):"")
			   + (((mLog[iL].size()-off)>logItLim())?(string("\n...<")+_("CUT")+" "+TSYS::cpct2str(mLog[iL].size()-off-logItLim())+">"):string("")) + "\n\n";
	    }
	    opt->setText(resLog);
	}
	if(ctrChkNode(opt,"SnthHgl",R_R___,"root",STR_ID,SEC_RD)) {
	    opt->setAttr("font", SYS->ui().at().fontSnthHglCode());
	    opt->childAdd("rule")->setAttr("expr","^\\[[^\\]]+.+$")->setAttr("color","blue")->setAttr("font_italic","1")->
		childAdd("rule")->setAttr("expr","^\\[[^\\]]+\\]")->setAttr("color","darkblue")->setAttr("font_weight","1");
	    //opt->childAdd("rule")->setAttr("expr","^[0-9a-fA-F]{2,} .+$")->setAttr("color","black");
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* TTransportOut                                *
//************************************************
TTransportOut::TTransportOut( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), runSt(false), mDefTimeouts(true), mLstReqTm(0), mId(cfg("ID")), mDB(idb), mStartTm(0), mReqRes(true),
    mLogLen(0), mLogItLim(1000), mLogLstDt(0), mLogLstDtTm(0), mLogTp(TTransportS::LTP_BinaryText)
{
    mId = iid;
}

TTransportOut::~TTransportOut( )	{ }

bool TTransportOut::isNetwork( )	{ return owner().isNetwork(); }

TCntrNode &TTransportOut::operator=( const TCntrNode &node )
{
    const TTransportOut *src_n = dynamic_cast<const TTransportOut*>(&node);
    if(!src_n) return *this;

    exclCopy(*src_n, "ID;");
    cfg("MODULE").setS(owner().modId());
    setDB(src_n->DB());
    load_();

    return *this;
}

TTypeTransport &TTransportOut::owner( ) const	{ return *(TTypeTransport*)nodePrev(); }

string TTransportOut::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TTransportOut::workId( )		{ return owner().modId()+"."+id(); }

string TTransportOut::tbl( ) const	{ return owner().owner().subId()+"_out"; }

TVariant TTransportOut::conPrm( const string &nm )
{
    MtxAlloc res(dataRes(), true);
    map<string, TVariant>::iterator iprm = mConPrms.find(nm);
    if(iprm == mConPrms.end())	return TVariant();
    return iprm->second;
}

void TTransportOut::setConPrm( const string &nm, const TVariant &vl )
{
    dataRes().lock();
    mConPrms[nm] = vl;
    dataRes().unlock();
}

void TTransportOut::clearConPrm( )
{
    dataRes().lock();
    mConPrms.clear();
    dataRes().unlock();
}

void TTransportOut::start( int time )	{ mStartTm = SYS->sysTm(); mLogLstDt = 0; }

void TTransportOut::postDisable( int flag )
{
    if(!(flag&NodeRemoveOnlyStor))
	try{ stop(); } catch(...){ }		//Stop at any disabling
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor) ,SYS->transport().at().nodePath()+tbl(), *this, TBDS::UseAllKeys);
	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

bool TTransportOut::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR") stop();	//By the address change and reconnect need ordinary
    modif();

    return true;
}

string TTransportOut::getStatus( )
{
    return (startStat()?_("Connected. "):_("Disconnected. ")) +
	TSYS::strMess(_("Established %s, last %s. "), atm2s(startTm(),"%d-%m-%Y %H:%M:%S").c_str(), atm2s(1e-6*lstReqTm(),"%d-%m-%Y %H:%M:%S").c_str()) +
	(conPrm("initConID").isNull()?string(""):TSYS::strMess(_("Initiative connection ID '%s'. "),conPrm("initConID").getS().c_str()));
}

void TTransportOut::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
}

void TTransportOut::save_( )
{
    TBDS::dataSet(fullDB(), SYS->transport().at().nodePath()+tbl(), *this);
    setDB(DB(), true);
}

void TTransportOut::preEnable( int flag )
{
    cfg("MODULE").setS(owner().modId());
    try{ load(); } catch(...){ }
}

void TTransportOut::messProtIO( XMLNode &io, const string &prot )
{
    if(!SYS->protocol().at().modPresent(prot)) throw err_sys(_("Transport protocol '%s' is missing"), prot.c_str());
    SYS->protocol().at().at(prot).at().outMess(io, *this);
}

void TTransportOut::setLogLen( int vl )
{
    MtxAlloc res(mLogRes, true);

    vl = vmax(0, vmin(10000,vl));
    if(vl && !mLogLen) mLogLstDt = 0;
    while((int)mLog.size() > vl) mLog.pop_back();

    mLogLen = vl;
}

void TTransportOut::pushLogMess( const string &vl, const string &data, int dataDir )
{
    MtxAlloc res(mLogRes, true);

    if(!logLen()) return;

    if(mLog.size() && dataDir && dataDir == mLogLstDt && (SYS->sysTm()-mLogLstDtTm) < prmWait_TM)
	mLog[0] += data;
    else {
	mLog.push_front(ll2s(TSYS::curTime()) + "\n" + vl + data);
	while((int)mLog.size() > logLen())	mLog.pop_back();
	mLogLstDtTm = SYS->sysTm();
    }
    mLogLstDt = dataDir;
}

TVariant TTransportOut::objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang )
{
    // string messIO( string mess, real timeOut = 0, int inBufLen = -1 ) -
    //    sending the message <mess> through the transport with the waiting timeout <timeOut> and reading for <inBufLen> bytes.
    //  mess - message text for send
    //  timeOut - connection timeout, in seconds. Set to "< -1e-3" for the no request mode
    //  inBufLen - input buffer length, < 0 - prmStrBuf_SZ(10000), 0 - no read but only write, > 0 - read pointed bytes
    if(id == "messIO" && prms.size() >= 1 && prms[0].type() != TVariant::Object) {
	string rez;
	int inBufLen = (prms.size() < 3 || prms[2].getI() < 0) ? prmStrBuf_SZ : vmin(prmStrBuf_SZ,prms[2].getI());
	char buf[inBufLen];
	try {
	    if(!startStat()) start();
	    int respLen = messIO(prms[0].getS().data(), prms[0].getS().size(), buf, inBufLen, (prms.size()>=2) ? (int)(1e3*prms[1].getR()) : 0);
	    if(inBufLen && respLen) rez.assign(buf, respLen);
	} catch(TError&) { return ""; }

	return rez;
    }
    // string messIO( XMLNodeObj req, string prt ) - sending the request <req> to the protocol <prt> for the implementation of a connection
    //      session through the transport by means of protocol.
    //  req - request into XML-tree
    //  prt - protocol name
    if(id == "messIO" && prms.size() >= 2 && !AutoHD<XMLNodeObj>(prms[0].getO()).freeStat()) {
	try {
	    XMLNode req;
	    if(!startStat()) start();
	    AutoHD<XMLNodeObj>(prms[0].getO()).at().toXMLNode(req);
	    messProtIO(req, prms[1].getS());
	    AutoHD<XMLNodeObj>(prms[0].getO()).at().fromXMLNode(req);
	} catch(TError &err) { return err.mess; }
	return "";
    }
    // bool isNetwork( ) - the transport network type
    if(id == "isNetwork") return isNetwork();
    // string status( ) - the transport status
    if(id == "status")	return getStatus();
    // bool start( bool vl = <EVAL>, int tm = 0 ) - the transport start status return, start/stop it by <vl> (if it is not <EVAL>).
    //  For starting you can set the specific timeout //tm//.
    if(id == "start") {
	char com = prms.size() ? prms[0].getB() : EVAL_BOOL;
	if(com == EVAL_BOOL) return startStat();
	try {
	    if(!com && startStat())	stop();
	    else if(com && !startStat())start((prms.size()>=2)?prms[1].getI():0);
	} catch(TError&) { }

	return startStat();
    }
    // string addr( string vl = "" ) - the transport address return, set it to not empty <vl>
    if(id == "addr") {
	if(prms.size() && prms[0].getS().size())
	    try{ setAddr(prms[0].getS()); } catch(TError&) { }
	return addr();
    }
    // string timings( string vl = "", isDef = true ) - the transport timings return, set it to not empty <vl>
    if(id == "timings") {
	if(prms.size() && prms[0].getS().size())
	    try{ setTimings(prms[0].getS(), (prms.size()>=2)?prms[1].getB():true); }
	    catch(TError&) { }
	return timings();
    }
    // int attempts( int vl = EVAL ) - the transport requesting attempts return, set it to not EVAL <vl>
    if(id == "attempts") {
	if(prms.size() && prms[0].getI() != EVAL_INT)
	    try{ setAttempts(prms[0].getI()); } catch(TError&) { }
	return attempts();
    }
    // ElTp conPrm( string nm ) - getting the connection parameter <nm>
    //  nm - connection parameter name.
    if(id == "conPrm" && prms.size() >= 1)
	return conPrm(prms[0].getS());
    // bool setConPrm( string nm, ElTp val ) - setting the connection parameter <nm> to the <val>
    //  nm - connection parameter name;
    //  val - parameter value.
    if(id == "setConPrm" && prms.size() >= 2) {
	setConPrm(prms[0].getS(), prms[1]);
	return true;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(id, prms, user_lang, RWRWR_, "root:" STR_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(id, prms, user_lang);
}

void TTransportOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",(_("Output transport: ")+name()).c_str(),RWRWR_,"root",STR_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Transport"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),R_R_R_,"root",STR_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Connect"),RWRWR_,"root",STR_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Transport DB"),RWRWR_,"root",STR_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labStor().c_str());
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/prm/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",STR_ID,
			1,"help",(DB(true)=="*.*")?TMess::labStorRemGenStor().c_str():"");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",STR_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/MODULE");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/req",_("Request"),RWRW__,"root",STR_ID)) {
	    ctrMkNode("fld",opt,-1,"/req/tm",_("Time"),R_R___,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/req/mode",_("Mode"),RWRW__,"root",STR_ID,4,"tp","dec","dest","select",
		"sel_id","0;1;2;3","sel_list",_("Binary;Text(LF);Text(CR);Text(CR/LF)"));
	    ctrMkNode("fld",opt,-1,"/req/toTmOut",_("Wait timeout"),RWRWR_,"root",STR_ID,2,"tp","bool","help",
		_("A sign of waiting timeout when receiving an answer.\n"
		  "Many systems, when responding to different protocols (such as HTTP), can send response data in several parts.\n"
		  "Without this, only the first part will be received and displayed.\n"
		  "When this flag is set, all parts of the response will be waiting up to missing data during the transport timeout."));
	    ctrMkNode("fld",opt,-1,"/req/inBufSz",_("Input buffer size, bytes"),RWRW__,"root",STR_ID,4,"tp","dec","min","0","max",i2s(prmStrBuf_SZ).c_str(),
		"help",_("Direct set the input buffer size. Use 0 to disable waiting and reading for a data - only to write."));
	    ctrMkNode("comm",opt,-1,"/req/send",_("Send"),RWRW__,"root",STR_ID);
	    ctrMkNode("fld",opt,-1,"/req/req",_("Request"),RWRW__,"root",STR_ID,4,"tp","str","cols","90","rows","5","SnthHgl","1");
	    ctrMkNode("fld",opt,-1,"/req/answ",_("Answer"),RWRW__,"root",STR_ID,4,"tp","str","cols","90","rows","5","SnthHgl","1");
	}
	if(ctrMkNode("area",opt,-1,"/log",_("IO log"),R_R___,"root",STR_ID)) {
	    ctrMkNode("fld",opt,-1,"/log/logLen",_("Log length, block limit, type"),RWRW__,"root",STR_ID,4,"tp","dec", "min","0", "max","10000",
		"help",_("Use zero for the log disabling."));
	    ctrMkNode("fld",opt,-1,"/log/logItLim","",RWRW__,"root",STR_ID,3,"tp","dec", "min","1000", "max","1000000");
	    ctrMkNode("fld",opt,-1,"/log/logTp","",RWRW__,"root",STR_ID,4,"tp","dec",
		"dest","select",
		"sel_id",TSYS::strMess("%d;%d;%d",TTransportS::LTP_BinaryText,TTransportS::LTP_Binary,TTransportS::LTP_Text).c_str(),
		"sel_list",_("Binary & Text;Binary;Text"));
	    if(logLen())
		ctrMkNode("fld",opt,-1,"/log/log",_("Log"),R_R___,"root",STR_ID,4,"tp","str", "rows","20", "cols","100", "SnthHgl","1");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/status" && ctrChkNode(opt))		opt->setText(getStatus());
    else if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(runSt?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	s2i(opt->text())?start():stop();
    }
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",STR_ID,RWRWR_);
    else if(a_path == "/req/tm" && ctrChkNode(opt,"get",R_R___,"root",STR_ID,SEC_RD))
	opt->setText(TBDS::genPrmGet(owner().nodePath()+"ReqTm","0",opt->attr("user")));
    else if(a_path == "/req/mode") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"ReqMode","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"ReqMode",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/req/toTmOut") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"ToTmOut","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"ToTmOut",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/req/inBufSz") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))
	    opt->setText(TBDS::genPrmGet(owner().nodePath()+"InBufSz",i2s(prmStrBuf_SZ),opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))
	    TBDS::genPrmSet(owner().nodePath()+"InBufSz", i2s(vmax(0,vmin(prmStrBuf_SZ,s2i(opt->text())))), opt->attr("user"));
    }
    else if(a_path == "/req/req") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"ReqReq","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR)) {
	    int mode = s2i(TBDS::genPrmGet(owner().nodePath()+"ReqMode","0",opt->attr("user")));
	    switch(mode) {
		case 0:
		    TBDS::genPrmSet(owner().nodePath()+"ReqReq",
			TSYS::strDecode(TSYS::strEncode(opt->text(),TSYS::Bin),TSYS::Bin," "), opt->attr("user"));
		    break;
		default:
		    TBDS::genPrmSet(owner().nodePath()+"ReqReq",opt->text(), opt->attr("user"));
		    break;
	    }
	}
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",STR_ID,SEC_RD)) {
	    opt->setAttr("font",SYS->ui().at().fontSnthHglCode());
	}
    }
    else if(a_path == "/req/answ") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"ReqAnsw","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"ReqAnsw",opt->text(),opt->attr("user"));
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",STR_ID,SEC_RD)) {
	    opt->setAttr("font",SYS->ui().at().fontSnthHglCode());
	}
    }
    else if(a_path == "/req/send" && ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR)) {
	string answ;
	int mode = s2i(TBDS::genPrmGet(owner().nodePath()+"ReqMode","0",opt->attr("user")));
	string req = TBDS::genPrmGet(owner().nodePath()+"ReqReq","",opt->attr("user"));

	switch(mode) {
	    case 0:	req = TSYS::strEncode(req,TSYS::Bin);	break;
	    case 1:	//TEXT(LF)
		for(size_t i_p = 0; (i_p=req.find("\n",i_p)) != string::npos; i_p++)
		    req.replace(i_p, strlen("\n"), "\x0A");
		req = TSYS::strEncode(req, TSYS::ShieldSimb);
		break;
	    case 2:	//TEXT(CR)
		for(size_t i_p = 0; (i_p=req.find("\n",i_p)) != string::npos; i_p++)
		    req.replace(i_p, strlen("\n"), "\x0D");
		req = TSYS::strEncode(req, TSYS::ShieldSimb);
		break;
	    case 3:	//TEXT(CR/LF)
		for(size_t i_p = 0; (i_p=req.find("\n",i_p)) != string::npos; i_p+=2)
		    req.replace(i_p, strlen("\n"), "\x0D\x0A");
		req = TSYS::strEncode(req, TSYS::ShieldSimb);
		break;
	}

	int64_t stm = TSYS::curTime();
	try {
	    int inBufSz = s2i(TBDS::genPrmGet(owner().nodePath()+"InBufSz",i2s(prmStrBuf_SZ),opt->attr("user")));
	    char buf[inBufSz];
	    if(!startStat()) start();
	    MtxAlloc resN(reqRes(), true);
	    int resp_len = messIO(req.data(), req.size(), buf, inBufSz);
	    if(inBufSz) {
		if(req.size()) answ.assign(buf, resp_len);
		else answ.append(buf, resp_len);

		bool ToTmOut = (bool)s2i(TBDS::genPrmGet(owner().nodePath()+"ToTmOut","0",opt->attr("user")));
		while(ToTmOut && resp_len > 0 && ((TSYS::curTime()-stm)/1000000) < prmInterf_TM) {
		    try { resp_len = messIO(NULL, 0, buf, inBufSz); } catch(TError &err) { break; }
		    answ.append(buf, resp_len);
		}
	    }

	    TBDS::genPrmSet(owner().nodePath()+"ReqTm", tm2s(1e-6*(TSYS::curTime()-stm)), opt->attr("user"));
	    TBDS::genPrmSet(owner().nodePath()+"ReqAnsw", (mode==0)?TSYS::strDecode(answ,TSYS::Bin,"<text>"):answ, opt->attr("user"));
	} catch(TError &err) {
	    TBDS::genPrmSet(owner().nodePath()+"ReqTm", tm2s(1e-6*(TSYS::curTime()-stm)), opt->attr("user"));
	    throw;
	}
    }
    else if(a_path == "/log/logLen") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(i2s(logLen()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setLogLen(s2i(opt->text()));
    }
    else if(a_path == "/log/logItLim") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(i2s(logItLim()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setLogItLim(s2i(opt->text()));
    }
    else if(a_path == "/log/logTp") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(i2s(mLogTp));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	mLogTp = s2i(opt->text());
    }
    else if(a_path == "/log/log") {
	if(ctrChkNode(opt,"get",R_R___,"root",STR_ID,SEC_RD)) {
	    MtxAlloc res(mLogRes, true);
	    time_t stTm = time(NULL);
	    string resLog;
	    for(int iL = mLog.size()-1; iL >= 0 && (time(NULL)-stTm) <= prmWait_TM; iL--) {
		int off = 0;
		int64_t itTm   = s2ll(TSYS::strLine(mLog[iL],0,&off));
		string  itDscr = TSYS::strLine(mLog[iL], 0, &off);
		resLog += TSYS::strMess("[%s.%06d] ",atm2s(itTm/1000000,"%Y-%m-%dT%H:%M:%S").c_str(),itTm%1000000) + itDscr;
		if(mLogTp == TTransportS::LTP_Binary || mLogTp == TTransportS::LTP_BinaryText)
		    resLog += ((off<(int)mLog[iL].size())?"\n"+TSYS::strDecode(mLog[iL].substr(off,logItLim()),TSYS::Bin,(mLogTp==TTransportS::LTP_Binary)?" ":"<text>"):"")
			   + (((mLog[iL].size()-off)>logItLim())?(string("\n...<")+_("CUT")+" "+TSYS::cpct2str(mLog[iL].size()-off-logItLim())+">"):string("")) + "\n\n";
		else if(mLogTp == TTransportS::LTP_Text)
		    resLog += ((off<(int)mLog[iL].size())?"\n"+mLog[iL].substr(off,logItLim()):"")
			   + (((mLog[iL].size()-off)>logItLim())?(string("\n...<")+_("CUT")+" "+TSYS::cpct2str(mLog[iL].size()-off-logItLim())+">"):string("")) + "\n\n";
		    //itDscr + ((off<(int)mLog[iL].size())?"\n"+TSYS::strDecode(mLog[iL].substr(off,logItLim()),TSYS::Bin,"<text>"):"")
		    //	   + (((mLog[iL].size()-off)>logItLim())?(string("\n...<")+_("CUT")+" "+TSYS::cpct2str(mLog[iL].size()-off-logItLim())+">"):string("")) + "\n\n";
	    }
	    opt->setText(resLog);
	}
	if(ctrChkNode(opt,"SnthHgl",R_R___,"root",STR_ID,SEC_RD)) {
	    opt->setAttr("font",SYS->ui().at().fontSnthHglCode());
	    opt->childAdd("rule")->setAttr("expr","^\\[[^\\]]+.+$")->setAttr("color","blue")->setAttr("font_italic","1")->
		childAdd("rule")->setAttr("expr","^\\[[^\\]]+\\]")->setAttr("color","darkblue")->setAttr("font_weight","1");
	    //opt->childAdd("rule")->setAttr("expr","^[0-9a-fA-F]{2,} .+$")->setAttr("color","black");
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

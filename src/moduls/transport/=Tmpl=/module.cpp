
//OpenSCADA module Transport.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// System includings - add need ones
#include <unistd.h>
#include <errno.h>

// OpenSCADA API includings - add need ones
#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>

// Own includings of the module - add need ones
#include "module.h"

//************************************************
//* Module info!                                 *
#define MOD_ID		"Tmpl"
#define MOD_NAME	trS("Transport template")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		trS("MyName MyFamily")
#define DESCRIPTION	trS("Transport's subsystem template module.")
#define LICENSE		"MyLicense"
//************************************************

TrTmpl::TTr *TrTmpl::mod;

// Required section for binding OpenSCADA core to this module, It gives information and creates module root object - do not change
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt tr_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *tr_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new TrTmpl::TTr(source);
	return NULL;
    }
}

using namespace TrTmpl;

//************************************************
//* TTr						 *
//************************************************
TTr::TTr( string name ) : TTypeTransport(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTr::~TTr( )
{

}

void TTr::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	owner().inEl().fldAdd(new TFld("A_PRMS",trS("Addition parameters"),TFld::String,TFld::FullText,"10000"));
	owner().outEl().fldAdd(new TFld("A_PRMS",trS("Addition parameters"),TFld::String,TFld::FullText,"10000"));
    }
}

TTransportIn *TTr::In( const string &id, const string &stor )	{ return new TTrIn(id, stor, &owner().inEl()); }

TTransportOut *TTr::Out( const string &id, const string &stor )	{ return new TTrOut(id, stor, &owner().outEl()); }

string TTr::outAddrHelp( )
{
    return string(_("Output transport has the address format:\n"
	//???? Describe here the output transport address
	));
}

//************************************************
//* TTrIn                                        *
//************************************************
TTrIn::TTrIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), mKeepAliveReqs(0), mKeepAliveTm(60), mTaskPrior(0)
{
    //???? Set here the default input transport address
    setAddr("localhost:10005");
}

TTrIn::~TTrIn( )
{

}

string TTrIn::getStatus( )
{
    string rez = TTransportIn::getStatus();

    //???? Append here the status of the input transport for its specific information
    if(startStat()) {
	//rez += TSYS::strMess(_("Connections %d, opened %d, last %s, closed by the limit %d. "),
	//    connNumb, (protocols().empty()?associateTrs().size():clId.size()), atm2s(lastConn()).c_str(), clsConnByLim);
    }

    return rez;
}

void TTrIn::load_( )
{
    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("KeepAliveReqs"); if(!vl.empty()) setKeepAliveReqs(s2i(vl));
	vl = prmNd.attr("KeepAliveTm");	if(!vl.empty()) setKeepAliveTm(s2i(vl));
	vl = prmNd.attr("TaskPrior");	if(!vl.empty()) setTaskPrior(s2i(vl));
	//???? Append loading the additional configuration attributes
    } catch(...) { }
}

void TTrIn::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("KeepAliveReqs", i2s(keepAliveReqs()));
    prmNd.setAttr("KeepAliveTm", i2s(keepAliveTm()));
    prmNd.setAttr("TaskPrior", i2s(taskPrior()));
    //???? Append saving the additional configuration attributes
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportIn::save_();
}

void TTrIn::start( )
{
    if(runSt) return;

    //???? Init-prepare here the connection properties

    SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this); //main task for processing

    runSt = true;

    TTransportIn::start();

    if(logLen()) pushLogMess(_("Connected"));
}

void TTrIn::stop()
{
    if(!runSt) return;

    SYS->taskDestroy(nodePath('.',true), &endrun);
    runSt = false;

    //???? Clean up here the connection properties

    TTransportIn::stop();

    if(logLen()) pushLogMess(_("Disconnected"));
}

bool TTrIn::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR" && co.getS() != pc.getS()) {
	//???? Process here the reconnection at the address change
    }

    return TTransportIn::cfgChange(co, pc);
}

void *TTrIn::Task( void *tr_in )
{
    TTrIn *tr = (TTrIn *)tr_in;

    tr->runSt	= true;
    tr->endrun	= false;

    while(!tr->endrun) {
	//???? Process here the inbound connections
    }

    tr->runSt = false;

    return NULL;
}

void TTrIn::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TTransportIn::cntrCmdProc(opt);
	ctrRemoveNode(opt, "/prm/cfg/A_PRMS");
	ctrMkNode("fld", opt, -1, "/prm/cfg/ADDR", EVAL_STR, startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "help",
	    _("Input transport has the address format:\n"
		//???? Describe here the input transport address
	    ));
	ctrMkNode("fld", opt, -1, "/prm/cfg/taskPrior", _("Priority"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
		"tp","dec", "help",TMess::labTaskPrior().c_str());
	ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveReqs", _("Keep alive requests"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		"help",_("Closing the connection after the specified requests.\nZero value to disable - do not close ever."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveTm", _("Keep alive timeout, seconds"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		"help",_("Closing the connection after no requests at the specified timeout.\nZero value to disable - do not close ever."));
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/keepAliveReqs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveReqs()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveReqs(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/keepAliveTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveTm(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/taskPrior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(taskPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTaskPrior(s2i(opt->text()));
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TTrOut                                   *
//************************************************
TTrOut::TTrOut(string name, const string &idb, TElem *el) : TTransportOut(name,idb,el), mAttemts(1)
{

}

TTrOut::~TTrOut()
{

}

string TTrOut::getStatus( )
{
    string rez = TTransportOut::getStatus();

    //???? Append here the status of the input transport for its specific information
    if(startStat()) {
	//rez += TSYS::strMess(_("To the host '%s'. "), connAddr.c_str());
    }

    return rez;
}

void TTrOut::setTimings( const string &vl, bool isDef )
{
    if((isDef && !mDefTimeouts) || vl == mTimings) return;
    else if(!isDef) mDefTimeouts = false;

    //???? Process here the timeouts structure for parse

    if(!isDef) modif();
}

void TTrOut::setAttempts( unsigned short vl )
{
    if(vl == mAttemts)	return;
    mAttemts = vmax(1, vmin(5,vl));
    modif();
}

void TTrOut::load_( )
{
    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("tms"); if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("attempts"); if(!vl.empty()) setAttempts(s2i(vl));
	//???? Append loading the additional configuration attributes
    } catch(...) { }
}

void TTrOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("tms", timings());
    prmNd.setAttr("attempts", i2s(attempts()));
    //???? Append saving the additional configuration attributes
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportOut::save_();
}

bool TTrOut::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR" && co.getS() != pc.getS()) {
	//???? Process here the reconnection at the address change
    }

    return TTransportOut::cfgChange(co, pc);
}

void TTrOut::start( int itmCon )
{
    MtxAlloc res(reqRes(), true);

    if(runSt) return;
    if(SYS->stopSignal()) throw TError(nodePath().c_str(), _("We are stopping!"));

    //???? Init-prepare here the connection properties and establish the connection for the connectable transports

    runSt = true;

    TTransportOut::start();

    if(logLen()) pushLogMess(_("Connected"));
}

void TTrOut::stop()
{
    MtxAlloc res(reqRes(), true);

    if(!runSt) return;

    //???? Disconnect for the connectable transports and clean up here the connection properties

    runSt = false;

    TTransportOut::stop();

    if(logLen()) pushLogMess(_("Disconnected"));
}

int TTrOut::messIO( const char *oBuf, int oLen, char *iBuf, int iLen, int time )
{
    int iB = 0;

    MtxAlloc res(reqRes(), true);

    if(!runSt) throw TError(nodePath().c_str(), _("Transport is not connected!"));

    //???? Perform the messages exchenging just here

    return vmax(0, iB);
}

void TTrOut::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TTransportOut::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1, "help",owner().outAddrHelp().c_str());
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,1, "tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/attempts",_("Attempts"),RWRWR_,"root",STR_ID,1, "tp","dec");
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/TMS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else if(a_path == "/prm/cfg/attempts") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(attempts()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setAttempts(s2i(opt->text()));
    }
    else TTransportOut::cntrCmdProc(opt);
}

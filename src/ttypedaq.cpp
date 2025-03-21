
//OpenSCADA file: ttypedaq.cpp
/***************************************************************************
 *   Copyright (C) 2003-2025 by Roman Savochenko, <roman@oscada.org>       *
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


#include <stdio.h>

#include "tsys.h"
#include "tmodule.h"
#include "tmess.h"
#include "tbds.h"
#include "tdaqs.h"
#include "ttypeparam.h"
#include "ttypedaq.h"

using namespace OSCADA;

//*************************************************
//* TTypeDAQ                                      *
//*************************************************
TTypeDAQ::TTypeDAQ( const string &id ) : TModule(id)
{
    mCntr = grpAdd("cntr_");

    fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    fldAdd(new TFld("DESCR",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,i2s(limObjDscr_SZ).c_str()));
    fldAdd(new TFld("ENABLE",trS("To enable"),TFld::Boolean,0,"1","0"));
    fldAdd(new TFld("START",trS("To start"),TFld::Boolean,0,"1","0"));
    fldAdd(new TFld("MESS_LEV",trS("Messages level"),TFld::Integer,0,"1","3"));

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TTypeDAQ::~TTypeDAQ( )
{
    nodeDelAll();

    while(paramt.size()) {
	delete paramt[0];
	paramt.erase(paramt.begin());
    }

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TTypeDAQ::objName( )	{ return TModule::objName()+":TTypeDAQ"; }

string TTypeDAQ::DAQPath( )	{ return modId(); }

TDAQS &TTypeDAQ::owner( ) const	{ return *(TDAQS*)nodePrev(); }

void TTypeDAQ::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(redntAllow()) {
	fldAdd(new TFld("REDNT",trS("Redundant"),TFld::Integer,TFld::Selectable,"1","0",
	    i2s(TController::Off)+";"+i2s(TController::Asymmetric)/*+";"+i2s(TController::Symmetric)*/,
	    trS("Off;Asymmetric"/*;Symmetric"*/)));
	fldAdd(new TFld("REDNT_RUN",trS("Preference for running"),TFld::String,0,"20","<high>"));
    }
}

void TTypeDAQ::modStart( )
{
    vector<string> lst;
    //Start all controllers
    list(lst);
    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
	if(at(lst[i_l]).at().toStart())
	    try{ at(lst[i_l]).at().start(); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error starting the controller '%s'."), (modId()+"."+lst[i_l]).c_str());
	    }
}

void TTypeDAQ::modStop( )
{
    vector<string> lst;
    //Stop all controllers
    list(lst);
    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
	at(lst[i_l]).at().stop();
}

string TTypeDAQ::add( const string &iid, const string &daq_db )
{
    return chldAdd(mCntr, ContrAttach(TSYS::strEncode(sTrm(iid),TSYS::oscdID),daq_db));
}

TTypeParam &TTypeDAQ::tpPrmAt( unsigned id )
{
    if(id >= paramt.size()/* || id < 0*/) throw err_sys(_("Error ID of the parameter type!"));
    return *paramt[id];
}

bool TTypeDAQ::tpPrmPresent( const string &name_t )
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name == name_t) return true;
    return false;
}

int TTypeDAQ::tpParmAdd( const char *id, const char *n_db, const char *name, bool isPrmCntr )
{
    return tpParmAdd(new TTypeParam(id,name,n_db,isPrmCntr));
}

int TTypeDAQ::tpParmAdd( TTypeParam *tp )
{
    if(tpPrmPresent(tp->name)) { delete tp; return tpPrmToId(tp->name); }
    paramt.push_back(tp);

    return paramt.size()-1;
}

int TTypeDAQ::tpPrmToId( const string &name_t )
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name == name_t) return i_t;
    throw err_sys(_("Parameter type '%s' is not present."), name_t.c_str());
}

TController *TTypeDAQ::ContrAttach( const string &name, const string &daq_db )
{
    throw err_sys(_("Error attaching the new controller '%s'."), name.c_str());
}

string TTypeDAQ::compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text, const string &usings, int maxCalcTm )
{
    throw err_sys(_("Module does not support compilation of the programming language."));
}

void TTypeDAQ::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/cntr_",_("Controller"),RWRWR_,"root",SDAQ_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	if(ctrMkNode("area",opt,0,"/tctr",_("Controllers"))) {
	    ctrMkNode("fld",opt,-1,"/tctr/nmb",_("Number"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/tctr/ctr",_("Controllers"),RWRWR_,"root",SDAQ_ID,5,
		"tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref","cntr_","idSz",i2s(limObjID_SZ).c_str());
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/cntr_" || a_path == "/tctr/ctr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    vector<string> c_list;
	    list(c_list);
	    for(unsigned iA = 0; iA < c_list.size(); iA++)
		opt->childAdd("el")->setAttr("id",c_list[iA])->setText(trD(at(c_list[iA]).at().name()));
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))	{ opt->setAttr("id", add(opt->attr("id"))); at(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	chldDel(mCntr, opt->attr("id"), -1, NodeRemove);
    }
    else if(a_path == "/tctr/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	list(c_list);
	unsigned enCnt = 0, stCnt = 0;
	map<int, unsigned> errCnt;
	for(unsigned iCntr = 0; iCntr < c_list.size(); iCntr++) {
	    AutoHD<TController> cntrO = at(c_list[iCntr]);
	    if(cntrO.at().enableStat()) enCnt++;
	    if(cntrO.at().startStat()) {
		stCnt++;
		int errSt = s2i(cntrO.at().getStatus());
		if(errSt) errCnt[errSt]++;
	    }
	}
	string errs;
	for(map<int, unsigned>::iterator iE = errCnt.begin(); iE != errCnt.end(); ++iE)
	    errs += (errs.size()?", ":"") + i2s(iE->first)+"="+i2s(iE->second);
	opt->setText(TSYS::strMess(_("all %d, running %d, enabled %d%s"),
			c_list.size(),stCnt,enCnt,errs.size()?(string("; ")+_("Errors: ")+errs).c_str():""));
    }
    else TModule::cntrCmdProc(opt);
}

//*************************************************
//* TTypeParam                                     *
//*************************************************
TTypeParam::TTypeParam( const char *iid, const char *iname, const char *idb, bool i_isPrmCntr ) :
    name(iid), descr(iname), mDB(idb), isPrmCntr(i_isPrmCntr)
{
    //Add typical structure fields
    fldAdd(new TFld("SHIFR",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    fldAdd(new TFld("OWNER",trS("Owner"),TFld::String,TCfg::Key|TCfg::NoVal|TFld::NoWrite,i2s(limObjID_SZ*5).c_str()));
    fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    fldAdd(new TFld("DESCR",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,i2s(limObjDscr_SZ).c_str()));
    fldAdd(new TFld("EN",trS("To enable"),TFld::Boolean,TCfg::NoVal,"1","0"));
    fldAdd(new TFld("TIMESTAMP",trS("Date of modification"),TFld::Integer,TFld::DateTimeDec|TCfg::NoVal));
}

string TTypeParam::DB( const TController *cntr ) const	{ return mDB.size() ? cntr->cfg(mDB).getS() : cntr->tblStd(*this); }

void TTypeParam::setDB( TController *cntr, const string &vl )	{ if(mDB.size()) cntr->cfg(mDB).setS(vl); }

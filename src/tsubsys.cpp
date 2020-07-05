
//OpenSCADA file: tsubsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#include <stdarg.h>
#include <unistd.h>

#include "tsys.h"
#include "tmess.h"
#include "tsubsys.h"

using namespace OSCADA;

//*************************************************
//* TSubSYS                                       *
//*************************************************
TSubSYS::TSubSYS( const char *id, const char *name, bool modi ) : mStart(false), mModSys(modi), mMod(-1), mId(id), mName(name)
{
    if(subModule()) mMod = grpAdd("mod_", true);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TSubSYS::~TSubSYS( )
{
    nodeDelAll();

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TSubSYS::objName( )	{ return TCntrNode::objName()+":TSubSYS"; }

TSYS &TSubSYS::owner( ) const	{ return *(TSYS*)nodePrev(); }

string TSubSYS::subName( )	{ return mName.size()?_(mName.c_str()):mId; }

void TSubSYS::modList( vector<string> &list )
{
    if(!subModule()) throw err_sys(_("Subsystem is not modular!"));
    chldList(mMod,list);
}

bool TSubSYS::modPresent( const string &name )
{
    if(!subModule()) throw err_sys(_("Subsystem is not modular!"));
    return chldPresent(mMod,name);
}

void TSubSYS::modAdd( TModule *modul )
{
    if(!subModule()) throw err_sys(_("Subsystem is not modular!"));
    if(chldPresent(mMod,modul->modId())) return;
    mess_sys(TMess::Info, _("Module '%s' connecting."), modul->modId().c_str());
    chldAdd(mMod, modul, s2i(modul->modInfo("HighPriority"))?0:-1);

    if(mess_lev() == TMess::Debug) {
	vector<string> list;
	modul->modInfo(list);
	for(unsigned i_opt = 0; i_opt < list.size(); i_opt++)
	    mess_sys(TMess::Debug, "-> %s: %s", _(list[i_opt].c_str()), modul->modInfo(list[i_opt]).c_str());
    }
}

void TSubSYS::modDel( const string &name )
{
    if(!subModule()) throw err_sys(_("Subsystem is not modular!"));
    chldDel(mMod, name);
    mess_sys(TMess::Info, _("Module '%s' disconnecting."), name.c_str());
}

AutoHD<TModule> TSubSYS::modAt( const string &name ) const
{
    if(!subModule()) throw err_sys(_("Subsystem is not modular!"));
    return chldAt(mMod,name);
}

void TSubSYS::unload( )
{
    //Delete all child nodes passes holded
    vector<string> nLs;
    for(int iCh = 0; iCh < grpSize(); iCh++) {
	chldList(iCh, nLs);
	for(unsigned iN = 0; iN < nLs.size(); iN++)
	    try { chldDel(iCh, nLs[iN]); }
	    catch(...) { }
    }
}

void TSubSYS::subStart( )
{
    mess_sys(TMess::Debug, _("Subsystem starting."));

    if(!SYS->security().at().grpPresent(subId())) {
	SYS->security().at().grpAdd(subId());
	SYS->security().at().grpAt(subId()).at().setDescr(subName());
	SYS->security().at().grpAt(subId()).at().setSysItem(true);
	SYS->security().at().grpAt(subId()).at().userAdd("root");
    }

    if(!subModule())	return;
    vector<string> list;
    modList(list);
    AutoHD<TModule> mO;
    for(unsigned iM = 0; iM < list.size(); iM++)
	try {
	    mO = modAt(list[iM]);
	    if(SYS->mainThr.freeStat() || &SYS->mainThr.at() != &mO.at()) mO.at().modStart();
	    if(!SYS->mainThr.freeStat()) SYS->mainThr.at().perSYSCall(0);
	}
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error starting the module '%s'."), list[iM].c_str());
	}

    mStart = true;
}

void TSubSYS::subStop( )
{
    mess_sys(TMess::Debug, _("Subsystem stopping."));

    if(!subModule())	return;
    vector<string> list;
    modList(list);
    AutoHD<TModule> mO;
    for(unsigned iM = 0; iM < list.size(); iM++)
	try {
	    mO = modAt(list[iM]);
	    if(SYS->mainThr.freeStat() || &SYS->mainThr.at() != &mO.at()) mO.at().modStop();
	    if(!SYS->mainThr.freeStat()) SYS->mainThr.at().perSYSCall(0);
	}
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error stopping the module '%s'."), list[iM].c_str());
	}

    mStart = false;
}

void TSubSYS::perSYSCall( unsigned int cnt )
{
    if(!subModule()) return;
    vector<string> list;
    modList(list);
    for(unsigned iM = 0; iM < list.size(); iM++)
	try{ modAt(list[iM]).at().perSYSCall(cnt); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

string TSubSYS::optDescr( )
{
    if(!subModule()) return "";
    string rez;

    vector<string> list;
    modList(list);
    for(unsigned iM = 0; iM < list.size(); iM++)
	rez += modAt(list[iM]).at().optDescr();

    return rez;
}

void TSubSYS::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path == "/serv/redundant") {	//Redundant service requests
	if(ctrChkNode(opt,"st",RWRWR_,"root",SDAQ_ID,SEC_RD)) {	//State
	    opt->setAttr("inProc", "0");
	    return;
	}
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Subsystem: ")+subName(),R_R_R_);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	if(TUIS::icoGet(subId(),NULL,true).size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(subModule()) {
	    ctrMkNode("grp",opt,-1,"/br/mod_",_("Module"),R_R_R_,"root","root",1,"idm","1");
	    if(ctrMkNode("area",opt,-1,"/mod",_("Modules"),R_R_R_,"root","root"))
		ctrMkNode("list",opt,-1,"/mod/br",_("Modules"),R_R_R_,"root","root",3,"tp","br","idm","1","br_pref","mod_");
	}
	return;
    }

    //Process command to page
    if(a_path == "/ico" && ctrChkNode(opt)) {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(subId(),&itp),TSYS::base64));
	opt->setAttr("tp",itp);
    }
    else if(subModule() && (a_path == "/br/mod_" || a_path == "/mod/br") && ctrChkNode(opt,"get",R_R_R_,"root","root",SEC_RD)) {
	vector<string> list;
	modList(list);
	for(unsigned iA = 0; iA < list.size(); iA++)
	    opt->childAdd("el")->setAttr("id",list[iA])->setText(modAt(list[iA]).at().modName());
    }
    else TCntrNode::cntrCmdProc(opt);
}

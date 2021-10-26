
//OpenSCADA file: tarchives.cpp
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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
#include <sys/time.h>
#include <string.h>
#include <algorithm>

#include "tsys.h"
#include "tarchives.h"

#define BUF_SIZE_DEF 500
#define BUF_SIZE_MAX 100000

using namespace OSCADA;

//************************************************
//* Archive subsystem                            *
//************************************************

//************************************************
//* TArchiveS                                    *
//************************************************
TArchiveS::TArchiveS( ) :
    TSubSYS(SARH_ID,_("Archives-History"),true), elMess(""), elVal(""), elAval(""), mMessPer(10), prcStMess(false), mRes(true),
    headBuf(0), vRes(true), mValPer(1000), mValPrior(10), mAutoIdMode(BothPrmAttrId),
    mValForceCurTm(false), prcStVal(false), endrunReqVal(false), toUpdate(false), mRdRestDtOverTm(0), mRdAlarms(0)
{
    mAval = grpAdd("va_");

    //Message archiver DB structure
    elMess.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elMess.fldAdd(new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elMess.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elMess.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"200"));
    elMess.fldAdd(new TFld("START",_("To start"),TFld::Boolean,0,"1"));
    elMess.fldAdd(new TFld("CATEG",_("Messages categories"),TFld::String,0,"100"));
    elMess.fldAdd(new TFld("LEVEL",_("Messages level"),TFld::Integer,TFld::Selectable,"1","0",
	"0;1;2;3;4;5;6;7",_("Debug (0);Information (1[X]);Notice (2[X]);Warning (3[X]);Error (4[X]);Critical (5[X]);Alert (6[X]);Emergency (7[X])")));
    elMess.fldAdd(new TFld("ADDR",_("Address"),TFld::String,0,"100"));
    elMess.fldAdd(new TFld("REDNT",_("Redundant"),TFld::Boolean,0,"1","0"));
    elMess.fldAdd(new TFld("REDNT_RUN",_("Preferable run"),TFld::String,0,"20","<high>"));

    //Value archiver DB structure
    elVal.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elVal.fldAdd(new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elVal.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elVal.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"200"));
    elVal.fldAdd(new TFld("START",_("To start"),TFld::Boolean,0,"1","0"));
    elVal.fldAdd(new TFld("ADDR",_("Address"),TFld::String,0,"100"));
    elVal.fldAdd(new TFld("V_PER",_("Period of the values, seconds"),TFld::Real,0,"12.6","1","0;100000"));
    elVal.fldAdd(new TFld("A_PER",_("Period of the archiving, seconds"),TFld::Integer,0,"4","60","0;1000"));
    elVal.fldAdd(new TFld("SEL_PR",_("Selection priority"),TFld::Integer,0,"4","10","0;1000"));

    //Value archive DB structure
    elAval.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limArchID_SZ).c_str()));
    elAval.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elAval.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"200"));
    elAval.fldAdd(new TFld("START",_("To start"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("SrcMode",_("Source"),TFld::Integer,TFld::Selectable,"1","0",
	TSYS::strMess("%d;%d;%d",TVArchive::Passive,TVArchive::PassiveAttr,TVArchive::ActiveAttr).c_str(),
	_("Passive;Passive param. attribute;Active param. attribute")));
    elAval.fldAdd(new TFld("Source",_("Source"),TFld::String,0,"100"));
    elAval.fldAdd(new TFld("CombMode",_("Data combining mode"),TFld::Integer,TFld::Selectable,"1","0",
	TSYS::strMess("%d;%d;%d;%d",TVArchive::MovAver,TVArchive::LastVal,TVArchive::MinVal,TVArchive::MaxVal).c_str(),
	_("Moving average;Single;Minimum;Maximum")));
    elAval.fldAdd(new TFld("VTYPE",_("Value type"),TFld::Integer,TFld::Selectable,"1","0",
	TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",TFld::Boolean,TFld::Integer,TFld::Real,TFld::String,TFld::Int16,TFld::Int32,TFld::Int64,TFld::Float,TFld::Double).c_str(),
	_("Boolean;Integer;Real;String;Int16;Int32;Int64;Real(Float);Real(Double)")));
    elAval.fldAdd(new TFld("BPER",_("Buffer period, seconds"),TFld::Real,0,"9.6","1","0;10000"));
    elAval.fldAdd(new TFld("BSIZE",_("Buffer size, items"),TFld::Integer,0,"8","100","10;10000000"));
    elAval.fldAdd(new TFld("BHGRD",_("Buffer in the hard time grid"),TFld::Boolean,0,"1","1"));
    elAval.fldAdd(new TFld("BHRES",_("Buffer in the high time resolution"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("FillLast",_("Filling the passage points with the last value"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("ArchS",_("Process into archivers"),TFld::String,0,"1000"));

    setMessBufLen(BUF_SIZE_DEF);
}

TArchiveS::~TArchiveS( )
{
    //Stop subsystem
    if(prcStMess || prcStVal) subStop();

    //Free other resources
    nodeDelAll();
}

void TArchiveS::load_( )
{
    //Load parameters from command line

    //Load parameters
    setMessBufLen(s2i(TBDS::genPrmGet(nodePath()+"MessBufSize",i2s(messBufLen()))));
    setMessPeriod(s2i(TBDS::genPrmGet(nodePath()+"MessPeriod",i2s(messPeriod()))));
    setValPeriod(s2i(TBDS::genPrmGet(nodePath()+"ValPeriod",i2s(valPeriod()))));
    setValPrior(s2i(TBDS::genPrmGet(nodePath()+"ValPriority",i2s(valPrior()))));
    setValForceCurTm(s2i(TBDS::genPrmGet(nodePath()+"ValForceCurTm",i2s(valForceCurTm()))));
    setAutoIdMode((AutoIdMode)s2i(TBDS::genPrmGet(nodePath()+"AutoIdMode",i2s(autoIdMode()))));
    setRdRestDtOverTm(s2r(TBDS::genPrmGet(nodePath()+"RdRestDtOverTm",r2s(rdRestDtOverTm()))));

    //LidDB
    // Message archivers load
    string id,type;
    map<string, bool>	itReg;
    try {
	TConfig cEl(&elMess);
	//cEl.cfgViewAll(false);
	vector<string> dbLs;

	// Search int DB and create new archivers
	TBDS::dbList(dbLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
	    for(int fldCnt = 0; TBDS::dataSeek(dbLs[iIt]+"."+subId()+"_mess_proc",nodePath()+subId()+"_mess_proc",fldCnt++,cEl,TBDS::UseCache); ) {
		id = cEl.cfg("ID").getS();
		type = cEl.cfg("MODUL").getS();
		if(!modPresent(type))	continue;
		if(!at(type).at().messPresent(id)) at(type).at().messAdd(id, dbLs[iIt]);
		if(at(type).at().messAt(id).at().DB() == dbLs[iIt]) at(type).at().messAt(id).at().load(&cEl);
		at(type).at().messAt(id).at().setDB(dbLs[iIt], true);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    vector<string> mLs;
	    modList(mLs);
	    for(unsigned iM = 0; iM < mLs.size(); iM++) {
		at(mLs[iM]).at().messList(dbLs);
		for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
		    if(itReg.find(mLs[iM]+"."+dbLs[iIt]) == itReg.end() && SYS->chkSelDB(at(mLs[iM]).at().messAt(dbLs[iIt]).at().DB()))
			at(mLs[iM]).at().messDel(dbLs[iIt]);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_sys(TMess::Error, _("Error loading the message archivers."));
    }

    // Value archivers load
    try {
	TConfig cEl(&elVal);
	//cEl.cfgViewAll(false);
	vector<string> dbLs;
	itReg.clear();

	//  Search into DB and create new archivers
	TBDS::dbList(dbLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
	    for(int fldCnt = 0; TBDS::dataSeek(dbLs[iIt]+"."+subId()+"_val_proc",nodePath()+subId()+"_val_proc",fldCnt++,cEl,TBDS::UseCache); ) {
		id = cEl.cfg("ID").getS();
		type = cEl.cfg("MODUL").getS();
		if(!modPresent(type))	continue;
		if(!at(type).at().valPresent(id)) at(type).at().valAdd(id, dbLs[iIt]);
		if(at(type).at().valAt(id).at().DB() == dbLs[iIt]) at(type).at().valAt(id).at().load(&cEl);
		at(type).at().valAt(id).at().setDB(dbLs[iIt], true);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    vector<string> mLs;
	    modList(mLs);
	    for(unsigned iM = 0; iM < mLs.size(); iM++) {
		at(mLs[iM]).at().valList(dbLs);
		for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
		    if(itReg.find(mLs[iM]+"."+dbLs[iIt]) == itReg.end() && SYS->chkSelDB(at(mLs[iM]).at().valAt(dbLs[iIt]).at().DB()))
			at(mLs[iM]).at().valDel(dbLs[iIt]);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_sys(TMess::Error, _("Error loading the value archivers."));
    }

    // Value archives load
    try {
	TConfig cEl(&elAval);
	//cEl.cfgViewAll(false);
	vector<string> dbLs;
	itReg.clear();

	//  Search into DB and create new archives
	TBDS::dbList(dbLs, TBDS::LsCheckSel|TBDS::LsInclGenFirst);
	for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
	    for(int fldCnt = 0; TBDS::dataSeek(dbLs[iIt]+"."+subId()+"_val",nodePath()+subId()+"_val",fldCnt++,cEl,TBDS::UseCache); ) {
		id = cEl.cfg("ID").getS();
		if(!valPresent(id)) valAdd(id, dbLs[iIt]);
		//!!!! For force loading after creation from the archiver storage
		//else if(valAt(id).at().DB() == "*.*" && dbLs[iIt] != SYS->workDB()) valAt(id).at().setDB(dbLs[iIt]);
		if(valAt(id).at().DB() == dbLs[iIt]) valAt(id).at().load(&cEl);
		valAt(id).at().setDB(dbLs[iIt], true);
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    valList(dbLs);
	    for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
		if(itReg.find(dbLs[iIt]) == itReg.end() && SYS->chkSelDB(valAt(dbLs[iIt]).at().DB()))
		    valDel(dbLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_sys(TMess::Error, _("Value archives load error."));
    }
}

void TArchiveS::save_( )
{
    //Save parameters
    TBDS::genPrmSet(nodePath()+"MessBufSize", i2s(messBufLen()));
    TBDS::genPrmSet(nodePath()+"MessPeriod", i2s(messPeriod()));
    TBDS::genPrmSet(nodePath()+"ValPeriod", i2s(valPeriod()));
    TBDS::genPrmSet(nodePath()+"ValPriority", i2s(valPrior()));
    TBDS::genPrmSet(nodePath()+"ValForceCurTm",i2s(valForceCurTm()));
    TBDS::genPrmSet(nodePath()+"AutoIdMode",i2s(autoIdMode()));
    TBDS::genPrmSet(nodePath()+"RdRestDtOverTm",r2s(rdRestDtOverTm()));
}

string TArchiveS::valAdd( const string &iid, const string &idb )
{
    //if(valPresent(iid)) return "";

    return chldAdd(mAval, new TVArchive(TSYS::strEncode(sTrm(iid),TSYS::oscdID),idb,&aValE()));
}

void TArchiveS::valDel( const string &id, bool db )
{
    //Try to start the stopped archive to remove from the storages also
    AutoHD<TVArchive> vDel = valAt(id);
    if(db && !vDel.at().startStat() && vDel.at().toStart())
	try {
	    vDel.at().setSrcMode(TVArchive::Passive);
	    vDel.at().start();
	} catch(TError&) { }
    vDel.free();

    //Same removing
    chldDel(mAval, id, -1, db);
}

string TArchiveS::optDescr( )
{
    return TSYS::strMess(_(
	"======================== Subsystem \"Archives-History\" options ===================\n"
	"------ Parameters of the section '%s' of the configuration file ------\n"
	"MessBufSize  <items>    Size of the messages buffer.\n"
	"MessPeriod   <seconds>  Period of the messages archiving.\n"
	"ValPeriod    <mseconds> Period of active archiving of values.\n"
	"ValPriority  <level>    Level of priority of the task of active archiving of values.\n"
	"RdRestDtOverTm <days>   Depth of the forced overloading of the reserve history at startup, in days.\n\n"
	), nodePath().c_str()) + TSubSYS::optDescr();
}

void TArchiveS::unload( )
{
    TSubSYS::unload();

    mRdRes.lock(true);
    mRdArchM.clear();
    mRdRestDtOverTm = 0, mRdAlarms = 0;
    mRdRes.unlock();

    mRes.lock();
    mAlarms.clear();
    mRes.unlock();
    setMessBufLen(BUF_SIZE_DEF);

    mMessPer = 10, mValPer = 1000, mValPrior = 10, mValForceCurTm = false, mAutoIdMode = BothPrmAttrId;
}

void TArchiveS::subStart( )
{
    mess_sys(TMess::Debug, _("Start/update subsystem."));

    subStarting = true;
    toUpdate = false;	//Moved to start for prevent possible changes the toUpdate at processing

    vector<string> tLst, oLst;

    bool stArchM = false, stArchV = false;
    modList(tLst);
    //Start no started early archivers and check for already started
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	AutoHD<TTypeArchivator> mod = modAt(tLst[iT]);
	//Messages
	mod.at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(oLst[iO]);
	    if(mess.at().startStat()) stArchM = true;
	    else if(mess.at().toStart())
		try { mess.at().start(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error starting the message archiver '%s'."), oLst[iO].c_str());
		}
	}
	//Values
	mod.at().valList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TVArchivator> val = mod.at().valAt(oLst[iO]);
	    if(val.at().startStat()) stArchV = true;
	    else if(val.at().toStart())
		try { val.at().start(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error starting the value archiver '%s'."), val.at().workId().c_str());
		}
	}
    }

    //Value archives start.
    valList(oLst);
    for(unsigned iO = 0; iO < oLst.size(); iO++) {
	AutoHD<TVArchive> aval = valAt(oLst[iO]);
	if(aval.at().toStart())
	    try { aval.at().start(); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error starting the value archive '%s'."), oLst[iO].c_str());
	    }
    }

    //Start already started for update
    for(unsigned iT = 0; iT < tLst.size() && (stArchM || stArchV); iT++) {
	AutoHD<TTypeArchivator> mod = modAt(tLst[iT]);
	//Messages
	mod.at().messList(oLst);
	for(unsigned iO = 0; stArchM && iO < oLst.size(); iO++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(oLst[iO]);
	    if(mess.at().startStat())
		try { mess.at().start(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error starting the message archiver '%s'."), oLst[iO].c_str());
		}
	}
	//Values
	mod.at().valList(oLst);
	for(unsigned iO = 0; stArchV && iO < oLst.size(); iO++) {
	    AutoHD<TVArchivator> val = mod.at().valAt(oLst[iO]);
	    if(val.at().startStat())
		try { val.at().start(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error starting the value archiver '%s'."), val.at().workId().c_str());
		}
	}
    }

    //Start messages values acquisition task
    if(!prcStMess) SYS->taskCreate(nodePath('.',true)+".mess", 0, TArchiveS::ArhMessTask, this);
    if(!prcStVal)  SYS->taskCreate(nodePath('.',true)+".vals", valPrior(), TArchiveS::ArhValTask, this);

    TSubSYS::subStart();

    subStarting = false;
}

void TArchiveS::subStop( )
{
    TSubSYS::subStop( );

    vector<string> tLst, oLst;

    //Messages and Values acquisition task stop
    if(prcStMess) SYS->taskDestroy(nodePath('.',true)+".mess");
    if(prcStVal)  SYS->taskDestroy(nodePath('.',true)+".vals", &endrunReqVal);

    //Archivers stop
    modList(tLst);
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	AutoHD<TTypeArchivator> mod = modAt(tLst[iT]);
	// Value archives stop
	mod.at().valList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TVArchivator> val = mod.at().valAt(oLst[iO]);
	    if(val.at().startStat())
		try { val.at().stop(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error stopping the value archiver '%s'."), oLst[iO].c_str());
		}
	}
	// Message archivers stop
	mod.at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(oLst[iO]);
	    if(mess.at().startStat())
		try { mess.at().stop(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error stopping the message archiver '%s'."), oLst[iO].c_str());
		}
	}
    }

    //Value archives stop
    valList(oLst);
    for(unsigned iO = 0; iO < oLst.size(); iO++) {
	AutoHD<TVArchive> aval = valAt(oLst[iO]);
	if(aval.at().startStat())
	    try { aval.at().stop(); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Value archive '%s' stop error."), oLst[iO].c_str());
	    }
    }
}

void TArchiveS::perSYSCall( unsigned int cnt )
{
    if(subStartStat() && toUpdate && !subStarting) subStart();

    TSubSYS::perSYSCall(cnt);
}

void TArchiveS::messPut( time_t tm, int utm, const string &categ, int8_t level, const string &mess, const string &arch )
{
    messPut(TMess::SRec(tm,utm,categ,level,mess), arch);
}

void TArchiveS::messPut( const TMess::SRec &rec, const string &arch )
{
    map<string, bool> archMap;
    string tVl;
    for(int off = 0; (tVl=TSYS::strParse(arch,0,";",&off)).size(); ) archMap[tVl] = true;

    MtxAlloc res(mRes);
    //Alarms processing. For level less 0 alarm is set
    if((arch.empty() || archMap[ARCH_BUF] || archMap[ARCH_ALRM] || archMap[ARCH_ALRM_CH]) && !archMap[ARCH_NOALRM]) {
	res.lock();
	map<string,TMess::SRec>::iterator p = mAlarms.find(rec.categ);
	if(p != mAlarms.end() && rec.level < 0 && abs(rec.level) == p->second.level && SYS->rdEnable() &&
		    rec.mess == p->second.mess && FTM(rec) >= FTM(p->second))
	    return;		//Prevent for update equal alarm in redundancy
	if(rec.level < 0 && ((p == mAlarms.end() && !archMap[ARCH_ALRM_CH]) ||
				(p != mAlarms.end() && (FTM(rec) >= FTM(p->second) || SYS->rdEnable()))))
	{ mAlarms[rec.categ] = rec; mAlarms[rec.categ].level = abs(mAlarms[rec.categ].level); }
	if(rec.level >= 0 && p != mAlarms.end() && FTM(rec) >= FTM(p->second)) mAlarms.erase(p);
	res.unlock();
    }

    //Put the message to the buffer
    if(arch.empty() || archMap[ARCH_NOALRM] || archMap[ARCH_BUF]) {
	res.lock();
	mBuf[headBuf] = rec;
	if((++headBuf) >= mBuf.size()) headBuf = 0;

	//Check for the archiver headers to messages buffer
	for(unsigned iM = 0; iM < actMess.size(); iM++) {
	    int &messHead = actMess[iM].at().messHead;
	    if(messHead >= 0 && messHead == (int)headBuf && (++messHead) >= (int)mBuf.size()) messHead = 0;
	}
	res.unlock();
    }

    //Put message to the archive <arch>
    if(!archMap[ARCH_BUF]) {
	vector<TMess::SRec> ml;
	ml.push_back(rec);
	vector<string> tLst, oLst;
	modList(tLst);
	for(unsigned iT = 0; iT < tLst.size(); iT++) {
	    at(tLst[iT]).at().messList(oLst);
	    for(unsigned iO = 0; iO < oLst.size(); iO++) {
		AutoHD<TMArchivator> archtor = at(tLst[iT]).at().messAt(oLst[iO]);
		if(archtor.at().startStat() && (arch.empty() || archMap[archtor.at().workId()]))
		    try { archtor.at().put(ml); }
		    catch(TError &er) {
			mess_sys(TMess::Error, _("Put message to the archiver '%s' error: %s"),
					(tLst[iT]+"."+oLst[iO]).c_str(), er.mess.c_str());
		    }
	    }
	}
    }
}

void TArchiveS::messPut( const vector<TMess::SRec> &recs, const string &arch )
{
    for(unsigned iR = 0; iR < recs.size(); iR++)
	messPut(recs[iR], arch);
}

time_t TArchiveS::messGet( time_t bTm, time_t eTm, vector<TMess::SRec> &recs,
    const string &category, int8_t level, const string &arch, time_t upTo )
{
    time_t result = fmin(eTm, (time_t)(TSYS::curTime()/1000000) - 1);	//Means successful buffers processing, -1 for waranty all curent date get without doubles and losses
    recs.clear();

    map<string, bool> archMap;
    string tVl;
    for(int off = 0; (tVl=TSYS::strParse(arch,0,";",&off)).size(); ) archMap[tVl] = true;

    if(!upTo) upTo = SYS->sysTm() + prmInterf_TM;
    TRegExp re(category, "p");

    //Get records from buffer
    MtxAlloc res(mRes, true);
    unsigned iBuf = headBuf;
    while(level >= 0 && (arch.empty() || archMap[ARCH_BUF]) /*&& SYS->sysTm() < upTo*/) {
	if(mBuf[iBuf].time >= bTm && mBuf[iBuf].time && mBuf[iBuf].time <= eTm &&
		TMess::messLevelTest(level,mBuf[iBuf].level) && re.test(mBuf[iBuf].categ)) {
	    //Unsorted, but can be wrong for some internal procedures waiting for the last message in the end
	    //recs.push_back(mBuf[iBuf]);
	    //Sorted
	    int iP = recs.size();
	    for(int iM = recs.size()-1; iM >= 0; iM--)
		if(FTM(recs[iM]) > FTM(mBuf[iBuf])) iP = iM;
		else if(FTM(recs[iM]) < FTM(mBuf[iBuf])) break;
	    recs.insert(recs.begin()+iP, mBuf[iBuf]);
	}
	if((++iBuf) >= mBuf.size()) iBuf = 0;
	if(iBuf == headBuf) break;
    }
    res.unlock();

    //Get records from the archives
    if(arch != ARCH_BUF && arch != ARCH_ALRM) {
	vector<string> tLst, oLst;
	modList(tLst);
	for(unsigned iT = 0; level >= 0 && iT < tLst.size(); iT++) {
	    at(tLst[iT]).at().messList(oLst);
	    for(unsigned iO = 0; iO < oLst.size() && SYS->sysTm() < upTo; iO++) {
		AutoHD<TMArchivator> archtor = at(tLst[iT]).at().messAt(oLst[iO]);
		if(archtor.at().startStat() && (arch.empty() || archMap[archtor.at().workId()]))
		    //!!!! But possible only one archiver, from all, processing and next continued by the limit
		    result = fmin(result, archtor.at().get(bTm,eTm,recs,category,level,arch.size()?upTo:0));
	    }
	}
    }

    //Alarms request processing
    if(level < 0) {
	res.lock();
	vector< pair<int64_t,TMess::SRec* > > mb;
	for(map<string,TMess::SRec>::iterator p = mAlarms.begin(); p != mAlarms.end() /*&& SYS->sysTm() < upTo*/; p++)
	    if((p->second.time >= bTm || bTm == eTm) && p->second.time <= eTm &&
		    TMess::messLevelTest(level,p->second.level) && re.test(p->second.categ))
		mb.push_back(pair<int64_t,TMess::SRec* >(FTM(p->second),&p->second));
	sort(mb.begin(), mb.end());
	for(unsigned iB = 0; iB < mb.size(); iB++) {
	    recs.push_back(*mb[iB].second);
	    if(recs.back().level > 0) recs.back().level *= -1;
	}
	res.unlock();
    }

    return result;
}

time_t TArchiveS::messBeg( const string &arch )
{
    time_t rez = 0;
    MtxAlloc res(mRes, true);
    if(arch.empty() || arch == ARCH_BUF) {
	unsigned iBuf = headBuf;
	while(!arch.size() || arch == ARCH_BUF) {
	    rez = rez ? vmin(rez,mBuf[iBuf].time) : mBuf[iBuf].time;
	    if((++iBuf) >= mBuf.size()) iBuf = 0;
	    if(iBuf == headBuf) break;
	}
	if(!arch.empty()) return rez;
    }
    res.unlock();

    //Get records from archives
    vector<string> tLst, oLst;
    modList(tLst);
    AutoHD<TMArchivator> archtor;
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	at(tLst[iT]).at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    archtor = at(tLst[iT]).at().messAt(oLst[iO]);
	    if(archtor.at().startStat() && (!arch.size() || arch==archtor.at().workId()))
		rez = rez ? vmin(rez,archtor.at().begin()) : archtor.at().begin();
	}
    }

    return rez;
}

time_t TArchiveS::messEnd( const string &arch )
{
    time_t rez = 0;
    MtxAlloc res(mRes, true);
    if(arch.empty() || arch == ARCH_BUF) {
	unsigned iBuf = headBuf;
	while(!arch.size() || arch == ARCH_BUF) {
	    rez = rez ? vmax(rez,mBuf[iBuf].time) : mBuf[iBuf].time;
	    if((++iBuf) >= mBuf.size()) iBuf = 0;
	    if(iBuf == headBuf) break;
	}
	if(!arch.empty()) return rez;
    }
    res.unlock();

    //Get records from archives
    vector<string> tLst, oLst;
    modList(tLst);
    AutoHD<TMArchivator> archtor;
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	at(tLst[iT]).at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    archtor = at(tLst[iT]).at().messAt(oLst[iO]);
	    if(archtor.at().startStat() && (!arch.size() || arch==archtor.at().workId()))
		rez = rez ? vmax(rez,archtor.at().end()) : archtor.at().end();
	}
    }

    return rez;
}

time_t TArchiveS::rdTm( )
{
    time_t rez = 0;

    vector<string> cls;
    rdActArchMList(cls);
    for(unsigned iC = 0; iC < cls.size(); iC++) {
	AutoHD<TMArchivator> arch = at(TSYS::strParse(cls[iC],0,".")).at().messAt(TSYS::strParse(cls[iC],1,"."));

	if(arch.at().startStat() && arch.at().redntUse()) rez = fmax(rez, arch.at().redntTm());
    }

    return rez ? rez : (time_t)(TSYS::curTime()/1000000) - 1;
}

bool TArchiveS::rdProcess( XMLNode *reqSt )
{
    if(reqSt) {
	string StId = reqSt->attr("StId");
	ResAlloc res(mRdRes, true);
	mRdArchM[StId].clear();
	for(unsigned iC = 0; iC < reqSt->childSize(); iC++)
	    if(reqSt->childGet(iC)->name() == "archM")
		mRdArchM[StId][reqSt->childGet(iC)->attr("id")] = s2i(reqSt->childGet(iC)->attr("run"));
	return true;
    }

    //Planing archivers running and process requests to remote run ones
    map<string,TSYS::SStat> sts = SYS->rdSts();
    map<string,TSYS::SStat>::iterator sit;
    map<string, bool>::iterator cit;

    vector<string> cls;
    rdActArchMList(cls);

    bool isRdArchPresent = false;

    for(unsigned iC = 0; iC < cls.size(); iC++) {
	AutoHD<TMArchivator> arch = at(TSYS::strParse(cls[iC],0,".")).at().messAt(TSYS::strParse(cls[iC],1,"."));

	// Process remote running archivers, before the redundancy status change
	if(arch.at().startStat() && arch.at().redntUse()) { arch.at().redntDataUpdate(); isRdArchPresent = true; }

	// Check archiver running plane
	if(!arch.at().redntMode()) arch.at().setRedntUse(false);
	else {
	    ResAlloc res(mRdRes, false);
	    if(arch.at().redntRun() == "<high>") {
		int wLev = SYS->rdStLevel();
		for(sit = sts.begin(); sit != sts.end(); ++sit)
		    if(sit->second.isLive && (cit=mRdArchM[sit->first].find(arch.at().workId())) != mRdArchM[sit->first].end() && cit->second)
			wLev = vmax(wLev, sit->second.lev);
		arch.at().setRedntUse(SYS->rdStLevel() < wLev);
	    }
	    else if(arch.at().redntRun() == "<low>") {
		int wLev = SYS->rdStLevel();
		for(sit = sts.begin(); sit != sts.end(); sit++)
		    if(sit->second.isLive && (cit=mRdArchM[sit->first].find(arch.at().workId())) != mRdArchM[sit->first].end() && cit->second)
		wLev = vmin(wLev, sit->second.lev);
		arch.at().setRedntUse(SYS->rdStLevel() > wLev);
	    }
	    else if(arch.at().redntRun() == "<optimal>") {
		vector<string> cls_lc;
		rdActArchMList(cls_lc, true);
		bool remPresent = false;
		for(sit = sts.begin(); sit != sts.end(); sit++)
		    if(sit->second.isLive && (cit=mRdArchM[sit->first].find(arch.at().workId())) != mRdArchM[sit->first].end()) {
			if(!remPresent) remPresent = cit->second;
			int aArch = 0;
			for(map<string,bool>::iterator scit = mRdArchM[sit->first].begin(); scit != mRdArchM[sit->first].end(); scit++)
			    if(scit->second) aArch++;
			if(((int)cls_lc.size()-aArch) >= 0 && cit->second) break;
		    }
		arch.at().setRedntUse(sit != sts.end());
	    }
	    else {
		for(sit = sts.begin(); sit != sts.end(); sit++)
		    if(sit->second.isLive && (cit=mRdArchM[sit->first].find(arch.at().workId())) != mRdArchM[sit->first].end() &&
			cit->second && arch.at().redntRun() == sit->first)
			    break;
		arch.at().setRedntUse(sit != sts.end());
	    }
	    res.release();
	}
	arch.free();
    }

    //Alarms initial obtain
    if(!mRdAlarms && isRdArchPresent) {
	XMLNode req("get"); req.setAttr("path", nodePath()+"/%2fserv%2fmess")->setAttr("lev","-1");
	if(SYS->rdStRequest(req).size()) {
	    mRdAlarms = s2i(req.attr("tm"));
	    // Process the result
	    for(unsigned iEl = 0; iEl < req.childSize(); ++iEl) {
		XMLNode *el = req.childGet(iEl);
		messPut(s2ll(el->attr("time")), s2i(el->attr("utime")), el->attr("cat"), s2i(el->attr("lev")), el->text(), ARCH_ALRM);
	    }
	}
    }

    return true;
}

void TArchiveS::rdActArchMList( vector<string> &ls, bool isRun )
{
    AutoHD<TMArchivator> archM;
    ls.clear();
    vector<string> mls, cls;
    modList(mls);
    for(unsigned iM = 0; iM < mls.size(); iM++) {
	//if(!at(mls[iM]).at().redntAllow()) continue;
	at(mls[iM]).at().messList(cls);
	for(unsigned iC = 0; iC < cls.size(); iC++) {
	    archM = at(mls[iM]).at().messAt(cls[iC]);
	    if(archM.at().startStat() && (!isRun || !archM.at().redntUse()))
		ls.push_back(archM.at().workId());
	}
    }
}

string TArchiveS::rdStRequest( const string &arch, XMLNode &req, const string &prevSt, bool toRun )
{
    string lcPath = req.attr("path");
    bool prevPresent = false;
    string rez;
    map<string, TSYS::SStat>::iterator sit;
    map<string, bool>::iterator cit;

    ResAlloc res(mRdRes, false);
    map<string, map<string, bool> > sts = mRdArchM;
    res.unlock();

    for(map<string, map<string,bool> >::iterator sit = sts.begin(); sit != sts.end(); ++sit) {
	if((cit=sit->second.find(arch)) != sit->second.end() && (!toRun || cit->second)) {
	    if(prevSt.size() && !prevPresent) {
		if(sit->first == prevSt) prevPresent = true;
		continue;
	    }
	    if((rez=SYS->rdStRequest(req,sit->first)).size()) return rez;

	    res.lock(true);
	    mRdArchM.erase(sit->first);
	    res.unlock();
	}
    }

    if(prevSt.empty()) at(TSYS::strParse(arch,0,".")).at().messAt(TSYS::strParse(arch,1,".")).at().setRedntUse(false);

    return "";
}

void TArchiveS::setMessBufLen( unsigned len )
{
    MtxAlloc res(mRes, true);
    len = vmin(BUF_SIZE_MAX, vmax(BUF_SIZE_DEF,len));
    while(mBuf.size() > len) {
	mBuf.erase(mBuf.begin() + headBuf);
	if(headBuf >= mBuf.size())	headBuf = 0;
	for(unsigned iM = 0; iM < actMess.size(); iM++) {
	    int &messHead = actMess[iM].at().messHead;
	    if(messHead >= 0 && messHead >= (int)mBuf.size()) messHead = 0;	//mBuf.size()-1;
	}
    }
    while(mBuf.size() < len) mBuf.insert(mBuf.begin() + headBuf, TMess::SRec());
    modif();
}

void TArchiveS::setActMess( TMArchivator *a, bool val )
{
    unsigned iArch;

    MtxAlloc res(mRes, true);
    for(iArch = 0; iArch < actMess.size(); iArch++)
	if(actMess[iArch].at().id() == a->id() && actMess[iArch].at().owner().modId() == a->owner().modId()) break;

    if(val && iArch >= actMess.size())	actMess.push_back(a);
    if(!val && iArch < actMess.size())	actMess.erase(actMess.begin()+iArch);
}

void TArchiveS::setActVal( TVArchive *a, bool val )
{
    unsigned iArch;

    MtxAlloc res(vRes, true);
    for(iArch = 0; iArch < actVal.size(); iArch++)
	if(actVal[iArch].at().id() == a->id()) break;

    if(val && iArch >= actVal.size())	actVal.push_back(a);
    if(!val && iArch < actVal.size())	actVal.erase(actVal.begin()+iArch);
}

void *TArchiveS::ArhMessTask( void *param )
{
    TArchiveS &arh = *(TArchiveS *)param;
    arh.prcStMess = true;
    bool isLast = false;

    //First wait
    TSYS::taskSleep(1000000000ll*arh.messPeriod());

    //Turns cycle
    while(true) {
	if(TSYS::taskEndRun()) isLast = true;
	//Message buffer read
	MtxAlloc res(arh.mRes, true);
	for(unsigned iM = 0; iM < arh.actMess.size(); iM++) {
	    AutoHD<TMArchivator> mArh = arh.actMess[iM];
	    int &messHead = mArh.at().messHead;
	    if(messHead < 0 && ((messHead=arh.headBuf+1) >= (int)arh.mBuf.size() || !arh.mBuf[messHead].time)) messHead = 0;
	    if(messHead == (int)arh.headBuf)	continue;

	    // Get new messages
	    unsigned newHeadLstread = arh.headBuf;
	    vector<TMess::SRec> oMess;
	    for(unsigned iM2 = messHead; iM2 != newHeadLstread; ) {
		oMess.push_back(arh.mBuf[iM2]);
		if((++iM2) >= arh.mBuf.size()) iM2 = 0;
	    }
	    res.unlock();

	    bool rez = false;
	    try { rez = mArh.at().put(oMess); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		arh.mess_sys(TMess::Error, _("Message buffer process error."));
	    }

	    res.lock();
	    if(rez) messHead = newHeadLstread;
	}
	res.unlock();

	if(isLast) break;

	TSYS::taskSleep(1000000000ll*arh.messPeriod());
    }

    arh.prcStMess = false;

    return NULL;
}

void *TArchiveS::ArhValTask( void *param )
{
    TArchiveS &arh = *(TArchiveS*)param;
    arh.endrunReqVal = false;
    arh.prcStVal = true;

    while(!arh.endrunReqVal) {
	int64_t work_tm = SYS->curTime();

	arh.vRes.lock();
	int64_t stm = arh.valForceCurTm() ? TSYS::curTime() : 0;
	for(unsigned iArh = 0; iArh < arh.actVal.size(); iArh++)
	    try {
		if(work_tm/arh.actVal[iArh].at().period() > arh.actVal[iArh].at().end()/arh.actVal[iArh].at().period())
		    arh.actVal[iArh].at().getActiveData(stm);
	    } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	arh.vRes.unlock();

	TSYS::taskSleep((int64_t)arh.valPeriod()*1000000);
    }

    arh.prcStVal = false;

    return NULL;
}

TVariant TArchiveS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // Array messGet(int btm, int etm, string cat = "", int lev = 0, string arch = "", int upTm = 0);
    //     - request of the program messages for the time from <btm>
    //       to <etm> for the category <cat>, level <lev> and archivers <arch>
    //  btm - begin time
    //  etm - end time
    //  cat - messages' category
    //  lev - messages level
    //  arch - message archivers by list items separated ';'
    //  upTm - sets the operation continuance limit to time; a negative value used as relative time; less to prmInterf_TM (7).
    if(iid == "messGet" && prms.size() >= 2) {
	vector<TMess::SRec> recs;
	int upTm = (prms.size() >= 6) ? prms[5].getI() : 0;
	time_t result = messGet(prms[0].getI(), prms[1].getI(), recs, ((prms.size()>=3) ? prms[2].getS() : string("")),
				((prms.size()>=4) ? prms[3].getI() : 0), ((prms.size()>=5) ? prms[4].getS() : string("")),
				vmin((upTm<0)?SYS->sysTm()+abs(upTm):upTm,SYS->sysTm()+prmInterf_TM));
	TArrayObj *rez = new TArrayObj();
	rez->propSet("tm", (int64_t)result);
	for(unsigned iM = 0; iM < recs.size(); iM++) {
	    TVarObj *am = new TVarObj();
	    am->propSet("tm", (int)recs[iM].time);
	    am->propSet("utm", recs[iM].utime);
	    am->propSet("categ", recs[iM].categ);
	    am->propSet("level", recs[iM].level);
	    am->propSet("mess", recs[iM].mess);
	    rez->arSet(iM, am);
	}
	return rez;
    }
    // bool messPut(int tm, int utm, string cat, int lev, string mess, string arch = "")
    //     - write message <mess> with category <cat>,
    //       level <lev> and time <tm>.<utm> to archivers <arch> or/and allarms list.
    //  tm.utm - seconds and microseconds message time
    //  cat - message' category
    //  lev - message level
    //  mess - message text
    //  arch - archivers by list items separated ';';
    //         zero or "<buffer>" cause to generic writing to the buffer and alarms (lev <0) else direct to the pointed archivers
    if(iid == "messPut" && prms.size() >= 5) {
	messPut(prms[0].getI(), prms[1].getI(), prms[2].getS(), prms[3].getI(), prms[4].getS(), (prms.size() >= 6)?prms[5].getS():"");
	return true;
    }

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TArchiveS::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path == "/serv/redundant") {	//Redundant service requests
	if(ctrChkNode(opt,"st",RWRWR_,"root",SARH_ID,SEC_RD)) {	//State
	    opt->setAttr("inProc", "1");
	    opt->setAttr("StLevel",i2s(SYS->rdStLevel()));
	    map<string, bool> mArchLs;
	    vector<string> cls;
	    rdActArchMList(cls);
	    for(unsigned iL = 0; iL < cls.size(); iL++) mArchLs[cls[iL]] = false;
	    rdActArchMList(cls, true);
	    for(unsigned iL = 0; iL < cls.size(); iL++) mArchLs[cls[iL]] = true;
	    for(map<string,bool>::iterator cit = mArchLs.begin(); cit != mArchLs.end(); cit++)
	    opt->childAdd("archM")->setAttr("id",cit->first)->setAttr("run",cit->second?"1":"0");
	    return;
	}
    }
    if(a_path == "/serv/mess") {	//Messages access
	if(ctrChkNode(opt,"info",RWRWR_,"root",SARH_ID,SEC_RD)) {	//Messages information
	    string arch = opt->attr("arch");
	    opt->setAttr("end", u2s(messEnd(arch)));
	    opt->setAttr("beg", u2s(messBeg(arch)));
	}
	else if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {	//Value's data request
	    time_t tm      = strtoul(opt->attr("tm").c_str(), 0, 10);
	    time_t tm_grnd = strtoul(opt->attr("tm_grnd").c_str(), 0, 10);
	    string arch    = opt->attr("arch");
	    string cat     = opt->attr("cat");
	    int    lev     = s2i(opt->attr("lev"));
	    vector<TMess::SRec> rez;

	    if(!tm) tm = (time_t)(TSYS::curTime()/1000000) - 1;	//-1 for waranty all curent date get without doubles and losses
	    opt->setAttr("tm", ll2s(messGet(tm_grnd,tm,rez,cat,(TMess::Type)lev,arch)));
	    for(unsigned iR = 0; iR < rez.size(); iR++)
		opt->childAdd("el")->
		    setAttr("time", u2s(rez[iR].time))->
		    setAttr("utime", u2s(rez[iR].utime))->
		    setAttr("cat", rez[iR].categ)->
		    setAttr("lev", i2s(rez[iR].level))->
		    setText(rez[iR].mess);
	}
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	//Value's data set
	    for(unsigned iR = 0; iR < opt->childSize(); iR++) {
		XMLNode *mNd = opt->childGet(iR);
		messPut(strtol(mNd->attr("time").c_str(),NULL,10),
			s2i(mNd->attr("utime")),
			mNd->attr("cat"), s2i(mNd->attr("lev")), mNd->text());
	    }
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/va_",_("Value archive"),RWRWR_,"root",SARH_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limArchID_SZ).c_str());
	if(SYS->rdEnable() && ctrMkNode("area",opt,0,"/redund",_("Redundancy"))) {
	    ctrMkNode("fld",opt,-1,"/redund/restDtOverTm",_("Overtime of the reserve history reload at start, days"),RWRWR_,"root",SARH_ID,1, "tp","real");
	    if(ctrMkNode("table",opt,-1,"/redund/mArch",_("Message archivers"),RWRWR_,"root",SARH_ID,1,"key","id")) {
		ctrMkNode("list",opt,-1,"/redund/mArch/id",_("Archiver"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/redund/mArch/nm",_("Name"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/redund/mArch/start",_("Run."),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/redund/mArch/rdndt",_("Redund."),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/redund/mArch/prefRun",_("Pref. to run"),RWRWR_,"root",SDAQ_ID,4,"tp","str",
		    "idm","1","dest","select","select","/redund/lsMode");
		ctrMkNode("list",opt,-1,"/redund/mArch/remoted",_("Remote"),R_R_R_,"root",SDAQ_ID,1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,1,"/m_arch",_("Messages"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/m_arch/size",_("Buffer size"),RWRWR_,"root",SARH_ID,2,
		"tp","dec","min",i2s(BUF_SIZE_DEF).c_str());
	    ctrMkNode("fld",opt,-1,"/m_arch/per",_("Archiving period, seconds"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    if(ctrMkNode("area",opt,-1,"/m_arch/view",_("View"),R_R___,"root",SARH_ID)) {
		ctrMkNode("fld",opt,-1,"/m_arch/view/tm",_("Time, size (seconds) and level"),RWRW__,"root",SARH_ID,1,"tp","time");
		ctrMkNode("fld",opt,-1,"/m_arch/view/size","",RWRW__,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/m_arch/view/lvl","",RWRW__,"root",SARH_ID,5,"tp","dec", "dest","select",
		    "sel_id","0;1;2;3;4;5;6;7;-1;-2;-3;-4;-5;-6;-7",
		    "sel_list",_("Debug (0);Information (1[X]);Notice (2[X]);Warning (3[X]);Error (4[X]);Critical (5[X]);Alert (6[X]);Emergency (7[X]);"
			         "Information (1[X]), ALARMS;Notice (2[X]), ALARMS;Warning (3[X]), ALARMS;Error (4[X]), ALARMS;Critical (5[X]), ALARMS;Alert (6[X]), ALARMS;Emergency (7[X]), ALARMS"),
		    "help",_("Receiving messages with a level greater or equal to the specified."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str","help",TMess::labMessCat());
		ctrMkNode("fld",opt,-1,"/m_arch/view/archtor",_("Archivers"),RWRW__,"root",SARH_ID,4,"tp","str","dest","sel_ed","select","/m_arch/lstAMess",
		    "help",_("Message archivers.\nDo not set the field for processing the request by the buffer and all archivers.\nSet '<buffer>' for processing by the buffer."));
		XMLNode *tNd = NULL;
		if((tNd=ctrMkNode("table",opt,-1,"/m_arch/view/mess",_("Messages"),RWRW__,"root",SARH_ID))) {
		    if(tNd && s2i(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user"))) < 0)
			tNd->setAttr("key","1")->setAttr("s_com","del");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/1",_("Category"),R_R___,"root",SARH_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/2",_("Level"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/3",_("Message"),R_R___,"root",SARH_ID,1,"tp","str");
		}
		if(s2i(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user"))) < 0 && mAlarms.size())
		    ctrMkNode("comm",opt,-1,"/m_arch/view/alClean",_("Clear visible violations"),RWRW__,"root",SARH_ID);
	    }
	}
	if(ctrMkNode("area",opt,2,"/v_arch",_("Values"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/v_arch/per",_("Period of the data receiving, milliseconds"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/prior",_("Level of priority of the data receiving task"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",TMess::labTaskPrior());
	    ctrMkNode("fld",opt,-1,"/v_arch/fCurTm",_("Forced to set timestampes in the current time"),RWRWR_,"root",SARH_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/v_arch/appAttrIdMode",_("Mode of forming ID of the automatic created archives"),RWRWR_,"root",SARH_ID,4,"tp","dec", "dest","select",
		"sel_id",TSYS::strMess("%d;%d;%d;%d",BothPrmAttrId,OnlyPrmId,OnlyAttrId,AllCntrPrmAttrId).c_str(),
		"sel_list",_("Both parameter and attribute ID;Only parameter ID;Only attribute ID;All controller, parameter and attribute ID"));
	    ctrMkNode("fld",opt,-1,"/v_arch/nmb",_("Number"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/v_arch/archs",_("Value archives"),RWRWR_,"root",SARH_ID,5,
		"tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref","va_","idSz",i2s(limArchID_SZ).c_str());
	}
	return;
    }

    //Process command to page
    if(a_path == "/m_arch/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(messPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessPeriod(s2i(opt->text()));
    }
    else if(a_path == "/m_arch/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(messBufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessBufLen(s2i(opt->text()));
    }
    else if(a_path == "/m_arch/view/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD)) {
	    opt->setText(TBDS::genPrmGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text()))	opt->setText(i2s(TSYS::curTime()/1000000));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genPrmSet(nodePath()+"messTm",(s2i(opt->text())>=TSYS::curTime()/1000000)?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messSize","60",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/cat") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/lvl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/archtor") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messArch","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messArch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/lstAMess" && ctrChkNode(opt,"get",R_R___)) {
	map<string, bool> itsMap;
	itsMap[ARCH_BUF] = true;
	vector<string> lsm, lsa;
	modList(lsm);
	for(unsigned iM = 0; iM < lsm.size(); iM++) {
	    at(lsm[iM]).at().messList(lsa);
	    for(unsigned iA = 0; iA < lsa.size(); iA++)
		itsMap[lsm[iM]+"."+lsa[iA]] = true;
	}

	string curVal = TBDS::genPrmGet(nodePath()+"messArch","",opt->attr("user")), tVl, tVl1;
	for(int off = 0; (tVl=TSYS::strParse(curVal,0,";",&off)).size(); tVl1 += (tVl1.size()?";":"")+tVl)
	{ opt->childAdd("el")->setText(tVl1); itsMap[tVl] = false; }

	for(map<string, bool>::iterator iM = itsMap.begin(); iM != itsMap.end(); ++iM)
	    if(iM->second) opt->childAdd("el")->setText(curVal+(curVal.size()?";":"")+iM->first);
    }
    else if(a_path == "/m_arch/view/mess"){
	if(ctrChkNode(opt,"get",R_R___,"root",SARH_ID)) {
	    vector<TMess::SRec> rec;
	    time_t gtm = s2i(TBDS::genPrmGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!gtm) gtm = (time_t)(TSYS::curTime()/1000000);
	    int gsz = s2i(TBDS::genPrmGet(nodePath()+"messSize","60",opt->attr("user")));
	    messGet(gtm-gsz, gtm, rec,
		TBDS::genPrmGet(nodePath()+"messCat","",opt->attr("user")),
		s2i(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user"))),
		TBDS::genPrmGet(nodePath()+"messArch","",opt->attr("user")));

	    XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0","",R_R___,"root",SARH_ID);
	    XMLNode *n_tmu	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a","",R_R___,"root",SARH_ID);
	    XMLNode *n_cat	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/1","",R_R___,"root",SARH_ID);
	    XMLNode *n_lvl	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/2","",R_R___,"root",SARH_ID);
	    XMLNode *n_mess	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/3","",R_R___,"root",SARH_ID);
	    for(int iRec = rec.size()-1; iRec >= 0; iRec--) {
		if(n_tm)	n_tm->childAdd("el")->setText(i2s(rec[iRec].time));
		if(n_tmu)	n_tmu->childAdd("el")->setText(i2s(rec[iRec].utime));
		if(n_cat)	n_cat->childAdd("el")->setText(rec[iRec].categ);
		if(n_lvl)	n_lvl->childAdd("el")->setText(i2s(rec[iRec].level));
		if(n_mess)	n_mess->childAdd("el")->setText(rec[iRec].mess);
	    }
	}
	if(s2i(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user"))) < 0 && ctrChkNode(opt,"del",RWRW__,"root",SARH_ID,SEC_WR)) {
	    mRes.lock();
	    mAlarms.erase(opt->attr("key_1"));
	    mRes.unlock();
	}
    }
    else if(a_path == "/m_arch/view/alClean" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR)) {
	vector<TMess::SRec> rec;
	time_t gtm = s2i(TBDS::genPrmGet(nodePath()+"messTm","0",opt->attr("user")));
	if(!gtm) gtm = (time_t)(TSYS::curTime()/1000000);
	int gsz = s2i(TBDS::genPrmGet(nodePath()+"messSize","60",opt->attr("user")));
	messGet(gtm-gsz, gtm, rec,
	    TBDS::genPrmGet(nodePath()+"messCat","",opt->attr("user")),
	    s2i(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user"))),
	    TBDS::genPrmGet(nodePath()+"messArch","",opt->attr("user")));
	mRes.lock();
	for(unsigned iRec = 0; iRec < rec.size(); ++iRec)
	    mAlarms.erase(rec[iRec].categ);
	mRes.unlock();
    }
    else if(a_path == "/v_arch/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(valPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPeriod(s2i(opt->text()));
    }
    else if(a_path == "/v_arch/prior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(valPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPrior(s2i(opt->text()));
    }
    else if(a_path == "/v_arch/fCurTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(valForceCurTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValForceCurTm(s2i(opt->text()));
    }
    else if(a_path == "/v_arch/appAttrIdMode") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(autoIdMode()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setAutoIdMode((AutoIdMode)s2i(opt->text()));
    }
    else if(a_path == "/v_arch/nmb" && ctrChkNode(opt)) {
	vector<string> list;
	valList(list);
	unsigned e_c = 0;
	for(unsigned iA = 0; iA < list.size(); iA++)
	    if(valAt(list[iA]).at().startStat()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),list.size(),e_c));
    }
    else if(a_path == "/br/va_" || a_path == "/v_arch/archs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> list;
	    valList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setAttr("id",list[iA])->setText(valAt(list[iA]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))	{ opt->setAttr("id", valAdd(opt->attr("id"))); valAt(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	valDel(opt->attr("id"), true);
    }
    else if(a_path == "/redund/restDtOverTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(rdRestDtOverTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setRdRestDtOverTm(s2r(opt->text()));
    }
    else if(a_path == "/redund/mArch") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    XMLNode *nId	= ctrMkNode("list",opt,-1,"/redund/mArch/id","",R_R_R_,"root",SARH_ID);
	    XMLNode *nNm	= ctrMkNode("list",opt,-1,"/redund/mArch/nm","",R_R_R_,"root",SARH_ID);
	    XMLNode *nStart	= ctrMkNode("list",opt,-1,"/redund/mArch/start","",RWRWR_,"root",SARH_ID);
	    XMLNode *nRdndt	= ctrMkNode("list",opt,-1,"/redund/mArch/rdndt","",RWRWR_,"root",SARH_ID);
	    XMLNode *nPrefRun	= ctrMkNode("list",opt,-1,"/redund/mArch/prefRun","",RWRWR_,"root",SARH_ID);
	    XMLNode *nRem	= ctrMkNode("list",opt,-1,"/redund/mArch/remoted","",R_R_R_,"root",SARH_ID);

	    vector<string> mls, cls;
	    modList(mls);
	    AutoHD<TMArchivator> arh;
	    for(unsigned iM = 0; iM < mls.size(); iM++) {
		//if(!at(mls[iM]).at().redntAllow()) continue;
		at(mls[iM]).at().messList(cls);
		for(unsigned iC = 0; iC < cls.size(); iC++) {
		    arh = at(mls[iM]).at().messAt(cls[iC]);
		    if(nId)	nId->childAdd("el")->setText(mls[iM]+"."+cls[iC]);
		    if(nNm)	nNm->childAdd("el")->setText(arh.at().name());
		    if(nStart)	nStart->childAdd("el")->setText(arh.at().startStat()?"1":"0");
		    if(nRdndt)	nRdndt->childAdd("el")->setText(i2s(arh.at().redntMode()));
		    if(nPrefRun)nPrefRun->childAdd("el")->setText(arh.at().redntRun());
		    if(nRem)	nRem->childAdd("el")->setText(arh.at().redntUse()?"1":"0");
		}
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR)) {
	    string col = opt->attr("col");
	    AutoHD<TMArchivator> arh = at(TSYS::strParse(opt->attr("key_id"),0,".")).at().messAt(TSYS::strParse(opt->attr("key_id"),1,"."));
	    if(col == "start")		s2i(opt->text()) ? arh.at().start() : arh.at().stop();
	    else if(col == "rdndt")	arh.at().setRedntMode(s2i(opt->text()));
	    else if(col == "prefRun")	arh.at().setRedntRun(opt->text());
	}
    }
    else if(a_path == "/redund/lsMode" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","<high>")->setText(_("<High level>"));
	opt->childAdd("el")->setAttr("id","<low>")->setText(_("<Low level>"));
	opt->childAdd("el")->setAttr("id","<optimal>")->setText(_("<Optimal>"));
	vector<string> sls;
	SYS->rdStList(sls);
	for(unsigned iS = 0; iS < sls.size(); iS++)
	    opt->childAdd("el")->setAttr("id",sls[iS])->setText(SYS->transport().at().extHostGet("*",sls[iS]).name);
    }
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTypeArchivator                               *
//************************************************
TTypeArchivator::TTypeArchivator( const string &id ) : TModule(id)
{
    mVal = grpAdd("val_");
    mMess = grpAdd("mess_");
}

TTypeArchivator::~TTypeArchivator( )	{ nodeDelAll(); }

TArchiveS &TTypeArchivator::owner( ) const	{ return (TArchiveS &)TModule::owner(); }

string TTypeArchivator::messAdd( const string &iid, const string &idb )
{
    return chldAdd(mMess, AMess(TSYS::strEncode(sTrm(iid),TSYS::oscdID),idb));
}

string TTypeArchivator::valAdd( const string &iid, const string &idb )
{
    return chldAdd(mVal, AVal(TSYS::strEncode(sTrm(iid),TSYS::oscdID),idb));
}

void TTypeArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/arch",_("Archivers"));
	ctrMkNode("grp",opt,-1,"/br/mess_",_("Message archiver"),RWRWR_,"root",SARH_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	ctrMkNode("grp",opt,-1,"/br/val_",_("Value archiver"),RWRWR_,"root",SARH_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	ctrMkNode("list",opt,-1,"/arch/mess",_("Message archivers"),RWRWR_,"root",SARH_ID,5,
	    "tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref","mess_","idSz",i2s(limObjID_SZ).c_str());
	ctrMkNode("list",opt,-1,"/arch/val",_("Value archivers"),RWRWR_,"root",SARH_ID,5,
	    "tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref","val_","idSz",i2s(limObjID_SZ).c_str());
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/mess_" || a_path == "/arch/mess") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> list;
	    messList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setAttr("id",list[iA])->setText(messAt(list[iA]).at().name());
	}
	else if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))	{ opt->setAttr("id", messAdd(opt->attr("id"))); messAt(opt->attr("id")).at().setName(opt->text()); }
	else if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	messDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/val_" || a_path == "/arch/val") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> list;
	    valList(list);
	    for(unsigned iA = 0; iA < list.size(); iA++)
		opt->childAdd("el")->setAttr("id",list[iA])->setText(valAt(list[iA]).at().name());
	}
	else if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))	{ opt->setAttr("id", valAdd(opt->attr("id"))); valAt(opt->attr("id")).at().setName(opt->text()); }
	else if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	valDel(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* Message archiver                             *
//************************************************

//************************************************
//* TMArchivator                                 *
//************************************************
TMArchivator::TMArchivator( const string &iid, const string &idb, TElem *cf_el ) :
    TConfig(cf_el), runSt(false), messHead(-1), mId(cfg("ID")), mLevel(cfg("LEVEL")), mStart(cfg("START").getBd()),
    mDB(idb), mRdUse(true), mRdFirst(true)
{
    mId = iid;
}

TCntrNode &TMArchivator::operator=( const TCntrNode &node )
{
    const TMArchivator *src_n = dynamic_cast<const TMArchivator*>(&node);
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;ADDR;START;");
    cfg("MODUL").setS(owner().modId());
    setDB(src_n->DB());

    //if(src_n->startStat() && toStart() && !startStat()) start();

    return *this;
}

void TMArchivator::postEnable( int flag )	{ cfg("MODUL").setS(owner().modId()); }

void TMArchivator::preDisable( int flag )
{
    if(startStat()) stop();
}

void TMArchivator::postDisable( int flag )
{
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor), SYS->archive().at().nodePath()+tbl(), *this, TBDS::UseAllKeys);

	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

TTypeArchivator &TMArchivator::owner( ) const	{ return *(TTypeArchivator*)nodePrev(); }

string TMArchivator::workId( )		{ return string(owner().modId())+"."+id(); }

string TMArchivator::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TMArchivator::tbl( )	{ return string(owner().owner().subId())+"_mess_proc"; }

void TMArchivator::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB()))	throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);

    mRdUse = redntMode();
}

void TMArchivator::save_( )
{
    TBDS::dataSet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
    setDB(DB(), true);
}

void TMArchivator::redntDataUpdate( )
{
    vector<TMess::SRec> mess;

    //Init the point from which the archives sync
    bool isInitial = !lstRdMess.time;
    if(isInitial) lstRdMess = TMess::SRec(vmax(0,(end()?end():SYS->sysTm())-(time_t)(owner().owner().rdRestDtOverTm()*86400)));

    //First start replay of the local archive for the active alarms invoking time update
    //!!!! But that is spare since of reading all active alarms before
    /*if(isInitial && end() > lstRdMess.time) {
	get(lstRdMess.time, end(), mess);
	owner().owner().messPut(mess, ARCH_ALRM_CH);
    }*/

    //Preparing and call the request for messages
    // end()+1 used for decrease traffic by request end() messages in each cycle. The messages in <= end() will transfer direct.
    XMLNode req("get");
    req.setAttr("path", nodePath()+"/%2fserv%2fmess")->
	setAttr("bTm", ll2s(lstRdMess.time /*vmax(0,end()+1-(mRdFirst?owner().owner().rdRestDtOverTm()*86400:0))*/));
    if(isInitial && owner().owner().rdRestDtOverTm())
	req.setAttr("eTm", ll2s(owner().owner().mRdAlarms));

    //Send request to first active station for the archiver
    if(owner().owner().rdStRequest(workId(),req,"",!mRdFirst).empty()) return;
    mRdFirst = false;

    //Processing the result
    mess.clear();
    XMLNode *mO = NULL;
    TMess::SRec lstRdMess_;
    for(unsigned iM = 0; iM < req.childSize(); ++iM)
	if((mO=req.childGet(iM)) && mO->name() == "it") {
	    mess.push_back(TMess::SRec(s2ll(mO->attr("tm")),s2i(mO->attr("tmu")),mO->attr("cat"),s2i(mO->attr("lev")),mO->text()));
	    if(mess.back().time >= lstRdMess_.time) {
		lstRdMess_ = mess.back();
		if(lstRdMess_ == lstRdMess) mess.pop_back();
	    }
	}

    // The bottom time border processing
    if(lstRdMess_.time > lstRdMess.time) lstRdMess = lstRdMess_;
    else if(lstRdMess_.time) lstRdMess = TMess::SRec(lstRdMess_.time+1);

    if(mess_lev() == TMess::Debug)
	mess_debug(nodePath().c_str(), "Redundancy %s: %d", TSYS::atime2str(lstRdMess.time).c_str(), req.childSize());

    put(mess, true);

    //Just for alarms and not for the initial
    if(!(isInitial && owner().owner().rdRestDtOverTm()))
	owner().owner().messPut(mess, ARCH_ALRM);
}

void TMArchivator::start( )
{
    messHead = -1;

    runSt = true;
    owner().owner().setActMess(this, true);
}

void TMArchivator::stop( )
{
    owner().owner().setActMess(this, false);
    runSt = false;

    messHead = -1;
}

bool TMArchivator::put( vector<TMess::SRec> &mess, bool force )
{
    //Redundancy process
    if(!force && redntMode()) {
	XMLNode req("put");
	req.setAttr("path", nodePath()+"/%2fserv%2fmess")->setAttr("redundancy", "1");
	if(redntUse()) {	//for slave
	    vector<TMess::SRec> messLoc;
	    for(unsigned iM = 0; iM < mess.size(); ++iM) {
		if(!chkMessOK(mess[iM].categ,mess[iM].level)) continue;
		req.childAdd("it")->setAttr("tm", ll2s(mess[iM].time))->setAttr("tmu", i2s(mess[iM].utime))->
				    setAttr("cat", mess[iM].categ)->setAttr("lev", i2s(mess[iM].level))->setText(mess[iM].mess);
		if(mess[iM].time < end()) messLoc.push_back(mess[iM]);
	    }
	    mess = messLoc;
	    if(req.childSize()) return owner().owner().rdStRequest(workId(),req).size();
	    return true;
	}
	else {			//for master
	    for(unsigned iM = 0; iM < mess.size(); ++iM)
		if(chkMessOK(mess[iM].categ,mess[iM].level) && mess[iM].time <= end())
		    req.childAdd("it")->setAttr("tm", ll2s(mess[iM].time))->setAttr("tmu", i2s(mess[iM].utime))->
					setAttr("cat", mess[iM].categ)->setAttr("lev", i2s(mess[iM].level))->setText(mess[iM].mess);
	    if(req.childSize()) {
		string lstStat;
		int successWrs = 0;
		while((lstStat=owner().owner().rdStRequest(workId(),req,lstStat,false)).size()) successWrs++;	//To all hosts
		return successWrs;
	    }
	    return true;
	}
    }

    return false;
}

void TMArchivator::categ( vector<string> &list )
{
    list.clear();
    string c_vl, cat = cfg("CATEG").getS();
    for(int i_off = 0; (c_vl=TSYS::strSepParse(cat,0,';',&i_off)).size(); )
	list.push_back(c_vl);
}

bool TMArchivator::chkMessOK( const string &icateg, int8_t ilvl )
{
    vector<string> cat_ls;

    categ(cat_ls);

    if(TMess::messLevelTest(level(),ilvl))
	for(unsigned iCat = 0; iCat < cat_ls.size(); iCat++)
	    if(TRegExp(cat_ls[iCat], "p").test(icateg))
		return true;

   return false;
}

TVariant TMArchivator::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // bool status( ) - get the archiver start status.
    if(iid == "status")	return startStat();
    // int end( ) - get the archiver data end time.
    if(iid == "end")	return (int64_t)end();
    // int begin( ) - get the archiver data begin time.
    if(iid == "begin")	return (int64_t)begin();

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user, RWRWR_, "root:" SARH_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TMArchivator::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    if(a_path == "/serv/mess") {
	vector<TMess::SRec> mess;
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    time_t eTm = s2ll(opt->attr("eTm"));
	    opt->setAttr("tm", ll2s(get(s2ll(opt->attr("bTm")),(eTm?eTm:SYS->sysTm()),mess,opt->attr("cat"),s2i(opt->attr("lev")))));
	    for(unsigned iM = 0; iM < mess.size(); ++iM)
		opt->childAdd("it")->setAttr("tm",ll2s(mess[iM].time))->setAttr("tmu",i2s(mess[iM].utime))->
				     setAttr("cat",mess[iM].categ)->setAttr("lev",i2s(mess[iM].level))->setText(mess[iM].mess);
	}
	else if(ctrChkNode(opt,"put",RWRWR_,"root",SARH_ID,SEC_WR)) {
	    XMLNode *mO = NULL;
	    for(unsigned iM = 0; iM < opt->childSize(); ++iM)
		if((mO=opt->childGet(iM)) && mO->name() == "it")
		    mess.push_back(TMess::SRec(s2ll(mO->attr("tm")),s2i(mO->attr("tmu")),mO->attr("cat"),s2i(mO->attr("lev")),mO->text()));
	    bool isRd = s2i(opt->attr("redundancy"));
	    if(isRd) owner().owner().messPut(mess, ARCH_ALRM_CH);	//just to update the presented alarms
	    put(mess, isRd);
	    opt->childClear("it");
	}
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Message archiver: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archiver"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archiver DB"),RWRWR_,"root","root",4,
		    "tp","str","dest","select","select","/db/list",
		    "help",(string(TMess::labStor())+"\n"+TMess::labStorGen()).c_str());
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/prm/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SARH_ID);
		ctrMkNode("fld",opt,-1,"/prm/st/end",_("End"),R_R_R_,"root","root",1,"tp","time");
		ctrMkNode("fld",opt,-1,"/prm/st/beg",_("Begin"),R_R_R_,"root","root",1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SARH_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/MODUL");
		ctrRemoveNode(opt,"/prm/cfg/REDNT");
		ctrRemoveNode(opt,"/prm/cfg/REDNT_RUN");
		ctrMkNode("fld",opt,-1,"/prm/cfg/LEVEL",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Receiving messages with a level greater or equal to the specified."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/CATEG",EVAL_STR,RWRWR_,"root",SARH_ID,1,"help",TMess::labMessCat());
	    }
	}
	if(runSt && ctrMkNode("area",opt,-1,"/mess",_("Messages"),R_R___,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time, size (seconds) and level"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size","",RWRW__,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/lvl","",RWRW__,"root",SARH_ID,5,"tp","dec", "dest","select",
		"sel_id","0;1;2;3;4;5;6;7",
		"sel_list",_("Debug (0);Information (1[X]);Notice (2[X]);Warning (3[X]);Error (4[X]);Critical (5[X]);Alert (6[X]);Emergency (7[X])"),
		"help",_("Receiving messages with a level greater or equal to the specified."));
	    ctrMkNode("fld",opt,-1,"/mess/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str", "help",TMess::labMessCat());
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SARH_ID)) {
		ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/mess/mess/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mess/mess/2",_("Level"),R_R___,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/3",_("Message"),R_R___,"root",SARH_ID,1,"tp","str");
	    }
	}
	return;
    }

    //Process command to page
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(startStat() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	s2i(opt->text()) ? start() : stop();
    }
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/prm/st/end" && ctrChkNode(opt))		opt->setText(i2s(end()));
    else if(a_path == "/prm/st/beg" && ctrChkNode(opt))		opt->setText(i2s(begin()));
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SARH_ID,RWRWR_);
    else if(a_path == "/mess/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD)) {
	    opt->setText(TBDS::genPrmGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text()))	opt->setText(i2s(TSYS::curTime()/1000000));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genPrmSet(nodePath()+"messTm",(s2i(opt->text())>=TSYS::curTime()/1000000)?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messSize","10",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/cat") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/lvl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/mess" && runSt && ctrChkNode(opt,"get",R_R___,"root",SARH_ID)) {
	vector<TMess::SRec> rec;
	time_t end = s2i(TBDS::genPrmGet(nodePath()+"messTm","0",opt->attr("user")));
	if(!end) end = (time_t)(TSYS::curTime()/1000000);
	time_t beg = end - s2i(TBDS::genPrmGet(nodePath()+"messSize","10",opt->attr("user")));
	string cat = TBDS::genPrmGet(nodePath()+"messCat","",opt->attr("user"));
	char   lev = s2i(TBDS::genPrmGet(nodePath()+"messLev","0",opt->attr("user")));

	int64_t stTm = 0;
	if(mess_lev() == TMess::Debug) stTm = TSYS::curTime();

	get(beg, end, rec, cat, lev);

	XMLNode *nTm	= ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SARH_ID);
	XMLNode *nTmu	= ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SARH_ID);
	XMLNode *nCat	= ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SARH_ID);
	XMLNode *nLvl	= ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SARH_ID);
	XMLNode *nMess	= ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SARH_ID);
	for(int iRec = rec.size()-1; iRec >= 0; iRec--) {
	    if(nTm)	nTm->childAdd("el")->setText(i2s(rec[iRec].time));
	    if(nTmu)	nTmu->childAdd("el")->setText(i2s(rec[iRec].utime));
	    if(nCat)	nCat->childAdd("el")->setText(rec[iRec].categ);
	    if(nLvl)	nLvl->childAdd("el")->setText(i2s(rec[iRec].level));
	    if(nMess)	nMess->childAdd("el")->setText(rec[iRec].mess);
	}

	if(mess_lev() == TMess::Debug)
	    mess_sys(TMess::Debug, _("Requested %d records for time %s."), rec.size(), tm2s(1e-6*(TSYS::curTime()-stTm)).c_str());
    }
    else TCntrNode::cntrCmdProc(opt);
}

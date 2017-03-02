
//OpenSCADA system file: tarchives.cpp
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
    TSubSYS(SARH_ID,_("Archives"),true), elMess(""), elVal(""), elAval(""), bufErr(0), mMessPer(10), prcStMess(false), mRes(true),
    headBuf(0), vRes(true), mValPer(1000), mValPrior(10), mValForceCurTm(false),
    prcStVal(false), endrunReqVal(false), toUpdate(false)
{
    mAval = grpAdd("va_");

    //Message archivator DB structure
    elMess.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    elMess.fldAdd(new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    elMess.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    elMess.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"200"));
    elMess.fldAdd(new TFld("START",_("To start"),TFld::Boolean,0,"1"));
    elMess.fldAdd(new TFld("CATEG",_("Messages categories"),TFld::String,0,"100"));
    elMess.fldAdd(new TFld("LEVEL",_("Messages level"),TFld::Integer,TFld::Selected,"1","0",
	"0;1;2;3;4;5;6;7",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)")));
    elMess.fldAdd(new TFld("ADDR",_("Address"),TFld::String,0,"100"));

    //Value archivator DB structure
    elVal.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    elVal.fldAdd(new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    elVal.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    elVal.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"200"));
    elVal.fldAdd(new TFld("START",_("To start"),TFld::Boolean,0,"1","0"));
    elVal.fldAdd(new TFld("ADDR",_("Address"),TFld::String,0,"100"));
    elVal.fldAdd(new TFld("V_PER",_("Value period (sec)"),TFld::Real,0,"12.6","1","0;100000"));
    elVal.fldAdd(new TFld("A_PER",_("Period archiving (sec)"),TFld::Integer,0,"4","60","0;1000"));
    elVal.fldAdd(new TFld("SEL_PR",_("Selection priority"),TFld::Integer,0,"4","10","0;1000"));

    //Value archive DB structure
    elAval.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"20"));
    elAval.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    elAval.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"200"));
    elAval.fldAdd(new TFld("START",_("To start"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("SrcMode",_("Source"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d",TVArchive::Passive,TVArchive::PassiveAttr,TVArchive::ActiveAttr).c_str(),
	_("Passive;Passive param. attribute;Active param. attribute")));
    elAval.fldAdd(new TFld("Source",_("Source"),TFld::String,0,"100"));
    elAval.fldAdd(new TFld("VTYPE",_("Value type"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d;%d",TFld::Boolean,TFld::Integer,TFld::Real,TFld::String).c_str(),_("Boolean;Integer;Real;String")));
    elAval.fldAdd(new TFld("BPER",_("Buffer period (sec)"),TFld::Real,0,"9.6","1","0;10000"));
    elAval.fldAdd(new TFld("BSIZE",_("Buffer size (items)"),TFld::Integer,0,"8","100","10;10000000"));
    elAval.fldAdd(new TFld("BHGRD",_("Buffer in hard time grid"),TFld::Boolean,0,"1","1"));
    elAval.fldAdd(new TFld("BHRES",_("Buffer in high time resolution"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("FillLast",_("Fill pass points to a last value"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("ArchS",_("Process into archivators"),TFld::String,0,"500"));

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
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters
    setMessBufLen(s2i(TBDS::genDBGet(nodePath()+"MessBufSize",i2s(messBufLen()))));
    setMessPeriod(s2i(TBDS::genDBGet(nodePath()+"MessPeriod",i2s(messPeriod()))));
    setValPeriod(s2i(TBDS::genDBGet(nodePath()+"ValPeriod",i2s(valPeriod()))));
    setValPrior(s2i(TBDS::genDBGet(nodePath()+"ValPriority",i2s(valPrior()))));
    setValForceCurTm(s2i(TBDS::genDBGet(nodePath()+"ValForceCurTm",i2s(valForceCurTm()))));

    //LidDB
    // Message archivators load
    string id,type;
    map<string, bool>	itReg;
    vector<vector<string> > full;
    try {
	TConfig cEl(&elMess);
	//cEl.cfgViewAll(false);
	vector<string> dbLs;

	// Search int DB and create new archivators
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+subId()+"_mess_proc",nodePath()+subId()+"_mess_proc",fldCnt++,cEl,false,&full); ) {
		id = cEl.cfg("ID").getS();
		type = cEl.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().messPresent(id))
		    at(type).at().messAdd(id,(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		at(type).at().messAt(id).at().load(&cEl);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    vector<string> m_ls;
	    modList(m_ls);
	    for(unsigned i_m = 0; i_m < m_ls.size(); i_m++) {
		at(m_ls[i_m]).at().messList(dbLs);
		for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		    if(itReg.find(m_ls[i_m]+"."+dbLs[i_it]) == itReg.end() && SYS->chkSelDB(at(m_ls[i_m]).at().messAt(dbLs[i_it]).at().DB()))
			at(m_ls[i_m]).at().messDel(dbLs[i_it]);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_sys(TMess::Error, _("Message archivators load error."));
    }

    // Value archivators load
    try
    {
	TConfig cEl(&elVal);
	//cEl.cfgViewAll(false);
	vector<string> dbLs;
	itReg.clear();

	//  Search into DB and create new archivators
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+subId()+"_val_proc",nodePath()+subId()+"_val_proc",fldCnt++,cEl,false,&full); ) {
		id = cEl.cfg("ID").getS();
		type = cEl.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().valPresent(id))
		    at(type).at().valAdd(id,(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		at(type).at().valAt(id).at().load(&cEl);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    vector<string> m_ls;
	    modList(m_ls);
	    for(unsigned i_m = 0; i_m < m_ls.size(); i_m++) {
		at(m_ls[i_m]).at().valList(dbLs);
		for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		    if(itReg.find(m_ls[i_m]+"."+dbLs[i_it]) == itReg.end() && SYS->chkSelDB(at(m_ls[i_m]).at().valAt(dbLs[i_it]).at().DB()))
			at(m_ls[i_m]).at().valDel(dbLs[i_it]);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_sys(TMess::Error, _("Value archivators load error."));
    }

    // Value archives load
    try
    {
	TConfig cEl(&elAval);
	//cEl.cfgViewAll(false);
	vector<string> dbLs;
	itReg.clear();

	//  Search into DB and create new archives
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(int fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+subId()+"_val",nodePath()+subId()+"_val",fldCnt++,cEl,false,&full); ) {
		id = cEl.cfg("ID").getS();
		if(!valPresent(id)) valAdd(id,(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		//   For force loading after creation from archivator storage
		else if(valAt(id).at().DB() == "*.*" && dbLs[iDB] != SYS->workDB()) valAt(id).at().setDB(dbLs[iDB]);
		valAt(id).at().load(&cEl);
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty())
	{
	    valList(dbLs);
	    for(unsigned i_it = 0; i_it < dbLs.size(); i_it++)
		if(itReg.find(dbLs[i_it]) == itReg.end() && SYS->chkSelDB(valAt(dbLs[i_it]).at().DB()))
		    valDel(dbLs[i_it]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_sys(TMess::Error, _("Value archives load error."));
    }
}

void TArchiveS::save_( )
{
    //Save parameters
    TBDS::genDBSet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen()));
    TBDS::genDBSet(nodePath()+"MessPeriod",TSYS::int2str(messPeriod()));
    TBDS::genDBSet(nodePath()+"ValPeriod",TSYS::int2str(valPeriod()));
    TBDS::genDBSet(nodePath()+"ValPriority",TSYS::int2str(valPrior()));
    TBDS::genDBSet(nodePath()+"ValForceCurTm",i2s(valForceCurTm()));
}

void TArchiveS::valAdd( const string &iid, const string &idb )
{
    if( valPresent(iid) ) return;
    chldAdd(mAval,new TVArchive(iid,idb,&aValE()));
}

string TArchiveS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf, sizeof(buf), _(
	"======================== Subsystem \"Archives\" options ===================\n"
	"------------ Parameters of section '%s' in config-file -----------\n"
	"MessBufSize   <items>       Messages buffer size.\n"
	"MessPeriod    <sec>         Message archiving period.\n"
	"ValPeriod     <msec>        Values archiving period.\n"
	"ValPriority   <level>	     Values task priority level.\n"
	"MaxReqMess    <items>       Maximum request messages.\n"
	"MaxReqVals    <items>       Maximum request values.\n\n"
	), nodePath().c_str());

    return buf;
}

void TArchiveS::subStart( )
{
    mess_sys(TMess::Debug, _("Start/update subsystem."));

    subStarting = true;
    toUpdate = false;	//Moved to start for prevent possible changes the toUpdate at processing

    vector<string> tLst, oLst;

    bool stArchM = false, stArchV = false;
    modList(tLst);
    //Start no started early archivators and check for already started
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	AutoHD<TTipArchivator> mod = modAt(tLst[iT]);
	//Messages
	mod.at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(oLst[iO]);
	    if(mess.at().startStat()) stArchM = true;
	    else if(mess.at().toStart())
		try { mess.at().start(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Message archivator '%s' start error."), oLst[iO].c_str());
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
		    mess_sys(TMess::Error, _("Value archivator '%s' start error."), val.at().workId().c_str());
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
		mess_sys(TMess::Error, _("Value archive '%s' start error."), oLst[iO].c_str());
	    }
    }

    //Start already started for update
    for(unsigned iT = 0; iT < tLst.size() && (stArchM || stArchV); iT++) {
	AutoHD<TTipArchivator> mod = modAt(tLst[iT]);
	//Messages
	mod.at().messList(oLst);
	for(unsigned iO = 0; stArchM && iO < oLst.size(); iO++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(oLst[iO]);
	    if(mess.at().startStat())
		try { mess.at().start(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Message archivator '%s' start error."), oLst[iO].c_str());
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
		    mess_sys(TMess::Error, _("Value archivator '%s' start error."), val.at().workId().c_str());
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

    //Archivators stop
    modList(tLst);
    for(unsigned iT = 0; iT < tLst.size(); iT++) {
	AutoHD<TTipArchivator> mod = modAt(tLst[iT]);
	// Value archives stop
	mod.at().valList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TVArchivator> val = mod.at().valAt(oLst[iO]);
	    if(val.at().startStat())
		try { val.at().stop(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Value archivator '%s' stop error."), oLst[iO].c_str());
		}
	}
	// Message archivators stop
	mod.at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size(); iO++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(oLst[iO]);
	    if(mess.at().startStat())
		try { mess.at().stop(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Message archivator '%s' stop error."), oLst[iO].c_str());
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
    map<string, bool> archMap;
    string tVl;
    for(int off = 0; (tVl=TSYS::strParse(arch,0,";",&off)).size(); ) archMap[tVl] = true;

    MtxAlloc res(mRes);
    //Alarms processing. For level less 0 alarm is set
    if(archMap.empty() || archMap[BUF_ARCH_NM] || archMap[ALRM_ARCH_NM]) {
	res.lock();
	map<string,TMess::SRec>::iterator p = mAlarms.find(categ);
	/*if(p != mAlarms.end() && level < 0 && abs(level) == p->second.level && SYS->rdEnable() &&
		mess == p->second.mess && FTM2(tm,utm) >= FTM(p->second))
	    return;*/		//Prevent for update equal alarm in redundancy
	if(level < 0 && (p == mAlarms.end() || FTM2(tm,utm) >= FTM(p->second) /*|| SYS->rdEnable()*/))
	    mAlarms[categ] = TMess::SRec(tm, utm, categ, (TMess::Type)abs(level), mess);
	if(level >= 0 && p != mAlarms.end() && FTM2(tm,utm) >= FTM(p->second)) mAlarms.erase(p);
	res.unlock();
    }

    //Put the message to the buffer
    if(archMap.empty() || archMap[BUF_ARCH_NM]) {
	res.lock();
	mBuf[headBuf].time  = tm;
	mBuf[headBuf].utime = utm;
	mBuf[headBuf].categ = categ;
	mBuf[headBuf].level = (TMess::Type)level;
	mBuf[headBuf].mess  = mess;
	if((++headBuf) >= mBuf.size()) headBuf = 0;

	//Check for the archivator's headers to messages buffer
	for(unsigned iM = 0; iM < actMess.size(); iM++) {
	    int &messHead = actMess[iM].at().messHead;
	    if(messHead >= 0 && messHead == (int)headBuf && ++messHead >= (int)mBuf.size()) messHead = 0;
	}
	res.unlock();
    }

    //Put message to the archive <arch>
    if(!archMap[BUF_ARCH_NM]) {
	vector<TMess::SRec> ml;
	ml.push_back(TMess::SRec(tm,utm,categ,level,mess));
	vector<string> tLst, oLst;
	modList(tLst);
	for(unsigned iT = 0; iT < tLst.size(); iT++) {
	    at(tLst[iT]).at().messList(oLst);
	    for(unsigned iO = 0; iO < oLst.size(); iO++) {
		AutoHD<TMArchivator> archtor = at(tLst[iT]).at().messAt(oLst[iO]);
		if(archtor.at().startStat() && (!archMap.size() || archMap[archtor.at().workId()]))
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
	messPut(recs[iR].time, recs[iR].utime, recs[iR].categ, recs[iR].level, recs[iR].mess, arch);
}

time_t TArchiveS::messGet( time_t bTm, time_t eTm, vector<TMess::SRec> &recs,
    const string &category, int8_t level, const string &arch, time_t upTo )
{
    time_t result = eTm;	//Means successful buffers processing
    recs.clear();

    map<string, bool> archMap;
    string tVl;
    for(int off = 0; (tVl=TSYS::strParse(arch,0,";",&off)).size(); ) archMap[tVl] = true;

    if(!upTo) upTo = time(NULL) + STD_INTERF_TM;
    TRegExp re(category, "p");

    //Get records from buffer
    MtxAlloc res(mRes, true);
    unsigned i_buf = headBuf;
    while(level >= 0 && (archMap.empty() || archMap[BUF_ARCH_NM]) /*&& time(NULL) < upTo*/) {
	if(mBuf[i_buf].time >= bTm && mBuf[i_buf].time && mBuf[i_buf].time <= eTm &&
		abs(mBuf[i_buf].level) >= level && re.test(mBuf[i_buf].categ))
	    recs.push_back(mBuf[i_buf]);
	if(++i_buf >= mBuf.size()) i_buf = 0;
	if(i_buf == headBuf) break;
    }
    res.unlock();

    //Get records from archives
    vector<string> tLst, oLst;
    modList(tLst);
    for(unsigned iT = 0; level >= 0 && iT < tLst.size(); iT++) {
	at(tLst[iT]).at().messList(oLst);
	for(unsigned iO = 0; iO < oLst.size() && time(NULL) < upTo; iO++) {
	    AutoHD<TMArchivator> archtor = at(tLst[iT]).at().messAt(oLst[iO]);
	    if(archtor.at().startStat() && (!archMap.size() || archMap[archtor.at().workId()]))
		//!! But possible only one archiver, from all, processing and next continued by the limit
		result = fmin(result, archtor.at().get(bTm,eTm,recs,category,level,arch.size()?upTo:0));
	}
    }

    //Alarms request processing
    if(level < 0)
    {
	res.lock();
	vector< pair<int64_t,TMess::SRec* > > mb;
	for(map<string,TMess::SRec>::iterator p = mAlarms.begin(); p != mAlarms.end() /*&& time(NULL) < upTo*/; p++)
	    if((p->second.time >= bTm || bTm == eTm) && p->second.time <= eTm &&
		    p->second.level >= abs(level) && re.test(p->second.categ))
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
    if(arch.empty() || arch == BUF_ARCH_NM)
    {
	unsigned i_buf = headBuf;
	while(!arch.size() || arch == BUF_ARCH_NM)
	{
	    rez = rez ? vmin(rez,mBuf[i_buf].time) : mBuf[i_buf].time;
	    if(++i_buf >= mBuf.size()) i_buf = 0;
	    if(i_buf == headBuf) break;
	}
	if( !arch.empty() ) return rez;
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
    if(arch.empty() || arch == BUF_ARCH_NM)
    {
	unsigned i_buf = headBuf;
	while(!arch.size() || arch == BUF_ARCH_NM)
	{
	    rez = rez ? vmax(rez,mBuf[i_buf].time) : mBuf[i_buf].time;
	    if(++i_buf >= mBuf.size()) i_buf = 0;
	    if(i_buf == headBuf) break;
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

void TArchiveS::setMessBufLen( unsigned len )
{
    MtxAlloc res(mRes, true);
    len = vmin(BUF_SIZE_MAX, vmax(BUF_SIZE_DEF,len));
    while(mBuf.size() > len)
    {
	mBuf.erase(mBuf.begin() + headBuf);
	if(headBuf >= mBuf.size())	headBuf = 0;
	for(unsigned iM = 0; iM < actMess.size(); iM++) {
	    int &messHead = actMess[iM].at().messHead;
	    if(messHead >= 0 && messHead >= (int)mBuf.size()) messHead = mBuf.size()-1;
	}
	//if(headLstread >= mBuf.size())	headLstread = mBuf.size()-1;
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
    while(true)
    {
	if(TSYS::taskEndRun()) isLast = true;
	//Message buffer read
	MtxAlloc res(arh.mRes, true);
	for(unsigned iM = 0; iM < arh.actMess.size(); iM++) {
	    AutoHD<TMArchivator> mArh = arh.actMess[iM];
	    int &messHead = mArh.at().messHead;
	    if(messHead < 0 && ((messHead=arh.headBuf+1) >= (int)arh.mBuf.size() || !arh.mBuf[messHead].time)) messHead = 0;
	    if(messHead == (int)arh.headBuf)	continue;
	    try
	    {
		// Get new messages
		unsigned newHeadLstread = arh.headBuf;
		unsigned iM = messHead;
		vector<TMess::SRec> oMess;
		while(iM != newHeadLstread)
		{
		    oMess.push_back(arh.mBuf[iM]);
		    if(++iM >= arh.mBuf.size()) iM = 0;
		}
		res.unlock();

		bool rez = mArh.at().put(oMess);

		res.lock();
		if(rez) messHead = newHeadLstread;
	    } catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		arh.mess_sys(TMess::Error, _("Message buffer process error."));
	    }
	    res.lock();
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
    TArchiveS &arh = *(TArchiveS *)param;
    arh.endrunReqVal = false;
    arh.prcStVal = true;

    while(!arh.endrunReqVal)
    {
	int64_t work_tm = SYS->curTime();

	arh.vRes.lock();
	int64_t stm = arh.valForceCurTm() ? TSYS::curTime() : 0;
	for(unsigned iArh = 0; iArh < arh.actVal.size(); iArh++)
	    try
	    {
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
    //     - request of the system messages for the time from <btm>
    //       to <etm> for the category <cat>, level <lev> and archivers <arch>
    //  btm - begin time
    //  etm - end time
    //  cat - messages' category
    //  lev - messages level
    //  arch - message archivators by list items separated ';'
    //  upTm - sets the operation continuance limit to time; a negative value used as relative time; less to STD_INTERF_TM (5).
    if( iid == "messGet" && prms.size() >= 2 )
    {
	vector<TMess::SRec> recs;
	int upTm = (prms.size() >= 6) ? prms[5].getI() : 0;
	time_t result = messGet(prms[0].getI(), prms[1].getI(), recs, ((prms.size()>=3) ? prms[2].getS() : string("")),
				((prms.size()>=4) ? prms[3].getI() : 0), ((prms.size()>=5) ? prms[4].getS() : string("")),
				vmin((upTm<0)?time(NULL)+abs(upTm):upTm,time(NULL)+STD_INTERF_TM));
	TArrayObj *rez = new TArrayObj();
	rez->propSet("tm", (int)result);
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
    //  arch - archivators by list items separated ';';
    //         zero or "<buffer>" cause to generic writing to the buffer and alarms (lev <0) else direct to the pointed archivators
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
    if(a_path == "/serv/mess")		//Messages access
    {
	if(ctrChkNode(opt,"info",RWRWR_,"root",SARH_ID,SEC_RD))		//Messages information
	{
	    string arch = opt->attr("arch");
	    opt->setAttr("end",TSYS::uint2str(messEnd(arch)));
	    opt->setAttr("beg",TSYS::uint2str(messBeg(arch)));
	}
	else if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	//Value's data request
	{
	    time_t tm      = strtoul(opt->attr("tm").c_str(),0,10);
	    time_t tm_grnd = strtoul(opt->attr("tm_grnd").c_str(),0,10);
	    string arch    = opt->attr("arch");
	    string cat     = opt->attr("cat");
	    int    lev     = s2i(opt->attr("lev"));
	    vector<TMess::SRec> rez;

	    if(!tm) { tm = time(NULL); opt->setAttr("tm", u2s(tm)); }
	    opt->setAttr("tm", ll2s(messGet(tm_grnd,tm,rez,cat,(TMess::Type)lev,arch)));
	    for(unsigned i_r = 0; i_r < rez.size(); i_r++)
		opt->childAdd("el")->
		    setAttr("time",TSYS::uint2str(rez[i_r].time))->
		    setAttr("utime",TSYS::uint2str(rez[i_r].utime))->
		    setAttr("cat",rez[i_r].categ)->
		    setAttr("lev",TSYS::int2str(rez[i_r].level))->
		    setText(rez[i_r].mess);
	}
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	//Value's data set
	    for(unsigned i_r = 0; i_r < opt->childSize(); i_r++)
	    {
		XMLNode *mNd = opt->childGet(i_r);
		messPut(strtol(mNd->attr("time").c_str(),NULL,10),
			s2i(mNd->attr("utime")),
			mNd->attr("cat"), s2i(mNd->attr("lev")), mNd->text());
	    }
	return;
    }

    //Get page info
    if(opt->name() == "info")
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/va_",_("Value archive"),RWRWR_,"root",SARH_ID,2,"idm",OBJ_NM_SZ,"idSz","20");
	if(ctrMkNode("area",opt,1,"/m_arch",_("Messages archive"),R_R_R_,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/m_arch/size",_("Messages buffer size"),RWRWR_,"root",SARH_ID,2,"tp","dec","min",TSYS::int2str(BUF_SIZE_DEF).c_str());
	    ctrMkNode("fld",opt,-1,"/m_arch/per",_("Archiving period (s)"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    if(ctrMkNode("area",opt,-1,"/m_arch/view",_("View messages"),R_R___,"root",SARH_ID))
	    {
		ctrMkNode("fld",opt,-1,"/m_arch/view/tm",_("Time, size (s) and level"),RWRW__,"root",SARH_ID,1,"tp","time");
		ctrMkNode("fld",opt,-1,"/m_arch/view/size","",RWRW__,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/m_arch/view/lvl","",RWRW__,"root",SARH_ID,5,"tp","dec", "dest","select",
		    "sel_id","0;1;2;3;4;5;6;7;-1;-2;-3;-4;-5;-6;-7",
		    "sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7);"
			         "Information (1), ALARMS;Notice (2), ALARMS;Warning (3), ALARMS;Error (4), ALARMS;Critical (5), ALARMS;Alert (6), ALARMS;Emergency (7), ALARMS"),
		    "help",_("Get messages for level more and equal it."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str","help",
		    _("Messages category template or regular expression.\n"
		      "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol.\n"
		      "Regular expression enclosed in symbols '/' (/mod_(System|LogicLev)/)."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/archtor",_("Archivators"),RWRW__,"root",SARH_ID,4,"tp","str","dest","sel_ed","select","/m_arch/lstAMess",
		    "help",_("Message archivators.\nNo set archivator for process by buffer and all archivators.\nSet '<buffer>' for process by buffer."));
		if(ctrMkNode("table",opt,-1,"/m_arch/view/mess",_("Messages"),R_R___,"root",SARH_ID))
		{
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/1",_("Category"),R_R___,"root",SARH_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/2",_("Lev."),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/3",_("Message"),R_R___,"root",SARH_ID,1,"tp","str");
		}
	    }
	}
	if(ctrMkNode("area",opt,2,"/v_arch",_("Value archives"),R_R_R_,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/v_arch/per",_("Get data period (ms)"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/prior",_("Get data task priority level"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/fCurTm",_("Variable timestamp force to current time"),RWRWR_,"root",SARH_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/v_arch/nmb",_("Number"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/v_arch/archs",_("Value archives"),RWRWR_,"root",SARH_ID,5,"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","va_","idSz","20");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SARH_ID,3,"tp","str","cols","90","rows","10");
	return;
    }

    //Process command to page
    if(a_path == "/m_arch/per")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(messPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessPeriod(s2i(opt->text()));
    }
    else if(a_path == "/m_arch/size")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(messBufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessBufLen(s2i(opt->text()));
    }
    else if(a_path == "/m_arch/view/tm")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))
	{
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text()))	opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(nodePath()+"messTm",(s2i(opt->text())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/size")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/cat")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/lvl")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/archtor")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messArch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/lstAMess" && ctrChkNode(opt,"get",R_R___))
    {
	map<string, bool> itsMap;
	itsMap[BUF_ARCH_NM] = true;
	vector<string> lsm, lsa;
	modList(lsm);
	for(unsigned iM = 0; iM < lsm.size(); iM++) {
	    at(lsm[iM]).at().messList(lsa);
	    for(unsigned i_a = 0; i_a < lsa.size(); i_a++)
		itsMap[lsm[iM]+"."+lsa[i_a]] = true;
	}

	string curVal = TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")), tVl, tVl1;
	for(int off = 0; (tVl=TSYS::strParse(curVal,0,";",&off)).size(); tVl1 += (tVl1.size()?";":"")+tVl)
	{ opt->childAdd("el")->setText(tVl1); itsMap[tVl] = false; }

	for(map<string, bool>::iterator iM = itsMap.begin(); iM != itsMap.end(); ++iM)
	    if(iM->second) opt->childAdd("el")->setText(curVal+(curVal.size()?";":"")+iM->first);
    }
    else if(a_path == "/m_arch/view/mess" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID))
    {
	vector<TMess::SRec> rec;
	time_t gtm = s2i(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	if(!gtm) gtm = time(NULL);
	int gsz = s2i(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")));
	messGet(gtm-gsz, gtm, rec,
		TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")),
		s2i(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user"))),
		TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));

	XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0","",R_R___,"root",SARH_ID);
	XMLNode *n_tmu	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a","",R_R___,"root",SARH_ID);
	XMLNode *n_cat	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/1","",R_R___,"root",SARH_ID);
	XMLNode *n_lvl	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/2","",R_R___,"root",SARH_ID);
	XMLNode *n_mess	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/3","",R_R___,"root",SARH_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--)
	{
	    if(n_tm)	n_tm->childAdd("el")->setText(TSYS::int2str(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(TSYS::int2str(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else if(a_path == "/v_arch/per")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(valPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPeriod(s2i(opt->text()));
    }
    else if(a_path == "/v_arch/prior")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(valPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPrior(s2i(opt->text()));
    }
    else if(a_path == "/v_arch/fCurTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(valForceCurTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValForceCurTm(s2i(opt->text()));
    }
    else if(a_path == "/v_arch/nmb" && ctrChkNode(opt)) {
	vector<string> list;
	valList(list);
	unsigned e_c = 0;
	for(unsigned i_a = 0; i_a < list.size(); i_a++)
	    if(valAt(list[i_a]).at().startStat()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),list.size(),e_c));
    }
    else if(a_path == "/br/va_" || a_path == "/v_arch/archs")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))
	{
	    vector<string> list;
	    valList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    valAdd(vid); valAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	chldDel(mAval,opt->attr("id"),-1,1);
    }
    else if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID))	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTipArchivator                               *
//************************************************
TTipArchivator::TTipArchivator( const string &id ) : TModule(id)
{
    mVal = grpAdd("val_");
    mMess = grpAdd("mess_");
}

TTipArchivator::~TTipArchivator( )	{ nodeDelAll(); }

TArchiveS &TTipArchivator::owner( ) const	{ return (TArchiveS &)TModule::owner(); }

void TTipArchivator::messAdd(const string &name, const string &idb )	{ chldAdd(mMess, AMess(name,idb)); }

void TTipArchivator::valAdd( const string &iid, const string &idb )	{ chldAdd(mVal, AVal(iid,idb)); }

void TTipArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/arch",_("Archivators"));
	ctrMkNode("grp",opt,-1,"/br/mess_",_("Message archivator"),RWRWR_,"root",SARH_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	ctrMkNode("grp",opt,-1,"/br/val_",_("Value archivator"),RWRWR_,"root",SARH_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	ctrMkNode("list",opt,-1,"/arch/mess",_("Message archivators"),RWRWR_,"root",SARH_ID,5,
	    "tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","mess_","idSz",OBJ_ID_SZ);
	ctrMkNode("list",opt,-1,"/arch/val",_("Value archivators"),RWRWR_,"root",SARH_ID,5,
	    "tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","val_","idSz",OBJ_ID_SZ);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/mess_" || a_path == "/arch/mess")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))
	{
	    vector<string> list;
	    messList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(messAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    messAdd(vid); messAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	messDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/val_" || a_path == "/arch/val")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))
	{
	    vector<string> list;
	    valList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    valAdd(vid); valAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	valDel(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* Message archivator                           *
//************************************************

//************************************************
//* TMArchivator                                 *
//************************************************
TMArchivator::TMArchivator(const string &iid, const string &idb, TElem *cf_el) :
    TConfig(cf_el), runSt(false), messHead(-1), mId(cfg("ID")), mLevel(cfg("LEVEL")), mStart(cfg("START").getBd()), mDB(idb)
{
    mId = iid;
}

TCntrNode &TMArchivator::operator=( const TCntrNode &node )
{
    const TMArchivator *src_n = dynamic_cast<const TMArchivator*>(&node);
    if( !src_n ) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;");
    cfg("MODUL").setS(owner().modId());
    mDB = src_n->mDB;

    if( src_n->startStat() && toStart() && !startStat() )
        start( );

    return *this;
}

void TMArchivator::postEnable( int flag )	{ cfg("MODUL").setS(owner().modId()); }

void TMArchivator::preDisable( int flag )
{
    if(startStat()) stop();
}

void TMArchivator::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(), SYS->archive().at().nodePath()+tbl(), *this, true);
}

TTipArchivator &TMArchivator::owner( ) const	{ return *(TTipArchivator*)nodePrev(); }

string TMArchivator::workId( )		{ return string(owner().modId())+"."+id(); }

string TMArchivator::name()
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TMArchivator::tbl( )	{ return string(owner().owner().subId())+"_mess_proc"; }

void TMArchivator::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB()))	throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
}

void TMArchivator::save_( )	{ SYS->db().at().dataSet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this); }

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

    if(abs(ilvl) >= level())
	for(unsigned i_cat = 0; i_cat < cat_ls.size(); i_cat++)
	    if(TRegExp(cat_ls[i_cat], "p").test(icateg))
		return true;

   return false;
}

TVariant TMArchivator::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // bool status( ) - get the archivator start status.
    if(iid == "status")	return startStat();
    // int end( ) - get the archivator data end time.
    if(iid == "end")	return (int)end();
    // int begin( ) - get the archivator data begin time.
    if(iid == "begin")	return (int)begin();

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TMArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Message archivator: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archivator")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archivator DB"),RWRWR_,"root","root",4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/prm/st/end",_("End"),R_R_R_,"root","root",1,"tp","time");
		ctrMkNode("fld",opt,-1,"/prm/st/beg",_("Begin"),R_R_R_,"root","root",1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration")))
	    {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SARH_ID,RWRWR_);
		ctrMkNode("fld",opt,-1,"/prm/cfg/LEVEL",EVAL_STR,RWRWR_,"root",SARH_ID,1,"help",_("Get messages for level more and equal it."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/CATEG",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Messages category template or regular expression to processing by archivator, separated by symbol ';'.\n"
			   "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol.\n"
			   "Regular expression enclosed in symbols '/' (/mod_(System|LogicLev)/)."));
		ctrRemoveNode(opt,"/prm/cfg/MODUL");
	    }
	}
	if(runSt && ctrMkNode("area",opt,-1,"/mess",_("Messages"),R_R___,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time, size (s) and level"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size","",RWRW__,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/lvl","",RWRW__,"root",SARH_ID,5,"tp","dec", "dest","select",
		"sel_id","0;1;2;3;4;5;6;7",
		"sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)"),
		"help",_("Get messages for level more and equal it."));
	    ctrMkNode("fld",opt,-1,"/mess/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str", "help",
		_("Messages category template or regular expression.\n"
		  "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol.\n"
		  "Regular expression enclosed in symbols '/' (/mod_(System|LogicLev)/)."));
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SARH_ID))
	    {
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
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( startStat() ? "1" : "0" );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	s2i(opt->text()) ? start() : stop();
    }
    else if(a_path == "/prm/st/db")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( DB() );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB( opt->text() );
    }
    else if(a_path == "/prm/st/end" && ctrChkNode(opt))		opt->setText(i2s(end()));
    else if(a_path == "/prm/st/beg" && ctrChkNode(opt))		opt->setText(i2s(begin()));
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SARH_ID,RWRWR_);
    else if(a_path == "/mess/tm")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))
	{
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text()))	opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(nodePath()+"messTm",(s2i(opt->text())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/cat")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/lvl")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/mess" && runSt && ctrChkNode(opt,"get",R_R___,"root",SARH_ID)) {
	vector<TMess::SRec> rec;
	time_t end = s2i(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	if(!end) end = time(NULL);
	time_t beg = end - s2i(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")));
	string cat = TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user"));
	char   lev = s2i(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));

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
    }
    else TCntrNode::cntrCmdProc(opt);
}


//OpenSCADA system file: tarchives.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
    TSubSYS(SARH_ID,"Archives",true), elMess(""), elVal(""), elAval(""), bufErr(0), mMessPer(10), prcStMess(false),
    headBuf(0), mValPer(1000), mValPrior(10), prcStVal(false), endrunReqVal(false), toUpdate(false)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mRes, &attrM);
    pthread_mutex_init(&vRes, &attrM);
    pthread_mutexattr_destroy(&attrM);

    mAval = grpAdd("va_");

    //Message archivator DB structure
    elMess.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elMess.fldAdd(new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elMess.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    elMess.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200"));
    elMess.fldAdd(new TFld("START",_("Start archive"),TFld::Boolean,0,"1"));
    elMess.fldAdd(new TFld("CATEG",_("Messages categories"),TFld::String,0,"100"));
    elMess.fldAdd(new TFld("LEVEL",_("Messages level"),TFld::Integer,0,"1","0","0;7"));
    elMess.fldAdd(new TFld("ADDR",_("Address"),TFld::String,0,"100"));

    //Value archivator DB structure
    elVal.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elVal.fldAdd(new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    elVal.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    elVal.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200"));
    elVal.fldAdd(new TFld("START",_("Start archive"),TFld::Boolean,0,"1","0"));
    elVal.fldAdd(new TFld("ADDR",_("Address"),TFld::String,0,"100"));
    elVal.fldAdd(new TFld("V_PER",_("Value period (sec)"),TFld::Real,0,"12.6","1","0;1000000"));
    elVal.fldAdd(new TFld("A_PER",_("Period archiving (sec)"),TFld::Integer,0,"4","60","0;1000"));

    //Value archive DB structure
    elAval.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20"));
    elAval.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    elAval.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200"));
    elAval.fldAdd(new TFld("START",_("Start archive"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("SrcMode",_("Source mode"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d",TVArchive::Passive,TVArchive::PassiveAttr,TVArchive::ActiveAttr).c_str(),
	_("Passive;Passive param. attribute;Active param. attribute")));
    elAval.fldAdd(new TFld("Source",_("Source"),TFld::String,0,"100"));
    elAval.fldAdd(new TFld("CombMode",_("Data combining mode"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d;%d",TVArchive::MovAver,TVArchive::LastVal,TVArchive::MinVal,TVArchive::MaxVal).c_str(),
	_("Moving average;Single;Minimum;Maximum")));
    elAval.fldAdd(new TFld("VTYPE",_("Value type"),TFld::Integer,TFld::Selected,"1","0",
	TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",TFld::Boolean,TFld::Integer,TFld::Real,TFld::String,TFld::Int16,TFld::Int32,TFld::Int64,TFld::Float,TFld::Double).c_str(),
	_("Boolean;Integer;Real;String;Int16;Int32;Int64;Real(Float);Real(Double)")));
    elAval.fldAdd(new TFld("BPER",_("Buffer period (sec)"),TFld::Real,0,"9.6","1","0;10000"));
    elAval.fldAdd(new TFld("BSIZE",_("Buffer size (items)"),TFld::Integer,0,"8","100","10;10000000"));
    elAval.fldAdd(new TFld("BHGRD",_("Buffer in hard time grid"),TFld::Boolean,0,"1","1"));
    elAval.fldAdd(new TFld("BHRES",_("Buffer in high time resolution"),TFld::Boolean,0,"1","0"));
    elAval.fldAdd(new TFld("ArchS",_("Process into archivators"),TFld::String,0,"500"));

    setMessBufLen(BUF_SIZE_DEF);
}

TArchiveS::~TArchiveS( )
{
    //Stop subsystem
    if(prcStMess || prcStVal) subStop();

    //Free other resources
    nodeDelAll();

    pthread_mutex_destroy(&mRes);
    pthread_mutex_destroy(&vRes);
}

int TArchiveS::valPeriod( )		{ return vmax(1,mValPer); }

void TArchiveS::setValPrior( int ivl )	{ mValPrior = vmax(-1,vmin(99,ivl)); modif(); }

void TArchiveS::load_( )
{
    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters
    setMessBufLen(atoi(TBDS::genDBGet(nodePath()+"MessBufSize",i2s(messBufLen())).c_str()));
    setMessPeriod(atoi(TBDS::genDBGet(nodePath()+"MessPeriod",i2s(mMessPer)).c_str()));
    setValPeriod(atoi(TBDS::genDBGet(nodePath()+"ValPeriod",i2s(mValPer)).c_str()));
    setValPrior(atoi(TBDS::genDBGet(nodePath()+"ValPriority",i2s(mValPrior)).c_str()));

    //LidDB
    // Message archivators load
    string id,type;
    map<string, bool>	itReg;
    try {
	TConfig c_el(&elMess);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	// Search int DB and create new archivators
	SYS->db().at().dbList(db_ls, true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_mess_proc",nodePath()+subId()+"_mess_proc",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().messPresent(id))
		    at(type).at().messAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    vector<string> m_ls;
	    modList(m_ls);
	    for(unsigned i_m = 0; i_m < m_ls.size(); i_m++) {
		at(m_ls[i_m]).at().messList(db_ls);
		for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		    if(itReg.find(m_ls[i_m]+"."+db_ls[i_it]) == itReg.end() && SYS->chkSelDB(at(m_ls[i_m]).at().messAt(db_ls[i_it]).at().DB()))
			at(m_ls[i_m]).at().messDel(db_ls[i_it]);
	    }
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Message archivators load error."));
    }

    // Value archivators load
    try {
	TConfig c_el(&elVal);
	c_el.cfgViewAll(false);
	vector<string> db_ls;
	itReg.clear();

	//  Search into DB and create new archivators
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_val_proc",nodePath()+subId()+"_val_proc",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().valPresent(id))
		    at(type).at().valAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[type+"."+id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    vector<string> m_ls;
	    modList(m_ls);
	    for(unsigned i_m = 0; i_m < m_ls.size(); i_m++) {
		at(m_ls[i_m]).at().valList(db_ls);
		for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		    if(itReg.find(m_ls[i_m]+"."+db_ls[i_it]) == itReg.end() && SYS->chkSelDB(at(m_ls[i_m]).at().valAt(db_ls[i_it]).at().DB()))
			at(m_ls[i_m]).at().valDel(db_ls[i_it]);
	    }
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Value archivators load error."));
    }

    // Value archives load
    try {
	TConfig c_el(&elAval);
	c_el.cfgViewAll(false);
	vector<string> db_ls;
	itReg.clear();

	//  Search into DB and create new archives
	SYS->db().at().dbList(db_ls, true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_val",nodePath()+subId()+"_val",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		if(!valPresent(id)) valAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		//   For force loading after creation from archivator storage
		else if(valAt(id).at().DB() == "*.*" && db_ls[i_db] != SYS->workDB()) valAt(id).at().setDB(db_ls[i_db]);
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    valList(db_ls);
	    for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(valAt(db_ls[i_it]).at().DB()))
		    valDel(db_ls[i_it]);
	}
    }
    catch(TError err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Value archives load error."));
    }
}

void TArchiveS::save_( )
{
    vector<string> t_lst, o_lst;

    //Save parameters
    TBDS::genDBSet(nodePath()+"MessBufSize", i2s(messBufLen()));
    TBDS::genDBSet(nodePath()+"MessPeriod", i2s(messPeriod()));
    TBDS::genDBSet(nodePath()+"ValPeriod", i2s(valPeriod()));
    TBDS::genDBSet(nodePath()+"ValPriority", i2s(valPrior()));
}

void TArchiveS::valAdd( const string &iid, const string &idb )
{
    if(valPresent(iid)) return;
    chldAdd(mAval, new TVArchive(iid,idb,&aValE()));
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
    mess_info(nodePath().c_str(),_("Start/update subsystem."));

    SubStarting = true;

    vector<string> t_lst, o_lst;

    modList(t_lst);
    for(unsigned i_t = 0; i_t < t_lst.size(); i_t++) {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);

	//Message archivators start
	mod.at().messList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(o_lst[i_o]);
	    if(mess.at().toStart())
		try{ mess.at().start(); }
		catch(TError err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Message archivator '%s' start error."),o_lst[i_o].c_str());
		}
	}
	//Value archivators start
	mod.at().valList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	    AutoHD<TVArchivator> val = mod.at().valAt(o_lst[i_o]);
	    if(val.at().toStart())
		try{ val.at().start(); }
		catch(TError err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_err(nodePath().c_str(), _("Value archivator '%s' start error."), val.at().workId().c_str());
		}
	}
    }

    //Value archives start
    valList(o_lst);
    for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	AutoHD<TVArchive> aval = valAt(o_lst[i_o]);
	if(aval.at().toStart())
	    try{ aval.at().start(); }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Value archive '%s' start error."),o_lst[i_o].c_str());
	    }
    }

    //Start messages values acquisition task
    if(!prcStMess) SYS->taskCreate(nodePath('.',true)+".mess", 0, TArchiveS::ArhMessTask, this);
    if(!prcStVal)  SYS->taskCreate(nodePath('.',true)+".vals", valPrior(), TArchiveS::ArhValTask, this);

    TSubSYS::subStart( );

    SubStarting = toUpdate = false;
}

void TArchiveS::subStop( )
{
    mess_info(nodePath().c_str(),_("Stop subsystem."));

    TSubSYS::subStop( );

    vector<string> t_lst, o_lst;

    //Messages and Values acquisition task stop
    if(prcStMess) SYS->taskDestroy(nodePath('.',true)+".mess");
    if(prcStVal)  SYS->taskDestroy(nodePath('.',true)+".vals", &endrunReqVal);

    //Archivators stop
    modList(t_lst);
    for(unsigned i_t = 0; i_t < t_lst.size(); i_t++) {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);
	// Value archives stop
	mod.at().valList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	    AutoHD<TVArchivator> val = mod.at().valAt(o_lst[i_o]);
	    if(val.at().startStat())
		try { val.at().stop(); }
		catch(TError err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Value archivator '%s' stop error."),o_lst[i_o].c_str());
		}
	}
	// Message archivators stop
	mod.at().messList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	    AutoHD<TMArchivator> mess = mod.at().messAt(o_lst[i_o]);
	    if(mess.at().startStat())
		try { mess.at().stop(); }
		catch(TError err) {
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Message archivator '%s' stop error."),o_lst[i_o].c_str());
		}
	}
    }

    //Value archives stop
    valList(o_lst);
    for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	AutoHD<TVArchive> aval = valAt(o_lst[i_o]);
	if(aval.at().startStat())
	    try { aval.at().stop(); }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Value archive '%s' stop error."),o_lst[i_o].c_str());
	    }
    }
}

void TArchiveS::perSYSCall( unsigned int cnt )
{
    if(subStartStat() && toUpdate && !SubStarting) subStart();

    TSubSYS::perSYSCall(cnt);
}

void TArchiveS::messPut( time_t tm, int utm, const string &categ, int8_t level, const string &mess )
{
    //Put message to buffer
    MtxAlloc res(mRes, true);
    mBuf[headBuf].time  = tm;
    mBuf[headBuf].utime = utm;
    mBuf[headBuf].categ = categ;
    mBuf[headBuf].level = (TMess::Type)level;
    mBuf[headBuf].mess  = mess;
    if((++headBuf) >= mBuf.size()) headBuf = 0;

    //Check for the archivator's headers to messages buffer
    for(unsigned i_m = 0; i_m < actMess.size(); i_m++) {
	int &messHead = actMess[i_m].at().messHead;
	if(messHead >= 0 && messHead == (int)headBuf && ++messHead >= (int)mBuf.size()) messHead = 0;
    }

    //Alarms processing. For level less 0 alarm is set
    map<string,TMess::SRec>::iterator p;
    if(level < 0) mAlarms[categ] = TMess::SRec(tm, utm, categ, (TMess::Type)abs(level), mess);
    else if((p=mAlarms.find(categ)) != mAlarms.end()) mAlarms.erase(p);
}

void TArchiveS::messPut( const vector<TMess::SRec> &recs )
{
    for(unsigned i_r = 0; i_r < recs.size(); i_r++)
	messPut(recs[i_r].time, recs[i_r].utime, recs[i_r].categ, recs[i_r].level, recs[i_r].mess);
}

void TArchiveS::messGet( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs,
    const string &category, int8_t level, const string &arch, time_t upTo )
{
    recs.clear();

    MtxAlloc res(mRes, true);
    if(!upTo) upTo = time(NULL)+STD_INTERF_TM;
    TRegExp re(category, "p");

    //Get records from buffer
    unsigned i_buf = headBuf;
    while(level >= 0 && (!arch.size() || arch == BUF_ARCH_NM) && time(NULL) < upTo)
    {
	if(mBuf[i_buf].time >= b_tm && mBuf[i_buf].time != 0 && mBuf[i_buf].time <= e_tm &&
		abs(mBuf[i_buf].level) >= level && re.test(mBuf[i_buf].categ))
	    recs.push_back(mBuf[i_buf]);
	if(++i_buf >= mBuf.size()) i_buf = 0;
	if(i_buf == headBuf) break;
    }

    //Get records from archives
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for(unsigned i_t = 0; level >= 0 && i_t < t_lst.size(); i_t++)
    {
	at(t_lst[i_t]).at().messList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size() && time(NULL) < upTo; i_o++)
	{
	    AutoHD<TMArchivator> archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
	    if(archtor.at().startStat() && (!arch.size() || arch == archtor.at().workId()))
		archtor.at().get(b_tm, e_tm, recs, category, level);
	}
    }

    //Alarms request processing
    if(level < 0) {
	vector< pair<int64_t,TMess::SRec* > > mb;
	for(map<string,TMess::SRec>::iterator p = mAlarms.begin(); p != mAlarms.end() && time(NULL) < upTo; p++)
	    if((p->second.time >= b_tm || b_tm == e_tm) && p->second.time <= e_tm &&
		    p->second.level >= abs(level) && re.test(p->second.categ))
		mb.push_back(pair<int64_t,TMess::SRec* >(FTM(p->second),&p->second));
	sort(mb.begin(),mb.end());
	for(unsigned i_b = 0; i_b < mb.size(); i_b++) {
	    recs.push_back(*mb[i_b].second);
	    if(recs.back().level > 0) recs.back().level *= -1;
	}
    }
}

time_t TArchiveS::messBeg( const string &arch )
{
    time_t rez = 0;
    MtxAlloc res(mRes, true);
    if(arch.empty() || arch == BUF_ARCH_NM) {
	unsigned i_buf = headBuf;
	while(!arch.size() || arch == BUF_ARCH_NM) {
	    rez = rez ? vmin(rez,mBuf[i_buf].time) : mBuf[i_buf].time;
	    if(++i_buf >= mBuf.size()) i_buf = 0;
	    if(i_buf == headBuf) break;
	}
	if(!arch.empty()) return rez;
    }

    //Get records from archives
    vector<string> t_lst, o_lst;
    modList(t_lst);
    AutoHD<TMArchivator> archtor;
    for(unsigned i_t = 0; i_t < t_lst.size(); i_t++) {
	at(t_lst[i_t]).at().messList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	    archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
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
    if(arch.empty() || arch == BUF_ARCH_NM) {
	unsigned i_buf = headBuf;
	while(!arch.size() || arch == BUF_ARCH_NM) {
	    rez = rez ? vmax(rez,mBuf[i_buf].time) : mBuf[i_buf].time;
	    if(++i_buf >= mBuf.size()) i_buf = 0;
	    if(i_buf == headBuf) break;
	}
	if(!arch.empty()) return rez;
    }

    //Get records from archives
    vector<string> t_lst, o_lst;
    modList(t_lst);
    AutoHD<TMArchivator> archtor;
    for(unsigned i_t = 0; i_t < t_lst.size(); i_t++) {
	at(t_lst[i_t]).at().messList(o_lst);
	for(unsigned i_o = 0; i_o < o_lst.size(); i_o++) {
	    archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
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
    while(mBuf.size() > len) {
	mBuf.erase(mBuf.begin() + headBuf);
	if(headBuf >= mBuf.size())	headBuf = 0;
	for(unsigned i_m = 0; i_m < actMess.size(); i_m++) {
	    int &messHead = actMess[i_m].at().messHead;
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
    while(true) {
	if(TSYS::taskEndRun()) isLast = true;
	//Message buffer read
	MtxAlloc res(arh.mRes, true);
	for(unsigned i_m = 0; i_m < arh.actMess.size(); i_m++) {
	    AutoHD<TMArchivator> mArh = arh.actMess[i_m];
	    int &messHead = mArh.at().messHead;
	    if(messHead < 0 && ((messHead=arh.headBuf+1) >= (int)arh.mBuf.size() || !arh.mBuf[messHead].time)) messHead = 0;
	    if(messHead == (int)arh.headBuf)	continue;
	    try {
		// Get new messages
		unsigned newHeadLstread = arh.headBuf;
		unsigned iM = messHead;
		vector<TMess::SRec> oMess;
		while(iM != newHeadLstread) {
		    oMess.push_back(arh.mBuf[iM]);
		    if(++iM >= arh.mBuf.size()) iM = 0;
		}
		res.unlock();

		bool rez = mArh.at().put(oMess);

		res.lock();
		if(rez) messHead = newHeadLstread;
	    }
	    catch(TError err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(arh.nodePath().c_str(),_("Message buffer process error."));
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
    TArchiveS &arh = *(TArchiveS*)param;
    arh.endrunReqVal = false;
    arh.prcStVal = true;

    while(!arh.endrunReqVal) {
	int64_t work_tm = SYS->curTime();

	pthread_mutex_lock(&arh.vRes);
	for(unsigned i_arh = 0; i_arh < arh.actVal.size(); i_arh++)
	    try {
		if(work_tm/arh.actVal[i_arh].at().period() > arh.actVal[i_arh].at().end()/arh.actVal[i_arh].at().period())
		    arh.actVal[i_arh].at().getActiveData();
	    }
	    catch(TError err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	pthread_mutex_unlock(&arh.vRes);

	TSYS::taskSleep((int64_t)arh.valPeriod()*1000000);
    }

    arh.prcStVal = false;

    return NULL;
}

TVariant TArchiveS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // Array messGet(int btm, int etm, string cat = "", int lev = 0, string arch = ""); - request of the system messages for the time from <btm>
    //       to <etm> for the category <cat>, level <lev> and archiver <arch>
    //  btm - begin time
    //  etm - end time
    //  cat - messages' category
    //  lev - messages level
    //  arch - messages archivator
    if(iid == "messGet" && prms.size() >= 2)
    {
	vector<TMess::SRec> recs;
	messGet(prms[0].getI(), prms[1].getI(), recs, ((prms.size()>=3) ? prms[2].getS() : string("")),
	    ((prms.size()>=4) ? prms[3].getI() : 0), ((prms.size()>=5) ? prms[4].getS() : string("")));
	TArrayObj *rez = new TArrayObj();
	for(unsigned i_m = 0; i_m < recs.size(); i_m++) {
	    TVarObj *am = new TVarObj();
	    am->propSet("tm", (int)recs[i_m].time);
	    am->propSet("utm", recs[i_m].utime);
	    am->propSet("categ", recs[i_m].categ);
	    am->propSet("level", recs[i_m].level);
	    am->propSet("mess", recs[i_m].mess);
	    rez->propSet(i2s(i_m), am);
	}
	return rez;
    }
    // bool messPut(int tm, int utm, string cat, int lev, string mess) - write message <mess> with category <cat>,
    //       level <lev> and time <tm>.<utm> to archive or/and allarms list.
    //  tm.utm - seconds and microseconds message time
    //  cat - message' category
    //  lev - message level
    //  mess - message text
    if(iid == "messPut" && prms.size() >= 5)
    {
	messPut(prms[0].getI(), prms[1].getI(), prms[2].getS(), prms[3].getI(), prms[4].getS());
	return true;
    }

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TArchiveS::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
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
	    int    lev     = atoi(opt->attr("lev").c_str());
	    vector<TMess::SRec> rez;
	    messGet(tm_grnd, tm, rez, cat, (TMess::Type)lev, arch);
	    for(unsigned i_r = 0; i_r < rez.size(); i_r++)
		opt->childAdd("el")->
		    setAttr("time", u2s(rez[i_r].time))->
		    setAttr("utime", u2s(rez[i_r].utime))->
		    setAttr("cat", rez[i_r].categ)->
		    setAttr("lev", i2s(rez[i_r].level))->
		    setText(rez[i_r].mess);
	}
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	//Value's data set
	    for(unsigned i_r = 0; i_r < opt->childSize(); i_r++) {
		XMLNode *mNd = opt->childGet(i_r);
		messPut(strtol(mNd->attr("time").c_str(),NULL,10),
			atoi(mNd->attr("utime").c_str()),
			mNd->attr("cat"), atoi(mNd->attr("lev").c_str()), mNd->text());
	    }
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/va_",_("Value archive"),RWRWR_,"root",SARH_ID,2,"idm",OBJ_NM_SZ,"idSz","20");
	if(ctrMkNode("area",opt,1,"/m_arch",_("Messages archive"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/m_arch/size",_("Messages buffer size"),RWRWR_,"root",SARH_ID,2,
		"tp","dec","min",i2s(BUF_SIZE_DEF).c_str());
	    ctrMkNode("fld",opt,-1,"/m_arch/per",_("Archiving period (s)"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    if(ctrMkNode("area",opt,-1,"/m_arch/view",_("View messages"),R_R___,"root",SARH_ID)) {
		ctrMkNode("fld",opt,-1,"/m_arch/view/tm",_("Time"),RWRW__,"root",SARH_ID,1,"tp","time");
		ctrMkNode("fld",opt,-1,"/m_arch/view/size",_("Size (s)"),RWRW__,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/m_arch/view/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str","help",
		    _("Messages category template or regular expression.\n"
		      "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol.\n"
		      "Regular expression enclosed in symbols '/' (/mod_(System|LogicLev)/)."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/lvl",_("Level"),RWRW__,"root",SARH_ID,5,"tp","dec","dest","select",
		    "sel_id","0;1;2;3;4;5;6;7;-1;-2;-3;-4;-5;-6;-7",
		    "sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7);"
			         "Information (1), ALARMS;Notice (2), ALARMS;Warning (3), ALARMS;Error (4), ALARMS;Critical (5), ALARMS;Alert (6), ALARMS;Emergency (7), ALARMS"),
		    "help",_("Get messages for level more and equal it."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/archtor",_("Archivator"),RWRW__,"root",SARH_ID,4,"tp","str","dest","select","select","/m_arch/lstAMess",
		    "help",_("Messages archivator.\nNo set archivator for process by buffer and all archivators.\nSet '<buffer>' for process by buffer."));
		if(ctrMkNode("table",opt,-1,"/m_arch/view/mess",_("Messages"),R_R___,"root",SARH_ID)) {
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/1",_("Category"),R_R___,"root",SARH_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/2",_("Lev."),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/3",_("Message"),R_R___,"root",SARH_ID,1,"tp","str");
		}
	    }
	}
	if(ctrMkNode("area",opt,2,"/v_arch",_("Value archives"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/v_arch/per",_("Get data period (ms)"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/prior",_("Get data task priority level"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/nmb",_("Number"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/v_arch/archs",_("Value archives"),RWRWR_,"root",SARH_ID,5,"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","va_","idSz","20");
	}
	return;
    }

    //Process command to page
    if(a_path == "/m_arch/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(messPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessPeriod(atoi(opt->text().c_str()));
    }
    else if(a_path == "/m_arch/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(messBufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessBufLen(atoi(opt->text().c_str()));
    }
    else if(a_path == "/m_arch/view/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD)) {
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!atoi(opt->text().c_str()))	opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(nodePath()+"messTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/cat") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/lvl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/archtor") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messArch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/lstAMess" && ctrChkNode(opt,"get",R_R___)) {
	opt->childAdd("el")->setText("");
	opt->childAdd("el")->setText(BUF_ARCH_NM);
	vector<string> lsm, lsa;
	modList(lsm);
	for(unsigned i_m = 0; i_m < lsm.size(); i_m++) {
	    at(lsm[i_m]).at().messList(lsa);
	    for(unsigned i_a = 0; i_a < lsa.size(); i_a++)
		opt->childAdd("el")->setText(lsm[i_m]+"."+lsa[i_a]);
	}
    }
    else if(a_path == "/m_arch/view/mess" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID)) {
	vector<TMess::SRec> rec;
	time_t gtm = atoi(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")).c_str());
	if(!gtm) gtm = time(NULL);
	int gsz = atoi(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")).c_str());
	messGet(gtm-gsz, gtm, rec,
		TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")),
		atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str()),
		TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));

	XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0","",R_R___,"root",SARH_ID);
	XMLNode *n_tmu	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a","",R_R___,"root",SARH_ID);
	XMLNode *n_cat	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/1","",R_R___,"root",SARH_ID);
	XMLNode *n_lvl	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/2","",R_R___,"root",SARH_ID);
	XMLNode *n_mess	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/3","",R_R___,"root",SARH_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--) {
	    if(n_tm)	n_tm->childAdd("el")->setText(i2s(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(i2s(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(i2s(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else if(a_path == "/v_arch/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(valPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPeriod(atoi(opt->text().c_str()));
    }
    else if(a_path == "/v_arch/prior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(valPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPrior(atoi(opt->text().c_str()));
    }
    else if(a_path == "/v_arch/nmb" && ctrChkNode(opt)) {
	vector<string> list;
	valList(list);
	unsigned e_c = 0;
	for(unsigned i_a = 0; i_a < list.size(); i_a++)
	    if(valAt(list[i_a]).at().startStat()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),list.size(),e_c));
    }
    else if(a_path == "/br/va_" || a_path == "/v_arch/archs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> list;
	    valList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    valAdd(vid); valAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	chldDel(mAval,opt->attr("id"),-1,1);
    }
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

TArchiveS &TTipArchivator::owner( )	{ return (TArchiveS &)TModule::owner(); }

void TTipArchivator::messAdd(const string &name, const string &idb )	{ chldAdd(mMess, AMess(name,idb)); }

void TTipArchivator::valAdd( const string &iid, const string &idb )	{ chldAdd(mVal, AVal(iid,idb)); }

void TTipArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
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
    if(a_path == "/br/mess_" || a_path == "/arch/mess") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> list;
	    messList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(messAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR)) {
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    messAdd(vid); messAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	messDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/val_" || a_path == "/arch/val") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> list;
	    valList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR)) {
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
    TConfig(cf_el), run_st(false), messHead(-1), mId(cfg("ID")), mLevel(cfg("LEVEL")), m_start(cfg("START").getBd()), m_db(idb)
{
    mId = iid;
}

TCntrNode &TMArchivator::operator=( TCntrNode &node )
{
    TMArchivator *src_n = dynamic_cast<TMArchivator*>(&node);
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;");
    cfg("MODUL").setS(owner().modId());
    m_db = src_n->m_db;

    if(src_n->startStat() && toStart() && !startStat()) start();

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

TTipArchivator &TMArchivator::owner( )	{ return *(TTipArchivator*)nodePrev(); }

string TMArchivator::workId( )		{ return string(owner().modId())+"."+id(); }

string TMArchivator::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TMArchivator::tbl( )	{ return string(owner().owner().subId())+"_mess_proc"; }

void TMArchivator::load_( )
{
    if(!SYS->chkSelDB(DB()))	throw TError();
    SYS->db().at().dataGet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
}

void TMArchivator::save_( )	{ SYS->db().at().dataSet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this); }

void TMArchivator::start( )
{
    messHead = -1;
    run_st = true;
    owner().owner().setActMess(this, true);
}

void TMArchivator::stop( )
{
    owner().owner().setActMess(this, false);
    run_st = false;
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
    // bool status() - get archivator status.
    if(iid == "status")	return startStat();
    // int end() - get archivator data end time.
    if(iid == "end")	return (int64_t)end();
    // int begin() - get archivator data begin time.
    if(iid == "begin")	return (int64_t)begin();

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TMArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Message archivator: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archivator"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archivator DB"),RWRWR_,"root","root",4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/prm/st/end",_("End"),R_R_R_,"root","root",1,"tp","time");
		ctrMkNode("fld",opt,-1,"/prm/st/beg",_("Begin"),R_R_R_,"root","root",1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),RWRWR_,"root",SARH_ID,2,"tp","str","len",OBJ_NM_SZ);
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),RWRWR_,"root",SARH_ID,3,"tp","str","cols","90","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",cfg("ADDR").fld().descr(),RWRWR_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/lvl",cfg("LEVEL").fld().descr(),RWRWR_,"root",SARH_ID,5,"tp","dec","dest","select",
		    "sel_id","0;1;2;3;4;5;6;7",
		    "sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)"),
		    "help",_("Get messages for level more and equal it."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/cats",cfg("CATEG").fld().descr(),RWRWR_,"root",SARH_ID,2,"tp","str",
		    "help",_("Messages category template or regular expression to processing by archivator, separated by symbol ';'.\n"
			   "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol.\n"
			   "Regular expression enclosed in symbols '/' (/mod_(System|LogicLev)/)."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",_("To start"),RWRWR_,"root",SARH_ID,1,"tp","bool");
	    }
	}
	if(run_st && ctrMkNode("area",opt,-1,"/mess",_("Messages"),R_R___,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size",_("Size (s)"),RWRW__,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str","help",
		_("Messages category template or regular expression.\n"
		  "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol.\n"
		  "Regular expression enclosed in symbols '/' (/mod_(System|LogicLev)/)."));
	    ctrMkNode("fld",opt,-1,"/mess/lvl",_("Level"),RWRW__,"root",SARH_ID,5,"tp","dec","dest","select",
		"sel_id","0;1;2;3;4;5;6;7",
		"sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)"),
		"help",_("Get messages for level more and equal it."));
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
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(startStat() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	atoi(opt->text().c_str()) ? start() : stop();
    }
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/st/end" && ctrChkNode(opt))		opt->setText(i2s(end()));
    else if(a_path == "/prm/st/beg" && ctrChkNode(opt))		opt->setText(i2s(begin()));
    else if(a_path == "/prm/cfg/id" && ctrChkNode(opt))		opt->setText(id());
    else if(a_path == "/prm/cfg/nm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(name());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setName(opt->text());
    }
    else if(a_path == "/prm/cfg/dscr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(dscr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDscr(opt->text());
    }
    else if(a_path == "/prm/cfg/ADDR") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(addr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setAddr(opt->text());
    }
    else if(a_path == "/prm/cfg/lvl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(level()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setLevel(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/start") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(toStart() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setToStart(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/cats") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(cfg("CATEG").getS());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	cfg("CATEG").setS(opt->text());
    }
    else if(a_path == "/mess/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD)) {
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!atoi(opt->text().c_str()))	opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(nodePath()+"messTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/cat") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/lvl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/mess" && run_st && ctrChkNode(opt,"get",R_R___,"root",SARH_ID)) {
	vector<TMess::SRec> rec;
	time_t end = atoi(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")).c_str());
	if(!end) end = time(NULL);
	time_t beg = end - atoi(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")).c_str());
	string cat = TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user"));
	char   lev = atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str());

	get(beg, end, rec, cat, lev);

	XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SARH_ID);
	XMLNode *n_tmu	= ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SARH_ID);
	XMLNode *n_cat	= ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SARH_ID);
	XMLNode *n_lvl	= ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SARH_ID);
	XMLNode *n_mess	= ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SARH_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--) {
	    if(n_tm)	n_tm->childAdd("el")->setText(i2s(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(i2s(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(i2s(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}


//OpenSCADA system module Archive.DBArch file: mess.cpp
/***************************************************************************
 *   Copyright (C) 2007-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <tsys.h>
#include "arch.h"
#include "mess.h"

using namespace DBArch;

//************************************************
//* DBArch::ModMArch - Messages archivator       *
//************************************************
ModMArch::ModMArch( const string &iid, const string &idb, TElem *cf_el ) :
    TMArchivator(iid, idb, cf_el), tmProc(0), mBeg(0), mEnd(0), mMaxSize(0), mTmAsStr(false)//, mTmDtSepScDayThr(0)
{
    setAddr("*.*");
}

ModMArch::~ModMArch( )
{
    try { stop(); } catch(...) { }
}

void ModMArch::postDisable( int flag )
{
    TMArchivator::postDisable(flag);

    if(flag) {
	//Remove info record
	TConfig cfg(&mod->archEl());
	cfg.cfg("TBL").setS(archTbl(),true);
	SYS->db().at().dataDel(addr()+"."+mod->mainTbl(),"",cfg);

	//Remove archive's DB table
	SYS->db().at().open(addr()+"."+archTbl());
	SYS->db().at().close(addr()+"."+archTbl(), true);
    }
}

void ModMArch::load_( )
{
    TMArchivator::load_();

    //Init address to DB
    if(addr().empty()) setAddr("*.*");

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	if(!(vl=prmNd.attr("Size")).empty())	setMaxSize(s2r(vl));
	if(!(vl=prmNd.attr("TmAsStr")).empty())	setTmAsStr(s2i(vl));
	//if(!(vl=prmNd.attr("TmDtSepScDayThr")).empty())	setTmDtSepScDayThr(s2i(vl));
    } catch(...) { }

    //Load message archive parameters
    TConfig wcfg(&mod->archEl());
    wcfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(addr()+"."+mod->mainTbl(),"",wcfg,false,true)) {
	mBeg = s2i(wcfg.cfg("BEGIN").getS());
	mEnd = s2i(wcfg.cfg("END").getS());
	// Check for delete archivator table
	if(maxSize() && mEnd <= (time(NULL)-(time_t)(maxSize()*3600))) {
	    SYS->db().at().open(addr()+"."+archTbl());
	    SYS->db().at().close(addr()+"."+archTbl(), true);
	    mBeg = mEnd = 0;
	}
    }
}

void ModMArch::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("Size", r2s(maxSize()));
    prmNd.setAttr("TmAsStr", i2s(tmAsStr()));
    //prmNd.setAttr("TmDtSepScDayThr", i2s(tmDtSepScDayThr()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TMArchivator::save_();
}

void ModMArch::start( )
{
    if(!runSt) {
	reqEl.fldClear();
	//if(tmDtSepScDayThr())
	//    reqEl.fldAdd(new TFld("DT",_("Date (days)"),TFld::Integer,TCfg::Key,"10"));
	reqEl.fldAdd(new TFld("MIN",_("In minutes"),TFld::Integer,TCfg::Key,"15"));	//Mostly for fast reading next, by minutes
	reqEl.fldAdd(new TFld("TM",_("Time (s)"),TFld::Integer,TCfg::Key|(tmAsStr()?TFld::DateTimeDec:0),"20"));
	reqEl.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"6","0"));
	reqEl.fldAdd(new TFld("CATEG",_("Category"),TFld::String,TCfg::Key,"100"));
	reqEl.fldAdd(new TFld("MESS",_("Message"),TFld::String,TFld::NoFlag/*TCfg::Key*/,"100000"));
	reqEl.fldAdd(new TFld("LEV",_("Level"),TFld::Integer,TFld::NoFlag,"2"));
    }

    //Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb, 0, '.');
    try { if(!db.at().enableStat()) db.at().enable(); }
    catch(TError &err) { mess_warning(nodePath().c_str(), _("Enable target DB error: %s"), err.mess.c_str()); }

    TMArchivator::start();
}

void ModMArch::stop( )
{
    TMArchivator::stop();

    reqEl.fldClear();
}

bool ModMArch::put( vector<TMess::SRec> &mess, bool force )
{
    TMArchivator::put(mess, force);    //Allow redundancy

    if(!runSt) throw TError(nodePath().c_str(), _("Archive is not started!"));

    AutoHD<TTable> tbl = SYS->db().at().open(addr()+"."+archTbl(), true);
    if(tbl.freeStat()) return false;

    TConfig cfg(&reqEl);
    int64_t t_cnt = TSYS::curTime();
    for(unsigned i_m = 0; i_m < mess.size(); i_m++) {
	if(!chkMessOK(mess[i_m].categ,mess[i_m].level)) continue;

	//Put record to DB
	//if(tmDtSepScDayThr()) cfg.cfg("DT").setI(mess[i_m].time/86400);
	cfg.cfg("MIN").setI(mess[i_m].time/60);
	cfg.cfg("TM").setI(mess[i_m].time);
	cfg.cfg("TMU").setI(mess[i_m].utime);
	cfg.cfg("CATEG").setS(mess[i_m].categ);
	cfg.cfg("MESS").setS(mess[i_m].mess);
	cfg.cfg("LEV").setI(mess[i_m].level);
	tbl.at().fieldSet(cfg);
	//Archive time border update
	mBeg = mBeg ? vmin(mBeg,mess[i_m].time) : mess[i_m].time;
	mEnd = mEnd ? vmax(mEnd,mess[i_m].time) : mess[i_m].time;
    }

    //Archive size limit process
    if(maxSize() && (mEnd-mBeg) > (time_t)(maxSize()*3600)) {
	time_t n_end = mEnd-(time_t)(maxSize()*3600);
	for(time_t t_c = vmax(mBeg,n_end-3600); t_c < n_end; t_c++) {
	    cfg.cfg("TM").setI(t_c, true);
	    tbl.at().fieldDel(cfg);
	}
	mBeg = n_end;
    }
    tbl.free();
    SYS->db().at().close(addr()+"."+archTbl());

    //Update archive info
    cfg.setElem(&mod->archEl());
    cfg.cfgViewAll(false);
    cfg.cfg("TBL").setS(archTbl(),true);
    cfg.cfg("BEGIN").setS(i2s(mBeg),true);
    cfg.cfg("END").setS(i2s(mEnd),true);
    bool rez = SYS->db().at().dataSet(addr()+"."+mod->mainTbl(),"",cfg,false,true);

    tmProc = 1e-3*(TSYS::curTime()-t_cnt);

    return rez;
}

void ModMArch::get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category, char level, time_t upTo )
{
    if(!runSt) throw TError(nodePath().c_str(), _("Archive is not started!"));
    if(!upTo) upTo = SYS->sysTm() + STD_INTERF_TM;

    bTm = vmax(bTm, begin());
    eTm = vmin(eTm, end());
    if(eTm <= bTm) return;

    TConfig cfg(&reqEl);
    TRegExp re(category, "p");

    //Get values from DB
    cfg.cfg("TM").setKeyUse(false);
    for(time_t tC = bTm; tC/60 <= eTm/60 && SYS->sysTm() < upTo; ) {
	tC = (tC/60)*60;
	cfg.cfg("MIN").setI(tC/60, true);
	int eC = 0;
	for( ; SYS->db().at().dataSeek(addr()+"."+archTbl(),"",eC++,cfg) && SYS->sysTm() < upTo; ) {
	    TMess::SRec rc(cfg.cfg("TM").getI(), cfg.cfg("TMU").getI(), cfg.cfg("CATEG").getS(),
			    (TMess::Type)cfg.cfg("LEV").getI(), cfg.cfg("MESS").getS());
	    if(rc.time >= bTm && rc.time <= eTm && abs(rc.level) >= level && re.test(rc.categ)) {
		bool equal = false;
		int i_p = mess.size();
		for(int i_m = mess.size()-1; i_m >= 0; i_m--) {
		    if(FTM(mess[i_m]) > FTM(rc)) i_p = i_m;
		    else if(FTM(mess[i_m]) == FTM(rc) && rc.level == mess[i_m].level && rc.mess == mess[i_m].mess)
		    { equal = true; break; }
		    else if(FTM(mess[i_m]) < FTM(rc)) break;
		}
		if(!equal) {
		    mess.insert(mess.begin()+i_p, rc);
		    if(time(NULL) >= upTo) return;
		}
	    }
	}
	tC += 60;
    }

    /*if(tmDtSepScDayThr()) {
	printf("TEST 00: tm=[%d:%d]\n", bTm, eTm);
	for(time_t tC = bTm, tTmp; tC <= eTm && SYS->sysTm() < upTo; ) {
	    tTmp = (tC/86400)*86400;
	    bool bySec = (eTm-bTm) < tmDtSepScDayThr() || (tC > tTmp && ((tTmp+86400)-tC) < tmDtSepScDayThr()) || (eTm-tTmp) < tmDtSepScDayThr();
	    if(!bySec) tC = tTmp;
	    cfg.cfg("DT").setI(tC/86400, true);
	    cfg.cfg("TM").setI(tC, true);
	    if(!bySec) cfg.cfg("TM").setKeyUse(false);
	    printf("TEST 01: bySec=%d; tm=%d\n", bySec, tC);
	    int e_c = 0;
	    for( ; SYS->db().at().dataSeek(addr()+"."+archTbl(),"",e_c++,cfg) && SYS->sysTm() < upTo; ) {
		TMess::SRec rc(cfg.cfg("TM").getI(), cfg.cfg("TMU").getI(), cfg.cfg("CATEG").getS(),
				(TMess::Type)cfg.cfg("LEV").getI(), cfg.cfg("MESS").getS());
		//printf("TEST 01a: tm=[%d.%d]\n", rc.time, rc.utime);
		if(rc.time >= bTm && rc.time <= eTm && abs(rc.level) >= level && re.test(rc.categ)) {
		    bool equal = false;
		    int i_p = mess.size();
		    for(int i_m = mess.size()-1; i_m >= 0; i_m--) {
			if(FTM(mess[i_m]) > FTM(rc)) i_p = i_m;
			else if(FTM(mess[i_m]) == FTM(rc) && rc.level == mess[i_m].level && rc.mess == mess[i_m].mess)
			{ equal = true; break; }
			else if(FTM(mess[i_m]) < FTM(rc)) break;
		    }
		    if(!equal) {
			mess.insert(mess.begin()+i_p, rc);
			if(time(NULL) >= upTo) return;
		    }
		}
	    }
	    printf("TEST 02: e_c=%d\n", e_c);
	    tC += bySec ? 1 : 86400;
	}
	return;
    }

    for(time_t tC = bTm; tC <= eTm && SYS->sysTm() < upTo; tC++) {
	cfg.cfg("TM").setI(tC, true);
	for(int e_c = 0; SYS->db().at().dataSeek(addr()+"."+archTbl(),"",e_c++,cfg); ) {
	    TMess::SRec rc(tC, cfg.cfg("TMU").getI(), cfg.cfg("CATEG").getS(), (TMess::Type)cfg.cfg("LEV").getI(), cfg.cfg("MESS").getS());
	    if(abs(rc.level) >= level && re.test(rc.categ)) {
		bool equal = false;
		int i_p = mess.size();
		for(int i_m = mess.size()-1; i_m >= 0; i_m--) {
		    if(FTM(mess[i_m]) > FTM(rc)) i_p = i_m;
		    else if(FTM(mess[i_m]) == FTM(rc) && rc.level == mess[i_m].level && rc.mess == mess[i_m].mess)
		    { equal = true; break; }
		    else if(FTM(mess[i_m]) < FTM(rc)) break;
		}
		if(!equal) {
		    mess.insert(mess.begin()+i_p, rc);
		    if(time(NULL) >= upTo) return;
		}
	    }
	}
    }*/
}

void ModMArch::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TMArchivator::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time (msek)"),R_R_R_,"root",SARH_ID,1,"tp","real");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","select","select","/db/list","help",TMess::labDB());
	if(ctrMkNode("area",opt,-1,"/prm/add",_("Additional options"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/prm/add/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,2,
		"tp","real", "help",_("Set to 0 for the limit disable and some performance rise."));
	    ctrMkNode("fld",opt,-1,"/prm/add/tmAsStr",_("Force time as string"),startStat()?R_R_R_:RWRWR_,"root",SARH_ID,2,
		"tp","bool", "help",_("Only for DBs it supports by a specific data type like to \"datetime\" into MySQL."));
	    //ctrMkNode("fld",opt,-1,"/prm/add/tmDtSepScDayThr",_("Scan by days threshold in seconds"),startStat()?R_R_R_:RWRWR_,"root",SARH_ID,2,
	    //	"tp","dec", "help",_("Nonzero value enables separating datetime to different date and time parts.\n"
	    //			      "Useful for faster access to big parts of the messages on it pick."));
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/tarch" && ctrChkNode(opt))	opt->setText(r2s(tmProc,6));
    else if(a_path == "/prm/add/sz") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(s2r(opt->text()));
    }
    else if(a_path == "/prm/add/tmAsStr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(tmAsStr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setTmAsStr(s2i(opt->text()));
    }
    /*else if(a_path == "/prm/add/tmDtSepScDayThr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(tmDtSepScDayThr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setTmDtSepScDayThr(s2i(opt->text()));
    }*/
    else TMArchivator::cntrCmdProc(opt);
}

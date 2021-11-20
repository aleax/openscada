
//OpenSCADA module BD.SQLite file: bd_sqlite.cpp
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

#include <errno.h>
#include <string.h>

#include <sqlite3.h>

#include <tsys.h>
#include <tmess.h>
#include "bd_sqlite.h"

//******************************************************************************
//* Modul info!                                                                *
#define MOD_ID		"SQLite"
#define MOD_NAME	_("DB SQLite")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"3.3.3"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("BD module. Provides support of the BD SQLite.")
#define LICENSE		"GPL2"
#define FEATURES	"SQL, LIST, STRUCT, GET, SEEK, PRELOAD, SET, DEL, FIX, TR, ERR"
//******************************************************************************

#define TRANS_CLOSE_TM_AFT_REQ	10
#define TRANS_CLOSE_TM_AFT_OPEN	60
#define SEEK_PRELOAD_LIM	100

BDSQLite::BDMod *BDSQLite::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_SQLite_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *bd_SQLite_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new BDSQLite::BDMod(source);
	return NULL;
    }
}

using namespace BDSQLite;

//*************************************************
//* BDSQLite::BDMod				  *
//*************************************************
BDMod::BDMod( string name ) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

BDMod::~BDMod( )	{ }

string BDMod::features( )	{ return FEATURES; }

TBD *BDMod::openBD( const string &iid )	{ return new MBD(iid,&owner().openDB_E()); }

void BDMod::modStop( )
{
    vector<string> dbs;

    //Close transactions for all DB
    list(dbs);
    for(unsigned iDB = 0; iDB < dbs.size(); iDB++)
	at(dbs[iDB]).at().transCommit();
}

//************************************************
//* BDSQLite::MBD				 *
//************************************************
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0), connRes(true), trans_reqs(1)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag&NodeRemove && owner().fullDeleteDB())
	if(remove(TSYS::strSepParse(addr(),0,';').c_str()) != 0)
	    throw err_sys(_("Error deleting DB: %s"), strerror(errno));
}

void MBD::enable( )
{
    MtxAlloc res(connRes, true);

    //Reconnecting
    if(enableStat()) {
	sqlite3_close(m_db);
	mEn = false;
	//return;
    }

    string fnm = TSYS::strSepParse(addr(), 0, ';');
    remove((fnm+"-journal").c_str());

    cd_pg = codePage().size() ? codePage() : Mess->charset();
    int rc = sqlite3_open(fnm.c_str(), &m_db);
    if(rc) {
	string err = sqlite3_errmsg(m_db);
	sqlite3_close(m_db);
	throw err_sys(_("Error opening DB file: %s"), err.c_str());
    }
    trans_reqs = vmax(1, vmin(100,s2i(TSYS::strSepParse(addr(),1,';'))));

    TBD::enable();
}

void MBD::disable( )
{
    MtxAlloc res(connRes, true);
    if(!enableStat()) return;

    //Last commit
    if(reqCnt) try{ transCommit(); } catch(...) { }

    TBD::disable();

    //Close DB
    sqlite3_close(m_db);
}

void MBD::allowList( vector<string> &list ) const
{
    if(!enableStat()) return;
    list.clear();
    vector< vector<string> > tbl;
    const_cast<MBD*>(this)->sqlReq("SELECT name FROM sqlite_master WHERE type IN ('table','view') AND name NOT LIKE 'sqlite_%';", &tbl/*, false*/);
    for(unsigned iT = 1; iT < tbl.size(); iT++)
	list.push_back(tbl[iT][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error opening the table '%s'. DB is disabled."), inm.c_str());

    try { sqlReq("SELECT * FROM '" + TSYS::strEncode(inm,TSYS::SQL,"'") + "' LIMIT 0;"); }
    catch(...) { if(!create) throw; }

    return new MTable(inm, this);
}

void MBD::sqlReq( const string &req, vector< vector<string> > *tbl, char intoTrans )
{
    char *zErrMsg = NULL;
    int rc, nrow = 0, ncol = 0;
    char **result = NULL;

    if(!enableStat())	return;
    if(tbl) tbl->clear();	//!! Clean only for enabled DB due to the possibility of wrong cleaning the table structure

    MtxAlloc res(connRes, true);//!! Moved before the transaction checking for prevent the "BEGIN;" and "COMMIT;"
				//   request's sequence breakage on high concurrency access activity

    //Commit set
    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    int repCnt = 0;
rep:
    //Put the request
    if(mess_lev() == TMess::Debug) mess_debug((owner().nodePath()+id()+"_tracing/").c_str(), _("Request: \"%s\""), req.c_str());
    rc = sqlite3_get_table(m_db, Mess->codeConvOut(cd_pg.c_str(),req).c_str(), &result, &nrow, &ncol, &zErrMsg);
    if(rc != SQLITE_OK) {
	string err = _("Unknown error");
	if(zErrMsg) { err = zErrMsg; sqlite3_free(zErrMsg); }

	if(rc == SQLITE_BUSY) {	//Treat locked DBs
	    //Try to reconnect
	    if((repCnt++) < 3)
		try { enable(); goto rep; } catch(TError&) { }
	    else mess_warning(nodePath().c_str(), _("Repeated errors of requesting the DB: %s(%d)."), err.c_str(), rc);

	    disable();
	}

	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("Error of the request \"%s\": %s(%d)"), req.c_str(), err.c_str(), rc);
	throw err_sys(TError::EXT+rc, _("Error of the request \"%s\": %s(%d)"), TSYS::strEncode(req,TSYS::Limit,"50").c_str(), err.c_str(), rc);
    }
    if(tbl && ncol > 0) {
	vector<string> row;
	// Add head
	for(int i = 0; i < ncol; i++) row.push_back(result[i]?result[i]:DB_NULL);
	tbl->push_back(row);
	// Add data
	for(int i = 0; i < nrow; i++) {
	    row.clear();
	    for(int ii = 0; ii < ncol; ii++)
		row.push_back(result[(i+1)*ncol+ii]?Mess->codeConvIn(cd_pg.c_str(),result[(i+1)*ncol+ii]):DB_NULL);
	    tbl->push_back(row);
	}
    }
    if(result) sqlite3_free_table(result);
}

void MBD::transOpen( )
{
    //Check for limit into one trinsaction
    if(reqCnt > 1000) transCommit();

    MtxAlloc res(connRes, true);
    int reqCnt_ = reqCnt;
    if(!reqCnt_) trOpenTm = TSYS::curTime();
    reqCnt++;
    reqCntTm = TSYS::curTime();

    if(!reqCnt_) sqlReq("BEGIN;");
}

void MBD::transCommit( )
{
    MtxAlloc res(connRes, true);
    int reqCnt_ = reqCnt;
    reqCnt = 0;
    reqCntTm = 0;

    if(reqCnt_)
	try { sqlReq("COMMIT;"); }
	catch(TError&) { }	//!!!! Pass spare commit errors of the sort "cannot commit - no transaction is active(1)"
}

void MBD::transCloseCheck( )
{
    if(enableStat() && reqCnt && ((TSYS::curTime()-reqCntTm) > 1e6*trTm_ClsOnReq() || (TSYS::curTime()-trOpenTm) > 1e6*trTm_ClsOnOpen()))
	transCommit();
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,3,
	    "dest","sel_ed","select","/prm/cfg/dbFsList","help",
		    _("SQLite DB address must be written as: \"{FileDBPath}\".\n"
		      "Where:\n"
		      "  FileDBPath - full path to DB file (./oscada/Main.db).\n"
		      "               Use the empty path to create a temporary database on the disk.\n"
		      "               Use \":memory:\" to create a temporary database in memory."));
	if(reqCnt)
	    ctrMkNode("comm",opt,-1,"/prm/st/end_tr",_("Close opened transaction"),RWRW__,"root",SDB_ID);
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/dbFsList" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText(":memory:");
	TSYS::ctrListFS(opt, addr(), "db;");
    }
    else if(a_path == "/prm/st/end_tr" && ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR) && reqCnt) transCommit();
    else TBD::cntrCmdProc(opt);
}

//************************************************
//* MBDMySQL::Table                              *
//************************************************
MTable::MTable( string inm, MBD *iown ) : TTable(inm)
{
    setNodePrev(iown);

    try { owner().sqlReq("PRAGMA table_info('"+TSYS::strEncode(name(),TSYS::SQL,"'")+"');", &tblStrct); }
    catch(...) { }
}

MTable::~MTable( )
{

}

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag&NodeRemove)
	try{ owner().sqlReq("DROP TABLE '"+TSYS::strEncode(name(),TSYS::SQL,"'")+"';"); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld][1];
	if(cfg.cfgPresent(sid)) continue;

	int flg = s2i(tblStrct[iFld][5]) ? (int)TCfg::Key : (int)TFld::NoFlag;
	if(tblStrct[iFld][2] == "TEXT")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(tblStrct[iFld][2] == "INTEGER")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(tblStrct[iFld][2] == "REAL" || tblStrct[iFld][2] == "DOUBLE")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg, const string &cacheKey )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    //cfg.cfgToDefault();	//reset the not key and viewed fields
    cfg.setTrcSet(true);

    //Check for not present and not empty keys allow
    if(row == 0) {
	vector<string> cf_el;
	cfg.cfgList(cf_el);
	for(unsigned iC = 0; iC < cf_el.size(); iC++) {
	    TCfg &cf = cfg.cfg(cf_el[iC]);
	    if(!cf.isKey() || !cf.getS().size()) continue;
	    unsigned iFld = 1;
	    for( ; iFld < tblStrct.size(); iFld++)
		if(cf.name() == tblStrct[iFld][1]) break;
	    if(iFld >= tblStrct.size()) return false;
	}
    }

    string sid, req_where, ls;
    //Make WHERE
    // Add use keys to list
    bool trPresent = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld][1];
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3),true);
	    if(u_cfg && !(u_cfg->fld().flg()&TFld::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey() && u_cfg->keyUse())
	    req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg) + " ";
	else if(u_cfg->isKey() || u_cfg->view())
	    ls += (ls.size()?",\"":"\"")+TSYS::strEncode(sid,TSYS::SQL,"\"")+"\"";
    }

    vector< vector<string> >	inTbl, *tbl = &inTbl;
    MtxAlloc res(owner().connRes);
    if(cacheKey.size()) { res.lock(); tbl = &seekSess[cacheKey]; }

    //Request
    if(!cacheKey.size() || !tbl->size() || (row%SEEK_PRELOAD_LIM) == 0) {
	if(ls.empty()) return false;
	string req = "SELECT " + ls + " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' " + (req_where.size()?"WHERE "+req_where:"");
	if(!cacheKey.size()) req += " LIMIT " +  i2s(row) + ",1;";
	else req += " LIMIT " + i2s((row/SEEK_PRELOAD_LIM)*SEEK_PRELOAD_LIM) + "," + i2s(SEEK_PRELOAD_LIM)+";";

	tbl->clear();
	owner().sqlReq(req, tbl/*, false*/);	// For seek to deletion into save context do not set to "false"
    }

    row = cacheKey.size() ? (row%SEEK_PRELOAD_LIM)+1 : 1;
    if(tbl->size() < 2 || (cacheKey.size() && row >= (int)tbl->size())) {
	if(cacheKey.size()) seekSess.erase(cacheKey);
	return false;
    }

    //Processing of the query
    for(unsigned iFld = 0; iFld < (*tbl)[0].size(); iFld++) {
	sid = (*tbl)[0][iFld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg, (*tbl)[row][iFld]);
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && (*tbl)[row][iFld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setVal(*u_cfg, (*tbl)[row][iFld], true);
    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    string sid, req_where, first_key, ls;
    //Prepare request
    // Adding the fields list to a queue
    bool trPresent = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld][1];
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TFld::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey()) {
	    req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg);
	    if(first_key.empty()) first_key = TSYS::strEncode(sid,TSYS::SQL,"\"");
	}
	else if(u_cfg->view())
	    ls += (ls.size()?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"";
    }

    //Query
    string req = "SELECT " + (ls.size()?ls:"\""+first_key+"\"") + " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' WHERE " + req_where + ";";
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) throw err_sys(_("Row \"%s\" is not present. Are you saved the object?"), req_where.c_str());

    //Processing of query
    for(unsigned iFld = 0; iFld < tbl[0].size(); iFld++) {
	sid = tbl[0][iFld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg, tbl[1][iFld]);
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[1][iFld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setVal(*u_cfg, tbl[1][iFld], true);
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    //Generic flags:
    // Mess->translCfg() - is the translatable configuration
    // trPresent - the table is translating or is going to be translating
    // isTransl  - the field is translating

    vector< vector<string> > tbl;

    if(tblStrct.empty()) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }
    mLstUse = SYS->sysTm();

    string sid, sval, req_where;

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Check for translation present
    bool trPresent = Mess->translCfg(), trDblDef = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	if(trPresent && (!Mess->translCfg() || trDblDef)) break;
	sid = tblStrct[iFld][1];
	if(sid.size() > 3) {
	    if(!trPresent && sid.size() > 3 && sid[2] == '#' /*!Mess->translDyn() && sid.find(Mess->lang2Code()+"#") == 0*/) trPresent = true;
	    if(Mess->lang2Code() == Mess->lang2CodeBase() && !trDblDef && sid.find(Mess->lang2CodeBase()+"#") == 0) trDblDef = true;
	}
    }
    if(trDblDef && !cfg.reqKeys()) fieldFix(cfg);

    //Get the available fields list
    // Add key list to queue
    bool noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();			//Force update by ReqKeys or reqKey() present
    for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
	TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
	if(!u_cfg.isKey()) continue;
	req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + getVal(u_cfg,TCfg::ExtValTwo);

	if(!isForceUpdt && u_cfg.extVal()) isForceUpdt = true;

	// Check for no key fields
	if(noKeyFld) continue;
	unsigned iFld = 1;
	for( ; iFld < tblStrct.size(); iFld++)
	    if(u_cfg.name() == tblStrct[iFld][1]) break;
	if(iFld >= tblStrct.size()) noKeyFld = true;
    }
    if(noKeyFld) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }

    //Query for presence detect or the current data
    string req, ls, ls2;
    if(trPresent)
	req = "SELECT * FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' WHERE " + req_where + ";";
    else if(!isForceUpdt)
	req = "SELECT 1 FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' WHERE " + req_where + ";";
    if(req.size()) owner().sqlReq(req, &tbl, true);

    req = "";
    if(!isForceUpdt) {
	if(tbl.size() < 2) {
	    //Add new record
	    for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
		TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
		if(!u_cfg.isKey() && !u_cfg.view()) continue;

		sval = getVal(u_cfg);
		bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && Mess->translCfg() && !u_cfg.noTransl() && Mess->isMessTranslable(sval));
		ls += (ls.size()?",\"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\" " +
		    (isTransl ? (",\""+TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[iEl],TSYS::SQL,"\"")+"\" ") : "");
		ls2 += (ls2.size()?",":"") + sval + " " + (isTransl?(","+sval+" "):"");
	    }
	    if(ls.size())
		req = "INSERT INTO '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' (" + ls + ") VALUES (" + ls2 + ");";
	} else isForceUpdt = true;
    }

    //Update present record
    bool toWarnReload = false;
    if(isForceUpdt) {
	for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
	    if((u_cfg.isKey() && !u_cfg.extVal()) || !u_cfg.view()) continue;

	    sval = getVal(u_cfg);

	    // ???? Propagate the last changes to DB.{MySQL,PostgreSQL,FireBird}
	    // No translation
	    if(!trPresent || u_cfg.fld().type() != TFld::String)
		ls += (ls.size()?",\"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + sval + " ";
	    else {
		// Translation
		bool isTransl = Mess->translCfg() && u_cfg.fld().flg()&TFld::TransltText;

		//  Clearing all the translation at setting no translable message
		if(isTransl && (u_cfg.noTransl() || (sval.size() > 2 && !Mess->isMessTranslable(sval)))) {
		    if(u_cfg.noTransl())
			ls += (ls.size()?",\"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + sval + " ";
		    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
			sid = tblStrct[iFld][1];
			if(sid.size() <= 3 || sid.compare(3,string::npos,cf_el[iEl]) != 0 || sid.compare(0,3,Mess->lang2CodeBase()+"#") == 0)
			    continue;
			ls += (ls.size()?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"='' ";
		    }
		}
		//  Setting the translation and the marks
		else {
		    sid = (isTransl?(Mess->lang2Code()+"#"):"") + cf_el[iEl];
		    //  Checking whether the field changed
		    bool isChanged = false, isPresent = false;
		    int fPos = -1;
		    for(unsigned iFld = 0; iFld < tbl[0].size() && !isPresent; iFld++)
			if(tbl[0][iFld] == sid) {
			    isChanged = (sval != ("'"+tbl[1][iFld]+"'"));
			    isPresent = true; fPos = iFld;
			}
		    if(isChanged || !isPresent) {
			unsigned mess_TrModifMarkLen = strlen(mess_TrModifMark);

			// The same field
			ls += (ls.size()?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + sval + " ";

			// Setting for marks and the base message
			for(unsigned iFld = 0; iFld < tbl[0].size(); iFld++)
			    //  clearing the translation at not empty base message
			    if(isTransl && tbl[0][iFld] == cf_el[iEl] && sval.size() <= 2 && tbl[1][iFld].size())
				toWarnReload = true;
			    //  the translation message for mark the base one
			    else if(isTransl && tbl[0][iFld] == cf_el[iEl] && sval.size() > 2) {
				if(tbl[1][iFld].empty())
				    ls += (ls.size()?",\"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + sval + " ";
				else if((fPos < 0 || tbl[1][fPos].size() <= mess_TrModifMarkLen || tbl[1][fPos].rfind(mess_TrModifMark) != (tbl[1][fPos].size()-mess_TrModifMarkLen)) &&
					(tbl[1][iFld].size() <= mess_TrModifMarkLen || tbl[1][iFld].rfind(mess_TrModifMark) != (tbl[1][iFld].size()-mess_TrModifMarkLen)))
				    ls += (ls.size()?",\"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"='" +
					TSYS::strEncode(tbl[1][iFld],TSYS::SQL,"'") + " " + mess_TrModifMark + "' ";
			    }
			    //  the base message for mark the translation ones
			    else if(!isTransl && tbl[0][iFld].size() > 3 && tbl[0][iFld].compare(3,string::npos,sid) == 0 && tbl[0][iFld].compare(0,3,Mess->lang2CodeBase()+"#") != 0 &&
				    (fPos < 0 || tbl[1][fPos].size() <= mess_TrModifMarkLen || tbl[1][fPos].rfind(mess_TrModifMark) != (tbl[1][fPos].size()-mess_TrModifMarkLen)) &&
				    tbl[1][iFld].size() &&
				    (tbl[1][iFld].size() <= mess_TrModifMarkLen || tbl[1][iFld].rfind(mess_TrModifMark) != (tbl[1][iFld].size()-mess_TrModifMarkLen)))
				ls += (ls.size()?",\"":"\"") + TSYS::strEncode(tbl[0][iFld],TSYS::SQL,"\"") + "\"='" +
					    ((sval.size()>2)?TSYS::strEncode(tbl[1][iFld],TSYS::SQL,"'")+" "+mess_TrModifMark:"") + "' ";
		    }
		}
	    }

	    /*bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[iEl]) : cf_el[iEl];
	    req += (next?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(u_cfg) + " ";
	    next = true;*/
	}
	if(ls.size())
	    req = "UPDATE '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' SET " + ls + "WHERE " + req_where + ";";
    }

    if(req.empty())	return;

    //Query
    try { owner().sqlReq(req, NULL, true); }
    catch(TError &err) {
	if((err.cod-TError::EXT) == SQLITE_READONLY) {
	    err.mess = err.mess + " " + _("The DB is into the Read only mode!");
	    throw;
	}
	fieldFix(cfg);
	owner().sqlReq(req, NULL, true);
    }

    if(toWarnReload)
	throw err_sys(TError::DB_TrRemoved, _("The translation removed! Reload the base values."));
}

void MTable::fieldDel( TConfig &cfg )
{
    if(tblStrct.empty()) return;
    mLstUse = SYS->sysTm();

    //Where prepare
    string req_where;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld][1];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse())
	    req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg) + " ";
    }

    //Main request
    try { owner().sqlReq("DELETE FROM '"+TSYS::strEncode(name(),TSYS::SQL,"'")+"' WHERE "+req_where+";", NULL, true); }
    catch(TError &err) {
	//Checking for the presence
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM '"+TSYS::strEncode(name(),TSYS::SQL,"'")+"' WHERE "+req_where+";", &tbl, true);
	if(tbl.size() < 2) return;
    }
}

void MTable::fieldFix( TConfig &cfg )
{
    bool toUpdate = false,
	 appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !tblStrct.empty()),	//Only for append no present fields
	 isVarTextTransl = Mess->translCfg();

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Create request variables
    string all_flds, pr_keys, tpCfg, ls;

    //Curent context copy list
    if(appMode) {
	if(tblStrct.empty()) return;
	for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	    all_flds += (all_flds.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\"";
	    ls += (ls.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\" "+
		tblStrct[iFld][2]+" DEFAULT " + ((tblStrct[iFld][4]==DB_NULL)?"NULL":tblStrct[iFld][4]) + " ";
	    if(s2i(tblStrct[iFld][5]))
		pr_keys += (pr_keys.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\"";
	}
    }

    //Check for need append or modify
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	TCfg &cf = cfg.cfg(cf_el[iCf]);
	// Check for update needs
	for(iFld = 1; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld][1]) {
		if(appMode) break;
		switch(cf.fld().type()) {
		    case TFld::String:	{ if(tblStrct[iFld][2] != "TEXT") toUpdate = true;	break; }
		    case TFld::Integer: case TFld::Boolean:
					{ if(tblStrct[iFld][2] != "INTEGER") toUpdate = true;	break; }
		    case TFld::Real:	{ if(tblStrct[iFld][2] != "DOUBLE") toUpdate = true;	break; }
		    default: toUpdate = true;
		}
		all_flds += (all_flds.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\"";
		break;
	    }

	// Type
	switch(cf.fld().type()) {
	    case TFld::String:
		tpCfg = "TEXT DEFAULT " + ((cf.fld().def()==EVAL_STR)?"NULL ":"'"+TSYS::strEncode(cf.fld().def(),TSYS::SQL,"'")+"' ");
		break;
	    case TFld::Boolean:
		tpCfg = "INTEGER DEFAULT " + ((s2i(cf.fld().def())==EVAL_BOOL)?"NULL ":"'"+i2s(s2i(cf.fld().def()))+"' ");
		break;
	    case TFld::Integer:
		tpCfg = "INTEGER DEFAULT " + ((s2ll(cf.fld().def())==EVAL_INT)?"NULL ":"'"+ll2s(s2ll(cf.fld().def()))+"' ");
		break;
	    case TFld::Real:
		tpCfg = "DOUBLE DEFAULT " + ((s2r(cf.fld().def())==EVAL_REAL)?"NULL ":"'"+r2s(s2r(cf.fld().def()))+"' ");
		break;
	    default:	break;
	}

	// Append
	if(iFld >= tblStrct.size() || !appMode) {
	    ls += (ls.size()?",\"":"\"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\" " + tpCfg;
	    if(iFld >= tblStrct.size()) toUpdate = true;
	}

	// Other languages for translation process
	if((cf.fld().flg()&TFld::TransltText) && !cf.noTransl()) {
	    bool col_cur = false;
	    for(unsigned iC = 1; iC < tblStrct.size(); iC++)
		if(tblStrct[iC][1].size() > 3 && tblStrct[iC][1].substr(2) == ("#"+cf_el[iCf])) {
		    all_flds += ",\"" + TSYS::strEncode(tblStrct[iC][1],TSYS::SQL,"\"") + "\"";
		    ls += ",\"" + TSYS::strEncode(tblStrct[iC][1],TSYS::SQL,"\"") + "\" " + tpCfg;
		    if(tblStrct[iC][1].compare(0,2,Mess->lang2Code()) == 0) col_cur = true;
		}
	    if(!col_cur && isVarTextTransl) {
		toUpdate = true;
		ls += ",\"" + TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[iCf],TSYS::SQL,"\"") + "\" " + tpCfg;
	    }
	}
	// Primary key
	else if(cf.fld().flg()&TCfg::Key && !appMode)
	    pr_keys += (pr_keys.size()?",\"":"\"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\"";
    }
    //Check deleted fields
    for(unsigned iFld = 1, iCf; iFld < tblStrct.size() && !toUpdate && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld][1] ||
		    ((cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText) && !cfg.cfg(cf_el[iCf]).noTransl() &&
		    tblStrct[iFld][1].size() > 3 && tblStrct[iFld][1].substr(2) == ("#"+cf_el[iCf]) && tblStrct[iFld][1].compare(0,2,Mess->lang2Code()) != 0))
		break;
	if(iCf >= cf_el.size()) toUpdate = true;
    }

    if(!toUpdate) return;

    //The copy is needed in save to the temporary table
    if(all_flds.size()) {
	string req = "CREATE TEMPORARY TABLE 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "'(" + all_flds + ");"
	      "INSERT INTO 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' SELECT " + all_flds + " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';"
	      "DROP TABLE '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';";
	owner().sqlReq(req, NULL, true);
    }

    //Create new-updated table
    string crtReq = "CREATE TABLE IF NOT EXISTS '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' (" + ls + ", PRIMARY KEY (" + pr_keys + "));";
    owner().sqlReq(crtReq, NULL, true);

    //Restore the data from the temporary table
    if(all_flds.size()) {
	string req = "INSERT INTO '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "'(" + all_flds + ") SELECT " + all_flds +
	      " FROM 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';DROP TABLE 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';";
	owner().sqlReq(req, NULL, true);
    }

    //Update the table structure
    string req = "PRAGMA table_info('" + TSYS::strEncode(name(),TSYS::SQL,"'") + "');";
    owner().sqlReq(req, &tblStrct, false);
}

string MTable::getVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->lang2Code());
	string prntRes = rez;
	bool isBin = false;
	for(unsigned iCh = 0; !isBin && iCh < prntRes.size(); ++iCh)
	    switch(prntRes[iCh]) {
		case 0 ... 8:
		case 0xB ... 0xC:
		case 0x0E ... 0x1F: isBin = true; break;
		case '\'': prntRes.insert(iCh, 1, prntRes[iCh]); ++iCh; break;
	    }
	return isBin ? "X'"+TSYS::strDecode(rez,TSYS::Bin)+"'" : "'"+prntRes+"'";
    }

    return rez;
}

void MTable::setVal( TCfg &cf, const string &ival, bool tr )
{
    string val = (ival==DB_NULL) ? EVAL_STR : ival;
    switch(cf.fld().type()) {
	case TFld::String:
	    if(!cf.extVal()) {
		if(!tr || (cf.fld().flg()&TFld::TransltText && !cf.noTransl())) cf.setS(val);
		if(!tr && cf.fld().flg()&TFld::TransltText && !cf.noTransl()) Mess->translReg(val, "db:"+fullDBName()+"#"+cf.name());
	    }
	    else {
		if(!tr) {
		    cf.setS(val, TCfg::ExtValOne);
		    cf.setS("", TCfg::ExtValTwo);	//!! Sets for clean up from previous Two value
		    cf.setS("db:"+fullDBName()+"#"+cf.name(), TCfg::ExtValThree);
		} else cf.setS(val, TCfg::ExtValTwo);
	    }
	    break;
	default: cf.setS(val); break;
    }
}

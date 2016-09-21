
//OpenSCADA system module BD.SQLite file: bd_sqlite.cpp
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
#define MOD_VER		"2.2.8"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("BD module. Provides support of the BD SQLite.")
#define LICENSE		"GPL2"
//******************************************************************************

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

TBD *BDMod::openBD( const string &iid )	{ return new MBD(iid,&owner().openDB_E()); }

void BDMod::modStop( )
{
    vector<string> dbs;

    //Close transactions for all DB
    list(dbs);
    for(unsigned i_db = 0; i_db < dbs.size(); i_db++)
	at(dbs[i_db]).at().transCommit();
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

    if(flag && owner().fullDeleteDB())
	if(remove(TSYS::strSepParse(addr(),0,';').c_str()) != 0)
	    throw err_sys(_("Delete bd error: %s"), strerror(errno));
}

void MBD::enable( )
{
    MtxAlloc res(connRes, true);
    if(enableStat()) return;

    cd_pg = codePage().size()?codePage():Mess->charset();
    int rc = sqlite3_open(TSYS::strSepParse(addr(),0,';').c_str(), &m_db);
    if(rc) {
	string err = sqlite3_errmsg(m_db);
	sqlite3_close(m_db);
	throw err_sys(_("Open DB file error: %s"), err.c_str());
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

void MBD::allowList( vector<string> &list )
{
    if(!enableStat()) return;
    list.clear();
    vector< vector<string> > tbl;
    sqlReq("SELECT name FROM sqlite_master WHERE type IN ('table','view') AND name NOT LIKE 'sqlite_%';", &tbl, false);
    for(unsigned i_t = 1; i_t < tbl.size(); i_t++)
	list.push_back(tbl[i_t][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error open table '%s'. DB is disabled."), inm.c_str());

    try { sqlReq("SELECT * FROM '" + TSYS::strEncode(inm,TSYS::SQL,"'") + "' LIMIT 0;"); }
    catch(...) { if(!create) throw; }

    return new MTable(inm, this);
}

void MBD::sqlReq( const string &req, vector< vector<string> > *tbl, char intoTrans )
{
    char *zErrMsg = NULL;
    int rc, nrow = 0, ncol = 0;
    char **result = NULL;

    if(tbl) tbl->clear();
    if(!enableStat())	return;

    MtxAlloc res(connRes, true);//!! Moved before the transaction checking for prevent the "BEGIN;" and "COMMIT;"
				//   request's sequence breakage on high concurrency access activity

    //Commit set
    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    //Put request
    rc = sqlite3_get_table(m_db, Mess->codeConvOut(cd_pg.c_str(),req).c_str(), &result, &nrow, &ncol, &zErrMsg);
    if(rc != SQLITE_OK) {
	string err = _("Unknown error");
	if(zErrMsg) { err = zErrMsg; sqlite3_free(zErrMsg); }
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("Request \"%s\" error: %s"), req.c_str(), err.c_str());
	throw err_sys(100+rc, _("Request \"%s\" error: %s"), TSYS::strMess(50,"%s",req.c_str()).c_str(), err.c_str());
    }
    if(tbl && ncol > 0) {
	vector<string> row;
	// Add head
	for(int i = 0; i < ncol; i++) row.push_back(result[i]?result[i]:"");
	tbl->push_back(row);
	// Add data
	for(int i = 0; i < nrow; i++) {
	    row.clear();
	    for(int ii = 0; ii < ncol; ii++)
		row.push_back(result[(i+1)*ncol+ii]?Mess->codeConvIn(cd_pg.c_str(),result[(i+1)*ncol+ii]):"");
	    tbl->push_back(row);
	}
    }
    if(result) sqlite3_free_table(result);
}

void MBD::transOpen( )
{
    //Check for limit into one trinsaction
    if(reqCnt > 1000) transCommit();

    connRes.lock();
    bool begin = !reqCnt;
    if(begin) trOpenTm = SYS->sysTm();
    reqCnt++;
    reqCntTm = SYS->sysTm();
    connRes.unlock();

    if(begin) sqlReq("BEGIN;");
}

void MBD::transCommit( )
{
    connRes.lock();
    bool commit = reqCnt;
    reqCnt = reqCntTm = 0;
    connRes.unlock();

    if(commit) sqlReq("COMMIT;");
}

void MBD::transCloseCheck( )
{
    if(enableStat() && reqCnt && ((SYS->sysTm()-reqCntTm) > 60 || (SYS->sysTm()-trOpenTm) > 10*60)) transCommit();
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,3,
	    "dest","sel_ed","select","/prm/cfg/dbFsList","help",
		    _("SQLite DB address must be written as: [<FileDBPath>].\n"
		      "Where:\n"
		      "  FileDBPath - full path to DB file (./oscada/Main.db).\n"
		      "               Use empty path for a private, temporary on-disk database create.\n"
		      "               Use \":memory:\" for a private, temporary in-memory database create."));
	if(reqCnt)
	    ctrMkNode("comm",opt,-1,"/prm/st/end_tr",_("Close opened transaction"),RWRWRW,"root",SDB_ID);
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/dbFsList" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText(":memory:");
	TSYS::ctrListFS(opt, addr(), "db;");
    }
    else if(a_path == "/prm/st/end_tr" && ctrChkNode(opt,"set",RWRWRW,"root",SDB_ID,SEC_WR) && reqCnt) transCommit();
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
    if(flag)
	try{ owner().sqlReq("DROP TABLE '"+TSYS::strEncode(name(),TSYS::SQL,"'")+"';"); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

MBD &MTable::owner()	{ return (MBD&)TTable::owner(); }

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

bool MTable::fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full )
{
    vector< vector<string> >	inTbl,
				&tbl = full ? *full : inTbl;

    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    //Check for no present and no empty keys allow
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

    string sid;
    //Make WHERE
    string req = "SELECT ";
    string req_where = "WHERE ";
    // Add use keys to list
    bool first_sel = true, next = false, trPresent = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld][1];
	TCfg *u_cfg = cfg.at(sid,true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3),true);
	    if(u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg) + " ";
	    next = true;
	}
	else if(u_cfg->isKey() || u_cfg->view()) {
	    req += (first_sel?"\"":",\"")+TSYS::strEncode(sid,TSYS::SQL,"\"")+"\"";
	    first_sel = false;
	}
    }

    //Request
    if(!full || !full->size() || row == 0) {
	if(first_sel) return false;
	req += " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' " + ((next)?req_where:"");
	if(!full) req += " LIMIT " +  i2s(row) + ",1";
	req += ";";

	tbl.clear();
	owner().sqlReq(req, &tbl/*, false*/);	// For seek to deletion into save context do not set to "false"
    }

    if(tbl.size() < 2 || (full && (row+1) >= tbl.size())) return false;

    //Processing of the query
    row = full ? row+1 : 1;
    for(unsigned iFld = 0; iFld < tbl[0].size(); iFld++) {
	sid = tbl[0][iFld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg, tbl[row][iFld]);
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[row][iFld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setVal(*u_cfg, tbl[row][iFld], true);
    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    string sid;
    //Prepare request
    string req = "SELECT ";
    string req_where, first_key;
    // Add fields list to queue
    bool first_sel = true, next_wr = false, trPresent = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld][1];
	TCfg *u_cfg = cfg.at(sid,true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey()) {
	    req_where += (next_wr?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg);
	    if(first_key.empty()) first_key = TSYS::strEncode(sid,TSYS::SQL,"\"");
	    next_wr = true;
	}
	else if(u_cfg->view()) {
	    req += (first_sel?"\"":",\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"";
	    first_sel = false;
	}
    }
    if(first_sel) req += "\""+first_key+"\"";
    req += " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' WHERE " + req_where + ";";

    //Query
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) throw err_sys(_("Row \"%s\" is not present."), req_where.c_str());

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
    vector< vector<string> > tbl;

    if(tblStrct.empty()) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }
    mLstUse = SYS->sysTm();

    string sid, sval;
    bool isVarTextTransl = (!Mess->lang2CodeBase().empty() && Mess->lang2Code() != Mess->lang2CodeBase());

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Check for translation present
    bool trPresent = isVarTextTransl, trDblDef = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	if(trPresent && (!isVarTextTransl || trDblDef)) break;
	sid = tblStrct[iFld][1];
	if(sid.size() > 3) {
	    if(!trPresent && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) trPresent = true;
	    if(Mess->lang2Code() == Mess->lang2CodeBase() && !trDblDef && sid.compare(0,3,Mess->lang2CodeBase()+"#") == 0) trDblDef = true;
	}
    }
    if(trDblDef && !cfg.reqKeys()) fieldFix(cfg);

    //Get present fields list
    string req_where = "WHERE ";
    // Add key list to queue
    bool next = false, noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();			//Force update by ReqKeys or reqKey() present
    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if(!u_cfg.isKey()) continue;
	req_where += (next?" AND \"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL,"\"") + "\"=" + getVal(u_cfg,true,TCfg::ExtValTwo);
	next = true;

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

    //Prepare query for presenting detect
    string req;
    if(!isForceUpdt) {
	req = "SELECT 1 FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' " + req_where + ";";
	owner().sqlReq(req, &tbl, true);
	if(tbl.size() < 2) {
	    //Add new record
	    req = "INSERT INTO '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' ";
	    string ins_name, ins_value;
	    next = false;
	    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
		TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
		if(!u_cfg.isKey() && !u_cfg.view()) continue;

		bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
		ins_name += (next?",\"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL,"\"") + "\" " +
		    (isTransl ? (",\""+TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[i_el],TSYS::SQL,"\"")+"\" ") : "");
		sval = getVal(u_cfg);
		ins_value += (next?",":"") + sval + " " + (isTransl?(","+sval+" "):"");
		next = true;
	    }
	    req += "(" + ins_name + ") VALUES (" + ins_value + ")";
	}
	else isForceUpdt = true;
    }
    //Update present record
    if(isForceUpdt) {
	req = "UPDATE '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' SET ";
	next = false;
	for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if((u_cfg.isKey() && !u_cfg.extVal()) || !u_cfg.view()) continue;

	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    req += (next?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(u_cfg) + " ";
	    next = true;
	}
	req += req_where;
    }
    req += ";";

    //Query
    try { owner().sqlReq(req, NULL, true); }
    catch(TError &err) {
	if((err.cod-100) == SQLITE_READONLY) throw;
	fieldFix(cfg);
	owner().sqlReq(req, NULL, true);
    }
}

void MTable::fieldDel( TConfig &cfg )
{
    if(tblStrct.empty()) return;
    mLstUse = SYS->sysTm();

    //Where prepare
    string req_where = "WHERE ";
    bool next = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld][1];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg) + " ";
	    next = true;
	}
    }

    //Main request
    try { owner().sqlReq("DELETE FROM '"+TSYS::strEncode(name(),TSYS::SQL,"'")+"' "+req_where+";", NULL, true); }
    catch(TError &err) {
	//Check for present
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM '"+TSYS::strEncode(name(),TSYS::SQL,"'")+"' "+req_where+";", &tbl, true);
	if(tbl.size() < 2) return;
    }
}

void MTable::fieldFix( TConfig &cfg )
{
    bool toUpdate = false,
	 appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !tblStrct.empty()),	//Only for append no present fields
	 isVarTextTransl = (!Mess->lang2CodeBase().empty() && Mess->lang2Code() != Mess->lang2CodeBase());

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Create request variables
    string all_flds, pr_keys, tpCfg, req,
	crtReq = "CREATE TABLE IF NOT EXISTS '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' (";
    bool next = false, next_key = false;

    //Curent context copy list
    if(appMode) {
	if(tblStrct.empty()) return;
	for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	    all_flds += (all_flds.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\"";
	    crtReq += (next?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\" "+
		tblStrct[iFld][2]+" DEFAULT " + tblStrct[iFld][4] + " ";
	    next = true;
	    if(s2i(tblStrct[iFld][5])) {
		pr_keys += (next_key?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\"";
		next_key = true;
	    }
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
		    case TFld::String:	if(tblStrct[iFld][2] != "TEXT")	toUpdate = true;	break;
		    case TFld::Integer: case TFld::Boolean:
					if(tblStrct[iFld][2] != "INTEGER")	toUpdate = true;	break;
		    case TFld::Real:	if(tblStrct[iFld][2] != "DOUBLE")	toUpdate = true;	break;
		    default: toUpdate = true;
		}
		all_flds += (all_flds.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld][1],TSYS::SQL,"\"") + "\"";
		break;
	    }

	// Type
	switch(cf.fld().type()) {
	    case TFld::String:	tpCfg = "TEXT DEFAULT '" + TSYS::strEncode(cf.fld().def(),TSYS::SQL,"'") + "' ";	break;
	    case TFld::Integer: case TFld::Boolean:
				tpCfg = "INTEGER DEFAULT '" + TSYS::strEncode(cf.fld().def(),TSYS::SQL,"'") + "' ";	break;
	    case TFld::Real:	tpCfg = "DOUBLE DEFAULT '" + TSYS::strEncode(cf.fld().def(),TSYS::SQL,"'") + "' ";	break;
	    default:	break;
	}

	// Append
	if(iFld >= tblStrct.size() || !appMode) {
	    crtReq += (next?",\"":"\"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\" " + tpCfg;
	    next = true;
	    if(iFld >= tblStrct.size()) toUpdate = true;
	}

	// Other languages for translation process
	if(cf.fld().flg()&TCfg::TransltText) {
	    bool col_cur = false;
	    for(unsigned iC = 1; iC < tblStrct.size(); iC++)
		if(tblStrct[iC][1].size() > 3 && tblStrct[iC][1].substr(2) == ("#"+cf_el[iCf])) {
		    all_flds += ",\"" + TSYS::strEncode(tblStrct[iC][1],TSYS::SQL,"\"") + "\"";
		    crtReq += ",\"" + TSYS::strEncode(tblStrct[iC][1],TSYS::SQL,"\"") + "\" " + tpCfg;
		    if(tblStrct[iC][1].compare(0,2,Mess->lang2Code()) == 0) col_cur = true;
		}
	    if(!col_cur && isVarTextTransl) {
		toUpdate = true;
		crtReq += ",\"" + TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[iCf],TSYS::SQL,"\"") + "\" " + tpCfg;
	    }
	}
	// Primary key
	else if(cf.fld().flg()&TCfg::Key && !appMode) {
	    pr_keys += (next_key?",\"":"\"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\"";
	    next_key = true;
	}
    }
    //Check deleted fields
    for(unsigned iFld = 1, iCf; iFld < tblStrct.size() && !toUpdate && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld][1] ||
		    (cfg.cfg(cf_el[iCf]).fld().flg()&TCfg::TransltText && tblStrct[iFld][1].size() > 3 &&
		    tblStrct[iFld][1].substr(2) == ("#"+cf_el[iCf]) && tblStrct[iFld][1].compare(0,2,Mess->lang2Code()) != 0))
		break;
	    if(iCf >= cf_el.size()) toUpdate = true;
	}

    if(!toUpdate) return;

    //Copy need for save to temporary table
    if(all_flds.size()) {
	req = "CREATE TEMPORARY TABLE 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "'(" + all_flds + ");"
	      "INSERT INTO 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' SELECT " + all_flds + " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';"
	      "DROP TABLE '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';";
	owner().sqlReq(req, NULL, true);
    }

    //Create new, updated table
    crtReq += ", PRIMARY KEY (" + pr_keys + "));";
    owner().sqlReq(crtReq, NULL, true);

    //Restore data from temporary table
    if(all_flds.size()) {
	req = "INSERT INTO '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "'(" + all_flds + ") SELECT " + all_flds +
	      " FROM 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';DROP TABLE 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';";
	owner().sqlReq(req, NULL, true);
    }

    //Update the table structure
    req = "PRAGMA table_info('" + TSYS::strEncode(name(),TSYS::SQL,"'") + "');";
    owner().sqlReq(req, &tblStrct, false);
}

string MTable::getVal( TCfg &cfg, bool toEnc, uint8_t RqFlg )
{
    string rez;
    switch(cfg.fld().type()) {	//!! Different types for correct EVAL represent
	case TFld::Boolean:	rez = i2s(cfg.getB());	break;
	case TFld::Integer:	rez = i2s(cfg.getI());	break;
	case TFld::Real:	rez = r2s(cfg.getR());	break;
	default:
	    rez = cfg.getS(RqFlg);
	    if(toEnc) {
		string prntRes = rez;
		bool isBin = false;
		for(unsigned iCh = 0; !isBin && iCh < prntRes.size(); ++iCh)
		    switch(prntRes[iCh]) {
			case 0: isBin = true; break;
			case '\'': prntRes.insert(iCh, 1, prntRes[iCh]); ++iCh; break;
		    }
		return isBin ? "X'"+TSYS::strDecode(rez, TSYS::Bin)+"'" : "'"+prntRes+"'";
	    }
	    break;
    }

    return toEnc ? "'"+rez+"'" : rez;

    /*string rez = cfg.getS(RqFlg);
    if(cfg.fld().type() == TFld::String && toEnc) {
	string prntRes = rez;
	bool isBin = false;
	for(unsigned iCh = 0; !isBin && iCh < prntRes.size(); ++iCh)
	    switch(prntRes[iCh]) {
		case 0: isBin = true; break;
		case '\'': prntRes.insert(iCh, 1, prntRes[iCh]); ++iCh; break;
	    }
	return isBin ? "X'"+TSYS::strDecode(rez, TSYS::Bin)+"'" : "'"+prntRes+"'";
    }

    return toEnc ? "'"+rez+"'" : rez;*/

    /*switch(cfg.fld().type()) {
	case TFld::String: {
	    if(!toEnc) return cfg.getS(RqFlg);
	    string prntRes = cfg.getS(RqFlg);
	    bool isBin = false;
	    for(unsigned iCh = 0; !isBin && iCh < prntRes.size(); ++iCh)
		switch(prntRes[iCh]) {
		    case 0: isBin = true; break;
		    case '\'':	prntRes.insert(iCh,1,prntRes[iCh]); ++iCh; break;
		}
	    return isBin ? "X'"+TSYS::strDecode(cfg.getS(RqFlg),TSYS::Bin)+"'" : "'"+prntRes+"'";
	    break;
	}
	default: return toEnc ? "'"+cfg.getS(RqFlg)+"'" : cfg.getS(RqFlg);
    }

    return "";*/
}

void MTable::setVal( TCfg &cf, const string &val, bool tr )
{
    if(!cf.extVal()) {
	if(!tr || (cf.fld().flg()&TCfg::TransltText && !cf.noTransl())) cf.setS(val);
	if(!tr && cf.fld().flg()&TCfg::TransltText && !cf.noTransl()) Mess->translReg(val, "db:"+fullDBName()+"#"+cf.name());
    }
    else {
	if(!tr) {
	    cf.setS(val, TCfg::ExtValOne);
	    cf.setS("", TCfg::ExtValTwo);	//!! Sets for clean up from previous Two value
	    cf.setS("db:"+fullDBName()+"#"+cf.name(), TCfg::ExtValThree);
	} else cf.setS(val, TCfg::ExtValTwo);
    }
}


//OpenSCADA module BD.SQLite file: bd_sqlite.cpp
/***************************************************************************
 *   Copyright (C) 2003-2022 by Roman Savochenko, <roman@oscada.org>       *
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
#define MOD_NAME	trS("DB SQLite")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"4.0.6"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("BD module. Provides support of the BD SQLite.")
#define LICENSE		"GPL2"
#define FEATURES	"SQL, LIST, STRUCT, GET, SEEK, PRELOAD, SET, DEL, FIX, TR, ERR"
//******************************************************************************

#define TRANS_CLOSE_TM_AFT_REQ	10
#define TRANS_CLOSE_TM_AFT_OPEN	60

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
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0), trans_reqs(1)
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
	throw err_sys((rc==SQLITE_READONLY)?TError::DB_ReadOnly:TError::EXT+rc,
	    _("Error of the request \"%s\": %s(%d)"), TSYS::strEncode(req,TSYS::Limit,"50").c_str(), err.c_str(), rc);
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

void MBD::getStructDB( const string &nm, vector<TTable::TStrIt> &tblStrct )
{
    vector< vector<string> > origTblStrct;

    sqlReq("PRAGMA table_info('"+TSYS::strEncode(nm,TSYS::SQL,"'")+"')", &origTblStrct);

    tblStrct.clear();
    for(unsigned iReq = 1; iReq < origTblStrct.size(); ++iReq)
	tblStrct.push_back(TTable::TStrIt(origTblStrct[iReq][1],origTblStrct[iReq][2],s2i(origTblStrct[iReq][5]),origTblStrct[iReq][4]));
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
MTable::MTable( string name, MBD *iown ) : TTable(name)
{
    setNodePrev(iown);

    try { owner().getStructDB(name, tblStrct); } catch(...) { }
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

    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld].nm;
	if(cfg.cfgPresent(sid)) continue;

	int flg = tblStrct[iFld].key ? (int)TCfg::Key : (int)TFld::NoFlag;
	if(tblStrct[iFld].tp == "TEXT")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(tblStrct[iFld].tp == "INTEGER")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(tblStrct[iFld].tp == "REAL" || tblStrct[iFld].tp == "DOUBLE")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    if(tblStrct.empty()) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }

    fieldSQLSet(cfg);
}

void MTable::fieldFix( TConfig &cfg, const string &ilangLs )
{
    bool toUpdate = false,
	 appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !tblStrct.empty());	//Only for append no present fields

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Create request variables
    string all_flds, pr_keys, tpCfg, ls, langLs;

    //Curent context copy list
    if(appMode) {
	if(tblStrct.empty()) return;
	for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	    all_flds += (all_flds.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL,"\"") + "\"";
	    ls += (ls.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL,"\"") + "\" "+
		tblStrct[iFld].tp+" DEFAULT " + ((tblStrct[iFld].def==DB_NULL)?"NULL":tblStrct[iFld].def) + " ";
	    if(tblStrct[iFld].key)
		pr_keys += (pr_keys.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL,"\"") + "\"";
	}
    }

    //Check for need to append or modify
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	langLs = ilangLs;
	TCfg &cf = cfg.cfg(cf_el[iCf]);
	// Check for update needs
	for(iFld = 0; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld].nm) {
		if(appMode) break;
		switch(cf.fld().type()) {
		    case TFld::String:	{ if(tblStrct[iFld].tp != "TEXT") toUpdate = true;	break; }
		    case TFld::Integer: case TFld::Boolean:
					{ if(tblStrct[iFld].tp != "INTEGER") toUpdate = true;	break; }
		    case TFld::Real:	{ if(tblStrct[iFld].tp != "DOUBLE") toUpdate = true;	break; }
		    default: toUpdate = true;
		}
		all_flds += (all_flds.size()?",\"":"\"") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL,"\"") + "\"";
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
	if(cf.fld().flg()&TFld::TransltText) {
	    size_t pos = 0;
	    //  Copy for present translations
	    for(unsigned iC = 0; iC < tblStrct.size(); iC++)
		if(tblStrct[iC].nm.size() > 3 && tblStrct[iC].nm.substr(2) == ("#"+cf_el[iCf])) {
		    all_flds += ",\"" + TSYS::strEncode(tblStrct[iC].nm,TSYS::SQL,"\"") + "\"";
		    ls += ",\"" + TSYS::strEncode(tblStrct[iC].nm,TSYS::SQL,"\"") + "\" " + tpCfg;

		    if((pos=langLs.find(tblStrct[iC].nm.substr(0,2)+";")) != string::npos)
			langLs.replace(pos, 3, "");
		}

	    //  Append translation for new languages
	    if(langLs.size()) toUpdate = true;
	    string toLang;
	    for(int off = 0; (toLang=TSYS::strParse(langLs,0,";",&off)).size(); )
		ls += ",\"" + TSYS::strEncode(toLang+"#"+cf_el[iCf],TSYS::SQL,"\"") + "\" " + tpCfg;
	}
	// Primary key
	else if(cf.fld().flg()&TCfg::Key && !appMode)
	    pr_keys += (pr_keys.size()?",\"":"\"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\"";
    }
    //Checking for deleted fields
    for(unsigned iFld = 0, iCf; iFld < tblStrct.size() && !toUpdate && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld].nm ||
		    // Pass all the column translation
		    (cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText &&
			tblStrct[iFld].nm.size() > 3 && tblStrct[iFld].nm.substr(2) == ("#"+cf_el[iCf])))
		break;
	if(iCf >= cf_el.size()) toUpdate = true;
    }

    if(!toUpdate) return;

    //The copy is needed in save to the temporary table
    if(all_flds.size()) {
	string req = "CREATE TEMPORARY TABLE 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "'(" + all_flds + ");"
	    "INSERT INTO 'temp_" + TSYS::strEncode(name(),TSYS::SQL,"'") + "' "
	    "SELECT " + all_flds + " FROM '" + TSYS::strEncode(name(),TSYS::SQL,"'") + "';"
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
    owner().getStructDB(name(), tblStrct);
}

string MTable::getSQLVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	//if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->langCode());
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

void MTable::setSQLVal( TCfg &cf, const string &ival, bool tr )
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

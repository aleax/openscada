
//OpenSCADA system module BD.SQLite file: bd_sqlite.cpp
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
#define MOD_VER		"1.6.4"
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
BDMod::BDMod( string name ) : TTipBD(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
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

void BDMod::load_( )
{
    //Load parameters from command line

    //Load parameters from config-file
}

string BDMod::sqlReqCode( const string &req, char symb )
{
    string sout = req;
    for(unsigned i_sz = 0; i_sz < sout.size(); i_sz++)
	if(sout[i_sz] == symb) sout.replace(i_sz++, 1, 2, symb);

    return sout;
}

//************************************************
//* BDSQLite::MBD				 *
//************************************************
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0), trans_reqs(1)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&connRes, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

MBD::~MBD( )
{
    pthread_mutex_destroy(&connRes);
}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag && owner().fullDeleteDB())
	if(remove(TSYS::strSepParse(addr(),0,';').c_str()) != 0)
	    throw TError(nodePath().c_str(), _("Delete bd error: %s"), strerror(errno));
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
	throw TError(nodePath().c_str(), _("Open DB file error: %s"), err.c_str());
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
    if(!enableStat()) throw TError(nodePath().c_str(), _("Error open table '%s'. DB is disabled."), inm.c_str());

    return new MTable(inm, this, create);
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    char *zErrMsg = NULL;
    int rc, nrow = 0, ncol = 0;
    char **result = NULL;

    if(tbl) tbl->clear();
    if(!enableStat())	return;

    //Commit set
    string req = ireq;

    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    MtxAlloc res(connRes, true);

    //Put request
    rc = sqlite3_get_table(m_db, Mess->codeConvOut(cd_pg.c_str(),req).c_str(), &result, &nrow, &ncol, &zErrMsg);
    if(rc != SQLITE_OK) {
	string err = _("Unknown error");
	if(zErrMsg) { err = zErrMsg; sqlite3_free(zErrMsg); }
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Request \"%s\" error: %s"), req.c_str(), err.c_str());
	throw TError(100+rc, nodePath().c_str(), _("Request \"%s\" error: %s"), TSYS::strMess(50,"%s",req.c_str()).c_str(), err.c_str());
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

    pthread_mutex_lock(&connRes);
    bool begin = !reqCnt;
    if(begin) trOpenTm = SYS->sysTm();
    reqCnt++;
    reqCntTm = SYS->sysTm();
    pthread_mutex_unlock(&connRes);

    if(begin) sqlReq("BEGIN;");
}

void MBD::transCommit( )
{
    pthread_mutex_lock(&connRes);
    bool commit = reqCnt;
    reqCnt = reqCntTm = 0;
    pthread_mutex_unlock(&connRes);

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
MTable::MTable( string inm, MBD *iown, bool create ) : TTable(inm)
{
    setNodePrev(iown);

    try {
	string req = "SELECT * FROM '"+mod->sqlReqCode(name())+"' LIMIT 0;";	//!! Need for table present checking
	owner().sqlReq(req);
	req ="PRAGMA table_info('"+mod->sqlReqCode(name())+"');";
	owner().sqlReq(req, &tblStrct);
    }
    catch(...) { if( !create ) throw; }
}

MTable::~MTable( )
{

}

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag)
	try{ owner().sqlReq("DROP TABLE '"+mod->sqlReqCode(name())+"';"); }
	catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

MBD &MTable::owner()	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty."));
    mLstUse = SYS->sysTm();

    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++)
    {
	string sid = tblStrct[i_fld][1];
	if(cfg.cfgPresent(sid)) continue;

	int flg = (tblStrct[i_fld][5]=="1") ? (int)TCfg::Key : (int)TFld::NoFlag;
	if(tblStrct[i_fld][2] == "TEXT")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(tblStrct[i_fld][2] == "INTEGER")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(tblStrct[i_fld][2] == "REAL" || tblStrct[i_fld][2] == "DOUBLE")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty."));
    mLstUse = SYS->sysTm();

    //Check for no present and no empty keys allow
    if(row == 0) {
	vector<string> cf_el;
	cfg.cfgList(cf_el);
	for(unsigned i_c = 0; i_c < cf_el.size(); i_c++) {
	    TCfg &cf = cfg.cfg(cf_el[i_c]);
	    if(!(cf.fld().flg()&TCfg::Key) || !cf.getS().size()) continue;
	    unsigned i_fld = 1;
	    for( ; i_fld < tblStrct.size(); i_fld++)
		if(cf.name() == tblStrct[i_fld][1]) break;
	    if(i_fld >= tblStrct.size()) return false;
	}
    }

    string sid;
    //Make WHERE
    string req = "SELECT ";
    string req_where = "WHERE ";
    // Add use keys to list
    bool first_sel = true, next = false, trPresent = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	sid = tblStrct[i_fld][1];
	TCfg *u_cfg = cfg.at(sid,true);
	if(!cfg.noTransl() && !u_cfg && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#"))
	{
	    u_cfg = cfg.at(sid.substr(3),true);
	    if(u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->fld().flg()&TCfg::Key && u_cfg->keyUse()) {
	    req_where += (next?" AND \"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(getVal(*u_cfg)) + "' ";
	    next = true;
	}
	else if(u_cfg->fld().flg()&TCfg::Key || u_cfg->view()) {
	    req += (first_sel?"\"":",\"")+mod->sqlReqCode(sid,'"')+"\"";
	    first_sel = false;
	}
    }

    //Request
    if(first_sel) return false;
    req += " FROM '" + mod->sqlReqCode(name()) + "' " + ((next)?req_where:"") + " LIMIT " +  i2s(row) + ",1;";
    owner().sqlReq(req, &tbl/*, false*/);	// For seek to deletion into save context do not set to "false"
    if(tbl.size() < 2) return false;
    //Processing of query
    for(unsigned i_fld = 0; i_fld < tbl[0].size(); i_fld++) {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg, tbl[1][i_fld]);
	else if(trPresent && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#") && tbl[1][i_fld].size())
	{
	    u_cfg = cfg.at(sid.substr(3),true);
	    if(u_cfg) setVal(*u_cfg,tbl[1][i_fld]);
	}
    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty."));
    mLstUse = SYS->sysTm();

    string sid;
    //Prepare request
    string req = "SELECT ";
    string req_where, first_key;
    // Add fields list to queue
    bool first_sel = true, next_wr = false, trPresent = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	sid = tblStrct[i_fld][1];
	TCfg *u_cfg = cfg.at(sid,true);
	if(!cfg.noTransl() && !u_cfg && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#"))
	{
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->fld().flg()&TCfg::Key) {
	    req_where += (next_wr?" AND \"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(getVal(*u_cfg)) + "'";
	    if(first_key.empty()) first_key = mod->sqlReqCode(sid,'"');
	    next_wr = true;
	}
	else if(u_cfg->view()) {
	    req += (first_sel?"\"":",\"") + mod->sqlReqCode(sid,'"') + "\"";
	    first_sel = false;
	}
    }
    if(first_sel) req += "\""+first_key+"\"";
    req += " FROM '" + mod->sqlReqCode(name()) + "' WHERE " + req_where + ";";

    //Query
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) throw TError(nodePath().c_str(), _("Row \"%s\" is not present."), req_where.c_str());

    //Processing of query
    for(unsigned i_fld = 0; i_fld < tbl[0].size(); i_fld++) {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg,tbl[1][i_fld]);
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[1][i_fld].size())
	{
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && u_cfg->fld().flg()&TCfg::TransltText && !u_cfg->noTransl()) setVal(*u_cfg, tbl[1][i_fld]);
	}
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) fieldFix(cfg);
    mLstUse = SYS->sysTm();

    string sid, sval;
    bool isVarTextTransl = (!Mess->lang2CodeBase().empty() && !cfg.noTransl() && Mess->lang2Code() != Mess->lang2CodeBase());

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Check for translation present
    bool trPresent = isVarTextTransl, trDblDef = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	if((trPresent || cfg.noTransl()) && (!isVarTextTransl || trDblDef)) break;
	sid = tblStrct[i_fld][1];
	if(sid.size() > 3) {
	    if(!trPresent && sid.substr(0,3) == (Mess->lang2Code()+"#")) trPresent = true;
	    if(Mess->lang2Code() == Mess->lang2CodeBase() && !trDblDef && sid.compare(0,3,Mess->lang2CodeBase()+"#") == 0) trDblDef = true;
	}
    }
    if(trDblDef) fieldFix(cfg);

    //Get present fields list
    string req_where = "WHERE ";
    // Add key list to queue
    bool next = false, noKeyFld = false;
    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if(!(u_cfg.fld().flg()&TCfg::Key)) continue;
	req_where += (next?" AND \"":"\"") + mod->sqlReqCode(cf_el[i_el],'"') + "\"='" + mod->sqlReqCode(getVal(u_cfg)) + "' ";
	next = true;

	// Check for no key fields
	if(noKeyFld) continue;
	unsigned i_fld = 1;
	for( ; i_fld < tblStrct.size(); i_fld++)
	    if(u_cfg.name() == tblStrct[i_fld][1]) break;
	if(i_fld >= tblStrct.size()) noKeyFld = true;
    }
    if(noKeyFld) fieldFix(cfg);

    //Prepare query
    string req = "SELECT 1 FROM '" + mod->sqlReqCode(name()) + "' " + req_where + ";";
    owner().sqlReq(req, &tbl, true);
    if(tbl.size() < 2) {
	//Add new record
	req = "INSERT INTO '" + mod->sqlReqCode(name()) + "' ";
	string ins_name, ins_value;
	next = false;
	for(unsigned i_el = 0; i_el < cf_el.size(); i_el++)
	{
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if(!(u_cfg.fld().flg()&TCfg::Key) && !u_cfg.view()) continue;

	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    ins_name += (next?",\"":"\"") + mod->sqlReqCode(cf_el[i_el],'"') + "\" " +
		(isTransl ? (",\""+mod->sqlReqCode(Mess->lang2Code()+"#"+cf_el[i_el],'"')+"\" ") : "");
	    sval = getVal(u_cfg);
	    ins_value += (next?",'":"'") + mod->sqlReqCode(sval) + "' " +
		(isTransl ? (",'"+mod->sqlReqCode(sval)+"' ") : "");
	    next = true;
	}
	req += "(" + ins_name + ") VALUES (" + ins_value + ")";
    }
    else {
	//Update present record
	req = "UPDATE '" + mod->sqlReqCode(name()) + "' SET ";
	next = false;
	for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if(u_cfg.fld().flg()&TCfg::Key || !u_cfg.view()) continue;

	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    sval = getVal(u_cfg);
	    req += (next?",\"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(sval) + "' ";
	    next = true;
	}
	req += req_where;
    }
    req += ";";

    //Query
    try { owner().sqlReq(req, NULL, true); }
    catch(TError err) {
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
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	string sid = tblStrct[i_fld][1];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->fld().flg()&TCfg::Key && u_cfg->keyUse()) {
	    req_where += (next?" AND \"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(getVal(*u_cfg)) + "' ";
	    next = true;
	}
    }

    //Main request
    try { owner().sqlReq("DELETE FROM '"+mod->sqlReqCode(name())+"' "+req_where+";", NULL, true); }
    catch(TError err)
    {
	//Check for present
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM '"+mod->sqlReqCode(name())+"' "+req_where+";", &tbl, true);
	if(tbl.size() < 2) return;
    }
}

void MTable::fieldFix( TConfig &cfg )
{
    bool fix = false;
    string all_flds;

    string req;

    bool isVarTextTransl = (!Mess->lang2CodeBase().empty() && !cfg.noTransl() && Mess->lang2Code() != Mess->lang2CodeBase());
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    if(!tblStrct.empty()) {
	//Check structure
	bool next = false;
	// Check present fields and find new fields
	for(unsigned i_cf = 0, i_fld; i_cf < cf_el.size(); i_cf++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
	    for(i_fld = 1; i_fld < tblStrct.size(); i_fld++)
		if(cf_el[i_cf] == tblStrct[i_fld][1]) {
		    all_flds += (next?",\"":"\"") + mod->sqlReqCode(tblStrct[i_fld][1],'"') + "\"";
		    next = true;
		    if(!fix)
			switch(u_cfg.fld().type())
			{
			    case TFld::String:	if(tblStrct[i_fld][2] != "TEXT")	fix = true;	break;
			    case TFld::Integer: case TFld::Boolean:
						if(tblStrct[i_fld][2] != "INTEGER")	fix = true;	break;
			    case TFld::Real:	if(tblStrct[i_fld][2] != "DOUBLE")	fix = true;	break;
			    default: fix = true;
			}
		    //Put other languages to all list an find column for current language
		    if(u_cfg.fld().flg()&TCfg::TransltText) {
			bool col_cur = false;
			for(unsigned i_c = i_fld; i_c < tblStrct.size(); i_c++)
			    if(tblStrct[i_c][1].size() > 3 && tblStrct[i_c][1].substr(2) == ("#"+cf_el[i_cf]) &&
				tblStrct[i_c][1].substr(0,2) != Mess->lang2Code())
			    {
				all_flds += ",\"" + mod->sqlReqCode(tblStrct[i_c][1],'"') + "\"";
				if(tblStrct[i_c][1].substr(0,2) == Mess->lang2Code()) col_cur = true;
			    }
			if(!col_cur && isVarTextTransl) fix = true;
		    }
		    break;
		}
	    if(i_fld >= tblStrct.size()) fix = true;
	}
	// Check delete fields
	for(unsigned i_fld = 1, i_cf; i_fld < tblStrct.size() && !fix; i_fld++)
	{
	    for(i_cf = 0; i_cf < cf_el.size(); i_cf++)
		if(cf_el[i_cf] == tblStrct[i_fld][1] ||
			(cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::TransltText && tblStrct[i_fld][1].size() > 3 &&
			tblStrct[i_fld][1].substr(2) == ("#"+cf_el[i_cf]) && tblStrct[i_fld][1].substr(0,2) != Mess->lang2Code()))
		    break;
	    if(i_cf >= cf_el.size()) fix = true;
	}

	if(!fix) return;	//Structure OK!
	// Fix structure
	//  Move data to temporary DB
	req = "CREATE TEMPORARY TABLE 'temp_" + mod->sqlReqCode(name()) + "'(" + all_flds + ");"
	    "INSERT INTO 'temp_" + mod->sqlReqCode(name()) + "' SELECT " + all_flds + " FROM '" + mod->sqlReqCode(name()) + "';"
	    "DROP TABLE '" + mod->sqlReqCode(name()) + "';";
	owner().sqlReq(req, NULL, true);
    }

    //Create new table
    req = "CREATE TABLE IF NOT EXISTS '" + mod->sqlReqCode(name()) + "' (";
    bool next = false;
    bool next_key = false;
    string pr_keys, tpCfg;
    for(unsigned i_cf = 0; i_cf < cf_el.size(); i_cf++) {
	TCfg &cf = cfg.cfg(cf_el[i_cf]);
	req += (next?",\"":"\"") + mod->sqlReqCode(cf_el[i_cf],'"') + "\" ";
	next = true;
	// Type param
	switch(cf.fld().type())
	{
	    case TFld::String:	tpCfg = "TEXT DEFAULT '" + mod->sqlReqCode(cf.fld().def()) + "' ";	break;
	    case TFld::Integer: case TFld::Boolean:
				tpCfg = "INTEGER DEFAULT '" + mod->sqlReqCode(cf.fld().def()) + "' ";	break;
	    case TFld::Real:	tpCfg = "DOUBLE DEFAULT '" + mod->sqlReqCode(cf.fld().def()) + "' ";	break;
	    default:	break;
	}
	req += tpCfg;

	//Other languages for translation process
	if(cf.fld().flg()&TCfg::TransltText) {
	    bool col_cur = false;
	    for(unsigned i_c = 1; i_c < tblStrct.size(); i_c++)
		if(tblStrct[i_c][1].size() > 3 && tblStrct[i_c][1].substr(2) == ("#"+cf_el[i_cf]) &&
		    tblStrct[i_c][1].substr(0,2) != Mess->lang2Code())
		{
		    req += ",\"" + mod->sqlReqCode(tblStrct[i_c][1],'"') + "\" " + tpCfg;
		    if(tblStrct[i_c][1].substr(0,2) == Mess->lang2Code()) col_cur = true;
		}
	    if(!col_cur && isVarTextTransl)
		req += ",\"" + mod->sqlReqCode(Mess->lang2Code()+"#"+cf_el[i_cf],'"') + "\" " + tpCfg;
	}
	// Primary key
	else if(cf.fld().flg()&TCfg::Key) {
	    pr_keys += (next_key?",\"":"\"") + mod->sqlReqCode(cf_el[i_cf],'"') + "\"";
	    next_key = true;
	}
    }

    req += ", PRIMARY KEY (" + pr_keys + "));";
    owner().sqlReq(req, NULL, true);

    //Copy data from temporary DB
    if(fix) {
	req = "INSERT INTO '" + mod->sqlReqCode(name()) + "'(" + all_flds + ") SELECT " + all_flds +
	    " FROM 'temp_" + mod->sqlReqCode(name()) + "';DROP TABLE 'temp_" + mod->sqlReqCode(name()) + "';";
	owner().sqlReq(req, NULL, true);
    }

    //Update table structure
    req = "PRAGMA table_info('" + mod->sqlReqCode(name()) + "');";
    owner().sqlReq(req, &tblStrct, false);
}

string MTable::getVal( TCfg &cfg )			{ return cfg.getS(); }

void MTable::setVal( TCfg &cfg, const string &val )	{ cfg.setS(val); }

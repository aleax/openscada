// 
//OpenSCADA module BD.PostgreSQL file: postgre.cpp
/***************************************************************************
 *   Copyright (C) 2013-2022 by Roman Savochenko, roman@oscada.org         *
 *                 2010-2011 by Maxim Lysenko, mlisenko@oscada.org         *
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

#include <string>
#include <libpq-fe.h>

#include <tsys.h>
#include <tmess.h>
#include "postgre.h"


//************************************************
//* Modul info!                                  *
#define MOD_ID		"PostgreSQL"
#define MOD_NAME	trS("DB PostgreSQL")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"3.1.0"
#define AUTHORS		trS("Roman Savochenko, Maxim Lysenko (2010-2011)")
#define DESCRIPTION	trS("DB module. Provides support of the DBMS PostgreSQL.")
#define MOD_LICENSE	"GPL2"
#define FEATURES	"SQL, LIST, STRUCT, GET, SEEK, PRELOAD, SET, DEL, FIX, TR, ERR"
//************************************************

BDPostgreSQL::BDMod *BDPostgreSQL::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_PostgreSQL_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *bd_PostgreSQL_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new BDPostgreSQL::BDMod(source);
	return NULL;
    }
}

using namespace BDPostgreSQL;

//************************************************
//* BDPostgreSQL::BDMod				 *
//************************************************
BDMod::BDMod( string name ) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, MOD_LICENSE, name);
}

BDMod::~BDMod( )	{ }

string BDMod::features( )	{ return FEATURES; }

TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }

//************************************************
//* BDPostgreSQL::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0),
    nReq(0), rqTm(0), rqTmMin(3600), rqTmMax(0), rqTmAll(0), rqTmMaxVl(dataRes()), conTm(0)
{
    setAddr(";127.0.0.1;postgres;123456;NewDB");
}

MBD::~MBD( )
{

}

//Override the default notice processor with the empty one. This is done to avoid the notice messages printing on stderr
void MyNoticeProcessor( void *arg, const char *message )	{ }

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag&NodeRemove && owner().fullDeleteDB()) {
	MtxAlloc resource(connRes, true);
	PGconn *tcon = NULL;
	PGresult *res;
	try {
	    if((tcon=PQconnectdb((conninfo+"dbname=template1").c_str())) == NULL)
		throw err_sys(_("Fatal error - unable to allocate connection."));
	    if(PQstatus(tcon) != CONNECTION_OK)
		throw err_sys(_("Error connecting the DB: %s"), PQerrorMessage(tcon));
	    string req = "DROP DATABASE \"" + db + "\"";
	    if((res=PQexec(tcon,req.c_str())) == NULL)
		throw err_sys(_("Error connecting the DB: %s"), PQerrorMessage(tcon));
	    if(PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
		string err, err1;
		err = PQresStatus(PQresultStatus(res));
		err1 = PQresultErrorMessage(res);
		PQclear(res);
		throw err_sys(_("Error querying the DB: '%s (%s)'!"), err1.c_str(), err.c_str());
	    }
	    else PQclear(res);
	    PQfinish(tcon);
	} catch(...) {
	    if(tcon) PQfinish(tcon);
	    throw;
	}
    }
}

void MBD::enable( )
{
    MtxAlloc resource(connRes, true);
    //Reconnecting
    if(enableStat()) {
	PQfinish(connection);
	mEn = false;
	//return;
    }

    int off = 0;
    host = sTrm(TSYS::strParse(addr(),0,";",&off));
    hostaddr = sTrm(TSYS::strParse(addr(),0,";",&off));
    user = sTrm(TSYS::strParse(addr(),0,";",&off));
    pass = sTrm(TSYS::strParse(addr(),0,";",&off));
    db   = sTrm(TSYS::strParse(addr(),0,";",&off));
    port = sTrm(TSYS::strParse(addr(),0,";",&off));
    connect_timeout = (off < (int)addr().size()) ? sTrm(TSYS::strParse(addr(),0,";",&off)) : "1";

    conninfo.clear();
    if(host.empty() && hostaddr.empty()) host = "localhost";
    if(host.size()) conninfo += "host=" + host + " ";
    if(hostaddr.size()) conninfo += "hostaddr=" + hostaddr + " ";
    if(port.size()) conninfo += "port=" + port + " ";
    if(pass.size()) conninfo += "password=" + pass + " ";
    if(connect_timeout.size()) conninfo += "connect_timeout=" + connect_timeout + " ";
    if(user.size()) conninfo += "user=" + user + " ";
    cd_pg  = codePage().size() ? codePage() : Mess->charset();
    try {
	bool dbCreateTry = false;
nextTry:
	if((connection=PQconnectdb((conninfo+"dbname="+db).c_str())) == NULL)
	    throw err_sys(_("Fatal error - unable to allocate connection."));
	if(PQstatus(connection) != CONNECTION_OK) {
	    if(dbCreateTry) throw err_sys(_("Error connecting the DB: %s"), PQerrorMessage(connection));
	    //Try for connect to system DB, check for need DB present and create otherwise
	    PQfinish(connection);
	    if((connection=PQconnectdb((conninfo+"dbname=template1").c_str())) == NULL)
		throw err_sys(_("Fatal error - unable to allocate connection."));
	    if(PQstatus(connection) != CONNECTION_OK)
		throw err_sys(_("Error connecting the DB: %s"), PQerrorMessage(connection));
	    TBD::enable();

	    vector< vector<string> > tbl;
	    sqlReq("SELECT count(*) FROM pg_catalog.pg_database WHERE datname = '"+db+"'", &tbl);
	    if(tbl.size() == 2 && tbl[1][0] == "0")
		sqlReq("CREATE DATABASE \""+TSYS::strEncode(db,TSYS::SQL,"\"")+"\" ENCODING = '"+cd_pg+"'");
	    PQfinish(connection);

	    dbCreateTry = true;
	    goto nextTry;
	}
	PQsetNoticeProcessor(connection, MyNoticeProcessor, NULL);
	if(!dbCreateTry) TBD::enable();
    } catch(...) {
	if(connection) PQfinish(connection);
	TBD::disable();
	throw;
    }

    nReq = rqTm = rqTmMax = rqTmAll = 0;
    rqTmMaxVl = "";
    rqTmMin = 3600;
    conTm = time(NULL);
}

void MBD::disable( )
{
    MtxAlloc resource(connRes, true);

    if(!enableStat())  return;

    //Last commit
    if(reqCnt) try{ transCommit(); } catch(...) { }

    TBD::disable();

    PQfinish(connection);
}

void MBD::allowList( vector<string> &list ) const
{
    if(!enableStat())  return;
    list.clear();
    string req = "SELECT c.relname as \"TableName\" "
		 "FROM pg_catalog.pg_class c "
		    "JOIN pg_catalog.pg_roles r ON r.oid = c.relowner "
		    "LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace "
		 "WHERE c.relkind IN ('r','v','S','') "
		    "AND n.nspname <> 'pg_catalog' "
		    "AND n.nspname !~ '^pg_toast' "
		    "AND pg_catalog.pg_table_is_visible(c.oid)";
    vector< vector<string> > tbl;
    const_cast<MBD*>(this)->sqlReq(req, &tbl/*, false*/);
    for(unsigned iT = 1; iT < tbl.size(); iT++) list.push_back(tbl[iT][0]);
}

TTable *MBD::openTable( const string &inm, bool icreate )
{
    if(!enableStat()) throw err_sys(_("Error opening the table '%s': the DB is disabled."), inm.c_str());

    create(inm, icreate);
    vector<TTable::TStrIt> tblStrct;
    getStructDB(inm, tblStrct);

    return new MTable(inm, this, &tblStrct);
}

void MBD::create( const string &nm, bool toCreate )
{
    /*vector< vector<string> > tbl;
    sqlReq("SELECT count(*) "
	  "FROM pg_catalog.pg_class c "
	    "JOIN pg_catalog.pg_roles r ON r.oid = c.relowner "
	    "LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace "
	  "WHERE c.relkind IN ('r','v','S','') "
	    "AND n.nspname <> 'pg_catalog' "
	    "AND n.nspname !~ '^pg_toast' "
	    "AND pg_catalog.pg_table_is_visible(c.oid) "
	    "AND c.relname = '" + TSYS::strEncode(nm,TSYS::SQL,"'") + "'", &tbl);
    if(toCreate && tbl.size() == 2 && tbl[1][0] == "0")
	sqlReq("CREATE TABLE \"" + TSYS::strEncode(nm,TSYS::SQL,"\"")+ "\"(\"<<empty>>\" character(20) NOT NULL DEFAULT '' PRIMARY KEY)");*/

    try { sqlReq("SELECT * FROM \"" + TSYS::strEncode(nm,TSYS::SQL,"\"") + "\" LIMIT 0;"); }
    catch(...) {
	if(!toCreate) throw;
	sqlReq("CREATE TABLE \"" + TSYS::strEncode(nm,TSYS::SQL,"\"")+ "\"(\"<<empty>>\" character(20) NOT NULL DEFAULT '' PRIMARY KEY)");
    }
}

void MBD::getStructDB( const string &nm, vector<TTable::TStrIt> &tblStrct )
{
    vector< vector<string> > origTblStrct, keyLst;

    //Getting the generic data structure
    sqlReq("SELECT a.attname as \"Field\", pg_catalog.format_type(a.atttypid, a.atttypmod) as \"Type\" "
	"FROM pg_catalog.pg_attribute a "
	"WHERE a.attnum > 0 "
	"AND NOT a.attisdropped "
	"AND a.attrelid = "
	"( "
	    "SELECT c.oid "
	    "FROM pg_catalog.pg_class c "
	    "LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace "
	    "WHERE c.relname ~ '^(" + TSYS::strEncode(nm,TSYS::SQL,"'") + ")$' "
	    "AND pg_catalog.pg_table_is_visible(c.oid) "
	")", &origTblStrct, false);

    tblStrct.clear();
    if(origTblStrct.size()) {
	//Getting the keys
	sqlReq("SELECT a.attname "
	    "FROM pg_class c, pg_class c2, pg_index i, pg_attribute a "
	    "WHERE c.relname = '" + TSYS::strEncode(nm,TSYS::SQL,"'") + "' AND c.oid = i.indrelid AND i.indexrelid = c2.oid "
	    "AND i.indisprimary AND i.indisunique "
	    "AND a.attrelid=c2.oid "
	    "AND a.attnum>0;", &keyLst, false);

	//Filling the structure
	for(unsigned iReq = 1; iReq < origTblStrct.size(); ++iReq) {
	    bool isKey = false;
	    for(unsigned iKey = 0; iKey < keyLst.size() && !isKey; ++iKey)
		isKey = (origTblStrct[iReq][0] == keyLst[iKey][0]);

	    tblStrct.push_back(TTable::TStrIt(origTblStrct[iReq][0],origTblStrct[iReq][1],isKey));
	}
    } else throw err_sys(_("The table '%s' is not present!"), nm.c_str());
}

void MBD::transOpen( )
{
    //Check for limit into one trinsaction
    if(reqCnt > 1000) transCommit();

#if 1
    connRes.lock();
    bool begin = !reqCnt;
    if(begin) trOpenTm = TSYS::curTime();
    reqCnt++;
    reqCntTm = TSYS::curTime();
    connRes.unlock();

    if(begin) sqlReq("BEGIN;");

#else
    MtxAlloc resource(connRes, true);
    PGTransactionStatusType tp;
    tp = PQtransactionStatus(connection);

    if(tp == PQTRANS_IDLE/*!(tp == PQTRANS_INTRANS || tp == PQTRANS_INERROR)*/) {
	PGresult *res;
	res = PQexec(connection, "BEGIN");
	if(!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
	    PQclear(res);
	    mess_sys(TMess::Warning, _("Error starting a transaction!"));
	    return;
	    //throw err_sys(_("Error starting a transaction!"));
	}
	PQclear(res);
	trOpenTm = TSYS::curTime();
    }
    reqCnt++;
    reqCntTm = TSYS::curTime();
#endif
}

void MBD::transCommit( )
{
#if 1
    connRes.lock();
    bool commit = reqCnt;
    reqCnt = reqCntTm = 0;
    connRes.unlock();

    if(commit) sqlReq("COMMIT;");
#else
    MtxAlloc resource(connRes, true);
    PGTransactionStatusType tp;
    tp = PQtransactionStatus(connection);

    if(tp != PQTRANS_IDLE) {
	PGresult *res;
	res = PQexec(connection, "COMMIT");
	if(!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
	    PQclear(res);
	    mess_sys(TMess::Warning, _("Error stopping a transaction!"));
	    return;
	    //throw err_sys(_("Error stopping a transaction!"));
	}
	PQclear(res);
    }
    reqCnt = reqCntTm = 0;
#endif
}

void MBD::transCloseCheck( )
{
    if(!enableStat() && toEnable()) enable();
    if(reqCnt && ((TSYS::curTime()-reqCntTm) > 1e6*trTm_ClsOnReq() || (TSYS::curTime()-trOpenTm) > 1e6*trTm_ClsOnOpen()))
	transCommit();
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    PGresult *res;

    if(!enableStat()) return;
    if(tbl) tbl->clear();

    string req = Mess->codeConvOut(cd_pg.c_str(), ireq);

    MtxAlloc resource(connRes, true);	//!! Moved before the transaction checking for prevent the "BEGIN;" and "COMMIT;"
					//   request's sequence breakage on high concurrency access activity

    if(reqCnt && PQtransactionStatus(connection) == PQTRANS_INERROR) transCommit();	//Close error transaction
    if(intoTrans && intoTrans != EVAL_BOOL)	transOpen();
    else if(!intoTrans && reqCnt)		transCommit();

    int repCnt = 0;
rep:
    int64_t tmBeg = SYS->curTime();
    if((res=PQexec(connection,req.c_str())) == NULL) {
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("ERR CON for: %s"), ireq.c_str());
	throw err_sys(TError::DB_SQL_Connect, _("Error connecting the DB: %s"), PQerrorMessage(connection));
    }
    if(PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
	string  err = PQresStatus(PQresultStatus(res)),
		err1 = PQresultErrorMessage(res);
	PQclear(res);

	if(PQstatus(connection) != CONNECTION_OK) {
	    //Try to reconnect
	    if((repCnt++) < 3)
		try { enable(); goto rep; } catch(TError&) { }
	    else mess_warning(nodePath().c_str(), _("Repeated errors of requesting the DB: '%s (%s)'."), err1.c_str(), err.c_str());

	    //resource.unlock();
	    disable();
	    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("ERR CON_st for: %s"), ireq.c_str());
	    throw err_sys(TError::DB_SQL_Connect, _("Error connecting the DB: '%s (%s)'!"), err1.c_str(), err.c_str());
	}
	if(mess_lev() == TMess::Debug)
	    mess_sys(TMess::Debug, _("ERR QUERY (con=%d;res=%d;tr=%d) for: %s"),
		PQstatus(connection), PQresultStatus(res), PQtransactionStatus(connection), ireq.c_str());
	throw err_sys(TError::DB_SQL_Query, _("Error querying the DB: '%s (%s)'!"), err1.c_str(), err.c_str());
    }
    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("OK (tr=%d) for: %s"), PQtransactionStatus(connection), ireq.c_str());

    if(tbl) {
	int num_fields = PQnfields(res);
	int row;
	vector<string> fld;
	//Add head
	for(int i = 0; i < num_fields; i++) fld.push_back(PQfname(res,i));
	tbl->push_back(fld);
	//Add data
	for(row = 0; row < PQntuples(res); row++) {
	    fld.clear();
	    for(int i = 0; i < num_fields; i++) {
		if(PQgetisnull(res,row,i)) fld.push_back(DB_NULL);
		//Clear spaces at the end of the string
		else if(PQftype(res,i) == 1042) {
		    string tmp_val = PQgetvalue(res, row, i);
		    char c;
		    int len = tmp_val.size(), spCol = 0, n = len;
		    while((c=tmp_val[n-1]) == ' ') { n--; spCol++; }
		    int len_new = len - spCol;
		    fld.push_back(Mess->codeConvIn(cd_pg.c_str(),tmp_val.substr(0,len_new)));
		}
		else fld.push_back(Mess->codeConvIn(cd_pg.c_str(),PQgetvalue(res,row,i)));
	    }
	    tbl->push_back(fld);
	}
    }
    PQclear(res);

    //Statistic update
    nReq++;
    rqTm = 1e-6*(SYS->curTime()-tmBeg); rqTmAll += rqTm;
    if(rqTm > rqTmMax) rqTmMaxVl = req;
    rqTmMax = vmax(rqTmMax, rqTm); rqTmMin = vmin(rqTmMin, rqTm);
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,0,"/prm/st/status",_("Status"),R_R_R_,"root",SDB_ID,1, "tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,"help",
	    _("PostgreSQL DBMS address must be written as: \"{host};{hostaddr};{user};{pass};{db}[;{port}[;{connect_timeout}]]\".\n"
	      "Where:\n"
	      "  host - hostname on which the DBMS server PostgreSQL works. If this begins with a slash ('/'),\n"
	      "         it specifies Unix socket rather than TCP/IP communication;\n"
	      "         the value is the name of the directory in which the socket file is stored;\n"
	      "  hostaddr - numeric IP address of the host for connecting;\n"
	      "  user - user name of the DB;\n"
	      "  pass - password of the user for accessing the DB;\n"
	      "  db   - name of the DB;\n"
	      "  port - port, which listening by the DBMS server (default 5432);\n"
	      "  connect_timeout - connection timeout, in seconds;\n"
	      "For local DBMS: \";;user;password;OpenSCADA;5432;10\".\n"
	      "For remote DBMS: \"server.nm.org;;user;password;OpenSCADA;5432;10\"."));
	return;
    }

    //Get page's info
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/status" && ctrChkNode(opt)) {
	MtxAlloc resource(connRes, true);
	opt->setText((enableStat()?_("Enabled. "):_("Disabled. ")) +
	    TSYS::strMess(_("Connected: %s. "),atm2s(conTm,"%d-%m-%Y %H:%M:%S").c_str()) +
	    (enableStat()?TSYS::strMess(_("Requests: %g; Request time: %s[%s,%s,%s]; Max time request: '%s'"),nReq,
			tm2s(rqTm).c_str(),tm2s(rqTmMin).c_str(),tm2s(nReq?(rqTmAll/nReq):0).c_str(),tm2s(rqTmMax).c_str(),rqTmMaxVl.getVal().c_str()):""));
    }
    else TBD::cntrCmdProc(opt);
}

//************************************************
//* BDPostgreSQL::Table                          *
//************************************************
MTable::MTable( string name, MBD *iown, vector<TStrIt> *itblStrct ) : TTable(name)
{
    setNodePrev(iown);

    try {
	if(itblStrct) tblStrct = *itblStrct;
	else owner().getStructDB(name, tblStrct);
    } catch(...) { }
}

MTable::~MTable( )	{ }

bool MTable::isEmpty( )	{ return tblStrct.empty() || tblStrct[0].nm == "<<empty>>"; }

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag&NodeRemove) {
	try { owner().sqlReq("DROP TABLE \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"\""); }
	catch(TError &err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }
    }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();
    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	int pr1;
	string	sid = tblStrct[iFld].nm, rowTp = tblStrct[iFld].tp;
	if(cfg.cfgPresent(sid)) continue;
	int flg = tblStrct[iFld].key ? (int)TCfg::Key : (int)TFld::NoFlag;
	if(rowTp == "text" || rowTp == "character varying")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(sscanf(rowTp.c_str(),"character(%d)",&pr1) || sscanf(rowTp.c_str(),"character varying(%d)",&pr1))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,i2s(pr1).c_str()));
	else if(rowTp == "smallint" || rowTp == "integer" || rowTp == "bigint")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(rowTp == "real" || rowTp == "double precision")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
	else if(rowTp.find("timestamp") == 0)
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg|TFld::DateTimeDec,"10"));
    }
}

void MTable::fieldFix_( TConfig &cfg, const string &ilangLs, bool recurse )
{
    //if(tblStrct.empty()) throw err_sys(_("Table is empty."));

    bool appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !isEmpty());	//Only for append no present fields

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    string pr_keys, ls, langLs;

    //DROP fields
    for(unsigned iFld = 0, iCf; iFld < tblStrct.size() && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld].nm ||
		// Pass all the column translation
		((cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText) &&
		    tblStrct[iFld].nm.size() > 3 && tblStrct[iFld].nm.substr(2) == ("#"+cf_el[iCf])))
	    {
		TCfg &cf = cfg.cfg(cf_el[iCf]);
		bool isEqual = false;
		string rwTp = tblStrct[iFld].tp;
		switch(cf.fld().type()) {
		    case TFld::String: {
			int pr1 = -1;
			if(rwTp == "text" || rwTp == "character varying") isEqual = true;
			else if((sscanf(rwTp.c_str(),"character(%d)",&pr1) || sscanf(rwTp.c_str(),"character varying(%d)",&pr1)) &&
				(cf.fld().len() <= 255 || cf.fld().flg()&TCfg::Key) && pr1 > 0)
			    isEqual = true;
			break;
		    }
		    case TFld::Integer:
			if(cf.fld().flg()&TFld::DateTimeDec)	isEqual = (rwTp.find("timestamp") == 0);
			else if(rwTp == "bigint") isEqual = true;
			break;
		    case TFld::Real:	if(rwTp == "double precision") isEqual = true;	break;
		    case TFld::Boolean:	if(rwTp == "smallint") isEqual = true;		break;
		    default: break;
		}
		if(isEqual) break;
		continue;
	    }
	if(iCf >= cf_el.size()) {
	    ls += (ls.size()?", DROP \"":" DROP \"") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL,"\"") + "\" ";
	    tblStrct.erase(tblStrct.begin()+iFld);
	    iFld--;
	}
    }

    //Add fields
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	langLs = ilangLs;
	TCfg &cf = cfg.cfg(cf_el[iCf]);
	// Check primary key
	if(cf.fld().flg()&TCfg::Key && !appMode)
	    pr_keys += (pr_keys.size()?",\"":"\"") + TSYS::strEncode(cf.name(),TSYS::SQL,"\"") + "\"";

	for(iFld = 0; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld].nm) break;

	string f_tp;
	switch(cf.fld().type()) {
	    case TFld::String:
		if((cf.fld().len() && cf.fld().len() < 256) || cf.fld().flg()&TCfg::Key)
		    f_tp = "CHARACTER VARYING(" + i2s(vmax(10,vmin(255,cf.fld().len()))) + ") ";
		else f_tp = "TEXT ";
		f_tp += "DEFAULT " + ((cf.fld().def()==EVAL_STR)?"NULL ":"'"+cf.fld().def()+"' ");
		break;
	    case TFld::Integer:
		if(cf.fld().flg()&TFld::DateTimeDec)
		    f_tp = "TIMESTAMP DEFAULT " + ((s2ll(cf.fld().def())==EVAL_INT)?"NULL ":"'"+UTCtoSQL(s2i(cf.fld().def()))+"' ");
		else f_tp = "BIGINT DEFAULT " + ((s2ll(cf.fld().def())==EVAL_INT)?"NULL ":"'"+ll2s(s2ll(cf.fld().def()))+"' ");
		break;
	    case TFld::Real:
		f_tp = "DOUBLE PRECISION DEFAULT " + ((s2r(cf.fld().def())==EVAL_REAL)?"NULL ":"'"+r2s(s2r(cf.fld().def()))+"' ");
		break;
	    case TFld::Boolean:
		f_tp = "SMALLINT DEFAULT " + ((s2i(cf.fld().def())==EVAL_BOOL)?"NULL ":"'"+i2s(s2i(cf.fld().def()))+"' ");
		break;
	    default: break;
	}

	// Add field
	if(iFld >= tblStrct.size())
	    ls += (ls.size()?", ADD \"":" ADD \"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\" " + f_tp;
	//Check other languages - append translation for new languages
	if(cf.fld().flg()&TFld::TransltText && langLs.size()) {
	    size_t pos = 0;
	    for(unsigned iC = iFld; iC < tblStrct.size(); iC++)
		if(tblStrct[iC].nm.size() > 3 && tblStrct[iC].nm.substr(2) == ("#"+cf_el[iCf]) &&
			(pos=langLs.find(tblStrct[iC].nm.substr(0,2)+";")) != string::npos)
		    langLs.replace(pos, 3, "");

	    string toLang;
	    for(int off = 0; (toLang=TSYS::strParse(langLs,0,";",&off)).size(); )
		ls += (ls.size()?", ADD \"":" ADD \"") + TSYS::strEncode(toLang+"#"+cf_el[iCf],TSYS::SQL,"\"") + "\" " + f_tp;
	}
    }

    if(ls.size()) {
	try {
	    //Prepare request for fix structure
	    string req = "ALTER TABLE \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" " +
		(appMode?"":"DROP CONSTRAINT \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"_pkey\", ") + ls +
		(pr_keys.size()?", ADD PRIMARY KEY ("+pr_keys+")":"");

	    owner().sqlReq(req, NULL, false);
	    owner().getStructDB(name(), tblStrct);	//Update the table structure information
	}
	//Drop unfixable table
	catch(TError &err) {
	    if(err.cod == TError::DB_SQL_Connect || recurse) throw;
	    owner().sqlReq("DROP TABLE \"" + TSYS::strEncode(name(),TSYS::SQL,"\"")+ "\"");
	    owner().create(name(), true);
	    owner().getStructDB(name(), tblStrct);	//Update the table structure information
	    fieldFix_(cfg, ilangLs, true);
	}
    }
}

string MTable::getSQLVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	//if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->langCode());
	rez = "'" + TSYS::strEncode(rez /*((cfg.fld().len()>0)?rez.substr(0,cfg.fld().len()):rez)*/, TSYS::SQL, "'") + "'";
    }
    else if(cfg.fld().flg()&TFld::DateTimeDec) rez = "'" + UTCtoSQL(s2i(rez)) + "'";

    return rez;
}

void MTable::setSQLVal( TCfg &cf, const string &ival, bool tr )
{
    string val = (ival==DB_NULL) ? EVAL_STR : ival;
    switch(cf.fld().type()) {
	case TFld::Integer:
	    if(cf.fld().flg()&TFld::DateTimeDec) cf.setI(SQLtoUTC(val));
	    else cf.setS(val);
	    break;
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

string MTable::UTCtoSQL( time_t val )
{
    char buf[255];
    struct tm tm_tm;

    //localtime_r(&val,&tm_tm);
    gmtime_r(&val, &tm_tm);
    int rez = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_tm);

    return (rez>0) ? string(buf,rez) : "";
}

time_t MTable::SQLtoUTC( const string &val )
{
    struct tm stm;
    strptime(val.c_str(), "%Y-%m-%d %H:%M:%S", &stm);

    //return mktime(&stm);
    return timegm(&stm);
}

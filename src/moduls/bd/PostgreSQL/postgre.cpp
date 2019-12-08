// 
//OpenSCADA module BD.PostgreSQL file: postgre.cpp
/***************************************************************************
 *   Copyright (C) 2013-2019 by Roman Savochenko, rom_as@oscada.org        *
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
#define MOD_NAME	_("DB PostgreSQL")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"2.3.0"
#define AUTHORS		_("Roman Savochenko, Maxim Lysenko (2010-2011)")
#define DESCRIPTION	_("DB module. Provides support of the DBMS PostgreSQL.")
#define MOD_LICENSE	"GPL2"
//************************************************

#define SEEK_PRELOAD_LIM	100

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

TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }

//************************************************
//* BDPostgreSQL::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0), connRes(true),
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

    if(flag && owner().fullDeleteDB()) {
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
    vector< vector<string> > tblStrct;
    getStructDB(inm, tblStrct);

    return new MTable(inm, this, &tblStrct);
}

void MBD::create( const string &nm, bool toCreate )
{
    vector< vector<string> > tbl;
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
	sqlReq("CREATE TABLE \"" + TSYS::strEncode(nm,TSYS::SQL,"\"")+ "\"(\"<<empty>>\" character(20) NOT NULL DEFAULT '' PRIMARY KEY)");
}

void MBD::getStructDB( const string &nm, vector< vector<string> > &tblStrct )
{
    //Get generic data structure
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
	")", &tblStrct, false);
    if(tblStrct.size() > 1) {
	//Get keys
	vector< vector<string> > keyLst;
	sqlReq("SELECT a.attname "
	    "FROM pg_class c, pg_class c2, pg_index i, pg_attribute a "
	    "WHERE c.relname = '" + TSYS::strEncode(nm,TSYS::SQL,"'") + "' AND c.oid = i.indrelid AND i.indexrelid = c2.oid "
	    "AND i.indisprimary AND i.indisunique "
	    "AND a.attrelid=c2.oid "
	    "AND a.attnum>0;", &keyLst, false);
	tblStrct[0].push_back("Key");
	for(unsigned i_f = 1, i_k; i_f < tblStrct.size(); i_f++) {
	    for(i_k = 1; i_k < keyLst.size(); i_k++)
	        if(tblStrct[i_f][0] == keyLst[i_k][0]) break;
	    tblStrct[i_f].push_back((i_k<keyLst.size())?"PRI":"");
	}
    }
    else throw err_sys(_("The table '%s' is not present!"), nm.c_str());
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

    if(tbl) tbl->clear();
    if(!enableStat()) return;

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
	throw err_sys(SQL_CONN, _("Error connecting the DB: %s"), PQerrorMessage(connection));
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
	    throw err_sys(SQL_CONN, _("Error connecting the DB: '%s (%s)'!"), err1.c_str(), err.c_str());
	}
	if(mess_lev() == TMess::Debug)
	    mess_sys(TMess::Debug, _("ERR QUERY (con=%d;res=%d;tr=%d) for: %s"),
		PQstatus(connection), PQresultStatus(res), PQtransactionStatus(connection), ireq.c_str());
	throw err_sys(SQL_QUERY, _("Error querying the DB: '%s (%s)'!"), err1.c_str(), err.c_str());
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
MTable::MTable( string iname, MBD *iown, vector< vector<string> > *itblStrct ) : TTable(iname)
{
    setNodePrev(iown);

    try {
	if(itblStrct) tblStrct = *itblStrct;
	else owner().getStructDB(name(), tblStrct);
    } catch(...) { }
}

MTable::~MTable( )	{ }



bool MTable::isEmpty( )	{ return tblStrct.empty() || tblStrct[1][0] == "<<empty>>"; }

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag) {
	try { owner().sqlReq("DROP TABLE \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"\""); }
	catch(TError &err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }
    }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
    mLstUse = SYS->sysTm();
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	int pr1;
	string	sid = tblStrct[iFld][0], rowTp = tblStrct[iFld][1];
	if(cfg.cfgPresent(sid)) continue;
	int flg = (tblStrct[iFld][2]=="PRI") ? (int)TCfg::Key : (int)TFld::NoFlag;
	if(rowTp == "text" || rowTp == "character varying")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(sscanf(rowTp.c_str(),"character(%d)",&pr1) || sscanf(rowTp.c_str(),"character varying(%d)",&pr1))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,i2s(pr1).c_str()));
	else if(rowTp == "smallint" || rowTp == "integer" || rowTp == "bigint")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(rowTp == "real" || rowTp == "double precision")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
	else if(rowTp == "timestamp with time zone")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg|TFld::DateTimeDec,"10"));
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full )
{
    vector< vector<string> >	inTbl,
				&tbl = full ? *full : inTbl;

    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
    mLstUse = SYS->sysTm();

    cfg.cfgToDefault();	//reset the not key and viewed fields

    //Check for no present and no empty keys allow
    if(row == 0) {
	vector<string> cf_el;
	cfg.cfgList(cf_el);
	for(unsigned iC = 0; iC < cf_el.size(); iC++) {
	    TCfg &cf = cfg.cfg(cf_el[iC]);
	    if(!cf.isKey() || !cf.getS().size()) continue;
	    unsigned iFld = 1;
	    for( ; iFld < tblStrct.size(); iFld++)
		if(cf.name() == tblStrct[iFld][0]) break;
	    if(iFld >= tblStrct.size()) return false;
	}
    }

    string sid;
    //Make SELECT and WHERE
    string req_sel = "", req_where = "WHERE ";
    //Add use keys to list
    bool first_sel = true, next = false, trPresent = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TFld::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?"AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg) + " ";
	    next = true;
	}
	else if(u_cfg->isKey() || u_cfg->view()) {
	    req_sel += (first_sel?"\"":",\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"";
	    first_sel = false;
	}
    }

    //Request
    if(!full || !full->size() || (row%SEEK_PRELOAD_LIM) == 0) {
	if(first_sel) return false;
	string req = "SELECT "+req_sel+" FROM \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"\" "+(next?req_where:"")+" ORDER BY "+req_sel;
	if(!full) req += " LIMIT 1 OFFSET " + i2s(row);
	else req += " LIMIT " + i2s(SEEK_PRELOAD_LIM) +" OFFSET "+i2s((row/SEEK_PRELOAD_LIM)*SEEK_PRELOAD_LIM);

	tbl.clear();
	owner().sqlReq(req, &tbl, false);
    }

    row = full ? (row%SEEK_PRELOAD_LIM)+1 : 1;
    if(tbl.size() < 2 || (full && row >= tbl.size())) return false;

    //Processing of the query
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

    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
    mLstUse = SYS->sysTm();
    string sid;
    //Prepare request
    string req = "SELECT ";
    string req_where, first_key;
    // Add fields list to queue
    bool first_sel = true, next_wr = false, trPresent = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TFld::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey()) {
	    req_where += (next_wr?"AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" +getVal(*u_cfg) + " ";
	    if(first_key.empty()) first_key = TSYS::strEncode(sid,TSYS::SQL,"\"");
	    next_wr = true;
	}
	else if(u_cfg->view()) {
	    req += (first_sel?"\"":",\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"";
	    first_sel = false;
	}
    }
    if(first_sel) req += "\""+first_key+"\"";
    req += " FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" WHERE " + req_where;

    //Query
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) throw err_sys(_("The row \"%s\" is not present. Are you saved the object?"), req_where.c_str());

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

    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
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
	sid = tblStrct[iFld][0];
	if(sid.size() > 3) {
	    if(!trPresent && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) trPresent = true;
	    if(Mess->lang2Code() == Mess->lang2CodeBase() && !trDblDef && sid.compare(0,3,Mess->lang2CodeBase()+"#") == 0)
		trDblDef = true;
	}
    }
    if(trDblDef && !cfg.reqKeys()) fieldFix(cfg, trPresent);

    //Get present fields list
    string req_where = "WHERE ";
    // Add key list to query
    bool next = false, noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();		//Force update by ReqKeys or reqKey() present
    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if(!u_cfg.isKey()) continue;
	req_where += (next?"AND \"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL,"\"") + "\"=" + getVal(u_cfg,TCfg::ExtValTwo) + " ";
	next = true;

	if(!isForceUpdt && u_cfg.extVal()) isForceUpdt = true;

	// Check for no key fields
	if(noKeyFld) continue;
	unsigned iFld = 1;
	for( ; iFld < tblStrct.size(); iFld++)
	    if(u_cfg.name() == tblStrct[iFld][0]) break;
	if(iFld >= tblStrct.size()) noKeyFld = true;
    }
    if(noKeyFld) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg, trPresent);
    }

    //Prepare query
    // Try for get already present field
    string req;
    if(!isForceUpdt) {
	req = "SELECT 1 FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" " + req_where;
	try { owner().sqlReq(req, &tbl, true); }
	catch(TError &err) { fieldFix(cfg, trPresent); owner().sqlReq(req, &tbl, true); }
	if(tbl.size() < 2) {
	    // Add new record
	    req = "INSERT INTO \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" ";
	    string ins_name, ins_value;
	    next = false;
	    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
		TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
		if(!u_cfg.isKey() && !u_cfg.view()) continue;

		bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && trPresent && !u_cfg.noTransl());
		ins_name = ins_name + (next?",\"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL,"\"") + "\" " +
			(isTransl ? (",\""+TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[i_el],TSYS::SQL,"\"")+"\" ") : "");
		sval = getVal(u_cfg);
		ins_value = ins_value + (next?",":"") + sval + " " + (isTransl?(","+sval+" "):"");	//???? E'{Val}'
		next = true;
	    }
	    req += "(" + ins_name + ") VALUES (" + ins_value + ")";
	}
	else isForceUpdt = true;
    }
    if(isForceUpdt) {
	// Update present record
	req = "UPDATE \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" SET ";
	next = false;
	for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if((u_cfg.isKey() && !u_cfg.extVal()) || !u_cfg.view()) continue;
	    bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    sval = getVal(u_cfg);
	    req += (next?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + sval + " ";	//???? E'{Val}'
	    next = true;
	}
	req += req_where;
    }

    //Query
    try{ owner().sqlReq(req, NULL, true); }
    catch(TError &err) { fieldFix(cfg, trPresent); owner().sqlReq(req, NULL, true); }
}

void MTable::fieldDel( TConfig &cfg )
{
    if(tblStrct.empty()) return;
    mLstUse = SYS->sysTm();

    //Where prepare
    string req_where = "WHERE ";
    bool next = false;
    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?"AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getVal(*u_cfg) + " ";
	    next = true;
	}
    }

    //Main request
    try { owner().sqlReq("DELETE FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" "+req_where, NULL, true); }
    catch(TError &err) {
	//Check for present
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"\" "+req_where, &tbl, false);
	if(tbl.size() < 2) return;
	throw;
    }
}

void MTable::fieldFix( TConfig &cfg, bool trPresent, bool recurse )
{
    bool next = false, next_key = false;
    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));

    bool appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !isEmpty()),	//Only for append no present fields
	 isVarTextTransl = trPresent || (!Mess->lang2CodeBase().empty() && Mess->lang2Code() != Mess->lang2CodeBase());

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Prepare request for fix structure
    string req = "ALTER TABLE \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" ";
    if(!appMode) req += "DROP CONSTRAINT \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "_pkey\", ";

    //DROP fields
    for(unsigned iFld = 1, iCf; iFld < tblStrct.size() && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld][0] ||
		((cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText) && !cfg.cfg(cf_el[iCf]).noTransl() &&
		tblStrct[iFld][0].size() > 3 && tblStrct[iFld][0].substr(2) == ("#"+cf_el[iCf]) && tblStrct[iFld][0].compare(0,2,Mess->lang2CodeBase()) != 0))
	    {
		TCfg &cf = cfg.cfg(cf_el[iCf]);
		bool isEqual = false;
		string rwTp = tblStrct[iFld][1];
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
			if(cf.fld().flg()&TFld::DateTimeDec)	isEqual = (rwTp=="timestamp with time zone");
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
	    req += (next?",DROP \"":"DROP \"") + TSYS::strEncode(tblStrct[iFld][0],TSYS::SQL,"\"") + "\" ";
	    tblStrct.erase(tblStrct.begin()+iFld);
	    iFld--;
	    next = true;
	}
    }

    string pr_keys;
    //Add fields
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	TCfg &cf = cfg.cfg(cf_el[iCf]);
	// Check primary key
	if(cf.fld().flg()&TCfg::Key && !appMode) {
	    pr_keys += (next_key?",\"":"\"") + TSYS::strEncode(cf.name(),TSYS::SQL,"\"") + "\"";
	    next_key = true;
	}

	for(iFld = 1; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld][0]) break;

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
		    f_tp = "TIMESTAMP WITH TIME ZONE DEFAULT " + ((s2ll(cf.fld().def())==EVAL_INT)?"NULL ":"'"+UTCtoSQL(s2i(cf.fld().def()))+"' ");
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
	if(iFld >= tblStrct.size()) {
	    req += (next?",ADD \"":"ADD \"") + TSYS::strEncode(cf_el[iCf],TSYS::SQL,"\"") + "\" " + f_tp;
	    next = true;
	}
	//Check other languages
	if((cf.fld().flg()&TFld::TransltText) && !cf.noTransl()) {
	    unsigned iC;
	    for(iC = iFld; iC < tblStrct.size(); iC++)
		if(tblStrct[iC][0].size() > 3 && tblStrct[iC][0].substr(2) == ("#"+cf_el[iCf]) &&
		   tblStrct[iC][0].compare(0,2,Mess->lang2CodeBase()) != 0 &&
		   tblStrct[iC][0].compare(0,2,Mess->lang2Code()) == 0) break;
	    if(iC >= tblStrct.size() && isVarTextTransl) {
		req += (next?",ADD \"":"ADD \"") + TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[iCf],TSYS::SQL,"\"") + "\" " + f_tp;
		next = true;
	    }
	}
    }
    if(pr_keys.size()) req += ",ADD PRIMARY KEY (" + pr_keys + ") ";

    if(next) {
	try {
	    owner().sqlReq(req, NULL, false);
	    owner().getStructDB(name(), tblStrct);	//Update the table structure information
	}
	//Drop unfixable table
	catch(TError &err) {
	    if(err.cod == MBD::SQL_CONN || recurse) throw;
	    owner().sqlReq("DROP TABLE \"" + TSYS::strEncode(name(),TSYS::SQL,"\"")+ "\"");
	    owner().create(name(), true);
	    owner().getStructDB(name(), tblStrct);
	    fieldFix(cfg, trPresent, true);
	}
    }
}

string MTable::getVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->lang2Code());
	rez = "'" + TSYS::strEncode(((cfg.fld().len()>0)?rez.substr(0,cfg.fld().len()):rez), TSYS::SQL, "'") + "'";
    }
    else if(cfg.fld().flg()&TFld::DateTimeDec) rez = "'" + UTCtoSQL(s2i(rez)) + "'";

    return rez;
}

void MTable::setVal( TCfg &cf, const string &ival, bool tr )
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

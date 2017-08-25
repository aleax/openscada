
//OpenSCADA system module BD.MySQL file: my_sql.cpp
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

#include <string>
#include <mysql.h>
#include <errmsg.h>
#include <mysqld_error.h>

#include <tsys.h>
#include <tmess.h>
#include "my_sql.h"


//************************************************
//* Modul info!                                  *
#define MOD_ID		"MySQL"
#define MOD_NAME	_("DB MySQL")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"2.6.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("BD module. Provides support of the BD MySQL.")
#define MOD_LICENSE	"GPL2"
//************************************************

BDMySQL::BDMod *BDMySQL::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_MySQL_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *bd_MySQL_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new BDMySQL::BDMod(source);
	return NULL;
    }
}

using namespace BDMySQL;

//************************************************
//* BDMySQL::BDMod				 *
//************************************************
BDMod::BDMod( string name ) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, MOD_LICENSE, name);
}

BDMod::~BDMod( )	{ }

TBD *BDMod::openBD( const string &name )	{ return new MBD(name,&owner().openDB_E()); }

//************************************************
//* BDMySQL::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0), connRes(true)
{
    setAddr("localhost;root;123456;test;;;utf8");
}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag && owner().fullDeleteDB())
	try {
	    MYSQL tcon;

	    MtxAlloc resource(connRes, true);
	    if(!mysql_init(&tcon)) throw err_sys(_("Error initializing client."));
	    tcon.reconnect = 1;
	    if(!mysql_real_connect(&tcon,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size()?u_sock.c_str():NULL),CLIENT_MULTI_STATEMENTS))
		throw err_sys(_("Connect to DB error: %s"), mysql_error(&tcon));

	    string req = "DROP DATABASE `" + bd + "`";
	    if(mysql_real_query(&tcon,req.c_str(),req.size())) throw err_sys(_("Query to DB error: %s"), mysql_error(&tcon));

	    mysql_close(&tcon);
	} catch(TError&) { }
}

void MBD::enable( )
{
    MtxAlloc resource(connRes, true);
    if(enableStat())	return;

    //Address parse
    int off = 0;
    host = TSYS::strParse(addr(), 0, ";", &off);
    user = TSYS::strParse(addr(), 0, ";", &off);
    pass = TSYS::strParse(addr(), 0, ";", &off);
    bd   = TSYS::strParse(addr(), 0, ";", &off);
    port = s2i(TSYS::strParse(addr(), 0, ";", &off));
    u_sock = TSYS::strParse(addr(), 0, ";", &off);
    string sets = TSYS::strParse(addr(), 0, ";", &off),
	stChar = TSYS::strParse(sets, 0, "-"),
	stColl = TSYS::strParse(sets, 1, "-"),
	stEngine = TSYS::strParse(sets, 2, "-");
    string tms = TSYS::strParse(addr(), 0, ";" , &off);
    cd_pg  = codePage().size() ? codePage() : Mess->charset();

    //API init
    if(!mysql_init(&connect)) throw err_sys(_("Error initializing client."));

    //Timeouts parse
    off = 0;
    unsigned int tTm;
    if(!(tTm=s2i(TSYS::strParse(tms,0,",",&off))))	tTm = 1;
    mysql_options(&connect, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&tTm);
    if(!(tTm=s2i(TSYS::strParse(tms,0,",",&off))))	tTm = 1;
    mysql_options(&connect, MYSQL_OPT_READ_TIMEOUT, (const char*)&tTm);
    if(!(tTm=s2i(TSYS::strParse(tms,0,",",&off))))	tTm = 1;
    mysql_options(&connect, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&tTm);

    connect.reconnect = 1;
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size()?u_sock.c_str():NULL),CLIENT_MULTI_STATEMENTS))
	throw err_sys(_("Connect to DB error: %s"), mysql_error(&connect));

    TBD::enable();

    try {
	string tvl, req = "CREATE DATABASE IF NOT EXISTS `" + TSYS::strEncode(bd,TSYS::SQL) + "`";
	if(stChar.size()) req += " CHARACTER SET '"+stChar+"'";
	if(stColl.size()) req += " COLLATE '"+stColl+"'";
	sqlReq(req);
    } catch(...) { }

    //Sets prepare and perform
    // Charcode and collation
    if(stChar.size()) {
	string req = "SET NAMES '"+stChar+"'";
	if(stColl.size()) req += " COLLATE '"+stColl+"'";
	sqlReq(req);
    }
    // Other direct
    if(stEngine.size()) sqlReq("SET storage_engine='"+stEngine+"'");
}

void MBD::disable( )
{
    MtxAlloc resource(connRes, true);
    if(!enableStat())	return;

    //Last commit
    if(reqCnt) try{ transCommit(); } catch(...) { }

    TBD::disable();

    mysql_close(&connect);
}

void MBD::allowList( vector<string> &list ) const
{
    if(!enableStat())	return;
    list.clear();
    vector< vector<string> > tbl;
    const_cast<MBD*>(this)->sqlReq("SHOW TABLES FROM `"+TSYS::strEncode(bd,TSYS::SQL)+"`", &tbl, false);
    for(unsigned i_t = 1; i_t < tbl.size(); i_t++)
	list.push_back(tbl[i_t][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error open table '%s'. DB is disabled."), inm.c_str());

    if(create) sqlReq("CREATE TABLE IF NOT EXISTS `"+TSYS::strEncode(bd,TSYS::SQL)+"`.`"+
			TSYS::strEncode(inm, TSYS::SQL)+"` (`<<empty>>` char(20) NOT NULL DEFAULT '' PRIMARY KEY)");
    //Get the table structure description and check it to presence
    vector< vector<string> > tblStrct;
    sqlReq("DESCRIBE `" + TSYS::strEncode(bd,TSYS::SQL) + "`.`" + TSYS::strEncode(inm,TSYS::SQL) + "`", &tblStrct);

    return new MTable(inm, this, &tblStrct);
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    MYSQL_RES *res = NULL;

    if(tbl) tbl->clear();
    if(!enableStat()) return;

    string req = Mess->codeConvOut(cd_pg.c_str(), ireq);

    MtxAlloc resource(connRes, true);	//!! Moved before the transaction checking for prevent the "BEGIN;" and "COMMIT;"
					//   request's sequence breakage on high concurrency access activity

    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    int irez, eNRez;
    rep:
    if((irez=mysql_real_query(&connect,req.c_str(),req.size()))) {
	eNRez = mysql_errno(&connect);
	if(irez == CR_SERVER_GONE_ERROR || irez == CR_SERVER_LOST ||
	    eNRez == CR_SERVER_GONE_ERROR || eNRez == CR_CONN_HOST_ERROR || eNRez == CR_SERVER_LOST || eNRez == CR_CONNECTION_ERROR)
	{
	    resource.unlock();
	    disable();
	    throw err_sys(_("Connect to DB error %d: %s"), irez, mysql_error(&connect));
	}
	if(irez) {
	    if(mysql_errno(&connect) == ER_NO_DB_ERROR) {
		resource.unlock();
		sqlReq("USE `"+TSYS::strEncode(bd,TSYS::SQL)+"`");
		resource.lock();
		goto rep;
	    }
	    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("Query '%s' is error."), ireq.c_str());
	    throw err_sys(_("Query to DB error %d: %s"), irez, mysql_error(&connect));
	}
    }

    do {
	if(!(res=mysql_store_result(&connect)) && mysql_field_count(&connect))
	    throw err_sys(_("Store result error: %s"), mysql_error(&connect));

	if(res && tbl && tbl->empty()) {	//Process only first statement's result
	    int num_fields = mysql_num_fields(res);
	    MYSQL_ROW row;

	    vector<string> fld;
	    //Add head
	    for(int i = 0; i < num_fields; i++)
		fld.push_back(mysql_fetch_field_direct(res,i)->name);
	    tbl->push_back(fld);
	    //Add data
	    while((row=mysql_fetch_row(res))) {
		fld.clear();
		for(int i = 0; i < num_fields; i++)
		    fld.push_back(row[i]?Mess->codeConvIn(cd_pg.c_str(),row[i]):DB_NULL);
		tbl->push_back(fld);
	    }
	}

	if(res) mysql_free_result(res);

	if((irez=mysql_next_result(&connect)) > 0) throw err_sys(_("Could not execute statement: %s"), mysql_error(&connect));
    }while(irez == 0);
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
    if(!enableStat() && toEnable()) enable();
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,"help",
	    _("MySQL DB address must be written as: \"{host};{user};{pass};{db};{port}[;{u_sock}[;{charset-collation-engine}[;{tms}]]]\".\n"
	      "Where:\n"
	      "  host - MySQL server hostname;\n"
	      "  user - DB user name;\n"
	      "  pass - user's password for DB access;\n"
	      "  db - DB name;\n"
	      "  port - DB server port (default 3306);\n"
	      "  u_sock - UNIX-socket name, for local access to DB (/var/lib/mysql/mysql.sock);\n"
	      "  charset-collation-engine - DB charset, collation and storage engine for CREATE DATABASE and SET;\n"
	      "  tms - MySQL timeouts in form \"{connect},{read},{write}\" and in seconds.\n"
	      "For local DB: \";roman;123456;OpenSCADA;;/var/lib/mysql/mysql.sock;utf8-utf8_general_ci-MyISAM;5,2,2\".\n"
	      "For remote DB: \"server.nm.org;roman;123456;OpenSCADA;3306\"."));
	if(reqCnt)
	    ctrMkNode("comm",opt,-1,"/prm/st/end_tr",_("Close opened transaction"),RWRW__,"root",SDB_ID);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/end_tr" && ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR) && reqCnt) transCommit();
    else TBD::cntrCmdProc(opt);
}

//************************************************
//* MBDMySQL::Table                              *
//************************************************
MTable::MTable( string name, MBD *iown, vector< vector<string> > *itblStrct ) : TTable(name)
{
    setNodePrev(iown);

    try {
	//Get the table structure description
	if(itblStrct) tblStrct = *itblStrct;
	else owner().sqlReq("DESCRIBE `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name,TSYS::SQL) + "`", &tblStrct);

	//req = "SELECT * FROM `"+TSYS::strEncode(name,TSYS::SQL)+"` LIMIT 0,1";
	//owner().sqlReq(req);
    } catch(...) { }
}

MTable::~MTable( )	{ }

bool MTable::isEmpty( )	{ return tblStrct.empty() || tblStrct[1][0] == "<<empty>>"; }

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag)
	try { owner().sqlReq("DROP TABLE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+TSYS::strEncode(name(),TSYS::SQL)+"`"); }
	catch(TError &err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
    mLstUse = SYS->sysTm();

    for(unsigned iFld = 1; iFld < tblStrct.size(); iFld++) {
	int pr1, pr2;
	string sid = tblStrct[iFld][0];
	if(cfg.cfgPresent(sid)) continue;

	int flg = (tblStrct[iFld][3]=="PRI") ? (int)TCfg::Key : (int)TFld::NoFlag;
	/*if(tblStrct[iFld][1] == "tinyint(1)")
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Boolean,flg,"1") );
	else */
	if(sscanf(tblStrct[iFld][1].c_str(),"char(%d)",&pr1) || sscanf(tblStrct[iFld][1].c_str(),"varchar(%d)",&pr1))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,i2s(pr1).c_str()));
	else if(tblStrct[iFld][1] == "text")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"65535"));
	else if(tblStrct[iFld][1] == "mediumtext")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(tblStrct[iFld][1] == "int")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(sscanf(tblStrct[iFld][1].c_str(),"int(%d)",&pr1) || sscanf(tblStrct[iFld][1].c_str(),"tinyint(%d)",&pr1) ||
		sscanf(tblStrct[iFld][1].c_str(),"bigint(%d)",&pr1))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg,i2s(pr1).c_str()));
	else if(tblStrct[iFld][1] == "double")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
	else if(sscanf(tblStrct[iFld][1].c_str(),"double(%d,%d)",&pr1,&pr2))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg,(i2s(pr1)+"."+i2s(pr2)).c_str()));
	else if(tblStrct[iFld][1] == "datetime")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg|TFld::DateTimeDec,"10"));
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full )
{
    vector< vector<string> >	inTbl,
				&tbl = full ? *full : inTbl;

    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
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
		if(cf.name() == tblStrct[iFld][0]) break;
	    if(iFld >= tblStrct.size()) return false;
	}
    }

    string sid;
    //Make SELECT and WHERE
    string req = "SELECT ";
    string req_where = "WHERE ";
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
	    req_where += (next?"AND `":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`=" + getVal(*u_cfg) + " ";
	    next = true;
	}
	else if(u_cfg->isKey() || u_cfg->view()) {
	    req += (first_sel?"`":",`") + TSYS::strEncode(sid,TSYS::SQL) + "`";
	    first_sel = false;
	}
    }

    //Request
    if(!full || !full->size() || row == 0) {
	if(first_sel) return false;
	req += " FROM `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` " + ((next)?req_where:"");
	if(!full) req += " LIMIT " + i2s(row) + ",1";

	tbl.clear();
	owner().sqlReq(req, &tbl, false);
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
	    req_where += (next_wr?"AND `":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`=" + getVal(*u_cfg) + " ";
	    if(first_key.empty()) first_key = TSYS::strEncode(sid, TSYS::SQL);
	    next_wr = true;
	}
	else if(u_cfg->view()) {
	    req += (first_sel?"`":",`") + TSYS::strEncode(sid,TSYS::SQL) + "`";
	    first_sel = false;
	}
    }
    if(first_sel) req += "`"+first_key+"`";
    req += " FROM `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` WHERE " + req_where;

    //Query
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
    if(trDblDef && !cfg.reqKeys()) fieldFix(cfg);

    //Get present fields list
    string req_where = "WHERE ";
    // Add key list to query
    bool next = false, noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();		//Force update by ReqKeys or reqKey() present
    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if(!u_cfg.isKey()) continue;
	req_where += (next?"AND `":"`") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "`=" + getVal(u_cfg,TCfg::ExtValTwo) + " ";
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
	fieldFix(cfg);
    }

    //Prepare query
    // Try for get already present field
    string req;
    if(!isForceUpdt) {
	req = "SELECT 1 FROM `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" +
				  TSYS::strEncode(name(),TSYS::SQL) + "` " + req_where;
	owner().sqlReq(req, &tbl, true);
	if(tbl.size() < 2) {
	    // Add new record
	    req = "INSERT INTO `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` ";
	    string ins_name, ins_value;
	    next = false;
	    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
		TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
		if(!u_cfg.isKey() && !u_cfg.view()) continue;

		bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && trPresent && !u_cfg.noTransl());
		ins_name += (next?",`":"`") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "` " +
		    (isTransl ? (",`"+TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[i_el],TSYS::SQL)+"` ") : "");
		sval = getVal(u_cfg);
		ins_value += (next?",":"") + sval + " " + (isTransl?(","+sval+" "):"");
		next = true;
	    }
	    req += "(" + ins_name + ") VALUES (" + ins_value + ")";
	}
	else isForceUpdt = true;
    }
    if(isForceUpdt) {
	// Update present record
	req = "UPDATE `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` SET ";
	next = false;
	for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if((u_cfg.isKey() && !u_cfg.extVal()) || !u_cfg.view()) continue;

	    bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    sval = getVal(u_cfg);
	    req += (next?",`":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`=" + sval + " ";
	    next = true;
	}
	req += req_where;
    }

    //Query
    try { owner().sqlReq(req, NULL, true); }
    catch(TError &err) {
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
	string sid = tblStrct[iFld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?"AND `":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`=" + getVal(*u_cfg) + " ";
	    next = true;
	}
    }

    //Main request
    try { owner().sqlReq("DELETE FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
					 TSYS::strEncode(name(),TSYS::SQL)+"` "+req_where, NULL, true);
    } catch(TError &err) {
	//Check for present
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
				     TSYS::strEncode(name(),TSYS::SQL)+"` "+req_where, &tbl, true);
	if(tbl.size() < 2) return;
    }
}

void MTable::fieldFix( TConfig &cfg )
{
    bool next = false, next_key = false;

    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));

    bool appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !isEmpty()),	//Only for append no present fields
	 isVarTextTransl = (!Mess->lang2CodeBase().empty() && Mess->lang2Code() != Mess->lang2CodeBase());
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Prepare request for fix structure
    string req = "ALTER TABLE `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` ";
    if(!appMode) req += "DROP PRIMARY KEY, ";

    string pr_keys;
    int keyCnt = 0;
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	TCfg &u_cfg = cfg.cfg(cf_el[iCf]);

	// Check primary key
	if(u_cfg.fld().flg()&TCfg::Key && !appMode) {
	    pr_keys += (next_key?",`":"`") + TSYS::strEncode(u_cfg.name(),TSYS::SQL) + "`";
	    next_key = true;
	    keyCnt++;
	}

	for(iFld = 1; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld][0]) break;

	// Change field
	string f_tp;
	if(iFld < tblStrct.size() && !appMode) {
	    switch(u_cfg.fld().type()) {
		case TFld::String:
		    if(u_cfg.fld().len() < 256 || u_cfg.fld().flg()&TCfg::Key)
			f_tp = "varchar(" + i2s(vmax(1,vmin((u_cfg.fld().flg()&TCfg::Key)?(333/(2*keyCnt)):255,u_cfg.fld().len()))) + ")";
		    else if(u_cfg.fld().len() < 65536) f_tp = "text";
		    else f_tp = "mediumtext";
		    break;
		case TFld::Integer:
		    if(u_cfg.fld().flg()&TFld::DateTimeDec) f_tp = "datetime";
		    else if(!u_cfg.fld().len()) f_tp = "bigint";
		    else f_tp = "int(" + i2s(vmax(1,u_cfg.fld().len())) + ")";
		    break;
		case TFld::Real:
		    if(!u_cfg.fld().len()) f_tp = "double";
		    else f_tp = "double(" + i2s(vmax(3,u_cfg.fld().len())) + "," + i2s(vmax(2,u_cfg.fld().dec())) + ")";
		    break;
		case TFld::Boolean: f_tp = "tinyint(1)";	break;
		default: break;
	    }
	    if(tblStrct[iFld][1] != f_tp) {
		req += (next?",CHANGE `":"CHANGE `") + TSYS::strEncode(cf_el[iCf],TSYS::SQL) + "` `" + TSYS::strEncode(cf_el[iCf],TSYS::SQL) + "` ";
		next = true;
		fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", req, keyCnt);
	    }
	}
	// Add field
	else if(iFld >= tblStrct.size()) {
	    req += (next?",ADD `":"ADD `") + TSYS::strEncode(cf_el[iCf],TSYS::SQL) + "` ";
	    next = true;
	    fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", req, keyCnt);
	}
	//Check other languages
	if(u_cfg.fld().flg()&TFld::TransltText) {
	    bool col_cur = false;
	    for(unsigned iC = iFld; iC < tblStrct.size(); iC++)
		if(tblStrct[iC][0].size() > 3 && tblStrct[iC][0].substr(2) == ("#"+cf_el[iCf])) {
		    if(tblStrct[iC][1] != f_tp && !appMode) {
			req += (next?",CHANGE `":"CHANGE `") + TSYS::strEncode(tblStrct[iC][0],TSYS::SQL) + "` `"
							     + TSYS::strEncode(tblStrct[iC][0],TSYS::SQL) + "` ";
			next = true;
			fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", req, keyCnt);
		    }
		    if(tblStrct[iC][0].compare(0,2,Mess->lang2Code()) == 0) col_cur = true;
		}
	    if(!col_cur && isVarTextTransl) {
		req += (next?",ADD `":"ADD `") + TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[iCf],TSYS::SQL) + "` ";
		next = true;
		fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", req, keyCnt);
	    }
	}
    }
    //DROP fields
    for(unsigned iFld = 1, iCf; iFld < tblStrct.size() && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld][0] ||
		    (cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText && tblStrct[iFld][0].size() > 3 &&
		    tblStrct[iFld][0].substr(2) == ("#"+cf_el[iCf]) && tblStrct[iFld][0].compare(0,2,Mess->lang2CodeBase()) != 0))
		break;
	if(iCf >= cf_el.size()) {
	    req += (next?",DROP `":"DROP `") + TSYS::strEncode(tblStrct[iFld][0],TSYS::SQL) + "` ";
	    next = true;
	}
    }
    if(pr_keys.size()) req += ",ADD PRIMARY KEY (" + pr_keys + ") ";

    if(next) {
	owner().sqlReq(req, NULL, false);
	//Update structure information
	req = "DESCRIBE `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "`";
	owner().sqlReq(req, &tblStrct, false);
    }
}

void MTable::fieldPrmSet( TCfg &cfg, const string &last, string &req, int keyCnt )
{
    //Type param
    switch(cfg.fld().type()) {
	case TFld::String:
	    if((cfg.fld().len() && cfg.fld().len() < 256) || cfg.fld().flg()&TCfg::Key)
		req += "varchar(" + i2s(vmax(1,vmin((cfg.fld().flg()&TCfg::Key)?(333/(2*keyCnt)):255,cfg.fld().len()))) + ") " +
			((cfg.fld().flg()&TCfg::Key)?"BINARY ":" ");
	    else if(cfg.fld().len() < 65536) req += "text ";
	    else req += "mediumtext ";
	    req += ((cfg.fld().def() == EVAL_STR) ? "DEFAULT NULL " : "NOT NULL DEFAULT '"+TSYS::strEncode(cfg.fld().def(),TSYS::SQL)+"' ");
	    break;
	case TFld::Integer:
	    if(cfg.fld().flg()&TFld::DateTimeDec)
		req += "datetime " + ((s2ll(cfg.fld().def())==EVAL_INT)?"DEFAULT NULL ":"NOT NULL DEFAULT '"+UTCtoSQL(s2ll(cfg.fld().def()))+"' ");
	    else if(!cfg.fld().len())
		req += "bigint " + ((s2ll(cfg.fld().def())==EVAL_INT)?"DEFAULT NULL ":"NOT NULL DEFAULT '"+ll2s(s2ll(cfg.fld().def()))+"' ");
	    else req += "int(" + i2s(vmax(1,cfg.fld().len())) + ") " +
			((s2ll(cfg.fld().def())==EVAL_INT)?"DEFAULT NULL ":"NOT NULL DEFAULT '"+ll2s(s2ll(cfg.fld().def()))+"' ");
	    break;
	case TFld::Real:
	    if(!cfg.fld().len()) req += "double ";
	    else req += "double(" + i2s(vmax(3,cfg.fld().len())) + "," + i2s(vmax(2,cfg.fld().dec())) + ") ";
	    req += ((s2r(cfg.fld().def())==EVAL_REAL)?"DEFAULT NULL ":"NOT NULL DEFAULT '"+r2s(s2r(cfg.fld().def()))+"' ");
	    break;
	case TFld::Boolean:
	    req += "tinyint(1) " + ((s2i(cfg.fld().def())==EVAL_BOOL)?"DEFAULT NULL ":"NOT NULL DEFAULT '"+i2s(s2i(cfg.fld().def()))+"' ");
	    break;
	default: break;
    }
    //Position param
    //if(last.size())	req += "AFTER `"+last+"` ";
}

string MTable::getVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->lang2Code());
	rez = TSYS::strEncode(((cfg.fld().len()>0)?rez.substr(0,cfg.fld().len()):rez), TSYS::SQL);
    }
    else if(cfg.fld().flg()&TFld::DateTimeDec)	rez = UTCtoSQL(s2i(rez));

    return "'" + rez + "'";
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

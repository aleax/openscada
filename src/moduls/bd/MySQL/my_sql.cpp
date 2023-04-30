
//OpenSCADA module BD.MySQL file: my_sql.cpp
/***************************************************************************
 *   Copyright (C) 2003-2023 by Roman Savochenko, <roman@oscada.org>       *
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
#define MOD_NAME	trS("DB MySQL")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"4.0.7"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("DB module. Provides support of the DBMS MySQL.")
#define MOD_LICENSE	"GPL2"
#define FEATURES	"SQL, LIST, STRUCT, GET, SEEK, PRELOAD, SET, DEL, FIX, TR, ERR"
//************************************************

#define MYSQL_RECONNECT		0		//!!!! The MySQL/MariaDB reconnect some time crashable due to a need of releasing some locks

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

string BDMod::features( )	{ return FEATURES; }

TBD *BDMod::openBD( const string &name )	{ return new MBD(name,&owner().openDB_E()); }

//************************************************
//* BDMySQL::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0)
{
    setAddr("localhost;root;123456;test;;;utf8");
}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag&NodeRemove && owner().fullDeleteDB())
	try {
	    MYSQL tcon;

	    MtxAlloc resource(connRes, true);
	    if(!mysql_init(&tcon)) throw err_sys(_("Error initializing."));
	    my_bool reconnect = MYSQL_RECONNECT;
	    mysql_options(&tcon, MYSQL_OPT_RECONNECT, &reconnect);
	    if(!mysql_real_connect(&tcon,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size()?u_sock.c_str():NULL),CLIENT_MULTI_STATEMENTS))
		throw err_sys(_("Error connecting to the DB: %s"), mysql_error(&tcon));

	    string req = "DROP DATABASE `" + bd + "`";
	    if(mysql_real_query(&tcon,req.c_str(),req.size()))
		throw err_sys(_("Error querying to the DB: %s"), mysql_error(&tcon));

	    mysql_close(&tcon);
	} catch(TError&) { }
}

void MBD::enable( )
{
    MtxAlloc resource(connRes, true);
    //Reconnecting
    if(enableStat()) {
	mysql_close(&connect);
	mEn = false;
	//return;
    }

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
    if(!mysql_init(&connect)) throw err_sys(_("Error initializing."));

    //Timeouts parse
    off = 0;
    unsigned int tTm;
    if(!(tTm=s2i(TSYS::strParse(tms,0,",",&off))))	tTm = 10;
    mysql_options(&connect, MYSQL_OPT_CONNECT_TIMEOUT, &tTm);
    if(!(tTm=s2i(TSYS::strParse(tms,0,",",&off))))	tTm = 5;
    mysql_options(&connect, MYSQL_OPT_READ_TIMEOUT, &tTm);
    if(!(tTm=s2i(TSYS::strParse(tms,0,",",&off))))	tTm = 5;
    mysql_options(&connect, MYSQL_OPT_WRITE_TIMEOUT, &tTm);

    my_bool reconnect = MYSQL_RECONNECT;
    mysql_options(&connect, MYSQL_OPT_RECONNECT, &reconnect);
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size()?u_sock.c_str():NULL),CLIENT_MULTI_STATEMENTS))
	throw err_sys(_("Error connecting to the DB: %s"), mysql_error(&connect));

    TBD::enable();

    //Setting the "ANSI_QUOTES" SQL MODE
    sqlReq("SET SESSION sql_mode='ANSI_QUOTES'");

    //Setting the character and collation mode
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
    const_cast<MBD*>(this)->sqlReq("SHOW TABLES FROM `"+TSYS::strEncode(bd,TSYS::SQL)+"`", &tbl/*, false*/);
    for(unsigned iT = 1; iT < tbl.size(); iT++)
	list.push_back(tbl[iT][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error opening the table '%s': the DB is disabled."), inm.c_str());

    if(create) sqlReq("CREATE TABLE IF NOT EXISTS `"+TSYS::strEncode(bd,TSYS::SQL)+"`.`"+
			TSYS::strEncode(inm, TSYS::SQL)+"` (`<<empty>>` char(20) NOT NULL DEFAULT '' PRIMARY KEY)");
    //Get the table structure description and check it to presence
    vector<TTable::TStrIt> tblStrct;
    getStructDB(inm, tblStrct);

    return new MTable(inm, this, &tblStrct);
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    MYSQL_RES *res = NULL;

    if(!enableStat()) return;
    if(tbl) tbl->clear();

    string req = Mess->codeConvOut(cd_pg.c_str(), ireq);

    MtxAlloc resource(connRes, true);	//!! Moved before the transaction checking for prevent the "BEGIN;" and "COMMIT;"
					//   request's sequence breakage on high concurrency access activity

    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    int irez, eNRez, repCnt = 0;
    rep:
    if((irez=mysql_real_query(&connect,req.c_str(),req.size()))) {
	eNRez = mysql_errno(&connect);
	if(irez == CR_SERVER_GONE_ERROR || irez == CR_SERVER_LOST ||
	    eNRez == CR_SERVER_GONE_ERROR || eNRez == CR_CONN_HOST_ERROR || eNRez == CR_SERVER_LOST || eNRez == CR_CONNECTION_ERROR)
	{
#if !MYSQL_RECONNECT
	    //Try to reconnect
	    if((repCnt++) < 3)
		try { enable(); goto rep; } catch(TError&) { }
	    else mess_warning(nodePath().c_str(), _("Repeated errors of requesting the DB: '%s (%d)'."), mysql_error(&connect), irez);
#endif
	    //resource.unlock();
	    disable();
	    throw err_sys(_("Error connecting to the DB: '%s (%d)'!"), mysql_error(&connect), irez);
	}
	if(irez) {
	    if(mysql_errno(&connect) == ER_NO_DB_ERROR) {
		//resource.unlock();
		sqlReq("USE `"+TSYS::strEncode(bd,TSYS::SQL)+"`");
		//resource.lock();
		goto rep;
	    }
	    if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, _("Error the query '%s': '%s (%d)'."), ireq.c_str(), mysql_error(&connect), irez);
	    throw err_sys(_("Error querying the DB: '%s (%d)'!"), mysql_error(&connect), irez);
	}
    }

    do {
	if(!(res=mysql_store_result(&connect)) && mysql_field_count(&connect))
	    throw err_sys(_("Error storing the result: %s"), mysql_error(&connect));

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
    } while(irez == 0);
}

void MBD::transOpen( )
{
    //Check for limit in one trinsaction
    if(reqCnt > 1000) transCommit();

    connRes.lock();
    bool begin = !reqCnt;
    if(begin) trOpenTm = TSYS::curTime();
    reqCnt++;
    reqCntTm = TSYS::curTime();
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
    if(enableStat() && reqCnt && ((TSYS::curTime()-reqCntTm) > 1e6*trTm_ClsOnReq() || (TSYS::curTime()-trOpenTm) > 1e6*trTm_ClsOnOpen()))
	transCommit();
    if(!enableStat() && toEnable()) enable();
}

void MBD::getStructDB( const string &nm, vector<TTable::TStrIt> &tblStrct )
{
    vector< vector<string> > origTblStrct;

    sqlReq("DESCRIBE `" + TSYS::strEncode(bd,TSYS::SQL) + "`.`" + TSYS::strEncode(nm,TSYS::SQL) + "`", &origTblStrct);

    tblStrct.clear();
    for(unsigned iReq = 1; iReq < origTblStrct.size(); ++iReq)
	tblStrct.push_back(TTable::TStrIt(origTblStrct[iReq][0],origTblStrct[iReq][1],(origTblStrct[iReq][3]=="PRI"),origTblStrct[iReq][4]));
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,"help",
	    _("MySQL DBMS address must be written as: \"{host};{user};{pass};{db};{port}[;{u_sock}[;{charset-collation-engine}[;{tms}]]]\".\n"
	      "Where:\n"
	      "  host - hostname on which the DBMS server MySQL works;\n"
	      "  user - user name of the DB;\n"
	      "  pass - password of the user for accessing the DB;\n"
	      "  db   - name of the DB;\n"
	      "  port - port, which listening by the DBMS server (default 3306);\n"
	      "  u_sock - UNIX-socket name, for local accessing to the DBMS (/var/lib/mysql/mysql.sock);\n"
	      "  charset-collation-engine - DB charset, collation and storage engine for CREATE DATABASE and SET;\n"
	      "  tms - MySQL timeouts in the form \"{connect},{read},{write}\" and in seconds.\n"
	      "For local DBMS: \";user;password;OpenSCADA;;/var/lib/mysql/mysql.sock;utf8-utf8_general_ci-MyISAM;5,2,2\".\n"
	      "For remote DBMS: \"server.nm.org;user;password;OpenSCADA;3306\"."));
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
    if(flag&NodeRemove)
	try { owner().sqlReq("DROP TABLE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+TSYS::strEncode(name(),TSYS::SQL)+"`"); }
	catch(TError &err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	int pr1, pr2;
	string sid = tblStrct[iFld].nm;
	if(cfg.cfgPresent(sid)) continue;

	int flg = tblStrct[iFld].key ? (int)TCfg::Key : (int)TFld::NoFlag;
	/*if(tblStrct[iFld].tp == "tinyint(1)")
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Boolean,flg,"1") );
	else */
	if(sscanf(tblStrct[iFld].tp.c_str(),"char(%d)",&pr1) || sscanf(tblStrct[iFld].tp.c_str(),"varchar(%d)",&pr1))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,i2s(pr1).c_str()));
	else if(tblStrct[iFld].tp == "text")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"65535"));
	else if(tblStrct[iFld].tp == "mediumtext")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215"));
	else if(tblStrct[iFld].tp == "int")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
	else if(sscanf(tblStrct[iFld].tp.c_str(),"int(%d)",&pr1) || sscanf(tblStrct[iFld].tp.c_str(),"tinyint(%d)",&pr1) ||
		sscanf(tblStrct[iFld].tp.c_str(),"bigint(%d)",&pr1))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg,i2s(pr1).c_str()));
	else if(tblStrct[iFld].tp == "double")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
	else if(sscanf(tblStrct[iFld].tp.c_str(),"double(%d,%d)",&pr1,&pr2))
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg,(i2s(pr1)+"."+i2s(pr2)).c_str()));
	else if(tblStrct[iFld].tp == "datetime")
	    cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg|TFld::DateTimeDec,"10"));
    }
}

void MTable::fieldFix( TConfig &cfg, const string &ilangLs )
{
    bool next = false;

    if(tblStrct.empty()) throw err_sys(_("Table is empty."));

    bool appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !isEmpty());	//Only for append no present fields
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    string pr_keys, ls, langLs;
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	langLs = ilangLs;
	TCfg &u_cfg = cfg.cfg(cf_el[iCf]);

	// Check primary key
	if(u_cfg.fld().flg()&TCfg::Key && !appMode)
	    pr_keys += (pr_keys.size()?",`":"`") + TSYS::strEncode(u_cfg.name(),TSYS::SQL) + "`";

	for(iFld = 0; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld].nm) break;

	// Change field
	string f_tp;
	if(iFld < tblStrct.size() && !appMode) {
	    switch(u_cfg.fld().type()) {
		case TFld::String:
		    if(u_cfg.fld().len() < 256 || u_cfg.fld().flg()&TCfg::Key)
			f_tp = "varchar(" + i2s(vmax(1,vmin(255,u_cfg.fld().len()))) + ")";
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
	    if(tblStrct[iFld].tp != f_tp) {
		ls += (ls.size()?", CHANGE `":" CHANGE `") + TSYS::strEncode(cf_el[iCf],TSYS::SQL) + "` `" + TSYS::strEncode(cf_el[iCf],TSYS::SQL) + "` ";
		fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", ls);
	    }
	}
	// Add field
	else if(iFld >= tblStrct.size()) {
	    ls += (ls.size()?", ADD `":" ADD `") + TSYS::strEncode(cf_el[iCf],TSYS::SQL) + "`";
	    fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", ls);
	}
	//Check other languages
	if(u_cfg.fld().flg()&TFld::TransltText) {
	    size_t pos = 0;
	    //  Change for present translations
	    for(unsigned iC = iFld; iC < tblStrct.size(); iC++)
		if(tblStrct[iC].nm.size() > 3 && tblStrct[iC].nm.substr(2) == ("#"+cf_el[iCf])) {
		    if(tblStrct[iC].tp != f_tp && !appMode) {
			ls += (ls.size()?", CHANGE `":" CHANGE `") + TSYS::strEncode(tblStrct[iC].nm,TSYS::SQL) + "` `" + TSYS::strEncode(tblStrct[iC].nm,TSYS::SQL) + "`";
			fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", ls);
		    }

		    if((pos=langLs.find(tblStrct[iC].nm.substr(0,2)+";")) != string::npos)
			langLs.replace(pos, 3, "");
		}

	    //  Append translation for new languages
	    string toLang;
	    for(int off = 0; (toLang=TSYS::strParse(langLs,0,";",&off)).size(); ) {
		ls += (ls.size()?", ADD `":" ADD `") + TSYS::strEncode(toLang+"#"+cf_el[iCf],TSYS::SQL) + "`";
		fieldPrmSet(u_cfg, (iCf>0)?cf_el[iCf-1]:"", ls);
	    }
	}
    }
    //DROP fields
    for(unsigned iFld = 0, iCf; iFld < tblStrct.size() && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld].nm ||
		    // Pass all the column translation
		    ((cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText) &&
			tblStrct[iFld].nm.size() > 3 && tblStrct[iFld].nm.substr(2) == ("#"+cf_el[iCf])))
		break;
	if(iCf >= cf_el.size())
	    ls += (ls.size()?", DROP `":" DROP `") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL) + "`";
    }

    if(ls.size()) {
	//Prepare request for fix structure
	string req = "ALTER TABLE `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` " +
	    (appMode?"":"DROP PRIMARY KEY, ") + ls + (pr_keys.size()?",ADD PRIMARY KEY ("+pr_keys+")":"");

	try { owner().sqlReq(req, NULL, false); }
	catch(TError&) {
	    //An error possible at creating already presented columns, so try to update the table structure information before
	    owner().getStructDB(name(), tblStrct);
	    owner().sqlReq(req, NULL, false);
	}

	//Update the table structure
	owner().getStructDB(name(), tblStrct);
    }
}

void MTable::fieldPrmSet( TCfg &cfg, const string &last, string &req )
{
    //Type param
    switch(cfg.fld().type()) {
	case TFld::String:
	    if((cfg.fld().len() && cfg.fld().len() < 256) || cfg.fld().flg()&TCfg::Key)
		req += "varchar(" + i2s(vmax(1,vmin(255,cfg.fld().len()))) + ") " + ((cfg.fld().flg()&TCfg::Key)?"BINARY ":" ") +
			((cfg.fld().def() == EVAL_STR) ? "DEFAULT NULL " : "NOT NULL DEFAULT '"+TSYS::strEncode(cfg.fld().def(),TSYS::SQL)+"' ");
	    // Due to "BLOB/TEXT can't have a default value (1)"
	    else if(cfg.fld().len() < 65536)
		req += string("text "); //+ ((cfg.fld().def() == EVAL_STR) ? "DEFAULT NULL " : "NOT NULL DEFAULT '' ");
	    else req += string("mediumtext ");// + ((cfg.fld().def() == EVAL_STR) ? "DEFAULT NULL " : "NOT NULL DEFAULT '' ");
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

string MTable::getSQLVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	//if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->langCode());
	rez = "'" + TSYS::strEncode(rez /*((cfg.fld().len()>0)?rez.substr(0,cfg.fld().len()):rez)*/, TSYS::SQL) + "'";
    }
    else if(cfg.fld().flg()&TFld::DateTimeDec)	rez = "'" + UTCtoSQL(s2i(rez)) + "'";

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

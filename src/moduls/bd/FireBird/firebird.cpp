
//OpenSCADA module BD.FireBird file: firebird.cpp
/***************************************************************************
 *   Copyright (C) 2007-2021 by Roman Savochenko, <roman@oscada.org>       *
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
#include <ibase.h>

#include <tsys.h>
#include <tmess.h>
#include "firebird.h"

//******************************************************************************
//* Modul info!                                                                *
#define MOD_ID		"FireBird"
#define MOD_NAME	_("DB FireBird")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"3.0.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("DB module. Provides support of the DBMS FireBird.")
#define LICENSE		"GPL2"
#define FEATURES	"SQL, LIST, STRUCT, GET, SEEK, PRELOAD, SET, DEL, FIX, TR"
//******************************************************************************

FireBird::BDMod *FireBird::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_FireBird_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *bd_FireBird_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new FireBird::BDMod(source);
	return NULL;
    }
}

using namespace FireBird;

//*************************************************
//* FireBird::BDMod				  *
//*************************************************
BDMod::BDMod( string name ) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

BDMod::~BDMod( )	{ }

string BDMod::features( )	{ return FEATURES; }

TBD *BDMod::openBD( const string &iid )	{ return new MBD(iid,&owner().openDB_E()); }

string BDMod::sqlReqCode( const string &req, char symb )
{
    string sout = req;
    for(unsigned i_sz = 0; i_sz < sout.size(); i_sz++)
	if(sout[i_sz] == symb) sout.replace(i_sz++, 1, 2, symb);

    return sout;
}

//************************************************
//* FireBird::MBD				 *
//************************************************
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), conTm("1"), hdb(0), htrans(0), reqCnt(0), reqCntTm(0), trOpenTm(0)
{
    setAddr("localhost:/var/tmp/test.fbd");
}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag&NodeRemove && owner().fullDeleteDB()) {
	//Attach to DB
	// Prepare database parameter buffer (DPB)
	char *dpb = (char*)malloc(50);
	short dpb_length = 0;
	isc_modify_dpb(&dpb, &dpb_length, isc_dpb_user_name, user.c_str(), user.size());
	isc_modify_dpb(&dpb, &dpb_length, isc_dpb_password, pass.c_str(), pass.size());
	isc_modify_dpb(&dpb, &dpb_length, isc_dpb_connect_timeout, conTm.c_str(), conTm.size());

	ISC_STATUS_ARRAY status;
	if(isc_attach_database(status,0,fdb.c_str(),&hdb,dpb_length,dpb)) return;
	if(isc_drop_database(status,&hdb))
	    throw err_sys(_("Error dropping the DB '%s': %s"), fdb.c_str(), getErr(status).c_str());
    }
}

void MBD::enable( )
{
    MtxAlloc res(connRes, true);
    if(enableStat()) {
	ISC_STATUS_ARRAY status;
	isc_detach_database(status, &hdb);
	hdb = 0;
	mEn = false;
	//return;
    }

    //DB address parsing
    int off = 0;
    fdb   = TSYS::strParse(addr(), 0, ";", &off);
    user  = TSYS::strParse(addr(), 0, ";", &off);
    pass  = TSYS::strParse(addr(), 0, ";", &off);
    conTm = (off < (int)addr().size()) ? TSYS::strParse(addr(), 0, ";", &off) : "1";
    cd_pg = codePage().size() ? codePage() : Mess->charset();

    char *dpb = (char*)malloc(100);
    short dpb_length = 0;
    isc_modify_dpb(&dpb, &dpb_length, isc_dpb_user_name, user.c_str(), user.size());
    isc_modify_dpb(&dpb, &dpb_length, isc_dpb_password, pass.c_str(), pass.size());
    //isc_modify_dpb(&dpb, &dpb_length, isc_dpb_connect_timeout, conTm.c_str(), conTm.size());

    ISC_STATUS_ARRAY status;
    if(isc_attach_database(status,0,fdb.c_str(),&hdb,dpb_length,dpb)) {
	//  Make try for create DB
	isc_tr_handle trans = 0;
	if(isc_dsql_execute_immediate(status,&hdb,&trans,0,
		    ("CREATE DATABASE '"+fdb+"' USER '"+user+"' PASSWORD '"+pass+"'").c_str(),3,NULL))
	{
	    isc_free(dpb);
	    isc_detach_database(status, &hdb);
	    throw err_sys(_("Error creating the DB '%s': %s"), fdb.c_str(), getErr(status).c_str());
	}
    }

    htrans = 0;

    TBD::enable();
}

void MBD::disable( )
{
    MtxAlloc res(connRes, true);
    if(!enableStat()) return;

    TBD::disable();

    //Last commit
    if(reqCnt) try { transCommit(); } catch(...) { }

    ISC_STATUS_ARRAY status;
    isc_detach_database(status, &hdb);
    hdb = 0;
}

void MBD::allowList( vector<string> &list ) const
{
    if(!enableStat()) return;
    list.clear();
    vector< vector<string> > tbl;
    const_cast<MBD*>(this)->sqlReq("SELECT rdb$relation_name FROM rdb$relations WHERE "
	"((rdb$system_flag = 0) OR (rdb$system_flag IS NULL)) AND "
	"(rdb$view_source IS NULL) ORDER BY rdb$relation_name", &tbl);
    for(unsigned iT = 1; iT < tbl.size(); iT++)
	list.push_back(tbl[iT][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error opening the table '%s': the DB is disabled."), inm.c_str());

    if(create) {
	string req = "EXECUTE BLOCK AS BEGIN "
	    "if (not exists(select 1 from rdb$relations where rdb$relation_name = '" + mod->sqlReqCode(inm) + "')) then "
	    "execute statement 'create table \"" + mod->sqlReqCode(inm,'"') + "\" (\"<<empty>>\" VARCHAR(20) NOT NULL, "
	    "CONSTRAINT \"pk_" + mod->sqlReqCode(inm,'"') + "\" PRIMARY KEY(\"<<empty>>\") )'; END";
	sqlReq(req);
    }
    vector<TTable::TStrIt> tblStrct;
    getStructDB(inm, tblStrct);
    if(tblStrct.empty()) throw err_sys(_("The table '%s' is not present."), name().c_str());

    return new MTable(inm, this, &tblStrct);
}

string MBD::getErr( ISC_STATUS_ARRAY status )
{
    string err;
    char msg[512];

    err = i2s(isc_sqlcode(status)) + ":";
    while(fb_interpret(msg,sizeof(msg),(const ISC_STATUS**)&status))
	err += string("-") + msg;

    return err;
}

void MBD::transOpen( )
{
    ISC_STATUS_ARRAY status;

    //Check for limit into one trinsaction
    if(reqCnt > 1000) transCommit();

    MtxAlloc res(connRes, true);
    if(!htrans) {
	if(isc_start_transaction(status,&htrans,1,&hdb,0,NULL)) {
	    mess_sys(TMess::Warning, _("Error starting a transaction: %s"), getErr(status).c_str());
	    return;
	    //throw err_sys(_("Error starting a transaction: %s"), getErr(status).c_str());
	}
	trOpenTm = TSYS::curTime();
    }
    reqCnt++;
    reqCntTm = TSYS::curTime();
}

void MBD::transCommit( )
{
    ISC_STATUS_ARRAY status;

    MtxAlloc res(connRes, true);
    if(!htrans) return;
    if(isc_commit_transaction(status, &htrans)) {
	mess_sys(TMess::Warning, _("Error committing a transaction: %s"), getErr(status).c_str());
	return;
	//throw err_sys(_("Error committing a transaction: %s"), getErr(status).c_str());
    }
    htrans = 0;
    reqCnt = reqCntTm = 0;
}

void MBD::transCloseCheck( )
{
    if(!enableStat() && toEnable()) enable();
    if(reqCnt && ((TSYS::curTime()-reqCntTm) > 1e6*trTm_ClsOnReq() || (TSYS::curTime()-trOpenTm) > 1e6*trTm_ClsOnOpen()))
	transCommit();
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    if(!enableStat()) return;
    if(tbl) tbl->clear();

    if(intoTrans && intoTrans != EVAL_BOOL)	transOpen();
    else if(!intoTrans && htrans)		transCommit();

    MtxAlloc res(connRes, true);

    XSQLDA  *out_sqlda = (XSQLDA*)malloc(XSQLDA_LENGTH(10));
    out_sqlda->version = SQLDA_VERSION1;
    out_sqlda->sqln = 10;
    char    *dtBuf = NULL;
    int      dtBufLen = 0;
    isc_stmt_handle stmt = 0;
    isc_tr_handle trans = htrans;
    ISC_STATUS_ARRAY status;

    try {
	int repCnt = 0;
	rep:
	ISC_STATUS rez = 0;
	//Prepare statement
	if((rez=isc_dsql_allocate_statement(status,&hdb,&stmt)))
	    mess_sys(TMess::Debug, _("Error allocating a statement: (%d)%s"), rez, getErr(status).c_str());
	else if(!trans && (rez=isc_start_transaction(status,&trans,1,&hdb,0,NULL)))
	    mess_sys(TMess::Debug, _("Error starting a transaction: (%d)%s"), rez, getErr(status).c_str());
	//Prepare output data structure
	else if((rez=isc_dsql_prepare(status,&trans,&stmt,0,Mess->codeConvOut(cd_pg.c_str(),ireq).c_str(),3,NULL)))
	    mess_sys(TMess::Debug, _("DSQL: error preparing: (%d)%s"), rez, getErr(status).c_str());
	else if(!rez && (rez=isc_dsql_describe(status,&stmt,1,out_sqlda)))
	    mess_sys(TMess::Debug, _("DSQL: error describing: (%d)%s"), rez, getErr(status).c_str());
	//Reconnect try for error
	if(rez)
	    switch(rez) {
		case isc_network_error:
		case isc_net_connect_err:
		case isc_net_connect_listen_err:
		case isc_net_write_err: case isc_net_read_err:
		    //Try to reconnect
		    if((repCnt++) < 3)
			try { enable(); goto rep; } catch(TError&) { }
		    else mess_warning(nodePath().c_str(), _("Repeated errors of requesting the DB: %s."), getErr(status).c_str());

		    disable();
		    throw err_sys(_("Error connecting to a DB: %s"), getErr(status).c_str());
		default:
		    throw err_sys(_("Error requesting: %s"), getErr(status).c_str());
	    }
	if(out_sqlda->sqld > out_sqlda->sqln) {
	    int n = out_sqlda->sqld;
	    free(out_sqlda);
	    out_sqlda = (XSQLDA*)malloc(XSQLDA_LENGTH(n));
	    out_sqlda->sqln = n;
	    out_sqlda->version = SQLDA_VERSION1;
	    isc_dsql_describe(status, &stmt, 1, out_sqlda);
	}
	//  Setup SQLDA
	//   Data buffer size calc
	for(int i = 0; i < out_sqlda->sqld; i++) {
	    dtBufLen += out_sqlda->sqlvar[i].sqllen + sizeof(short);
	    if((out_sqlda->sqlvar[i].sqltype&(~1)) == SQL_VARYING) dtBufLen += sizeof(short);
	}
	if(dtBufLen) {
	    dtBuf = (char*)malloc(dtBufLen);
	    for(int i = 0, off = 0; i < out_sqlda->sqld; i++) {
		out_sqlda->sqlvar[i].sqldata = dtBuf + off;
		off += out_sqlda->sqlvar[i].sqllen;
		if((out_sqlda->sqlvar[i].sqltype&(~1)) == SQL_VARYING) off += sizeof(short);
		out_sqlda->sqlvar[i].sqlind = (short*)(dtBuf+off);
		off += sizeof(short);
	    }
	}

	//  Get data
	if(isc_dsql_execute(status,&trans,&stmt,1,NULL))
	    throw err_sys(_("DSQL: error executing: %s"), getErr(status).c_str());
	if(tbl && out_sqlda->sqld) {
	    //if( isc_dsql_set_cursor_name(status, &stmt,"dyn_cursor", 0) )
	    //  throw err_sys(_("DSQL: error opening a cursor: %s"),getErr(status).c_str());
	    vector<string> row;
	    long  fetch_stat;
	    while((fetch_stat=isc_dsql_fetch(status,&stmt,1,out_sqlda)) == 0) {
		//  Add head
		if(row.empty()) {
		    for(int i = 0; i < out_sqlda->sqld; i++)
			row.push_back(out_sqlda->sqlvar[i].sqlname);
		    tbl->push_back(row);
		}
		//  Add row
		row.clear();
		for(int i = 0; i < out_sqlda->sqld; i++) {
		    XSQLVAR &var = out_sqlda->sqlvar[i];
		    // Null handling.  If the column is nullable and null
		    if((var.sqltype&1) && (*var.sqlind < 0)) row.push_back(DB_NULL);
		    else switch(var.sqltype&(~1)) {
			case SQL_TEXT:
			    row.push_back(Mess->codeConvIn(cd_pg.c_str(),clrEndSpace(string(var.sqldata,var.sqllen))));
			    break;
			case SQL_VARYING:
			    row.push_back(Mess->codeConvIn(cd_pg.c_str(),string((char*)var.sqldata+sizeof(short),*(short*)var.sqldata)));
			    break;
			case SQL_SHORT:	row.push_back(i2s(*(short *)var.sqldata));	break;
			case SQL_LONG:	row.push_back(i2s(*(int*)var.sqldata));		break;
			case SQL_INT64:	row.push_back(ll2s(*(ISC_INT64 *)var.sqldata));	break;
			case SQL_FLOAT:	row.push_back(r2s(*(float *)var.sqldata));	break;
			case SQL_DOUBLE:row.push_back(r2s(*(double *)var.sqldata));	break;
			case SQL_BLOB: {
			    //> Read blob data
			    ISC_QUAD blob_id = *((ISC_QUAD*)var.sqldata);
			    ISC_STATUS blob_stat;
			    isc_blob_handle blob_handle = 0;
			    char blob_segment[prmStrBuf_SZ];
			    unsigned short actual_seg_len;
			    string bval;
			    isc_open_blob2(status, &hdb, &trans, &blob_handle, &blob_id, 0, NULL);
			    do {
				blob_stat = isc_get_segment(status, &blob_handle, &actual_seg_len, sizeof(blob_segment), blob_segment);
				bval.append(blob_segment, actual_seg_len);
			    }
			    while(blob_stat == 0 && status[1] == isc_segment);
			    isc_close_blob(status, &blob_handle);
			    row.push_back(bval);
			    break;
			}
			default: row.push_back("");
		    }
		}
		tbl->push_back(row);
	    }
	}
	if(isc_dsql_free_statement(status,&stmt,DSQL_drop)) {
	    stmt = 0;
	    throw err_sys(_("DSQL: error freeing a statement: %s"), getErr(status).c_str());
	}
	if(trans && !htrans && isc_commit_transaction(status,&trans)) {
	    stmt = trans = 0;
	    throw err_sys(_("DSQL: error closing a transaction: %s"), getErr(status).c_str());
	}
    } catch(...) {
	if(stmt) isc_dsql_free_statement(status, &stmt, DSQL_drop);
	if(trans && !htrans) isc_commit_transaction(status, &trans);
	free(out_sqlda);
	if(dtBuf) free(dtBuf);
	throw;
    }

    free(out_sqlda);
    if(dtBuf) free(dtBuf);
}

string MBD::clrEndSpace( const string &vl )
{
    int i = vl.size()-1;
    while(i >= 0 && (vl[i] == ' ' || vl[i] == '\t' || vl[i] == '\n')) i--;
    return vl.substr(0, i+1);
}

void MBD::getStructDB( const string &nm, vector<TTable::TStrIt> &tblStrct )
{
    vector< vector<string> > origTblStrct, keyLst;

    //Getting the generic data structure
    sqlReq("SELECT R.RDB$FIELD_NAME, F.RDB$FIELD_TYPE, F.RDB$FIELD_LENGTH "
	"FROM RDB$FIELDS F, RDB$RELATION_FIELDS R where F.RDB$FIELD_NAME = R.RDB$FIELD_SOURCE and "
	"R.RDB$SYSTEM_FLAG = 0 and R.RDB$RELATION_NAME = '"+mod->sqlReqCode(nm)+"'", &origTblStrct, false);

    tblStrct.clear();
    if(origTblStrct.size()) {
	//Getting the keys
	sqlReq("SELECT I.RDB$FIELD_NAME, C.RDB$CONSTRAINT_TYPE "
	    "FROM RDB$RELATION_CONSTRAINTS C, RDB$INDEX_SEGMENTS I "
	    "WHERE C.RDB$INDEX_NAME = I.RDB$INDEX_NAME AND C.RDB$RELATION_NAME = '"+
	    mod->sqlReqCode(nm)+"'", &keyLst, false);

	//Filling the structure
	for(unsigned iReq = 1; iReq < origTblStrct.size(); ++iReq) {
	    bool isKey = false;
	    for(unsigned iKey = 0; iKey < keyLst.size() && !isKey; ++iKey)
		isKey = (origTblStrct[iReq][0] == keyLst[iKey][0] && keyLst[iKey][1] == "PRIMARY KEY");

	    tblStrct.push_back(TTable::TStrIt(origTblStrct[iReq][0],origTblStrct[iReq][1],isKey,origTblStrct[iReq][2]));
	}
    }
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,"help",
	    _("Address to the FireBird DBMS must be written as: \"{file};{user};{pass}[;{conTm}]\".\n"
	      "Where:\n"
	      "  file - full path to the DB file in the form: \"[{host}:]{filePath}\";\n"
	      "  user - DB user;\n"
	      "  pass - password of the DB user;\n"
	      "  conTm- connection timeout, seconds."));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//************************************************
//* FireBird::Table				 *
//************************************************
MTable::MTable( string inm, MBD *iown, vector<TStrIt> *itblStrct ) : TTable(inm)
{
    setNodePrev(iown);

    //Get table structure description
    try {
	if(itblStrct) tblStrct = *itblStrct;
	else owner().getStructDB(name(), tblStrct);
    } catch(...) { }
}

MTable::~MTable( )	{ }

bool MTable::isEmpty( )	{ return tblStrct.empty() || tblStrct[0].nm == "<<empty>>"; }

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag&NodeRemove) {
	try { owner().sqlReq("DROP TABLE \"" + mod->sqlReqCode(name(),'"') + "\""); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
    }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("The table is empty."));
    mLstUse = SYS->sysTm();

    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld].nm;
	if(cfg.cfgPresent(sid)) continue;

	int flg = tblStrct[iFld].key ? (int)TCfg::Key : (int)TFld::NoFlag;
	switch(s2i(tblStrct[iFld].tp)) {
	    case blr_varying:
	    case blr_varying2:
		cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,tblStrct[iFld].def.c_str()));
		break;
	    case blr_text:
	    case blr_text2:
	    case blr_blob:
		cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"1048576"));
		break;
	    case blr_short:
	    case blr_long:
	    case blr_int64:
		cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg));
		break;
	    case blr_float:
	    case blr_double:
	    case blr_d_float:
		cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg));
		break;
	}
    }
}

void MTable::fieldFix( TConfig &cfg )
{
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    bool appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !isEmpty());	//Only for append no present fields

    string pr_keys, ls;

    //DROP fields
    for(unsigned iFld = 0, iCf; iFld < tblStrct.size() && !appMode; iFld++) {
	for(iCf = 0; iCf < cf_el.size(); iCf++)
	    if(cf_el[iCf] == tblStrct[iFld].nm ||
		    ((cfg.cfg(cf_el[iCf]).fld().flg()&TFld::TransltText) && !cfg.cfg(cf_el[iCf]).noTransl() &&
		    tblStrct[iFld].nm.size() > 3 && tblStrct[iFld].nm.substr(2) == ("#"+cf_el[iCf]) && tblStrct[iFld].nm.compare(0,2,Mess->lang2CodeBase()) != 0))
	    {
		TCfg &cf = cfg.cfg(cf_el[iCf]);
		bool isEqual = false;
		int rwTp = s2i(tblStrct[iFld].tp);
		switch(cf.fld().type()) {
		    case TFld::String:
			if(rwTp == blr_varying && ((cf.fld().len() <= 255 && cf.fld().len() == s2i(tblStrct[iFld].def)) || cf.fld().flg()&TCfg::Key))
			    isEqual = true;
			else if(rwTp == blr_blob && cf.fld().len() > 255)	isEqual = true;
			break;
		    case TFld::Integer:	if(rwTp == blr_int64)	isEqual = true;	break;
		    case TFld::Real:	if(rwTp == blr_double)	isEqual = true;	break;
		    case TFld::Boolean:	if(rwTp == blr_short)	isEqual = true;	break;
		    default: break;
		}
		if(isEqual) break;
		continue;
	    }
	if(iCf >= cf_el.size()) {
	    ls += (ls.size()?", DROP \"":" DROP \"") + mod->sqlReqCode(tblStrct[iFld].nm,'"') + "\" ";
	    tblStrct.erase(tblStrct.begin()+iFld);
	    iFld--;
	}
    }

    //Add fields
    for(unsigned iCf = 0, iFld; iCf < cf_el.size(); iCf++) {
	TCfg &cf = cfg.cfg(cf_el[iCf]);
	// Check primary key
	if(cf.fld().flg()&TCfg::Key && !appMode)
	    pr_keys += (pr_keys.size()?",\"":"\"") + mod->sqlReqCode(cf.name(),'"') + "\"";

	for(iFld = 0; iFld < tblStrct.size(); iFld++)
	    if(cf_el[iCf] == tblStrct[iFld].nm) break;

	string f_tp;
	switch(cf.fld().type()) {
	    case TFld::String:
		if((cf.fld().len() && cf.fld().len() <= 255) || cf.fld().flg()&TCfg::Key)
		    f_tp = "VARCHAR(" + i2s(vmax(10,vmin(255,cf.fld().len()))) + ") ";
		else f_tp = "BLOB SUB_TYPE TEXT ";
		f_tp += "DEFAULT " + ((cf.fld().def()==EVAL_STR)?"NULL ":"'"+cf.fld().def()+"' NOT NULL ");
		break;
	    case TFld::Integer:
		f_tp = "BIGINT DEFAULT " + ((s2ll(cf.fld().def())==EVAL_INT)?"NULL ":"'"+ll2s(s2ll(cf.fld().def()))+"' NOT NULL ");
		break;
	    case TFld::Real:
		f_tp = "DOUBLE PRECISION DEFAULT " + ((s2r(cf.fld().def())==EVAL_REAL)?"NULL ":"'"+r2s(s2r(cf.fld().def()))+"' NOT NULL ");
		break;
	    case TFld::Boolean:
		f_tp = "SMALLINT DEFAULT " + ((s2i(cf.fld().def())==EVAL_BOOL)?"NULL ":"'"+i2s(s2i(cf.fld().def()))+"' NOT NULL ");
		break;
	    default: break;
	}

	// Add field
	if(iFld >= tblStrct.size())
	    ls += (ls.size()?", ADD \"":" ADD \"") + mod->sqlReqCode(cf_el[iCf],'"') + "\" " + f_tp;
	//Check other languages
	if(cf.fld().flg()&TFld::TransltText && !cf.noTransl()) {
	    unsigned iC;
	    for(iC = iFld; iC < tblStrct.size(); iC++)
		if(tblStrct[iC].nm.size() > 3 && tblStrct[iC].nm.substr(2) == ("#"+cf_el[iCf]) &&
		    tblStrct[iC].nm.compare(0,2,Mess->lang2CodeBase()) != 0 &&
		    tblStrct[iC].nm.compare(0,2,Mess->lang2Code()) == 0) break;
	    if(iC >= tblStrct.size() && Mess->translCfg())
		ls += (ls.size()?", ADD \"":" ADD \"") + mod->sqlReqCode(Mess->lang2Code()+"#"+cf_el[iCf],'"') + "\" "+f_tp;
	}
    }

    if(ls.size()) {
	//Prepare request for fix structure
	string req = "ALTER TABLE \"" + mod->sqlReqCode(name(),'"') + "\" " +
	    (isEmpty()?"":"DROP CONSTRAINT \"pk_"+mod->sqlReqCode(name(),'"')+"\", ") + ls +
	    (pr_keys.size()?", ADD CONSTRAINT \"pk_"+mod->sqlReqCode(name(),'"')+"\" PRIMARY KEY ("+pr_keys+") ":"");

	owner().sqlReq(req, NULL, false);
	owner().getStructDB(name(), tblStrct);	//Update structure information
    }
}

string MTable::getSQLVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";
    if(cfg.fld().type() == TFld::String) {
	//if(Mess->translDyn() && (cfg.fld().flg()&TFld::TransltText)) rez = trL(rez, Mess->lang2Code());
	rez = "'" + mod->sqlReqCode(rez /*(cfg.fld().len()>0)?rez.substr(0,cfg.fld().len()):rez*/) + "'";
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

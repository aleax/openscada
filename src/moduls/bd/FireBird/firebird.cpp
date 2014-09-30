
//OpenSCADA system module BD.FireBird file: firebird.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"0.9.7"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("DB module. Provides support of the DB FireBird.")
#define LICENSE		"GPL2"
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

void BDMod::load_( )	{ }

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
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&connRes, &attrM);
    pthread_mutexattr_destroy(&attrM);

    setAddr("localhost:/var/tmp/test.fbd");
}

MBD::~MBD( )
{
    pthread_mutex_destroy(&connRes);
}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag && owner().fullDeleteDB()) {
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
	    throw TError(nodePath().c_str(), _("Drop DB '%s' error: %s"), fdb.c_str(), getErr(status).c_str());
    }
}

void MBD::enable( )
{
    MtxAlloc res(connRes, true);
    if(enableStat()) return;

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
	    throw TError(nodePath().c_str(), _("Create DB '%s' error: %s"), fdb.c_str(), getErr(status).c_str());
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

void MBD::allowList( vector<string> &list )
{
    if(!enableStat()) return;
    list.clear();
    vector< vector<string> > tbl;
    sqlReq("SELECT rdb$relation_name FROM rdb$relations WHERE "
	"((rdb$system_flag = 0) OR (rdb$system_flag IS NULL)) AND "
	"(rdb$view_source IS NULL) ORDER BY rdb$relation_name", &tbl);
    for(unsigned i_t = 1; i_t < tbl.size(); i_t++)
	list.push_back(tbl[i_t][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw TError(nodePath().c_str(), _("Error open table '%s'. DB is disabled."), inm.c_str());

    return new MTable(inm, this, create);
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
	    mess_warning(nodePath().c_str(), _("Start transaction error: %s"), getErr(status).c_str());
	    return;
	    //throw TError(nodePath().c_str(), _("Start transaction error: %s"), getErr(status).c_str());
	}
	trOpenTm = SYS->sysTm();
    }
    reqCnt++;
    reqCntTm = SYS->sysTm();
}

void MBD::transCommit( )
{
    ISC_STATUS_ARRAY status;

    MtxAlloc res(connRes, true);
    if(!htrans) return;
    if(isc_commit_transaction(status, &htrans)) {
	mess_warning(nodePath().c_str(), _("DSQL close transaction error: %s"), getErr(status).c_str());
	return;
	//throw TError(nodePath().c_str(), _("DSQL close transaction error: %s"), getErr(status).c_str());
    }
    htrans = 0;
    reqCnt = reqCntTm = 0;
}

void MBD::transCloseCheck( )
{
    if(!enableStat() && toEnable()) enable();
    if(reqCnt && ((SYS->sysTm()-reqCntTm) > 10*60 || (SYS->sysTm()-trOpenTm) > 10*60)) transCommit();
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    if(tbl) tbl->clear();
    if(!enableStat()) return;

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
	ISC_STATUS rez = 0;
	//Prepare statement
	if((rez=isc_dsql_allocate_statement(status,&hdb,&stmt)))
	    mess_debug(nodePath().c_str(), _("Allocate statement error: (%d)%s"), rez, getErr(status).c_str());
	else if(!trans && (rez=isc_start_transaction(status,&trans,1,&hdb,0,NULL)))
	    mess_debug(nodePath().c_str(), _("Start transaction error: (%d)%s"), rez, getErr(status).c_str());
	//Prepare output data structure
	else if((rez=isc_dsql_prepare(status,&trans,&stmt,0,Mess->codeConvOut(cd_pg.c_str(),ireq).c_str(),3,NULL)))
	    mess_debug(nodePath().c_str(), _("DSQL prepare error: (%d)%s"), rez, getErr(status).c_str());
	else if(!rez && (rez=isc_dsql_describe(status,&stmt,1,out_sqlda)))
	    mess_debug(nodePath().c_str(), _("DSQL describe error: (%d)%s"), rez, getErr(status).c_str());
	//Reconnect try for error
	if(rez)
	    switch(rez)
	    {
		case isc_network_error:
		case isc_net_connect_err:
		case isc_net_connect_listen_err:
		    disable();
		    throw TError(nodePath().c_str(), _("Connect to DB error: %s"), getErr(status).c_str());
		default:
		    throw TError(nodePath().c_str(), _("Request error: %s"), getErr(status).c_str());
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
	    throw TError(nodePath().c_str(), _("DSQL execute error: %s"), getErr(status).c_str());
	if(tbl && out_sqlda->sqld) {
	    //if( isc_dsql_set_cursor_name(status, &stmt,"dyn_cursor", 0) )
	    //  throw TError(nodePath().c_str(),_("DSQL open cursor error: %s"),getErr(status).c_str());
	    vector<string> row;
	    long  fetch_stat;
	    while((fetch_stat=isc_dsql_fetch(status,&stmt,1,out_sqlda)) == 0)
	    {
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
		    if((var.sqltype&1) && (*var.sqlind < 0)) row.push_back("");
		    switch(var.sqltype & (~1))
		    {
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
			case SQL_BLOB:
			{
			    //> Read blob data
			    ISC_QUAD blob_id = *((ISC_QUAD*)var.sqldata);
			    ISC_STATUS blob_stat;
			    isc_blob_handle blob_handle = 0;
			    char blob_segment[STR_BUF_LEN];
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
	    throw TError(nodePath().c_str(), _("DSQL free statement error: %s"), getErr(status).c_str());
	}
	if(trans && !htrans && isc_commit_transaction(status,&trans)) {
	    stmt = trans = 0;
	    throw TError(nodePath().c_str(), _("DSQL close transaction error: %s"), getErr(status).c_str());
	}
    }
    catch(...) {
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

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,"help",
	    _("FireBird address to DB must be written as: \"{file};{user};{pass}[;{conTm}]\".\n"
	      "Where:\n"
	      "  file - full path to DB file in form: \"[{host}:]{filePath}\";\n"
	      "  user - DB user;\n"
	      "  pass - password of DB user;\n"
	      "  conTm- connection timeout (s)."));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//************************************************
//* FireBird::Table				 *
//************************************************
MTable::MTable( string inm, MBD *iown, bool create ) : TTable(inm)
{
    setNodePrev(iown);

    if(create) {
	string req = "EXECUTE BLOCK AS BEGIN "
	    "if (not exists(select 1 from rdb$relations where rdb$relation_name = '" + mod->sqlReqCode(name()) + "')) then "
	    "execute statement 'create table \"" + mod->sqlReqCode(name(),'"') + "\" (\"<<empty>>\" VARCHAR(20) NOT NULL, "
	    "CONSTRAINT \"pk_" + mod->sqlReqCode(name(),'"') + "\" PRIMARY KEY(\"<<empty>>\") )'; END";
	owner().sqlReq(req);
    }

    //Get table structure description
    getStructDB(tblStrct);
    if(tblStrct.size() <= 1) throw TError(nodePath().c_str(), _("Table '%s' is not present."), name().c_str());
}

MTable::~MTable( )	{ }

bool MTable::isEmpty( )	{ return tblStrct.empty() || tblStrct[1][0] == "<<empty>>"; }

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag) {
	try { owner().sqlReq("DROP TABLE \"" + mod->sqlReqCode(name(),'"') + "\""); }
	catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
    }
}

MBD &MTable::owner()	{ return (MBD&)TTable::owner(); }

void MTable::getStructDB( vector< vector<string> > &tblStrct )
{
    //Get generic data structure
    //owner().transCommit(/*&trans*/);
    owner().sqlReq("SELECT R.RDB$FIELD_NAME, F.RDB$FIELD_TYPE, F.RDB$FIELD_LENGTH "
	"FROM RDB$FIELDS F, RDB$RELATION_FIELDS R where F.RDB$FIELD_NAME = R.RDB$FIELD_SOURCE and "
	"R.RDB$SYSTEM_FLAG = 0 and R.RDB$RELATION_NAME = '"+mod->sqlReqCode(name())+"'", &tblStrct, false);
    if(tblStrct.size() > 1) {
	//Get keys
	vector< vector<string> > keyLst;
	owner().sqlReq("SELECT I.RDB$FIELD_NAME, C.RDB$CONSTRAINT_TYPE "
	    "FROM RDB$RELATION_CONSTRAINTS C, RDB$INDEX_SEGMENTS I "
	    "WHERE C.RDB$INDEX_NAME = I.RDB$INDEX_NAME AND C.RDB$RELATION_NAME = '"+
	    mod->sqlReqCode(name())+"'", &keyLst, false);
	tblStrct[0].push_back("Key");
	for(unsigned i_f = 1, i_k; i_f < tblStrct.size(); i_f++) {
	    for(i_k = 1; i_k < keyLst.size(); i_k++)
		if(tblStrct[i_f][0] == keyLst[i_k][0]) break;
	    tblStrct[i_f].push_back((i_k<keyLst.size()) ? keyLst[i_k][1] : "");
	}
    }
}

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty."));
    mLstUse = SYS->sysTm();

    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	string sid = tblStrct[i_fld][0];
	if(cfg.cfgPresent(sid)) continue;

	int flg = (tblStrct[i_fld][3]=="PRIMARY KEY") ? (int)TCfg::Key : (int)TFld::NoFlag;
	switch(s2i(tblStrct[i_fld][1]))
	{
	    case blr_varying:
	    case blr_varying2:
		cfg.elem().fldAdd(new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,tblStrct[i_fld][2].c_str()));
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
	    if(!cf.isKey() || !cf.getS().size()) continue;
	    unsigned i_fld = 1;
	    for( ; i_fld < tblStrct.size(); i_fld++)
		if(cf.name() == tblStrct[i_fld][0]) break;
	    if(i_fld >= tblStrct.size()) return false;
	}
    }

    //Make WHERE
    string req = "SELECT FIRST 1 SKIP " + i2s(row) + " ", req_where = "WHERE ", sid;
    // Add use keys to list
    bool first_sel = true, next = false, trPresent = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	sid = tblStrct[i_fld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?"AND \"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(getVal(*u_cfg)) + "' ";
	    next = true;
	}
	else if(u_cfg->isKey() || u_cfg->view()) {
	    req += (first_sel?"\"":",\"") + mod->sqlReqCode(sid,'"') + "\"";
	    first_sel = false;
	}
    }

    //Request
    if(first_sel) return false;
    req += " FROM \"" + mod->sqlReqCode(name(),'"') + "\" " + (next?req_where:"");
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) return false;
    //Processing of query
    for(unsigned i_fld = 0; i_fld < tbl[0].size(); i_fld++) {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) {
	    setVal(*u_cfg, tbl[1][i_fld]);
	    if(u_cfg->fld().flg()&TCfg::TransltText && !u_cfg->noTransl()) Mess->translReg(u_cfg->getS(), "db:"+fullDBName()+"#"+sid);
	}
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[1][i_fld].size()) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg) setVal(*u_cfg, tbl[1][i_fld]);
	}
    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty."));
    mLstUse = SYS->sysTm();

    //Prepare request
    string req = "SELECT ", req_where, sid, first_key;
    // Add fields list to queue
    bool first_sel = true, next_wr = false, trPresent = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	sid = tblStrct[i_fld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey()) {
	    req_where += (next_wr?"AND \"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(getVal(*u_cfg)) + "' ";
	    if(first_key.empty()) first_key = mod->sqlReqCode(sid, '"');
	    next_wr = true;
	}
	else if(u_cfg->view()) {
	    req += (first_sel?"\"":",\"") + mod->sqlReqCode(sid,'"') + "\"";
	    first_sel = false;
	}
    }
    if(first_sel) req += "\"" + first_key + "\"";
    req += " FROM \"" + mod->sqlReqCode(name(),'"') + "\" WHERE " + req_where;

    //Query
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) throw TError(nodePath().c_str(), _("Row \"%s\" is not present."), req_where.c_str());

    //Processing of query
    for(unsigned i_fld = 0; i_fld < tbl[0].size(); i_fld++) {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) {
	    setVal(*u_cfg, tbl[1][i_fld]);
	    if(u_cfg->fld().flg()&TCfg::TransltText && !u_cfg->noTransl()) Mess->translReg(u_cfg->getS(), "db:"+fullDBName()+"#"+sid);
	}
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[1][i_fld].size()) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg) setVal(*u_cfg, tbl[1][i_fld]);
	}
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    mLstUse = SYS->sysTm();
    if(tblStrct.empty()) fieldFix(cfg);

    string sid, sval;
    bool isVarTextTransl = (!Mess->lang2CodeBase().empty() && Mess->lang2Code() != Mess->lang2CodeBase());

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Check for translation present
    bool trPresent = isVarTextTransl, trDblDef = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	if(trPresent && (!isVarTextTransl || trDblDef)) break;
	sid = tblStrct[i_fld][0];
	if(sid.size() > 3) {
	    if(!trPresent && !Mess->translDyn() && sid.compare(0,3,Mess->lang2Code()+"#") == 0) trPresent = true;
	    if(Mess->lang2Code() == Mess->lang2CodeBase() && !trDblDef && sid.compare(0,3,Mess->lang2CodeBase()+"#") == 0)
		trDblDef = true;
	}
    }
    if(trDblDef && !cfg.reqKeys()) fieldFix(cfg);

    //Get present fields list
    string req_where = "WHERE ";
    // Add key list to queue
    bool next = false, noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();		//Force update by ReqKeys or reqKey() or keyUpdt() present
    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if(!u_cfg.isKey()) continue;
	if(!next) next = true; else req_where = req_where + "AND ";
	req_where += "\"" + mod->sqlReqCode(cf_el[i_el],'"') + "\"='" +
			    mod->sqlReqCode(u_cfg.getS(TCfg::KeyUpdtBase)/*getVal(u_cfg)*/) + "' ";

	if(!isForceUpdt && u_cfg.keyUpdt()) isForceUpdt = true;

	// Check for no key fields
	if(noKeyFld) continue;
	unsigned i_fld = 1;
	for( ; i_fld < tblStrct.size(); i_fld++)
	    if(u_cfg.name() == tblStrct[i_fld][0]) break;
	if(i_fld >= tblStrct.size()) noKeyFld = true;
    }
    if(noKeyFld) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }

    //Prepare query
    string req;
    if(!isForceUpdt) {
	req = "SELECT 1 FROM \"" + mod->sqlReqCode(name(),'"') + "\" " + req_where;
	owner().sqlReq(req, &tbl, true);
	if(tbl.size() < 2) {
	    // Add new record
	    req = "INSERT INTO \"" + mod->sqlReqCode(name(),'"') + "\" ";
	    string ins_name, ins_value;
	    next = false;
	    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
		TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
		if(!u_cfg.isKey() && !u_cfg.view()) continue;

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
	else isForceUpdt = true;
    }
    if(isForceUpdt) {
	// Update present record
	req = "UPDATE \"" + mod->sqlReqCode(name(),'"') + "\" SET ";
	next = false;
	for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if((u_cfg.isKey() && !u_cfg.keyUpdt()) || !u_cfg.view()) continue;
	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    sval = getVal(u_cfg);
	    req += (next?",\"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(sval) + "' ";
	    next = true;
	}
	req += req_where;
    }

    //Query
    try{ owner().sqlReq(req, NULL, true); }
    catch(TError err) { fieldFix(cfg); owner().sqlReq(req, NULL, true); }
}

void MTable::fieldDel( TConfig &cfg )
{
    if(tblStrct.empty()) return;
    mLstUse = SYS->sysTm();

    //Where prepare
    string req_where = "WHERE ";
    bool next = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	string sid = tblStrct[i_fld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?"AND \"":"\"") + mod->sqlReqCode(sid,'"') + "\"='" + mod->sqlReqCode(getVal(*u_cfg)) + "' ";
	    next = true;
	}
    }

    //Main request
    owner().sqlReq("DELETE FROM \""+mod->sqlReqCode(name(),'"')+"\" "+req_where, NULL, true);
}

void MTable::fieldFix( TConfig &cfg )
{
    bool next = false, next_key = false;

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    bool appMode = cfg.reqKeys() || (cfg.incomplTblStruct() && !isEmpty()),	//Only for append no present fields
	 isVarTextTransl = (!Mess->lang2CodeBase().empty() && Mess->lang2Code() != Mess->lang2CodeBase());

    //Prepare request for fix structure
    string req = "ALTER TABLE \"" + mod->sqlReqCode(name(),'"') + "\" ";
    if(!appMode) req += "DROP CONSTRAINT \"pk_" + mod->sqlReqCode(name(),'"') + "\", ";

    //DROP fields
    for(unsigned i_fld = 1, i_cf; i_fld < tblStrct.size() && !appMode; i_fld++) {
	for(i_cf = 0; i_cf < cf_el.size(); i_cf++)
	    if(cf_el[i_cf] == tblStrct[i_fld][0] ||
		    (cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::TransltText && tblStrct[i_fld][0].size() > 3 &&
		    tblStrct[i_fld][0].substr(2) == ("#"+cf_el[i_cf]) && tblStrct[i_fld][0].compare(0,2,Mess->lang2CodeBase()) != 0))
	    {
		TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		bool isEqual = false;
		int rwTp = s2i(tblStrct[i_fld][1]);
		switch(u_cfg.fld().type()) {
		    case TFld::String:
			if(rwTp == blr_varying && (u_cfg.fld().len() <= 255 || u_cfg.fld().flg()&TCfg::Key) &&
				u_cfg.fld().len() == s2i(tblStrct[i_fld][2]))
			    isEqual = true;
			else if(rwTp == blr_blob && u_cfg.fld().len() > 255)	isEqual = true;
			break;
		    case TFld::Integer:	if(rwTp == blr_int64)	isEqual = true;	break;
		    case TFld::Real:	if(rwTp == blr_double)	isEqual = true;	break;
		    case TFld::Boolean:	if(rwTp == blr_short)	isEqual = true;	break;
		    default: break;
		}
		if(isEqual) break;
		continue;
	    }
	if(i_cf >= cf_el.size()) {
	    req += (next?",DROP \"":"DROP \"") + mod->sqlReqCode(tblStrct[i_fld][0],'"') + "\" ";
	    tblStrct.erase(tblStrct.begin()+i_fld);
	    i_fld--;
	    next = true;
	}
    }

    string pr_keys;
    //Add fields
    for(unsigned i_cf = 0, i_fld; i_cf < cf_el.size(); i_cf++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
	// Check primary key
	if(u_cfg.fld().flg()&TCfg::Key && !appMode) {
	    pr_keys += (next_key?",\"":"\"") + mod->sqlReqCode(u_cfg.name(),'"') + "\"";
	    next_key = true;
	}

	for(i_fld = 1; i_fld < tblStrct.size(); i_fld++)
	    if(cf_el[i_cf] == tblStrct[i_fld][0]) break;

	string f_tp;
	switch(u_cfg.fld().type()) {
	    case TFld::String:
		if((u_cfg.fld().len() && u_cfg.fld().len() <= 255) || u_cfg.fld().flg()&TCfg::Key)
		    f_tp = "VARCHAR(" + i2s(vmax(10,vmin(255,u_cfg.fld().len()))) + ") DEFAULT '" + u_cfg.fld().def() + "' NOT NULL ";
		else f_tp = "BLOB SUB_TYPE TEXT DEFAULT '" + u_cfg.fld().def() + "' NOT NULL ";
		break;
	    case TFld::Integer:	f_tp = "BIGINT DEFAULT '" + i2s(s2i(u_cfg.fld().def())) + "' NOT NULL ";			break;
	    case TFld::Real:	f_tp = "DOUBLE PRECISION DEFAULT '" + r2s(s2r(u_cfg.fld().def())) + "' NOT NULL ";	break;
	    case TFld::Boolean:	f_tp = "SMALLINT DEFAULT '" + i2s(s2i(u_cfg.fld().def())) + "' NOT NULL ";		break;
	    default: break;
	}

	// Add field
	if(i_fld >= tblStrct.size()) {
	    req += (next?",ADD \"":"ADD \"") + mod->sqlReqCode(cf_el[i_cf],'"') + "\" " + f_tp;
	    next = true;
	}
	//Check other languages
	if(u_cfg.fld().flg()&TCfg::TransltText) {
	    unsigned i_c;
	    for(i_c = i_fld; i_c < tblStrct.size(); i_c++)
		if(tblStrct[i_c][0].size() > 3 && tblStrct[i_c][0].substr(2) == ("#"+cf_el[i_cf]) &&
		    tblStrct[i_c][0].compare(0,2,Mess->lang2CodeBase()) != 0 &&
		    tblStrct[i_c][0].compare(0,2,Mess->lang2Code()) == 0) break;
	    if(i_c >= tblStrct.size() && isVarTextTransl) {
		req += (next?",ADD \"":"ADD \"") + mod->sqlReqCode(Mess->lang2Code()+"#"+cf_el[i_cf],'"') + "\" "+f_tp;
		next = true;
	    }
	}
    }

    if(pr_keys.size()) req += ", ADD CONSTRAINT \"pk_" + mod->sqlReqCode(name(),'"') + "\" PRIMARY KEY (" + pr_keys + ") ";

    if(next) {
	owner().sqlReq(req, NULL, false);
	getStructDB(tblStrct);		//Update structure information
    }
}

string MTable::getVal( TCfg &cfg )	{ return cfg.getS(); }

void MTable::setVal( TCfg &cfg, const string &val )	{ cfg.setS(val); }

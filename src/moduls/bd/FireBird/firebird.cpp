
//OpenSCADA system module BD.FireBird file: firebird.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <getopt.h>
#include <errno.h>
#include <ibase.h>

#include <tsys.h>
#include <tmess.h>
#include "firebird.h"

//******************************************************************************
//* Modul info!                                                                *
#define MOD_ID		"FireBird"
#define MOD_NAME	"DB FireBird"
#define MOD_TYPE	"BD"
#define VER_TYPE	VER_BD
#define VERSION		"0.9.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"DB modul. Allow support of the DB FireBird."
#define LICENSE		"GPL"
//******************************************************************************

FireBird::BDMod *FireBird::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new FireBird::BDMod( source );
	return NULL;
    }
}

using namespace FireBird;

//*************************************************
//* FireBird::BDMod				  *
//*************************************************
BDMod::BDMod(string name)
{
    mId 	= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

BDMod::~BDMod( )
{
    nodeDelAll();
}

TBD *BDMod::openBD( const string &iid )
{
    return new MBD(iid,&owner().openDB_E());
}

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void BDMod::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //- Load parameters from config file -=

}

string BDMod::sqlReqCode( const string &req, char symb )
{
    string sout = req;
    for( int i_sz = 0; i_sz < sout.size(); i_sz++ )
	if( sout[i_sz] == symb ) sout.replace(i_sz++,1,2,symb);
    return sout;
}


//************************************************
//* FireBird::MBD				 *
//************************************************
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), hdb(0)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable(int flag)
{
    TBD::postDisable(flag);

    if( flag && owner().fullDeleteDB() )
    {
	//- Attach to DB -
	//-- Prepare database parameter buffer (DPB) --
	char  *dpb = (char *)malloc(50);
	short dpb_length = 0;
	isc_modify_dpb(&dpb, &dpb_length, isc_dpb_user_name, user.c_str(),user.size());
	isc_modify_dpb(&dpb, &dpb_length, isc_dpb_password, pass.c_str(),pass.size());

	ISC_STATUS_ARRAY status;
	isc_tr_handle trans = 0;
	if( isc_attach_database( status, 0, fdb.c_str(), &hdb, dpb_length, dpb) ) return;
	if( isc_drop_database(status, &hdb) ) 
	    throw TError(TSYS::DBOpen,nodePath().c_str(),_("Drop DB '%s' error: %s"),fdb.c_str(),getErr(status).c_str());
    }
}

void MBD::enable( )
{
    if( enableStat() )  return;

    //- DB address parsing -
    fdb   = TSYS::strSepParse(addr(),0,';');
    user  = TSYS::strSepParse(addr(),1,';');
    pass  = TSYS::strSepParse(addr(),2,';');
    cd_pg = codePage().size()?codePage():Mess->charset();

    //- Attach to DB -
    //-- Prepare database parameter buffer (DPB) --
    char  *dpb = (char *)malloc(50);
    short dpb_length = 0;
    isc_modify_dpb(&dpb, &dpb_length, isc_dpb_user_name, user.c_str(),user.size());
    isc_modify_dpb(&dpb, &dpb_length, isc_dpb_password, pass.c_str(),pass.size());

    ISC_STATUS_ARRAY status;
    if( isc_attach_database( status, 0, fdb.c_str(), &hdb, dpb_length, dpb) )
    {
	//--- Make try for create DB ---
	isc_tr_handle trans = 0;
	if( isc_dsql_execute_immediate(status, &hdb, &trans, 0, 
		    ("CREATE DATABASE '"+fdb+"' USER '"+user+"' PASSWORD '"+pass+"'").c_str(), 3, NULL) )
	    throw TError(TSYS::DBOpen,nodePath().c_str(),_("Create DB '%s' error: %s"),fdb.c_str(),getErr(status).c_str());
    }

    TBD::enable( );
}

void MBD::disable( )
{
    if( !enableStat() )  return;

    TBD::disable( );

    ResAlloc res(conn_res,true);
    ISC_STATUS_ARRAY status;
    isc_detach_database( status, &hdb );
    hdb = 0;
}

void MBD::allowList( vector<string> &list )
{
    if( !enableStat() )  return;
    list.clear();
    vector< vector<string> > tbl;
    sqlReq("SELECT rdb$relation_name FROM rdb$relations WHERE "
	"((rdb$system_flag = 0) OR (rdb$system_flag IS NULL)) AND " 
	"(rdb$view_source IS NULL) ORDER BY rdb$relation_name",&tbl);
    for( int i_t = 1; i_t < tbl.size(); i_t++ )
        list.push_back(tbl[i_t][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if( !enableStat() )
	throw TError(TSYS::DBOpenTable,nodePath().c_str(),_("Error open table <%s>. DB is disabled."),inm.c_str());

    return new MTable(inm,this,create);
}

string MBD::getErr( ISC_STATUS_ARRAY status )
{
    string err;
    char msg[512];

    err=TSYS::int2str(isc_sqlcode(status))+":";
    while( fb_interpret( msg, sizeof(msg), (const ISC_STATUS**)&status ) )
	err += string("-")+msg;

    return err;
}

void MBD::transOpen( isc_tr_handle *trans )
{
    ISC_STATUS_ARRAY status;

    ResAlloc res(conn_res,true);
    if( !trans || *trans ) return;
    if( isc_start_transaction(status, trans, 1, &hdb, 0, NULL) )
	throw TError(TSYS::DBRequest,nodePath().c_str(),_("Start trasaction error: %s"),getErr(status).c_str());
}

void MBD::transCommit( isc_tr_handle *trans )
{
    ISC_STATUS_ARRAY status;

    ResAlloc res(conn_res,true);
    if( !trans || !(*trans) ) return;
    if( isc_commit_transaction(status, trans) )
	throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL close transaction error: %s"),getErr(status).c_str());
    *trans = 0;
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl )
{
    sqlReq(NULL,ireq,tbl);
}

void MBD::sqlReq( isc_tr_handle *itrans, const string &ireq, vector< vector<string> > *tbl )
{
    if( tbl ) tbl->clear();
    if(!enableStat()) return;

    ResAlloc res(conn_res,true);

    XSQLDA  *out_sqlda = (XSQLDA *)malloc(XSQLDA_LENGTH(10));
    out_sqlda->version = SQLDA_VERSION1;
    out_sqlda->sqln = 10;
    char    *dtBuf = NULL;
    int      dtBufLen = 0;
    isc_stmt_handle stmt = 0;
    isc_tr_handle trans = (itrans && *itrans) ? (*itrans) : 0;
    ISC_STATUS_ARRAY status;

    try
    {
	//- Prepare statement -
	if( isc_dsql_allocate_statement(status, &hdb, &stmt) )
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("Allocate statement error: %s"),getErr(status).c_str());
	if( !trans && isc_start_transaction(status, &trans, 1, &hdb, 0, NULL) )
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("Start trasaction error: %s"),getErr(status).c_str());
	//- Prepare output data structure -
	if( isc_dsql_prepare(status, &trans, &stmt, 0, Mess->codeConvOut(cd_pg.c_str(),ireq).c_str(), 3, NULL) )
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL prepare error: %s"),getErr(status).c_str());
	if( isc_dsql_describe(status, &stmt, 1, out_sqlda) )
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL describe error: %s"),getErr(status).c_str());
	if( out_sqlda->sqld > out_sqlda->sqln )
	{
	    int n = out_sqlda->sqld;
	    free(out_sqlda);
	    out_sqlda = (XSQLDA *)malloc(XSQLDA_LENGTH(n));
	    out_sqlda->sqln = n;
	    out_sqlda->version = SQLDA_VERSION1;
	    isc_dsql_describe(status, &stmt, 1, out_sqlda);
	}
	//-- Setup SQLDA --
	//--- Data buffer size calc ---
	for( int i = 0; i < out_sqlda->sqld; i++ )
	{
	    dtBufLen += out_sqlda->sqlvar[i].sqllen + sizeof(short);
	    if( ((out_sqlda->sqlvar[i].sqltype & ~1) == SQL_VARYING) )
		dtBufLen += sizeof(short);
	}
	if( dtBufLen )
	{
	    dtBuf = (char*)malloc(dtBufLen);
	    for( int i = 0, off = 0; i < out_sqlda->sqld; i++ )
	    {
		out_sqlda->sqlvar[i].sqldata = dtBuf+off;
		off += out_sqlda->sqlvar[i].sqllen;
		if( ((out_sqlda->sqlvar[i].sqltype & ~1) == SQL_VARYING) )
		    off += sizeof(short);
		out_sqlda->sqlvar[i].sqlind  = (short*)(dtBuf+off);
		off += sizeof(short);
	    }
	}

	//-- Get data --
	if( isc_dsql_execute(status, &trans, &stmt, 1, NULL) )
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL execute error: %s"),getErr(status).c_str());
	if( tbl && out_sqlda->sqld )
	{
	    //if( isc_dsql_set_cursor_name(status, &stmt,"dyn_cursor", 0) )
	    //  throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL open cursor error: %s"),getErr(status).c_str());
	    vector<string> row;
	    long  fetch_stat;
	    while( (fetch_stat = isc_dsql_fetch(status, &stmt, 1, out_sqlda)) == 0 )
	    {
		//-- Add head --
		if( row.empty() )
		{
		    for( int i = 0; i < out_sqlda->sqld; i++ )
			row.push_back(out_sqlda->sqlvar[i].sqlname);
		    tbl->push_back(row);
		}
		//-- Add row --
		row.clear();
		for( int i = 0; i < out_sqlda->sqld; i++ )
		{
		    XSQLVAR &var = out_sqlda->sqlvar[i];
		    // Null handling.  If the column is nullable and null
		    if( (var.sqltype & 1) &&
			    (*var.sqlind < 0) ) row.push_back("");
		    switch( var.sqltype & ~1 )
		    {
			case SQL_TEXT:
			    row.push_back(Mess->codeConvIn(cd_pg.c_str(),clrEndSpace(string(var.sqldata,var.sqllen))));
			    break;
			case SQL_VARYING:
			    row.push_back(Mess->codeConvIn(cd_pg.c_str(),string((char*)var.sqldata+sizeof(short),*(short*)var.sqldata)));
			    break;
			case SQL_SHORT:
			    row.push_back(TSYS::int2str(*(short *)var.sqldata));
			    break;
			case SQL_LONG:
			    row.push_back(TSYS::int2str(*(int *)var.sqldata));
			    break;
			case SQL_INT64:
			    row.push_back(TSYS::ll2str(*(ISC_INT64 *)var.sqldata));
			    break;
			case SQL_FLOAT:
			    row.push_back(TSYS::real2str(*(float *)var.sqldata));
			    break;
			case SQL_DOUBLE:
			    row.push_back(TSYS::real2str(*(double *)var.sqldata));
			    break;
			case SQL_BLOB:
			{
			    //- Read blob data -
			    ISC_QUAD blob_id = *((ISC_QUAD*)var.sqldata);
			    ISC_STATUS blob_stat;
			    isc_blob_handle blob_handle = 0;
			    char blob_segment[STR_BUF_LEN];
			    unsigned short actual_seg_len;
			    string bval;
			    isc_open_blob2( status, &hdb, &trans, &blob_handle, &blob_id, 0, NULL );
			    do
			    {
				blob_stat = isc_get_segment( status, &blob_handle, &actual_seg_len, sizeof(blob_segment), blob_segment );
				bval.append(blob_segment,actual_seg_len);
			    }
			    while( blob_stat == 0 && status[1] == isc_segment );
			    isc_close_blob( status, &blob_handle );
			    row.push_back(bval);
			    break;
			}
			default:
			    row.push_back("");
		    }
		}
		tbl->push_back(row);
	    }
	}
	if( isc_dsql_free_statement(status, &stmt, DSQL_drop) )
	{
	    stmt = 0;
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL free statement error: %s"),getErr(status).c_str());
	}
	if( (!itrans || !(*itrans)) && isc_commit_transaction(status, &trans) )
	{
	    stmt = trans = 0;
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("DSQL close transaction error: %s"),getErr(status).c_str());
	}
    }catch(...)
    {
	if( stmt ) isc_dsql_free_statement(status, &stmt, DSQL_drop);
	if( trans && (!itrans || !(*itrans)) ) isc_commit_transaction(status, &trans);
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
    while( i >= 0 && (vl[i]==' ' || vl[i]=='\t' || vl[i]=='\n') ) i--;
    return vl.substr(0,i+1);
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","BD",2,
	    "tp","str","help",
	    _("FireBird address to DB must be written as: [<file>;<user>;<pass>].\n"
	      "Where:\n"
	      "  file - full DB file;\n"
	      "  user - DB user;\n"
	      "  pass - password of DB user."));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//************************************************
//* FireBird::Table				 *
//************************************************
MTable::MTable(string inm, MBD *iown, bool create ) : TTable(inm), trans(0)
{
    setNodePrev(iown);

    if( create )
    {
	string req = "EXECUTE BLOCK AS BEGIN "
	    "if (not exists(select 1 from rdb$relations where rdb$relation_name = '"+mod->sqlReqCode(name())+"')) then "
	    "execute statement 'create table \""+mod->sqlReqCode(name(),'"')+"\" ( \"name\" VARCHAR(20) NOT NULL, "
	    "CONSTRAINT \"pk_"+mod->sqlReqCode(name(),'"')+"\" PRIMARY KEY(\"name\") )'; END";
	owner().sqlReq(req);
    }
    //- Get table structure description -
    getStructDB( tblStrct );
    if( tblStrct.size() <= 1 )
	throw TError(TSYS::DBOpenTable,nodePath().c_str(),_("Table '%s' is not present."),name().c_str());
}

MTable::~MTable(  )
{

}

void MTable::postDisable(int flag)
{
    owner().transCommit(&trans);
    if( flag )
    {
	try
	{ owner().sqlReq("DROP TABLE \""+mod->sqlReqCode(name(),'"')+"\""); }
	catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

MBD &MTable::owner()	{ return (MBD&)TTable::owner(); }

void MTable::getStructDB( vector< vector<string> > &tblStrct )
{
    //- Get generic data structure -
    owner().transCommit(&trans);
    owner().sqlReq("SELECT R.RDB$FIELD_NAME, F.RDB$FIELD_TYPE, F.RDB$FIELD_LENGTH "
	"FROM RDB$FIELDS F, RDB$RELATION_FIELDS R where F.RDB$FIELD_NAME = R.RDB$FIELD_SOURCE and "
	"R.RDB$SYSTEM_FLAG = 0 and R.RDB$RELATION_NAME = '"+mod->sqlReqCode(name())+"'",&tblStrct);
    if( tblStrct.size( ) > 1 )
    {
	//- Get keys -
	vector< vector<string> > keyLst;
	owner().sqlReq(&trans,"SELECT I.RDB$FIELD_NAME, C.RDB$CONSTRAINT_TYPE "
	    "FROM RDB$RELATION_CONSTRAINTS C, RDB$INDEX_SEGMENTS I "
	    "WHERE C.RDB$INDEX_NAME = I.RDB$INDEX_NAME AND C.RDB$RELATION_NAME = '"+
	    mod->sqlReqCode(name())+"'",&keyLst);
	tblStrct[0].push_back("Key");
	for( int i_f = 1; i_f < tblStrct.size(); i_f++ )
	{
	    int i_k;
	    for( i_k = 1; i_k < keyLst.size(); i_k++ )
		if( tblStrct[i_f][0] == keyLst[i_k][0] ) break;
	    tblStrct[i_f].push_back( (i_k<keyLst.size()) ? keyLst[i_k][1] : "" );
	}
    }
}

void MTable::fieldStruct( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty."));
    mLstUse = time(NULL);

    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	string sid = tblStrct[i_fld][0];
	if( cfg.cfgPresent(sid) ) continue;

	int flg = (tblStrct[i_fld][3]=="PRIMARY KEY") ? (int)TCfg::Key : (int)TFld::NoFlag;
	if( tblStrct[i_fld][1] == "37" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,tblStrct[i_fld][2].c_str()) );
	else if( tblStrct[i_fld][1] == "261" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"1048576") );
	else if( tblStrct[i_fld][1] == "8" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg) );
	else if( tblStrct[i_fld][1] == "27" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg) );
	else if( tblStrct[i_fld][1] == "7" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Boolean,flg) );
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty."));
    mLstUse = time(NULL);

    owner().transCommit(&trans);
    //> Make WHERE
    string req = "SELECT FIRST 1 SKIP "+TSYS::int2str(row)+" ";
    string req_where = "WHERE ";
    //>> Add use keys to list
    bool first_sel = true;
    bool next = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	string sid = tblStrct[i_fld][0];
	if( !cfg.cfgPresent(sid) ) continue;
	TCfg &u_cfg = cfg.cfg(sid);
	if( u_cfg.fld().flg()&TCfg::Key && u_cfg.keyUse() )
	{
	    if( !next ) next = true;
	    else req_where=req_where+"AND ";
	    req_where=req_where+"\""+mod->sqlReqCode(sid,'"')+"\"='"+mod->sqlReqCode(getVal(u_cfg))+"' ";
	}
	else if( u_cfg.fld().flg()&TCfg::Key || u_cfg.view() )
	{
	    if( first_sel ) req=req+"\""+mod->sqlReqCode(sid,'"')+"\"";
	    else req=req+",\""+mod->sqlReqCode(sid,'"')+"\"";
	    first_sel = false;
	}
    }

    //> Request
    req = req+" FROM \""+mod->sqlReqCode(name(),'"')+"\" "+((next)?req_where:"");
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) return false;
    //> Processing of query
    for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
	if( cfg.cfgPresent(tbl[0][i_fld]) )
	    setVal(cfg.cfg(tbl[0][i_fld]),tbl[1][i_fld]);

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty."));
    mLstUse = time(NULL);

    owner().transCommit(&trans);
    //> Prepare request
    string req = "SELECT ";
    string req_where;
    //>> Add fields list to queue
    bool first_sel = true;
    bool next_wr = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	if( !cfg.cfgPresent(tblStrct[i_fld][0]) ) continue;
	TCfg &u_cfg = cfg.cfg(tblStrct[i_fld][0]);
	if( u_cfg.fld().flg()&TCfg::Key )
	{
	    if( !next_wr ) next_wr = true; else req_where=req_where+"AND ";
	    req_where=req_where+"\""+mod->sqlReqCode(tblStrct[i_fld][0],'"')+"\"='"+mod->sqlReqCode(getVal(u_cfg))+"'";
	}
	else if( u_cfg.view() )
	{
	    if( first_sel ) req=req+"\""+mod->sqlReqCode(tblStrct[i_fld][0],'"')+"\"";
	    else req=req+",\""+mod->sqlReqCode(tblStrct[i_fld][0],'"')+"\"";
	    first_sel = false;
	}
    }
    req = req+" FROM \""+mod->sqlReqCode(name(),'"')+"\" WHERE "+req_where;

    //> Query
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) throw TError(TSYS::DBRowNoPresent,nodePath().c_str(),_("Row is not present."));

    //> Processing of query
    for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
	if( cfg.cfgPresent(tbl[0][i_fld]) )
	    setVal(cfg.cfg(tbl[0][i_fld]),tbl[1][i_fld]);
}

void MTable::fieldSet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    mLstUse = time(NULL);
    if( tblStrct.empty() ) fieldFix(cfg);

    owner().transOpen(&trans);
    //> Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //> Get present fields list
    string req_where = "WHERE ";
    //>> Add key list to queue
    bool next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( !(u_cfg.fld().flg()&TCfg::Key) ) continue;
	if( !next ) next = true; else req_where=req_where+"AND ";
	req_where=req_where+"\""+mod->sqlReqCode(cf_el[i_el],'"')+"\"='"+mod->sqlReqCode(getVal(u_cfg))+"' ";
    }

    //> Prepare query
    //>> Add new record
    string reqi = "INSERT INTO \""+mod->sqlReqCode(name(),'"')+"\" ";
    string ins_name, ins_value;
    next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( !(u_cfg.fld().flg()&TCfg::Key) && !u_cfg.view() ) continue;

	if( !next ) next = true;
	else
	{
	    ins_name=ins_name+",";
	    ins_value=ins_value+",";
	}
	ins_name=ins_name+"\""+mod->sqlReqCode(cf_el[i_el],'"')+"\" ";
	ins_value=ins_value+"'"+mod->sqlReqCode(getVal(u_cfg))+"' ";
    }
    reqi = reqi + "("+ins_name+") VALUES ("+ins_value+")";
    try{ owner().sqlReq( &trans, reqi ); }
    catch(TError err)
    {
	//>> Update present record
	string requ = "UPDATE \""+mod->sqlReqCode(name(),'"')+"\" SET ";
	next = false;
	for( int i_el = 0; i_el < cf_el.size(); i_el++ )
	{
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if( u_cfg.fld().flg()&TCfg::Key || !u_cfg.view() ) continue;

	    if( !next ) next = true; else requ=requ+",";
	    requ=requ+"\""+mod->sqlReqCode(cf_el[i_el],'"')+"\"='"+mod->sqlReqCode(getVal(u_cfg))+"' ";
	}
	requ = requ + req_where;
	try{ owner().sqlReq( &trans, requ ); }
	catch( TError err )
	{
	    fieldFix(cfg);
	    try{ owner().sqlReq( &trans, reqi ); }
	    catch(TError err)	{ owner().sqlReq( &trans, requ ); }
	}
    }
}

void MTable::fieldDel( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty."));
    mLstUse = time(NULL);

    owner().transOpen(&trans);
    //> Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //> Prepare request
    string req = "DELETE FROM \""+mod->sqlReqCode(name(),'"')+"\" WHERE ";
    //>> Add key list to queue
    bool next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( u_cfg.fld().flg()&TCfg::Key && u_cfg.keyUse() )
	{
	    if( !next ) next = true; else req=req+"AND ";
	    req=req+"\""+mod->sqlReqCode(cf_el[i_el],'"')+"\"='"+mod->sqlReqCode(getVal(u_cfg))+"' ";
	}
    }
    owner().sqlReq( &trans, req );
}

void MTable::fieldFix( TConfig &cfg )
{
    bool next = false, next_key = false;

    owner().transCommit(&trans);

    //- Get config fields list -
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //- Prepare request for fix structure -
    string req = "ALTER TABLE \""+mod->sqlReqCode(name(),'"')+"\" "
    "DROP CONSTRAINT \"pk_"+mod->sqlReqCode(name(),'"')+"\", ";

    string pr_keys;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
	//-- Check primary key --
	if( u_cfg.fld().flg()&TCfg::Key )
	{
	    if( !next_key ) next_key = true;
	    else pr_keys=pr_keys+",";
	    pr_keys=pr_keys+"\""+mod->sqlReqCode(u_cfg.name(),'"')+"\"";
	}
	int i_fld;
	for( i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
	    if( cf_el[i_cf] == tblStrct[i_fld][0] )     break;
	if( i_fld < tblStrct.size() )
	{
	    string f_tp;
	    switch(u_cfg.fld().type())
	    {
		case TFld::String:
		    if( tblStrct[i_fld][1] == "37" && (u_cfg.fld().len() <= 255 || u_cfg.fld().flg()&TCfg::Key) &&
			    u_cfg.fld().len() == atoi(tblStrct[i_fld][2].c_str()) )
			continue;
		    else if( tblStrct[i_fld][1] == "261" && u_cfg.fld().len() > 255 )	continue;
		    break;
		case TFld::Integer:	if( tblStrct[i_fld][1] == "8" )	continue;	break;
		case TFld::Real:	if( tblStrct[i_fld][1] == "27" )continue;	break;
		case TFld::Boolean:	if( tblStrct[i_fld][1] == "7" ) continue;	break;
	    }
	    //-- Change field --
	    if( !next ) next = true; else req=req+",";
	    req=req+"DROP \""+mod->sqlReqCode(cf_el[i_cf],'"')+"\" ";
	}
	if( !next ) next = true; else req=req+",";
	//-- Add field --
	req=req+"ADD \""+mod->sqlReqCode(cf_el[i_cf],'"')+"\" ";
	switch(u_cfg.fld().type())
	{
	    case TFld::String:
		if( u_cfg.fld().len() <= 255 || u_cfg.fld().flg()&TCfg::Key )
		    req=req+"VARCHAR("+SYS->int2str(vmax(10,vmin(255,u_cfg.fld().len())))+") DEFAULT '"+u_cfg.fld().def()+"' NOT NULL ";
		else  req=req+"BLOB SUB_TYPE TEXT DEFAULT '"+u_cfg.fld().def()+"' NOT NULL ";
		break;
	    case TFld::Integer:
		req=req+"INTEGER DEFAULT '"+TSYS::int2str(atoi(u_cfg.fld().def().c_str()))+"' NOT NULL ";
		break;
	    case TFld::Real:
		req=req+"DOUBLE PRECISION DEFAULT '"+TSYS::real2str(atof(u_cfg.fld().def().c_str()))+"' NOT NULL ";
		break;
	    case TFld::Boolean:
		req=req+"SMALLINT DEFAULT '"+TSYS::int2str(atoi(u_cfg.fld().def().c_str()))+"' NOT NULL ";
		break;
	}
    }
    //- DROP fields -
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
	if( !cfg.cfgPresent(tblStrct[i_fld][0]) )
	{
	    if( !next ) next = true; else req=req+",";
	    req=req+"DROP \""+mod->sqlReqCode(tblStrct[i_fld][0],'"')+"\" ";
	}
    req=req+", ADD CONSTRAINT \"pk_"+mod->sqlReqCode(name(),'"')+"\" PRIMARY KEY ("+pr_keys+") ";

    if( next )
    {
	owner().sqlReq( req );
	getStructDB( tblStrct );
    }

    owner().transOpen(&trans);
}

string MTable::getVal( TCfg &cfg )
{
    switch( cfg.fld().type() )
    {
	case TFld::String:	return cfg.getS();
	case TFld::Integer:	return SYS->int2str(cfg.getI());
	case TFld::Real:	return SYS->real2str(cfg.getR());
	case TFld::Boolean:	return SYS->int2str(cfg.getB());
    }
    return "";
}

void MTable::setVal( TCfg &cfg, const string &val )
{
    switch( cfg.fld().type() )
    {
	case TFld::String:	cfg.setS(val);	break;
	case TFld::Integer:	cfg.setI(atoi(val.c_str()));	break;
	case TFld::Real:	cfg.setR(atof(val.c_str()));	break;
	case TFld::Boolean:	cfg.setB(atoi(val.c_str()));	break;
    }
}

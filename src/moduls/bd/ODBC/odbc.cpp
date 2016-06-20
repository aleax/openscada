
//OpenSCADA system module BD.ODBC file: odbc.cpp
/***************************************************************************
 *   Copyright (C) 2015-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>

#include <tsys.h>
#include <tmess.h>

#include "odbc.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"ODBC"
#define MOD_NAME	_("DB by ODBC")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"0.2.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("BD module. Provides support of different databases by the ODBC connectors and drivers to the databases.")
#define MOD_LICENSE	"GPL2"
//************************************************

BD_ODBC::BDMod *BD_ODBC::mod; //Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_ODBC_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }
#ifdef MOD_INCL
    TModule *bd_ODBC_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new BD_ODBC::BDMod(source);
	return NULL;
    }
}

using namespace BD_ODBC;

//************************************************
//* BD_ODBC::BDMod				 *
//************************************************
BDMod::BDMod(string name) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, MOD_LICENSE, name);
}

BDMod::~BDMod( )
{

}

TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }


//************************************************
//* BD_ODBC::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) :
    TBD(iid, cf_el), henv(SQL_NULL_HANDLE), hdbc(SQL_NULL_HANDLE), hstmt(SQL_NULL_HANDLE), reqCnt(0), reqCntTm(0), trOpenTm(0), connRes(true)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag && owner().fullDeleteDB()) {
	MtxAlloc resource(connRes, true);
    }
}

string MBD::errors( )
{
    MtxAlloc resource(connRes, true);

    string rez;
    SQLTCHAR sqlstate[15];
    SQLINTEGER native_error = 0;
    SQLTCHAR buf[512];

#if (ODBCVER < 0x0300)
    while(hstmt) {
	if(!SQL_SUCCEEDED(SQLError(henv,hdbc,hstmt,sqlstate,&native_error,buf,sizeof(buf),NULL))) break;
	rez += TSYS::strMess("%s (%ld) SQLSTATE=%s; ", buf, (long)native_error, sqlstate);
    }
    while(hdbc) {
	if(!SQL_SUCCEEDED(SQLError(henv,hdbc,SQL_NULL_HSTMT,sqlstate,&native_error,buf,sizeof(buf),NULL))) break;
	rez += TSYS::strMess("%s (%ld) SQLSTATE=%s; ", buf, (long)native_error, sqlstate);
    }
    while(henv) {
	if(!SQL_SUCCEEDED(SQLError(henv,SQL_NULL_HDBC,SQL_NULL_HSTMT,sqlstate,&native_error,buf,sizeof(buf),NULL))) break;
	rez += TSYS::strMess("%s (%ld) SQLSTATE=%s; ", buf, (long)native_error, sqlstate);
    }
#else
    for(int i = 0; hstmt && i < 5; ) {
	if(!SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_STMT,hstmt,++i,sqlstate,&native_error,buf,sizeof(buf),NULL))) break;
	rez += TSYS::strMess("%d = %s (%ld) SQLSTATE=%s; ", i, buf, (long)native_error, sqlstate);
    }
    for(int i = 0; hdbc && i < 5; ) {
	if(!SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_DBC,hdbc,++i,sqlstate,&native_error,buf,sizeof(buf),NULL))) break;
	rez += TSYS::strMess("%d = %s (%ld) SQLSTATE=%s; ", i, buf, (long)native_error, sqlstate);
    }
    for(int i = 0; henv && i < 5; ) {
	if(!SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_ENV,henv,++i,sqlstate,&native_error,buf,sizeof(buf),NULL))) break;
	rez += TSYS::strMess("%d = %s (%ld) SQLSTATE=%s; ", i, buf, (long)native_error, sqlstate);
    }
#endif

    return rez;
}

string MBD::errState( )
{
    MtxAlloc resource(connRes, true);

    SQLTCHAR sqlstate[15] = "";

#if (ODBCVER < 0x0300)
    if(hstmt && !sqlstate[0]) SQLError(henv, hdbc, hstmt, sqlstate, NULL, NULL, 0, NULL);
    if(hdbc && !sqlstate[0]) SQLError(henv, hdbc, SQL_NULL_HSTMT, sqlstate, NULL, NULL, 0, NULL);
    if(henv && !sqlstate[0]) SQLError(henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, sqlstate, NULL, NULL, 0, NULL);
#else
    if(hstmt && !sqlstate[0]) SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlstate, NULL, NULL, 0, NULL);
    if(hdbc && !sqlstate[0]) SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, 1, sqlstate, NULL, NULL, 0, NULL);
    if(henv && !sqlstate[0]) SQLGetDiagRec(SQL_HANDLE_ENV, henv, 1, sqlstate, NULL, NULL, 0, NULL);
#endif

    return (char*)sqlstate;
}

void MBD::enable( )
{
    MtxAlloc resource(connRes, true);
    if(enableStat())	return;

    short buflen;

    try {
#if (ODBCVER < 0x0300)
	if(SQLAllocEnv(&henv) != SQL_SUCCESS)		throw TError(nodePath().c_str(), "SQLAllocEnv: %s", errors().c_str());
	if(SQLAllocConnect(henv,&hdbc) != SQL_SUCCESS)	throw TError(nodePath().c_str(), "SQLAllocConnect: %s", errors().c_str());
#else
	if(SQLAllocHandle(SQL_HANDLE_ENV,NULL,&henv) != SQL_SUCCESS)	throw TError(nodePath().c_str(), "SQLAllocHandle: %s", errors().c_str());
	SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);
	if(SQLAllocHandle(SQL_HANDLE_DBC,henv,&hdbc) != SQL_SUCCESS)	throw TError(nodePath().c_str(), "SQLAllocHandle: %s", errors().c_str());
#endif

	//Set the application name
	SQLSetConnectOption(hdbc, SQL_APPLICATION_NAME, (SQLULEN)PACKAGE_NAME".BD."MOD_ID);

	//The version number of the driver manager
	SQLTCHAR driverInfo[255];
	if(SQLGetInfo(hdbc,SQL_DM_VER,driverInfo,sizeof(driverInfo),NULL) == SQL_SUCCESS)
	    printf("Driver Manager: %s\n", driverInfo);

	//Real connect
	if(SQLDriverConnect(hdbc,0,(SQLCHAR*)addr().c_str(),SQL_NTS,(SQLCHAR*)outdsn,sizeof(outdsn)/sizeof(SQLTCHAR),&buflen,SQL_DRIVER_COMPLETE) != SQL_SUCCESS)
	    throw TError(nodePath().c_str(), "SQLDriverConnect: %s", errors().c_str());

	//The version number and name of the driver
	if(SQLGetInfo(hdbc,SQL_DRIVER_VER,driverInfo,sizeof(driverInfo),NULL) == SQL_SUCCESS) {
	    printf ("Driver: %s", driverInfo);
	    if(SQLGetInfo(hdbc,SQL_DRIVER_NAME,driverInfo,sizeof(driverInfo),NULL) == SQL_SUCCESS)
		printf(" (%s)", driverInfo);
	    printf("\n");
	}

	//Allocate statement handle
#if (ODBCVER < 0x0300)
	if(SQLAllocStmt(hdbc,&hstmt) != SQL_SUCCESS)	throw TError(nodePath().c_str(), "SQLAllocStmt: %s", errors().c_str());
#else
	if(SQLAllocHandle(SQL_HANDLE_STMT,hdbc,&hstmt) != SQL_SUCCESS)	throw TError(nodePath().c_str(), "SQLAllocHandle: %s", errors().c_str());
#endif
	TBD::enable();
    } catch(TError) { disable(); throw; }
}

void MBD::disable( )
{
    MtxAlloc resource(connRes, true);

    //Last commit
    if(reqCnt) try{ transCommit(); } catch(...) { }

    TBD::disable();

#if (ODBCVER < 0x0300)
    if(hstmt) SQLFreeStmt(hstmt, SQL_DROP);
    if(hdbc) {
	SQLDisconnect(hdbc);
	SQLFreeConnect(hdbc);
    }
    if(henv) SQLFreeEnv(henv);
#else
    if(hstmt) {
	SQLCloseCursor(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    if(hdbc) {
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    }
    if(henv) SQLFreeHandle(SQL_HANDLE_ENV, henv);
#endif
    hstmt = hdbc = henv = SQL_NULL_HANDLE;
}

void MBD::allowList( vector<string> &list )
{
    if(!enableStat())	return;
    list.clear();
    vector< vector<string> > tbl;
    sqlReq("tables", &tbl, false);
    for(unsigned i_t = 1; i_t < tbl.size(); i_t++)
	if(tbl[i_t].size() >= 4 && tbl[i_t][3] == "TABLE")
	    list.push_back(tbl[i_t][2]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw TError(nodePath().c_str(), _("Error open table '%s'. DB is disabled."), inm.c_str());

    if(create) {
	/*string req = "CREATE TABLE IF NOT EXISTS `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	    TSYS::strEncode(inm,TSYS::SQL)+"` (`<<empty>>` char(20) NOT NULL DEFAULT '' PRIMARY KEY)";
	owner().sqlReq(req);*/
    }

    return new MTable(inm, this);
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

void MBD::sqlReq( const string &req, vector< vector<string> > *tbl, char intoTrans )
{
    if(tbl) tbl->clear();
    if(!enableStat())	return;

    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    MtxAlloc resource(connRes, true);
    string err;

    try {
	SQLRETURN sts;
	SQLTCHAR fBuf[STR_BUF_LEN];
	SQLSMALLINT numCols, colType, colScale, colNullable;
	SQLLEN colIndicator;
	SQLULEN colPrecision;

	if(req == "tables") {
	    if(SQLTables(hstmt,NULL,0,NULL,0,NULL,0,NULL,0) != SQL_SUCCESS)
		throw TError("", "SQLTables(tables): %s", errors().c_str());
	}
	else if(req == "qualifiers") {
	    if(SQLTables(hstmt,(SQLCHAR*)("%"),SQL_NTS,(SQLCHAR*)(""),0,(SQLCHAR*)(""),0,(SQLCHAR*)(""),0) != SQL_SUCCESS)
		throw TError("", "SQLTables(owners): %s", errors().c_str());
	}
	else if(req == "owners") {
	    if(SQLTables(hstmt,(SQLCHAR*)(""),0,(SQLCHAR*)(""),0,(SQLCHAR*)(""),0,(SQLCHAR*)("%"),SQL_NTS) != SQL_SUCCESS)
		throw TError("", "SQLTables(types): %s", errors().c_str());
	}
	else if(req == "datatypes") {
	    if(SQLGetTypeInfo(hstmt,0) != SQL_SUCCESS)
		throw TError("", "SQLGetTypeInfo: %s", errors().c_str());
	}
	else {
	    //Prepare & Execute the statement
	    if(SQLPrepare(hstmt,(SQLTCHAR*)Mess->codeConvOut(codePage().c_str(),req).c_str(),SQL_NTS) != SQL_SUCCESS)
		throw TError("", "SQLPrepare: %s", errors().c_str());
	    if((sts=SQLExecute(hstmt)) != SQL_SUCCESS && sts != SQL_SUCCESS_WITH_INFO)	//!!!! Get details for SQL_SUCCESS_WITH_INFO about RO and other
		throw TError("", "SQLExecute: %s", errors().c_str());
	}

	//Result processing
	if(SQLNumResultCols(hstmt,&numCols) != SQL_SUCCESS)
	    throw TError("", "SQLNumResultCols: %s", errors().c_str());
	if(numCols) {
	    // Get the names for the columns
	    vector<string> row;
	    for(short colNum = 0; colNum < numCols; colNum++) {
		if(SQLDescribeCol(hstmt,colNum+1,(SQLTCHAR*)fBuf,sizeof(fBuf)/sizeof(SQLTCHAR),NULL,&colType,&colPrecision,&colScale,&colNullable) != SQL_SUCCESS)
		    throw TError("", "SQLDescribeCol: %s", errors().c_str());
		row.push_back((char*)fBuf);
	    }
	    tbl->push_back(row);

	    // Get all the fields
	    while(true) {
#if (ODBCVER < 0x0300)
		sts = SQLFetch(hstmt);
#else
		sts = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
#endif
		if(sts == SQL_NO_DATA_FOUND)	break;
		if(sts != SQL_SUCCESS)	throw TError("", "Fetch: %s", errors().c_str());
		row.clear();
		for(short colNum = 0; colNum < numCols; colNum++) {
		    //  Fetch this column as character
		    row.push_back("");
		    do {
			sts = SQLGetData(hstmt, colNum+1, SQL_C_CHAR, fBuf, sizeof(fBuf)/sizeof(SQLTCHAR), &colIndicator);
			if(sts != SQL_SUCCESS && sts != SQL_SUCCESS_WITH_INFO)
			    throw TError("", "SQLGetData: %s", errors().c_str());
			if(colIndicator != SQL_NULL_DATA) row.back() += (char*)fBuf;
		    } while(sts == SQL_SUCCESS_WITH_INFO && errState() == "01004");
		}
		tbl->push_back(row);
	    }
	}
	//if(sts == SQL_ERROR) throw TError(nodePath().c_str(), "SQLMoreResults: %s", errors().c_str());	//!!!! Only single result support !!!!
    }
    catch(TError ier) { err = ier.mess; }

#if (ODBCVER < 0x0300)
    SQLFreeStmt(hstmt, SQL_CLOSE);
#else
    SQLCloseCursor(hstmt);
#endif

    if(err.size()) throw TError(nodePath().c_str(), "%s", err.c_str());
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,
	    "help",_("!!! Type here the help information about the db address of your module"));
	if(reqCnt)
	    ctrMkNode("comm",opt,-1,"/prm/st/end_tr",_("Close opened transaction"),RWRWRW,"root",SDB_ID);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/end_tr" && ctrChkNode(opt,"set",RWRWRW,"root",SDB_ID,SEC_WR) && reqCnt) transCommit();
    else TBD::cntrCmdProc(opt);
}

//************************************************
//* BD_ODBC::Table                                *
//************************************************
MTable::MTable( string name, MBD *iown ) : TTable(name)
{
    setNodePrev(iown);

    //Get table structure description
    /*try {
	string req = "DESCRIBE `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name,TSYS::SQL) + "`";
	owner().sqlReq(req, &tblStrct);
    } catch(...) { }*/
}

MTable::~MTable( )	{ }

void MTable::postDisable( int flag )
{
    owner().transCommit();
    /*if(flag)
	try { owner().sqlReq("DROP TABLE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+TSYS::strEncode(name(),TSYS::SQL)+"`"); }
	catch(TError err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }*/
}

MBD &MTable::owner( )	{ return (MBD&)TTable::owner(); }

/*void MTable::getStructDB( string name, vector< vector<string> > &tblStrct )
{
    //Get generic data structure
    owner().transCommit();

		// Calculate the display width for the column
		switch(colType) {
		    case SQL_VARCHAR:
		    case SQL_CHAR:
		    case SQL_WVARCHAR:
		    case SQL_WCHAR:
		    case SQL_GUID:	dispWdts[colNum] = colPrecision;	break;
		    case SQL_BINARY:	dispWdts[colNum] = colPrecision * 2;	break;
		    case SQL_LONGVARCHAR:
		    case SQL_WLONGVARCHAR:
		    case SQL_LONGVARBINARY: dispWdts[colNum] = 30;		break;	// show only first 30
		    case SQL_BIT:	dispWdts[colNum] = 1;			break;
		    case SQL_TINYINT:
		    case SQL_SMALLINT:
		    case SQL_INTEGER:
		    case SQL_BIGINT:	dispWdts[colNum] = colPrecision + 1;	break;	// sign
		    case SQL_DOUBLE:
		    case SQL_DECIMAL:
		    case SQL_NUMERIC:
		    case SQL_FLOAT:
		    case SQL_REAL:	dispWdts[colNum] = colPrecision + 2;	break;	// sign, comma
#ifdef SQL_TYPE_DATE
		    case SQL_TYPE_DATE:
#endif
		    case SQL_DATE:	dispWdts[colNum] = 10;			break;
#ifdef SQL_TYPE_TIME
		    case SQL_TYPE_TIME:
#endif
		    case SQL_TIME:	dispWdts[colNum] = 8;			break;
#ifdef SQL_TYPE_TIMESTAMP
		    case SQL_TYPE_TIMESTAMP:
#endif
		    case SQL_TIMESTAMP:	dispWdts[colNum] = 19 + (colScale > 0) ? colScale + 1 : 0;	break;
		    default:		dispWdts[colNum] = 0;			continue;	// skip other data types
		}

}

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw TError(TSYS::DBTableEmpty, nodePath().c_str(), _("Table is empty!"));
    mLstUse = time(NULL);
    for(int i_fld = 1; i_fld < tblStrct.size(); i_fld++) {

    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{

}

void MTable::fieldGet( TConfig &cfg )
{

}

void MTable::fieldSet( TConfig &cfg )
{

}

void MTable::fieldDel( TConfig &cfg )
{

}

void MTable::fieldFix( TConfig &cfg )
{
    owner().transCommit();
    //!!! Process the code here for the fields' fixing(change its type if it doesn't match to the necessary one)
    owner().transOpen();
}

string MTable::getVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(cfg.fld().flg()&TFld::DateTimeDec) return UTCtoSQL(s2i(rez));
    return rez;
}

void MTable::setVal( TCfg &cf, const string &val, bool tr )
{
    switch(cf.fld().type()) {
	case TFld::Integer:
	    if(cf.fld().flg()&TFld::DateTimeDec) cf.setI(SQLtoUTC(val));
	    else cf.setS(val);
	    break;
	case TFld::String:
	    if(!cf.extVal()) {
		if(!tr || (cf.fld().flg()&TCfg::TransltText && !cf.noTransl())) cf.setS(val);
		if(!tr && cf.fld().flg()&TCfg::TransltText && !cf.noTransl()) Mess->translReg(val, "db:"+fullDBName()+"#"+cf.name());
	    }
	    else {
		cf.setS(val, (tr?TCfg::ExtValTwo:TCfg::ExtValOne));
		if(!tr) cf.setS("db:"+fullDBName()+"#"+cf.name(), TCfg::ExtValThree);
	    }
	    break;
	default: cf.setS(val); break;
    }
}

string MTable::UTCtoSQL( time_t val )
{
    char buf[255];
    struct tm tm_tm;

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
}*/

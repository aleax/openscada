// 
//OpenSCADA system module BD.PostgreSQL file: postgre.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Maxim Lysenko                                   *
 *   mlisenko@oscada.org                                                   *
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
#include <string>
#include <libpq-fe.h>

#include <tsys.h>
#include <tmess.h>
#include "postgre.h"


//************************************************
//* Modul info!                                  *
#define MOD_ID		"PostgreSQL"
#define MOD_NAME	"DB PostgreSQL"
#define MOD_TYPE	"BD"
#define VER_TYPE	VER_BD
#define VERSION		"0.5.0"
#define AUTORS		"Maxim Lysenko"
#define DESCRIPTION	"BD module. Provides support of the BD PostgreSQL."
#define MOD_LICENSE	"GPL2"
//************************************************

BDPostgreSQL::BDMod *BDPostgreSQL::mod;

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
	    return new BDPostgreSQL::BDMod( source );
	return NULL;
    }
}

using namespace BDPostgreSQL;

//************************************************
//* BDPostgreSQL::BDMod				 *
//************************************************
BDMod::BDMod(string name)
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= MOD_LICENSE;
    mSource	= name;

    mod		= this;
}

BDMod::~BDMod()
{

}

TBD *BDMod::openBD( const string &name )
{
    return new MBD(name,&owner().openDB_E());
}

//************************************************
//* BDPostgreSQL::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid,cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0)
{

}

MBD::~MBD( )
{

}

//Override the default notice processor with the empty one. This is done to avoid the notice messages printing on stderr
void MyNoticeProcessor(void *arg, const char *message)
{
}

void MBD::postDisable(int flag)
{
    TBD::postDisable(flag);

    if( flag && owner().fullDeleteDB() )
    {
        ResAlloc resource(conn_res,true);
        PGconn * connection = NULL;
        PGresult *res;
        string conninfo;
        try
        {
            conninfo = "host = " + host + " hostaddr = " + hostaddr + " port = " + port + " dbname = template1" + " user = " + user + 
                    " password = " + pass + " connect_timeout = " + connect_timeout;
            if(( connection = PQconnectdb( conninfo.c_str() )) == NULL )
                throw TError(TSYS::DBInit,nodePath().c_str(),_("Fatal error - unable to allocate connection."));
            if( PQstatus( connection ) != CONNECTION_OK )
                throw TError(TSYS::DBConn,nodePath().c_str(),_("Connect to DB error: %s"),PQerrorMessage( connection ));
            string req = "DROP DATABASE \""+db+"\"";
            if( (res = PQexec(connection,req.c_str())) == NULL )
                throw TError(TSYS::DBRequest,nodePath().c_str(),_("Connect to DB error: %s"),PQerrorMessage( connection ));
            if( ( PQresultStatus( res ) != PGRES_COMMAND_OK ) && ( PQresultStatus( res ) != PGRES_TUPLES_OK ) )
            {
                string err, err1;
                err = PQresStatus( PQresultStatus( res ));
                err1 = PQresultErrorMessage( res );
                PQclear( res );
                throw TError(TSYS::DBRequest,nodePath().c_str(),_("Query to DB error: %s. %s"),err.c_str(),err1.c_str());
            }
            else PQclear( res );
            PQfinish( connection );
        }
        catch(...)
        {
            if( connection ) PQfinish( connection );
            throw;
        }
    }
}

void MBD::enable( )
{
    if( enableStat() )	return;

    host = (TSYS::strSepParse(addr(),0,';') == "") ? "localhost" : TSYS::strSepParse(addr(),0,';');
    hostaddr = (TSYS::strSepParse(addr(),1,';') == "") ? "127.0.0.1" : TSYS::strSepParse(addr(),1,';');
    user = TSYS::strSepParse(addr(),2,';');
    pass = TSYS::strSepParse(addr(),3,';');
    db   = TSYS::strSepParse(addr(),4,';');
    port = (TSYS::strSepParse(addr(),5,';') == "") ? "5432" : TSYS::strSepParse(addr(),5,';');
    connect_timeout = (TSYS::strSepParse(addr(),6,';') == "") ? "10" : TSYS::strSepParse(addr(),6,';');
    string conninfo, conninfoReal;
    conninfo = "host = " + host + " hostaddr = " + hostaddr + " port = " + port + " dbname = template1" + " user = " + user + 
               " password = " + pass + " connect_timeout = " + connect_timeout;
    conninfoReal = "host = " + host + " hostaddr = " + hostaddr + " port = " + port + " dbname = " + db + " user = " + user + 
                   " password = " + pass + " connect_timeout = " + connect_timeout;
    cd_pg  = codePage().size()?codePage():Mess->charset();
    try
    {
        if(( connection = PQconnectdb( conninfo.c_str() )) == NULL )
            throw TError(TSYS::DBInit,nodePath().c_str(),_("Fatal error - unable to allocate connection."));
        if( PQstatus( connection ) != CONNECTION_OK )
            throw TError(TSYS::DBConn,nodePath().c_str(),_("Connect to DB error: %s"),PQerrorMessage( connection ));
        TBD::enable( );

        vector< vector<string> > tbl;
        string req = "SELECT count(*) FROM pg_catalog.pg_database WHERE datname = '" + db + "'";
        sqlReq(req,&tbl);
        if( tbl.size() == 2 && tbl[1][0] == "0" )
        {
            sqlReq("CREATE DATABASE \""+TSYS::strEncode(db,TSYS::SQL)+"\" ENCODING = '" + cd_pg + "'");
            PQfinish( connection );
            if(( connection = PQconnectdb( conninfoReal.c_str() )) == NULL )
                throw TError(TSYS::DBInit,nodePath().c_str(),_("Fatal error - unable to allocate connection."));
            if( PQstatus( connection ) != CONNECTION_OK )
                throw TError(TSYS::DBConn,nodePath().c_str(),_("Connect to DB error: %s"),PQerrorMessage( connection ));
            else  PQsetNoticeProcessor(connection, MyNoticeProcessor, NULL);
        }
        else
        {
            PQfinish( connection );
            if(( connection = PQconnectdb( conninfoReal.c_str() )) == NULL )
                throw TError(TSYS::DBInit,nodePath().c_str(),_("Fatal error - unable to allocate connection."));
            if( PQstatus( connection ) != CONNECTION_OK )
                throw TError(TSYS::DBConn,nodePath().c_str(),_("Connect to DB error: %s"),PQerrorMessage( connection ));
            else  PQsetNoticeProcessor(connection, MyNoticeProcessor, NULL);

        }
    }
    catch(...)
    {
        if( connection ) PQfinish( connection );
        throw;
    }
}

void MBD::disable( )
{
    if( !enableStat() )  return;
    TBD::disable( );
    ResAlloc resource(conn_res,true);
    PQfinish( connection );
}

void MBD::allowList( vector<string> &list )
{
    if( !enableStat() )  return;
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
    sqlReq(req,&tbl);
    for( int i_t = 1; i_t < tbl.size(); i_t++ ) list.push_back(tbl[i_t][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if( !enableStat() )
	throw TError(TSYS::DBOpen,nodePath().c_str(),_("Error open table <%s>. DB is disabled."),inm.c_str());
    return new MTable(inm,this,create);
}

void MBD::transOpen( )
{
    //> Check for limit into one trinsaction
    if( reqCnt > 1000 ) transCommit( );

    ResAlloc resource(conn_res,true);
    PGTransactionStatusType tp;
    tp = PQtransactionStatus( connection );

    if( tp != PQTRANS_INTRANS )
    {
        PGresult   *res;
        res = PQexec(connection, "BEGIN");
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            PQclear(res);
            throw TError(TSYS::DBRequest,nodePath().c_str(),_("Start trasaction error!"));
        }
        PQclear(res);
        trOpenTm = time(NULL);
    }
    reqCnt++;
    reqCntTm = time(NULL);
}

void MBD::transCommit( )
{
    ResAlloc resource(conn_res,true);
    PGTransactionStatusType tp;
    tp = PQtransactionStatus( connection );
    if( tp != PQTRANS_IDLE )
    {
        PGresult   *res;
        res = PQexec(connection, "COMMIT");
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            PQclear(res);
            throw TError(TSYS::DBRequest,nodePath().c_str(),_("Stop trasaction error!"));
        }
        PQclear(res);
    }
    reqCnt = reqCntTm = 0;
}


void MBD::transCloseCheck( )
{
    if( reqCnt && ((time(NULL)-reqCntTm) > 10*60 || (time(NULL)-trOpenTm) > 10*60 ) )
        transCommit();
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    PGresult *res;

    if( tbl ) tbl->clear();
    if( !enableStat() ) return;

    string req = Mess->codeConvOut(cd_pg.c_str(),ireq);

    ResAlloc resource(conn_res,true);
    if( intoTrans && intoTrans != EVAL_BOOL )	transOpen();
    else if( !intoTrans && reqCnt )	transCommit();

    if( PQstatus( connection ) != CONNECTION_OK  )
    {
        resource.release();
        disable();
        enable();
        resource.request(true);
    }
    if( (res = PQexec(connection,req.c_str())) == NULL )
        throw TError(TSYS::DBRequest,nodePath().c_str(),_("Connect to DB error: %s"),PQerrorMessage( connection ));
    if( ( PQresultStatus( res ) != PGRES_COMMAND_OK ) && ( PQresultStatus( res ) != PGRES_TUPLES_OK ) )
    {
        string err, err1;
        err = PQresStatus( PQresultStatus( res ));
        err1 = PQresultErrorMessage( res );
        PQclear( res );
        throw TError(TSYS::DBRequest,nodePath().c_str(),_("Query to DB error: %s. %s"),err.c_str(),err1.c_str());
    }

    if( tbl )
    {
        int num_fields = PQnfields( res );
        int row;
	vector<string> fld;
	//- Add head -
	for( int i=0; i < num_fields; i++ )
            fld.push_back(PQfname( res, i ));
	tbl->push_back(fld);
	//- Add data -
        for( row = 0; row < PQntuples( res ); row++ )
	{
	    fld.clear();
	    for(int i=0; i < num_fields; i++)
            {
                string val = "";
                //Clear spaces at the end of the string
                if( PQftype(res, i) == 1042 )
                {
                    string tmp_val = PQgetvalue(res, row, i);
                    char c;
                    int len  = tmp_val.size(), spCol = 0, n = len;
                    while((c = tmp_val[n-1]) == ' ') { n--; spCol++; }
                    int len_new = len - spCol;
                    val = tmp_val.substr (0,len_new);
                    fld.push_back(Mess->codeConvIn(cd_pg.c_str(),val));
                }
                else fld.push_back(Mess->codeConvIn(cd_pg.c_str(),PQgetvalue(res, row, i)));
            }
	    tbl->push_back(fld);
	}
    }
    PQclear( res );
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","BD",2,
	    "tp","str","help",
	    _("PostgreSQL DB address must be written as: [<host>;<hostaddr>;<user>;<pass>;<db>;<port>;<connect_timeout>].\n"
	      "Where:\n"
              "  host - Name of the host (PostgreSQL server) to connect to. If this begins with a slash ('/'),\n"
              "         it specifies Unix domain communication rather than TCP/IP communication;\n"
              "         the value is the name of the directory in which the socket file is stored.\n"
              "  hostaddr - Numeric IP address of host to connect to;\n"
	      "  user - DB user name;\n"
	      "  pass - user's password for DB access;\n"
	      "  db - DB name;\n"
	      "  port - DB server port (default 5432);\n"
	      "  connect_timeout - connection timeout\n"
	      "For local DB: [;;roman;123456;OpenSCADA;5432;10].\n"
	      "For remote DB: [server.nm.org;192.168.2.1;roman;123456;OpenSCADA;5432;10]."));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//************************************************
//* BDPostgreSQL::Table                          *
//************************************************
MTable::MTable(string name, MBD *iown, bool create ) : TTable(name)
{
    string req;

    setNodePrev(iown);
    vector< vector<string> > tbl;
    string id;
    req = "SELECT count(*) "
          "FROM pg_catalog.pg_class c "
            "JOIN pg_catalog.pg_roles r ON r.oid = c.relowner "
            "LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace "
          "WHERE c.relkind IN ('r','v','S','') "
            "AND n.nspname <> 'pg_catalog' "
            "AND n.nspname !~ '^pg_toast' "
            "AND pg_catalog.pg_table_is_visible(c.oid) "
            "AND c.relname = '" + TSYS::strEncode(name,TSYS::SQL) + "'";
    owner().sqlReq(req, &tbl);
    if( create && tbl.size() == 2 && tbl[1][0] == "0" )
    {
        req = "CREATE TABLE \"" + TSYS::strEncode(name,TSYS::SQL)+ "\"(\"name\" character(20) NOT NULL DEFAULT '' PRIMARY KEY)";
        owner().sqlReq( req );
    }
    getStructDB( name, tblStrct );
}

MTable::~MTable(  )
{

}

void MTable::postDisable(int flag)
{
    owner().transCommit();
    if( flag )
    {
	try
	{
	    owner().sqlReq("DROP TABLE \""+ TSYS::strEncode(name(),TSYS::SQL) + "\"");
	}
	catch(TError err){ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

MBD &MTable::owner()	{ return (MBD&)TTable::owner(); }

void MTable::getStructDB( string name, vector< vector<string> > &tblStrct )
{
    //- Get generic data structure -
    owner().transCommit();
    string req = "SELECT a.attname as \"Field\", pg_catalog.format_type(a.atttypid, a.atttypmod) as \"Type\" "
                 "FROM pg_catalog.pg_attribute a "
                 "WHERE a.attnum > 0 "
                 "AND NOT a.attisdropped "
                 "AND a.attrelid = "
                 "( "
                    "SELECT c.oid "
                    "FROM pg_catalog.pg_class c "
                    "LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace "
                    "WHERE c.relname ~ '^(" + TSYS::strEncode(name,TSYS::SQL) + ")$' "
                    "AND pg_catalog.pg_table_is_visible(c.oid) "
                 ")";
    owner().sqlReq(req,&tblStrct);
    if( tblStrct.size( ) > 1 )
    {
	//- Get keys -
        vector< vector<string> > keyLst;
        req = "SELECT a.attname "
              "FROM pg_class c, pg_class c2, pg_index i, pg_attribute a "
              "WHERE c.relname = '" + TSYS::strEncode(name,TSYS::SQL) + "' AND c.oid = i.indrelid AND i.indexrelid = c2.oid "
              "AND i.indisprimary AND i.indisunique "
              "AND a.attrelid=c2.oid "
              "AND a.attnum>0;";
        owner().sqlReq(req,&keyLst);
        tblStrct[0].push_back("Key");
        for( int i_f = 1; i_f < tblStrct.size(); i_f++ )
        {
            int i_k;
            for( i_k = 1; i_k < keyLst.size(); i_k++ )
                if( tblStrct[i_f][0] == keyLst[i_k][0] ) break;
            tblStrct[i_f].push_back( (i_k<keyLst.size()) ? "PRI" : "" );
        }
    }
    else throw TError(TSYS::DBRequest,nodePath().c_str(),_("Table is not present!"));
}

void MTable::fieldStruct( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	int pr1, pr2;
	string sid = tblStrct[i_fld][0];
	if( cfg.cfgPresent(sid) ) continue;
	int flg = (tblStrct[i_fld][2]=="PRI") ? (int)TCfg::Key : (int)TFld::NoFlag;
        if( (tblStrct[i_fld][1] == "text") || (tblStrct[i_fld][1] == "character varying") )
            cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215") );
        else if( sscanf(tblStrct[i_fld][1].c_str(),"character(%d)",&pr1) ||
                 sscanf(tblStrct[i_fld][1].c_str(),"character varying(%d)",&pr1) )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,TSYS::int2str(pr1).c_str()) );
        else if( (tblStrct[i_fld][1] == "integer") || (tblStrct[i_fld][1] == "bigint") )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg) );
        else if( tblStrct[i_fld][1] == "double precision" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg) );
        else if( tblStrct[i_fld][1] == "smallint" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Boolean,flg) );
        else if( tblStrct[i_fld][1] == "timestamp with time zone" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg|TFld::DateTimeDec,"10") );
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    //owner().transCommit();
    string sid;
    //> Make SELECT and WHERE
    string req = "SELECT ";
    string req_where = "WHERE ";
    //> Add use keys to list
    bool first_sel = true, next = false, trPresent = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	sid = tblStrct[i_fld][0];
	TCfg *u_cfg = cfg.at(sid,true);
	if( !cfg.noTransl() && !u_cfg && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#") )
	{
	    u_cfg = cfg.at(sid.substr(3),true);
	    if( u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText) ) continue;
	    trPresent = true;
	}
	if( !u_cfg ) continue;

	if( u_cfg->fld().flg()&TCfg::Key && u_cfg->keyUse() )
	{
	    req_where = req_where + (next?"AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL) + "\"='" + TSYS::strEncode(getVal(*u_cfg),TSYS::SQL) + "' ";
	    next = true;
	}
	else if( u_cfg->fld().flg()&TCfg::Key || u_cfg->view() )
	{
	    req = req + (first_sel?"\"":",\"") + TSYS::strEncode(sid,TSYS::SQL) + "\"";
	    first_sel = false;
	}
    }

    //> Request
    if( first_sel ) return false;
    req = req + " FROM \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" " +
	((next)?req_where:"") + " LIMIT 1 OFFSET " + TSYS::int2str(row);
    owner().sqlReq( req, &tbl, false );
    if( tbl.size() < 2 ) return false;
    for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
    {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid,true);
	if( u_cfg ) setVal(*u_cfg,tbl[1][i_fld]);
	else if( trPresent && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#") && tbl[1][i_fld].size() )
	{
	    u_cfg = cfg.at(sid.substr(3),true);
	    if( u_cfg ) setVal(*u_cfg,tbl[1][i_fld]);
	}
    }
    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    //owner().transCommit();
    string sid;
    //> Prepare request
    string req = "SELECT ";
    string req_where;
    //>> Add fields list to queue
    bool first_sel = true, next_wr = false, trPresent = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	sid = tblStrct[i_fld][0];
	TCfg *u_cfg = cfg.at(sid,true);
	if( !cfg.noTransl() && !u_cfg && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#") )
	{
	    u_cfg = cfg.at(sid.substr(3),true);
	    if( u_cfg && !(u_cfg->fld().flg()&TCfg::TransltText) ) continue;
	    trPresent = true;
	}
	if( !u_cfg ) continue;

	if( u_cfg->fld().flg()&TCfg::Key )
	{
	    req_where = req_where + (next_wr?"AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL) + "\"='"  +TSYS::strEncode(getVal(*u_cfg),TSYS::SQL) + "' ";
	    next_wr = true;
	}
	else if( u_cfg->view() )
	{
	    req = req + (first_sel?"\"":",\"") + TSYS::strEncode(sid,TSYS::SQL) + "\"";
	    first_sel = false;
	}
    }
    req = req + " FROM \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" WHERE " + req_where;

    //> Query
    owner().sqlReq( req, &tbl, false );
    if( tbl.size() < 2 ) throw TError(TSYS::DBRowNoPresent,nodePath().c_str(),_("Row is not present!"));

    //> Processing of query
    for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
    {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid,true);
	if( u_cfg ) setVal(*u_cfg,tbl[1][i_fld]);
	else if( trPresent && sid.size() > 3 && sid.substr(0,3) == (Mess->lang2Code()+"#") && tbl[1][i_fld].size() )
	{
	    u_cfg = cfg.at(sid.substr(3),true);
	    if( u_cfg ) setVal(*u_cfg,tbl[1][i_fld]);
	}
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    //owner().transOpen();
    string sid, sval;
    bool isVarTextTransl = (!Mess->lang2CodeBase().empty() && !cfg.noTransl() && Mess->lang2Code() != Mess->lang2CodeBase());
    //> Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //> Check for translation present
    bool trPresent = isVarTextTransl, trDblDef = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	if( (trPresent || cfg.noTransl()) && (!isVarTextTransl || trDblDef) ) break;
	sid = tblStrct[i_fld][0];
	if( sid.size() > 3 )
	{
	    if( !trPresent && sid.substr(0,3) == (Mess->lang2Code()+"#") ) trPresent = true;
	    if( isVarTextTransl && !trDblDef && sid.substr(0,3) == (Mess->lang2CodeBase()+"#") ) trDblDef = true;
	}
    }
    if( trDblDef ) fieldFix(cfg);

    //> Get present fields list
    string req_where = "WHERE ";
    //>> Add key list to query
    bool next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( !(u_cfg.fld().flg()&TCfg::Key) ) continue;
	req_where = req_where + (next?"AND \"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "\"='" + TSYS::strEncode(getVal(u_cfg),TSYS::SQL) + "' ";
	next = true;
    }

    //> Prepare query
    //>> Try for get already present field
    string req = "SELECT 1 FROM \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" " + req_where;
    try{ owner().sqlReq( req, &tbl, true ); }
    catch(TError err)	{ fieldFix(cfg); owner().sqlReq( req, NULL, true ); }
    if( tbl.size() < 2 )
    {
	//>> Add new record
	req = "INSERT INTO \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" ";
	string ins_name, ins_value;
	next = false;
	for( int i_el = 0; i_el < cf_el.size(); i_el++ )
	{
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if( !(u_cfg.fld().flg()&TCfg::Key) && !u_cfg.view() ) continue;

	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    ins_name = ins_name + (next?",\"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "\" " +
		       ( isTransl ? (",\"" + TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[i_el],TSYS::SQL) + "\" ") : "" );
	    sval = getVal(u_cfg);
	    ins_value = ins_value + (next?",E'":"E'") + TSYS::strEncode(sval,TSYS::SQL) + "' " +
		        ( isTransl ? (",E'" + TSYS::strEncode(sval,TSYS::SQL) + "' ") : "" );
	    next = true;
	}
	req = req + "("+ins_name+") VALUES ("+ins_value+")";
    }
    else
    {
	//>> Update present record
	req = "UPDATE \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" SET ";
	next = false;
	for( int i_el = 0; i_el < cf_el.size(); i_el++ )
	{
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if( u_cfg.fld().flg()&TCfg::Key || !u_cfg.view() ) continue;
	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    sval = getVal(u_cfg);
	    req = req + (next?",\"":"\"") + TSYS::strEncode(sid,TSYS::SQL) + "\"=E'" + TSYS::strEncode(sval,TSYS::SQL) + "' ";
	    next = true;
	}
	req = req + req_where;
    }

    //> Query
    try{ owner().sqlReq( req, NULL, true ); }
    catch(TError err)	{ fieldFix(cfg); owner().sqlReq( req, NULL, true ); }
}

void MTable::fieldDel( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    //owner().transOpen();
    //> Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //> Prepare request
    string req = "DELETE FROM \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" WHERE ";
    //>> Add key list to query
    bool next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( u_cfg.fld().flg()&TCfg::Key && u_cfg.keyUse() )
	{
	    req = req + (next?"AND \"":"\"") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "\"='" + TSYS::strEncode(getVal(u_cfg),TSYS::SQL) + "' ";
	    next = true;
	}
    }

    owner().sqlReq( req, NULL, true );
}

void MTable::fieldFix( TConfig &cfg )
{
    bool next = false, next_key = false;
    //owner().transCommit();
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    bool isVarTextTransl = (!Mess->lang2CodeBase().empty() && !cfg.noTransl() && Mess->lang2Code() != Mess->lang2CodeBase());
    //> Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //> Prepare request for fix structure
    string req = "ALTER TABLE \"" + TSYS::strEncode(name(),TSYS::SQL) + "\" DROP CONSTRAINT \"" + TSYS::strEncode(name(),TSYS::SQL) + "_pkey\", ";

    //> DROP fields
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
        int i_cf;
        for( i_cf = 0; i_cf < cf_el.size(); i_cf++ )
            if( cf_el[i_cf] == tblStrct[i_fld][0] ||
                    (cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::TransltText && tblStrct[i_fld][0].size() > 3 && 
                    tblStrct[i_fld][0].substr(2) == ("#"+cf_el[i_cf]) && tblStrct[i_fld][0].substr(0,2) != Mess->lang2CodeBase()) )
            {
                TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
                bool isEqual = false;
                switch(u_cfg.fld().type())
                {
                    case TFld::String:
                    {
                        int pr1 = -1;
                        if( (tblStrct[i_fld][1] == "text") || (tblStrct[i_fld][1] == "character varying") )
                            isEqual = true;
                        else if( ( sscanf(tblStrct[i_fld][1].c_str(),"character(%d)",&pr1) ||
                                   sscanf(tblStrct[i_fld][1].c_str(),"character varying(%d)",&pr1) ) && 
                                 (u_cfg.fld().len() <= 255 || u_cfg.fld().flg()&TCfg::Key) && pr1 > 0 )
                            isEqual = true;
                        break;
                    }
                    case TFld::Integer:	
                        if( u_cfg.fld().flg()&TFld::DateTimeDec && (tblStrct[i_fld][1] == "timestamp with time zone") )
                            isEqual = true;
                        else if( (tblStrct[i_fld][1] == "integer") || 
                                 (tblStrct[i_fld][1] == "bigint") )
                            isEqual = true;
                        break;
                    case TFld::Real:	if( tblStrct[i_fld][1] == "double precision" ) isEqual = true;	break;
                    case TFld::Boolean:	if( tblStrct[i_fld][1] == "smallint" ) isEqual = true;	break;
                }
                if( isEqual ) break;
                continue;
            }
        if( i_cf >= cf_el.size() )
        {
            req = req + (next?",DROP \"":"DROP \"") + TSYS::strEncode(tblStrct[i_fld][0],TSYS::SQL) + "\" ";
            next = true;
        }
    }

    string pr_keys;
    //Add fields
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
	//>> Check primary key
	if( u_cfg.fld().flg()&TCfg::Key )
	{
	    pr_keys = pr_keys + (next_key?",\"":"\"") + TSYS::strEncode(u_cfg.name(),TSYS::SQL) + "\"";
	    next_key = true;
	}

	int i_fld, tsz;
	for( i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
	    if( cf_el[i_cf] == tblStrct[i_fld][0] ) break;

	string f_tp;
        switch(u_cfg.fld().type())
        {
            case TFld::String:
                if( u_cfg.fld().len() < 256 || u_cfg.fld().flg()&TCfg::Key )
                    f_tp="CHARACTER VARYING("+SYS->int2str(vmax(10,vmin(255,u_cfg.fld().len())))+") DEFAULT '"+u_cfg.fld().def()+"' ";
                else  f_tp="TEXT DEFAULT '"+u_cfg.fld().def()+"' ";
                break;
            case TFld::Integer:
                if( u_cfg.fld().flg()&TFld::DateTimeDec )
                    f_tp = req+"TIMESTAMP WITH TIME ZONE DEFAULT '"+UTCtoSQL(atoi(u_cfg.fld().def().c_str()))+"' "; 
                else f_tp="INTEGER DEFAULT '"+TSYS::int2str(atoi(u_cfg.fld().def().c_str()))+"' ";
                break;
            case TFld::Real:
                f_tp="DOUBLE PRECISION DEFAULT '"+TSYS::real2str(atof(u_cfg.fld().def().c_str()))+"' ";
                break;
            case TFld::Boolean:
                f_tp="SMALLINT DEFAULT '"+TSYS::int2str(atoi(u_cfg.fld().def().c_str()))+"' ";
                break;
        }

	if( i_fld >= tblStrct.size() )
        {
            if( !next ) next = true; else req=req+",";
	   //-- Add field --
            req=req+"ADD COLUMN \""+TSYS::strEncode(cf_el[i_cf],TSYS::SQL)+"\" "+f_tp;
        }
	//> Check other languages
        if( u_cfg.fld().flg()&TCfg::TransltText )
        {
            int i_c;
            for( i_c = i_fld; i_c < tblStrct.size(); i_c++ )
                if( tblStrct[i_c][0].size() > 3 && tblStrct[i_c][0].substr(2) == ("#"+cf_el[i_cf]) && 
                    tblStrct[i_c][0].substr(0,2) != Mess->lang2CodeBase() &&
                    tblStrct[i_c][0].substr(0,2) == Mess->lang2Code() ) break;
            if( i_c >= tblStrct.size() && isVarTextTransl )
            {
                req = req + (next?",ADD \"":"ADD \"") + TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[i_cf],TSYS::SQL) + "\" "+f_tp;
                next = true;
            }
        }
    }
    req = req + ",ADD PRIMARY KEY (" + pr_keys + ") ";

    if( next )
    {
	owner().sqlReq( req, NULL, false );
	//> Update structure information
        getStructDB( name(), tblStrct );
    }
    //owner().transOpen();
}


string MTable::getVal( TCfg &cfg )
{
    switch( cfg.fld().type() )
    {
	case TFld::String:	return cfg.getS();
	case TFld::Integer:
	    if( cfg.fld().flg()&TFld::DateTimeDec )	return UTCtoSQL(cfg.getI());
	    else		return SYS->int2str(cfg.getI());
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
	case TFld::Integer:
	    if( cfg.fld().flg()&TFld::DateTimeDec )	cfg.setI(SQLtoUTC(val));
	    else cfg.setI(atoi(val.c_str()));
	    break;
	case TFld::Real:	cfg.setR(atof(val.c_str()));	break;
	case TFld::Boolean:	cfg.setB(atoi(val.c_str()));	break;
    }
}

string MTable::UTCtoSQL( time_t val )
{
    struct tm tm_tm;
    localtime_r(&val,&tm_tm);
    char buf[255];
    int rez = strftime( buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_tm );

    return (rez>0) ? string(buf,rez) : "";
}

time_t MTable::SQLtoUTC( const string &val )
{
    struct tm stm;
    strptime(val.c_str(),"%Y-%m-%d %H:%M:%S",&stm);

    return mktime(&stm);
}

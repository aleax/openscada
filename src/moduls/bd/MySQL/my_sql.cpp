
//OpenSCADA system module BD.MySQL file: my_sql.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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
#include <string>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#include <tsys.h>
#include <tmess.h>
#include "my_sql.h"


//************************************************
//* Modul info!                                  *
#define MOD_ID		"MySQL"
#define MOD_NAME	"DB MySQL"
#define MOD_TYPE	"BD"
#define VER_TYPE	VER_BD
#define VERSION		"1.4.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"BD modul. Allow support of the BD MySQL."
#define MOD_LICENSE	"GPL"
//************************************************

BDMySQL::BDMod *BDMySQL::mod;

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
	    return new BDMySQL::BDMod( source );
	return NULL;
    }
}

using namespace BDMySQL;

//************************************************
//* BDMySQL::BDMod				 *
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

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

//************************************************
//* BDMySQL::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid,cf_el)
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
	MYSQL connect;

	if(!mysql_init(&connect)) throw TError(TSYS::DBInit,nodePath().c_str(),_("Error initializing client."));
	connect.reconnect = 1;
	if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size())?u_sock.c_str():NULL,0))
	    throw TError(TSYS::DBConn,nodePath().c_str(),_("Connect to DB error: %s"),mysql_error(&connect));

	string req = "DROP DATABASE `"+bd+"`";
	if(mysql_real_query(&connect,req.c_str(),req.size()))
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("Query to DB error: %s"),mysql_error(&connect));

	mysql_close(&connect);
    }
}

void MBD::enable( )
{
    if( enableStat() )	return;

    host = TSYS::strSepParse(addr(),0,';');
    user = TSYS::strSepParse(addr(),1,';');
    pass = TSYS::strSepParse(addr(),2,';');
    bd   = TSYS::strSepParse(addr(),3,';');
    port = atoi(TSYS::strSepParse(addr(),4,';').c_str());
    u_sock = TSYS::strSepParse(addr(),5,';');
    cd_pg  = codePage().size()?codePage():Mess->charset();

    if(!mysql_init(&connect))
	throw TError(TSYS::DBInit,nodePath().c_str(),_("Error initializing client."));
    connect.reconnect = 1;
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size())?u_sock.c_str():NULL,0))
	throw TError(TSYS::DBConn,nodePath().c_str(),_("Connect to DB error: %s"),mysql_error(&connect));

    TBD::enable( );

    string req = "CREATE DATABASE IF NOT EXISTS `"+TSYS::strEncode(bd,TSYS::SQL)+"`";
    sqlReq(req);
}

void MBD::disable( )
{
    if( !enableStat() )  return;

    TBD::disable( );

    ResAlloc resource(conn_res,true);
    mysql_close(&connect);
}

void MBD::allowList( vector<string> &list )
{
    if( !enableStat() )  return;
    list.clear();
    vector< vector<string> > tbl;
    sqlReq("SHOW TABLES FROM `"+TSYS::strEncode(bd,TSYS::SQL)+"`",&tbl);
    for( int i_t = 1; i_t < tbl.size(); i_t++ )
	list.push_back(tbl[i_t][0]);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if( !enableStat() )
	throw TError(TSYS::DBOpen,nodePath().c_str(),_("Error open table <%s>. DB disabled."),inm.c_str());

    return new MTable(inm,this,create);
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl )
{
    MYSQL_RES *res = NULL;

    if( tbl ) tbl->clear();
    if( !enableStat() ) return;

    string req = Mess->codeConvOut(cd_pg.c_str(),ireq);

    ResAlloc resource(conn_res,true);

    int irez;
    if( irez = mysql_real_query(&connect,req.c_str(),req.size()) )
    {
	if( irez == CR_SERVER_GONE_ERROR || irez == CR_SERVER_LOST )
	{
	    resource.release();
	    disable();
	    enable();
	    resource.request(true);
	    irez = mysql_real_query(&connect,req.c_str(),req.size());
	}
	if( irez )
	{
	    //mess_debug(nodePath().c_str(),_("Query <%s> is error."),ireq.c_str());
	    throw TError(TSYS::DBRequest,nodePath().c_str(),_("Query to DB error %d: %s"),irez,mysql_error(&connect));
	}
    }
    if( mysql_field_count(&connect) == 0 ) return;
    if( !(res = mysql_store_result(&connect)) )
	throw TError(TSYS::DBRequest,nodePath().c_str(),_("Store result error: %s"),mysql_error(&connect));

    if( tbl )
    {
	int num_fields = mysql_num_fields(res);
	MYSQL_ROW row;

	vector<string> fld;
	//- Add head -
	for(int i=0; i < num_fields; i++)
	    fld.push_back(mysql_fetch_field_direct(res,i)->name);
	tbl->push_back(fld);
	//- Add data -
	while( (row = mysql_fetch_row(res)) )
	{
	    fld.clear();
	    for(int i=0; i < num_fields; i++)
		fld.push_back(row[i]?Mess->codeConvIn(cd_pg.c_str(),row[i]):"");
	    tbl->push_back(fld);
	}
    }

    mysql_free_result(res);
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","BD",2,
	    "tp","str","help",
	    _("MySQL DB address writed in form: [<host>;<user>;<pass>;<db>;<port>;<u_sock>].\n"
	      "Where:\n"
	      "  host - MySQL server hostname;\n"
	      "  user - DB user name;\n"
	      "  pass - user's password for DB access;\n"
	      "  db - DB name;\n"
	      "  port - DB server port (default 3306);\n"
	      "  u_sock - UNIX-socket name, for local access to DB (/var/lib/mysql/mysql.sock).\n"
	      "For local DB: [;roman;123456;OpenSCADA;;/var/lib/mysql/mysql.sock].\n"
	      "For remote DB: [server.nm.org;roman;123456;OpenSCADA;3306]."));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//************************************************
//* MBDMySQL::Table                              *
//************************************************
MTable::MTable(string name, MBD *iown, bool create ) : TTable(name)
{
    string req;

    setNodePrev(iown);

    if( create )
    {
	req = "CREATE TABLE IF NOT EXISTS `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	    TSYS::strEncode(name,TSYS::SQL)+"` (`name` char(20) NOT NULL DEFAULT '' PRIMARY KEY)";
	owner().sqlReq( req );
    }
    //- Get table structure description -
    req ="DESCRIBE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+TSYS::strEncode(name,TSYS::SQL)+"`";
    owner().sqlReq(req,&tblStrct);
    //req = "SELECT * FROM `"+TSYS::strEncode(name,TSYS::SQL)+"` LIMIT 0,1";
    //owner().sqlReq( req );
}

MTable::~MTable(  )
{

}

void MTable::postDisable(int flag)
{
    if( flag )
    {
	try
	{
	    owner().sqlReq("DROP TABLE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
		TSYS::strEncode(name(),TSYS::SQL)+"`");
	}
	catch(TError err){ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

void MTable::fieldStruct( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	int pr1, pr2;
	string sid = tblStrct[i_fld][0];
	if( cfg.cfgPresent(sid) ) continue;

	int flg = (tblStrct[i_fld][3]=="PRI") ? (int)TCfg::Key : (int)TFld::NoFlag;
	if( sscanf(tblStrct[i_fld][1].c_str(),"char(%d)",&pr1) ||
		sscanf(tblStrct[i_fld][1].c_str(),"varchar(%d)",&pr1) )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,TSYS::int2str(pr1).c_str()) );
	else if( tblStrct[i_fld][1] == "text" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"65535") );
	else if( tblStrct[i_fld][1] == "mediumtext" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::String,flg,"16777215") );
	else if( sscanf(tblStrct[i_fld][1].c_str(),"int(%d)",&pr1) )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Integer,flg,TSYS::int2str(pr1).c_str()) );
	else if( sscanf(tblStrct[i_fld][1].c_str(),"double(%d,%d)",&pr1,&pr2) )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Real,flg,(TSYS::int2str(pr1)+"."+TSYS::int2str(pr2)).c_str()) );
	else if( tblStrct[i_fld][1] == "tinyint(1)" )
	    cfg.elem().fldAdd( new TFld(sid.c_str(),sid.c_str(),TFld::Boolean,flg,"1") );
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    //- Make SELECT and WHERE -
    string req = "SELECT ";
    string req_where = "WHERE ";
    //- Add use keys to list -
    bool first_sel = true;
    bool next = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	string sid = tblStrct[i_fld][0];
	if( !cfg.cfgPresent(sid) ) continue;
	TCfg &u_cfg = cfg.cfg(sid);
	if( u_cfg.fld().flg()&TCfg::Key && u_cfg.getS().size() )
	{
	    if( !next ) next = true;
	    else req_where=req_where+"AND ";
	    req_where=req_where+"`"+TSYS::strEncode(sid,TSYS::SQL)+"`='"+
			TSYS::strEncode(u_cfg.getS(),TSYS::SQL)+"' ";
	}
	else if( u_cfg.fld().flg()&TCfg::Key || u_cfg.view() )
	{
	    if( first_sel ) req=req+"`"+TSYS::strEncode(sid,TSYS::SQL)+"`";
	    else req=req+",`"+TSYS::strEncode(sid,TSYS::SQL)+"`";
	    first_sel = false;
	}
    }

    //- Request -
    if( first_sel ) return false;
    req = req + " FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+TSYS::strEncode(name(),TSYS::SQL)+"` "+
		 ((next)?req_where:"") +" LIMIT "+TSYS::int2str(row)+",1";
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) return false;
    for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
    {
	if( !cfg.cfgPresent(tbl[0][i_fld]) ) continue;
	string val = tbl[1][i_fld];
	TCfg &u_cfg = cfg.cfg(tbl[0][i_fld]);
	switch(u_cfg.fld().type())
	{
	    case TFld::String:	u_cfg.setS(val);		break;
	    case TFld::Integer:	u_cfg.setI(atoi(val.c_str()));	break;
	    case TFld::Real:	u_cfg.setR(atof(val.c_str()));	break;
	    case TFld::Boolean:	u_cfg.setB(atoi(val.c_str()));	break;
	}
    }
    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    //- Prepare request -
    string req = "SELECT ";
    string req_where;
    //-- Add fields list to queue --
    bool first_sel = true;
    bool next_wr = false;
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
	if( !cfg.cfgPresent(tblStrct[i_fld][0]) ) continue;
	TCfg &u_cfg = cfg.cfg(tblStrct[i_fld][0]);
	if( u_cfg.fld().flg()&TCfg::Key )
	{
	    if( !next_wr ) next_wr = true; else req_where=req_where+"AND ";
	    req_where=req_where+"`"+TSYS::strEncode(tblStrct[i_fld][0],TSYS::SQL)+"`='"+
		    TSYS::strEncode(u_cfg.getS(),TSYS::SQL)+"' ";
	}
	else if( u_cfg.view() )
	{
	    if( first_sel ) req=req+"`"+TSYS::strEncode(tblStrct[i_fld][0],TSYS::SQL)+"`";
	    else req=req+",`"+TSYS::strEncode(tblStrct[i_fld][0],TSYS::SQL)+"`";
	    first_sel = false;
	}
    }
    req = req+" FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	TSYS::strEncode(name(),TSYS::SQL)+"` WHERE "+req_where;

    //- Query -
    //printf("TEST 01: query: <%s>\n",req.c_str());
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) throw TError(TSYS::DBRowNoPresent,nodePath().c_str(),_("Row no present!"));

    //- Processing of query -
    for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
    {
	if( !cfg.cfgPresent(tbl[0][i_fld]) ) continue;
	string val = tbl[1][i_fld];
	TCfg &u_cfg = cfg.cfg(tbl[0][i_fld]);
	switch(u_cfg.fld().type())
	{
	    case TFld::String:	u_cfg.setS(val);		break;
	    case TFld::Integer:	u_cfg.setI(atoi(val.c_str()));	break;
	    case TFld::Real:	u_cfg.setR(atof(val.c_str()));	break;
	    case TFld::Boolean:	u_cfg.setB(atoi(val.c_str()));	break;
	}
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    //- Get config fields list -
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //- Get present fields list -
    string req_where = "WHERE ";
    //-- Add key list to query --
    bool next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( !(u_cfg.fld().flg()&TCfg::Key) ) continue;
	if( !next ) next = true; else req_where=req_where+"AND ";
	req_where=req_where+"`"+TSYS::strEncode(cf_el[i_el],TSYS::SQL)+"`='"+
				TSYS::strEncode(u_cfg.getS(),TSYS::SQL)+"' ";
    }

    //- Prepare query -
    //-- Try for get already present field --
    string req = "SELECT 1 FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	TSYS::strEncode(name(),TSYS::SQL)+"` "+req_where;
    try{ owner().sqlReq( req, &tbl ); }
    catch(TError err)	{ fieldFix(cfg); owner().sqlReq( req ); }
    if( tbl.size() < 2 )
    {
	//-- Add new record --
	req = "INSERT INTO `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	    TSYS::strEncode(name(),TSYS::SQL)+"` ";
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
	    ins_name=ins_name+"`"+TSYS::strEncode(cf_el[i_el],TSYS::SQL)+"` ";
	    string val;
	    switch(u_cfg.fld().type())
	    {
		case TFld::String:	val = u_cfg.getS();	break;
		case TFld::Integer:	val = SYS->int2str(u_cfg.getI());	break;
		case TFld::Real:	val = SYS->real2str(u_cfg.getR());	break;
		case TFld::Boolean:	val = SYS->int2str(u_cfg.getB());	break;
	    }
	    ins_value=ins_value+"'"+TSYS::strEncode(val,TSYS::SQL)+"' ";
	}
	req = req + "("+ins_name+") VALUES ("+ins_value+")";
    }
    else
    {
	//-- Update present record --
	req = "UPDATE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	    TSYS::strEncode(name(),TSYS::SQL)+"` SET ";
	next = false;
	for( int i_el = 0; i_el < cf_el.size(); i_el++ )
	{
	    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	    if( u_cfg.fld().flg()&TCfg::Key || !u_cfg.view() ) continue;

	    if( !next ) next = true; else req=req+",";
	    string val;
	    switch(u_cfg.fld().type())
	    {
		case TFld::String:	val = u_cfg.getS();			break;
		case TFld::Integer:	val = SYS->int2str(u_cfg.getI());	break;
		case TFld::Real:	val = SYS->real2str(u_cfg.getR());	break;
		case TFld::Boolean:	val = SYS->int2str(u_cfg.getB());	break;
	    }
	    req=req+"`"+TSYS::strEncode(cf_el[i_el],TSYS::SQL)+"`='"+
			TSYS::strEncode(val,TSYS::SQL)+"' ";
	}
	req = req + req_where;
    }

    //- Query -
    //printf("TEST 02: query: <%s>\n",req.c_str());
    try{ owner().sqlReq( req ); }
    catch(TError err)	{ fieldFix(cfg); owner().sqlReq( req ); }
}

void MTable::fieldDel( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    //- Get config fields list -
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //- Prepare request -
    string req = "DELETE FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	TSYS::strEncode(name(),TSYS::SQL)+"` WHERE ";
    //-- Add key list to query --
    bool next = false;
    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if( u_cfg.fld().flg()&TCfg::Key && u_cfg.getS().size() )
	{
	    if( !next ) next = true; else req=req+"AND ";
	    req=req+"`"+TSYS::strEncode(cf_el[i_el],TSYS::SQL)+"`='"+
			TSYS::strEncode(u_cfg.getS(),TSYS::SQL)+"' ";
	}
    }

    owner().sqlReq( req );
}

void MTable::fieldFix( TConfig &cfg )
{
    bool next = false, next_key = false;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));

    //- Get config fields list -
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //- Prepare request for fix structure -
    string req = "ALTER TABLE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
	TSYS::strEncode(name(),TSYS::SQL)+"` DROP PRIMARY KEY, ";

    string pr_keys;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
	//-- Check primary key --
	if( u_cfg.fld().flg()&TCfg::Key )
	{
	    if( !next_key ) next_key = true;
	    else pr_keys=pr_keys+",";
	    pr_keys=pr_keys+"`"+TSYS::strEncode(u_cfg.name(),TSYS::SQL)+"`";
	}
	
	int i_fld;
	for( i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
	    if( cf_el[i_cf] == tblStrct[i_fld][0] )	break;
	if( i_fld < tblStrct.size() )
	{
	    string f_tp;
	    switch(u_cfg.fld().type())
	    {
		    if( u_cfg.fld().len() < 256 || u_cfg.fld().flg()&TCfg::Key )
			f_tp = "varchar("+TSYS::int2str(vmax(1,vmin((u_cfg.fld().flg()&TCfg::Key)?150:255,u_cfg.fld().len())))+")";
		    else if( u_cfg.fld().len() < 65536 )
			f_tp = "text";
		    else f_tp = "mediumtext";
		    break;
		case TFld::Integer:
		    f_tp = "int("+TSYS::int2str(vmax(1,u_cfg.fld().len()))+")";
		    break;
		case TFld::Real:
		    f_tp = "double("+TSYS::int2str(vmax(3,u_cfg.fld().len()))+","+TSYS::int2str(vmax(2,u_cfg.fld().dec()))+")";
		    break;
		case TFld::Boolean:
		    f_tp = "tinyint(1)";
		    break;
	    }
	    if( tblStrct[i_fld][1] == f_tp )	continue;
	    //-- Change field --
	    if( !next ) next = true; else req=req+",";
	    req=req+"CHANGE `"+TSYS::strEncode(cf_el[i_cf],TSYS::SQL)+"` `"+TSYS::strEncode(cf_el[i_cf],TSYS::SQL)+"` ";
	    fieldPrmSet(u_cfg,(i_cf>0)?cf_el[i_cf-1]:"",req);
	}
	else
	{
	    if( !next ) next = true; else req=req+",";
	    //-- Add field --
	    req=req+"ADD `"+TSYS::strEncode(cf_el[i_cf],TSYS::SQL)+"` ";
	    fieldPrmSet(u_cfg,(i_cf>0)?cf_el[i_cf-1]:"",req);
	}
    }
    //- DROP fields -
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
	if( !cfg.cfgPresent(tblStrct[i_fld][0]) )
	{
	    if( !next ) next = true; else req=req+",";
	    req=req+"DROP `"+TSYS::strEncode(tblStrct[i_fld][0],TSYS::SQL)+"` ";
	}
    req=req+",ADD PRIMARY KEY ("+pr_keys+") ";

    if( next )
    {
	owner().sqlReq( req );
	//- Update structure information -
	req ="DESCRIBE `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+TSYS::strEncode(name(),TSYS::SQL)+"`";
	owner().sqlReq( req, &tblStrct );
    }
}

void MTable::fieldPrmSet( TCfg &cfg, const string &last, string &req )
{
    //- Type param -
    switch(cfg.fld().type())
    {
	case TFld::String:
	    if( cfg.fld().len() < 256 || cfg.fld().flg()&TCfg::Key )
		req=req+"varchar("+SYS->int2str(vmax(1,vmin((cfg.fld().flg()&TCfg::Key)?150:255,cfg.fld().len())))+") NOT NULL DEFAULT '"+cfg.fld().def()+"' ";
	    else if( cfg.fld().len() < 65536 )
		req=req+"text NOT NULL ";// DEFAULT '"+cfg.fld().def()+"' ";
	    else req=req+"mediumtext NOT NULL ";// DEFAULT '"+cfg.fld().def()+"' ";
	    break;
	case TFld::Integer:
	    req=req+"int("+SYS->int2str(vmax(1,cfg.fld().len()))+") NOT NULL DEFAULT '"+TSYS::int2str(atoi(cfg.fld().def().c_str()))+"' ";
	    break;
	case TFld::Real:
	    req=req+"double("+SYS->int2str(vmax(3,cfg.fld().len()))+","+SYS->int2str(vmax(2,cfg.fld().dec()))+") NOT NULL DEFAULT '"+TSYS::real2str(atof(cfg.fld().def().c_str()))+"' ";
	    break;
	case TFld::Boolean:
	    req=req+"tinyint(1) NOT NULL DEFAULT '"+TSYS::int2str(atoi(cfg.fld().def().c_str()))+"' ";
	    break;
    }
    //- Position param -
    //if( last.size() )	req=req+"AFTER `"+last+"` ";
}


//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module BD.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! System's includings. Add need for your module includings.
#include <string>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>
#include <tmess.h>

//!!! Self your module's includings. Add need for your module includings.
#include "module.h"

//!!! Module's meta-information. Change for your module.
//************************************************
//* Modul info!                                  *
#define MOD_ID		"Tmpl"
#define MOD_NAME	_("DB Tmpl")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		_("MyName MyFamily")
#define DESCRIPTION	_("BD Tmpl description.")
#define MOD_LICENSE	"MyLicense"
//************************************************

BDTmpl::BDMod *BDTmpl::mod; //Pointer for direct access to the module

//!!! Required section for binding OpenSCADA core to this module. It gives information and creates module root object.
//!!! Do not remove this section!
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }
#ifdef MOD_INCL
    TModule *bd_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new BDTmpl::BDMod(source);
	return NULL;
    }
}

//!!! Include for default enter to your module namespace.
using namespace BDTmpl;

//!!! Constructor for Root module object.
//************************************************
//* BDTmpl::BDMod				 *
//************************************************
BDMod::BDMod(string name) : TTipBD(MOD_ID)
{
    //!!! Init shortcut to module root object. Don't change it!
    mod		= this;

    //!!! Load module meta-information to root object. Don't change it!
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= MOD_LICENSE;
    mSource	= name;
}

//!!! Destructor for Root module object.
BDMod::~BDMod()
{

}

//!!! Processing the openBD function
TBD *BDMod::openBD( const string &name )
{
    return new MBD(name,&owner().openDB_E());
}


//!!! Constructor for DB-subsystem database object.
//************************************************
//* BDTmpl::MBD				         *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid,cf_el)
{

}

//!!! Destructor for DB-subsystem database object.
MBD::~MBD( )
{

}

//Override the default notice processor with the empty one. This is done to avoid the notice messages printing on stderr
/*void MyNoticeProcessor(void *arg, const char *message)
{
}*/

//!!! Processing the postDisable function
void MBD::postDisable(int flag)
{
    TBD::postDisable(flag);

    if( flag && owner().fullDeleteDB() )
    {
        //!!! Process here the full deleting of the database code
        ResAlloc resource(conn_res,true);
    }
}

//!!! Processing virtual functions for enable the database
void MBD::enable( )
{
    if( enableStat() )	return;
    //!!!Enabling of the database code processing:
    //!!! - establishing of the connection to the db server;
    //!!! - creating the database if it is not exist;
    //!!! - process the errors.
}

//!!! Processing virtual function for disable the database
void MBD::disable( )
{
    if( !enableStat() )  return;
    TBD::disable( );
    //!!! Process the code for the database disable
    ResAlloc resource(conn_res,true);
}

//!!! Processing virtual function to get the list of tables available in the database
void MBD::allowList( vector<string> &list )
{
    if( !enableStat() )  return;
    //!!! Prosess the code to get the list of available tables in the database
}

//!!! Processing the openTable function, don't change it!
TTable *MBD::openTable( const string &inm, bool create )
{
    if( !enableStat() )
	throw TError(TSYS::DBOpen,nodePath().c_str(),_("Error open table '%s'. DB is disabled."),inm.c_str());
    return new MTable(inm,this,create);
}

//!!! Processing function of the transaction starting
void MBD::transOpen( )
{
    ResAlloc resource(conn_res,true);
    //!!! Process the transaction starting code
}

//!!! Processing function of the transaction commit
void MBD::transCommit( )
{
    ResAlloc resource(conn_res,true);
    //!!! Process the transaction commit code
}

//!!! Processing virtual function to send the request to the  database and to process its result
void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl )
{
     if( tbl ) tbl->clear();
    if( !enableStat() ) return;

    string req = Mess->codeConvOut(cd_pg.c_str(),ireq);

    ResAlloc resource(conn_res,true);
    //!!! Process the request to the database sending code, the error checking and saving the result of the request
}

//!!!Control interface command process
void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,
	    "help",_("!!! Type here the help information about the db address of your module"));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//!!! Constructor for DB-subsystem table object.
//************************************************
//* BDTmpl::Table                                *
//************************************************
MTable::MTable(string name, MBD *iown, bool create ) : TTable(name)
{
    string req;

    setNodePrev(iown);
    //!!! Process the code of the table creation in the database if it is not exist there
    //!!! Get the db structure
    getStructDB( name, tblStrct );
}

//!!! Destructor for DB-subsystem table object.
MTable::~MTable(  )
{

}

//!!! Processing the postDisable function
void MTable::postDisable(int flag)
{
    //!!! Commit the transaction
    owner().transCommit();
    if( flag )
    {
        //!!! Process the code for the table drop
    }
}

//!!! Direct link to table's owner database
MBD &MTable::owner()	{ return (MBD&)TTable::owner(); }

//!!! Processing the getStructDB function
void MTable::getStructDB( string name, vector< vector<string> > &tblStrct )
{
    //- Get generic data structure -
    //!!! Commit the transaction
    owner().transCommit();
    //!!! Process the code to get the table's structure in the table with the following columns : ("Field", "Type", "Key")
}

//!!! Processing virtual function for getting the field's structure(value's type of the field and it's primary key flag )
void MTable::fieldStruct( TConfig &cfg )
{
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    for( int i_fld = 1; i_fld < tblStrct.size(); i_fld++ )
    {
        //!!! Process the code to get the each field's type	
    }
}

//!!! Processing virtual functions for seeking the field
bool MTable::fieldSeek( int row, TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    owner().transCommit();
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
    owner().sqlReq( req, &tbl );
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

//!!! Processing virtual functions for getting the field
void MTable::fieldGet( TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    owner().transCommit();
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
    owner().sqlReq( req, &tbl );
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

//!!! Processing virtual functions for setting the field
void MTable::fieldSet( TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    vector< vector<string> > tbl;

    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    owner().transOpen();
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
    try{ owner().sqlReq( req, &tbl ); }
    catch(TError err)	{ fieldFix(cfg); owner().sqlReq( req ); }
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
    try{ owner().sqlReq( req ); }
    catch(TError err)	{ fieldFix(cfg); owner().sqlReq( req ); }
}

//!!! Processing virtual functions for deleting the field
void MTable::fieldDel( TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    if( tblStrct.empty() ) throw TError(TSYS::DBTableEmpty,nodePath().c_str(),_("Table is empty!"));
    mLstUse = time(NULL);
    owner().transOpen();
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

    owner().sqlReq( req );
}

//!!! Processing the fieldFix function
void MTable::fieldFix( TConfig &cfg )
{
    owner().transCommit();
    //!!! Process the code here for the fields' fixing(change its type if it doesn't match to the necessary one)
    owner().transOpen();
}

//!!! Processing the getVal function
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

//!!! Processing the setVal function
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

//!!! Processing the UTCtoSQL function
string MTable::UTCtoSQL( time_t val )
{
    struct tm tm_tm;
    localtime_r(&val,&tm_tm);
    char buf[255];
    int rez = strftime( buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_tm );

    return (rez>0) ? string(buf,rez) : "";
}

//!!! Processing the SQLtoUTC function
time_t MTable::SQLtoUTC( const string &val )
{
    struct tm stm;
    strptime(val.c_str(),"%Y-%m-%d %H:%M:%S",&stm);

    return mktime(&stm);
}

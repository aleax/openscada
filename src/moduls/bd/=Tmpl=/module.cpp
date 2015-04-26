
//!!! The module name, the file name and the module's license. Change for your need.
//OpenSCADA system module BD.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily, <my@email.org>                 *
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

//!!! Constructor for the Root module object.
//************************************************
//* BDTmpl::BDMod				 *
//************************************************
BDMod::BDMod(string name) : TTypeBD(MOD_ID)
{
    //!!! Init shortcut to the module root object. Don't change it!
    mod		= this;

    //!!! Load the module meta-information to the root object. Don't change it!
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= MOD_LICENSE;
    mSource	= name;
}

//!!! Destructor for the Root module object.
BDMod::~BDMod( )
{

}

//!!! Processing the openBD function
TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }


//!!! Constructor for a DB-subsystem database object.
//************************************************
//* BDTmpl::MBD				         *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&connRes, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

//!!! Destructor for a DB-subsystem database object.
MBD::~MBD( )
{
    pthread_mutex_destroy(&connRes);
}

//!!! Post disable processing function
void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag && owner().fullDeleteDB()) {
	//!!! Process here the full deleting of the database code
	MtxAlloc resource(connRes, true);
    }
}

//!!! Enable the database object processing virtual functions
void MBD::enable( )
{
    MtxAlloc resource(connRes, true);
    if(enableStat())	return;

    //!!!Enabling of the database code processing:
    //!!! - establishing of the connection to the db server;
    //!!! - creating the database if it is not exist;
    //!!! - process the errors.

    TBD::enable();
}

//!!! Disable the database object processing virtual functions
void MBD::disable( )
{
    MtxAlloc resource(connRes, true);
    if(!enableStat())	return;
    TBD::disable();

    //!!!Disabling of the database code processing:
    //!!! - the connection to the db server disable and free all the resources;
}

//!!! Processing virtual function to get the list of tables available in the database
void MBD::allowList( vector<string> &list )
{
    if(!enableStat())	return;

    //!!! Code to get the list of available tables in the database
}

//!!! Open a table processing function, don't change it!
TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw TError(nodePath().c_str(), _("Error open table '%s'. DB is disabled."), inm.c_str());

    return new MTable(inm, this, create);
}

//!!! BEGIN a transaction processing functions
void MBD::transOpen( )
{
    MtxAlloc resource(connRes, true);

    //!!! The transaction starting code
}

//!!! COMMIT a transaction processing functions
void MBD::transCommit( )
{
    MtxAlloc resource(connRes, true);

    //!!! The transaction commit code
}

//!!! SQL request processing main virtual function
void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans );
{
    if(tbl) tbl->clear();
    if(!enableStat())	return;

    string req = Mess->codeConvOut(cdPg.c_str(), ireq);

    MtxAlloc resource(connRes, true);

    //!!! The request to the database sending code, the error checking and saving the result of the request
}

//!!!Control interface command process
void MBD::cntrCmdProc( XMLNode *opt )
{
    //Getting the page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,
	    "help",_("!!! Type here the help information about the db address of your module"));
	return;
    }
    TBD::cntrCmdProc(opt);
}

//!!! Constructor for a DB-subsystem table object.
//************************************************
//* BDTmpl::Table                                *
//************************************************
MTable::MTable( string name, MBD *iown, bool create ) : TTable(name)
{
    string req;

    setNodePrev(iown);

    //!!! The code of the table creation in the database if it is not exist there

    //!!! Get the table structure
    getStructDB(name, tblStrct);
}

//!!! Destructor for a DB-subsystem table object.
MTable::~MTable( )
{

}

//!!! Post disable processing function
void MTable::postDisable( int flag )
{
    //!!! Commit the transaction
    owner().transCommit();
    if(flag) {
        //!!! The code for the table drop
    }
}

//!!! Direct link to table's owner database
MBD &MTable::owner( )	{ return (MBD&)TTable::owner(); }

//!!! The table structure obtain
void MTable::getStructDB( string name, vector< vector<string> > &tblStrct )
{
    //Getting for the generic data structure

    //!!! Commit the transaction
    owner().transCommit();

    //!!! The code to get the table's structure with the following columns : ("Field", "Type", "Key")
}

//!!! Processing virtual function for getting the field's structure (value's type of the field and it's primary key flag)
void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty!"));
    mLstUse = time(NULL);
    for(int i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	//!!! The code to each field's type getting
    }
}

//!!! Processing virtual functions for seeking, getting, setting and deleting a field
bool MTable::fieldSeek( int row, TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty!"));
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

    string sid;
    //Make SELECT and WHERE
    string req = "SELECT ";
    string req_where = "WHERE ";
    //Add use keys to list
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
	    req_where += (next?"AND `":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`='" + TSYS::strEncode(getVal(*u_cfg),TSYS::SQL) + "' ";
	    next = true;
	}
	else if(u_cfg->isKey() || u_cfg->view()) {
	    req += (first_sel?"`":",`") + TSYS::strEncode(sid,TSYS::SQL) + "`";
	    first_sel = false;
	}
    }

    //Request
    if(first_sel) return false;
    req += " FROM `" + TSYS::strEncode(owner().bd,TSYS::SQL) + "`.`" + TSYS::strEncode(name(),TSYS::SQL) + "` " +
	((next)?req_where:"") + " LIMIT " + i2s(row) + ",1";
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) return false;
    for(unsigned i_fld = 0; i_fld < tbl[0].size(); i_fld++) {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg, tbl[1][i_fld]);
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[1][i_fld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setVal(*u_cfg, tbl[1][i_fld], true);
    }

    return true;
}

//!!! Processing virtual functions for getting the field
void MTable::fieldGet( TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty!"));
    mLstUse = SYS->sysTm();

    string sid;
    //Prepare request
    string req = "SELECT ";
    string req_where, first_key;
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
	    req_where += (next_wr?"AND `":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`='" + TSYS::strEncode(getVal(*u_cfg),TSYS::SQL) + "' ";
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
    if(tbl.size() < 2) throw TError(nodePath().c_str(), _("Row \"%s\" is not present."), req_where.c_str());

    //Processing of query
    for(unsigned i_fld = 0; i_fld < tbl[0].size(); i_fld++) {
	sid = tbl[0][i_fld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setVal(*u_cfg, tbl[1][i_fld]);
	else if(trPresent && sid.compare(0,3,Mess->lang2Code()+"#") == 0 && tbl[1][i_fld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setVal(*u_cfg, tbl[1][i_fld], true);
    }
}

//!!! Processing virtual functions for setting the field
void MTable::fieldSet( TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    vector< vector<string> > tbl;

    if(tblStrct.empty()) throw TError(nodePath().c_str(), _("Table is empty!"));
    mLstUse = SYS->sysTm();

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
    // Add key list to query
    bool next = false, noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();		//Force update by ReqKeys or reqKey() present
    for(unsigned i_el = 0; i_el < cf_el.size(); i_el++) {
	TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
	if(!u_cfg.isKey()) continue;
	req_where += (next?"AND `":"`") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "`='" +
					  TSYS::strEncode(getVal(u_cfg,TCfg::ExtValTwo),TSYS::SQL) + "' ";
	next = true;

	if(!isForceUpdt && u_cfg.extVal()) isForceUpdt = true;

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

		bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
		ins_name += (next?",`":"`") + TSYS::strEncode(cf_el[i_el],TSYS::SQL) + "` " +
		    (isTransl ? (",`"+TSYS::strEncode(Mess->lang2Code()+"#"+cf_el[i_el],TSYS::SQL)+"` ") : "");
		sval = getVal(u_cfg);
		ins_value += (next?",'":"'") + TSYS::strEncode(sval,TSYS::SQL) + "' " +
		    (isTransl ? (",'"+TSYS::strEncode(sval,TSYS::SQL)+"' ") : "");
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

	    bool isTransl = (u_cfg.fld().flg()&TCfg::TransltText && trPresent && !u_cfg.noTransl());
	    sid = isTransl ? (Mess->lang2Code()+"#"+cf_el[i_el]) : cf_el[i_el];
	    sval = getVal(u_cfg);
	    req += (next?",`":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`='" + TSYS::strEncode(sval,TSYS::SQL) + "' ";
	    next = true;
	}
	req += req_where;
    }

    //Query
    try { owner().sqlReq(req, NULL, true); }
    catch(TError err) {
	fieldFix(cfg);
	owner().sqlReq(req, NULL, true);
    }
}

//!!! Processing virtual functions for deleting the field
void MTable::fieldDel( TConfig &cfg )
{
    //!!! Check the syntax of the request's, it may differ in your database
    if(tblStrct.empty()) return;
    mLstUse = SYS->sysTm();

    //Where prepare
    string req_where = "WHERE ";
    bool next = false;
    for(unsigned i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	string sid = tblStrct[i_fld][0];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse()) {
	    req_where += (next?"AND `":"`") + TSYS::strEncode(sid,TSYS::SQL) + "`='" + TSYS::strEncode(getVal(*u_cfg),TSYS::SQL) + "' ";
	    next = true;
	}
    }

    //Main request
    try { owner().sqlReq("DELETE FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
					 TSYS::strEncode(name(),TSYS::SQL)+"` "+req_where, NULL, true);
    }
    catch(TError err) {
	//Check for present
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM `"+TSYS::strEncode(owner().bd,TSYS::SQL)+"`.`"+
					 TSYS::strEncode(name(),TSYS::SQL)+"` "+req_where, &tbl, true);
	if(tbl.size() < 2) return;
    }
}

//!!! The table structure fixing function to the field
void MTable::fieldFix( TConfig &cfg )
{
    owner().transCommit();

    //!!! The code here for the fields' fixing(change its type if it doesn't match to the necessary one)
    owner().transOpen();
}

//!!! Get field item's value processing functions
string MTable::getVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(cfg.fld().flg()&TFld::DateTimeDec) return UTCtoSQL(s2i(rez));
    return rez;
}

//!!! Processing the setVal function
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

//!!! Processing the UTCtoSQL function
string MTable::UTCtoSQL( time_t val )
{
    char buf[255];
    struct tm tm_tm;

    gmtime_r(&val, &tm_tm);
    int rez = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_tm);

    return (rez>0) ? string(buf,rez) : "";
}

//!!! Processing the SQLtoUTC function
time_t MTable::SQLtoUTC( const string &val )
{
    struct tm stm;
    strptime(val.c_str(), "%Y-%m-%d %H:%M:%S", &stm);

    return timegm(&stm);
}

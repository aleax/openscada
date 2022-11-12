
//OpenSCADA module BD.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// System includings - add need ones
#include <string.h>

// OpenSCADA API includings - add need ones
#include <tsys.h>
#include <tmess.h>

// Own includings of the module - add need ones
#include "module.h"

//************************************************
//* Module info!                                 *
#define MOD_ID		"Tmpl"
#define MOD_NAME	trS("DB Tmpl")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		trS("MyName MyFamily")
#define DESCRIPTION	trS("BD Tmpl description.")
#define MOD_LICENSE	"MyLicense"
#define FEATURES	"SQL"
//, LIST, STRUCT, GET, SEEK, PRELOAD, SET, DEL, FIX, TR, ERR"
//************************************************

BDTmpl::BDMod *BDTmpl::mod;

// Required section for binding OpenSCADA core to this module, It gives information and creates module root object - do not change
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

using namespace BDTmpl;

//************************************************
//* BDTmpl::BDMod				 *
//************************************************
BDMod::BDMod( string name ) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, MOD_LICENSE, name);
}

BDMod::~BDMod( )
{

}

string BDMod::features( ) { return FEATURES; }

TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }

//************************************************
//* BDTmpl::MBD				         *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el), reqCnt(0), reqCntTm(0), trOpenTm(0)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag&NodeRemove && owner().fullDeleteDB()) {
	//???? Process here the full deleting of the database
    }
}

void MBD::enable( )
{
    MtxAlloc res(connRes, true);
    if(enableStat())	return;

    //???? Enabling the database
    // - establishing of the connection to the db server;
    // - creating the database if it is not exist;
    // - processing the errors.

    TBD::enable();
}

void MBD::disable( )
{
    MtxAlloc res(connRes, true);
    if(!enableStat())	return;

    //Last commit
    if(reqCnt) try{ transCommit(); } catch(...) { }

    TBD::disable();

    //???? Disabling the database
    // - closing all transactions;
    // - disabling the db server connection connection and releasing all the resources.
}

void MBD::allowList( vector<string> &list ) const
{
    if(!enableStat())	return;
    list.clear();

    //???? Getting the list of available tables in the database
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error opening the table '%s'. DB is disabled."), inm.c_str());

    //???? The table creation in the database if it doesn't exist there and generation any exceptions

    return new MTable(inm, this);
}

void MBD::transOpen( )
{
    //Check for limit into one trinsaction
    if(reqCnt > 1000) transCommit();

    connRes.lock();
    bool begin = !reqCnt;
    if(begin) trOpenTm = TSYS::curTime();
    reqCnt++;
    reqCntTm = TSYS::curTime();
    connRes.unlock();

    if(begin) sqlReq("BEGIN;");

    //???? Or implement a very specific API of your DB
}

void MBD::transCommit( )
{
    connRes.lock();
    bool commit = reqCnt;
    reqCnt = reqCntTm = 0;
    connRes.unlock();

    if(commit) sqlReq("COMMIT;");

    //???? Or implement a very specific API of your DB
}

void MBD::transCloseCheck( )
{
    if(enableStat() && reqCnt && ((TSYS::curTime()-reqCntTm) > 1e6*trTm_ClsOnReq() || (TSYS::curTime()-trOpenTm) > 1e6*trTm_ClsOnOpen()))
	transCommit();
}

void MBD::getStructDB( const string &nm, vector<TTable::TStrIt> &tblStrct )
{
    vector< vector<string> > origTblStrct;

    //???? Requesting the table structure
}

void MBD::sqlReq( const string &req, vector< vector<string> > *tbl, char intoTrans )
{
    if(!enableStat())	return;
    if(tbl) tbl->clear();

    MtxAlloc res(connRes, true);

    //Commit set
    if(intoTrans && intoTrans != EVAL_BOOL) transOpen();
    else if(!intoTrans && reqCnt) transCommit();

    //???? Implementation the SQL-requests specific for the module
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,3,
	    "dest","sel_ed","select","/prm/cfg/dbFsList","help",
		    _("SQLite DB address must be written as: \"{FileDBPath}\".\n"
		      "Where:\n"
		      "  FileDBPath - full path to DB file (./oscada/Main.db).\n"
		      "               Use the empty path to create a temporary database on the disk.\n"
		      "               Use \":memory:\" to create a temporary database in memory."));
	if(reqCnt)
	    ctrMkNode("comm",opt,-1,"/prm/st/end_tr",_("Close opened transaction"),RWRW__,"root",SDB_ID);
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/dbFsList" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText(":memory:");
	TSYS::ctrListFS(opt, addr(), "db;");
    }
    else if(a_path == "/prm/st/end_tr" && ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR) && reqCnt) transCommit();
    else TBD::cntrCmdProc(opt);
}

//************************************************
//* BDTmpl::Table                                *
//************************************************
MTable::MTable( string name, MBD *iown ) : TTable(name)
{
    setNodePrev(iown);

    try { owner().getStructDB(name, tblStrct); } catch(...) { }
}

MTable::~MTable( )
{

}

void MTable::postDisable( int flag )
{
    owner().transCommit();
    if(flag&NodeRemove) {
        //???? Physical dropping commands of the table
    }
}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    if(tblStrct.empty()) throw err_sys(_("Table is empty!"));
    mLstUse = time(NULL);

    for(int i_fld = 1; i_fld < tblStrct.size(); i_fld++) {
	//???? Getting for each field type
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg, const string &cacheKey )
{
    return fieldSQLSeek(row, cfg, cacheKey);

    //???? Or implement a very specific API of your DB
}

void MTable::fieldGet( TConfig &cfg )
{
    fieldSQLGet(cfg);

    //???? Or implement a very specific API of your DB
}

void MTable::fieldSet( TConfig &cfg )
{
    if(tblStrct.empty()) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }

    fieldSQLSet(cfg);

    //???? Or implement a very specific API of your DB
}

void MTable::fieldDel( TConfig &cfg )
{
    fieldSQLDel(cfg);

    //???? Or implement a very specific API of your DB
}

void MTable::fieldFix( TConfig &cfg, const string &langLs )
{
    owner().transCommit();

    //???? Implement for specific of the DB correcting

    owner().transOpen();
}

string MTable::getSQLVal( TCfg &cfg, uint8_t RqFlg )
{
    string rez = cfg.getS(RqFlg);
    if(rez == EVAL_STR)	return "NULL";

    //???? Implement the data encoding specific

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

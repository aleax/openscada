
//OpenSCADA file: tbds.cpp
/***************************************************************************
 *   Copyright (C) 2003-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <unistd.h>

#include "tsys.h"
#include "tmess.h"
#include "tmodule.h"
#include "tbds.h"

using namespace OSCADA;

//************************************************
//* TBDS                                         *
//************************************************
TBDS::TBDS( ) : TSubSYS(SDB_ID,_("Data Bases"),true), mSYSStPref(true)
{
    //Generic system DB
    fldAdd(new TFld("user","User",TFld::String,TCfg::Key,OBJ_ID_SZ));
    fldAdd(new TFld("id",_("Value ID"),TFld::String,TCfg::Key,"100"));
    fldAdd(new TFld("val","Value"  ,TFld::String,TFld::TransltText,"1000"));

    //Open data bases DB structure
    elDB.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    elDB.fldAdd(new TFld("TYPE",_("DB type (module)"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    elDB.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    elDB.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"2000"));
    elDB.fldAdd(new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"1000"));
    elDB.fldAdd(new TFld("CODEPAGE",_("Code page"),TFld::String,TFld::NoFlag,"20"));
    elDB.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,TFld::NoFlag,"1","1"));
    elDB.fldAdd(new TFld("TRTM_CLS_ON_OPEN",_("Transaction closing: after opening, seconds"),TFld::Real,TFld::NoFlag,"4.1",i2s(3*SERV_TASK_PER).c_str()));
    elDB.fldAdd(new TFld("TRTM_CLS_ON_REQ",_("Transaction closing: after request, seconds"),TFld::Real,TFld::NoFlag,"4.1",i2s(SERV_TASK_PER).c_str()));
    elDB.fldAdd(new TFld("TRPR_CLS_TASK",_("Transaction closing: separate task priority"),TFld::Integer,TFld::NoFlag,"3","0"));
}

TBDS::~TBDS( )	{ }

string TBDS::realDBName( const string &bdn )
{
    int off = 0;
    string bd_t = TSYS::strParse(bdn,0,".",&off);
    string bd_n = TSYS::strParse(bdn,0,".",&off);
    string bd_tbl = TSYS::strParse(bdn,0,".",&off);

    if(bd_t == DB_CFG)	return bdn;
    if(SYS->workDB() == DB_CFG && bd_t == "*" && bd_n == "*")	return DB_CFG"."+bd_tbl;
    return ((bd_t=="*") ? TSYS::strParse(SYS->workDB(),0,".") : bd_t)+"."+
	   ((bd_n=="*") ? TSYS::strParse(SYS->workDB(),1,".") : bd_n)+(bd_tbl.empty() ? "" : "."+bd_tbl);
}

void TBDS::dbList( vector<string> &ls, bool checkSel )
{
    ls.clear();

    if(checkSel && !SYS->selDB().empty()) {
	if(SYS->selDB() != DB_CFG) ls.push_back(SYS->selDB());
	return;
    }

    vector<string> tdb_ls, db_ls;
    modList(tdb_ls);
    for(unsigned iTp = 0; iTp < tdb_ls.size(); iTp++) {
	SYS->db().at().at(tdb_ls[iTp]).at().list(db_ls);
	for(unsigned iDB = 0; iDB < db_ls.size(); iDB++)
	    ls.push_back(tdb_ls[iTp]+"."+db_ls[iDB]);
    }
}

void TBDS::perSYSCall( unsigned int cnt )
{
    int secOld = 600;
    vector<string> tdbs, dbs, tbls;

    try {
	modList(tdbs);
	for(unsigned iTDB = 0; iTDB < tdbs.size(); iTDB++) {
	    at(tdbs[iTDB]).at().list(dbs);
	    for(unsigned iDB = 0; iDB < dbs.size(); iDB++) {
		AutoHD<TBD> db = at(tdbs[iTDB]).at().at(dbs[iDB]);
		if(db.at().enableStat()) {
		    //Closing for not used tables
		    db.at().list(tbls);
		    for(unsigned iTbl = 0; iTbl < tbls.size(); iTbl++) {
			AutoHD<TTable> tbl = db.at().at(tbls[iTbl]);
			if((time(NULL)-tbl.at().lstUse()) > secOld) {
			    tbl.free();
			    db.at().close(tbls[iTbl]);
			}
		    }
		    //Checking for transaction close
		    if(db.at().trTm_ClsOnReq() >= SERV_TASK_PER) db.at().transCloseCheck();
		}
		else { if(db.at().toEnable() && !db.at().disabledByUser() && !SYS->stopSignal()) db.at().enable(); }
	    }
	}
    } catch(...){ }

    TSubSYS::perSYSCall(cnt);
}

AutoHD<TTable> TBDS::open( const string &bdn, bool create )
{
    AutoHD<TTable> tbl;

    try {
	string bdT = TSYS::strSepParse(bdn,0,'.');
	string bdN = TSYS::strSepParse(bdn,1,'.');
	string bdTbl = TSYS::strSepParse(bdn,2,'.');
	if(bdT == "*") bdT = TSYS::strSepParse(SYS->workDB(),0,'.');
	if(bdN == "*") bdN = TSYS::strSepParse(SYS->workDB(),1,'.');
	if(bdT == DB_CFG) return tbl;
	AutoHD<TBD> obd = at(bdT).at().at(bdN);
	MtxAlloc res(obd.at().resTbls);	//!!!! For prevent multiple entry and creation try
	if(obd.at().enableStat()) {
	    if(!obd.at().openStat(bdTbl)) obd.at().open(bdTbl, create);
	    tbl = obd.at().at(bdTbl);
	}
    } catch(TError &err) {
	//mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
    }

    return tbl;
}

void TBDS::close( const string &bdn, bool del )
{
    try {
	string bdT = TSYS::strSepParse(bdn,0,'.');
	string bdN = TSYS::strSepParse(bdn,1,'.');
	string bdTbl = TSYS::strSepParse(bdn,2,'.');
	if(bdT == "*") bdT = TSYS::strSepParse(SYS->workDB(),0,'.');
	if(bdN == "*") bdN = TSYS::strSepParse(SYS->workDB(),1,'.');
	if(bdT == DB_CFG) return;
	AutoHD<TBD> obd = at(bdT).at().at(bdN);
	MtxAlloc res(obd.at().resTbls);	//!!!! For prevent multiple entry and closing try
	if(obd.at().enableStat() && obd.at().openStat(bdTbl) && obd.at().at(bdTbl).at().nodeUse() == 1)
	    obd.at().close(bdTbl, del);
    } catch(TError &err) {
	mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Warning, _("Error closing database '%s'!"), bdn.c_str());
    }
}

string TBDS::fullDBSYS( )	{ return SYS->workDB()+".SYS"; }

string TBDS::fullDB( )		{ return SYS->workDB()+".DB"; }

bool TBDS::dataSeek( const string &ibdn, const string &path, int lev, TConfig &cfg, bool forceCfg, vector< vector<string> > *full )
{
    int c_lev = 0;
    string bdn = realDBName(ibdn);

    cfg.cfgToDefault();	//reset the not key and viewed fields

    if(path.size() && (forceCfg || ibdn.empty() || TSYS::strParse(bdn,0,".") == DB_CFG)) {
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *nd, *fnd = NULL, *el;
	string vl, vl_tr;
	bool isPresent = false;
	vector<string> cf_el;

	nd = SYS->cfgNode(SYS->id()+"/"+path);
	for(unsigned iFld = 0, iEl; nd && iFld < nd->childSize(); iFld++) {
	    el = nd->childGet(iFld);
	    if(el->name() == "fld") {
		cfg.cfgList(cf_el);

		//Check keywords
		for(iEl = 0; iEl < cf_el.size(); iEl++)
		    if(cfg.cfg(cf_el[iEl]).isKey() && cfg.cfg(cf_el[iEl]).keyUse() &&
		        cfg.cfg(cf_el[iEl]).getS() != el->attr(cf_el[iEl])) break;
		if(iEl == cf_el.size() && lev <= c_lev++) {
		    for(iEl = 0; iEl < cf_el.size(); iEl++) {
			TCfg &cf = cfg.cfg(cf_el[iEl]);
			vl = el->attr(cf_el[iEl], true, &isPresent);
			// Check for field's tag, for store big values
			if(!isPresent && (fnd=el->childGet(cf_el[iEl],0,true))) { vl = fnd->text(true); isPresent = true; }
			// Check for translation
			if((cf.fld().flg()&TFld::TransltText) && !cf.noTransl()) {
			    vl_tr = "";
			    if(Mess->lang2CodeBase().empty() || Mess->lang2Code() != Mess->lang2CodeBase()) {
				vl_tr = el->attr(cf_el[iEl]+"_"+Mess->lang2Code());
				// Check for field's tag, for store big values
				if(vl_tr.empty() && (fnd=el->childGet(cf_el[iEl]+"_"+Mess->lang2Code(),0,true))) vl_tr = fnd->text(true);
			    }
			    if(!cf.extVal()) {
				cf.setS((vl_tr.size() && !Mess->translDyn()) ? vl_tr : vl);
				Mess->translReg(vl, "cfg:"+path+"#"+cf_el[iEl]);
			    }
			    else {
				cf.setS(vl, TCfg::ExtValOne);
				cf.setS(vl_tr, TCfg::ExtValTwo);
				cf.setS("cfg:"+path+"#"+cf_el[iEl], TCfg::ExtValThree);
			    }
			} else if(isPresent)	cf.setS(vl);
		    }
		    return true;
		}
	    }
	}
    }

    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = open(bdn);
	if(!tbl.freeStat()) {
	    bool rez = tbl.at().fieldSeek(lev-c_lev, cfg, full);
	    //tbl.free(); close(bdn);
	    return rez;
	}
    }

    return false;
}

bool TBDS::dataGet( const string &ibdn, const string &path, TConfig &cfg, bool forceCfg, bool noEx )
{
    bool db_true = false;
    string bdn = realDBName(ibdn);
    TError dbErr;

    //Load from DB
    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = open(bdn);
	if(!tbl.freeStat()) {
	    db_true = true;
	    try { tbl.at().fieldGet(cfg); }
	    catch(TError &err) {
		db_true = false;
		dbErr = err;
	    }
	    if(db_true && !forceCfg) return true;
	}
    }

    //Load from Config-file if tbl no present
    ResAlloc res(SYS->cfgRes(), false);
    XMLNode *nd, *fnd = NULL, *el;
    string vl, vl_tr;
    bool isPresent = false;
    vector<string> cf_el;

    nd = SYS->cfgNode(SYS->id()+"/"+path);

    // Scan fields and fill Configuration
    for(unsigned iFld = 0, iEl; nd && iFld < nd->childSize(); iFld++) {
	el = nd->childGet(iFld);
	if(el->name() == "fld") {
	    cfg.cfgList(cf_el);

	    //  Check keywords
	    for(iEl = 0; iEl < cf_el.size(); iEl++)
		if(cfg.cfg(cf_el[iEl]).isKey() && cfg.cfg(cf_el[iEl]).getS() != el->attr(cf_el[iEl])) break;
	    if(iEl == cf_el.size()) {
		for(iEl = 0; iEl < cf_el.size(); iEl++) {
		    TCfg &cf = cfg.cfg(cf_el[iEl]);
		    vl = el->attr(cf_el[iEl], true, &isPresent);
		    //  Check for field's tag, for store big values
		    if(!isPresent && (fnd=el->childGet(cf_el[iEl],0,true))) { vl = fnd->text(true); isPresent = true; }
		    //  Check for translation
		    if((cf.fld().flg()&TFld::TransltText) && !cf.noTransl()) {
			vl_tr = "";
			if(Mess->lang2CodeBase().empty() || Mess->lang2Code() != Mess->lang2CodeBase()) {
			    vl_tr = el->attr(cf_el[iEl]+"_"+Mess->lang2Code());
			    //  Check for field's tag, for store big values
			    if(vl_tr.empty() && (fnd=el->childGet(cf_el[iEl]+"_"+Mess->lang2Code(),0,true))) vl_tr = fnd->text(true);
			}
			if(!cf.extVal()) {
			    cf.setS((vl_tr.size() && !Mess->translDyn()) ? vl_tr : vl);
			    Mess->translReg(vl, "cfg:"+path+"#"+cf_el[iEl]);
			}
			else {
			    cf.setS(vl, TCfg::ExtValOne);
			    cf.setS(vl_tr, TCfg::ExtValTwo);
			    cf.setS("cfg:"+path+"#"+cf_el[iEl], TCfg::ExtValThree);
			}
		    } else if(isPresent)	cf.setS(vl);
		}
		return true;
	    }
	}
    }

    if(!db_true && !noEx) {
	if(dbErr.cat.empty()) throw err_sys("%s", dbErr.mess.empty() ? _("The requested entry is missing.") : dbErr.mess.c_str());
	throw TError(dbErr.cat.c_str(), "%s", dbErr.mess.empty() ? _("The requested entry is missing.") : dbErr.mess.c_str());
    }

    return db_true;
}

bool TBDS::dataSet( const string &ibdn, const string &path, TConfig &cfg, bool forceCfg, bool noEx )
{
    string bdn = realDBName(ibdn);

    //Save to DB
    if(!forceCfg && bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = open(bdn,true);
	if(!tbl.freeStat()) {
	    bool db_true = true;
	    try { tbl.at().fieldSet(cfg); }
	    catch(TError &err) {
		//mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		db_true = false;
		if(!noEx) throw;
	    }
	    return db_true;
	}
    }

    //Save to config
    if(forceCfg || TSYS::strParse(bdn,0,".") == DB_CFG) {
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *nd, *wel = NULL, *fnd;
	vector<string> cf_el;
	string vnm;

	SYS->modifCfg(true);

	if((nd=SYS->cfgNode(SYS->id()+"/"+path,true))) {
	    cfg.cfgList(cf_el);
	    if(nd->name() != "tbl")	nd->setName("tbl");

	    // Search present field
	    for(unsigned iFld = 0, iEl; iFld < nd->childSize(); iFld++) {
		XMLNode *el = nd->childGet(iFld);
		if(el->name() != "fld")	continue;
		//  Check keywords
		for(iEl = 0; iEl < cf_el.size(); iEl++)
		    if(cfg.cfg(cf_el[iEl]).isKey() && cfg.cfg(cf_el[iEl]).getS(TCfg::ExtValTwo) != el->attr(cf_el[iEl])) break;
		if(iEl == cf_el.size()) { wel = el; break; }
	    }

	    bool isCreate = !wel;
	    if(!wel) wel = nd->childAdd("fld");
	    for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
		vnm = cf_el[iEl];
		TCfg &cf = cfg.cfg(vnm);
		bool isTransl = (cf.fld().flg()&TFld::TransltText && !cf.noTransl() &&
				Mess->lang2CodeBase().size() && Mess->lang2Code() != Mess->lang2CodeBase());
		if(isCreate || !isTransl) {
		    if(cf.getS().size() < 100) {
			wel->setAttr(vnm, cf.getS());
			if((fnd=wel->childGet(vnm,0,true))) wel->childDel(fnd);
		    }
		    else {
			if(!(fnd=wel->childGet(vnm,0,true))) fnd = wel->childAdd(vnm);
			fnd->setText(cf.getS(), true);
			wel->setAttr(vnm, "");
		    }
		}
		if(isTransl) {
		    vnm = cf_el[iEl]+"_"+Mess->lang2Code();
		    if(cf.getS().size() < 100) {
			wel->setAttr(vnm, (Mess->translDyn()?trL(cf.getS(),Mess->lang2Code()):cf.getS()));
			if((fnd=wel->childGet(vnm,0,true))) wel->childDel(fnd);
		    }
		    else {
			if(!(fnd=wel->childGet(vnm,0,true))) fnd = wel->childAdd(vnm);
			fnd->setText(cf.getS(), true);
			wel->setAttr(vnm, "");
		    }
		}
	    }
	    SYS->modifCfg();
	    return true;
	}
    }

    if(!noEx) throw err_sys(_("Error writing to DB or configuration file."));

    return false;
}

bool TBDS::dataDel( const string &ibdn, const string &path, TConfig &cfg, bool useKeyAll, bool forceCfg, bool noEx )
{
    vector<string> cels;
    string bdn = realDBName(ibdn);
    bool db_true = false;
    TError dbErr;

    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = open(bdn);
	if(!tbl.freeStat()) {
	    try {
		//Select for using all keys
		if(useKeyAll) {
		    cfg.cfgList(cels);
		    for(unsigned iEl = 0; iEl < cels.size(); ) {
			if(cfg.cfg(cels[iEl]).isKey() && !cfg.cfg(cels[iEl]).keyUse()) cfg.cfg(cels[iEl]).setKeyUse(true);
			else { cels.erase(cels.begin()+iEl); continue; }
			iEl++;
		    }
		}

		tbl.at().fieldDel(cfg);

		db_true = true;

		//Restore for not using keys selection
		if(useKeyAll)
		    for(unsigned iEl = 0; iEl < cels.size(); iEl++)
			cfg.cfg(cels[iEl]).setKeyUse(false);
	    } catch(TError &err) {
		dbErr = err;
		//mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	    }
	    //tbl.free(); close(bdn);
	}
    }

    //Delete from config
    if(path.size() && (forceCfg || ibdn.empty() || TSYS::strParse(bdn,0,".") == DB_CFG || !db_true)) {
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *nd = SYS->cfgNode(SYS->id()+"/"+path, true);
	vector<string> cf_el;

	// Search present field
	for(unsigned iFld = 0, iEl; nd && iFld < nd->childSize(); iFld++) {
	    XMLNode *el = nd->childGet(iFld);
	    if(el->name() != "fld")	continue;
	    //Check keywords
	    cfg.cfgList(cf_el);
	    for(iEl = 0; iEl < cf_el.size(); iEl++)
	        if(cfg.cfg(cf_el[iEl]).isKey() && cfg.cfg(cf_el[iEl]).getS() != el->attr(cf_el[iEl])) break;
	    if(iEl == cf_el.size()) {
		SYS->modifCfg(true);
		nd->childDel(iFld);
		SYS->modifCfg();
		return true;
	    }
	}
    }

    if(!db_true && !noEx && !dbErr.cat.empty()) throw TError(dbErr.cat.c_str(), "%s", dbErr.mess.c_str());

    return db_true;
}

void TBDS::genDBSet( const string &path, const string &val, const string &user, char rFlg )
{
    bool bd_ok = false;

    //Set to DB
    if(SYS->present(SDB_ID) && !(rFlg&TBDS::OnlyCfg)) {
	AutoHD<TBDS> dbs = SYS->db();
	AutoHD<TTable> tbl = dbs.at().open(dbs.at().fullDBSYS(), true);
	if(!tbl.freeStat()) {
	    TConfig db_el(&dbs.at());
	    db_el.cfg("user").setS(user);
	    db_el.cfg("id").setS(dbs.at().mSYSStPref ? SYS->id()+"/"+path : path);
	    db_el.cfg("val").setNoTransl(!(rFlg&TBDS::UseTranslate));
	    db_el.cfg("val").setS(val);

	    try {
		tbl.at().fieldSet(db_el);
		bd_ok = true;
	    } catch(TError &err){ }
	}
    }

    //Set to config
    if(!bd_ok && (SYS->workDB() == DB_CFG || rFlg&TBDS::OnlyCfg)) {
	if(genDBGet(path,"",user,(rFlg|OnlyCfg)) == val) return;
	ResAlloc res(SYS->cfgRes(), true);
	XMLNode *tgtN = NULL;
	if((rFlg&TBDS::UseTranslate) && Mess->lang2Code().size())
	    tgtN = SYS->cfgNode(SYS->id()+"/"+path+"_"+Mess->lang2Code(), true);
	if(!tgtN) tgtN = SYS->cfgNode(SYS->id()+"/"+path, true);
	if(tgtN) { tgtN->setText(val,true); SYS->modifCfg(); }
    }
}

string TBDS::genDBGet( const string &path, const string &oval, const string &user, char rFlg )
{
    bool bd_ok = false;
    string rez = oval;

    //Get from generic DB
    if(SYS->present(SDB_ID) && !(rFlg&TBDS::OnlyCfg)) {
	AutoHD<TBDS> dbs = SYS->db();
	AutoHD<TTable> tbl = dbs.at().open(SYS->db().at().fullDBSYS());
	if(!tbl.freeStat()) {
	    TConfig db_el(&dbs.at());
	    db_el.cfg("user").setS(user);
	    db_el.cfg("id").setS(dbs.at().mSYSStPref ? SYS->id()+"/"+path : path);
	    db_el.cfg("val").setNoTransl(!(rFlg&TBDS::UseTranslate));
	    try {
		tbl.at().fieldGet(db_el);
		rez = db_el.cfg("val").getS();
		bd_ok = true;
	    } catch(TError &err) {  }
	}
    }

    if(!bd_ok) {
	//Get from config-file
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *tgtN = NULL;
	if(rFlg&TBDS::UseTranslate) {
	    //if((tgtN=SYS->cfgNode(SYS->id()+"/"+path))) Mess->translReg(tgtN->text(true), "cfgSYS:"+path);
	    if(Mess->lang2Code().size()) tgtN = SYS->cfgNode(SYS->id()+"/"+path+"_"+Mess->lang2Code());
	}
	if(!tgtN) tgtN = SYS->cfgNode(SYS->id()+"/"+path);
	if(tgtN) rez = tgtN->text(true);
    }

    return rez;
}

string TBDS::optDescr( )
{
    return TSYS::strMess(_(
	"========================= Subsystem \"DB\" options ========================\n"
	"------ Parameters of the section '%s' of the configuration file ------\n"
	"SYSStPref  <0|1>        Use the station ID in the common table (SYS).\n\n"
	),nodePath().c_str()) + TSubSYS::optDescr();
}

void TBDS::load_( )
{
    //Load parameters from command line

    //Load parameters from config-file
    mSYSStPref = (bool)s2i(TBDS::genDBGet(nodePath()+"SYSStPref",(mSYSStPref?"1":"0"),"root",TBDS::OnlyCfg));

    //DB open
    // Open, load and enable generic DB
    string db_tp = TSYS::strSepParse(fullDB(), 0, '.');
    string db_nm = TSYS::strSepParse(fullDB(), 1, '.');
    if(modPresent(db_tp) && !at(db_tp).at().openStat(db_nm)) {
	at(db_tp).at().open(db_nm);
	at(db_tp).at().at(db_nm).at().load();
	at(db_tp).at().at(db_nm).at().enable();
    }

    // Open other DB stored into the table 'DB' and the config-file
    try {
	string id, type;
	//if(SYS->chkSelDB(fullDB())) {	//!!!! Must be forced one for config file rescan, release test
	    TConfig c_el(&elDB);
	    //c_el.cfgViewAll(false);
	    vector<vector<string> > full;
	    for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB(),nodePath()+"DB/",fldCnt++,c_el,true,&full); ) {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("TYPE").getS();
		if(!modPresent(type))	continue;
		if((type+"."+id) != SYS->workDB() && !at(type).at().openStat(id))
		    at(type).at().open(id);
		try{ at(type).at().at(id).at().load(&c_el); } catch(TError&) { }
	    }
	//}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and opening a new database."));
    }
}

void TBDS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTypeBD                                      *
//************************************************
TTypeBD::TTypeBD( const string &id ) : TModule(id), fullDBDel(false)
{
    mDB = grpAdd("db_");
}

TTypeBD::~TTypeBD( )
{
    nodeDelAll();
}

string TTypeBD::open( const string &iid )
{
    if(openStat(iid)) return iid;

    return chldAdd(mDB, openBD(TSYS::strEncode(sTrm(iid),TSYS::oscdID)));
}

TBDS &TTypeBD::owner( ) const	{ return (TBDS&)TModule::owner(); }

void TTypeBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/db_",_("DB"),RWRWR_,"root",SDB_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/db",_("DB"),R_R_R_)) {
	    ctrMkNode("fld",opt,-1,"/db/ful_db_del",_("Complete DB removal"),RWRW__,"root",SDB_ID,2,
		"tp","bool","help",_("Select to completely remove the database when closing, otherwise the DB will simply be closed."));
	    ctrMkNode("list",opt,-1,"/db/odb",_("DB"),RWRWR_,"root",SDB_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","db_","idSz",OBJ_ID_SZ);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/db/ful_db_del") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD))	opt->setText(fullDBDel?"1":"0");
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR))	fullDBDel = s2i(opt->text());
    }
    else if(a_path == "/br/db_" || a_path == "/db/odb") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		opt->childAdd("el")->setAttr("id",lst[iL])->setText(at(lst[iL]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDB_ID,SEC_WR))	{ opt->setAttr("id", open(opt->attr("id"))); at(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDB_ID,SEC_WR))	close(opt->attr("id"), true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* TBD                                          *
//************************************************
TBD::TBD( const string &iid, TElem *cf_el ) : TConfig(cf_el), mId(cfg("ID")), mToEn(cfg("EN").getBd()),
    mTrTm_ClsOnOpen(cfg("TRTM_CLS_ON_OPEN").getRd()), mTrTm_ClsOnReq(cfg("TRTM_CLS_ON_REQ").getRd()), mTrPr_ClsTask(cfg("TRPR_CLS_TASK").getId()),
    mEn(false), userSQLTrans(EVAL_BOOL), mDisByUser(true)
{
    mId = iid;
    mTbl = grpAdd("tbl_");
}

TCntrNode &TBD::operator=( const TCntrNode &node )
{
    const TBD *src_n = dynamic_cast<const TBD*>(&node);
    if(!src_n) return *this;

    if(!enableStat()) {
	exclCopy(*src_n, "ID;");
	cfg("TYPE").setS(owner().modId());
	modif();
    }

    if(src_n->enableStat() && enableStat()) {
	vector<string> tbl_ls;
	src_n->allowList(tbl_ls);
	for(unsigned iL = 0; iL < tbl_ls.size(); iL++) {
	    //Open source and destination tables
	    const_cast<TBD*>(src_n)->open(tbl_ls[iL], false);
	    open(tbl_ls[iL], true);

	    //Copy table
	    (TCntrNode&)at(tbl_ls[iL]).at() = (TCntrNode&)src_n->at(tbl_ls[iL]).at();

	    //Close source and destination tables
	    const_cast<TBD*>(src_n)->close(tbl_ls[iL]);
	    close(tbl_ls[iL]);
	}
    }

    return *this;
}

TBD::~TBD( )	{ }

void TBD::postEnable( int flag )	{ cfg("TYPE").setS(owner().modId()); }

void TBD::preDisable( int flag )
{
    disable();

    SYS->taskDestroy(nodePath('.',true), NULL, 0);	//wait to complete the task stopping
}

void TBD::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(owner().owner().fullDB(), SYS->db().at().nodePath()+"DB/", *this, true, true);
}

bool TBD::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "TRTM_CLS_ON_OPEN" || co.name() == "TRTM_CLS_ON_REQ") {
	mTrTm_ClsOnOpen = vmax(0.1, vmin(100,mTrTm_ClsOnOpen));
	mTrTm_ClsOnReq = vmax(0.1, vmin(mTrTm_ClsOnOpen,mTrTm_ClsOnReq));
	if(enableStat()) disable();
    }
    if(enableStat() && co.name() == "TRPR_CLS_TASK")	disable();

    modif();
    return true;
}

TTypeBD &TBD::owner( ) const	{ return *(TTypeBD*)nodePrev(); }

string TBD::fullDBName( )	{ return owner().modId()+"."+id(); }

string TBD::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : id();
}

void TBD::enable( )
{
    if(enableStat()) return;

    mEn = true; mDisByUser = false;

    Mess->translReg("", "uapi:"+fullDBName());

    if(trTm_ClsOnReq() < SERV_TASK_PER)
	try { SYS->taskCreate(nodePath('.',true), trPr_ClsTask(), Task, this, 0); }
	catch(TError&) { }	//Can be for retry to enable the DB
}

void TBD::disable( )
{
    if(!enableStat()) return;

    //Close all tables
    //!!!! Comment the part for omit tables closing temporary before the AutoHD proper locking fix, mostly reproduced on remote MySQL
    try {
	vector<string> t_list;
	list(t_list);
	for(unsigned iL = 0; iL < t_list.size(); iL++)
	    close(t_list[iL], false, 1);
    } catch(...) { }	//Pass removing for locked

    SYS->taskDestroy(nodePath('.',true), NULL, 1);	//try to stop the task

    mEn = false;
}

void TBD::open( const string &table, bool create )
{
    if(!chldPresent(mTbl,table)) chldAdd(mTbl, openTable(table,create));
}

void TBD::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB_CFG))	throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else SYS->db().at().dataGet(owner().owner().fullDB(), SYS->db().at().nodePath()+"DB/", *this, true);

    if(!enableStat() && toEnable()) enable();
}

void TBD::save_( )
{
    SYS->db().at().dataSet(owner().owner().fullDB(), SYS->db().at().nodePath()+"DB/", *this, true);
}

TVariant TBD::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // Array SQLReq(string req, bool tr = EVAL_BOOL) - formation of the SQL-request to the DB.
    //  req - SQL-request text
    if(iid == "SQLReq" && prms.size() >= 1) {
	TArrayObj *rez = new TArrayObj(); rez->propSet("err", "");
	try {
	    vector< vector<string> > rtbl;
	    sqlReq(prms[0].getS(), &rtbl, ((prms.size()>=2)?prms[1].getB():EVAL_BOOL));
	    for(unsigned iR = 0; iR < rtbl.size(); iR++) {
		TArrayObj *row = new TArrayObj();
		for(unsigned iC = 0; iC < rtbl[iR].size(); iC++) {
		    row->arSet(iC, rtbl[iR][iC]);
		    if(iR) row->TVarObj::propSet(rtbl[0][iC], rtbl[iR][iC]);
		}
		rez->arSet(iR, row);
	    }
	} catch(TError &err)	{ rez->propSet("err", err.cat+":"+err.mess); }

	return rez;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user, RWRWR_, "root:" SDB_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid,prms,user);
}

AutoHD<TCntrNode> TBD::chldAt( int8_t igr, const string &name, const string &user ) const
{
    try { return TCntrNode::chldAt(igr, name, user); }
    catch(...) {
	if(igr == mTbl && !openStat(name)) {
	    const_cast<TBD*>(this)->open(name, false);
	    return TCntrNode::chldAt(igr, name, user);
	}
	else throw;
    }
}

void *TBD::Task( void *param )
{
    TBD &db = *(TBD *)param;

    while(!TSYS::taskEndRun()) {
	if(db.enableStat()) db.transCloseCheck();

	TSYS::taskSleep(1e9*db.trTm_ClsOnReq());
    }

    return NULL;
}

void TBD::cntrCmdProc( XMLNode *opt )
{
    string u = opt->attr("user"), l = opt->attr("lang");

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Data base: ")+trLU(name(),l,u),RWRWR_,"root",SDB_ID);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	ctrMkNode("grp",opt,-1,"/br/tbl_",_("Opened table"),RWRW__,"root",SDB_ID,1,"idSz","255");
	if(ctrMkNode("area",opt,0,"/prm",_("Data base"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Enabled"),RWRWR_,"root",SDB_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/prm/st/allow_tbls",_("Accessible tables"),RWRW__,"root",SDB_ID,4,
		    "tp","br","br_pref","tbl_","s_com","del","help",_("Tables in the database, but not open at this time."));
		if(enableStat())
		    ctrMkNode("comm",opt,-1,"/prm/st/load",_("Load the program from this DB"),RWRW__,"root","root");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SDB_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/TYPE");
		ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID);
		ctrMkNode2("fld",opt,-1,"/prm/cfg/CODEPAGE",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDB_ID,
		    "dest","sel_ed","sel_list",(Mess->charset()+";UTF-8;KOI8-R;KOI8-U;CP1251;CP866").c_str(),
		    "help",_("Codepage of data into the DB. For example it is: UTF-8, KOI8-R, KOI8-U ... ."),NULL);
		if(mTrTm_ClsOnReq < SERV_TASK_PER)
		    ctrMkNode("fld",opt,-1,"/prm/cfg/TRPR_CLS_TASK",EVAL_STR,RWRWR_,"root",SDB_ID,1,"help",TMess::labTaskPrior());
		else ctrRemoveNode(opt,"/prm/cfg/TRPR_CLS_TASK");
	    }
	}
	if(ctrMkNode("area",opt,1,"/tbls",_("Tables"),R_R___))
	    ctrMkNode("list",opt,-1,"/tbls/otbl",_("Opened tables"),RWRW__,"root",SDB_ID,5,
		"tp","br","idSz","255","s_com","add,del","br_pref","tbl_",
		"help",_("Opened tables list.\nAdding and removing tables actually consists of opening and closing tables."));
	if(enableStat() && ctrMkNode("area",opt,-1,"/sql","SQL",R_R___,"root",SDB_ID)) {
	    ctrMkNode("fld",opt,-1,"/sql/tm",_("Time"),R_R___,"root",SDB_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/sql/req",_("Request"),RWRW__,"root",SDB_ID,3,"tp","str","cols","100","rows","2");
	    ctrMkNode("fld",opt,-1,"/sql/trans",_("Transaction"),RWRW__,"root",SDB_ID,4,"tp","dec","dest","select",
		"sel_id","0;1;2","sel_list",_("Out;Into;No matter"));
	    ctrMkNode("comm",opt,-1,"/sql/send",_("Send"),RWRW__,"root",SDB_ID);
	    ctrMkNode("table",opt,-1,"/sql/tbl",_("Result"),R_R___,"root",SDB_ID);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR)) {
	    if(s2i(opt->text())) enable();
	    else { disable(); mDisByUser = true; }
	}
    }
    else if(a_path == "/prm/st/allow_tbls") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD)) {
	    vector<string> lst;
	    allowList(lst);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		opt->childAdd("el")->setText(lst[iL]);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDB_ID,SEC_WR)) {
	    open(opt->text(), false);
	    close(opt->text(), true);
	}
    }
    else if(a_path == "/prm/st/load" && ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR)) {
	bool isMdf = isModify(TCntrNode::Self);
	SYS->setSelDB(owner().modId()+"."+id());
	SYS->modifG();
	SYS->load();
	SYS->setSelDB("");
	if(isMdf) modif();
    }
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SDB_ID,RWRWR_);
    else if(a_path == "/br/tbl_" || a_path == "/tbls/otbl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		opt->childAdd("el")->setText(lst[iL]);
	}
	if(ctrChkNode(opt,"add",RWRW__,"root",SDB_ID,SEC_WR))	open(opt->text(), true);
	if(ctrChkNode(opt,"del",RWRW__,"root",SDB_ID,SEC_WR))	close(opt->text());
    }
    else if(a_path == "/sql/tm" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD))
	opt->setText(TBDS::genDBGet(owner().nodePath()+"ReqTm","0",opt->attr("user")));
    else if(a_path == "/sql/req") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID))	opt->setText(userSQLReq);
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID))	userSQLReq = opt->text();
    }
    else if(a_path == "/sql/trans") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID))	opt->setText(i2s(userSQLTrans));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID))	userSQLTrans = s2i(opt->text());
    }
    else if(a_path == "/sql/send" && enableStat() && ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR)) {
	int64_t stm = TSYS::curTime();
	sqlReq(userSQLReq, &userSQLResTbl, userSQLTrans);
	TBDS::genDBSet(owner().nodePath()+"ReqTm", tm2s(1e-6*(TSYS::curTime()-stm)), opt->attr("user"));
    }
    else if(a_path == "/sql/tbl" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD))
	for(unsigned i_r = 0; i_r < userSQLResTbl.size(); i_r++)
	    for(unsigned i_c = 0; i_c < userSQLResTbl[i_r].size(); i_c++) {
		if(i_r == 0)
		    ctrMkNode("list",opt,i_c,("/sql/tbl/"+userSQLResTbl[0][i_c]).c_str(),userSQLResTbl[0][i_c],R_R___,"root",SDB_ID,1,"tp","str");
		else opt->childGet(i_c)->childAdd("el")->setText(userSQLResTbl[i_r][i_c]);
	    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* TTable                                       *
//************************************************
TTable::TTable( const string &name ) : mName(name), notFullShow(false), tblOff(0), tblSz(100)
{
    modifClr();
    mLstUse = time(NULL);
}

TTable::~TTable( )
{

}

TCntrNode &TTable::operator=( const TCntrNode &node )
{
    const TTable *src_n = dynamic_cast<const TTable*>(&node);
    if(!src_n || !src_n->owner().enableStat() || !owner().enableStat()) return *this;

    //Table content copy
    TConfig req;
    const_cast<TTable*>(src_n)->fieldStruct(req);

    // Scan source table and write to destination table
    for(int row = 0; const_cast<TTable*>(src_n)->fieldSeek(row,req); row++) fieldSet(req);

    return *this;
}

string TTable::fullDBName( )	{ return owner().fullDBName()+"."+name(); }

TBD &TTable::owner( ) const	{ return *(TBD*)nodePrev(); }

TVariant TTable::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // XMLNodeObj fieldStruct() - field structure get.
    if(iid == "fieldStruct") {
        XMLNodeObj *rez = new XMLNodeObj("field");
	TConfig cfg;
	vector<string> elst;

	try {
	    fieldStruct(cfg);

	    cfg.cfgList(elst);
	    for(unsigned iEl = 0; iEl < elst.size(); iEl++) {
		TCfg &icfg = cfg.cfg(elst[iEl]);
		string stp = "str";
		switch(icfg.fld().type()) {
		    case TFld::Boolean:	stp = "bool";	break;
		    case TFld::Integer:	stp = "int";	break;
		    case TFld::Real:	stp = "real";	break;
		    case TFld::String:	stp = "str";	break;
		    default: break;
	        }
		XMLNodeObj *el = new XMLNodeObj(icfg.name());
		el->propSet("type", stp);
		el->propSet("len", TSYS::strMess("%d.%d",icfg.fld().len(),icfg.fld().dec()));
		el->propSet("def", icfg.fld().def());
		el->propSet("key", (icfg.fld().flg()&TCfg::Key)?"1":"0");
		rez->childAdd(el);
	    }
	} catch(TError &err) { }

	return rez;
    }
    // string fieldSeek(int row, XMLNodeObj fld) - Field seek for <row> and <fld>.
    // string fieldGet(XMLNodeObj fld) - Get field value for set keys
    // string fieldSet(XMLNodeObj fld) - Set field value for set keys
    // string fieldDel(XMLNodeObj fld) - Remove field for set keys
    //  row - seeked row number;
    //  fld - source and result field's record.
    if((iid == "fieldSeek" && prms.size() >= 2 && !AutoHD<XMLNodeObj>(prms[1].getO()).freeStat()) ||
	((iid == "fieldGet" || iid == "fieldSet" || iid == "fieldDel") &&
	    prms.size() >= 1 && !AutoHD<XMLNodeObj>(prms[0].getO()).freeStat()))
    {
	string rez = "1";
	bool isRet = false;

	try {
	    AutoHD<XMLNodeObj> fld = prms[(iid=="fieldSeek")?1:0].getO();
	    TConfig cfg;
	    for(unsigned i_r = 0; i_r < fld.at().childSize(); i_r++) {
		AutoHD<XMLNodeObj> xel = fld.at().childGet(i_r);
		TFld::Type eltp = TFld::String;
		if(xel.at().propGet("type").getS() == "int")	eltp = TFld::Integer;
		else if(xel.at().propGet("type").getS() == "real")	eltp = TFld::Real;
		else if(xel.at().propGet("type").getS() == "bool")	eltp = TFld::Boolean;
		cfg.elem().fldAdd(new TFld(xel.at().name().c_str(),"",eltp,(xel.at().propGet("key").getI()==1?(int)TCfg::Key:(int)TFld::NoFlag),
		    xel.at().propGet("len").getS().c_str(),xel.at().propGet("def").getS().c_str()));
		cfg.cfg(xel.at().name()).setS(xel.at().text());
	    }

	    if(iid == "fieldSeek")	{ rez = i2s(fieldSeek(prms[0].getI(),cfg)); isRet = true; }
	    else if(iid == "fieldGet")	{ fieldGet(cfg); isRet = true; }
	    else if(iid == "fieldSet")	fieldSet(cfg);
	    else if(iid == "fieldDel")	fieldDel(cfg);

	    if(isRet) {
		vector<string> el;
		cfg.cfgList(el);
		for(unsigned iEl = 0; iEl < el.size(); iEl++)
		    fld.at().childGet(iEl).at().setText(cfg.cfg(el[iEl]).getS());
	    }
	} catch(TError &err) { rez = "0:"+err.mess; }

	return rez;
    }
    return TCntrNode::objFuncCall(iid,prms,user);
}

void TTable::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Table: ")+name(),RWRW__,"root",SDB_ID);
	if(ctrMkNode("area",opt,0,"/prm",_("Table"))) {
	    ctrMkNode("fld",opt,-1,"/prm/nm",_("Name"),R_R___,"root",SDB_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/tblOff",_("Table offset, size"),RWRW__,"root",SDB_ID,2,"tp","dec","min","0");
	    ctrMkNode("fld",opt,-1,"/prm/tblSz","",RWRW__,"root",SDB_ID,3,"tp","dec","min","10","max","10000");
	    XMLNode *tbl;
	    if((tbl=ctrMkNode("table",opt,-1,"/prm/tbl",_("Data"),RWRW__,"root",SDB_ID,1,"s_com","add,del"))) {
		TConfig req;
		try { fieldStruct(req); } catch(...) { }
		for(unsigned i_f = 0; i_f < req.elem().fldSize(); i_f++) {
		    string     eid = req.elem().fldAt(i_f).name();
		    TFld::Type etp = req.elem().fldAt(i_f).type();
		    if(req.elem().fldAt(i_f).flg()&TCfg::Key) tbl->setAttr("key",tbl->attr("key")+eid+",");
		    ctrMkNode("list",opt,-1,("/prm/tbl/"+eid).c_str(),eid.c_str(),RWRW__,"root",SDB_ID,
			1,"tp",(etp==TFld::Boolean)?"bool":(etp==TFld::Integer)?"dec":(etp==TFld::Real)?"real":"str");
		}
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/nm" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD)) opt->setText(name());
    else if(a_path == "/prm/tblOff") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID))	opt->setText(i2s(tblOff));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID))	tblOff = s2i(opt->text());
    }
    else if(a_path == "/prm/tblSz") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID))	opt->setText(i2s(tblSz));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID))	tblSz = s2i(opt->text());
    }
    else if(a_path == "/prm/tbl") {
	TConfig req;
	string eid;
	fieldStruct(req);
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) {
	    time_t upTo = time(NULL)+STD_INTERF_TM;
	    bool firstRow = true;
	    vector< vector<string> > full;
	    for(unsigned iR = vmax(0,tblOff); (iR-tblOff) < tblSz && time(NULL) < upTo && fieldSeek(iR,req,&full); iR++, firstRow = false)
		for(unsigned iF = 0; iF < req.elem().fldSize(); iF++) {
		    eid = req.elem().fldAt(iF).name();
		    if(firstRow) ctrMkNode("list",opt,-1,("/prm/tbl/"+eid).c_str(),"",RWRWR_);
		    opt->childGet(iF)->childAdd("el")->setText(req.cfg(eid).getS());
		}
	    notFullShow = (time(NULL) >= upTo);
	}
	if(ctrChkNode(opt,"add",RWRW__,"root",SDB_ID,SEC_WR)) {
	    for(unsigned iF = 0; iF < req.elem().fldSize(); iF++) {
		eid = req.elem().fldAt(iF).name();
		if(!(req.elem().fldAt(iF).flg()&TCfg::Key)) continue;
		req.cfg(eid).setS("newReqKey");
	    }
	    req.cfgViewAll(false);
	    fieldSet(req);
	}
	if(ctrChkNode(opt,"del",RWRW__,"root",SDB_ID,SEC_WR)) {
	    for(unsigned iF = 0; iF < req.elem().fldSize(); iF++)
		if(req.elem().fldAt(iF).flg()&TCfg::Key) {
		    eid = req.elem().fldAt(iF).name();
		    req.cfg(eid).setS(opt->attr("key_"+eid), TCfg::ForceUse);
		}
	    fieldDel(req);
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR)) {
	    string col = opt->attr("col");
	    // Keys obtain
	    for(unsigned iF = 0; iF < req.elem().fldSize(); iF++)
		if(req.elem().fldAt(iF).flg()&TCfg::Key) {
		    eid = req.elem().fldAt(iF).name();
		    req.cfg(eid).setS(opt->attr("key_"+eid), TCfg::ForceUse|TCfg::ExtValTwo);
		}
	    // Same set
	    req.cfgViewAll(false);
	    req.cfg(col).setS(opt->text(), TCfg::ForceUse);
	    fieldSet(req);
	    opt->setAttr("noReload","1");	//Pass for reload
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

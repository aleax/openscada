
//OpenSCADA file: tbds.cpp
/***************************************************************************
 *   Copyright (C) 2003-2023 by Roman Savochenko, <roman@oscada.org>       *
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
#include <algorithm>

#include "tsys.h"
#include "tmess.h"
#include "tmodule.h"
#include "tbds.h"

using namespace OSCADA;

//************************************************
//* TBDS                                         *
//************************************************
TElem TBDS::elSYS;

TBDS::TBDS( ) : TSubSYS(SDB_ID, true), mTblLifeTime(600)
{
    //Open data bases DB structure
    elDB.fldAdd(new TFld("ID",trS("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elDB.fldAdd(new TFld("TYPE",trS("DB type (module)"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    elDB.fldAdd(new TFld("NAME",trS("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    elDB.fldAdd(new TFld("DESCR",trS("Description"),TFld::String,TFld::FullText|TFld::TransltText,"2000"));
    elDB.fldAdd(new TFld("ADDR",trS("Address"),TFld::String,TFld::NoFlag,"1000"));
    elDB.fldAdd(new TFld("CODEPAGE",trS("Code page"),TFld::String,TFld::NoFlag,"20"));
    elDB.fldAdd(new TFld("EN",trS("To enable"),TFld::Boolean,TFld::NoFlag,"1","1"));
    elDB.fldAdd(new TFld("LS_PR",trS("Priority in list"),TFld::Integer,0,"2","0","0;99"));
    elDB.fldAdd(new TFld("TRTM_CLS_ON_OPEN",trS("Transaction closing: after opening, seconds"),TFld::Real,TFld::NoFlag,"4.1",i2s(3*prmServTask_PER).c_str()));
    elDB.fldAdd(new TFld("TRTM_CLS_ON_REQ",trS("Transaction closing: after request, seconds"),TFld::Real,TFld::NoFlag,"4.1",i2s(prmServTask_PER).c_str()));
    elDB.fldAdd(new TFld("TRPR_CLS_TASK",trS("Transaction closing: separate task priority"),TFld::Integer,TFld::NoFlag,"3","0"));
}

TBDS::~TBDS( )	{ }

string TBDS::realDBName( const string &bdn, bool back )
{
    if(back) {
	bool isEqSz = false;
	if(bdn.find(SYS->workDB()) == 0 && ((isEqSz=(bdn.size()==SYS->workDB().size())) || bdn[SYS->workDB().size()] == '.'))
	    return string("*.*") + (isEqSz ? "" : bdn.substr(SYS->workDB().size()));
	else if(bdn.find(DB_CFG) == 0 && ((isEqSz=(bdn.size()==strlen(DB_CFG))) || bdn[strlen(DB_CFG)] == '.'))
	    return string("*.*") + (isEqSz ? "" : bdn.substr(strlen(DB_CFG)));
	return bdn;
    }

    int off = 0;
    string bd_t = TSYS::strParse(bdn, 0, ".", &off);
    string bd_n = TSYS::strParse(bdn, 0, ".", &off);
    string bd_tbl = TSYS::strParse(bdn, 0, ".", &off);

    if(bd_t == DB_CFG)	return bdn;
    if(SYS->workDB() == DB_CFG && bd_t == "*" && bd_n == "*")	return DB_CFG"."+bd_tbl;
    return ((bd_t=="*") ? TSYS::strParse(SYS->workDB(),0,".") : bd_t)+"."+
	   ((bd_n=="*") ? TSYS::strParse(SYS->workDB(),1,".") : bd_n)+(bd_tbl.empty() ? "" : "."+bd_tbl);
}

string TBDS::dbPart( const string &bdn, bool tbl )
{
    int off = 0;
    string p1 = TSYS::strParse(bdn, 0, ".", &off);
    string p2 = TSYS::strParse(bdn, 0, ".", &off);
    string p3 = TSYS::strParse(bdn, 0, ".", &off);

    return (p1 == DB_CFG) ? (tbl ? p2 : p1) :
			    (tbl ? p3 : p1+"."+p2);
}

void TBDS::dbList( vector<string> &ls, char flg )
{
    ls.clear();

    if((flg&LsCheckSel) && !SYS->selDB().empty()) {
	if(SYS->selDB() != DB_CFG) ls.push_back(SYS->selDB());
	return;
    }

    if(flg&LsInclGenFirst) ls.push_back("*.*");

    AutoHD<TBDS> dbs = SYS->db();

    vector<string> tdb_ls, db_ls;
    dbs.at().modList(tdb_ls);
    vector< pair<int,string> > sortIts;
    for(unsigned iTp = 0; iTp < tdb_ls.size(); iTp++) {
	AutoHD<TTypeBD> tbd = dbs.at().at(tdb_ls[iTp]);
	tbd.at().list(db_ls);
	for(unsigned iDB = 0; iDB < db_ls.size(); iDB++)
	    //if(!(flg&LsInclGenFirst) || (tdb_ls[iTp]+"."+db_ls[iDB]) != SYS->workDB())
	    if((tdb_ls[iTp]+"."+db_ls[iDB]) == SYS->workDB())	ls.push_back(tdb_ls[iTp]+"."+db_ls[iDB]);
	    else sortIts.push_back(pair<int,string>(tbd.at().at(db_ls[iDB]).at().lsPr()*10+tbd.at().lsPr(),
						    tdb_ls[iTp]+"."+db_ls[iDB]));
    }

    sort(sortIts.begin(), sortIts.end());
    reverse(sortIts.begin(), sortIts.end());
    for(unsigned iLs = 0; iLs < sortIts.size(); ++iLs)
	ls.push_back(sortIts[iLs].second);
}

void TBDS::perSYSCall( unsigned int cnt )
{
    vector<string> tdbs, dbs, tbls;
    modList(tdbs);
    for(unsigned iTDB = 0; iTDB < tdbs.size(); iTDB++) {
	at(tdbs[iTDB]).at().list(dbs);
	for(unsigned iDB = 0; iDB < dbs.size(); iDB++)
	    try {
		AutoHD<TBD> db = at(tdbs[iTDB]).at().at(dbs[iDB]);
		if(db.at().enableStat()) {
		    //Closing for not used tables
		    db.at().list(tbls);
		    for(unsigned iTbl = 0; iTbl < tbls.size(); iTbl++) {
			AutoHD<TTable> tbl = db.at().at(tbls[iTbl]);
			if((time(NULL)-tbl.at().lstUse()) > tblLifeTime()) {
			    tbl.free();
			    db.at().close(tbls[iTbl]);
			}
		    }
		    //Checking for the transaction closing
		    if(db.at().trTm_ClsOnReq() >= prmServTask_PER) db.at().transCloseCheck();
		}
		else if(db.at().toEnable() && !db.at().disabledByUser() && !SYS->stopSignal())
		    db.at().enable();
	    } catch(TError&) { }
    }

    TSubSYS::perSYSCall(cnt);
}

AutoHD<TTable> TBDS::tblOpen( const string &bdn, bool create )
{
    AutoHD<TTable> tbl;

    try {
	string bdT = TSYS::strSepParse(bdn,0,'.');
	string bdN = TSYS::strSepParse(bdn,1,'.');
	string bdTbl = TSYS::strSepParse(bdn,2,'.');
	if(bdT == "*") bdT = TSYS::strSepParse(SYS->workDB(),0,'.');
	if(bdN == "*") bdN = TSYS::strSepParse(SYS->workDB(),1,'.');
	if(bdT == DB_CFG) return tbl;
	AutoHD<TBD> obd = SYS->db().at().at(bdT).at().at(bdN);
	MtxAlloc res(obd.at().resTbls, true);	//!!!! To prevent of the multiple entry and creation try
	if(obd.at().enableStat()) {
	    if(!obd.at().openStat(bdTbl)) obd.at().open(bdTbl, create);
	    tbl = obd.at().at(bdTbl);
	}
    } catch(TError &err) {
	//mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
    }

    return tbl;
}

void TBDS::tblClose( const string &bdn, bool del )
{
    try {
	string bdT = TSYS::strSepParse(bdn,0,'.');
	string bdN = TSYS::strSepParse(bdn,1,'.');
	string bdTbl = TSYS::strSepParse(bdn,2,'.');
	if(bdT == "*") bdT = TSYS::strSepParse(SYS->workDB(),0,'.');
	if(bdN == "*") bdN = TSYS::strSepParse(SYS->workDB(),1,'.');
	if(bdT == DB_CFG) return;
	AutoHD<TBD> obd = SYS->db().at().at(bdT).at().at(bdN);
	MtxAlloc res(obd.at().resTbls, true);	//!!!! To prevent of the multiple entry and creation try
	if(obd.at().enableStat() && obd.at().openStat(bdTbl) && obd.at().at(bdTbl).at().nodeUse() == 1)
	    obd.at().close(bdTbl, del);
    } catch(TError &err) {
	mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	SYS->db().at().mess_sys(TMess::Warning, _("Error closing database '%s'!"), bdn.c_str());
    }
}

bool TBDS::dataSeek( const string &ibdn, const string &path, int lev, TConfig &cfg, char flags, XMLNode *localCfgCtx )
{
    int c_lev = 0;
    string bdn = realDBName(ibdn);
    bool isCfgCtx = (localCfgCtx || ((ibdn.size() || path.size()) && SYS->cfgCtx()));

    cfg.setTrcSet(true);

    if(isCfgCtx || (path.size() && (ibdn.empty() || ibdn.find("*.*") == 0 || TSYS::strParse(bdn,0,".") == DB_CFG))) {
	ResAlloc res(SYS->cfgRes());
	XMLNode *nd = NULL, *fnd = NULL, *el;
	string vl, vl_tr;
	bool isPrm = (path.find("prm:") == 0), isPresent = false;
	vector<string> cf_el;

	if(isCfgCtx) {
	    nd = localCfgCtx ? localCfgCtx : SYS->cfgCtx();
	    if(path.size()) {
		fnd = nd->childGet("id", path, true);
		for(int iTbl = 0; !fnd && (fnd=nd->childGet("tbl",iTbl,true)); iTbl++)
		    if(fnd->attr("dstId").size() && fnd->attr("dstId") != path)	fnd = NULL;
		    else fnd->setAttr("dstId", path);
		nd = fnd;
	    }
	}
	else {
	    res.lock(false);
	    nd = SYS->cfgNode(SYS->id()+"/"+(isPrm?path.substr(4):path));
	}

	for(unsigned iFld = 0, iEl; nd && iFld < (isPrm?1:nd->childSize()); iFld++) {
	    el = isPrm ? nd : nd->childGet(iFld);
	    if(el->name() == (isPrm?"prm":"fld")) {
		//Restore the configuration structure
		bool isComplLoad = (localCfgCtx && !cfg.elem().fldSize());
		if(isComplLoad) {
		    string fId, fldS; size_t pOff;
		    el->attrList(cf_el);
		    for(iEl = 0; iEl < cf_el.size(); iEl++) {
			if((pOff=(vl=cf_el[iEl]).rfind("_str")) == string::npos)	continue;
			fId = vl.substr(0, pOff);
			fldS = el->attr(vl);
			int offIt = 0;
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			TFld::Type tp = (TFld::Type)s2i(vl);
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			int flg = s2i(vl);
			string len = TSYS::strParse(fldS, 0, ":", &offIt);
			string def = TSYS::strDecode(TSYS::strParse(fldS,0,":",&offIt), TSYS::Custom);
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			bool isView = s2i(vl);
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			bool isKeyUse = s2i(vl);
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			bool isNoTransl = s2i(vl);
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			bool isReqKey = s2i(vl);
			if(!(vl=TSYS::strParse(fldS,0,":",&offIt)).size())	continue;
			bool isExtVal = s2i(vl);
			if(!cfg.cfgPresent(fId))
			    cfg.elem().fldAdd(new TFld(fId.c_str(),fId.c_str(),tp,flg,len.c_str(),def.c_str()));
			TCfg &cf = cfg.cfg(fId);
			cf.setView(isView); cf.setKeyUse(isKeyUse); cf.setNoTransl(isNoTransl); cf.setReqKey(isReqKey, false); cf.setExtVal(isExtVal);
		    }
		}

		cfg.cfgList(cf_el);

		//Check keywords
		for(iEl = 0; !isPrm && iEl < cf_el.size(); iEl++)
		    if(cfg.cfg(cf_el[iEl]).isKey() && !isComplLoad && cfg.cfg(cf_el[iEl]).keyUse() &&
		        cfg.cfg(cf_el[iEl]).getS() != el->attr(cf_el[iEl])) break;
		if((isPrm && lev == 0) || (!isPrm && iEl == cf_el.size() && lev <= c_lev++)) {
		    for(iEl = 0; iEl < cf_el.size(); iEl++) {
			if(isPrm && cf_el[iEl].find("val") != 0) continue;
			TCfg &cf = cfg.cfg(cf_el[iEl]);

			// From attribute
			if(!isPrm) vl = el->attr(cf_el[iEl], true, &isPresent);
			// From text
			if((isPrm && (cf_el[iEl] == "val" || (fnd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+TSYS::strParse(cf_el[iEl],1,"_"))))) ||
				(!isPrm && !isPresent && (fnd=el->childGet(cf_el[iEl],0,true))))
			    vl = (fnd?fnd:nd)->text(true), isPresent = true;

			// Checking for the translation
			if(cf.fld().flg()&TFld::TransltText && !cf.noTransl() && (!isPrm || cf_el[iEl] == "val")) {
			    vl_tr = "";
			    if(!Mess->translDyn()) {
				// From attribute
				if(!isPrm) vl_tr = el->attr(cf_el[iEl]+"_"+Mess->langCode());
				// From text
				if((isPrm && (fnd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+Mess->langCode()))) ||
					(!isPrm && vl_tr.empty() && (fnd=el->childGet(cf_el[iEl]+"_"+Mess->langCode(),0,true))))
				    vl_tr = fnd->text(true);
			    }
			    if(!cf.extVal()) {
				cf.setS(vl_tr.size() ? vl_tr : vl);
				Mess->translReg(vl, "cfg:"+path+"#"+cf_el[iEl]);
			    }
			    else {
				cf.setS(vl, TCfg::ExtValOne);
				cf.setS(vl_tr, TCfg::ExtValTwo);
				cf.setS("cfg:"+path+"#"+cf_el[iEl], TCfg::ExtValThree);
			    }
			}
			else if(isPresent) cf.setS(vl);
			else cf.toDefault(true);
			// Extended context
			if(localCfgCtx && cf.isKey() && cf.extVal())
			    cf.setS(el->attr(cf_el[iEl]+"_ext"), TCfg::ExtValTwo);
		    }
		    return true;
		}
	    }
	}
    }

    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = tblOpen(bdn);
	if(!tbl.freeStat()) {
	    bool rez = tbl.at().fieldSeek(lev-c_lev, cfg, ((flags&UseCache)?TSYS::addr2str(&cfg):""));
	    //tbl.free(); close(bdn);
	    return rez;
	}
    }

    return false;
}

bool TBDS::dataGet( const string &ibdn, const string &path, TConfig &cfg, char flags, XMLNode *localCfgCtx )
{
    bool db_true = false;
    string bdn = realDBName(ibdn);
    bool isCfgCtx = (localCfgCtx || ((ibdn.size() || path.size()) && SYS->cfgCtx()));
    TError dbErr;

    //Load from config
    if(isCfgCtx || (path.size() && (ibdn.empty() || ibdn.find("*.*") == 0 || TSYS::strParse(bdn,0,".") == DB_CFG))) {
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *nd = NULL, *fnd = NULL, *el;
	string vl, vl_tr;
	bool isPrm = (path.find("prm:") == 0), isPresent = false;
	vector<string> cf_el;

	if(isCfgCtx) {
	    nd = localCfgCtx ? localCfgCtx : SYS->cfgCtx();
	    if(path.size()) {
		fnd = nd->childGet("id", path, true);
		for(int iTbl = 0; !fnd && (fnd=nd->childGet("tbl",iTbl,true)); iTbl++)
		    if(fnd->attr("dstId").size() && fnd->attr("dstId") != path)	fnd = NULL;
		    else fnd->setAttr("dstId", path);
		nd = fnd;
	    }
	}
	else {
	    res.lock(false);
	    nd = SYS->cfgNode(SYS->id()+"/"+(isPrm?path.substr(4):path));
	}

	// Tables representing - scan fields and fill Configuration
	for(unsigned iFld = 0, iEl; nd && iFld < (isPrm?1:nd->childSize()); iFld++) {
	    el = isPrm ? nd : nd->childGet(iFld);
	    if(el->name() == (isPrm?"prm":"fld")) {
		cfg.cfgList(cf_el);

		//  Check keywords
		for(iEl = 0; !isPrm && iEl < cf_el.size(); iEl++)
		    if(cfg.cfg(cf_el[iEl]).isKey() && cfg.cfg(cf_el[iEl]).getS() != el->attr(cf_el[iEl])) break;
		if(isPrm || iEl == cf_el.size()) {
		    for(iEl = 0; iEl < cf_el.size(); iEl++) {
			if(isPrm && cf_el[iEl].find("val") != 0) continue;
			TCfg &cf = cfg.cfg(cf_el[iEl]);

			//  From attribute
			if(!isPrm) vl = el->attr(cf_el[iEl], true, &isPresent);
			//  From text
			if((isPrm && (cf_el[iEl] == "val" || (fnd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+TSYS::strParse(cf_el[iEl],1,"_"))))) ||
				(!isPrm && !isPresent && (fnd=el->childGet(cf_el[iEl],0,true))))
			    vl = (fnd?fnd:nd)->text(true), isPresent = true;

			//  Checking for the translation
			if(cf.fld().flg()&TFld::TransltText && !cf.noTransl() && (!isPrm || cf_el[iEl] == "val")) {
			    vl_tr = "";
			    if(!Mess->translDyn()) {
				//  From attribute
				if(!isPrm) vl_tr = el->attr(cf_el[iEl]+"_"+Mess->langCode());
				//  From text
				if((isPrm && (fnd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+Mess->langCode()))) ||
					(!isPrm && vl_tr.empty() && (fnd=el->childGet(cf_el[iEl]+"_"+Mess->langCode(),0,true))))
				    vl_tr = fnd->text(true);
			    }
			    if(!cf.extVal()) {
				cf.setS(vl_tr.size() ? vl_tr : vl);
				Mess->translReg(vl, "cfg:"+path+"#"+cf_el[iEl]);
			    }
			    else {
				cf.setS(vl, TCfg::ExtValOne);
				cf.setS(vl_tr, TCfg::ExtValTwo);
				cf.setS("cfg:"+path+"#"+cf_el[iEl], TCfg::ExtValThree);
			    }
			}
			else if(isPresent) cf.setS(vl);
			else cf.toDefault(true);
			//  Extended context
			if(localCfgCtx && cf.isKey() && cf.extVal())
			    cf.setS(el->attr(cf_el[iEl]+"_ext"), TCfg::ExtValTwo);
		    }
		    return true;
		}
	    }
	}
    }

    //Load from DB
    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = tblOpen(bdn);
	if(!tbl.freeStat()) {
	    db_true = true;
	    try { tbl.at().fieldGet(cfg); }
	    catch(TError &err) {
		db_true = false;
		dbErr = err;
	    }
	    if(db_true) return true;
	}
    }

    if(!db_true && !(flags&NoException)) {
	if(dbErr.cat.empty()) throw SYS->db().at().err_sys("%s", dbErr.mess.empty() ? _("The requested entry is missing.") : dbErr.mess.c_str());
	throw TError(dbErr.cat.c_str(), "%s", dbErr.mess.empty() ? _("The requested entry is missing.") : dbErr.mess.c_str());
    }

    return db_true;
}

bool TBDS::dataSet( const string &ibdn, const string &path, TConfig &cfg, char flags, XMLNode *localCfgCtx )
{
    string bdn = realDBName(ibdn);

    bool isCfgCtx = (localCfgCtx || ((ibdn.size() || path.size()) && SYS->cfgCtx()));
    bool toChangeExistsCfg = (!isCfgCtx && ibdn.find("*.*") == 0 && !(flags&OnlyCfg || TSYS::strParse(bdn,0,".") == DB_CFG));

    //Save to config
    if(isCfgCtx || (path.size() && (ibdn.empty() || ibdn.find("*.*") == 0 || toChangeExistsCfg || TSYS::strParse(bdn,0,".") == DB_CFG))) {
	ResAlloc res(SYS->cfgRes());
	XMLNode *nd = NULL, *wel = NULL, *fnd;
	vector<string> cf_el;
	bool isPrm = (path.find("prm:") == 0);
	string vnm;

	if(isCfgCtx) {
	    XMLNode *curCtx = localCfgCtx ? localCfgCtx : SYS->cfgCtx();
	    nd = curCtx;
	    if(path.size() && !(nd=curCtx->childGet("id",path,true)))
		nd = curCtx->childAdd("node")->setAttr("id", path);
	    if(SYS->cfgCtx())	SYS->setCfgCtx(nd, true);
	}
	else {
	    res.lock(false);
	    nd = SYS->cfgNode(SYS->id()+"/"+(isPrm?path.substr(4):path), !toChangeExistsCfg);
	    SYS->modifCfg(true);
	}

	// Tables representing
	if(nd) {
	    int fldCnt = 0;

	    cfg.cfgList(cf_el);

	    if(isPrm) wel = nd;
	    else {
		if(nd->name() != "tbl" && !(isCfgCtx && !path.size())) nd->setName("tbl");

		// Search the field presence
		for(unsigned iFld = 0, iEl; iFld < nd->childSize(); iFld++) {
		    XMLNode *el = nd->childGet(iFld);
		    if(el->name() != "fld") continue;
		    fldCnt++;
		    //  Check keywords
		    for(iEl = 0; iEl < cf_el.size(); iEl++)
			if(cfg.cfg(cf_el[iEl]).isKey() && cfg.cfg(cf_el[iEl]).getS(TCfg::ExtValTwo) != el->attr(cf_el[iEl])) break;
		    if(iEl == cf_el.size()) { wel = el; break; }
		}
		if(!wel && !toChangeExistsCfg) wel = nd->childAdd("fld");
	    }

	    bool isCreate = (isPrm && nd->text(true).empty()) || !wel;

	    if(wel) {
		for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
		    vnm = cf_el[iEl];
		    if(isPrm && vnm.find("val") != 0) continue;
		    TCfg &cf = cfg.cfg(vnm);
		    if(!cf.isKey() && !cf.view()) {
			if(localCfgCtx && !fldCnt) wel->attrDel(vnm+"_str");
			continue;
		    }

		    // Translation
		    string sval = cf.getS(), toLang = Mess->langCode(), svalBASE = sval, tVl;
		    bool isTransl = cf.fld().flg()&TFld::TransltText, isDynSet = false;
		    //  ... system prestored
		    bool isSysPreStor = (TSYS::strParse(sval,0,string(1,0)) != sval && TSYS::strParse(sval,2,string(1,0)).empty());
		    if(isSysPreStor) {
			svalBASE = TSYS::strParse(sval, 0, string(1,0));
			sval = Mess->I18N(sval);
			if(!Mess->translCfg()) svalBASE = sval;
		    }
		    //  ... dynamic data
		    if(!isSysPreStor && Mess->translDyn() && (tVl=TSYS::strParse(sval,1,string(1,0))).size()) {
			isDynSet = true;
			toLang = tVl;
			svalBASE = TSYS::strParse(sval, 0, string(1,0));
			sval = TSYS::strParse(sval, 2, string(1,0));
			isTransl = (isTransl && toLang != Mess->langCodeBase());
		    }
		    //  ... default
		    else isTransl = (isTransl && ((Mess->translCfg() && toLang != Mess->langCodeBase()) ||
					(isPrm && SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+toLang)) ||
					(!isPrm && (wel->attr(vnm+"_"+toLang).size() || wel->childGet(vnm+"_"+toLang,0,true)))));

		    // Getting the previous value for some translation cases
		    if(isTransl) {
			tVl = "";
			if(isPrm && (fnd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)))) tVl = fnd->text(true);
			else if(!isPrm && (tVl=wel->attr(vnm)).size()) ;
			else if(!isPrm && (fnd=wel->childGet(vnm,0,true))) tVl = fnd->text(true);
		    }

		    // Setting for default or not translated
		    if(isCreate || !isTransl || cf.noTransl() || (isTransl && tVl.empty())) {
			if(localCfgCtx && cf.isKey() && cf.extVal())
			    wel->setAttr(vnm+"_ext", cf.getS(TCfg::ExtValTwo));

			// Updating the translation cache of the dynamic mode
			if(Mess->translDyn() && cf.fld().flg()&TFld::TransltText && !cf.noTransl()) {
			    tVl = isPrm ? wel->text(true) :
					  ((tVl=wel->text(true)).size() ? tVl : wel->attr(vnm));
			    Mess->translIdxCacheUpd(tVl, "", svalBASE, "cfg:"+path+"#"+vnm);
			}

			// To attribute
			if(!isPrm && svalBASE.size() < CFG_A_LEN) {
			    wel->setAttr(vnm, svalBASE);
			    if((fnd=wel->childGet(vnm,0,true))) wel->childDel(fnd);
			}
			// To text
			else {
			    if(isPrm && (vnm == "val" || (nd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+TSYS::strParse(vnm,1,"_"),true))))
				fnd = nd;
			    else if(!isPrm && !(fnd=wel->childGet(vnm,0,true))) fnd = wel->childAdd(vnm);
			    fnd->setText(svalBASE, true);
			    if(!isPrm) wel->setAttr(vnm, "");
			}
		    }
		    // ... for translated
		    if(!isCfgCtx && isTransl && !cf.noTransl() && (!isPrm || vnm == "val") && (!Mess->translDyn() || isDynSet || isSysPreStor)) {
			vnm = cf_el[iEl]+"_"+toLang;
			if(!isPrm && sval.size() < CFG_A_LEN) {
			    wel->setAttr(vnm, sval);
			    if((fnd=wel->childGet(vnm,0,true))) wel->childDel(fnd);
			}
			else {
			    if(isPrm && (fnd=SYS->cfgNode(SYS->id()+"/"+path.substr(4)+"_"+toLang,true))) ;
			    else if(!isPrm && !(fnd=wel->childGet(vnm,0,true))) fnd = wel->childAdd(vnm);
			    fnd->setText(sval, true);
			    wel->setAttr(vnm, "");
			}
		    }
		    if(localCfgCtx && !fldCnt)
			wel->setAttr(vnm+"_str", i2s(cf.fld().type())+":"+i2s(cf.fld().flg())+":"+cf.fld().lenS()+":"+TSYS::strEncode(cf.fld().def(),TSYS::Custom,":\n")+":"+
						 i2s(cf.view())+":"+i2s(cf.keyUse())+":"+i2s(cf.noTransl())+":"+i2s(cf.reqKey())+":"+i2s(cf.extVal()));
		}
		SYS->modifCfg();
		return true;
	    }
	}
    }

    //Save to DB
    if(!isCfgCtx && bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = tblOpen(bdn, true);
	if(!tbl.freeStat()) {
	    bool db_true = true;
	    try { tbl.at().fieldSet(cfg); }
	    catch(TError &err) {
		//mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		db_true = false;
		if(!(flags&NoException)) throw;
	    }
	    return db_true;
	}
    }

    if(!(flags&NoException)) throw SYS->db().at().err_sys(_("Error writing to DB or configuration file."));

    return false;
}

bool TBDS::dataDel( const string &ibdn, const string &path, TConfig &cfg, char flags )
{
    vector<string> cels;
    string bdn = realDBName(ibdn);
    bool db_true = false;
    TError dbErr;

    //Delete from config
    if(path.size() && (ibdn.empty() || ibdn.find("*.*") == 0 || TSYS::strParse(bdn,0,".") == DB_CFG)) {
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *nd = SYS->cfgNode(SYS->id()+"/"+path, true);
	vector<string> cf_el;

	// Searching the present field(s)
	for(int iFld = 0, iEl; nd && iFld < (int)nd->childSize(); iFld++) {
	    XMLNode *el = nd->childGet(iFld);
	    if(el->name() != "fld")	continue;

	    // Checking for the keywords
	    cfg.cfgList(cf_el);
	    for(iEl = 0; iEl < (int)cf_el.size(); iEl++) {
		TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
	        if(u_cfg.isKey() && (u_cfg.keyUse() || flags&UseAllKeys) && u_cfg.getS() != el->attr(cf_el[iEl])) break;
	    }
	    if(iEl == cf_el.size()) {
		SYS->modifCfg(true);
		nd->childDel(iFld--);
		SYS->modifCfg();
		db_true = true;
		continue;
	    }
	}
	// Removing empty table nodes
	if(nd && !nd->childSize()) nd->parent()->childDel(nd);
    }

    //Delete from the database
    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG) {
	AutoHD<TTable> tbl = tblOpen(bdn);
	if(!tbl.freeStat()) {
	    try {
		//Select for using all keys
		if(flags&UseAllKeys) {
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
		if(flags&UseAllKeys)
		    for(unsigned iEl = 0; iEl < cels.size(); iEl++)
			cfg.cfg(cels[iEl]).setKeyUse(false);
	    } catch(TError &err) {
		dbErr = err;
		//mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	    }
	    //tbl.free(); close(bdn);
	}
    }

    if(!db_true && !(flags&NoException) && !dbErr.cat.empty()) throw TError(dbErr.cat.c_str(), "%s", dbErr.mess.c_str());

    return db_true;
}

bool TBDS::dataDelTbl( const string &ibdn, const string &path, char flags )
{
    string bdn = realDBName(ibdn);
    bool db_true = false;

    //Delete from config
    if(path.size() && (ibdn.empty() || ibdn.find("*.*") == 0 || TSYS::strParse(bdn,0,".") == DB_CFG)) {
	ResAlloc res(SYS->cfgRes(), false);
	XMLNode *nd = SYS->cfgNode(SYS->id()+"/"+path, true);
	if(nd) { nd->parent()->childDel(nd); db_true = true; }
    }

    //Delete from the database
    if(bdn.size() && TSYS::strParse(bdn,0,".") != DB_CFG && !tblOpen(bdn).freeStat()) {
	tblClose(bdn, true);
	db_true = true;
    }

    return db_true;
}

string TBDS::genPrmGet( const string &path, const string &oval, const string &user, char flags )
{
    //Generic system DB first init
    if(!elSYS.fldSize()) {
	elSYS.fldAdd(new TFld("user","User",TFld::String,TCfg::Key,i2s(limObjID_SZ).c_str()));
	elSYS.fldAdd(new TFld("id",trS("Value ID"),TFld::String,TCfg::Key,"100"));
	elSYS.fldAdd(new TFld("val","Value"  ,TFld::String,TFld::TransltText,"1000"));
    }

    string rez = oval;

    TConfig db_el(&elSYS);
    db_el.cfg("user").setS(user);
    db_el.cfg("id").setS(path);
    db_el.cfg("val").setNoTransl(!(flags&UseTranslation));
    try {
	if(dataGet(fullDBSYS(),"prm:"+path,db_el,flags))
	    rez = db_el.cfg("val").getS();
    } catch(TError &err) { }

    return rez;
}

void TBDS::genPrmSet( const string &path, const string &val, const string &user, char flags )
{
    if(!elSYS.fldSize()) return;	//Waiting for init in the reading

    TConfig db_el(&elSYS);
    db_el.cfg("user").setS(user);
    db_el.cfg("id").setS(path);
    db_el.cfg("val").setNoTransl(!(flags&UseTranslation));
    db_el.cfg("val").setS(val);
    try { dataSet(fullDBSYS(), "prm:"+path, db_el, flags); }
    catch(TError &err) { }
}

string TBDS::optDescr( )
{
    return TSYS::strMess(_(
	"========================= Subsystem \"DB\" options ========================\n"
	"------ Parameters of the section '%s' of the configuration file ------\n"
	"TblLifeTime  <seconds>  Opened tables lifetime (by default 600 seconds).\n\n"
	),nodePath().c_str()) + TSubSYS::optDescr();
}

void TBDS::load_( )
{
    //Load parameters from the command line

    //DB open
    // Open, load and enable the generic DB
    if(SYS->workDB().size() && SYS->workDB() != DB_CFG) {
	string db_tp = TSYS::strSepParse(SYS->workDB(), 0, '.');
	string db_nm = TSYS::strSepParse(SYS->workDB(), 1, '.');
	if(modPresent(db_tp) && !at(db_tp).at().openStat(db_nm)) {
	    at(db_tp).at().open(db_nm);
	    at(db_tp).at().at(db_nm).at().load();
	    at(db_tp).at().at(db_nm).at().enable();
	}
    }

    // Open other DB stored into the table 'DB' and the config-file
    try {
	string id, type;
	map<string, bool> itReg;
	TConfig c_el(&elDB);
	//c_el.cfgViewAll(false);
	for(int fldCnt = 0; dataSeek(fullDB(),nodePath()+"DB/",fldCnt++,c_el,UseCache); ) {
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("TYPE").getS();
	    if(!modPresent(type) || itReg[type+"."+id])	continue;
	    if((type+"."+id) != SYS->workDB() && !at(type).at().openStat(id))
		at(type).at().open(id);
	    try{ at(type).at().at(id).at().load(&c_el); } catch(TError&) { }
	    itReg[type+"."+id] = true;
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error finding and opening a new database."));
    }

    //Load parameters from the table "SYS" or the config-file
    setTblLifeTime(s2i(genPrmGet(nodePath()+"TblLifeTime",i2s(tblLifeTime()))));
}

void TBDS::save_( )
{
    //Save parameters to the table "SYS"
    genPrmSet(nodePath()+"TblLifeTime",i2s(tblLifeTime()));
}

void TBDS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),R_R_R_))
	    ctrMkNode("fld",opt,-1,"/sub/tblKeepAlive",_("Opened tables lifetime, seconds"),RWRWR_,"root",STR_ID,4,
		"tp","dec", "min","10", "max","1000", "help",_("Time of inactivity in the tables for it closing."));
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/sub/tblKeepAlive") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(tblLifeTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setTblLifeTime(s2i(opt->text()));
    }
    else TSubSYS::cntrCmdProc(opt);
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

void TTypeBD::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("Features");
}

string TTypeBD::modInfo( const string &name )
{
    if(name == "Features")	return features();

    return TModule::modInfo(name);
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
	ctrMkNode("grp",opt,-1,"/br/db_",_("DB"),RWRWR_,"root",SDB_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	if(ctrMkNode("area",opt,0,"/db",_("DB"),R_R_R_)) {
	    ctrMkNode("fld",opt,-1,"/db/ful_db_del",_("Complete DB removal"),RWRW__,"root",SDB_ID,2,
		"tp","bool","help",_("Select to completely remove the database when closing, otherwise the DB will simply be closed."));
	    ctrMkNode("list",opt,-1,"/db/odb",_("DB"),RWRWR_,"root",SDB_ID,5,
		"tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref","db_","idSz",i2s(limObjID_SZ).c_str());
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
TBD::TBD( const string &iid, TElem *cf_el ) : TConfig(cf_el), mEn(false), mId(cfg("ID")), mToEn(cfg("EN").getBd()),
    mTrTm_ClsOnOpen(cfg("TRTM_CLS_ON_OPEN").getRd()), mTrTm_ClsOnReq(cfg("TRTM_CLS_ON_REQ").getRd()), mTrPr_ClsTask(cfg("TRPR_CLS_TASK").getId()),
    userSQLTrans(EVAL_BOOL), mDisByUser(true), mIsFirst(true)
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
    if(flag&NodeRemove)
	TBDS::dataDel(TBDS::fullDB(), owner().owner().nodePath()+"DB/", *this, TBDS::UseAllKeys);
}

bool TBD::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR") mIsFirst = true;
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

    if(mIsFirst) Mess->translReg("", "uapi:"+fullDBName());

    if(trTm_ClsOnReq() < prmServTask_PER)
	try { SYS->taskCreate(nodePath('.',true), trPr_ClsTask(), Task, this, 0); }
	catch(TError&) { }	//Can be for retry to enable the DB

    mIsFirst = false;
}

void TBD::disable( )
{
    if(!enableStat()) return;

    //Close all tables
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
    else TBDS::dataGet(TBDS::fullDB(), owner().owner().nodePath()+"DB/", *this);

    if(!enableStat() && toEnable())
	try { enable(); } catch(TError&) { mDisByUser = false; }
}

void TBD::save_( )
{
    TBDS::dataSet(TBDS::fullDB(), owner().owner().nodePath()+"DB/", *this, TBDS::OnlyCfg);
}

TVariant TBD::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
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
    TVariant cfRez = objFunc(iid, prms, user_lang, RWRWR_, "root:" SDB_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user_lang);
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
    //Service commands processing
    string a_path = opt->attr("path");
    // SQL-request
    if(a_path == "/serv/SQL" && ctrChkNode(opt,"call",RWRWR_,"root",SDB_ID,SEC_WR)) {
	vector< vector<string> > tbl;
	sqlReq(opt->text(), s2i(opt->attr("withRez"))?&tbl:NULL, opt->attr("intoTrans").size()?s2i(opt->attr("intoTrans")):EVAL_BOOL);
	if(tbl.size()) opt->setText("");
	for(unsigned iR = 0; iR < tbl.size(); iR++)
	    for(unsigned iC = 0; iC < tbl[iR].size(); iC++) {
		while(iR == 0 && iC >= opt->childSize()) opt->childAdd("list");
		opt->childGet(iC)->childAdd("el")->setText(tbl[iR][iC]);
	    }
	return;
    }
    else if(a_path == "/serv/fieldStruct" && ctrChkNode(opt,"call",RWRWR_,"root",SDB_ID,SEC_RD)) {
	TConfig cfg;
	at(opt->attr("tbl")).at().fieldStruct(cfg);
	TBDS::dataSet("", "", cfg, TBDS::NoFlg, opt);
	return;
    }
    else if(a_path == "/serv/fieldSeek" && ctrChkNode(opt,"call",RWRWR_,"root",SDB_ID,SEC_RD)) {
	TConfig cfg;
	TBDS::dataSeek("", "", 0, cfg, TBDS::NoFlg, opt);
	opt->setAttr("fRez", i2s(at(opt->attr("tbl")).at().fieldSeek(s2i(opt->attr("row")),cfg,opt->attr("cacheKey"))));
	opt->childClear();
	TBDS::dataSet("", "", cfg, TBDS::NoFlg, opt);
	return;
    }
    else if(a_path == "/serv/fieldGet" && ctrChkNode(opt,"call",RWRWR_,"root",SDB_ID,SEC_RD)) {
	TConfig cfg;
	TBDS::dataSeek("", "", 0, cfg, TBDS::NoFlg, opt);
	at(opt->attr("tbl")).at().fieldGet(cfg);
	opt->childClear();
	TBDS::dataSet("", "", cfg, TBDS::NoFlg, opt);
	return;
    }
    else if(a_path == "/serv/fieldSet" && ctrChkNode(opt,"call",RWRWR_,"root",SDB_ID,SEC_WR)) {
	TConfig cfg;
	TBDS::dataSeek("", "", 0, cfg, TBDS::NoFlg, opt);
	at(opt->attr("tbl")).at().fieldSet(cfg);
	return;
    }
    else if(a_path == "/serv/fieldDel" && ctrChkNode(opt,"call",RWRWR_,"root",SDB_ID,SEC_WR)) {
	TConfig cfg;
	TBDS::dataSeek("", "", 0, cfg, TBDS::NoFlg, opt);
	at(opt->attr("tbl")).at().fieldDel(cfg);
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Data base: ")+trD(name()),RWRWR_,"root",SDB_ID);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	ctrMkNode("grp",opt,-1,"/br/tbl_",_("Opened table"),RWRW__,"root",SDB_ID,1,"idSz","255");
	if(ctrMkNode("area",opt,0,"/prm",_("Data base"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Enabled"),RWRWR_,"root",SDB_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/prm/st/allow_tbls",_("Accessible tables"),RWRW__,"root",SDB_ID,4,
		    "tp","br","br_pref","tbl_","s_com","del","help",_("Tables in the database, but not open at this time."));
		if(enableStat() && SYS->workDB() != fullDBName())
		    ctrMkNode("comm",opt,-1,"/prm/st/load",_("Load the program from this DB"),RWRW__,"root","root");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SDB_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/TYPE");
		ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID);
		ctrMkNode2("fld",opt,-1,"/prm/cfg/CODEPAGE",EVAL_STR,enableStat()?R_R_R_:RWRWR_,"root",SDB_ID,
		    "dest","sel_ed","sel_list",(Mess->charset()+";UTF-8;KOI8-R;KOI8-U;CP1251;CP866").c_str(),
		    "help",_("Codepage of data into the DB. For example it is: UTF-8, KOI8-R, KOI8-U ... ."),NULL);
		ctrMkNode2("fld",opt,-1,"/prm/cfg/LS_PR",EVAL_STR,RWRWR_,"root",SDB_ID,
		    "help",_("Priority in the range [0...99]. Useful for libraries placed in several databases for specifying the priority of the project - used ones."),NULL);
		if(mTrTm_ClsOnReq < prmServTask_PER)
		    ctrMkNode("fld",opt,-1,"/prm/cfg/TRPR_CLS_TASK",EVAL_STR,RWRWR_,"root",SDB_ID,1,"help",TMess::labTaskPrior().c_str());
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
	MtxAlloc res(SYS->cfgLoadSaveM(), true);
	bool isMdf = isModify(TCntrNode::Self);
	SYS->setSelDB(owner().modId()+"."+id());
	SYS->modifG();
	SYS->load();
	SYS->setSelDB("");
	if(isMdf) modif();
    }
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SDB_ID, RWRWR_);
    else if(a_path == "/br/tbl_" || a_path == "/tbls/otbl") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) {
	    vector<string> lst;
	    list(lst);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		opt->childAdd("el")->setText(lst[iL]);
	}
	if(ctrChkNode(opt,"add",RWRW__,"root",SDB_ID,SEC_WR))	open(opt->text(), true);
	if(ctrChkNode(opt,"del",RWRW__,"root",SDB_ID,SEC_WR))	close(opt->text(), s2i(opt->attr("del")));
    }
    else if(a_path == "/sql/tm" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD))
	opt->setText(TBDS::genPrmGet(owner().nodePath()+"ReqTm","0",opt->attr("user")));
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
	TBDS::genPrmSet(owner().nodePath()+"ReqTm", tm2s(1e-6*(TSYS::curTime()-stm)), opt->attr("user"));
    }
    else if(a_path == "/sql/tbl" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD))
	for(unsigned iR = 0; iR < userSQLResTbl.size(); iR++)
	    for(unsigned iC = 0; iC < userSQLResTbl[iR].size(); iC++) {
		if(iR == 0)
		    ctrMkNode("list",opt,iC,("/sql/tbl/"+userSQLResTbl[0][iC]).c_str(),userSQLResTbl[0][iC],R_R___,"root",SDB_ID,1,"tp","str");
		else opt->childGet(iC)->childAdd("el")->setText(userSQLResTbl[iR][iC]);
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
    req.setNoTransl(true);
    const_cast<TTable*>(src_n)->fieldStruct(req);

    // Scan source table and write to destination table
    for(int row = 0; const_cast<TTable*>(src_n)->fieldSeek(row,req); row++) fieldSet(req);

    return *this;
}

string TTable::fullDBName( )	{ return owner().fullDBName()+"."+name(); }

TBD &TTable::owner( ) const	{ return *(TBD*)nodePrev(); }

TVariant TTable::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
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
	    for(unsigned iR = 0; iR < fld.at().childSize(); iR++) {
		AutoHD<XMLNodeObj> xel = fld.at().childGet(iR);
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

    return TCntrNode::objFuncCall(iid, prms, user_lang);
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
		for(unsigned iF = 0; iF < req.elem().fldSize(); iF++) {
		    string     eid = req.elem().fldAt(iF).name();
		    TFld::Type etp = req.elem().fldAt(iF).type();
		    if(req.elem().fldAt(iF).flg()&TCfg::Key) tbl->setAttr("key",tbl->attr("key")+eid+",");
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
	req.setNoTransl(true);
	string eid;
	fieldStruct(req);
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) {
	    time_t upTo = time(NULL)+prmInterf_TM;
	    bool firstRow = true;
	    for(unsigned iR = vmax(0,tblOff); (iR-tblOff) < (unsigned)tblSz && time(NULL) < upTo && fieldSeek(iR,req,TSYS::addr2str(&req)); iR++, firstRow = false)
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

bool TTable::fieldSQLSeek( int row, TConfig &cfg, const string &cacheKey, int flags )
{
    if(tblStrct.empty()) return false;	// throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    //cfg.cfgToDefault();	//reset the not key and viewed fields
    cfg.setTrcSet(true);

    //Check for not present and not empty keys allow
    if(row == 0) {
	vector<string> cf_el;
	cfg.cfgList(cf_el);
	for(unsigned iC = 0; iC < cf_el.size(); iC++) {
	    TCfg &cf = cfg.cfg(cf_el[iC]);
	    if(!cf.isKey() || !cf.getS().size()) continue;
	    unsigned iFld = 0;
	    for( ; iFld < tblStrct.size(); iFld++)
		if(cf.name() == tblStrct[iFld].nm) break;
	    if(iFld >= tblStrct.size()) return false;
	}
    }

    string sid, req_where, ls;
    //Making WHERE
    // Adding used keys to the list
    bool trPresent = false;
    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld].nm;
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.find(Mess->langCode()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TFld::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey() && u_cfg->keyUse())
	    req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getSQLVal(*u_cfg);
	else if(u_cfg->isKey() || u_cfg->view())
	    ls += (ls.size()?", \"":"\"")+TSYS::strEncode(sid,TSYS::SQL,"\"")+"\"";
    }

    vector< vector<string> > inTbl, *tbl = &inTbl;
    MtxAlloc res(owner().connRes);
    if(cacheKey.size()) { res.lock(); tbl = &seekSess[cacheKey]; }

    //Request
    if(!cacheKey.size() || !tbl->size() || (row%SEEK_PRELOAD_LIM) == 0) {
	//if(ls.empty()) return false;	//!!!! But must to return TRUE at empty "ls" and presented keys
	string req = "SELECT";
	if(flags&SQLFirstSkipForSeek) {
	    if(!cacheKey.size()) req += " FIRST 1 SKIP " + i2s(row);
	    else req += " FIRST " + i2s(SEEK_PRELOAD_LIM) + " SKIP " + i2s((row/SEEK_PRELOAD_LIM)*SEEK_PRELOAD_LIM);
	}
	req += " " + (ls.size()?ls:"*") + " FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\"" +
		    (req_where.size()?" WHERE "+req_where:"") +
		    ((flags&SQLOrderForSeek && ls.size())?" ORDER BY "+ls:"");
	if(!(flags&SQLFirstSkipForSeek)) {
	    if(!cacheKey.size()) req += " LIMIT 1 OFFSET "+ i2s(row);
	    else req += " LIMIT " + i2s(SEEK_PRELOAD_LIM) + " OFFSET " + i2s((row/SEEK_PRELOAD_LIM)*SEEK_PRELOAD_LIM);
	}

	tbl->clear();
	owner().sqlReq(req, tbl/*, false*/);	//To seek for deletion into the save context do not set to "false"
    }

    row = cacheKey.size() ? (row%SEEK_PRELOAD_LIM)+1 : 1;
    if(tbl->size() < 2 || (cacheKey.size() && row >= (int)tbl->size())) {
	if(cacheKey.size()) seekSess.erase(cacheKey);
	return false;
    }

    //Processing of the query
    for(unsigned iFld = 0; iFld < (*tbl)[0].size(); iFld++) {
	sid = (*tbl)[0][iFld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setSQLVal(*u_cfg, (*tbl)[row][iFld]);
	else if(trPresent && sid.find(Mess->langCode()+"#") == 0 && (*tbl)[row][iFld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setSQLVal(*u_cfg, (*tbl)[row][iFld], true);
    }

    return true;
}

void TTable::fieldSQLGet( TConfig &cfg )
{
    vector< vector<string> > tbl;

    if(tblStrct.empty()) return; //throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    string sid, req_where, first_key, ls;
    //Prepare request
    // Adding the fields list to a queue
    bool trPresent = false;
    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	sid = tblStrct[iFld].nm;
	TCfg *u_cfg = cfg.at(sid, true);
	if(!u_cfg && !Mess->translDyn() && sid.find(Mess->langCode()+"#") == 0) {
	    u_cfg = cfg.at(sid.substr(3), true);
	    if(u_cfg && !(u_cfg->fld().flg()&TFld::TransltText)) continue;
	    trPresent = true;
	}
	if(!u_cfg) continue;

	if(u_cfg->isKey()) {
	    req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getSQLVal(*u_cfg);
	    if(first_key.empty()) first_key = TSYS::strEncode(sid,TSYS::SQL,"\"");
	}
	else if(u_cfg->view())
	    ls += (ls.size()?", \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"";
    }

    //Query
    string req = "SELECT " + (ls.size()?ls:"\""+first_key+"\"") + " FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" WHERE " + req_where + ";";
    owner().sqlReq(req, &tbl, false);
    if(tbl.size() < 2) throw err_sys(_("Row \"%s\" is not present. Are you saved the object?"), req_where.c_str());

    //Processing of query
    for(unsigned iFld = 0; iFld < tbl[0].size(); iFld++) {
	sid = tbl[0][iFld];
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg) setSQLVal(*u_cfg, tbl[1][iFld]);
	else if(trPresent && sid.find(Mess->langCode()+"#") == 0 && tbl[1][iFld].size() && (u_cfg=cfg.at(sid.substr(3),true)))
	    setSQLVal(*u_cfg, tbl[1][iFld], true);
    }
}

void TTable::fieldSQLSet( TConfig &cfg )
{
    //Generic flags:
    // Mess->translCfg() - is the translatable configuration
    // hasTr    - the table has translation in whole or is going to be translated
    // hasTrCol - has the translated column
    // isTransl - the field is translating

    vector< vector<string> > tbl;

    //if(tblStrct.empty()) throw err_sys(_("Table is empty."));
    mLstUse = SYS->sysTm();

    string sid, req_where;

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Checking for the translation presence
    bool hasTr = Mess->translCfg(), hasTrCol = false;
    for(unsigned iFld = 0; iFld < tblStrct.size() && !hasTrCol; iFld++)
	if((sid=tblStrct[iFld].nm).size() > 3 && sid[2] == '#') {
	    hasTr = true;
	    if(sid.find(Mess->langCode()+"#") == 0) hasTrCol = true;
	}

    //Get the available fields list
    // Add key list to queue
    bool noKeyFld = false,
	 isForceUpdt = cfg.reqKeys();			//Force update by ReqKeys or reqKey() present
    for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
	TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
	if(!u_cfg.isKey()) continue;
	req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + getSQLVal(u_cfg,TCfg::ExtValTwo);

	if(!isForceUpdt && u_cfg.extVal()) isForceUpdt = true;

	// Check for no key fields
	if(noKeyFld) continue;
	unsigned iFld = 0;
	for( ; iFld < tblStrct.size(); iFld++)
	    if(u_cfg.name() == tblStrct[iFld].nm) break;
	if(iFld >= tblStrct.size()) noKeyFld = true;
    }
    if(noKeyFld) {
	if(cfg.reqKeys()) return;
	fieldFix(cfg);
    }

    //Query for presence detect or the current data
    string req, ls, ls2, sval, tVl, langLs;
    if(hasTr)
	req = "SELECT * FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" WHERE " + req_where + ";";
    else if(!isForceUpdt)
	req = "SELECT 1 FROM \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" WHERE " + req_where + ";";
    if(req.size()) owner().sqlReq(req, &tbl, true);

    vector<string> trCacheUpd;
    req = "";
    if(!isForceUpdt) {
	if(tbl.size() < 2) {
	    //Adding new record
	    for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
		TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
		sval = getSQLVal(u_cfg);

		// Translation
		string svalRAW = u_cfg.getS(), toLang = Mess->langCode(), svalBASE = sval, svalBASE_RAW = svalRAW;
		bool isTransl = (u_cfg.fld().flg()&TFld::TransltText && !u_cfg.noTransl() && Mess->isMessTranslable(sval));
		//  ... system prestored
		bool isSysPreStor = (isTransl && TSYS::strParse(svalRAW,0,string(1,0)) != svalRAW && TSYS::strParse(svalRAW,2,string(1,0)).empty());
		if(isSysPreStor) {
		    tVl = svalRAW;
		    u_cfg.setS((svalRAW=Mess->I18N(tVl))); sval = getSQLVal(u_cfg);
		    u_cfg.setS((svalBASE_RAW=TSYS::strParse(tVl,0,string(1,0)))); svalBASE = getSQLVal(u_cfg);
		    u_cfg.setS(tVl);
		    if(!Mess->translCfg()) svalBASE = sval;
		}
		//  ... dynamic data
		if(!isSysPreStor && Mess->translDyn()) {
		    if((tVl=TSYS::strParse(svalRAW,1,string(1,0))).size()) {
			toLang = tVl; tVl = svalRAW;
			u_cfg.setS((svalRAW=TSYS::strParse(tVl,2,string(1,0)))); sval = getSQLVal(u_cfg);
			if((svalBASE_RAW=TSYS::strParse(tVl,0,string(1,0))).empty()) {
			    svalBASE_RAW = svalRAW;
			    trCacheUpd.push_back(u_cfg.name()+string(1,0)+""+string(1,0)+svalBASE_RAW);
			}
			u_cfg.setS(svalBASE_RAW); svalBASE = getSQLVal(u_cfg);
			u_cfg.setS(tVl);
		    }
		    else if(toLang != Mess->langCodeBase()) {
			tVl = svalRAW;
			u_cfg.setS((svalRAW=trD_L(svalBASE_RAW,toLang))); sval = getSQLVal(u_cfg);
			u_cfg.setS(tVl);
		    }
		}
		//  ... default
		else isTransl = (isTransl && hasTr);

		isTransl = (isTransl && ((!Mess->langCodeBase().size() && hasTrCol) ||
					 (Mess->langCodeBase().size() && toLang != Mess->langCodeBase())));
		if(isTransl && langLs.find(toLang+";") == string::npos) langLs += toLang+";";

		// Main values setting
		if(u_cfg.isKey() || u_cfg.view()) {
		    ls += (ls.size()?", \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"" +
			(isTransl ? (", \""+TSYS::strEncode(toLang+"#"+cf_el[iEl],TSYS::SQL,"\"")+"\"") : "");
		    ls2 += (ls2.size()?", ":"") + ((toLang==Mess->langCodeBase())?sval:svalBASE) + " " + (isTransl?(","+sval):"");
		}

		// Filling the translation fields in the DYNAMIC tanslation mode
		if(Mess->translDyn() && !u_cfg.isKey() && u_cfg.view() && u_cfg.fld().flg()&TFld::TransltText && !u_cfg.noTransl()) {
		    trCacheUpd.push_back(u_cfg.name()+string(1,0)+svalBASE_RAW+string(1,0)+svalRAW+(isTransl?string(1,0)+toLang:""));
		    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++)
			if(tblStrct[iFld].nm.size() > 3 && tblStrct[iFld].nm.compare(3,string::npos,cf_el[iEl]) == 0 &&
				tblStrct[iFld].nm.find(toLang) != 0 && trD_L(svalBASE_RAW,tblStrct[iFld].nm.substr(0,2)) != svalBASE_RAW) {
			    ls += (ls.size()?", \"":"\"") + TSYS::strEncode(tblStrct[iFld].nm,TSYS::SQL,"\"") + "\"";
			    ls2 += (ls2.size()?", '":"'") + TSYS::strEncode(trD_L(svalBASE_RAW,tblStrct[iFld].nm.substr(0,2)),TSYS::SQL,"'") + "'";
			}
		}
	    }
	    if(ls.size())
		req = "INSERT INTO \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" (" + ls + ") VALUES (" + ls2 + ");";
	} else isForceUpdt = true;
    }

    //Update present record
    bool toWarnReload = false;
    if(isForceUpdt) {
	for(unsigned iEl = 0; iEl < cf_el.size(); iEl++) {
	    TCfg &u_cfg = cfg.cfg(cf_el[iEl]);
	    if((u_cfg.isKey() && !u_cfg.extVal()) || !u_cfg.view()) continue;

	    sval = getSQLVal(u_cfg);

	    // No translation
	    if(!hasTr || u_cfg.fld().type() != TFld::String || !(u_cfg.fld().flg()&TFld::TransltText) || (cf_el[iEl].size() > 3 && cf_el[iEl][2] == '#'))
		ls += (ls.size()?", \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + sval;
	    else {
		string svalRAW = u_cfg.getS(), toLang = Mess->langCode();

		// Translation
		bool isTransl = true /*u_cfg.fld().flg()&TFld::TransltText*/, isDynSet = false;
		//  ... system prestored
		bool isSysPreStor = (isTransl && TSYS::strParse(svalRAW,0,string(1,0)) != svalRAW && TSYS::strParse(svalRAW,2,string(1,0)).empty());
		if(isSysPreStor) {
		    tVl = svalRAW;
		    u_cfg.setS((svalRAW=Mess->I18N(tVl))); sval = getSQLVal(u_cfg);
		    //u_cfg.setS((svalBASE_RAW=TSYS::strParse(tVl,0,string(1,0)))); svalBASE = getSQLVal(u_cfg);
		    u_cfg.setS(tVl);
		}
		//  ... dynamic data
		if(!isSysPreStor && Mess->translDyn() && (tVl=TSYS::strParse(svalRAW,1,string(1,0))).size()) {
		    isDynSet = true;
		    toLang = tVl; tVl = svalRAW;
		    u_cfg.setS((svalRAW=TSYS::strParse(svalRAW,2,string(1,0)))); sval = getSQLVal(u_cfg);
		    u_cfg.setS(tVl);
		}
		//  ... default
		else isTransl = (isTransl && hasTr);

		//  Clearing all the translation at setting no translatable message
		if(isTransl && (u_cfg.noTransl() || (svalRAW.size() && !Mess->isMessTranslable(svalRAW)))) {
		    ls += (ls.size()?", \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + sval;
		    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
			sid = tblStrct[iFld].nm;
			if(sid.size() <= 3 || sid.compare(3,string::npos,cf_el[iEl]) != 0 || sid.compare(0,3,Mess->langCodeBase()+"#") == 0)
			    continue;
			ls += (ls.size()?", \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=''";
		    }
		}
		//  Setting the translation and the marks
		else {
		    isTransl = (isTransl && ((!Mess->langCodeBase().size() && hasTrCol) ||
					     (Mess->langCodeBase().size() && toLang != Mess->langCodeBase())));
		    if(isTransl && langLs.find(toLang+";") == string::npos) langLs += toLang+";";

		    sid = (isTransl?(toLang+"#"):"") + cf_el[iEl];
		    //  Checking whether the field changed
		    bool isChanged = false, isPresent = false, isEmpty = false, isTrAsBase = false;
		    int fPos = -1;
		    for(unsigned iFld = 0; iFld < tbl[0].size() && !isPresent; iFld++)
			if(tbl[0][iFld] == sid) {
			    isChanged = (svalRAW != tbl[1][iFld]);
			    isEmpty = tbl[1][iFld].empty();
			    isPresent = true; fPos = iFld;
			}
			else if(isTransl && tbl[0][iFld] == cf_el[iEl])
			    isTrAsBase  = (tbl[1][iFld] == svalRAW);

		    if((isChanged || !isPresent) && (isEmpty || !isTrAsBase || isDynSet)) {
			unsigned mess_TrModifMarkLen = strlen(mess_TrModifMark);

			// The same field
			ls += (ls.size()?", \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + (isTrAsBase?"''":sval);

			// Setting for marks and the base message
			for(unsigned iFld = 0; !u_cfg.noTransl() && iFld < tbl[0].size(); iFld++) {
			    //  the setting message updating
			    if(Mess->translDyn() && u_cfg.fld().flg()&TFld::TransltText && tbl[0][iFld] == cf_el[iEl] && !isTrAsBase)
				trCacheUpd.push_back(u_cfg.name()+string(1,0)+tbl[1][iFld]+string(1,0)+svalRAW+(isTransl?string(1,0)+toLang:""));

			    //  clearing the translation at not empty base message
			    if(isTransl && tbl[0][iFld] == cf_el[iEl] && svalRAW.empty() && tbl[1][iFld].size())
				toWarnReload = true;
			    //  the translation message for mark the base one
			    else if(isTransl && tbl[0][iFld] == cf_el[iEl] && svalRAW.size()) {
				if(tbl[1][iFld].empty()) {	//double the empty base for the setting translation
				    ls += (ls.size()?", \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"=" + sval;

				    if(Mess->translDyn()) trCacheUpd.push_back(u_cfg.name()+string(1,0)+""+string(1,0)+svalRAW);
				}
				else if(!isTrAsBase && !Mess->translDyn()) {	//!!!! Do not mark the base modified for the dynamic translation
				    //if(Mess->translDyn())
				    //	trCacheUpd.push_back(u_cfg.name()+string(1,0)+tbl[1][iFld]+string(1,0)+svalRAW+string(1,0)+toLang);

				    if(fPos >= 0 && tbl[1][fPos].size() &&
					    (tbl[1][fPos].size() <= mess_TrModifMarkLen || tbl[1][fPos].rfind(mess_TrModifMark) != (tbl[1][fPos].size()-mess_TrModifMarkLen)) &&
					    (tbl[1][iFld].size() <= mess_TrModifMarkLen || tbl[1][iFld].rfind(mess_TrModifMark) != (tbl[1][iFld].size()-mess_TrModifMarkLen)))
				    {
					tVl = tbl[1][iFld] + mess_TrModifMark;
					ls += (ls.size()?", \"":"\"") + TSYS::strEncode(cf_el[iEl],TSYS::SQL,"\"") + "\"='" + TSYS::strEncode(tVl,TSYS::SQL,"'") + "'";

					if(Mess->translDyn()) trCacheUpd.push_back(u_cfg.name()+string(1,0)+tbl[1][iFld]+string(1,0)+tVl);
				    }
				}
			    }
			    //  the base message for mark the translation ones
			    else if(!isTransl && tbl[0][iFld].size() > 3 && tbl[0][iFld].compare(3,string::npos,sid) == 0 && tbl[0][iFld].compare(0,3,Mess->langCodeBase()+"#") != 0 &&
				    (svalRAW.empty() || fPos < 0 || tbl[1][fPos].size() <= mess_TrModifMarkLen || tbl[1][fPos].rfind(mess_TrModifMark) != (tbl[1][fPos].size()-mess_TrModifMarkLen)) &&
				    tbl[1][iFld].size() &&
				    (svalRAW.empty() || tbl[1][iFld].size() <= mess_TrModifMarkLen || tbl[1][iFld].rfind(mess_TrModifMark) != (tbl[1][iFld].size()-mess_TrModifMarkLen)))
			    {
				tVl = svalRAW.size() ? tbl[1][iFld] + mess_TrModifMark : "";
				ls += (ls.size()?", \"":"\"") + TSYS::strEncode(tbl[0][iFld],TSYS::SQL,"\"") + "\"='" + TSYS::strEncode(tVl,TSYS::SQL,"'") + "'";

				if(Mess->translDyn() && svalRAW.size())
				    trCacheUpd.push_back(u_cfg.name()+string(1,0)+svalRAW+string(1,0)+tVl+string(1,0)+tbl[0][iFld].substr(0,2));
			    }
			}
		    }
		}
	    }
	}
	if(ls.size())
	    req = "UPDATE \"" + TSYS::strEncode(name(),TSYS::SQL,"\"") + "\" SET " + ls + " WHERE " + req_where + ";";
    }

    if(req.empty())	return;

    //Query
    try { owner().sqlReq(req, NULL, true); }
    catch(TError &err) {
	if(err.cod == TError::DB_ReadOnly) {
	    err.mess = err.mess + " " + _("The DB is in the Read only mode.");
	    throw;
	}
	fieldFix(cfg, langLs);
	owner().sqlReq(req, NULL, true);
    }

    //Updating the translation cache of the dynamic mode
    for(unsigned iTr = 0; iTr < trCacheUpd.size(); ++iTr)
	Mess->translIdxCacheUpd(TSYS::strParse(trCacheUpd[iTr],1,string(1,0)),
			TSYS::strParse(trCacheUpd[iTr],3,string(1,0)),
			TSYS::strParse(trCacheUpd[iTr],2,string(1,0)),
			"db:"+fullDBName()+"#"+TSYS::strParse(trCacheUpd[iTr],0,string(1,0)));

    if(toWarnReload)
	throw err_sys(TError::DB_TrRemoved, _("Translation removed! Reload the base message."));
}

void TTable::fieldSQLDel( TConfig &cfg )
{
    if(tblStrct.empty()) return;
    mLstUse = SYS->sysTm();

    //Where prepare
    string req_where;
    for(unsigned iFld = 0; iFld < tblStrct.size(); iFld++) {
	string sid = tblStrct[iFld].nm;
	TCfg *u_cfg = cfg.at(sid, true);
	if(u_cfg && u_cfg->isKey() && u_cfg->keyUse())
	    req_where += (req_where.size()?" AND \"":"\"") + TSYS::strEncode(sid,TSYS::SQL,"\"") + "\"=" + getSQLVal(*u_cfg);
    }

    //Main request
    try { owner().sqlReq("DELETE FROM \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"\" WHERE "+req_where+";", NULL, true); }
    catch(TError &err) {
	//Checking for the presence
	vector< vector<string> > tbl;
	owner().sqlReq("SELECT 1 FROM \""+TSYS::strEncode(name(),TSYS::SQL,"\"")+"\" WHERE "+req_where+";", &tbl, true);
	if(tbl.size() < 2) return;
    }
}

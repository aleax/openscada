
//OpenSCADA system module Archive.DBArch file: val.cpp
/***************************************************************************
 *   Copyright (C) 2007-2017 by Roman Savochenko, <rom_as@oscada.org>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include <tsys.h>
#include <tmess.h>
#include "arch.h"
#include "val.h"

using namespace DBArch;

//*************************************************
//* DBArch::ModVArch - Value archivator           *
//*************************************************
ModVArch::ModVArch( const string &iid, const string &idb, TElem *cf_el ) :
    TVArchivator(iid,idb,cf_el), needMeta(true), needRePushGrps(false), reqRes(true), mMaxSize(0), mTmAsStr(false), mGroupPrms(0)
{
    setSelPrior(1);
    setAddr("*.*");
}

ModVArch::~ModVArch( )
{
    try { stop(); } catch(...) { }
}

void ModVArch::load_( )
{
    //TVArchivator::load_();

    if(addr().empty()) setAddr("*.*");

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	if(!(vl=prmNd.attr("Size")).empty())		setMaxSize(s2r(vl));
	if(!(vl=prmNd.attr("TmAsStr")).empty())		setTmAsStr(s2i(vl));
	if(!(vl=prmNd.attr("GroupPrms")).empty())	setGroupPrms(s2i(vl));
    } catch(...) { }
}

void ModVArch::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("Size", r2s(maxSize()));
    prmNd.setAttr("TmAsStr", i2s(tmAsStr()));
    prmNd.setAttr("GroupPrms", i2s(groupPrms()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TVArchivator::save_();
}

void ModVArch::start( )
{
    //Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    try { if(!db.at().enableStat()) db.at().enable(); }
    catch(TError &err) { mess_sys(TMess::Warning, _("Enable target DB error: %s"), err.mess.c_str()); }

    //Start getting data cycle
    TVArchivator::start();

    //First scan meta-DB. Load and connect archives
    checkArchivator();
}

void ModVArch::stop( bool full_del )
{
    //Stop getting data cicle an detach archives
    TVArchivator::stop(full_del);

    if(groupPrms()) accm.clear();
    needMeta = true;
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch )	{ return new ModVArchEl(arch, *this); }

string ModVArch::archTbl( int iG )	{ return "DBAVl_"+id()+((iG<0)?"":("_<GRP>"+(iG?i2s(iG):""))); }

void ModVArch::checkArchivator( unsigned int cnt )
{
    if(needRePushGrps) pushAccumVals();

    if(!needMeta || (cnt%60)) return;

    //Clear the accummulator's groups
    if(groupPrms()) accm.clear();

    //Load and process the main table with the meta-information.
    vector<vector<string> > full;
    TConfig cfg(&mod->archEl());
    for(int aCnt = 0; SYS->db().at().dataSeek(addr()+"."+mod->mainTbl(),"",aCnt++,cfg,false,&full); ) {
	string vTbl = cfg.cfg("TBL").getS(), aNm;
	if(vTbl.find(archTbl()+"_") == string::npos) continue;
	//Table per parameter mode
	if(vTbl.find(archTbl(0)) == string::npos) {
	    if(groupPrms()) continue;
	    aNm = vTbl.substr(archTbl().size()+1);

	    // Check to the archive present
	    AutoHD<TVArchive> varch;
	    if(owner().owner().valPresent(aNm)) varch = owner().owner().valAt(aNm);
	    else {
		// Add no present archive
		owner().owner().valAdd(aNm);
		varch = owner().owner().valAt(aNm);
		varch.at().setToStart(true);
		varch.at().setValType((TFld::Type)cfg.cfg("PRM2").getI());
		//varch.at().start();
	    }
	    //  Check for archive's start state and its starts early for propper redundancy sync
	    if(!varch.at().startStat() && varch.at().toStart())
		try { varch.at().start(); } catch(TError&) { continue; }	//!!!! Pass wrong archives
	    // Check for attached
	    if(!varch.at().archivatorPresent(workId()))	varch.at().archivatorAttach(workId());
	}
	//Single table per a parameters group mode
	else {
	    if(!groupPrms()) continue;
	    int gId = s2i(vTbl.substr(archTbl(0).size()));
	    SGrp *gO = NULL;

	    MtxAlloc res(reqRes, true);

	    // The parameters
	    for(int off = 0, aTp; (aNm=TSYS::strLine(cfg.cfg("PRM2").getS(),0,&off)).size(); ) {
		aTp = s2i(TSYS::strParse(aNm,0,":"));
		aNm = TSYS::strParse(aNm,1,":");
		bool gO_init = !gO;
		accmGetReg(aNm, &gO, (TFld::Type)aTp, gId);
		if(!gO) continue;
		if(gO_init) {
		    gO->beg = s2ll(cfg.cfg("BEGIN").getS());
		    gO->end = s2ll(cfg.cfg("END").getS());
		    gO->per = s2ll(cfg.cfg("PRM1").getS());
		    //  Check for delete the archives group table
		    if(maxSize() && gO->end <= (TSYS::curTime()-(int64_t)(maxSize()*86400e6))) {
			SYS->db().at().open(addr()+"."+vTbl);
			SYS->db().at().close(addr()+"."+vTbl, true);
			gO->beg = gO->end = gO->per = 0;
		    }
		}

		// Check to the archive present
		AutoHD<TVArchive> varch;
		if(owner().owner().valPresent(aNm)) varch = owner().owner().valAt(aNm);
		else {
		    // Add no present archive
		    owner().owner().valAdd(aNm);
		    varch = owner().owner().valAt(aNm);
		    varch.at().setToStart(true);
		    varch.at().setValType((TFld::Type)aTp);
		    varch.at().start();
		}
		// Check for archive's start state and it starts early for propper redundancy sync
		if(!varch.at().startStat() && varch.at().toStart()) varch.at().start();

		// Check for attached
		if(!varch.at().archivatorPresent(workId())) varch.at().archivatorAttach(workId());

		// Set the main parameters
		ResAlloc res1(archRes, false);
		map<string,TVArchEl*>::iterator iel = archEl.find(aNm);
		if(iel != archEl.end()) {
		    ModVArchEl *ael = (ModVArchEl*)iel->second;
		    ael->mBeg = gO->beg; ael->mEnd = gO->end; ael->mPer = gO->per;

		    //  Read previous one
		    int64_t curTm = (TSYS::curTime()/vmax(1,ael->period()))*ael->period();
		    if(curTm >= ael->begin() && curTm <= ael->end() && ael->period() > 10000000 && ael->prevVal == EVAL_REAL) {
			ael->prevTm = curTm;
			switch(varch.at().valType()) {
			    case TFld::Integer: case TFld::Real: ael->prevVal = ael->getVal(&curTm, false).getR();	break;
			    default: break;
			}
		    }
		}
	    }
	}
    }

    string wDB = TBDS::realDBName(addr());
    needMeta = (TSYS::strParse(wDB,0,".") != DB_CFG &&
		!SYS->db().at().at(TSYS::strParse(wDB,0,".")).at().at(TSYS::strParse(wDB,1,".")).at().enableStat());
}

TValBuf &ModVArch::accmGetReg( const string &aNm, SGrp **grp, TFld::Type tp, int prefGrpPos )
{
    //Find the parameter into a group
    for(unsigned iG = 0; iG < accm.size(); iG++) {
	map<string,TValBuf>::iterator iP = accm[iG].els.find(aNm);
	if(iP != accm[iG].els.end()) {
	    if(grp) *grp = &accm[iG];
	    return iP->second;
	}
    }

    //Find or create a propper group or pointed by <prefGrpPos>
    for(unsigned iG = 0; prefGrpPos < 0 && iG < accm.size(); iG++)
	if(accm[iG].els.size() < groupPrms()) prefGrpPos = iG;
    if(prefGrpPos < 0) prefGrpPos = accm.size();
    while(accm.size() <= prefGrpPos) {
	accm.push_back(SGrp(accm.size()));
	accm.back().tblEl.fldAdd(new TFld("MARK",_("Mark, time/(10*per)"),TFld::Integer,TCfg::Key,"20"));
	accm.back().tblEl.fldAdd(new TFld("TM",_("Time, seconds"),TFld::Integer,TCfg::Key|(tmAsStr()?TFld::DateTimeDec:0),"20"));
	//accm.back().tblEl.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"10"));
    }

    //Place the parameter to the selected group
    SGrp &gO = accm[prefGrpPos];
    switch(tp&TFld::GenMask) {
	case TFld::Boolean: gO.tblEl.fldAdd(new TFld(aNm.c_str(),aNm.c_str(),TFld::Integer,TFld::NoFlag,"1",i2s(EVAL_BOOL).c_str()));	break;
	case TFld::Integer: gO.tblEl.fldAdd(new TFld(aNm.c_str(),aNm.c_str(),TFld::Integer,TFld::NoFlag,"20",ll2s(EVAL_INT).c_str()));	break;
	case TFld::Real:    gO.tblEl.fldAdd(new TFld(aNm.c_str(),aNm.c_str(),TFld::Real,TFld::NoFlag,"",r2s(EVAL_REAL).c_str()));	break;
	case TFld::String:  gO.tblEl.fldAdd(new TFld(aNm.c_str(),aNm.c_str(),TFld::String,TFld::NoFlag,"1000",EVAL_STR));		break;
	default: break;
    }

    if(grp) *grp = &gO;
    return gO.els[aNm];
}

void ModVArch::accmUnreg( const string &aNm )
{
    MtxAlloc res(reqRes, true);

    //Find the parameter into a group
    for(unsigned iG = 0; iG < accm.size(); iG++) {
	SGrp &oG = accm[iG];
	map<string,TValBuf>::iterator iP = oG.els.find(aNm);
	if(iP == oG.els.end())	continue;
	oG.els.erase(iP);

	string	pLs;
	for(iP = oG.els.begin(); iP != oG.els.end(); ++iP)
	    pLs += i2s(iP->second.valType(true)) + ":" + iP->first + "\n";

	//Update the group meta info
	try{ grpMetaUpd(oG, &pLs); } catch(TError&) { }
	break;
    }
}

bool ModVArch::grpLimits( SGrp &oG, int64_t *ibeg, int64_t *iend )
{
    int64_t wEnd = iend ? vmax(oG.end, *iend) : oG.end;
    int64_t wBeg = ibeg ? (vmin(oG.beg,*ibeg) ? vmin(oG.beg,*ibeg) : vmax(oG.beg,*ibeg)) : oG.beg;

    if(ibeg && iend && wEnd <= oG.end && wBeg >= oG.beg) return false;

    try {
	AutoHD<TTable> tbl = SYS->db().at().open(addr()+"."+archTbl(oG.pos), true);

	//Remove limited records
	TConfig	cfg(&oG.tblEl);
	if(maxSize() && (wEnd-wBeg) > (int64_t)(maxSize()*86400e6)) {
	    cfg.cfg("TM").setKeyUse(false); //cfg.cfg("TMU").setKeyUse(false);
	    int64_t nEnd = ((wEnd-(int64_t)(maxSize()*86400e6))/oG.per)*oG.per;
	    for(int tC = vmax(wBeg,nEnd-3600ll*oG.per)/(10*oG.per); tC < nEnd/(10*oG.per); ++tC) {
		cfg.cfg("MARK").setI(tC, true);
		tbl.at().fieldDel(cfg);
	    }
	    wBeg = nEnd;
	}
	oG.beg = wBeg;
	if(ibeg) *ibeg = wBeg;

	oG.dbOK = true;
    } catch(TError&) { oG.dbOK = false; throw; }

    return true;
}

void ModVArch::grpMetaUpd( SGrp &oG, const string *aLs )
{
    //Update the group meta info
    TConfig cfg(&mod->archEl());
    cfg.cfgViewAll(false);
    cfg.cfg("TBL").setS(archTbl(oG.pos), true);
    cfg.cfg("BEGIN").setS(ll2s(oG.beg), true);
    cfg.cfg("END").setS(ll2s(oG.end), true);
    cfg.cfg("PRM1").setS(ll2s(oG.per), true);
    if(aLs) cfg.cfg("PRM2").setS(*aLs, true);
    try {
	SYS->db().at().dataSet(addr()+"."+mod->mainTbl(), "", cfg);
	oG.dbOK = true;
    } catch(TError&) { oG.dbOK = false; throw; }
}

void ModVArch::pushAccumVals( )
{
    MtxAlloc res(reqRes, true);

    needRePushGrps = false;
    for(unsigned iG = 0; iG < accm.size(); iG++) {
	SGrp &oG = accm[iG];
	if(!oG.accmBeg || !oG.accmEnd) continue;

	try {
	    AutoHD<TTable> tbl = SYS->db().at().open(addr()+"."+archTbl(iG), true);
	    if(tbl.freeStat()) continue;

	    TConfig	cfg(&oG.tblEl);

	    //Write data to the table
	    for(int64_t beg = oG.accmBeg, ctm, per = (oG.per?oG.per:(valPeriod()*1e6)); beg <= oG.accmEnd; beg += per) {
		for(map<string,TValBuf>::iterator iP = oG.els.begin(); iP != oG.els.end(); ++iP) {
		    TValBuf &oP = iP->second;
		    ctm = beg;
		    bool noData = (ctm < oP.begin() || ctm > oP.end());
		    switch(oP.valType()) {
			case TFld::Boolean:	cfg.cfg(iP->first).setI(noData?EVAL_BOOL:oP.getB(&ctm,true));	break;
			case TFld::Integer:	cfg.cfg(iP->first).setI(noData?EVAL_INT:oP.getI(&ctm,true));	break;
			case TFld::Real:	cfg.cfg(iP->first).setR(noData?EVAL_REAL:oP.getR(&ctm,true));	break;
			case TFld::String:	cfg.cfg(iP->first).setS(noData?EVAL_STR:oP.getS(&ctm,true));	break;
			default: break;
		    }
		}

		cfg.cfg("MARK").setI(beg/(10*per));
		cfg.cfg("TM").setI(beg/1000000);
		//cfg.cfg("TMU").setI(beg%1000000);
		tbl.at().fieldSet(cfg);
	    }

	    if(!oG.per) oG.per = (valPeriod()*1e6);
	    if(!oG.beg) oG.beg = oG.accmBeg;
	    oG.end = oG.accmEnd;

	    //Archive size limit process
	    grpLimits(oG);

	    //Actual parameters list prepare
	    string	pLs;
	    for(map<string,TValBuf>::iterator iP = oG.els.begin(); iP != oG.els.end(); ++iP)
		pLs += i2s(iP->second.valType(true)) + ":" + iP->first + "\n";

	    //Update the group meta info
	    grpMetaUpd(oG, &pLs);

	    //Clear the accumulators and some parameters update
	    for(map<string,TValBuf>::iterator iP = oG.els.begin(); iP != oG.els.end(); ++iP) {
		// Update the main archive parameters
		ResAlloc res1(archRes, false);
		map<string,TVArchEl*>::iterator iel = archEl.find(iP->first);
		if(iel != archEl.end()) {
		    ModVArchEl *ael = (ModVArchEl*)iel->second;
		    ael->mBeg = oG.beg;
		    ael->mEnd = /*oG.end;*/ vmin(oG.end, iP->second.end());
		    ael->mPer = oG.per;
		}

		iP->second.clear();
	    }

	    oG.accmBeg = oG.accmEnd = 0;

	    oG.dbOK = true;
	}
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    oG.dbOK = false; needRePushGrps = true;
	    continue;
	}
    }
}

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TVArchivator::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","select","select","/db/list","help",TMess::labDB());
	if(ctrMkNode("area",opt,-1,"/prm/add",_("Additional options"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/prm/add/sz",_("Archive size (days)"),RWRWR_,"root",SARH_ID,2,
		"tp","real", "help",_("Set to 0 for the limit disable and some performance rise"));
	    ctrMkNode("fld",opt,-1,"/prm/add/tmAsStr",_("Force time as string"),startStat()?R_R_R_:RWRWR_,"root",SARH_ID,2,
		"tp","bool", "help",_("Only for DBs it supports by a specific data type like to \"datetime\" into MySQL."));
	    ctrMkNode("fld",opt,-1,"/prm/add/groupPrms",_("Grouping parameters limit"),startStat()?R_R_R_:RWRWR_,"root",SARH_ID,4,
		"tp","dec", "min","0", "max","10000",
		"help",_("Enable grouping arhivator's parameters into a single table. Set to '0' for one table per parameter."));
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/add/sz") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(s2r(opt->text()));
    }
    else if(a_path == "/prm/add/tmAsStr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(tmAsStr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setTmAsStr(s2i(opt->text()));
    }
    else if(a_path == "/prm/add/groupPrms") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(groupPrms()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setGroupPrms(s2i(opt->text()));
    }
    else TVArchivator::cntrCmdProc(opt);
}

bool ModVArch::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "V_PER" && co.getR() && co.getR() != pc.getR()) co.setR(floor(vmax(1,co.getR())));
    return TVArchivator::cfgChange(co, pc);
}

//*************************************************
//* DBArch::ModVArchEl - Value archive element    *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive, iarchivator), mBeg(0), mEnd(0), mPer(0), needMeta(false)
{
    needMeta = !readMeta();

    if(!archivator().groupPrms()) {
	reqEl.fldAdd(new TFld("MARK",_("Mark, time/(10*per)"),TFld::Integer,TCfg::Key,"20"));
	reqEl.fldAdd(new TFld("TM",_("Time, seconds"),TFld::Integer,TCfg::Key|(archivator().tmAsStr()?TFld::DateTimeDec:0),"20"));
	//reqEl.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"10"));
	switch(archive().valType()) {
	    case TFld::Boolean: reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::Integer,TFld::NoFlag,"1",i2s(EVAL_BOOL).c_str()));	break;
	    case TFld::Integer: reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::Integer,TFld::NoFlag,"20",ll2s(EVAL_INT).c_str()));break;
	    case TFld::Real:    reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::Real,TFld::NoFlag,"",r2s(EVAL_REAL).c_str()));	break;
	    case TFld::String:  reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::String,TFld::NoFlag,"1000",EVAL_STR));		break;
	    default: break;
	}
    }
}

ModVArchEl::~ModVArchEl( )	{ }

string ModVArchEl::archTbl( )	{ return "DBAVl_"+archivator().id()+"_"+archive().id(); }

void ModVArchEl::fullErase( )
{
    if(archivator().groupPrms()) archivator().accmUnreg(archive().id());
    else {
	//Remove info record
	TConfig cfg(&mod->archEl());
	cfg.cfg("TBL").setS(archTbl(), true);
	SYS->db().at().dataDel(archivator().addr()+"."+mod->mainTbl(), "", cfg);

	//Remove archive's DB table
	SYS->db().at().open(archivator().addr()+"."+archTbl());
	SYS->db().at().close(archivator().addr()+"."+archTbl(), true);
    }
}

void ModVArchEl::getValsProc( TValBuf &ibuf, int64_t ibegIn, int64_t iendIn )
{
    if(needMeta && (needMeta=!readMeta()))	return;

    TValBuf buf(ibuf.valType(), ibuf.size(), ibuf.period(), ibuf.hardGrid(), ibuf.highResTm());

    //Request by single values for most big buffer's period
    if(buf.period()/10 > period()) {
	ibegIn = (ibegIn/buf.period())*buf.period();
	for(int64_t ctm; ibegIn <= iendIn; ibegIn += buf.period()) {
	    ctm = ibegIn;
	    TVariant vl = getValProc(&ctm, false);
	    buf.set(vl, ibegIn);
	}
	return;
    }

    //Going border to period time
    ibegIn = (ibegIn/period())*period();
    iendIn = (iendIn/period())*period();

    //Prepare border
    int64_t ibeg = vmax(ibegIn, begin());
    int64_t iend = vmin(iendIn, end());

    if(iend < ibeg)	return;

    //Get values
    // Fill by EVAL the data full range
    for(int64_t cTm = ibegIn; cTm <= iendIn; cTm += period()) buf.setR(EVAL_REAL, cTm);

    // Fill by EVAL previous range part without a real data
    //for(int64_t cTm = ibegIn; cTm < ibeg; cTm += period()) buf.setR(EVAL_REAL, cTm);

    // Same data values get
    TConfig cfg(&reqEl);
    string tblAddr = archivator().addr() + "." + archTbl(),
	   vlFld = "VAL";
    MtxAlloc res(archivator().reqRes, false);
    if(archivator().groupPrms()) {
	vlFld = archive().id();
	res.lock();
	ModVArch::SGrp *gO = NULL;
	archivator().accmGetReg(vlFld, &gO, archive().valType(true));
	//if(!gO->dbOK && pDt.realSize()) return 0;
	tblAddr = archivator().addr() + "." + archivator().archTbl(gO->pos);

	cfg.setElem(&gO->tblEl);
	cfg.cfgViewAll(false);
	cfg.cfg(vlFld).setView(true);
    }
    cfg.cfg("TM").setKeyUse(false); //cfg.cfg("TMU").setKeyUse(false);
    cfg.cfg("TM").setView(true);
    for(int64_t tC = ibeg, cTm; tC/(10*period()) <= iend/(10*period()); ) {
	tC = (tC/(10*period()))*(10*period());
	cfg.cfg("MARK").setI(tC/(10*period()), true);
	int eC = 0;
	vector< vector<string> > full;
	for( ; SYS->db().at().dataSeek(tblAddr,"",eC++,cfg,false,&full); ) {
	    cTm = 1000000ll * cfg.cfg("TM").getI();
	    if(cTm < ibeg || cTm > iend) continue;
	    switch(archive().valType()) {
		case TFld::Boolean:	buf.setB(cfg.cfg(vlFld).getB(), cTm);	break;
		case TFld::Integer:	buf.setI(cfg.cfg(vlFld).getI(), cTm);	break;
		case TFld::Real:	buf.setR(cfg.cfg(vlFld).getR(), cTm);	break;
		case TFld::String:	buf.setS(cfg.cfg(vlFld).getS(), cTm);	break;
		default:		buf.setR(EVAL_REAL, cTm);		break;
	    }
	}
	tC += 10*period();
    }

    res.unlock();

    // Fill by EVAL following range part without a real data
    //for(int64_t cTm = iend+period(); cTm <= iendIn; cTm += period()) buf.setR(EVAL_REAL, cTm);

    //Check for target DB enabled (disabled by the connection lost)
    string wDB = TBDS::realDBName(archivator().addr());
    if(TSYS::strParse(wDB,0,".") == DB_CFG ||
	    SYS->db().at().at(TSYS::strParse(wDB,0,".")).at().at(TSYS::strParse(wDB,1,".")).at().enableStat())
	buf.getVals(ibuf, buf.begin(), buf.end());
}

TVariant ModVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    if(needMeta && (needMeta=!readMeta()))	return EVAL_REAL;

    int64_t itm = tm ? *tm : SYS->curTime();
    itm = (itm/period())*period()+((up_ord && itm%period())?period():0);

    TConfig cf(&reqEl);
    string tblAddr = archivator().addr() + "." + archTbl(),
	   vlFld = "VAL";
    MtxAlloc res(archivator().reqRes, false);
    if(archivator().groupPrms()) {
	vlFld = archive().id();
	res.lock();
	ModVArch::SGrp *gO = NULL;
	archivator().accmGetReg(vlFld, &gO, archive().valType(true));
	//if(!gO->dbOK && pDt.realSize()) return 0;
	tblAddr = archivator().addr() + "." + archivator().archTbl(gO->pos);

	cf.setElem(&gO->tblEl);
	cf.cfgViewAll(false);
	cf.cfg(vlFld).setView(true);
    }
    cf.cfg("MARK").setKeyUse(false);
    cf.cfg("TM").setI(itm/1000000);
    //cf.cfg("TMU").setI(itm%1000000);
    if(SYS->db().at().dataGet(tblAddr,"",cf,false,true)) {
	if(tm) *tm = itm;
	switch(archive().valType()) {
	    case TFld::Boolean:	return cf.cfg(vlFld).getB();
	    case TFld::Integer:	return cf.cfg(vlFld).getI();
	    case TFld::Real:	return cf.cfg(vlFld).getR();
	    case TFld::String:	return cf.cfg(vlFld).getS();
	    default: break;
	}
    }
    res.unlock();

    if(tm) *tm = 0;
    return EVAL_REAL;
}

int64_t ModVArchEl::setValsProc( TValBuf &buf, int64_t ibeg, int64_t iend, bool toAccum )
{
    if(needMeta && (needMeta=!readMeta())) return 0;

    //Check border
    if(!buf.vOK(ibeg,iend)) return 0;
    ibeg = vmax(ibeg, buf.begin());
    iend = vmin(iend, buf.end());
    ibeg = (ibeg/period())*period();
    iend = (iend/period())*period();

    TConfig cfg(&reqEl);
    if(!archivator().groupPrms()) {
	//Table struct init
	AutoHD<TTable> tbl = SYS->db().at().open(archivator().addr()+"."+archTbl(), true);
	if(tbl.freeStat()) return 0;

	//Write data to the table
	for(int64_t ctm; ibeg <= iend; ibeg++) {
	    switch(archive().valType()) {
		case TFld::Boolean:	cfg.cfg("VAL").setI(buf.getB(&ibeg,true));	break;
		case TFld::Integer:	cfg.cfg("VAL").setI(buf.getI(&ibeg,true));	break;
		case TFld::Real:	cfg.cfg("VAL").setR(buf.getR(&ibeg,true));	break;
		case TFld::String:	cfg.cfg("VAL").setS(buf.getS(&ibeg,true));	break;
		default: break;
	    }
	    ctm = (ibeg/period())*period();
	    cfg.cfg("MARK").setI(ctm/(10*period()));
	    cfg.cfg("TM").setI(ctm/1000000);
	    //cfg.cfg("TMU").setI(ctm%1000000);
	    tbl.at().fieldSet(cfg);
	    //Archive time border update
	    mBeg = mBeg ? vmin(mBeg,ctm) : ctm;
	    mEnd = mEnd ? vmax(mEnd,ctm) : ctm;
	}

	//Archive size limit process
	if(archivator().maxSize() && (mEnd-mBeg) > (int64_t)(archivator().maxSize()*86400e6)) {
	    int64_t nEnd = ((mEnd-(int64_t)(archivator().maxSize()*86400e6))/period())*period();
	    cfg.cfg("TM").setKeyUse(false); //cfg.cfg("TMU").setKeyUse(false);
	    for(int tC = vmax(mBeg,nEnd-3600ll*period())/(10*period()); tC < nEnd/(10*period()); ++tC) {
		cfg.cfg("MARK").setI(tC, true);
		tbl.at().fieldDel(cfg);
	    }
	    mBeg = nEnd;
	}
	tbl.free();
	//SYS->db().at().close(archivator().addr()+"."+archTbl());		//!!! No close the table manually

	//Update archive info
	cfg.setElem(&mod->archEl());
	cfg.cfgViewAll(false);
	cfg.cfg("TBL").setS(archTbl(), true);
	cfg.cfg("BEGIN").setS(ll2s(mBeg), true);
	cfg.cfg("END").setS(ll2s(mEnd), true);
	cfg.cfg("PRM1").setS(ll2s(mPer), true);
	cfg.cfg("PRM2").setS(i2s(archive().valType(true)), true);

	return SYS->db().at().dataSet(archivator().addr()+"."+mod->mainTbl(),"",cfg,false,true) ? iend : 0;
    }

    //The group table processing
    MtxAlloc res(archivator().reqRes, true);
    string vlFld = archive().id();
    ModVArch::SGrp *gO = NULL;
    TValBuf &pDt = archivator().accmGetReg(vlFld, &gO, archive().valType(true));
    if(toAccum) {
	if(!gO->dbOK && pDt.realSize()) return 0;	//Prevent the accumulated data lost at DB errors
	pDt = buf;
	if(!gO->accmBeg || !gO->accmEnd) { gO->accmBeg = ibeg; gO->accmEnd = iend; }
	return vmin(gO->accmEnd, iend);
    }

    //Direct writing to the group table
    // Table struct init
    AutoHD<TTable> tbl = SYS->db().at().open(archivator().addr()+"."+archivator().archTbl(gO->pos), true);
    if(tbl.freeStat()) return 0;

    cfg.setElem(&gO->tblEl);
    cfg.cfgViewAll(false);
    cfg.cfg(vlFld).setView(true);

    //Write data to the table
    int64_t lstWrTm = 0;
    for(int64_t ctm; ibeg <= iend; ibeg++) {
	switch(archive().valType()) {
	    case TFld::Boolean:	cfg.cfg(vlFld).setI(buf.getB(&ibeg,true));	break;
	    case TFld::Integer:	cfg.cfg(vlFld).setI(buf.getI(&ibeg,true));	break;
	    case TFld::Real:	cfg.cfg(vlFld).setR(buf.getR(&ibeg,true));	break;
	    case TFld::String:	cfg.cfg(vlFld).setS(buf.getS(&ibeg,true));	break;
	    default: break;
	}
	ctm = (ibeg/period())*period();
	cfg.cfg("MARK").setI(ctm/(10*period()));
	cfg.cfg("TM").setI(ctm/1000000);
	//cfg.cfg("TMU").setI(ctm%1000000);
	tbl.at().fieldSet(cfg);
	lstWrTm = ctm;

	//Archive's range update
	mBeg = mBeg ? vmin(mBeg,ctm) : ctm;
	mEnd = mEnd ? vmax(mEnd,ctm) : ctm;
    }

    //Archive size limit process
    if(archivator().grpLimits(*gO,&mBeg,&mEnd)) archivator().grpMetaUpd(*gO);

    return lstWrTm;
}

bool ModVArchEl::readMeta( )
{
    if(archivator().groupPrms()) {
	if(!mPer) mPer = (int64_t)(archivator().valPeriod()*1e6);
	return !archivator().needMeta;
    }

    bool rez = true;

    //Load message archive parameters
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(archivator().addr()+"."+mod->mainTbl(),"",cfg,false,true)) {
	mBeg = s2ll(cfg.cfg("BEGIN").getS());
	mEnd = s2ll(cfg.cfg("END").getS());
	mPer = s2ll(cfg.cfg("PRM1").getS());
	// Check for delete archivator table
	if(archivator().maxSize() && mEnd <= (TSYS::curTime()-(int64_t)(archivator().maxSize()*86400e6))) {
	    SYS->db().at().open(archivator().addr()+"."+archTbl());
	    SYS->db().at().close(archivator().addr()+"."+archTbl(), true);
	    mBeg = mEnd = mPer = 0;
	}
    } else rez = false;

    if(!mPer) mPer = (int64_t)(archivator().valPeriod()*1e6);

    //Check for target DB enabled (disabled by the connection lost)
    if(!rez) {
	string wDB = TBDS::realDBName(archivator().addr());
	rez = (TSYS::strParse(wDB,0,".") == DB_CFG ||
		SYS->db().at().at(TSYS::strParse(wDB,0,".")).at().at(TSYS::strParse(wDB,1,".")).at().enableStat());
    }

    //Read previous
    if(rez) {
	// Load previous val check
	int64_t cur_tm = (TSYS::curTime()/vmax(1,period()))*period();
	if(cur_tm >= begin() && cur_tm <= end() && period() > 10000000 && prevVal == EVAL_REAL) {
	    prevTm = cur_tm;
	    switch(archive().valType()) {
		case TFld::Integer: case TFld::Real: prevVal = getVal(&cur_tm, false).getR();	break;
		default: break;
	    }
	}
    }

    return rez;
}

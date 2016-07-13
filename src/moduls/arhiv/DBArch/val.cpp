
//OpenSCADA system module Archive.DBArch file: val.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
    TVArchivator(iid,idb,cf_el), mMaxSize(0), mTmAsStr(false)
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
	if(!(vl=prmNd.attr("Size")).empty()) setMaxSize(s2r(vl));
	if(!(vl=prmNd.attr("TmAsStr")).empty()) setTmAsStr(s2i(vl));
    } catch(...) { }
}

void ModVArch::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("Size", r2s(maxSize()));
    prmNd.setAttr("TmAsStr", i2s(tmAsStr()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TVArchivator::save_();
}

void ModVArch::start( )
{
    //Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    try { if(!db.at().enableStat()) db.at().enable(); }
    catch(TError &err) { mess_warning(nodePath().c_str(), _("Enable target DB error: %s"), err.mess.c_str()); }

    //Start getting data cycle
    TVArchivator::start();
}

void ModVArch::stop( )
{
    //Stop getting data cicle an detach archives
    TVArchivator::stop();
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch )	{ return new ModVArchEl(arch, *this); }

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TVArchivator::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","select","select","/db/list","help",TMess::labDB());
	if(ctrMkNode("area",opt,-1,"/prm/add",_("Additional options"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/prm/add/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,2,
		"tp","real", "help",_("Set to 0 for the limit disable and some performance rise"));
	    ctrMkNode("fld",opt,-1,"/prm/add/tmAsStr",_("Force time as string"),startStat()?R_R_R_:RWRWR_,"root",SARH_ID,2,
		"tp","bool", "help",_("Only for DBs it supports by a specific data type like to \"datetime\" into MySQL."));
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
    else TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* DBArch::ModVArchEl - Value archive element    *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive, iarchivator), mBeg(0), mEnd(0), mPer(0), needMeta(false)
{
    needMeta = !readMeta();

    reqEl.fldAdd(new TFld("TM",_("Time (s)"),TFld::Integer,TCfg::Key|(archivator().tmAsStr()?TFld::DateTimeDec:0),"20"));
    reqEl.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"10"));
    switch(archive().valType()) {
	case TFld::Boolean: case TFld::Integer:
	    reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::Integer,TFld::NoFlag));
	    break;
	case TFld::Real:
	    reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::Real,TFld::NoFlag));
	    break;
	case TFld::String:
	    reqEl.fldAdd(new TFld("VAL",_("Value"),TFld::String,TFld::NoFlag,"1000"));
	    break;
	default: break;
    }
}

ModVArchEl::~ModVArchEl( )	{ }

string ModVArchEl::archTbl( )	{ return "DBAVl_"+archivator().id()+"_"+archive().id(); }

void ModVArchEl::fullErase( )
{
    //Remove info record
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl(), true);
    SYS->db().at().dataDel(archivator().addr()+"."+mod->mainTbl(), "", cfg);

    //Remove archive's DB table
    SYS->db().at().open(archivator().addr()+"."+archTbl());
    SYS->db().at().close(archivator().addr()+"."+archTbl(), true);
}

void ModVArchEl::getValsProc( TValBuf &ibuf, int64_t ibegIn, int64_t iendIn )
{
    if(needMeta && (needMeta=!readMeta()))	return;

    TValBuf buf(ibuf.valType(), ibuf.size(), ibuf.period(), ibuf.hardGrid(), ibuf.highResTm());

    //Request by single values for most big buffer period
    if(buf.period()/2 > period()) {
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

    TConfig cfg(&reqEl);

    //Get values
    for(int64_t c_tm = ibegIn; c_tm < ibeg; c_tm += period()) buf.setR(EVAL_REAL, c_tm);
    for(int64_t c_tm = ibeg; c_tm <= iend; c_tm += period()) {
	cfg.cfg("TM").setI(c_tm/1000000);
	cfg.cfg("TMU").setI(c_tm%1000000);
	if(SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg,false,true))
	    switch(archive().valType()) {
		case TFld::Boolean:
		case TFld::Integer:	buf.setI(cfg.cfg("VAL").getI(), c_tm);	break;
		case TFld::Real:	buf.setR(cfg.cfg("VAL").getR(), c_tm);	break;
		case TFld::String:	buf.setS(cfg.cfg("VAL").getS(), c_tm);	break;
		default:		buf.setR(EVAL_REAL, c_tm);		break;
	    }
	else buf.setR(EVAL_REAL, c_tm);
    }
    for(int64_t c_tm = iend+period(); c_tm <= iendIn; c_tm += period()) buf.setR(EVAL_REAL, c_tm);

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
    cf.cfg("TM").setI(itm/1000000);
    cf.cfg("TMU").setI(itm%1000000);
    if(SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cf,false,true)) {
	if(tm) *tm = itm;
	switch(archive().valType()) {
	    case TFld::Boolean:	return (char)cf.cfg("VAL").getI();
	    case TFld::Integer:	return cf.cfg("VAL").getI();
	    case TFld::Real:	return cf.cfg("VAL").getR();
	    case TFld::String:	return cf.cfg("VAL").getS();
	    default: break;
	}
    }
    if(tm) *tm = 0;
    return EVAL_REAL;
}

bool ModVArchEl::setValsProc( TValBuf &buf, int64_t beg, int64_t end )
{
    if(needMeta && (needMeta=!readMeta())) return false;

    //Check border
    if(!buf.vOK(beg,end)) return false;
    beg = vmax(beg, buf.begin());
    end = vmin(end, buf.end());
    beg = (beg/period())*period();
    end = (end/period())*period();

    //Table struct init
    TConfig cfg(&reqEl);
    AutoHD<TTable> tbl = SYS->db().at().open(archivator().addr()+"."+archTbl(), true);
    if(tbl.freeStat()) return false;

    //Write data to table
    for(int64_t ctm; beg <= end; beg++) {
	switch(archive().valType()) {
	    case TFld::Boolean:	cfg.cfg("VAL").setI(buf.getB(&beg,true));	break;
	    case TFld::Integer:	cfg.cfg("VAL").setI(buf.getI(&beg,true));	break;
	    case TFld::Real:	cfg.cfg("VAL").setR(buf.getR(&beg,true));	break;
	    case TFld::String:	cfg.cfg("VAL").setS(buf.getS(&beg,true));	break;
	    default: break;
	}
	ctm = (beg/period())*period();
	cfg.cfg("TM").setI(ctm/1000000);
	cfg.cfg("TMU").setI(ctm%1000000);
	tbl.at().fieldSet(cfg);
	//Archive time border update
	mBeg = mBeg ? vmin(mBeg,ctm) : ctm;
	mEnd = mEnd ? vmax(mEnd,ctm) : ctm;
    }

    //Archive size limit process
    if(archivator().maxSize() && (mEnd-mBeg) > (int64_t)(archivator().maxSize()*3600e6)) {
	int64_t n_end = ((mEnd-(int64_t)(archivator().maxSize()*3600e6))/period())*period();
	for(int64_t t_c = vmax(mBeg,n_end-3600ll*period()); t_c < n_end; t_c += period()) {
	    cfg.cfg("TM").setI(t_c/1000000, true);
	    cfg.cfg("TMU").setI(t_c%1000000, true);
	    tbl.at().fieldDel(cfg);
	}
	mBeg = n_end;
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

    return SYS->db().at().dataSet(archivator().addr()+"."+mod->mainTbl(),"",cfg,false,true);
}

bool ModVArchEl::readMeta( )
{
    bool rez = true;

    //Load message archive parameters
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(archivator().addr()+"."+mod->mainTbl(),"",cfg,false,true)) {
	mBeg = strtoll(cfg.cfg("BEGIN").getS().c_str(), NULL, 10);
	mEnd = strtoll(cfg.cfg("END").getS().c_str(), NULL, 10);
	mPer = strtoll(cfg.cfg("PRM1").getS().c_str(), NULL, 10);
	// Check for delete archivator table
	if(archivator().maxSize() && mEnd <= (TSYS::curTime()-(int64_t)(archivator().maxSize()*3600e6))) {
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
		case TFld::Int16: case TFld::Int32: case TFld::Int64: case TFld::Float: case TFld::Double:
		    prevVal = getVal(&cur_tm, false).getR();
		    break;
		default: break;
	    }
	}
    }

    return rez;
}


//OpenSCADA system module Archive.DBArch file: val.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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
#include <dirent.h>
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
    TVArchivator(iid,idb,cf_el), mMaxSize(24)
{
    setAddr("*.*");
}

ModVArch::~ModVArch( )
{
    try{ stop(); }catch(...){}
}

void ModVArch::setValPeriod( double iper )	{ TVArchivator::setValPeriod(iper); }

void ModVArch::load_( )
{
    TVArchivator::load_();

    if(addr().empty()) setAddr("*.*");

    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("Size");
	if(!vl.empty()) setMaxSize(atof(vl.c_str()));
    } catch(...){ }
}

void ModVArch::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("Size", r2s(maxSize()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TVArchivator::save_();
}

void ModVArch::start( )
{
    //Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    try { if(!db.at().enableStat()) db.at().enable(); }
    catch(TError err) { mess_warning(nodePath().c_str(), _("Enable target DB error: %s"), err.mess.c_str()); }

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
    if(opt->name() == "info")
    {
	TVArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","select","select","/db/list","help",TMess::labDB());
	ctrMkNode("fld",opt,-1,"/prm/cfg/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,1,"tp","real");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/sz")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(atof(opt->text().c_str()));
    }
    else TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* DBArch::ModVArchEl - Value archive element    *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator), mBeg(0), mEnd(0), mPer(0)
{
    //Load message archive parameters
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(archivator().addr()+"."+mod->mainTbl(),"",cfg))
    {
	mBeg = strtoll(cfg.cfg("BEGIN").getS().c_str(), NULL, 10);
	mEnd = strtoll(cfg.cfg("END").getS().c_str(), NULL, 10);
	mPer = strtoll(cfg.cfg("PRM1").getS().c_str(), NULL, 10);
	// Check for delete archivator table
	if(mEnd <= (TSYS::curTime()-(int64_t)(archivator().maxSize()*3600000000.)))
	{
	    SYS->db().at().open(archivator().addr()+"."+archTbl());
	    SYS->db().at().close(archivator().addr()+"."+archTbl(), true);
	    mBeg = mEnd = mPer = 0;
	}
    }
    if(!mPer) mPer = (int64_t)(archivator().valPeriod()*1000000.);
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

void ModVArchEl::getValsProc( TValBuf &buf, int64_t ibegIn, int64_t iendIn )
{
    //Request by single values for most big buffer period
    if(buf.period()/2 > period())
    {
	ibegIn = (ibegIn/buf.period())*buf.period();
	for(int64_t ctm; ibegIn <= iendIn; ibegIn += buf.period())
	{
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
    for(int64_t c_tm = ibegIn; c_tm < ibeg; c_tm += period()) buf.setR(EVAL_REAL, c_tm);
    switch(archive().valType())
    {
	case TFld::Boolean: case TFld::Integer:
	{
	    TConfig cfg(&mod->vlIntEl());
	    for(int64_t c_tm = ibeg; c_tm <= iend; c_tm += period())
	    {
		cfg.cfg("TM").setI(c_tm/1000000);
		cfg.cfg("TMU").setI(c_tm%1000000);
		if(SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg))
		{
		    if(archive().valType() == TFld::Integer)	buf.setI(cfg.cfg("VAL").getI(), c_tm);
		    else buf.setB(cfg.cfg("VAL").getI(), c_tm);
		}
		else buf.setI(EVAL_INT, c_tm);
	    }
	    break;
	}
	case TFld::Real:
	{
	    TConfig cfg(&mod->vlRealEl());
	    for(int64_t c_tm = ibeg; c_tm <= iend; c_tm += period())
	    {
		cfg.cfg("TM").setI(c_tm/1000000);
		cfg.cfg("TMU").setI(c_tm%1000000);
		if(SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg))
		    buf.setR(cfg.cfg("VAL").getR(), c_tm);
		else buf.setR(EVAL_REAL, c_tm);
	    }
	    break;
	}
	case TFld::String:
	{
	    TConfig cfg(&mod->vlStrEl());
	    for(int64_t c_tm = ibeg; c_tm <= iend; c_tm += period())
	    {
		cfg.cfg("TM").setI(c_tm/1000000);
		cfg.cfg("TMU").setI(c_tm%1000000);
		if(SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg))
		    buf.setS(cfg.cfg("VAL").getS(), c_tm);
		else buf.setS(EVAL_STR, c_tm);
	    }
	    break;
	}
	default: break;
    }
    for(int64_t c_tm = iend+period(); c_tm <= iendIn; c_tm += period()) buf.setR(EVAL_REAL, c_tm);
}

TVariant ModVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    int64_t itm = tm ? *tm : SYS->curTime();
    itm = (itm/period())*period()+((up_ord && itm%period())?period():0);

    TConfig cf(NULL);

    switch(archive().valType())
    {
	case TFld::Boolean: case TFld::Integer:
			    cf.setElem(&mod->vlIntEl());	break;
	case TFld::Real:    cf.setElem(&mod->vlRealEl());	break;
	case TFld::String:  cf.setElem(&mod->vlStrEl());	break;
	default: break;
    }
    cf.cfg("TM").setI(itm/1000000);
    cf.cfg("TMU").setI(itm%1000000);
    if(SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cf))
    {
	if(tm) *tm = itm;
	switch(archive().valType())
	{
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
    //Check border
    if(!buf.vOK(beg,end)) return false;
    beg = vmax(beg, buf.begin());
    end = vmin(end, buf.end());
    beg = (beg/period())*period();
    end = (end/period())*period();

    //Table struct init
    TConfig cfg((archive().valType()==TFld::Real) ? (&mod->vlRealEl()) :
		(archive().valType()==TFld::String) ? (&mod->vlStrEl()) : &mod->vlIntEl());

    AutoHD<TTable> tbl = SYS->db().at().open(archivator().addr()+"."+archTbl(), true);
    if(tbl.freeStat()) return false;

    //Write data to table
    for(int64_t ctm; beg <= end; beg++)
    {
	switch(archive().valType())
	{
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
    if((mEnd-mBeg) > (int64_t)(archivator().maxSize()*3600000000.))
    {
	int64_t n_end = ((mEnd-(int64_t)(archivator().maxSize()*3600000000.))/period())*period();
	for(int64_t t_c = vmax(mBeg,n_end-3600ll*period()); t_c < n_end; t_c += period())
	{
	    cfg.cfg("TM").setI(t_c/1000000, true);
	    cfg.cfg("TMU").setI(t_c%1000000, true);
	    tbl.at().fieldDel(cfg);
	}
	mBeg = n_end;
    }
    tbl.free();
    SYS->db().at().close(archivator().addr()+"."+archTbl());

    //Update archive info
    cfg.setElem(&mod->archEl());
    cfg.cfgViewAll(false);
    cfg.cfg("TBL").setS(archTbl(), true);
    cfg.cfg("BEGIN").setS(ll2s(mBeg), true);
    cfg.cfg("END").setS(ll2s(mEnd), true);
    cfg.cfg("PRM1").setS(ll2s(mPer), true);
    return SYS->db().at().dataSet(archivator().addr()+"."+mod->mainTbl(),"",cfg);
}

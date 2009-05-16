
//OpenSCADA system module Archive.DBArch file: val.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
    TVArchivator(iid,idb,cf_el), m_max_size(cfg("DBArchSize").getRd())
{
    setAddr("*.*");
}

ModVArch::~ModVArch( )
{
    try{ stop(); }catch(...){}
}

void ModVArch::setValPeriod( double iper )
{
    TVArchivator::setValPeriod(iper);
}

void ModVArch::load_( )
{
    TVArchivator::load_();

    if( addr().empty() ) setAddr("*.*");
}

void ModVArch::start()
{
    //> Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    if( !db.at().enableStat( ) ) db.at().enable();

    //> Start getting data cycle
    TVArchivator::start();
}

void ModVArch::stop()
{
    //> Stop getting data cicle an detach archives
    TVArchivator::stop();
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch )
{
    return new ModVArchEl(arch,*this);
}

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TVArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","Archive",4,"tp","str","dest","select","select","/db/list",
	    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/sz",cfg("DBArchSize").fld().descr(),0664,"root","Archive",1,"tp","real");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/sz" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) ) opt->setText(TSYS::real2str( maxSize() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) ) setMaxSize( atof(opt->text().c_str()) );
    }
    else TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* DBArch::ModVArchEl - Value archive element    *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator), m_beg(0), m_end(0), m_per(0)
{
    //- Load message archive parameters -
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(archivator().addr()+"."+mod->mainTbl(),"",cfg))
    {
	m_beg = strtoll(cfg.cfg("BEGIN").getS().c_str(),NULL,10);
	m_end = strtoll(cfg.cfg("END").getS().c_str(),NULL,10);
	m_per = strtoll(cfg.cfg("PRM1").getS().c_str(),NULL,10);
	//-- Check for delete archivator table --
	if( m_end <= (TSYS::curTime()-(long long)(archivator().maxSize()*3600000000.)) )
	{
	    SYS->db().at().open(archivator().addr()+"."+archTbl());
	    SYS->db().at().close(archivator().addr()+"."+archTbl(),true);
	    m_beg = m_end = m_per = 0;
	}
    }
    if( !m_per ) m_per = (long long)(archivator().valPeriod()*1000000.);
}

ModVArchEl::~ModVArchEl( )
{

}

string ModVArchEl::archTbl( )
{
    return "DBAVl_"+archivator().id()+"_"+archive().id();
}

void ModVArchEl::fullErase()
{
    //> Remove info record
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl(),true);
    SYS->db().at().dataDel(archivator().addr()+"."+mod->mainTbl(),"",cfg);

    //> Remove archive's DB table
    SYS->db().at().open( archivator().addr()+"."+archTbl() );
    SYS->db().at().close( archivator().addr()+"."+archTbl(), true );
}

void ModVArchEl::getValsProc( TValBuf &buf, long long ibegIn, long long iendIn )
{
    //> Going border to period time
    ibegIn = (ibegIn/period())*period();
    iendIn = (iendIn/period())*period();

    //> Prepare border
    long ibeg = vmax( ibegIn, begin() );
    long iend = vmin( iendIn, end() );

    if( iend < ibeg )	return;

    //> Get values
    for( long long c_tm = ibegIn; c_tm < ibeg; c_tm += period() ) buf.setI(EVAL_INT);
    switch(archive().valType())
    {
	case TFld::Boolean: case TFld::Integer:
	{
	    TConfig cfg(&mod->vlIntEl());
	    for( long long c_tm = ibeg; c_tm <= iend; c_tm+=period() )
	    {
		cfg.cfg("TM").setI(c_tm/1000000);
		cfg.cfg("TMU").setI(c_tm%1000000);
		if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
		{
		    if( archive().valType() == TFld::Integer )	buf.setI(cfg.cfg("VAL").getI(),c_tm);
		    else buf.setB(cfg.cfg("VAL").getI(),c_tm);
		}
		else buf.setI(EVAL_INT,c_tm);
	    }
	    break;
	}
	case TFld::Real:
	{
	    TConfig cfg(&mod->vlRealEl());
	    for( long long c_tm = ibeg; c_tm <= iend; c_tm+=period() )
	    {
		cfg.cfg("TM").setI(c_tm/1000000);
		cfg.cfg("TMU").setI(c_tm%1000000);
		if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
		    buf.setR(cfg.cfg("VAL").getR(),c_tm);
		else buf.setR(EVAL_REAL,c_tm);
	    }
	    break;
	}
	case TFld::String:
	{
	    TConfig cfg(&mod->vlStrEl());
	    for( long long c_tm = ibeg; c_tm <= iend; c_tm+=period() )
	    {
		cfg.cfg("TM").setI(c_tm/1000000);
		cfg.cfg("TMU").setI(c_tm%1000000);
		if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
		    buf.setS(cfg.cfg("VAL").getS(),c_tm);
		else buf.setS(EVAL_STR,c_tm);
	    }
	    break;
	}
    }
    for( long long c_tm = iend+period(); c_tm <= iendIn; c_tm += period() ) buf.setI(EVAL_INT);
}

string ModVArchEl::getS( long long *tm, bool up_ord )
{
    switch(archive().valType())
    {
	case TFld::Boolean:	return TSYS::int2str(getB(tm,up_ord));
	case TFld::Integer:	return TSYS::int2str(getI(tm,up_ord));
	case TFld::Real:	return TSYS::real2str(getR(tm,up_ord));
	case TFld::String:
	{
	    long long itm = tm?*tm:SYS->curTime();
	    itm = (itm/period())*period()+((up_ord && itm%period())?period():0);

	    TConfig cfg(&mod->vlStrEl());
	    cfg.cfg("TM").setI(itm/1000000);
	    cfg.cfg("TMU").setI(itm%1000000);
	    if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
	    {
		if(tm) *tm = itm;
		return cfg.cfg("VAL").getS();
	    }
	    if(tm) *tm = 0;
	    return EVAL_STR;
	}
    }
}

double ModVArchEl::getR( long long *tm, bool up_ord )
{
    switch(archive().valType())
    {
	case TFld::Boolean:	return getB(tm,up_ord);
	case TFld::Integer:	return getI(tm,up_ord);
	case TFld::String:	return atof(getS(tm,up_ord).c_str());
	case TFld::Real:
	{
	    long long itm = tm?*tm:SYS->curTime();
	    itm = (itm/period())*period()+((up_ord && itm%period())?period():0);

	    TConfig cfg(&mod->vlRealEl());
	    cfg.cfg("TM").setI(itm/1000000);
	    cfg.cfg("TMU").setI(itm%1000000);
	    if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
	    {
		if(tm) *tm = itm;
		return cfg.cfg("VAL").getR();
	    }
	    if(tm) *tm = 0;
	    return EVAL_REAL;
	}
    }
}

int ModVArchEl::getI( long long *tm, bool up_ord )
{
    switch(archive().valType())
    {
	case TFld::Boolean:	return getB(tm,up_ord);
	case TFld::Real:	return (int)getR(tm,up_ord);
	case TFld::String:	return atoi(getS(tm,up_ord).c_str());
	case TFld::Integer:
	{
	    long long itm = tm?*tm:SYS->curTime();
	    itm = (itm/period())*period()+((up_ord && itm%period())?period():0);

	    TConfig cfg(&mod->vlIntEl());
	    cfg.cfg("TM").setI(itm/1000000);
	    cfg.cfg("TMU").setI(itm%1000000);
	    if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
	    {
		if(tm) *tm = itm;
		return cfg.cfg("VAL").getI();
	    }
	    if(tm) *tm = 0;
	    return EVAL_INT;
	}
    }
}

char ModVArchEl::getB( long long *tm, bool up_ord )
{
    switch(archive().valType())
    {
	case TFld::Real:	return (int)getR(tm,up_ord);
	case TFld::String:	return atoi(getS(tm,up_ord).c_str());
	case TFld::Integer:	return getI(tm,up_ord);
	case TFld::Boolean:
	{
	    long long itm = tm?*tm:SYS->curTime();
	    itm = (itm/period())*period()+((up_ord && itm%period())?period():0);

	    TConfig cfg(&mod->vlIntEl());
	    cfg.cfg("TM").setI(itm/1000000);
	    cfg.cfg("TMU").setI(itm%1000000);
	    if( SYS->db().at().dataGet(archivator().addr()+"."+archTbl(),"",cfg) )
	    {
		if(tm) *tm = itm;
		return cfg.cfg("VAL").getI();
	    }
	    if(tm) *tm = 0;
	    return EVAL_BOOL;
	}
    }
}

void ModVArchEl::setValsProc( TValBuf &buf, long long beg, long long end )
{
    //- Check border -
    if( !buf.vOK(beg,end) )	return;
    beg = vmax(beg,buf.begin());
    end = vmin(end,buf.end());

    //- Table struct init -
    TConfig cfg( (archive().valType()==TFld::Real) ? (&mod->vlRealEl()) :
		 (archive().valType()==TFld::String) ? (&mod->vlStrEl()) : &mod->vlIntEl() );

    AutoHD<TTable> tbl = SYS->db().at().open(archivator().addr()+"."+archTbl(),true);
    if( tbl.freeStat() ) return;
    //- Write data to table -
    for( long long ctm; beg <= end; beg++ )
    {
	switch( archive().valType() )
	{
	    case TFld::Boolean:	cfg.cfg("VAL").setI(buf.getB(&beg,true));	break;
	    case TFld::Integer:	cfg.cfg("VAL").setI(buf.getI(&beg,true));	break;
	    case TFld::Real:	cfg.cfg("VAL").setR(buf.getR(&beg,true));	break;
	    case TFld::String:	cfg.cfg("VAL").setS(buf.getS(&beg,true));	break;
	}
	ctm = (beg/period())*period();
	cfg.cfg("TM").setI(ctm/1000000);
	cfg.cfg("TMU").setI(ctm%1000000);
	tbl.at().fieldSet(cfg);
	//- Archive time border update -
	m_beg = m_beg ? vmin(m_beg,ctm) : ctm;
	m_end = m_end ? vmax(m_end,ctm) : ctm;
    }

    //> Archive size limit process
    if( (m_end-m_beg) > (long long)(archivator().maxSize()*3600000000.) )
    {
	long long n_end = ((m_end-(long long)(archivator().maxSize()*3600000000.))/period())*period();
	for( long long t_c = vmax(m_beg,n_end-3600ll*period()); t_c < n_end; t_c+=period() )
	{
	    cfg.cfg("TM").setI(t_c/1000000,true);
	    cfg.cfg("TMU").setI(t_c%1000000,true);
	    tbl.at().fieldDel(cfg);
	}
	m_beg=n_end;
    }
    tbl.free();
    SYS->db().at().close(archivator().addr()+"."+archTbl());

    //- Update archive info -
    cfg.setElem(&mod->archEl());
    cfg.cfgViewAll(false);
    cfg.cfg("TBL").setS(archTbl(),true);
    cfg.cfg("BEGIN").setS(TSYS::ll2str(m_beg),true);
    cfg.cfg("END").setS(TSYS::ll2str(m_end),true);
    cfg.cfg("PRM1").setS(TSYS::ll2str(m_per),true);
    SYS->db().at().dataSet(archivator().addr()+"."+mod->mainTbl(),"",cfg);
}


//OpenSCADA system module Archive.DBArch file: mess.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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

#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include <tsys.h>
#include "arch.h"
#include "mess.h"


using namespace DBArch;

//************************************************
//* DBArch::ModMArch - Messages archivator       *
//************************************************
ModMArch::ModMArch( const string &iid, const string &idb, TElem *cf_el ) :
    TMArchivator(iid,idb,cf_el), m_beg(0), m_end(0), tm_calc(0.0),
    m_max_size(cfg("DBArchSize").getRd())
{
    setAddr("*.*");
}

ModMArch::~ModMArch( )
{
    try{ stop(); }catch(...){}
}

void ModMArch::postDisable( int flag )
{
    TMArchivator::postDisable( flag );
    try
    {
	if( flag )
	{
	    //> Remove info record
	    TConfig cfg(&mod->archEl());
	    cfg.cfg("TBL").setS(archTbl(),true);
	    SYS->db().at().dataDel(addr()+"."+mod->mainTbl(),"",cfg);

	    //> Remove archive's DB table
	    SYS->db().at().open( addr()+"."+archTbl() );
	    SYS->db().at().close( addr()+"."+archTbl(), true );
	}
    }
    catch(TError err)	{ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void ModMArch::load_( )
{
    TMArchivator::load_();

    //> Init address to DB
    if( addr().empty() ) setAddr("*.*");

    //> Load message archive parameters
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(addr()+"."+mod->mainTbl(),"",cfg))
    {
	m_beg = atoi(cfg.cfg("BEGIN").getS().c_str());
	m_end = atoi(cfg.cfg("END").getS().c_str());
	//>> Check for delete archivator table
	if( m_end <= (time(NULL)-(time_t)(maxSize()*3600.)) )
	{
	    SYS->db().at().open(addr()+"."+archTbl());
	    SYS->db().at().close(addr()+"."+archTbl(),true);
	    m_beg = m_end = 0;
	}
    }
}

void ModMArch::start( )
{
    //> Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    if( !db.at().enableStat( ) ) db.at().enable();

    run_st = true;
}

void ModMArch::stop( )
{
    run_st = false;
}

time_t ModMArch::begin( )
{
    return m_beg;
}

time_t ModMArch::end()
{
    return m_end;
}

void ModMArch::put( vector<TMess::SRec> &mess )
{
    if(!run_st) throw TError(nodePath().c_str(),_("Archive is not started!"));

    AutoHD<TTable> tbl = SYS->db().at().open(addr()+"."+archTbl(),true);
    if( tbl.freeStat() ) return;

    TConfig cfg(&mod->messEl());
    unsigned long long t_cnt = SYS->shrtCnt();
    for( unsigned i_m = 0; i_m < mess.size(); i_m++)
    {
	if( !chkMessOK(mess[i_m].categ,mess[i_m].level) ) continue;

	//> Put record to DB
	cfg.cfg("TM").setI(mess[i_m].time);
	cfg.cfg("TMU").setI(mess[i_m].utime);
	cfg.cfg("CATEG").setS(mess[i_m].categ);
	cfg.cfg("MESS").setS(mess[i_m].mess);
	cfg.cfg("LEV").setI(mess[i_m].level);
	tbl.at().fieldSet(cfg);
	//> Archive time border update
	m_beg = m_beg ? vmin(m_beg,mess[i_m].time) : mess[i_m].time;
	m_end = m_end ? vmax(m_end,mess[i_m].time) : mess[i_m].time;
    }

    //> Archive size limit process
    if( (m_end-m_beg) > (time_t)(maxSize()*3600.) )
    {
	time_t n_end = m_end-(time_t)(maxSize()*3600.);
	for( time_t t_c = vmax(m_beg,n_end-3600); t_c < n_end; t_c++ )
	{
	    cfg.cfg("TM").setI(t_c,true);
	    tbl.at().fieldDel(cfg);
	}
	m_beg=n_end;
    }
    tbl.free();
    SYS->db().at().close(addr()+"."+archTbl());

    //> Update archive info
    cfg.setElem(&mod->archEl());
    cfg.cfgViewAll(false);
    cfg.cfg("TBL").setS(archTbl(),true);
    cfg.cfg("BEGIN").setS(TSYS::int2str(m_beg),true);
    cfg.cfg("END").setS(TSYS::int2str(m_end),true);
    SYS->db().at().dataSet(addr()+"."+mod->mainTbl(),"",cfg);

    tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
}

void ModMArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level )
{
    if( !run_st ) throw TError(nodePath().c_str(),_("Archive is not started!"));

    b_tm = vmax(b_tm,begin());
    e_tm = vmin(e_tm,end());
    if( e_tm <= b_tm ) return;

    TConfig cfg(&mod->messEl());
    //> Get values from DB
    for( time_t t_c = b_tm; t_c <= e_tm; t_c++ )
    {
	cfg.cfg("TM").setI(t_c,true);
	for( int e_c = 0; SYS->db().at().dataSeek(addr()+"."+archTbl(),"",e_c++,cfg); )
	{
	    TMess::SRec rc(t_c,cfg.cfg("TMU").getI(),cfg.cfg("CATEG").getS(),(TMess::Type)cfg.cfg("LEV").getI(),cfg.cfg("MESS").getS());
	    if( rc.level >= level && TMess::chkPattern(rc.categ,category) )
	    {
		bool equal = false;
		int i_p = mess.size();
		for( int i_m = mess.size()-1; i_m >= 0; i_m-- )
		{
		    if( FTM(mess[i_m]) > FTM(rc) ) i_p = i_m;
		    else if( FTM(mess[i_m]) == FTM(rc) && rc.level == mess[i_m].level && rc.mess == mess[i_m].mess )
		    { equal = true; break; }
		    else if( FTM(mess[i_m]) < FTM(rc) ) break;
		}
		if( !equal )
		{
		    mess.insert(mess.begin()+i_p,rc);
		    if( mess.size() >= TArchiveS::max_req_mess ) return;
		}
	    }
	}
    }
}

void ModMArch::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TMArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time (msek)"),0444,"root","Archive",1,"tp","real");
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","Archive",4,"tp","str","dest","select","select","/db/list",
		"help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/sz",cfg("DBArchSize").fld().descr(),0664,"root","Archive",1,"tp","real");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/tarch" && ctrChkNode(opt) ) 	opt->setText(TSYS::real2str(tm_calc,6));
    else if( a_path == "/prm/cfg/sz" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) ) opt->setText(TSYS::real2str( maxSize() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) ) setMaxSize( atof(opt->text().c_str()) );
    }
    else TMArchivator::cntrCmdProc(opt);
}

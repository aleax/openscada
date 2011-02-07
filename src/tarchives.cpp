
//OpenSCADA system file: tarchives.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <algorithm>

#include "tsys.h"
#include "tarchives.h"

#define BUF_SIZE_DEF 500
#define BUF_SIZE_MAX 100000

using namespace OSCADA;

//************************************************
//* Archive subsystem                            *
//************************************************

//************************************************
//* TArchiveS                                    *
//************************************************
unsigned TArchiveS::max_req_mess = 3000;

TArchiveS::TArchiveS( ) :
    TSubSYS(SARH_ID,"Archives",true), prcStMess(false), prcStVal(false), endrunReqVal(false), mMessPer(10),
    mValPer(1000), mValPrior(10), headBuf(0), headLstread(0), elMess(""), elVal(""), elAval(""), bufErr(0)
{
    mAval = grpAdd("va_");

    //> Message archivator DB structure
    elMess.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    elMess.fldAdd( new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key,"20") );
    elMess.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    elMess.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200") );
    elMess.fldAdd( new TFld("START",_("Start archive"),TFld::Boolean,0,"1") );
    elMess.fldAdd( new TFld("CATEG",_("Messages categories"),TFld::String,0,"100") );
    elMess.fldAdd( new TFld("LEVEL",_("Messages level"),TFld::Integer,0,"1","","0;7") );
    elMess.fldAdd( new TFld("ADDR",_("Address"),TFld::String,0,"100") );

    //> Value archivator DB structure
    elVal.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    elVal.fldAdd( new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key,"20") );
    elVal.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    elVal.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200") );
    elVal.fldAdd( new TFld("START",_("Start archive"),TFld::Boolean,0,"1","0") );
    elVal.fldAdd( new TFld("ADDR",_("Address"),TFld::String,0,"50") );
    elVal.fldAdd( new TFld("V_PER",_("Value period (sec)"),TFld::Real,0,"12.6","1","0;1000000") );
    elVal.fldAdd( new TFld("A_PER",_("Period archiving (sec)"),TFld::Integer,0,"4","60","0;1000") );

    //> Value archive DB structure
    elAval.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    elAval.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    elAval.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200") );
    elAval.fldAdd( new TFld("START",_("Start archive"),TFld::Boolean,0,"1","0") );
    elAval.fldAdd( new TFld("SrcMode",_("Source mode"),TFld::Integer,0,"1") );
    elAval.fldAdd( new TFld("Source",_("Source"),TFld::String,0,"100") );
    elAval.fldAdd( new TFld("VTYPE",_("Value type"),TFld::Integer,0,"1") );
    elAval.fldAdd( new TFld("BPER",_("Buffer period (sec)"),TFld::Real,0,"9.6","1","0;10000") );
    elAval.fldAdd( new TFld("BSIZE",_("Buffer size (items)"),TFld::Integer,0,"6","100","0;1000000") );
    elAval.fldAdd( new TFld("BHGRD",_("Buffer hard time griding"),TFld::Boolean,0,"1","1") );
    elAval.fldAdd( new TFld("BHRES",_("Buffer high time resolution"),TFld::Boolean,0,"1","0") );
    elAval.fldAdd( new TFld("ArchS",_("Process into archivators"),TFld::String,0,"500") );

    setMessBufLen( BUF_SIZE_DEF );

    //> Create message archive timer
    struct sigevent sigev;
    memset(&sigev,0,sizeof(sigev));
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = ArhMessTask;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmIdMess);
}

TArchiveS::~TArchiveS(  )
{
    //> Stop messages timer
    timer_delete(tmIdMess);

    //> Stop values archiving task
    if( prcStVal ) SYS->taskDestroy( nodePath('.',true)+".vals", &prcStVal, &endrunReqVal );

    //> Free other resources
    nodeDelAll();
}

int TArchiveS::valPeriod( )		{ return vmax(1,mValPer); }

void TArchiveS::setValPrior( int ivl )	{ mValPrior = vmax(-1,vmin(99,ivl)); modif(); }

void TArchiveS::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=0,opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters
    setMessBufLen( atoi(TBDS::genDBGet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen())).c_str()) );
    setMessPeriod( atoi(TBDS::genDBGet(nodePath()+"MessPeriod",TSYS::int2str(mMessPer)).c_str()) );
    setValPeriod( atoi(TBDS::genDBGet(nodePath()+"ValPeriod",TSYS::int2str(mValPer)).c_str()) );
    setValPrior( atoi(TBDS::genDBGet(nodePath()+"ValPriority",TSYS::int2str(mValPrior)).c_str()) );
    max_req_mess = vmax(100,atoi(TBDS::genDBGet(nodePath()+"MaxReqMess",TSYS::int2str(max_req_mess)).c_str()));

    //> LidDB
    //>> Message archivators load
    string id,type;
    try
    {
	TConfig c_el(&elMess);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//>> Search int DB and create new archivators
	SYS->db().at().dbList(db_ls,true);
	for(int i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_mess_proc","",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().messPresent(id))
		    at(type).at().messAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }
	//>> Search into config file and create new archivators
	if(SYS->chkSelDB("<cfg>"))
	    for(int fld_cnt=0; SYS->db().at().dataSeek("",nodePath()+subId()+"_mess_proc",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().messPresent(id)) at(type).at().messAdd(id,"*.*");
	    }
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Message archivators load error."));
    }

    //>> Value archivators load
    try
    {
	TConfig c_el(&elVal);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//>> Search into DB and create new archivators
	SYS->db().at().dbList(db_ls,true);
	for(int i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_val_proc","",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().valPresent(id))
		    at(type).at().valAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }
	//>> Search into config file and create new archivators
	if(SYS->chkSelDB("<cfg>"))
	    for(int fld_cnt=0; SYS->db().at().dataSeek("",nodePath()+subId()+"_val_proc",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if(modPresent(type) && !at(type).at().valPresent(id)) at(type).at().valAdd(id,"*.*");
	    }
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str()); 
	mess_err(nodePath().c_str(),_("Value archivators load error."));
    }

    //>> Value archives load
    try
    {
	TConfig c_el(&elAval);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//>> Search into DB and create new archives
	SYS->db().at().dbList(db_ls,true);
	for(int i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt=0; SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_val","",fld_cnt++,c_el); )
	    {
	        id = c_el.cfg("ID").getS();
	        if(!valPresent(id)) valAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }
	//>> Search into config file and create new archives
	if(SYS->chkSelDB("<cfg>"))
	    for(int fld_cnt=0; SYS->db().at().dataSeek("",nodePath()+subId()+"_val",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		if(!valPresent(id)) valAdd(id,"*.*");
	    }
    }catch(TError err)
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Value archives load error."));
    }
}

void TArchiveS::save_( )
{
    vector<string> t_lst, o_lst;

    //> Save parameters
    TBDS::genDBSet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen()));
    TBDS::genDBSet(nodePath()+"MessPeriod",TSYS::int2str(messPeriod()));
    TBDS::genDBSet(nodePath()+"ValPeriod",TSYS::int2str(valPeriod()));
    TBDS::genDBSet(nodePath()+"ValPriority",TSYS::int2str(valPrior()));
    TBDS::genDBSet(nodePath()+"MaxReqMess",TSYS::int2str(max_req_mess));
}

void TArchiveS::valAdd( const string &iid, const string &idb )
{
    if( valPresent(iid) ) return;
    chldAdd(mAval,new TVArchive(iid,idb,&aValE()));
}

string TArchiveS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================== Subsystem \"Archives\" options ===================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"MessBufSize   <items>       Messages buffer size.\n"
	"MessPeriod    <sec>         Message arhiving period.\n"
	"ValPeriod     <msec>        Values arhiving period.\n"
	"ValPriority   <level>	     Values task priority level.\n"
	"MaxReqMess    <items>       Maximum request messages.\n"
	"MaxReqVals    <items>       Maximum request values.\n\n"
	),nodePath().c_str());

    return buf;
}

void TArchiveS::subStart( )
{
    mess_info(nodePath().c_str(),_("Start subsystem."));

    SubStarting = true;

    vector<string> t_lst, o_lst;

    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);

	//> Message archivators start
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TMArchivator> mess = mod.at().messAt(o_lst[i_o]);
	    if( !mess.at().startStat() && mess.at().toStart() )
		try{ mess.at().start(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str()); 
		    mess_err(nodePath().c_str(),_("Message archivator <%s> start error."),o_lst[i_o].c_str());
		}
	}
	//> Value archivators start
	mod.at().valList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TVArchivator> val = mod.at().valAt(o_lst[i_o]);
	    if( !val.at().startStat() && val.at().toStart() )
		try{ val.at().start(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Value archivator <%s> start error."),val.at().workId().c_str());
		}
	}
    }

    //> Value archives start
    valList(o_lst);
    for( int i_o = 0; i_o < o_lst.size(); i_o++ )
    {
	AutoHD<TVArchive> aval = valAt(o_lst[i_o]);
	if( !aval.at().startStat() && aval.at().toStart() )
	    try{ aval.at().start(); }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Value archive <%s> start error."),o_lst[i_o].c_str());
	    }
    }

    //> Start messages interval timer
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = messPeriod();
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);

    //> Start values acquisition task
    if( !prcStVal ) SYS->taskCreate( nodePath('.',true)+".vals", valPrior(), TArchiveS::ArhValTask, this, &prcStVal );

    TSubSYS::subStart( );

    SubStarting = false;
}

void TArchiveS::subStop( )
{
    mess_info(nodePath().c_str(),_("Stop subsystem."));

    TSubSYS::subStop( );

    vector<string> t_lst, o_lst;

    //> Stop interval timer for periodic thread creating for messages archivation
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
    if( TSYS::eventWait( prcStMess, false, nodePath()+"mess_stop",10) )
	throw TError(nodePath().c_str(),_("Archive messages thread is not stopped!"));

    //> Values acquisition task stop
    if( prcStVal ) SYS->taskDestroy( nodePath('.',true)+".vals", &prcStVal, &endrunReqVal );

    //> Last messages archivation call
    sigval obj; obj.sival_ptr = this;
    ArhMessTask(obj);

    //> Archivators stop
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);
	//Value archives stop
	mod.at().valList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TVArchivator> val = mod.at().valAt(o_lst[i_o]);
	    if( val.at().startStat() )
		try{ val.at().stop(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Value archivator <%s> stop error."),o_lst[i_o].c_str());
		}
	}
	// Message archivators stop
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TMArchivator> mess = mod.at().messAt(o_lst[i_o]);
	    if( mess.at().startStat() )
		try{ mess.at().stop(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Message archivator <%s> stop error."),o_lst[i_o].c_str());
		}
	}
    }

    //> Value archives stop
    valList(o_lst);
    for( int i_o = 0; i_o < o_lst.size(); i_o++ )
    {
	AutoHD<TVArchive> aval = valAt(o_lst[i_o]);
	if( aval.at().startStat() )
	    try{ aval.at().stop(); }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Value archive <%s> stop error."),o_lst[i_o].c_str());
	    }
    }
}

void TArchiveS::messPut( time_t tm, int utm, const string &categ, int8_t level, const string &mess )
{
    //> Put message to buffer
    ResAlloc res(mRes,true);
    mBuf[headBuf].time  = tm;
    mBuf[headBuf].utime = utm;
    mBuf[headBuf].categ = categ;
    mBuf[headBuf].level = (TMess::Type)abs(level);
    mBuf[headBuf].mess  = mess;
    if( ++headBuf >= mBuf.size() ) headBuf = 0;
    //> Check to no archivated messages
    if( headBuf == headLstread )
    {
	if( !(bufErr&0x01) )
	{
	    bufErr |= 0x01;
	    res.release();
	    mess_err(nodePath().c_str(),_("Buffer full. Messages lost!"));
	    res.request(true);
	}
	if( ++headLstread >= mBuf.size() ) headLstread = 0;
    }
    //> Check fill buffer speed.
    else if( headBuf-headLstread > messBufLen( )/2 )
    {
	if( !(bufErr&0x02) )
	{
	    bufErr |= 0x02;
	    res.release();
	    mess_warning(nodePath().c_str(),_("Messagess buffer filling is too fast!"));
	    res.request(true);
	}
    }
    else bufErr = 0;

    //> Alarms processing. For level less 0 alarm is set
    map<string,TMess::SRec>::iterator p;
    if( level < 0 ) mAlarms[categ] = TMess::SRec(tm,utm,categ,(TMess::Type)abs(level),mess);
    else if( (p=mAlarms.find(categ)) != mAlarms.end() ) mAlarms.erase(p);
}

void TArchiveS::messPut( const vector<TMess::SRec> &recs )
{
    for( int i_r = 0; i_r < recs.size(); i_r++ )
	messPut(recs[i_r].time,recs[i_r].utime,recs[i_r].categ,recs[i_r].level,recs[i_r].mess);
}

void TArchiveS::messGet( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category, int8_t level, const string &arch )
{
    recs.clear();

    ResAlloc res(mRes,false);

    //> Get records from buffer
    int i_buf = headBuf;
    while( level >= 0 && (!arch.size() || arch==BUF_ARCH_NM) )
    {
	if( mBuf[i_buf].time >= b_tm && mBuf[i_buf].time != 0 && mBuf[i_buf].time <= e_tm &&
		mBuf[i_buf].level >= level && TMess::chkPattern( mBuf[i_buf].categ, category ) )
	    recs.push_back(mBuf[i_buf]);
	if( ++i_buf >= mBuf.size() ) i_buf = 0;
	if(i_buf == headBuf) break;
    }

    //> Get records from archives
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( int i_t = 0; level >= 0 && i_t < t_lst.size(); i_t++ )
    {
	at(t_lst[i_t]).at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TMArchivator> archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
	    if( archtor.at().startStat() && (!arch.size() || arch==archtor.at().workId()) )
		archtor.at().get(b_tm,e_tm,recs,category,level);
	}
    }

    //> Alarms request processing
    if( level < 0 )
    {
	vector< pair<long long,TMess::SRec* > > mb;
	for( map<string,TMess::SRec>::iterator p = mAlarms.begin(); p != mAlarms.end(); p++ )
	    if( (p->second.time >= b_tm || b_tm == e_tm) && p->second.time <= e_tm &&
		    p->second.level >= abs(level) && Mess->chkPattern(p->second.categ,category) )
		mb.push_back(pair<long long,TMess::SRec* >(FTM(p->second),&p->second));
	sort(mb.begin(),mb.end());
	for( int i_b = 0; i_b < mb.size(); i_b++ ) recs.push_back(*mb[i_b].second);
    }
}

time_t TArchiveS::messBeg( const string &arch )
{
    time_t rez = 0;
    ResAlloc res(mRes,false);
    if( arch.empty() || arch == BUF_ARCH_NM )
    {
	int i_buf = headBuf;
	while( !arch.size() || arch==BUF_ARCH_NM )
	{
	    rez = rez ? vmin(rez,mBuf[i_buf].time) : mBuf[i_buf].time;
	    if( ++i_buf >= mBuf.size() ) i_buf = 0;
	    if(i_buf == headBuf) break;
	}
	if( !arch.empty() ) return rez;
    }

    //- Get records from archives -
    vector<string> t_lst, o_lst;
    modList(t_lst);
    AutoHD<TMArchivator> archtor;
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	at(t_lst[i_t]).at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
	    if( archtor.at().startStat() && (!arch.size() || arch==archtor.at().workId()) )
		rez = rez ? vmin(rez,archtor.at().begin()) : archtor.at().begin();
	}
    }

    return rez;
}

time_t TArchiveS::messEnd( const string &arch )
{
    time_t rez = 0;
    ResAlloc res(mRes,false);
    if( arch.empty() || arch == BUF_ARCH_NM )
    {
	int i_buf = headBuf;
	while( !arch.size() || arch==BUF_ARCH_NM )
	{
	    rez = rez ? vmax(rez,mBuf[i_buf].time) : mBuf[i_buf].time;
	    if( ++i_buf >= mBuf.size() ) i_buf = 0;
	    if(i_buf == headBuf) break;
	}
	if( !arch.empty() ) return rez;
    }

    //> Get records from archives
    vector<string> t_lst, o_lst;
    modList(t_lst);
    AutoHD<TMArchivator> archtor;
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	at(t_lst[i_t]).at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
	    if( archtor.at().startStat() && (!arch.size() || arch==archtor.at().workId()) )
		rez = rez ? vmax(rez,archtor.at().end()) : archtor.at().end();
	}
    }

    return rez;
}

void TArchiveS::setMessBufLen(int len)
{
    ResAlloc res(mRes,true);
    len=(len<BUF_SIZE_DEF)?BUF_SIZE_DEF:(len>BUF_SIZE_MAX)?BUF_SIZE_MAX:len;
    while( mBuf.size() > len )
    {
	mBuf.erase( mBuf.begin() + headBuf );
	if( headBuf >= mBuf.size() ) headBuf = 0;
	if( headLstread >= mBuf.size() )	headLstread=mBuf.size()-1;
    }
    while( mBuf.size() < len )
	mBuf.insert( mBuf.begin() + headBuf, TMess::SRec() );
    modif();
}

void TArchiveS::setActValArch( const string &id, bool val )
{
    unsigned i_arch;

    ResAlloc res(vRes,true);
    for( i_arch = 0; i_arch < actUpSrc.size(); i_arch++ )
	if( actUpSrc[i_arch].at().id() == id ) break;

    if( val && i_arch >= actUpSrc.size() )
	actUpSrc.push_back(valAt(id));
    if( !val && i_arch < actUpSrc.size() )
	actUpSrc.erase(actUpSrc.begin()+i_arch);
}

void TArchiveS::setMessPeriod( int ivl )
{
    mMessPer = ivl;
    modif();

    if( subStartStat( ) )
    {
	struct itimerspec itval;
	itval.it_interval.tv_sec = itval.it_value.tv_sec = mMessPer;
	itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
	timer_settime(tmIdMess, 0, &itval, NULL);
    }
}

void TArchiveS::ArhMessTask( union sigval obj )
{
    TArchiveS &arh = *(TArchiveS *)obj.sival_ptr;
    if( arh.prcStMess )  return;
    arh.prcStMess = true;

    //> Message bufer read
    if( arh.headLstread != arh.headBuf )
	try
	{
	    ResAlloc res(arh.mRes,false);

	    //>> Get new messages
	    unsigned new_headLstread = arh.headBuf;
	    unsigned i_m = arh.headLstread;
	    vector<TMess::SRec> o_mess;
	    while( i_m != new_headLstread )
	    {
		o_mess.push_back(arh.mBuf[i_m]);
		if( ++i_m >= arh.mBuf.size() ) i_m = 0;
	    }
	    arh.headLstread = new_headLstread;

	    res.release();

	    //>> Put to archivators
	    vector<string> t_lst, o_lst;
	    arh.modList(t_lst);
	    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
	    {
		arh.at(t_lst[i_t]).at().messList(o_lst);
		for( int i_o = 0; i_o < o_lst.size(); i_o++ )
		    if( arh.at(t_lst[i_t]).at().messAt(o_lst[i_o]).at().startStat() )
			arh.at(t_lst[i_t]).at().messAt(o_lst[i_o]).at().put(o_mess);
	    }
	}
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(arh.nodePath().c_str(),_("Message bufer read error."));
	}

    arh.prcStMess = false;
}

void *TArchiveS::ArhValTask( void *param )
{
    TArchiveS &arh = *(TArchiveS *)param;
    arh.endrunReqVal = false;
    arh.prcStVal = true;

    while( !arh.endrunReqVal )
    {
	long long work_tm = SYS->curTime();

	arh.vRes.resRequestR( );
	for(unsigned i_arh = 0; i_arh < arh.actUpSrc.size(); i_arh++)
	    try
	    {
		if( work_tm/arh.actUpSrc[i_arh].at().period() > arh.actUpSrc[i_arh].at().end()/arh.actUpSrc[i_arh].at().period() )
		    arh.actUpSrc[i_arh].at().getActiveData();
	    }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	arh.vRes.resRelease( );

	TSYS::taskSleep((long long)arh.valPeriod()*1000000);
    }

    arh.prcStVal = false;

    return NULL;
}

TVariant TArchiveS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // Area messGet(int btm, int etm, string cat = "", int lev = 0, string arch = ""); - request of the system messages for the time from <btm>
    //       to <etm> for the category <cat>, level <lev> and archiver <arch>
    //  btm - begin time
    //  etm - end time
    //  cat - messages' category
    //  lev - messages level
    //  arch - messages archivator
    if( iid == "messGet" && prms.size() >= 2 )
    {
	vector<TMess::SRec> recs;
	messGet( prms[0].getI(), prms[1].getI(), recs, ((prms.size()>=3) ? prms[2].getS() : string("")),
	    ((prms.size()>=4) ? prms[3].getI() : 0), ((prms.size()>=5) ? prms[4].getS() : string("")) );
	TArrayObj *rez = new TArrayObj();
	for( int i_m = 0; i_m < recs.size(); i_m++ )
	{
	    TVarObj *am = new TVarObj();
	    am->propSet("tm",(int)recs[i_m].time);
	    am->propSet("utm",recs[i_m].utime);
	    am->propSet("categ",recs[i_m].categ);
	    am->propSet("level",recs[i_m].level);
	    am->propSet("mess",recs[i_m].mess);
	    rez->propSet(TSYS::int2str(i_m),am);
	}
	return rez;
    }

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TArchiveS::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //> Service commands process
    if(a_path == "/serv/mess")		//Messages access
    {
	if(ctrChkNode(opt,"info",RWRWRW,"root",SARH_ID,SEC_RD))		//Messages information
	{
	    string arch = opt->attr("arch");
	    opt->setAttr("end",TSYS::uint2str(messEnd(arch)));
	    opt->setAttr("beg",TSYS::uint2str(messBeg(arch)));
	}
	else if(ctrChkNode(opt,"get",RWRWRW,"root",SARH_ID,SEC_RD))	//Value's data request
	{
	    time_t tm      = strtoul(opt->attr("tm").c_str(),0,10);
	    time_t tm_grnd = strtoul(opt->attr("tm_grnd").c_str(),0,10);
	    string arch    = opt->attr("arch");
	    string cat     = opt->attr("cat");
	    int    lev     = atoi(opt->attr("lev").c_str());
	    vector<TMess::SRec> rez;
	    messGet( tm_grnd, tm, rez, cat, (TMess::Type)lev, arch );
	    for( int i_r = 0; i_r < rez.size(); i_r++ )
		opt->childAdd("el")->
		    setAttr("time",TSYS::uint2str(rez[i_r].time))->
		    setAttr("utime",TSYS::uint2str(rez[i_r].utime))->
		    setAttr("cat",rez[i_r].categ)->
		    setAttr("lev",TSYS::int2str(rez[i_r].level))->
		    setText(rez[i_r].mess);
	}
	return;
    }

    //> Get page info
    if(opt->name() == "info")
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/va_",_("Value archive"),RWRWR_,"root",SARH_ID,2,"idm","1","idSz","20");
	if(ctrMkNode("area",opt,1,"/m_arch",_("Messages archive"),R_R_R_,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/m_arch/max_am_req",_("Maximum requested messages"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/m_arch/size",_("Messages buffer size"),RWRWR_,"root",SARH_ID,2,"tp","dec","min",TSYS::int2str(BUF_SIZE_DEF).c_str());
	    ctrMkNode("fld",opt,-1,"/m_arch/per",_("Archiving period (s)"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    if(ctrMkNode("area",opt,-1,"/m_arch/view",_("View messages"),R_R___,"root",SARH_ID))
	    {
		ctrMkNode("fld",opt,-1,"/m_arch/view/tm",_("Time"),RWRW__,"root",SARH_ID,1,"tp","time");
		ctrMkNode("fld",opt,-1,"/m_arch/view/size",_("Size (s)"),RWRW__,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/m_arch/view/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str",
		    "help",_("Messages category. Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/lvl",_("Level"),RWRW__,"root",SARH_ID,4,"tp","dec","min","-7","max","7",
		    "help",_("Get messages for level more and equaly it."));
		ctrMkNode("fld",opt,-1,"/m_arch/view/archtor",_("Archivator"),RWRW__,"root",SARH_ID,4,"tp","str","dest","select","select","/m_arch/lstAMess",
		    "help",_("Messages archivator.\nNo set archivator for process by buffer and all archivators.\nSet '<buffer>' for process by buffer."));
		if(ctrMkNode("table",opt,-1,"/m_arch/view/mess",_("Messages"),R_R___,"root",SARH_ID))
		{
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/1",_("Category"),R_R___,"root",SARH_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/2",_("Lev."),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/3",_("Message"),R_R___,"root",SARH_ID,1,"tp","str");
		}
	    }
	}
	if(ctrMkNode("area",opt,2,"/v_arch",_("Value archives"),R_R_R_,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/v_arch/per",_("Get data period (ms)"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/prior",_("Get data task priority level"),RWRWR_,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/nmb",_("Number"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/v_arch/archs",_("Value archives"),RWRWR_,"root",SARH_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","va_","idSz","20");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SARH_ID,3,"tp","str","cols","90","rows","10");
	return;
    }

    //> Process command to page
    if(a_path == "/m_arch/max_am_req")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(max_req_mess));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	{ max_req_mess = vmax(100,atoi(opt->text().c_str())); modif(); }
    }
    else if(a_path == "/m_arch/per")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(messPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessPeriod(atoi(opt->text().c_str()));
    }
    else if(a_path == "/m_arch/size")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(messBufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMessBufLen(atoi(opt->text().c_str()));
    }
    else if(a_path == "/m_arch/view/tm")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))
	{
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if( !atoi(opt->text().c_str()) )    opt->setText(TSYS::int2str(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(nodePath()+"messTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/size")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/cat")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/lvl")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/view/archtor")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messArch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/m_arch/lstAMess" && ctrChkNode(opt,"get",R_R___))
    {
	opt->childAdd("el")->setText("");
	opt->childAdd("el")->setText(BUF_ARCH_NM);
	vector<string> lsm, lsa;
	modList(lsm);
	for(int i_m = 0; i_m < lsm.size(); i_m++)
	{
	    at(lsm[i_m]).at().messList(lsa);
	    for( int i_a = 0; i_a < lsa.size(); i_a++ )
		opt->childAdd("el")->setText(lsm[i_m]+"."+lsa[i_a]);
	}
    }
    else if(a_path == "/m_arch/view/mess" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID))
    {
	vector<TMess::SRec> rec;
	time_t gtm = atoi(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")).c_str());
	if( !gtm ) gtm = time(NULL);
	int gsz = atoi(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")).c_str());
	messGet( gtm-gsz, gtm, rec,
		 TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")),
		 atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str()),
		 TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")) );

	XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0","",R_R___,"root",SARH_ID);
	XMLNode *n_tmu	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/0a","",R_R___,"root",SARH_ID);
	XMLNode *n_cat	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/1","",R_R___,"root",SARH_ID);
	XMLNode *n_lvl	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/2","",R_R___,"root",SARH_ID);
	XMLNode *n_mess	= ctrMkNode("list",opt,-1,"/m_arch/view/mess/3","",R_R___,"root",SARH_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--)
	{
	    if(n_tm)	n_tm->childAdd("el")->setText(TSYS::int2str(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(TSYS::int2str(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else if(a_path == "/v_arch/per")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(valPeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPeriod(atoi(opt->text().c_str()));
    }
    else if(a_path == "/v_arch/prior")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(valPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setValPrior(atoi(opt->text().c_str()));
    }
    else if(a_path == "/v_arch/nmb" && ctrChkNode(opt))
    {
	vector<string> list;
	valList(list);
	int e_c = 0;
	for(int i_a = 0; i_a < list.size(); i_a++)
	    if(valAt(list[i_a]).at().startStat()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),list.size(),e_c));
    }
    else if(a_path == "/br/va_" || a_path == "/v_arch/archs")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))
	{
	    vector<string> list;
	    valList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    valAdd(vid); valAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	chldDel(mAval,opt->attr("id"),-1,1);
    }
    else if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID))	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTipArchivator                               *
//************************************************
TTipArchivator::TTipArchivator( const string &id ) : TModule(id)
{
    mVal = grpAdd("val_");
    mMess = grpAdd("mess_");
}

TTipArchivator::~TTipArchivator()
{
    nodeDelAll();
}

TArchiveS &TTipArchivator::owner()
{
    return (TArchiveS &)TModule::owner();
}

void TTipArchivator::messAdd(const string &name, const string &idb )
{
    if( chldPresent(mMess,name) ) return;
    chldAdd(mMess,AMess(name,idb));
}

void TTipArchivator::valAdd( const string &iid, const string &idb )
{
    if( chldPresent(mVal,iid) ) return;
    chldAdd(mVal,AVal(iid,idb));
}

void TTipArchivator::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/arch",_("Archivators"));
	ctrMkNode("grp",opt,-1,"/br/mess_",_("Message archivator"),RWRWR_,"root",SARH_ID,2,"idm","1","idSz","20");
	ctrMkNode("grp",opt,-1,"/br/val_",_("Value archivator"),RWRWR_,"root",SARH_ID,2,"idm","1","idSz","20");
	ctrMkNode("list",opt,-1,"/arch/mess",_("Message archivators"),RWRWR_,"root",SARH_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","mess_","idSz","20");
	ctrMkNode("list",opt,-1,"/arch/val",_("Value archivators"),RWRWR_,"root",SARH_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","val_","idSz","20");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/mess_" || a_path == "/arch/mess")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))
	{
	    vector<string> list;
	    messList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(messAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    messAdd(vid); messAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	messDel(opt->attr("id"),true);
    }
    else if(a_path == "/br/val_" || a_path == "/arch/val")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))
	{
	    vector<string> list;
	    valList(list);
	    for(unsigned i_a=0; i_a < list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SARH_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    valAdd(vid); valAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SARH_ID,SEC_WR))	valDel(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* Message archivator                           *
//************************************************

//************************************************
//* TMArchivator                                 *
//************************************************
TMArchivator::TMArchivator(const string &iid, const string &idb, TElem *cf_el) :
    TConfig( cf_el ), m_db(idb), run_st(false),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_dscr(cfg("DESCR").getSd()), m_addr(cfg("ADDR").getSd()),
    m_cat_o(cfg("CATEG").getSd()), m_level(cfg("LEVEL").getId()) ,m_start(cfg("START").getBd())
{
    m_id = iid;
}

TCntrNode &TMArchivator::operator=( TCntrNode &node )
{
    TMArchivator *src_n = dynamic_cast<TMArchivator*>(&node);
    if( !src_n ) return *this;

    //> Configuration copy
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    cfg("MODUL").setS(owner().modId());
    m_id = tid;
    m_db = src_n->m_db;

    if( src_n->startStat() && toStart() && !startStat() )
        start( );

    return *this;
}

void TMArchivator::postEnable( int flag )
{
    cfg("MODUL").setS(owner().modId());
}

void TMArchivator::preDisable( int flag )
{
    if( startStat() )	stop( );
}

void TMArchivator::postDisable(int flag)
{
    try
    {
	if( flag )
	    SYS->db().at().dataDel(fullDB(),SYS->archive().at().nodePath()+tbl(),*this,true);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TTipArchivator &TMArchivator::owner( )	{ return *(TTipArchivator*)nodePrev(); }

string TMArchivator::workId( )		{ return owner().modId()+"."+id(); }

string TMArchivator::name()		{ return (m_name.size())?m_name:m_id; }

string TMArchivator::tbl( )		{ return owner().owner().subId()+"_mess_proc"; }

void TMArchivator::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;
    SYS->db().at().dataGet(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
}

void TMArchivator::save_( )
{
    SYS->db().at().dataSet(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
}

void TMArchivator::categ( vector<string> &list )
{
    list.clear();
    string c_vl;
    for( int i_off = 0; (c_vl=TSYS::strSepParse(m_cat_o,0,';',&i_off)).size(); )
	list.push_back(c_vl);
}

bool TMArchivator::chkMessOK( const string &icateg, TMess::Type ilvl )
{
    vector<string> cat_ls;

    categ(cat_ls);

    if( ilvl >= level() )
	for( unsigned i_cat = 0; i_cat < cat_ls.size(); i_cat++ )
	    if( TMess::chkPattern(icateg,cat_ls[i_cat]) )
		return true;
   return false;
}

void TMArchivator::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Message archivator: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archivator")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archivator DB"),RWRWR_,"root","root",4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/prm/st/end",_("End"),R_R_R_,"root","root",1,"tp","time");
		ctrMkNode("fld",opt,-1,"/prm/st/beg",_("Begin"),R_R_R_,"root","root",1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),RWRWR_,"root",SARH_ID,2,"tp","str","len","50");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),RWRWR_,"root",SARH_ID,3,"tp","str","cols","90","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),RWRWR_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/lvl",cfg("LEVEL").fld().descr(),RWRWR_,"root",SARH_ID,2,"tp","dec",
		    "help",_("Get messages for level more and equaly it."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/cats",cfg("CATEG").fld().descr(),RWRWR_,"root",SARH_ID,2,"tp","str",
		    "help",_("Messages categories to processing by archivator, separated by symbol ';'.\n"
			   "Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",_("To start"),RWRWR_,"root",SARH_ID,1,"tp","bool");
	    }
	}
	if(run_st && ctrMkNode("area",opt,-1,"/mess",_("Messages"),R_R___,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size",_("Size (s)"),RWRW__,"root",SARH_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/cat",_("Category pattern"),RWRW__,"root",SARH_ID,2,"tp","str",
		"help",_("Messages category. Use template symbols for group selection:\n  '*' - any substring;\n  '?' - any symbol."));
	    ctrMkNode("fld",opt,-1,"/mess/lvl",_("Level"),RWRW__,"root",SARH_ID,4,"tp","dec","min","0","max","7",
		"help",_("Get messages for level more and equaly it."));
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SARH_ID))
	    {
		ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/mess/mess/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mess/mess/2",_("Level"),R_R___,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/3",_("Message"),R_R___,"root",SARH_ID,1,"tp","str");
	    }
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( startStat() ? "1" : "0" );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	atoi(opt->text().c_str()) ? start() : stop();
    }
    else if(a_path == "/prm/st/db")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( DB() );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB( opt->text() );
    }
    else if(a_path == "/prm/st/end" && ctrChkNode(opt))		opt->setText( TSYS::int2str(end()) );
    else if(a_path == "/prm/st/beg" && ctrChkNode(opt))		opt->setText( TSYS::int2str(begin()) );
    else if(a_path == "/prm/cfg/id" && ctrChkNode(opt))		opt->setText( id() );
    else if(a_path == "/prm/cfg/nm" )
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( name() );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setName( opt->text() );
    }
    else if(a_path == "/prm/cfg/dscr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( dscr() );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDscr( opt->text() );
    }
    else if(a_path == "/prm/cfg/addr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( addr() );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setAddr( opt->text() );
    }
    else if(a_path == "/prm/cfg/lvl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::int2str(level()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setLevel( atoi(opt->text().c_str()) );
    }
    else if(a_path == "/prm/cfg/start")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText( toStart() ? "1" : "0" );
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setToStart( atoi(opt->text().c_str()) );
    }
    else if(a_path == "/prm/cfg/cats")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(m_cat_o);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	{ m_cat_o = opt->text(); modif(); }
    }
    else if(a_path == "/mess/tm")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))
	{
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if(!atoi(opt->text().c_str()))	opt->setText(TSYS::int2str(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(nodePath()+"messTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/cat")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/lvl")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/mess" && run_st && ctrChkNode(opt,"get",R_R___,"root",SARH_ID))
    {
	vector<TMess::SRec> rec;
	time_t end = atoi(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")).c_str());
	if( !end ) end = time(NULL);
	time_t beg = end - atoi(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")).c_str());
	string cat = TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user"));
	char   lev = atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str());

	get( beg, end, rec, cat, lev );

	XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SARH_ID);
	XMLNode *n_tmu	= ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SARH_ID);
	XMLNode *n_cat	= ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SARH_ID);
	XMLNode *n_lvl	= ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SARH_ID);
	XMLNode *n_mess	= ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SARH_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--)
	{
	    if(n_tm)	n_tm->childAdd("el")->setText(TSYS::int2str(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(TSYS::int2str(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

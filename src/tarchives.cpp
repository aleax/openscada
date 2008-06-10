
//OpenSCADA system file: tarchives.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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

#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>

#include "tsys.h"
#include "tarchives.h"

#define BUF_SIZE_DEF 500
#define BUF_SIZE_MAX 100000

//************************************************
//* Archive subsystem                            *
//************************************************

//************************************************
//* TArchiveS                                    *
//************************************************
int TArchiveS::max_req_mess = 3000;

TArchiveS::TArchiveS( ) :
    TSubSYS("Archive","Archives",true), prc_st_mess(false), prc_st_val(false), endrun_req_val(false), m_mess_per(2), 
    m_val_per(1000), m_val_prior(10), head_buf(0), head_lstread(0), el_mess(""), el_val(""), el_aval(""), buf_err(0)
{
    m_aval = grpAdd("va_");

    //- Message archivator DB structure -
    el_mess.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_mess.fldAdd( new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key,"20") );
    el_mess.fldAdd( new TFld("NAME",_("Name"),TFld::String,0,"50") );
    el_mess.fldAdd( new TFld("DESCR",_("Description"),TFld::String,0,"200") );
    el_mess.fldAdd( new TFld("START",_("Start archive"),TFld::Boolean,0,"1") );
    el_mess.fldAdd( new TFld("CATEG",_("Message categories"),TFld::String,0,"100") );
    el_mess.fldAdd( new TFld("LEVEL",_("Message level"),TFld::Integer,0,"1","","0;7") );
    el_mess.fldAdd( new TFld("ADDR",_("Address"),TFld::String,0,"100") );

    //- Value archivator DB structure -
    el_val.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_val.fldAdd( new TFld("MODUL",_("Module(plugin) name"),TFld::String,TCfg::Key,"20") );
    el_val.fldAdd( new TFld("NAME",_("Name"),TFld::String,0,"50") );
    el_val.fldAdd( new TFld("DESCR",_("Description"),TFld::String,0,"200") );
    el_val.fldAdd( new TFld("START",_("Start archive"),TFld::Boolean,0,"1","0") );
    el_val.fldAdd( new TFld("ADDR",_("Address"),TFld::String,0,"50") );
    el_val.fldAdd( new TFld("V_PER",_("Value period (sec)"),TFld::Real,0,"8.2","1.0","0;1000000") );
    el_val.fldAdd( new TFld("A_PER",_("Period archiving (sec)"),TFld::Integer,0,"4","60","0;1000") );

    //- Value archive DB structure -
    el_aval.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_aval.fldAdd( new TFld("NAME",_("Name"),TFld::String,0,"50") );
    el_aval.fldAdd( new TFld("DESCR",_("Description"),TFld::String,0,"200") );
    el_aval.fldAdd( new TFld("START",_("Start archive"),TFld::Boolean,0,"1","0") );
    el_aval.fldAdd( new TFld("SrcMode",_("Source mode"),TFld::Integer,0,"1") );
    el_aval.fldAdd( new TFld("Source",_("Source"),TFld::String,0,"100") );
    el_aval.fldAdd( new TFld("VTYPE",_("Value type"),TFld::Integer,0,"1") );
    el_aval.fldAdd( new TFld("BPER",_("Buffer period (sec)"),TFld::Real,0,"5.2","1","0;10000") );
    el_aval.fldAdd( new TFld("BSIZE",_("Buffer size (items)"),TFld::Integer,0,"6","100","0;1000000") );
    el_aval.fldAdd( new TFld("BHGRD",_("Buffer hard time griding"),TFld::Boolean,0,"1","1") );
    el_aval.fldAdd( new TFld("BHRES",_("Buffer high time resolution"),TFld::Boolean,0,"1","0") );
    el_aval.fldAdd( new TFld("ArchS",_("Process into archivators"),TFld::String,0,"500") );

    setMessBufLen( BUF_SIZE_DEF );

    //- Create message archive timer -
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = ArhMessTask;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmIdMess);
}

TArchiveS::~TArchiveS(  )
{
    //- Stop messages timer -
    timer_delete(tmIdMess);
    //- Stop values archiving task -
    if( prc_st_val )
    {
	endrun_req_val = true;
	pthread_kill( m_val_pthr, SIGALRM );
	TSYS::eventWait(prc_st_val,false,nodePath()+"val_task_stop",5);
	pthread_join( m_val_pthr, NULL );
    }

    //- Free other resources -
    nodeDelAll();
}

int TArchiveS::valPeriod( )
{
    return vmax(1,m_val_per);
}

void TArchiveS::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    char *short_opt="h";
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //- Load parameters -
    setMessBufLen( atoi(TBDS::genDBGet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen())).c_str()) );
    setMessPeriod( atoi(TBDS::genDBGet(nodePath()+"MessPeriod",TSYS::int2str(m_mess_per)).c_str()) );
    setValPeriod( atoi(TBDS::genDBGet(nodePath()+"ValPeriod",TSYS::int2str(m_val_per)).c_str()) );
    setValPrior( atoi(TBDS::genDBGet(nodePath()+"ValPriority",TSYS::int2str(m_val_prior)).c_str()) );
    max_req_mess = atoi(TBDS::genDBGet(nodePath()+"MaxReqMess",TSYS::int2str(max_req_mess)).c_str());

    //- LidDB -
    //-- Message archivators load --
    string id,type;
    try
    {
	TConfig c_el(&el_mess);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//-- Search int DB and create new archivators --
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_mess_proc","",fld_cnt++,c_el) )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if( !at(type).at().messPresent(id) ) 
			at(type).at().messAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		c_el.cfg("ID").setS("");
		c_el.cfg("MODUL").setS("");
	    }
	}
	//-- Search int config file and create new archivators --
	int fld_cnt=0;
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_mess_proc",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("MODUL").getS();
	    if( !at(type).at().messPresent(id) ) 
		at(type).at().messAdd(id,"*.*");
	    c_el.cfg("ID").setS("");
	    c_el.cfg("MODUL").setS("");
	}
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Message archivators load error."));
    }

    //-- Value archivators load --
    try
    {
	TConfig c_el(&el_val);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//-- Search into DB and create new archivators --
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_val_proc","",fld_cnt++,c_el) )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("MODUL").getS();
		if( !at(type).at().valPresent(id) )
			at(type).at().valAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		c_el.cfg("ID").setS("");
		c_el.cfg("MODUL").setS("");
	    }
	}
	//-- Search into config file and create new archivators --
	int fld_cnt=0;
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_val_proc",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("MODUL").getS();
	    if( !at(type).at().valPresent(id) )
		at(type).at().valAdd(id,"*.*");
	    c_el.cfg("ID").setS("");
	    c_el.cfg("MODUL").setS("");
	}	
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str()); 
	mess_err(nodePath().c_str(),_("Value archivators load error."));
    }

    //-- Value archives load --
    try
    {
	TConfig c_el(&el_aval);
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//-- Search into DB and create new archives --
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek(db_ls[i_db]+"."+subId()+"_val","",fld_cnt++,c_el) )
	    {
	        id = c_el.cfg("ID").getS();
	        if( !valPresent(id) ) valAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	        c_el.cfg("ID").setS("");
	    }
	}
	//-- Search into config file and create new archives --
	int fld_cnt=0;
	while( SYS->db().at().dataSeek("",nodePath()+subId()+"_val",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    if( !valPresent(id) ) valAdd(id,"*.*");
	    c_el.cfg("ID").setS("");
	}
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Value archives load error."));
    }
}

void TArchiveS::save_( )
{
    vector<string> t_lst, o_lst;

    //- Save parameters -
    TBDS::genDBSet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen()));
    TBDS::genDBSet(nodePath()+"MessPeriod",TSYS::int2str(messPeriod()));
    TBDS::genDBSet(nodePath()+"ValPeriod",TSYS::int2str(valPeriod()));
    TBDS::genDBSet(nodePath()+"ValPriority",TSYS::int2str(valPrior()));
    TBDS::genDBSet(nodePath()+"MaxReqMess",TSYS::int2str(max_req_mess));
}

void TArchiveS::valAdd( const string &iid, const string &idb )
{
    if( valPresent(iid) ) return;
    chldAdd(m_aval,new TVArchive(iid,idb,&aValE()));
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

    vector<string> t_lst, o_lst;

    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);
	//- Message archivators start -
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
	//- Value archivators start -
	mod.at().valList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TVArchivator> val = mod.at().valAt(o_lst[i_o]);
	    if( !val.at().startStat() && val.at().toStart() )
		try{ val.at().start(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Value archivator <%s> start error."),o_lst[i_o].c_str());
		}
	}
    }

    //- Value archives start -
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

    //- Start messages interval timer -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = messPeriod();
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
    //- Start values acquisition task -
    if( !prc_st_val )
    {
	pthread_attr_t pthr_attr;
	pthread_attr_init(&pthr_attr);
	struct sched_param prior;
	if( valPrior() && SYS->user() == "root" )
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_RR);
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	prior.__sched_priority = valPrior();
	pthread_attr_setschedparam(&pthr_attr,&prior);

	pthread_create(&m_val_pthr,&pthr_attr,TArchiveS::ArhValTask,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait(prc_st_val, true, nodePath()+"val_task_start",5) )
	    throw TError(nodePath().c_str(),_("Values acquisition task no started!"));
    }

    TSubSYS::subStart( );
}

void TArchiveS::subStop( )
{
    mess_info(nodePath().c_str(),_("Stop subsystem."));

    TSubSYS::subStop( );

    vector<string> t_lst, o_lst;

    //- Stop interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st_mess, false, nodePath()+"mess_stop",5) )
	throw TError(nodePath().c_str(),_("Archive messages thread no stoped!"));

    //- Values acquisition task stop -
    if( prc_st_val )
    {
	endrun_req_val = true;
	pthread_kill( m_val_pthr, SIGALRM );
	if( TSYS::eventWait(prc_st_val,false,nodePath()+"val_task_stop",5) )
	    throw TError(nodePath().c_str(),_("Archive values task no stoped!"));
	pthread_join( m_val_pthr, NULL );
    }

    //- Value archives stop -
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

    //- Archivators stop -
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
}

void TArchiveS::messPut( time_t tm, const string &categ, TMess::Type level, const string &mess )
{
    //- Put message to buffer -
    ResAlloc res(m_res,true);
    m_buf[head_buf].time  = tm;
    m_buf[head_buf].categ = categ;
    m_buf[head_buf].level = level;
    m_buf[head_buf].mess  = mess;
    if( ++head_buf >= m_buf.size() ) head_buf = 0;
    //- Check to no archivated messages -
    if( head_buf == head_lstread )
    {
	if( !(buf_err&0x01) )
	{
	    buf_err |= 0x01;
	    res.release();
	    mess_err(nodePath().c_str(),_("Buffer full. Messages lost!"));
	    res.request(true);
	}
	if( ++head_lstread >= m_buf.size() ) head_lstread = 0;
    }
    //- Check fill buffer speed. -
    else if( head_buf-head_lstread > messBufLen( )/2 )
    {
	if( !(buf_err&0x02) )
	{
	    buf_err |= 0x02;
	    res.release();
	    mess_warning(nodePath().c_str(),_("Messagess buffer fill too fast!"));
	    res.request(true);
	}
    }
    else buf_err = 0;
}

void TArchiveS::messPut( const vector<TMess::SRec> &recs )
{
    for( int i_r = 0; i_r < recs.size(); i_r++ )
	messPut(recs[i_r].time,recs[i_r].categ,recs[i_r].level,recs[i_r].mess);
}

void TArchiveS::messGet( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category, TMess::Type level, const string &arch )
{
    recs.clear();

    ResAlloc res(m_res,false);
    //- Get records from buffer -
    int i_buf = head_buf;
    while( !arch.size() || arch==BUF_ARCH_NM )
    {
	if( m_buf[i_buf].time >= b_tm && m_buf[i_buf].time != 0 && m_buf[i_buf].time < e_tm &&
		m_buf[i_buf].level >= level && TMess::chkPattern( m_buf[i_buf].categ, category ) )
	    recs.push_back(m_buf[i_buf]);
	if( ++i_buf >= m_buf.size() ) i_buf = 0;
	if(i_buf == head_buf) break;
    }

    //- Get records from archives -
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	at(t_lst[i_t]).at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TMArchivator> archtor = at(t_lst[i_t]).at().messAt(o_lst[i_o]);
	    if( archtor.at().startStat() && (!arch.size() || arch==archtor.at().workId()) )
		archtor.at().get(b_tm,e_tm,recs,category,level);
	}
    }
}

time_t TArchiveS::messBeg( const string &arch )
{
    time_t rez = 0;
    ResAlloc res(m_res,false);
    if( arch.empty() || arch == BUF_ARCH_NM )
    {
	int i_buf = head_buf;
	while( !arch.size() || arch==BUF_ARCH_NM )
	{
	    rez = rez ? vmin(rez,m_buf[i_buf].time) : m_buf[i_buf].time;
	    if( ++i_buf >= m_buf.size() ) i_buf = 0;
	    if(i_buf == head_buf) break;
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
    ResAlloc res(m_res,false);
    if( arch.empty() || arch == BUF_ARCH_NM )
    {
	int i_buf = head_buf;
	while( !arch.size() || arch==BUF_ARCH_NM )
	{
	    rez = rez ? vmax(rez,m_buf[i_buf].time) : m_buf[i_buf].time;
	    if( ++i_buf >= m_buf.size() ) i_buf = 0;
	    if(i_buf == head_buf) break;
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
		rez = rez ? vmax(rez,archtor.at().end()) : archtor.at().end();
	}
    }

    return rez;
}

void TArchiveS::setMessBufLen(int len)
{
    ResAlloc res(m_res,true);
    len=(len<BUF_SIZE_DEF)?BUF_SIZE_DEF:(len>BUF_SIZE_MAX)?BUF_SIZE_MAX:len;
    while( m_buf.size() > len )
    {
	m_buf.erase( m_buf.begin() + head_buf );
	if( head_buf >= m_buf.size() ) head_buf = 0;
	if( head_lstread >= m_buf.size() )	head_lstread=m_buf.size()-1;
    }
    while( m_buf.size() < len )
	m_buf.insert( m_buf.begin() + head_buf, TMess::SRec() );
    modif();
}

void TArchiveS::setActValArch( const string &id, bool val )
{
    unsigned i_arch;

    ResAlloc res(v_res,true);
    for( i_arch = 0; i_arch < act_up_src.size(); i_arch++)
    if( act_up_src[i_arch].at().id() == id ) break;

    if( val && i_arch >= act_up_src.size() )
	act_up_src.push_back(valAt(id));
    if( !val && i_arch < act_up_src.size() )
	act_up_src.erase(act_up_src.begin()+i_arch);
}

void TArchiveS::setMessPeriod( int ivl )
{
    m_mess_per = ivl;
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_mess_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
    modif();
}

void TArchiveS::ArhMessTask(union sigval obj)
{
    TArchiveS &arh = *(TArchiveS *)obj.sival_ptr;
    if( arh.prc_st_mess )  return;
    arh.prc_st_mess = true;

    //- Message bufer read -
    if( arh.head_lstread != arh.head_buf )
	try
	{
	    ResAlloc res(arh.m_res,false);
	    //-- Get new messages --
	    unsigned new_head_lstread = arh.head_buf;
	    unsigned i_m = arh.head_lstread;
	    vector<TMess::SRec> o_mess;
	    while(i_m != new_head_lstread)
	    {
		o_mess.push_back(arh.m_buf[i_m]);
		if( ++i_m >= arh.m_buf.size() ) i_m = 0;
	    }
	    arh.head_lstread = new_head_lstread;

	    res.release();

	    //-- Put to archivators --
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

    arh.prc_st_mess = false;
}

void *TArchiveS::ArhValTask( void *param )
{
    long long work_tm, last_tm = 0;
    struct timespec get_tm;

    TArchiveS &arh = *(TArchiveS *)param;
    arh.endrun_req_val = false;
    arh.prc_st_val = true;

    while(!arh.endrun_req_val)
    {
	work_tm = SYS->curTime();

	arh.v_res.resRequestR( );
	for(unsigned i_arh = 0; i_arh < arh.act_up_src.size(); i_arh++)
	    try
	    {
		if( work_tm/arh.act_up_src[i_arh].at().period() > arh.act_up_src[i_arh].at().end()/arh.act_up_src[i_arh].at().period() )
		    arh.act_up_src[i_arh].at().getActiveData();
	    }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	arh.v_res.resReleaseR( );

	//-- Calc next work time and sleep --
	clock_gettime(CLOCK_REALTIME,&get_tm);
	work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/(arh.valPeriod()*1000000) + 1)*arh.valPeriod()*1000000;
	if(last_tm == work_tm)	work_tm+=arh.valPeriod()*1000000;	//Fix early call
	last_tm = work_tm;
	get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
	clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);
    }

    arh.prc_st_val = false;

    return NULL;
}

void TArchiveS::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/0" )           //Messages access
    {
	if( ctrChkNode(opt,"info",RWRWRW,"root","root",SEQ_RD) )        //Messages information
	{
	    string arch = opt->attr("arch");
	    opt->setAttr("end",TSYS::uint2str(messEnd(arch)));
	    opt->setAttr("beg",TSYS::uint2str(messBeg(arch)));
	}
	else if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )    //Value's data request
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
		    setAttr("cat",rez[i_r].categ)->
		    setAttr("lev",TSYS::int2str(rez[i_r].level))->
		    setText(rez[i_r].mess);
	}
	return;
    }

    string my_gr = subId();
    //- Get page info -
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/va_",_("Value archive"),0664,"root",my_gr.c_str(),1,"idm","1");
	if(ctrMkNode("area",opt,1,"/m_arch",_("Messages archive"),0444,"root",my_gr.c_str()))
	{
	    ctrMkNode("fld",opt,-1,"/m_arch/max_am_req",_("Maximum requested messages"),0664,"root",my_gr.c_str(),1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/m_arch/size",_("Messages buffer size"),0664,"root",my_gr.c_str(),2,"tp","dec","min",TSYS::int2str(BUF_SIZE_DEF).c_str());
	    ctrMkNode("fld",opt,-1,"/m_arch/per",_("Archiving period (s)"),0664,"root",my_gr.c_str(),1,"tp","dec");
	    if(ctrMkNode("area",opt,-1,"/m_arch/view",_("View messages"),0444,"root",my_gr.c_str()))
	    {
		ctrMkNode("fld",opt,-1,"/m_arch/view/tm",_("Time"),0664,"root",my_gr.c_str(),1,"tp","time");
		ctrMkNode("fld",opt,-1,"/m_arch/view/size",_("Size"),0664,"root",my_gr.c_str(),1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/m_arch/view/cat",_("Category"),0664,"root",my_gr.c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/m_arch/view/lvl",_("Level"),0664,"root",my_gr.c_str(),3,"tp","dec","min","0","max","7");
		ctrMkNode("fld",opt,-1,"/m_arch/view/archtor",_("Archivator"),0664,"root",my_gr.c_str(),1,"tp","str");
		if(ctrMkNode("table",opt,-1,"/m_arch/view/mess",_("Messages"),0440,"root",my_gr.c_str()))
		{
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/0",_("Time"),0440,"root",my_gr.c_str(),1,"tp","time");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/1",_("Category"),0440,"root",my_gr.c_str(),1,"tp","str");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/2",_("Level"),0440,"root",my_gr.c_str(),1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/m_arch/view/mess/3",_("Message"),0440,"root",my_gr.c_str(),1,"tp","str"); 
		}
	    }
	}
	if(ctrMkNode("area",opt,2,"/v_arch",_("Value archives"),0444,"root",my_gr.c_str()))
	{
	    ctrMkNode("fld",opt,-1,"/v_arch/per",_("Get data period (ms)"),0664,"root",my_gr.c_str(),1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/v_arch/prior",_("Get data task priority level"),0664,"root",my_gr.c_str(),1,"tp","dec");
	    ctrMkNode("list",opt,-1,"/v_arch/archs",_("Value archives"),0664,"root",my_gr.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","va_");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root",my_gr.c_str(),3,"tp","str","cols","90","rows","10");
	return;
    }

    //- Process command to page -
    if( a_path == "/m_arch/max_am_req" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(max_req_mess));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	{ max_req_mess = atoi(opt->text().c_str()); modif(); }
    }
    else if( a_path == "/m_arch/per" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(messPeriod()));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	setMessPeriod(atoi(opt->text().c_str()));
    }
    else if( a_path == "/m_arch/size" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(messBufLen()));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	setMessBufLen(atoi(opt->text().c_str()));
    }
    else if( a_path == "/m_arch/view/tm" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )
	{
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if( !atoi(opt->text().c_str()) )    opt->setText(TSYS::int2str(time(NULL)));
	}
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )
	    TBDS::genDBSet(nodePath()+"messTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/size" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/cat" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/lvl" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/archtor" ) 
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messArch",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/mess" && ctrChkNode(opt,"get",0440) )
    {
	vector<TMess::SRec> rec;
	time_t gtm = atoi(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")).c_str());
	if( !gtm ) gtm = time(NULL);
	int    gsz = atoi(TBDS::genDBGet(nodePath()+"messSize","60",opt->attr("user")).c_str());
	messGet( gtm-gsz, gtm, rec,
		 TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")),
		 (TMess::Type)atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str()),
		 TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")) );

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/m_arch/view/mess/0","",0440);
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/m_arch/view/mess/1","",0440);
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/m_arch/view/mess/2","",0440);
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/m_arch/view/mess/3","",0440);
	for( int i_rec = rec.size()-1; i_rec >= 0; i_rec-- )
	{
	    if(n_tm)	n_tm->childAdd("el")->setText(TSYS::int2str(rec[i_rec].time));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else if( a_path == "/v_arch/per" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText( TSYS::int2str(valPeriod()) );
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	setValPeriod( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/v_arch/prior" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->setText( TSYS::int2str(valPrior()) );
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	setValPrior( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/br/va_" || a_path == "/v_arch/archs" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )
	{
	    vector<string> list;
	    valList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root",my_gr.c_str(),SEQ_WR) )
	{
	    valAdd(opt->attr("id"));
	    valAt(opt->attr("id")).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root",my_gr.c_str(),SEQ_WR) )	chldDel(m_aval,opt->attr("id"),-1,1);
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTipArchivator                               *
//************************************************
TTipArchivator::TTipArchivator()
{
    m_val = grpAdd("val_");
    m_mess = grpAdd("mess_");
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
    if( chldPresent(m_mess,name) ) return;
    chldAdd(m_mess,AMess(name,idb));
}

void TTipArchivator::valAdd( const string &iid, const string &idb )
{
    if( chldPresent(m_val,iid) ) return;
    chldAdd(m_val,AVal(iid,idb));
}

void TTipArchivator::cntrCmdProc( XMLNode *opt )
{
    string grp = owner().subId().c_str();
    //- Get page info -
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/arch",_("Archivators"));
	ctrMkNode("grp",opt,-1,"/br/mess_",_("Message archivator"),0664,"root",grp.c_str(),1,"idm","1");
	ctrMkNode("grp",opt,-1,"/br/val_",_("Value archivator"),0664,"root",grp.c_str(),1,"idm","1");
	ctrMkNode("list",opt,-1,"/arch/mess",_("Message archivators"),0664,"root",grp.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","mess_");
	ctrMkNode("list",opt,-1,"/arch/val",_("Value archivators"),0664,"root",grp.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","val_");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/br/mess_" || a_path == "/arch/mess" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )
	{
	    vector<string> list;
	    messList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(messAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root",grp.c_str(),SEQ_WR) )
	{
	    messAdd(opt->attr("id"));
	    messAt(opt->attr("id")).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root",grp.c_str(),SEQ_WR) )	messDel(opt->attr("id"),true);
    }
    else if( a_path == "/br/val_" || a_path == "/arch/val" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )
	{
	    vector<string> list;
	    valList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(valAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root",grp.c_str(),SEQ_WR) )
	{
	    valAdd(opt->attr("id"));
	    valAt(opt->attr("id")).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root",grp.c_str(),SEQ_WR) )	valDel(opt->attr("id"),true);
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

    //- Configuration copy -
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
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
	    SYS->db().at().dataDel(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TMArchivator::workId()
{
    return owner().modId()+"."+id();
}

string TMArchivator::name()
{
    return (m_name.size())?m_name:m_id;
}

string TMArchivator::tbl( )
{
    return owner().owner().subId()+"_mess_proc";
}

void TMArchivator::load_( )
{
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
    string grp = owner().owner().subId();
    //- Get page info -
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Message archivator: ")+name(),0664,"root",grp.c_str());
	if(ctrMkNode("area",opt,-1,"/prm",_("Archivator")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root",grp.c_str(),1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archivator DB"),0664,"root","root",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
		ctrMkNode("fld",opt,-1,"/prm/st/end",_("End"),0444,"root","root",1,"tp","time");
		ctrMkNode("fld",opt,-1,"/prm/st/beg",_("Begin"),0444,"root","root",1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root",grp.c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,"root",grp.c_str(),3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/lvl",cfg("LEVEL").fld().descr(),0664,"root",grp.c_str(),1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/prm/cfg/cats",cfg("CATEG").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",_("To start"),0664,"root",grp.c_str(),1,"tp","bool");
	    }
	}
	if( run_st && ctrMkNode("area",opt,-1,"/mess",_("Messages"),0440,"root",grp.c_str()) )
	{
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time"),0660,"root",grp.c_str(),1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size",_("Size"),0660,"root",grp.c_str(),1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mess/cat",_("Category pattern"),0660,"root",grp.c_str(),1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mess/lvl",_("Level"),0660,"root",grp.c_str(),3,"tp","dec","min","0","max","7");
	    ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),0440,"root",grp.c_str());
	    ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),0440,"root",grp.c_str(),1,"tp","time");
	    ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),0440,"root",grp.c_str(),1,"tp","str");
	    ctrMkNode("list",opt,-1,"/mess/mess/2",_("Level"),0440,"root",grp.c_str(),1,"tp","dec");
	    ctrMkNode("list",opt,-1,"/mess/mess/3",_("Message"),0440,"root",grp.c_str(),1,"tp","str");
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText( startStat() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	atoi(opt->text().c_str()) ? start() : stop();
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText( DB() );
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	setDB( opt->text() );
    }
    else if( a_path == "/prm/st/end" && ctrChkNode(opt) )		opt->setText( TSYS::int2str(end()) );
    else if( a_path == "/prm/st/beg" && ctrChkNode(opt) )		opt->setText( TSYS::int2str(begin()) );
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )		opt->setText( id() );
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText( name() );
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	setName( opt->text() );
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText( dscr() );
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	setDscr( opt->text() );
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText( addr() );
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	setAddr( opt->text() );
    }
    else if( a_path == "/prm/cfg/lvl" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(level()));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	setLevel( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/cats" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(m_cat_o);
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	{ m_cat_o = opt->text(); modif(); }
    }
    else if( a_path == "/mess/tm" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )
	{
	    opt->setText(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")));
	    if( !atoi(opt->text().c_str()) )    opt->setText(TSYS::int2str(time(NULL)));
	}
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )
	    TBDS::genDBSet(nodePath()+"messTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/size" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/cat" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/lvl" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/mess" && run_st && ctrChkNode(opt,"get",0440,"root",grp.c_str()) )
    {
	vector<TMess::SRec> rec;
	time_t end = atoi(TBDS::genDBGet(nodePath()+"messTm","0",opt->attr("user")).c_str());
	if( !end ) end = time(NULL);
	time_t beg = end - atoi(TBDS::genDBGet(nodePath()+"messSize","10",opt->attr("user")).c_str());
	string cat = TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user"));
	char   lev = atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str());

	get( beg, end, rec, cat, lev );

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/mess/mess/0","",0440,"root",grp.c_str());
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/mess/mess/1","",0440,"root",grp.c_str());
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/mess/mess/2","",0440,"root",grp.c_str());
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/mess/mess/3","",0440,"root",grp.c_str());
	for( int i_rec = rec.size()-1; i_rec >= 0; i_rec-- )
	{
	    if(n_tm)	n_tm->childAdd("el")->setText(TSYS::int2str(rec[i_rec].time));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

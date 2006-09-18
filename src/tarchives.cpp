
//OpenSCADA system file: tarchives.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>

#include "resalloc.h"
#include "tsys.h"
#include "tarchives.h"

#define BUF_SIZE_DEF 500
#define BUF_SIZE_MAX 100000

//================================================================
//=================== Archive subsystem ==========================
//================================================================ 

//=============== TArchiveS =======================================
int TArchiveS::max_req_mess = 3000;
int TArchiveS::max_req_vals = 100000;

TArchiveS::TArchiveS( ) :
    TSubSYS("Archive","Archives",true), prc_st_mess(false), prc_st_val(false), endrun_req_val(false), m_mess_per(2), 
    m_val_per(1000), m_val_prior(10), head_buf(0), head_lstread(0), el_mess(""), el_val(""), el_aval(""), buf_err(0)
{
    v_res = ResAlloc::resCreate( );
    m_res = ResAlloc::resCreate( );
    
    m_aval = grpAdd("va_");
    
    //Message archivator DB structure
    el_mess.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_mess.fldAdd( new TFld("MODUL",Mess->I18N("Module(plugin) name"),TFld::String,FLD_KEY,"20") );
    el_mess.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
    el_mess.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"200") );
    el_mess.fldAdd( new TFld("START",Mess->I18N("Start archive"),TFld::Bool,0,"1") );
    el_mess.fldAdd( new TFld("CATEG",Mess->I18N("Message categories"),TFld::String,0,"100") );
    el_mess.fldAdd( new TFld("LEVEL",Mess->I18N("Message level"),TFld::Dec,0,"1","","0;7") );
    el_mess.fldAdd( new TFld("ADDR",Mess->I18N("Address"),TFld::String,0,"100") );

    //Value archivator DB structure    
    el_val.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_val.fldAdd( new TFld("MODUL",Mess->I18N("Module(plugin) name"),TFld::String,FLD_KEY,"20") );
    el_val.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
    el_val.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"200") );
    el_val.fldAdd( new TFld("START",Mess->I18N("Start archive"),TFld::Bool,0,"1","false") );
    el_val.fldAdd( new TFld("ADDR",Mess->I18N("Address"),TFld::String,0,"50") );
    el_val.fldAdd( new TFld("V_PER",Mess->I18N("Value period (sec)"),TFld::Real,0,"0","1.0","0;1000000") );
    el_val.fldAdd( new TFld("A_PER",Mess->I18N("Period archiving (sec)"),TFld::Dec,0,"0","60","0;1000") );

    //Value archive DB structure
    el_aval.fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY,"20") );
    el_aval.fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
    el_aval.fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"200") );    
    el_aval.fldAdd( new TFld("START",Mess->I18N("Start archive"),TFld::Bool,0,"1","false") );
    el_aval.fldAdd( new TFld("SrcMode",Mess->I18N("Source mode"),TFld::Dec,0,"1") );
    el_aval.fldAdd( new TFld("Source",Mess->I18N("Source"),TFld::String,0,"100") );
    el_aval.fldAdd( new TFld("VTYPE",Mess->I18N("Value type"),TFld::Dec,0,"1") );
    el_aval.fldAdd( new TFld("BPER",Mess->I18N("Buffer period (sec)"),TFld::Real,0,"0","1","0;10000") );
    el_aval.fldAdd( new TFld("BSIZE",Mess->I18N("Buffer size (items)"),TFld::Dec,0,"6","100","0;1000000") );
    el_aval.fldAdd( new TFld("BHGRD",Mess->I18N("Buffer hard time griding"),TFld::Bool,0,"1","true") );
    el_aval.fldAdd( new TFld("BHRES",Mess->I18N("Buffer high time resolution"),TFld::Bool,0,"1","false") );
    el_aval.fldAdd( new TFld("ArchS",Mess->I18N("Process into archivators"),TFld::String,0,"500") );
    
    messBufLen( BUF_SIZE_DEF );
    
    //Create message archive timer
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = ArhMessTask;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmIdMess);
}

TArchiveS::~TArchiveS(  )
{
    //Stop messages timer
    timer_delete(tmIdMess);
    //Stop values archiving task
    if( prc_st_val ) 
    { 
	endrun_req_val = true;
	pthread_kill( m_val_pthr, SIGALRM );
        TSYS::eventWait(prc_st_val,false,nodePath()+"val_task_stop",5);
        pthread_join( m_val_pthr, NULL );
    }     
    
    //Free other resources
    nodeDelAll();
    
    ResAlloc::resDelete( v_res );
    ResAlloc::resDelete( m_res );
}

void TArchiveS::subLoad( )
{
    //---- Load parameters from command line ----
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

    //---- Load parameters ----
    messBufLen( atoi(TBDS::genDBGet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen())).c_str()) );
    m_mess_per = atoi(TBDS::genDBGet(nodePath()+"MessPeriod",TSYS::int2str(m_mess_per)).c_str());
    m_val_per = atoi(TBDS::genDBGet(nodePath()+"ValPeriod",TSYS::int2str(m_val_per)).c_str());
    m_val_prior = atoi(TBDS::genDBGet(nodePath()+"ValPriority",TSYS::int2str(m_val_prior)).c_str());
    max_req_mess = atoi(TBDS::genDBGet(nodePath()+"MaxReqMess",TSYS::int2str(max_req_mess)).c_str());
    max_req_vals = atoi(TBDS::genDBGet(nodePath()+"MaxReqVals",TSYS::int2str(max_req_vals)).c_str());

    //---- LidDB ----
    //Message archivators load
    string id,type;
    try
    {
	TConfig c_el(&el_mess);
        vector<string> tdb_ls, db_ls;
	
	//Search int DB and create new archivators
	SYS->db().at().modList(tdb_ls);
	for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		int fld_cnt=0;
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_mess_proc","",fld_cnt++,c_el) )
		{
		    id = c_el.cfg("ID").getS();
		    type = c_el.cfg("MODUL").getS();
		    if( !at(type).at().messPresent(id) ) 
			at(type).at().messAdd(id,(wbd==SYS->workDB())?"*.*":wbd);
		    c_el.cfg("ID").setS("");
		    c_el.cfg("MODUL").setS("");
		}
	    }
	}
	//Search int config file and create new archivators
	int fld_cnt=0;
	while( SYS->db().at().dataSeek("",nodePath()+"MessProc/",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("MODUL").getS();
	    if( !at(type).at().messPresent(id) ) 
		at(type).at().messAdd(id,"*.*");
	    c_el.cfg("ID").setS("");
	    c_el.cfg("MODUL").setS("");
	}
	
	//Load present archivators
	modList(tdb_ls);
	for( int i_m = 0; i_m < tdb_ls.size(); i_m++ )
	{
	    at(tdb_ls[i_m]).at().messList(db_ls);
	    for( int i_a = 0; i_a < db_ls.size(); i_a++ ) 
		at(tdb_ls[i_m]).at().messAt(db_ls[i_a]).at().load();
	}    	    
    }catch( TError err )
    { 
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Message archivators load error."));
    }

    //Value archivators load
    try
    {
	TConfig c_el(&el_val);
	vector<string> tdb_ls, db_ls;
	
	//Search into DB and create new archivators
	SYS->db().at().modList(tdb_ls);
	for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		int fld_cnt=0; 	
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_val_proc","",fld_cnt++,c_el) )
		{
		    id = c_el.cfg("ID").getS();
		    type = c_el.cfg("MODUL").getS();
		    if( !at(type).at().valPresent(id) )
			at(type).at().valAdd(id,(wbd==SYS->workDB())?"*.*":wbd);
		    c_el.cfg("ID").setS("");
		    c_el.cfg("MODUL").setS("");
		}
	    }
	}
	//Search into config file and create new archivators
	int fld_cnt=0; 	
	while( SYS->db().at().dataSeek("",nodePath()+"ValProc/",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("MODUL").getS();
	    if( !at(type).at().valPresent(id) )
		at(type).at().valAdd(id,"*.*");
	    c_el.cfg("ID").setS("");
	    c_el.cfg("MODUL").setS("");
	}
	
	
	//Load present archivators
	modList(tdb_ls);
	for( int i_m = 0; i_m < tdb_ls.size(); i_m++ )
	{
	    at(tdb_ls[i_m]).at().valList(db_ls);
	    for( int i_a = 0; i_a < db_ls.size(); i_a++ ) 
		at(tdb_ls[i_m]).at().valAt(db_ls[i_a]).at().load();
	}	
    }catch( TError err )
    { 
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Value archivators load error."));
    }
    
    //Value archives load
    try
    {
	TConfig c_el(&el_aval);
 	vector<string> tdb_ls, db_ls;
	
	//Search into DB and create new archives
	SYS->db().at().modList(tdb_ls);
	for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
	{
	    SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	    for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    {
		string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		int fld_cnt=0;
		while( SYS->db().at().dataSeek(wbd+"."+subId()+"_val","",fld_cnt++,c_el) )
		{
		    id = c_el.cfg("ID").getS();		    
		    if( !valPresent(id) ) valAdd(id,(wbd==SYS->workDB())?"*.*":wbd);	    
		    c_el.cfg("ID").setS("");
		}
	    }
	}
	//Search into config file and create new archives
	int fld_cnt=0;
	while( SYS->db().at().dataSeek("",nodePath()+"ValArchive/",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();		    
	    if( !valPresent(id) ) valAdd(id,"*.*");	    
	    c_el.cfg("ID").setS("");
	}	
		
	//Load present archives
	valList(tdb_ls);
	for( int i_a = 0; i_a < tdb_ls.size(); i_a++ )
	    valAt(tdb_ls[i_a]).at().load();
    }catch( TError err )
    { 
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Value archives load error."));
    }
    
    //---- Load modules ----
    TSubSYS::subLoad( );
}

void TArchiveS::subSave( )
{
    vector<string> t_lst, o_lst;

    //========== Save parameters =============
    TBDS::genDBSet(nodePath()+"MessBufSize",TSYS::int2str(messBufLen()));
    TBDS::genDBSet(nodePath()+"MessPeriod",TSYS::int2str(m_mess_per));
    TBDS::genDBSet(nodePath()+"ValPeriod",TSYS::int2str(m_val_per));
    TBDS::genDBSet(nodePath()+"ValPriority",TSYS::int2str(m_val_prior));
    TBDS::genDBSet(nodePath()+"MaxReqMess",TSYS::int2str(max_req_mess));
    TBDS::genDBSet(nodePath()+"MaxReqVals",TSYS::int2str(max_req_vals));

    //Value archives save to DB
    valList(o_lst);
    for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	valAt(o_lst[i_o]).at().save();
    
    //Messages and values archivators save to DB
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);
	//Messages save
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().messAt(o_lst[i_o]).at().save();
	//Values save
	mod.at().valList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().valAt(o_lst[i_o]).at().save();
    }
}

void TArchiveS::valAdd( const string &iid, const string &idb )
{
    if( valPresent(iid) ) return;
    chldAdd(m_aval,new TVArchive(iid,idb,&aValE()));
}

string TArchiveS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
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
    vector<string> t_lst, o_lst;

    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchivator> mod = modAt(t_lst[i_t]);
	//Message archivators start
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TMArchivator> mess = mod.at().messAt(o_lst[i_o]);
	    if( !mess.at().startStat() && mess.at().toStart() )
		try{ mess.at().start(); }
		catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Message archivator <%s> start error."),o_lst[i_o].c_str());
		}
	}
	//Value archivators start
	mod.at().valList(o_lst);
        for( int i_o = 0; i_o < o_lst.size(); i_o++ )
        {
            AutoHD<TVArchivator> val = mod.at().valAt(o_lst[i_o]);
            if( !val.at().startStat() && val.at().toStart() )
                try{ val.at().start(); }
		catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Value archivator <%s> start error."),o_lst[i_o].c_str());
		}
        }
    }    
    
    //Value archives start
    valList(o_lst);
    for( int i_o = 0; i_o < o_lst.size(); i_o++ )
    {
	AutoHD<TVArchive> aval = valAt(o_lst[i_o]);
	if( !aval.at().startStat() && aval.at().toStart() )
	    try{ aval.at().start(); }
	    catch(TError err) 
	    { 
		Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
		Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Value archive <%s> start error."),o_lst[i_o].c_str());
	    }
    }    
        
    //Start messages interval timer
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_mess_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
    //Start values acquisition task
    if( !prc_st_val )
    {
	pthread_attr_t pthr_attr; 
	pthread_attr_init(&pthr_attr); 
	struct sched_param prior;
	if( m_val_prior && SYS->user() == "root" )
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_RR);
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	prior.__sched_priority=m_val_prior;
	pthread_attr_setschedparam(&pthr_attr,&prior);
	
        pthread_create(&m_val_pthr,&pthr_attr,TArchiveS::ArhValTask,this);
        pthread_attr_destroy(&pthr_attr); 
        if( TSYS::eventWait(prc_st_val, true, nodePath()+"val_task_start",5) ) 
    	    throw TError(nodePath().c_str(),Mess->I18N("Values acquisition task no started!"));
    }    

    TSubSYS::subStart( );
}

void TArchiveS::subStop( )
{
    vector<string> t_lst, o_lst;
    
    //Value archives stop
    valList(o_lst);
    for( int i_o = 0; i_o < o_lst.size(); i_o++ )
    {
        AutoHD<TVArchive> aval = valAt(o_lst[i_o]);
	if( aval.at().startStat() )	
	    try{ aval.at().stop(); }
	    catch(TError err) 
	    { 
		Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Value archive <%s> stop error."),o_lst[i_o].c_str());
	    }
    }    
    
    //Archivators stop
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
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Value archivator <%s> stop error."),o_lst[i_o].c_str());
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
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Message archivator <%s> stop error."),o_lst[i_o].c_str());
		}
	}
    }

    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st_mess, false, nodePath()+"mess_stop",5) )
        throw TError(nodePath().c_str(),Mess->I18N("Archive messages thread no stoped!"));
    //Values acquisition task stop
    if( prc_st_val ) 
    { 
	endrun_req_val = true; 
	pthread_kill( m_val_pthr, SIGALRM );
        if( TSYS::eventWait(prc_st_val,false,nodePath()+"val_task_stop",5) )
	    throw TError(nodePath().c_str(),Mess->I18N("Archive values task no stoped!"));
        pthread_join( m_val_pthr, NULL );
    }
	
    TSubSYS::subStop( );	
}
 
void TArchiveS::messPut( time_t tm, const string &categ, TMess::Type level, const string &mess )
{
    //Put message to buffer
    ResAlloc res(m_res,true);
    m_buf[head_buf].time  = tm;
    m_buf[head_buf].categ = categ;
    m_buf[head_buf].level = level;
    m_buf[head_buf].mess  = mess;
    if( ++head_buf >= m_buf.size() ) head_buf = 0;
    //Check to no archivated messages
    if( head_buf == head_lstread )
    {
	if( !(buf_err&0x01) )
	{
	    buf_err |= 0x01;
	    res.release();	    
	    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Buffer full. Messages lost!"));
	    res.request(true);
	}
	if( ++head_lstread >= m_buf.size() ) head_lstread = 0;
    }
    //Check fill buffer speed. 	
    else if( head_buf-head_lstread > messBufLen( )/2 )
    {
	if( !(buf_err&0x02) )
	{
	    buf_err |= 0x02;
	    res.release();	    
	    Mess->put(nodePath().c_str(),TMess::Warning,Mess->I18N("Messagess buffer fill too fast!"));
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
    //Get records from buffer
    int i_buf = head_buf;
    while( !arch.size() || arch==BUF_ARCH_NM )
    {
	if( m_buf[i_buf].time >= b_tm && m_buf[i_buf].time != 0 && m_buf[i_buf].time < e_tm &&
		m_buf[i_buf].level >= level && TMess::chkPattern( m_buf[i_buf].categ, category ) )
	    recs.push_back(m_buf[i_buf]);
	if( ++i_buf >= m_buf.size() ) i_buf = 0;
    	if(i_buf == head_buf) break;	    
    }
    
    //Get records from archives
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
 
void TArchiveS::messBufLen(int len)
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

void TArchiveS::messPeriod( int ivl )
{
    m_mess_per = ivl;
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_mess_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmIdMess, 0, &itval, NULL);
}

void TArchiveS::ArhMessTask(union sigval obj)
{
    TArchiveS &arh = *(TArchiveS *)obj.sival_ptr;
    if( arh.prc_st_mess )  return;
    arh.prc_st_mess = true;
    
    //Message bufer read
    if( arh.head_lstread != arh.head_buf )
	try
	{
	    ResAlloc res(arh.m_res,false);
	    //Get new messages
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
	
	    //Put to archivators
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
	    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	    Mess->put(arh.nodePath().c_str(),TMess::Error,Mess->I18N("Message bufer read error.")); 
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
	
	ResAlloc::resRequestR(arh.v_res);
	for(unsigned i_arh = 0; i_arh < arh.act_up_src.size(); i_arh++)
    	    try
	    { 
		if( work_tm/arh.act_up_src[i_arh].at().period() > arh.act_up_src[i_arh].at().end()/arh.act_up_src[i_arh].at().period() )
		    arh.act_up_src[i_arh].at().getActiveData();
	    }
    	    catch(TError err)
    	    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
	ResAlloc::resReleaseR(arh.v_res);	
    
	//Calc next work time and sleep
	clock_gettime(CLOCK_REALTIME,&get_tm);
	work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/(arh.m_val_per*1000000) + 1)*arh.m_val_per*1000000;
	if(last_tm == work_tm)	work_tm+=arh.m_val_per*1000000;	//Fix early call
	last_tm = work_tm;
	get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
	clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);
    }
    
    arh.prc_st_val = false;
    
    return NULL;
}

void TArchiveS::cntrCmdProc( XMLNode *opt )
{
    string my_gr = subId();
    //Get page info
    if( opt->name() == "info" )
    {
        TSubSYS::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/sub",Mess->I18N("Subsystem"),0444,"root",my_gr.c_str());
	ctrMkNode("grp",opt,-1,"/br/va_",Mess->I18N("Value archive"),0444,"root","root",1,"list","/v_arch/archs");
	ctrMkNode("fld",opt,-1,"/sub/max_am_req",Mess->I18N("Maximum requested messages"),0664,"root",my_gr.c_str(),1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/sub/max_av_req",Mess->I18N("Maximum requested values"),0664,"root",my_gr.c_str(),1,"tp","dec");
	ctrMkNode("comm",opt,-1,"/sub/load_bd",Mess->I18N("Load"),0440,"root",my_gr.c_str());
	ctrMkNode("comm",opt,-1,"/sub/upd_bd",Mess->I18N("Save"),0440,"root",my_gr.c_str());
	ctrMkNode("area",opt,1,"/m_arch",Mess->I18N("Messages archive"),0444,"root",my_gr.c_str());
	ctrMkNode("fld",opt,-1,"/m_arch/size",Mess->I18N("Messages buffer size"),0664,"root",my_gr.c_str(),2,"tp","dec","min",TSYS::int2str(BUF_SIZE_DEF).c_str());
	ctrMkNode("fld",opt,-1,"/m_arch/per",Mess->I18N("Archiving period (s)"),0664,"root",my_gr.c_str(),1,"tp","dec");
	ctrMkNode("area",opt,-1,"/m_arch/view",Mess->I18N("View messages"),0444,"root",my_gr.c_str());
	ctrMkNode("fld",opt,-1,"/m_arch/view/beg",Mess->I18N("Begin"),0664,"root",my_gr.c_str(),1,"tp","time");
	ctrMkNode("fld",opt,-1,"/m_arch/view/end",Mess->I18N("End"),0664,"root",my_gr.c_str(),1,"tp","time");
	ctrMkNode("fld",opt,-1,"/m_arch/view/cat",Mess->I18N("Category"),0664,"root",my_gr.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/m_arch/view/lvl",Mess->I18N("Level"),0664,"root",my_gr.c_str(),3,"tp","dec","min","0","max","7");
	ctrMkNode("fld",opt,-1,"/m_arch/view/archtor",Mess->I18N("Archivator"),0664,"root",my_gr.c_str(),1,"tp","str");    
	ctrMkNode("table",opt,-1,"/m_arch/view/mess",Mess->I18N("Messages"),0440,"root",my_gr.c_str());
	ctrMkNode("list",opt,-1,"/m_arch/view/mess/0",Mess->I18N("Time"),0440,"root",my_gr.c_str(),1,"tp","time");
	ctrMkNode("list",opt,-1,"/m_arch/view/mess/1",Mess->I18N("Category"),0440,"root",my_gr.c_str(),1,"tp","str");
	ctrMkNode("list",opt,-1,"/m_arch/view/mess/2",Mess->I18N("Level"),0440,"root",my_gr.c_str(),1,"tp","dec");
	ctrMkNode("list",opt,-1,"/m_arch/view/mess/3",Mess->I18N("Message"),0440,"root",my_gr.c_str(),1,"tp","str"); 
	ctrMkNode("area",opt,2,"/v_arch",Mess->I18N("Value archives"),0444,"root",my_gr.c_str());
	ctrMkNode("fld",opt,-1,"/v_arch/per",Mess->I18N("Get data period (ms)"),0664,"root",my_gr.c_str(),1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/v_arch/prior",Mess->I18N("Get data task priority level"),0664,"root",my_gr.c_str(),1,"tp","dec");
	ctrMkNode("list",opt,-1,"/v_arch/archs",Mess->I18N("Value archives"),0664,"root",my_gr.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","va_");
	ctrMkNode("fld",opt,-1,"/help/g_help",Mess->I18N("Options help"),0440,"root",my_gr.c_str(),3,"tp","str","cols","90","rows","10");	
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/sub/max_am_req" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(max_req_mess));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	max_req_mess = atoi(opt->text().c_str());
    }
    else if( a_path == "/sub/max_av_req" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(max_req_vals));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	max_req_vals = atoi(opt->text().c_str());
    }
    else if( a_path == "/m_arch/per" )
    {	
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(m_mess_per));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	messPeriod(atoi(opt->text().c_str()));
    }	
    else if( a_path == "/m_arch/size" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(messBufLen()));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	messBufLen(atoi(opt->text().c_str()));
    }	
    else if( a_path == "/m_arch/view/beg" )	
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messBeg","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messBeg",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/end" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messEnd","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messEnd",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/cat" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/lvl" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/archtor" ) 
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messArch",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/m_arch/view/mess" && ctrChkNode(opt,"get",0440) )
    {
	vector<TMess::SRec> rec;
	messGet( atoi(TBDS::genDBGet(nodePath()+"messBeg","0",opt->attr("user")).c_str()), 
		 atoi(TBDS::genDBGet(nodePath()+"messEnd","0",opt->attr("user")).c_str()), rec, 
		 TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")), 
		 (TMess::Type)atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str()), 
		 TBDS::genDBGet(nodePath()+"messArch","",opt->attr("user")) );
	    
	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/m_arch/view/mess/0","",0440);
        XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/m_arch/view/mess/1","",0440);
        XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/m_arch/view/mess/2","",0440);
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/m_arch/view/mess/3","",0440);
	for( int i_rec = 0; i_rec < rec.size(); i_rec++)
	{
	    if(n_tm)	n_tm->childAdd("el")->text(TSYS::int2str(rec[i_rec].time));
	    if(n_cat)	n_cat->childAdd("el")->text(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->text(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->text(rec[i_rec].mess);
	}        
    }
    else if( a_path == "/v_arch/per" )	
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(m_val_per));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	m_val_per = atoi(opt->text().c_str());
    }
    else if( a_path == "/v_arch/prior" )	
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(m_val_prior));
	if( ctrChkNode(opt,"set",0664,"root",my_gr.c_str(),SEQ_WR) )	m_val_prior = atoi(opt->text().c_str());
    }
    else if( a_path == "/v_arch/archs" )
    {
	if( ctrChkNode(opt,"get",0664,"root",my_gr.c_str(),SEQ_RD) )
	{
	    vector<string> list;
            valList(list);
            for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->attr("id",list[i_a])->text(valAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root",my_gr.c_str(),SEQ_WR) )
	{
	    valAdd(opt->attr("id"));
	    valAt(opt->attr("id")).at().name(opt->text());
	}
        if( ctrChkNode(opt,"del",0664,"root",my_gr.c_str(),SEQ_WR) )	chldDel(m_aval,opt->attr("id"),-1,1);
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->text(optDescr());
    else if( a_path == "/sub/load_bd" && ctrChkNode(opt,"set",0440) )	subLoad();
    else if( a_path == "/sub/upd_bd" && ctrChkNode(opt,"set",0440) )   	subSave();
    else TSubSYS::cntrCmdProc(opt);
}

//=============== TTipArchivator =================================
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
    //Get page info
    if( opt->name() == "info" )
    {
        TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/arch",Mess->I18N("Archivators"));
	ctrMkNode("grp",opt,-1,"/br/mess_",Mess->I18N("Message archivator"),0444,"root","root",1,"list","/arch/mess");
	ctrMkNode("grp",opt,-1,"/br/val_",Mess->I18N("Value archivator"),0444,"root","root",1,"list","/arch/val");
	ctrMkNode("list",opt,-1,"/arch/mess",Mess->I18N("Message archivators"),0664,"root",grp.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","mess_");
	ctrMkNode("list",opt,-1,"/arch/val",Mess->I18N("Value archivators"),0664,"root",grp.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","val_");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/arch/mess" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )
	{
	    vector<string> list;
	    messList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->attr("id",list[i_a])->text(messAt(list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root",grp.c_str(),SEQ_WR) )		
	{
	    messAdd(opt->attr("id"));
	    messAt(opt->attr("id")).at().name(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root",grp.c_str(),SEQ_WR) )	 messDel(opt->attr("id"),true);
    }
    else if( a_path == "/arch/val" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )
	{
	    vector<string> list;
	    valList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->attr("id",list[i_a])->text(valAt(list[i_a]).at().name());
	}
        if( ctrChkNode(opt,"add",0664,"root",grp.c_str(),SEQ_WR) )		
	{
	    valAdd(opt->attr("id"));
	    valAt(opt->attr("id")).at().name(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root",grp.c_str(),SEQ_WR) )	valDel(opt->attr("id"),true);    
    }
    else TModule::cntrCmdProc(opt);
}
			
//================================================================
//===================== Message archivator =======================
//================================================================

//====================== TMArchivator ============================
TMArchivator::TMArchivator(const string &iid, const string &idb, TElem *cf_el) :
    TConfig( cf_el ), m_bd(idb), run_st(false),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_dscr(cfg("DESCR").getSd()), m_addr(cfg("ADDR").getSd()),
    m_cat_o(cfg("CATEG").getSd()), m_level(cfg("LEVEL").getId()) ,m_start(cfg("START").getBd())
{    
    m_id = iid;
}

void TMArchivator::postEnable( )
{
    cfg("MODUL").setS(owner().modId());
}

void TMArchivator::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(BD(),SYS->archive().at().nodePath()+"MessProc/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,"%s",err.mess.c_str()); }
}

string TMArchivator::workId()
{
    return owner().modId()+".mess_"+id();
}

string TMArchivator::name()   
{ 
    return (m_name.size())?m_name:m_id;
}

string TMArchivator::BD()
{
    return m_bd+"."+owner().owner().subId()+"_mess_proc";
}

void TMArchivator::load( )
{
    SYS->db().at().dataGet(BD(),SYS->archive().at().nodePath()+"MessProc/",*this);
}

void TMArchivator::save( )
{
    SYS->db().at().dataSet(BD(),SYS->archive().at().nodePath()+"MessProc/",*this);
}

void TMArchivator::categ( vector<string> &list )
{
    list.clear();
    int i_lv = 0;
    while(TSYS::strSepParse(m_cat_o,i_lv,';').size())
    {
	list.push_back(TSYS::strSepParse(m_cat_o,i_lv,';'));
	i_lv++;
    }
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
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",Mess->I18N("Message archivator: ")+name());
	ctrMkNode("area",opt,-1,"/prm",Mess->I18N("Archivator"));
	ctrMkNode("area",opt,-1,"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,"/prm/st/st",Mess->I18N("Runing"),0664,"root",grp.c_str(),1,"tp","bool");
	ctrMkNode("fld",opt,-1,"/prm/st/bd",Mess->I18N("Archivator DB (module.db)"),0660,"root","root",1,"tp","str");
	ctrMkNode("area",opt,-1,"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root",grp.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,"root",grp.c_str(),3,"tp","str","cols","50","rows","3");
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/lvl",cfg("LEVEL").fld().descr(),0664,"root",grp.c_str(),1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/prm/cfg/cats",cfg("CATEG").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/start",Mess->I18N("To start"),0664,"root",grp.c_str(),1,"tp","bool");	
	ctrMkNode("comm",opt,-1,"/prm/cfg/load",Mess->I18N("Load"),0440,"root",grp.c_str());
	ctrMkNode("comm",opt,-1,"/prm/cfg/save",Mess->I18N("Save"),0440,"root",grp.c_str());
	if( run_st )
	{
	    ctrMkNode("area",opt,-1,"/mess",Mess->I18N("Messages"),0440,"root",grp.c_str());
	    ctrMkNode("fld",opt,-1,"/mess/v_beg",Mess->I18N("Begin"),0660,"root",grp.c_str(),1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/v_end",Mess->I18N("End"),0660,"root",grp.c_str(),1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/v_cat",Mess->I18N("Category pattern"),0660,"root",grp.c_str(),1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mess/v_lvl",Mess->I18N("Level"),0660,"root",grp.c_str(),3,"tp","dec","min","0","max","7");
	    ctrMkNode("table",opt,-1,"/mess/mess",Mess->I18N("Messages"),0440,"root",grp.c_str());
	    ctrMkNode("list",opt,-1,"/mess/mess/0",Mess->I18N("Time"),0440,"root",grp.c_str(),1,"tp","time");
	    ctrMkNode("list",opt,-1,"/mess/mess/1",Mess->I18N("Category"),0440,"root",grp.c_str(),1,"tp","str");
	    ctrMkNode("list",opt,-1,"/mess/mess/2",Mess->I18N("Level"),0440,"root",grp.c_str(),1,"tp","dec");
	    ctrMkNode("list",opt,-1,"/mess/mess/3",Mess->I18N("Message"),0440,"root",grp.c_str(),1,"tp","str");
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	atoi(opt->text().c_str())?start():stop();
    }	
    else if( a_path == "/prm/st/bd" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->text(m_bd);
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	m_bd = opt->text();
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )	opt->text(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(name());
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_name = opt->text();
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(dscr());
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_dscr = opt->text();
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(m_addr);
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_addr = opt->text();
    }
    else if( a_path == "/prm/cfg/lvl" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(TSYS::int2str(m_level));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_level = atoi(opt->text().c_str());
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(m_start?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_start = atoi(opt->text().c_str());
    }
    else if( a_path == "/prm/cfg/cats" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(m_cat_o);
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_cat_o = opt->text();
    }    
    else if( a_path == "/mess/v_beg" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messBeg","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messBeg",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/v_end" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messEnd","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messEnd",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/v_cat" )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) ) 	TBDS::genDBSet(nodePath()+"messCat",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/v_lvl" )
    {	
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	TBDS::genDBSet(nodePath()+"messLev",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/mess/mess" && run_st && ctrChkNode(opt,"get",0440,"root",grp.c_str()) )
    {
	vector<TMess::SRec> rec;
	get( atoi(TBDS::genDBGet(nodePath()+"messBeg","0",opt->attr("user")).c_str()), 
	     atoi(TBDS::genDBGet(nodePath()+"messEnd","0",opt->attr("user")).c_str()), rec, 
	     TBDS::genDBGet(nodePath()+"messCat","",opt->attr("user")), 
	     atoi(TBDS::genDBGet(nodePath()+"messLev","0",opt->attr("user")).c_str()) );

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/mess/mess/0","",0440,"root",grp.c_str());
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/mess/mess/1","",0440,"root",grp.c_str());
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/mess/mess/2","",0440,"root",grp.c_str());
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/mess/mess/3","",0440,"root",grp.c_str());
	for( int i_rec = 0; i_rec < rec.size(); i_rec++)
	{
	    if(n_tm)	n_tm->childAdd("el")->text(TSYS::int2str(rec[i_rec].time));
	    if(n_cat)	n_cat->childAdd("el")->text(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->text(TSYS::int2str(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->text(rec[i_rec].mess);
	}
    }
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0440,"root",grp.c_str()) )	load();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0440,"root",grp.c_str()) )	save();
}

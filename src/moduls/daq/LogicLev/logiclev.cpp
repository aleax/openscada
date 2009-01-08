
//OpenSCADA system module DAQ.LogicLev file: logiclev.cpp
/***************************************************************************
 *   Copyright (C) 2006-2008 by Roman Savochenko                           *
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

#include <sys/times.h>
#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "logiclev.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"LogicLev"
#define MOD_NAME	"Logic level"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.9.2"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow logic level paramers."
#define LICENSE		"GPL"
//*************************************************

LogicLev::TTpContr *LogicLev::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new LogicLev::TTpContr( source );
	return NULL;
    }
}

using namespace LogicLev;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name )
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

TTpContr::~TTpContr()
{

}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TTpContr::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTpContr::postEnable( int flag )
{
    TModule::postEnable( flag );

    //- Controler's bd structure -
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Request data period (ms)"),TFld::Integer,TFld::NoFlag,"5","1000","0;10000") );
    fldAdd( new TFld("PRIOR",_("Request task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );

    //- Parameter type bd structure -
    int t_prm = tpParmAdd("std","PRM_BD",_("Logical"));
    tpPrmAt(t_prm).fldAdd( new TFld("MODE",_("Mode"),TFld::Integer,TCfg::NoVal,"1","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("PRM","",TFld::String,TCfg::NoVal,"100","") );

    //- Logical level parameter IO BD structure -
    el_prm_io.fldAdd( new TFld("PRM_ID",_("Parameter ID"),TFld::String,TCfg::Key,"20") );
    el_prm_io.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_prm_io.fldAdd( new TFld("VALUE",_("Value"),TFld::String,TFld::NoFlag,"200") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_calc(0),
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId())
{
    cfg("PRM_BD").setS("LogLevPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
}

void TMdContr::postDisable(int flag)
{
    TController::postDisable(flag);
    try
    {
	if( flag )
	{
	    //- Delete parameter's io table -
	    string tbl = DB()+"."+cfg("PRM_BD").getS()+"_io";
	    SYS->db().at().open(tbl);
	    SYS->db().at().close(tbl,true);
	}
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    //- Former process parameters list -
    vector<string> list_p;
    list(list_p);
    for(int i_prm=0; i_prm < list_p.size(); i_prm++)
	if( at(list_p[i_prm]).at().enableStat() )
	    prmEn(list_p[i_prm],true);

    //- Start the request data task -
    if( !prc_st )
    {
	pthread_attr_t pthr_attr;
	pthread_attr_init(&pthr_attr);
	struct sched_param prior;
	if( m_prior && SYS->user() == "root" )
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_RR);
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	prior.__sched_priority=m_prior;
	pthread_attr_setschedparam(&pthr_attr,&prior);

	pthread_create(&procPthr,&pthr_attr,TMdContr::Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait(prc_st, true, nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),_("Acquisition task is not started!"));
    }
}

void TMdContr::stop_( )
{
    //- Stop the request and calc data task -
    if( prc_st )
    {
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),_("Acquisition task is not stopped!"));
	pthread_join( procPthr, NULL );
    }

    //- Clear process parameters list -
    p_hd.clear();
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )
	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )
	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    bool is_start = true;
    bool is_stop  = false;

    while(true)
    {
	long long t_cnt = SYS->shrtCnt();

	//- Update controller's data -
	cntr.en_res.resRequestR( );
	for(unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++)
	    try{ cntr.p_hd[i_p].at().calc(is_start,is_stop); }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resReleaseR( );
	cntr.tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());

	if(is_stop) break;

	TSYS::taskSleep((long long)cntr.period()*1000000);

	if(cntr.endrun_req) is_stop = true;
	is_start = false;
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/calc_tm",_("Calc template functions time (ms)"),0444,"root","root",1,"tp","real");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/calc_tm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_calc,6));
    else TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), p_el("w_attr"), prm_refl(NULL), m_wmode(TMdPrm::Free), chk_lnk_need(false), 
    id_freq(-1), id_start(-1), id_stop(-1), m_mode(cfg("MODE").getId()), m_prm(cfg("PRM").getSd())
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable( flag );
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

void TMdPrm::postDisable(int flag)
{
    TParamContr::postDisable(flag);

    try
    {
	if( flag )
	{
	    string io_bd = owner().DB()+"."+owner().cfg(type().db).getS()+"_io";
	    TConfig cfg(&mod->prmIOE());
	    cfg.cfg("PRM_ID").setS(id());
	    cfg.cfg("ID").setS("");
	    SYS->db().at().dataDel(io_bd,owner().owner().nodePath()+owner().cfg(type().db).getS()+"_io",cfg);
	}
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    try
    {
	mode((TMdPrm::Mode)m_mode,m_prm);
	loadIO();
	if(mode() == TMdPrm::Template)
	{
	    //- Init system attributes identifiers -
	    id_freq	= tmpl->val.func()->ioId("f_frq");
	    id_start	= tmpl->val.func()->ioId("f_start");
	    id_stop	= tmpl->val.func()->ioId("f_stop");
	    id_err	= tmpl->val.func()->ioId("f_err");
	}
	if(owner().startStat())	owner().prmEn(id(),true);
    }
    catch(...){ disable(); throw; }
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    if(owner().startStat())	owner().prmEn( id(), false );

    mode(TMdPrm::Free);
    id_freq=id_start=id_stop=id_err=-1;

    TParamContr::disable();
}

void TMdPrm::load_( )
{
    TParamContr::load_();
    try
    {
	mode(mode(),m_prm);
	loadIO();
    }catch(...){ }
}

void TMdPrm::loadIO()
{
    //- Load IO and init links -
    if( mode() == TMdPrm::Template )
    {
	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(id());
	string io_bd = owner().DB()+"."+owner().cfg(type().db).getS()+"_io";

	for( int i_io = 0; i_io < tmpl->val.ioSize(); i_io++ )
	{
	    cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
	    if(!SYS->db().at().dataGet(io_bd,owner().owner().nodePath()+owner().cfg(type().db).getS()+"_io",cfg))
		continue;
	    if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink )
		lnk(lnkId(i_io)).prm_attr = cfg.cfg("VALUE").getS();
	    else tmpl->val.setS(i_io,cfg.cfg("VALUE").getS());
	}
	initTmplLnks();
    }
}

void TMdPrm::save_( )
{
    TParamContr::save_();
    saveIO();
}

void TMdPrm::saveIO()
{
    //- Save IO and init links -
    if( mode() == TMdPrm::Template )
    {
	TConfig cfg(&mod->prmIOE());
	cfg.cfg("PRM_ID").setS(id());
	string io_bd = owner().DB()+"."+owner().cfg(type().db).getS()+"_io";

	for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
	{
	    cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
	    if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink )
		cfg.cfg("VALUE").setS(lnk(lnkId(i_io)).prm_attr);
	    else cfg.cfg("VALUE").setS(tmpl->val.getS(i_io));
	    SYS->db().at().dataSet(io_bd,owner().owner().nodePath()+owner().cfg(type().db).getS()+"_io",cfg);
	}
    }
}

void TMdPrm::mode( TMdPrm::Mode md, const string &prm )
{
    vector<string> list;

    if( !enableStat() ) return;

    ResAlloc res(moderes,true);

    //- Free old mode -
    if( md != mode() || prm != m_wprm )
    {
	if( mode() == TMdPrm::DirRefl )
	{
	    for(int i_f = 0; i_f < p_el.fldSize(); i_f++ )
		if( vlAt(p_el.fldAt(i_f).name()).at().nodeUse() == 1 )
		{
		    p_el.fldDel(i_f);
		    i_f--;
		}
	    delete prm_refl;
	    prm_refl = NULL;
	}
	else if( mode() == TMdPrm::Template )
	{
	    for(int i_f = 0; i_f < p_el.fldSize(); i_f++ )
		if( vlAt(p_el.fldAt(i_f).name()).at().nodeUse() == 1 )
		{
		    p_el.fldDel(i_f);
		    i_f--;
		}
	    delete tmpl;
	    tmpl = NULL;
	}
    }

    //- Init/update new mode -
    if( md == TMdPrm::DirRefl )
    {
	if( !prm_refl ) prm_refl = new AutoHD<TValue>;
	try
	{
	    prm_refl->free();
	    *prm_refl = SYS->daq().at().at(TSYS::strSepParse(prm,0,'.')).at().
					at(TSYS::strSepParse(prm,1,'.')).at().
					at(TSYS::strSepParse(prm,2,'.'));
	    prm_refl->at().vlList(list);
	    for( int i_l = 0; i_l < list.size(); i_l++ )
		if( !vlPresent(list[i_l]) )
		    p_el.fldAdd( new TFld(list[i_l].c_str(),prm_refl->at().vlAt(list[i_l]).at().fld().descr().c_str(),
			prm_refl->at().vlAt(list[i_l]).at().fld().type(),TVal::DirWrite|TVal::DirRead|(prm_refl->at().vlAt(list[i_l]).at().fld().flg())&TFld::NoWrite) );
	}catch(...)
	{
	    m_wmode = Free;
	    delete prm_refl;
	    prm_refl = NULL;
	    throw;
	}
    }
    else if( md == TMdPrm::Template )
    {
	bool to_make = false;
	if( !tmpl )	tmpl = new STmpl;
	try
	{
	    if( !tmpl->val.func() )
	    {
		tmpl->val.setFunc(&SYS->daq().at().tmplLibAt(TSYS::strSepParse(prm,0,'.')).at().
					        at(TSYS::strSepParse(prm,1,'.')).at().func().at());
		tmpl->val.setVfName(id()+"_tmplprm");
		to_make = true;
	    }
	    //-- Set mode --
	    m_wmode = md;
	    //-- Init attrubutes --
	    for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
	    {
		if( (tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink) && lnkId(i_io) < 0 )
		    tmpl->lnk.push_back(SLnk(i_io));
		if( (tmpl->val.func()->io(i_io)->flg()&(TPrmTempl::AttrRead|TPrmTempl::AttrFull)) &&
		    !vlPresent(tmpl->val.func()->io(i_io)->id()) )
		{
		    TFld::Type tp;
		    unsigned flg = TVal::DirWrite|TVal::DirRead;

		    switch( tmpl->val.ioType(i_io) )
		    {
			case IO::String:	tp = TFld::String;	break;
			case IO::Integer:	tp = TFld::Integer;	break;
			case IO::Real:		tp = TFld::Real;	break;
			case IO::Boolean:	tp = TFld::Boolean;	break;
		    }
		    if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::AttrRead )	flg|=TFld::NoWrite;
		    p_el.fldAdd( new TFld(tmpl->val.func()->io(i_io)->id().c_str(),tmpl->val.func()->io(i_io)->name().c_str(),tp,flg) );
		}
		if( to_make && (tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink) )	tmpl->val.setS(i_io,"0");
	    }
	    //-- Init links --
	    initTmplLnks();
	}catch(TError err)
	{
	    m_wmode = Free;
	    delete tmpl;
	    tmpl = NULL;
	    throw;
	}
    }
    m_wmode = md;
    m_wprm = prm;
}

void TMdPrm::initTmplLnks()
{
    if( mode() != TMdPrm::Template )    return;
    //- Init links -
    chk_lnk_need = false;
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
    {
	try
	{
	    lnk(i_l).aprm.free();
	    lnk(i_l).aprm = SYS->daq().at().at(TSYS::strSepParse(lnk(i_l).prm_attr,0,'.')).at().
					    at(TSYS::strSepParse(lnk(i_l).prm_attr,1,'.')).at().
					    at(TSYS::strSepParse(lnk(i_l).prm_attr,2,'.')).at().
					    vlAt(TSYS::strSepParse(lnk(i_l).prm_attr,3,'.'));
	    tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).aprm.at().getS());
	}catch(TError err){ chk_lnk_need = true; }
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if( !enableStat() || !owner().startStat() )
    {
	if( val.name() == "err" )
	{
	    if(!enableStat())
		val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())
		val.setS(_("2:Controller is stoped."),0,true);
	}
	return;
    }
    try
    {
	ResAlloc res(moderes,false);
	if( mode() == TMdPrm::DirRefl )
	    switch(val.fld().type())
	    {
		case TFld::String:
		    val.setS(prm_refl->at().vlAt(val.name()).at().getS(),0,true);
		    break;
		case TFld::Integer:
		    val.setI(prm_refl->at().vlAt(val.name()).at().getI(),0,true);
		    break;
		case TFld::Real:
		    val.setR(prm_refl->at().vlAt(val.name()).at().getR(),0,true);
		    break;
		case TFld::Boolean:
		    val.setB(prm_refl->at().vlAt(val.name()).at().getB(),0,true);
		    break;
	    }
	else if( mode() == TMdPrm::Template )
	{
	    int id_lnk = lnkId(val.name());
	    if( id_lnk >= 0 && lnk(id_lnk).aprm.freeStat() ) id_lnk=-1;
	    switch(val.fld().type())
	    {
		case TFld::String:
		    if( id_lnk < 0 ) val.setS(tmpl->val.getS(tmpl->val.ioId(val.name())),0,true);
		    else val.setS(lnk(id_lnk).aprm.at().getS(),0,true);
		    break;
		case TFld::Integer:
		    if( id_lnk < 0 ) val.setI(tmpl->val.getI(tmpl->val.ioId(val.name())),0,true);
		    else val.setI(lnk(id_lnk).aprm.at().getI(),0,true);
		    break;
		case TFld::Real:
		    if( id_lnk < 0 ) val.setR(tmpl->val.getR(tmpl->val.ioId(val.name())),0,true);
		    else val.setR(lnk(id_lnk).aprm.at().getR(),0,true);
		    break;
		case TFld::Boolean:
		    if( id_lnk < 0 ) val.setB(tmpl->val.getB(tmpl->val.ioId(val.name())),0,true);
		    else val.setB(lnk(id_lnk).aprm.at().getB(),0,true);
		    break;
	    }
	}
    }catch(TError err)
    {
	if( val.name() == "err" )
	{
	    if( mode() == TMdPrm::Template && id_err >= 0 )
		val.setS(tmpl->val.getS(id_err),0,true);
	    else val.setS("0",0,true);
	}
    }
}

void TMdPrm::vlSet( TVal &val )
{
    try
    {
	ResAlloc res(moderes,false);
	if( mode() == TMdPrm::DirRefl )
	    switch(val.fld().type())
	    {
		case TFld::String:
		    prm_refl->at().vlAt(val.name()).at().setS(val.getS(0,true));
		    break;
		case TFld::Integer:
		    prm_refl->at().vlAt(val.name()).at().setI(val.getI(0,true));
		    break;
		case TFld::Real:
		    prm_refl->at().vlAt(val.name()).at().setR(val.getR(0,true));
		    break;
		case TFld::Boolean:
		    prm_refl->at().vlAt(val.name()).at().setB(val.getB(0,true));
		    break;
	    }
	else if( mode() == TMdPrm::Template )
	{
	    int id_lnk = lnkId(val.name());
	    if( id_lnk >= 0 && lnk(id_lnk).aprm.freeStat() ) id_lnk=-1;
	    switch(val.fld().type())
	    {
		case TFld::String:
		    if( id_lnk < 0 ) tmpl->val.setS(tmpl->val.ioId(val.name()),val.getS(0,true));
		    else lnk(id_lnk).aprm.at().setS(val.getS(0,true));
		    break;
		case TFld::Integer:
		    if( id_lnk < 0 ) tmpl->val.setI(tmpl->val.ioId(val.name()),val.getI(0,true));
		    else lnk(id_lnk).aprm.at().setI(val.getI(0,true));
		    break;
		case TFld::Real:
		    if( id_lnk < 0 ) tmpl->val.setR(tmpl->val.ioId(val.name()),val.getR(0,true));
		    else lnk(id_lnk).aprm.at().setR(val.getR(0,true));
		    break;
		case TFld::Boolean:
		    if( id_lnk < 0 ) tmpl->val.setB(tmpl->val.ioId(val.name()),val.getB(0,true));
		    else lnk(id_lnk).aprm.at().setB(val.getB(0,true));
		    break;
	    }
	}
    }catch(TError err) {  }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(((long long)owner().period())*1000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

int TMdPrm::lnkSize()
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),_("Parameter is disabled or is not based on the template."));
    return tmpl->lnk.size();
}

int TMdPrm::lnkId( int id )
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),_("Parameter is disabled or is not based on the template."));
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
	if( lnk(i_l).io_id == id )
	    return i_l;
    return -1;
}

int TMdPrm::lnkId( const string &id )
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),_("Parameter is disabled or is not based on the template."));
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
	if( tmpl->val.func()->io(lnk(i_l).io_id)->id() == id )
	    return i_l;
    return -1;
}

TMdPrm::SLnk &TMdPrm::lnk( int num )
{
    if( mode() != TMdPrm::Template )
        throw TError(nodePath().c_str(),_("Parameter is disabled or is not based on the template."));
    if( num < 0 || num >= tmpl->lnk.size() )
	throw TError(nodePath().c_str(),_("Parameter id error."));
    return tmpl->lnk[num];
}

void TMdPrm::calc( bool first, bool last )
{
    if( mode() != TMdPrm::Template )    return;
    try
    {
	ResAlloc res(moderes,false);
	if(chk_lnk_need) initTmplLnks();

	//- Set fixed system attributes -
	if(id_freq>=0) 	tmpl->val.setR(id_freq,1000./owner().period());
	if(id_start>=0)	tmpl->val.setB(id_start,first);
	if(id_stop>=0)	tmpl->val.setB(id_stop,last);

        //- Get input links -
	for( int i_l = 0; i_l < lnkSize(); i_l++ )
	    if( lnk(i_l).aprm.freeStat() )	tmpl->val.setS(lnk(i_l).io_id,EVAL_STR);
	    else switch(tmpl->val.ioType(lnk(i_l).io_id))
	    {
		case IO::String:
		    tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).aprm.at().getS());
		    break;
		case IO::Integer:
		    tmpl->val.setI(lnk(i_l).io_id,lnk(i_l).aprm.at().getI());
		    break;
		case IO::Real:
		    tmpl->val.setR(lnk(i_l).io_id,lnk(i_l).aprm.at().getR());
		    break;
		case IO::Boolean:
		    tmpl->val.setB(lnk(i_l).io_id,lnk(i_l).aprm.at().getB());
		    break;
	    }

	//- Calc template -
	tmpl->val.calc();
	modif();

	//- Put output links -
	for( int i_l = 0; i_l < lnkSize(); i_l++ )
	    if( !lnk(i_l).aprm.freeStat() &&
		    tmpl->val.ioFlg(lnk(i_l).io_id)&(IO::Output|IO::Return) &&
		    !(lnk(i_l).aprm.at().fld().flg()&TFld::NoWrite) )
		switch(tmpl->val.ioType(lnk(i_l).io_id))
		{
		    case IO::String:
			lnk(i_l).aprm.at().setS(tmpl->val.getS(lnk(i_l).io_id));
			break;
		    case IO::Integer:
			lnk(i_l).aprm.at().setI(tmpl->val.getI(lnk(i_l).io_id));
			break;
		    case IO::Real:
			lnk(i_l).aprm.at().setR(tmpl->val.getR(lnk(i_l).io_id));
			break;
		    case IO::Boolean:
			lnk(i_l).aprm.at().setB(tmpl->val.getB(lnk(i_l).io_id));
			break;
		}
    }catch(TError err)
    {
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Error calc template."));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //- Service commands process -
    string a_path = opt->attr("path");
    if( a_path.substr(0,6) == "/serv/" )  { TParamContr::cntrCmdProc(opt); return; }

    vector<string> list;
    //- Get page info -
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/MODE",cfg("MODE").fld().descr(),0660,"root","root",4,"tp","str","dest","select",
	    "sel_id",(TSYS::int2str(TMdPrm::Free)+";"+TSYS::int2str(TMdPrm::DirRefl)+";"+TSYS::int2str(TMdPrm::Template)).c_str(),
	    "sel_list",_("Free;Direct reflection;Template"));
	ctrMkNode("fld",opt,-1,"/prm/cfg/PRM",cfg("PRM").fld().descr(),0660,"root","root",3,"tp","str","dest","sel_ed","select","/prm/cfg/prmp_lst");
	if( mode() == TMdPrm::Template && ctrMkNode("area",opt,-1,"/cfg",_("Template config")) )
	{
	    ctrMkNode("fld",opt,-1,"/cfg/attr_only",_("Only atributes are to be shown"),0664,"root","root",1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/cfg/prm",_("Parameters")))
		for( int i_io = 0; i_io < tmpl->val.ioSize(); i_io++ )
		{
		    if( !(tmpl->val.func()->io(i_io)->flg()&(TPrmTempl::CfgLink|TPrmTempl::CfgPublConst)) )
			continue;
		    //-- Check select param --
		    bool is_lnk = tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink;
		    if( is_lnk && tmpl->val.func()->io(i_io)->def().size() && 
			!atoi(TBDS::genDBGet(mod->nodePath()+"onlAttr","0",opt->attr("user")).c_str()) )
		    {
			string nprm = TSYS::strSepParse(tmpl->val.func()->io(i_io)->def(),0,'|');
			//-- Check already to present parameters --
			bool f_ok = false;
			for( int i_l = 0; i_l < list.size(); i_l++ )
			    if( list[i_l] == nprm ) { f_ok = true; break; }
			if(!f_ok)
			{
			    ctrMkNode("fld",opt,-1,(string("/cfg/prm/pr_")+TSYS::int2str(i_io)).c_str(),nprm,0664,"root","root",
				    3,"tp","str","dest","sel_ed","select",(string("/cfg/prm/pl_")+TSYS::int2str(i_io)).c_str());
			    list.push_back(nprm);
			}
		    }
		    else
		    {
			const char *tip = "str";
			if( !is_lnk )
			    switch(tmpl->val.ioType(i_io))
			    {
				case IO::Integer:	tip = "dec";	break;
				case IO::Real:		tip = "real";	break;
				case IO::Boolean:	tip = "bool";	break;
			    }
			ctrMkNode("fld",opt,-1,(string("/cfg/prm/el_")+TSYS::int2str(i_io)).c_str(),tmpl->val.func()->io(i_io)->name(),0664,"root","root",
			    3,"tp",tip,"dest",is_lnk?"sel_ed":"","select",is_lnk?(string("/cfg/prm/ls_")+TSYS::int2str(i_io)).c_str():"");
		    }
		}
	}
    }

    //- Process command to page -
    if( a_path == "/prm/cfg/MODE" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	try
	{
	    m_mode = atoi(opt->text().c_str());
	    mode( (TMdPrm::Mode)m_mode, m_prm = "" );
	} catch(...) { disable(); throw; }
    else if( a_path == "/prm/cfg/PRM" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	try
	{
	    m_prm = opt->text();
	    if( m_mode == TMdPrm::DirRefl && m_prm == owner().owner().modId()+"."+owner().id()+"."+id() )
		m_prm = owner().owner().modId()+"."+owner().id();
	    mode( (TMdPrm::Mode)m_mode, m_prm );
	} catch(...){ disable(); throw; }
    else if( a_path == "/prm/cfg/prmp_lst" && ctrChkNode(opt) )
    {
	int c_lv = 0;
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for( int c_off = 0; (c_el=TSYS::strSepParse(m_prm,0,'.',&c_off)).size(); c_lv++ )
	{
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	string prm0 = TSYS::strSepParse(m_prm,0,'.');
	string prm1 = TSYS::strSepParse(m_prm,1,'.');
	vector<string>  ls;
	switch(c_lv)
	{
	    case 0:
		if( m_mode == DirRefl )		SYS->daq().at().modList(ls);
		if( m_mode == Template )	SYS->daq().at().tmplLibList(ls);
		break;
	    case 1:
		if( m_mode == DirRefl && SYS->daq().at().modPresent(prm0) )
		    SYS->daq().at().at(prm0).at().list(ls);
		if( m_mode == Template && SYS->daq().at().tmplLibPresent(prm0) )
		    SYS->daq().at().tmplLibAt(prm0).at().list(ls);
		break;
	    case 2:
		if( m_mode == DirRefl && SYS->daq().at().modPresent(prm0) &&
			SYS->daq().at().at(prm0).at().present(prm1) )
		    SYS->daq().at().at(prm0).at().at(prm1).at().list(ls);
		break;
	}
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if( a_path == "/cfg/attr_only" && mode() == TMdPrm::Template )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(TBDS::genDBGet(mod->nodePath()+"onlAttr","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	TBDS::genDBSet(mod->nodePath()+"onlAttr",opt->text(),opt->attr("user"));
    }
    else if( a_path.substr(0,12) == "/cfg/prm/pr_" && mode() == TMdPrm::Template )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    string lnk_val = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
	    int c_lvl = 0;
	    for( int c_off = 0; TSYS::strSepParse(lnk_val,0,'.',&c_off).size(); c_lvl++ );
	    if( c_lvl==4 ) opt->setText(lnk_val.substr(0,lnk_val.rfind(".")));
	    else opt->setText(lnk_val);	
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    bool noonly_no_set = true;
	    string no_set;
	    string p_nm = TSYS::strSepParse(tmpl->val.func()->io(lnk(lnkId(atoi(a_path.substr(12).c_str()))).io_id)->def(),0,'|');
	    string p_vl = opt->text();
	    int c_lvl = 0;
	    for( int c_off = 0; TSYS::strSepParse(p_vl,0,'.',&c_off).size(); c_lvl++ );
	    AutoHD<TValue> prm;
	    if(c_lvl==3)
	    {
		if( TSYS::strSepParse(p_vl,0,'.') == owner().owner().modId() &&
			TSYS::strSepParse(p_vl,1,'.') == owner().id() &&
			TSYS::strSepParse(p_vl,2,'.') == id() )
		    throw TError(nodePath().c_str(),_("Self to self linking error."));
		prm = SYS->daq().at().at(TSYS::strSepParse(p_vl,0,'.')).at().
				      at(TSYS::strSepParse(p_vl,1,'.')).at().
				      at(TSYS::strSepParse(p_vl,2,'.'));
	    }
	    for( int i_l = 0; i_l < lnkSize(); i_l++ )
		if( p_nm == TSYS::strSepParse(tmpl->val.func()->io(lnk(i_l).io_id)->def(),0,'|') )
		{
		    lnk(i_l).prm_attr = p_vl;
		    string p_attr = TSYS::strSepParse(tmpl->val.func()->io(lnk(i_l).io_id)->def(),1,'|');
		    if( !prm.freeStat() )
		    {
			if( prm.at().vlPresent(p_attr) )
			{
			    lnk(i_l).prm_attr= p_vl+"."+p_attr;
			    modif( );
			    noonly_no_set = false;
			}
			else no_set+=p_attr+",";
		    }
		}
	    if(!prm.freeStat())
	    {
		if( noonly_no_set )	throw TError(nodePath().c_str(),_("Parameter has no one atribute!"));
		else if( no_set.size() )throw TError(nodePath().c_str(),_("Parameter has not atributes: %s !"),no_set.c_str());
		mode( (TMdPrm::Mode)m_mode, m_prm );
	    }
	}
    }
    else if( (a_path.substr(0,12) == "/cfg/prm/pl_" || a_path.substr(0,12) == "/cfg/prm/ls_") && 
	    mode() == TMdPrm::Template && ctrChkNode(opt) )
    {
	int c_lv = 0;
	string l_prm = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
	bool is_pl = (a_path.substr(0,12) == "/cfg/prm/pl_");
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for( int c_off = 0; (c_el=TSYS::strSepParse(l_prm,0,'.',&c_off)).size(); c_lv++ )
	{
	    if( is_pl && c_lv>2 ) break;
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	string prm0 = TSYS::strSepParse(l_prm,0,'.');
	string prm1 = TSYS::strSepParse(l_prm,1,'.');
	string prm2 = TSYS::strSepParse(l_prm,2,'.');
	vector<string>  ls;
	switch( c_lv )
        {
	    case 0:	SYS->daq().at().modList(ls);	break;
	    case 1:
		if(SYS->daq().at().modPresent(prm0))
		    SYS->daq().at().at(prm0).at().list(ls);
		break;
	    case 2:
		 if(SYS->daq().at().modPresent(prm0) && SYS->daq().at().at(prm0).at().present(prm1))
		    SYS->daq().at().at(prm0).at().at(prm1).at().list(ls);
		break;
	    case 3:
		if(!is_pl && SYS->daq().at().modPresent(prm0) && SYS->daq().at().at(prm0).at().present(prm1)
			&& SYS->daq().at().at(prm0).at().at(prm1).at().present(prm2) )
		    SYS->daq().at().at(prm0).at().at(prm1).at().at(prm2).at().vlList(ls);
		break;
	}
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if( a_path.substr(0,12) == "/cfg/prm/el_" && mode() == TMdPrm::Template )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    int i_io = atoi(a_path.substr(12).c_str());
	    if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink )
		opt->setText(lnk(lnkId(i_io)).prm_attr);
	    else if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgPublConst )
		opt->setText(tmpl->val.getS(i_io));
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    int i_io = atoi(a_path.substr(12).c_str());
	    if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgLink )
	    {
		if( TSYS::strSepParse(opt->text(),0,'.') == owner().owner().modId() &&
			TSYS::strSepParse(opt->text(),1,'.') == owner().id() &&
			TSYS::strSepParse(opt->text(),2,'.') == id() )
		    throw TError(nodePath().c_str(),_("Self to self linking error."));
		lnk(lnkId(i_io)).prm_attr = opt->text();
		mode( (TMdPrm::Mode)m_mode, m_prm );
	    }
	    else if( tmpl->val.func()->io(i_io)->flg()&TPrmTempl::CfgPublConst )
		tmpl->val.setS(i_io,opt->text());
	    modif();
	}
    }
    else TParamContr::cntrCmdProc(opt);
}

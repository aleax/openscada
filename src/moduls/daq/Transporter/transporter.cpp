
//OpenSCADA system module DAQ.Transporter file: transporter.cpp
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

#include <getopt.h>
#include <signal.h>

#include <tsys.h>
#include <ttiparam.h>

#include "transporter.h"

//******************************************************
//* Modul info!                                        *
#define MOD_ID		"Transporter"
#define MOD_NAME	"Data sources transporter"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.3.2"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow to make transport data sources of remote OpenSCADA station to local OpenSCADA station."
#define LICENSE		"GPL"
//******************************************************

DAQTrasport::TTpContr *DAQTrasport::mod;  //Pointer for direct access to main module object

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
	    return new DAQTrasport::TTpContr( source );
	return NULL;
    }
}

using namespace DAQTrasport;

//******************************************************
//* TTpContr                                           *
//******************************************************
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

TTpContr::~TTpContr( )
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
    TModule::postEnable(flag);

    //- Controler's DB structure -
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000") );
    fldAdd( new TFld("STATIONS",_("Remote stations list"),TFld::String,TFld::FullText,"100") );
    fldAdd( new TFld("CNTRPRM",_("Remote cotrollers and parameters list"),TFld::String,TFld::FullText,"200") );
    //- Parameter type bd structure -
    int t_prm = tpParmAdd("std","",_("Standard"));
    //- Set to read only  -
    for( int i_sz = 0; i_sz < tpPrmAt(t_prm).fldSize(); i_sz++ )
	tpPrmAt(t_prm).fldAt(i_sz).setFlg(tpPrmAt(t_prm).fldAt(i_sz).flg()|TFld::NoWrite);
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

int TTpContr::cntrIfCmd( XMLNode &node )
{
    int path_off = 0;
    string path = node.attr("path");
    string station = TSYS::pathLev(path,0,false,&path_off);
    node.setAttr("path",path.substr(path_off));

    //-- Connect to transport --
    TTransportS::ExtHost host = SYS->transport().at().extHostGet("*",station);
    AutoHD<TTransportOut> tr = SYS->transport().at().extHost(host,"TrCntr");
    if( !tr.at().startStat() )	tr.at().start();
    node.load(tr.at().messProtIO("0\n"+host.user+"\n"+host.pass+"\n"+node.save(),"SelfSystem"));
    node.setAttr("path",path);
    return atoi(node.attr("rez").c_str());
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0),
	m_per(cfg("PERIOD").getRd()), m_prior(cfg("PRIOR").getId()), m_sync(cfg("SYNCPER").getRd()),
	m_stations(cfg("STATIONS").getSd()), m_contr_prm(cfg("CNTRPRM").getSd())
{

}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    string statv, cp_el, daqtp, cntrnm, prmnm, cntrpath;
    vector<string> prm_ls;
    XMLNode req("list");

    bool en_err = false;

    //- Remote station scaning -
    for( int st_off = 0; (statv=TSYS::strSepParse(m_stations,0,'\n',&st_off)).size(); )
	//- Controllers and parameters scaning -
	for( int cp_off = 0; (cp_el=TSYS::strSepParse(m_contr_prm,0,'\n',&cp_off)).size(); )
	    try
	    {
		//-- Parse parameter template --
		daqtp  = TSYS::strSepParse(cp_el,0,'.');
		cntrnm = TSYS::strSepParse(cp_el,1,'.');
		prmnm  = TSYS::strSepParse(cp_el,2,'.');
		if( daqtp.empty() || cntrnm.empty() )	continue;
		cntrpath = "/"+statv+"/DAQ/"+daqtp+"/"+cntrnm+"/";
		//-- Get parameters list --
		prm_ls.clear();
		if( prmnm.empty() || prmnm == "*" )
		{
		    //-- Get attributes list --
		    req.clear()->setName("get")->setAttr("path",cntrpath+"%2fprm%2fprm");
		    if( mod->cntrIfCmd(req) || req.childSize() == 0 )	en_err = true;
		    else for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
			prm_ls.push_back(req.childGet(i_ch)->attr("id"));
		}
		else prm_ls.push_back(prmnm);

		//-- Process remote parameters --
		for( int i_p = 0; i_p < prm_ls.size(); i_p++ )
		{
		    if( !present(prm_ls[i_p]) )
		    {
			//--- Parameter name request and make new parameter object ---
			req.clear()->setName("get")->setAttr("path",cntrpath+prm_ls[i_p]+"/%2fprm%2fcfg%2fNAME");
			if( mod->cntrIfCmd(req) ) { en_err = true; continue; }
			add(prm_ls[i_p],owner().tpPrmToId("std"));
			at(prm_ls[i_p]).at().setName(req.text());
		    }
		    if( !at(prm_ls[i_p]).at().enableStat() )
			at(prm_ls[i_p]).at().enable();
		    at(prm_ls[i_p]).at().setCntrAdr(cntrpath);
		    at(prm_ls[i_p]).at().load();
		}
	    }catch(TError err){ }

    if( en_err ) throw TError(nodePath().c_str(),_("Some enable errors is present"));
}

void TMdContr::start_( )
{
    if( !prc_st )
    {
	//- Start the gathering data task -
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
	    throw TError(nodePath().c_str(),_("Gathering task no started!"));
    }
}

void TMdContr::stop_( )
{
    if( prc_st )
    {
	//- Stop the request and calc data task -
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),_("Gathering task no stoped!"));
	pthread_join( procPthr, NULL );
    }
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {
	for( unsigned int it_cnt = 0; !cntr.endrun_req; it_cnt++ )
	{
	    long long t_cnt = SYS->shrtCnt();
	    cntr.en_res.resRequestR( );

	    //- Update controller's data -
	    for( int i_p=0; i_p < cntr.p_hd.size(); i_p++)
	    {
		//-- Update parameter's values --
		cntr.p_hd[i_p].at().update();
		//-- Check sync moment and sync parameter's structure --
		unsigned int div = (unsigned int)(cntr.m_sync/cntr.period());
		if( div && (it_cnt+i_p)%div == 0 )
		{
		    cntr.p_hd[i_p].at().modifG();
		    cntr.p_hd[i_p].at().load();
		    //-- Check for delete parameter --
		    if( cntr.p_hd[i_p].at().isDel() )
		    {
			cntr.en_res.resReleaseR( );
			string pid = cntr.p_hd[i_p].at().id();
			cntr.at(pid).at().disable();
			cntr.del(pid);
			cntr.en_res.resRequestR( );
			i_p--;
		    }
		}
	    }

	    //- Calc acquisition process time -
	    cntr.en_res.resReleaseR( );
	    cntr.tm_gath = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());

	    TSYS::taskSleep((long long)cntr.period()*1000000000);
	}
    }catch(TError err)	{ mess_err(err.cat.c_str(),err.mess.c_str()); }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/gath_tm",_("Gather data time (ms)"),0444,"root","root",1,"tp","real");
	ctrMkNode("comm",opt,-1,"/cntr/cfg/host_lnk",_("Go to remote stations list configuration"),0660,"root","root",1,"tp","lnk");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/gath_tm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_gath,6));
    else if( a_path == "/cntr/cfg/host_lnk" && ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )
    {
	SYS->transport().at().setSysHost(true);
	opt->setText("/Transport");
    }
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), m_pdel(false)
{
    setToEnable(true);
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(vlCfg())  setVlCfg(NULL);
    if(!vlElemPresent(&p_el))	vlElemAtt(&p_el);
}

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

    //- Set EVAL to parameter attributes -
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::setCntrAdr( const string &vl )
{
    string scntr;
    for( int off = 0; (scntr=TSYS::strSepParse(m_cntr_adr,0,';',&off)).size(); )
	if( scntr == vl ) return;
    m_cntr_adr+=vl+";";
}

void TMdPrm::load_( )
{
    string scntr;
    XMLNode req("get");
    //- Request and update attributes list -
    for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
    {
	try
	{
	    //-- Parameter name request ---
	    req.setAttr("path",scntr+id()+"/%2fprm%2fcfg%2fNAME");
	    if( mod->cntrIfCmd(req) )	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	    setName(req.text());
	    //-- Parameter description request --
	    req.clear()->setAttr("path",scntr+id()+"/%2fprm%2fcfg%2fDESCR");
	    if( mod->cntrIfCmd(req) )	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	    setDescr(req.text());

	    //-- Attributes list request --
	    req.clear()->setName("list")->setAttr("path",scntr+id()+"/%2fserv%2fattr");
	    if( mod->cntrIfCmd(req) )	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	    //--- Check and create new attributes ---
	    for( int i_a = 0; req.childSize(); i_a++ )
	    {
		XMLNode *ael = req.childGet(i_a);
		if( vlPresent(ael->attr("id")) )	continue;
		TFld::Type tp = (TFld::Type)atoi(ael->attr("tp").c_str());
		string dvl    = EVAL_STR;
		switch(tp)
		{
		    case TFld::Boolean:	dvl = TSYS::int2str(EVAL_BOOL);	break;
		    case TFld::Integer:	dvl = TSYS::int2str(EVAL_INT);	break;
		    case TFld::Real:	dvl = TSYS::real2str(EVAL_REAL);break;
		}
		p_el.fldAdd( new TFld( ael->attr("id").c_str(),ael->attr("nm").c_str(),tp,
		    atoi(ael->attr("flg").c_str())&(TFld::Selected|TFld::NoWrite|TFld::HexDec|TFld::OctDec|TFld::FullText)|TVal::DirWrite,
		    "",dvl.c_str(),ael->attr("vals").c_str(),ael->attr("names").c_str()) );
	    }
	    //--- Remove attributes ---
	    //????
	    return;
	}catch(TError err) { continue; }
    }
}

void TMdPrm::update( )
{
    string scntr;
    XMLNode req("get");
    //- Request and update attributes list -
    for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
	try
	{
	    //-- Attributes values request --
	    req.clear()->setAttr("path",scntr+id()+"/%2fserv%2fattr");
	    if( mod->cntrIfCmd(req) )	throw TError(req.attr("mcat").c_str(),req.text().c_str());
	    for( int i_a = 0; req.childSize(); i_a++ )
		vlAt(req.childGet(i_a)->attr("id")).at().setS(req.childGet(i_a)->text(),0,true);
	    return;
	}catch(TError err) { continue; }
}

void TMdPrm::vlSet( TVal &valo )
{
    if( !enableStat() )	valo.setI(EVAL_INT,0,true);
    string scntr;
    for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
	try
	{
	    XMLNode req("set");
	    req.clear()->setAttr("path",scntr+id()+"/%2fserv%2fattr")->
		childAdd("el")->setAttr("id",valo.name())->setText(valo.getS());
	    if( mod->cntrIfCmd(req) )   throw TError(req.attr("mcat").c_str(),req.text().c_str());
	}catch(TError err) { continue; }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //- Service commands process -
    if( a_path.substr(0,6) == "/serv/" ) { TParamContr::cntrCmdProc(opt); return; }

    //- Get page info -
    if( opt->name() == "info" )
    {
	TValue::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter: ")+name());
	if(ctrMkNode("area",opt,0,"/prm",_("Parameter")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),0444,"root","root",1,"tp","str");
		if( owner().enableStat() )
		    ctrMkNode("fld",opt,-1,"/prm/st/en",_("Enable"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/id",_("Id"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/nm",_("Name"),0444,"root","root",1,"tp","str");
	    }
	    XMLNode *cfgN = ctrMkNode("area",opt,-1,"/prm/cfg",_("Config"));
	    if(cfgN)
	    {
		//-- Get remote parameter's config section --
		string scntr;
		XMLNode req("info");
		for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
		    try
		    {
			req.clear()->setAttr("path",scntr+id()+"/%2fprm%2fcfg");
			if( mod->cntrIfCmd(req) ) throw TError(req.attr("mcat").c_str(),req.text().c_str());
			break;
		    }catch(TError err) { continue; }
		if( req.childSize() )
		{
		    *cfgN = *req.childGet(0);
		    cfgN->setAttr("dscr",_("Remote station config"));
		}
	    }
	}
	return;
    }
    //- Process command to page -
    if( a_path == "/prm/st/type" && ctrChkNode(opt) )   opt->setText(type().descr);
    else if( a_path == "/prm/st/en" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )   opt->setText(enableStat()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    if( !owner().enableStat() ) throw TError(nodePath().c_str(),"Controller no started!");
	    else atoi(opt->text().c_str())?enable():disable();
	}
    }
    else if( a_path == "/prm/st/id" && ctrChkNode(opt) )opt->setText(id());
    else if( a_path == "/prm/st/nm" && ctrChkNode(opt) )opt->setText(name());
    else if( a_path.substr(0,8) == "/prm/cfg" )
    {
	//- Request to remote host -
	string scntr;
	for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
	    try
	    {
		opt->setAttr("path",scntr+id()+"/"+TSYS::strEncode(a_path,TSYS::PathEl));
		if( mod->cntrIfCmd(*opt) ) throw TError(opt->attr("mcat").c_str(),opt->text().c_str());
	    }catch(TError err) { continue; }
	opt->setAttr("path",a_path);
    }
    else TValue::cntrCmdProc(opt);
}

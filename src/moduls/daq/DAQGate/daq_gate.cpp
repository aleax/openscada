
//OpenSCADA system module DAQ.DAQGate file: daq_gate.cpp
/***************************************************************************
 *   Copyright (C) 2007-2009 by Roman Savochenko                           *
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

#include "daq_gate.h"

//******************************************************
//* Modul info!                                        *
#define MOD_ID		"DAQGate"
#define MOD_NAME	"Data sources gate"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.8.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow to make gate data sources of remote OpenSCADA station to local OpenSCADA station."
#define LICENSE		"GPL"
//******************************************************

DAQGate::TTpContr *DAQGate::mod;  //Pointer for direct access to main module object

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 ) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) ) return new DAQGate::TTpContr( source );
	return NULL;
    }
}

using namespace DAQGate;

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
    //> Load parameters from command line
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
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's DB structure
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"3","30","0;1000") );
    fldAdd( new TFld("TM_REST_DT",_("Restore data depth time (hour). Zero for disable archive access."),TFld::Real,TFld::NoFlag,"6.2","1","0;12") );
    fldAdd( new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000") );
    fldAdd( new TFld("STATIONS",_("Remote stations list"),TFld::String,TFld::FullText,"100") );
    fldAdd( new TFld("CNTRPRM",_("Remote cotrollers and parameters list"),TFld::String,TFld::FullText,"200") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","",_("Standard"));
    //> Set to read only
    for( int i_sz = 0; i_sz < tpPrmAt(t_prm).fldSize(); i_sz++ )
	tpPrmAt(t_prm).fldAt(i_sz).setFlg(tpPrmAt(t_prm).fldAt(i_sz).flg()|TFld::NoWrite);
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prcSt(false), endrunReq(false), tmGath(0),
	mPer(cfg("PERIOD").getRd()), mPrior(cfg("PRIOR").getId()), mSync(cfg("SYNCPER").getRd()),
	mRestTm(cfg("TM_REST").getId()), mRestDtTm(cfg("TM_REST_DT").getRd()),
	mStations(cfg("STATIONS").getSd()), mContrPrm(cfg("CNTRPRM").getSd())
{

}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );

    if( startStat( ) && !redntUse( ) )
    {
	val += TSYS::strMess(_("Gather data time %.6g ms. "),tmGath);
	bool isWork = false;
	for( int i_st = 0; i_st < mStatWork.size(); i_st++ )
	    if( mStatWork[i_st].second > 0 ) val += TSYS::strMess(_("Station '%s' error, restoring in %.6g s."),mStatWork[i_st].first.c_str(),mStatWork[i_st].second);
	    else { val += TSYS::strMess(_("Requests to station '%s': %.6g."),mStatWork[i_st].first.c_str(),-mStatWork[i_st].second); isWork = true; }
	if( !isWork ) val.replace(0,1,"10");
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    string statv, cp_el, daqtp, cntrnm, prmnm, cntrpath, gPrmLs;
    bool gAddPrm = false;
    vector<string> prm_ls;
    XMLNode req("list");

    bool en_err = false;

    //> Clear present parameters configuration
    list(prm_ls);
    for( int i_p = 0; i_p < prm_ls.size(); i_p++ ) at(prm_ls[i_p]).at().setCntrAdr("");

    //> Remote station scaning. Controllers and parameters scaning
    for( int st_off = 0; (statv=TSYS::strSepParse(mStations,0,'\n',&st_off)).size(); )
    {
	if( !enableStat( ) ) mStatWork.push_back(pair<string,float>(statv,0));
	for( int cp_off = 0; (cp_el=TSYS::strSepParse(mContrPrm,0,'\n',&cp_off)).size(); )
	    try
	    {
		//>> Parse parameter template
		daqtp  = TSYS::strSepParse(cp_el,0,'.');
		cntrnm = TSYS::strSepParse(cp_el,1,'.');
		prmnm  = TSYS::strSepParse(cp_el,2,'.');
		if( daqtp.empty() || cntrnm.empty() )	continue;
		cntrpath = "/"+statv+"/DAQ/"+daqtp+"/"+cntrnm+"/";
		//>> Get parameters list
		prm_ls.clear();
		if( prmnm.empty() || prmnm == "*" )
		{
		    //>> Get attributes list
		    req.clear()->setName("get")->setAttr("path",cntrpath+"%2fprm%2fprm");
		    if( cntrIfCmd(req) ) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
		    else for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
			prm_ls.push_back(req.childGet(i_ch)->attr("id"));
		}
		else prm_ls.push_back(prmnm);

		//>> Process remote parameters
		for( int i_p = 0; i_p < prm_ls.size(); i_p++ )
		{
		    if( !present(prm_ls[i_p]) )
		    {
			//>>> Parameter name request and make new parameter object
			req.clear()->setName("get")->setAttr("path",cntrpath+prm_ls[i_p]+"/%2fprm%2fcfg%2fNAME");
			if( cntrIfCmd(req) ) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
			add(prm_ls[i_p],owner().tpPrmToId("std"));
			at(prm_ls[i_p]).at().setName(req.text());
			gAddPrm = true;
		    }
		    if( !at(prm_ls[i_p]).at().enableStat() ) at(prm_ls[i_p]).at().enable();
		    at(prm_ls[i_p]).at().setCntrAdr(cntrpath);
		    at(prm_ls[i_p]).at().load();
		    gPrmLs += prm_ls[i_p]+";";
		}
	    }catch(TError err){ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }

    //> Removing parameter's try
    if( !enableStat( ) )
    {
	list(prm_ls);
	for( int i_p = 0; i_p < prm_ls.size(); i_p++ )
	    if( gPrmLs.find(prm_ls[i_p]+";",0) == string::npos )
		try{ del(prm_ls[i_p]); }
		catch( TError err )
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Deletion parameter '%s' is error but it no present on configuration or remote station."),prm_ls[i_p].c_str());
		}
    }

    //> Update try for archives start
    if( gAddPrm && SYS->archive().at().subStartStat() )
    {
	SYS->archive().at().valList(prm_ls);
	for( int i_a = 0; i_a < prm_ls.size(); i_a++ )
	{
	    AutoHD<TVArchive> archt = SYS->archive().at().valAt(prm_ls[i_a]);
	    if( archt.at().toStart() && !archt.at().startStat() )
		try{ archt.at().start(); } catch(...) { }
	}
    }
}

void TMdContr::disable_( )
{
    mStatWork.clear();
}

void TMdContr::start_( )
{
    if( prcSt ) return;

    //> Clear stations request counter
    for( int i_st = 0; i_st < mStatWork.size(); i_st++ ) mStatWork[i_st].second = 0;

    //> Start the gathering data task
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    struct sched_param prior;
    if( mPrior && SYS->user() == "root" )
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_RR);
    else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    prior.__sched_priority = mPrior;
    pthread_attr_setschedparam(&pthr_attr,&prior);

    pthread_create(&procPthr,&pthr_attr,TMdContr::Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait(prcSt, true, nodePath()+"start",5) )
	throw TError(nodePath().c_str(),_("Gathering task is not started!"));
}

void TMdContr::stop_( )
{
    if( !prcSt ) return;

    //> Stop the request and calc data task
    endrunReq = true;
    pthread_kill( procPthr, SIGALRM );
    if( TSYS::eventWait(prcSt,false,nodePath()+"stop",5) )
	throw TError(nodePath().c_str(),_("Gathering task is not stopped!"));
    pthread_join( procPthr, NULL );
}

void *TMdContr::Task( void *icntr )
{
    map<string,float>::iterator sti;
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrunReq = false;
    cntr.prcSt = true;


    for( unsigned int it_cnt = 0; !cntr.endrunReq; it_cnt++ )
    {
	if( cntr.redntUse( ) ) { usleep( STD_WAIT_DELAY*1000 ); continue; }

	long long t_cnt = TSYS::curTime();

	try
	{
	    ResAlloc res(cntr.enRes,false);

	    //> Allow stations presenting
	    bool isAccess = false;
	    for( int i_st = 0; i_st < cntr.mStatWork.size(); i_st++ )
		if( cntr.mStatWork[i_st].second > 0 ) cntr.mStatWork[i_st].second = vmax(0,cntr.mStatWork[i_st].second-cntr.period());
		else isAccess = true;

	    if( isAccess )
	    {
		unsigned int div = (unsigned int)(cntr.mSync/cntr.period());
		vector<string> pLS;
		cntr.list(pLS);
		AutoHD<TMdPrm> prm;
		string scntr;

		//> Parameters list update
		if( (it_cnt > div && it_cnt%div == 0) || !pLS.size() )
		    try { res.release( ); cntr.enable_( ); res.request(false); }
		    catch(TError err) { }

		//> Mark no process
		for( int i_p=0; i_p < pLS.size(); i_p++)
		{
		    if( it_cnt > div && (it_cnt+i_p)%div == 0 ) cntr.at(pLS[i_p]).at().load_();
		    cntr.at(pLS[i_p]).at().isPrcOK = false;
		}

		//> Station's cycle
		for( int i_st = 0; i_st < cntr.mStatWork.size(); i_st++ )
		{
		    if( cntr.mStatWork[i_st].second > 0 ) continue;
		    XMLNode req("CntrReqs"); req.setAttr("path","/"+cntr.mStatWork[i_st].first+"/DAQ/");

		    //> Put attributes reuests
		    for( int i_p=0; i_p < pLS.size(); i_p++)
		    {
			prm = cntr.at(pLS[i_p]);
			if( prm.at().isPrcOK ) continue;
			for( int c_off = 0; (scntr=TSYS::strSepParse(prm.at().cntrAdr(),0,';',&c_off)).size(); )
			{
			    if( TSYS::pathLev(scntr,0) != cntr.mStatWork[i_st].first ) continue;

			    XMLNode *prmNd = req.childAdd("get")->setAttr("path","/"+TSYS::pathLev(scntr,2)+"/"+TSYS::pathLev(scntr,3)+"/"+prm.at().id()+"/%2fserv%2fattr");

			    //>> Put archive request
			    if( !cntr.restDtTm() ) continue;
			    vector<string> listV;
			    prm.at().vlList(listV);
			    for( int iV = 0; iV < listV.size(); iV++ )
			    {
				AutoHD<TVal> vl = prm.at().vlAt(listV[iV]);
				if( vl.at().arch().freeStat() ) continue;
				prmNd->childAdd("ael")->setAttr("id",listV[iV])->setAttr("tm",TSYS::ll2str(vmax(vl.at().arch().at().end(""),TSYS::curTime()-(long long)(3.6e9*cntr.restDtTm()))));
			    }
			}
		    }
		    if( !req.childSize() ) continue;

		    //> Same request
		    if( cntr.cntrIfCmd(req) ) { mess_err(req.attr("mcat").c_str(),"%s",req.text().c_str()); continue; }

		    //> Result process
		    for( int i_r = 0; i_r < req.childSize(); i_r++ )
		    {
			XMLNode *prmNd = req.childGet(i_r);
			if( atoi(prmNd->attr("err").c_str()) ) continue;

			prm = cntr.at(TSYS::pathLev(prmNd->attr("path"),2));
			if( prm.at().isPrcOK ) continue;
			prm.at().isPrcOK = true;

			for( int i_a = 0; i_a < prmNd->childSize(); i_a++ )
			{
			    XMLNode *aNd = prmNd->childGet(i_a);
			    if( !prm.at().vlPresent(aNd->attr("id")) ) continue;
			    AutoHD<TVal> vl = prm.at().vlAt(aNd->attr("id"));

			    if( aNd->name() == "el" ) vl.at().setS(aNd->text(),cntr.restDtTm()?atoll(aNd->attr("tm").c_str()):0,true);
			    else if( aNd->name() == "ael" && !vl.at().arch().freeStat() && aNd->childSize() )
			    {
				long long btm = atoll(aNd->attr("tm").c_str());
				long long per = atoll(aNd->attr("per").c_str());
				TValBuf buf(vl.at().arch().at().valType(),0,per,false,true);
				for( int i_v = 0; i_v < aNd->childSize(); i_v++ )
				    buf.setS(aNd->childGet(i_v)->text(),btm+per*i_v);
				vl.at().arch().at().setVals(buf,buf.begin(),buf.end(),"");
			    }
			}
		    }
		}

		//> Mark no processed parameters to EVAL
		for( int i_p=0; i_p < pLS.size(); i_p++)
		{
		    prm = cntr.at(pLS[i_p]);
		    if( prm.at().isPrcOK ) continue;
		    vector<string> vLs;
		    prm.at().elem().fldList(vLs);
		    for( int i_v = 0; i_v < vLs.size(); i_v++ )
		    {
			if( vLs[i_v] == "ID" || vLs[i_v] == "NAME" || vLs[i_v] == "DESCR" ) continue;
			AutoHD<TVal> vl = prm.at().vlAt(vLs[i_v]);
			if( vl.at().getS() == EVAL_STR ) continue;
			vl.at().setS( EVAL_STR, vl.at().arch().freeStat() ? 0 : vmax(vl.at().arch().at().end(""),TSYS::curTime()-(long long)(3.6e9*cntr.restDtTm())), true );
		    }
		}
	    }
	    //res.release( );
	}catch(TError err)	{ mess_err(err.cat.c_str(),err.mess.c_str()); }

	//> Calc acquisition process time
	cntr.tmGath = 1e-3*(TSYS::curTime()-t_cnt);

	TSYS::taskSleep((long long)(1e9*cntr.period()));
    }


    cntr.prcSt = false;

    return NULL;
}

int TMdContr::cntrIfCmd( XMLNode &node )
{
    string reqStat = TSYS::pathLev(node.attr("path"),0);

    try
    {
	for( int i_st = 0; i_st < mStatWork.size(); i_st++ )
	    if( mStatWork[i_st].first == reqStat )
	    {
		if( mStatWork[i_st].second > 0 ) break;
		try
		{
		    int rez = SYS->transport().at().cntrIfCmd(node,MOD_ID+id());
		    mStatWork[i_st].second -= 1;
		    return rez;
		}
		catch(...){ mStatWork[i_st].second = mRestTm; throw; }
	    }
    }catch(TError err) { node.setAttr("mcat",err.cat)->setAttr("err","10")->setText(err.mess); }

    return atoi(node.attr("err").c_str());
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/STATIONS",cfg("STATIONS").fld().descr(),0664,"root","root",4,"tp","str","cols","100","rows","4",
	    "help",_("Remote OpenSCADA stations' identifiers list used into it controller."));
	ctrMkNode("fld",opt,-1,"/cntr/cfg/CNTRPRM",cfg("CNTRPRM").fld().descr(),0664,"root","root",4,"tp","str","cols","100","rows","4",
	    "help",_("Remote OpenSCADA full controller's or separated controller's parameters list. Address example:\n"
		     "  System.AutoDA - for controller;\n"
		     "  System.AutoDA.UpTimeStation - for controller's parameter."));
	ctrMkNode("comm",opt,-1,"/cntr/cfg/host_lnk",_("Go to remote stations list configuration"),0660,"root","root",1,"tp","lnk");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/host_lnk" && ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )
    {
	SYS->transport().at().setSysHost(true);
	opt->setText("/Transport");
    }
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : TParamContr(name,tp_prm), p_el("w_attr"), isPrcOK(false)
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

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::setCntrAdr( const string &vl )
{
    if( vl.empty() ) { mCntrAdr = ""; return; }

    string scntr;
    for( int off = 0; (scntr=TSYS::strSepParse(mCntrAdr,0,';',&off)).size(); )
	if( scntr == vl ) return;
    mCntrAdr += vl+";";
}

void TMdPrm::load_( )
{
    string scntr;
    XMLNode req("CntrReqs");
    //> Request and update attributes list
    for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
	try
	{
	    req.clear()->setAttr("path",scntr+id());
	    req.childAdd("get")->setAttr("path","/%2fprm%2fcfg%2fNAME");
	    req.childAdd("get")->setAttr("path","/%2fprm%2fcfg%2fDESCR");
	    req.childAdd("list")->setAttr("path","/%2fserv%2fattr");
	    if( owner().cntrIfCmd(req) ) throw TError(req.attr("mcat").c_str(),req.text().c_str());

	    setName(req.childGet(0)->text());
	    setDescr(req.childGet(1)->text());
	    //>> Check and create new attributes
	    for( int i_a = 0; req.childGet(2)->childSize(); i_a++ )
	    {
		XMLNode *ael = req.childGet(2)->childGet(i_a);
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
	    //>> Remove attributes
	    //????
	    return;
	}catch(TError err) { continue; }
}

void TMdPrm::save_( )
{
    //> Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(int i_a = 0; i_a < a_ls.size(); i_a++)
	if( !vlAt(a_ls[i_a]).at().arch().freeStat() )
	    vlAt(a_ls[i_a]).at().arch().at().save();
}

void TMdPrm::vlGet( TVal &val )
{
    if( val.name() == "err" && (!enableStat() || !owner().startStat()) ) TParamContr::vlGet(val);
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
    if( !enableStat() || !owner().startStat() )	valo.setI(EVAL_INT,0,true);
    if( valo.getS() == EVAL_STR || valo.getS() == pvl.getS() ) return;

    XMLNode req("set");

    //> Send to active reserve station
    if( owner().redntUse( ) )
    {
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",valo.name())->setText(valo.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }
    //> Direct write
    string scntr;
    for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
	try
	{
	    req.clear()->setAttr("path",scntr+id()+"/%2fserv%2fattr")->
		childAdd("el")->setAttr("id",valo.name())->setText(valo.getS());
	    if( owner().cntrIfCmd(req) )   throw TError(req.attr("mcat").c_str(),req.text().c_str());
	}catch(TError err) { continue; }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(1e6*owner().period()));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

TVal* TMdPrm::vlNew( )	{ return new TMdVl(); }

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //> Service commands process
    if( a_path.substr(0,6) == "/serv/" ) { TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
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
		//>> Get remote parameter's config section
		string scntr;
		XMLNode req("info");
		for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
		    try
		    {
			req.clear()->setAttr("path",scntr+id()+"/%2fprm%2fcfg");
			if( owner().cntrIfCmd(req) ) throw TError(req.attr("mcat").c_str(),req.text().c_str());
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
    //> Process command to page
    if( a_path == "/prm/st/type" && ctrChkNode(opt) )   opt->setText(type().descr);
    else if( a_path == "/prm/st/en" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )   opt->setText(enableStat()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    if( !owner().enableStat() ) throw TError(nodePath().c_str(),"Controller is not started!");
	    else atoi(opt->text().c_str())?enable():disable();
	}
    }
    else if( a_path == "/prm/st/id" && ctrChkNode(opt) )opt->setText(id());
    else if( a_path == "/prm/st/nm" && ctrChkNode(opt) )opt->setText(name());
    else if( a_path.substr(0,8) == "/prm/cfg" )
    {
	//> Request to remote host
	string scntr;
	for( int c_off = 0; (scntr=TSYS::strSepParse(cntrAdr(),0,';',&c_off)).size(); )
	    try
	    {
		opt->setAttr("path",scntr+id()+"/"+TSYS::strEncode(a_path,TSYS::PathEl));
		if( owner().cntrIfCmd(*opt) ) throw TError(opt->attr("mcat").c_str(),opt->text().c_str());
	    }catch(TError err) { continue; }
	opt->setAttr("path",a_path);
    }
    else TValue::cntrCmdProc(opt);
}

//******************************************************
//* TMdVl                                              *
//******************************************************
TMdPrm &TMdVl::owner( )	{ return *(dynamic_cast<TMdPrm*>(nodePrev())); }

void TMdVl::cntrCmdProc( XMLNode *opt )
{
    if( !arch( ).freeStat() ) { TVal::cntrCmdProc(opt); return; }

    string a_path = opt->attr("path");
    //> Service commands process
    if( a_path == "/serv/val" && owner().owner().restDtTm() )	//Values access
    {
	//>> Request to remote station
	string scntr;
	for( int c_off = 0; (scntr=TSYS::strSepParse(owner().cntrAdr(),0,';',&c_off)).size(); )
	    try
	    {
		opt->setAttr("path",scntr+owner().id()+"/"+name()+"/"+TSYS::strEncode(a_path,TSYS::PathEl));
		if( !owner().owner().cntrIfCmd(*opt) ) break;
	    }catch(TError err) { continue; }
	opt->setAttr("path",a_path);
	return;
    }

    TVal::cntrCmdProc(opt);
}

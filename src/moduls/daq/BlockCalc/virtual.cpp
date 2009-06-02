
//OpenSCADA system module DAQ.BlockCalc file: virtual.cpp
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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

#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <tconfig.h>
#include <tvalue.h>
#include <ttiparam.h>
#include "virtual.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"BlockCalc"
#define MOD_NAME	"Block based calculator"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"1.1.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow block based calculator."
#define LICENSE		"GPL"
//************************************************

Virtual::TipContr *Virtual::mod;

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
	    return new Virtual::TipContr( source );
	return NULL;
    }
}

using namespace Virtual;

//************************************************
//* TipContr - BlockCalc type controller         *
//************************************************
TipContr::TipContr( string name ) 
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

TipContr::~TipContr()
{
    nodeDelAll();
}

string TipContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
    return(buf);
}

void TipContr::load_()
{
    //========== Load parameters from command line ============
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //========== Load parameters from config file =============
}

void TipContr::postEnable( int flag )
{
    TTipDAQ::postEnable( flag );

    //Controllers BD structure
    fldAdd( new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30","system") );
    fldAdd( new TFld("BLOCK_SH",_("Block's table"),TFld::String,TFld::NoFlag,"30","block") );
    fldAdd( new TFld("PERIOD",_("Calc period (ms)"),TFld::Integer,TFld::NoFlag,"5","1000","0;10000") );
    fldAdd( new TFld("PRIOR",_("Calc task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("ITER",_("Iteration number into calc period"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );

    //Add parameter types
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("IO",_("Blocks' IOs"),TFld::String,TFld::FullText|TCfg::TransltText|TCfg::NoVal,"200") );

    //Blok's db structure
    blk_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    blk_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    blk_el.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"300") );
    blk_el.fldAdd( new TFld("FUNC",_("Function"),TFld::String,TFld::NoFlag,"75") );
    blk_el.fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,TFld::NoFlag,"1","0") );
    blk_el.fldAdd( new TFld("PROC",_("To process"),TFld::Boolean,TFld::NoFlag,"1","0") );

    //IO blok's db structure
    blkio_el.fldAdd( new TFld("BLK_ID",_("Blok's ID"),TFld::String,TCfg::Key,"20") );
    blkio_el.fldAdd( new TFld("ID",_("IO ID"),TFld::String,TCfg::Key,"20") );
    blkio_el.fldAdd( new TFld("TLNK",_("Link's type"),TFld::Integer,TFld::NoFlag,"2") );
    blkio_el.fldAdd( new TFld("LNK",_("Link"),TFld::String,TFld::NoFlag,"50") );
    blkio_el.fldAdd( new TFld("VAL",_("Link's value"),TFld::String,TFld::NoFlag,"20") );
}

void TipContr::preDisable(int flag)
{
    vector<string> lst;
    list(lst);
    for(int i_cnt = 0; i_cnt < lst.size(); i_cnt++)
	if( at(lst[i_cnt]).at().startStat() )
	    at(lst[i_cnt]).at().stop( );
    for(int i_cnt = 0; i_cnt < lst.size(); i_cnt++)
	if( at(lst[i_cnt]).at().enableStat() )
	    at(lst[i_cnt]).at().disable( );
}

TController *TipContr::ContrAttach( const string &name, const string &daq_db )
{
    return new Contr(name,daq_db,this);
}

void TipContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TTipDAQ::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else TTipDAQ::cntrCmdProc(opt);
}

//************************************************
//* Contr - Blocks and parameters container      *
//************************************************ 
Contr::Contr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    ::TController(name_c, daq_db, cfgelem), prc_st(false), endrun_req(false), sync_st(false), tm_calc(0.0),
    m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), m_iter(cfg("ITER").getId())
{
    cfg("PRM_BD").setS("BlckCalcPrm_"+name_c);
    cfg("BLOCK_SH").setS("BlckCalcBlcks_"+name_c);
    m_bl = grpAdd("blk_");
}

Contr::~Contr( )
{

}

TCntrNode &Contr::operator=( TCntrNode &node )
{
    TController::operator=(node);

    Contr *src_n = dynamic_cast<Contr*>(&node);
    if( !src_n ) return *this;

    //- Blocks copy -
    if( src_n->enableStat( ) )
    {
	if( !enableStat( ) )    enable();

	//-- Blocks copy --
	vector<string> ls;
	src_n->blkList(ls);
	for( int i_l = 0; i_l < ls.size(); i_l++ )
	{
	    if( !blkPresent(ls[i_l]) ) blkAdd(ls[i_l]);
	    (TCntrNode&)blkAt(ls[i_l]).at() = (TCntrNode&)src_n->blkAt(ls[i_l]).at();
	}
    }

    return *this;
}

string Contr::getStatus( )
{
    string rez = TController::getStatus( );
    if( startStat() && !redntUse( ) ) rez += TSYS::strMess(_("Calc time %g us. "),tm_calc);
    return rez;
}

void Contr::postDisable(int flag)
{
    if( run_st ) stop();
    try
    {
	if( flag )
	{
	    //Delete parameter's tables
	    string wbd = DB()+"."+cfg("BLOCK_SH").getS();
	    SYS->db().at().open(wbd);
	    SYS->db().at().close(wbd,true);

	    wbd=wbd+"_io";
	    SYS->db().at().open(wbd);
	    SYS->db().at().close(wbd,true);
	}
    }catch(TError err)
    { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }

    TController::postDisable(flag);
}

TipContr &Contr::owner( )	{ return (TipContr&)TController::owner( ); }

void Contr::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;

    TController::load_( );

    //- Load block's configuration -
    TConfig c_el(&mod->blockE());
    c_el.cfgViewAll(false);
    string bd = DB()+"."+cfg("BLOCK_SH").getS();

    for( int fld_cnt = 0; SYS->db().at().dataSeek(bd,mod->nodePath()+cfg("BLOCK_SH").getS(),fld_cnt++,c_el); )
    {
	string id = c_el.cfg("ID").getS();
	if( !chldPresent(m_bl,id) )
	{
	    blkAdd(id);
	    ((TConfig &)blkAt(id).at()) = c_el;
	}
	blkAt(id).at().load();
    }
}

void Contr::enable_( )
{
    //Enable blocks
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	if( blkAt(lst[i_l]).at().toEnable() )
	try{ blkAt(lst[i_l]).at().setEnable(true); }
	catch(TError err)
	{
	    mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_warning(nodePath().c_str(),_("Enable block <%s> error."),lst[i_l].c_str());
	}
}

void Contr::disable_( )
{
    //Disable blocks
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	if( blkAt(lst[i_l]).at().enable() )
	try{ blkAt(lst[i_l]).at().setEnable(false); }
	    catch(TError err)
	    {
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Enable block <%s> error."),lst[i_l].c_str());
	    }
}

void Contr::start_( )
{
    //- Make process all bloks -
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	if( blkAt(lst[i_l]).at().enable() && blkAt(lst[i_l]).at().toProcess() )
	try{ blkAt(lst[i_l]).at().setProcess(true); }
	catch(TError err)
	{
	    mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_warning(nodePath().c_str(),_("Process block <%s> error."),lst[i_l].c_str());
	}

    //- Start the request and calc data task -
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

	pthread_create(&calcPthr,&pthr_attr,Contr::Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait(prc_st, true, nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),_("Acquisition task is not started!"));
    }
}

void Contr::stop_( )
{
    //- Stop the request and calc data task -
    if( prc_st )
    {
	endrun_req = true;
	pthread_kill( calcPthr, SIGALRM );
	if( TSYS::eventWait(prc_st,false,nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),_("Acquisition task is not stopped!"));
	pthread_join( calcPthr, NULL );
    }

    //- Make deprocess all blocks -
    vector<string> lst;
    blkList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	if( blkAt(lst[i_l]).at().process() )
	    blkAt(lst[i_l]).at().setProcess(false);
}

void *Contr::Task( void *icontr )
{
    Contr &cntr = *(Contr *)icontr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    bool is_start = true;
    bool is_stop  = false;

    while(true)
    {
	//Check calk time
	long long t_cnt = TSYS::curTime();

	cntr.hd_res.resRequestR( );
	for( unsigned i_it = 0; i_it < cntr.m_iter && !cntr.redntUse(); i_it++ )
	    for( unsigned i_blk = 0; i_blk < cntr.clc_blks.size(); i_blk++ )
	    {
		try{ cntr.clc_blks[i_blk].at().calc(is_start,is_stop); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    string blck = cntr.clc_blks[i_blk].at().id();
		    mess_err(cntr.nodePath().c_str(),_("Block <%s> calc error."),blck.c_str());
		    if( cntr.clc_blks[i_blk].at().errCnt() < 10 ) continue;
		    cntr.hd_res.resRelease( );
		    mess_err(cntr.nodePath().c_str(),_("Block <%s> is stoped."),blck.c_str());
		    cntr.blkAt(blck).at().setProcess(false);
		    cntr.hd_res.resRequestR( );
		}
	    }
	cntr.hd_res.resRelease( );

	cntr.tm_calc = TSYS::curTime()-t_cnt;

	if(is_stop) break;

	TSYS::taskSleep((long long)cntr.period()*1000000);

	if(cntr.endrun_req) is_stop = true;
	is_start = false;
    }

    cntr.prc_st = false;

    return NULL;
}

void Contr::redntDataUpdate( bool firstArchiveSync )
{
    TController::redntDataUpdate(firstArchiveSync);

    vector<string> bls; blkList(bls);

    //> Request for template's attributes values
    XMLNode req("CntrReqs"); req.setAttr("path",nodePath(0,true));
    for( int i_b = 0; i_b < bls.size(); i_b++ )
    {
	if( !blkAt(bls[i_b]).at().enable( ) ) continue;
	req.childAdd("get")->setAttr("path","/blk_"+bls[i_b]+"/%2fserv%2fattr");
    }

    //> Send request to first active station for this controller
    try{ owner().owner().rdStRequest(workId(),req); }
    catch(TError err) { return; }

    //> Redirect respond to local parameters
    req.setAttr("path","/");
    for( int i_b = 0; i_b < req.childSize(); i_b++ )
    {
	if( atoi(req.childGet(i_b)->attr("err").c_str()) ) { req.childDel(i_b--); continue; }
	req.childGet(i_b)->setName("set");
    }
    cntrCmd(&req);
}


TParamContr *Contr::ParamAttach( const string &name, int type )
{
    return new Prm(name,&owner().tpPrmAt(type));
}

void Contr::blkAdd( const string &iid )
{
    if( chldPresent(m_bl,iid) ) return;
    chldAdd(m_bl,new Block( iid, this ));
}

void Contr::blkProc( const string & id, bool val )
{
    unsigned i_blk;

    ResAlloc res(hd_res,true);
    for( i_blk = 0; i_blk < clc_blks.size(); i_blk++)
	if( clc_blks[i_blk].at().id() == id ) break;

    if( val && i_blk >= clc_blks.size() ) clc_blks.push_back(blkAt(id));
    if( !val && i_blk < clc_blks.size() ) clc_blks.erase(clc_blks.begin()+i_blk);
}

void Contr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/blk_",_("Block"),0664,"root","root",2,"idm","1","idSz","20");
	if(ctrMkNode("area",opt,-1,"/scheme",_("Blocks scheme")))
	{
	    ctrMkNode("fld",opt,-1,"/scheme/nmb",_("Number"),0444,"root","root",1,"tp","str");
	    ctrMkNode("list",opt,-1,"/scheme/sch",_("Blocks"),0664,"root","root",5,"tp","br","idm","1","s_com","add,del","br_pref","blk_","idSz","20");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/br/blk_" || a_path == "/scheme/sch" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> lst;
	    blkList(lst);
	    for( unsigned i_f=0; i_f < lst.size(); i_f++ )
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(blkAt(lst[i_f]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::ID);
	    blkAdd(vid); blkAt(vid).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	chldDel(m_bl,opt->attr("id"),-1,1);
    }
    else if( a_path == "/scheme/nmb" && ctrChkNode(opt) )
    {
	vector<string> lst;
	blkList(lst);
	int enCnt = 0, prcCnt = 0;
	for( int i_b = 0; i_b < lst.size(); i_b++ )
	{
	    if( blkAt(lst[i_b]).at().enable( ) )	enCnt++;
	    if( blkAt(lst[i_b]).at().process( ) )	prcCnt++;
	}
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d; Process: %d"),lst.size(),enCnt,prcCnt));
    }
    else TController::cntrCmdProc(opt);
}

//************************************************
//* Prm - parameters for access to data          *
//*       of calced blocks                       *
//************************************************
Prm::Prm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), v_el(name)
{

}

Prm::~Prm()
{
    nodeDelAll();
}

void Prm::postEnable( int flag )
{
    TParamContr::postEnable( flag );
    if(!vlElemPresent(&v_el))	vlElemAtt(&v_el);
}

Contr &Prm::owner( )	{ return (Contr&)TParamContr::owner( ); }

void Prm::enable()
{
    if( enableStat() )  return;

    //> Check and delete no used fields
    for(int i_fld = 0; i_fld < v_el.fldSize(); i_fld++)
    {
	string fel;
	for( int io_off = 0; (fel=TSYS::strSepParse(cfg("IO").getS(),0,'\n',&io_off)).size(); )
	    if( TSYS::strSepParse(fel,0,':') == v_el.fldAt(i_fld).reserve() ) break;
	if( fel.empty() )
	{
	    try{ v_el.fldDel(i_fld); i_fld--; }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }

    //> Init elements
    vector<string> pls;
    AutoHD<Block> blk;
    int io;
    string mio, ioaddr, ioblk, ioid, aid, anm;
    for( int io_off = 0; (mio=TSYS::strSepParse(cfg("IO").getS(),0,'\n',&io_off)).size(); )
    {
	ioaddr = TSYS::strSepParse(mio,0,':');
	ioblk  = TSYS::strSepParse(ioaddr,0,'.');
	ioid   = TSYS::strSepParse(ioaddr,1,'.');
	aid    = TSYS::strSepParse(mio,1,':');
	anm    = TSYS::strSepParse(mio,2,':');
	if( aid.empty() ) aid = ioblk+"_"+ioid;
	if( !((Contr&)owner()).blkPresent(ioblk) ) continue;
	blk = ((Contr&)owner()).blkAt(ioblk);
	if( (io=blk.at().ioId(ioid)) < 0 )	continue;

	unsigned	flg = TVal::DirWrite|TVal::DirRead;
	TFld::Type	tp  = TFld::String;
	string		def;
	switch( blk.at().ioType(io) )
	{
	    case IO::String:	tp = TFld::String; def = EVAL_STR; break;
	    case IO::Integer:	tp = TFld::Integer; def = TSYS::int2str(EVAL_INT);	break;
	    case IO::Real:	tp = TFld::Real; def = TSYS::real2str(EVAL_REAL);	break;
	    case IO::Boolean:	tp = TFld::Boolean; def = TSYS::int2str(EVAL_BOOL);	break;
	}
	if( !v_el.fldPresent(aid) || v_el.fldAt(v_el.fldId(aid)).type() != tp ||
	    v_el.fldAt(v_el.fldId(aid)).flg() != flg )
	{
	    if(v_el.fldPresent(aid)) v_el.fldDel(v_el.fldId(aid));
	    v_el.fldAdd( new TFld(aid.c_str(),"",tp,flg,"",def.c_str()) );
	}
	int el_id = v_el.fldId(aid);
	v_el.fldAt(el_id).setDescr( anm.empty() ? blk.at().func()->io(io)->name() : anm );
	v_el.fldAt(el_id).setReserve( ioaddr );

	pls.push_back(aid);
    }

    //> Check and delete no used attrs
    for( int i_fld = 0; i_fld < v_el.fldSize(); i_fld++ )
    {
	int i_p;
	for( i_p = 0; i_p < pls.size(); i_p++ )
	    if( pls[i_p] == v_el.fldAt(i_fld).name() )	break;
	if( i_p < pls.size() )	continue;
	try{ v_el.fldDel(i_fld); i_fld--; }
	catch(TError err)
	{ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }

    TParamContr::enable();
}

void Prm::disable()
{
    if( !enableStat() )  return;

    TParamContr::disable();
}

void Prm::vlSet( TVal &val, const TVariant &pvl )
{
    if( !enableStat() )	return;

    //> Send to active reserve station
    if( owner().redntUse( ) )
    {
	if( val.getS() == pvl.getS() ) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",val.name())->setText(val.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //> Direct write
    try
    {
	AutoHD<Block> blk = ((Contr &)owner()).blkAt(TSYS::strSepParse(val.fld().reserve(),0,'.'));
	int io_id = blk.at().ioId(TSYS::strSepParse(val.fld().reserve(),1,'.'));
	if( io_id < 0 ) disable();
	else
	    switch(val.fld().type())
	    {
		case TFld::String:	blk.at().setS(io_id,val.getS(0,true));	break;
		case TFld::Integer:	blk.at().setI(io_id,val.getI(0,true));	break;
		case TFld::Real:	blk.at().setR(io_id,val.getR(0,true));	break;
		case TFld::Boolean:	blk.at().setB(io_id,val.getB(0,true));	break;
	    }
    }catch(TError err) { disable(); }
}

void Prm::vlGet( TVal &val )
{
    if( val.name() == "err" )
    {
	if( !owner().startStat() )
	    val.setS(_("2:Controller is stoped"),0,true);
	else if( !enableStat() )
	    val.setS(_("1:Parameter is disabled"),0,true);
	else val.setS("0",0,true);
	return;
    }

    if( owner().redntUse( ) ) return;

    try
    {
	//if( !enableStat() ) return;
	AutoHD<Block> blk = ((Contr &)owner()).blkAt(TSYS::strSepParse(val.fld().reserve(),0,'.'));
	int io_id = blk.at().ioId(TSYS::strSepParse(val.fld().reserve(),1,'.'));
	if( io_id < 0 )	disable();
	else
	    switch(val.fld().type())
	    {
		case TFld::String:
		    val.setS( (enableStat()&&owner().startStat()) ? blk.at().getS(io_id) : EVAL_STR, 0, true );	break;
		case TFld::Integer:
		    val.setI( (enableStat()&&owner().startStat()) ? blk.at().getI(io_id) : EVAL_INT, 0, true );	break;
		case TFld::Real:
		    val.setR( (enableStat()&&owner().startStat()) ? blk.at().getR(io_id) : EVAL_REAL, 0, true );break;
		case TFld::Boolean:
		    val.setB( (enableStat()&&owner().startStat()) ? blk.at().getB(io_id) : EVAL_BOOL, 0, true );break;
	    }
    }catch(TError err) { disable(); }
}

void Prm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
	val.arch().at().setSrcMode(TVArchive::ActiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(1000000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( false );
}

void Prm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/IO",cfg("IO").fld().descr(),0664,"root","root",1,
	    "help",_("Attributes configuration list. List must be written by lines in format: [<blk>.<blk_io>:<aid>:<anm>]\n"
	    "Where:\n"
	    "  blk - block identifier from block's scheme;\n"
	    "  blk_io - block's parameter from block's scheme;\n"
	    "  aid - created attribute identifier;\n"
	    "  anm - created attribute name.\n"
	    "If 'aid' or 'anm' are not set they will be generated from selected block's parameter."));
	return;
    }
    //Process command to page
    TParamContr::cntrCmdProc(opt);
}

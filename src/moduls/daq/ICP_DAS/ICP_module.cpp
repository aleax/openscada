
//OpenSCADA system module DAQ.ICP_DAS file: ICP_module.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

extern "C"
{
#include "msw.h"
}

#include <tsys.h>
#include <ttiparam.h>

#include "ICP_module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"ICP_DAS"
#define MOD_NAME	"ICP DAS hardware"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.5.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow realisation of ICP DAS hardware support. Include I87000 and I-7000 DCON modules and I-8000 fast modules."
#define LICENSE		"GPL"
//*************************************************

ICP_DAS_DAQ::TTpContr *ICP_DAS_DAQ::mod;  //Pointer for direct access to module

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
	    return new ICP_DAS_DAQ::TTpContr( source );
	return NULL;
    }
}

using namespace ICP_DAS_DAQ;

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

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable( flag );

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0.01;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );
    fldAdd( new TFld("BUS",_("Bus"),TFld::Integer,TFld::NoFlag,"2","1") );
    fldAdd( new TFld("BAUD",_("Baudrate"),TFld::Integer,TFld::NoFlag,"6","115200") );
    fldAdd( new TFld("LP_PRMS",_("LinPAC parameters"),TFld::String,TFld::FullText,"1000") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_TP",_("Module type"),TFld::Integer,TFld::HexDec|TCfg::NoVal,"10","552985") );
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_ADDR",_("Module address"),TFld::Integer,TCfg::NoVal,"3","0","0;255") );
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_SLOT",_("Module slot"),TFld::Integer,TCfg::NoVal,"2","1","1;11") );
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_PRMS",_("Module addon parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"1000") );
}

void TTpContr::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt = "h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind = opterr = 0;
    do
    {
	next_opt = getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch( next_opt )
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while( next_opt != -1 );
}

void TTpContr::save_()
{

}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}


//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
	TController( name_c, daq_db, cfgelem ), prcSt(false), endRunReq(false), tm_gath(0),
	mPer(cfg("PERIOD").getRd()), mPrior(cfg("PRIOR").getId()), mBus(cfg("BUS").getId()), mBaud(cfg("BAUD").getId()), mLPprms(cfg("LP_PRMS").getSd())
{
    cfg("PRM_BD").setS("ICPDASPrm_"+name_c);
    cfg("BUS").setI(1);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );

    if( startStat( ) && !redntUse( ) ) val += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm( name, &owner().tpPrmAt(type) );
}

void TMdContr::disable_( )
{

}

void TMdContr::start_( )
{
    if( !prcSt )
    {
	if( mBus == 0 && Open_SlotAll() > 0 ) throw TError( nodePath().c_str(), _("Open All LP-slots error.") );

	if( Open_Com( (mBus?mBus:1), mBaud, Data8Bit, NonParity, OneStopBit ) > 0 )
	    throw TError( nodePath().c_str(), _("Open COM%d port error."), (mBus?mBus:1) );

	//> Start the gathering data task
	SYS->taskCreate( nodePath('.',true), mPrior, TMdContr::Task, this, &prcSt );
    }
}

void TMdContr::stop_( )
{
    if( prcSt )
    {
	//> Stop the request and calc data task
	SYS->taskDestroy( nodePath('.',true), &prcSt, &endRunReq );

	Close_Com( (mBus?mBus:1) );

	if( mBus == 0 )	Close_SlotAll();
    }
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if( icfg.name() == "BUS" )
    {
	cfg("BAUD").setView( icfg.getI() != 0 );
	if( startStat() ) stop();
    }
    else if( icfg.name() == "BAUD" && startStat( ) ) stop( );

    return true;
}

string TMdContr::prmLP( const string &prm )
{
    XMLNode prmNd;
    try { prmNd.load(mLPprms); return prmNd.attr(prm); } catch(...){ }

    return "";
}

void TMdContr::setPrmLP( const string &prm, const string &vl )
{
    XMLNode prmNd;
    try { prmNd.load(mLPprms); } catch(...){ }
    prmNd.setAttr(prm,vl);
    mLPprms = prmNd.save(XMLNode::BrAllPast);
    modif();
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res( en_res, true );
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++ )
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr*)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endRunReq = false;
    cntr.prcSt = true;

    float wTm = 0;
    //> Init watchdog and get previous state
    if( cntr.mBus == 0 ) wTm = atof(cntr.prmLP("wTm").c_str());

    try
    {
	while( !cntr.endRunReq )
	{
	    if( !cntr.redntUse( ) )
	    {
		long long t_cnt = TSYS::curTime();

		//> Update controller's data
		ResAlloc res( cntr.en_res, false );
		for( int i_p = 0; i_p < cntr.p_hd.size(); i_p++ ) cntr.p_hd[i_p].at().getVals( );
		res.release();

		//> Calc acquisition process time
		cntr.tm_gath = 1e-3*(TSYS::curTime()-t_cnt);
	    }

	    //> Watchdog timer process
	    if( cntr.mBus == 0 && wTm > 0 ) { ResAlloc res( cntr.reqRes, true ); EnableWDT((int)(1e3*vmax(1.5*cntr.period(),wTm))); res.release(); }

	    //> Calc next work time and sleep
	    TSYS::taskSleep((long long)(1e9*cntr.period()));
	}
    }
    catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

    //> Watchdog timer disable
    if( cntr.mBus == 0 && wTm > 0 ) { ResAlloc res( cntr.reqRes, true ); DisableWDT(); res.release(); }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/BUS",cfg("BUS").fld().descr(),0664,"root","root",3,"tp","dec","dest","select","select","/cntr/cfg/busLst");
	if( cfg("BAUD").view() )
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/BAUD",cfg("BAUD").fld().descr(),0664,"root","root",3,"tp","dec","dest","sel_ed","select","/cntr/cfg/boudLst");
	if( mBus == 0 && ctrMkNode("area",opt,-1,"/LPcfg","LinPAC") )
	{
	    if( startStat() )
	    {
		ctrMkNode("fld",opt,-1,"/LPcfg/sn",_("Serial number"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/LPcfg/SDKv",_("SDK version"),R_R_R_,"root","DAQ",1,"tp","str");
		//ctrMkNode("fld",opt,-1,"/LPcfg/RSW",_("Rotary switch ID"),R_R_R_,"root","DAQ",1,"tp","dec");
		//ctrMkNode("fld",opt,-1,"/LPcfg/dipSW",_("DIP switch"),R_R_R_,"root","DAQ",1,"tp","hex");
	    }
	    ctrMkNode("fld",opt,-1,"/LPcfg/wTm",_("Watchdog timeout (s)"),RWRWR_,"root","DAQ",1,"tp","real");
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/busLst" && ctrChkNode(opt) )
	for( int i_s = 0; i_s < 11; i_s++ )
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(i_s))->setText("COM "+TSYS::int2str(i_s?i_s:1)+(i_s==0?_(" (Master LP-8x81)"):""));
    else if( a_path == "/cntr/cfg/boudLst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setText("300");
	opt->childAdd("el")->setText("600");
	opt->childAdd("el")->setText("1200");
	opt->childAdd("el")->setText("2400");
	opt->childAdd("el")->setText("4800");
	opt->childAdd("el")->setText("9600");
	opt->childAdd("el")->setText("19200");
	opt->childAdd("el")->setText("38400");
	opt->childAdd("el")->setText("57600");
	opt->childAdd("el")->setText("115200");
	opt->childAdd("el")->setText("230400");
	opt->childAdd("el")->setText("460800");
	opt->childAdd("el")->setText("500000");
	opt->childAdd("el")->setText("576000");
	opt->childAdd("el")->setText("921600");
    }
    else if( mBus == 0 && startStat() && a_path == "/LPcfg/sn" && ctrChkNode(opt) )
    {
	ResAlloc res( reqRes, true );
	unsigned char sN[8];
	Read_SN(sN);
	opt->setText( TSYS::strMess("%x%x%x%x%x%x%x%x",sN[0],sN[1],sN[2],sN[3],sN[4],sN[5],sN[6],sN[7]) );
    }
    else if( mBus == 0 && startStat() && a_path == "/LPcfg/SDKv" && ctrChkNode(opt) )
    {
	ResAlloc res( reqRes, true );
	opt->setText( TSYS::strMess("%4.2f",GetSDKversion()) );
    }
    /*else if( mBus == 0 && startStat() && a_path == "/LPcfg/RSW" && ctrChkNode(opt) )
    {
	ResAlloc res( reqRes, true );
	opt->setText( TSYS::int2str(GetRotaryID(0)) );
    }
    else if( mBus == 0 && startStat() && a_path == "/LPcfg/dipSW" && ctrChkNode(opt) )
    {
	ResAlloc res( reqRes, true );
	opt->setText( TSYS::int2str(GetDIPswitch()) );
    }*/
    else if( mBus == 0 && a_path == "/LPcfg/wTm" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )	opt->setText( prmLP("wTm") );
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )	setPrmLP("wTm",opt->text());
    }
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr( name, tp_prm ), p_el("w_attr"), extPrms(NULL), endRunReq(false), prcSt(false), clcCnt(0), wTm(0),
    modTp(cfg("MOD_TP").getId()), modAddr(cfg("MOD_ADDR").getId()), modSlot(cfg("MOD_SLOT").getId()), modPrms(cfg("MOD_PRMS").getSd())
{

}

TMdPrm::~TMdPrm( )
{
    disable( );
    nodeDelAll( );
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if( !vlElemPresent(&p_el) )	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    int wRetVal;
    if( enableStat() )	return;

    TParamContr::enable();

    loadExtPrms( );

    //> Delete DAQ parameter's attributes
    while( p_el.fldSize() > 0 )
    {
	try{ p_el.fldDel(0); }
	catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    //> Make DAQ parameter's attributes
    switch( modTp )
    {
	case 0x8017:
	    for( int i_i = 0; i_i < 8; i_i++ )
	    {
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
		p_el.fldAdd( new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
		p_el.fldAdd( new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    }
	    break;
	case 0x8042:
	    for( int i_i = 0; i_i < 16; i_i++ )
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Boolean,TFld::NoWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    for( int i_o = 0; i_o < 16; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    break;
	case 0x87019:
	    p_el.fldAdd( new TFld("cvct",_("Cold-Junction Compensation(CJC) temperature"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
	    for( int i_i = 0; i_i < 8; i_i++ )
	    {
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
		p_el.fldAdd( new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
		p_el.fldAdd( new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    }
	    break;
	case 0x87024:
	    for( int i_o = 0; i_o < 4; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Real,TVal::DirWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
	    break;
	case 0x87057:
	    for( int i_o = 0; i_o < 16; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    break;
	default: break;
    }

    owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS( EVAL_STR, 0, true );

    //> Stop fast task
    if( prcSt ) SYS->taskDestroy( nodePath('.',true), &prcSt, &endRunReq );

    //> Free module object
    switch( modTp )
    {
	case 0x8017:	delete ((PrmsI8017*)extPrms); extPrms = NULL; break;
    }
}

void TMdPrm::loadExtPrms( )
{
    if( !enableStat() )	return;

    XMLNode prmNd;
    try{ prmNd.load(modPrms); } catch(...){ }
    string  vl;

    switch( modTp )
    {
	case 0x8017:
	    if( !extPrms ) extPrms = new PrmsI8017();
	    vl = prmNd.attr("cnls"); if( !vl.empty() ) ((PrmsI8017*)extPrms)->prmNum = vmin(8,vmax(0,atoi(vl.c_str())));
	    vl = prmNd.attr("fastPer"); if( !vl.empty() ) ((PrmsI8017*)extPrms)->fastPer = atof(vl.c_str());
	    for( int i_n = 0; i_n < prmNd.childSize(); i_n++ )
		if( prmNd.childGet(i_n)->name() == "cnl" )
		    ((PrmsI8017*)extPrms)->cnlMode[atoi(prmNd.childGet(i_n)->attr("id").c_str())] = atoi(prmNd.childGet(i_n)->text().c_str());
	    break;
	case 0x87024: case 0x87057:
	    vl = prmNd.attr("wTm"); if( !vl.empty() ) wTm = vmin(25.5,vmax(0,atof(vl.c_str())));
	    break;
    }
}

void TMdPrm::saveExtPrms( )
{
    if( !enableStat() || !extPrms )	return;

    XMLNode prmNd("prms");

    switch( modTp )
    {
	case 0x8017:
	    prmNd.setAttr("cnls",TSYS::int2str(((PrmsI8017*)extPrms)->prmNum));
	    prmNd.setAttr("fastPer",TSYS::real2str(((PrmsI8017*)extPrms)->fastPer,5));
	    for( int i_c = 0; i_c < 8; i_c++ )
		prmNd.childAdd("cnl")->setAttr("id",TSYS::int2str(i_c))->setText(TSYS::int2str(((PrmsI8017*)extPrms)->cnlMode[i_c]));
	    break;
	case 0x87024: case 0x87057:
	    prmNd.setAttr("wTm",TSYS::real2str(wTm));
	    break;
    }

    modPrms = prmNd.save(XMLNode::BrAllPast);
    modif();
}

void TMdPrm::getVals( )
{
    int RetValue;
    WORD wT;

    switch( modTp )
    {
	case 0x8017:
	{
	    //> Check for I8017 init
	    if( !((PrmsI8017*)extPrms)->init ) { I8017_Init(modSlot); ((PrmsI8017*)extPrms)->init = true; }
	    //> Check for I8017 fast task start
	    if( ((PrmsI8017*)extPrms)->fastPer && !prcSt ) SYS->taskCreate( nodePath('.',true), 32, TMdPrm::fastTask, this, &prcSt );
	    //> Get values direct
	    for( int i_v = 0; i_v < 8; i_v++ )
		if( i_v >= ((PrmsI8017*)extPrms)->prmNum ) vlAt(TSYS::strMess("i%d",i_v)).at().setR(EVAL_REAL,0,true);
		else if( !((PrmsI8017*)extPrms)->fastPer )
		{
		    I8017_SetChannelGainMode(modSlot,i_v,((PrmsI8017*)extPrms)->cnlMode[i_v],0);
		    vlAt(TSYS::strMess("i%d",i_v)).at().setR( I8017_GetCurAdChannel_Float_Cal(modSlot), 0, true );
		}
	    if( ((PrmsI8017*)extPrms)->fastPer )
	    {
		acq_err.setVal( (((PrmsI8017*)extPrms)->prevCnt==((PrmsI8017*)extPrms)->curCnt) ? _("10:Fast acquisition task is stoped or locked.") : "" );
		((PrmsI8017*)extPrms)->prevCnt = ((PrmsI8017*)extPrms)->curCnt;
	    }
	    break;
	}
	case 0x8042:
	{
	    ResAlloc res( owner().reqRes, true );
	    int c_vl = DI_16(modSlot);
	    for( int i_v = 0; i_v < 16; i_v++ ) vlAt(TSYS::strMess("i%d",i_v)).at().setB( !((c_vl>>i_v)&0x01), 0, true );
	    c_vl = DO_16_RB(modSlot);
	    for( int o_v = 0; o_v < 16; o_v++ ) vlAt(TSYS::strMess("o%d",o_v)).at().setB( (c_vl>>o_v)&0x01, 0, true );
	    break;
	}
	case 0x87019:
	{
	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	    char szReceive[255];

	    //> Read inputs
	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("#%02X",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	    for( int i_v = 0; i_v < 8; i_v++ )
		vlAt(TSYS::strMess("i%d",i_v)).at().setR( (RetValue||szReceive[0]!='>') ? EVAL_REAL : atof(szReceive+1+7*i_v), 0, true );

	    //> Read Cold-Junction Compensation(CJC) temperature
	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("$%02X3",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	    vlAt("cvct").at().setR( (RetValue||szReceive[0]!='>') ? EVAL_REAL : atof(szReceive+1), 0, true );
	    acq_err.setVal(RetValue?_("10:Request to module error."):((szReceive[0]!='>')?_("11:Respond from module error."):""));
	    break;
	}
	case 0x87024:
	{
	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	    char szReceive[20];

	    //> Host watchdog processing
	    if( wTm >= 0.1 )
	    {
		RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X31%02X",(owner().mBus==0)?0:modAddr,(int)(10*wTm)).c_str(), szReceive, 1, 0, &wT );
		if( RetValue ) { acq_err.setVal(_("10:Request to module error.")); break; }
	    }

	    //> Get data
	    for( int i_v = 0; i_v < 4; i_v++ )
	    {
		RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("$%02X8%d",(owner().mBus==0)?0:modAddr,i_v).c_str(), szReceive, 1, 0, &wT );
		vlAt(TSYS::strMess("o%d",i_v)).at().setR( (RetValue||szReceive[0]!='!' ? EVAL_REAL : atof(szReceive+3)), 0, true );
	    }
	    acq_err.setVal(RetValue?_("10:Request to module error."):"");
	    break;
	}
	case 0x87057:
	{
	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	    char szReceive[20];

	    //> Host watchdog processing
	    if( wTm >= 0.1 )
	    {
		RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X31%02X",(owner().mBus==0)?0:modAddr,(int)(10*wTm)).c_str(), szReceive, 1, 0, &wT );
		if( RetValue ) { acq_err.setVal(_("10:Request to module error.")); break; }
	    }

	    //> Get data
	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("$%02X6",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	    int vl = -1;
	    if( !(RetValue||szReceive[0]!='!') )
	    {
		vl = strtoul(string(szReceive+1,4).c_str(),NULL,16);
		acq_err.setVal("");
	    }
	    else acq_err.setVal(RetValue?_("10:Request to module error."):_("11:Respond from module error."));

	    for( int i_v = 0; i_v < 16; i_v++ )
		vlAt(TSYS::strMess("o%d",i_v)).at().setB( (vl<0) ? EVAL_BOOL : (vl>>i_v)&0x01, 0, true );
	    break;
	}
    }
}

bool TMdPrm::cfgChange( TCfg &icfg )
{
    TParamContr::cfgChange(icfg);

    if( (icfg.name() == "MOD_TP" || icfg.name() == "MOD_ADDR" || icfg.name() == "MOD_SLOT") && enableStat() ) disable( );

    return true;
}

void TMdPrm::vlGet( TVal &val )
{
    if( !enableStat() || !owner().startStat() )
    {
	if( val.name() == "err" )
	{
	    if( !enableStat() )			val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition is stoped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

    if( owner().redntUse( ) ) return;

    if( val.name() == "err" )
    {
	if( acq_err.getVal().empty() )	val.setS("0",0,true);
	else				val.setS(acq_err.getVal(),0,true);
    }
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
    if( !enableStat() || !owner().startStat() )	valo.setI( EVAL_INT, 0, true );
    char szReceive[10];
    WORD wT;
    int RetValue;

    //> Send to active reserve station
    if( owner().redntUse( ) )
    {
	if( valo.getS(0,true) == pvl.getS() ) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",valo.name())->setText(valo.getS(0,true));
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }
    //> Direct write
    switch( modTp )
    {
	case 0x8017:
	{
	    bool ha = (valo.name().substr(0,2) == "ha");
	    bool la = (valo.name().substr(0,2) == "la");
	    if( !(ha||la) || !((PrmsI8017*)extPrms)->init )	break;

	    //> Create previous value
	    int hvl = 0, lvl = 0;
	    for( int i_v = 7; i_v >= 0; i_v-- )
	    {
		hvl = hvl << 1;
		lvl = lvl << 1;
		if( vlAt(TSYS::strMess("ha%d",i_v)).at().getB() == true )	hvl |= 1;
		if( vlAt(TSYS::strMess("la%d",i_v)).at().getB() == true )	lvl |= 1;
	    }
	    I8017_SetLed(modSlot,(lvl<<8)|hvl);
	    break;
	}
	case 0x87019:
	{
	    bool ha = (valo.name().substr(0,2) == "ha");
	    bool la = (valo.name().substr(0,2) == "la");
	    if( !(ha||la) )	break;

	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	    //> Create previous value
	    int hvl = 0, lvl = 0;
	    for( int i_v = 7; i_v >= 0; i_v-- )
	    {
		hvl = hvl << 1;
		lvl = lvl << 1;
		if( vlAt(TSYS::strMess("ha%d",i_v)).at().getB() == true )	hvl |= 1;
		if( vlAt(TSYS::strMess("la%d",i_v)).at().getB() == true )	lvl |= 1;
	    }

	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("@%02XL%02X%02X",(owner().mBus==0)?0:modAddr,lvl,hvl).c_str(), szReceive, 1, 0, &wT );
	    acq_err.setVal(RetValue?_("10:Request to module error."):"");
	    break;
	}
	case 0x8042:
	{
	    ResAlloc res( owner().reqRes, true );

	    bool vl = valo.getB(0,true);
	    if( vl == EVAL_BOOL || vl == pvl.getB() ) break;
	    DO_16( modSlot, vl ? (DO_16_RB(modSlot) | 0x01<<atoi(valo.name().c_str()+1)) :
				 (DO_16_RB(modSlot) & ~(0x01<<atoi(valo.name().c_str()+1))) );
	    break;
	}
	case 0x87024:
	{
	    double vl = valo.getR(0,true);
	    if( vl == EVAL_REAL || vl == pvl.getR() ) break;

	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	    string cmd = TSYS::strMess("#%02X%d%+07.3f",(owner().mBus==0)?0:modAddr,atoi(valo.name().c_str()+1),vl);

	    rep24:
	    int RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)cmd.c_str(), szReceive, 1, 0, &wT );
	    //> Set watchdog flag is process
	    if( !RetValue && szReceive[0] == '!' )
	    {
		Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("~%02X1",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
		goto rep24;
	    }
	    valo.setR( (RetValue||szReceive[0]!='>' ? EVAL_REAL : vl), 0, true );
	    acq_err.setVal(RetValue?_("10:Request to module error."):"");
	    break;
	}
	case 0x87057:
	{
	    char vl = valo.getB(0,true);
	    if( vl == EVAL_BOOL || vl == pvl.getB() ) break;

	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	    int daddr = atoi(valo.name().c_str()+1);
	    string cmd = TSYS::strMess("#%02X%s%d%02X",(owner().mBus==0)?0:modAddr,(daddr/8)?"B":"A",daddr%8,vl);

	    rep57:
	    int RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)cmd.c_str(), szReceive, 1, 0, &wT );
	    //> Set watchdog flag is process
	    if( !RetValue && szReceive[0] == '!' )
	    {
		Send_Receive_Cmd( owner().mBus?owner().mBus:1, (char*)TSYS::strMess("~%02X1",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
		goto rep57;
	    }

	    valo.setB( (RetValue||szReceive[0]!='>' ? EVAL_BOOL : vl), 0, true );
	    acq_err.setVal(RetValue?_("10:Request to module error."):((szReceive[0]!='>')?_("11:Respond from module error."):""));
	    break;
	}
    }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode( TVArchive::ActiveAttr, val.arch().at().srcData() );
    val.arch().at().setPeriod( (long long)(owner().period()*1000000) );
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

void *TMdPrm::fastTask( void *iprm )
{
    TMdPrm &prm = *(TMdPrm*)iprm;

#if OSC_DEBUG >= 2
    mess_debug(prm.nodePath().c_str(),_("Thread <%u> started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    prm.endRunReq = false;
    prm.prcSt = true;

    struct timespec sp_tm;
    long long wTm = TSYS::curTime();
    int c_mode;

    vector< AutoHD<TVal> > cnls;
    for( int i_c = 0; i_c < ((PrmsI8017*)prm.extPrms)->prmNum; i_c++ )
	cnls.push_back( prm.vlAt(TSYS::strMess("i%d",i_c)) );

    for( ;!prm.endRunReq && prm.owner().startStat(); ((PrmsI8017*)prm.extPrms)->curCnt++ )
    {
	for( int i_c = 0; i_c < cnls.size(); i_c++ )
	{
	    c_mode = ((PrmsI8017*)prm.extPrms)->cnlMode[i_c];
	    I8017_SetChannelGainMode(prm.modSlot,i_c,c_mode,0);
	    cnls[i_c].at().setR( (10.0/(c_mode?2*c_mode:1))*(float)I8017HW_GetCurAdChannel_Hex(prm.modSlot)/8000, wTm, true );
	}

	//> Calc next work time and sleep
	wTm += (long long)(1e6*((PrmsI8017*)prm.extPrms)->fastPer);
	sp_tm.tv_sec = wTm/1000000; sp_tm.tv_nsec = 1000*(wTm%1000000);
	clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&sp_tm,NULL);

	//TSYS::taskSleep((long long)(1e9*100e-6));
    }

    prm.prcSt = false;

    return NULL;
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    char szReceive[20];
    WORD wT;
    int RetValue;

    //> Get page info
    if( opt->name() == "info" )
    {
	cfg("MOD_ADDR").setView( (modTp>>12) != 8 && owner().mBus != 0 );
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_TP",cfg("MOD_TP").fld().descr(),0664,"root","root",3,"tp","dec","dest","select","select","/prm/cfg/modLst");
	switch( modTp )
	{
	    case 0x8017:
		if( !enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration")) ) break;
		ctrMkNode("fld",opt,-1,"/cfg/prms",_("Process parameters"),RWRWR_,"root","DAQ",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/cfg/fastPer",_("Fast data get period (s)"),RWRWR_,"root","DAQ",1,"tp","real");
		if( ctrMkNode("area",opt,-1,"/cfg/mode",_("Mode")) )
		    for( int i_v = 0; i_v < 8; i_v++ )
			ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/mode/in%d",i_v).c_str(),TSYS::strMess(_("Input %d"),i_v).c_str(),RWRWR_,"root","DAQ",3,"tp","dec","dest","select","select","/cfg/tpLst");
		break;
	    case 0x87019:
		if( !enableStat() || !owner().startStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration")) ) break;
		for( int i_v = 0; i_v < 8; i_v++ )
		    ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/inTp%d",i_v).c_str(),TSYS::strMess(_("Input %d type"),i_v).c_str(),RWRWR_,"root","DAQ",3,"tp","dec","dest","select","select","/cfg/tpLst");
		break;
	    case 0x87024:
		if( !enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration")) ) break;
		ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root","DAQ",3,"tp","real","min","0","max","25.5");
		if( !owner().startStat() || !ctrMkNode("area",opt,-1,"/cfg/mod",_("Module")) ) break;
		ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from curent"),RWRW__,"root","DAQ");
		ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from curent"),RWRW__,"root","DAQ");
		break;
	    case 0x87057:
		if( !enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration")) ) break;
		ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root","DAQ",3,"tp","real","min","0","max","25.5");
		if( !owner().startStat() || !ctrMkNode("area",opt,-1,"/cfg/mod",_("Module")) ) break;
		ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from curent"),RWRW__,"root","DAQ");
		ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from curent"),RWRW__,"root","DAQ");
		break;
		break;
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/modLst" && ctrChkNode(opt) )
    {
	if( owner().mBus == 0 )
	{
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8017))->setText("I-8017");
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8042))->setText("I-8042");
	}
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87019))->setText("I-87019");
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87024))->setText("I-87024");
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87057))->setText("I-87057");
    }
    else if( modTp == 0x8017 && enableStat() && a_path == "/cfg/prms" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )	opt->setText(TSYS::int2str(((PrmsI8017*)extPrms)->prmNum));
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )
	{ ((PrmsI8017*)extPrms)->prmNum = atoi(opt->text().c_str()); saveExtPrms(); }
    }
    else if( modTp == 0x8017 && enableStat() && a_path == "/cfg/fastPer" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )	opt->setText(TSYS::real2str(((PrmsI8017*)extPrms)->fastPer,5));
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )
	{ ((PrmsI8017*)extPrms)->fastPer = atof(opt->text().c_str()); saveExtPrms(); }
    }
    else if( modTp == 0x8017 && enableStat() && a_path.substr(0,12) == "/cfg/mode/in" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )	opt->setText(TSYS::int2str(((PrmsI8017*)extPrms)->cnlMode[atoi(a_path.substr(12).c_str())]));
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )
	{ ((PrmsI8017*)extPrms)->cnlMode[atoi(a_path.substr(12).c_str())] = atoi(opt->text().c_str()); saveExtPrms(); }
    }
    else if( modTp == 0x8017 && enableStat() && a_path == "/cfg/tpLst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("-10V to +10V"));
	opt->childAdd("el")->setAttr("id","1")->setText(_("-5V to +5V"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("-2.5V to +2.5V"));
	opt->childAdd("el")->setAttr("id","3")->setText(_("-1.25V to +1.25V"));
	opt->childAdd("el")->setAttr("id","4")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
    }
    else if( modTp == 0x87019 && enableStat() && owner().startStat() && a_path.substr(0,9) == "/cfg/inTp" )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )
	{
	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("$%02X8C%d",
		(owner().mBus==0)?0:modAddr,atoi(a_path.substr(9).c_str())).c_str(), szReceive, 1, 0, &wT );
	    opt->setText( (RetValue||szReceive[0]!='!') ? "-1" : TSYS::int2str(strtol(szReceive+6,NULL,16)) );
	}
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )
	    Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("$%02X7C%dR%02X",
		(owner().mBus==0)?0:modAddr,atoi(a_path.substr(9).c_str()),atoi(opt->text().c_str())).c_str(), szReceive, 1, 0, &wT );
    }
    else if( modTp == 0x87019 && a_path == "/cfg/tpLst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","-1")->setText(_("Error"));
	opt->childAdd("el")->setAttr("id","0")->setText(_("-15mV to +15mV"));
	opt->childAdd("el")->setAttr("id","1")->setText(_("-50mV to +50mV"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("-100mV to +100mV"));
	opt->childAdd("el")->setAttr("id","3")->setText(_("-500mV to +500mV"));
	opt->childAdd("el")->setAttr("id","4")->setText(_("-1V to +1V"));
	opt->childAdd("el")->setAttr("id","5")->setText(_("-2.5V to +2.5V"));
	opt->childAdd("el")->setAttr("id","6")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
	opt->childAdd("el")->setAttr("id","8")->setText(_("-10V to +10V"));
	opt->childAdd("el")->setAttr("id","9")->setText(_("-5V to +5V"));
	opt->childAdd("el")->setAttr("id","10")->setText(_("-1V to +1V"));
	opt->childAdd("el")->setAttr("id","11")->setText(_("-500mV to +500mV"));
	opt->childAdd("el")->setAttr("id","12")->setText(_("-150mV to +150mV"));
	opt->childAdd("el")->setAttr("id","13")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
	opt->childAdd("el")->setAttr("id","14")->setText(_("J Type"));
	opt->childAdd("el")->setAttr("id","15")->setText(_("K Type"));
	opt->childAdd("el")->setAttr("id","16")->setText(_("T Type"));
	opt->childAdd("el")->setAttr("id","17")->setText(_("E Type"));
	opt->childAdd("el")->setAttr("id","18")->setText(_("R Type"));
	opt->childAdd("el")->setAttr("id","19")->setText(_("S Type"));
	opt->childAdd("el")->setAttr("id","20")->setText(_("B Type"));
	opt->childAdd("el")->setAttr("id","21")->setText(_("N Type"));
	opt->childAdd("el")->setAttr("id","22")->setText(_("C Type"));
	opt->childAdd("el")->setAttr("id","23")->setText(_("L Type"));
	opt->childAdd("el")->setAttr("id","24")->setText(_("M Type"));
	opt->childAdd("el")->setAttr("id","25")->setText(_("L Type (DIN43710C Type)"));
    }
    else if( enableStat() && a_path == "/cfg/wTm" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )	opt->setText( TSYS::real2str(wTm) );
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )	{ wTm = atof(opt->text().c_str()); saveExtPrms(); }
    }
    else if( enableStat() && owner().startStat() && a_path == "/cfg/mod/wSt" && ctrChkNode(opt) )
    {
	string wSt;
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);
	RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X0",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	if( !RetValue && szReceive[0]=='!' )
	{
	    wSt += (bool)strtol(szReceive+3,NULL,16) ? _("Set. ") : _("Clear. ");

	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X2",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	    if( !RetValue && szReceive[0]=='!' )
	    {
		wSt += (bool)strtol(string(szReceive+3,1).c_str(),NULL,16) ? _("Enabled, ") : _("Disabled, ");
		wSt += TSYS::real2str(0.1*strtol(szReceive+4,NULL,16))+_(" s.");
	    }
	}
	opt->setText(wSt);
    }
    else if( modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPon" && ctrChkNode(opt) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	string rez;
	for( int i_c = 0; i_c < 4; i_c++ )
	{
	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("$%02X7%d",(owner().mBus==0)?0:modAddr,i_c).c_str(), szReceive, 1, 0, &wT );
	    if( RetValue || szReceive[0]!='!' ) { rez = _("Error"); break; }
	    rez = rez + (szReceive+3) + " ";
	}
	opt->setText(rez);
    }
    else if( modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPonSet" && ctrChkNode(opt,"set",RWRW__,"root","DAQ",SEQ_WR) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);
	for( int i_c = 0; i_c < 4; i_c++ )
	    Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("$%02X4%d",(owner().mBus==0)?0:modAddr,i_c).c_str(), szReceive, 1, 0, &wT );
    }
    else if( modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vSf" && ctrChkNode(opt) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	string rez;
	for( int i_c = 0; i_c < 4; i_c++ )
	{
	    RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X4%d",(owner().mBus==0)?0:modAddr,i_c).c_str(), szReceive, 1, 0, &wT );
	    if( RetValue || szReceive[0]!='!' ) { rez = _("Error"); break; }
	    rez = rez + (szReceive+3) + " ";
	}
	opt->setText(rez);
    }
    else if( modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vSfSet" && ctrChkNode(opt,"set",RWRW__,"root","DAQ",SEQ_WR) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);
	for( int i_c = 0; i_c < 4; i_c++ )
	    Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X5%d",(owner().mBus==0)?0:modAddr,i_c).c_str(), szReceive, 1, 0, &wT );
    }
    else if( modTp == 0x87057 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPon" && ctrChkNode(opt) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X4P",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	if( RetValue || szReceive[0]!='!' ) opt->setText(_("Error"));
	else
	{
	    string rez;
	    int vl = strtol(szReceive+3,NULL,16);
	    for( int i_o = 0; i_o < 16; i_o++ )	rez += ((vl>>i_o)&0x01)?"1 ":"0 ";
	    opt->setText(rez);
	}
    }
    else if( modTp == 0x87057 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPonSet" && ctrChkNode(opt,"set",RWRW__,"root","DAQ",SEQ_WR) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);
	Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X5P",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
    }
    else if( modTp == 0x87057 && owner().startStat() && owner().startStat() && a_path == "/cfg/mod/vSf" && ctrChkNode(opt) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);

	RetValue = Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X4S",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
	if( RetValue || szReceive[0]!='!' ) opt->setText(_("Error"));
	else
	{
	    string rez;
	    int vl = strtol(szReceive+3,NULL,16);
	    for( int i_o = 0; i_o < 16; i_o++ )	rez += ((vl>>i_o)&0x01)?"1 ":"0 ";
	    opt->setText(rez);
	}
    }
    else if( modTp == 0x87057 && owner().startStat() && owner().startStat() && a_path == "/cfg/mod/vSfSet" && ctrChkNode(opt,"set",RWRW__,"root","DAQ",SEQ_WR) )
    {
	ResAlloc res( owner().reqRes, true );
	if( owner().mBus == 0 )	ChangeToSlot(modSlot);
	Send_Receive_Cmd( owner().mBus?owner().mBus:1,(char*)TSYS::strMess("~%02X5S",(owner().mBus==0)?0:modAddr).c_str(), szReceive, 1, 0, &wT );
    }
    else TParamContr::cntrCmdProc(opt);
}

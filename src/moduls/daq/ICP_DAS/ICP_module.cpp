
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
#define VERSION		"0.1.0"
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
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("BUS",_("Bus"),TFld::Integer,TFld::NoFlag,"2","1") );
    fldAdd( new TFld("BAUD",_("Baudrate"),TFld::Integer,TFld::NoFlag,"6","115200") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_TP",_("Module type"),TFld::Integer,TFld::HexDec|TCfg::NoVal,"10","552985") );
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_ADDR",_("Module address"),TFld::Integer,TCfg::NoVal,"3","0","0;255") );
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_SLOT",_("Module slot"),TFld::Integer,TCfg::NoVal,"2","1","1;11") );
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
	TController( name_c, daq_db, cfgelem ), prc_st(false), endrun_req(false), tm_gath(0),
	mPer(cfg("PERIOD").getRd()), mPrior(cfg("PRIOR").getId()),
	mBus(cfg("BUS").getId()), mBaud(cfg("BAUD").getId())
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
    if( !prc_st )
    {
	if( mBus == 1 && Open_SlotAll() > 0 ) throw TError( nodePath().c_str(), _("Open All LP-slots error.") );

	if( Open_Com( mBus, mBaud, Data8Bit, NonParity, OneStopBit ) > 0 )
	    throw TError( nodePath().c_str(), _("Open COM%d port error."), mBus );

	//> Start the gathering data task
	pthread_attr_t pthr_attr;
	pthread_attr_init( &pthr_attr );
	struct sched_param prior;
	if( mPrior && SYS->user() == "root" )
	    pthread_attr_setschedpolicy( &pthr_attr, SCHED_RR );
	else pthread_attr_setschedpolicy( &pthr_attr, SCHED_OTHER );
	prior.__sched_priority = mPrior;
	pthread_attr_setschedparam( &pthr_attr, &prior );

	pthread_create( &procPthr, &pthr_attr, TMdContr::Task, this );
	pthread_attr_destroy( &pthr_attr );
	if( TSYS::eventWait( prc_st, true, nodePath()+"start", 5 ) )
	    throw TError( nodePath().c_str(), _("Gathering task is not started!") );
    }
}

void TMdContr::stop_( )
{
    if( prc_st )
    {
	//> Stop the request and calc data task
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait( prc_st, false, nodePath()+"stop", 5 ) )
	    throw TError( nodePath().c_str(), _("Gathering task is not stopped!") );
	pthread_join( procPthr, NULL );

	Close_Com(mBus);
    }
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if( icfg.name() == "BUS" )
    {
	cfg("BAUD").setView( icfg.getI() != 1 );
	if( startStat() ) stop();
    }
    else if( icfg.name() == "BAUD" && startStat( ) ) stop( );

    return true;
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
    long long work_tm, last_tm = 0;
    TMdContr &cntr = *(TMdContr*)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {
	while( !cntr.endrun_req )
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

	    //> Calc next work time and sleep
	    TSYS::taskSleep((long long)(1e9*cntr.period()));
	}
    }
    catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

    cntr.prc_st = false;

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
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/busLst" && ctrChkNode(opt) )
	for( int i_s = 1; i_s < 11; i_s++ )
            opt->childAdd("el")->setAttr("id",TSYS::int2str(i_s))->setText("COM "+TSYS::int2str(i_s)+(i_s==1?_(" (Master)"):""));
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
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr( name, tp_prm ), p_el("w_attr"), I8017_init(false),
    mod_tp(cfg("MOD_TP").getId()), mod_addr(cfg("MOD_ADDR").getId()), mod_slot(cfg("MOD_SLOT").getId())
{

}

TMdPrm::~TMdPrm( )
{
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

    //> Delete DAQ parameter's attributes
    while(p_el.fldSize()>0)
    {
	try{ p_el.fldDel(0); }
	catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    //> Make DAQ parameter's attributes
    switch( mod_tp )
    {
	//> Make DAQ parameter's for I-7051 module
	case 0x8017:
	    //if( wRetVal > 0 ) throw TError(nodePath().c_str(),_("Open slot %d error."),mod_slot);
	    //wRetVal = I8017_Init(mod_slot);
	    for( int i_i = 0; i_i < 8; i_i++ )
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite|TVal::DirRead,"",TSYS::real2str(EVAL_REAL).c_str()) );
	    break;
	case 0x8042:
	    for( int i_i = 0; i_i < 16; i_i++ )
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Boolean,TFld::NoWrite|TVal::DirRead,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    for( int i_o = 0; i_o < 16; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite,"",TSYS::real2str(EVAL_BOOL).c_str()) );
	    break;
	case 0x87019:
	    for( int i_i = 0; i_i < 8; i_i++ )
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite|TVal::DirRead,"",TSYS::real2str(EVAL_REAL).c_str()) );
	    break;
	case 0x87024:
	    for( int i_o = 0; i_o < 4; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Real,TVal::DirWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
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

    switch( mod_tp )
    {
	//> Make DAQ parameter's for I-7051 module
	case 50: case 51:	//I-8017, I-8042
	    Close_Slot(mod_slot);
	    break;
    }
}

void TMdPrm::getVals( )
{
    int RetValue;

    switch( mod_tp )
    {    
	case 0x8017:
	{
	    printf("TEST 00\n");
	    if( !I8017_init ) { I8017_Init(mod_slot); I8017_init = true; }
	    for( int i_v = 0; i_v < 8; i_v++ )
	    {
		I8017_SetChannelGainMode(mod_slot,i_v,0,0);
		vlAt(TSYS::strMess("i%d",i_v)).at().setR( I8017_GetCurAdChannel_Float_Cal(mod_slot), 0, true );
	    }
	    break;
	}
	case 0x87019:
	{
	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 1 )	ChangeToSlot(mod_slot);

	    WORD wT;
	    char szReceive[255];

	    RetValue = Send_Receive_Cmd( owner().mBus, (char*)TSYS::strMess("#%02x",(owner().mBus==1)?0:mod_addr).c_str(), szReceive, 1, 0, &wT );
	    for( int i_v = 0; i_v < 8; i_v++ )
		vlAt(TSYS::strMess("i%d",i_v)).at().setR( (RetValue||szReceive[0]!='>') ? EVAL_REAL : atof(szReceive+1+7*i_v), 0, true );
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

    if( val.name() != "err" )
	switch( mod_tp )
	{
/*	    case 0x8017:
		I8017_SetChannelGainMode(mod_slot,atoi(val.name().c_str()+1),0,0);
		val.setR( I8017_GetCurAdChannel_Float_Cal(mod_slot), 0, true );
		break;*/
	    case 0x8042:
		val.setB( (DI_16(mod_slot) >> atoi(val.name().c_str()+1))&0x01, 0, true );
		break;
	}
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
    if( !enableStat() || !owner().startStat() )	valo.setI( EVAL_INT, 0, true );

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
    switch( mod_tp )
    {
	case 0x8042:
	{
	    bool vl = valo.getB(0,true);
	    if( vl == EVAL_BOOL || vl == pvl.getB() ) break;
	    DO_16( mod_slot, vl ? (DI_16(mod_slot) | 0x01<<atoi(valo.name().c_str()+1)) :
				  (DI_16(mod_slot) & ~(0x01<<atoi(valo.name().c_str()+1))) );
	    break;
	}
	case 0x87024:
	{
	    double vl = valo.getR(0,true);
	    if( vl == EVAL_REAL || vl == pvl.getR() ) break;

	    ResAlloc res( owner().reqRes, true );
	    if( owner().mBus == 1 )	ChangeToSlot(mod_slot);

	    WORD wT;
	    string cmd = TSYS::strMess("#%02x%d%+07.2f",(owner().mBus==1)?0:mod_addr,atoi(valo.name().c_str()+1),vl);
	    char szReceive[10];

	    int RetValue = Send_Receive_Cmd( owner().mBus, (char*)cmd.c_str(), szReceive, 1, 0, &wT );
	    printf("TEST 00: '%s' -> '%s'\n",cmd.c_str(),szReceive);
	    valo.setR( (RetValue||szReceive[0]!='>' ? EVAL_REAL : vl), 0, true );

	    /*double vl = valo.getR(0,true);
	    if( vl == EVAL_REAL || vl == pvl.getR() ) break;
	    DWORD dwBuf[12];
	    float fBuf[12];
	    char  szSend[20], szReceive[20];

	    dwBuf[0] = owner().mBus;
	    dwBuf[1] = mod_addr;
	    dwBuf[2] = mod_tp;
	    dwBuf[3] = 0;
	    dwBuf[4] = 100;
	    dwBuf[5] = atoi(valo.name().c_str()+1);
	    dwBuf[6] = 0;
	    dwBuf[7] = mod_slot;
	    fBuf[0]  = valo.getR();
	    int RetValue = AnalogOut_87K(dwBuf,fBuf,szSend,szReceive);
	    valo.setR( (RetValue ? EVAL_REAL : fBuf[0]), 0, true );
	    break;*/
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

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	cfg("MOD_ADDR").setView( (mod_tp>>12) != 8 && owner().mBus != 1 );
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_TP",cfg("MOD_TP").fld().descr(),0664,"root","root",3,"tp","dec","dest","select","select","/prm/cfg/modLst");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/modLst" && ctrChkNode(opt) )
    {
	if( owner().mBus == 1 )
	{
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8017))->setText("I-8017");
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8042))->setText("I-8042");
	}
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87019))->setText("I-87019");
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87024))->setText("I-87024");
    }
    else TParamContr::cntrCmdProc(opt);
}
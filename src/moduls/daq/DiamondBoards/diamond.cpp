
//OpenSCADA system module DAQ.DiamondBoards file: diamond.cpp
/***************************************************************************
 *   Copyright (C) 2005-2009 by Roman Savochenko                           *
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

#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <tsys.h>
#include <ttiparam.h>

#include "diamond.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DiamondBoards"
#define MOD_NAME	"Diamond DA boards"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"1.2.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow access to Diamond systems DA boards. Include support of Athena board."
#define LICENSE		"GPL"
//*************************************************

Diamond::TTpContr *Diamond::mod;

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
	    return new Diamond::TTpContr( source );
	return NULL;
    }
}

using namespace Diamond;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) :
    m_init(false), elem_ai("AI"), elem_ao("AO"), elem_di("DI"), elem_do("DO")
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
    //- Free DSCAD -
    if( drvInitOk( ) )	dscFree();
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable( flag );

    //- Init DSCAD -
    if( dscInit( DSC_VERSION ) != DE_NONE )
	mess_err(mod->nodePath().c_str(),_("dscInit error."));
    else m_init = true;

    //- Controler's bd structure -
    fldAdd( new TFld("BOARD",_("Diamond system board"),TFld::Integer,TFld::Selected,"2","25",
	(TSYS::int2str(DSC_DMM16)+";"+TSYS::int2str(DSC_ATHENA)+";"+TSYS::int2str(DSC_DMM32XAT)).c_str(),
	"DMM16;ATHENA;DMM32XAT") );
    fldAdd( new TFld("PRM_BD_A",_("Analog parameters' table"),TFld::String,0,"30","diamond_prm_a") );
    fldAdd( new TFld("PRM_BD_D",_("Digital parameters' table"),TFld::String,0,"30","diamond_prm_d") );
    fldAdd( new TFld("DATA_EMUL",_("Data emulation"),TFld::Boolean,TFld::NoFlag,"1","0") );
    fldAdd( new TFld("ADDR",_("Base board address"),TFld::Integer,TFld::HexDec,"3","640") );
    fldAdd( new TFld("INT",_("Interrupt vector"),TFld::Integer,0,"2","5") );
    fldAdd( new TFld("DIO_CFG",_("Digit IO config byte"),TFld::Integer,TFld::HexDec,"2","0") );
    fldAdd( new TFld("ADMODE",_("A/D interrupt mode"),TFld::Boolean,TFld::NoFlag,"1","0") );
    fldAdd( new TFld("ADRANGE",_("A/D voltage range"),TFld::Integer,TFld::Selected,"1",TSYS::int2str(RANGE_10).c_str(),
	(TSYS::int2str(RANGE_5)+";"+TSYS::int2str(RANGE_10)).c_str(),_("5v;10v")) );
    fldAdd( new TFld("ADPOLAR",_("A/D polarity"),TFld::Integer,TFld::Selected,"1",TSYS::int2str(BIPOLAR).c_str(),
	(TSYS::int2str(BIPOLAR)+";"+TSYS::int2str(UNIPOLAR)).c_str(),_("Bipolar;Unipolar")) );
    fldAdd( new TFld("ADGAIN",_("A/D gain"),TFld::Integer,TFld::Selected,"1",TSYS::int2str(GAIN_1).c_str(),
	(TSYS::int2str(GAIN_1)+";"+TSYS::int2str(GAIN_2)+";"+TSYS::int2str(GAIN_4)+";"+TSYS::int2str(GAIN_8)).c_str(),"x1;x2;x4;x8") );
    fldAdd( new TFld("ADCONVRATE",_("A/D convertion rate (Hz)"),TFld::Integer,0,"6","200","100;100000") );

    //- Parameter type bd structure -
    //-- Analog --
    int t_prm = tpParmAdd("a_prm","PRM_BD_A",_("Analog parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",_("Analog parameter type"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1",_("Input;Output")) );
    tpPrmAt(t_prm).fldAdd( new TFld("CNL",_("Channel"),TFld::Integer,TCfg::NoVal,"2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("GAIN",_("A/D converter gain"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1",TSYS::int2str(GAIN_1).c_str(),
	(TSYS::int2str(GAIN_1)+";"+TSYS::int2str(GAIN_2)+";"+TSYS::int2str(GAIN_4)+";"+TSYS::int2str(GAIN_8)).c_str(),"x1;x2;x4;x8") );
    //-- Digit --
    t_prm = tpParmAdd("d_prm","PRM_BD_D",_("Digital parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",_("Digital parameter type"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1",_("Input;Output")) );
    tpPrmAt(t_prm).fldAdd( new TFld("PORT",_("Port"),TFld::Integer,TFld::Selected|TCfg::NoVal,"2","0","0;1;2","A;B;C") );
    tpPrmAt(t_prm).fldAdd( new TFld("CNL",_("Channel"),TFld::Integer,TCfg::NoVal,"1") );

    //- Init value elements -
    //-- Analog input --
    elem_ai.fldAdd( new TFld("value",_("Value %"),TFld::Real,TFld::NoWrite|TVal::DirRead,"",TSYS::real2str(EVAL_REAL).c_str(),"0;100","","1") );
    elem_ai.fldAdd( new TFld("voltage",_("Voltage V"),TFld::Real,TFld::NoWrite|TVal::DirRead,"",TSYS::real2str(EVAL_REAL).c_str(),"-10;10","","2") );
    elem_ai.fldAdd( new TFld("code",_("A/D code"),TFld::Integer,TFld::NoWrite|TVal::DirRead,"",TSYS::int2str(EVAL_INT).c_str(),"","","3") );
    //-- Analog output --
    elem_ao.fldAdd( new TFld("value",_("Value %"),TFld::Real,TVal::DirWrite,"",TSYS::real2str(EVAL_REAL).c_str(),"0;100","","1") );
    elem_ao.fldAdd( new TFld("voltage",_("Voltage V"),TFld::Real,TVal::DirWrite,"",TSYS::real2str(EVAL_REAL).c_str(),"-10;10","","2") );
    //-- Digit input --
    elem_di.fldAdd( new TFld("value",_("Value"),TFld::Boolean,TFld::NoWrite|TVal::DirRead,"",TSYS::int2str(EVAL_BOOL).c_str(),"","","1") );
    //-- Digit output --
    elem_do.fldAdd( new TFld("value",_("Value"),TFld::Boolean,TVal::DirWrite,"",TSYS::int2str(EVAL_BOOL).c_str(),"","","1") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) : ad_dsc_st(false),
	::TController(name_c,daq_db,cfgelem), 
	data_emul(cfg("DATA_EMUL").getBd()), m_addr(cfg("ADDR").getId()), ad_int_mode(cfg("ADMODE").getBd())
{
    cfg("PRM_BD_A").setS("DiamPrmA_"+name_c);
    cfg("PRM_BD_D").setS("DiamPrmD_"+name_c);

    //- Hide sevral config fields -
    cfg("INT").setView(false);
    cfg("DIO_CFG").setView(false);
    cfg("ADCONVRATE").setView(false);
    cfg("ADGAIN").setView(false);

    memset(&dscadsettings, 0, sizeof(DSCADSETTINGS));
}

TMdContr::~TMdContr()
{

}

TTpContr &TMdContr::owner( )	{ return (TTpContr&)TController::owner(); }

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::load_( )
{
    cfgViewAll(true);
    TController::load_( );
    cfg("ADMODE").setB(cfg("ADMODE").getB());	//For hiden attributes visible status update
}

void TMdContr::start_( )
{
    //> Main DSC code
    if( !dataEmul() )
    {
	//> Check inited of Diamond API
	if( !mod->drvInitOk() )
	    throw TError(nodePath().c_str(),_("DSC driver is not initialized!"));

	//> DSC strucures init
	ERRPARAMS errorParams;
	DSCADSETTINGS dscadsettings;
	memset(&dscadsettings, 0, sizeof(DSCADSETTINGS));

	//>> Init Board
	DSCCB dsccb;
	dsccb.base_address = m_addr;
	dsccb.int_level = cfg("INT").getI();
	if(dscInitBoard(cfg("BOARD").getI(), &dsccb, &dscb)!= DE_NONE)
	{
	    dscGetLastError(&errorParams);
	    throw TError(nodePath().c_str(),_("dscInitBoard %d(%xh) error: %s %s"),
	        cfg("BOARD").getI(),m_addr,dscGetErrorString(errorParams.ErrCode), errorParams.errstring );
	}
	//>> Set DIO config
	BYTE cfg_byte = cfg("DIO_CFG").getI()|0x80;
	if( dscDIOSetConfig(dscb, &cfg_byte) != DE_NONE )
 	{
	    dscGetLastError(&errorParams);
	    throw TError(nodePath().c_str(),_("dscDIOSetConfig error: %s %s"),dscGetErrorString(errorParams.ErrCode), errorParams.errstring );
	}
	//>> Init AD acquisition
	dscadsettings.range = cfg("ADRANGE").getI();
	dscadsettings.polarity = cfg("ADPOLAR").getI();
	dscadsettings.gain = cfg("ADGAIN").getI();
	dscadsettings.load_cal = 0;
    }

    //> Create interrupt AD DSC task
    if( ad_int_mode ) SYS->taskCreate( nodePath('.',true)+".int", 0, AD_DSCTask, this, &ad_dsc_st );
}

void TMdContr::stop_( )
{
    //> Close AI DAQ task
    if( ad_dsc_st ) SYS->taskDestroy( nodePath('.',true)+".int", &ad_dsc_st, &endrun_req_ad_dsc );

    if( !dataEmul() )	dscFreeBoard(dscb);
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if(icfg.fld().name() == "ADMODE")
    {
	if(icfg.getB())
	{
	    cfg("INT").setView(true);
	    cfg("ADCONVRATE").setView(true);
	    cfg("ADGAIN").setView(true);
	}
	else
	{
	    cfg("INT").setView(false);
	    cfg("ADCONVRATE").setView(false);
	    cfg("ADGAIN").setView(false);
	}
	if(startStat()) stop();
    }
    else if(icfg.fld().name() == "DATA_EMUL" && startStat() )	stop();

    return true;
}

void *TMdContr::AD_DSCTask( void *param )
{
    long long vtm = 0;
    struct timespec get_tm;
    get_tm.tv_sec = 0; get_tm.tv_nsec = 200000000;
    int prev_trans = -1;

    //- AI interrupt dequisition -
    vector<string> ai_prm;
    int p_beg = 100, p_end = 0;

    TMdContr &cntr = *(TMdContr *)param;
    cntr.endrun_req_ad_dsc = false;
    cntr.ad_dsc_st = true;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    //- DSC strucures init -
    BYTE result;
    ERRPARAMS errorParams;
    DSCAIOINT dscaioint;
    memset(&dscaioint, 0, sizeof(DSCAIOINT));
    DSCS dscs;

    //- Main DSC code -
    try
    {
	//- Get AI param list and address border -
	cntr.list(ai_prm);
	for( int i_p = 0; i_p < ai_prm.size(); i_p++ )
	{
	    AutoHD<TMdPrm> prm = cntr.at(ai_prm[i_p]);
	    if(prm.at().type() != TMdPrm::AI)
	    {
	        ai_prm.erase(ai_prm.begin()+i_p);
	        i_p--;
	    }
	    else
	    {
	        p_beg = vmin(p_beg,prm.at().cnl());
	        p_end = vmax(p_end,prm.at().cnl());
	    }
	}
	//- Generic data -
	int convRate = 2*(cntr.cfg("ADCONVRATE").getI()/2);

	if( !cntr.dataEmul() )
	{
	    if(ai_prm.size())
	    {
		cntr.dscadsettings.current_channel = p_beg;
		if( dscADSetSettings( cntr.dscb, &cntr.dscadsettings ) != DE_NONE )
		{
		    dscGetLastError(&errorParams);
		    throw TError(cntr.nodePath().c_str(),_("dscADSetSettings error: %s %s"), dscGetErrorString(errorParams.ErrCode), errorParams.errstring );
		}

		//- Init interrupt -
		dscaioint.conversion_rate = convRate;
		dscaioint.num_conversions = 2*(int)dscaioint.conversion_rate*(p_end-p_beg+1);
		dscaioint.cycle = 1;
		dscaioint.internal_clock = 1;
		dscaioint.low_channel = p_beg;
		dscaioint.high_channel = p_end;
		dscaioint.external_gate_enable = 0;
		dscaioint.internal_clock_gate = 0;
		dscaioint.dump_threshold = dscaioint.num_conversions/2;
		dscaioint.fifo_enab = 1;
		dscaioint.fifo_depth = dscaioint.num_conversions;
		while( dscaioint.fifo_depth>46 )
		{
		    if( dscaioint.fifo_depth > 2*46 ) dscaioint.fifo_depth/=10;
		    else dscaioint.fifo_depth/=2;
		}
		dscaioint.sample_values = (DSCSAMPLE*)malloc( sizeof(DSCSAMPLE) * dscaioint.num_conversions );
		if( dscADScanInt( cntr.dscb, &dscaioint ) != DE_NONE )
		{
		    dscGetLastError(&errorParams);
		    throw TError(cntr.nodePath().c_str(),_("dscADScanInt error: %s %s"),dscGetErrorString(errorParams.ErrCode), errorParams.errstring );
		}
		//- Init operation data -
		dscs.transfers = 0;
		dscs.overflows = 0;
		dscs.op_type = OP_TYPE_INT;
	    }

	    while( !cntr.endrun_req_ad_dsc )
	    {
		dscGetStatus(cntr.dscb, &dscs);
		if( prev_trans < 0 ) prev_trans = dscs.transfers;
		if( dscs.transfers != prev_trans && !cntr.redntUse() )
		{
		    //> Init current time and check for current time correction
		    long long cTm = TSYS::curTime();
		    if( !vtm || fabs((vtm+1000000)-cTm) > 1e6 )
		    {
			if( vtm ) mess_warning(cntr.nodePath().c_str(),_("DSC card's counter run from system time is corrected."));
			vtm = cTm-1000000;
		    }

		    int v_a_step;
		    int p_cnt = p_end-p_beg+1;
		    for( int i_p = 0; i_p < ai_prm.size(); i_p++ )
		    {
			if( !cntr.present(ai_prm[i_p]) )	continue;
			AutoHD<TMdPrm> prm = cntr.at(ai_prm[i_p]);
			int p_cnl = prm.at().cnl();
			if(prm.at().type() != TMdPrm::AI || p_cnl < p_beg || p_cnl > p_end || !prm.at().enableStat() )
			    continue;
			int voff = (dscs.transfers+dscaioint.dump_threshold)%dscaioint.num_conversions;
			//- Get code -
			AutoHD<TVal> val = prm.at().vlAt("code");
			if( !val.at().arch().freeStat() && val.at().arch().at().srcMode() == TVArchive::PassiveAttr )
			{
			    v_a_step = vmax(1,val.at().arch().at().period()*(int)dscaioint.conversion_rate/1000000);
			    for( int i_smpl = 0; i_smpl < dscaioint.conversion_rate; i_smpl+=v_a_step )
				val.at().arch().at().setI(dscaioint.sample_values[voff+p_cnl-p_beg+i_smpl*p_cnt],vtm+(long long)i_smpl*1000000/(int)dscaioint.conversion_rate);
			}
			val.at().setI(dscaioint.sample_values[voff+p_cnl-p_beg+((int)dscaioint.conversion_rate-1)*p_cnt],vtm+((long long)dscaioint.conversion_rate-1)*1000000/(int)dscaioint.conversion_rate,true);
			//- Get procent -
			val = prm.at().vlAt("value");
			if(!val.at().arch().freeStat() && val.at().arch().at().srcMode() == TVArchive::PassiveAttr)
			{
			    v_a_step = vmax(1,val.at().arch().at().period()*(int)dscaioint.conversion_rate/1000000);
			    for( int i_smpl = 0; i_smpl < dscaioint.conversion_rate; i_smpl+=v_a_step )
				val.at().arch().at().setR( 100.*(double)dscaioint.sample_values[voff+p_cnl-p_beg+i_smpl*p_cnt]/32768.,vtm+(long long)i_smpl*1000000/(int)dscaioint.conversion_rate);
			}
			val.at().setR( 100.*(double)dscaioint.sample_values[voff+p_cnl-p_beg+((int)dscaioint.conversion_rate-1)*p_cnt]/32768.,vtm+((long long)dscaioint.conversion_rate-1)*1000000/(int)dscaioint.conversion_rate,true);
			//- Get voltage -
			//for( int i_smpl = 0; i_smpl < dscaioint.conversion_rate; i_smpl++ )
			//	printf("Canal %d(%d): %xh\n",prm.at().cnl(),voff+prm.at().cnl()-p_beg+i_smpl*(p_end-p_beg+1),dscaioint.sample_values[voff+prm.at().cnl()-p_beg+i_smpl*(p_end-p_beg+1)]);
		    }
		    vtm += 1000000;
		}
		prev_trans = dscs.transfers;
		clock_nanosleep(CLOCK_REALTIME,0,&get_tm,NULL);
	    }
	}
	else
	    while( !cntr.endrun_req_ad_dsc )
	    {
		if( !vtm ) vtm = SYS->curTime()-1000000;
		int v_a_step;
		int p_cnt = p_end-p_beg+1;
		for( int i_p = 0; i_p < ai_prm.size() && !cntr.redntUse(); i_p++ )
		{
		    if( !cntr.present(ai_prm[i_p]) )	continue;
		    AutoHD<TMdPrm> prm = cntr.at(ai_prm[i_p]);
		    int p_cnl = prm.at().cnl();
		    if( prm.at().type() != TMdPrm::AI || p_cnl < p_beg || p_cnl > p_end || !prm.at().enableStat() )
		        continue;
		    //- Get code -
		    AutoHD<TVal> val = prm.at().vlAt("code");
		    if(!val.at().arch().freeStat() && val.at().arch().at().srcMode() == TVArchive::PassiveAttr)
		    {
			v_a_step = vmax(1,val.at().arch().at().period()*convRate/1000000);
			for( int i_smpl = 0; i_smpl < convRate; i_smpl+=v_a_step )
			    val.at().arch().at().setI((int)((float)rand()*20000/RAND_MAX),vtm+(long long)i_smpl*1000000/convRate);
		    }
		    val.at().setI((int)((float)rand()*20000/RAND_MAX),vtm+((long long)convRate-1)*1000000/convRate,true);
		    //- Get procent -
		    val = prm.at().vlAt("value");
		    if(!val.at().arch().freeStat() && val.at().arch().at().srcMode() == TVArchive::PassiveAttr)
		    {
		        v_a_step = vmax(1,val.at().arch().at().period()*convRate/1000000);
			for( int i_smpl = 0; i_smpl < convRate; i_smpl+=v_a_step )
			    val.at().arch().at().setR( 100.*((float)rand()*20000/RAND_MAX)/32768.,vtm+(long long)i_smpl*1000000/convRate);
		    }
		    val.at().setR( 100.*(double)((float)rand()*20000/RAND_MAX)/32768.,vtm+((long long)convRate-1)*1000000/convRate,true);
		}
		vtm+=1000000;
		get_tm.tv_sec = vtm/1000000; get_tm.tv_nsec = 1000*(vtm%1000000);
		clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);
	    }
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(cntr.nodePath().c_str(),_("AD DSC task error."));
    }

    cntr.ad_dsc_st = false;

    if(!cntr.dataEmul())
    {
	if( dscs.op_type != OP_TYPE_NONE )	dscCancelOp(cntr.dscb);
	free( dscaioint.sample_values );
    }

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,-1,"/board",_("Board config")))
	    if(ctrMkNode("area",opt,-1,"/board/dio",_("Digital IO ports. Select input!")))
	    {
		ctrMkNode("fld",opt,-1,"/board/dio/a",_("Port A"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/board/dio/b",_("Port B"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/board/dio/c1",_("Port C1"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/board/dio/c2",_("Port C2"),0664,"root","root",1,"tp","bool");
	    }
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path.substr(0,11) == "/board/dio/" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )
	{
	    string port_n = TSYS::pathLev(a_path,2);
	    int	cfg_b = cfg("DIO_CFG").getI();
	    if( port_n == "a" )		cfg_b&=0x10;
	    else if( port_n == "b" )	cfg_b&=0x02;
	    else if( port_n == "c1" )	cfg_b&=0x01;
	    else if( port_n == "c2" )	cfg_b&=0x08;
	    opt->setText(cfg_b?"1":"0");
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )
	{
	    string port_n = TSYS::pathLev(a_path,2);
	    int cfg_b = cfg("DIO_CFG").getI();
	    if( port_n == "a" )		cfg_b = atoi(opt->text().c_str())?cfg_b|0x10:cfg_b&(~0x10);
	    else if( port_n == "b" )	cfg_b = atoi(opt->text().c_str())?cfg_b|0x02:cfg_b&(~0x02);
	    else if( port_n == "c1" )	cfg_b = atoi(opt->text().c_str())?cfg_b|0x01:cfg_b&(~0x01);
	    else if( port_n == "c2" )	cfg_b = atoi(opt->text().c_str())?cfg_b|0x08:cfg_b&(~0x08);
	    cfg("DIO_CFG").setI(cfg_b);
	}
    }
    else TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), m_cnl(cfg("CNL").getId()), m_tp(NONE)
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable( flag );

    if( TParamContr::type().name == "a_prm" )		setType(AI);
    else if( TParamContr::type().name == "d_prm" )	setType(DI);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::load_( )
{
    cfgViewAll(true);
    TParamContr::load_();
    setType(type());		//For hiden attributes visible status update
}

void TMdPrm::setType( TMdPrm::Type vtp )
{
    //- Free previos type -
    switch(m_tp)
    {
	case AI: vlElemDet( &mod->elemAI() ); break;
	case AO: vlElemDet( &mod->elemAO() ); break;
	case DI: vlElemDet( &mod->elemDI() ); break;
	case DO: vlElemDet( &mod->elemDO() ); break;
    }

    //- Init new type -
    switch(vtp)
    {
	case AI:
	    cfg("GAIN").setView(true);
	    m_gain = cfg("GAIN").getI();
	    cfg("GAIN").setView(!owner().ADIIntMode());
	    vlElemAtt( &mod->elemAI() );
	    break;
	case AO:
	    cfg("GAIN").setView(false);
	    vlElemAtt( &mod->elemAO() );
	    break;
	case DI:
	    m_dio_port = (cfg("PORT").getI()<<4)+cfg("CNL").getI();
	    vlElemAtt( &mod->elemDI() );
	    break;
	case DO:
	    vlElemAtt( &mod->elemDO() );
	    break;
    }
    m_tp = vtp;
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{
    TParamContr::cfgChange(i_cfg);

    //- Change TYPE parameter -
    if( i_cfg.name() == "TYPE" )
    {
	if( i_cfg.getI() == 0 && type() == AO )		setType(AI);
	else if( i_cfg.getI() == 0 && type() == DO )	setType(DI);
	else if( i_cfg.getI() == 1 && type() == AI )	setType(AO);
	else if( i_cfg.getI() == 1 && type() == DI )	setType(DO);
	else return false;
	return true;
    }
    switch( type() )
    {
	case AI:
	    if( i_cfg.name() == "GAIN" ) m_gain = i_cfg.getI();
	    break;
	case DI: case DO:
	    if( i_cfg.name() == "PORT" )	m_dio_port = (i_cfg.getI()<<4)+cfg("CNL").getI();
	    else if( i_cfg.name() == "CNL" )	m_dio_port = (cfg("PORT").getI()<<4)+i_cfg.getI();
	    break;
    }

    return true;
}

void TMdPrm::vlSet( TVal &val, const TVariant &pvl )
{
    if( !owner().startStat() || !enableStat() )	return;

    //> Send to active reserve station
    if( owner().redntUse( ) )
    {
	if( val.getS(0,true) == pvl.getS() ) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",val.name())->setText(val.getS(0,true));
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //> Direct write
    switch(type())
    {
	case AO:
	{
	    int code;
	    switch( atoi(val.fld().reserve().c_str()) )
	    {
		case 1:	code = (int)(4095.*val.getR(0,true)/100.);	break;
		case 2:	code = (int)(4095.*val.getR(0,true)/10.);	break;
	    }

	    //- Direct writing -
	    if( owner().dataEmul() )	break;
	    owner().ao_res.resRequestW( );
	    if( dscDAConvert(owner().dscb,m_cnl,code) != DE_NONE )
	    {
		ERRPARAMS errorParams;
		dscGetLastError(&errorParams);
		mess_err(nodePath().c_str(),_("dscDAConvert error: %s %s"),dscGetErrorString(errorParams.ErrCode),errorParams.errstring );
	    }
	    owner().ao_res.resRelease( );
	    break;
	}
	case DO:
	{
	    //- Direct writing -
	    if( owner().dataEmul() )	break;
	    owner().dio_res.resRequestW( );
	    if( dscDIOOutputBit(owner().dscb, m_dio_port>>4, m_dio_port&0x0f, val.getB(0,true)) != DE_NONE )
	    {
		ERRPARAMS errorParams;
		dscGetLastError(&errorParams);
		mess_err(nodePath().c_str(),_("dscDIOOutputBit error: %s %s"),dscGetErrorString(errorParams.ErrCode),errorParams.errstring );
	    }
	    owner().dio_res.resRelease( );
	}
    }
}

void TMdPrm::vlGet( TVal &val )
{
    int aid = atoi(val.fld().reserve().c_str());
    if( aid == 0 )
    {
	if( !owner().startStat() )
	    val.setS(_("2:Controller is stoped"),0,true);
	else if( !enableStat() )
	    val.setS(_("1:Parameter is disabled"),0,true);
	else val.setS("0",0,true);
	return;
    }
    if( !owner().startStat() || !enableStat() )	{ val.setS(EVAL_STR,0,true); return; }

    if( owner().redntUse( ) ) return;

    switch(type())
    {
	case AI:
	{
	    if(owner().ADIIntMode()) return;
	    short gval;
	    if( enableStat() )
	    {
		//- Direct reading -
		if(owner().dataEmul())	gval = rand()*10000/RAND_MAX;
		else
		{
		    owner().ai_res.resRequestW( );
		    owner().dscadsettings.gain = m_gain;
		    owner().dscadsettings.current_channel = m_cnl;
		    if( dscADSetSettings(owner().dscb,&owner().dscadsettings) != DE_NONE )
		    {
			ERRPARAMS errorParams;
			dscGetLastError(&errorParams);
			mess_err(nodePath().c_str(),_("dscADSetSettings error: %s %s"), dscGetErrorString(errorParams.ErrCode), errorParams.errstring );
		    }
		    DSCSAMPLE smpl;
		    if( dscADSample(owner().dscb,&smpl) != DE_NONE )
		    {
			ERRPARAMS errorParams;
			dscGetLastError(&errorParams);
			mess_err(nodePath().c_str(),_("dscADSample error: %s %s"), dscGetErrorString(errorParams.ErrCode), errorParams.errstring );
		    }
		    gval = smpl;
		    owner().ai_res.resRelease( );
		}
	    }
	    switch( aid )
	    {
		case 1: val.setR(enableStat()?(100.*((double)gval/32768.)):EVAL_REAL,0,true); break;
		case 2: val.setR(enableStat()?(10.*((double)gval/32768.)):EVAL_REAL,0,true);  break;
		case 3: val.setI(enableStat()?gval:EVAL_INT,0,true);  break;
	    }
	    break;
	}
	case DI:
	{
	    char gval = EVAL_BOOL;
	    if( enableStat() )
	    {
		//- Direct reading -
		if( owner().dataEmul() )	gval = !((bool)rand()%3);
		else
		{
		    owner().dio_res.resRequestW( );
		    BYTE i_bt;
		    if( dscDIOInputBit(owner().dscb,m_dio_port>>4,m_dio_port&0x0f,&i_bt) != DE_NONE )
		    {
			ERRPARAMS errorParams;
			dscGetLastError(&errorParams);
			mess_err(nodePath().c_str(),_("dscDIOInputBit error: %s %s"), dscGetErrorString(errorParams.ErrCode),errorParams.errstring );
		    }
		    gval = i_bt;
		    owner().dio_res.resRelease( );
		}
	    }
	    val.setB(gval,0,true);
	    break;
	}
    }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(owner().ADIIntMode() ? TVArchive::PassiveAttr : TVArchive::ActiveAttr,
		val.arch().at().srcData());
    val.arch().at().setPeriod(owner().ADIIntMode() ? 1000000/owner().cfg("ADCONVRATE").getI() : 1000000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

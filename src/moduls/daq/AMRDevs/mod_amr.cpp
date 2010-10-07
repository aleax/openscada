
//OpenSCADA system module DAQ.AMRDevs file: mod_amr.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
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
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "da_FlowTEC.h"
#include "da_Ergomera.h"
#include "mod_amr.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"AMRDevs"
#define MOD_NAME	"AMR devices"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	SDAQ_VER
#define VERSION		"0.0.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow access to automatic meter reading devices. Supported devices: "
#define LICENSE		"GPL2"
//*************************************************

AMRDevs::TTpContr *AMRDevs::mod;  //Pointer for direct access to module

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
	    return new AMRDevs::TTpContr( source );
	return NULL;
    }
}

using namespace AMRDevs;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTpContr::~TTpContr()
{

}

void TTpContr::load_( )
{

}

void TTpContr::save_( )
{

}

uint8_t TTpContr::CRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

uint8_t TTpContr::CRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t TTpContr::CRC16( const string &mbap )
{
    uint8_t hi = 0xFF;
    uint8_t lo = 0xFF;
    uint16_t index;
    for( int i_b = 0; i_b < mbap.size(); i_b++ )
    {
	index = lo^(uint8_t)mbap[i_b];
	lo = hi^CRCHi[index];
	hi = CRCLo[index];
    }
    return hi|(lo<<8);
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("SCHEDULE",_("Calc schedule"),TFld::String,TFld::NoFlag,"100","1") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );
    fldAdd( new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"3","30") );
    fldAdd( new TFld("REQ_TRY",_("Request tries"),TFld::Integer,TFld::NoFlag,"1","3","1;10") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("DEV_TP",_("Device type"),TFld::String,TCfg::NoVal,"20") );
    tpPrmAt(t_prm).fldAdd( new TFld("ADDR",_("Transport's address"),TFld::String,TCfg::NoVal,"30","") );
    tpPrmAt(t_prm).fldAdd( new TFld("DEV_ADDR",_("Device address"),TFld::String,TCfg::NoVal,"50") );
    tpPrmAt(t_prm).fldAdd( new TFld("DEV_PRMS",_("Device addon parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"1000") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0),
	mSched(cfg("SCHEDULE").getSd()), mPrior(cfg("PRIOR").getId()),
	mRestTm(cfg("TM_REST").getId()), mConnTry(cfg("REQ_TRY").getId())
{
    cfg("PRM_BD").setS("AMRDevsPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );
    if( startStat() && !redntUse( ) )
    {
	if( period() ) val += TSYS::strMess(_("Call by period %g s. "),(1e-9*period()));
	else val += TSYS::strMess(_("Call by cron '%s'. "),cron().c_str());
	val += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    }
    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    if( prc_st ) return;

    //> Schedule process
    mPer = TSYS::strSepParse(mSched,1,' ').empty() ? vmax(0,(long long)(1e9*atof(mSched.c_str()))) : 0;

    //> Start the gathering data task
    SYS->taskCreate( nodePath('.',true), mPrior, TMdContr::Task, this, &prc_st );
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if( prc_st ) SYS->taskDestroy( nodePath('.',true), &prc_st, &endrun_req );
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

    while( !cntr.endrun_req )
    {
	long long t_cnt = TSYS::curTime();

	//> Update controller's data
	cntr.en_res.resRequestR( );
	for( unsigned i_p=0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++ )
	    try { cntr.p_hd[i_p].at().getVals( ); }
	    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resRelease( );

	cntr.tm_gath = 1e-3*(TSYS::curTime()-t_cnt);

	TSYS::taskSleep(cntr.period(),cntr.period()?0:TSYS::cron(cntr.cron()));
    }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),0664,"root","DAQ",4,"tp","str","dest","sel_ed",
	    "sel_list",_("1;1e-3;* * * * *;10 * * * *;10-20 2 */2 * *"),
	    "help",_("Schedule is writed in seconds periodic form or in standard Cron form.\n"
		    "Seconds form is one real number (1.5, 1e-3).\n"
		    "Cron it is standard form '* * * * *'. Where:\n"
		    "  - minutes (0-59);\n"
		    "  - hours (0-23);\n"
		    "  - days (1-31);\n"
		    "  - month (1-12);\n"
		    "  - week day (0[sunday]-6)."));
	return;
    }
    //> Process command to page
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : TParamContr(name,tp_prm), mDA(NULL), needApply(false),
    p_el("w_attr"), mAddr(cfg("ADDR").getSd()), devTp(cfg("DEV_TP").getSd()), devAddr(cfg("DEV_ADDR").getSd()), devPrms(cfg("DEV_PRMS").getSd())
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if( !vlElemPresent(&p_el) )	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    //> Delete DAQ parameter's attributes
    for( int i_f = 0; i_f < p_el.fldSize(); i_f++ )
    {
	try { p_el.fldDel(i_f); i_f--; }
	catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    //> Connect device's code
    if( devTp == "FLowTC_UGTAA55" )	mDA = new FlowTEC(this);
    else if( devTp == "Ergomera" )	mDA = new Ergomera(this);
    else throw TError(nodePath().c_str(),_("No one device selected."));

    owner().prmEn( id(), true );

    needApply = false;
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

    if( mDA ) delete mDA;
    mDA = NULL;

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);

    needApply = false;
}

string TMdPrm::extPrmGet( const string &prm, bool isText )
{
    try
    {
	XMLNode prmNd;
	ResAlloc res( nodeRes(), false );
	prmNd.load(devPrms);
	if( !isText ) return prmNd.attr(prm);
	return prmNd.childGet(prm)->text();
    } catch(...){ }
    return "";
}

void TMdPrm::extPrmSet( const string &prm, const string &val, bool isText, bool nApply )
{
    XMLNode prmNd("prms");
    ResAlloc res( nodeRes(), false );
    try{ prmNd.load(devPrms); } catch(...){ }
    if( !isText ) prmNd.setAttr(prm,val);
    else
    {
	XMLNode *pNd = prmNd.childGet(prm,0,true);
	if( !pNd ) pNd = prmNd.childAdd(prm);
	pNd->setText(val);
    }
    res.request(true);
    devPrms = prmNd.save(XMLNode::BrAllPast);
    modif();
    if( nApply && enableStat() ) needApply = true;
}

void TMdPrm::getVals( )
{
    if( mDA ) mDA->getVals();
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

bool TMdPrm::cfgChange( TCfg &icfg )
{
    TParamContr::cfgChange(icfg);

    if( icfg.name() == "DEV_TP" && enableStat() ) disable( );

    return true;
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/status",_("Status"),0444,"root","DAQ",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/DEV_TP",cfg("DEV_TP").fld().descr(),0664,"root","DAQ",3,"tp","str","dest","select","select","/prm/cfg/devLst");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",cfg("ADDR").fld().descr(),0664,"root","DAQ",3,"tp","str","dest","select","select","/prm/cfg/trLst");
	ctrRemoveNode(opt,"/prm/cfg/DEV_PRMS");
	if( mDA ) mDA->cntrCmdProc( opt );
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/status" && ctrChkNode(opt) )
    {
	string rez;
	if( !enableStat() )		rez = TSYS::strMess("2:%s. ",_("Disabled"));
	else if( !owner().startStat() )	rez = TSYS::strMess("1:%s. ",_("Enabled"));
	else if( mErr.getVal().empty() )rez = TSYS::strMess("0:%s. ",_("Processed"));
	else rez = TSYS::strMess("%s:%s. %s. ",
	    TSYS::strSepParse(mErr.getVal(),0,':').c_str(),_("Processed"),TSYS::strSepParse(mErr.getVal(),1,':').c_str());
	if( needApply )	rez += _("Need re-enable for configuration apply! ");
	opt->setText(rez);
    }
    else if( mDA && mDA->cntrCmdProc( opt ) ) ;
    else if( a_path == "/prm/cfg/devLst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","FLowTC_UGTAA55")->setText(_("FLowTEC UGT-AA55"));
	opt->childAdd("el")->setAttr("id","Ergomera")->setText(_("Ergomera"));
    }
    else if( a_path == "/prm/cfg/trLst" && ctrChkNode(opt) )
    {
	vector<string> sls;
	if( SYS->transport().at().modPresent("Serial") )
	    SYS->transport().at().at("Serial").at().outList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlGet( TVal &val )
{
    if( val.name() == "err" )
    {
	TParamContr::vlGet(val);
	if( val.getS(NULL,true) == "0" && !mErr.getVal().empty() ) val.setS(mErr.getVal(),0,true);
    }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod( owner().period() ? owner().period()/1e3 : 1000000 );
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}


//OpenSCADA system module DAQ.ICP_DAS file: ICP_module.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
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
#define MOD_NAME	_("ICP DAS hardware")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.7.2"
#define AUTORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow realisation of ICP DAS hardware support. Include I87000 and I-7000 DCON modules and I-8000 fast modules.")
#define LICENSE		"GPL2"
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
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTpContr::~TTpContr()
{

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
    fldAdd( new TFld("REQ_TRY",_("Serial request tries"),TFld::Integer,TFld::NoFlag,"1","3","1;10") );

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
TMdContr::TMdContr(string name_c, const string &daq_db, TElem *cfgelem) :
	TController(name_c, daq_db, cfgelem),
	mPer(cfg("PERIOD").getRd()), mPrior(cfg("PRIOR").getId()), mBus(cfg("BUS").getId()), mBaud(cfg("BAUD").getId()),
	connTry(cfg("REQ_TRY").getId()), mLPprms(cfg("LP_PRMS").getSd()),
	prcSt(false), endRunReq(false), tm_gath(0), mCurSlot(-1), numReq(0), numErr(0), numErrResp(0)
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

    if( startStat( ) && !redntUse( ) ) val += TSYS::strMess(_("Spent time: %s. Serial requests %g, errors %g. "),
	TSYS::time2str(tm_gath).c_str(),numReq,numErr);

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
	if( mBus == 0 )
	{
	    ResAlloc res( pBusRes, true );
	    if( Open_SlotAll() > 0 ) throw TError( nodePath().c_str(), _("Open All LP-slots error.") );
	    if( Open_Slot(9) > 0 ) { Close_SlotAll(); throw TError( nodePath().c_str(), _("Open LP-slot 9 error.") ); }
	}

	try
	{
	    if( Open_Com( (mBus?mBus:1), mBus?mBaud:115200, Data8Bit, NonParity, OneStopBit ) > 0 )
		throw TError( nodePath().c_str(), _("Open COM%d port error."), (mBus?mBus:1) );

	    //> Create and init serial transport
	    /*string trName = TSYS::strMess("%sCOM%d",MOD_ID,(mBus?mBus:1));
	    if( !SYS->transport().at().at("Serial").at().outPresent(trName) )
	    {
		SYS->transport().at().at("Serial").at().outAdd(trName);
		SYS->transport().at().at("Serial").at().outAt(trName).at().setAddr("/dev/ttySA0:115200:8N1");
	    }
	    tr = SYS->transport().at().at("Serial").at().outAt(trName);

	    if( tr.at().startStat() ) tr.at().stop();
	    tr.at().start();*/

	    numReq = numErr = numErrResp = 0;

	    //> Start the gathering data task
	    SYS->taskCreate( nodePath('.',true), mPrior, TMdContr::Task, this, &prcSt, 10 );
	}
	catch(TError err)
	{
	    if( mBus == 0 )	{ Close_Slot(9); Close_SlotAll(); }
	    throw;
	}
    }
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy( nodePath('.',true), &prcSt, &endRunReq );

    //tr.free();
    Close_Com( (mBus?mBus:1) );

    if( mBus == 0 )	{ pBusRes.resRequestW(); Close_Slot(9); Close_SlotAll(); pBusRes.resRelease(); }
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
    XMLNode prmNd("prms");
    try { prmNd.load(mLPprms); } catch(...){ }
    prmNd.setAttr(prm,vl);
    mLPprms = prmNd.save(XMLNode::BrAllPast);
    modif();
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res( en_res, true );

    unsigned i_prm;
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    float wTm = 0;

    TMdContr &cntr = *(TMdContr*)icntr;

    cntr.endRunReq = false;

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
		for(unsigned i_p = 0; i_p < cntr.p_hd.size(); i_p++) cntr.p_hd[i_p].at().getVals();
		res.release();

		//> Calc acquisition process time
		cntr.tm_gath = TSYS::curTime()-t_cnt;
	    }

	    //> Watchdog timer process
	    if( cntr.mBus == 0 && wTm > 0 ) { ResAlloc res( cntr.reqRes, true ); EnableWDT((int)(1e3*vmax(1.5*cntr.period(),wTm))); res.release(); }

	    cntr.prcSt = true;

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

string TMdContr::serReq( string req, char mSlot )
{
    ResAlloc res( reqRes, true );
    if( mBus == 0 && mSlot != mCurSlot ) { pBusRes.resRequestW(); ChangeToSlot(mSlot); mCurSlot = mSlot; pBusRes.resRelease(); }

    WORD wT;
    char szReceive[255];
    //bool errTm = false, errResp = false;

    numReq++;

    /*try
    {
	if( !tr.at().startStat() ) tr.at().start();
	req += "\r";
    }
    catch(...)	{ return ""; }*/

    //ResAlloc resN( tr.nodeRes(), true );
    for( int i_tr = 0; i_tr < vmax(1,vmin(10,connTry)); i_tr++ )
    {
	/*try
	{
	    int resp_len = tr.at().messIO( req.data(), req.size(), szReceive, sizeof(szReceive), 0, true );
	    string rez( szReceive, resp_len );

	    //> Wait tail
	    while( rez.size() < 2 || rez[rez.size()-1] != '\r' )
	    {
		try{ resp_len = tr.at().messIO( NULL, 0, szReceive, sizeof(szReceive), 0, true ); } catch(TError er){ break; }
		rez.append( szReceive, resp_len );
	    }
	    if( rez.size() < 2 || rez[rez.size()-1] != '\r' )	{ errResp = true; continue; }
	    return rez.substr(0,rez.size()-1);
	}
	catch(...) { errTm = true; continue; }*/

	if( !Send_Receive_Cmd(mBus?mBus:1,(char*)req.c_str(),szReceive,1,0,&wT) ) return szReceive;
    }

    //if( errTm )
    numErr++;
    //if( errResp ) numErrResp++;

    return "";
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/BUS",cfg("BUS").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select","/cntr/cfg/busLst");
	if(cfg("BAUD").view())
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/BAUD",cfg("BAUD").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","sel_ed","select","/cntr/cfg/boudLst");
	if(mBus == 0 && ctrMkNode("area",opt,-1,"/LPcfg","LinPAC"))
	    ctrMkNode("fld",opt,-1,"/LPcfg/wTm",_("Watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,1,"tp","real");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/busLst" && ctrChkNode(opt))
	for(int i_s = 0; i_s < 11; i_s++)
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(i_s))->setText("COM "+TSYS::int2str(i_s?i_s:1)+(i_s==0?_(" (Master LP-8x81)"):""));
    else if(a_path == "/cntr/cfg/boudLst" && ctrChkNode(opt))
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
    else if(mBus == 0 && a_path == "/LPcfg/wTm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(prmLP("wTm"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setPrmLP("wTm",opt->text());
    }
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm(string name, TTipParam *tp_prm) :
    TParamContr(name, tp_prm), p_el("w_attr"), modTp(cfg("MOD_TP").getId()), modAddr(cfg("MOD_ADDR").getId()),
    modSlot(cfg("MOD_SLOT").getId()), modPrms(cfg("MOD_PRMS").getSd()),
    endRunReq(false), prcSt(false), clcCnt(0), extPrms(NULL), wTm(0), dOutRev(0), dInRev(0)
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
    if(enableStat())	return;

    TParamContr::enable();

    loadExtPrms();

    //> Delete DAQ parameter's attributes
    for(int i_f = 0; i_f < (int)p_el.fldSize(); i_f++)
    {
	if(atoi(p_el.fldAt(i_f).reserve().c_str()) == modTp) continue;
	try
	{
	    p_el.fldDel(i_f);
	    i_f--;
	}
	catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    //> Make DAQ parameter's attributes
    switch(modTp)
    {
	case 0x8781:
	    p_el.fldAdd(new TFld("serial",_("Serial number"),TFld::String,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()));
	    p_el.fldAdd(new TFld("SDK",_("SDK version"),TFld::Real,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()));
	    p_el.fldAdd(new TFld("DIP",_("DIP switch"),TFld::Integer,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()));
	    break;
	case 0x8017:
	    for(int i_i = 0; i_i < 8; i_i++)
	    {
		p_el.fldAdd(new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()));
		p_el.fldAdd(new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()));
		p_el.fldAdd(new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()));
	    }
	    break;
	case 0x8042:
	    for( int i_i = 0; i_i < 16; i_i++ )
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Boolean,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()) );
	    for( int i_o = 0; i_o < 16; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()) );
	    break;
	case 0x87019:
	    p_el.fldAdd( new TFld("cvct",_("Cold-Junction Compensation(CJC) temperature"),TFld::Real,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()) );
	    for( int i_i = 0; i_i < 8; i_i++ )
	    {
		p_el.fldAdd( new TFld(TSYS::strMess("i%d",i_i).c_str(),TSYS::strMess(_("Input %d"),i_i).c_str(),TFld::Real,TFld::NoWrite,"","","","",TSYS::int2str(modTp).c_str()) );
		p_el.fldAdd( new TFld(TSYS::strMess("ha%d",i_i).c_str(),TSYS::strMess(_("H/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()) );
		p_el.fldAdd( new TFld(TSYS::strMess("la%d",i_i).c_str(),TSYS::strMess(_("L/A %d"),i_i).c_str(),TFld::Boolean,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()) );
	    }
	    break;
	case 0x87024:
	    for( int i_o = 0; i_o < 4; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Real,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()) );
	    break;
	case 0x87057:
	    for( int i_o = 0; i_o < 16; i_o++ )
		p_el.fldAdd( new TFld(TSYS::strMess("o%d",i_o).c_str(),TSYS::strMess(_("Out %d"),i_o).c_str(),TFld::Boolean,TVal::DirWrite,"","","","",TSYS::int2str(modTp).c_str()) );
	    break;
	default: break;
    }

    owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if(!enableStat())  return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);

    //> Stop fast task
    if(prcSt) SYS->taskDestroy(nodePath('.',true), &prcSt, &endRunReq);

    //> Free module object
    switch(modTp)
    {
	case 0x8017:	delete ((PrmsI8017*)extPrms); extPrms = NULL; break;
    }
}

void TMdPrm::loadExtPrms( )
{
    if(!enableStat())	return;

    XMLNode prmNd;
    try{ prmNd.load(modPrms); } catch(...){ }
    string  vl;

    //> Generic
    vl = prmNd.attr("wTm"); if( !vl.empty() ) wTm = vmin(25.5,vmax(0,atof(vl.c_str())));
    vl = prmNd.attr("dInRev"); if( !vl.empty() ) dInRev = atoi(vl.c_str());
    vl = prmNd.attr("dOutRev"); if( !vl.empty() ) dOutRev = atoi(vl.c_str());

    //> By module
    switch(modTp)
    {
	case 0x8017:
	    if(!extPrms) extPrms = new PrmsI8017();
	    vl = prmNd.attr("cnls"); if(!vl.empty()) ((PrmsI8017*)extPrms)->prmNum = vmin(8,vmax(0,atoi(vl.c_str())));
	    vl = prmNd.attr("fastPer"); if(!vl.empty()) ((PrmsI8017*)extPrms)->fastPer = atof(vl.c_str());
	    for(unsigned i_n = 0; i_n < prmNd.childSize(); i_n++)
		if(prmNd.childGet(i_n)->name() == "cnl")
		    ((PrmsI8017*)extPrms)->cnlMode[atoi(prmNd.childGet(i_n)->attr("id").c_str())] = atoi(prmNd.childGet(i_n)->text().c_str());
	    break;
    }
}

void TMdPrm::saveExtPrms( )
{
    if( !enableStat() )	return;

    XMLNode prmNd("prms");

    //> Generic
    prmNd.setAttr("wTm",TSYS::real2str(wTm));
    prmNd.setAttr("dInRev",TSYS::int2str(dInRev));
    prmNd.setAttr("dOutRev",TSYS::int2str(dOutRev));

    //> By module
    switch( modTp )
    {
	case 0x8017:
	    if( !extPrms ) return;
	    prmNd.setAttr("cnls",TSYS::int2str(((PrmsI8017*)extPrms)->prmNum));
	    prmNd.setAttr("fastPer",TSYS::real2str(((PrmsI8017*)extPrms)->fastPer,5));
	    for( int i_c = 0; i_c < 8; i_c++ )
		prmNd.childAdd("cnl")->setAttr("id",TSYS::int2str(i_c))->setText(TSYS::int2str(((PrmsI8017*)extPrms)->cnlMode[i_c]));
	    break;
    }

    modPrms = prmNd.save(XMLNode::BrAllPast);
    modif();
}

void TMdPrm::getVals( )
{
    string rez;

    switch( modTp )
    {
	case 0x8781:
	{
	    //> Read serial number
	    AutoHD<TVal> vl = vlAt("serial");
	    if( vl.at().getS() == EVAL_STR )
	    {
		ResAlloc res( owner().reqRes, true );
		unsigned char sN[8];
		Read_SN(sN);
		res.release();
		vl.at().setS( TSYS::strMess("%x%x%x%x%x%x%x%x",sN[0],sN[1],sN[2],sN[3],sN[4],sN[5],sN[6],sN[7]), 0, true );
	    }
	    //> Read SDK version
	    vl = vlAt("SDK");
	    if( vl.at().getR() == EVAL_REAL )	vl.at().setR( GetSDKversion(), 0, true );

	    //> Read DIP switch status
	    owner().pBusRes.resRequestW();
	    int dpSw = GetDIPswitch();
	    owner().pBusRes.resRelease();
	    vlAt("DIP").at().setI(dpSw,0,true);
	    break;
	}
	case 0x8017:
	{
	    //> Check for I8017 init
	    if( !((PrmsI8017*)extPrms)->init )
	    { owner().pBusRes.resRequestW(); I8017_Init(modSlot); ((PrmsI8017*)extPrms)->init = true; owner().pBusRes.resRelease(); }
	    //> Check for I8017 fast task start
	    if( ((PrmsI8017*)extPrms)->fastPer && !prcSt ) SYS->taskCreate( nodePath('.',true), 32, fastTask, this, &prcSt );
	    //> Get values direct
	    for( int i_v = 0; i_v < 8; i_v++ )
		if( i_v >= ((PrmsI8017*)extPrms)->prmNum ) vlAt(TSYS::strMess("i%d",i_v)).at().setR(EVAL_REAL,0,true);
		else if( !((PrmsI8017*)extPrms)->fastPer )
		{
		    ResAlloc res( owner().pBusRes, true );
		    I8017_SetChannelGainMode(modSlot,i_v,((PrmsI8017*)extPrms)->cnlMode[i_v],0);
		    vlAt(TSYS::strMess("i%d",i_v)).at().setR( I8017_GetCurAdChannel_Float_Cal(modSlot), 0, true );
		}
	    break;
	}
	case 0x8042:
	{
	    owner().pBusRes.resRequestW();
	    int c_vl = DI_16(modSlot);
	    owner().pBusRes.resRelease();
	    for( int i_v = 0; i_v < 16; i_v++ ) vlAt(TSYS::strMess("i%d",i_v)).at().setB( ((dInRev^c_vl)>>i_v)&0x01, 0, true );
	    owner().pBusRes.resRequestW();
	    c_vl = DO_16_RB(modSlot);
	    owner().pBusRes.resRelease();
	    for( int o_v = 0; o_v < 16; o_v++ ) vlAt(TSYS::strMess("o%d",o_v)).at().setB( ((dOutRev^c_vl)>>o_v)&0x01, 0, true );
	    break;
	}
	case 0x87019:
	    rez = owner().serReq( TSYS::strMess("#%02X",(owner().mBus==0)?0:modAddr), modSlot );
	    for( int i_v = 0; i_v < 8; i_v++ )
		vlAt(TSYS::strMess("i%d",i_v)).at().setR( (rez.size() != 57 || rez[0] != '>') ? EVAL_REAL : atof(rez.data()+1+7*i_v), 0, true );

	    if( !rez.empty() )	rez = owner().serReq( TSYS::strMess("$%02X3",(owner().mBus==0)?0:modAddr), modSlot );
	    vlAt("cvct").at().setR( (rez.size() != 8 || rez[0] != '>') ? EVAL_REAL : atof(rez.data()+1), 0, true );
	    acq_err.setVal( rez.empty() ? _("10:Request to module error.") : ((rez[0]!='>')?_("11:Respond from module error."):""));

	    break;
	case 0x87024:
	    //> Host watchdog processing
	    rez = "1";
	    if( wTm >= 0.1 )
	    {
		rez = owner().serReq( TSYS::strMess("~%02X31%02X",(owner().mBus==0)?0:modAddr,(int)(10*wTm)), modSlot );
		if( rez.empty() ) { acq_err.setVal(_("10:Request to module error.")); break; }
	    }

	    //> Get data
	    for( int i_v = 0; i_v < 4; i_v++ )
	    {
		if( !rez.empty() ) rez = owner().serReq( TSYS::strMess("$%02X8%d",(owner().mBus==0)?0:modAddr,i_v), modSlot );
		vlAt(TSYS::strMess("o%d",i_v)).at().setR( (rez.size() != 10 || rez[0]!='!') ? EVAL_REAL : atof(rez.data()+3), 0, true );
	    }
	    acq_err.setVal( rez.empty()?_("10:Request to module error."):"" );

	    break;
	case 0x87057:
	{
	    //> Host watchdog processing
	    rez = "1";
	    if( wTm >= 0.1 )
	    {
		rez = owner().serReq( TSYS::strMess("~%02X31%02X",(owner().mBus==0)?0:modAddr,(int)(10*wTm)), modSlot );
		if( rez.empty() ) { acq_err.setVal(_("10:Request to module error.")); break; }
	    }

	    //> Get data
	    if( !rez.empty() ) rez = owner().serReq( TSYS::strMess("$%02X6",(owner().mBus==0)?0:modAddr), modSlot );
	    int vl = -1;
	    if( !(rez.size() != 7 || rez[0] != '!') )
	    {
		vl = strtoul(rez.substr(1,4).c_str(),NULL,16);
		acq_err.setVal("");
	    }
	    else acq_err.setVal(rez.empty()?_("10:Request to module error."):_("11:Respond from module error."));

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
    string rez;

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
    try
    {
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
	    owner().pBusRes.resRequestW(1000);
	    I8017_SetLed(modSlot,(lvl<<8)|hvl);
	    owner().pBusRes.resRelease();
	    break;
	}
	case 0x87019:
	{
	    bool ha = (valo.name().substr(0,2) == "ha");
	    bool la = (valo.name().substr(0,2) == "la");
	    if( !(ha||la) )	break;

	    //> Create previous value
	    int hvl = 0, lvl = 0;
	    for( int i_v = 7; i_v >= 0; i_v-- )
	    {
		hvl = hvl << 1;
		lvl = lvl << 1;
		if( vlAt(TSYS::strMess("ha%d",i_v)).at().getB() == true )	hvl |= 1;
		if( vlAt(TSYS::strMess("la%d",i_v)).at().getB() == true )	lvl |= 1;
	    }

	    rez = owner().serReq( TSYS::strMess("@%02XL%02X%02X",(owner().mBus==0)?0:modAddr,lvl,hvl), modSlot );
	    acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
	    break;
	}
	case 0x8042:
	{
	    bool vl = valo.getB(0,true);
	    if( vl == EVAL_BOOL || vl == pvl.getB() ) break;
	    int chnl = atoi(valo.name().c_str()+1);

	    owner().pBusRes.resRequestW(1000);
	    DO_16( modSlot, ((vl^(dOutRev>>chnl))&1) ? (DO_16_RB(modSlot) | 0x01<<chnl) : (DO_16_RB(modSlot) & ~(0x01<<chnl)) );
	    owner().pBusRes.resRelease();
	    break;
	}
	case 0x87024:
	{
	    double vl = valo.getR(0,true);
	    if( vl == EVAL_REAL || vl == pvl.getR() ) break;

	    string cmd = TSYS::strMess("#%02X%d%+07.3f",(owner().mBus==0)?0:modAddr,atoi(valo.name().c_str()+1),vl);

	    rep24:
	    rez = owner().serReq( cmd, modSlot );
	    //> Set watchdog flag is process
	    if( !rez.empty() && rez[0] == '!' )
	    {
		owner().serReq( TSYS::strMess("~%02X1",(owner().mBus==0)?0:modAddr), modSlot );
		goto rep24;
	    }
	    valo.setR( (rez.empty() || rez[0]!='>') ? EVAL_REAL : vl, 0, true );
	    acq_err.setVal(rez.empty()?_("10:Request to module error."):"");
	    break;
	}
	case 0x87057:
	{
	    char vl = valo.getB(0,true);
	    if( !acq_err.getVal().empty() || vl == EVAL_BOOL || vl == pvl.getB() ) break;

	    int daddr = atoi(valo.name().c_str()+1);
	    string cmd = TSYS::strMess("#%02X%s%d%02X",(owner().mBus==0)?0:modAddr,(daddr/8)?"B":"A",daddr%8,(vl^(dOutRev>>daddr))&1);

	    rep57:
	    rez = owner().serReq( cmd, modSlot );
	    //> Set watchdog flag is process
	    if( !rez.empty() && rez[0] == '!' )
	    {
		owner().serReq( TSYS::strMess("~%02X1",(owner().mBus==0)?0:modAddr), modSlot );
		goto rep57;
	    }

	    valo.setB( (rez.empty() || rez[0]!='>') ? EVAL_BOOL : vl, 0, true );
	    acq_err.setVal(rez.empty()?_("10:Request to module error."):((rez[0]!='>')?_("11:Respond from module error."):""));
	    break;
	}
    }
    }catch( TError err )
    {
	mess_err(nodePath().c_str(),_("Write value to attribute '%s' error: %s"),valo.name().c_str(),err.mess.c_str());
	valo.setS( pvl.getS(), 0, true );
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

    prm.endRunReq = false;
    prm.prcSt = true;

    struct timespec sp_tm;
    long long wTm = TSYS::curTime();
    int c_mode;

    vector< AutoHD<TVal> > cnls;
    for(int i_c = 0; i_c < ((PrmsI8017*)prm.extPrms)->prmNum; i_c++)
	cnls.push_back(prm.vlAt(TSYS::strMess("i%d",i_c)));
    float vbuf[cnls.size()];

    while(!prm.endRunReq)
    {
	prm.owner().pBusRes.resRequestW( );
	for(unsigned i_c = 0; prm.owner().startStat() && i_c < cnls.size(); i_c++)
	{
	    c_mode = ((PrmsI8017*)prm.extPrms)->cnlMode[i_c];
	    I8017_SetChannelGainMode(prm.modSlot,i_c,c_mode,0);
	    vbuf[i_c] = (10.0/(c_mode?2*c_mode:1))*(float)I8017HW_GetCurAdChannel_Hex(prm.modSlot)/8000;
	}
	prm.owner().pBusRes.resRelease( );

	for(unsigned i_c = 0; prm.owner().startStat() && i_c < cnls.size(); i_c++)
	    cnls[i_c].at().setR(vbuf[i_c], wTm, true);

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
    string rez;

    //> Get page info
    if(opt->name() == "info")
    {
	cfg("MOD_SLOT").setView( modTp != 0x8781 );
	cfg("MOD_ADDR").setView( (modTp>>12) != 8 && owner().mBus != 0 );
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/MOD_TP",cfg("MOD_TP").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select","/prm/cfg/modLst");
	switch(modTp)
	{
	    case 0x8017:
		if(!enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) break;
		ctrMkNode("fld",opt,-1,"/cfg/prms",_("Process parameters"),RWRWR_,"root",SDAQ_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/cfg/fastPer",_("Fast data get period (s)"),RWRWR_,"root",SDAQ_ID,1,"tp","real");
		if(ctrMkNode("area",opt,-1,"/cfg/mode",_("Mode")))
		    for(int i_v = 0; i_v < 8; i_v++)
			ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/mode/in%d",i_v).c_str(),TSYS::strMess(_("Input %d"),i_v).c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select","/cfg/tpLst");
		break;
	    case 0x8042:
		if(!enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) break;
		for(int i_o = 0; i_o < 16; i_o++)
		    ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/revIn%d",i_o).c_str(),i_o?"":_("Inputs reverse"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		for(int i_o = 0; i_o < 16; i_o++)
		    ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/revOut%d",i_o).c_str(),i_o?"":_("Outputs reverse"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		break;
	    case 0x87019:
		if(!enableStat() || !owner().startStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) break;
		for(int i_v = 0; i_v < 8; i_v++)
		    ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/inTp%d",i_v).c_str(),TSYS::strMess(_("Input %d type"),i_v).c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select","/cfg/tpLst");
		break;
	    case 0x87024:
		if(!enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) break;
		ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,3,"tp","real","min","0","max","25.5");
		if(!owner().startStat() || !ctrMkNode("area",opt,-1,"/cfg/mod",_("Module"))) break;
		ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from curent"),RWRW__,"root",SDAQ_ID);
		ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from curent"),RWRW__,"root",SDAQ_ID);
		break;
	    case 0x87057:
		if(!enableStat() || !ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) break;
		for(int i_o = 0; i_o < 16; i_o++)
		    ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/revOut%d",i_o).c_str(),i_o?"":_("Out reverse"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/cfg/wTm",_("Host watchdog timeout (s)"),RWRWR_,"root",SDAQ_ID,3,"tp","real","min","0","max","25.5");
		if(!owner().startStat() || !ctrMkNode("area",opt,-1,"/cfg/mod",_("Module"))) break;
		ctrMkNode("fld",opt,-1,"/cfg/mod/wSt",_("Host watchdog status"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cfg/mod/vPon",_("Power on values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vPonSet",_("Set power on values from curent"),RWRW__,"root",SDAQ_ID);
		ctrMkNode("fld",opt,-1,"/cfg/mod/vSf",_("Safe values"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("comm",opt,-1,"/cfg/mod/vSfSet",_("Set safe values from curent"),RWRW__,"root",SDAQ_ID);
		break;
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/modLst" && ctrChkNode(opt))
    {
	if(owner().mBus == 0)
	{
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8781))->setText("LP-8781");
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8017))->setText("I-8017");
	    opt->childAdd("el")->setAttr("id",TSYS::int2str(0x8042))->setText("I-8042");
	}
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87019))->setText("I-87019");
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87024))->setText("I-87024");
	opt->childAdd("el")->setAttr("id",TSYS::int2str(0x87057))->setText("I-87057");
    }
    else if(modTp == 0x8017 && enableStat() && a_path == "/cfg/prms" )
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(TSYS::int2str(((PrmsI8017*)extPrms)->prmNum));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{ ((PrmsI8017*)extPrms)->prmNum = atoi(opt->text().c_str()); saveExtPrms(); }
    }
    else if(modTp == 0x8017 && enableStat() && a_path == "/cfg/fastPer")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(TSYS::real2str(((PrmsI8017*)extPrms)->fastPer,5));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{ ((PrmsI8017*)extPrms)->fastPer = atof(opt->text().c_str()); saveExtPrms(); }
    }
    else if(modTp == 0x8017 && enableStat() && a_path.substr(0,12) == "/cfg/mode/in")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(TSYS::int2str(((PrmsI8017*)extPrms)->cnlMode[atoi(a_path.substr(12).c_str())]));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{ ((PrmsI8017*)extPrms)->cnlMode[atoi(a_path.substr(12).c_str())] = atoi(opt->text().c_str()); saveExtPrms(); }
    }
    else if(modTp == 0x8017 && enableStat() && a_path == "/cfg/tpLst" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("-10V to +10V"));
	opt->childAdd("el")->setAttr("id","1")->setText(_("-5V to +5V"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("-2.5V to +2.5V"));
	opt->childAdd("el")->setAttr("id","3")->setText(_("-1.25V to +1.25V"));
	opt->childAdd("el")->setAttr("id","4")->setText(_("-20mA to +20mA (with 125 ohms resistor)"));
    }
    else if(modTp == 0x87019 && enableStat() && owner().startStat() && a_path.substr(0,9) == "/cfg/inTp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    rez = owner().serReq(TSYS::strMess("$%02X8C%d",(owner().mBus==0)?0:modAddr,atoi(a_path.substr(9).c_str())), modSlot);
	    opt->setText((rez.size()!=8||rez[0]!='!') ? "-1" : TSYS::int2str(strtol(rez.data()+6,NULL,16)));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	    owner().serReq(TSYS::strMess("$%02X7C%dR%02X",(owner().mBus==0)?0:modAddr,atoi(a_path.substr(9).c_str()),atoi(opt->text().c_str())), modSlot);
    }
    else if(modTp == 0x87019 && a_path == "/cfg/tpLst" && ctrChkNode(opt))
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
    else if(enableStat() && a_path.substr(0,10) == "/cfg/revIn")
    {
	int rin = atoi(a_path.substr(10).c_str());
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText((dInRev&(1<<rin))?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{ dInRev = atoi(opt->text().c_str()) ? (dInRev|(1<<rin)) : (dInRev & ~(1<<rin)); saveExtPrms(); }
    }
    else if(enableStat() && a_path.substr(0,11) == "/cfg/revOut")
    {
	int rout = atoi(a_path.substr(11).c_str());
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText((dOutRev&(1<<rout))?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{ dOutRev = atoi(opt->text().c_str()) ? (dOutRev|(1<<rout)) : (dOutRev & ~(1<<rout)); saveExtPrms(); }
    }
    else if(enableStat() && a_path == "/cfg/wTm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(TSYS::real2str(wTm));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	{ wTm = atof(opt->text().c_str()); saveExtPrms(); }
    }
    else if(enableStat() && owner().startStat() && a_path == "/cfg/mod/wSt" && ctrChkNode(opt))
    {
	string wSt;

	rez = owner().serReq(TSYS::strMess("~%02X0",(owner().mBus==0)?0:modAddr), modSlot);
	if(rez.size() == 5 && rez[0]=='!')
	{
	    wSt += (bool)strtol(rez.data()+3,NULL,16) ? _("Set. ") : _("Clear. ");

	    rez = owner().serReq(TSYS::strMess("~%02X2",(owner().mBus==0)?0:modAddr), modSlot);
	    if(rez.size() == 6 && rez[0]=='!')
	    {
		wSt += (bool)strtol(string(rez.data()+3,1).c_str(),NULL,16) ? _("Enabled, ") : _("Disabled, ");
		wSt += TSYS::real2str(0.1*strtol(rez.data()+4,NULL,16))+_(" s.");
	    }
	}
	opt->setText(wSt);
    }
    else if(modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPon" && ctrChkNode(opt))
    {
	string cnt;
	for(int i_c = 0; i_c < 4; i_c++)
	{
	    rez = owner().serReq( TSYS::strMess("$%02X7%d",(owner().mBus==0)?0:modAddr,i_c), modSlot );
	    if(rez.size() != 10 || rez[0] != '!') { cnt = _("Error"); break; }
	    cnt = cnt + (rez.data()+3) + " ";
	}
	opt->setText(cnt);
    }
    else if(modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPonSet" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
    {
	for(int i_c = 0; i_c < 4; i_c++)
	    owner().serReq(TSYS::strMess("$%02X4%d",(owner().mBus==0)?0:modAddr,i_c), modSlot);
    }
    else if(modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vSf" && ctrChkNode(opt))
    {
	string cnt;
	for(int i_c = 0; i_c < 4; i_c++)
	{
	    rez = owner().serReq(TSYS::strMess("~%02X4%d",(owner().mBus==0)?0:modAddr,i_c), modSlot);
	    if(rez.size() != 10 || rez[0] != '!') { cnt = _("Error"); break; }
	    cnt = cnt + (rez.data()+3) + " ";
	}
	opt->setText(cnt);
    }
    else if(modTp == 0x87024 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vSfSet" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
    {
	for(int i_c = 0; i_c < 4; i_c++)
	    owner().serReq(TSYS::strMess("~%02X5%d",(owner().mBus==0)?0:modAddr,i_c), modSlot);
    }
    else if(modTp == 0x87057 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPon" && ctrChkNode(opt))
    {
	rez = owner().serReq(TSYS::strMess("~%02X4P",(owner().mBus==0)?0:modAddr), modSlot);
	if(rez.size() != 7 || rez[0] != '!') opt->setText(_("Error"));
	else
	{
	    string cnt;
	    int vl = strtol(rez.data()+3,NULL,16);
	    for(int i_o = 0; i_o < 16; i_o++)	cnt += ((vl>>i_o)&0x01)?"1 ":"0 ";
	    opt->setText(cnt);
	}
    }
    else if(modTp == 0x87057 && enableStat() && owner().startStat() && a_path == "/cfg/mod/vPonSet" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	owner().serReq(TSYS::strMess("~%02X5P",(owner().mBus==0)?0:modAddr), modSlot);
    else if(modTp == 0x87057 && owner().startStat() && owner().startStat() && a_path == "/cfg/mod/vSf" && ctrChkNode(opt))
    {
	rez = owner().serReq( TSYS::strMess("~%02X4S",(owner().mBus==0)?0:modAddr), modSlot );
	if(rez.size() != 7 || rez[0] != '!') opt->setText(_("Error"));
	else
	{
	    string cnt;
	    int vl = strtol(rez.data()+3,NULL,16);
	    for(int i_o = 0; i_o < 16; i_o++)	cnt += ((vl>>i_o)&0x01)?"1 ":"0 ";
	    opt->setText(cnt);
	}
    }
    else if(modTp == 0x87057 && owner().startStat() && owner().startStat() && a_path == "/cfg/mod/vSfSet" && ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR))
	owner().serReq(TSYS::strMess("~%02X5S",(owner().mBus==0)?0:modAddr), modSlot);
    else TParamContr::cntrCmdProc(opt);
}

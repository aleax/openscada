
//OpenSCADA system module DAQ.ModBus file: modbus_daq.cpp
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <ttiparam.h>

#include "modbus_daq.h"

ModBus::TTpContr *ModBus::mod;

using namespace ModBus;

//******************************************************
//* TTpContr                                           *
//******************************************************
TTpContr::TTpContr( string name )
{
    mId		= DAQ_ID;
    mName	= DAQ_NAME;
    mType	= DAQ_TYPE;
    mVers	= DAQ_MVER;
    mAutor	= DAQ_AUTORS;
    mDescr	= DAQ_DESCR;
    mLicense	= DAQ_LICENSE;
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
	DAQ_TYPE,DAQ_ID,nodePath().c_str());

    return buf;
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable( flag );

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0.01;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("PROT",_("Modbus protocol"),TFld::String,TFld::Selected,"5","TCP","TCP;RTU;ASCII",_("TCP/IP;RTU;ASCII")) );
    fldAdd( new TFld("ADDR",_("Transport address"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("NODE",_("Destination node"),TFld::Integer,TFld::NoFlag,"20","1","0;255") );
    fldAdd( new TFld("FRAG_MERGE",_("Data fragments merge"),TFld::Boolean,TFld::NoFlag,"1","0") );
    fldAdd( new TFld("TM_REQ",_("Connection timeout (ms)"),TFld::Integer,TFld::NoFlag,"5","0") );
    fldAdd( new TFld("TM_REST",_("Restore timeout (s)"),TFld::Integer,TFld::NoFlag,"3","30") );
    fldAdd( new TFld("REQ_TRY",_("Request tries"),TFld::Integer,TFld::NoFlag,"1","3") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("ATTR_LS",_("Attributes list"),TFld::String,TFld::FullText|TCfg::NoVal|TCfg::TransltText,"100","") );
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
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
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
	TController( name_c, daq_db, cfgelem ), prc_st(false), endrun_req(false), tmGath(0), tmDelay(0),
	numRReg(0), numRRegIn(0), numRCoil(0), numRCoilIn(0), numWReg(0), numWCoil(0), numErrCon(0), numErrResp(0),
	mPer(cfg("PERIOD").getRd()), mPrior(cfg("PRIOR").getId()), mPrt(cfg("PROT").getSd()),
	mAddr(cfg("ADDR").getSd()), mNode(cfg("NODE").getId()), mMerge(cfg("FRAG_MERGE").getBd()),
	reqTm(cfg("TM_REQ").getId()), restTm(cfg("TM_REST").getId()), connTry(cfg("REQ_TRY").getId())
{
    cfg("PRM_BD").setS("ModBusPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );

    if( startStat( ) && !redntUse( ) )
    {
	if( tmDelay ) val += TSYS::strMess(_("Connection error. Restoring in %.6g s."),tmDelay);
	else val += TSYS::strMess(_("Gather data time %.6g ms. Read %g(%g) registers, %g(%g) coils. Write %g registers, %g coils. Errors of connection %g, of respond %g."),
				    tmGath,numRReg,numRRegIn,numRCoil,numRCoilIn,numWReg,numWCoil,numErrCon,numErrResp);
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm( name, &owner().tpPrmAt(type) );
}

void TMdContr::disable_( )
{
    //> Clear acquisition data block
    acqBlks.clear();
    acqBlksIn.clear();
    acqBlksCoil.clear();
    acqBlksCoilIn.clear();
}

void TMdContr::start_( )
{
    if( !prc_st )
    {
	//> Establish connection
	SYS->transport().at().at(TSYS::strSepParse(mAddr,0,'.')).at().outAt(TSYS::strSepParse(mAddr,1,'.')).at().start();

	//> Clear statistic
	numRReg = numRRegIn = numRCoil = numRCoilIn = numWReg = numWCoil = numErrCon = numErrResp = tmDelay = 0;

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

	//> Clear statistic
	numRReg = numRRegIn = numRCoil = numRCoilIn = numWReg = numWCoil = numErrCon = numErrResp = 0;
    }
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if( icfg.fld().name() == "PROT" )
    {
	if( icfg.getS() == "TCP" )	cfg("REQ_TRY").setView(false);
	else				cfg("REQ_TRY").setView(true);
	if(startStat())	stop();
    }
    else if( icfg.fld().name() == "FRAG_MERGE" && enableStat( ) ) disable( );

    return true;
}

void TMdContr::regVal( int reg, const string &dt )
{
    if( reg < 0 )	return;

    ResAlloc res( req_res, true );

    //> Register to acquisition block
    if( dt == "R" || dt == "RI" )
    {
	vector< SDataRec > &workCnt = (dt == "RI") ? acqBlksIn : acqBlks;
	int i_b;
	for( i_b = 0; i_b < workCnt.size(); i_b++ )
	{
	    if( (reg*2) < workCnt[i_b].off )
	    {
		if( (mMerge || (reg*2+2) >= workCnt[i_b].off) && (workCnt[i_b].val.size()+workCnt[i_b].off-(reg*2)) < MaxLenReq )
		{
		    workCnt[i_b].val.insert(0,workCnt[i_b].off-reg*2,0);
		    workCnt[i_b].off = reg*2;
		}
		else workCnt.insert(workCnt.begin()+i_b,SDataRec(reg*2,2));
	    }
	    else if( (reg*2+2) > (workCnt[i_b].off+workCnt[i_b].val.size()) )
	    {
		if( (mMerge || reg*2 <= (workCnt[i_b].off+workCnt[i_b].val.size())) && (reg*2+2-workCnt[i_b].off) < MaxLenReq )
		{
		    workCnt[i_b].val.append((reg*2+2)-(workCnt[i_b].off+workCnt[i_b].val.size()),0);
		    //>> Check for allow mergin to next block
		    if( !mMerge && i_b+1 < workCnt.size() && (workCnt[i_b].off+workCnt[i_b].val.size()) >= workCnt[i_b+1].off )
		    {
			workCnt[i_b].val.append(workCnt[i_b+1].val,workCnt[i_b].off+workCnt[i_b].val.size()-workCnt[i_b+1].off,string::npos);
			workCnt.erase(workCnt.begin()+i_b+1);
		    }
		}
		else continue;
	    }
	    break;
	}
	if( i_b >= workCnt.size() )
	    workCnt.insert(workCnt.begin()+i_b,SDataRec(reg*2,2));
    }
    //> Coils
    else if( dt == "C" || dt == "CI" )
    {
	vector< SDataRec > &workCnt = (dt == "CI") ? acqBlksCoilIn : acqBlksCoil;
	int i_b;
	for( i_b = 0; i_b < workCnt.size(); i_b++ )
	{
	    if( reg < workCnt[i_b].off )
	    {
		if( (mMerge || (reg+1) >= workCnt[i_b].off) && (workCnt[i_b].val.size()+workCnt[i_b].off-reg) < MaxLenReq*8 )
		{
		    workCnt[i_b].val.insert(0,workCnt[i_b].off-reg,0);
		    workCnt[i_b].off = reg;
		}
		else workCnt.insert(workCnt.begin()+i_b,SDataRec(reg,1));
	    }
	    else if( (reg+1) > (workCnt[i_b].off+workCnt[i_b].val.size()) )
	    {
		if( (mMerge || reg <= (workCnt[i_b].off+workCnt[i_b].val.size())) && (reg+1-workCnt[i_b].off) < MaxLenReq*8 )
		{
		    workCnt[i_b].val.append((reg+1)-(workCnt[i_b].off+workCnt[i_b].val.size()),0);
		    //>> Check for allow mergin to next block
		    if( !mMerge && i_b+1 < workCnt.size() && (workCnt[i_b].off+workCnt[i_b].val.size()) >= workCnt[i_b+1].off )
		    {
			workCnt[i_b].val.append(workCnt[i_b+1].val,workCnt[i_b].off+workCnt[i_b].val.size()-workCnt[i_b+1].off,string::npos);
			workCnt.erase(workCnt.begin()+i_b+1);
		    }
		}
		else continue;
	    }
	    break;
	}
	if( i_b >= workCnt.size() )
	    workCnt.insert(workCnt.begin()+i_b,SDataRec(reg,1));
    }
}

int TMdContr::getValR( int addr, string &err, bool in )
{
    int rez = EVAL_INT;
    vector< SDataRec >	&workCnt = in ? acqBlksIn : acqBlks;
    for( int i_b = 0; i_b < workCnt.size(); i_b++ )
	if( (addr*2) >= workCnt[i_b].off && (addr*2+2) <= (workCnt[i_b].off+workCnt[i_b].val.size()) )
	{
	    err = workCnt[i_b].err;
	    if( err.empty() )
		rez = (unsigned short)(workCnt[i_b].val[addr*2-workCnt[i_b].off]<<8)|(unsigned char)workCnt[i_b].val[addr*2-workCnt[i_b].off+1];
	    break;
	}
    return rez;
}

char TMdContr::getValC( int addr, string &err, bool in )
{
    char rez = EVAL_BOOL;
    vector< SDataRec >	&workCnt = in ? acqBlksCoilIn : acqBlksCoil;
    for( int i_b = 0; i_b < workCnt.size(); i_b++ )
	if( addr >= workCnt[i_b].off && (addr+1) <= (workCnt[i_b].off+workCnt[i_b].val.size()) )
	{
	    err = workCnt[i_b].err;
	    if( err.empty() )
		rez = workCnt[i_b].val[addr-workCnt[i_b].off];
	    break;
	}
    return rez;
}

void TMdContr::setValR( int val, int addr, string &err )
{
    //> Encode request PDU (Protocol Data Units)
    string pdu;
    pdu = (char)0x6;		//Function, preset single register
    pdu += (char)(addr>>8);	//Address MSB
    pdu += (char)addr;		//Address LSB
    pdu += (char)(val>>8);	//Data MSB
    pdu += (char)val;		//Data LSB
    //> Request to remote server
    err = modBusReq( pdu );
    if( err.empty() ) numWReg++;
    //> Set to acquisition block
    for( int i_b = 0; i_b < acqBlks.size(); i_b++ )
	if( (addr*2) >= acqBlks[i_b].off && (addr*2+2) <= (acqBlks[i_b].off+acqBlks[i_b].val.size()) )
	{
	    acqBlks[i_b].val[addr*2-acqBlks[i_b].off]   = (char)(val>>8);
	    acqBlks[i_b].val[addr*2-acqBlks[i_b].off+1] = (char)val;
	    break;
	}
}

void TMdContr::setValC( char val, int addr, string &err )
{
    //> Encode request PDU (Protocol Data Units)
    string pdu;
    pdu = (char)0x5;		//Function, preset single coil
    pdu += (char)(addr>>8);	//Address MSB
    pdu += (char)addr;		//Address LSB
    pdu += (char)val?0xFF:0x00;	//Data MSB
    pdu += (char)0x00;		//Data LSB
    //> Request to remote server
    err = modBusReq( pdu );
    if( err.empty() ) numWCoil++;
    //> Set to acquisition block
    for( int i_b = 0; i_b < acqBlksCoil.size(); i_b++ )
	if( addr >= acqBlksCoil[i_b].off && (addr+1) <= (acqBlksCoil[i_b].off+acqBlksCoil[i_b].val.size()) )
	{
	    acqBlksCoil[i_b].val[addr-acqBlksCoil[i_b].off] = val;
	    break;
	}
}

string TMdContr::modBusReq( string &pdu )
{
    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(mAddr,0,'.')).at().outAt(TSYS::strSepParse(mAddr,1,'.'));
    //if( !tr.at().startStat() ) tr.at().start();

    XMLNode req(mPrt);
    req.setAttr("id",id())->
	setAttr("reqTm",TSYS::int2str(reqTm))->
	setAttr("node",TSYS::int2str(mNode))->
	setAttr("reqTry",TSYS::int2str(connTry))->
	setText(pdu);

    tr.at().messProtIO(req,"ModBus");

    if( !req.attr("err").empty() )
    {
	if( atoi(req.attr("err").c_str()) == 14 ) numErrCon++;
	else numErrResp++;
	return req.attr("err");
    }
    pdu = req.text();
    return "";
}

void *TMdContr::Task( void *icntr )
{
    string pdu;
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {
	while( !cntr.endrun_req )
	{
	    long long t_cnt = TSYS::curTime();

	    if( cntr.tmDelay > 0 )	cntr.tmDelay-=cntr.period();
	    else
	    {
#if OSC_DEBUG >= 3
		mess_debug(cntr.nodePath().c_str(),_("Fetch coils' and registers' blocks."));
#endif
		ResAlloc res( cntr.req_res, false );

		//> Get coils
		for( int i_b = 0; i_b < cntr.acqBlksCoil.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    if( cntr.redntUse( ) ) { cntr.acqBlksCoil[i_b].err = _("4:Server failure."); continue; }
		    //>> Encode request PDU (Protocol Data Units)
		    pdu = (char)0x1;					//Function, read multiple coils
		    pdu += (char)(cntr.acqBlksCoil[i_b].off>>8);	//Address MSB
		    pdu += (char)cntr.acqBlksCoil[i_b].off;		//Address LSB
		    pdu += (char)(cntr.acqBlksCoil[i_b].val.size()>>8);	//Number of coils MSB
		    pdu += (char)cntr.acqBlksCoil[i_b].val.size();	//Number of coils LSB
		    //>> Request to remote server
		    cntr.acqBlksCoil[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlksCoil[i_b].err.empty() )
		    {
			for( int i_c = 0; i_c < cntr.acqBlksCoil[i_b].val.size(); i_c++ )
			    cntr.acqBlksCoil[i_b].val[i_c] = (bool)((pdu[2+i_c/8]>>(i_c%8))&0x01);
			cntr.numRCoil += cntr.acqBlksCoil[i_b].val.size();
		    }
		    else if( atoi(cntr.acqBlksCoil[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlksCoil[i_b].err);
			break;
		    }
		}
		if( cntr.tmDelay > 0 )	continue;
		//> Get input's coils
		for( int i_b = 0; i_b < cntr.acqBlksCoilIn.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    if( cntr.redntUse( ) ) { cntr.acqBlksCoilIn[i_b].err = _("4:Server failure."); continue; }
		    //>> Encode request PDU (Protocol Data Units)
		    pdu = (char)0x2;					//Function, read multiple input's coils
		    pdu += (char)(cntr.acqBlksCoilIn[i_b].off>>8);	//Address MSB
		    pdu += (char)cntr.acqBlksCoilIn[i_b].off;		//Address LSB
		    pdu += (char)(cntr.acqBlksCoilIn[i_b].val.size()>>8);	//Number of coils MSB
		    pdu += (char)cntr.acqBlksCoilIn[i_b].val.size();	//Number of coils LSB
		    //>> Request to remote server
		    cntr.acqBlksCoilIn[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlksCoilIn[i_b].err.empty() )
		    {
			for( int i_c = 0; i_c < cntr.acqBlksCoilIn[i_b].val.size(); i_c++ )
			    cntr.acqBlksCoilIn[i_b].val[i_c] = (bool)((pdu[2+i_c/8]>>(i_c%8))&0x01);
			cntr.numRCoilIn += cntr.acqBlksCoilIn[i_b].val.size();
		    }
		    else if( atoi(cntr.acqBlksCoilIn[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlksCoilIn[i_b].err);
			break;
		    }
		}
		if( cntr.tmDelay > 0 )	continue;
		//> Get registers
		for( int i_b = 0; i_b < cntr.acqBlks.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    if( cntr.redntUse( ) ) { cntr.acqBlks[i_b].err = _("4:Server failure."); continue; }
		    //>> Encode request PDU (Protocol Data Units)
		    pdu = (char)0x3;				//Function, read multiple registers
		    pdu += (char)((cntr.acqBlks[i_b].off/2)>>8);	//Address MSB
		    pdu += (char)(cntr.acqBlks[i_b].off/2);		//Address LSB
		    pdu += (char)((cntr.acqBlks[i_b].val.size()/2)>>8);	//Number of registers MSB
		    pdu += (char)(cntr.acqBlks[i_b].val.size()/2);	//Number of registers LSB
		    //>> Request to remote server
		    cntr.acqBlks[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlks[i_b].err.empty() )
		    {
			cntr.acqBlks[i_b].val.replace(0,cntr.acqBlks[i_b].val.size(),pdu.substr(2).c_str(),cntr.acqBlks[i_b].val.size());
			cntr.numRReg += cntr.acqBlks[i_b].val.size()/2;
		    }
		    else if( atoi(cntr.acqBlks[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlks[i_b].err);
			break;
		    }
		}
		if( cntr.tmDelay > 0 )	continue;
		//> Get input registers
		for( int i_b = 0; i_b < cntr.acqBlksIn.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    if( cntr.redntUse( ) ) { cntr.acqBlksIn[i_b].err = _("4:Server failure."); continue; }
		    //>> Encode request PDU (Protocol Data Units)
		    pdu = (char)0x4;				//Function, read multiple input registers
		    pdu += (char)((cntr.acqBlksIn[i_b].off/2)>>8);	//Address MSB
		    pdu += (char)(cntr.acqBlksIn[i_b].off/2);		//Address LSB
		    pdu += (char)((cntr.acqBlksIn[i_b].val.size()/2)>>8);	//Number of registers MSB
		    pdu += (char)(cntr.acqBlksIn[i_b].val.size()/2);	//Number of registers LSB
		    //>> Request to remote server
		    cntr.acqBlksIn[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlksIn[i_b].err.empty() )
		    {
			cntr.acqBlksIn[i_b].val.replace(0,cntr.acqBlksIn[i_b].val.size(),pdu.substr(2).c_str(),cntr.acqBlksIn[i_b].val.size());
			cntr.numRRegIn += cntr.acqBlksIn[i_b].val.size()/2;
		    }
		    else if( atoi(cntr.acqBlksIn[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlksIn[i_b].err);
			break;
		    }
		}
		res.release();
	    }

	    //> Calc acquisition process time
	    cntr.tmGath = 1e-3*(TSYS::curTime()-t_cnt);

	    TSYS::taskSleep((long long)(1e9*cntr.period()));
	}
    }
    catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::setCntrDelay( const string &err )
{
    tmDelay = restTm;
    ResAlloc res( req_res, false );
    for( int i_b = 0; i_b < acqBlksCoil.size(); i_b++ )	acqBlksCoil[i_b].err = err;
    for( int i_b = 0; i_b < acqBlks.size(); i_b++ )	acqBlks[i_b].err = err;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/cntr/cfg/trLst");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/trLst" && ctrChkNode(opt) )
    {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr( name, tp_prm ), p_el("w_attr"), m_attrLs(cfg("ATTR_LS").getSd())
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
    if( enableStat() )	return;

    TParamContr::enable();

    //- Parse ModBus attributes and convert to strong list -
    vector<string> als;
    int  ai;
    string sel, atp, aid, anm, awr;
    for( int ioff = 0; (sel=TSYS::strSepParse(m_attrLs,0,'\n',&ioff)).size(); )
    {
	atp = TSYS::strSepParse(sel,0,':');
	if( atp.empty() ) atp = "R";
	ai  = strtol(TSYS::strSepParse(sel,1,':').c_str(),NULL,0);
	awr = TSYS::strSepParse(sel,2,':');
	aid = TSYS::strSepParse(sel,3,':');
	if( aid.empty() ) aid = TSYS::int2str(ai);
	anm = TSYS::strSepParse(sel,4,':');
	if( anm.empty() ) anm = TSYS::int2str(ai);

	if( vlPresent(aid) && !p_el.fldPresent(aid) )	continue;
	TFld::Type	tp  = (atp[0]=='C') ? TFld::Boolean : TFld::Integer;
	if( !p_el.fldPresent(aid) || p_el.fldAt(p_el.fldId(aid)).type() != tp )
	{
	    if( p_el.fldPresent(aid)) p_el.fldDel(p_el.fldId(aid));
	    p_el.fldAdd( new TFld(aid.c_str(),"",tp,TFld::NoFlag,"",TSYS::int2str((atp[0]=='C')?EVAL_BOOL:EVAL_INT).c_str()) );
	}
	int el_id = p_el.fldId(aid);
	unsigned flg = (awr=="rw") ? TVal::DirWrite|TVal::DirRead :
				     ((awr=="w") ? TVal::DirWrite : TFld::NoWrite|TVal::DirRead);
	if( atp.size() >= 2 && atp[1] == 'I' )	flg = (flg & (~TVal::DirWrite)) | TFld::NoWrite;
	p_el.fldAt(el_id).setFlg( flg );
	p_el.fldAt(el_id).setDescr( anm );
	p_el.fldAt(el_id).setReserve( atp+":"+TSYS::int2str(ai) );
	if( flg&TVal::DirRead ) owner().regVal(ai,atp);

	als.push_back(aid);
    }

    //- Check for delete DAQ parameter's attributes -
    for( int i_p = 0; i_p < p_el.fldSize(); i_p++ )
    {
	int i_l;
	for( i_l = 0; i_l < als.size(); i_l++ )
	    if( p_el.fldAt(i_p).name() == als[i_l] )
		break;
	if( i_l >= als.size() )
	    try{ p_el.fldDel(i_p); i_p--; }
	    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    TParamContr::disable();

    //- Set EVAL to parameter attributes -
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS( EVAL_STR, 0, true );
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

    int off = 0;
    string tp = TSYS::strSepParse(val.fld().reserve(),0,':',&off);
    int aid = atoi(TSYS::strSepParse(val.fld().reserve(),0,':',&off).c_str());
    if( !tp.empty() )
	switch(val.fld().type())
	{
	    case TFld::Boolean:	val.setB(owner().getValC(aid,acq_err,tp=="CI"),0,true);	break;
	    case TFld::Integer:	val.setI(owner().getValR(aid,acq_err,tp=="RI"),0,true);	break;
	}
    else if( val.name() == "err" )
    {
	if( acq_err.empty() )	val.setS("0",0,true);
	else			val.setS(acq_err,0,true);
    }
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
    if( !enableStat() )	valo.setS( EVAL_STR, 0, true );

    //> Send to active reserve station
    if( owner().redntUse( ) )
    {
	if( valo.getS() == pvl.getS() ) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",valo.name())->setText(valo.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //> Direct write
    int aid = atoi(TSYS::strSepParse(valo.fld().reserve(),1,':').c_str());
    switch(valo.fld().type())
    {
	case TFld::Boolean:
	    owner().setValC(valo.getB(NULL,true),aid,acq_err);
	    break;
	case TFld::Integer:
	    owner().setValR(valo.getI(NULL,true),aid,acq_err);
	    break;
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
    //- Get page info -
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ATTR_LS",cfg("ATTR_LS").fld().descr(),0664,"root","root",1,
	    "help",_("Attributes configuration list. List must be written by lines in format: [dt:numb:rw:id:name]\n"
		    "Where:\n"
		    "  dt - Modbus data type (R-register,C-coil,RI-input register,CI-input coil);\n"
		    "  numb - ModBus device's data address (dec, hex or octal);\n"
		    "  rw - read-write mode (r-read; w-write; rw-readwrite);\n"
		    "  id - created attribute identifier;\n"
		    "  name - created attribute name.\n"
		    "Example:\n"
		    "  'R:0x300:rw:var:Variable' - register access;\n"
		    "  'C:100:r:var1:Variable 1' - coin access."));
	return;
    }
    //- Process command to page -
    TParamContr::cntrCmdProc(opt);
}

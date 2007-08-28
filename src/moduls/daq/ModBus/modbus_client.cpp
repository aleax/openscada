
//OpenSCADA system module DAQ.ModBus file: modbus_client.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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
#include <signal.h>

#include <tsys.h>
#include <ttiparam.h>

#include "modbus_client.h"

//******************************************************
//* Modul info!                                        *
#define MOD_ID      "ModBus"
#define MOD_NAME    "ModBus client"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow realising of ModBus client service. On time realised only Modbus TCP/IP"
#define LICENSE     "GPL"
//******************************************************

ModBusDAQ::TTpContr *ModBusDAQ::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
    	    AtMod.id	= "";

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	ModBusDAQ::TTpContr *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = ModBusDAQ::mod = new ModBusDAQ::TTpContr( source );

	return self_addr;
    }
}

using namespace ModBusDAQ;

//******************************************************
//* TTpContr                                           *
//******************************************************
TTpContr::TTpContr( string name )  
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
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

void TTpContr::modLoad( )
{
    //- Load parameters from command line -
    int next_opt;
    char *short_opt="h";
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

    //- Controler's bd structure -
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("TRANSP",_("Connection transport"),TFld::String,TFld::NoFlag,"30","Sockets") );
    fldAdd( new TFld("ADDR",_("Remote host address"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("NODE",_("Server destination node"),TFld::Integer,TFld::NoFlag,"20","1","0;255") );
    //- Parameter type bd structure -
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("ATTR_LS",_("Attributes list (next line separated)"),TFld::String,TFld::FullText,"100","") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0), isErr(false),
	m_per(cfg("PERIOD").getRd()), m_prior(cfg("PRIOR").getId()), 
	m_tr(cfg("TRANSP").getSd()), m_addr(cfg("ADDR").getSd()), m_node(cfg("NODE").getId())
{    
    cfg("PRM_BD").setS("ModBusPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::load( )
{
    TController::load( );
}

void TMdContr::save( )
{
    TController::save();
}

void TMdContr::start_( )
{
    if( !prc_st )
    {
	//- Establish connection to server -
	if( !SYS->transport().at().at(m_tr).at().outPresent(mod->modId()+id()) )
	    SYS->transport().at().at(m_tr).at().outAdd(mod->modId()+id());
	SYS->transport().at().at(m_tr).at().outAt(mod->modId()+id()).at().setAddr(m_addr);
	SYS->transport().at().at(m_tr).at().outAt(mod->modId()+id()).at().start();
	
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

     	//- Delete output transport -
	if( !SYS->transport().at().at(m_tr).at().outPresent(mod->modId()+id()) )
	    SYS->transport().at().at(m_tr).at().outDel(mod->modId()+id());
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

string TMdContr::modBusReq( string &pdu )
{    
    ResAlloc res(req_res,true);

    string mbap;
    unsigned char buf[1000];
    AutoHD<TTransportOut> tr = SYS->transport().at().at(m_tr).at().outAt(mod->modId()+id());
    
    //-- Encode MBAP (Modbus Application Protocol)
    mbap  = (char)0x15;		//Transaction ID MSB
    mbap += (char)0x01;		//Transaction ID LSB
    mbap += (char)0x00;		//Protocol ID MSB
    mbap += (char)0x00;		//Protocol ID LSB
    mbap += (char)(pdu.size()>>8);	//PDU size MSB
    mbap += (char)pdu.size();	//PDU size LSB
    mbap += (char)0xFF;		//Unit identifier
    try
    {
    	//-- Send request --
    	int resp_len = tr.at().messIO((mbap+pdu).c_str(),mbap.size()+pdu.size(),(char*)buf,sizeof(buf),20);
    	if( resp_len < mbap.size() ) return _("13:Error server respond");
    	int resp_sz = (buf[4]<<8)+buf[5];
    	pdu.assign((char*)buf+mbap.size(),resp_len-mbap.size());
    	//-- Wait tail --
    	while( pdu.size() < resp_sz-1 )
    	{
    	    resp_len = tr.at().messIO(NULL,0,(char*)buf,sizeof(buf),20);
    	    pdu.append((char*)buf,resp_len);
	}
	if( pdu[0]&0x80 )
	    switch(pdu[1])
	    {
		case 0x1: 
		    snprintf((char*)buf,sizeof(buf),_("1:Function %xh no support."),pdu[0]&(~0x80));
		    return (char*)buf;
		case 0x2: return _("2:Requested registers length too long or.");
		case 0x3: return _("3:Illegal data value.");
		case 0x4: return _("4:Server failure.");
		case 0x5: return _("5:Request require too long time for execute.");
		case 0x6: return _("6:Server busy.");
		case 0xA: case 0xB: return _("10:Gateway problem.");
		default:  
		    snprintf((char*)buf,sizeof(buf),_("12:Unknown error: %xh."),pdu[1]);
		    return (char*)buf;
	    }	    
    }catch(...){ return _("14:Connection error"); }

    return "";
}


void *TMdContr::Task( void *icntr )
{
    string pdu, rez;
    long long work_tm, last_tm = 0;
    vector<string> als;
    struct timespec get_tm;
    TMdContr &cntr = *(TMdContr *)icntr;
    AutoHD<TVal> val;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {    
    	while(!cntr.endrun_req)
	{	
	    long long t_cnt = SYS->shrtCnt();	
	    cntr.en_res.resRequestR( );

	    //- Update controller's data -
	    for( int i_p=0; i_p < cntr.p_hd.size(); i_p++)
	    {               
		cntr.p_hd[i_p].at().elem().fldList(als);
		for( int i_a = 0; i_a < als.size(); i_a++ )
		{
    		    val = cntr.p_hd[i_p].at().vlAt(als[i_a]);
    		    //- Encode request PDU (Protocol Data Units) --
    		    int reg = val.at().fld().reserve();
    		    pdu = (char)0x3;	//Function, read multiple registers
    		    pdu += (char)(reg>>8);	//Address MSB
    		    pdu += (char)reg;	//Address LSB
    		    pdu += (char)0;		//Number of registers MSB
    		    pdu += (char)1;		//Number of registers LSB
		    //- Request to remote server -
	    	    rez = cntr.modBusReq( pdu );
	    	    if( !rez.empty() )
		    { 
			if( !cntr.isErr ) cntr.p_hd[i_p].at().vlAt("err").at().setS(rez); 
			cntr.isErr = true;
			break; 
		    }
		    else if( cntr.isErr ) 
		    { cntr.p_hd[i_p].at().vlAt("err").at().setS("0"); cntr.isErr = false; }
	    	    val.at().setI((pdu[2]<<8)+pdu[3],0,true);
		}
    	    }
	    val.free();

    	    //- Calc acquisition process time -
    	    cntr.en_res.resReleaseR( );    
    	    cntr.tm_gath = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    
    	    //- Calc next work time and sleep -
    	    clock_gettime(CLOCK_REALTIME,&get_tm);
	    work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/(long long)(cntr.m_per*1000000000) + 1)*(long long)(cntr.m_per*1000000000);
	    if(last_tm == work_tm)	work_tm+=(long long)(cntr.m_per*1000000000);	//Fix early call
    	    last_tm = work_tm;
	    get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
    	    clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);
    	}
    }catch(TError err)	{ mess_err(err.cat.c_str(),err.mess.c_str()); }
    
    cntr.prc_st = false;
    
    return NULL;
}

/*char TMdContr::crc_table[] = 
    { 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,	//00
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,	//01
      0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,	//02
      0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,	//03
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,	//04
      0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,	//05
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,	//06
      0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,	//07
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,	//08
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,	//09
      0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,	//10
      0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,	//11
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,	//12
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,	//13
      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,	//14
      0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,	//15
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,	//16
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,	//17
      0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,	//18
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,	//19
      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,	//20
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,	//21
      0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,	//22
      0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,	//23
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,	//24
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC0, 0xC1, 0x01,	//25
      0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5,	//26
      0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,	//27
      0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18,	//28
      0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F,	//29
      0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17,	//30
      0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,	//31
      0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6,	//32
      0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,	//33
      0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9,	//34
      0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,	//35
      0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24,	//36
      0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23,	//37
      0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3,	//38
      0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,	//39
      0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A,	//40
      0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79,	//41
      0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD,	//42
      0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,	//43
      0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90,	//44
      0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,	//45
      0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F,	//46
      0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,	//47
      0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E,	//48
      0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45,	//49
      0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81,	//50
      0x80, 0x40  };

unsigned int TMdContr::crc16( const string &mframe )
{
    int index;
    unsigned char crc_Low = 0xFF;
    unsigned char crc_High = 0xFF;
    
    for( int i_b = 0; i_b < mframe.size(); i_b++ )
    {
	index    = crc_High ^ (unsigned char)mframe[i_b];
	crc_High = crc_Low ^ crc_table[index];
	crc_Low  = crc_table[index+256];
    }
    return crc_High*256+crc_Low;
}*/

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/gath_tm",_("Gather data time (ms)"),0444,"root","root",1,"tp","real");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/gath_tm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_gath,6));
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), p_el("w_attr"), m_attrLs(cfg("ATTR_LS").getSd())
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();    
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

void TMdPrm::enable()
{
    if( enableStat() )	return;    

    TParamContr::enable();

    //- Parse ModBus attributes and convert to strong list -
    vector<string> als;
    int  ai;
    bool awr;
    string sel, aid, anm;
    for( int ioff = 0; (sel=TSYS::strSepParse(m_attrLs,0,'\n',&ioff)).size(); )
    {
	ai  = atoi(TSYS::strSepParse(sel,0,':').c_str());
	awr = atoi(TSYS::strSepParse(sel,1,':').c_str());
	aid = TSYS::strSepParse(sel,2,':');
	if( aid.empty() ) aid = TSYS::int2str(ai);
	anm = TSYS::strSepParse(sel,3,':');
	if( anm.empty() ) anm = TSYS::int2str(ai); 
        als.push_back(aid+":"+anm+":"+TSYS::int2str(ai)+":"+TSYS::int2str(awr));
    }

    //- Make DAQ parameter's attributes -
    for( int i_l = 0; i_l < als.size(); i_l++ )
    {
	aid = TSYS::strSepParse(als[i_l],0,':');
	if( vlPresent(aid) && !p_el.fldPresent(aid) )	continue;
	if( !p_el.fldPresent(aid) )
	    p_el.fldAdd( new TFld( aid.c_str(),"",TFld::Integer,TFld::NoFlag,"",TSYS::int2str(EVAL_INT).c_str(),"0:65535") );
	int el_id = p_el.fldId(aid);
	p_el.fldAt(el_id).setDescr( TSYS::strSepParse(als[i_l],1,':') );
	p_el.fldAt(el_id).setReserve( atoi(TSYS::strSepParse(als[i_l],2,':').c_str()) );
	p_el.fldAt(el_id).setFlg( atoi(TSYS::strSepParse(als[i_l],3,':').c_str()) ? (int)TVal::DirWrite : (int)TFld::NoWrite );
    }
    //- Check for delete DAQ parameter's attributes -
    for( int i_p = 0; i_p < p_el.fldSize(); i_p++ )
    {
	int i_l;
	for( i_l = 0; i_l < als.size(); i_l++ )
	    if( p_el.fldAt(i_p).name() == TSYS::strSepParse(als[i_l],0,':') )
		break;
	if( i_l >= als.size() )
	    try{ p_el.fldDel(i_p); }
	    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }	

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

void TMdPrm::vlSet( TVal &valo )
{
    if( !enableStat() )	valo.setI(EVAL_INT,0,true);
    //- Encode request PDU (Protocol Data Units) --
    string pdu;
    int reg = valo.fld().reserve();
    int val = valo.getI(NULL,true);
    pdu = (char)0x6;		//Function, read multiple registers
    pdu += (char)(reg>>8);	//Address MSB
    pdu += (char)reg;		//Address LSB
    pdu += (char)(val>>8);	//Data MSB
    pdu += (char)val;		//Data LSB
    //- Request to remote server -
    string rez = owner().modBusReq( pdu );
    if( !rez.empty() )
    {
	if( !owner().isErr ) vlAt("err").at().setS(rez); 
	owner().isErr = true;
    }
    else if( owner().isErr ) { vlAt("err").at().setS("0"); owner().isErr = false; }
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

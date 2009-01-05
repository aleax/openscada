
//OpenSCADA system module DAQ.ModBus file: modbus_client.cpp
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <tsys.h>
#include <ttiparam.h>

#include "modbus_client.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"ModBus"
#define MOD_NAME	"ModBus client"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.9.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow realisation of ModBus client service. Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols."
#define LICENSE		"GPL"
//*************************************************

ModBusDAQ::TTpContr *ModBusDAQ::mod;  //Pointer for direct access to module

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
	    return new ModBusDAQ::TTpContr( source );
	return NULL;
    }
}

using namespace ModBusDAQ;

//******************************************************
//* TTpContr                                           *
//******************************************************
TTpContr::TTpContr( string name ) : mSerConnResume(30), mPrtLen(0)
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

    m_sdev	= grpAdd("sdev_");
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

string TTpContr::serDevDB( )
{
    return SYS->workDB()+"."+modId()+"_sdevs";
}

void TTpContr::serDevAdd( const string &dev )
{
    if( chldPresent(m_sdev,dev) ) return;
    chldAdd( m_sdev, new SSerial(dev,this) );
}

void TTpContr::postEnable( int flag )
{
    TModule::postEnable( flag );

    //- Controler's bd structure -
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0.01;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("PROT",_("Modbus protocol"),TFld::Integer,TFld::Selected,"1","0","0;1;2",_("TCP/IP;RTU;ASCII")) );
    fldAdd( new TFld("ADDR",_("Host address"),TFld::String,TFld::NoFlag,"30","devhost.org:502") );
    fldAdd( new TFld("NODE",_("Destination node"),TFld::Integer,TFld::NoFlag,"20","1","0;255") );
    fldAdd( new TFld("FRAG_MERGE",_("Data fragments merge"),TFld::Boolean,TFld::NoFlag,"1","0") );
    fldAdd( new TFld("TM_FRM",_("Time frame"),TFld::Integer,TFld::NoFlag,"5","0") );
    fldAdd( new TFld("TM_CHAR",_("Time char"),TFld::Real,TFld::NoFlag,"5.2","0") );
    fldAdd( new TFld("TM_REQ",_("Time request"),TFld::Integer,TFld::NoFlag,"5","0") );

    //- Parameter type bd structure -
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("ATTR_LS",_("Attributes list"),TFld::String,TFld::FullText|TCfg::NoVal,"100","") );

    //- Serial devices DB structure -
    el_ser_dev.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    el_ser_dev.fldAdd( new TFld("SPEED",_("Speed"),TFld::Integer,TFld::NoFlag,"6","9600") );
    el_ser_dev.fldAdd( new TFld("LEN",_("Length"),TFld::Integer,TFld::NoFlag,"1","8") );
    el_ser_dev.fldAdd( new TFld("TWOSTOP",_("Two stop bit"),TFld::Boolean,TFld::NoFlag,"1","1") );
    el_ser_dev.fldAdd( new TFld("PARITY",_("Parity"),TFld::Integer,TFld::NoFlag,"1","0") );
    el_ser_dev.fldAdd( new TFld("TM_FRM",_("Time frame"),TFld::Integer,TFld::NoFlag,"5","1000") );
    el_ser_dev.fldAdd( new TFld("TM_CHAR",_("Time char"),TFld::Real,TFld::NoFlag,"5.2","4") );
    el_ser_dev.fldAdd( new TFld("TM_REQ",_("Time request"),TFld::Integer,TFld::NoFlag,"5","2000") );
}

void TTpContr::load_( )
{
    //- Load parameters from command line -
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while( next_opt != -1 );

    //- Load parameters from config file -
    setSerConnResume( atoi(TBDS::genDBGet(nodePath()+"SerConnResume",TSYS::int2str(serConnResume())).c_str()) );

    //- Load Serial devices configuration -
    try
    {
	TConfig c_el(&serDevE());
	for( int fld_cnt = 0; SYS->db().at().dataSeek(serDevDB(),nodePath()+"sDevs/",fld_cnt,c_el); fld_cnt++ )
	{
	    string id = c_el.cfg("ID").getS();
	    c_el.cfg("ID").setS("");
	    try
	    {
		if( !serDevPresent(id) )	serDevAdd(id);
		serDevAt(id).at().load();
		if( !serDevAt(id).at().hasOpen() )	serDevAt(id).at().setOpen(true);
	    }catch( TError err ) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and load serial devices' DB is error."));
    }
}

void TTpContr::save_()
{
    //- Save parameters to config file -
    TBDS::genDBSet(nodePath()+"SerConnResume",TSYS::int2str(serConnResume()).c_str());

    //- Save Serial devices configuration -
    TConfig cfg(&serDevE());
    vector<string> lst;
    serDevList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	serDevAt(lst[i_l]).at().save();
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

ui8 TTpContr::CRCHi[] =
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

ui8 TTpContr::CRCLo[] =
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

ui16 TTpContr::CRC16( const string &mbap )
{
    ui8	hi = 0xFF;
    ui8	lo = 0xFF;
    ui16 index;
    for( int i_b = 0; i_b < mbap.size(); i_b++ )
    {
	index = lo^(ui8)mbap[i_b];
	lo = hi^CRCHi[index];
	hi = CRCLo[index];
    }
    return hi|(lo<<8);
}

ui8 TTpContr::LRC( const string &mbap )
{
    ui8 ch = 0;
    for( int i_b = 0; i_b < mbap.size(); i_b++ )
	ch += (ui8)mbap[i_b];

    return ch;
}

string TTpContr::DataToASCII( const string &in )
{
    ui8 ch;
    string rez;

    for( int i = 0; i < in.size(); i++ )
    {
	ch = (in[i]&0xF0)>>4;
	rez += (ch + ((ch <= 9)?'0':('A' - 10)));
	ch = in[i]&0x0F;
	rez += (ch + ((ch <= 9) ? '0' : ('A' - 10)));
    }

    return rez;
}

string TTpContr::ASCIIToData( const string &in )
{
    ui8 ch1, ch2;
    string rez;

    for( int i=0; i < (in.size()&(~0x01)); i+=2 )
    {
	ch2 = 0;
	ch1 = in[i];
	if( ch1 >= '0' && ch1 <= '9' )		ch1 -= '0';
	else if( ch1 >= 'A' && ch1 <= 'F' )	ch1 -= ('A' + 10);
	else 					ch1 = 0;
	ch2 = ch1 << 4;
	ch1 = in[i+1];
	if( ch1 >= '0' && ch1 <= '9' )		ch1 -= '0';
	else if ( ch1 >= 'A' && ch1 <= 'F' )	ch1 -= ('A' + 10);
	else					ch1 = 0;
	rez += ch2|ch1;
    }

    return rez;
}

void TTpContr::setPrtLen( int vl )
{
    ResAlloc res(mPrtRes,true);

    while( mPrt.size() > vl )	mPrt.pop_back();

    mPrtLen = vl;
}

void TTpContr::pushPrtMess( const string &vl )
{
    ResAlloc res(mPrtRes,true);

    if( !prtLen() )	return;

    mPrt.push_front(vl);

    while( mPrt.size() > prtLen() )	mPrt.pop_back();
}

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TTipDAQ::cntrCmdProc( opt );
	if( ctrMkNode("area",opt,0,"/mod",_("ModBus")) )
	{
	    ctrMkNode("fld",opt,-1,"/mod/serResConn",_("Serial devices resume timeout"),0664,"root","DAQ",2,"tp","dec",
		"help",_("Resume timeout for connection to dead serial devices in seconds."));
	    if( ctrMkNode("table",opt,-1,"/mod/dev",_("Serial devices"),0664,"root","root",2,"s_com","add,del","key","dev") )
	    {
		ctrMkNode("list",opt,-1,"/mod/dev/dev",_("Device"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/speed",_("Speed"),0664,"root","root",5,"tp","dec","idm","1","dest","select",
		    "sel_id","300;600;1200;2400;4800;9600;19200;38400;57600;115200;230400;460800",
		    "sel_list","300;600;1200;2400;4800;9600;19.2k;38.4k;57.6k;115.2k;230.4k;460.8k");
		ctrMkNode("list",opt,-1,"/mod/dev/len",_("Char length (bit)"),0664,"root","root",3,"tp","dec","min","5","max","8");
		ctrMkNode("list",opt,-1,"/mod/dev/stop",_("Stop bits"),0664,"root","root",3,"tp","dec","min","1","max","2");
		ctrMkNode("list",opt,-1,"/mod/dev/parity",_("Parity"),0664,"root","root",5,"tp","dec","idm","1","dest","select",
		    "sel_id","0;1;2","sel_list",_("No check;Parity;Odd parity"));
		ctrMkNode("list",opt,-1,"/mod/dev/frTm",_("Time frame"),0664,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/charTm",_("Time char"),0664,"root","root",1,"tp","real");
		ctrMkNode("list",opt,-1,"/mod/dev/reqTm",_("Time request"),0664,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/open",_("Opened"),0664,"root","root",1,"tp","bool");
	    }
	    ctrMkNode("fld",opt,-1,"/mod/protLen",_("Protocol length"),0664,"root","DAQ",4,"tp","dec","min","0","max","10000",
		"help",_("Zero use for protocol disabling"));
	    if( prtLen() )
		ctrMkNode("fld",opt,-1,"/mod/prot",_("Protocol"),0444,"root","DAQ",3,"tp","str","cols","90","rows","20");
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/mod/serResConn" )
    {
	if( ctrChkNode(opt,"get",0664,"root","DAQ",SEQ_RD) )	opt->setText( TSYS::int2str(serConnResume()) );
	if( ctrChkNode(opt,"set",0664,"root","DAQ",SEQ_WR) )	setSerConnResume( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/mod/dev" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    //- Fill Archives table -
	    XMLNode *n_dev	= ctrMkNode("list",opt,-1,"/mod/dev/dev","");
	    XMLNode *n_speed	= ctrMkNode("list",opt,-1,"/mod/dev/speed","");
	    XMLNode *n_len	= ctrMkNode("list",opt,-1,"/mod/dev/len","");
	    XMLNode *n_stop	= ctrMkNode("list",opt,-1,"/mod/dev/stop","");
	    XMLNode *n_parity	= ctrMkNode("list",opt,-1,"/mod/dev/parity","");
	    XMLNode *n_frTm	= ctrMkNode("list",opt,-1,"/mod/dev/frTm","");
	    XMLNode *n_charTm	= ctrMkNode("list",opt,-1,"/mod/dev/charTm","");
	    XMLNode *n_reqTm	= ctrMkNode("list",opt,-1,"/mod/dev/reqTm","");
	    XMLNode *n_open	= ctrMkNode("list",opt,-1,"/mod/dev/open","");
	    vector<string> lst;
	    serDevList(lst);
	    for( int i_s = 0; i_s < lst.size(); i_s++ )
	    {
		AutoHD<SSerial>	ser = serDevAt(lst[i_s]);
		if( n_dev )	n_dev->childAdd("el")->setText(lst[i_s]);
		if( n_speed )	n_speed->childAdd("el")->setText(TSYS::int2str(ser.at().speed()));
		if( n_len )	n_len->childAdd("el")->setText(TSYS::int2str(ser.at().len()));
		if( n_stop )	n_stop->childAdd("el")->setText(TSYS::int2str(ser.at().twostop()?2:1));
		if( n_parity )	n_parity->childAdd("el")->setText(TSYS::int2str(ser.at().parity()));
		if( n_frTm )	n_frTm->childAdd("el")->setText(TSYS::int2str(ser.at().timeoutFrame()));
		if( n_charTm )	n_charTm->childAdd("el")->setText(TSYS::real2str(ser.at().timeoutChar()));
		if( n_reqTm )	n_reqTm->childAdd("el")->setText(TSYS::int2str(ser.at().timeoutReq()));
		if( n_open )	n_open->childAdd("el")->setText(TSYS::int2str(ser.at().hasOpen()));
	    }
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
	{
	    string devNm = "/dev/NewSDev";
	    if( serDevPresent(devNm) )
		for( int i_d = 0; true; i_d++ )
		    if( !serDevPresent(devNm+TSYS::int2str(i_d)) )
		    { devNm = devNm+TSYS::int2str(i_d); break; }
	    serDevAdd(devNm);
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )
	    serDevDel(opt->attr("key_dev"),true);
	if( ctrChkNode(opt,"set",0666,"root","root",SEQ_WR) )
	{
	    string col	= opt->attr("col");
	    string dev	= opt->attr("key_dev");
	    if( col == "dev" )
	    {
		serDevAdd(opt->text());
		(TConfig&)serDevAt(opt->text()).at() = (TConfig&)serDevAt(dev).at();
		serDevAt(opt->text()).at().cfg("ID").setS(opt->text());
		serDevDel(dev,true);
	    }
	    else if( col == "speed" )	serDevAt(dev).at().setSpeed(atoi(opt->text().c_str()),true);
	    else if( col == "len" )	serDevAt(dev).at().setLen(atoi(opt->text().c_str()));
	    else if( col == "stop" )	serDevAt(dev).at().setTwostop(atoi(opt->text().c_str())==2);
	    else if( col == "parity" )	serDevAt(dev).at().setParity(atoi(opt->text().c_str()));
	    else if( col == "frTm" )	serDevAt(dev).at().setTimeoutFrame(atoi(opt->text().c_str()));
	    else if( col == "charTm" )	serDevAt(dev).at().setTimeoutChar(atof(opt->text().c_str()));
	    else if( col == "reqTm" )	serDevAt(dev).at().setTimeoutReq(atoi(opt->text().c_str()));
	    else if( col == "open" )	serDevAt(dev).at().setOpen(atoi(opt->text().c_str()));
	}
    }
    else if( a_path == "/mod/protLen" )
    {
	if( ctrChkNode(opt,"get",0664,"root","DAQ",SEQ_RD) )	opt->setText( TSYS::int2str(prtLen()) );
	if( ctrChkNode(opt,"set",0664,"root","DAQ",SEQ_WR) )	setPrtLen( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/mod/prot" && ctrChkNode(opt) )
    {
	ResAlloc res(mPrtRes,true);
	for( int i_p = 0; i_p < mPrt.size(); i_p++ )
	    opt->setText(opt->text()+mPrt[i_p]+"\n");
    }
    else TTipDAQ::cntrCmdProc( opt );
}

//*************************************************
//* SSerial                                       *
//*************************************************
SSerial::SSerial( const string &iDev, TTpContr *iown ) :
    TCntrNode(iown), TConfig( &iown->serDevE() ), fd(-1),
    m_id(cfg("ID").getSd()), m_speed(cfg("SPEED").getId()), m_len(cfg("LEN").getId()),
    m_twostop(cfg("TWOSTOP").getBd()), m_parity(cfg("PARITY").getId()),
    frTm(cfg("TM_FRM").getId()), charTm(cfg("TM_CHAR").getRd()), reqTm(cfg("TM_REQ").getId())
{
    m_id = iDev;
}

void SSerial::postDisable( int flag )
{
    if( hasOpen() )	setOpen(false);
    //- Delete serial device from DB -
    try
    {
	if( flag )
	    SYS->db().at().dataDel(owner().serDevDB(),mod->nodePath()+"sDevs/",*this);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void SSerial::load_( )
{
    SYS->db().at().dataGet(owner().serDevDB(),mod->nodePath()+"sDevs/",*this);
}

void SSerial::save_( )
{
    SYS->db().at().dataSet(owner().serDevDB(),mod->nodePath()+"sDevs/",*this);
}

void SSerial::setOpen( bool vl )
{
    ResAlloc res(m_res,true);
    if( vl == hasOpen() )	return;

    if( vl )
    {
	//- Serial port open -
	fd = open( id().c_str(), O_RDWR|O_NOCTTY );
	if( fd < 0 )	throw TError(mod->nodePath().c_str(),_("Serial port '%s' open error."),id().c_str());
	//-- Set serial port parameters --
	struct termios tio;
	bzero( &tio, sizeof(tio) );
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = B9600|CS8|CREAD|CLOCAL;
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 1;            ///< blocking read until 1 character arrives
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
	res.release();
	//-- Set byte length --
	setLen(len());
	//-- Set stop bits number --
	setTwostop(twostop());
	//-- Set parity --
	setParity(parity());
	//-- Set speed --
	setSpeed(speed());
    }
    else
    {
	close(fd);
	fd = -1;
    }
}

void SSerial::setSpeed( int val, bool tmAdj )
{
    m_speed = val;

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	speed_t tspd = B9600;
	switch( val )
	{
	    case 300:	tspd = B300;	break;
	    case 600:	tspd = B600;	break;
	    case 1200:	tspd = B1200;	break;
	    case 2400:	tspd = B2400;	break;
	    case 4800:	tspd = B4800;	break;
	    case 9600:	tspd = B9600;	break;
	    case 19200:	tspd = B19200;	break;
	    case 38400:	tspd = B38400;	break;
	    case 57600:	tspd = B57600;	break;
	    case 115200:tspd = B115200;	break;
	    case 230400:tspd = B230400;	break;
	    case 460800:tspd = B460800;	break;
	}
	cfsetispeed( &tio, tspd );
	cfsetospeed( &tio, tspd );
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    //- Times adjust -
    if( tmAdj )
    {
	setTimeoutChar(TSYS::realRound((12.*1000.*3.)/(double)val,2));
	setTimeoutFrame((12*1000*512)/val);
	setTimeoutReq(timeoutFrame()*2);
    }
    modif();
}

void SSerial::setLen( int val )
{
    m_len = vmin(8,vmax(5,val));

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	tio.c_cflag &= ~CSIZE;
	switch(m_len)
	{
	    case 5:	tio.c_cflag |= CS5;	break;
	    case 6:	tio.c_cflag |= CS6;	break;
	    case 7:	tio.c_cflag |= CS7;	break;
	    case 8:	tio.c_cflag |= CS8;	break;
	}
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    modif();
}

void SSerial::setTwostop( bool val )
{
    m_twostop = val;

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	if( val )	tio.c_cflag &= ~CSTOPB;
	else		tio.c_cflag |= CSTOPB;
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    modif();
}

void SSerial::setParity( int val )
{
    m_parity = val;

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	switch(val)
	{
	    case 0:
		tio.c_cflag &= ~PARENB;
		break;
	    case 1:
		tio.c_cflag |= PARENB;
		tio.c_cflag &= ~PARODD;
		break;
	    case 2:
		tio.c_cflag |= PARENB;
		tio.c_cflag |= PARODD;
		break;
	}
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    modif();
}

string SSerial::req( const string &vl, int iFrTm, double iCharTm, int iReqTm )
{
    int wFrTm = 1000*vmin( (iFrTm>0) ? iFrTm : timeoutFrame(), 10000 );
    double wCharTm = vmax( (iCharTm>0) ? iCharTm : timeoutChar(), 0.001 );
    int wReqTm = 1000*vmin( (iReqTm>0) ? iReqTm : timeoutReq(), 10000 );

    ResAlloc res( m_res, true );

    if( !hasOpen() )	throw TError(mod->nodePath().c_str(),_("Serial port '%s' no opened."),id().c_str());

    usleep((int)(2500.0*wCharTm));

    //- Write request -
    if( write( fd, vl.data(), vl.size() ) == -1 )
	throw TError(mod->nodePath().c_str(),_("Write to serial port '%s' is error."),id().c_str());

    //- Read reply -
    char buf[1000];
    int  blen = 0;
    long long tmptm;
    fd_set fdset;

    tmptm = TSYS::curTime();
    while( true )
    {
	int bytes = 0;
	ioctl( fd, FIONREAD, &bytes );
	if( bytes > 2 ) break;
	//--- Serial timeout ---
	if( (TSYS::curTime() - tmptm) >= wReqTm )
	    throw TError(mod->nodePath().c_str(),_("Respond from serial port '%s' is timeouted."),id().c_str());
	usleep( 1000 );
    }
    fcntl( fd, F_SETFL, 0 );
    blen = read( fd, buf, sizeof(buf) );

    tmptm = TSYS::curTime();
    FD_ZERO( &fdset );
    FD_SET( fd, &fdset );
    struct timeval tv;
    while( true )
    {
	//--- Char timeout ---
	tv.tv_sec = 0; tv.tv_usec = (int)(1000.0*wCharTm);
	if( select(fd+1,&fdset,NULL,NULL,&tv) <= 0 )	break;
	blen += read( fd, buf+blen, sizeof(buf)-blen );
	//--- Frame timeout ---
	if( (TSYS::curTime()-tmptm) > wFrTm )	break;
    }

    usleep((int)(2500.0*wCharTm));

    return string( buf, blen );
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
	TController( name_c, daq_db, cfgelem ), prc_st(false), endrun_req(false), tm_gath(0), tm_delay(0),
	m_per(cfg("PERIOD").getRd()), m_prior(cfg("PRIOR").getId()), m_prt(cfg("PROT").getId()),
	m_addr(cfg("ADDR").getSd()), m_node(cfg("NODE").getId()), m_merge(cfg("FRAG_MERGE").getBd()),
	frTm(cfg("TM_FRM").getId()), charTm(cfg("TM_CHAR").getRd()), reqTm(cfg("TM_REQ").getId())
{
    cfg("PRM_BD").setS("ModBusPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm( name, &owner().tpPrmAt(type) );
}

void TMdContr::disable_( )
{
    //- Clear acquisition data block -
    acqBlks.clear();
    acqBlksIn.clear();
    acqBlksCoil.clear();
    acqBlksCoilIn.clear();
}

void TMdContr::start_( )
{
    if( !prc_st )
    {
	//- Establish connection -
	switch( m_prt )
	{
	    case 0:
	    {
		if( !SYS->transport().at().at("Sockets").at().outPresent(mod->modId()+id()) )
		    SYS->transport().at().at("Sockets").at().outAdd(mod->modId()+id());
		string trAddr = "TCP:"+TSYS::strSepParse(m_addr,0,':')+":"+
		    (TSYS::strSepParse(m_addr,1,':').empty() ? "502" : TSYS::strSepParse(m_addr,1,':'));
		SYS->transport().at().at("Sockets").at().outAt(mod->modId()+id()).at().setAddr(trAddr);
		SYS->transport().at().at("Sockets").at().outAt(mod->modId()+id()).at().start();
		break;
	    }
	    case 1: case 2:
	    {
		mod->serDevAt(m_addr);
		break;
	    }
	}

	//- Start the gathering data task -
	pthread_attr_t pthr_attr;
	pthread_attr_init( &pthr_attr );
	struct sched_param prior;
	if( m_prior && SYS->user() == "root" )
	    pthread_attr_setschedpolicy( &pthr_attr, SCHED_RR );
	else pthread_attr_setschedpolicy( &pthr_attr, SCHED_OTHER );
	prior.__sched_priority = m_prior;
	pthread_attr_setschedparam( &pthr_attr, &prior );

	pthread_create( &procPthr, &pthr_attr, TMdContr::Task, this );
	pthread_attr_destroy( &pthr_attr );
	if( TSYS::eventWait( prc_st, true, nodePath()+"start", 5 ) )
	    throw TError( nodePath().c_str(), _("Gathering task no started!") );
    }
}

void TMdContr::stop_( )
{
    if( prc_st )
    {
	//- Stop the request and calc data task -
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait( prc_st, false, nodePath()+"stop", 5 ) )
	    throw TError( nodePath().c_str(), _("Gathering task no stoped!") );
	pthread_join( procPthr, NULL );

	//- Dissconnection -
	if( m_prt == 0 && SYS->transport().at().at("Sockets").at().outPresent(mod->modId()+id()) )
	    SYS->transport().at().at("Sockets").at().outDel(mod->modId()+id());
    }
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if( icfg.fld().name() == "PROT" )
    {
	if( icfg.getI() == 0 )
	{
	    cfg("ADDR").fld().setDescr(_("Host address"));
	    cfg("TM_FRM").setView(false);
	    cfg("TM_CHAR").setView(false);
	    cfg("TM_REQ").setView(false);
	    //cfg("ADDR").setS("devhost.org:502");
	}
	else
	{
	    cfg("ADDR").fld().setDescr(_("Serial port"));
	    cfg("TM_FRM").setView(true);
	    cfg("TM_CHAR").setView(true);
	    cfg("TM_REQ").setView(true);
	    //cfg("ADDR").setS("/dev/ttyS0");
	}
	if(startStat())	stop();
    }

    return true;
}

void TMdContr::regVal( int reg, const string &dt )
{
    if( reg < 0 )	return;

    ResAlloc res( en_res, true );

    //- Register to acquisition block -
    //-- Registers and input registers --
    if( dt == "R" || dt == "RI" )
    {
	vector< SDataRec > &workCnt = (dt == "RI") ? acqBlksIn : acqBlks;
	int i_b;
	for( i_b = 0; i_b < workCnt.size(); i_b++ )
	{
	    if( (reg*2) < workCnt[i_b].off )
	    {
		if( (m_merge || (reg*2+2) >= workCnt[i_b].off) && (workCnt[i_b].val.size()+workCnt[i_b].off-(reg*2)) < MaxLenReq )
		{
		    workCnt[i_b].val.insert(0,workCnt[i_b].off-reg*2,0);
		    workCnt[i_b].off = reg*2;
		}
		else workCnt.insert(workCnt.begin()+i_b,SDataRec(reg*2,2));
	    }
	    else if( (reg*2+2) > (workCnt[i_b].off+workCnt[i_b].val.size()) )
	    {
		if( (m_merge || reg*2 <= (workCnt[i_b].off+workCnt[i_b].val.size())) && (reg*2+2-workCnt[i_b].off) < MaxLenReq )
		{
		    workCnt[i_b].val.append((reg*2+2)-(workCnt[i_b].off+workCnt[i_b].val.size()),0);
		    //- Check for allow mergin to next block -
		    if( !m_merge && i_b+1 < workCnt.size() && (workCnt[i_b].off+workCnt[i_b].val.size()) >= workCnt[i_b+1].off )
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
    //-- Coils --
    else if( dt == "C" || dt == "CI" )
    {
	vector< SDataRec > &workCnt = (dt == "CI") ? acqBlksCoilIn : acqBlksCoil;
	int i_b;
	for( i_b = 0; i_b < workCnt.size(); i_b++ )
	{
	    if( reg < workCnt[i_b].off )
	    {
		if( (m_merge || (reg+1) >= workCnt[i_b].off) && (workCnt[i_b].val.size()+workCnt[i_b].off-reg) < MaxLenReq*8 )
		{
		    workCnt[i_b].val.insert(0,workCnt[i_b].off-reg,0);
		    workCnt[i_b].off = reg;
		}
		else workCnt.insert(workCnt.begin()+i_b,SDataRec(reg,1));
	    }
	    else if( (reg+1) > (workCnt[i_b].off+workCnt[i_b].val.size()) )
	    {
		if( (m_merge || reg <= (workCnt[i_b].off+workCnt[i_b].val.size())) && (reg+1-workCnt[i_b].off) < MaxLenReq*8 )
		{
		    workCnt[i_b].val.append((reg+1)-(workCnt[i_b].off+workCnt[i_b].val.size()),0);
		    //- Check for allow mergin to next block -
		    if( !m_merge && i_b+1 < workCnt.size() && (workCnt[i_b].off+workCnt[i_b].val.size()) >= workCnt[i_b+1].off )
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
    //- Encode request PDU (Protocol Data Units) --
    string pdu;
    pdu = (char)0x6;		//Function, preset single register
    pdu += (char)(addr>>8);	//Address MSB
    pdu += (char)addr;		//Address LSB
    pdu += (char)(val>>8);	//Data MSB
    pdu += (char)val;		//Data LSB
    //- Request to remote server -
    err = modBusReq( pdu );
    //- Set to acquisition block -
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
    //- Encode request PDU (Protocol Data Units) --
    string pdu;
    pdu = (char)0x5;		//Function, preset single coil
    pdu += (char)(addr>>8);	//Address MSB
    pdu += (char)addr;		//Address LSB
    pdu += (char)val?0xFF:0x00;	//Data MSB
    pdu += (char)0x00;		//Data LSB
    //- Request to remote server -
    err = modBusReq( pdu );
    //- Set to acquisition block -
    for( int i_b = 0; i_b < acqBlksCoil.size(); i_b++ )
	if( addr >= acqBlksCoil[i_b].off && (addr+1) <= (acqBlksCoil[i_b].off+acqBlksCoil[i_b].val.size()) )
	{
	    acqBlksCoil[i_b].val[addr-acqBlksCoil[i_b].off] = val;
	    break;
	}
}

string TMdContr::modBusReq( string &pdu )
{
    ResAlloc res( req_res, true );

    string mbap, err, rez;
    char buf[1000];

    try
    {
	switch( m_prt )
	{
	    case 0:	// Modbus/TCP protocol process
	    {
		AutoHD<TTransportOut> tr;
		try{ tr = SYS->transport().at().at("Sockets").at().outAt(mod->modId()+id()); }
		catch(...) { tr.at().stop(); throw; }

		//- Encode MBAP (Modbus Application Protocol) -
		mbap  = (char)0x15;			//Transaction ID MSB
		mbap += (char)0x01;			//Transaction ID LSB
		mbap += (char)0x00;			//Protocol ID MSB
		mbap += (char)0x00;			//Protocol ID LSB
		mbap += (char)((pdu.size()+1)>>8);	//PDU size MSB
		mbap += (char)(pdu.size()+1);		//PDU size LSB
		mbap += (char)m_node;			//Unit identifier
		mbap += pdu;
		if( !tr.at().startStat() )	tr.at().start();
		//- Send request -
		int resp_len = tr.at().messIO( mbap.data(), mbap.size(), buf, sizeof(buf), 3 );
		rez.assign(buf,resp_len);
		if( rez.size() < 7 )	{ err = _("13:Error server respond"); break; }
		int resp_sz = (unsigned short)(rez[4]<<8)|(unsigned char)rez[5];
		//- Wait tail -
		while( rez.size() < (resp_sz+6) )
		{
		    resp_len = tr.at().messIO( NULL, 0, buf, sizeof(buf), 10 );
		    rez.append( buf, resp_len );
		}
		pdu = rez.substr(7);
		
		break;
	    }
	    case 1:	// Modbus/RTU protocol process
	    {
		mbap = (ui8)m_node;		//Unit identifier
		mbap += pdu;
		ui16 crc = mod->CRC16( mbap );
		mbap += crc >> 8;
		mbap += crc;
		rez = mod->serDevAt(m_addr).at().req(mbap,frTm,charTm,reqTm);
		if( rez.size() < 2 )	{ err = _("13:Error respond: Too short."); break; }
		if( mod->CRC16(rez.substr(0,rez.size()-2)) != (ui16)((rez[rez.size()-2]<<8)+(ui8)rez[rez.size()-1]) )
		{ err = _("13:Error respond: CRC check error."); break; }
		pdu = rez.substr( 1, rez.size()-3 );
		break;
	    }
	    case 2:	// Modbus/ASCII protocol process
	    {
		mbap = (ui8)m_node;		//Unit identifier
		mbap += pdu;
		mbap += mod->LRC(mbap);
		rez = mod->serDevAt(m_addr).at().req(":"+mod->DataToASCII(mbap)+"\0x0D\0x0A",frTm,charTm,reqTm);
		if( rez.size() < 3 || rez[0] != ':' || rez[rez.size()-2] != 0x0D || rez[rez.size()-1] != 0x0A )
		{ err = _("13:Error respond: Error format."); break; }
		rez = mod->ASCIIToData(rez.substr(1,rez.size()-3));
		if( mod->LRC(rez.substr(0,rez.size()-1)) != rez[rez.size()-1] )
		{ err = _("13:Error respond: LRC check error."); break; }
		pdu = rez.substr(1,rez.size()-2);
		break;
	    }
	}
	//- Check respond pdu -
	if( err.empty() )
	{
	    if( pdu.size() < 2 ) err = _("13:Error respond");
	    if( pdu[0]&0x80 )
		switch( pdu[1] )
		{
		    case 0x1:
			snprintf( buf, sizeof(buf), _("1:Function %xh no support."), pdu[0]&(~0x80) );
			err = buf;
			break;
		    case 0x2: err = _("2:Requested registers length too long.");	break;
		    case 0x3: err = _("3:Illegal data value.");				break;
		    case 0x4: err = _("4:Server failure.");				break;
		    case 0x5: err = _("5:Request require too long time for execute.");	break;
		    case 0x6: err = _("6:Server busy.");				break;
		    case 0xA: case 0xB: err = _("10:Gateway problem.");			break;
		    default:
			snprintf( buf, sizeof(buf), _("12:Unknown error: %xh."), pdu[1] );
			err = buf;
			break;
		}
	    }
    }catch( TError er )
    {
	//mess_err( err.cat.c_str(), er.mess.c_str() );
	err = _("14:Device error: ")+er.mess;
    }

    //- Prepare log -
    if( mod->prtLen( ) )
    {
	time_t tm_t = time(NULL);
	string mess = TSYS::strSepParse(ctime(&tm_t),0,'\n');
	switch( m_prt )
	{
	    case 0:	mess += " '"+id()+"' TCP:"+m_addr;	break;
	    case 1:	mess += " '"+id()+"' RTU:"+TSYS::int2str(m_node)+"("+m_addr+")";	break;
	    case 2:	mess += " '"+id()+"' ASCII:"+TSYS::int2str(m_node)+"("+m_addr+")";	break;
	}
	mess += _("\nREQ -> ");
	for( int i_d = 0; i_d < mbap.size(); i_d++ )
	{
	    sprintf(buf,"%0.2X",(unsigned char)mbap[i_d]);
	    mess += string(buf) + " ";
	}
	if( !err.empty() )	mess += _("\nERR -> ")+err;
	else
	{
	    mess += _("\nRESP -> ");
	    for( int i_d = 0; i_d < rez.size(); i_d++ )
	    {
		sprintf(buf,"%0.2X",(unsigned char)rez[i_d]);
		mess += string(buf) + " ";
	    }
	}
	mod->pushPrtMess(mess+"\n");
    }

    return err;
}

void *TMdContr::Task( void *icntr )
{
    string pdu;
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {
	while( !cntr.endrun_req )
	{
	    long long t_cnt = SYS->shrtCnt();

	    if( cntr.tm_delay > 0 )	cntr.tm_delay-=cntr.period();
	    else
	    {
#if OSC_DEBUG >= 3
		mess_debug(cntr.nodePath().c_str(),_("Fetch coils' and registers' blocks."));
#endif
		//- Update controller's data -
		ResAlloc res( cntr.en_res, false );
		//- Get coils -
		for( int i_b = 0; i_b < cntr.acqBlksCoil.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    //- Encode request PDU (Protocol Data Units) -
		    pdu = (char)0x1;					//Function, read multiple coils
		    pdu += (char)(cntr.acqBlksCoil[i_b].off>>8);	//Address MSB
		    pdu += (char)cntr.acqBlksCoil[i_b].off;		//Address LSB
		    pdu += (char)(cntr.acqBlksCoil[i_b].val.size()>>8);	//Number of coils MSB
		    pdu += (char)cntr.acqBlksCoil[i_b].val.size();	//Number of coils LSB
		    //- Request to remote server -
		    cntr.acqBlksCoil[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlksCoil[i_b].err.empty() )
			for( int i_c = 0; i_c < cntr.acqBlksCoil[i_b].val.size(); i_c++ )
			    cntr.acqBlksCoil[i_b].val[i_c] = (bool)((pdu[2+i_c/8]>>(i_c%8))&0x01);
		    else if( atoi(cntr.acqBlksCoil[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlksCoil[i_b].err);
			break;
		    }
		}
		if( cntr.tm_delay > 0 )	continue;
		//- Get input's coils -
		for( int i_b = 0; i_b < cntr.acqBlksCoilIn.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    //- Encode request PDU (Protocol Data Units) -
		    pdu = (char)0x2;					//Function, read multiple input's coils
		    pdu += (char)(cntr.acqBlksCoilIn[i_b].off>>8);	//Address MSB
		    pdu += (char)cntr.acqBlksCoilIn[i_b].off;		//Address LSB
		    pdu += (char)(cntr.acqBlksCoilIn[i_b].val.size()>>8);	//Number of coils MSB
		    pdu += (char)cntr.acqBlksCoilIn[i_b].val.size();	//Number of coils LSB
		    //- Request to remote server -
		    cntr.acqBlksCoilIn[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlksCoilIn[i_b].err.empty() )
			for( int i_c = 0; i_c < cntr.acqBlksCoilIn[i_b].val.size(); i_c++ )
			    cntr.acqBlksCoilIn[i_b].val[i_c] = (bool)((pdu[2+i_c/8]>>(i_c%8))&0x01);
		    else if( atoi(cntr.acqBlksCoilIn[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlksCoilIn[i_b].err);
			break;
		    }
		}
		if( cntr.tm_delay > 0 )	continue;
		//- Get registers -
		for( int i_b = 0; i_b < cntr.acqBlks.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    //- Encode request PDU (Protocol Data Units) -
		    pdu = (char)0x3;				//Function, read multiple registers
		    pdu += (char)((cntr.acqBlks[i_b].off/2)>>8);	//Address MSB
		    pdu += (char)(cntr.acqBlks[i_b].off/2);		//Address LSB
		    pdu += (char)((cntr.acqBlks[i_b].val.size()/2)>>8);	//Number of registers MSB
		    pdu += (char)(cntr.acqBlks[i_b].val.size()/2);	//Number of registers LSB
		    //- Request to remote server -
		    cntr.acqBlks[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlks[i_b].err.empty() )
			cntr.acqBlks[i_b].val.replace(0,cntr.acqBlks[i_b].val.size(),pdu.substr(2).c_str(),cntr.acqBlks[i_b].val.size());
		    else if( atoi(cntr.acqBlks[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlks[i_b].err);
			break;
		    }
		}
		if( cntr.tm_delay > 0 )	continue;
		//- Get input registers -
		for( int i_b = 0; i_b < cntr.acqBlksIn.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    //- Encode request PDU (Protocol Data Units) -
		    pdu = (char)0x4;				//Function, read multiple input registers
		    pdu += (char)((cntr.acqBlksIn[i_b].off/2)>>8);	//Address MSB
		    pdu += (char)(cntr.acqBlksIn[i_b].off/2);		//Address LSB
		    pdu += (char)((cntr.acqBlksIn[i_b].val.size()/2)>>8);	//Number of registers MSB
		    pdu += (char)(cntr.acqBlksIn[i_b].val.size()/2);	//Number of registers LSB
		    //- Request to remote server -
		    cntr.acqBlksIn[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlksIn[i_b].err.empty() )
			cntr.acqBlksIn[i_b].val.replace(0,cntr.acqBlksIn[i_b].val.size(),pdu.substr(2).c_str(),cntr.acqBlksIn[i_b].val.size());
		    else if( atoi(cntr.acqBlksIn[i_b].err.c_str()) == 14 )
		    {
			cntr.setCntrDelay(cntr.acqBlksIn[i_b].err);
			break;
		    }
		}
		res.release();
	    }

	    //- Calc acquisition process time -
	    cntr.tm_gath = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());

	    TSYS::taskSleep((long long)cntr.period()*1000000000);
	}
    }
    catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::setCntrDelay( const string &err )
{
    tm_delay = mod->serConnResume( );
    ResAlloc res( en_res, false );
    for( int i_b = 0; i_b < acqBlksCoil.size(); i_b++ )	acqBlksCoil[i_b].err = err;
    for( int i_b = 0; i_b < acqBlks.size(); i_b++ )	acqBlks[i_b].err = err;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/st/gath_tm",_("Gather data time (ms)"),R_R_R_,"root","root",1,"tp","real");
	if( m_prt == 1 || m_prt == 2 )
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/cntr/cfg/serDevLst");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/gath_tm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_gath,6));
    else if( a_path == "/cntr/cfg/serDevLst" && ctrChkNode(opt) )
    {
	vector<string> sls;
	mod->serDevList(sls);
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
	    if( !enableStat() )			val.setS(_("1:Parameter had disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition is stoped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

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

void TMdPrm::vlSet( TVal &valo )
{
    if( !enableStat() )	valo.setS( EVAL_STR, 0, true );

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
	    "help",_("Attributes configuration list. List writed by lines in format: [dt:numb:rw:id:name]\n"
		    "Where:\n"
		    "  dt - Modbus data type (R-register,C-coil,RI-input register,CI-input coil);\n"
		    "  numb - ModBus device's data address (dec, hex or octal);\n"
		    "  wr - read-write mode (r-read; w-write; rw-readwrite);\n"
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

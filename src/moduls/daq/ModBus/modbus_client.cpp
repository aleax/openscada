
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

#include <tsys.h>
#include <ttiparam.h>

#include "modbus_client.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"ModBus"
#define MOD_NAME	"ModBus client"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.8.0"
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
    fldAdd( new TFld("FRAG_MERGE",_("Register's fragment merge"),TFld::Boolean,TFld::NoFlag,"1","0") );

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
    char *short_opt = "h";
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

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TTipDAQ::cntrCmdProc( opt );
	if( ctrMkNode("area",opt,0,"/mod",_("ModBus")) )
	{
	    if( ctrMkNode("table",opt,-1,"/mod/dev",_("Serial devices"),0664,"root","root",2,"s_com","add,del","key","dev") )
	    {
		ctrMkNode("list",opt,-1,"/mod/dev/dev",_("Device"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/speed",_("Speed"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/mod/dev/lsspd");
		ctrMkNode("list",opt,-1,"/mod/dev/len",_("Char length (bit)"),0664,"root","root",3,"tp","dec","min","5","max","8");
		ctrMkNode("list",opt,-1,"/mod/dev/stop",_("Stop bits"),0664,"root","root",3,"tp","dec","min","1","max","2");
		ctrMkNode("list",opt,-1,"/mod/dev/parity",_("Parity"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/mod/dev/lsprt");
		ctrMkNode("list",opt,-1,"/mod/dev/frTm",_("Time frame"),0664,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/charTm",_("Time char"),0664,"root","root",1,"tp","real");
		ctrMkNode("list",opt,-1,"/mod/dev/reqTm",_("Time request"),0664,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/open",_("Opened"),0664,"root","root",1,"tp","bool");
	    }
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/mod/dev" )
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
    else if( a_path == "/mod/dev/lsspd" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","300")->setText("300");
	opt->childAdd("el")->setAttr("id","600")->setText("600");
	opt->childAdd("el")->setAttr("id","1200")->setText("1200");
	opt->childAdd("el")->setAttr("id","2400")->setText("2400");
	opt->childAdd("el")->setAttr("id","4800")->setText("4800");
	opt->childAdd("el")->setAttr("id","9600")->setText("9600");
	opt->childAdd("el")->setAttr("id","19200")->setText("19.2k");
	opt->childAdd("el")->setAttr("id","38400")->setText("38.4k");
	opt->childAdd("el")->setAttr("id","57600")->setText("57.6k");
	opt->childAdd("el")->setAttr("id","115200")->setText("115.2k");
	opt->childAdd("el")->setAttr("id","230400")->setText("230.4k");
	opt->childAdd("el")->setAttr("id","460800")->setText("460.8k");
    }
    else if( a_path == "/mod/dev/lsprt" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("No check"));
	opt->childAdd("el")->setAttr("id","1")->setText(_("Parity"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("Odd parity"));
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

string SSerial::req( const string &vl )
{
    ResAlloc res( m_res, true );
    if( !hasOpen() )	throw TError(mod->nodePath().c_str(),_("Serial port '%s' no opened."),id().c_str());

    //- Write request -
    if( write( fd, vl.data(), vl.size() ) == -1 )
	throw TError(mod->nodePath().c_str(),_("Write to serial port '%s' is error."),id().c_str());

    //- Read reply -
    char buf[1000];
    int  blen = 0;
    long long tmptm;
    fd_set fdset;

    //-- Char timeout init --
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = vmax((int)(timeoutChar()*1000.0),1);

    //--- Serial timeout ---
    tmptm = TSYS::curTime();
    while( true )
    {
	int bytes = 0;
	ioctl( fd, FIONREAD, &bytes );
	if( bytes > 2 ) break;
	if( (TSYS::curTime() - tmptm) >= vmin(timeoutReq()*1000,10000000) )
	    throw TError(mod->nodePath().c_str(),_("Respond from serial port '%s' is timeouted."),id().c_str());
	usleep( 1000 );
    }
    fcntl( fd, F_SETFL, 0 );
    blen = read( fd, buf, sizeof(buf) );

    //--- Frame timeout ---
    tmptm = TSYS::curTime();
    FD_ZERO( &fdset );
    FD_SET( fd, &fdset );
    while( true )
    {
	if( select(fd+1,&fdset,NULL,NULL,&tv) <= 0 )	break;
	blen += read( fd, buf+blen, sizeof(buf)-blen );
	if( (TSYS::curTime()-tmptm) > vmin(timeoutFrame()*1000,10000000) )	break;
    }

    return string( buf, blen );
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
	TController( name_c, daq_db, cfgelem ), prc_st(false), endrun_req(false), tm_gath(0),
	m_per(cfg("PERIOD").getRd()), m_prior(cfg("PRIOR").getId()), m_prt(cfg("PROT").getId()),
	m_addr(cfg("ADDR").getSd()), m_node(cfg("NODE").getId()), m_merge(cfg("FRAG_MERGE").getBd())
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
	    //cfg("ADDR").setS("devhost.org:502");
	}
	else
	{
	    cfg("ADDR").fld().setDescr(_("Serial port"));
	    //cfg("ADDR").setS("/dev/ttyS0");
	}
	if(startStat())	stop();
    }

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

void TMdContr::regVal( int reg )
{
    if( reg < 0 )	return;

    ResAlloc res( en_res, true );

    //- Register to acquisition block -
    int i_b;
    for( i_b = 0; i_b < acqBlks.size(); i_b++ )
    {
	if( (reg*2) < acqBlks[i_b].off )
	{
	    if( (m_merge || (reg*2+2) >= acqBlks[i_b].off) && (acqBlks[i_b].val.size()+acqBlks[i_b].off-(reg*2)) < MaxLenReq )
	    {
		acqBlks[i_b].val.insert(0,acqBlks[i_b].off-reg*2,0);
		acqBlks[i_b].off = reg*2;
	    }
	    else acqBlks.insert(acqBlks.begin()+i_b,SDataRec(reg*2,2));
	}
	else if( (reg*2+2) > (acqBlks[i_b].off+acqBlks[i_b].val.size()) )
	{
	    if( (m_merge || reg*2 <= (acqBlks[i_b].off+acqBlks[i_b].val.size())) && (reg*2+2-acqBlks[i_b].off) < MaxLenReq )
	    {
		acqBlks[i_b].val.append((reg*2+2)-(acqBlks[i_b].off+acqBlks[i_b].val.size()),0);
		//- Check for allow mergin to next block -
		if( !m_merge && i_b+1 < acqBlks.size() && (acqBlks[i_b].off+acqBlks[i_b].val.size()) >= acqBlks[i_b+1].off )
		{
		    acqBlks[i_b].val.append(acqBlks[i_b+1].val,acqBlks[i_b].off+acqBlks[i_b].val.size()-acqBlks[i_b+1].off,string::npos);
		    acqBlks.erase(acqBlks.begin()+i_b+1);
		}
	    }
	    else continue;
	}
	break;
    }
    if( i_b >= acqBlks.size() )
	acqBlks.insert(acqBlks.begin()+i_b,SDataRec(reg*2,2));
}

int TMdContr::getVal( int reg, string &err )
{
    int rez = EVAL_INT;
    for( int i_b = 0; i_b < acqBlks.size(); i_b++ )
	if( (reg*2) >= acqBlks[i_b].off && (reg*2+2) <= (acqBlks[i_b].off+acqBlks[i_b].val.size()) )
	{
	    err = acqBlks[i_b].err;
	    if( err.empty() )
		rez = (acqBlks[i_b].val[reg*2-acqBlks[i_b].off]<<8)+acqBlks[i_b].val[reg*2-acqBlks[i_b].off+1];
	    break;
        }
    return rez;
}

void TMdContr::setVal( int val, int reg, string &err )
{
    //- Encode request PDU (Protocol Data Units) --
    string pdu;
    pdu = (char)0x6;		//Function, read multiple registers
    pdu += (char)(reg>>8);	//Address MSB
    pdu += (char)reg;		//Address LSB
    pdu += (char)(val>>8);	//Data MSB
    pdu += (char)val;		//Data LSB
    //- Request to remote server -
    err = modBusReq( pdu );
    //- Set to acquisition block -
    for( int i_b = 0; i_b < acqBlks.size(); i_b++ )
	if( (reg*2) >= acqBlks[i_b].off && (reg*2+2) <= (acqBlks[i_b].off+acqBlks[i_b].val.size()) )
	{
	    acqBlks[i_b].val[reg*2-acqBlks[i_b].off]   = (char)(val<<8);
	    acqBlks[i_b].val[reg*2-acqBlks[i_b].off+1] = (char)val;
	    break;
	}
}

string TMdContr::modBusReq( string &pdu )
{
    ResAlloc res( req_res, true );

    string mbap;
    unsigned char buf[1000];

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
		mbap  = (char)0x15;		//Transaction ID MSB
		mbap += (char)0x01;		//Transaction ID LSB
		mbap += (char)0x00;		//Protocol ID MSB
		mbap += (char)0x00;		//Protocol ID LSB
		mbap += (char)(pdu.size()>>8);	//PDU size MSB
		mbap += (char)pdu.size();	//PDU size LSB
		mbap += (char)m_node;		//Unit identifier

		if( !tr.at().startStat() )	tr.at().start();
		//- Send request -
		int resp_len = tr.at().messIO( (mbap+pdu).c_str(), mbap.size()+pdu.size(), (char*)buf, sizeof(buf), 10 );
		if( resp_len < mbap.size() ) return _("13:Error server respond");
		int resp_sz = (buf[4]<<8)+buf[5];
		pdu.assign( (char*)buf+mbap.size(), resp_len-mbap.size() );
		//- Wait tail -
		while( pdu.size() < resp_sz-1 )
		{
		    resp_len = tr.at().messIO( NULL, 0, (char*)buf, sizeof(buf), 10 );
		    pdu.append( (char*)buf, resp_len );
		}
		break;
	    }
	    case 1:	// Modbus/RTU protocol process
	    {
		mbap = (ui8)m_node;		//Unit identifier
		mbap += pdu;
		ui16 crc = mod->CRC16( mbap );
		mbap += crc >> 8;
		mbap += crc;
		string rez = mod->serDevAt(m_addr).at().req(mbap);
		if( rez.size() < 2 ) return _("13:Error respond: Too short.");
		if( mod->CRC16(rez.substr(0,rez.size()-2)) != (ui16)((rez[rez.size()-2]<<8)+(ui8)rez[rez.size()-1]) )
		    return _("13:Error respond: CRC check error.");
		pdu = rez.substr( 1, rez.size()-3 );
		break;
	    }
	    case 2:	// Modbus/ASCII protocol process
	    {
		mbap = (ui8)m_node;		//Unit identifier
		mbap += pdu;
		mbap += mod->LRC(mbap);
		mbap = ":"+mod->DataToASCII(mbap)+"\0x0D\0x0A";
		string rez = mod->serDevAt(m_addr).at().req(mbap);
		if( rez.size() < 3 || rez[0] != ':' || rez[rez.size()-2] != 0x0D || rez[rez.size()-1] != 0x0A )
		    return _("13:Error respond: Error format.");
		rez = mod->ASCIIToData(rez.substr(1,rez.size()-3));
		if( mod->LRC(rez.substr(0,rez.size()-1)) != rez[rez.size()-1] )
		    return _("13:Error respond: LRC check error.");
		pdu = rez.substr(1,rez.size()-2);
		break;
	    }
	}
	//- Check respond pdu -
	if( pdu.size() < 2 ) return _("13:Error respond");
	if( pdu[0]&0x80 )
	    switch( pdu[1] )
	    {
		case 0x1:
		    snprintf( (char*)buf, sizeof(buf), _("1:Function %xh no support."), pdu[0]&(~0x80) );
		    return (char*)buf;
		case 0x2: return _("2:Requested registers length too long.");
		case 0x3: return _("3:Illegal data value.");
		case 0x4: return _("4:Server failure.");
		case 0x5: return _("5:Request require too long time for execute.");
		case 0x6: return _("6:Server busy.");
		case 0xA: case 0xB: return _("10:Gateway problem.");
		default:
		    snprintf( (char*)buf, sizeof(buf), _("12:Unknown error: %xh."), pdu[1] );
		    return (char*)buf;
	    }
    }catch( TError err )
    {
	//mess_err( err.cat.c_str(), err.mess.c_str() );
	return _("14:Request is error: ")+err.mess;
    }

    return "";
}

void *TMdContr::Task( void *icntr )
{
    string pdu;
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {
	while( !cntr.endrun_req )
	{
	    long long t_cnt = SYS->shrtCnt();

	    //- Update controller's data -
	    ResAlloc res( cntr.en_res, false );
	    for( int i_p = 0; i_p < cntr.p_hd.size(); i_p++ )
		for( int i_b = 0; i_b < cntr.acqBlks.size(); i_b++ )
		{
		    if( cntr.endrun_req ) break;
		    //- Encode request PDU (Protocol Data Units) -
		    pdu = (char)0x3;					//Function, read multiple registers
		    pdu += (char)((cntr.acqBlks[i_b].off/2)>>8);	//Address MSB
		    pdu += (char)(cntr.acqBlks[i_b].off/2);		//Address LSB
		    pdu += (char)((cntr.acqBlks[i_b].val.size()/2)>>8);	//Number of registers MSB
		    pdu += (char)(cntr.acqBlks[i_b].val.size()/2);	//Number of registers LSB
		    //- Request to remote server -
		    cntr.acqBlks[i_b].err = cntr.modBusReq( pdu );
		    if( cntr.acqBlks[i_b].err.empty() )
			cntr.acqBlks[i_b].val.replace(0,cntr.acqBlks[i_b].val.size(),pdu.substr(2).c_str(),cntr.acqBlks[i_b].val.size());
		}
	    res.release();

	    //- Calc acquisition process time -
	    cntr.tm_gath = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());

	    //- Calc next work time and sleep -
	    clock_gettime( CLOCK_REALTIME, &get_tm );
	    work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/(long long)(cntr.period()*1000000000) + 1)*(long long)(cntr.period()*1000000000);
	    if( last_tm == work_tm )	work_tm += (long long)(cntr.period()*1000000000);	//Fix early call
	    last_tm = work_tm;
	    get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
	    clock_nanosleep( CLOCK_REALTIME, TIMER_ABSTIME, &get_tm, NULL );
	}
    }
    catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode( "fld", opt, -1, "/cntr/st/gath_tm", _("Gather data time (ms)"), 0444, "root", "root", 1, "tp", "real" );
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
	aid = TSYS::strSepParse( als[i_l], 0, ':' );
	if( vlPresent(aid) && !p_el.fldPresent(aid) )	continue;
	if( !p_el.fldPresent(aid) )
	    p_el.fldAdd( new TFld( aid.c_str(), "", TFld::Integer, TFld::NoFlag, "", TSYS::int2str(EVAL_INT).c_str(), "0:65535" ) );
	int el_id = p_el.fldId(aid);
	p_el.fldAt(el_id).setDescr( TSYS::strSepParse(als[i_l],1,':') );
	p_el.fldAt(el_id).setReserve( TSYS::strSepParse(als[i_l],2,':') );
	p_el.fldAt(el_id).setFlg( atoi(TSYS::strSepParse(als[i_l],3,':').c_str()) ? (int)(TVal::DirWrite|TVal::DirRead) : (int)(TFld::NoWrite|TVal::DirRead) );
	owner().regVal(atoi(TSYS::strSepParse(als[i_l],2,':').c_str()));
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
    int aid = atoi(val.fld().reserve().c_str());
    if( aid )	val.setI(owner().getVal(aid,acq_err),0,true);
    else if( val.name() == "err" )
    {
	if( acq_err.empty() )	val.setS("0",0,true);
	else			val.setS(acq_err,0,true);
    }
}

void TMdPrm::vlSet( TVal &valo )
{
    if( !enableStat() )	valo.setI( EVAL_INT, 0, true );
    owner().setVal(valo.getI(NULL,true),atoi(valo.fld().reserve().c_str()),acq_err);
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
	    "help",_("Attributes configuration list. List writed by lines in format: [<numb>:<wr>:<id>:<name>]\n"
		    "Where:\n"
		    "  numb - ModBus device's register number (decimal);\n"
		    "  wr - write allow (1-allow; 0-deny);\n"
		    "  id - created attribute identifier;\n"
		    "  name - created attribute name."));
	return;
    }
    //- Process command to page -
    TParamContr::cntrCmdProc(opt);
}

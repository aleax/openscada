
//OpenSCADA system module Protocol.ModBus file: modbus_prt.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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
#include <string.h>

#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

#include "modbus_daq.h"
#include "modbus_prt.h"

ModBus::TProt *ModBus::modPrt;

using namespace ModBus;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : mPrtLen(0)
{
    mId		= PRT_ID;
    mType	= PRT_TYPE;
    mName	= PRT_NAME;
    mVers	= PRT_MVER;
    mAutor	= PRT_AUTORS;
    mDescr	= PRT_DESCR;
    mLicense	= PRT_LICENSE;
    mSource	= name;

    modPrt	= this;
}

TProt::~TProt()
{

}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	PRT_TYPE,PRT_ID,nodePath().c_str());

    return buf;
}

void TProt::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL, 0 }
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
}

void TProt::save_( )
{

}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

ui8 TProt::CRCHi[] =
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

ui8 TProt::CRCLo[] =
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

ui16 TProt::CRC16( const string &mbap )
{
    ui8 hi = 0xFF;
    ui8 lo = 0xFF;
    ui16 index;
    for( int i_b = 0; i_b < mbap.size(); i_b++ )
    {
        index = lo^(ui8)mbap[i_b];
        lo = hi^CRCHi[index];
        hi = CRCLo[index];
    }
    return hi|(lo<<8);
}

ui8 TProt::LRC( const string &mbap )
{
    ui8 ch = 0;
    for( int i_b = 0; i_b < mbap.size(); i_b++ )
        ch += (ui8)mbap[i_b];

    return ch;
}

string TProt::DataToASCII( const string &in )
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

string TProt::ASCIIToData( const string &in )
{
    ui8 ch1, ch2;
    string rez;

    for( int i=0; i < (in.size()&(~0x01)); i+=2 )
    {
        ch2 = 0;
        ch1 = in[i];
        if( ch1 >= '0' && ch1 <= '9' )          ch1 -= '0';
        else if( ch1 >= 'A' && ch1 <= 'F' )     ch1 -= ('A' + 10);
        else                                    ch1 = 0;
        ch2 = ch1 << 4;
        ch1 = in[i+1];
        if( ch1 >= '0' && ch1 <= '9' )          ch1 -= '0';
        else if ( ch1 >= 'A' && ch1 <= 'F' )    ch1 -= ('A' + 10);
        else                                    ch1 = 0;
        rez += ch2|ch1;
    }

    return rez;
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string mbap, err, rez;
    char buf[1000];

    string prt   = io.name();
    string sid   = io.attr("id");
    int    reqTm = atoi(io.attr("reqTm").c_str());
    int    node  = atoi(io.attr("node").c_str());
    int    reqTry = vmin(10,vmax(1,atoi(io.attr("reqTry").c_str())));
    string pdu   = io.text();

    try
    {
	if( prt == "TCP" )		// Modbus/TCP protocol process
	{
	    //> Encode MBAP (Modbus Application Protocol)
	    mbap  = (char)0x15;			//Transaction ID MSB
	    mbap += (char)0x01;			//Transaction ID LSB
	    mbap += (char)0x00;			//Protocol ID MSB
	    mbap += (char)0x00;			//Protocol ID LSB
	    mbap += (char)((pdu.size()+1)>>8);	//PDU size MSB
	    mbap += (char)(pdu.size()+1);	//PDU size LSB
	    mbap += (char)node;			//Unit identifier
	    mbap += pdu;

	    //> Send request
	    int resp_len = tro.messIO( mbap.data(), mbap.size(), buf, sizeof(buf), reqTm );
	    rez.assign(buf,resp_len);
	    if( rez.size() < 7 )	err = _("13:Error server respond");
	    else
	    {
	    	int resp_sz = (unsigned short)(rez[4]<<8)|(unsigned char)rez[5];

	    	//> Wait tail
	    	while( rez.size() < (resp_sz+6) )
	    	{
		    resp_len = tro.messIO( NULL, 0, buf, sizeof(buf), 10000 );
		    rez.append( buf, resp_len );
		}
		pdu = rez.substr(7);
	    }
	}
	else if( prt == "RTU" )		// Modbus/RTU protocol process
	{
	    mbap = (ui8)node;			//Unit identifier
	    mbap += pdu;
	    ui16 crc = CRC16( mbap );
	    mbap += crc >> 8;
	    mbap += crc;

	    //> Send request
	    for( int i_tr = 0; i_tr < reqTry; i_tr++ )
	    {
		int resp_len = tro.messIO( mbap.data(), mbap.size(), buf, sizeof(buf), reqTm );
		rez.assign(buf,resp_len);

		if( rez.size() < 2 )	{ err = _("13:Error respond: Too short."); continue; }
		if( CRC16(rez.substr(0,rez.size()-2)) != (ui16)((rez[rez.size()-2]<<8)+(ui8)rez[rez.size()-1]) )
		{ err = _("13:Error respond: CRC check error."); continue; }
		pdu = rez.substr( 1, rez.size()-3 );
		err = "";
		break;
	    }
	}
	else if( prt == "ASCII" )	// Modbus/ASCII protocol process
	{
	    mbap = (ui8)node;			//Unit identifier
	    mbap += pdu;
	    mbap += LRC(mbap);
	    mbap = ":"+DataToASCII(mbap)+"\r\n";

	    //> Send request
	    for( int i_tr = 0; i_tr < reqTry; i_tr++ )
	    {
		int resp_len = tro.messIO( mbap.data(), mbap.size(), buf, sizeof(buf), reqTm );
		rez.assign(buf,resp_len);

		if( rez.size() < 3 || rez[0] != ':' || rez.substr(rez.size()-2,2) != "\r\n" )
		{ err = _("13:Error respond: Error format."); continue; }
		rez = ASCIIToData(rez.substr(1,rez.size()-3));
		if( LRC(rez.substr(0,rez.size()-1)) != rez[rez.size()-1] )
		{ err = _("13:Error respond: LRC check error."); continue; }
		pdu = rez.substr(1,rez.size()-2);
		err = "";
		break;
	    }
	}
	else err = TSYS::strMess(_("Protocol '%s' error."),prt.c_str());

	//> Check respond pdu
	if( err.empty() )
	{
	    if( pdu.size() < 2 ) err = _("13:Error respond");
	    if( pdu[0]&0x80 )
		switch( pdu[1] )
		{
		    case 0x1: err = TSYS::strMess(_("1:Function %xh is not supported."),pdu[0]&(~0x80));	break;
		    case 0x2: err = _("2:Requested registers' length is too long.");	break;
		    case 0x3: err = _("3:Illegal data value.");				break;
		    case 0x4: err = _("4:Server failure.");				break;
		    case 0x5: err = _("5:Request requires too long time for execute.");	break;
		    case 0x6: err = _("6:Server is busy.");				break;
		    case 0xA: case 0xB: err = _("10:Gateway problem.");			break;
		    default: err = TSYS::strMess(_("12:Unknown error: %xh."),pdu[1]);	break;
		}
	}
    }catch( TError er ) { err = _("14:Device error: ")+er.mess; }

    io.setText(err.empty()?pdu:"");
    if( !err.empty() ) io.setAttr("err",err);

    //> Prepare log
    if( prtLen( ) )
    {
	time_t tm_t = time(NULL);
	string mess = TSYS::strSepParse(ctime(&tm_t),0,'\n')+" '"+sid+"' "+prt+":"+TSYS::int2str(node)+"("+tro.addr()+")\n"+
	    _("REQ -> ")+TSYS::strDecode(mbap,TSYS::Bin)+"\n";
	if( !err.empty() ) mess += _("ERR -> ")+err;
	else mess += _("RESP -> ")+TSYS::strDecode(rez,TSYS::Bin);
	pushPrtMess(mess+"\n");
    }
}

void TProt::setPrtLen( int vl )
{
    ResAlloc res(nodeRes(),true);

    while( mPrt.size() > vl )	mPrt.pop_back();

    mPrtLen = vl;
}

void TProt::pushPrtMess( const string &vl )
{
    ResAlloc res(nodeRes(),true);

    if( !prtLen() )	return;

    mPrt.push_front(vl);

    while( mPrt.size() > prtLen() )	mPrt.pop_back();
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	if( ctrMkNode("area",opt,-1,"/out",_("Output")) )
	{
	    ctrMkNode("fld",opt,-1,"/out/protLen",_("Protocol length"),0664,"root","DAQ",4,"tp","dec","min","0","max","10000",
		"help",_("Zero use for protocol disabling"));
	    if( prtLen() )
		ctrMkNode("fld",opt,-1,"/out/prot",_("Protocol"),0444,"root","DAQ",3,"tp","str","cols","90","rows","20");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/out/protLen" )
    {
	if( ctrChkNode(opt,"get",0664,"root","DAQ",SEQ_RD) )	opt->setText( TSYS::int2str(prtLen()) );
	if( ctrChkNode(opt,"set",0664,"root","DAQ",SEQ_WR) )	setPrtLen( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/out/prot" && ctrChkNode(opt) )
    {
	ResAlloc res(nodeRes(),true);
	for( int i_p = 0; i_p < mPrt.size(); i_p++ )
	    opt->setText(opt->text()+mPrt[i_p]+"\n");
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else TProtocol::cntrCmdProc(opt);
}


//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

TProtIn::~TProtIn()
{

}

bool TProtIn::mess( const string &reqst, string &answer, const string &sender )
{
    //!!!! No released yet
    answer = reqst;

    return false;
}


//OpenSCADA system module Transport.Serial file: mod_serial.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>

#include "mod_serial.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"Serial"
#define MOD_NAME	"Serial interfaces"
#define MOD_TYPE	"Transport"
#define VER_TYPE	VER_TR
#define VERSION		"0.5.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow serial based interfaces. Used for data exchanging through serial interfaces like RS232, RS485, GSM and other."
#define LICENSE		"GPL"
//************************************************

Serial::TTr *Serial::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 ) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new Serial::TTr( source );
	return NULL;
    }
}

using namespace Serial;

//************************************************
//* TTr						 *
//************************************************
TTr::TTr( string name )
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

TTr::~TTr( )
{

}

void TTr::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeConnect )
    {
	//> Add self DB-fields to input transport
	owner().inEl().fldAdd( new TFld("BufLen",_("Input buffer length (kB)"),TFld::Integer,0,"4","5") );
	owner().inEl().fldAdd( new TFld("TMS",_("Timings"),TFld::String,0,"100","0.57:320") );

	//> Add self DB-fields to input transport
	owner().outEl().fldAdd( new TFld("TMS",_("Timings"),TFld::String,0,"100","640:0.57:320") );
    }
}


string TTr::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TTr::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
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
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

TTransportIn *TTr::In( const string &name, const string &idb )
{
    return new TTrIn(name,idb,&owner().inEl());
}

TTransportOut *TTr::Out( const string &name, const string &idb )
{
    return new TTrOut(name,idb,&owner().outEl());
}

void TTr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TTipTransport::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->setText(optDescr());
    else TTipTransport::cntrCmdProc(opt);
}

//************************************************
//* TTrIn                                        *
//************************************************
TTrIn::TTrIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), trIn(0), trOut(0), fd(-1), mTimings(cfg("TMS").getSd())
{
    setAddr("/dev/ttyS0:19200:8:2:p");
}

TTrIn::~TTrIn()
{
    try{ stop(); }catch(...){ }
}

string TTrIn::getStatus( )
{
    string rez = TTransportIn::getStatus( );

    if( startStat() ) rez += TSYS::strMess(_("Traffic in %.4g kb, out %.4g kb."),trIn,trOut);

    return rez;
}

void TTrIn::setAddr( const string &iaddr )
{
    TTransportIn::setAddr(iaddr);

    //> Times adjust
    int speed = atoi(TSYS::strSepParse(iaddr,1,':').c_str());
    if( speed )	setTimings(TSYS::real2str((11*1000)/(float)speed,2,'f')+":"+TSYS::int2str((512*11*1000)/speed));
}

void TTrIn::start()
{
    if( run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    try
    {
	//> Open and setup device
	//>> Serial port open
	string port = TSYS::strSepParse(addr(),0,':');
	fd = open( port.c_str(), O_RDWR|O_NOCTTY );
	if( fd < 0 ) throw TError(nodePath().c_str(),_("Serial port '%s' open error."),port.c_str());
	//>> Set serial port parameters
	struct termios tio;
	bzero( &tio, sizeof(tio) );
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = B9600|CS8|CREAD|CLOCAL;
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 1;            ///< blocking read until 1 character arrives
	//>> Set byte length
	int len = atoi(TSYS::strSepParse(addr(),2,':').c_str());
	if( len < 5 || len > 8 ) throw TError(nodePath().c_str(),_("Char length '%d' error."),len);
	tio.c_cflag &= ~CSIZE;
	switch(len)
	{
	    case 5:	tio.c_cflag |= CS5;	break;
	    case 6:	tio.c_cflag |= CS6;	break;
	    case 7:	tio.c_cflag |= CS7;	break;
	    case 8:	tio.c_cflag |= CS8;	break;
	}
	//>> Set stop bits number
	int stopbt = atoi(TSYS::strSepParse(addr(),3,':').c_str());
	if( stopbt == 1 ) tio.c_cflag |= CSTOPB;
	else if( stopbt == 2 ) tio.c_cflag &= ~CSTOPB;
	else throw TError(nodePath().c_str(),_("Stop bits '%d' error."),stopbt);
	//>> Set parity
	string parity = TSYS::strNoSpace(TSYS::strSepParse(addr(),4,':'));
	if( parity == "p" )		{ tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD; }
	else if( parity == "n" )	{ tio.c_cflag |= PARENB; tio.c_cflag |= PARODD; }
	else if( parity == "0" )	tio.c_cflag &= ~PARENB;
	else throw TError(nodePath().c_str(),_("Parity checking mode '%s' error."),parity.c_str());
	//>> Set speed
	int speed = atoi(TSYS::strSepParse(addr(),1,':').c_str());
	speed_t tspd = B9600;
	switch( speed )
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
	    case 500000:tspd = B500000;	break;
	    case 576000:tspd = B576000;	break;
	    case 921600:tspd = B921600;	break;
	    default: throw TError(nodePath().c_str(),_("Speed '%d' error."),speed);
	}
	cfsetispeed( &tio, tspd );
	cfsetospeed( &tio, tspd );
	//>> Set port's data
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );

	//> Start listen task
	pthread_attr_t pthr_attr;
	pthread_attr_init(&pthr_attr);
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait( run_st, true,nodePath()+"open",5) )
	    throw TError(nodePath().c_str(),_("Not opened!"));
    }
    catch(TError err)
    {
	if( fd >= 0 ) { close(fd); fd = -1; }
	throw;
    }
}

void TTrIn::stop()
{
    if( !run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    endrun = true;
    if( TSYS::eventWait( run_st, false, nodePath()+"close",5) )
	throw TError(nodePath().c_str(),_("Not closed!"));
    pthread_join( pthr_tsk, NULL );

    close(fd); fd = -1;
}

void *TTrIn::Task( void *tr_in )
{
    TTrIn *tr = (TTrIn*)tr_in;

#if OSC_DEBUG >= 2
    mess_debug(sock->nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    tr->run_st	= true;
    tr->endrun	= false;

    AutoHD<TProtocolIn> prot_in;
    struct timeval tv;
    int r_len;
    string req, answ;
    char buf[1000];
    fd_set fdset;

    double wCharTm = atof(TSYS::strSepParse(tr->timings(),0,':').c_str());
    wCharTm = vmax(0.001,wCharTm);
    int wFrTm = atoi(TSYS::strSepParse(tr->timings(),1,':').c_str());
    wFrTm = 1000*vmin(10000,wFrTm);
    long long stFrTm;

    fcntl( tr->fd, F_SETFL, 0 );

    while( !tr->endrun )
    {
	//>> Char timeout
	while(true)
	{
	    tv.tv_sec = 0; tv.tv_usec = (int)(1500.0*wCharTm);
	    FD_ZERO( &fdset ); FD_SET( tr->fd, &fdset );

	    if( select( tr->fd+1, &fdset, NULL, NULL, &tv ) <= 0 )
	    {
		if( tr->endrun || !req.empty() )	break;
		continue;
	    }
	    r_len = read( tr->fd, buf, sizeof(buf));
	    if( r_len <= 0 ) break;
	    if( req.empty() ) stFrTm = TSYS::curTime();
	    req += string(buf,r_len);
	    if( (TSYS::curTime()-stFrTm) > wFrTm )	break;
	}
	if( tr->endrun || req.empty() ) break;

	tr->trIn += (float)req.size()/1024;

#if OSC_DEBUG >= 5
	mess_debug( nodePath().c_str(), _("Serial received message <%d>."), req.size() );
#endif

	//> Send message to protocol
	try
	{
	    if( prot_in.freeStat() )
	    {
		AutoHD<TProtocol> proto = SYS->protocol().at().modAt(tr->protocol());
		string n_pr = tr->id()+TSYS::int2str(tr->fd);
		if( !proto.at().openStat(n_pr) ) proto.at().open( n_pr, tr->workId() );
		prot_in = proto.at().at( n_pr );
	    }
	    prot_in.at().mess(req,answ,"");
	}catch(TError err)
	{
	    mess_err(tr->nodePath().c_str(),"%s",err.mess.c_str() );
	    mess_err(tr->nodePath().c_str(),_("Error request to protocol."));
	}

	//> Send respond
	if( answ.size() )
	{
#if OSC_DEBUG >= 5
	    mess_debug( nodePath().c_str(), _("Serial replied message <%d>."), answ.size() );
#endif
	    r_len = write( tr->fd, answ.c_str(), answ.size() ); tr->trOut += (float)r_len/1024;
	    answ = "";
	}
	req = "";
    }

    //> Close protocol
    if( !prot_in.freeStat() )
    {
	string n_pr = prot_in.at().name();
	prot_in.free();
	SYS->protocol().at().at(tr->protocol()).at().close(n_pr);
    }

    tr->run_st = false;

    return NULL;
}

void TTrIn::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TTransportIn::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",2,"tp","str","help",
	    _("Serial transport has address format: \"[dev]:[speed]:[len]:[stop]:[parity]\". Where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600);\n"
	    "    len - symbol length (bites: 7,8);\n"
	    "    stop - stop bites number (1 or 2);\n"
	    "    parity - parity check (p-parity,n-odd parity,0-disable)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),0664,"root","root",2,"tp","str","help",
	    _("Connection timings in format: \"[symbol]:[frm]\". Where:\n"
	    "    symbol - one symbol maximum time, used for frame end detection, in ms;\n"
	    "    frm - maximum frame length, in ms."));
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/TMS" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(timings());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setTimings(opt->text());
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TTrOut                                   *
//************************************************
TTrOut::TTrOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), mTimings(cfg("TMS").getSd()), fd(-1), mLstReqTm(0)
{
    setAddr("/dev/ttyS0:19200:8:2:p");
}

TTrOut::~TTrOut()
{
    if( startStat() )	stop();
}

string TTrOut::getStatus( )
{
    string rez = TTransportOut::getStatus( );

    if( startStat() )	rez += TSYS::strMess(_("Traffic in %.4g kb, out %.4g kb."),trIn,trOut);

    return rez;
}

void TTrOut::setAddr( const string &iaddr )
{
    TTransportOut::setAddr(iaddr);

    //> Times adjust
    int speed = atoi(TSYS::strSepParse(iaddr,1,':').c_str());
    if( speed )	setTimings(TSYS::int2str((1024*11*1000)/speed)+":"+TSYS::real2str((11*1000)/(float)speed,2,'f')+":"+TSYS::int2str((512*11*1000)/speed));
}

void TTrOut::start( )
{
    if( run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    try
    {
	//> Open and setup device
	//>> Serial port open
	string port = TSYS::strSepParse(addr(),0,':');
	fd = open( port.c_str(), O_RDWR|O_NOCTTY );
	if( fd < 0 ) throw TError(nodePath().c_str(),_("Serial port '%s' open error."),port.c_str());
	//>> Set serial port parameters
	struct termios tio;
	bzero( &tio, sizeof(tio) );
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = B9600|CS8|CREAD|CLOCAL;
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 1;            ///< blocking read until 1 character arrives
	//>> Set byte length
	int len = atoi(TSYS::strSepParse(addr(),2,':').c_str());
	if( len < 5 || len > 8 ) throw TError(nodePath().c_str(),_("Char length '%d' error."),len);
	tio.c_cflag &= ~CSIZE;
	switch(len)
	{
	    case 5:	tio.c_cflag |= CS5;	break;
	    case 6:	tio.c_cflag |= CS6;	break;
	    case 7:	tio.c_cflag |= CS7;	break;
	    case 8:	tio.c_cflag |= CS8;	break;
	}
	//>> Set stop bits number
 	int stopbt = atoi(TSYS::strSepParse(addr(),3,':').c_str());
	if( stopbt == 1 ) tio.c_cflag |= CSTOPB;
	else if( stopbt == 2 ) tio.c_cflag &= ~CSTOPB;
	else throw TError(nodePath().c_str(),_("Stop bits '%d' error."),stopbt); 
	//>> Set parity
	string parity = TSYS::strNoSpace(TSYS::strSepParse(addr(),4,':'));
	if( parity == "p" )		{ tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD; }
	else if( parity == "n" )	{ tio.c_cflag |= PARENB; tio.c_cflag |= PARODD; }
	else if( parity == "0" )	tio.c_cflag &= ~PARENB;
	else throw TError(nodePath().c_str(),_("Parity checking mode '%s' error."),parity.c_str());
	//>> Set speed
	int speed = atoi(TSYS::strSepParse(addr(),1,':').c_str());
	speed_t tspd = B9600;
	switch( speed )
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
	    case 500000:tspd = B500000;	break;
	    case 576000:tspd = B576000;	break;
	    case 921600:tspd = B921600;	break;
	    default: throw TError(nodePath().c_str(),_("Speed '%d' error."),speed);
	}
	cfsetispeed( &tio, tspd );
	cfsetospeed( &tio, tspd );
	//>> Set port's data
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    catch(TError err)
    {
	if( fd >= 0 ) { close(fd); fd = -1; }
	throw;
    }

    run_st = true;
}

void TTrOut::stop()
{
    if( !run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    close(fd); fd = -1;

    run_st = false;
}

int TTrOut::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time )
{
    int blen = 0, off = 0;
    fd_set fdset;

    ResAlloc res( nodeRes(), true );

    if( !run_st ) throw TError(nodePath().c_str(),_("Transport is not started!"));

    int wReqTm = atoi(TSYS::strSepParse(timings(),0,':',&off).c_str());
    wReqTm = time?(1000*time):(1000*vmin(10000,wReqTm));
    double wCharTm = atof(TSYS::strSepParse(timings(),0,':',&off).c_str());
    wCharTm = vmax(0.001,wCharTm);
    int wFrTm = atoi(TSYS::strSepParse(timings(),0,':',&off).c_str());
    wFrTm = 1000*vmin(10000,wFrTm);

    long long tmptm = TSYS::curTime();

    //> Write request
    if( obuf && len_ob > 0 )
    {
	if( (tmptm-mLstReqTm) < (5500*wCharTm) ) usleep( (int)((5500*wCharTm)-(tmptm-mLstReqTm)) );
	tcflush( fd, TCIFLUSH );
	if( write(fd,obuf,len_ob) == -1 ) throw TError(nodePath().c_str(),_("Writing request error."));
	trOut += (float)len_ob/1024;
    }

    //> Read reply
    if( ibuf != NULL && len_ib > 0 )
    {
	bool isEnter = true;
	while( true )
	{
	    int bytes = 0;
	    ioctl( fd, FIONREAD, &bytes );
	    //>> Reset old broken session's data
	    if( bytes > 2 ) break;
	    //>> Connection timeout
	    mLstReqTm = TSYS::curTime();
	    if( (mLstReqTm-tmptm) >= wReqTm )
		throw TError(nodePath().c_str(),_("Respond from remote device is timeouted."));
	    usleep( 1000 );
	    isEnter = false;
	}
	//fcntl( fd, F_SETFL, 0 );
	blen = read( fd, ibuf, len_ib );

	//>> Wait tail
	tmptm = mLstReqTm;
	struct timeval tv;
	while( true )
	{
	    //>> Char timeout
	    tv.tv_sec = 0; tv.tv_usec = (int)(1500.0*wCharTm);
	    FD_ZERO( &fdset ); FD_SET( fd, &fdset );

	    if( select(fd+1,&fdset,NULL,NULL,&tv) <= 0 ) break;
	    blen += read( fd, ibuf+blen, len_ib-blen );
	    //>> Frame timeout
	    mLstReqTm = TSYS::curTime();
	    if( (mLstReqTm-tmptm) > wFrTm )	break;
	}
	trIn += (float)blen/1024;
    }

    return blen;
}

void TTrOut::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TTransportOut::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",2,"tp","str","help",
	    _("Serial transport has address format: \"[dev]:[speed]:[len]:[stop]:[parity]\". Where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600 );\n"
	    "    len - symbol length (bites: 7,8);\n"
	    "    stop - stop bites number (1 or 2);\n"
	    "    parity - parity check (p-parity,n-odd parity,0-disable)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),0664,"root","root",2,"tp","str","help",
	    _("Connection timings in format: \"[conn]:[symbol]:[frm]\". Where:\n"
	    "    conn - maximum time for connection respond wait, in ms;\n"
	    "    symbol - one symbol maximum time, used for frame end detection, in ms;\n"
	    "    frm - maximum frame length, in ms."));
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/TMS" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(timings());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setTimings(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

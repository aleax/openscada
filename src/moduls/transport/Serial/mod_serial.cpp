
//OpenSCADA system module Transport.Serial file: mod_serial.cpp
/***************************************************************************
 *   Copyright (C) 2009-2010 by Roman Savochenko                           *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>

#include "mod_serial.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"Serial"
#define MOD_NAME	_("Serial interfaces")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"0.7.1"
#define AUTORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow serial based interfaces. Used for data exchanging through serial interfaces like RS232, RS485, GSM and other.")
#define LICENSE		"GPL2"
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
TTr::TTr( string name ) : TTipTransport(MOD_ID), prcSt(false)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //> Create transports checking timer
    struct sigevent sigev;
    memset(&sigev,0,sizeof(sigev));
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
}

TTr::~TTr( )
{
    try{ modStop(); }catch(...){}

    timer_delete(tmId);
}

void TTr::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeConnect )
    {
	owner().inEl().fldAdd( new TFld("A_PRMS",_("Addon parameters"),TFld::String,TFld::FullText,"10000") );
	owner().outEl().fldAdd( new TFld("A_PRMS",_("Addon parameters"),TFld::String,TFld::FullText,"10000") );
    }
}

AutoHD<TTrIn> TTr::inAt( const string &name )	{ return TTipTransport::inAt(name); }

AutoHD<TTrOut> TTr::outAt( const string &name )	{ return TTipTransport::outAt(name); }

void TTr::modStart( )
{
    //> Start interval timer for transports checking
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = CHECK_TR_PER;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
}

void TTr::modStop( )
{
    //> Stop interval timer for transports checking
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prcSt, false, nodePath()+"stop",5) )
	throw TError(nodePath().c_str(),_("Check transports thread is not stopped!"));
}

void TTr::load_( )
{
    //> Load parameters from command line

}

TTransportIn *TTr::In( const string &name, const string &idb )
{
    return new TTrIn(name,idb,&owner().inEl());
}

TTransportOut *TTr::Out( const string &name, const string &idb )
{
    return new TTrOut(name,idb,&owner().outEl());
}

bool TTr::devLock( const string &dn, bool check )
{
    ResAlloc res( nodeRes(), true );

    if( check ) return mDevLock[dn];
    if( mDevLock[dn] ) return false;
    mDevLock[dn] = true;
    return true;
}

void TTr::devUnLock( const string &dn )
{
    ResAlloc res( nodeRes(), true );
    mDevLock[dn] = false;
}

void TTr::Task( union sigval obj )
{
    if(mod->prcSt) return;
    mod->prcSt = true;

    //> Check all output transports
    vector<string> ls;
    mod->outList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	try{ mod->outAt(ls[i_l]).at().check(); }
	catch(...){ }

    mod->prcSt = false;
}

void TTr::writeLine( int fd, const string &ln )
{
    string obuf = ln+"\r\n";
    for(unsigned wOff = 0, kz = 0; wOff != obuf.size(); wOff += kz)
	if((kz=write(fd,obuf.data()+wOff,obuf.size()-wOff)) <= 0)
	    throw TError(mod->nodePath().c_str(),_("Write line error."));
}

string TTr::expect( int fd, const string& expLst, int tm )
{
    char buf[255];
    string rez, stmp;
    int rl, kz;

    time_t st_exp = time(NULL);

    while( (time(NULL)-st_exp) < tm )
    {
	//rl = messIO(NULL,0,buf,sizeof(buf),0,true);
	fd_set rd_fd;
	struct timeval tv;

	do
	{
	    tv.tv_sec = tm-(time(NULL)-st_exp); tv.tv_usec = 0;
	    FD_ZERO(&rd_fd); FD_SET(fd,&rd_fd);
	    kz = select(fd+1,&rd_fd,NULL,NULL,&tv);
	}
	while( kz == -1 && errno == EINTR );
	if( kz == 0 )	continue;
	else if( kz < 0 ) throw TError(mod->nodePath().c_str(),_("Read from serial error."));
	else if( FD_ISSET(fd,&rd_fd) )
	{
	    rl = read(fd,buf,sizeof(buf));
	    rez.append(buf,rl);
	    for( int off = 0; (stmp=TSYS::strParse(expLst,0,"\n",&off)).size(); )
		if( rez.find(stmp) != string::npos ) return stmp;
	}
    }

    return "";
}

//************************************************
//* TTrIn                                        *
//************************************************
TTrIn::TTrIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), mAPrms(cfg("A_PRMS").getSd()), fd(-1), endrun(false), trIn(0), trOut(0), tmMax(0),
    mMdmTm(20), mMdmPreInit(0.5), mMdmPostInit(1), mMdmInitStr1("ATZ"), mMdmInitStr2(""), mMdmInitResp("OK"),
    mMdmRingReq("RING"), mMdmRingAnswer("ATA"), mMdmRingAnswerResp("CONNECT"),
    mMdmMode(false), mMdmDataMode(false)
{
    setAddr("/dev/ttyS0:19200:8E2");
    setTimings("6:320");
}

TTrIn::~TTrIn()
{
    try{ stop(); }catch(...){ }
}

void TTrIn::load_( )
{
    TTransportIn::load_();

    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(mAPrms);
	vl = prmNd.attr("TMS");		if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("MdmTm");	if(!vl.empty()) setMdmTm(atoi(vl.c_str()));
	vl = prmNd.attr("MdmPreInit");	if(!vl.empty()) setMdmPreInit(atof(vl.c_str()));
	vl = prmNd.attr("MdmPostInit");	if(!vl.empty()) setMdmPostInit(atof(vl.c_str()));
	vl = prmNd.attr("MdmInitStr1");	if(!vl.empty()) setMdmInitStr1(vl);
	vl = prmNd.attr("MdmInitStr2");	if(!vl.empty()) setMdmInitStr2(vl);
	vl = prmNd.attr("MdmInitResp");	if(!vl.empty()) setMdmInitResp(vl);
	vl = prmNd.attr("MdmRingReq");	if(!vl.empty()) setMdmRingReq(vl);
	vl = prmNd.attr("MdmRingAnswer");if(!vl.empty()) setMdmRingAnswer(vl);
	vl = prmNd.attr("MdmRingAnswerResp");	if(!vl.empty()) setMdmRingAnswerResp(vl);
    } catch(...){ }
}

void TTrIn::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("TMS",timings());
    prmNd.setAttr("MdmTm",TSYS::int2str(mdmTm()));
    prmNd.setAttr("MdmPreInit",TSYS::real2str(mdmPreInit()));
    prmNd.setAttr("MdmPostInit",TSYS::real2str(mdmPostInit()));
    prmNd.setAttr("MdmInitStr1",mdmInitStr1());
    prmNd.setAttr("MdmInitStr2",mdmInitStr2());
    prmNd.setAttr("MdmInitResp",mdmInitResp());
    prmNd.setAttr("MdmRingReq",mdmRingReq());
    prmNd.setAttr("MdmRingAnswer",mdmRingAnswer());
    prmNd.setAttr("MdmRingAnswerResp",mdmRingAnswerResp());
    mAPrms = prmNd.save(XMLNode::BrAllPast);

    TTransportIn::save_();
}

string TTrIn::getStatus( )
{
    string rez = TTransportIn::getStatus( );

    if(startStat())
	rez += TSYS::strMess(_("Traffic in %s, out %s. Maximum char timeout %.4g ms."),
	    TSYS::cpct2str(trIn).c_str(),TSYS::cpct2str(trOut).c_str(),tmMax);

    return rez;
}

void TTrIn::setAddr( const string &iaddr )
{
    TTransportIn::setAddr(iaddr);

    //> Times adjust
    int speed = atoi(TSYS::strSepParse(iaddr,1,':').c_str());
    if(speed)	setTimings(TSYS::real2str(11e4/(float)speed,2,'f')+":"+TSYS::int2str((512*11*1000)/speed));

    if(startStat())	stop();
}

void TTrIn::setTimings( const string &vl )
{
    double wCharTm = vmax(0.01,vmin(1e3,atof(TSYS::strSepParse(vl,0,':').c_str())));
    int wFrTm = vmax(1,vmin(10000,atoi(TSYS::strSepParse(vl,1,':').c_str())));
    mTimings = TSYS::strMess("%g:%d",wCharTm,wFrTm);

    modif();
}

void TTrIn::connect( )
{
    try
    {
	//> Open and setup device
	//>> Serial port open
	mDevPort = TSYS::strSepParse(addr(),0,':');
	//  O_NONBLOCK is used for prevent function open() hang on several USB->RS485 converters
	fd = open(mDevPort.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) throw TError(nodePath().c_str(),_("Serial port '%s' open error: %s."),mDevPort.c_str(),strerror(errno));
	//>> Set serial port parameters
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag |= (CREAD|CLOCAL);
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 0;            ///< blocking read until 0 character arrives*/
	//>> Set speed
	string speed = TSYS::strNoSpace(TSYS::strSepParse(addr(),1,':').c_str());
	if(!speed.empty())
	{
	    speed_t tspd = B9600;
	    switch(atoi(speed.c_str()))
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
		case 115200:	tspd = B115200;	break;
		case 230400:	tspd = B230400;	break;
		case 460800:	tspd = B460800;	break;
		case 500000:	tspd = B500000;	break;
		case 576000:	tspd = B576000;	break;
		case 921600:	tspd = B921600;	break;
		default: throw TError(nodePath().c_str(),_("Speed '%s' error."),speed.c_str());
	    }
	    cfsetispeed(&tio, tspd);
	    cfsetospeed(&tio, tspd);
	}
	//>> Set asynchronous data format
	string format = TSYS::strNoSpace(TSYS::strNoSpace(TSYS::strSepParse(addr(),2,':')));
	if(!format.empty())
	{
	    if(format.size() != 3) throw TError(nodePath().c_str(),_("Asynchronous data format '%s' error."),format.c_str());
	    //>>> Set byte length
	    int len =  format[0]-'0';
	    if(len < 5 || len > 8) throw TError(nodePath().c_str(),_("Char length '%d' error."),len);
	    tio.c_cflag &= ~CSIZE;
	    switch(len)
	    {
		case 5:	tio.c_cflag |= CS5;	break;
		case 6:	tio.c_cflag |= CS6;	break;
		case 7:	tio.c_cflag |= CS7;	break;
		case 8:	tio.c_cflag |= CS8;	break;
	    }
	    //>>> Set parity
	    char parity = tolower(format[1]);
	    switch(parity)
	    {
		case 'e': tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD;break;
		case 'o': tio.c_cflag |= PARENB; tio.c_cflag |= PARODD;	break;
		case 'n': tio.c_cflag &= ~PARENB;			break;
		default: throw TError(nodePath().c_str(),_("Parity checking mode '%c' error."),parity);
	    }
	    //>>> Set stop bits number
 	    int stopbt = format[2]-'0';
	    if(stopbt == 1) tio.c_cflag &= ~CSTOPB;
	    else if(stopbt == 2) tio.c_cflag |= CSTOPB;
	    else throw TError(nodePath().c_str(),_("Stop bits '%d' error."),stopbt);
	}

	//>> Set flow control
	string fc = TSYS::strNoSpace(TSYS::strSepParse(addr(),3,':'));
	if(!fc.empty())
	    switch(tolower(fc[0]))
	    {
		case 'h': tio.c_cflag |= CRTSCTS;	break;
		case 's': tio.c_iflag |= (IXON|IXOFF|IXANY);	break;
	    }

	//>> Set port's data
	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &tio);

	//> Modem init
	mMdmMode = atoi(TSYS::strSepParse(addr(),4,':').c_str());
	if(mMdmMode)
	{
	    mMdmDataMode = false;
	    //>> Send init 1 string
	    if(!mdmInitStr1().empty())
	    {
		if(mdmPreInit() > 0)
		{
		    usleep((int)(mdmPreInit()*0.5e6));
		    TTr::writeLine(fd,"");
		    usleep((int)(mdmPreInit()*0.5e6));
		}
		TTr::writeLine(fd,mdmInitStr1());
		if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
		    throw TError(nodePath().c_str(),_("No response to init request '%s'."),mdmInitStr1().c_str());
		usleep((int)(mdmPostInit()*1e6));
	    }
	    //>> Send init 2 string
	    if(!mdmInitStr2().empty())
	    {
		if(mdmPreInit() > 0)
		{
		    usleep( (int)(mdmPreInit()*0.5e6) );
		    TTr::writeLine(fd,"");
		    usleep( (int)(mdmPreInit()*0.5e6) );
		}
		TTr::writeLine(fd,mdmInitStr2());
		if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
		    throw TError(nodePath().c_str(),_("No response to init request '%s'."),mdmInitStr2().c_str());
		usleep((int)(mdmPostInit()*1e6));
	    }
	}
    }
    catch(TError err)
    {
	if( fd >= 0 ) { close(fd); fd = -1; }
	throw;
    }
}

void TTrIn::start()
{
    if( run_st ) return;

    //> Status clear
    trIn = trOut = tmMax = 0;

    connect( );

    //> Start listen task
    SYS->taskCreate( nodePath('.',true), 0, Task, this, &run_st );
}

void TTrIn::stop()
{
    if( !run_st ) return;

    if( mMdmMode && mMdmDataMode ) mod->devUnLock(mDevPort);

    mMdmMode = mMdmDataMode = false;

    //> Status clear
    trIn = trOut = tmMax = 0;

    SYS->taskDestroy( nodePath('.',true), &run_st, &endrun );

    if( fd >= 0 ) close(fd);
    fd = -1;
}

void *TTrIn::Task( void *tr_in )
{
    TTrIn *tr = (TTrIn*)tr_in;

    tr->run_st	= true;
    tr->endrun	= false;

    AutoHD<TProtocolIn> prot_in;
    struct timeval tv;
    int r_len = 0;
    string req, answ;
    char buf[1000];
    fd_set fdset;

    double wCharTm = atof(TSYS::strSepParse(tr->timings(),0,':').c_str());
    int wFrTm = 1000*atoi(TSYS::strSepParse(tr->timings(),1,':').c_str());
    long long stFrTm = 0, tmW = 0, tmTmp1;

    fcntl( tr->fd, F_SETFL, 0 );

    while( !tr->endrun )
    {
	//>> Char timeout
	while(true)
	{
	    tv.tv_sec = 0; tv.tv_usec = (int)(1.5e3*wCharTm);
	    FD_ZERO( &fdset ); FD_SET( tr->fd, &fdset );

	    if( select( tr->fd+1, &fdset, NULL, NULL, &tv ) <= 0 )
	    {
		if( tr->endrun || !req.empty() )	break;
		continue;
	    }
	    r_len = read( tr->fd, buf, sizeof(buf));
	    if( r_len <= 0 ) break;

	    //>> Requests statistic
	    tmTmp1 = TSYS::curTime();
	    if( req.empty() ) stFrTm = tmW = tmTmp1;
	    if( tmW ) tr->tmMax = vmax(tr->tmMax,1e-3*(tmTmp1-tmW));
	    tmW = tmTmp1;

	    req += string(buf,r_len);
	    if( (TSYS::curTime()-stFrTm) > wFrTm )	break;
	}
	if( tr->endrun || req.empty() )
	{
	    if( r_len == 0 )
	    {
		close(tr->fd); tr->fd = -1;
		if( tr->mMdmMode && tr->mMdmDataMode )
		{
		    //>> Reconnect try after hung up by remote agent
		    mod->devUnLock(tr->mDevPort);
		    sleep(10);
		    try{ tr->connect(); } catch(TError err) { break; }
		    continue;
		}
	    }
	    break;
	}

	tr->trIn += req.size();

#if OSC_DEBUG >= 5
	mess_debug( nodePath().c_str(), _("Serial received message <%d>."), req.size() );
#endif

	//> Check for device lock and RING request from modem
	if( tr->mMdmMode && !tr->mMdmDataMode )
	{
	    if( mod->devLock(tr->mDevPort,true) ) continue;
	    if( req.find(tr->mdmRingReq()) != string::npos )
	    {
		if( !mod->devLock(tr->mDevPort) ) continue;
		//>> Send ring answer
		TTr::writeLine(tr->fd,tr->mdmRingAnswer());
		if( TTr::expect(tr->fd,tr->mdmRingAnswerResp(),tr->mdmTm()).empty() ) continue;
		tr->mMdmDataMode = true;

		req = "";
		continue;
	    }
	}
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
	    mess_debug(nodePath().c_str(), _("Serial replied message <%d>."), answ.size());
#endif
	    for(unsigned wOff = 0, wL = 1; wOff != answ.size() && wL > 0; wOff += wL)
	    {
		wL = write(tr->fd, answ.data()+wOff, answ.size()-wOff);
		tr->trOut += wL;
	    }
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
    if(opt->name() == "info")
    {
	TTransportIn::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Serial transport has address format: \"[dev]:[speed]:[format]:[fc]:[mdm]\". Where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600);\n"
	    "    format - asynchronous data format '<size><parity><stop>' (8N1, 7E1, 5O2);\n"
	    "    fc - flow control: 'h' - hardware (CRTSCTS), 's' - software (IXON|IXOFF);\n"
	    "    mdm - modem mode, listen for 'RING'."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"[symbol]:[frm]\". Where:\n"
	    "    symbol - one symbol maximum time, used for frame end detection, in ms;\n"
	    "    frm - maximum frame length, in ms."));
	if(atoi(TSYS::strParse(addr(),4,":").c_str()) && ctrMkNode("area",opt,-1,"/mod",_("Modem"),R_R_R_,"root",STR_ID))
	{
	    ctrMkNode("fld",opt,-1,"/mod/tm",_("Timeout (sec)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/preInitDl",_("Pre-init delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/postInitDl",_("Post-init delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/initStr1",_("Initialization string 1"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initStr2",_("Initialization string 2"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initResp",_("Init response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringReq",_("Ring request"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringAnswer",_("Ring answer"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringAnswerResp",_("Ring answer response"),RWRWR_,"root",STR_ID,1,"tp","str");
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/TMS")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else if(a_path == "/mod/tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::int2str(mdmTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/mod/preInitDl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::real2str(mdmPreInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPreInit(atof(opt->text().c_str()));
    }
    else if(a_path == "/mod/postInitDl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::real2str(mdmPostInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPostInit(atof(opt->text().c_str()));
    }
    else if(a_path == "/mod/initStr1")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr1());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr1(opt->text());
    }
    else if(a_path == "/mod/initStr2")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr2());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr2(opt->text());
    }
    else if(a_path == "/mod/initResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitResp(opt->text());
    }
    else if(a_path == "/mod/ringReq")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmRingReq());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmRingReq(opt->text());
    }
    else if(a_path == "/mod/ringAnswer")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmRingAnswer());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmRingAnswer(opt->text());
    }
    else if(a_path == "/mod/ringAnswerResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmRingAnswerResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmRingAnswerResp(opt->text());
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TTrOut                                   *
//************************************************
TTrOut::TTrOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), mAPrms(cfg("A_PRMS").getSd()), fd(-1), mLstReqTm(0), trIn(0), trOut(0),
    mMdmTm(30), mMdmLifeTime(30), mMdmPreInit(0.5), mMdmPostInit(1), mMdmInitStr1("ATZ"), mMdmInitStr2(""), mMdmInitResp("OK"),
    mMdmDialStr("ATDT"), mMdmCnctResp("CONNECT"), mMdmBusyResp("BUSY"), mMdmNoCarResp("NO CARRIER"), mMdmNoDialToneResp("NO DIALTONE"),
    mMdmHangUp("+++ATH"), mMdmHangUpResp("OK"), mMdmMode(false), mMdmDataMode(false)
{
    setAddr("/dev/ttyS0:19200:8E2");
    setTimings("640:6");
}

TTrOut::~TTrOut()
{
    if( startStat() )	stop();
}

void TTrOut::load_( )
{
    TTransportOut::load_();

    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(mAPrms);
	vl = prmNd.attr("TMS");		if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("MdmTm");	if(!vl.empty()) setMdmTm(atoi(vl.c_str()));
	vl = prmNd.attr("MdmLifeTime");	if(!vl.empty()) setMdmLifeTime(atoi(vl.c_str()));
	vl = prmNd.attr("MdmPreInit");	if(!vl.empty()) setMdmPreInit(atof(vl.c_str()));
	vl = prmNd.attr("MdmPostInit");	if(!vl.empty()) setMdmPostInit(atof(vl.c_str()));
	vl = prmNd.attr("MdmInitStr1");	if(!vl.empty()) setMdmInitStr1(vl);
	vl = prmNd.attr("MdmInitStr2");	if(!vl.empty()) setMdmInitStr2(vl);
	vl = prmNd.attr("MdmInitResp");	if(!vl.empty()) setMdmInitResp(vl);
	vl = prmNd.attr("MdmDialStr");	if(!vl.empty()) setMdmDialStr(vl);
	vl = prmNd.attr("MdmCnctResp");	if(!vl.empty()) setMdmCnctResp(vl);
	vl = prmNd.attr("MdmBusyResp");	if(!vl.empty()) setMdmBusyResp(vl);
	vl = prmNd.attr("MdmNoCarResp");if(!vl.empty()) setMdmNoCarResp(vl);
	vl = prmNd.attr("MdmNoDialToneResp");	if(!vl.empty()) setMdmNoDialToneResp(vl);
	vl = prmNd.attr("MdmHangUp");	if(!vl.empty()) setMdmHangUp(vl);
	vl = prmNd.attr("MdmHangUpResp");if(!vl.empty()) setMdmHangUpResp(vl);
    } catch(...){ }
}

void TTrOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("TMS",timings());
    prmNd.setAttr("MdmTm",TSYS::int2str(mdmTm()));
    prmNd.setAttr("MdmLifeTime",TSYS::int2str(mdmLifeTime()));
    prmNd.setAttr("MdmPreInit",TSYS::real2str(mdmPreInit()));
    prmNd.setAttr("MdmPostInit",TSYS::real2str(mdmPostInit()));
    prmNd.setAttr("MdmInitStr1",mdmInitStr1());
    prmNd.setAttr("MdmInitStr2",mdmInitStr2());
    prmNd.setAttr("MdmInitResp",mdmInitResp());
    prmNd.setAttr("MdmDialStr",mdmDialStr());
    prmNd.setAttr("MdmCnctResp",mdmCnctResp());
    prmNd.setAttr("MdmBusyResp",mdmBusyResp());
    prmNd.setAttr("MdmNoCarResp",mdmNoCarResp());
    prmNd.setAttr("MdmNoDialToneResp",mdmNoDialToneResp());
    prmNd.setAttr("MdmHangUp",mdmHangUp());
    prmNd.setAttr("MdmHangUpResp",mdmHangUpResp());
    mAPrms = prmNd.save(XMLNode::BrAllPast);

    TTransportOut::save_();
}

string TTrOut::getStatus( )
{
    string rez = TTransportOut::getStatus( );

    if( startStat() )
	rez += TSYS::strMess(_("Traffic in %s, out %s. "),TSYS::cpct2str(trIn).c_str(),TSYS::cpct2str(trOut).c_str());

    return rez;
}

void TTrOut::setAddr( const string &iaddr )
{
    TTransportOut::setAddr(iaddr);

    //> Times adjust
    int speed = atoi(TSYS::strSepParse(iaddr,1,':').c_str());
    if(TSYS::strSepParse(addr(),4,':').size()) setTimings("5000:1000");
    else if(speed) setTimings(TSYS::int2str((1024*11*1000)/speed)+":"+TSYS::real2str(11e4/(float)speed,2,'f'));

    if(startStat())	stop();
}

void TTrOut::setTimings( const string &vl )
{
    int wReqTm = vmax(1,vmin(10000,atoi(TSYS::strSepParse(vl,0,':').c_str())));
    double wCharTm = vmax(0.01,vmin(1e3,atof(TSYS::strSepParse(vl,1,':').c_str())));
    mTimings = TSYS::strMess("%d:%g",wReqTm,wCharTm);

    modif();
}

void TTrOut::start( )
{
    if(run_st) return;

    //> Status clear
    trIn = trOut = 0;
    bool isLock = false;

    try
    {
	//> Open and setup device
	mDevPort = TSYS::strSepParse(addr(),0,':');
	//>> Lock device for all serial transports
	if(!(isLock=mod->devLock(mDevPort))) throw TError(nodePath().c_str(),_("Device '%s' is used now."),mDevPort.c_str());

	//>> Serial port open
	//  O_NONBLOCK is used for prevent function open() hang on several USB->RS485 converters
	fd = open(mDevPort.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) throw TError(nodePath().c_str(),_("Serial port '%s' open error: %s."),mDevPort.c_str(),strerror(errno));

	//>> Set serial port parameters
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag |= (CREAD|CLOCAL);
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 0;            ///< blocking read until 0 character arrives

	//>> Set speed
	string speed = TSYS::strNoSpace(TSYS::strSepParse(addr(),1,':').c_str());
	if(!speed.empty())
	{
	    speed_t tspd = B9600;
	    switch(atoi(speed.c_str()))
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
		case 115200:	tspd = B115200;	break;
		case 230400:	tspd = B230400;	break;
		case 460800:	tspd = B460800;	break;
		case 500000:	tspd = B500000;	break;
		case 576000:	tspd = B576000;	break;
		case 921600:	tspd = B921600;	break;
		default: throw TError(nodePath().c_str(),_("Speed '%s' error."),speed.c_str());
	    }
	    cfsetispeed(&tio, tspd);
	    cfsetospeed(&tio, tspd);
	}

	//>> Set asynchronous data format
	string format = TSYS::strNoSpace(TSYS::strNoSpace(TSYS::strSepParse(addr(),2,':')));
	if(!format.empty())
	{
	    if(format.size() != 3) throw TError(nodePath().c_str(),_("Asynchronous data format '%s' error."),format.c_str());

	    //>>> Set byte length
	    int len =  format[0]-'0';
	    if(len < 5 || len > 8) throw TError(nodePath().c_str(),_("Char length '%d' error."),len);
	    tio.c_cflag &= ~CSIZE;
	    switch(len)
	    {
		case 5:	tio.c_cflag |= CS5;	break;
		case 6:	tio.c_cflag |= CS6;	break;
		case 7:	tio.c_cflag |= CS7;	break;
		case 8:	tio.c_cflag |= CS8;	break;
	    }

	    //>>> Set parity
	    char parity = tolower(format[1]);
	    switch(parity)
	    {
		case 'e': tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD;break;
		case 'o': tio.c_cflag |= PARENB; tio.c_cflag |= PARODD;	break;
		case 'n': tio.c_cflag &= ~PARENB;			break;
		default: throw TError(nodePath().c_str(),_("Parity checking mode '%c' error."),parity);
	    }

	    //>>> Set stop bits number
	    int stopbt = format[2]-'0';
	    if(stopbt == 1) tio.c_cflag &= ~CSTOPB;
	    else if(stopbt == 2) tio.c_cflag |= CSTOPB;
	    else throw TError(nodePath().c_str(),_("Stop bits '%d' error."),stopbt);
	}

	//>> Set flow control
	string fc = TSYS::strNoSpace(TSYS::strSepParse(addr(),3,':'));
	if(!fc.empty())
	    switch(tolower(fc[0]))
	    {
		case 'h': tio.c_cflag |= CRTSCTS;		break;
		case 's': tio.c_iflag |= (IXON|IXOFF|IXANY);	break;
	    }

	//>> Set port's data
	tcflush( fd, TCIOFLUSH );
	tcsetattr( fd, TCSANOW, &tio );

	//> Modem connection establish
	string telNumb = TSYS::strNoSpace(TSYS::strSepParse(addr(),4,':'));
	if(!telNumb.empty())
	{
	    //>> Resource to transfer function alloc
	    ResAlloc res(nodeRes(), true);
	    run_st = mMdmMode = true;

	    //>> Send init 1 string
	    if(!mdmInitStr1().empty())
	    {
		if(mdmPreInit() > 0)
		{
		    usleep((int)(mdmPreInit()*0.5e6));
		    TTr::writeLine(fd,"");
		    usleep((int)(mdmPreInit()*0.5e6));
		}
		TTr::writeLine(fd,mdmInitStr1());
		if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
		    throw TError(nodePath().c_str(),_("No response to init request '%s'."),mdmInitStr1().c_str());
		usleep((int)(mdmPostInit()*1e6));
	    }
	    //>> Send init 2 string
	    if(!mdmInitStr2().empty())
	    {
		if(mdmPreInit() > 0)
		{
		    usleep((int)(mdmPreInit()*0.5e6));
		    TTr::writeLine(fd,"");
		    usleep((int)(mdmPreInit()*0.5e6));
		}
		TTr::writeLine(fd,mdmInitStr2());
		if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
		    throw TError(nodePath().c_str(),_("No response to init request '%s'."),mdmInitStr2().c_str());
		usleep((int)(mdmPostInit()*1e6));
	    }
	    //>> Dial number and connection wait
	    string rez;
	    telNumb = mdmDialStr()+telNumb;
	    TTr::writeLine(fd,telNumb);
	    if((rez=TTr::expect(fd,mdmCnctResp()+"\n"+mdmBusyResp()+"\n"+mdmNoCarResp()+"\n"+mdmNoDialToneResp(),mdmTm())) != mdmCnctResp())
	    {
		if(rez == mdmBusyResp())		throw TError(nodePath().c_str(),_("Modem busy"));
		else if(rez == mdmNoCarResp())		throw TError(nodePath().c_str(),_("Modem no carrier"));
		else if(rez == mdmNoDialToneResp())	throw TError(nodePath().c_str(),_("Modem no dial tone"));
		else 					throw TError(nodePath().c_str(),_("Modem no connected"));
	    }
	    mLstReqTm = TSYS::curTime();
	    mMdmDataMode = true;
	}
    }
    catch(TError err)
    {
	//> HangUp
	if(mMdmMode) TTr::writeLine(fd,mdmHangUp());

	if(fd >= 0) { close(fd); fd = -1; }
	if(isLock) mod->devUnLock(mDevPort);
	run_st = mMdmMode = false;
	throw;
    }

    run_st = true;
}

void TTrOut::stop()
{
    if( !run_st ) return;

    ResAlloc res( nodeRes(), true );

    if( mMdmDataMode )
    {
	//> HangUp
	TTr::writeLine(fd,mdmHangUp());
	mMdmDataMode = false;
    }

    //> Status clear
    trIn = trOut = 0;

    //> Port close
    close(fd); fd = -1;

    //> Unlock device
    mod->devUnLock(mDevPort);

    run_st = mMdmMode = false;
}

void TTrOut::check( )
{
    bool reRs = false;
    bool toStop = (mMdmMode && mMdmDataMode && (reRs=nodeRes().resTryW()) && (TSYS::curTime()-mLstReqTm)/1000000 > mdmLifeTime());
    if( reRs ) nodeRes().resRelease();
    if( toStop ) stop();
}

int TTrOut::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time, bool noRes )
{
    int blen = 0, off = 0, kz;

    if( !noRes ) ResAlloc res( nodeRes(), true );

    if( !run_st ) throw TError(nodePath().c_str(),_("Transport is not started!"));

    int wReqTm = atoi(TSYS::strSepParse(timings(),0,':',&off).c_str());
    wReqTm = time ? time : wReqTm;
    double wCharTm = atof(TSYS::strSepParse(timings(),0,':',&off).c_str());

    long long tmW = TSYS::curTime();

    //> Write request
    if( obuf && len_ob > 0 )
    {
	tcflush( fd, TCIOFLUSH );
	if( (tmW-mLstReqTm) < (4000*wCharTm) ) kz = usleep( (int)((4000*wCharTm)-(tmW-mLstReqTm)) );
	for( int wOff = 0, kz = 0; wOff != len_ob; wOff += kz )
	{
	    kz = write(fd,obuf+wOff,len_ob-wOff);
	    if( kz <= 0 ) { mLstReqTm = TSYS::curTime(); stop(); throw TError(nodePath().c_str(),_("Writing request error.")); }
	}
	trOut += len_ob;
    }

    //> Read reply
    if( ibuf != NULL && len_ib > 0 )
    {
	fd_set rd_fd;
	struct timeval tv;

	do
	{
	    if( obuf && len_ob > 0 ) { tv.tv_sec  = wReqTm/1000; tv.tv_usec = 1000*(wReqTm%1000); }
	    else { tv.tv_sec = (int)(1.5e-3*wCharTm); tv.tv_usec = (int)(1.5e3*wCharTm)%1000000; }
	    FD_ZERO(&rd_fd); FD_SET(fd,&rd_fd);
	    kz = select(fd+1,&rd_fd,NULL,NULL,&tv);
	}
	while( kz == -1 && errno == EINTR );
	if( kz == 0 )	{ mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(),_("Timeouted!")); }
	else if( kz < 0){ mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(),_("Serial error!")); }
	else if( FD_ISSET(fd, &rd_fd) )
	{
	    blen = read(fd,ibuf,len_ib);
	    trIn += blen;
	}
    }
    mLstReqTm = TSYS::curTime();

    return blen;
}

void TTrOut::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTransportOut::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Serial transport has address format: \"[dev]:[speed]:[format]:[fc]:[modTel]\". Where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600 );\n"
	    "    format - asynchronous data format '<size><parity><stop>' (8N1, 7E1, 5O2);\n"
	    "    fc - flow control: 'h' - hardware (CRTSCTS), 's' - software (IXON|IXOFF);\n"
	    "    modTel - modem telephone, the field presence do switch transport to work with modem mode."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"[conn]:[symbol]\". Where:\n"
	    "    conn - maximum time for connection respond wait, in ms;\n"
	    "    symbol - one symbol maximum time, used for frame end detection, in ms."));
	if(TSYS::strParse(addr(),4,":").size() && ctrMkNode("area",opt,-1,"/mod",_("Modem"),R_R_R_,"root",STR_ID))
	{
	    ctrMkNode("fld",opt,-1,"/mod/tm",_("Timeout (sec)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/lifeTm",_("Life time (sec)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/preInitDl",_("Pre-init delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/postInitDl",_("Post-init delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/initStr1",_("Initialization string 1"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initStr2",_("Initialization string 2"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initResp",_("Init response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/dialStr",_("Dial string"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/cnctResp",_("Connect response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/busyResp",_("Busy response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/noCarResp",_("No carrier response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/noDialToneResp",_("No dial tone response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/hangUp",_("Hangup string"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/hangUpResp",_("Hangup response"),RWRWR_,"root",STR_ID,1,"tp","str");
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/TMS")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else if(a_path == "/mod/tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::int2str(mdmTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/mod/lifeTm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::int2str(mdmLifeTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmLifeTime(atoi(opt->text().c_str()));
    }
    else if(a_path == "/mod/preInitDl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::real2str(mdmPreInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPreInit(atof(opt->text().c_str()));
    }
    else if(a_path == "/mod/postInitDl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(TSYS::real2str(mdmPostInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPostInit(atof(opt->text().c_str()));
    }
    else if(a_path == "/mod/initStr1")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr1());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr1(opt->text());
    }
    else if(a_path == "/mod/initStr2")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr2());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr2(opt->text());
    }
    else if(a_path == "/mod/initResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitResp(opt->text());
    }
    else if(a_path == "/mod/dialStr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmDialStr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmDialStr(opt->text());
    }
    else if(a_path == "/mod/cnctResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmCnctResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmCnctResp(opt->text());
    }
    else if(a_path == "/mod/busyResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmBusyResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmBusyResp(opt->text());
    }
    else if(a_path == "/mod/noCarResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmNoCarResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmNoCarResp(opt->text());
    }
    else if(a_path == "/mod/noDialToneResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmNoDialToneResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmNoDialToneResp(opt->text());
    }
    else if(a_path == "/mod/hangUp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmHangUp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmHangUp(opt->text());
    }
    else if(a_path == "/mod/hangUpResp")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmHangUpResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmHangUpResp(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

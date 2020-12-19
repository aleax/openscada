
//OpenSCADA module Transport.Serial file: mod_serial.cpp
/***************************************************************************
 *   Copyright (C) 2009-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#ifdef HAVE_LINUX_SERIAL_H
# include <linux/serial.h>
#endif

#ifdef HAVE_LINUX_I2C_DEV_H
# include <linux/i2c-dev.h>
#endif

#ifdef HAVE_LINUX_SPI_SPIDEV_H
# include <linux/spi/spidev.h>
#endif

//************************************************
//* Modul info!                                  *
#define MOD_ID		"Serial"
#define MOD_NAME	_("Serial interfaces")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"2.5.3"
#define AUTHORS		_("Roman Savochenko, Maxim Kochetkov (2016)")
#define DESCRIPTION	_("Provides transport based on the serial interfaces.\
 It is used for data exchanging via the serial interfaces of the type RS232, RS485, GSM and similar.")
#define LICENSE		"GPL2"
//************************************************

Serial::TTr *Serial::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt tr_Serial_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *tr_Serial_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new Serial::TTr(source);
	return NULL;
    }
}

using namespace Serial;

//************************************************
//* TTr						 *
//************************************************
TTr::TTr( string name ) : TTypeTransport(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTr::~TTr( )
{
    try { modStop(); } catch(...) { }
}

void TTr::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	owner().inEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
	owner().outEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
    }
}

AutoHD<TTrIn> TTr::inAt( const string &name )	{ return TTypeTransport::inAt(name); }

AutoHD<TTrOut> TTr::outAt( const string &name )	{ return TTypeTransport::outAt(name); }

void TTr::load_( )
{
    TTypeTransport::load_();

    //Load parameters from command line

}

TTransportIn *TTr::In( const string &name, const string &idb )	{ return new TTrIn(name,idb,&owner().inEl()); }

TTransportOut *TTr::Out( const string &name, const string &idb ){ return new TTrOut(name,idb,&owner().outEl()); }

string TTr::outAddrHelp( )
{
    return _("The serial transport has the address format \"{dev}:{speed}:{format}[:{opts}[:{modTel}]]\", where:\n"
	"    dev - serial device address (/dev/ttyS0);\n"
	"    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	"                          230400, 460800, 500000, 576000 or 921600);\n"
	"    format - asynchronous data format '{size}{parity}{stop}' (8N1, 7E1, 5O2);\n"
	"    opts - different options, mostly for flow control, separated by ',':\n"
	"      '[-]h' - hardware (CRTSCTS);\n"
	"      '[-]s' - software (IXON|IXOFF);\n"
	"      'rts' - using of the RTS signal for transferring(false) and checking for echo, for raw RS-485;\n"
	"      'rts1' - using of the RTS signal for transferring(true) and checking for echo, for raw RS-485;\n"
	"      'rtsne' - using of the RTS signal for transferring(false) and without checking for echo, for raw RS-485;\n"
	"      'rts1ne' - using of the RTS signal for transferring(true) and without checking for echo, for raw RS-485;\n"
	"      '[-]RS485' - using RS-485 mode, through TIOCSRS485.\n"
	"    modTel - modem telephone, the field presence do switch transport to work with modem mode.");
}

bool TTr::devLock( const string &dn, bool check )
{
    MtxAlloc res(dataRes(), true);

    if(check) return mDevLock[dn];
    if(mDevLock[dn]) return false;
    mDevLock[dn] = true;
    return true;
}

void TTr::devUnLock( const string &dn )
{
    MtxAlloc res(dataRes(), true);
    mDevLock[dn] = false;
}

void TTr::perSYSCall( unsigned int cnt )
{
    TTypeTransport::perSYSCall(cnt);

    //Check all output transports
    vector<string> ls;
    outList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++)
	try{ outAt(ls[iL]).at().check(); }
	catch(TError &err) { }
}

void TTr::writeLine( int fd, const string &ln, bool noNewLn )
{
    string obuf = ln + (noNewLn?"":"\x0D\x0A");
    for(unsigned wOff = 0, kz = 0; wOff != obuf.size(); wOff += kz)
	if((kz=write(fd,obuf.data()+wOff,obuf.size()-wOff)) <= 0)
	    throw TError(mod->nodePath().c_str(),_("Error writing a line."));
    mess_debug(mod->nodePath().c_str(), _("Sent to the modem %d: '%s'."), fd, ln.c_str());
}

string TTr::expect( int fd, const string& expLst, int tm )
{
    char buf[255];
    string rez, stmp;
    int rl, kz;

    time_t st_exp = time(NULL);

    while((time(NULL)-st_exp) < tm) {
	//rl = messIO(NULL,0,buf,sizeof(buf),0,true);
	fd_set rd_fd;
	struct timeval tv;

	tv.tv_sec = tm-(time(NULL)-st_exp); tv.tv_usec = 0;
	FD_ZERO(&rd_fd); FD_SET(fd,&rd_fd);
	kz = select(fd+1,&rd_fd,NULL,NULL,&tv);
	if(kz == 0)	continue;
	else if(kz < 0) throw TError(mod->nodePath().c_str(),_("Error reading from the serial interfaces."));
	else if(FD_ISSET(fd,&rd_fd)) {
	    rl = read(fd, buf, sizeof(buf));
	    rez.append(buf,rl);
	    for(int off = 0; (stmp=TSYS::strParse(expLst,0,"\n",&off)).size(); )
		if(rez.find(stmp) != string::npos) {
		    mess_debug(mod->nodePath().c_str(), _("Received from the modem %d: '%s'."), fd, stmp.c_str());
		    return stmp;
		}
	}
    }

    if(rez.empty()) mess_debug(mod->nodePath().c_str(), _("No response from the modem %d."), fd);
    else mess_debug(mod->nodePath().c_str(), _("No expected response, but receiving from the modem %d: '%s'."), fd, rez.c_str());

    return "";
}

//************************************************
//* TTrIn					 *
//************************************************
TTrIn::TTrIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), fd(-1), endrun(false), trIn(0), trOut(0), tmMax(0), prcTm(0), prcTmMax(0), mTaskPrior(0),
    mMdmTm(20), mMdmPreInit(0.5), mMdmPostInit(1), mMdmInitStr1("ATZ"), mMdmInitStr2(""), mMdmInitResp("OK"),
    mMdmRingReq("RING"), mMdmRingAnswer("ATA"), mMdmRingAnswerResp("CONNECT"),
    mMdmMode(false), mMdmDataMode(false), mRTSfc(false), mRTSlvl(false), mRTSEcho(false)
{
    setAddr("/dev/ttyS0:19200:8E2");
    setTimings("6:320");
}

TTrIn::~TTrIn( )	{ }

void TTrIn::load_( )
{
    //TTransportIn::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("TMS");		if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("TaskPrior");	if(!vl.empty()) setTaskPrior(s2i(vl));
	vl = prmNd.attr("MdmTm");	if(!vl.empty()) setMdmTm(s2i(vl));
	vl = prmNd.attr("MdmPreInit");	if(!vl.empty()) setMdmPreInit(s2r(vl));
	vl = prmNd.attr("MdmPostInit");	if(!vl.empty()) setMdmPostInit(s2r(vl));
	vl = prmNd.attr("MdmInitStr1");	if(!vl.empty()) setMdmInitStr1(vl);
	vl = prmNd.attr("MdmInitStr2");	if(!vl.empty()) setMdmInitStr2(vl);
	vl = prmNd.attr("MdmInitResp");	if(!vl.empty()) setMdmInitResp(vl);
	vl = prmNd.attr("MdmRingReq");	if(!vl.empty()) setMdmRingReq(vl);
	vl = prmNd.attr("MdmRingAnswer");if(!vl.empty()) setMdmRingAnswer(vl);
	vl = prmNd.attr("MdmRingAnswerResp");	if(!vl.empty()) setMdmRingAnswerResp(vl);
    } catch(...) { }
}

void TTrIn::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("TMS", timings());
    prmNd.setAttr("TaskPrior", i2s(taskPrior()));
    prmNd.setAttr("MdmTm", i2s(mdmTm()));
    prmNd.setAttr("MdmPreInit", r2s(mdmPreInit()));
    prmNd.setAttr("MdmPostInit", r2s(mdmPostInit()));
    prmNd.setAttr("MdmInitStr1",mdmInitStr1());
    prmNd.setAttr("MdmInitStr2",mdmInitStr2());
    prmNd.setAttr("MdmInitResp",mdmInitResp());
    prmNd.setAttr("MdmRingReq",mdmRingReq());
    prmNd.setAttr("MdmRingAnswer",mdmRingAnswer());
    prmNd.setAttr("MdmRingAnswerResp",mdmRingAnswerResp());
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportIn::save_();
}

bool TTrIn::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR") {
	//Times adjust
	int speed = s2i(TSYS::strParse(co.getS(),1,":"));
	if(speed) {
	    float symbMlt = TRegExp("^.+\\/ttyS\\d+$").test(TSYS::strParse(co.getS(),0,":")) ? 1 : 3;
	    setTimings(r2s(symbMlt*11e4/(float)speed,2,'f')+":"+i2s((512*11*1000)/speed)+
		"::"+TSYS::strParse(timings(),3,":")+":"+TSYS::strParse(timings(),4,":"));
	}
    }

    return TTransportIn::cfgChange(co, pc);
}

string TTrIn::getStatus( )
{
    string rez = TTransportIn::getStatus( );

    if(startStat()) {
	rez += TSYS::strMess(_("Traffic in %s, out %s. Maximum char timeout %.4g ms. "),
	    TSYS::cpct2str(trIn).c_str(),TSYS::cpct2str(trOut).c_str(),tmMax);
	if(mess_lev() == TMess::Debug)
	    rez += TSYS::strMess(_("Processing time %s[%s]. "), tm2s(1e-6*prcTm).c_str(), tm2s(1e-6*prcTmMax).c_str());
    }

    return rez;
}

void TTrIn::setTimings( const string &vl )
{
    float wCharTm = vmax(0.01, vmin(1e3,s2r(TSYS::strParse(vl,0,":"))));
    int wFrTm = vmax(1, vmin(10000,s2i(TSYS::strParse(vl,1,":"))));
    mTimings = TSYS::strMess("%g:%d", wCharTm, wFrTm);

    float wRtsDel1 = vmax(0, vmin(10000,s2r(TSYS::strParse(vl,3,":"))));
    float wRtsDel2 = vmax(0, vmin(10000,s2r(TSYS::strParse(vl,4,":"))));
    if(wRtsDel1 || wRtsDel2) mTimings += TSYS::strMess("::%g:%g", wRtsDel1, wRtsDel2);

    modif();
}

void TTrIn::connect( )
{
    try {
	//Open and setup device
	// Serial port open
	mDevPort = TSYS::strParse(addr(), 0, ":");
	//  O_NONBLOCK is used for prevent function open() hang on several USB->RS485 converters
	fd = open(mDevPort.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) throw TError(nodePath().c_str(), _("Error the serial port '%s' %s: %s."), mDevPort.c_str(), "open", strerror(errno));
	// Set serial port parameters
	struct termios tio;
	if(tcgetattr(fd,&tio) < 0)
	    throw TError(nodePath().c_str(), _("Error the serial port '%s' %s: %s."), mDevPort.c_str(), "tcgetattr", strerror(errno));
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag |= (CREAD|CLOCAL);
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;		//< inter-character timer unused
	tio.c_cc[VMIN] = 0;		//< blocking read until 0 character arrives
	// Set speed
	string speed = sTrm(TSYS::strParse(addr(),1,":"));
	if(!speed.empty()) {
	    speed_t tspd = B9600;
	    switch(s2i(speed)) {
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
		default: throw TError(nodePath().c_str(),_("Error the speed '%s'."),speed.c_str());
	    }
	    cfsetispeed(&tio, tspd);
	    cfsetospeed(&tio, tspd);
	}
	// Set asynchronous data format
	string format = sTrm(TSYS::strParse(addr(),2,":"));
	if(!format.empty()) {
	    if(format.size() != 3) throw TError(nodePath().c_str(),_("Error the asynchronous data format '%s'."),format.c_str());
	    //  Set byte length
	    int len =  format[0]-'0';
	    if(len < 5 || len > 8) throw TError(nodePath().c_str(),_("Error char length %d."),len);
	    tio.c_cflag &= ~CSIZE;
	    switch(len) {
		case 5:	tio.c_cflag |= CS5;	break;
		case 6:	tio.c_cflag |= CS6;	break;
		case 7:	tio.c_cflag |= CS7;	break;
		case 8:	tio.c_cflag |= CS8;	break;
	    }
	    //  Set parity
	    char parity = tolower(format[1]);
	    switch(parity) {
		case 'e': tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD;break;
		case 'o': tio.c_cflag |= PARENB; tio.c_cflag |= PARODD;	break;
		case 'n': tio.c_cflag &= ~PARENB;			break;
		default: throw TError(nodePath().c_str(),_("Error the parity checking mode '%c'."),parity);
	    }
	    //  Set stop bits number
	    int stopbt = format[2]-'0';
	    if(stopbt == 1) tio.c_cflag &= ~CSTOPB;
	    else if(stopbt == 2) tio.c_cflag |= CSTOPB;
	    else throw TError(nodePath().c_str(),_("Error the stop bits %d."),stopbt);
	}

	// Set options
	mRTSfc = mRTSlvl = mRTSEcho = false;
	string opts = sTrm(TSYS::strParse(addr(),3,":")), opt;
	for(int off = 0; (opt=TSYS::strParse(opts,0,",",&off)).size(); ) {
	    if(strcasecmp(opt.c_str(),"h") == 0)	tio.c_cflag |= CRTSCTS;
	    else if(strcasecmp(opt.c_str(),"-h") == 0)	tio.c_cflag &= ~CRTSCTS;

	    if(strcasecmp(opt.c_str(),"s") == 0)	tio.c_iflag |= (IXON|IXOFF|IXANY);
	    else if(strcasecmp(opt.c_str(),"-s") == 0)	tio.c_iflag &= ~(IXON|IXOFF|IXANY);

	    if(strcasecmp(opt.c_str(),"rts") == 0)	{ mRTSfc = mRTSEcho = true; }
	    else if(strcasecmp(opt.c_str(),"rts1") == 0){ mRTSfc = mRTSlvl = mRTSEcho = true; }
	    else if(strcasecmp(opt.c_str(),"rts1ne") == 0) { mRTSfc = mRTSlvl = true; }
	    else if(strcasecmp(opt.c_str(),"rtsne") == 0)  { mRTSfc = true; }

#ifdef SER_RS485_ENABLED
#ifndef TIOCSRS485
#define TIOCSRS485      0x542f
#endif
	    // Standard RS-485 mode
	    if(strcasecmp(opt.c_str(),"rs485") == 0 || strcasecmp(opt.c_str(),"-rs485") == 0) {
		serial_rs485 rs485conf;
		memset(&rs485conf, 0, sizeof(serial_rs485));
		if(strcasecmp(opt.c_str(),"rs485") == 0) rs485conf.flags |= SER_RS485_ENABLED;
		ioctl(fd, TIOCSRS485, &rs485conf);
	    }
#endif
	}

	// Set port's data
	tcflush(fd, TCIOFLUSH);
	if(tcsetattr(fd,TCSANOW,&tio) < 0)
	    throw TError(nodePath().c_str(), _("Error the serial port '%s' %s: %s."), mDevPort.c_str(), "tcsetattr", strerror(errno));

	//Modem init
	mMdmMode = s2i(TSYS::strParse(addr(),4,":"));
	if(mMdmMode) {
	    mMdmDataMode = false;
	    // Send init 1 string
	    if(!mdmInitStr1().empty()) {
		if(mdmPreInit() > 0) {
		    TSYS::sysSleep(mdmPreInit()*0.5);
		    TTr::writeLine(fd,"");
		    TSYS::sysSleep(mdmPreInit()*0.5);
		}
		TTr::writeLine(fd,mdmInitStr1());
		if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
		    throw TError(nodePath().c_str(),_("No response to the initial request '%s'."),mdmInitStr1().c_str());
		TSYS::sysSleep(mdmPostInit());
	    }
	    // Send init 2 string
	    if(!mdmInitStr2().empty()) {
		if(mdmPreInit() > 0) {
		    TSYS::sysSleep(mdmPreInit()*0.5);
		    TTr::writeLine(fd,"");
		    TSYS::sysSleep(mdmPreInit()*0.5);
		}
		TTr::writeLine(fd,mdmInitStr2());
		if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
		    throw TError(nodePath().c_str(),_("No response to the initial request '%s'."),mdmInitStr2().c_str());
		TSYS::sysSleep(mdmPostInit());
	    }
	}
    } catch(TError &err) {
	if(fd >= 0) { close(fd); fd = -1; }
	throw;
    }
}

void TTrIn::start( )
{
    if(runSt) return;

    //Status clear
    trIn = trOut = prcTm = prcTmMax = 0;
    tmMax = 0;

    connect();

    //Start listen task
    SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);

    TTransportIn::start();

    if(logLen()) pushLogMess(_("Connected"));
}

void TTrIn::stop( )
{
    if(!runSt) return;

    if(mMdmMode && mMdmDataMode) mod->devUnLock(mDevPort);

    mMdmMode = mMdmDataMode = false;

    //Status clear
    trIn = trOut = 0;
    tmMax = 0;

    SYS->taskDestroy(nodePath('.',true), &endrun);

    if(fd >= 0) close(fd);
    fd = -1;

    TTransportIn::stop();

    if(logLen()) pushLogMess(_("Disconnected"));
}

void *TTrIn::Task( void *tr_in )
{
    TTrIn *tr = (TTrIn*)tr_in;

    tr->runSt	= true;
    tr->endrun	= false;

    vector< AutoHD<TProtocolIn> > prot_in;
    struct timeval tv;
    ssize_t r_len = 0;
    string req, answ;
    char buf[1000];
    fd_set rw_fd;
    int kz, sec;

    float wCharTm = s2r(TSYS::strParse(tr->timings(),0,":"));
    int wFrTm = 1000*s2i(TSYS::strParse(tr->timings(),1,":"));
    float wRtsDelay1 = 1e-3*s2r(TSYS::strParse(tr->timings(),3,":"));
    float wRtsDelay2 = 1e-3*s2r(TSYS::strParse(tr->timings(),4,":"));
    int64_t stFrTm = 0, tmW = 0, tmTmp1;

    fcntl(tr->fd, F_SETFL, 0);

    if(tr->mRTSfc) {
	ioctl(tr->fd, TIOCMGET, &sec);
	//Disable transfer for read allow
	if(!tr->mRTSlvl) sec |= TIOCM_RTS;
	else sec &= ~TIOCM_RTS;
	ioctl(tr->fd, TIOCMSET, &sec);
    }

    while(!tr->endrun) {
	// Char timeout
	while(true) {
	    tv.tv_sec = 0; tv.tv_usec = (int)(1.5e3*wCharTm);
	    FD_ZERO(&rw_fd); FD_SET(tr->fd, &rw_fd);
	    if(select(tr->fd+1,&rw_fd,NULL,NULL,&tv) <= 0) {
		if(tr->endrun || !req.empty())	break;
		continue;
	    }
	    r_len = read(tr->fd, buf, sizeof(buf));
	    if(r_len <= 0) break;

	    // Requests statistic
	    tmTmp1 = TSYS::curTime();
	    if(req.empty()) stFrTm = tmW = tmTmp1;
	    if(tmW) tr->tmMax = vmax(tr->tmMax,1e-3*(tmTmp1-tmW));
	    tmW = tmTmp1;

	    req += string(buf, r_len);
	    if((TSYS::curTime()-stFrTm) > wFrTm) break;
	}
	if(tr->endrun || req.empty()) {
	    if(r_len == 0) {
		close(tr->fd); tr->fd = -1;
		if(tr->mMdmMode && tr->mMdmDataMode) {
		    // Reconnect try after hung up by remote agent
		    mod->devUnLock(tr->mDevPort);
		    TSYS::sysSleep(10);
		    try{ tr->connect(); } catch(TError &err) { break; }
		    continue;
		}
	    }
	    break;
	}

	tr->trIn += req.size();

	if(mess_lev() == TMess::Debug)
	    mess_debug(tr->nodePath().c_str(), _("The serial interface received the message '%d'."), req.size());
	if(tr->logLen()) tr->pushLogMess(_("Received from\n") + req);

	//Check for device lock and RING request from modem
	if(tr->mMdmMode && !tr->mMdmDataMode) {
	    if(mod->devLock(tr->mDevPort,true)) continue;
	    if(req.find(tr->mdmRingReq()) != string::npos) {
		if(!mod->devLock(tr->mDevPort)) continue;
		// Send ring answer
		TTr::writeLine(tr->fd,tr->mdmRingAnswer());
		if(TTr::expect(tr->fd,tr->mdmRingAnswerResp(),tr->mdmTm()).empty()) continue;
		tr->mMdmDataMode = true;

		req = "";
		continue;
	    }
	}
	//Send message to the protocols
	string prts = tr->protocols(), prt, subPrt, tAnsw;
	for(int off = 0, iP = 0; (prt=TSYS::strParse(prts,0,";",&off)).size(); iP++, answ += tAnsw, tAnsw = "") {
	    try {
		if(iP >= (int)prot_in.size() || prot_in[iP].freeStat()) {
		    AutoHD<TProtocol> proto = SYS->protocol().at().modAt(TSYS::strParse(prt,0,"."));
		    subPrt = TSYS::strParse(prt, 1, ".");
		    string n_pr = tr->id() + i2s(tr->fd) + (subPrt.size()?"#"+subPrt:"");
		    if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, tr, "\n"+i2s(tr->fd));
		    if(iP < (int)prot_in.size()) prot_in[iP] = proto.at().at(n_pr);
		    else prot_in.push_back(proto.at().at(n_pr));
		}

		int64_t stTm = 0;
		if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
		prot_in[iP].at().mess(req, tAnsw);
		if(mess_lev() == TMess::Debug && stTm) {
		    tr->prcTm = SYS->curTime() - stTm;
		    tr->prcTmMax = vmax(tr->prcTmMax, tr->prcTm);
		}
	    } catch(TError &err) {
		mess_err(tr->nodePath().c_str(),"%s",err.mess.c_str() );
		mess_err(tr->nodePath().c_str(),_("Error requesting the protocol."));
	    }
	}

	//Sending request
	if(answ.size()) {
	    if(mess_lev() == TMess::Debug)
		mess_debug(tr->nodePath().c_str(), _("The serial interface replied by the message '%d'."), answ.size());
	    // Pure RS-485 flow control: Clear RTS for transfer allow
	    if(tr->mRTSfc) {
		if(!tr->mRTSlvl) sec &= ~TIOCM_RTS;
		else sec |= TIOCM_RTS;
		ioctl(tr->fd, TIOCMSET, &sec);
		if(wRtsDelay1) TSYS::sysSleep(wRtsDelay1);
	    }
	    ssize_t wL = 1;
	    unsigned wOff = 0;
	    for( ; wOff != answ.size() && wL > 0; wOff += wL) {
		wL = write(tr->fd, answ.data()+wOff, answ.size()-wOff);
		if(wL == 0) { mess_err(tr->nodePath().c_str(), _("Write: the answer is zero byte.")); break; }
		else if(wL < 0) {
		    if(errno == EAGAIN) {
			tv.tv_sec = wFrTm/1000000; tv.tv_usec = wFrTm%1000000;
			FD_ZERO(&rw_fd); FD_SET(tr->fd, &rw_fd);
			kz = select(tr->fd+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(tr->fd,&rw_fd)) { wL = 0; continue; }
		    }
		    mess_err(tr->nodePath().c_str(), _("Write: error '%s (%d)'."), strerror(errno), errno);
		    break;
		}
		tr->trOut += vmax(0, wL);
		if(wL > 0 && tr->logLen()) tr->pushLogMess(_("Transmitted to\n") + string(answ.data()+wOff,wL));
	    }

	    // Hard read for wait request echo and transfer disable
	    if(tr->mRTSfc) {
		if(tr->mRTSEcho && wOff == answ.size()) {
		    char echoBuf[255];
		    int64_t mLstReqTm = TSYS::curTime();
		    for(unsigned r_off = 0; r_off < answ.size(); ) {
			int kz = read(tr->fd,echoBuf,vmin(answ.size()-r_off,sizeof(echoBuf)));
			if(kz == 0 || (kz == -1 && errno == EAGAIN)) {
			    if((TSYS::curTime()-mLstReqTm) > wCharTm*answ.size()*1e3) break;
			    sched_yield();
			    continue;
			}
			if(kz < 0 || memcmp(echoBuf,answ.data()+r_off,kz) != 0) break;
			r_off += kz;
		    }
		} else tcdrain(tr->fd);
		if(wRtsDelay2) TSYS::sysSleep(wRtsDelay2);
		if(!tr->mRTSlvl) sec |= TIOCM_RTS;
		else sec &= ~TIOCM_RTS;
		ioctl(tr->fd, TIOCMSET, &sec);
	    }
	    answ = "";
	}
	req = "";
    }

    //Close protocol
    for(unsigned iP = 0; iP < prot_in.size(); iP++) {
	if(prot_in[iP].freeStat())	continue;
	try {
	    string n_pr = prot_in[iP].at().name();
	    AutoHD<TProtocol> proto = AutoHD<TProtocol>(&prot_in[iP].at().owner());
	    prot_in[iP].free();
	    proto.at().close(n_pr);
	} catch(TError &err) {
	    mess_err(tr->nodePath().c_str(), "%s", err.mess.c_str() );
	    mess_err(tr->nodePath().c_str(), _("Error closing the protocol."));
	}
    }

    tr->runSt = false;

    return NULL;
}

void TTrIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportIn::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID,3,
	    "dest","sel_ed","select","/prm/cfg/devLS","help",
	    _("The serial transport has the address format \"{dev}:{speed}:{format}[:{opts}[:{mdm}]]\", where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600);\n"
	    "    format - asynchronous data format '{size}{parity}{stop}' (8N1, 7E1, 5O2);\n"
	    "    opts - different options, mostly for flow control, separated by ',':\n"
	    "      '[-]h' - hardware (CRTSCTS);\n"
	    "      '[-]s' - software (IXON|IXOFF);\n"
	    "      'rts' - using of the RTS signal for transferring(false) and checking for echo, for raw RS-485;\n"
	    "      'rts1' - using of the RTS signal for transferring(true) and checking for echo, for raw RS-485;\n"
	    "      'rtsne' - using of the RTS signal for transferring(false) and without checking for echo, for raw RS-485;\n"
	    "      'rts1ne' - using of the RTS signal for transferring(true) and without checking for echo, for raw RS-485;\n"
	    "      '[-]RS485' - using RS-485 mode, through TIOCSRS485.\n"
	    "    mdm - modem mode, listen for 'RING'."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID);
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in the format \"{symbol}:{frm}[::{rtsDelay1}:{rtsDelay2}]\", where:\n"
	    "    symbol - maximum time of one symbol, used for the frame end detection and for timeout of the next request, in milliseconds;\n"
	    "    frm - maximum frame length, in milliseconds;\n"
	    "    rtsDelay1 - delay between the transmitter activation with RTS signal and start up of the transmission, in milliseconds;\n"
	    "    rtsDelay2 - delay between the transmitting and disconnecting the transmitter with RTS signal, in milliseconds."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/taskPrior",_("Priority"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,
	    "tp","dec", "help",TMess::labTaskPrior());
	if(s2i(TSYS::strParse(addr(),4,":")) && ctrMkNode("area",opt,-1,"/mod",_("Modem"),R_R_R_,"root",STR_ID)) {
	    ctrMkNode("fld",opt,-1,"/mod/tm",_("Timeout, seconds"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/preInitDl",_("Pre-initial delay, seconds"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/postInitDl",_("Post-initial delay, seconds"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/initStr1",_("Initialization string 1"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initStr2",_("Initialization string 2"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initResp",_("Initial response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringReq",_("Ring request"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringAnswer",_("Ring answer"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringAnswerResp",_("Result of the ring answer"),RWRWR_,"root",STR_ID,1,"tp","str");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/devLS" && ctrChkNode(opt)) {
	int off = 0;
	TSYS::ctrListFS(opt, TSYS::strParse(addr(),0,":",&off), "<chrdev>;");
	string suf = (off < (int)addr().size()) ? addr().substr(off) : "";
	for(unsigned i_t = 0; suf.size() && i_t < opt->childSize(); i_t++)
	    if(opt->childGet(i_t)->text().size())
		opt->childGet(i_t)->setText(opt->childGet(i_t)->text()+":"+suf);
    }
    else if(a_path == "/prm/cfg/TMS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else if(a_path == "/prm/cfg/taskPrior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))   opt->setText(i2s(taskPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))   setTaskPrior(s2i(opt->text()));
    }
    else if(a_path == "/mod/tm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(mdmTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmTm(s2i(opt->text()));
    }
    else if(a_path == "/mod/preInitDl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(r2s(mdmPreInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPreInit(s2r(opt->text()));
    }
    else if(a_path == "/mod/postInitDl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(r2s(mdmPostInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPostInit(s2r(opt->text()));
    }
    else if(a_path == "/mod/initStr1") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr1());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr1(opt->text());
    }
    else if(a_path == "/mod/initStr2") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr2());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr2(opt->text());
    }
    else if(a_path == "/mod/initResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitResp(opt->text());
    }
    else if(a_path == "/mod/ringReq") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmRingReq());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmRingReq(opt->text());
    }
    else if(a_path == "/mod/ringAnswer") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmRingAnswer());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmRingAnswer(opt->text());
    }
    else if(a_path == "/mod/ringAnswerResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmRingAnswerResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmRingAnswerResp(opt->text());
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TTrOut					 *
//************************************************
TTrOut::TTrOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), mNotStopOnProceed(false), fd(-1), mKeepAliveLstTm(0), trIn(0), trOut(0),
    mMdmTm(30), mMdmLifeTime(30), mMdmPreInit(0.5), mMdmPostInit(1), mMdmInitStr1("ATZ"), mMdmInitStr2(""), mMdmInitResp("OK"),
    mMdmDialStr("ATDT"), mMdmCnctResp("CONNECT"), mMdmBusyResp("BUSY"), mMdmNoCarResp("NO CARRIER"), mMdmNoDialToneResp("NO DIALTONE"),
    mMdmExit("+++"), mMdmHangUp("+++ATH"), mMdmHangUpResp("OK"),
    mMdmMode(false), mMdmDataMode(false), mRTSfc(false), mRTSlvl(false), mRTSEcho(false), mI2C(false), mSPI(false)
{
    setAddr("/dev/ttyS0:19200:8E2");
    setTimings("640:6", true);
}

TTrOut::~TTrOut( )	{ }

void TTrOut::load_( )
{
    //TTransportOut::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("TMS");		if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("NoStopOnProceed");	if(!vl.empty()) setNotStopOnProceed(s2i(vl));
	vl = prmNd.attr("MdmTm");	if(!vl.empty()) setMdmTm(s2i(vl));
	vl = prmNd.attr("MdmLifeTime");	if(!vl.empty()) setMdmLifeTime(s2i(vl));
	vl = prmNd.attr("MdmPreInit");	if(!vl.empty()) setMdmPreInit(s2r(vl));
	vl = prmNd.attr("MdmPostInit");	if(!vl.empty()) setMdmPostInit(s2r(vl));
	vl = prmNd.attr("MdmInitStr1");	if(!vl.empty()) setMdmInitStr1(vl);
	vl = prmNd.attr("MdmInitStr2");	if(!vl.empty()) setMdmInitStr2(vl);
	vl = prmNd.attr("MdmInitResp");	if(!vl.empty()) setMdmInitResp(vl);
	vl = prmNd.attr("MdmDialStr");	if(!vl.empty()) setMdmDialStr(vl);
	vl = prmNd.attr("MdmCnctResp");	if(!vl.empty()) setMdmCnctResp(vl);
	vl = prmNd.attr("MdmBusyResp");	if(!vl.empty()) setMdmBusyResp(vl);
	vl = prmNd.attr("MdmNoCarResp");if(!vl.empty()) setMdmNoCarResp(vl);
	vl = prmNd.attr("MdmNoDialToneResp");	if(!vl.empty()) setMdmNoDialToneResp(vl);
	vl = prmNd.attr("MdmExit");	if(!vl.empty()) setMdmExit(vl);
	vl = prmNd.attr("MdmHangUp");	if(!vl.empty()) setMdmHangUp(vl);
	vl = prmNd.attr("MdmHangUpResp");if(!vl.empty()) setMdmHangUpResp(vl);
    } catch(...) { }
}

void TTrOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("TMS", timings());
    prmNd.setAttr("NoStopOnProceed", i2s(notStopOnProceed()));
    prmNd.setAttr("MdmTm", i2s(mdmTm()));
    prmNd.setAttr("MdmLifeTime", i2s(mdmLifeTime()));
    prmNd.setAttr("MdmPreInit", r2s(mdmPreInit()));
    prmNd.setAttr("MdmPostInit", r2s(mdmPostInit()));
    prmNd.setAttr("MdmInitStr1",mdmInitStr1());
    prmNd.setAttr("MdmInitStr2",mdmInitStr2());
    prmNd.setAttr("MdmInitResp",mdmInitResp());
    prmNd.setAttr("MdmDialStr",mdmDialStr());
    prmNd.setAttr("MdmCnctResp",mdmCnctResp());
    prmNd.setAttr("MdmBusyResp",mdmBusyResp());
    prmNd.setAttr("MdmNoCarResp",mdmNoCarResp());
    prmNd.setAttr("MdmNoDialToneResp",mdmNoDialToneResp());
    prmNd.setAttr("MdmExit",mdmExit());
    prmNd.setAttr("MdmHangUp",mdmHangUp());
    prmNd.setAttr("MdmHangUpResp",mdmHangUpResp());
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportOut::save_();
}

string TTrOut::getStatus( )
{
    string rez = TTransportOut::getStatus();

    if(startStat()) {
	rez += TSYS::strMess(_("Traffic in %s, out %s. "),TSYS::cpct2str(trIn).c_str(),TSYS::cpct2str(trOut).c_str());
	if(mess_lev() == TMess::Debug && (respTmMax || respSymbTmMax))
	    rez += TSYS::strMess(_("Response time %s[%s:%s/1.5]. "), tm2s(1e-6*respTm).c_str(), tm2s(1e-6*respTmMax).c_str(), tm2s(1e-6*respSymbTmMax).c_str());
    }

    return rez;
}

bool TTrOut::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR") {
	//Times adjust
	int speed = s2i(TSYS::strParse(co.getS(),1,":"));
	if(TSYS::strParse(addr(),4,":").size()) { mDefTimeouts = true; setTimings("5000:1000", true); }
	else if(speed) {
	    float symbMlt = TRegExp("^.+\\/ttyS\\d+$").test(TSYS::strParse(co.getS(),0,":")) ? 1 : 3;
	    mDefTimeouts = true;
	    setTimings(i2s((1024*11*1000)/speed)+":"+r2s(symbMlt*11e4/(float)speed,2,'f')+
			":"+TSYS::strParse(timings(),2,":")+":"+TSYS::strParse(timings(),3,":")+":"+TSYS::strParse(timings(),4,":"), true);
	}
    }

    return TTransportOut::cfgChange(co, pc);
}

void TTrOut::setTimings( const string &vl, bool isDef )
{
    if((isDef && !mDefTimeouts) || vl == mTimings) return;
    else if(!isDef) mDefTimeouts = false;

    int wReqTm = vmax(1, vmin(10000,s2i(TSYS::strParse(vl,0,":"))));
    string charTm = TSYS::strParse(vl, 1, ":");
    float wCharTm = vmax(0.01, vmin(1e4,s2r(charTm)));
    string reqRetrMult = TSYS::strParse(charTm, 1, "-");
    mTimings = reqRetrMult.size() ? TSYS::strMess("%d:%g-%g", wReqTm, wCharTm, vmax(0,vmin(10,s2r(reqRetrMult)))) :
				    TSYS::strMess("%d:%g", wReqTm, wCharTm);

    float wKeepAliveTm = vmax(0, vmin(1e3,s2r(TSYS::strParse(vl,2,":"))));
    float wRtsDelay1 = vmax(0, vmin(10000,s2r(TSYS::strParse(vl,3,":"))));
    float wRtsDelay2 = vmax(0, vmin(10000,s2r(TSYS::strParse(vl,4,":"))));
    if(wKeepAliveTm || wRtsDelay1 || wRtsDelay2) mTimings += TSYS::strMess(":%g", wKeepAliveTm);
    if(wRtsDelay1 || wRtsDelay2) mTimings += TSYS::strMess(":%g:%g", wRtsDelay1, wRtsDelay2);

    respSymbTmMax = 0;

    if(!isDef) modif();
}

void TTrOut::start( int tmCon )
{
    MtxAlloc res(reqRes(), true);
    if(runSt) return;

    //Statuses clear
    mMdmMode = mRTSfc = mRTSlvl = mRTSEcho = mI2C = mSPI = false;
    trIn = trOut = respTm = respTmMax = respSymbTmMax = 0;
    bool isLock = false;

    try {
	//Open and setup device
	mDevPort = TSYS::strParse(addr(), 0, ":");
	// Lock device for all serial transports
	if(!(isLock=mod->devLock(mDevPort))) throw TError(nodePath().c_str(),_("The device '%s' is using now."),mDevPort.c_str());

	mess_debug(nodePath().c_str(), _("Connecting."));

	// Serial port open
	//  O_NONBLOCK is used for prevent function open() hang on several USB->RS485 converters
	fd = open(mDevPort.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) throw TError(nodePath().c_str(), _("Error the serial port '%s' %s: %s."), mDevPort.c_str(), "open", strerror(errno));

	// Set serial port parameters
	struct termios tio;
	uint8_t spiMode = 0,
		spiBits = 8;
	uint32_t spiSpeed = 500000;

	if(tcgetattr(fd,&tio) < 0) {
	    string tErr = TSYS::strMess(_("Error the serial port '%s' %s: %s."), mDevPort.c_str(), "tcgetattr", strerror(errno));
#ifdef I2C_SLAVE
	    //  Try for I2C by set some slave device address
	    if(ioctl(fd,I2C_SLAVE,0) >= 0)	mI2C = true;
	    else
#endif
#ifdef HAVE_LINUX_SPI_SPIDEV_H
	    //  Try for SPI ...
	    if(ioctl(fd,SPI_IOC_WR_MODE,&spiMode) >= 0 && ioctl(fd,SPI_IOC_RD_MODE,&spiMode) >= 0 &&
		    ioctl(fd,SPI_IOC_WR_BITS_PER_WORD,&spiBits) >= 0 && ioctl(fd,SPI_IOC_RD_BITS_PER_WORD,&spiBits) >= 0 &&
		    ioctl(fd,SPI_IOC_WR_MAX_SPEED_HZ,&spiSpeed) >= 0 && ioctl(fd,SPI_IOC_RD_MAX_SPEED_HZ,&spiSpeed) >= 0)
		mSPI = true;
	    else
#endif
		throw TError(nodePath().c_str(), "%s", tErr.c_str());
	}
	if(!mI2C && !mSPI) {
	    tio.c_iflag = 0;
	    tio.c_oflag = 0;
	    tio.c_cflag |= (CREAD|CLOCAL);
	    tio.c_lflag = 0;
	    tio.c_cc[VTIME] = 0;	//< inter-character timer unused
	    tio.c_cc[VMIN] = 0;		//< blocking read until 0 character arrives

	    // Set speed
	    string speed = sTrm(TSYS::strParse(addr(),1,":"));
	    if(!speed.empty()) {
		speed_t tspd = B9600;
		switch(s2i(speed)) {
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
		    default: throw TError(nodePath().c_str(),_("Error the speed '%s'."),speed.c_str());
		}
		cfsetispeed(&tio, tspd);
		cfsetospeed(&tio, tspd);
	    }

	    // Set asynchronous data format
	    string format = sTrm(TSYS::strParse(addr(),2,":"));
	    if(!format.empty()) {
		if(format.size() != 3) throw TError(nodePath().c_str(),_("Error the asynchronous data format '%s'."),format.c_str());

		//  Set byte length
		int len =  format[0]-'0';
		if(len < 5 || len > 8) throw TError(nodePath().c_str(),_("Error char length %d."),len);
		tio.c_cflag &= ~CSIZE;
		switch(len) {
		    case 5:	tio.c_cflag |= CS5;	break;
		    case 6:	tio.c_cflag |= CS6;	break;
		    case 7:	tio.c_cflag |= CS7;	break;
		    case 8:	tio.c_cflag |= CS8;	break;
		}

		//  Set parity
		char parity = tolower(format[1]);
		switch(parity) {
		    case 'e': tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD;break;
		    case 'o': tio.c_cflag |= PARENB; tio.c_cflag |= PARODD;	break;
		    case 'n': tio.c_cflag &= ~PARENB;			break;
		    default: throw TError(nodePath().c_str(),_("Error the parity checking mode '%c'."),parity);
		}

		//  Set stop bits number
		int stopbt = format[2]-'0';
		if(stopbt == 1) tio.c_cflag &= ~CSTOPB;
		else if(stopbt == 2) tio.c_cflag |= CSTOPB;
		else throw TError(nodePath().c_str(),_("Error the stop bits %d."),stopbt);
	    }

	    // Set options
	    string opts = sTrm(TSYS::strParse(addr(),3,":")), opt;
	    for(int off = 0; (opt=TSYS::strParse(opts,0,",",&off)).size(); ) {
		if(strcasecmp(opt.c_str(),"h") == 0)		tio.c_cflag |= CRTSCTS;
		else if(strcasecmp(opt.c_str(),"-h") == 0)	tio.c_cflag &= ~CRTSCTS;

		if(strcasecmp(opt.c_str(),"s") == 0)		tio.c_iflag |= (IXON|IXOFF|IXANY);
		else if(strcasecmp(opt.c_str(),"-s") == 0)	tio.c_iflag &= ~(IXON|IXOFF|IXANY);

		if(strcasecmp(opt.c_str(),"rts") == 0)		{ mRTSfc = mRTSEcho = true; }
		else if(strcasecmp(opt.c_str(),"rts1") == 0)	{ mRTSfc = mRTSlvl = mRTSEcho = true; }
		else if(strcasecmp(opt.c_str(),"rts1ne") == 0)	{ mRTSfc = mRTSlvl = true; }
		else if(strcasecmp(opt.c_str(),"rtsne") == 0)	{ mRTSfc = true; }

#ifdef SER_RS485_ENABLED
#ifndef TIOCSRS485
#define TIOCSRS485      0x542f
#endif
		// Standard RS-485 mode
		if(strcasecmp(opt.c_str(),"rs485") == 0 || strcasecmp(opt.c_str(),"-rs485") == 0) {
		    serial_rs485 rs485conf;
		    memset(&rs485conf, 0, sizeof(serial_rs485));
		    if(strcasecmp(opt.c_str(),"rs485") == 0) rs485conf.flags |= SER_RS485_ENABLED;
		    ioctl(fd, TIOCSRS485, &rs485conf);
		}
#endif
	    }

	    // Set port's data
	    tcflush(fd, TCIOFLUSH);
	    if(tcsetattr(fd,TCSANOW,&tio) < 0)
		throw TError(nodePath().c_str(), _("Error the serial port '%s' %s: %s."), mDevPort.c_str(), "tcsetattr", strerror(errno));

	    //Modem connection establish
	    string telNumb = sTrm(TSYS::strParse(addr(),4,":"));
	    if(!telNumb.empty()) {
		// Resource to transfer function alloc
		runSt = true;
		mMdmMode = true;

		// Send init 1 string
		if(!mdmInitStr1().empty()) {
		    if(mdmPreInit() > 0) {
			TSYS::sysSleep(mdmPreInit()*0.5);
			TTr::writeLine(fd,"");
			TSYS::sysSleep(mdmPreInit()*0.5);
		    }
		    TTr::writeLine(fd,mdmInitStr1());
		    if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
			throw TError(nodePath().c_str(), _("No response to the initial request '%s'."), mdmInitStr1().c_str());
		    TSYS::sysSleep(mdmPostInit());
		}
		// Send init 2 string
		if(!mdmInitStr2().empty()) {
		    if(mdmPreInit() > 0) {
			TSYS::sysSleep(mdmPreInit()*0.5);
			TTr::writeLine(fd,"");
			TSYS::sysSleep(mdmPreInit()*0.5);
		    }
		    TTr::writeLine(fd,mdmInitStr2());
		    if(TTr::expect(fd,mdmInitResp(),mdmTm()).empty())
			throw TError(nodePath().c_str(), _("No response to the initial request '%s'."), mdmInitStr2().c_str());
		    TSYS::sysSleep(mdmPostInit());
		}
		// Dial number and connection wait
		string rez;
		telNumb = mdmDialStr()+telNumb;
		TTr::writeLine(fd,telNumb);
		if((rez=TTr::expect(fd,mdmCnctResp()+"\n"+mdmBusyResp()+"\n"+mdmNoCarResp()+"\n"+mdmNoDialToneResp(),mdmTm())) != mdmCnctResp())
		{
		    if(rez == mdmBusyResp())		throw TError(nodePath().c_str(), _("Modem busy"));
		    else if(rez == mdmNoCarResp())	throw TError(nodePath().c_str(), _("Modem no carrier"));
		    else if(rez == mdmNoDialToneResp())	throw TError(nodePath().c_str(), _("Modem no dial tone"));
		    else				throw TError(nodePath().c_str(), _("Modem no connected"));
		}
		mLstReqTm = TSYS::curTime();
		mMdmDataMode = true;
	    }
	}
    } catch(TError &err) {
	//HangUp
	if(mMdmMode) TTr::writeLine(fd,mdmHangUp());

	if(fd >= 0) { close(fd); fd = -1; }
	if(isLock) mod->devUnLock(mDevPort);
	runSt = false;
	mMdmMode = false;
	throw;
    }

    mKeepAliveLstTm = TSYS::curTime();
    runSt = true;

    TTransportOut::start();

    if(logLen()) pushLogMess(_("Connected"));
}

void TTrOut::stop( )
{
    MtxAlloc res(reqRes(), true);
    if(!runSt) return;

    mess_debug(nodePath().c_str(), _("Disconnecting."));

    if(mMdmDataMode) {
	TTr::writeLine(fd, mdmExit(), true);
	if(mdmPreInit() > 0) TSYS::sysSleep(mdmPreInit());
	//HangUp
	TTr::writeLine(fd,mdmHangUp());
	mMdmDataMode = false;
    }

    //Status clear
    trIn = trOut = 0;

    //Port close
    close(fd); fd = -1;

    //Unlock device
    mod->devUnLock(mDevPort);

    runSt = false;
    mMdmMode = false;

    TTransportOut::stop();

    if(logLen()) pushLogMess(_("Disconnected"));
}

void TTrOut::check( )
{
    int reRs = 1;
    bool toStop = (mMdmMode && mMdmDataMode && (reRs=reqRes().tryLock()) == 0 && (TSYS::curTime()-mLstReqTm)/1000000 > mdmLifeTime());
    if(reRs == 0) reqRes().unlock();
    if(toStop) stop();
}

int TTrOut::messIO( const char *oBuf, int oLen, char *iBuf, int iLen, int time )
{
    string err = _("Unknown error");
    ssize_t blen = 0;
    int off = 0, kz, sec;
    fd_set rw_fd;
    struct timeval tv;
    bool notReq = (time < 0);
    time = abs(time);

    MtxAlloc res(reqRes(), true);

    if(!runSt) throw TError(nodePath().c_str(),_("Transport is not connected!"));

    int wReqTm = s2i(TSYS::strParse(timings(),0,":",&off));
    wReqTm = time ? time : wReqTm;

    string tVl = TSYS::strParse(timings(), 0, ":", &off);
    float wCharTm = s2r(TSYS::strParse(tVl,0,"-"));
    tVl = TSYS::strParse(tVl, 1, "-");
    float reqRetrMult = tVl.size() ? vmax(0,vmin(10,s2r(tVl))) : 4;

    float wKeepAliveTm = s2r(TSYS::strParse(timings(),0,":",&off));

    float wRtsDelay1 = 1e-3*s2r(TSYS::strParse(timings(),0,":",&off));

    float wRtsDelay2 = 1e-3*s2r(TSYS::strParse(timings(),0,":",&off));

    if(wKeepAliveTm > 0 && !notStopOnProceed() && (TSYS::curTime()-mKeepAliveLstTm) > wKeepAliveTm*1000000) {
	mess_debug(nodePath().c_str(), _("Reconnect by KeepAliveTm %gs."), wKeepAliveTm);
	stop();
	start();
	mKeepAliveLstTm = TSYS::curTime();
    }

    int64_t tmW = TSYS::curTime();

    if(mRTSfc)	ioctl(fd, TIOCMGET, &sec);

    int64_t stRespTm = 0;

    //Write request
    if(oBuf && oLen > 0) {
	// Wait for char based timeout
	if((tmW-mLstReqTm) < (reqRetrMult*wCharTm)) TSYS::sysSleep(1e-6*((reqRetrMult*wCharTm)-(tmW-mLstReqTm)));

	// Input buffer clean
	if(!notReq && !mI2C && !mSPI) {
	    tcflush(fd, TCIOFLUSH);
	    char tbuf[30]; while(read(fd,tbuf,sizeof(tbuf)) > 0) ;
	}

	// Pure RS-485 flow control: Clear RTS for transfer allow
	if(mRTSfc) {
	    if(!mRTSlvl) sec &= ~TIOCM_RTS;
	    else sec |= TIOCM_RTS;
	    ioctl(fd, TIOCMSET, &sec);
	    if(wRtsDelay1) TSYS::sysSleep(wRtsDelay1);
	}

	// Same writing
	int wOff = 0;
#ifdef I2C_SLAVE
	if(mI2C && oLen) { ioctl(fd, I2C_SLAVE, oBuf[wOff]); wOff++; }
#endif
	for( ; wOff != oLen; wOff += kz)
	    if((kz=write(fd,oBuf+wOff,oLen-wOff)) <= 0) {
		if(kz == 0 || (kz < 0 && errno == EAGAIN)) {
		    tv.tv_sec = wReqTm/1000; tv.tv_usec = 1000*(wReqTm%1000);
		    FD_ZERO(&rw_fd); FD_SET(fd, &rw_fd);
		    kz = select(fd+1, NULL, &rw_fd, NULL, &tv);
		    if(kz > 0 && FD_ISSET(fd,&rw_fd)) { kz = 0; continue; }
		}
		err = (kz < 0) ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No wrote data");
		mLstReqTm = TSYS::curTime();
		if(!notStopOnProceed()) stop();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error writing: %s"), err.c_str());
		if(logLen()) pushLogMess(TSYS::strMess(_("Error transmitting: %s"), err.c_str()));
		throw TError(nodePath().c_str(), _("Error writing: %s"), err.c_str());
	    }
	    else {
		trOut += kz;
		if(logLen()) pushLogMess(_("Transmitted to\n") + string(oBuf+wOff,kz));
		if(mess_lev() == TMess::Debug) stRespTm = SYS->curTime();
	    }

	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Wrote %s."), TSYS::cpct2str(oLen).c_str());

	// Hard read for wait request echo and transfer disable
	if(mRTSfc) {
	    if(mRTSEcho) {
		char echoBuf[255];
		mLstReqTm = TSYS::curTime();
		for(int r_off = 0; r_off < oLen; ) {
		    kz = read(fd, echoBuf, vmin(oLen-r_off,(int)sizeof(echoBuf)));
		    if(kz == 0 || (kz == -1 && errno == EAGAIN)) {
			if((TSYS::curTime()-mLstReqTm) > wCharTm*oLen*1e3) throw TError(nodePath().c_str(), _("Timeouted!"));
			sched_yield();
			continue;
		    }
		    if(kz < 0 || memcmp(echoBuf,oBuf+r_off,kz) != 0) throw TError(nodePath().c_str(),_("Error reading the echo request."));
		    r_off += kz;
		}
	    } else tcdrain(fd);
	    if(wRtsDelay2) TSYS::sysSleep(wRtsDelay2);
	    if(!mRTSlvl) sec |= TIOCM_RTS;
	    else sec &= ~TIOCM_RTS;
	    ioctl(fd, TIOCMSET, &sec);
	}
    }

    //Read reply
    if(iBuf != NULL && iLen > 0) {
	if(oBuf && oLen > 0) { tv.tv_sec  = wReqTm/1000; tv.tv_usec = 1000*(wReqTm%1000); }
	else { tv.tv_sec = (int)(1.5e-3*wCharTm); tv.tv_usec = (int)(1.5e3*wCharTm)%1000000; }
	FD_ZERO(&rw_fd); FD_SET(fd, &rw_fd);
	kz = select(fd+1, &rw_fd, NULL, NULL, &tv);
	if(kz == 0) {
	    mLstReqTm = TSYS::curTime();
	    if(!notReq) {
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Reading timeouted."));
		if(logLen()) pushLogMess(_("Reading timeouted"));
		if(wKeepAliveTm < 0 && oLen > 0 && !notStopOnProceed()) stop();
	    }
	    throw TError(nodePath().c_str(), _("Reading timeouted."));
	}
	else if(kz < 0) {
	    err = TSYS::strMess("%s (%d)", strerror(errno), errno);
	    mLstReqTm = TSYS::curTime();
	    if(!notStopOnProceed()) stop();
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading (select): %s"), err.c_str());
	    if(logLen()) pushLogMess(TSYS::strMess(_("Error reading (select): %s"), err.c_str()));
	    throw TError(nodePath().c_str(), _("Error reading (select): %s"), err.c_str());
	}
	else if(FD_ISSET(fd,&rw_fd)) {
	    //!! Reading in that way but some time read() return 0 after the select() pass.
	    // * Force wait any data in the request mode or EAGAIN
	    // * No wait any data in the not request mode but it can get the data later
	    for(int iRtr = 0; (((blen=read(fd,iBuf,iLen)) == 0 && !notReq) || (blen < 0 && errno == EAGAIN)) && iRtr < time; ++iRtr)
		TSYS::sysSleep(1e-3);
	    // * Force errors
	    // * Retry if any data was wrote but no a reply there into the request mode
	    if(blen < 0 || (blen == 0 && oBuf && oLen > 0 && !notReq)) {
		err = (blen < 0) ? TSYS::strMess("%s (%d)", strerror(errno), errno) : _("No data");
		mLstReqTm = TSYS::curTime();
		if(!notStopOnProceed()) stop();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading: %s"), err.c_str());
		if(logLen()) pushLogMess(TSYS::strMess(_("Error reading: %s"), err.c_str()));
		throw TError(nodePath().c_str(), _("Error reading: %s"), err.c_str());
	    }
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read %s."), TSYS::cpct2str(vmax(0,blen)).c_str());
	    if(blen > 0 && logLen()) pushLogMess(_("Received from\n") + string(iBuf,blen));
	    if(blen > 0 && !notReq && mess_lev() == TMess::Debug) {
		if(!(oBuf && oLen > 0))
		    respSymbTmMax = fmax(respSymbTmMax, SYS->curTime()-tmW);
		    /*{
		    if((stRespTm=SYS->curTime()-mLstReqTm)/1.5e3 < wCharTm)
			respSymbTmMax = fmax(respSymbTmMax, stRespTm);
		}*/
		else if(stRespTm) {
		    respTm = SYS->curTime() - stRespTm;
		    respTmMax = vmax(respTmMax, respTm);
		}
	    }
	    trIn += vmax(0, blen);
	}
    }
    mLstReqTm = TSYS::curTime();

    return vmax(0, blen);
}

TVariant TTrOut::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // bool TS(bool rts = EVAL) - To Send control by set request <rts> and return Clear CTS state
    //  rts - Request value RTS
    if(iid == "TS") {
	MtxAlloc res(reqRes(), true);
	if(!runSt) return EVAL_BOOL;
	int tiocm;
	//Get TIOCM current status
	ioctl(fd, TIOCMGET, &tiocm);
	if(prms.size()) {
	    //Set RTS
	    if(!prms[0].isEVal() && !prms[0].isNull()) {
		if(prms[0].getB()) tiocm |= TIOCM_RTS; else tiocm &= ~TIOCM_RTS;
		ioctl(fd, TIOCMSET, &tiocm);
	    }
	    //Get current RTS
	    else { prms[0].setB((bool)(tiocm&TIOCM_RTS)); prms[0].setModify(); }
	}
	return (bool)(tiocm&TIOCM_CTS);
    }
    // bool DR(bool dtr = EVAL) - Device ready to communicate control by set Terminal Ready <dtr> and return Set Ready DSR state
    //  dtr - Terminal ready value DTR
    if(iid == "DR") {
	MtxAlloc res(reqRes(), true);
	if(!runSt) return EVAL_BOOL;
	int tiocm;
	//Get TIOCM current status
	ioctl(fd, TIOCMGET, &tiocm);
	if(prms.size()) {
	    //Set DTR
	    if(!prms[0].isEVal() && !prms[0].isNull()) {
		if(prms[0].getB()) tiocm |= TIOCM_DTR; else tiocm &= ~TIOCM_DTR;
		ioctl(fd, TIOCMSET, &tiocm);
	    }
	    //Get current DTR
	    else { prms[0].setB((bool)(tiocm&TIOCM_DTR)); prms[0].setModify(); }
	}
	return (bool)(tiocm&TIOCM_DSR);
    }
    // bool DCD() - Data Carrier Detect control
    if(iid == "DCD") {
	MtxAlloc res(reqRes(), true);
	if(!runSt) return EVAL_BOOL;
	int tiocm;
	//Get TIOCM current status
	ioctl(fd, TIOCMGET, &tiocm);
	return (bool)(tiocm&TIOCM_CD);
    }
    // bool RI() - Ring Indicator control
    if(iid == "RI") {
	MtxAlloc res(reqRes(), true);
	if(!runSt) return EVAL_BOOL;
	int tiocm;
	//Get TIOCM current status
	ioctl(fd, TIOCMGET, &tiocm);
	return (bool)(tiocm&TIOCM_RI);
    }
    // int sendbreak(int duration = 0)
    if(iid == "sendbreak") {
	MtxAlloc res(reqRes(), true);
	if(!runSt) return (int64_t)EVAL_INT;
	return tcsendbreak(fd, prms.size() ? prms[0].getI() : 0);
    }

    return TTransportOut::objFuncCall(iid, prms, user);
}

void TTrOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportOut::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,3,
	    "dest","sel_ed", "select","/prm/cfg/devLS", "help",owner().outAddrHelp().c_str());
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in the format \"{conn}:{symbol}[-{NextReqMult}][:{KeepAliveTm}[:{rtsDelay1}:{rtsDelay2}]]\", where:\n"
	    "    conn - maximum time of waiting the connecting response, in milliseconds;\n"
	    "    symbol - maximum time of one symbol, used for the frame end detection, in milliseconds;\n"
	    "    NextReqMult - next request's multiplicator to the <symbol> time, 4 by default;\n"
	    "    KeepAliveTm - keep alive timeout to reconnect the transport, in seconds;\n"
	    "                  use the value < 0 for disconnecting the transport after missing response at each request;\n"
	    "    rtsDelay1 - delay between the transmitter activation with RTS signal and start up of the transmission, in milliseconds;\n"
	    "    rtsDelay2 - delay between the transmitting and disconnecting the transmitter with RTS signal, in milliseconds."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/notStopOnProceed",_("Do not disconnect at processing"),RWRWR_,"root",STR_ID,2,"tp","bool", "help",
	    _("Sometime opened device closing can be breakage, on ICP-DAS LP PLC for example, then you are alowed to prevent it by this option."));
	if(TSYS::strParse(addr(),4,":").size() && ctrMkNode("area",opt,-1,"/mod",_("Modem"),R_R_R_,"root",STR_ID)) {
	    ctrMkNode("fld",opt,-1,"/mod/tm",_("Timeout, seconds"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/lifeTm",_("Life time, seconds"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/preInitDl",_("Pre-initial delay, seconds"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/postInitDl",_("Post-initial delay, seconds"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/initStr1",_("Initialization string 1"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initStr2",_("Initialization string 2"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initResp",_("Initial response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/dialStr",_("Dial string"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/cnctResp",_("Connect response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/busyResp",_("Busy response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/noCarResp",_("No carrier response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/noDialToneResp",_("No dial tone response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/exit",_("Exit"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/hangUp",_("Hangup string"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/hangUpResp",_("Hangup response"),RWRWR_,"root",STR_ID,1,"tp","str");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/devLS" && ctrChkNode(opt)) {
	int off = 0;
	TSYS::ctrListFS(opt, TSYS::strParse(addr(),0,":",&off), "<chrdev>;");
	string suf = (off < (int)addr().size()) ? addr().substr(off) : "";
	for(unsigned i_t = 0; suf.size() && i_t < opt->childSize(); i_t++)
	    if(opt->childGet(i_t)->text().size())
		opt->childGet(i_t)->setText(opt->childGet(i_t)->text()+":"+suf);
    }
    else if(a_path == "/prm/cfg/TMS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else if(a_path == "/prm/cfg/notStopOnProceed") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(notStopOnProceed()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setNotStopOnProceed(s2i(opt->text()));
    }
    else if(a_path == "/mod/tm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(mdmTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmTm(s2i(opt->text()));
    }
    else if(a_path == "/mod/lifeTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(mdmLifeTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmLifeTime(s2i(opt->text()));
    }
    else if(a_path == "/mod/preInitDl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(r2s(mdmPreInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPreInit(s2r(opt->text()));
    }
    else if(a_path == "/mod/postInitDl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(r2s(mdmPostInit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmPostInit(s2r(opt->text()));
    }
    else if(a_path == "/mod/initStr1") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr1());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr1(opt->text());
    }
    else if(a_path == "/mod/initStr2") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitStr2());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitStr2(opt->text());
    }
    else if(a_path == "/mod/initResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmInitResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmInitResp(opt->text());
    }
    else if(a_path == "/mod/dialStr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmDialStr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmDialStr(opt->text());
    }
    else if(a_path == "/mod/cnctResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmCnctResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmCnctResp(opt->text());
    }
    else if(a_path == "/mod/busyResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmBusyResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmBusyResp(opt->text());
    }
    else if(a_path == "/mod/noCarResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmNoCarResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmNoCarResp(opt->text());
    }
    else if(a_path == "/mod/noDialToneResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmNoDialToneResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmNoDialToneResp(opt->text());
    }
    else if(a_path == "/mod/exit") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmExit());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmExit(opt->text());
    }
    else if(a_path == "/mod/hangUp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmHangUp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmHangUp(opt->text());
    }
    else if(a_path == "/mod/hangUpResp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(mdmHangUpResp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMdmHangUpResp(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

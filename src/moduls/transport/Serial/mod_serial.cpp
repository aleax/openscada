
//OpenSCADA system module Transport.Serial file: mod_serial.cpp
/***************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <linux/serial.h>
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
#define MOD_VER		"0.8.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow serial based interfaces. Used for data exchanging through serial interfaces like RS232, RS485, GSM and other.")
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
TTr::TTr( string name ) : TTipTransport(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTr::~TTr( )
{
    try{ modStop(); }catch(...){}
}

void TTr::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	owner().inEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
	owner().outEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
    }
}

AutoHD<TTrIn> TTr::inAt( const string &name )	{ return TTipTransport::inAt(name); }

AutoHD<TTrOut> TTr::outAt( const string &name )	{ return TTipTransport::outAt(name); }

void TTr::load_( )
{
    //> Load parameters from command line

}

TTransportIn *TTr::In( const string &name, const string &idb )	{ return new TTrIn(name,idb,&owner().inEl()); }

TTransportOut *TTr::Out( const string &name, const string &idb ){ return new TTrOut(name,idb,&owner().outEl()); }

bool TTr::devLock( const string &dn, bool check )
{
    ResAlloc res(nodeRes(), true);

    if(check) return mDevLock[dn];
    if(mDevLock[dn]) return false;
    mDevLock[dn] = true;
    return true;
}

void TTr::devUnLock( const string &dn )
{
    ResAlloc res(nodeRes(), true);
    mDevLock[dn] = false;
}

void TTr::perSYSCall( unsigned int cnt )
{
    //Check all output transports
    vector<string> ls;
    mod->outList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	try{ mod->outAt(ls[i_l]).at().check(); }
	catch(TError err){ }
}

void TTr::writeLine( int fd, const string &ln, bool noNewLn )
{
    string obuf = ln + (noNewLn?"":"\x0D\x0A");
    for(unsigned wOff = 0, kz = 0; wOff != obuf.size(); wOff += kz)
	if((kz=write(fd,obuf.data()+wOff,obuf.size()-wOff)) <= 0)
	    throw TError(mod->nodePath().c_str(),_("Write line error."));
    mess_debug(mod->nodePath().c_str(), _("Send to modem %d: '%s'."), fd, ln.c_str());
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
	else if(kz < 0) throw TError(mod->nodePath().c_str(),_("Read from serial error."));
	else if(FD_ISSET(fd,&rd_fd)) {
	    rl = read(fd, buf, sizeof(buf));
	    rez.append(buf,rl);
	    for(int off = 0; (stmp=TSYS::strParse(expLst,0,"\n",&off)).size(); )
		if(rez.find(stmp) != string::npos) {
		    mess_debug(mod->nodePath().c_str(), _("Receive from modem %d: '%s'."), fd, stmp.c_str());
		    return stmp;
		}
	}
    }

    if(rez.empty()) mess_debug(mod->nodePath().c_str(), _("No any respond from modem %d."), fd);
    else mess_debug(mod->nodePath().c_str(), _("No any expected respond but receive from modem %d: '%s'."), fd, rez.c_str());

    return "";
}

//************************************************
//* TTrIn                                        *
//************************************************
TTrIn::TTrIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), fd(-1), endrun(false), trIn(0), trOut(0), tmMax(0), mTaskPrior(0),
    mMdmTm(20), mMdmPreInit(0.5), mMdmPostInit(1), mMdmInitStr1("ATZ"), mMdmInitStr2(""), mMdmInitResp("OK"),
    mMdmRingReq("RING"), mMdmRingAnswer("ATA"), mMdmRingAnswerResp("CONNECT"),
    mMdmMode(false), mMdmDataMode(false), mRTSfc(false)
{
    setAddr("/dev/ttyS0:19200:8E2");
    setTimings("6:320");
}

TTrIn::~TTrIn( )	{ }

void TTrIn::load_( )
{
    TTransportIn::load_();

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
    } catch(...){ }
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
	int speed = s2i(TSYS::strSepParse(co.getS(),1,':'));
	if(speed)	setTimings(r2s(11e4/(float)speed,2,'f')+":"+i2s((512*11*1000)/speed));
    }

    return TTransportIn::cfgChange(co, pc);
}

string TTrIn::getStatus( )
{
    string rez = TTransportIn::getStatus( );

    if(startStat())
	rez += TSYS::strMess(_("Traffic in %s, out %s. Maximum char timeout %.4g ms."),
	    TSYS::cpct2str(trIn).c_str(),TSYS::cpct2str(trOut).c_str(),tmMax);

    return rez;
}

void TTrIn::setTimings( const string &vl )
{
    double wCharTm = vmax(0.01,vmin(1e3,s2r(TSYS::strSepParse(vl,0,':'))));
    int wFrTm = vmax(1,vmin(10000,s2i(TSYS::strSepParse(vl,1,':'))));
    mTimings = TSYS::strMess("%g:%d",wCharTm,wFrTm);

    modif();
}

void TTrIn::connect( )
{
    try {
	//Open and setup device
	// Serial port open
	mDevPort = TSYS::strSepParse(addr(),0,':');
	//  O_NONBLOCK is used for prevent function open() hang on several USB->RS485 converters
	fd = open(mDevPort.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) throw TError(nodePath().c_str(),_("Serial port '%s' open error: %s."),mDevPort.c_str(),strerror(errno));
	// Set serial port parameters
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag |= (CREAD|CLOCAL);
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 0;            ///< blocking read until 0 character arrives*/
	// Set speed
	string speed = TSYS::strNoSpace(TSYS::strSepParse(addr(),1,':').c_str());
	if(!speed.empty()) {
	    speed_t tspd = B9600;
	    switch(s2i(speed))
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
	// Set asynchronous data format
	string format = TSYS::strNoSpace(TSYS::strNoSpace(TSYS::strSepParse(addr(),2,':')));
	if(!format.empty()) {
	    if(format.size() != 3) throw TError(nodePath().c_str(),_("Asynchronous data format '%s' error."),format.c_str());
	    //  Set byte length
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
	    //  Set parity
	    char parity = tolower(format[1]);
	    switch(parity)
	    {
		case 'e': tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD;break;
		case 'o': tio.c_cflag |= PARENB; tio.c_cflag |= PARODD;	break;
		case 'n': tio.c_cflag &= ~PARENB;			break;
		default: throw TError(nodePath().c_str(),_("Parity checking mode '%c' error."),parity);
	    }
	    //  Set stop bits number
	    int stopbt = format[2]-'0';
	    if(stopbt == 1) tio.c_cflag &= ~CSTOPB;
	    else if(stopbt == 2) tio.c_cflag |= CSTOPB;
	    else throw TError(nodePath().c_str(),_("Stop bits '%d' error."),stopbt);
	}

	// Set flow control
	string fc = TSYS::strNoSpace(TSYS::strSepParse(addr(),3,':'));
	mRTSfc = false;
	tio.c_cflag &= ~CRTSCTS;
	if(strcasecmp(fc.c_str(),"h") == 0)         tio.c_cflag |= CRTSCTS;
	else if(strcasecmp(fc.c_str(),"s") == 0)    tio.c_iflag |= (IXON|IXOFF|IXANY);
	else if(strcasecmp(fc.c_str(),"rts") == 0)  mRTSfc = true;

	// Set port's data
	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &tio);

#ifdef SER_RS485_ENABLED
#ifndef TIOCSRS485
#define TIOCSRS485      0x542f
#endif
	// Standard RS-485 mode
	serial_rs485 rs485conf;
	memset(&rs485conf, 0, sizeof(serial_rs485));
	if(strcasecmp(fc.c_str(),"rs485") == 0)	rs485conf.flags |= SER_RS485_ENABLED;
	ioctl(fd, TIOCSRS485, &rs485conf);
#endif

	//Modem init
	mMdmMode = s2i(TSYS::strSepParse(addr(),4,':'));
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
		    throw TError(nodePath().c_str(),_("No response to initial request '%s'."),mdmInitStr1().c_str());
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
		    throw TError(nodePath().c_str(),_("No response to initial request '%s'."),mdmInitStr2().c_str());
		TSYS::sysSleep(mdmPostInit());
	    }
	}
    }
    catch(TError err) {
	if(fd >= 0) { close(fd); fd = -1; }
	throw;
    }
}

void TTrIn::start( )
{
    if(run_st) return;

    //Status clear
    trIn = trOut = 0;
    tmMax = 0;

    connect();

    //Start listen task
    SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);

    TTransportIn::start();
}

void TTrIn::stop( )
{
    if(!run_st) return;

    if(mMdmMode && mMdmDataMode) mod->devUnLock(mDevPort);

    mMdmMode = mMdmDataMode = false;

    //Status clear
    trIn = trOut = 0;
    tmMax = 0;

    SYS->taskDestroy(nodePath('.',true), &endrun);

    if(fd >= 0) close(fd);
    fd = -1;

    TTransportIn::stop();
}

void *TTrIn::Task( void *tr_in )
{
    TTrIn *tr = (TTrIn*)tr_in;

    tr->run_st	= true;
    tr->endrun	= false;

    AutoHD<TProtocolIn> prot_in;
    struct timeval tv;
    ssize_t r_len = 0;
    string req, answ;
    char buf[1000];
    fd_set rw_fd;
    int kz, sec;

    double wCharTm = s2r(TSYS::strSepParse(tr->timings(),0,':'));
    int wFrTm = 1000*s2i(TSYS::strSepParse(tr->timings(),1,':'));
    int64_t stFrTm = 0, tmW = 0, tmTmp1;

    fcntl(tr->fd, F_SETFL, 0);

    if(tr->mRTSfc) {
	ioctl(tr->fd, TIOCMGET, &sec);
	//Disable transfer for read allow
	sec |= TIOCM_RTS;
	ioctl(tr->fd, TIOCMSET, &sec);
    }

    while(!tr->endrun) {
	// Char timeout
	while(true) {
	    tv.tv_sec = 0; tv.tv_usec = (int)(1.5e3*wCharTm);
	    FD_ZERO(&rw_fd); FD_SET(tr->fd, &rw_fd);
	    if(select(tr->fd+1, &rw_fd, NULL, NULL, &tv) <= 0) {
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
		    try{ tr->connect(); } catch(TError err) { break; }
		    continue;
		}
	    }
	    break;
	}

	tr->trIn += req.size();

	if(mess_lev() == TMess::Debug)
	    mess_debug(tr->nodePath().c_str(), _("Serial received message '%d'."), req.size());

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
	//Send message to protocol
	try {
	    if(prot_in.freeStat()) {
		AutoHD<TProtocol> proto = SYS->protocol().at().modAt(tr->protocol());
		string n_pr = tr->id()+i2s(tr->fd);
		if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, tr, "\n"+i2s(tr->fd));
		prot_in = proto.at().at(n_pr);
	    }
	    prot_in.at().mess(req, answ);
	}
	catch(TError err) {
	    mess_err(tr->nodePath().c_str(),"%s",err.mess.c_str() );
	    mess_err(tr->nodePath().c_str(),_("Error request to protocol."));
	}

	//Send respond
	if(answ.size()) {
	    if(mess_lev() == TMess::Debug)
		mess_debug(tr->nodePath().c_str(), _("Serial replied message '%d'."), answ.size());
	    // Pure RS-485 flow control: Clear RTS for transfer allow
	    if(tr->mRTSfc) { sec &= ~TIOCM_RTS; ioctl(tr->fd, TIOCMSET, &sec); }

	    ssize_t wL = 1;
	    unsigned wOff = 0;
	    for( ; wOff != answ.size() && wL > 0; wOff += wL) {
		wL = write(tr->fd, answ.data()+wOff, answ.size()-wOff);
		if(wL == 0) { mess_err(tr->nodePath().c_str(), _("Write: reply for zero bytes.")); break; }
		else if(wL < 0) {
		    if(errno == EAGAIN) {
			tv.tv_sec = wFrTm/1000000; tv.tv_usec = wFrTm%1000000;
			FD_ZERO(&rw_fd); FD_SET(tr->fd, &rw_fd);
			kz = select(tr->fd+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(tr->fd,&rw_fd)) { wL = 0; continue; }
		    }
		    mess_err(tr->nodePath().c_str(), _("Write: error '%s (%d)'!"), strerror(errno), errno);
		    break;
		}
		tr->trOut += vmax(0,wL);
	    }

	    // Hard read for wait request echo and transfer disable
	    if(tr->mRTSfc && wOff == answ.size()) {
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
		sec |= TIOCM_RTS;
		ioctl(tr->fd, TIOCMSET, &sec);
	    }
	    answ = "";
	}
	req = "";
    }

    //Close protocol
    if(!prot_in.freeStat()) {
	string n_pr = prot_in.at().name();
	AutoHD<TProtocol> proto = AutoHD<TProtocol>(&prot_in.at().owner());
	prot_in.free();
	proto.at().close(n_pr);
    }

    tr->run_st = false;

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
	    _("Serial transport has address format: \"dev:speed:format[:fc[:mdm]]\". Where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600);\n"
	    "    format - asynchronous data format '{size}{parity}{stop}' (8N1, 7E1, 5O2);\n"
	    "    fc - flow control:\n"
	    "      'h' - hardware (CRTSCTS);\n"
	    "      's' - software (IXON|IXOFF);\n"
	    "      'rts' - use RTS signal for transfer(false) and check for echo, for pure RS-485;\n"
	    "      'RS485' - use RS-485 mode, by TIOCSRS485.\n"
	    "    mdm - modem mode, listen for 'RING'."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID);
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"symbol:frm\". Where:\n"
	    "    symbol - one symbol maximum time, used for frame end detection, in ms;\n"
	    "    frm - maximum frame length, in ms."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/taskPrior",_("Priority"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,4,
	    "tp","dec","min","-1","max","99","help",TMess::labTaskPrior());
	if(s2i(TSYS::strParse(addr(),4,":")) && ctrMkNode("area",opt,-1,"/mod",_("Modem"),R_R_R_,"root",STR_ID))
	{
	    ctrMkNode("fld",opt,-1,"/mod/tm",_("Timeout (sec)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/preInitDl",_("Pre-initial delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/postInitDl",_("Post-initial delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/initStr1",_("Initialization string 1"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initStr2",_("Initialization string 2"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/initResp",_("Initial response"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringReq",_("Ring request"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringAnswer",_("Ring answer"),RWRWR_,"root",STR_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/mod/ringAnswerResp",_("Ring answer response"),RWRWR_,"root",STR_ID,1,"tp","str");
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
//* TTrOut                                   *
//************************************************
TTrOut::TTrOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), fd(-1), mLstReqTm(0), mKeepAliveLstTm(0), trIn(0), trOut(0),
    mMdmTm(30), mMdmLifeTime(30), mMdmPreInit(0.5), mMdmPostInit(1), mMdmInitStr1("ATZ"), mMdmInitStr2(""), mMdmInitResp("OK"),
    mMdmDialStr("ATDT"), mMdmCnctResp("CONNECT"), mMdmBusyResp("BUSY"), mMdmNoCarResp("NO CARRIER"), mMdmNoDialToneResp("NO DIALTONE"),
    mMdmExit("+++"), mMdmHangUp("+++ATH"), mMdmHangUpResp("OK"), mMdmMode(false), mMdmDataMode(false), mRTSfc(false)
{
    setAddr("/dev/ttyS0:19200:8E2");
    setTimings("640:6");
}

TTrOut::~TTrOut( )	{ }

void TTrOut::load_( )
{
    TTransportOut::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("TMS");		if(!vl.empty()) setTimings(vl);
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
    } catch(...){ }
}

void TTrOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("TMS", timings());
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

    if(startStat())
	rez += TSYS::strMess(_("Traffic in %s, out %s. "),TSYS::cpct2str(trIn).c_str(),TSYS::cpct2str(trOut).c_str());

    return rez;
}

bool TTrOut::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR") {
	//Times adjust
	int speed = s2i(TSYS::strSepParse(co.getS(),1,':'));
	if(TSYS::strSepParse(addr(),4,':').size()) setTimings("5000:1000");
	else if(speed) setTimings(i2s((1024*11*1000)/speed)+":"+r2s(11e4/(float)speed,2,'f'));
    }

    return TTransportOut::cfgChange(co, pc);
}

void TTrOut::setTimings( const string &vl )
{
    int wReqTm = vmax(1,vmin(10000,s2i(TSYS::strSepParse(vl,0,':'))));
    double wCharTm = vmax(0.01,vmin(1e3,s2r(TSYS::strSepParse(vl,1,':'))));
    double wKeepAliveTm = vmax(0,vmin(1e3,s2r(TSYS::strSepParse(vl,2,':'))));
    mTimings = TSYS::strMess(wKeepAliveTm?"%d:%g:%g":"%d:%g",wReqTm,wCharTm,wKeepAliveTm);

    modif();
}

void TTrOut::start( int tmCon )
{
    ResAlloc res(nodeRes(), true);
    if(run_st) return;

    //Status clear
    trIn = trOut = 0;
    bool isLock = false;

    try {
	//Open and setup device
	mDevPort = TSYS::strSepParse(addr(),0,':');
	// Lock device for all serial transports
	if(!(isLock=mod->devLock(mDevPort))) throw TError(nodePath().c_str(),_("Device '%s' is used now."),mDevPort.c_str());

	// Serial port open
	//  O_NONBLOCK is used for prevent function open() hang on several USB->RS485 converters
	fd = open(mDevPort.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) throw TError(nodePath().c_str(),_("Serial port '%s' open error: %s."),mDevPort.c_str(),strerror(errno));

	// Set serial port parameters
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag |= (CREAD|CLOCAL);
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;		///< inter-character timer unused
	tio.c_cc[VMIN] = 0;		///< blocking read until 0 character arrives

	// Set speed
	string speed = TSYS::strNoSpace(TSYS::strSepParse(addr(),1,':').c_str());
	if(!speed.empty()) {
	    speed_t tspd = B9600;
	    switch(s2i(speed))
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

	// Set asynchronous data format
	string format = TSYS::strNoSpace(TSYS::strNoSpace(TSYS::strSepParse(addr(),2,':')));
	if(!format.empty()) {
	    if(format.size() != 3) throw TError(nodePath().c_str(),_("Asynchronous data format '%s' error."),format.c_str());

	    //  Set byte length
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

	    //  Set parity
	    char parity = tolower(format[1]);
	    switch(parity)
	    {
		case 'e': tio.c_cflag |= PARENB; tio.c_cflag &= ~PARODD;break;
		case 'o': tio.c_cflag |= PARENB; tio.c_cflag |= PARODD;	break;
		case 'n': tio.c_cflag &= ~PARENB;			break;
		default: throw TError(nodePath().c_str(),_("Parity checking mode '%c' error."),parity);
	    }

	    //  Set stop bits number
	    int stopbt = format[2]-'0';
	    if(stopbt == 1) tio.c_cflag &= ~CSTOPB;
	    else if(stopbt == 2) tio.c_cflag |= CSTOPB;
	    else throw TError(nodePath().c_str(),_("Stop bits '%d' error."),stopbt);
	}

	// Set flow control
	string fc = TSYS::strNoSpace(TSYS::strSepParse(addr(),3,':'));
	mRTSfc = false;
	tio.c_cflag &= ~CRTSCTS;
	if(strcasecmp(fc.c_str(),"h") == 0)		tio.c_cflag |= CRTSCTS;
	else if(strcasecmp(fc.c_str(),"s") == 0)	tio.c_iflag |= (IXON|IXOFF|IXANY);
	else if(strcasecmp(fc.c_str(),"rts") == 0)	mRTSfc = true;

	// Set port's data
	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &tio);

#ifdef SER_RS485_ENABLED
#ifndef TIOCSRS485
#define TIOCSRS485      0x542f
#endif
	// Standard RS-485 mode
	serial_rs485 rs485conf;
	memset(&rs485conf, 0, sizeof(serial_rs485));
	if(strcasecmp(fc.c_str(),"rs485") == 0)	rs485conf.flags |= SER_RS485_ENABLED;
	ioctl(fd, TIOCSRS485, &rs485conf);
#endif

	//Modem connection establish
	string telNumb = TSYS::strNoSpace(TSYS::strSepParse(addr(),4,':'));
	if(!telNumb.empty()) {
	    // Resource to transfer function alloc
	    run_st = true;
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
		    throw TError(nodePath().c_str(),_("No response to initial request '%s'."),mdmInitStr1().c_str());
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
		    throw TError(nodePath().c_str(),_("No response to initial request '%s'."),mdmInitStr2().c_str());
		TSYS::sysSleep(mdmPostInit());
	    }
	    // Dial number and connection wait
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
    catch(TError err) {
	//HangUp
	if(mMdmMode) TTr::writeLine(fd,mdmHangUp());

	if(fd >= 0) { close(fd); fd = -1; }
	if(isLock) mod->devUnLock(mDevPort);
	run_st = false;
	mMdmMode = false;
	throw;
    }

    mKeepAliveLstTm = TSYS::curTime();
    run_st = true;

    TTransportOut::start();
}

void TTrOut::stop( )
{
    ResAlloc res(nodeRes(), true);
    if(!run_st) return;

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

    run_st = false;
    mMdmMode = false;

    TTransportOut::stop();
}

void TTrOut::check( )
{
    bool reRs = false;
    bool toStop = (mMdmMode && mMdmDataMode && (reRs=nodeRes().resTryW()) && (TSYS::curTime()-mLstReqTm)/1000000 > mdmLifeTime());
    if(reRs) nodeRes().resRelease();
    if(toStop) stop();
}

int TTrOut::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time, bool noRes )
{
    ssize_t blen = 0;
    int off = 0, kz, sec;
    fd_set rw_fd;
    struct timeval tv;

    if(!noRes) ResAlloc res(nodeRes(), true);

    if(!run_st) throw TError(nodePath().c_str(),_("Transport is not started!"));

    int wReqTm = s2i(TSYS::strSepParse(timings(),0,':',&off));
    wReqTm = time ? time : wReqTm;
    double wCharTm = s2r(TSYS::strSepParse(timings(),0,':',&off));
    double wKeepAliveTm = s2r(TSYS::strSepParse(timings(),0,':',&off));
    if(wKeepAliveTm && (TSYS::curTime()-mKeepAliveLstTm) > wKeepAliveTm*1000000)
    {
	stop();
	start();
	mKeepAliveLstTm = TSYS::curTime();
    }

    int64_t tmW = TSYS::curTime();

    if(mRTSfc)	ioctl(fd, TIOCMGET, &sec);

    //Write request
    if(obuf && len_ob > 0) {
	tcflush(fd, TCIOFLUSH);
	if((tmW-mLstReqTm) < (4000*wCharTm)) kz = TSYS::sysSleep(1e-6*((4e3*wCharTm)-(tmW-mLstReqTm)));

	// Pure RS-485 flow control: Clear RTS for transfer allow
	if(mRTSfc) { sec &= ~TIOCM_RTS; ioctl(fd, TIOCMSET, &sec); }

	for(int wOff = 0; wOff != len_ob; wOff += kz) {
	    kz = write(fd, obuf+wOff, len_ob-wOff);
	    if(kz <= 0) {
		if(errno == EAGAIN) {
		    tv.tv_sec = wReqTm/1000; tv.tv_usec = 1000*(wReqTm%1000);
		    FD_ZERO(&rw_fd); FD_SET(fd, &rw_fd);
		    kz = select(fd+1, NULL, &rw_fd, NULL, &tv);
		    if(kz > 0 && FD_ISSET(fd,&rw_fd)) { kz = 0; continue; }
		}
		mLstReqTm = TSYS::curTime();
		stop();
		throw TError(nodePath().c_str(),_("Writing request error."));
	    }
	    else trOut += kz;
	}

	// Hard read for wait request echo and transfer disable
	if(mRTSfc) {
	    char echoBuf[255];
	    mLstReqTm = TSYS::curTime();
	    for(int r_off = 0; r_off < len_ob; ) {
		kz = read(fd, echoBuf, vmin(len_ob-r_off,(int)sizeof(echoBuf)));
		if(kz == 0 || (kz == -1 && errno == EAGAIN)) {
		    if((TSYS::curTime()-mLstReqTm) > wCharTm*len_ob*1e3) throw TError(nodePath().c_str(),_("Timeouted!"));
		    sched_yield();
		    continue;
		}
		if(kz < 0 || memcmp(echoBuf,obuf+r_off,kz) != 0) throw TError(nodePath().c_str(),_("Echo request reading error."));
		r_off += kz;
	    }
	    sec |= TIOCM_RTS;
	    ioctl(fd, TIOCMSET, &sec);
	}
    }

    //> Read reply
    if(ibuf != NULL && len_ib > 0) {
	if(obuf && len_ob > 0) { tv.tv_sec  = wReqTm/1000; tv.tv_usec = 1000*(wReqTm%1000); }
	else { tv.tv_sec = (int)(1.5e-3*wCharTm); tv.tv_usec = (int)(1.5e3*wCharTm)%1000000; }
	FD_ZERO(&rw_fd); FD_SET(fd, &rw_fd);
	kz = select(fd+1, &rw_fd, NULL, NULL, &tv);
	if(kz == 0)	{ mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(),_("Timeouted!")); }
	else if(kz < 0)	{ mLstReqTm = TSYS::curTime(); stop(); throw TError(nodePath().c_str(),_("Serial error!")); }
	else if(FD_ISSET(fd,&rw_fd)) {
	    blen = read(fd, ibuf, len_ib);
	    trIn += vmax(0, blen);
	}
    }
    mLstReqTm = TSYS::curTime();

    return vmax(0,blen);
}

TVariant TTrOut::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // bool TS(bool rts = EVAL) - To Send control by set request <rts> and return Clear CTS state
    //  rts - Request value RTS
    if(iid == "TS") {
	ResAlloc res(nodeRes(), true);
	if(!run_st) return EVAL_BOOL;
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
	ResAlloc res(nodeRes(), true);
	if(!run_st) return EVAL_BOOL;
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
	ResAlloc res(nodeRes(), true);
	if(!run_st) return EVAL_BOOL;
	int tiocm;
	//Get TIOCM current status
	ioctl(fd, TIOCMGET, &tiocm);
	return (bool)(tiocm&TIOCM_CD);
    }
    // bool RI() - Ring Indicator control
    if(iid == "RI") {
	ResAlloc res(nodeRes(), true);
	if(!run_st) return EVAL_BOOL;
	int tiocm;
	//Get TIOCM current status
	ioctl(fd, TIOCMGET, &tiocm);
	return (bool)(tiocm&TIOCM_RI);
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
	    "dest","sel_ed","select","/prm/cfg/devLS","help",
	    _("Serial transport has address format: \"dev:speed:format[:fc[:modTel]]\". Where:\n"
	    "    dev - serial device address (/dev/ttyS0);\n"
	    "    speed - device speed (300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,\n"
	    "                          230400, 460800, 500000, 576000 or 921600 );\n"
	    "    format - asynchronous data format '{size}{parity}{stop}' (8N1, 7E1, 5O2);\n"
	    "    fc - flow control:\n"
	    "      'h' - hardware (CRTSCTS);\n"
	    "      's' - software (IXON|IXOFF);\n"
	    "      'rts' - use RTS signal for transfer(false) and check for echo, for pure RS-485;\n"
	    "      'RS485' - use RS-485 mode, by TIOCSRS485.\n"
	    "    modTel - modem telephone, the field presence do switch transport to work with modem mode."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"conn:symbol[:KeepAliveTm]\". Where:\n"
	    "    conn - maximum time for connection respond wait, in ms;\n"
	    "    symbol - one symbol maximum time, used for frame end detection, in ms;\n"
	    "    KeepAliveTm - keep alive timeout in seconds for restart transport."));
	if(TSYS::strParse(addr(),4,":").size() && ctrMkNode("area",opt,-1,"/mod",_("Modem"),R_R_R_,"root",STR_ID))
	{
	    ctrMkNode("fld",opt,-1,"/mod/tm",_("Timeout (sec)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/lifeTm",_("Life time (sec)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/mod/preInitDl",_("Pre-initial delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/mod/postInitDl",_("Post-initial delay (sec)"),RWRWR_,"root",STR_ID,1,"tp","real");
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


//OpenSCADA system module Transport.Sockets file: socket.cpp
/***************************************************************************
 *   Copyright (C) 2003-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <errno.h>

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>
#include "socket.h"

#ifdef HAVE_LINUX_CAN_H
# include <linux/can.h>
# include <linux/can/raw.h>

# ifndef PF_CAN
#  define PF_CAN 29
# endif

# ifndef AF_CAN
#  define AF_CAN PF_CAN
# endif
#endif


//************************************************
//* Modul info!                                  *
#define MOD_ID		"Sockets"
#define MOD_NAME	_("Sockets")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"2.6.0"
#define AUTHORS		_("Roman Savochenko, Maxim Kochetkov")
#define DESCRIPTION	_("Provides sockets based transport. Support inet and unix sockets. Inet socket uses TCP, UDP and RAWCAN protocols.")
#define LICENSE		"GPL2"
//************************************************

Sockets::TTransSock *Sockets::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt tr_Sockets_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *tr_Sockets_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new Sockets::TTransSock(source);
	return NULL;
    }
}

using namespace Sockets;

//************************************************
//* TTransSock					 *
//************************************************
TTransSock::TTransSock( string name ) : TTypeTransport(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTransSock::~TTransSock( )	{ }

void TTransSock::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	owner().inEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
	owner().outEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
    }
}

void TTransSock::load_( )
{
    //Load parameters from command line

}

void TTransSock::perSYSCall( unsigned int cnt )
{
    vector<string> trls;

    //Iniciative input protocols check for restart/reconnect need.
    inList(trls);
    for(unsigned iTr = 0; !SYS->stopSignal() && iTr < trls.size(); iTr++)
	((AutoHD<TSocketIn>)inAt(trls[iTr])).at().check();
}

TTransportIn *TTransSock::In( const string &name, const string &idb )	{ return new TSocketIn(name, idb, &owner().inEl()); }

TTransportOut *TTransSock::Out( const string &name, const string &idb )	{ return new TSocketOut(name, idb, &owner().outEl()); }

//************************************************
//* TSocketIn                                    *
//************************************************
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), sockRes(true), wrToRes(true), mMode(0), mMSS(0), mMaxQueue(10), mMaxFork(20), mMaxForkPerHost(0), mBufLen(5),
    mKeepAliveReqs(0), mKeepAliveTm(60), mTaskPrior(0), clFree(true)
{
    setAddr("TCP:localhost:10002:0");
}

TSocketIn::~TSocketIn( )
{

}

string TSocketIn::getStatus( )
{
    string rez = TTransportIn::getStatus();

    if(startStat()) {
	rez += TSYS::strMess(_("Connections %d, opened %d, last %s. Traffic in %s, out %s. Closed connections by limit %d. "),
				connNumb, (protocol().empty()?assTrs().size():clId.size()), atm2s(lastConn()).c_str(),
				TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str(), clsConnByLim);
	if(mess_lev() == TMess::Debug)
	    rez += TSYS::strMess(_("Processing time %s[%s]. "), tm2s(1e-6*prcTm).c_str(), tm2s(1e-6*prcTmMax).c_str());
    }

    return rez;
}

void TSocketIn::load_( )
{
    //TTransportIn::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("MSS");		if(!vl.empty()) setMSS(s2i(vl));
	vl = prmNd.attr("MaxQueue");	if(!vl.empty()) setMaxQueue(s2i(vl));
	vl = prmNd.attr("MaxClients");	if(!vl.empty()) setMaxFork(s2i(vl));
	vl = prmNd.attr("MaxClientsPerHost");	if(!vl.empty()) setMaxForkPerHost(s2i(vl));
	vl = prmNd.attr("BufLen");	if(!vl.empty()) setBufLen(s2i(vl));
	vl = prmNd.attr("KeepAliveReqs");	if(!vl.empty()) setKeepAliveReqs(s2i(vl));
	vl = prmNd.attr("KeepAliveTm");	if(!vl.empty()) setKeepAliveTm(s2i(vl));
	vl = prmNd.attr("TaskPrior");	if(!vl.empty()) setTaskPrior(s2i(vl));
    } catch(...) { }
}

void TSocketIn::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("MSS", i2s(MSS()));
    prmNd.setAttr("MaxQueue", i2s(maxQueue()));
    prmNd.setAttr("MaxClients", i2s(maxFork()));
    prmNd.setAttr("MaxClientsPerHost", i2s(maxForkPerHost()));
    prmNd.setAttr("BufLen", i2s(bufLen()));
    prmNd.setAttr("KeepAliveReqs", i2s(keepAliveReqs()));
    prmNd.setAttr("KeepAliveTm", i2s(keepAliveTm()));
    prmNd.setAttr("TaskPrior", i2s(taskPrior()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportIn::save_();
}

void TSocketIn::start( )
{
    if(runSt) return;

    //Status clear
    trIn = trOut = prcTm = prcTmMax = clntDetchCnt = 0;
    connNumb = clsConnByLim = 0;

    //Socket init
    string s_type = TSYS::strSepParse(addr(), 0, ':');

    if(s_type == S_NM_TCP) {
	if((sockFd=socket(PF_INET,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Create %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	int vl = 1; setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
	if(MSS()) { vl = MSS(); setsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, &vl, sizeof(int)); }
	type = SOCK_TCP;
    }
    else if(s_type == S_NM_UDP) {
	if((sockFd=socket(PF_INET,SOCK_DGRAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Create %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	type = SOCK_UDP;
    }
    else if(s_type == S_NM_UNIX) {
	if((sockFd=socket(PF_UNIX,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Create %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	type = SOCK_UNIX;
    }
#ifdef HAVE_LINUX_CAN_H
    else if(s_type == S_NM_RAWCAN) {
	if((sockFd=socket(PF_CAN,SOCK_RAW,CAN_RAW)) == -1)
	    throw TError(nodePath().c_str(), _("Create %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	type = SOCK_RAWCAN;
    }
#endif
    else throw TError(nodePath().c_str(), _("Socket type '%s' error!"), s_type.c_str());

    if(type == SOCK_TCP || type == SOCK_UDP) {
	struct sockaddr_in  nameIn;
	memset(&nameIn, 0, sizeof(nameIn));
	nameIn.sin_family = AF_INET;
	char hBuf[STR_BUF_LEN], sBuf[256];
	struct hostent hostbuf, *hp;
	struct servent servbuf, *sp;

	host	= TSYS::strSepParse(addr(), 1, ':');
	port	= TSYS::strSepParse(addr(), 2, ':');
	if(host.size()) {
	    int herr;
	    gethostbyname_r(host.c_str(), &hostbuf, hBuf, sizeof(hBuf), &hp, &herr);
	    if(!hp) switch(herr) {
		case HOST_NOT_FOUND:
		    throw TError(nodePath().c_str(), _("Host '%s' not found!"), host.c_str());
		case NO_ADDRESS:
		    throw TError(nodePath().c_str(), _("The requested name '%s' does not have an IP address!"), host.c_str());
		case NO_RECOVERY:
		    throw TError(nodePath().c_str(), _("A non-recoverable name server error occurred while for '%s'!"), host.c_str());
		case TRY_AGAIN:
		    throw TError(nodePath().c_str(), _("A temporary error occurred on an authoritative name server for '%s'!"), host.c_str());
		default:
		    throw TError(nodePath().c_str(), _("Unknown error code from gethostbyname_r for '%s'!"), host.c_str());
	    }
	    nameIn.sin_addr.s_addr = *((int*)(hp->h_addr_list[0]));	//!!!! Append all IP list processing
	}
	else nameIn.sin_addr.s_addr = INADDR_ANY;
	if(type == SOCK_TCP) {
	    //Get system port for "oscada" /etc/services
#if defined(__ANDROID__)
	    struct servent *sptr = getservbyname(port.c_str(), "tcp");
	    if(sptr)				nameIn.sin_port = sptr->s_port;
#else
	    if(getservbyname_r(port.c_str(),"tcp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
		nameIn.sin_port = sp->s_port;
#endif
	    else if(htons(s2i(port)) > 0)	nameIn.sin_port = htons(s2i(port));
	    else nameIn.sin_port = 10005;

	    if(mode() == 2) {							//Initiate connection
		int flags = fcntl(sockFd, F_GETFL, 0);
		fcntl(sockFd, F_SETFL, flags|O_NONBLOCK);
		int rez = connect(sockFd, (sockaddr*)&nameIn, sizeof(nameIn));
		if(rez == -1 && errno == EINPROGRESS) {
		    struct timeval tv;
		    socklen_t slen = sizeof(rez);
		    fd_set fdset;
		    tv.tv_sec = 5; tv.tv_usec = 0;
		    FD_ZERO(&fdset); FD_SET(sockFd, &fdset);
		    if((rez=select(sockFd+1,NULL,&fdset,NULL,&tv)) > 0 && !getsockopt(sockFd,SOL_SOCKET,SO_ERROR,&rez,&slen) && !rez) rez = 0;
		    else rez = -1;
		}
		if(rez) {
		    close(sockFd);
		    sockFd = -1;
		    throw TError(nodePath().c_str(), _("Connect to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
		}
	    }
	    else if(bind(sockFd,(sockaddr*)&nameIn,sizeof(nameIn)) == -1) {	//Wait connection
		int rez = errno;
		shutdown(sockFd, SHUT_RDWR);
		close(sockFd);
		throw TError(nodePath().c_str(), _("Bind to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
	    }
	    listen(sockFd, maxQueue());
	}
	else if(type == SOCK_UDP) {
	    //Get system port for "oscada" /etc/services
#if defined(__ANDROID__)
	    struct servent *sptr = getservbyname(port.c_str(), "udp");
	    if(sptr)				nameIn.sin_port = sptr->s_port;
#else
	    if(getservbyname_r(port.c_str(),"udp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
		nameIn.sin_port = sp->s_port;
#endif
	    else if(htons(s2i(port)) > 0)	nameIn.sin_port = htons(s2i(port));
	    else nameIn.sin_port = 10005;

	    if(bind(sockFd,(sockaddr*)&nameIn,sizeof(nameIn)) == -1) {
		int rez = errno;
		shutdown(sockFd, SHUT_RDWR);
		close(sockFd);
		throw TError(nodePath().c_str(), _("Bind to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
	    }
	}
    }
    else if(type == SOCK_UNIX) {
	path	= TSYS::strSepParse(addr(), 1, ':');
	if(!path.size()) path = "/tmp/oscada";
	remove(path.c_str());
	struct sockaddr_un  nameUn;
	memset(&nameUn, 0, sizeof(nameUn));
	nameUn.sun_family = AF_UNIX;
	strncpy(nameUn.sun_path, path.c_str(), sizeof(nameUn.sun_path));
	if(mode() == 2) {
	    if(connect(sockFd,(sockaddr*)&nameUn,sizeof(nameUn)) == -1) {
		close(sockFd);
		sockFd = -1;
		throw TError(nodePath().c_str(), _("Connect to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	    }
	}
	else if(bind(sockFd,(sockaddr*)&nameUn,sizeof(nameUn)) == -1) {
	    int rez = errno;
	    close(sockFd);
	    throw TError(nodePath().c_str(),_("Bind to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
	}
	listen(sockFd, maxQueue());
    }
#ifdef HAVE_LINUX_CAN_H
    else if(type == SOCK_RAWCAN) {
	path	= TSYS::strSepParse(addr(), 1, ':');
	struct can_filter rfilter;
	rfilter.can_id = strtoul(TSYS::strSepParse(addr(),2,':').c_str(), NULL, 0);
	rfilter.can_mask = strtoul(TSYS::strSepParse(addr(),3,':').c_str(), NULL, 0);
	setsockopt(sockFd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	if(!path.size()) path = "can0";
	struct ifreq ifr;
	strcpy(ifr.ifr_name, path.c_str());
	ioctl(sockFd, SIOCGIFINDEX, &ifr);
	struct sockaddr_can name_can;
	name_can.can_family = AF_CAN;
	name_can.can_ifindex = ifr.ifr_ifindex;
	if(bind(sockFd,(struct sockaddr*)&name_can,sizeof(name_can)) == -1) {
	    close(sockFd);
	    throw TError(nodePath().c_str(), _("Bind to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	}
	else if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("RAWCAN socket binded '%s'!"), addr().c_str());
    }
#endif

    if(mode() == 2) {	//client task create for initiative connection
	SSockIn *sin = new SSockIn(this, sockFd, addr());
	try {
	    endrunCl = false;
	    SYS->taskCreate(nodePath('.',true)+"."+i2s(sockFd), taskPrior(), ClTask, sin);
	    connNumb++;
	} catch(TError &err) { close(sockFd); delete sin; throw; }
    }
    else SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);	//main task for processing or client task create
    runSt = true;

    TTransportIn::start();

    if(logLen()) pushLogMess(_("Started"));
}

void TSocketIn::stop( )
{
    if(!runSt) return;

    //Status clear
    trIn = trOut = 0;
    connNumb = connTm = clsConnByLim = 0;

    if(mode() == 2) SYS->taskDestroy(nodePath('.',true)+"."+i2s(sockFd), &endrunCl);
    else SYS->taskDestroy(nodePath('.',true), &endrun);
    runSt = false;

    shutdown(sockFd, SHUT_RDWR);
    close(sockFd);
    if(type == SOCK_UNIX) remove(path.c_str());

    TTransportIn::stop();

    if(logLen()) pushLogMess(_("Stopped"));
}

void TSocketIn::check( )
{
    try {
	//Check for activity for initiative mode
	if(mode() == 2 && (toStart() || startStat()) && (!startStat() || time(NULL) > (lastConn()+keepAliveTm()))) {
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Restart by no ingoing activity to '%s'."), addr().c_str());
	    if(startStat()) stop();
	    start();
	}

	//Check the assigned to that output transports for inactivity
	/*vector<AutoHD<TTransportOut> > aTrLs = assTrs(true);
	for(unsigned iTr = 0; iTr < aTrLs.size(); iTr++) {
	    if(!aTrLs[iTr].at().startStat() || aTrLs[iTr].at().addr().compare(0,5,"SOCK:") != 0) continue;
	    int oSockFd = s2i(TSYS::strSepParse(aTrLs[iTr].at().addr(),1,':'));
	    //struct sockaddr aNm;
	    //socklen_t aLn = sizeof(aNm);
	    //int fRes = getpeername(oSockFd, &aNm, &aLn);

	    int error = 0;
	    socklen_t slen = sizeof(error);
	    int fRes = getsockopt(oSockFd, SOL_SOCKET, SO_ERROR, &error, &slen);
	    printf("TEST 01: fRes=%d; error=%d\n", fRes, error);
	}*/
    } catch(...) { }
}

int TSocketIn::writeTo( const string &sender, const string &data )
{
    fd_set		rw_fd;
    struct timeval	tv;

    MtxAlloc resN(wrToRes, true);	//Prevent simultaneous writing

    switch(type) {
	case SOCK_TCP: case SOCK_UNIX: {
	    int sId = s2i(TSYS::strLine(sender,1));
	    if(sId < 0) return -1;
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(),_("Write: wrote %s."), TSYS::cpct2str(data.size()).c_str());
	    ssize_t wL = 1;
	    unsigned wOff = 0;
	    for( ; wOff != data.size() && wL > 0; wOff += wL) {
		wL = write(sId, data.data()+wOff, data.size()-wOff);
		if(wL == 0) { mess_err(nodePath().c_str(), _("Write: reply for zero bytes.")); break; }
		else if(wL < 0) {
		    if(errno == EAGAIN) {
			tv.tv_sec = 1; tv.tv_usec = 0;		//!!!! Where the time get?
			FD_ZERO(&rw_fd); FD_SET(sId, &rw_fd);
			int kz = select(sId+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(sId,&rw_fd)) { wL = 0; continue; }
			//???? Maybe some flush ????
		    }
		    mess_err(nodePath().c_str(), _("Write: error '%s (%d)'!"), strerror(errno), errno);
		    break;
		}

		//Counters
		dataRes().lock();
		trOut += vmax(0, wL);
		dataRes().unlock();
		sockRes.lock();
		map<int, SSockIn*>::iterator cI = clId.find(sId);
		if(cI != clId.end()) cI->second->trOut += vmax(0, wL);
		sockRes.unlock();
		if(wL > 0 && logLen()) pushLogMess(TSYS::strMess(_("%d:Transmitted directly to '%s'\n"),sId,TSYS::strLine(sender,0).c_str()) + string(data.data()+wOff,wL));
	    }
	    return wOff;
	}
    }

    return 0;
}

unsigned TSocketIn::forksPerHost( const string &sender )
{
    sockRes.lock();
    unsigned rez = clS[sender];
    sockRes.unlock();

    return rez;
}

bool TSocketIn::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR" && co.getS() != pc.getS()) {
	mMode = 0;
	if(co.getS().compare(0,4,"TCP:") == 0)		mMode = s2i(TSYS::strParse(co.getS(),3,":"));
	else if(co.getS().compare(0,5,"UNIX:") == 0)	mMode = s2i(TSYS::strParse(co.getS(),2,":"));
    }

    return TTransportIn::cfgChange(co, pc);
}

void *TSocketIn::Task( void *sock_in )
{
    char		*buf = NULL;
    fd_set		rd_fd;
    struct timeval	tv;
    TSocketIn *sock = (TSocketIn*)sock_in;
    AutoHD<TProtocolIn> prot_in;

    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setdetachstate(&pthr_attr, PTHREAD_CREATE_DETACHED);

    sock->runSt	= true;
    sock->endrunCl	= false;
    sock->endrun	= false;

    if(sock->type == SOCK_UDP) buf = new char[sock->bufLen()*1024 + 1];

    while(!sock->endrun) {
	tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	FD_ZERO(&rd_fd); FD_SET(sock->sockFd,&rd_fd);

	int kz = select(sock->sockFd+1, &rd_fd, NULL, NULL, &tv);
	if(kz < 0 && errno != EINTR) {
	    mess_err(sock->nodePath().c_str(), _("Close input transport by error: '%s (%d)'"), strerror(errno), errno);
	    break;
	}
	if(kz <= 0 || !FD_ISSET(sock->sockFd, &rd_fd)) continue;

	struct sockaddr_in name_cl;
	socklen_t	   name_cl_len = sizeof(name_cl);
	if(sock->type == SOCK_TCP) {
	    int sockFdCL = accept(sock->sockFd, (sockaddr *)&name_cl, &name_cl_len);
	    if(sockFdCL != -1) {
		string sender = inet_ntoa(name_cl.sin_addr);
		if(sock->clId.size() >= sock->maxFork() || (sock->maxForkPerHost() && sock->forksPerHost(sender) >= sock->maxForkPerHost())) {
		    sock->clsConnByLim++;
		    close(sockFdCL);
		    continue;
		}
		//Create presenting the client connection output transport
		if(sock->protocol().empty() && sock->assTrs(true).size() <= sock->maxFork()) {
		    sock->assTrO("SOCK:"+i2s(sockFdCL));
		    sock->connNumb++;
		    continue;
		}

		//Thread create for input requests process.
		SSockIn *sin = new SSockIn(sock, sockFdCL, sender);
		try {
		    SYS->taskCreate(sock->nodePath('.',true)+"."+i2s(sockFdCL), sock->taskPrior(), ClTask, sin, 5, &pthr_attr);
		    sock->connNumb++;
		} catch(TError &err) {
		    delete sin;
		    mess_err(err.cat.c_str(), err.mess.c_str());
		    mess_err(sock->nodePath().c_str(), _("Error creation of the thread!"));
		}
	    }
	}
	else if(sock->type == SOCK_UNIX) {
	    int sockFdCL = accept(sock->sockFd, NULL, NULL);
	    if(sockFdCL != -1) {
		if(sock->clId.size() >= sock->maxFork()) {
		    sock->clsConnByLim++;
		    close(sockFdCL);
		    continue;
		}
		SSockIn *sin = new SSockIn(sock, sockFdCL, "");
		try {
		    SYS->taskCreate(sock->nodePath('.',true)+"."+i2s(sockFdCL), sock->taskPrior(), ClTask, sin, 5, &pthr_attr);
		    sock->connNumb++;
		} catch(TError &err) {
		    delete sin;
		    mess_err(err.cat.c_str(), err.mess.c_str());
		    mess_err(sock->nodePath().c_str(), _("Error creation of the thread!"));
		}
	    }
	}
	else if(sock->type == SOCK_UDP) {
	    string req, answ;

	    ssize_t r_len = recvfrom(sock->sockFd, buf, sock->bufLen()*1000, 0, (sockaddr*)&name_cl, &name_cl_len);
	    if(r_len <= 0) continue;
	    sock->trIn += r_len;
	    req.assign(buf, r_len);

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Read datagram %s from '%s'!"), TSYS::cpct2str(r_len).c_str(), inet_ntoa(name_cl.sin_addr));

	    int64_t stTm = 0;
	    if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
	    sock->messPut(sock->sockFd, req, answ, inet_ntoa(name_cl.sin_addr), prot_in);
	    if(mess_lev() == TMess::Debug) {
		sock->prcTm = SYS->curTime() - stTm;
		sock->prcTmMax = vmax(sock->prcTmMax, sock->prcTm);
	    }
	    if(!prot_in.freeStat()) continue;

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Wrote datagram %s to '%s'!"), TSYS::cpct2str(answ.size()).c_str(), inet_ntoa(name_cl.sin_addr));

	    r_len = sendto(sock->sockFd, answ.c_str(), answ.size(), 0, (sockaddr *)&name_cl, name_cl_len);
	    sock->trOut += vmax(0, r_len);
	}
#ifdef HAVE_LINUX_CAN_H
	else if(sock->type == SOCK_RAWCAN) {
	    struct can_frame frame;
	    string req, answ;
	    int r_len = recv(sock->sockFd, &frame, sizeof(frame), 0);
	    if(r_len <= 0) continue;
	    sock->trIn += r_len;
	    req.assign((char*)frame.data, frame.can_dlc);

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Received CAN frame id:%08X; dlc:%d; data:%02X%02X%02X%02X%02X%02X%02X%02X!"),
		    frame.can_id, frame.can_dlc, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4],
		    frame.data[5], frame.data[6], frame.data[7]);

	    int64_t stTm = 0;
	    if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
	    sock->messPut(sock->sockFd, req, answ, u2s(frame.can_id), prot_in);
	    if(mess_lev() == TMess::Debug) {
		sock->prcTm = SYS->curTime() - stTm;
		sock->prcTmMax = vmax(sock->prcTmMax, sock->prcTm);
	    }
	    if(!prot_in.freeStat()) continue;

	    r_len = send(sock->sockFd, answ.c_str(), answ.size(), 0);
	    sock->trOut += vmax(0, r_len);
	}
#endif
    }
    pthread_attr_destroy(&pthr_attr);

    if(sock->type == SOCK_UDP) delete []buf;

    //Client tasks stop command
    sock->endrunCl = true;

    //Find already registry
    MtxAlloc res(sock->sockRes, true);
    for(map<int, SSockIn*>::iterator iId = sock->clId.begin(); iId != sock->clId.end(); ++iId)
	pthread_kill(iId->second->pid, SIGALRM);
    res.unlock();
    TSYS::eventWait(sock->clFree, true, string(MOD_ID)+": "+sock->id()+_(" client task is stopping...."));

    sock->runSt = false;

    return NULL;
}

void *TSocketIn::ClTask( void *s_inf )
{
    SSockIn &s = *(SSockIn*)s_inf;
    s.pid = pthread_self();
    unsigned cnt = 0;		//Requests counter
    int tm = s.s->connTm = time(NULL);	//Last connection time

    if(mess_lev() == TMess::Debug)
	mess_debug(s.s->nodePath().c_str(), _("Has been connected by '%s'!"), s.sender.c_str());

    s.s->clientReg(&s);

    //Client socket process
    struct timeval tv;
    fd_set rw_fd;
    string req, answ;
    char   buf[s.s->bufLen()*1000 + 1];
    AutoHD<TProtocolIn> prot_in;
    bool   sessOk = false;

    try {
	do {
	    tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	    bool poolPrt = s.s->prtInit(prot_in, s.sock, s.sender, true) && prot_in.at().waitReqTm();
	    if(poolPrt) { tv.tv_sec = prot_in.at().waitReqTm()/1000; tv.tv_usec = (prot_in.at().waitReqTm()%1000)*1000; }
	    FD_ZERO(&rw_fd); FD_SET(s.sock, &rw_fd);
	    int kz = select(s.sock+1, &rw_fd, NULL, NULL, &tv);
	    if((kz == 0 && !poolPrt) || (kz == -1 && errno == EINTR) || (kz > 0 && !FD_ISSET(s.sock,&rw_fd))) continue;
	    if(kz < 0) {
		if(mess_lev() == TMess::Debug)
		    mess_debug(s.s->nodePath().c_str(), _("Has been terminated by error: '%s (%d)'"), strerror(errno), errno);
		break;
	    }

	    ssize_t r_len = 0;
	    if(kz && (r_len=read(s.sock, buf, s.s->bufLen()*1000)) <= 0) break;
	    s.s->dataRes().lock();
	    s.s->trIn += r_len; s.trIn += r_len;
	    s.s->dataRes().unlock();

	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("Read message %s from '%s'."), TSYS::cpct2str(r_len).c_str(), s.sender.c_str());
	    req.assign(buf, r_len);
	    if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Received from '%s'\n"),s.sock,s.sender.c_str()) + req);

	    int64_t stTm = 0;
	    if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
	    s.s->messPut(s.sock, req, answ, s.sender, prot_in);
	    if(mess_lev() == TMess::Debug) {
		s.s->dataRes().lock();
		s.prcTm = s.s->prcTm = SYS->curTime() - stTm;
		s.prcTmMax = vmax(s.prcTmMax, s.prcTm);
		s.s->prcTmMax = vmax(s.s->prcTmMax, s.s->prcTm);
		s.s->dataRes().unlock();
	    }

	    if(answ.size()) {
		if(mess_lev() == TMess::Debug)
		    mess_debug(s.s->nodePath().c_str(), _("Wrote message %s to '%s'."), TSYS::cpct2str(answ.size()).c_str(), s.sender.c_str());
		ssize_t wL = 1;
		for(unsigned wOff = 0; wOff != answ.size() && wL > 0; wOff += wL) {
		    wL = write(s.sock, answ.data()+wOff, answ.size()-wOff);
		    if(wL == 0) { mess_err(s.s->nodePath().c_str(), _("Write: reply for zero bytes.")); break; }
		    else if(wL < 0) {
			if(errno == EAGAIN) {
			    tv.tv_sec = 1; tv.tv_usec = 0;		//!!!! Where the time get?
			    FD_ZERO(&rw_fd); FD_SET(s.sock, &rw_fd);
			    kz = select(s.sock+1, NULL, &rw_fd, NULL, &tv);
			    if(kz > 0 && FD_ISSET(s.sock,&rw_fd)) { wL = 0; continue; }
			    //???? May be some flush ????
			}
			string err = TSYS::strMess(_("Write: error '%s (%d)'!"), strerror(errno), errno);
			if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("Transmitting error: %s"),err.c_str()));
			mess_err(s.s->nodePath().c_str(), "%s", err.c_str());
			s.s->dataRes().lock();
			s.s->clntDetchCnt++;
			s.s->dataRes().unlock();
			break;
		    }
		    s.s->dataRes().lock();
		    s.s->trOut += vmax(0, wL); s.trOut += vmax(0, wL);
		    s.s->dataRes().unlock();
		    if(wL > 0 && s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Transmitted to '%s'\n"),s.sock,s.sender.c_str()) + string(answ.data()+wOff,wL));
		}
		answ = "";
	    }
	    cnt++;
	    s.tmReq = s.s->connTm = tm = time(NULL);
	    sessOk = true;
	} while(!s.s->endrunCl && (s.s->mode() == 2 || ((!s.s->keepAliveTm() || (time(NULL)-tm) < s.s->keepAliveTm()) &&
		(!sessOk || ((s.s->mode() == 1 || !prot_in.freeStat()) && (!s.s->keepAliveReqs() || cnt < s.s->keepAliveReqs()))))));

	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Has been disconnected by '%s'!"), s.sender.c_str());
    } catch(TError &err) {
	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Has been terminated by execution: %s"), err.mess.c_str());
	if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Has been terminated by execution: %s"),s.sock,err.mess.c_str()));
    }

    //Close protocol on broken connection
    if(!prot_in.freeStat()) {
	string n_pr = prot_in.at().name();
	AutoHD<TProtocol> proto = AutoHD<TProtocol>(&prot_in.at().owner());
	prot_in.free();
	proto.at().close(n_pr);
    }

    s.s->clientUnreg(&s);

    return NULL;
}

bool TSocketIn::prtInit( AutoHD<TProtocolIn> &prot_in, int sock, const string &sender, bool noex )
{
    if(!prot_in.freeStat()) return true;

    try {
	AutoHD<TProtocol> proto = SYS->protocol().at().modAt(protocol());
	string n_pr = id() + i2s(sock);
	if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, this, sender+"\n"+i2s(sock));
	prot_in = proto.at().at(n_pr);
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("New input protocol's object '%s' created!"), n_pr.c_str());
    } catch(TError &err) {
	if(!noex) throw;
	return false;
    }

    return !prot_in.freeStat();
}

void TSocketIn::messPut( int sock, string &request, string &answer, const string &sender, AutoHD<TProtocolIn> &prot_in )
{
    AutoHD<TProtocol> proto;
    string n_pr;
    try {
	prtInit(prot_in, sock, sender);
	if(prot_in.at().mess(request,answer)) return;
	if(proto.freeStat()) proto = AutoHD<TProtocol>(&prot_in.at().owner());
	n_pr = prot_in.at().name();
	prot_in.free();
	if(proto.at().openStat(n_pr)) proto.at().close(n_pr);
	if(mess_lev() == TMess::Debug)
	    mess_debug(nodePath().c_str(), _("Input protocol's object '%s' closed by self!"), n_pr.c_str());
    } catch(TError &err) {
	if(!prot_in.freeStat()) {
	    if(proto.freeStat()) proto = AutoHD<TProtocol>(&prot_in.at().owner());
	    n_pr = prot_in.at().name();
	}
	prot_in.free();
	if(!proto.freeStat() && proto.at().openStat(n_pr)) proto.at().close(n_pr);

	mess_err(nodePath().c_str(), "%s", err.mess.c_str());
	mess_err(nodePath().c_str(), _("Error request to protocol."));
    }
}

void TSocketIn::clientReg( SSockIn *so )
{
    MtxAlloc res(sockRes, true);

    //Find already registry
    for(map<int,SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId)
	if(iId->second == so) return;

    clId[so->sock] = so;
    clS[so->sender]++;
    clFree = false;

    if(logLen()) pushLogMess(TSYS::strMess(_("A new client %d from '%s' connected"),so->sock,so->sender.c_str()));
}

void TSocketIn::clientUnreg( SSockIn *so )
{
    MtxAlloc res(sockRes, true);
    for(map<int,SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId)
	if(iId->second == so) {
	    if(logLen()) pushLogMess(TSYS::strMess(_("Client %d from '%s' disconnected"),so->sock,so->sender.c_str()));

	    shutdown(iId->first, SHUT_RDWR);
	    close(iId->first);
	    clS[iId->second->sender]--;
	    clId.erase(iId);
	    delete so;
	    clFree = clId.empty();
	    break;
	}
}

void TSocketIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportIn::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/st",_("State")) && protocol().size() && clId.size())
	    ctrMkNode("list", opt, -1, "/prm/st/conns", _("Active connections"), R_R_R_, "root", STR_ID);
	ctrRemoveNode(opt, "/prm/cfg/A_PRMS");
	ctrMkNode("fld", opt, -1, "/prm/cfg/ADDR", EVAL_STR, startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "help",
	    _("Socket's input transport has address format:\n"
	    "  TCP:{addr}:{port}:{mode} - TCP socket:\n"
	    "    addr - address for socket to be opened, empty address opens socket for all interfaces;\n"
	    "    port - network port (/etc/services);\n"
	    "    mode - work mode (0 - break connection; 1 - keep alive; 2 - initiative connection).\n"
	    "  UDP:{addr}:{port} - UDP socket:\n"
	    "    addr - address for socket to be opened, empty address opens socket for all interfaces;\n"
	    "    port - network port (/etc/services).\n"
	    "  RAWCAN:{if}:{mask}:{id} - CAN socket:\n"
	    "    if - interface name;\n"
	    "    mask - CAN frame id mask;\n"
	    "    id - CAN id.\n"
	    "  UNIX:{name}:{mode} - UNIX socket:\n"
	    "    name - UNIX-socket's file name;\n"
	    "    mode - work mode (0 - break connection; 1 - keep alive; 2 - initiative connection)."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/PROT", EVAL_STR, startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1,
	    "help",_("Empty value for the protocol selection switchs the transport to mode\n"
		     "of creation associated output transports for each connection to one."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/bfLn", _("Input buffer (kbyte)"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "tp","dec");
	ctrMkNode("fld", opt, -1, "/prm/cfg/taskPrior", _("Priority"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
	    "tp","dec", "help",TMess::labTaskPrior());
	if(addr().compare(0,4,"TCP:") == 0)
	    ctrMkNode("fld", opt, -1, "/prm/cfg/MSS", _("Maximum segment size (MSS)"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
		"tp","str", "help",_("Set 0 for system MSS."));
	if(addr().compare(0,4,"TCP:") == 0 || addr().compare(0,5,"UNIX:") == 0 || addr().compare(0,7,"RAWCAN:") == 0) {
	    ctrMkNode("fld", opt, -1, "/prm/cfg/qLn", _("Queue length"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "tp","dec");
	    if(mode() != 2) {
		ctrMkNode("fld", opt, -1, "/prm/cfg/clMax", _("Clients maximum"), RWRWR_, "root", STR_ID, 1, "tp","dec");
		if(addr().compare(0,4,"TCP:") == 0)
		    ctrMkNode("fld", opt, -1, "/prm/cfg/clMaxPerHost", _("Clients maximum, per host"), RWRWR_, "root", STR_ID, 2, "tp","dec",
			"help",_("Set to zero for disable that limit."));
		ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveReqs", _("Keep alive requests"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		    "help",_("Close the connection after specified requests.\nZero value for disable (not close ever)."));
	    }
	    ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveTm", _("Keep alive timeout, seconds"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		"help",_("Close the connection after no requests at specified timeout.\nZero value for disable (not close ever)."));
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/conns" && ctrChkNode(opt)) {
	MtxAlloc res(sockRes, true);
	for(map<int,SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId) {
	    string mess = TSYS::strMess(_("%s %d(%s): last %s; traffic in %s, out %s; client detachings %g; "),
		atm2s(iId->second->tmCreate,"%Y-%m-%dT%H:%M:%S").c_str(),iId->first,iId->second->sender.c_str(),
		atm2s(iId->second->tmReq,"%Y-%m-%dT%H:%M:%S").c_str(),
		TSYS::cpct2str(iId->second->trIn).c_str(),TSYS::cpct2str(iId->second->trOut).c_str(),
		iId->second->clntDetchCnt);
	    if(mess_lev() == TMess::Debug)
		mess += TSYS::strMess(_("processing time %s[%s]; "),
		    tm2s(1e-6*iId->second->prcTm).c_str(), tm2s(1e-6*iId->second->prcTmMax).c_str());
	    opt->childAdd("el")->setText(mess);
	}
    }
    else if(a_path == "/prm/cfg/MSS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(MSS()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMSS(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/qLn") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxQueue()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxQueue(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/clMax") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxFork()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxFork(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/clMaxPerHost") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxForkPerHost()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxForkPerHost(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/bfLn") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(bufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setBufLen(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/keepAliveReqs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveReqs()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveReqs(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/keepAliveTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveTm(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/taskPrior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(taskPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTaskPrior(s2i(opt->text()));
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TSocketOut                                   *
//************************************************
TSocketOut::TSocketOut( string name, const string &idb, TElem *el ) :
    TTransportOut(name, idb, el), mMSS(0), sockFd(-1), mLstReqTm(0)
{
    setAddr("TCP:localhost:10002");
    setTimings("5:1");
}

TSocketOut::~TSocketOut( )	{ }

void TSocketOut::setTimings( const string &vl )
{
    mTmCon = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,0,":"))*1e3)));
    mTmNext = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,1,":"))*1e3)));
    mTmRep = vmax(0, vmin(10000,(int)(s2r(TSYS::strParse(vl,2,":"))*1e3)));
    mTimings = mTmRep ? TSYS::strMess("%g:%g:%g",(1e-3*mTmCon),(1e-3*mTmNext),(1e-3*mTmRep)) :
			TSYS::strMess("%g:%g",(1e-3*mTmCon),(1e-3*mTmNext));
    modif();
}

string TSocketOut::getStatus( )
{
    string rez = TTransportOut::getStatus();

    if(startStat())
	rez += TSYS::strMess(_("Traffic in %s, out %s."), TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str());

    return rez;
}

void TSocketOut::load_( )
{
    //TTransportOut::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("tms"); if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("MSS"); if(!vl.empty()) setMSS(s2i(vl));
    } catch(...) { }
}

void TSocketOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("tms", timings());
    prmNd.setAttr("MSS", i2s(MSS()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportOut::save_();
}

void TSocketOut::start( int itmCon )
{
    MtxAlloc res(reqRes(), true);

    if(runSt) return;

    //Reconnect try after 2*tmCon()
    /*if((TSYS::curTime()-mLstReqTm) < 2000ll*tmCon())
	throw TError(nodePath().c_str(),_("Try next connect after '%g' seconds!"),(2e-3*tmCon()-1e-6*(TSYS::curTime()-mLstReqTm)));
    mLstReqTm = TSYS::curTime();*/

    //Status clear
    trIn = trOut = 0;
    if(!itmCon) itmCon = tmCon();

    //Connect to remote host
    string s_type = TSYS::strSepParse(addr(), 0, ':');

    if(s_type == S_NM_SOCK)		type = SOCK_FORCE;
    else if(s_type == S_NM_TCP)		type = SOCK_TCP;
    else if(s_type == S_NM_UDP)		type = SOCK_UDP;
    else if(s_type == S_NM_UNIX)	type = SOCK_UNIX;
#ifdef HAVE_LINUX_CAN_H
    else if(s_type == S_NM_RAWCAN)	type = SOCK_RAWCAN;
#endif
    else throw TError(nodePath().c_str(), _("Type socket '%s' error!"), s_type.c_str());

    if(type == SOCK_FORCE) {
	sockFd = s2i(TSYS::strSepParse(addr(),1,':'));
	int rez;
	if((rez=fcntl(sockFd,F_GETFL,0)) < 0 || fcntl(sockFd,F_SETFL,rez|O_NONBLOCK) < 0) {
	    close(sockFd);
	    throw TError(nodePath().c_str(), _("Error force socket %d using: '%s (%d)'!"), sockFd, strerror(errno), errno);
	}
    }
    else if(type == SOCK_TCP || type == SOCK_UDP) {
	memset(&nameIn, 0, sizeof(nameIn));
	nameIn.sin_family = AF_INET;
	char hBuf[STR_BUF_LEN], sBuf[256];
	struct hostent hostbuf, *hp;
	struct servent servbuf, *sp;

	string host = TSYS::strSepParse(addr(), 1, ':');
	string port = TSYS::strSepParse(addr(), 2, ':');
	if(host.size()) {
	    int herr;
	    gethostbyname_r(host.c_str(), &hostbuf, hBuf, sizeof(hBuf), &hp, &herr);
	    if(!hp) switch(herr) {
		case HOST_NOT_FOUND:
		    throw TError(nodePath().c_str(), _("Host '%s' not found!"), host.c_str());
		case NO_ADDRESS:
		    throw TError(nodePath().c_str(), _("The requested name '%s' does not have an IP address!"), host.c_str());
		case NO_RECOVERY:
		    throw TError(nodePath().c_str(), _("A non-recoverable name server error occurred while for '%s'!"), host.c_str());
		case TRY_AGAIN:
		    throw TError(nodePath().c_str(), _("A temporary error occurred on an authoritative name server for '%s'!"), host.c_str());
		default:
		    throw TError(nodePath().c_str(), _("Unknown error code from gethostbyname_r for '%s'!"), host.c_str());
	    }
	    nameIn.sin_addr.s_addr = *((int*)(hp->h_addr_list[0]));	//!!!! Append all IP list processing
	}
	else nameIn.sin_addr.s_addr = INADDR_ANY;

	//Get system port for "oscada" /etc/services
#if defined(__ANDROID__)
	struct servent *sptr = getservbyname(port.c_str(), (type == SOCK_TCP)?"tcp":"udp");
	if(sptr)			nameIn.sin_port = sptr->s_port;
#else
	if(getservbyname_r(port.c_str(),(type == SOCK_TCP)?"tcp":"udp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
	    nameIn.sin_port = sp->s_port;
#endif
	else if(htons(s2i(port)) > 0)	nameIn.sin_port = htons(s2i(port));
	else nameIn.sin_port = 10005;

	//Create socket
	if(type == SOCK_TCP) {
	    if((sockFd=socket(PF_INET,SOCK_STREAM,0)) == -1)
		throw TError(nodePath().c_str(), _("Error creation TCP socket: '%s (%d)'!"), strerror(errno), errno);
	    int vl = 1; setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
	    if(MSS()) { vl = MSS(); setsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, &vl, sizeof(int)); }
	}
	else if(type == SOCK_UDP) {
	    if((sockFd=socket(PF_INET,SOCK_DGRAM,0)) == -1)
		throw TError(nodePath().c_str(), _("Error creation UDP socket: '%s (%d)'!"), strerror(errno), errno);
	}
	//Connect to socket
	int flags = fcntl(sockFd, F_GETFL, 0);
	fcntl(sockFd, F_SETFL, flags|O_NONBLOCK);
	int rez = connect(sockFd, (sockaddr*)&nameIn, sizeof(nameIn));
	if(rez == -1 && errno == EINPROGRESS) {
	    struct timeval tv;
	    socklen_t slen = sizeof(rez);
	    fd_set fdset;
	    tv.tv_sec = itmCon/1000; tv.tv_usec = 1000*(itmCon%1000);
	    FD_ZERO(&fdset); FD_SET(sockFd, &fdset);
	    if((rez=select(sockFd+1,NULL,&fdset,NULL,&tv)) > 0 && !getsockopt(sockFd,SOL_SOCKET,SO_ERROR,&rez,&slen) && !rez) rez = 0;
	    else rez = -1;
	}
	if(rez) {
	    close(sockFd);
	    sockFd = -1;
	    if(mess_lev() == TMess::Debug)
		mess_debug(nodePath().c_str(), _("Connect by timeout %s error: '%s (%d)'"), tm2s(1e-3*itmCon).c_str(), strerror(errno), errno);
	    throw TError(nodePath().c_str(), _("Connect to Internet socket error: '%s (%d)'!"), strerror(errno), errno);
	}
    }
    else if(type == SOCK_UNIX) {
	string path = TSYS::strSepParse(addr(), 1, ':');
	if(!path.size()) path = "/tmp/oscada";
	memset(&nameUn, 0, sizeof(nameUn));
	nameUn.sun_family = AF_UNIX;
	strncpy(nameUn.sun_path, path.c_str(), sizeof(nameUn.sun_path));

	//Create socket
	if((sockFd=socket(PF_UNIX,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error creation UNIX socket: '%s (%d)'!"), strerror(errno), errno);
	if(connect(sockFd,(sockaddr*)&nameUn,sizeof(nameUn)) == -1) {
	    close(sockFd);
	    sockFd = -1;
	    throw TError(nodePath().c_str(), _("Connect to UNIX error: '%s (%d)'!"), strerror(errno), errno);
	}
	fcntl(sockFd, F_SETFL, fcntl(sockFd,F_GETFL,0)|O_NONBLOCK);
    }
#ifdef HAVE_LINUX_CAN_H
    else if(type == SOCK_RAWCAN) {
	if((sockFd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
	    throw TError(nodePath().c_str(), _("Error create '%s' socket!"), s_type.c_str());
	int flags = fcntl(sockFd, F_GETFL, 0);
	fcntl(sockFd, F_SETFL, flags | O_NONBLOCK);;
	string path = TSYS::strSepParse(addr(), 1, ':');
	struct can_filter rfilter;
	rfilter.can_id = strtoul(TSYS::strSepParse(addr(),2,':').c_str(), NULL, 0);
	rfilter.can_mask = strtoul(TSYS::strSepParse(addr(),3,':').c_str(), NULL, 0);
	setsockopt(sockFd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	if(!path.size()) path = "can0";
	struct ifreq ifr;
	strcpy(ifr.ifr_name, path.c_str());
	ioctl(sockFd, SIOCGIFINDEX, &ifr);
	struct sockaddr_can name_can;
	name_can.can_family = AF_CAN;
	name_can.can_ifindex = ifr.ifr_ifindex;
	if(bind(sockFd,(struct sockaddr*)&name_can,sizeof(name_can)) == -1) {
	    close(sockFd);
	    throw TError(nodePath().c_str(), _("RAWCAN socket doesn't bind to '%s'!"), addr().c_str());
	}
    }
#endif

    mLstReqTm = TSYS::curTime();

    runSt = true;

    TTransportOut::start();

    if(logLen()) pushLogMess(_("Started"));
}

void TSocketOut::stop( )
{
    MtxAlloc res(reqRes(), true);

    if(!runSt) return;

    //Status clear
    trIn = trOut = 0;

    //Close connection
    if(sockFd >= 0) {
	shutdown(sockFd, SHUT_RDWR);
	close(sockFd);
    }
    runSt = false;

    TTransportOut::stop();

    if(logLen()) pushLogMess(_("Stopped"));
}

int TSocketOut::messIO( const char *oBuf, int oLen, char *iBuf, int iLen, int time )
{
    string err = _("Unknown error");
    ssize_t kz = 0;
    struct timeval tv;
    fd_set rw_fd;
    int reqTry = 0,
	iB = 0;
    bool noReq = (time < 0),
	 writeReq = false;
    time = abs(time);

    MtxAlloc res(reqRes(), true);

    int prevTmOut = 0;
    if(time) { prevTmOut = tmCon(); setTmCon(time); }

    try {
	if(!runSt) throw TError(nodePath().c_str(),_("Transport is not started!"));

repeate:
	if(reqTry++ >= 2) { mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(), _("Request error: %s"), err.c_str()); }
	//Write request
	writeReq = false;
	if(oBuf != NULL && oLen > 0) {
	    if(!time) time = mTmCon;

	    // Input buffer clear
	    char tbuf[100];
	    while(!noReq && read(sockFd,tbuf,sizeof(tbuf)) > 0) ;
	    // Write request
	    if(mTmRep && (TSYS::curTime()-mLstReqTm) < (1000*mTmRep))
		TSYS::sysSleep(1e-6*((1e3*mTmRep)-(TSYS::curTime()-mLstReqTm)));

	    for(int wOff = 0; wOff != oLen; wOff += kz)
		if((kz=write(sockFd,oBuf+wOff,oLen-wOff)) <= 0) {
		    if(kz == 0 || (kz < 0 && errno == EAGAIN)) {
			tv.tv_sec  = (time/2)/1000; tv.tv_usec = 1000*((time/2)%1000);
			FD_ZERO(&rw_fd); FD_SET(sockFd, &rw_fd);
			kz = select(sockFd+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(sockFd,&rw_fd)) { kz = 0; continue; }
		    }
		    err = (kz < 0) ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No data wrote");
		    stop();
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Write error: %s"), err.c_str());
		    if(logLen()) pushLogMess(TSYS::strMess(_("Transmitting error: %s"), err.c_str()));
		    if(noReq) throw TError(nodePath().c_str(),_("Write error: %s"), err.c_str());
		    start();
		    goto repeate;
		}
		else {
		    trOut += kz;
		    if(logLen()) pushLogMess(_("Transmitted to\n") + string(oBuf+wOff,kz));
		}

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Wrote %s."), TSYS::cpct2str(oLen).c_str());

	    writeReq = true;
	}
	else if(!noReq) time = mTmNext;
	if(!time) time = 5000;

	//Read reply
	if(iBuf != NULL && iLen > 0) {
	    tv.tv_sec  = time/1000; tv.tv_usec = 1000*(time%1000);
	    FD_ZERO(&rw_fd); FD_SET(sockFd, &rw_fd);
	    kz = select(sockFd+1, &rw_fd, NULL, NULL, &tv);
	    if(kz == 0) {
		if(writeReq && !noReq) stop();
		mLstReqTm = TSYS::curTime();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read timeouted."));
		if(logLen()) pushLogMess(_("Receiving timeouted"));
		throw TError(nodePath().c_str(),_("Timeouted!"));
	    }
	    else if(kz < 0) {
		err = TSYS::strMess("%s (%d)", strerror(errno), errno);
		stop();
		mLstReqTm = TSYS::curTime();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read (select) error: %s"), err.c_str());
		if(logLen()) pushLogMess(TSYS::strMess(_("Receiving (select) error: %s"), err.c_str()));
		throw TError(nodePath().c_str(),_("Read (select) error: %s"), err.c_str());
	    }
	    else if(FD_ISSET(sockFd,&rw_fd)) {
		//!! Reading in that way but some time read() return 0 after the select() pass.
		// * Force wait any data in the request mode or EAGAIN
		// * No wait any data in the not request mode but it can get the data later
		for(int iRtr = 0; (((iB=read(sockFd,iBuf,iLen)) == 0 && !noReq) || (iB < 0 && errno == EAGAIN)) && iRtr < /*time*/mTmNext; ++iRtr) {
		    if(iRtr == 1 && iB == 0) {	//Check for same socket's errors
			int sockError = 0, sockLen = sizeof(sockError);
			getsockopt(sockFd, SOL_SOCKET, SO_ERROR, (char*)&sockError, (socklen_t*)&sockLen);
			if(sockError) { errno = sockError; break; }
		    }
		    TSYS::sysSleep(1e-3);
		}
		// * Force errors
		// * Retry if any data was wrote but no a reply there into the request mode
		// * !!: Zero can be also after disconection by peer and possible undetected here for the not request mode
		if(iB < 0 || (iB == 0 && writeReq && !noReq)) {
		    err = (iB < 0) ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No data");
		    stop();
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read error: %s"), err.c_str());
		    if(logLen()) pushLogMess(TSYS::strMess(_("Receiving error: %s"), err.c_str()));
		    // * Pass to retry into the request mode and on the successful writing
		    if(!writeReq || noReq) throw TError(nodePath().c_str(),_("Read error: %s"), err.c_str());
		    start();
		    goto repeate;
		}
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read %s."), TSYS::cpct2str(vmax(0,iB)).c_str());
		if(iB > 0 && logLen()) pushLogMess(_("Received from\n") + string(iBuf,iB));
		trIn += vmax(0, iB);
	    }
	}
    } catch(TError&) {
	if(prevTmOut) setTmCon(prevTmOut);
	throw;
    }

    if(prevTmOut) setTmCon(prevTmOut);

    if(mTmRep)	mLstReqTm = TSYS::curTime();

    return vmax(0, iB);
}

void TSocketOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportOut::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1,"help",
	    _("Socket's output transport has address format:\n"
	    "  TCP:{addr}:{port} - TCP socket:\n"
	    "    addr - address for remote socket to be opened;\n"
	    "    port - network port (/etc/services);\n"
	    "  UDP:{addr}:{port} - UDP socket:\n"
	    "    addr - address for remote socket to be opened;\n"
	    "    port - network port (/etc/services).\n"
	    "  RAWCAN:{if}:{mask}:{id} - CAN socket:\n"
	    "    if - interface name;\n"
	    "    mask - CAN frame id mask;\n"
	    "    id - CAN id.\n"
	    "  UNIX:{name} - UNIX socket:\n"
	    "    name - UNIX-socket's file name."));
	if(addr().compare(0,4,"TCP:") == 0)
	    ctrMkNode("fld",opt,-1,"/prm/cfg/MSS",_("Maximum segment size (MSS)"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,
		"tp","str","help",_("Set 0 for system MSS."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"conn:next[:rep]\". Where:\n"
	    "    conn - maximum time for connection respond wait, in seconds;\n"
	    "    next - maximum time for continue respond wait, in seconds;\n"
	    "    rep  - minimum repeate timeout, in seconds."));
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/MSS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(MSS()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMSS(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/TMS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

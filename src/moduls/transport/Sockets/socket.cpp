
//OpenSCADA module Transport.Sockets file: socket.cpp
/***************************************************************************
 *   Copyright (C) 2003-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"3.3.0"
#define AUTHORS		_("Roman Savochenko, Maxim Kochetkov")
#define DESCRIPTION	_("Provides sockets based transport. Support network and UNIX sockets. Network socket supports TCP, UDP and RAWCAN protocols.")
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

string TTransSock::outAddrHelp( )
{
    return string(_("Socket's output transport has the address format:\n"
	"  [TCP:]{addr}[,{addrN}]:{port} - TCP socket:\n"
	"    addr - address with which the connection is made; there may be as the symbolic representation as well as IPv4 \"127.0.0.1\" or IPv6 \"[::1]\";\n"
	"    port - network port with which the connection is made; indication of the character name of the port according to /etc/services is available;\n"
	"  UDP:{addr}[,{addrN}]:{port} - UDP socket:\n"
	"    addr - address with which the connection is made; there may be as the symbolic representation as well as IPv4 \"127.0.0.1\" or IPv6 \"[::1]\";\n"
	"    port - network port with which the connection is made; indication of the character name of the port according to /etc/services is available.\n"
	"  RAWCAN:{if}:{mask}:{id} - CAN socket:\n"
	"    if - interface name;\n"
	"    mask - CAN mask;\n"
	"    id - CAN id.\n"
	"  UNIX:{name} - UNIX socket:\n"
	"    name - UNIX-socket's file name.")) + "\n\n|| " + outTimingsHelp() + "\n\n|| " + outAttemptsHelp();
}

string TTransSock::outTimingsHelp( )
{
    return _("Connection timings in the format \"{conn}:{next}[:{rep}]\", where:\n"
	"    conn - maximum time of waiting the connection, in seconds;\n"
	"    next - maximum time of waiting for continue the response, in seconds;\n"
	"    rep  - minimum time of waiting the request repeating, in seconds.\n"
	"Can be prioritatile specified into the address field as the second global argument, as such \"localhost:123||5:1\".");
}

string TTransSock::outAttemptsHelp( )
{
    return _("Attempts of the requesting both for this transport and protocol, for full requests.\n"
	"Can be prioritatile specified into the address field as the third global argument, as such \"localhost:123||5:1||3\".");
}

//************************************************
//* TSocketIn                                    *
//************************************************
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), sockRes(true), wrToRes(true), type(SOCK_TCP), mMode(0), mMSS(0), mMaxQueue(10), mMaxFork(20), mMaxForkPerHost(0), mBufLen(5),
    mKeepAliveReqs(0), mKeepAliveTm(60), mTaskPrior(0), clFree(true)
{
    setAddr("localhost:10005");
}

TSocketIn::~TSocketIn( )
{

}

string TSocketIn::getStatus( )
{
    string rez = TTransportIn::getStatus();

    if(startStat()) {
	string s_type = "UNKNOWN";
	switch(type) {
	    case SOCK_FORCE:	s_type = S_NM_SOCK;	break;
	    case SOCK_TCP:	s_type = S_NM_TCP;	break;
	    case SOCK_UDP:	s_type = S_NM_UDP;	break;
	    case SOCK_UNIX:	s_type = S_NM_UNIX;	break;
	    case SOCK_RAWCAN:	s_type = S_NM_RAWCAN;	break;
	}
	if(type == SOCK_TCP || type == SOCK_UDP)
	    rez += TSYS::strMess(_("Connections %d, opened %d, last %s, closed by the limit %d. "),
				connNumb, (protocols().empty()?assTrs().size():clId.size()), atm2s(lastConn()).c_str(), clsConnByLim);
	if(protocols().size())
	    rez += TSYS::strMess(_("%s traffic in %s, out %s. "),
				s_type.c_str(), TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str());
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

    int aOff = 0;

    //Socket init
    string s_type = TSYS::strParse(addr(), 0, ":", &aOff);

    if(s_type == S_NM_TCP)	type = SOCK_TCP;
    else if(s_type == S_NM_UDP)	type = SOCK_UDP;
    else if(s_type == S_NM_UNIX)type = SOCK_UNIX;
    else if(s_type == S_NM_RAWCAN)
#ifdef HAVE_LINUX_CAN_H
	type = SOCK_RAWCAN;
#else
	throw TError(nodePath().c_str(), _("Error the socket type '%s'!"), s_type.c_str());
#endif
    else s_type = "TCP", type = SOCK_TCP, aOff = 0;
	//throw TError(nodePath().c_str(), _("Error the socket type '%s'!"), s_type.c_str());

    if(type == SOCK_TCP || type == SOCK_UDP) {
	if(addr()[aOff] != '[') host = TSYS::strParse(addr(), 0, ":", &aOff);
	else { aOff++; host = TSYS::strParse(addr(), 0, "]:", &aOff); } //Get IPv6
	port	= TSYS::strParse(addr(), 0, ":", &aOff);

	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = (type == SOCK_TCP) ? SOCK_STREAM : SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;		//For server side all addresses allow
	int error;
	string aErr;
	sockFd = -1;

	MtxAlloc aRes(*SYS->commonLock("getaddrinfo"), true);
	if((error=getaddrinfo(((host.size() && host != "*")?host.c_str():NULL),(port.size()?port.c_str():"10005"),&hints,&res)))
	    throw TError(nodePath().c_str(), _("Error the address '%s': '%s (%d)'"), addr().c_str(), gai_strerror(error), error);
	vector<sockaddr_storage> addrs;
	for(struct addrinfo *iAddr = res; iAddr != NULL; iAddr = iAddr->ai_next) {
	    static struct sockaddr_storage ss;
	    if(iAddr->ai_addrlen > sizeof(ss))	{ aErr = _("sockaddr to large."); continue; }
	    memcpy(&ss, iAddr->ai_addr, iAddr->ai_addrlen);
	    addrs.push_back(ss);
	}
	freeaddrinfo(res);
	aRes.unlock();

	// Try for all addresses
	for(int iA = 0; iA < addrs.size(); iA++) {
	    try {
		if(type == SOCK_TCP) {
		    if((sockFd=socket((((sockaddr*)&addrs[iA])->sa_family==AF_INET6)?PF_INET6:PF_INET,SOCK_STREAM,0)) == -1)
			throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
		    int vl = 1; setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
		    if(MSS()) { vl = MSS(); setsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, &vl, sizeof(int)); }
		}
		else {
		    if((sockFd=socket((((sockaddr*)&addrs[iA])->sa_family==AF_INET6)?PF_INET6:PF_INET,SOCK_DGRAM,0)) == -1)
			throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
		}

		if(type == SOCK_TCP) {
		    if(mode() == 2) {							//Initiate connection
			int flags = fcntl(sockFd, F_GETFL, 0);
			fcntl(sockFd, F_SETFL, flags|O_NONBLOCK);
			int rez = connect(sockFd, (sockaddr*)&addrs[iA], sizeof(addrs[iA]));
			if(rez == -1 && errno == EINPROGRESS) {
			    struct timeval tv;
			    socklen_t slen = sizeof(rez);
			    fd_set fdset;
			    tv.tv_sec = 5; tv.tv_usec = 0;
			    FD_ZERO(&fdset); FD_SET(sockFd, &fdset);
			    if((rez=select(sockFd+1,NULL,&fdset,NULL,&tv)) > 0 && !getsockopt(sockFd,SOL_SOCKET,SO_ERROR,&rez,&slen) && !rez) rez = 0;
			    else rez = -1;
			}
			if(rez) throw TError(nodePath().c_str(), _("Error connecting the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
			else if(addon.size()) write(sockFd, addon.data(), addon.size());	//Writing the identification sequence
		    }
		    else if(bind(sockFd,(sockaddr*)&addrs[iA],sizeof(addrs[iA])) == -1) {	//Waiting a connection
			int rez = errno;
			shutdown(sockFd, SHUT_RDWR);
			throw TError(nodePath().c_str(), _("Error binding the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
		    }
		    listen(sockFd, maxQueue());
		}
		else if(type == SOCK_UDP) {
		    if(bind(sockFd,(sockaddr*)&addrs[iA],sizeof(addrs[iA])) == -1) {
			int rez = errno;
			shutdown(sockFd, SHUT_RDWR);
			throw TError(nodePath().c_str(), _("Error binding the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
		    }
		}
	    } catch(TError &err) {
		aErr = err.mess;
		if(sockFd >= 0) close(sockFd);
		sockFd = -1;
		continue;	//Try next
	    }
	    break;	//OK
	}

	if(sockFd < 0) throw TError(nodePath().c_str(), "%s", aErr.c_str());
    }
    else if(type == SOCK_UNIX) {
	if((sockFd=socket(PF_UNIX,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);

	path	= TSYS::strParse(addr(), 0, ":", &aOff);
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
		throw TError(nodePath().c_str(), _("Error connecting the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	    }
	}
	else if(bind(sockFd,(sockaddr*)&nameUn,sizeof(nameUn)) == -1) {
	    int rez = errno;
	    close(sockFd);
	    throw TError(nodePath().c_str(),_("Error binding the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
	}
	listen(sockFd, maxQueue());
    }
#ifdef HAVE_LINUX_CAN_H
    else if(type == SOCK_RAWCAN) {
	if((sockFd=socket(PF_CAN,SOCK_RAW,CAN_RAW)) == -1)
	    throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);

	path	= TSYS::strParse(addr(), 0, ":", &aOff);
	struct can_filter rfilter;
	rfilter.can_id = s2i(TSYS::strParse(addr(),0,":",&aOff));
	rfilter.can_mask = s2i(TSYS::strParse(addr(),0,":",&aOff));
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
	    throw TError(nodePath().c_str(), _("Error binding the %s socket: '%s (%d)'!"), s_type.c_str(), strerror(errno), errno);
	}
	else if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("RAWCAN socket binded to '%s'!"), addr().c_str());
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
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Restart due to lack of input activity to '%s'."), addr().c_str());
	    if(startStat()) stop();
	    start();
	}

	//Check the assigned to that output transports for inactivity
	/*vector<AutoHD<TTransportOut> > aTrLs = assTrs(true);
	for(unsigned iTr = 0; iTr < aTrLs.size(); iTr++) {
	    if(!aTrLs[iTr].at().startStat() || aTrLs[iTr].at().addr().compare(0,5,"SOCK:") != 0) continue;
	    int oSockFd = s2i(TSYS::strParse(aTrLs[iTr].at().addr(),1,":"));
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
		if(wL == 0) { mess_err(nodePath().c_str(), _("Write: the answer is zero byte.")); break; }
		else if(wL < 0) {
		    if(errno == EAGAIN) {
			tv.tv_sec = 1; tv.tv_usec = 0;		//!!!! Where the time get?
			FD_ZERO(&rw_fd); FD_SET(sId, &rw_fd);
			int kz = select(sId+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(sId,&rw_fd)) { wL = 0; continue; }
			//!!!! Maybe some flush !!!!
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
	int off = 0;
	mMode = 0;
	string s_type = TSYS::strParse(co.getS(), 0, ":");
	if(s_type == S_NM_TCP) {
	    type = SOCK_TCP;
	    mMode = (s_type=TSYS::strParse(co.getS(),3,":",&off)).size() ? s2i(s_type) : 1;
	    addon = (off < co.getS().size()) ? co.getS().substr(off) : "";
	}
	else if(s_type == S_NM_UDP)	type = SOCK_UDP;
	else if(s_type == S_NM_UNIX) {
	    type = SOCK_UNIX;
	    mMode = (s_type=TSYS::strParse(co.getS(),2,":")).size() ? s2i(s_type) : 1;
	}
	else if(s_type == S_NM_RAWCAN)	type = SOCK_RAWCAN;
	else {
	    type = SOCK_TCP;
	    mMode = (s_type=TSYS::strParse(co.getS(),2,":",&off)).size() ? s2i(s_type) : 1;
	    addon = (off < co.getS().size()) ? co.getS().substr(off) : "";
	}
    }

    return TTransportIn::cfgChange(co, pc);
}

void *TSocketIn::Task( void *sock_in )
{
    char		*buf = NULL;
    fd_set		rd_fd;
    struct timeval	tv;
    TSocketIn *sock = (TSocketIn*)sock_in;
    vector< AutoHD<TProtocolIn> > prot_in;

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
	    mess_err(sock->nodePath().c_str(), _("Closing the input transport by the error \"%s (%d)\""), strerror(errno), errno);
	    break;
	}
	if(kz <= 0 || !FD_ISSET(sock->sockFd, &rd_fd)) continue;

	struct sockaddr_storage	sockStor;
	struct sockaddr *sadr = (struct sockaddr*) &sockStor;;
	socklen_t sadrLen = sizeof(sockStor);
	if(sock->type == SOCK_TCP) {
	    int sockFdCL = accept(sock->sockFd, sadr, &sadrLen);
	    if(sockFdCL != -1) {
		string sender;
		if(sadr->sa_family == AF_INET6) {
		    char aBuf[INET6_ADDRSTRLEN];
		    getnameinfo(sadr, sadrLen, aBuf, sizeof(aBuf), 0, 0, NI_NUMERICHOST);
		    sender = aBuf;
		} else sender = inet_ntoa(((sockaddr_in*)sadr)->sin_addr);

		if(sock->clId.size() >= sock->maxFork() || (sock->maxForkPerHost() && sock->forksPerHost(sender) >= sock->maxForkPerHost())) {
		    sock->clsConnByLim++;
		    close(sockFdCL);
		    continue;
		}
		//Create presenting the client connection output transport
		if(sock->protocols().empty() && sock->assTrs(true).size() <= sock->maxFork()) {
		    string outTrId = sock->assTrO("SOCK:"+i2s(sockFdCL));
		    ((AutoHD<TSocketOut>)sock->owner().outAt(outTrId)).at().connAddr = sender;
		    sock->connNumb++;
		    sock->connTm = time(NULL);
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
		    mess_err(sock->nodePath().c_str(), _("Error creating a thread!"));
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
		    mess_err(sock->nodePath().c_str(), _("Error creating a thread!"));
		}
	    }
	}
	else if(sock->type == SOCK_UDP) {
	    string req, answ;

	    ssize_t r_len = recvfrom(sock->sockFd, buf, sock->bufLen()*1000, 0, sadr, &sadrLen);
	    if(r_len <= 0) continue;
	    sock->trIn += r_len;
	    req.assign(buf, r_len);

	    string sender;
	    if(sadr->sa_family == AF_INET6) {
		char aBuf[INET6_ADDRSTRLEN];
		getnameinfo(sadr, sadrLen, aBuf, sizeof(aBuf), 0, 0, NI_NUMERICHOST);
		sender = aBuf;
	    } else sender = inet_ntoa(((sockaddr_in*)sadr)->sin_addr); //name_cl.sin_addr);

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Read datagram %s from '%s'!"), TSYS::cpct2str(r_len).c_str(), sender.c_str());
	    if(sock->logLen()) sock->pushLogMess(TSYS::strMess(_("%d:Received from '%s'\n"),sock->sockFd,sender.c_str()) + req);

	    int64_t stTm = 0;
	    if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
	    sock->messPut(sock->sockFd, req, answ, sender, prot_in);
	    if(mess_lev() == TMess::Debug && stTm) {
		sock->prcTm = SYS->curTime() - stTm;
		sock->prcTmMax = vmax(sock->prcTmMax, sock->prcTm);
	    }
	    if(answ.empty()) continue;

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Wrote datagram %s to '%s'!"), TSYS::cpct2str(answ.size()).c_str(), sender.c_str());

	    r_len = sendto(sock->sockFd, answ.c_str(), answ.size(), 0, sadr, sadrLen);
	    sock->trOut += vmax(0, r_len);

	    if(r_len > 0 && sock->logLen()) sock->pushLogMess(TSYS::strMess(_("%d:Transmitted to '%s'\n"),sock->sockFd,sender.c_str()) + answ);
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
	    if(mess_lev() == TMess::Debug && stTm) {
		sock->prcTm = SYS->curTime() - stTm;
		sock->prcTmMax = vmax(sock->prcTmMax, sock->prcTm);
	    }
	    if(answ.empty()) continue;

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
	if(iId->second->pid) pthread_kill(iId->second->pid, SIGALRM);
    res.unlock();
    TSYS::eventWait(sock->clFree, true, string(MOD_ID)+": "+sock->id()+_(" stopping client tasks ..."));

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
	mess_debug(s.s->nodePath().c_str(), _("Connected by '%s'!"), s.sender.c_str());

    s.s->clientReg(&s);

    //Client socket process
    struct timeval tv;
    fd_set rw_fd;
    string req, answ;
    char   buf[s.s->bufLen()*1000 + 1];
    vector< AutoHD<TProtocolIn> > prot_in;
    bool   sessOk = false;
    int    actPrts = 0;

    try {
	do {
	    tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;

	    unsigned poolPrt = 0;
	    if((actPrts=s.s->prtInit(prot_in,s.sock,s.sender)))
		for(int iP = 0; iP < prot_in.size(); iP++)
		    if(!prot_in[iP].freeStat() && (poolPrt=prot_in[iP].at().waitReqTm()))
			break;
	    if(poolPrt) { tv.tv_sec = poolPrt/1000; tv.tv_usec = (poolPrt%1000)*1000; }

	    FD_ZERO(&rw_fd); FD_SET(s.sock, &rw_fd);
	    int kz = select(s.sock+1, &rw_fd, NULL, NULL, &tv);
	    if((kz == 0 && !poolPrt) || (kz == -1 && errno == EINTR) || (kz > 0 && !FD_ISSET(s.sock,&rw_fd))) continue;
	    if(kz < 0) {
		if(mess_lev() == TMess::Debug)
		    mess_debug(s.s->nodePath().c_str(), _("Terminated by the error \"%s (%d)\""), strerror(errno), errno);
		if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Terminated by the error \"%s (%d)\"\n"),s.sock,strerror(errno),errno));
		break;
	    }

	    ssize_t r_len = 0;
	    if(kz && (r_len=read(s.sock,buf,s.s->bufLen()*1000)) <= 0) {
		if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Terminated by the reading %d error \"%s (%d)\"\n"),s.sock,r_len,strerror(errno),errno));
		break;
	    }
	    s.s->dataRes().lock();
	    s.s->trIn += r_len; s.trIn += r_len;
	    s.s->dataRes().unlock();

	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("Read message %s from '%s'."), TSYS::cpct2str(r_len).c_str(), s.sender.c_str());
	    req.assign(buf, r_len);
	    if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Received from '%s'\n"),s.sock,s.sender.c_str()) + req);

	    int64_t stTm = 0;
	    if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
	    actPrts = s.s->messPut(s.sock, req, answ, s.sender, prot_in);
	    if(mess_lev() == TMess::Debug && stTm) {
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
		    if(wL == 0) { mess_err(s.s->nodePath().c_str(), _("Write: the answer is zero byte.")); break; }
		    else if(wL < 0) {
			if(errno == EAGAIN) {
			    tv.tv_sec = 1; tv.tv_usec = 0;		//!!!! Where the time get?
			    FD_ZERO(&rw_fd); FD_SET(s.sock, &rw_fd);
			    kz = select(s.sock+1, NULL, &rw_fd, NULL, &tv);
			    if(kz > 0 && FD_ISSET(s.sock,&rw_fd)) { wL = 0; continue; }
			    //!!!! May be some flush !!!!
			}
			string err = TSYS::strMess(_("Write: error '%s (%d)'!"), strerror(errno), errno);
			if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("Error transmitting: %s"),err.c_str()));
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
		(!sessOk || ((s.s->mode() == 1 || actPrts) && (!s.s->keepAliveReqs() || cnt < s.s->keepAliveReqs()))))));

	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Disconnected by '%s'!"), s.sender.c_str());
    } catch(TError &err) {
	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Terminated by the execution '%s'"), err.mess.c_str());
	if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Terminated by the execution '%s'"),s.sock,err.mess.c_str()));
    }

    //Close protocol on broken connection
    for(int iP = 0; iP < prot_in.size(); iP++) {
	if(prot_in[iP].freeStat())	continue;
	try {
	    string n_pr = prot_in[iP].at().name();
	    AutoHD<TProtocol> proto = AutoHD<TProtocol>(&prot_in[iP].at().owner());
	    prot_in[iP].free();
	    proto.at().close(n_pr);
	} catch(TError &err) {
	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("Terminated by the execution '%s'"), err.mess.c_str());
	    if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Terminated by the execution '%s'"),s.sock,err.mess.c_str()));
	}
    }

    s.s->clientUnreg(&s);

    return NULL;
}

int TSocketIn::prtInit( vector< AutoHD<TProtocolIn> > &prot_in, int sock, const string &sender )
{
    bool initErr = false;
    string prts = protocols(), prt, subPrt;
    int iActP = 0;
    for(int off = 0, iP = 0; (prt=TSYS::strParse(prts,0,";",&off)).size(); iP++) {
	if(iP < prot_in.size() && !prot_in[iP].freeStat()) { iActP++; continue; }
	try {
	    AutoHD<TProtocol> proto = SYS->protocol().at().modAt(TSYS::strParse(prt,0,"."));
	    subPrt = TSYS::strParse(prt, 1, ".");
	    string n_pr = id() + i2s(sock) + (subPrt.size()?"#"+subPrt:"");
	    if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, this, sender+"\n"+i2s(sock));
	    if(iP < prot_in.size()) prot_in[iP] = proto.at().at(n_pr);
	    else prot_in.push_back(proto.at().at(n_pr));
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("The new input protocol's object '%s' is created!"), n_pr.c_str());
	    iActP++;
	} catch(TError &err) {
	    initErr = true;
	    mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	}
    }

    if(initErr && !iActP)
	mess_warning(nodePath().c_str(), _("All protocols is wrong for their initialization."));

    return iActP;
}

int TSocketIn::messPut( int sock, string &request, string &answer, const string &sender, vector< AutoHD<TProtocolIn> > &prot_in )
{
    if(!prtInit(prot_in,sock,sender))	return 0;

    int iActP = 0;
    string n_pr, tAnsw;

    for(int iP = 0; iP < prot_in.size(); iP++, answer += tAnsw, tAnsw = "") {
	if(prot_in[iP].freeStat())	continue;
	AutoHD<TProtocol> proto;
	try {
	    if(prot_in[iP].at().mess(request,tAnsw)) { iActP++; continue; }
	    proto = AutoHD<TProtocol>(&prot_in[iP].at().owner());
	    n_pr = prot_in[iP].at().name();
	    prot_in[iP].free();
	    if(proto.at().openStat(n_pr)) proto.at().close(n_pr);
	    if(mess_lev() == TMess::Debug)
		mess_debug(nodePath().c_str(), _("The input protocol object '%s' has closed itself!"), n_pr.c_str());
	} catch(TError &err) {
	    if(!prot_in[iP].freeStat()) {
		if(proto.freeStat()) proto = AutoHD<TProtocol>(&prot_in[iP].at().owner());
		n_pr = prot_in[iP].at().name();
	    }
	    prot_in[iP].free();
	    if(!proto.freeStat() && proto.at().openStat(n_pr)) proto.at().close(n_pr);

	    mess_err(nodePath().c_str(), "%s", err.mess.c_str());
	    mess_err(nodePath().c_str(), _("Error requesting the protocol."));
	}
    }

    return iActP;
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

    if(logLen()) pushLogMess(TSYS::strMess(_("New client %d from '%s' connected"),so->sock,so->sender.c_str()));
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
	if(ctrMkNode("area",opt,1,"/prm/st",_("State")) && protocols().size() && clId.size() && !(type == SOCK_TCP && mode() == 2))
	    ctrMkNode("list", opt, -1, "/prm/st/conns", _("Active connections"), R_R_R_, "root", STR_ID);
	ctrRemoveNode(opt, "/prm/cfg/A_PRMS");
	ctrMkNode("fld", opt, -1, "/prm/cfg/ADDR", EVAL_STR, startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "help",
	    _("Socket's input transport has the address format:\n"
	    "  [TCP:]{addr}:{port}:{mode}[:{IDmess}] - TCP socket:\n"
	    "    addr - address for socket to be opened, empty or \"*\" address opens socket for all interfaces; there may be as the symbolic representation as well as IPv4 \"127.0.0.1\" or IPv6 \"[::1]\";\n"
	    "    port - network port on which the socket is opened, indication of the character name of the port, according to /etc/services is available;\n"
	    "    mode - mode of operation: 0 - break connections; 1(default) - keep alive; 2 - initiative connections;\n"
	    "    IDmess - identification message of the initiative connection - the mode 2.\n"
	    "  UDP:{addr}:{port} - UDP socket:\n"
	    "    addr - address for socket to be opened, empty or \"*\" address opens socket for all interfaces; there may be as the symbolic representation as well as IPv4 \"127.0.0.1\" or IPv6 \"[::1]\";\n"
	    "    port - network port on which the socket is opened, indication of the character name of the port, according to /etc/services is available.\n"
	    "  RAWCAN:{if}:{mask}:{id} - CAN socket:\n"
	    "    if - interface name;\n"
	    "    mask - CAN mask;\n"
	    "    id - CAN id.\n"
	    "  UNIX:{name}:{mode} - UNIX socket:\n"
	    "    name - UNIX-socket's file name;\n"
	    "    mode - mode of operation: 0 - break connections; 1(default) - keep alive; 2 - initiative connections."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/PROT", EVAL_STR, startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1,
	    "help",_("Empty value for the protocol selection switchs the transport to a mode\n"
		     "of creation associated output transports for each connection to this transport."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/taskPrior", _("Priority"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
	    "tp","dec", "help",TMess::labTaskPrior());
	if(type == SOCK_TCP)
	    ctrMkNode("fld", opt, -1, "/prm/cfg/MSS", _("Maximum segment size (MSS)"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
		"tp","str", "help",_("Set 0 for the system MSS."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/bfLn", _("Input buffer, KB"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "tp","dec");
	if(type == SOCK_TCP || type == SOCK_UNIX || type == SOCK_RAWCAN) {
	    ctrMkNode("fld", opt, -1, "/prm/cfg/qLn", _("Queue length"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "tp","dec");
	    if(mode() != 2) {
		ctrMkNode("fld", opt, -1, "/prm/cfg/clMax", _("Maximum number of clients"), RWRWR_, "root", STR_ID, 1, "tp","dec");
		if(type == SOCK_TCP)
		    ctrMkNode("fld", opt, -1, "/prm/cfg/clMaxPerHost", _("Maximum number of clients per host"), RWRWR_, "root", STR_ID, 2, "tp","dec",
			"help",_("Set to 0 to disable this limit."));
		ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveReqs", _("Keep alive requests"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		    "help",_("Closing the connection after the specified requests.\nZero value to disable - do not close ever."));
	    }
	    ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveTm", _("Keep alive timeout, seconds"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		"help",_("Closing the connection after no requests at the specified timeout.\nZero value to disable - do not close ever."));
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/conns" && ctrChkNode(opt)) {
	MtxAlloc res(sockRes, true);
	for(map<int,SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId) {
	    string mess = TSYS::strMess(_("%s %d(%s): last %s; traffic in %s, out %s; "),
		atm2s(iId->second->tmCreate,"%Y-%m-%dT%H:%M:%S").c_str(),iId->first,iId->second->sender.c_str(),
		atm2s(iId->second->tmReq,"%Y-%m-%dT%H:%M:%S").c_str(),
		TSYS::cpct2str(iId->second->trIn).c_str(),TSYS::cpct2str(iId->second->trOut).c_str());
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
    TTransportOut(name, idb, el), mAttemts(2), mMSS(0), sockFd(-1), type(SOCK_TCP), mLstReqTm(0)
{
    setAddr("localhost:10005");
    setTimings("5:1");
}

TSocketOut::~TSocketOut( )	{ }

string TSocketOut::getStatus( )
{
    string rez = TTransportOut::getStatus();

    if(startStat()) {
	string s_type = "UNKNOWN";
	switch(type) {
	    case SOCK_FORCE:	s_type = S_NM_SOCK;	break;
	    case SOCK_TCP:	s_type = S_NM_TCP;	break;
	    case SOCK_UDP:	s_type = S_NM_UDP;	break;
	    case SOCK_UNIX:	s_type = S_NM_UNIX;	break;
	    case SOCK_RAWCAN:	s_type = S_NM_RAWCAN;	break;
	}
	rez += TSYS::strMess(_("To the host '%s'. "), connAddr.c_str());
	rez += TSYS::strMess(_("%s traffic in %s, out %s. "), s_type.c_str(), TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str());
	if(mess_lev() == TMess::Debug && respTmMax)
	    rez += TSYS::strMess(_("Response time %s[%s]. "), tm2s(1e-6*respTm).c_str(), tm2s(1e-6*respTmMax).c_str());
    }

    return rez;
}

void TSocketOut::setTimings( const string &vl )
{
    if(vl == mTimings)	return;
    mTmCon = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,0,":"))*1e3)));
    mTmNext = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,1,":"))*1e3)));
    mTmRep = vmax(0, vmin(10000,(int)(s2r(TSYS::strParse(vl,2,":"))*1e3)));
    mTimings = mTmRep ? TSYS::strMess("%g:%g:%g",(1e-3*mTmCon),(1e-3*mTmNext),(1e-3*mTmRep)) :
			TSYS::strMess("%g:%g",(1e-3*mTmCon),(1e-3*mTmNext));
    modif();
}

void TSocketOut::setAttempts( unsigned short vl )
{
    if(vl == mAttemts)	return;
    mAttemts = vmax(1, vmin(5,vl));
    modif();
}

void TSocketOut::load_( )
{
    //TTransportOut::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("tms"); if(!vl.empty()) setTimings(vl);
	vl = prmNd.attr("attempts"); if(!vl.empty()) setAttempts(s2i(vl));
	vl = prmNd.attr("MSS"); if(!vl.empty()) setMSS(s2i(vl));
    } catch(...) { }
}

void TSocketOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("tms", timings());
    prmNd.setAttr("attempts", i2s(attempts()));
    prmNd.setAttr("MSS", i2s(MSS()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportOut::save_();
}

bool TSocketOut::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "ADDR" && co.getS() != pc.getS()) {
	string s_type = TSYS::strParse(co.getS(), 0, ":");
	if(s_type == S_NM_TCP)		type = SOCK_TCP;
	else if(s_type == S_NM_UDP)	type = SOCK_UDP;
	else if(s_type == S_NM_UNIX)	type = SOCK_UNIX;
	else if(s_type == S_NM_RAWCAN)	type = SOCK_RAWCAN;
	else				type = SOCK_TCP;
    }

    return TTransportOut::cfgChange(co, pc);
}

void TSocketOut::start( int itmCon )
{
    MtxAlloc res(reqRes(), true);

    if(runSt) return;

    //Status clear
    trIn = trOut = respTm = respTmMax = 0;
    if(!itmCon) itmCon = tmCon();

    int aOff = 0;
    // Reading the global arguments
    string addr_ = TSYS::strParse(addr(), 0, "||");
    string tVl;
    if((tVl=TSYS::strParse(addr(),1,"||")).size()) setTimings(tVl);
    if((tVl=TSYS::strParse(addr(),2,"||")).size()) setAttempts(s2i(tVl));

    try {

    //Connect to the remote host
    string s_type = TSYS::strParse(addr_, 0, ":", &aOff);

    if(s_type == S_NM_SOCK)	type = SOCK_FORCE;
    else if(s_type == S_NM_TCP)	type = SOCK_TCP;
    else if(s_type == S_NM_UDP)	type = SOCK_UDP;
    else if(s_type == S_NM_UNIX)type = SOCK_UNIX;
    else if(s_type == S_NM_RAWCAN)
#ifdef HAVE_LINUX_CAN_H
	type = SOCK_RAWCAN;
#else
	throw TError(nodePath().c_str(), _("Error the socket type '%s'!"), s_type.c_str());
#endif
    else s_type = "TCP", type = SOCK_TCP, aOff = 0;
	//throw TError(nodePath().c_str(), _("Error the socket type '%s'!"), s_type.c_str());

    if(type == SOCK_FORCE) {
	sockFd = s2i(TSYS::strParse(addr_,0,":",&aOff));
	int rez;
	if(sockFd < 0)	throw TError(nodePath().c_str(), _("The force socket is deactivated!"));
	else if((rez=fcntl(sockFd,F_GETFL,0)) < 0 || fcntl(sockFd,F_SETFL,rez|O_NONBLOCK) < 0) {
	    close(sockFd);
	    throw TError(nodePath().c_str(), _("Error the force socket %d using: '%s (%d)'!"), sockFd, strerror(errno), errno);
	}
    }
    else if(type == SOCK_TCP || type == SOCK_UDP) {
	string host, host_, port;
	if(addr_[aOff] != '[') host = TSYS::strParse(addr_, 0, ":", &aOff);
	else { aOff++; host = TSYS::strParse(addr_, 0, "]:", &aOff); }	//Get IPv6
	port	= TSYS::strParse(addr_, 0, ":", &aOff);

	string aErr;
	sockFd = -1;
	for(int off = 0; (host_=TSYS::strParse(host,0,",",&off)).size(); ) {
	    struct addrinfo hints, *res;
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_socktype = (type == SOCK_TCP) ? SOCK_STREAM : SOCK_DGRAM;
	    int error;

	    if(logLen()) pushLogMess(TSYS::strMess(_("Resolving for '%s'"),host_.c_str()));

	    MtxAlloc aRes(*SYS->commonLock("getaddrinfo"), true);
	    if((error=getaddrinfo(host_.c_str(),(port.size()?port.c_str():"10005"),&hints,&res)))
		throw TError(nodePath().c_str(), _("Error the address '%s': '%s (%d)'"), addr_.c_str(), gai_strerror(error), error);
	    vector<sockaddr_storage> addrs;
	    for(struct addrinfo *iAddr = res; iAddr != NULL; iAddr = iAddr->ai_next) {
		static struct sockaddr_storage ss;
		if(iAddr->ai_addrlen > sizeof(ss)) { aErr = _("sockaddr to large."); continue; }
		memcpy(&ss, iAddr->ai_addr, iAddr->ai_addrlen);
		addrs.push_back(ss);
	    }
	    freeaddrinfo(res);
	    aRes.unlock();

	    // Try for all addresses
	    for(int iA = 0; iA < addrs.size(); iA++) {
		try {
		    //Create socket
		    if(type == SOCK_TCP) {
			if((sockFd=socket((((sockaddr*)&addrs[iA])->sa_family==AF_INET6)?PF_INET6:PF_INET,SOCK_STREAM,0)) == -1)
			    throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), "TCP", strerror(errno), errno);
			int vl = 1; setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
			if(MSS()) { vl = MSS(); setsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, &vl, sizeof(int)); }
		    }
		    else if(type == SOCK_UDP) {
			if((sockFd=socket((((sockaddr*)&addrs[iA])->sa_family==AF_INET6)?PF_INET6:PF_INET,SOCK_DGRAM,0)) == -1)
			    throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), "UDP", strerror(errno), errno);
		    }

		    //Get the connected address
		    if(((sockaddr*)&addrs[iA])->sa_family == AF_INET6) {
			char aBuf[INET6_ADDRSTRLEN];
			getnameinfo((sockaddr*)&addrs[iA], sizeof(addrs[iA]), aBuf, sizeof(aBuf), 0, 0, NI_NUMERICHOST);
			connAddr = aBuf;
		    } else connAddr = inet_ntoa(((sockaddr_in*)&addrs[iA])->sin_addr);

		    if(logLen()) pushLogMess(TSYS::strMess(_("Connecting to '%s'"), connAddr.c_str()));

		    //Connect to the socket
		    int flags = fcntl(sockFd, F_GETFL, 0);
		    fcntl(sockFd, F_SETFL, flags|O_NONBLOCK);
		    int rez = connect(sockFd, (sockaddr*)&addrs[iA], sizeof(addrs[iA]));
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
			if(mess_lev() == TMess::Debug)
			    mess_debug(nodePath().c_str(), _("Error connecting during the time %s: '%s (%d)'"), tm2s(1e-3*itmCon).c_str(), strerror(errno), errno);
			throw TError(nodePath().c_str(), _("Error connecting to the internet socket '%s:%s' during the timeout, it seems in down or inaccessible: '%s (%d)'!"),
			    host_.c_str(), port.c_str(), strerror(errno), errno);
		    }
		} catch(TError &err) {
		    aErr = err.mess;
		    if(sockFd >= 0) close(sockFd);
		    sockFd = -1;
		    continue;	//Try next
		}
		break;	//OK
	    }
	    if(sockFd >= 0) break;
	}

	if(sockFd < 0) throw TError(nodePath().c_str(), "%s", aErr.c_str());
    }
    else if(type == SOCK_UNIX) {
	string path = TSYS::strParse(addr_, 0, ":", &aOff);
	if(!path.size()) path = "/tmp/oscada";
	memset(&nameUn, 0, sizeof(nameUn));
	nameUn.sun_family = AF_UNIX;
	strncpy(nameUn.sun_path, path.c_str(), sizeof(nameUn.sun_path));

	//Create socket
	if((sockFd=socket(PF_UNIX,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error creating the %s socket: '%s (%d)'!"), "UNIX", strerror(errno), errno);
	if(connect(sockFd,(sockaddr*)&nameUn,sizeof(nameUn)) == -1) {
	    close(sockFd);
	    sockFd = -1;
	    throw TError(nodePath().c_str(), _("Error connecting the %s socket: '%s (%d)'!"), "UNIX", strerror(errno), errno);
	}
	fcntl(sockFd, F_SETFL, fcntl(sockFd,F_GETFL,0)|O_NONBLOCK);
    }
#ifdef HAVE_LINUX_CAN_H
    else if(type == SOCK_RAWCAN) {
	if((sockFd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
	    throw TError(nodePath().c_str(), _("Error creating the %s socket!"), s_type.c_str());
	int flags = fcntl(sockFd, F_GETFL, 0);
	fcntl(sockFd, F_SETFL, flags | O_NONBLOCK);;
	string path = TSYS::strParse(addr_, 0, ":", &aOff);
	struct can_filter rfilter;
	rfilter.can_id = s2i(TSYS::strParse(addr_,0,":",&aOff).c_str());
	rfilter.can_mask = s2i(TSYS::strParse(addr_,0,":",&aOff).c_str());
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
	    throw TError(nodePath().c_str(), _("RAWCAN socket doesn't bind to '%s'!"), addr_.c_str());
	}
    }
#endif

    } catch(TError &err) {
	if(logLen()) pushLogMess(TSYS::strMess(_("Error starting: %s"),err.mess.c_str()));
	throw;
    }

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
	sockFd = -1;
	//To prevent of reusing the closed socket at a possible start
	if(type == SOCK_FORCE)	{ runSt = false; setAddr("SOCK:-1"); modifClr(); }
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
    bool notReq = (time < 0),
	 writeReq = false;
    time = abs(time);

    MtxAlloc res(reqRes(), true);

    int prevTmOut = 0;
    if(time) { prevTmOut = tmCon(); setTmCon(time); }

    try {
	if(!runSt) throw TError(nodePath().c_str(),_("Transport is not started!"));

repeate:
	if(reqTry++ >= attempts()) { mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(), _("Error requesting: %s"), err.c_str()); }
	int64_t stRespTm = 0;

	//Write request
	writeReq = false;
	if(oBuf != NULL && oLen > 0) {
	    if(!time) time = mTmCon;

	    // Input buffer clear
	    char tbuf[100];
	    while(!notReq && read(sockFd,tbuf,sizeof(tbuf)) > 0) ;
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
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error writing: %s"), err.c_str());
		    if(logLen()) pushLogMess(TSYS::strMess(_("Error writing: %s"), err.c_str()));
		    if(notReq) throw TError(nodePath().c_str(),_("Error writing: %s"), err.c_str());
		    start();
		    goto repeate;
		}
		else {
		    trOut += kz;
		    if(logLen()) pushLogMess(_("Transmitted to\n") + string(oBuf+wOff,kz));
		}

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Wrote %s."), TSYS::cpct2str(oLen).c_str());

	    writeReq = true;

	    if(mess_lev() == TMess::Debug) stRespTm = SYS->curTime();
	}
	else if(!notReq) time = mTmNext;
	if(!time) time = 5000;

	//Read reply
	if(iBuf != NULL && iLen > 0) {
	    tv.tv_sec  = time/1000; tv.tv_usec = 1000*(time%1000);
	    FD_ZERO(&rw_fd); FD_SET(sockFd, &rw_fd);
	    kz = select(sockFd+1, &rw_fd, NULL, NULL, &tv);
	    if(kz == 0) {
		if(!notReq) {
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Reading timeouted."));
		    if(logLen()) pushLogMess(_("Reading timeouted."));
		    if(writeReq) stop();
		}
		mLstReqTm = TSYS::curTime();
		throw TError(nodePath().c_str(),_("Timeouted!"));
	    }
	    else if(kz < 0) {
		err = TSYS::strMess("%s (%d)", strerror(errno), errno);
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading (select): %s"), err.c_str());
		if(logLen()) pushLogMess(TSYS::strMess(_("Error reading (select): %s"), err.c_str()));
		stop();
		mLstReqTm = TSYS::curTime();
		throw TError(nodePath().c_str(),_("Error reading (select): %s"), err.c_str());
	    }
	    else if(FD_ISSET(sockFd,&rw_fd)) {
		//!! Reading in that way but some time read() return < 0 after the select() pass.
		// * Force waiting any data in the request mode and not EAGAIN
		for(int iRtr = 0; (iB=read(sockFd,iBuf,iLen)) < 0 && errno == EAGAIN && iRtr < /*time*/mTmNext; ++iRtr)
		    TSYS::sysSleep(1e-3);

		//!! Reading in that way but some time read() return 0 after the select() pass.
		//!!> Commented due there is not any sense to wait any data after zero return, which proven on: VPN
		// * Force wait any data in the request mode or EAGAIN
		// * No wait any data in the not request mode but it can get the data later
		/*bool stZero = false;
		for(int iRtr = 0; (((iB=read(sockFd,iBuf,iLen)) == 0 && !notReq) || (iB < 0 && errno == EAGAIN)) && iRtr < mTmNext; ++iRtr) {
		    stZero = (iB == 0);
		    if(iRtr == 1 && iB == 0) {	//Check for same socket's errors
			int sockError = 0, sockLen = sizeof(sockError);
			getsockopt(sockFd, SOL_SOCKET, SO_ERROR, (char*)&sockError, (socklen_t*)&sockLen);
			if(sockError && sockError != EAGAIN) { errno = sockError; break; }
		    }
		    TSYS::sysSleep(1e-3);
		}
		if(stZero && iB > 0)	printf("TEST 00: Have waited after zero for %d.\n", iB);*/

		// * Force errors
		// * Retry if any data was wrote but no a reply there into the request mode
		// * !!: Zero can be also after disconection by peer and possible undetected here for the not request mode
		if(iB < 0 || (iB == 0 && writeReq && !notReq)) {
		    err = (iB < 0) ? TSYS::strMess("%s (%d)",strerror(errno),errno) : TSYS::strMess(_("No data by: %s (%d)"),strerror(errno),errno);
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading: %s"), err.c_str());
		    if(logLen()) pushLogMess(TSYS::strMess(_("Error reading: %s"), err.c_str()));
		    stop();
		    // * Pass to retry into the request mode and on the successful writing
		    if(!writeReq || notReq) throw TError(nodePath().c_str(),_("Error reading: %s"), err.c_str());
		    start();
		    goto repeate;
		}
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read %s."), TSYS::cpct2str(vmax(0,iB)).c_str());
		if(iB > 0 && logLen()) pushLogMess(_("Received from\n") + string(iBuf,iB));
		if(iB > 0 && mess_lev() == TMess::Debug && stRespTm) {
		    respTm = SYS->curTime() - stRespTm;
		    respTmMax = vmax(respTmMax, respTm);
		}
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
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1, "help",owner().outAddrHelp().c_str());
	if(type == SOCK_TCP)
	    ctrMkNode("fld",opt,-1,"/prm/cfg/MSS",_("Maximum segment size (MSS)"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,
		"tp","str","help",_("Set 0 for the system MSS."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2, "tp","str", "help",((TTransSock&)owner()).outTimingsHelp().c_str());
	ctrMkNode("fld",opt,-1,"/prm/cfg/attempts",_("Attempts"),RWRWR_,"root",STR_ID,2, "tp","dec", "help",((TTransSock&)owner()).outAttemptsHelp().c_str());
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
    else if(a_path == "/prm/cfg/attempts") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(attempts()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setAttempts(s2i(opt->text()));
    }
    else TTransportOut::cntrCmdProc(opt);
}

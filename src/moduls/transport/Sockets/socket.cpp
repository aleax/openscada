
//OpenSCADA system module Transport.Sockets file: socket.cpp
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <errno.h>

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>
#include "socket.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"Sockets"
#define MOD_NAME	_("Sockets")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"2.2.5"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides sockets based transport. Support inet and unix sockets. Inet socket uses TCP, and UDP protocols.")
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
TTransSock::TTransSock( string name ) : TTipTransport(MOD_ID)
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

TTransportIn *TTransSock::In( const string &name, const string &idb )	{ return new TSocketIn(name, idb, &owner().inEl()); }

TTransportOut *TTransSock::Out( const string &name, const string &idb )	{ return new TSocketOut(name, idb, &owner().outEl()); }

//************************************************
//* TSocketIn                                    *
//************************************************
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), sockRes(true), mMSS(0), mMaxQueue(10), mMaxFork(20), mMaxForkPerHost(0), mBufLen(5),
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

    if(startStat())
	rez += TSYS::strMess(_("Connections %d, opened %d, last %s. Traffic in %s, out %s. Closed connections by limit %d."),
				connNumb, clId.size(), tm2s(lastConn(),"").c_str(),
				TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str(), clsConnByLim);

    return rez;
}

void TSocketIn::load_( )
{
    TTransportIn::load_();

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
    trIn = trOut = 0;
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
	    mode = s2i(TSYS::strSepParse(addr(),3,':'));
	    //Get system port for "oscada" /etc/services
	    if(getservbyname_r(port.c_str(),"tcp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
		nameIn.sin_port = sp->s_port;
	    else if(htons(s2i(port)) > 0)	nameIn.sin_port = htons(s2i(port));
	    else nameIn.sin_port = 10005;

	    if(bind(sockFd,(sockaddr*)&nameIn,sizeof(nameIn)) == -1) {	//Wait connection
		int rez = errno;
		shutdown(sockFd, SHUT_RDWR);
		close(sockFd);
		throw TError(nodePath().c_str(), _("Bind to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
	    }
	    listen(sockFd, maxQueue());
	}
	else if(type == SOCK_UDP) {
	    //Get system port for "oscada" /etc/services
	    if(getservbyname_r(port.c_str(),"udp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
		nameIn.sin_port = sp->s_port;
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
	mode	= s2i(TSYS::strSepParse(addr(),2,':'));
	if(!path.size()) path = "/tmp/oscada";
	remove(path.c_str());
	struct sockaddr_un  nameUn;
	memset(&nameUn, 0, sizeof(nameUn));
	nameUn.sun_family = AF_UNIX;
	strncpy(nameUn.sun_path, path.c_str(), sizeof(nameUn.sun_path));
	if(bind(sockFd,(sockaddr*)&nameUn,sizeof(nameUn)) == -1) {	//Wait connection
	    int rez = errno;
	    close(sockFd);
	    throw TError(nodePath().c_str(),_("Bind to %s socket error: '%s (%d)'!"), s_type.c_str(), strerror(rez), rez);
	}
	listen(sockFd, maxQueue());
    }

    SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);	//main task for processing or client task create
    runSt = true;

    TTransportIn::start();
}

void TSocketIn::stop( )
{
    if(!runSt) return;

    //Status clear
    trIn = trOut = 0;
    connNumb = clsConnByLim = 0;

    SYS->taskDestroy(nodePath('.',true), &endrun);
    runSt = false;

    shutdown(sockFd, SHUT_RDWR);
    close(sockFd);
    if(type == SOCK_UNIX) remove(path.c_str());

    TTransportIn::stop();
}

unsigned TSocketIn::forksPerHost( const string &sender )
{
    sockRes.lock();
    unsigned rez = clS[sender];
    sockRes.unlock();

    return rez;
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
	    req.assign(buf,r_len);

#if OSC_DEBUG >= 5
	    mess_debug(sock->nodePath().c_str(), _("Read datagram %s from '%s'!"), TSYS::cpct2str(r_len).c_str(), inet_ntoa(name_cl.sin_addr));
#endif

	    sock->messPut(sock->sockFd, req, answ, inet_ntoa(name_cl.sin_addr), prot_in);
	    if(!prot_in.freeStat()) continue;

#if OSC_DEBUG >= 5
	    mess_debug(sock->nodePath().c_str(), _("Wrote datagram %s to '%s'!"), TSYS::cpct2str(answ.size()).c_str(), inet_ntoa(name_cl.sin_addr));
#endif

	    r_len = sendto(sock->sockFd, answ.c_str(), answ.size(), 0, (sockaddr *)&name_cl, name_cl_len);
	    sock->trOut += vmax(0, r_len);
	}
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
    int tm = time(NULL);	//Last connection time

#if OSC_DEBUG >= 5
    mess_debug(s.s->nodePath().c_str(), _("Has been connected by '%s'!"), s.sender.c_str());
#endif

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
#if OSC_DEBUG >= 5
		mess_debug(s.s->nodePath().c_str(), _("Has been terminated by error: '%s (%d)'"), strerror(errno), errno);
#endif
		break;
	    }

	    ssize_t r_len = 0;
	    if(kz && (r_len=read(s.sock, buf, s.s->bufLen()*1000)) <= 0) break;
	    s.s->dataRes().lock();
	    s.s->trIn += r_len; s.trIn += r_len;
	    s.s->dataRes().unlock();

#if OSC_DEBUG >= 5
	    mess_debug(s.s->nodePath().c_str(), _("Read message %s from '%s'."), TSYS::cpct2str(r_len).c_str(), s.sender.c_str());
#endif
	    req.assign(buf, r_len);
	    s.s->messPut(s.sock, req, answ, s.sender, prot_in);

	    if(answ.size()) {
#if OSC_DEBUG >= 5
		mess_debug(s.s->nodePath().c_str(), _("Wrote message %s to '%s'."), TSYS::cpct2str(answ.size()).c_str(), s.sender.c_str());
#endif
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
			mess_err(s.s->nodePath().c_str(), _("Write: error '%s (%d)'!"), strerror(errno), errno);
			break;
		    }
		    s.s->dataRes().lock();
		    s.s->trOut += vmax(0, wL); s.trOut += vmax(0, wL);
		    s.s->dataRes().unlock();
		}
		answ = "";
	    }
	    cnt++;
	    s.tmReq = s.s->connTm = tm = time(NULL);
	    sessOk = true;
	} while(!s.s->endrunCl && (!s.s->keepAliveTm() || (time(NULL)-tm) < s.s->keepAliveTm()) &&
		(!sessOk || ((s.s->mode || !prot_in.freeStat()) && (!s.s->keepAliveReqs() || cnt < s.s->keepAliveReqs()))));

#if OSC_DEBUG >= 5
	mess_debug(s.s->nodePath().c_str(), _("Has been disconnected by '%s'!"), s.sender.c_str());
#endif
    } catch(TError &err) {
#if OSC_DEBUG >= 5
	mess_debug(s.s->nodePath().c_str(), _("Has been terminated by execution: %s"), err.mess.c_str());
#endif
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
	if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, this);
	prot_in = proto.at().at(n_pr);
#if OSC_DEBUG >= 5
	mess_debug(nodePath().c_str(), _("New input protocol's object '%s' created!"), n_pr.c_str());
#endif
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
	if(prot_in.at().mess(request,answer,sender)) return;
	if(proto.freeStat()) proto = AutoHD<TProtocol>(&prot_in.at().owner());
	n_pr = prot_in.at().name();
	prot_in.free();
	if(proto.at().openStat(n_pr)) proto.at().close(n_pr);
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
}

void TSocketIn::clientUnreg( SSockIn *so )
{
    MtxAlloc res(sockRes, true);
    for(map<int,SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId)
	if(iId->second == so) {
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
	    "    mode - work mode (0 - break connection; 1 - keep alive).\n"
	    "  UDP:{addr}:{port} - UDP socket:\n"
	    "    addr - address for socket to be opened, empty address opens socket for all interfaces;\n"
	    "    port - network port (/etc/services).\n"
	    "  UNIX:{name}:{mode} - UNIX socket:\n"
	    "    name - UNIX-socket's file name;\n"
	    "    mode - work mode (0 - break connection; 1 - keep alive)."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/PROT", EVAL_STR, startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1,
	    "help",_("Empty value for the protocol selection switchs the transport to mode\n"
		     "of creation associated output transports for each connection to one."));
	ctrMkNode("fld", opt, -1, "/prm/cfg/bfLn", _("Input buffer (kbyte)"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "tp","dec");
	ctrMkNode("fld", opt, -1, "/prm/cfg/taskPrior", _("Priority"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
	    "tp","dec", "help",TMess::labTaskPrior());
	if(addr().compare(0,4,"TCP:") == 0)
	    ctrMkNode("fld", opt, -1, "/prm/cfg/MSS", _("Maximum segment size (MSS)"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 2,
		"tp","str", "help",_("Set 0 for system MSS."));
	if(addr().compare(0,4,"TCP:") == 0 || addr().compare(0,5,"UNIX:") == 0) {
	    ctrMkNode("fld", opt, -1, "/prm/cfg/qLn", _("Queue length"), startStat()?R_R_R_:RWRWR_, "root", STR_ID, 1, "tp","dec");
	    ctrMkNode("fld", opt, -1, "/prm/cfg/clMax", _("Clients maximum"), RWRWR_, "root", STR_ID, 1, "tp","dec");
	    if(addr().compare(0,4,"TCP:") == 0)
		ctrMkNode("fld", opt, -1, "/prm/cfg/clMaxPerHost", _("Clients maximum, per host"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		    "help",_("Set to zero for disable that limit."));
	    ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveReqs", _("Keep alive requests"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		"help",_("Close the connection after specified requests.\nZero value for disable (not close ever)."));
	    ctrMkNode("fld", opt, -1, "/prm/cfg/keepAliveTm", _("Keep alive timeout (s)"), RWRWR_, "root", STR_ID, 2, "tp","dec",
		"help",_("Close the connection after no requests at specified timeout.\nZero value for disable (not close ever)."));
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/conns" && ctrChkNode(opt)) {
	MtxAlloc res(sockRes, true);
	for(map<int,SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId)
	    opt->childAdd("el")->setText(TSYS::strMess(_("%s %d(%s): last %s; traffic in %s, out %s."),
		tm2s(iId->second->tmCreate,"%Y-%m-%dT%H:%M:%S").c_str(),iId->first,iId->second->sender.c_str(),
		tm2s(iId->second->tmReq,"%Y-%m-%dT%H:%M:%S").c_str(),
		TSYS::cpct2str(iId->second->trIn).c_str(),TSYS::cpct2str(iId->second->trOut).c_str()));
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
TSocketOut::TSocketOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), mMSS(0), sockFd(-1), wres(true), mLstReqTm(0)
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
    TTransportOut::load_();

    try
    {
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
    MtxAlloc res(wres, true);

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

    if(s_type == S_NM_TCP)		type = SOCK_TCP;
    else if(s_type == S_NM_UDP)		type = SOCK_UDP;
    else if(s_type == S_NM_UNIX)	type = SOCK_UNIX;
    else throw TError(nodePath().c_str(), _("Type socket '%s' error!"), s_type.c_str());

    if(type == SOCK_TCP || type == SOCK_UDP) {
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
	if(getservbyname_r(port.c_str(),(type == SOCK_TCP)?"tcp":"udp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
	    nameIn.sin_port = sp->s_port;
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
#if OSC_DEBUG >= 5
	    mess_debug(nodePath().c_str(), _("Connect by timeout %s error: '%s (%d)'"), tm2s(1e3*itmCon).c_str(), strerror(errno), errno);
#endif
	    throw TError(nodePath().c_str(), _("Connect to Internet socket error: '%s (%d)'!"), strerror(errno), errno);
	}
    }
    else if(type == SOCK_UNIX)
    {
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

    mLstReqTm = TSYS::curTime();

    runSt = true;

    TTransportOut::start();
}

void TSocketOut::stop( )
{
    MtxAlloc res(wres, true);

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
}

int TSocketOut::messIO( const char *oBuf, int oLen, char *iBuf, int iLen, int time, bool noRes )
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

    ResAlloc resN(nodeRes());
    if(!noRes) resN.lock(true);
    MtxAlloc res(wres, true);

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
		    res.unlock();
		    stop();
#if OSC_DEBUG >= 5
		    mess_debug(nodePath().c_str(), _("Write error: %s"), err.c_str());
#endif
		    if(noReq) throw TError(nodePath().c_str(),_("Write error: %s"), err.c_str());
		    start();
		    res.lock();
		    goto repeate;
		} else trOut += kz;

#if OSC_DEBUG >= 5
	    mess_debug(nodePath().c_str(), _("Wrote %s."), TSYS::cpct2str(oLen).c_str());
#endif

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
		res.unlock();
		if(writeReq && !noReq) stop();
		mLstReqTm = TSYS::curTime();
#if OSC_DEBUG >= 5
		mess_debug(nodePath().c_str(), _("Read timeouted."));
#endif
		throw TError(nodePath().c_str(),_("Timeouted!"));
	    }
	    else if(kz < 0) {
		err = TSYS::strMess("%s (%d)", strerror(errno), errno);
		res.unlock();
		stop();
		mLstReqTm = TSYS::curTime();
#if OSC_DEBUG >= 5
		mess_debug(nodePath().c_str(), _("Read (select) error: %s"), err.c_str());
#endif
		throw TError(nodePath().c_str(),_("Read (select) error: %s"), err.c_str());
	    }
	    else if(FD_ISSET(sockFd,&rw_fd)) {
		//!! Reading in that way but some time read() return 0 after the select() pass.
		// * Force wait any data in the request mode or EAGAIN
		// * No wait any data in the not request mode but it can get the data later
		for(int iRtr = 0; (((iB=read(sockFd,iBuf,iLen)) == 0 && !noReq) || (iB < 0 && errno == EAGAIN)) && iRtr < mTmNext; ++iRtr)
		    TSYS::sysSleep(1e-3);
		// * Force errors
		// * Retry if any data was wrote but no a reply there into the request mode
		// * !!: Zero can be also after disconection by peer and possible undetected here for the not request mode
		if(iB < 0 || (iB == 0 && writeReq && !noReq)) {
		    err = (iB < 0) ? TSYS::strMess("%s (%d)", strerror(errno), errno): _("No data");
		    res.unlock();
		    stop();
#if OSC_DEBUG >= 5
		    mess_debug(nodePath().c_str(), _("Read error: %s"), err.c_str());
#endif
		    // * Pass to retry into the request mode and on the successful writing
		    if(!writeReq || noReq) throw TError(nodePath().c_str(),_("Read error: %s"), err.c_str());
		    start();
		    res.lock();
		    goto repeate;
		}
#if OSC_DEBUG >= 5
		mess_debug(nodePath().c_str(), _("Read %s."), TSYS::cpct2str(vmax(0,i_b)).c_str());
#endif
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
    if(opt->name() == "info")
    {
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

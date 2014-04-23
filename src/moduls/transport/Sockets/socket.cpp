
//OpenSCADA system module Transport.Sockets file: socket.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko                           *
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
#include <sys/un.h>
#include <netinet/in.h>
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
#define MOD_VER		"1.5.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow sockets based transport. Support inet and unix sockets. Inet socket use TCP and UDP protocols.")
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
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTransSock::~TTransSock( )	{ }

void TTransSock::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	owner().inEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
	owner().outEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
    }
}

void TTransSock::load_( )
{
    //Load parameters from command line

}

TTransportIn *TTransSock::In( const string &name, const string &idb )
{
    return new TSocketIn(name, idb, &owner().inEl());
}

TTransportOut *TTransSock::Out( const string &name, const string &idb )
{
    return new TSocketOut(name, idb, &owner().outEl());
}

//************************************************
//* TSocketIn                                    *
//************************************************
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), mMaxQueue(10), mMaxFork(10), mBufLen(5), mKeepAliveReqs(0), mKeepAliveTm(60), mTaskPrior(0), cl_free(true)
{
    setAddr("TCP:localhost:10002:0");
}

TSocketIn::~TSocketIn( )	{ }

string TSocketIn::getStatus( )
{
    string rez = TTransportIn::getStatus();

    if(startStat())
	rez += TSYS::strMess(_("Connections %d, opened %d. Traffic in %s, out %s. Closed connections by limit %d."),
				connNumb, (protocol().empty()?assTrs().size():cl_id.size()), TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str(), clsConnByLim);

    return rez;
}

void TSocketIn::load_( )
{
    TTransportIn::load_();

    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("MaxQueue");	if(!vl.empty()) setMaxQueue(atoi(vl.c_str()));
	vl = prmNd.attr("MaxClients");	if(!vl.empty()) setMaxFork(atoi(vl.c_str()));
	vl = prmNd.attr("BufLen");	if(!vl.empty()) setBufLen(atoi(vl.c_str()));
	vl = prmNd.attr("KeepAliveReqs");if(!vl.empty()) setKeepAliveReqs(atoi(vl.c_str()));
	vl = prmNd.attr("KeepAliveTm");	if(!vl.empty()) setKeepAliveTm(atoi(vl.c_str()));
	vl = prmNd.attr("TaskPrior");	if(!vl.empty()) setTaskPrior(atoi(vl.c_str()));
    } catch(...){ }
}

void TSocketIn::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("MaxQueue", i2s(maxQueue()));
    prmNd.setAttr("MaxClients", i2s(maxFork()));
    prmNd.setAttr("BufLen", i2s(bufLen()));
    prmNd.setAttr("KeepAliveReqs", i2s(keepAliveReqs()));
    prmNd.setAttr("KeepAliveTm", i2s(keepAliveTm()));
    prmNd.setAttr("TaskPrior", i2s(taskPrior()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportIn::save_();
}

void TSocketIn::start( )
{
    if(run_st) return;

    //Status clear
    trIn = trOut = 0;
    connNumb = clsConnByLim = 0;

    //Socket init
    string s_type = TSYS::strSepParse(addr(),0,':');

    if(s_type == S_NM_TCP)
    {
	if((sock_fd=socket(PF_INET,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(),_("Error create '%s' socket!"),s_type.c_str());
	int vl = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
	type = SOCK_TCP;
    }
    else if(s_type == S_NM_UDP)
    {
	if((sock_fd=socket(PF_INET,SOCK_DGRAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error create '%s' socket!"), s_type.c_str());
	type = SOCK_UDP;
    }
    else if(s_type == S_NM_UNIX)
    {
	if((sock_fd = socket(PF_UNIX,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error create '%s' socket!"), s_type.c_str());
	type = SOCK_UNIX;
    }
    else throw TError(nodePath().c_str(), _("Socket type '%s' error!"), s_type.c_str());

    if(type == SOCK_TCP || type == SOCK_UDP)
    {
	struct sockaddr_in  name_in;
	struct hostent *loc_host_nm;
	memset(&name_in, 0, sizeof(name_in));
	name_in.sin_family = AF_INET;

	host	= TSYS::strSepParse(addr(), 1, ':');
	port	= TSYS::strSepParse(addr(), 2, ':');
	if(host.size())
	{
	    loc_host_nm = gethostbyname(host.c_str());
	    if(loc_host_nm == NULL || loc_host_nm->h_length == 0)
		throw TError(nodePath().c_str(), _("Socket name '%s' error!"), host.c_str());
	    name_in.sin_addr.s_addr = *((int*)(loc_host_nm->h_addr_list[0]));
	}
	else name_in.sin_addr.s_addr = INADDR_ANY;
	if(type == SOCK_TCP)
	{
	    mode = atoi(TSYS::strSepParse(addr(),3,':').c_str());
	    //Get system port for "oscada" /etc/services
	    struct servent *sptr = getservbyname(port.c_str(),"tcp");
	    if(sptr != NULL)                       name_in.sin_port = sptr->s_port;
	    else if(htons(atol(port.c_str())) > 0) name_in.sin_port = htons(atol(port.c_str()));
	    else name_in.sin_port = 10001;

	    if(bind(sock_fd,(sockaddr*)&name_in,sizeof(name_in)) == -1)
	    {
		shutdown(sock_fd, SHUT_RDWR);
		close(sock_fd);
		throw TError(nodePath().c_str(), _("TCP socket doesn't bind to '%s'!"), addr().c_str());
	    }
	    listen(sock_fd, maxQueue());
	}
	else if(type == SOCK_UDP)
	{
	    //Get system port for "oscada" /etc/services
	    struct servent *sptr = getservbyname(port.c_str(), "udp");
	    if(sptr != NULL)                       name_in.sin_port = sptr->s_port;
	    else if(htons(atol(port.c_str())) > 0) name_in.sin_port = htons(atol(port.c_str()));
	    else name_in.sin_port = 10001;

	    if(bind(sock_fd,(sockaddr*)&name_in,sizeof(name_in)) == -1)
	    {
		shutdown(sock_fd, SHUT_RDWR);
		close(sock_fd);
		throw TError(nodePath().c_str(), _("UDP socket doesn't bind to '%s'!"), addr().c_str());
	    }
	}
    }
    else if(type == SOCK_UNIX)
    {
	path	= TSYS::strSepParse(addr(), 1, ':');
	mode	= atoi(TSYS::strSepParse(addr(),2,':').c_str());
	if(!path.size()) path = "/tmp/oscada";
	remove(path.c_str());
	struct sockaddr_un  name_un;
	memset(&name_un, 0, sizeof(name_un));
	name_un.sun_family = AF_UNIX;
	strncpy(name_un.sun_path, path.c_str(), sizeof(name_un.sun_path));
	if(bind(sock_fd,(sockaddr*)&name_un,sizeof(name_un)) == -1)
	{
	    close(sock_fd);
	    throw TError(nodePath().c_str(),_("UNIX socket doesn't bind to '%s'!"),addr().c_str());
	}
	listen(sock_fd, maxQueue());
    }

    SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);

    TTransportIn::start();
}

void TSocketIn::stop( )
{
    if(!run_st) return;

    //Status clear
    trIn = trOut = 0;
    connNumb = clsConnByLim = 0;

    SYS->taskDestroy(nodePath('.',true), &endrun);

    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    if(type == SOCK_UNIX) remove(path.c_str());

    TTransportIn::stop();
}

int TSocketIn::writeTo( const string &sender, const string &data )
{
    fd_set		rw_fd;
    struct timeval	tv;

    ResAlloc resN(nodeRes(), true);	//Prevent simultaneous writing

    switch(type)
    {
	case SOCK_TCP: case SOCK_UNIX:
	{
	    int sId = atoi(TSYS::strLine(sender,1).c_str());
	    if(sId < 0) return -1;
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(),_("Socket write message '%d'."), data.size());
	    ssize_t wL = 1;
	    unsigned wOff = 0;
	    for( ; wOff != data.size() && wL > 0; wOff += wL)
	    {
		wL = write(sId, data.data()+wOff, data.size()-wOff);
		if(wL == 0) { mess_err(nodePath().c_str(), _("Write: reply for zero bytes.")); break; }
		else if(wL < 0)
		{
		    if(errno == EAGAIN)
		    {
			tv.tv_sec = 1; tv.tv_usec = 0;		//!!!! Where the time take?
			FD_ZERO(&rw_fd); FD_SET(sId, &rw_fd);
			int kz = select(sId+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(sId,&rw_fd)) { wL = 0; continue; }
		    }
		    mess_err(nodePath().c_str(), _("Write: error '%s (%d)'!"), strerror(errno), errno);
		    break;
		}
		sock_res.resRequestW();
		trOut += vmax(0,wL);
		sock_res.resRelease();
	    }
	    return wOff;
	}
    }

    return 0;
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

    sock->run_st    = true;
    sock->endrun_cl = false;
    sock->endrun    = false;

    if(sock->type == SOCK_UDP) buf = new char[sock->bufLen()*1024 + 1];

    while(!sock->endrun)
    {
	tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	FD_ZERO(&rd_fd); FD_SET(sock->sock_fd,&rd_fd);

	int kz = select(sock->sock_fd+1, &rd_fd, NULL, NULL, &tv);
	if(kz < 0 && errno != EINTR)
	{
	    mess_err(sock->nodePath().c_str(), _("Close input transport by error: %s"), strerror(errno));
	    break;
	}
	if(kz <= 0 || !FD_ISSET(sock->sock_fd, &rd_fd)) continue;

	struct sockaddr_in name_cl;
	socklen_t          name_cl_len = sizeof(name_cl);
	if(sock->type == SOCK_TCP)
	{
	    int sock_fd_CL = accept(sock->sock_fd, (sockaddr *)&name_cl, &name_cl_len);
	    if(sock_fd_CL != -1)
	    {
		if(sock->maxFork() <= (int)sock->cl_id.size())
		{
		    sock->clsConnByLim++;
		    close(sock_fd_CL);
		    continue;
		}
		//Create presenting the client connection output transport
		if(sock->protocol().empty() && (int)sock->assTrs().size() <= sock->maxFork())
		{
		    sock->assTrO("SOCK:"+i2s(sock_fd_CL));
		    sock->connNumb++;
		    continue;
		}

		//Thread create for input requests process.
		SSockIn *sin = new SSockIn(sock, sock_fd_CL, inet_ntoa(name_cl.sin_addr));
		try
		{
		    SYS->taskCreate(sock->nodePath('.',true)+"."+i2s(sock_fd_CL), sock->taskPrior(), ClTask, sin, 5, &pthr_attr);
		    sock->connNumb++;
		}
		catch(TError err)
		{
		    delete sin;
		    mess_err(err.cat.c_str(), err.mess.c_str());
		    mess_err(sock->nodePath().c_str(), _("Error creation of the thread!"));
		}
	    }
	}
	else if(sock->type == SOCK_UNIX)
	{
	    int sock_fd_CL = accept(sock->sock_fd, NULL, NULL);
	    if(sock_fd_CL != -1)
	    {
		if(sock->maxFork() <= (int)sock->cl_id.size())
		{
		    sock->clsConnByLim++;
		    close(sock_fd_CL);
		    continue;
		}
		SSockIn *sin = new SSockIn(sock, sock_fd_CL, "");
		try
		{
		    SYS->taskCreate(sock->nodePath('.',true)+"."+i2s(sock_fd_CL), sock->taskPrior(), ClTask, sin, 5, &pthr_attr);
		    sock->connNumb++;
		}
		catch(TError err)
		{
		    delete sin;
		    mess_err(err.cat.c_str(), err.mess.c_str());
		    mess_err(sock->nodePath().c_str(), _("Error creation of the thread!"));
		}
	    }
	}
	else if(sock->type == SOCK_UDP)
	{
	    string req, answ;

	    ssize_t r_len = recvfrom(sock->sock_fd, buf, sock->bufLen()*1000, 0, (sockaddr*)&name_cl, &name_cl_len);
	    if(r_len <= 0) continue;
	    sock->trIn += r_len;
	    req.assign(buf, r_len);

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Socket received datagram '%d' from '%s'!"), r_len, inet_ntoa(name_cl.sin_addr));

	    sock->messPut(sock->sock_fd, req, answ, inet_ntoa(name_cl.sin_addr), prot_in);
	    if(!prot_in.freeStat()) continue;

	    if(mess_lev() == TMess::Debug)
		mess_debug(sock->nodePath().c_str(), _("Socket replied datagram '%d' to '%s'!"), answ.size(), inet_ntoa(name_cl.sin_addr));

	    r_len = sendto(sock->sock_fd, answ.c_str(), answ.size(), 0, (sockaddr *)&name_cl, name_cl_len);
	    sock->trOut += vmax(0,r_len);
	}
    }
    pthread_attr_destroy(&pthr_attr);

    if(sock->type == SOCK_UDP) delete []buf;
    //Client tasks stop command
    sock->endrun_cl = true;
    ResAlloc res(sock->sock_res,false);
    //Find already registry
    for(unsigned i_id = 0; i_id < sock->cl_id.size(); i_id++)
        pthread_kill(sock->cl_id[i_id].cl_id, SIGALRM);
    res.release();
    TSYS::eventWait(sock->cl_free, true, string(MOD_ID)+": "+sock->id()+_(" client task is stopping...."));

    sock->run_st = false;

    return NULL;
}

void *TSocketIn::ClTask( void *s_inf )
{
    SSockIn &s = *(SSockIn*)s_inf;
    int cnt = 0;		//Requests counter
    int tm = time(NULL);	//Last connection time

    if(mess_lev() == TMess::Debug)
	mess_debug(s.s->nodePath().c_str(),_("Socket has been connected by '%s'!"),s.sender.c_str());

    s.s->clientReg(pthread_self(), s.cSock);

    //Client socket process
    struct  timeval tv;
    fd_set  rw_fd;
    string  req, answ;
    char    buf[s.s->bufLen()*1000 + 1];
    AutoHD<TProtocolIn> prot_in;
    bool sessOk = false;

    do
    {
	tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	FD_ZERO(&rw_fd); FD_SET(s.cSock,&rw_fd);
	int kz = select(s.cSock+1, &rw_fd, NULL, NULL, &tv);
	if(kz == 0 || (kz == -1 && errno == EINTR) || kz < 0 || !FD_ISSET(s.cSock, &rw_fd)) continue;
	ssize_t r_len = read(s.cSock, buf, s.s->bufLen()*1000);
	if(r_len <= 0) break;
	s.s->sock_res.resRequestW();
	s.s->trIn += r_len;
	s.s->sock_res.resRelease();

	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Socket received message '%d' from '%s'."), r_len, s.sender.c_str());
	req.assign(buf, r_len);

	s.s->messPut(s.cSock, req, answ, s.sender, prot_in);
	if(answ.size())
	{
	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("Socket replied message '%d' to '%s'."), answ.size(), s.sender.c_str());
	    ssize_t wL = 1;
	    for(unsigned wOff = 0; wOff != answ.size() && wL > 0; wOff += wL)
	    {
		wL = write(s.cSock, answ.data()+wOff, answ.size()-wOff);
		if(wL == 0) { mess_err(s.s->nodePath().c_str(), _("Write: reply for zero bytes.")); break; }
		else if(wL < 0)
		{
		    if(errno == EAGAIN)
		    {
			tv.tv_sec = 1; tv.tv_usec = 0;		//!!!! Where the time get?
			FD_ZERO(&rw_fd); FD_SET(s.cSock, &rw_fd);
			kz = select(s.cSock+1, NULL, &rw_fd, NULL, &tv);
			if(kz > 0 && FD_ISSET(s.cSock,&rw_fd)) { wL = 0; continue; }
		    }
		    mess_err(s.s->nodePath().c_str(), _("Write: error '%s (%d)'!"), strerror(errno), errno);
		    break;
		}
		s.s->sock_res.resRequestW();
		s.s->trOut += vmax(0, wL);
		s.s->sock_res.resRelease();
	    }
	    answ = "";
	}
	cnt++;
	tm = time(NULL);
	sessOk = true;
    }while(!s.s->endrun_cl && (!s.s->keepAliveTm() || (time(NULL)-tm) < s.s->keepAliveTm()) &&
	    (!sessOk || ((s.s->mode || !prot_in.freeStat()) && (!s.s->keepAliveReqs() || cnt < s.s->keepAliveReqs()))));

    //Close protocol on broken connection
    if(!prot_in.freeStat())
    {
	string n_pr = prot_in.at().name();
	AutoHD<TProtocol> proto = AutoHD<TProtocol>(&prot_in.at().owner());
	prot_in.free();
	proto.at().close(n_pr);
    }

    s.s->clientUnreg(pthread_self());

    if(mess_lev() == TMess::Debug)
	mess_debug(s.s->nodePath().c_str(),_("Socket has been disconnected by '%s'!"),s.sender.c_str());

    delete (SSockIn*)s_inf;

    return NULL;
}

void TSocketIn::messPut( int sock, string &request, string &answer, string sender, AutoHD<TProtocolIn> &prot_in )
{
    AutoHD<TProtocol> proto;
    string n_pr;
    try
    {
	if(prot_in.freeStat())
	{
	    proto = SYS->protocol().at().modAt(protocol());
	    n_pr = id() + i2s(sock);
	    if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, this, sender+"\n"+i2s(sock));
	    prot_in = proto.at().at(n_pr);
	}
	if(prot_in.at().mess(request,answer)) return;
	if(proto.freeStat()) proto = AutoHD<TProtocol>(&prot_in.at().owner());
	n_pr = prot_in.at().name();
	prot_in.free();
	if(proto.at().openStat(n_pr)) proto.at().close(n_pr);
    }
    catch(TError err)
    {
	if(!prot_in.freeStat())
	{
	    if(proto.freeStat()) proto = AutoHD<TProtocol>(&prot_in.at().owner());
	    n_pr = prot_in.at().name();
	}
	prot_in.free();
	if(!proto.freeStat() && proto.at().openStat(n_pr)) proto.at().close(n_pr);

	mess_err(nodePath().c_str(), "%s", err.mess.c_str());
	mess_err(nodePath().c_str(), _("Error request to protocol."));
    }
}

void TSocketIn::clientReg( pthread_t thrid, int i_sock )
{
    ResAlloc res(sock_res, true);
    //Find already registry
    for(unsigned i_id = 0; i_id < cl_id.size(); i_id++)
	if(cl_id[i_id].cl_id == thrid) return;
    SSockCl scl = { thrid, i_sock };
    cl_id.push_back(scl);
    cl_free = false;
}

void TSocketIn::clientUnreg( pthread_t thrid )
{
    ResAlloc res(sock_res, true);
    for(unsigned i_id = 0; i_id < cl_id.size(); i_id++)
	if(cl_id[i_id].cl_id == thrid)
	{
	    shutdown(cl_id[i_id].cl_sock, SHUT_RDWR);
	    close(cl_id[i_id].cl_sock);
	    cl_id.erase(cl_id.begin() + i_id);
	    if(!cl_id.size()) cl_free = true;
	    break;
	}
}

void TSocketIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TTransportIn::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID,1,"help",
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
	ctrMkNode("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID);
	ctrMkNode("fld",opt,-1,"/prm/cfg/qLn",_("Queue length"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Used for TCP and UNIX sockets."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/clMax",_("Clients maximum"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Used for TCP and UNIX sockets."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/bfLn",_("Input buffer (kbyte)"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/prm/cfg/taskPrior",_("Priority"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,4,
            "tp","dec","min","-1","max","99","help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/prm/cfg/keepAliveReqs",_("Keep alive requests"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Close the connection after specified requests.\nZero value for disable (not close ever)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/keepAliveTm",_("Keep alive timeout (s)"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Close the connection after no requests at specified timeout.\nZero value for disable (not close ever)."));
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/qLn")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxQueue()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxQueue(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/clMax")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxFork()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxFork(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/bfLn")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(bufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setBufLen(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/keepAliveReqs")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveReqs()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveReqs(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/keepAliveTm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/taskPrior")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(taskPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTaskPrior(atoi(opt->text().c_str()));
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TSocketOut                                   *
//************************************************
TSocketOut::TSocketOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), sock_fd(-1), mLstReqTm(0)
{
    setAddr("TCP:localhost:10002");
    setTimings("5:0.1");
}

TSocketOut::~TSocketOut( )	{ }

void TSocketOut::setTimings( const string &vl )
{
    mTmCon = vmax(1, vmin(60000,(int)(atof(TSYS::strParse(vl,0,":").c_str())*1e3)));
    mTmNext = vmax(1, vmin(60000,(int)(atof(TSYS::strParse(vl,1,":").c_str())*1e3)));
    mTmRep = vmax(0, vmin(10000,(int)(atof(TSYS::strParse(vl,2,":").c_str())*1e3)));
    mTimings = mTmRep ? TSYS::strMess("%g:%g:%g",(1e-3*mTmCon),(1e-3*mTmNext),(1e-3*mTmRep)) :
			TSYS::strMess("%g:%g",(1e-3*mTmCon),(1e-3*mTmNext));
    modif();
}

void TSocketOut::setAddr( const string &addr )
{
    TTransportOut::setAddr(addr);
    if(startStat()) stop();
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
	vl = prmNd.attr("tms");	if(!vl.empty()) setTimings(vl);
    } catch(...){ }
}

void TSocketOut::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("tms",timings());
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportOut::save_();
}

void TSocketOut::start( )
{
    ResAlloc res(wres, true);

    if(run_st) return;

    //Reconnect try after 2*tmCon()
    /*if((TSYS::curTime()-mLstReqTm) < 2000ll*tmCon())
	throw TError(nodePath().c_str(),_("Try next connect after '%g' seconds!"),(2e-3*tmCon()-1e-6*(TSYS::curTime()-mLstReqTm)));
    mLstReqTm = TSYS::curTime();*/

    //Status clear
    trIn = trOut = 0;

    //Connect to remote host
    string s_type = TSYS::strSepParse(addr(), 0, ':');

    if(s_type == S_NM_SOCK)	type = SOCK_FORCE;
    else if(s_type == S_NM_TCP)	type = SOCK_TCP;
    else if(s_type == S_NM_UDP)	type = SOCK_UDP;
    else if(s_type == S_NM_UNIX)type = SOCK_UNIX;
    else throw TError(nodePath().c_str(),_("Type socket '%s' error!"),s_type.c_str());

    if(type == SOCK_FORCE)
    {
	sock_fd = atoi(TSYS::strSepParse(addr(),1,':').c_str());
	int rez;
	if((rez=fcntl(sock_fd,F_GETFL,0)) < 0 || fcntl(sock_fd,F_SETFL,rez|O_NONBLOCK) < 0)
	{
	    close(sock_fd);
	    throw TError(nodePath().c_str(), _("Error force socket %d using: %s!"), sock_fd, strerror(errno));
	}
    }
    else if(type == SOCK_TCP || type == SOCK_UDP)
    {
	memset(&name_in, 0, sizeof(name_in));
	name_in.sin_family = AF_INET;

	string host = TSYS::strSepParse(addr(), 1, ':');
	string port = TSYS::strSepParse(addr(), 2, ':');
	if(host.size())
	{
	    struct hostent *loc_host_nm = gethostbyname(host.c_str());
	    if(loc_host_nm == NULL || loc_host_nm->h_length == 0)
		throw TError(nodePath().c_str(),_("Socket name '%s' error!"),host.c_str());
	    name_in.sin_addr.s_addr = *((int*)(loc_host_nm->h_addr_list[0]));
	}
	else name_in.sin_addr.s_addr = INADDR_ANY;
	//Get system port for "oscada" /etc/services
	struct servent *sptr = getservbyname(port.c_str(), (type == SOCK_TCP)?"tcp":"udp");
	if(sptr != NULL)                       name_in.sin_port = sptr->s_port;
	else if(htons(atol(port.c_str())) > 0) name_in.sin_port = htons(atol(port.c_str()));
	else name_in.sin_port = 10001;

	//Create socket
	if(type == SOCK_TCP)
	{
	    if((sock_fd=socket(PF_INET,SOCK_STREAM,0)) == -1)
		throw TError(nodePath().c_str(), _("Error creation TCP socket: %s!"), strerror(errno));
	    int vl = 1;
	    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
	}
	else if(type == SOCK_UDP)
	{
	    if((sock_fd=socket(PF_INET,SOCK_DGRAM,0)) == -1)
		throw TError(nodePath().c_str(), _("Error creation UDP socket: %s!"), strerror(errno));
	}
	//Connect to socket
	int flags = fcntl(sock_fd, F_GETFL, 0);
	fcntl(sock_fd, F_SETFL, flags|O_NONBLOCK);
	int res = connect(sock_fd, (sockaddr*)&name_in, sizeof(name_in));
	if(res == -1 && errno == EINPROGRESS)
	{
	    struct timeval tv;
	    socklen_t slen = sizeof(res);
	    fd_set fdset;
	    tv.tv_sec = tmCon()/1000; tv.tv_usec = 1000*(tmCon()%1000);
	    FD_ZERO(&fdset); FD_SET(sock_fd, &fdset);
	    if((res=select(sock_fd+1, NULL, &fdset, NULL, &tv)) > 0 && !getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,&res,&slen) && !res) res = 0;
	    else res = -1;
	}
	if(res)
	{
	    close(sock_fd);
	    sock_fd = -1;
	    throw TError(nodePath().c_str(),_("Connect to Internet socket error: %s!"),strerror(errno));
	}
    }
    else if(type == SOCK_UNIX)
    {
	string path = TSYS::strSepParse(addr(), 1, ':');
	if(!path.size()) path = "/tmp/oscada";
	memset(&name_un, 0, sizeof(name_un));
	name_un.sun_family = AF_UNIX;
	strncpy(name_un.sun_path, path.c_str(), sizeof(name_un.sun_path));

	//Create socket
	if((sock_fd=socket(PF_UNIX,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error creation UNIX socket: %s!"), strerror(errno));
	if(connect(sock_fd,(sockaddr*)&name_un,sizeof(name_un)) == -1)
	{
	    close(sock_fd);
	    sock_fd = -1;
	    throw TError(nodePath().c_str(), _("Connect to UNIX error: %s!"), strerror(errno));
	}
	fcntl(sock_fd, F_SETFL, fcntl(sock_fd,F_GETFL,0)|O_NONBLOCK);
    }

    mLstReqTm = TSYS::curTime();

    run_st = true;

    TTransportOut::start();
}

void TSocketOut::stop( )
{
    ResAlloc res(wres, true);

    if(!run_st) return;

    //Status clear
    trIn = trOut = 0;

    //Close connection
    if(sock_fd >= 0)
    {
	shutdown(sock_fd, SHUT_RDWR);
	close(sock_fd);
    }
    run_st = false;

    TTransportOut::stop();
}

int TSocketOut::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time, bool noRes )
{
    string err(_("Unknown error"));
    ssize_t kz = 0;
    struct timeval tv;
    fd_set rw_fd;
    int reqTry = 0;
    bool writeReq = false;

    if(!noRes) ResAlloc resN(nodeRes(), true);
    ResAlloc res(wres, true);

    int prevTmOut = 0;
    if(time) { prevTmOut = tmCon(); setTmCon(time); }

    if(!run_st) throw TError(nodePath().c_str(),_("Transport is not started!"));

repeate:
    if(reqTry++ >= 2) { mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(),_("Request error: %s"),err.c_str()); }
    //Write request
    writeReq = false;
    if(obuf != NULL && len_ob > 0)
    {
	if(!time) time = mTmCon;

	// Input buffer clear
	char tbuf[100];
	while(read(sock_fd,tbuf,sizeof(tbuf)) > 0) ;
	// Write request
	if(mTmRep && (TSYS::curTime()-mLstReqTm) < (1000*mTmRep))
	    TSYS::sysSleep(1e-6*((1e3*mTmRep)-(TSYS::curTime()-mLstReqTm)));
	for(int wOff = 0; wOff != len_ob; wOff += kz)
	{
	    kz = write(sock_fd, obuf+wOff, len_ob-wOff);
	    if(kz <= 0)
	    {
		if(errno == EAGAIN)
		{
		    tv.tv_sec  = (time/2)/1000; tv.tv_usec = 1000*((time/2)%1000);
		    FD_ZERO(&rw_fd); FD_SET(sock_fd, &rw_fd);
		    kz = select(sock_fd+1, NULL, &rw_fd, NULL, &tv);
		    if(kz > 0 && FD_ISSET(sock_fd,&rw_fd)) { kz = 0; continue; }
		}
		err = strerror(errno);
		res.release();
		stop(); start();
		res.request(true);
		goto repeate;
	    }
	}

	writeReq = true;
    }
    else time = mTmNext;
    if(!time) time = 5000;

    trOut += kz;

    //Read reply
    int i_b = 0;
    if(ibuf != NULL && len_ib > 0)
    {
	tv.tv_sec  = time/1000; tv.tv_usec = 1000*(time%1000);
	FD_ZERO(&rw_fd); FD_SET(sock_fd, &rw_fd);
	kz = select(sock_fd+1, &rw_fd, NULL, NULL, &tv);
	if(kz == 0)	{ res.release(); if(writeReq) stop(); mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(),_("Timeouted!")); }
	else if(kz < 0)	{ res.release(); stop(); mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(),_("Socket error!")); }
	else if(FD_ISSET(sock_fd, &rw_fd))
	{
	    i_b = read(sock_fd, ibuf, len_ib);
	    if(i_b <= 0 && obuf)
	    {
		err = strerror(errno);
		res.release();
		stop(); start();
		res.request(true);
		goto repeate;
	    }
	    trIn += vmax(0,i_b);
	}
    }

    if(prevTmOut) setTmCon(prevTmOut);

    if(mTmRep)	mLstReqTm = TSYS::curTime();

    return vmax(0, i_b);
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
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"conn:next[:rep]\". Where:\n"
	    "    conn - maximum time for connection respond wait, in seconds;\n"
	    "    next - maximum time for continue respond wait, in seconds;\n"
	    "    rep  - minimum repeate timeout, in seconds."));
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/TMS")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

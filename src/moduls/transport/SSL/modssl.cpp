
//OpenSCADA system module Transport.SSL file: modssl.cpp
/***************************************************************************
 *   Copyright (C) 2008-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/rand.h>

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>
#include "modssl.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"SSL"
#define MOD_NAME	_("SSL")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"1.8.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides transport based on the secure sockets' layer.\
 OpenSSL is used and SSLv3, TLSv1, TLSv1.1, TLSv1.2, DTLSv1 are supported.")
#define LICENSE		"GPL2"
//************************************************

MSSL::TTransSock *MSSL::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt tr_SSL_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *tr_SSL_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new MSSL::TTransSock(source);
	return NULL;
    }
}

using namespace MSSL;

//************************************************
//* TTransSock					 *
//************************************************
TTransSock::TTransSock( string name ) : TTypeTransport(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    //CRYPTO reentrant init
    bufRes = (pthread_mutex_t*)malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
    for(int i = 0; i < CRYPTO_num_locks(); i++) pthread_mutex_init(&bufRes[i], NULL);
    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);
    CRYPTO_set_dynlock_create_callback(dyn_create_function);
    CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
    CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);

    //SSL init
    SSL_library_init();
    SSL_load_error_strings();
    RAND_load_file("/dev/urandom", 1024);
}

TTransSock::~TTransSock( )
{
    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);
    for(int i = 0; i < CRYPTO_num_locks(); i++) pthread_mutex_destroy(&bufRes[i]);
    free(bufRes);
}

void TTransSock::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	owner().inEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
	owner().outEl().fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
    }
}

unsigned long TTransSock::id_function( )	{ return (unsigned long)pthread_self(); }

void TTransSock::locking_function( int mode, int n, const char * file, int line )
{
    if(mode&CRYPTO_LOCK) pthread_mutex_lock(&mod->bufRes[n]);
    else		 pthread_mutex_unlock(&mod->bufRes[n]);
}

struct CRYPTO_dynlock_value *TTransSock::dyn_create_function( const char *file, int line )
{
    struct CRYPTO_dynlock_value *value;
    value = (struct CRYPTO_dynlock_value*)malloc(sizeof(struct CRYPTO_dynlock_value));
    if(value)	pthread_mutex_init(&value->mutex, NULL);

    return value;
}

void TTransSock::dyn_lock_function( int mode, struct CRYPTO_dynlock_value *l, const char *file, int line )
{
    if(mode&CRYPTO_LOCK) pthread_mutex_lock(&l->mutex);
    else		 pthread_mutex_unlock(&l->mutex);
}

void TTransSock::dyn_destroy_function( struct CRYPTO_dynlock_value *l, const char *file, int line )
{
    pthread_mutex_destroy(&l->mutex);
    free(l);
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
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) : TTransportIn(name,idb,el), sockRes(true), ctx(NULL),
    mMaxFork(20), mMaxForkPerHost(0), mBufLen(5), mKeepAliveReqs(0), mKeepAliveTm(60), mTaskPrior(0), clFree(true)
{
    setAddr("localhost:10042");
}

TSocketIn::~TSocketIn( )
{

}

string TSocketIn::getStatus( )
{
    string rez = TTransportIn::getStatus();

    if(!startStat() && !stErr.empty())	rez += _("Start error: ") + stErr;
    else if(startStat()) {
	rez += TSYS::strMess(_("Connections %d, opened %d. Traffic in %s, out %s. Closed connections by limit %d. "),
	    connNumb, clId.size(), TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str(), clsConnByLim);
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
	vl = prmNd.attr("MaxClients");	if(!vl.empty()) setMaxFork(s2i(vl));
	vl = prmNd.attr("MaxClientsPerHost");	if(!vl.empty()) setMaxForkPerHost(s2i(vl));
	vl = prmNd.attr("BufLen");	if(!vl.empty()) setBufLen(s2i(vl));
	vl = prmNd.attr("KeepAliveReqs");	if(!vl.empty()) setKeepAliveReqs(s2i(vl));
	vl = prmNd.attr("KeepAliveTm");	if(!vl.empty()) setKeepAliveTm(s2i(vl));
	vl = prmNd.attr("TaskPrior");	if(!vl.empty()) setTaskPrior(s2i(vl));
	if( prmNd.childGet("CertKey",0,true) ) mCertKey = prmNd.childGet("CertKey")->text();
	mKeyPass = prmNd.attr("PKeyPass");
    } catch(...) { }
}

void TSocketIn::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("MaxClients", i2s(maxFork()));
    prmNd.setAttr("MaxClientsPerHost", i2s(maxForkPerHost()));
    prmNd.setAttr("BufLen", i2s(bufLen()));
    prmNd.setAttr("KeepAliveReqs", i2s(keepAliveReqs()));
    prmNd.setAttr("KeepAliveTm", i2s(keepAliveTm()));
    prmNd.setAttr("TaskPrior", i2s(taskPrior()));
    if(prmNd.childGet("CertKey",0,true)) prmNd.childGet("CertKey")->setText(mCertKey);
    else prmNd.childAdd("CertKey")->setText(mCertKey);
    prmNd.setAttr("PKeyPass",mKeyPass);
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportIn::save_();
}

void TSocketIn::start( )
{
    if(runSt) return;

    //Status clear
    stErr = "";
    trIn = trOut = prcTm = prcTmMax = 0;
    connNumb = clsConnByLim = 0;

    //Wait connection main task start
    SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);

    TTransportIn::start();

    if(logLen()) pushLogMess(_("Started"));
}

void TSocketIn::stop( )
{
    if(!runSt)	return;

    //Status clear
    stErr = "";
    trIn = trOut = 0;
    connNumb = clsConnByLim = 0;

    //Wait connection main task stop
    SYS->taskDestroy(nodePath('.',true), &endrun);

    TTransportIn::stop();

    if(logLen()) pushLogMess(_("Stopped"));
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
    SSL *ssl;
    BIO	*bio = NULL, *abio = NULL;
    char err[255];
    TSocketIn &s = *(TSocketIn*)sock_in;
    AutoHD<TProtocolIn> prot_in;
    string cfile;

    //Client's sockets pthreads attrs init
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setdetachstate(&pthr_attr, PTHREAD_CREATE_DETACHED);

    //SSL context init
    string ssl_host = TSYS::strParse(s.addr(), 0, ":");
    string ssl_port = TSYS::strParse(s.addr(), 1, ":");
    string ssl_method = TSYS::strParse(s.addr(), 2, ":");

    // Set SSL method
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
    const SSL_METHOD *meth;
#else
    SSL_METHOD *meth;
#endif

#ifndef OPENSSL_NO_SSL3
    if(ssl_method == "SSLv3")		meth = SSLv3_server_method();
    else
#endif
	if(ssl_method == "TLSv1")	meth = TLSv1_server_method();
    else
#if OPENSSL_VERSION_NUMBER >= 0x1000114fL
	 if(ssl_method == "TLSv1_1")	meth = TLSv1_1_server_method();
    else if(ssl_method == "TLSv1_2")	meth = TLSv1_2_server_method();
    else if(ssl_method == "DTLSv1")	meth = DTLSv1_server_method();
    else
#endif
	 meth = SSLv23_server_method();

    try {
	s.ctx = SSL_CTX_new(meth);
	if(s.ctx == NULL) {
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(s.nodePath().c_str(), "SSL_CTX_new: %s", err);
	}

	//Write certificate and private key to temorary file
#if defined(__ANDROID__)
	cfile = MOD_TYPE "_" MOD_ID "_" + s.id() + "_" + i2s(rand()) + ".tmp";
#else
	cfile = tmpnam(err);
#endif
	int icfile = open(cfile.c_str(), O_EXCL|O_CREAT|O_WRONLY, 0600);
	if(icfile < 0) throw TError(s.nodePath().c_str(), _("Open temporary file '%s' error: '%s'"), cfile.c_str(), strerror(errno));
	bool fOK = (write(icfile,s.certKey().data(),s.certKey().size()) == (int)s.certKey().size());
	close(icfile);
	if(!fOK) throw TError(s.nodePath().c_str(), _("Error write to file '%s'."), cfile.c_str());

	// Set private key password
	SSL_CTX_set_default_passwd_cb_userdata(s.ctx, (char*)s.pKeyPass().c_str());
	// Load certificate
	if(SSL_CTX_use_certificate_chain_file(s.ctx,cfile.c_str()) != 1) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(s.nodePath().c_str(), "SSL_CTX_use_certificate_chain_file: %s", err);
	}
	// Load private key
	if(SSL_CTX_use_PrivateKey_file(s.ctx,cfile.c_str(),SSL_FILETYPE_PEM) != 1) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(s.nodePath().c_str(), "SSL_CTX_use_PrivateKey_file: %s", err);
	}

	//Remove temporary certificate file
	remove(cfile.c_str()); cfile = "";

	//Create BIO object
	if((bio=BIO_new_ssl(s.ctx,0)) == NULL) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(s.nodePath().c_str(), "BIO_new_ssl: %s", err);
	}
	BIO_get_ssl(bio, &ssl);
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

	abio = BIO_new_accept((char*)(ssl_host+":"+ssl_port).c_str());

	//BIO_ctrl(abio,BIO_C_SET_ACCEPT,1,(void*)"a");
	//BIO_set_nbio(abio,1);
	BIO_set_accept_bios(abio, bio);
	BIO_set_bind_mode(abio, BIO_BIND_REUSEADDR);

	//Set up to accept BIO
	if(BIO_do_accept(abio) <= 0) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(s.nodePath().c_str(), "BIO_do_accept: %s", err);
	}

	s.runSt		= true;
	s.endrun	= false;
	s.endrunCl	= false;

	//Select mode
	struct  timeval tv;
	fd_set  rd_fd;
	while(!s.endrun) {
	    tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	    FD_ZERO(&rd_fd); FD_SET(BIO_get_fd(abio,NULL), &rd_fd);

	    int kz = select(BIO_get_fd(abio,NULL)+1,&rd_fd,NULL,NULL,&tv);
	    if(kz < 0 && errno != EINTR) {
		mess_err(s.nodePath().c_str(), _("Close input transport by error: %s"), strerror(errno));
		break;
	    }
	    if(kz <= 0 || !FD_ISSET(BIO_get_fd(abio,NULL),&rd_fd)) continue;

	    if(BIO_do_accept(abio) <= 0) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		mess_err(s.nodePath().c_str(), "BIO_do_accept: %s", err);
		continue;
	    }

	    BIO *cbio = BIO_pop(abio);

	    struct sockaddr_in	name_cl;
	    socklen_t		name_cl_len = sizeof(name_cl);
	    getpeername(BIO_get_fd(cbio,NULL), (sockaddr*)&name_cl, &name_cl_len);
	    string sender = inet_ntoa(name_cl.sin_addr);

	    if(s.clId.size() >= s.maxFork() || (s.maxForkPerHost() && s.forksPerHost(sender) >= s.maxForkPerHost())) {
		s.clsConnByLim++;
		//BIO_reset(cbio);
		//close(BIO_get_fd(cbio,NULL)); BIO_free(cbio);
		BIO_free_all(cbio);
	    }
	    //Make client's socket thread
	    else {
		SSockIn *sin = new SSockIn(&s, cbio, sender);
		try {
		    SYS->taskCreate(s.nodePath('.',true)+"."+i2s(BIO_get_fd(cbio,NULL)), s.taskPrior(), ClTask, sin, 5, &pthr_attr);
		    s.connNumb++;
		} catch(TError &err) {
		    delete sin;
		    //close(BIO_get_fd(cbio,NULL)); BIO_free(cbio);
		    BIO_free_all(cbio);
		    mess_err(err.cat.c_str(), err.mess.c_str());
		    mess_err(s.nodePath().c_str(), _("Error creation of the thread!"));
		}
	    }
	}
    } catch(TError &err) { s.stErr = err.mess; mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //Client tasks stop command
    s.endrunCl = true;
    TSYS::eventWait(s.clFree, true, string(MOD_ID)+": "+s.id()+_(" client tasks is stopping...."));

    //Free context
    if(abio)	BIO_reset(abio);
    if(bio)	BIO_free_all(bio);
    if(s.ctx)	{ SSL_CTX_free(s.ctx); s.ctx = NULL; }
    if(!cfile.empty()) remove(cfile.c_str());

    pthread_attr_destroy(&pthr_attr);

    s.runSt = false;

    return NULL;
}

void *TSocketIn::ClTask( void *s_inf )
{
    SSockIn	&s = *(SSockIn *)s_inf;
    s.pid = pthread_self();

    int		rez;
    char	err[255];
    char	buf[s.s->bufLen()*1024];
    string	req, answ;
    AutoHD<TProtocolIn> prot_in;
    SSL		*ssl;

    if(mess_lev() == TMess::Debug)
	mess_debug(s.s->nodePath().c_str(),_("Socket has been connected by '%s'!"), s.sender.c_str());

    if(BIO_do_handshake(s.bio) <= 0) {
	if(BIO_should_retry(s.bio))
	    while(BIO_should_retry(s.bio) && !s.s->endrunCl)
	    { BIO_do_handshake(s.bio); TSYS::sysSleep(STD_WAIT_DELAY*1e-3); }
	else {
	    if(ERR_peek_last_error()) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		mess_err(s.s->nodePath().c_str(), "BIO_should_retry: %s", err);
	    }
	    BIO_flush(s.bio);
	    //BIO_reset(s.bio);
	    //close(s.sock); BIO_free(s.bio);
	    BIO_free_all(s.bio);
	    return NULL;
	}
    }

    s.sock = BIO_get_fd(s.bio, NULL);

    s.s->clientReg(&s);

    BIO_get_ssl(s.bio, &ssl);

    //Select mode
    struct  timeval tv;
    fd_set  rd_fd;
    int cnt = 0;		//Requests counter
    int tm = time(NULL);	//Last connection time

    try {
	do {
	    int kz = 1;
	    if(!SSL_pending(ssl)) {
		tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
		bool poolPrt = s.s->prtInit(prot_in, s.sock, s.sender, true) && prot_in.at().waitReqTm();
		if(poolPrt) { tv.tv_sec = prot_in.at().waitReqTm()/1000; tv.tv_usec = (prot_in.at().waitReqTm()%1000)*1000; }
		FD_ZERO(&rd_fd); FD_SET(s.sock, &rd_fd);

		kz = select(s.sock+1, &rd_fd, NULL, NULL, &tv);
		if((kz == 0 && !poolPrt) || (kz == -1 && errno == EINTR) || (kz > 0 && !FD_ISSET(s.sock,&rd_fd))) continue;
		if(kz < 0) {
		    if(mess_lev() == TMess::Debug)
			mess_debug(s.s->nodePath().c_str(), _("Socket has been terminated by error: %s"), strerror(errno));
		    break;
		}
	    }

	    rez = 0;
	    if(kz && (rez=BIO_read(s.bio,buf,sizeof(buf))) <= 0) break;
	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("The message is received with the size '%d'."), rez);
	    req.assign(buf, rez);
	    if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Received from '%s'\n"),s.sock,s.sender.c_str()) + req);

	    s.s->dataRes().lock();
	    s.s->trIn += rez; s.trIn += rez;
	    s.s->dataRes().unlock();

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
		    mess_debug(s.s->nodePath().c_str(), _("The message is replied with the size '%d'."), answ.size());
		do { rez = BIO_write(s.bio, answ.data(), answ.size()); }
		while(rez < 0 && SSL_get_error(ssl,rez) == SSL_ERROR_WANT_WRITE);

		s.s->dataRes().lock();
		s.s->trOut += vmax(0, rez); s.trOut += vmax(0, rez);
		s.s->dataRes().unlock();
		if(rez > 0 && s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Transmitted to '%s'\n"),s.sock,s.sender.c_str()) + string(answ.data(),rez));
		answ = "";
	    }
	    cnt++;
	    s.tmReq = tm = time(NULL);
	} while(!s.s->endrunCl &&
		(!s.s->keepAliveReqs() || cnt < s.s->keepAliveReqs()) &&
		(!s.s->keepAliveTm() || (time(NULL)-tm) < s.s->keepAliveTm()));

	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Has been disconnected by '%s'!"), s.sender.c_str());
    } catch(TError &err) {
	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Socket has been terminated by execution: %s"), err.mess.c_str());
	if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Has been terminated by execution: %s"),s.sock,err.mess.c_str()));
    }

    BIO_flush(s.bio);
    //BIO_reset(s.bio);
    //close(s.sock); BIO_free(s.bio);
    BIO_free_all(s.bio);

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
	string n_pr = mod->modId()+"_"+id()+"_"+i2s(sock);
	if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, this, sender+"\n"+i2s(sock));
	prot_in = proto.at().at(n_pr);
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("New input protocol's object '%s' created!"), n_pr.c_str());
    } catch(TError &err) {
	if(!noex) throw;
	return false;
    }

    return !prot_in.freeStat();
}

void TSocketIn::messPut( int sock, string &request, string &answer, string sender, AutoHD<TProtocolIn> &prot_in )
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

    for(unsigned iId = 0; iId < clId.size(); iId++)
	if(clId[iId] == so) return;

    clId.push_back(so);
    clS[so->sender]++;
    clFree = false;

    if(logLen()) pushLogMess(TSYS::strMess(_("A new client %d from '%s' connected"),so->sock,so->sender.c_str()));
}

void TSocketIn::clientUnreg( SSockIn *so )
{
    MtxAlloc res(sockRes, true);

    for(unsigned iId = 0; iId < clId.size(); iId++)
	if(clId[iId] == so) {
	    if(logLen()) pushLogMess(TSYS::strMess(_("Client %d from '%s' disconnected"),so->sock,so->sender.c_str()));

	    clS[so->sender]--;
	    clId.erase(clId.begin()+iId);
	    delete so;
	    break;
	}

    clFree = clId.empty();
}

void TSocketIn::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportIn::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/st",_("State")) && clId.size())
	    ctrMkNode("list", opt, -1, "/prm/st/conns", _("Active connections"), R_R_R_, "root", STR_ID);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID,1,"help",
	    _("SSL input transport has address format:\n"
	    "  {addr}:{port}[:{mode}] - where:\n"
	    "    addr - address for SSL to be opened, '*' address opens for all interfaces;\n"
	    "    port - network port (/etc/services);\n"
	    "    mode - SSL mode and version (SSLv3, SSLv23, TLSv1, TLSv1_1, TLSv1_2, DTLSv1)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID);
	ctrMkNode("fld",opt,-1,"/prm/cfg/certKey",_("Certificates and private key"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,4,
	    "tp","str","cols","90","rows","7","help",_("SSL PAM certificates chain and private key."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/pkey_pass",_("Private key password"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/cl_n",_("Clients maximum"),RWRWR_,"root",STR_ID,1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/prm/cfg/cl_n_pHost",_("Clients maximum, per host"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Set to zero for disable that limit."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/bf_ln",_("Input buffer (kbyte)"),RWRWR_,"root",STR_ID,1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/prm/cfg/keepAliveReqs",_("Keep alive requests"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Close the connection after specified requests.\nZero value for disable (not close ever)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/keepAliveTm",_("Keep alive timeout, seconds"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Close the connection after no requests at specified timeout.\nZero value for disable (not close ever)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/taskPrior",_("Priority"),RWRWR_,"root",STR_ID,2, "tp","dec", "help",TMess::labTaskPrior());
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/conns" && ctrChkNode(opt)) {
	MtxAlloc res(sockRes, true);
	for(vector<SSockIn*>::iterator iId = clId.begin(); iId != clId.end(); ++iId) {
	    string mess = TSYS::strMess(_("%s %d(%s): last %s; traffic in %s, out %s; "),
		atm2s((*iId)->tmCreate,"%Y-%m-%dT%H:%M:%S").c_str(),(*iId)->sock,(*iId)->sender.c_str(),
		atm2s((*iId)->tmReq,"%Y-%m-%dT%H:%M:%S").c_str(),
		TSYS::cpct2str((*iId)->trIn).c_str(),TSYS::cpct2str((*iId)->trOut).c_str());
	    if(mess_lev() == TMess::Debug)
		mess += TSYS::strMess(_("processing time %s[%s]; "),
		tm2s(1e-6*(*iId)->prcTm).c_str(), tm2s(1e-6*(*iId)->prcTmMax).c_str());
	    opt->childAdd("el")->setText(mess);
	}
    }
    else if(a_path == "/prm/cfg/certKey") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(certKey());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setCertKey(opt->text());
    }
    else if(a_path == "/prm/cfg/pkey_pass") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(string(pKeyPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setPKeyPass(opt->text());
    }
    else if(a_path == "/prm/cfg/cl_n") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxFork()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxFork(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/cl_n_pHost") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxForkPerHost()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxForkPerHost(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/bf_ln") {
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
TSocketOut::TSocketOut( string name, const string &idb, TElem *el ) : TTransportOut(name, idb, el)
{
    setAddr("localhost:10042");
    setTimings("5:1");
}

TSocketOut::~TSocketOut( )	{ }

void TSocketOut::setTimings( const string &vl )
{
    mTmCon = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,0,":"))*1e3)));
    mTmNext = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,1,":"))*1e3)));
    mTimings = TSYS::strMess("%g:%g", (1e-3*mTmCon), (1e-3*mTmNext));
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
	if(prmNd.childGet("CertKey",0,true)) setCertKey(prmNd.childGet("CertKey")->text());
	vl = prmNd.attr("PKeyPass");	if(!vl.empty()) setPKeyPass(vl);
	vl = prmNd.attr("TMS");		if(!vl.empty()) setTimings(vl);
    } catch(...) { }
}

void TSocketOut::save_( )
{
    XMLNode prmNd("prms");
    if(prmNd.childGet("CertKey",0,true)) prmNd.childGet("CertKey")->setText(certKey());
    else prmNd.childAdd("CertKey")->setText(certKey());
    prmNd.setAttr("PKeyPass", pKeyPass());
    prmNd.setAttr("TMS", timings());
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TTransportOut::save_();
}

void TSocketOut::start( int tmCon )
{
    int sock_fd = -1;

    string	cfile;
    char	err[255];

    MtxAlloc res(reqRes(), true);
    if(runSt) return;

    ctx = NULL;
    ssl = NULL;
    conn = NULL;

    //Status clear
    trIn = trOut = 0;

    //SSL context init
    string ssl_host = TSYS::strParse(addr(), 0, ":");
    string ssl_port = TSYS::strParse(addr(), 1, ":");
    string ssl_method = TSYS::strParse(addr(), 2, ":");
    if(!tmCon) tmCon = mTmCon;

    //Set SSL method
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
    const SSL_METHOD *meth;
#else
    SSL_METHOD *meth;
#endif

#ifndef OPENSSL_NO_SSL3
    if(ssl_method == "SSLv3")		meth = SSLv3_client_method();
    else
#endif
	if(ssl_method == "TLSv1")	meth = TLSv1_client_method();
    else
#if OPENSSL_VERSION_NUMBER >= 0x1000114fL
	 if(ssl_method == "TLSv1_1")	meth = TLSv1_1_client_method();
    else if(ssl_method == "TLSv1_2")	meth = TLSv1_2_client_method();
    else if(ssl_method == "DTLSv1")	meth = DTLSv1_client_method();
    else
#endif
	 meth = SSLv23_client_method();

    try {
	//Connect to remote host try
	struct sockaddr_in	nameIn;
	memset(&nameIn, 0, sizeof(nameIn));
	nameIn.sin_family = AF_INET;
	char hBuf[STR_BUF_LEN], sBuf[256];
	struct hostent hostbuf, *hp;
	struct servent servbuf, *sp;

	if(ssl_host.size()) {
	    int herr;
	    gethostbyname_r(ssl_host.c_str(), &hostbuf, hBuf, sizeof(hBuf), &hp, &herr);
	    if(!hp) switch(herr) {
		case HOST_NOT_FOUND:
		    throw TError(nodePath().c_str(), _("Host '%s' not found!"), ssl_host.c_str());
		case NO_ADDRESS:
		    throw TError(nodePath().c_str(), _("The requested name '%s' does not have an IP address!"), ssl_host.c_str());
		case NO_RECOVERY:
		    throw TError(nodePath().c_str(), _("A non-recoverable name server error occurred while for '%s'!"), ssl_host.c_str());
		case TRY_AGAIN:
		    throw TError(nodePath().c_str(), _("A temporary error occurred on an authoritative name server for '%s'!"), ssl_host.c_str());
		default:
		    throw TError(nodePath().c_str(), _("Unknown error code from gethostbyname_r for '%s'!"), ssl_host.c_str());
	    }
	    nameIn.sin_addr.s_addr = *((int*)(hp->h_addr_list[0]));	//!!!! Append all IP list processing
	}
	else nameIn.sin_addr.s_addr = INADDR_ANY;
	// Get system port for "oscada" /etc/services
#if defined(__ANDROID__)
	struct servent *sptr = getservbyname(ssl_port.c_str(), "tcp");
	if(sptr)				nameIn.sin_port = sptr->s_port;
#else
	if(getservbyname_r(ssl_port.c_str(),"tcp",&servbuf,sBuf,sizeof(sBuf),&sp) == 0 && sp)
	    nameIn.sin_port = sp->s_port;
#endif
	else if(htons(s2i(ssl_port)) > 0)	nameIn.sin_port = htons(s2i(ssl_port));
	else nameIn.sin_port = 10041;

	if((sock_fd=socket(PF_INET,SOCK_STREAM,0)) == -1)
	    throw TError(nodePath().c_str(), _("Error creation TCP socket: %s!"), strerror(errno));
	int vl = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));

	//Connect to socket
	int flags = fcntl(sock_fd, F_GETFL, 0);
	fcntl(sock_fd, F_SETFL, flags|O_NONBLOCK);
	int res = connect(sock_fd, (sockaddr*)&nameIn, sizeof(nameIn));
	if(res == -1 && errno == EINPROGRESS) {
	    struct timeval tv;
	    socklen_t slen = sizeof(res);
	    fd_set fdset;
	    tv.tv_sec = tmCon/1000; tv.tv_usec = 1000*(tmCon%1000);
	    FD_ZERO(&fdset); FD_SET(sock_fd, &fdset);
	    if((res=select(sock_fd+1,NULL,&fdset,NULL,&tv)) > 0 && !getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,&res,&slen) && !res) res = 0;
	    else res = -1;
	}
	if(res)	throw TError(nodePath().c_str(), _("Connect to Internet socket error: %s!"), strerror(errno));

	//SSL processing
	if((ctx=SSL_CTX_new(meth)) == NULL) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(nodePath().c_str(), "SSL_CTX_new: %s", err);
	}

	//Certificates, private key and it password loading
	if(!sTrm(certKey()).empty()) {
	    // Write certificate and private key to temorary file
#if defined(__ANDROID__)
	    cfile = MOD_TYPE "_" MOD_ID "_" + id() + "_" + i2s(rand()) + ".tmp";
#else
	    cfile = tmpnam(err);
#endif
	    int icfile = open(cfile.c_str(), O_EXCL|O_CREAT|O_WRONLY, 0600);
	    if(icfile < 0) throw TError(nodePath().c_str(), _("Open temporary file '%s' error: '%s'"), cfile.c_str(), strerror(errno));
	    bool fOK = (write(icfile,certKey().data(),certKey().size()) == (int)certKey().size());
	    close(icfile);
	    if(!fOK) throw TError(nodePath().c_str(), _("Error write to file '%s'."), cfile.c_str());

	    // Set private key password
	    SSL_CTX_set_default_passwd_cb_userdata(ctx, (char*)pKeyPass().c_str());
	    // Load certificate
	    if(SSL_CTX_use_certificate_chain_file(ctx,cfile.c_str()) != 1) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		throw TError(nodePath().c_str(), _("SSL_CTX_use_certificate_chain_file: %s"), err);
	    }
	    // Load private key
	    if(SSL_CTX_use_PrivateKey_file(ctx,cfile.c_str(),SSL_FILETYPE_PEM) != 1) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		throw TError(nodePath().c_str(), _("SSL_CTX_use_PrivateKey_file: %s"), err);
	    }

	    // Remove temporary certificate file
	    remove(cfile.c_str()); cfile = "";
	}

	if((ssl=SSL_new(ctx)) == NULL) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(nodePath().c_str(), "SSL_new: %s", err);
	}

	SSL_set_connect_state(ssl);
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	SSL_set_read_ahead(ssl, 1);

	if(SSL_set_fd(ssl,sock_fd) != 1) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(nodePath().c_str(), "SSL_set_fd: %s", err);
	}

	fcntl(sock_fd, F_SETFL, flags);	//Clear nonblock
	if(SSL_connect(ssl) != 1) {
	    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	    throw TError(nodePath().c_str(), "SSL_connect: %s", err);
	}

	conn = BIO_new(BIO_f_ssl());
	BIO_set_ssl(conn, ssl, BIO_NOCLOSE);
	BIO_set_nbio(conn, 1);

	fcntl(sock_fd, F_SETFL, flags|O_NONBLOCK);
    } catch(TError &err) {
	if(conn) BIO_reset(conn);
	if(sock_fd >= 0) close(sock_fd);
	if(conn)BIO_free_all(conn);	//BIO_free(conn);
	if(ssl)	SSL_free(ssl);
	if(ctx)	SSL_CTX_free(ctx);
	if(!cfile.empty()) remove(cfile.c_str());
	throw;
    }

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

    //SSL deinit
    BIO_flush(conn);
    BIO_reset(conn);
    close(BIO_get_fd(conn,NULL));
    //BIO_free(conn);
    BIO_free_all(conn);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    ctx = NULL;
    ssl = NULL;
    conn = NULL;

    runSt = false;

    TTransportOut::stop();

    if(logLen()) pushLogMess(_("Stopped"));
}

int TSocketOut::messIO( const char *oBuf, int oLen, char *iBuf, int iLen, int time )
{
    string err = _("Unknown error");
    int	 ret = 0, reqTry = 0;;
    char err_[255];
    bool noReq = (time < 0),
	 writeReq = false;
    time = abs(time);

    MtxAlloc res(reqRes(), true);

    if(!runSt) throw TError(nodePath().c_str(), _("Transport is not started!"));

repeate:
    if(reqTry++ >= 2)	throw TError(nodePath().c_str(), _("Request error: %s"), err.c_str());
    //Write request
    if(oBuf != NULL && oLen > 0) {
	if(!time) time = mTmCon;
	// Input buffer clear
	while(!noReq && BIO_read(conn,err_,sizeof(err_)) > 0) ;
	// Write request
	do { ret = BIO_write(conn, oBuf, oLen); } while(ret < 0 && SSL_get_error(ssl,ret) == SSL_ERROR_WANT_WRITE);
	if(ret <= 0) {
	    err = TSYS::strMess("%s (%d)", strerror(errno), errno);
	    stop();
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Write error: %s"), err.c_str());
	    if(logLen()) pushLogMess(TSYS::strMess(_("Transmitting error: %s"), err.c_str()));
	    if(noReq) throw TError(nodePath().c_str(), _("Write error: %s"), err.c_str());
	    start();
	    goto repeate;
	}

	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Wrote %s."), TSYS::cpct2str(oLen).c_str());
	if(logLen()) pushLogMess(_("Transmitted to\n") + string(oBuf,ret));

	trOut += ret;
	writeReq = true;
    }
    else if(!noReq) time = mTmNext;
    if(!time) time = 5000;

    //Read reply
    if(iBuf != NULL && iLen > 0) {
	ret = BIO_read(conn, iBuf, iLen);
	if(ret > 0) trIn += ret;
	else if(ret == 0) {
	    err = errno ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No data");
	    stop();
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read error: %s"), err.c_str());
	    if(logLen()) pushLogMess(TSYS::strMess(_("Receiving error: %s"), err.c_str()));
	    if(!writeReq || noReq) throw TError(nodePath().c_str(),_("Read error: %s"), err.c_str());
	    start();
	    goto repeate;
	}
	else if(ret < 0 && SSL_get_error(ssl,ret) != SSL_ERROR_WANT_READ && SSL_get_error(ssl,ret) != SSL_ERROR_WANT_WRITE) {
	    ERR_error_string_n(ERR_peek_last_error(), err_, sizeof(err_));
	    throw TError(nodePath().c_str(), "BIO_read: %s", err_);
	}
	else {
	    //Wait data from the socket
	    int kz = 0;
	    fd_set rd_fd;
	    struct timeval tv;

	    int sock_fd = BIO_get_fd(conn, NULL);
	    tv.tv_sec  = time/1000; tv.tv_usec = 1000*(time%1000);
	    FD_ZERO(&rd_fd); FD_SET(sock_fd, &rd_fd);
	    kz = select(sock_fd+1, &rd_fd, NULL, NULL, &tv);
	    if(kz == 0) {
		if(writeReq && !noReq) stop();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read timeouted."));
		throw TError(nodePath().c_str(),_("Timeouted!"));
	    }
	    else if(kz < 0) {
		err = TSYS::strMess("%s (%d)", strerror(errno), errno);
		stop();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read (select) error: %s"), err.c_str());
		if(logLen()) pushLogMess(TSYS::strMess(_("Receiving (select) error: %s"), err.c_str()));
		throw TError(nodePath().c_str(),_("Read (select) error: %s"), err.c_str());
	    }
	    else if(FD_ISSET(sock_fd,&rd_fd)) {
		ret = BIO_read(conn, iBuf, iLen);
		if(ret == -1)
		    while((ret=BIO_read(conn,iBuf,iLen)) == -1) sched_yield();
		if(ret < 0) {
		    err = (ret < 0) ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No data");
		    stop();
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read error: %s"), err.c_str());
		    if(logLen()) pushLogMess(TSYS::strMess(_("Receiving error: %s"),err.c_str()));
		    // * Pass to retry into the request mode and on the successful writing
		    if(!writeReq || noReq) throw TError(nodePath().c_str(),_("Read error: %s"), err.c_str());
		    start();
		    goto repeate;
		}
		trIn += vmax(0, ret);
	    }
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read %s."), TSYS::cpct2str(vmax(0,ret)).c_str());
	    if(ret > 0 && logLen()) pushLogMess(_("Received from\n") + string(iBuf,ret));
	}
    }

    return vmax(0, ret);
}

void TSocketOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportOut::cntrCmdProc(opt);
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1,"help",
	    _("SSL output transport has address format:\n"
	    "  {addr}:{port}[:{mode}] - where:\n"
	    "    addr - remote SSL host address;\n"
	    "    port - network port (/etc/services);\n"
	    "    mode - SSL mode and version (SSLv3, SSLv23, TLSv1, TLSv1_1, TLSv1_2, DTLSv1)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/certKey",_("Certificates and private key"),RWRW__,"root",STR_ID,4,"tp","str","cols","90","rows","7",
	    "help",_("SSL PAM certificates chain and private key."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/pkey_pass",_("Private key password"),RWRW__,"root",STR_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/TMS",_("Timings"),RWRWR_,"root",STR_ID,2,"tp","str","help",
	    _("Connection timings in format: \"[conn]:[next]\". Where:\n"
	    "    conn - maximum time for connection respond wait, in seconds;\n"
	    "    next - maximum time for continue respond wait, in seconds."));
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/certKey") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(certKey());
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setCertKey(opt->text());
    }
    else if(a_path == "/prm/cfg/pkey_pass") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(string(pKeyPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setPKeyPass(opt->text());
    }
    else if(a_path == "/prm/cfg/TMS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

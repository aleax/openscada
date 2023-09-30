
//OpenSCADA module Transport.SSL file: modssl.cpp
/***************************************************************************
 *   Copyright (C) 2008-2023 by Roman Savochenko, <roman@oscada.org>       *
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
#include <netinet/tcp.h>
#include <openssl/rand.h>
#if OPENSSL_VERSION_NUMBER < 0x10100000L
# include <openssl/md5.h>
#endif

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>
#include "modssl.h"

#define DEF_MaxClients		20
#define DEF_MaxClientsPerHost	0
#define DEF_InBufLen		0
#define DEF_MSS			0
#define DEF_KeepAliveReqs	0
#define DEF_KeepAliveTm		60
#define DEF_TaskPrior		0
#define DEF_TMS			"10:1"
#define DEF_PORT		"10045"


//************************************************
//* Modul info!                                  *
#define MOD_ID		"SSL"
#define MOD_NAME	trS("SSL")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"4.4.0"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("Provides transport based on the secure sockets' layer.\
 OpenSSL is used and supported SSL-TLS depending on the library version.")
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
TTransSock::TTransSock( string name ) : TTypeTransport(MOD_ID), ctxIn(NULL), ctxOut(NULL)
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

    //Set SSL method
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
    const SSL_METHOD *methIn, *methOut;
#else
    SSL_METHOD *methIn, *methOut;
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    methIn = TLS_server_method();
    methOut = TLS_client_method();
#else
    methIn = SSLv23_server_method();
    methOut = SSLv23_client_method();
#endif

    ctxIn = SSL_CTX_new(methIn);
    ctxOut = SSL_CTX_new(methOut);
    /*if(ctx == NULL) {
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw TError(nodePath().c_str(), "SSL_CTX_new: %s", err);
    }*/
}

TTransSock::~TTransSock( )
{
    if(ctxIn)	{ SSL_CTX_free(ctxIn); ctxIn = NULL; }
    if(ctxOut)	{ SSL_CTX_free(ctxOut); ctxOut = NULL; }

    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);
    for(int i = 0; i < CRYPTO_num_locks(); i++) pthread_mutex_destroy(&bufRes[i]);
    free(bufRes);
}

void TTransSock::preDisable( int flag )
{
    //!!!! Due to SSL_library_init() is some time crashable at second call, saw on Ubuntu 16.04
    if(SYS->stopSignal() == SIGUSR2) throw err_sys("Hold when overloaded to another project.");
}

void TTransSock::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTypeTransport::cntrCmdProc(opt);
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	if(ctrMkNode("area",opt,0,"/tr/st",_("State"))) {
	    ctrMkNode("fld",opt,-1,"/tr/st/ciphersIn",_("Server ciphers"),R_R_R_,"root",STR_ID,3,"tp","str", "cols","90", "rows","3");
	    ctrMkNode("fld",opt,-1,"/tr/st/ciphersOut",_("Client ciphers"),R_R_R_,"root",STR_ID,3,"tp","str", "cols","90", "rows","3");
	}
#endif
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    if(a_path == "/tr/st/ciphersIn" && ctrChkNode(opt)) {
	STACK_OF(SSL_CIPHER) *sk = SSL_CTX_get_ciphers(ctxIn);
	for(int iL = 0; iL < sk_SSL_CIPHER_num(sk); iL++) {
	    const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, iL);
	    opt->setText(opt->text() + (iL?", ":"") + SSL_CIPHER_get_name(c) + " ("+SSL_CIPHER_get_version(c)+")");
	}
    }
    else if(a_path == "/tr/st/ciphersOut" && ctrChkNode(opt)) {
	STACK_OF(SSL_CIPHER) *sk = SSL_CTX_get_ciphers(ctxOut);
	for(int iL = 0; iL < sk_SSL_CIPHER_num(sk); iL++) {
	    const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, iL);
	    opt->setText(opt->text() + (iL?", ":"") + SSL_CIPHER_get_name(c) + " ("+SSL_CIPHER_get_version(c)+")");
	}
    }
    else
#endif
    TTypeTransport::cntrCmdProc(opt);
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
    TTypeTransport::load_();

    //Load parameters from command line

}

void TTransSock::perSYSCall( unsigned int cnt )
{
    TTypeTransport::perSYSCall(cnt);

    //Checking the certificate file update
    //?!?! and iniciative input protocols checking for the reconnect need
    vector<string> trls;
    inList(trls);
    for(unsigned iTr = 0; !SYS->stopSignal() && iTr < trls.size(); iTr++)
	((AutoHD<TSocketIn>)inAt(trls[iTr])).at().check(cnt);
}

TTransportIn *TTransSock::In( const string &name, const string &idb )	{ return new TSocketIn(name, idb, &owner().inEl()); }

TTransportOut *TTransSock::Out( const string &name, const string &idb )	{ return new TSocketOut(name, idb, &owner().outEl()); }

string TTransSock::outAddrHelp( )
{
    return string(_("SSL output transport has the address format \"{addr}[,{addrN}]:{port}\", where:\n"
	"    addr - address with which the connection is made; there may be as the symbolic representation as well as IPv4 \"127.0.0.1\" or IPv6 \"[::1]\";\n"
	"    port - network port with which the connection is made; indication of the character name of the port according to /etc/services is available."))+
	"\n\n|| " + outTimingsHelp() + "\n\n|| " + outAttemptsHelp();
}

string TTransSock::outTimingsHelp( bool noAdd )
{
    return string(_("Connection timings in the format \"{conn}:{next}\", where:\n"
	"    conn - maximum time of waiting the connection, in seconds;\n"
	"    next - maximum time of waiting for continue the response, in seconds.\n")) + 
	(noAdd?"":_("Can be prioritatile specified in the address field as the second global argument, as such \"localhost:123||5:1\"."));
}

string TTransSock::outAttemptsHelp( bool noAdd )
{
    return string(_("Attempts of the requesting both for this transport and protocol, for full requests.\n")) +
	(noAdd?"":_("Can be prioritatile specified in the address field as the third global argument, as such \"localhost:123||5:1||3\"."));
}

string TTransSock::MD5( const string &file )
{
    int hd = open(file.c_str(), O_RDONLY);
    if(hd < 0) return "";

    string data;
    char buf[prmStrBuf_SZ];
    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) data.append(buf, len);
    if(close(hd) != 0)
	mess_warning(nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), hd, strerror(errno), errno);

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
    EVP_DigestUpdate(mdctx, (unsigned char*)data.data(), data.size());
    unsigned int result_len = EVP_MD_size(EVP_md5());
    unsigned char *result = (unsigned char *)OPENSSL_malloc(result_len);
    EVP_DigestFinal_ex(mdctx, result, &result_len);
    EVP_MD_CTX_free(mdctx);
#else
    unsigned int result_len = MD5_DIGEST_LENGTH;
    unsigned char result[result_len];
    ::MD5((unsigned char*)data.data(), data.size(), result);
#endif

    return string((char*)result, result_len);
}

//************************************************
//* TSocketIn                                    *
//************************************************
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) : TTransportIn(name,idb,el),
    sockRes(true), ssl(NULL), bio(NULL), abio(NULL), sockFd(-1), mMode(M_Ordinal),
    mInBufLen(DEF_InBufLen), mMSS(DEF_MSS), mMaxFork(DEF_MaxClients), mMaxForkPerHost(DEF_MaxClientsPerHost),
    mKeepAliveReqs(DEF_KeepAliveReqs), mKeepAliveTm(DEF_KeepAliveTm), mTaskPrior(DEF_TaskPrior), clFree(true)
{
    setAddr("*:" DEF_PORT);
}

TSocketIn::~TSocketIn( )
{

}

string TSocketIn::getStatus( )
{
    string rez = TTransportIn::getStatus(), tVl;

    if(!startStat() && !stErrMD5.empty())	rez += _("Error connecting: ") + stErrMD5;
    else if(startStat()) {
	rez += TSYS::strMess(_("Connections %d, opened %d, last %s, closed by the limit %d. "),
	    connNumb, (protocols().empty()?associateTrs().size():clId.size()), atm2s(lastConn()).c_str(), clsConnByLim);

	if(protocols().size())
	    rez += TSYS::strMess(_("Traffic in %s, out %s. "),
		TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str());

	if(mess_lev() == TMess::Debug)
	    rez += TSYS::strMess(_("Processing time %s[%s]. "),
		tm2s(1e-6*prcTm).c_str(), tm2s(1e-6*prcTmMax).c_str());

	int bufSz, MSSsz; socklen_t sz = sizeof(socklen_t);
	getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, (void*)&bufSz, &sz);
	getsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, (void*)&MSSsz, &sz);
	rez += TSYS::strMess(_("Size input buffer %s, MSS %s. "), TSYS::cpct2str(bufSz).c_str(), TSYS::cpct2str(MSSsz).c_str());

	char buf[prmStrBuf_SZ], *rezSh = SSL_get_shared_ciphers(ssl, buf, sizeof(buf));
	if(rezSh) rez += TSYS::strMess(_("Shared ciphers: %s. "), rezSh);

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	STACK_OF(SSL_CIPHER) *sk = SSL_get_client_ciphers(ssl);
	if(sk && sk_SSL_CIPHER_num(sk)) {
	    for(int iL = 0; iL < sk_SSL_CIPHER_num(sk); iL++) {
		const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, iL);
		tVl = tVl + (iL?", ":"") + SSL_CIPHER_get_name(c) + " ("+SSL_CIPHER_get_version(c)+")";
	    }
	    rez += TSYS::strMess(_("Client ciphers: %s. "), tVl.c_str());
	}
#endif
    }

    return rez;
}

void TSocketIn::load_( )
{
    setMaxFork((int)prm("MaxClients",DEF_MaxClients));
    setMaxForkPerHost((int)prm("MaxClientsPerHost",DEF_MaxClientsPerHost));
    setInBufLen((int)prm("InBufLen",DEF_InBufLen));
    setMSS((int)prm("MSS",DEF_MSS));
    setKeepAliveReqs((int)prm("KeepAliveReqs",DEF_KeepAliveReqs));
    setKeepAliveTm((int)prm("KeepAliveTm",DEF_KeepAliveTm));
    setTaskPrior(prm("TaskPrior",DEF_TaskPrior));
    setCertKeyFile(prm("CertKeyFile",""));
    setCertKey(prm("CertKey",""));
    setPKeyPass(prm("PKeyPass",""));
    setInitAssocPrms(prm("InitAssocPrms",""));
}

void TSocketIn::save_( )
{
    prm("MaxClients", (int)maxFork(), true);
    prm("MaxClientsPerHost", (int)maxForkPerHost(), true);
    prm("InBufLen", (int)inBufLen(), true);
    prm("MSS", (int)MSS(), true);
    prm("KeepAliveReqs", (int)keepAliveReqs(), true);
    prm("KeepAliveTm", (int)keepAliveTm(), true);
    prm("TaskPrior", taskPrior(), true);
    prm("CertKeyFile", certKeyFile(), true);
    prm("CertKey", certKey(), true);
    prm("PKeyPass", pKeyPass(), true);
    prm("InitAssocPrms", initAssocPrms(), true);

    TTransportIn::save_();
}

void TSocketIn::start( )
{
    if(runSt) return;

    //Status clear
    stErrMD5 = "";
    trIn = trOut = prcTm = prcTmMax = 0;
    connNumb = clsConnByLim = 0;

    //Mode of the initiative connection
    if(mode() == M_Initiative) {
	TSocketOut::connectSSL(TSYS::strParse(addr(),0,"||"), &ssl, &bio,
	    1000*(initAssocPrms().size()?vmax(1,s2i(initAssocPrms())):5), certKey(), pKeyPass(), certKeyFile());
	sockFd = BIO_get_fd(bio, NULL);

	if(addon.size()) BIO_write(bio, addon.data(), addon.size());	//Writing the identification sequence

	SSockIn *sin = new SSockIn(this, bio, addr(), true);
	try {
	    endrunCl = false;
	    SYS->taskCreate(nodePath('.',true)+"."+i2s(sockFd), taskPrior(), ClTask, sin);
	    connNumb++;
	} catch(TError &err) { TSocketOut::disconnectSSL(&ssl, &bio); delete sin; throw; }
    }

    //The generic mode
    else {
	int aOff = 0;

	//SSL context init
	string ssl_host, cfile;
	if(addr()[aOff] != '[') ssl_host = TSYS::strParse(addr(), 0, ":", &aOff);
	else { aOff++; ssl_host = "["+TSYS::strParse(addr(),0,"]:",&aOff)+"]"; }	//Get IPv6
	if(ssl_host.empty()) ssl_host = "*";
	string ssl_port = TSYS::strParse(addr(), 0, ":", &aOff);

	try {
	    char err[255];

	    //Try the external PEM-file of the certificates and the private key
	    if(certKeyFile().size()) stErrMD5 = mod->MD5(cfile=certKeyFile());
	    //Write certificate and private key to temorary file
	    else {
#if defined(__ANDROID__)
		cfile = MOD_TYPE "_" MOD_ID "_" + id() + "_" + i2s(rand()) + ".tmp";
#else
		cfile = tmpnam(err);
#endif
		int icfile = open(cfile.c_str(), O_EXCL|O_CREAT|O_WRONLY, 0600);
		if(icfile < 0) throw TError(nodePath().c_str(), _("Error opening the temporary file '%s': '%s'"), cfile.c_str(), strerror(errno));
		bool fOK = (write(icfile,certKey().data(),certKey().size()) == (int)certKey().size());
		if(close(icfile) != 0)
		    mess_warning(nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), icfile, strerror(errno), errno);
		if(!fOK) throw TError(nodePath().c_str(), _("Error writing the file '%s'."), cfile.c_str());
	    }

	    // Set private key password
	    SSL_CTX_set_default_passwd_cb_userdata(mod->ctxIn, (char*)pKeyPass().c_str());
	    // Load certificate
	    if(SSL_CTX_use_certificate_chain_file(mod->ctxIn,cfile.c_str()) != 1) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		if(certKeyFile().empty() && certKey().empty())
		    throw TError(nodePath().c_str(), _("Specify a certificate file or directly the certificates and private key with password!"), err);
		else throw TError(nodePath().c_str(), "SSL_CTX_use_certificate_chain_file: %s.", err);
	    }
	    // Load private key
	    if(SSL_CTX_use_PrivateKey_file(mod->ctxIn,cfile.c_str(),SSL_FILETYPE_PEM) != 1) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		throw TError(nodePath().c_str(), "SSL_CTX_use_PrivateKey_file: %s", err);
	    }

	    //Remove temporary certificate file
	    if(certKeyFile().empty()) remove(cfile.c_str());
	    cfile = "";

	    //Create BIO object
	    if((bio=BIO_new_ssl(mod->ctxIn,0)) == NULL) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		throw TError(nodePath().c_str(), "BIO_new_ssl: %s", err);
	    }
	    BIO_get_ssl(bio, &ssl);
	    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

	    MtxAlloc aRes(*SYS->commonLock("getaddrinfo"), true);
	    abio = BIO_new_accept((char*)(ssl_host+":"+ssl_port).c_str());

	    //BIO_ctrl(abio,BIO_C_SET_ACCEPT,1,(void*)"a");
	    //BIO_set_nbio(abio,1);
	    BIO_set_accept_bios(abio, bio);
	    BIO_set_bind_mode(abio, BIO_BIND_REUSEADDR);

	    //Set up to accept BIO
	    if(BIO_do_accept(abio) <= 0) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		throw TError(nodePath().c_str(), "BIO_do_accept: %s", err);
	    }
	    aRes.unlock();

	    sockFd = BIO_get_fd(abio, NULL);
	} catch(TError &err) {
	    //Free context
	    if(abio)	{ BIO_reset(abio); abio = NULL; }
	    if(bio)	{ BIO_free_all(bio); bio = NULL; }
	    ssl = NULL;

	    if(!cfile.empty() && certKeyFile().empty()) remove(cfile.c_str());
	    throw;
	}

	SYS->taskCreate(nodePath('.',true), taskPrior(), Task, this);
    }

    int vl = 1; setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
    if(MSS()) { vl = MSS(); setsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, &vl, sizeof(int)); }
    if(inBufLen()) { vl = inBufLen()*1024; setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &vl, sizeof(int)); }

    runSt = true;

    TTransportIn::start();

    if(logLen()) pushLogMess(_("Connected"));
}

void TSocketIn::stop( )
{
    if(!runSt)	return;

    //Status clear
    stErrMD5 = "";
    trIn = trOut = 0;
    connNumb = connTm = clsConnByLim = 0;

    if(mode() == M_Initiative) {
	SYS->taskDestroy(nodePath('.',true)+"."+i2s(sockFd), &endrunCl);
	TSocketOut::disconnectSSL(&ssl, &bio);
    }
    else {
	//Wait connection main task stop
	SYS->taskDestroy(nodePath('.',true), &endrun);

	//Free context
	if(abio){ BIO_reset(abio); abio = NULL; }
	if(bio)	{ BIO_free_all(bio); bio = NULL; }
	ssl = NULL;
    }
    sockFd = -1;

    runSt = false;

    TTransportIn::stop();

    if(logLen()) pushLogMess(_("Disconnected"));
}

void TSocketIn::check( unsigned int cnt )
{
    try {
	//Checking the certificate file change
	string newMD5;
	if(!(cnt%60) && startStat() && certKeyFile().size() && stErrMD5.size()
		&& (newMD5=mod->MD5(certKeyFile())).size() && newMD5 != stErrMD5) {
	    mess_note(nodePath().c_str(), _("The certificate file '%s' was updated, reconnecting the transport..."), certKeyFile().c_str());
	    stop();
	    start();
	}
	//Checking for activity the initiative mode
	else if(mode() == M_Initiative && (toStart() || startStat()) && (!startStat() || time(NULL) > (lastConn()+keepAliveTm()))) {
	    if(mess_lev() == TMess::Debug)
		mess_debug(nodePath().c_str(), _("Reconnect due to lack of input activity to '%s'."), addr().c_str());
	    if(startStat()) stop();
	    start();
	}
    } catch(...) { }
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
	mMode = s2i(TSYS::strParse(co.getS(),2,":",&off));
	addon = (off < (int)co.getS().size()) ? co.getS().substr(off) : "";
    }

    return TTransportIn::cfgChange(co, pc);
}

void *TSocketIn::Task( void *sock_in )
{
    char err[255];
    TSocketIn &s = *(TSocketIn*)sock_in;
    vector< AutoHD<TProtocolIn> > prot_in;

    //Client's sockets pthreads attrs init
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setdetachstate(&pthr_attr, PTHREAD_CREATE_DETACHED);

    try {
	s.runSt		= true;
	s.endrun	= false;
	s.endrunCl	= false;

	//Select mode
	struct  timeval tv;
	fd_set  rd_fd;
	while(!s.endrun) {
	    tv.tv_sec  = 0; tv.tv_usec = prmWait_DL*1000000;
	    FD_ZERO(&rd_fd); FD_SET(BIO_get_fd(s.abio,NULL), &rd_fd);

	    int kz = select(BIO_get_fd(s.abio,NULL)+1,&rd_fd,NULL,NULL,&tv);
	    if(kz < 0 && errno != EINTR)
		throw TError(s.nodePath().c_str(), _("The input transport closed by the error: %s"), strerror(errno));
	    if(kz <= 0 || !FD_ISSET(BIO_get_fd(s.abio,NULL),&rd_fd)) continue;

	    if(BIO_do_accept(s.abio) <= 0) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		mess_err(s.nodePath().c_str(), "BIO_do_accept: %s", err);
		continue;
	    }

	    BIO *cbio = BIO_pop(s.abio);
	    struct sockaddr_storage sockStor;
	    struct sockaddr *sadr = (struct sockaddr*) &sockStor;;
	    socklen_t sadrLen = sizeof(sockStor);
	    getpeername(BIO_get_fd(cbio,NULL), sadr, &sadrLen);
	    string sender;
	    if(sadr->sa_family == AF_INET6) {
		char aBuf[INET6_ADDRSTRLEN];
		getnameinfo(sadr, sadrLen, aBuf, sizeof(aBuf), 0, 0, NI_NUMERICHOST);
		sender = aBuf;
	    } else sender = inet_ntoa(((sockaddr_in*)sadr)->sin_addr);

	    if(s.clId.size() >= s.maxFork() ||
		(s.maxForkPerHost() && s.forksPerHost(sender) >= s.maxForkPerHost()) ||
		(s.protocols().empty() && s.associateTrs(true).size() >= s.maxFork()))
	    {
		s.clsConnByLim++;
		//BIO_reset(cbio);
		//close(BIO_get_fd(cbio,NULL)); BIO_free(cbio);
		BIO_free_all(cbio);
		continue;
	    }

	    //Creating an output transport of representing the client connection
	    if(s.protocols().empty()) {
		MtxAlloc resN(s.associateTrRes, true);

		// Registering
		string outTrId = s.associateTrO((S_NM_SOCKET ":")+i2s(BIO_get_fd(cbio,NULL)), ATrStg_Create);

		// Additional parameterization
		AutoHD<TSocketOut> tr = s.owner().outAt(outTrId);
		if(s.initAssocPrms().size()) {
		    tr.at().setTimings(TSYS::strParse(s.initAssocPrms(),0,"||"));
		    tr.at().setAttempts(vmax(1,s2i(TSYS::strParse(s.initAssocPrms(),1,"||"))));
		}
		tr.at().connAddr = sender;
		tr.at().ssl = s.ssl;
		tr.at().conn = cbio;

		// Finishing
		s.associateTrO(outTrId, ATrStg_Proc);

		s.connNumb++;
		s.connTm = time(NULL);
		continue;
	    }

	    //Making a thread for the client socket
	    SSockIn *sin = new SSockIn(&s, cbio, sender);
	    try {
		SYS->taskCreate(s.nodePath('.',true)+"."+i2s(BIO_get_fd(cbio,NULL)), s.taskPrior(), ClTask, sin, 5, &pthr_attr);
		s.connNumb++;
	    } catch(TError &err) {
		delete sin;
		//close(BIO_get_fd(cbio,NULL)); BIO_free(cbio);
		BIO_free_all(cbio);
		mess_err(err.cat.c_str(), err.mess.c_str());
		mess_err(s.nodePath().c_str(), _("Error creating the thread!"));
	    }
	}
    } catch(TError &err) { s.stErrMD5 = err.mess; mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //Client tasks stop command
    s.endrunCl = true;

    //Finding up already registered to terminate
    MtxAlloc res(s.sockRes, true);
    for(vector<SSockIn*>::iterator iId = s.clId.begin(); iId != s.clId.end(); ++iId)
	if((*iId)->pid) pthread_kill((*iId)->pid, SIGALRM);
    res.unlock();
    TSYS::eventWait(s.clFree, true, string(MOD_ID)+": "+s.id()+_(" stopping client tasks ..."));

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
    char	buf[STR_BUF_LEN];
    string	req, answ;
    vector< AutoHD<TProtocolIn> > prot_in;
    SSL		*ssl;

    if(mess_lev() == TMess::Debug)
	mess_debug(s.s->nodePath().c_str(),_("Socket has been connected by '%s'!"), s.sender.c_str());

    if(BIO_do_handshake(s.bio) <= 0) {
	if(BIO_should_retry(s.bio))
	    while(BIO_should_retry(s.bio) && !s.s->endrunCl)
	    { BIO_do_handshake(s.bio); TSYS::sysSleep(prmWait_DL); }
	else {
	    if(ERR_peek_last_error()) {
		ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		mess_warning(s.s->nodePath().c_str(), "BIO_should_retry: %s", err);
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
    struct timeval tv;
    fd_set rd_fd;
    unsigned cnt = 0;			//Requests counter
    int    tm = s.s->connTm = time(NULL);	//Last connection time
    int    actPrts = 0;

    try {
	do {
	    int kz = 1;
	    if(!SSL_pending(ssl)) {
		tv.tv_sec  = 0; tv.tv_usec = prmWait_DL*1000000;

		unsigned pollPrt = 0;
		if((actPrts=s.s->prtInit(prot_in,s.sock,s.sender)))
		    for(unsigned iP = 0; iP < prot_in.size(); iP++)
			if(!prot_in[iP].freeStat() && (pollPrt=prot_in[iP].at().waitReqTm()))
			    break;
		if(pollPrt) {
		    // Aligning to the grid
		    int64_t curTm = TSYS::curTime(), targTm = (curTm/(1000ll*pollPrt) + 1)*pollPrt*1000ll;
		    tv.tv_sec = (targTm-curTm)/1000000; tv.tv_usec = (targTm-curTm)%1000000;
		    //tv.tv_sec = pollPrt/1000; tv.tv_usec = (pollPrt%1000)*1000;
		}

		FD_ZERO(&rd_fd); FD_SET(s.sock, &rd_fd);
		kz = select(s.sock+1, &rd_fd, NULL, NULL, &tv);
		if((kz == 0 && !pollPrt) || (kz == -1 && errno == EINTR) || (kz > 0 && !FD_ISSET(s.sock,&rd_fd))) continue;
		if(kz < 0) {
		    if(mess_lev() == TMess::Debug)
			mess_debug(s.s->nodePath().c_str(), _("Socket has been terminated by the error %s"), strerror(errno));
		    break;
		}
	    }

	    rez = 0;
	    if(kz && (rez=BIO_read(s.bio,buf,sizeof(buf))) <= 0) {
		if(rez < 0 && errno == EAGAIN)	continue;	//!!!! Appeared at the initiative connections
		break;
	    }
	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("The message is received in size %d."), rez);
	    req.assign(buf, rez);
	    if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:< Received from '%s'\n"),s.sock,s.sender.c_str()), req, -s.sock);

	    s.s->dataRes().lock();
	    s.s->trIn += rez; s.trIn += rez;
	    s.s->dataRes().unlock();

	    int64_t stTm = 0;
	    if(mess_lev() == TMess::Debug) stTm = SYS->curTime();
	    s.s->messPut(s.sock, req, answ, s.sender, prot_in);
	    if(mess_lev() == TMess::Debug && stTm) {
		s.s->dataRes().lock();
		s.prcTm = s.s->prcTm = SYS->curTime() - stTm;
		s.prcTmMax = vmax(s.prcTmMax, s.prcTm);
		s.s->prcTmMax = vmax(s.s->prcTmMax, s.s->prcTm);
		s.s->dataRes().unlock();
	    }

	    if(answ.size()) {
		if(mess_lev() == TMess::Debug)
		    mess_debug(s.s->nodePath().c_str(), _("The reply by the message in size %d."), answ.size());
		do { rez = BIO_write(s.bio, answ.data(), answ.size()); }
		while(rez < 0 && SSL_get_error(ssl,rez) == SSL_ERROR_WANT_WRITE);

		s.s->dataRes().lock();
		s.s->trOut += vmax(0, rez); s.trOut += vmax(0, rez);
		s.s->dataRes().unlock();
		if(rez > 0 && s.s->logLen())
		    s.s->pushLogMess(TSYS::strMess(_("%d:> Transmitted to '%s'\n"),s.sock,s.sender.c_str()),
			string(answ.data(),rez), s.sock);
		answ = "";
	    }
	    cnt++;
	    s.tmReq = tm = time(NULL);

	} while(!s.s->endrunCl && (s.s->mode() == M_Initiative || ((!s.s->keepAliveReqs() || cnt < s.s->keepAliveReqs()) &&
		(!s.s->keepAliveTm() || (time(NULL)-tm) < s.s->keepAliveTm()))));

	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("Has been disconnected by '%s'!"), s.sender.c_str());
    } catch(TError &err) {
	if(mess_lev() == TMess::Debug)
	    mess_debug(s.s->nodePath().c_str(), _("The socket has been terminated by the exception %s"), err.mess.c_str());
	if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Has been terminated by the exception %s"),s.sock,err.mess.c_str()));
    }

    if(!s.isCon) { BIO_flush(s.bio); BIO_free_all(s.bio); }

    //Close protocol on broken connection
    for(unsigned iP = 0; iP < prot_in.size(); iP++) {
	if(prot_in[iP].freeStat())	continue;
	try {
	    string n_pr = prot_in[iP].at().name();
	    AutoHD<TProtocol> proto = AutoHD<TProtocol>(&prot_in[iP].at().owner());
	    prot_in[iP].free();
	    proto.at().close(n_pr);
	} catch(TError &err) {
	    if(mess_lev() == TMess::Debug)
		mess_debug(s.s->nodePath().c_str(), _("The socket has been terminated by the exception %s"), err.mess.c_str());
	    if(s.s->logLen()) s.s->pushLogMess(TSYS::strMess(_("%d:Has been terminated by the exception %s"),s.sock,err.mess.c_str()));
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
	if(iP < (int)prot_in.size() && !prot_in[iP].freeStat()) { iActP++; continue; }
	try {
	    AutoHD<TProtocol> proto = SYS->protocol().at().modAt(TSYS::strParse(prt,0,"."));
	    subPrt = TSYS::strParse(prt, 1, ".");
	    string n_pr = id() + i2s(sock) + (subPrt.size()?"#"+subPrt:"");
	    if(!proto.at().openStat(n_pr)) proto.at().open(n_pr, this, sender+"\n"+i2s(sock));
	    if(iP < (int)prot_in.size()) prot_in[iP] = proto.at().at(n_pr);
	    else prot_in.push_back(proto.at().at(n_pr));
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("The new input protocol's object '%s' is created!"), n_pr.c_str());
	} catch(TError &err) {
	    initErr = true;
	    mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
	}
    }

    if(initErr && !iActP)
	mess_warning(nodePath().c_str(), _("All protocols is wrong for their initialization."));

    return iActP;
}

int TSocketIn::messPut( int sock, string &request, string &answer, string sender, vector< AutoHD<TProtocolIn> > &prot_in )
{
    if(!prtInit(prot_in,sock,sender))	return 0;

    int iActP = 0;
    string n_pr, tAnsw;

    for(unsigned iP = 0; iP < prot_in.size(); iP++, answer += tAnsw, tAnsw = "") {
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

    for(unsigned iId = 0; iId < clId.size(); iId++)
	if(clId[iId] == so) return;

    clId.push_back(so);
    clS[so->sender]++;
    clFree = false;

    if(logLen()) pushLogMess(TSYS::strMess(_("New client %d of '%s' connected"),so->sock,so->sender.c_str()));
}

void TSocketIn::clientUnreg( SSockIn *so )
{
    MtxAlloc res(sockRes, true);

    for(unsigned iId = 0; iId < clId.size(); iId++)
	if(clId[iId] == so) {
	    if(logLen()) pushLogMess(TSYS::strMess(_("The client %d of '%s' disconnected"),so->sock,so->sender.c_str()));

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
	if(ctrMkNode("area",opt,1,"/prm/st",_("State"))) {
	    if(protocols().size() && clId.size() && mode() != M_Initiative)
		ctrMkNode("list", opt, -1, "/prm/st/conns", _("Active connections"), R_R_R_, "root", STR_ID);
	    if(ssl) ctrMkNode("fld",opt,-1,"/prm/st/ciphers",_("Supported ciphers"),R_R_R_,"root",STR_ID,3,"tp","str", "cols","90", "rows","3");
	}
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID,1,"help",
	    _("SSL input transport has the address format \"{addr}:{port}[:{mode}[:{IDmess}]]\", where:\n"
	    "    addr - address to open SSL, it must be one of the addresses of the host; empty or \"*\" address opens SSL for all interfaces; "
	    "there may be as the symbolic representation as well as IPv4 \"127.0.0.1\" or IPv6 \"[::1]\";\n"
	    "    port - network port on which the SSL is opened, indication of the character name of the port, according to /etc/services is available;\n"
	    "    mode - mode of operation: 0(default) - ordinal connection; 2 - initiative connection;\n"
	    "    IDmess - identification message of the initiative connection - the mode 2."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/PROT",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",STR_ID);

	int pos = 0;
	ctrMkNode("fld",opt,pos++,"/aprm/taskPrior",_("Priority"),RWRWR_,"root",STR_ID,2, "tp","dec", "help",TMess::labTaskPrior().c_str());
	ctrMkNode("fld",opt,pos++,"/aprm/inBfLn",_("Input buffer size, kB"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,
	    "tp","dec", "help",_("Set 0 for the system value."));
	ctrMkNode("fld",opt,pos++,"/aprm/MSS",_("Maximum segment size (MSS), B"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,
	    "tp","dec", "help",_("Set 0 for the system value."));
	if(!startStat()) {
	    if(certKey().empty())
		ctrMkNode("fld",opt,pos++,"/aprm/certKeyFile",_("PEM-file of the certificates and private key"),RWRW__,"root",STR_ID,3,
		    "tp","str","dest","sel_ed","select","/aprm/certKeyFileList");
	    if(certKeyFile().empty())
		ctrMkNode("fld",opt,pos++,"/aprm/certKey",_("Certificates and private key"),RWRW__,"root",STR_ID,4,
		    "tp","str","cols","90","rows","7","help",_("SSL PAM certificates chain and private key."));
	    ctrMkNode("fld",opt,pos++,"/aprm/pkey_pass",_("Private key password"),RWRW__,"root",STR_ID,1,"tp","str");
	}
	if(mode() != M_Initiative) {
	    ctrMkNode("fld",opt,pos++,"/aprm/cl_n",_("Maximum number of clients"),RWRWR_,"root",STR_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,pos++,"/aprm/cl_n_pHost",_("Maximum number of clients per host"),RWRWR_,"root",STR_ID,2,"tp","dec",
		"help",_("Set to 0 to disable this limit."));
	    ctrMkNode("fld",opt,pos++,"/aprm/keepAliveReqs",_("Keep alive requests"),RWRWR_,"root",STR_ID,2,"tp","dec",
		"help",_("Closing the connection after the specified requests.\nZero value to disable - do not close ever."));
	}
	ctrMkNode("fld",opt,pos++,"/aprm/keepAliveTm",_("Keep alive timeout, seconds"),RWRWR_,"root",STR_ID,2,"tp","dec",
	    "help",_("Closing the connection after no requests at the specified timeout.\nZero value to disable - do not close ever."));
	if(mode() == M_Initiative || protocols().empty())
	    ctrMkNode("fld",opt,pos++,"/aprm/initAssocPrms",_("Timeouts, tries"),RWRW__,"root",STR_ID,2,"tp","str",
		"help",(_("... of the initiative connection and the associated output transports, empty for default and separated by '||'. ")+
			 ((TTransSock&)owner()).outTimingsHelp(true)+((TTransSock&)owner()).outAttemptsHelp(true)).c_str());
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
    else if(a_path == "/prm/st/ciphers" && ctrChkNode(opt) && ssl) {
	bool toFree = false;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	STACK_OF(SSL_CIPHER) *sk = SSL_get1_supported_ciphers(ssl);
	toFree = true;
#else
	STACK_OF(SSL_CIPHER) *sk = SSL_get_ciphers(ssl);
#endif
	for(int iL = 0; iL < sk_SSL_CIPHER_num(sk); iL++) {
	    const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, iL);
	    opt->setText(opt->text() + (iL?", ":"") + SSL_CIPHER_get_name(c) + " ("+SSL_CIPHER_get_version(c)+")");
	}
	if(toFree) sk_SSL_CIPHER_free(sk);
    }
    else if(a_path == "/aprm/taskPrior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(taskPrior()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTaskPrior(s2i(opt->text()));
    }
    else if(a_path == "/aprm/inBfLn") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(inBufLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setInBufLen(s2i(opt->text()));
    }
    else if(a_path == "/aprm/MSS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(MSS()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMSS(s2i(opt->text()));
    }
    else if(a_path == "/aprm/certKeyFile") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(certKeyFile());
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setCertKeyFile(opt->text());
    }
    else if(a_path == "/aprm/certKey") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(certKey());
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setCertKey(opt->text());
    }
    else if(a_path == "/aprm/pkey_pass") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(string(pKeyPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setPKeyPass(opt->text());
    }
    else if(a_path == "/aprm/cl_n") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxFork()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxFork(s2i(opt->text()));
    }
    else if(a_path == "/aprm/cl_n_pHost") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(maxForkPerHost()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMaxForkPerHost(s2i(opt->text()));
    }
    else if(a_path == "/aprm/keepAliveReqs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveReqs()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveReqs(s2i(opt->text()));
    }
    else if(a_path == "/aprm/keepAliveTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(keepAliveTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setKeepAliveTm(s2i(opt->text()));
    }
    else if(a_path == "/aprm/initAssocPrms") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(initAssocPrms());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setInitAssocPrms(opt->text());
    }
    else if(a_path == "/aprm/certKeyFileList" && ctrChkNode(opt)) TSYS::ctrListFS(opt, certKeyFile(), "pem;");
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TSocketOut                                   *
//************************************************
TSocketOut::TSocketOut( string name, const string &idb, TElem *el ) : TTransportOut(name, idb, el),
    mAttemts(1), mMSS(0), ssl(NULL), conn(NULL)
{
    setAddr("localhost:" DEF_PORT);
    setTimings(DEF_TMS, true);
}

TSocketOut::~TSocketOut( )	{ }

string TSocketOut::getStatus( )
{
    string rez = TTransportOut::getStatus();

    if(startStat()) {
	rez += TSYS::strMess(_("To the host '%s'. "), connAddr.c_str());
	rez += TSYS::strMess(_("Traffic in %s, out %s. "), TSYS::cpct2str(trIn).c_str(), TSYS::cpct2str(trOut).c_str());

	if(mess_lev() == TMess::Debug && respTmMax)
	    rez += TSYS::strMess(_("Response time %s[%s]. "), tm2s(1e-6*respTm).c_str(), tm2s(1e-6*respTmMax).c_str());

	int bufSz, MSSsz; socklen_t sz = sizeof(socklen_t), sockFd = BIO_get_fd(conn, NULL);
	getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, (void*)&bufSz, &sz);
	getsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, (void*)&MSSsz, &sz);
	rez += TSYS::strMess(_("Size input buffer %s, MSS %s. "), TSYS::cpct2str(bufSz).c_str(), TSYS::cpct2str(MSSsz).c_str());
    }

    return rez;
}

void TSocketOut::setTimings( const string &vl, bool isDef )
{
    if((isDef && !mDefTimeouts) || vl == mTimings) return;
    else if(!isDef) mDefTimeouts = false;

    mTmCon = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,0,":"))*1e3)));
    mTmNext = vmax(1, vmin(60000,(int)(s2r(TSYS::strParse(vl,1,":"))*1e3)));
    mTimings = TSYS::strMess("%g:%g", (1e-3*mTmCon), (1e-3*mTmNext));

    if(!isDef) modif();
}

void TSocketOut::setAttempts( unsigned short vl )
{
    if(vl == mAttemts)	return;
    mAttemts = vmax(1, vmin(5,vl));
    modif();
}

void TSocketOut::load_( )
{
    setCertKeyFile(prm("CertKeyFile",""));
    setCertKey(prm("CertKey",""));
    setPKeyPass(prm("PKeyPass",""));
    setTimings(prm("TMS",DEF_TMS));
    setMSS((int)prm("MSS",DEF_MSS));
}

void TSocketOut::save_( )
{
    if(addr().find(S_NM_SOCKET ":") != string::npos)	return;

    prm("CertKeyFile", certKeyFile(), true);
    prm("CertKey", certKey(), true);
    prm("PKeyPass", pKeyPass(), true);
    prm("TMS", timings(), true);
    prm("MSS", (int)MSS(), true);

    TTransportOut::save_();
}

string TSocketOut::connectSSL( const string &addr, SSL **ssl, BIO **conn,
    int tmCon, const string &certKey, const string &pKeyPass, const string &certKeyFile )
{
    int sockFd = -1, aOff = 0;
    string	cfile, aErr, connAddr;
    char	err[255];

    *ssl = NULL; *conn = NULL;

    string addr_ = addr, ssl_host, ssl_host_;
    if(addr_[aOff] != '[') ssl_host = TSYS::strParse(addr_, 0, ":", &aOff);
    else { aOff++; ssl_host = TSYS::strParse(addr_, 0, "]:", &aOff); }	//Get IPv6
    string ssl_port = TSYS::strParse(addr_, 0, ":", &aOff);

    for(int off = 0; (ssl_host_=TSYS::strParse(ssl_host,0,",",&off)).size(); ) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	int error;

	MtxAlloc aRes(*SYS->commonLock("getaddrinfo"), true);
	if((error=getaddrinfo(ssl_host_.c_str(),(ssl_port.size()?ssl_port.c_str():DEF_PORT),&hints,&res)))
	    throw TError(mod->nodePath().c_str(), _("Error the address '%s': '%s (%d)'"), addr_.c_str(), gai_strerror(error), error);
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
	for(unsigned iA = 0; iA < addrs.size(); iA++) {
	    try {
		if((sockFd=socket((((sockaddr*)&addrs[iA])->sa_family==AF_INET6)?PF_INET6:PF_INET,SOCK_STREAM,0)) == -1)
		    throw TError(mod->nodePath().c_str(), _("Error creating TCP socket: %s!"), strerror(errno));
		int vl = 1;
		setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));

		//Connect to the socket
		int flags = fcntl(sockFd, F_GETFL, 0);
		fcntl(sockFd, F_SETFL, flags|O_NONBLOCK);
		int res = connect(sockFd, (sockaddr*)&addrs[iA], sizeof(addrs[iA]));
		if(res == -1 && errno == EINPROGRESS) {
		    struct timeval tv;
		    socklen_t slen = sizeof(res);
		    fd_set fdset;
		    tv.tv_sec = tmCon/1000; tv.tv_usec = 1000*(tmCon%1000);
		    FD_ZERO(&fdset); FD_SET(sockFd, &fdset);
		    if((res=select(sockFd+1,NULL,&fdset,NULL,&tv)) > 0 && !getsockopt(sockFd,SOL_SOCKET,SO_ERROR,&res,&slen) && !res) res = 0;
		    else res = -1;
		}
		if(res)	throw TError(mod->nodePath().c_str(),
		    _("Error connecting to the internet socket '%s:%s' during the time %s, it seems in down or inaccessible: '%s (%d)'!"),
		    ssl_host_.c_str(), ssl_port.c_str(), tm2s(1e-3*tmCon).c_str(), strerror(errno), errno);

		//Certificates, private key and it password loading

		// Try the external PEM-file of the certificates and the private key
		if(certKeyFile.size()) cfile = certKeyFile;
		// Write certificate and private key to temorary file
		else if(!sTrm(certKey).empty()) {
#if defined(__ANDROID__)
		    cfile = MOD_TYPE "_" MOD_ID "_" + i2s(rand()) + ".tmp";
#else
		    cfile = tmpnam(err);
#endif
		    int icfile = open(cfile.c_str(), O_EXCL|O_CREAT|O_WRONLY, 0600);
		    if(icfile < 0) throw TError(mod->nodePath().c_str(), _("Error opening the temporary file '%s': '%s'"), cfile.c_str(), strerror(errno));
		    bool fOK = (write(icfile,certKey.data(),certKey.size()) == (int)certKey.size());
		    if(close(icfile) != 0)
			mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), icfile, strerror(errno), errno);
		    if(!fOK) throw TError(mod->nodePath().c_str(), _("Error writing the file '%s'."), cfile.c_str());
		}

		if(cfile.size()) {
		    // Set private key password
		    SSL_CTX_set_default_passwd_cb_userdata(mod->ctxOut, (char*)pKeyPass.c_str());
		    // Load certificate
		    if(SSL_CTX_use_certificate_chain_file(mod->ctxOut,cfile.c_str()) != 1) {
			ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
			throw TError(mod->nodePath().c_str(), _("SSL_CTX_use_certificate_chain_file: %s"), err);
		    }
		    // Load private key
		    if(SSL_CTX_use_PrivateKey_file(mod->ctxOut,cfile.c_str(),SSL_FILETYPE_PEM) != 1) {
			ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
			throw TError(mod->nodePath().c_str(), _("SSL_CTX_use_PrivateKey_file: %s"), err);
		    }

		    // Remove the temporary certificate file
		    if(certKeyFile.empty())	remove(cfile.c_str());
		    cfile = "";
		}

		if((*ssl=SSL_new(mod->ctxOut)) == NULL) {
		    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		    throw TError(mod->nodePath().c_str(), "SSL_new: %s", err);
		}

		SSL_set_connect_state(*ssl);
		SSL_set_mode(*ssl, SSL_MODE_AUTO_RETRY);
		SSL_set_read_ahead(*ssl, 1);

		if(SSL_set_fd(*ssl,sockFd) != 1) {
		    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		    throw TError(mod->nodePath().c_str(), "SSL_set_fd: %s", err);
		}

		fcntl(sockFd, F_SETFL, flags);	//Clear nonblock
		if(SSL_connect(*ssl) != 1) {
		    ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
		    throw TError(mod->nodePath().c_str(), "SSL_connect: %s", err);
		}

		*conn = BIO_new(BIO_f_ssl());
		BIO_set_ssl(*conn, *ssl, BIO_NOCLOSE);
		BIO_set_nbio(*conn, 1);

		fcntl(sockFd, F_SETFL, flags|O_NONBLOCK);

		//Get the connected address
		if(((sockaddr*)&addrs[iA])->sa_family == AF_INET6) {
		    char aBuf[INET6_ADDRSTRLEN];
		    getnameinfo((sockaddr*)&addrs[iA], sizeof(addrs[iA]), aBuf, sizeof(aBuf), 0, 0, NI_NUMERICHOST);
		    connAddr = aBuf;
		} else connAddr = inet_ntoa(((sockaddr_in*)&addrs[iA])->sin_addr);

	    } catch(TError &err) {
		aErr = err.mess;
		if(*conn)	BIO_reset(*conn);
		if(sockFd >= 0 && close(sockFd) != 0)
		    mess_warning(mod->nodePath().c_str(), _("Closing the socket %d error '%s (%d)'!"), sockFd, strerror(errno), errno);
		sockFd = -1;
		if(*conn)	{ BIO_free_all(*conn); *conn = NULL; }
		if(*ssl)	{ SSL_free(*ssl); *ssl = NULL; }
		if(!cfile.empty() && certKeyFile.empty()) remove(cfile.c_str());
		continue;	//Try next
	    }
	    break;	//OK
	}

	if(sockFd >= 0) break;
    }

    if(sockFd < 0) throw TError(mod->nodePath(), aErr);

    return connAddr;
}

void TSocketOut::disconnectSSL( SSL **ssl, BIO **conn )
{
    //SSL deinit
    if(conn && *conn) {
	BIO_flush(*conn);
	BIO_reset(*conn);

	if(close(BIO_get_fd(*conn,NULL)) != 0)
	    mess_warning(mod->nodePath().c_str(), _("Closing the socket %d error '%s (%d)'!"), BIO_get_fd(*conn,NULL), strerror(errno), errno);
	BIO_free_all(*conn);

	*conn = NULL;
    }
    if(ssl && *ssl) { SSL_free(*ssl); *ssl = NULL; }
}

void TSocketOut::start( int tmCon )
{
    MtxAlloc resReq(reqRes(), true);
    if(runSt) return;
    if(SYS->stopSignal()) throw TError(nodePath(), _("We are stopping!"));

    //Status cleaning
    trIn = trOut = respTm = respTmMax = 0;

    if(addr().find(S_NM_SOCKET ":") == string::npos) {
	// Reading the global arguments
	string tVl;
	if((tVl=TSYS::strParse(addr(),1,"||")).size()) setTimings(tVl);
	if((tVl=TSYS::strParse(addr(),2,"||")).size()) setAttempts(s2i(tVl));

	try {
	    connAddr = connectSSL(TSYS::strParse(addr(),0,"||"), &ssl, &conn,
				    tmCon?tmCon:mTmCon, certKey(), pKeyPass(), certKeyFile());

	    int sockFd = BIO_get_fd(conn,NULL);
	    int vl = 1; setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));
	    if(MSS()) { vl = MSS(); setsockopt(sockFd, IPPROTO_TCP, TCP_MAXSEG, &vl, sizeof(int)); }
	} catch(TError &err) {
	    if(logLen()) pushLogMess(TSYS::strMess(_("Error connecting: %s"),err.mess.c_str()));
	    throw;
	}
    }
    else {
	if(s2i(TSYS::strParse(addr(),1,":")) < 0)
	    throw TError(nodePath(), _("The force socket is deactivated!"));
	if(!conn)
	    throw TError(nodePath(), _("The force socket is not activated!"));
	int sockFd = BIO_get_fd(conn,NULL),
	    flags = fcntl(sockFd, F_GETFL, 0);
	if(fcntl(sockFd,F_SETFL,flags|O_NONBLOCK) < 0) {
	    disconnectSSL(NULL, &conn); ssl = NULL; setAddr(S_NM_SOCKET ":-1");
	    throw TError(nodePath().c_str(), _("Error the force socket %d using: '%s (%d)'!"), sockFd, strerror(errno), errno);
	}
    }

    mLstReqTm = TSYS::curTime();

    runSt = true;

    TTransportOut::start();

    if(logLen()) pushLogMess(_("Connected"));
}

void TSocketOut::stop( )
{
    MtxAlloc res(reqRes(), true);
    if(!runSt) return;

    //Status clear
    trIn = trOut = 0;

    if(addr().find(S_NM_SOCKET ":") != string::npos) {
	if(conn) { BIO_flush(conn); BIO_free_all(conn); conn = NULL; }
	//disconnectSSL(NULL, NULL, &conn);
	ssl = NULL;
	setAddr(S_NM_SOCKET ":-1");
    } else disconnectSSL(&ssl, &conn);

    runSt = false;

    TTransportOut::stop();

    if(logLen()) pushLogMess(_("Disconnected"));
}

int TSocketOut::messIO( const char *oBuf, int oLen, char *iBuf, int iLen, int time )
{
    string err = _("Unknown error");
    int	 ret = 0, reqTry = 0;;
    char err_[255];
    bool notReq = (time < 0),
	 writeReq = false;
    time = abs(time);
    unsigned short wAttempts = attempts();

    MtxAlloc res(reqRes(), true);

    if(!runSt) throw TError(nodePath(), _("Transport is not connected!"));

repeate:
    if(reqTry++ >= wAttempts)	{ mLstReqTm = TSYS::curTime(); throw TError(nodePath().c_str(), _("Error requesting: %s"), err.c_str()); }

    int64_t stRespTm = 0;

    //Write request
    if(oBuf != NULL && oLen > 0) {
	if(!time) time = mTmCon;
	// Input buffer clear
	while(!notReq && BIO_read(conn,err_,sizeof(err_)) > 0) ;
	// Write request
	do { ret = BIO_write(conn, oBuf, oLen); } while(ret < 0 && SSL_get_error(ssl,ret) == SSL_ERROR_WANT_WRITE);
	if(ret <= 0) {
	    err = TSYS::strMess("%s (%d)", strerror(errno), errno);
	    stop();
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error writing: %s"), err.c_str());
	    if(logLen()) pushLogMess(TSYS::strMess(_("Error writing: %s"), err.c_str()));
	    if(notReq) throw TError(nodePath().c_str(), _("Error writing: %s"), err.c_str());
	    start();
	    if(ret == 0 && wAttempts == 1) wAttempts = 2;	//!!!! To restore the lost connections
	    goto repeate;
	}

	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Wrote %s."), TSYS::cpct2str(oLen).c_str());
	if(logLen()) pushLogMess(_("> Transmitted to\n"), string(oBuf,ret), 1);

	trOut += ret;
	writeReq = true;

	if(mess_lev() == TMess::Debug) stRespTm = SYS->curTime();
    }
    else if(!notReq) time = mTmNext;
    if(!time) time = 5000;

    //Read reply
    if(iBuf != NULL && iLen > 0) {
	ret = BIO_read(conn, iBuf, iLen);
	if(ret > 0) trIn += ret;
	else if(ret == 0) {
	    err = errno ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No data, the connection seems closed");
	    stop();
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading: %s"), err.c_str());
	    if(logLen()) pushLogMess(TSYS::strMess(_("Error reading: %s"), err.c_str()));
	    if(!writeReq || notReq) throw TError(nodePath().c_str(),_("Error reading: %s"), err.c_str());
	    start();
	    if(!errno && wAttempts == 1) wAttempts = 2;		//!!!! To restore the lost connections
	    goto repeate;
	}
	else if(ret < 0 && SSL_get_error(ssl,ret) != SSL_ERROR_WANT_READ && SSL_get_error(ssl,ret) != SSL_ERROR_WANT_WRITE && errno != EAGAIN) {
	    ERR_error_string_n(ERR_peek_last_error(), err_, sizeof(err_));
	    throw TError(nodePath().c_str(), "BIO_read: %s", err_);
	}
	else {
	    //Wait data from the socket
	    int kz = 0;
	    fd_set rd_fd;
	    struct timeval tv;

	    int sockFd = BIO_get_fd(conn, NULL);
	    tv.tv_sec  = time/1000; tv.tv_usec = 1000*(time%1000);
	    FD_ZERO(&rd_fd); FD_SET(sockFd, &rd_fd);
	    kz = select(sockFd+1, &rd_fd, NULL, NULL, &tv);
	    if(kz == 0) {
		err = _("Reading timeouted.");
		if(!notReq) {
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), err.c_str());
		    if(logLen()) pushLogMess(err.c_str());
		    if(writeReq) {
			//!!!! For the initial input connection we must keep the connection up to the last
			if(addr().find(S_NM_SOCKET ":") != string::npos) {
			    if(reqTry >= wAttempts) stop();
			    else goto repeate;
			}
			//!!!! Must be reconnected to prevent the possibility of getting response of the previous request.
			else {
			    stop();
			    if(reqTry < wAttempts) { start(); goto repeate; }
			}
		    }
		}
		mLstReqTm = TSYS::curTime();
		throw TError(nodePath(), err);
	    }
	    else if(kz < 0) {
		err = TSYS::strMess("%s (%d)", strerror(errno), errno);
		stop();
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading (select): %s"), err.c_str());
		if(logLen()) pushLogMess(TSYS::strMess(_("Error reading (select): %s"), err.c_str()));
		mLstReqTm = TSYS::curTime();
		throw TError(nodePath().c_str(),_("Error reading (select): %s"), err.c_str());
	    }
	    else if(FD_ISSET(sockFd,&rd_fd)) {
		while((ret=BIO_read(conn,iBuf,iLen)) == -1 && errno == EAGAIN) sched_yield();
		if(ret < 0) {
		    err = (ret < 0) ? TSYS::strMess("%s (%d)",strerror(errno),errno) : _("No data, the connection seems closed");
		    stop();
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error reading: %s"), err.c_str());
		    if(logLen()) pushLogMess(TSYS::strMess(_("Error reading: %s"),err.c_str()));
		    // * Pass to retry in the request mode and on the successful writing
		    if(!writeReq || notReq) throw TError(nodePath().c_str(),_("Error reading: %s"), err.c_str());
		    start();
		    goto repeate;
		}
		trIn += vmax(0, ret);
	    }
	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Read %s."), TSYS::cpct2str(vmax(0,ret)).c_str());
	    if(ret > 0 && logLen()) pushLogMess(_("< Received from\n"), string(iBuf,ret), -1);
	    if(ret > 0 && mess_lev() == TMess::Debug && stRespTm) {
		respTm = SYS->curTime() - stRespTm;
		respTmMax = vmax(respTmMax, respTm);
	    }
	}
    }

    mLstReqTm = TSYS::curTime();

    return vmax(0, ret);
}

void TSocketOut::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TTransportOut::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/st",_("State")) && ssl)
	    ctrMkNode("fld",opt,-1,"/prm/st/ciphers",_("Supported ciphers"),R_R_R_,"root",STR_ID,3,"tp","str", "cols","90", "rows","3");
	if(opt->childSize() && ctrId(opt->childGet(0),"/prm/cfg",true))
	    ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1, "help",owner().outAddrHelp().c_str());

	if(opt->childSize() && ctrId(opt->childGet(0),"/aprm",true)) {
	    int pos = 0;
	    if(!startStat()) {
		if(certKey().empty())
		    ctrMkNode("fld",opt,pos++,"/aprm/certKeyFile",_("PEM-file of the certificates and private key"),RWRW__,"root",STR_ID,3,
			"tp","str","dest","sel_ed","select","/aprm/certKeyFileList");
		if(certKeyFile().empty())
		    ctrMkNode("fld",opt,pos++,"/aprm/certKey",_("Certificates and private key"),RWRW__,"root",STR_ID,4,
			"tp","str","cols","90","rows","7",
			"help",_("SSL PAM certificates chain and private key."));
		ctrMkNode("fld",opt,pos++,"/aprm/pkey_pass",_("Private key password"),RWRW__,"root",STR_ID,1,"tp","str");
	    }
	    ctrMkNode("fld",opt,pos++,"/aprm/TMS",_("Timings"),RWRWR_,"root",STR_ID,2, "tp","str", "help",((TTransSock&)owner()).outTimingsHelp().c_str());
	    ctrMkNode("fld",opt,pos++,"/aprm/attempts",_("Attempts"),RWRWR_,"root",STR_ID,2, "tp","dec", "help",((TTransSock&)owner()).outAttemptsHelp().c_str());
	    ctrMkNode("fld",opt,pos++,"/aprm/MSS",_("Maximum segment size (MSS), B"),startStat()?R_R_R_:RWRWR_,"root",STR_ID,2,
		"tp","dec","help",_("Set 0 for the system value."));
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/ciphers" && ctrChkNode(opt) && ssl) {
	bool toFree = false;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	STACK_OF(SSL_CIPHER) *sk = SSL_get1_supported_ciphers(ssl);
	toFree = true;
#else
	STACK_OF(SSL_CIPHER) *sk = SSL_get_ciphers(ssl);
#endif
	for(int iL = 0; iL < sk_SSL_CIPHER_num(sk); iL++) {
	    const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, iL);
	    opt->setText(opt->text() + (iL?", ":"") + SSL_CIPHER_get_name(c) + " ("+SSL_CIPHER_get_version(c)+")");
	}
	if(toFree) sk_SSL_CIPHER_free(sk);
    }
    else if(a_path == "/aprm/MSS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(MSS()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setMSS(s2i(opt->text()));
    }
    else if(a_path == "/aprm/certKeyFile") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(certKeyFile());
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setCertKeyFile(opt->text());
    }
    else if(a_path == "/aprm/certKey") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(certKey());
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setCertKey(opt->text());
    }
    else if(a_path == "/aprm/pkey_pass") {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(string(pKeyPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setPKeyPass(opt->text());
    }
    else if(a_path == "/aprm/TMS") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(timings());
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setTimings(opt->text());
    }
    else if(a_path == "/aprm/attempts") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",STR_ID,SEC_RD))	opt->setText(i2s(attempts()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",STR_ID,SEC_WR))	setAttempts(s2i(opt->text()));
    }
    else if(a_path == "/aprm/certKeyFileList" && ctrChkNode(opt)) TSYS::ctrListFS(opt, certKeyFile(), "pem;");
    else TTransportOut::cntrCmdProc(opt);
}

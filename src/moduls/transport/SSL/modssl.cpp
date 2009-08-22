
//OpenSCADA system module Transport.SSL file: modssl.cpp
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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

#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <openssl/rand.h>

#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>
#include "modssl.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"SSL"
#define MOD_NAME	"SSL"
#define MOD_TYPE	"Transport"
#define VER_TYPE	VER_TR
#define VERSION		"0.8.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow security socket layer based transport. Used OpenSSL and supported SSLv2, SSLv3 and TLSv1."
#define LICENSE		"GPL"
//************************************************

MSSL::TTransSock *MSSL::mod;

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
	    return new MSSL::TTransSock( source );
	return NULL;
    }
}

using namespace MSSL;

//************************************************
//* TTransSock					 *
//************************************************
TTransSock::TTransSock( string name )
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

    //> CRYPTO reentrant init
    mutex_buf = (pthread_mutex_t *)malloc( CRYPTO_num_locks( ) * sizeof(pthread_mutex_t) );
    for( int i = 0; i < CRYPTO_num_locks( ); i++ )
	pthread_mutex_init(&mutex_buf[i],NULL);
    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);
    CRYPTO_set_dynlock_create_callback(dyn_create_function);
    CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
    CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);

    //> SSL init
    SSL_library_init();
    SSL_load_error_strings();
    RAND_load_file("/dev/urandom",1024);
}

TTransSock::~TTransSock()
{
    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);
    for( int i = 0; i < CRYPTO_num_locks(); i++ )
	pthread_mutex_destroy(&mutex_buf[i]);
    free(mutex_buf);
}

void TTransSock::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeConnect )
    {
	//> Add self DB-fields to input transport
	owner().inEl().fldAdd( new TFld("BufLen",_("Input socket buffer length (kB)"),TFld::Integer,0,"4","5") );
	owner().inEl().fldAdd( new TFld("MaxClients",_("Maximum clients process"),TFld::Integer,0,"3","10") );
	owner().inEl().fldAdd( new TFld("SSLCertKey",_("SSL PEM certificat and private key"),TFld::String,TFld::FullText,"10000") );
	owner().inEl().fldAdd( new TFld("SSLKeyPass",_("SSL key password"),TFld::String,0,"20") );

	//> Add self DB-fields to input transport
	owner().outEl().fldAdd( new TFld("SSLCertKey",_("SSL PEM certificat and private key"),TFld::String,TFld::FullText,"10000") );
	owner().outEl().fldAdd( new TFld("SSLKeyPass",_("SSL key password"),TFld::String,0,"20") );
    }
}

unsigned long TTransSock::id_function( )
{
    return ((unsigned long)pthread_self());
}

void TTransSock::locking_function( int mode, int n, const char * file, int line )
{
    if( mode&CRYPTO_LOCK )	pthread_mutex_lock(&mod->mutex_buf[n]);
    else			pthread_mutex_unlock(&mod->mutex_buf[n]);
}

struct CRYPTO_dynlock_value *TTransSock::dyn_create_function( const char *file, int line )
{
    struct CRYPTO_dynlock_value *value;
    value = (struct CRYPTO_dynlock_value *)malloc( sizeof(struct CRYPTO_dynlock_value) );
    if( value )	pthread_mutex_init(&value->mutex,NULL);

    return value;
}

void TTransSock::dyn_lock_function( int mode, struct CRYPTO_dynlock_value *l, const char *file, int line )
{
    if( mode&CRYPTO_LOCK )	pthread_mutex_lock(&l->mutex);
    else			pthread_mutex_unlock(&l->mutex);
}

void TTransSock::dyn_destroy_function( struct CRYPTO_dynlock_value *l, const char *file, int line )
{
    pthread_mutex_destroy(&l->mutex);
    free(l);
}

string TTransSock::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TTransSock::load_( )
{
    //- Load parameters from command line -
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
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

TTransportIn *TTransSock::In( const string &name, const string &idb )
{
    return new TSocketIn(name,idb,&owner().inEl());
}

TTransportOut *TTransSock::Out( const string &name, const string &idb )
{
    return new TSocketOut(name,idb,&owner().outEl());
}

void TTransSock::cntrCmdProc( XMLNode *opt )
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
//* TSocketIn                                    *
//************************************************
TSocketIn::TSocketIn( string name, const string &idb, TElem *el ) :
    TTransportIn(name,idb,el), ctx(NULL), cl_free(true),
    mBufLen(cfg("BufLen").getId()), mMaxFork(cfg("MaxClients").getId()),
    mCertKey(cfg("SSLCertKey").getSd()), mKeyPass(cfg("SSLKeyPass").getSd())
{
    setAddr("localhost:10042");
}

TSocketIn::~TSocketIn()
{
    try{ stop(); }catch(...){ }
}

string TSocketIn::getStatus( )
{
    string rez = TTransportIn::getStatus( );

    if( !startStat() && !stErr.empty() )
	rez += _("Start error: ") + stErr;
    else if( startStat() )
    {
	rez += TSYS::strMess(_("Connections %d, opened %d. Traffic in %.4g kb, out %.4g kb. Closed connections by limit %d."),
	    connNumb,opConnCnt(),trIn,trOut,clsConnByLim);
    }

    return rez;
}

void TSocketIn::start()
{
    pthread_attr_t pthr_attr;

    if( run_st ) return;

    //- Status clear -
    stErr = "";
    trIn = trOut = 0;
    connNumb = clsConnByLim = 0;

    //- Wait connection main task start -
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true,nodePath()+"open",5) )
	throw TError(nodePath().c_str(),_("Not opened!"));
}

void TSocketIn::stop()
{
    if( !run_st )	return;

    //- Status clear -
    stErr = "";
    trIn = trOut = 0;
    connNumb = clsConnByLim = 0;

    //- Wait connection main task stop -
    endrun = true;
    if( TSYS::eventWait( run_st, false, nodePath()+"close",5) )
	throw TError(nodePath().c_str(),_("Not closed!"));
    pthread_join( pthr_tsk, NULL );
}

void *TSocketIn::Task( void *sock_in )
{
    SSL *ssl;
    BIO	*bio = NULL, *abio = NULL;
    char err[255];
    TSocketIn &s = *(TSocketIn *)sock_in;
    AutoHD<TProtocolIn> prot_in;
    string cfile;

#if OSC_DEBUG >= 2
    mess_debug(s.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    //- Client's sockets attrs init
    pthread_t th;
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_attr_setdetachstate(&pthr_attr, PTHREAD_CREATE_DETACHED);

    //- SSL context init -
    string ssl_host = TSYS::strSepParse(s.addr(),0,':');
    string ssl_port = TSYS::strSepParse(s.addr(),1,':');
    string ssl_method = TSYS::strSepParse(s.addr(),2,':');

    //-- Set SSL method --
    SSL_METHOD *meth = (SSL_METHOD*)SSLv23_server_method();
    if( ssl_method == "SSLv2" )		meth = (SSL_METHOD*)SSLv2_server_method();
    else if( ssl_method == "SSLv3" )	meth = (SSL_METHOD*)SSLv3_server_method();
    else if( ssl_method == "TLSv1" )	meth = (SSL_METHOD*)TLSv1_server_method();

    try
    {
	s.ctx = SSL_CTX_new(meth);
	if( s.ctx == NULL )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(s.nodePath().c_str(),"SSL_CTX_new: %s",err);
	}

	//- Write certificate and private key to temorary file -
	cfile = tmpnam(err);
	int icfile = open(cfile.c_str(),O_EXCL|O_CREAT|O_WRONLY,0644);
	if( icfile < 0 ) throw TError(s.nodePath().c_str(),_("Open temporaty file '%s' error: '%s'"),cfile.c_str(),strerror(errno));
	write(icfile,s.certKey().data(),s.certKey().size());
	close(icfile);

	//-- Set private key password --
	SSL_CTX_set_default_passwd_cb_userdata(s.ctx,(char*)s.pKeyPass().c_str());
	//-- Load certificate --
	if( SSL_CTX_use_certificate_chain_file(s.ctx,cfile.c_str()) != 1 )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(s.nodePath().c_str(),"SSL_CTX_use_certificate_chain_file: %s",err);
	}
	//-- Load private key --
	if( SSL_CTX_use_PrivateKey_file(s.ctx,cfile.c_str(),SSL_FILETYPE_PEM) != 1 )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(s.nodePath().c_str(),"SSL_CTX_use_PrivateKey_file: %s",err);
	}

	//- Remove temporary certificate file -
	remove(cfile.c_str()); cfile = "";

	//- Create BIO object -
	if( (bio=BIO_new_ssl(s.ctx,0)) == NULL )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(s.nodePath().c_str(),"BIO_new_ssl: %s",err);
	}
	BIO_get_ssl(bio,&ssl);
	SSL_set_mode(ssl,SSL_MODE_AUTO_RETRY);

	abio = BIO_new_accept((char*)(ssl_host+":"+ssl_port).c_str());

	//BIO_ctrl(abio,BIO_C_SET_ACCEPT,1,(void*)"a");
	//BIO_set_nbio(abio,1);
	BIO_set_accept_bios(abio, bio);
	BIO_set_bind_mode(abio,BIO_BIND_REUSEADDR);

	//- Sets up accept BIO -
	if( BIO_do_accept(abio) <= 0 )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(s.nodePath().c_str(),"BIO_do_accept: %s",err);
	}

	s.run_st	= true;
	s.endrun	= false;
	s.endrun_cl	= false;

	//- Select mode -
	struct  timeval tv;
	fd_set  rd_fd;
	while( !s.endrun )
        {
	    tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	    FD_ZERO(&rd_fd); FD_SET(BIO_get_fd(abio,NULL),&rd_fd);

	    int kz = select(BIO_get_fd(abio,NULL)+1,&rd_fd,NULL,NULL,&tv);
	    if( kz == 0 || (kz == -1 && errno == EINTR) || kz < 0 || !FD_ISSET(BIO_get_fd(abio,NULL),&rd_fd) ) continue;

	    if( BIO_do_accept(abio) <= 0 )
	    {
	        ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	        mess_err(s.nodePath().c_str(),"BIO_do_accept: %s",err);
	        continue;
	    }

	    BIO *cbio = BIO_pop(abio);

	    if( s.maxFork() <= s.opConnCnt() )	{ s.clsConnByLim++; /*BIO_reset(cbio);*/ close(BIO_get_fd(cbio,NULL)); BIO_free(cbio); }
	    //- Make client's socket thread -
	    else if( pthread_create( &th, &pthr_attr, ClTask, new SSockIn(&s,cbio) ) < 0 )
	    {
		mess_err(s.nodePath().c_str(),_("Error creation of the thread!"));
		BIO_reset(cbio);
		BIO_free(cbio);
	    }
	    else s.connNumb++;
	}
    }catch(TError err)	{ s.stErr = err.mess; mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //- Client tasks stop command -
    s.endrun_cl = true;
    TSYS::eventWait( s.cl_free, true, string(MOD_ID)+": "+s.id()+" client tasks is stopping....");

    //- Free context -
    if( abio )	BIO_reset(abio);
    if( bio )	BIO_free_all(bio);
    if( s.ctx )	{ SSL_CTX_free(s.ctx); s.ctx = NULL; }
    if( !cfile.empty() ) remove(cfile.c_str());

    s.run_st = false;

    return NULL;
}

void *TSocketIn::ClTask( void *s_inf )
{
    SSockIn	&s = *(SSockIn *)s_inf;

    int		rez;
    char	err[255];
    char	buf[s.s->bufLen()*1024];
    string	req, answ;
    AutoHD<TProtocolIn> prot_in;
    SSL		*ssl;

    int cSock = s.s->clientReg( pthread_self() );

#if OSC_DEBUG >= 2
    mess_debug(s.s->nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif
#if OSC_DEBUG >= 3
    mess_debug(s.s->nodePath().c_str(),_("Connecting to the socket (%d)."),cSock);
#endif

    if( BIO_do_handshake(s.bio) <= 0 )
    {
	if( BIO_should_retry(s.bio) )
	    while( BIO_should_retry(s.bio) && !s.s->endrun_cl )	{ BIO_do_handshake(s.bio); usleep(STD_WAIT_DELAY*1000); }
	else
	{
	    if( ERR_peek_last_error() )
	    {
		ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
		mess_err(s.s->nodePath().c_str(),"BIO_should_retry: %s",err);
	    }
	    BIO_flush(s.bio);
	    delete (SSockIn*)s_inf;
	    pthread_exit(NULL);
	}
    }

    int sock_fd = BIO_get_fd(s.bio,NULL);

    //> Select mode
    struct  timeval tv;
    fd_set  rd_fd;
    while( !s.s->endrun_cl )
    {
	tv.tv_sec  = 0; tv.tv_usec = STD_WAIT_DELAY*1000;
	FD_ZERO(&rd_fd); FD_SET(sock_fd,&rd_fd);

	int kz = select(sock_fd+1,&rd_fd,NULL,NULL,&tv);
	if( kz == 0 || (kz == -1 && errno == EINTR) || kz < 0 || !FD_ISSET(sock_fd,&rd_fd) ) continue;

	rez = BIO_read(s.bio,buf,sizeof(buf));
	if( rez == 0 )	break;		//Connection closed by client
#if OSC_DEBUG >= 4
        mess_debug(s.s->nodePath().c_str(),_("The message is received with the size <%d>."),rez);
#endif
	req.assign(buf,rez);
	s.s->trIn += (float)rez/1024;

	s.s->messPut(cSock,req,answ,prot_in);
	if( answ.size() )
	{
#if OSC_DEBUG >= 4
            mess_debug(s.s->nodePath().c_str(),_("The message is replied with the size <%d>."),answ.size());
#endif
	    rez = BIO_write(s.bio,answ.data(),answ.size()); s.s->trOut += (float)rez/1024;
	    answ = "";
	}
    }

    BIO_flush(s.bio);
    close(sock_fd);
    //BIO_reset(s.bio);
    BIO_free(s.bio);

    //> Close protocol on broken connection
    if( !prot_in.freeStat() )
    {
	string n_pr = prot_in.at().name();
	prot_in.free();
	AutoHD<TProtocol> proto = SYS->protocol().at().modAt(s.s->protocol());
	if( proto.at().openStat(n_pr) ) proto.at().close(n_pr);
    }

    s.s->clientUnreg( pthread_self() );

#if OSC_DEBUG >= 3
    mess_debug(s.s->nodePath().c_str(),_("Socket has been disconnected (%d)."),s.s->cl_id.size() );
#endif

    delete (SSockIn*)s_inf;

    pthread_exit(NULL);
}

void TSocketIn::messPut( int sock, string &request, string &answer, AutoHD<TProtocolIn> &prot_in )
{
    AutoHD<TProtocol> proto;
    string n_pr = mod->modId()+"_"+id()+"_"+TSYS::int2str(sock);
    try
    {
	proto = SYS->protocol().at().modAt(protocol());
	if( prot_in.freeStat() )
	{
	    if( !proto.at().openStat(n_pr) ) proto.at().open( n_pr, workId() );
	    prot_in = proto.at().at( n_pr );
	}
	if( prot_in.at().mess(request,answer,"") ) return;
	prot_in.free();
	if( proto.at().openStat(n_pr) ) proto.at().close(n_pr);
    }catch(TError err)
    {
	prot_in.free();
	if( !proto.freeStat() && proto.at().openStat(n_pr) ) proto.at().close( n_pr );

	mess_err(nodePath().c_str(),"%s",err.mess.c_str() );
	mess_err(nodePath().c_str(),_("Error request to protocol."));
    }
}

int TSocketIn::opConnCnt( )
{
    ResAlloc res(sock_res,true);
    int opConn = 0;
    for( int i_c = 0; i_c < cl_id.size(); i_c++ )
	if( cl_id[i_c] ) opConn++;

    return opConn;
}

int TSocketIn::clientReg( pthread_t thrid )
{
    ResAlloc res(sock_res,true);

    int i_empt = -1;
    for( int i_id = 0; i_id < cl_id.size(); i_id++ )
	if( !cl_id[i_id] && i_empt < 0 ) i_empt = i_id;
	else if( cl_id[i_id] == thrid ) return i_id;

    if( i_empt >= 0 ) cl_id[i_empt] = thrid;
    else { i_empt = cl_id.size(); cl_id.push_back(thrid); }

    cl_free = false;

    return i_empt;
}

void TSocketIn::clientUnreg( pthread_t thrid )
{
    ResAlloc res(sock_res,true);

    bool noFreePres = false;
    for( int i_id = 0; i_id < cl_id.size(); i_id++ )
    {
	if( cl_id[i_id] == thrid ) cl_id[i_id] = 0;
	if( cl_id[i_id] && !noFreePres ) noFreePres = true;
    }

    cl_free = !noFreePres;
}

void TSocketIn::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TTransportIn::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",2,"tp","str","help",
	    _("SSL input transport has address format:\n"
	    "  [addr]:[port]:[mode] - where:\n"
	    "    addr - address for SSL to be opened, '*' address opens for all interfaces;\n"
	    "    port - network port (/etc/services);\n"
	    "    mode - SSL mode and version (SSLv2, SSLv3, SSLv23 and TLSv1)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/certKey",_("Certificates and private key"),0664,"root","root",4,"tp","str","cols","90","rows","7","help",_("SSL PAM certificates chain and private key."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/pkey_pass",_("Private key password"),0664,"root","root",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/cl_n",_("Clients maximum"),0664,"root","root",1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/prm/cfg/bf_ln",_("Input buffer (kbyte)"),0664,"root","root",1,"tp","dec");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/certKey" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(certKey());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setCertKey(opt->text());
    }
    else if( a_path == "/prm/cfg/pkey_pass" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(string(pKeyPass().size(),'*'));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setPKeyPass(opt->text());
    }
    else if( a_path == "/prm/cfg/cl_n" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(maxFork()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setMaxFork( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/bf_ln" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(bufLen()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setBufLen( atoi(opt->text().c_str()) );
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TSocketOut                                   *
//************************************************
TSocketOut::TSocketOut(string name, const string &idb, TElem *el) :
    TTransportOut(name,idb,el), mCertKey(cfg("SSLCertKey").getSd()), mKeyPass(cfg("SSLKeyPass").getSd())
{
    setAddr("localhost:10042");
}

TSocketOut::~TSocketOut()
{
    if( startStat() )	stop();
}

string TSocketOut::getStatus( )
{
    string rez = TTransportOut::getStatus( );

    if( startStat() )	rez += TSYS::strMess(_("Traffic in %.4g kb, out %.4g kb."),trIn,trOut);

    return rez;
}

void TSocketOut::start()
{
    string	cfile;
    char	err[255];
    ResAlloc res( wres, true );

    if( run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    //> SSL context init
    string ssl_host = TSYS::strSepParse(addr(),0,':');
    string ssl_port = TSYS::strSepParse(addr(),1,':');
    string ssl_method = TSYS::strSepParse(addr(),2,':');

    //> Set SSL method
    SSL_METHOD *meth = (SSL_METHOD*)SSLv23_client_method();
    if( ssl_method == "SSLv2" )		meth = (SSL_METHOD*)SSLv2_client_method();
    else if( ssl_method == "SSLv3" )	meth = (SSL_METHOD*)SSLv3_client_method();
    else if( ssl_method == "TLSv1" )	meth = (SSL_METHOD*)TLSv1_client_method();

    try
    {
	conn = NULL;
	ctx = NULL;

	ctx = SSL_CTX_new(meth);
	if( ctx == NULL )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(nodePath().c_str(),"SSL_CTX_new: %s",err);
	}

	//> Certificates, private key and it password loading
	if( !TSYS::strNoSpace(certKey()).empty() )
	{
	    //>> Write certificate and private key to temorary file
	    cfile = tmpnam(err);
	    int icfile = open(cfile.c_str(),O_EXCL|O_CREAT|O_WRONLY,0644);
	    if( icfile < 0 ) throw TError(nodePath().c_str(),_("Open temporaty file '%s' error: '%s'"),cfile.c_str(),strerror(errno));
	    write(icfile,certKey().data(),certKey().size());
	    close(icfile);

	    //>> Set private key password
	    SSL_CTX_set_default_passwd_cb_userdata(ctx,(char*)pKeyPass().c_str());
	    //>> Load certificate
	    if( SSL_CTX_use_certificate_chain_file(ctx,cfile.c_str()) != 1 )
	    {
		ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
		throw TError(nodePath().c_str(),_("SSL_CTX_use_certificate_chain_file: %s"),err);
	    }
	    //>> Load private key
	    if( SSL_CTX_use_PrivateKey_file(ctx,cfile.c_str(),SSL_FILETYPE_PEM) != 1 )
	    {
		ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
		throw TError(nodePath().c_str(),_("SSL_CTX_use_PrivateKey_file: %s"),err);
	    }

	    //>> Remove temporary certificate file
	    remove(cfile.c_str()); cfile = "";
	}

	conn = BIO_new_ssl_connect(ctx);
	if( !conn )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(nodePath().c_str(),"BIO_new_ssl_connect: %s",err);
	}
	BIO_get_ssl(conn,&ssl);
	SSL_set_mode(ssl,SSL_MODE_AUTO_RETRY);

	BIO_set_conn_hostname(conn,(char*)(ssl_host+":"+ssl_port).c_str());

	if( BIO_do_connect(conn) <= 0 )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(nodePath().c_str(),"BIO_do_connect: %s",err);
	}

	int sock_fd = BIO_get_fd(conn,NULL);
	int flags = fcntl(sock_fd,F_GETFL,0);
	fcntl(sock_fd,F_SETFL,flags|O_NONBLOCK);
    }
    catch(TError err)
    {
	if( conn )	{ BIO_reset(conn); BIO_free(conn); }
	if( ctx )	SSL_CTX_free(ctx);
	if( !cfile.empty() )	remove(cfile.c_str());
	throw;
    }

    run_st = true;
}

void TSocketOut::stop()
{
    ResAlloc res( wres, true );

    if( !run_st ) return;

    //- Status clear -
    trIn = trOut = 0;

    //SSL deinit
    BIO_flush(conn);
    BIO_reset(conn);
    BIO_free(conn);
    SSL_CTX_free(ctx);

    run_st = false;
}

int TSocketOut::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time, bool noRes )
{
    int		ret = 0;
    char	err[255];

    if( !time ) time = 5000;
    if( !noRes ) ResAlloc resN( nodeRes(), true );
    ResAlloc res( wres, true );

    if( !run_st ) throw TError(nodePath().c_str(),_("Transport is not started!"));

    //> Write request
    if( obuf != NULL && len_ob > 0 )
    {
	if( (ret=BIO_write(conn,obuf,len_ob)) != len_ob )
	{
	    if( ret == 0 )	{ res.release(); stop(); return 0; }
	    else
	    {
		ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
		throw TError(nodePath().c_str(),"BIO_write: %s",err);
	    }
	}
    }
    trOut += (float)ret/1024;
#if OSC_DEBUG >= 4
    if( ret > 0 ) mess_debug(nodePath().c_str(),_("The message is sent with the size <%d>."),ret);
#endif

    //> Read reply
    if( ibuf != NULL && len_ib > 0 )
    {
	ret=BIO_read(conn,ibuf,len_ib);
	if( ret > 0 ) trIn += (float)ret/1024;
	else if( ret == 0 ) { res.release(); stop(); return 0; }
	else if( ret < 0 && SSL_get_error(ssl,ret) != SSL_ERROR_WANT_READ )
	{
	    ERR_error_string_n(ERR_peek_last_error(),err,sizeof(err));
	    throw TError(nodePath().c_str(),"BIO_read: %s",err);
	}
	else
	{
	    //> Wait data from socket
	    int kz = 0;
	    fd_set rd_fd;
	    struct timeval tv;
	    int sock_fd = BIO_get_fd(conn,NULL);

	    do
	    {
		tv.tv_sec  = time/1000; tv.tv_usec = 1000*(time%1000);
		FD_ZERO(&rd_fd); FD_SET(sock_fd,&rd_fd);
		kz = select(sock_fd+1,&rd_fd,NULL,NULL,&tv);
	    }
	    while( kz == -1 && errno == EINTR );
	    if( kz == 0 )	{ res.release(); stop(); throw TError(nodePath().c_str(),_("Timeouted!")); }
	    else if( kz < 0)	{ res.release(); stop(); throw TError(nodePath().c_str(),_("Socket error!")); }
	    else if( FD_ISSET(sock_fd, &rd_fd) )
	    {
		ret = BIO_read(conn,ibuf,len_ib);
		if( ret < 0 )	{ res.release(); stop(); throw TError(nodePath().c_str(),_("Read reply error: %s"),strerror(errno)); }
		trIn += (float)ret/1024;
	    }
	}
    }

#if OSC_DEBUG >= 4
    if( ret > 0 ) mess_debug(nodePath().c_str(),_("The message is received with the size <%d>."),ret);
#endif

    return ret;
}

void TSocketOut::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TTransportOut::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","root",2,"tp","str","help",
	    _("SSL output transport has address format:\n"
	    "  [addr]:[port]:[mode] - where:\n"
	    "    addr - remote SSL host address;\n"
	    "    port - network port (/etc/services);\n"
	    "    mode - SSL mode and version (SSLv2, SSLv3, SSLv23 and TLSv1)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/certKey",_("Certificates and private key"),0660,"root","root",4,"tp","str","cols","90","rows","7","help",_("SSL PAM certificates chain and private key."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/pkey_pass",_("Private key password"),0660,"root","root",1,"tp","str");
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/certKey" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(certKey());
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	setCertKey(opt->text());
    }
    else if( a_path == "/prm/cfg/pkey_pass" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(string(pKeyPass().size(),'*'));
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	setPKeyPass(opt->text());
    }
    else TTransportOut::cntrCmdProc(opt);
}

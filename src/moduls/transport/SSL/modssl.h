
//OpenSCADA module Transport.SSL file: modssl.h
/***************************************************************************
 *   Copyright (C) 2008-2024 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef MODSSL_H
#define MODSSL_H

#include <pthread.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include <ttransports.h>

#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

#define S_NM_SOCKET	"SOCKET"


struct CRYPTO_dynlock_value
{
    pthread_mutex_t mutex;
};

using namespace OSCADA;

namespace MSSL
{

class TSocketIn;

//************************************************
//* Sockets::SSockIn				 *
//************************************************
class SSockIn
{
    public:
	SSockIn( TSocketIn *is, BIO *ibio, const string &isender, bool iisCon = false ) :
	    pid(0), bio(ibio), sock(0), sender(isender), tmCreate(time(NULL)), tmReq(time(NULL)), trIn(0), trOut(0), isCon(iisCon), s(is) { }

	pthread_t pid;
	BIO	*bio;
	int	sock;
	string	sender;
	time_t	tmCreate, tmReq;
	uint64_t trIn, trOut;	//Traffic in and out counters
	float	prcTm, prcTmMax;
	bool	isCon;		//Is an initiative connection

	TSocketIn	*s;
};

//************************************************
//* SSL::TSocketIn				 *
//************************************************
class TSocketIn: public TTransportIn
{
    public:
	//Data
	// Modes
	enum TRModes { M_Ordinal = 0, M_Initiative = 2 };

	//Methods
	TSocketIn( string name, const string &idb, TElem *el );
	~TSocketIn( );

	string getStatus( );

	int lastConn( )		{ return connTm; }
	unsigned mode( )	{ return mMode; }
	unsigned inBufLen( )	{ return mInBufLen; }
	unsigned MSS( )		{ return mMSS; }
	unsigned maxFork( )	{ return mMaxFork; }
	unsigned maxForkPerHost( ) { return mMaxForkPerHost; }
	unsigned keepAliveReqs( )  { return mKeepAliveReqs; }
	unsigned keepAliveTm( )	{ return mKeepAliveTm; }
	int taskPrior( )	{ return mTaskPrior; }
	string certKeyFile( )	{ return mCertKeyFile; }
	string certKey( )	{ return mCertKey; }
	string pKeyPass( )	{ return mKeyPass; }
	string initAssocPrms( )	{ return mInitAssocPrms; }

	void setInBufLen( unsigned vl )		{ mInBufLen = vl ? vmax(4,vmin(10240,vl)) : 0; modif(); }
	void setMSS( unsigned vl )		{ mMSS = vl ? vmax(100,vmin(65535,vl)) : 0; modif(); }
	void setMaxFork( unsigned vl )		{ mMaxFork = vmax(1,vmin(1000,vl)); modif(); }
	void setMaxForkPerHost( unsigned vl )	{ mMaxForkPerHost = vmin(1000,vl); modif(); }
	void setKeepAliveReqs( unsigned vl )	{ mKeepAliveReqs = vl; modif(); }
	void setKeepAliveTm( unsigned vl )	{ mKeepAliveTm = vl; modif(); }
	void setTaskPrior( int vl )		{ mTaskPrior = vmax(-1,vmin(199,vl)); modif(); }
	void setCertKeyFile( const string &val ){ mCertKeyFile = val; modif(); }
	void setCertKey( const string &val )	{ mCertKey = val; modif(); }
	void setPKeyPass( const string &val )	{ mKeyPass = val; modif(); }
	void setInitAssocPrms( const string &tms ) { mInitAssocPrms = tms; modif(); }

	void start( );
	void stop( );
	void check( unsigned int cnt );	//Some periodic tests and checkings like to the certificate file update and the initiative connection
	int writeTo( const string &sender, const string &data );

	unsigned forksPerHost( const string &sender );

    protected:
	//Methods
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( );
	void save_( );

    private:
	//Methods
	static void *Task( void * );
	static void *ClTask( void * );

	void clientReg( SSockIn *so );
	void clientUnreg( SSockIn *so );

	int prtInit( vector< AutoHD<TProtocolIn> > &prot_in, int sock, const string &sender );
	int messPut( int sock, string &request, string &answer, string sender, vector< AutoHD<TProtocolIn> > &prot_in );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	ResMtx		sockRes;
	SSL		*ssl;
	BIO		*bio, *abio;
	int		sockFd;			//For the initiative mode

	bool		endrun;			//Command for stop task
	bool		endrunCl;		//Command for stop client tasks

	unsigned short	mMode,			//Mode of SSL:
						//  0 - ordinal, 2 - initiative connection
			mInBufLen,		//Input buffer length
			mMSS,			//MSS
			mMaxFork,		//Maximum forking (opened SSL)
			mMaxForkPerHost,	//Maximum forking (opened sockets), per host
			mKeepAliveReqs,		//KeepAlive connections
			mKeepAliveTm;		//KeepAlive timeout
	int		mTaskPrior;		//Requests processing task prioritet
	string		mCertKeyFile, mCertKey,	//SSL certificate file and PEM-text
			mKeyPass,		//SSL private key password
			addon, mInitAssocPrms;

	bool		clFree;			//Clients stopped
	//vector<pthread_t>	clId;		//Client's pids
	map<int, SSockIn*> clId;		//Client's control object, by sockets FD
	map<string, int> clS;			//Clients (senders) counters

	// Status atributes
	string		connAddr;
	string		stErrMD5;		//Last error messages or certificate file MD5
	uint64_t	trIn, trOut;		//Traffic in and out counter
	float		prcTm, prcTmMax;
	int		connNumb, connTm, clsConnByLim;	//Close connections by limit
};

//************************************************
//* SSL::TSocketOut				 *
//************************************************
class TSocketOut: public TTransportOut
{
    friend class TSocketIn;

    public:
	TSocketOut( string name, const string &idb, TElem *el );
	~TSocketOut( );

	string getStatus( );

	string certKeyFile( )	{ return mCertKeyFile; }
	string certKey( )	{ return mCertKey; }
	string pKeyPass( )	{ return mKeyPass; }
	string timings( )	{ return mTimings; }
	unsigned short attempts( )	{ return mAttemts; }
	unsigned MSS( )		{ return mMSS; }

	void setCertKeyFile( const string &val ){ mCertKeyFile = val; modif(); }
	void setCertKey( const string &val )	{ mCertKey = val; modif(); }
	void setPKeyPass( const string &val )	{ mKeyPass = val; modif(); }
	void setTimings( const string &vl, bool isDef = false );
	void setAttempts( unsigned short vl );
	void setMSS( unsigned vl )	{ mMSS = vl ? vmax(100,vmin(65535,vl)) : 0; modif(); }

	static string connectSSL( const string &addr, SSL **ssl, BIO **conn,
	    int tmCon, const string &certKey, const string &pKeyPass, const string &certKeyFile );
	static void disconnectSSL( SSL **ssl, BIO **conn );

	void start( int time = 0 );
	void stop( );

	int messIO( const char *oBuf, int oLen, char *iBuf = NULL, int iLen = 0, int time = 0 );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string		mCertKeyFile, mCertKey,	//SSL certificate file and PEM-text
			mKeyPass;		//SSL private key password
	string		mTimings;
	unsigned short	mAttemts,
			mMSS,			//MSS
			mTmCon,
			mTmNext;

	SSL		*ssl;
	BIO		*conn;

	// Status atributes
	string		connAddr;
	uint64_t	trIn, trOut;		// Traffic in and out counter
	float		respTm, respTmMax;
};

//************************************************
//* SSL::TTransSock				 *
//************************************************
class TTransSock: public TTypeTransport
{
    public:
	TTransSock( string name );
	~TTransSock( );

	void perSYSCall( unsigned int cnt );

	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );

	static string addrResolve( const string &host, const string &port, vector<sockaddr_storage> &addrs, bool isServer = false );
	static string addrGet( const sockaddr_storage &addr );

	string outAddrHelp( );
	string outTimingsHelp( bool noAdd = false );
	string outAttemptsHelp( bool noAdd = false );

	string MD5( const string &file );

	//Attributes
	SSL_CTX		*ctxIn, *ctxOut;

	uint8_t		use_getaddrinfo;

    protected:
	void load_( );

    private:
	//Methods
	void preDisable( int flag );
	string optDescr( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	static unsigned long id_function( );
	static void locking_function( int mode, int n, const char * file, int line );
	static struct CRYPTO_dynlock_value *dyn_create_function( const char *file, int line );
	static void dyn_lock_function( int mode, struct CRYPTO_dynlock_value *l, const char *file, int line );
	static void dyn_destroy_function( struct CRYPTO_dynlock_value *l, const char *file, int line );

	//Attributes
	pthread_mutex_t *bufRes;
};

extern TTransSock *mod;
}

#endif //MODSSL_H

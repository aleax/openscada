
//OpenSCADA module Transport.SSL file: modssl.h
/***************************************************************************
 *   Copyright (C) 2008-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define _(mess) mod->I18N(mess)

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
	SSockIn( TSocketIn *is, BIO *ibio, const string &isender ) :
	    pid(0), bio(ibio), sock(0), sender(isender), tmCreate(time(NULL)), tmReq(time(NULL)), trIn(0), trOut(0), s(is) { }

	pthread_t pid;
	BIO	*bio;
	int	sock;
	string	sender;
	time_t	tmCreate, tmReq;
	uint64_t trIn, trOut;	//Traffic in and out counters
	float	prcTm, prcTmMax;

	TSocketIn	*s;
};

//************************************************
//* SSL::TSocketIn				 *
//************************************************
class TSocketIn: public TTransportIn
{
    public:
	TSocketIn( string name, const string &idb, TElem *el );
	~TSocketIn( );

	string getStatus( );

	int lastConn( )		{ return connTm; }
	unsigned bufLen( )	{ return mBufLen; }
	unsigned maxFork( )	{ return mMaxFork; }
	unsigned maxForkPerHost( ) { return mMaxForkPerHost; }
	unsigned keepAliveReqs( )  { return mKeepAliveReqs; }
	unsigned keepAliveTm( )	{ return mKeepAliveTm; }
	int taskPrior( )	{ return mTaskPrior; }
	string certKey( )	{ return mCertKey; }
	string pKeyPass( )	{ return mKeyPass; }

	void setBufLen( unsigned vl )		{ mBufLen = vmax(1,vmin(1024,vl)); modif(); }
	void setMaxFork( unsigned vl )		{ mMaxFork = vmax(1,vmin(1000,vl)); modif(); }
	void setMaxForkPerHost( unsigned vl )	{ mMaxForkPerHost = vmin(1000,vl); modif(); }
	void setKeepAliveReqs( unsigned vl )	{ mKeepAliveReqs = vl; modif(); }
	void setKeepAliveTm( unsigned vl )	{ mKeepAliveTm = vl; modif(); }
	void setTaskPrior( int vl )		{ mTaskPrior = vmax(-1,vmin(199,vl)); modif(); }
	void setCertKey( const string &val )	{ mCertKey = val; modif(); }
	void setPKeyPass( const string &val )	{ mKeyPass = val; modif(); }

	void start( );
	void stop( );

	unsigned forksPerHost( const string &sender );

    protected:
	//Methods
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
	SSL_CTX		*ctx;

	bool		endrun;			//Command for stop task
	bool		endrunCl;		//Command for stop client tasks

	unsigned short	mMaxFork,		//Maximum forking (opened SSL)
			mMaxForkPerHost,	//Maximum forking (opened sockets), per host
			mBufLen,		//Input buffer length
			mKeepAliveReqs,		//KeepAlive connections
			mKeepAliveTm;		//KeepAlive timeout
	int		mTaskPrior;		//Requests processing task prioritet
	string		mCertKey,		//SSL certificate
			mKeyPass;		//SSL private key password

	bool		clFree;			//Clients stopped
	//vector<pthread_t>	clId;		//Client's pids
	vector<SSockIn*> clId;			//Client's control object, by sockets FD
	map<string, int> clS;			//Clients (senders) counters

	// Status atributes
	string		stErr;			//Last error messages
	uint64_t	trIn, trOut;		//Traffic in and out counter
	float		prcTm, prcTmMax;
	int		connNumb, connTm, clsConnByLim;	//Close connections by limit
};

//************************************************
//* SSL::TSocketOut				 *
//************************************************
class TSocketOut: public TTransportOut
{
    public:
	TSocketOut( string name, const string &idb, TElem *el );
	~TSocketOut( );

	string getStatus( );

	string certKey( )	{ return mCertKey; }
	string pKeyPass( )	{ return mKeyPass; }
	string timings( )	{ return mTimings; }
	unsigned short attempts( )	{ return mAttemts; }

	void setCertKey( const string &val )	{ mCertKey = val; modif(); }
	void setPKeyPass( const string &val )	{ mKeyPass = val; modif(); }
	void setTimings( const string &vl );
	void setAttempts( unsigned short vl );

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
	string		mCertKey,		// SSL certificate
			mKeyPass;		// SSL private key password
	string		mTimings;
	unsigned short	mAttemts,
			mTmCon,
			mTmNext;

	SSL_CTX		*ctx;
	BIO		*conn;
	SSL		*ssl;

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

	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );

	string outAddrHelp( );
	string outTimingsHelp( );
	string outAttemptsHelp( );

    protected:
	void load_( );

    private:
	//Methods
	void postEnable( int flag );
	void preDisable( int flag );

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

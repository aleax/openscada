
//OpenSCADA system module Transport.Sockets file: socket.h
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

#ifndef SOCKET_H
#define SOCKET_H

#include <pthread.h>

#include <ttransports.h>

#undef _
#define _(mess) mod->I18N(mess)

#define S_NM_SOCK	"SOCK"
#define S_NM_TCP	"TCP"
#define S_NM_UDP	"UDP"
#define S_NM_UNIX	"UNIX"

#define SOCK_FORCE	-1
#define SOCK_TCP	0
#define SOCK_UDP	1
#define SOCK_UNIX	2

using namespace OSCADA;

namespace Sockets
{

class TSocketIn;

//************************************************
//* Sockets::SSockIn				 *
//************************************************
class SSockIn
{
    public:
	SSockIn( TSocketIn *is, int icSock, string isender ) :
	    s(is), cSock(icSock), sender(isender)	{ }

	TSocketIn	*s;
	int		cSock;
	string		sender;
};

//************************************************
//* Sockets::SSockCl				 *
//************************************************
struct SSockCl
{
    pthread_t	cl_id;    // Client's thread id
    int		cl_sock;
};

//************************************************
//* Sockets::TSocketIn				 *
//************************************************
class TSocketIn: public TTransportIn
{
    public:
	/* Open input socket <name> for locale <address>
	 * address : <type:<specific>>
	 * type:
	 *   TCP  - TCP socket with  "UDP:<host>:<port>"
	 *   UDP  - UDP socket with  "TCP:<host>:<port>"
	 *   UNIX - UNIX socket with "UNIX:<path>"
	 */
	TSocketIn( string name, const string &idb, TElem *el );
	~TSocketIn( );

	string getStatus( );

	int maxQueue( )		{ return mMaxQueue; }
	int maxFork( )		{ return mMaxFork; }
	int bufLen( )		{ return mBufLen; }
	int keepAliveReqs( )	{ return mKeepAliveReqs; }
	int keepAliveTm( )	{ return mKeepAliveTm; }
	int taskPrior( )	{ return mTaskPrior; }

	void setMaxQueue( int vl )	{ mMaxQueue = vmax(1,vmin(100,vl)); modif(); }
	void setMaxFork( int vl )	{ mMaxFork = vmax(1,vmin(1000,vl)); modif(); }
	void setBufLen( int vl )	{ mBufLen = vmax(1,vmin(1024,vl)); modif(); }
	void setKeepAliveReqs( int vl )	{ mKeepAliveReqs = vmax(0,vl); modif(); }
	void setKeepAliveTm( int vl )	{ mKeepAliveTm = vmax(0,vl); modif(); }
	void setTaskPrior( int vl )	{ mTaskPrior = vmax(-1,vmin(99,vl)); modif(); }

	void start( );
	void stop( );
	int writeTo( const string &sender, const string &data );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	static void *Task( void * );
	static void *ClTask( void * );

	void messPut( int sock, string &request, string &answer, string sender, AutoHD<TProtocolIn> &prot_in );

	void clientReg( pthread_t thrid, int i_sock );
	void clientUnreg( pthread_t thrid );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	int		sock_fd;
	Res		sock_res;

	bool		endrun;			// Command for stop task
	bool		endrun_cl;		// Command for stop client tasks

	int		type;			// socket's types
	string		path;			// path to file socket for UNIX socket
	string		host;			// host for TCP/UDP sockets
	string		port;			// port for TCP/UDP sockets
	int		mode;			// mode for TCP/UNIX sockets (0 - no hand; 1 - hand connect)

	int		mMaxQueue,		// max queue for TCP, UNIX sockets
			mMaxFork,		// maximum forking (opened sockets)
			mBufLen,		// input buffer length
			mKeepAliveReqs,		// KeepAlive requests
			mKeepAliveTm,		// KeepAlive timeout
			mTaskPrior;		// Requests processing task prioritet

	bool		cl_free;		// Clients stopped
	vector<SSockCl>	cl_id;			// Client's pids

	// Status atributes
	uint64_t	trIn, trOut;		// Traffic in and out counter
	int		connNumb, clsConnByLim;	// Connections number
};

//************************************************
//* Sockets::TSocketOut				 *
//************************************************
class TSocketOut: public TTransportOut
{
    public:
	/* Open output socket <name> for locale <address>
	 * address : <type:<specific>>
	 * type:
	 *   TCP  - TCP socket with  "UDP:<host>:<port>"
	 *   UDP  - UDP socket with  "TCP:<host>:<port>"
	 *   UNIX - UNIX socket with "UNIX:<path>"
	 */
	TSocketOut( string name, const string &idb, TElem *el );
	~TSocketOut( );

	string getStatus( );

	string timings( )		{ return mTimings; }
	int tmCon( )			{ return mTmCon; }

	void setAddr( const string &addr );
	void setTimings( const string &vl );
	void setTmCon( int vl )		{ mTmCon = vmax(1,vmin(60000,vl)); }

	void start( );
	void stop( );

	int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0, bool noRes = false );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string		mTimings;
	unsigned short	mTmCon;
	unsigned short	mTmNext;
	unsigned short	mTmRep;

	int		sock_fd;

	int		type;			// socket's types
	struct sockaddr_in	name_in;
	struct sockaddr_un	name_un;

	// Status atributes
	uint64_t	trIn, trOut;			// Traffic in and out counter
	Res		wres;
	int64_t		mLstReqTm;
};

//************************************************
//* Sockets::TTransSock				 *
//************************************************
class TTransSock: public TTipTransport
{
    public:
	TTransSock( string name );
	~TTransSock( );

	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );

    protected:
	void load_( );

    private:
	//Methods
	void postEnable( int flag );
};

extern TTransSock *mod;
}

#endif //SOCKET_H

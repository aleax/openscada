
//OPC_UA implementation test: test.h
/******************************************************************************
 *   Copyright (C) 2013-2014 by Roman Savochenko, <rom_as@oscada.org>         *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include "libOPC_UA.h"

using namespace OPC;

/************************************************
 * TestClient - inherit OPC UA library's client *
 ************************************************/
class TestClient: public Client
{
    public:
	// Transport connection implementation and main data init
	TestClient( const string &ep, const string &aData = "" ); //As the constructor's argument's used target server's EndPoint address
	~TestClient( );

	void start( );
	void stop( );

	// OPC UA client's session name - the client programm name
	string	sessionName( )	{ return "OpenSCADA OPC_UA library's test station."; }
	// Used target server's EndPoint
	string	endPoint( )	{ return mEp; }
	// Used policy for the secure channel, allowed: None, Basic128Rsa15, Basic256
	string	secPolicy( )	{ return mSecPol; }
	// Used messages mode for the secure channel, allowed: MS_None, MS_Sign, MS_SignAndEncrypt
	int	secMessMode( )	{ return mSecMessMode; }

	//Auth data:	Empty - anonymous
	//		{User}\n{Password} - by user and password
	string authData( )	{ return mAuthData; }

	// Client's selfsigned certificate and the private key
	string	cert( )		{ return mCert; }
	string	pvKey( )	{ return mPvKey; }

	// Connection state by TCP connection state
	bool	connected( )	{ return (sock_fd >= 0); }

	// Write to the transport and read respond
	int	messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0 );

    private:
	string	mEp,
		mURI,		//Parsed from the EndPoint server URI
		mAddr,		//Parsed from URI server's host address and TCP-port number
		mSecPol, mCert, mPvKey,
		mAuthData;
	int	mSecMessMode;
	int	sock_fd;
};

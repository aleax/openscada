
//OpenSCADA system file: resalloc.h
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

#ifndef RESALLOC_H
#define RESALLOC_H

#include <features.h>
#include <pthread.h>

#include <string>

using std::string;

namespace OSCADA
{

//***********************************************************
//* RW Resources allocation object			    *
//***********************************************************
class ResRW
{
    public:
	ResRW( );
	~ResRW( );

	void lock( bool toWr, unsigned short tm = 0 ) { if(toWr) resRequestW(tm); else resRequestR(tm); }
	bool tryLock( bool toWr )	{ return toWr ? resTryW() : resTryR(); }
	void unlock( )			{ resRelease(); }

	void resRequestW( unsigned short tm = 0 );	// Write request, tm in milliseconds
	bool resTryW( );
	void resRequestR( unsigned short tm = 0 );	// Read request, tm in milliseconds
	bool resTryR( );
	void resRelease( );				// Release

    private:
	pthread_rwlock_t	rwc;
#if !__GLIBC_PREREQ(2,4)
	pthread_t		wThr;
#endif
};


//***********************************************************
//* Automatic resource RW unlock object			    *
//***********************************************************
class ResAlloc
{
    public:
	//Methods
	ResAlloc( ResRW &rid );
	ResAlloc( ResRW &rid, bool write, unsigned short tm = 0 );
	~ResAlloc( );

	void request( bool write = false, unsigned short tm = 0 );
	void lock( bool write = false, unsigned short tm = 0 )	{ request(write, tm); }
	void release( );
	void unlock( )	{ release(); }

    private:
	//Attributes
	ResRW	&mId;
	bool	mAlloc;
};

//***********************************************************
//* Resources allocation object, by mutex		    *
//***********************************************************
class ResMtx
{
    public:
	ResMtx( bool isRecurs = false ) {
	    pthread_mutexattr_t attrM;
	    pthread_mutexattr_init(&attrM);
	    if(isRecurs) pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
	    pthread_mutex_init(&m, &attrM);
	    pthread_mutexattr_destroy(&attrM);
	}
	~ResMtx( ) {
	    pthread_mutex_destroy(&m);
	}

	int lock( )	{ return pthread_mutex_lock(&m); }
	int tryLock( )	{ return pthread_mutex_trylock(&m); }
	int unlock( )	{ return pthread_mutex_unlock(&m); }

	pthread_mutex_t &mtx( )	{ return m; }

    private:
	//Attributes
	pthread_mutex_t	m;
};

//********************************************
//* String+resource RW lock for		     *
//********************************************
class ResString
{
    public:
	//Methods
	explicit ResString( const string &vl = "" );
	~ResString( );

	ResString &operator=( const string &val );
	operator string( )		{ return getVal(); }

	size_t size( );
	bool   empty( );

	void setVal( const string &vl );
	string getVal( );
	const string &getValRef( )	{ return str; }

    private:
	//Attributes
	ResMtx	mRes;
	string	str;
};

//***********************************************************
//* Conditional variable object, by mutex		    *
//***********************************************************
class CondVar
{
    public:
	CondVar( );
	~CondVar( );

	int wait( ResMtx &mtx, unsigned short tm = 0 );
	int wakeOne( );
	int wakeAll( );

    private:
	pthread_cond_t	cnd;
};

//***********************************************************
//* Automatic POSIX mutex unlock object			    *
//***********************************************************
class MtxAlloc
{
    public:
	//Methods
	MtxAlloc( ResMtx &iM, bool lock = false );
	~MtxAlloc( );

	int lock( );
	int tryLock( );
	int unlock( );

    private:
	//Attributes
	ResMtx	&m;
	bool	mLock;
};

//********************************************
//* String + reference mutex lock for	     *
//********************************************
class MtxString
{
    public:
	//Methods
	MtxString( ResMtx &iM );
	~MtxString( );

	MtxString &operator=( const string &val );
	operator string( )	{ return getVal(); }

	size_t size( );
	bool   empty( );

	void setVal( const string &vl );
	string getVal( );
	const string &getValRef( )	{ return str; }

    private:
	//Attributes
	ResMtx	&m;
	string	str;
};

}

#endif // RESALLOC_H

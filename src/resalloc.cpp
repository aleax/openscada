
//OpenSCADA system file: resalloc.cpp
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

#include "errno.h"

#include "tsys.h"
#include "resalloc.h"

using namespace OSCADA;

//********************************************
//* Resource object                          *
//********************************************
Res::Res( )
{
#if !__GLIBC_PREREQ(2,4)
    wThr = 0;
#endif
    if(pthread_rwlock_init(&rwc,NULL)) throw TError("ResAlloc", _("Error open semaphore!"));
}

Res::~Res( )
{
    pthread_rwlock_wrlock(&rwc);
    pthread_rwlock_destroy(&rwc);
}

void Res::resRequestW( unsigned short tm )
{
    int rez = 0;
#if !__GLIBC_PREREQ(2,4)
    //EDEADLK imitation
    if(wThr && wThr == pthread_self()) rez == EDEADLK;
    else
#endif
    if(!tm) rez = pthread_rwlock_wrlock(&rwc);
    else
    {
	timespec wtm;
	clock_gettime(CLOCK_REALTIME,&wtm);
	wtm.tv_nsec += 1000000*(tm%1000);
	wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
	rez = pthread_rwlock_timedwrlock(&rwc,&wtm);
    }
    if(rez == EDEADLK) throw TError(10, "ResAlloc", _("Resource is try deadlock a thread!"));
    else if(tm && rez == ETIMEDOUT) throw TError("ResAlloc", _("Resource is timeouted!"));
#if !__GLIBC_PREREQ(2,4)
    wThr = pthread_self();
#endif
}

bool Res::resTryW( )
{
    int rez = pthread_rwlock_trywrlock(&rwc);
    if(rez == EBUSY) return false;
    else if(rez == EDEADLK) throw TError(10, "ResAlloc", _("Resource is try deadlock a thread!"));
    return true;
}

void Res::resRequestR( unsigned short tm )
{
    int rez = 0;
#if !__GLIBC_PREREQ(2,4)
    //EDEADLK imitation
    if(wThr && wThr == pthread_self()) rez == EDEADLK;
    else
#endif
    if(!tm) rez = pthread_rwlock_rdlock(&rwc);
    else
    {
	timespec wtm;
	clock_gettime(CLOCK_REALTIME,&wtm);
	wtm.tv_nsec += 1000000*(tm%1000);
	wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
	rez = pthread_rwlock_timedrdlock(&rwc,&wtm);
    }
    if(rez == EDEADLK) throw TError(10,"ResAlloc",_("Resource is try deadlock a thread!"));
    else if(tm && rez == ETIMEDOUT) throw TError("ResAlloc",_("Resource is timeouted!"));
}

bool Res::resTryR( )
{
    int rez = pthread_rwlock_tryrdlock(&rwc);
    if(rez == EBUSY) return false;
    else if(rez == EDEADLK) throw TError(10,"ResAlloc",_("Resource is try deadlock a thread!"));
    return true;
}

void Res::resRelease( )
{
    pthread_rwlock_unlock(&rwc);
#if !__GLIBC_PREREQ(2,4)
    if(wThr == pthread_self()) wThr = 0;
#endif
}

//********************************************
//* Automatic resource allocator/deallocator *
//********************************************
ResAlloc::ResAlloc( Res &rid ) : mId(rid), mAlloc(false)	{ }

ResAlloc::ResAlloc( Res &rid, bool write, unsigned short tm ) : mId(rid), mAlloc(false)	{ request(write, tm); }

ResAlloc::~ResAlloc( )	{ if(mAlloc) release(); }

void ResAlloc::request( bool write, unsigned short tm )
{
    if(mAlloc) release();
    mAlloc = false;
    try
    {
	if(write) mId.resRequestW(tm);
	else mId.resRequestR(tm);
	mAlloc = true;
    }catch(TError err) { if(err.cod!=10) throw; }
}

void ResAlloc::release()
{
    if(!mAlloc) return;
    mId.resRelease();
    mAlloc = false;
}

//********************************************
//* String+resource for                      *
//********************************************
ResString::ResString( const string &vl )
{
    pthread_mutex_init(&mRes, NULL);
    setVal(vl);
}

ResString::~ResString( )
{
    pthread_mutex_lock(&mRes);
    pthread_mutex_destroy(&mRes);
}

size_t ResString::size( )	{ return getVal().size(); }

bool   ResString::empty( )	{ return getVal().empty(); }

void ResString::setVal( const string &vl )
{
    pthread_mutex_lock(&mRes);
    str.assign(vl.data(), vl.size());	//Bypass for COW algorithm prevent
    pthread_mutex_unlock(&mRes);
}

string ResString::getVal( )
{
    pthread_mutex_lock(&mRes);
    string rez(str.data(), str.size());	//Bypass for COW algorithm prevent
    pthread_mutex_unlock(&mRes);
    return rez;
}

ResString &ResString::operator=( const string &val )	{ setVal(val); return *this; }

//***********************************************************
//* Automatic POSIX mutex allocator/deallocator		    *
//***********************************************************
MtxAlloc::MtxAlloc( pthread_mutex_t &iM, bool iLock ) : m(iM),  mLock(false){ if(iLock) lock(); }

MtxAlloc::~MtxAlloc( )	{ unlock(); }

int MtxAlloc::lock( )
{
    if(mLock) return 0;
    int rez = pthread_mutex_lock(&m);
    if(!rez) mLock = true;

    return rez;
}

int MtxAlloc::tryLock( )
{
    if(mLock) return 0;
    int rez = pthread_mutex_trylock(&m);
    if(!rez) mLock = true;

    return rez;
}

int MtxAlloc::unlock( )
{
    if(!mLock) return 0;
    int rez = pthread_mutex_unlock(&m);
    if(!rez) mLock = false;

    return rez;
}

//********************************************
//* String + reference mutex lock for        *
//********************************************
MtxString::MtxString( pthread_mutex_t &iM ) : m(iM)	{ }

MtxString::~MtxString( )	{ }

size_t MtxString::size( )	{ return getVal().size(); }

bool   MtxString::empty( )	{ return getVal().empty(); }

void MtxString::setVal( const string &vl )
{
    pthread_mutex_lock(&m);
    str.assign(vl.data(), vl.size());	//Bypass for COW algorithm prevent
    pthread_mutex_unlock(&m);
}

string MtxString::getVal( )
{
    pthread_mutex_lock(&m);
    string rez(str.data(), str.size());	//Bypass for COW algorithm prevent
    pthread_mutex_unlock(&m);
    return rez;
}

MtxString &MtxString::operator=( const string &val )	{ setVal(val); return *this; }

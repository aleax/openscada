
//OpenSCADA system file: resalloc.cpp
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

#include "errno.h"

#include "tsys.h"
#include "resalloc.h"

using namespace OSCADA;

//********************************************
//* RW Resources allocation object           *
//********************************************
ResRW::ResRW( )
{
#if !__GLIBC_PREREQ(2,4)
    wThr = 0;
#endif
    if(pthread_rwlock_init(&rwc,NULL)) throw TError("ResRW", _("Error open semaphore!"));
}

ResRW::~ResRW( )
{
    pthread_rwlock_wrlock(&rwc);
    pthread_rwlock_destroy(&rwc);
}

void ResRW::resRequestW( unsigned short tm )
{
    int rez = 0;
#if !__GLIBC_PREREQ(2,4)
    //EDEADLK imitation
    if(wThr && wThr == pthread_self()) rez == EDEADLK;
    else
#endif
    if(!tm) rez = pthread_rwlock_wrlock(&rwc);
    else {
	timespec wtm;
	//clock_gettime(SYS->clockRT()?CLOCK_REALTIME:CLOCK_MONOTONIC, &wtm);
	clock_gettime(CLOCK_REALTIME, &wtm);	//But for pthread_rwlock the clock source changing unallowed!
	wtm.tv_nsec += 1000000*(tm%1000);
	wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
	rez = pthread_rwlock_timedwrlock(&rwc, &wtm);
    }
    if(rez == EDEADLK) throw TError(10, "ResRW", _("Resource is try deadlock a thread!"));
    else if(tm && rez == ETIMEDOUT) throw TError("ResRW", _("Resource is timeouted!"));
#if !__GLIBC_PREREQ(2,4)
    wThr = pthread_self();
#endif
}

bool ResRW::resTryW( )
{
    int rez = pthread_rwlock_trywrlock(&rwc);
    if(rez == EBUSY) return false;
    else if(rez == EDEADLK) throw TError(10, "ResRW", _("Resource is try deadlock a thread!"));
    return true;
}

void ResRW::resRequestR( unsigned short tm )
{
    int rez = 0;
#if !__GLIBC_PREREQ(2,4)
    //EDEADLK imitation
    if(wThr && wThr == pthread_self()) rez == EDEADLK;
    else
#endif
    if(!tm) rez = pthread_rwlock_rdlock(&rwc);
    else {
	timespec wtm;
	//clock_gettime(SYS->clockRT()?CLOCK_REALTIME:CLOCK_MONOTONIC, &wtm);
	clock_gettime(CLOCK_REALTIME, &wtm);	//But for pthread_rwlock the clock source changing unallowed!
	wtm.tv_nsec += 1000000*(tm%1000);
	wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
	rez = pthread_rwlock_timedrdlock(&rwc,&wtm);
    }
    if(rez == EDEADLK) throw TError(10,"ResRW",_("Resource is try deadlock a thread!"));
    else if(tm && rez == ETIMEDOUT) throw TError("ResRW",_("Resource is timeouted!"));
}

bool ResRW::resTryR( )
{
    int rez = pthread_rwlock_tryrdlock(&rwc);
    if(rez == EBUSY) return false;
    else if(rez == EDEADLK) throw TError(10,"ResRW",_("Resource is try deadlock a thread!"));
    return true;
}

void ResRW::resRelease( )
{
    pthread_rwlock_unlock(&rwc);
#if !__GLIBC_PREREQ(2,4)
    if(wThr == pthread_self()) wThr = 0;
#endif
}

//********************************************
//* Automatic resource allocator/deallocator *
//********************************************
ResAlloc::ResAlloc( ResRW &rid ) : mId(rid), mAlloc(false)	{ }

ResAlloc::ResAlloc( ResRW &rid, bool write, unsigned short tm ) : mId(rid), mAlloc(false)	{ request(write, tm); }

ResAlloc::~ResAlloc( )	{ if(mAlloc) release(); }

void ResAlloc::request( bool write, unsigned short tm )
{
    if(mAlloc) release();
    mAlloc = false;
    try {
	if(write) mId.resRequestW(tm);
	else mId.resRequestR(tm);
	mAlloc = true;
    } catch(TError &err) { if(err.cod!=10) throw; }
}

void ResAlloc::release( )
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
    setVal(vl);
}

ResString::~ResString( )	{ }

size_t ResString::size( )	{ return getVal().size(); }

bool ResString::empty( )	{ return getVal().empty(); }

void ResString::setVal( const string &vl )
{
    mRes.lock();
    str.assign(vl.data(), vl.size());	//Bypass for COW algorithm prevent
    mRes.unlock();
}

string ResString::getVal( )
{
    mRes.lock();
    string rez(str.data(), str.size());	//Bypass for COW algorithm prevent
    mRes.unlock();
    return rez;
}

ResString &ResString::operator=( const string &val )	{ setVal(val); return *this; }

//***********************************************************
//* Conditional variable object, by mutex		    *
//***********************************************************
CondVar::CondVar( )
{
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, (SYS && SYS->clockRT()) ? CLOCK_REALTIME : CLOCK_MONOTONIC);
    pthread_cond_init(&cnd, &attr);
}

CondVar::~CondVar( )
{
    pthread_cond_destroy(&cnd);
}

int CondVar::wait( ResMtx &mtx, unsigned short tm )
{
    if(tm) {
	timespec wtm;
	clock_gettime(SYS->clockRT()?CLOCK_REALTIME:CLOCK_MONOTONIC, &wtm);
	wtm.tv_nsec += 1000000*(tm%1000);
	wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
	return pthread_cond_timedwait(&cnd, &mtx.mtx(), &wtm);
    }
    return pthread_cond_wait(&cnd, &mtx.mtx());
}

int CondVar::wakeOne( )	{ return pthread_cond_signal(&cnd); }

int CondVar::wakeAll( )	{ return pthread_cond_broadcast(&cnd); }

//***********************************************************
//* Automatic POSIX mutex allocator/deallocator		    *
//***********************************************************
MtxAlloc::MtxAlloc( ResMtx &iM, bool iLock ) : m(iM), mLock(false) { if(iLock) lock(); }

MtxAlloc::~MtxAlloc( )	{ unlock(); }

int MtxAlloc::lock( )
{
    if(mLock) return 0;
    int rez = m.lock();
    if(!rez) mLock = true;

    return rez;
}

int MtxAlloc::tryLock( )
{
    if(mLock) return 0;
    int rez = m.tryLock();
    if(!rez) mLock = true;

    return rez;
}

int MtxAlloc::unlock( )
{
    if(!mLock) return 0;
    int rez = m.unlock();
    if(!rez) mLock = false;

    return rez;
}

//********************************************
//* String + reference mutex lock for        *
//********************************************
MtxString::MtxString( ResMtx &iM ) : m(iM)	{ }

MtxString::~MtxString( )	{ }

size_t MtxString::size( )	{ return getVal().size(); }

bool   MtxString::empty( )	{ return getVal().empty(); }

void MtxString::setVal( const string &vl )
{
    m.lock();
    str.assign(vl.data(), vl.size());	//Bypass for COW algorithm prevent
    m.unlock();
}

string MtxString::getVal( )
{
    m.lock();
    string rez(str.data(), str.size());	//Bypass for COW algorithm prevent
    m.unlock();
    return rez;
}

MtxString &MtxString::operator=( const string &val )	{ setVal(val); return *this; }

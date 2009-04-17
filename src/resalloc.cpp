
//OpenSCADA system file: resalloc.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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

#include "tsys.h"
#include "resalloc.h"

//********************************************
//* Resource object                          *
//********************************************
Res::Res( )
{
    if( pthread_rwlock_init(&rwc,NULL) )
	throw TError("ResAlloc",_("Error open semaphor!"));
}

Res::~Res( )
{
    pthread_rwlock_wrlock(&rwc);
    pthread_rwlock_destroy(&rwc);
}

void Res::resRequestW( long tm )
{
    if( !tm ) pthread_rwlock_wrlock(&rwc);
    else
    {
	timespec wtm;
	clock_gettime(CLOCK_REALTIME,&wtm);
	wtm.tv_sec += tm;
	if( pthread_rwlock_timedwrlock(CLOCK_REALTIME,&wtm) )
	    throw TError("ResAlloc",_("Timeouted!"));
    }
}

void Res::resRequestR( long tm )
{
    if( !tm ) pthread_rwlock_rdlock(&rwc);
    else
    {
	timespec wtm;
	clock_gettime(CLOCK_REALTIME,&wtm);
	wtm.tv_sec += tm;
	if( pthread_rwlock_timedrdlock(CLOCK_REALTIME,&wtm) )
	    throw TError("ResAlloc",_("Timeouted!"));
    }
}

void Res::resRelease( )
{
    pthread_rwlock_unlock(&rwc);
}

//********************************************
//* Automatic resource allocator/deallocator *
//********************************************
ResAlloc::ResAlloc( Res &rid ) : mId(rid), mAlloc(false)
{

}

ResAlloc::ResAlloc( Res &rid, bool write, long tm ) : mId(rid), mAlloc(false)
{
    request( write, tm );
}

ResAlloc::~ResAlloc( )
{
    if( mAlloc ) release();
}

void ResAlloc::request( bool write, long tm )
{
    if( mAlloc ) release();
    mAlloc = false;
    if( write ) mId.resRequestW(tm);
    else mId.resRequestR(tm);
    mAlloc = true;
}

void ResAlloc::release()
{
    if( !mAlloc ) return;
    mId.resRelease( );
    mAlloc = false;
}

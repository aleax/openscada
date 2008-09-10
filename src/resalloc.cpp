
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
Res::Res( unsigned val ) : rd_c(0)
{
    if( sem_init(&sem,0,val) || sem_init(&sem_rc,0,1) )
	throw TError("ResAlloc",_("Error open semaphor!"));
}

Res::~Res( )
{
    sem_wait( &sem );
    while( rd_c ) usleep(STD_WAIT_DELAY*1000);
    sem_destroy( &sem );
    sem_destroy( &sem_rc );
}

void Res::resRequestW( long tm )
{
    if( !tm ) sem_wait( &sem );
    else
    {
	timespec wtm = { tm, 0 };
	if( sem_timedwait( &sem, &wtm ) ) throw TError("ResAlloc",_("Timeouted!"));
    }
    //- Wait of readers free -
    if( rd_c )
    {
	time_t st_tm = time(NULL);
	while( rd_c )
	{
	    if( tm && st_tm+tm > time(NULL) ) throw TError("ResAlloc",_("Timeouted!"));
	    usleep(STD_WAIT_DELAY*1000);
	}
    }
}

void Res::resReleaseW( )
{
    sem_post( &sem );
}

void Res::resRequestR( long tm )
{
    if( !tm ) sem_wait( &sem );
    else
    {
	timespec wtm = { tm, 0 };
	if( sem_timedwait( &sem, &wtm ) ) throw TError("ResAlloc",_("Timeouted!"));
    }
    sem_wait( &sem_rc );
    rd_c++;
    sem_post( &sem_rc );

    sem_post( &sem );
}

void Res::resReleaseR( )
{
    sem_wait( &sem_rc );
    if( rd_c > 0 ) rd_c--;
    sem_post( &sem_rc );
}

//********************************************
//* Automatic resource allocator/deallocator *
//********************************************
ResAlloc::ResAlloc( Res &rid ) : m_id(rid), m_wr(0)
{

}

ResAlloc::ResAlloc( Res &rid, bool write, long tm ) : m_id(rid), m_wr(0)
{
    request( write, tm );
}

ResAlloc::~ResAlloc( )
{
    if( m_wr&0x01 ) release();
}

void ResAlloc::request( bool write, long tm )
{
    if( m_wr&0x01 ) release();
    m_wr |= 0x01;
    if( write ) 
    {
	m_wr |= 0x02;
	m_id.resRequestW(tm);
    }
    else m_id.resRequestR(tm);
}

void ResAlloc::release()
{
    if( !(m_wr&0x01) )	return;
    if( m_wr&0x02 ) m_id.resReleaseW( );
    else            m_id.resReleaseR( );
    m_wr &= ~0x03;
}

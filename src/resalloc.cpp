
//OpenSCADA system file: resalloc.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

vector<ResAlloc::SSem> ResAlloc::sems;

ResAlloc::ResAlloc( unsigned id ) : m_id(id), m_wr(0)
{

}

ResAlloc::ResAlloc( unsigned id, bool write, long tm ) : m_id(id), m_wr(0) 
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
	resRequestW(m_id, tm);
    }
    else resRequestR(m_id, tm);	
}

    
void ResAlloc::release()
{
    if( !(m_wr&0x01) )	return;
    if( m_wr&0x02 ) resReleaseW(m_id);
    else            resReleaseR(m_id);	
    m_wr &= ~0x03;
}

unsigned ResAlloc::resCreate( unsigned val )
{
    unsigned i_sem;
    
    for(i_sem = 0; i_sem < sems.size(); i_sem++)
	if( !sems[i_sem].use ) break;
    if( i_sem == sems.size() ) sems.push_back( ResAlloc::SSem() );
    if( sem_init(&sems[i_sem].sem,0,val) != 0 )	throw TError("ResAlloc",_("Error open semaphor!"));
    if( sem_init(&sems[i_sem].sem_rc,0,1) != 0 )throw TError("ResAlloc",_("Error open read semaphor!"));
    sems[i_sem].use = true;   
    sems[i_sem].del = false;   
    sems[i_sem].rd_c = 0;   

    return(i_sem);
}

void ResAlloc::resDelete( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("ResAlloc",_("Error delete semaphor %d!"), res);
    
    sems[res].del = true;
    sem_wait( &sems[res].sem );
    while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
    sem_destroy( &sems[res].sem );
    sem_destroy( &sems[res].sem_rc );
    sems[res].use = false;   
}

void ResAlloc::resRequestW( unsigned res, long tm )
{
    time_t st_tm;
    if( res >= sems.size() || sems[res].del || !sems[res].use )
	throw TError("ResAlloc",_("Error 'write' request semaphor %d!"), res);
    
    if( !tm ) sem_wait( &sems[res].sem );    
    else
    {
	timespec wtm = { tm, 0 };
	if( sem_timedwait( &sems[res].sem, &wtm ) ) throw TError("ResAlloc",_("Timeouted!"));
	
	/*st_tm = time(NULL);
	while( sem_trywait( &sems[res].sem ) )
	{
	    if( tm && st_tm+tm > time(NULL) ) throw TError("ResAlloc",_("Timeouted!"));
	    usleep(STD_WAIT_DELAY*1000);    
	}*/
    }
    //- Wait of readers free -
    st_tm = time(NULL);
    while( sems[res].rd_c )
    { 
	if( tm && st_tm+tm > time(NULL) ) throw TError("ResAlloc",_("Timeouted!"));
	usleep(STD_WAIT_DELAY*1000);
    }
    
    //sem_wait( &sems[res].sem );
    //while( sems[res].rd_c ) usleep(STD_WAIT_DELAY*1000);
}

void ResAlloc::resReleaseW( unsigned res )
{
    if(res >= sems.size() || !sems[res].use )
	throw TError("ResAlloc",_("Error 'write' release semaphor %d!"), res);
    sem_post( &sems[res].sem );
}

void ResAlloc::resRequestR( unsigned res, long tm )
{
    if( res >= sems.size() || !sems[res].use || sems[res].del )
	throw TError("ResAlloc",_("Error 'read' request semaphor %d!"), res);
    
    if( !tm ) sem_wait( &sems[res].sem );
    else
    {
	timespec wtm = { tm, 0 };
	if( sem_timedwait( &sems[res].sem, &wtm ) ) throw TError("ResAlloc",_("Timeouted!"));    
    
	/*time_t st_tm = time(NULL);
	while( sem_trywait( &sems[res].sem ) )
	{
    	    if( st_tm+tm > time(NULL) ) throw TError("ResAlloc",_("Timeouted!"));
    	    usleep(STD_WAIT_DELAY*1000);
	}*/	
    }
    sem_wait( &sems[res].sem_rc );
    sems[res].rd_c++;   
    sem_post( &sems[res].sem_rc );
    
    sem_post( &sems[res].sem );
}

void ResAlloc::resReleaseR( unsigned res )
{
    if( res >= sems.size() || !sems[res].use )
	throw TError("ResAlloc",_("Error 'read' release semaphor %d!"), res);
    sem_wait( &sems[res].sem_rc );
    if( sems[res].rd_c > 0 ) sems[res].rd_c--;   
    sem_post( &sems[res].sem_rc );
}

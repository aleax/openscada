/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#ifndef RESALLOC_H
#define RESALLOC_H

#include <semaphore.h>

#include <vector>

using std::vector;

struct SSem
{
    bool  use;          // using flag
    bool  del;          // deleting flag    
    sem_t sem;          // semafor id 
    int   rd_c;         // readers counter
};

class ResAlloc 
{
    public: 
	ResAlloc( unsigned id );
	ResAlloc( unsigned id, bool write );
	~ResAlloc( );

	void request( bool write = false, long tm = 0 );
	void release();
	
	// Static metods
	static unsigned resCreate( unsigned val = 1 );
	static void resDelete( unsigned res );
    
	static void resRequestW( unsigned res, long tm = 0 ); // Write request
        static void resReleaseW( unsigned res );              // Write release
	static void resRequestR( unsigned res, long tm = 0 ); // Read request
	static void resReleaseR( unsigned res );              // Read release
	
    private:
	int   m_id;     //
	char  m_wr;     //0x01 - alloc; 0x02 - write
	
	static vector<SSem>  sems;
};

#endif // RESALLOC_H

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

#ifndef THD_H
#define THD_H

#include <string>
#include <vector>

using std::string;
using std::vector;

//======================================================================================
//====================== THD ===========================================================
//======================================================================================
struct SHD_hd
{
    unsigned use;
    unsigned hd;
    string   user;
};

struct SHD_obj
{
    void     *obj;
    string   *name;
    bool     del;     //If object deleted (no attached)
};

class THD
{
    public:
	THD( const char *obj_n );
	~THD( );

	// Objects avoid counter
        unsigned obj_cnt( );
	// Internal free object stat
        bool &obj_free(){ return(m_free); }
	// Avoid object list	
        void obj_list( vector<string> &list );
	// Add object	
	void obj_add( void *obj, string *name, int pos = -1 );
	// Delete object	
        void *obj_del( string &name, long tm = 0);	
	// Rotate object (rotate position objects )	
        void obj_rotate( string &name1, string &name2 );	
	// Use object counter.
	unsigned obj_use( string &name );
	unsigned obj_use( unsigned i_hd );
	// Get object. Dangerous no resources!!!!!
	void *obj( string &name );

	// Attach to object and make hd for access it
        unsigned hd_att( string &name, string user = "" );
	// Detach from object 
	void hd_det( unsigned i_hd );
	// Get attached object
        void *hd_at( unsigned i_hd );	
	
	// Lock for attach and add object
	void lock() { m_lock = true; }
	// Use external resource (header)
	int  res( int id_res );
    private:
	vector<SHD_hd>  m_hd;
	vector<SHD_obj> m_obj;

	int hd_res;
        bool res_ext;       //External resource used
	bool m_lock;        //Locked hd
	bool m_free;        //No object avoid 

	const char *u_name;     //Object name
	
	static const char *o_name; 
};

/*
class TResHD
{
    public:
	TResHD( THD &hd ) : m_hd(hd){ }
	~TResHD( );

    private:
	THD &m_hd;
};
*/
	
#endif // THD_H

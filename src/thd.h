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

#include "terror.h"

using std::string;
using std::vector;

//======================================================================================
//====================== THD ===========================================================
//======================================================================================

class THD
{
    public:
	struct Shd
	{
	    unsigned use;
	    unsigned hd;
	    string   user;
    	};    
	
	struct Sobj
	{
	    void     *obj;
	    string   *name;
	    bool     del;     //If object deleted (no attached)
	};	
    
	THD( const char *obj_n );
	~THD( );

        unsigned objSize( );			// Objects avoid counter
        void objList( vector<string> &list );	// Avoid object list
	bool objAvoid( const string &name );	// Check avoid object
	void objAdd( void *obj, string *name, int pos = -1 );	// Add object
        void *objDel( const string &name, long tm = 0);	// Delete object
        void objRotate( const string &name1, const string &name2 );	// Rotate object (rotate position objects )
	
        unsigned hdAtt( const string &name, const string &user = "" );	// Attach to object and make hd for access it
        void hdDet( unsigned i_hd );	// Detach from object
        void *hdAt( unsigned i_hd );	// Get attached object						
    	
	// Use object counter.
	unsigned objUse( const string &name );
	unsigned objUse( unsigned i_hd );
	
	// Get object. Dangerous no resources!!!!!
	void *obj( const string &name );
	
	bool &objFreeStat(){ return(m_free); }  // Internal free object state	 

	void lock() { m_lock = true; }	// Lock for attach and add object
	int  res( int id_res );		// Use external resource (header)
	
	Sobj obj( unsigned i_hd );	// Hd obj
	Shd hd( unsigned i_hd );	// Hd info
	
    private:
	vector<Shd>  m_hd;
	vector<Sobj> m_obj;

	int hd_res;
        bool res_ext;       //External resource used
	bool m_lock;        //Locked hd
	bool m_free;        //No object avoid 

	const char *u_name;     //Object name
	
	static const char *o_name; 
};

/****************************************************************
 * AutoHD - for auto released HD resources			*
 ****************************************************************/
template <class ORes> class AutoHD
{
    public:
        AutoHD( ): m_hd(NULL), m_id(-1)	{  }
	AutoHD( const string &name, THD &hd, const string &who = "" ): m_hd(&hd), m_id(-1)	
	{ 
	    m_id = m_hd->hdAtt(name, who); 
	    m_val = m_hd->hdAt(m_id);
	}	
	//Copying constructor
	AutoHD( const AutoHD &hd ): m_hd(NULL) { operator=(hd); }	
	template <class ORes1> AutoHD( const AutoHD<ORes1> &hd_s )
	{  
	    m_hd = hd_s.hd();
	    if( m_hd )
	    {
	        m_id = m_hd->hdAtt(*m_hd->obj(hd_s.id()).name, m_hd->hd(hd_s.id()).user );
		m_val = m_hd->hdAt(m_id);
	    }
	}
	~AutoHD( ){ if(m_hd) m_hd->hdDet(m_id); }
	
	ORes &at()
	{ 
	    if(m_hd) return *(ORes *)m_val; 
	    throw TError("AutoHD no init!");
	}
	
	void operator=( const AutoHD &hd )
	{  
	    //New attach from source parameter
	    THD *m_hd_t = m_hd;
	    m_hd = NULL;
	    if(m_hd_t) m_hd_t->hdDet(m_id);
	    m_id = hd.m_id;	    
	    m_val = hd.m_hd->hdAt(m_id);
	    m_hd = hd.m_hd;
	    ((AutoHD<ORes> &)hd).m_hd = NULL;
	}		

        THD *hd() const { return m_hd; }
        int id() const { return m_id; }

	void free() 
	{
	    if(m_hd) m_hd->hdDet(m_id);
	    m_hd = NULL; 
	    m_id = -1;
	}
	
	bool freeStat() { return (m_id < 0)?true:false; }
		    
    private:
	THD  *m_hd;
	int   m_id;
	void *m_val;
};

	
#endif // THD_H

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
 
#include <unistd.h>

#include "tsys.h"
#include "terror.h"
#include "tmessage.h"
#include "thd.h"

const char *THD::o_name = "THD";

THD::THD( const char *obj_n ) : res_ext(false), m_lock(false), m_free(true), u_name(obj_n)
{
    hd_res = ResAlloc::ResCreate();
}

THD::~THD()
{    
    if( !res_ext )ResAlloc::ResDelete(hd_res);
}

int THD::res( int id_res )
{
    if(res_ext)
    {
    	hd_res = id_res;
	return(hd_res);	
    }
    ResAlloc::ResDelete(hd_res);
    hd_res = id_res;
    res_ext = true;
    
    return(hd_res);   
}

void *THD::obj( const string &name )
{
    ResAlloc res(hd_res,false);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	    return(m_obj[i_o].obj);
    throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}

unsigned THD::obj_use( const string &name )
{
    ResAlloc res(hd_res,false);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    for(unsigned i_h = 0; i_h < m_hd.size(); i_h++ )
		if( m_hd[i_h].hd == i_o )
		    return( m_hd[i_h].use );
	    return(0);
	}
    throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}

unsigned THD::obj_use( unsigned i_hd )
{
    ResAlloc res(hd_res,false);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
	throw TError("(%s) hd %d no avoid!",o_name,i_hd);
    return( m_hd[i_hd].use );
}

unsigned THD::obj_cnt( )
{
    ResAlloc res(hd_res,false);
    unsigned cnt = m_obj.size();
    return(cnt);
}

void THD::obj_list( vector<string> &list )
{
    list.clear();
    ResAlloc res(hd_res,false);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( !m_obj[i_o].del ) list.push_back( *m_obj[i_o].name );
}

void THD::obj_add( void *obj, string *name, int pos )
{    
    unsigned id;
    if( m_lock ) throw TError("(%s) hd locked!",o_name);
    ResAlloc res(hd_res,true);
    //check already avoid object
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == *name ) 
	    throw TError("(%s) Object <%s> already avoid!",o_name, name->c_str());

    SHD_obj OHD = { obj, name, false };
    if( pos >= m_obj.size() || pos < 0 ) m_obj.push_back( OHD );
    else
    {
	m_obj.insert( m_obj.begin()+pos, OHD );
	for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
	    if( m_hd[i_hd].use && m_hd[i_hd].hd >= pos ) m_hd[i_hd].hd++;	
    }
    m_free = false;
}

void *THD::obj_del( const string &name, long tm )
{
    unsigned id;
    
    ResAlloc res(hd_res,true);
    //Check avoid object
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name )
	{
	    if( m_obj[i_o].del )
		throw TError("(%s) Object <%s> already deleted!",o_name, name.c_str());

	    //Mark object as deleted
	    m_obj[i_o].del = true;
	    res.release();
	
	    //Wait of free hd
	    time_t t_cur = time(NULL);
	    res.request(false);
	    for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
		if( m_hd[i_hd].use && m_hd[i_hd].hd == i_o)
		{
		    while( m_hd[i_hd].use )
		    {
			//Check timeout
			if( tm && time(NULL) > t_cur+tm)
			{
			    m_obj[i_o].del = false;
			    throw TError("(%s) %s: wait of freeing <%s> timeouted. Used for <%s>!",
				o_name, u_name, name.c_str(),m_hd[i_hd].user.c_str());
			}
#if OSC_DEBUG
		        Mess->put("DEBUG",MESS_INFO,"%s: %s wait of free header - %d:%s(%d), for <%s>!",
			    o_name,u_name,i_hd,m_hd[i_hd].user.c_str(),m_hd[i_hd].use,name.c_str());
#endif			
			usleep(STD_WAIT_DELAY*1000);			
		    }
		}
	    res.release( );

	    //Free object
	    res.request( true );
	    for( unsigned i_hd1 = 0; i_hd1 < m_hd.size(); i_hd1++ )
		if( m_hd[i_hd1].use && m_hd[i_hd1].hd > i_o ) m_hd[i_hd1].hd--;
	    void *t_obj = m_obj[i_o].obj;
    	    m_obj.erase(m_obj.begin() + i_o);
	    if( !m_obj.size() ) m_free = true;
	
	    return(t_obj);
	}
    throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}


void THD::obj_rotate( const string &name1, const string &name2 )
{
    unsigned i_o, n_1, n_2;
    ResAlloc res(hd_res,true);
    //Check avoid object
    for( i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name1 && !m_obj[i_o].del )
	    break;
    if( i_o >= m_obj.size() )
    	throw TError("(%s) Object <%s> no avoid!",o_name, name1.c_str());
    n_1 = i_o;
	
    for( i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name2 && !m_obj[i_o].del )
	    break;
    if( i_o >= m_obj.size() )
    	throw TError("(%s) Object <%s> no avoid!",o_name, name2.c_str());
    n_2 = i_o;

    SHD_obj t_obj = m_obj[n_1];
    m_obj[n_1] = m_obj[n_2];
    m_obj[n_2] = t_obj;
}

unsigned THD::hd_att( const string &name, const string &user )
{
    if( m_lock ) throw TError("(%s) hd locked!",o_name);
    ResAlloc res(hd_res,true);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    //Find used hd
	    for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
		if( m_hd[i_hd].use && m_hd[i_hd].hd == i_o && m_hd[i_hd].user == user )
		{
		    m_hd[i_hd].use++;
		    return(i_hd);
		}
	    //Find free hd
	    SHD_hd HD_hd = { 1, i_o, user };
	    unsigned i_hd;
	    for( i_hd = 0; i_hd < m_hd.size(); i_hd++ )
    		if( !m_hd[i_hd].use ) break;
	    //Make hd
	    if( i_hd >= m_hd.size() ) m_hd.push_back(HD_hd);
	    else                      m_hd[i_hd] = HD_hd;
	    return(i_hd);
	}
	throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}

void THD::hd_det( unsigned i_hd )
{
    ResAlloc res(hd_res,true);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
	throw TError("(%s) hd %d no avoid!",o_name,i_hd);
    m_hd[i_hd].use--;
}

void *THD::hd_at( unsigned i_hd )
{
    ResAlloc res(hd_res,false);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
	throw TError("(%s) hd %d no avoid!",o_name,i_hd);
    void *t_obj = m_obj[m_hd[i_hd].hd].obj;
    return(t_obj);
}

SHD_obj THD::hd_obj( unsigned i_hd )
{
    ResAlloc res(hd_res,false);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
	throw TError("(%s) hd %d no avoid!",o_name,i_hd);
    return m_obj[m_hd[i_hd].hd];
}

SHD_hd  THD::hd_hd( unsigned i_hd )
{
    ResAlloc res(hd_res,false);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
	throw TError("(%s) hd %d no avoid!",o_name,i_hd);
    return m_hd[i_hd];
}


#include <unistd.h>

#include "tsys.h"
#include "terror.h"
#include "tmessage.h"
#include "thd.h"

const char *THD::o_name = "THD";

THD::THD( const char *obj_n ) : res_ext(false), m_lock(false), m_free(true), u_name(obj_n)
{
    hd_res = TSYS::ResCreate();
}

THD::~THD()
{    
    if( !res_ext )TSYS::ResDelete(hd_res);
}

int THD::res( int id_res )
{
    if(res_ext)
    {
    	hd_res = id_res;
	return(hd_res);	
    }
    TSYS::ResDelete(hd_res);
    hd_res = id_res;
    res_ext = true;
    
    return(hd_res);   
}

void *THD::obj( string &name )
{
    TSYS::RResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    TSYS::RResRelease(hd_res);
	    return(m_obj[i_o].obj);
	}
    TSYS::RResRelease(hd_res);    
    throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}

unsigned THD::obj_use( string &name )
{
    TSYS::RResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    for(unsigned i_h = 0; i_h < m_hd.size(); i_h++ )
		if( m_hd[i_h].hd == i_o )
		{
		    TSYS::RResRelease(hd_res);
		    return( m_hd[i_h].use );
		}
	    TSYS::RResRelease(hd_res);
	    return(0);
	}
    TSYS::RResRelease(hd_res);    
    throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}

unsigned THD::obj_use( unsigned i_hd )
{
    TSYS::RResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	TSYS::RResRelease(hd_res);
	throw TError("(%s) hd %d error!",o_name,i_hd);
    }
    TSYS::RResRelease(hd_res);    
    return( m_hd[i_hd].use );
}

unsigned THD::obj_cnt( )
{
    TSYS::RResRequest(hd_res);
    unsigned cnt = m_obj.size();
    TSYS::RResRelease(hd_res);
    return(cnt);
}

void THD::obj_list( vector<string> &list )
{
    list.clear();
    TSYS::RResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( !m_obj[i_o].del ) list.push_back( *m_obj[i_o].name );
    TSYS::RResRelease(hd_res);
}

void THD::obj_add( void *obj, string *name, int pos )
{    
    unsigned id;
    if( m_lock ) throw TError("(%s) HD locked!",o_name);
    TSYS::WResRequest(hd_res);
    //check already avoid object
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == *name ) 
	{
	    TSYS::WResRelease(hd_res);
	    throw TError("(%s) Object <%s> already avoid!",o_name, name->c_str());
	}

    SHD_obj OHD = { obj, name, false };
    if( pos >= m_obj.size() || pos < 0 ) m_obj.push_back( OHD );
    else
    {
	m_obj.insert( m_obj.begin()+pos, OHD );
	for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
	    if( m_hd[i_hd].use && m_hd[i_hd].hd >= pos ) m_hd[i_hd].hd++;	
    }
    m_free = false;
    TSYS::WResRelease(hd_res);
}

void *THD::obj_del( string &name, long tm )
{
    unsigned id;
    
    TSYS::WResRequest(hd_res);
    //Check avoid object
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name )
	{
	    if( m_obj[i_o].del )
	    {
    		TSYS::WResRelease(hd_res);
		throw TError("(%s) Object <%s> already deleted!",o_name, name.c_str());
	    }

	    //Mark object as deleted
	    m_obj[i_o].del = true;
	    TSYS::WResRelease(hd_res);
	
	    //Wait of free hd
	    time_t t_cur = time(NULL);
	    TSYS::RResRequest(hd_res);
	    for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
		if( m_hd[i_hd].use && m_hd[i_hd].hd == i_o)
		{
		    while( m_hd[i_hd].use )
		    {
			//Check timeout
			if( tm && time(NULL) > t_cur+tm)
			{
			    m_obj[i_o].del = false;
			    TSYS::RResRelease(hd_res);
			    throw TError("(%s) %s object <%s> delete timeouted. Used <%s>!",
				o_name, u_name, name.c_str(),m_hd[i_hd].use,name.c_str());
			}
#if OSC_DEBUG
		        Mess->put("DEBUG",MESS_INFO,"%s: %s wait of free header - %d:%s(%d), for <%s>!",
			    o_name,u_name,i_hd,m_hd[i_hd].user.c_str(),m_hd[i_hd].use,name.c_str());
#endif			
			usleep(STD_WAIT_DELAY*1000);			
		    }
		}
	    TSYS::RResRelease(hd_res);

	    //Free object
	    TSYS::WResRequest(hd_res);
	    for( unsigned i_hd1 = 0; i_hd1 < m_hd.size(); i_hd1++ )
		if( m_hd[i_hd1].use && m_hd[i_hd1].hd > i_o ) m_hd[i_hd1].hd--;
	    void *t_obj = m_obj[i_o].obj;
    	    m_obj.erase(m_obj.begin() + i_o);
	    if( !m_obj.size() ) m_free = true;
    	    TSYS::WResRelease(hd_res);
	
	    return(t_obj);
	}
    TSYS::RResRelease(hd_res);
    throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}


void THD::obj_rotate( string &name1, string &name2 )
{
    unsigned i_o, n_1, n_2;
    TSYS::WResRequest(hd_res);
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

    TSYS::WResRelease(hd_res);
}

unsigned THD::hd_att( string &name, string user )
{
    if( m_lock ) throw TError("(%s) HD locked!",o_name);
    TSYS::WResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    //Find used hd
	    for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
		if( m_hd[i_hd].use && m_hd[i_hd].hd == i_o && m_hd[i_hd].user == user )
		{
		    m_hd[i_hd].use++;
		    TSYS::WResRelease(hd_res);
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
	    TSYS::WResRelease(hd_res);
	    return(i_hd);
	}
	TSYS::WResRelease(hd_res);
	throw TError("(%s) Object <%s> no avoid!",o_name, name.c_str());
}

void THD::hd_det( unsigned i_hd )
{
    TSYS::WResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	TSYS::WResRelease(hd_res);
	throw TError("(%s) hd %d error!",o_name,i_hd);
    }
    m_hd[i_hd].use--;
    TSYS::WResRelease(hd_res);
}

void *THD::hd_at( unsigned i_hd )
{
    TSYS::RResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	TSYS::RResRelease(hd_res);
	throw TError("(%s) hd %d error!",o_name,i_hd);
    }
    void *t_obj = m_obj[m_hd[i_hd].hd].obj;
    TSYS::RResRelease(hd_res);
    return(t_obj);
}


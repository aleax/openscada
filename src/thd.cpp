
#include <unistd.h>

#include "tsys.h"
#include "terror.h"
#include "tmessage.h"
#include "thd.h"

const char *THD::o_name = "THD";

THD::THD() : res_ext(false), m_lock(false), m_free(true)
{
    hd_res = SYS->ResCreate();
}

THD::~THD()
{    
    if( !res_ext )SYS->ResDelete(hd_res);
}

int THD::res( int id_res )
{
    if(res_ext) return(hd_res);	
    SYS->ResDelete(hd_res);
    hd_res = id_res;
    res_ext = true;
    
    return(hd_res);   
}

void *THD::obj( string &name )
{
    SYS->RResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    SYS->RResRelease(hd_res);
	    return(m_obj[i_o].obj);
	}
    SYS->RResRelease(hd_res);    
    throw TError("%s: Object <%s> no avoid!",o_name, name.c_str());
}

void *THD::obj( unsigned i_hd )
{
    SYS->RResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	SYS->RResRelease(hd_res);
	throw TError("%s: hd %d error!",o_name,i_hd);
    }
    SYS->RResRelease(hd_res);    
    return(m_obj[m_hd[i_hd].hd].obj);
}

unsigned THD::obj_use( string &name )
{
    SYS->RResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    for(unsigned i_h = 0; i_h < m_hd.size(); i_h++ )
		if( m_hd[i_h].hd == i_o )
		{
		    SYS->RResRelease(hd_res);
		    return( m_hd[i_h].use );
		}
	    SYS->RResRelease(hd_res);
	    return(0);
	}
    SYS->RResRelease(hd_res);    
    throw TError("%s: Object <%s> no avoid!",o_name, name.c_str());
}

unsigned THD::obj_use( unsigned i_hd )
{
    SYS->RResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	SYS->RResRelease(hd_res);
	throw TError("%s: hd %d error!",o_name,i_hd);
    }
    SYS->RResRelease(hd_res);    
    return( m_hd[i_hd].use );
}

unsigned THD::hd_obj_cnt( )
{
    SYS->RResRequest(hd_res);
    unsigned cnt = m_obj.size();
    SYS->RResRelease(hd_res);
    return(cnt);
}

void THD::hd_obj_list( vector<string> &list )
{
    list.clear();
    SYS->RResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( !m_obj[i_o].del ) list.push_back( *m_obj[i_o].name );
    SYS->RResRelease(hd_res);
}

void THD::hd_obj_add( void *obj, string *name )
{    
    unsigned id;
    if( m_lock ) throw TError("%s: HD locked!",o_name);
    SYS->WResRequest(hd_res);
    //check already avoid object
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
	if( *m_obj[i_o].name == *name ) 
	    throw TError("%s: Object %s already avoid!",o_name, name->c_str());

    SHD_obj OHD = { obj, name, false };
    m_obj.push_back( OHD );
    m_free = false;
    SYS->WResRelease(hd_res);
}

void *THD::hd_obj_del( string &name, long tm )
{
    unsigned id;
    SYS->RResRequest(hd_res);
    //Check avoid object
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name )
	{
	    if( m_obj[i_o].del )
	    {
		SYS->RResRelease(hd_res);
		throw TError("%s: Object <%s> already deleted!",o_name, name.c_str());
	    }
	    SYS->RResRelease(hd_res);

	    //Mark object as deleted
	    SYS->WResRequest(hd_res);
	    m_obj[i_o].del = true;
	    SYS->WResRelease(hd_res);
	
	    //Wait of free hd
	    time_t t_cur = time(NULL);
	    SYS->RResRequest(hd_res);
	    for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
		if( m_hd[i_hd].use && m_hd[i_hd].hd == i_o)
		{
		    while( m_hd[i_hd].use )
		    {
			//Check timeout
			if( tm && time(NULL) > t_cur+tm)
			{
			    m_obj[i_o].del = false;
			    SYS->RResRelease(hd_res);
			    throw TError("%s: object <%s> delete timeouted!",o_name, name.c_str());
			}
#if OSC_DEBUG
		        Mess->put("DEBUG",MESS_INFO,"%s: wait of free header - %d!",o_name,i_hd);
#endif			
			usleep(STD_WAIT_DELAY*1000);			
		    }
		}
	    SYS->RResRelease(hd_res);

	    //Free object
	    SYS->WResRequest(hd_res);
	    for( unsigned i_hd1 = 0; i_hd1 < m_hd.size(); i_hd1++ )
		if( m_hd[i_hd1].use && m_hd[i_hd1].hd > i_o ) m_hd[i_hd1].hd--;
	    void *t_obj = m_obj[i_o].obj;
    	    m_obj.erase(m_obj.begin() + i_o);
	    if( !m_obj.size() ) m_free = true;
    	    SYS->WResRelease(hd_res);
	
	    return(t_obj);
	}
    SYS->RResRelease(hd_res);
    throw TError("%s: Object <%s> no avoid!",o_name, name.c_str());
}

unsigned THD::hd_att( string &name )
{
    if( m_lock ) throw TError("%s: HD locked!",o_name);
    SYS->WResRequest(hd_res);
    for( unsigned i_o = 0; i_o < m_obj.size(); i_o++ )
    	if( *m_obj[i_o].name == name && !m_obj[i_o].del )
	{
	    //Find used hd
	    for( unsigned i_hd = 0; i_hd < m_hd.size(); i_hd++ )
		if( m_hd[i_hd].use && m_hd[i_hd].hd == i_o )
		{
		    m_hd[i_hd].use++;
		    SYS->WResRelease(hd_res);
		    return(i_hd);
		}
	    //Find free hd
	    SHD_hd HD_hd = { 1, i_o };
	    unsigned i_hd;
	    for( i_hd = 0; i_hd < m_hd.size(); i_hd++ )
    		if( !m_hd[i_hd].use ) break;
	    //Make hd
	    if( i_hd >= m_hd.size() ) m_hd.push_back(HD_hd);
	    else                      m_hd[i_hd] = HD_hd;
	    SYS->WResRelease(hd_res);
	    return(i_hd);
	}
	SYS->WResRelease(hd_res);
	throw TError("%s: Object <%s> no avoid!",o_name, name.c_str());
}

void THD::hd_det( unsigned i_hd )
{
    SYS->WResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	SYS->WResRelease(hd_res);
	throw TError("%s: hd %d error!",o_name,i_hd);
    }
    m_hd[i_hd].use--;
    SYS->WResRelease(hd_res);
}

void *THD::hd_at( unsigned i_hd )
{
    SYS->RResRequest(hd_res);
    if( i_hd >= m_hd.size() || !m_hd[i_hd].use )
    {
	SYS->RResRelease(hd_res);
	throw TError("%s: hd %d error!",o_name,i_hd);
    }
    void *t_obj = m_obj[m_hd[i_hd].hd].obj;
    SYS->RResRelease(hd_res);
    return(t_obj);
}


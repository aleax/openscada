
#include "xml.h"
#include "tmessage.h"
#include "terror.h"
#include "tcontr.h"

TContr::TContr( const char *i_tmpl ) : m_inf(i_tmpl)
{


}

TContr::~TContr()
{

}

XMLNode *TContr::ctr_info()
{
    XMLNode *node = new XMLNode();
    node->load_xml( m_inf );    
    ctr_fill_info( *node );
    
    return( node );
}

XMLNode *TContr::ctr_opt( XMLNode &inf, unsigned numb )
{
    int s_cfg = 0;    //section counter
    int fgn_cfg = 0;    //field counter
    try
    {
	while( true )
	{
	    XMLNode *t_s = inf.get_child("configs",s_cfg++);
	    try
	    {
		int f_cfg = 0;
		while(true)
		{
		    XMLNode *t_c = t_s->get_child("fld",f_cfg++);
		    if( fgn_cfg == numb ) return(t_c);
		    fgn_cfg++;
		}
	    }catch(...){ }
	    try{ return( ctr_opt(*t_s, numb-fgn_cfg) ); } catch(...){  }
	}
    }
    catch(...) { throw TError("Config field %d no avoid!",numb); }
    
    return(NULL);
}

string TContr::ctr_opt_getS( XMLNode &inf, string name )
{
    int s_cfg = 0;
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);
	if( t_s->get_attr("id") == name && !t_s->get_attr("type").size() )
	    return( t_s->get_text( ) );
    }
    return("");
}

int TContr::ctr_opt_getI( XMLNode &inf, string name )
{
    int s_cfg = 0;
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);
	if( t_s->get_attr("id") == name )
	{
	    if( t_s->get_attr("type") == "oct" )
    		return( strtol(t_s->get_text( ).c_str(),NULL,8) );
	    else
    		return( atoi(t_s->get_text( ).c_str()) );
	}
    }
    return(0);
}

double TContr::ctr_opt_getR( XMLNode &inf, string name )
{
    int s_cfg = 0;
    
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);	
	if( t_s->get_attr("id") == name && t_s->get_attr("type") == "real" )
	    return( atof( t_s->get_text( ).c_str() ) );
    }
    return(0.0);
}

bool TContr::ctr_opt_getB( XMLNode &inf, string name )
{
    int s_cfg = 0;
    
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);	
	if( t_s->get_attr("id") == name && t_s->get_attr("type") == "bool" )
	{
	    if( t_s->get_text( ) == "true" ) return(true);
	    else                             return(false);
	}
    }
    return(false);
}
	
void TContr::ctr_opt_setS( XMLNode &inf, string name, string val, bool mdf )
{
    int s_cfg = 0;
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);
	if( t_s->get_attr("id") == name && !t_s->get_attr("type").size() )
	{
    	    int len = atoi( t_s->get_attr("len").c_str() );
	    if( len && len < val.size() )
       		t_s->set_text( val.substr(val.size()-len,len) );
	    else t_s->set_text(val);
	    if( mdf ) t_s->set_attr("mdf","yes",true);
	    
	    return;
	}
    }
}

void TContr::ctr_opt_setI( XMLNode &inf, string name, int val, bool mdf )
{
    char b_str[100];
    int s_cfg = 0;
    
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);	
	if( t_s->get_attr("id") == name  )
	{
	    int len = atoi( t_s->get_attr("len").c_str() );
	    if( t_s->get_attr("type") == "oct" )
		snprintf(b_str,sizeof(b_str),"%o",val);
	    else
		snprintf(b_str,sizeof(b_str),"%d",val);
	    string s_v = b_str;
	    if( len && len < s_v.size() )
       		t_s->set_text( s_v.substr(s_v.size()-len,len) );
	    else t_s->set_text( s_v );
	    if( mdf ) t_s->set_attr("mdf","yes",true);
	    
	    return;
	}
    }
}

void TContr::ctr_opt_setR( XMLNode &inf, string name, double val, bool mdf )
{
    char b_str[100];
    int s_cfg = 0;
    
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);	
	if( t_s->get_attr("id") == name && t_s->get_attr("type") == "real" )
	{
	    int len = atoi( t_s->get_attr("len").c_str() );
	    snprintf(b_str,sizeof(b_str),"%f",val);
	    string s_v = b_str;
	    if( len && len < s_v.size() )
       		t_s->set_text( s_v.substr(0,len) );
	    else t_s->set_text( s_v );
	    if( mdf ) t_s->set_attr("mdf","yes",true);
	    
	    return;
	}
    }
}

void TContr::ctr_opt_setB( XMLNode &inf, string name, bool val, bool mdf )
{
    int s_cfg = 0;
    
    while( true )
    {
	XMLNode *t_s = ctr_opt(inf,s_cfg++);	
	if( t_s->get_attr("id") == name && t_s->get_attr("type") == "bool" )
	{
	    if( val ) t_s->set_text( "true" );
	    else      t_s->set_text( "false" );
	    if( mdf ) t_s->set_attr("mdf","yes",true);
	    
	    return;
	}
    }
}

void TContr::ctr_br_putlist( XMLNode &inf, vector<string> &list )
{
    XMLNode *br = inf.get_child("branchs"); 
    for( unsigned i_br = 0; i_br < list.size(); i_br++ )
	br->add_child("br")->set_text(list[i_br]);    
}

void TContr::ctr_opts_apply( XMLNode &inf )
{
    int s_cfg = 0;
    try
    {
	while( true )
	{
	    XMLNode *t_s = ctr_opt(inf,s_cfg++);	
	    if( t_s->get_attr("mdf") == "yes" )
	    {
	       	ctr_opt_apply( inf, *t_s );
	       	t_s->set_attr("mdf","no");
	    }
	}    
    }catch(...){ }
}



#include "xml.h"
#include "tmessage.h"
#include "tcontr.h"

const char *TContr::o_name = "TContr";	

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
    ctr_fill_info( node );
    
    return( node );
}

XMLNode *TContr::ctr_opt( XMLNode *inf, unsigned numb )
{
    int s_cfg = 0;    //section counter
    int fgn_cfg = 0;    //field counter
    try
    {
	while( true )
	{
	    XMLNode *t_s = inf->get_child("area",s_cfg++);
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
	    try{ return( ctr_opt(t_s, numb-fgn_cfg) ); } catch(...){  }
	}
    }
    catch(...) { throw TError("Config field %d no avoid!",numb); }
    
    return(NULL);
}

XMLNode *TContr::ctr_id( XMLNode *inf, string name_id )
{
    for( unsigned i_f = 0; i_f < inf->get_child_count(); i_f++)
	if( inf->get_child(i_f)->get_attr("id") == name_id ) return(inf->get_child(i_f));
	
    throw TError("%s:Field id = %s no avoid!",o_name,name_id.c_str());    
}

string TContr::chk_opt_val( XMLNode *fld, bool fix )
{
    string rez;
    if( fld->get_name() == "fld" )
    {
	if( !fld->get_attr("tp").size() || fld->get_attr("tp") == "str" )
	{
	    string text = fld->get_text();
	    int len = atoi( fld->get_attr("len").c_str() );
     	    if( len && len < fld->get_text().size() )
	    {
		if(fix) fld->set_text( text.substr(text.size()-len,len) );
		else    rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' string length more "+fld->get_attr("len")+"!";	    
	    }
	}
	else if( fld->get_attr("tp") == "oct" )
	{
	    string text = fld->get_text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !(text[i_t] >= '0' && text[i_t] <= '7') )
		    rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' used invalid symbol:'"+text[i_t]+"' for '"+fld->get_attr("tp")+"' type!";
	    //check maximum and minimum
	    string max = fld->get_attr("max");
	    if( max.size() && strtol(text.c_str(),NULL,8) > strtol(max.c_str(),NULL,8) )
		if( fix ) fld->set_text(max);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" > "+max+"!";
	    string min = fld->get_attr("min");
	    if( min.size() && strtol(text.c_str(),NULL,8) < strtol(min.c_str(),NULL,8) )
		if( fix ) fld->set_text(min);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" < "+min+"!";   		    
	}
	else if( fld->get_attr("tp") == "dec" )
	{
	    string text = fld->get_text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || text[i_t] == '-') )
		    rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' used invalid symbol:'"+text[i_t]+"' for '"+fld->get_attr("tp")+"' type!";
	    //check maximum and minimum
	    string max = fld->get_attr("max");
	    if( max.size() && atoi(text.c_str()) > atoi(max.c_str()) )
		if( fix ) fld->set_text(max);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" > "+max+"!";
	    string min = fld->get_attr("min");
	    if( min.size() && atoi(text.c_str()) < atoi(min.c_str()) )
		if( fix ) fld->set_text(min);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" < "+min+"!"; 
	}
	else if( fld->get_attr("tp") == "hex" )
	{
	    string text = fld->get_text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || (text[i_t] >= 'A' && text[i_t] <= 'F') || (text[i_t] >= 'a' && text[i_t] <= 'f')) )
		    rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' used invalid symbol:'"+text[i_t]+"' for '"+fld->get_attr("tp")+"' type!";
	    //check maximum and minimum
	    string max = fld->get_attr("max");
	    if( max.size() && strtol(text.c_str(),NULL,16) > strtol(max.c_str(),NULL,16) )
		if( fix ) fld->set_text(max);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" > "+max+"!";
	    string min = fld->get_attr("min");
	    if( min.size() && strtol(text.c_str(),NULL,16) < strtol(min.c_str(),NULL,16) )
		if( fix ) fld->set_text(min);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" < "+min+"!";  
	}
	else if( fld->get_attr("tp") == "real" ) 
	{
	    string text = fld->get_text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || text[i_t] == '-' || text[i_t] == '.') )
		    rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' used invalid symbol:'"+text[i_t]+"' for '"+fld->get_attr("tp")+"' type!";
	    //check maximum and minimum
	    string max = fld->get_attr("max");
	    if( max.size() && atof(text.c_str()) > atof(max.c_str()) )
		if( fix ) fld->set_text(max);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" > "+max+"!";
	    string min = fld->get_attr("min");
	    if( min.size() && atof(text.c_str()) < atof(min.c_str()) )
		if( fix ) fld->set_text(min);
		else rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' Error: "+text+" < "+min+"!";
	}
	else if( fld->get_attr("tp") == "bool" ) 
	{
	    string text = fld->get_text();
	    //check avoid symbols
	    if( !(text == "true" || text == "false") )
		rez = fld->get_attr("id")+":'"+fld->get_attr("dscr")+"' invalid value '"+text+"' for '"+fld->get_attr("tp")+"' type!";
	}  
    }
    return(rez);
}

string TContr::ctr_opt_getS( XMLNode *fld )
{
    if( fld->get_name() != "fld" ) throw TError("Node no <fld>!");    
    if( !fld->get_attr("tp").size() || fld->get_attr("tp") == "str" )
	return( fld->get_text( ) );
    throw TError("Field id = %s no string type!",fld->get_attr("id").c_str());    
}

int TContr::ctr_opt_getI( XMLNode *fld )
{
    if( fld->get_name() != "fld" ) throw TError("Node no <fld>!");    
    if( fld->get_attr("tp") == "oct" || fld->get_attr("tp") == "dec" || fld->get_attr("tp") == "hex" || fld->get_attr("tp") == "time")
    {
	if( fld->get_attr("tp") == "oct" )      
	    return( strtol(fld->get_text( ).c_str(),NULL,8) );
	else if( fld->get_attr("tp") == "hex" || fld->get_attr("tp") == "time") 
	    return( strtol(fld->get_text( ).c_str(),NULL,16) );
	else                         	        
	    return( atoi(fld->get_text( ).c_str()) );
    }
    throw TError("Field id = %s no integer type!",fld->get_attr("id").c_str());    
}

double TContr::ctr_opt_getR( XMLNode *fld )
{
    if( fld->get_name() != "fld" ) throw TError("Node no <fld>!");    
    if( fld->get_attr("tp") == "real" )
	return( atof( fld->get_text( ).c_str() ) );
    throw TError("Field id = %s no real type!",fld->get_attr("id").c_str());    
}

bool TContr::ctr_opt_getB( XMLNode *fld )
{
    if( fld->get_name() != "fld" ) throw TError("Node no <fld>!");    
    if( fld->get_attr("tp") == "bool" )
	return( (fld->get_text( ) == "true")?true:false );
    throw TError("Field id = %s no boolean type!",fld->get_attr("id").c_str());    
}
	
void TContr::ctr_opt_setS( XMLNode *fld, string val, int id )
{
    int len = atoi( fld->get_attr("len").c_str() );
    if( !fld->get_attr("tp").size() || fld->get_attr("tp") == "str" || fld->get_attr("tp") == "br")
    {
	XMLNode *el=fld;
        if( fld->get_name() == "list" )
	{	
	    char buf[10];
	    snprintf(buf,sizeof(buf),"%d",id); 
	    el = fld->add_child("el");
	    el->set_attr("id",buf,true);
	}
	if( len && len < val.size() )
	    el->set_text( val.substr(val.size()-len,len) );
	else el->set_text(val);
    }
    else throw TError("Field id = %s no string type!",fld->get_attr("id").c_str());    
}

void TContr::ctr_opt_setI( XMLNode *fld, int val, int id )
{
    char b_str[100];
    int len = atoi( fld->get_attr("len").c_str() );
    if( fld->get_attr("tp") == "oct" || fld->get_attr("tp") == "dec" || fld->get_attr("tp") == "hex" || fld->get_attr("tp") == "time")
    {
	if( fld->get_attr("tp") == "oct" )      
	    snprintf(b_str,sizeof(b_str),"%o",val);
	else if( fld->get_attr("tp") == "hex" || fld->get_attr("tp") == "time" ) 
	    snprintf(b_str,sizeof(b_str),"%x",val);
	else         	                        
	    snprintf(b_str,sizeof(b_str),"%d",val);
	string s_v = b_str;
	XMLNode *el=fld;    
        if( fld->get_name() == "list" )
	{	
	    snprintf(b_str,sizeof(b_str),"%d",id); 
	    el = fld->add_child("el");
	    el->set_attr("id",b_str,true);
	}	
	if( len && len < s_v.size() )
	    el->set_text( s_v.substr(s_v.size()-len,len) );
	else el->set_text( s_v );
    }
    else throw TError("Field id = %s no integer type!",fld->get_attr("id").c_str());    
}

void TContr::ctr_opt_setR( XMLNode *fld, double val, int id )
{
    char b_str[100];
    
    if( fld->get_attr("tp") == "real" )
    {
	int len = atoi( fld->get_attr("len").c_str() );
	snprintf(b_str,sizeof(b_str),"%f",val);
	string s_v = b_str;
	XMLNode *el=fld;    
        if( fld->get_name() == "list" )
	{	
	    snprintf(b_str,sizeof(b_str),"%d",id); 
	    el = fld->add_child("el");
	    el->set_attr("id",b_str,true);
	}	
	if( len && len < s_v.size() )
	    el->set_text( s_v.substr(0,len) );
	else el->set_text( s_v );
    }    
    else throw TError("Field id = %s no real type!",fld->get_attr("id").c_str());    
}

void TContr::ctr_opt_setB( XMLNode *fld, bool val, int id )
{
    if( fld->get_attr("tp") == "bool" )
    {    
    	XMLNode *el=fld;
        if( fld->get_name() == "list" )
	{	
	    char buf[10];
	    snprintf(buf,sizeof(buf),"%d",id); 
	    el = fld->add_child("el");
	    el->set_attr("id",buf,true);
	} 
	el->set_text( (val)?"true":"false" );
    }
    else throw TError("Field id = %s no boolean type!",fld->get_attr("id").c_str());    
}
	
string TContr::ctr_path_l(string path, int level)
{
    int an_dir = 0, t_lev = 0;
    if(path[0]=='/') an_dir = 1;
    while(true)
    {
	int t_dir = path.find("/",an_dir);
	if( t_lev++ == level ) return(path.substr(an_dir,t_dir-an_dir));
	if( t_dir == string::npos ) return("");
 	an_dir = t_dir+1;
    }
}

void TContr::ctr_din_set( string area_path, XMLNode *opt )
{
    string rez = chk_opt_val( opt, true );
    if( rez.size() ) throw TError(rez);
    ctr_din_set_( area_path, opt );
}

void TContr::ctr_din_get( string area_path, XMLNode *opt )
{
    ctr_din_get_( area_path, opt );
}

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include "xml.h"
#include "tsys.h"
#include "tmessage.h"
#include "tcontr.h"

#include "sstream"

using std::ostringstream;

const char *TContr::o_name = "TContr";	

TContr::TContr( )
{


}

TContr::~TContr()
{

}

void TContr::ctrStat( XMLNode &node )
{
    ctrStat_( &node );
}

XMLNode *TContr::ctrId( XMLNode *inf, const string &name_id )
{
    int level = 0;
    string s_el;

    XMLNode *t_node = inf;    
    while(true)
    {
	s_el = pathLev(name_id,level);
	if( !s_el.size() ) return(t_node);
	bool ok = false;
	for( unsigned i_f = 0; i_f < t_node->childSize(); i_f++)
	    if( t_node->childGet(i_f)->attr("id") == s_el ) 
	    {
		t_node = t_node->childGet(i_f);
		ok = true;
		break;
	    }
	if( !ok ) break;
	level++;
    }
	
    throw TError("(%s) Field id = %s(%s) no avoid!",o_name,name_id.c_str(),s_el.c_str());    
}

string TContr::ctrChk( XMLNode *fld, bool fix )
{
    char buf[STR_BUF_LEN];
    buf[0] = '\0';
    
    if( fld->name() == "fld" )
    {
	if( !fld->attr("tp").size() || fld->attr("tp") == "str" )
	{
	    string text = fld->text();
	    int len = atoi( fld->attr("len").c_str() );
     	    if( len && len < fld->text().size() )
	    {
		if(fix) fld->text( text.substr(text.size()-len,len) );
		else snprintf(buf,sizeof(buf),Mess->I18N("String length more '%d'!"),len );
	    }
	    if( fld->attr("dest") == "file" )
	    {
		int hd = open(text.c_str(),O_RDONLY);
		if( hd < 0 ) snprintf(buf,sizeof(buf),Mess->I18N("File error: '%s'!"),strerror(errno) );
		else close(hd);
	    }		
	    else if( fld->attr("dest") == "dir" )
	    {
		DIR *t_dr = opendir(text.c_str());
		if( t_dr == NULL ) snprintf(buf,sizeof(buf),Mess->I18N("Directory error: '%s'!"),strerror(errno) );
		else closedir(t_dr);
	    }		
	}
	else if( fld->attr("tp") == "oct" )
	{
	    string text = fld->text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !(text[i_t] >= '0' && text[i_t] <= '7') )
		    snprintf(buf,sizeof(buf),Mess->I18N("Used invalid symbol '%c' for element type '%s'!"),text[i_t],fld->attr("tp").c_str() );
	    //check maximum and minimum
	    string max = fld->attr("max");
	    if( max.size() && strtol(text.c_str(),NULL,8) > strtol(max.c_str(),NULL,8) )
		if( fix ) fld->text(max);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s > %s!"),text.c_str(), max.c_str() );       	
	    string min = fld->attr("min");
	    if( min.size() && strtol(text.c_str(),NULL,8) < strtol(min.c_str(),NULL,8) )
		if( fix ) fld->text(min);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s < %s!"),text.c_str(), min.c_str() );       	
	}
	else if( fld->attr("tp") == "dec" )
	{
	    string text = fld->text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || text[i_t] == '-') )
		    snprintf(buf,sizeof(buf),Mess->I18N("Used invalid symbol '%c' for element type '%s'!"),text[i_t],fld->attr("tp").c_str() );
	    //check maximum and minimum
	    string max = fld->attr("max");
	    if( max.size() && atoi(text.c_str()) > atoi(max.c_str()) )
		if( fix ) fld->text(max);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s > %s!"),text.c_str(), max.c_str() );       	
	    string min = fld->attr("min");
	    if( min.size() && atoi(text.c_str()) < atoi(min.c_str()) )
		if( fix ) fld->text(min);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s < %s!"),text.c_str(), min.c_str() );       	
	}
	else if( fld->attr("tp") == "hex" )
	{
	    string text = fld->text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || (text[i_t] >= 'A' && text[i_t] <= 'F') || (text[i_t] >= 'a' && text[i_t] <= 'f')) )
		    snprintf(buf,sizeof(buf),Mess->I18N("Used invalid symbol '%c' for element type '%s'!"),text[i_t],fld->attr("tp").c_str() );
	    //check maximum and minimum
	    string max = fld->attr("max");
	    if( max.size() && strtol(text.c_str(),NULL,16) > strtol(max.c_str(),NULL,16) )
		if( fix ) fld->text(max);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s > %s!"),text.c_str(), max.c_str() );       	
	    string min = fld->attr("min");
	    if( min.size() && strtol(text.c_str(),NULL,16) < strtol(min.c_str(),NULL,16) )
		if( fix ) fld->text(min);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s < %s!"),text.c_str(), min.c_str() );       	
	}
	else if( fld->attr("tp") == "real" ) 
	{
	    string text = fld->text();
	    //check avoid symbols
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || text[i_t] == '-' || text[i_t] == '.' || text[i_t] == ',') )
		    snprintf(buf,sizeof(buf),Mess->I18N("Used invalid symbol '%c' for element type '%s'!"),text[i_t],fld->attr("tp").c_str() );
	    //check maximum and minimum
	    string max = fld->attr("max");
	    if( max.size() && atof(text.c_str()) > atof(max.c_str()) )
		if( fix ) fld->text(max);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s > %s!"),text.c_str(), max.c_str() );       	
	    string min = fld->attr("min");
	    if( min.size() && atof(text.c_str()) < atof(min.c_str()) )
		if( fix ) fld->text(min);
		else snprintf(buf,sizeof(buf),Mess->I18N("Value of %s < %s!"),text.c_str(), min.c_str() );       	
	}
	else if( fld->attr("tp") == "bool" ) 
	{
	    string text = fld->text();
	    //check avoid symbols
	    if( !(text == "true" || text == "false") )
		snprintf(buf,sizeof(buf),Mess->I18N("Invalid value '%s' for element type '%s'!"),text.c_str(),fld->attr("tp").c_str() );
	}  
    }
    return(buf);
}

string TContr::ctrGetS( XMLNode *fld )
{
    if( fld->name() != "fld" ) throw TError("(%s) Node no <fld>!",o_name );    
    if( !fld->attr("tp").size() || fld->attr("tp") == "str" )
	return( fld->text( ) );
    throw TError("(%s) Field id = %s no string type!",o_name,fld->attr("id").c_str());    
}

int TContr::ctrGetI( XMLNode *fld )
{
    if( fld->name() != "fld" ) throw TError("(%s) Node no <fld>!",o_name);    
    if( fld->attr("tp") == "oct" || fld->attr("tp") == "dec" || fld->attr("tp") == "hex" || fld->attr("tp") == "time")
    {
	if( fld->attr("tp") == "oct" )      
	    return( strtol(fld->text( ).c_str(),NULL,8) );
	else if( fld->attr("tp") == "hex" || fld->attr("tp") == "time") 
	    return( strtol(fld->text( ).c_str(),NULL,16) );
	else                         	        
	    return( atoi(fld->text( ).c_str()) );
    }
    throw TError("(%s) Field id = %s no integer type!",o_name,fld->attr("id").c_str());    
}

double TContr::ctrGetR( XMLNode *fld )
{
    if( fld->name() != "fld" ) throw TError("(%s) Node no <fld>!",o_name);    
    if( fld->attr("tp") == "real" )
	return( atof( fld->text( ).c_str() ) );
    throw TError("(%s) Field id = %s no real type!",o_name,fld->attr("id").c_str());    
}

bool TContr::ctrGetB( XMLNode *fld )
{
    if( fld->name() != "fld" ) throw TError("(%s) Node no <fld>!",o_name);    
    if( fld->attr("tp") == "bool" )
	return( (fld->text( ) == "true")?true:false );
    throw TError("(%s) Field id = %s no boolean type!",o_name,fld->attr("id").c_str());    
}
	
void TContr::ctrSetS( XMLNode *fld, const string &val, int id )
{

    int len = atoi( fld->attr("len").c_str() );
    if( !fld->attr("tp").size() || fld->attr("tp") == "str" || fld->attr("tp") == "br")
    {
	XMLNode *el=fld;
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    el->attr("id",TSYS::int2str(id,C_INT_DEC),true);
	}
	if( len && len < val.size() )
	    el->text( val.substr(val.size()-len,len) );
	else el->text(val);
    }
    else throw TError("(%s) Field id = %s no string type!",o_name,fld->attr("id").c_str());    
}

void TContr::ctrSetI( XMLNode *fld, int val, int id )
{
    string s_v;
    
    int len = atoi( fld->attr("len").c_str() );
    if( fld->attr("tp") == "oct" || fld->attr("tp") == "dec" || fld->attr("tp") == "hex" || fld->attr("tp") == "time")
    {
	if( fld->attr("tp") == "oct" ) 
	    s_v = TSYS::int2str(val,C_INT_OCT);
	else if( fld->attr("tp") == "hex" || fld->attr("tp") == "time" ) 
	    s_v = TSYS::int2str(val,C_INT_HEX);
	else         	               
	    s_v = TSYS::int2str(val,C_INT_DEC);
	    
	XMLNode *el=fld;    
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    el->attr("id",TSYS::int2str(id),true);
	}	
	if( len && len < s_v.size() )
	    el->text( s_v.substr(s_v.size()-len,len) );
	else el->text( s_v );
    }
    else throw TError("(%s) Field id = %s no integer type!",o_name,fld->attr("id").c_str());    
}

void TContr::ctrSetR( XMLNode *fld, double val, int id )
{
    if( fld->attr("tp") == "real" )
    {
	int len = atoi( fld->attr("len").c_str() );
	string s_v = TSYS::real2str(val);
	XMLNode *el=fld;    
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    el->attr("id",TSYS::int2str(id,C_INT_DEC),true);
	}	
	if( len && len < s_v.size() )
	    el->text( s_v.substr(0,len) );
	else el->text( s_v );
    }    
    else throw TError("(%s) Field id = %s no real type!",o_name,fld->attr("id").c_str());    
}

void TContr::ctrSetB( XMLNode *fld, bool val, int id )
{
    if( fld->attr("tp") == "bool" )
    {    
    	XMLNode *el=fld;
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    el->attr("id",TSYS::int2str(id),true);
	} 
	el->text( (val)?"true":"false" );
    }
    else throw TError("(%s) Field id = %s no boolean type!",o_name,fld->attr("id").c_str());    
}
	
string TContr::pathLev( const string &path, int level, bool encode )
{
    int an_dir = 0, t_lev = 0;
    if(path[0]=='/') an_dir = 1;
    while(true)
    {
	int t_dir = path.find("/",an_dir);
	if( t_lev++ == level ) 
	{
	    if( encode ) return( pathEncode(path.substr(an_dir,t_dir-an_dir),true) );
	    return( path.substr(an_dir,t_dir-an_dir) );
	}
	if( t_dir == string::npos ) return("");
 	an_dir = t_dir+1;
    }
}

string TContr::pathCode( const string &in, bool el )
{
    string path = in;
    
    for( unsigned i_sz = 0; i_sz < path.size(); i_sz++ )
	if( el )
	{
    	    if( path[i_sz] == '/' )     path.replace(i_sz,1,"%2f");
	    else if( path[i_sz] == ':' )path.replace(i_sz,1,"%3a");
    	    else if( path[i_sz] == '%' )path.replace(i_sz++,1,"%%");
	}
	else if( path[i_sz] == '/' ) path[i_sz] = ':';	
    
    return(path);    
}

string TContr::pathEncode( const string &in, bool el )
{
    int n_pos=0;
    string path = in;
    
    if( el )
    {
	while(true)
	{
    	    n_pos = path.find("%",n_pos);
    	    if( n_pos == string::npos ) break;
    	    if( path[n_pos+1] == '%' ) path.replace(n_pos,2,"%");
    	    else
        	path.replace(n_pos,3,string("")+(char)strtol(path.substr(n_pos+1,2).c_str(),NULL,16));	
	    n_pos+=1;
	}
    }
    else    
	for( unsigned i_sz = 0; i_sz < path.size(); i_sz++ )
    	    if( path[i_sz] == ':' ) path[i_sz] = '/';	
	    
    return(path);
}

void TContr::ctrDinSet( const string &area_path, XMLNode *opt )
{
    string rez = ctrChk( opt, true );
    if( rez.size() ) throw TError(rez);
    ctrDinSet_( area_path, opt );
}

void TContr::ctrDinGet( const string &area_path, XMLNode *opt )
{
    ctrDinGet_( area_path, opt );
}

void TContr::ctr_cfg_parse( const string &p_elem, XMLNode *fld, int pos, TConfig *cfg )
{    	
    vector<string> list_c;
    cfg->cfgList(list_c);
    XMLNode *w_fld = ctrId(fld, p_elem);
    
    for( unsigned i_el = 0; i_el < list_c.size(); i_el++ )
	ctr_fld_parse( p_elem, cfg->cfg(list_c[i_el]).fld(), w_fld, (pos<0)?pos:pos++ );
}


void TContr::ctr_cfg_set( const string &elem, XMLNode *fld, TConfig *cfg )
{    
    if( elem.substr(0,4) == "sel_" )
    {
	TFld &n_e_fld = cfg->cfg(elem.substr(4)).fld();
	for( unsigned i_a=0; i_a < n_e_fld.selNm().size(); i_a++ )
	    ctrSetS( fld, n_e_fld.selNm()[i_a], i_a );
	return;
    }
    TFld &n_e_fld = cfg->cfg(elem).fld();
    if(n_e_fld.type()&T_SELECT) 	ctrSetS(fld,cfg->cfg(elem).getSEL());	
    else if(n_e_fld.type()&T_STRING)   	ctrSetS(fld,cfg->cfg(elem).getS());	
    else if(n_e_fld.type()&(T_DEC|T_OCT|T_HEX))	ctrSetI(fld,cfg->cfg(elem).getI());	
    else if(n_e_fld.type()&T_REAL) 	ctrSetR(fld,cfg->cfg(elem).getR());	    
    else if(n_e_fld.type()&T_BOOL) 	ctrSetB(fld,cfg->cfg(elem).getB());	    
}

void TContr::ctr_cfg_get( const string &elem, XMLNode *fld, TConfig *cfg )
{
    TFld &n_e_fld = cfg->cfg(elem).fld();
    if(n_e_fld.type()&T_SELECT)		cfg->cfg(elem).setSEL(ctrGetS(fld));	
    else if(n_e_fld.type()&T_STRING)	cfg->cfg(elem).setS(ctrGetS(fld));	
    else if(n_e_fld.type()&(T_DEC|T_OCT|T_HEX))	cfg->cfg(elem).setI(ctrGetI(fld));	
    else if(n_e_fld.type()&T_REAL) 	cfg->cfg(elem).setR(ctrGetR(fld));	
    else if(n_e_fld.type()&T_BOOL) 	cfg->cfg(elem).setB(ctrGetB(fld));	
}

void TContr::ctr_val_parse( const string &p_elem, XMLNode *fld, int pos, TValue *val )
{    	
    vector<string> list_c;
    val->vlList(list_c);
    XMLNode *w_fld = ctrId(fld, p_elem);
    
    for( unsigned i_el = 0; i_el < list_c.size(); i_el++ )
	ctr_fld_parse( p_elem, val->vlAt(list_c[i_el]).at().fld(), w_fld, (pos<0)?pos:pos++ );
}

void TContr::ctr_val_set( const string &elem, XMLNode *fld, TValue *val )  //?!?!
{    
    if( elem.substr(0,4) == "sel_" )
    {
	AutoHD<TVal> vl = val->vlAt(elem.substr(4));
	for( unsigned i_a=0; i_a < vl.at().fld().selNm().size(); i_a++ )
	    ctrSetS( fld, vl.at().fld().selNm()[i_a], i_a );
	return;
    }
    AutoHD<TVal> vl = val->vlAt(elem);
    if(vl.at().fld().type()&T_SELECT) 		ctrSetS(fld,vl.at().getSEL());	
    else if(vl.at().fld().type()&T_STRING)   	ctrSetS(fld,vl.at().getS());	
    else if(vl.at().fld().type()&(T_DEC|T_OCT|T_HEX))	ctrSetI(fld,vl.at().getI());	
    else if(vl.at().fld().type()&T_REAL) 	ctrSetR(fld,vl.at().getR());	    
    else if(vl.at().fld().type()&T_BOOL) 	ctrSetB(fld,vl.at().getB());	    
}

void TContr::ctr_val_get( const string &elem, XMLNode *fld, TValue *val )
{
    AutoHD<TVal> vl = val->vlAt(elem);
    if(vl.at().fld().type()&T_SELECT)		vl.at().setSEL(ctrGetS(fld));	
    else if(vl.at().fld().type()&T_STRING)	vl.at().setS(ctrGetS(fld));	
    else if(vl.at().fld().type()&(T_DEC|T_OCT|T_HEX))	vl.at().setI(ctrGetI(fld));	
    else if(vl.at().fld().type()&T_REAL) 	vl.at().setR(ctrGetR(fld));	
    else if(vl.at().fld().type()&T_BOOL) 	vl.at().setB(ctrGetB(fld));	
}

void TContr::ctr_fld_parse( const string &p_elem, TFld &fld, XMLNode *w_fld, int pos )
{
    XMLNode *n_e;
    
    if( pos < 0 && pos > w_fld->childSize() ) n_e = w_fld->childAdd("fld");
    else n_e = w_fld->childIns(pos,"fld");
    n_e->attr("id",fld.name());
    if( fld.type()&F_NWR ) n_e->attr("acs","0440");
    else n_e->attr("acs","0660");
    n_e->attr("dscr",fld.descr());
    n_e->attr("len",TSYS::int2str(fld.len()));
    if(fld.type()&T_SELECT) 
    {
	n_e->attr("tp","str");	
	n_e->attr("len","");
	n_e->attr("dest","select");
	n_e->attr("select",p_elem+"/sel_"+fld.name());
	n_e = w_fld->childAdd("list");
	n_e->attr("id","sel_"+fld.name());
	n_e->attr("tp","str");	
	n_e->attr("hide","1");	
    }
    else if(fld.type()&T_STRING)n_e->attr("tp","str");	
    else if(fld.type()&T_DEC)	n_e->attr("tp","dec");
    else if(fld.type()&T_OCT)	n_e->attr("tp","oct");
    else if(fld.type()&T_HEX)	n_e->attr("tp","hex");
    else if(fld.type()&T_REAL)	n_e->attr("tp","real");
    else if(fld.type()&T_BOOL)	n_e->attr("tp","bool");
}
	


//OpenSCADA system file: tcntrnode.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>

#include "xml.h"
#include "tsys.h"
#include "resalloc.h"
#include "tmess.h"
#include "tcntrnode.h"

TCntrNode::TCntrNode( TCntrNode *iprev ) : m_mod(Disable), m_use(0)
{
    hd_res = ResAlloc::resCreate();
    prev.node = iprev;
    prev.grp = -1;
}

TCntrNode::~TCntrNode()
{
    nodeDelAll();
    ResAlloc::resDelete(hd_res);
}

void TCntrNode::nodeDelAll( )
{
    if( nodeMode() != Disable )     nodeDis();
    
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	while( chGrp[i_g].el.size() )
	{
	    delete chGrp[i_g].el.back();
	    chGrp[i_g].el.pop_back();
	}
}

XMLNode *TCntrNode::ctrId( XMLNode *inf, const string &name_id )
{
    int level = 0;
    string s_el;

    XMLNode *t_node = inf;    
    while(true)
    {
	s_el = TSYS::pathLev(name_id,level);
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
	
    throw TError("XML","Field id = %s(%s) no present!",name_id.c_str(),s_el.c_str());    
}

string TCntrNode::ctrChk( XMLNode *fld, bool fix )
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
	    //check of symbols passed
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
	    //check symbols passed
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
	    //check symbols passed
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
	    //check symbols passed
	    for( unsigned i_t = 0; i_t < text.size(); i_t++ )
		if( !((text[i_t] >= '0' && text[i_t] <= '9') || text[i_t] == '-' || 
			text[i_t] == '.' || text[i_t] == ',' || text[i_t] == '+' || text[i_t] == 'e' ) )
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
	    //check symbols passed
	    if( !(text == "true" || text == "false") )
		snprintf(buf,sizeof(buf),Mess->I18N("Invalid value '%s' for element type '%s'!"),text.c_str(),fld->attr("tp").c_str() );
	}  
    }
    return(buf);
}

string TCntrNode::ctrGetS( XMLNode *fld )
{
    return( fld->text( ) );
}

int TCntrNode::ctrGetI( XMLNode *fld )
{
    if( fld->attr("tp") == "oct" )      
	return( strtol(fld->text( ).c_str(),NULL,8) );
    else if( fld->attr("tp") == "hex" || fld->attr("tp") == "time") 
	return( strtol(fld->text( ).c_str(),NULL,16) );
    else return( atoi(fld->text( ).c_str()) );
}

double TCntrNode::ctrGetR( XMLNode *fld )
{
    return( atof( fld->text( ).c_str() ) );
}

bool TCntrNode::ctrGetB( XMLNode *fld )
{
    return( (fld->text( ) == "true")?true:false );
}
	
void TCntrNode::ctrSetS( XMLNode *fld, const string &val, const char *id )
{
    //int len = atoi( fld->attr("len").c_str() );
    if( !fld->attr("tp").size() || fld->attr("tp") == "str" || fld->attr("tp") == "br" )
    {
	XMLNode *el=fld;
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    if(id) el->attr("id",id);
	}
	//if( len && len < val.size() )
	//    el->text( val.substr(val.size()-len,len) );
	//else 
	el->text(val);
    }
    else throw TError("Node","Field id = %s no string type!",fld->attr("id").c_str());    
}

void TCntrNode::ctrSetI( XMLNode *fld, int val, const char *id )
{
    string s_v;
    
    int len = atoi( fld->attr("len").c_str() );
    if( fld->attr("tp") == "oct" || fld->attr("tp") == "dec" || fld->attr("tp") == "hex" || fld->attr("tp") == "time")
    {
	if( fld->attr("tp") == "oct" ) 
	    s_v = TSYS::int2str(val,TSYS::Oct);
	else if( fld->attr("tp") == "hex" || fld->attr("tp") == "time" ) 
	    s_v = TSYS::int2str(val,TSYS::Hex);
	else         	               
	    s_v = TSYS::int2str(val,TSYS::Dec);
	    
	XMLNode *el=fld;    
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    if(id) el->attr("id",id);
	}	
	if( len && len < s_v.size() )
	    el->text( s_v.substr(s_v.size()-len,len) );
	else el->text( s_v );
    }
    else throw TError("Node","Field id = %s no integer type!",fld->attr("id").c_str());    
}

void TCntrNode::ctrSetR( XMLNode *fld, double val, const char *id )
{
    if( fld->attr("tp") == "real" )
    {
	int len = atoi( fld->attr("len").c_str() );
	string s_v = TSYS::real2str(val);
	XMLNode *el=fld;    
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    if(id) el->attr("id",id);
	}	
	if( len && len < s_v.size() )
	    el->text( s_v.substr(0,len) );
	else el->text( s_v );
    }    
    else throw TError("Node","Field id = %s no real type!",fld->attr("id").c_str());    
}

void TCntrNode::ctrSetB( XMLNode *fld, bool val, const char *id )
{
    if( fld->attr("tp") == "bool" )
    {    
    	XMLNode *el=fld;
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    if(id) el->attr("id",id);
	} 
	el->text( (val)?"true":"false" );
    }
    else throw TError("Node","Field id = %s no boolean type!",fld->attr("id").c_str());    
}
	
void TCntrNode::cntrCmd( const string &path, XMLNode *opt, TCntrNode::Command cmd, int lev )
{   
    string s_br = TSYS::strEncode(TSYS::pathLev(path,lev,false),TSYS::Path);
    
    if( s_br.size() && s_br[0] != '/' )
    {
	for( int i_g = 0; i_g < chGrp.size(); i_g++ )
    	    if( s_br.substr(0,chGrp[i_g].id.size()) == chGrp[i_g].id )
	    {
		chldAt(i_g,s_br.substr(chGrp[i_g].id.size())).at().cntrCmd(path,opt,cmd,lev+1);
		return;
	    }
	//Go to default thread
	if( chGrp.size() )
	    chldAt(0,s_br).at().cntrCmd(path,opt,cmd,lev+1);
	return;    
    }
    if(cmd == Info)	opt->clear();
    cntrCmd_(s_br,opt,cmd);
}

//***********************************************************
//*********** Resource section ******************************
//***********************************************************
void TCntrNode::nodeEn()
{ 
    if( m_mod == Enable )	throw TError(nodePath().c_str(),"Node already enabled!");
    if( m_mod != Disable )	throw TError(nodePath().c_str(),"Node already in process!");
    
    ResAlloc res(hd_res,true);
    m_mod = MkEnable;
    res.release();
    
    preEnable();

    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	for( unsigned i_o = 0; i_o < chGrp[i_g].el.size(); i_o++ )
    	    if( chGrp[i_g].el[i_o]->nodeMode() == Disable )
    		chGrp[i_g].el[i_o]->nodeEn();
		
    res.request(true);	    
    m_mod = Enable;
    res.release();
    
    postEnable();
};

void TCntrNode::nodeDis(long tm, int flag)
{ 
    if( m_mod == Disable )	throw TError(nodePath().c_str(),"Node already disabled!");
    if( m_mod != Enable )	throw TError(nodePath().c_str(),"Node already in process!");
    
    preDisable(flag);

    ResAlloc res(hd_res,true);    
    m_mod = MkDisable;
    res.release();
    
    try
    {
	for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	    for( int i_o = chGrp[i_g].el.size()-1; i_o >= 0; i_o-- )
	        if( chGrp[i_g].el[i_o]->nodeMode() == Enable )
	    	    chGrp[i_g].el[i_o]->nodeDis(tm);	     
	
	//Wait of free node	
	time_t t_cur = time(NULL);
	while(1)
	{
	    if( !m_use )	break;
#if OSC_DEBUG
            Mess->put(nodePath().c_str(),TMess::Debug,"Wait of free %d users!",m_use);
#endif	    
	    //Check timeout
	    if( tm && time(NULL) > t_cur+tm)
		throw TError(nodePath().c_str(),Mess->I18N("Timeouted of wait. Object used by %d users. Free object first!"),m_use);
	    usleep(STD_WAIT_DELAY*1000);	    
	}
        res.request(true);
	m_mod = Disable;
	res.release();			       
    }catch(TError err)
    {
	res.request(true);
	m_mod = Disable;
	res.release();
	nodeEn();
	throw;
    }   
    postDisable(flag);     
};

void TCntrNode::nodeList(vector<string> &list)
{
    vector<string> tls;
    list.clear();
    for( int i_gr = 0; i_gr < chGrp.size(); i_gr++ )
    {
	chldList(i_gr,tls);
	for( int i_l = 0; i_l < tls.size(); i_l++ )
	    list.push_back(chGrp[i_gr].id+tls[i_l]);
    }
}

AutoHD<TCntrNode> TCntrNode::nodeAt(const string &path, int lev, char sep)
{
    string s_br;
    if( sep != 0 )	s_br = TSYS::strEncode(TSYS::strSepParse(path,lev,sep),TSYS::Path);
    else 		s_br = TSYS::strEncode(TSYS::pathLev(path,lev,false),TSYS::Path);
    if( !s_br.size() )
    {	
	if( nodeMode() == Disable ) throw TError(nodePath().c_str(),"Node is disabled!");
	return this;
    }
    ResAlloc res(hd_res,false);
    for( int i_g = 0; i_g < chGrp.size(); i_g++ )
        if( s_br.substr(0,chGrp[i_g].id.size()) == chGrp[i_g].id )
            return chldAt(i_g,s_br.substr(chGrp[i_g].id.size())).at().nodeAt(path,lev+1,sep);
    //Go to default thread
    if( chGrp.size() )
	return chldAt(0,s_br).at().nodeAt(path,lev+1,sep);
}

unsigned TCntrNode::grpAdd( const string &iid )
{
    int g_id = chGrp.size();
    chGrp.push_back( GrpEl() );
    chGrp[g_id].id = iid;

    return g_id;
}

void TCntrNode::chldList( unsigned igr, vector<string> &list )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");

    list.clear();
    for( unsigned i_o = 0; i_o < chGrp[igr].el.size(); i_o++ )	
        if( chGrp[igr].el[i_o]->nodeMode() != Disable )
            list.push_back( chGrp[igr].el[i_o]->nodeName() );    
}

bool TCntrNode::chldPresent( unsigned igr, const string &name )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");
    
    for( unsigned i_o = 0; i_o < chGrp[igr].el.size(); i_o++ )
        if( chGrp[igr].el[i_o]->nodeName() == name )	return true;
    
    return false;
}

void TCntrNode::chldAdd( unsigned igr, TCntrNode *node, int pos )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs <%d> error!",igr);
    if( nodeMode() != Enable ) 	throw TError(nodePath().c_str(),"Node is not enabled!");
    
    if( TSYS::strEmpty( node->nodeName() ) )
    {
	delete node;
        throw TError(nodePath().c_str(),"Add child id is empty!");
    }
        
    //check object present
    for( unsigned i_o = 0; i_o < chGrp[igr].el.size(); i_o++ )
        if( chGrp[igr].el[i_o]->nodeName() == node->nodeName() )
            throw TError(nodePath().c_str(),"Child <%s> already present!",node->nodeName().c_str());
    res.release();
    
    res.request(true);
    if( pos >= chGrp[igr].el.size() || pos < 0 ) chGrp[igr].el.push_back( node );
    else chGrp[igr].el.insert( chGrp[igr].el.begin()+pos, node );    
    node->prev.node = this;
    node->prev.grp = igr;
    res.release();
    
    if( node->nodeMode() == Disable )	node->nodeEn();    
}

void TCntrNode::chldDel( unsigned igr, const string &name, long tm, int flag )
{
    if( tm < 0 )	tm = DEF_TIMEOUT;
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs <%d> error!",igr);
    if( nodeMode() != Enable )	throw TError(nodePath().c_str(),"Node is not enabled!");    
    
    //Check object present
    TCntrNode *nd = NULL;
    for( unsigned i_o = 0; i_o < chGrp[igr].el.size(); i_o++ )
        if( chGrp[igr].el[i_o]->nodeName() == name )
            nd = chGrp[igr].el[i_o];
    res.release();	    
    if( nd )
    {
	if( nd->nodeMode() && Enable ) nd->nodeDis(tm,flag);
	res.request(true);
	for( unsigned i_o = 0; i_o < chGrp[igr].el.size(); i_o++ )
	    if( chGrp[igr].el[i_o]->nodeName() == name )
	    {
		delete chGrp[igr].el[i_o];
        	chGrp[igr].el.erase(chGrp[igr].el.begin()+i_o);			    
	    }
	res.release();    
	return;    			     
    }
    throw TError(nodePath().c_str(),"Child <%s> no present!", name.c_str());
}

unsigned TCntrNode::nodeUse(  )
{
    ResAlloc res(hd_res,false);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");
    
    unsigned i_use = m_use;
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	for( unsigned i_o = 0; i_o < chGrp[i_g].el.size(); i_o++ )
	    if( chGrp[i_g].el[i_o]->nodeMode() != Disable )
		i_use+=chGrp[i_g].el[i_o]->nodeUse();
		
    return i_use;
}

string TCntrNode::nodePath( char sep )
{
    if( sep )
    {
	if( prev.node ) 
	    return prev.node->nodePath(sep)+string(1,sep)+((prev.grp<0)?"":prev.node->chGrp[prev.grp].id)+nodeName();
	else return nodeName();
    }	
    else 
    {
	if( prev.node )
    	    return prev.node->nodePath(sep)+((prev.grp<0)?"":prev.node->chGrp[prev.grp].id)+nodeName()+"/";
	else return "/"+nodeName()+"/";
    }
}

TCntrNode *TCntrNode::nodePrev()
{ 
    if( prev.node ) return prev.node;
    throw TError(nodePath().c_str(),"Node is it root or no connect!");
}

AutoHD<TCntrNode> TCntrNode::chldAt( unsigned igr, const string &name, const string &user )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() ) 	throw TError(nodePath().c_str(),"Group of childs <%d> error!",igr);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");
    //Check object present
    for( unsigned i_o = 0; i_o < chGrp[igr].el.size(); i_o++ )
    	if( chGrp[igr].el[i_o]->nodeName() == name && chGrp[igr].el[i_o]->nodeMode() != Disable )
            return AutoHD<TCntrNode>(chGrp[igr].el[i_o],user);
    throw TError(nodePath().c_str(),Mess->I18N("Element <%s> no present or disabled!"), name.c_str());
}

void TCntrNode::connect()
{
    ResAlloc res(hd_res,true);
    m_use++;
}

void TCntrNode::disConnect()
{
    ResAlloc res(hd_res,true);
    m_use--;
}

XMLNode *TCntrNode::ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *req, const char *path,
        const string &dscr, int perm, int uid, int gid, int n_attr, ... )
{
    int i_lv;
    
    //- Check displaing node -
    for( i_lv = 0; TSYS::pathLev(req,i_lv).size(); i_lv++ )
	if( TSYS::pathLev(path,i_lv) != TSYS::pathLev(req,i_lv) )
	    return NULL;
    
    //- Go to element -
    for( ;TSYS::pathLev(path,i_lv).size(); i_lv++ )
        try { nd = nd->childGet("id",TSYS::pathLev(path,i_lv) ); }
	catch(TError err)
	{
	    if( TSYS::pathLev(path,i_lv+1).size() )	throw;
	    nd = nd->childIns(pos);
	}	
    nd->name(n_nd);
    nd->attr("id",TSYS::pathLev(path,i_lv-1));
    nd->attr("dscr",dscr);
    nd->attr("acs",TSYS::int2str(perm,TSYS::Oct));
    nd->attr("own",TSYS::int2str(uid));
    nd->attr("grp",TSYS::int2str(gid));
    
    //- Get addon attributes -
    if( n_attr )
    {
	char *atr_id, *atr_vl;
	va_list argptr;
	va_start(argptr,n_attr);
	for( int i_a = 0; i_a < n_attr; i_a++ )
	{
    	    atr_id = va_arg(argptr, char *);
	    atr_vl = va_arg(argptr, char *);
	    nd->attr(atr_id,atr_vl);
	}
	va_end(argptr);
    }
    
    return nd;
}

void TCntrNode::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{ 
    if( cmd==TCntrNode::Info )
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18N("Node: ")+nodeName());
    else if( cmd==TCntrNode::Get )
	throw TError(nodePath().c_str(),Mess->I18N("Node control element <%s> error!"),a_path.c_str());
    else if( cmd==TCntrNode::Set )
	throw TError(nodePath().c_str(),Mess->I18N("Node control element <%s> error!"),a_path.c_str());
}

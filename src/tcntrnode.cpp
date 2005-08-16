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
#include "resalloc.h"
#include "tmessage.h"
#include "tcntrnode.h"

#include "sstream"

using std::ostringstream;

long TCntrNode::dtm = 2;
XMLNode TCntrNode::m_dummy;

TCntrNode::TCntrNode( TCntrNode *prev ) : m_mod(Disable), m_use(0), m_prev(prev)
{
    hd_res = ResAlloc::resCreate();
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
	while( chGrp[i_g].size() )
	{
	    delete chGrp[i_g].back();
	    chGrp[i_g].pop_back();
	    //delete chGrp[i_g][0];
	    //chGrp[i_g].erase(chGrp[i_g].begin());
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
	    //check avoid symbols
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

    int len = atoi( fld->attr("len").c_str() );
    if( !fld->attr("tp").size() || fld->attr("tp") == "str" || fld->attr("tp") == "br" )
    {
	XMLNode *el=fld;
        if( fld->name() == "list" )
	{	
	    el = fld->childAdd("el");
	    if(id) el->attr("id",id);
	}
	if( len && len < val.size() )
	    el->text( val.substr(val.size()-len,len) );
	else el->text(val);
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
	    s_v = TSYS::int2str(val,C_INT_OCT);
	else if( fld->attr("tp") == "hex" || fld->attr("tp") == "time" ) 
	    s_v = TSYS::int2str(val,C_INT_HEX);
	else         	               
	    s_v = TSYS::int2str(val,C_INT_DEC);
	    
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
    char t_br;
    
    string s_br = TSYS::strEncode( TSYS::pathLev(path,lev,false),TSYS::Path);
    if( s_br.size() )	t_br = s_br[0];
    else	t_br = ' ';
        
    if( t_br == 'd' )		ctrAt1(s_br.substr(1)).at().cntrCmd(path,opt,cmd,lev+1);
    else if( t_br == 's' )      ctrAt(s_br.substr(1)).cntrCmd(path,opt,cmd,lev+1);
    else
    {
	if(cmd == Info)	
	{	
	    opt->clean();
	    cntrCmd_(s_br,opt,TCntrNode::Info);
        }
        else if(cmd == Get)
	    cntrCmd_(s_br,opt,TCntrNode::Get);
        else if(cmd == Set)
	    cntrCmd_(s_br,opt,TCntrNode::Set);
    }
}

//***********************************************************
//*********** Resource section ******************************
//***********************************************************
void TCntrNode::nodeEn()
{ 
    if( m_mod == Enable )	throw TError(nodeName().c_str(),"Node already enabled!");
    if( m_mod != Disable )	throw TError(nodeName().c_str(),"Node already in process!");
    
    ResAlloc res(hd_res,true);
    m_mod = MkEnable;
    res.release();
    
    preEnable();
    
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	for( unsigned i_o = 0; i_o < chGrp[i_g].size(); i_o++ )
    	    if( chGrp[i_g][i_o]->nodeMode() == Disable )
    		chGrp[i_g][i_o]->nodeEn();
		
    res.request(true);	    
    m_mod = Enable;
    res.release();
    
    postEnable();
};

void TCntrNode::nodeDis(long tm, int flag)
{ 
    if( m_mod == Disable )	throw TError(nodeName().c_str(),"Node already disabled!");
    if( m_mod != Enable )	throw TError(nodeName().c_str(),"Node already in process!");
    
    preDisable(flag);

    ResAlloc res(hd_res,true);    
    m_mod = MkDisable;
    res.release();
    
    try
    {
	for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	    for( int i_o = chGrp[i_g].size()-1; i_o >= 0; i_o-- )
	    if( chGrp[i_g][i_o]->nodeMode() == Enable )
		chGrp[i_g][i_o]->nodeDis(tm);
	//Wait of free node	
	time_t t_cur = time(NULL);
	while(1)
	{
	    if( !m_use )	break;
#if OSC_DEBUG
            Mess->put("DEBUG",TMess::Info,"%s: Wait of free %d users!",nodeName().c_str(),m_use);
#endif	    
	    //Check timeout
	    if( tm && time(NULL) > t_cur+tm)
		throw TError(nodeName().c_str(),"Timeouted of wait!");
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

unsigned TCntrNode::grpAdd( )
{
    chGrp.push_back( vector<TCntrNode*>() );

    return chGrp.size()-1;
}

void TCntrNode::chldList( unsigned igr, vector<string> &list )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodeName().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodeName().c_str(),"Node is disabled!");

    list.clear();
    for( unsigned i_o = 0; i_o < chGrp[igr].size(); i_o++ )
	if( chGrp[igr][i_o]->nodeMode() != Disable )
	    list.push_back( chGrp[igr][i_o]->nodeName() );
}

bool TCntrNode::chldAvoid( unsigned igr, const string &name )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodeName().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodeName().c_str(),"Node is disabled!");
    
    for( unsigned i_o = 0; i_o < chGrp[igr].size(); i_o++ )
	if( chGrp[igr][i_o]->nodeName() == name )	return true;
    
    return false;
}

void TCntrNode::chldAdd( unsigned igr, TCntrNode *node, int pos )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodeName().c_str(),"Group of childs <%d> error!",igr);
    if( nodeMode() != Enable ) 	throw TError(nodeName().c_str(),"Node is not enabled!");
    
    if( TSYS::strEmpty( node->nodeName() ) )
    {
	delete node;
        throw TError(nodeName().c_str(),"Add child id is empty!");
    }
        
    //check already avoid object    
    for( unsigned i_o = 0; i_o < chGrp[igr].size(); i_o++ )
	if( chGrp[igr][i_o]->nodeName() == node->nodeName() )
	    throw TError(nodeName().c_str(),"Child <%s> already avoid!",node->nodeName().c_str());
    res.release();
    
    res.request(true);
    if( pos >= chGrp[igr].size() || pos < 0 )	chGrp[igr].push_back( node );
    else chGrp[igr].insert( chGrp[igr].begin()+pos, node );    
    node->m_prev = this;
    res.release();
    
    if( node->nodeMode() == Disable )	node->nodeEn();    
}

void TCntrNode::chldDel( unsigned igr, const string &name, long tm, int flag )
{
    if( tm < 0 )	tm = dtm;
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodeName().c_str(),"Group of childs <%d> error!",igr);
    if( nodeMode() != Enable )	throw TError(nodeName().c_str(),"Node is not enabled!");    
    
    //Check avoid object
    TCntrNode *nd = NULL;    
    for( unsigned i_o = 0; i_o < chGrp[igr].size(); i_o++ )
        if( chGrp[igr][i_o]->nodeName() == name )
	    nd = chGrp[igr][i_o];
    res.release();	    
    if( nd )
    {
	if( nd->nodeMode() && Enable ) nd->nodeDis(tm,flag);
	res.request(true);
	for( unsigned i_o = 0; i_o < chGrp[igr].size(); i_o++ )
	    if( chGrp[igr][i_o]->nodeName() == name )
	    {
		delete chGrp[igr][i_o];
        	chGrp[igr].erase(chGrp[igr].begin()+i_o);			    
	    }
	res.release();    
	return;    			     
    }
    throw TError(nodeName().c_str(),"Child <%s> no avoid!", name.c_str());
}

unsigned TCntrNode::nodeUse(  )
{
    ResAlloc res(hd_res,false);
    if( nodeMode() == Disable )	throw TError(nodeName().c_str(),"Node is disabled!");
    
    unsigned i_use = m_use;
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	for( unsigned i_o = 0; i_o < chGrp[i_g].size(); i_o++ )
	    if( chGrp[i_g][i_o]->nodeMode() != Disable )
		i_use+=chGrp[i_g][i_o]->nodeUse();
		
    return i_use;
}

string TCntrNode::nodePath()
{
    if( m_prev )return m_prev->nodePath()+nodePref()+nodeName()+"/";
    else 	return "/"+nodePref()+nodeName()+"/";
}

TCntrNode *TCntrNode::nodePrev()
{ 
    if( m_prev ) return m_prev; 
    throw TError(("NODE/"+nodeName()).c_str(),"Node is it root or no connect!");
}

AutoHD<TCntrNode> TCntrNode::chldAt( unsigned igr, const string &name, const string &user )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() ) 	throw TError(nodeName().c_str(),"Group of childs <%d> error!",igr);
    if( nodeMode() == Disable )	throw TError(nodeName().c_str(),"Node is disabled!");
    
    //Check avoid object
    for( unsigned i_o = 0; i_o < chGrp[igr].size(); i_o++ )
    	if( chGrp[igr][i_o]->nodeName() == name && chGrp[igr][i_o]->nodeMode() != Disable )
	    return AutoHD<TCntrNode>(chGrp[igr][i_o],user);
    throw TError(nodeName().c_str(),"Child <%s> no present or disabled!", name.c_str());
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

XMLNode *TCntrNode::ctrInsNode( const char *n_nd, int pos, XMLNode *nd, const char *req, 
    const char *path, const string &dscr, int perm, int uid, int gid, const char *tp )
{
    int i_lv = 0;
    
    //Check displaing node
    while( TSYS::pathLev(req,i_lv).size() )
	if( TSYS::pathLev(path,i_lv) != TSYS::pathLev(req,i_lv) )
	{
	    m_dummy.clean();
	    return &m_dummy;
	} else i_lv++;
    
    //Go to element
    while( TSYS::pathLev(path,i_lv).size() )
    {
        try{ nd = nd->childGet("id",TSYS::pathLev(path,i_lv) ); }
	catch(TError err)
	{
	    if( TSYS::pathLev(path,i_lv+1).size() )	throw;
	    nd = nd->childIns(pos);
	}
        i_lv++;
    }
    nd->name(n_nd);
    nd->attr("id",TSYS::pathLev(path,i_lv-1));
    nd->attr("dscr",dscr);
    nd->attr("acs",TSYS::int2str(perm,C_INT_OCT));
    nd->attr("own",TSYS::int2str(uid));
    nd->attr("grp",TSYS::int2str(gid));
    nd->attr("tp",tp);
    
    return nd;
}

XMLNode *TCntrNode::ctrMkNode( const char *n_nd, XMLNode *nd, const char *req, 
    const char *path, const string &dscr, int perm, int uid, int gid, const char *tp )
{
    return ctrInsNode( n_nd,-1,nd,req,path,dscr,perm,uid,gid,tp );
}

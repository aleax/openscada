
//OpenSCADA system file: tcntrnode.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
#include "tmess.h"
#include "tcntrnode.h"

//*************************************************
//* TCntrNode                                     *
//*************************************************

//*************************************************
//* Controll scenaries language section           *
TCntrNode::TCntrNode( TCntrNode *iprev ) : m_mod(Disable), m_use(0), m_oi(USHRT_MAX)
{
    prev.node = iprev;
    prev.grp = -1;
}

TCntrNode::~TCntrNode()
{
    nodeDelAll();
}

void TCntrNode::nodeDelAll( )
{
    if( nodeMode() != Disable )     nodeDis();
    
    TMap::iterator p;
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
    	while( (p=chGrp[i_g].elem.begin())!=chGrp[i_g].elem.end() )
	{
	    delete p->second;
	    chGrp[i_g].elem.erase(p);
	}
}

XMLNode *TCntrNode::ctrId( XMLNode *inf, const string &name_id, bool noex )
{
    int l_off = 0;
    string s_el;

    XMLNode *t_node = inf;    
    while(true)
    {
	s_el = TSYS::pathLev(name_id,0,true,&l_off);
	if( s_el.empty() ) return t_node;
	bool ok = false;
	for( unsigned i_f = 0; i_f < t_node->childSize(); i_f++)
	    if( t_node->childGet(i_f)->attr("id") == s_el ) 
	    {
		t_node = t_node->childGet(i_f);
		ok = true;
		break;
	    }
	if( !ok ) break;
    }

    if(noex) return NULL;	
    throw TError("XML","Field id = %s(%s) no present!",name_id.c_str(),s_el.c_str());    
}

void TCntrNode::cntrCmd( XMLNode *opt, int lev, const string &ipath, int off )
{   
    string path = ipath.empty() ? opt->attr("path") : ipath;
    string s_br = TSYS::pathLev(path,lev,true,&off);

    try
    {
	if( !s_br.empty() && s_br[0] != '/' )
	{
	    for( int i_g = 0; i_g < chGrp.size(); i_g++ )
    		if( s_br.substr(0,chGrp[i_g].id.size()) == chGrp[i_g].id )
		{
		    chldAt(i_g,s_br.substr(chGrp[i_g].id.size())).at().cntrCmd(opt,0,path,off);
		    return;
		}
	    //- Go to default thread -
	    if( !chGrp.empty() ) chldAt(0,s_br).at().cntrCmd(opt,0,path,off);
	    return;
	}
	//- Post command to node -
	opt->setAttr("path",s_br);
	cntrCmdProc(opt);
	if( opt->attr("rez") != "0" )
	    throw TError("ContrItfc",_("%s:%s:> Control element <%s> error!"),opt->name().c_str(),path.c_str(),s_br.c_str());
    }
    catch(TError err)
    {
	if( err.cat == "warning" )	opt->setAttr("rez","1");
	else opt->setAttr("rez","2");
	opt->childClean();
	opt->setAttr("mcat",err.cat);
	opt->setText(err.mess);	
    }
    opt->setAttr("path",path);
}

//*************************************************
//* Resource section                              *
void TCntrNode::nodeEn( int flag )
{ 
    if( m_mod == Enable )	throw TError(nodePath().c_str(),"Node already enabled!");
    if( m_mod != Disable )	throw TError(nodePath().c_str(),"Node already in process!");
    
    ResAlloc res(hd_res,true);
    m_mod = MkEnable;
    res.release();
    
    preEnable(flag);

    TMap::iterator p;
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
	for( p=chGrp[i_g].elem.begin(); p!=chGrp[i_g].elem.end(); p++ )	
	    if( p->second->nodeMode() == Disable )
		p->second->nodeEn(flag);
		
    res.request(true);	    
    m_mod = Enable;
    res.release();
    
    postEnable(flag);
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
	TMap::iterator p;
	for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
    	    for( p=chGrp[i_g].elem.begin(); p!=chGrp[i_g].elem.end(); p++ )	
    		if( p->second->nodeMode() == Enable )
		    p->second->nodeDis(tm,flag);
	
	//- Wait of free node -
	time_t t_cur = time(NULL);
	while(1)
	{
	    if( !m_use )	break;
#if OSC_DEBUG
            mess_debug(nodePath().c_str(),_("Wait of free %d users!"),m_use);
#endif	    
	    //Check timeout
	    if( tm && time(NULL) > t_cur+tm)
	    {
		if( !TSYS::finalKill )
		    throw TError(nodePath().c_str(),_("Timeouted of wait. Object used by %d users. Free object first!"),m_use);
		mess_err(nodePath().c_str(),_("Blocking node error. Inform developpers please!"));
		break;
	    }
	    usleep(STD_WAIT_DELAY*1000);	    
	}
        res.request(true);
	m_mod = Disable;
	res.release();			       
    }catch(TError err)
    {	
	mess_warning(err.cat.c_str(),_("Node disable error. Restore node enabling."));
	res.request(true);
	m_mod = Disable;
	res.release();
	nodeEn(NodeRestore|(flag<<8));
	throw;
    }   
    try{ postDisable(flag); } 
    catch(TError err)	{ mess_warning(err.cat.c_str(),err.mess.c_str()); }
};

void TCntrNode::nodeList(vector<string> &list, const string& gid)
{
    vector<string> tls;
    list.clear();
    for( int i_gr = 0; i_gr < chGrp.size(); i_gr++ )
	if( gid.empty() || gid == chGrp[i_gr].id )
	{
	    chldList(i_gr,tls);
	    for( int i_l = 0; i_l < tls.size(); i_l++ )
		list.push_back(chGrp[i_gr].id+tls[i_l]);
	    if( !gid.empty() )	break;
	}
}

AutoHD<TCntrNode> TCntrNode::nodeAt( const string &path, int lev, char sep, int off )
{
    string s_br = sep ? TSYS::strDecode(TSYS::strSepParse(path,lev,sep,&off),TSYS::PathEl) :
			TSYS::pathLev(path,lev,true,&off);
    if( s_br.empty() )
    {	
	if( nodeMode() == Disable ) throw TError(nodePath().c_str(),"Node is disabled!");
	return this;
    }
    ResAlloc res(hd_res,false);
    for( int i_g = 0; i_g < chGrp.size(); i_g++ )
        if( s_br.substr(0,chGrp[i_g].id.size()) == chGrp[i_g].id )
            return chldAt(i_g,s_br.substr(chGrp[i_g].id.size())).at().nodeAt(path,0,sep,off);
    //Go to default group
    if( chGrp.size() )	return chldAt(0,s_br).at().nodeAt(path,0,sep,off);
}

void TCntrNode::nodeDel( const string &path, char sep, int flag )
{
    AutoHD<TCntrNode> del_n = nodeAt(path,0,sep);
    int n_grp = del_n.at().prev.grp;
    string n_id  = del_n.at().nodeName();
    del_n = AutoHD<TCntrNode>(del_n.at().prev.node);
    del_n.at().chldDel(n_grp,n_id,-1,flag);
}

unsigned TCntrNode::grpAdd( const string &iid, bool iordered )
{
    int g_id;
    for( g_id = 0; g_id < chGrp.size(); g_id++ )
	if( chGrp[g_id].id == iid ) break;
    if( g_id == chGrp.size() )	chGrp.push_back( GrpEl() );
    chGrp[g_id].id = iid;
    chGrp[g_id].ordered = iordered;

    return g_id;
}

void TCntrNode::chldList( unsigned igr, vector<string> &list )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");

    list.clear();
    if(!chGrp[igr].ordered)
    {
	for( TMap::iterator p=chGrp[igr].elem.begin(); p!=chGrp[igr].elem.end(); p++ )	
	    if( p->second->nodeMode() != Disable )
		list.push_back(p->first);
    }		
    else
    {
	for( TMap::iterator p=chGrp[igr].elem.begin(); p!=chGrp[igr].elem.end(); p++ )
	    if( p->second->nodeMode() != Disable )
	    {
		while(p->second->m_oi>=list.size())	list.push_back("");
		list[p->second->m_oi]=p->first;
	    }
    }
}

bool TCntrNode::chldPresent( unsigned igr, const string &name )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");
    
    TMap::iterator p=chGrp[igr].elem.find(name);
    if(p!=chGrp[igr].elem.end()) return true;
    
    return false;
}

void TCntrNode::chldAdd( unsigned igr, TCntrNode *node, int pos )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() != Enable ) 	throw TError(nodePath().c_str(),"Node is not enabled!");
    
    TMap::iterator p;
    if( TSYS::strEmpty(node->nodeName()) || 
	(p=chGrp[igr].elem.find(node->nodeName())) != chGrp[igr].elem.end() )
    {
	delete node;
        throw TError(nodePath().c_str(),"Add child id is empty or already present!");
    }
        
    res.request(true);
    node->prev.node = this;
    node->prev.grp = igr;
    if(chGrp[igr].ordered)
    {
	pos = (pos<0||pos>chGrp[igr].elem.size())?chGrp[igr].elem.size():pos;
	node->m_oi = pos;
	for( p = chGrp[igr].elem.begin(); p != chGrp[igr].elem.end(); p++ )
	    if( p->second->m_oi >= pos ) p->second->m_oi++;
    }
    chGrp[igr].elem.insert(std::pair<string,TCntrNode*>(node->nodeName(),node));
    res.release();
    
    if( node->nodeMode() == Disable )	node->nodeEn(TCntrNode::NodeConnect);
}

void TCntrNode::chldDel( unsigned igr, const string &name, long tm, int flag )
{
    if( tm < 0 )	tm = DEF_TIMEOUT;
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() )	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() != Enable )	throw TError(nodePath().c_str(),"Node is not enabled!");    
    
    TMap::iterator p=chGrp[igr].elem.find(name);
    if(p==chGrp[igr].elem.end())
	throw TError(nodePath().c_str(),"Child <%s> no present!", name.c_str());
    
    if( p->second->nodeMode() && Enable ) p->second->nodeDis(tm,flag);
    res.request(true);
    if(chGrp[igr].ordered)
    {
	int pos = p->second->m_oi;
	for( TMap::iterator p1=chGrp[igr].elem.begin(); p1!=chGrp[igr].elem.end(); p1++ )
    	    if( p1->second->m_oi > pos ) p1->second->m_oi--;
    }
    delete p->second;
    chGrp[igr].elem.erase(p);
}

unsigned TCntrNode::nodeUse(  )
{
    ResAlloc res(hd_res,false);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");
    
    unsigned i_use = m_use;
    TMap::iterator p;
    for( unsigned i_g = 0; i_g < chGrp.size(); i_g++ )
     for( p=chGrp[i_g].elem.begin(); p!=chGrp[i_g].elem.end(); p++ )	
	if( p->second->nodeMode() != Disable )
	    i_use+=p->second->nodeUse();
		
    return i_use;
}

string TCntrNode::nodePath( char sep, bool from_root )
{
    if( sep )
    {
	if( prev.node ) 
	{
	    if( from_root && !prev.node->prev.node )
		return ((prev.grp<0)?"":prev.node->chGrp[prev.grp].id)+nodeName();
	    else
	        return prev.node->nodePath(sep,from_root)+string(1,sep)+
			((prev.grp<0)?"":prev.node->chGrp[prev.grp].id)+nodeName();
	}
	else return nodeName();
    }	
    else 
    {
	if( prev.node )
    	    return prev.node->nodePath(sep,from_root)+
		    ((prev.grp<0)?"":prev.node->chGrp[prev.grp].id)+nodeName()+"/";
	else return from_root?"/":("/"+nodeName()+"/");
    }
}

TCntrNode *TCntrNode::nodePrev( bool noex )
{ 
    if( prev.node ) return prev.node;
    if( noex )	return NULL;
    throw TError(nodePath().c_str(),"Node is it root or no connect!");
}

AutoHD<TCntrNode> TCntrNode::chldAt( unsigned igr, const string &name, const string &user )
{
    ResAlloc res(hd_res,false);
    if( igr >= chGrp.size() ) 	throw TError(nodePath().c_str(),"Group of childs %d error!",igr);
    if( nodeMode() == Disable )	throw TError(nodePath().c_str(),"Node is disabled!");

    TMap::iterator p=chGrp[igr].elem.find(name);
    if(p == chGrp[igr].elem.end() || p->second->nodeMode() == Disable)
	throw TError(nodePath().c_str(),_("Element <%s> no present or disabled!"), name.c_str());

    return AutoHD<TCntrNode>(p->second,user);
}

void TCntrNode::AHDConnect()
{
    conn_res.resRequestW( );
    m_use++;
    conn_res.resReleaseW( );
}

void TCntrNode::AHDDisConnect()
{
    conn_res.resRequestW( );
    m_use--;
    conn_res.resReleaseW( );
}

XMLNode *TCntrNode::ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *path, const string &dscr,
    int perm, const char *user, const char *grp, int n_attr, ... )
{
    int woff = 0;
    string req = nd->attr("path");
    string reqt, reqt1;
    
    //- Check displaing node -
    int itbr = 0;
    for( int i_off = 0, i_off1 = 0; (reqt=TSYS::pathLev(req,0,true,&i_off)).size(); woff=i_off )
	if( reqt != (reqt1=TSYS::pathLev(path,0,true,&i_off1)) )
	{
	    if( !reqt1.empty() ) return NULL;
	    itbr = 1;
	    break;
	}    
    
    //- Check permission -
    char n_acs = SYS->security().at().access(nd->attr("user"),SEQ_RD|SEQ_WR|SEQ_XT,user,grp,perm);
    if( !(n_acs&SEQ_RD) ) return NULL;
    if( itbr )	return nd; 
    
    XMLNode *obj = nd;
    if( obj->name() == "info" )	obj = nd->childGet(0,true);
    if( !obj )	
    {
	obj = nd->childAdd();
	nd->setAttr("rez","0");
    }
    
    //- Go to element -    
    for( ;(reqt=TSYS::pathLev(path,0,true,&woff)).size(); reqt1=reqt )
    {
        XMLNode *obj1 = obj->childGet("id",reqt,true);
	if( obj1 ) { obj = obj1; continue; }
	//int wofft = woff;
	if( TSYS::pathLev(path,0,true,&woff).size() )	
	    throw TError("ContrItfc",_("Some tags on path <%s> missed!"),req.c_str());
	obj = obj->childIns(pos);
    }
    obj->setName(n_nd);
    obj->setAttr("id",reqt1);
    obj->setAttr("dscr",dscr);
    obj->setAttr("acs",TSYS::int2str(n_acs));
    
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
	    obj->setAttr(atr_id,atr_vl);
	}
	va_end(argptr);
    }
    
    return obj;
}

bool TCntrNode::ctrChkNode( XMLNode *nd, const char *cmd, int perm, const char *user, const char *grp, char mode, const char *warn )
{
    if( nd->name() != cmd ) return false;
    if( ((char)perm&mode) != mode && SYS->security().at().access(nd->attr("user"),mode,user,grp,perm) != mode )
	throw TError("ContrItfc",_("Error access to element <%s>!"),nd->attr("path").c_str());
    if( warn && !atoi(nd->attr("force").c_str()) )
	throw TError("warning",_("Element <%s> warning! %s"),nd->attr("path").c_str(),warn);
    nd->setAttr("rez","0");
    return true;
}

void TCntrNode::cntrCmdProc( XMLNode *opt )
{
    if( opt->name() == "info" )
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Node: ")+nodeName());
}


//OpenSCADA system module Special.FLibSYS file: xmlfnc.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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
#include <string.h>

#include "xmlfnc.h"

using namespace FLibSYS;

//*************************************************
//* XMLNodeObj - XML node object                  *
//*************************************************
XMLNodeObj::XMLNodeObj( const string &name ) : mName(name)
{

}

XMLNodeObj::~XMLNodeObj( )
{
    while( childSize() ) childDel(0);
}

void XMLNodeObj::childAdd( XMLNodeObj *nd )
{
    mChilds.push_back(nd);
    nd->connect();
}

void XMLNodeObj::childIns( unsigned id, XMLNodeObj *nd )
{
    if( id < 0 ) id = mChilds.size();
    id = vmin(id,mChilds.size());
    mChilds.insert(mChilds.begin()+id,nd);
    nd->connect();
}

void XMLNodeObj::childDel( unsigned id )
{
    if( id < 0 || id >= mChilds.size() ) throw TError("XMLNodeObj",_("Deletion child '%d' error."),id);
    if( !mChilds[id]->disconnect() ) delete mChilds[id];
    mChilds.erase(mChilds.begin()+id);
}

XMLNodeObj *XMLNodeObj::childGet( unsigned id )
{
    if( id < 0 || id >= mChilds.size() ) throw TError("XMLNodeObj",_("Child '%d' is not allow."),id);
    return mChilds[id];
}

string XMLNodeObj::getStrXML( const string &oid )		{ return ""; }

TVariant XMLNodeObj::funcCall( const string &id, vector<TVariant> &prms )
{
    if( id == "name" )		{ return name(); }
    if( id == "text" )		{ return text(); }
    if( id == "attr" && prms.size() )		{ return propGet(prms[0].getS()).getS(); }
    if( id == "setName" && prms.size() )	{ setName(prms[0].getS()); return this; }
    if( id == "setText" && prms.size() )	{ setText(prms[0].getS()); return this; }
    if( id == "setAttr" && prms.size() >= 2 )	{ propSet(prms[0].getS(),prms[1].getS()); return this; }
    if( id == "childSize" )	{ return childSize(); }
    if( id == "childAdd" )
    {
	XMLNodeObj *no = NULL;
	if( prms.size() && prms[0].type() == TVariant::Object && dynamic_cast<XMLNodeObj*>(prms[0].getO()) ) no = (XMLNodeObj*)prms[0].getO();
	else if( prms.size() ) no = new XMLNodeObj(prms[0].getS());
	else no = new XMLNodeObj();
	childAdd( no );
	return no;
    }
    if( id == "childIns" && prms.size() )
    {
	XMLNodeObj *no = NULL;
	if( prms.size() > 1 && prms[1].type() == TVariant::Object && dynamic_cast<XMLNodeObj*>(prms[1].getO()) ) no = (XMLNodeObj*)prms[1].getO();
	else if( prms.size() > 1 ) no = new XMLNodeObj(prms[1].getS());
	else no = new XMLNodeObj();
	childIns( prms[0].getI(), no );
	return no;
    }
    if( id == "childDel" && prms.size() )	{ childDel(prms[0].getI()); return this; }
    if( id == "childGet" && prms.size() )	{ return childGet(prms[0].getI()); }
    if( id == "load" && prms.size() )
    {
	XMLNode nd;
	//> Load from file
	if( prms.size() >= 2 && prms[1].getB() )
	{
	    int hd = open(prms[0].getS().c_str(),O_RDONLY);
	    if( hd < 0 ) return TSYS::strMess(_("2:Open file <%s> error: %s"),prms[0].getS().c_str(),strerror(errno));
	    int cf_sz = lseek(hd,0,SEEK_END);
	    lseek(hd,0,SEEK_SET);
	    char *buf = (char *)malloc(cf_sz+1);
	    read(hd,buf,cf_sz);
	    buf[cf_sz] = 0;
	    close(hd);
	    string s_buf = buf;
	    free(buf);
	    try{ nd.load(s_buf); } 
	    catch( TError err ) { return "1:"+err.mess; }
	}
	//> Load from string
	else
	    try{ nd.load(prms[0].getS()); } 
	    catch( TError err ) { return "1:"+err.mess; }
	fromXMLNode(nd);
	return string("0");
    }
    if( id == "save" )
    {
	XMLNode nd;
	toXMLNode(nd);
	string s_buf = nd.save( (prms.size()>=1)?prms[0].getI():0 );
	//> Save to file
	if( prms.size() >= 2 )
	{
	    int hd = open( prms[1].getS().c_str(), O_RDWR|O_CREAT|O_TRUNC, 0664 );
	    if( hd < 0 ) return string("");
	    write(hd,s_buf.data(),s_buf.size());
	}
	return s_buf;
    }

    throw TError("XMLNodeObj",_("Function '%s' error or not enough parameters."),id.c_str());
}

void XMLNodeObj::toXMLNode( XMLNode &nd )
{
    nd.clear();
    nd.setName(name())->setText(text());
    for( map<string,TVariant>::iterator ip = mProps.begin(); ip != mProps.end(); ip++ )
	nd.setAttr(ip->first,ip->second.getS());
    for( int i_ch = 0; i_ch < mChilds.size(); i_ch++ )
	mChilds[i_ch]->toXMLNode(*nd.childAdd());
}

void XMLNodeObj::fromXMLNode( XMLNode &nd )
{
    while( childSize() ) childDel(0);

    setName(nd.name());
    setText(nd.text());

    vector<string> alst;
    nd.attrList( alst );
    for( int i_a = 0; i_a < alst.size(); i_a++ )
	propSet(alst[i_a],nd.attr(alst[i_a]));

    for( int i_ch = 0; i_ch < nd.childSize(); i_ch++ )
    {
	XMLNodeObj *xn = new XMLNodeObj();
	childAdd(xn);
	xn->fromXMLNode(*nd.childGet(i_ch));
    }
}

//*************************************************
//* Control request                               *
//*************************************************
void xmlCntrReq::calc( TValFunc *val )
{
    try
    {
	XMLNode req;
	if( !dynamic_cast<XMLNodeObj*>(val->getO(1)) ) { val->setS(0,_("1:Request is not object!")); return; }
	((XMLNodeObj*)val->getO(1))->toXMLNode(req);
	string path = req.attr("path");
	if( val->getS(2).empty() )
	{
	    req.setAttr("user",val->user());
	    SYS->cntrCmd(&req);
	}
	else
	{
	    req.setAttr("path",val->getS(2)+"/"+path);
	    SYS->transport().at().cntrIfCmd(req,"xmlCntrReq",val->user());
	    req.setAttr("path",path);
	}
	((XMLNodeObj*)val->getO(1))->fromXMLNode(req);
	val->setS(0,"0");
    }catch(TError err){ val->setS(0,TSYS::strMess(_("1:Request error: %s"),err.mess.c_str())); }
}


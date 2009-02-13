
//OpenSCADA system module UI.VCAEngine file: vcaFuncs.h
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

#ifndef VCAFUNCS_H
#define VCAFUNCS_H

#include <tfunction.h>

#include "vcaengine.h"

namespace VCA
{
//*************************************************
//* Widget's list                                 *
//*************************************************
class wdgList : public TFunction
{
    public:
	wdgList( ) : TFunction("WdgList")
	{
	    ioAdd( new IO("list",_("List"),IO::String,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    ioAdd( new IO("pg",_("Pages"),IO::Boolean,IO::Default,"0") );
	    setStart(true);
	}

	string name( )	{ return _("Widgets list"); }
	string descr( )	{ return _("Return widgets list into widget container or child list. If set <pg> then return pages list for projects and sessions."); }

	void calc( TValFunc *val )
	{
	    string sls;
	    vector<string> ls;

	    try
	    {
		AutoHD<TCntrNode> nd = nodePrev()->nodeAt(val->getS(1));
		if( dynamic_cast<Session*>(&nd.at()) && val->getB(2) )		((Session*)&nd.at())->list(ls);
		else if( dynamic_cast<SessPage*>(&nd.at()) && val->getB(2) )	((SessPage*)&nd.at())->pageList(ls);
		else if( dynamic_cast<Project*>(&nd.at()) && val->getB(2) )	((Project*)&nd.at())->list(ls);
		else if( dynamic_cast<Page*>(&nd.at()) && val->getB(2) )	((Page*)&nd.at())->pageList(ls);
		else if( dynamic_cast<Widget*>(&nd.at()) && !val->getB(2) )	((Widget*)&nd.at())->wdgList(ls);
	    }
	    catch(TError err) { }
	    for( int i_l = 0; i_l < ls.size(); i_l++ ) sls += ls[i_l]+"\n";
	    val->setS(0,sls);
	}
};

//*************************************************
//* Node present. Include widgets, attributes and *
//* other objects.                                *
//*************************************************
class nodePresent : public TFunction
{
    public:
	nodePresent( ) : TFunction("NodePresent")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Boolean,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Node present"); }
	string descr( )	{ return _("Checking for node, include widgets, attributes and other present."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		nodePrev()->nodeAt(val->getS(1));
		val->setB(0,true);
	    }
	    catch(TError err) { val->setB(0,false); }
	}
};

//*************************************************
//* Attr's list                                 *
//*************************************************
class attrList : public TFunction
{
    public:
	attrList( ) : TFunction("AttrList")
	{
	    ioAdd( new IO("list",_("List"),IO::String,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    ioAdd( new IO("noUser",_("Without user's"),IO::Boolean,IO::Default,"1") );
	    setStart(true);
	}

	string name( )	{ return _("Attributes list"); }
	string descr( )	{ return _("Return attributes list for widget. If set <noUser> then return only not user's attributes."); }

	void calc( TValFunc *val )
	{
	    string sls;
	    vector<string> ls;
	    try
	    {
		AutoHD<Widget> nd = nodePrev()->nodeAt(val->getS(1));
		nd.at().attrList(ls);
		if( val->getB(2) )
		    for( int i_a = 0; i_a < ls.size(); i_a++ )
			if( nd.at().attrAt(ls[i_a]).at().flgGlob()&Attr::IsUser )
			{ ls.erase(ls.begin()+i_a); i_a--; }
	    }
	    catch(TError err) { }
	    for( int i_a = 0; i_a < ls.size(); i_a++ ) sls += ls[i_a]+"\n";
	    val->setS(0,sls);
	}
};

//*************************************************
//* Attr get, string                              *
//*************************************************
class attrGetS : public TFunction
{
    public:
	attrGetS( ) : TFunction("AttrGetS")
	{
	    ioAdd( new IO("val",_("Value"),IO::String,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute get, string"); }
	string descr( )	{ return _("Getting widget's attribute by string value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(1));
		val->setS(0,nd.at().getS());
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr get, integer                             *
//*************************************************
class attrGetI : public TFunction
{
    public:
	attrGetI( ) : TFunction("AttrGetI")
	{
	    ioAdd( new IO("val",_("Value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute get, integer"); }
	string descr( )	{ return _("Getting widget's attribute by integer value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(1));
		val->setI(0,nd.at().getI());
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr get, real                                *
//*************************************************
class attrGetR : public TFunction
{
    public:
	attrGetR( ) : TFunction("AttrGetR")
	{
	    ioAdd( new IO("val",_("Value"),IO::Real,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute get, real"); }
	string descr( )	{ return _("Getting widget's attribute by real value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(1));
		val->setR(0,nd.at().getR());
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr get, boolean                             *
//*************************************************
class attrGetB : public TFunction
{
    public:
	attrGetB( ) : TFunction("AttrGetB")
	{
	    ioAdd( new IO("val",_("Value"),IO::Boolean,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute get, boolean"); }
	string descr( )	{ return _("Getting widget's attribute by boolean value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(1));
		val->setB(0,nd.at().getB());
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr set, string                              *
//*************************************************
class attrSetS : public TFunction
{
    public:
	attrSetS( ) : TFunction("AttrSetS")
	{
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::String,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute set, string"); }
	string descr( )	{ return _("Setup widget's attribute to string value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(0));
		if( !(nd.at().flgGlob()&TFld::NoWrite) ) nd.at().setS(val->getS(1));
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr set, integer                             *
//*************************************************
class attrSetI : public TFunction
{
    public:
	attrSetI( ) : TFunction("AttrSetI")
	{
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Integer,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute set, integer"); }
	string descr( )	{ return _("Setup widget's attribute to integer value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(0));
		if( !(nd.at().flgGlob()&TFld::NoWrite) ) nd.at().setI(val->getI(1));
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr set, real                                *
//*************************************************
class attrSetR : public TFunction
{
    public:
	attrSetR( ) : TFunction("AttrSetR")
	{
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Real,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute set, real"); }
	string descr( )	{ return _("Setup widget's attribute to real value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(0));
		if( !(nd.at().flgGlob()&TFld::NoWrite) ) nd.at().setR(val->getR(1));
	    }
	    catch(TError err) { }
	}
};

//*************************************************
//* Attr set, boolean                             *
//*************************************************
class attrSetB : public TFunction
{
    public:
	attrSetB( ) : TFunction("AttrSetB")
	{
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Boolean,IO::Default) );
	    setStart(true);
	}

	string name( )	{ return _("Attribute set, boolean"); }
	string descr( )	{ return _("Setup widget's attribute to boolean value."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		AutoHD<Attr> nd = nodePrev()->nodeAt(val->getS(0));
		if( !(nd.at().flgGlob()&TFld::NoWrite) ) nd.at().setB(val->getB(1));
	    }
	    catch(TError err) { }
	}
};

} 

#endif //VCAFUNCS_H

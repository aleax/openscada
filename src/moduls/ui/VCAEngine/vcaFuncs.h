
//OpenSCADA module UI.VCAEngine file: vcaFuncs.h
/***************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
//* Session's user                                *
//*************************************************
class sesUser : public TFunction
{
    public:
	sesUser( ) : TFunction("SesUser") {
	    ioAdd(new IO("user",_("User"),IO::String,IO::Return));
	    ioAdd(new IO("addr",_("Address"),IO::String,IO::Default));
	    setStart(true);
	}

	string name( )	{ return _("Session user"); }
	string descr( )	{ return _("Returns the session user by the session widget path."); }

	void calc( TValFunc *val ) {
	    try {
		string sses = TSYS::pathLev(val->getS(1),0,true);
		if(sses.substr(0,4) == "ses_") { val->setS(0,mod->sesAt(sses.substr(4)).at().user()); return; }
	    }
	    catch(TError err) { }
	    val->setS(0,"");
	}
};

//*************************************************
//* Widget's list                                 *
//*************************************************
class wdgList : public TFunction
{
    public:
	wdgList( ) : TFunction("WdgList") {
	    ioAdd(new IO("list",_("List"),IO::Object,IO::Return));
	    ioAdd(new IO("addr",_("Address"),IO::String,IO::Default));
	    ioAdd(new IO("pg",_("Pages"),IO::Boolean,IO::Default,"0"));
	    setStart(true);
	}

	string name( )	{ return _("Widgets list"); }
	string descr( )	{ return _("Returns a list of the widgets, into the widgets container, or the child list. If set <pg> then returns the pages list for projects and sessions."); }

	void calc( TValFunc *val ) {
	    TArrayObj *rez = new TArrayObj();
	    vector<string> ls;

	    try {
		AutoHD<TCntrNode> nd = nodePrev()->nodeAt(val->getS(1));
		if(dynamic_cast<Session*>(&nd.at()) && val->getB(2))		((Session*)&nd.at())->list(ls);
		else if(dynamic_cast<SessPage*>(&nd.at()) && val->getB(2))	((SessPage*)&nd.at())->pageList(ls);
		else if(dynamic_cast<Project*>(&nd.at()) && val->getB(2))	((Project*)&nd.at())->list(ls);
		else if(dynamic_cast<Page*>(&nd.at()) && val->getB(2))		((Page*)&nd.at())->pageList(ls);
		else if(dynamic_cast<WidgetLib*>(&nd.at()) && !val->getB(2))	((WidgetLib*)&nd.at())->list(ls);
		else if(dynamic_cast<Widget*>(&nd.at()) && !val->getB(2))	((Widget*)&nd.at())->wdgList(ls);
	    }
	    catch(TError err) { }
	    for(unsigned i_l = 0; i_l < ls.size(); i_l++) rez->arSet(i_l, ls[i_l]);
	    val->setO(0, rez);
	}
};

//*************************************************
//* Node present. Include widgets, attributes and *
//* other objects.                                *
//*************************************************
class nodePresent : public TFunction
{
    public:
	nodePresent( ) : TFunction("NodePresent") {
	    ioAdd(new IO("rez",_("Result"),IO::Boolean,IO::Return));
	    ioAdd(new IO("addr",_("Address"),IO::String,IO::Default));
	    setStart(true);
	}

	string name( )	{ return _("Node presence"); }
	string descr( )	{ return _("Checking for the node presence, including widgets, attributes and other."); }

	void calc( TValFunc *val ) {
	    try {
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
	attrList( ) : TFunction("AttrList") {
	    ioAdd(new IO("list",_("List"),IO::String,IO::Return|IO::FullText));
	    ioAdd(new IO("addr",_("Address"),IO::String,IO::Default));
	    ioAdd(new IO("noUser",_("Not user"),IO::Boolean,IO::Default,"1"));
	    setStart(true);
	}

	string name( )	{ return _("Attributes list"); }
	string descr( )	{ return _("Returns an attributes list of the widget. If set <noUser> then returns only not user attributes."); }

	void calc( TValFunc *val ) {
	    string sls;
	    vector<string> ls;
	    try {
		AutoHD<Widget> nd = nodePrev()->nodeAt(val->getS(1));
		nd.at().attrList(ls);
		if(val->getB(2))
		    for(unsigned iA = 0; iA < ls.size(); )
			if(nd.at().attrAt(ls[iA]).at().flgGlob()&Attr::IsUser) ls.erase(ls.begin()+iA);
			else iA++;
	    } catch(TError err) { }

	    for(unsigned iA = 0; iA < ls.size(); iA++) sls += ls[iA]+"\n";

	    val->setS(0, sls);
	}
};

//*************************************************
//* Attr get                                      *
//*************************************************
class attrGet : public TFunction
{
    public:
	attrGet( ) : TFunction("AttrGet") {
	    ioAdd(new IO("val",_("Value"),IO::String,IO::Return));
	    ioAdd(new IO("addr",_("Address"),IO::String,IO::Default));
	    ioAdd(new IO("attr",_("Attribute"),IO::String,IO::Default));
	    setStart(true);
	}

	string name( )	{ return _("Get the attribute"); }
	string descr( )	{ return _("Getting value of the widget attribute. "
	    "The request can be done as by indicating the full address of the attribute in <addr>, and "
	    "by: indicating separately the address of the widget in <addr> and the the attribute identifier in the <attr>."); }

	void calc( TValFunc *val ) {
	    string a = val->getS(2);
	    string addr = val->getS(1);
	    val->setS(0,EVAL_STR);
	    if(a.empty()) {
		string c_val;
		addr = "";
		for(int i_off = 0; (c_val=TSYS::pathLev(val->getS(1),0,true,&i_off)).size(); )
		{ if(!a.empty()) addr += "/"+a; a = c_val; }
		if(a.size() < 2 || a.substr(0,2) != "a_") return;
		a = a.substr(2);
	    }
	    if(addr.empty() || a.empty()) return;

	    XMLNode req("get");
	    req.setAttr("user",val->user())->setAttr("path",addr+"/%2fattr%2f"+a);
	    mod->cntrCmd(&req);
	    if(!s2i(req.attr("rez"))) val->setS(0, req.text());

	    /*try {
		AutoHD<TCntrNode> nd = nodePrev()->nodeAt(val->getS(1));
		if( dynamic_cast<Attr*>(&nd.at()) )		val->setS(0,((Attr*)&nd.at())->getS());
		else if( dynamic_cast<Widget*>(&nd.at()) )	val->setS(0,((Widget*)&nd.at())->attrAt(val->getS(2)).at().getS());
		else val->setS(0,"");
	    }
	    catch(TError err) { }*/
	}
};

//*************************************************
//* Attr set                                      *
//*************************************************
class attrSet : public TFunction
{
    public:
	attrSet( ) : TFunction("AttrSet") {
	    ioAdd(new IO("addr",_("Address"),IO::String,IO::Default));
	    ioAdd(new IO("val",_("Value"),IO::String,IO::Default));
	    ioAdd(new IO("attr",_("Attribute"),IO::String,IO::Default));
	    setStart(true);
	}

	string name( )	{ return _("Set the attribute"); }
	string descr( )	{ return _("Setting value of the widget attribute. "
	    "Setting can be done as by indicating the full address of the attribute in <addr>, and "
	    "by: indicating separately the address of the widget in <addr> and the the attribute identifier in the <attr>."); }

	void calc( TValFunc *val ) {
	    string a = val->getS(2);
	    string addr = val->getS(0);
	    if(a.empty()) {
		string c_val;
		addr = "";
		for( int i_off = 0; (c_val=TSYS::pathLev(val->getS(0),0,true,&i_off)).size(); )
		{ if(!a.empty()) addr += "/"+a; a = c_val; }
		if( a.size() < 2 || a.substr(0,2) != "a_" ) return;
		a = a.substr(2);
	    }
	    if(addr.empty() || a.empty()) return;

	    XMLNode req("set");
	    req.setAttr("user",val->user())->setAttr("path",addr+"/%2fattr%2f"+a)->setText(val->getS(1));
	    mod->cntrCmd(&req);

	    /*try
	    {
		AutoHD<TCntrNode> nd = nodePrev()->nodeAt(val->getS(0));
		if( dynamic_cast<Attr*>(&nd.at()) )		((Attr*)&nd.at())->setS(val->getS(1));
		else if( dynamic_cast<Widget*>(&nd.at()) )	((Widget*)&nd.at())->attrAt(val->getS(2)).at().setS(val->getS(1));
	    }
	    catch(TError err) { }*/
	}
};

}

#endif //VCAFUNCS_H

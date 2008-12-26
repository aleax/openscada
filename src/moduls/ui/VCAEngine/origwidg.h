
//OpenSCADA system module UI.VCAEngine file: origwidg.h
/***************************************************************************
 *   Copyright (C) 2006-2008 by Roman Savochenko                           *
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

#ifndef ORIGWIDG_H
#define ORIGWIDG_H

#include <string.h>

#include "libwidg.h"

namespace VCA
{

//*************************************************
//* PrWidget: Primitive widget template           *
//*************************************************
class PrWidget : public LWidget
{
    public:
	//Methods
	PrWidget( const string &iid );

	string rootId( )	{ return id(); }
	string ico( );
	string type( )		{ return "Terminator"; }
	string parentNm( )	{ return "root"; }
	string name( )		{ return "Root primitive"; }
	string descr( )		{ return "Abstract root primitive"; }

	void setName( const string &inm )	{ };
	void setDescr( const string &idscr )	{ };
	void setEnable( bool val );

    protected:
	//Methods
	void preDisable( int flag );
	bool cntrCmdGeneric( XMLNode *opt );
	void cntrCmdProc( XMLNode *opt );
};

//*************************************************
//* Original widgets based at primitive           *
//* widget template                               *

//*************************************************
//* OrigElFigure:                                 *
//*  Elementary figures original widget           *
//*************************************************
class OrigElFigure : public PrWidget
{
    public:
	//Methods
	OrigElFigure( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );
};

//*************************************************
//* OrigFormEl: Form element original widget      *
//*************************************************
class OrigFormEl : public PrWidget
{
    public:
	//Methods
	OrigFormEl( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );
};

//************************************************
//* OrigText: Text element original widget       *
//************************************************
class OrigText : public PrWidget
{
    public:
	//Methods
	OrigText( );

	string name( );
	string descr( );

     protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );
};

//************************************************
//* OrigMedia: Media view original widget        *
//************************************************
class OrigMedia : public PrWidget
{
    public:
	//Methods
	OrigMedia( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );
};

//************************************************
//* OrigDiagram: Diagram view original widget    *
//************************************************
class OrigDiagram : public PrWidget
{
    public:
	//Methods
	OrigDiagram( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );
};

//************************************************
//* OrigProtocol: Protocol view original widget  *
//************************************************
class OrigProtocol : public PrWidget
{
    public:
	//Methods
	OrigProtocol( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );
};

//************************************************
//* OrigDocument: Document view original widget  *
//************************************************
class OrigDocument : public PrWidget
{
    public:
	//Methods
	OrigDocument( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );

	string makeDoc( const string &tmpl, Widget *wdg );
	void nodeProcess( XMLNode *xcur, TValFunc &funcV, TFunction &funcIO, const string &iLang, bool instrDel = false );
	void nodeClear( XMLNode *xcur );

	//Attributes
	static const char *XHTML_entity;
};

//************************************************
//* OrigFunction: User function original widget  *
//************************************************
class OrigFunction : public PrWidget
{
    public:
	//Methods
	OrigFunction( );

	string name( );
	string descr( );
};

//************************************************
//* OrigBox: Box original widget                 *
//************************************************
class OrigBox : public PrWidget
{
    public:
	//Methods
	OrigBox( );

	string name( );
	string descr( );

	bool isContainer( )	{ return true; }

    protected:
	//Methods
	void postEnable( int flag );
};

//************************************************
//* OrigLink: Link original widget               *
//************************************************
class OrigLink : public PrWidget
{
    public:
	//Methods
	OrigLink( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
};

}

#endif //ORIGWIDG_H

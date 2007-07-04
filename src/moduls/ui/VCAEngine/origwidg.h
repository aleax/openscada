
//OpenSCADA system module UI.VCAEngine file: origwidg.h
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef ORIGWIDG_H
#define ORIGWIDG_H

#include "libwidg.h"

namespace VCA
{

//*************************************************
//* Primitive widget template                     *
//*************************************************
class PrWidget : public LWidget
{
    public:
	PrWidget( const string &iid );    

	string rootId( )	{ return id(); }
	string ico( );
	string type( )          { return "Terminator"; }
	string parentNm( )	{ return "root"; }
	string name( )     	{ return "Root primitive"; }
	string descr( )    	{ return "Abstract root primitive"; }

	void setName( const string &inm )     { };
	void setDescr( const string &idscr )  { };

    protected:
	void preDisable( int flag );
	void cntrCmdProc( XMLNode *opt );
};

//============ Original widgets based at primitive widget template ============

//*************************************************
//* Elementary figures original widget            *
//*************************************************
class OrigElFigure : public PrWidget
{
    public:
	OrigElFigure( );
	
	string name( );
	string descr( );
    
    protected:
	void postEnable( int flag );
};

//*************************************************
//* Form element original widget                  *
//*************************************************
class OrigFormEl : public PrWidget
{
    public:
	OrigFormEl( );

	string name( );
	string descr( );
    
    protected:
	void postEnable( int flag );
	bool attrChange( Attr &cfg );
};

//************************************************
//* Text element original widget                 *
//************************************************
class OrigText : public PrWidget
{
    public:
      	OrigText( );
    
	string name( );
	string descr( );

     protected:
	void postEnable( int flag );    
}; 

//************************************************
//* Media view original widget                   *
//************************************************
class OrigMedia : public PrWidget
{
    public:
	OrigMedia( );	
    
	string name( );
	string descr( );
 
    protected:
	void postEnable( int flag );
	bool attrChange( Attr &cfg );
};

//************************************************
//* Trend view original widget                   *
//************************************************
class OrigTrend : public PrWidget
{
    public:
    	OrigTrend( );
    
	string name( );
	string descr( );
    
    protected:
	void postEnable( int flag );
};    

//************************************************
//* Protocol view original widget                *
//************************************************
class OrigProtocol : public PrWidget
{
    public:
    	OrigProtocol( );
    
	string name( );
	string descr( );
};

//************************************************
//* Document view original widget                *
//************************************************
class OrigDocument : public PrWidget
{
    public:
    	OrigDocument( );
    
    	string name( );
	string descr( );
};

//************************************************
//* User function original widget                *
//************************************************
class OrigFunction : public PrWidget
{
    public:
    	OrigFunction( );
    
	string name( );
	string descr( );
};

//************************************************
//* User element original widget                 *
//************************************************
class OrigUserEl : public PrWidget
{
    public:
	OrigUserEl( );
    
	string name( );
	string descr( );
    
	bool isContainer( )	{ return true; }
     
    protected:
	void postEnable( int flag );    
};

//************************************************
//* Link original widget                         *
//************************************************
class OrigLink : public PrWidget
{
    public:
    	OrigLink( );
    
	string name( );
	string descr( );
 
    protected:
	void postEnable( int flag );       
};

}

#endif //ORIGWIDG_H

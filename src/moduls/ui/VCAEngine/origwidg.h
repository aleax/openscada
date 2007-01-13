
//OpenSCADA system module UI.VCSEngine file: libwidgbase.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
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

#include "widget.h"
#include "libwidg.h"
		  
namespace VCA
{

//==================================
//=== Elementary figures element ===
//==================================
class OrigElFigure : public Widget
{
    public:
	OrigElFigure( const string &iid, bool lib_loc = true );

	string original()	{ return "ElFigure"; }
		
	void attrProc( Widget *uwdg, bool init = false );
};

//====================
//=== Form element ===
//====================
class OrigFormEl : public Widget
{
    public:
	OrigFormEl( const string &iid, bool lib_loc = true );
	
	string original()       { return "FormEl"; }
	
	void attrProc( Widget *uwdg, bool init = false );
};

//====================
//=== Text element ===
//====================
class OrigText : public Widget
{
    public:
	OrigText( const string &iid, bool lib_loc = true );
	
	string original()       { return "Text"; }
	
	void attrProc( Widget *uwdg, bool init = false );
}; 

//=====================
//=== Media element ===
//=====================
class OrigMedia : public Widget
{
    public:
	OrigMedia( const string &iid, bool lib_loc = true );
	
	string original()       { return "Media"; }
	
	void attrProc( Widget *uwdg, bool init = false );
};  

//=====================
//=== Trend element ===
//=====================
class OrigTrend : public Widget
{
    public:
	OrigTrend( const string &iid, bool lib_loc = true );
	
	string original()       { return "Trend"; }
	
	void attrProc( Widget *uwdg, bool init = false );
};    

//========================
//=== Protocol element ===
//========================
class OrigProtocol : public Widget
{
    public:
	OrigProtocol( const string &iid, bool lib_loc = true );
	
	string original()       { return "Protocol"; }

	void attrProc( Widget *uwdg, bool init = false );
};

//=============================
//=== User function element ===
//=============================
class OrigFunction : public Widget
{
    public:
	OrigFunction( const string &iid, bool lib_loc = true );
	
	string original()       { return "Function"; }
	
	void attrProc( Widget *uwdg, bool init = false );
};

//====================
//=== User element ===
//====================
class OrigUserEl : public Widget
{
    public:
	//Methods
        OrigUserEl( const string & iid, bool lib_loc = true );
	~OrigUserEl( );
	
	string original()       { return "UserEl"; }

	//- User attributes -
	void attrProc( Widget *uwdg, bool init = false );

    protected:
	//void cntrCmdProc( XMLNode *opt );       //Control interface command process
};

//================================
//=== Association line element ===
//================================
class OrigAssocLine : public Widget
{
    public:
	OrigAssocLine( const string &iid, bool lib_loc = true );
	
	string original()       { return "AssocLine"; }
	
	void attrProc( Widget *uwdg, bool init = false );
};

}

#endif //ORIGWIDG_H


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
    OrigElFigure( ) : PrWidget("ElFigure")	
    { 
        attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );
        attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"20","#000000") );
        attrAdd( new TFld("lineDecor",_("Line:decorate"),TFld::Integer,TFld::Selected,"1","0","0;1",_("No decor;Pipe")) );
        attrAdd( new TFld("bordWdth",_("Border:width"),TFld::Integer,TFld::NoFlag,"2","0","0;99") );
        attrAdd( new TFld("bordClr",_("Border:color"),TFld::String,Attr::Color,"20","#000000") );
        attrAdd( new TFld("backgClr",_("Background:color"),TFld::String,Attr::Color,"20","") );
        attrAdd( new TFld("backgImg",_("Background:image"),TFld::String,Attr::Image,"20","") );
        attrAdd( new TFld("arrowBeginType",_("Arrow:begin type"),TFld::Integer,TFld::Selected,"2","0","0;1;2;3;4;5;6;7;8",
                          _("No arrow;Arrow;Arc arrow;Line arrow;Two arrow;Rectangle;Rhomb;Cyrcle;Size line")) );
        attrAdd( new TFld("arrowBeginWidth",_("Arrow:begin width"),TFld::Integer,TFld::NoFlag,"2","0","0;99") );
        attrAdd( new TFld("arrowBeginHeight",_("Arrow:begin height"),TFld::Integer,TFld::NoFlag,"2","0","0;99") );
        attrAdd( new TFld("arrowEndType",_("Arrow:end type"),TFld::Integer,TFld::Selected,"2","0","0;1;2;3;4;5;6;7;8",
                          _("No arrow;Arrow;Arc arrow;Line arrow;Two arrow;Rectangle;Rhomb;Cyrcle;Size line")) );
        attrAdd( new TFld("arrowEndWidth",_("Arrow:end width"),TFld::Integer,TFld::NoFlag,"2","0","0;99") );
        attrAdd( new TFld("arrowEndHeight",_("Arrow:end height"),TFld::Integer,TFld::NoFlag,"2","0","0;99") );
            //Elements: line, arc, besie, grad(line,biline, radial, square decLine, atForm)
        attrAdd( new TFld("elLst",_("Element's list"),TFld::String,TFld::FullText,"300","") );
            //Next is dynamic created Element's points attributes
    }
    
    string name( )	{ return _("Elementary figures"); }
    string descr( )	{ return _("Elementary figures widget of the end visualisation."); }
};

//*************************************************
//* Form element original widget                  *
//*************************************************
class OrigFormEl : public PrWidget
{
    public:
    OrigFormEl( ) : PrWidget("FormEl")	
    {
        attrAdd( new TFld("elType",_("Element type"),TFld::Integer,TFld::Selected,"2","0","0;1;2;3;4;5",
                          _("Line edit;Text edit;Chek box;Button;Combo box;List")) );
	//Next is dynamic created individual elements attributes    
    }

    string name( )      { return _("Form's elements"); }
    string descr( )	{ return _("Form's elements widget of the end visualisation."); }
};

//************************************************
//* Text element original widget                 *
//************************************************
class OrigText : public PrWidget
{
    public:
    OrigText( ) : PrWidget("Text")	
    { 
        attrAdd( new TFld("text",_("Text"),TFld::String,TFld::FullText,"1000","Text") );
        attrAdd( new TFld("font",_("Font:full"),TFld::String,TFld::NoFlag,"50","Arial 11") );
        attrAdd( new TFld("fontFamily",_("Font:family"),TFld::String,TFld::NoFlag,"10","Arial") );
        attrAdd( new TFld("fontSize",_("Font:size"),TFld::Integer,TFld::NoFlag,"2","11") );
        attrAdd( new TFld("fontBold",_("Font:bold"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("fontItalic",_("Font:italic"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("fontUnderline",_("Font:underline"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("fontStrikeout",_("Font:strikeout"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("color",_("Color"),TFld::String,Attr::Color,"20","#000000") );
        attrAdd( new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-180;180") );
        attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,TFld::NoFlag,"1","1") );
        attrAdd( new TFld("alignment",_("Alignment"),TFld::Integer,TFld::Selected,"1","0","0;1;2;3;4;5;6;7;8;9;10;11",
                          _("Top left;Top right;Top center;Top justify;"
			    "Bottom left;Bottom right;Bottom center;Bottom justify;"
			    "V center left; V center right; Center; V center justify")) );
    }
    
    string name( )      { return _("Text fields"); }
    string descr( )	{ return _("Text fields widget of the end visualisation."); }
}; 

//************************************************
//* Media view original widget                   *
//************************************************
class OrigMedia : public PrWidget
{
    public:
    OrigMedia( ) : PrWidget("Media")	
    { 
        attrAdd( new TFld("src",_("Source"),TFld::String,TFld::NoFlag,"50","") );
        attrAdd( new TFld("play",_("Media play"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("cycle",_("Media cyclic play"),TFld::Boolean,TFld::NoFlag,"1","0") );    
    }
    
    string name( )      { return _("Media view"); }
    string descr( )	{ return _("Media view widget of the end visualisation."); }
};  
//************************************************
//* Trend view original widget                   *
//************************************************
class OrigTrend : public PrWidget
{
    public:
    OrigTrend( ) : PrWidget("Trend")	
    { 
        attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected,"1","0","0;1",
                          _("Tradition;Cyrcle")) );
        attrAdd( new TFld("widthTime",_("Width time (ms)"),TFld::Integer,TFld::Selected,"6","60000","10;360000") );
        attrAdd( new TFld("number",_("Number"),TFld::Integer,TFld::Selected,"1","0","1;2;3;4;5;6;7;8","1;2;3;4;5;6;7;8") );
	//Next is dynamic created individual trend's item attributes    
    }
    
    string name( )      { return _("Trend view"); }
    string descr( )     { return _("Trend view widget of the end visualisation."); }
};    

//************************************************
//* Protocol view original widget                *
//************************************************
class OrigProtocol : public PrWidget
{
    public:
    OrigProtocol( ) : PrWidget("Protocol")
    { 
    
    }
    
    string name( )      { return _("Protocol view"); }
    string descr( )	{ return _("Protocol view widget of the end visualisation."); }
};

//************************************************
//* Document view original widget                *
//************************************************
class OrigDocument : public PrWidget
{
    public:
    OrigDocument( ) : PrWidget("Document")	
    { 
    
    }
    
    string name( )      { return _("Document view"); }
    string descr( )     { return _("Document view widget of the end visualisation."); }
};

//************************************************
//* User function original widget                *
//************************************************
class OrigFunction : public PrWidget
{
    public:
    OrigFunction( ) : PrWidget("Function")	
    { 
    
    }
    
    string name( )      { return _("Buildin function"); }
    string descr( )     { return _("Buildin function widget of the end visualisation."); }
};

//************************************************
//* User element original widget                 *
//************************************************
class OrigUserEl : public PrWidget
{
    public:
    OrigUserEl( ) : PrWidget("UserEl")	
    { 
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF") );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","") );
    }
    
    string name( )      { return _("User element"); }
    string descr( )     { return _("User element widget of the end visualisation."); }
    
    bool isContainer( )	{ return true; }    
};

//************************************************
//* Link original widget                         *
//************************************************
class OrigLink : public PrWidget
{
    public:
    OrigLink( ) : PrWidget("Link")	
    { 
    	attrAdd( new TFld("out",_("Output"),TFld::String,TFld::NoFlag,"50","") );
    	attrAdd( new TFld("in",_("Input"),TFld::String,TFld::NoFlag,"50","") );
    	attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );
    	attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"20","#000000") );
    	attrAdd( new TFld("lineSquare",_("Line:square angle"),TFld::Boolean,TFld::NoFlag,"1","0") );
	//Next is dynamic created internal points    
    }
    
    string name( )      { return _("Interwidget link"); }
    string descr( )     { return _("Interwidget link of the end visualisation."); }
};

}

#endif //ORIGWIDG_H

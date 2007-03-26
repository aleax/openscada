
//OpenSCADA system module UI.Vision file: vis_shapes.h
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko
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

#ifndef VIS_SHAPES_H
#define VIS_SHAPES_H

class QEvent;

namespace VISION
{

class WdgView;

//*************************************************
//* Widget shape abstract object                  *
//*************************************************
class WdgShape
{
    public:
	WdgShape( const string &iid );
	
	string id( )	{ return m_id; }
	
	virtual void init( WdgView *view )	{ }
	virtual void loadData( WdgView *view )	{ }
	virtual void saveData( WdgView *view )	{ }
	virtual bool event( WdgView *view, QEvent *event );

    private:
	string m_id;
};

//============ Support widget's shapes ============

//*************************************************
//* Elementary figures shape widget               *
//*************************************************
class ShapeElFigure : public WdgShape
{
    public:    
	ShapeElFigure( );
	//bool event( WdgView *view, QEvent *event );
};

//*************************************************
//* Form element shape widget                     *
//*************************************************
class ShapeFormEl : public WdgShape
{
    public:
	ShapeFormEl( );
	//bool event( WdgView *view, QEvent *event );
};

//************************************************
//* Text element shape widget                    *
//************************************************
class ShapeText : public WdgShape
{
    public:
	ShapeText( );

	void loadData( WdgView *view );
	bool event( WdgView *view, QEvent *event );
}; 

//************************************************
//* Media view shape widget                      *
//************************************************
class ShapeMedia : public WdgShape
{
    public:
	ShapeMedia( );
	//bool event( WdgView *view, QEvent *event );
};

//************************************************
//* Trend view shape widget                      *
//************************************************
class ShapeTrend : public WdgShape
{
    public:
	ShapeTrend( );
	//bool event( WdgView *view, QEvent *event );
};    

//************************************************
//* Protocol view shape widget                   *
//************************************************
class ShapeProtocol : public WdgShape
{
    public:
	ShapeProtocol( );
	//bool event( WdgView *view, QEvent *event );    
};

//************************************************
//* Document view shape widget                   *
//************************************************
class ShapeDocument : public WdgShape
{
    public:
	ShapeDocument( );
	//bool event( WdgView *view, QEvent *event );
};

//************************************************
//* User function shape widget                   *
//************************************************
class ShapeFunction : public WdgShape
{
    public:
	ShapeFunction( );
	//bool event( WdgView *view, QEvent *event );
};

//************************************************
//* User element shape widget                    *
//************************************************
class ShapeUserEl : public WdgShape
{
    public:
	ShapeUserEl( );

	void loadData( WdgView *view );
	bool event( WdgView *view, QEvent *event );
};

//************************************************
//* Link shape widget                            *
//************************************************
class ShapeLink : public WdgShape
{
    public:
	ShapeLink( );
	//bool event( WdgView *view, QEvent *event );
};

}

#endif //VIS_SHAPES_H

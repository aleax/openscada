
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

#include <string>
#include <vector> 

#include <QObject>
#include <QMap>

using std::string;
using std::vector; 

class QEvent;
class QAction;

namespace VISION
{

class WdgView;

//*************************************************
//* Widget shape abstract object                  *
//*************************************************
class WdgShape : public QObject
{
    Q_OBJECT

    public:
	WdgShape( const string &iid );
	
	string id( )	{ return m_id; }
	
	virtual bool isEditable( ) 		{ return false; }
	
	virtual void init( WdgView *view )	{ }
	virtual void destroy( WdgView *view )	{ }
	
	virtual void editEnter( WdgView *view )	{ }
	virtual void editExit( WdgView *view )	{ }
	
	virtual void load( WdgView *view, QMap<QString, QString> &attrs )	{ }
	virtual void save( WdgView *view )	{ }
	
	virtual bool event( WdgView *view, QEvent *event );
	virtual bool eventFilter( WdgView *view, QObject *object, QEvent *event )	{ }

    private:
	string m_id;
};

//============ Support widget's shapes ============

//*************************************************
//* Elementary figures shape widget               *
//*************************************************
class ShapeElFigure : public WdgShape
{
    Q_OBJECT

    public:    
	ShapeElFigure( );
	
	bool isEditable( )	{ return true; }
	
	void editEnter( WdgView *view );
	void editExit( WdgView *view );

    public slots:
    	void toolAct( QAction * );
};

//*************************************************
//* Form element shape widget                     *
//*************************************************
class ShapeFormEl : public WdgShape
{
    Q_OBJECT

    public:
	ShapeFormEl( );
	
	void load( WdgView *view, QMap<QString, QString> &attrs );
	bool event( WdgView *view, QEvent *event );	
	bool eventFilter( WdgView *view, QObject *object, QEvent *event );
    
    public slots:
	//-- Edit line events --
	void lineAccept( );
	//-- Edit text events --	
	void textAccept( );
	//-- Check box events --
	void checkChange(int);
	//-- Combo box and list events --
	void comboChange(const QString&);
	//-- List events --
	void listChange(int);	
	//-- Button's events --
	void buttonPressed( );
	void buttonReleased( );
	void buttonToggled( bool val );

    private:
	//- Private methods -
	//Recursively widgets process for disable focusable and events filter set    
	void eventFilterSet( WdgView *view, QWidget *wdg, bool en );
	void setFocus(WdgView *view, QWidget *wdg, bool en = false, bool devel = false );
};

//************************************************
//* Text element shape widget                    *
//************************************************
class ShapeText : public WdgShape
{
    public:
	ShapeText( );

	void load( WdgView *view, QMap<QString, QString> &attrs );
	bool event( WdgView *view, QEvent *event );
}; 

//************************************************
//* Media view shape widget                      *
//************************************************
class ShapeMedia : public WdgShape
{
    public:
	ShapeMedia( );
	
	void init( WdgView *view );
	void destroy( WdgView *view );
	void load( WdgView *view, QMap<QString, QString> &attrs );
	bool event( WdgView *view, QEvent *event );
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

	void init( WdgView *view );

	void load( WdgView *view, QMap<QString, QString> &attrs );
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

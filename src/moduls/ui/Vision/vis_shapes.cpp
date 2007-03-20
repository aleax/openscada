
//OpenSCADA system module UI.Vision file: vis_shapes.cpp
/***************************************************************************
 *   Copyright (C) 2007-2007 by Roman Savochenko
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

#include <QEvent>
#include <QPainter>


#include "tvision.h"
#include "vis_widgs.h"
#include "vis_shapes.h"

using namespace VISION;

//*************************************************
//* Widget shape abstract object                  *
//*************************************************
WdgShape::WdgShape( const string &iid ) : m_id(iid)
{ 

}
    
bool WdgShape::event( WdgView *view, QEvent *event )
{
    switch(event->type())
    {
        case QEvent::Paint:
        {
    	    QPainter pnt( view );
            pnt.setWindow(0,0,200,40);
            pnt.setPen(QColor(255,0,0));
            pnt.setBackground(QBrush(QColor(210,237,234)));
            pnt.drawRect(0,0,199,39);
            pnt.drawText(3,3,194,34,Qt::AlignCenter,
		QString(_("Shape '%1'\nno implemented yet!")).arg(id().c_str()));
            event->accept();
            return true;
        }
    }
    return false;
}

//============ Support widget's shapes ============

//*************************************************
//* Elementary figures shape widget               *
//*************************************************
ShapeElFigure::ShapeElFigure( ) : WdgShape("ElFigure")
{
		    
}
			    
/*bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
					    
}*/

//*************************************************
//* Form element shape widget                     *
//*************************************************
ShapeFormEl::ShapeFormEl( ) : WdgShape("FormEl")
{

}
		
/*bool ShapeFormEl::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* Text element shape widget                    *
//************************************************
ShapeText::ShapeText( ) : WdgShape("Text")
{
		    
}
			    
/*bool ShapeText::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* Media view shape widget                      *
//************************************************
ShapeMedia::ShapeMedia( ) : WdgShape("Media")
{

}
			    
/*bool ShapeMedia::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* Trend view shape widget                      *
//************************************************
ShapeTrend::ShapeTrend( ) : WdgShape("Trend")
{

}
		
/*bool ShapeTrend::event( WdgView *view, QEvent *event )
{

}*/
			    
//************************************************
//* Protocol view shape widget                   *
//************************************************
ShapeProtocol::ShapeProtocol( ) : WdgShape("Protocol")
{

}

/*bool ShapeProtocol::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* Document view shape widget                   *
//************************************************
ShapeDocument::ShapeDocument( ) : WdgShape("Document")
{

}

/*bool ShapeDocument::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* User function shape widget                   *
//************************************************
ShapeFunction::ShapeFunction( ) : WdgShape("Function")
{
	    
}
		
/*bool ShapeFunction::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* User element shape widget                    *
//************************************************
ShapeUserEl::ShapeUserEl( ) : WdgShape("UserEl")
{

}
		
/*bool ShapeUserEl::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* Link shape widget                            *
//************************************************
ShapeLink::ShapeLink( ) : WdgShape("Link")
{

}

/*bool ShapeLink::event( WdgView *view, QEvent *event )
{

}*/

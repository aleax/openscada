//OpenSCADA system module UI.Vision file: vis_shapes.cpp
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
            pnt.setWindow(view->rect());
	    pnt.drawImage(view->rect(),QImage(":/images/attention.png"));
            event->accept();
	    view->setToolTip(QString(_("Widget's shape '%1' no implement yet!")).arg(id().c_str()));
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

void ShapeText::loadData( WdgView *view )
{
    AutoHD<VCA::Widget> wdgLnk = view->wdg();
    if( wdgLnk.freeStat() ) return;
    
    view->dataCache()["margin"] = wdgLnk.at().attrAt("geomMargin").at().getI();
    view->dataCache()["color"].setValue(QColor(wdgLnk.at().attrAt("color").at().getS().c_str()));
    view->dataCache()["text"] = wdgLnk.at().attrAt("text").at().getS().c_str();
    //- Font process -
    char family[101];
    int	 size, bold, italic, underline, strike;
    int pcnt = sscanf(wdgLnk.at().attrAt("font").at().getS().c_str(),
	    "%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
    if( pcnt < 1 ) strncpy(family,wdgLnk.at().attrAt("fontFamily").at().getS().c_str(),100);
    if( pcnt < 2 ) size = wdgLnk.at().attrAt("fontSize").at().getI();
    if( pcnt < 3 ) bold = wdgLnk.at().attrAt("fontBold").at().getB();
    if( pcnt < 4 ) italic = wdgLnk.at().attrAt("fontItalic").at().getB();
    if( pcnt < 5 ) underline = wdgLnk.at().attrAt("fontUnderline").at().getB();
    if( pcnt < 6 ) strike = wdgLnk.at().attrAt("fontStrikeout").at().getB();
	    
    QFont fnt(family,size);
    fnt.setBold(bold);
    fnt.setItalic(italic);
    fnt.setUnderline(underline);
    fnt.setStrikeOut(strike);	    
    view->dataCache()["font"].setValue(fnt);    
    //-- Set text flags --
    int txtflg = 0;
    if( wdgLnk.at().attrAt("wordWrap").at().getB() )	txtflg |= Qt::TextWordWrap;
    switch(wdgLnk.at().attrAt("alignment").at().getI()&0x3)
    {
	case 0:	txtflg |= Qt::AlignLeft; 	break;
	case 1: txtflg |= Qt::AlignRight;	break;
	case 2: txtflg |= Qt::AlignHCenter;	break;
    }
    switch(wdgLnk.at().attrAt("alignment").at().getI()>>2)
    {
	case 0:	txtflg |= Qt::AlignTop; 	break;
	case 1: txtflg |= Qt::AlignBottom;	break;
	case 2: txtflg |= Qt::AlignVCenter;	break;		
    }
    view->dataCache()["text_flg"] = txtflg;
}

bool ShapeText::event( WdgView *view, QEvent *event )
{
    switch(event->type())
    {
        case QEvent::Paint:
        {
    	    QPainter pnt( view );
	    int margin = view->dataCache().value("margin").toInt();
	    QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
	    pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));
	    
	    pnt.setPen(view->dataCache().value("color").value<QColor>());
	    pnt.setFont(view->dataCache().value("font").value<QFont>());
	    pnt.drawText(draw_area,view->dataCache().value("text_flg").toInt(),view->dataCache().value("text").toString());
	    
            event->accept();
            return true;
        }
    }
    return false;
}

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

void ShapeUserEl::loadData( WdgView *view )
{
    AutoHD<VCA::Widget> wdgLnk = view->wdg();
    if( wdgLnk.freeStat() ) return;

    view->dataCache()["margin"] = wdgLnk.at().attrAt("geomMargin").at().getI();
    view->dataCache()["brash"].setValue(QBrush(QColor(wdgLnk.at().attrAt("backColor").at().getS().c_str())));
}

bool ShapeUserEl::event( WdgView *view, QEvent *event )
{
    switch(event->type())
    {
        case QEvent::Paint:
        {
    	    QPainter pnt( view );

	    int margin = view->dataCache().value("margin").toInt();
	    QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
	    pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));

	    pnt.fillRect(draw_area,view->dataCache().value("brash").value<QBrush>());

            event->accept();
            return true;
        }
    }
    return false;
}

//************************************************
//* Link shape widget                            *
//************************************************
ShapeLink::ShapeLink( ) : WdgShape("Link")
{

}

/*bool ShapeLink::event( WdgView *view, QEvent *event )
{

}*/

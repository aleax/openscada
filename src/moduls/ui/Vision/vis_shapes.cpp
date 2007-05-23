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

#include <tsys.h>
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
	    if( view->develMode() )
    	    {
    		QPainter pnt( view );	    
        	pnt.setWindow(view->rect());
		pnt.drawImage(view->rect(),QImage(":/images/attention.png"));
        	event->accept();
		view->setToolTip(QString(_("Widget's shape '%1' no implement yet!")).arg(id().c_str()));
	    }
            return true;
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

void ShapeText::loadData( WdgView *w )
{
    QMap<QString, QString>::const_iterator	vl, 
						end = w->dataReq().end();
    
    if( (vl=w->dataReq().find("geomMargin")) != end ) 	w->dataCache()["margin"] = vl.value().toInt();
    if( (vl=w->dataReq().find("color")) != end )	w->dataCache()["color"].setValue(QColor(vl.value()));
    if( (vl=w->dataReq().find("text")) != end )		w->dataCache()["text"] = vl.value();
    
    //- Font process -	
    QFont fnt = w->dataCache().value("font").value<QFont>();
    if( (vl=w->dataReq().find("fontFamily")) != end )	fnt.setFamily(vl.value());
    if( (vl=w->dataReq().find("fontSize")) != end )	fnt.setPointSize(vl.value().toInt());
    if( (vl=w->dataReq().find("fontBold")) != end )	fnt.setBold(vl.value().toInt());
    if( (vl=w->dataReq().find("fontItalic")) != end )	fnt.setItalic(vl.value().toInt());
    if( (vl=w->dataReq().find("fontUnderline")) != end )fnt.setUnderline(vl.value().toInt());
    if( (vl=w->dataReq().find("fontStrikeout")) != end )fnt.setStrikeOut(vl.value().toInt());
    if( (vl=w->dataReq().find("font")) != end )
    {
	char family[101];
	int	 size, bold, italic, underline, strike;        
	int pcnt = sscanf(vl.value().toAscii().data(),"%100s %d %d %d %d %d",
		    family,&size,&bold,&italic,&underline,&strike);
	if( pcnt >= 1 )	fnt.setFamily(string(family,100).c_str());
	if( pcnt >= 2 ) fnt.setPointSize(size);
	if( pcnt >= 3 ) fnt.setBold(bold);
	if( pcnt >= 4 ) fnt.setItalic(italic);
	if( pcnt >= 5 ) fnt.setUnderline(underline);
	if( pcnt >= 6 ) fnt.setStrikeOut(strike);
    }
    w->dataCache()["font"].setValue(fnt);
    
    //-- Set text flags --
    int txtflg = w->dataCache().value("text_flg",0).toInt();    
    if( (vl=w->dataReq().find("wordWrap")) != end )	
	txtflg = vl.value().toInt()?(txtflg|Qt::TextWordWrap):(txtflg&(~Qt::TextWordWrap));
    if( (vl=w->dataReq().find("alignment")) != end )
    {    
	txtflg &= ~(Qt::AlignLeft|Qt::AlignRight|Qt::AlignHCenter|Qt::AlignTop|Qt::AlignBottom|Qt::AlignVCenter);
	switch(vl.value().toInt()&0x3)
	{
	    case 0: txtflg |= Qt::AlignLeft; 	break;
	    case 1: txtflg |= Qt::AlignRight;	break;
	    case 2: txtflg |= Qt::AlignHCenter;	break;
	}
	switch(vl.value().toInt()>>2)
	{
	    case 0: txtflg |= Qt::AlignTop; 	break;
	    case 1: txtflg |= Qt::AlignBottom;	break;
	    case 2: txtflg |= Qt::AlignVCenter;	break;		
	}
    }
    w->dataCache()["text_flg"] = txtflg;
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

void ShapeUserEl::loadData( WdgView *w )
{
    QMap<QString, QString>::const_iterator	vl, 
						end = w->dataReq().end();

    if( (vl=w->dataReq().find("geomMargin")) != end ) 	w->dataCache()["margin"] = vl.value().toInt();
    if( (vl=w->dataReq().find("backColor")) != end )	w->dataCache()["color"].setValue(QColor(vl.value()));
    //- Prepare brush -
    if( (vl=w->dataReq().find("backImg")) != end )
    {	
	XMLNode get_req("get");
        get_req.setAttr("user","user")->setAttr("path",w->id()+"/%2fwdg%2fres")->setAttr("id",vl.value().toAscii().data());
        if( !mod->cntrIfCmd(get_req) )
        {
	    QBrush brsh;
	    string backimg = TSYS::strDecode(get_req.text(),TSYS::base64);
	    if( !backimg.empty() )
	    {
		QImage img;
		if(img.loadFromData((const uchar*)backimg.c_str(),backimg.size()))	
		    brsh.setTextureImage(img);
	    }
	    w->dataCache()["brash"].setValue(brsh);	    
        }
    }
    //- Prepare border -
    QPen pen = w->dataCache().value("pen").value<QPen>();
    if( (vl=w->dataReq().find("bordColor")) != end )	pen.setColor(QColor(vl.value()));
    if( (vl=w->dataReq().find("bordWidth")) != end )	pen.setWidth(vl.value().toInt());
    w->dataCache()["pen"].setValue(pen);
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

	    pnt.setPen(view->dataCache().value("pen").value<QPen>());
	    pnt.fillRect(draw_area,view->dataCache().value("color").value<QColor>());
	    pnt.fillRect(draw_area,view->dataCache().value("brash").value<QBrush>());
	    
	    QPen bpen = view->dataCache().value("pen").value<QPen>();
	    if( bpen.width() )
	    {
		pnt.setPen(bpen);
		pnt.drawRect(draw_area);
	    }

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

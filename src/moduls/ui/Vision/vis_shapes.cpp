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
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>

#include <QApplication>

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
    //printf("TEST 01: Event %d \n",event->type());

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

void ShapeElFigure::editEnter( WdgView *view )
{
    printf("TEST 00\n");
}

void ShapeElFigure::editExit( WdgView *view )
{
    printf("TEST 01\n");
}

/*bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
    printf("TEST 01: Event %d \n",event->type());
}*/

//*************************************************
//* Form element shape widget                     *
//*************************************************
ShapeFormEl::ShapeFormEl( ) : WdgShape("FormEl")
{

}

void ShapeFormEl::loadData( WdgView *w )
{

    QMap<QString, QString>::const_iterator	vl, 
    						end = w->dataReq().end();   

    //- Update generic properties -
    int el = w->dataCache().value("elType",-1).toInt();
    int el_new = -1;
    if( (vl=w->dataReq().find("elType")) != end ) el_new = vl.value().toInt();
    QWidget *el_wdg = (QWidget *)TSYS::str2addr(w->dataCache()["wdg"].toString().toAscii().data());
    if( el >= 0 && el_new >= 0 && el != el_new ) delete el_wdg;
    if( !(el >= 0 && el_new >= 0 && el == el_new) )
    {
	if( el_new < 0 || el_new > 5 ) el_new = 0;
	QVBoxLayout *lay = (QVBoxLayout *)w->layout();
	if( !lay ) lay = new QVBoxLayout(w);	

	QWidget *view_wdg = NULL;
	
	switch(el_new)
	{
	    case 0: el_wdg = new QLineEdit(w);	break;
	    case 1: 
		el_wdg = new QTextEdit(w);
		view_wdg = ((QTextEdit *)el_wdg)->viewport();
		break;
	    case 2: el_wdg = new QCheckBox("test",w);	break;
	    case 3: 
	    	el_wdg = new QPushButton("test",w);
		el_wdg->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );			
		break;
	    case 4: el_wdg = new QComboBox(w);	break;
	    case 5: 
		el_wdg = new QListWidget(w);	
		view_wdg = ((QListWidget *)el_wdg)->viewport();
		break;
	}
	if( w->develMode() )
	{
	    el_wdg->setFocusPolicy(Qt::NoFocus);
	    el_wdg->unsetCursor();
	    el_wdg->setMouseTracking(true);
	    el_wdg->installEventFilter(w);
	    if( view_wdg )
	    {
		view_wdg->setFocusPolicy(Qt::NoFocus);
		view_wdg->unsetCursor();
		view_wdg->setMouseTracking(true);
		view_wdg->installEventFilter(w);
	    }
	}
	lay->addWidget(el_wdg);
	w->dataCache()["wdg"] = TSYS::addr2str(el_wdg).c_str();
	w->dataCache()["elType"] = el_new;
    }
    if( (vl=w->dataReq().find("geomMargin")) != end ) w->layout()->setMargin(vl.value().toInt());
    
    //- Update specific properties -
    switch(el_new)
    {
        case 0: 
	    if( (vl=w->dataReq().find("value")) != end ) ((QLineEdit*)el_wdg)->setText(vl.value());
	    break;
        case 1: 
	    if( (vl=w->dataReq().find("value")) != end ) ((QTextEdit*)el_wdg)->setPlainText(vl.value());
	    if( (vl=w->dataReq().find("alignment")) != end )
	    {
		Qt::Alignment txtal;
		switch(vl.value().toInt()&0x3)
		{
		    case 0: txtal |= Qt::AlignLeft; 	break;
		    case 1: txtal |= Qt::AlignRight;	break;
		    case 2: txtal |= Qt::AlignHCenter;	break;
		}
		((QTextEdit*)el_wdg)->setAlignment(txtal);
	    }
	    if( (vl=w->dataReq().find("wordWrap")) != end )
		((QTextEdit*)el_wdg)->setLineWrapMode( vl.value().toInt() ? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
	    break;
	case 2: 
	    if( (vl=w->dataReq().find("value")) != end )((QCheckBox*)el_wdg)->setChecked(vl.value().toInt());
	    if( (vl=w->dataReq().find("text")) != end )	((QCheckBox*)el_wdg)->setText(vl.value());
	    break;
	case 3: 
	    if( (vl=w->dataReq().find("text")) != end )((QPushButton*)el_wdg)->setText(vl.value());
	    if( (vl=w->dataReq().find("img")) != end )
	    {
		XMLNode get_req("get");
    		get_req.setAttr("user",w->user())->setAttr("path",w->id()+"/%2fwdg%2fres")->setAttr("id",vl.value().toAscii().data());
    		if( !mod->cntrIfCmd(get_req) )
    		{
		    string backimg = TSYS::strDecode(get_req.text(),TSYS::base64);
		    if( !backimg.empty() )
		    {
			QImage img;
			if(img.loadFromData((const uchar*)backimg.c_str(),backimg.size()))	
			    ((QPushButton*)el_wdg)->setIcon(QPixmap::fromImage(img));
		    }
		}
	    }
	    break;
	case 4:
	    if( (vl=w->dataReq().find("items")) != end )
	    {
		((QComboBox*)el_wdg)->clear();
		((QComboBox*)el_wdg)->addItems(vl.value().split("\n"));
	    }
	    if( (vl=w->dataReq().find("value")) != end )
	    {
		if( ((QComboBox*)el_wdg)->findText(vl.value()) < 0 )
		    ((QComboBox*)el_wdg)->addItem(vl.value());
		((QComboBox*)el_wdg)->setCurrentIndex(((QComboBox*)el_wdg)->findText(vl.value()));	    
	    }
	    break;	    
	case 5: 
	    if( (vl=w->dataReq().find("items")) != end )
	    {
		((QListWidget*)el_wdg)->clear();
		((QListWidget*)el_wdg)->addItems(vl.value().split("\n"));
	    }
	    if( (vl=w->dataReq().find("value")) != end )
	    {
		QList<QListWidgetItem *> its = ((QListWidget*)el_wdg)->findItems(vl.value(),Qt::MatchExactly);
		if( its.size() ) ((QListWidget*)el_wdg)->setCurrentItem(its[0]);
	    }	    
	    break;
    }
}

bool ShapeFormEl::event( WdgView *view, QEvent *event )
{

}

bool ShapeFormEl::eventFilter( WdgView *view, QObject *object, QEvent *event )
{
    //printf("TEST 00: Object %xh Event %d \n",object,event->type());
    
    switch(event->type())
    {
	case QEvent::MouseButtonPress: 
	case QEvent::MouseButtonRelease: 
	case QEvent::MouseMove: 
	case QEvent::Enter: 
	case QEvent::Leave:
	    QApplication::sendEvent(view,event);
	    return true;
    }

    return false;
}

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
    if( (vl=w->dataReq().find("orient")) != end )	w->dataCache()["rotate"] = vl.value().toInt();    
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
    
    //- Decoration -
    if( (vl=w->dataReq().find("backColor")) != end )	w->dataCache()["backColor"].setValue(QColor(vl.value()));
    //-- Prepare brush --
    if( (vl=w->dataReq().find("backImg")) != end )
    {	
	XMLNode get_req("get");
        get_req.setAttr("user",w->user())->setAttr("path",w->id()+"/%2fwdg%2fres")->setAttr("id",vl.value().toAscii().data());
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
	    w->dataCache()["backBrash"].setValue(brsh);	    
        }
    }
    //-- Prepare border --
    QPen pen = w->dataCache().value("bordPen").value<QPen>();
    if( (vl=w->dataReq().find("bordColor")) != end )	pen.setColor(QColor(vl.value()));
    if( (vl=w->dataReq().find("bordWidth")) != end )	pen.setWidth(vl.value().toInt());
    w->dataCache()["bordPen"].setValue(pen);
}

bool ShapeText::event( WdgView *view, QEvent *event )
{
    switch(event->type())
    {
        case QEvent::Paint:
        {
    	    QPainter pnt( view );
	    
	    //- Prepare draw area -
	    int margin = view->dataCache().value("margin").toInt();
	    QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
	    pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));
	    
	    pnt.translate(draw_area.width()/2,draw_area.height()/2 );
	    pnt.rotate(view->dataCache().value("rotate").toInt());
	    QRect draw_rect = QRect(QPoint(-draw_area.size().width()/2,-draw_area.size().height()/2),draw_area.size());
	    //QSize asz(draw_area.size());
	    
	    //- Draw decoration -
	    QColor bkcol = view->dataCache().value("backColor").value<QColor>();
	    if(  bkcol.isValid() ) pnt.fillRect(draw_rect,bkcol);
	    QBrush bkbrsh = view->dataCache().value("backBrash").value<QBrush>();
	    if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_rect,bkbrsh);
	    
	    QPen bpen = view->dataCache().value("bordPen").value<QPen>();
	    if(  bpen.width() )
	    {
		pnt.setPen(bpen);
		pnt.drawRect(draw_rect);
		draw_rect.adjust(bpen.width()+1,bpen.width()+1,bpen.width()-1,bpen.width()-1);
	    }
	    
	    //- Draw text -
	    pnt.setPen(view->dataCache().value("color").value<QColor>());
	    pnt.setFont(view->dataCache().value("font").value<QFont>());
	    pnt.drawText(draw_rect,view->dataCache().value("text_flg").toInt(),view->dataCache().value("text").toString());
	    
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
        get_req.setAttr("user",w->user())->setAttr("path",w->id()+"/%2fwdg%2fres")->setAttr("id",vl.value().toAscii().data());
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

	    QColor bkcol = view->dataCache().value("color").value<QColor>();
	    if(  bkcol.isValid() ) pnt.fillRect(draw_area,bkcol);
	    QBrush bkbrsh = view->dataCache().value("brash").value<QBrush>();
	    if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_area,bkbrsh);
	    //pnt.fillRect(draw_area,view->dataCache().value("color").value<QColor>());
	    //pnt.fillRect(draw_area,view->dataCache().value("brash").value<QBrush>());
	    
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

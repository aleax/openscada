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

#include <math.h>

#include <QEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QToolBar>
#include <QAction>
#include <QMovie>
#include <QBuffer>
#include <QPicture>

#include <QApplication>

#include <tsys.h>
#include "tvision.h"
#include "vis_devel.h"
#include "vis_run.h"
#include "vis_run_widgs.h"
#include "vis_devel_widgs.h"
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
	    if( qobject_cast<DevelWdgView*>(view) )
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
//* Form element shape widget                     *
//*************************************************
ShapeFormEl::ShapeFormEl( ) : WdgShape("FormEl")
{

}

void ShapeFormEl::load( WdgView *w, QMap<QString, QString> &attrs )
{

    QMap<QString, QString>::const_iterator	vl, end = attrs.end();

    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);

    //- Update generic properties -
    int el = w->dataCache().value("elType",-1).toInt();
    int el_new = -1;
    if( (vl=attrs.find("elType")) != end ) el_new = vl.value().toInt();
    QWidget *el_wdg = (QWidget *)w->dataCache().value("addrWdg").value< void* >();
    if( el >= 0 && el_new >= 0 && el != el_new ) delete el_wdg;
    if( el < 0 || (el_new >= 0 && el != el_new) )
    {
	if( el_new < 0 || el_new > 5 ) el_new = 0;
	QVBoxLayout *lay = (QVBoxLayout *)w->layout();
	if( !lay ) lay = new QVBoxLayout(w);

	switch(el_new)
	{
	    case 0:	
		el_wdg = new LineEdit(w);
		if( runW ) connect( el_wdg, SIGNAL(apply()), this, SLOT(lineAccept()) );
		break;
	    case 1: 
		el_wdg = new TextEdit(w);
		if( runW ) connect( el_wdg, SIGNAL(apply()), this, SLOT(textAccept()) );
		break;
	    case 2: 
		el_wdg = new QCheckBox("test",w);
		if( runW ) connect( el_wdg, SIGNAL(stateChanged(int)), this, SLOT(checkChange(int)) );
		break;
	    case 3: 
	    	el_wdg = new QPushButton("test",w);
		el_wdg->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
		if( runW ) 
		{
		    connect( el_wdg, SIGNAL(pressed()), this, SLOT(buttonPressed()) );
		    connect( el_wdg, SIGNAL(released()), this, SLOT(buttonReleased()) );
		    connect( el_wdg, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)) );
		}
		break;
	    case 4: 
		el_wdg = new QComboBox(w);
		if( runW ) connect( el_wdg, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(comboChange(const QString&)) );
		break;
	    case 5: 
		el_wdg = new QListWidget(w);
		if( runW ) connect( el_wdg, SIGNAL(currentRowChanged(int)), this, SLOT(listChange(int)) );
		break;
	}
	//-- Install event's filter and disable focus --	
	if( devW ) eventFilterSet(w,el_wdg,true);
	setFocus(w,el_wdg,false,devW);
	//-- Fix widget --
	lay->addWidget(el_wdg);
	w->dataCache()["addrWdg"].setValue((void*)el_wdg);
	w->dataCache()["elType"] = el_new;
    }    
    
    //- Update specific properties -
    switch(el_new)
    {
        case 0: 
	    if( (vl=attrs.find("view")) != end )
	    {
		LineEdit::LType tp = LineEdit::Text;
		switch(vl.value().toInt())
		{
		    case 0: tp = LineEdit::Text; 	break;
		    case 1: tp = LineEdit::Combo;	break;		    
		    case 2: tp = LineEdit::Integer;	break;
		    case 3: tp = LineEdit::Real;	break;
		    case 4: tp = LineEdit::Time;	break;
		    case 5: tp = LineEdit::Date;	break;
		    case 6: tp = LineEdit::DateTime;	break;
		}
		((LineEdit*)el_wdg)->setType(tp);
		if(devW) 
		{
		    eventFilterSet(w,el_wdg,true);
		    setFocus(w,el_wdg,false,devW);
		}
	    }
	    if( (vl=attrs.find("value")) != end ) 	((LineEdit*)el_wdg)->setValue(vl.value());
	    if( (vl=attrs.find("cfg")) != end ) 	((LineEdit*)el_wdg)->setCfg(vl.value());
	    break;
        case 1: 
	    if( (vl=attrs.find("value")) != end ) ((TextEdit*)el_wdg)->setText(vl.value());
	    if( (vl=attrs.find("wordWrap")) != end )
		((TextEdit*)el_wdg)->workWdg()->setLineWrapMode( vl.value().toInt() ? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
	    break;
	case 2: 
	    if( (vl=attrs.find("value")) != end )	((QCheckBox*)el_wdg)->setChecked(vl.value().toInt());
	    if( (vl=attrs.find("name")) != end )	((QCheckBox*)el_wdg)->setText(vl.value());
	    break;
	case 3: 
	    if( (vl=attrs.find("name")) != end )((QPushButton*)el_wdg)->setText(vl.value());
	    if( (vl=attrs.find("img")) != end )
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
			{
			    int ic_sz = vmin(w->size().width(), w->size().height()) - w->layout()->margin() - 5;
			    ((QPushButton*)el_wdg)->setIconSize(QSize(ic_sz,ic_sz));
			    ((QPushButton*)el_wdg)->setIcon(QPixmap::fromImage(img));
			}
		    }
		}
	    }
	    if( (vl=attrs.find("color")) != end )
	    {
		QColor clr(vl.value());
		((QPushButton*)el_wdg)->setPalette( clr.isValid() ? QPalette(clr) : QPalette() );
	    }
	    if( (vl=attrs.find("checkable")) != end )	((QPushButton*)el_wdg)->setCheckable(vl.value().toInt());
	    if( (vl=attrs.find("value")) != end )   	((QPushButton*)el_wdg)->setChecked(vl.value().toInt());
	    break;
	case 4:
	    if( (vl=attrs.find("items")) != end )
	    {
		((QComboBox*)el_wdg)->clear();
		((QComboBox*)el_wdg)->addItems(vl.value().split("\n"));
	    }
	    if( (vl=attrs.find("value")) != end )
	    {
		if( ((QComboBox*)el_wdg)->findText(vl.value()) < 0 )
		    ((QComboBox*)el_wdg)->addItem(vl.value());
		((QComboBox*)el_wdg)->setCurrentIndex(((QComboBox*)el_wdg)->findText(vl.value()));	    
	    }
	    break;	    
	case 5: 
	    if( (vl=attrs.find("items")) != end )
	    {
		((QListWidget*)el_wdg)->clear();
		((QListWidget*)el_wdg)->addItems(vl.value().split("\n"));
	    }
	    if( (vl=attrs.find("value")) != end )
	    {
		QList<QListWidgetItem *> its = ((QListWidget*)el_wdg)->findItems(vl.value(),Qt::MatchExactly);
		if( its.size() ) ((QListWidget*)el_wdg)->setCurrentItem(its[0]);
	    }	    
	    break;
    }
    if( (vl=attrs.find("geomMargin")) != end ) w->layout()->setMargin(vl.value().toInt());
    if( runW )
    {
	if( (vl=attrs.find("en")) != end ) el_wdg->setVisible(vl.value().toInt());
	if( (vl=attrs.find("active")) != end )
	{
	    w->dataCache()["active"] = (bool)vl.value().toInt();
	    setFocus(w,el_wdg,vl.value().toInt());
	    //eventFilterSet(w,el_wdg,!w->dataCache().value("active").toBool());
	    //el_wdg->setFocusPolicy( vl.value().toInt() ? Qt::StrongFocus : Qt::NoFocus );
	}
    }    
}

bool ShapeFormEl::event( WdgView *view, QEvent *event )
{

}

bool ShapeFormEl::eventFilter( WdgView *view, QObject *object, QEvent *event )
{
    switch(event->type())
    {
	case QEvent::Enter:
	case QEvent::Leave:	    
	    return true;
	case QEvent::MouseMove:	    
	case QEvent::MouseButtonPress: 
	case QEvent::MouseButtonRelease:
	    QApplication::sendEvent(view,event);
	return true;
    }
    
    return false;
}

void ShapeFormEl::lineAccept( )
{
    LineEdit *el   = (LineEdit*)sender();
    WdgView  *view = (WdgView *)el->parentWidget();
    
    view->attrSet("value",el->value().toAscii().data());
    view->attrSet("event","ws_LnAccept");
}

void ShapeFormEl::textAccept( )
{
    TextEdit *el   = (TextEdit*)sender();
    WdgView  *view = (WdgView *)el->parentWidget();
    
    view->attrSet("value",el->text().toAscii().data());
    view->attrSet("event","ws_TxtAccept");
}

void ShapeFormEl::checkChange(int st)
{
    WdgView *view = (WdgView *)((QCheckBox*)sender())->parentWidget();
    view->attrSet("value",TSYS::int2str(st));
    view->attrSet("event",st ? "ws_ChkOn" : "ws_ChkOff");
}

void ShapeFormEl::buttonPressed( )
{
    WdgView *view = (WdgView *)((QPushButton*)sender())->parentWidget();
    view->attrSet("event","ws_BtPress");
}

void ShapeFormEl::buttonReleased( )
{
    WdgView *view = (WdgView *)((QPushButton*)sender())->parentWidget();
    view->attrSet("event","ws_BtRelease");
}

void ShapeFormEl::buttonToggled( bool val )
{
    WdgView *view = (WdgView *)((QPushButton*)sender())->parentWidget();
    view->attrSet("event",string("ws_BtToggle")+(val?"On":"Off"));
    view->attrSet("value",TSYS::int2str(val));
}

void ShapeFormEl::comboChange(const QString &val)
{
    WdgView *view = (WdgView *)((QWidget*)sender())->parentWidget();
    
    view->attrSet("value",val.toAscii().data());
    view->attrSet("event","ws_CombChange");
}

void ShapeFormEl::listChange( int row )
{
    if( row < 0 ) return;
    
    QListWidget *el   = (QListWidget*)sender();
    WdgView     *view = (WdgView *)el->parentWidget();
    
    view->attrSet("value",el->item(row)->text().toAscii().data());
    view->attrSet("event","ws_ListChange");
}

void ShapeFormEl::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if( en ) 	wdg->installEventFilter(view);
    else 	wdg->removeEventFilter(view);
    //- Process childs -
    for( int i_c = 0; i_c < wdg->children().size(); i_c++ )
	if( qobject_cast<QWidget*>(wdg->children().at(i_c)) )
	    eventFilterSet(view,(QWidget*)wdg->children().at(i_c),en);
}

void ShapeFormEl::setFocus(WdgView *view, QWidget *wdg, bool en, bool devel )
{
    int isFocus = wdg->windowIconText().toInt();
    //- Set up current widget -
    if( en )
    { 
	if( isFocus ) wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
    }
    else
    {	
	if( wdg->focusPolicy() != Qt::NoFocus )
	{
	    wdg->setWindowIconText(QString::number((int)wdg->focusPolicy()));	    
	    wdg->setFocusPolicy(Qt::NoFocus);
	}
	if( devel ) wdg->setMouseTracking(true);
    }
    
    //- Process childs -
    for( int i_c = 0; i_c < wdg->children().size(); i_c++ )
	if( qobject_cast<QWidget*>(wdg->children().at(i_c)) )
	    setFocus(view,(QWidget*)wdg->children().at(i_c),en,devel);
}

//************************************************
//* Text element shape widget                    *
//************************************************
ShapeText::ShapeText( ) : WdgShape("Text")
{

}

void ShapeText::load( WdgView *w, QMap<QString, QString> &attrs )
{
    QMap<QString, QString>::const_iterator	vl, end = attrs.end();
    
    bool up = false; 
    
    if( (vl=attrs.find("geomMargin")) != end ) 	{ w->dataCache()["margin"] = vl.value().toInt(); up = true; }
    if( (vl=attrs.find("color")) != end )	{ w->dataCache()["color"].setValue(QColor(vl.value())); up = true; }
    if( (vl=attrs.find("orient")) != end )	{ w->dataCache()["rotate"] = vl.value().toInt(); up = true; }
    if( (vl=attrs.find("text")) != end )	{ w->dataCache()["text"] = vl.value(); up = true; }
    if( (vl=attrs.find("numbPrec")) != end )	{ w->dataCache()["numbPrec"] = vl.value().toInt(); up = true; }
    
    //- Font process -	
    QFont fnt = w->dataCache().value("font").value<QFont>();
    if( (vl=attrs.find("fontFamily")) != end )	{ fnt.setFamily(vl.value()); up = true; }
    if( (vl=attrs.find("fontSize")) != end )	{ fnt.setPointSize(vl.value().toInt()); up = true; }
    if( (vl=attrs.find("fontBold")) != end )	{ fnt.setBold(vl.value().toInt()); up = true; }
    if( (vl=attrs.find("fontItalic")) != end )	{ fnt.setItalic(vl.value().toInt()); up = true; }
    if( (vl=attrs.find("fontUnderline")) != end ){ fnt.setUnderline(vl.value().toInt()); up = true; }
    if( (vl=attrs.find("fontStrikeout")) != end ){ fnt.setStrikeOut(vl.value().toInt()); up = true; }
    if( (vl=attrs.find("font")) != end )
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
	up = true;
    }
    w->dataCache()["font"].setValue(fnt);
    
    //-- Set text flags --
    int txtflg = w->dataCache().value("text_flg",0).toInt();    
    if( (vl=attrs.find("wordWrap")) != end )
    { txtflg = vl.value().toInt()?(txtflg|Qt::TextWordWrap):(txtflg&(~Qt::TextWordWrap)); up = true; }
    if( (vl=attrs.find("alignment")) != end )
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
	up = true;
    }
    w->dataCache()["text_flg"] = txtflg;
    
    //- Decoration -
    if( (vl=attrs.find("backColor")) != end )	{ w->dataCache()["backColor"].setValue(QColor(vl.value())); up = true; }
    //-- Prepare brush --
    if( (vl=attrs.find("backImg")) != end )
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
	up = true;
    }
    //-- Prepare border --
    QPen pen = w->dataCache().value("bordPen").value<QPen>();
    if( (vl=attrs.find("bordColor")) != end )	{ pen.setColor(QColor(vl.value())); up = true; }
    if( (vl=attrs.find("bordWidth")) != end )	{ pen.setWidth(vl.value().toInt()); up = true; }
    w->dataCache()["bordPen"].setValue(pen);
    
    //-- Enable widget state --
    if( qobject_cast<RunWdgView*>(w) && (vl=attrs.find("en")) != end )
    { w->dataCache()["en"].setValue(vl.value().toInt()); up = true; }

    if( up ) w->update();
}

bool ShapeText::event( WdgView *view, QEvent *event )
{
    if( !view->dataCache().value("en",1).toInt() ) return true;
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
	    int angle = view->dataCache().value("rotate").toInt();
	    pnt.rotate(angle);
	    //- Calc whidth and hight draw rect at rotate -
	    double rad_angl  = fabs(3.14159*(double)angle/180.);
	    double rect_rate = 1./(fabs(cos(rad_angl))+fabs(sin(rad_angl)));
	    int wdth  = (int)(rect_rate*draw_area.size().width()+
			    sin(rad_angl)*(draw_area.size().height()-draw_area.size().width()));
	    int heigt = (int)(rect_rate*draw_area.size().height()+
			    sin(rad_angl)*(draw_area.size().width()-draw_area.size().height()));
	    
	    //QRect draw_rect = QRect(QPoint(-draw_area.size().width()/2,-draw_area.size().height()/2),draw_area.size());
	    QRect draw_rect = QRect(QPoint(-wdth/2,-heigt/2),QSize(wdth,heigt));
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
		pnt.drawRect(draw_rect.adjusted(bpen.width()/2,bpen.width()/2,
			     -bpen.width()/2-bpen.width()%2,-bpen.width()/2-bpen.width()%2));
		draw_rect.adjust(bpen.width()+1,bpen.width()+1,bpen.width()-1,bpen.width()-1);
	    }
	    
	    //- Draw text -
	    pnt.setPen(view->dataCache().value("color").value<QColor>());
	    pnt.setFont(view->dataCache().value("font").value<QFont>());
	    
	    QString textv = view->dataCache().value("text").toString();
	    int numbPrec = view->dataCache().value("numbPrec").toInt();
	    if( numbPrec ) textv = QString::number(textv.toDouble(),'f',numbPrec);
	    pnt.drawText(draw_rect,view->dataCache().value("text_flg").toInt(),textv);
	    
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

void ShapeMedia::init( WdgView *view )
{
    //- Create label widget -
    QLabel *lab = new QLabel(view);
    if( qobject_cast<DevelWdgView*>(view) ) lab->setMouseTracking(true);
    lab->setAlignment(Qt::AlignCenter);
    lab->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);    
    view->dataCache()["labWdg"].setValue( (void*)lab );
    QVBoxLayout *lay = new QVBoxLayout(view);
    lay->addWidget(lab);
}

void ShapeMedia::destroy( WdgView *view )
{
    QLabel *lab = (QLabel*)view->dataCache().value("labWdg").value< void* >();
    if( lab && lab->movie() )
    {
	if(lab->movie()->device()) delete lab->movie()->device();
	delete lab->movie();
	lab->clear();
    }
}

void ShapeMedia::load( WdgView *w, QMap<QString, QString> &attrs )
{
    QMap<QString, QString>::const_iterator vl, end = attrs.end();
    
    bool up = false, reld_src = false;
    QLabel *lab = (QLabel*)w->dataCache().value("labWdg").value< void* >();

    //- Media data process -
    if( (vl=attrs.find("type")) != end && w->dataCache().value("mediaType",-1).toInt() != vl.value().toInt() ) 
    { 
	w->dataCache()["mediaType"] = vl.value().toInt(); 
	reld_src = true; 
    }
    if( (vl=attrs.find("src")) != end && w->dataCache().value("mediaSrc").toString() != vl.value() )
    { 
	w->dataCache()["mediaSrc"] = vl.value();
	reld_src = true;
    }
    if( reld_src )
    {
	XMLNode get_req("get");
        get_req.setAttr("user",w->user())->
		setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",w->dataCache().value("mediaSrc").toString().toAscii().data());
        if( !mod->cntrIfCmd(get_req) )
        {
	    string sdata = TSYS::strDecode(get_req.text(),TSYS::base64);	    
	    if( !sdata.empty() )
		switch(w->dataCache().value("mediaType",0).toInt())
		{
		    case 0:
		    {
			QImage img;
			//- Free movie data, if set -
			if( lab->movie() )
			{
			    if(lab->movie()->device()) delete lab->movie()->device();
			    delete lab->movie();
			    lab->clear();
			}
			//- Set new image -
			double scl_rat = w->dataCache().value("mediaScale",1).toDouble();
			if( img.loadFromData((const uchar*)sdata.data(),sdata.size()) )
 			    lab->setPixmap(QPixmap::fromImage(img.scaled(
					    (int)((double)img.width()*scl_rat),
					    (int)((double)img.height()*scl_rat),
					    Qt::KeepAspectRatio,Qt::SmoothTransformation)));        		    
			break;
		    }
		    case 1:
		    {
			//- Clear previous movie data -
			if( lab->movie() )
			{
			    if(lab->movie()->device()) delete lab->movie()->device();
			    delete lab->movie();
			    lab->clear();
			}
			//- Set new data -
			QBuffer *buf = new QBuffer(w);
			buf->setData( sdata.data(), sdata.size() );
			buf->open( QIODevice::ReadOnly );
			lab->setMovie( new QMovie(buf) );
			break;
		    }			
		}
	}
    }
    //-- Process self type options --
    switch(w->dataCache().value("mediaType",-1).toInt())
    {
	case 0:
	    if( (vl=attrs.find("scale")) != end )
	    {
		double scl_rat = vl.value().toDouble();
		w->dataCache()["mediaScale"] = scl_rat;
		if( scl_rat < 0.1 ) lab->setScaledContents(true);
		else
		{
		    lab->setScaledContents(false);
		    //- Reload image -
	     	    XMLNode get_req("get");
		    get_req.setAttr("user",w->user())->
			    setAttr("path",w->id()+"/%2fwdg%2fres")->
			    setAttr("id",w->dataCache().value("mediaSrc").toString().toAscii().data());
		    if( mod->cntrIfCmd(get_req) ) break;
		    string sdata = TSYS::strDecode(get_req.text(),TSYS::base64);	    
		    if( sdata.empty() )	break;
		    QImage img;
    		    if( img.loadFromData((const uchar*)sdata.data(),sdata.size()) )
			    lab->setPixmap(QPixmap::fromImage(img.scaled(
					    (int)((double)img.width()*scl_rat),
					    (int)((double)img.height()*scl_rat),
					    Qt::KeepAspectRatio,Qt::SmoothTransformation)));
    		}
	    }
	    break;
	case 1:
	    if( (vl=attrs.find("play")) != end && lab->movie() )
		vl.value().toInt() ? lab->movie()->start() : lab->movie()->stop();
	    if( (vl=attrs.find("speed")) != end && lab->movie() )
		lab->movie()->setSpeed( vl.value().toInt() );
	    if( (vl=attrs.find("fit")) != end && lab->movie() )
		lab->setScaledContents(vl.value().toInt());
	    break;
    }
    
    //- Decoration -    
    if( (vl=attrs.find("geomMargin")) != end ) 	
    { 
	w->dataCache()["margin"] = vl.value().toInt(); 
	w->layout()->setMargin(w->dataCache().value("margin").toInt());
	up = true;
    }
    if( (vl=attrs.find("backColor")) != end )	{ w->dataCache()["backColor"].setValue(QColor(vl.value())); up = true; }
    //-- Prepare brush --
    if( (vl=attrs.find("backImg")) != end )
    {	
	XMLNode get_req("get");
        get_req.setAttr("user",w->user())->
		setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",vl.value().toAscii().data());
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
	up = true;
    }
    //-- Prepare border --
    QPen pen = w->dataCache().value("bordPen").value<QPen>();
    if( (vl=attrs.find("bordColor")) != end )	{ pen.setColor(QColor(vl.value())); up = true; }
    if( (vl=attrs.find("bordWidth")) != end )	{ pen.setWidth(vl.value().toInt()); up = true; }
    w->dataCache()["bordPen"].setValue(pen);
    
    //- Enable widget state -
    if( qobject_cast<RunWdgView*>(w) && (vl=attrs.find("en")) != end )
    { w->dataCache()["en"].setValue(vl.value().toInt()); up = true; }

    if( up ) w->update();
}

bool ShapeMedia::event( WdgView *view, QEvent *event )
{
    if( !view->dataCache().value("en",1).toInt() ) return true;
    if( event->type() == QEvent::Paint )
    {
	QPainter pnt( view );
	    
	//- Prepare draw area -
	int margin = view->dataCache().value("margin").toInt();
	QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
        pnt.setWindow(draw_area);
	pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));
	
	//- Draw decoration -
	QColor bkcol = view->dataCache().value("backColor").value<QColor>();
	if( bkcol.isValid() ) pnt.fillRect(draw_area,bkcol);
	QBrush bkbrsh = view->dataCache().value("backBrash").value<QBrush>();
	if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_area,bkbrsh);
	    
	QPen bpen = view->dataCache().value("bordPen").value<QPen>();
	if( bpen.width() )
	{
	    pnt.setPen(bpen);
	    pnt.drawRect(draw_area.adjusted(bpen.width()/2,bpen.width()/2,
		        -bpen.width()/2-bpen.width()%2,-bpen.width()/2-bpen.width()%2));
	}
        return true;
    }
    return false;
}

//************************************************
//* Diagram view shape widget                      *
//************************************************
ShapeDiagram::ShapeDiagram( ) : WdgShape("Diagram")
{

}

void ShapeDiagram::init( WdgView *view )
{
    QPicture *pict = new QPicture();
    view->dataCache()["pictObj"].setValue( (void*)pict );
}

void ShapeDiagram::destroy( WdgView *view )
{
    delete (QPicture*)view->dataCache().value("pictObj").value< void* >();
}

void ShapeDiagram::load( WdgView *w, QMap<QString, QString> &attrs )
{
    QMap<QString, QString>::const_iterator vl, end = attrs.end();
    bool up = false;  
    
    if( qobject_cast<RunWdgView*>(w) && (vl=attrs.find("en")) != end )
    { w->dataCache()["en"].setValue(vl.value().toInt()); up = true; }        
    if( (vl=attrs.find("geomMargin")) != end )	{ w->dataCache()["margin"] = vl.value().toInt(); up = true; }
    if( (vl=attrs.find("backColor")) != end )	{ w->dataCache()["backColor"].setValue(QColor(vl.value())); up = true; }
    if( (vl=attrs.find("backImg")) != end )
    {	
	XMLNode get_req("get");
        get_req.setAttr("user",w->user())->
		setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",vl.value().toAscii().data());
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
	up = true;
    }
    //-- Prepare border --
    QPen pen = w->dataCache().value("bordPen").value<QPen>();
    if( (vl=attrs.find("bordColor")) != end )	{ pen.setColor(QColor(vl.value())); up = true; }
    if( (vl=attrs.find("bordWidth")) != end )	{ pen.setWidth(vl.value().toInt()); up = true; }
    w->dataCache()["bordPen"].setValue(pen);    
    
    if( up )	update(w);
}

void ShapeDiagram::update( WdgView *w )
{
    if( !w->dataCache().value("en",1).toInt() )	return;

    //- Prepare picture -    
    QPainter pnt((QPicture*)w->dataCache().value("pictObj").value< void* >());
	
    int margin = w->dataCache().value("margin").toInt();	
    QRect draw_area = w->rect().adjusted(0,0,-2*margin,-2*margin);	    
    pnt.setWindow(draw_area);
    pnt.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));
	
    //- Draw decoration -
    QColor bkcol = w->dataCache().value("backColor").value<QColor>();
    if( bkcol.isValid() ) pnt.fillRect(draw_area,bkcol);
    QBrush bkbrsh = w->dataCache().value("backBrash").value<QBrush>();
    if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_area,bkbrsh);
    
    QPen bpen = w->dataCache().value("bordPen").value<QPen>();
    if( bpen.width() )
    {
        pnt.setPen(bpen);
        pnt.drawRect(draw_area.adjusted(bpen.width()/2,bpen.width()/2,
	        -bpen.width()/2-bpen.width()%2,-bpen.width()/2-bpen.width()%2));
    }
	
    pnt.drawEllipse(10,20, 80,70);
    
    //- Call repaint -
    w->update();
}

bool ShapeDiagram::event( WdgView *view, QEvent *event )
{
    if( !view->dataCache().value("en",1).toInt() ) return true;
    if( event->type() == QEvent::Paint )
    {
	QPainter pnt( view );
	pnt.drawPicture(0, 0,*(QPicture*)view->dataCache().value("pictObj").value< void* >());
        return true;
    }
    return false;
}

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
ShapeBox::ShapeBox( ) : WdgShape("Box")
{

}

void ShapeBox::init( WdgView *w )
{
    w->dataCache()["inclWidget"].setValue((void*)NULL);
}

void ShapeBox::load( WdgView *w, QMap<QString, QString> &attrs )
{
    QMap<QString, QString>::const_iterator	vl, end = attrs.end();
    
    bool up = false;

    if( (vl=attrs.find("geomMargin")) != end )
    {
	w->dataCache()["margin"] = vl.value().toInt();
	if( w->layout() ) w->layout()->setMargin( w->dataCache().value("margin").toInt() );
	up = true;
    }
    if( (vl=attrs.find("backColor")) != end )	{ w->dataCache()["color"].setValue(QColor(vl.value())); up = true; }
    //- Prepare brush -
    if( (vl=attrs.find("backImg")) != end )
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
	up = true;
    }
    //- Prepare border -
    QPen pen = w->dataCache().value("pen").value<QPen>();
    if( (vl=attrs.find("bordColor")) != end )	{ pen.setColor(QColor(vl.value())); up = true; }    
    if( (vl=attrs.find("bordWidth")) != end )	{ pen.setWidth(vl.value().toInt()); up = true; }
    w->dataCache()["pen"].setValue(pen);
    //- Check for include widget -
    if( (vl=attrs.find("pgOpenSrc")) != end && qobject_cast<RunWdgView*>(w) )
    {
	RunWdgView *el_wdg = (RunWdgView *)w->dataCache().value("inclWidget").value< void* >();	
	//-- Put previous include widget to page cache --
	if( !el_wdg || vl.value() != el_wdg->id().c_str() ) 
	{
	    if( el_wdg ) 
	    { 
		((RunWdgView*)w)->mainWin()->pgCacheAdd(el_wdg);
		el_wdg->setEnabled(false);
		el_wdg->setVisible(false);
		el_wdg->setParent(NULL);
		el_wdg = NULL; 
	    }
	    //-- Create new include widget --	
	    if( vl.value().size() )
	    {
		QVBoxLayout *lay = (QVBoxLayout *)w->layout();
		if( !lay ) lay = new QVBoxLayout(w);
		
		el_wdg = ((RunWdgView*)w)->mainWin()->pgCacheGet(vl.value().toAscii().data());
		if( el_wdg )
		{
		    el_wdg->setParent(w);
		    el_wdg->setEnabled(true);
		    el_wdg->setVisible(true);
		}
		else 
		{
		    el_wdg = new RunWdgView(vl.value().toAscii().data(),0,(VisRun*)w->mainWin(),w);
		    el_wdg->load("");
		}
		//el_wdg->resize(w->size());
		lay->addWidget(el_wdg);
		lay->setMargin(w->dataCache().value("margin").toInt());
	    }
	    w->dataCache()["inclWidget"].setValue((void*)el_wdg);
	    up = true;
	}	
    }        
    if( up ) w->update();
}

bool ShapeBox::event( WdgView *view, QEvent *event )
{
    switch(event->type())
    {
        case QEvent::Paint:
        {
	    if( view->dataCache().value("inclWidget").value< void* >() ) return false;
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
		pnt.drawRect(draw_area.adjusted(bpen.width()/2,bpen.width()/2,
						-bpen.width()/2-bpen.width()%2,-bpen.width()/2-bpen.width()%2));
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

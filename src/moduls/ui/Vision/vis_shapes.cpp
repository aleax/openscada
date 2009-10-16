//OpenSCADA system module UI.Vision file: vis_shapes.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko
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
#include <QStackedWidget>
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
#include <QTimer>
#include <QKeyEvent>
#include <QTableWidget>
#include <QDateTime>
#include <QToolTip>
#include <QScrollBar>
#include <QHeaderView>

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
		view->setToolTip(QString(_("Widget's shape '%1' is not implemented yet!")).arg(id().c_str()));
	    }
	    return true;
    }
    return false;
}

void WdgShape::borderDraw( QPainter &pnt, QRect dA, QPen bpen, int bordStyle )
{
    int bordWidth = bpen.width();
    if( bordStyle && bordWidth )
    switch(bordStyle)
    {
	case 1:	//Dotted
	    bpen.setStyle(Qt::DotLine);
	    pnt.setPen(bpen);
	    pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    break;
	case 2:	//Dashed
	    bpen.setStyle(Qt::DashLine);
	    pnt.setPen(bpen);
	    pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    break;
	case 3:	//Solid
	    bpen.setStyle(Qt::SolidLine);
	    pnt.setPen(bpen);
	    pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    break;
	case 4:	//Double
	    bpen.setStyle(Qt::SolidLine);
	    if( bordWidth/3 )
	    {
		int brdLnSpc = bordWidth-2*(bordWidth/3);
		bordWidth/=3;
		bpen.setWidth(bordWidth);
		pnt.setPen(bpen);
		pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
		pnt.drawRect(dA.adjusted(bordWidth+brdLnSpc+bordWidth/2,bordWidth+brdLnSpc+bordWidth/2,
			-bordWidth-brdLnSpc-bordWidth/2-bordWidth%2,-bordWidth-brdLnSpc-bordWidth/2-bordWidth%2));
	    }
	    else
	    {
	        pnt.setPen(bpen);
	        pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    }
	    break;
	case 5:	//Groove
	{
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadeRect(&pnt,dA,plt,true,bordWidth/2);
	    break;
	}
	case 6:	//Ridge
	{
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadeRect(&pnt,dA,plt,false,bordWidth/2);
	    break;
	}
	case 7:	//Inset
	{
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadePanel(&pnt,dA,plt,true,bordWidth);
	    break;
	}
	case 8:	//Outset
	{
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadePanel(&pnt,dA,plt,false,bordWidth);
	    break;
	}
    }
}

bool WdgShape::attrSet( WdgView *view, int uiPrmPos, const string &val )
{
    return false;
}

QFont WdgShape::getFont( const string &val, float fsc, bool pixSize )
{
    QFont rez;

    char family[101]; strcpy(family,"Arial");
    int size = 10, bold = 0, italic = 0, underline = 0, strike = 0;
    sscanf(val.c_str(),"%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
    rez.setFamily(QString(family).replace(QRegExp("_")," "));
    if( pixSize ) rez.setPixelSize( fsc*size );
    else rez.setPointSize( fsc*size );
    rez.setBold(bold);
    rez.setItalic(italic);
    rez.setUnderline(underline);
    rez.setStrikeOut(strike);

    return rez;
}

//============ Support widget's shapes ============

//*************************************************
//* Form element shape widget                     *
//*************************************************
ShapeFormEl::ShapeFormEl( ) : WdgShape("FormEl")
{

}

void ShapeFormEl::init( WdgView *w )
{
    QVBoxLayout *lay = new QVBoxLayout(w);
    w->shpData = new ShpDt();
}

void ShapeFormEl::destroy( WdgView *w )
{
    delete (ShpDt*)w->shpData;
}

bool ShapeFormEl::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);

    bool rel_cfg = false;	//Reload configuration

    shD->evLock = true;
    switch( uiPrmPos )
    {
	case -1:	//load
	    rel_cfg = true;
	    break;
	case 5:		//en
	    if(!runW)	break;
	    shD->en = (bool)atoi(val.c_str());
	    runW->setVisible( atoi(val.c_str()) && runW->permView() );
	    if( shD->elType >= 0 ) shD->addrWdg->setVisible( atoi(val.c_str()) && runW->permView() );
	    break;
	case 6:		//active
	    if(!runW)	break;
	    shD->active = (bool)atoi(val.c_str());
	    if( shD->elType >= 0 )
	    {
		setFocus(w,shD->addrWdg,atoi(val.c_str()) && runW->permCntr());
		shD->addrWdg->setEnabled(atoi(val.c_str()) && runW->permCntr());
	    }
	    break;
	case 12:	//geomMargin
	    w->layout()->setMargin(atoi(val.c_str()));	break;
	case 20:	//elType
	    if( shD->elType == atoi(val.c_str()) ) break;
	    shD->elType = atoi(val.c_str());
	    shD->welType = -1;
	    rel_cfg = true;
	    break;
	case 21:	//value
	    shD->value = val;
	    if( !shD->addrWdg ) break;
	    switch( shD->welType )
	    {
		case 0:
		    if( !((LineEdit*)shD->addrWdg)->isEdited( ) ) ((LineEdit*)shD->addrWdg)->setValue(val.c_str());
		    break;
		case 1:	((TextEdit*)shD->addrWdg)->setText(val.c_str());	break;
		case 2:	((QCheckBox*)shD->addrWdg)->setChecked(atoi(val.c_str()));	break;
		case 3:	((QPushButton*)shD->addrWdg)->setChecked(atoi(val.c_str()));	break;
		case 4:	
		    if( ((QComboBox*)shD->addrWdg)->findText(val.c_str()) < 0 ) ((QComboBox*)shD->addrWdg)->addItem(val.c_str());
			((QComboBox*)shD->addrWdg)->setCurrentIndex(((QComboBox*)shD->addrWdg)->findText(val.c_str()));
		    break;
		case 5:
		{
		    QList<QListWidgetItem *> its = ((QListWidget*)shD->addrWdg)->findItems(val.c_str(),Qt::MatchExactly);
		    if( its.size() ) ((QListWidget*)shD->addrWdg)->setCurrentItem(its[0]);
		    break;
		}
		case 6: case 7:	((QAbstractSlider*)shD->addrWdg)->setValue(atoi(val.c_str()));	break;
	    }
	    break;
	case 22:	//view, wordWrap, img, items, cfg
	    rel_cfg = true;
	    switch( shD->elType )
	    {
		case 0:		//view
		    shD->view = atoi(val.c_str());	break;
		case 1:		//wordWrap
		    shD->wordWrap = atoi(val.c_str());	break;
		case 3:		//img
		    shD->img = val;		break;
		case 4: case 5:	//items
		    shD->items = val;		break;
		case 6: case 7:	//cfg
		    shD->cfg = val;		break;
		default: rel_cfg = false;
	    }
	    break;
	case 23:	//cfg, color
	    rel_cfg = true;
	    switch( shD->elType )
	    {
		case 0:	//cfg
		    shD->cfg = val;
		    //((LineEdit*)shD->addrWdg)->setCfg(val.c_str());
		    break;
		case 3:	//color
		    shD->color = val;
		    //((QPushButton*)shD->addrWdg)->setPalette( QColor(val.c_str()).isValid() ? QPalette(QColor(val.c_str())) : QPalette() );
		    break;
		default: rel_cfg = false;
	    }
	    break;
	case 24:	//checkable
	    shD->checkable = (bool)atoi(val.c_str());
	    rel_cfg = true;
	    break;
	case 25:	//font
	    shD->font = getFont(val);
	    rel_cfg = true;
	    break;
	case 26:	//name
	    shD->name = val;
	    if( shD->welType == 2)	((QCheckBox*)shD->addrWdg)->setText(val.c_str());
	    else if( shD->welType == 3 )	((QPushButton*)shD->addrWdg)->setText(val.c_str());
	    break;
	case 27:	//colorText
	    shD->colorText = val;
	    rel_cfg = true;
	    break;
    }
    if( rel_cfg && !w->allAttrLoad() )
    {
	bool mk_new = false;
	Qt::Alignment wAlign = 0;
	QFont elFnt = shD->font;
	if( elFnt.pixelSize() > 0 )
	    elFnt.setPixelSize((int)((float)elFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true))));
	switch( shD->elType )
	{
	    case 0:	//Line edit
	    {
		if( !shD->addrWdg || !qobject_cast<LineEdit*>(shD->addrWdg) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = new LineEdit(w);
		    if( runW ) connect( shD->addrWdg, SIGNAL(apply()), this, SLOT(lineAccept()) );
		    mk_new = true;
		}
		//> View
		LineEdit::LType tp = LineEdit::Text;
		switch( shD->view )
		{
		    case 0: tp = LineEdit::Text;	break;
		    case 1: tp = LineEdit::Combo;	break;
		    case 2: tp = LineEdit::Integer;	break;
		    case 3: tp = LineEdit::Real;	break;
		    case 4: tp = LineEdit::Time;	break;
		    case 5: tp = LineEdit::Date;	break;
		    case 6: tp = LineEdit::DateTime;	break;
		}
		if( ((LineEdit*)shD->addrWdg)->type() != tp )
		{ ((LineEdit*)shD->addrWdg)->setType(tp); mk_new = true; }
		//> Cfg
		((LineEdit*)shD->addrWdg)->setCfg(shD->cfg.c_str());
		//> Value
		((LineEdit*)shD->addrWdg)->setValue(shD->value.c_str());
		//> Font
		shD->addrWdg->setFont(elFnt);
		break;
	    }
	    case 1:	//Text edit
		if( !shD->addrWdg || !qobject_cast<TextEdit*>(shD->addrWdg) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = new TextEdit(w);
		    if( runW ) connect( shD->addrWdg, SIGNAL(apply()), this, SLOT(textAccept()) );
		    mk_new = true;
		}
		//- Value -
		((TextEdit*)shD->addrWdg)->setText(shD->value.c_str());
		//- WordWrap -
		((TextEdit*)shD->addrWdg)->workWdg()->setLineWrapMode( shD->wordWrap ? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
		//- Font -
		shD->addrWdg->setFont(elFnt);
		break;
	    case 2:	//Chek box
		if( !shD->addrWdg || !qobject_cast<QCheckBox*>(shD->addrWdg) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = new QCheckBox("test",w);
		    if( runW ) connect( shD->addrWdg, SIGNAL(stateChanged(int)), this, SLOT(checkChange(int)) );
		    mk_new = true;
		}
		//- Name -
		((QCheckBox*)shD->addrWdg)->setText(shD->name.c_str());
		//- Value -
		((QCheckBox*)shD->addrWdg)->setChecked(atoi(shD->value.c_str()));
		//- Font -
		shD->addrWdg->setFont(elFnt);
		break;
	    case 3:	//Button
	    {
		if( !shD->addrWdg || !qobject_cast<QPushButton*>(shD->addrWdg) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = new QPushButton("test",w);
		    shD->addrWdg->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
		    if( runW )
		    {
			connect( shD->addrWdg, SIGNAL(pressed()), this, SLOT(buttonPressed()) );
			connect( shD->addrWdg, SIGNAL(released()), this, SLOT(buttonReleased()) );
			connect( shD->addrWdg, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)) );
		    }
		    mk_new = true;
		}
		//- Name -
		((QPushButton*)shD->addrWdg)->setText(shD->name.c_str());
		//- Img -
		QImage img;
		string backimg = w->resGet(shD->img);
		if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		{
		    int ic_sz = vmin(w->size().width(), w->size().height()) - w->layout()->margin() - 5;
		    ((QPushButton*)shD->addrWdg)->setIconSize(QSize(ic_sz,ic_sz));
		    ((QPushButton*)shD->addrWdg)->setIcon(QPixmap::fromImage(img));
		} else ((QPushButton*)shD->addrWdg)->setIcon(QPixmap());
		//- Color -
		QPalette plt;
		QColor clr(shD->color.c_str());
		if( clr.isValid() )	plt.setColor(QPalette::Button,clr);
		clr = QColor(shD->colorText.c_str());
		if( clr.isValid() )	plt.setColor(QPalette::ButtonText,clr);
		((QPushButton*)shD->addrWdg)->setPalette(plt);
		//- Checkable -
		((QPushButton*)shD->addrWdg)->setCheckable(shD->checkable);
		//- Value -
		((QPushButton*)shD->addrWdg)->setChecked(atoi(shD->value.c_str()));
		//- Font -
		shD->addrWdg->setFont(elFnt);
		break;
	    }
	    case 4:	//Combo box
	    {
		if( !shD->addrWdg || !qobject_cast<QComboBox*>(shD->addrWdg) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = new QComboBox(w);
		    if( runW ) connect( shD->addrWdg, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(comboChange(const QString&)) );
		    mk_new = true;
		}
		//- Items -
		((QComboBox*)shD->addrWdg)->clear();
		((QComboBox*)shD->addrWdg)->addItems(QString(shD->items.c_str()).split("\n"));
		//- Value -
		if( ((QComboBox*)shD->addrWdg)->findText(shD->value.c_str()) < 0 ) ((QComboBox*)shD->addrWdg)->addItem(shD->value.c_str());
		((QComboBox*)shD->addrWdg)->setCurrentIndex(((QComboBox*)shD->addrWdg)->findText(shD->value.c_str()));
		//- Font -
		shD->addrWdg->setFont(elFnt);
		break;
	    }
	    case 5:	//List
	    {
		if( !shD->addrWdg || !qobject_cast<QListWidget*>(shD->addrWdg) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = new QListWidget(w);
		    if( runW ) connect( shD->addrWdg, SIGNAL(currentRowChanged(int)), this, SLOT(listChange(int)) );
		    mk_new = true;
		}
		//- Items -
		((QListWidget*)shD->addrWdg)->clear();
		((QListWidget*)shD->addrWdg)->addItems(QString(shD->items.c_str()).split("\n"));
		//- Value -
		QList<QListWidgetItem *> its = ((QListWidget*)shD->addrWdg)->findItems(shD->value.c_str(),Qt::MatchExactly);
		if( its.size() ) ((QListWidget*)shD->addrWdg)->setCurrentItem(its[0]);
		//- Font -
		shD->addrWdg->setFont(elFnt);
		break;
	    }
	    case 6:	//Slider
	    case 7:	//Scroll bar
	    {
		if( !shD->addrWdg || (shD->elType==6 && !qobject_cast<QSlider*>(shD->addrWdg)) || (shD->elType==7 && !qobject_cast<QScrollBar*>(shD->addrWdg)) )
		{
		    if( shD->addrWdg ) shD->addrWdg->deleteLater();
		    shD->addrWdg = (shD->elType==6 ? (QWidget *)new QSlider(w) : (QWidget *)new QScrollBar(w));
		    if( runW ) connect( shD->addrWdg, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)) );
		    mk_new = true;
		}
		int cfgOff = 0;
		if( shD->elType == 6 )	((QSlider*)shD->addrWdg)->setTickPosition(QSlider::TicksBothSides);
		if( atoi(TSYS::strSepParse(shD->cfg,0,':',&cfgOff).c_str()) )
		{
		    ((QAbstractSlider*)shD->addrWdg)->setOrientation( Qt::Vertical );
		    wAlign = Qt::AlignHCenter;
		}
		else
		{
		    ((QAbstractSlider*)shD->addrWdg)->setOrientation( Qt::Horizontal );
		    wAlign = Qt::AlignVCenter;
		}
		((QAbstractSlider*)shD->addrWdg)->setMinimum( atoi(TSYS::strSepParse(shD->cfg,0,':',&cfgOff).c_str()) );
		((QAbstractSlider*)shD->addrWdg)->setMaximum( atoi(TSYS::strSepParse(shD->cfg,0,':',&cfgOff).c_str()) );
		((QAbstractSlider*)shD->addrWdg)->setSingleStep( atoi(TSYS::strSepParse(shD->cfg,0,':',&cfgOff).c_str()) );
		((QAbstractSlider*)shD->addrWdg)->setPageStep( atoi(TSYS::strSepParse(shD->cfg,0,':',&cfgOff).c_str()) );
		break;
	    }
	}
	if( mk_new )
	{
	    //-- Install event's filter and disable focus --
	    eventFilterSet(w,shD->addrWdg,true);
	    w->setFocusProxy(shD->addrWdg);
	    if( devW )	setFocus(w,shD->addrWdg,false,devW);
	    if( runW )
	    {
		setFocus( w, shD->addrWdg, shD->active && runW->permCntr() );
		shD->addrWdg->setEnabled( shD->active && runW->permCntr() );
	    }
	    shD->addrWdg->setVisible(shD->en);
	    //-- Fix widget --
	    ((QVBoxLayout*)w->layout())->addWidget(shD->addrWdg);
	}
	if( wAlign ) ((QVBoxLayout*)w->layout())->setAlignment(shD->addrWdg,wAlign);
	shD->welType = shD->elType;
    }

    shD->evLock = false;

    return true;
}

bool ShapeFormEl::event( WdgView *w, QEvent *event )
{
    if( event->type() == QEvent::Hide && qobject_cast<RunWdgView*>(w) )
    {
	ShpDt *shD = (ShpDt*)w->shpData;
	switch( shD->elType )
	{
	    case 0:	//Line edit
		((LineEdit*)shD->addrWdg)->setValue(((LineEdit*)shD->addrWdg)->value());
		break;
	    case 1:	//Text edit
		((TextEdit*)shD->addrWdg)->setText(((TextEdit*)shD->addrWdg)->text());
		break;
	}
    }
    
    return false;
}

bool ShapeFormEl::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
    if( qobject_cast<DevelWdgView*>(w) )
    {
	switch( event->type() )
	{
	    case QEvent::Enter:
	    case QEvent::Leave:		return true;
	    case QEvent::MouseMove:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::MouseButtonDblClick:
	    case QEvent::ContextMenu:
		QApplication::sendEvent(w,event);
		return true;
	}
    }
    else
	switch( event->type() )
	{
	    case QEvent::FocusIn:
		w->attrSet("focus","1");
		w->attrSet("event","ws_FocusIn");
		break;
	    case QEvent::FocusOut:
		w->attrSet("focus","0");
		w->attrSet("event","ws_FocusOut");
		break;
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
    view->attrSet("event","ws_ChkChange");
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
    WdgView *w = (WdgView *)((QPushButton*)sender())->parentWidget();
    if( ((ShpDt*)w->shpData)->evLock )	return;

    w->attrSet("event","ws_BtToggleChange");
    w->attrSet("value",TSYS::int2str(val));
}

void ShapeFormEl::comboChange(const QString &val)
{
    WdgView *w = (WdgView *)((QWidget*)sender())->parentWidget();
    if( ((ShpDt*)w->shpData)->evLock )	return;

    w->attrSet("value",val.toAscii().data());
    w->attrSet("event","ws_CombChange");
}

void ShapeFormEl::listChange( int row )
{
    QListWidget *el   = (QListWidget*)sender();
    WdgView     *w = (WdgView *)el->parentWidget();

    if( row < 0 || ((ShpDt*)w->shpData)->evLock ) return;

    w->attrSet("value",el->item(row)->text().toAscii().data());
    w->attrSet("event","ws_ListChange");
}

void ShapeFormEl::sliderMoved( int val )
{
    QAbstractSlider	*el   = (QAbstractSlider*)sender();
    WdgView		*view = (WdgView *)el->parentWidget();

    view->attrSet("value",TSYS::int2str(val));
    view->attrSet("event","ws_SliderChange");
}

void ShapeFormEl::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if( en )	wdg->installEventFilter(view);
    else	wdg->removeEventFilter(view);
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
	if( isFocus && !devel )	wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
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

void ShapeText::init( WdgView *w )
{
    w->shpData = new ShpDt();
}

void ShapeText::destroy( WdgView *w )
{
    delete (ShpDt*)w->shpData;
}

bool ShapeText::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    bool up = true,		//Update view checking
	 reform = false;	//Text reformation

    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos)
    {
	case -1:	//load
	    up = reform = true;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    shD->en = (bool)atoi(val.c_str());
	    w->setVisible( atoi(val.c_str()) && ((RunWdgView*)w)->permView() );
	    break;
	case 6:		//active
	    if( !qobject_cast<RunWdgView*>(w) ) break;
	    shD->active = (bool)atoi(val.c_str());
	    w->setFocusPolicy( (atoi(val.c_str())&&((RunWdgView*)w)->permCntr()) ? Qt::StrongFocus : Qt::NoFocus );
	    break;
	case 12:	//geomMargin
	    shD->geomMargin = atoi(val.c_str());	up = true;
	    break;
	case 20:	//backColor
	{
	    shD->backGrnd.setColor(QColor(val.c_str()));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if( !brsh.color().isValid() ) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle( brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);

	    up = true;
	    break;
	}
	case 21:	//backImg
	{
	    QImage img;
	    string backimg = w->resGet(val);
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		shD->backGrnd.setTextureImage(img);
	    else shD->backGrnd.setTextureImage(QImage());

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle( !brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern );
	    plt.setBrush(QPalette::Base,brsh);
	    w->setPalette(plt);

	    up = true;
	    break;
	}
	case 22:	//bordWidth
	    shD->border.setWidth(atoi(val.c_str()));	up = true;	break;
	case 23:	//bordColor
	    shD->border.setColor(QColor(val.c_str()));	up = true;	break;
	case 24:	//bordStyle
	    shD->bordStyle = atoi(val.c_str()); up = true; break;
	case 25:	//font
	    shD->font = getFont(val); up = true; break;
	case 26:	//color
	    shD->color = QColor(val.c_str()); break;
	case 27:	//orient
	    shD->orient = atoi(val.c_str()); break;
	case 28:	//wordWrap
	{
	    if( atoi(val.c_str()) )	shD->text_flg |= Qt::TextWordWrap;
	    else			shD->text_flg &= (~Qt::TextWordWrap);
	    break;
	}
	case 29:	//alignment
	{
	    shD->text_flg &= ~(Qt::AlignLeft|Qt::AlignRight|Qt::AlignHCenter|Qt::AlignTop|Qt::AlignBottom|Qt::AlignVCenter);
	    switch( atoi(val.c_str())&0x3 )
	    {
		case 0: shD->text_flg |= Qt::AlignLeft;		break;
		case 1: shD->text_flg |= Qt::AlignRight;	break;
		case 2: shD->text_flg |= Qt::AlignHCenter;	break;
	    }
	    switch( atoi(val.c_str())>>2 )
	    {
		case 0: shD->text_flg |= Qt::AlignTop;		break;
		case 1: shD->text_flg |= Qt::AlignBottom;	break;
		case 2: shD->text_flg |= Qt::AlignVCenter;	break;
	    }
	    break;
	}
	case 30:	//text
	{
	    if( shD->text_tmpl == val.c_str() )	break;
	    shD->text_tmpl = val;
	    reform = true;
	    break;
	}
	case 40:	//numbArg
	{
	    int numbArg = atoi(val.c_str());
	    while( shD->args.size() < numbArg )	shD->args.push_back(ArgObj());
	    while( shD->args.size() > numbArg )	shD->args.pop_back();
	    reform = true;
	    break;
	}
	default:
	    //- Individual arguments process -
	    if( uiPrmPos >= 50 )
	    {
		int argN = (uiPrmPos/10)-5;
		if( argN >= shD->args.size() )	break;
		if( (uiPrmPos%10) == 0 || (uiPrmPos%10) == 1 )
		{
		    QVariant gval = shD->args[argN].val();
		    int tp = (gval.type()==QVariant::Double) ? 1 : ((gval.type()==QVariant::String) ? 2 : 0);
		    if( (uiPrmPos%10) == 0 )	gval = val.c_str();
		    if( (uiPrmPos%10) == 1 )	tp = atoi(val.c_str());
		    switch( tp )
		    {
			case 0: shD->args[argN].setVal(gval.toInt());	break;
			case 1: shD->args[argN].setVal(gval.toDouble());	break;
			case 2: shD->args[argN].setVal(gval.toString());	break;
		    }
		}
		if( (uiPrmPos%10) == 2 ) shD->args[argN].setCfg(val.c_str());
		reform = true;
	    }else up = false;
    }

    //- Text reformation -
    if( reform && !w->allAttrLoad() )
    {
	QString text = shD->text_tmpl.c_str();
	for( int i_a = 0; i_a < shD->args.size(); i_a++ )
	{
	    switch( shD->args[i_a].val().type())
	    {
		case QVariant::String: text = text.arg(shD->args[i_a].val().toString(),atoi(shD->args[i_a].cfg().c_str())); break;
		case QVariant::Double:
		{
		    int off = 0;
		    int wdth = atoi(TSYS::strSepParse(shD->args[i_a].cfg(),0,';',&off).c_str());
		    string form = TSYS::strSepParse(shD->args[i_a].cfg(),0,';',&off);
		    int prec = atoi(TSYS::strSepParse(shD->args[i_a].cfg(),0,';',&off).c_str());
		    text = text.arg(shD->args[i_a].val().toDouble(),wdth,form.empty()?0:form[0],prec,' ');
		    break;
		}
		default: text = text.arg(shD->args[i_a].val().toInt(),atoi(shD->args[i_a].cfg().c_str())); break;
	    }
	}
	if( text != shD->text.c_str() )	{ shD->text = text.toAscii().data(); up = true; }
    }

    if( up && !w->allAttrLoad( ) && uiPrmPos != -1 ) w->update();

    return up;
}

bool ShapeText::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    if( !shD->en ) return false;
    switch(event->type())
    {
	case QEvent::Paint:
	{
	    QPainter pnt( w );

	    //- Prepare draw area -
	    QRect dA(w->rect().x(),w->rect().y(),
		(int)TSYS::realRound(w->sizeF().width()/w->xScale(true),2,true)-2*shD->geomMargin,
		(int)TSYS::realRound(w->sizeF().height()/w->yScale(true),2,true)-2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted((int)TSYS::realRound(w->xScale(true)*shD->geomMargin,2,true),(int)TSYS::realRound(w->yScale(true)*shD->geomMargin,2,true),
		-(int)TSYS::realRound(w->xScale(true)*shD->geomMargin,2,true),-(int)TSYS::realRound(w->yScale(true)*shD->geomMargin,2,true)));

	    int scale = 0;
#if QT_VERSION < 0x040400
	    if( pnt.window()!=pnt.viewport() )	scale = 1;
#endif
	    pnt.translate( dA.width()/2+scale,dA.height()/2+scale );
	    pnt.rotate(shD->orient);

	    //- Calc whidth and hight draw rect at rotate -
	    double rad_angl  = fabs(3.14159*(double)shD->orient/180.);
	    double rect_rate = 1./(fabs(cos(rad_angl))+fabs(sin(rad_angl)));
	    int wdth  = (int)(rect_rate*dA.size().width()+
			    sin(rad_angl)*(dA.size().height()-dA.size().width()));
	    int heigt = (int)(rect_rate*dA.size().height()+
			    sin(rad_angl)*(dA.size().width()-dA.size().height()));

	    QRect dR = QRect(QPoint(-wdth/2,-heigt/2),QSize(wdth,heigt));

	    //- Draw decoration -
	    if( shD->backGrnd.color().isValid() ) pnt.fillRect(dR,shD->backGrnd.color());
	    if( !shD->backGrnd.textureImage().isNull() ) pnt.fillRect(dR,shD->backGrnd.textureImage());

	    //- Draw border -
	    if( shD->border.width() )
	    {
		borderDraw( pnt, dR, shD->border, shD->bordStyle );
		dR.adjust( shD->border.width()+1, shD->border.width()+1, shD->border.width()-1, shD->border.width()-1);
	    }

	    //- Draw text -
	    pnt.setPen(shD->color);
	    pnt.setFont(shD->font);

	    pnt.drawText(dR,shD->text_flg,shD->text.c_str());

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

void ShapeMedia::init( WdgView *w )
{
    w->shpData = new ShpDt();

    //- Create label widget -
    QLabel *lab = new QLabel(w);
    if( qobject_cast<DevelWdgView*>(w) ) lab->setMouseTracking(true);
    w->setMouseTracking(true);
    lab->setAlignment(Qt::AlignCenter);
    lab->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ((ShpDt*)w->shpData)->labWdg = lab;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->addWidget(lab);
}

void ShapeMedia::destroy( WdgView *w )
{
    //- Clear label widget's elements -
    QLabel *lab = ((ShpDt*)w->shpData)->labWdg;
    if( lab && lab->movie() )
    {
	if(lab->movie()->device()) delete lab->movie()->device();
	delete lab->movie();
	lab->clear();
    }

    delete (ShpDt*)w->shpData;
}

bool ShapeMedia::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    bool up = true, reld_src = false;
    ShpDt *shD = (ShpDt*)w->shpData;

    switch( uiPrmPos )
    {
	case -1:	//load
	    reld_src = true;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    shD->en = (bool)atoi(val.c_str());
	    w->setVisible( shD->en && ((RunWdgView*)w)->permView() );
	    break;
	case 6:		//active
	    if( !qobject_cast<RunWdgView*>(w) )	break;
	    shD->labWdg->setMouseTracking( atoi(val.c_str()) && ((RunWdgView*)w)->permCntr() );
	    w->setMouseTracking( atoi(val.c_str()) && ((RunWdgView*)w)->permCntr() );
	    break;
	case 12:	//geomMargin
	    shD->geomMargin = atoi(val.c_str());
	    w->layout()->setMargin( shD->geomMargin );
	    break;
	case 20:	//backColor
	{
	    shD->backGrnd.setColor(QColor(val.c_str()));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if( !brsh.color().isValid() ) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle( brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    break;
	}
	case 21:	//backImg
	{
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage(QImage());
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		shD->backGrnd.setTextureImage(img);

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle( !brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    break;
	}
	case 22:	//bordWidth
	    shD->border.setWidth(atoi(val.c_str()));	break;
	case 23:	//bordColor
	    shD->border.setColor(QColor(val.c_str()));	break;
	case 24:	//bordStyle
	    shD->bordStyle = atoi(val.c_str());		break;
	case 25:	//src
	    if( shD->mediaSrc == val )	break;
	    shD->mediaSrc = val;
	    reld_src = true;
	    break;
	case 26:	//fit
	    shD->mediaFit = (bool)atoi(val.c_str());
	    shD->labWdg->setScaledContents( shD->mediaFit );
	    break;
	case 27:	//type
	    if( shD->mediaType == atoi(val.c_str()) )	break;
	    shD->mediaType = atoi(val.c_str());
	    reld_src = true;
	    break;
	case 28:	//areas
	{
	    int numbMAr = atoi(val.c_str());
	    while( shD->maps.size() < numbMAr )	shD->maps.push_back(MapArea());
	    while( shD->maps.size() > numbMAr )	shD->maps.pop_back();
	    break;
	}
	case 29: 	//speed
	    shD->mediaSpeed = atoi(val.c_str());
	    if( !shD->labWdg->movie() ) break;
	    if( shD->mediaSpeed <= 1 ) shD->labWdg->movie()->stop();
	    else
	    {
		shD->labWdg->movie()->setSpeed(shD->mediaSpeed);
		shD->labWdg->movie()->start();
	    }
	    break;
	default:
	    //- Individual arguments process -
	    if( uiPrmPos >= 40 )
	    {
		int areaN = (uiPrmPos-40)/3;
		if( areaN >= shD->maps.size() ) break;
		switch( (uiPrmPos-40)%3 )
		{
		    case 0:	//shape
			shD->maps[areaN].shp = atoi(val.c_str());	break;
		    case 1:	//coordinates
		    {
			string stmp;
			shD->maps[areaN].pnts.clear();
			for( int ncrd = 0, pos = 0; (stmp=TSYS::strSepParse(val,0,',',&ncrd)).size(); pos++ )
			    if( !(pos%2) ) shD->maps[areaN].pnts.push_back(QPoint(atoi(stmp.c_str()),0));
			    else           shD->maps[areaN].pnts[shD->maps[areaN].pnts.size()-1].setY(atoi(stmp.c_str()));
		    }
		    case 2:	//title
			shD->maps[areaN].title = val;	break;
		}
	    }
    }

    if( reld_src && !w->allAttrLoad() )
    {
	string sdata = w->resGet(shD->mediaSrc);
	switch( shD->mediaType )
	{
	    case 0:
	    {
		QImage img;
		//> Free movie data, if set
		if( shD->labWdg->movie() )
		{
		    if(shD->labWdg->movie()->device()) delete shD->labWdg->movie()->device();
		    delete shD->labWdg->movie();
		    shD->labWdg->clear();
		}
		//> Set new image
		if( !sdata.empty() && img.loadFromData((const uchar*)sdata.data(),sdata.size()) )
		{
		    shD->labWdg->setPixmap(QPixmap::fromImage(img.scaled(
			(int)((float)img.width()*w->xScale(true)),
			(int)((float)img.height()*w->yScale(true)),
			Qt::KeepAspectRatio,Qt::SmoothTransformation)));
		    shD->labWdg->setScaledContents( shD->mediaFit );
		}
		else shD->labWdg->setText("");
		break;
	    }
	    case 1:
	    {
		//> Clear previous movie data
		if( shD->labWdg->movie() )
		{
		    if(shD->labWdg->movie()->device()) delete shD->labWdg->movie()->device();
		    delete shD->labWdg->movie();
		    shD->labWdg->clear();
		}
		//> Set new data
		if( sdata.size() )
		{
		    QBuffer *buf = new QBuffer(w);
		    buf->setData( sdata.data(), sdata.size() );
		    buf->open( QIODevice::ReadOnly );
		    shD->labWdg->setMovie( new QMovie(buf) );

		    //> Play speed set
		    if( shD->mediaSpeed <= 1 ) shD->labWdg->movie()->stop();
		    else
		    {
			shD->labWdg->movie()->setSpeed(shD->mediaSpeed);
			shD->labWdg->movie()->start();
		    }
		    //> Fit set
		    shD->labWdg->setScaledContents( shD->mediaFit );
		    if( !shD->mediaFit && shD->labWdg->movie()->jumpToNextFrame() )
		    {
			QImage img = shD->labWdg->movie()->currentImage();
			shD->labWdg->movie()->setScaledSize(QSize((int)((float)img.width()*w->xScale(true)),(int)((float)img.height()*w->yScale(true))));
		    }
		}else shD->labWdg->setText("");

		break;
	    }
	}
    }

    if( up && !w->allAttrLoad( ) && uiPrmPos != -1 ) w->update();

    return up;
}

bool ShapeMedia::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if( !shD->en ) return false;

    switch( event->type() )
    {
	case QEvent::Paint:
	{
	    QPainter pnt( w );

	    //> Prepare draw area
	    QRect dA = w->rect().adjusted(0,0,-2*shD->geomMargin,-2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(shD->geomMargin,shD->geomMargin,-shD->geomMargin,-shD->geomMargin));

	    //> Draw decoration
	    if( shD->backGrnd.color().isValid() ) pnt.fillRect(dA,shD->backGrnd.color());
	    if( !shD->backGrnd.textureImage().isNull() ) pnt.fillRect(dA,shD->backGrnd.textureImage());

	    //> Draw border
	    borderDraw( pnt, dA, shD->border, shD->bordStyle );

	    return true;
	}
	case QEvent::MouseMove:
	{
	    Qt::CursorShape new_shp = Qt::ArrowCursor;
	    if( !shD->maps.empty() )
	    {
		for( int i_a = 0; i_a < shD->maps.size(); i_a++ )
		    if( shD->maps[i_a].containsPoint(w->mapFromGlobal(w->cursor().pos())) )
		    {
			new_shp = Qt::PointingHandCursor;
			if( !shD->maps[i_a].title.empty() ) QToolTip::showText(w->cursor().pos(),shD->maps[i_a].title.c_str());
			break;
		    }
	    }
	    else new_shp = Qt::PointingHandCursor;

	    if( new_shp != w->cursor().shape() ) w->setCursor(new_shp);

	    return true;
	}
	case QEvent::MouseButtonPress:
	{
	    string sev;
	    for( int i_a = 0; i_a < shD->maps.size(); i_a++ )
	        if( shD->maps[i_a].containsPoint(w->mapFromGlobal(w->cursor().pos())) )
	        { sev="ws_MapAct"+TSYS::int2str(i_a); break; }
	    if( !sev.empty() )
	    {
		switch(((QMouseEvent*)event)->button())
		{
		    case Qt::LeftButton:	sev+="Left";	break;
		    case Qt::RightButton:	sev+="Right";	break;
		    case Qt::MidButton:		sev+="Midle";	break;
		}
		w->attrSet("event",sev);
		return true;
	    }
	    break;
	}
    }

    return false;
}

//* Map areas structure                           *
//*************************************************
bool ShapeMedia::MapArea::containsPoint( const QPoint & point )
{
    switch( shp )
    {
	case 0:		//rect
	    if( pnts.size() < 2 ) return false;
	    return QRect(pnts[0],pnts[1]).contains(point);
	case 1:		//poly
	    return QPolygon(pnts).containsPoint(point,Qt::OddEvenFill);
	case 2: 	//circle
	{
	    if( pnts.size() < 2 ) return false;
	    QPoint work = point-pnts[0];
	    return (pow(pow((float)work.x(),2)+pow((float)work.y(),2),0.5) < pnts[0].x());
	}
    }
    return false;
}

//*************************************************
//* Diagram view shape widget                     *
//*************************************************
ShapeDiagram::ShapeDiagram( ) : WdgShape("Diagram")
{

}

void ShapeDiagram::init( WdgView *w )
{
    w->shpData = new ShpDt();

    //- Init tracing timer -
    ((ShpDt*)w->shpData)->trcTimer = new QTimer(w);
    connect( ((ShpDt*)w->shpData)->trcTimer, SIGNAL(timeout()), this, SLOT(tracing()) );
}

void ShapeDiagram::destroy( WdgView *w )
{
    ((ShpDt*)w->shpData)->trcTimer->stop();

    delete (ShpDt*)w->shpData;
}

bool ShapeDiagram::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    bool up = false,		//Repaint diagramm picture
	 make_pct = false;	//Remake diagramm picture
    int  reld_tr_dt = 0;	//Reload trend's data ( 1-reload addons, 2-full reload)

    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos)
    {
	case -1:	//load
	    up = make_pct = true;
	    reld_tr_dt = 2;
	    break;
	case -2:	//focus
	    if( (bool)atoi(val.c_str()) != w->hasFocus() )	up = true;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	break;
	    shD->en = (bool)atoi(val.c_str());
	    w->setVisible( shD->en && ((RunWdgView*)w)->permView() );
	    up = true;
	    break;
	case 6:		//active
	    shD->active = (bool)atoi(val.c_str());
	    if( !qobject_cast<RunWdgView*>(w) )	break;
	    if( shD->active && ((RunWdgView*)w)->permCntr() )	w->setFocusPolicy(Qt::StrongFocus);
	    else w->setFocusPolicy(Qt::NoFocus);
	    break;
	case 9:	case 10: make_pct = true;	break;
	case 12:	//geomMargin
	    shD->geomMargin = atoi(val.c_str()); make_pct = true; break;
	case 20:	//backColor
	{
	    shD->backGrnd.setColor( QColor(val.c_str()) );

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if( !brsh.color().isValid() ) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle( brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    up = true;
	    break;
	}
	case 21:	//backImg
	{
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage(QImage());
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		shD->backGrnd.setTextureImage(img);

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle( !brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    up = true;
	    break;
	}
	case 22:	//bordWidth
	    shD->border.setWidth(atoi(val.c_str())); make_pct = true; break;
	case 23:	//bordColor
	    shD->border.setColor(QColor(val.c_str())); up = true; break;
	case 24:	//bordStyle
	    shD->bordStyle = atoi(val.c_str()); up = true; break;
	case 25:	//trcPer
	    shD->trcPer = atoi(val.c_str());
	    if( shD->trcPer )	shD->trcTimer->start(shD->trcPer*1000);
	    else shD->trcTimer->stop();
	    break;
	case 26:	//type
	    shD->type = atoi(val.c_str());
	    reld_tr_dt = 2;
	    break;
	case 27:	//tSek
	    shD->tTimeCurent = false;
	    if( atoll(val.c_str()) == 0 )
	    {
		shD->tTime = (long long)time(NULL)*1000000;
		shD->tTimeCurent = true;
	    } else shD->tTime = atoll(val.c_str())*1000000 + shD->tTime%1000000;
	    reld_tr_dt = 1;
	    break;
	case 28: 	//tUSek
	    shD->tTime = 1000000ll*(shD->tTime/1000000)+atoll(val.c_str());
	    reld_tr_dt = 1;
	    break;
	case 29:	//tSize
	    shD->tSize = atof(val.c_str());
	    reld_tr_dt = 2;
	    break;
	case 30:	//curSek
	    if( (shD->curTime/1000000) == atoi(val.c_str()) ) break;
	    shD->curTime = atoll(val.c_str())*1000000 + shD->curTime%1000000;
	    up = true;
	    break;
	case 31:	//curUSek
	    if( (shD->curTime%1000000) == atoi(val.c_str()) ) break;
	    shD->curTime = 1000000ll*(shD->curTime/1000000)+atoll(val.c_str());
	    up = true;
	    break;
	case 32:	//curColor
	    shD->curColor = QColor(val.c_str());	up = true;		break;
	case 33:	//sclColor
	    shD->sclColor = QColor(val.c_str());	make_pct = true;	break;
	case 34:	//sclHor
	    shD->sclHor = atoi(val.c_str());		make_pct = true;	break;
	case 35:	//sclVer
	    shD->sclVer = atoi(val.c_str());		make_pct = true;	break;
	case 36:	//sclMarkColor
	    shD->sclMarkColor = QColor(val.c_str());	make_pct = true;	break;
	case 37:	//sclMarkFont
	    shD->sclMarkFont = getFont(val);		make_pct = true;	break;
	case 38:	//valArch
	    if( shD->valArch == val )	break;
	    shD->valArch = val;
	    reld_tr_dt = 2;
	    break;
	case 39:	//parNum
	{
	    int parNum = atoi(val.c_str());
	    while( shD->prms.size() < parNum )	shD->prms.push_back(TrendObj(w));
	    while( shD->prms.size() > parNum )	shD->prms.pop_back();
	    make_pct = true;
	    break;
	}
	case 40:	//sclVerScl
	    if( shD->sclVerScl == atof(val.c_str()) )	break;
	    shD->sclVerScl = atof(val.c_str());
	    make_pct = true;
	    break;
	case 41:	//sclVerSclOff
	    if( shD->sclVerSclOff == atof(val.c_str()) ) break;
	    shD->sclVerSclOff = atof(val.c_str());
	    make_pct = true;
	    break;
	default:
	    //> Individual trend's attributes process
	    if( uiPrmPos >= 50 && uiPrmPos < 150 )
	    {
		int trndN = (uiPrmPos/10)-5;
		if( trndN >= shD->prms.size() ) break;
		make_pct = true;
		switch(uiPrmPos%10)
		{
		    case 0: shD->prms[trndN].setAddr(val);			break;		//addr
		    case 1: shD->prms[trndN].setBordL(atof(val.c_str()));	break;		//bordL
		    case 2: shD->prms[trndN].setBordU(atof(val.c_str()));	break;		//bordU
		    case 3: shD->prms[trndN].setColor(QColor(val.c_str()));			break;		//color
		    case 4: shD->prms[trndN].setCurVal(atof(val.c_str())); make_pct = false;	break;		//value
		}
	    }
    }

    if( !w->allAttrLoad( ) )
    {
	if( reld_tr_dt )	{ loadData(w,reld_tr_dt==2); make_pct = true; }
	if( make_pct )		{ makePicture(w); up = true; }
	if( up && uiPrmPos != -1 )	w->update();
    }

    return (reld_tr_dt|make_pct|up);
}

void ShapeDiagram::loadData( WdgView *w, bool full )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    for( int i_p = 0; i_p < shD->prms.size(); i_p++ )
	shD->prms[i_p].loadData(full);
}

void ShapeDiagram::makePicture( WdgView *w )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if( !shD->en )	return;
    switch(shD->type)
    {
	case 0:	makeTrendsPicture(w);	break;
	case 1:	makeSpectrumPicture(w);	break;
    }
}

void ShapeDiagram::makeSpectrumPicture( WdgView *w )
{
    QPen grdPen, mrkPen;
    int  mrkHeight = 0;

    ShpDt *shD = (ShpDt*)w->shpData;

    //> Prepare picture
    QPainter pnt( &shD->pictObj );

    //> Get generic parameters
    long long tSize = (long long)(1e6*shD->tSize);			//Time size (us)
    long long tEnd  = shD->tTime;					//Time end point (us)
    long long tBeg  = tEnd - tSize;					//Time begin point (us)
    if( shD->prms.empty() || tSize <= 0 ) return;

    //> Make decoration and prepare trends area
    QRect tAr  = w->rect().adjusted(1,1,-2*(shD->geomMargin+shD->border.width()+1),-2*(shD->geomMargin+shD->border.width()+1));	//Curves of spectrum area rect
    int sclHor = shD->sclHor;						//Horisontal scale mode
    int sclVer = shD->sclVer;						//Vertical scale mode

    //> Process scale
    if( sclHor&0x3 || sclVer&0x3 )
    {
	//>> Set grid pen
	grdPen.setColor(shD->sclColor);
	grdPen.setStyle(Qt::SolidLine);
	grdPen.setWidth( (int)vmax(1.0,vmin(w->xScale(true),w->yScale(true))) );
	//>> Set markers font and color
	if( sclHor&0x2 || sclVer&0x2 )
	{
	    mrkPen.setColor(shD->sclMarkColor);
	    QFont mrkFnt = shD->sclMarkFont;
	    mrkFnt.setPixelSize( (int)((double)mrkFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true))) );
	    pnt.setFont(mrkFnt);
	    mrkHeight = pnt.fontMetrics().height()-pnt.fontMetrics().descent();
	    if( sclHor&0x2 )
	    {
		if( tAr.height() < (int)(100.0*vmin(w->xScale(true),w->yScale(true))) ) sclHor &= ~(0x02);
		else tAr.adjust(0,0,0,-mrkHeight);
	    }
	    if( sclVer&0x2 && tAr.width() < (int)(100.0*vmin(w->xScale(true),w->yScale(true))) )
		sclVer &= ~(0x02);
	}
    }

    //> Calc horizontal scale
    int fftN = w->size().width();
    double fftBeg = 1e6/(double)tSize;			//Minimum frequency or maximum period time (s)
    double fftEnd = (double)fftN*fftBeg/2;		//Maximum frequency or minimum period time (s)
    double hDiv = 1;					//Horisontal scale divisor
    int hmax_ln = tAr.width() / (int)((sclHor&0x2)?pnt.fontMetrics().width("000000"):15.0*vmin(w->xScale(true),w->yScale(true)));
    if( hmax_ln >= 2 )
    {
	double hLen = fftEnd-fftBeg;
	while( hLen/hDiv > hmax_ln )	hDiv *= 10;
	while( hLen/hDiv < hmax_ln/10)	hDiv /= 10;
	fftBeg = floor(10*fftBeg/hDiv)*hDiv/10;
	fftEnd = ceil(10*fftEnd/hDiv)*hDiv/10;
	while(((fftEnd-fftBeg)/hDiv) < hmax_ln/2) hDiv/=2;

	//>> Draw horisontal grid and markers
	if( sclHor&0x3 )
	{
	    string labH;
	    double labDiv = 1;
	    if( fftEnd>1000 ) labDiv = 1000;
	    //>>> Draw generic grid line
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y()+tAr.height(),tAr.x()+tAr.width(),tAr.y()+tAr.height());
	    //>>> Draw full trend's data and time to the trend end position
	    int begMarkBrd = -1;
	    int endMarkBrd = tAr.x()+tAr.width();
	    if( sclHor&0x2 )
	    {
		pnt.setPen(mrkPen);
		labH = TSYS::strMess("%0.4g",fftEnd/labDiv)+((labDiv==1000)?_("kHz"):_("Hz"));

		int markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().boundingRect(labH.c_str()).width();
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+mrkHeight,labH.c_str());
	    }
	    //>>> Draw grid and/or markers
	    for( double i_h = fftBeg; (fftEnd-i_h)/hDiv > -0.1; i_h+=hDiv )
	    {
		//>>>> Draw grid
		pnt.setPen(grdPen);
		int h_pos = tAr.x()+(int)((double)tAr.width()*(i_h-fftBeg)/(fftEnd-fftBeg));
		if( sclHor&0x1 ) pnt.drawLine(h_pos,tAr.y(),h_pos,tAr.y()+tAr.height());
		else pnt.drawLine(h_pos,tAr.y()+tAr.height()-3,h_pos,tAr.y()+tAr.height()+3);

		if( sclHor&0x2 )
		{
		    pnt.setPen(mrkPen);
		    labH = TSYS::strMess("%0.4g",i_h/labDiv);
		    int wdth = pnt.fontMetrics().boundingRect(labH.c_str()).width();
		    int tpos = vmax(h_pos-wdth/2,0);
		    if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			pnt.drawText( tpos, tAr.y()+tAr.height()+mrkHeight, labH.c_str() );
		    begMarkBrd = vmax(begMarkBrd,tpos+wdth);
		}
	    }
	}
    }

    int prmRealSz = -1;
    //>> Calc real parameters size
    for( int i_p = 0; i_p < shD->prms.size(); i_p++ )
	if( shD->prms[i_p].fftN && shD->prms[i_p].color().isValid() )
	{
	    if( prmRealSz == -1 ) prmRealSz = i_p;
	    else if( prmRealSz >= 0 ) prmRealSz = -2;
	    else prmRealSz -= 1;
	}

    //>> Calc vertical scale
    double vsMax = 100, vsMin = 0, curVl;	//Trend's vertical scale border
    bool   vsPerc = true;			//Vertical scale percent mode
    if( prmRealSz >= 0 )
    {
	if( !shD->prms[prmRealSz].fftN ) return;

	vsPerc = false;
	if( shD->prms[prmRealSz].bordU() > shD->prms[prmRealSz].bordL() )
	{ vsMax = shD->prms[prmRealSz].bordU(); vsMin = shD->prms[prmRealSz].bordL(); }
	else
	{
	    //>>> Calc value borders
	    vsMax = -3e300, vsMin = 3e300;
	    double vlOff = shD->prms[prmRealSz].fftOut[0][0]/shD->prms[prmRealSz].fftN;
	    for( int i_v = 1; i_v < (shD->prms[prmRealSz].fftN/2+1); i_v++ )
	    {
		curVl = vlOff+pow(pow(shD->prms[prmRealSz].fftOut[i_v][0],2)+pow(shD->prms[prmRealSz].fftOut[i_v][1],2),0.5)/(shD->prms[prmRealSz].fftN/2+1);
		vsMin = vmin(vsMin,curVl);
		vsMax = vmax(vsMax,curVl);
	    }
	    if( vsMax == vsMin )	{ vsMax += 1.0; vsMin -= 1.0; }
	    else if( (vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.001 )
	    {
		double wnt_dp = 0.001*fabs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
		vsMin -= wnt_dp/2;
		vsMax += wnt_dp/2;
	    }
	}
    }

    //>> Vertical scale and offset apply
    float vsDif = vsMax - vsMin;
    vsMax += shD->sclVerSclOff*vsDif/100; vsMin += shD->sclVerSclOff*vsDif/100;
    vsMax += (shD->sclVerScl*vsDif/100-vsDif)/2; vsMin -= (shD->sclVerScl*vsDif/100-vsDif)/2;

    double vmax_ln = tAr.height() / ( (sclVer&0x2)?(2*mrkHeight):(int)(15.0*vmin(w->xScale(true),w->yScale(true))) );
    if( vmax_ln >= 2 )
    {
	double vDiv = 1.;
	double v_len = vsMax - vsMin;
	while( v_len > vmax_ln )	{ vDiv *= 10; v_len /= 10; }
	while( v_len < vmax_ln/10 )	{ vDiv /= 10; v_len *= 10; }
	vsMin = floor(vsMin/vDiv)*vDiv;
	vsMax = ceil(vsMax/vDiv)*vDiv;
	while( ((vsMax-vsMin)/vDiv) < vmax_ln/2 ) vDiv/=2;

	//>>> Draw vertical grid and markers
	if( sclVer&0x3 )
	{
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y(),tAr.x(),tAr.height());
	    for( double i_v = vsMin; (vsMax-i_v)/vDiv > -0.1; i_v+=vDiv )
	    {
		int v_pos = tAr.y()+tAr.height()-(int)((double)tAr.height()*(i_v-vsMin)/(vsMax-vsMin));
		pnt.setPen(grdPen);
		if( sclVer&0x1 ) pnt.drawLine(tAr.x(),v_pos,tAr.x()+tAr.width(),v_pos);
		else pnt.drawLine(tAr.x()-3,v_pos,tAr.x()+3,v_pos);

		if( sclVer&0x2 )
		{
		    bool isMax = (fabs((vsMax-i_v)/vDiv) < 0.1);
		    pnt.setPen(mrkPen);
		    pnt.drawText(tAr.x()+2,v_pos-1+(isMax?mrkHeight:0),(TSYS::strMess("%0.4g",i_v)+((vsPerc&&isMax)?" %":"")).c_str());
		}
	    }
	}
    }

    //>> Draw trends
    for( int i_t = 0; i_t < shD->prms.size(); i_t++ )
    {
	TrendObj *sTr = &shD->prms[i_t];
	if( !sTr->fftN || !sTr->color().isValid() ) continue;

	//>>> Set trend's pen
	QPen trpen(sTr->color());
	trpen.setStyle(Qt::SolidLine);
	trpen.setWidth(1);
	pnt.setPen(trpen);

	double vlOff = sTr->fftOut[0][0]/sTr->fftN;
	double fftDt = (1e6/(double)tSize)*(double)w->size().width()/sTr->fftN;

	//>>> Prepare border for percent trend
	double bordL = sTr->bordL();
	double bordU = sTr->bordU();
	if( vsPerc && bordL >= bordU )
	{
	    bordU = -3e300, bordL = 3e300;
	    for( int i_v = 1; i_v < (sTr->fftN/2+1); i_v++ )
	    {
		curVl = vlOff+pow(pow(sTr->fftOut[i_v][0],2)+pow(sTr->fftOut[i_v][1],2),0.5)/(sTr->fftN/2+1);
		bordL = vmin(bordL,curVl);
		bordU = vmax(bordU,curVl);
	    }
	    double vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//>>> Draw trend
	double prevVl = EVAL_REAL;
	int curPos = 0, prevPos = 0;
	for( int i_v = 1; i_v < (sTr->fftN/2+1); i_v++ )
	{
	    curVl = vlOff+pow(pow(sTr->fftOut[i_v][0],2)+pow(sTr->fftOut[i_v][1],2),0.5)/(sTr->fftN/2+1);
	    if( vsPerc )
	    {
		curVl = 100.*(curVl-bordL)/(bordU-bordL);
		curVl = (curVl>100) ? 100 : (curVl<0) ? 0 : curVl;
	    }
	    curPos = tAr.x()+(int)((double)tAr.width()*(fftDt*i_v-fftBeg)/(fftEnd-fftBeg));

	    int c_vpos = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,(curVl-vsMin)/(vsMax-vsMin))));
	    if( prevVl == EVAL_REAL ) pnt.drawPoint(curPos,c_vpos);
	    else
	    {
		int c_vpos_prv = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,(prevVl-vsMin)/(vsMax-vsMin))));
		pnt.drawLine(prevPos,c_vpos_prv,curPos,c_vpos);
	    }
	    prevPos = curPos;
	    prevVl = curVl;
	}

	//>>> Update value on cursor
	if( shD->active && shD->tTimeCurent && shD->trcPer )
	{
	    double curFrq = vmax(vmin(1e6/(double)shD->curTime,fftEnd),fftBeg);
	    curPos = (int)(curFrq/fftDt);
	    if( curPos >= 1 && curPos < (sTr->fftN/2+1) )
	    {
		double val = sTr->fftOut[0][0]/sTr->fftN + pow(pow(sTr->fftOut[curPos][0],2)+pow(sTr->fftOut[curPos][1],2),0.5)/(sTr->fftN/2+1);
		w->attrSet(TSYS::strMess("prm%dval",i_t),TSYS::real2str(val,6),54+10*i_t);
	    }
	}
    }

    shD->fftBeg = fftBeg;
    shD->fftEnd = fftEnd;
    shD->pictRect = tAr;
}

void ShapeDiagram::makeTrendsPicture( WdgView *w )
{
    QPen grdPen, mrkPen;
    int  mrkHeight = 0;

    ShpDt *shD = (ShpDt*)w->shpData;

    //> Prepare picture
    QPainter pnt( &shD->pictObj );

    //> Get generic parameters
    long long tSize = (long long)(1e6*shD->tSize);			//Trends size (us)
    long long tEnd  = shD->tTime;					//Trends end point (us)
    long long tPict = tEnd;
    long long tBeg  = tEnd - tSize;					//Trends begin point (us)
    if( shD->prms.empty() || tSize <= 0 ) return;

    //> Make decoration and prepare trends area
    QRect tAr  = w->rect().adjusted(1,1,-2*(shD->geomMargin+shD->border.width()+1),-2*(shD->geomMargin+shD->border.width()+1));	//Curves of trends area rect
    int sclHor = shD->sclHor;						//Horisontal scale mode
    int sclVer = shD->sclVer;						//Vertical scale mode

    //> Process scale
    if( sclHor&0x3 || sclVer&0x3 )
    {
	//>> Set grid pen
	grdPen.setColor(shD->sclColor);
	grdPen.setStyle(Qt::SolidLine);
	grdPen.setWidth( (int)vmax(1.0,vmin(w->xScale(true),w->yScale(true))) );
	//>> Set markers font and color
	if( sclHor&0x2 || sclVer&0x2 )
	{
	    mrkPen.setColor(shD->sclMarkColor);
	    QFont mrkFnt = shD->sclMarkFont;
	    mrkFnt.setPixelSize( (int)((double)mrkFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true))) );
	    pnt.setFont(mrkFnt);
	    mrkHeight = pnt.fontMetrics().height()-pnt.fontMetrics().descent();

	    if( sclHor&0x2 )
	    {
		if( tAr.height() < (int)(100.0*vmin(w->xScale(true),w->yScale(true))) ) sclHor &= ~(0x02);
		else tAr.adjust(0,0,0,-2*mrkHeight);
	    }
	    if( sclVer&0x2 && tAr.width() < (int)(100.0*vmin(w->xScale(true),w->yScale(true))) )
		sclVer &= ~(0x02);
	}
    }

    //> Calc horizontal scale
    long long hDiv = 1;	//Horisontal scale divisor

    int hmax_ln = tAr.width() / (int)((sclHor&0x2)?pnt.fontMetrics().width("000000"):15.0*vmin(w->xScale(true),w->yScale(true)));
    if( hmax_ln >= 2 )
    {
	int hvLev = 0;
	long long hLen = tEnd - tBeg;
	if( hLen/86400000000ll >= 2 )		{ hvLev = 5; hDiv = 86400000000ll; }	//Days
	else if( hLen/3600000000ll >= 2 )	{ hvLev = 4; hDiv =  3600000000ll; }	//Hours
	else if( hLen/60000000 >= 2 )		{ hvLev = 3; hDiv =    60000000; }	//Minutes
	else if( hLen/1000000 >= 2 )		{ hvLev = 2; hDiv =     1000000; }	//Seconds
	else if( hLen/1000 >= 2 )		{ hvLev = 1; hDiv =        1000; }	//Milliseconds
	while( hLen/hDiv > hmax_ln )	hDiv *= 10;
	while( hLen/hDiv < hmax_ln/2 )	hDiv /= 2;

	if( (hLen/hDiv) >= 5 && shD->trcPer )
	{
	    tPict = hDiv*(tEnd/hDiv+1);
	    tBeg = tPict-hLen;
	}

	//>>> Draw horisontal grid and markers
	if( sclHor&0x3 )
	{
	    time_t tm_t;
	    struct tm ttm, ttm1;
	    string lab_tm, lab_dt;
	    //>>>> Draw generic grid line
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y()+tAr.height(),tAr.x()+tAr.width(),tAr.y()+tAr.height());
	    //>>>> Draw full trend's data and time to the trend end position
	    int begMarkBrd = -1;
	    int endMarkBrd = tAr.x()+tAr.width();
	    if( sclHor&0x2 )
	    {
		pnt.setPen(mrkPen);
		tm_t = tPict/1000000;
		localtime_r(&tm_t,&ttm);
		lab_dt = TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
		if( ttm.tm_sec == 0 && tPict%1000000 == 0 ) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		else if( tPict%1000000 == 0 ) lab_tm = TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
		else lab_tm = TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(tPict%1000000)/1e6);
		int markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().boundingRect(lab_tm.c_str()).width();
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+mrkHeight,lab_tm.c_str());
		markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().boundingRect(lab_dt.c_str()).width();
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+2*mrkHeight,lab_dt.c_str());
	    }
	    //>>>> Draw grid and/or markers
	    bool first_m = true;
	    for( long long i_h = tBeg; true; )
	    {
		//>>>> Draw grid
		pnt.setPen(grdPen);
		int h_pos = tAr.x()+tAr.width()*(i_h-tBeg)/(tPict-tBeg);
		if( sclHor&0x1 ) pnt.drawLine(h_pos,tAr.y(),h_pos,tAr.y()+tAr.height());
		else pnt.drawLine(h_pos,tAr.y()+tAr.height()-3,h_pos,tAr.y()+tAr.height()+3);

		if( sclHor&0x2 && !(i_h%hDiv) && i_h != tPict )
		{
		    tm_t = i_h/1000000;
		    localtime_r(&tm_t,&ttm);
		    int chLev = -1;
		    if( !first_m )
		    {
			if( ttm.tm_mon > ttm1.tm_mon || ttm.tm_year > ttm1.tm_year )	chLev = 5;
			else if( ttm.tm_mday > ttm1.tm_mday )	chLev = 4;
			else if( ttm.tm_hour > ttm1.tm_hour )	chLev = 3;
			else if( ttm.tm_min > ttm1.tm_min )	chLev = 2;
			else if( ttm.tm_sec > ttm1.tm_sec )	chLev = 1;
			else chLev = 0;
		    }

		    //Check for data present
		    lab_dt.clear(), lab_tm.clear();
		    //Date
		    if( hvLev == 5 || chLev >= 4 )
			lab_dt = (chLev>=5 || chLev==-1) ? TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900) : TSYS::strMess("%d",ttm.tm_mday);
		    //Hours and minuts
		    if( (hvLev == 4 || hvLev == 3 || ttm.tm_hour || ttm.tm_min) && !ttm.tm_sec ) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		    //Seconds
		    else if( (hvLev == 2 || ttm.tm_sec) && !(i_h%1000000) )
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) : TSYS::strMess(_("%ds"),ttm.tm_sec);
		    //Milliseconds
		    else if( hvLev <= 1 || i_h%1000000 )
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
				 (chLev>=1) ? TSYS::strMess(_("%gs"),(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
					      TSYS::strMess(_("%gms"),(double)(i_h%1000000)/1000.);
		    int wdth, tpos, endPosTm = 0, endPosDt = 0;
		    pnt.setPen(mrkPen);
		    if( lab_tm.size() )
		    {
			wdth = pnt.fontMetrics().boundingRect(lab_tm.c_str()).width();
			tpos = vmax(h_pos-wdth/2,0);
			if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			{
			    pnt.drawText( tpos, tAr.y()+tAr.height()+mrkHeight, lab_tm.c_str() );
			    endPosTm = tpos+wdth;
			}
		    }
		    if( lab_dt.size() )
		    {
			wdth = pnt.fontMetrics().boundingRect(lab_dt.c_str()).width();
			tpos = vmax(h_pos-wdth/2,0);
			if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			{
			    pnt.drawText( tpos, tAr.y()+tAr.height()+2*mrkHeight, lab_dt.c_str() );
			    endPosDt = tpos+wdth;
			}
		    }
		    begMarkBrd = vmax(begMarkBrd,vmax(endPosTm,endPosDt));
		    memcpy((char*)&ttm1,(char*)&ttm,sizeof(tm));
		    first_m = false;
		}
		//>>>> Next
		if( i_h >= tPict )	break;
		i_h = (i_h/hDiv)*hDiv + hDiv;
		if( i_h > tPict )	i_h = tPict;
	    }
	}
    }

    int prmRealSz = -1;
    //>> Calc real parameters size
    for( int i_p = 0; i_p < shD->prms.size(); i_p++ )
	if( shD->prms[i_p].val().size() && shD->prms[i_p].color().isValid() )
	{
	    if( prmRealSz == -1 ) prmRealSz = i_p;
	    else if( prmRealSz >= 0 ) prmRealSz = -2;
	    else prmRealSz -= 1;
	}

    //>> Calc vertical scale
    long long aVend;			//Corrected for allow data the trend end point
    long long aVbeg;			//Corrected for allow data the trend begin point
    double vsMax = 100, vsMin = 0;	//Trend's vertical scale border
    bool   vsPerc = true;		//Vertical scale percent mode
    if( prmRealSz >= 0 )
    {
	vsPerc = false;
	if( shD->prms[prmRealSz].bordU() <= shD->prms[prmRealSz].bordL() )
	{
	    //>>> Check trend for valid data
	    aVbeg = vmax(tBeg,shD->prms[prmRealSz].valBeg());
	    aVend = vmin(tEnd,shD->prms[prmRealSz].valEnd());

	    if( aVbeg >= aVend ) return;
	    //>>> Calc value borders
	    vsMax = -3e300, vsMin = 3e300;
	    bool end_vl = false;
	    int ipos = shD->prms[prmRealSz].val(aVbeg);
	    if( ipos && shD->prms[prmRealSz].val()[ipos].tm > aVbeg ) ipos--;
	    while( true )
	    {
		if( ipos >= shD->prms[prmRealSz].val().size() || end_vl )	break;
		if( shD->prms[prmRealSz].val()[ipos].tm >= aVend )	end_vl = true;
		if( shD->prms[prmRealSz].val()[ipos].val != EVAL_REAL )
		{
		    vsMin  = vmin(vsMin,shD->prms[prmRealSz].val()[ipos].val);
		    vsMax  = vmax(vsMax,shD->prms[prmRealSz].val()[ipos].val);
		}
		ipos++;
	    }
	    if( vsMax == -3e300 )	{ vsMax = 1.0; vsMin = 0.0; }
	    else if( vsMax == vsMin )	{ vsMax += 1.0; vsMin -= 1.0; }
	    else if( (vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.001 )
	    {
		double wnt_dp = 0.001*fabs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
		vsMin -= wnt_dp/2;
		vsMax += wnt_dp/2;
	    }
	}
	else { vsMax = shD->prms[prmRealSz].bordU(); vsMin = shD->prms[prmRealSz].bordL(); }
    }

    //>> Vertical scale and offset apply
    float vsDif = vsMax - vsMin;
    vsMax += shD->sclVerSclOff*vsDif/100; vsMin += shD->sclVerSclOff*vsDif/100;
    vsMax += (shD->sclVerScl*vsDif/100-vsDif)/2; vsMin -= (shD->sclVerScl*vsDif/100-vsDif)/2;

    float vmax_ln = tAr.height() / ( (sclVer&0x2)?(2*mrkHeight):(int)(15.0*vmin(w->xScale(true),w->yScale(true))) );
    if( vmax_ln >= 2 )
    {
	double vDiv = 1.;
	double v_len = vsMax - vsMin;
	while( v_len > vmax_ln )	{ vDiv *= 10; v_len /= 10; }
	while( v_len < vmax_ln/10 )	{ vDiv /= 10; v_len *= 10; }
	vsMin = floor(vsMin/vDiv)*vDiv;
	vsMax = ceil(vsMax/vDiv)*vDiv;
	while( ((vsMax-vsMin)/vDiv) < vmax_ln/2 ) vDiv/=2;

	//>>> Draw vertical grid and markers
	if( sclVer&0x3 )
	{
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y(),tAr.x(),tAr.height());
	    for( double i_v = vsMin; (vsMax-i_v)/vDiv > -0.1; i_v+=vDiv )
	    {
		int v_pos = tAr.y()+tAr.height()-(int)((double)tAr.height()*(i_v-vsMin)/(vsMax-vsMin));
		pnt.setPen(grdPen);
		if( sclVer&0x1 ) pnt.drawLine(tAr.x(),v_pos,tAr.x()+tAr.width(),v_pos);
		else pnt.drawLine(tAr.x()-3,v_pos,tAr.x()+3,v_pos);
		
		if( sclVer&0x2 )
		{
		    bool isMax = (fabs((vsMax-i_v)/vDiv) < 0.1);
		    pnt.setPen(mrkPen);
		    pnt.drawText(tAr.x()+2,v_pos-1+(isMax?mrkHeight:0),(TSYS::strMess("%0.4g",i_v)+((vsPerc&&isMax)?" %":"")).c_str());
		}
	    }
	}
    }

    //>> Draw trends
    for( int i_t = 0; i_t < shD->prms.size(); i_t++ )
    {
	TrendObj *sTr = &shD->prms[i_t];

	//>>> Set trend's pen
	QPen trpen(sTr->color());
	trpen.setStyle(Qt::SolidLine);
	trpen.setWidth(1);
	pnt.setPen(trpen);

	//>>> Prepare generic parameters
	aVbeg = vmax(tBeg,sTr->valBeg());
	aVend = vmin(tEnd,sTr->valEnd());

	if( aVbeg >= aVend || !sTr->color().isValid() ) continue;
	int aPosBeg = sTr->val(aVbeg);
	if( aPosBeg && sTr->val()[aPosBeg].tm > aVbeg ) aPosBeg--;

	//>>> Prepare border for percent trend
	float bordL = sTr->bordL();
	float bordU = sTr->bordU();
	if( vsPerc && bordL >= bordU )
	{
	    bordU = -3e300, bordL = 3e300;
	    bool end_vl = false;
	    int ipos = aPosBeg;
	    while( true )
	    {
		if( ipos >= sTr->val().size() || end_vl )	break;
		if( sTr->val()[ipos].tm >= aVend )	end_vl = true;
		if( sTr->val()[ipos].val != EVAL_REAL )
		{
		    bordL = vmin(bordL,sTr->val()[ipos].val);
		    bordU = vmax(bordU,sTr->val()[ipos].val);
		}
		ipos++;
	    }
	    float vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//>>> Draw trend
	bool end_vl = false;
	double curVl, averVl = EVAL_REAL, prevVl = EVAL_REAL;
	int    curPos, averPos = 0, prevPos = 0;
	long long curTm, averTm = 0, averLstTm = 0;
	for( int a_pos = aPosBeg; true; a_pos++ )
	{
	    curTm = vmin(aVend,vmax(aVbeg,sTr->val()[a_pos].tm));
	    if( a_pos < sTr->val().size() && !end_vl )
	    {
		curVl = sTr->val()[a_pos].val;
		if( vsPerc && curVl != EVAL_REAL )
		{
		    curVl = 100.*(curVl-bordL)/(bordU-bordL);
		    curVl = (curVl>100) ? 100 : (curVl<0) ? 0 : curVl;
		}
		if( isnan(curVl) ) curVl = EVAL_REAL;
		curPos = tAr.x()+tAr.width()*(curTm-tBeg)/(tPict-tBeg);
	    }else curPos = 0;
	    if( sTr->val()[a_pos].tm >= aVend )	end_vl = true;

	    //Square Average
	    if( averPos == curPos )
	    {
		if( !(2*curTm-averTm-averLstTm) ) continue;
		if( averVl == EVAL_REAL ) averVl = curVl;
		else if( curVl != EVAL_REAL )
		    averVl = (averVl*(double)(curTm-averTm)+curVl*(double)(curTm-averLstTm))/
			((double)(2*curTm-averTm-averLstTm));
		averLstTm = curTm;
		continue;
	    }

	    //Write point and line
	    if( averVl != EVAL_REAL )
	    {
		int c_vpos = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,(averVl-vsMin)/(vsMax-vsMin))));
		if( prevVl == EVAL_REAL ) pnt.drawPoint(averPos,c_vpos);
		else
		{
		    int c_vpos_prv = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,(prevVl-vsMin)/(vsMax-vsMin))));
		    pnt.drawLine(prevPos,c_vpos_prv,averPos,c_vpos);
		}
	    }
	    prevVl  = averVl;
	    prevPos = averPos;
	    averVl  = curVl;
	    averPos = curPos;
	    averTm  = averLstTm = curTm;
	    if( !curPos ) break;
	}
    }

    shD->pictRect = tAr;
    shD->tPict = tPict;
}

void ShapeDiagram::tracing( )
{
    WdgView *w = (WdgView *)((QTimer*)sender())->parent();
    ShpDt *shD = (ShpDt*)w->shpData;

    if( !w->isEnabled() ) return;

    long long trcPer = (long long)shD->trcPer*1000000;
    if( shD->tTimeCurent )	shD->tTime = (long long)time(NULL)*1000000;
    else if( shD->tTime )	shD->tTime += trcPer;
    loadData(w);
    makePicture(w);

    //> Trace cursors value
    if( shD->type == 0 && shD->active && (shD->holdCur || shD->curTime <= (shD->tPict-(long long)(1e6*shD->tSize))) )
	setCursor( w, shD->tTime );
    w->update();
}

bool ShapeDiagram::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    if( !shD->en ) return false;

    //> Process event
    switch( event->type() )
    {
	case QEvent::Paint:
	{
	    QPainter pnt( w );
	
	    //> Decoration draw
	    QRect dA = w->rect().adjusted(0,0,-2*shD->geomMargin,-2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(shD->geomMargin,shD->geomMargin,-shD->geomMargin,-shD->geomMargin));
	
	    //> Draw decoration
	    if( shD->backGrnd.color().isValid() ) pnt.fillRect(dA,shD->backGrnd.color());
	    if( !shD->backGrnd.textureImage().isNull() ) pnt.fillRect(dA,shD->backGrnd.textureImage());

	    //> Draw border
	    borderDraw( pnt, dA, shD->border, shD->bordStyle );

	    //> Trend's picture
	    pnt.drawPicture(shD->border.width(),shD->border.width(),shD->pictObj);

	    //> Draw focused border
	    if( w->hasFocus() )	qDrawShadeRect(&pnt,dA.x(),dA.y(),dA.width(),dA.height(),w->palette());

	    //> Draw cursor
	    int curPos = -1;
	    if( shD->type == 0 && shD->active )
	    {
		long long tTimeGrnd = shD->tPict - (long long)(1e6*shD->tSize);
		long long curTime = vmax(vmin(shD->curTime,shD->tPict),tTimeGrnd);
		if( curTime && tTimeGrnd && shD->tPict && (curTime >= tTimeGrnd || curTime <= shD->tPict) )
		    curPos = shD->pictRect.x()+shD->pictRect.width()*(curTime-tTimeGrnd)/(shD->tPict-tTimeGrnd);
	    }
	    else if( shD->type == 1 && shD->active && shD->tSize )
	    {
		float curFrq = vmax(vmin(1e6/(float)shD->curTime,shD->fftEnd),shD->fftBeg);
		curPos = shD->pictRect.x()+(int)(shD->pictRect.width()*(curFrq-shD->fftBeg)/(shD->fftEnd-shD->fftBeg));
	    }
	    if( curPos >= 0 && curPos <= shD->pictRect.width() )
	    {
		QPen curpen(shD->curColor);
		curpen.setWidth(1);
		pnt.setPen(curpen);
		pnt.drawLine(curPos,shD->pictRect.y(),curPos,shD->pictRect.y()+shD->pictRect.height());
	    }

	    return true;
	}
	case QEvent::KeyPress:
	{
	    QKeyEvent *key = static_cast<QKeyEvent*>(event);

	    switch( key->key() )
	    {
		case Qt::Key_Left: case Qt::Key_Right:
		    if( !shD->active ) break;
		    if( shD->type == 0 )
		    {
			long long tTimeGrnd = shD->tPict - (long long)(1e6*shD->tSize);
			long long curTime = vmax(vmin(shD->curTime,shD->tPict),tTimeGrnd);
			setCursor( w, curTime+((key->key()==Qt::Key_Left)?-1:1)*(shD->tTime-tTimeGrnd)/shD->pictRect.width() );
		    }
		    else if( shD->type == 1 )
		    {
			if( !shD->tSize ) break;
			float curFrq = vmax(vmin(1e6/(float)shD->curTime,shD->fftEnd),shD->fftBeg);
			setCursor( w, (long long)(1e6/(curFrq+((key->key()==Qt::Key_Left)?-1:1)*(shD->fftEnd-shD->fftBeg)/shD->pictRect.width())) );
		    }
		    w->update();
		    return true;
	    }
	    break;
	}
	case QEvent::MouseButtonPress:
	{
	    if( !shD->active || !w->hasFocus() ) break;
	    QPoint curp = w->mapFromGlobal(w->cursor().pos());
	    if( curp.x() < shD->pictRect.x() || curp.x() > (shD->pictRect.x()+shD->pictRect.width()) ) break;
	    if( shD->type == 0 )
	    {
		long long tTimeGrnd = shD->tPict - (long long)(1e6*shD->tSize);
		setCursor( w, tTimeGrnd + (shD->tPict-tTimeGrnd)*(curp.x()-shD->pictRect.x())/shD->pictRect.width() );
	    }
	    else if( shD->type == 1 )
		setCursor( w, (long long)(1e6/(shD->fftBeg+(shD->fftEnd-shD->fftBeg)*(curp.x()-shD->pictRect.x())/shD->pictRect.width())) );
	    w->update();
	    break;
	}
    }

    return false;
}

void ShapeDiagram::setCursor( WdgView *w, long long itm )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    if( shD->type == 0 )
    {
	long long tTimeGrnd = shD->tTime - (long long)(1e6*shD->tSize);
	long long curTime   = vmax(vmin(itm,shD->tTime),tTimeGrnd);

	shD->holdCur = (curTime==shD->tTime);

	w->setAllAttrLoad(true);
	w->attrSet("curSek",TSYS::int2str(curTime/1000000),30);
	w->attrSet("curUSek",TSYS::int2str(curTime%1000000),31);

	//> Update trend's current values
	for( int i_p = 0; i_p < shD->prms.size(); i_p++ )
	{
	    int vpos = shD->prms[i_p].val(curTime);
	    if( !shD->prms[i_p].val().size() || (!shD->holdCur && vpos >= shD->prms[i_p].val().size()) ) continue;
	    vpos = vmax(0,vmin(shD->prms[i_p].val().size()-1,vpos));
	    if( vpos && shD->prms[i_p].val()[vpos].tm > curTime )	vpos--;
	    double val = shD->prms[i_p].val()[vpos].val;
	    if( val != shD->prms[i_p].curVal() )
		w->attrSet(TSYS::strMess("prm%dval",i_p),TSYS::real2str(val,6),54+10*i_p);
	}
	w->setAllAttrLoad(false);
    }
    else if( shD->type == 1 )
    {
	float curFrq = vmax(vmin(1e6/(float)itm,shD->fftEnd),shD->fftBeg);

	w->setAllAttrLoad(true);
	w->attrSet("curSek",TSYS::int2str(((long long)(1e6/curFrq))/1000000),30);
	w->attrSet("curUSek",TSYS::int2str(((long long)(1e6/curFrq))%1000000),31);

	//> Update trend's current values
	for( int i_p = 0; i_p < shD->prms.size(); i_p++ )
	{
	    if( !shD->prms[i_p].fftN )	continue;
	    float fftDt = (1/shD->tSize)*(float)w->size().width()/shD->prms[i_p].fftN;
	    int vpos = (int)(curFrq/fftDt);
	    double val = EVAL_REAL;
	    if( vpos >= 1 && vpos < (shD->prms[i_p].fftN/2+1) )
		val = shD->prms[i_p].fftOut[0][0]/shD->prms[i_p].fftN +
		    pow(pow(shD->prms[i_p].fftOut[vpos][0],2)+pow(shD->prms[i_p].fftOut[vpos][1],2),0.5)/(shD->prms[i_p].fftN/2+1);
	    w->attrSet(TSYS::strMess("prm%dval",i_p),TSYS::real2str(val,6),54+10*i_p);
	}
	w->setAllAttrLoad(false);
    }
}

//* Trend object's class                         *
//************************************************
ShapeDiagram::TrendObj::TrendObj( WdgView *iview ) : view(iview),
    m_bord_low(0), m_bord_up(0), m_curvl(EVAL_REAL),
    arh_beg(0), arh_end(0), arh_per(0), val_tp(0), fftN(0), fftOut(NULL)
{
    loadData();
}

ShapeDiagram::TrendObj::~TrendObj( )
{
    if( fftOut ) { delete fftOut; fftN = 0; }
}

long long ShapeDiagram::TrendObj::valBeg()
{
    return vals.empty() ? 0 : vals[0].tm;
}

long long ShapeDiagram::TrendObj::valEnd()
{
    return vals.empty() ? 0 : vals[vals.size()-1].tm;
}

int ShapeDiagram::TrendObj::val( long long tm )
{
    int i_p = 0;
    for( int d_win = vals.size()/2; d_win > 10; d_win/=2 )
	if( tm < vals[i_p+d_win].tm )	i_p+=d_win;
    for( int i_p = 0; i_p < vals.size(); i_p++ )
	if( vals[i_p].tm >= tm ) return i_p;
    return vals.size();
}

void ShapeDiagram::TrendObj::setAddr( const string &vl )
{
    if( vl == m_addr ) return;
    m_addr = vl;
    loadData( true );
}

void ShapeDiagram::TrendObj::loadData( bool full )
{
    ShpDt *shD = (ShpDt*)view->shpData;
    switch( shD->type )
    {
	case 0: loadTrendsData(full);	break;
	case 1: loadSpectrumData(full);	break;
    }
}

void ShapeDiagram::TrendObj::loadTrendsData( bool full )
{
    ShpDt *shD = (ShpDt*)view->shpData;

    long long tSize     = (long long)(1e6*shD->tSize);
    long long tTime     = shD->tTime;
    long long tTimeGrnd = tTime - tSize;
    long long wantPer = tSize/view->size().width();

    //> Clear trend for empty address and for full reload data
    if( full || addr().empty() )
    {
	arh_per = arh_beg = arh_end = 0;
	val_tp = 0;
	vals.clear();
	if( addr().empty() )	return;
    }
    //> Get archive parameters
    if( !arh_per || tTime > arh_end )
    {
	XMLNode req("info");
	req.setAttr("arch",shD->valArch)->setAttr("path",addr()+"/%2fserv%2fval");
	if( view->cntrIfCmd(req,true) || atoi(req.attr("vtp").c_str()) == 5 )
	{ arh_per = arh_beg = arh_end = 0; return; }
	else
	{
	    val_tp  = atoi(req.attr("vtp").c_str());
	    arh_beg = atoll(req.attr("beg").c_str());
	    arh_end = atoll(req.attr("end").c_str());
	    arh_per = atoi(req.attr("per").c_str());
	}
    }

    //> One request check and prepare
    int trcPer = shD->trcPer*1000000;
    if( shD->tTimeCurent && trcPer && shD->valArch.empty() && (!arh_per || (arh_per >= trcPer && (tTime-valEnd())/vmax(wantPer,trcPer) < 2)) )
    {
	XMLNode req("get");
	req.setAttr("path",addr()+"/%2fserv%2fval")->
	    setAttr("tm",TSYS::ll2str(tTime))->
	    setAttr("tm_grnd","0");
	if( view->cntrIfCmd(req,true) )	return;

	long long lst_tm = atoll(req.attr("tm").c_str());
	if( lst_tm > valEnd() )
	{
	    double curVal = atof(req.text().c_str());
	    if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	    if( valEnd() && (lst_tm-valEnd())/vmax(wantPer,trcPer) > 2 ) vals.push_back(SHg(lst_tm-trcPer,EVAL_REAL));
	    else if( (lst_tm-valEnd()) >= wantPer ) vals.push_back(SHg(lst_tm,curVal));
	    else if( vals[vals.size()-1].val == EVAL_REAL ) vals[vals.size()-1].val = curVal;
	    else if( curVal != EVAL_REAL )
	    {
		int s_k = lst_tm-wantPer*(lst_tm/wantPer), n_k = trcPer;
		vals[vals.size()-1].val = (vals[vals.size()-1].val*s_k+curVal*n_k)/(s_k+n_k);
	    }
	    while( vals.size() > 2000 )	vals.pop_front();
	}
	return;
    }

    if( !arh_per )	return;
    //> Correct request to archive border
    wantPer   = (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime     = vmin(tTime,arh_end);
    tTimeGrnd = vmax(tTimeGrnd,arh_beg);

    //> Clear data at time error
    if( tTime <= tTimeGrnd || tTimeGrnd/wantPer > valEnd()/wantPer || tTime/wantPer < valBeg()/wantPer )
	vals.clear();
    if( tTime <= tTimeGrnd ) return;
    //> Check for request to present in buffer data
    if( tTime/wantPer <= valEnd()/wantPer && tTimeGrnd/wantPer >= valBeg()/wantPer )	return;
    //> Correct request to present data
    if( valEnd() && tTime > valEnd() )		tTimeGrnd = valEnd()+1;
    else if( valBeg() && tTimeGrnd < valBeg() )	tTime = valBeg()-1;

    //> Get values data
    long long bbeg, bend, bper;
    int		curPos, prevPos;
    double	curVal, prevVal;
    string	svl;
    vector<SHg>	buf;
    bool toEnd = (tTimeGrnd >= valEnd());
    int  endBlks = 0;
    XMLNode req("get");
    m1:	req.clear()->
	    setAttr("arch",shD->valArch)->
	    setAttr("path",addr()+"/%2fserv%2fval")->
	    setAttr("tm",TSYS::ll2str(tTime))->
	    setAttr("tm_grnd",TSYS::ll2str(tTimeGrnd))->
	    setAttr("per",TSYS::ll2str(wantPer))->
	    setAttr("mode","1")->
	    setAttr("real_prec","6")->
	    setAttr("round_perc","0");//TSYS::real2str(100.0/(float)view->size().height()));
    if( view->cntrIfCmd(req,true) )	return;

    //> Get data buffer parameters
    bbeg = atoll(req.attr("tm_grnd").c_str());
    bend = atoll(req.attr("tm").c_str());
    bper = atoll(req.attr("per").c_str());

    if( !bbeg || !bend || req.text().empty() ) return;

    prevPos = 0;
    prevVal = EVAL_REAL;
    buf.clear();
    for( int v_off = 0; true; )
    {
	svl = TSYS::strSepParse(req.text(),0,'\n',&v_off);
	if( svl.size() )
	{
	    sscanf(svl.c_str(),"%d %lf",&curPos,&curVal);
	    if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	}
	else curPos = ((bend-bbeg)/bper)+1;
	for( ; prevPos < curPos; prevPos++ ) buf.push_back(SHg(bbeg+prevPos*bper,prevVal));
	prevVal = curVal;
	if( prevPos > (bend-bbeg)/bper ) break;
    }

    //> Append buffer to values deque
    if( toEnd )
    {
	vals.insert(vals.end()-endBlks,buf.begin(),buf.end());
	while( vals.size() > 2000 )	vals.pop_front();
	endBlks+=buf.size();
    }
    else
    {
	vals.insert(vals.begin(),buf.begin(),buf.end());
	while( vals.size() > 2000 )	vals.pop_back();
    }
    //> Check for archive jump
    if( shD->valArch.empty() && (bbeg-tTimeGrnd)/bper )	{ tTime = bbeg-bper; goto m1; }
}

void ShapeDiagram::TrendObj::loadSpectrumData( bool full )
{
    ShpDt *shD = (ShpDt*)view->shpData;

    loadTrendsData(true);

    if( !valBeg( ) || !valEnd( ) ) return;

    if( fftOut ) { delete fftOut; fftN = 0; }

    long long tSize	= (long long)(1e6*shD->tSize);
    long long tTime	= shD->tTime;
    long long tTimeGrnd	= tTime - tSize;
    long long workPer	= tSize/view->size().width();

    tTimeGrnd = vmax(tTimeGrnd,valBeg());
    tTime = vmin(tTime,valEnd());

    fftN = (tTime-tTimeGrnd)/workPer;
    double fftIn[fftN];
    fftOut = (fftw_complex*)malloc(sizeof(fftw_complex)*(fftN/2+1));

    int fftFirstPos = -1, fftLstPos = -1;
    for( int a_pos = val(tTimeGrnd); a_pos < val().size() && val()[a_pos].tm <= tTime; a_pos++ )
    {
	int fftPos = (val()[a_pos].tm-tTimeGrnd)/workPer;
	if( fftPos >= fftN ) break;
	if( val()[a_pos].val == EVAL_REAL ) continue;
	if( fftFirstPos < 0 ) fftFirstPos = fftPos;

	if( fftLstPos == fftPos ) fftIn[fftPos-fftFirstPos] = (fftIn[fftPos-fftFirstPos]+val()[a_pos].val)/2;
	else fftIn[fftPos-fftFirstPos] = val()[a_pos].val;

	for( ; fftLstPos >= 0 && (fftLstPos+1) < fftPos; fftLstPos++ )
	    fftIn[fftLstPos-fftFirstPos+1] = fftIn[fftLstPos-fftFirstPos];
	fftLstPos = fftPos;
    }

    fftN = fftLstPos-fftFirstPos;
    if( fftN < 20 ) { delete fftOut; fftOut = NULL; fftN = 0; return; }

    fftw_plan p = fftw_plan_dft_r2c_1d( fftN, fftIn, fftOut, FFTW_ESTIMATE );
    fftw_execute(p);
    fftw_destroy_plan(p);
}

//************************************************
//* Protocol view shape widget                   *
//************************************************
ShapeProtocol::ShapeProtocol( ) : WdgShape("Protocol")
{

}

void ShapeProtocol::init( WdgView *w )
{
    w->shpData = new ShpDt();

    ShpDt *shD = (ShpDt*)w->shpData;

    //> Init main widget
    QVBoxLayout *lay = new QVBoxLayout(w);
    shD->addrWdg = new QTableWidget(w);
    shD->addrWdg->setSelectionBehavior(QAbstractItemView::SelectRows);
    //shD->addrWdg->setSortingEnabled(true);
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    eventFilterSet(w,shD->addrWdg,true);
    w->setFocusProxy( shD->addrWdg );
    if( qobject_cast<DevelWdgView*>(w) ) setFocus(w,shD->addrWdg,false,true);
    lay->addWidget(shD->addrWdg);
    //> Init tracing timer
    shD->trcTimer = new QTimer(w);
    connect( shD->trcTimer, SIGNAL(timeout()), this, SLOT(tracing()) );

    //> Bckground palette init
    QPalette plt(shD->addrWdg->palette());
    plt.setBrush(QPalette::Background,QPalette().brush(QPalette::Background));
    shD->addrWdg->setPalette(plt);
}

void ShapeProtocol::destroy( WdgView *w )
{
    ((ShpDt*)w->shpData)->trcTimer->stop();

    delete (ShpDt*)w->shpData;
}

bool ShapeProtocol::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    int  reld_dt = 0;	//Reload data ( 1-reload addons, 2-full reload )

    ShpDt *shD = (ShpDt*)w->shpData;

    switch( uiPrmPos )
    {
	case -1:	//load
	    reld_dt = 2;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	break;
	    w->setVisible( (bool)atoi(val.c_str()) && ((RunWdgView*)w)->permView() );
	    break;
	case 6:		//active
	    if( !qobject_cast<RunWdgView*>(w) ) break;
	    shD->active = (bool)atoi(val.c_str());
	    setFocus( w, shD->addrWdg, shD->active && ((RunWdgView*)w)->permCntr() );
//	    shD->addrWdg->setEnabled( shD->active && ((RunWdgView*)w)->permCntr() );
	    break;
	case 12:	//geomMargin
	    w->layout()->setMargin(atoi(val.c_str()));	break;
	case 20:	//backColor
	{
	    QPalette plt(shD->addrWdg->palette());
	    QBrush brsh = plt.brush(QPalette::Base);
	    brsh.setColor(QColor(val.c_str()));
	    if( !brsh.color().isValid() ) brsh.setColor(QPalette().brush(QPalette::Base).color());
	    brsh.setStyle( brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern );
	    plt.setBrush(QPalette::Base,brsh);
	    shD->addrWdg->setPalette(plt);
	    break;
	}
	case 21:	//backImg
	{
	    QImage img;
	    QPalette plt(shD->addrWdg->palette());
	    QBrush brsh = plt.brush(QPalette::Base);
	    string backimg = w->resGet(val);
	    if( !backimg.empty() ) img.loadFromData((const uchar*)backimg.c_str(),backimg.size());
	    brsh.setTextureImage(img);
	    brsh.setStyle( !brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern );
	    plt.setBrush(QPalette::Base,brsh);
	    shD->addrWdg->setPalette(plt);
	    break;
	}
	case 22:	//font
	    shD->addrWdg->setFont(getFont(val,vmin(w->xScale(true),w->yScale(true)))); break;
	case 23:	//headVis
	    shD->addrWdg->horizontalHeader()->setVisible(atoi(val.c_str()));
	    shD->addrWdg->verticalHeader()->setVisible(atoi(val.c_str()));
	    break;
	case 24:	//time
	{
	    unsigned int tm = strtoul(val.c_str(),0,10);
	    //if( shD->time == tm ) break;
	    shD->timeCurent = false;
	    if( tm == 0 )
	    {
		shD->time = (unsigned int)time(NULL);
		shD->timeCurent = true;
	    }else shD->time = tm;
	    reld_dt = 1;
	    break;
	}
	case 25:	//tSize
	    if( shD->tSize == strtoul(val.c_str(),0,10) ) break;
	    shD->tSize = (unsigned int)strtoul(val.c_str(),0,10);
	    reld_dt = 1;
	    break;
	case 26:	//trcPer
	{
	    unsigned int trcPer = strtoul(val.c_str(),0,10);
	    if( shD->trcPer == trcPer ) break;
	    shD->trcPer = trcPer;
 	    if( trcPer )	shD->trcTimer->start(trcPer*1000);
	    else		shD->trcTimer->stop();
	    break;
	}
	case 27:	//arch
	    if( shD->arch == val ) break;
	    shD->arch = val; reld_dt = 2; break;
	case 28:	//tmpl
	    if( shD->tmpl == val ) break;
	    shD->tmpl = val; reld_dt = 2; break;
	case 29:	//lev
	    if( shD->lev == atoi(val.c_str()) ) break;
	    shD->lev = atoi(val.c_str()); reld_dt = 2; break;
	case 30:	//viewOrd
	    if( shD->viewOrd == atoi(val.c_str()) )	break;
	    shD->viewOrd = atoi(val.c_str()); reld_dt = 1; break;
	case 31:	//col
	    if( shD->col == val ) break;
	    shD->col =  val; reld_dt = 1; break;
	case 32:	//itProp
	{
	    int itNum = atoi(val.c_str());
	    if( shD->itProps.size() == itNum ) break;
	    while( shD->itProps.size() < itNum )	shD->itProps.push_back( ShpDt::ItProp() );
	    while( shD->itProps.size() > itNum )	shD->itProps.pop_back( );
	    reld_dt = 1; break;
	}
	default:
	    //> Item's properties configuration
	    if( uiPrmPos >= 40 )
	    {
		int itNum = (uiPrmPos-40)/5;
		if( itNum >= shD->itProps.size() ) break;
		reld_dt = 1;
		switch( uiPrmPos%5 )
		{
		    case 0: shD->itProps[itNum].lev = atoi(val.c_str());	break;	//lev
		    case 1: shD->itProps[itNum].tmpl = val;			break;	//tmpl
		    case 2: shD->itProps[itNum].font = getFont(val,vmin(w->xScale(true),w->yScale(true)));	break;	//fnt
		    case 3: shD->itProps[itNum].clr = QColor(val.c_str());	break;	//color
		}
	    }
    }

    if( reld_dt && !w->allAttrLoad( ) )
	loadData(w,reld_dt==2);

    return true;
}

void ShapeProtocol::loadData( WdgView *w, bool full )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    unsigned int tTime		= shD->time,
		tTimeGrnd	= tTime - shD->tSize,
		arhBeg		= shD->arhBeg,
		arhEnd		= shD->arhEnd;
    bool	toUp = false,
		isDtChang = false,
		newFill = false;

    //> Check table structure
    //> Clear collumns
    for( int ncl = 0; ncl < shD->addrWdg->columnCount(); ncl++ )
	if( shD->col.find(shD->addrWdg->horizontalHeaderItem(ncl)->data(Qt::UserRole).toString().toAscii().data()) == string::npos )
	{
	    shD->addrWdg->removeColumn(ncl);
	    ncl--;
	    newFill = true;
	}

    //> Get collumns indexes
    int c_tm = -1, c_tmu = -1, c_lev = -1, c_cat = -1, c_mess = -1;
    shD->addrWdg->verticalHeader()->setVisible(false);
    string clm;
    for( int c_off = 0; (clm=TSYS::strSepParse(shD->col,0,';',&c_off)).size(); )
    {
	string clmNm;
	int ncl;
	for( ncl = 0; ncl < shD->addrWdg->columnCount(); ncl++ )
	    if( shD->addrWdg->horizontalHeaderItem(ncl)->data(Qt::UserRole).toString() == clm.c_str() )
		break;
	if( clm == "pos" )	shD->addrWdg->verticalHeader()->setVisible(true);
	else if( clm == "tm" )	{ clmNm = _("Time"); c_tm = ncl; }
	else if( clm == "utm" )	{ clmNm = _("mcsec"); c_tmu = ncl; }
	else if( clm == "lev" )	{ clmNm = _("Level"); c_lev = ncl; }
	else if( clm == "cat" )	{ clmNm = _("Category"); c_cat = ncl; }
	else if( clm == "mess" ){ clmNm = _("Message"); c_mess = ncl; }
	if( clmNm.empty() || ncl < shD->addrWdg->columnCount() )	continue;

	ncl = shD->addrWdg->columnCount();
	shD->addrWdg->setColumnCount(ncl+1);
	shD->addrWdg->setHorizontalHeaderItem(ncl,new QTableWidgetItem());
	shD->addrWdg->horizontalHeaderItem(ncl)->setText(clmNm.c_str());
	shD->addrWdg->horizontalHeaderItem(ncl)->setData(Qt::UserRole,clm.c_str());
	newFill = true;
    }

    newFill = newFill || !shD->messList.size();

    if( shD->lev < 0 ) shD->messList.clear();
    else
    {
	//> Clear loaded data
	if( full )
	{
	    shD->messList.clear();
	    arhBeg = arhEnd = 0;
	}

	//> Get archive parameters
	if( !arhBeg || !arhEnd || tTime > arhEnd )
	{
	    XMLNode req("info");
	    req.setAttr("arch",shD->arch)->setAttr("path","/Archive/%2fserv%2fmess");
	    if( w->cntrIfCmd(req,true) )	arhBeg = arhEnd = 0;
	    else
	    {
		arhBeg = strtoul(req.attr("beg").c_str(),0,10);
		arhEnd = strtoul(req.attr("end").c_str(),0,10);
	    }
	}
	if( !arhBeg || !arhEnd )	return;

	//> Correct request to archive border
	tTime     = vmin(tTime,arhEnd);
	tTimeGrnd = vmax(tTimeGrnd,arhBeg);
    }

    //> Clear data at time error
    unsigned int valEnd = 0, valBeg = 0;

    while( shD->messList.size() && (valEnd=shD->messList[0].time) > tTime ) { shD->messList.pop_front(); isDtChang = true; }
    while( shD->messList.size() && (valBeg=shD->messList[shD->messList.size()-1].time) < tTimeGrnd )	{ shD->messList.pop_back(); isDtChang = true; }

    if( tTime < tTimeGrnd || (tTime < valEnd && tTimeGrnd > valBeg) )
    {
	shD->messList.clear();
	shD->addrWdg->setRowCount(0);
	valEnd = valBeg = 0;
	return;
    }

    //> Correct request to present data
    if( valEnd && tTime >= valEnd )	{ tTimeGrnd = valEnd; toUp = true; }
    else if( valBeg && tTimeGrnd < valBeg )	tTime = valBeg-1;

    //> Get values data
    XMLNode req("get");
    req.clear()->
	setAttr("arch",shD->arch)->
	setAttr("path","/Archive/%2fserv%2fmess")->
	setAttr("tm",TSYS::ll2str(tTime))->
	setAttr("tm_grnd",TSYS::ll2str(tTimeGrnd))->
	setAttr("cat",shD->tmpl)->
	setAttr("lev",TSYS::int2str(shD->lev));
    if( w->cntrIfCmd(req,true) )	return;


    if( toUp )
	for( int i_req = 0; i_req < req.childSize(); i_req++ )
	{
	    XMLNode *rcd = req.childGet(i_req);
	    TMess::SRec mess(strtoul(rcd->attr("time").c_str(),0,10),atoi(rcd->attr("utime").c_str()),rcd->attr("cat"),(TMess::Type)atoi(rcd->attr("lev").c_str()),rcd->text());

	    //>> Check for dublicates
	    int i_p;
	    for( i_p = 0; i_p < shD->messList.size(); i_p++ )
	    {
		if( mess.time > shD->messList[0].time && i_p )	continue;
		if( shD->messList[i_p].utime == mess.utime && shD->messList[i_p].level == mess.level &&
			shD->messList[i_p].categ == mess.categ && shD->messList[i_p].mess == mess.mess )
		    break;
	    }
	    if( i_p < shD->messList.size() ) continue;

	    //>> Insert new row
	    shD->messList.push_front(mess);
	    isDtChang = true;
	}
    else
	for( int i_req = req.childSize()-1; i_req >= 0; i_req-- )
	{
	    XMLNode *rcd = req.childGet(i_req);
	    TMess::SRec mess(strtoul(rcd->attr("time").c_str(),0,10),atoi(rcd->attr("utime").c_str()),rcd->attr("cat"),(TMess::Type)atoi(rcd->attr("lev").c_str()),rcd->text());

	    //>> Check for dublicates
	    int i_p;
	    for( i_p = shD->messList.size()-1; i_p >= 0; i_p-- )
	    {
		if( mess.time < shD->messList[shD->messList.size()-1].time && i_p < (shD->messList.size()-1) ) continue;
		if( shD->messList[i_p].utime == mess.utime && shD->messList[i_p].level == mess.level &&
			shD->messList[i_p].categ == mess.categ && shD->messList[i_p].mess == mess.mess )
		    break;
	    }
	    if( i_p >= 0 ) continue;

	    //>> Insert new row
	    shD->messList.push_back(mess);
	    isDtChang = true;
	}

    if( shD->addrWdg->rowCount() == shD->messList.size() && !isDtChang ) return;

    //> Sort data
    vector< pair<string,int> > sortIts;
    switch( shD->viewOrd&0x3 )
    {
	case 0:
	    for( int i_m = 0; i_m < shD->messList.size(); i_m++ )
		sortIts.push_back( pair<string,int>(TSYS::uint2str(shD->messList[i_m].time)+" "+TSYS::uint2str(shD->messList[i_m].utime),i_m) );
	    break;
	case 1:
	    for( int i_m = 0; i_m < shD->messList.size(); i_m++ )
		sortIts.push_back( pair<string,int>(TSYS::int2str(shD->messList[i_m].level),i_m) );
	    break;
	case 2:
	    for( int i_m = 0; i_m < shD->messList.size(); i_m++ )
		sortIts.push_back( pair<string,int>(shD->messList[i_m].categ,i_m) );
	    break;
	case 3:
	    for( int i_m = 0; i_m < shD->messList.size(); i_m++ )
		sortIts.push_back( pair<string,int>(shD->messList[i_m].mess,i_m) );
	    break;
    }
    sort(sortIts.begin(),sortIts.end());
    if( shD->viewOrd&0x4 ) reverse(sortIts.begin(),sortIts.end());

    //> Write to table
    shD->addrWdg->setRowCount(sortIts.size());
    QTableWidgetItem *tit;
    for( int i_m = 0; i_m < sortIts.size(); i_m++ )
    {
	QFont fnt;
	QColor clr, fclr;
	//>> Check properties
	for( int i_it = 0, lst_lev = -1; i_it < shD->itProps.size(); i_it++ )
	    if( shD->messList[sortIts[i_m].second].level >= shD->itProps[i_it].lev && shD->itProps[i_it].lev > lst_lev &&
		TMess::chkPattern(shD->messList[sortIts[i_m].second].categ,shD->itProps[i_it].tmpl) )
	    {
		fnt = shD->itProps[i_it].font;
		clr = shD->itProps[i_it].clr;
		if( shD->messList[sortIts[i_m].second].level == shD->itProps[i_it].lev ) break;
		lst_lev = shD->itProps[i_it].lev;
	    }
	if( clr.isValid() )
	    fclr = ((0.3*clr.red()+0.59*clr.green()+0.11*clr.blue()) > 128) ? Qt::black : Qt::white;

	for( int i_cl = 0; i_cl < shD->addrWdg->columnCount(); i_cl++ )
	{
	    if( i_cl == c_tm )
	    {
		QDateTime	dtm;
		dtm.setTime_t(shD->messList[sortIts[i_m].second].time);
		shD->addrWdg->setItem( i_m, c_tm, tit=new QTableWidgetItem(dtm.toString("dd.MM.yyyy hh:mm:ss")) );
	    }
	    else if( i_cl == c_tmu )
		shD->addrWdg->setItem( i_m, c_tmu, tit=new QTableWidgetItem(QString::number(shD->messList[sortIts[i_m].second].utime)) );
	    else if( i_cl == c_lev )
		shD->addrWdg->setItem( i_m, c_lev, tit=new QTableWidgetItem(QString::number(shD->messList[sortIts[i_m].second].level)) );
	    else if( i_cl == c_cat )
		shD->addrWdg->setItem( i_m, c_cat, tit=new QTableWidgetItem(shD->messList[sortIts[i_m].second].categ.c_str()) );
	    else if( i_cl == c_mess )
		shD->addrWdg->setItem( i_m, c_mess, tit=new QTableWidgetItem(shD->messList[sortIts[i_m].second].mess.c_str()) );
	    else continue;
	    tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	    tit->setData(Qt::FontRole,fnt);
	    tit->setData(Qt::BackgroundRole,clr.isValid() ? clr : QVariant());
	    tit->setData(Qt::ForegroundRole,fclr.isValid() ? fclr : QVariant());
	}
    }

    if( newFill )
    {
	shD->addrWdg->resizeColumnsToContents();
	//Resize too long columns
	int max_col_sz = vmax(w->size().width()/2,40);
	for( int i_c = 0; i_c < shD->addrWdg->columnCount(); i_c++ )
	    shD->addrWdg->setColumnWidth(i_c,vmin(max_col_sz,shD->addrWdg->columnWidth(i_c)));
	shD->addrWdg->horizontalHeader()->setStretchLastSection(true);
    }
    shD->addrWdg->resizeRowsToContents();
}

void ShapeProtocol::tracing( )
{
    WdgView *w = (WdgView *)((QTimer*)sender())->parent();
    ShpDt *shD = (ShpDt*)w->shpData;
    if( !w->isEnabled() ) return;

    if( shD->timeCurent )	shD->time = (unsigned int)time(NULL);
    else if( shD->time )	shD->time += shD->trcPer;
    loadData(w);
}

bool ShapeProtocol::event( WdgView *w, QEvent *event )
{
    return false;
}

bool ShapeProtocol::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
    if( qobject_cast<DevelWdgView*>(w) )
	switch(event->type())
	{
	    case QEvent::Enter:
	    case QEvent::Leave:
		return true;
	    case QEvent::MouseMove:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
		QApplication::sendEvent(w,event);
	    return true;
	}
    else
	switch( event->type() )
	{
	    case QEvent::FocusIn:
		w->attrSet("focus","1");
		w->attrSet("event","ws_FocusIn");
		break;
	    case QEvent::FocusOut:
		w->attrSet("focus","0");
		w->attrSet("event","ws_FocusOut");
		break;
	}

    return false;
}

void ShapeProtocol::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if( en )	wdg->installEventFilter(view);
    else	wdg->removeEventFilter(view);
    //- Process childs -
    for( int i_c = 0; i_c < wdg->children().size(); i_c++ )
	if( qobject_cast<QWidget*>(wdg->children().at(i_c)) )
	{
	    eventFilterSet(view,(QWidget*)wdg->children().at(i_c),en);
	    if( en ) ((QWidget*)wdg->children().at(i_c))->setMouseTracking(true);
	}
}

void ShapeProtocol::setFocus(WdgView *view, QWidget *wdg, bool en, bool devel )
{
    int isFocus = wdg->windowIconText().toInt();
    //- Set up current widget -
    if( en )
    {
	if( isFocus )	wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
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
//* Document view shape widget                   *
//************************************************
const char *ShapeDocument::XHTML_entity =
    "<!DOCTYPE xhtml [\n"
    "  <!ENTITY nbsp \"&#160;\" >\n"
    "]>\n";

ShapeDocument::ShapeDocument( ) : WdgShape("Document")
{

}

void ShapeDocument::init( WdgView *w )
{
    w->shpData = new ShpDt();
    ShpDt *shD = (ShpDt*)w->shpData;

    QVBoxLayout *lay = new QVBoxLayout(w);
    shD->web = new QTextBrowser(w);

    eventFilterSet( w, shD->web, true );
    w->setFocusProxy( shD->web );
    if( qobject_cast<DevelWdgView*>(w) )
	setFocus( w, shD->web, false, true );

    lay->addWidget(shD->web);
}

void ShapeDocument::destroy( WdgView *w )
{
    delete (ShpDt*)w->shpData;
}

bool ShapeDocument::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);

    bool relDoc = false;	//Reload configuration

    switch( uiPrmPos )
    {
	case -1:	//load
	    relDoc = true;
	    break;
	case 5:		//en
	    if(!runW)	break;
	    shD->en = (bool)atoi(val.c_str());
	    shD->web->setVisible( shD->en && runW->permView() );
	    break;
	case 6:		//active
	    if(!runW)	break;
	    shD->active = (bool)atoi(val.c_str());
	    setFocus( w, shD->web, shD->active && runW->permCntr() );
	    shD->web->setEnabled( shD->active && runW->permCntr() );
	    break;
	case 12:	//geomMargin
	    w->layout()->setMargin(atoi(val.c_str()));	break;
	case 20:	//style
	    shD->style = val;
	    relDoc = true;
	    break;
	case 21:	//tmpl
	    if( !shD->doc.empty() && !shD->tmpl )	break;
	    shD->doc = val;
	    relDoc = true;
	    shD->tmpl = true;
	    break;
	case 22:	//doc
	    if( TSYS::strNoSpace(val).empty() )		break;
	    shD->doc = val;
	    relDoc = true;
	    shD->tmpl = false;
	    break;
	case 26:	//font
	    shD->web->setFont(getFont(val,vmin(w->xScale(true),w->yScale(true)),false));	break;
    }
    if( relDoc && !w->allAttrLoad() )
    {
	//> Process source document
	//>> Parse document
	XMLNode xproc;
	try{ xproc.load(string(XHTML_entity)+shD->doc); }
	catch( TError err )
	{ mess_err(mod->nodePath().c_str(),"Document '%s' parsing is error: %s",w->id().c_str(),err.mess.c_str()); }

	nodeProcess( &xproc, shD );

	int scrollPos = shD->web->verticalScrollBar()->value();

	shD->web->setHtml(
	    ("<?xml version='1.0' ?>\n"
	    "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	    "'DTD/xhtml1-transitional.dtd'>\n"
	    "<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	    "<head>\n"
	    "  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	    "  <style type='text/css'>\n"+shD->style+"</style>\n"
	    "</head>\n"+
	    xproc.save()+
	    "</html>").c_str());

	shD->web->verticalScrollBar()->setValue(scrollPos);
    }

    return true;
}

void ShapeDocument::nodeProcess( XMLNode *xcur, ShapeDocument::ShpDt *shD )
{
    //> Delete process instructions
    xcur->prcInstrClear();

    //> Go to include nodes
    for( int i_c = 0; i_c < xcur->childSize(); i_c++ )
    {
	//>> Check for special tags
	if( xcur->childGet(i_c)->name().substr(0,3) == "doc" )
	{ xcur->childDel(i_c); i_c--; continue; }

	nodeProcess(xcur->childGet(i_c),shD);
    }
}

bool ShapeDocument::event( WdgView *w, QEvent *event )
{
    return false;
}

bool ShapeDocument::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
    if( qobject_cast<DevelWdgView*>(w) )
	switch( event->type() )
	{
	    case QEvent::Enter:
	    case QEvent::Leave:		return true;
	    case QEvent::MouseMove:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::ContextMenu:
	    case QEvent::Wheel:
		QApplication::sendEvent(w,event);
		return true;
	}
    else
	switch( event->type() )
	{
	    case QEvent::FocusIn:
		w->attrSet("focus","1");
		w->attrSet("event","ws_FocusIn");
		break;
	    case QEvent::FocusOut:
		w->attrSet("focus","0");
		w->attrSet("event","ws_FocusOut");
		break;
	}

    return false;
}

void ShapeDocument::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if( en )	wdg->installEventFilter(view);
    else	wdg->removeEventFilter(view);
    //> Process childs
    for( int i_c = 0; i_c < wdg->children().size(); i_c++ )
	if( qobject_cast<QWidget*>(wdg->children().at(i_c)) )
	    eventFilterSet(view,(QWidget*)wdg->children().at(i_c),en);
}

void ShapeDocument::setFocus( WdgView *view, QWidget *wdg, bool en, bool devel )
{
    int isFocus = wdg->windowIconText().toInt();
    //> Set up current widget
    if( en )
    {
	if( isFocus && !devel )	wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
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
    w->shpData = new ShpDt();
}

void ShapeBox::destroy( WdgView *w )
{
    delete (ShpDt*)w->shpData;
}

bool ShapeBox::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    bool up = true;
    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos)
    {
	case -1:	//load
	    up = true;
	    if( qobject_cast<RunWdgView*>(w) && shD->inclWidget )
		shD->inclWidget->setMinimumSize(w->size());
	    break;
	case -2:	//focus
	    //if( (bool)atoi(val.c_str()) == w->hasFocus() )	up = false;
	    break;
        case 5:         //en
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    shD->en = (bool)atoi(val.c_str());
	    w->setVisible( shD->en && (((RunWdgView*)w)->permView() || dynamic_cast<RunPageView*>(w)) );
	    break;
	case 6:		//active
	    if( !qobject_cast<RunWdgView*>(w) ) { up = false; break; }
	    if( atoi(val.c_str()) && ((RunWdgView*)w)->permCntr() ) w->setFocusPolicy( Qt::StrongFocus );
	    else w->setFocusPolicy( Qt::NoFocus );
	    break;
	case 12:	//geomMargin
	    shD->geomMargin = atoi(val.c_str());
	    if( w->layout() ) w->layout()->setMargin( shD->geomMargin );
	    break;
	case 20: 	//backColor
	{
	    shD->backGrnd.setColor(QColor(val.c_str()));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if( !brsh.color().isValid() ) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle( brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    break;
	}
	case 21: 	//backImg
	{
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage(QImage());
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		shD->backGrnd.setTextureImage(img);

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle( !brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    break;
	}
	case 22:	//bordWidth
	    shD->border.setWidth(atoi(val.c_str()));	break;
	case 23:	//bordColor
	    shD->border.setColor(QColor(val.c_str()));	break;
	case 24:	//bordStyle
	    shD->bordStyle = atoi(val.c_str());	break;
	case 3:		//pgOpenSrc
	{
	    if( !qobject_cast<RunWdgView*>(w) || qobject_cast<RunPageView*>(w) )	{ up = false; break; }

	    //>> Put previous include widget to page cache
	    if( (shD->inclWidget && val != shD->inclWidget->id()) || (!shD->inclWidget && !val.empty()) )
	    {
		if( shD->inclWidget )
		{
		    shD->inclWidget->setReqTm(shD->inclWidget->mainWin()->reqTm());
		    ((RunPageView*)w)->mainWin()->pgCacheAdd(shD->inclWidget);
		    shD->inclWidget->setEnabled(false);
		    shD->inclWidget->setVisible(false);
		    shD->inclScrl->takeWidget();
		    shD->inclWidget->setParent(NULL);
		    shD->inclWidget->wx_scale = shD->inclWidget->mainWin()->xScale( );
		    shD->inclWidget->wy_scale = shD->inclWidget->mainWin()->yScale( );
		    shD->inclWidget = NULL;
		    w->setProperty("inclPg","");
		}
		//>> Create new include widget
		if( val.size() )
		{
		    if( !shD->inclScrl )
		    {
			QGridLayout *wlay = (QGridLayout*)w->layout();
			if( !wlay ) wlay = new QGridLayout(w);
			wlay->setMargin(0/*shD->geomMargin*/);
			shD->inclScrl = new QScrollArea(w);
			shD->inclScrl->setFocusPolicy( Qt::NoFocus );
			shD->inclScrl->setFrameShape( QFrame::NoFrame );
			wlay->addWidget(shD->inclScrl);
		    }

		    QLabel *lab = new QLabel(QString("Load page: '%1'.").arg(val.c_str()),shD->inclWidget);
		    lab->setAlignment(Qt::AlignCenter);
		    lab->setWordWrap(true);
		    lab->resize(w->size());
		    QPalette plt = lab->palette();
		    plt.setColor(QPalette::WindowText,shD->border.color());
		    lab->setPalette(plt);
		    lab->setLineWidth(shD->border.width());
		    lab->setFrameShape(QFrame::Box);
		    shD->inclScrl->setWidget(lab);
		    qApp->processEvents();

		    shD->inclWidget = ((RunWdgView*)w)->mainWin()->pgCacheGet(val);
		    if( shD->inclWidget )
		    {
			shD->inclWidget->setProperty("cntPg",TSYS::addr2str(w).c_str());
			shD->inclScrl->setWidget(shD->inclWidget);
			shD->inclWidget->setEnabled(true);
			shD->inclWidget->setVisible(true);
			shD->inclWidget->setMinimumSize(w->size());
			if( shD->inclWidget->wx_scale != shD->inclWidget->mainWin()->xScale() ||
				shD->inclWidget->wy_scale != shD->inclWidget->mainWin()->yScale() )
			    shD->inclWidget->load("");
			else
			{
			    unsigned trt = shD->inclWidget->mainWin()->reqTm();
			    shD->inclWidget->mainWin()->setReqTm(shD->inclWidget->reqTm());
			    shD->inclWidget->update(false);
			    shD->inclWidget->mainWin()->setReqTm(trt);
			}
		    }
		    else
		    {
			shD->inclWidget = new RunPageView(val,(VisRun*)w->mainWin(),shD->inclScrl,Qt::SubWindow);
			shD->inclWidget->setProperty("cntPg",TSYS::addr2str(w).c_str());
			shD->inclScrl->setWidget(shD->inclWidget);
			shD->inclWidget->setMinimumSize(w->size());
			shD->inclWidget->load("");
		    }
		    w->setProperty("inclPg",TSYS::addr2str(shD->inclWidget).c_str());
		}
	    } else up = false;
	    break;
	}
	default: up = false;
    }

    if( up && !w->allAttrLoad( ) && uiPrmPos != -1 )	w->update();

    return up;
}

bool ShapeBox::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if( !shD->en ) return false;

    switch(event->type())
    {
	case QEvent::Paint:
	{
	    if( shD->inclWidget ) return false;
	    QPainter pnt( w );

	    //> Apply margin
	    QRect dA = w->rect().adjusted(0,0,-2*shD->geomMargin,-2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(shD->geomMargin,shD->geomMargin,-shD->geomMargin,-shD->geomMargin));

	    //> Draw background
	    if( shD->backGrnd.color().isValid() ) pnt.fillRect(dA,shD->backGrnd.color());
	    if( !shD->backGrnd.textureImage().isNull() ) pnt.fillRect(dA,shD->backGrnd.textureImage());

	    //> Draw border
	    borderDraw( pnt, dA, shD->border, shD->bordStyle );

	    //> Draw focused border
	    //if( w->hasFocus() )	qDrawShadeRect(&pnt,dA,w->palette(),false,1);

	    return true;
	}
	//case QEvent::User:
	//    if( shD->inclWidget ) shD->inclWidget->update();
    }
    return false;
}

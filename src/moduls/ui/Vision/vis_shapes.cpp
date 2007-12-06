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
#include <QTimer>
#include <QKeyEvent>
#include <QTableWidget>
#include <QDateTime>

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

void ShapeFormEl::init( WdgView *w )
{
    QVBoxLayout *lay = new QVBoxLayout(w);
    w->dc()["en"] = true;
    w->dc()["active"] = true;
    w->dc()["addrWdg"].setValue((void*)NULL);
    w->dc()["elType"] = -1;
}

bool ShapeFormEl::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);

    bool rel_cfg = false;	//Reload configuration

    w->dc()["evLock"] = true;
    int     el = w->dc()["elType"].toInt();
    QWidget *el_wdg = (QWidget *)w->dc()["addrWdg"].value<void*>();

    switch( uiPrmPos )
    {
	case -1:	//load
	    rel_cfg = true;
	    break;
	case 2:		//name
	    w->dc()["name"] = val.c_str();
	    if( el == 2) 	((QCheckBox*)el_wdg)->setText(val.c_str());
	    else if( el == 3 )	((QPushButton*)el_wdg)->setText(val.c_str());
	    break;
	case 5:		//en
	    if(!runW)	break;
	    w->dc()["en"] = (bool)atoi(val.c_str());
    	    if(el >= 0) el_wdg->setVisible(atoi(val.c_str()));
	    break;
	case 6:		//active
	    if(!runW)	break;
	    w->dc()["active"] = (bool)atoi(val.c_str());
    	    if( el >= 0 ) setFocus(w,el_wdg,atoi(val.c_str()));
	    break;
	case 12:	//geomMargin
	    w->layout()->setMargin(atoi(val.c_str()));	break;
	case 20:	//elType
	    if( el == atoi(val.c_str()) ) break;
	    w->dc()["elType"] = atoi(val.c_str());
	    rel_cfg = true;
	    break;
	case 21:	//value
	    w->dc()["value"] = val.c_str();
	    if( !el_wdg ) break;
	    switch(el)
	    {
		case 0: ((LineEdit*)el_wdg)->setValue(val.c_str());	break;
		case 1:	((TextEdit*)el_wdg)->setText(val.c_str());	break;
		case 2:	((QCheckBox*)el_wdg)->setChecked(atoi(val.c_str()));	break;
		case 3:	((QPushButton*)el_wdg)->setChecked(atoi(val.c_str()));	break;
		case 4:	
		    if( ((QComboBox*)el_wdg)->findText(val.c_str()) < 0 ) ((QComboBox*)el_wdg)->addItem(val.c_str());
			((QComboBox*)el_wdg)->setCurrentIndex(((QComboBox*)el_wdg)->findText(val.c_str()));
		    break;
		case 5:
		{
		    QList<QListWidgetItem *> its = ((QListWidget*)el_wdg)->findItems(val.c_str(),Qt::MatchExactly);
		    if( its.size() ) ((QListWidget*)el_wdg)->setCurrentItem(its[0]);
		    break;
		}
	    }
	    break;
	case 22:	//view, wordWrap, img, items
	    rel_cfg = true;
	    switch(el)
	    {
		case 0:	//view
		    w->dc()["view"] = atoi(val.c_str());	break;
		case 1:	//wordWrap
		    w->dc()["wordWrap"] = atoi(val.c_str());	break;
		case 3:	//img
		    w->dc()["img"] = val.c_str(); 		break;
		case 4: case 5:	//items
		    w->dc()["items"] = val.c_str(); 		break;
		default: rel_cfg = false;
	    }
	    break;
	case 23:	//cfg, color
	    rel_cfg = true;
	    switch(el)
	    {	
		case 0:	//cfg    
		    w->dc()["cfg"] = val.c_str();
		    //((LineEdit*)el_wdg)->setCfg(val.c_str());
		    break;
		case 3:	//color
		    w->dc()["color"] = val.c_str();
		    //((QPushButton*)el_wdg)->setPalette( QColor(val.c_str()).isValid() ? QPalette(QColor(val.c_str())) : QPalette() );
		    break;
		default: rel_cfg = false;
	    }
	    break;
	case 24:	//checkable
	    w->dc()["checkable"] = atoi(val.c_str());	
	    rel_cfg = true; 
	    break;
    }
    if( rel_cfg && !w->allAttrLoad() )
    {
	bool mk_new = false;
	switch(w->dc()["elType"].toInt())
	{
	    case 0:	//Line edit
	    {
		if( !el_wdg || !qobject_cast<LineEdit*>(el_wdg) )
		{
		    if( el_wdg ) delete el_wdg;
		    el_wdg = new LineEdit(w);
		    if( runW ) connect( el_wdg, SIGNAL(apply()), this, SLOT(lineAccept()) );
		    mk_new = true;
		}
		//- View -
		LineEdit::LType tp = LineEdit::Text;
		switch(w->dc()["view"].toInt())
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
		//- Cfg -
		((LineEdit*)el_wdg)->setCfg(w->dc()["cfg"].toString());
		//- Value -
		((LineEdit*)el_wdg)->setValue(w->dc()["value"].toString());		
		break;
	    }
	    case 1:	//Text edit
		if( !el_wdg || !qobject_cast<TextEdit*>(el_wdg) )
		{
		    if( el_wdg ) delete el_wdg;		
		    el_wdg = new TextEdit(w);
		    if( runW ) connect( el_wdg, SIGNAL(apply()), this, SLOT(textAccept()) );
		    mk_new = true;
		}
		//- Value -
		((TextEdit*)el_wdg)->setText(w->dc()["value"].toString());
		//- WordWrap -
		((TextEdit*)el_wdg)->workWdg()->setLineWrapMode( 
			w->dc().value("wordWrap",1).toInt() ? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
		break;
	    case 2:	//Chek box
		if( !el_wdg || !qobject_cast<QCheckBox*>(el_wdg) )
                {
            	    if( el_wdg ) delete el_wdg;
		    el_wdg = new QCheckBox("test",w);
		    if( runW ) connect( el_wdg, SIGNAL(stateChanged(int)), this, SLOT(checkChange(int)) );
		    mk_new = true;
		}
		//- Name -
		((QCheckBox*)el_wdg)->setText(w->dc()["name"].toString());
		//- Value -
		((QCheckBox*)el_wdg)->setChecked(w->dc()["value"].toInt());
		break;
	    case 3:	//Button
	    {
		if( !el_wdg || !qobject_cast<QPushButton*>(el_wdg) )
		{
		    if( el_wdg ) delete el_wdg;
	    	    el_wdg = new QPushButton("test",w);
		    el_wdg->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
		    if( runW ) 
		    {
			connect( el_wdg, SIGNAL(pressed()), this, SLOT(buttonPressed()) );
			connect( el_wdg, SIGNAL(released()), this, SLOT(buttonReleased()) );
			connect( el_wdg, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)) );
		    }
		    mk_new = true;
		}
		//- Name -
		((QPushButton*)el_wdg)->setText(w->dc()["name"].toString());
		//- Img -
		XMLNode req("get");
    		req.setAttr("path",w->id()+"/%2fwdg%2fres")->
		    setAttr("id",w->dc()["img"].toString().toAscii().data());
    		if( !w->cntrIfCmd(req) )
    		{
		    string backimg = TSYS::strDecode(req.text(),TSYS::base64);
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
		//- Color -
		QColor clr(w->dc()["color"].toString());
		((QPushButton*)el_wdg)->setPalette( clr.isValid() ? QPalette(clr) : QPalette() );
		//- Checkable -
		((QPushButton*)el_wdg)->setCheckable(w->dc()["checkable"].toInt());
		//- Value -
		((QPushButton*)el_wdg)->setChecked(w->dc()["value"].toInt());		
		break;
	    }
	    case 4:	//Combo box
	    {
		if( !el_wdg || !qobject_cast<QPushButton*>(el_wdg) )
		{
		    if( el_wdg ) delete el_wdg;
		    el_wdg = new QComboBox(w);
		    if( runW ) connect( el_wdg, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(comboChange(const QString&)) );
		    mk_new = true;
		}
		//- Items -
		((QComboBox*)el_wdg)->clear();
		((QComboBox*)el_wdg)->addItems(w->dc()["items"].toString().split("\n"));
		//- Value -
		QString vl = w->dc()["value"].toString();
		if( ((QComboBox*)el_wdg)->findText(vl) < 0 ) ((QComboBox*)el_wdg)->addItem(vl);
		((QComboBox*)el_wdg)->setCurrentIndex(((QComboBox*)el_wdg)->findText(vl));
		break;
	    }
	    case 5: 	//List
	    {
		if( !el_wdg || !qobject_cast<QPushButton*>(el_wdg) )
		{
		    if( el_wdg ) delete el_wdg;
		    el_wdg = new QListWidget(w);
		    if( runW ) connect( el_wdg, SIGNAL(currentRowChanged(int)), this, SLOT(listChange(int)) );
		    mk_new = true;
		}
		//- Items -
		((QListWidget*)el_wdg)->clear();
                ((QListWidget*)el_wdg)->addItems(w->dc()["items"].toString().split("\n"));
		//- Value -
		QList<QListWidgetItem *> its = ((QListWidget*)el_wdg)->findItems(w->dc()["value"].toString(),Qt::MatchExactly);
		if( its.size() ) ((QListWidget*)el_wdg)->setCurrentItem(its[0]);		
		break;
	    }
	}
	if( mk_new )
	{
	    //-- Install event's filter and disable focus --	
	    if( devW ) eventFilterSet(w,el_wdg,true);
	    setFocus(w,el_wdg,w->dc()["active"].toInt(),devW);
	    el_wdg->setVisible(w->dc()["en"].toInt());
	    //-- Fix widget --
	    ((QVBoxLayout*)w->layout())->addWidget(el_wdg);
	    w->dc()["addrWdg"].setValue((void*)el_wdg);
	}
    }

    w->dc()["evLock"] = false;
    
    return true;
}

bool ShapeFormEl::event( WdgView *view, QEvent *event )
{
    return false;
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
    if( view->dc()["evLock"].toBool() )	return;
    
    view->attrSet("event",string("ws_BtToggle")+(val?"On":"Off"));
    view->attrSet("value",TSYS::int2str(val));
}

void ShapeFormEl::comboChange(const QString &val)
{
    WdgView *view = (WdgView *)((QWidget*)sender())->parentWidget();
    if( view->dc()["evLock"].toBool() )	return;

    view->attrSet("value",val.toAscii().data());
    view->attrSet("event","ws_CombChange");
}

void ShapeFormEl::listChange( int row )
{
    QListWidget *el   = (QListWidget*)sender();
    WdgView     *view = (WdgView *)el->parentWidget();
    
    if( row < 0 || view->dc()["evLock"].toBool() ) return;
    
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
//* Text element shape widget                    *
//************************************************
ShapeText::ShapeText( ) : WdgShape("Text")
{

}

void ShapeText::init( WdgView *w )
{
    w->dc()["numbArg"] = 0;
    w->dc()["QFont"].setValue( (void*)new QFont() );
    w->dc()["border"].setValue( (void*)new QPen() );
}

void ShapeText::destroy( WdgView *w )
{
    delete (QFont*)w->dc()["QFont"].value<void*>();
    delete (QPen*)w->dc()["border"].value<void*>();
    //- Clear argument's data objects -
    int numbArg = w->dc()["numbArg"].toInt();
    for( int i_a = 0; i_a < numbArg; i_a++ )
        delete (ArgObj*)w->dc().value(QString("arg_%1").arg(i_a)).value<void*>();
}

bool ShapeText::attrSet( WdgView *w, int uiPrmPos, const string &val)
{    
    bool up = true, 		//Update view checking
	 rel_fnt = false,	//Reload font checking
	 reform = false;	//Text reformation

    switch(uiPrmPos)
    {
	case -1:	//load
	    rel_fnt = reform = true;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    w->dc()["en"] = (bool)atoi(val.c_str());
	    break;
	case 12: 	//geomMargin
	    w->dc()["geomMargin"] = atoi(val.c_str());	break;
	case 20: 	//backColor
	    w->dc()["backColor"] = QColor(val.c_str());	break;
	case 21:	//backImg
	{
	    XMLNode req("get");
    	    req.setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",val);
    	    if( !w->cntrIfCmd(req) )
    	    {
		QBrush brsh;
		string backimg = TSYS::strDecode(req.text(),TSYS::base64);
		if( !backimg.empty() )
		{
		    QImage img;
		    if(img.loadFromData((const uchar*)backimg.c_str(),backimg.size()))	
			brsh.setTextureImage(img);
		}
		w->dc()["backImg"] = brsh;
    	    } else up = false;
	    break;
	}
	case 22:	//bordWidth
	    ((QPen*)w->dc()["border"].value<void*>())->setWidth(atoi(val.c_str()));	break;
	case 23:	//bordColor
	    ((QPen*)w->dc()["border"].value<void*>())->setColor(QColor(val.c_str()));	break;
	case 24:	//font
	    w->dc()["font"] = val.c_str(); rel_fnt = true; break;
	case 25:	//fontFamily
	    ((QFont*)w->dc()["QFont"].value<void*>())->setFamily(val.c_str()); 
	    rel_fnt = true;	
	    break;
	case 26:	//fontSize
	    ((QFont*)w->dc()["QFont"].value<void*>())->setPointSize(atoi(val.c_str())); 
	    rel_fnt = true;	
	    break;
	case 27: 	//fontBold
	    ((QFont*)w->dc()["QFont"].value<void*>())->setBold(atoi(val.c_str()));
	    rel_fnt = true;
	    break;
	case 28:	//fontItalic
	    ((QFont*)w->dc()["QFont"].value<void*>())->setItalic(atoi(val.c_str()));
	    rel_fnt = true;
	    break;
	case 29: 	//fontUnderline
	    ((QFont*)w->dc()["QFont"].value<void*>())->setUnderline(atoi(val.c_str()));
	    rel_fnt = true;	    
	    break;
	case 30:	//fontStrikeout	    
	    ((QFont*)w->dc()["QFont"].value<void*>())->setStrikeOut(atoi(val.c_str()));
	    rel_fnt = true;
	    break;
	case 31:	//color
	    w->dc()["color"] = QColor(val.c_str()); break;
	case 32:	//orient
	    w->dc()["orient"] = atoi(val.c_str()); break;
	case 33:	//wordWrap
	{    
	    int txtflg = w->dc().value("text_flg",0).toInt();    
	    w->dc()["text_flg"] = atoi(val.c_str()) ? (txtflg|Qt::TextWordWrap) : (txtflg&(~Qt::TextWordWrap)); 
	    break;
	}
	case 34:	//alignment
	{   
	    int txtflg = w->dc().value("text_flg",0).toInt();
	    txtflg &= ~(Qt::AlignLeft|Qt::AlignRight|Qt::AlignHCenter|Qt::AlignTop|Qt::AlignBottom|Qt::AlignVCenter);
	    switch(atoi(val.c_str())&0x3)
	    {
		case 0: txtflg |= Qt::AlignLeft; 	break;
		case 1: txtflg |= Qt::AlignRight;	break;
		case 2: txtflg |= Qt::AlignHCenter;	break;
	    }
	    switch(atoi(val.c_str())>>2)
	    {
		case 0: txtflg |= Qt::AlignTop; 	break;
		case 1: txtflg |= Qt::AlignBottom;	break;
		case 2: txtflg |= Qt::AlignVCenter;	break;		
	    }
	    w->dc()["text_flg"] = txtflg;
	    break;
	}
	case 35:	//text
	{ 
	    if( w->dc()["text_tmpl"] == val.c_str() )	break;
	    w->dc()["text_tmpl"] = val.c_str();
	    reform = true;
	    break; 
	}
	case 36:	//numbArg
	{
	    int numbArgPrev = w->dc()["numbArg"].toInt();
	    int numbArg = atoi(val.c_str());
	    if( numbArgPrev == numbArg ) break;
	    for( int i_a = 0; i_a < 10; i_a++ )
		if( i_a < numbArgPrev && i_a >= numbArg )
    		    delete (ArgObj*)w->dc()[QString("arg_%1").arg(i_a)].value<void*>();
		else if( i_a >= numbArgPrev && i_a < numbArg )
		    w->dc()[QString("arg_%1").arg(i_a)].setValue( (void*)new ArgObj() );
	    w->dc()["numbArg"] = numbArg;
	    reform = true;
	    break;
	}
	default: 
 	    //- Individual arguments process -
	    if( uiPrmPos >= 50 && uiPrmPos < 150 )
	    {
		int argN = (uiPrmPos/10)-5;
		ArgObj *arg = (ArgObj*)w->dc()[QString("arg_%1").arg(argN)].value<void*>();
		if((uiPrmPos%10) == 0 || (uiPrmPos%10) == 1 )
		{
		    QVariant gval = arg->val();
		    int tp = (gval.type()==QVariant::Double) ? 1 : ((gval.type()==QVariant::String) ? 2 : 0);
		    if( (uiPrmPos%10) == 0 )	gval = val.c_str();
		    if( (uiPrmPos%10) == 1 )	tp = atoi(val.c_str());
		    switch( tp )
    		    {
			case 0: arg->setVal(gval.toInt());	break;
		    	case 1: arg->setVal(gval.toDouble());	break;
    			case 2: arg->setVal(gval.toString());	break;
		    }
		}
		if( (uiPrmPos%10) == 2 ) arg->setCfg(val.c_str());
    		reform = true;
	    }else up = false;
    }

    //- Reload generic font record -
    if( rel_fnt && !w->allAttrLoad() )
    {
        QFont *fnt = (QFont*)w->dc()["QFont"].value<void*>();
	char family[101];
	int size, bold, italic, underline, strike;        
	int pcnt = sscanf(w->dc().value("font",0).toString().toAscii().data(),
		    "%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
	if( pcnt >= 1 )	fnt->setFamily(string(family,100).c_str());
	if( pcnt >= 2 )	fnt->setPointSize(size);
	if( pcnt >= 3 )	fnt->setBold(bold);
	if( pcnt >= 4 )	fnt->setItalic(italic);
	if( pcnt >= 5 )	fnt->setUnderline(underline);
	if( pcnt >= 6 )	fnt->setStrikeOut(strike);
	up = true;
    }

    //- Text reformation -
    if( reform && !w->allAttrLoad() )
    {
	ArgObj *arg;
	int numbArg = w->dc()["numbArg"].toInt();
	QString text = w->dc()["text_tmpl"].toString();
	for( int i_a = 0; i_a < numbArg; i_a++ )
	{
	    arg = (ArgObj*)w->dc()[QString("arg_%1").arg(i_a)].value<void*>();
	    switch(arg->val().type())
	    {
		case QVariant::String: text = text.arg(arg->val().toString(),atoi(arg->cfg().c_str())); break;
		case QVariant::Double:
		{
		    int off = 0;
		    int wdth = atoi(TSYS::strSepParse(arg->cfg(),0,';',&off).c_str());
		    string form = TSYS::strSepParse(arg->cfg(),0,';',&off);
		    int prec = atoi(TSYS::strSepParse(arg->cfg(),0,';',&off).c_str());
		    text = text.arg(arg->val().toDouble(),wdth,form.empty()?0:form[0],prec,' ');
		    break;
		}
		default: text = text.arg(arg->val().toInt(),atoi(arg->cfg().c_str())); break;
	    }
	}
	if( w->dc()["text"].toString() == text ) up = false;
	else { w->dc()["text"] = text; up = true; }
    }
    
    if( up && !w->allAttrLoad( ) ) w->update();
    
    return up;
}

bool ShapeText::event( WdgView *w, QEvent *event )
{
    if( !w->dc().value("en",1).toInt() ) return true;
    switch(event->type())
    {
        case QEvent::Paint:
        {    	    
	    QPainter pnt( w );
	    
	    //- Prepare draw area -
	    int margin = w->dc()["geomMargin"].toInt();
	    QRect draw_area(w->rect().x(),w->rect().y(),
		(int)((float)w->rect().width()/w->xScale(true))-2*margin,
		(int)((float)w->rect().height()/w->yScale(true))-2*margin);
	    pnt.setWindow(draw_area);
	    pnt.setViewport(w->rect().adjusted((int)(w->xScale(true)*margin),(int)(w->yScale(true)*margin),
		-(int)(w->xScale(true)*margin),-(int)(w->yScale(true)*margin)));
	    
	    /*QRect draw_area = w->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
	    pnt.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));*/
	    
	    pnt.translate(draw_area.width()/2,draw_area.height()/2 );
	    int angle = w->dc()["orient"].toInt();
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
	    QColor bkcol = w->dc()["backColor"].value<QColor>();
	    if(  bkcol.isValid() ) pnt.fillRect(draw_rect,bkcol);
	    QBrush bkbrsh = w->dc()["backImg"].value<QBrush>();
	    if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_rect,bkbrsh);
	    
	    QPen *bpen = (QPen*)w->dc()["border"].value<void*>();
	    if(  bpen->width() )
	    {
		pnt.setPen(*bpen);
		pnt.drawRect(draw_rect.adjusted(bpen->width()/2,bpen->width()/2,
			     -bpen->width()/2-bpen->width()%2,-bpen->width()/2-bpen->width()%2));
		draw_rect.adjust(bpen->width()+1,bpen->width()+1,bpen->width()-1,bpen->width()-1);
	    }
	    
	    //- Draw text -
	    pnt.setPen(w->dc()["color"].value<QColor>());
	    pnt.setFont(*(QFont*)w->dc()["QFont"].value<void*>());
	    
	    pnt.drawText(draw_rect,w->dc()["text_flg"].toInt(),w->dc()["text"].toString());
	    
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
    w->dc()["border"].setValue( (void*)new QPen() );
    w->dc()["mediaType"] = -1;
    //- Create label widget -    
    QLabel *lab = new QLabel(w);
    if( qobject_cast<DevelWdgView*>(w) ) lab->setMouseTracking(true);
    lab->setAlignment(Qt::AlignCenter);
    lab->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);    
    w->dc()["labWdg"].setValue( (void*)lab );
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->addWidget(lab);
}

void ShapeMedia::destroy( WdgView *w )
{
    delete (QPen*)w->dc()["border"].value<void*>();
    //- Clear label widget's elements -
    QLabel *lab = (QLabel*)w->dc()["labWdg"].value<void*>();
    if( lab && lab->movie() )
    {
	if(lab->movie()->device()) delete lab->movie()->device();
	delete lab->movie();
	lab->clear();
    }
}

bool ShapeMedia::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    bool up = true, reld_src = false;
    QLabel *lab = (QLabel*)w->dc()["labWdg"].value< void* >();

    switch(uiPrmPos)
    {
	case -1:	//load
	    reld_src = true;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    w->dc()["en"] = (bool)atoi(val.c_str()); 
	    break;
	case 12:	//geomMargin
	    w->dc()["geomMargin"] = atoi(val.c_str());
	    w->layout()->setMargin( atoi(val.c_str()) );
	    break;
	case 20: 	//backColor
	    w->dc()["backColor"] = QColor(val.c_str()); break;
	case 21:	//backImg
	{
	    XMLNode req("get");
    	    req.setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",val);
    	    if( !w->cntrIfCmd(req) )
    	    {
		QBrush brsh;
		string backimg = TSYS::strDecode(req.text(),TSYS::base64);
		if( !backimg.empty() )
		{
		    QImage img;
		    if(img.loadFromData((const uchar*)backimg.c_str(),backimg.size()))	
			brsh.setTextureImage(img);
		}
		w->dc()["backImg"] = brsh;
    	    } else up = false;
	    break;
	}
	case 22:	//bordWidth
	    ((QPen*)w->dc()["border"].value<void*>())->setWidth(atoi(val.c_str()));     break;
	case 23:	//bordColor
	    ((QPen*)w->dc()["border"].value<void*>())->setColor(QColor(val.c_str()));	break;
	case 24:	//src
	    if( w->dc()["mediaSrc"].toString() == val.c_str() )	break;
	    w->dc()["mediaSrc"] = val.c_str();
	    reld_src = true;
	    break;	
	case 25:	//type
	    if( w->dc()["mediaType"].toInt() == atoi(val.c_str()) )	break;
	    w->dc()["mediaType"] = atoi(val.c_str()); 
	    reld_src = true; 
	    break;
	case 26:	//fit
	    w->dc()["mediaFit"] = atoi(val.c_str());
	    lab->setScaledContents( atoi(val.c_str()) );
	    break;	    
	case 27: 	//speed
	{
	    int vl = atoi(val.c_str());
	    w->dc()["mediaSpeed"] = vl;
	    if( !lab->movie() ) break;
	    if( vl <= 1 ) lab->movie()->stop();
	    else
	    {
		lab->movie()->setSpeed(vl);
		lab->movie()->start();
	    }
	    break;
	}
    }
    
    if( reld_src && !w->allAttrLoad() )
    {
	XMLNode req("get");
        req.setAttr("path",w->id()+"/%2fwdg%2fres")->
	    setAttr("id",w->dc()["mediaSrc"].toString().toAscii().data());
        if( !w->cntrIfCmd(req) )
        {
	    string sdata = TSYS::strDecode(req.text(),TSYS::base64);	    
	    if( !sdata.empty() )
		switch(w->dc()["mediaType"].toInt())
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
			if( img.loadFromData((const uchar*)sdata.data(),sdata.size()) )
 			    lab->setPixmap(QPixmap::fromImage(img.scaled(
				    (int)((float)img.width()*w->xScale(true)),
				    (int)((float)img.height()*w->yScale(true)),
				    Qt::KeepAspectRatio,Qt::SmoothTransformation)));			
			lab->setScaledContents( w->dc()["mediaFit"].toInt() );
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
			//- Play speed set -
			int vl = w->dc()["mediaSpeed"].toInt();
	        	if( vl <= 1 ) lab->movie()->stop();
			else
			{
			    lab->movie()->setSpeed(vl);
			    lab->movie()->start();
			}
			//- Fit set -
			lab->setScaledContents( w->dc()["mediaFit"].toInt() );
			//if( !lab->hasScaledContents() )
			//    lab->movie()->setScaledSize(QSize((int)((float)lab->movie()->scaledSize().width()*w->xScale(true)), 
			//				      (int)((float)lab->movie()->scaledSize().height()*w->yScale(true))) );
			break;
		    }			
		}
	}
    }

    if( up && !w->allAttrLoad( ) ) w->update();
    
    return up;
}

bool ShapeMedia::event( WdgView *w, QEvent *event )
{
    if( !w->dc().value("en",1).toInt() ) return true;
    if( event->type() == QEvent::Paint )
    {
	QPainter pnt( w );
	    
	//- Prepare draw area -
	int margin = w->dc()["geomMargin"].toInt();
	QRect draw_area = w->rect().adjusted(0,0,-2*margin,-2*margin);	    
        pnt.setWindow(draw_area);
	pnt.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));
	
	//- Draw decoration -
	QColor bkcol = w->dc()["backColor"].value<QColor>();
	if( bkcol.isValid() ) pnt.fillRect(draw_area,bkcol);
	QBrush bkbrsh = w->dc()["backImg"].value<QBrush>();
	if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_area,bkbrsh);
	    
	QPen bpen = w->dc()["bordPen"].value<QPen>();
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

void ShapeDiagram::init( WdgView *w )
{
    w->dc()["tTime"] = 0;
    w->dc()["curTime"] = 0;
    w->dc()["parNum"] = 0;
    w->dc()["tTimeCurent"] = false; 
    w->dc()["trcPer"] = 0;
    w->dc()["border"].setValue( (void*)new QPen() );
    w->dc()["pictObj"].setValue( (void*)new QPicture() );
    w->dc()["pictRect"].setValue( (void*)new QRect() );
    w->dc()["sclMarkFont"].setValue( (void*)new QFont() );
    //- Init tracing timer -
    QTimer *tmr = new QTimer(w);
    w->dc()["trcTimer"].setValue( (void*)tmr );
    connect( tmr, SIGNAL(timeout()), this, SLOT(tracing()) );    
}

void ShapeDiagram::destroy( WdgView *w )
{
    delete (QPen*)w->dc()["border"].value<void*>();
    delete (QPicture*)w->dc()["pictObj"].value<void*>();
    delete (QRect*)w->dc()["pictRect"].value<void*>();
    delete (QFont*)w->dc()["sclMarkFont"].value<void*>();
    ((QTimer*)w->dc()["trcTimer"].value<void*>())->stop();
    //- Clear trend's data objects -
    int parNum = w->dc()["parNum"].toInt();
    for( int i_p = 0; i_p < parNum; i_p++ )
	delete (TrendObj*)w->dc().value(QString("trend_%1").arg(i_p)).value<void*>();
}

bool ShapeDiagram::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    bool up = false, 		//Repaint diagramm picture
	 make_pct = false;  	//Remake diagramm picture
    int  reld_tr_dt = 0;	//Reload trend's data ( 1-reload addons, 2-full reload)

    switch(uiPrmPos)
    {
	case -1:	//load
	    up = make_pct = true;
	    reld_tr_dt = 2;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	break;
	    w->dc()["en"] = (bool)atoi(val.c_str());
	    up = true; 
	    break;
	case 6:		//active
	    w->dc()["active"] = (bool)atoi(val.c_str());
	    if( w->dc()["active"].toBool() )	w->setFocusPolicy(Qt::StrongFocus);
	    else w->setFocusPolicy(Qt::NoFocus);
	    break;
	case 9:	case 10: make_pct = true;	break;
	case 12:	//geomMargin
	    w->dc()["geomMargin"] = atoi(val.c_str()); make_pct = true; break;
	case 20:	//backColor
	    w->dc()["backColor"] = QColor(val.c_str()); make_pct = true; break;
	case 21:	//backImg
	{
	    XMLNode req("get");
    	    req.setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",val);
    	    if( !w->cntrIfCmd(req) )
    	    {
		QBrush brsh;
		string backimg = TSYS::strDecode(req.text(),TSYS::base64);
		if( !backimg.empty() )
		{
		    QImage img;
		    if(img.loadFromData((const uchar*)backimg.c_str(),backimg.size()))	
			brsh.setTextureImage(img);
		}
		w->dc()["backImg"] = brsh;
	      	up = true;
    	    } 
	    break;
	}
	case 22:	//bordWidth
	    ((QPen*)w->dc()["border"].value<void*>())->setWidth(atoi(val.c_str())); make_pct = true; break;
	case 23:	//bordColor
	    ((QPen*)w->dc()["border"].value<void*>())->setColor(QColor(val.c_str())); make_pct = true; break;
	case 24:	//trcPer
	    w->dc()["trcPer"] = atoi(val.c_str());
	    if( atoi(val.c_str()) )
		((QTimer*)w->dc()["trcTimer"].value<void*>())->start(atoi(val.c_str())*1000);
	    else ((QTimer*)w->dc()["trcTimer"].value<void*>())->stop();
	    break;
	//case 25: 	//type
	//    w->dc()["type"] = atoi(val.c_str()); make_pct = true; break;
	case 26:    	//tSek
	    w->dc()["tTimeCurent"] = false;
	    if( atoll(val.c_str()) == 0 )
	    {
		w->dc()["tTime"] = (long long)time(NULL)*1000000;
		w->dc()["tTimeCurent"] = true;
	    } else w->dc()["tTime"] = atoll(val.c_str())*1000000 + w->dc()["tTime"].toLongLong()%1000000;
	    reld_tr_dt = 1;	break;
	case 27: 	//tUSek
	    w->dc()["tTime"] = 1000000ll*(w->dc()["tTime"].toLongLong()/1000000)+atoll(val.c_str());
	    reld_tr_dt = 1;	break;
	case 28:	//tSize
	    w->dc()["tSize"] = atof(val.c_str());
	    reld_tr_dt = 1;  break;
	case 29:	//curSek
	    w->dc()["curTime"] = atoll(val.c_str())*1000000 + w->dc()["curTime"].toLongLong()%1000000;
	    up = true;	break;
	case 30:	//curUSek
	    w->dc()["curTime"] = 1000000ll*(w->dc()["curTime"].toLongLong()/1000000)+atoll(val.c_str());
	    up = true;	break;
	case 36:	//curColor
	    w->dc()["curColor"] = val.c_str();
	    up = true;  break;
	case 31:	//sclColor
	    w->dc()["sclColor"] = val.c_str();		make_pct = true;	break;
	case 32:	//sclHor
	    w->dc()["sclHor"] = atoi(val.c_str());	make_pct = true;   	break;
	case 33:	//sclVer
	    w->dc()["sclVer"] = atoi(val.c_str());	make_pct = true;	break;
	case 37:	//sclMarkColor
	    w->dc()["sclMarkColor"] = val.c_str();	make_pct = true;        break;
	case 38:	//sclMarkFont
	{
	    QFont *fnt = (QFont*)w->dc()["sclMarkFont"].value<void*>();
	    char family[101];
	    int size, bold, italic, underline, strike;        
	    int pcnt = sscanf(val.c_str(),"%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
	    if( pcnt >= 1 )	fnt->setFamily(string(family,100).c_str());
	    if( pcnt >= 2 )	fnt->setPointSize(size);
	    if( pcnt >= 3 )	fnt->setBold(bold);
	    if( pcnt >= 4 )	fnt->setItalic(italic);
	    if( pcnt >= 5 )	fnt->setUnderline(underline);
	    if( pcnt >= 6 )	fnt->setStrikeOut(strike);
	    make_pct = true;
	    break;	    
	}    
	case 34:	//valArch
	    if( w->dc()["valArch"].toString() == val.c_str() )	break;
	    w->dc()["valArch"] = val.c_str();
	    reld_tr_dt = 2;	break;
	case 35:	//parNum
	{	    
	    int parNumPrev = w->dc()["parNum"].toInt();
	    int parNum = atoi(val.c_str());
	    if( parNum == parNumPrev )	break;
	    for( int i_p = 0; i_p < 10; i_p++ )
		if( i_p < parNumPrev && i_p >= parNum )
		    delete (TrendObj*)w->dc()[QString("trend_%1").arg(i_p)].value<void*>();
		else if( i_p >= parNumPrev && i_p < parNum )
		    w->dc()[QString("trend_%1").arg(i_p)].setValue( (void*)new TrendObj(w) );
	    w->dc()["parNum"] = parNum;	    
	    make_pct = true;
	    break;
	}
	default:
	    //- Individual trend's attributes process -
	    if( uiPrmPos >= 50 && uiPrmPos < 150 )
	    {
		int trndN = (uiPrmPos/10)-5;
		TrendObj *trnd = (TrendObj*)w->dc()[QString("trend_%1").arg(trndN)].value<void*>();
		switch(uiPrmPos%10)
		{
		    case 0: trnd->setAddr(val.c_str());		break;		//addr
		    case 1: trnd->setBordL(atof(val.c_str()));	break;		//bordL
		    case 2: trnd->setBordU(atof(val.c_str()));	break;		//bordU
		    case 3: trnd->setColor(val.c_str());	break;		//color
		    case 4: trnd->setCurVal(atof(val.c_str()));	break;		//value
		}
		make_pct = true;
	    }
    }
    
    if( !w->allAttrLoad( ) )
    {
	if( reld_tr_dt )loadTrendsData(w,reld_tr_dt==2);
	if( make_pct )	makeTrendsPicture(w);
	if( up )	w->update();
    }
    
    return up;
}

void ShapeDiagram::loadTrendsData( WdgView *w, bool full )
{
    int parNum = w->dc()["parNum"].toInt();
    for( int i_p = 0; i_p < parNum; i_p++ )
	((TrendObj*)w->dc()[QString("trend_%1").arg(i_p)].value<void*>())->loadData(full);
}

void ShapeDiagram::makeTrendsPicture( WdgView *w )
{
    QPen grdPen, mrkPen;
    int  mrkHeight = 0;
    if( !w->dc().value("en",1).toInt() )	return;

    //- Prepare picture -    
    QPainter pnt((QPicture*)w->dc()["pictObj"].value<void*>());

    //-- Get generic parameters --
    int parNum     = w->dc()["parNum"].toInt();				//Parameter's number
    int tSize      = (int)(w->dc()["tSize"].toDouble()*1000000.);	//Trends size (us)
    long long tEnd = w->dc()["tTime"].toLongLong();			//Trends end point (us)
    long long tBeg = tEnd - tSize;					//Trends begin point (us)
    if( !parNum || tSize <= 0 ) return;

    //-- Make decoration and prepare trends area --
    int margin = w->dc()["geomMargin"].toInt();				//Trends generic margin
    QRect tAr  = w->rect().adjusted(margin+1,margin+1,-2*(margin+1),-2*(margin+1));	//Curves of trends area rect
    int sclHor = w->dc()["sclHor"].toInt();				//Horisontal scale mode
    int sclVer = w->dc()["sclVer"].toInt();				//Vertical scale mode
    
    if( sclHor&0x3 || sclVer&0x3 )
    {   
	//--- Set grid pen ---
	grdPen.setColor(w->dc()["sclColor"].toString());
	grdPen.setStyle(Qt::SolidLine);
	grdPen.setWidth(1);
	if( sclHor&0x2 || sclVer&0x2 )
	{
	    //--- Set markers font and color ---
	    mrkPen.setColor(w->dc()["sclMarkColor"].toString());
	    pnt.setFont(*(QFont*)w->dc()["sclMarkFont"].value<void*>());
	    mrkHeight = pnt.fontMetrics().height();
    
	    if( sclHor&0x2 )
	    {
		if( tAr.height() < 100 ) sclHor &= ~(0x02);
		else tAr.adjust(0,0,0,-2*mrkHeight);
	    }
	    if( sclVer&0x2 )
	    {
		if( tAr.width() < 100 ) sclVer &= ~(0x02);
		else tAr.adjust(pnt.fontMetrics().width("9.999"),0,0,0);
	    }
	}
    }
    
    //--- Calc horizontal scale ---
    long long aVend;        		//Corrected for allow data the trend end point
    long long aVbeg;			//Corrected for allow data the trend begin point
    long long hDiv = 1, hDivBase = 1;	//Horisontal scale divisor
    
    int hmax_ln = tAr.width()/30;
    if( hmax_ln >= 2 )
    {
	int hvLev = 0;
	long long hLen = tEnd - tBeg;
	if( hLen/86400000000ll > 2 )	{ hvLev = 5; hDivBase = hDiv = 86400000000ll; }	//Days
	else if( hLen/3600000000ll > 2 ){ hvLev = 4; hDivBase = hDiv =  3600000000ll; }	//Hours
	else if( hLen/60000000 > 2 )	{ hvLev = 3; hDivBase = hDiv =    60000000; }	//Minutes
	else if( hLen/1000000 > 2 )	{ hvLev = 2; hDivBase = hDiv =     1000000; }	//Seconds
	else if( hLen/1000 > 2 )	{ hvLev = 1; hDivBase = hDiv =        1000; }	//Milliseconds
	while( hLen/hDiv > hmax_ln )	hDiv *= 10; 
	while( hLen/hDiv < hmax_ln/2 && !((hDiv/2)%hDivBase) )	hDiv/=2;

	//--- Draw horisontal grid and markers ---
	if( sclHor&0x3 )
	{
	    time_t tm_t;
	    struct tm *ttm, ttm1;
	    QString lab_tm, lab_dt;
	    //---- Draw generic grid line ----
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y()+tAr.height(),tAr.x()+tAr.width(),tAr.y()+tAr.height());
	    //---- Draw full trend's data and time to the trend end position ----
	    int endMarkBrd = tAr.x()+tAr.width();
	    if( sclHor&0x2 )
	    {
		pnt.setPen(mrkPen);
		tm_t = tEnd/1000000;
		ttm = localtime(&tm_t);
		lab_dt = QString("%1-%2-%3").arg(ttm->tm_mday).arg(ttm->tm_mon+1,2,10,QChar('0')).arg(ttm->tm_year+1900);
		if( ttm->tm_sec == 0 && tEnd%1000000 == 0 )
		    lab_tm = QString("%1:%2").arg(ttm->tm_hour).arg(ttm->tm_min,2,10,QChar('0'));
		else if( tEnd%1000000 == 0 )
		    lab_tm = QString("%1:%2:%3").arg(ttm->tm_hour).arg(ttm->tm_min,2,10,QChar('0')).arg(ttm->tm_sec,2,10,QChar('0'));
		else lab_tm = QString("%1:%2:%3.%4").arg(ttm->tm_hour).arg(ttm->tm_min,2,10,QChar('0')).arg(ttm->tm_sec,2,10,QChar('0')).arg(tEnd%1000000);
		int markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().boundingRect(lab_tm).width();
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+mrkHeight,lab_tm);
		markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().boundingRect(lab_dt).width();
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+2*mrkHeight,lab_dt);
	    }
	    //---- Draw grid and/or markers ----
	    bool first_m = true;
	    for( long long i_h = tBeg; true; )
	    {
		//---- Draw grid ----
		pnt.setPen(grdPen);
		int h_pos = tAr.x()+tAr.width()*(i_h-tBeg)/(tEnd-tBeg);
		if( sclHor&0x1 ) pnt.drawLine(h_pos,tAr.y(),h_pos,tAr.y()+tAr.height());
		else pnt.drawLine(h_pos,tAr.y()+tAr.height()-3,h_pos,tAr.y()+tAr.height()+3);
		
		if( sclHor&0x2 && !(i_h%hDiv) && i_h != tEnd )
		{		    
		    tm_t = i_h/1000000;
		    ttm = localtime(&tm_t);
		    int chLev = 0;
		    if( !first_m )
		    {
			if( ttm->tm_mon > ttm1.tm_mon || ttm->tm_year > ttm1.tm_year )	chLev = 5;
			else if( ttm->tm_mday > ttm1.tm_mday )	chLev = 4;
			else if( ttm->tm_hour > ttm1.tm_hour )	chLev = 3;
			else if( ttm->tm_min > ttm1.tm_min )	chLev = 2;
			else if( ttm->tm_sec > ttm1.tm_sec )	chLev = 1;
		    }
		
		    //Check for data present
		    lab_dt.clear(), lab_tm.clear();
		    if( hvLev == 5 || chLev >= 4 ) 					//Date
			lab_dt = (chLev>=5) ? QString("%1-%2-%3").arg(ttm->tm_mday).arg(ttm->tm_mon+1,2,10,QChar('0')).arg(ttm->tm_year+1900) :
					      QString::number(ttm->tm_mday);
		    if( (hvLev == 4 || hvLev == 3 || ttm->tm_min) && !ttm->tm_sec )	//Hours and minuts
			lab_tm =  QString("%1:%2").arg(ttm->tm_hour).arg(ttm->tm_min,2,10,QChar('0'));
		    else if( (hvLev == 2 || ttm->tm_sec) && !(i_h%1000000) )		//Seconds
			lab_tm = (chLev>=2) ? QString("%1:%2:%3").arg(ttm->tm_hour).arg(ttm->tm_min,2,10,QChar('0')).arg(ttm->tm_sec,2,10,QChar('0')) :
					      QString("%1s").arg(ttm->tm_sec);
		    else if( hvLev <= 1 || i_h%1000000 )				//Milliseconds
			lab_tm = (chLev>=2) ? QString("%1:%2:%3.%4").arg(ttm->tm_hour).arg(ttm->tm_min,2,10,QChar('0')).arg(ttm->tm_sec,2,10,QChar('0')).arg(i_h%1000000) :
				 (chLev>=1) ? QString("%1.%2s").arg(ttm->tm_sec).arg(i_h%1000000) :
					      QString("%1ms").arg((double)(i_h%1000000)/1000.,0,'g',4);
		    int wdth;
		    pnt.setPen(mrkPen);
		    if( lab_tm.size() )
		    {
			wdth = pnt.fontMetrics().boundingRect(lab_tm).width();
			if( (h_pos+wdth/2) < endMarkBrd )
			    pnt.drawText( vmax(h_pos-wdth/2,0), tAr.y()+tAr.height()+mrkHeight, lab_tm );
		    }
		    if( lab_dt.size() )
		    {
			wdth = pnt.fontMetrics().boundingRect(lab_dt).width();
			if( (h_pos+wdth/2) < endMarkBrd )
			    pnt.drawText( vmax(h_pos-wdth/2,0), tAr.y()+tAr.height()+2*mrkHeight, lab_dt );
		    }
		    memcpy((char*)&ttm1,(char*)ttm,sizeof(tm));
		    first_m = false;
		}
		//---- Next ----
		if( i_h >= tEnd )	break;
		i_h = (i_h/hDiv)*hDiv + hDiv;
		if( i_h > tEnd )	i_h = tEnd;
	    }
	}
    }
    
    //--- Calc vertical scale ---
    //---- Check for scale mode ----
    double vsMax = 100, vsMin = 0;	//Trend's vertical scale border
    bool   vsPerc = true;		//Vertical scale percent mode
    if( parNum == 1 )
    {
	TrendObj *sTr = (TrendObj*)w->dc()["trend_0"].value<void*>();
	if( sTr->bordU() <= sTr->bordL() )
	{
    	    vsPerc = false;
	    //----- Check trend for valid data -----
	    aVbeg = vmax(tBeg,sTr->valBeg());
	    aVend = vmin(tEnd,sTr->valEnd());

	    if( aVbeg >= aVend ) return;
	    //----- Calc value borders -----
	    vsMax = -3e300, vsMin = 3e300;
	    for( int ipos = sTr->val(aVbeg); ipos < sTr->val().size() && sTr->val()[ipos].tm <= aVend; ipos++ )
	    {
		if( sTr->val()[ipos].val == EVAL_REAL ) continue;
		vsMin  = vmin(vsMin,sTr->val()[ipos].val);
                vsMax  = vmax(vsMax,sTr->val()[ipos].val);
	    }
	    if( vsMax == -3e300 ) { vsMax = 1; vsMin = 0; }
	}
    }
    
    float vmax_ln = tAr.height()/20;
    if( vmax_ln >= 2 )
    {
	double vDiv = 1.;        
	double v_len = vsMax - vsMin;
	while(v_len > vmax_ln)	{ vDiv*=10.; v_len/=10.; }
	while(v_len < vmax_ln/10){ vDiv/=10.; v_len*=10.; }
	vsMin = floor(vsMin/vDiv)*vDiv;
	vsMax = ceil(vsMax/vDiv)*vDiv;
	while(((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv/=2;	
	//--- Draw vertical grid and markers ---
	if( sclVer&0x3 )
	{
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y(),tAr.x(),tAr.height());
	    for(double i_v = vsMin; i_v <= vsMax; i_v+=vDiv)
	    {
		int v_pos = tAr.y()+tAr.height()-(int)((double)tAr.height()*(i_v-vsMin)/(vsMax-vsMin));
		pnt.setPen(grdPen);
		if( sclVer&0x1 ) pnt.drawLine(tAr.x(),v_pos,tAr.x()+tAr.width(),v_pos);
		else pnt.drawLine(tAr.x()-3,v_pos,tAr.x()+3,v_pos);
		
		if( sclVer&0x2 )
		{
		    pnt.setPen(mrkPen);
		    pnt.drawText(tAr.x()-20,v_pos+((i_v==vsMin)?0:(i_v==vsMax)?10:5),
						QString::number(i_v,'g',4));
		}
	    }
	}
    }
    
    //-- Draw trends --
    for( int i_t = 0; i_t < parNum; i_t++ )
    {
	TrendObj *sTr = (TrendObj*)w->dc()[QString("trend_%1").arg(i_t)].value<void*>();
	
	//--- Set trend's pen ---
	QPen trpen(QColor(sTr->color().c_str()));
	trpen.setStyle(Qt::SolidLine);
	trpen.setWidth(1);
	pnt.setPen(trpen);
	
	//--- Prepare generic parameters ---
	aVbeg = vmax(tBeg,sTr->valBeg());
	aVend = vmin(tEnd,sTr->valEnd());
	if( aVbeg >= aVend ) continue;
	int aPosBeg = sTr->val(aVbeg);;
	//int aPosEnd = (aVend-sTr->valBeg())/sTr->valPer();
	
	//--- Prepare border for percent trend ---
	float bordL = sTr->bordL();
	float bordU = sTr->bordU();	
	if( vsPerc && bordL >= bordU )
	{
	    bordU = -3e300, bordL = 3e300;
	    for( int ipos = aPosBeg; ipos < sTr->val().size() && sTr->val()[ipos].tm <= aVend; ipos++ )
	    {
		if( sTr->val()[ipos].val == EVAL_REAL ) continue;
		bordL = vmin(bordL,sTr->val()[ipos].val);
                bordU = vmax(bordU,sTr->val()[ipos].val);
	    }
	    float vMarg = (bordU-bordL)/10;
	    bordL-= vMarg;
	    bordU+= vMarg;
	}
	
	//--- Draw trend ---
	double curVl, averVl = EVAL_REAL, prevVl = EVAL_REAL;
	int    curPos, averPos = 0, prevPos = 0;
	long long curTm, averTm = 0, averLstTm = 0;
	for( int a_pos = aPosBeg; true; a_pos++ )
	{	    
	    curTm = sTr->val()[a_pos].tm;//   sTr->valBeg()+a_pos*sTr->valPer();
    	    if( a_pos < sTr->val().size() && sTr->val()[a_pos].tm <= aVend )
	    {
        	curVl = sTr->val()[a_pos].val;
		if( vsPerc && curVl != EVAL_REAL )
		{
		    curVl = 100.*(curVl-bordL)/(bordU-bordL);
		    curVl = (curVl>100) ? 100 : (curVl<0) ? 0 : curVl;
		}
                curPos = tAr.x()+tAr.width()*(curTm-tBeg)/(tEnd-tBeg);
    	    }else curPos = 0;
    	    //Square Average
	    if( averPos == curPos )
            {
                if( averVl == EVAL_REAL )  averVl = curVl;
        	else if( curVl != EVAL_REAL )
                    averVl = (averVl*(double)(curTm-averTm)+curVl*(double)(curTm-averLstTm))/
                              ((double)(2*curTm-averTm-averLstTm));
                averLstTm = curTm;
        	continue;
            }
    	    //Write point and line
	    if( averVl != EVAL_REAL )
	    {
	        int c_vpos = tAr.y()+tAr.height()-(int)((double)tAr.height()*(averVl-vsMin)/(vsMax-vsMin));
		pnt.drawPoint(averPos,c_vpos);
		if( prevVl != EVAL_REAL )
	        {
	            int c_vpos_prv = tAr.y()+tAr.height()-(int)((double)tAr.height()*(prevVl-vsMin)/(vsMax-vsMin));
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
    
    (*(QRect*)w->dc()["pictRect"].value<void*>()) = tAr;
    
    //- Call repaint -
    w->update();
}

void ShapeDiagram::tracing( )
{
    WdgView *w = (WdgView *)((QTimer*)sender())->parent();
    if( !w->isEnabled() ) return;
    
    long long tTime  = w->dc()["tTime"].toLongLong();
    long long trcPer = w->dc()["trcPer"].toInt();
    if( w->dc()["tTimeCurent"].toBool() )
	w->dc()["tTime"] = (long long)time(NULL)*1000000;
    else if( tTime )	w->dc()["tTime"] = tTime+trcPer*1000000;
    loadTrendsData(w);
    makeTrendsPicture(w);
    
    //- Trace cursors value -
    if( w->dc().value("active",1).toInt() )
    {
	long long tTimeGrnd = tTime - (int)(w->dc()["tSize"].toDouble()*1000000.);
	long long curTime = w->dc()["curTime"].toLongLong();
	if( curTime >= tTime || curTime <= tTimeGrnd )	setCursor( w, tTime+trcPer*1000000 );
    }
}

bool ShapeDiagram::event( WdgView *w, QEvent *event )
{
    if( !w->dc().value("en",1).toInt() ) return true;
    
    //- Get generic data -
    long long tTime     = w->dc()["tTime"].toLongLong();
    long long tTimeGrnd = tTime - (int)(w->dc()["tSize"].toDouble()*1000000.);
    long long curTime	= vmax(vmin(w->dc()["curTime"].toLongLong(),tTime),tTimeGrnd);
    QRect *tAr = (QRect*)w->dc()["pictRect"].value<void*>();    
    
    //- Process event -
    switch( event->type() )
    {
	case QEvent::Paint:
	{
	    QPainter pnt( w );
	
	    //- Decoration draw -
	    int margin = w->dc()["geomMargin"].toInt();	
	    QRect dA = w->rect().adjusted(0,0,-2*margin,-2*margin);	    
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));
	
	    //- Draw decoration -
	    QColor bkcol = w->dc()["backColor"].value<QColor>();
	    if( bkcol.isValid() ) pnt.fillRect(dA,bkcol);
	    QBrush bkbrsh = w->dc()["backImg"].value<QBrush>();
	    if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(dA,bkbrsh);
    
	    QPen *bpen = (QPen*)w->dc()["border"].value<void*>();
	    if( bpen->width() )
	    {
    		pnt.setPen(*bpen);
    		pnt.drawRect(dA.adjusted(bpen->width()/2,bpen->width()/2,
	    	    -bpen->width()/2-bpen->width()%2,-bpen->width()/2-bpen->width()%2));
	    }
	
	    //- Trend's picture -
	    pnt.drawPicture(0, 0,*(QPicture*)w->dc()["pictObj"].value<void*>());
	
	    //- Draw focused border -
	    if( w->hasFocus() )	qDrawShadeRect(&pnt,dA.x(),dA.y(),dA.width(),dA.height(),w->palette());
	
	    //- Draw cursor -
	    if( w->dc().value("active",1).toInt() && (curTime >= tTimeGrnd || curTime <= tTime) )
	    {
		int curPos = tAr->x()+tAr->width()*(curTime-tTimeGrnd)/(tTime-tTimeGrnd);
		QPen curpen(QColor(w->dc()["curColor"].toString()));
		curpen.setWidth(1);
		pnt.setPen(curpen);
		pnt.drawLine(curPos,tAr->y(),curPos,tAr->y()+tAr->height());
	    }	
    	    return true;
	}
	case QEvent::KeyPress:
	{
    	    QKeyEvent *key = static_cast<QKeyEvent*>(event);

	    switch(key->key())
	    {
		case Qt::Key_Left:
		    if( curTime <= tTimeGrnd ) break;
		    setCursor( w, curTime-(tTime-tTimeGrnd)/tAr->width() );
		    return true;
		case Qt::Key_Right:
		    if( curTime >= tTime ) break;
		    setCursor( w, curTime+(tTime-tTimeGrnd)/tAr->width() );
		    return true;
	    }
	    break;
	}
	case QEvent::MouseButtonPress:
	{
	    if( !w->hasFocus() ) break;
	    QPoint curp = w->mapFromGlobal(w->cursor().pos());
	    if( curp.x() < tAr->x() || curp.x() > (tAr->x()+tAr->width()) ) break;
	    setCursor( w, tTimeGrnd + (tTime-tTimeGrnd)*(curp.x()-tAr->x())/tAr->width() );
	    break;
	}
    }
    
    return false;
}

void ShapeDiagram::setCursor( WdgView *w, long long itm )
{
    long long tTime     = w->dc()["tTime"].toLongLong();
    long long tTimeGrnd = tTime - (int)(w->dc()["tSize"].toDouble()*1000000.);
    long long curTime   = vmax(vmin(itm,tTime),tTimeGrnd);	    

    w->attrSet("curSek",TSYS::int2str(itm/1000000),29);
    w->attrSet("curUSek",TSYS::int2str(itm%1000000),30);

    //- Update trend's current values -
    int parNum = w->dc()["parNum"].toInt();
    for( int i_p = 0; i_p < parNum; i_p++ )
    {
        TrendObj *sTr = (TrendObj*)w->dc()[QString("trend_%1").arg(i_p)].value<void*>();
	int vpos = sTr->val(itm);
	if( vpos >= sTr->val().size() )	continue;
	if( vpos && sTr->val()[vpos].tm > itm )	vpos--;
	double val = sTr->val()[vpos].val;
	if( val != sTr->curVal() ) 
	    w->attrSet(QString("prm%1val").arg(i_p).toAscii().data(),TSYS::real2str(val,6),54+10*i_p);
    }
}

//* Trend object's class                         *
//************************************************
ShapeDiagram::TrendObj::TrendObj( WdgView *iview ) : view(iview),
    m_bord_low(0), m_bord_up(0), m_curvl(EVAL_REAL),
    arh_beg(0), arh_end(0), arh_per(0),val_tp(0)
{
    loadData();
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
    int tSize   = (int)(view->dc()["tSize"].toDouble()*1000000.);
    long long tTime     = view->dc()["tTime"].toLongLong();
    long long tTimeGrnd = tTime - tSize;
    int wantPer = tSize/view->size().width();
    string arch = view->dc()["valArch"].toString().toAscii().data();

    //- Clear trend for empty address and the full reload data -
    if( full || addr().empty() )
    { 
	arh_per = arh_beg = arh_end = 0;
	val_tp = 0;
	vals.clear();
	if( addr().empty() )	return;
    }
    //- Get archive parameters -
    if( !arh_per || tTime > arh_end )
    {
	XMLNode req("info");
	req.setAttr("arch",arch)->setAttr("path",addr()+"/%2fserv%2f0");
	if( view->cntrIfCmd(req,true) || atoi(req.attr("vtp").c_str()) == 5 )
	    arh_per = arh_beg = arh_end = 0;
	else
	{
	    val_tp  = atoi(req.attr("vtp").c_str());
	    arh_beg = atoll(req.attr("beg").c_str());
	    arh_end = atoll(req.attr("end").c_str());
	    arh_per = atoi(req.attr("per").c_str());
	}
    }
    //- One request check and prepare -
    int trcPer = view->dc()["trcPer"].toInt()*1000000;
    if( view->dc()["tTimeCurent"].toBool() && trcPer && 
	(!arh_per || (arh_per >= trcPer && (tTime-valEnd())/trcPer < 2)) )
    {
	XMLNode req("get");
	req.setAttr("path",addr()+"/%2fserv%2f0")->
    	    setAttr("tm",TSYS::ll2str(tTime))->
      	    setAttr("tm_grnd","0");
    	if( view->cntrIfCmd(req,true) )	return;
	
    	long long lst_tm = atoll(req.attr("tm").c_str());
	if( lst_tm > valEnd() )
	{
	    double curVal = atof(req.text().c_str());
	    if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	    //printf("TEST 10: %lld %f\n",lst_tm,curVal);
	    if( valEnd() && (lst_tm-valEnd())/trcPer > 2 ) vals.push_back(SHg(lst_tm-trcPer,EVAL_REAL));
	    vals.push_back(SHg(lst_tm,curVal));
	    while( vals.size() > 2000 )	vals.pop_front();
	}
	return;
    }
    
    if( !arh_per )	return;
    //- Correct request to archive border -
    wantPer   = (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime     = vmin(tTime,arh_end);
    tTimeGrnd = vmax(tTimeGrnd,arh_beg);
    //- Clear data at time error -
    if( tTime <= tTimeGrnd || tTimeGrnd/wantPer > valEnd()/wantPer || tTime/wantPer < valBeg()/wantPer )
	vals.clear();
    if( tTime <= tTimeGrnd ) return;
    //- Check for request to present in buffer data -
    if( tTime/wantPer <= valEnd()/wantPer && tTimeGrnd/wantPer >= valBeg()/wantPer )	return;
    //- Correct request to present data -
    if( valEnd() && tTime > valEnd() )		tTimeGrnd = valEnd()+1;
    else if( valBeg() && tTimeGrnd < valBeg() )	tTime = valBeg()-1;
    //- Get values data -
    long long bbeg, bend;
    int bper;
    int 	curPos, prevPos;
    double	curVal, prevVal;
    string	svl;
    vector<SHg>	buf;
    deque<SHg>::iterator bufEndOff = vals.end();
    //printf("TEST 00: (%lld - %lld):%d -- (%lld - %lld)\n",tTimeGrnd,tTime,wantPer,valBeg(),valEnd());
    XMLNode req("get");
    m1: req.clear()->
	    setAttr("arch",arch)->
	    setAttr("path",addr()+"/%2fserv%2f0")->
    	    setAttr("tm",TSYS::ll2str(tTime))->
      	    setAttr("tm_grnd",TSYS::ll2str(tTimeGrnd))->
	    setAttr("per",TSYS::ll2str(wantPer))->
	    setAttr("mode","1")->
	    setAttr("real_prec","4")->
	    setAttr("round_perc","1");
    //printf("TEST 00a: (%lld - %lld):%d\n",tTimeGrnd,tTime,wantPer);
    if( view->cntrIfCmd(req,true) )	return;
    //- Get data buffer parameters -
    bbeg = atoll(req.attr("tm_grnd").c_str());
    bend = atoll(req.attr("tm").c_str());
    bper = atoi(req.attr("per").c_str());
    //printf("TEST 01: (%lld - %lld):%d\n",bbeg,bend,bper);
	    
    prevPos = 0;
    prevVal = EVAL_REAL;
    buf.clear();
    for( int v_off = 0; (svl=TSYS::strSepParse(req.text(),0,'\n',&v_off)).size(); )
    {
	sscanf(svl.c_str(),"%d %lf",&curPos,&curVal);
	if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	//printf("TEST 02: %d %f\n",curPos,curVal);
	for( ; prevPos < curPos-1; prevPos++ )	buf.push_back(SHg(bbeg+(prevPos+1)*bper,prevVal));
    	buf.push_back(SHg(bbeg+curPos*bper,curVal));
    	prevPos = curPos; prevVal = curVal;
    }
    for( ; prevPos < (bend-bbeg)/bper; prevPos++ ) buf.push_back(SHg(bbeg+(prevPos+1)*bper,prevVal));
    //- Append buffer to values deque -
    if( bbeg >= valEnd() )
    {
	vals.insert(bufEndOff,buf.begin(),buf.end());
	while( vals.size() > 2000 )	vals.pop_front();
	bufEndOff = vals.end()-buf.size();
    }
    else if( bend <= valBeg() )
    {
 	vals.insert(vals.begin(),buf.begin(),buf.end());
        while( vals.size() > 2000 )	vals.pop_back();
    }
    //- Check for archive jump -
    if( arch.empty() && (bbeg-tTimeGrnd)/bper )	{ tTime = bbeg-1; goto m1; }
    //- Test print -
    //for( int i_el = 0; i_el < vals.size(); i_el++ )
    //	printf("TEST 03: %lld : %f\n",vals[i_el].tm,vals[i_el].val);
}

//************************************************
//* Protocol view shape widget                   *
//************************************************
ShapeProtocol::ShapeProtocol( ) : WdgShape("Protocol")
{

}
 
void ShapeProtocol::init( WdgView *w )
{
    w->dc()["en"] = true;
    w->dc()["active"] = true; 
    w->dc()["time"] = 0;
    w->dc()["tSize"] = 60;
    w->dc()["tTimeCurent"] = false; 
    w->dc()["arhBeg"] = 0;
    w->dc()["arhEnd"] = 0;
    // - Init main widget -
    QVBoxLayout *lay = new QVBoxLayout(w);
    QTableWidget *tw = new QTableWidget(w);
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    //tw->setSortingEnabled(true);
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    if( devW ) eventFilterSet(w,tw,true);
    setFocus(w,tw,w->dc()["active"].toInt(),devW);
    lay->addWidget(tw);    
    w->dc()["addrWdg"].setValue((void*)tw);
    //- Init tracing timer -
    QTimer *tmr = new QTimer(w);
    w->dc()["trcTimer"].setValue( (void*)tmr );
    connect( tmr, SIGNAL(timeout()), this, SLOT(tracing()) );
    //- Init index map -
    QMap<QString,int> *imp = new QMap<QString,int>();
    w->dc()["indMap"].setValue( (void*)imp );
} 

void ShapeProtocol::destroy( WdgView *w )
{
    ((QTimer*)w->dc()["trcTimer"].value<void*>())->stop();
} 

bool ShapeProtocol::attrSet( WdgView *w, int uiPrmPos, const string &val)
{
    int  reld_dt = 0;	//Reload data ( 1-reload addons, 2-full reload )
    QTableWidget *tw = (QTableWidget *)w->dc()["addrWdg"].value<void*>();

    switch(uiPrmPos)
    {
	case -1:	//load
	    reld_dt = 2;
	    break;
	case 5:		//en
	    if( !qobject_cast<RunWdgView*>(w) )	break;
	    w->dc()["en"] = (bool)atoi(val.c_str());
	    tw->setVisible(atoi(val.c_str()));
	    break;
	case 6:		//active
	    if( !qobject_cast<RunWdgView*>(w) ) break;
	    w->dc()["active"] = (bool)atoi(val.c_str());
	    setFocus(w,tw,atoi(val.c_str()));
	    break;
 	case 12:	//geomMargin
	    w->layout()->setMargin(atoi(val.c_str()));	break;
	case 20:	//backColor
	{
	    QPalette plt;
	    QColor clr(val.c_str());
	    if( clr.isValid() )	plt.setColor(QPalette::Base,QColor(val.c_str()));
	    tw->setPalette(plt);	    
	    break;
	}
	case 21:	//backImg
	{
	    QPalette plt;	
	    XMLNode req("get");
    	    req.setAttr("path",w->id()+"/%2fwdg%2fres")->setAttr("id",val);
    	    if( !w->cntrIfCmd(req) )
    	    {
		QImage img;	    
		string backimg = TSYS::strDecode(req.text(),TSYS::base64);
		if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		    plt.setBrush(QPalette::Base,QBrush(img));
    	    }
	    tw->setPalette(plt);
	    break;
	}
	case 24:	//time
	{
            unsigned int tm = strtoul(val.c_str(),0,10);
	    //if( w->dc()["time"].toUInt() == tm ) break;
	    w->dc()["timeCurent"] = false;
	    if( tm == 0 )
	    {
		w->dc()["time"] = (unsigned int)time(NULL);
		w->dc()["timeCurent"] = true;
	    }else w->dc()["time"] = tm;
	    reld_dt = 1;
	    break;
	}
	case 25:	//tSize
	    if( w->dc()["tSize"].toUInt() == strtoul(val.c_str(),0,10) ) break;
	    w->dc()["tSize"] = (unsigned int)strtoul(val.c_str(),0,10);
	    reld_dt = 1;
	    break;
	case 26:	//trcPer
	{
	    unsigned int trcPer = strtoul(val.c_str(),0,10);
	    if( w->dc()["trcPer"].toUInt() == trcPer ) break;
	    w->dc()["trcPer"] = trcPer;
 	    if( trcPer )
		((QTimer*)w->dc()["trcTimer"].value<void*>())->start(trcPer*1000);
	    else ((QTimer*)w->dc()["trcTimer"].value<void*>())->stop();
	    break;
	}
	case 27:	//arch
	    if( w->dc()["arch"].toString() == val.c_str() ) break;
	    w->dc()["arch"] = val.c_str();
	    reld_dt = 2;
	    break;
	case 28:	//tmpl
	    if( w->dc()["tmpl"].toString() == val.c_str() ) break;
	    w->dc()["tmpl"] = val.c_str();
	    reld_dt = 2;
	    break;
	case 29:	//lev
	    if( w->dc()["lev"].toInt() == atoi(val.c_str()) ) break;
	    w->dc()["lev"] = atoi(val.c_str());
	    reld_dt = 2;
	    break;
	case 30:	//viewOrd
	    if( w->dc()["viewOrd"].toInt() == atoi(val.c_str()) )	break;
	    w->dc()["viewOrd"] = atoi(val.c_str());
	    reld_dt = 2;
	    break;
	case 31:	//col
	    if( w->dc()["col"].toString() == val.c_str() ) break;
	    w->dc()["col"] =  val.c_str();
	    reld_dt = 2;
	    break;
	case 32:	//itProp
	    if( w->dc()["itProp"].toInt() == atoi(val.c_str()) ) break;
	    w->dc()["itProp"] = atoi(val.c_str());
	    reld_dt = 2;
	    break;
    }

    if( reld_dt && !w->allAttrLoad( ) ) 
	loadData(w,reld_dt==2);
    
    return true;
} 

void ShapeProtocol::loadData( WdgView *w, bool full )
{     
    QTableWidget *tw = (QTableWidget *)w->dc()["addrWdg"].value<void*>();
    QMap<QString,int> *imp = (QMap<QString,int> *)w->dc()["indMap"].value<void*>();
    
    //- Check for border of present data -
    unsigned int tTime     = w->dc()["time"].toUInt();
    unsigned int tTimeGrnd = tTime - w->dc()["tSize"].toUInt();
    string arch = w->dc()["arch"].toString().toAscii().data();
    unsigned int arhBeg = w->dc()["arhBeg"].toUInt();
    unsigned int arhEnd = w->dc()["arhEnd"].toUInt();

    if( full )
    {
	tw->setRowCount(0);
	tw->setColumnCount(0);
	imp->clear();
	string clm;
	for( int c_off = 0; (clm=TSYS::strSepParse(w->dc()["col"].toString().toAscii().data(),0,';',&c_off)).size(); )
	    if( clm == "tm" || clm == "lev" || clm == "cat" || clm == "mess" ) 
	    {
		int ncl = tw->columnCount();
		tw->setColumnCount(ncl+1);	
		tw->setHorizontalHeaderItem(ncl,new QTableWidgetItem());
		if( clm == "tm" )	tw->horizontalHeaderItem(ncl)->setText(_("Time"));
		else if( clm == "lev" )	tw->horizontalHeaderItem(ncl)->setText(_("Level"));
		else if( clm == "cat" )	tw->horizontalHeaderItem(ncl)->setText(_("Category"));
		else if( clm == "mess" )tw->horizontalHeaderItem(ncl)->setText(_("Message"));
		tw->horizontalHeaderItem(ncl)->setData(Qt::UserRole,clm.c_str());
		(*imp)[clm.c_str()] = ncl;
	    }
	arhBeg = arhEnd = 0;
    }

    //- Get archive parameters -
    if( !arhBeg || !arhEnd || tTime > arhEnd )
    {
 	XMLNode req("info");
	req.setAttr("arch",arch)->setAttr("path","/Archive/%2fserv%2f0");
	if( w->cntrIfCmd(req,true) )	arhBeg = arhEnd = 0;
	else
	{
	    arhBeg = strtoul(req.attr("beg").c_str(),0,10);
	    arhEnd = strtoul(req.attr("end").c_str(),0,10);
	}
    }
    if( !tw->columnCount() || !arhBeg || !arhEnd )	return;
    //- Correct request to archive border -
    tTime     = vmin(tTime,arhEnd);
    tTimeGrnd = vmax(tTimeGrnd,arhBeg); 
    //- Clear data at time error -
    unsigned int valEnd = (tw->rowCount() && tw->columnCount()) ? tw->item(0,0)->data(Qt::UserRole).toUInt() : 0;
    unsigned int valBeg = (tw->rowCount() && tw->columnCount()) ? tw->item(tw->rowCount()-1,0)->data(Qt::UserRole).toUInt() : 0;    
    if( tTime <= tTimeGrnd || (tTime < valEnd && tTimeGrnd > valBeg) )
    {
        tw->setRowCount(0);
	valEnd = valBeg = 0;
	return;
    }
    //- Correct request to present data -
    bool toUp = false;
    if( valEnd && tTime > valEnd )	{ tTimeGrnd = valEnd; toUp = true; }
    else if( valBeg && tTimeGrnd < valBeg )	tTime = valBeg-1; 
    //printf("TEST 00: %d - %d\n",tTimeGrnd,tTime);
    //- Get values data -
    unsigned int rtm;			//Record's data
    QDateTime    dtm;
    QString   rlev, rcat, rmess;	//Record's level category and message
    
    XMLNode req("get");
    req.clear()->
	setAttr("arch",arch)->
    	setAttr("path","/Archive/%2fserv%2f0")->
	setAttr("tm",TSYS::ll2str(tTime))->
	setAttr("tm_grnd",TSYS::ll2str(tTimeGrnd))->
	setAttr("cat",w->dc()["tmpl"].toString().toAscii().data())->
	setAttr("lev",TSYS::uint2str(w->dc()["lev"].toInt()));
    if( w->cntrIfCmd(req,true) )	return;
    //int row = toUp ? 0 : tw->rowCount();
    bool newFill = (tw->rowCount()==0);
    
    //- Get collumns indexes -
    int c_tm   = imp->value("tm",-1),
	c_lev  = imp->value("lev",-1),
	c_cat  = imp->value("cat",-1),
	c_mess = imp->value("mess",-1);    

    QTableWidgetItem *tit;
    //- Process records -
    if( toUp )
	for( int i_req = 0; i_req < req.childSize(); i_req++ )
	{
	    XMLNode *rcd = req.childGet(i_req);
	    
	    //-- Get parameters --
	    rtm  = strtoul(rcd->attr("time").c_str(),0,10);
	    rlev = rcd->attr("lev").c_str();
	    rcat = rcd->attr("cat").c_str();
	    rmess = rcd->text().c_str();

	    //-- Check for dublicates --
	    //--- Check for last message dublicate and like time messages ---
	    bool is_dbl = false;
	    for( int i_c = 0, i_p = 0; i_p < tw->rowCount(); i_p++, i_c++ )
	    {
		if( rtm > tw->item(0,0)->data(Qt::UserRole).toUInt() && i_c )	continue;
		if( (c_lev<0 || tw->item(i_p,c_lev)->text() == rlev) &&
		    (c_cat<0 || tw->item(i_p,c_cat)->text() == rcat)  &&
		    (c_mess<0 || tw->item(i_p,c_mess)->text() == rmess ) )
		{ 
		    is_dbl = true;
		    break;
		}	    
	    }
	    if( is_dbl ) continue;	
	    //--- Insert new row ---
	    tw->insertRow(0);
	    if( c_tm >= 0 )
	    {
		dtm.setTime_t(rtm);
		tw->setItem( 0, c_tm, tit=new QTableWidgetItem(dtm.toString(Qt::ISODate)) );
		tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	    }
	    if( c_lev >= 0 )	{ tw->setItem( 0, c_lev, tit=new QTableWidgetItem(rlev) ); tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable); }
	    if( c_cat >= 0 )	{ tw->setItem( 0, c_cat, tit=new QTableWidgetItem(rcat) ); tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable); }
	    if( c_mess >= 0 )	{ tw->setItem( 0, c_mess, tit=new QTableWidgetItem(rmess) ); tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable); }
	    tw->item(0,0)->setData(Qt::UserRole,rtm);
	}
    else    
	for( int i_req = req.childSize()-1; i_req >= 0; i_req-- )
	{
	    XMLNode *rcd = req.childGet(i_req);
	    //-- Get parameters --
	    rtm  = strtoul(rcd->attr("time").c_str(),0,10);
	    rlev = rcd->attr("lev").c_str();
	    rcat = rcd->attr("cat").c_str();
	    rmess = rcd->text().c_str();

	    //-- Check for dublicates --
	    //--- Check for last message dublicate and like time messages ---
	    bool is_dbl = false;
	    for( int i_c = 0, i_p = tw->rowCount()-1; i_p >= 0; i_p--, i_c++ )
	    {
		if( rtm < tw->item(0,0)->data(Qt::UserRole).toUInt() && i_c )	continue;
		if( (c_lev<0 || tw->item(i_p,c_lev)->text() == rlev) &&
		    (c_cat<0 || tw->item(i_p,c_cat)->text() == rcat)  &&
		    (c_mess<0 || tw->item(i_p,c_mess)->text() == rmess ) )
		{ 
		    is_dbl = true;
		    break;
		}	    	
	    }
	    if( is_dbl ) continue;
	
	    //--- Insert new row ---
	    int row = tw->rowCount();
	    tw->insertRow(row);
	    if( c_tm >= 0 )
	    {
		dtm.setTime_t(rtm);
		tw->setItem( row, c_tm, tit=new QTableWidgetItem(dtm.toString(Qt::ISODate)) );
		tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	    }
	    if( c_lev >= 0 )	{ tw->setItem( row, c_lev, tit=new QTableWidgetItem(rlev) ); tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable); }
	    if( c_cat >= 0 )	{ tw->setItem( row, c_cat, tit=new QTableWidgetItem(rcat) ); tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable); }
	    if( c_mess >= 0 )	{ tw->setItem( row, c_mess, tit=new QTableWidgetItem(rmess) ); tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable); }
	    tw->item(row,0)->setData(Qt::UserRole,rtm);
	}
    if( newFill )
    {
	tw->resizeColumnsToContents();
        //Resize too long columns
        int max_col_sz = vmax(w->size().width()/tw->columnCount(),40);
        for( int i_c = 0; i_c < tw->columnCount(); i_c++ )
            tw->setColumnWidth(i_c,vmin(max_col_sz,tw->columnWidth(i_c)));
    }
}

void ShapeProtocol::tracing( )
{
    WdgView *w = (WdgView *)((QTimer*)sender())->parent();
    if( !w->isEnabled() ) return;
    
    unsigned int tm     = w->dc()["time"].toUInt();
    unsigned int trcPer = w->dc()["trcPer"].toUInt();
    if( w->dc()["timeCurent"].toBool() ) w->dc()["time"] = (unsigned int)time(NULL);
    else if( tm )	w->dc()["time"] = tm+trcPer;
    loadData(w);
} 

bool ShapeProtocol::event( WdgView *w, QEvent *event )
{
    return false;
}
 
bool ShapeProtocol::eventFilter( WdgView *view, QObject *object, QEvent *event )
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
 
void ShapeProtocol::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if( en ) 	wdg->installEventFilter(view);
    else 	wdg->removeEventFilter(view);
    //- Process childs -
    for( int i_c = 0; i_c < wdg->children().size(); i_c++ )
	if( qobject_cast<QWidget*>(wdg->children().at(i_c)) )
	    eventFilterSet(view,(QWidget*)wdg->children().at(i_c),en);
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
    w->dc()["inclWidget"].setValue((void*)NULL);
    w->dc()["border"].setValue((void*)new QPen());
}

void ShapeBox::destroy( WdgView *w )
{
    delete (QPen*)w->dc()["border"].value<void*>();
}

bool ShapeBox::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    bool up = true;

    switch(uiPrmPos)
    {
	case -1:	//load
	    up = true;
	    break;
	case 12:	//geomMargin
	    w->dc()["geomMargin"] = atoi(val.c_str());
	    if( w->layout() ) w->layout()->setMargin( w->dc()["geomMargin"].toInt() );
	    break;
	case 20: 	//backColor
	    w->dc()["backColor"] = QColor(val.c_str()); break;
	case 21: 	//backImg
	{
	    XMLNode req("get");
    	    req.setAttr("path",w->id()+"/%2fwdg%2fres")->
		setAttr("id",val);
    	    if( !w->cntrIfCmd(req) )
    	    {
	        QBrush brsh;
		string backimg = TSYS::strDecode(req.text(),TSYS::base64);
		if( !backimg.empty() )
		{
		    QImage img;
		    if(img.loadFromData((const uchar*)backimg.c_str(),backimg.size()))	
			brsh.setTextureImage(img);
		}
		w->dc()["backImg"] = brsh;
    	    } else up = false;
	    break;
	}
	case 22:	//bordWidth
	    ((QPen*)w->dc()["border"].value<void*>())->setWidth(atoi(val.c_str()));	break;
	case 23:	//bordColor
	    ((QPen*)w->dc()["border"].value<void*>())->setColor(QColor(val.c_str()));	break;
	case 26:	//pgOpenSrc
	{
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    w->dc()["pgOpenSrc"] = val.c_str();
		
	    RunPageView *el_wdg = (RunPageView *)w->dc()["inclWidget"].value<void*>();
	    //-- Put previous include widget to page cache --
	    if( !el_wdg || val != el_wdg->id() )
	    {		
		if( el_wdg ) 
		{ 
		    ((RunPageView*)w)->mainWin()->pgCacheAdd(el_wdg);
		    el_wdg->setEnabled(false);
		    el_wdg->setVisible(false);
		    el_wdg->setParent(NULL);
		    el_wdg = NULL; 
		}
		//-- Create new include widget --	
		if( val.size() )
		{
		    QVBoxLayout *lay = (QVBoxLayout *)w->layout();
		    if( !lay ) lay = new QVBoxLayout(w);
		
		    el_wdg = (RunPageView *)(((RunWdgView*)w)->mainWin()->pgCacheGet(val));
		    if( el_wdg )
		    {
			el_wdg->setParent(w);
			el_wdg->setEnabled(true);
			el_wdg->setVisible(true);
		    }
		    else 
		    {
		        el_wdg = new RunPageView(val,(VisRun*)w->mainWin(),w);
			el_wdg->load("");
		    }
		    //el_wdg->resize(w->size());
		    lay->addWidget(el_wdg);
		    lay->setMargin(w->dc()["geomMargin"].toInt());
		}
		w->dc()["inclWidget"].setValue((void*)el_wdg);
	    } else up = false;
	    break;
	}
	case 27:	//pgGrp
	    if( !qobject_cast<RunWdgView*>(w) )	{ up = false; break; }
	    w->dc()["pgGrp"] = val.c_str(); 
	    break;
	case 28:	//pgFullScr
	    w->dc()["pgFullScr"] = val.c_str();
	    up = false;
	    break;
	default: up = false;
    }
    
    if( up && !w->allAttrLoad( ) ) w->update();
    
    return up;
}

bool ShapeBox::event( WdgView *w, QEvent *event )
{
    switch(event->type())
    {
        case QEvent::Paint:
        {
	    if( w->dc()["inclWidget"].value<void*>() ) return false;
    	    QPainter pnt( w );

	    //- Apply margin -
	    int margin = w->dc()["geomMargin"].toInt();
	    QRect dA = w->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));

	    //- Draw background -
	    QColor bkcol = w->dc()["backColor"].value<QColor>();
	    if(  bkcol.isValid() ) pnt.fillRect(dA,bkcol);
	    QBrush bkbrsh = w->dc()["backImg"].value<QBrush>();
	    if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(dA,bkbrsh);
	    
	    //- Draw border -
	    QPen *bpen = (QPen*)w->dc()["border"].value<void*>();
	    if( bpen->width() )
	    {
		pnt.setPen(*bpen);
		pnt.drawRect(dA.adjusted(bpen->width()/2,bpen->width()/2,
					-bpen->width()/2-bpen->width()%2,-bpen->width()/2-bpen->width()%2));
	    }
            //- Draw focused border -
	    if( w->hasFocus() ) qDrawShadeRect(&pnt,dA.x(),dA.y(),dA.width(),dA.height(),w->palette());

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

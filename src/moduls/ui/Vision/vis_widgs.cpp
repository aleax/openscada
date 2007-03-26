
//OpenSCADA system module UI.Vision file: vis_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QEvent>
#include <QPainter>
#include <QDialogButtonBox>
#include <QApplication>
#include <QPaintEvent>

#include <tsys.h>

#include "vis_shapes.h"
#include "tvision.h"
#include "vis_widgs.h"

using namespace VISION;

//*************************************************
//* Id and name input dialog                      *
//*************************************************
InputDlg::InputDlg( QWidget *parent, const QIcon &icon, const QString &mess, 
	const QString &ndlg, bool with_id, bool with_nm ) :
		QDialog(parent), m_id(NULL), m_name(NULL)
{
    setWindowTitle(ndlg);
    setMinimumSize( QSize( 120, 150 ) );
    setWindowIcon(icon);
    setSizeGripEnabled(true);
    
    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);
    
    //Icon label and text message
    QHBoxLayout *intr_lay = new QHBoxLayout;
    intr_lay->setSpacing(6);    
    
    QLabel *icon_lab = new QLabel(this);
    icon_lab->setSizePolicy( QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum) );
    icon_lab->setPixmap(icon.pixmap(48));
    intr_lay->addWidget(icon_lab);
    
    QLabel *inp_lab = new QLabel(mess,this);
    //inp_lab->setAlignment(Qt::AlignHCenter);
    inp_lab->setWordWrap(true);
    intr_lay->addWidget(inp_lab);
    dlg_lay->addItem(intr_lay);
    
    //Id and name fields
    if( with_nm || with_id )
    {
	QGridLayout *ed_lay = new QGridLayout;
	ed_lay->setSpacing(6);
	if( with_id ) 
	{
	    ed_lay->addWidget( new QLabel(_("ID:"),this), 0, 0 );
	    m_id = new QLineEdit(this);
	    ed_lay->addWidget( m_id, 0, 1 );
	}	    
	if( with_nm )
	{
	    ed_lay->addWidget( new QLabel(_("Name:"),this), 1, 0 );
	    m_name = new QLineEdit(this);
	    ed_lay->addWidget( m_name, 1, 1 );
	}
	dlg_lay->addItem(ed_lay);	
    }
    
    //Qk and Cancel buttons
    dlg_lay->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );
    
    QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|
                                    		     QDialogButtonBox::Cancel,Qt::Horizontal,this);
    QImage ico_t;
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box );
    
    resize(250,120+(40*with_nm)+(40*with_id));
}

QString InputDlg::id()
{
    if( m_id )	return m_id->text();
    return "";
}

QString InputDlg::name()
{
    if( m_name )return m_name->text();
    return "";
}

void InputDlg::setId(const QString &val)
{
    if( m_id )	m_id->setText(val);
}

void InputDlg::setName(const QString &val)
{
    if( m_name )m_name->setText(val);
}

//*************************************************
//* User select dialog                            *
//*************************************************
DlgUser::DlgUser( )
{
    setWindowTitle(_("Select user"));
    
    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QGridLayout *ed_lay = new QGridLayout;
    ed_lay->setSpacing(6);    
    ed_lay->addWidget( new QLabel(_("User:"),this), 0, 0 );    
    users = new QComboBox(this);
    ed_lay->addWidget( users, 0, 1 );
    ed_lay->addWidget( new QLabel(_("Password:"),this), 1, 0 );    
    passwd = new QLineEdit(this);
    passwd->setEchoMode( QLineEdit::Password );
    ed_lay->addWidget( passwd, 1, 1 );
    dlg_lay->addItem(ed_lay);            

    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );

    QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|
                                    		     QDialogButtonBox::Cancel,Qt::Horizontal,this);
    QImage ico_t;    
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box );
    
    connect(this, SIGNAL(finished(int)), this, SLOT(finish(int)));
    
    //Fill users list
    vector<string> u_list;	
    SYS->security().at().usrList(u_list);
    for(int i_l = 0; i_l < u_list.size(); i_l++ )
    {
        string simg = TSYS::strDecode(SYS->security().at().usrAt(u_list[i_l]).at().picture(),TSYS::base64);
        QImage img;
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
	    users->addItem(QPixmap::fromImage(img),u_list[i_l].c_str());
	else users->addItem(u_list[i_l].c_str());
    }
}

QString DlgUser::user()
{
    return users->currentText();
}

QString DlgUser::password()
{
    return passwd->text();
}

void DlgUser::finish( int result )
{
    if( result )
    {
	//Check user
	if( SYS->security().at().usrPresent(user().toAscii().data()) && 
		SYS->security().at().usrAt(user().toAscii().data()).at().auth(password().toAscii().data()) )
	    setResult(SelOK);
	else setResult(SelErr);
    }
    else setResult(SelCancel);
}

//*********************************************
//* Status bar user widget                    *
//*********************************************
UserStBar::UserStBar( const QString &iuser, QWidget * parent ) : QLabel(parent)
{
    setUser(iuser);
}

QString UserStBar::user()
{
    return user_txt;
}

void UserStBar::setUser( const QString &val )
{
    setText(QString("<font color='%1'>%2</font>").arg((val=="root")?"red":"green").arg(val));
    user_txt = val;
}

bool UserStBar::event( QEvent *event )
{
    if( event->type() == QEvent::MouseButtonDblClick )	userSel();
    return QLabel::event( event );
}

bool UserStBar::userSel()
{
    DlgUser d_usr;
    int rez = d_usr.exec();
    if( rez == DlgUser::SelOK && d_usr.user() != user() )
    {
        setUser( d_usr.user() );
	emit userChanged();
	return true;
    }
    else if( rez == DlgUser::SelErr )
	mod->postMess(mod->nodePath().c_str(),_("Auth wrong!!!"),TVision::Warning,this);
    
    return false;
}

//****************************************
//* Shape widget view                    *
//****************************************
WdgView::WdgView( const string &iwid, int ilevel, QWidget* parent ) :
    QWidget(parent), idWidget(iwid), shape(NULL), selWidget(false), moveHold(false), w_level(ilevel)
{
    setMouseTracking(true);
    if( wLevel() == 0 )	
    {
	//setAttribute(Qt::WA_StaticContents);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);
	setFocusPolicy(Qt::StrongFocus);
	setCursor(Qt::CrossCursor);
    }

    string wlib_id = TSYS::strSepParse(iwid,0,'.');
    string wdg_id = TSYS::strSepParse(iwid,1,'.');
    string wdgc_id = TSYS::strSepParse(iwid,2,'.');

    try
    {			
	//- Connect to widget -
	if( wdgc_id.size() )
    	    wdgLnk = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
	else if( wdg_id.size() )
    	    wdgLnk = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
	if( !wdgLnk.freeStat() )
	{
            //- Init childs -
            vector<string> lst;
	    string b_nm = iwid;
	    while( TSYS::strSepParse(b_nm,2,'.').size() )
		b_nm = mod->engine().at().wlbAt(TSYS::strSepParse(b_nm,0,'.')).at().
				          at(TSYS::strSepParse(b_nm,1,'.')).at().
					  wdgAt(TSYS::strSepParse(b_nm,2,'.')).at().parentNm();
		
	    mod->engine().at().wlbAt(TSYS::strSepParse(b_nm,0,'.')).at().
			       at(TSYS::strSepParse(b_nm,1,'.')).at().wdgList(lst);		
	    
            for( int i_l = 0; i_l < lst.size(); i_l++ )
                new WdgView( b_nm+"."+lst[i_l], wLevel()+1, this );
            //- Init shape -
	    shape = mod->getWdgShape(wdgLnk.at().rootId());
	    shape->init(this);
            //- Load widget's data from data model -
	    loadData(id());
	}
    }catch(...)	{ }
}

WdgView::~WdgView( )
{

}

void WdgView::setSelect( bool vl )
{
    selWidget = vl;
    
    if( vl )
    {
	string sel_chlds;
	for( int i_c = 0; i_c < children().size(); i_c++ )
        {
            WdgView *curw = qobject_cast<WdgView*>(children().at(i_c));
            if( curw->select() ) sel_chlds=sel_chlds+curw->id()+";";
        }
	if( sel_chlds.size() )	emit selected(sel_chlds);
	else			emit selected(id());
    }
    else
    {
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    qobject_cast<WdgView*>(children().at(i_c))->setSelect(false);
	emit selected("");
    }
    update();
}

void WdgView::loadData( const string& item )
{
    if( item.empty() || item == id() )
    {
	move(wdg().at().attrAt("geomX").at().getI(),wdg().at().attrAt("geomY").at().getI());
	resize(wdg().at().attrAt("geomW").at().getI(), wdg().at().attrAt("geomH").at().getI());

	if( shape ) shape->loadData( this );

	//- Update view -
    	update();
    }
    
    if( item != id() && wLevel() == 0 )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    qobject_cast<WdgView*>(children().at(i_c))->loadData(item);
}

void WdgView::saveData( const string& item )
{
    if( item.empty() || item == id() )
    {
	wdg().at().attrAt("geomX").at().setI(pos().x());
	wdg().at().attrAt("geomY").at().setI(pos().y());    
	wdg().at().attrAt("geomW").at().setI(size().width());
	wdg().at().attrAt("geomH").at().setI(size().height());

	if( shape ) shape->saveData( this );
    }
    if( item != id() && wLevel() == 0 )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    qobject_cast<WdgView*>(children().at(i_c))->saveData(item);	

    //- For top items (like inspector) data update -
    if( wLevel() == 0 )	setSelect(true);
}

bool WdgView::grepAnchor( const QPoint &apnt, const QPoint &cpnt )
{
    if( (cpnt.x() > apnt.x()-4) && (cpnt.x() < apnt.x()+4) &&
	    (cpnt.y() > apnt.y()-4) && (cpnt.y() < apnt.y()+4) )
	return true;
    else return false;
}

void WdgView::upMouseCursors( const QPoint &curp )
{
    if( !moveHold )
    {
	Qt::CursorShape new_shp = Qt::CrossCursor;	    		
	//- Check child's anchor selection and widget's geometry -
	leftTop  = false;
	selRect = QRect();
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<WdgView*>(children().at(i_c))->select( ) )
		selRect = selRect.united(qobject_cast<WdgView*>(children().at(i_c))->geometry());
	//- Select childs anchors -
	if( !selRect.isNull() )
	{
	    if( grepAnchor(selRect.topLeft(),curp) )
	    { new_shp = Qt::SizeFDiagCursor; leftTop = true; }
	    else if( grepAnchor(selRect.bottomRight(),curp) )
		new_shp = Qt::SizeFDiagCursor;
	    else if( grepAnchor(selRect.bottomLeft(),curp) )
	    { new_shp = Qt::SizeBDiagCursor; leftTop = true; }
	    else if( grepAnchor(selRect.topRight(),curp) )
		new_shp = Qt::SizeBDiagCursor;
	    else if( grepAnchor(QPoint(selRect.center().x(),selRect.y()),curp) )
	    { new_shp = Qt::SizeVerCursor; leftTop = true; }
	    else if( grepAnchor(QPoint(selRect.center().x(),selRect.bottomRight().y()),curp) )
		new_shp = Qt::SizeVerCursor;
	    else if( grepAnchor(QPoint(selRect.x(),selRect.center().y()),curp) )
	    { new_shp = Qt::SizeHorCursor; leftTop = true; }
	    else if( grepAnchor(QPoint(selRect.bottomRight().x(),selRect.center().y()),curp) )
		new_shp = Qt::SizeHorCursor;
	    else if( selRect.contains(curp) )
	        new_shp = Qt::PointingHandCursor;
	    if( new_shp != Qt::CrossCursor ) hold_child = true;
	}
	//- Widget geometry -
	if( new_shp == Qt::CrossCursor )
	{
	    if( grepAnchor(rect().bottomRight(),curp) )
		new_shp = Qt::SizeFDiagCursor;		
	    else if( curp.x()>(rect().width()-4) && curp.x()<(rect().width()+4) )
	        new_shp = Qt::SizeHorCursor;
	    else if( curp.y()>(rect().height()-4) && curp.y()<(rect().height()+4) )
		new_shp = Qt::SizeVerCursor;
	    hold_child = false;
	}
	if( new_shp != cursor().shape() ) setCursor(new_shp);
    }
}

bool WdgView::event( QEvent *event )
{
    //const QEvent::Type MyEvent = (QEvent::Type)1234;
    //printf("TEST 00: Event %d \n",event->type());
    switch(event->type())
    {   
	/*case MyEvent:
	{
	    if( wLevel() != 0 )	return true;
	    printf("TEST 00: Event %d \n",event->type());
	    
	    QPainter pnt(this);
	    pnt.setWindow( rect() );
	    
	    QRect rsel;
	    for( int i_c = 0; i_c < children().size(); i_c++ )
		if( qobject_cast<WdgView*>(children().at(i_c))->select( ) )		    
		    rsel = rsel.united(qobject_cast<WdgView*>(children().at(i_c))->geometry());
	    if( !rsel.isNull() )
	    {
		//- Draw select board -
		pnt.setPen("black");
		pnt.setBrush(QBrush(QColor("lightgreen")));
		for(int i_p = 0; i_p < 9; i_p++)
		{
		    if( i_p == 4 ) continue;
		    QRect anch((rsel.x()-3)+(i_p%3)*(rsel.width()/2),
			   (rsel.y()-3)+(i_p/3)*(rsel.height()/2),6,6);
		    pnt.drawRect(anch);
		}
	    }
	    return true;
	}*/
	case QEvent::Paint:
	{
	    QPainter pnt( this );
	    pnt.setWindow( rect() );
	    
	    //printf("TEST 10: %d %s\n",wLevel(),id().c_str());
	    
	    //- Draw background for root widget -
	    if( wLevel() == 0 )
	    {
		pnt.setPen("black");
		pnt.setBrush(QBrush(QColor("white")));
                pnt.drawRect(rect().adjusted(0,0,-1,-1));
	    }
	    
	    //- Check widget -
	    if( wdg().freeStat() || !shape )
	    {
		pnt.drawImage(rect(),QImage(":/images/attention.png"));
		if(wdg().freeStat())
		    setToolTip(QString(_("Widget '%1' no allow!")).arg(id().c_str()));
		else if( !shape )
		    setToolTip(QString(_("Widget shape '%1' no support!")).arg(wdg().at().rootId().c_str()));
		//event->accept();
		return true;
	    }
	    //- Self widget view -
	    shape->event(this,event);
	    
	    if( wLevel() == 0 )
	    {	
		//const QEvent::Type MyEvent = (QEvent::Type)1234;
		//QApplication::postEvent(this,new QEvent(MyEvent));
		//printf("TEST 01: %d %s\n",wLevel(),id().c_str());
	    
		QRect rsel;
		for( int i_c = 0; i_c < children().size(); i_c++ )
		    if( qobject_cast<WdgView*>(children().at(i_c))->select( ) )		    
			rsel = rsel.united(qobject_cast<WdgView*>(children().at(i_c))->geometry());
		if( !rsel.isNull() )
		{
		    //- Draw select board -
		    pnt.setPen("black");
		    pnt.setBrush(QBrush(QColor("lightgreen")));
		    for(int i_p = 0; i_p < 9; i_p++)
		    {
			if( i_p == 4 ) continue;
			QRect anch((rsel.x()-3)+(i_p%3)*(rsel.width()/2),
				   (rsel.y()-3)+(i_p/3)*(rsel.height()/2),6,6);
			pnt.drawRect(anch);
		    }
		}
		event->accept();
		return true;
	    }
	    break;
	}
	case QEvent::MouseButtonPress:
	    //- Select widget -
	    if( wLevel() == 0 )
	    {
		QPoint curp = mapFromGlobal(cursor().pos());		    
		if( cursor().shape() == Qt::CrossCursor )
		{
		    //-- Scan childs --
		    bool sel_modif = false;
		    bool chld_sel = false;
		    WdgView *cwdg = NULL;
		    for( int i_c = children().size()-1; i_c >= 0; i_c-- )
		    {
			cwdg = qobject_cast<WdgView*>(children().at(i_c));
			if( cwdg->geometry().contains(curp) ) 
			{
			    if( !cwdg->select() )	{ cwdg->setSelect(true); sel_modif = true; }
			    chld_sel = true;
			    break;
			}
		    }
		    //-- Select clean for childs --
		    if( !(QApplication::keyboardModifiers()&Qt::ShiftModifier) )
			for( int i_c = 0; i_c < children().size(); i_c++ )
			{
			    WdgView *curw = qobject_cast<WdgView*>(children().at(i_c));
			    if( chld_sel && (curw == cwdg) )	continue;
			    if( curw->select() )	{ curw->setSelect(false); sel_modif = true; }
	    		}
		    if( sel_modif || !select() ) setSelect(true);
		    event->accept();
		    
		    upMouseCursors(mapFromGlobal(cursor().pos()));
		}
		if( cursor().shape() != Qt::CrossCursor )
		{
		    moveHold = true;
		    hold_pnt = curp;
		}
		return true;
	    }
	    break;
	case QEvent::MouseButtonRelease:
	    if( moveHold )
	    {
		moveHold = false;
		if( cursor().shape() != Qt::CrossCursor )
		    saveData("");
	    }
	    break;
	case QEvent::FocusOut:	
	    if( cursor().shape() != Qt::CrossCursor )
		setCursor(Qt::CrossCursor);	    
	    setSelect(false);
	    return true;
	case QEvent::MouseMove:
	    if( wLevel() == 0 )
	    {
		QPoint curp = mapFromGlobal(cursor().pos());
		
		upMouseCursors(curp);
		
		if( moveHold && cursor().shape() != Qt::CrossCursor )
		{
		    if( hold_child )
		    {
			QPoint dP = curp-hold_pnt;
			//-- Update selected widgets geometry --
			for( int i_c = 0; i_c < children().size(); i_c++ )
			{
			    WdgView *curw = qobject_cast<WdgView*>(children().at(i_c));
			    if( !curw->select() ) continue;			    
			    QRect  geom = curw->geometry();
			    switch(cursor().shape())
			    {
				case Qt::SizeFDiagCursor:
				    if( leftTop )
				    {
					curw->move(geom.x()+dP.x(), geom.y()+dP.y());
					curw->resize(geom.width()-dP.x(), geom.height()-dP.y());
				    }
				    else curw->resize(geom.width()+dP.x(), geom.height()+dP.y());
				    break;
				case Qt::SizeBDiagCursor:
				    if( leftTop )
					curw->setGeometry(geom.x()+dP.x(),geom.y(),
							  geom.width()-dP.x(), geom.height()+dP.y());
				    else
					curw->setGeometry(geom.x(),geom.y()+dP.y(),
					                  geom.width()+dP.x(), geom.height()-dP.y());
				    break;
				case Qt::SizeVerCursor:
				    if( leftTop )
					curw->setGeometry(geom.x(),geom.y()+dP.y(),
							  geom.width(), geom.height()-dP.y());
				    else curw->resize(geom.width(), geom.height()+dP.y());				    
				    break;				    
				case Qt::SizeHorCursor:
				    if( leftTop )
					curw->setGeometry(geom.x()+dP.x(),geom.y(),
							  geom.width()-dP.x(), geom.height());
				    else curw->resize(geom.width()+dP.x(), geom.height());
				    break;
				case Qt::PointingHandCursor:
				    curw->move(curw->pos()+dP);
				    break;
			    }			    
			}			
			hold_pnt = curp;
			update();
		    }
		    else
			//- Change widget geometry -
			switch(cursor().shape())
			{
		    	    case Qt::SizeHorCursor:
				resize(curp.x(),size().height());
				break;
			    case Qt::SizeVerCursor:
				resize(size().width(),curp.y());
			        break;
			    case Qt::SizeFDiagCursor:
				resize(curp.x(),curp.y());
				break;	    
			}
		}
		event->accept();
        	return true;				    
	    }
    }
    return QWidget::event(event);
}



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
	mod->postMess(mod->nodePath().c_str(),_("Auth wrong!!!"),TVision::Warning);
    
    return false;
}

//****************************************
//* Shape widget view                          *
//****************************************
WdgView::WdgView( const string &iwid, QWidget* parent ) :
    QWidget(parent), idWidget(iwid), shape(NULL), selWidget(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString(_("Widget: %1")).arg(idWidget.c_str()));
    
    string wlib_id = TSYS::strSepParse(iwid,0,'.');
    string wdg_id = TSYS::strSepParse(iwid,1,'.');
    string wdgc_id = TSYS::strSepParse(iwid,2,'.');
			
    //- Connect to widget -
    if( wdgc_id.size() )
        wdgLnk = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
    else if( wdg_id.size() )
        wdgLnk = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
    if( !wdgLnk.freeStat() )
    {	
	shape = mod->getWdgShape(wdgLnk.at().rootId());
	resize(wdgLnk.at().attrAt("geomW").at().getI(),wdgLnk.at().attrAt("geomH").at().getI());
    }
}
	    
WdgView::~WdgView( )
{
	    
}

bool WdgView::event( QEvent *event )
{
    //printf("TEST 00: Event %d \n",event->type());
    switch(event->type())
    {    
	case QEvent::Paint:
	    if( wdgLnk.freeStat() || !shape )
	    {
		QPainter pnt( this );
		pnt.setWindow(0,0,200,40);
		pnt.setPen(QColor(255,0,0));
		pnt.setBackground(QBrush(QColor(210,237,234)));
		pnt.drawRect(0,0,199,39);
		QString text;		
		if(wdgLnk.freeStat())
		    text = QString(_("Widget '%1'\nno allow!")).arg(id().c_str());
		else if( !shape )
		    text = QString(_("Widget shape\n'%1' no support!")).arg(wdgLnk.at().rootId().c_str());
		pnt.drawText(3,3,194,34,Qt::AlignCenter,text);
		event->accept();
		return true;
	    }
	    return shape->event(this,event);
    }
    return false;// QWidget::event(event);
}

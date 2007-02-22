
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

#include "xpm/button_ok.xpm"
#include "xpm/button_cancel.xpm"

#include <tsys.h>

#include "tvision.h"
#include "vis_widgs.h"

using namespace VISION;

//*************************************************
//* Id and name input dialog                      *
//*************************************************
InputDlg::InputDlg( const QIcon &icon, const QString &mess, const QString &ndlg, bool with_id, bool with_nm ) :
    m_id(NULL), m_name(NULL)
{
    setWindowTitle(ndlg);
    setMinimumSize( QSize( 180, 120 ) );
    setWindowIcon(icon);
    
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
    
    QLabel *inp_lab = new QLabel(QString("<b>%1</b>").arg(mess),this);
    inp_lab->setAlignment(Qt::AlignHCenter);
    inp_lab->setWordWrap(true);
    intr_lay->addWidget(inp_lab);
    dlg_lay->addItem(intr_lay);
    
    //Id and name fields
    if( with_nm || with_id )
    {
	QHBoxLayout *req_lay = new QHBoxLayout;
	req_lay->setSpacing(6);
    
	QVBoxLayout *lab_lay = new QVBoxLayout;
	lab_lay->setSpacing(6);
	if( with_id ) lab_lay->addWidget( new QLabel(_("ID:"),this) );
	if( with_nm ) lab_lay->addWidget( new QLabel(_("Name:"),this) );

	QVBoxLayout *el_lay = new QVBoxLayout;
	el_lay->setSpacing(6);
	if( with_id ) 
	{ 
	    m_id = new QLineEdit(this);
	    el_lay->addWidget( m_id );
	}
	if( with_nm )
	{
	    m_name = new QLineEdit(this);
	    el_lay->addWidget( m_name );
        }
    
	req_lay->addItem( lab_lay );
	req_lay->addItem( el_lay );    

	dlg_lay->addItem(req_lay);
    }
    
    //Qk and Cancel buttons
    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );

    dlg_lay->addWidget( sep );
    
    QHBoxLayout *butt_lay = new QHBoxLayout;
    butt_lay->setSpacing(6);
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
 
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t = QImage(button_ok_xpm);
    QPushButton *butt_ok = new QPushButton( QPixmap::fromImage(ico_t), _("OK"), this );
    connect(butt_ok, SIGNAL(clicked()), this, SLOT(accept()));
    butt_lay->addWidget(butt_ok);    

    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t = QImage(button_cancel_xpm);
    QPushButton *butt_cancel = new QPushButton( QPixmap::fromImage(ico_t), _("Cancel"), this );
    connect(butt_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    butt_lay->addWidget(butt_cancel);
    
    dlg_lay->addItem( butt_lay );
    
    //resize(250,200);
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
	    
void InputDlg::id(const QString &val)
{
    if( m_id )	m_id->setText(val);
}

void InputDlg::name(const QString &val)
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

    QHBoxLayout *req_lay = new QHBoxLayout;
    req_lay->setSpacing(6);
    QVBoxLayout *lab_lay = new QVBoxLayout;
    lab_lay->setSpacing(6);
    lab_lay->addWidget( new QLabel(_("User:"),this) );
    lab_lay->addWidget( new QLabel(_("Password:"),this) );

    QVBoxLayout *el_lay = new QVBoxLayout;
    el_lay->setSpacing(6);
    
    users = new QComboBox(this);
    passwd = new QLineEdit(this);
    passwd->setEchoMode( QLineEdit::Password );
    el_lay->addWidget( users );
    el_lay->addWidget( passwd );
    
    req_lay->addItem( lab_lay );
    req_lay->addItem( el_lay );    

    dlg_lay->addItem(req_lay);
    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );
    
    QHBoxLayout *butt_lay = new QHBoxLayout;
    butt_lay->setSpacing(6);
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));    
    
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t = QImage(button_ok_xpm);
    QPushButton *butt_ok = new QPushButton( QPixmap::fromImage(ico_t), _("OK"), this );
    connect(butt_ok, SIGNAL(clicked()), this, SLOT(accept()));    
    butt_lay->addWidget(butt_ok);    
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
 
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t = QImage(button_cancel_xpm);
    QPushButton *butt_cancel = new QPushButton( QPixmap::fromImage(ico_t), _("Cancel"), this );	
    connect(butt_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    butt_lay->addWidget(butt_cancel);
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));    
    
    dlg_lay->addItem( butt_lay );
    
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
    user(iuser);
}
					
QString UserStBar::user()
{
    return user_txt;
}

void UserStBar::user( const QString &val )
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
        user( d_usr.user() );
	emit userChanged();
	return true;
    }
    else if( rez == DlgUser::SelErr )
	mod->postMess(mod->nodePath().c_str(),_("Auth wrong!!!"),TVision::Warning);
    
    return false;
}

/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qframe.h> 
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qimage.h>

#include "xpm/button_ok.xpm"
#include "xpm/button_cancel.xpm"

#include "tuimod.h"
#include "dlguser.h"

using namespace QTCFG;

DlgUser::DlgUser( TUIMod *module ) : m_mod(module)
{
    setCaption(m_mod->I18N("Select user"));
    
    QVBoxLayout *dlg_lay = new QVBoxLayout( this, 10 );

    QHBoxLayout *req_lay = new QHBoxLayout( 5 );
    
    QVBoxLayout *lab_lay = new QVBoxLayout( 5 );
    lab_lay->addWidget( new QLabel(m_mod->I18N("User:"),this) );
    lab_lay->addWidget( new QLabel(m_mod->I18N("Password:"),this) );

    QVBoxLayout *el_lay = new QVBoxLayout( 5 );
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
    
    QHBoxLayout *butt_lay = new QHBoxLayout( );
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
    QPushButton *butt_ok = new QPushButton( QPixmap(QImage(button_ok_xpm)), m_mod->I18N("OK"), this );
    connect(butt_ok, SIGNAL(clicked()), this, SLOT(accept()));    
    butt_lay->addWidget(butt_ok);    
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));    
    QPushButton *butt_cancel = new QPushButton( QPixmap(QImage(button_cancel_xpm)), m_mod->I18N("Cancel"), this );	
    connect(butt_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    butt_lay->addWidget(butt_cancel);
    butt_lay->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));    
    
    dlg_lay->addItem( butt_lay );
}

DlgUser::~DlgUser()
{

}



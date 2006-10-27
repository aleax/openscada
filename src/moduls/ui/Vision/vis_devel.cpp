
//OpenSCADA system module UI.Vision file: vis_devel.cpp
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#include <QWorkspace>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>

#include "xpm/close.xpm"
#include "xpm/exit.xpm"
#include "xpm/help.xpm"

#include <tsys.h>
#include "tvision.h"
#include "vis_devel_widgs.h"
#include "vis_devel.h"

using namespace VISION;

VisDevelop::VisDevelop()
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    mod->regWin( this );

    setWindowTitle(mod->I18N("Vision developing"));

    setCentralWidget(new QWorkspace);

    //- Create actions -
    //-- Close --
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t = QImage(close_xpm);
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(mod->I18N("Close configurator window"));
    actClose->setWhatsThis(mod->I18N("Close OpenSCADA configurator window"));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //-- Quit --
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t = QImage(exit_xpm);
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(mod->I18N("Quit OpenSCADA"));
    actQuit->setWhatsThis(mod->I18N("Quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //-- About "System info" --
    if(!ico_t.load(TUIS::icoPath("help").c_str())) ico_t = QImage(help_xpm);
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&About"),this);
    actAbout->setShortcut(Qt::Key_F1);
    actAbout->setToolTip(mod->I18N("Programm and OpenSCADA information"));
    actAbout->setWhatsThis(mod->I18N("Display programm and OpenSCADA information"));
    connect(actAbout, SIGNAL(activated()), this, SLOT(about()));
    //-- About Qt --
    QAction *actQtAbout = new QAction(mod->I18N("About &Qt"),this);
    actQtAbout->setToolTip(mod->I18N("Qt information"));
    actQtAbout->setWhatsThis(mod->I18N("Using QT information"));
    connect(actQtAbout, SIGNAL(activated()), this, SLOT(aboutQt()));
    
    //- Create menu -
    QMenu *mn_file = menuBar()->addMenu(mod->I18N("&File"));
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);	
    QMenu *mn_proj = menuBar()->addMenu(mod->I18N("&Project"));
    QMenu *mn_widg = menuBar()->addMenu(mod->I18N("&Widget"));
    QMenu *help = menuBar()->addMenu(mod->I18N("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);

    //- Init dock windows -
    addDockWidget(Qt::LeftDockWidgetArea,new ProjTree(this));

    //- Init status bar -
}

VisDevelop::~VisDevelop()
{
    mod->unregWin(this);
}

void VisDevelop::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

void VisDevelop::quitSt()
{
    SYS->stop();
}

void VisDevelop::about()
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),mod->I18N(
        "%s v%s.\n"
        "Autor: %s\n"
        "License: %s\n"), 
	mod->modInfo("Name").c_str(),mod->modInfo("Version").c_str(),mod->modInfo("Author").c_str(),
	mod->modInfo("License").c_str());
					
    QMessageBox::about(this,windowTitle(),buf);
}
					    
void VisDevelop::aboutQt()
{
    QMessageBox::aboutQt( this, mod->modInfo("Name").c_str() );
}


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
#include <QToolBar>
#include <QStatusBar>
#include <QPushButton>
#include <QWhatsThis>

#include "xpm/close.xpm"
#include "xpm/exit.xpm"
#include "xpm/help.xpm"
#include "xpm/contexthelp.xpm"
#include "xpm/wdg_del.xpm"
#include "xpm/wdg_edit.xpm"
#include "xpm/wdg_prop.xpm"
#include "xpm/wlib_add.xpm"
#include "xpm/wlib_del.xpm"
#include "xpm/wlib_prop.xpm"

#include <tsys.h>
#include "tvision.h"
#include "vis_devel_widgs.h"
#include "vis_widgs.h"
#include "vis_devel.h"

using namespace VISION;

VisDevelop::VisDevelop( string open_user )
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    mod->regWin( this );

    setWindowTitle(_("Vision developing"));
    setWindowIcon(mod->icon());

    setCentralWidget(new QWorkspace);

    //- Create actions -
    //-- Generic actions --
    //--- Close ---
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t = QImage(close_xpm);
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close Vision window"));
    actClose->setWhatsThis(_("The button for closing Vision developing window"));
    actClose->setStatusTip(_("Press for close of current Vision developing window."));        
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //--- Quit ---
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t = QImage(exit_xpm);
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit from OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA system."));    
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //--- About "System info" ---
    if(!ico_t.load(TUIS::icoPath("help").c_str())) ico_t = QImage(help_xpm);
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setShortcut(Qt::Key_F1);
    actAbout->setToolTip(_("Programm and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display the programm and OpenSCADA information"));
    actAbout->setStatusTip(_("Press for display the programm and OpenSCADA information."));
    connect(actAbout, SIGNAL(activated()), this, SLOT(about()));
    //--- About Qt ---
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for using QT information"));
    actQtAbout->setStatusTip(_("Press for using QT information."));
    connect(actQtAbout, SIGNAL(activated()), this, SLOT(aboutQt()));
    //What is
    if(!ico_t.load(TUIS::icoPath("contexthelp").c_str())) ico_t = QImage(contexthelp_xpm);
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setToolTip(_("The button for question about GUI elements"));
    actWhatIs->setWhatsThis(_("Get respond about user interface elements"));
    actWhatIs->setStatusTip(_("Press for respond about user interface elements."));
    connect(actWhatIs, SIGNAL(activated()), this, SLOT(enterWhatsThis()));			    
    
    //-- Widget and this library actions --
    //--- Widgets library add ---
    if(!ico_t.load(TUIS::icoPath("vision_wlib_add").c_str())) ico_t = QImage(wlib_add_xpm);
    actWdgLibAdd = new QAction(QPixmap::fromImage(ico_t),_("Add library"),this);
    actWdgLibAdd->setToolTip(_("Add widgets library"));
    actWdgLibAdd->setWhatsThis(_("The button for adding widget library"));
    actWdgLibAdd->setStatusTip(_("Press for adding widget library."));
    connect(actWdgLibAdd, SIGNAL(activated()), this, SLOT(wLibAdd()));
    //--- Widgets library delete ---
    if(!ico_t.load(TUIS::icoPath("vision_wlib_del").c_str())) ico_t = QImage(wlib_del_xpm);
    actWdgLibDel = new QAction(QPixmap::fromImage(ico_t),_("Delete library"),this);
    actWdgLibDel->setToolTip(_("Delete widgets library"));
    actWdgLibDel->setWhatsThis(_("The button for deleting widget library"));
    actWdgLibDel->setStatusTip(_("Press for deleting widget library"));
    actWdgLibDel->setEnabled(false);
    connect(actWdgLibDel, SIGNAL(activated()), this, SLOT(wLibDel()));
    //--- Widgets library properties ---
    if(!ico_t.load(TUIS::icoPath("vision_wlib_prop").c_str())) ico_t = QImage(wlib_prop_xpm);
    actWdgLibProp = new QAction(QPixmap::fromImage(ico_t),_("Library properties"),this);
    actWdgLibProp->setToolTip(_("Widgets library properties"));
    actWdgLibProp->setWhatsThis(_("The button for getting of widget library properties"));
    actWdgLibProp->setStatusTip(_("Press for getting of widget library properties."));
    actWdgLibProp->setEnabled(false);
    //connect(actWdgLibDel, SIGNAL(activated()), this, SLOT(???()));    
    //--- Add elementary figure widget ---
    /*if(!ico_t.load(TUIS::icoPath("vision_wdg_elfig").c_str())) ico_t = QImage(wdg_add_xpm);
    actWdgAdd = new QAction(QPixmap::fromImage(ico_t),_("Add widget"),this);
    actWdgAdd->setToolTip(_("Add widget into library"));
    actWdgAdd->setWhatsThis(_("The button for adding widget into library"));
    actWdgAdd->setStatusTip(_("Press for adding widget into library."));
    actWdgAdd->setEnabled(false);*/
    //connect(actWdgAdd, SIGNAL(activated()), this, SLOT(wdgAdd()));
    //--- Widget delete ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_del").c_str())) ico_t = QImage(wdg_del_xpm);
    actWdgDel = new QAction(QPixmap::fromImage(ico_t),_("Delete widget"),this);
    actWdgDel->setToolTip(_("Delete widget from library"));
    actWdgDel->setWhatsThis(_("The button for deleting widget from library"));
    actWdgDel->setStatusTip(_("Press for deleting widget from library."));
    actWdgDel->setEnabled(false);
    connect(actWdgDel, SIGNAL(activated()), this, SLOT(wdgDel()));
    //--- Widget properties ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_prop").c_str())) ico_t = QImage(wdg_prop_xpm);
    actWdgProp = new QAction(QPixmap::fromImage(ico_t),_("Widget properties"),this);
    actWdgProp->setToolTip(_("Get widget properties"));
    actWdgProp->setWhatsThis(_("The button for getting of widget properies"));
    actWdgProp->setStatusTip(_("Press for getting of widget properies."));
    actWdgProp->setEnabled(false);
    //connect(actWdgLibDel, SIGNAL(activated()), this, SLOT(???()));
    //--- Widget edit ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_edit").c_str())) ico_t = QImage(wdg_edit_xpm);
    actWdgEdit = new QAction(QPixmap::fromImage(ico_t),_("Widget edit"),this);
    actWdgEdit->setToolTip(_("Goes widget edit"));
    actWdgEdit->setWhatsThis(_("The button for goes to widget edition"));
    actWdgEdit->setStatusTip(_("Press for goes to widget edition."));
    actWdgEdit->setEnabled(false);
    //connect(actWdgLibDel, SIGNAL(activated()), this, SLOT(???()));
    //--- Get widget's primitives from VCA ---
    vector<string> pw_ls;
    mod->engine().at().origList(pw_ls);
    for( int i_w = 0; i_w < pw_ls.size(); i_w++ )
    {
        string simg = TSYS::strDecode(mod->engine().at().origIcon(pw_ls[i_w]),TSYS::base64);
	ico_t.loadFromData((const uchar*)simg.c_str(),simg.size());
	QAction *aprim = new QAction(QPixmap::fromImage(ico_t),QString(_("Add primitive '%1'")).arg(pw_ls[i_w].c_str()),this);
	aprim->setObjectName(pw_ls[i_w].c_str());
	aprim->setToolTip(QString(_("Add primitive widget '%1'")).arg(pw_ls[i_w].c_str()));
	aprim->setWhatsThis(QString(_("The button for add primitive widget '%1'")).arg(pw_ls[i_w].c_str()));
	aprim->setStatusTip(QString(_("Press for add primitive widget '%1'.")).arg(pw_ls[i_w].c_str()));
	//aprim->setEnabled(false);	
	prim_act.push_back(aprim);
    }    
    
    //- Create menu -
    QMenu *mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);	
    QMenu *mn_proj = menuBar()->addMenu(_("&Project"));
    QMenu *mn_widg = menuBar()->addMenu(_("&Widget"));
    mn_widg->addAction(actWdgLibAdd);
    mn_widg->addAction(actWdgLibDel);
    mn_widg->addAction(actWdgLibProp);
    mn_widg->addSeparator();
    for( int i_w = 0; i_w < prim_act.size(); i_w++ )
	mn_widg->addAction(prim_act[i_w]);
    mn_widg->addAction(actWdgDel);
    mn_widg->addAction(actWdgProp);
    mn_widg->addAction(actWdgEdit);
    QMenu *viewMenu = menuBar()->addMenu(_("&View"));
    QMenu *help = menuBar()->addMenu(_("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);
    help->addSeparator();
    help->addAction(actWhatIs);

    //- Init tool bars -
    //-- Widget tool bar --
    QToolBar *wdgToolBar = new QToolBar(_("Widgets toolbar"),this);
    addToolBar(wdgToolBar);
    wdgToolBar->addAction(actWdgLibAdd);
    wdgToolBar->addAction(actWdgLibDel);
    wdgToolBar->addAction(actWdgLibProp);
    wdgToolBar->addSeparator();
    for( int i_w = 0; i_w < prim_act.size(); i_w++ )
	wdgToolBar->addAction(prim_act[i_w]);    
    wdgToolBar->addAction(actWdgDel);
    wdgToolBar->addAction(actWdgProp);
    wdgToolBar->addAction(actWdgEdit);
    viewMenu->addAction(wdgToolBar->toggleViewAction());    
    viewMenu->addSeparator();

    //- Init dock windows -
    prjTree = new ProjTree(this);
    prjTree->setWhatsThis(_("Dock window for packet management."));
    wdgTree = new WdgTree(this);
    wdgTree->setWhatsThis(_("Dock window for widgets and this libraries management."));
    addDockWidget(Qt::LeftDockWidgetArea,prjTree);
    addDockWidget(Qt::LeftDockWidgetArea,wdgTree);
    tabifyDockWidget(prjTree,wdgTree);
    viewMenu->addAction(prjTree->toggleViewAction());
    viewMenu->addAction(wdgTree->toggleViewAction());
    viewMenu->addSeparator();
    
    attrInsp = new InspAttr(this);
    attrInsp->setWhatsThis(_("Dock window for widget's attributes inspection."));
    lnkInsp  = new InspLnk(this);
    lnkInsp->setWhatsThis(_("Dock window for widget's links inspection."));
    addDockWidget(Qt::RightDockWidgetArea,attrInsp);
    addDockWidget(Qt::RightDockWidgetArea,lnkInsp);
    tabifyDockWidget(attrInsp,lnkInsp);
    viewMenu->addAction(attrInsp->toggleViewAction());
    viewMenu->addAction(lnkInsp->toggleViewAction());
    
    //- Init status bar -
    user = new UserStBar(open_user.c_str(), this);
    user->setWhatsThis(_("This label display curent user."));
    user->setToolTip(_("Field for display of the current user."));
    user->setStatusTip(_("Double click for change user."));    
    statusBar()->insertPermanentWidget(0,user);
    
    statusBar()->showMessage(_("Ready"), 2000 );
    
    resize( 1000, 800 );
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
    
    snprintf(buf,sizeof(buf),_(
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

void VisDevelop::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void VisDevelop::wLibAdd( )
{
    //Make request id and name dialog
    InputDlg dlg(actWdgLibAdd->icon(),
	    _("Enter new widget's library identifier and name."),_("Add widget's library"),true,true);
    int dlrez = dlg.exec();
    if( dlrez == QDialog::Accepted )
    {
	string wl_id = dlg.id().toAscii().data();
	string wl_nm = dlg.name().toAscii().data();    
	//Create new widget's library
	if( TSYS::strEmpty(wl_id) || mod->engine().at().wlbPresent(wl_id) )
	{
	    mod->postMess(mod->nodePath().c_str(), 
		_("Widget library identifier is error or library already present."), TVision::Info );
	    return;
	}
	try
	{ 
	    mod->engine().at().wlbAdd(wl_id,wl_nm); 
	    wdgTree->updateLibs();
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Widget library create is error: %1")).arg(err.mess.c_str()), TVision::Error );
	}
    }
}

void VisDevelop::wLibDel( )
{
    //Check to library present
    string wl_id = TSYS::strSepParse(work_wdg,0,'.');
    if(!mod->engine().at().wlbPresent(wl_id))
	mod->postMess( mod->nodePath().c_str(), 
	    QString(_("Widget library '%1' no present.")).arg(wl_id.c_str()), TVision::Info );
    
    //Request to confirm
    InputDlg dlg(actWdgLibDel->icon(),
	    QString(_("You sure for delete widget's library '%1'.")).arg(wl_id.c_str()),
	    _("Delete widget's library"),false,false);
    int dlrez = dlg.exec();
    if( dlrez == QDialog::Accepted )
    {
	//Delete widget's library    
	try
	{
	    mod->engine().at().wlbDel(wl_id,true);
	    wdgTree->updateLibs();
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Widget library deleting is error: %1")).arg(err.mess.c_str()), TVision::Error );
	}
    }
}

void VisDevelop::wdgDel( )
{
    //Check to widget present
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    string wdgc_id = TSYS::strSepParse(work_wdg,2,'.');
    
    if( !mod->engine().at().wlbPresent(wlib_id) || 
	    !mod->engine().at().wlbAt(wlib_id).at().present(wdg_id) || 
	    (wdgc_id.size() && !mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgPresent(wdgc_id)) )
	mod->postMess( mod->nodePath().c_str(), 
	    QString(_("Widget '%1' no present.")).arg(work_wdg.c_str()), TVision::Info );
    
    //Request to confirm
    InputDlg dlg(actWdgDel->icon(),
	    QString(_("You sure for delete widget '%1'.")).arg(work_wdg.c_str()),
	    _("Delete widget"),false,false);
    int dlrez = dlg.exec();
    if( dlrez == QDialog::Accepted )
    {
	//Delete widget
	try
	{	
	    if( wdgc_id.size() )
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgDel(wdgc_id,true);
	    else mod->engine().at().wlbAt(wlib_id).at().del(wdg_id,true);
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Widget library deleting is error: %1")).arg(err.mess.c_str()), TVision::Error );
	}	
    }
}
		

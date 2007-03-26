
//OpenSCADA system module UI.Vision file: vis_devel.cpp
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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
#include <QSignalMapper>
#include <QCloseEvent>
#include <QMessageBox>
#include <QToolBar>
#include <QStatusBar>
#include <QPushButton>
#include <QWhatsThis>
#include <QTimer>
#include <QScrollArea>

#include <tsys.h>
#include "tvision.h"
#include "vis_devel_widgs.h"
#include "vis_widgs.h"
#include "vis_devel.h"

using namespace VISION;

VisDevelop::VisDevelop( string open_user ) : libPropDlg(NULL), wdgPropDlg(NULL), winClose(false)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    mod->regWin( this );

    setWindowTitle(_("Vision developing"));
    setWindowIcon(mod->icon());

    //- Init workspace -
    work_space = new QWorkspace(this);
    work_space->setScrollBarsEnabled(true);
    work_space->setBackground(QBrush(QColor(156,179,196)));
    setCentralWidget(work_space);

    //- Create actions -
    //-- Generic actions --
    //--- Close ---
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close Vision window"));
    actClose->setWhatsThis(_("The button for closing Vision developing window"));
    actClose->setStatusTip(_("Press for close of current Vision developing window."));        
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //--- Quit ---
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit from OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA system."));    
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //--- About "System info" ---
    if(!ico_t.load(TUIS::icoPath("help").c_str())) ico_t.load(":/images/help.png");
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
    if(!ico_t.load(TUIS::icoPath("contexthelp").c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setToolTip(_("The button for question about GUI elements"));
    actWhatIs->setWhatsThis(_("Get respond about user interface elements"));
    actWhatIs->setStatusTip(_("Press for respond about user interface elements."));
    connect(actWhatIs, SIGNAL(activated()), this, SLOT(enterWhatsThis()));			    
    
    //-- Widget and this library actions --
    //--- Load from db ---
    if(!ico_t.load(TUIS::icoPath("vision_db_load").c_str())) ico_t.load(":/images/db_load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t),_("Load from DB"),this);
    actDBLoad->setToolTip(_("Load item data from DB"));
    actDBLoad->setWhatsThis(_("The button for loading item data from DB"));
    actDBLoad->setStatusTip(_("Press for loading item data from DB."));
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(activated()), this, SLOT(itDBLoad()));
    //--- Save to db ---
    if(!ico_t.load(TUIS::icoPath("vision_db_save").c_str())) ico_t.load(":/images/db_save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t),_("Save to DB"),this);
    actDBSave->setToolTip(_("Save item data to DB"));
    actDBSave->setWhatsThis(_("The button for saving item data to DB"));
    actDBSave->setStatusTip(_("Press for saving item data to DB."));
    actDBSave->setEnabled(false);
    connect(actDBSave, SIGNAL(activated()), this, SLOT(itDBSave()));
    //--- Widgets library add ---
    if(!ico_t.load(TUIS::icoPath("vision_wlib_add").c_str())) ico_t.load(":/images/wlib_add.png");
    actWdgLibAdd = new QAction(QPixmap::fromImage(ico_t),_("Add library"),this);
    actWdgLibAdd->setToolTip(_("Add widgets library"));
    actWdgLibAdd->setWhatsThis(_("The button for adding widget library"));
    actWdgLibAdd->setStatusTip(_("Press for adding widget library."));
    connect(actWdgLibAdd, SIGNAL(activated()), this, SLOT(wLibAdd()));
    //--- Widgets library delete ---
    if(!ico_t.load(TUIS::icoPath("vision_wlib_del").c_str())) ico_t.load(":/images/wlib_del.png");
    actWdgLibDel = new QAction(QPixmap::fromImage(ico_t),_("Delete library"),this);
    actWdgLibDel->setToolTip(_("Delete widgets library"));
    actWdgLibDel->setWhatsThis(_("The button for deleting widget library"));
    actWdgLibDel->setStatusTip(_("Press for deleting widget library"));
    actWdgLibDel->setEnabled(false);
    connect(actWdgLibDel, SIGNAL(activated()), this, SLOT(wLibDel()));
    //--- Widgets library properties ---
    if(!ico_t.load(TUIS::icoPath("vision_wlib_prop").c_str())) ico_t.load(":/images/wlib_prop.png");
    actWdgLibProp = new QAction(QPixmap::fromImage(ico_t),_("Library properties"),this);
    actWdgLibProp->setToolTip(_("Widgets library properties"));
    actWdgLibProp->setWhatsThis(_("The button for getting of widget library properties"));
    actWdgLibProp->setStatusTip(_("Press for getting of widget library properties."));
    actWdgLibProp->setEnabled(false);
    connect(actWdgLibProp, SIGNAL(activated()), this, SLOT(wLibProp( )));
    //--- Add widget based at parent widget ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_elfig").c_str())) ico_t.load(":/images/wdg_add.png");
    actWdgAdd = new QAction(QPixmap::fromImage(ico_t),_("Add widget"),this);
    actWdgAdd->setToolTip(_("Add widget into library"));
    actWdgAdd->setWhatsThis(_("The button for adding widget into library"));
    actWdgAdd->setStatusTip(_("Press for adding widget into library."));
    actWdgAdd->setEnabled(false);
    connect(actWdgAdd, SIGNAL(activated()), this, SLOT(wdgAdd()));
    //--- Widget delete ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_del").c_str())) ico_t.load(":/images/wdg_del.png");
    actWdgDel = new QAction(QPixmap::fromImage(ico_t),_("Delete widget"),this);
    actWdgDel->setToolTip(_("Delete widget from library"));
    actWdgDel->setWhatsThis(_("The button for deleting widget from library"));
    actWdgDel->setStatusTip(_("Press for deleting widget from library."));
    actWdgDel->setEnabled(false);
    connect(actWdgDel, SIGNAL(activated()), this, SLOT(wdgDel()));
    //--- Widget properties ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_prop").c_str())) ico_t.load(":/images/wdg_prop.png");
    actWdgProp = new QAction(QPixmap::fromImage(ico_t),_("Widget properties"),this);
    actWdgProp->setToolTip(_("Get widget properties"));
    actWdgProp->setWhatsThis(_("The button for getting of widget properies"));
    actWdgProp->setStatusTip(_("Press for getting of widget properies."));
    actWdgProp->setEnabled(false);
    connect(actWdgProp, SIGNAL(activated()), this, SLOT(wdgProp()));
    //--- Widget edit ---
    if(!ico_t.load(TUIS::icoPath("vision_wdg_edit").c_str())) ico_t.load(":/images/wdg_edit.png");
    actWdgEdit = new QAction(QPixmap::fromImage(ico_t),_("Widget edit"),this);
    actWdgEdit->setToolTip(_("Goes widget edit"));
    actWdgEdit->setWhatsThis(_("The button for goes to widget edition"));
    actWdgEdit->setStatusTip(_("Press for goes to widget edition."));
    actWdgEdit->setEnabled(false);
    connect(actWdgEdit, SIGNAL(activated()), this, SLOT(wdgEdit()));
    //-- MDI windows actions --
    //--- Close active window ---
    actWinClose = new QAction(_("Cl&ose"), this);
    actWinClose->setToolTip(_("Close active window"));
    actWinClose->setWhatsThis(_("The button for the active window closing"));
    actWinClose->setStatusTip(_("Press for close active window."));
    connect(actWinClose, SIGNAL(triggered()), work_space, SLOT(closeActiveWindow()));
    //--- Close all windows ---
    actWinCloseAll = new QAction(_("Close &All"), this);
    actWinCloseAll->setToolTip(_("Close all windows"));
    actWinCloseAll->setWhatsThis(_("The button for all windows closing"));
    actWinCloseAll->setStatusTip(_("Press for close all windows."));
    connect(actWinCloseAll, SIGNAL(triggered()), work_space, SLOT(closeAllWindows()));
    //--- Tile windows ---
    actWinTile = new QAction(_("&Tile"), this);
    actWinTile->setToolTip(_("Tile all windows"));
    actWinTile->setWhatsThis(_("The button for all windows tiling"));
    actWinTile->setStatusTip(_("Press for tile all windows."));
    connect(actWinTile, SIGNAL(triggered()), work_space, SLOT(tile()));
    //--- Cascade windows ---
    actWinCascade = new QAction(_("&Cascade"), this);
    actWinCascade->setToolTip(_("Cascade all windows"));
    actWinCascade->setWhatsThis(_("The button for all windows cascading"));
    actWinCascade->setStatusTip(_("Press for cascade all windows."));
    connect(actWinCascade, SIGNAL(triggered()), work_space, SLOT(cascade()));
    //--- Activate next window ---
    actWinNext = new QAction(_("Ne&xt"), this);
    actWinNext->setToolTip(_("Activate next window"));
    actWinNext->setWhatsThis(_("The button for next window activation"));
    actWinNext->setStatusTip(_("Press for activate next window."));
    connect(actWinNext, SIGNAL(triggered()), work_space, SLOT(activateNextWindow()));
    //--- Activate previous window ---
    actWinPrevious = new QAction(_("Pre&vious"), this);
    actWinPrevious->setToolTip(_("Activate previous window"));
    actWinPrevious->setWhatsThis(_("The button for previous window activation"));
    actWinPrevious->setStatusTip(_("Press for activate previous window."));
    connect(actWinPrevious, SIGNAL(triggered()), work_space, SLOT(activatePreviousWindow()));																					     
    
    //- Create menu -
    mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actDBLoad);
    mn_file->addAction(actDBSave);
    mn_file->addSeparator();
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);	
    mn_proj = menuBar()->addMenu(_("&Project"));
    mn_widg = menuBar()->addMenu(_("&Widget"));
    mn_widg->addAction(actWdgLibAdd);
    mn_widg->addAction(actWdgLibDel);
    mn_widg->addAction(actWdgLibProp);
    mn_widg->addSeparator();
    mn_widg->addAction(actWdgAdd);
    mn_widg->addAction(actWdgDel);
    mn_widg->addAction(actWdgProp);
    mn_widg->addAction(actWdgEdit);
    mn_window = menuBar()->addMenu(_("&Window"));
    connect(mn_window, SIGNAL(aboutToShow()), this, SLOT(updateMenuWindow()));
    wMapper = new QSignalMapper(this);
    connect(wMapper, SIGNAL(mapped(QWidget *)), work_space, SLOT(setActiveWindow(QWidget *)));
    mn_view = menuBar()->addMenu(_("&View"));
    mn_help = menuBar()->addMenu(_("&Help"));
    mn_help->addAction(actAbout);
    mn_help->addAction(actQtAbout);
    mn_help->addSeparator();
    mn_help->addAction(actWhatIs);

    //- Init tool bars -
    //-- Main toolbar --
    QToolBar *mainToolBar = new QToolBar(_("Main toolbar"),this);
    mainToolBar->setObjectName("mainToolBar");
    addToolBar(mainToolBar);
    mainToolBar->addAction(actDBLoad);
    mainToolBar->addAction(actDBSave);
    mn_view->addAction(mainToolBar->toggleViewAction());
    //-- Widget tool bar --
    QToolBar *wdgToolBar = new QToolBar(_("Widgets toolbar"),this);
    wdgToolBar->setObjectName("wdgToolBar");
    addToolBar(wdgToolBar);
    wdgToolBar->addAction(actWdgLibAdd);
    wdgToolBar->addAction(actWdgLibDel);
    wdgToolBar->addAction(actWdgLibProp);
    wdgToolBar->addSeparator();
    wdgToolBar->addAction(actWdgAdd);
    wdgToolBar->addAction(actWdgDel);
    wdgToolBar->addAction(actWdgProp);
    wdgToolBar->addAction(actWdgEdit);
    mn_view->addAction(wdgToolBar->toggleViewAction());    
    mn_view->addSeparator();

    //- Init dock windows -
    prjTree = new ProjTree(this);
    prjTree->setWhatsThis(_("Dock window for packet management."));
    wdgTree = new WdgTree(this);
    wdgTree->setWhatsThis(_("Dock window for widgets and this libraries management."));
    addDockWidget(Qt::LeftDockWidgetArea,prjTree);
    addDockWidget(Qt::LeftDockWidgetArea,wdgTree);
    tabifyDockWidget(prjTree,wdgTree);
    mn_view->addAction(prjTree->toggleViewAction());
    mn_view->addAction(wdgTree->toggleViewAction());
    //mn_view->addSeparator();
    
    attrInsp = new InspAttrDock(this);
    attrInsp->setWhatsThis(_("Dock window for widget's attributes inspection."));
    lnkInsp  = new InspLnk(this);
    lnkInsp->setWhatsThis(_("Dock window for widget's links inspection."));
    addDockWidget(Qt::RightDockWidgetArea,attrInsp);
    addDockWidget(Qt::RightDockWidgetArea,lnkInsp);
    tabifyDockWidget(attrInsp,lnkInsp);
    mn_view->addAction(attrInsp->toggleViewAction());
    mn_view->addAction(lnkInsp->toggleViewAction());
    mn_view->addSeparator();
    
    //- Init status bar -
    w_user = new UserStBar(open_user.c_str(), this);
    w_user->setWhatsThis(_("This label display curent user."));
    w_user->setToolTip(_("Field for display of the current user."));
    w_user->setStatusTip(_("Double click for change user."));    
    statusBar()->insertPermanentWidget(0,w_user);    
    statusBar()->showMessage(_("Ready"), 2000 );
    
    //- Create timers -
    work_wdgTimer = new QTimer( this );
    work_wdgTimer->setSingleShot(true);
    work_wdgTimer->setInterval(200);
    connect(work_wdgTimer, SIGNAL(timeout()), this, SLOT(applyWorkWdg()));
    
    resize( 1000, 800 );
    
    updateLibToolbar();    
    
    //Restore main window state
    string st = TSYS::strDecode(TBDS::genDBGet(mod->nodePath()+"devWinState","",user()),TSYS::base64);
    restoreState(QByteArray(st.data(),st.size()));
}

VisDevelop::~VisDevelop()
{
    winClose = true;
    
    //Save main window state
    QByteArray st = saveState();
    TBDS::genDBSet(mod->nodePath()+"devWinState",
	    TSYS::strEncode(string(st.data(),st.size()),TSYS::base64),user());
    
    //Other data clean
    if( libPropDlg )	delete libPropDlg;
    if( wdgPropDlg )	delete wdgPropDlg;

    mod->unregWin(this);
}

string VisDevelop::user()
{
    return w_user->user().toAscii().data();
}

void VisDevelop::closeEvent( QCloseEvent* ce )
{
    winClose = true;
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

void VisDevelop::updateLibToolbar()
{
    bool is_create, root_allow;
    int i_lb, i_t, i_m, i_a, i_w;
    vector<string> lbls, wdgls;  
    QImage ico_t;
    string simg;

    //- Update library toolbars list -
    mod->engine().at().wlbList(lbls);
    //-- Delete toolbars --
    for(i_t = 0; i_t < lb_toolbar.size(); i_t++)
    {
	for( i_lb = 0; i_lb < lbls.size(); i_lb++ )
	    if( lb_toolbar[i_t]->objectName() == lbls[i_lb].c_str() )
		break;
	if( i_lb >= lbls.size() )
	{
	    delete lb_toolbar[i_t];
	    lb_toolbar.erase(lb_toolbar.begin()+i_t);
	}
    }

    //-- Delete menus --
    for(i_m = 0; i_m < lb_menu.size(); i_m++)
    {
	for( i_lb = 0; i_lb < lbls.size(); i_lb++ )
	    if( lb_menu[i_m]->objectName() == lbls[i_lb].c_str() )
		break;
	if( i_lb >= lbls.size() )
	{
	    delete lb_menu[i_m];
	    lb_menu.erase(lb_menu.begin()+i_m);
	}
    }
    
    //- Add libraries and check widget's actions at present -
    for( i_lb = 0; i_lb < lbls.size(); i_lb++ )
    {
	is_create = false;
	root_allow = false;
	//-- Add toolbars and menus --
	for(i_t = 0; i_t < lb_toolbar.size(); i_t++)
	    if( lb_toolbar[i_t]->objectName() == lbls[i_lb].c_str() )
		break;
	if( i_t == lb_toolbar.size() )
	{
	    lb_toolbar.push_back( new QToolBar(QString(_("Library: %1")).arg(lbls[i_lb].c_str()),this) );
	    lb_toolbar[i_t]->setObjectName(lbls[i_lb].c_str());
	    addToolBar(lb_toolbar[i_t]);
	    mn_view->addAction(lb_toolbar[i_t]->toggleViewAction());
	    is_create = true;
	}		
	for(i_m = 0; i_m < lb_menu.size(); i_m++)
	    if( lb_menu[i_m]->objectName() == lbls[i_lb].c_str() )
		break;	
	if( i_m == lb_menu.size() )
	{
	    lb_menu.push_back( new QMenu(QString(_("Library: %1")).arg(lbls[i_lb].c_str())) );
	    lb_menu[i_m]->setObjectName(lbls[i_lb].c_str());
	    mn_widg->addMenu(lb_menu[i_m]);
	}	
	//--- Update menu icon ---
    	simg = TSYS::strDecode(mod->engine().at().wlbAt(lbls[i_lb]).at().ico(),TSYS::base64);
	ico_t.loadFromData((const uchar*)simg.c_str(),simg.size());	    	    
	lb_menu[i_m]->setIcon(QPixmap::fromImage(ico_t));
	
	//-- Get widget's actions list --
	mod->engine().at().wlbAt(lbls[i_lb]).at().list(wdgls);
	QList<QAction *> use_act = lb_toolbar[i_t]->actions();
	//-- Delete widget's actions --	
	for(i_a = 0; i_a < use_act.size(); i_a++)
	{
	    for(i_w = 0; i_w < wdgls.size(); i_w++)
		if( TSYS::strSepParse(use_act[i_a]->objectName().toAscii().data(),1,'.') == wdgls[i_w] )
		    break;
	    if( i_w >= wdgls.size() )	delete use_act[i_a];
	}
	//-- Add widget's actions --
	use_act = lb_toolbar[i_t]->actions();	
	for(i_w = 0; i_w < wdgls.size(); i_w++)
	{
	    QAction *cur_act;
	    if(!root_allow && (mod->engine().at().wlbAt(lbls[i_lb]).at().at(wdgls[i_w]).at().parentNm() == "root"))
		root_allow = true;
	    for(i_a = 0; i_a < use_act.size(); i_a++)	    
		if( TSYS::strSepParse(use_act[i_a]->objectName().toAscii().data(),1,'.') == wdgls[i_w] )
		    break;
	    if( i_a < use_act.size() )	cur_act = use_act[i_a];
	    else
	    { 
		//--- Create new action ---
		string wl_path = lbls[i_lb]+"."+wdgls[i_w];
		cur_act = new QAction(mod->engine().at().wlbAt(lbls[i_lb]).at().at(wdgls[i_w]).at().name().c_str(),this);
		connect(cur_act, SIGNAL(activated()), this, SLOT(wdgAdd()));
		cur_act->setObjectName(wl_path.c_str());
		cur_act->setToolTip(QString(_("Add widget based at '%1'")).arg(wl_path.c_str()));
		cur_act->setWhatsThis(QString(_("The button for add widget based at '%1'")).arg(wl_path.c_str()));
		cur_act->setStatusTip(QString(_("Press for add widget based at '%1'.")).arg(wl_path.c_str()));
		cur_act->setEnabled(false);
		//--- Add action to toolbar and menu ---
		lb_toolbar[i_t]->addAction(cur_act);
		lb_menu[i_m]->addAction(cur_act);
	    }	    
	    //--- Update action ---
    	    simg = TSYS::strDecode(mod->engine().at().wlbAt(lbls[i_lb]).at().at(wdgls[i_w]).at().ico(),TSYS::base64);
	    ico_t.loadFromData((const uchar*)simg.c_str(),simg.size());	    
	    cur_act->setIcon(QPixmap::fromImage(ico_t));
	}
	if(is_create) lb_toolbar[i_t]->setVisible(root_allow);
    }
}

void VisDevelop::selectItem( const string &item )
{
    if( winClose )	return;
    
    work_wdg_new = item;
    work_wdgTimer->start();
}    
    
void VisDevelop::applyWorkWdg( )
{    
    bool isEn = false;
    
    //Set/update attributes inspector
    attrInsp->setWdg(work_wdg_new);
    
    //Update actions
    if( work_wdg == work_wdg_new ) return;
    work_wdg = work_wdg_new;
    
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    string wdgc_id = TSYS::strSepParse(work_wdg,2,'.');    

    //- Process main actions -
    isEn = wlib_id.size();
    actDBLoad->setEnabled(isEn);
    actDBSave->setEnabled(isEn);
    actWdgAdd->setEnabled(isEn);
    //- Process widget's library actions -
    isEn = wlib_id.size()&&(!wdgc_id.size());
    actWdgAdd->setEnabled(isEn);
    //-- Process toolbars actions of libraries widgets --
    for(int i_t = 0; i_t < lb_toolbar.size(); i_t++)
    {
	QList<QAction *> use_act = lb_toolbar[i_t]->actions();
        //-- Delete widget's actions --
        for(int i_a = 0; i_a < use_act.size(); i_a++)
	    use_act[i_a]->setEnabled(isEn);
    }    
    //-- Other widget's actions process --
    isEn = wlib_id.size()&&(!wdg_id.size());    
    actWdgLibDel->setEnabled(isEn);
    actWdgLibProp->setEnabled(isEn);
    //- Process widget actions -
    isEn = wdg_id.size();
    actWdgDel->setEnabled(isEn);
    actWdgProp->setEnabled(isEn);
    actWdgEdit->setEnabled(isEn);
}

void VisDevelop::updateMenuWindow()
{
    mn_window->clear();
    //- Add actions to menu -
    mn_window->addAction(actWinClose);
    mn_window->addAction(actWinCloseAll);
    mn_window->addSeparator();
    mn_window->addAction(actWinTile);
    mn_window->addAction(actWinCascade);
    mn_window->addSeparator();
    mn_window->addAction(actWinNext);
    mn_window->addAction(actWinPrevious);
    
    QList<QWidget *> windows = work_space->windowList();    
    //- Enable action state -
    QWidget *act_win = work_space->activeWindow();
    actWinClose->setEnabled(act_win);
    actWinCloseAll->setEnabled(!windows.isEmpty());
    actWinTile->setEnabled(!windows.isEmpty());
    actWinCascade->setEnabled(!windows.isEmpty());
    actWinNext->setEnabled(act_win&&windows.size()>1);
    actWinPrevious->setEnabled(act_win&&windows.size()>1);
    
    //- Add windows list -
    if( !windows.isEmpty() )	mn_window->addSeparator();    
    for(int i_w = 0; i_w < windows.size(); ++i_w) 
    {
	QWidget *child = windows.at(i_w);
	QAction *act = mn_window->addAction(QString((i_w<9)?"&%1 %2":"%1 %2").arg(i_w+1).arg(child->windowTitle()));
	act->setCheckable(true);
	act->setChecked(child == act_win);
	connect(act, SIGNAL(triggered()), wMapper, SLOT(map()));
        wMapper->setMapping(act, child);
    }
}

void VisDevelop::itDBLoad( )
{
    //Check to widget present
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    string wdgc_id = TSYS::strSepParse(work_wdg,2,'.');
    
    if( !mod->engine().at().wlbPresent(wlib_id) )
    	mod->postMess( mod->nodePath().c_str(), 
    		QString(_("Item no present or no select (%1).")).arg(work_wdg.c_str()), 
		TVision::Info, this );
    
    //Request to confirm
    InputDlg dlg(this,actDBLoad->icon(),
	    QString(wdg_id.size()?_("You sure load widget '%1' from DB?"):
				  _("You sure load widget library '%1' from DB?")).arg(work_wdg.c_str()),
	    _("Load item's data from DB"),false,false);
    int dlrez = dlg.exec();
    if( dlrez == QDialog::Accepted )
    {
	//Load widget
	try
	{
	    if( wdgc_id.size() )
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id).at().load();
	    else if( wdg_id.size() )
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().load();
	    else
	    {
		mod->engine().at().wlbAt(wlib_id).at().load();
		wdgTree->updateLibs();
		updateLibToolbar();
	    }
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		    QString(_("Load item's '%1' data is error: %2")).arg(work_wdg.c_str()).arg(err.mess.c_str()), 
		    TVision::Error, this );
	}	
    }
}

void VisDevelop::itDBSave( )
{
    //Check to widget present
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    string wdgc_id = TSYS::strSepParse(work_wdg,2,'.');
    
    if( !mod->engine().at().wlbPresent(wlib_id) )
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Item no present or no select (%1).")).arg(work_wdg.c_str()), 
		TVision::Info, this );
    
    //Request to confirm
    InputDlg dlg(this,actDBSave->icon(),
	    QString(wdg_id.size()?_("You sure save widget '%1' to DB?"):
				  _("You sure save widget library '%1' to DB?")).arg(work_wdg.c_str()),
	    _("Save item's data to DB"),false,false);
    int dlrez = dlg.exec();
    if( dlrez == QDialog::Accepted )
    {
	//Load widget
	try
	{
	    if( wdgc_id.size() )
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id).at().save();
	    else if( wdg_id.size() )
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().save();
	    else
		mod->engine().at().wlbAt(wlib_id).at().save();
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		    QString(_("Save item's '%1' data is error: %2")).arg(work_wdg.c_str()).arg(err.mess.c_str()), 
		    TVision::Error, this );
	}	
    }
}

void VisDevelop::wLibAdd( )
{
    //Check permission
    if(!SYS->security().at().access(user(),SEQ_WR,"root","UI",RWRWR_))
	mod->postMess(mod->nodePath().c_str(), _("Creating new libraries no permited."),TVision::Info, this );

    //Make request id and name dialog
    InputDlg dlg(this,actWdgLibAdd->icon(),
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
		_("Widget library identifier is error or library already present."), TVision::Info, this );
	    return;
	}
	try
	{ 
	    mod->engine().at().wlbAdd(wl_id,wl_nm); 
	    mod->engine().at().wlbAt(wl_id).at().setUser(user());
	    
	    wdgTree->updateLibs();
	    updateLibToolbar();
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Widget library create is error: %1")).arg(err.mess.c_str()), TVision::Error, this );
	}
    }
}

void VisDevelop::wLibDel( )
{
    //Check to library present
    string wl_id = TSYS::strSepParse(work_wdg,0,'.');
    if(!mod->engine().at().wlbPresent(wl_id))
	mod->postMess( mod->nodePath().c_str(), 
	    QString(_("Widget library '%1' no present.")).arg(wl_id.c_str()), TVision::Info, this );

    //Check permission
    if(!SYS->security().at().access(user(),SEQ_WR,
	    mod->engine().at().wlbAt(wl_id).at().user(),
	    mod->engine().at().wlbAt(wl_id).at().grp(),
	    mod->engine().at().wlbAt(wl_id).at().permit()))
	mod->postMess(mod->nodePath().c_str(), _("Deleting library is no permited."),TVision::Info, this );
    
    //Request to confirm
    InputDlg dlg(this,actWdgLibDel->icon(),
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
	    updateLibToolbar();
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Widget library deleting is error: %1")).arg(err.mess.c_str()), TVision::Error, this );
	}
    }
}

void VisDevelop::wLibProp( )
{
    if(!libPropDlg)
    {
	libPropDlg = new WdgLibProp(this);
    }
    libPropDlg->showDlg(TSYS::strSepParse(work_wdg,0,'.'));
}

void VisDevelop::wdgAdd( )
{
    QAction *cact = (QAction *)sender();
    string par_nm = cact->objectName().toAscii().data();
    
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    
    //Check to widget present    
    if( !mod->engine().at().wlbPresent(wlib_id) || 
	    (wdg_id.size()&&(!mod->engine().at().wlbAt(wlib_id).at().present(wdg_id))) )
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Widget container no present: '%1'.")).arg(work_wdg.c_str()), TVision::Info, this );
    
    //Check permission
    if( (wdg_id.size() && !SYS->security().at().access(user(),SEQ_WR,
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().user(),
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().grp(),
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().permit())) ||
	    (!wdg_id.size() && !SYS->security().at().access(user(),SEQ_WR, 
		mod->engine().at().wlbAt(wlib_id).at().user(),
		mod->engine().at().wlbAt(wlib_id).at().grp(),
		mod->engine().at().wlbAt(wlib_id).at().permit())) )
	mod->postMess(mod->nodePath().c_str(), _("Creating new widget no permited."),TVision::Info, this );
    
    //Make request id and name dialog
    InputDlg dlg(this,cact->icon(),
	    _("Enter new widget identifier and name."),_("Create widget"),true,true);
    int dlrez = dlg.exec();
    if( dlrez == QDialog::Accepted )
    {
	string w_id = dlg.id().toAscii().data();
	string w_nm = dlg.name().toAscii().data();
	if( wdg_id.size() && mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgPresent(w_id) )
	    mod->postMess(mod->nodePath().c_str(), 
		_("Source widget not container or created widget already present."), TVision::Info, this );
	else if( !wdg_id.size() && mod->engine().at().wlbAt(wlib_id).at().present(w_id) )
	    mod->postMess(mod->nodePath().c_str(), _("Widget already present."), TVision::Info, this );
	else
	{
	    try
	    { 
		if( wdg_id.size() )
		{
		    mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAdd(w_id,w_nm,par_nm);
		    if( par_nm.size() )
			mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(w_id).at().setEnable(true);
		}
		else 
		{
		    mod->engine().at().wlbAt(wlib_id).at().add(w_id,w_nm,par_nm);
		    mod->engine().at().wlbAt(wlib_id).at().at(w_id).at().setUser(user());
		    if( par_nm.size() )
			mod->engine().at().wlbAt(wlib_id).at().at(w_id).at().setEnable(true);
		}
		
		wdgTree->updateLibs();
		updateLibToolbar();
	    }
	    catch(TError err) 
	    { 
		mod->postMess(mod->nodePath().c_str(),
			QString(_("Widget create is error: %1")).arg(err.mess.c_str()), TVision::Error, this );
	    }
	}
    }
}

void VisDevelop::wdgDel( )
{
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    string wdgc_id = TSYS::strSepParse(work_wdg,2,'.');

    //Check to widget present    
    if( !mod->engine().at().wlbPresent(wlib_id) || 
	    !mod->engine().at().wlbAt(wlib_id).at().present(wdg_id) || 
	    (wdgc_id.size() && !mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgPresent(wdgc_id)) )
	mod->postMess( mod->nodePath().c_str(), 
	    QString(_("Widget '%1' no present.")).arg(work_wdg.c_str()), TVision::Info, this );

    //Check permission
    if( (wdg_id.size() && !SYS->security().at().access(user(),SEQ_WR,
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().user(),
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().grp(),
		mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().permit())) )
	mod->postMess(mod->nodePath().c_str(), _("Deleting widget is no permited."), TVision::Info, this);
    
    //Request to confirm
    InputDlg dlg(this,actWdgDel->icon(),
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

	    wdgTree->updateLibs();
	    updateLibToolbar();
	}
	catch(TError err) 
	{ 
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Widget library deleting is error: %1")).arg(err.mess.c_str()), TVision::Error, this );
	}	
    }
}

void VisDevelop::wdgProp( )
{
    if(!wdgPropDlg)
    {
	wdgPropDlg = new WdgProp(this);
    }
    wdgPropDlg->showDlg(work_wdg);
}

void VisDevelop::wdgEdit( )
{
    QScrollArea *scrl = new QScrollArea;
    //scrl->setAlignment(Qt::AlignCenter);
    scrl->setBackgroundRole(QPalette::Dark);
    scrl->setAttribute(Qt::WA_DeleteOnClose);
    scrl->setWindowTitle(QString(_("Widget: %1")).arg(work_wdg.c_str()));
    //- Set window icon -
    string wlib_id = TSYS::strSepParse(work_wdg,0,'.');
    string wdg_id = TSYS::strSepParse(work_wdg,1,'.');
    string wdgc_id = TSYS::strSepParse(work_wdg,2,'.');
    try
    {
        //-- Connect to widget --
	AutoHD<VCA::Widget> wdgLnk;
        if( wdgc_id.size() )
            wdgLnk = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
        else if( wdg_id.size() )
            wdgLnk = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
        if( !wdgLnk.freeStat() )
        {
            QImage ico_t;
	    string simg = TSYS::strDecode(wdgLnk.at().ico(),TSYS::base64);
            if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
        	scrl->setWindowIcon(QPixmap::fromImage(ico_t));
        }
    }catch(...) { }
    //- Make and place view widget -
    WdgView *vw = new WdgView(work_wdg);
    connect(vw, SIGNAL(selected(const string&)), this, SLOT(selectItem(const string&)));
    connect(attrInsp, SIGNAL(modified(const string &)), vw, SLOT(loadData(const string &)));
    
    scrl->setWidget( vw );
    work_space->addWindow(scrl);
    scrl->show();
    scrl->resize(300,200);
}


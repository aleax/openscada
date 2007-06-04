
//OpenSCADA system module UI.Vision file: vis_devel.cpp
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
 *   rom_as@diyaorg.dp.ua                                                  *
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
#include "vis_shapes.h"
#include "vis_devel_dlgs.h"
#include "vis_widgs.h"
#include "vis_run.h"
#include "vis_devel.h"

using namespace VISION;

VisDevelop::VisDevelop( string open_user ) : prjLibPropDlg(NULL), visItPropDlg(NULL), winClose(false)
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
    //--- What is ---
    if(!ico_t.load(TUIS::icoPath("contexthelp").c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setToolTip(_("The button for question about GUI elements"));
    actWhatIs->setWhatsThis(_("Get respond about user interface elements"));
    actWhatIs->setStatusTip(_("Press for respond about user interface elements."));
    connect(actWhatIs, SIGNAL(activated()), this, SLOT(enterWhatsThis()));			    
    
    //-- Page, project, widget and this library actions --
    //--- Load item from db ---
    if(!ico_t.load(TUIS::icoPath("vision_db_load").c_str())) ico_t.load(":/images/db_load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t),_("Load from DB"),this);
    actDBLoad->setToolTip(_("Load item data from DB"));
    actDBLoad->setWhatsThis(_("The button for loading item data from DB"));
    actDBLoad->setStatusTip(_("Press for loading item data from DB."));
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(activated()), this, SLOT(itDBLoad()));
    //--- Save item to db ---
    if(!ico_t.load(TUIS::icoPath("vision_db_save").c_str())) ico_t.load(":/images/db_save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t),_("Save to DB"),this);
    actDBSave->setToolTip(_("Save item data to DB"));
    actDBSave->setWhatsThis(_("The button for saving item data to DB"));
    actDBSave->setStatusTip(_("Press for saving item data to DB."));
    actDBSave->setEnabled(false);
    connect(actDBSave, SIGNAL(activated()), this, SLOT(itDBSave()));
    //--- Run project execution ---
    if(!ico_t.load(TUIS::icoPath("vision_prj_run").c_str())) ico_t.load(":/images/prj_run.png");
    actPrjRun = new QAction(QPixmap::fromImage(ico_t),_("Run project execution"),this);
    actPrjRun->setToolTip(_("Run project execution from selected item"));
    actPrjRun->setWhatsThis(_("The button for runing project execution from selected item"));
    actPrjRun->setStatusTip(_("Press for runing project execution from selected item."));
    actPrjRun->setEnabled(false);
    connect(actPrjRun, SIGNAL(activated()), this, SLOT(prjRun()));
    //--- Project create ---
    if(!ico_t.load(TUIS::icoPath("vision_prj_new").c_str())) ico_t.load(":/images/prj_new.png");
    actPrjNew = new QAction(QPixmap::fromImage(ico_t),_("New project"),this);
    actPrjNew->setToolTip(_("New project create"));
    actPrjNew->setWhatsThis(_("The button for creating new project"));
    actPrjNew->setStatusTip(_("Press for creating new project."));
    connect(actPrjNew, SIGNAL(activated()), this, SLOT(prjNew()));
    //--- Widgets library new create ---
    if(!ico_t.load(TUIS::icoPath("vision_lib_new").c_str())) ico_t.load(":/images/lib_new.png");
    actLibNew = new QAction(QPixmap::fromImage(ico_t),_("New library"),this);
    actLibNew->setToolTip(_("New widgets library create"));
    actLibNew->setWhatsThis(_("The button for creating new widgets library"));
    actLibNew->setStatusTip(_("Press for creating new widgets library."));
    connect(actLibNew, SIGNAL(activated()), this, SLOT(libNew()));    
    //--- Visual item add (widget or page) ---
    if(!ico_t.load(TUIS::icoPath("vision_it_add").c_str())) ico_t.load(":/images/it_add.png");
    actVisItAdd = new QAction(QPixmap::fromImage(ico_t),_("Add visual item"),this);
    actVisItAdd->setToolTip(_("Add visual item into library, container widget, project or page"));
    actVisItAdd->setWhatsThis(_("The button for adding visual item into library, container widget, project or page"));
    actVisItAdd->setStatusTip(_("Press for adding visual item into library, container widget, project or page."));
    actVisItAdd->setEnabled(false);
    actVisItAdd->setCheckable(true);
    //--- Visual item delete ---
    if(!ico_t.load(TUIS::icoPath("vision_it_del").c_str())) ico_t.load(":/images/it_del.png");
    actVisItDel = new QAction(QPixmap::fromImage(ico_t),_("Delete visual item"),this);
    actVisItDel->setToolTip(_("Delete visual item from library, container widget, project or page"));
    actVisItDel->setWhatsThis(_("The button for deleting visual item from library, container widget, project or page"));
    actVisItDel->setStatusTip(_("Press for deleting visual item library, container widget, project or page."));
    actVisItDel->setEnabled(false);
    connect(actVisItDel, SIGNAL(activated()), this, SLOT(visualItDel()));
    //--- Visual item properties ---
    if(!ico_t.load(TUIS::icoPath("vision_it_prop").c_str())) ico_t.load(":/images/it_prop.png");
    actVisItProp = new QAction(QPixmap::fromImage(ico_t),_("Visual item properties"),this);
    actVisItProp->setToolTip(_("Get visual item properties"));
    actVisItProp->setWhatsThis(_("The button for getting of visual item properies"));
    actVisItProp->setStatusTip(_("Press for getting of visual item properies."));
    actVisItProp->setEnabled(false);
    connect(actVisItProp, SIGNAL(activated()), this, SLOT(visualItProp()));
    //--- Visual item graphical edit ---
    if(!ico_t.load(TUIS::icoPath("vision_it_edit").c_str())) ico_t.load(":/images/it_edit.png");
    actVisItEdit = new QAction(QPixmap::fromImage(ico_t),_("Visual item edit"),this);
    actVisItEdit->setToolTip(_("Goes visual item edit"));
    actVisItEdit->setWhatsThis(_("The button for goes to visual item edition"));
    actVisItEdit->setStatusTip(_("Press for goes to visual item edition."));
    actVisItEdit->setEnabled(false);
    connect(actVisItEdit, SIGNAL(activated()), this, SLOT(visualItEdit()));

    //-- Widgets level actions --
    //--- Level up for widget ---
    if(!ico_t.load(TUIS::icoPath("vision_level_up").c_str())) ico_t.load(":/images/level_up.png");
    actLevUp = new QAction(QPixmap::fromImage(ico_t),_("Up widget"),this);
    actLevUp->setObjectName("level_up");
    actLevUp->setToolTip(_("Up selected widget"));
    actLevUp->setWhatsThis(_("The button for make up selected widget"));
    actLevUp->setStatusTip(_("Press for make up selected widget."));
    actLevUp->setEnabled(false);
    //--- Level down for widget ---
    if(!ico_t.load(TUIS::icoPath("vision_level_down").c_str())) ico_t.load(":/images/level_down.png");
    actLevDown = new QAction(QPixmap::fromImage(ico_t),_("Down widget"),this);
    actLevDown->setObjectName("level_down");    
    actLevDown->setToolTip(_("Down selected widget"));
    actLevDown->setWhatsThis(_("The button for make down selected widget"));
    actLevDown->setStatusTip(_("Press for make down selected widget."));
    actLevDown->setEnabled(false);
    //--- Rise widget ---
    if(!ico_t.load(TUIS::icoPath("vision_level_rise").c_str())) ico_t.load(":/images/level_rise.png");
    actLevRise = new QAction(QPixmap::fromImage(ico_t),_("Rise widget"),this);
    actLevRise->setObjectName("level_rise");
    actLevRise->setToolTip(_("Rise selected widget"));
    actLevRise->setWhatsThis(_("The button for rising selected widget"));
    actLevRise->setStatusTip(_("Press for rising selected widget."));
    actLevRise->setEnabled(false);
    //--- Lower widget ---
    if(!ico_t.load(TUIS::icoPath("vision_level_lower").c_str())) ico_t.load(":/images/level_lower.png");
    actLevLower = new QAction(QPixmap::fromImage(ico_t),_("Lower widget"),this);
    actLevLower->setObjectName("level_lower");
    actLevLower->setToolTip(_("Lower selected widget"));
    actLevLower->setWhatsThis(_("The button for lowering selected widget"));
    actLevLower->setStatusTip(_("Press for lowering selected widget."));
    actLevLower->setEnabled(false);
    
    //-- Widgets alignment actions --
    //--- Align widgets left ---
    if(!ico_t.load(TUIS::icoPath("vision_allign_left").c_str())) ico_t.load(":/images/allign_left.png");
    actAlignLeft = new QAction(QPixmap::fromImage(ico_t),_("Allign to left"),this);
    actAlignLeft->setObjectName("allign_left");
    actAlignLeft->setToolTip(_("Align selected widgets to left"));
    actAlignLeft->setWhatsThis(_("The button for align selected widgets to left"));
    actAlignLeft->setStatusTip(_("Press for align selected widgets to left."));
    actAlignLeft->setEnabled(false);
    //--- Align widgets to vertical center ---
    if(!ico_t.load(TUIS::icoPath("vision_allign_vcenter").c_str())) ico_t.load(":/images/allign_vcenter.png");
    actAlignVCenter = new QAction(QPixmap::fromImage(ico_t),_("Allign to vertical center"),this);
    actAlignVCenter->setObjectName("allign_vcenter");
    actAlignVCenter->setToolTip(_("Align selected widgets to vertical center"));
    actAlignVCenter->setWhatsThis(_("The button for align selected widgets to vertical center"));
    actAlignVCenter->setStatusTip(_("Press for align selected widgets to vertical center."));
    actAlignVCenter->setEnabled(false);
    //--- Align widgets to right ---
    if(!ico_t.load(TUIS::icoPath("vision_allign_right").c_str())) ico_t.load(":/images/allign_right.png");
    actAlignRight = new QAction(QPixmap::fromImage(ico_t),_("Allign to right"),this);
    actAlignRight->setObjectName("allign_right");
    actAlignRight->setToolTip(_("Align selected widgets to right"));
    actAlignRight->setWhatsThis(_("The button for align selected widgets to right"));
    actAlignRight->setStatusTip(_("Press for align selected widgets to right."));
    actAlignRight->setEnabled(false);
    //--- Align widgets to top ---
    if(!ico_t.load(TUIS::icoPath("vision_allign_top").c_str())) ico_t.load(":/images/allign_top.png");
    actAlignTop = new QAction(QPixmap::fromImage(ico_t),_("Allign to top"),this);
    actAlignTop->setObjectName("allign_top");
    actAlignTop->setToolTip(_("Align selected widgets to top"));
    actAlignTop->setWhatsThis(_("The button for align selected widgets to top"));
    actAlignTop->setStatusTip(_("Press for align selected widgets to top."));
    actAlignTop->setEnabled(false);
    //--- Align widgets to horizontal center ---
    if(!ico_t.load(TUIS::icoPath("vision_allign_hcenter").c_str())) ico_t.load(":/images/allign_hcenter.png");
    actAlignHCenter = new QAction(QPixmap::fromImage(ico_t),_("Allign to horizontal center"),this);
    actAlignHCenter->setObjectName("allign_hcenter");
    actAlignHCenter->setToolTip(_("Align selected widgets to horizontal center"));
    actAlignHCenter->setWhatsThis(_("The button for align selected widgets to horizontal center"));
    actAlignHCenter->setStatusTip(_("Press for align selected widgets to horizontal center."));
    actAlignHCenter->setEnabled(false);
    //--- Align widgets to bottom ---
    if(!ico_t.load(TUIS::icoPath("vision_allign_bottom").c_str())) ico_t.load(":/images/allign_bottom.png");
    actAlignBottom = new QAction(QPixmap::fromImage(ico_t),_("Allign to bottom"),this);
    actAlignBottom->setObjectName("allign_bottom");
    actAlignBottom->setToolTip(_("Align selected widgets to bottom"));
    actAlignBottom->setWhatsThis(_("The button for align selected widgets to bottom"));
    actAlignBottom->setStatusTip(_("Press for align selected widgets to bottom."));
    actAlignBottom->setEnabled(false);
    
    //-- Elementar figure actions --
    //--- Line creation ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_line").c_str())) ico_t.load(":/images/elfig_line.png");
    actElFigLine = new QAction(QPixmap::fromImage(ico_t),_("Add line"),this);
    actElFigLine->setObjectName("line");
    actElFigLine->setToolTip(_("Add line to elementary figure"));
    actElFigLine->setWhatsThis(_("The button for add line to elementary figure"));
    actElFigLine->setStatusTip(_("Press for add line to elementary figure."));
    //--- Arc creation ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_arc").c_str())) ico_t.load(":/images/elfig_arc.png");
    actElFigArc = new QAction(QPixmap::fromImage(ico_t),_("Add arc"),this);
    actElFigArc->setObjectName("arc");
    actElFigArc->setToolTip(_("Add arc to elementary figure"));
    actElFigArc->setWhatsThis(_("The button for add arc to elementary figure"));
    actElFigArc->setStatusTip(_("Press for add arc to elementary figure."));
    //--- Add Besie curve ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_besie").c_str())) ico_t.load(":/images/elfig_besie.png");
    actElFigBesie = new QAction(QPixmap::fromImage(ico_t),_("Add besie curve"),this);
    actElFigBesie->setObjectName("besie");
    actElFigBesie->setToolTip(_("Add Besie curve to elementary figure"));
    actElFigBesie->setWhatsThis(_("The button for add Besie curve to elementary figure"));
    actElFigBesie->setStatusTip(_("Press for add Besie curve to elementary figure."));    

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

    //- Action groups -
    //-- Create widgets action groups --
    actGrpWdgAdd = new QActionGroup(this);
    actGrpWdgAdd->setExclusive(true);
    connect(actGrpWdgAdd, SIGNAL(triggered(QAction*)), this, SLOT(visualItAdd(QAction*)));
    actGrpWdgAdd->addAction(actVisItAdd);
    
    //- Create menu -
    mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actDBLoad);
    mn_file->addAction(actDBSave);
    mn_file->addSeparator();
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);	
    mn_proj = menuBar()->addMenu(_("&Project"));
    mn_proj->addAction(actPrjRun);
    mn_proj->addSeparator();
    mn_proj->addAction(actPrjNew);
    mn_proj->addAction(actVisItAdd);
    mn_proj->addAction(actVisItDel);
    mn_proj->addAction(actVisItProp);
    mn_proj->addAction(actVisItEdit);    
    mn_widg = menuBar()->addMenu(_("&Widget"));
    mn_widg->addAction(actLibNew);
    mn_widg->addAction(actVisItAdd);
    mn_widg->addAction(actVisItDel);
    mn_widg->addAction(actVisItProp);
    mn_widg->addAction(actVisItEdit);
    mn_widg_fnc = new QMenu(_("&View"));
    mn_widg_fnc->addAction(actLevRise);
    mn_widg_fnc->addAction(actLevLower);
    mn_widg_fnc->addAction(actLevUp);
    mn_widg_fnc->addAction(actLevDown);
    mn_widg_fnc->addSeparator();
    mn_widg_fnc->addAction(actAlignLeft);
    mn_widg_fnc->addAction(actAlignVCenter);
    mn_widg_fnc->addAction(actAlignRight);
    mn_widg_fnc->addAction(actAlignTop);
    mn_widg_fnc->addAction(actAlignHCenter);
    mn_widg_fnc->addAction(actAlignBottom);
    mn_widg->addMenu(mn_widg_fnc);
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
    //-- Visual items tools bar --
    QToolBar *visItToolBar = new QToolBar(_("Visual items toolbar"),this);
    visItToolBar->setObjectName("visItToolBar");
    addToolBar(visItToolBar);
    visItToolBar->addAction(actPrjRun);
    visItToolBar->addSeparator();
    visItToolBar->addAction(actDBLoad);
    visItToolBar->addAction(actDBSave);
    visItToolBar->addSeparator();
    visItToolBar->addAction(actPrjNew);
    visItToolBar->addAction(actLibNew);
    visItToolBar->addAction(actVisItAdd);
    visItToolBar->addAction(actVisItDel);
    visItToolBar->addAction(actVisItProp);
    visItToolBar->addAction(actVisItEdit);
    mn_view->addAction(visItToolBar->toggleViewAction());
    //-- Widget view functions toolbar --
    wdgToolView = new QToolBar(_("Widgets view functions"),this);
    wdgToolView->setObjectName("wdgToolView");
    addToolBar(wdgToolView);
    wdgToolView->addAction(actLevRise);
    wdgToolView->addAction(actLevLower);
    wdgToolView->addAction(actLevUp);
    wdgToolView->addAction(actLevDown);
    wdgToolView->addSeparator();
    wdgToolView->addAction(actAlignLeft);
    wdgToolView->addAction(actAlignVCenter);
    wdgToolView->addAction(actAlignRight);
    wdgToolView->addAction(actAlignTop);
    wdgToolView->addAction(actAlignHCenter);
    wdgToolView->addAction(actAlignBottom);    
    mn_view->addAction(wdgToolView->toggleViewAction());
    //-- Elementar figure base widget tools --
    elFigTool = new QToolBar(_("Elementars figure tools"),this);
    elFigTool->setObjectName("elFigTool");
    addToolBar(elFigTool);
    elFigTool->addAction(actElFigLine);
    elFigTool->addAction(actElFigArc);
    elFigTool->addAction(actElFigBesie);
    mn_view->addAction(elFigTool->toggleViewAction());    
    mn_view->addSeparator();

    //- Init status bar -
    w_user = new UserStBar(open_user.c_str(), this);
    w_user->setWhatsThis(_("This label display curent user."));
    w_user->setToolTip(_("Field for display of the current user."));
    w_user->setStatusTip(_("Double click for change user."));    
    statusBar()->insertPermanentWidget(0,w_user);    

    //- Init dock windows -
    prjTree = new ProjTree(this);
    connect(this,SIGNAL(modifiedItem(const string&)),prjTree,SLOT(updateTree(const string&)));    
    connect(prjTree,SIGNAL(selectItem(const string&)),this,SLOT(selectItem(const string&)));
    prjTree->setWhatsThis(_("Dock window for packet management."));
    wdgTree = new WdgTree(this);
    connect(this,SIGNAL(modifiedItem(const string&)),wdgTree,SLOT(updateTree(const string&)));
    connect(wdgTree,SIGNAL(selectItem(const string&)),this,SLOT(selectItem(const string&)));
    wdgTree->setWhatsThis(_("Dock window for widgets and this libraries management."));
    addDockWidget(Qt::LeftDockWidgetArea,prjTree);
    addDockWidget(Qt::LeftDockWidgetArea,wdgTree);
    tabifyDockWidget(prjTree,wdgTree);
    mn_view->addAction(prjTree->toggleViewAction());
    mn_view->addAction(wdgTree->toggleViewAction());
    //mn_view->addSeparator();
    
    attrInsp = new InspAttrDock(this);
    connect(attrInsp, SIGNAL(modified(const string &)), this, SIGNAL(modifiedItem(const string &)));
    attrInsp->setWhatsThis(_("Dock window for widget's attributes inspection."));
    lnkInsp  = new InspLnkDock(this);
    lnkInsp->setWhatsThis(_("Dock window for widget's links inspection."));
    addDockWidget(Qt::RightDockWidgetArea,attrInsp);
    addDockWidget(Qt::RightDockWidgetArea,lnkInsp);
    tabifyDockWidget(attrInsp,lnkInsp);
    mn_view->addAction(attrInsp->toggleViewAction());
    mn_view->addAction(lnkInsp->toggleViewAction());
    mn_view->addSeparator();
    
    //- Create timers -
    work_wdgTimer = new QTimer( this );
    work_wdgTimer->setSingleShot(true);
    work_wdgTimer->setInterval(200);
    connect(work_wdgTimer, SIGNAL(timeout()), this, SLOT(applyWorkWdg()));

    resize( 1000, 800 );

    connect(this, SIGNAL(modifiedItem(const string&)), this, SLOT(updateLibToolbar()));
    updateLibToolbar();
    wdgTree->updateTree();
    prjTree->updateTree();

    //Restore main window state
    string st = TSYS::strDecode(TBDS::genDBGet(mod->nodePath()+"devWinState","",user()),TSYS::base64);
    restoreState(QByteArray(st.data(),st.size()));

    //Hide specific tools
    wdgToolView->setVisible(false);
    elFigTool->setVisible(false);
    
    statusBar()->showMessage(_("Ready"), 2000 );    
}

VisDevelop::~VisDevelop()
{
    winClose = true;
    
    //Save main window state
    QByteArray st = saveState();
    TBDS::genDBSet(mod->nodePath()+"devWinState",
	    TSYS::strEncode(string(st.data(),st.size()),TSYS::base64),user());
    
    //Other data clean
    if( prjLibPropDlg )	delete prjLibPropDlg;
    if( visItPropDlg )	delete visItPropDlg;

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
    QMessageBox::about(this,windowTitle(),
    	    QString(_("%1 v%2.\nAutor: %3\nLicense: %4\n")).
	        arg(mod->modInfo("Name").c_str()).
	        arg(mod->modInfo("Version").c_str()).
	        arg(mod->modInfo("Author").c_str()).
	        arg(mod->modInfo("License").c_str()));
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
    vector<string> lbls;
    QImage ico_t;
    string simg;

    XMLNode prm_req("get");
    prm_req.setAttr("user",user()); 
    
    //- Update library toolbars list -
    XMLNode lb_req("get");    
    lb_req.setAttr("user",user())->setAttr("path","/%2fprm%2fcfg%2fwlb");
    if( !mod->cntrIfCmd(lb_req) )
	for( int i_ch = 0; i_ch < lb_req.childSize(); i_ch++ )
	    lbls.push_back(lb_req.childGet(i_ch)->attr("id"));
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
 	prm_req.setAttr("path","/wlb_"+lbls[i_lb]+"/%2fico");
	if( !mod->cntrIfCmd(prm_req) )
	{
	    simg = TSYS::strDecode(prm_req.text(),TSYS::base64);	    
	    if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		lb_menu[i_m]->setIcon(QPixmap::fromImage(ico_t));
	}
	
	//-- Get widget's actions list --	
    	vector<string> wdgls;  
	lb_req.childClean();
	lb_req.setAttr("path","/wlb_"+lbls[i_lb]+"/%2fwdg%2fwdg");
	if( !mod->cntrIfCmd(lb_req) )
    	    for( int i_ch = 0; i_ch < lb_req.childSize(); i_ch++ )
    		wdgls.push_back(lb_req.childGet(i_ch)->attr("id"));
	
	QList<QAction *> use_act = lb_toolbar[i_t]->actions();	
	//-- Delete widget's actions --	
	for(i_a = 0; i_a < use_act.size(); i_a++)
	{
	    for(i_w = 0; i_w < wdgls.size(); i_w++)
		if( use_act[i_a]->objectName() == (string("/wlb_")+lbls[i_lb]+"/wdg_"+wdgls[i_w]).c_str() )
		    break;
	    if( i_w >= wdgls.size() )	delete use_act[i_a];
	}
	//-- Add widget's actions --
	use_act = lb_toolbar[i_t]->actions();	
	for(i_w = 0; i_w < wdgls.size(); i_w++)
	{	    
	    QAction *cur_act;
	    //--- Get parent name ---
	    string wipath = "/wlb_"+lbls[i_lb]+"/wdg_"+wdgls[i_w];
	    prm_req.setAttr("path",wipath+"/%2fwdg%2fst%2fparent");
    	    if( !mod->cntrIfCmd(prm_req) )
	    	if(!root_allow && prm_req.text() == "root") root_allow = true;
	    //--- Delete action ---
	    for(i_a = 0; i_a < use_act.size(); i_a++)	    
		if( use_act[i_a]->objectName() == wipath.c_str() )
		    break;
	    if( i_a < use_act.size() )	cur_act = use_act[i_a];
	    else
	    { 
		//--- Create new action ---
		cur_act = new QAction(lb_req.childGet(i_w)->text().c_str(),this);
		//connect(cur_act, SIGNAL(activated()), this, SLOT(wdgAdd()));
		cur_act->setObjectName(wipath.c_str());
		cur_act->setToolTip(QString(_("Add widget based at '%1'")).arg(wipath.c_str()));
		cur_act->setWhatsThis(QString(_("The button for add widget based at '%1'")).arg(wipath.c_str()));
		cur_act->setStatusTip(QString(_("Press for add widget based at '%1'.")).arg(wipath.c_str()));
		cur_act->setEnabled(false);
		cur_act->setCheckable(true);
		//--- Add action to toolbar and menu ---
		actGrpWdgAdd->addAction(cur_act);
		lb_toolbar[i_t]->addAction(cur_act);
		lb_menu[i_m]->addAction(cur_act);
	    }	    
	    //--- Update action ---
	    prm_req.setAttr("path",wipath+"/%2fico");
    	    if( !mod->cntrIfCmd(prm_req) )
    	    {
    		simg = TSYS::strDecode(prm_req.text(),TSYS::base64);	    
    		if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    cur_act->setIcon(QPixmap::fromImage(ico_t));
	    }
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
    lnkInsp->setWdg(work_wdg_new);
    
    //Update actions
    if( work_wdg == work_wdg_new ) return;
    work_wdg = work_wdg_new;
    
    string cur_wdg = TSYS::strSepParse(work_wdg,0,';');	//Get first select element
    string sel1 = TSYS::pathLev(cur_wdg,0);
    string sel2 = TSYS::pathLev(cur_wdg,1);
    string sel3 = TSYS::pathLev(cur_wdg,2);

    bool isProj = sel1.substr(0,4)=="prj_";
    bool isLib  = sel1.substr(0,4)=="wlb_";

    //- Process main actions -
    actPrjRun->setEnabled(isProj);
    actDBLoad->setEnabled(sel1.size());
    actDBSave->setEnabled(sel1.size());
    
    //- Set visual item's actions -
    actVisItAdd->setEnabled(isProj || (isLib&&sel3.empty()));    
    //-- Process add actions of visual items --
    for( int i_a = 0; i_a < actGrpWdgAdd->actions().size(); i_a++ )
	actGrpWdgAdd->actions().at(i_a)->setEnabled(isProj || (isLib&&sel3.empty()));
    //- Process visual item actions -
    actVisItDel->setEnabled(isProj || isLib);
    actVisItProp->setEnabled(isProj || isLib);
    actVisItEdit->setEnabled((isProj || isLib) && sel2.size());
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
    string own_wdg = work_wdg;

    //- Request to confirm -
    InputDlg dlg(this,actDBLoad->icon(),
	    QString(_("You sure for load visual items '%1' from DB?")).arg(own_wdg.c_str()),
	    _("Load visual item's data from DB"),false,false);
    if( dlg.exec() == QDialog::Accepted )
    {
	int i_it = 0;
     	string cur_wdg;
	while( !(cur_wdg=TSYS::strSepParse(own_wdg,i_it++,';')).empty() )
	{
	    //-- Send load request --
	    string sel2 = TSYS::pathLev(cur_wdg,1);
	    
	    XMLNode prm_req("set");
	    prm_req.setAttr("user",user())->
		    setAttr("path",cur_wdg+"/"+TSYS::strEncode(sel2.empty()?"/obj/cfg/load":"/wdg/cfg/load",TSYS::PathEl));
        
	    if( mod->cntrIfCmd(prm_req) )
		mod->postMess(prm_req.attr("mcat").c_str(),prm_req.text().c_str(),TVision::Error,this);
	}
    }
}

void VisDevelop::itDBSave( )
{
    string own_wdg = work_wdg;

    //- Request to confirm -
    InputDlg dlg(this,actDBSave->icon(),
	    QString(_("You sure for save visual items '%1' to DB?")).arg(own_wdg.c_str()),
	    _("Save visual item's data to DB"),false,false);
    if( dlg.exec() == QDialog::Accepted )
    {
	int i_it = 0;    
     	string cur_wdg;	
	while( !(cur_wdg=TSYS::strSepParse(own_wdg,i_it++,';')).empty() )
	{
	    //-- Send load request --
	    string sel2 = TSYS::pathLev(cur_wdg,1);
	    
	    XMLNode prm_req("set");
	    prm_req.setAttr("user",user())->
		    setAttr("path",cur_wdg+"/"+TSYS::strEncode(sel2.empty()?"/obj/cfg/save":"/wdg/cfg/save",TSYS::PathEl));
        
	    if( mod->cntrIfCmd(prm_req) )
		mod->postMess(prm_req.attr("mcat").c_str(),prm_req.text().c_str(),TVision::Error,this);
	}
    }
}

void VisDevelop::prjRun( )
{
    string own_wdg = TSYS::strSepParse(work_wdg,0,';');
    
    InputDlg dlg(this,actPrjRun->icon(),
	    QString(_("You sure for run a new project's item session '%1'.")).arg(work_wdg.c_str()),
	    _("Run project"),false,false);
    if( dlg.exec() == QDialog::Accepted )
    {
	VisRun *sess = new VisRun( own_wdg, user() );
	sess->show();
	sess->raise();
	sess->activateWindow();
    }
}

void VisDevelop::prjNew( )
{
    InputDlg dlg(this,actPrjNew->icon(),
	    _("Enter new project's identifier and name."),_("New project"),true,true);
    if( dlg.exec() == QDialog::Accepted )
    {
	XMLNode dt_req("add");
	dt_req.setAttr("path","/%2fprm%2fcfg%2fprj")->
	       setAttr("id",dlg.id().toAscii().data())->
	       setAttr("user",user())->
	       setText(dlg.name().toAscii().data());
        if( mod->cntrIfCmd(dt_req) )	    
	    mod->postMess(dt_req.attr("mcat").c_str(),dt_req.text().c_str(),TVision::Error,this);
	else emit modifiedItem(string("prj_")+dlg.id().toAscii().data());
    }
}

void VisDevelop::libNew( )
{
    InputDlg dlg(this,actPrjNew->icon(),
	    _("Enter new widget's library identifier and name."),_("New widget's library"),true,true);
    if( dlg.exec() == QDialog::Accepted )
    {
	XMLNode dt_req("add");
	dt_req.setAttr("path","/%2fprm%2fcfg%2fwlb")->
	       setAttr("id",dlg.id().toAscii().data())->
	       setAttr("user",user())->
	       setText(dlg.name().toAscii().data());
        if( mod->cntrIfCmd(dt_req) )	    
	    mod->postMess(dt_req.attr("mcat").c_str(),dt_req.text().c_str(),TVision::Error,this);
	else emit modifiedItem(string("wlb_")+dlg.id().toAscii().data());
    }    
}

void VisDevelop::visualItAdd( QAction *cact, const QPoint &pnt )
{
    //QAction *cact = (QAction *)sender();
    string own_wdg = TSYS::strSepParse(work_wdg,0,';');
    string par_nm = cact->objectName().toAscii().data();

    if( work_space->activeWindow() && !wdgTree->hasFocus() && !prjTree->hasFocus() && pnt.isNull() )	return;
    
    //Count level
    int p_el_cnt = 0;
    while( TSYS::pathLev(own_wdg,p_el_cnt).size() ) p_el_cnt++;
    
    //Make request id and name dialog
    InputDlg dlg(this,cact->icon(),
	    _("Enter new widget/page identifier and name."),_("Create widget/page"),true,true);
    if( dlg.exec() == QDialog::Accepted )
    {    
        string w_id = dlg.id().toAscii().data();
        string w_nm = dlg.name().toAscii().data();			
    
	string sid1 = TSYS::pathLev(own_wdg,0);
    
	XMLNode add_req("add");
	add_req.setAttr("user",user());
	
	//Check for widget's library
	string new_wdg;
	if( sid1.substr(0,4) == "wlb_" )
	{
	    if( p_el_cnt == 1 )
		add_req.setAttr("path",own_wdg+"/%2fwdg%2fwdg")->setAttr("id",w_id)->setText(w_nm);
	    else add_req.setAttr("path",own_wdg+"/%2finclwdg%2fwdg")->setAttr("id",w_id)->setText(w_nm);
	    new_wdg=own_wdg+"/wdg_"+w_id;
	}
	else if( sid1.substr(0,4) == "prj_" )
	{
	    if( p_el_cnt == 1 )
		add_req.setAttr("path",own_wdg+"/%2fpage%2fpage")->setAttr("id",w_id)->setText(w_nm);
	    else add_req.setAttr("path",own_wdg+"/%2fpage%2fpage")->setAttr("id",w_id)->setText(w_nm);
	    new_wdg=own_wdg+"/pg_"+w_id;
	}
	//- Create widget -
    	int err = mod->cntrIfCmd(add_req); 
	if( err ) mod->postMess(add_req.attr("mcat").c_str(),add_req.text().c_str(),TVision::Error,this);
	else
	{
	    //- Set some parameters -
	    XMLNode set_req("set");
	    set_req.setAttr("user",user());
	    if( !par_nm.empty() )
	    {
		//-- Set parent widget name --
	        set_req.setAttr("path",new_wdg+"/%2fwdg%2fst%2fparent")->setText(par_nm);
		err = mod->cntrIfCmd(set_req);
		set_req.setAttr("path",new_wdg+"/%2fwdg%2fst%2fen")->setText("1");
		err = mod->cntrIfCmd(set_req);
	    }
	    if( !err && !pnt.isNull() )
	    {
		set_req.setAttr("path",new_wdg+"/%2fattr%2fgeomX")->setText(TSYS::int2str(pnt.x()));
		err = mod->cntrIfCmd(set_req);
		set_req.setAttr("path",new_wdg+"/%2fattr%2fgeomY")->setText(TSYS::int2str(pnt.y()));
		err = mod->cntrIfCmd(set_req);
	    }
	    if( err ) mod->postMess(set_req.attr("mcat").c_str(),set_req.text().c_str(),TVision::Error,this);
	    else emit modifiedItem(own_wdg);
	}
    }
    cact->setChecked(false);    
}

void VisDevelop::visualItDel( )
{
    string del_wdg;
    int w_cnt = 0;
    while((del_wdg=TSYS::strSepParse(work_wdg,w_cnt++,';')).size())
    {
	//- Get owner object path and deleted item identifier -
	string it_own, it_id;
	int p_el_cnt = 0;
	string it_tmp = TSYS::pathLev(del_wdg,p_el_cnt++);
	do 
	{
	    it_own= it_own+(it_id.empty() ? "" : ("/"+it_id));
	    it_id = it_tmp;
	}	
	while( (it_tmp=TSYS::pathLev(del_wdg,p_el_cnt++)).size() );
	p_el_cnt--;
	//- Request to confirm -
	InputDlg dlg(this,actVisItDel->icon(),
		QString(_("You sure for delete visual item '%1'.")).arg(del_wdg.c_str()),
			_("Delete visual item"),false,false);
	if( dlg.exec() == QDialog::Accepted )
	{
	    XMLNode dt_req("del");
	    dt_req.setAttr("user",user());
	    
	    string sid1 = TSYS::pathLev(it_own,0);
	    //Check for widget's library
	    if( sid1.empty() )
	    {
		if( it_id.substr(0,4) == "wlb_" )
	    	    dt_req.setAttr("path","/%2fprm%2fcfg%2fwlb")->setAttr("id",it_id.substr(4));
		else if( it_id.substr(0,4) == "prj_" )
	    	    dt_req.setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("id",it_id.substr(4));
	    }
	    else if( sid1.substr(0,4) == "wlb_" )
	    {
		if( p_el_cnt <= 2 )
		    dt_req.setAttr("path",it_own+"/%2fwdg%2fwdg")->setAttr("id",it_id.substr(4));
		else dt_req.setAttr("path",it_own+"/%2finclwdg%2fwdg")->setAttr("id",it_id.substr(4));
	    }
	    else if( sid1.substr(0,4) == "prj_" )
	    {
		if( p_el_cnt <= 2 )
		    dt_req.setAttr("path",it_own+"/%2fpage%2fpage")->setAttr("id",it_id.substr(3));
		else if( it_id.substr(0,3) == "pg_" )
		    dt_req.setAttr("path",it_own+"/%2fpage%2fpage")->setAttr("id",it_id.substr(3));
		else dt_req.setAttr("path",it_own+"/%2finclwdg%2fwdg")->setAttr("id",it_id.substr(4));
	    }
    	    if( mod->cntrIfCmd(dt_req) )	    
		mod->postMess(dt_req.attr("mcat").c_str(),dt_req.text().c_str(),TVision::Error,this);
	    else emit modifiedItem(it_own);
	}	
    }
}

void VisDevelop::visualItProp( )
{
    string prop_wdg=TSYS::strSepParse(work_wdg,0,';');
    
    string sel1 = TSYS::pathLev(prop_wdg,0);
    string sel2 = TSYS::pathLev(prop_wdg,1);    
    
    if( sel1.size() && !sel2.size() )
    {    
    	if(!prjLibPropDlg)
    	{
    	    prjLibPropDlg = new LibProjProp(this);
    	    connect(prjLibPropDlg, SIGNAL(apply(const string&)), this, SIGNAL(modifiedItem(const string&)));
	}
	prjLibPropDlg->showDlg(prop_wdg);
    }
    else
    {    
	if(!visItPropDlg)
	{
	    visItPropDlg = new VisItProp(this);
	    connect(visItPropDlg, SIGNAL(apply(const string &)), this, SIGNAL(modifiedItem(const string&)));
	}
	visItPropDlg->showDlg(prop_wdg);
    }
}

void VisDevelop::visualItEdit( )
{
    string ed_wdg;
    int w_cnt = 0;
    while((ed_wdg=TSYS::strSepParse(work_wdg,w_cnt++,';')).size())
    {
	QString w_title(QString(_("Widget: %1")).arg(ed_wdg.c_str()));
	//Check to already opened widget window
	QWidgetList ws_wdg = work_space->windowList();
	int i_w;
	for( i_w = 0; i_w < ws_wdg.size(); i_w++ )
	    if( ws_wdg.at(i_w)->windowTitle() == w_title )
	    {
		mod->postMess(mod->nodePath().c_str(),
		    QString(_("Widget's '%1' edit window already opened.")).
			    arg(ed_wdg.c_str()), TVision::Info, this );
		work_space->setActiveWindow(ws_wdg.at(i_w));
		break;
	    }
	if( i_w < ws_wdg.size() ) continue;

	QScrollArea *scrl = new QScrollArea;
	//scrl->setAlignment(Qt::AlignCenter);
	scrl->setBackgroundRole(QPalette::Dark);
	scrl->setAttribute(Qt::WA_DeleteOnClose);
	scrl->setWindowTitle(w_title);
	//- Set window icon -
	XMLNode prm_req("get");
	prm_req.setAttr("user",user())->setAttr("path",ed_wdg+"/%2fico");
        if( !mod->cntrIfCmd(prm_req) )
        {
    	    QImage ico_t;	
            string simg = TSYS::strDecode(prm_req.text(),TSYS::base64);
    	    if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
        	scrl->setWindowIcon(QPixmap::fromImage(ico_t));
        }	
	//- Make and place view widget -
	WdgView *vw = new WdgView(ed_wdg,0,true,this);
	connect(vw, SIGNAL(selected(const string&)), this, SLOT(selectItem(const string&)));    
	connect(this, SIGNAL(modifiedItem(const string&)), vw, SLOT(loadData(const string &)));
    
	scrl->setWidget( vw );
	work_space->addWindow(scrl);
	scrl->show();
	scrl->resize(vmax(300,vmin(800,vw->size().width()+10)),vmax(200,vmin(600,vw->size().height()+10)));
    }
}


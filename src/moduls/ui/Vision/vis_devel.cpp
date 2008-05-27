
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

#include <QApplication>
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
#include "vis_shapes.h"
#include "vis_devel_dlgs.h"
#include "vis_widgs.h"
#include "vis_run.h"
#include "vis_devel.h"
#include "vis_shape_elfig.h"

using namespace VISION;

VisDevelop::VisDevelop( const string &open_user, const string &VCAstat ) : 
    prjLibPropDlg(NULL), visItPropDlg(NULL), winClose(false), copy_buf("0")
{
    setAttribute(Qt::WA_DeleteOnClose,true);
#if QT_VERSION >= 0x040301
    setDockOptions(dockOptions() | QMainWindow::VerticalTabs);
#endif
    mod->regWin( this );

    setWindowTitle(_("Vision developing"));
    setWindowIcon(mod->icon());

    //- Init workspace -
    work_space = new QWorkspace(this);
    work_space->setScrollBarsEnabled(true);
    work_space->setBackground(QBrush(QColor(156,179,196),Qt::Dense2Pattern));
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
    if(!ico_t.load(TUIS::icoPath("load").c_str())) ico_t.load(":/images/load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t),_("Load from DB"),this);
    actDBLoad->setToolTip(_("Load item data from DB"));
    actDBLoad->setWhatsThis(_("The button for loading item data from DB"));
    actDBLoad->setStatusTip(_("Press for loading item data from DB."));
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(activated()), this, SLOT(itDBLoad()));
    //--- Save item to db ---
    if(!ico_t.load(TUIS::icoPath("save").c_str())) ico_t.load(":/images/save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t),_("Save to DB"),this);
    actDBSave->setToolTip(_("Save item data to DB"));
    actDBSave->setWhatsThis(_("The button for saving item data to DB"));
    actDBSave->setStatusTip(_("Press for saving item data to DB."));
    actDBSave->setShortcut(QKeySequence("Ctrl+S"));    
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
    actVisItAdd->setShortcut(QKeySequence("Ctrl+I"));
    actVisItAdd->setEnabled(false);
    actVisItAdd->setCheckable(true);
    //--- Visual item delete ---
    if(!ico_t.load(TUIS::icoPath("vision_it_del").c_str())) ico_t.load(":/images/it_del.png");
    actVisItDel = new QAction(QPixmap::fromImage(ico_t),_("Delete visual item"),this);
    actVisItDel->setToolTip(_("Delete visual item from library, container widget, project or page"));
    actVisItDel->setWhatsThis(_("The button for deleting visual item from library, container widget, project or page"));
    actVisItDel->setStatusTip(_("Press for deleting visual item library, container widget, project or page."));
    actVisItDel->setShortcut(QKeySequence("Ctrl+D"));
    actVisItDel->setEnabled(false);
    connect(actVisItDel, SIGNAL(activated()), this, SLOT(visualItDel()));
    //--- Visual item properties ---
    if(!ico_t.load(TUIS::icoPath("vision_it_prop").c_str())) ico_t.load(":/images/it_prop.png");
    actVisItProp = new QAction(QPixmap::fromImage(ico_t),_("Visual item properties"),this);
    actVisItProp->setToolTip(_("Get visual item properties"));
    actVisItProp->setWhatsThis(_("The button for getting of visual item properies"));
    actVisItProp->setStatusTip(_("Press for getting of visual item properies."));
    actVisItProp->setShortcut(QKeySequence("Ctrl+P"));
    actVisItProp->setEnabled(false);
    connect(actVisItProp, SIGNAL(activated()), this, SLOT(visualItProp()));
    //--- Visual item graphical edit ---
    if(!ico_t.load(TUIS::icoPath("vision_it_edit").c_str())) ico_t.load(":/images/it_edit.png");
    actVisItEdit = new QAction(QPixmap::fromImage(ico_t),_("Visual item edit"),this);
    actVisItEdit->setToolTip(_("Goes visual item edit"));
    actVisItEdit->setWhatsThis(_("The button for goes to visual item edition"));
    actVisItEdit->setStatusTip(_("Press for goes to visual item edition."));
    actVisItEdit->setShortcut(QKeySequence("Ctrl+E"));
    actVisItEdit->setEnabled(false);
    connect(actVisItEdit, SIGNAL(activated()), this, SLOT(visualItEdit()));
    //--- Cut visual item ---
    if(!ico_t.load(TUIS::icoPath("editcut").c_str())) ico_t.load(":/images/editcut.png");
    actVisItCut = new QAction(QPixmap::fromImage(ico_t),_("Visual item cut"),this);
    actVisItCut->setToolTip(_("Goes visual item cut"));
    actVisItCut->setWhatsThis(_("The button for goes to visual item cut"));
    actVisItCut->setStatusTip(_("Press for goes to visual item cut."));
    actVisItCut->setShortcut(QKeySequence("Ctrl+X"));
    actVisItCut->setEnabled(false);
    connect(actVisItCut, SIGNAL(activated()), this, SLOT(visualItCut()));
    //--- Copy visual item ---
    if(!ico_t.load(TUIS::icoPath("editcopy").c_str())) ico_t.load(":/images/editcopy.png");
    actVisItCopy = new QAction(QPixmap::fromImage(ico_t),_("Visual item copy"),this);
    actVisItCopy->setToolTip(_("Goes visual item copy"));
    actVisItCopy->setWhatsThis(_("The button for goes to visual item copy"));
    actVisItCopy->setStatusTip(_("Press for goes to visual item copy."));
    actVisItCopy->setShortcut(QKeySequence("Ctrl+C"));
    actVisItCopy->setEnabled(false);
    connect(actVisItCopy, SIGNAL(activated()), this, SLOT(visualItCopy()));
    //--- Paste visual item ---
    if(!ico_t.load(TUIS::icoPath("editpaste").c_str())) ico_t.load(":/images/editpaste.png");
    actVisItPaste = new QAction(QPixmap::fromImage(ico_t),_("Visual item paste"),this);
    actVisItPaste->setToolTip(_("Goes visual item paste"));
    actVisItPaste->setWhatsThis(_("The button for goes to visual item paste"));
    actVisItPaste->setStatusTip(_("Press for goes to visual item paste."));
    actVisItPaste->setShortcut(QKeySequence("Ctrl+V"));
    actVisItPaste->setEnabled(false);
    connect(actVisItPaste, SIGNAL(activated()), this, SLOT(visualItPaste()));

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
    if(!ico_t.load(TUIS::icoPath("vision_align_left").c_str())) ico_t.load(":/images/align_left.png");
    actAlignLeft = new QAction(QPixmap::fromImage(ico_t),_("Align to left"),this);
    actAlignLeft->setObjectName("align_left");
    actAlignLeft->setToolTip(_("Align selected widgets to left"));
    actAlignLeft->setWhatsThis(_("The button for align selected widgets to left"));
    actAlignLeft->setStatusTip(_("Press for align selected widgets to left."));
    actAlignLeft->setEnabled(false);
    //--- Align widgets to vertical center ---
    if(!ico_t.load(TUIS::icoPath("vision_align_vcenter").c_str())) ico_t.load(":/images/align_vcenter.png");
    actAlignVCenter = new QAction(QPixmap::fromImage(ico_t),_("Align to vertical center"),this);
    actAlignVCenter->setObjectName("align_vcenter");
    actAlignVCenter->setToolTip(_("Align selected widgets to vertical center"));
    actAlignVCenter->setWhatsThis(_("The button for align selected widgets to vertical center"));
    actAlignVCenter->setStatusTip(_("Press for align selected widgets to vertical center."));
    actAlignVCenter->setEnabled(false);
    //--- Align widgets to right ---
    if(!ico_t.load(TUIS::icoPath("vision_align_right").c_str())) ico_t.load(":/images/align_right.png");
    actAlignRight = new QAction(QPixmap::fromImage(ico_t),_("Align to right"),this);
    actAlignRight->setObjectName("align_right");
    actAlignRight->setToolTip(_("Align selected widgets to right"));
    actAlignRight->setWhatsThis(_("The button for align selected widgets to right"));
    actAlignRight->setStatusTip(_("Press for align selected widgets to right."));
    actAlignRight->setEnabled(false);
    //--- Align widgets to top ---
    if(!ico_t.load(TUIS::icoPath("vision_align_top").c_str())) ico_t.load(":/images/align_top.png");
    actAlignTop = new QAction(QPixmap::fromImage(ico_t),_("Align to top"),this);
    actAlignTop->setObjectName("align_top");
    actAlignTop->setToolTip(_("Align selected widgets to top"));
    actAlignTop->setWhatsThis(_("The button for align selected widgets to top"));
    actAlignTop->setStatusTip(_("Press for align selected widgets to top."));
    actAlignTop->setEnabled(false);
    //--- Align widgets to horizontal center ---
    if(!ico_t.load(TUIS::icoPath("vision_align_hcenter").c_str())) ico_t.load(":/images/align_hcenter.png");
    actAlignHCenter = new QAction(QPixmap::fromImage(ico_t),_("Align to horizontal center"),this);
    actAlignHCenter->setObjectName("align_hcenter");
    actAlignHCenter->setToolTip(_("Align selected widgets to horizontal center"));
    actAlignHCenter->setWhatsThis(_("The button for align selected widgets to horizontal center"));
    actAlignHCenter->setStatusTip(_("Press for align selected widgets to horizontal center."));
    actAlignHCenter->setEnabled(false);
    //--- Align widgets to bottom ---
    if(!ico_t.load(TUIS::icoPath("vision_align_bottom").c_str())) ico_t.load(":/images/align_bottom.png");
    actAlignBottom = new QAction(QPixmap::fromImage(ico_t),_("Align to bottom"),this);
    actAlignBottom->setObjectName("align_bottom");
    actAlignBottom->setToolTip(_("Align selected widgets to bottom"));
    actAlignBottom->setWhatsThis(_("The button for align selected widgets to bottom"));
    actAlignBottom->setStatusTip(_("Press for align selected widgets to bottom."));
    actAlignBottom->setEnabled(false);

    //-- Elementar figure actions --
    //--- Unset cursor ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_cursor").c_str())) ico_t.load(":/images/elfig_cursor.png");
    actElFigCursorAct = new QAction(QPixmap::fromImage(ico_t),_("Cursor"),this);
    actElFigCursorAct->setCheckable(true);
    actElFigCursorAct->setChecked(true);
    actElFigCursorAct->setObjectName("cursor");
    actElFigCursorAct->setToolTip(_("Unset cursor"));
    actElFigCursorAct->setWhatsThis(_("The button to unset cursor"));
    actElFigCursorAct->setStatusTip(_("Press for cursor to be unseted"));
    actElFigCursorAct->setEnabled(false);
    //--- Line creation ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_line").c_str())) ico_t.load(":/images/elfig_line.png");
    actElFigLine = new QAction(QPixmap::fromImage(ico_t),_("Add line"),this);
    //actElFigCursorAct->setCheckable(true);
    actElFigLine->setObjectName("line");
    actElFigLine->setToolTip(_("Add line to elementary figure"));
    actElFigLine->setWhatsThis(_("The button for adding line to elementary figure"));
    actElFigLine->setStatusTip(_("Press for add line to elementary figure."));
    actElFigLine->setEnabled(false);
    //--- Arc creation ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_arc").c_str())) ico_t.load(":/images/elfig_arc.png");
    actElFigArc = new QAction(QPixmap::fromImage(ico_t),_("Add arc"),this);
    //actElFigCursorAct->setCheckable(true);
    actElFigArc->setObjectName("arc");
    actElFigArc->setToolTip(_("Add arc to elementary figure"));
    actElFigArc->setWhatsThis(_("The button for adding arc to elementary figure"));
    actElFigArc->setStatusTip(_("Press for add arc to elementary figure."));
    actElFigArc->setEnabled(false);
    //--- Add Besie curve ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_besie").c_str())) ico_t.load(":/images/elfig_besie.png");
    actElFigBesie = new QAction(QPixmap::fromImage(ico_t),_("Add besier curve"),this);
    //actElFigCursorAct->setCheckable(true);
    actElFigBesie->setObjectName("besier");
    actElFigBesie->setToolTip(_("Add Besier curve to elementary figure"));
    actElFigBesie->setWhatsThis(_("The button for adding Besier curve to elementary figure"));
    actElFigBesie->setStatusTip(_("Press for add Besier curve to elementary figure."));
    actElFigBesie->setEnabled(false);
    //--- Add Hold points check ---
    if(!ico_t.load(TUIS::icoPath("vision_elfig_lock").c_str())) ico_t.load(":/images/elfig_lock.png");
    actElFigCheckAct=new QAction(QPixmap::fromImage(ico_t),_("Holds"),this);
    //actElFigCheckAct->setCheckable(true);
    actElFigCursorAct->setCheckable(true);
    actElFigCursorAct->setChecked(true);
    actElFigCheckAct->setObjectName("hold");
    actElFigCheckAct->setToolTip(_("Enable holds"));
    actElFigCheckAct->setWhatsThis(_("The button for enabling holds"));
    actElFigCheckAct->setStatusTip(_("Press for holds to be anabled"));
    actElFigCheckAct->setEnabled(false);

    //connect(checkAct, SIGNAL(toggled(bool)),this, SLOT(setHold()));
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
    mn_edit = menuBar()->addMenu(_("&Edit"));
    mn_edit->addAction(actVisItCut);    
    mn_edit->addAction(actVisItCopy);
    mn_edit->addAction(actVisItPaste);
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
    mn_widg->addSeparator();
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
    mn_widg->addSeparator();
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
    visItToolBar->addSeparator();
    visItToolBar->addAction(actVisItCut);
    visItToolBar->addAction(actVisItCopy);
    visItToolBar->addAction(actVisItPaste);
    mn_view->addAction(visItToolBar->toggleViewAction());
    mn_view->addSeparator();
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
    elFigTool->addAction(actElFigCursorAct);
    elFigTool->addAction(actElFigLine);
    elFigTool->addAction(actElFigArc);
    elFigTool->addAction(actElFigBesie);
    elFigTool->addAction(actElFigCheckAct);
    mn_view->addAction(elFigTool->toggleViewAction());
    mn_view->addSeparator();
    //- Init status bar -
    w_user = new UserStBar(open_user.c_str(), this);
    w_user->setWhatsThis(_("This label display curent user."));
    w_user->setToolTip(_("Field for display of the current user."));
    w_user->setStatusTip(_("Double click for change user."));
    statusBar()->insertPermanentWidget(0,w_user);
    w_stat = new QLabel(VCAstat.c_str(), this);
    w_stat->setWhatsThis(_("This label display used VCA engine station."));
    w_stat->setToolTip(_("Field for display of the used VCA engine station."));
    w_stat->setStatusTip(_("Double click for change VCA engine station."));
    statusBar()->insertPermanentWidget(0,w_stat);
    w_scale = new WScaleStBar( this );
    w_scale->setWhatsThis(_("This label display widgets' scaling mode."));
    w_scale->setToolTip(_("Field for display of widgets' scaling mode."));
    w_scale->setStatusTip(_("Click for change widgets' scaling mode."));
    statusBar()->insertPermanentWidget(0,w_scale);
    mStModify = new QLabel(" ",this);
    mStModify->setWhatsThis(_("This label display modifying."));
    mStModify->setToolTip(_("Field for display of modifying."));
    statusBar()->insertPermanentWidget(0,mStModify);

    //- Init dock windows -
    prjTree = new ProjTree(this);
    connect(this,SIGNAL(modifiedItem(const string&)),prjTree,SLOT(updateTree(const string&)));
    connect(prjTree,SIGNAL(selectItem(const string&,bool)),this,SLOT(selectItem(const string&,bool)));
    prjTree->setWhatsThis(_("Dock window for projects management."));
    wdgTree = new WdgTree(this);
    connect(this,SIGNAL(modifiedItem(const string&)),wdgTree,SLOT(updateTree(const string&)));
    connect(wdgTree,SIGNAL(selectItem(const string&,bool)),this,SLOT(selectItem(const string&,bool)));
    wdgTree->setWhatsThis(_("Dock window for widgets and this libraries management."));
    addDockWidget(Qt::LeftDockWidgetArea,prjTree);
    addDockWidget(Qt::LeftDockWidgetArea,wdgTree);
    tabifyDockWidget(prjTree,wdgTree);
    mn_view->addAction(prjTree->toggleViewAction());
    mn_view->addAction(wdgTree->toggleViewAction());
    //mn_view->addSeparator();

    attrInsp = new InspAttrDock(this);
    connect(attrInsp, SIGNAL(modified(const string &)), this, SIGNAL(modifiedItem(const string &)));
    connect(attrInsp, SIGNAL(modified(const string &)), this, SLOT(modifyToolUpdate(const string &)));
    attrInsp->setWhatsThis(_("Dock window for widget's attributes inspection."));
    lnkInsp  = new InspLnkDock(this);
    lnkInsp->setWhatsThis(_("Dock window for widget's links inspection."));
    addDockWidget(Qt::LeftDockWidgetArea,attrInsp);
    addDockWidget(Qt::LeftDockWidgetArea,lnkInsp);
    tabifyDockWidget(attrInsp,lnkInsp);
    mn_view->addAction(attrInsp->toggleViewAction());
    mn_view->addAction(lnkInsp->toggleViewAction());
    mn_view->addSeparator();

    //- Create timers -
    //-- Main widget's work timer --
    work_wdgTimer = new QTimer( this );
    work_wdgTimer->setSingleShot(true);
    work_wdgTimer->setInterval(200);
    connect(work_wdgTimer, SIGNAL(timeout()), this, SLOT(applyWorkWdg()));    
    //-- End run timer --
    endRunTimer   = new QTimer( this );
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);

    //resize( 1000, 800 );
    setWindowState(Qt::WindowMaximized);

    setVCAStation(VCAstat);

    connect(this, SIGNAL(modifiedItem(const string&)), this, SLOT(updateLibToolbar(const string&)));
    updateLibToolbar();
    wdgTree->updateTree();
    prjTree->updateTree();

    //Restore main window state
    string st = TSYS::strDecode(TBDS::genDBGet(mod->nodePath()+"devWinState","",user()),TSYS::base64);
    restoreState(QByteArray(st.data(),st.size()));

    //Hide specific tools
    wdgToolView->setVisible(false);
    elFigTool->setVisible(false);

    //w_stat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000 );
}

VisDevelop::~VisDevelop()
{
    winClose = true;

    //- Save main window state -
    QByteArray st = saveState();
    TBDS::genDBSet(mod->nodePath()+"devWinState",TSYS::strEncode(string(st.data(),st.size()),TSYS::base64),user());

    //- Timers stop -
    endRunTimer->stop();
    work_wdgTimer->stop();

    //- Other data clean -
    if( prjLibPropDlg )	delete prjLibPropDlg;
    if( visItPropDlg )	delete visItPropDlg;

    mod->unregWin(this);
}

void VisDevelop::setVCAStation( const string& st )
{
    m_stat = st.empty() ? "." : st;
}

int VisDevelop::cntrIfCmd( XMLNode &node, bool glob )
{
    if( VCAStation().empty() || VCAStation() == "." ) node.setAttr("user",user());
    return mod->cntrIfCmd(node,user(),VCAStation(),glob);
}

string VisDevelop::user()
{
    return w_user->user().toAscii().data();
}

bool VisDevelop::wdgScale( )
{ 
    return w_scale->scale();
}

void VisDevelop::setWdgScale( bool val )
{ 
    w_scale->setScale(val);
}

void VisDevelop::closeEvent( QCloseEvent* ce )
{
    winClose = true;

    work_space->closeAllWindows();

    if( !SYS->stopSignal() )	exitModifChk( );

    ce->accept();
}

void VisDevelop::exitModifChk( )
{
    XMLNode req("modify");
    req.setAttr("path","/%2fobj");
    if( !cntrIfCmd(req) && atoi(req.text().c_str()) )
    {
	bool saveExit = false;
	req.clear()->setName("get")->setAttr("path","/%2fgen%2fsaveExit");
	if( !cntrIfCmd(req,true) )   saveExit |= atoi(req.text().c_str());
	req.setAttr("path","/%2fgen%2fsavePeriod");
	if( !cntrIfCmd(req,true) )   saveExit |= atoi(req.text().c_str());
	if( !saveExit )
	{
	    InputDlg dlg(this,actDBSave->icon(),
		    _("Some visual items is changed. Save changing to DB on exit?"),
		    _("Visual items save"),false,false);
	    if( dlg.exec() == QDialog::Accepted )
	    {
		req.clear()->setName("save")->setAttr("path","/%2fobj");
		cntrIfCmd(req);
	    }
	}
    }
}

void VisDevelop::endRunChk( )
{
    if( mod->endRun() )	close();
}

void VisDevelop::quitSt()
{
    exitModifChk( );

    SYS->stop( );
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

void VisDevelop::updateLibToolbar( const string &vca_it )
{
    bool is_create, root_allow;
    int i_lb, i_t, i_m, i_a, i_w;
    vector<string> lbls;
    QImage ico_t;
    string t_el, simg;

    XMLNode req("get");

    //- Get elements number into VCA item -
    int vca_lev = 0;
    for( int off = 0; !(t_el=TSYS::pathLev(vca_it,0,true,&off)).empty(); )  vca_lev++;
    if( vca_lev == 3 )	return;
    string upd_lb   = (vca_lev>=1) ? TSYS::pathLev(vca_it,0).substr(4) : "";
    string upd_wdg  = (vca_lev>=2) ? TSYS::pathLev(vca_it,1).substr(4) : "";

    //- Update library toolbars list -
    XMLNode lb_req("get");    
    lb_req.setAttr("path","/%2fprm%2fcfg%2fwlb");
    if( !cntrIfCmd(lb_req) )
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
	if( !upd_lb.empty() && upd_lb != lbls[i_lb] ) continue;
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
	req.clear()->setAttr("path","/wlb_"+lbls[i_lb]+"/%2fico");
	if( !cntrIfCmd(req) )
	{
	    simg = TSYS::strDecode(req.text(),TSYS::base64);
	    if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		lb_menu[i_m]->setIcon(QPixmap::fromImage(ico_t));
	}
	//-- Get widget's actions list --
	vector<string> wdgls;
	lb_req.clear()->setAttr("path","/wlb_"+lbls[i_lb]+"/%2fwdg%2fwdg");
	if( !cntrIfCmd(lb_req) )
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
	    if( !upd_wdg.empty() && upd_wdg != wdgls[i_w] ) continue;
	    QAction *cur_act;
	    //--- Get parent name ---
	    string wipath = "/wlb_"+lbls[i_lb]+"/wdg_"+wdgls[i_w];
	    req.clear()->setAttr("path",wipath+"/%2fwdg%2fst%2fparent");
	    if( !cntrIfCmd(req) )
		if(!root_allow && req.text() == "root") root_allow = true;
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
	    req.clear()->setAttr("path",wipath+"/%2fico");
	    if( !cntrIfCmd(req) )
	    {
		simg = TSYS::strDecode(req.text(),TSYS::base64);
		if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    cur_act->setIcon(QPixmap::fromImage(ico_t));
	    }
	}
	if(is_create) lb_toolbar[i_t]->setVisible(root_allow);
    }
}

void VisDevelop::selectItem( const string &item, bool force )
{
    if( winClose )	return;

    work_wdg_new = item;
    if( force )	applyWorkWdg( );
    else work_wdgTimer->start( );
}

void VisDevelop::applyWorkWdg( )
{
    if( winClose )      return;
    bool isEn = false;

    modifyToolUpdate(work_wdg_new);

    //Set/update attributes inspector
    attrInsp->setWdg(work_wdg_new);
    lnkInsp->setWdg(work_wdg_new);

    //Update actions
    if( work_wdg == work_wdg_new )	return;
    work_wdg = work_wdg_new;

    string cur_wdg = TSYS::strSepParse(work_wdg,0,';');	//Get first select element
    string sel1 = TSYS::pathLev(cur_wdg,0);
    string sel2 = TSYS::pathLev(cur_wdg,1);
    string sel3 = TSYS::pathLev(cur_wdg,2);

    bool isProj = sel1.substr(0,4)=="prj_";
    bool isLib  = sel1.substr(0,4)=="wlb_";

    //- Process main actions -
    actPrjRun->setEnabled(isProj);

    //- Set visual item's actions -
    actVisItAdd->setEnabled(isProj || (isLib&&sel3.empty()));
    //-- Process add actions of visual items --
    for( int i_a = 0; i_a < actGrpWdgAdd->actions().size(); i_a++ )
	actGrpWdgAdd->actions().at(i_a)->setEnabled(isProj || (isLib&&sel3.empty()));
    //- Process visual item actions -
    actVisItDel->setEnabled(isProj || isLib);
    actVisItProp->setEnabled(isProj || isLib);
    actVisItEdit->setEnabled((isProj || isLib) && sel2.size());
    //- Edit actions update -
    editToolUpdate( );
}

void VisDevelop::modifyToolUpdate( const string &wdgs )
{
    actDBLoad->setEnabled(false); actDBSave->setEnabled(false);

    //- Request modify flag for select widgets -
    string cur_wdg;
    XMLNode req("modify");
    for( int i_off = 0; (cur_wdg=TSYS::strSepParse(wdgs,0,';',&i_off)).size(); )
    {
	req.setAttr("path",cur_wdg+"/%2fobj");
	if( !cntrIfCmd(req) && atoi(req.text().c_str()) )
	{
	    actDBLoad->setEnabled(true);
	    actDBSave->setEnabled(true);
	}
    }

    //- Request global VCA modify -
    mStModify->setText(" ");
    req.setAttr("path","/%2fobj");
    if( !cntrIfCmd(req) && atoi(req.text().c_str()) )
	mStModify->setText("*");
}

void VisDevelop::updateMenuWindow( )
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
	string cur_wdg;
	for( int i_off = 0; (cur_wdg=TSYS::strSepParse(own_wdg,0,';',&i_off)).size(); )
	{
	    //-- Send load request --
	    XMLNode req("load");
	    req.setAttr("path",cur_wdg+"/%2fobj");
	    if( cntrIfCmd(req) )
		mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	    else emit modifiedItem(cur_wdg);
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
	string cur_wdg;
	for( int i_off = 0; (cur_wdg=TSYS::strSepParse(own_wdg,0,';',&i_off)).size(); )
	{
	    //-- Send load request --
	    XMLNode req("save");
	    req.setAttr("path",cur_wdg+"/%2fobj");
	    if( cntrIfCmd(req) )
		mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	}
    }
}

void VisDevelop::prjRun( )
{
    string own_wdg = TSYS::strSepParse(work_wdg,0,';');

    VisRun *sess = new VisRun( own_wdg, user(), VCAStation() );
    sess->show();
    sess->raise();
    sess->activateWindow();
}

void VisDevelop::prjNew( )
{
    InputDlg dlg(this,actPrjNew->icon(),
	    _("Enter new project's identifier and name."),_("New project"),true,true);
    if( dlg.exec() == QDialog::Accepted )
    {
	XMLNode req("add");
	req.setAttr("path","/%2fprm%2fcfg%2fprj")->
	    setAttr("id",dlg.id().toAscii().data())->
	    setText(dlg.name().toAscii().data());
	if( cntrIfCmd(req) )
	    mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	else
	{
	    //-- Set enable for item container --
	    req.clear()->setName("set")->
			 setAttr("path",string("/prj_")+dlg.id().toAscii().data()+"/%2fobj%2fst%2fen")->
			 setText("1");
	    cntrIfCmd(req);
	    emit modifiedItem(string("prj_")+dlg.id().toAscii().data());
	}
    }
}

void VisDevelop::libNew( )
{
    InputDlg dlg(this,actPrjNew->icon(),
	    _("Enter new widget's library identifier and name."),_("New widget's library"),true,true);
    if( dlg.exec() == QDialog::Accepted )
    {
	XMLNode req("add");
	req.setAttr("path","/%2fprm%2fcfg%2fwlb")->
	    setAttr("id",dlg.id().toAscii().data())->
	    setText(dlg.name().toAscii().data());
	if( cntrIfCmd(req) )
	    mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	else
	{
	    //-- Set enable for item container --
	    req.clear()->setName("set")->
			 setAttr("path",string("/wlb_")+dlg.id().toAscii().data()+"/%2fobj%2fst%2fen")->
			 setText("1");
	    cntrIfCmd(req);
	    emit modifiedItem(string("wlb_")+dlg.id().toAscii().data());
	}
    }
}

void VisDevelop::visualItAdd( QAction *cact, const QPointF &pnt )
{
    XMLNode req("get");
    //QAction *cact = (QAction *)sender();
    string own_wdg = TSYS::strSepParse(work_wdg,0,';');
    string par_nm = cact->objectName().toAscii().data();

    if( work_space->activeWindow() && !wdgTree->hasFocus() && !prjTree->hasFocus() && pnt.isNull() &&
	    !((DevelWdgView*)((QScrollArea*)work_space->activeWindow())->widget())->edit()  )
	return;

    //- Count level -
    int p_el_cnt = 0;
    for( int i_off = 0; TSYS::pathLev(own_wdg,0,true,&i_off).size(); p_el_cnt++ ) ;
    string sid1 = TSYS::pathLev(own_wdg,0);    

    //- Make request id and name dialog -
    InputDlg dlg(this,cact->icon(),
	    _("Enter new widget/page identifier and name."),_("Create widget/page"),true,true);

    if( p_el_cnt > 1 )
    {
	//-- New include item id generator --
	//--- Present include widgets list request ---
	if( sid1.substr(0,4) == "prj_" && pnt.isNull() ) req.setAttr("path",own_wdg+"/%2fpage%2fpage");
	else req.setAttr("path",own_wdg+"/%2finclwdg%2fwdg");
	if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	else
	{
	    //--- Get parent widget id ---
	    string base_nm = "item";
	    if( !par_nm.empty() )	base_nm = TSYS::pathLev(par_nm,1,true).substr(4);
	    int i_c = 1, i_w = 0;
	    while( i_w < req.childSize() )
		if( req.childGet(i_w)->attr("id") == base_nm+TSYS::int2str(i_c) ) 
		{ i_w = 0; i_c++; }
		else i_w++;
	    dlg.setId((base_nm+TSYS::int2str(i_c)).c_str());
	}
    }

    //-- Execute dialog --
    if( dlg.exec() == QDialog::Accepted )
    {
	string w_id = dlg.id().toAscii().data();
	string w_nm = dlg.name().toAscii().data();

	//-- Check for widget's library --
	req.clear()->setName("add");
	string new_wdg;
	if( sid1.substr(0,4) == "wlb_" )
	{
	    if( p_el_cnt == 1 )
		req.setAttr("path",own_wdg+"/%2fwdg%2fwdg")->setAttr("id",w_id)->setText(w_nm);
	    else req.setAttr("path",own_wdg+"/%2finclwdg%2fwdg")->setAttr("id",w_id)->setText(w_nm);
	    new_wdg=own_wdg+"/wdg_"+w_id;
	}
	else if( sid1.substr(0,4) == "prj_" )
	{
	    if( pnt.isNull() )
	    {
		req.setAttr("path",own_wdg+"/%2fpage%2fpage")->setAttr("id",w_id)->setText(w_nm);
		new_wdg=own_wdg+"/pg_"+w_id;
	    }
	    else
	    {
		req.setAttr("path",own_wdg+"/%2finclwdg%2fwdg")->setAttr("id",w_id)->setText(w_nm);
		new_wdg=own_wdg+"/wdg_"+w_id;
	    }
	}
	//-- Create widget --
	int err = cntrIfCmd(req); 
	if( err ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	else
	{
	    //--- Set some parameters ---
	    req.clear()->setName("set");
	    if( !par_nm.empty() )
	    {
		//---- Set parent widget name and enable item ----
		req.setAttr("path",new_wdg+"/%2fwdg%2fst%2fparent")->setText(par_nm);
		err = cntrIfCmd(req);
		req.setAttr("path",new_wdg+"/%2fwdg%2fst%2fen")->setText("1");
		err = cntrIfCmd(req);
	    }
	    //---- Set geometry for include widget ----
	    if( !err && !pnt.isNull() )
	    {
		req.setAttr("path",new_wdg+"/%2fattr%2fgeomX")->setText(TSYS::real2str(pnt.x()));
		err = cntrIfCmd(req);
		req.setAttr("path",new_wdg+"/%2fattr%2fgeomY")->setText(TSYS::real2str(pnt.y()));
		err = cntrIfCmd(req);
	    }
	    if( err ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	    else emit modifiedItem(new_wdg);
	}
    }
    cact->setChecked(false);
}

void VisDevelop::visualItDel( const string &itms )
{
    string work_wdg_loc = itms.empty() ? work_wdg : itms;
    string del_wdg;

    //- Request to confirm -
    if( itms.empty() )
    {
	InputDlg dlg(this,actVisItDel->icon(),
		QString(_("You sure for delete visual items: '%1'?")).arg(work_wdg_loc.c_str()),
		_("Delete visual items"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }
    for( int w_off = 0; (del_wdg=TSYS::strSepParse(work_wdg_loc,0,';',&w_off)).size(); )
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

	XMLNode req("del");
	string sid1 = TSYS::pathLev(it_own,0);
	//Check for widget's library
	if( sid1.empty() )
	{
	    if( it_id.substr(0,4) == "wlb_" )
		req.setAttr("path","/%2fprm%2fcfg%2fwlb")->setAttr("id",it_id.substr(4));
	    else if( it_id.substr(0,4) == "prj_" )
		req.setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("id",it_id.substr(4));
	}
	else if( sid1.substr(0,4) == "wlb_" )
	{
	    if( p_el_cnt <= 2 )
		req.setAttr("path",it_own+"/%2fwdg%2fwdg")->setAttr("id",it_id.substr(4));
	    else req.setAttr("path",it_own+"/%2finclwdg%2fwdg")->setAttr("id",it_id.substr(4));
	}
	else if( sid1.substr(0,4) == "prj_" )
	{
	    if( p_el_cnt <= 2 )
		req.setAttr("path",it_own+"/%2fpage%2fpage")->setAttr("id",it_id.substr(3));
	    else if( it_id.substr(0,3) == "pg_" )
		req.setAttr("path",it_own+"/%2fpage%2fpage")->setAttr("id",it_id.substr(3));
	    else req.setAttr("path",it_own+"/%2finclwdg%2fwdg")->setAttr("id",it_id.substr(4));
	}
	if( cntrIfCmd(req) )
	    mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	else emit modifiedItem(del_wdg);
    }
}

void VisDevelop::visualItProp( )
{
    //if( work_wdg.empty() )	return;
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
    for( int w_off = 0; (ed_wdg=TSYS::strSepParse(work_wdg,0,';',&w_off)).size(); )
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
#if QT_VERSION >= 0x040400
	scrl->setAlignment(Qt::AlignCenter);
#endif
	QPalette plt = scrl->palette();
	plt.setBrush(QPalette::Window,QBrush("grey",Qt::Dense2Pattern));
	scrl->setPalette(plt);
	//scrl->setBackgroundRole(QPalette::Dark);
	scrl->setAttribute(Qt::WA_DeleteOnClose);
	scrl->setWindowTitle(w_title);
	//- Set window icon -
	XMLNode req("get");
	req.setAttr("path",ed_wdg+"/%2fico");
	if( !cntrIfCmd(req) )
	{
	    QImage ico_t;
	    string simg = TSYS::strDecode(req.text(),TSYS::base64);
	    if( ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		scrl->setWindowIcon(QPixmap::fromImage(ico_t));
	}
	//- Make and place view widget -
	DevelWdgView *vw = new DevelWdgView(ed_wdg,0,this);
	vw->load("");
	connect(vw, SIGNAL(selected(const string&)), this, SLOT(selectItem(const string&)));
	connect(vw, SIGNAL(apply(const string&)), this, SIGNAL(modifiedItem(const string&)));
	connect(this, SIGNAL(modifiedItem(const string&)), vw, SLOT(load(const string &)));

	scrl->setWidget( vw );
	scrl->resize(vmax(300,vmin(650,vw->size().width()+10)),vmax(200,vmin(550,vw->size().height()+10)));
	work_space->addWindow(scrl);
	scrl->show();
    }
}

void VisDevelop::visualItCut( )
{
    copy_buf = "1"+work_wdg;
    editToolUpdate();
}

void VisDevelop::visualItCopy( )
{
    copy_buf = "0"+work_wdg;
    editToolUpdate();
}

void VisDevelop::visualItPaste( )
{
    string copy_buf_el;
    string work_wdg_work = work_wdg;
    vector<string> copy_els, del_els;

    InputDlg dlg(this,actVisItPaste->icon(),"",_("Visual items move or copy"),true,true);
    for( int w_off = 0; (copy_buf_el=TSYS::strSepParse(copy_buf.substr(1),0,';',&w_off)).size(); )
    {
	string s_elp, d_elp, s_el, d_el, t_el, t1_el;
	//- Destination elements calc -
	int n_del = 0;
	for( int off = 0; !(t_el=TSYS::pathLev(work_wdg_work,0,true,&off)).empty(); n_del++ )
	{ if( n_del ) d_elp += ("/"+d_el); d_el = t_el; }
	//- Src elements calc -
	int n_sel = 0;
	for( int off = 0; !(t_el=TSYS::pathLev(copy_buf_el,0,true,&off)).empty(); n_sel++ )
	{ if( n_sel ) s_elp += ("/"+s_el); s_el = t_el; }

	//- Copy visual item -
	XMLNode req("get");
	//-- Project copy --
	if( s_el.substr(0,4)=="prj_" )
	{
	    t_el = (QString((copy_buf[0] == '1') ? _("Project '%1' move.\n") : _("Project '%1' copy.\n"))+
		_("Enter new project's identifier and name.")).arg(s_el.substr(4).c_str()).toAscii().data();
	    req.setAttr("path","/%2fprm%2fcfg%2fprj");
	    d_el = "prj_";
	    t1_el = s_el.substr(4);
	}
	//-- Widget's library copy --
	else if( s_el.substr(0,4)=="wlb_" )
	{
	    t_el = (QString((copy_buf[0] == '1') ? _("Widget's library '%1' move.\n") : _("Widget's library '%1' copy.\n"))+
		_("Enter new widget's library identifier and name.")).arg(s_el.substr(4).c_str()).toAscii().data();
	    req.setAttr("path","/%2fprm%2fcfg%2fwlb");
	    d_el = "wlb_"; 
	    t1_el = s_el.substr(4);
	}
	//-- Page copy --
	else if( s_el.substr(0,3)=="pg_" && (d_el.substr(0,4)=="prj_" || d_el.substr(0,3)=="pg_" || d_el.substr(0,4)=="wlb_") )
	{
	    t_el = (QString((copy_buf[0] == '1') ? _("Move page '%1' to '%2'.\n") : _("Copy page '%1' to '%2'.\n"))+
		_("Enter new widget/page identifier and name.")).arg(copy_buf_el.c_str()).arg(work_wdg_work.c_str()).toAscii().data();
	    if( d_el.substr(0,4)=="wlb_" ) 	req.setAttr("path",work_wdg_work+"/%2fwdg%2fwdg");
	    else req.setAttr("path",work_wdg_work+"/%2fpage%2fpage");
	    d_elp += ("/"+d_el);
	    d_el = (d_el.substr(0,4)=="wlb_") ? "wdg_" : "pg_";
	    t1_el = s_el.substr(3);
	}
	//-- Widget copy --
	else if( s_el.substr(0,4)=="wdg_" && (d_el.substr(0,3)=="pg_" || d_el.substr(0,4)=="wlb_" || (TSYS::pathLev(d_elp,0).substr(0,4)=="wlb_" && n_del==2)) )
	{
	    t_el = (QString((copy_buf[0] == '1') ? _("Move widget '%1' to '%2'.\n") : _("Copy widget '%1' to '%2'.\n"))+
		_("Enter new widget identifier and name.")).arg(copy_buf_el.c_str()).arg(work_wdg_work.c_str()).toAscii().data();
	    if( d_el.substr(0,4)=="wlb_" )	req.setAttr("path",work_wdg_work+"/%2fwdg%2fwdg");
	    else req.setAttr("path",work_wdg_work+"/%2finclwdg%2fwdg");
	    d_elp += ("/"+d_el);
	    d_el = "wdg_";
	    t1_el = s_el.substr(4);
	}
	//-- Copy scheme error --
	else 
	{
	    mod->postMess(mod->nodePath().c_str(),QString(_("Copy/move scheme from '%1' to '%2' no support.")).
		arg(copy_buf_el.c_str()).arg(work_wdg_work.c_str()),TVision::Error,this);
	    return;
	}
	//-- Prepare new widget identifier --
	//--- Remove digits from end of new identifier ---
	int no_numb = t1_el.size()-1;
	while( no_numb >= 0 && t1_el[no_numb]>='0' && t1_el[no_numb]<='9' ) no_numb--;
	if( no_numb >= 0 ) t1_el = t1_el.substr(0,no_numb+1);
	//--- New identifier generator ---
	if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	else
	{
	    int i_c = 1, i_w = 0;
	    while( i_w < req.childSize() )
		if( req.childGet(i_w)->attr("id") == t1_el+TSYS::int2str(i_c) ) { i_w = 0; i_c++; }
		else i_w++;
	    t1_el += TSYS::int2str(i_c);
	}	
	//-- Make request dialog --
	dlg.setMess(t_el.c_str());
	dlg.setId(t1_el.c_str());
	if( dlg.exec() == QDialog::Accepted )
	{
	    d_el += dlg.id().toAscii().data();
	    string it_nm = dlg.name().toAscii().data();
	
	    req.clear()->setName("set")->
		setAttr( "path", "/%2fprm%2fcfg%2fcp%2fcp" )->
		setAttr( "src", s_elp+"/"+s_el )->
		setAttr( "dst", d_elp+"/"+d_el );
	    if( cntrIfCmd(req) )
		mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	    else
	    {
		if( it_nm.size() )
		{
		    req.clear()->setName("set")->setText(it_nm);
		    if( d_el.substr(0,4) == "prj_" || d_el.substr(0,4) == "wlb_" )
			req.setAttr("path",d_elp+"/"+d_el+"/%2fobj%2fcfg%2fname");
		    else req.setAttr("path",d_elp+"/"+d_el+"/%2fwdg%2fcfg%2fname");
		    cntrIfCmd(req);
		}
		copy_els.push_back(d_elp+"/"+d_el);
		del_els.push_back(copy_buf_el);
	    }
	}
    }

    //- Send created widgets events-
    for( int i_e = 0; i_e < copy_els.size(); i_e++ )
	emit modifiedItem(copy_els[i_e]);
    //- Remove source widget -
    if( copy_buf[0] == '1' )
    {
	for( int i_e = 0; i_e < del_els.size(); i_e++ )
	    visualItDel(del_els[i_e]);
	copy_buf = "0";
    }
}

void VisDevelop::editToolUpdate( )
{
    actVisItCut->setEnabled(!work_wdg.empty());
    actVisItCopy->setEnabled(!work_wdg.empty());
    //- Src and destination elements calc -
    string s_elp, d_elp, s_el, d_el, t_el;
    int n_sel = 0;
    int n_del = 0;
    for( int off = 0; !(t_el=TSYS::pathLev(copy_buf.substr(1),0,true,&off)).empty(); n_sel++ )
    { s_elp += ("/"+s_el); s_el = t_el; }
    for( int off = 0; !(t_el=TSYS::pathLev(work_wdg,0,true,&off)).empty(); n_del++ )
    { d_elp += ("/"+d_el); d_el = t_el; }
    if( (s_el.substr(0,4)=="prj_" || s_el.substr(0,4)=="wlb_") ||										//Project and library copy
	    (s_el.substr(0,3)=="pg_" && (d_el.substr(0,4)=="prj_" || d_el.substr(0,3)=="pg_" || d_el.substr(0,4)=="wlb_")) ||			//Page copy
	    (s_el.substr(0,4)=="wdg_" && (d_el.substr(0,3)=="pg_" || d_el.substr(0,4)=="wlb_" || (TSYS::pathLev(d_elp,0).substr(0,4)=="wlb_" && n_del==2))) )	//Widget copy
	actVisItPaste->setEnabled(true);
    else actVisItPaste->setEnabled(false);
}

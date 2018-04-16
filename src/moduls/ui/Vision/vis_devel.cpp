
//OpenSCADA module UI.Vision file: vis_devel.cpp
/***************************************************************************
 *   Copyright (C) 2006-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <QMdiArea>
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
#include <QScrollBar>
#include <QCheckBox>
#include <QMdiSubWindow>

// #include <config.h>
#include <tsys.h>
#include "vis_shapes.h"
#include "vis_devel_dlgs.h"
#include "vis_widgs.h"
#include "vis_run.h"
#include "vis_devel.h"
#include "vis_shape_elfig.h"

using namespace VISION;

VisDevelop::VisDevelop( const string &open_user, const string &user_pass, const string &VCAstat ) :
    fileDlg(NULL), winClose(false), mWaitCursorSet(false), copy_buf("0"), prjLibPropDlg(NULL), visItPropDlg(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    connect(this, SIGNAL(makeStarterMenu()), qApp, SLOT(makeStarterMenu()));

    setDockOptions(dockOptions() | QMainWindow::VerticalTabs);
    mod->regWin(this);

    setWindowTitle(TSYS::strMess(_("OpenSCADA Vision developing: %s"),trU(SYS->name(),open_user).c_str()).c_str());
    setWindowIcon(mod->icon());

    //Init workspace
    work_space = new QMdiArea(this);
    //work_space->setScrollBarsEnabled(true);
    work_space->setBackground(QBrush(QColor(156,179,196)/*,Qt::Dense2Pattern*/));
    setCentralWidget(work_space);

    //Create actions
    // Generic actions
    //  Fullscreen
    actFullScr = new QAction(_("Full screen"),this);
    actFullScr->setCheckable(true);
    actFullScr->setToolTip(_("Full screen mode toggling"));
    actFullScr->setWhatsThis(_("The button for full screen mode toggling"));
    actFullScr->setStatusTip(_("Press for toggling the full screen mode."));
    connect(actFullScr, SIGNAL(toggled(bool)), this, SLOT(fullScreen(bool)));
    //  Close
    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("close",NULL,true).c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close the Vision window"));
    actClose->setWhatsThis(_("The button for closing the Vision developing window"));
    actClose->setStatusTip(_("Press to close of the current Vision developing window."));
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    //  Quit
    if(!ico_t.load(TUIS::icoGet("exit",NULL,true).c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit the program"));
    actQuit->setWhatsThis(_("The button for complete quit the program"));
    actQuit->setStatusTip(_("Press for complete quit the program."));
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quitSt()));
    //  About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button of the information of the program and OpenSCADA"));
    actAbout->setStatusTip(_("Press for information of the program and OpenSCADA."));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    //  About Qt
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using Qt information"));
    actQtAbout->setStatusTip(_("Press for getting the using Qt information."));
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    //  Vision manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    QAction *actManual = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(mod->modId().c_str()),this);
    actManual->setProperty("doc", "Modules/Vision|Modules/Vision");
    actManual->setShortcut(Qt::Key_F1);
    actManual->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press for getting the using '%1' manual.")).arg(mod->modId().c_str()));
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  VCAEngine manual
    QAction *actManualVCA = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg("VCAEngine"),this);
    actManualVCA->setProperty("doc", "Modules/VCAEngine|Modules/VCAEngine");
    actManualVCA->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg("VCAEngine"));
    actManualVCA->setStatusTip(QString(_("Press for getting the using '%1' manual.")).arg("VCAEngine"));
    connect(actManualVCA, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  OpenSCADA manual index
    QAction *actManualSYS = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(PACKAGE_STRING),this);
    actManualSYS->setProperty("doc", "index|Documents");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press for getting the using '%1' manual.")).arg(PACKAGE_STRING));
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setShortcut(Qt::SHIFT+Qt::Key_F1);
    actWhatIs->setToolTip(_("The button for requesting the information about GUI elements"));
    actWhatIs->setWhatsThis(_("Requesting the information about GUI elements"));
    actWhatIs->setStatusTip(_("Press for requesting the information about GUI elements."));
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    // Page, project, widget and this library actions
    //  Load item from db
    if(!ico_t.load(TUIS::icoGet("load",NULL,true).c_str())) ico_t.load(":/images/load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t),_("Load from the DB"),this);
    actDBLoad->setToolTip(_("Load the item data from the DB"));
    actDBLoad->setWhatsThis(_("The button for loading the item data from the DB"));
    actDBLoad->setStatusTip(_("Press for loading the item data from the DB."));
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(triggered()), this, SLOT(itDBLoad()));
    //  Save item to db
    if(!ico_t.load(TUIS::icoGet("save",NULL,true).c_str())) ico_t.load(":/images/save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t),_("Save to the DB"),this);
    actDBSave->setToolTip(_("Save the item data to the DB"));
    actDBSave->setWhatsThis(_("The button for saving the item data to the DB"));
    actDBSave->setStatusTip(_("Press for saving the item data to the DB."));
    actDBSave->setShortcut(QKeySequence("Ctrl+S"));
    actDBSave->setEnabled(false);
    connect(actDBSave, SIGNAL(triggered()), this, SLOT(itDBSave()));
    //  Start up the project execution
    if(!ico_t.load(TUIS::icoGet("vision_prj_run",NULL,true).c_str())) ico_t.load(":/images/prj_run.png");
    actPrjRun = new QAction(QPixmap::fromImage(ico_t),_("Run the project"),this);
    actPrjRun->setToolTip(_("Run the project execution for the selected item"));
    actPrjRun->setWhatsThis(_("The button for running the project execution for the selected item"));
    actPrjRun->setStatusTip(_("Press for running the project execution for the selected item."));
    actPrjRun->setEnabled(false);
    connect(actPrjRun, SIGNAL(triggered()), this, SLOT(prjRun()));
    //  Project create
    if(!ico_t.load(TUIS::icoGet("vision_prj_new",NULL,true).c_str())) ico_t.load(":/images/prj_new.png");
    actPrjNew = new QAction(QPixmap::fromImage(ico_t),_("New project"),this);
    actPrjNew->setToolTip(_("New project"));
    actPrjNew->setWhatsThis(_("The button for creation a new project"));
    actPrjNew->setStatusTip(_("Press for creation a new project."));
    connect(actPrjNew, SIGNAL(triggered()), this, SLOT(prjNew()));
    //  Widgets library new create
    if(!ico_t.load(TUIS::icoGet("vision_lib_new",NULL,true).c_str())) ico_t.load(":/images/lib_new.png");
    actLibNew = new QAction(QPixmap::fromImage(ico_t),_("New library"),this);
    actLibNew->setToolTip(_("New widgets library creation"));
    actLibNew->setWhatsThis(_("The button for creation a new widgets library"));
    actLibNew->setStatusTip(_("Press for creation a new widgets library."));
    connect(actLibNew, SIGNAL(triggered()), this, SLOT(libNew()));
    //  Visual item add (widget or page)
    if(!ico_t.load(TUIS::icoGet("vision_it_add",NULL,true).c_str())) ico_t.load(":/images/v_it_add.png");
    actVisItAdd = new QAction(QPixmap::fromImage(ico_t),_("Add a visual item"),this);
    actVisItAdd->setToolTip(_("Adding a visual item to the library, container widget, project or page"));
    actVisItAdd->setWhatsThis(_("The button for adding a visual item to the library, container widget, project or page"));
    actVisItAdd->setStatusTip(_("Press for adding a visual item to the library, container widget, project or page."));
    actVisItAdd->setShortcut(QKeySequence("Ctrl+I"));
    actVisItAdd->setEnabled(false);
    actVisItAdd->setCheckable(true);
    //  Visual item delete
    if(!ico_t.load(TUIS::icoGet("vision_it_del",NULL,true).c_str())) ico_t.load(":/images/v_it_del.png");
    actVisItDel = new QAction(QPixmap::fromImage(ico_t),_("Delete the visual item"),this);
    actVisItDel->setToolTip(_("Deleting the visual item from the library, container widget, project or page"));
    actVisItDel->setWhatsThis(_("The button for deleting the visual item from the library, container widget, project or page"));
    actVisItDel->setStatusTip(_("Press for deleting the visual item from the library, container widget, project or page."));
    actVisItDel->setShortcut(QKeySequence("Ctrl+D"));
    actVisItDel->setEnabled(false);
    connect(actVisItDel, SIGNAL(triggered()), this, SLOT(visualItDel()));
    //  Visual item properties
    if(!ico_t.load(TUIS::icoGet("vision_it_prop",NULL,true).c_str())) ico_t.load(":/images/v_it_prop.png");
    actVisItProp = new QAction(QPixmap::fromImage(ico_t),_("Properties of the visual item"),this);
    actVisItProp->setToolTip(_("Getting the properties of the visual item"));
    actVisItProp->setWhatsThis(_("The button for getting the properties of the visual item"));
    actVisItProp->setStatusTip(_("Press for getting the properties of the visual item."));
    actVisItProp->setShortcut(QKeySequence("Ctrl+P"));
    actVisItProp->setEnabled(false);
    connect(actVisItProp, SIGNAL(triggered()), this, SLOT(visualItProp()));
    //  Visual item graphical edit
    if(!ico_t.load(TUIS::icoGet("vision_it_edit",NULL,true).c_str())) ico_t.load(":/images/v_it_edit.png");
    actVisItEdit = new QAction(QPixmap::fromImage(ico_t),_("Edit the visual item"),this);
    actVisItEdit->setToolTip(_("Going to editing the visual item"));
    actVisItEdit->setWhatsThis(_("The button for going to editing the visual item"));
    actVisItEdit->setStatusTip(_("Press for going to editing the visual item."));
    actVisItEdit->setShortcut(QKeySequence("Ctrl+E"));
    actVisItEdit->setEnabled(false);
    connect(actVisItEdit, SIGNAL(triggered()), this, SLOT(visualItEdit()));
    //  Visual item changes clear
    if(!ico_t.load(TUIS::icoGet("vision_it_clear",NULL,true).c_str())) ico_t.load(":/images/v_it_clear.png");
    actVisItClear = new QAction(QPixmap::fromImage(ico_t),_("Clear the visual item changes"),this);
    actVisItClear->setToolTip(_("Clearing the visual items changes"));
    actVisItClear->setWhatsThis(_("The button for clearing the visual items changes"));
    actVisItClear->setStatusTip(_("Press for clearing the visual items changes."));
    actVisItClear->setEnabled(false);
    connect(actVisItClear, SIGNAL(triggered()), this, SLOT(visualItClear()));
    //  Visual item changes put down
    if(!ico_t.load(TUIS::icoGet("vision_it_ChDown",NULL,true).c_str())) ico_t.load(":/images/v_it_ChDown.png");
    actVisItChDown = new QAction(QPixmap::fromImage(ico_t),_("Lower down the visual item changes"),this);
    actVisItChDown->setToolTip(_("Lowering down the visual item changes to the parent"));
    actVisItChDown->setWhatsThis(_("The button for lowering down the visual item changes to the parent"));
    actVisItChDown->setStatusTip(_("Press for lowering down the visual item changes to the parent."));
    actVisItChDown->setEnabled(false);
    connect(actVisItChDown, SIGNAL(triggered()), this, SLOT(visualItDownParent()));
    //  UnDo visual item changes
    if(!ico_t.load(TUIS::icoGet("undo",NULL,true).c_str())) ico_t.load(":/images/undo.png");
    actVisItUnDo = new QAction(QPixmap::fromImage(ico_t),_("Undo the visual item changes"),this);
    actVisItUnDo->setObjectName("undo");
    actVisItUnDo->setToolTip(_("Undoing the visual item changes:"));
    actVisItUnDo->setWhatsThis(_("The button for undoing the visual item changes"));
    actVisItUnDo->setStatusTip(_("Press for undoing the visual item changes."));
    actVisItUnDo->setShortcut(QKeySequence("Ctrl+Z"));
    actVisItUnDo->setEnabled(false);
    //  ReDo visual item changes
    if(!ico_t.load(TUIS::icoGet("redo",NULL,true).c_str())) ico_t.load(":/images/redo.png");
    actVisItReDo = new QAction(QPixmap::fromImage(ico_t),_("Redo the visual item changes"),this);
    actVisItReDo->setObjectName("redo");
    actVisItReDo->setToolTip(_("Redoing the visual item changes:"));
    actVisItReDo->setWhatsThis(_("The button for redoing the visual item changes"));
    actVisItReDo->setStatusTip(_("Press for redoing the visual item changes."));
    actVisItReDo->setShortcut(QKeySequence("Ctrl+Y"));
    actVisItReDo->setEnabled(false);
    //  Copy visual item
    if(!ico_t.load(TUIS::icoGet("editcopy",NULL,true).c_str())) ico_t.load(":/images/editcopy.png");
    actVisItCopy = new QAction(QPixmap::fromImage(ico_t),_("Copy the visual item"),this);
    actVisItCopy->setObjectName("editcopy");
    actVisItCopy->setToolTip(_("Copying the visual item"));
    actVisItCopy->setWhatsThis(_("The button for copying the visual item"));
    actVisItCopy->setStatusTip(_("Press for copying the visual item."));
    actVisItCopy->setShortcut(QKeySequence("Ctrl+C"));
    actVisItCopy->setEnabled(false);
    connect(actVisItCopy, SIGNAL(triggered()), this, SLOT(visualItCopy()));
    //  Cut visual item
    if(!ico_t.load(TUIS::icoGet("editcut",NULL,true).c_str())) ico_t.load(":/images/editcut.png");
    actVisItCut = new QAction(QPixmap::fromImage(ico_t),_("Cut the visual item"),this);
    actVisItCut->setObjectName("editcut");
    actVisItCut->setToolTip(_("Cutting the visual item"));
    actVisItCut->setWhatsThis(_("The button for cutting the visual item"));
    actVisItCut->setStatusTip(_("Press for cutting the visual item."));
    actVisItCut->setShortcut(QKeySequence("Ctrl+X"));
    actVisItCut->setEnabled(false);
    connect(actVisItCut, SIGNAL(triggered()), this, SLOT(visualItCut()));
    //  Paste visual item
    if(!ico_t.load(TUIS::icoGet("editpaste",NULL,true).c_str())) ico_t.load(":/images/editpaste.png");
    actVisItPaste = new QAction(QPixmap::fromImage(ico_t),_("Paste the visual item"),this);
    actVisItPaste->setObjectName("editpaste");
    actVisItPaste->setToolTip(_("Pasting the visual item"));
    actVisItPaste->setWhatsThis(_("The button for pasting the visual item"));
    actVisItPaste->setStatusTip(_("Press for pasting the visual item."));
    actVisItPaste->setShortcut(QKeySequence("Ctrl+V"));
    actVisItPaste->setEnabled(false);
    connect(actVisItPaste, SIGNAL(triggered()), this, SLOT(visualItPaste()));

    // Widgets level actions
    //  Level up for widget
    if(!ico_t.load(TUIS::icoGet("vision_level_up",NULL,true).c_str())) ico_t.load(":/images/level_up.png");
    actLevUp = new QAction(QPixmap::fromImage(ico_t),_("Raise up to the top"),this);
    actLevUp->setObjectName("level_up");
    actLevUp->setToolTip(_("Raising up the selected widgets to the top"));
    actLevUp->setWhatsThis(_("The button for raising up the selected widgets to the top"));
    actLevUp->setStatusTip(_("Press for raising up the selected widgets to the top."));
    actLevUp->setEnabled(false);
    //  Level down for widget
    if(!ico_t.load(TUIS::icoGet("vision_level_down",NULL,true).c_str())) ico_t.load(":/images/level_down.png");
    actLevDown = new QAction(QPixmap::fromImage(ico_t),_("Lower down to the bottom"),this);
    actLevDown->setObjectName("level_down");
    actLevDown->setToolTip(_("Lowering down the selected widgets to the bottom"));
    actLevDown->setWhatsThis(_("The button for lowering down the selected widgets to the bottom"));
    actLevDown->setStatusTip(_("Press for lowering down the selected widgets to the bottom."));
    actLevDown->setEnabled(false);
    //  Rise widget
    if(!ico_t.load(TUIS::icoGet("vision_level_rise",NULL,true).c_str())) ico_t.load(":/images/level_rise.png");
    actLevRise = new QAction(QPixmap::fromImage(ico_t),_("Rise up"),this);
    actLevRise->setObjectName("level_rise");
    actLevRise->setToolTip(_("Raising up the selected widgets"));
    actLevRise->setWhatsThis(_("The button for raising up the selected widgets"));
    actLevRise->setStatusTip(_("Press for raising up the selected widgets."));
    actLevRise->setEnabled(false);
    //  Lower widget
    if(!ico_t.load(TUIS::icoGet("vision_level_lower",NULL,true).c_str())) ico_t.load(":/images/level_lower.png");
    actLevLower = new QAction(QPixmap::fromImage(ico_t),_("Lower down"),this);
    actLevLower->setObjectName("level_lower");
    actLevLower->setToolTip(_("Lowering down the selected widgets"));
    actLevLower->setWhatsThis(_("The button for lowering down the selected widgets"));
    actLevLower->setStatusTip(_("Press for lowering down the selected widgets."));
    actLevLower->setEnabled(false);

    // Widgets alignment actions
    //  Align widgets left
    if(!ico_t.load(TUIS::icoGet("vision_align_left",NULL,true).c_str())) ico_t.load(":/images/align_left.png");
    actAlignLeft = new QAction(QPixmap::fromImage(ico_t),_("Align to the left"),this);
    actAlignLeft->setObjectName("align_left");
    actAlignLeft->setToolTip(_("Aligning the selected widgets to the left"));
    actAlignLeft->setWhatsThis(_("The button for aligning the selected widgets to the left"));
    actAlignLeft->setStatusTip(_("Press for aligning the selected widgets to the left."));
    actAlignLeft->setEnabled(false);
    //  Align widgets to vertical center
    if(!ico_t.load(TUIS::icoGet("vision_align_vcenter",NULL,true).c_str())) ico_t.load(":/images/align_vcenter.png");
    actAlignVCenter = new QAction(QPixmap::fromImage(ico_t),_("Align vertically to the center"),this);
    actAlignVCenter->setObjectName("align_vcenter");
    actAlignVCenter->setToolTip(_("Aligning the selected widgets vertically to the center"));
    actAlignVCenter->setWhatsThis(_("The button for aligning the selected widgets vertically to the center"));
    actAlignVCenter->setStatusTip(_("Press for aligning the selected widgets vertically to the center."));
    actAlignVCenter->setEnabled(false);
    //  Align widgets to right
    if(!ico_t.load(TUIS::icoGet("vision_align_right",NULL,true).c_str())) ico_t.load(":/images/align_right.png");
    actAlignRight = new QAction(QPixmap::fromImage(ico_t),_("Align to the right"),this);
    actAlignRight->setObjectName("align_right");
    actAlignRight->setToolTip(_("Aligning the selected widgets to the right"));
    actAlignRight->setWhatsThis(_("The button for aligning the selected widgets to the right"));
    actAlignRight->setStatusTip(_("Press for aligning the selected widgets to the right."));
    actAlignRight->setEnabled(false);
    //  Align widgets to top
    if(!ico_t.load(TUIS::icoGet("vision_align_top",NULL,true).c_str())) ico_t.load(":/images/align_top.png");
    actAlignTop = new QAction(QPixmap::fromImage(ico_t),_("Align to the top"),this);
    actAlignTop->setObjectName("align_top");
    actAlignTop->setToolTip(_("Aligning the selected widgets to the top"));
    actAlignTop->setWhatsThis(_("The button for aligning the selected widgets to the top"));
    actAlignTop->setStatusTip(_("Press for aligning the selected widgets to the top."));
    actAlignTop->setEnabled(false);
    //  Align widgets to horizontal center
    if(!ico_t.load(TUIS::icoGet("vision_align_hcenter",NULL,true).c_str())) ico_t.load(":/images/align_hcenter.png");
    actAlignHCenter = new QAction(QPixmap::fromImage(ico_t),_("Align horizontally to the center"),this);
    actAlignHCenter->setObjectName("align_hcenter");
    actAlignHCenter->setToolTip(_("Aligning the selected widgets horizontally to the center"));
    actAlignHCenter->setWhatsThis(_("The button for aligning the selected widgets horizontally to the center"));
    actAlignHCenter->setStatusTip(_("Press for aligning the selected widgets horizontally to the center."));
    actAlignHCenter->setEnabled(false);
    //  Align widgets to bottom
    if(!ico_t.load(TUIS::icoGet("vision_align_bottom",NULL,true).c_str())) ico_t.load(":/images/align_bottom.png");
    actAlignBottom = new QAction(QPixmap::fromImage(ico_t),_("Align to the bottom"),this);
    actAlignBottom->setObjectName("align_bottom");
    actAlignBottom->setToolTip(_("Aligning the selected widgets to the bottom"));
    actAlignBottom->setWhatsThis(_("The button for aligning the selected widgets to the bottom"));
    actAlignBottom->setStatusTip(_("Press for aligning the selected widgets to the bottom."));
    actAlignBottom->setEnabled(false);

    // Elementar figure actions
    //  Unset cursor
    if(!ico_t.load(TUIS::icoGet("vision_elfig_cursor",NULL,true).c_str())) ico_t.load(":/images/elfig_cursor.png");
    actElFigCursorAct = new QAction(QPixmap::fromImage(ico_t),_("Return the cursor"),this);
    actElFigCursorAct->setCheckable(true);
    actElFigCursorAct->setChecked(true);
    actElFigCursorAct->setObjectName("cursor");
    actElFigCursorAct->setToolTip(_("Returning the cursor"));
    actElFigCursorAct->setWhatsThis(_("The button for returning the cursor"));
    actElFigCursorAct->setStatusTip(_("Press for returning the cursor."));
    actElFigCursorAct->setEnabled(false);
    //  Line creation
    if(!ico_t.load(TUIS::icoGet("vision_elfig_line",NULL,true).c_str())) ico_t.load(":/images/elfig_line.png");
    actElFigLine = new QAction(QPixmap::fromImage(ico_t),_("Add a line"),this);
    actElFigLine->setCheckable(true);
    actElFigLine->setObjectName("line");
    actElFigLine->setToolTip(_("Adding a line to the elementary figure"));
    actElFigLine->setWhatsThis(_("The button for adding a line to the elementary figure"));
    actElFigLine->setStatusTip(_("Press for adding a line to the elementary figure."));
    actElFigLine->setEnabled(false);
    //  Arc creation
    if(!ico_t.load(TUIS::icoGet("vision_elfig_arc",NULL,true).c_str())) ico_t.load(":/images/elfig_arc.png");
    actElFigArc = new QAction(QPixmap::fromImage(ico_t),_("Add an arc"),this);
    actElFigArc->setCheckable(true);
    actElFigArc->setObjectName("arc");
    actElFigArc->setToolTip(_("Adding an arc to the elementary figure"));
    actElFigArc->setWhatsThis(_("The button for adding an arc to the elementary figure"));
    actElFigArc->setStatusTip(_("Press for adding an arc to the elementary figure."));
    actElFigArc->setEnabled(false);
    //  Add Besie curve
    if(!ico_t.load(TUIS::icoGet("vision_elfig_besie",NULL,true).c_str())) ico_t.load(":/images/elfig_besie.png");
    actElFigBesie = new QAction(QPixmap::fromImage(ico_t),_("Add a Besier curve"),this);
    actElFigBesie->setCheckable(true);
    actElFigBesie->setObjectName("besier");
    actElFigBesie->setToolTip(_("Adding a Besier curve to the elementary figure"));
    actElFigBesie->setWhatsThis(_("The button for adding a Besier curve to the elementary figure"));
    actElFigBesie->setStatusTip(_("Press for adding a Besier curve to the elementary figure."));
    actElFigBesie->setEnabled(false);
    //  Add Hold points check
    if(!ico_t.load(TUIS::icoGet("vision_elfig_lock",NULL,true).c_str())) ico_t.load(":/images/elfig_lock.png");
    actElFigCheckAct=new QAction(QPixmap::fromImage(ico_t),_("Connections"),this);
    actElFigCheckAct->setCheckable(true);
    actElFigCheckAct->setChecked(true);
    actElFigCheckAct->setObjectName("hold");
    actElFigCheckAct->setToolTip(_("Enabling the connections"));
    actElFigCheckAct->setWhatsThis(_("The button for enabling the connections"));
    actElFigCheckAct->setStatusTip(_("Press for enabling the connections."));
    actElFigCheckAct->setEnabled(false);

    // MDI windows actions
    //  Close active window
    actWinClose = new QAction(_("Cl&ose"), this);
    actWinClose->setToolTip(_("Close the active window"));
    actWinClose->setWhatsThis(_("The button for closing the active window"));
    actWinClose->setStatusTip(_("Press for closing the active window."));
    connect(actWinClose, SIGNAL(triggered()), work_space, SLOT(closeActiveSubWindow()));
    //  Close all windows
    actWinCloseAll = new QAction(_("Close &All"), this);
    actWinCloseAll->setToolTip(_("Close all windows"));
    actWinCloseAll->setWhatsThis(_("The button for closing all windows"));
    actWinCloseAll->setStatusTip(_("Press for closing all windows."));
    connect(actWinCloseAll, SIGNAL(triggered()), work_space, SLOT(closeAllSubWindows()));
    //  Tile windows
    actWinTile = new QAction(_("&Tile"), this);
    actWinTile->setToolTip(_("Tile all windows"));
    actWinTile->setWhatsThis(_("The button for tiling all windows"));
    actWinTile->setStatusTip(_("Press for tiling all windows."));
    connect(actWinTile, SIGNAL(triggered()), work_space, SLOT(tileSubWindows()));
    //  Cascade windows
    actWinCascade = new QAction(_("&Cascade"), this);
    actWinCascade->setToolTip(_("Cascade all windows"));
    actWinCascade->setWhatsThis(_("The button for cascading all windows"));
    actWinCascade->setStatusTip(_("Press for cascading all windows."));
    connect(actWinCascade, SIGNAL(triggered()), work_space, SLOT(cascadeSubWindows()));
    //  Activate next window
    actWinNext = new QAction(_("Ne&xt"), this);
    actWinNext->setToolTip(_("Activate the next window"));
    actWinNext->setWhatsThis(_("The button for activating the next window"));
    actWinNext->setStatusTip(_("Press for activating the next window."));
    connect(actWinNext, SIGNAL(triggered()), work_space, SLOT(activateNextSubWindow()));
    //  Activate previous window
    actWinPrevious = new QAction(_("Pre&vious"), this);
    actWinPrevious->setToolTip(_("Activate the previous window"));
    actWinPrevious->setWhatsThis(_("The button for activating the previous window"));
    actWinPrevious->setStatusTip(_("Press for activating the previous window."));
    connect(actWinPrevious, SIGNAL(triggered()), work_space, SLOT(activatePreviousSubWindow()));

    //Action groups
    // Create widgets action groups
    actGrpWdgAdd = new QActionGroup(this);
    actGrpWdgAdd->setExclusive(true);
    connect(actGrpWdgAdd, SIGNAL(triggered(QAction*)), this, SLOT(visualItAdd(QAction*)));
    actGrpWdgAdd->addAction(actVisItAdd);

    //Create menu
    mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actDBLoad);
    mn_file->addAction(actDBSave);
    mn_file->addSeparator();
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);
    mn_edit = menuBar()->addMenu(_("&Edit"));
    mn_edit->addAction(actVisItUnDo);
    mn_edit->addAction(actVisItReDo);
    mn_edit->addSeparator();
    mn_edit->addAction(actVisItCopy);
    mn_edit->addAction(actVisItCut);
    mn_edit->addAction(actVisItPaste);
    mn_proj = menuBar()->addMenu(_("&Project"));
    mn_proj->addAction(actPrjRun);
    mn_proj->addSeparator();
    mn_proj->addAction(actPrjNew);
    mn_proj->addAction(actVisItAdd);
    mn_proj->addAction(actVisItDel);
    mn_proj->addAction(actVisItClear);
    mn_proj->addAction(actVisItChDown);
    mn_proj->addAction(actVisItProp);
    mn_proj->addAction(actVisItEdit);
    mn_widg = menuBar()->addMenu(_("&Widget"));
    mn_widg->addAction(actLibNew);
    mn_widg->addAction(actVisItAdd);
    mn_widg->addAction(actVisItDel);
    mn_widg->addAction(actVisItClear);
    mn_widg->addAction(actVisItChDown);
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
    connect(wMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));
    mn_view = menuBar()->addMenu(_("&View"));
    if(s2i(SYS->cmdOpt("showWin")) != 2) {
	mn_view->addAction(actFullScr);
	mn_view->addSeparator();
    }
    mn_help = menuBar()->addMenu(_("&Help"));
    mn_help->addAction(actAbout);
    mn_help->addAction(actQtAbout);
    mn_help->addAction(actManual);
    mn_help->addAction(actManualVCA);
    mn_help->addAction(actManualSYS);
    mn_help->addSeparator();
    mn_help->addAction(actWhatIs);
    // QTStarter
    emit makeStarterMenu();

    //Init tool bars
    // Visual items tools bar
    visItToolBar = new QToolBar(_("Visual items toolbar"),this);
    visItToolBar->setObjectName("visItToolBar");
    visItToolBar->setIconSize(QSize(icoSize(1.6),icoSize(1.6)));
    addToolBar(visItToolBar);
    visItToolBar->setMovable(true);
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
    visItToolBar->addAction(actVisItUnDo);
    visItToolBar->addAction(actVisItReDo);
    visItToolBar->addSeparator();
    visItToolBar->addAction(actVisItCopy);
    visItToolBar->addAction(actVisItCut);
    visItToolBar->addAction(actVisItPaste);
    mn_view->addAction(visItToolBar->toggleViewAction());
    mn_view->addSeparator();
    // Widget view functions toolbar
    wdgToolView = new QToolBar(_("Functions of the widgets visibility"),this);
    wdgToolView->setObjectName("wdgToolView");
    wdgToolView->setIconSize(QSize(icoSize(1.6),icoSize(1.6)));
    addToolBar(wdgToolView);
    wdgToolView->setMovable(true);
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
    // Elementar figure base widget tools
    elFigTool = new QToolBar(_("Elementary figure tools"),this);
    elFigTool->setObjectName("elFigTool");
    elFigTool->setIconSize(QSize(icoSize(1.6),icoSize(1.6)));
    addToolBar(elFigTool);
    elFigTool->setMovable(true);
    elFigTool->addAction(actElFigCursorAct);
    elFigTool->addAction(actElFigLine);
    elFigTool->addAction(actElFigArc);
    elFigTool->addAction(actElFigBesie);
    elFigTool->addAction(actElFigCheckAct);
    mn_view->addAction(elFigTool->toggleViewAction());
    mn_view->addSeparator();
    //Init status bar
    mWUser = new UserStBar(open_user.c_str(), user_pass.c_str(), VCAstat.c_str(), this);
    mWUser->setWhatsThis(_("This label displays the current user."));
    mWUser->setToolTip(_("Field for displaying the current user."));
    mWUser->setStatusTip(_("Double click to change the user."));
    statusBar()->insertPermanentWidget(0,mWUser);
    mWStat = new QLabel(VCAStation().c_str(), this);
    mWStat->setWhatsThis(_("This label displays the used VCA engine station."));
    mWStat->setToolTip(_("Field for displaying the used VCA engine station."));
    statusBar()->insertPermanentWidget(0, mWStat);
    w_scale = new WScaleStBar(this);
    w_scale->setWhatsThis(_("This label displays the widgets scaling mode."));
    w_scale->setToolTip(_("Field for displaying the widgets scaling mode."));
    w_scale->setStatusTip(_("Click to change the widgets scaling mode."));
    statusBar()->insertPermanentWidget(0,w_scale);
    mStModify = new WMdfStBar(this);
    connect(mStModify, SIGNAL(press()), this, SLOT(itDBSave()));
    mStModify->setWhatsThis(_("This label indicates the fact of making changes."));
    mStModify->setToolTip(_("Field for indicating the fact of making changes."));
    mStModify->setStatusTip(_("Click to save all the made changes."));
    statusBar()->insertPermanentWidget(0,mStModify);
    mWVisScale = new QLabel("100%", this);
    mWVisScale->setWhatsThis(_("This label displays the current widget scale."));
    mWVisScale->setToolTip(_("Field for displaying the current widget scale."));
    statusBar()->insertPermanentWidget(0,mWVisScale);

    //Init dock windows
    prjTree = new ProjTree(this);
    connect(this,SIGNAL(modifiedItem(const string&)),prjTree,SLOT(updateTree(const string&)));
    connect(prjTree,SIGNAL(selectItem(const string&,bool)),this,SLOT(selectItem(const string&,bool)));
    prjTree->setWhatsThis(_("Dock window of the projects manager."));
    wdgTree = new WdgTree(this);
    connect(this,SIGNAL(modifiedItem(const string&)),wdgTree,SLOT(updateTree(const string&)));
    connect(wdgTree,SIGNAL(selectItem(const string&,bool)),this,SLOT(selectItem(const string&,bool)));
    wdgTree->setWhatsThis(_("Dock window of the manager of the widgets libraries."));
    addDockWidget(Qt::LeftDockWidgetArea, prjTree);
    addDockWidget(Qt::LeftDockWidgetArea, wdgTree);
    tabifyDockWidget(prjTree, wdgTree);
    mn_view->addAction(prjTree->toggleViewAction());
    mn_view->addAction(wdgTree->toggleViewAction());
    //mn_view->addSeparator();

    attrInsp = new InspAttrDock(this);
    connect(attrInsp, SIGNAL(modified(const string &)), this, SIGNAL(modifiedItem(const string &)));
    attrInsp->setWhatsThis(_("Dock window of the inspector ot the widget attributes."));
    lnkInsp  = new InspLnkDock(this);
    lnkInsp->setWhatsThis(_("Dock window of the inspector ot the widget links."));
    addDockWidget(Qt::LeftDockWidgetArea, attrInsp);
    addDockWidget(Qt::LeftDockWidgetArea, lnkInsp);
    tabifyDockWidget(attrInsp, lnkInsp);
    mn_view->addAction(attrInsp->toggleViewAction());
    mn_view->addAction(lnkInsp->toggleViewAction());
    mn_view->addSeparator();

    connect(this, SIGNAL(modifiedItem(const string&)), this, SLOT(modifyGlbStUpdate()));

    //Create timers
    // Main widget's work timer
    work_wdgTimer = new QTimer(this);
    work_wdgTimer->setSingleShot(true);
    work_wdgTimer->setInterval(200);
    connect(work_wdgTimer, SIGNAL(timeout()), this, SLOT(applyWorkWdg()));
    // End run timer
    endRunTimer   = new QTimer(this);
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);
    // Wait cursor clean up timer
    waitCursorClear = new QTimer(this);
    waitCursorClear->setSingleShot(true);
    waitCursorClear->setInterval(50);
    connect(waitCursorClear, SIGNAL(timeout()), SLOT(waitCursorSet()));

    //if(!s2i(SYS->cmdOpt("showWin")))	resize(1000, 800);
    //setWindowState(Qt::WindowMaximized);

    //menuBar()->setVisible(true);	//!!!! Spare for Qt5 and the native menu bar

    wdgTree->updateTree("", true);	//Initial for allow the widgets loading on the server side mostly
    prjTree->updateTree("", NULL, true);//Initial for allow the projects loading on the server side mostly

    //Restore main window state
    int off = 0;
    string rst = mod->uiPropGet("devWinState", user());
    string sRst = TSYS::strDecode(TSYS::strParse(rst,0,":",&off), TSYS::base64);
    if(sRst.size()) restoreState(QByteArray(sRst.data(),sRst.size()));
    int	wH = s2i(TSYS::strParse(rst,0,":",&off)),
	wW = s2i(TSYS::strParse(rst,0,":",&off));
    if(!s2i(SYS->cmdOpt("showWin")) && wH > 100 && wW > 100) resize(wH, wW);

    //Restore ToolBars icons size
    for(int iCh = 0; iCh < children().size(); iCh++) {
	if(!qobject_cast<QToolBar*>(children()[iCh])) continue;
	int icSz = s2i(mod->uiPropGet((children()[iCh]->objectName()+"_icSz").toStdString(),user()));
	if(icSz) ((QToolBar*)children()[iCh])->setIconSize(QSize(icSz,icSz));
    }

    //Hide specific tools
    wdgToolView->setVisible(false);
    elFigTool->setVisible(false);

    //mWStat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000);
}

VisDevelop::~VisDevelop( )
{
    winClose = true;

    //Save main window state
    QByteArray st = saveState();
    mod->uiPropSet("devWinState", TSYS::strEncode(string(st.data(),st.size()),TSYS::base64,"")+":"+i2s(width())+":"+i2s(height()), user());

    //Timers stop
    endRunTimer->stop();
    work_wdgTimer->stop();

    //Other data clean
    if(prjLibPropDlg)	delete prjLibPropDlg;
    if(visItPropDlg)	delete visItPropDlg;
    if(fileDlg)		delete fileDlg;

    mod->unregWin(this);
    waitCursorSet(-1);
}

int VisDevelop::cntrIfCmd( XMLNode &node, bool glob )
{
    //Mark commands in "primaryCmd", for redundant hosts mostly transfer
    // !!! Move further to the command's source
    if(node.name() == "set" || node.name() == "add" || node.name() == "ins" || node.name() == "del" || node.name() == "move" ||
	    node.name() == "load" || node.name() == "save" || node.name() == "copy")
	node.setAttr("primaryCmd", "1");

    waitCursorSet(1);	//QApplication::setOverrideCursor(Qt::WaitCursor);
    int rez = mod->cntrIfCmd(node,user(),password(),VCAStation(),glob);
    waitCursorSet(0);	//QApplication::restoreOverrideCursor();

    return rez;
}

QString VisDevelop::getFileName(const QString &caption, const QString &dir, const QString &filter, QFileDialog::AcceptMode mode)
{
    if(!fileDlg) fileDlg = new QFileDialog(this);
    fileDlg->setFileMode(QFileDialog::AnyFile);
    fileDlg->setAcceptMode(mode);
    fileDlg->setWindowTitle(caption);
    fileDlg->setNameFilter(filter);
    if(dir.size()) { QString dirF = dir; fileDlg->selectFile(dirF.replace("\"","")); }
    if(fileDlg->exec() && !fileDlg->selectedFiles().empty()) return fileDlg->selectedFiles()[0];

    return "";
}

string VisDevelop::user( )	{ return mWUser->user(); }

string VisDevelop::password( )	{ return mWUser->pass(); }

string VisDevelop::VCAStation( )	{ return mWUser->VCAStation(); }

bool VisDevelop::wdgScale( )		{ return w_scale->scale(); }

void VisDevelop::setWdgScale( bool val ){ w_scale->setScale(val); }

double VisDevelop::wdgVisScale( )	{ return s2r(mWVisScale->text().toStdString()); }

void VisDevelop::setWdgVisScale( double val )
{
    mWVisScale->setText((r2s(rRnd(val*100,POS_PREC_DIG,true))+"%").c_str());
}

void VisDevelop::closeEvent( QCloseEvent* ce )
{
    winClose = true;

    if(!SYS->stopSignal() && !property("forceClose").toBool() && !mod->endRun() && !exitModifChk())
    {
	ce->ignore();
	winClose = false;
	return;
    }

    work_space->closeAllSubWindows();
    ce->accept();
}

QMenu *VisDevelop::createPopupMenu( )
{
    QMenu *mn = QMainWindow::createPopupMenu( );
    if(!mn) return mn;

    //Check for widget under cursor
    QWidget *ucw = childAt(mapFromGlobal(QCursor::pos()));
    if(qobject_cast<QToolBar*>(ucw) && !mn->children().isEmpty()) {
	QAction *first = mn->actions().isEmpty() ? NULL : mn->actions()[0];
	QMenu *iSz = new QMenu(_("Icons size"));
	mn->insertMenu(first, iSz);
	mn->insertSeparator(first);

	QAction *act = new QAction(_("Small (16x16)"), iSz);
	connect(act, SIGNAL(triggered()), this, SLOT(setToolIconSize()));
	act->setObjectName("16");
	act->setProperty("toolAddr", TSYS::addr2str(ucw).c_str());
	iSz->addAction(act);

	act = new QAction(_("Medium (22x22)"), iSz);
	connect(act, SIGNAL(triggered()), this, SLOT(setToolIconSize()));
	act->setObjectName("22");
	act->setProperty("toolAddr", TSYS::addr2str(ucw).c_str());
	iSz->addAction(act);

	act = new QAction(_("Big (32x32)"), iSz);
	connect(act, SIGNAL(triggered()), this, SLOT(setToolIconSize()));
	act->setObjectName("32");
	act->setProperty("toolAddr", TSYS::addr2str(ucw).c_str());
	iSz->addAction(act);

	act = new QAction(_("Huge (48x48)"), iSz);
	connect(act, SIGNAL(triggered()), this, SLOT(setToolIconSize()));
	act->setObjectName("32");
	act->setProperty("toolAddr", TSYS::addr2str(ucw).c_str());
	iSz->addAction(act);
    }

    return mn;
}

void VisDevelop::setToolIconSize( )
{
    if(!sender()) return;

    QToolBar *tb = qobject_cast<QToolBar*>((QToolBar*)TSYS::str2addr(sender()->property("toolAddr").toString().toStdString()));
    int icSz = s2i(sender()->objectName().toStdString());
    if(tb) {
	tb->setIconSize(QSize(icSz,icSz));
	mod->uiPropSet((tb->objectName()+"_icSz").toStdString(), i2s(icSz), user());
    }
}

void VisDevelop::setActiveSubWindow( QWidget *w )	{ work_space->setActiveSubWindow(dynamic_cast<QMdiSubWindow *>(w)); }

void VisDevelop::fullScreen( bool vl )
{
    setWindowState(vl?Qt::WindowFullScreen:Qt::WindowNoState);
    //!!!! But switching to the WindowMaximized performs only through the WindowNoState
    if(!vl && s2i(SYS->cmdOpt("showWin"))) setWindowState(Qt::WindowMaximized);
}

bool VisDevelop::exitModifChk( )
{
    XMLNode req("modify");
    req.setAttr("path","/%2fobj");
    if(!cntrIfCmd(req) && s2i(req.text())) {
	bool saveExit = false;
	req.clear()->setName("get")->setAttr("path","/%2fgen%2fsaveExit");
	if(!cntrIfCmd(req,true)) saveExit |= s2i(req.text());
	req.setAttr("path","/%2fgen%2fsavePeriod");
	if(!cntrIfCmd(req,true)) saveExit |= s2i(req.text());
	if(!saveExit)
	    switch(QMessageBox::information(this,_("Saving of the visual items"),_("Some visual items have been changed.\nSave the changes to the DB before exiting?"),
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Yes))
	    {
		case QMessageBox::Yes:
		    req.clear()->setName("save")->setAttr("path","/%2fobj");
		    cntrIfCmd(req);
		    return true;
		case QMessageBox::No:		return true;
		case QMessageBox::Cancel:	return false;
		default: break;
	    }
    }

    return true;
}

void VisDevelop::waitCursorSet( int val )
{
    //Set
    if(val == 1) {
	if(!mWaitCursorSet) QApplication::setOverrideCursor(Qt::WaitCursor);
	mWaitCursorSet = true;
	waitCursorClear->stop();
    }
    //Clear cursor command
    else if(val == 0 && mWaitCursorSet) waitCursorClear->start();
    //Real clear after the timer shot
    else if(val == -1 && mWaitCursorSet) { QApplication::restoreOverrideCursor(); mWaitCursorSet = false; }
}

void VisDevelop::endRunChk( )
{
    if(mod->endRun())	close();
}

void VisDevelop::quitSt( )
{
    if(exitModifChk())	SYS->stop( );
}

void VisDevelop::about( )
{
    QMessageBox::about(this,windowTitle(),
	QString(_("%1 v%2.\n%3\nAuthor: %4\nLicense: %5\n\n%6 v%7.\n%8\nLicense: %9\nAuthor: %10\nWeb site: %11")).
	    arg(mod->modInfo("Name").c_str()).arg(mod->modInfo("Version").c_str()).arg(mod->modInfo("Description").c_str()).
	    arg(mod->modInfo("Author").c_str()).arg(mod->modInfo("License").c_str()).
	    arg(PACKAGE_NAME).arg(VERSION).arg(_(PACKAGE_DESCR)).arg(PACKAGE_LICENSE).arg(_(PACKAGE_AUTHOR)).arg(PACKAGE_SITE));
}

void VisDevelop::aboutQt( )		{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void VisDevelop::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void VisDevelop::selectItem( const string &item, bool force )
{
    if(winClose) return;

    work_wdg_new = item;
    if(force)	applyWorkWdg();
    else work_wdgTimer->start();
}

void VisDevelop::applyWorkWdg( )
{
    if(winClose) return;

    modifyToolUpdate(work_wdg_new);
    modifyGlbStUpdate(true);

    //Set/update attributes inspector
    attrInsp->setWdg(work_wdg_new);
    lnkInsp->setWdg(work_wdg_new);

    //Update actions
    if(work_wdg == work_wdg_new) return;
    work_wdg = work_wdg_new;

    string cur_wdg = TSYS::strSepParse(work_wdg,0,';');	//Get first select element
    string sel1 = TSYS::pathLev(cur_wdg,0);
    string sel2 = TSYS::pathLev(cur_wdg,1);
    string sel3 = TSYS::pathLev(cur_wdg,2);

    bool isProj = (sel1.substr(0,4)=="prj_");
    bool isLib  = (sel1.substr(0,4)=="wlb_");

    //Process main actions
    actPrjRun->setEnabled(isProj);

    //Set visual item's actions
    actVisItAdd->setEnabled(isProj || (isLib&&sel3.empty()));
    // Process add actions of visual items
    for(int i_a = 0; i_a < actGrpWdgAdd->actions().size(); i_a++)
	actGrpWdgAdd->actions().at(i_a)->setEnabled(isProj || (isLib&&sel3.empty()));
    //Process visual item actions
    actVisItDel->setEnabled(isProj || isLib);
    actVisItProp->setEnabled(isProj || isLib);
    actVisItEdit->setEnabled((isProj || isLib) && sel2.size());
    actVisItClear->setEnabled((isProj || isLib) && sel2.size());
    actVisItChDown->setEnabled((isProj || isLib) && sel2.size());

    //Edit actions update
    editToolUpdate();
}

void VisDevelop::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString());
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("The manual '%1' was not found offline or online!")).arg(sender()->property("doc").toString()));
}

void VisDevelop::modifyToolUpdate( const string &wdgs )
{
    actDBLoad->setEnabled(false); actDBSave->setEnabled(false);

    //Request modify flag for select widgets
    string cur_wdg;
    XMLNode req("modify");
    for(int i_off = 0; (cur_wdg=TSYS::strSepParse(wdgs,0,';',&i_off)).size(); ) {
	req.setAttr("path",cur_wdg+"/%2fobj");
	if(!cntrIfCmd(req) && s2i(req.text())) {
	    actDBLoad->setEnabled(true);
	    actDBSave->setEnabled(true);
	}
    }
}

void VisDevelop::modifyGlbStUpdate( bool check )
{
    if(!check) mStModify->setText("*");
    else {
        //Request global VCA modify
	XMLNode req("modify");
	req.setAttr("path", "/%2fobj");
	mStModify->setText((!cntrIfCmd(req) && s2i(req.text()))?"*":" ");
    }
}

void VisDevelop::updateMenuWindow( )
{
    mn_window->clear();
    //Add actions to menu
    mn_window->addAction(actWinClose);
    mn_window->addAction(actWinCloseAll);
    mn_window->addSeparator();
    mn_window->addAction(actWinTile);
    mn_window->addAction(actWinCascade);
    mn_window->addSeparator();
    mn_window->addAction(actWinNext);
    mn_window->addAction(actWinPrevious);

    QList<QMdiSubWindow *> windows = work_space->subWindowList();
    //Enable action state
    QMdiSubWindow *act_win = work_space->activeSubWindow();
    actWinClose->setEnabled(act_win);
    actWinCloseAll->setEnabled(!windows.isEmpty());
    actWinTile->setEnabled(!windows.isEmpty());
    actWinCascade->setEnabled(!windows.isEmpty());
    actWinNext->setEnabled(act_win&&windows.size()>1);
    actWinPrevious->setEnabled(act_win&&windows.size()>1);

    //Add windows list
    if(!windows.isEmpty()) mn_window->addSeparator();
    for(int i_w = 0; i_w < windows.size(); ++i_w) {
	QMdiSubWindow *child = windows.at(i_w);
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
    if(own_wdg.empty()) return;

    //Request to confirm
    InputDlg dlg(this, actDBLoad->icon(),
	    QString(_("Are you sure of loading visual items '%1' from the DB?")).arg(QString(own_wdg.c_str()).replace(";","; ")),
	    _("Loading the visual items data from the DB"), false, false);
    if(dlg.exec() == QDialog::Accepted) {
	string cur_wdg;
	for(int i_off = 0; (cur_wdg=TSYS::strSepParse(own_wdg,0,';',&i_off)).size(); ) {
	    // Send load request
	    XMLNode req("load");
	    req.setAttr("path",cur_wdg+"/%2fobj");
	    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	    else emit modifiedItem(cur_wdg);
	}
    }
}

void VisDevelop::itDBSave( )
{
    string own_wdg = work_wdg;

    if(sender() == mStModify) {
	if(mStModify->text() != "*") return;
	own_wdg = "/";
    }
    if(!own_wdg.empty()) {
	//Request to confirm
	InputDlg dlg(this, actDBSave->icon(),
		(own_wdg == "/" ? QString(_("Are you sure of saving all modifications to the DB?")) :
		QString(_("Are you sure of saving the visual items '%1' to the DB?")).arg(QString(own_wdg.c_str()).replace(";","; "))),
		_("Saving the visual items data to the DB"), false, false);
	if(dlg.exec() == QDialog::Accepted) {
	    string cur_wdg;
	    for(int i_off = 0; (cur_wdg=TSYS::strSepParse(own_wdg,0,';',&i_off)).size(); ) {
		// Send load request
		XMLNode req("save");
		req.setAttr("path", cur_wdg+"/%2fobj");
		if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	    }
	}
    }
    modifyToolUpdate(own_wdg);
    modifyGlbStUpdate(true);
}

void VisDevelop::prjRun( )
{
    string own_wdg = TSYS::strSepParse(work_wdg, 0, ';');

    VisRun *sess = new VisRun(own_wdg, user(), password(), VCAStation());
    switch(s2i(SYS->cmdOpt("showWin"))) {
	case 1:	sess->showMaximized();	break;
	case 2:	sess->showFullScreen();	break;
	default: {
	    sess->show();
	    sess->raise();
	}
    }
    sess->activateWindow();
}

void VisDevelop::prjNew( )
{
    InputDlg dlg(this, actPrjNew->icon(), _("Enter the identifier and the name of the new project."), _("New project"), true, true);
    dlg.setIdLen(30);
    if(dlg.exec() == QDialog::Accepted) {
	XMLNode req("add");
	req.setAttr("path", "/%2fprm%2fcfg%2fprj")->setAttr("id", dlg.id().toStdString())->setText(dlg.name().toStdString());
	if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	else {
	    string nId = req.attr("id");
	    // Set enable for item container
	    req.clear()->setName("set")->setAttr("path", "/prj_"+nId+"/%2fobj%2fst%2fen")->setText("1");
	    cntrIfCmd(req);
	    emit modifiedItem("prj_"+nId);
	}
    }
}

void VisDevelop::libNew( )
{
    InputDlg dlg(this, actPrjNew->icon(), _("Enter the identifier and the name of the new widgets library."), _("New widgets library"), true, true);
    dlg.setIdLen(30);
    if(dlg.exec() == QDialog::Accepted) {
	XMLNode req("add");
	req.setAttr("path", "/%2fprm%2fcfg%2fwlb")->setAttr("id",dlg.id().toStdString())->setText(dlg.name().toStdString());
	if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	else {
	    string nId = req.attr("id");
	    // Set enable for item container
	    req.clear()->setName("set")->setAttr("path", "/wlb_"+nId+"/%2fobj%2fst%2fen")->setText("1");
	    cntrIfCmd(req);
	    emit modifiedItem("wlb_"+nId);
	}
    }
}

void VisDevelop::visualItAdd( QAction *cact, const QPointF &pnt, const string &iWid, const string &iWnm, const string &iOwn, bool chNoWr )
{
    XMLNode req("get");
    //QAction *cact = (QAction *)sender();
    string own_wdg = iOwn.empty() ? TSYS::strSepParse(work_wdg,0,';') : iOwn;
    string par_nm = cact->objectName().toStdString();
    QMdiSubWindow *actSubW = work_space->activeSubWindow();

    if(actSubW && !wdgTree->hasFocus() && !prjTree->hasFocus() && pnt.isNull() &&
	    !((DevelWdgView*)((QScrollArea*)actSubW->widget())->widget())->edit())
	return;

    //Count level
    int p_el_cnt = 0;
    for(int i_off = 0; TSYS::pathLev(own_wdg,0,true,&i_off).size(); p_el_cnt++) ;
    string sid1 = TSYS::pathLev(own_wdg, 0);

    //Make request id and name dialog
    InputDlg dlg(this, cact->icon(), _("Enter the identifier and the name of the new widget/page."), _("Creating a widget/page"), true, true);
    dlg.setIdLen(30);

    if(p_el_cnt > 1 && iWid.empty()) {
	// New include item id generator
	//  Present include widgets list request
	if(!sid1.compare(0,4,"prj_") && pnt.isNull()) req.setAttr("path", own_wdg+"/%2fpage%2fpage");
	else req.setAttr("path", own_wdg+"/%2finclwdg%2fwdg");
	if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	else {
	    //  Get parent widget id
	    string base_nm = "item";
	    if(!par_nm.empty())	base_nm = TSYS::pathLev(par_nm,1,true).substr(4);
	    unsigned i_c = 1, i_w = 0;
	    while(i_w < req.childSize())
		if(req.childGet(i_w)->attr("id") == base_nm+i2s(i_c))	{ i_w = 0; i_c++; }
		else i_w++;
	    dlg.setId((base_nm+i2s(i_c)).c_str());
	}
    }

    // Execute dialog
    if(!iWid.empty() || dlg.exec() == QDialog::Accepted) {
	string w_id = iWid.empty() ? dlg.id().toStdString() : iWid;
	string w_nm = iWid.empty() ? dlg.name().toStdString() : iWnm;

	// Check for widget's library
	req.clear()->setName("add");
	string new_wdg;
	if(!sid1.compare(0,4,"wlb_")) {
	    if(p_el_cnt == 1) req.setAttr("path", own_wdg+"/%2fwdg%2fwdg")->setAttr("id", w_id)->setText(w_nm);
	    else req.setAttr("path", own_wdg+"/%2finclwdg%2fwdg")->setAttr("id", w_id)->setText(w_nm);
	    new_wdg = own_wdg + "/wdg_";
	}
	else if(!sid1.compare(0,4,"prj_")) {
	    if(pnt.isNull()) {
		req.setAttr("path", own_wdg+"/%2fpage%2fpage")->setAttr("id", w_id)->setText(w_nm);
		new_wdg = own_wdg + "/pg_";
	    }
	    else {
		req.setAttr("path", own_wdg+"/%2finclwdg%2fwdg")->setAttr("id", w_id)->setText(w_nm);
		new_wdg = own_wdg + "/wdg_";
	    }
	}

	// Create widget
	int err = cntrIfCmd(req);
	if(err) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	if(err == 1)	emit modifiedItem(new_wdg+req.attr("id"));					//Warning
	else if(!err) {
	    new_wdg += req.attr("id");
	    //  Set some parameters
	    req.clear()->setName("set");
	    if(!par_nm.empty()) {
		//   Set parent widget name and enable item
		req.setAttr("path", new_wdg+"/%2fwdg%2fst%2fparent")->setText(par_nm);
		err = cntrIfCmd(req);
		req.setAttr("path", new_wdg+"/%2fwdg%2fst%2fen")->setText("1");
		err = cntrIfCmd(req);
	    }
	    //   Set geometry for include widget
	    if(!err && !pnt.isNull()) {
		req.setAttr("path", new_wdg+"/%2fattr%2fgeomX")->setText(r2s(pnt.x()));
		err = cntrIfCmd(req);
		req.setAttr("path", new_wdg+"/%2fattr%2fgeomY")->setText(r2s(pnt.y()));
		err = cntrIfCmd(req);

		//Send change request to opened for edit widget
		if(!chNoWr) {
		    DevelWdgView *dw = work_space->findChild<DevelWdgView*>(own_wdg.c_str());
		    if(dw) dw->chRecord(*XMLNode("chldAdd").setAttr("path", new_wdg)->setAttr("id", w_id)->setAttr("name", w_nm)->
			setAttr("parent", par_nm)->setAttr("x",r2s(pnt.x()))->setAttr("y",r2s(pnt.y())));
		}
		work_space->setActiveSubWindow(actSubW);	//For set focus to target subwindow and the new widget select
	    }
	    if(err) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	    emit modifiedItem(new_wdg);
	}
    }
    cact->setChecked(false);
}

void VisDevelop::visualItDel( const string &itms, bool chNoWr )
{
    string work_wdg_loc = itms.empty() ? work_wdg : itms;
    string del_wdg, lst_wdg;
    DevelWdgView *dw = NULL;
    XMLNode chCtx("chldDel");

    //Request to confirm
    if(itms.empty()) {
	InputDlg dlg(this,actVisItDel->icon(),
		QString(_("Are you sure of deleting the visual items: '%1'?")).arg(QString(work_wdg_loc.c_str()).replace(";","; ")),
		_("Deleting the visual items"),false,false);
	if(dlg.exec() != QDialog::Accepted) return;
    }

    for(int w_off = 0; (del_wdg=TSYS::strSepParse(work_wdg_loc,0,';',&w_off)).size(); )
    {
	//Get owner object path and deleted item identifier
	string it_own, it_id;
	int p_el_cnt = 0;
	string it_tmp = TSYS::pathLev(del_wdg,p_el_cnt++);
	do {
	    it_own= it_own+(it_id.empty() ? "" : ("/"+it_id));
	    it_id = it_tmp;
	}
	while((it_tmp=TSYS::pathLev(del_wdg,p_el_cnt++)).size());
	p_el_cnt--;

	XMLNode req("del");
	string sid1 = TSYS::pathLev(it_own,0);
	//Check for widget's library
	if(sid1.empty()) {
	    if(!it_id.compare(0,4,"wlb_")) req.setAttr("path", "/%2fprm%2fcfg%2fwlb")->setAttr("id", it_id.substr(4));
	    else if(!it_id.compare(0,4,"prj_")) req.setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("id",it_id.substr(4));
	}
	else if(!sid1.compare(0,4,"wlb_")) {
	    if(p_el_cnt <= 2) req.setAttr("path", it_own+"/%2fwdg%2fwdg")->setAttr("id", it_id.substr(4));
	    else req.setAttr("path", it_own+"/%2finclwdg%2fwdg")->setAttr("id", it_id.substr(4));
	}
	else if(!sid1.compare(0,4,"prj_")) {
	    if(p_el_cnt <= 2) req.setAttr("path", it_own+"/%2fpage%2fpage")->setAttr("id", it_id.substr(3));
	    else if(!it_id.compare(0,3,"pg_")) req.setAttr("path", it_own+"/%2fpage%2fpage")->setAttr("id", it_id.substr(3));
	    else req.setAttr("path", it_own+"/%2finclwdg%2fwdg")->setAttr("id", it_id.substr(4));
	}

	//Save removed widget context for change record
	if(!chNoWr && (dw=work_space->findChild<DevelWdgView*>(del_wdg.c_str()))) dw->chLoadCtx(chCtx, "parent;");

	//Send remove context
	if(cntrIfCmd(req))	mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	else if(p_el_cnt < 3)	emit modifiedItem(del_wdg);
	else {
	    if(!lst_wdg.empty() && lst_wdg != it_own)	emit modifiedItem(lst_wdg);
	    lst_wdg = it_own;

	    //Send change request to opened for edit widget
	    if(dw) dw->chRecord(chCtx);
	}
    }
    if(!lst_wdg.empty()) emit modifiedItem(lst_wdg);
}

void VisDevelop::visualItProp( )
{
    //if( work_wdg.empty() )	return;
    string prop_wdg = TSYS::strSepParse(work_wdg,0,';');

    string sel1 = TSYS::pathLev(prop_wdg,0);
    string sel2 = TSYS::pathLev(prop_wdg,1);

    if(sel1.size() && !sel2.size()) {
	if(!prjLibPropDlg) {
	    prjLibPropDlg = new LibProjProp(this);
	    connect(prjLibPropDlg, SIGNAL(apply(const string&)), this, SIGNAL(modifiedItem(const string&)));
	}
	prjLibPropDlg->showDlg(prop_wdg);
    }
    else {
	if(!visItPropDlg) {
	    visItPropDlg = new VisItProp(this);
	    connect(visItPropDlg, SIGNAL(apply(const string &)), this, SIGNAL(modifiedItem(const string&)));
	}
	visItPropDlg->showDlg(prop_wdg);
    }
}

void VisDevelop::visualItEdit( )
{
    string ed_wdg;
    for(int w_off = 0; (ed_wdg=TSYS::strSepParse(work_wdg,0,';',&w_off)).size(); ) {
	QString w_title(QString(_("Widget: %1")).arg(ed_wdg.c_str()));

	//Check to already opened widget window
	QList<QMdiSubWindow *> ws_wdg = work_space->subWindowList();
	int i_w;
	for(i_w = 0; i_w < ws_wdg.size(); i_w++)
	    if(ws_wdg.at(i_w)->windowTitle() == w_title) {
		mod->postMess(mod->nodePath().c_str(),
		    QString(_("The widget '%1' editing window is already open.")).
			    arg(ed_wdg.c_str()), TVision::Info, this );
		work_space->setActiveSubWindow(ws_wdg.at(i_w));
		break;
	    }
	if(i_w < ws_wdg.size()) continue;

	QScrollArea *scrl = new QScrollArea;
#if QT_VERSION >= 0x040400
	scrl->setAlignment(Qt::AlignCenter);
#endif
	QPalette plt = scrl->palette();
        plt.setBrush(QPalette::Window,QBrush("grey", Qt::Dense7Pattern));
	scrl->setPalette(plt);
	//scrl->setBackgroundRole(QPalette::Dark);
	scrl->setAttribute(Qt::WA_DeleteOnClose);
	scrl->setWindowTitle(w_title);

	//Make and place widget's view
	DevelWdgView *vw = new DevelWdgView(ed_wdg, 0, this, 0, scrl);
	vw->load("");
	connect(vw, SIGNAL(selected(const string&)), this, SLOT(selectItem(const string&)));
	connect(vw, SIGNAL(apply(const string&)), this, SIGNAL(modifiedItem(const string&)));
	connect(this, SIGNAL(modifiedItem(const string&)), vw, SLOT(load(const string&)));

	scrl->setWidget(vw);
	work_space->addSubWindow(scrl);

	//Set window icon
	XMLNode req("get");
	req.setAttr("path",ed_wdg+"/%2fico");
	if(!cntrIfCmd(req)) {
	    QImage ico_t;
	    string simg = TSYS::strDecode(req.text(), TSYS::base64);
	    if(ico_t.loadFromData((const uchar*)simg.data(),simg.size()))
		scrl->parentWidget()->setWindowIcon(QPixmap::fromImage(ico_t));	//parentWidget is QMdiSubWindow
	}

	scrl->parentWidget()->show();

	if(/*work_space->subWindowList().length() <= 1*/ !(work_space->activeSubWindow() && work_space->activeSubWindow()->isMaximized()))
	    scrl->parentWidget()->resize(fmax(300,fmin(work_space->width(),vw->size().width()+(scrl->parentWidget()->width()-scrl->width())+5)),
				     fmax(200,fmin(work_space->height(),vw->size().height()+(scrl->parentWidget()->height()-scrl->height())+5)));
    }
}

void VisDevelop::visualItClear( const string &elWa )
{
    string clrW;
    string workWdgLoc, workAttr;

    if(elWa.empty()) {
	workWdgLoc = work_wdg;

	InputDlg dlg(this,actVisItClear->icon(),
		QString(_("Are you sure of clear all changes of the visual items: '%1'?\n"
			  "All changes will be lost and values will return to the previous state or will be inherited!")).arg(QString(workWdgLoc.c_str()).replace(";","; ")),
		_("Clearing the visual items changes"),false,false);
	if(dlg.exec() != QDialog::Accepted)	return;
    }
    else {
	string work_tmp;
	for(int off = 0; (work_tmp=TSYS::pathLev(elWa,0,true,&off)).size(); ) {
	    if(workAttr.size()) workWdgLoc += "/"+workAttr;
	    workAttr = work_tmp;
	}
	if(workAttr.size() > 2 && workAttr.substr(0,2) == "a_") workAttr = workAttr.substr(2);
	else { workWdgLoc += "/"+workAttr; workAttr = ""; }
    }

    for(int w_off = 0; (clrW=TSYS::strSepParse(workWdgLoc,0,';',&w_off)).size(); ) {
	XMLNode req("set");
	req.setAttr("path",clrW+"/%2fwdg%2fcfg%2fclear")->setAttr("attr",workAttr);
	if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	else emit modifiedItem(clrW);
    }
}

void VisDevelop::visualItDownParent( const string &elWa )
{
    string clrW;
    string workWdgLoc, workAttr;

    if(elWa.empty()) {
	workWdgLoc = work_wdg;

	InputDlg dlg(this,actVisItChDown->icon(),
		QString(_("Are you sure for lowering down to the parent for all changes of the visual items: '%1'?\n"
			  "All other visual elements, descendant from this ancestor, will use these changes!")).arg(QString(workWdgLoc.c_str()).replace(";","; ")),
		_("Lowering down the visual item changes to the parent"),false,false);
	if(dlg.exec() != QDialog::Accepted)	return;
    }
    else {
	string work_tmp;
	for(int off = 0; (work_tmp=TSYS::pathLev(elWa,0,true,&off)).size(); ) {
	    if(workAttr.size()) workWdgLoc += "/"+workAttr;
	    workAttr = work_tmp;
	}
	if(workAttr.size() > 2 && workAttr.substr(0,2) == "a_") workAttr = workAttr.substr(2);
	else { workWdgLoc += "/"+workAttr; workAttr = ""; }
    }

    for(int w_off = 0; (clrW=TSYS::strSepParse(workWdgLoc,0,';',&w_off)).size(); ) {
	XMLNode req("set");
	req.setAttr("path",clrW+"/%2fwdg%2fcfg%2fchDown")->setAttr("attr", workAttr);
	if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	else emit modifiedItem(req.attr("chParent"));
    }
}

void VisDevelop::visualItCut( )
{
    if(!actVisItCut->property("wdgAddr").toString().isEmpty())	return;
    copy_buf = "1"+work_wdg;
    editToolUpdate();
}

void VisDevelop::visualItCopy( )
{
    if(!actVisItCopy->property("wdgAddr").toString().isEmpty())	return;
    copy_buf = "0"+work_wdg;
    editToolUpdate();
}

void VisDevelop::visualItPaste( const string &wsrc, const string &wdst, const string &wnm, bool chNoWr )
{
    if(!actVisItPaste->property("wdgAddr").toString().isEmpty())return;
    string copy_buf_el, del_els, last_del;
    string copy_buf_w = wsrc.empty() ? copy_buf : wsrc;
    string work_wdg_w = wdst.empty() ? work_wdg : wdst;
    vector<string> copy_els;
    QCheckBox *wInher = NULL;
    int zLev = 0;

    InputDlg dlg(this, actVisItPaste->icon(), "", _("Moving or copying the visual items"), true, true);
    dlg.setIdLen(30);
    for(int w_off = 0; (copy_buf_el=TSYS::strSepParse(copy_buf_w.substr(1),0,';',&w_off)).size(); ) {
	string s_elp, d_elp, s_el, d_el, t_el, t1_el;
	//Destination elements calc
	int n_del = 0;
	for(int off = 0; !(t_el=TSYS::pathLev(work_wdg_w,0,true,&off)).empty(); n_del++)
	{ if(n_del) d_elp += ("/"+d_el); d_el = t_el; }
	//Src elements calc
	int n_sel = 0;
	for(int off = 0; !(t_el=TSYS::pathLev(copy_buf_el,0,true,&off)).empty(); n_sel++)
	{ if(n_sel) s_elp += ("/"+s_el); s_el = t_el; }

	//Copy visual item
	XMLNode req("get");
	// Project copy
	if(s_el.substr(0,4) == "prj_") {
	    t_el = (QString((copy_buf_w[0] == '1') ? _("Moving the project '%1'.\n") : _("Copying the project '%1'.\n"))+
		_("Enter the identifier and the name of the new project.")).arg(s_el.substr(4).c_str()).toStdString();
	    req.setAttr("path","/%2fprm%2fcfg%2fprj");
	    d_el = "prj_";
	    t1_el = s_el.substr(4);
	}
	// Widget's library copy
	else if(s_el.substr(0,4) == "wlb_") {
	    t_el = (QString((copy_buf_w[0] == '1') ? _("Moving the widgets library '%1'.\n") : _("Copying the widgets library '%1'.\n"))+
		_("Enter the identifier and the name of the new widgets library.")).arg(s_el.substr(4).c_str()).toStdString();
	    req.setAttr("path","/%2fprm%2fcfg%2fwlb");
	    d_el = "wlb_";
	    t1_el = s_el.substr(4);
	}
	// Page copy
	else if(s_el.substr(0,3) == "pg_" && (d_el.substr(0,4) == "prj_" || d_el.substr(0,3) == "pg_" || d_el.substr(0,4) == "wlb_"))
	{
	    t_el = (QString((copy_buf_w[0] == '1') ? _("Moving the page '%1' to '%2'.\n") : _("Copying the page '%1' to '%2'.\n"))+
		_("Enter the identifier and the name of the new widget/page.")).arg(copy_buf_el.c_str()).arg(work_wdg_w.c_str()).toStdString();
	    if(d_el.substr(0,4)=="wlb_") req.setAttr("path",work_wdg_w+"/%2fwdg%2fwdg");
	    else req.setAttr("path",work_wdg_w+"/%2fpage%2fpage");
	    d_elp += ("/"+d_el);
	    d_el = (d_el.substr(0,4)=="wlb_") ? "wdg_" : "pg_";
	    t1_el = s_el.substr(3);
	}
	// Widget copy
	else if(s_el.substr(0,4) == "wdg_" && (d_el.substr(0,3) == "pg_" || d_el.substr(0,4) == "wlb_" ||
	    (TSYS::pathLev(d_elp,0).substr(0,4) == "wlb_" && n_del == 2)))
	{
	    t_el = (QString((copy_buf_w[0] == '1') ? _("Moving the widget '%1' to '%2'.\n") : _("Copying the widget '%1' to '%2'.\n"))+
		_("Enter the identifier and the name of the new widget.")).arg(copy_buf_el.c_str()).arg(work_wdg_w.c_str()).toStdString();
	    if(d_el.substr(0,4)=="wlb_") req.setAttr("path",work_wdg_w+"/%2fwdg%2fwdg");
	    else req.setAttr("path",work_wdg_w+"/%2finclwdg%2fwdg");
	    d_elp += ("/"+d_el);
	    d_el = "wdg_";
	    t1_el = s_el.substr(4);
	}
	// Direct widget to widget copy
	else if(s_el.substr(0,4) == "wdg_" && d_el.substr(0,4) == "wdg_") {
	    t_el = (QString((copy_buf_w[0] == '1') ? _("Moving the widget '%1' to '%2'.\n") : _("Copying the widget '%1' to '%2'.\n"))+
		_("Enter the identifier and the name of the new widget.")).arg(copy_buf_el.c_str()).arg(work_wdg_w.c_str()).toStdString();
	    req.setAttr("path",work_wdg_w+"/%2finclwdg%2fwdg");
	    t1_el = d_el.substr(4);
	    d_el = "wdg_";
	}
	// Copy scheme error
	else {
	    mod->postMess(mod->nodePath().c_str(),QString(_("Copy/move scheme from '%1' to '%2' is not supported.")).
		arg(copy_buf_el.c_str()).arg(work_wdg_w.c_str()),TVision::Error,this);
	    return;
	}
	// Prepare new widget identifier
	//  Remove digits from end of new identifier
	if(wdst.empty()) {
	    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	    else {
		zLev = req.childSize();
		for(unsigned i_w = 0; i_w < req.childSize(); )
		    if(req.childGet(i_w)->attr("id") == t1_el) { i_w = 0; t1_el = TSYS::strLabEnum(t1_el); }
		    else i_w++;
	    }
	}
	// Make request dialog
	dlg.setMess(t_el.c_str());
	dlg.setId(t1_el.c_str());
	// Add Link flag for copy operation
	if(copy_buf_w[0] != '1' && d_el.substr(0,4) != "prj_" && d_el.substr(0,4) != "wlb_") {
	    dlg.edLay()->addWidget(new QLabel(_("Inherit:"),&dlg), 2, 0);
	    wInher = new QCheckBox(&dlg);
	    dlg.edLay()->addWidget(wInher, 2, 1);
	}
	if(!wsrc.empty() || dlg.exec() == QDialog::Accepted) {
	    dlg.setId(TSYS::strEncode(dlg.id().toStdString(),TSYS::oscdID).c_str());
	    if(wdst.empty() && dlg.id().toStdString() != t1_el) {
		unsigned i_w = 0;
		for( ; i_w < req.childSize() && req.childGet(i_w)->attr("id") != dlg.id().toStdString(); i_w++) ;
		if(i_w < req.childSize() &&
			QMessageBox::question(this,_("Moving or copying the visual items"),
			    QString(_("The target item '%1' is already present!\nPaste to it anyway?")).arg(dlg.id()),QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
		    continue;
	    }

	    d_el += dlg.id().toStdString();
	    string it_nm = wnm.empty() ? dlg.name().toStdString() : wnm;

	    //  Make link
	    if(wInher && wInher->isChecked()) {
		QAction addAct(NULL);
		addAct.setObjectName((s_elp+"/"+s_el).c_str());
		visualItAdd(&addAct, QPointF(), dlg.id().toStdString(), it_nm, d_elp);
	    }
	    //  Make copy
	    else {
		if(d_elp == s_elp && d_el == s_el) {
		    mod->postMess(mod->nodePath().c_str(),QString(_("Attempting to copy/move yourself for \"%1/%1\"!")).
			arg(d_elp.c_str()).arg(d_el.c_str()),TVision::Error,this);
		    return;
		}

		req.clear()->setName("set")->setAttr("path", "/%2fprm%2fcfg%2fcp%2fcp")->
		    setAttr("src", s_elp+"/"+s_el)->setAttr("dst", d_elp+"/"+d_el);
		if(cntrIfCmd(req))
		    mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
		else {
		    if(it_nm.size()) {
			req.clear()->setName("set")->setText(it_nm);
			if(d_el.substr(0,4) == "prj_" || d_el.substr(0,4) == "wlb_")
			    req.setAttr("path",d_elp+"/"+d_el+"/%2fobj%2fcfg%2fname");
			else req.setAttr("path",d_elp+"/"+d_el+"/%2fwdg%2fcfg%2fname");
			cntrIfCmd(req);
		    }
		    if(n_del < 2) copy_els.push_back(d_elp+"/"+d_el);
		    else {
			//  geomZ set to UP
			req.clear()->setName("set")->setAttr("path",d_elp+"/"+d_el+"/%2fattr%2fgeomZ")->setText(i2s(zLev));
			cntrIfCmd(req);

			if(!last_del.empty() && last_del != d_elp) copy_els.push_back(last_del);
			last_del = d_elp;
		    }
		    del_els += copy_buf_el+";";

		    // Send change request to opened for edit widget
		    if(!chNoWr) {
			DevelWdgView *dw = work_space->findChild<DevelWdgView*>(d_elp.c_str());
			if(dw) dw->chRecord(*XMLNode("chldPaste").
				setAttr("src",s_elp+"/"+s_el)->setAttr("dst",d_elp+"/"+d_el)->setAttr("name",it_nm));
		    }
		}
	    }
	}
    }
    if(!last_del.empty())	copy_els.push_back(last_del);

    //Send events to created widgets
    for(unsigned i_e = 0; i_e < copy_els.size(); i_e++)
	emit modifiedItem(copy_els[i_e]);
    //Remove source widget
    if(copy_buf_w[0] == '1') 	{ visualItDel(del_els); if(wsrc.empty()) copy_buf = "0"; }
}

void VisDevelop::editToolUpdate( )
{
    if(!actVisItCopy->property("wdgAddr").toString().isEmpty()) return;
    actVisItCut->setEnabled(!work_wdg.empty());
    actVisItCopy->setEnabled(!work_wdg.empty());

    //Src and destination elements calc
    string s_elp, d_elp, s_el, d_el, t_el;
    int n_sel = 0;
    int n_del = 0;
    for(int off = 0; !(t_el=TSYS::pathLev(copy_buf.substr(1),0,true,&off)).empty(); n_sel++)
    { s_elp += ("/"+s_el); s_el = t_el; }
    for(int off = 0; !(t_el=TSYS::pathLev(work_wdg,0,true,&off)).empty(); n_del++)
    { d_elp += ("/"+d_el); d_el = t_el; }
    if((s_el.substr(0,4) == "prj_" || s_el.substr(0,4) == "wlb_") ||										//Project and library copy
	    (s_el.substr(0,3) == "pg_" && (d_el.substr(0,4) == "prj_" || d_el.substr(0,3) == "pg_" || d_el.substr(0,4) == "wlb_")) ||		//Page copy
	    (s_el.substr(0,4) == "wdg_" && (d_el.substr(0,3) == "pg_" || d_el.substr(0,4) == "wlb_" || (TSYS::pathLev(d_elp,0).substr(0,4) == "wlb_" && n_del==2))))	//Widget copy
	actVisItPaste->setEnabled(true);
    else actVisItPaste->setEnabled(false);
}

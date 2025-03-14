
//OpenSCADA module UI.QTCfg file: qtcfg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2025 by Roman Savochenko, <roman@oscada.org>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>

#include <QToolTip>
#include <QTreeWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPixmap>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QWhatsThis>
#include <QTabWidget>
#include <QGridLayout>
#include <QStatusBar>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QTimer>
#include <QMessageBox>
#include <QGroupBox>
#include <QTableWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QDateTime>
#include <QLineEdit>
#include <QErrorMessage>
#include <QScrollArea>
#include <QCloseEvent>
#include <QFileDialog>
#include <QScrollBar>
#include <QClipboard>
#include <QTextBrowser>

#include "qtcfg.h"
#include "selfwidg.h"
#include "../QTStarter/lib_qtgen.h"
#include "tuimod.h"

#if QT_VERSION < 0x060000
# define setSecsSinceEpoch(tm)	setTime_t(tm)
#endif

#define CH_REFR_TM	100	//Typical refresh time
#define CH_REFR_TM_MAX	10000	//Maximum ordered refresh time
#define GRP_SHOW_OP_LIM	10

#undef _
#define _(mess) mod->I18N(mess, lang().c_str()).c_str()

using namespace OSCADA_QT;
using namespace QTCFG;

//*************************************************
//* ConfApp                                       *
//*************************************************
int ConfApp::winCntr = 0;

ConfApp::ConfApp( string open_user ) : winClose(false), reqPrgrs(NULL),
    pgInfo("info"), genReqs("CntrReqs"), root(&pgInfo), copyBuf("0"), queSz(20), inHostReq(0), tblInit(false), pgDisplay(false)
{
    connect(this, SIGNAL(makeStarterMenu(QWidget*,const QString&)), qApp, SLOT(makeStarterMenu(QWidget*,const QString&)));

    //Main window settings
    setAttribute(Qt::WA_DeleteOnClose, true);
    QImage ico_t;
    mod->regWin(this);

    setWindowTitle((PACKAGE_NAME " "+mod->modId()+": "+trD_U(SYS->name(),open_user)).c_str());
    setWindowIcon(mod->icon());

    //Init centrall widget
    setCentralWidget(new QWidget(this));
    QGridLayout *QTCfgLayout = new QGridLayout(centralWidget());
    QTCfgLayout->setContentsMargins(3, 3, 3, 3);

    //Init splitter
    splitter = new QSplitter(centralWidget());
    splitter->setOrientation(Qt::Horizontal);

    //Create Navigator tree
    QFrame *frm = new QFrame(splitter);
    QVBoxLayout *vlay = new QVBoxLayout;
    vlay->setContentsMargins(0, 0, 0, 0);
    CtrTree = new QTreeWidget;
    vlay->addWidget(CtrTree);
    CtrTree->setContextMenuPolicy(Qt::CustomContextMenu);
    CtrTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    CtrTree->setAutoScroll(false);
    //splitter->setSizeConstraint(QSplitter::KeepSize);
    CtrTree->header()->setStretchLastSection(false);
    CtrTree->header()->resizeSection(0, icoSize(14));
    //CtrTree->header()->setSectionHidden(1,true);
    //CtrTree->header()->setSectionHidden(2,true);
    //CtrTree->header()->hide();
    //CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );
    CtrTree->setMinimumSize(QSize(icoSize(10),0));

    connect(CtrTree, SIGNAL(itemSelectionChanged()), this, SLOT(selectItem()));
    connect(CtrTree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(viewChild(QTreeWidgetItem*)));
    connect(CtrTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ctrTreePopup()));

    //Create search field
    QLineEdit *trSrchW = new QLineEdit;
    connect(trSrchW, SIGNAL(textChanged(const QString)), this, SLOT(treeSearch()));
    connect(trSrchW, SIGNAL(returnPressed()), this, SLOT(treeSearch()));
    vlay->addWidget(trSrchW);
    frm->setLayout(vlay);

    //Right frame add
    gFrame = new QFrame(splitter);
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp.setHorizontalStretch(3);
    gFrame->setSizePolicy(sp);
    gFrame->setFrameShape(QFrame::WinPanel);
    gFrame->setFrameShadow(QFrame::Raised);

    QGridLayout *gFrameLayout = new QGridLayout(gFrame);

    titleIco = new QLabel(gFrame);
    titleIco->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    gFrameLayout->addWidget(titleIco, 0, 0);

    titleLab = new QLabel(gFrame);
    //titleLab->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred));
    QFont tFnt(titleLab->font());
    if(tFnt.pointSizeF() > 0)	tFnt.setPointSizeF(tFnt.pointSizeF()*1.4);
    else if(tFnt.pixelSize() > 0) tFnt.setPixelSize(tFnt.pixelSize()*1.4);
    //tFnt.setPointSize(14);
    titleLab->setFont(tFnt);
    titleLab->setWordWrap(true);
    gFrameLayout->addWidget(titleLab, 0, 1);

    //Create the tabulator
    tabs = new QTabWidget(gFrame);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabSelect(int)));
    gFrameLayout->addWidget(tabs, 1, 0, 1, 2);

    QList<int> splSz;
    splSz.push_back(icoSize(14));
    splSz.push_back(icoSize(40));
    splitter->setSizes(splSz);
    QTCfgLayout->addWidget(splitter, 0, 0);

    if(!s2i(SYS->cmdOpt("showWin"))) resize(icoSize(50), icoSize(40));

    //Create actions
    // Close
    if(!ico_t.load(TUIS::icoGet("close",NULL,true).c_str())) ico_t.load(":/images/close.png");
    actClose = new QAction(QPixmap::fromImage(ico_t), "", this);
    actClose->setShortcut(Qt::CTRL|Qt::Key_W);
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    // Quit
    if(!ico_t.load(TUIS::icoGet("exit",NULL,true).c_str())) ico_t.load(":/images/exit.png");
    actQuit = new QAction(QPixmap::fromImage(ico_t), "", this);
    actQuit->setShortcut(Qt::CTRL|Qt::Key_Q);
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quitSt()));
    // Up button
    if(!ico_t.load(TUIS::icoGet("up",NULL,true).c_str())) ico_t.load(":/images/up.png");
    actUp = new QAction(QPixmap::fromImage(ico_t), "", this);
    actUp->setShortcut(Qt::ALT|Qt::Key_Up);
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(triggered()), this, SLOT(pageUp()));
    // Previous page
    if(!ico_t.load(TUIS::icoGet("previous",NULL,true).c_str())) ico_t.load(":/images/previous.png");
    actPrev = new QAction(QPixmap::fromImage(ico_t), "", this);
    actPrev->setShortcut(Qt::ALT|Qt::Key_Left);
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(triggered()), this, SLOT(pagePrev()));
    // Next page
    if(!ico_t.load(TUIS::icoGet("next",NULL,true).c_str())) ico_t.load(":/images/next.png");
    actNext = new QAction(QPixmap::fromImage(ico_t), "", this);
    actNext->setShortcut(Qt::ALT|Qt::Key_Right);
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(triggered()), this, SLOT(pageNext()));
    // Load item from db
    if(!ico_t.load(TUIS::icoGet("load",NULL,true).c_str())) ico_t.load(":/images/load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t), "", this);
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(triggered()), this, SLOT(itDBLoad()));
    actDBLoadF = new QAction(QPixmap::fromImage(ico_t), "", this);
    connect(actDBLoadF, SIGNAL(triggered()), this, SLOT(itDBLoad()));
    // Save item to db
    if(!ico_t.load(TUIS::icoGet("save",NULL,true).c_str())) ico_t.load(":/images/save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t), "", this);
    actDBSave->setShortcut(QKeySequence("Ctrl+S"));
    actDBSave->setEnabled(false);
    connect(actDBSave, SIGNAL(triggered()), this, SLOT(itDBSave()));
    actDBSaveF = new QAction(QPixmap::fromImage(ico_t), "", this);
    connect(actDBSaveF, SIGNAL(triggered()), this, SLOT(itDBSave()));
    // Add an item
    if(!ico_t.load(TUIS::icoGet("it_add",NULL,true).c_str())) ico_t.load(":/images/it_add.png");
    actItAdd = new QAction(QPixmap::fromImage(ico_t), "", this);
    actItAdd->setEnabled(false);
    connect(actItAdd, SIGNAL(triggered()), this, SLOT(itAdd()));
    // Delete the item
    if(!ico_t.load(TUIS::icoGet("it_del",NULL,true).c_str())) ico_t.load(":/images/it_del.png");
    actItDel = new QAction(QPixmap::fromImage(ico_t), "", this);
    actItDel->setShortcut(QKeySequence("Ctrl+D"));
    actItDel->setEnabled(false);
    connect(actItDel, SIGNAL(triggered()), this, SLOT(itDel()));
    // Copy the item
    if(!ico_t.load(TUIS::icoGet("editcopy",NULL,true).c_str())) ico_t.load(":/images/editcopy.png");
    actItCopy = new QAction(QPixmap::fromImage(ico_t), "", this);
    actItCopy->setShortcut(QKeySequence("Ctrl+C"));
    actItCopy->setEnabled(false);
    connect(actItCopy, SIGNAL(triggered()), this, SLOT(itCopy()));
    // Cut the item
    if(!ico_t.load(TUIS::icoGet("editcut",NULL,true).c_str())) ico_t.load(":/images/editcut.png");
    actItCut = new QAction(QPixmap::fromImage(ico_t), "", this);
    actItCut->setShortcut(QKeySequence("Ctrl+X"));
    actItCut->setEnabled(false);
    connect(actItCut, SIGNAL(triggered()), this, SLOT(itCut()));
    // Paste item
    if(!ico_t.load(TUIS::icoGet("editpaste",NULL,true).c_str())) ico_t.load(":/images/editpaste.png");
    actItPaste = new QAction(QPixmap::fromImage(ico_t), "", this);
    actItPaste->setShortcut(QKeySequence("Ctrl+V"));
    actItPaste->setEnabled(false);
    connect(actItPaste, SIGNAL(triggered()), this, SLOT(itPaste()));
    // Update
    if(!ico_t.load(TUIS::icoGet("reload",NULL,true).c_str())) ico_t.load(":/images/reload.png");
    actUpdate = new QAction(QPixmap::fromImage(ico_t), "", this);
    actUpdate->setShortcut(Qt::Key_F5);
    connect(actUpdate, SIGNAL(triggered()), this, SLOT(pageRefresh()));
    // Start of "Auto update"
    if(!ico_t.load(TUIS::icoGet("start",NULL,true).c_str())) ico_t.load(":/images/start.png");
    actStartUpd = new QAction(QPixmap::fromImage(ico_t), "", this);
    actStartUpd->setShortcut(Qt::CTRL|Qt::Key_R);
    connect(actStartUpd, SIGNAL(triggered()), this, SLOT(pageCyclRefrStart()));
    // Stop of "Auto update"
    if(!ico_t.load(TUIS::icoGet("stop",NULL,true).c_str())) ico_t.load(":/images/stop.png");
    actStopUpd = new QAction(QPixmap::fromImage(ico_t), "", this);
    actStopUpd->setShortcut(Qt::CTRL|Qt::Key_E);
    actStopUpd->setEnabled(false);
    connect(actStopUpd, SIGNAL(triggered()), this, SLOT(pageCyclRefrStop()));
    // Favorite page toggling
    if(!favToggleAdd.load(TUIS::icoGet("favorites_add",NULL,true).c_str())) favToggleAdd.load(":/images/favorites_add.png");
    if(!favToggleDel.load(TUIS::icoGet("favorites_del",NULL,true).c_str())) favToggleDel.load(":/images/favorites_del.png");
    actFavToggle = new QAction(QPixmap::fromImage(favToggleAdd), "", this);
    actFavToggle->setShortcut(Qt::CTRL|Qt::Key_B);
    actFavToggle->setEnabled(false);
    connect(actFavToggle, SIGNAL(triggered()), this, SLOT(favToggle()));
    // Favorite page
    if(!ico_t.load(TUIS::icoGet("favorites",NULL,true).c_str())) ico_t.load(":/images/favorites.png");
    actFav = new QAction(QPixmap::fromImage(ico_t), "", this);
    actFav->setEnabled(false);
    actFav->setMenu(new QMenu(this));
    connect(actFav, SIGNAL(triggered()), this, SLOT(favGo()));
    // About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    actAbout = new QAction(QPixmap::fromImage(ico_t), "", this);
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    // About Qt
    actQtAbout = new QAction("", this);
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // QTCfg manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    actManual = new QAction(QPixmap::fromImage(ico_t), "", this);
    actManual->setProperty("doc", "Modules/QTCfg|Modules/QTCfg");
    actManual->setShortcut(Qt::Key_F1);
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    // OpenSCADA manual index
    actManualSYS = new QAction(QPixmap::fromImage(ico_t), "", this);
    actManualSYS->setProperty("doc", "index|Documents");
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    // Page help
    actManualPage = new QAction(QPixmap::fromImage(ico_t), "", this);
    actManualPage->setEnabled(false);
    connect(actManualPage, SIGNAL(triggered()), this, SLOT(enterManual()));
    // What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    actWhatIs = new QAction(QPixmap::fromImage(ico_t), "", this);
    actWhatIs->setShortcut(Qt::SHIFT|Qt::Key_F1);
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    // Menus
    menuBar()->addMenu((menuFile=new QMenu(this)));
    menuFile->addAction(actDBLoad);
    menuFile->addAction(actDBLoadF);
    menuFile->addAction(actDBSave);
    menuFile->addAction(actDBSaveF);
    menuFile->addSeparator();
    menuFile->addAction(actClose);
    menuFile->addAction(actQuit);

    menuBar()->addMenu((menuEdit=new QMenu(this)));
    menuEdit->addAction(actItAdd);
    menuEdit->addAction(actItDel);
    menuEdit->addSeparator();
    menuEdit->addAction(actItCopy);
    menuEdit->addAction(actItCut);
    menuEdit->addAction(actItPaste);

    menuBar()->addMenu((menuView=new QMenu(this)));
    menuView->addAction(actUp);
    menuView->addAction(actPrev);
    menuView->addAction(actNext);
    menuView->addSeparator();
    menuView->addAction(actUpdate);
    menuView->addAction(actStartUpd);
    menuView->addAction(actStopUpd);
    menuView->addSeparator();
    menuView->addAction(actFav);
    menuView->addAction(actFavToggle);

    menuBar()->addMenu((menuHelp=new QMenu(this)));
    menuHelp->addAction(actAbout);
    menuHelp->addAction(actQtAbout);
    menuHelp->addAction(actManual);
    menuHelp->addAction(actManualSYS);
    menuHelp->addAction(actManualPage);
    menuHelp->addSeparator();
    menuHelp->addAction(actWhatIs);

    //Create tool bars
    // Main tool bar
    toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(icoSize(1.7),icoSize(1.7)));
    toolBar->setAllowedAreas(Qt::AllToolBarAreas);
    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(true);
    toolBar->addAction(actDBLoad);
    toolBar->addAction(actDBSave);
    toolBar->addSeparator();
    toolBar->addAction(actItAdd);
    toolBar->addAction(actItDel);
    toolBar->addSeparator();
    toolBar->addAction(actItCopy);
    toolBar->addAction(actItCut);
    toolBar->addAction(actItPaste);
    toolBar->addSeparator();
    toolBar->addAction(actUp);
    toolBar->addAction(actPrev);
    toolBar->addAction(actNext);
    toolBar->addSeparator();
    toolBar->addAction(actUpdate);
    toolBar->addAction(actStartUpd);
    toolBar->addAction(actStopUpd);
    toolBar->addSeparator();
    toolBar->addAction(actFav);
    toolBar->addAction(actFavToggle);
    toolBar->addSeparator();
    toolBar->addAction(actManualPage);
    // QTStarter
    QTStarter = new QToolBar("QTStarter", this);
    QTStarter->setIconSize(QSize(icoSize(1.7),icoSize(1.7)));
    QTStarter->setObjectName("QTStarterTool");
    addToolBar(Qt::TopToolBarArea, QTStarter);
    QTStarter->setMovable(true);

    //Init status bar
    connect(statusBar(), SIGNAL(messageChanged(const QString&)), this, SLOT(stMessChanged(const QString&)));
    wUser = new UserStBar(open_user.c_str(), this);
    statusBar()->insertPermanentWidget(0, wUser);
    connect(wUser, SIGNAL(userChanged()), this, SLOT(userSel()));

    mStModify = new QLabel(" ", this);
    statusBar()->insertPermanentWidget(0, mStModify);

    if(!ico_t.load(TUIS::icoGet("combar",NULL,true).c_str())) ico_t.load(":/images/combar.png");
    stBt = new QPushButton(QPixmap::fromImage(ico_t), "", this);
    stBt->setMaximumSize(QSize(16,18));
    stBt->setFlat(true);
    statusBar()->insertPermanentWidget(0,stBt);
    connect(stBt, SIGNAL(released()), this, SLOT(stHistCall()));

    messUpd();

    statusBar()->showMessage(_("Ready"), 2000);

    //Generic state restore
    string rst = TBDS::genPrmGet(mod->nodePath()+"st", DEF_st, user());
    int off = 0,
	wH = s2i(TSYS::strParse(rst,0,":",&off)),
	wW = s2i(TSYS::strParse(rst,0,":",&off));
    string sRst = TSYS::strDecode(TSYS::strParse(rst,0,":",&off), TSYS::base64);
    if(!s2i(SYS->cmdOpt("showWin")) && wH > 100 && wW > 100) resize(wH, wW);
    if(sRst.size()) splitter->restoreState(QByteArray(sRst.data(),sRst.size()));

    //Other resources init
    // Create auto update timer
    autoUpdTimer = new QTimer(this);
    connect(autoUpdTimer, SIGNAL(timeout()), SLOT(pageRefresh()));
    // Create end run timer
    endRunTimer  = new QTimer(this);
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(1000/*1e3*prmWait_DL*/);
    // Create Request progress closing timer
    reqPrgrsTimer = new QTimer(this);
    reqPrgrsTimer->setSingleShot(true);
    reqPrgrsTimer->setInterval(500);
    connect(reqPrgrsTimer, SIGNAL(timeout()), SLOT(reqPrgrsSet()));

    //menuBar()->setVisible(true);	//!!!! Spare for Qt5 and the native menu bar

    // Display root page and init external pages
    initHosts();
    try{ pageDisplay("/"+SYS->id()+mod->startPath()); }
    catch(TError &err) { pageDisplay("/"+SYS->id()); }

    favUpd(Fav_Reload|Fav_List);

    winCntr++;
}

ConfApp::~ConfApp( )
{
    endRunTimer->stop();
    autoUpdTimer->stop();
    reqPrgrsTimer->stop();

    mod->unregWin(this);

    // Threads deleting
    if(inHostReq)
	mess_err(mod->nodePath().c_str(), _("The configurator is using the remote host %d times."), inHostReq);
    for(map<string, SCADAHost*>::iterator iH = hosts.begin(); iH != hosts.end(); ++iH)
	delete iH->second;
    hosts.clear();

    // Push down all Qt events of the window to free the module
    for(int iTr = 0; iTr < 5; iTr++) qApp->processEvents();

    winCntr--;
}

string ConfApp::user( )	{ return wUser->user().toStdString(); }

string ConfApp::lang( bool withSystem )	{ return Mess->langCode(user(), !withSystem); }

void ConfApp::messUpd( )
{
    qApp->setProperty("lang", lang().c_str());	//For the Qt internal messages translation

    //Navigator tree
    CtrTree->setHeaderLabels(QStringList() << _("Name") << _("Type") << _("Path"));
    CtrTree->setWhatsThis(_("Main navigation tree of the configurator."));

    //Right frame
    gFrame->setWhatsThis(_("Main frame of the configurator."));

    //The tabulator
    tabs->setWhatsThis(_("Tabulated configuration pages."));

    //Actions
    // Close
    actClose->setText(_("&Close"));
    actClose->setToolTip(_("Close the configurator window"));
    actClose->setWhatsThis(_("The button for closing the configurator window"));
    actClose->setStatusTip(_("Press to close the configurator window."));
    // Quit
    actQuit->setText(_("&Quit"));
    actQuit->setToolTip(_("Quit the program"));
    actQuit->setWhatsThis(_("The button for complete quit the program"));
    actQuit->setStatusTip(_("Press for complete quit the program."));
    // Up button
    actUp->setText(_("&Up"));
    actUp->setToolTip(_("Go to the parent page"));
    actUp->setWhatsThis(_("The button for going to the parent page"));
    actUp->setStatusTip(_("Press for going to the parent page."));
    // Previous page
    actPrev->setText(_("&Back"));
    actPrev->setToolTip(_("Go back"));
    actPrev->setWhatsThis(_("The button for going to the back page"));
    actPrev->setStatusTip(_("Press for going to the back page."));
    // Next page
    actNext->setText(_("&Forward"));
    actNext->setToolTip(_("Go forward"));
    actNext->setWhatsThis(_("The button for going to the forward page"));
    actNext->setStatusTip(_("Press for going to the forward page."));
    // Load item from db
    actDBLoad->setText(_("Load"));
    actDBLoad->setToolTip(_("Load the item data"));
    actDBLoad->setWhatsThis(_("The button for loading the item data from storage"));
    actDBLoad->setStatusTip(_("Press for loading the item data from storage."));
    actDBLoadF->setText(_("Load forcibly"));
    actDBLoadF->setToolTip(_("Load the item data forcibly"));
    actDBLoadF->setWhatsThis(_("The button for loading the item data from storage forcibly, not only when it changed"));
    actDBLoadF->setStatusTip(_("Press for loading the item data from storage forcibly, not only when it changed."));
    // Save item to db
    actDBSave->setText(_("Save"));
    actDBSave->setToolTip(_("Save the item data"));
    actDBSave->setWhatsThis(_("The button for saving the item data to storage"));
    actDBSave->setStatusTip(_("Press for saving the item data to storage."));
    actDBSaveF->setText(_("Save forcibly"));
    actDBSaveF->setToolTip(_("Save the item data forcibly"));
    actDBSaveF->setWhatsThis(_("The button for saving the item data to storage forcibly, not only when it changed"));
    actDBSaveF->setStatusTip(_("Press for saving the item data to storage forcibly, not only when it changed."));
    // Add an item
    actItAdd->setText(_("&Add"));
    actItAdd->setToolTip(_("Add item"));
    actItAdd->setWhatsThis(_("The button for adding a new item"));
    actItAdd->setStatusTip(_("Press for adding a new item."));
    // Delete the item
    actItDel->setText(_("&Delete"));
    actItDel->setToolTip(_("Delete item"));
    actItDel->setWhatsThis(_("The button for deleting the item"));
    actItDel->setStatusTip(_("Press for deleting the item."));
    // Copy the item
    actItCopy->setText(_("Copy item"));
    actItCopy->setToolTip(_("Copying the item"));
    actItCopy->setWhatsThis(_("The button for copying the item"));
    actItCopy->setStatusTip(_("Press for copying the item."));
    // Cut the item
    actItCut->setText(_("Cut item"));
    actItCut->setToolTip(_("Cutting the item"));
    actItCut->setWhatsThis(_("The button for cutting the item"));
    actItCut->setStatusTip(_("Press for cutting the item."));
    // Paste item
    actItPaste->setText(_("Paste item"));
    actItPaste->setToolTip(_("Pasting the item"));
    actItPaste->setWhatsThis(_("The button for pasting the item"));
    actItPaste->setStatusTip(_("Press for pasting the item."));
    // Update
    actUpdate->setText(_("&Refresh"));
    actUpdate->setToolTip(_("Refresh the page"));
    actUpdate->setWhatsThis(_("The button for refreshing the page content"));
    actUpdate->setStatusTip(_("Press for refreshing the page content."));
    // Start of "Auto update"
    actStartUpd->setText(_("&Start"));
    actStartUpd->setToolTip(_("Start the cycled refreshing"));
    actStartUpd->setWhatsThis(_("The button for starting the cycled refreshing of the page content"));
    actStartUpd->setStatusTip(_("Press for starting the cycled refreshing of the page content."));
    // Stop of "Auto update"
    actStopUpd->setText(_("&Stop"));
    actStopUpd->setToolTip(_("Stop the cycled refreshing"));
    actStopUpd->setWhatsThis(_("The button for stopping the cycled refreshing of the page content"));
    actStopUpd->setStatusTip(_("Press for stopping the cycled refreshing of the page content."));
    // Favorite page toggling
    actFavToggle->setText(TSYS::strMess(_("Append to favorite for '%s'"),"?").c_str());
    actFavToggle->setToolTip(actFavToggle->text());
    actFavToggle->setWhatsThis(_("The button to toggle the current page as favorite"));
    actFavToggle->setStatusTip(_("Press to toggle the current page as favorite."));
    // Favorite page
    actFav->setText(_("Favorite"));
    actFav->setToolTip(_("Go to favorite"));
    actFav->setWhatsThis(_("The button-menu to go the favorite page"));
    actFav->setStatusTip(_("Press to go the favorite page."));
    // About "System info"
    actAbout->setText(_("&About"));
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button of the information of the program and OpenSCADA"));
    actAbout->setStatusTip(_("Press for information of the program and OpenSCADA."));
    // About Qt
    actQtAbout->setText(_("About &Qt"));
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using Qt information"));
    actQtAbout->setStatusTip(_("Press for getting the using Qt information."));
    // QTCfg manual
    actManual->setText(QString(_("Manual on '%1'")).arg(mod->modId().c_str()));
    actManual->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(mod->modId().c_str()));
    // OpenSCADA manual index
    actManualSYS->setText(QString(_("Manual on '%1'")).arg(PACKAGE_STRING));
    actManualSYS->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(PACKAGE_STRING));
    // Page help
    actManualPage->setText(_("Manual on the page"));
    actManualPage->setToolTip(_("Selected page manual"));
    actManualPage->setWhatsThis(_("The button for getting the using selected page manual."));
    actManualPage->setStatusTip(_("Press to get the using selected page manual."));
    // What is
    actWhatIs->setText(_("What's &This"));
    actWhatIs->setToolTip(_("Requesting about GUI elements"));
    actWhatIs->setWhatsThis(_("The button for requesting information about the user interface elements"));
    actWhatIs->setStatusTip(_("Press for requesting information about the user interface elements."));

    //Menus
    menuFile->setTitle(_("&File"));
    menuEdit->setTitle(_("&Edit"));
    menuView->setTitle(_("&View"));
    menuHelp->setTitle(_("&Help"));
    emit makeStarterMenu(NULL, lang().c_str());

    //Main tool bar
    toolBar->setWindowTitle(_("Main toolbar"));
    emit makeStarterMenu(QTStarter, lang().c_str());

    //Status bars
    wUser->setWhatsThis(_("This label displays the current user."));
    wUser->setToolTip(_("Field for displaying the current user."));
    wUser->setStatusTip(_("Double click to change the user."));
    mStModify->setWhatsThis(_("This label indicates the fact of making changes."));
    mStModify->setToolTip(_("Field for indicating the fact of making changes."));
    stBt->setWhatsThis(_("This button calls the status bar history."));
    stBt->setToolTip(_("Button for calling the status bar history."));
}

void ConfApp::quitSt( )
{
    if(exitModifChk()) SYS->stop();
}

bool ConfApp::exitModifChk( )
{
    //Check for no saved local station
    XMLNode req("modify");
    req.setAttr("path","/"+SYS->id()+"/%2fobj");
    if(!cntrIfCmd(req) && s2i(req.text())) {
	bool saveExit = false;
	req.clear()->setName("get")->setAttr("path","/"+SYS->id()+"/%2fgen%2fsaveExit");
	if(!cntrIfCmd(req))	saveExit |= s2i(req.text());
	req.setAttr("path","/"+SYS->id()+"/%2fgen%2fsavePeriod");
	if(!cntrIfCmd(req))	saveExit |= s2i(req.text());
	if(!saveExit) {
	    int ret = QMessageBox::information(this,_("Saving the changes"),
		_("Some changes were made!\nSave the changes to storage before exiting?"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Yes);
	    switch(ret) {
		case QMessageBox::Yes:
		    req.clear()->setName("save")->setAttr("path","/"+SYS->id()+"/%2fobj")->setAttr("primaryCmd", "1");
		    cntrIfCmd(req);
		    return true;
		case QMessageBox::No:
		    return true;
		case QMessageBox::Cancel:
		    return false;
	    }
	}
    }
    return true;
}

void ConfApp::hostStSet( const QString &hid, int lnkOK, const QImage &img, const QStringList &brs, const QString &toolTip )
{
    for(unsigned iTop = 0; iTop < (unsigned)CtrTree->topLevelItemCount(); ++iTop) {
	QTreeWidgetItem *nit = CtrTree->topLevelItem(iTop);
	if(hid.toStdString() != TSYS::pathLev(nit->text(2).toStdString(),0)) continue;
	nit->setIcon(0, img.isNull() ? QPixmap() : QPixmap::fromImage(img).scaled(icoSize(),icoSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
	if(lnkOK > 0 && !nit->data(0,Qt::UserRole).toBool()) {
	    //? Used for rechange status for fix indicator hide after all childs remove on bad connection
	    nit->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
	    nit->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	}
	nit->setData(2, Qt::UserRole, brs);
	if(lnkOK == 0) nit->setExpanded(false);
	if(lnkOK >= 0) {
	    nit->setToolTip(0, toolTip);
	    nit->setData(0, Qt::UserRole, (bool)lnkOK);
	}
	break;
    }
}

void ConfApp::endRunChk( )
{
    if(mod->endRun()) close();
}

void ConfApp::treeSearch( )
{
    if(!sender()) return;
    QLineEdit *sl = (QLineEdit*)sender();
    QString wvl = sTrm(sl->text().toStdString()).c_str();
    bool fromCur = !sl->isModified();
    sl->setModified(false);

    //Get current element selection
    QTreeWidgetItem *si = (CtrTree->selectedItems().size()==1) ? CtrTree->selectedItems()[0] : NULL;
    if(!si || wvl.isEmpty()) return;

    QTreeWidgetItem *pi = si->parent();
    if(!pi || !pi->isExpanded()) return;

    bool curReach = false;
    int iC;
    for(iC = 0; iC < pi->childCount(); iC++) {
	if(fromCur && !curReach) {
	    if(pi->child(iC) == si) curReach = true;
	    continue;
	}
	if(pi->child(iC)->text(0).contains(wvl,Qt::CaseInsensitive)) break;
	else if(QString(TSYS::pathLevEnd(pi->child(iC)->text(2).toStdString(),0).c_str()).contains(wvl,Qt::CaseInsensitive)) break;
    }
    if(iC < pi->childCount()) {
	pi->treeWidget()->setCurrentItem(pi->child(iC), 0, QItemSelectionModel::Clear|QItemSelectionModel::Select);
	pi->treeWidget()->scrollTo(pi->treeWidget()->currentIndex());
    }
    else if(fromCur) { sl->setModified(true); treeSearch(); }
}

void ConfApp::pageUp( )
{
    size_t iL = string::npos;
    while(true) {
	iL = selPath.rfind("/", iL);
	if(iL == string::npos || iL == 0) return;
	if((selPath.size()-iL) > 1) break;
	iL--;
    }

    selectPage(selPath.substr(0,iL));
}

void ConfApp::pagePrev( )
{
    if(!prev.size()) return;

    string path = prev[0];
    XMLNode *tabN = root->childGet("area", tabs->currentIndex(), true);
    if(sender() && !sender()->objectName().isEmpty() && sender()->objectName().toStdString() != path) {
	path = sender()->objectName().toStdString();
	if(prev.empty() || TSYS::strParse(prev[0],0,"#") != selPath)
	    prev.insert(prev.begin(), selPath+(tabN?"#"+tabN->attr("id"):""));
    }
    else {
	next.insert(next.begin(), selPath+(tabN?"#"+tabN->attr("id"):""));
	prev.erase(prev.begin());
    }

    try{ pageDisplay(path); } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::pageNext( )
{
    if(!next.size()) return;

    string path = next[0];
    XMLNode *tabN = root->childGet("area", tabs->currentIndex(), true);
    if(sender() && !sender()->objectName().isEmpty() && sender()->objectName().toStdString() != path) {
	path = sender()->objectName().toStdString();
	if(prev.empty() || TSYS::strParse(prev[0],0,"#") != selPath)
	    prev.insert(prev.begin(), selPath+(tabN?"#"+tabN->attr("id"):""));
    }
    else {
	prev.insert(prev.begin(), selPath+(tabN?"#"+tabN->attr("id"):""));
	next.erase(next.begin());
    }

    try{ pageDisplay(path); } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::itDBLoad( )
{
    XMLNode req("load");
    req.setAttr("path", selPath+"/%2fobj")->setAttr("primaryCmd", "1")->setAttr("force", (sender()==actDBLoadF)?"1":"");
    if(cntrIfCmd(req)) mod->postMessCntr(req, this);
    pageRefresh();	//Any time but warnings in the deep
}

void ConfApp::itDBSave( )
{
    XMLNode req("save");
    req.setAttr("path",selPath+"/%2fobj")->setAttr("primaryCmd", "1")->setAttr("force", (sender()==actDBSaveF)?"1":"");
    if(cntrIfCmd(req)) mod->postMessCntr(req, this);
    else pageRefresh();
}

void ConfApp::itAdd( )
{
    if(selPath.empty() || !root->childGet("id","br",true)) return;
    XMLNode *branch = root->childGet("id", "br");

    //Load branches list
    vector<string> brs;
    string tVl, tHint = actItAdd->property("grpHint").toString().toStdString();
    for(unsigned iB = 0; iB < branch->childSize(); iB++)
	if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR) {
	    tVl = branch->childGet(iB)->attr("idSz") + "\n" + branch->childGet(iB)->attr("idm") + "\n" +
		  branch->childGet(iB)->attr("id") + "\n" + branch->childGet(iB)->attr("dscr");
	    if(tHint == branch->childGet(iB)->attr("id") && brs.size()) brs.insert(brs.begin(), tVl);
	    else brs.push_back(tVl);
	}
    if(!brs.size()) { mod->postMess(mod->nodePath(), _("No edited container is present."), TUIMod::Info, this); return; }

    ReqIdNameDlg dlg(this, actItAdd->icon(), QString(_("Adding an item to the node '%1'.")).arg(selPath.c_str()),_("Adding a node"));
    dlg.setTargets(brs);
    if(dlg.exec() != QDialog::Accepted)	return;

    //Check for already present node
    XMLNode req("get");
    req.setAttr("path",selPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target(),2,'\n'));
    if(!cntrIfCmd(req))
	for(unsigned iLel = 0; iLel < req.childSize(); iLel++)
	    if((req.childGet(iLel)->attr("id").size() && req.childGet(iLel)->attr("id") == dlg.id().toStdString()) ||
	       (!req.childGet(iLel)->attr("id").size() && req.childGet(iLel)->text() == dlg.id().toStdString()))
	    {
		mod->postMess(mod->nodePath(), QString(_("The node '%1' is already present.")).arg(dlg.id()).toStdString(), TUIMod::Info, this);
		return;
	    }

    //Send create request
    req.clear()->
	setName("add")->
	setAttr("path",selPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target(),2,'\n'));
    if(s2i(TSYS::strSepParse(dlg.target(),1,'\n')))
	req.setAttr("id",dlg.id().toStdString())->setText(dlg.name().toStdString());
    else req.setText(dlg.id().toStdString());
    if(cntrIfCmd(req)) mod->postMessCntr(req, this);
    else { treeUpdate(); pageRefresh(); }
}

void ConfApp::itDel( const string &iit )
{
    string rmits = iit, rmit, rmits_lim = iit;
    if(iit.empty()) {
	QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
	if(sel_ls.size() <= 1) rmits = rmits_lim = selPath;
	else for(int iEl = 0; iEl < sel_ls.size(); iEl++) {
	    rmit = sel_ls.at(iEl)->text(2).toStdString() + ((iEl<(sel_ls.size()-1))?"\n":"");
	    rmits += rmit;
	    if(iEl < GRP_SHOW_OP_LIM) rmits_lim += rmit;
	}
	if(sel_ls.size() > GRP_SHOW_OP_LIM)
	    rmits_lim += TSYS::strMess(_("... and yet %d nodes"), sel_ls.size()-GRP_SHOW_OP_LIM);
    }
    if(rmits.empty())	return;

    if(iit.empty()) {
	InputDlg dlg(this, actItDel->icon(),
		QString(_("Are you sure of deleting the nodes: %1 ?")).arg(rmits_lim.c_str()),_("Deleting the nodes"), 0, 0);
	if(dlg.exec() != QDialog::Accepted)	return;
    }

    string firstOwn;
    for(int roff = 0; (rmit=TSYS::strSepParse(rmits,0,'\n',&roff)).size(); ) {
	string t_el, sel_own, sel_el;
	int n_obj = 0;
	for(int off = 0; !(t_el=TSYS::pathLev(rmit,0,true,&off)).empty(); n_obj++)
	{ if(n_obj) sel_own += "/" +sel_el; sel_el = t_el; }
	if(n_obj > 2) {
	    XMLNode req("info");
	    req.setAttr("path", sel_own+"/%2fbr");
	    if(cntrIfCmd(req) || !req.childGet(0,true)) return;

	    XMLNode *branch = req.childGet(0);
	    for(unsigned iB = 0; iB < branch->childSize(); iB++) {
		string b_id = branch->childGet(iB)->attr("id");
		if(b_id == sel_el.substr(0,b_id.size()) && s2i(branch->childGet(iB)->attr("acs"))&SEC_WR) {
		    bool idm = s2i(branch->childGet(iB)->attr("idm"));
		    req.clear()->setName("del")->setAttr("path", sel_own+"/%2fbr%2f"+b_id);
		    if(idm) req.setAttr("id", sel_el.substr(b_id.size()));
		    else req.setText(sel_el.substr(b_id.size()));
		    if(cntrIfCmd(req)) mod->postMessCntr(req, this);
		    else {
			if(firstOwn.empty())	firstOwn = sel_own;
			QTreeWidgetItem *tIt = getExpandTreeWIt(rmit);
			if(tIt) {
			    CtrTree->blockSignals(true);
			    tIt->parent()->takeChild(tIt->parent()->indexOfChild(tIt));
			    CtrTree->blockSignals(false);
			}
		    }
		    break;
		}
	    }
	}
    }

    if(firstOwn.size())	pageDisplay(firstOwn);
}

void ConfApp::itCut( )
{
    copyBuf = "1";
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() <= 1) copyBuf += selPath;
	else for(int iEl = 0; iEl < sel_ls.size(); iEl++)
	    copyBuf += sel_ls.at(iEl)->text(2).toStdString()+"\n";
    editToolUpdate();
}

void ConfApp::itCopy( )
{
    copyBuf = "0";
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() <= 1) copyBuf += selPath;
    else for(int iEl = 0; iEl < sel_ls.size(); iEl++)
	    copyBuf += sel_ls.at(iEl)->text(2).toStdString()+"\n";
    editToolUpdate();
}

void ConfApp::itPaste( )
{
    int off;
    string sEl, sElp, tEl, bGrp, copyEl, toPath, chSel;
    QCheckBox *prcReq = NULL, *prcAlrPres = NULL;
    XMLNode parNode("info"), *rootW = root;
    bool prcReqMiss = false, prcAlrPresMiss = false;

    bool isCut = (copyBuf[0] == '1');
    bool isMult = !TSYS::strParse(copyBuf,1,"\n").empty();

    for(int elOff = 1; (copyEl=TSYS::strParse(copyBuf,0,"\n",&elOff)).size(); ) {
	rootW = root;
	toPath = selPath;

	//Src elements calc
	int nSel = 0;
	for(off = 0; !(tEl=TSYS::pathLev(copyEl,0,true,&off)).empty(); nSel++)
	{ if(nSel) sElp += "/" + sEl; sEl = tEl; }

	//if(TSYS::pathLev(copyEl,0) != TSYS::pathLev(toPath,0))
	//{ mod->postMess(mod->nodePath(), _("Copying is not possible."), TUIMod::Error, this); return; }

	vector<string> brs;
	if(copyEl == toPath) {	//For copy into the branch and do not select directly the parent node
	    toPath = sElp;
	    parNode.setAttr("path", toPath);
	    if(cntrIfCmd(parNode)) continue;
	    rootW = parNode.childGet(0);
	}
	if(s2i(rootW->attr("acs"))&SEC_WR) brs.push_back(string("-1\n0\n\n")+_("Selected"));

	string statNm, statNmSrc, srcNm, dstNm;
	off = 0; statNmSrc = TSYS::pathLev(copyEl, 0, true, &off);srcNm = copyEl.substr(off);
	off = 0; statNm = TSYS::pathLev(toPath, 0, true, &off);	dstNm = toPath.substr(off);

	XMLNode *branch = rootW->childGet("id", "br", true);
	for(unsigned iB = 0; branch && iB < branch->childSize(); iB++)
	    if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR) {
		string gbrId = branch->childGet(iB)->attr("id");
		brs.push_back(branch->childGet(iB)->attr("idSz")+"\n0\n"+gbrId+"\n"+branch->childGet(iB)->attr("dscr"));
		if(sEl.substr(0,gbrId.size()) == gbrId) { brs[brs.size()-1] = brs[brs.size()-1]+"\n1"; bGrp = gbrId; }
	    }

	//Make request dialog
	ReqIdNameDlg dlg(this, actItPaste->icon(), "", _("Moving or copying the node"));
	dlg.setTargets(brs);
	dlg.setMess(QString(isCut?_("Moving the node '%1' to '%2'.\n"):_("Copying the node '%1' to '%2'.\n")).arg(copyEl.c_str()).arg(toPath.c_str()));
	dlg.setId(sEl.substr(bGrp.size()).c_str());
	if(statNm != statNmSrc) dlg.setPassive();
	if(isMult) {
	    prcReq = new QCheckBox(_("Do not ask for more."), &dlg);
	    dlg.edLay->addWidget(prcReq, 5, 0, 1, 2);
	}
	if(!prcReqMiss && dlg.exec() != QDialog::Accepted) return;
	if(!prcReqMiss && prcReq && prcReq->checkState() == Qt::Checked) prcReqMiss = true;
	if(s2i(TSYS::strSepParse(dlg.target(),0,'\n')) >= 0) {
	    dstNm += "/" +TSYS::strSepParse(dlg.target(),2,'\n') + dlg.id().toStdString();
	    // Check for already present node
	    XMLNode req("get");
	    req.setAttr("path", toPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target(),2,'\n'));
	    if(cntrIfCmd(req)) { mod->postMessCntr(req, this); return; }
	    for(unsigned iLel = 0; iLel < req.childSize(); iLel++)
		if((req.childGet(iLel)->attr("id").size() && req.childGet(iLel)->attr("id") == dlg.id().toStdString()) ||
		   (!req.childGet(iLel)->attr("id").size() && req.childGet(iLel)->text() == dlg.id().toStdString()))
		{
		    InputDlg dlg1(this, actItPaste->icon(), QString(_("The node '%1' is already present.\nContinue?")).arg(dstNm.c_str()),
					_("Moving or copying the node"), 0, 0);
		    if(isMult) {
			prcAlrPres = new QCheckBox(_("Do not ask for more."), &dlg1);
			dlg1.edLay->addWidget(prcAlrPres, 5, 0, 5, 1);
		    }
		    if(!prcAlrPresMiss && dlg1.exec() != QDialog::Accepted) return;
		    if(!prcAlrPresMiss && prcAlrPres && prcAlrPres->checkState() == Qt::Checked) prcAlrPresMiss = true;
		    break;
		}
	}

	try {
	    //Local copy visual item
	    if(statNm == statNmSrc) {
		XMLNode req("copy");
		req.setAttr("path", "/"+statNm+"/%2fobj")->setAttr("src", srcNm)->setAttr("dst", dstNm);
		if(cntrIfCmd(req))
		    throw TError(s2i(req.attr("rez")), req.attr("mcat"), req.text());
	    }
	    //Interstation copy
	    else {
		XMLNode req("add");
		//Create the destination node
		req.setAttr("path", selPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target(),2,'\n'))->
		    setAttr("id", dlg.id().toStdString())->setText(dlg.id().toStdString());
		/*if(s2i(TSYS::strSepParse(dlg.target(),1,'\n')))
		    req.setAttr("id", dlg.id().toStdString())->setText(dlg.name().toStdString());
		else req.setText(dlg.id().toStdString());*/
		if(cntrIfCmd(req))
		    throw TError(s2i(req.attr("rez")), req.attr("mcat"), req.text());

		//Get context of the source node
		req.clear()->setName("save")->setAttr("path", "/"+statNmSrc+srcNm+"/%2fobj")->setAttr("ctx", "1");
		if(cntrIfCmd(req))
		    throw TError(s2i(req.attr("rez")), req.attr("mcat"), req.text());

		//Load context of the source node to the destination one
		req.setName("load")->setAttr("path", "/"+statNm+dstNm+"/%2fobj")->setAttr("primaryCmd", "1")->attrDel("ctx")->attrDel("rez");
		if(cntrIfCmd(req))
		    throw TError(s2i(req.attr("rez")), req.attr("mcat"), req.text());
	    }
	} catch(TError &err) {
	    if(elOff >= (int)copyBuf.size()) mod->postMess(err.cat, err.mess, TUIMod::Error, this);
	    else if(InputDlg(this,actItPaste->icon(),
			QString(_("Copy/Move node '%1' error '%2'.\nContinue other nodes?")).arg(dstNm.c_str()).arg(err.mess.c_str()),
			_("Moving or copying the node"),0,0).exec() == QDialog::Accepted)
		    continue;
	    pageRefresh();
	    return;
	}

	//Remove source widget
	if(isCut) {
	    itDel(copyEl);
	    if(selPath == copyEl) chSel = "/"+statNm+"/"+dstNm;
	}
    }

    if(isCut) copyBuf = "0";

    treeUpdate();
    if(chSel.size()) selectPage(chSel);
    else pageRefresh();
}

void ConfApp::favToggle( )
{
    string selNmPath = getTreeWItNmPath(selPath);

    int fvPresent = -1;
    for(unsigned iFv = 0; fvPresent < 0 && iFv < favs.size(); ++iFv)
	if(TSYS::strParse(TSYS::strParse(favs[iFv],0,":"),0,"#") == selPath) fvPresent = iFv;

    if(fvPresent >= 0) {
	favs.erase(favs.begin()+fvPresent);
	actFavToggle->setIcon(QPixmap::fromImage(favToggleAdd));
	actFavToggle->setText(TSYS::strMess(_("Append to favorite for '%s'"),(selNmPath.size()?selNmPath:selPath).c_str()).c_str());
    }
    else {
	XMLNode *tabN = root->childGet("area", tabs->currentIndex(), true);
	favs.push_back(selPath + (tabN?"#"+tabN->attr("id"):"") + (selNmPath.size()?":"+selNmPath:""));
	while(favs.size() > limCacheIts_N) favs.erase(favs.begin());

	actFavToggle->setIcon(QPixmap::fromImage(favToggleDel));
	actFavToggle->setText(TSYS::strMess(_("Remove from favorite for '%s'"),(selNmPath.size()?selNmPath:selPath).c_str()).c_str());
    }
    actFavToggle->setToolTip(actFavToggle->text());

    string sfavs;
    for(unsigned iFv = 0; iFv < favs.size(); ++iFv)
	sfavs += (sfavs.size()?"\n":"") + favs[iFv];

    TBDS::genPrmSet(mod->nodePath()+"favorites", sfavs, user());

    favUpd(Fav_List);
}

void ConfApp::favUpd( unsigned opts )
{
    if(opts&Fav_Reload) {
	favs.clear();
	string sfavs = TBDS::genPrmGet(mod->nodePath()+"favorites", "", user()), fav;
	for(int off = 0; (fav=TSYS::strLine(sfavs,0,&off)).size() || off < (int)sfavs.size(); )
	    favs.push_back(fav);
    }

    if(opts&Fav_List) {
	actFav->setMenu(new QMenu(this));

	QAction *actFavLnk;
	for(int iFv = (int)favs.size()-1; iFv >= 0; --iFv) {
	    int off = 0;
	    string spath = TSYS::strParse(favs[iFv], 0, ":", &off);
	    actFavLnk = new QAction(((off<(int)favs[iFv].size())?favs[iFv].substr(off):spath).c_str(), this);
	    actFavLnk->setObjectName(spath.c_str());
	    actFav->menu()->addAction(actFavLnk);
	    connect(actFavLnk, SIGNAL(triggered()), this, SLOT(favGo()));
	}
	if(favs.size()) {
	    actFav->menu()->addSeparator();
	    actFavLnk = new QAction(_("Clear the list"), this);
	    actFav->menu()->addAction(actFavLnk);
	    connect(actFavLnk, SIGNAL(triggered()), this, SLOT(favGo()));
	}
    }

    if(opts&Fav_Sel) {
	actFavToggle->setEnabled(selPath.size());
	string selNmPath = getTreeWItNmPath(selPath);
	if(selNmPath.size()) {
	    bool fvPresent = false;
	    for(unsigned iFv = 0; !fvPresent && iFv < favs.size(); ++iFv)
		fvPresent = (TSYS::strParse(TSYS::strParse(favs[iFv],0,":"),0,"#") == selPath);
	    if(!fvPresent) {
		actFavToggle->setIcon(QPixmap::fromImage(favToggleAdd));
		actFavToggle->setText(TSYS::strMess(_("Append to favorite for '%s'"),selNmPath.c_str()).c_str());
	    }
	    else {
		actFavToggle->setIcon(QPixmap::fromImage(favToggleDel));
		actFavToggle->setText(TSYS::strMess(_("Remove from favorite for '%s'"),selNmPath.c_str()).c_str());
	    }
	    actFavToggle->setToolTip(actFavToggle->text());
	}
    }

    actFav->setEnabled(favs.size());
}

void ConfApp::favGo( )
{
    if(!sender()) return;
    QAction *sa = (QAction*)sender();
    if(sa->menu() && sa->menu()->actions().size()) sa = sa->menu()->actions()[0];

    if(sa->objectName().isEmpty()) {
	TBDS::genPrmSet(mod->nodePath()+"favorites", "", user());
	favUpd(Fav_Reload|Fav_List|Fav_Sel);
    }
    else try {
	//Prev and next
	if(selPath.size()) {
	    XMLNode *tabN = root->childGet("area", tabs->currentIndex(), true);
	    prev.insert(prev.begin(), selPath+(tabN?"#"+tabN->attr("id"):""));
	}
	if((int)prev.size() >= queSz)	prev.pop_back();
	next.clear();

	pageDisplay(sa->objectName().toStdString());
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::editToolUpdate( )
{
    int rootAccess = root ? s2i(root->attr("acs")) : 0;
    actItCut->setEnabled(selPath.size() && (rootAccess&SEC_WR));
    actItCopy->setEnabled((rootAccess>>3)?((rootAccess>>3)&SEC_WR):selPath.size());
    actItPaste->setEnabled(false);

    if(TSYS::strParse(copyBuf,1,"\n").empty()) {
	//Src and destination elements calc
	if(copyBuf.size() <= 1 /*|| copyBuf.substr(1) == selPath || TSYS::pathLev(copyBuf.substr(1),0) != TSYS::pathLev(selPath,0)*/)
	    return;
	string sElp, sEl, tEl;
	for(int off = 0; !(tEl=TSYS::pathLev(copyBuf.substr(1),0,true,&off)).empty(); )
	{ sElp += "/" + sEl; sEl = tEl; }

	if(rootAccess&SEC_WR) actItPaste->setEnabled(true);
    }

    XMLNode *branch = root ? root->childGet("id", "br", true) : NULL;
    if(branch)
	for(unsigned iB = 0; iB < branch->childSize(); iB++)
	    if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR)
	    { actItPaste->setEnabled(true); break; }
}

void ConfApp::treeUpdate( )
{
    for(int iT = 0; iT < CtrTree->topLevelItemCount(); iT++)
	if(CtrTree->topLevelItem(iT)->isExpanded())
	    viewChildRecArea(CtrTree->topLevelItem(iT), true);
}

void ConfApp::userSel( )
{
    messUpd();

    initHosts(true);

    pgInfo.setAttr("path", "");

    try { pageDisplay("/"+SYS->id()+mod->startPath()); }
    catch(TError &err) { pageDisplay("/"+SYS->id()); }

    treeUpdate();
    favUpd(Fav_Reload|Fav_List);
}

void ConfApp::pageRefresh( int tm )
{
    if(tm) {
	if(!actStartUpd->isEnabled())	return;

	autoUpdTimer->setSingleShot(true);
	autoUpdTimer->start(tm);

	return;
    }

    try {
	//Tree part update.
	//!!!! That is bad here for big items number, at least for about 2000 ones, in updating at selecting any item
	if(CtrTree->currentItem() && !pgDisplay && dynamic_cast<QAction*>(sender()))
	    viewChildRecArea(CtrTree->currentItem()->parent() ? CtrTree->currentItem()->parent() : CtrTree->currentItem(), true);

	//Same page update
	pageDisplay(selPath);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::pageCyclRefrStart( )
{
    actStartUpd->setEnabled(false);
    actStopUpd->setEnabled(true);
    actUpdate->setEnabled(false);

    autoUpdTimer->setSingleShot(false);
    autoUpdTimer->start(1000);
}

void ConfApp::pageCyclRefrStop( )
{
    actStopUpd->setEnabled(false);
    actStartUpd->setEnabled(true);
    actUpdate->setEnabled(true);

    autoUpdTimer->stop();
}

void ConfApp::stMessChanged( const QString &mess )
{
    if(mess.isEmpty()) return;

    stMess.push_back(mess.toStdString());
    if(stMess.size() > 100) stMess.erase(stMess.begin());
}

void ConfApp::stHistCall( )
{
    InputDlg dlg(this,QIcon(),QString(_("List of messages in the status bar:")),_("Status messages"),0,0,QDialogButtonBox::Ok);
    QTextBrowser *tb = new QTextBrowser(&dlg);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setVerticalStretch(10);
    tb->setSizePolicy(sizePolicy);
    tb->setMinimumSize(600, 300);
    dlg.edLay->addWidget(tb, 0, 0);
    string textv;
    for(int iE = stMess.size()-1; iE >= 0; iE--) textv += stMess[iE]+"\n";
    tb->setPlainText(textv.c_str());
    dlg.exec();
}

void ConfApp::about( )
{
    string mess = _("%s v%s on Qt v%s.\n%s\nAuthor: %s\nLicense: %s\n\n"
		    "%s v%s.\n%s\nLicense: %s\nAuthor: %s\nWeb site: %s");

#undef _
#define _(mess) Mess->I18N(mess, lang().c_str()).c_str()

    QMessageBox::about(this, windowTitle(),
	TSYS::strMess(mess.c_str(),
	    _(mod->modInfo("Name")),mod->modInfo("Version").c_str(),QT_VERSION_STR,_(mod->modInfo("Description")),
	    _(mod->modInfo("Author")),mod->modInfo("License").c_str(),
	    PACKAGE_NAME,VERSION,_(PACKAGE_DESCR),PACKAGE_LICENSE,_(PACKAGE_AUTHOR),PACKAGE_SITE).c_str());

#undef _
#define _(mess) mod->I18N(mess, lang().c_str()).c_str()
}

void ConfApp::aboutQt( )	{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void ConfApp::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void ConfApp::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString()+"\n"+lang());
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("The manual '%1' was not found offline or online!")).arg(sender()->property("doc").toString()));
}

void ConfApp::closeEvent( QCloseEvent* ce )
{
    if(!SYS->stopSignal() && !property("forceClose").toBool() && !mod->endRun() && winCntr <= 1 && !exitModifChk()) {
	ce->ignore();
	return;
    }

    winClose = true;

    //Call for next processing by the events handler for the real closing after release all background requests
    if(inHostReq || pgDisplay) { ce->ignore(); return; }

    if(endRunTimer->isActive()) {
	//Save the generic state
	QByteArray st = splitter->saveState();
	TBDS::genPrmSet(mod->nodePath()+"st", i2s(width())+":"+i2s(height())+":"+TSYS::strEncode(string(st.data(),st.size()),TSYS::base64,""), user());

	// Timers early stop
	endRunTimer->stop();
	autoUpdTimer->stop();
	reqPrgrsTimer->stop();
    }

    ce->accept();
}

void ConfApp::resizeEvent( QResizeEvent *rszEv )
{
    if((rszEv->size()-rszEv->oldSize()).height() && actStartUpd->isEnabled()) pageRefresh(500);
}

void ConfApp::selectItem( )
{
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() == 1 && selPath != sel_ls.at(0)->text(2).toStdString()) {
	selectPage(sel_ls.at(0)->text(2).toStdString(), CH_REFR_TM);

	//!!!! Due to that performed in pageDisplay() already
	/*if((sel_ls=CtrTree->selectedItems()).size()) {	//Updating but it can be changed after "selectPage"
	    int saveVl = CtrTree->horizontalScrollBar() ? CtrTree->horizontalScrollBar()->value() : 0;
	    CtrTree->scrollToItem(sel_ls.at(0), QAbstractItemView::EnsureVisible);
	    if(CtrTree->horizontalScrollBar()) CtrTree->horizontalScrollBar()->setValue(saveVl);
	}*/
    }
}

bool ConfApp::eventFilter( QObject *obj, QEvent *event )
{
    if(event->type() == QEvent::Wheel && qobject_cast<QComboBox*>(obj) /*&& !((QComboBox*)obj)->hasFocus()*/) {
	event->ignore();
	return true;
    }

    return false;
}

void ConfApp::selectPage( const string &path, int tm )
{
    try {
	//Prev and next
	if(selPath.size() && (prev.empty() || TSYS::strParse(prev[0],0,"#") != selPath)) {
	    XMLNode *tabN = root->childGet("area", tabs->currentIndex(), true);
	    prev.insert(prev.begin(), selPath+(tabN?"#"+tabN->attr("id"):""));
	}
	if((int)prev.size() >= queSz)	prev.pop_back();
	next.clear();

	//Display page
	if(tm > 0) {
	    selPath = path;
	    pageRefresh(tm);
	} else pageDisplay(path);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget )
{
    //For append no named widgets
    QHBoxLayout *l_hbox = NULL;
    int		iArea = 0;	//Areas counter
    int		l_pos = 0;
    int		rez;

    //View title name
    if(a_path == "/") {
	// Set node icon
	if(node.childGet("id","ico",true)) {
	    XMLNode req("get");
	    req.setAttr("path", TSYS::strEncode(a_path+"ico",TSYS::PathEl));
	    if((rez=cntrIfCmd(req)) > TError::NoError) mod->postMessCntr(req, this);
	    else if(rez == TError::NoError) {
		string simg = TSYS::strDecode(req.text(), TSYS::base64);
		QImage img;
		if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
		    titleIco->setPixmap(QPixmap::fromImage(img.scaled(icoSize(2),icoSize(2),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
		else titleIco->clear();
	    }
	} else titleIco->clear();

	// Set title
	titleLab->setText((string("<p align='center'><i><b>")+TSYS::strEncode(node.attr("dscr"),TSYS::Html)+"</b></i></p>").c_str());

	// Delete tabs of deleted areas
	bool actRemoved = false;
	for(int iTbs = 0; iTbs < tabs->count(); iTbs++) {
	    unsigned iCf;
	    for(iCf = 0; iCf < node.childSize(); iCf++)
		if(node.childGet(iCf)->name() == "area" && tabs->tabText(iTbs) == node.childGet(iCf)->attr("dscr").c_str())
		    break;
	    if(iCf >= node.childSize()) {
		if(tabs->currentIndex() == iTbs) actRemoved = true;
		tabs->widget(iTbs)->deleteLater();
		tabs->removeTab(iTbs);
		iTbs--;
	    }
	}
	// Add new tabs
	for(unsigned iCf = 0; iCf < node.childSize(); iCf++) {
	    XMLNode &t_s = *node.childGet(iCf);
	    if(t_s.name() != "area")	continue;

	    int iTbs;
	    for(iTbs = 0; iTbs < tabs->count(); iTbs++)
		if(tabs->tabText(iTbs) == t_s.attr("dscr").c_str())
		    break;
	    if(iTbs >= tabs->count()) {
		QScrollArea *scrl = new QScrollArea();
		tabs->insertTab(iArea, scrl, t_s.attr("dscr").c_str());
		t_s.setAttr("qview", "0");
	    }

	    //  Find and prepare current tab
	    if(actRemoved && iArea == 0)	tabs->setCurrentIndex(0);
	    if(tabs->currentIndex() == iArea) {
		if(!s2i(t_s.attr("qview"))) {
		    QScrollArea *scrl = (QScrollArea*)tabs->widget(iArea);
		    int v_scrl = (scrl->verticalScrollBar()) ? scrl->verticalScrollBar()->value() : 0;

		    QWidget *wdg = new QFrame;
		    QVBoxLayout *wdg_lay = new QVBoxLayout(wdg);
		    wdg_lay->setSpacing(3);
		    wdg_lay->setAlignment(Qt::AlignTop);

		    selectChildRecArea(t_s, a_path+t_s.attr("id")+'/', wdg);
		    scrl->setWidget(wdg);
                    //wdg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
		    scrl->setWidgetResizable(true);
		    if(scrl->verticalScrollBar()) scrl->verticalScrollBar()->setValue(v_scrl);

		    //  Mark last drawed tabs
		    t_s.setAttr("qview", "1");
		}
		else selectChildRecArea(t_s, a_path+t_s.attr("id")+"/");

		// Elements of scalable by vertical get and their grow up to the scroll appear in the container
		if(!s2i(genReqs.attr("fillMode"))) {
		    QScrollArea *scrl = (QScrollArea*)tabs->widget(iArea);
		    QWidget *lastW = qobject_cast<QWidget*>(scrl->widget()->children().last());

		    QList<TextEdit*> texts = scrl->findChildren<TextEdit*>();
		    QList<QTableWidget*> tbls = scrl->findChildren<QTableWidget*>();
		    QList<ListView*> lsts = scrl->findChildren<ListView*>();

		    //Fit reset
		    for(int iL = 0; iL < texts.size(); iL++)	texts[iL]->setMinimumHeight(0);
		    for(int iL = 0; iL < tbls.size(); iL++)	tbls[iL]->setMinimumHeight(0);
		    for(int iL = 0; iL < lsts.size(); iL++)	lsts[iL]->setMinimumHeight(0);

		    for(int iTr = 0; iTr < 5; iTr++) qApp->processEvents();	//Call all cascade events

		    int sclFitSz = lastW ? (scrl->maximumViewportSize().height() - (lastW->y()+lastW->height()) - 10): 0;

		    //Same fitting
		    for(int fitStp = vmax(5, sclFitSz/(8*vmax(1,texts.length()+tbls.length()+lsts.length()))), iScN = 0; sclFitSz > fitStp; ) {
			QAbstractScrollArea *sclIt = NULL, *tEl = NULL;
			bool sclFromBeg = (iScN == 0);
			for( ; iScN < (texts.length()+tbls.length()+lsts.length()) && !sclIt; iScN++) {
			    if(iScN < texts.length()) {
				if(!(tEl=dynamic_cast<QAbstractScrollArea*>(texts[iScN]->edit()))) continue;
				if(!tEl->verticalScrollBar() || !tEl->verticalScrollBar()->maximum())	continue;
				texts[iScN]->setMinimumHeight(fmax(texts[iScN]->height(),texts[iScN]->minimumHeight())+fitStp);
			    }
			    else if(iScN < (texts.length()+tbls.length())) {
				int iScN_ = iScN - texts.length();
				if(!(tEl=dynamic_cast<QAbstractScrollArea*>(tbls[iScN_]))) continue;
				if(!tEl->verticalScrollBar() || !tEl->verticalScrollBar()->maximum())	continue;
				tbls[iScN_]->setMinimumHeight(fmax(tbls[iScN_]->height(),tbls[iScN_]->minimumHeight())+fitStp);
			    }
			    else if(iScN < (texts.length()+tbls.length()+lsts.length())) {
				int iScN_ = iScN - texts.length() - tbls.length();
				if(!(tEl=dynamic_cast<QAbstractScrollArea*>(lsts[iScN_]))) continue;
				if(!tEl->verticalScrollBar() || !tEl->verticalScrollBar()->maximum())	continue;
				lsts[iScN_]->setMinimumHeight(fmax(lsts[iScN_]->height(),lsts[iScN_]->minimumHeight())+fitStp);
			    }

			    sclIt = tEl;
			    qApp->processEvents();
			    sclFitSz -= fitStp;
			}
			if((!sclIt && sclFromBeg) || (scrl->verticalScrollBar() && scrl->verticalScrollBar()->maximum())) break;
			if(iScN && iScN >= (texts.length()+tbls.length()+lsts.length())) iScN = 0;
		    }
		}
	    }
	    iArea++;
	}
	return;
    }
    //Read node tree and create widgets
    else for(unsigned iCf = 0; iCf < node.childSize(); iCf++) {
	XMLNode &t_s = *node.childGet(iCf);

	// Check access to node
	bool wr = s2i(t_s.attr("acs"))&SEC_WR;

	// View areas
	if(t_s.name() == "area") {
	    QWidget *wdg = widget;
	    if(widget) {
		wdg = new QGroupBox(t_s.attr("dscr").c_str(), widget);
		QVBoxLayout *w_lay = new QVBoxLayout(wdg);
		//((QGroupBox *)wdg)->setColumnLayout(0, Qt::Vertical);
		w_lay->setSpacing(3);
		w_lay->setContentsMargins(3, 3, 3, 3);
		//w_lay->setAlignment(Qt::AlignTop);
		widget->layout()->addWidget(wdg);
	    }
	    selectChildRecArea(t_s, a_path+t_s.attr("id")+'/', wdg);
	}
	// View list elements
	else if(t_s.name() == "list") {
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"), TSYS::PathEl);

	    QLabel *lab;
	    ListView *lstbox;

	    if(widget) {
		lab = new QLabel(widget);
		lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		lstbox = new ListView(widget);
		lstbox->setObjectName(br_path.c_str());
		lstbox->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(lstbox, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(listBoxPopup()));

		//  Go branche signal connect
		if(t_s.attr("tp") == "br")
		    connect(lstbox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listBoxGo(QListWidgetItem*)));

		lstbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum/*Expanding*/));

		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->setAlignment(Qt::AlignLeft);
		vbox->addWidget(lab);
		vbox->addWidget(lstbox);
		widget->layout()->addItem(vbox);

		//lstbox->setMinimumHeight(QFontMetrics(lstbox->font()).height()*4);

		t_s.setAttr("addr_lab", TSYS::addr2str(lab));
		t_s.setAttr("addr_el", TSYS::addr2str(lstbox));
	    }
	    else {
		lab	= (QLabel*)TSYS::str2addr(t_s.attr("addr_lab"));
		lstbox	= (ListView*)TSYS::str2addr(t_s.attr("addr_el"));
		lstbox->clear();
	    }

	    //  Fill the list
	    lab->setText((t_s.attr("dscr")+":").c_str());
	    mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, lstbox);
	    XMLNode req("get");
	    req.setAttr("path", br_path);
	    if((rez=cntrIfCmd(req)) == TError::NoError || rez == TError::Core_CntrWarning)
		for(unsigned iEl = 0; iEl < req.childSize(); iEl++)
		    if(req.childGet(iEl)->name() == "el")
			lstbox->addItem(req.childGet(iEl)->text().c_str());
	    if(rez > TError::NoError) mod->postMessCntr(req, this);
	}
	// View table elements
	else if(t_s.name() == "table") {
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"), TSYS::PathEl);

	    //QLabel *lab;
	    CfgTable *tbl;

	    if(widget) {
		QLabel *lab = new QLabel(widget);
		lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		lab->setText((t_s.attr("dscr")+":").c_str());
		widget->layout()->addWidget(lab);
		tbl = new CfgTable(widget);
		tbl->setItemDelegate(new TableDelegate);
		tbl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		tbl->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		//tbl->setTextElideMode(Qt::ElideNone);
		tbl->setObjectName(br_path.c_str());
		QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Minimum/*Expanding*/);
		sp.setVerticalStretch(1);
		tbl->setSizePolicy(sp);
		tbl->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(tbl, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(tablePopup(const QPoint&)));
		connect(tbl, SIGNAL(cellChanged(int,int)), this, SLOT(tableSet(int,int)));
		connect(tbl->horizontalHeader(), SIGNAL(sectionClicked(int)/*sectionResized(int,int,int)*/), tbl, SLOT(resizeRowsToContentsLim()));
		tbl->setMinimumHeight(150); //tbl->setMaximumHeight(500);
		widget->layout()->addWidget(tbl);

		//  Find items
		QImage ico_t;
		if(!ico_t.load(TUIS::icoGet("find",NULL,true).c_str())) ico_t.load(":/images/find.png");
		QAction *actFind = new QAction(QPixmap::fromImage(ico_t), _("Find"), tbl);
		actFind->setObjectName("tableFind");
		actFind->setShortcut(Qt::CTRL|Qt::Key_F);
		actFind->setShortcutContext(Qt::WidgetShortcut);
		connect(actFind, SIGNAL(triggered()), this, SLOT(tableFind()));
		tbl->addAction(actFind);
		QAction *actFindNext = new QAction(_("Find next"), tbl);
		actFindNext->setObjectName("tableFindNext");
		actFindNext->setShortcut(Qt::Key_F3);
		actFindNext->setShortcutContext(Qt::WidgetShortcut);
		connect(actFindNext, SIGNAL(triggered()), this, SLOT(tableFind()));
		tbl->addAction(actFindNext);
		actFindNext->setEnabled(false);

		//t_s.attr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_tbl",TSYS::addr2str(tbl));
	    }
	    else {
		//lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		tbl = (CfgTable *)TSYS::str2addr(t_s.attr("addr_tbl"));
	    }

	    //  Fill the table
	    tbl->setProperty("rows", s2i(t_s.attr("rows")));
	    mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, tbl);
	    XMLNode req("get"); req.setAttr("path",br_path);
	    if((rez=cntrIfCmd(req)) == TError::NoError || rez == TError::Core_CntrWarning) {
		//   Columns adjusting flag
		bool adjCol = widget || !tbl->rowCount();
		bool adjRow = false;

		//   Copy values to the info tree
		for(unsigned iCol = 0; iCol < req.childSize(); iCol++) {
		    XMLNode *t_lsel = req.childGet(iCol);
		    XMLNode *t_linf = t_s.childGet("id", t_lsel->attr("id"), true);
		    if(!t_linf) { t_linf = t_s.childIns(iCol); *t_linf = *t_lsel; adjCol = true; }
		    t_linf->childClear();
		    for(unsigned iRow = 0; iRow < t_lsel->childSize(); iRow++)
			*t_linf->childAdd() = *t_lsel->childGet(iRow);
		}

		//   Calc rows and columns
		int nCol = t_s.childSize();
		int nRow = (nCol) ? t_s.childGet(0)->childSize() : 0;

		if(tbl->columnCount() != nCol) tbl->setColumnCount(nCol);
		if(tbl->rowCount() != nRow)	{ tbl->setRowCount(nRow); adjRow = true; }

		for(unsigned iLst = 0; iLst < t_s.childSize(); iLst++) {
		    XMLNode *t_linf = t_s.childGet(iLst);
		    if(!t_linf) continue;
		    bool c_wr = wr && (s2i(t_linf->attr("acs"))&SEC_WR);

		    QTableWidgetItem *thd_it = tbl->horizontalHeaderItem(iLst);
		    if(!thd_it) {
			thd_it = new QTableWidgetItem("");
			tbl->setHorizontalHeaderItem(iLst, thd_it);
		    }

		    thd_it->setData(Qt::DisplayRole, t_linf->attr("dscr").c_str());
		    string colHelp = t_linf->attr("help");
		    if(colHelp.size()) {
			thd_it->setData(Qt::ToolTipRole, mod->setHelp(colHelp).c_str());
			thd_it->setData(Qt::WhatsThisRole, colHelp.c_str());
		    }

		    //   Set elements
		    tblInit = true;
		    QStringList elmsG, elmiG;
		    for(unsigned iEl = 0; iEl < t_linf->childSize(); iEl++) {
			//   The list processing
			bool isSel = false, isLocSel = false;
			QStringList elms, elmi;
			if((isLocSel=(t_linf->childGet(iEl)->attr("dest") == "select" || t_linf->childGet(iEl)->attr("dest") == "sel_ed")) ||
			    t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed")
			{
			    isSel = true;
			    XMLNode *selO = isLocSel ? t_linf->childGet(iEl) : t_linf;

			    if(isLocSel || elmsG.empty()) {
				if(selO->attr("select").empty()) {
				    elms = QString(selO->attr("sel_list").c_str()).split(";");
				    elmi = QString(selO->attr("sel_id").c_str()).split(";");
				}
				else {
				    XMLNode x_lst("get");
				    x_lst.setAttr("path",TSYS::strEncode(selO->attr("select"),TSYS::PathEl));
				    if((rez=cntrIfCmd(x_lst)) > TError::NoError) mod->postMessCntr(x_lst, this);
				    else if(rez == TError::NoError)
					for(unsigned iLs = 0; iLs < x_lst.childSize(); iLs++) {
					    if(!x_lst.childGet(iLs)->attr("id").empty())
						elmi += x_lst.childGet(iLs)->attr("id").c_str();
					    elms += x_lst.childGet(iLs)->text().c_str();
					}
				}
				if(elmsG.empty()) elmsG = elms, elmiG = elmi;
			    }
			    if(!isLocSel) elms = elmsG, elmi = elmiG;
			}

			//   The item data
			thd_it = tbl->item(iEl, iLst);
			if(!thd_it) {
			    thd_it = new QTableWidgetItem("");
			    tbl->setItem(iEl, iLst, thd_it);
			    if(t_linf->childGet(iEl)->attr("help").size()) {
				thd_it->setData(Qt::ToolTipRole, mod->setHelp(t_linf->childGet(iEl)->attr("help")).c_str());
				thd_it->setData(Qt::WhatsThisRole, t_linf->childGet(iEl)->attr("help").c_str());
			    }
			    else if(colHelp.size()) {
				thd_it->setData(Qt::ToolTipRole, mod->setHelp(colHelp).c_str());
				thd_it->setData(Qt::WhatsThisRole, colHelp.c_str());
			    }
			}

			//   Setting the element
			if(t_linf->attr("tp") == "bool")	thd_it->setData(Qt::DisplayRole, (bool)s2i(t_linf->childGet(iEl)->text()));
			else if(isSel) {
			    int sel_n;
			    for(sel_n = 0; sel_n < elms.size(); sel_n++)
				if((elms.size() == elmi.size() && elmi[sel_n] == t_linf->childGet(iEl)->text().c_str()) ||
					(elms.size() != elmi.size() && elms[sel_n] == t_linf->childGet(iEl)->text().c_str()))
				    break;
			    if(sel_n >= elms.size()) {
				elms.push_front(t_linf->childGet(iEl)->text().c_str());
				elmi.push_front(t_linf->childGet(iEl)->text().c_str());
				sel_n = 0;
			    }

			    thd_it->setData(Qt::DisplayRole, elms.at(sel_n));
			    thd_it->setData(TableDelegate::SelectRole, elms);
			    thd_it->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
			}
			else if(t_linf->attr("tp") == "dec")	thd_it->setData(Qt::DisplayRole, s2ll(t_linf->childGet(iEl)->text()));
			else if(t_linf->attr("tp") == "real")	thd_it->setData(Qt::DisplayRole, s2r(t_linf->childGet(iEl)->text()));
			else if(t_linf->attr("tp") == "time") {
			    thd_it->setData(Qt::DisplayRole, atm2s(s2i(t_linf->childGet(iEl)->text()),"%d-%m-%Y %H:%M:%S").c_str());
			    thd_it->setData(Qt::TextAlignmentRole, (int)(Qt::AlignCenter|Qt::TextWordWrap));
			}
			else thd_it->setData(Qt::DisplayRole, TSYS::strEncode(t_linf->childGet(iEl)->text(),TSYS::ShieldBin).c_str());

			//   Set access
			if(!c_wr) thd_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
			else thd_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable);
		    }
		}
		if((adjCol || adjRow) && tbl->columnCount()) {
		    tbl->resizeColumnsToContents();
		    for(int iTr = 0; iTr < 5; iTr++)	qApp->processEvents();	//Call all cascade events

		    int tblWdth = tbl->maximumViewportSize().width() -
				    ((tbl->verticalScrollBar()&&tbl->verticalScrollBar()->isVisible())?tbl->verticalScrollBar()->size().width():0);
		    int averWdth = tbl->columnCount() ? (tblWdth/tbl->columnCount()) : 0;
		    int fullColsWdth = 0, niceForceColsWdth = 0, busyCols = 0;
		    //   Count width params
		    for(int iC = 0; iC < tbl->columnCount(); iC++) {
			fullColsWdth += tbl->columnWidth(iC);
			if(tbl->columnWidth(iC) <= averWdth)	niceForceColsWdth += tbl->columnWidth(iC);
			else busyCols++;
		    }
		    for(int iIt = 0; busyCols && iIt < 10; iIt++) {
			int busyColsWdth = (tblWdth-niceForceColsWdth)/busyCols;
			int busyCols_ = 0, niceForceColsWdth_ = 0;
			for(int iC = 0; iC < tbl->columnCount(); iC++)
			    if(tbl->columnWidth(iC) < busyColsWdth) niceForceColsWdth_ += tbl->columnWidth(iC);
			    else busyCols_++;
			if(busyCols_ == busyCols) break;
			busyCols = busyCols_; niceForceColsWdth = niceForceColsWdth_;
		    }
		    //   Set busyCols
		    if(fullColsWdth > tblWdth && busyCols) {
			int busyColsWdth = (tblWdth-niceForceColsWdth)/busyCols;
			for(int iC = 0; iC < tbl->columnCount(); iC++)
			    if(tbl->columnWidth(iC) > averWdth && tbl->columnWidth(iC) > busyColsWdth)
				tbl->setColumnWidth(iC, busyColsWdth);
		    }

		    //tbl->resizeRowsToContentsLim();
		}

		if(tbl->columnCount() && tbl->rowCount()) tbl->resizeRowsToContentsLim();

		tblInit = false;
	    }
	    if(rez > TError::NoError) mod->postMessCntr(req, this);
	}
	// View images
	else if(t_s.name() == "img") {
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

	    QLabel *lab;
	    ImgView *img;

	    if(widget) {
		lab = new QLabel(widget);
		lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		img = new ImgView(widget, Qt::Widget, s2i(t_s.attr("h_sz")), s2i(t_s.attr("v_sz")));
		img->setObjectName(br_path.c_str());
		img->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
		img->setMinimumSize(200,200);
		img->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(img, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(imgPopup(const QPoint&)));

		int vsz = s2i(t_s.attr("v_sz"));
		QBoxLayout *box = new QBoxLayout((vsz&&vsz<70)?QBoxLayout::LeftToRight:QBoxLayout::TopToBottom);
		box->addWidget(lab);
		box->addWidget(img);
		if(box->direction() == QBoxLayout::LeftToRight) {
		    lab->setAlignment(Qt::AlignTop);
		    box->setSpacing(6);
		    box->addItem(new QSpacerItem(0,20,QSizePolicy::Expanding,QSizePolicy::Minimum));
		}
		widget->layout()->addItem(box);

		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_el",TSYS::addr2str(img));
	    }
	    else {
		lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		img = (ImgView *)TSYS::str2addr(t_s.attr("addr_el"));
	    }

	    //  Set image
	    lab->setText((t_s.attr("dscr")+":").c_str());
	    mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, img);

	    XMLNode req("get");
	    req.setAttr("path", br_path);
	    if((rez=cntrIfCmd(req)) == TError::NoError || rez == TError::Core_CntrWarning)
		img->setImage(TSYS::strDecode(req.text(),TSYS::base64));
	    if(rez > TError::NoError) mod->postMessCntr(req, this);
	}
	// View standard fields
	else if(t_s.name() == "fld")	basicFields(t_s, a_path, widget, wr, &l_hbox, l_pos);
	// View commands
	else if(t_s.name() == "comm") {
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"), TSYS::PathEl);

	    QPushButton *button;
	    QGroupBox *comm_pan = NULL;
	    QHBoxLayout *c_hbox = NULL;
	    int c_pos = 0;

	    if(widget) {
		if(!t_s.childSize()) {
		    button = new QPushButton(widget);
		    button->setObjectName(br_path.c_str());
		    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
		    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
		    widget->layout()->addWidget(button);
		}
		else {
		    comm_pan = new QGroupBox(t_s.attr("dscr").c_str(), widget);
		    QVBoxLayout *comm_lay = new QVBoxLayout(comm_pan);
		    //comm_pan->setColumnLayout(0, Qt::Vertical);
		    //comm_lay->setMargin(6);
		    comm_lay->setSpacing(3);
		    comm_lay->setAlignment(Qt::AlignTop);

		    button = new QPushButton(comm_pan);
		    button->setObjectName(br_path.c_str());
		    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
		    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

		    comm_lay->addWidget(button);
		    widget->layout()->addWidget(comm_pan);
		}

		t_s.setAttr("addr_butt", TSYS::addr2str(button));
	    }
	    else button = (QPushButton *)TSYS::str2addr(t_s.attr("addr_butt"));

	    //  Update or create parameters
	    for(unsigned iCf = 0; iCf < t_s.childSize(); iCf++) {
		XMLNode &t_scm = *t_s.childGet(iCf);
		if(t_scm.name() == "fld") basicFields(t_scm, a_path+t_s.attr("id")+'/', comm_pan, true, &c_hbox, c_pos, true);
	    }

	    //  Fill command
	    button->setText(t_s.attr("dscr").c_str());
	    mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, button);
	}
    }
}

void ConfApp::basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool comm )
{
    int rezReq = 0;
    string br_path = TSYS::strEncode(string((comm)?"b":"")+a_path+t_s.attr("id"), TSYS::PathEl);

    XMLNode data_req("get");
    if(!comm) {
	data_req.setAttr("path", br_path);
	if((rezReq=cntrIfCmd(data_req)) > TError::NoError) {
	    mod->postMessCntr(data_req, this);
	    if(data_req.attr("mtxt").empty()) data_req.setText("");
	}
    }

    //View select fields
    if(t_s.attr("dest") == "select") {
	QLabel *lab = NULL, *val_r = NULL;
	QComboBox *val_w = NULL;

	if(widget) {
	    if(!wr) {
		val_r = new QLabel(widget);
		val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		//QSizePolicy sp(QSizePolicy::Ignored/*Expanding*/, QSizePolicy::Preferred);
		//sp.setHorizontalStretch(1);
		//val_r->setSizePolicy(sp);
	    }
	    else {
		val_w = new QComboBox(widget);
		val_w->setMinimumSize(100, 0);
		val_w->setObjectName(br_path.c_str());
		val_w->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		val_w->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));
		//val_w->setFocusPolicy(Qt::StrongFocus);
		connect(val_w, SIGNAL(activated(int)), this, SLOT(combBoxActivate(int)));

		val_w->installEventFilter(this);
	    }

	    if(t_s.attr("dscr").size()) {
		*l_hbox = new QHBoxLayout; l_pos = 0;
		(*l_hbox)->setSpacing(6);
		lab = new QLabel(widget);
		lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		(*l_hbox)->insertWidget(l_pos++, lab);
		if(val_w) (*l_hbox)->insertWidget(l_pos++, val_w);
		if(val_r) (*l_hbox)->insertWidget(l_pos++, val_r);
		(*l_hbox)->addItem(new QSpacerItem(0,10,QSizePolicy::Expanding,QSizePolicy::Minimum));
		widget->layout()->addItem(*l_hbox);
	    }
	    else {
		if(val_w) { if(*l_hbox) (*l_hbox)->insertWidget(l_pos++, val_w); else { val_w->deleteLater(); val_w = NULL; } }
		if(val_r) { if(*l_hbox) (*l_hbox)->insertWidget(l_pos++, val_r); else { val_r->deleteLater(); val_r = NULL; } }
	    }

	    t_s.setAttr("addr_lab", TSYS::addr2str(lab));
	    t_s.setAttr("addr_val_w", TSYS::addr2str(val_w));
	    t_s.setAttr("addr_val_r", TSYS::addr2str(val_r));
	}
	else {
	    lab  = (QLabel*)TSYS::str2addr(t_s.attr("addr_lab"));
	    val_r = (QLabel*)TSYS::str2addr(t_s.attr("addr_val_r"));
	    val_w = (QComboBox*)TSYS::str2addr(t_s.attr("addr_val_w"));
	}

	// Filling the combo
	if(lab) lab->setText((t_s.attr("dscr")+":").c_str());
	if(val_w || val_r) {
	    mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_w?(QWidget*)val_w:(QWidget*)val_r);
	    if(val_w) val_w->clear();

	    bool sel_ok = false;
	    unsigned c_el = 0;
	    if(t_s.attr("select").empty()) {
		string s_nm;
		bool ind_ok = t_s.attr("sel_id").size();	//Index present
		for(int ls_off = 0, id_off = 0; !(s_nm=TSYS::strSepParse(t_s.attr("sel_list"),0,';',&ls_off)).empty(); c_el++) {
		    if(val_w)	val_w->insertItem(c_el, s_nm.c_str());
		    if((ind_ok && TSYS::strSepParse(t_s.attr("sel_id"),0,';',&id_off) == data_req.text()) ||
			(!ind_ok && s_nm == data_req.text()))
		    {
			sel_ok = true;
			if(val_w)	val_w->setCurrentIndex(c_el);
			if(val_r)	val_r->setText((string("<b>")+TSYS::strEncode(s_nm,TSYS::Html)+"</b>").c_str());
		    }
		}
	    }
	    else {
		XMLNode x_lst("get");
		x_lst.setAttr("path",TSYS::strEncode(t_s.attr("select"),TSYS::PathEl));
		if(!cntrIfCmd(x_lst))
		    for(unsigned iEl = 0; iEl < x_lst.childSize(); iEl++) {
			if(x_lst.childGet(iEl)->name() != "el") continue;
			if(val_w)	val_w->insertItem(c_el++, x_lst.childGet(iEl)->text().c_str());
			bool ind_ok = x_lst.childGet(iEl)->attr("id").size();	//Index present
			if((ind_ok && x_lst.childGet(iEl)->attr("id") == data_req.text()) ||
			    (!ind_ok && x_lst.childGet(iEl)->text() == data_req.text()))
			{
			    sel_ok = true;
			    if(val_w)	val_w->setCurrentIndex(c_el-1);
			    if(val_r)	val_r->setText((string("<b>")+TSYS::strEncode(x_lst.childGet(iEl)->text(),TSYS::Html)+"</b>").c_str());
			}
		    }
	    }
	    //  Insert empty field if none selected
	    if(!sel_ok) {
		if(val_w) {
		    val_w->insertItem(c_el, data_req.text().c_str());
		    val_w->setCurrentIndex(c_el);
		}
		if(val_r) val_r->setText((string("<b>")+TSYS::strEncode(data_req.text(),TSYS::Html)+"</b>").c_str());
	    }
	}
    }
    else {
	//View Boolean fields
	if(t_s.attr("tp") == "bool") {
	    QLabel *lab = NULL, *val_r = NULL;
	    QCheckBox *val_w	= NULL;

	    if(widget) {
		// View info
		if(!wr) {
		    val_r = new QLabel(widget);
		    val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    //QSizePolicy sp(QSizePolicy::Ignored/*Expanding*/, QSizePolicy::Preferred);
		    //sp.setHorizontalStretch(1);
		    //val_r->setSizePolicy(sp);
		}
		// View edit
		else {
		    val_w = new QCheckBox(widget);
		    val_w->setObjectName(br_path.c_str());
		    connect(val_w, SIGNAL(stateChanged(int)), this, SLOT(checkBoxStChange(int)));
		    //if(!wr)	val_w->setDisabled(true);
		}
		// Check use label
		if(t_s.attr("dscr").size()) {
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    lab = new QLabel(widget);
		    lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    (*l_hbox)->insertWidget(l_pos++, lab);
		    if(val_w)	(*l_hbox)->insertWidget(l_pos++, val_w);
		    if(val_r)	(*l_hbox)->insertWidget(l_pos++, val_r);
		    (*l_hbox)->addItem(new QSpacerItem(0,10,QSizePolicy::Expanding,QSizePolicy::Minimum));
		    widget->layout()->addItem(*l_hbox);
		}
		else {
		    if(val_w) { if(*l_hbox) (*l_hbox)->insertWidget(l_pos++, val_w); else { val_w->deleteLater(); val_w = NULL; } }
		    if(val_r) { if(*l_hbox) (*l_hbox)->insertWidget(l_pos++, val_r); else { val_r->deleteLater(); val_r = NULL; } }
		}

		t_s.setAttr("addr_lab", TSYS::addr2str(lab));
		t_s.setAttr("addr_val_w", TSYS::addr2str(val_w));
		t_s.setAttr("addr_val_r", TSYS::addr2str(val_r));
	    }
	    else {
		lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		val_w = (QCheckBox *)TSYS::str2addr(t_s.attr("addr_val_w"));
		val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_val_r"));
	    }

	    // Fill CheckBox
	    if(lab)	lab->setText((t_s.attr("dscr")+":").c_str());
	    if(val_w && rezReq >= 0) {
		mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_w);
		val_w->blockSignals(true);
		if(data_req.text() == "<EVAL>") val_w->setCheckState(Qt::PartiallyChecked);
		else if(s2i(data_req.text())) val_w->setCheckState(Qt::Checked);
		else val_w->setCheckState(Qt::Unchecked);
		val_w->blockSignals(false);
	    }
	    if(val_r && rezReq >= 0) {
		mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_r);
		val_r->setText((string("<b>")+TSYS::strEncode((data_req.text() == "<EVAL>")?_("<EVAL>"):
		    (s2i(data_req.text())?_("Yes"):_("No")),TSYS::Html)+"</b>").c_str());
	    }
	}
	//View edit fields
	else if(t_s.attr("tp") == "str" && (t_s.attr("rows").size() || t_s.attr("cols").size())) {
	    QLabel *lab;
	    TextEdit *edit;

	    if(widget) {
		lab = new QLabel(t_s.attr("dscr").c_str(),widget);
		lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		widget->layout()->addWidget(lab);

		edit = new TextEdit(widget, br_path.c_str());
		edit->setRowsCols(s2i(t_s.attr("cols")), s2i(t_s.attr("rows")));
		//edit->setMinimumHeight(QFontMetrics(edit->edit()->currentFont()).height()*(edit->rowsCols().height()+2));
		    //2*edit->edit()->currentFont().pointSize()*(edit->rowsCols().height()+1));
		widget->layout()->addWidget(edit);

		if(!wr)	edit->edit()->setReadOnly(true);
		else {
		    connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(editChange(const QString&)));
		    connect(edit, SIGNAL(apply()), this, SLOT(applyButton()));
		    connect(edit, SIGNAL(cancel()), this, SLOT(cancelButton()));
		}

		t_s.setAttr("addr_lab", TSYS::addr2str(lab));
		t_s.setAttr("addr_edit", TSYS::addr2str(edit));
	    }
	    else {
		lab  = (QLabel*)TSYS::str2addr(t_s.attr("addr_lab"));
		edit = (TextEdit*)TSYS::str2addr(t_s.attr("addr_edit"));
	    }

	    // Fill Edit
	    if(lab)	lab->setText((t_s.attr("dscr")+":").c_str());
	    string tVl;
	    if(edit && !edit->isChanged() && rezReq >= 0 && ((tVl=TSYS::strEncode(data_req.text(),TSYS::ShieldBin)) != edit->text().toStdString() || !edit->text().size())) {
		int cursorPos = edit->edit()->textCursor().position();
		int scrollVPos = edit->edit()->verticalScrollBar()->value();
		int scrollHPos = edit->edit()->horizontalScrollBar()->value();
		//Requesting the syntax higlihgt
		if(s2i(t_s.attr("SnthHgl"))) {
		    XMLNode hgl_req("SnthHgl");
		    hgl_req.setAttr("path", br_path);
		    if(!cntrIfCmd(hgl_req)) edit->setSnthHgl(hgl_req);
		}

		mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, edit);

		edit->setText(tVl.c_str());

		//Cursor position restore
		QTextCursor tCur = edit->edit()->textCursor(); tCur.setPosition(cursorPos);
		edit->edit()->setTextCursor(tCur); edit->edit()->ensureCursorVisible();
		edit->edit()->verticalScrollBar()->setValue(scrollVPos);
		edit->edit()->horizontalScrollBar()->setValue(scrollHPos);
	    }
	}
	//View Data-Time fields
	else if(t_s.attr("tp") == "time") {
	    QLabel	*lab = NULL, *val_r = NULL;
	    LineEdit	*val_w = NULL;

	    if(widget) {
		if(!wr) {
		    val_r = new QLabel(widget);
		    val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		    sp.setHorizontalStretch(1);
		    val_r->setSizePolicy(sp);
		    val_r->setAlignment(Qt::AlignVCenter);
		    val_r->setWordWrap(true);
		}
		else {
		    val_w = new LineEdit(widget, LineEdit::DateTime, comm);
		    val_w->setObjectName(br_path.c_str());
		    val_w->setCfg("dd-MM-yyyy hh:mm:ss");
		    connect(val_w, SIGNAL(valChanged(const QString&)), this, SLOT(editChange(const QString&)));
		    connect(val_w, SIGNAL(apply()), this, SLOT(applyButton()));
		    connect(val_w, SIGNAL(cancel()), this, SLOT(cancelButton()));
		}

		// Checking the label using
		if(t_s.attr("dscr").size()) {
		    lab = new QLabel(widget);
		    lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    (*l_hbox)->insertWidget(l_pos++, lab);
		    if(val_w)	(*l_hbox)->insertWidget(l_pos++, val_w);
		    if(val_r) {
			(*l_hbox)->insertWidget(l_pos++, val_r);
			//lab->setAlignment(Qt::AlignTop);
		    }
		    (*l_hbox)->addItem(new QSpacerItem(0,10,QSizePolicy::Expanding,QSizePolicy::Minimum));
		    widget->layout()->addItem(*l_hbox);
		}
		else {
		    if(*l_hbox) (*l_hbox)->insertWidget(l_pos++, (val_w?(QWidget*)val_w:(QWidget*)val_r));
		    else { (val_w?(QWidget*)val_w:(QWidget*)val_r)->deleteLater(); val_w = NULL; val_r = NULL; }
		}
		t_s.setAttr("addr_lab", TSYS::addr2str(lab));
		t_s.setAttr("addr_dtw", TSYS::addr2str(val_w));
		t_s.setAttr("addr_dtr", TSYS::addr2str(val_r));
	    }
	    else {
		lab  = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_dtr"));
		val_w = (LineEdit *)TSYS::str2addr(t_s.attr("addr_dtw"));
	    }

	    // Fill data
	    if(lab)	lab->setText((t_s.attr("dscr")+":").c_str());
	    if(val_r && rezReq >= 0) {
		time_t tm_t;
		if(data_req.text().size()) tm_t = s2i(data_req.text());
		else tm_t = time(NULL);
		QDateTime dtm;
		dtm.setSecsSinceEpoch(tm_t);
		mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_r);
		val_r->setText( "<b>"+dtm.toString("dd.MM.yyyy hh:mm:ss")+"</b>" );
	    }
	    if(val_w && rezReq >= 0 && !val_w->isEdited()) {
		mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_w);
		val_w->setValue(data_req.text().c_str());
	    }
	}
	//View other string and numberic fields
	else {
	    QLabel *lab = NULL, *val_r = NULL;
	    LineEdit *val_w	= NULL;
	    if(widget) {
		// View info
		if(!wr) {
		    val_r = new QLabel(widget);
		    val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    val_r->setWordWrap(true);
		    //val_r->setAlignment(Qt::AlignVCenter);
		    //val_r->setFrameStyle(QFrame::Panel | QFrame::Sunken);

		    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
		    sp.setControlType(QSizePolicy::Label);
#if QT_VERSION >= 0x040800
		    sp.setWidthForHeight(true);
#endif
		    sp.setHorizontalStretch(1);	//!!!! At setting there 0 we enable work at size hint which is very wrapping big texts,
						//     then we need to reimplement the size hint calculation in that case
		    val_r->setSizePolicy(sp);
		}
		// View edit
		else {
		    val_w = new LineEdit(widget, LineEdit::Text, comm);
		    val_w->setObjectName(br_path.c_str());
		    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
		    sp.setHorizontalStretch(10);
		    val_w->setSizePolicy(sp);
		    connect(val_w, SIGNAL(valChanged(const QString&)), this, SLOT(editChange(const QString&)));
		    connect(val_w, SIGNAL(apply()), this, SLOT(applyButton()));
		    connect(val_w, SIGNAL(cancel()), this, SLOT(cancelButton()));

		    //  addon parameters
		    string tp = t_s.attr("tp");
		    if(t_s.attr("dest") == "sel_ed") {
			val_w->setMinimumSize(100, 0);
			val_w->setType(LineEdit::Combo);
			//val_w->workWdg()->setFocusPolicy(Qt::StrongFocus);
			val_w->workWdg()->installEventFilter(this);
		    }
		    else if(tp == "dec" || tp == "hex" || tp == "oct") {
			val_w->setFixedWidth(QFontMetrics(val_w->workWdg()->font()).size(Qt::TextSingleLine,"0000000000").width()+30);
			//val_w->setType(LineEdit::Text);
			//QIntValidator *iv = new QIntValidator(val_w->workWdg());
			//((QLineEdit*)val_w->workWdg())->setValidator(iv);
		    }
		    /*else if(tp == "dec") {
			val_w->setFixedWidth(5*15+30);
			val_w->setType(LineEdit::Integer);
			QString	max = t_s.attr("max").empty() ? "2147483647" : t_s.attr("max").c_str();
			QString	min = t_s.attr("min").empty() ? "-2147483647" : t_s.attr("min").c_str();
			val_w->setCfg(min+":"+max+":1");
		    }
		    else if(tp == "hex" || tp == "oct")	val_w->setFixedWidth(5*15+30);*/
		    else if(tp == "real") {
			val_w->setFixedWidth(QFontMetrics(val_w->workWdg()->font()).size(Qt::TextSingleLine,"3.14159265e123").width()+30);
			//val_w->setType(LineEdit::Text);
/*#if QT_VERSION < 0x050000
			QDoubleValidator *dv = new QDoubleValidator(val_w->workWdg());
			dv->setNotation(QDoubleValidator::ScientificNotation);
			((QLineEdit*)val_w->workWdg())->setValidator(dv);
#endif
			QString	max = t_s.attr("max").empty() ? "9999999999" : t_s.attr("max").c_str();
			QString	min = t_s.attr("min").empty() ? "-9999999999" : t_s.attr("min").c_str();
			val_w->setCfg(min+":"+max+":1:::4");*/
		    }
		    else {
			val_w->setType(LineEdit::Text);
			val_w->setMinimumWidth(7*15+30);
			int flen = s2i(t_s.attr("len"));
			if(flen) ((QLineEdit*)val_w->workWdg())->setMaxLength(flen);
		    }
		}
		// Check use label
		if(t_s.attr("dscr").size()) {
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    lab = new QLabel(widget);
		    lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    (*l_hbox)->insertWidget(l_pos++, lab);
		    if(val_w)	(*l_hbox)->insertWidget(l_pos++, val_w);
		    if(val_r) {
			(*l_hbox)->insertWidget(l_pos++, val_r);
			//lab->setAlignment(Qt::AlignTop);
		    }

		    (*l_hbox)->addItem(new QSpacerItem(0,10,QSizePolicy::Expanding,QSizePolicy::Minimum));
		    widget->layout()->addItem(*l_hbox);
		}
		else {
		    if(*l_hbox) {
			if(val_w) (*l_hbox)->insertWidget(l_pos++, val_w);
			if(val_r) (*l_hbox)->insertWidget(l_pos++, val_r);
		    }
		    else {
			if(val_w) { val_w->deleteLater(); val_w = NULL; }
			if(val_r) { val_r->deleteLater(); val_r = NULL; }
		    }
		}

		t_s.setAttr("addr_lab", TSYS::addr2str(lab));
		t_s.setAttr("addr_val_w", TSYS::addr2str(val_w));
		t_s.setAttr("addr_val_r", TSYS::addr2str(val_r));
	    }
	    else {
		lab  = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_val_r"));
		val_w = (LineEdit *)TSYS::str2addr(t_s.attr("addr_val_w"));
	    }
	    // Fill line
	    string sval = TSYS::strEncode(data_req.text(), TSYS::ShieldBin);
	    if(t_s.attr("tpCh") == "hex" || (t_s.attr("tpCh").empty() && t_s.attr("tp") == "hex"))
		sval = "0x" + QString::number(s2ll(data_req.text()),16).toUpper().toStdString();
	    else if(t_s.attr("tpCh") == "oct" || (t_s.attr("tpCh").empty() && t_s.attr("tp") == "oct"))
		sval = "0" + QString::number(s2ll(data_req.text()),8).toStdString();

	    if(lab)	lab->setText((t_s.attr("dscr")+":").c_str());
	    if(val_r) {
		mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_r);
		val_r->setText((string("<b>")+TSYS::strEncode(sval,TSYS::Html)+"</b>").c_str());

		if(QString(sval.c_str()).toStdWString().size() < limObjID_SZ)
		    val_r->setFixedWidth(1.2*QFontMetrics(val_r->font()).size(Qt::TextSingleLine,sval.c_str()).width());
		else { val_r->setMinimumWidth(10); val_r->setMaximumWidth(100000); }

		//val_r->adjustSize();
	    }
	    if(val_w && !val_w->isEdited()) {
		if(rezReq >= 0) {
		    mod->setHelp(t_s.attr("help"), selPath+"/"+br_path, val_w);
		    val_w->setValue(sval.c_str());
		}

		// Fill combo
		if(t_s.attr("dest") == "sel_ed") {
		    string cfg_vls;
		    if(t_s.attr("select").empty()) {
			string s_nm;
			for(int ls_off = 0; !(s_nm=TSYS::strSepParse(t_s.attr("sel_list"),0,';',&ls_off)).empty(); )
			    cfg_vls += s_nm+"\n";
		    }
		    else {
			XMLNode x_lst("get");
			x_lst.setAttr("path", TSYS::strEncode(t_s.attr("select"),TSYS::PathEl));
			if(!cntrIfCmd(x_lst))
			    for(unsigned iEl = 0; iEl < x_lst.childSize(); iEl++)
				if(x_lst.childGet(iEl)->name() == "el")
				    cfg_vls += x_lst.childGet(iEl)->text()+"\n";
		    }
		    val_w->setCfg(cfg_vls.c_str());
		}
	    }
	}
    }
}

void ConfApp::viewChild( QTreeWidgetItem * i )
{
    try {
	//Delete ViewItem childs
	while(i->childCount()) delete i->takeChild(0);
	viewChildRecArea(i);
	CtrTree->resizeColumnToContents(0);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::pageDisplay( const string &ipath )
{
    if(pgDisplay) return;
    pgDisplay = true;

    string tVl;
    int off = 0;
    string  path = TSYS::strParse(ipath, 0, "#", &off),
	    tab = TSYS::strParse(ipath, 0, "#", &off);

    try {
    //Checking for Up
    actUp->setEnabled(path.rfind("/") != string::npos && path.rfind("/") != 0);

    //Checking for Prev and Next
    actPrev->setEnabled(prev.size());
    while(prev.size() > NAV_BACK_DEPTH) prev.pop_back();
    actPrev->setMenu(new QMenu(this));
    for(vector<string>::iterator iv = prev.begin(); iv != prev.end(); ++iv) {
	QAction *actPrevLnk = new QAction(iv->c_str(), this);
	actPrevLnk->setObjectName(iv->c_str());
	actPrev->menu()->addAction(actPrevLnk);
	connect(actPrevLnk, SIGNAL(triggered()), this, SLOT(pagePrev()));

	tVl += (tVl.size()?"\n":"") + *iv;
    }
    actPrev->setToolTip((string(_("Go back"))+":\n"+tVl).c_str());

    actNext->setEnabled(next.size());
    while(next.size() > NAV_BACK_DEPTH) next.pop_back();
    tVl = "";
    actNext->setMenu(new QMenu(this));
    for(vector<string>::iterator iv = next.begin(); iv != next.end(); ++iv) {
	QAction *actNextLnk = new QAction(iv->c_str(), this);
	actNextLnk->setObjectName(iv->c_str());
	actNext->menu()->addAction(actNextLnk);
	connect(actNextLnk, SIGNAL(triggered()), this, SLOT(pageNext()));

	tVl += (tVl.size()?"\n":"") + *iv;
    }
    actNext->setToolTip((string(_("Go forward"))+":\n"+tVl).c_str());

    //Updating the current page
    if(path != pgInfo.attr("path")) {
	if(path == selPath) selPath = pgInfo.attr("path");	//!!!! To ensure of proper working of the checking for not applied editable widgets

	// Trace the control tree
	QTreeWidgetItem *tIt = NULL;
	if((!CtrTree->currentItem() || CtrTree->currentItem()->text(2).toStdString() != path) && (tIt=getExpandTreeWIt(path))) {
	    CtrTree->blockSignals(true);
	    CtrTree->setCurrentItem(tIt);
	    CtrTree->blockSignals(false);
	    CtrTree->scrollToItem(tIt, QAbstractItemView::EnsureVisible);
	}

	// Stop the refreshing
	pageCyclRefrStop();

	// Check for not applied editable widgets
	//vector<QWidget*> prcW;
	int editIts = 0;
	QList<LineEdit*> lines = tabs->findChildren<LineEdit*>();
	QList<TextEdit*> texts = tabs->findChildren<TextEdit*>();
	for(int iIt = 0; iIt < lines.size(); ++iIt)
	    if(lines[iIt]->isEdited())	editIts++;
	for(int iIt = 0; iIt < texts.size(); ++iIt)
	    if(texts[iIt]->isChanged())	editIts++;
	if(editIts) {
	    bool isOK = QMessageBox::information(this,_("Applying the changes"),_("Some changes were made!\nAccept the changes now or lose?"),
		QMessageBox::Apply|QMessageBox::Cancel,QMessageBox::Apply) == QMessageBox::Apply;
	    for(int iIt = 0; iIt < lines.size(); ++iIt)
		if(lines[iIt]->isEdited()) isOK ? lines[iIt]->btApply() : lines[iIt]->btCancel();
	    for(int iIt = 0; iIt < texts.size(); ++iIt)
		if(texts[iIt]->isChanged()) isOK ? texts[iIt]->btApply() : texts[iIt]->btCancel();
	}

	// Request new page tree
	XMLNode n_node("info");
	n_node.setAttr("path", path);
	if(cntrIfCmd(n_node))		throw TError(s2i(n_node.attr("rez")), n_node.attr("mcat"), n_node.text());
	if(!n_node.childGet(0,true))	throw TError(mod->nodePath(), _("Broken information request - there are no descendants."));

	selPath = path;
	pgInfo = n_node;
	root = pgInfo.childGet(0);

	actManualPage->setEnabled(root->attr("doc").size());
	actManualPage->setProperty("doc", root->attr("doc").c_str());
    }
    else {
	// Check the new node structure and the old node
	XMLNode n_node("info");
	n_node.setAttr("path", selPath);
	if(cntrIfCmd(n_node))	throw TError(s2i(n_node.attr("rez")), n_node.attr("mcat"), n_node.text());
	upStruct(*root, *n_node.childGet(0));
    }

    //The add and the delete access allow check
    actItAdd->setEnabled(false);
    if(root->childGet("id","br",true)) {
	XMLNode *branch = root->childGet("id","br");
	for(unsigned iB = 0; iB < branch->childSize(); iB++)
	    if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR)
	    { actItAdd->setEnabled(true); break; }
    }
    actItDel->setEnabled(root&&s2i(root->attr("acs"))&SEC_WR);

    //Start complex request forming
    genReqs.setAttr("path",selPath)->setAttr("fillMode","1");

loadGenReqDate:
    //Page content forming
    //CtrTree->blockSignals(true);
    tabs->blockSignals(true);
    selectChildRecArea(*root, "/");
    tabs->blockSignals(false);
    //CtrTree->blockSignals(false);

    //Load and Save allow check
    actDBLoad->setEnabled(false); actDBSave->setEnabled(false);
    XMLNode req("modify");
    req.setAttr("path", "/%2fobj");
    if(cntrIfCmd(req) > TError::NoError) mod->postMessCntr(req, this);
    else if(s2i(req.text()))	{ actDBLoad->setEnabled(true); actDBSave->setEnabled(true); }

    //Stop complex request forming
    if(genReqs.attr("fillMode") == "1") {
	genReqs.attrDel("fillMode");
	if(cntrIfCmd(genReqs)) {
	    mod->postMessCntr(genReqs, this);
	    genReqs.clear();
	    pgDisplay = false;
	    if(winClose) close();
	    return;
	}
	goto loadGenReqDate;
    }
    else genReqs.clear();

    //Local station modifying check
    mStModify->setText(" ");
    req.setAttr("path","/"+SYS->id()+"/%2fobj");
    if(!cntrIfCmd(req) && s2i(req.text()))	mStModify->setText("*");

    //Edit tools updating
    editToolUpdate();

    //Checking for Favorite Pages
    favUpd(Fav_Sel);

    pgDisplay = false;
    if(winClose) close();
    }
    catch(TError) {
	pgDisplay = false;
	if(winClose) close();
	throw;
    }

    //Changing the tab
    XMLNode *tabN = NULL;
    for(int tabId = 0; tab.size() && (tabN=root->childGet("area",tabId,true)); ++tabId) {
	if(tabN->attr("id") != tab)	continue;
	if(tabs->currentIndex() != tabId) tabs->setCurrentIndex(tabId);
	break;
    }
}

bool ConfApp::upStruct( XMLNode &w_nd, const XMLNode &n_nd )
{
    bool str_ch = false;

    //Check access
    if(w_nd.attr("acs") != n_nd.attr("acs")) str_ch = true;

    //Scan deleted nodes
    for(unsigned iW = 0, iN; iW < w_nd.childSize(); ) {
	for(iN = 0; iN < n_nd.childSize(); iN++)
	    if(w_nd.childGet(iW)->name() == n_nd.childGet(iN)->name() &&
		    w_nd.childGet(iW)->attr("id") == n_nd.childGet(iN)->attr("id"))
		break;
	if(iN >= n_nd.childSize()) {
	    w_nd.childDel(iW);
	    if(w_nd.name() != "table" && w_nd.name() != "list" && w_nd.name() != "oscada_cntr") str_ch = true;
	    continue;
	}
	iW++;
    }

    //Scan for the new nodes and check present nodes
    for(unsigned iN = 0, iW; iN < n_nd.childSize(); iN++) {
	for(iW = 0; iW < w_nd.childSize(); iW++)
	    if(w_nd.childGet(iW)->name() == n_nd.childGet(iN)->name() &&
		    w_nd.childGet(iW)->attr("id") == n_nd.childGet(iN)->attr("id"))
		break;
	if(iW >= w_nd.childSize()) {
	    // Add node
	    *w_nd.childIns(iN) = *n_nd.childGet(iN);
	    str_ch = true;
	    iW = iN;
	}
	else {
	    // Check present node
	    if(upStruct(*w_nd.childGet(iW),*n_nd.childGet(iN))) str_ch = true;
	    if(str_ch && w_nd.name() == "oscada_cntr") {
		w_nd.childGet(iW)->setAttr("qview","0");
		str_ch = false;
		continue;
	    }
	}

	//Check of the description present
	if((bool)w_nd.childGet(iW)->attr("dscr").size() ^ (bool)n_nd.childGet(iN)->attr("dscr").size())
	    str_ch = true;

	//Check base fields destination change
	if(w_nd.childGet(iW)->name() == "fld" &&
	    (w_nd.childGet(iW)->attr("dest") != n_nd.childGet(iN)->attr("dest") ||
	     w_nd.childGet(iW)->attr("tp") != n_nd.childGet(iN)->attr("tp")))
	{
	    w_nd.childGet(iW)->setAttr("dest", "");
	    w_nd.childGet(iW)->setAttr("tp", "");
	    str_ch = true;
	}

	//Sync node parameters (text and attributes)
	w_nd.childGet(iW)->setText(n_nd.childGet(iN)->text());

	w_nd.childGet(iW)->setAttr("dscr", "");
	vector<string> ls;
	n_nd.childGet(iN)->attrList(ls);
	for(unsigned iA = 0; iA < ls.size(); iA++)
	    w_nd.childGet(iW)->setAttr(ls[iA],n_nd.childGet(iN)->attr(ls[iA]));
    }

    return str_ch;
}

void ConfApp::onItem( QTreeWidgetItem * i )	{ statusBar()->showMessage(i->text(2), 10000); }

void ConfApp::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    try {
	if(lview && lview->currentItem()) {
	    if(lview->currentItem()->text(2)[0] == '*') {
		popup.addAction(actItAdd);
		popup.addSeparator();
		lview->currentItem()->parent()->setSelected(true);
		actItAdd->setProperty("grpHint", lview->currentItem()->text(2).toStdString().substr(1).c_str());
	    }
	    else {
		//Load and Save actions
		popup.addAction(actDBLoad);
		popup.addAction(actDBSave);
		popup.addSeparator();
		//Add and delete item action add
		popup.addAction(actItAdd);
		popup.addAction(actItDel);
		popup.addSeparator();
		popup.addAction(actItCut);
		popup.addAction(actItCopy);
		popup.addAction(actItPaste);
		popup.addSeparator();
		//Favorite
		popup.addAction(actFav);
		popup.addAction(actFavToggle);
		popup.addSeparator();
	    }
	}
	//Main action add
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("reload",NULL,true).c_str())) ico_t.load(":/images/reload.png");
	QAction *actRemHostUp = new QAction(QPixmap::fromImage(ico_t), _("Refresh the items tree"), this);
	popup.addAction(actRemHostUp);
	QAction *rez = popup.exec(QCursor::pos());
	if(rez == actRemHostUp) {
	    initHosts();
	    treeUpdate();
	}
	popup.clear();
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::tabSelect( int idx )
{
    try {
	pageCyclRefrStop();
	pageDisplay(selPath);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::viewChildRecArea( QTreeWidgetItem *i, bool upTree )
{
    int64_t d_cnt = 0;
    if(mess_lev() == TMess::Debug) d_cnt = TSYS::curTime();

    QStringList grps = i->data(2,Qt::UserRole).toStringList();
    if(grps.empty()) return;
    else if(grps.size() > 1) {
	//Add and update the present ones
	for(int iG = 0; iG < grps.size(); iG++) {
	    bool grpChCnt = s2i(TSYS::strLine(grps[iG].toStdString(),0));
	    string grpId = TSYS::strLine(grps[iG].toStdString(), 1);
	    string grpDscr = TSYS::strLine(grps[iG].toStdString(), 2);
	    QTreeWidgetItem *it = NULL;
	    //Search present item
	    if(upTree)
		for(int iIt = 0; iIt < i->childCount(); iIt++)
		    if(i->child(iIt)->text(2) == ("*"+grpId).c_str())
		    { it = i->child(iIt); break; }
	    if(!it) it = new QTreeWidgetItem(i);
	    it->setText(0, (grpDscr+":").c_str());
	    it->setText(1, grpDscr.c_str());
	    it->setText(2, ("*"+grpId).c_str());
	    QStringList it_grp; it_grp.push_back(grps[iG]);
	    it->setData(2, Qt::UserRole, it_grp);
	    it->setFlags(Qt::ItemIsEnabled);
	    it->setChildIndicatorPolicy(grpChCnt?QTreeWidgetItem::ShowIndicator:QTreeWidgetItem::DontShowIndicator);
	    QFont fnt = it->font(0);
	    fnt.setItalic(true);
	    it->setFont(0,fnt);
	    // Next node for update
	    if(upTree && it->isExpanded()) viewChildRecArea(it,upTree);
	}
	//Deleting not presented
	for(int iIt = 0, iG = 0; upTree && iIt < i->childCount(); iIt++) {
	    for(iG = 0; iG < grps.size(); iG++)
		if(i->child(iIt)->text(2) == ("*"+TSYS::strLine(grps[iG].toStdString(),1)).c_str())
		    break;
	    if(iG >= grps.size()) { delete i->takeChild(iIt); iIt--; }
	}
    }
    else {
	string path = i->text(2).toStdString();
	if(path[0] == '*') path = i->parent()->text(2).toStdString();
	string grpId = TSYS::strLine(grps[0].toStdString(), 1);
	string grpDscr = TSYS::strLine(grps[0].toStdString(), 2);

	XMLNode req("chlds");
	req.setAttr("path",path+"/%2fobj")->setAttr("grp", grpId);
	if(cntrIfCmd(req)) {
	    //if(s2i(req.attr("rez")) == TError::Tr_Connect) initHosts();
	    mod->postMessCntr(req, this);
	    return;
	}
	//Add and update presented ones
	for(unsigned iE = 0; iE < req.childSize(); iE++) {
	    XMLNode *chEl = req.childGet(iE);
	    // Prepare branch patch
	    string br_path = grpId;
	    if(chEl->attr("id").size()) br_path.append(chEl->attr("id")); else br_path.append(chEl->text());
	    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
	    // Find item
	    QTreeWidgetItem *it = NULL;
	    if(upTree)
		for(int iIt = 0; iIt < i->childCount(); iIt++)
		    if(i->child(iIt)->text(2) == (path+"/"+br_path).c_str())
		    { it = i->child(iIt); break; }
	    if(!it) it = new QTreeWidgetItem(i);
	    it->setText(0, chEl->text().c_str());
	    it->setText(1, grpDscr.c_str());
	    it->setText(2, (path+"/"+br_path).c_str());
	    // Set icon
	    XMLNode *chIco = chEl->childGet("ico", 0, true);
	    if(chIco) {
		string simg = TSYS::strDecode(chIco->text(),TSYS::base64);
		QImage img;
		if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
		    it->setIcon(0, QPixmap::fromImage(img).scaled(icoSize(),icoSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
	    }
	    // Set groups
	    QStringList it_grp;
	    for(unsigned iG = 0; iG < chEl->childSize(); iG++)
		if(chEl->childGet(iG)->name() == "grp")
		    it_grp.push_back((chEl->childGet(iG)->attr("chPresent")+"\n"+chEl->childGet(iG)->attr("id")+"\n"+chEl->childGet(iG)->attr("dscr")).c_str());
	    it->setData(2,Qt::UserRole,it_grp);
	    // Check for childs presence
	    bool grpChCnt = it_grp.size() && (it_grp.size()>1 || s2i(TSYS::strLine(it_grp[0].toStdString(),0)));
	    it->setChildIndicatorPolicy(grpChCnt?QTreeWidgetItem::ShowIndicator:QTreeWidgetItem::DontShowIndicator);
	    // Next node for update
	    if(upTree && it->isExpanded()) viewChildRecArea(it, upTree);
	}
	//Delete no present
	if(upTree) {
	    //CtrTree->blockSignals(true);
	    for(unsigned iIt = 0, iE; iIt < (unsigned)i->childCount(); ) {
		for(iE = 0; iE < req.childSize(); iE++) {
		    // Prepare branch patch
		    string br_path = grpId;
		    if(req.childGet(iE)->attr("id").size()) br_path.append(req.childGet(iE)->attr("id"));
		    else br_path.append(req.childGet(iE)->text());
		    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
		    if(i->child(iIt)->text(2) == (path+"/"+br_path).c_str())	break;
		}
		if(iE >= req.childSize()) { delete i->takeChild(iIt); continue; }
		iIt++;
	    }
	    //CtrTree->blockSignals(false);
	}
    }

    if(mess_lev() == TMess::Debug)
	mess_debug(mod->nodePath().c_str(), _("Time of expanding/updating '%s': %f ms."), i->text(2).toStdString().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
}

QTreeWidgetItem *ConfApp::getExpandTreeWIt( const string &path )
{
    string sit, pathAdd, grp;
    QTreeWidgetItem *curIt = NULL;
    for(int off = 0, lev = 0; (sit=TSYS::pathLev(path,0,true,&off)).size(); lev++) {
	bool isOK = false, isGrpOK = false;
	if(curIt && !curIt->isExpanded()) curIt->setExpanded(true);
	for(int iIt = 0; iIt < (curIt?curIt->childCount():CtrTree->topLevelItemCount()); iIt++) {
	    QTreeWidgetItem *tit = curIt ? curIt->child(iIt) : CtrTree->topLevelItem(iIt);
	    if(tit->text(2)[0] == '*' &&
		    ((isGrpOK=sit.find(tit->text(2).toStdString().substr(1)) == 0) || (iIt+1) >= curIt->childCount()))
	    {
		curIt = isGrpOK ? tit : curIt->child(0); iIt = -1;
		if(!curIt->isExpanded()) curIt->setExpanded(true);
	    }
	    else if(tit->text(2)[0] != '*') {
		if((pathAdd+"/"+sit) == tit->text(2).toStdString()) { curIt = tit; isOK = true; break; }
		grp = TSYS::strParse(TSYS::pathLev(tit->text(2).toStdString(),lev,true), 0, "_");
		if(sit.find(grp+"_") != 0 && (pathAdd+"/"+grp+"_"+sit) == tit->text(2).toStdString())
		{ sit = grp+"_"+sit; curIt = tit; isOK = true; break; }
	    }
	}
	pathAdd += "/" + sit;
	if(!isOK) return NULL;
    }

    if(curIt && !curIt->isSelected()) curIt->setSelected(true);

    return curIt;
}

string ConfApp::getTreeWItNmPath( const string &path )
{
    string selNmPath;
    QTreeWidgetItem *treeIt = getExpandTreeWIt(path);
    while(treeIt) {
	selNmPath = treeIt->text(0).toStdString() + (selNmPath.size()?" > "+selNmPath:"");
	treeIt = treeIt->parent();
    }

    return selNmPath;
}

int ConfApp::cntrIfCmd( XMLNode &node )
{
    //Force for direct and multiple requests
    if(node.name() == "set" || node.name() == "load" || node.name() == "save") ;
    //Fill generic request
    else if(&node != &genReqs && genReqs.attr("fillMode") == "1") { genReqs.childAdd()->operator=(node); return -1; }
    //Get from generic request's result
    else if(&node != &genReqs && genReqs.childSize()) {
	XMLNode *sNd = NULL;
	int c_pos = s2i(genReqs.attr("curPos"));
	for( ; c_pos < (int)genReqs.childSize() && !sNd; c_pos++) {
	    XMLNode *wNd = genReqs.childGet(c_pos);
	    if(wNd->attr("path") == node.attr("path") && wNd->name() == node.name()) sNd = wNd;
	}
	if(!sNd && c_pos)
	    for(c_pos = 0; c_pos < (int)genReqs.childSize() && !sNd; c_pos++) {
		XMLNode *wNd = genReqs.childGet(c_pos);
		if(wNd->attr("path") == node.attr("path") && wNd->name() == node.name()) sNd = wNd;
	    }
	if(sNd) {
	    genReqs.setAttr("curPos", i2s(c_pos+1));
	    node = *sNd;
	    return s2i(node.attr("rez"));
	}
	node.setAttr("path", genReqs.attr("path")+"/"+node.attr("path"));
    }

    //Direct request
    try {
	node.setAttr("lang", lang(true));
	int rez = cntrIfCmdHosts(node);
	//int rez = SYS->transport().at().cntrIfCmd(node,"UIQtCfg",user());

	// Multiple requests to selected nodes in the tree
	if(rez == TError::NoError && (node.name() == "set" || node.name() == "load" || node.name() == "save") && CtrTree->selectedItems().size() >= 2) {
	    string reqPath = node.attr("path");
	    size_t reqElPos = reqPath.rfind("/");
	    if(reqElPos != string::npos) {
		string reqPathEl = reqPath.substr(reqElPos+1), selNds_lim;
		reqPath = reqPath.substr(0, reqElPos);
		vector<string> selNds;
		QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
		for(unsigned iEl = 0; (int)iEl < sel_ls.size(); iEl++)
		    if(sel_ls.at(iEl)->text(2).toStdString() != reqPath) {
			selNds.push_back(sel_ls.at(iEl)->text(2).toStdString());
			if(selNds.size() < GRP_SHOW_OP_LIM)
			    selNds_lim += (selNds_lim.size()?"\n":"") + selNds.back();
		    }
		if(selNds.size() > GRP_SHOW_OP_LIM)
		    selNds_lim += TSYS::strMess(_("... and yet %d nodes"), selNds.size()-GRP_SHOW_OP_LIM);
		if(selNds.size()) {
		    int questRes = QMessageBox::question(this, _("Sending the changes to the selected ones"),
			    QString(_("Send the command '%1' to other selected nodes: %2 ?")).arg(node.name().c_str()).arg(selNds_lim.c_str()),
			    QMessageBox::Apply|QMessageBox::Cancel, QMessageBox::Apply);
		    for(unsigned iSel = 0; questRes == QMessageBox::Apply && iSel < selNds.size(); ++iSel) {
			node.setAttr("path", selNds[iSel]+"/"+reqPathEl);
			cntrIfCmdHosts(node);
			//SYS->transport().at().cntrIfCmd(node, "UIQtCfg", user());
		    }
		}
	    }
	}
	return rez;
    } catch(TError &err) {
	node.childClear();
	node.setAttr("mcat",err.cat)->setAttr("rez",i2s(TError::Tr_Connect))->setText(err.mess);
    }

    return s2i(node.attr("rez"));
}

int ConfApp::cntrIfCmdHosts( XMLNode &node )
{
    string hostId = TSYS::pathLev(node.attr("path"), 0);
    SCADAHost *iHost = hosts[hostId];

    //No the host present
    if(!iHost) {
	node.childClear();
	node.setAttr("mcat", mod->nodePath())->
	     setAttr("rez", i2s(TError::Tr_UnknownHost))->
	     setText(TSYS::strMess(_("Unknown host '%s'."),hostId.c_str()));
	return s2i(node.attr("rez"));
    }

    //Main-first request
    inHostReq++;
    while(iHost->reqBusy()) {
	reqPrgrsSet(0, QString(_("Waiting the reply from the host '%1'")).arg(hostId.c_str()), iHost->reqTmMax);
	qApp->processEvents();
	TSYS::sysSleep(0.01);
    }
    bool done = false;
    if(!iHost->reqDo(node,done)) {
	reqPrgrsSet(0, QString(_("Waiting the reply from the host '%1'")).arg(hostId.c_str()), iHost->reqTmMax);

	//Wait for the request done
	time_t stTm = SYS->sysTm();
	while(!done) {
	    reqPrgrsSet(vmax(0,SYS->sysTm()-stTm));
	    if(reqPrgrs && reqPrgrs->wasCanceled()) {
		if(!actStartUpd->isEnabled()) pageCyclRefrStop();	//!!!! Could not check
		else iHost->sendSIGALRM();
	    }
	    if(!actStartUpd->isEnabled()) autoUpdTimer->start(iHost->reqTmMax*2000);	//To slow the cycled updating period
	    qApp->processEvents();
	    TSYS::sysSleep(0.01);
	}
    }
    inHostReq--;
    if(winClose && !inHostReq) close();

    return s2i(node.attr("rez"));
}

void ConfApp::reqPrgrsSet( int cur, const QString &lab, int max )
{
    //Create
    if(!reqPrgrs && cur >= 0) {
	reqPrgrs = new QProgressDialog(this);
	reqPrgrs->setWindowTitle((PACKAGE_NAME " "+mod->modId()).c_str());
	reqPrgrs->setWindowModality(Qt::WindowModal);
	reqPrgrs->setCancelButtonText(_("Cancel"));
	reqPrgrs->show();
    }
    //Close
    else if(reqPrgrs && cur < 0) {
	reqPrgrsTimer->stop();
	reqPrgrs->deleteLater();
	//delete reqPrgrs;
	reqPrgrs = NULL;
    }
    //Set the progress value
    if(reqPrgrs) {
	if(max >= 0)	reqPrgrs->setMaximum(max);
	if(lab.size())	reqPrgrs->setLabelText(lab);
	reqPrgrsTimer->start();
	if(cur && cur >= reqPrgrs->maximum())	reqPrgrs->setMaximum(cur+1);
	reqPrgrs->setValue(cur);
    }
}

/*string ConfApp::getPrintVal( const string &vl )
{
    bool isBool = false;
    for(unsigned iCh = 0; !isBool && iCh < vl.size(); ++iCh)
	switch(vl[iCh]) {
	    case 0 ... 8: isBool = true; break;
	}

    return isBool ? "B["+TSYS::strDecode(vl,TSYS::Bin)+"]" : vl;
}*/

bool ConfApp::compareHosts( const TTransportS::ExtHost &v1, const TTransportS::ExtHost &v2 )	{ return v1.name < v2.name; }

void ConfApp::initHosts( bool toReconnect )
{
    vector<TTransportS::ExtHost> stls;
    SYS->transport().at().extHostList(user(), stls, false, -1, lang(true));
    sort(stls.begin(), stls.end(), compareHosts);
    stls.insert(stls.begin(), TTransportS::ExtHost("",SYS->id()));

    //Remove for not present hosts
    for(unsigned iTop = 0, iH; iTop < (unsigned)CtrTree->topLevelItemCount(); ) {
	for(iH = 0; iH < stls.size(); iH++)
	    if(stls[iH].id == TSYS::pathLev(CtrTree->topLevelItem(iTop)->text(2).toStdString(),0))
		break;
	if(iH >= stls.size()) {
	    // Remove the host thread
	    map<string,SCADAHost*>::iterator iHst = hosts.find(TSYS::pathLev(CtrTree->topLevelItem(iTop)->text(2).toStdString(),0));
	    if(iHst != hosts.end()) { delete iHst->second; hosts.erase(iHst); }

	    // Remove the tree root item
	    delete CtrTree->takeTopLevelItem(iTop);

	    continue;
	}
	iTop++;
    }

    //Add/update hosts
    bool emptyTree = !CtrTree->topLevelItemCount();
    for(unsigned iSt = 0; iSt < stls.size(); iSt++) {
	QTreeWidgetItem *nit = NULL;
	if(!emptyTree)
	    for(int iTop = 0; iTop < CtrTree->topLevelItemCount(); iTop++)
		if(stls[iSt].id == TSYS::pathLev(CtrTree->topLevelItem(iTop)->text(2).toStdString(),0))
		{ nit = CtrTree->topLevelItem(iTop); break; }
	map<string, SCADAHost*>::iterator iHost = hosts.find(stls[iSt].id);

	if(!nit) {
	    nit = new QTreeWidgetItem(CtrTree);

	    // Append the host thread
	    if(iHost == hosts.end()) {
		hosts[stls[iSt].id] = new SCADAHost(stls[iSt].id.c_str(), user().c_str(), (stls[iSt].id!=SYS->id()), this);
		connect(hosts[stls[iSt].id], SIGNAL(setSt(const QString&,int,const QImage&,const QStringList&,const QString&)),
			this, SLOT(hostStSet(const QString&,int,const QImage&,const QStringList&,const QString&)), Qt::QueuedConnection);
		hosts[stls[iSt].id]->start();
	    } else iHost->second->userSet(user().c_str());
	}
	else if(iHost != hosts.end()) {
	    iHost->second->userSet(user().c_str());
	    if(toReconnect) {
		iHost->second->terminate();
		iHost->second->start();
	    }
	}

	if(stls[iSt].id == SYS->id()) {
	    nit->setText(0, trD_U(SYS->name(),user()).c_str());
	    nit->setText(1, _("Local station"));
	    nit->setText(2, ("/"+SYS->id()).c_str());
	}
	else {
	    nit->setText(0, trD_U(stls[iSt].name,user()).c_str());
	    nit->setText(1, _("Remote station"));
	    nit->setText(2, ("/"+stls[iSt].id).c_str());
	}
	//if(hosts[stls[iSt].id]) hosts[stls[iSt].id]->userSet(user().c_str());
    }
}

//*************************************************
//* ConfApp: Self widget's slots                  *
//*************************************************
void ConfApp::checkBoxStChange( int stat )
{
    QCheckBox *box = (QCheckBox *)sender();
    int updTm = CH_REFR_TM;

    if(stat == Qt::PartiallyChecked) return;
    try {
	string path = box->objectName().toStdString();
	string val = (stat==Qt::Checked) ? "1" : "0";

	//Check block element
	if(path[0] == 'b') {
	    SYS->ctrId(root,TSYS::strDecode(path.substr(1),TSYS::PathEl))->setText(val);
	    return;
	}
	else {
	    XMLNode req("get");
	    req.setAttr("path",selPath+"/"+path);
	    if(cntrIfCmd(req)) { mod->postMessCntr(req, this); return; }

	    if(req.text() == val) return;
	    mess_info(mod->nodePath().c_str(), _("%s| Set '%s' to '%s'!"),
		user().c_str(), (selPath+"/"+path).c_str(), val.c_str());

	    req.setName("set")->setAttr("primaryCmd", "1")->setText(val);
	    if(cntrIfCmd(req))	mod->postMessCntr(req, this);
	    else if(req.attr("updTm").size())
		updTm = vmin(CH_REFR_TM_MAX, s2r(req.attr("updTm"))*1000);
	}
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }

    //Redraw
    pageRefresh(updTm);
}

void ConfApp::buttonClicked( )
{
    QPushButton *button = (QPushButton *)sender();

    try {
	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(button->objectName().toStdString(),TSYS::PathEl));

	//Check link
	if(n_el->attr("tp") == "lnk") {
	    XMLNode req("get"); req.setAttr("path",selPath+"/"+button->objectName().toStdString());
	    if(cntrIfCmd(req)) { mod->postMessCntr(req, this); return; }
	    string url = "/" + TSYS::pathLev(selPath,0) + req.text();
	    mess_info(mod->nodePath().c_str(), _("%s| Went to the link '%s'!"), user().c_str(), url.c_str());
	    selectPage(url);
	    return;
	}
	else {
	    XMLNode req("set");
	    req.setAttr("path", selPath+"/"+button->objectName().toStdString())->
		setAttr("tp", n_el->name())->
		setAttr("primaryCmd", "1");
	    //Copy parameters
	    for(unsigned iCh = 0; iCh < n_el->childSize(); iCh++)
		//*(req.childAdd()) = *(n_el->childGet(iCh));
		req.childAdd(n_el->childGet(iCh)->name())->
					setAttr("id",n_el->childGet(iCh)->attr("id"))->
					setText(n_el->childGet(iCh)->text());

	    mess_info(mod->nodePath().c_str(), _("%s| Pressed down '%s'!"),
		user().c_str(), (selPath+"/"+button->objectName().toStdString()).c_str());
	    if(cntrIfCmd(req)) mod->postMessCntr(req, this);
	}
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }

    //Redraw
    pageRefresh(CH_REFR_TM);
}

void ConfApp::combBoxActivate( int ival )
{
    bool block = false;
    XMLNode *n_el;
    QComboBox *comb = (QComboBox *)sender();
    string val = comb->itemText(ival).toStdString();
    int updTm = CH_REFR_TM;

    try {
	string path = comb->objectName().toStdString();
	if(path[0] == 'b') { block = true; path = path.substr(1); }

	n_el = SYS->ctrId(root, TSYS::strDecode(path,TSYS::PathEl));

	//Get list for index list check!
	if(n_el->attr("dest") == "select") {
	    bool find_ok = false;
	    if(n_el->attr("select").empty()) {
		bool ind_ok = n_el->attr("sel_id").size();
		string s_nm;
		for(int ls_off = 0, c_el = 0; !find_ok && !(s_nm=TSYS::strSepParse(n_el->attr("sel_list"),0,';',&ls_off)).empty(); c_el++)
		    if(s_nm == val) {
			if(ind_ok)	val = TSYS::strSepParse(n_el->attr("sel_id"), c_el, ';');
			find_ok = true;
		    }
	    }
	    else {
		XMLNode x_lst("get");
		x_lst.setAttr("path",selPath+"/"+TSYS::strEncode(n_el->attr("select"),TSYS::PathEl));
		if(cntrIfCmd(x_lst)) { mod->postMessCntr(x_lst, this); return; }

		for(unsigned iEl = 0; !find_ok && iEl < x_lst.childSize(); iEl++)
		    if(x_lst.childGet(iEl)->name() == "el" && x_lst.childGet(iEl)->text() == val) {
			if(x_lst.childGet(iEl)->attr("id").size()) val = x_lst.childGet(iEl)->attr("id");
			find_ok = true;
		    }
	    }
	    if(!find_ok) { /*mod->postMess(mod->nodePath(),_("Value is missing: ")+val,TUIMod::Info,this);*/ return; }
	}

	//Checking the block element. Command box!
	if(block) { n_el->setText(val); return; }
	else {
	    XMLNode req("get"); req.setAttr("path",selPath+"/"+path);
	    if(cntrIfCmd(req)) { mod->postMessCntr(req, this); return; }

	    if(req.text() == val) return;
	    mess_info(mod->nodePath().c_str(),_("%s| Changed '%s' from '%s' to '%s'!"),
		    user().c_str(), (selPath+"/"+path).c_str(), req.text().c_str(), val.c_str());

	    req.setName("set")->setAttr("primaryCmd", "1")->setText(val);
	    if(cntrIfCmd(req)) mod->postMessCntr(req, this);
	    else if(req.attr("updTm").size())
		updTm = vmin(CH_REFR_TM_MAX, s2r(req.attr("updTm"))*1000);
	}
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }

    //Redraw
    pageRefresh(updTm);
}

void ConfApp::listBoxPopup( )
{
    QMenu popup;
    QListWidget *lbox = (QListWidget *)sender();
    QListWidgetItem *item = lbox->currentItem();
    string el_path = selPath+"/"+lbox->objectName().toStdString();
    XMLNode *n_el;

    QAction *last_it, *actBr, *actAdd, *actIns, *actEd, *actDel, *actMoveUp, *actMoveDown, *actCopy;
    last_it = actBr = actAdd = actIns = actEd = actDel = actMoveUp = actMoveDown = actCopy = NULL;

    try {
	n_el = SYS->ctrId(root, TSYS::strDecode(lbox->objectName().toStdString(),TSYS::PathEl));
	if(n_el->attr("tp") == "br" && item != NULL) {
	    actBr = last_it = new QAction(_("Go"), this);
	    popup.addAction(actBr);
	    popup.addSeparator();
	}
	if((s2i(n_el->attr("acs"))&SEC_WR) && n_el->attr("s_com").size()) {
	    if(n_el->attr("s_com").find("add") != string::npos) {
	        actAdd = last_it = new QAction(_("Add"), this);
		popup.addAction(actAdd);
	    }
	    if(n_el->attr("s_com").find("ins") != string::npos && item != NULL) {
		actIns = last_it = new QAction(_("Insert"), this);
		popup.addAction(actIns);
	    }
	    if(n_el->attr("s_com").find("edit") != string::npos && item != NULL) {
		actEd = last_it = new QAction(_("Edit"), this);
		popup.addAction(actEd);
	    }
	    if(n_el->attr("s_com").find("del") != string::npos && item != NULL) {
		popup.addSeparator();
		actDel = last_it = new QAction(_("Delete"), this);
		popup.addAction(actDel);
	    }
	    if(n_el->attr("s_com").find("move") != string::npos && item != NULL) {
		popup.addSeparator();
		actMoveUp = last_it = new QAction(_("Up"), this);
		popup.addAction(actMoveUp);
		actMoveDown = last_it = new QAction(_("Down"), this);
		popup.addAction(actMoveDown);
	    }
	}
	if(!lbox->selectedItems().isEmpty()) {
	    popup.addSeparator();
	    actCopy = last_it = new QAction(_("Copy"),this);
	    popup.addAction(actCopy);
	}

	if(last_it) {
	    string p_text, p_id;
	    string text, id;
	    int ind_m = s2i(n_el->attr("idm"));
	    int	c_id  = lbox->currentRow();

	    if(item != NULL) {
		//Get select id
		XMLNode x_lst("get");
		x_lst.setAttr("path", el_path);
		if(cntrIfCmd(x_lst)) { mod->postMessCntr(x_lst, this); return; }

		p_text = item->text().toStdString();
		if(ind_m)
		    for(unsigned iEl = 0; iEl < x_lst.childSize(); iEl++)
			if(x_lst.childGet(iEl)->text() == item->text().toStdString()) {
			    p_id = x_lst.childGet(iEl)->attr("id");
			    break;
			}
	    }

	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	{ popup.clear(); return; }

	    if(rez == actCopy) {
		QApplication::clipboard()->setText(lbox->selectedItems()[0]->text());
		popup.clear();
		return;
	    }
	    else if(rez == actAdd || rez == actIns || rez == actEd) {
		ReqIdNameDlg dlg(this, this->windowIcon(), "", _("Setting the item name"));
		vector<string> ils;
		ils.push_back(n_el->attr("idSz")+"\n"+i2s(ind_m));
		dlg.setTargets(ils);
		if(rez == actAdd)	dlg.setMess(_("Add a new element."));
		else if(rez == actIns)	dlg.setMess(_("Insert a new element."));
		if(rez == actEd) {
		    dlg.setMess(_("Rename the element."));
		    dlg.setId(p_id.c_str());
		    dlg.setName(p_text.c_str());
		}
		int dlrez = dlg.exec();
		id = dlg.id().toStdString();
		text = ind_m ? dlg.name().toStdString() : id;
		if(dlrez != QDialog::Accepted) return;
	    }

	    //Make command
	    XMLNode n_el1;
	    n_el1.setAttr("path", el_path);
	    if(rez == actBr) { listBoxGo(item); return; }
	    else if(rez == actAdd) {
		n_el1.setName("add");
		if(ind_m) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| '%s' added by the element <%s:%s>!"),
			user().c_str(), el_path.c_str(), id.c_str(), text.c_str());
	    }
	    else if(rez == actIns) {
		n_el1.setName("ins")->setAttr("pos", i2s(c_id))->setAttr("p_id",(ind_m)?p_id:p_text);
		if(ind_m) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| '%s' inserted by the element <%s:%s> to %d!"),
			user().c_str(), el_path.c_str(), id.c_str(), text.c_str(),c_id);
	    }
	    else if(rez == actEd) {
		n_el1.setName("edit")->setAttr("pos", i2s(c_id))->setAttr("p_id",(ind_m)?p_id:p_text);
		if(ind_m) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| '%s' set for the element %d to <%s:%s>!"),
			user().c_str(), el_path.c_str(), c_id, id.c_str(), text.c_str());
	    }
	    else if(rez == actDel) {
		n_el1.setName("del")->setAttr("pos", i2s(c_id));
		if(ind_m) n_el1.setAttr("id",p_id);
		else n_el1.setText(item->text().toStdString());
		mess_info(mod->nodePath().c_str(),_("%s| '%s' deleted for the element <%s:%s>!"),
			user().c_str(), el_path.c_str(), n_el1.attr("id").c_str(), n_el1.text().c_str());
	    }
	    else if(rez == actMoveUp || rez == actMoveDown) {
		int c_new = (rez==actMoveDown) ? c_id+1 : c_id-1;
		n_el1.setName("move")->setAttr("pos", i2s(c_id))->setAttr("to", i2s(c_new));
		mess_info(mod->nodePath().c_str(),_("%s| '%s' moved for the element %d to %d!"),
			user().c_str(), el_path.c_str(), c_id, c_new);
	    }
	    if(cntrIfCmd(n_el1)) { mod->postMessCntr(n_el1, this); return; }
	    pageRefresh(CH_REFR_TM);	//Redraw

	    if(n_el->attr("tp") == "br" && (rez == actAdd || rez == actIns || rez == actEd || rez == actDel))
		treeUpdate();

	    popup.clear();
	}
    } catch(TError &err) {
	mod->postMess(err.cat, err.mess, TUIMod::Error, this);
	pageRefresh(CH_REFR_TM);	//Redraw
    }
}

void ConfApp::tablePopup( const QPoint &pos )
{
    QMenu popup;
    QTableWidget *tbl = (QTableWidget *)sender();
    string el_path = selPath+"/"+tbl->objectName().toStdString();

    QAction *last_it, *actAdd, *actIns, *actDel, *actMoveUp, *actMoveDown, *actCopy, *actCopyForMWiki, *actFind, *actFindNext;
    last_it = actAdd = actIns = actDel = actMoveUp = actMoveDown = actCopy = actCopyForMWiki = actFind = actFindNext = NULL;

    int row = tbl->currentRow();
    bool noReload = false;

    try {
	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(tbl->objectName().toStdString(),TSYS::PathEl));

	if((s2i(n_el->attr("acs"))&SEC_WR) && n_el->attr("s_com").size()) {
	    string sVl, sVl1;
	    for(int off = 0; (sVl=TSYS::strParse(n_el->attr("s_com"),0,",",&off)).size(); )
		if((sVl1=TSYS::strParse(sVl,0,":")) == "add") {
		    actAdd = last_it = new QAction((sVl1=TSYS::strParse(sVl,1,":")).size()?sVl1.c_str():_("Add record"), this);
		    popup.addAction(actAdd);
		}
		else if(sVl1 == "ins") {
		    if(row < 0)	continue;
		    actIns = last_it = new QAction((sVl1=TSYS::strParse(sVl,1,":")).size()?sVl1.c_str():_("Insert record"), this);
		    popup.addAction(actIns);
		}
		else if(sVl1 == "del") {
		    if(row < 0)	continue;
		    actDel = last_it = new QAction((sVl1=TSYS::strParse(sVl,1,":")).size()?sVl1.c_str():_("Delete record"), this);
		    popup.addAction(actDel);
		}
		else if(sVl1 == "move") {
		    if(row < 0)	continue;
		    //popup.addSeparator();
		    actMoveUp = last_it = new QAction(_("Move Up (Ctrl+Up)"),this);
		    popup.addAction(actMoveUp);
		    actMoveDown = last_it = new QAction(_("Move Down (Ctrl+Down)"),this);
		    popup.addAction(actMoveDown);
		}
		else {	//User commands
		    string comId = sVl1;
		    last_it = new QAction((sVl1=TSYS::strParse(sVl,1,":")).size()?sVl1.c_str():comId.c_str(), this);
		    last_it->setObjectName(comId.c_str());
		    popup.addAction(last_it);
		}
	}
	if(!tbl->selectedItems().isEmpty()) {
	    popup.addSeparator();
	    actCopy = last_it = new QAction(_("Copy"), this);
	    popup.addAction(actCopy);
	    actCopyForMWiki = last_it = new QAction(_("Copy for MediaWiki"), this);
	    popup.addAction(actCopyForMWiki);
	}
	if(tbl->rowCount()) {
	    popup.addSeparator();
	    actFind = tbl->findChild<QAction*>("tableFind");
	    if(actFind) popup.addAction(last_it=actFind);
	    actFindNext = tbl->findChild<QAction*>("tableFindNext");
	    if(actFindNext) popup.addAction(last_it=actFindNext);
	}

	if(last_it) {
	    QString text;

	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	{ popup.clear(); return; }

	    if(rez == actCopy || rez == actCopyForMWiki) {
		QString cbRez;
		bool firstRow = false, firstClm = false, forMWiki = (rez == actCopyForMWiki);
		for(int iR = 0; iR < tbl->rowCount(); iR++) {
		    //if(firstRow && firstClm) cbRez += "\n";
		    firstClm = false;
		    for(int iC = 0; iC < tbl->columnCount(); iC++) {
			if(!tbl->item(iR,iC)->isSelected()) continue;
			if(firstClm) cbRez += forMWiki ? " || " : "\t";
			if(!firstClm && firstRow) cbRez += "\n";
			if(!firstClm && forMWiki) cbRez += "|-\n| ";
			cbRez += tbl->item(iR,iC)->text();
			firstClm = firstRow = true;
		    }
		}
		QApplication::clipboard()->setText(cbRez);
		popup.clear();
		return;
	    }
	    else if(rez == actFind || rez == actFindNext) return;

	    XMLNode n_el1;
	    n_el1.setAttr("path", el_path)->setAttr("primaryCmd", "1");
	    if(rez == actAdd) {
		n_el1.setName("add");
		mess_info(mod->nodePath().c_str(), _("%s| '%s' add by a record."),
			user().c_str(), el_path.c_str());
	    }
	    else if(rez == actIns) {
		n_el1.setName("ins");
		n_el1.setAttr("row", i2s(row));
		mess_info(mod->nodePath().c_str(),_("%s| '%s' inserted for the record %d."),
			user().c_str(), el_path.c_str(), row);
	    }
	    else if(rez == actMoveUp || rez == actMoveDown) {
		int r_new = row-1;
		if(rez == actMoveDown)	r_new = row+1;
		n_el1.setName("move");
		n_el1.setAttr("row", i2s(row))->setAttr("to", i2s(r_new));
		mess_info(mod->nodePath().c_str(),_("%s| '%s' moved for the record %d to %d."),
			user().c_str(), el_path.c_str(), row, r_new);
	    }
	    else {	//Key commands
		string row_addr, key;
		if(!n_el->attr("key").size()) n_el1.setAttr("row", (row_addr=i2s(row)));
		else if(row >= 0) {	// Getting the Key columns
		    for(int iOff = 0; (key=TSYS::strSepParse(n_el->attr("key"),0,',',&iOff)).size(); )
			for(unsigned iEl = 0; iEl < n_el->childSize(); iEl++)
			    if(n_el->childGet(iEl)->attr("id") == key) {
				n_el1.setAttr("key_"+key,n_el->childGet(iEl)->childGet(row)->text());
				row_addr = row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
				break;
			    }
		}

		if(rez == actDel) {
		    n_el1.setName("del");
		    mess_info(mod->nodePath().c_str(),_("%s| '%s' deleted for the record '%s'."),
			user().c_str(), el_path.c_str(), row_addr.c_str());
		}
		else {	//User commands
		    n_el1.setName(rez->objectName().toStdString());
		    mess_info(mod->nodePath().c_str(),_("%s| '%s' user command '%s(%s)' to the record '%s'."),
			user().c_str(), el_path.c_str(),
			rez->text().toStdString().c_str(), rez->objectName().toStdString().c_str(), row_addr.c_str());
		}
	    }

	    if(cntrIfCmd(n_el1)) throw TError(n_el1.attr("mcat"), n_el1.text());
	    noReload = s2i(n_el1.attr("noReload"));

	    popup.clear();
	}
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }

    if(!noReload) pageRefresh(CH_REFR_TM);	//Redraw
}

void ConfApp::tableFind( )
{
    QAction *actFind = (QAction *)sender();
    QTableWidget *tbl = (QTableWidget *)actFind->parent();
    if(!tbl->rowCount()) return;

    if(actFind->objectName() == "tableFindNext") {
	int findPos = tbl->property("findPos").toInt();
	string findIt = TSYS::strParse(tbl->property("findRez").toString().toStdString(),findPos,"|");
	tbl->setCurrentItem(tbl->item(s2i(TSYS::strParse(findIt,0,":")),s2i(TSYS::strParse(findIt,1,":"))));
	tbl->setProperty("findPos", ++findPos);
	if(findPos >= tbl->property("findRezLen").toInt()) actFind->setEnabled(false);
	return;
    }

    int fopt = tbl->property("findOpt").toInt();
    QString fstr = tbl->property("findStr").toString();
    InputDlg dlg(this, actFind->icon(), QString(_("Enter a string to search:")), _("Searching a string"), 0, 0);
    QLineEdit *le = new QLineEdit(fstr, &dlg);
    dlg.edLay->addWidget(le, 0, 0);
    QCheckBox *cs = new QCheckBox(_("Case sensitively"), &dlg);
    if(fopt & Qt::MatchCaseSensitive) cs->setCheckState(Qt::Checked);
    dlg.edLay->addWidget(cs, 1, 0);
    le->setFocus(Qt::OtherFocusReason);
    if(dlg.exec() == QDialog::Accepted && !le->text().isEmpty()) {
	tbl->setProperty("findStr", le->text());
	Qt::MatchFlags flgs = Qt::MatchContains;
	if(cs->checkState() == Qt::Checked) flgs |= Qt::MatchCaseSensitive;
	tbl->setProperty("findOpt", (int)flgs);
	QList<QTableWidgetItem *> its = tbl->findItems(le->text(), flgs);
	if(its.length()) tbl->setCurrentItem(its[0]);
	if(its.length() > 1) {
	    tbl->setProperty("findPos", 0);
	    string fRez;
	    for(int iIt = 1; iIt < its.length(); ++iIt)
		fRez += TSYS::strMess("%d:%d|",its[iIt]->row(),its[iIt]->column());
	    tbl->setProperty("findRez", fRez.c_str());
	    tbl->setProperty("findRezLen", its.length()-1);
	    if((actFind=tbl->findChild<QAction*>("tableFindNext"))) actFind->setEnabled(true);
	}
	else { tbl->setProperty("findPos", 0); tbl->setProperty("findRez", ""); tbl->setProperty("findRezLen", 0); }
    }
}

void ConfApp::imgPopup( const QPoint &pos )
{
    QMenu popup;
    ImgView *img = (ImgView *)sender();
    string el_path = selPath+"/"+img->objectName().toStdString();

    QAction *last_it, *save_img, *load_img, *clear_img;
    last_it = save_img = load_img = clear_img = NULL;

    try {
	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(img->objectName().toStdString(),TSYS::PathEl));
	if(!img->image().isNull()) {
	    save_img = last_it = new QAction(_("Save image"),this);
	    popup.addAction(save_img);
	}
	if(s2i(n_el->attr("acs"))&SEC_WR) {
	    load_img = last_it = new QAction(_("Load image"),this);
	    popup.addAction(load_img);
	    if(!img->image().isNull()) {
		clear_img = last_it = new QAction(_("Clear image"),this);
		popup.addAction(clear_img);
	    }
	}

	if(last_it) {
	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	return;
	    if(rez == save_img) {
		QString fileName = QFileDialog::getSaveFileName(this,_("Saving the picture"),"img.png",_("Images (*.png *.xpm *.jpg)"));
		if(!fileName.isEmpty() && !img->image().save(fileName))
		    throw TError(mod->nodePath().c_str(), _("Error saving to the file '%s'\n"), fileName.toStdString().c_str());
	    }
	    else if(rez == load_img) {
		//Get path to image file
		QString fileName = QFileDialog::getOpenFileName(this,_("Loading the picture"),"",_("Images (*.png *.jpg)"));
		if(fileName.isNull()) return;
		int len;
		char buf[prmStrBuf_SZ];
		string rez;

		//Load image file
		int hd = open(fileName.toStdString().c_str(), O_RDONLY);
		if(hd < 0) throw TError(mod->nodePath().c_str(), _("Error opening the file '%s'\n"), fileName.toStdString().c_str());
		while((len=read(hd,buf,sizeof(buf))) > 0) rez.append(buf, len);
		if(::close(hd) != 0)
		    mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), hd, strerror(errno), errno);

		//Set image to widget
		if(!img->setImage(rez))
		    throw TError(mod->nodePath().c_str(), _("Error image file '%s'\n"), fileName.toStdString().c_str());

		//Send image to system
		XMLNode n_el1("set");
		n_el1.setAttr("path",el_path)->setAttr("primaryCmd", "1")->setText(TSYS::strEncode(rez,TSYS::base64));
		mess_info(mod->nodePath().c_str(), _("%s| '%s' uploaded by the picture '%s'."),
		    user().c_str(), el_path.c_str(), fileName.toStdString().c_str());
		if(cntrIfCmd(n_el1)) { mod->postMessCntr(n_el1, this); return; }
	    }
	    else if(rez == clear_img) {
		XMLNode n_el1("set");
		n_el1.setAttr("path", el_path)->setAttr("primaryCmd", "1")->setText("");
		mess_info(mod->nodePath().c_str(), _("%s| '%s' cleared."), user().c_str(), el_path.c_str());
		if(cntrIfCmd(n_el1)) { mod->postMessCntr(n_el1, this); return; }
		img->setImage("");
	    }
	}
    } catch(TError &err) {
	mod->postMess(err.cat, err.mess, TUIMod::Error, this);
	pageRefresh(CH_REFR_TM);	//Redraw
    }
}

void ConfApp::tableSet( int row, int col )
{
    bool noReload = false, isLocSel = false;
    string value;
    if(tblInit || row < 0 || col < 0) return;

    try {
	QTableWidget *tbl = (QTableWidget *)sender();
	string el_path = selPath+"/"+tbl->objectName().toStdString();

	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(tbl->objectName().toStdString(),TSYS::PathEl));

	QVariant val = tbl->item(row,col)->data(Qt::EditRole);
	if(n_el->childGet(col)->attr("tp") == "bool") value = val.toBool() ? "1" : "0";
	else if((isLocSel=n_el->childGet(col)->childGet(row)->attr("dest") == "select") || n_el->childGet(col)->attr("dest") == "select") {
	    XMLNode *selO = isLocSel ? n_el->childGet(col)->childGet(row) : n_el->childGet(col);

	    value = val.toString().toStdString();
	    bool find_ok = false;
	    if(selO->attr("select").empty()) {
		bool ind_ok = selO->attr("sel_id").size();
		string s_nm;
		for(int ls_off = 0, c_el = 0; !(s_nm=TSYS::strSepParse(selO->attr("sel_list"),0,';',&ls_off)).empty(); c_el++)
		    if(s_nm == value) {
			if(ind_ok) value = TSYS::strSepParse(selO->attr("sel_id"),c_el,';');
			find_ok = true;
		    }
	    }
	    else {
		XMLNode x_lst("get"); x_lst.setAttr("path",selPath+"/"+TSYS::strEncode(selO->attr("select"),TSYS::PathEl));
		if(cntrIfCmd(x_lst)) { mod->postMessCntr(x_lst, this); return; }

		bool ind_ok = s2i(selO->attr("idm"));
		for(unsigned iEl = 0; iEl < x_lst.childSize(); iEl++) {
		    if(!iEl) ind_ok = x_lst.childGet(iEl)->attr("id").size();
		    if(x_lst.childGet(iEl)->text() == value) {
			if(ind_ok) value = x_lst.childGet(iEl)->attr("id");
			find_ok = true;
		    }
		}
	    }
	    if(!find_ok) throw TError(mod->nodePath().c_str(),_("The value '%s' is wrong!"),value.c_str());
	}
	else value = val.toString().toStdString();

	//Prepare request
	XMLNode n_el1("set"); n_el1.setAttr("path",el_path)->setAttr("primaryCmd", "1")->setText(value);
	// Get current column id
	for(unsigned iEl = 0; iEl < n_el->childSize(); iEl++)
	    if(tbl->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString() == n_el->childGet(iEl)->attr("dscr").c_str())
	    { n_el1.setAttr("col", n_el->childGet(iEl)->attr("id")); break; }
	// Get row position
	string row_addr;
	if(!n_el->attr("key").size()) {
	    row_addr = i2s(row);
	    n_el1.setAttr("row", row_addr);
	}
	else {
	    // Get Key columns
	    string key;
	    for(int iOff = 0; (key=TSYS::strSepParse(n_el->attr("key"),0,',',&iOff)).size(); )
		for(unsigned iEl = 0; iEl < n_el->childSize(); iEl++)
		    if(n_el->childGet(iEl)->attr("id") == key) {
			n_el1.setAttr("key_"+key,n_el->childGet(iEl)->childGet(row)->text());
			row_addr = row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
			break;
		    }
	}

	// Put request
	mess_info(mod->nodePath().c_str(),_("%s| '%s' set for cell ('%s':%s) to: %s."),
	    user().c_str(), el_path.c_str(), row_addr.c_str(), n_el1.attr("col").c_str(), value.c_str());
	if(cntrIfCmd(n_el1))	throw TError(n_el1.attr("mcat"), n_el1.text());
	noReload = s2i(n_el1.attr("noReload"));
	if(noReload) n_el->childGet(col)->childGet(row)->setText(value);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }

    if(!noReload) pageRefresh(CH_REFR_TM);
}

void ConfApp::listBoxGo( QListWidgetItem* item )
{
    string path;

    if(!item)	return;
    QListWidget *lbox = item->listWidget();
    try {
	XMLNode &t_c = *TCntrNode::ctrId(root,TSYS::strDecode(lbox->objectName().toStdString(),TSYS::PathEl));
	string br_pref = t_c.attr("br_pref");

	XMLNode req("get"); req.setAttr("path",selPath+"/"+lbox->objectName().toStdString());
	if(cntrIfCmd(req)) throw TError(mod->nodePath(), req.text());

	//Find selected index
	bool sel_ok = false;
	for(unsigned iEl = 0; iEl < req.childSize(); iEl++)
	    if(req.childGet(iEl)->name() == "el" && req.childGet(iEl)->text() == item->text().toStdString()) {
		if(req.childGet(iEl)->attr("id").size())
		    path = selPath+"/"+TSYS::strEncode(br_pref+req.childGet(iEl)->attr("id"),TSYS::PathEl);
		else
		    path = selPath+"/"+TSYS::strEncode(br_pref+req.childGet(iEl)->text(),TSYS::PathEl);
		sel_ok = true;
	    }
	if(!sel_ok) throw TError(mod->nodePath().c_str(), _("Selective element '%s' is missing!"), item->text().toStdString().c_str());

	selectPage(path, CH_REFR_TM);
	//selPath = path;
	//pageRefresh(CH_REFR_TM);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::editChange( const QString& txt )
{
    QWidget *wed = (QWidget*)sender();

    try {
	string path = wed->objectName().toStdString();

	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	SYS->ctrId(root, TSYS::strDecode(path,TSYS::PathEl))->setText(txt.toStdString());
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::applyButton( )
{
    QWidget *bwidg = (QWidget*)sender();
    string path = bwidg->objectName().toStdString();
    int updTm = CH_REFR_TM;

    try {
	XMLNode *el = SYS->ctrId(root, TSYS::strDecode(path,TSYS::PathEl));
	string sval = el->text();
	if(el->attr("tp") == "dec" || el->attr("tp") == "hex" || el->attr("tp") == "oct") {
	    //Check and change decimal format and the limits control
	    if(TRegExp("^0x[0-9a-fA-F]+$").test(sval))
		el->setAttr("tpCh", "hex");
	    else if(sval.size() > 1 && sval[0] == '0')
		el->setAttr("tpCh", "oct");
	    else el->setAttr("tpCh", "dec");

	    long long vl = strtoll(sval.c_str(), NULL, 0);
	    if(el->attr("min").size()) vl = vmax(s2ll(el->attr("min")), vl);
	    if(el->attr("max").size()) vl = vmin(s2ll(el->attr("max")), vl);
	    sval = ll2s(vl);
	}
	else if(el->attr("tp") == "real") {
	    double vl = s2r(sval);
	    if(el->attr("min").size()) vl = vmax(s2r(el->attr("min")), vl);
	    if(el->attr("max").size()) vl = vmin(s2r(el->attr("max")), vl);
	    sval = r2s(vl);
	}

	mess_info(mod->nodePath().c_str(), _("%s| '%s' changed to: '%s'!"),
		user().c_str(), (selPath+"/"+path).c_str(), sval.c_str());

	XMLNode n_el("set");
	n_el.setAttr("path", selPath+"/"+path)->setAttr("primaryCmd", "1")->setText(sval);
	if(cntrIfCmd(n_el)) mod->postMessCntr(n_el, this);
	else if(n_el.attr("updTm").size())
	    updTm = vmin(CH_REFR_TM_MAX, s2r(n_el.attr("updTm"))*1000);
    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }

    //Redraw only for changing in same this widget
    pageRefresh(updTm);
}

void ConfApp::cancelButton( )
{
    QWidget *bwidg = (QWidget *)sender();

    string path = bwidg->objectName().toStdString();

    //Redraw
    pageRefresh(CH_REFR_TM);
}

#undef _
#define _(mess) mod->I18N(mess, ((ConfApp*)parent())->lang().c_str()).c_str()

//***********************************************
// SHost - Host thread's control object         *
SCADAHost::SCADAHost( const QString &iid, const QString &iuser, bool iIsRemote, QObject *p ) :
    QThread(p), isRemote(iIsRemote), reqTmMax(0), id(iid), user(iuser), lnkOK(false), endRun(false), reqDone(false), tm(0), req(NULL), done(NULL), pid(0)
{

}

SCADAHost::~SCADAHost( )
{
    terminate();
}

void SCADAHost::terminate( )
{
    if(isRunning()) {
	endRun = true;
	while(!wait(100)) sendSIGALRM();
    }
}

void SCADAHost::userSet( const QString &iuser )
{
    mtx.lock();
    user = iuser;
    tm = 0;
    mtx.unlock();
}

void SCADAHost::sendSIGALRM( )
{
    if(pid) pthread_kill(pid, SIGALRM);
}

bool SCADAHost::reqDo( XMLNode &node, bool &idone )
{
    if(req) return false;

    //Set the request
    mtx.lock();
    reqDone = false;
    req = &node;
    done = &idone; *done = false;
    cond.wakeOne();
    cond.wait(mtx, 100);
    if(!reqDone) { mtx.unlock(); return false; }
    *done = true;
    done = NULL;
    req = NULL;
    reqDone = false;
    mtx.unlock();

    return true;
}

bool SCADAHost::reqBusy( )
{
    if(req && !reqDone)	return true;

    //Free done status
    if(reqDone) {
	mtx.lock();
	done = NULL;
	req = NULL;
	reqDone = false;
	mtx.unlock();
    }

    return false;
}

void SCADAHost::run( )
{
    bool isFirst = true;
    int rez = 0;
    string stmp;
    QString wuser;
    QImage imgConnEst, imgDisConnect, imgRemConnected, img;

    pid = pthread_self();
    endRun = false;

    //Images for statuses init
    stmp = TUIS::icoGet("connecting"); imgConnEst.loadFromData((const uchar*)stmp.c_str(), stmp.size());
    stmp = TUIS::icoGet("disconnect"); imgDisConnect.loadFromData((const uchar*)stmp.c_str(), stmp.size());
    stmp = TUIS::icoGet("connected"); imgRemConnected.loadFromData((const uchar*)stmp.c_str(), stmp.size());

    while(!endRun && !((ConfApp*)parent())->winClose) {
	mtx.lock(); wuser = user; mtx.unlock();
	//Link status processing
	if(isFirst || (!lnkOK && (SYS->sysTm()-tm) > s2i(TSYS::strParse(mod->tmConChk(),0,":"))) ||
		      (lnkOK && (SYS->sysTm()-tm) > s2i(TSYS::strParse(mod->tmConChk(),1,":")))) {
	    emit setSt(id, -1, imgConnEst);

	    // Check connection by the station name, icon and branches request
	    QString toolTip;
	    QStringList brs;
	    XMLNode req("CntrReqs"), *reqN;
	    req.setAttr("path", "/"+id.toStdString())->
		setAttr("lang", Mess->langCode(user.toStdString(),true));
	    req.childAdd("get")->setAttr("path", "%2fgen%2fstat");
	    req.childAdd("get")->setAttr("path", "%2fico");
	    req.childAdd("info")->setAttr("path","%2fbr");
	    if(!(rez=cntrIfCmd(req,wuser))) {
		// Station name set as toolTip
		if((reqN=req.childGet(0,true))) toolTip = reqN->text().c_str();
		// Icon get
		if((reqN=req.childGet(1,true))) {
		    stmp = TSYS::strDecode(reqN->text(), TSYS::base64);
		    if(!img.loadFromData((const uchar*)stmp.c_str(),stmp.size()) && isRemote) img = imgRemConnected;
		}
		// Branches list
		if((reqN=req.childGet(2,true)))
		    for(unsigned iBr = 0; reqN->childSize() && iBr < reqN->childGet(0)->childSize(); iBr++)
			brs.push_back(("1\n"+reqN->childGet(0)->childGet(iBr)->attr("id")+"\n"+reqN->childGet(0)->childGet(iBr)->attr("dscr")).c_str());

		lnkOK = true;
	    }
	    else if(rez/* == TError::Tr_Connect*/) {
		img = imgDisConnect;
		toolTip = req.text().c_str();

		lnkOK = false;
	    }

	    emit setSt(id, lnkOK, img, brs, toolTip);
	    tm = SYS->sysTm();
	}

	//Interface's requests processing
	mtx.lock();
	if(!req || reqDone) cond.wait(mtx, 1000);
	if(req && !reqDone) {
	    wuser = user;
	    mtx.unlock();
	    if(lnkOK) lnkOK = (rez=cntrIfCmd(*req,wuser)) != TError::Tr_Connect;
	    else {
		req->childClear();
		req->setAttr("mcat",mod->nodePath()+"/"+id.toStdString())->setAttr("rez",i2s(TError::Tr_Connect))->setText(_("No connection is established"));
	    }
	    mtx.lock();
	    reqDone = *done = true;
	    cond.wakeOne();
	}
	mtx.unlock();

	isFirst = false;
    }
}

int SCADAHost::cntrIfCmd( XMLNode &node, const QString &iuser )
{
    try {
	time_t stTm = SYS->sysTm();
	int rez = SYS->transport().at().cntrIfCmd(node, "UIQtCfg", iuser.toStdString());
	reqTmMax = vmax(reqTmMax, (tm=SYS->sysTm())-stTm);
	return rez;
    } catch(TError &err) {
	node.childClear();
	node.setAttr("mcat",err.cat)->setAttr("rez",i2s(TError::Tr_Connect))->setText(err.mess);
	tm = 0;		//Check the link immediately
    }

    return s2i(node.attr("rez"));
}

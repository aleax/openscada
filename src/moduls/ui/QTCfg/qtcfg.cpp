
//OpenSCADA system module UI.QTCfg file: qtcfg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <QApplication>
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
#include <QMenu>
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

#include <tmess.h>
#include <tsys.h>
#include <tsecurity.h>

#include "selfwidg.h"
#include "tuimod.h"
#include "qtcfg.h"

#define CH_REFR_TM	100
#define TOOL_TIP_LIM	150
#define TOOL_TIP_ADD	"Shift+F1"

using namespace QTCFG;

//*************************************************
//* ConfApp                                       *
//*************************************************
ConfApp::ConfApp( string open_user ) : reqPrgrs(NULL),
    pgInfo("info"), genReqs("CntrReqs"), root(&pgInfo), copyBuf("0"), queSz(20), inHostReq(0), tblInit(false), pgDisplay(false)
{
    //Main window settings
    setAttribute(Qt::WA_DeleteOnClose, true);
    QImage ico_t;
    mod->regWin(this);

    setWindowTitle((PACKAGE_NAME " "+mod->modId()+": "+trU(SYS->name(),open_user)).c_str());
    setWindowIcon(mod->icon());

    //Init centrall widget
    setCentralWidget(new QWidget(this));
    QGridLayout *QTCfgLayout = new QGridLayout(centralWidget());
    QTCfgLayout->setMargin(3);

    //Init splitter
    QSplitter *splitter = new QSplitter(centralWidget());
    splitter->setOrientation(Qt::Horizontal);

    //Create Navigator tree
    QFrame *frm = new QFrame(splitter);
    QVBoxLayout *vlay = new QVBoxLayout;
    vlay->setMargin(0);
    CtrTree = new QTreeWidget;
    vlay->addWidget(CtrTree);
    CtrTree->setContextMenuPolicy(Qt::CustomContextMenu);
    CtrTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    CtrTree->setAutoScroll(false);
    //splitter->setSizeConstraint(QSplitter::KeepSize);
    CtrTree->setHeaderLabels(QStringList() << _("Name") << _("Type") << _("Path"));
    CtrTree->header()->setStretchLastSection(false);
    CtrTree->header()->resizeSection(0, 200);
    //CtrTree->header()->setSectionHidden(1,true);
    //CtrTree->header()->setSectionHidden(2,true);
    //CtrTree->header()->hide();
    //CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );
    CtrTree->setMinimumSize(QSize(150,0));
    CtrTree->setWhatsThis(_("The main navigation tree of the configurator."));
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
    QFrame *gFrame = new QFrame(splitter);
    gFrame->setWhatsThis(_("The main configurator frame."));
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
    //titleLab->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
    QFont titleLab_font(titleLab->font());
    titleLab_font.setPointSize(14);
    titleLab->setFont(titleLab_font);
    titleLab->setWordWrap(true);
    gFrameLayout->addWidget(titleLab, 0, 1);

    //Create the tabulator
    tabs = new QTabWidget(gFrame);
    tabs->setWhatsThis(_("Tabulated configuration pages."));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabSelect(int)));
    gFrameLayout->addWidget(tabs, 1, 0, 1, 2);

    QList<int> splSz;
    splSz.push_back(200);
    splSz.push_back(600);
    splitter->setSizes(splSz);
    QTCfgLayout->addWidget(splitter, 0, 0);

    resize(800, 600);

    //Create actions
    // Close
    if(!ico_t.load(TUIS::icoGet("close",NULL,true).c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close configurator window"));
    actClose->setWhatsThis(_("The button for closing OpenSCADA configurator window"));
    actClose->setStatusTip(_("Press to close OpenSCADA configurator window"));
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    // Quit
    if(!ico_t.load(TUIS::icoGet("exit",NULL,true).c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA"));
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quitSt()));
    // Up button
    if(!ico_t.load(TUIS::icoGet("up",NULL,true).c_str())) ico_t.load(":/images/up.png");
    actUp = new QAction(QPixmap::fromImage(ico_t),_("&Up"),this);
    actUp->setShortcut(Qt::ALT+Qt::Key_Up);
    actUp->setToolTip(_("Up page"));
    actUp->setWhatsThis(_("The button for going to level up"));
    actUp->setStatusTip(_("Press for going to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(triggered()), this, SLOT(pageUp()));
    // Previous page
    if(!ico_t.load(TUIS::icoGet("previous",NULL,true).c_str())) ico_t.load(":/images/previous.png");
    actPrev = new QAction(QPixmap::fromImage(ico_t),_("&Previous"),this);
    actPrev->setShortcut(Qt::ALT+Qt::Key_Left);
    actPrev->setToolTip(_("Previous page"));
    actPrev->setWhatsThis(_("The button for going to previous page"));
    actPrev->setStatusTip(_("Press for going to previous page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(triggered()), this, SLOT(pagePrev()));
    // Next page
    if(!ico_t.load(TUIS::icoGet("next",NULL,true).c_str())) ico_t.load(":/images/next.png");
    actNext = new QAction(QPixmap::fromImage(ico_t),_("&Next"),this);
    actNext->setShortcut(Qt::ALT+Qt::Key_Right);
    actNext->setToolTip(_("Next page"));
    actNext->setWhatsThis(_("The button for going to next page"));
    actNext->setStatusTip(_("Press for going to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(triggered()), this, SLOT(pageNext()));
    //  Load item from db
    if(!ico_t.load(TUIS::icoGet("load",NULL,true).c_str())) ico_t.load(":/images/load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t),_("Load from DB"),this);
    actDBLoad->setToolTip(_("Load item data from DB"));
    actDBLoad->setWhatsThis(_("The button for loading item data from DB"));
    actDBLoad->setStatusTip(_("Press for loading item data from DB."));
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(triggered()), this, SLOT(itDBLoad()));
    //  Save item to db
    if(!ico_t.load(TUIS::icoGet("save",NULL,true).c_str())) ico_t.load(":/images/save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t),_("Save to DB"),this);
    actDBSave->setToolTip(_("Save item data to DB"));
    actDBSave->setWhatsThis(_("The button for saving item data to DB"));
    actDBSave->setStatusTip(_("Press for saving item data to DB."));
    actDBSave->setShortcut(QKeySequence("Ctrl+S"));
    actDBSave->setEnabled(false);
    connect(actDBSave, SIGNAL(triggered()), this, SLOT(itDBSave()));
    // Item add
    if(!ico_t.load(TUIS::icoGet("it_add",NULL,true).c_str())) ico_t.load(":/images/it_add.png");
    actItAdd = new QAction(QPixmap::fromImage(ico_t),_("&Add"),this);
    actItAdd->setToolTip(_("Add item"));
    actItAdd->setWhatsThis(_("The button for adding new item"));
    actItAdd->setStatusTip(_("Press for adding new item."));
    actItAdd->setEnabled(false);
    connect(actItAdd, SIGNAL(triggered()), this, SLOT(itAdd()));
    // Item delete
    if(!ico_t.load(TUIS::icoGet("it_del",NULL,true).c_str())) ico_t.load(":/images/it_del.png");
    actItDel = new QAction(QPixmap::fromImage(ico_t),_("&Delete"),this);
    actItDel->setToolTip(_("Delete item"));
    actItDel->setWhatsThis(_("The button for deleting item"));
    actItDel->setStatusTip(_("Press for deleting item."));
    actItDel->setShortcut(QKeySequence("Ctrl+D"));
    actItDel->setEnabled(false);
    connect(actItDel, SIGNAL(triggered()), this, SLOT(itDel()));
    //  Cut item
    if(!ico_t.load(TUIS::icoGet("editcut",NULL,true).c_str())) ico_t.load(":/images/editcut.png");
    actItCut = new QAction(QPixmap::fromImage(ico_t),_("Item cut"),this);
    actItCut->setToolTip(_("Cutting of the item"));
    actItCut->setWhatsThis(_("The button for cutting of the item"));
    actItCut->setStatusTip(_("Press to cut the item."));
    actItCut->setShortcut(QKeySequence("Ctrl+X"));
    actItCut->setEnabled(false);
    connect(actItCut, SIGNAL(triggered()), this, SLOT(itCut()));
    //  Copy item
    if(!ico_t.load(TUIS::icoGet("editcopy",NULL,true).c_str())) ico_t.load(":/images/editcopy.png");
    actItCopy = new QAction(QPixmap::fromImage(ico_t),_("Item copy"),this);
    actItCopy->setToolTip(_("Copying of the item"));
    actItCopy->setWhatsThis(_("The button for copying of the item"));
    actItCopy->setStatusTip(_("Press to copy the item."));
    actItCopy->setShortcut(QKeySequence("Ctrl+C"));
    actItCopy->setEnabled(false);
    connect(actItCopy, SIGNAL(triggered()), this, SLOT(itCopy()));
    //  Paste item
    if(!ico_t.load(TUIS::icoGet("editpaste",NULL,true).c_str())) ico_t.load(":/images/editpaste.png");
    actItPaste = new QAction(QPixmap::fromImage(ico_t),_("Item paste"),this);
    actItPaste->setToolTip(_("Pasting of the item"));
    actItPaste->setWhatsThis(_("The button for pasting of the item"));
    actItPaste->setStatusTip(_("Press to paste the item."));
    actItPaste->setShortcut(QKeySequence("Ctrl+V"));
    actItPaste->setEnabled(false);
    connect(actItPaste, SIGNAL(triggered()), this, SLOT(itPaste()));
    // Update
    if(!ico_t.load(TUIS::icoGet("reload",NULL,true).c_str())) ico_t.load(":/images/reload.png");
    QAction *actUpdate = new QAction(QPixmap::fromImage(ico_t),_("&Refresh"),this);
    actUpdate->setShortcut(Qt::Key_F5);
    actUpdate->setToolTip(_("Refresh current page"));
    actUpdate->setWhatsThis(_("The button for refreshing of the content of the current page."));
    actUpdate->setStatusTip(_("Press for refreshing of the content of the current page."));
    connect(actUpdate, SIGNAL(triggered()), this, SLOT(pageRefresh()));
    // Start of "Auto update"
    if(!ico_t.load(TUIS::icoGet("start",NULL,true).c_str())) ico_t.load(":/images/start.png");
    actStartUpd = new QAction(QPixmap::fromImage(ico_t),_("&Start"),this);
    actStartUpd->setShortcut(Qt::CTRL+Qt::Key_B);
    actStartUpd->setToolTip(_("Start cycled refresh"));
    actStartUpd->setWhatsThis(_("The button for starting of the cycled refresh of the content of the current page."));
    actStartUpd->setStatusTip(_("Press to start the cycled refresh of the content of the current page."));
    connect(actStartUpd, SIGNAL(triggered()), this, SLOT(pageCyclRefrStart()));
    // Stop of "Auto update"
    if(!ico_t.load(TUIS::icoGet("stop",NULL,true).c_str())) ico_t.load(":/images/stop.png");
    actStopUpd = new QAction(QPixmap::fromImage(ico_t),_("&Stop"),this);
    actStopUpd->setShortcut(Qt::CTRL+Qt::Key_E);
    actStopUpd->setToolTip(_("Stop cycled refresh"));
    actStopUpd->setWhatsThis(_("The button for stopping of the cycled refresh of the content of the current page."));
    actStopUpd->setStatusTip(_("Press to stop the cycled refresh of the content of the current page."));
    actStopUpd->setEnabled(false);
    connect(actStopUpd, SIGNAL(triggered()), this, SLOT(pageCyclRefrStop()));
    // About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display program and OpenSCADA information"));
    actAbout->setStatusTip(_("Press to display program and OpenSCADA information"));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    // About Qt
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using Qt information"));
    actQtAbout->setStatusTip(_("Press to get the using Qt information."));
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // QTCfg manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    QAction *actManual = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(mod->modId().c_str()),this);
    actManual->setProperty("doc", "Modules/QTCfg|Modules/QTCfg");
    actManual->setShortcut(Qt::Key_F1);
    actManual->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(mod->modId().c_str()));
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    // OpenSCADA manual index
    QAction *actManualSYS = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(PACKAGE_STRING),this);
    actManualSYS->setProperty("doc", "index|Documents");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(PACKAGE_STRING));
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    // Page help
    actManualPage = new QAction(QPixmap::fromImage(ico_t),_("The page manual"),this);
    actManualPage->setToolTip(_("Selected page manual"));
    actManualPage->setWhatsThis(_("The button for getting the using selected page manual."));
    actManualPage->setStatusTip(_("Press to get the using selected page manual."));
    actManualPage->setEnabled(false);
    connect(actManualPage, SIGNAL(triggered()), this, SLOT(enterManual()));
    // What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setShortcut(Qt::SHIFT+Qt::Key_F1);
    actWhatIs->setToolTip(_("Question about GUI elements"));
    actWhatIs->setWhatsThis(_("The button for requesting the information about user interface elements"));
    actWhatIs->setStatusTip(_("Press for requesting about user interface elements"));
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    //Create menu
    // Create menu "file"
    QMenu *mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actDBLoad);
    mn_file->addAction(actDBSave);
    mn_file->addSeparator( );
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);
    // Create menu "edit"
    QMenu *mn_edit = menuBar()->addMenu(_("&Edit"));
    mn_edit->addAction(actItAdd);
    mn_edit->addAction(actItDel);
    mn_edit->addSeparator( );
    mn_edit->addAction(actItCopy);
    mn_edit->addAction(actItCut);
    mn_edit->addAction(actItPaste);
    // Create menu "view"
    QMenu *mn_view = menuBar()->addMenu(_("&View"));
    mn_view->addAction(actUp);
    mn_view->addAction(actPrev);
    mn_view->addAction(actNext);
    mn_view->addSeparator( );
    mn_view->addAction(actUpdate);
    mn_view->addAction(actStartUpd);
    mn_view->addAction(actStopUpd);
    // Create menu "help"
    QMenu *help = menuBar()->addMenu(_("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);
    help->addAction(actManual);
    help->addAction(actManualSYS);
    help->addAction(actManualPage);
    help->addSeparator();
    help->addAction(actWhatIs);

    //Create tool bars
    // Main tool bar
    QToolBar *toolBar = new QToolBar(_("OpenSCADA toolbar"),this);
    toolBar->setAllowedAreas(Qt::AllToolBarAreas);
    addToolBar(Qt::TopToolBarArea,toolBar);
    toolBar->setMovable(true);
    toolBar->addAction(actDBLoad);
    toolBar->addAction(actDBSave);
    toolBar->addSeparator();
    toolBar->addAction(actUp);
    toolBar->addAction(actPrev);
    toolBar->addAction(actNext);
    toolBar->addSeparator();
    toolBar->addAction(actItAdd);
    toolBar->addAction(actItDel);
    toolBar->addSeparator();
    toolBar->addAction(actItCopy);
    toolBar->addAction(actItCut);
    toolBar->addAction(actItPaste);
    toolBar->addSeparator();
    toolBar->addAction(actUpdate);
    toolBar->addAction(actStartUpd);
    toolBar->addAction(actStopUpd);
    toolBar->addSeparator();
    toolBar->addAction(actManualPage);

    //Init status bar
    connect(statusBar(), SIGNAL(messageChanged(const QString&)), this, SLOT(stMessChanged(const QString&)));
    wUser = new UserStBar(open_user.c_str(), this);
    wUser->setWhatsThis(_("This label displays current user."));
    wUser->setToolTip(_("Field for display of the current user."));
    wUser->setStatusTip(_("Double click to change user."));
    statusBar()->insertPermanentWidget(0, wUser);
    connect(wUser, SIGNAL(userChanged()), this, SLOT(userSel()));

    mStModify = new QLabel(" ",this);
    mStModify->setWhatsThis(_("This label displays the local station modifying."));
    mStModify->setToolTip(_("Field for display of the local station modifying."));
    statusBar()->insertPermanentWidget(0,mStModify);

    if(!ico_t.load(TUIS::icoGet("combar",NULL,true).c_str())) ico_t.load(":/images/combar.png");
    QPushButton *stBt = new QPushButton(QPixmap::fromImage(ico_t),"",this);
    stBt->setWhatsThis(_("This button for call status bar history."));
    stBt->setToolTip(_("Button for call status bar history."));
    stBt->setMaximumSize(QSize(16,18));
    stBt->setFlat(true);
    statusBar()->insertPermanentWidget(0,stBt);
    connect(stBt, SIGNAL(released()), this, SLOT(stHistCall()));

    statusBar()->showMessage(_("Ready"), 2000);

    //Other resources init
    // Create auto update timer
    autoUpdTimer = new QTimer(this);
    connect(autoUpdTimer, SIGNAL(timeout()), SLOT(pageRefresh()));
    // Create end run timer
    endRunTimer  = new QTimer(this);
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);
    // Create Request progress closing timer
    reqPrgrsTimer = new QTimer(this);
    reqPrgrsTimer->setSingleShot(true);
    reqPrgrsTimer->setInterval(500);
    connect(reqPrgrsTimer, SIGNAL(timeout()), SLOT(reqPrgrsSet()));

    menuBar()->setVisible(true);

    // Display root page and init external pages
    initHosts();
    try{ pageDisplay("/"+SYS->id()+mod->startPath()); }
    catch(TError &err) { pageDisplay("/"+SYS->id()); }
}

ConfApp::~ConfApp( )
{
    endRunTimer->stop();
    autoUpdTimer->stop();
    reqPrgrsTimer->stop();

    mod->unregWin(this);

    //Final left hosts' tasks stopping.
    // Wait for a host request finish
    while(inHostReq) qApp->processEvents();

    // Threads delete
    for(map<string, SCADAHost*>::iterator iH = hosts.begin(); iH != hosts.end(); ++iH) delete iH->second;
    hosts.clear();
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
	    int ret = QMessageBox::information(this,_("Changes save"),
		_("Some changes made.\nSave the changes to DB on exit?"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Yes);
	    switch(ret) {
		case QMessageBox::Yes:
		    req.clear()->setName("save")->setAttr("path","/"+SYS->id()+"/%2fobj");
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
	nit->setIcon(0, img.isNull() ? QPixmap() : QPixmap::fromImage(img).scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	if(lnkOK > 0 && !nit->data(0,Qt::UserRole).toBool()) {
	    //? Used for rechange status for fix indicator hide after all childs remove on bad connection
	    nit->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
	    nit->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	    nit->setData(2, Qt::UserRole, brs);
	}
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
    int i_c;
    for(i_c = 0; i_c < pi->childCount(); i_c++) {
	if(fromCur && !curReach) {
	    if(pi->child(i_c) == si) curReach = true;
	    continue;
	}
	if(pi->child(i_c)->text(0).contains(wvl,Qt::CaseInsensitive)) break;
	else {
	    // Get last item from path
	    string itpth, tstr;
	    for(int off = 0; (tstr=TSYS::pathLev(pi->child(i_c)->text(2).toStdString(),0,true,&off)).size(); ) itpth = tstr;
	    if(QString(itpth.c_str()).contains(wvl,Qt::CaseInsensitive)) break;
	}
    }
    if(i_c < pi->childCount()) {
	pi->treeWidget()->setCurrentItem(pi->child(i_c), 0, QItemSelectionModel::Clear|QItemSelectionModel::Select);
	pi->treeWidget()->scrollTo(pi->treeWidget()->currentIndex());
    }
    else if(fromCur) { sl->setModified(true); treeSearch(); }
}

void ConfApp::pageUp( )
{
    size_t i_l = string::npos;
    while(true) {
	i_l = selPath.rfind("/",i_l);
	if(i_l == string::npos || i_l == 0) return;
	if((selPath.size()-i_l) > 1) break;
	i_l--;
    }

    selectPage(selPath.substr(0,i_l));
}

void ConfApp::pagePrev( )
{
    if(!prev.size()) return;
    next.insert(next.begin(), selPath);
    string path = prev[0];
    prev.erase(prev.begin());

    try{ pageDisplay(path); } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::pageNext( )
{
    if(!next.size()) return;
    prev.insert(prev.begin(), selPath);
    string path = next[0];
    next.erase(next.begin());

    try{ pageDisplay(path); } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
}

void ConfApp::itDBLoad( )
{
    XMLNode req("load"); req.setAttr("path",selPath+"/%2fobj");
    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
    pageRefresh();	//Any time but warnings in the deep
}

void ConfApp::itDBSave( )
{
    XMLNode req("save"); req.setAttr("path",selPath+"/%2fobj");
    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
    else pageRefresh();
}

void ConfApp::itAdd( )
{
    if(selPath.empty() || !root->childGet("id","br",true)) return;
    XMLNode *branch = root->childGet("id", "br");

    //Load branches list
    vector<string> brs;
    for(unsigned iB = 0; iB < branch->childSize(); iB++)
	if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR)
	    brs.push_back(branch->childGet(iB)->attr("idSz")+"\n"+branch->childGet(iB)->attr("idm")+"\n"+
			  branch->childGet(iB)->attr("id")+"\n"+branch->childGet(iB)->attr("dscr"));
    if(!brs.size()) { mod->postMess(mod->nodePath().c_str(),_("No one editable container is present."),TUIMod::Info,this); return; }

    ReqIdNameDlg dlg(this, actItAdd->icon(), QString(_("Add item to node: '%1'.")).arg(selPath.c_str()),_("Add node"));
    dlg.setTargets(brs);
    if(dlg.exec() != QDialog::Accepted)	return;

    //Check for already present node
    XMLNode req("get");
    req.setAttr("path",selPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target( ),2,'\n'));
    if(!cntrIfCmd(req))
	for(unsigned i_lel = 0; i_lel < req.childSize(); i_lel++)
	    if((req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->attr("id") == dlg.id().toStdString()) ||
	       (!req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->text() == dlg.id().toStdString()))
	    {
		mod->postMess(mod->nodePath().c_str(), QString(_("Node '%1' is already present.")).arg(dlg.id()).toStdString(), TUIMod::Info, this);
		return;
	    }

    //Send create request
    req.clear()->
	setName("add")->
	setAttr("path",selPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target( ),2,'\n'));
    if(s2i(TSYS::strSepParse(dlg.target(),1,'\n')))
	req.setAttr("id",dlg.id().toStdString())->setText(dlg.name().toStdString());
    else req.setText(dlg.id().toStdString());
    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
    else { treeUpdate(); pageRefresh(); }
}

void ConfApp::itDel( const string &iit )
{
    string rmits = iit, rmit;
    if(iit.empty()) {
	QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
	if(sel_ls.size() <= 1) rmits = selPath;
	else for(int i_el = 0; i_el < sel_ls.size(); i_el++)
	    rmits += sel_ls.at(i_el)->text(2).toStdString() + "\n";
    }
    if(rmits.empty())	return;

    if(iit.empty()) {
	InputDlg dlg(this, actItDel->icon(),
		QString(_("Are you sure of deleting nodes: '%1'?")).arg(rmits.c_str()),_("Delete node"), 0, 0);
	if(dlg.exec() != QDialog::Accepted)	return;
    }

    bool toTreeUpdate = false;
    for(int roff = 0; (rmit=TSYS::strSepParse(rmits,0,'\n',&roff)).size(); ) {
	string t_el, sel_own, sel_el;
	int n_obj = 0;
	for(int off = 0; !(t_el=TSYS::pathLev(rmit,0,true,&off)).empty(); n_obj++)
	{ if(n_obj) sel_own += ("/"+sel_el); sel_el = t_el; }
	if(n_obj > 2) {
	    XMLNode req("info");
	    req.setAttr("path",sel_own+"/%2fbr");
	    if(cntrIfCmd(req) || !req.childGet(0,true)) return;

	    XMLNode *branch = req.childGet(0);
	    for(unsigned iB = 0; iB < branch->childSize(); iB++) {
		string b_id = branch->childGet(iB)->attr("id");
		if(b_id == sel_el.substr(0,b_id.size()) && s2i(branch->childGet(iB)->attr("acs"))&SEC_WR) {
		    bool idm = s2i(branch->childGet(iB)->attr("idm"));
		    req.clear()->setName("del")->setAttr("path",sel_own+"/%2fbr%2f"+b_id);
		    if(idm) req.setAttr("id",sel_el.substr(b_id.size()));
		    else req.setText(sel_el.substr(b_id.size()));
		    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
		    else toTreeUpdate = true;
		    break;
		}
	    }
	}
    }

    if(toTreeUpdate) treeUpdate();
}

void ConfApp::itCut( )
{
    copyBuf = "1";
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() <= 1) copyBuf += selPath;
	else for(int i_el = 0; i_el < sel_ls.size(); i_el++)
	    copyBuf += sel_ls.at(i_el)->text(2).toStdString()+"\n";
    editToolUpdate();
}

void ConfApp::itCopy( )
{
    copyBuf = "0";
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() <= 1) copyBuf += selPath;
    else for(int i_el = 0; i_el < sel_ls.size(); i_el++)
	    copyBuf += sel_ls.at(i_el)->text(2).toStdString()+"\n";
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
	{ if(nSel) sElp += ("/"+sEl); sEl = tEl; }

	if(TSYS::pathLev(copyEl,0) != TSYS::pathLev(toPath,0))
	{ mod->postMess(mod->nodePath().c_str(), _("Copy is impossible."), TUIMod::Error, this); return; }

	vector<string> brs;
	if(copyEl == toPath) {	//For copy into the branch and no select direct the parent node
	    toPath = sElp;
	    parNode.setAttr("path", toPath);
	    if(cntrIfCmd(parNode)) continue;
	    rootW = parNode.childGet(0);
	}
	if(s2i(rootW->attr("acs"))&SEC_WR) brs.push_back(string("-1\n0\n\n")+_("Selected"));

	XMLNode *branch = rootW->childGet("id", "br", true);
	for(unsigned iB = 0; branch && iB < branch->childSize(); iB++)
	    if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR) {
		string gbrId = branch->childGet(iB)->attr("id");
		brs.push_back(branch->childGet(iB)->attr("idSz")+"\n0\n"+gbrId+"\n"+branch->childGet(iB)->attr("dscr"));
		if(sEl.substr(0,gbrId.size()) == gbrId) { brs[brs.size()-1] = brs[brs.size()-1]+"\n1"; bGrp = gbrId; }
	    }

	//Make request dialog
	ReqIdNameDlg dlg(this, actItAdd->icon(), "", _("Move or copy node"));
	dlg.setTargets(brs);
	dlg.setMess(QString(isCut?_("Move node '%1' to '%2'.\n"):_("Copy node '%1' to '%2'.\n")).arg(copyEl.c_str()).arg(toPath.c_str()));
	dlg.setId(sEl.substr(bGrp.size()).c_str());
	if(isMult) {
	    prcReq = new QCheckBox(_("Do not the question anymore."), &dlg);
	    dlg.edLay->addWidget(prcReq, 5, 0, 1, 2);
	}
	if(!prcReqMiss && dlg.exec() != QDialog::Accepted) return;
	if(!prcReqMiss && prcReq && prcReq->checkState() == Qt::Checked) prcReqMiss = true;

	string stat_nm, src_nm, dst_nm;
	off = 0; stat_nm = TSYS::pathLev(copyEl, 0, true, &off);	src_nm = copyEl.substr(off);
	off = 0; stat_nm = TSYS::pathLev(toPath, 0, true, &off);	dst_nm = toPath.substr(off);

	if(s2i(TSYS::strSepParse(dlg.target(),0,'\n')) >= 0) {
	    dst_nm += "/" +TSYS::strSepParse(dlg.target(),2,'\n') + dlg.id().toStdString();
	    // Check for already present node
	    XMLNode req("get");
	    req.setAttr("path", toPath+"/%2fbr%2f"+TSYS::strSepParse(dlg.target(),2,'\n'));
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Error,this); return; }
	    for(unsigned i_lel = 0; i_lel < req.childSize(); i_lel++)
		if((req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->attr("id") == dlg.id().toStdString()) ||
		   (!req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->text() == dlg.id().toStdString()))
		{
		    InputDlg dlg1(this, actItPaste->icon(), QString(_("Node '%1' is already present. Continue?")).arg(dst_nm.c_str()),
					_("Move or copy node"), 0, 0);
		    if(isMult) {
			prcAlrPres = new QCheckBox(_("Do not the question anymore."),&dlg1);
			dlg1.edLay->addWidget(prcAlrPres,5,0,5,1);
		    }
		    if(!prcAlrPresMiss && dlg1.exec() != QDialog::Accepted) return;
		    if(!prcAlrPresMiss && prcAlrPres && prcAlrPres->checkState() == Qt::Checked) prcAlrPresMiss = true;
		    break;
		}
	}

	//Copy visual item
	XMLNode req("copy");
	req.setAttr("path", "/"+stat_nm+"/%2fobj")->setAttr("src", src_nm)->setAttr("dst", dst_nm);
	if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Error,this); return; }

	//Remove source widget
	if(isCut) {
	    itDel(copyEl);
	    if(selPath == copyEl) chSel = "/"+stat_nm+"/"+dst_nm;
	}
    }

    if(isCut) copyBuf = "0";

    treeUpdate();
    if(chSel.size()) selectPage(chSel);
    else pageRefresh();
}

void ConfApp::editToolUpdate( )
{
    actItCut->setEnabled(selPath.size() && root && s2i(root->attr("acs"))&SEC_WR);
    actItCopy->setEnabled(selPath.size());
    actItPaste->setEnabled(false);

    if(TSYS::strParse(copyBuf,1,"\n").empty()) {
	//Src and destination elements calc
	if(copyBuf.size() <= 1 || /*copyBuf.substr(1) == selPath ||*/ TSYS::pathLev(copyBuf.substr(1),0) != TSYS::pathLev(selPath,0))
	    return;
	string sElp, sEl, tEl;
	for(int off = 0; !(tEl=TSYS::pathLev(copyBuf.substr(1),0,true,&off)).empty(); )
	{ sElp += ("/"+sEl); sEl = tEl; }

	if(s2i(root->attr("acs"))&SEC_WR) actItPaste->setEnabled(true);
    }

    XMLNode *branch = root->childGet("id","br",true);
    if(branch)
	for(unsigned iB = 0; iB < branch->childSize(); iB++)
	    if(s2i(branch->childGet(iB)->attr("acs"))&SEC_WR)
	    { actItPaste->setEnabled(true); break; }
}

void ConfApp::treeUpdate( )
{
    for(int i_t = 0; i_t < CtrTree->topLevelItemCount(); i_t++)
	if(CtrTree->topLevelItem(i_t)->isExpanded())
	    viewChildRecArea(CtrTree->topLevelItem(i_t),true);
}

void ConfApp::userSel( )
{
    initHosts();

    pgInfo.setAttr("path", "");

    try { pageDisplay("/"+SYS->id()+mod->startPath()); }
    catch(TError &err) { pageDisplay("/"+SYS->id()); }
}

void ConfApp::pageRefresh( bool tm )
{
    if(tm) {
	if(!actStartUpd->isEnabled())	return;

	autoUpdTimer->setSingleShot(true);
	autoUpdTimer->start(CH_REFR_TM);

	return;
    }

    try {
	//Tree part update
	if(CtrTree->currentItem())
	    viewChildRecArea(CtrTree->currentItem()->parent() ? CtrTree->currentItem()->parent() : CtrTree->currentItem(), true);

	//Same page update
	pageDisplay(selPath);
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::pageCyclRefrStart( )
{
    actStartUpd->setEnabled(false);
    actStopUpd->setEnabled(true);

    autoUpdTimer->setSingleShot(false);
    autoUpdTimer->start(1000);
}

void ConfApp::pageCyclRefrStop( )
{
    actStopUpd->setEnabled(false);
    actStartUpd->setEnabled(true);

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
    InputDlg dlg(this,QIcon(),QString(_("Status bar messages list:")),_("Status messages"),0,0,QDialogButtonBox::Ok);
    QTextBrowser *tb = new QTextBrowser(&dlg);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setVerticalStretch(10);
    tb->setSizePolicy(sizePolicy);
    tb->setMinimumSize(600,300);
    dlg.edLay->addWidget(tb, 0, 0);
    string textv;
    for(int i_e = stMess.size()-1; i_e >=0; i_e--) textv += stMess[i_e]+"\n";
    tb->setPlainText(textv.c_str());
    dlg.exec();
}

void ConfApp::about( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf, sizeof(buf), _(
	"%s v%s.\n%s\nAuthor: %s\nLicense: %s\n\n"
	"%s v%s.\n%s\nLicense: %s\nAuthor: %s\nWeb site: %s"),
	mod->modInfo("Name").c_str(), mod->modInfo("Version").c_str(), mod->modInfo("Description").c_str(),
	mod->modInfo("Author").c_str(), mod->modInfo("License").c_str(),
	PACKAGE_NAME, VERSION, _(PACKAGE_DESCR), PACKAGE_LICENSE, _(PACKAGE_AUTHOR), PACKAGE_SITE);

    QMessageBox::about(this, windowTitle(), buf);
}

void ConfApp::aboutQt( )	{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void ConfApp::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void ConfApp::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString());
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("No the manual '%1' found offline or online!")).arg(sender()->property("doc").toString()));
}

void ConfApp::closeEvent( QCloseEvent* ce )
{
    if(!SYS->stopSignal() && !property("forceClose").toBool() && !mod->endRun() && !exitModifChk()) {
	ce->ignore();
	return;
    }

    //Early hosts' tasks stopping by into the destructor sometime late.
    // Wait for a host request finish
    while(inHostReq) qApp->processEvents();

    // Timers early stop
    endRunTimer->stop();
    autoUpdTimer->stop();
    reqPrgrsTimer->stop();

    // Threads delete
    for(map<string, SCADAHost*>::iterator iH = hosts.begin(); iH != hosts.end(); ++iH) delete iH->second;
    hosts.clear();

    ce->accept();
}

void ConfApp::selectItem( )
{
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() == 1 && selPath != sel_ls.at(0)->text(2).toStdString()) {
	selectPage(sel_ls.at(0)->text(2).toStdString());

	if((sel_ls=CtrTree->selectedItems()).size()) {	//Updating but it can be changed after "selectPage"
	    int saveVl = CtrTree->horizontalScrollBar() ? CtrTree->horizontalScrollBar()->value() : 0;
	    CtrTree->scrollToItem(sel_ls.at(0), QAbstractItemView::EnsureVisible);
	    if(CtrTree->horizontalScrollBar()) CtrTree->horizontalScrollBar()->setValue(saveVl);
	}
    }
}

void ConfApp::selectPage( const string &path )
{
    try {
	//Prev and next
	if(selPath.size())		prev.insert(prev.begin(),selPath);
	if((int)prev.size() >= queSz)	prev.pop_back();
	next.clear();

	//Display page
	pageDisplay(path);
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget )
{
    //For append no named widgets
    QHBoxLayout *l_hbox = NULL;
    int		i_area = 0;	//Areas counter
    int		l_pos = 0;
    int		rez;

    //View title name
    if(a_path == "/") {
	// Set node icon
	if(node.childGet("id","ico",true)) {
	    XMLNode req("get");
	    req.setAttr("path", TSYS::strEncode(a_path+"ico",TSYS::PathEl));
	    if((rez=cntrIfCmd(req)) > 0) mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this);
	    else if(rez == 0) {
		string simg = TSYS::strDecode(req.text(), TSYS::base64);
		QImage img;
		if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
		    titleIco->setPixmap(QPixmap::fromImage(img.scaled(32,32,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
		else titleIco->clear();
	    }
	} else titleIco->clear();

	// Set title
	titleLab->setText((string("<p align='center'><i><b>")+TSYS::strEncode(node.attr("dscr"),TSYS::Html)+"</b></i></p>").c_str());

	// Delete tabs of deleted areas
	bool actRemoved = false;
	for(int i_tbs = 0; i_tbs < tabs->count(); i_tbs++) {
	    unsigned i_cf;
	    for(i_cf = 0; i_cf < node.childSize(); i_cf++)
		if(node.childGet(i_cf)->name() == "area" && tabs->tabText(i_tbs) == node.childGet(i_cf)->attr("dscr").c_str())
		    break;
	    if(i_cf >= node.childSize()) {
		if(tabs->currentIndex() == i_tbs) actRemoved = true;
		tabs->widget(i_tbs)->deleteLater();
		tabs->removeTab(i_tbs);
		i_tbs--;
	    }
	}
	// Add new tabs
	for(unsigned i_cf = 0; i_cf < node.childSize(); i_cf++) {
	    XMLNode &t_s = *node.childGet(i_cf);
	    if(t_s.name() != "area")	continue;

	    int i_tbs;
	    for(i_tbs = 0; i_tbs < tabs->count(); i_tbs++)
		if(tabs->tabText(i_tbs) == t_s.attr("dscr").c_str())
		    break;
	    if(i_tbs >= tabs->count()) {
		QScrollArea *scrl = new QScrollArea();
		tabs->insertTab(i_area, scrl, t_s.attr("dscr").c_str());
		t_s.setAttr("qview", "0");
	    }

	    //  Find and prepare current tab
	    if(actRemoved && i_area == 0)	tabs->setCurrentIndex(0);
	    if(tabs->currentIndex() == i_area) {
		if(!s2i(t_s.attr("qview"))) {
		    QScrollArea *scrl = (QScrollArea*)tabs->widget(i_area);
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

		// Elements of scalable by vertical get and their grow up to the scroll appear into the container
		if(!s2i(genReqs.attr("fillMode"))) {
		    qApp->processEvents();
		    QScrollArea *scrl = (QScrollArea*)tabs->widget(i_area);
		    QWidget *lastW = qobject_cast<QWidget*>(scrl->widget()->children().last());
		    int sclFitSz = lastW ? (scrl->maximumViewportSize().height() - (lastW->y()+lastW->height()) - 10): 0;
		    QList<TextEdit*> texts = scrl->findChildren<TextEdit*>();
		    QList<QTableWidget*> tbls = scrl->findChildren<QTableWidget*>();
		    QList<ListView*> lsts = scrl->findChildren<ListView*>();
		    for(int fitStp = vmax(5, sclFitSz/(10*vmax(1,texts.length()+tbls.length()+lsts.length()))), iScN = 0; sclFitSz > fitStp; ) {
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
	    i_area++;
	}
	return;
    }
    //Read node tree and create widgets
    else for(unsigned i_cf = 0; i_cf < node.childSize(); i_cf++) {
	XMLNode &t_s = *node.childGet(i_cf);

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
		w_lay->setMargin(3);
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
		lstbox = new ListView(widget);
		lstbox->setStatusTip((selPath+"/"+br_path).c_str());
		lstbox->setObjectName(br_path.c_str());
		lstbox->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(lstbox, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(listBoxPopup()));

		//  Go branche signal connect
		if(t_s.attr("tp") == "br")
		    connect(lstbox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listBoxGo(QListWidgetItem*)));

		lstbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum/*Expanding*/));

		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->setAlignment(Qt::AlignLeft);
		lab = new QLabel(widget);
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

	    //lstbox->setMinimumHeight(0);	//Fit reset

	    //  Fill list
	    lab->setText((t_s.attr("dscr")+":").c_str());
	    string helpVl = t_s.attr("help");
	    lstbox->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
	    if(helpVl.size() > TOOL_TIP_LIM) lstbox->setWhatsThis(helpVl.c_str());
	    XMLNode req("get");
	    req.setAttr("path",br_path);
	    if((rez=cntrIfCmd(req)) > 0) mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this);
	    else if(rez == 0)
		for(unsigned i_el = 0; i_el < req.childSize(); i_el++)
		    if(req.childGet(i_el)->name() == "el")
			lstbox->addItem(req.childGet(i_el)->text().c_str());
	}
	// View table elements
	else if(t_s.name() == "table") {
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"), TSYS::PathEl);

	    //QLabel *lab;
	    CfgTable *tbl;

	    if(widget) {
		tbl = new CfgTable(widget);
		tbl->setItemDelegate(new TableDelegate);
		tbl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		tbl->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		//tbl->setTextElideMode(Qt::ElideNone);
		tbl->setStatusTip((selPath+"/"+br_path).c_str());
		tbl->setObjectName(br_path.c_str());
		QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Minimum/*Expanding*/);
		sp.setVerticalStretch(1);
		tbl->setSizePolicy(sp);
		tbl->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(tbl, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(tablePopup(const QPoint&)));
		connect(tbl, SIGNAL(cellChanged(int,int)), this, SLOT(tableSet(int,int)));
		connect(tbl->horizontalHeader(), SIGNAL(sectionClicked(int)/*sectionResized(int,int,int)*/), tbl, SLOT(resizeRowsToContentsLim()));

		tbl->setMinimumHeight(150); //tbl->setMaximumHeight(500);

		widget->layout()->addWidget(new QLabel((t_s.attr("dscr")+":").c_str(),widget));
		widget->layout()->addWidget(tbl);

		//t_s.attr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_tbl",TSYS::addr2str(tbl));
	    }
	    else {
		//lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		tbl = (CfgTable *)TSYS::str2addr(t_s.attr("addr_tbl"));
	    }

	    tbl->setMinimumHeight(0);	//Fit reset

	    //  Fill the table
	    string helpVl = t_s.attr("help");
	    tbl->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
	    if(helpVl.size() > TOOL_TIP_LIM) tbl->setWhatsThis(helpVl.c_str());
	    XMLNode req("get"); req.setAttr("path",br_path);
	    if((rez=cntrIfCmd(req)) > 0) mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this);
	    else if(rez <= 0) {
		//   Collumns adjusting flag
		bool adjCol = widget || !tbl->rowCount();
		bool adjRow = false;

		//   Copy values to info tree
		for(unsigned i_col = 0; i_col < req.childSize(); i_col++) {
		    XMLNode *t_lsel = req.childGet(i_col);
		    XMLNode *t_linf = t_s.childGet("id",t_lsel->attr("id"),true);
		    if(!t_linf) { t_linf = t_s.childIns(i_col); *t_linf = *t_lsel; adjCol = true; }
		    t_linf->childClear();
		    for(unsigned i_rw = 0; i_rw < t_lsel->childSize(); i_rw++)
			*t_linf->childAdd() = *t_lsel->childGet(i_rw);
		}

		//   Calc rows and columns
		int n_col = t_s.childSize();
		int n_row = (n_col)?t_s.childGet(0)->childSize():0;

		if(rez == 0 && tbl->columnCount() != n_col)	tbl->setColumnCount(n_col);
		if(rez == 0 && tbl->rowCount() != n_row)	{ tbl->setRowCount(n_row); adjRow = true; }

		for(unsigned i_lst = 0; i_lst < t_s.childSize(); i_lst++) {
		    XMLNode *t_linf = t_s.childGet(i_lst);
		    if(!t_linf) continue;
		    bool c_wr = wr && (s2i(t_linf->attr("acs"))&SEC_WR);

		    QTableWidgetItem *thd_it = tbl->horizontalHeaderItem(i_lst);
		    if(!thd_it) {
			thd_it = new QTableWidgetItem("");
			tbl->setHorizontalHeaderItem(i_lst,thd_it);
		    }
		    thd_it->setData(Qt::DisplayRole,t_linf->attr("dscr").c_str());

		    //   Once list process
		    QStringList elms, elmi;
		    if(t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed") {
			if(t_linf->attr("select").empty()) {
			    elms = QString(t_linf->attr("sel_list").c_str()).split(";");
			    elmi = QString(t_linf->attr("sel_id").c_str()).split(";");
			}
			else {
			    XMLNode x_lst("get");
			    x_lst.setAttr("path",TSYS::strEncode(t_linf->attr("select"),TSYS::PathEl));
			    if((rez=cntrIfCmd(x_lst)) > 0) mod->postMess(x_lst.attr("mcat"), x_lst.text(), TUIMod::Error, this);
			    else if(rez == 0)
				for(unsigned i_ls = 0; i_ls < x_lst.childSize(); i_ls++) {
				    if(!x_lst.childGet(i_ls)->attr("id").empty())
					elmi += x_lst.childGet(i_ls)->attr("id").c_str();
				    elms += x_lst.childGet(i_ls)->text().c_str();
				}
			}
		    }

		    //   Set elements
		    tblInit = true;
		    for(unsigned i_el = 0; i_el < t_linf->childSize(); i_el++) {
			thd_it = tbl->item(i_el,i_lst);
			if(!thd_it) {
			    thd_it = new QTableWidgetItem("");
			    tbl->setItem(i_el,i_lst,thd_it);
			}

			//   Set element
			if(t_linf->attr("tp") == "bool") thd_it->setData(Qt::DisplayRole,(bool)s2i(t_linf->childGet(i_el)->text()));
			else if(t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed")
			{
			    int sel_n;
			    for(sel_n = 0; sel_n < elms.size(); sel_n++)
				if((elms.size() == elmi.size() && elmi[sel_n] == t_linf->childGet(i_el)->text().c_str()) ||
					(elms.size() != elmi.size() && elms[sel_n] == t_linf->childGet(i_el)->text().c_str()))
				    break;
			    if(sel_n >= elms.size()) {
				elms.insert(elms.begin(),t_linf->childGet(i_el)->text().c_str());
				sel_n = 0;
			    }

			    thd_it->setData(Qt::DisplayRole,elms.at(sel_n));
			    thd_it->setData(Qt::UserRole,elms);
			}
			else if(t_linf->attr("tp") == "time")
			    thd_it->setData(Qt::DisplayRole, atm2s(s2i(t_linf->childGet(i_el)->text()),"%d-%m-%Y %H:%M:%S").c_str());
			else thd_it->setData(Qt::DisplayRole, getPrintVal(t_linf->childGet(i_el)->text()).c_str());

			//   Set access
			if(!c_wr) thd_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
			else thd_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable);
		    }
		}
		if((adjCol || adjRow) && tbl->columnCount()) {
		    tbl->resizeColumnsToContents();

		    int averWdth = tbl->size().width()/tbl->columnCount();
		    int fullColsWdth = 0, niceForceColsWdth = 0, busyCols = 0;
		    //   Count width params
		    for(int i_c = 0; i_c < tbl->columnCount(); i_c++) {
			fullColsWdth += tbl->columnWidth(i_c);
			if(tbl->columnWidth(i_c) <= averWdth)	niceForceColsWdth += tbl->columnWidth(i_c);
			else busyCols++;
		    }
		    //   Set busyCols
		    if(fullColsWdth > tbl->size().width() && busyCols) {
			int busyColsWdth = (tbl->size().width()-niceForceColsWdth)/busyCols;
			for(int i_c = 0; i_c < tbl->columnCount(); i_c++)
			    if(tbl->columnWidth(i_c) > averWdth && tbl->columnWidth(i_c) > busyColsWdth)
				tbl->setColumnWidth(i_c, busyColsWdth);
		    }

		    tbl->resizeRowsToContentsLim();
		}

		tblInit = false;
	    }
	}
	// View images
	else if(t_s.name() == "img") {
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

	    QLabel *lab;
	    ImgView *img;

	    if(widget) {
		img = new ImgView(widget,0,s2i(t_s.attr("h_sz")),s2i(t_s.attr("v_sz")));
		img->setObjectName(br_path.c_str());
		img->setStatusTip((selPath+"/"+br_path).c_str());
		img->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
		img->setMinimumSize(200,200);
		img->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(img, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(imgPopup(const QPoint&)));

		int vsz = s2i(t_s.attr("v_sz"));
		QBoxLayout *box = new QBoxLayout((vsz&&vsz<70)?QBoxLayout::LeftToRight:QBoxLayout::TopToBottom);
		lab = new QLabel(widget);
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
	    string helpVl = t_s.attr("help");
	    img->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
	    if(helpVl.size() > TOOL_TIP_LIM) img->setWhatsThis(helpVl.c_str());

	    XMLNode req("get");
	    req.setAttr("path",br_path);
	    if((rez=cntrIfCmd(req)) > 0) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	    else if(rez == 0) img->setImage(TSYS::strDecode(req.text(),TSYS::base64));
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
		    button->setStatusTip((selPath+"/"+br_path).c_str());
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
	    for(unsigned i_cf = 0; i_cf < t_s.childSize(); i_cf++) {
		XMLNode &t_scm = *t_s.childGet(i_cf);
		if(t_scm.name() == "fld") basicFields(t_scm, a_path+t_s.attr("id")+'/', comm_pan, true, &c_hbox, c_pos, true);
	    }

	    //  Fill command
	    button->setText(t_s.attr("dscr").c_str());
	    string helpVl = t_s.attr("help");
	    button->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
	    if(helpVl.size() > TOOL_TIP_LIM) button->setWhatsThis(helpVl.c_str());
	}
    }
}

void ConfApp::basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool comm )
{
    int rezReq = 0;
    string br_path = TSYS::strEncode(string((comm)?"b":"")+a_path+t_s.attr("id"),TSYS::PathEl);

    XMLNode data_req("get");
    if(!comm) {
	data_req.setAttr("path",br_path);
	if((rezReq=cntrIfCmd(data_req)) > 0) {
	    mod->postMess(data_req.attr("mcat"),data_req.text(),TUIMod::Error,this);
	    data_req.setText("");
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
		val_r->setStatusTip((selPath+"/"+br_path).c_str());
		//QSizePolicy sp(QSizePolicy::Ignored/*Expanding*/, QSizePolicy::Preferred);
		//sp.setHorizontalStretch(1);
		//val_r->setSizePolicy(sp);
	    }
	    else {
		val_w = new QComboBox(widget);
		val_w->setMinimumSize(100, 0);
		val_w->setObjectName(br_path.c_str());
		val_w->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		val_w->setStatusTip((selPath+"/"+br_path).c_str());
		val_w->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));
		connect(val_w, SIGNAL(activated(const QString&)), this, SLOT(combBoxActivate(const QString&)));
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

	// Fill combo
	if(lab) lab->setText((t_s.attr("dscr")+":").c_str());
	if(val_w || val_r) {
	    string helpVl = t_s.attr("help");
	    (val_w?(QWidget*)val_w:(QWidget*)val_r)->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
	    if(helpVl.size() > TOOL_TIP_LIM) (val_w?(QWidget*)val_w:(QWidget*)val_r)->setWhatsThis(helpVl.c_str());
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
		    for(unsigned i_el = 0; i_el < x_lst.childSize(); i_el++) {
			if(x_lst.childGet(i_el)->name() != "el") continue;
			if(val_w)	val_w->insertItem(c_el++, x_lst.childGet(i_el)->text().c_str());
			bool ind_ok = x_lst.childGet(i_el)->attr("id").size();	//Index present
			if((ind_ok && x_lst.childGet(i_el)->attr("id") == data_req.text()) ||
			    (!ind_ok && x_lst.childGet(i_el)->text() == data_req.text()))
			{
			    sel_ok = true;
			    if(val_w)	val_w->setCurrentIndex(c_el-1);
			    if(val_r)	val_r->setText((string("<b>")+TSYS::strEncode(x_lst.childGet(i_el)->text(),TSYS::Html)+"</b>").c_str());
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
		    val_r->setStatusTip((selPath+"/"+br_path).c_str());
		    //QSizePolicy sp(QSizePolicy::Ignored/*Expanding*/, QSizePolicy::Preferred);
		    //sp.setHorizontalStretch(1);
		    //val_r->setSizePolicy(sp);
		}
		// View edit
		else {
		    val_w = new QCheckBox(widget);
		    val_w->setObjectName(br_path.c_str());
		    val_w->setStatusTip((selPath+"/"+br_path).c_str());
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
		string helpVl = t_s.attr("help");
		val_w->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		if(helpVl.size() > TOOL_TIP_LIM) val_w->setWhatsThis(helpVl.c_str());
		val_w->blockSignals(true);
		if(data_req.text() == "<EVAL>") val_w->setCheckState(Qt::PartiallyChecked);
		else if(s2i(data_req.text())) val_w->setCheckState(Qt::Checked);
		else val_w->setCheckState(Qt::Unchecked);
		val_w->blockSignals(false);
	    }
	    if(val_r && rezReq >= 0) {
		string helpVl = t_s.attr("help");
		val_r->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		if(helpVl.size() > TOOL_TIP_LIM) val_r->setWhatsThis(helpVl.c_str());
		val_r->setText((string("<b>")+TSYS::strEncode((data_req.text() == "<EVAL>")?_("<EVAL>"):
		    (s2i(data_req.text())?_("On"):_("Off")),TSYS::Html)+"</b>").c_str());
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
		edit->setStatusTip((selPath+"/"+br_path).c_str());
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

	    edit->setMinimumHeight(0);	//Fit reset

	    // Fill Edit
	    if(lab)	lab->setText((t_s.attr("dscr")+":").c_str());
	    if(edit && !edit->isChanged()) {
		if(rezReq >= 0) {
		    string helpVl = t_s.attr("help");
		    edit->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		    if(helpVl.size() > TOOL_TIP_LIM) edit->setWhatsThis(helpVl.c_str());
		    edit->setText(getPrintVal(data_req.text()).c_str());
		}

		//Request syntax higlihgt
		if(s2i(t_s.attr("SnthHgl"))) {
		    XMLNode hgl_req("SnthHgl");
		    hgl_req.setAttr("path",br_path);
		    if(!cntrIfCmd(hgl_req)) edit->setSnthHgl(hgl_req);
		}
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
		    val_r->setStatusTip((selPath+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		    sp.setHorizontalStretch(1);
		    val_r->setSizePolicy(sp);
		    val_r->setAlignment(Qt::AlignVCenter);
		    val_r->setWordWrap(true);
		}
		else {
		    val_w = new LineEdit(widget, LineEdit::DateTime, comm);
		    val_w->setObjectName(br_path.c_str());
		    val_w->setStatusTip((selPath+"/"+br_path).c_str());
		    val_w->setCfg("dd-MM-yyyy hh:mm:ss");
		    connect(val_w, SIGNAL(valChanged(const QString&)), this, SLOT(editChange(const QString&)));
		    connect(val_w, SIGNAL(apply()), this, SLOT(applyButton()));
		    connect(val_w, SIGNAL(cancel()), this, SLOT(cancelButton()));
		}

		// Check use label
		if(t_s.attr("dscr").size()) {
		    lab = new QLabel(widget);
		    lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    (*l_hbox)->insertWidget(l_pos++, lab);
		    if(val_w)	(*l_hbox)->insertWidget(l_pos++, val_w);
		    if(val_r) {
			(*l_hbox)->insertWidget( l_pos++, val_r );
			lab->setAlignment( Qt::AlignTop );
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
		dtm.setTime_t(tm_t);
		string helpVl = t_s.attr("help");
		val_r->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		if(helpVl.size() > TOOL_TIP_LIM) val_r->setWhatsThis(helpVl.c_str());
		val_r->setText( "<b>"+dtm.toString("dd.MM.yyyy hh:mm:ss")+"</b>" );
	    }
	    if(val_w && rezReq >= 0 && !val_w->isEdited()) {
		string helpVl = t_s.attr("help");
		val_w->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		if(helpVl.size() > TOOL_TIP_LIM) val_w->setWhatsThis(helpVl.c_str());
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
		    val_r->setStatusTip((selPath+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Ignored/*Expanding*/, QSizePolicy::Preferred);
		    sp.setHorizontalStretch(1);
		    val_r->setSizePolicy(sp);
		    val_r->setAlignment(Qt::AlignVCenter);
		    val_r->setWordWrap(true);
		    //val_r->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		}
		// View edit
		else {
		    val_w = new LineEdit(widget, LineEdit::Text, comm);
		    val_w->setObjectName(br_path.c_str());
		    val_w->setStatusTip((selPath+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
		    sp.setHorizontalStretch(1);
		    val_w->setSizePolicy(sp);
		    connect(val_w, SIGNAL(valChanged(const QString&)), this, SLOT(editChange(const QString&)));
		    connect(val_w, SIGNAL(apply()), this, SLOT(applyButton()));
		    connect(val_w, SIGNAL(cancel()), this, SLOT(cancelButton()));

		    //  addon parameters
		    string tp = t_s.attr("tp");
		    if(t_s.attr("dest") == "sel_ed") {
			val_w->setMinimumSize(100, 0);
			val_w->setType(LineEdit::Combo);
		    }
		    else if(tp == "dec" || tp == "hex" || tp == "oct") {
			val_w->setFixedWidth(QFontMetrics(val_w->workWdg()->font()).width("0000000000")+30);
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
			val_w->setFixedWidth(QFontMetrics(val_w->workWdg()->font()).width("3.14159265e123")+30);
			//val_w->setType(LineEdit::Text);
#if QT_VERSION < 0x050000
			QDoubleValidator *dv = new QDoubleValidator(val_w->workWdg());
			dv->setNotation(QDoubleValidator::ScientificNotation);
			((QLineEdit*)val_w->workWdg())->setValidator(dv);
#endif
			/*QString	max = t_s.attr("max").empty() ? "9999999999" : t_s.attr("max").c_str();
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
			lab->setAlignment(Qt::AlignTop);
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
	    string sval = getPrintVal(data_req.text());
	    if(t_s.attr("tpCh") == "hex" || (t_s.attr("tpCh").empty() && t_s.attr("tp") == "hex"))
		sval = "0x" + QString::number(s2ll(data_req.text()),16).toUpper().toStdString();
	    else if(t_s.attr("tpCh") == "oct" || (t_s.attr("tpCh").empty() && t_s.attr("tp") == "oct"))
		sval = "0" + QString::number(s2ll(data_req.text()),8).toStdString();

	    if(lab)	lab->setText((t_s.attr("dscr")+":").c_str());
	    if(val_r) {
		string helpVl = t_s.attr("help");
		val_r->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		if(helpVl.size() > TOOL_TIP_LIM) val_r->setWhatsThis(helpVl.c_str());
		val_r->setText((string("<b>")+TSYS::strEncode(sval,TSYS::Html)+"</b>").c_str());
	    }
	    if(val_w && !val_w->isEdited()) {
		if(rezReq >= 0) {
		    string helpVl = t_s.attr("help");
		    val_w->setToolTip((TSYS::strMess(TOOL_TIP_LIM,"%s",helpVl.c_str())+((helpVl.size()>TOOL_TIP_LIM)?TOOL_TIP_ADD:"")).c_str());
		    if(helpVl.size() > TOOL_TIP_LIM) val_w->setWhatsThis(helpVl.c_str());
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
			    for(unsigned i_el = 0; i_el < x_lst.childSize(); i_el++)
				if(x_lst.childGet(i_el)->name() == "el")
				    cfg_vls += x_lst.childGet(i_el)->text()+"\n";
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
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::pageDisplay( const string &path )
{
    if(pgDisplay) return;
    pgDisplay = true;

    try {
    //Chek Up
    actUp->setEnabled(path.rfind("/") != string::npos && path.rfind("/") != 0);

    //Check Prev and Next
    actPrev->setEnabled(prev.size());
    actNext->setEnabled(next.size());

    if(path != pgInfo.attr("path")) {
	// Stop refresh
	pageCyclRefrStop();

	// Check for no apply editable widgets
	vector<QWidget*> prcW;
	QList<LineEdit*> lines = tabs->findChildren<LineEdit*>();
	for(int iIt = 0; iIt < lines.size(); ++iIt)
	    if(lines[iIt]->isEdited()) prcW.push_back(lines[iIt]);
	QList<TextEdit*> texts = tabs->findChildren<TextEdit*>();
	for(int iIt = 0; iIt < texts.size(); ++iIt)
	    if(texts[iIt]->isChanged()) prcW.push_back(texts[iIt]);
	if(prcW.size() && QMessageBox::information(this,_("The changes apply"),_("You don't apply some changes!\nDo you want its apply now or lost?"),
		QMessageBox::Apply|QMessageBox::Cancel,QMessageBox::Apply) == QMessageBox::Apply)
	    for(vector<QWidget*>::iterator iW = prcW.begin(); iW != prcW.end(); ++iW) applyButton(*iW);

	// Request new page tree
	XMLNode n_node("info");
	n_node.setAttr("path", path);
	if(cntrIfCmd(n_node))		throw TError(s2i(n_node.attr("rez")), n_node.attr("mcat").c_str(), "%s", n_node.text().c_str());
	if(!n_node.childGet(0,true))	throw TError(mod->nodePath().c_str(), _("Broken info respond - no a child."));

	selPath = path;
	pgInfo = n_node;
	root = pgInfo.childGet(0);

	actManualPage->setEnabled(root->attr("doc").size());
	actManualPage->setProperty("doc", root->attr("doc").c_str());
    }
    else {
	// Check the new node structure and the old node
	XMLNode n_node("info");
	n_node.setAttr("path",selPath);
	if(cntrIfCmd(n_node)) { throw TError(s2i(n_node.attr("rez")), n_node.attr("mcat").c_str(), "%s", n_node.text().c_str()); }
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
    if(cntrIfCmd(req) > 0) mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this);
    else if(s2i(req.text()))	{ actDBLoad->setEnabled(true); actDBSave->setEnabled(true); }

    //Stop complex request forming
    if(genReqs.attr("fillMode") == "1") {
	genReqs.attrDel("fillMode");
	if(cntrIfCmd(genReqs)) {
	    mod->postMess(genReqs.attr("mcat"), genReqs.text(), TUIMod::Error, this);
	    genReqs.clear();
	    pgDisplay = false;
	    return;
	}
	goto loadGenReqDate;
    }
    else genReqs.clear();

    //Local station modifying check
    mStModify->setText(" ");
    req.setAttr("path","/"+SYS->id()+"/%2fobj");
    if(!cntrIfCmd(req) && s2i(req.text()))	mStModify->setText("*");

    //Edit tools update
    editToolUpdate();

    pgDisplay = false;
    } catch(TError) { pgDisplay = false; throw; }
}

bool ConfApp::upStruct( XMLNode &w_nd, const XMLNode &n_nd )
{
    bool str_ch = false;

    //Check access
    if(w_nd.attr("acs") != n_nd.attr("acs")) str_ch = true;

    //Scan deleted nodes
    for(unsigned i_w = 0, i_n; i_w < w_nd.childSize(); ) {
	for(i_n = 0; i_n < n_nd.childSize(); i_n++)
	    if(w_nd.childGet(i_w)->name() == n_nd.childGet(i_n)->name() &&
		    w_nd.childGet(i_w)->attr("id") == n_nd.childGet(i_n)->attr("id"))
		break;
	if(i_n >= n_nd.childSize()) {
	    w_nd.childDel(i_w);
	    if(w_nd.name() != "table" && w_nd.name() != "list" && w_nd.name() != "oscada_cntr") str_ch = true;
	    continue;
	}
	i_w++;
    }

    //Scan for new nodes and check present nodes
    for(unsigned i_n = 0, i_w; i_n < n_nd.childSize(); i_n++) {
	for(i_w = 0; i_w < w_nd.childSize(); i_w++)
	    if(w_nd.childGet(i_w)->name() == n_nd.childGet(i_n)->name() &&
		    w_nd.childGet(i_w)->attr("id") == n_nd.childGet(i_n)->attr("id"))
		break;
	if(i_w >= w_nd.childSize()) {
	    // Add node
	    *w_nd.childIns(i_n) = *n_nd.childGet(i_n);
	    str_ch = true;
	    i_w = i_n;
	}
	else {
	    // Check present node
	    if(upStruct(*w_nd.childGet(i_w),*n_nd.childGet(i_n))) str_ch = true;
	    if(str_ch && w_nd.name() == "oscada_cntr") {
		w_nd.childGet(i_w)->setAttr("qview","0");
		str_ch = false;
		continue;
	    }
	}

	//Check of the description present
	if((bool)w_nd.childGet(i_w)->attr("dscr").size() ^ (bool)n_nd.childGet(i_n)->attr("dscr").size())
	    str_ch = true;

	//Check base fields destination change
	if(w_nd.childGet(i_w)->name() == "fld" &&
	    (w_nd.childGet(i_w)->attr("dest") != n_nd.childGet(i_n)->attr("dest") ||
	     w_nd.childGet(i_w)->attr("tp") != n_nd.childGet(i_n)->attr("tp")))
	{
	    w_nd.childGet(i_w)->setAttr("dest","");
	    w_nd.childGet(i_w)->setAttr("tp","");
	    str_ch = true;
	}

	//Sync node parameters (text and attributes)
	w_nd.childGet(i_w)->setText(n_nd.childGet(i_n)->text());
	vector<string> ls;
	n_nd.childGet(i_n)->attrList(ls);
	for(unsigned i_a = 0; i_a < ls.size(); i_a++)
	    w_nd.childGet(i_w)->setAttr(ls[i_a],n_nd.childGet(i_n)->attr(ls[i_a]));
    }

    return str_ch;
}

void ConfApp::onItem( QTreeWidgetItem * i )	{ statusBar()->showMessage(i->text(2), 10000); }

void ConfApp::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    try {
	if(lview && lview->currentItem() && lview->currentItem()->text(2)[0] != '*') {
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
	}
	//Main action add
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("reload",NULL,true).c_str())) ico_t.load(":/images/reload.png");
	QAction *actRemHostUp = new QAction(QPixmap::fromImage(ico_t), _("Refresh items tree"), this);
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
	//Add and update present
	for(int i_g = 0; i_g < grps.size(); i_g++) {
	    bool grpChCnt = s2i(TSYS::strSepParse(grps[i_g].toStdString(),0,'\n'));
	    string grpId = TSYS::strSepParse(grps[i_g].toStdString(),1,'\n');
	    string grpDscr = TSYS::strSepParse(grps[i_g].toStdString(),2,'\n');
	    QTreeWidgetItem *it = NULL;
	    //Search present item
	    if(upTree)
		for(int i_it = 0; i_it < i->childCount(); i_it++)
		    if(i->child(i_it)->text(2) == ("*"+grpId).c_str())
		    { it = i->child(i_it); break; }
	    if(!it) it = new QTreeWidgetItem(i);
	    it->setText(0,(grpDscr+":").c_str());
	    it->setText(1,grpDscr.c_str());
	    it->setText(2,("*"+grpId).c_str());
	    QStringList it_grp; it_grp.push_back(grps[i_g]);
	    it->setData(2,Qt::UserRole,it_grp);
	    it->setFlags(Qt::ItemIsEnabled);
	    it->setChildIndicatorPolicy(grpChCnt?QTreeWidgetItem::ShowIndicator:QTreeWidgetItem::DontShowIndicator);
	    QFont fnt = it->font(0);
	    fnt.setItalic(true);
	    it->setFont(0,fnt);
	    // Next node for update
	    if(upTree && it->isExpanded()) viewChildRecArea(it,upTree);
	}
	//Delete no present
	for(int i_it = 0, i_g = 0; upTree && i_it < i->childCount(); i_it++) {
	    for(i_g = 0; i_g < grps.size(); i_g++)
		if(i->child(i_it)->text(2) == ("*"+TSYS::strSepParse(grps[i_g].toStdString(),1,'\n')).c_str())
		    break;
	    if(i_g >= grps.size()) { delete i->takeChild(i_it); i_it--; }
	}
    }
    else {
	string path = i->text(2).toStdString();
	if(path[0]=='*') path = i->parent()->text(2).toStdString();
	string grpId = TSYS::strSepParse(grps[0].toStdString(),1,'\n');
	string grpDscr = TSYS::strSepParse(grps[0].toStdString(),2,'\n');
	XMLNode req("chlds");
	req.setAttr("path",path+"/%2fobj")->setAttr("grp",grpId);
	if(cntrIfCmd(req)) {
	    //if(s2i(req.attr("rez")) == 10) initHosts();
	    mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this);
	    return;
	}
	//Add and update present
	for(unsigned i_e = 0; i_e < req.childSize(); i_e++) {
	    XMLNode *chEl = req.childGet(i_e);
	    // Prepare branch patch
	    string br_path = grpId;
	    if(chEl->attr("id").size()) br_path.append(chEl->attr("id")); else br_path.append(chEl->text());
	    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
	    // Find item
	    QTreeWidgetItem *it = NULL;
	    if(upTree)
		for(int i_it = 0; i_it < i->childCount(); i_it++)
		    if(i->child(i_it)->text(2) == (path+"/"+br_path).c_str())
		    { it = i->child(i_it); break; }
	    if(!it) it = new QTreeWidgetItem(i);
	    it->setText(0, chEl->text().c_str());
	    it->setText(1, grpDscr.c_str());
	    it->setText(2, (path+"/"+br_path).c_str());
	    // Set icon
	    XMLNode *chIco = chEl->childGet("ico",0,true);
	    if(chIco) {
		string simg = TSYS::strDecode(chIco->text(),TSYS::base64);
		QImage img;
		if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
		    it->setIcon(0,QPixmap::fromImage(img).scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	    }
	    // Set groups
	    QStringList it_grp;
	    for(unsigned i_g = 0; i_g < chEl->childSize(); i_g++)
		if(chEl->childGet(i_g)->name() == "grp")
		    it_grp.push_back((chEl->childGet(i_g)->attr("chPresent")+"\n"+chEl->childGet(i_g)->attr("id")+"\n"+chEl->childGet(i_g)->attr("dscr")).c_str());
	    it->setData(2,Qt::UserRole,it_grp);
	    // Check for childs present
	    bool grpChCnt = it_grp.size() && (it_grp.size()>1 || s2i(TSYS::strSepParse(it_grp[0].toStdString(),0,'\n')));
	    it->setChildIndicatorPolicy(grpChCnt?QTreeWidgetItem::ShowIndicator:QTreeWidgetItem::DontShowIndicator);
	    // Next node for update
	    if(upTree && it->isExpanded()) viewChildRecArea(it,upTree);
	}
	//Delete no present
	if(upTree) {
	    //CtrTree->blockSignals(true);
	    for(unsigned i_it = 0, i_e; i_it < (unsigned)i->childCount(); ) {
		for(i_e = 0; i_e < req.childSize(); i_e++) {
		    // Prepare branch patch
		    string br_path = grpId;
		    if(req.childGet(i_e)->attr("id").size()) br_path.append(req.childGet(i_e)->attr("id"));
		    else br_path.append(req.childGet(i_e)->text());
		    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
		    if(i->child(i_it)->text(2) == (path+"/"+br_path).c_str())	break;
		}
		if(i_e >= req.childSize()) { delete i->takeChild(i_it); continue; }
		i_it++;
	    }
	    //CtrTree->blockSignals(false);
	}
    }

    if(mess_lev() == TMess::Debug)
	mess_debug(mod->nodePath().c_str(), _("Expand/update branch '%s' time %f ms."), i->text(2).toStdString().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
}

int ConfApp::cntrIfCmd( XMLNode &node )
{
    //Fill generic request
    if(&node != &genReqs && genReqs.attr("fillMode") == "1") { genReqs.childAdd()->operator=(node); return -1; }
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
	node.setAttr("path",genReqs.attr("path")+"/"+node.attr("path"));
    }

    //Direct request
    try {
	int rez = cntrIfCmdHosts(node);
	//int rez = SYS->transport().at().cntrIfCmd(node,"UIQtCfg",wUser->user().toStdString());

	// Multiple requests to selected nodes into the tree
	if((node.name() == "set" || node.name() == "load" || node.name() == "save") && CtrTree->selectedItems().size() >= 2) {
	    string reqPath = node.attr("path"), reqPathEl, selNds;
	    size_t reqElPos = reqPath.rfind("/");
	    if(reqElPos != string::npos) {
		reqPathEl = reqPath.substr(reqElPos+1); reqPath = reqPath.substr(0,reqElPos);
		QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
		for(int i_el = 0; i_el < sel_ls.size(); i_el++)
		    if(sel_ls.at(i_el)->text(2).toStdString() != reqPath)
			selNds += sel_ls.at(i_el)->text(2).toStdString()+"\n";
		if(selNds.size()) {
		    int questRes = QMessageBox::question(this,_("Send changes to selections"),
			    TSYS::strMess(_("Send current command '%s' to other selected nodes \"%s\"?"),node.name().c_str(),selNds.c_str()).c_str(),
			    QMessageBox::Apply|QMessageBox::Cancel,QMessageBox::Apply);
		    for(int off = 0; questRes == QMessageBox::Apply && (reqPath=TSYS::strLine(selNds,0,&off)).size(); ) {
			node.setAttr("path", reqPath+"/"+reqPathEl);
			cntrIfCmdHosts(node);
			//SYS->transport().at().cntrIfCmd(node, "UIQtCfg", wUser->user().toStdString());
		    }
		}
	    }
	}
	return rez;
    } catch(TError &err) {
	node.childClear();
	node.setAttr("mcat",err.cat)->setAttr("rez","10")->setText(err.mess);
    }

    return s2i(node.attr("rez"));
}

int ConfApp::cntrIfCmdHosts( XMLNode &node )
{
    //Mark commands in "primaryCmd", for redundant hosts mostly transfer
    // !!! Move further to the command's source
    if(node.name() == "set" || node.name() == "add" || node.name() == "ins" || node.name() == "del" || node.name() == "move" ||
	    node.name() == "load" || node.name() == "save" || node.name() == "copy")
	node.setAttr("primaryCmd", "1");

    string hostId = TSYS::pathLev(node.attr("path"), 0);
    SCADAHost *iHost = hosts[hostId];

    //No the host present
    if(!iHost) {
	node.childClear();
	node.setAttr("mcat",mod->nodePath())->setAttr("rez","11")->setText(TSYS::strMess(_("Unknown host '%s'."),hostId.c_str()));
	return s2i(node.attr("rez"));
    }

    //???? Main-first request
    inHostReq++;
    while(iHost->reqBusy()) {
	reqPrgrsSet(0, QString(_("Wait for reply from host '%1'")).arg(hostId.c_str()), iHost->reqTmMax);
	qApp->processEvents();
	TSYS::sysSleep(0.01);
    }
    if(!iHost->reqDo(node)) {
	reqPrgrsSet(0, QString(_("Wait for reply from host '%1'")).arg(hostId.c_str()), iHost->reqTmMax);

	//Wait for the request done
	time_t stTm = SYS->sysTm();
	while(iHost->reqBusy()) {
	    reqPrgrsSet(vmax(0,SYS->sysTm()-stTm));
	    if(reqPrgrs && reqPrgrs->wasCanceled()) iHost->sendSIGALRM();
	    qApp->processEvents();
	    TSYS::sysSleep(0.01);
	}
    }
    inHostReq--;

    return s2i(node.attr("rez"));
}

void ConfApp::reqPrgrsSet( int cur, const QString &lab, int max )
{
    //Create
    if(!reqPrgrs && cur >= 0) {
	reqPrgrs = new QProgressDialog(this);
	reqPrgrs->setWindowModality(Qt::WindowModal);
	reqPrgrs->setCancelButtonText(_("Cancel"));
	reqPrgrs->show();
    }
    //Close
    else if(reqPrgrs && cur < 0) {
	reqPrgrsTimer->stop();
	delete reqPrgrs;
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

string ConfApp::getPrintVal( const string &vl )
{
    bool isBool = false;
    for(unsigned iCh = 0; !isBool && iCh < vl.size(); ++iCh)
	switch(vl[iCh]) {
	    case 0: isBool = true; break;
	}

    return isBool ? "B["+TSYS::strDecode(vl,TSYS::Bin)+"]" : vl;
}

void ConfApp::initHosts( )
{
    vector<TTransportS::ExtHost> stls;
    SYS->transport().at().extHostList(wUser->user().toStdString(), stls);
    stls.insert(stls.begin(), TTransportS::ExtHost("",SYS->id()));

    //Remove no present hosts
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
	if(!nit) {
	    nit = new QTreeWidgetItem(CtrTree);

	    // Append the host thread
	    if(hosts.find(stls[iSt].id) == hosts.end()) {
		hosts[stls[iSt].id] = new SCADAHost(stls[iSt].id.c_str(), wUser->user(), (stls[iSt].id!=SYS->id()), this);
		connect(hosts[stls[iSt].id], SIGNAL(setSt(const QString&,int,const QImage&,const QStringList&,const QString&)),
			this, SLOT(hostStSet(const QString&,int,const QImage&,const QStringList&,const QString&)), Qt::QueuedConnection);
		hosts[stls[iSt].id]->start();
	    }
	}
	if(stls[iSt].id == SYS->id()) {
	    nit->setText(0, trU(SYS->name(),wUser->user().toStdString()).c_str());
	    nit->setText(1, _("Local station"));
	    nit->setText(2, ("/"+SYS->id()).c_str());
	}
	else {
	    nit->setText(0, trU(stls[iSt].name,wUser->user().toStdString()).c_str());
	    nit->setText(1, _("Remote station"));
	    nit->setText(2, ("/"+stls[iSt].id).c_str());
	}
	if(hosts[stls[iSt].id]) hosts[stls[iSt].id]->userSet(wUser->user());
    }
}

//*************************************************
//* ConfApp: Self widget's slots                  *
//*************************************************
void ConfApp::checkBoxStChange( int stat )
{
    QCheckBox *box = (QCheckBox *)sender();

    if(stat == Qt::PartiallyChecked) return;
    try {
	string path = box->objectName().toStdString();
	string val = (stat==Qt::Checked) ? "1" : "0";

	//Check block element
	if(path[0] == 'b') {
	    SYS->ctrId(root,TSYS::strDecode(path.substr(1),TSYS::PathEl) )->setText(val);
	    return;
	}
	else {
	    XMLNode req("get");
	    req.setAttr("path",selPath+"/"+path);
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }

	    if(req.text() == val) return;
	    mess_info(mod->nodePath().c_str(), _("%s| Set '%s' to '%s'!"),
		wUser->user().toStdString().c_str(), (selPath+"/"+path).c_str(), val.c_str());

	    req.setName("set")->setText(val);
	    if(cntrIfCmd(req))	mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this);
	}
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //Redraw
    pageRefresh(true);
}

void ConfApp::buttonClicked( )
{
    QPushButton *button = (QPushButton *)sender();

    try {
	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(button->objectName().toStdString(),TSYS::PathEl));

	//Check link
	if(n_el->attr("tp") == "lnk") {
	    XMLNode req("get"); req.setAttr("path",selPath+"/"+button->objectName().toStdString());
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat"), req.text(), TUIMod::Error, this); return; }
	    string url = "/"+TSYS::pathLev(selPath,0)+req.text();
	    mess_info(mod->nodePath().c_str(), _("%s| Go to link '%s'!"), wUser->user().toStdString().c_str(), url.c_str());
	    selectPage(url);
	    return;
	}
	else {
	    XMLNode req("set"); req.setAttr("path", selPath+"/"+button->objectName().toStdString());
	    //Copy parameters
	    for(unsigned i_ch = 0; i_ch < n_el->childSize(); i_ch++)
		*(req.childAdd()) = *(n_el->childGet(i_ch));

	    mess_info(mod->nodePath().c_str(), _("%s| Press '%s'!"),
		wUser->user().toStdString().c_str(), (selPath+"/"+button->objectName().toStdString()).c_str());
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }
	}
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //Redraw
    pageRefresh(true);
}

void ConfApp::combBoxActivate( const QString& ival )
{
    bool block = false;
    string val = ival.toStdString();
    XMLNode *n_el;
    QComboBox *comb = (QComboBox *)sender();

    try {
	string path = comb->objectName().toStdString();
	if(path[0] == 'b') { block = true; path = path.substr(1); }

	n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );

	//Get list for index list check!
	if(n_el->attr("dest") == "select") {
	    bool find_ok = false;
	    if(n_el->attr("select").empty()) {
		bool ind_ok = n_el->attr("sel_id").size();
		string s_nm;
		for(int ls_off = 0, c_el = 0; !(s_nm=TSYS::strSepParse(n_el->attr("sel_list"),0,';',&ls_off)).empty(); c_el++)
		    if(s_nm == val) {
			if(ind_ok)	val = TSYS::strSepParse(n_el->attr("sel_id"),c_el,';');
			find_ok = true;
		    }
	    }
	    else {
		XMLNode x_lst("get");
		x_lst.setAttr("path",selPath+"/"+TSYS::strEncode( n_el->attr("select"),TSYS::PathEl));
		if(cntrIfCmd(x_lst)) { mod->postMess(x_lst.attr("mcat"),x_lst.text(),TUIMod::Error,this); return; }

		for(unsigned i_el = 0; i_el < x_lst.childSize(); i_el++)
		    if(x_lst.childGet(i_el)->name() == "el" && x_lst.childGet(i_el)->text() == val) {
			if(x_lst.childGet(i_el)->attr("id").size()) val = x_lst.childGet(i_el)->attr("id");
			find_ok = true;
		    }
	    }
	    if(!find_ok) { mod->postMess(mod->nodePath().c_str(),_("Value no valid: ")+val,TUIMod::Info,this); return; }
	}

	//Check block element. Command box!
	if(block) { n_el->setText(val); return; }
	else {
	    XMLNode req("get"); req.setAttr("path",selPath+"/"+path);
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }

	    if(req.text() == val) return;
	    mess_info(mod->nodePath().c_str(),_("%s| Change '%s' from '%s' to '%s'!"),
		    wUser->user().toStdString().c_str(), (selPath+"/"+path).c_str(), req.text().c_str(), val.c_str());

	    req.setName("set")->setText(val);
	    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);// return; }
	}
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //Redraw
    pageRefresh(true);
}

void ConfApp::listBoxPopup( )
{
    QMenu popup;
    QListWidget *lbox = (QListWidget *)sender();
    QListWidgetItem *item = lbox->currentItem();
    string el_path = selPath+"/"+lbox->objectName().toStdString();
    XMLNode *n_el;

    QAction *last_it, *actBr, *actAdd, *actIns, *actEd, *actDel, *actMoveUp, *actMoveDown;
    last_it = actBr = actAdd = actIns = actEd = actDel = actMoveUp = actMoveDown = NULL;

    try {
	n_el = SYS->ctrId(root, TSYS::strDecode(lbox->objectName().toStdString(),TSYS::PathEl));
	if(n_el->attr("tp") == "br" && item != NULL) {
	    actBr = last_it = new QAction(_("Go"),this);
	    popup.addAction(actBr);
	    popup.addSeparator();
	}
	if((s2i(n_el->attr("acs"))&SEC_WR) && n_el->attr("s_com").size()) {
	    if(n_el->attr("s_com").find("add") != string::npos) {
	        actAdd = last_it = new QAction(_("Add"),this);
		popup.addAction(actAdd);
	    }
	    if(n_el->attr("s_com").find("ins") != string::npos && item != NULL) {
		actIns = last_it = new QAction(_("Insert"),this);
		popup.addAction(actIns);
	    }
	    if(n_el->attr("s_com").find("edit") != string::npos && item != NULL) {
		actEd = last_it = new QAction(_("Edit"),this);
		popup.addAction(actEd);
	    }
	    if(n_el->attr("s_com").find("del") != string::npos && item != NULL) {
		popup.addSeparator();
		actDel = last_it = new QAction(_("Delete"),this);
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

	if(last_it) {
	    string p_text, p_id;
	    string text, id;
	    int ind_m = s2i(n_el->attr("idm"));
	    int	c_id  = lbox->currentRow();

	    if(item != NULL) {
		//Get select id
		XMLNode x_lst("get");
		x_lst.setAttr("path",el_path);
		if(cntrIfCmd(x_lst)) {
		    mod->postMess(x_lst.attr("x_lst"),x_lst.text(),TUIMod::Error,this);
		    return;
		}

		p_text = item->text().toStdString();
		if(ind_m)
		    for(unsigned i_el = 0; i_el < x_lst.childSize(); i_el++)
			if(x_lst.childGet(i_el)->text() == item->text().toStdString()) {
			    p_id = x_lst.childGet(i_el)->attr("id");
			    break;
			}
	    }

	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	{ popup.clear(); return; }
	    if(rez == actAdd || rez == actIns || rez == actEd) {
		ReqIdNameDlg dlg(this, this->windowIcon(), "", _("Item name set"));
		vector<string> ils;
		ils.push_back(n_el->attr("idSz")+"\n"+i2s(ind_m));
		dlg.setTargets(ils);
		if(rez == actAdd)	dlg.setMess(_("Add new element."));
		else if(rez == actIns)	dlg.setMess(_("Insert new element."));
		if(rez == actEd) {
		    dlg.setMess(_("Rename element."));
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
		mess_info(mod->nodePath().c_str(),_("%s| Add '%s' element <%s:%s>!"),
			wUser->user().toStdString().c_str(), el_path.c_str(), id.c_str(), text.c_str());
	    }
	    else if(rez == actIns) {
		n_el1.setName("ins")->setAttr("pos", i2s(c_id))->setAttr("p_id",(ind_m)?p_id:p_text);
		if(ind_m) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| Insert '%s' element <%s:%s> to %d!"),
			wUser->user().toStdString().c_str(), el_path.c_str(), id.c_str(), text.c_str(),c_id);
	    }
	    else if(rez == actEd) {
		n_el1.setName("edit")->setAttr("pos", i2s(c_id))->setAttr("p_id",(ind_m)?p_id:p_text);
		if(ind_m) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| Set '%s' element %d to <%s:%s>!"),
			wUser->user().toStdString().c_str(), el_path.c_str(), c_id, id.c_str(), text.c_str());
	    }
	    else if(rez == actDel) {
		n_el1.setName("del")->setAttr("pos", i2s(c_id));
		if(ind_m) n_el1.setAttr("id",p_id);
		else n_el1.setText(item->text().toStdString());
		mess_info(mod->nodePath().c_str(),_("%s| Delete '%s' element <%s:%s>!"),
			wUser->user().toStdString().c_str(), el_path.c_str(), n_el1.attr("id").c_str(), n_el1.text().c_str());
	    }
	    else if(rez == actMoveUp || rez == actMoveDown) {
		int c_new = (rez==actMoveDown) ? c_id+1 : c_id-1;
		n_el1.setName("move")->setAttr("pos", i2s(c_id))->setAttr("to", i2s(c_new));
		mess_info(mod->nodePath().c_str(),_("%s| Move '%s' from %d to %d!"),
			wUser->user().toStdString().c_str(), el_path.c_str(), c_id, c_new);
	    }
	    if(cntrIfCmd(n_el1)) {
		mod->postMess(n_el1.attr("mcat"),n_el1.text(),TUIMod::Error,this);
		return;
	    }

	    pageRefresh(true);	//Redraw

	    if(n_el->attr("tp") == "br" && (rez == actAdd || rez == actIns || rez == actEd || rez == actDel))
		treeUpdate();

	    popup.clear();
	}
    } catch(TError &err) {
	mod->postMess(err.cat,err.mess,TUIMod::Error,this);
	pageRefresh(true);	//Redraw
    }
}

void ConfApp::tablePopup( const QPoint &pos )
{
    QMenu popup;
    QTableWidget *tbl = (QTableWidget *)sender();
    string el_path = selPath+"/"+tbl->objectName().toStdString();

    QAction *last_it, *actAdd, *actIns, *actDel, *actMoveUp, *actMoveDown, *actCopy;
    last_it = actAdd = actIns = actDel = actMoveUp = actMoveDown = actCopy = NULL;

    int row = tbl->currentRow();

    try {
	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(tbl->objectName().toStdString(),TSYS::PathEl));

	if((s2i(n_el->attr("acs"))&SEC_WR) && n_el->attr("s_com").size()) {
	    if(n_el->attr("s_com").find("add") != string::npos) {
	        actAdd = last_it = new QAction(_("Add record"),this);
		popup.addAction(actAdd);
	    }
	    if(n_el->attr("s_com").find("ins") != string::npos && row != -1) {
		actIns = last_it = new QAction(_("Insert record"),this);
		popup.addAction(actIns);
	    }
	    if(n_el->attr("s_com").find("del") != string::npos && row != -1) {
		actDel = last_it = new QAction(_("Delete record"),this);
		popup.addAction(actDel);
	    }
	    if(n_el->attr("s_com").find("move") != string::npos && row != -1) {
		popup.addSeparator();
		actMoveUp = last_it = new QAction(_("Move Up"),this);
		popup.addAction(actMoveUp);
		actMoveDown = last_it = new QAction(_("Move Down"),this);
		popup.addAction(actMoveDown);
	    }
	}
	if(!tbl->selectedItems().isEmpty()) {
	    popup.addSeparator();
	    actCopy = last_it = new QAction(_("Copy"),this);
	    popup.addAction(actCopy);
	}

	if(last_it) {
	    QString text;

	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	{ popup.clear(); return; }

	    if(rez == actCopy) {
		QString cbRez;
		bool firstRow = false, firstClm = false;
		for(int i_r = 0; i_r < tbl->rowCount(); i_r++) {
		    //if(firstRow && firstClm) cbRez += "\n";
		    firstClm = false;
		    for(int i_c = 0; i_c < tbl->columnCount(); i_c++) {
			if(!tbl->item(i_r,i_c)->isSelected()) continue;
			if(firstClm) cbRez += "\t";
			if(!firstClm && firstRow) cbRez += "\n";
			cbRez += tbl->item(i_r,i_c)->text();
			firstClm = firstRow = true;
		    }
		}
		QApplication::clipboard()->setText(cbRez);
		popup.clear();
		return;
	    }

	    XMLNode n_el1;
	    n_el1.setAttr("path",el_path);
	    if(rez == actAdd) {
		n_el1.setName("add");
		mess_info(mod->nodePath().c_str(), _("%s| Add '%s' record."),
			wUser->user().toStdString().c_str(), el_path.c_str());
	    }
	    else if(rez == actIns) {
		n_el1.setName("ins");
		n_el1.setAttr("row", i2s(row));
		mess_info(mod->nodePath().c_str(),_("%s| Insert '%s' record %d."),
			wUser->user().toStdString().c_str(), el_path.c_str(), row);
	    }
	    else if(rez == actDel) {
		n_el1.setName("del");
		string row_addr;
		if(!n_el->attr("key").size()) {
		    row_addr = i2s(row);
		    n_el1.setAttr("row",row_addr);
		}
		else {
		    // Get Key columns
		    string key;
		    for(int i_off = 0; (key=TSYS::strSepParse(n_el->attr("key"),0,',',&i_off)).size(); )
			for(unsigned i_el = 0; i_el < n_el->childSize(); i_el++)
			    if(n_el->childGet(i_el)->attr("id") == key) {
				n_el1.setAttr("key_"+key,n_el->childGet(i_el)->childGet(row)->text());
				row_addr = row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
				break;
			    }
		}
		mess_info(mod->nodePath().c_str(),_("%s| Delete '%s' record '%s'."),
			wUser->user().toStdString().c_str(), el_path.c_str(), row_addr.c_str());
	    }
	    else if(rez == actMoveUp || rez == actMoveDown) {
		int r_new = row-1;
		if(rez == actMoveDown)	r_new = row+1;
		n_el1.setName("move");
		n_el1.setAttr("row", i2s(row))->setAttr("to", i2s(r_new));
		mess_info(mod->nodePath().c_str(),_("%s| Move '%s' record from %d to %d."),
			wUser->user().toStdString().c_str(), el_path.c_str(), row, r_new);
	    }
	    if(cntrIfCmd(n_el1)) throw TError(n_el1.attr("mcat").c_str(), n_el1.text().c_str());

	    popup.clear();
	}
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    pageRefresh(true);	//Redraw
}

void ConfApp::imgPopup( const QPoint &pos )
{
    QMenu popup;
    ImgView *img = (ImgView *)sender();
    string el_path = selPath+"/"+img->objectName().toStdString();

    QAction *last_it, *save_img, *load_img;
    last_it=save_img=load_img=NULL;

    try {
	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(img->objectName().toStdString(),TSYS::PathEl));
	if(!img->image().isNull()) {
	    save_img = last_it = new QAction(_("Save image"),this);
	    popup.addAction(save_img);
	}
	if(s2i(n_el->attr("acs"))&SEC_WR) {
	    load_img = last_it = new QAction(_("Load image"),this);
	    popup.addAction(load_img);
	}

	if(last_it) {
	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	return;
	    if(rez == save_img) {
		QString fileName = QFileDialog::getSaveFileName(this,_("Save picture"),"img.png",_("Images (*.png *.xpm *.jpg)"));
		if(!fileName.isEmpty() && !img->image().save(fileName))
		    throw TError(mod->nodePath().c_str(),_("Save to file %s error\n"),fileName.toStdString().c_str());
	    }
	    else if(rez == load_img) {
		//Get path to image file
		QString fileName = QFileDialog::getOpenFileName(this,_("Load picture"),"",_("Images (*.png *.jpg)"));
		if(fileName.isNull()) return;
		int len;
		char buf[STR_BUF_LEN];
		string rez;

		//Load image file
		int hd = open(fileName.toStdString().c_str(), O_RDONLY);
		if(hd < 0) throw TError(mod->nodePath().c_str(), _("Open file %s error\n"), fileName.toStdString().c_str());
		while((len=read(hd,buf,sizeof(buf))) > 0) rez.append(buf, len);
		::close(hd);

		//Set image to widget
		if(!img->setImage(rez))
		    throw TError(mod->nodePath().c_str(), _("Image file %s error\n"), fileName.toStdString().c_str());

		//Send image to system
		XMLNode n_el1("set");
		n_el1.setAttr("path",el_path)->setText(TSYS::strEncode(rez,TSYS::base64));
		mess_info(mod->nodePath().c_str(),_("%s| Upload picture '%s' to: %s."),
		    wUser->user().toStdString().c_str(), fileName.toStdString().c_str(), el_path.c_str());
		if(cntrIfCmd(n_el1)) { mod->postMess(n_el1.attr("mcat"),n_el1.text(),TUIMod::Error,this); return; }
	    }
	}
    } catch(TError &err) {
	mod->postMess(err.cat,err.mess,TUIMod::Error,this);
	pageRefresh(true);	//Redraw
    }
}

void ConfApp::tableSet( int row, int col )
{
    bool noReload = false;
    string value;
    if(tblInit || row < 0 || col < 0) return;

    try {
	QTableWidget *tbl = (QTableWidget *)sender();
	string el_path = selPath+"/"+tbl->objectName().toStdString();

	XMLNode *n_el = SYS->ctrId(root, TSYS::strDecode(tbl->objectName().toStdString(),TSYS::PathEl));

	QVariant val = tbl->item(row,col)->data(Qt::EditRole);
	if(n_el->childGet(col)->attr("tp") == "bool") value = val.toBool() ? "1" : "0";
	else if(n_el->childGet(col)->attr("dest") == "select") {
	    value = val.toString().toStdString();
	    bool find_ok = false;
	    if(n_el->childGet(col)->attr("select").empty()) {
		bool ind_ok = n_el->childGet(col)->attr("sel_id").size();
		string s_nm;
		for(int ls_off = 0, c_el = 0; !(s_nm=TSYS::strSepParse(n_el->childGet(col)->attr("sel_list"),0,';',&ls_off)).empty(); c_el++)
		    if(s_nm == value) {
			if(ind_ok) value = TSYS::strSepParse(n_el->childGet(col)->attr("sel_id"),c_el,';');
			find_ok = true;
		    }
	    }
	    else {
		XMLNode x_lst("get"); x_lst.setAttr("path",selPath+"/"+TSYS::strEncode(n_el->childGet(col)->attr("select"),TSYS::PathEl));
		if(cntrIfCmd(x_lst)) { mod->postMess(x_lst.attr("mcat"),x_lst.text(),TUIMod::Error,this); return; }

		bool ind_ok = s2i(n_el->childGet(col)->attr("idm"));
		for(unsigned i_el = 0; i_el < x_lst.childSize(); i_el++) {
		    if(!i_el) ind_ok = x_lst.childGet(i_el)->attr("id").size();
		    if(x_lst.childGet(i_el)->text() == value) {
			if(ind_ok) value = x_lst.childGet(i_el)->attr("id");
			find_ok = true;
		    }
		}
	    }
	    if(!find_ok) throw TError(mod->nodePath().c_str(),_("Value '%s' is not valid!"),value.c_str());
	}
	else value = val.toString().toStdString();

	//Prepare request
	XMLNode n_el1("set"); n_el1.setAttr("path",el_path)->setText(value);
	// Get current column id
	for(unsigned i_el = 0; i_el < n_el->childSize(); i_el++)
	    if(tbl->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString() == n_el->childGet(i_el)->attr("dscr").c_str())
	    { n_el1.setAttr("col",n_el->childGet(i_el)->attr("id")); break; }
	// Get row position
	string row_addr;
	if(!n_el->attr("key").size()) {
	    row_addr = i2s(row);
	    n_el1.setAttr("row",row_addr);
	}
	else {
	    // Get Key columns
	    string key;
	    for(int i_off = 0; (key=TSYS::strSepParse(n_el->attr("key"),0,',',&i_off)).size(); )
		for(unsigned i_el = 0; i_el < n_el->childSize(); i_el++)
		    if(n_el->childGet(i_el)->attr("id") == key) {
			n_el1.setAttr("key_"+key,n_el->childGet(i_el)->childGet(row)->text());
			row_addr = row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
			break;
		    }
	}

	// Put request
	mess_info(mod->nodePath().c_str(),_("%s| Set '%s' cell ('%s':%s) to: %s."),
	    wUser->user().toStdString().c_str(), el_path.c_str(), row_addr.c_str(), n_el1.attr("col").c_str(), value.c_str());
	if(cntrIfCmd(n_el1))	throw TError(n_el1.attr("mcat").c_str(),n_el1.text().c_str());
	noReload = s2i(n_el1.attr("noReload"));
	if(noReload) n_el->childGet(col)->childGet(row)->setText(value);
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    if(!noReload) pageRefresh(true);
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
	if(cntrIfCmd(req)) throw TError(mod->nodePath().c_str(),"%s",req.text().c_str(),4);

	//Find selected index
	bool sel_ok = false;
	for(unsigned i_el = 0; i_el < req.childSize(); i_el++)
	    if(req.childGet(i_el)->name() == "el" && req.childGet(i_el)->text() == item->text().toStdString()) {
		if(req.childGet(i_el)->attr("id").size())
		    path = selPath+"/"+TSYS::strEncode(br_pref+req.childGet(i_el)->attr("id"),TSYS::PathEl);
		else
		    path = selPath+"/"+TSYS::strEncode(br_pref+req.childGet(i_el)->text(),TSYS::PathEl);
		sel_ok = true;
	    }
	if(!sel_ok) throw TError(mod->nodePath().c_str(), _("Selective element '%s' is not present!"), item->text().toStdString().c_str());

	selPath = path;
	pageRefresh(true);
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::editChange( const QString& txt )
{
    QWidget *wed = (QWidget*)sender();

    try {
	string path = wed->objectName().toStdString();

	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	SYS->ctrId(root, TSYS::strDecode(path,TSYS::PathEl))->setText(txt.toStdString());
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::applyButton( QWidget *src )
{
    QWidget *bwidg = src ? src : (QWidget*)sender();

    string path = bwidg->objectName().toStdString();

    try {
	XMLNode *el = SYS->ctrId(root, TSYS::strDecode(path,TSYS::PathEl));
	string sval = el->text();
	if(el->attr("tp") == "dec" || el->attr("tp") == "hex" || el->attr("tp") == "oct") {
	    //Check and change decimal format and the limits control
	    if(sval.compare(0,2,"0x") == 0 || QString(sval.c_str()).contains(QRegExp("[abcdefABCDEF]")))
		el->setAttr("tpCh", "hex");
	    else if(sval.size() > 1 && sval[0] == '0')
		el->setAttr("tpCh", "oct");
	    else el->setAttr("tpCh", "dec");

	    long long vl = strtoll(sval.c_str(), NULL, 0), vlBrd;
	    if(el->attr("min").size()) vl = vmax(s2ll(el->attr("min")), vl);
	    if(el->attr("max").size()) vl = vmin(s2ll(el->attr("max")), vl);
	    sval = ll2s(vl);
	}

	mess_info(mod->nodePath().c_str(), _("%s| Change '%s' to: '%s'!"),
		wUser->user().toStdString().c_str(), (selPath+"/"+path).c_str(), sval.c_str());

	XMLNode n_el("set");
	n_el.setAttr("path", selPath+"/"+path)->setText(sval);
	if(cntrIfCmd(n_el)) { mod->postMess(n_el.attr("mcat"),n_el.text(),TUIMod::Error,this); return; }
    } catch(TError &err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //Redraw only for changing into same this widget
    if(!src) pageRefresh(true);
}

void ConfApp::cancelButton( )
{
    QWidget *bwidg = (QWidget *)sender();

    string path = bwidg->objectName().toStdString();

    //Redraw
    pageRefresh(true);
}

//***********************************************
// SHost - Host thread's control object         *
SCADAHost::SCADAHost( const QString &iid, const QString &iuser, bool iIsRemote, QObject *p ) :
    QThread(p), reqTmMax(0), id(iid), user(iuser), isRemote(iIsRemote), lnkOK(false), endRun(false), reqDone(false), tm(0), req(NULL), pid(0)
{

}

SCADAHost::~SCADAHost( )
{
    endRun = true;
    while(!wait(100)) sendSIGALRM();
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

bool SCADAHost::reqDo( XMLNode &node )
{
    if(req) return false;

    //Set the request
    mtx.lock();
    reqDone = false;
    req = &node;
    cond.wakeOne();
    cond.wait(mtx, 100);
    if(!reqDone) { mtx.unlock(); return false; }
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

    //Images for statuses init
    stmp = TUIS::icoGet("connecting"); imgConnEst.loadFromData((const uchar*)stmp.c_str(), stmp.size());
    stmp = TUIS::icoGet("disconnect"); imgDisConnect.loadFromData((const uchar*)stmp.c_str(), stmp.size());
    stmp = TUIS::icoGet("connected"); imgRemConnected.loadFromData((const uchar*)stmp.c_str(), stmp.size());

    while(!endRun) {
	mtx.lock(); wuser = user; mtx.unlock();
	//Link status processing
	if(isFirst || (!lnkOK && (SYS->sysTm()-tm) > s2i(TSYS::strParse(mod->tmConChk(),0,":"))) ||
		      (lnkOK && (SYS->sysTm()-tm) > s2i(TSYS::strParse(mod->tmConChk(),1,":")))) {
	    emit setSt(id, -1, imgConnEst);

	    // Check connection by the station name, icon and branches request
	    QString toolTip;
	    QStringList brs;
	    XMLNode req("CntrReqs"), *reqN;
	    req.setAttr("path", "/"+id.toStdString());
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
	    else if(rez/* == 10*/) {
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
	    if(lnkOK) lnkOK = (rez=cntrIfCmd(*req,wuser)) != 10;
	    else {
		req->childClear();
		req->setAttr("mcat",mod->nodePath()+"/"+id.toStdString())->setAttr("rez","10")->setText(_("No a link set"));
	    }
	    mtx.lock();
	    reqDone = true;
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
	node.setAttr("mcat",err.cat)->setAttr("rez","10")->setText(err.mess);
	tm = 0;		//Check the link immediately
    }

    return s2i(node.attr("rez"));
}

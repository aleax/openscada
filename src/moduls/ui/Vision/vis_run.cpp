
//OpenSCADA module UI.Vision file: vis_run.cpp
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <algorithm>

#include <linux/input.h>

#include <QApplication>
#if QT_VERSION >= 0x050000
# include <QWindow>
#endif
#include <QLocale>
#include <QDesktopWidget>
#include <QTimer>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QStatusBar>
#include <QWhatsThis>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QPainter>
#include <QToolBar>
#include <QPrintDialog>
#include <QDateTime>
#include <QTextStream>

#include "../QTStarter/lib_qtgen.h"
#include "vis_run.h"
#include "vis_run_widgs.h"
#include "vis_shapes.h"

#ifdef HAVE_PHONON
#ifdef HAVE_PHONON_VIDEOPLAYER
#include <phonon/VideoPlayer>
#include <phonon/VideoWidget>
#include <phonon/MediaObject>
#else
#include <Phonon/VideoPlayer>
#include <Phonon/VideoWidget>
#include <Phonon/MediaObject>
#endif
using namespace Phonon;
#endif

#undef _
#define _(mess) mod->I18N(mess, lang().c_str()).c_str()

using namespace OSCADA_QT;
using namespace VISION;

VisRun::VisRun( const string &iprjSes_it, const string &open_user, const string &user_pass, const string &VCAstat,
		bool icrSessForce, unsigned iScr ) :
#if QT_VERSION < 0x050000
    QMainWindow(QDesktopWidget().screen(iScr)),
#endif
    winClose(false), isResizeManual(false), updTmMax(0), planePer(0),
#ifndef QT_NO_PRINTER
    prPg(NULL), prDiag(NULL), prDoc(NULL),
#endif
    fileDlg(NULL),
    conErr(NULL), crSessForce(icrSessForce), mKeepAspectRatio(true), mWinPosCntrSave(false), prjSes_it(iprjSes_it),
    master_pg(NULL), mPeriod(1000), mConId(0), mScreen(iScr), wPrcCnt(0), reqtm(1), expDiagCnt(1), expDocCnt(1), expTblCnt(1), x_scale(1), y_scale(1),
    mAlrmSt(0xFFFFFF), alrLevSet(false), ntfSet(0), alrmUpdCnt(0), updPage(false), host(NULL)
{
#if QT_VERSION >= 0x050000
    if(qApp->screens().size() > 1 && iScr < qApp->screens().size() && windowHandle())
	windowHandle()->setScreen(qApp->screens()[iScr]);	//?!?! Check sometime
#endif

    QImage ico_t;

    connect(this, SIGNAL(makeStarterMenu(QWidget*,const QString&)), qApp, SLOT(makeStarterMenu(QWidget*,const QString&)));
    setAttribute(Qt::WA_DeleteOnClose, true);
    mod->regWin(this);

    setWindowIcon(mod->icon());

    //Create actions
    // Generic actions
    //  Print
    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
    menuPrint = new QMenu(this);
    menuPrint->setIcon(QPixmap::fromImage(ico_t));
    menuPrint->menuAction()->setShortcut(QKeySequence::Print);
    connect(menuPrint->menuAction(), SIGNAL(triggered()), this, SLOT(print()));
    actPrintPg = new QAction(this);
    connect(actPrintPg, SIGNAL(triggered()), this, SLOT(printPg()));
    menuPrint->addAction(actPrintPg);
    actPrintDiag = new QAction(this);
    connect(actPrintDiag, SIGNAL(triggered()), this, SLOT(printDiag()));
    menuPrint->addAction(actPrintDiag);
    actPrintDoc = new QAction(this);
    connect(actPrintDoc, SIGNAL(triggered()), this, SLOT(printDoc()));
    menuPrint->addAction(actPrintDoc);
    //  Export
    if(!ico_t.load(TUIS::icoGet("export",NULL,true).c_str())) ico_t.load(":/images/export.png");
    menuExport = new QMenu(this);
    menuExport->setIcon(QPixmap::fromImage(ico_t));
    connect(menuExport->menuAction(), SIGNAL(triggered()), this, SLOT(exportDef()));
    actExpPg = new QAction(this);
    connect(actExpPg, SIGNAL(triggered()), this, SLOT(exportPg()));
    menuExport->addAction(actExpPg);
    actExpDiag = new QAction(this);
    connect(actExpDiag, SIGNAL(triggered()), this, SLOT(exportDiag()));
    menuExport->addAction(actExpDiag);
    actExpDoc = new QAction(this);
    connect(actExpDoc, SIGNAL(triggered()), this, SLOT(exportDoc()));
    menuExport->addAction(actExpDoc);
    actExpTable = new QAction(this);
    connect(actExpTable, SIGNAL(triggered()), this, SLOT(exportTable()));
    menuExport->addAction(actExpTable);

    //  Close
    if(!ico_t.load(TUIS::icoGet("close",NULL,true).c_str())) ico_t.load(":/images/close.png");
    actClose = new QAction(QPixmap::fromImage(ico_t), "", this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    //  Quit
    if(!ico_t.load(TUIS::icoGet("exit",NULL,true).c_str())) ico_t.load(":/images/exit.png");
    actQuit = new QAction(QPixmap::fromImage(ico_t), "", this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quitSt()));
    // View actions
    //  Fullscreen
    actFullScr = new QAction(this);
    actFullScr->setCheckable(true);
    connect(actFullScr, SIGNAL(toggled(bool)), this, SLOT(fullScreen(bool)));

    // Help actions
    //  About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    actAbout = new QAction(QPixmap::fromImage(ico_t), "", this);
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    //  About Qt
    actQtAbout = new QAction(this);
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    //  The project manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    actProjManual = new QAction(QPixmap::fromImage(ico_t),"",this);
    actProjManual->setShortcut(Qt::Key_F1);
    connect(actProjManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  Vision manual
    actManual = new QAction(QPixmap::fromImage(ico_t), "", this);
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  OpenSCADA manual index
    actManualSYS = new QAction(QPixmap::fromImage(ico_t), "", this);
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    actWhatIs = new QAction(QPixmap::fromImage(ico_t), "", this);
    actWhatIs->setShortcut(Qt::SHIFT+Qt::Key_F1);
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    // Alarms actions
    //  Alarm level display button and full alarms quietance
    if(!ico_t.load(TUIS::icoGet("alarmLev",NULL,true).c_str())) ico_t.load(":/images/alarmLev.png");
    actAlrmLev = new QAction(QPixmap::fromImage(ico_t), "", this);
    actAlrmLev->setObjectName("alarmLev");

    menuFile.addAction(menuPrint->menuAction());
    menuFile.addAction(menuExport->menuAction());
    menuFile.addSeparator();
    menuFile.addAction(actClose);
    menuFile.addAction(actQuit);
    menuAlarm.addAction(actAlrmLev);
    menuView.addAction(actFullScr);
    menuHelp.addAction(actAbout);
    menuHelp.addAction(actQtAbout);
    menuHelp.addAction(actProjManual);
    menuHelp.addAction(actManual);
    menuHelp.addAction(actManualSYS);
    menuHelp.addSeparator();
    menuHelp.addAction(actWhatIs);

    //Init tool bars
    // Generic tools bar
    toolBarStatus = new QToolBar(this);
    connect(toolBarStatus, SIGNAL(actionTriggered(QAction*)), this, SLOT(alarmAct(QAction*)));
    toolBarStatus->setIconSize(QSize(icoSize(),icoSize()));
    toolBarStatus->addAction(actProjManual);
    toolBarStatus->addSeparator();
    toolBarStatus->addAction(menuPrint->menuAction());
    toolBarStatus->addAction(menuExport->menuAction());
    toolBarStatus->addSeparator();
    toolBarStatus->addAction(actAlrmLev);

    //Init status bar
    mWTime = new QLabel(this);
    mWTime->setVisible(false);
    mWTime->setAlignment(Qt::AlignCenter);
    statusBar()->insertPermanentWidget(0, mWTime);
    mWUser = new UserStBar(open_user.c_str(), user_pass.c_str(), VCAstat.c_str(), this);
    connect(mWUser, SIGNAL(userChanged(const QString&,const QString&)), this, SLOT(userChanged(const QString&,const QString&)));
    statusBar()->insertPermanentWidget(0, mWUser);
    mWStat = new QLabel(VCAStation().c_str(), this);
    mWStat->setVisible(VCAStation() != ".");
    statusBar()->insertPermanentWidget(0, mWStat);
    mStlBar = new StylesStBar(-1, this);
    connect(mStlBar, SIGNAL(styleChanged()), this, SLOT(styleChanged()));
    statusBar()->insertPermanentWidget(0, mStlBar);
    statusBar()->insertPermanentWidget(0, toolBarStatus);
    //statusBar()->setVisible(mod->runPrjsSt());

    //Init scroller
    QScrollArea *scrl = new QScrollArea;
    scrl->setFocusPolicy(Qt::NoFocus);
    scrl->setAlignment(Qt::AlignCenter);
    setCentralWidget(scrl);

    //Create timers
    // End run timer
    endRunTimer = new QTimer(this);
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(1e3*prmWait_DL);
    // Update timer
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(false);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePage()));

    //actProjManual->setEnabled(TUIS::docGet(actProjManual->property("doc").toString().toStdString(),NULL,TUIS::GetFilePath).size());

    if(!s2i(SYS->cmdOpt("showWin"))) resize(1024, 768);

    //Establish connection to the remote station
    //?!?! Disabled by default for the requesting into different threads before resolving the mouse release event in the same widget processing the press event
    if(SYS->cmdOptPresent("ReqInDifThread")) initHost();

    initSess(prjSes_it, crSessForce);	//init session

    messUpd();

    //mWStat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000);

    //Restoring the main window position
    if(!s2i(SYS->cmdOpt("showWin")) && winPosCntrSave() && masterPg() && !(windowState()&(Qt::WindowMaximized|Qt::WindowFullScreen))) {
	string xPos, yPos;
	if((xPos=wAttrGet(masterPg()->id(),i2s(screen())+"geomX",true)).size() &&
		(yPos=wAttrGet(masterPg()->id(),i2s(screen())+"geomY",true)).size())
	    move(s2i(xPos), s2i(yPos));
	else if(abs(masterPg()->posF().x()) || abs(masterPg()->posF().y()))
	    move(masterPg()->posF().x(), masterPg()->posF().y());
    }

    alarmSet(0);
}

VisRun::~VisRun( )
{
    winClose = true;

    endRunTimer->stop();
    updateTimer->stop();

    alarmSet(0);
    for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); iN != mNotify.end(); ++iN) delete iN->second;
    //alrmPlay->wait();

    //Disconnect/delete session
    XMLNode req("disconnect");
    req.setAttr("path", "/%2fserv%2fsess")->setAttr("sess", work_sess)->setAttr("conId", i2s(mConId));
    cntrIfCmd(req);

    //Unregister window
    mod->unregWin(this);

    //Clear cache
    pgCacheClear();

#ifndef QT_NO_PRINTER
    //Print objects free
    if(prPg)	delete prPg;
    if(prDiag)	delete prDiag;
    if(prDoc)	delete prDoc;
    if(fileDlg)	delete fileDlg;
#endif

    if(host && host->inHostReq)
	mess_err(mod->nodePath().c_str(), _("Session '%s(%s)' using the remote host %d times."),
	    workSess().c_str(), srcProject().c_str(), host->inHostReq);

    if(host) {
	delete host;

	// Push down all Qt events of the window to free the module
	for(int iTr = 0; iTr < 5; iTr++) qApp->processEvents();
    }

    //Child widgets remove before
    QObjectList chLst = children();
    for(int iC = 0; iC < chLst.size(); iC++) {
	WdgView *cw = qobject_cast<WdgView*>(chLst[iC]);
	if(cw)	delete cw;//cw->deleteLater();	//!!!! Direct deleting due to this step is the last one
    }
}

void VisRun::setFocus( const string &addr, bool ack )
{
    if(focusWdf == addr) return;

    XMLNode req("set");

    if(focusWdf.size()) {
	req.setAttr("path", focusWdf+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","focus")->setText("0");
	req.childAdd("el")->setAttr("id","event")->setText("ws_FocusOut");
	cntrIfCmd(req);
    }

    if(ack) focusWdf = addr;

    req.clear()->setAttr("path", addr+"/%2fserv%2fattr");
    req.childAdd("el")->setAttr("id","focus")->setText("1");
    req.childAdd("el")->setAttr("id","event")->setText("ws_FocusIn");
    cntrIfCmd(req);
}

bool VisRun::winMenu( )	{ return menuBar()->actions().length(); }

void VisRun::setWinMenu( bool act )
{
    menuBar()->clear();

    //Create menu
    if(act) {
	menuBar()->addMenu(&menuFile);
	menuBar()->addMenu(&menuAlarm);
	if(s2i(SYS->cmdOpt("showWin")) != 2) menuBar()->addMenu(&menuView);
	menuBar()->addMenu(&menuHelp);
	emit makeStarterMenu(NULL, lang().c_str());
    }
}

string VisRun::user( )		{ return mWUser->user(); }

bool VisRun::userSel( const string &hint ) { return mWUser->userSel(hint); }

string VisRun::password( )	{ return mWUser->pass(); }

string VisRun::VCAStation( )	{ return mWUser->VCAStation(); }

int VisRun::style( )		{ return mStlBar->style(); }

void VisRun::setStyle( int istl ) { mStlBar->setStyle(istl); }

void VisRun::initHost( )
{
    if(!host) {
	host = new SCADAHost(this);
	host->start();
    }
}

int VisRun::cntrIfCmd( XMLNode &node, bool glob, bool main )
{
    node.setAttr("reforwardRedundOff", "1");	//To prevent for redundancy

    if(masterPg() && conErr && (!main || (time(NULL)-conErr->property("tm").toLongLong()) < conErr->property("tmRest").toInt())) {
	if(main && conErr->property("labTmpl").toString().size())
	    conErr->setText(conErr->property("labTmpl").toString().arg(conErr->property("tmRest").toInt()-(time(NULL)-conErr->property("tm").toLongLong())));
	return TError::Tr_Connect;
    }

    int rez = 0;
    if(host) {
	host->inHostReq++;
	while(host->reqBusy()) {
	    qApp->processEvents();
	    TSYS::sysSleep(0.01);
	}
	//Do and wait for the request
	bool done = false;
	if(!host->reqDo(node,done,glob))
	    while(!done) {
		qApp->processEvents();
		TSYS::sysSleep(0.01);
	    }
	host->inHostReq--;
	if(winClose && !host->inHostReq) close();

	rez = s2i(node.attr("rez"));
    }
    else rez = mod->cntrIfCmd(node, user(), password(), VCAStation(), glob);

    //Display error message about connection error
    if(rez == TError::Tr_Connect && main && masterPg()) {
	if(!conErr) {
	    //Create error message
	    conErr = new QLabel(masterPg());
	    conErr->setAlignment(Qt::AlignCenter);
	    conErr->setWordWrap(true);
	    //Prepare message's style
	    conErr->setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
	    conErr->setAutoFillBackground(true);
	    QPalette plt(conErr->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(Qt::red);
	    brsh.setStyle(Qt::SolidPattern);
	    plt.setBrush(QPalette::Background, brsh);
	    conErr->setPalette(plt);
	    //Calc size and position
	    conErr->resize(300, 100);
	    conErr->move((masterPg()->size().width()-conErr->size().width())/2, (masterPg()->size().height()-conErr->size().height())/2);
	    //conErr->show();
	    conErr->setProperty("tmRest", 0);
	} else conErr->setProperty("tmRest", vmin(mod->restoreTime(),conErr->property("tmRest").toInt()+1));
	conErr->setProperty("tm", (long long)time(NULL));
	if(conErr->property("tmRest").toInt() > 3) {
	    if(!conErr->isVisible()) conErr->show();
	    conErr->setProperty("labTmpl",
		QString(_("Error connecting to the visualization server '%1': %2.\nThe next recovery attempt after %3s!"))
		    .arg(VCAStation().c_str()).arg(node.text().c_str()).arg("%1"));
	    conErr->setText(conErr->property("labTmpl").toString().arg(conErr->property("tmRest").toInt()));
	}
    }
    //Remove the error message about the connection error
    else if(rez != TError::Tr_Connect && main && conErr) {
	if(masterPg()) conErr->deleteLater();
	conErr = NULL;
	updTmMax = planePer = 0;
    }

    return rez;
}

QString VisRun::getFileName( const QString &caption, const QString &dir, const QString &filter, QFileDialog::AcceptMode mode )
{
    if(!fileDlg) fileDlg = new QFileDialog(this);
    fileDlg->setFileMode(QFileDialog::AnyFile);
    fileDlg->setAcceptMode(mode);
    fileDlg->setWindowTitle(caption);
    fileDlg->setNameFilter(filter);
    if(dir.size()) { QString dirF = dir; fileDlg->selectFile(dirF.replace("\"","")); }
#if QT_VERSION >= 0x040500
    fileDlg->setReadOnly(!winMenu());
#endif
    if(fileDlg->exec() && !fileDlg->selectedFiles().empty()) return fileDlg->selectedFiles()[0];

    return "";
}

void VisRun::closeEvent( QCloseEvent* ce )
{
    winClose = true;

    //Call for next processing by the events handler for the real closing after release all background requests
    if(host && host->inHostReq) { ce->ignore(); /*QCoreApplication::postEvent(this, new QCloseEvent());*/ return; }

    if(endRunTimer->isActive()) {
	//Save main window position
	if(winPosCntrSave() && masterPg()) {
	    wAttrSet(masterPg()->id(), i2s(screen())+"geomX", i2s(pos().x()), true);
	    wAttrSet(masterPg()->id(), i2s(screen())+"geomY", i2s(pos().y()), true);
	}

	//Exit on close last run project
	if(mod->exitLstRunPrjCls() && masterPg()) {
	    unsigned winCnt = 0;
	    for(int iW = 0; iW < QApplication::topLevelWidgets().size(); iW++)
		if(qobject_cast<QMainWindow*>(QApplication::topLevelWidgets()[iW]) && QApplication::topLevelWidgets()[iW]->isVisible())
		    winCnt++;

	    if(winCnt <= 1 && !qApp->property("closeToTray").toBool()) SYS->stop();
	}

	endRunTimer->stop();
	updateTimer->stop();
    }

    ce->accept();
}

void VisRun::resizeEvent( QResizeEvent *ev )
{
    if(masterPg()) {
	float x_scale_old = x_scale;
	float y_scale_old = y_scale;
	if(windowState()&(Qt::WindowMaximized|Qt::WindowFullScreen)) {
	    x_scale = (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(masterPg()->sizeOrigF().width()*masterPg()->xScale());
	    y_scale = (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(masterPg()->sizeOrigF().height()*masterPg()->yScale());
	    if(x_scale > 1 && x_scale < 1.02) x_scale = 1;
	    if(y_scale > 1 && y_scale < 1.02) y_scale = 1;
	    if(keepAspectRatio()) x_scale = y_scale = vmin(x_scale, y_scale);
	} else x_scale = y_scale = 1;
	if(x_scale_old != x_scale || y_scale_old != y_scale)	fullUpdatePgs();

	// Fit to the master page size
	if((x_scale_old != x_scale || y_scale_old != y_scale || !ev || !ev->oldSize().isValid()) && !(windowState()&(Qt::WindowMaximized|Qt::WindowFullScreen))) {
	    QRect ws = QApplication::desktop()->availableGeometry(this);
	    resize(fmin(ws.width()-10,masterPg()->size().width()+(centralWidget()->parentWidget()->width()-centralWidget()->width())+5),
		fmin(ws.height()-10,masterPg()->size().height()+(centralWidget()->parentWidget()->height()-centralWidget()->height())+5));
	}

	mess_debug(mod->nodePath().c_str(), _("Scale of the root page [%f:%f]."), x_scale, y_scale);
    }
    mWTime->setVisible(windowState()==Qt::WindowFullScreen);
}

void VisRun::endRunChk( )
{
    if(mod->endRun()) close();
}

void VisRun::quitSt( )
{
    SYS->stop();
}

void VisRun::print( )
{
    if(masterPg()) {
	//Check for the single and big document present for default the printing
	RunPageView *rpg;
	RunWdgView *rwdg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Document",lst);
	if(lst.size() == 1 && (rwdg=findOpenWidget(lst[0])) &&
		((masterPg()->width()/vmax(1,rwdg->width())) < 2 || (masterPg()->height()/vmax(1,rwdg->height())) < 2))
	    printDoc(rwdg->id());
	//Print master page
	else printPg(masterPg()->id());
    }
}

void VisRun::printPg( const string &ipg )
{
#ifndef QT_NO_PRINTER
    RunPageView *rpg;
    string pg = ipg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Printing a page"),_("There is no page to print!")); return; }

    if(pg.empty() && pgList.size() == 1)	pg = pgList[0];
    if(pg.empty() && pgList.size() > 1) {
	//Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a page to print."), _("Printing a page"), false, false, lang().c_str());
	sdlg.edLay()->addWidget(new QLabel(_("Pages:"),&sdlg), 2, 0);
	QComboBox *spg = new QComboBox(&sdlg);
	sdlg.edLay()->addWidget(spg, 2, 1);
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		spg->addItem((rpg->name()+" ("+pgList[iP]+")").c_str(),pgList[iP].c_str());
	if(sdlg.exec() != QDialog::Accepted)	return;
	pg = spg->itemData(spg->currentIndex()).toString().toStdString();
    }

    //Find need page
    rpg = master_pg;
    if(rpg->id() != pg)	rpg = findOpenPage(pg);
    if(!rpg) return;

    string pnm = rpg->name();
    if(!prPg)	prPg = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prPg, this);
    dlg.setWindowTitle(QString(_("Printing the page: \"%1\" (%2)")).arg(pnm.c_str()).arg(pg.c_str()));
    if(dlg.exec() == QDialog::Accepted) {
#if QT_VERSION >= 0x050300
	QRect	paperRect = prPg->pageLayout().fullRectPixels(prPg->resolution()),
		pgRect = prPg->pageLayout().paintRectPixels(prPg->resolution());
#else
	QRect	paperRect = prPg->paperRect(), pgRect = prPg->pageRect();
#endif
	int fntSize = 35;
	QSize papl(2048, 2048*paperRect.height()/paperRect.width());
	QSize pagl(papl.width()*pgRect.width()/paperRect.width(), papl.height()*pgRect.height()/paperRect.height());

	QPainter painter;
	painter.begin(prPg);
	painter.setWindow(QRect(QPoint(),papl));
	painter.setViewport(paperRect);

	//Draw image
# if QT_VERSION >= 0x050000
	QImage im = rpg->grab().toImage();
# else
	QImage im = QPixmap::grabWidget(rpg).toImage();
# endif
	im = im.scaled(QSize(vmin(im.width()*4,pagl.width()),vmin(im.height()*4,pagl.height()-2*fntSize)),Qt::KeepAspectRatio/*,Qt::SmoothTransformation*/);
	painter.drawImage((pagl.width()-im.width())/2,fntSize,im);

	//Draw notes
	painter.setPen(Qt::black);
	QFont fnt("Arial");
	fnt.setPixelSize(fntSize-5);
	painter.setFont(fnt);
	painter.drawText(QRect(0,0,pagl.width(),fntSize),Qt::AlignLeft,QString(_("OpenSCADA project: \"%1\"")).arg(windowTitle()));
	painter.drawText(QRect(0,0,pagl.width(),fntSize),Qt::AlignRight,QString(_("User: \"%1\"")).arg(user().c_str()));
	painter.drawText(QRect(0,im.height()+fntSize,pagl.width(),fntSize),Qt::AlignLeft,(pnm+" ("+pg+")").c_str());
	QDateTime dt;
	dt.setTime_t(time(NULL));
	painter.drawText(QRect(0,im.height()+fntSize,pagl.width(),fntSize),Qt::AlignRight,dt.toString("d.MM.yyyy h:mm:ss"));

	painter.end();
    }
#endif
}

void VisRun::printDiag( const string &idg )
{
#ifndef QT_NO_PRINTER
    RunWdgView *rwdg;
    string dg = idg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Printing a diagram"),_("There is no page!")); return; }

    if(dg.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Diagram", lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Printing a diagram"),_("There is no diagram!")); return; }
	if(lst.size() == 1)	dg = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a diagram to print."), _("Printing a diagram"), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Diagrams:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		if((rwdg=findOpenWidget(lst[iL])))
		    spg->addItem((rwdg->name()+" ("+lst[iL]+")").c_str(),lst[iL].c_str());
	    if(sdlg.exec() != QDialog::Accepted) return;
	    dg = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(dg)))	return;

    string dgnm = rwdg->name();
    if(!prDiag)	prDiag = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prDiag, this);
    dlg.setWindowTitle(QString(_("Printing the diagram: \"%1\" (%2)")).arg(dgnm.c_str()).arg(dg.c_str()));
    if(dlg.exec() == QDialog::Accepted) {
#if QT_VERSION >= 0x050300
	QRect	paperRect = prDiag->pageLayout().fullRectPixels(prDiag->resolution()),
		pgRect = prDiag->pageLayout().paintRectPixels(prDiag->resolution());
#else
	QRect	paperRect = prDiag->paperRect(), pgRect = prDiag->pageRect();
#endif
	int fntSize = 35;
	QSize papl(2048, 2048*paperRect.height()/paperRect.width());
	QSize pagl(papl.width()*pgRect.width()/paperRect.width(), papl.height()*pgRect.height()/paperRect.height());

	ShapeDiagram::ShpDt *sD = (ShapeDiagram::ShpDt*)rwdg->shpData;
	int elLine = sD->prms.size()/2+((sD->prms.size()%2)?1:0);

	QPainter painter;
	painter.begin(prDiag);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(paperRect);

	//Draw image
# if QT_VERSION >= 0x050000
	QImage im = rwdg->grab().toImage();
# else
	QImage im = QPixmap::grabWidget(rwdg).toImage();
# endif
	im = im.scaled(QSize(vmin(im.width()*4,pagl.width()),vmin(im.height()*4,pagl.height()-(2+elLine)*fntSize)),Qt::KeepAspectRatio/*,Qt::SmoothTransformation*/);
	painter.drawImage((pagl.width()-im.width())/2,fntSize*2,im);

	//Draw notes
	painter.setPen(Qt::black);
	QFont fnt("Arial");
	fnt.setPixelSize(fntSize-5);
	painter.setFont(fnt);
	QDateTime dt;
	dt.setTime_t(time(NULL));
	painter.drawText(QRect(0,0,pagl.width(),fntSize*2),Qt::AlignLeft,QString(_("OpenSCADA project: \"%1\"\n%2 (%3)")).arg(windowTitle()).arg(dgnm.c_str()).arg(dg.c_str()));
	painter.drawText(QRect(0,0,pagl.width(),fntSize*2),Qt::AlignRight,QString(_("User: \"%1\"\n%2")).arg(user().c_str()).arg(dt.toString("d.MM.yyyy h:mm:ss")));

	// Draw trend's elements
	XMLNode reqName("name");
	for(unsigned iE = 0; iE < sD->prms.size(); iE++) {
	    QPoint pnt((iE/elLine)*(pagl.width()/2),im.height()+fntSize*(2+iE%elLine));
	    if(sD->prms[iE].val().empty() || !sD->prms[iE].color().isValid()) continue;
	    //  Trend name request
	    reqName.setAttr("path",sD->prms[iE].addr()+"/%2fserv%2fval");
	    if(cntrIfCmd(reqName,true) || reqName.text().empty())	reqName.setText(sD->prms[iE].addr());

	    painter.fillRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5),QBrush(sD->prms[iE].color()));
	    painter.drawRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5));
	    painter.drawText(QRect(pnt.x()+fntSize,pnt.y(),pagl.width()/2,fntSize),Qt::AlignLeft,
		QString("%1 [%2...%3]").arg(reqName.text().c_str()).arg(sD->prms[iE].bordL()).arg(sD->prms[iE].bordU()));
	}

	painter.end();
    }
#endif
}

void VisRun::printDoc( const string &idoc )
{
#ifndef QT_NO_PRINTER
    RunWdgView *rwdg;
    string doc = idoc;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Printing a document"),_("There is no page!")); return; }

    if(doc.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Document",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Printing a document"),_("There is no document!")); return; }
	if(lst.size() == 1)	doc = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a document to print."), _("Printing a document"), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Document:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		if((rwdg=findOpenWidget(lst[iL])))
		    spg->addItem((rwdg->name()+" ("+lst[iL]+")").c_str(),lst[iL].c_str());
	    if(sdlg.exec() != QDialog::Accepted)	return;
	    doc = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(doc)))	return;

    string docnm = rwdg->name();
    if(!prDoc) prDoc = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prDoc, this);
    dlg.setWindowTitle(QString(_("Printing the document: \"%1\" (%2)")).arg(docnm.c_str()).arg(doc.c_str()));
    if(dlg.exec() == QDialog::Accepted) ((ShapeDocument::ShpDt*)rwdg->shpData)->print(prDoc);
#endif
}

void VisRun::exportDef( )
{
    if(master_pg) {
	//Check for the single and big document present for default the exporting
	RunPageView *rpg;
	RunWdgView *rwdg;
	vector<string> lstDoc, lstDiagr, lstTbls;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP]))) {
		rpg->shapeList("Document", lstDoc);
		rpg->shapeList("Diagram", lstDiagr);
		rpg->shapeList("FormEl",lstTbls);
		//Getting only tables
		for(int iW = 0; iW < lstTbls.size(); iW++) {
		    if((rwdg=findOpenWidget(lstTbls[iW])) && qobject_cast<QTableWidget*>(((ShapeFormEl::ShpDt*)rwdg->shpData)->addrWdg))
			continue;
		    lstTbls.erase(lstTbls.begin()+iW);
		    iW--;
		}
	    }
	if(lstDoc.size() == 1 && (rwdg=findOpenWidget(lstDoc[0])) &&
		((masterPg()->width()/vmax(1,rwdg->width())) < 2 || (masterPg()->height()/vmax(1,rwdg->height())) < 2))
	    exportDoc(rwdg->id());
	else if(lstDiagr.size() == 1 && (rwdg=findOpenWidget(lstDiagr[0])) &&
		((masterPg()->width()/vmax(1,rwdg->width())) < 2 || (masterPg()->height()/vmax(1,rwdg->height())) < 2))
	    exportDiag(rwdg->id());
	else if(lstTbls.size() == 1 && (rwdg=findOpenWidget(lstTbls[0])) &&
		((masterPg()->width()/vmax(1,rwdg->width())) < 2 || (masterPg()->height()/vmax(1,rwdg->height())) < 2))
	    exportTable(rwdg->id());
	//Export the master page
	else exportPg(master_pg->id());
    }
}

void VisRun::exportPg( const string &ipg )
{
    RunPageView *rpg;
    string pg = ipg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Exporting a page"),_("There is no page for exporting!")); return; }

    if(pg.empty() && pgList.size() == 1)	pg = pgList[0];
    if(pg.empty() && pgList.size() > 1) {
	//Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("export",NULL,true).c_str())) ico_t.load(":/images/export.png");
	InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a page to export."), _("Exporting a page"), false, false, lang().c_str());
	sdlg.edLay()->addWidget(new QLabel(_("Pages:"),&sdlg), 2, 0);
	QComboBox *spg = new QComboBox(&sdlg);
	sdlg.edLay()->addWidget(spg, 2, 1);
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		spg->addItem((rpg->name()+" ("+pgList[iP]+")").c_str(),pgList[iP].c_str());
	if(sdlg.exec() != QDialog::Accepted)	return;
	pg = spg->itemData(spg->currentIndex()).toString().toStdString();
    }

    //Find need page
    rpg = master_pg;
    if(rpg->id() != pg)	rpg = findOpenPage(pg);
    if(!rpg) return;

#if QT_VERSION >= 0x050000
    QPixmap img = rpg->grab();
#else
    QPixmap img = QPixmap::grabWidget(rpg);
#endif
    QString fn = getFileName(_("Saving the page image"), (rpg->name()+".png").c_str(), _("Images (*.png *.xpm *.jpg)"), QFileDialog::AcceptSave);
    if(fn.size() && !img.save(fn)) mod->postMess(mod->nodePath().c_str(), QString(_("Error saving to the file '%1'.")).arg(fn), TVision::Error, this);
}

void VisRun::exportDiag( const string &idg )
{
    RunWdgView *rwdg;
    string dg = idg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Exporting a diagram"),_("There is no page!")); return; }

    if(dg.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Diagram",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Exporting a diagram"),_("There is no diagram!")); return; }
	if(lst.size() == 1) dg = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a diagram to export."), _("Exporting a diagram"), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Diagrams:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		if((rwdg=findOpenWidget(lst[iL])))
		    spg->addItem((rwdg->name()+" ("+lst[iL]+")").c_str(),lst[iL].c_str());
	    if(sdlg.exec() != QDialog::Accepted) return;
	    dg = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(dg))) return;

#if QT_VERSION >= 0x050000
    QPixmap img = rwdg->grab();
#else
    QPixmap img = QPixmap::grabWidget(rwdg);
#endif
    QString fileName = getFileName(_("Saving a diagram"), QString(_("Trend %1.png")).arg(expDiagCnt++),
	_("Images (*.png *.xpm *.jpg);;CSV file (*.csv)"), QFileDialog::AcceptSave);
    if(!fileName.isEmpty()) {
	// Export to CSV
	if(fileName.indexOf(QRegExp("\\.csv$")) != -1) {
	    //  Open destination file
	    int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, SYS->permCrtFiles());
	    if(fd < 0) {
		mod->postMess(mod->nodePath().c_str(),QString(_("Error saving to the file '%1'.")).arg(fileName),TVision::Error,this);
		return;
	    }

	    ShapeDiagram::ShpDt *dgDt = (ShapeDiagram::ShpDt*)rwdg->shpData;
	    string CSVr;
	    //  Trend type process
	    if(dgDt->type == 0) {
		int firstPrm = -1, vPos = 0;
		//  Prepare header
		CSVr += _("\"Date and time\";\"us\"");
		for(unsigned iP = 0; iP < dgDt->prms.size(); iP++)
		    if(dgDt->prms[iP].val().size() && dgDt->prms[iP].color().isValid()) {
			CSVr += ";\""+TSYS::path2sepstr(dgDt->prms[iP].addr())+"\"";
			if(firstPrm < 0) firstPrm = iP;
		    }
		CSVr += "\x0D\x0A";
		if(firstPrm < 0) return;
		//  Place data
		deque<ShapeDiagram::TrendObj::SHg> &baseVls = dgDt->prms[firstPrm].val();
		int64_t eTmVl = dgDt->tTime;
		int64_t bTmVl = eTmVl - 1e6*dgDt->tSize;
		for(unsigned i_v = 0; i_v < baseVls.size() && baseVls[i_v].tm <= eTmVl; i_v++) {
		    if(baseVls[i_v].tm < bTmVl) continue;
		    CSVr += atm2s(baseVls[i_v].tm/1000000, "\"%d/%m/%Y %H:%M:%S\"") + ";" + i2s(baseVls[i_v].tm%1000000);
		    for(unsigned iP = 0; iP < dgDt->prms.size(); iP++) {
			ShapeDiagram::TrendObj &cPrm = dgDt->prms[iP];
			if(cPrm.val().size() && cPrm.color().isValid()) {
			    vPos = cPrm.val(baseVls[i_v].tm);
			    CSVr = CSVr + ";"+((vPos < (int)cPrm.val().size())?((cPrm.val()[vPos].val!=EVAL_REAL)?QLocale().toString(cPrm.val()[vPos].val).toStdString():"\""+string(_("Empty"))+"\""):"");
			}
		    }
		    CSVr += "\x0D\x0A";
		}
	    }
	    //  Frequency spectrum type
	    else if(dgDt->type == 1) {
#if HAVE_FFTW3_H
		//  Prepare header
		CSVr += _("\"Frequency (Hz)\"");
		for(unsigned iP = 0; iP < dgDt->prms.size(); iP++)
		    if(dgDt->prms[iP].fftN && dgDt->prms[iP].color().isValid())
			CSVr += ";\""+TSYS::path2sepstr(dgDt->prms[iP].addr())+"\"";
		CSVr += "\x0D\x0A";
		//  Place data
		int fftN = rwdg->size().width();		//Samples number
		double fftBeg = 1/dgDt->tSize;			//Minimum frequency or maximum period time (s)
		double fftEnd = (double)fftN*fftBeg/2;		//Maximum frequency or minimum period time (s)
		for(double i_frq = fftBeg; i_frq <= fftEnd; i_frq += fftBeg) {
		    CSVr += QLocale().toString(i_frq).toStdString();
		    for(unsigned iP = 0; iP < dgDt->prms.size(); iP++) {
			ShapeDiagram::TrendObj &cPrm = dgDt->prms[iP];
			if(cPrm.fftN && cPrm.color().isValid()) {
			    int vpos = (int)((i_frq*cPrm.fftN)/(fftBeg*fftN));
			    double val = EVAL_REAL;
			    if(vpos >= 1 && vpos < (cPrm.fftN/2+1))
				val = cPrm.fftOut[0][0]/cPrm.fftN + pow(pow(cPrm.fftOut[vpos][0],2)+pow(cPrm.fftOut[vpos][1],2),0.5)/(cPrm.fftN/2+1);
			    CSVr += ";"+QLocale().toString(val).toStdString();
			}
		    }
		    CSVr += "\x0D\x0A";
		}
#endif
	    }
	    //  Save to file
	    bool fOK = (write(fd,CSVr.data(),CSVr.size()) == (int)CSVr.size());
	    if(::close(fd) != 0)
		mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), fd, strerror(errno), errno);
	    if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error writing to: %1.")).arg(fileName), TVision::Error, this);
	}
	// Export to image
	else if(!img.save(fileName))
	    mod->postMess(mod->nodePath().c_str(),QString(_("Error saving to the file '%1'.")).arg(fileName),TVision::Error,this);
    }
}

void VisRun::exportDoc( const string &idoc )
{
    RunWdgView *rwdg;
    string doc = idoc;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Exporting a document"),_("There is no page!")); return; }

    if(doc.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Document",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Exporting a document"),_("There is no document!")); return; }
	if(lst.size() == 1) doc = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a document to export."), _("Exporting a document"), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Document:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		if((rwdg=findOpenWidget(lst[iL])))
		    spg->addItem((rwdg->name()+" ("+lst[iL]+")").c_str(),lst[iL].c_str());
	    if(sdlg.exec() != QDialog::Accepted) return;
	    doc = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(doc))) return;
    QString fileName = getFileName(_("Saving a document"), QString(_("Document %1.html")).arg(expDocCnt++),
	_("XHTML (*.html);;CSV file (*.csv)"), QFileDialog::AcceptSave);
    if(!fileName.isEmpty()) {
	int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, SYS->permCrtFiles());
	if(fd < 0) {
	    mod->postMess(mod->nodePath().c_str(),QString(_("Error saving to the file '%1'.")).arg(fileName),TVision::Error,this);
	    return;
	}
	string rez;
	// Export to CSV
	if(fileName.indexOf(QRegExp("\\.csv$")) != -1) {
	    //  Parse document
	    XMLNode docTree;
	    docTree.load(((ShapeDocument::ShpDt*)rwdg->shpData)->doc, true);
	    XMLNode *curNode = &docTree;
	    vector<unsigned> treeStk;
	    treeStk.push_back(0);
	    while(curNode->parent() || treeStk.back() < curNode->childSize()) {
		if(treeStk.back() < curNode->childSize()) {
		    curNode = curNode->childGet(treeStk.back());
		    treeStk.push_back(0);
		    //  Check for marked table and process it
		    if(strcasecmp(curNode->name().c_str(),"table") != 0 || !s2i(curNode->attr("export"))) continue;
		    map<int,int>	rowSpn;
		    XMLNode *tblN = NULL, *tblRow;
		    string val;
		    for(int iSt = 0; iSt < 4; iSt++) {
			switch(iSt) {
			    case 0: tblN = curNode->childGet("thead", 0, true);	break;
			    case 1: tblN = curNode->childGet("tbody", 0, true);	break;
			    case 2: tblN = curNode->childGet("tfoot", 0, true);	break;
			    case 3: tblN = curNode;				break;
			    default: tblN = NULL;
			}
			if(!tblN)	continue;
			//  Rows process
			for(unsigned iN = 0; iN < tblN->childSize(); iN++) {
			    if(strcasecmp(tblN->childGet(iN)->name().c_str(),"tr") != 0)	continue;
			    tblRow = tblN->childGet(iN);
			    for(unsigned iC = 0, iCl = 0; iC < tblRow->childSize(); iC++) {
				if(!(strcasecmp(tblRow->childGet(iC)->name().c_str(),"th") == 0 ||
					strcasecmp(tblRow->childGet(iC)->name().c_str(),"td") == 0))
				    continue;
				while(rowSpn[iCl] > 1) { rez += ";"; rowSpn[iCl]--; iCl++; }
				rowSpn[iCl] = s2i(tblRow->childGet(iC)->attr("rowspan",false));
				val = tblRow->childGet(iC)->text(true,true);
				for(size_t iSz = 0; (iSz=val.find("\"",iSz)) != string::npos; iSz += 2)
				    val.replace(iSz, 1, 2, '"');
				rez += "\"" + sTrm(val) + "\";";
				//   Colspan process
				int colSpan = s2i(tblRow->childGet(iC)->attr("colspan",false));
				for(int iCs = 1; iCs < colSpan; iCs++) rez += ";";
				iCl++;
			    }
			    rez += "\x0D\x0A";
			}
		    }
		    rez += "\x0D\x0A";
		}
		curNode = curNode->parent();
		treeStk.pop_back();
		treeStk.back()++;
	    }
	}
	// Export to XHTML
	else rez = ((ShapeDocument::ShpDt*)rwdg->shpData)->toHtml();

	bool fOK = true;
	if(rez.empty())	mod->postMess(mod->nodePath().c_str(),QString(_("No data to export.")),TVision::Error,this);
	else fOK = (write(fd,rez.data(),rez.size()) == (int)rez.size());
	if(::close(fd) != 0)
	    mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), fd, strerror(errno), errno);
	if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error writing to: %1.")).arg(fileName), TVision::Error, this);
    }
}

void VisRun::exportTable( const string &itbl )
{
    RunWdgView *rwdg;
    QTableWidget *wdg;
    string tbl = itbl;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Exporting a table"),_("There is no page!")); return; }

    if(tbl.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	//Getting all FormEls
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("FormEl",lst);

	//Getting only tables
	for(int iW = 0; iW < lst.size(); iW++) {
	    if((rwdg=findOpenWidget(lst[iW])) && qobject_cast<QTableWidget*>(((ShapeFormEl::ShpDt*)rwdg->shpData)->addrWdg))
		continue;
	    lst.erase(lst.begin()+iW);
	    iW--;
	}

	//Specifying a source
	if(lst.empty())	{ QMessageBox::warning(this,_("Exporting a table"),_("There is no table!")); return; }
	if(lst.size() == 1) tbl = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("table",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a table to export."), _("Exporting a table"), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Table:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned iL = 0; iL < lst.size(); iL++)
		if((rwdg=findOpenWidget(lst[iL])))
		    spg->addItem((rwdg->name()+" ("+lst[iL]+")").c_str(),lst[iL].c_str());
	    if(sdlg.exec() != QDialog::Accepted) return;
	    tbl = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(tbl)) || !(wdg=qobject_cast<QTableWidget*>(((ShapeFormEl::ShpDt*)rwdg->shpData)->addrWdg))) return;
    QString fileName = getFileName(_("Saving a table"), QString(_("Table %1.csv")).arg(expTblCnt++), _("CSV file (*.csv)"), QFileDialog::AcceptSave);
    if(!fileName.isEmpty()) {
	int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, SYS->permCrtFiles());
	if(fd < 0) {
	    mod->postMess(mod->nodePath().c_str(),QString(_("Error saving to the file '%1'.")).arg(fileName),TVision::Error,this);
	    return;
	}
	string rez;
	//Export to CSV
	if(fileName.indexOf(QRegExp("\\.csv$")) != -1) {
	    QTableWidgetItem *tit;
	    for(int iC = 0; iC < wdg->columnCount(); iC++)
		rez += "\""+((tit=wdg->horizontalHeaderItem(iC))?tit->text().toStdString():string(""))+"\";";
	    rez += "\x0D\x0A";
	    for(int iR = 0; iR < wdg->rowCount(); iR++) {
		for(int iC = 0; iC < wdg->columnCount(); iC++) {
		    QVariant vl = (tit=wdg->item(iR,iC)) ? tit->data(Qt::DisplayRole) : QVariant();
		    if(!vl.isValid()) rez += ";";
		    else if(vl.type() == QVariant::String) rez += "\""+TSYS::strEncode(vl.toString().toStdString(),TSYS::SQL,"\"")+"\";";
		    else rez += vl.toString().toStdString()+";";
		}
		rez += "\x0D\x0A";
	    }
	}
	bool fOK = true;
	if(rez.empty())	mod->postMess(mod->nodePath().c_str(),QString(_("No data to export.")),TVision::Error,this);
	else fOK = (write(fd,rez.data(),rez.size()) == (int)rez.size());
	if(::close(fd) != 0)
	    mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), fd, strerror(errno), errno);
	if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error writing to: %1.")).arg(fileName), TVision::Error, this);
    }
}

void VisRun::about( )
{
    TrCtxAlloc trCtx;
    if(Mess->translDyn()) trCtx.hold(user()+"\n"+lang());

    QString mess = _("%1 v%2.\n%3\nAuthor: %4\nLicense: %5\n\n"
		     "%6 v%7.\n%8\nLicense: %9\nAuthor: %10\nWeb site: %11");

#undef _
#define _(mess) Mess->I18N(mess, lang().c_str()).c_str()

    QMessageBox::about(this, windowTitle(), mess.
	arg(_(mod->modInfo("Name"))).arg(mod->modInfo("Version").c_str()).arg(_(mod->modInfo("Description"))).
	arg(_(mod->modInfo("Author"))).arg(mod->modInfo("License").c_str()).
	arg(PACKAGE_NAME).arg(VERSION).arg(_(PACKAGE_DESCR)).arg(PACKAGE_LICENSE).arg(_(PACKAGE_AUTHOR)).arg(PACKAGE_SITE));

#undef _
#define _(mess) mod->I18N(mess, lang().c_str()).c_str()
}

void VisRun::userChanged( const QString &oldUser, const QString &oldPass )
{
    //Try second connect to the session for permission check
    XMLNode req("connect");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess())->setAttr("userChange","1");
    if(cntrIfCmd(req)) {
	mWUser->setUser(oldUser.toStdString());
	mWUser->setPass(oldPass.toStdString());
	mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	return;
    }
    if(req.attr("userIsRoot").size()) setWinMenu(s2i(req.attr("userIsRoot")));
    else setWinMenu(SYS->security().at().access(user(),SEC_WR,"root","root",RWRWR_));
    int oldConId = mConId;
    mConId = s2i(req.attr("conId"));

    req.clear()->setName("CntrReqs")->setAttr("path","");
    //req.childAdd("disconnect")->setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess())->setAttr("conId", i2s(oldConId));
    req.childAdd("get")->setAttr("path","/ses_"+workSess()+"/%2fobj%2fcfg%2fstyle");
    if(!cntrIfCmd(req)) setStyle(s2i(req.childGet(0)->text()));

    //Update pages after the user change
    pgCacheClear();
    bool oldMenuVis = winMenu();
    QApplication::processEvents();
    if(masterPg()) {
	if(oldMenuVis != winMenu() && (windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen)) {
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	    if(x_scale > 1 && x_scale < 1.05) x_scale = 1;
	    if(y_scale > 1 && y_scale < 1.05) y_scale = 1;
	    if(keepAspectRatio()) x_scale = y_scale = vmin(x_scale, y_scale);
	    mess_debug(mod->nodePath().c_str(), _("Scale of the root page [%f:%f]."), x_scale, y_scale);
	}
	fullUpdatePgs();
	messUpd();

	//Resize
	resizeEvent(NULL);
    }
}

void VisRun::styleChanged( )
{
    //Get current style
    XMLNode req("set");
    req.setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fstyle")->setText(i2s(style()));
    if(cntrIfCmd(req)) {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	return;
    }
    fullUpdatePgs();
    pgCacheClear();
}

void VisRun::aboutQt( )		{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void VisRun::fullScreen( bool vl )
{
    setWindowState(vl?Qt::WindowFullScreen:Qt::WindowNoState);
    //!!!! But switching to the WindowMaximized is performed only through the WindowNoState
    if(!vl && s2i(SYS->cmdOpt("showWin"))) {
	QCoreApplication::processEvents();
	setWindowState(Qt::WindowMaximized);
    }
}

void VisRun::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void VisRun::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString()+"\n"+lang());
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("The manual '%1' was not found offline or online!")).arg(sender()->property("doc").toString()));
}

void VisRun::alarmAct( QAction *alrm )
{
    if(alrm == NULL) return;

    int quietance = 0;
    bool quietanceRet = false;
    string qwdg;
    if(alrm->objectName() == "alarmLev")	quietance = 0xFF;
    else if(alrm->objectName().toStdString().compare(0,8,"alarmNtf") == 0) {
	quietanceRet = alrm->property("quietanceRet").toBool();
	quietanceRet = quietanceRet && !alrm->isChecked();
	quietance = s2i(alrm->objectName().toStdString().substr(8));
	map<uint8_t, Notify*>::iterator iN = mNotify.find(quietance);
	if(!quietanceRet && iN != mNotify.end()) qwdg = iN->second->curQueueWdg();
	quietance = (1<<quietance);
    }
    else return;

    XMLNode req("quietance");
    req.setAttr("path", "/ses_"+work_sess+"/%2fserv%2falarm")->
	setAttr("tmpl", u2s(quietance))->
	setAttr("ret", i2s(quietanceRet))->
	setAttr("wdg", qwdg);
    cntrIfCmd(req);

    //Send event to master page
    if(master_pg) master_pg->attrSet("event", ("ws_"+alrm->objectName()).toStdString(), A_NO_ID, true);
}

void VisRun::usrStatus( const string &val, RunPageView *pg )
{
    UserItStBar *userSt;
    if(!pg) pg = masterPg();
    if(!pg || pg != masterPg()) return;

    //Presence mark clean
    for(int iC = 0; iC < statusBar()->children().size(); iC++)
	if((userSt=qobject_cast<UserItStBar*>(statusBar()->children().at(iC))) && userSt->objectName().indexOf("usr_") == 0)
	    userSt->setProperty("usrStPresent", false);

    //Items list parse
    string iLn;
    for(int off = 0, lCnt = 0; (iLn=TSYS::strLine(val,0,&off)).size(); lCnt++) {
	// Parse line in the format: "{id}:{label}:{tip}:{color}:{ico}
	int offIt = 0;
	string	itId  = TSYS::strParse(iLn, 0, ":", &offIt),
		itLab = TSYS::strParse(iLn, 0, ":", &offIt),
		itTip = TSYS::strParse(iLn, 0, ":", &offIt),
		itColor = TSYS::strParse(iLn, 0, ":", &offIt),
		itIco = TSYS::strParse(iLn, 0, ":", &offIt);
	if(itTip.empty()) itTip = itId;
	if(itColor.empty()) itColor = "black";

	// Try presence yet
	userSt = statusBar()->findChild<UserItStBar*>(("usr_"+itId).c_str());
	// Create new one
	if(!userSt) {
	    userSt = new UserItStBar(this);
	    userSt->setObjectName(("usr_"+itId).c_str());
	    userSt->setAlignment(Qt::AlignCenter);
	    statusBar()->insertPermanentWidget(0, userSt);
	}
	// Set properties
	userSt->setProperty("usrStPresent", true);
	userSt->setProperty("usrStPos", lCnt);
	userSt->setToolTip(itTip.c_str());

	QImage ico_t;
	if(!itIco.empty()) {
	    itIco = pg->resGet(itIco);
	    ico_t.loadFromData((const uchar*)itIco.data(), itIco.size());
	}
	userSt->setPixmap(QPixmap::fromImage(ico_t));

	if(itLab.size()) userSt->setText(QString("<font color='%1'>%2</font>").arg(itColor.c_str()).arg(itLab.c_str()));
    }

    //Check for remove and order
    for(int iC = 0; iC < statusBar()->children().size(); iC++)
	if((userSt=qobject_cast<UserItStBar*>(statusBar()->children().at(iC))) && userSt->objectName().indexOf("usr_") == 0) {
	    if(!userSt->property("usrStPresent").toBool())
		//delete userSt;
		userSt->deleteLater();
	    /*else for(int iC1 = iC; iC1 > 0 ; iC1--) {
		if(!(userSt1=qobject_cast<UserItStBar*>(statusBar()->children().at(iC1-1))) ||
		    userSt1->objectName().indexOf("usr_") != 0) continue;
		if(userSt->property("usrStPos").toInt() < userSt1->property("usrStPos").toInt()) userSt1->stackUnder(userSt);
	    }*/
	}
}

void VisRun::initSess( const string &iprjSes_it, bool icrSessForce )
{
    bool isSess = false;
    string openPgs;
    src_prj = work_sess = "";

    //Connect/create session
    int off = 0;
    if((src_prj=TSYS::pathLev(iprjSes_it,0,true,&off)).empty()) return;
    if(off > 0 && off < (int)iprjSes_it.size()) openPgs = iprjSes_it.substr(off);
    // Check for ready session connection or project
    if(src_prj.compare(0,4,"ses_") == 0) { work_sess = src_prj.substr(4); src_prj = ""; isSess = true; }
    else if(src_prj.compare(0,4,"prj_") == 0) src_prj.erase(0,4);
    else return;

    //Get opened sessions list for our page and put dialog for connection
    XMLNode req("list");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("prj",src_prj);
    if(!isSess && !icrSessForce && !cntrIfCmd(req) && req.childSize()) {
	// Prepare and execute a session selection dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("vision_prj_run",NULL,true).c_str())) ico_t.load(":/images/prj_run.png");
	InputDlg conreq(this,QPixmap::fromImage(ico_t),
	    QString(_("Several sessions are already opened for the project \"%1\".\n"
		"You can create a new or connect to the present session. Please select the one you want or click \"Cancel\".")).arg(src_prj.c_str()),
	    _("Selecting a session for connection or a new one creating"), false, false, lang().c_str());
	QListWidget *ls_wdg = new QListWidget(&conreq);
	conreq.edLay()->addWidget(ls_wdg, 0, 0);
	ls_wdg->addItem(_("<Create new session>"));
	for(unsigned iCh = 0; iCh < req.childSize(); iCh++)
	    ls_wdg->addItem(req.childGet(iCh)->text().c_str());
	ls_wdg->setCurrentRow(0);

	if(conreq.exec() == QDialog::Accepted && ls_wdg->currentItem())
	    work_sess = (ls_wdg->currentRow() > 0) ? ls_wdg->currentItem()->text().toStdString() : "";
	else { close(); return; }
    }

    req.clear()->setName("connect")->
		 setAttr("path", "/%2fserv%2fsess")->
		 setAttr("conTm", i2s(mod->restoreTime()*1000));	//Initial for allow the project loading and
									//the session creation on the server side mostly/.
    if(work_sess.empty()) req.setAttr("prj", src_prj);
    else req.setAttr("sess", work_sess);
    if(cntrIfCmd(req)) {
	if(!(conErr && s2i(req.attr("rez")) == TError::Tr_Connect)) {	//Need check for prevent the warning dialog and the run closing by the session creation wait
	    mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	    close();
	}
	return;
    }
    if(req.attr("userIsRoot").size()) setWinMenu(s2i(req.attr("userIsRoot")));
    else setWinMenu(SYS->security().at().access(user(),SEC_WR,"root","root",RWRWR_));

    if(work_sess.empty()) work_sess = req.attr("sess");
    if(src_prj.empty()) src_prj = req.attr("prj");

    mConId = s2i(req.attr("conId"));
    bool toRestore = master_pg;
    if(openPgs.size()) openPgs = "/ses_"+work_sess+openPgs;
    if(toRestore) isResizeManual = true;

    //Prepare group for parameters request and apply
    setWindowTitle(QString(_("Running project: %1")).arg(src_prj.c_str()));	//Default title
    setWindowIcon(mod->icon());
    req.clear()->setName("CntrReqs")->setAttr("path","");
    req.childAdd("get")->setAttr("path","/prj_"+src_prj+"/%2fobj%2fcfg%2fname");
    req.childAdd("get")->setAttr("path","/ses_"+work_sess+"/%2fico");
    req.childAdd("get")->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fper");
    req.childAdd("get")->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fstyle");
    req.childAdd("get")->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fstLst");
    //req.childAdd("get")->setAttr("path","/prj_"+src_prj+"/%2fobj%2fcfg%2fflgs");
    //req.childAdd("openlist")->setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("tm","0")->setAttr("conId", i2s(mConId));
    if(!cntrIfCmd(req)) {
	// Title
	XMLNode *pN = req.childGet(0);
	setWindowTitle(pN->text().c_str());

	// Icon
	pN = req.childGet(1);
	QImage img;
	string simg = TSYS::strDecode(pN->text(), TSYS::base64);
	if(img.loadFromData((const uchar*)simg.data(),simg.size()))
	    setWindowIcon(QPixmap::fromImage(img));

	// Period
	pN = req.childGet(2);
	mPeriod = s2i(pN->text());

	// Style
	pN = req.childGet(3);
	setStyle(s2i(pN->text()));
	pN = req.childGet(4);
	mStlBar->setVisible(style() >= 0 && pN->childSize() > 1);
	//if(style() < 0 && pN->childSize() <= 1) mStlBar->setVisible(false);

	// Clean up the previous pages for clean reconnection
	if(toRestore) {
	    pgCacheClear();
	    openPgs = "";
	    for(unsigned iP = 0; iP < pgList.size(); iP++) {
		RunPageView *pg = master_pg->findOpenPage(pgList[iP]);
		if(pg) { pg->deleteLater(); openPgs += pgList[iP] + ";"; }
	    }
	    ((QScrollArea *)centralWidget())->takeWidget();
	    master_pg->deleteLater();
	    master_pg = NULL;
	    focusWdf = "";
	    setXScale(1); setYScale(1);
	}

	// Open pages list
	//pN = req.childGet(5);
	pgList.clear();
	/*for(unsigned iCh = 0; iCh < pN->childSize(); iCh++) {
	    pgList.push_back(pN->childGet(iCh)->text());
	    callPage(pN->childGet(iCh)->text());
	}*/
	reqtm = 0;	//strtoul(pN->attr("tm").c_str(), NULL, 10);

	//Open direct-selected page or openned before ones
	if(openPgs.size()) {
	    req.clear()->setName("CntrReqs")->setAttr("path", "/ses_"+work_sess);
	    string pIt;
	    for(int off = 0; (pIt=TSYS::strParse(openPgs,0,";",&off)).size(); )
		req.childAdd("open")->setAttr("path","/%2fserv%2fpg")->setAttr("pg",pIt);
	    if(!cntrIfCmd(req)) openPgs = "";
	}
    }

    //QCoreApplication::processEvents();

    //Start timer
    updTmMax = planePer = 0;
    updateTimer->start(/*period()*/);
}

void VisRun::fullUpdatePgs( )
{
    isResizeManual = true;

    for(unsigned iP = 0; iP < pgList.size(); iP++) {
	RunPageView *pg = master_pg->findOpenPage(pgList[iP]);
	if(pg) pg->update(true);
    }
}

string VisRun::lang( )
{
    return Mess->langCode(user(), true);

    //try { return SYS->security().at().usrAt(user()).at().lang(); } catch(...) { }
    //return "";
}

void VisRun::messUpd( )
{
    qApp->setProperty("lang", lang().c_str());	//For the Qt internal messages translation

    //setWindowTitle(_("Vision runtime"));

    //Close
    actClose->setText(_("&Close"));
    actClose->setToolTip(_("Close the Vision window"));
    actClose->setWhatsThis(_("The button for closing the Vision runtime window"));
    actClose->setStatusTip(_("Press to close of the current Vision runtime window."));
    //Quit
    actQuit->setText(_("&Quit"));
    actQuit->setToolTip(_("Quit the program"));
    actQuit->setWhatsThis(_("The button for complete quit the program"));
    actQuit->setStatusTip(_("Press for complete quit the program."));
    //Full screen
    actFullScr->setText(_("Full screen"));
    actFullScr->setToolTip(_("Full screen mode toggling"));
    actFullScr->setWhatsThis(_("The button for full screen mode toggling"));
    actFullScr->setStatusTip(_("Press for toggling the full screen mode."));
    //About
    actAbout->setText(_("&About"));
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button of the information of the program and OpenSCADA"));
    actAbout->setStatusTip(_("Press for information of the program and OpenSCADA."));
    //About Qt
    actQtAbout->setText(_("About &Qt"));
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using Qt information"));
    actQtAbout->setStatusTip(_("Press for getting the using Qt information."));

    //Menus
    menuFile.setTitle(_("&File"));
    menuAlarm.setTitle(_("&Alarm"));
    menuView.setTitle(_("&View"));
    menuHelp.setTitle(_("&Help"));

    //Menu "Print"
    menuPrint->setTitle(_("&Print"));
    menuPrint->menuAction()->setToolTip(_("Print the master page"));
    menuPrint->menuAction()->setWhatsThis(_("The button for printing the master page, by default"));
    menuPrint->menuAction()->setStatusTip(_("Press for printing the master page, by default."));
    actPrintPg->setText(_("Page"));
    actPrintPg->setToolTip(_("Print the selected page"));
    actPrintPg->setWhatsThis(_("The button for printing the selected page"));
    actPrintPg->setStatusTip(_("Press for printing the selected page."));
    actPrintDiag->setText(_("Diagram"));
    actPrintDiag->setToolTip(_("Print the selected diagram"));
    actPrintDiag->setWhatsThis(_("The button for printing the selected diagram"));
    actPrintDiag->setStatusTip(_("Press for printing the selected diagram."));
    actPrintDoc->setText(_("Document"));
    actPrintDoc->setToolTip(_("Print the selected document"));
    actPrintDoc->setWhatsThis(_("The button for printing the selected document"));
    actPrintDoc->setStatusTip(_("Press for printing the selected document."));

    //Menu "Export"
    menuExport->setTitle(_("&Export"));
    menuExport->menuAction()->setToolTip(_("Export the master page"));
    menuExport->menuAction()->setWhatsThis(_("The button for exporting the master page, by default"));
    menuExport->menuAction()->setStatusTip(_("Press for exporting the master page, by default."));
    actExpPg->setText(_("Page"));
    actExpPg->setToolTip(_("Export the selected page"));
    actExpPg->setWhatsThis(_("The button for exporting the selected page"));
    actExpPg->setStatusTip(_("Press for exporting the selected page."));
    actExpDiag->setText(_("Diagram"));
    actExpDiag->setToolTip(_("Export the selected diagram"));
    actExpDiag->setWhatsThis(_("The button for exporting the selected diagram"));
    actExpDiag->setStatusTip(_("Press for exporting the selected diagram."));
    actExpDoc->setText(_("Document"));
    actExpDoc->setToolTip(_("Export the selected document"));
    actExpDoc->setWhatsThis(_("The button for exporting the selected document"));
    actExpDoc->setStatusTip(_("Press for exporting the selected document."));
    actExpTable->setText(_("Table"));
    actExpTable->setToolTip(_("Export the selected table"));
    actExpTable->setWhatsThis(_("The button for exporting the selected table"));
    actExpTable->setStatusTip(_("Press for exporting the selected table."));

    //Project's manual
    actProjManual->setText(QString(_("Project '%1' manual")).arg(srcProject().c_str()));
    actProjManual->setProperty("doc", srcProject().c_str());
    actProjManual->setWhatsThis(QString(_("The button for getting the using project '%1' manual")).arg(srcProject().c_str()));
    actProjManual->setStatusTip(QString(_("Press for getting the using project '%1' manual.")).arg(srcProject().c_str()));
    //Module manual
    actManual->setText(QString(_("Manual on '%1'")).arg(mod->modId().c_str()));
    actManual->setProperty("doc", "Modules/Vision|Modules/Vision");
    actManual->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press for getting the using '%1' manual.")).arg(mod->modId().c_str()));
    //OpenSCADA manual
    actManualSYS->setText(QString(_("Manual on '%1'")).arg(PACKAGE_STRING));
    actManualSYS->setProperty("doc", "index|Documents");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press for getting the using '%1' manual.")).arg(PACKAGE_STRING));
    //What is
    actWhatIs->setText(_("What's &This"));
    actWhatIs->setToolTip(_("The button for requesting the information about GUI elements"));
    actWhatIs->setWhatsThis(_("Requesting the information about GUI elements"));
    actWhatIs->setStatusTip(_("Press for requesting the information about GUI elements."));

    //Alarm level display button and full alarms quietance
    actAlrmLev->setText(_("Alarm level"));
    actAlrmLev->setToolTip(_("Alarm level"));
    actAlrmLev->setWhatsThis(_("The button for quietance all alarms"));
    actAlrmLev->setStatusTip(_("Press for quietance all alarms."));

    //Generic tools bar
    toolBarStatus->setWindowTitle(_("General (status)"));

    //Init status bar
    mWTime->setWhatsThis(_("This label shows the current system time."));
    mWUser->setWhatsThis(_("This label displays the current user."));
    mWUser->setToolTip(_("Field for displaying the current user."));
    mWUser->setStatusTip(_("Double click to change the user."));
    mWStat->setWhatsThis(_("This label displays the used VCA engine station."));
    mWStat->setToolTip(_("Field for displaying the used VCA engine station."));
    mStlBar->setWhatsThis(_("This label displays the used interface style."));
    mStlBar->setToolTip(_("Field for displaying the used interface style."));
    mStlBar->setStatusTip(_("Double click to change the style."));

    //Style update
    setStyle(style());
}

void VisRun::callPage( const string& pg_it, bool updWdg )
{
    vector<int> idst;
    string stmp;

    //Scan and update opened page
    if(master_pg) {
	RunPageView *pg = master_pg->findOpenPage(pg_it);
	if(pg) {
	    if(!(wPrcCnt%(5000/vmin(5000,period()))))	pg->update(false, NULL, true);
	    else if(updWdg) pg->update(false);
	    return;
	}
    }

    //Placing all needed atrributes to one request
    XMLNode req("CntrReqs"), *chN; req.setAttr("path", pg_it);
    req.childAdd("get")->setAttr("path", "/%2fattr%2fpgGrp");
    req.childAdd("get")->setAttr("path", "/%2fattr%2fpgOpenSrc");
    // For per-page notification
    for(unsigned iNtf = 0; iNtf < 8; iNtf++) {
	req.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fnotifyVis"+mod->modId()+i2s(iNtf));
	req.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fnotify"+i2s(iNtf));
    }
    cntrIfCmd(req);

    //Get group and parent page
    string pgGrp = (chN=req.getElementBy("path","/%2fattr%2fpgGrp")) ? chN->text() : "";	//wAttrGet(pg_it, "pgGrp");
    string pgSrc = (chN=req.getElementBy("path","/%2fattr%2fpgOpenSrc")) ? chN->text() : "";	//wAttrGet(pg_it, "pgOpenSrc");

    //Check for master page replace
    if(!master_pg || pgGrp == "main" || pgGrp.empty() || master_pg->pgGrp() == pgGrp) {
	// Send close command
	if(master_pg) {
	    XMLNode req("close"); req.setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("pg",master_pg->id());
	    cntrIfCmd(req);
	    //!!!! Without the next rows the master page removing performs just into setWidget() of the scroll area
	    ((QScrollArea*)centralWidget())->takeWidget();
	    master_pg->deleteLater();
	}

	// Get and activate for specific attributes to the master-page
	XMLNode reqSpc("CntrReqs"); reqSpc.setAttr("path", pg_it);
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fstatLine")->
				     setAttr("aNm", _("Status line items"))->
				     setAttr("aTp", i2s(TFld::String))->setAttr("aFlg", i2s(TFld::FullText));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2frunWin")->
				     setAttr("aNm", _("Running window"))->
				     setAttr("aTp", i2s(TFld::Integer))->setAttr("aFlg", i2s(TFld::Selectable))->
				     setAttr("aVls", "0;1;2")->setAttr("aNms", _("Original size;Maximize;Full screen"));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fkeepAspectRatio")->
				     setAttr("aNm", _("Keep aspect ratio on the scaling"))->
				     setAttr("aTp", i2s(TFld::Boolean))->
				     setText("1");
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fstBarNoShow")->
				     setAttr("aNm", _("Do not show the status bar"))->
				     setAttr("aTp", i2s(TFld::Boolean));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fwinPosCntrSave")->
				     setAttr("aNm", _("Control and save window positions"))->
				     setAttr("aTp", i2s(TFld::Boolean));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fuserSetVis");
	cntrIfCmd(reqSpc);

	// Create widget view
	master_pg = new RunPageView(pg_it, this, centralWidget());
	conErr = NULL;			//possible a connection error status clean up
	focusWdf = "";
	//master_pg->load("");
	master_pg->setFocusPolicy(Qt::StrongFocus);
	((QScrollArea *)centralWidget())->setWidget(master_pg);

	if(windowState()&(Qt::WindowFullScreen|Qt::WindowMaximized))	x_scale = y_scale = 1.0;
	resizeEvent(NULL);

	/*if(!(windowState()&(Qt::WindowFullScreen|Qt::WindowMaximized))) {
	    QRect ws = QApplication::desktop()->availableGeometry(this);
	    resize(vmin(master_pg->size().width()+10,ws.width()-10), vmin(master_pg->size().height()+55,ws.height()-10));
	}
	else {
	    x_scale = y_scale = 1.0;
	    resizeEvent(NULL);
	}*/
    }
    //Put to check for include
    else master_pg->callPage(pg_it, pgGrp, pgSrc);
}

void VisRun::pgCacheClear( )
{
    while(!cachePg.empty()) {
	cachePg.front()->deleteLater();	//delete cachePg.front();
	cachePg.pop_front();
    }
}

void VisRun::pgCacheAdd( RunPageView *wdg )
{
    if(!wdg) return;
    cachePg.push_front(wdg);
    while(mod->cachePgSz() && (int)cachePg.size() > mod->cachePgSz()) {
	cachePg.back()->deleteLater();	//delete cachePg.back();
	cachePg.pop_back();
    }
}

RunPageView *VisRun::pgCacheGet( const string &id )
{
    RunPageView *pg = NULL;

    for(unsigned iPg = 0; iPg < cachePg.size(); iPg++)
	if(cachePg[iPg]->id() == id) {
	    pg = cachePg[iPg];
	    cachePg.erase(cachePg.begin()+iPg);
	    break;
	}

    return pg;
}

RunPageView *VisRun::findOpenPage( const string &pg )
{
    if(!master_pg) return NULL;

    return master_pg->findOpenPage(pg);
}

RunWdgView *VisRun::findOpenWidget( const string &wdg )
{
    int woff = 0;
    for(int off = 0; true; woff = off) {
	string sel = TSYS::pathLev(wdg, 0, true, &off);
	if(sel.empty() || sel.substr(0,4) == "wdg_")	break;
    }
    RunPageView *rpg = findOpenPage(wdg.substr(0,woff));
    if(!rpg)	return NULL;
    if(woff >= (int)wdg.size())	return rpg;

    return rpg->findOpenWidget(wdg);
}

string VisRun::wAttrGet( const string &path, const string &attr, bool sess )
{
    XMLNode req("get");
    if(sess) req.setAttr("path",path+"/%2fserv%2fattrSess%2f"+attr);
    else req.setAttr("path",path+"/%2fattr%2f"+attr);
    return cntrIfCmd(req) ? "" : req.text();
}

bool VisRun::wAttrSet( const string &path, const string &attr, const string &val, bool sess )
{
    XMLNode req("set");
    if(sess) req.setAttr("path",path+"/%2fserv%2fattrSess%2f"+attr)->setText(val);
    else req.setAttr("path",path+"/%2fserv%2fattr")->
	    childAdd("el")->setAttr("id",attr)->setText(val);
    return !cntrIfCmd(req);
}

void VisRun::alarmSet( unsigned alarm )
{
    unsigned ch_tp = alarm^mAlrmSt;

    //Check for early this session running equalent project
    bool isMaster = true;
    MtxAlloc res(mod->dataRes(), true);
    for(unsigned iW = 0; iW < mod->mnWinds.size(); iW++)
	if(qobject_cast<VisRun*>(mod->mnWinds[iW]) && ((VisRun*)mod->mnWinds[iW])->srcProject() == srcProject()) {
	    if(((VisRun*)mod->mnWinds[iW])->workSess() != workSess()) isMaster = false;
	    break;
	}
    res.unlock();

    //Set notify to the alarm
    for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); isMaster && iN != mNotify.end(); ++iN) iN->second->ntf(alarm);
    for(int iAl = 0; iAl < menuAlarm.actions().size(); ++iAl) {
	QAction *cO = menuAlarm.actions()[iAl];
	if(!cO || cO->objectName().toStdString().compare(0,8,"alarmNtf") != 0)	continue;
	unsigned nTp = s2i(cO->objectName().toStdString().substr(8));
	bool newSt;
	if((ch_tp>>8)&(1<<nTp) && (newSt=(alarm>>8)&(1<<nTp)) != cO->isVisible()) cO->setVisible(newSt);
	if((ch_tp>>16)&(1<<nTp)) {
	    newSt = (alarm>>16)&(1<<nTp);
	    if(cO->property("quietanceRet").toBool() && !newSt != cO->isChecked()) cO->setChecked(!newSt);
	    if(!cO->property("quietanceRet").toBool() && newSt != cO->isEnabled()) cO->setEnabled(newSt);
	}
    }

    //Alarm action indicators update
    // Alarm level icon update
    if(ch_tp&0xFF || (alarm>>16)&ntfSet /*|| (alarm>>16)&(TVision::Light|TVision::Alarm|TVision::Sound)*/ || !alrLevSet) {
	int alarmLev = alarm&0xFF;
	actAlrmLev->setToolTip(QString(_("Alarm level: %1")).arg(alarmLev));

	QImage lens(":/images/alarmLev.png");
	QImage levImage(lens.size(), lens.format());

	QPainter painter(&levImage);
	//QColor lclr( alarmLev ? 224 : 0, alarmLev ? 224-(int)(0.87*alarmLev) : 224, 0 );
	QColor lclr(alarmLev ? 255 : 0, alarmLev ? 255-alarmLev : 255, 0);

	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(levImage.rect(),Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	if(!((alarm>>16)&ntfSet && alrLevSet)) {
	    for(int iX = 0; iX < lens.size().width(); iX++)
		for(int iY = 0; iY < lens.size().height(); iY++)
		    if(lens.pixel(iX,iY)&0xFF000000)
			levImage.setPixel(iX, iY, lclr.rgba());
	    alrLevSet = true;
	} else alrLevSet = false;

	painter.drawImage(0, 0, lens);
	painter.end();
	actAlrmLev->setIcon(QPixmap::fromImage(levImage));
    }

    mAlrmSt = alarm;
}

void VisRun::ntfReg( int8_t tp, const string &props, const string &pgCrtor, bool prior )
{
    if(tp < 0) {
	for(unsigned iNtf = 0; iNtf < 8; iNtf++)
	    ntfReg(iNtf, props, pgCrtor, prior);
	return;
    }

    vector<string> pgPropsQ;

    //Search for presented notification type
    map<uint8_t,Notify*>::iterator iN = mNotify.find(tp);
    if(iN != mNotify.end()) {
	if(pgCrtor == iN->second->pgCrtor() && (props == iN->second->props() || !prior)) return;
	pgPropsQ = iN->second->pgPropsQ;
	if(pgCrtor != iN->second->pgCrtor()) {
	    // Check the queue for the page already here
	    for(vector<string>::iterator iQ = iN->second->pgPropsQ.begin(); iQ != iN->second->pgPropsQ.end(); ++iQ)
		if(TSYS::strLine(*iQ,0) == pgCrtor) {
		    if(props.empty()) iN->second->pgPropsQ.erase(iQ);
		    else *iQ = pgCrtor + "\n" + props;
		    return;
		}
	    if(props.empty()) return;
	    pgPropsQ.push_back(iN->second->pgProps);
	}
	delete iN->second;
	mNotify.erase(iN);
	ntfSet &= ~(1<<tp);
    }
    //Creation new or replacing present one
    if(props.size())		mNotify[tp] = new Notify(tp, pgCrtor+"\n"+props, this);
    //Take and place a notificator from the queue
    else if(pgPropsQ.size())	{ mNotify[tp] = new Notify(tp, pgPropsQ.back(), this); pgPropsQ.pop_back(); }
    else return;

    mNotify[tp]->pgPropsQ = pgPropsQ;
    ntfSet |= (1<<tp);
}

string VisRun::cacheResGet( const string &res )
{
    map<string,CacheEl>::iterator ires = mCacheRes.find(res);
    if(ires == mCacheRes.end()) return "";
    ires->second.tm = SYS->sysTm();
    return ires->second.val;
}

void VisRun::cacheResSet( const string &res, const string &val )
{
    if(val.size() > limUserFile_SZ) return;
    mCacheRes[res] = CacheEl(SYS->sysTm(), val);
    if(mCacheRes.size() > (limCacheIts_N+limCacheIts_N/10)) {
	vector< pair<time_t,string> > sortQueue;
	for(map<string,CacheEl>::iterator itr = mCacheRes.begin(); itr != mCacheRes.end(); ++itr)
	    sortQueue.push_back(pair<time_t,string>(itr->second.tm,itr->first));
	sort(sortQueue.begin(), sortQueue.end());
	for(unsigned i_del = 0; i_del < (limCacheIts_N/10); ++i_del) mCacheRes.erase(sortQueue[i_del].second);
    }
}

void VisRun::updatePage( )
{
    if(winClose || updPage) return;

    int rez;
    int64_t d_cnt = TSYS::curTime();

    updPage = true;

    //Pages update
    XMLNode req("openlist");
    req.setAttr("tm", u2s(reqtm))->setAttr("conId", i2s(mConId))->
	setAttr("path", "/ses_"+work_sess+"/%2fserv%2fpg");

    if(!(rez=cntrIfCmd(req,false,true))) {
	// Check for delete the pages
	RunPageView *pg;
	for(unsigned iP = 0, iCh; iP < pgList.size(); iP++) {
	    for(iCh = 0; iCh < req.childSize(); iCh++)
		if(pgList[iP] == req.childGet(iCh)->text())
		    break;
	    if(iCh < req.childSize() || !(master_pg && (pg=master_pg->findOpenPage(pgList[iP])))) continue;
	    if(!pg->property("cntPg").toString().isEmpty())
		((RunWdgView*)TSYS::str2addr(pg->property("cntPg").toString().toStdString()))->setPgOpenSrc("");
	    else {
		if(pg != master_pg) {
		    if(pg->isWindow()) pg->close();
		    pg->deleteLater();
		}
		else {
		    ((QScrollArea*)centralWidget())->setWidget(new QWidget());
		    master_pg = NULL;
		    conErr = NULL;		//possible a connection error status clean up
		    focusWdf = "";
		}
	    }
	}

	// Process the opened pages
	pgList.clear();
	for(unsigned iCh = 0, iCh2 = 0; iCh < req.childSize(); ++iCh) {
	    XMLNode *chN = req.childGet(iCh);
	    pgList.push_back(chN->text());

	    // Detection the double container pages and opening the last one
	    if(chN->attr("pgGrp").size() && chN->attr("pgGrp") != "main" && chN->attr("pgGrp") != "fl") {
		for(iCh2 = iCh+1; iCh2 < req.childSize(); ++iCh2)
		    if(req.childGet(iCh2)->attr("pgGrp") == chN->attr("pgGrp"))
			break;
		if(iCh2 < req.childSize()) {
		    //  Force closing lost opened and included pages
		    XMLNode reqClose("close"); reqClose.setAttr("path","/ses_"+workSess()+"/%2fserv%2fpg")->setAttr("pg", chN->text());
		    cntrIfCmd(reqClose);
		    continue;
		}
	    }

	    // Opening
	    callPage(chN->text(), s2i(chN->attr("updWdg")));
	}
    }
    // Restore closed session of used project.
    else if(rez == 2) {
	mess_warning(mod->nodePath().c_str(),_("Restore the session creation for '%s'."),prjSes_it.c_str());
	updateTimer->stop();
	initSess(prjSes_it, crSessForce);
	updPage = false;
	return;
    }

    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //Alarms update (0.5 second update)
    if((alrmUpdCnt+=1e-3*planePer) > TM_ALRM_UPD) {
	alrmUpdCnt = 0;
	// Get the alarm status
	unsigned wAlrmSt = alarmSt();
	req.clear()->
	    setName("get")->
	    setAttr("mode", "stat")->
	    setAttr("path", "/ses_"+work_sess+"/%2fserv%2falarm");
	if(!cntrIfCmd(req,false,true)) wAlrmSt = s2i(req.attr("alarmSt"));

	// Set the alarm
	alarmSet(wAlrmSt);
    }

    //Calc the planed executing period to correct the main one
    float updTm = 1e-3*(TSYS::curTime()-d_cnt);
    updTmMax = vmax(updTmMax, updTm);
    if(!planePer) planePer = period();
    planePer += (vmax(period(),updTm*3)-planePer)/100;
    if(mess_lev() == TMess::Debug)
	mess_debug(mod->nodePath().c_str(), _("Time of the session updating '%s': %s[%s]ms. Planer period %s(%s)ms"),
	    workSess().c_str(), tm2s(1e-3*updTm).c_str(), tm2s(1e-3*updTmMax).c_str(), tm2s(1e-3*planePer).c_str(), tm2s(1e-3*period()).c_str());
    updateTimer->start(vmax(0,planePer-updTm));

    //Removing from the cache for old pages
    for(unsigned iPg = 0; iPg < cachePg.size(); )
	if(mod->cachePgLife() > 0.01 && (period()*(reqTm()-cachePg[iPg]->reqTm())/1000) > (unsigned)(mod->cachePgLife()*60*60)) {
	    cachePg[iPg]->deleteLater();	//delete cachePg[iPg];
	    cachePg.erase(cachePg.begin()+iPg);
	} else iPg++;

    //Time update
    if(mWTime->isVisible() && !(wPrcCnt%vmax(1000/vmin(1000,period()),1))) {
	QDateTime dtm = QDateTime::currentDateTime();
	mWTime->setText(locale().toString(dtm,"hh:mm:ss\nddd, d MMM"));
	mWTime->setToolTip(locale().toString(dtm,"dddd, dd-MMM-yyyy"));
    }

    //Scale for full screen check
    if(centralWidget() && master_pg && !(wPrcCnt%vmax(1000/vmin(1000,period()),1)) &&
	(windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen))
    {
	float xSc = (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	float ySc = (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	if(xSc < 1 || ySc < 1) {
	    x_scale *= xSc;
	    y_scale *= ySc;
	    //Proportional scale
	    if(keepAspectRatio()) x_scale = y_scale = vmin(x_scale,y_scale);

	    fullUpdatePgs();
	}
    }

    wPrcCnt++;
    updPage = isResizeManual = false;
}

#undef _
#define _(mess) mod->I18N(mess, owner()->lang().c_str()).c_str()

//* Notify: Generic notifying object.		 *
//************************************************
VisRun::Notify::Notify( uint8_t itp, const string &ipgProps, VisRun *iown ) : pgProps(ipgProps),
    tp(itp), alSt(0xFFFFFFFF), repDelay(-1), comIsExtScript(false), f_notify(false), f_resource(false), f_queue(false), f_quietanceRet(false),
    toDo(false), alEn(false), delay(0), queueCurTm(0), dataM(true), mOwner(iown), actAlrm(NULL), ntfPlay(NULL)
{
    //Parsing the properties
    string iLn, iOpt, ico, name, iProps = props();
    bool hasLang  = false, hasFlags = false;
    for(int off = 0, lCnt = 0, fPos; (!hasLang || !hasFlags || ico.empty() || name.empty()) && (iLn=TSYS::strLine(iProps,0,&off)).size(); lCnt++)
	if(!hasLang && !lCnt && iLn.find("#!") == 0) { hasLang = comIsExtScript = true; continue; }
	else if(!hasFlags && (size_t)(fPos=iLn.find("flags=")) != string::npos) {
	    for(fPos += 6; (iOpt=TSYS::strParse(iLn,0,"|",&fPos)).size(); )
		if(iOpt.compare(0,6,"notify") == 0) {
		    f_notify = true;
		    repDelay = (iOpt.size() > 6) ? vmax(0,vmin(100,atoi(iOpt.c_str()+6))) : -1;
		}
		else if(iOpt == "resource")	f_resource = true;
		else if(iOpt == "queue")	{ f_queue = true; if(repDelay < 0) repDelay = 0; }
		else if(iOpt == "quietanceRet")	f_quietanceRet = true;
	    hasFlags = true;
	}
	else if(ico.empty() && (size_t)(fPos=iLn.find("ico=")) != string::npos)	  ico = iLn.substr(fPos+4);
	else if(name.empty() && (size_t)(fPos=iLn.find("name=")) != string::npos) name = iLn.substr(fPos+5);

#ifdef HAVE_PHONON
    if(f_notify && (f_queue || f_resource))
	ntfPlay = new VideoPlayer(Phonon::MusicCategory);
#endif

    //The command procedure prepare
    if(comIsExtScript) {
	// Prepare the external script
	comProc = "sesRun_"+owner()->workSess()+"_ntf"+i2s(tp);
	bool fOK = false;
	int hd = open(comProc.c_str(), O_CREAT|O_TRUNC|O_WRONLY, SYS->permCrtFiles(true));
	if(hd >= 0) {
	    fOK = write(hd, props().data(), props().size()) == (ssize_t)props().size();
	    if(::close(hd) != 0)
		mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), hd, strerror(errno), errno);
	}
	if(!fOK) {
	    mess_err((mod->nodePath()+"/sesRun_"+owner()->workSess()).c_str(), _("Error function of the notificator '%s': %s"), comProc.c_str(), strerror(errno));
	    comProc = "";
	}
    }
    else {
	// Prepare an internal procedure
	TFunction funcIO("sesRun_"+owner()->workSess()+"_ntf"+i2s(tp));
	//funcIO.setStor(DB());
	funcIO.ioIns(new IO("en",trS("Enabled notification"),IO::Boolean,IO::Default), IFA_en);
	funcIO.ioIns(new IO("doNtf",trS("Performing the notification, always 1"),IO::Boolean,IO::Default), IFA_doNtf);
	funcIO.ioIns(new IO("doRes",trS("Making the resource, always 0"),IO::Boolean,IO::Default), IFA_doRes);
	funcIO.ioIns(new IO("res",trS("Resource stream"),IO::String,IO::Output), IFA_res);
	funcIO.ioIns(new IO("mess",trS("Notification message"),IO::String,IO::Default), IFA_mess);
	funcIO.ioIns(new IO("lang",trS("Language of the notification message"),IO::String,IO::Default), IFA_lang);
	funcIO.ioIns(new IO("resTp",trS("Resource stream type"),IO::String,IO::Return), IFA_resTp);
	funcIO.ioIns(new IO("prcID",trS("Procedure ID"),IO::String,IO::Default), IFA_prcID);
	try { comProc = SYS->daq().at().at("JavaLikeCalc").at().compileFunc("JavaScript", funcIO, props()); }
	catch(TError &er) {
	    mess_err((mod->nodePath()+"/sesRun_"+owner()->workSess()).c_str(), _("Error function of the notificator '%s': %s"),
		    funcIO.id().c_str(), er.mess.c_str());
	}
    }

    if(f_notify && !ntfPlay) {
	//Call conditional variable init
	pthread_cond_init(&callCV, NULL);

	//Notification task create
	SYS->taskCreate(mod->nodePath('.',true)+".sesRun_"+owner()->workSess()+".ntf"+i2s(tp), 0, VisRun::Notify::Task, this);
    }

    //The quietance action prepare and place
    QImage ico_t;
    if(!ico.empty()) {
	ico = owner()->masterPg()->resGet(ico);
	ico_t.loadFromData((const uchar*)ico.data(), ico.size());
    }
    if(ico_t.isNull() && !ico_t.load(TUIS::icoGet("alarmAlarm",NULL,true).c_str())) ico_t.load(":/images/alarmAlarm.png");
    if(name.empty()) name = TSYS::strMess(_("Notificator %d"), tp);
    actAlrm = new QAction(QPixmap::fromImage(ico_t), name.c_str(), owner());
    actAlrm->setObjectName(("alarmNtf"+i2s(tp)).c_str());
    //actAlrm->setToolTip(_("Blink alarm"));
    if(f_queue) {
	actAlrm->setWhatsThis(QString(_("The button for quietance the current \"%1\"")).arg(name.c_str()));
	actAlrm->setStatusTip(QString(_("Press for quietance the current \"%1\".")).arg(name.c_str()));
    }
    else {
	actAlrm->setWhatsThis(QString(_("The button for quietance all \"%1\"")).arg(name.c_str()));
	actAlrm->setStatusTip(QString(_("Press for quietance all \"%1\".")).arg(name.c_str()));
    }
    actAlrm->setProperty("quietanceRet", (bool)f_quietanceRet);
    actAlrm->setCheckable(f_quietanceRet);
    owner()->menuAlarm.addAction(actAlrm);
    owner()->toolBarStatus->addAction(actAlrm);

    //Apply to the current alarm status
    actAlrm->setVisible((owner()->alarmSt()>>8)&(1<<tp));
    if(f_quietanceRet) actAlrm->setChecked(!((owner()->alarmSt()>>16)&(1<<tp)));
    else actAlrm->setEnabled((owner()->alarmSt()>>16)&(1<<tp));

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:Notify", 1);
}

VisRun::Notify::~Notify( )
{
    if(f_notify && !ntfPlay) {
	SYS->taskDestroy(mod->nodePath('.',true)+".sesRun_"+owner()->workSess()+".ntf"+i2s(tp), NULL, 60, false, &callCV);
	pthread_cond_destroy(&callCV);
    }
#ifdef HAVE_PHONON
    if(ntfPlay) { delete ntfPlay; ntfPlay = NULL; }
#endif

    //The resource file remove
    if(resFile.size()) remove(resFile.c_str());

    //The command procedure remove
    if(comIsExtScript && comProc.size()) remove(comProc.c_str());

    //The quietance action remove
    if(actAlrm) actAlrm->deleteLater();
    actAlrm = NULL;

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:Notify", -1);
}

string	VisRun::Notify::pgCrtor( )	{ return TSYS::strLine(pgProps, 0); }

string	VisRun::Notify::props( )
{
    int off = 0;
    TSYS::strLine(pgProps, 0, &off);
    return pgProps.substr(off);
}

string VisRun::Notify::curQueueWdg( )
{
    if(!hasQueue()) return "";
    pthread_mutex_lock(&dataM.mtx());
    string rez = queueCurPath;
    pthread_mutex_unlock(&dataM.mtx());

    return rez;
}

void VisRun::Notify::ntf( int ialSt )
{
    alEn = (bool)((ialSt>>16)&(1<<tp));

#ifdef HAVE_PHONON
    if(ntfPlay) {
	State mSt = ((VideoPlayer*)ntfPlay)->mediaObject()->state();
	bool plSt = (mSt == LoadingState || mSt == BufferingState || mSt == PlayingState);
	if(!alEn && ((VideoPlayer*)ntfPlay)->mediaObject()->currentSource().type() != MediaSource::Empty)
	    ((VideoPlayer*)ntfPlay)->load(MediaSource());
	else if(alEn && (((VideoPlayer*)ntfPlay)->mediaObject()->currentSource().type() == MediaSource::Empty ||
			    (repDelay >= 0 && !plSt && (delay-=vmax(TM_ALRM_UPD,1e-3*owner()->planePer)) <= 0))) {
	    string nRes, nResTp, nMess, nLang;
	    nRes = ntfRes(nResTp, nMess, nLang);
	    commCall(nRes, nResTp, nMess, nLang);
	    delay = repDelay;
	}
	return;
    } else
#endif
    //Check for the alarm state change
    if(f_notify && (((ialSt^alSt)>>16)&(1<<tp))) {
	pthread_mutex_lock(&dataM.mtx());
	toDo = true;
	pthread_cond_signal(&callCV);
	pthread_mutex_unlock(&dataM.mtx());

	alSt = ialSt;
    }
}

string VisRun::Notify::ntfRes( string &resTp, string &mess, string &lang )
{
    string rez;
    mess = lang = resTp = "";

    //Call same request to the VCA server for resources
    XMLNode req("get");
    req.setAttr("path", "/ses_"+owner()->workSess()+"/%2fserv%2falarm")->
	setAttr("mode", "resource")->
	setAttr("tp", i2s(tp))->
	setAttr("tm", u2s(queueCurTm))->
	setAttr("wdg", queueCurPath);
    //if(!owner()->cntrIfCmd(req)) {
    if(!mod->cntrIfCmd(req,owner()->user(),owner()->password(),owner()->VCAStation())) {
	queueCurTm = strtoul(req.attr("tm").c_str(), NULL, 10);
	queueCurPath = req.attr("wdg");
	rez = TSYS::strDecode(req.text(), TSYS::base64);
	mess = req.attr("mess");
	lang = req.attr("lang");
	resTp = req.attr("resTp");
    }

    return rez;
}

void VisRun::Notify::commCall( string &res, string &resTp, const string &mess, const string &lang )
{
    if(comProc.empty()) return;

    //Shared data obtain
    pthread_mutex_lock(&dataM.mtx());
    string wcomProc = comProc;
    pthread_mutex_unlock(&dataM.mtx());

    //Loading the resource
    if(ntfPlay || comIsExtScript) {
	resFile = "sesRun_"+owner()->workSess()+"_res"+i2s(tp);
	int hdRes = res.size() ? open(resFile.c_str(), O_CREAT|O_TRUNC|O_WRONLY, SYS->permCrtFiles()) : -1;
	if(hdRes >= 0) {
	    write(hdRes, res.data(), res.size());
	    if(::close(hdRes) != 0)
		mess_warning(mod->nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), hdRes, strerror(errno), errno);
	}
	else resFile = "";
    }

    //Playing by an internal mechanism (Phonon)
#ifdef HAVE_PHONON
    if(ntfPlay)	((VideoPlayer*)ntfPlay)->play(MediaSource(QUrl(resFile.c_str())));
    else
#endif
    //Call external procedures
    if(comIsExtScript)
	// Prepare environment and execute the external script
	system(("prcID=sesRun_"+owner()->workSess()+"_ntf"+i2s(tp)+" en="+i2s(alEn)+" doNtf=1 doRes=0 res="+resFile+" resTp="+resTp+
	    " mess=\""+TSYS::strEncode(mess,TSYS::SQL)+"\" lang=\""+TSYS::strEncode(lang,TSYS::SQL)+"\" ./"+wcomProc).c_str());
    else {
	// Prepare and execute internal procedure
	TValFunc funcV;
	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(wcomProc)).at());

	//  Load inputs
	funcV.setB(IFA_en, alEn);
	funcV.setB(IFA_doNtf, true);
	funcV.setB(IFA_doRes, false);
	funcV.setS(IFA_res, res);
	funcV.setS(IFA_mess, mess);
	funcV.setS(IFA_lang, lang);
	funcV.setS(IFA_resTp, resTp);
	funcV.setS(IFA_prcID, "sesRun_"+owner()->workSess()+"_ntf"+i2s(tp));

	//  Call to processing
	funcV.calc();
    }
}

void *VisRun::Notify::Task( void *intf )
{
    VisRun::Notify &ntf = *(VisRun::Notify*)intf;

    pthread_mutex_lock(&ntf.dataM.mtx());
    while(!TSYS::taskEndRun() || ntf.toDo) {
	if(!ntf.toDo) pthread_cond_wait(&ntf.callCV, &ntf.dataM.mtx());
	if(!ntf.toDo || ntf.comProc.empty()) { ntf.toDo = false; continue; }
	ntf.toDo = false;
	pthread_mutex_unlock(&ntf.dataM.mtx());

	string ntfRes, ntfResTp, ntfMess, ntfLang;
	unsigned delayCnt = 0;
	do {
	    if(delayCnt) { TSYS::sysSleep(1); delayCnt--; continue; }

	    //  Get the resources for the notification
	    if((ntf.f_queue || ntf.f_resource) && ntf.alEn) ntfRes = ntf.ntfRes(ntfResTp, ntfMess, ntfLang);

	    //  Same notification
	    ntf.commCall(ntfRes, ntfResTp, ntfMess, ntfLang);

	    delayCnt = ntf.repDelay;
	} while((ntf.repDelay >= 0 || ntf.f_queue) && ntf.alEn && !TSYS::taskEndRun());

	pthread_mutex_lock(&ntf.dataM.mtx());
    }
    pthread_mutex_unlock(&ntf.dataM.mtx());

    return NULL;
}

//***********************************************
// SHost - Host thread's control object         *
SCADAHost::SCADAHost( QObject *p ) :
    QThread(p), inHostReq(0), endRun(false), reqDone(false), glob(false), req(NULL), done(NULL), pid(0)
{

}

SCADAHost::~SCADAHost( )
{
    endRun = true;
    while(!wait(100)) sendSIGALRM();
}

void SCADAHost::sendSIGALRM( )
{
    if(pid) pthread_kill(pid, SIGALRM);
}

bool SCADAHost::reqDo( XMLNode &node, bool &idone, bool iglob )
{
    if(req) return false;

    //Set the request
    mtx.lock();
    reqDone = false;
    glob = iglob;
    req = &node;
    done = &idone; *done = false;
    cond.wakeOne();
    cond.wait(mtx, 10);
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
    pid = pthread_self();

    while(!endRun) {
	//Interface's requests processing
	mtx.lock();
	if(!req || reqDone) cond.wait(mtx, 1000);
	if(req && !reqDone) {
	    mtx.unlock();
	    mod->cntrIfCmd(*req, owner()->user(), owner()->password(), owner()->VCAStation(), glob);
	    mtx.lock();
	    reqDone = *done = true;
	    cond.wakeOne();
	}
	mtx.unlock();
    }
}

VisRun *SCADAHost::owner( ) const	{ return dynamic_cast<VisRun*>(parent()); }

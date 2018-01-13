
//OpenSCADA system module UI.Vision file: vis_run.cpp
/***************************************************************************
 *   Copyright (C) 2007-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

// #include <config.h>
#include <tsys.h>
#include "tvision.h"
#include "vis_run_widgs.h"
#include "vis_shapes.h"
#include "vis_run.h"

#undef _
#define _(mess) mod->I18N(mess, lang().c_str())

using namespace VISION;

VisRun::VisRun( const string &iprjSes_it, const string &open_user, const string &user_pass, const string &VCAstat,
		bool icrSessForce, unsigned iScr ) :
    QMainWindow(QDesktopWidget().screen(iScr)), isResizeManual(false),
#ifndef QT_NO_PRINTER
    prPg(NULL), prDiag(NULL), prDoc(NULL),
#endif
    fileDlg(NULL),
    winClose(false), conErr(NULL), crSessForce(icrSessForce), mKeepAspectRatio(true), mWinPosCntrSave(false), prjSes_it(iprjSes_it),
    master_pg(NULL), mPeriod(1000), mConId(0), mScreen(iScr), wPrcCnt(0), reqtm(1), expDiagCnt(1), expDocCnt(1), x_scale(1), y_scale(1),
    mAlrmSt(0xFFFFFF), alrLevSet(false), ntfSet(0)
{
    QImage ico_t;

    connect(this, SIGNAL(makeStarterMenu()), qApp, SLOT(makeStarterMenu()));
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
    //  Alarm level display button and full alarms quittance
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
    toolBarStatus->setIconSize(QSize(16,16));
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
    statusBar()->insertPermanentWidget(0,mWStat);
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
    endRunTimer   = new QTimer(this);
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);
    // Update timer
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(false);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePage()));

    //actProjManual->setEnabled(TUIS::docGet(actProjManual->property("doc").toString().toStdString(),NULL,TUIS::GetFilePath).size());

    resize(600, 400);

    //Init session
    initSess(prjSes_it, crSessForce);

    messUpd();

    //mWStat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000);

    //Restore main window position
    if(winPosCntrSave() && masterPg()) {
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
}

bool VisRun::winMenu( )	{ return menuBar()->actions().length(); }

void VisRun::setWinMenu( bool act )
{
    menuBar()->clear();

    //Create menu
    if(act) {
	menuBar()->addMenu(&menuFile);
	menuBar()->addMenu(&menuAlarm);
	menuBar()->addMenu(&menuView);
	menuBar()->addMenu(&menuHelp);
	emit makeStarterMenu();
    }
}

string VisRun::user( )		{ return mWUser->user(); }

bool VisRun::userSel( const string &hint )	{ return mWUser->userSel(hint); }

string VisRun::password( )	{ return mWUser->pass(); }

string VisRun::VCAStation( )	{ return mWUser->VCAStation(); }

int VisRun::style( )		{ return mStlBar->style(); }

void VisRun::setStyle( int istl )		{ mStlBar->setStyle(istl); }

int VisRun::cntrIfCmd( XMLNode &node, bool glob, bool main )
{
    if(masterPg() && conErr && (!main || (time(NULL)-conErr->property("tm").toLongLong()) < conErr->property("tmRest").toInt())) {
	if(main) conErr->setText(conErr->property("labTmpl").toString().arg(conErr->property("tmRest").toInt()-(time(NULL)-conErr->property("tm").toLongLong())));
	return 10;
    }

    int rez = mod->cntrIfCmd(node, user(), password(), VCAStation(), glob);
    //Display error message about connection error
    if(rez == 10 && main && masterPg()) {
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
		QString(_("Connection to visualization server '%1' error: %2.\nWill restore try after %3s!"))
		    .arg(VCAStation().c_str()).arg(node.text().c_str()).arg("%1"));
	    conErr->setText(conErr->property("labTmpl").toString().arg(conErr->property("tmRest").toInt()));
	}
    }
    //Remove error message about connection error
    else if(rez != 10 && main && conErr) {
	if(masterPg()) conErr->deleteLater();
	conErr = NULL;
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
    //Save main window position
    if(winPosCntrSave() && masterPg()) {
	wAttrSet(masterPg()->id(), i2s(screen())+"geomX", i2s(pos().x()), true);
	wAttrSet(masterPg()->id(), i2s(screen())+"geomY", i2s(pos().y()), true);
    }

    //Exit on close last run project
    if(mod->exitLstRunPrjCls() && masterPg()) {
	unsigned winCnt = 0;
	for(int i_w = 0; i_w < QApplication::topLevelWidgets().size(); i_w++)
	    if(qobject_cast<QMainWindow*>(QApplication::topLevelWidgets()[i_w]) && QApplication::topLevelWidgets()[i_w]->isVisible())
		winCnt++;

	if(winCnt <= 1 && !qApp->property("closeToTray").toBool()) SYS->stop();
    }

    endRunTimer->stop();
    updateTimer->stop();

    winClose = true;
    ce->accept();
}

void VisRun::resizeEvent( QResizeEvent *ev )
{
    if(masterPg()) {
	float x_scale_old = x_scale;
	float y_scale_old = y_scale;
	if(windowState()&(Qt::WindowMaximized|Qt::WindowFullScreen)) {
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)masterPg()->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)masterPg()->size().height();
	    if(x_scale > 1 && x_scale < 1.02) x_scale = 1;
	    if(y_scale > 1 && y_scale < 1.02) y_scale = 1;
	    if(keepAspectRatio()) x_scale = y_scale = vmin(x_scale, y_scale);
	} else x_scale = y_scale = 1;
	if(x_scale_old != x_scale || y_scale_old != y_scale) {
	    isResizeManual = true;
	    fullUpdatePgs();
	    isResizeManual = false;
	}

	// Fit to the master page size
	if((x_scale_old != x_scale || y_scale_old != y_scale || !ev || !ev->oldSize().isValid()) && !(windowState()&(Qt::WindowMaximized|Qt::WindowFullScreen))) {
	    QRect ws = QApplication::desktop()->availableGeometry(this);
	    resize(fmin(ws.width()-10,masterPg()->size().width()+(centralWidget()->parentWidget()->width()-centralWidget()->width())+5),
		fmin(ws.height()-10,masterPg()->size().height()+(centralWidget()->parentWidget()->height()-centralWidget()->height())+5));
	}

	mess_debug(mod->nodePath().c_str(), _("Root page scale [%f:%f]."), x_scale, y_scale);
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

    if(pgList.empty())	{ QMessageBox::warning(this,_("Print page"),_("No one page for print is present!")); return; }

    if(pg.empty() && pgList.size() == 1)	pg = pgList[0];
    if(pg.empty() && pgList.size() > 1) {
	//Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a page for print."), _("Page print."), false, false, lang().c_str());
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
    dlg.setWindowTitle(QString(_("Print page: \"%1\" (%2)")).arg(pnm.c_str()).arg(pg.c_str()));
    if(dlg.exec() == QDialog::Accepted) {
	int fntSize = 35;
	QSize papl(2048,2048*prPg->paperRect().height()/prPg->paperRect().width());
	QSize pagl(papl.width()*prPg->pageRect().width()/prPg->paperRect().width(), papl.height()*prPg->pageRect().height()/prPg->paperRect().height());

	QPainter painter;
	painter.begin(prPg);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(prPg->paperRect());

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

    if(pgList.empty())	{ QMessageBox::warning(this,_("Print diagram"),_("No one page is present!")); return; }

    if(dg.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Diagram",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Print diagram"),_("No one diagram is present!")); return; }
	if(lst.size() == 1)	dg = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select diagram for print."), _("Diagram print."), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Diagrams:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		if((rwdg=findOpenWidget(lst[i_l])))
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if(sdlg.exec() != QDialog::Accepted) return;
	    dg = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(dg)))	return;

    string dgnm = rwdg->name();
    if(!prDiag)	prDiag = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prDiag, this);
    dlg.setWindowTitle(QString(_("Print diagram: \"%1\" (%2)")).arg(dgnm.c_str()).arg(dg.c_str()));
    if(dlg.exec() == QDialog::Accepted) {
	int fntSize = 35;
	QSize papl(2048,2048*prDiag->paperRect().height()/prDiag->paperRect().width());
	QSize pagl(papl.width()*prDiag->pageRect().width()/prDiag->paperRect().width(), papl.height()*prDiag->pageRect().height()/prDiag->paperRect().height());

	ShapeDiagram::ShpDt *sD = (ShapeDiagram::ShpDt*)rwdg->shpData;
	int elLine = sD->prms.size()/2+((sD->prms.size()%2)?1:0);

	QPainter painter;
	painter.begin(prDiag);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(prDiag->paperRect());

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
	for(unsigned i_e = 0; i_e < sD->prms.size(); i_e++) {
	    QPoint pnt((i_e/elLine)*(pagl.width()/2),im.height()+fntSize*(2+i_e%elLine));
	    if(sD->prms[i_e].val().empty() || !sD->prms[i_e].color().isValid()) continue;
	    //  Trend name request
	    reqName.setAttr("path",sD->prms[i_e].addr()+"/%2fserv%2fval");
	    if(cntrIfCmd(reqName,true) || reqName.text().empty())	reqName.setText(sD->prms[i_e].addr());

	    painter.fillRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5),QBrush(sD->prms[i_e].color()));
	    painter.drawRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5));
	    painter.drawText(QRect(pnt.x()+fntSize,pnt.y(),pagl.width()/2,fntSize),Qt::AlignLeft,
		QString("%1 [%2...%3]").arg(reqName.text().c_str()).arg(sD->prms[i_e].bordL()).arg(sD->prms[i_e].bordU()));
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

    if(pgList.empty())	{ QMessageBox::warning(this,_("Print document"),_("No one page is present!")); return; }

    if(doc.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Document",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Print document"),_("No one document is present!")); return; }
	if(lst.size() == 1)	doc = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select document for print."), _("Document print."), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Document:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		if((rwdg=findOpenWidget(lst[i_l])))
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if(sdlg.exec() != QDialog::Accepted)	return;
	    doc = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(doc)))	return;

    string docnm = rwdg->name();
    if(!prDoc) prDoc = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prDoc, this);
    dlg.setWindowTitle(QString(_("Print document: \"%1\" (%2)")).arg(docnm.c_str()).arg(doc.c_str()));
    if(dlg.exec() == QDialog::Accepted) ((ShapeDocument::ShpDt*)rwdg->shpData)->print(prDoc);
#endif
}

void VisRun::exportDef( )
{
    if(master_pg) {
	//Check for the single and big document present for default the exporting
	RunPageView *rpg;
	RunWdgView *rwdg;
	vector<string> lstDoc, lstDiagr;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP]))) {
		rpg->shapeList("Document", lstDoc);
		rpg->shapeList("Diagram", lstDiagr);
	    }
	if(lstDoc.size() == 1 && (rwdg=findOpenWidget(lstDoc[0])) &&
		((masterPg()->width()/vmax(1,rwdg->width())) < 2 || (masterPg()->height()/vmax(1,rwdg->height())) < 2))
	    exportDoc(rwdg->id());
	else if(lstDiagr.size() == 1 && (rwdg=findOpenWidget(lstDiagr[0])) &&
		((masterPg()->width()/vmax(1,rwdg->width())) < 2 || (masterPg()->height()/vmax(1,rwdg->height())) < 2))
	    exportDiag(rwdg->id());
	//Export master page
	else exportPg(master_pg->id());
    }
}

void VisRun::exportPg( const string &ipg )
{
    RunPageView *rpg;
    string pg = ipg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Export page"),_("No one page for export is present!")); return; }

    if(pg.empty() && pgList.size() == 1)	pg = pgList[0];
    if(pg.empty() && pgList.size() > 1) {
	//Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("export",NULL,true).c_str())) ico_t.load(":/images/export.png");
	InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select a page for export."), _("Page export."), false, false, lang().c_str());
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
    QString fn = getFileName(_("Save page's image"), (rpg->name()+".png").c_str(), _("Images (*.png *.xpm *.jpg)"), QFileDialog::AcceptSave);
    if(fn.size() && !img.save(fn)) mod->postMess(mod->nodePath().c_str(), QString(_("Save to file '%1' is error.")).arg(fn), TVision::Error, this);
}

void VisRun::exportDiag( const string &idg )
{
    RunWdgView *rwdg;
    string dg = idg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Export diagram"),_("No one page is present!")); return; }

    if(dg.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Diagram",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Export diagram"),_("No one diagram is present!")); return; }
	if(lst.size() == 1) dg = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select diagram for export."), _("Diagram export."), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Diagrams:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget(spg, 2, 1);
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		if((rwdg=findOpenWidget(lst[i_l])))
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
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
    QString fileName = getFileName(_("Save diagram"), QString(_("Trend %1.png")).arg(expDiagCnt++),
	_("Images (*.png *.xpm *.jpg);;CSV file (*.csv)"), QFileDialog::AcceptSave);
    if(!fileName.isEmpty()) {
	// Export to CSV
	if(fileName.indexOf(QRegExp("\\.csv$")) != -1) {
	    //  Open destination file
	    int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
	    if(fd < 0) {
		mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
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
			    CSVr = CSVr + ";"+((vPos < (int)cPrm.val().size())?QLocale().toString(cPrm.val()[vPos].val).toStdString():"");
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
	    ::close(fd);
	    if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error write to: %1.")).arg(fileName), TVision::Error, this);
	}
	// Export to image
	else if(!img.save(fileName))
	    mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
    }
}

void VisRun::exportDoc( const string &idoc )
{
    RunWdgView *rwdg;
    string doc = idoc;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Export document"),_("No one page is present!")); return; }

    if(doc.empty()) {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned iP = 0; iP < pgList.size(); iP++)
	    if((rpg=findOpenPage(pgList[iP])))
		rpg->shapeList("Document",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Export document"),_("No one document is present!")); return; }
	if(lst.size() == 1) doc = lst[0];
	else {
	    //Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select document for export."), _("Document export."), false, false, lang().c_str());
	    sdlg.edLay()->addWidget(new QLabel(_("Document:"),&sdlg), 2, 0);
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		if((rwdg=findOpenWidget(lst[i_l])))
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if(sdlg.exec() != QDialog::Accepted) return;
	    doc = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(doc))) return;
    QString fileName = getFileName(_("Save document"), QString(_("Document %1.html")).arg(expDocCnt++),
	_("XHTML (*.html);;CSV file (*.csv)"), QFileDialog::AcceptSave);
    if(!fileName.isEmpty()) {
	int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if(fd < 0) {
	    mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
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
	    while(!(!curNode->parent() && treeStk.back() >= curNode->childSize())) {
		if(treeStk.back() < curNode->childSize()) {
		    curNode = curNode->childGet(treeStk.back());
		    treeStk.push_back(0);
		    //  Check for marked table and process it
		    if(strcasecmp(curNode->name().c_str(),"table") == 0 && s2i(curNode->attr("export"))) {
			map<int,int>	rowSpn;
			XMLNode *tblN = NULL, *tblRow;
			string val;
			for(int i_st = 0; i_st < 4; i_st++) {
			    switch(i_st) {
				case 0:	tblN = curNode->childGet("thead", 0, true);	break;
				case 1:	tblN = curNode->childGet("tbody", 0, true);	break;
				case 2:	tblN = curNode->childGet("tfoot", 0, true);	break;
				case 3:	tblN = curNode;					break;
				default: tblN = NULL;
			    }
			    if(!tblN)	continue;
			    //  Rows process
			    for(unsigned i_n = 0; i_n < tblN->childSize(); i_n++) {
				if(strcasecmp(tblN->childGet(i_n)->name().c_str(),"tr") != 0)	continue;
				tblRow = tblN->childGet(i_n);
				for(unsigned i_c = 0, i_cl = 0; i_c < tblRow->childSize(); i_c++) {
				    if(!(strcasecmp(tblRow->childGet(i_c)->name().c_str(),"th") == 0 ||
					    strcasecmp(tblRow->childGet(i_c)->name().c_str(),"td") == 0))
					continue;
				    while(rowSpn[i_cl] > 1) { rez += ";"; rowSpn[i_cl]--; i_cl++; }
				    rowSpn[i_cl] = s2i(tblRow->childGet(i_c)->attr("rowspan",false));
				    val = tblRow->childGet(i_c)->text(true,true);
				    for(size_t i_sz = 0; (i_sz=val.find("\"",i_sz)) != string::npos; i_sz += 2) val.replace(i_sz,1,2,'"');
				    rez += "\""+sTrm(val)+"\";";
				    //   Colspan process
				    int colSpan = s2i(tblRow->childGet(i_c)->attr("colspan",false));
				    for(int i_cs = 1; i_cs < colSpan; i_cs++) rez += ";";
				    i_cl++;
				}
				rez += "\x0D\x0A";
			    }
			}
			rez += "\x0D\x0A";
		    }
		    else continue;
		}
		curNode = curNode->parent();
		treeStk.pop_back();
		treeStk.back()++;
	    }
	}
	// Export to XHTML
	else rez = ((ShapeDocument::ShpDt*)rwdg->shpData)->toHtml();

	bool fOK = true;
	if(rez.empty())	mod->postMess(mod->nodePath().c_str(),QString(_("No data for export.")),TVision::Error,this);
	else fOK = (write(fd,rez.data(),rez.size()) == (int)rez.size());
	::close(fd);
	if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error write to: %1.")).arg(fileName), TVision::Error, this);
    }
}

void VisRun::about( )
{
    QMessageBox::about(this, windowTitle(),
	QString(_("%1 v%2.\n%3\nAuthor: %4\nLicense: %5\n\n%6 v%7.\n%8\nLicense: %9\nAuthor: %10\nWeb site: %11")).
	arg(mod->modInfo("Name:"+lang()).c_str()).arg(mod->modInfo("Version").c_str()).arg(mod->modInfo("Description:"+lang()).c_str()).
	arg(mod->modInfo("Author:"+lang()).c_str()).arg(mod->modInfo("License").c_str()).
	arg(PACKAGE_NAME).arg(VERSION).arg(Mess->I18N(PACKAGE_DESCR,NULL,lang().c_str())).arg(PACKAGE_LICENSE).arg(Mess->I18N(PACKAGE_AUTHOR,NULL,lang().c_str())).arg(PACKAGE_SITE));
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
    req.clear()->setName("disconnect")->setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess())->setAttr("conId", i2s(oldConId));
    cntrIfCmd(req);

    //Update pages after an user change
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
	    mess_debug(mod->nodePath().c_str(), _("Root page scale [%f:%f]."), x_scale, y_scale);
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
}

void VisRun::aboutQt( )		{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void VisRun::fullScreen( bool vl )	{ setWindowState(vl?Qt::WindowFullScreen:Qt::WindowNoState); }

void VisRun::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void VisRun::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString());
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("No the manual '%1' found offline or online!")).arg(sender()->property("doc").toString()));
}

void VisRun::alarmAct( QAction *alrm )
{
    if(alrm == NULL) return;

    int quittance = 0;
    bool quittanceRet = false;
    string qwdg;
    if(alrm->objectName() == "alarmLev")	quittance = 0xFF;
    else if(alrm->objectName().toStdString().compare(0,8,"alarmNtf") == 0) {
	quittanceRet = alrm->property("quittanceRet").toBool();
	quittanceRet = quittanceRet && !alrm->isChecked();
	quittance = s2i(alrm->objectName().toStdString().substr(8));
	map<uint8_t, Notify*>::iterator iN = mNotify.find(quittance);
	if(!quittanceRet && iN != mNotify.end()) qwdg = iN->second->curQueueWdg();
	quittance = (1<<quittance);
    }
    else return;

    XMLNode req("quittance");
    req.setAttr("path", "/ses_"+work_sess+"/%2fserv%2falarm")->
	setAttr("tmpl", u2s(quittance))->
	setAttr("ret", i2s(quittanceRet))->
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
		delete userSt;
		//userSt->deleteLater();
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
    src_prj = work_sess = "";
    string openPgs;

    //Connect/create session
    int off = 0;
    if((src_prj=TSYS::pathLev(iprjSes_it,0,true,&off)).empty()) return;
    if(off > 0 && off < iprjSes_it.size()) openPgs = iprjSes_it.substr(off);
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
	    QString(_("Several sessions are already opened on the project \"%1\".\n"
		"You can create new or connect to present session. Please, select needed or press \"Cancel\".")).arg(src_prj.c_str()),
	    _("Select session for connection or new creation"), false, false, lang().c_str());
	QListWidget *ls_wdg = new QListWidget(&conreq);
	conreq.edLay()->addWidget(ls_wdg, 0, 0);
	ls_wdg->addItem(_("<Create new session>"));
	for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++)
	    ls_wdg->addItem(req.childGet(i_ch)->text().c_str());
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
	if(!(conErr && s2i(req.attr("rez")) == 10)) {	//Need check for prevent the warning dialog and the run closing by the session creation wait
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
    req.childAdd("openlist")->setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("tm","0")->setAttr("conId", i2s(mConId));
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
	if(style() < 0 && pN->childSize() <= 1) mStlBar->setVisible(false);

	// Clean up the previous pages for clean reconnection
	if(toRestore) {
	    pgCacheClear();
	    openPgs = "";
	    for(unsigned iP = 0; iP < pgList.size(); iP++) {
		RunPageView *pg = master_pg->findOpenPage(pgList[iP]);
		if(pg) { pg->deleteLater(); openPgs += pgList[iP] + ";"; }
	    }
	    master_pg->deleteLater();
	    master_pg = NULL;
	    setXScale(1); setYScale(1);
	}

	// Open pages list
	pN = req.childGet(5);
	pgList.clear();
	for(unsigned i_ch = 0; i_ch < pN->childSize(); i_ch++) {
	    pgList.push_back(pN->childGet(i_ch)->text());
	    callPage(pN->childGet(i_ch)->text()/*, toRestore*/);
	}
	reqtm = strtoul(pN->attr("tm").c_str(), NULL, 10);
    }

    //Open direct-selected page or openned before ones
    if(openPgs.size()) {
	req.clear()->setName("CntrReqs")->setAttr("path", "/ses_"+work_sess);
	string pIt;
	for(off = 0; (pIt=TSYS::strParse(openPgs,0,";",&off)).size(); )
	    req.childAdd("open")->setAttr("path","/%2fserv%2fpg")->setAttr("pg",pIt);
	cntrIfCmd(req);
	// Force call for blinks prevent
	for(off = 0; (pIt=TSYS::strParse(openPgs,0,";",&off)).size(); )
	    callPage(pIt);
    }

    if(toRestore) isResizeManual = false;

    QCoreApplication::processEvents();

    //Start timer
    updateTimer->start(period());
}

void VisRun::fullUpdatePgs( )
{
    for(unsigned iP = 0; iP < pgList.size(); iP++) {
	RunPageView *pg = master_pg->findOpenPage(pgList[iP]);
	if(pg) pg->update(true);
    }
}

string	VisRun::lang( )
{
    try { return SYS->security().at().usrAt(user()).at().lang(); } catch(...) { }
    return "";
}

void VisRun::messUpd( )
{
    //setWindowTitle(_("Vision runtime"));

    //Close
    actClose->setText(_("&Close"));
    actClose->setToolTip(_("Close Vision window"));
    actClose->setWhatsThis(_("The button for closing Vision runtime window"));
    actClose->setStatusTip(_("Press to close of the current Vision runtime window."));
    //Quit
    actQuit->setText(_("&Quit"));
    actQuit->setToolTip(_("Quit from OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA system."));
    //Full screen
    actFullScr->setText(_("Full screen"));
    actFullScr->setToolTip(_("Full screen toggle"));
    actFullScr->setWhatsThis(_("The button for full screen toggle"));
    actFullScr->setStatusTip(_("Press for toggle full screen."));
    //About
    actAbout->setText(_("&About"));
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display the program and OpenSCADA information"));
    actAbout->setStatusTip(_("Press for display the program and OpenSCADA information."));
    //About Qt
    actQtAbout->setText(_("About &Qt"));
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using QT information"));
    actQtAbout->setStatusTip(_("Press to get the using QT information."));

    //Menus
    menuFile.setTitle(_("&File"));
    menuAlarm.setTitle(_("&Alarm"));
    menuView.setTitle(_("&View"));
    menuHelp.setTitle(_("&Help"));

    //Menu "Print"
    menuPrint->setTitle(_("&Print"));
    menuPrint->menuAction()->setToolTip(_("Print the master page"));
    menuPrint->menuAction()->setWhatsThis(_("The button for printing of the master page by default"));
    menuPrint->menuAction()->setStatusTip(_("Press for printing of the master page by default."));
    actPrintPg->setText(_("Page"));
    actPrintPg->setToolTip(_("Print the selected page"));
    actPrintPg->setWhatsThis(_("The button for printing of the selected page"));
    actPrintPg->setStatusTip(_("Press for printing of the selected page."));
    actPrintDiag->setText(_("Diagram"));
    actPrintDiag->setToolTip(_("Print the selected diagram"));
    actPrintDiag->setWhatsThis(_("The button for printing of the selected diagram"));
    actPrintDiag->setStatusTip(_("Press for printing of the selected diagram."));
    actPrintDoc->setText(_("Document"));
    actPrintDoc->setToolTip(_("Print the selected document"));
    actPrintDoc->setWhatsThis(_("The button for printing of the selected document"));
    actPrintDoc->setStatusTip(_("Press for printing of the selected document."));

    //Menu "Export"
    menuExport->setTitle(_("&Export"));
    menuExport->menuAction()->setToolTip(_("Export the master page"));
    menuExport->menuAction()->setWhatsThis(_("The button for exporting of the master page by default"));
    menuExport->menuAction()->setStatusTip(_("Press for exporting of the master page by default."));
    actExpPg->setText(_("Page"));
    actExpPg->setToolTip(_("Export the selected page"));
    actExpPg->setWhatsThis(_("The button for exporting of the selected page"));
    actExpPg->setStatusTip(_("Press for exporting of the selected page."));
    actExpDiag->setText(_("Diagram"));
    actExpDiag->setToolTip(_("Export the selected diagram"));
    actExpDiag->setWhatsThis(_("The button for exporting of the selected diagram"));
    actExpDiag->setStatusTip(_("Press for exporting of the selected diagram."));
    actExpDoc->setText(_("Document"));
    actExpDoc->setToolTip(_("Export the selected document"));
    actExpDoc->setWhatsThis(_("The button for exporting of the selected document"));
    actExpDoc->setStatusTip(_("Press for exporting of the selected document."));

    //Project's manual
    actProjManual->setText(QString(_("Project '%1' manual")).arg(srcProject().c_str()));
    actProjManual->setProperty("doc", srcProject().c_str());
    actProjManual->setWhatsThis(QString(_("The button for getting the using project '%1' manual")).arg(srcProject().c_str()));
    actProjManual->setStatusTip(QString(_("Press to get the using project '%1' manual.")).arg(srcProject().c_str()));
    //Module manual
    actManual->setText(QString(_("'%1' manual")).arg(mod->modId().c_str()));
    actManual->setProperty("doc", "Modules/Vision|Modules/Vision");
    actManual->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press to get the using '%1' manual.")).arg(mod->modId().c_str()));
    //OpenSCADA manual
    actManualSYS->setText(QString(_("'%1' manual")).arg(PACKAGE_STRING));
    actManualSYS->setProperty("doc", "index|Documents");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using '%1' manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press to get the using '%1' manual.")).arg(PACKAGE_STRING));
    //What is
    actWhatIs->setText(_("What's &This"));
    actWhatIs->setToolTip(_("The button for question about GUI elements"));
    actWhatIs->setWhatsThis(_("Get respond about user interface elements"));
    actWhatIs->setStatusTip(_("Press to respond about user interface elements."));

    //Alarm level display button and full alarms quittance
    actAlrmLev->setText(_("Alarm level"));
    actAlrmLev->setToolTip(_("Alarm level"));
    actAlrmLev->setWhatsThis(_("The button for all alarms quittance"));
    actAlrmLev->setStatusTip(_("Press for all alarms quittance."));

    //Generic tools bar
    toolBarStatus->setWindowTitle(_("Generic (status)"));

    //Init status bar
    mWTime->setWhatsThis(_("This label displays current system's time."));
    mWUser->setWhatsThis(_("This label displays current user."));
    mWUser->setToolTip(_("Field for display of the current user."));
    mWUser->setStatusTip(_("Double click to change user."));
    mWStat->setWhatsThis(_("This label displays used VCA engine station."));
    mWStat->setToolTip(_("Field for display of the used VCA engine station."));
    mStlBar->setWhatsThis(_("This label displays used interface style."));
    mStlBar->setToolTip(_("Field for display the used interface style."));
    mStlBar->setStatusTip(_("Double click for style change."));

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

    //Place all needs atrributes to one request
    XMLNode req("CntrReqs"), *chN; req.setAttr("path", pg_it);
    req.childAdd("get")->setAttr("path", "/%2fattr%2fpgGrp");
    req.childAdd("get")->setAttr("path", "/%2fattr%2fpgOpenSrc");
    for(unsigned iNtf = 0; iNtf < 7; iNtf++) {
	req.childAdd("get")->setAttr("path", "/%2fattr%2fnotifyVis"+mod->modId()+i2s(iNtf));
	req.childAdd("get")->setAttr("path", "/%2fattr%2fnotify"+i2s(iNtf));
    }
    cntrIfCmd(req);

    //Get group and parent page
    string pgGrp = (chN=req.getElementBy("path","/%2fattr%2fpgGrp")) ? chN->text() : "";	//wAttrGet(pg_it, "pgGrp");
    string pgSrc = (chN=req.getElementBy("path","/%2fattr%2fpgOpenSrc")) ? chN->text() : "";	//wAttrGet(pg_it, "pgOpenSrc");

    //Check for master page replace
    if(!master_pg || pgGrp == "main" || master_pg->pgGrp() == pgGrp) {
	// Send close command
	if(master_pg) {
	    XMLNode req("close"); req.setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("pg",master_pg->id());
	    cntrIfCmd(req);
	}

	// Get and activate for specific attributes to the master-page
	XMLNode reqSpc("CntrReqs"); reqSpc.setAttr("path", pg_it);
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fstatLine")->
				     setAttr("aNm", _("Status line items"))->
				     setAttr("aTp", i2s(TFld::String))->setAttr("aFlg", i2s(TFld::FullText));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2frunWin")->
				     setAttr("aNm", _("Run window"))->
				     setAttr("aTp", i2s(TFld::Integer))->setAttr("aFlg", i2s(TFld::Selected))->
				     setAttr("aVls", "0;1;2")->setAttr("aNms", _("Original size;Maximize;Full screen"));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fkeepAspectRatio")->
				     setAttr("aNm", _("Keep aspect ratio on scale"))->
				     setAttr("aTp", i2s(TFld::Boolean));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fstBarNoShow")->
				     setAttr("aNm", _("Not show status bar"))->
				     setAttr("aTp", i2s(TFld::Boolean));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fwinPosCntrSave")->
				     setAttr("aNm", _("Windows position control and save"))->
				     setAttr("aTp", i2s(TFld::Boolean));
	reqSpc.childAdd("activate")->setAttr("path", "/%2fserv%2fattr%2fuserSetVis");
	cntrIfCmd(reqSpc);

	// Create widget view
	master_pg = new RunPageView(pg_it, this, centralWidget());
	conErr = NULL;			//possible a connection error status clean up
	//master_pg->load("");
	master_pg->setFocusPolicy(Qt::StrongFocus);
	((QScrollArea *)centralWidget())->setWidget(master_pg);
	if(!(windowState()&(Qt::WindowFullScreen|Qt::WindowMaximized))) {
	    QRect ws = QApplication::desktop()->availableGeometry(this);
	    resize(vmin(master_pg->size().width()+10,ws.width()-10), vmin(master_pg->size().height()+55,ws.height()-10));
	}
	else x_scale = y_scale = 1.0;
    }
    //Put to check for include
    else master_pg->callPage(pg_it, pgGrp, pgSrc);

    //Get the notificators configuration and register its
    for(unsigned iNtf = 0; iNtf < 7; iNtf++)
	if(((chN=req.getElementBy("path","/%2fattr%2fnotifyVis"+mod->modId()+i2s(iNtf))) && !s2i(chN->attr("rez"))) ||
		((chN=req.getElementBy("path","/%2fattr%2fnotify"+i2s(iNtf))) && !s2i(chN->attr("rez"))))
	    ntfReg(iNtf, chN->text(), pg_it);
}

void VisRun::pgCacheClear( )
{
    while(!cachePg.empty()) {
	delete cachePg.front();
	cachePg.pop_front();
    }
}

void VisRun::pgCacheAdd( RunPageView *wdg )
{
    if(!wdg) return;
    cachePg.push_front(wdg);
    while(cachePg.size() > 100) {
	delete cachePg.back();
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
	string sel = TSYS::pathLev(wdg,0,true,&off);
	if(sel.empty() || sel.substr(0,4) == "wdg_")	break;
    }
    RunPageView *rpg = findOpenPage(wdg.substr(0,woff));
    if(!rpg )	return NULL;
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
    for(unsigned i_w = 0; i_w < mod->mnWinds.size(); i_w++)
	if(qobject_cast<VisRun*>(mod->mnWinds[i_w]) && ((VisRun*)mod->mnWinds[i_w])->srcProject() == srcProject()) {
	    if(((VisRun*)mod->mnWinds[i_w])->workSess() != workSess()) isMaster = false;
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
	    if(cO->property("quittanceRet").toBool() && !newSt != cO->isChecked()) cO->setChecked(!newSt);
	    if(!cO->property("quittanceRet").toBool() && newSt != cO->isEnabled()) cO->setEnabled(newSt);
	}
    }

    //Alarm action indicators update
    // Alarm level icon update
    if(ch_tp&0xFF || (alarm>>16)&ntfSet /*|| (alarm>>16)&(TVision::Light|TVision::Alarm|TVision::Sound)*/ || !alrLevSet) {
	int alarmLev = alarm&0xFF;
	actAlrmLev->setToolTip(QString(_("Alarm level: %1")).arg(alarmLev));

	QImage lens(":/images/alarmLev.png");
	QImage levImage(lens.size(),lens.format());

	QPainter painter(&levImage);
	//QColor lclr( alarmLev ? 224 : 0, alarmLev ? 224-(int)(0.87*alarmLev) : 224, 0 );
	QColor lclr(alarmLev ? 255 : 0, alarmLev ? 255-alarmLev : 255, 0);

	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(levImage.rect(),Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	if(!((alarm>>16)&ntfSet && /*(alarm>>16)&(TVision::Light|TVision::Alarm|TVision::Sound) &&*/ alrLevSet)) {
	    for(int i_x = 0; i_x < lens.size().width(); i_x++)
		for(int i_y = 0; i_y < lens.size().height(); i_y++)
		    if(lens.pixel(i_x,i_y)&0xFF000000)	levImage.setPixel(i_x,i_y,lclr.rgba());
	    alrLevSet = true;
	} else alrLevSet = false;

	painter.drawImage(0, 0, lens);
	painter.end();
	actAlrmLev->setIcon(QPixmap::fromImage(levImage));
    }

    mAlrmSt = alarm;
}

void VisRun::ntfReg( uint8_t tp, const string &props, const string &pgCrtor )
{
    vector<string> pgPropsQ;

    //Find for presented notification type
    map<uint8_t,Notify*>::iterator iN = mNotify.find(tp);
    if(iN != mNotify.end()) {
	if(pgCrtor == iN->second->pgCrtor() && props == iN->second->props()) return;
	pgPropsQ = iN->second->pgPropsQ;
	if(pgCrtor != iN->second->pgCrtor()) {
	    // Check the queue for the page already here
	    for(vector<string>::iterator iQ = iN->second->pgPropsQ.begin(); iQ != iN->second->pgPropsQ.end(); ++iQ)
		if(TSYS::strLine(*iQ,0) == pgCrtor) {
		    if(props.empty()) iN->second->pgPropsQ.erase(iQ);
		    else *iQ = pgCrtor+"\n"+props;
		    return;
		}
	    if(props.empty()) return;
	    pgPropsQ.push_back(iN->second->pgProps);
	}
	delete iN->second;
	mNotify.erase(iN);
	ntfSet &= ~(1<<tp);
    }
    //New or replaced creation
    if(props.size()) {
	mNotify[tp] = new Notify(tp, pgCrtor+"\n"+props, this);
	mNotify[tp]->pgPropsQ = pgPropsQ;
	ntfSet |= (1<<tp);
    }
    //Take and place a notificator from the queue
    else if(pgPropsQ.size()) {
	mNotify[tp] = new Notify(tp, pgPropsQ.back(), this); pgPropsQ.pop_back();
	mNotify[tp]->pgPropsQ = pgPropsQ;
	ntfSet |= (1<<tp);
    }
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
    if(val.size() > USER_FILE_LIMIT) return;
    mCacheRes[res] = CacheEl(SYS->sysTm(), val);
    if(mCacheRes.size() > (STD_CACHE_LIM+STD_CACHE_LIM/10)) {
	vector< pair<time_t,string> > sortQueue;
	for(map<string,CacheEl>::iterator itr = mCacheRes.begin(); itr != mCacheRes.end(); ++itr)
	    sortQueue.push_back(pair<time_t,string>(itr->second.tm,itr->first));
	sort(sortQueue.begin(), sortQueue.end());
	for(unsigned i_del = 0; i_del < (STD_CACHE_LIM/10); ++i_del) mCacheRes.erase(sortQueue[i_del].second);
    }
}

void VisRun::updatePage( )
{
    int64_t d_cnt = 0;
    if(winClose) return;

    int rez;

    if(mess_lev() == TMess::Debug) d_cnt = TSYS::curTime();

    //Pages update
    XMLNode req("openlist");
    req.setAttr("tm", u2s(reqtm))->setAttr("conId", i2s(mConId))->
	setAttr("path", "/ses_"+work_sess+"/%2fserv%2fpg");

    if(!(rez=cntrIfCmd(req,false,true))) {
	// Check for delete the pages
	RunPageView *pg;
	for(unsigned iP = 0, i_ch; iP < pgList.size(); iP++) {
	    for(i_ch = 0; i_ch < req.childSize(); i_ch++)
		if(pgList[iP] == req.childGet(i_ch)->text())
		    break;
	    if(i_ch < req.childSize() || !(master_pg && (pg=master_pg->findOpenPage(pgList[iP])))) continue;
	    if(!pg->property("cntPg").toString().isEmpty())
		((RunWdgView*)TSYS::str2addr(pg->property("cntPg").toString().toStdString()))->setPgOpenSrc("");
	    else {
		if(pg != master_pg)	pg->deleteLater();
		else {
		    ((QScrollArea*)centralWidget())->setWidget(new QWidget());
		    master_pg = NULL;
		    conErr = NULL;		//possible a connection error status clean up
		}
	    }
	}

	// Process the opened pages
	pgList.clear();
	for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++) {
	    pgList.push_back(req.childGet(i_ch)->text());
	    callPage(req.childGet(i_ch)->text(), s2i(req.childGet(i_ch)->attr("updWdg")));
	}
    }
    // Restore closed session of used project.
    else if(rez == 2) {
	mess_warning(mod->nodePath().c_str(),_("Session creation restore for '%s'."),prjSes_it.c_str());
	updateTimer->stop();
	initSess(prjSes_it, crSessForce);
	return;
    }

    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //Alarms update (0.5 second update)
    if((wPrcCnt%(500/vmin(500,period()))) == 0) {
	// Get alarm status
	unsigned wAlrmSt = alarmSt();
	req.clear()->
	    setName("get")->
	    setAttr("mode", "stat")->
	    setAttr("path", "/ses_"+work_sess+"/%2fserv%2falarm");
	if(!cntrIfCmd(req,false,true)) wAlrmSt = s2i(req.attr("alarmSt"));

	// Set alarm
	alarmSet(wAlrmSt);
    }

    //Old pages from cache for close checking
    for(unsigned iPg = 0; iPg < cachePg.size(); )
	if(mod->cachePgLife() > 0.01 && (period()*(reqTm()-cachePg[iPg]->reqTm())/1000) > (unsigned)(mod->cachePgLife()*60*60)) {
	    delete cachePg[iPg];
	    cachePg.erase(cachePg.begin()+iPg);
	}
	else iPg++;

    if(mess_lev() == TMess::Debug) {
	upd_tm += 1e-3*(TSYS::curTime()-d_cnt);
	if(!(1000/vmin(1000,period()) && wPrcCnt%(1000/vmin(1000,period())))) {
	    mess_debug(mod->nodePath().c_str(), _("Session '%s' update time %f ms."), workSess().c_str(), upd_tm);
	    upd_tm = 0;
	}
    }

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
}

#undef _
#define _(mess) mod->I18N(mess, owner()->lang().c_str())

//* Notify: Generic notifying object.		 *
//************************************************
VisRun::Notify::Notify( uint8_t itp, const string &ipgProps, VisRun *iown ) : pgProps(ipgProps),
    tp(itp), alSt(0xFFFFFFFF), repDelay(-1), comIsExtScript(false), f_notify(false), f_resource(false), f_queue(false), f_quittanceRet(false),
    toDo(false), alEn(false), mQueueCurTm(0), dataM(true), mOwner(iown), actAlrm(NULL)
{
    //Parse properties
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
		else if(iOpt == "quittanceRet")	f_quittanceRet = true;
	    hasFlags = true;
	}
	else if(ico.empty() && (size_t)(fPos=iLn.find("ico=")) != string::npos)	  ico = iLn.substr(fPos+4);
	else if(name.empty() && (size_t)(fPos=iLn.find("name=")) != string::npos) name = iLn.substr(fPos+5);

    //The command procedure prepare
    if(comIsExtScript) {
	// Prepare the external script
	comProc = "sesRun_"+owner()->workSess()+"_ntf"+i2s(tp);
	bool fOK = false;
	int hd = open(comProc.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0775);
	if(hd >= 0) {
	    fOK = write(hd, props().data(), props().size()) == (ssize_t)props().size();
	    ::close(hd);
	}
	if(!fOK) {
	    mess_err((mod->nodePath()+"/sesRun_"+owner()->workSess()).c_str(), _("Function of the notificator '%s' error: %s"), comProc.c_str(), strerror(errno));
	    comProc = "";
	}
    }
    else {
	// Prepare an internal procedure
	TFunction funcIO("sesRun_"+owner()->workSess()+"_ntf"+i2s(tp));
	//funcIO.setStor(DB());
	funcIO.ioIns(new IO("en",_("Enabled notification"),IO::Boolean,IO::Default), IFA_en);
	funcIO.ioIns(new IO("doNtf",_("Doing notification, always 1"),IO::Boolean,IO::Default), IFA_doNtf);
	funcIO.ioIns(new IO("doRes",_("Doing resource, always 0"),IO::Boolean,IO::Default), IFA_doRes);
	funcIO.ioIns(new IO("res",_("Resource stream"),IO::String,IO::Output), IFA_res);
	funcIO.ioIns(new IO("mess",_("Notification message"),IO::String,IO::Default), IFA_mess);
	funcIO.ioIns(new IO("lang",_("Notification message's language"),IO::String,IO::Default), IFA_lang);
	try { comProc = SYS->daq().at().at("JavaLikeCalc").at().compileFunc("JavaScript", funcIO, props()); }
	catch(TError &er) {
	    mess_err((mod->nodePath()+"/sesRun_"+owner()->workSess()).c_str(), _("Function of the notificator '%s' error: %s"),
		    funcIO.id().c_str(), er.mess.c_str());
	}
    }

    if(f_notify) {
	//Call conditional variable init
	pthread_cond_init(&callCV, NULL);

	//Notification task create
	SYS->taskCreate(mod->nodePath('.',true)+".sesRun_"+owner()->workSess()+".ntf"+i2s(tp), 0, VisRun::Notify::Task, this);
    }

    //The quittance action prepare and place
    QImage ico_t;
    if(!ico.empty()) {
	ico = owner()->masterPg()->resGet(ico);
	ico_t.loadFromData((const uchar*)ico.data(), ico.size());
    }
    if(ico_t.isNull() && !ico_t.load(TUIS::icoGet("alarmAlarm",NULL,true).c_str())) ico_t.load(":/images/alarmAlarm.png");
    if(name.empty()) name = TSYS::strMess(_("Notyfier %d"), tp);
    actAlrm = new QAction(QPixmap::fromImage(ico_t), name.c_str(), owner());
    actAlrm->setObjectName(("alarmNtf"+i2s(tp)).c_str());
    //actAlrm->setToolTip(_("Blink alarm"));
    if(f_queue) {
	actAlrm->setWhatsThis(QString(_("The button for current \"%1\" quittance")).arg(name.c_str()));
	actAlrm->setStatusTip(QString(_("Press for current \"%1\" quittance.")).arg(name.c_str()));
    }
    else {
	actAlrm->setWhatsThis(QString(_("The button for all \"%1\" quittance")).arg(name.c_str()));
	actAlrm->setStatusTip(QString(_("Press for all \"%1\" quittance.")).arg(name.c_str()));
    }
    actAlrm->setProperty("quittanceRet", (bool)f_quittanceRet);
    actAlrm->setCheckable(f_quittanceRet);
    owner()->menuAlarm.addAction(actAlrm);
    owner()->toolBarStatus->addAction(actAlrm);

    //Apply to the current alarm status
    actAlrm->setVisible((owner()->alarmSt()>>8)&(1<<tp));
    if(f_quittanceRet) actAlrm->setChecked(!((owner()->alarmSt()>>16)&(1<<tp)));
    else actAlrm->setEnabled((owner()->alarmSt()>>16)&(1<<tp));

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:Notify", 1);
}

VisRun::Notify::~Notify( )
{
    if(f_notify) {
	SYS->taskDestroy(mod->nodePath('.',true)+".sesRun_"+owner()->workSess()+".ntf"+i2s(tp), NULL, 10, false, &callCV);
	pthread_cond_destroy(&callCV);
    }

    //The command procedure remove
    if(comIsExtScript && comProc.size()) remove(comProc.c_str());

    //The quittance action remove
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
    string rez = mQueueCurPath;
    pthread_mutex_unlock(&dataM.mtx());

    return rez;
}

void VisRun::Notify::ntf( int ialSt )
{
    //Check for the alarm state change
    if(!f_notify || !(((ialSt^alSt)>>16)&(1<<tp)))	return;

    alEn = (bool)((ialSt>>16)&(1<<tp));
    pthread_mutex_lock(&dataM.mtx());
    toDo = true;
    pthread_cond_signal(&callCV);
    pthread_mutex_unlock(&dataM.mtx());

    alSt = ialSt;
}

string VisRun::Notify::ntfRes( string &mess, string &lang )
{
    string rez;
    mess = lang = "";

    //Call same request to the VCA server for resources
    XMLNode req("get");
    req.setAttr("path", "/ses_"+owner()->workSess()+"/%2fserv%2falarm")->
	setAttr("mode", "resource")->
	setAttr("tp", i2s(tp))->
	setAttr("tm", u2s(mQueueCurTm))->
	setAttr("wdg", mQueueCurPath);
    if(!owner()->cntrIfCmd(req)) {
	mQueueCurTm = strtoul(req.attr("tm").c_str(), NULL, 10);
	mQueueCurPath = req.attr("wdg");
	rez = TSYS::strDecode(req.text(), TSYS::base64);
	mess = req.attr("mess");
	lang = req.attr("lang");
    }

    return rez;
}

void VisRun::Notify::commCall( string &res, const string &mess, const string &lang )
{
    if(comProc.empty()) return;

    //Shared data obtain
    pthread_mutex_lock(&dataM.mtx());
    string wcomProc = comProc;
    pthread_mutex_unlock(&dataM.mtx());

    if(comIsExtScript) {
	string resFile = "sesRun_"+owner()->workSess()+"_res"+i2s(tp);
	int hdRes = res.size() ? open(resFile.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0664) : -1;
	if(hdRes >= 0) { write(hdRes, res.data(), res.size()); ::close(hdRes); }
	// Prepare environment and execute the external script
	system(("en="+i2s(alEn)+" doNtf=1 doRes=0 res="+resFile+
	    " mess=\""+TSYS::strEncode(mess,TSYS::SQL)+"\" lang=\""+TSYS::strEncode(lang,TSYS::SQL)+"\" ./"+wcomProc).c_str());
	if(hdRes >= 0) remove(resFile.c_str());
    }
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

	string ntfRes, ntfMess, ntfLang;
	unsigned delayCnt = 0;
	do {
	    if(delayCnt) { TSYS::sysSleep(1); delayCnt--; continue; }

	    //  Get the resources for the notification
	    if((ntf.f_queue || ntf.f_resource) && ntf.alEn) ntfRes = ntf.ntfRes(ntfMess, ntfLang);

	    //  Same notification
	    ntf.commCall(ntfRes, ntfMess, ntfLang);

	    delayCnt = ntf.repDelay;
	} while((ntf.repDelay >= 0 || ntf.f_queue) && ntf.alEn && !TSYS::taskEndRun());

	pthread_mutex_lock(&ntf.dataM.mtx());
    }
    pthread_mutex_unlock(&ntf.dataM.mtx());

    return NULL;
}

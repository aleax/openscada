
//OpenSCADA system module UI.Vision file: vis_run.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <linux/input.h>

#include <QApplication>
#include <QLocale>
#include <QDesktopWidget>
#include <QMenu>
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

#include <config.h>
#include <tsys.h>
#include "tvision.h"
#include "vis_run_widgs.h"
#include "vis_shapes.h"
#include "vis_run.h"

using namespace VISION;

VisRun::VisRun( const string &iprj_it, const string &open_user, const string &user_pass, const string &VCAstat, bool icrSessForce, QWidget *parent ) :
    QMainWindow(parent), prPg(NULL), prDiag(NULL), prDoc(NULL), fileDlg(NULL), winClose(false), crSessForce(icrSessForce), keepAspectRatio(false),
    prj_it(iprj_it), master_pg(NULL), mPeriod(1000), wPrcCnt(0), reqtm(1), expDiagCnt(1), expDocCnt(1), x_scale(1), y_scale(1), mAlrmSt(0xFFFFFF),
    isConErr(false)
{
    QImage ico_t;

    setAttribute(Qt::WA_DeleteOnClose, true);
    mod->regWin(this);

    setWindowTitle(_("Vision runtime"));
    setWindowIcon(mod->icon());

    setProperty("QTStarterToolDis", true);

    //Create actions
    // Generic actions
    //  Print
    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
    QMenu *menuPrint = new QMenu(_("&Print"), this);
    menuPrint->setIcon(QPixmap::fromImage(ico_t));
    menuPrint->menuAction()->setShortcut(QKeySequence::Print);
    menuPrint->menuAction()->setToolTip(_("Print the master page"));
    menuPrint->menuAction()->setWhatsThis(_("The button for printing of the master page by default"));
    menuPrint->menuAction()->setStatusTip(_("Press for printing of the master page by default."));
    connect(menuPrint->menuAction(), SIGNAL(triggered()), this, SLOT(print()));
    QAction *actPrintPg = new QAction(_("Page"),this);
    actPrintPg->setToolTip(_("Print the selected page"));
    actPrintPg->setWhatsThis(_("The button for printing of the selected page"));
    actPrintPg->setStatusTip(_("Press for printing of the selected page."));
    connect(actPrintPg, SIGNAL(triggered()), this, SLOT(printPg()));
    menuPrint->addAction(actPrintPg);
    QAction *actPrintDiag = new QAction(_("Diagram"),this);
    actPrintDiag->setToolTip(_("Print the selected diagram"));
    actPrintDiag->setWhatsThis(_("The button for printing of the selected diagram"));
    actPrintDiag->setStatusTip(_("Press for printing of the selected diagram."));
    connect(actPrintDiag, SIGNAL(triggered()), this, SLOT(printDiag()));
    menuPrint->addAction(actPrintDiag);
    QAction *actPrintDoc = new QAction(_("Document"),this);
    actPrintDoc->setToolTip(_("Print the selected document"));
    actPrintDoc->setWhatsThis(_("The button for printing of the selected document"));
    actPrintDoc->setStatusTip(_("Press for printing of the selected document."));
    connect(actPrintDoc, SIGNAL(triggered()), this, SLOT(printDoc()));
    menuPrint->addAction(actPrintDoc);
    //  Export
    if(!ico_t.load(TUIS::icoGet("export",NULL,true).c_str())) ico_t.load(":/images/export.png");
    QMenu *menuExport = new QMenu(_("&Export"), this);
    menuExport->setIcon(QPixmap::fromImage(ico_t));
    menuExport->menuAction()->setToolTip(_("Export the master page"));
    menuExport->menuAction()->setWhatsThis(_("The button for exporting of the master page by default"));
    menuExport->menuAction()->setStatusTip(_("Press for exporting of the master page by default."));
    connect(menuExport->menuAction(), SIGNAL(triggered()), this, SLOT(exportDef()));
    QAction *actExpPg = new QAction(_("Page"),this);
    actExpPg->setToolTip(_("Export the selected page"));
    actExpPg->setWhatsThis(_("The button for exporting of the selected page"));
    actExpPg->setStatusTip(_("Press for exporting of the selected page."));
    connect(actExpPg, SIGNAL(triggered()), this, SLOT(exportPg()));
    menuExport->addAction(actExpPg);
    QAction *actExpDiag = new QAction(_("Diagram"),this);
    actExpDiag->setToolTip(_("Export the selected diagram"));
    actExpDiag->setWhatsThis(_("The button for exporting of the selected diagram"));
    actExpDiag->setStatusTip(_("Press for exporting of the selected diagram."));
    connect(actExpDiag, SIGNAL(triggered()), this, SLOT(exportDiag()));
    menuExport->addAction(actExpDiag);
    QAction *actExpDoc = new QAction(_("Document"),this);
    actExpDoc->setToolTip(_("Export the selected document"));
    actExpDoc->setWhatsThis(_("The button for exporting of the selected document"));
    actExpDoc->setStatusTip(_("Press for exporting of the selected document."));
    connect(actExpDoc, SIGNAL(triggered()), this, SLOT(exportDoc()));
    menuExport->addAction(actExpDoc);
    //  Close
    if(!ico_t.load(TUIS::icoGet("close",NULL,true).c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close Vision window"));
    actClose->setWhatsThis(_("The button for closing Vision runtime window"));
    actClose->setStatusTip(_("Press to close of the current Vision runtime window."));
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    //  Quit
    if(!ico_t.load(TUIS::icoGet("exit",NULL,true).c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit from OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA system."));
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quitSt()));
    // View actions
    //  Fullscreen
    actFullScr = new QAction(_("Full screen"),this);
    actFullScr->setCheckable(true);
    actFullScr->setToolTip(_("Full screen toggle"));
    actFullScr->setWhatsThis(_("The button for full screen toggle"));
    actFullScr->setStatusTip(_("Press for toggle full screen."));
    connect(actFullScr, SIGNAL(toggled(bool)), this, SLOT(fullScreen(bool)));

    // Help actions
    //  About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display the program and OpenSCADA information"));
    actAbout->setStatusTip(_("Press for display the program and OpenSCADA information."));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    //  About Qt
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using QT information"));
    actQtAbout->setStatusTip(_("Press to get the using QT information."));
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    //  Vision manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    QAction *actManual = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(mod->modId().c_str()),this);
    actManual->setProperty("doc", "Modules/UI.Vision|Vision");
    actManual->setShortcut(Qt::Key_F1);			//Move and use for the project manual
    actManual->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(mod->modId().c_str()));
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  OpenSCADA manual index
    QAction *actManualSYS = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(PACKAGE_STRING),this);
    actManualSYS->setProperty("doc", "index|/");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(PACKAGE_STRING));
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    //  What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setShortcut(Qt::SHIFT+Qt::Key_F1);
    actWhatIs->setToolTip(_("The button for question about GUI elements"));
    actWhatIs->setWhatsThis(_("Get respond about user interface elements"));
    actWhatIs->setStatusTip(_("Press to respond about user interface elements."));
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    // Alarms actions
    //  Alarm level display button and full alarms quittance
    if(!ico_t.load(TUIS::icoGet("alarmLev",NULL,true).c_str())) ico_t.load(":/images/alarmLev.png");
    actAlrmLev = new QAction( QPixmap::fromImage(ico_t), _("Alarm level"), this );
    actAlrmLev->setObjectName("alarmLev");
    actAlrmLev->setToolTip(_("Alarm level"));
    actAlrmLev->setWhatsThis(_("The button for all alarms quittance"));
    actAlrmLev->setStatusTip(_("Press for all alarms quittance."));
    //  Alarm by Light
    if(!ico_t.load(TUIS::icoGet("alarmLight",NULL,true).c_str())) ico_t.load(":/images/alarmLight.png");
    actAlrmLight = new QAction( QPixmap::fromImage(ico_t), _("Blink alarm"), this );
    actAlrmLight->setObjectName("alarmLight");
    actAlrmLight->setToolTip(_("Blink alarm"));
    actAlrmLight->setWhatsThis(_("The button for all blink alarms quittance"));
    actAlrmLight->setStatusTip(_("Press for all blink alarms quittance."));
    actAlrmLight->setVisible(false);
    //  Alarm by mono sound (PC speaker)
    if(!ico_t.load(TUIS::icoGet("alarmAlarm",NULL,true).c_str())) ico_t.load(":/images/alarmAlarm.png");
    actAlrmAlarm = new QAction( QPixmap::fromImage(ico_t), _("Speaker alarm"), this );
    actAlrmAlarm->setObjectName("alarmAlarm");
    actAlrmAlarm->setToolTip(_("PC speaker alarm"));
    actAlrmAlarm->setWhatsThis(_("The button for all PC speaker alarms quittance"));
    actAlrmAlarm->setStatusTip(_("Press for all PC speaker alarms quittance."));
    actAlrmAlarm->setVisible(false);
    //  Alarm by sound or synthesis of speech
    if(!ico_t.load(TUIS::icoGet("alarmSound",NULL,true).c_str())) ico_t.load(":/images/alarmSound.png");
    actAlrmSound = new QAction( QPixmap::fromImage(ico_t), _("Sound/speech alarm"), this );
    actAlrmSound->setObjectName("alarmSound");
    actAlrmSound->setToolTip(_("Sound or speech alarm"));
    actAlrmSound->setWhatsThis(_("The button for all sound or speech alarms quittance"));
    actAlrmSound->setStatusTip(_("Press for all sound or speech alarms quittance."));
    actAlrmSound->setVisible(false);

    //Create menu
    mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(menuPrint->menuAction());
    mn_file->addAction(menuExport->menuAction());
    mn_file->addSeparator();
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);
    mn_alarm = menuBar()->addMenu(_("&Alarm"));
    mn_alarm->addAction(actAlrmLev);
    mn_alarm->addAction(actAlrmLight);
    mn_alarm->addAction(actAlrmAlarm);
    mn_alarm->addAction(actAlrmSound);
    mn_view = menuBar()->addMenu(_("&View"));
    mn_view->addAction(actFullScr);
    mn_help = menuBar()->addMenu(_("&Help"));
    mn_help->addAction(actAbout);
    mn_help->addAction(actQtAbout);
    mn_help->addAction(actManual);
    mn_help->addAction(actManualSYS);
    mn_help->addSeparator();
    mn_help->addAction(actWhatIs);

    //Init tool bars
    // Alarms tools bar
    toolBarStatus = new QToolBar(_("Alarms (status)"),this);
    connect( toolBarStatus, SIGNAL(actionTriggered(QAction*)), this, SLOT(alarmAct(QAction*)) );
    toolBarStatus->setIconSize(QSize(16,16));
    toolBarStatus->addAction(menuPrint->menuAction());
    toolBarStatus->addAction(menuExport->menuAction());
    toolBarStatus->addSeparator();
    toolBarStatus->addAction(actAlrmLev);
    toolBarStatus->addAction(actAlrmLight);
    toolBarStatus->addAction(actAlrmAlarm);
    toolBarStatus->addAction(actAlrmSound);

    //Init status bar
    mWTime = new QLabel( this );
    mWTime->setVisible(false);
    mWTime->setAlignment(Qt::AlignCenter);
    mWTime->setWhatsThis(_("This label displays current system's time."));
    statusBar()->insertPermanentWidget(0,mWTime);
    mWUser = new UserStBar( open_user.c_str(), user_pass.c_str(), VCAstat.c_str(), this );
    mWUser->setWhatsThis(_("This label displays current user."));
    mWUser->setToolTip(_("Field for display of the current user."));
    mWUser->setStatusTip(_("Double click to change user."));
    connect( mWUser, SIGNAL(userChanged(const QString&,const QString&)), this, SLOT(userChanged(const QString&,const QString&)) );
    statusBar()->insertPermanentWidget(0,mWUser);
    mWStat = new QLabel( VCAStation().c_str(), this );
    mWStat->setWhatsThis(_("This label displays used VCA engine station."));
    mWStat->setToolTip(_("Field for display of the used VCA engine station."));
    mWStat->setVisible( VCAStation() != "." );
    statusBar()->insertPermanentWidget(0,mWStat);
    mStlBar = new StylesStBar( -1, this );
    mStlBar->setWhatsThis(_("This label displays used interface style."));
    mStlBar->setToolTip(_("Field for display the used interface style."));
    mStlBar->setStatusTip(_("Double click for style change."));
    connect(mStlBar, SIGNAL(styleChanged()), this, SLOT(styleChanged()));
    statusBar()->insertPermanentWidget(0,mStlBar);
    statusBar()->insertPermanentWidget(0,toolBarStatus);
    statusBar()->setVisible(mod->runPrjsSt());

    //Init scroller
    QScrollArea *scrl = new QScrollArea;
    scrl->setFocusPolicy(Qt::NoFocus);
    scrl->setAlignment(Qt::AlignCenter);
    setCentralWidget(scrl);

    //Create timers
    // End run timer
    endRunTimer   = new QTimer( this );
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);
    // Update timer
    updateTimer = new QTimer( this );
    updateTimer->setSingleShot(false);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePage()));

    alrmPlay = new SndPlay(this);

    menuBar()->setVisible(SYS->security().at().access(user(),SEC_WR,"root","root",RWRWR_));

    resize(600, 400);

    //Init session
    initSess(prj_it, crSessForce);

    //mWStat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000);

    alarmSet(0);
}

VisRun::~VisRun( )
{
    winClose = true;

    endRunTimer->stop();
    updateTimer->stop();

    alarmSet(0);
    alrmPlay->wait();

    //> Disconnect/delete session
    XMLNode req("disconnect");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("sess",work_sess);
    cntrIfCmd(req);

    //> Unregister window
    mod->unregWin(this);

    //> Clear cache
    pgCacheClear();

    //> Print objects free
    if(prPg)	delete prPg;
    if(prDiag)	delete prDiag;
    if(prDoc)	delete prDoc;
    if(fileDlg)	delete fileDlg;
}

string VisRun::user( )		{ return mWUser->user().toStdString(); }

string VisRun::password( )	{ return mWUser->pass().toStdString(); }

string VisRun::VCAStation( )	{ return mWUser->VCAStation().toStdString(); }

int VisRun::style( )		{ return mStlBar->style(); }

void VisRun::setStyle( int istl )	{ mStlBar->setStyle(istl); }

int VisRun::cntrIfCmd( XMLNode &node, bool glob )
{
    QLabel *conErr;

    int rez = mod->cntrIfCmd(node,user(),password(),VCAStation(),glob);
    //> Display error message about connection error
    if(rez == 10 && master_pg && !isConErr && !master_pg->findChild<QLabel*>("==ConnError=="))
    {
	//> Create error message
	conErr = new QLabel(QString(_("Connection to visualization server '%1' error: %2")).
				arg(VCAStation().c_str()).arg(node.text().c_str()),master_pg);
	conErr->setObjectName("==ConnError==");
	conErr->setAlignment(Qt::AlignCenter);
	conErr->setWordWrap(true);
	//> Prepare message's style
	conErr->setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
	conErr->setAutoFillBackground(true);
	QPalette plt(conErr->palette());
	QBrush brsh = plt.brush(QPalette::Background);
	brsh.setColor(Qt::red);
	brsh.setStyle(Qt::SolidPattern);
	plt.setBrush(QPalette::Background, brsh);
	conErr->setPalette(plt);
	//> Calc size and position
	conErr->resize(300,100);
	conErr->move((master_pg->size().width()-conErr->size().width())/2,(master_pg->size().height()-conErr->size().height())/2);
	conErr->show();
	isConErr = true;
    }
    //> Remove error message about connection error
    else if(rez != 10 && isConErr && (conErr=master_pg->findChild<QLabel*>("==ConnError==")))
    {
	conErr->deleteLater();
	isConErr = false;
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
    if(dir.size()) fileDlg->selectFile(dir);
#if QT_VERSION >= 0x040500
    fileDlg->setReadOnly(!menuBar()->isVisible());
#endif
    if(fileDlg->exec() && !fileDlg->selectedFiles().empty()) return fileDlg->selectedFiles()[0];

    return "";
}

void VisRun::closeEvent( QCloseEvent* ce )
{
    if(mod->exitLstRunPrjCls() && master_pg)	//Exit on close last run project
    {
	unsigned winCnt = 0;
	for(int i_w = 0; i_w < QApplication::topLevelWidgets().size(); i_w++)
    	    if(qobject_cast<QMainWindow*>(QApplication::topLevelWidgets()[i_w]) && QApplication::topLevelWidgets()[i_w]->isVisible())
        	winCnt++;

	if(winCnt <= 1) SYS->stop();
    }

    winClose = true;
    ce->accept();
}

void VisRun::resizeEvent( QResizeEvent *ev )
{
    if(ev && ev->oldSize().isValid() && master_pg)
    {
	float x_scale_old = x_scale;
	float y_scale_old = y_scale;
	if(windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen)
	{
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	    if(x_scale > 1 && x_scale < 1.05) x_scale = 1;
	    if(y_scale > 1 && y_scale < 1.05) y_scale = 1;
	    if(keepAspectRatio) x_scale = y_scale = vmin(x_scale, y_scale);
	}else x_scale = y_scale = 1;
	if(x_scale_old != x_scale || y_scale_old != y_scale)	fullUpdatePgs();
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
    if(master_pg) printPg(master_pg->id());
}

void VisRun::printPg( const string &ipg )
{
    RunPageView *rpg;
    string pg = ipg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Print page"),_("No one page for print is present!")); return; }

    if(pg.empty() && pgList.size() == 1)	pg = pgList[0];
    if(pg.empty() && pgList.size() > 1)
    {
	//> Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select page for print."), _("Page print."), false, false );
	sdlg.edLay()->addWidget( new QLabel(_("Pages:"),&sdlg), 2, 0 );
	QComboBox *spg = new QComboBox(&sdlg);
	sdlg.edLay()->addWidget( spg, 2, 1 );
	for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
	    if((rpg=findOpenPage(pgList[i_p])))
		spg->addItem((rpg->name()+" ("+pgList[i_p]+")").c_str(),pgList[i_p].c_str());
	if(sdlg.exec() != QDialog::Accepted)	return;
	pg = spg->itemData(spg->currentIndex()).toString().toStdString();
    }

    //> Find need page
    rpg = master_pg;
    if(rpg->id() != pg)	rpg = findOpenPage(pg);
    if(!rpg) return;

    string pnm = rpg->name();
    if(!prPg)	prPg = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prPg, this);
    dlg.setWindowTitle(QString(_("Print page: \"%1\" (%2)")).arg(pnm.c_str()).arg(pg.c_str()));
    if(dlg.exec() == QDialog::Accepted)
    {
	int fntSize = 35;
	QSize papl(2048,2048*prPg->paperRect().height()/prPg->paperRect().width());
	QSize pagl(papl.width()*prPg->pageRect().width()/prPg->paperRect().width(), papl.height()*prPg->pageRect().height()/prPg->paperRect().height());

	QPainter painter;
	painter.begin(prPg);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(prPg->paperRect());

	//> Draw image
	QImage im = QPixmap::grabWidget(rpg).toImage();
	im = im.scaled(QSize(vmin(im.width()*4,pagl.width()),vmin(im.height()*4,pagl.height()-2*fntSize)),Qt::KeepAspectRatio/*,Qt::SmoothTransformation*/);
	painter.drawImage((pagl.width()-im.width())/2,fntSize,im);

	//> Draw notes
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
}

void VisRun::printDiag( const string &idg )
{
    RunWdgView *rwdg;
    string dg = idg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Print diagram"),_("No one page is present!")); return; }

    if(dg.empty())
    {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
	    if((rpg=findOpenPage(pgList[i_p])))
		rpg->shapeList("Diagram",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Print diagram"),_("No one diagram is present!")); return; }
	if(lst.size() == 1)	dg = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select diagram for print."), _("Diagram print."), false, false);
	    sdlg.edLay()->addWidget( new QLabel(_("Diagrams:"),&sdlg), 2, 0 );
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		if((rwdg=findOpenWidget(lst[i_l])))
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if( sdlg.exec() != QDialog::Accepted )	return;
	    dg = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if(!(rwdg=findOpenWidget(dg)))	return;

    string dgnm = rwdg->name();
    if(!prDiag)	prDiag = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prDiag, this);
    dlg.setWindowTitle(QString(_("Print diagram: \"%1\" (%2)")).arg(dgnm.c_str()).arg(dg.c_str()));
    if( dlg.exec() == QDialog::Accepted )
    {
	int fntSize = 35;
	QSize papl(2048,2048*prDiag->paperRect().height()/prDiag->paperRect().width());
	QSize pagl(papl.width()*prDiag->pageRect().width()/prDiag->paperRect().width(), papl.height()*prDiag->pageRect().height()/prDiag->paperRect().height());

	ShapeDiagram::ShpDt *sD = (ShapeDiagram::ShpDt*)rwdg->shpData;
	int elLine = sD->prms.size()/2+((sD->prms.size()%2)?1:0);

	QPainter painter;
	painter.begin(prDiag);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(prDiag->paperRect());

	//> Draw image
	QImage im = QPixmap::grabWidget(rwdg).toImage();
	im = im.scaled(QSize(vmin(im.width()*4,pagl.width()),vmin(im.height()*4,pagl.height()-(2+elLine)*fntSize)),Qt::KeepAspectRatio/*,Qt::SmoothTransformation*/);
	painter.drawImage((pagl.width()-im.width())/2,fntSize*2,im);

	//> Draw notes
	painter.setPen(Qt::black);
	QFont fnt("Arial");
	fnt.setPixelSize(fntSize-5);
	painter.setFont(fnt);
	QDateTime dt;
	dt.setTime_t(time(NULL));
	painter.drawText(QRect(0,0,pagl.width(),fntSize*2),Qt::AlignLeft,QString(_("OpenSCADA project: \"%1\"\n%2 (%3)")).arg(windowTitle()).arg(dgnm.c_str()).arg(dg.c_str()));
	painter.drawText(QRect(0,0,pagl.width(),fntSize*2),Qt::AlignRight,QString(_("User: \"%1\"\n%2")).arg(user().c_str()).arg(dt.toString("d.MM.yyyy h:mm:ss")));

	//>> Draw trend's elements
	XMLNode reqName("name");
	for(unsigned i_e = 0; i_e < sD->prms.size(); i_e++)
	{
	    QPoint pnt((i_e/elLine)*(pagl.width()/2),im.height()+fntSize*(2+i_e%elLine));
	    if(sD->prms[i_e].val().empty() || !sD->prms[i_e].color().isValid()) continue;
	    //>>> Trend name request
	    reqName.setAttr("path",sD->prms[i_e].addr()+"/%2fserv%2fval");
    	    if(cntrIfCmd(reqName,true) || reqName.text().empty())	reqName.setText(sD->prms[i_e].addr());

	    painter.fillRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5),QBrush(sD->prms[i_e].color()));
	    painter.drawRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5));
	    painter.drawText(QRect(pnt.x()+fntSize,pnt.y(),pagl.width()/2,fntSize),Qt::AlignLeft,
		QString("%1 [%2...%3]").arg(reqName.text().c_str()).arg(sD->prms[i_e].bordL()).arg(sD->prms[i_e].bordU()));
	}

	painter.end();
    }
}

void VisRun::printDoc( const string &idoc )
{
    RunWdgView *rwdg;
    string doc = idoc;

    if( pgList.empty() )	{ QMessageBox::warning(this,_("Print document"),_("No one page is present!")); return; }

    if( doc.empty() )
    {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
	    if((rpg=findOpenPage(pgList[i_p])))
		rpg->shapeList("Document",lst);
	if( lst.empty() )	{ QMessageBox::warning(this,_("Print document"),_("No one document is present!")); return; }
	if( lst.size() == 1 )	doc = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select document for print."), _("Document print."), false, false );
	    sdlg.edLay()->addWidget( new QLabel(_("Document:"),&sdlg), 2, 0 );
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
		if((rwdg=findOpenWidget(lst[i_l])))
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if( sdlg.exec() != QDialog::Accepted )	return;
	    doc = spg->itemData(spg->currentIndex()).toString().toStdString();
	}
    }

    if( !(rwdg=findOpenWidget(doc)) )	return;

    string docnm = rwdg->name();
    if(!prDoc) prDoc = new QPrinter(QPrinter::HighResolution);
    QPrintDialog dlg(prDoc, this);
    dlg.setWindowTitle(QString(_("Print document: \"%1\" (%2)")).arg(docnm.c_str()).arg(doc.c_str()));
    if(dlg.exec() == QDialog::Accepted)
#ifdef HAVE_WEBKIT
	((ShapeDocument::ShpDt*)rwdg->shpData)->web->print(prDoc);
#else
	((ShapeDocument::ShpDt*)rwdg->shpData)->web->document()->print(prDoc);
#endif
}

void VisRun::exportDef( )
{
    if(master_pg) exportPg(master_pg->id());
}

void VisRun::exportPg( const string &ipg )
{
    RunPageView *rpg;
    string pg = ipg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Export page"),_("No one page for export is present!")); return; }

    if(pg.empty() && pgList.size() == 1)	pg = pgList[0];
    if(pg.empty() && pgList.size() > 1)
    {
	//> Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("export",NULL,true).c_str())) ico_t.load(":/images/export.png");
	InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select page for export."), _("Page export."), false, false);
	sdlg.edLay()->addWidget( new QLabel(_("Pages:"),&sdlg), 2, 0 );
	QComboBox *spg = new QComboBox(&sdlg);
	sdlg.edLay()->addWidget( spg, 2, 1 );
	for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
	    if((rpg=findOpenPage(pgList[i_p])))
		spg->addItem((rpg->name()+" ("+pgList[i_p]+")").c_str(),pgList[i_p].c_str());
	if(sdlg.exec() != QDialog::Accepted)	return;
	pg = spg->itemData(spg->currentIndex()).toString().toStdString();
    }

    //> Find need page
    rpg = master_pg;
    if(rpg->id() != pg)	rpg = findOpenPage(pg);
    if(!rpg) return;

    QPixmap img = QPixmap::grabWidget(rpg);
    QString fn = getFileName(_("Save page's image"), (rpg->name()+".png").c_str(), _("Images (*.png *.xpm *.jpg)"), QFileDialog::AcceptSave);
    if(fn.size() && !img.save(fn)) mod->postMess(mod->nodePath().c_str(), QString(_("Save to file '%1' is error.")).arg(fn), TVision::Error, this);
}

void VisRun::exportDiag( const string &idg )
{
    RunWdgView *rwdg;
    string dg = idg;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Export diagram"),_("No one page is present!")); return; }

    if(dg.empty())
    {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
	    if((rpg=findOpenPage(pgList[i_p])))
		rpg->shapeList("Diagram",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Export diagram"),_("No one diagram is present!")); return; }
	if(lst.size() == 1) dg = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select diagram for export."), _("Diagram export."), false, false);
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

    QPixmap img = QPixmap::grabWidget(rwdg);
    QString fileName = getFileName(_("Save diagram"), QString(_("Trend %1.png")).arg(expDiagCnt++),
	_("Images (*.png *.xpm *.jpg);;CSV file (*.csv)"), QFileDialog::AcceptSave);
    if(!fileName.isEmpty())
    {
	//>> Export to CSV
	if(fileName.indexOf(QRegExp("\\.csv$")) != -1)
	{
	    //>>> Open destination file
	    int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
	    if(fd < 0)
	    {
		mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
		return;
	    }

	    ShapeDiagram::ShpDt *dgDt = (ShapeDiagram::ShpDt*)rwdg->shpData;
	    string CSVr;
	    //>>> Trend type process
	    if(dgDt->type == 0)
	    {
		int firstPrm = -1, vPos = 0;
		//>>> Prepare header
		CSVr += _("\"Date and time\";\"us\"");
		for(unsigned i_p = 0; i_p < dgDt->prms.size(); i_p++)
    		    if(dgDt->prms[i_p].val().size() && dgDt->prms[i_p].color().isValid())
    		    {
			CSVr += ";\""+TSYS::path2sepstr(dgDt->prms[i_p].addr())+"\"";
			if(firstPrm < 0) firstPrm = i_p;
		    }
		CSVr += "\x0D\x0A";
		if(firstPrm < 0) return;
		//>>> Place data
		deque<ShapeDiagram::TrendObj::SHg> &baseVls = dgDt->prms[firstPrm].val();
		int64_t eTmVl = dgDt->tTime;
		int64_t bTmVl = eTmVl - 1e6*dgDt->tSize;
		for(unsigned i_v = 0; i_v < baseVls.size() && baseVls[i_v].tm <= eTmVl; i_v++)
		{
		    if(baseVls[i_v].tm < bTmVl) continue;
		    CSVr += TSYS::time2str(baseVls[i_v].tm/1000000,"\"%d/%m/%Y %H:%M:%S\"")+";"+i2s(baseVls[i_v].tm%1000000);
		    for(unsigned i_p = 0; i_p < dgDt->prms.size(); i_p++)
		    {
			ShapeDiagram::TrendObj &cPrm = dgDt->prms[i_p];
			if(cPrm.val().size() && cPrm.color().isValid())
			{
			    vPos = cPrm.val(baseVls[i_v].tm);
			    CSVr = CSVr + ";"+((vPos < (int)cPrm.val().size())?QLocale().toString(cPrm.val()[vPos].val).toStdString():"");
			}
		    }
		    CSVr += "\x0D\x0A";
		}
	    }
	    //>>> Frequency spectrum type
	    else if(dgDt->type == 1)
	    {
#if HAVE_FFTW3_H
		//>>> Prepare header
		CSVr += _("\"Frequency (Hz)\"");
		for(unsigned i_p = 0; i_p < dgDt->prms.size(); i_p++)
    		    if(dgDt->prms[i_p].fftN && dgDt->prms[i_p].color().isValid())
			CSVr += ";\""+TSYS::path2sepstr(dgDt->prms[i_p].addr())+"\"";
		CSVr += "\x0D\x0A";
		//>>> Place data
		int fftN = rwdg->size().width();		//Samples number
		double fftBeg = 1/dgDt->tSize;			//Minimum frequency or maximum period time (s)
		double fftEnd = (double)fftN*fftBeg/2;		//Maximum frequency or minimum period time (s)
		for(double i_frq = fftBeg; i_frq <= fftEnd; i_frq += fftBeg)
		{
		    CSVr += QLocale().toString(i_frq).toStdString();
		    for(unsigned i_p = 0; i_p < dgDt->prms.size(); i_p++)
		    {
			ShapeDiagram::TrendObj &cPrm = dgDt->prms[i_p];
    			if(cPrm.fftN && cPrm.color().isValid())
    			{
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
	    //>>> Save to file
	    bool fOK = (write(fd,CSVr.data(),CSVr.size()) == (int)CSVr.size());
	    ::close(fd);
	    if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error write to: %1.")).arg(fileName), TVision::Error, this);
	}
	//>> Export to image
	else if(!img.save(fileName))
	    mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
    }
}

void VisRun::exportDoc( const string &idoc )
{
    RunWdgView *rwdg;
    string doc = idoc;

    if(pgList.empty())	{ QMessageBox::warning(this,_("Export document"),_("No one page is present!")); return; }

    if(doc.empty())
    {
	RunPageView *rpg;
	vector<string> lst;
	for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
	    if((rpg=findOpenPage(pgList[i_p])))
		rpg->shapeList("Document",lst);
	if(lst.empty())	{ QMessageBox::warning(this,_("Export document"),_("No one document is present!")); return; }
	if(lst.size() == 1) doc = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg(this, QPixmap::fromImage(ico_t), _("Select document for export."), _("Document export."), false, false);
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
    if(!fileName.isEmpty())
    {
	int fd = open(fileName.toStdString().c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if(fd < 0)
	{
	    mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
	    return;
	}
	string rez;
	//>> Export to CSV
	if(fileName.indexOf(QRegExp("\\.csv$")) != -1)
	{
	    //>>> Parse document
	    XMLNode docTree;
	    docTree.load(((ShapeDocument::ShpDt*)rwdg->shpData)->doc, true);
	    XMLNode *curNode = &docTree;
	    vector<unsigned> treeStk;
	    treeStk.push_back(0);
	    while(!(!curNode->parent() && treeStk.back() >= curNode->childSize()))
	    {
		if(treeStk.back() < curNode->childSize())
		{
		    curNode = curNode->childGet(treeStk.back());
		    treeStk.push_back(0);
		    //>>> Check for marked table and process it
		    if(strcasecmp(curNode->name().c_str(),"table") == 0 && atoi(curNode->attr("export").c_str()))
		    {
			map<int,int>	rowSpn;
			XMLNode *tblN = NULL, *tblRow;
			string val;
			for(int i_st = 0; i_st < 4; i_st++)
			{
			    switch(i_st)
			    {
				case 0:	tblN = curNode->childGet("thead", 0, true);	break;
				case 1: tblN = curNode->childGet("tbody", 0, true);	break;
				case 2: tblN = curNode->childGet("tfoot", 0, true);	break;
				case 3: tblN = curNode;					break;
				default: tblN = NULL;
			    }
			    if(!tblN)	continue;
			    //>>> Rows process
			    for(unsigned i_n = 0; i_n < tblN->childSize(); i_n++)
			    {
				if(strcasecmp(tblN->childGet(i_n)->name().c_str(),"tr") != 0)	continue;
				tblRow = tblN->childGet(i_n);
				for(unsigned i_c = 0, i_cl = 0; i_c < tblRow->childSize(); i_c++)
				{
				    if(!(strcasecmp(tblRow->childGet(i_c)->name().c_str(),"th") == 0 || strcasecmp(tblRow->childGet(i_c)->name().c_str(),"td") == 0))
					continue;
				    while(rowSpn[i_cl] > 1) { rez += ";"; rowSpn[i_cl]--; i_cl++; }
				    rowSpn[i_cl] = atoi(tblRow->childGet(i_c)->attr("rowspan",false).c_str());
				    val = tblRow->childGet(i_c)->text(true,true);
				    for(size_t i_sz = 0; (i_sz=val.find("\"",i_sz)) != string::npos; i_sz += 2) val.replace(i_sz,1,2,'"');
				    rez += "\""+TSYS::strNoSpace(val)+"\";";
				    //>>>> Colspan process
				    int colSpan = atoi(tblRow->childGet(i_c)->attr("colspan",false).c_str());
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
	//>> Export to XHTML
	else rez = ((ShapeDocument::ShpDt*)rwdg->shpData)->toHtml();

	bool fOK = true;
	if(rez.empty())	mod->postMess(mod->nodePath().c_str(),QString(_("No data for export.")),TVision::Error,this);
	else fOK = (write(fd,rez.data(),rez.size()) == (int)rez.size());
	::close(fd);
	if(!fOK) mod->postMess(mod->nodePath().c_str(), QString(_("Error write to: %1.")).arg(fileName), TVision::Error, this);
    }
}

void VisRun::about()
{
    QMessageBox::about(this,windowTitle(),
	QString(_("%1 v%2.\n%3\nAuthor: %4\nDevelopers: %5\nLicense: %6\n\n%7 v%8.\n%9\nLicense: %10\nAuthor: %11\nWeb site: %12")).
	arg(mod->modInfo("Name").c_str()).arg(mod->modInfo("Version").c_str()).arg(mod->modInfo("Description").c_str()).
	arg(mod->modInfo("Author").c_str()).arg(mod->modInfo(_("Developers")).c_str()).arg(mod->modInfo("License").c_str()).
	arg(PACKAGE_NAME).arg(VERSION).arg(_(PACKAGE_DESCR)).arg(PACKAGE_LICENSE).arg(_(PACKAGE_AUTHOR)).arg(PACKAGE_SITE));
}

void VisRun::userChanged( const QString &oldUser, const QString &oldPass )
{
    //> Try second connect to session for permission check
    XMLNode req("connect");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess());
    if(cntrIfCmd(req))
    {
	mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	mWUser->setUser(oldUser);
	mWUser->setPass(oldPass);
	return;
    }
    req.clear()->setName("disconnect")->setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess());
    cntrIfCmd(req);

    //> Update pages after user change
    pgCacheClear();
    bool oldMenuVis = menuBar()->isVisible();
    menuBar()->setVisible(SYS->security().at().access(user(),SEC_WR,"root","root",RWRWR_));
    QApplication::processEvents();
    if(master_pg)
    {
	if(oldMenuVis != menuBar()->isVisible() && (windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen))
	{
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	    if(x_scale > 1 && x_scale < 1.05) x_scale = 1;
	    if(y_scale > 1 && y_scale < 1.05) y_scale = 1;
	    if(keepAspectRatio) x_scale = y_scale = vmin(x_scale, y_scale);
	    mess_debug(mod->nodePath().c_str(), _("Root page scale [%f:%f]."), x_scale, y_scale);
	}
	fullUpdatePgs();
    }
}

void VisRun::styleChanged( )
{
    //> Get current style
    XMLNode req("set");
    req.setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fstyle")->setText(i2s(style()));
    if( cntrIfCmd(req) )
    {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	return;
    }
    fullUpdatePgs();
}

void VisRun::aboutQt( )	{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void VisRun::fullScreen( bool vl )	{ setWindowState(vl?Qt::WindowFullScreen:Qt::WindowNoState); }

void VisRun::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void VisRun::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString(), NULL, TUIS::GetExecCommand);
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("No the manual '%1' found offline or online!")).arg(sender()->property("doc").toString()));
}

void VisRun::alarmAct( QAction *alrm )
{
    if(alrm == NULL) return;

    int quittance = 0;
    string qwdg;
    if(alrm->objectName() == "alarmLev")	quittance = 0xFF;
    else if(alrm->objectName() == "alarmLight")	quittance = 0x01;
    else if(alrm->objectName() == "alarmAlarm")	quittance = 0x02;
    else if(alrm->objectName() == "alarmSound")
    {
	quittance = 0x04;
	qwdg = alrmPlay->widget( );
    }
    else return;

    XMLNode req("quittance");
    req.setAttr("path","/ses_"+work_sess+"/%2fserv%2falarm")->
	setAttr("tmpl",u2s(quittance))->
	setAttr("wdg",qwdg);
    cntrIfCmd(req);

    //> Send event to master page
    if(master_pg) master_pg->attrSet("event",("ws_"+alrm->objectName()).toStdString());
}

void VisRun::initSess( const string &prj_it, bool crSessForce )
{
    //> Connect/create session
    src_prj = TSYS::pathLev(prj_it,0);
    if(src_prj.empty()) return;
    src_prj = src_prj.substr(4);
    work_sess = "";

    //> Get opened sessions list for our page and put dialog for connection
    XMLNode req("list");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("prj",src_prj);
    if(!crSessForce && !cntrIfCmd(req) && req.childSize())
    {
	//>> Prepare and execute a session selection dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet("vision_prj_run",NULL,true).c_str())) ico_t.load(":/images/prj_run.png");
	InputDlg conreq(this,QPixmap::fromImage(ico_t),
	    QString(_("Several sessions are already opened on the project \"%1\".\n"
		"You can create new or connect to present session. Please, select needed or press \"Cancel\".")).arg(src_prj.c_str()),
	    _("Select session for connection or new creation"),false,false);
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

    req.clear()->setName("connect")->setAttr("path","/%2fserv%2fsess");
    if(work_sess.empty()) req.setAttr("prj",src_prj);
    else req.setAttr("sess",work_sess);
    if(cntrIfCmd(req))
    {
	mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
	close();
	return;
    }

    work_sess = req.attr("sess");

    //> Set window title
    //>> Get project's name
    req.clear()->setName("get")->setAttr("path","/prj_"+src_prj+"/%2fobj%2fcfg%2fname");
    if(!cntrIfCmd(req))	setWindowTitle(req.text().c_str());
    else setWindowTitle(QString(_("Running project: %1")).arg(src_prj.c_str()));

    //> Set project's icon to window
    req.clear()->setAttr("path","/ses_"+work_sess+"/%2fico");
    if(!cntrIfCmd(req))
    {
	QImage img;
	string simg = TSYS::strDecode(req.text(),TSYS::base64);
	if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
	    setWindowIcon(QPixmap::fromImage(img));
    }
    else setWindowIcon(mod->icon());

    //> Get update period
    req.clear()->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fper");
    if(!cntrIfCmd(req)) mPeriod = atoi(req.text().c_str());

    //> Get current style
    req.clear()->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fstyle");
    if(!cntrIfCmd(req))
    {
	setStyle(atoi(req.text().c_str()));
	//> Check for styles present
	if(style() < 0)
	{
	    req.clear()->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fstLst");
	    if(!cntrIfCmd(req) && req.childSize() <= 1) mStlBar->setVisible(false);
	}
    }

    //> Get project's flags
    req.clear()->setName("get")->setAttr("path","/prj_"+src_prj+"/%2fobj%2fcfg%2fflgs");
    if(!cntrIfCmd(req))
    {
	if(atoi(req.text().c_str())&0x01)	setWindowState(Qt::WindowMaximized);
	else if(atoi(req.text().c_str())&0x02)	actFullScr->setChecked(true);
	keepAspectRatio = atoi(req.text().c_str())&0x04;
    }

    //> Get open pages list
    req.clear()->setName("openlist")->setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg");
    if(!cntrIfCmd(req))
	for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++)
	{
	    pgList.push_back(req.childGet(i_ch)->text());
	    callPage(req.childGet(i_ch)->text());
	}
    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //> Open direct-selected page
    if(!TSYS::pathLev(prj_it,1).empty())
    {
	//>> Convert project path to session path
	string prj_el;
	string ses_it = "/ses_"+work_sess;
	int i_el = 1;
	while((prj_el=TSYS::pathLev(prj_it,i_el++)).size())
	    if(prj_el.substr(0,3) != "pg_") break;
	    else ses_it += "/"+prj_el;

	//>> Send open command
	req.clear()->setName("open")->setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("pg",ses_it);
	cntrIfCmd(req);

	callPage(ses_it);
    }

    QCoreApplication::processEvents();

    //> Start timer
    updateTimer->start(period());
}

void VisRun::fullUpdatePgs( )
{
    for(unsigned i_p = 0; i_p < pgList.size(); i_p++)
    {
	RunPageView *pg = master_pg->findOpenPage(pgList[i_p]);
	if(pg) pg->update(true);
    }
}

void VisRun::callPage( const string& pg_it, bool updWdg )
{
    vector<int> idst;
    string stmp;

    //> Scan and update opened page
    if(master_pg)
    {
	RunPageView *pg = master_pg->findOpenPage(pg_it);
	if(pg)
	{
	    if(!(wPrcCnt%(5000/vmin(5000,period()))))	pg->update(false, NULL, true);
	    else if(updWdg) pg->update(false);
	    return;
	}
    }

    // Get group and parent page
    string pgGrp = wAttrGet(pg_it,"pgGrp");
    string pgSrc = wAttrGet(pg_it,"pgOpenSrc");

    //> Check for master page replace
    if(!master_pg || pgGrp == "main" || master_pg->pgGrp() == pgGrp)
    {
	//>> Send close command
	if(master_pg)
	{
	    XMLNode req("close"); req.setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("pg",master_pg->id());
	    cntrIfCmd(req);
	}

	//>> Create widget view
	master_pg = new RunPageView(pg_it,this,centralWidget());
	//master_pg->load("");
	master_pg->setFocusPolicy(Qt::StrongFocus);
	((QScrollArea *)centralWidget())->setWidget(master_pg);
	if(!(windowState()&(Qt::WindowFullScreen|Qt::WindowMaximized)))
	{
	    QRect ws = QApplication::desktop()->availableGeometry(this);
	    resize(vmin(master_pg->size().width()+10,ws.width()-10), vmin(master_pg->size().height()+55,ws.height()-10));
	}
	else x_scale = y_scale = 1.0;
    }
    //> Put to check for include
    else master_pg->callPage(pg_it,pgGrp,pgSrc);
}

void VisRun::pgCacheClear( )
{
    while( !cache_pg.empty() )
    {
	delete cache_pg.front();
	cache_pg.pop_front();
    }
}

void VisRun::pgCacheAdd( RunPageView *wdg )
{
    if( !wdg ) return;
    cache_pg.push_front(wdg);
    while( cache_pg.size() > 100 )
    {
	delete cache_pg.back();
	cache_pg.pop_back();
    }
}

RunPageView *VisRun::pgCacheGet( const string &id )
{
    RunPageView *pg = NULL;

    for(unsigned i_pg = 0; i_pg < cache_pg.size(); i_pg++)
	if( cache_pg[i_pg]->id() == id )
	{
	    pg = cache_pg[i_pg];
	    cache_pg.erase(cache_pg.begin()+i_pg);
	    break;
	}

    return pg;
}

RunPageView *VisRun::findOpenPage( const string &pg )
{
    if( !master_pg )	return NULL;

    return master_pg->findOpenPage(pg);
}

RunWdgView *VisRun::findOpenWidget( const string &wdg )
{
    int woff = 0;
    for(int off = 0; true; woff = off)
    {
	string sel = TSYS::pathLev(wdg,0,true,&off);
	if(sel.empty() || sel.substr(0,4) == "wdg_")	break;
    }
    RunPageView *rpg = findOpenPage(wdg.substr(0,woff));
    if(!rpg )	return NULL;
    if(woff >= (int)wdg.size())	return rpg;

    return rpg->findOpenWidget(wdg);
}

string VisRun::wAttrGet( const string &path, const string &attr )
{
    XMLNode req("get"); req.setAttr("path",path+"/%2fattr%2f"+attr);
    if( !cntrIfCmd(req) ) return req.text();
    return "";
}

bool VisRun::wAttrSet( const string &path, const string &attr, const string &val )
{
    XMLNode req("set"); req.setAttr("path",path+"/%2fserv%2fattr");
    req.childAdd("el")->setAttr("id",attr)->setText(val);
    return !cntrIfCmd(req);
}

void VisRun::alarmSet( unsigned alarm )
{
    unsigned ch_tp = alarm^mAlrmSt;

    //> Check for early this session running equalent project
    bool isMaster = true;
    for(unsigned i_w = 0; i_w < mod->mn_winds.size(); i_w++)
	if(qobject_cast<VisRun*>(mod->mn_winds[i_w]) && ((VisRun*)mod->mn_winds[i_w])->srcProject() == srcProject())
	{
	    if(((VisRun*)mod->mn_winds[i_w])->workSess() != workSess()) isMaster = false;
	    break;
	}

    //> Alarm types init
    //>> Set momo sound alarm
    if(isMaster && (ch_tp>>16)&TVision::Alarm)
    {
	const char *spkEvDev = "/dev/input/by-path/platform-pcspkr-event-spkr";
	int hd = open(spkEvDev,O_WRONLY);
	if(hd < 0) mess_warning(mod->nodePath().c_str(),_("Error open: %s"),spkEvDev);
	else
	{
	    input_event ev;
	    ev.time.tv_sec = time(NULL);
	    ev.type = EV_SND;
	    ev.code = SND_TONE;
	    ev.value = ((alarm>>16)&TVision::Alarm) ? 1000 : 0;
	    bool fOK = (write(hd,&ev,sizeof(ev)) == sizeof(ev));
	    ::close(hd);
	    if(!fOK) mess_warning(mod->nodePath().c_str(), _("Error write to: %s"), spkEvDev);
	}
    }
    //>> Set speach or sound alarm
    if(isMaster && (alarm>>16)&TVision::Sound && !alrmPlay->isRunning() && !alrmPlay->playData().empty()) alrmPlay->start();

    //> Alarm action indicators update
    //>> Alarm level icon update
    if(ch_tp&0xFF || (alarm>>16)&(TVision::Light|TVision::Alarm|TVision::Sound) || !alrLevSet)
    {
	int alarmLev = alarm&0xFF;
	actAlrmLev->setToolTip(QString(_("Alarm level: %1")).arg(alarmLev));

	QImage lens(":/images/alarmLev.png");
	QImage levImage(lens.size(),lens.format());

	QPainter painter(&levImage);
	//QColor lclr( alarmLev ? 224 : 0, alarmLev ? 224-(int)(0.87*alarmLev) : 224, 0 );
	QColor lclr( alarmLev ? 255 : 0, alarmLev ? 255-alarmLev : 255, 0 );

	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(levImage.rect(),Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	if( !((alarm>>16)&(TVision::Light|TVision::Alarm|TVision::Sound) && alrLevSet) )
	{
	    for( int i_x = 0; i_x < lens.size().width(); i_x++ )
		for( int i_y = 0; i_y < lens.size().height(); i_y++ )
		    if( lens.pixel(i_x,i_y)&0xFF000000 )	levImage.setPixel(i_x,i_y,lclr.rgba());
	    alrLevSet = true;
	} else alrLevSet = false;

	painter.drawImage(0, 0, lens);
	painter.end();
	actAlrmLev->setIcon(QPixmap::fromImage(levImage));
    }
    //> Alarm buttons status process
    for( int i_b = 0; i_b < 3; i_b++ )
    {
	QAction *actAlrm = (i_b==0) ? actAlrmLight : ((i_b==1) ? actAlrmAlarm : actAlrmSound);
	if( (ch_tp>>8)&(0x01<<i_b) )	actAlrm->setVisible((alarm>>8)&(0x01<<i_b));
	if( (ch_tp>>16)&(0x01<<i_b) )	actAlrm->setEnabled((alarm>>16)&(0x01<<i_b));
    }

    mAlrmSt = alarm;
}

string VisRun::cacheResGet( const string &res )
{
    map<string,CacheEl>::iterator ires = mCacheRes.find(res);
    if( ires == mCacheRes.end() ) return "";
    ires->second.tm = time(NULL);
    return ires->second.val;
}

void VisRun::cacheResSet( const string &res, const string &val )
{
    if( val.size() > 1024*1024 ) return;
    mCacheRes[res] = CacheEl(time(NULL),val);
    if( mCacheRes.size() > 100 )
    {
	map<string,CacheEl>::iterator ilast = mCacheRes.begin();
	for( map<string,CacheEl>::iterator ires = mCacheRes.begin(); ires != mCacheRes.end(); ++ires )
	    if( ires->second.tm < ilast->second.tm )	ilast = ires;
	mCacheRes.erase(ilast);
    }
}

void VisRun::updatePage( )
{
    int64_t d_cnt = 0;
    if(winClose) return;

    int rez;

    if(mess_lev() == TMess::Debug) d_cnt = TSYS::curTime();

    //> Pages update
    XMLNode req("openlist");
    req.setAttr("tm",u2s(reqtm))->
	setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg");

    if(!(rez=cntrIfCmd(req)))
    {
	//>> Check for delete pages
	RunPageView *pg;
	for(unsigned i_p = 0, i_ch; i_p < pgList.size(); i_p++)
	{
	    for(i_ch = 0; i_ch < req.childSize(); i_ch++)
		if(pgList[i_p] == req.childGet(i_ch)->text())
		    break;
	    if(i_ch < req.childSize() || !(master_pg && (pg=master_pg->findOpenPage(pgList[i_p])))) continue;
	    if(!pg->property("cntPg").toString().isEmpty())
		((RunWdgView*)TSYS::str2addr(pg->property("cntPg").toString().toStdString()))->setPgOpenSrc("");
	    else
	    {
		if(pg != master_pg)	pg->deleteLater();
		else
		{
		    ((QScrollArea *)centralWidget())->setWidget(new QWidget());
		    master_pg = NULL;
		}
	    }
	}

	//>> Process opened pages
	pgList.clear();
	for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++)
	{
	    pgList.push_back(req.childGet(i_ch)->text());
	    callPage(req.childGet(i_ch)->text(),atoi(req.childGet(i_ch)->attr("updWdg").c_str()));
	}
    }
    //>> Restore closed session of used project.
    else if(rez == 2)
    {
	mess_warning(mod->nodePath().c_str(),_("Session creation restore for '%s'."),prj_it.c_str());
	updateTimer->stop();
	initSess(prj_it, crSessForce);
	return;
    }

    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //> Alarms update (0.5 second update)
    if((wPrcCnt%(500/vmin(500,period()))) == 0)
    {
	//>> Get alarm status
	unsigned wAlrmSt = alarmSt( );
	req.clear()->
	    setName("get")->
	    setAttr("mode", "stat")->
	    setAttr("path", "/ses_"+work_sess+"/%2fserv%2falarm");
	if(!cntrIfCmd(req)) wAlrmSt = atoi(req.attr("alarmSt").c_str());

	//>> Get sound resources for play
	if(alarmTp(TVision::Sound,true) && !alrmPlay->isRunning())
	{
	    req.clear()->
		setName("get")->
		setAttr("mode", "sound")->
		setAttr("path", "/ses_"+work_sess+"/%2fserv%2falarm")->
		setAttr("tm", u2s(alrmPlay->time()))->
		setAttr("wdg", alrmPlay->widget());
	    if(!cntrIfCmd(req))
	    {
		alrmPlay->setTime(strtoul(req.attr("tm").c_str(),NULL,10));
		alrmPlay->setWidget(req.attr("wdg"));
		alrmPlay->setData(TSYS::strDecode(req.text(),TSYS::base64));
	    }
	}

	//>> Set alarm
	alarmSet(wAlrmSt);
    }

    //> Old pages from cache for close checking
    for(unsigned i_pg = 0; i_pg < cache_pg.size(); )
	if(mod->cachePgLife() > 0.01 && (period()*(reqTm()-cache_pg[i_pg]->reqTm())/1000) > (unsigned)(mod->cachePgLife()*60*60))
	{
	    delete cache_pg[i_pg];
	    cache_pg.erase(cache_pg.begin()+i_pg);
	}
	else i_pg++;

    if(mess_lev() == TMess::Debug)
    {
	upd_tm += 1e-3*(TSYS::curTime()-d_cnt);
	if(!(1000/vmin(1000,period()) && wPrcCnt%(1000/vmin(1000,period()))))
	{
	    mess_debug(mod->nodePath().c_str(), _("Session '%s' update time %f ms."), workSess().c_str(), upd_tm);
	    upd_tm = 0;
	}
    }

    //> Time update
    if(mWTime->isVisible() && !(wPrcCnt%vmax(1000/vmin(1000,period()),1)))
    {
	QDateTime dtm = QDateTime::currentDateTime();
	mWTime->setText( locale().toString(dtm,"hh:mm:ss\nddd, d MMM") );
	mWTime->setToolTip( locale().toString(dtm,"dddd, dd-MMM-yyyy") );
    }

    //> Scale for full screen check
    if(centralWidget() && master_pg && !(wPrcCnt%vmax(1000/vmin(1000,period()),1)) &&
	(windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen))
    {
	float xSc = (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	float ySc = (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	if(xSc < 1 || ySc < 1)
	{
	    x_scale *= xSc;
	    y_scale *= ySc;
	    //> Proportional scale
	    if(keepAspectRatio) x_scale = y_scale = vmin(x_scale,y_scale);

	    fullUpdatePgs();
	}
    }

    wPrcCnt++;
}

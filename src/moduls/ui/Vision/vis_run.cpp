
//OpenSCADA system module UI.Vision file: vis_run.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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

#include <sys/stat.h>
#include <fcntl.h>

#include <linux/input.h>

#include <QApplication>
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
#include <QPrinter>
#include <QPrintDialog>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>

#include <config.h>
#include <tsys.h>
#include "tvision.h"
#include "vis_run_widgs.h"
#include "vis_shapes.h"
#include "vis_run.h"

using namespace VISION;

VisRun::VisRun( const string &prj_it, const string &open_user, const string &user_pass, const string &VCAstat, bool crSessForce, QWidget *parent ) :
    QMainWindow(parent), winClose(false), master_pg(NULL), mPeriod(1000), wPrcCnt(0), reqtm(1), x_scale(1.0), y_scale(1.0), mAlrmSt(0xFFFFFF)
{
    QImage ico_t;

    setAttribute(Qt::WA_DeleteOnClose,true);
    mod->regWin( this );

    setWindowTitle(_("Vision runtime"));
    setWindowIcon(mod->icon());

    //> Create actions
    //>> Generic actions
    //>>> Print
    if(!ico_t.load(TUIS::icoPath("print").c_str())) ico_t.load(":/images/print.png");
    QMenu *menuPrint = new QMenu(_("&Print"), this);
    menuPrint->setIcon(QPixmap::fromImage(ico_t));
    menuPrint->menuAction()->setShortcut(QKeySequence::Print);
    menuPrint->menuAction()->setToolTip(_("Print the master page"));
    menuPrint->menuAction()->setWhatsThis(_("The button for printing of the master page by default"));
    menuPrint->menuAction()->setStatusTip(_("Press for printing of the master page by default."));
    connect(menuPrint->menuAction(), SIGNAL(activated()), this, SLOT(print()));
    QAction *actPrintPg = new QAction(_("Page"),this);
    actPrintPg->setToolTip(_("Print the selected page"));
    actPrintPg->setWhatsThis(_("The button for printing of the selected page"));
    actPrintPg->setStatusTip(_("Press for printing of the selected page."));
    connect(actPrintPg, SIGNAL(activated()), this, SLOT(printPg()));
    menuPrint->addAction(actPrintPg);
    QAction *actPrintDiag = new QAction(_("Diagram"),this);
    actPrintDiag->setToolTip(_("Print the selected diagram"));
    actPrintDiag->setWhatsThis(_("The button for printing of the selected diagram"));
    actPrintDiag->setStatusTip(_("Press for printing of the selected diagram."));
    connect(actPrintDiag, SIGNAL(activated()), this, SLOT(printDiag()));
    menuPrint->addAction(actPrintDiag);
    QAction *actPrintDoc = new QAction(_("Document"),this);
    actPrintDoc->setToolTip(_("Print the selected document"));
    actPrintDoc->setWhatsThis(_("The button for printing of the selected document"));
    actPrintDoc->setStatusTip(_("Press for printing of the selected document."));
    connect(actPrintDoc, SIGNAL(activated()), this, SLOT(printDoc()));
    menuPrint->addAction(actPrintDoc);
    //>>> Export
    if(!ico_t.load(TUIS::icoPath("export").c_str())) ico_t.load(":/images/export.png");
    QMenu *menuExport = new QMenu(_("&Export"), this);
    menuExport->setIcon(QPixmap::fromImage(ico_t));
    menuExport->menuAction()->setToolTip(_("Export the master page"));
    menuExport->menuAction()->setWhatsThis(_("The button for exporting of the master page by default"));
    menuExport->menuAction()->setStatusTip(_("Press for exporting of the master page by default."));
    connect(menuExport->menuAction(), SIGNAL(activated()), this, SLOT(exportDef()));
    QAction *actExpPg = new QAction(_("Page"),this);
    actExpPg->setToolTip(_("Export the selected page"));
    actExpPg->setWhatsThis(_("The button for exporting of the selected page"));
    actExpPg->setStatusTip(_("Press for exporting of the selected page."));
    connect(actExpPg, SIGNAL(activated()), this, SLOT(exportPg()));
    menuExport->addAction(actExpPg);
    QAction *actExpDiag = new QAction(_("Diagram"),this);
    actExpDiag->setToolTip(_("Export the selected diagram"));
    actExpDiag->setWhatsThis(_("The button for exporting of the selected diagram"));
    actExpDiag->setStatusTip(_("Press for exporting of the selected diagram."));
    connect(actExpDiag, SIGNAL(activated()), this, SLOT(exportDiag()));
    menuExport->addAction(actExpDiag);
    QAction *actExpDoc = new QAction(_("Document"),this);
    actExpDoc->setToolTip(_("Export the selected document"));
    actExpDoc->setWhatsThis(_("The button for exporting of the selected document"));
    actExpDoc->setStatusTip(_("Press for exporting of the selected document."));
    connect(actExpDoc, SIGNAL(activated()), this, SLOT(exportDoc()));
    menuExport->addAction(actExpDoc);
    //>>> Close
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close Vision window"));
    actClose->setWhatsThis(_("The button for closing Vision runtime window"));
    actClose->setStatusTip(_("Press to close of the current Vision runtime window."));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //>>> Quit
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit from OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA system."));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //>> View actions
    //>>> Fullscreen
    actFullScr = new QAction(_("Full screen"),this);
    actFullScr->setCheckable(true);
    actFullScr->setToolTip(_("Full screen toggle"));
    actFullScr->setWhatsThis(_("The button for full screen toggle"));
    actFullScr->setStatusTip(_("Press for toggle full screen."));
    connect(actFullScr, SIGNAL(toggled(bool)), this, SLOT(fullScreen(bool)));

    //>> Help actions
    //>>> About "System info"
    if(!ico_t.load(TUIS::icoPath("help").c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setShortcut(Qt::Key_F1);
    actAbout->setToolTip(_("Programm and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display the programm and OpenSCADA information"));
    actAbout->setStatusTip(_("Press for display the programm and OpenSCADA information."));
    connect(actAbout, SIGNAL(activated()), this, SLOT(about()));
    //>>> About Qt
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using QT information"));
    actQtAbout->setStatusTip(_("Press to get the using QT information."));
    connect(actQtAbout, SIGNAL(activated()), this, SLOT(aboutQt()));
    //>>> What is
    //if(!ico_t.load(TUIS::icoPath("contexthelp").c_str())) ico_t.load(":/images/contexthelp.png");
    //QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    //actWhatIs->setToolTip(_("The button for requestion about GUI elements"));
    //actWhatIs->setWhatsThis(_("Get request about user interface elements"));
    //actWhatIs->setStatusTip(_("Press for requesting about user interface elements."));
    //connect(actWhatIs, SIGNAL(activated()), this, SLOT(enterWhatsThis()));

    //>> Alarms actions
    //>>> Alarm level display button and full alarms quitance
    if(!ico_t.load(TUIS::icoPath("alarmLev").c_str())) ico_t.load(":/images/alarmLev.png");
    actAlrmLev = new QAction( QPixmap::fromImage(ico_t), _("Alarm level"), this );
    actAlrmLev->setObjectName("alarmLev");
    actAlrmLev->setToolTip(_("Alarm level"));
    actAlrmLev->setWhatsThis(_("The button for all alarms quitance"));
    actAlrmLev->setStatusTip(_("Press for all alarms quitance."));
    //>>> Alarm by Light
    if(!ico_t.load(TUIS::icoPath("alarmLight").c_str())) ico_t.load(":/images/alarmLight.png");
    actAlrmLight = new QAction( QPixmap::fromImage(ico_t), _("Light alarm"), this );
    actAlrmLight->setObjectName("alarmLight");
    actAlrmLight->setToolTip(_("Light alarm"));
    actAlrmLight->setWhatsThis(_("The button for all light alarms quitance"));
    actAlrmLight->setStatusTip(_("Press for all light alarms quitance."));
    actAlrmLight->setVisible(false);
    //>>> Alarm by mono sound (PC speaker)
    if(!ico_t.load(TUIS::icoPath("alarmAlarm").c_str())) ico_t.load(":/images/alarmAlarm.png");
    actAlrmAlarm = new QAction( QPixmap::fromImage(ico_t), _("Speaker alarm"), this );
    actAlrmAlarm->setObjectName("alarmAlarm");
    actAlrmAlarm->setToolTip(_("PC speaker alarm"));
    actAlrmAlarm->setWhatsThis(_("The button for all PC speaker alarms quitance"));
    actAlrmAlarm->setStatusTip(_("Press for all PC speaker alarms quitance."));
    actAlrmAlarm->setVisible(false);
    //>>> Alarm by sound or synthesis of speech
    if(!ico_t.load(TUIS::icoPath("alarmSound").c_str())) ico_t.load(":/images/alarmSound.png");
    actAlrmSound = new QAction( QPixmap::fromImage(ico_t), _("Sound/speech alarm"), this );
    actAlrmSound->setObjectName("alarmSound");
    actAlrmSound->setToolTip(_("Sound or speech alarm"));
    actAlrmSound->setWhatsThis(_("The button for all sound or speech alarms quitance"));
    actAlrmSound->setStatusTip(_("Press for all sound or speech alarms quitance."));
    actAlrmSound->setVisible(false);

    //> Create menu
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
    mn_help->addSeparator();
    //mn_help->addAction(actWhatIs);

    //> Init tool bars
    //>> Alarms tools bar
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

    //> Init status bar
    mWTime = new QLabel( this );
    mWTime->setVisible(false);
    mWTime->setWhatsThis(_("This label displays curent system's time."));
    statusBar()->insertPermanentWidget(0,mWTime);
    mWUser = new UserStBar( open_user.c_str(), user_pass.c_str(), VCAstat.c_str(), this );
    mWUser->setWhatsThis(_("This label displays curent user."));
    mWUser->setToolTip(_("Field for display of the current user."));
    mWUser->setStatusTip(_("Double click to change user."));
    connect( mWUser, SIGNAL(userChanged(const QString&,const QString&)), this, SLOT(userChanged(const QString&,const QString&)) );
    statusBar()->insertPermanentWidget(0,mWUser);
    mWStat = new QLabel( VCAStation().c_str(), this );
    mWStat->setWhatsThis(_("This label displays used VCA engine station."));
    mWStat->setToolTip(_("Field for display of the used VCA engine station."));
    statusBar()->insertPermanentWidget(0,mWStat);
    statusBar()->insertPermanentWidget(0,toolBarStatus);

    //> Init scroller
    QScrollArea *scrl = new QScrollArea;
    scrl->setFocusPolicy( Qt::NoFocus );
    setCentralWidget( scrl );

    //> Create timers
    //>> End run timer
    endRunTimer   = new QTimer( this );
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);
    //>> Update timer
    updateTimer = new QTimer( this );
    updateTimer->setSingleShot(false);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePage()));

    alrmPlay = new SndPlay(this);

    menuBar()->setVisible(SYS->security().at().access(user(),SEQ_WR,"root","root",RWRWR_));

    resize( 600, 400 );

    //> Init session
    initSess(prj_it,crSessForce);

    //mWStat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000 );

    alarmSet(0);
}

VisRun::~VisRun()
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
}

string VisRun::user()
{
    return mWUser->user().toAscii().data();
}

string VisRun::password( )
{
    return mWUser->pass().toAscii().data();
}

string VisRun::VCAStation( )
{
    return mWUser->VCAStation().toAscii().data();
}

int VisRun::cntrIfCmd( XMLNode &node, bool glob )
{
    return mod->cntrIfCmd(node,user(),password(),VCAStation(),glob);
}

void VisRun::closeEvent( QCloseEvent* ce )
{
    winClose = true;
    ce->accept();
}

void VisRun::resizeEvent( QResizeEvent *ev )
{
    if( ev && ev->oldSize().isValid() && master_pg )
    {
	float x_scale_old = x_scale;
	float y_scale_old = y_scale;
	if( windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen )
	{
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	}else x_scale = y_scale = 1.0;
	if( x_scale_old != x_scale || y_scale_old != y_scale )	fullUpdatePgs();
    }
    mWTime->setVisible(windowState()==Qt::WindowFullScreen);
}

void VisRun::endRunChk( )
{
    if( mod->endRun() ) close();
}

void VisRun::quitSt()
{
    SYS->stop();
}

void VisRun::print( )
{
    if( master_pg ) printPg( master_pg->id() );
}

void VisRun::printPg( const string &ipg )
{
    RunPageView *rpg;
    string pg = ipg;

    if( pgList.empty() )	{ QMessageBox::warning(this,_("Print page"),_("No one page for print is present!")); return; }

    if( pg.empty() && pgList.size() == 1 )	pg = pgList[0];
    if( pg.empty() && pgList.size() > 1 )
    {
	//> Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoPath("print").c_str())) ico_t.load(":/images/print.png");
	InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select page for print."), _("Page print."), false, false );
	sdlg.edLay()->addWidget( new QLabel(_("Pages:"),&sdlg), 2, 0 );
	QComboBox *spg = new QComboBox(&sdlg);
	sdlg.edLay()->addWidget( spg, 2, 1 );
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	    if( (rpg=findOpenPage(pgList[i_p])) )
		spg->addItem((rpg->name()+" ("+pgList[i_p]+")").c_str(),pgList[i_p].c_str());
	if( sdlg.exec() != QDialog::Accepted )	return;
	pg = spg->itemData(spg->currentIndex()).toString().toAscii().data();
    }

    //> Find need page
    rpg = master_pg;
    if( rpg->id() != pg )	rpg = findOpenPage(pg);
    if( !rpg ) return;

    string pnm = rpg->name();
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer,this);
    dlg.setWindowTitle(QString(_("Print page: \"%1\" (%2)")).arg(pnm.c_str()).arg(pg.c_str()));
    if( dlg.exec() == QDialog::Accepted )
    {
	int fntSize = 35;
	QSize papl(2048,2048*printer.paperRect().height()/printer.paperRect().width());
	QSize pagl(papl.width()*printer.pageRect().width()/printer.paperRect().width(), papl.height()*printer.pageRect().height()/printer.paperRect().height());

	QPainter painter;
	painter.begin(&printer);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(printer.paperRect());

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

    if( pgList.empty() )	{ QMessageBox::warning(this,_("Print diagram"),_("No one page is present!")); return; }

    if( dg.empty() )
    {
	RunPageView *rpg;
	vector<string> lst;
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	    if( (rpg=findOpenPage(pgList[i_p])) )
		rpg->shapeList("Diagram",lst);
	if( lst.empty() )	{ QMessageBox::warning(this,_("Print diagram"),_("No one diagram is present!")); return; }
	if( lst.size() == 1 )	dg = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoPath("print").c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select diagramm for print."), _("Diagram print."), false, false );
	    sdlg.edLay()->addWidget( new QLabel(_("Diagrams:"),&sdlg), 2, 0 );
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for( int i_l = 0; i_l < lst.size(); i_l++ )
		if( (rwdg=findOpenWidget(lst[i_l])) )
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if( sdlg.exec() != QDialog::Accepted )	return;
	    dg = spg->itemData(spg->currentIndex()).toString().toAscii().data();
	}
    }

    if( !(rwdg=findOpenWidget(dg)) )	return;

    string dgnm = rwdg->name();
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer,this);
    dlg.setWindowTitle(QString(_("Print diagram: \"%1\" (%2)")).arg(dgnm.c_str()).arg(dg.c_str()));
    if( dlg.exec() == QDialog::Accepted )
    {
	int fntSize = 35;
	QSize papl(2048,2048*printer.paperRect().height()/printer.paperRect().width());
	QSize pagl(papl.width()*printer.pageRect().width()/printer.paperRect().width(), papl.height()*printer.pageRect().height()/printer.paperRect().height());

	ShapeDiagram::ShpDt *sD = (ShapeDiagram::ShpDt*)rwdg->shpData;
	int elLine = sD->prms.size()/2+((sD->prms.size()%2)?1:0);

	QPainter painter;
	painter.begin(&printer);
	painter.setWindow(QRect(QPoint(0,0),papl));
	painter.setViewport(printer.paperRect());

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
	for( int i_e = 0; i_e < sD->prms.size(); i_e++ )
	{
	    QPoint pnt((i_e/elLine)*(pagl.width()/2),im.height()+fntSize*(2+i_e%elLine));
	    if( TSYS::strNoSpace(sD->prms[i_e].addr()).empty() ) continue;
	    painter.fillRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5),QBrush(QColor(sD->prms[i_e].color().c_str())));
	    painter.drawRect(QRect(pnt.x()+2,pnt.y()+2,fntSize-5,fntSize-5));
	    painter.drawText(QRect(pnt.x()+fntSize,pnt.y(),pagl.width()/2,fntSize),Qt::AlignLeft,
		QString("%1 [%2...%3]").arg(sD->prms[i_e].addr().c_str()).arg(sD->prms[i_e].bordL()).arg(sD->prms[i_e].bordU()));
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
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	    if( (rpg=findOpenPage(pgList[i_p])) )
		rpg->shapeList("Document",lst);
	if( lst.empty() )	{ QMessageBox::warning(this,_("Print document"),_("No one document is present!")); return; }
	if( lst.size() == 1 )	doc = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoPath("print").c_str())) ico_t.load(":/images/print.png");
	    InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select document for print."), _("Document print."), false, false );
	    sdlg.edLay()->addWidget( new QLabel(_("Document:"),&sdlg), 2, 0 );
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for( int i_l = 0; i_l < lst.size(); i_l++ )
		if( (rwdg=findOpenWidget(lst[i_l])) )
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if( sdlg.exec() != QDialog::Accepted )	return;
	    doc = spg->itemData(spg->currentIndex()).toString().toAscii().data();
	}
    }

    if( !(rwdg=findOpenWidget(doc)) )	return;

    string docnm = rwdg->name();
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer,this);
    dlg.setWindowTitle(QString(_("Print document: \"%1\" (%2)")).arg(docnm.c_str()).arg(doc.c_str()));
    if( dlg.exec() == QDialog::Accepted )
	((ShapeDocument::ShpDt*)rwdg->shpData)->web->document()->print(&printer);
}

void VisRun::exportDef( )
{
    if( master_pg ) exportPg( master_pg->id() );
}

void VisRun::exportPg( const string &ipg )
{
    RunPageView *rpg;
    string pg = ipg;

    if( pgList.empty() )	{ QMessageBox::warning(this,_("Export page"),_("No one page for export is present!")); return; }

    if( pg.empty() && pgList.size() == 1 )	pg = pgList[0];
    if( pg.empty() && pgList.size() > 1 )
    {
	//> Make select page dialog
	QImage ico_t;
	if(!ico_t.load(TUIS::icoPath("export").c_str())) ico_t.load(":/images/export.png");
	InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select page for export."), _("Page export."), false, false );
	sdlg.edLay()->addWidget( new QLabel(_("Pages:"),&sdlg), 2, 0 );
	QComboBox *spg = new QComboBox(&sdlg);
	sdlg.edLay()->addWidget( spg, 2, 1 );
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	    if( (rpg=findOpenPage(pgList[i_p])) )
		spg->addItem((rpg->name()+" ("+pgList[i_p]+")").c_str(),pgList[i_p].c_str());
	if( sdlg.exec() != QDialog::Accepted )	return;
	pg = spg->itemData(spg->currentIndex()).toString().toAscii().data();
    }

    //> Find need page
    rpg = master_pg;
    if( rpg->id() != pg )	rpg = findOpenPage(pg);
    if( !rpg ) return;

    QPixmap img = QPixmap::grabWidget(rpg);

    //> Call save file dialog -
    QString fileName = QFileDialog::getSaveFileName(this,_("Save page's image"),
	(TSYS::path2sepstr(rpg->name())+".png").c_str(), _("Images (*.png *.xpm *.jpg)"));
    if( !fileName.isEmpty() && !img.save(fileName) )
	mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
}

void VisRun::exportDiag( const string &idg )
{
    RunWdgView *rwdg;
    string dg = idg;

    if( pgList.empty() )	{ QMessageBox::warning(this,_("Export diagram"),_("No one page is present!")); return; }

    if( dg.empty() )
    {
	RunPageView *rpg;
	vector<string> lst;
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	    if( (rpg=findOpenPage(pgList[i_p])) )
		rpg->shapeList("Diagram",lst);
	if( lst.empty() )	{ QMessageBox::warning(this,_("Export diagram"),_("No one diagram is present!")); return; }
	if( lst.size() == 1 )	dg = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoPath("print").c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select diagramm for export."), _("Diagram export."), false, false );
	    sdlg.edLay()->addWidget( new QLabel(_("Diagrams:"),&sdlg), 2, 0 );
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for( int i_l = 0; i_l < lst.size(); i_l++ )
		if( (rwdg=findOpenWidget(lst[i_l])) )
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if( sdlg.exec() != QDialog::Accepted )	return;
	    dg = spg->itemData(spg->currentIndex()).toString().toAscii().data();
	}
    }

    if( !(rwdg=findOpenWidget(dg)) )	return;

    QPixmap img = QPixmap::grabWidget(rwdg);

    //> Call save file dialog -
    QString fileName = QFileDialog::getSaveFileName(this,_("Save diagram's image"),
	(TSYS::path2sepstr(rwdg->name())+".png").c_str(), _("Images (*.png *.xpm *.jpg)"));
    if( !fileName.isEmpty() && !img.save(fileName) )
	mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
}

void VisRun::exportDoc( const string &idoc )
{
    RunWdgView *rwdg;
    string doc = idoc;

    if( pgList.empty() )	{ QMessageBox::warning(this,_("Export document"),_("No one page is present!")); return; }

    if( doc.empty() )
    {
	RunPageView *rpg;
	vector<string> lst;
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	    if( (rpg=findOpenPage(pgList[i_p])) )
		rpg->shapeList("Document",lst);
	if( lst.empty() )	{ QMessageBox::warning(this,_("Export document"),_("No one document is present!")); return; }
	if( lst.size() == 1 )	doc = lst[0];
	else
	{
	    //> Make select diagrams dialog
	    QImage ico_t;
	    if(!ico_t.load(TUIS::icoPath("print").c_str())) ico_t.load(":/images/export.png");
	    InputDlg sdlg( this, QPixmap::fromImage(ico_t), _("Select document for export."), _("Document export."), false, false );
	    sdlg.edLay()->addWidget( new QLabel(_("Document:"),&sdlg), 2, 0 );
	    QComboBox *spg = new QComboBox(&sdlg);
	    sdlg.edLay()->addWidget( spg, 2, 1 );
	    for( int i_l = 0; i_l < lst.size(); i_l++ )
		if( (rwdg=findOpenWidget(lst[i_l])) )
		    spg->addItem((rwdg->name()+" ("+lst[i_l]+")").c_str(),lst[i_l].c_str());
	    if( sdlg.exec() != QDialog::Accepted )	return;
	    doc = spg->itemData(spg->currentIndex()).toString().toAscii().data();
	}
    }

    if( !(rwdg=findOpenWidget(doc)) )	return;

    //> Call save file dialog -
    QString fileName = QFileDialog::getSaveFileName(this,_("Save document"),(TSYS::path2sepstr(rwdg->name())+".html").c_str(), _("HTML (*.html)"));
    if( !fileName.isEmpty() )
    {
	int fd = ::open( fileName.toAscii().data(), O_WRONLY|O_CREAT|O_TRUNC, 0644 );
	if( fd < 0 )
	{
	    mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
	    return;
	}
	string rez = "<?xml version='1.0' ?>\n"
	    "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	    "'DTD/xhtml1-transitional.dtd'>\n"
	    "<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	    "<head>\n"
	    "  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	    "  <style type='text/css'>\n"+((ShapeDocument::ShpDt*)rwdg->shpData)->style+"</style>\n"
	    "</head>\n"+
	    ((ShapeDocument::ShpDt*)rwdg->shpData)->doc+
	    "</html>";
	::write(fd,rez.data(),rez.size());
	::close(fd);
    }
}

void VisRun::about()
{
    QMessageBox::about(this,windowTitle(),
	QString(_("%1 v%2.\n%3\nAutor: %4\nDevelopers: %5\nLicense: %6\n\n%7 v%8.\n%9\nLicense: %10\nAuthor: %11\nWeb site: %12")).
	arg(mod->modInfo("Name").c_str()).arg(mod->modInfo("Version").c_str()).arg(mod->modInfo("Descript").c_str()).
	arg(mod->modInfo("Author").c_str()).arg(mod->modInfo(_("Developers")).c_str()).arg(mod->modInfo("License").c_str()).
	arg(PACKAGE_NAME).arg(VERSION).arg(_(PACKAGE_DESCR)).arg(PACKAGE_LICENSE).arg(_(PACKAGE_AUTHOR)).arg(PACKAGE_SITE));
}

void VisRun::userChanged( const QString &oldUser, const QString &oldPass )
{
    //> Try second connect to session for permition check
    XMLNode req("connect");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess());
    if( cntrIfCmd(req) )
    {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	mWUser->setUser(oldUser);
	mWUser->setPass(oldPass);
	return;
    }
    req.clear()->setName("disconnect")->setAttr("path","/%2fserv%2fsess")->setAttr("sess",workSess());
    cntrIfCmd(req);

    //> Update pages after user change
    pgCacheClear();
    bool oldMenuVis = menuBar()->isVisible();
    menuBar()->setVisible(SYS->security().at().access(user(),SEQ_WR,"root","root",RWRWR_));
    QApplication::processEvents();
    if( master_pg )
    {
	if( oldMenuVis != menuBar()->isVisible() )
	{
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	}
	fullUpdatePgs();
    }
}

void VisRun::aboutQt()
{
    QMessageBox::aboutQt( this, mod->modInfo("Name").c_str() );
}

void VisRun::fullScreen( bool vl )
{
    if( vl ) setWindowState(Qt::WindowFullScreen);
    else setWindowState(Qt::WindowNoState);
}

/*void VisRun::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}*/

void VisRun::alarmAct( QAction *alrm )
{
    if( alarm == NULL ) return;

    int quitance = 0;
    string qwdg;
    if( alrm->objectName() == "alarmLev" )		quitance = 0xFF;
    else if( alrm->objectName() == "alarmLight" )	quitance = 0x01;
    else if( alrm->objectName() == "alarmAlarm" )	quitance = 0x02;
    else if( alrm->objectName() == "alarmSound" )
    {
	quitance = 0x04;
	qwdg = alrmPlay->widget( );
    }
    else return;

    XMLNode req("quittance");
    req.setAttr("path","/ses_"+work_sess+"/%2fserv%2falarm")->
	setAttr("tmpl",TSYS::uint2str(quitance))->
	setAttr("wdg",qwdg);
    cntrIfCmd(req);
}

void VisRun::initSess( const string &prj_it, bool crSessForce )
{
    //- Connect/create session -
    src_prj = TSYS::pathLev(prj_it,0);
    if( src_prj.empty() ) return;
    src_prj = src_prj.substr(4);
    work_sess = "";

    //-- Get opened sessions list for our page and put dialog for connection --
    XMLNode req("list");
    req.setAttr("path","/%2fserv%2fsess")->setAttr("prj",src_prj);
    if( !crSessForce && !cntrIfCmd(req) && req.childSize() )
    {
	//--- Prepare dialog ---
	QImage ico_t;
	if(!ico_t.load(TUIS::icoPath("vision_prj_run").c_str())) ico_t.load(":/images/prj_run.png");
	QDialog conreq(this);
	conreq.setWindowTitle(_("Select session for connection"));
	conreq.setMinimumSize( QSize( 150, 100 ) );
	conreq.setWindowIcon(QPixmap::fromImage(ico_t));
	conreq.setSizeGripEnabled(true);

	QVBoxLayout *dlg_lay = new QVBoxLayout(&conreq);
	dlg_lay->setMargin(10);
	dlg_lay->setSpacing(6);

	QHBoxLayout *intr_lay = new QHBoxLayout;
	intr_lay->setSpacing(6);

	QLabel *icon_lab = new QLabel(&conreq);
	icon_lab->setSizePolicy( QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum) );
	icon_lab->setPixmap(QPixmap::fromImage(ico_t));
	intr_lay->addWidget(icon_lab);

        QLabel *inp_lab = new QLabel(QString(_("Several sessions are already opened on the page %1. You can create new "
				       "session or connect to present session. Please, select needed session "
				       "and press 'Connect' or press 'Create' for creation of a new, individual, "
				       "session.")).arg(src_prj.c_str()),&conreq);
	inp_lab->setWordWrap(true);
	intr_lay->addWidget(inp_lab);
	dlg_lay->addItem(intr_lay);

	intr_lay = new QHBoxLayout;
	intr_lay->setSpacing(6);
	intr_lay->addStretch();
	QListWidget *ls_wdg = new QListWidget(&conreq);
	intr_lay->addWidget(ls_wdg);
	intr_lay->addStretch();
	dlg_lay->addItem(intr_lay);

	dlg_lay->addStretch();

	QFrame *sep = new QFrame(&conreq);
	sep->setFrameShape( QFrame::HLine );
	sep->setFrameShadow( QFrame::Raised );
	dlg_lay->addWidget( sep );

	QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,&conreq);
	but_box->button(QDialogButtonBox::Ok)->setText(_("Connect"));
	but_box->button(QDialogButtonBox::Cancel)->setText(_("Create"));
	connect(but_box, SIGNAL(accepted()), &conreq, SLOT(accept()));
	connect(but_box, SIGNAL(rejected()), &conreq, SLOT(reject()));
	dlg_lay->addWidget( but_box );
	conreq.resize(400,300);

	//--- Load session list ---
	for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
	    ls_wdg->addItem(req.childGet(i_ch)->text().c_str());
	ls_wdg->setCurrentRow(0);

	//--- Execute dialog ---
	int rez = 0;
	if( (rez=conreq.exec()) == QDialog::Accepted && ls_wdg->currentItem() )
	    work_sess = ls_wdg->currentItem()->text().toAscii().data();
    }

    req.clear()->setName("connect")->setAttr("path","/%2fserv%2fsess");
    if( work_sess.empty() ) req.setAttr("prj",src_prj);
    else req.setAttr("sess",work_sess);
    if( cntrIfCmd(req) )
    {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	close();
	return;
    }

    work_sess = req.attr("sess");

    //-- Set window title --
    //--- Get project's name ---
    req.clear()->setName("get")->setAttr("path","/prj_"+src_prj+"/%2fobj%2fcfg%2fname");
    if( !cntrIfCmd(req) )	setWindowTitle(req.text().c_str());
    else setWindowTitle(QString(_("Runing project: %1")).arg(src_prj.c_str()));

    //-- Set project's icon to window --
    req.clear()->setAttr("path","/ses_"+work_sess+"/%2fico");
    if( !cntrIfCmd(req) )
    {
	QImage img;
	string simg = TSYS::strDecode(req.text(),TSYS::base64);
	if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
	    setWindowIcon(QPixmap::fromImage(img));
    }
    else setWindowIcon(mod->icon());

    //- Get update period -
    req.clear()->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fper");
    if( !cntrIfCmd(req) ) mPeriod = atoi(req.text().c_str());

    //- Get project's flags -
    req.clear()->setName("get")->setAttr("path","/prj_"+src_prj+"/%2fobj%2fcfg%2frunWin");
    if( !cntrIfCmd(req) )
    {
	if( atoi(req.text().c_str())&0x01 )	setWindowState( Qt::WindowMaximized );
	else if( atoi(req.text().c_str())&0x02 )actFullScr->setChecked( true );
    }

    //- Get open pages list -
    req.clear()->setName("openlist")->setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg");
    if( !cntrIfCmd(req) )
	for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
	{
	    pgList.push_back(req.childGet(i_ch)->text());
	    callPage(req.childGet(i_ch)->text());
	}
    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //> Open direct-selected page
    if( !TSYS::pathLev(prj_it,1).empty() )
    {
	//- Convert project path to session path -
	string prj_el;
	string ses_it = "/ses_"+work_sess;
	int i_el = 1;
	while( (prj_el=TSYS::pathLev(prj_it,i_el++)).size() )
	    ses_it = ses_it+"/"+prj_el;

	//- Send open command -
	req.clear()->setName("open")->setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("pg",ses_it);
	cntrIfCmd(req);

	callPage(ses_it);
    }

    QCoreApplication::processEvents();

    //- Start timer -
    updateTimer->start(period());
}

void VisRun::fullUpdatePgs( )
{
    for( int i_p = 0; i_p < pgList.size(); i_p++ )
    {
	RunPageView *pg = master_pg->findOpenPage(pgList[i_p]);
	if( pg ) pg->update(true);
    }
}

void VisRun::callPage( const string& pg_it, bool updWdg )
{
    vector<int> idst;
    string stmp;

    //> Scan and update opened page
    if( master_pg )
    {
	RunPageView *pg = master_pg->findOpenPage(pg_it);
	if( pg && updWdg ) pg->update(false);
	if( pg ) return;
    }

    // Get group and parent page
    string pgGrp = wAttrGet(pg_it,"pgGrp");
    string pgSrc = wAttrGet(pg_it,"pgOpenSrc");

    //> Check for master page replace
    if( !master_pg || pgGrp == "main" || master_pg->pgGrp() == pgGrp )
    {
	//>> Send close command
	if( master_pg )
	{
	    XMLNode req("close"); req.setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg")->setAttr("pg",master_pg->id());
	    cntrIfCmd(req);
	}

	//>> Create widget view
	master_pg = new RunPageView(pg_it,this,centralWidget());
	master_pg->load("");
	master_pg->setFocusPolicy( Qt::StrongFocus );
	((QScrollArea *)centralWidget())->setWidget( master_pg );
	if( !(windowState()&(Qt::WindowFullScreen|Qt::WindowMaximized)) )
	{
	    QRect ws = QApplication::desktop()->availableGeometry(this);
	    resize( vmin(master_pg->size().width()+10,ws.width()-10), vmin(master_pg->size().height()+55,ws.height()-10) );
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

    for( int i_pg = 0; i_pg < cache_pg.size(); i_pg++ )
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
    for( int off = 0; true; woff = off )
    {
	string sel=TSYS::pathLev(wdg,0,true,&off);
	if( sel.empty() || sel.substr(0,4) == "wdg_" )	break;
    }
    RunPageView *rpg = findOpenPage(wdg.substr(0,woff));
    if( !rpg )	return NULL;
    if( woff >= wdg.size() )	return rpg;

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
    for( int i_w = 0; i_w < mod->mn_winds.size(); i_w++ )
	if( qobject_cast<VisRun*>(mod->mn_winds[i_w]) && ((VisRun*)mod->mn_winds[i_w])->srcProject( ) == srcProject( ) )
	{
	    if( ((VisRun*)mod->mn_winds[i_w])->workSess( ) != workSess( ) ) isMaster = false;
	    break;
	}

    //> Alarm types init
    //>> Set momo sound alarm
    if( isMaster && (ch_tp>>16)&TVision::Alarm )
    {
	const char *spkEvDev = "/dev/input/by-path/platform-pcspkr-event-spkr";
	int hd = open(spkEvDev,O_WRONLY);
	if( hd < 0 )	mess_warning(mod->nodePath().c_str(),"Error open: %s",spkEvDev);
	else
	{
	    input_event ev;
	    ev.time.tv_sec = time(NULL);
	    ev.type = EV_SND;
	    ev.code = SND_TONE;
	    ev.value = ((alarm>>16)&TVision::Alarm) ? 1000 : 0;
	    write(hd,&ev,sizeof(ev));
	    ::close(hd);
	}
    }
    //>> Set speach or sound alarm
    if( isMaster && (alarm>>16)&TVision::Sound && !alrmPlay->isRunning() && !alrmPlay->playData().empty() )
	alrmPlay->start( );

    //> Alarm action indicators update
    //>> Alarm level icon update
    if( ch_tp&0xFF || (alarm>>16)&(TVision::Light|TVision::Alarm|TVision::Sound) || !alrLevSet )
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
    if( winClose ) return;

#if OSC_DEBUG >= 3
    unsigned long long t_cnt = SYS->shrtCnt();
#endif

    //> Pages update
    XMLNode req("openlist");
    req.setAttr("tm",TSYS::uint2str(reqtm))->
	setAttr("path","/ses_"+work_sess+"/%2fserv%2fpg");

    if( !cntrIfCmd(req) )
    {
	//>> Check for delete pages
	RunPageView *pg;
	for( int i_p = 0; i_p < pgList.size(); i_p++ )
	{
	    int i_ch;
	    for( i_ch = 0; i_ch < req.childSize(); i_ch++ )
		if( pgList[i_p] == req.childGet(i_ch)->text() )
		    break;
	    if( i_ch < req.childSize() || !(pg=master_pg->findOpenPage(pgList[i_p])) ) continue;
	    if( !pg->property("cntPg").toString().isEmpty() )
		((RunWdgView*)TSYS::str2addr(pg->property("cntPg").toString().toAscii().data()))->setPgOpenSrc("");
	    else
	    {
		pg->deleteLater();
		if( pg == master_pg )	master_pg = NULL;
	    }
	}
	//>> Process opened pages
	pgList.clear();
	for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
	{
	    pgList.push_back(req.childGet(i_ch)->text());
	    callPage(req.childGet(i_ch)->text(),atoi(req.childGet(i_ch)->attr("updWdg").c_str()));
	}
    }
    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //> Alarms update (one seconds update)
    if( wPrcCnt%(500/period()) == 0 )
    {
	//>> Get alarm status
	unsigned wAlrmSt = alarmSt( );
	req.clear()->
	    setName("get")->
	    setAttr("mode","stat")->
	    setAttr("path","/ses_"+work_sess+"/%2fserv%2falarm");
	if( !cntrIfCmd(req) ) wAlrmSt = atoi(req.attr("alarmSt").c_str());

	//>> Get sound resources for play
	if( alarmTp(TVision::Sound,true) && !alrmPlay->isRunning() )
	{
	    req.clear()->
		setName("get")->
		setAttr("mode","sound")->
		setAttr("path","/ses_"+work_sess+"/%2fserv%2falarm")->
		setAttr("tm",TSYS::uint2str(alrmPlay->time()))->
		setAttr("wdg",alrmPlay->widget());
	    if( !cntrIfCmd(req) )
	    {
		alrmPlay->setTime( strtoul(req.attr("tm").c_str(),NULL,10) );
		alrmPlay->setWidget( req.attr("wdg") );
		alrmPlay->setData( TSYS::strDecode(req.text(),TSYS::base64) );
	    }
	}

	//>> Set alarm
	alarmSet( wAlrmSt );
    }

    //> Old pages from cache for close checking
    RunWdgView *pg = NULL;
    for( int i_pg = 0; i_pg < cache_pg.size(); i_pg++ )
	if( (period()*(reqTm()-cache_pg[i_pg]->reqTm())/1000) > 60*60 )
	{
	    delete cache_pg[i_pg];
	    cache_pg.erase(cache_pg.begin()+i_pg);
	    i_pg--;
	}

#if OSC_DEBUG >= 3
    upd_tm+=1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    if( !(1000/period() && wPrcCnt%(1000/period())) )
    {
	mess_debug("VCA DEBUG",_("Session '%s' update time %f ms."),workSess().c_str(),upd_tm);
	upd_tm = 0;
    }
#endif

    if( mWTime->isVisible() && !(wPrcCnt%vmax(1000/period(),1)) )
    {
	mWTime->setText(QTime::currentTime().toString("hh:mm:ss"));
	mWTime->setToolTip(QDate::currentDate().toString("dddd, dd-MMM-yyyy"));
    }

    wPrcCnt++;
}

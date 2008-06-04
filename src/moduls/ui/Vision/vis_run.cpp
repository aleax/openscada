
//OpenSCADA system module UI.Vision file: vis_run.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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
#include <QPushButton>
#include <QDialogButtonBox>
#include <QPainter>
#include <QToolBar>

#include <tsys.h>
#include "tvision.h"
#include "vis_run_widgs.h"
#include "vis_run.h"

using namespace VISION;

VisRun::VisRun( const string &prj_it, const string &open_user, const string &VCAstat, bool crSessForce ) :
    winClose(false), master_pg(NULL), m_period(1000), w_prc_cnt(0), reqtm(1), x_scale(1.0), y_scale(1.0), mAlrmSt(0xFFFFFF)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    mod->regWin( this );

    setWindowTitle(_("Vision runtime"));
    setWindowIcon(mod->icon());

    //- Create actions -
    //-- Generic actions --
    //--- Close ---
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close Vision window"));
    actClose->setWhatsThis(_("The button for closing Vision runtime window"));
    actClose->setStatusTip(_("Press for close of current Vision runtime window."));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //--- Quit ---
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit from OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA system."));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //-- View actions --
    //--- Fullscreen ---
    actFullScr = new QAction(_("Full screen"),this);
    actFullScr->setCheckable(true);
    actFullScr->setToolTip(_("Full screen toggle"));
    actFullScr->setWhatsThis(_("The button for full screen toggle"));
    actFullScr->setStatusTip(_("Press for toggle full screen."));
    connect(actFullScr, SIGNAL(toggled(bool)), this, SLOT(fullScreen(bool)));

    //-- Help actions --
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

    //-- Alarms actions --
    //--- Alarm level display button and full alarms quitance ---
    if(!ico_t.load(TUIS::icoPath("alarmLev").c_str())) ico_t.load(":/images/alarmLev.png");
    actAlrmLev = new QAction( QPixmap::fromImage(ico_t), _("Alarm level"), this );
    actAlrmLev->setObjectName("alarmLev");
    actAlrmLev->setToolTip(_("Alarm level"));
    actAlrmLev->setWhatsThis(_("The button for all alarms quitance"));
    actAlrmLev->setStatusTip(_("Press for all alarms quitance."));
    //--- Alarm by Light ---
    if(!ico_t.load(TUIS::icoPath("alarmLight").c_str())) ico_t.load(":/images/alarmLight.png");
    actAlrmLight = new QAction( QPixmap::fromImage(ico_t), _("Light alarm"), this );
    actAlrmLight->setObjectName("alarmLight");
    actAlrmLight->setToolTip(_("Light alarm"));
    actAlrmLight->setWhatsThis(_("The button for all light alarms quitance"));
    actAlrmLight->setStatusTip(_("Press for all light alarms quitance."));
    actAlrmLight->setVisible(false);
    //--- Alarm by mono sound (PC speaker) ---
    if(!ico_t.load(TUIS::icoPath("alarmAlarm").c_str())) ico_t.load(":/images/alarmAlarm.png");
    actAlrmAlarm = new QAction( QPixmap::fromImage(ico_t), _("Speaker alarm"), this );
    actAlrmAlarm->setObjectName("alarmAlarm");
    actAlrmAlarm->setToolTip(_("PC speaker alarm"));
    actAlrmAlarm->setWhatsThis(_("The button for all PC speaker alarms quitance"));
    actAlrmAlarm->setStatusTip(_("Press for all PC speaker alarms quitance."));
    actAlrmAlarm->setVisible(false);
    //--- Alarm by sound or synthesis of speech ---
    if(!ico_t.load(TUIS::icoPath("alarmSound").c_str())) ico_t.load(":/images/alarmSound.png");
    actAlrmSound = new QAction( QPixmap::fromImage(ico_t), _("Sound/speech alarm"), this );
    actAlrmSound->setObjectName("alarmSound");
    actAlrmSound->setToolTip(_("Sound or speech alarm"));
    actAlrmSound->setWhatsThis(_("The button for all sound or speech alarms quitance"));
    actAlrmSound->setStatusTip(_("Press for all sound or speech alarms quitance."));
    actAlrmSound->setVisible(false);

    //- Create menu -
    mn_file = menuBar()->addMenu(_("&File"));
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
    mn_help->addAction(actWhatIs);

    //- Init tool bars -
    //-- Alarms tools bar --
    toolBarAlarm = new QToolBar(_("Alarms (status)"),this);
    connect( toolBarAlarm, SIGNAL(actionTriggered(QAction*)), this, SLOT(alarmAct(QAction*)) );
    toolBarAlarm->setIconSize(QSize(16,16));
    toolBarAlarm->addAction(actAlrmLev);
    toolBarAlarm->addAction(actAlrmLight);
    toolBarAlarm->addAction(actAlrmAlarm);
    toolBarAlarm->addAction(actAlrmSound);

    //- Init status bar -
    w_user = new UserStBar(open_user.c_str(), this);
    w_user->setWhatsThis(_("This label display curent user."));
    w_user->setToolTip(_("Field for display of the current user."));
    w_user->setStatusTip(_("Double click for change user."));
    statusBar()->insertPermanentWidget(0,w_user);
    w_stat = new QLabel(VCAstat.c_str(), this);
    w_stat->setWhatsThis(_("This label display used VCA engine station."));
    w_stat->setToolTip(_("Field for display of the used VCA engine station."));
    statusBar()->insertPermanentWidget(0,w_stat);
    statusBar()->insertPermanentWidget(0,toolBarAlarm);

    //- Init scroller -
    QScrollArea *scrl = new QScrollArea;
    scrl->setFocusPolicy( Qt::NoFocus );
    setCentralWidget( scrl );

    //- Create timers -
    //-- End run timer --
    endRunTimer   = new QTimer( this );
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);
    //-- Update timer --
    updateTimer = new QTimer( this );
    updateTimer->setSingleShot(false);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePage()));

    alrmPlay = new SndPlay(this);

    resize( 600, 400 );

    setVCAStation(VCAstat);

    //- Init sesion -
    initSess(prj_it,crSessForce);

    //w_stat->setText(host.st_nm.c_str());
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

    //- Disconnect/delete session -
    XMLNode req("disconnect");
    req.setAttr("path","/%2fserv%2f0")->setAttr("sess",work_sess);
    cntrIfCmd(req);

    //- Unregister window -
    mod->unregWin(this);

    //- Clear cache -
    while( !cache_pg.empty() )
    {
	delete cache_pg.front();
	cache_pg.pop_front();
    }
}

string VisRun::user()
{
    return w_user->user().toAscii().data();
}

void VisRun::setVCAStation( const string& st )
{
    m_stat = st.empty() ? "." : st;
}

int VisRun::cntrIfCmd( XMLNode &node, bool glob )
{
    if( VCAStation().empty() || VCAStation() == "." ) node.setAttr("user",user());
    return mod->cntrIfCmd(node,user(),VCAStation(),glob);
}

void VisRun::closeEvent( QCloseEvent* ce )
{
    winClose = true;
    ce->accept();
}

void VisRun::resizeEvent( QResizeEvent *ev )
{
    if( !ev->oldSize().isEmpty() && master_pg )
    {
	float x_scale_old = x_scale;
	float y_scale_old = y_scale;
	if( windowState() == Qt::WindowMaximized || windowState() == Qt::WindowFullScreen )
	{
	    x_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().width()/(float)master_pg->size().width();
	    y_scale *= (float)((QScrollArea*)centralWidget())->maximumViewportSize().height()/(float)master_pg->size().height();
	}else x_scale = y_scale = 1.0;
	if( x_scale_old != x_scale || y_scale_old != y_scale )	master_pg->update(0,0);
    }
}

void VisRun::endRunChk( )
{
    if( mod->endRun() ) close();
}

void VisRun::quitSt()
{
    SYS->stop();
}

void VisRun::about()
{
    QMessageBox::about(this,windowTitle(), 
		QString(_("%1 v%2.\nAutor: %3\nLicense: %4\n")).
			arg(mod->modInfo("Name").c_str()).
			arg(mod->modInfo("Version").c_str()).
			arg(mod->modInfo("Author").c_str()).
			arg(mod->modInfo("License").c_str()));
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

void VisRun::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

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
    req.setAttr("path","/%2fserv%2f0")->setAttr("prj",src_prj);
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

	QLabel *inp_lab = new QLabel(QString(_("Already opened several sessions on page %1. You may create new "
				       "session or connect to present session. Please, select need session "
				       "and press 'Connect' or press 'Create' for creation new, individual, "
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
	if( conreq.exec() == QDialog::Accepted && ls_wdg->currentItem() )
	    work_sess = ls_wdg->currentItem()->text().toAscii().data();
    }

    req.clear()->setName("connect")->setAttr("path","/%2fserv%2f0");
    if( work_sess.empty() ) req.setAttr("prj",src_prj);
    else req.setAttr("sess",work_sess);
    if( cntrIfCmd(req) )
    {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	return;
    }

    work_sess = req.attr("sess");
    //-- Set window title --
    setWindowTitle(QString(_("Runing project: %1")).arg(src_prj.c_str()));
    //-- Set project's icon to window --
    req.clear()->setName("get")->setAttr("path","/ses_"+work_sess+"/%2fico");
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
    if( !cntrIfCmd(req) ) m_period = atoi(req.text().c_str());

    //- Get open pages list -
    req.clear()->setName("openlist")->setAttr("path","/ses_"+work_sess+"/%2fserv%2f0");
    if( !cntrIfCmd(req) )
	for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
	    callPage(req.childGet(i_ch)->text());

    //- Open direct-selected page -
    if( !TSYS::pathLev(prj_it,1).empty() )
    {
	//- Convert project path to session path -
	string prj_el;
	string ses_it = "/ses_"+work_sess;
	int i_el = 1;
	while( (prj_el=TSYS::pathLev(prj_it,i_el++)).size() )
	    ses_it = ses_it+"/"+prj_el;

	//- Send open command -
	wAttrSet(ses_it,"pgOpen","1");

	callPage(ses_it);
    }

    //- Start timer -
    updateTimer->start(period());
}

void VisRun::callPage( const string& pg_it, XMLNode *upw )
{
    vector<int> idst;
    string pgGrp, pgSrc, stmp;

    //- Scan and update opened page -
    if( master_pg )
    {
	RunPageView *pg = master_pg->findOpenPage(pg_it);
	if( pg && upw )
	    for( int i_p = 0, off = 0; i_p < upw->childSize(); i_p++, off = 0 )
	    {
		stmp = upw->childGet(i_p)->text();
		TSYS::pathLev(stmp,0,true,&off);
		pg->update(1,0,stmp.substr(off));
	    }
	if( pg ) return;
    }

    //- Get group and parent page -
    pgGrp = wAttrGet(pg_it,"pgGrp");
    pgSrc = wAttrGet(pg_it,"pgOpenSrc");

    //- Check for master page replace -
    if( !master_pg || pgGrp == "main" || master_pg->pgGrp() == pgGrp )
    {
	//-- Send close command --
	if( master_pg ) wAttrSet(master_pg->id(),"pgOpen","0");

	//-- Create widget view --
	master_pg = new RunPageView(pg_it,this,centralWidget());
	master_pg->load("");
	master_pg->setFocusPolicy( Qt::StrongFocus );
	((QScrollArea *)centralWidget())->setWidget( master_pg );
	if( master_pg->dc().value("pgFullScr",false).toBool() )
	    actFullScr->setChecked(true);
	else
	{
	    actFullScr->setChecked(false);
	    resize( master_pg->size().width()+10, master_pg->size().height()+55 );
	}
    }
    //- Put to check for include -
    else master_pg->callPage(pg_it,pgGrp,pgSrc);

    //- Update widgets of now opened page -
    if( upw && master_pg )
    {
	RunPageView *pg = master_pg->findOpenPage(pg_it);
	if( !pg || pg->reqTm() == reqtm ) return;
	XMLNode req("openlist");
	req.setAttr("tm",TSYS::uint2str(pg->reqTm()))->setAttr("path","/ses_"+work_sess+"/%2fserv%2f0");
	if( !cntrIfCmd(req) )
	    for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
		if( req.childGet(i_ch)->text() == pg_it )
		{
		    for( int i_p = 0, off = 0; i_p < req.childGet(i_ch)->childSize(); i_p++, off = 0 )
		    {
			stmp = req.childGet(i_ch)->childGet(i_p)->text();
			TSYS::pathLev(stmp,0,true,&off);
			pg->update(1,0,stmp.substr(off));
		    }
		    break;
		}
    }
}

void VisRun::pgCacheAdd( RunWdgView *wdg )
{
    if( !wdg ) return;
    cache_pg.push_front(wdg);
    while( cache_pg.size() > 100 )
    {
	delete cache_pg.back();
	cache_pg.pop_back();
    }
}

RunWdgView *VisRun::pgCacheGet( const string &id )
{
    RunWdgView *pg = NULL;

    for( int i_pg = 0; i_pg < cache_pg.size(); i_pg++ )
	if( cache_pg[i_pg]->id() == id )
	{
	    pg = cache_pg[i_pg];
	    cache_pg.erase(cache_pg.begin()+i_pg);
	    break;
	}

    return pg;
}

string VisRun::wAttrGet( const string &path, const string &attr )
{
    XMLNode req("get");
    req.setAttr("path",path+"/%2fattr%2f"+attr);
    if( !cntrIfCmd(req) ) return req.text();
    return "";
}

bool VisRun::wAttrSet( const string &path, const string &attr, const string &val )
{
    //- Send value to model -
    XMLNode req("set");
    req.setAttr("path",path+"/%2fserv%2f0");
    req.childAdd("el")->setAttr("id",attr)->setText(val);
    return !cntrIfCmd(req);
}

void VisRun::alarmSet( unsigned alarm )
{
    //printf("TEST 00: Set alarm: %xh (%d)\n",alarm,res.size());
    unsigned ch_tp = alarm^mAlrmSt;

    //- Alarm types init -
    //-- Set momo sound alarm --
    if( (ch_tp>>16)&TVision::Alarm )
    {
	char *spkEvDev = "/dev/input/by-path/platform-pcspkr-event-spkr";
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
    //-- Set speach or sound alarm --
    if( (alarm>>16)&TVision::Sound && !alrmPlay->isRunning() && !alrmPlay->playData().empty() )
	alrmPlay->start( );

    //- Alarm action indicators update
    //-- Alarm level icon update --
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
    //- Alarm buttons status process -
    for( int i_b = 0; i_b < 3; i_b++ )
    {
	QAction *actAlrm = (i_b==0) ? actAlrmLight : ((i_b==1) ? actAlrmAlarm : actAlrmSound);
	if( (ch_tp>>8)&(0x01<<i_b) )	actAlrm->setVisible((alarm>>8)&(0x01<<i_b));
	if( (ch_tp>>16)&(0x01<<i_b) )	actAlrm->setEnabled((alarm>>16)&(0x01<<i_b));
    }

    mAlrmSt = alarm;
}

void VisRun::updatePage( )
{
    if( winClose ) return;

    //unsigned long long t_cnt = SYS->shrtCnt();

    //- Pages update -
    XMLNode req("openlist");
    req.setAttr("tm",TSYS::uint2str(reqtm))->
	setAttr("path","/ses_"+work_sess+"/%2fserv%2f0");
    if( !cntrIfCmd(req) )
	for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
	    callPage(req.childGet(i_ch)->text(),req.childGet(i_ch));
    reqtm = strtoul(req.attr("tm").c_str(),NULL,10);

    //- Alarms update (one seconds update) -
    if( w_prc_cnt%(500/period()) == 0 )
    {
	//-- Get alarm status --
	unsigned wAlrmSt = alarmSt( );
	req.clear()->
	    setName("get")->
	    setAttr("mode","stat")->
	    setAttr("path","/ses_"+work_sess+"/%2fserv%2falarm");
	if( !cntrIfCmd(req) ) wAlrmSt = atoi(req.attr("alarmSt").c_str());

	//-- Get sound resources for play --
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

	//-- Set alarm --
	alarmSet( wAlrmSt );
    }

    /*upd_tm+=1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    if( !(w_prc_cnt%10) )
    {
	printf("TEST 01: Pages update time: %f\n",upd_tm);
	upd_tm = 0;
    }*/

    w_prc_cnt++;
}

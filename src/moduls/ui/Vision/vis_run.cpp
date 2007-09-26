
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

#include <tsys.h>
#include "tvision.h"
#include "vis_run_widgs.h"
#include "vis_run.h"

using namespace VISION;

VisRun::VisRun( const string &prj_it, const string &open_user, const string &VCAstat, bool crSessForce ) : 
    winClose(false), master_pg(NULL), m_period(1000), w_prc_cnt(0), proc_st(false)
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
    
    //- Create menu -
    mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);
    mn_view = menuBar()->addMenu(_("&View"));
    mn_view->addAction(actFullScr);
    mn_help = menuBar()->addMenu(_("&Help"));
    mn_help->addAction(actAbout);
    mn_help->addAction(actQtAbout);
    mn_help->addSeparator();
    mn_help->addAction(actWhatIs);

    //- Init tool bars -
    //????
    
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

    resize( 600, 400 );

    setVCAStation(VCAstat);
    
    //- Init sesion -
    initSess(prj_it,crSessForce);
    
    //w_stat->setText(host.st_nm.c_str());
    statusBar()->showMessage(_("Ready"), 2000 );    
}

VisRun::~VisRun()
{
    winClose = true;
    endRunTimer->stop();
    updateTimer->stop();
    while(proc_st);
    
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
	conreq.setWindowTitle(_("Connection to session select"));
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
			
	QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|
	                                                 QDialogButtonBox::Cancel,Qt::Horizontal,&conreq);
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

void VisRun::callPage( const string& pg_it )
{
    vector<int> idst;
    string pgGrp, pgSrc;

    //- Scan opened pages -
    if( master_pg && master_pg->findOpenPage(pg_it) ) return;
    
    //- Get group and parent page -
    pgGrp = wAttrGet(pg_it,"pgGrp");
    pgSrc = wAttrGet(pg_it,"pgOpenSrc");
    
    //- First master page creation -    
    if( !master_pg )
    {
	QScrollArea *scrl = new QScrollArea;
	scrl->setWidgetResizable(true);
	scrl->setFocusPolicy( Qt::NoFocus );    

	//- Create widget view -
	master_pg = new RunPageView(pg_it,this,this);
	master_pg->load("");
	master_pg->setFocusPolicy( Qt::StrongFocus );
	scrl->setWidget( master_pg );
	setCentralWidget( scrl );
	actFullScr->setChecked(master_pg->dc().value("pgFullScr",false).toBool());
	//if( master_pg->dc().value("pgFullScr",false).toBool() )	setWindowState(Qt::WindowFullScreen);
	//else	setWindowState(Qt::WindowNoState);
	return;
    }
    else
    {
	//- Check for master page replace -
	if( pgGrp == "main" || master_pg->pgGrp() == pgGrp )
	{
	    //-- Send close command --
	    wAttrSet(master_pg->id(),"pgOpen","0");
	    
	    //-- Create widget view --
	    master_pg = new RunPageView(pg_it,this,this);
	    master_pg->load("");
	    master_pg->setFocusPolicy( Qt::StrongFocus );
	    ((QScrollArea *)centralWidget())->setWidget( master_pg );
	    return;
	}
	//- Put to check for include -
	master_pg->callPage(pg_it,pgGrp,pgSrc);
    }
    
    //adjustSize();
    //setGeometry(0,0,master_pg->width(),master_pg->height());
}

void VisRun::pgCacheAdd( RunWdgView *wdg )
{
    if( !wdg )	return;
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

void VisRun::updatePage( )
{
    if( winClose ) return;
    proc_st = true;

    XMLNode req("openlist");
    req.setAttr("path","/ses_"+work_sess+"/%2fserv%2f0");
    if( !cntrIfCmd(req) )
        for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
	    callPage(req.childGet(i_ch)->text());

    //unsigned long long t_cnt = SYS->shrtCnt();

    //- Update opened pages -
    if( master_pg ) master_pg->update(w_prc_cnt,1000/period());

    /*upd_tm+=1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    if( !(w_prc_cnt%10) )
    {
	printf("TEST 01: Pages update time: %f\n",upd_tm);
	upd_tm = 0;
    }*/

    w_prc_cnt++;
    
    proc_st = false;
}

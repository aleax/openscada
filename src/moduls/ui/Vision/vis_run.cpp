
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

#include <tsys.h>
#include "tvision.h"
#include "vis_run_widgs.h"
#include "vis_run.h"

using namespace VISION;

VisRun::VisRun( const string &prj_it, string open_user ) : winClose(false), master_pg(NULL), 
    m_period(0), w_prc_cnt(0), proc_st(false)
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
    statusBar()->showMessage(_("Ready"), 2000 );

    //- Create timers -
    updateTimer = new QTimer( this );
    updateTimer->setSingleShot(false);    
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePage()));

    resize( 600, 400 );
    
    //- Init sesion -
    initSess(prj_it);
}

VisRun::~VisRun()
{
    winClose = true;
    updateTimer->stop();
    while(proc_st);
    
    //- Delete session -
    XMLNode del_req("del");
    del_req.setAttr("user",user())->setAttr("path","/%2fses%2fses")->setText(work_sess);
    mod->cntrIfCmd(del_req);

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

void VisRun::closeEvent( QCloseEvent* ce )
{
    winClose = true;
    ce->accept();
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

void VisRun::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void VisRun::initSess( const string &prj_it )
{
    //printf("TEST 10: %s\n",prj_it.c_str());

    //- Create session -
    string prj_nm = TSYS::pathLev(prj_it,0);
    if( prj_nm.empty() ) return;
    prj_nm = prj_nm.substr(4);
    
    XMLNode add_req("add");
    XMLNode get_req("get");
    XMLNode set_req("set");
    
    add_req.setAttr("user",user())->setAttr("path","/%2fses%2fses")->setText(prj_nm);
    if( mod->cntrIfCmd(add_req) )
    {
	mod->postMess(add_req.attr("mcat").c_str(),add_req.text().c_str(),TVision::Error,this);
	return;
    }
    //- Init session -
    set_req.setAttr("user",user())->setAttr("path","/ses_"+prj_nm+"/%2fobj%2fst%2fprj")->setText(prj_nm);
    mod->cntrIfCmd(set_req);
    set_req.setAttr("path","/ses_"+prj_nm+"/%2fobj%2fst%2fstart")->setText("1");
    if( mod->cntrIfCmd(set_req) )
    {
	mod->postMess(set_req.attr("mcat").c_str(),set_req.text().c_str(),TVision::Error,this);
	return;
    }
    work_sess = src_page = prj_nm;
    //-- Set window title --
    setWindowTitle(QString(_("Runing project: %1")).arg(prj_nm.c_str()));
    //-- Set project's icon --
    get_req.setAttr("user",user())->setAttr("path","/ses_"+prj_nm+"/%2fico");
    if( !mod->cntrIfCmd(get_req) )
    {
	QImage img;
	string simg = TSYS::strDecode(get_req.text(),TSYS::base64);
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
	    setWindowIcon(QPixmap::fromImage(img));
    }
    else setWindowIcon(mod->icon());

    //- Get open pages list -
    get_req.setAttr("user",user())->setAttr("path","/ses_"+prj_nm+"/%2fobj%2fcfg%2fopenPg");
    if( !mod->cntrIfCmd(get_req) )
	for( int i_ch = 0; i_ch < get_req.childSize(); i_ch++ )
	    callPage(get_req.childGet(i_ch)->text());

    //- Open direct-selected page -
    if( !TSYS::pathLev(prj_it,1).empty() )
    {
	//- Convert project path to session path -    
	string prj_el;	    
	string ses_it = "/ses_"+prj_nm;        
	int i_el = 1;
	while( (prj_el=TSYS::pathLev(prj_it,i_el++)).size() )
	    ses_it = ses_it+"/"+prj_el;
	    
	//- Send open command -
	wAttrSet(ses_it,"pgOpen","1");
	    
	callPage(ses_it);
    }
    
    //- Start timer -    
    get_req.setAttr("user",user())->setAttr("path","/ses_"+prj_nm+"/%2fobj%2fcfg%2fper");
    if( !mod->cntrIfCmd(get_req) ) 
    {
	m_period = atoi(get_req.text().c_str());
	updateTimer->start(m_period);
    }
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
    XMLNode get_req("get");
    get_req.setAttr("user",user())->
	    setAttr("path",path+"/%2fattr%2f"+attr);
    if( !mod->cntrIfCmd(get_req) ) return get_req.text();
    return "";
}

bool VisRun::wAttrSet( const string &path, const string &attr, const string &val )
{
    //- Send value to model -
    XMLNode set_req("set");
    set_req.setAttr("user",user())->
    setAttr("path",path+"/%2fattr%2fscmd");
    set_req.childAdd("el")->setAttr("id",attr)->setText(val);
    return !mod->cntrIfCmd(set_req);
}

void VisRun::updatePage( )
{
    if( winClose ) return;
    proc_st = true;

    XMLNode get_req("get");    
    get_req.setAttr("user",user())->setAttr("path","/ses_"+work_sess+"/%2fobj%2fcfg%2fopenPg");
    if( !mod->cntrIfCmd(get_req) )
        for( int i_ch = 0; i_ch < get_req.childSize(); i_ch++ )
	    callPage(get_req.childGet(i_ch)->text());
    //- Update opened pages -
    if( master_pg ) master_pg->update(w_prc_cnt,1000/period());
    
    w_prc_cnt++;
    
    proc_st = false;
}
